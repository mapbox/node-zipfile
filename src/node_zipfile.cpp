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
    NODE_SET_PROTOTYPE_METHOD(constructor, "readFileSync", readFileSync);
    NODE_SET_PROTOTYPE_METHOD(constructor, "readFile", readFile);

    // properties
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("count"), get_prop);
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("names"), get_prop);

    target->Set(String::NewSymbol("ZipFile"),constructor->GetFunction());
}

ZipFile::ZipFile(std::string const& file_name) :
  ObjectWrap(),
  file_name_(file_name),
  archive_(),
  names_() {}

ZipFile::~ZipFile() {
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

Handle<Value> ZipFile::readFileSync(const Arguments& args)
{
    HandleScope scope;

    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(Exception::TypeError(
          String::New("first argument must be a file name inside the zip")));

    std::string name = TOSTR(args[0]);
  
    // TODO - enforce valid index
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());

    struct zip_file *zf_ptr;

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

    if ((zf_ptr=zip_fopen_index(zf->archive_, idx, 0)) == NULL) {
        zip_fclose(zf_ptr);
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << name << ": archive error: " << zip_strerror(zf->archive_) << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    struct zip_stat st;
    zip_stat_index(zf->archive_, idx, 0, &st);
  
    std::vector<unsigned char> data;
    data.clear();
    data.resize( st.size );
    
    int result =  0;
    result = (int)zip_fread( zf_ptr, reinterpret_cast<void*> (&data[0]), data.size() );

    if (result < 0) {
        zip_fclose(zf_ptr);
        std::stringstream s;
        s << "error reading file #" << idx << " in " << name << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    #if NODE_VERSION_AT_LEAST(0,3,0)
        node::Buffer *retbuf = Buffer::New((char *)&data[0],data.size());
    #else
        node::Buffer *retbuf = Buffer::New(data.size());
        std::memcpy(retbuf->data(), (char *)&data[0], data.size());
    #endif
    
    zip_fclose(zf_ptr);
    return scope.Close(retbuf->handle_);
}

typedef struct {
    ZipFile* zf;
    struct zip *za;
    std::string name;
    bool error;
    std::string error_name;
    std::vector<unsigned char> data;
    Persistent<Function> cb;
} closure_t;


Handle<Value> ZipFile::readFile(const Arguments& args)
{
    HandleScope scope;

    if (args.Length() < 2)
        return ThrowException(Exception::TypeError(
          String::New("requires two arguments, the name of a file and a callback")));
    
    // first arg must be name
    if(!args[0]->IsString())
        return ThrowException(Exception::TypeError(
          String::New("first argument must be a file name inside the zip")));
    
    // last arg must be function callback
    if (!args[args.Length()-1]->IsFunction())
        return ThrowException(Exception::TypeError(
                  String::New("last argument must be a callback function")));
  
    std::string name = TOSTR(args[0]);
  
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());

    closure_t *closure = new closure_t();

    // libzip is not threadsafe so we cannot use the zf->archive_
    // instead we open a new zip archive for each thread
    struct zip *za;
    int err;
    char errstr[1024];
    if ((za=zip_open(zf->file_name_.c_str() , 0, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << zf->file_name_ << " error: " << errstr << "\n";
        zip_close(za);
        return ThrowException(Exception::Error(
            String::New(s.str().c_str())));
    }

    closure->zf = zf;
    closure->za = za;
    closure->error = false;
    closure->name = name;
    closure->cb = Persistent<Function>::New(Handle<Function>::Cast(args[args.Length()-1]));
    eio_custom(EIO_ReadFile, EIO_PRI_DEFAULT, EIO_AfterReadFile, closure);
    ev_ref(EV_DEFAULT_UC);
    zf->Ref();
    return Undefined();
}


int ZipFile::EIO_ReadFile(eio_req *req)
{
    closure_t *closure = static_cast<closure_t *>(req->data);

    struct zip_file *zf_ptr=NULL;

    int idx = -1;
    
    std::vector<std::string>::iterator it = std::find(closure->zf->names_.begin(),
                                            closure->zf->names_.end(), 
                                            closure->name);
    if (it!=closure->zf->names_.end()) {
        idx = distance(closure->zf->names_.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << closure->name << "\n";
        closure->error = true;
        closure->error_name = s.str();

    } else {

        if ((zf_ptr = zip_fopen_index(closure->za, idx, 0)) == NULL) {
            std::stringstream s;
            s << "cannot open file #" << idx << " in "
              << closure->name << ": archive error: " << zip_strerror(closure->za) << "\n";
            closure->error = true;
            closure->error_name = s.str();

        } else {
        
            struct zip_stat st;
            zip_stat_index(closure->za, idx, 0, &st);
            closure->data.clear();
            closure->data.resize( st.size );
            
            int result =  0;
            result = (int)zip_fread( zf_ptr, reinterpret_cast<void*> (&closure->data[0]), closure->data.size() );
        
            if (result < 0) {
                std::stringstream s;
                s << "error reading file #" << idx << " in " 
                  << closure->name << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
                closure->error = true;
                closure->error_name = s.str();
            }
        }
    }

    zip_fclose(zf_ptr);
    return 0;
}

int ZipFile::EIO_AfterReadFile(eio_req *req)
{
    HandleScope scope;

    closure_t *closure = static_cast<closure_t *>(req->data);
    ev_unref(EV_DEFAULT_UC);

    TryCatch try_catch;
  
    if (closure->error) {
        Local<Value> argv[1] = { Exception::Error(String::New(closure->error_name.c_str())) };
        closure->cb->Call(Context::GetCurrent()->Global(), 1, argv);
    } else {
        #if NODE_VERSION_AT_LEAST(0,3,0)
          node::Buffer *retbuf = Buffer::New((char *)&closure->data[0],closure->data.size());
        #else
          node::Buffer *retbuf = Buffer::New(closure->data.size());
          std::memcpy(retbuf->data(), (char *)&closure->data[0], closure->data.size());
        #endif
        Local<Value> argv[2] = { Local<Value>::New(Null()), Local<Value>::New(retbuf->handle_) };
        closure->cb->Call(Context::GetCurrent()->Global(), 2, argv);
    }

    if (try_catch.HasCaught()) {
      FatalException(try_catch);
      //try_catch.ReThrow();
    }
    
    closure->zf->Unref();
    closure->cb.Dispose();
    delete closure;
    return 0;
}

