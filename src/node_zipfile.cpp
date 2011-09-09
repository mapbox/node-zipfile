#include "node_zipfile.hpp"

// stl
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>
//#include <iostream>

#include <node_buffer.h>
#include <node_version.h>


#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

Persistent<FunctionTemplate> ZipFile::constructor;

void ZipFile::Initialize(Handle<Object> target) {

    HandleScope scope;
  
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ZipFile::New));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(String::NewSymbol("ZipFile"));

    // functions
    //NODE_SET_PROTOTYPE_METHOD(constructor, "readFileSync", readFileSync);
    NODE_SET_METHOD(target, "open", Open);
    NODE_SET_METHOD(target, "read", Read);
    NODE_SET_METHOD(target, "close", Close);

    // properties
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("count"), get_prop);
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("names"), get_prop);

    target->Set(String::NewSymbol("ZipFile"),constructor->GetFunction());
}

ZipFile::ZipFile(std::string const& file_name) :
  ObjectWrap(),
  file_name_(file_name),
  archive_(),
  zip_file_(NULL),
  zip_source_(NULL),
  names_() {}

ZipFile::~ZipFile() {
    if (zip_file_ != NULL) zip_fclose(zip_file_);
    zip_close(archive_);
}

Handle<Value> ZipFile::New(const Arguments& args)
{
    HandleScope scope;

    if (!args.IsConstructCall())
        return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(Exception::TypeError(
          String::New("first argument must be a path to a zipfile")));

    std::string input_file = TOSTR(args[0]);
    struct zip *za;
    int err;
    char errstr[1024];
    if ((za=zip_open(input_file.c_str(), 0, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << input_file << " error: " << errstr << "\n";
        return ThrowException(Exception::Error(
            String::New(s.str().c_str())));
    }

    ZipFile* zf = new ZipFile(input_file);

    int num = zip_get_num_files(za);
    zf->names_.reserve(num);
    int i = 0;
    for (i=0; i<num; i++) {
        struct zip_stat st;
        zip_stat_index(za, i, 0, &st);
        zf->names_.push_back(st.name);
    }

    zf->archive_ = za;
    zf->Wrap(args.This());
    return args.This();

}

Handle<Value> ZipFile::get_prop(Local<String> property,
                         const AccessorInfo& info)
{
    HandleScope scope;
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(info.This());
    std::string a = TOSTR(property);
    if (a == "count") {
          return scope.Close(Integer::New(zf->names_.size()));
    }
    if (a == "names") {
          unsigned num = zf->names_.size();
          Local<Array> a = Array::New(num);
          for (unsigned i = 0; i < num; ++i )
          {
              a->Set(i,String::New(zf->names_[i].c_str()));
          }
          
          return scope.Close(a);
    }
    return Undefined();    
}

Handle<Value> ZipFile::Open(const Arguments& args)
{
    HandleScope scope;

    if (args.Length() != 2 || !args[1]->IsString())
        return ThrowException(Exception::TypeError(
          String::New("Argument must be a file name inside the zip")));

    std::string name = TOSTR(args[1]);
  
    // TODO - enforce valid index
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (zf->zip_file_ != NULL || zf->zip_source_ != NULL)
      return ThrowException(Exception::Error(String::New("Zipfile already in use..")));

    int idx = -1;
    
    std::vector<std::string>::iterator it = std::find(zf->names_.begin(), zf->names_.end(), name);
    if (it!=zf->names_.end()) {
        idx = distance(zf->names_.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << name << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    if ((zf->zip_file_=zip_fopen_index(zf->archive_, idx, 0)) == NULL) {
        zip_fclose(zf->zip_file_);
        zf->zip_file_ = NULL;
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << name << ": archive error: " << zip_strerror(zf->archive_) << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    return Undefined();
}

Handle<Value> ZipFile::Close(const Arguments& args)
{
    HandleScope scope;

    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (zf->zip_file_ == NULL)
      return ThrowException(Exception::Error(String::New("No file opened!")));

    if (zf->zip_source_ != NULL)
      return ThrowException(Exception::Error(String::New("Zipfile already in use..")));

    if (zip_fclose(zf->zip_file_) != 0) {
        std::stringstream s;
        s << "Error while closing zip file: " << zip_file_strerror(zf->zip_file_) << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    zf->zip_file_ = NULL;

    return Undefined();
}

typedef struct {
    ZipFile *zf;
    zip_file *file;
    zip_int64_t read;
    zip_uint64_t len;
    char *data;
    Persistent<Function> cb;
} read_closure_t;

/* bytesRead = zipfile.read(buffer, pos, len, cb) */
Handle<Value> ZipFile::Read(const Arguments& args)
{
    HandleScope scope;

    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (!Buffer::HasInstance(args[1])) {
      return ThrowException(Exception::Error(
                  String::New("Second argument needs to be a buffer")));
    }
    Local<Object> buffer_obj = args[1]->ToObject();
    char *buffer_data = Buffer::Data(buffer_obj);
    size_t buffer_length = Buffer::Length(buffer_obj);

    size_t off = args[2]->Int32Value();
    if (off >= buffer_length) {
      return ThrowException(Exception::Error(
            String::New("Offset is out of bounds")));
    }

    size_t len = args[3]->Int32Value();
    if (off + len > buffer_length) {
      return ThrowException(Exception::Error(
            String::New("Length is extends beyond buffer")));
    }

    Local<Value> cb = args[4];
    if (!cb->IsFunction())
      return ThrowException(Exception::Error(
            String::New("Fourth argument should be a callback function.")));

    read_closure_t *closure = new read_closure_t();
    closure->zf = zf;
    closure->file = zf->zip_file_;
    closure->read = 0;
    closure->data = buffer_data+off;
    closure->len = len;
    closure->cb = Persistent<Function>::New(Handle<Function>::Cast(args[4]));

    eio_custom(EIO_Read, EIO_PRI_DEFAULT, EIO_AfterRead, closure);
    zf->Ref();
    ev_ref(EV_DEFAULT_UC);

    return Undefined();
}

int ZipFile::EIO_Read(eio_req *req)
{
    read_closure_t *closure = static_cast<read_closure_t *>(req->data);

    closure->read = zip_fread(closure->file, closure->data, closure->len);

    return 0;
}

int ZipFile::EIO_AfterRead(eio_req *req)
{
    HandleScope scope;

    read_closure_t *closure = static_cast<read_closure_t *>(req->data);
    ev_unref(EV_DEFAULT_UC);

    Handle<Value> argv[2];

    if (closure->read < 0) {
      std::stringstream s;
      s << "Error while reading zip file: " << zip_file_strerror(closure->file) << "\n";
      argv[0] = Exception::Error(String::New(s.str().c_str()));
      argv[1] = Undefined();
    } else {
      argv[0] = Undefined();
      argv[1] = Integer::New(closure->read);
    }

    closure->cb->Call(Context::GetCurrent()->Global(), 2, argv);

    closure->zf->Unref();
    closure->cb.Dispose();
    delete closure;
    return 0;
}
