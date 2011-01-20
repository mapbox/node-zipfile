#include "node_zipfile.hpp"

#include <sstream>
#include <vector>

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

    // properties
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("count"), get_prop);
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("names"), get_prop);

    target->Set(String::NewSymbol("ZipFile"),constructor->GetFunction());
}

ZipFile::ZipFile(std::string const& file_name) :
  ObjectWrap(),
  file_name_(file_name),
  archive_() {}

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
  	    s << "cannot open file" << input_file << " error: " << errstr << "\n";
        return ThrowException(Exception::Error(
          String::New(s.str().c_str())));
  	}

    ZipFile* zf = new ZipFile(input_file);
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
          int num = zip_get_num_files(zf->archive_);
          return scope.Close(Integer::New(num));
    }
    if (a == "names") {
          int num = zip_get_num_files(zf->archive_);
          Local<Array> a = Array::New(num);
          int i = 0;
          for (i=0; i<num; i++) {
              struct zip_stat st;
              zip_stat_index(zf->archive_, i, 0, &st);
              a->Set(i,String::New(st.name));
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

    std::string input_file = TOSTR(args[0]);
  
    // TODO - enforce valid index
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());

    struct zip_file *zf_ptr;

    int idx = -1;
    int num = zip_get_num_files(zf->archive_);
    int i = 0;
    for (i=0; i<num; i++) {
        struct zip_stat st;
        zip_stat_index(zf->archive_, i, 0, &st);
        if (st.name == input_file) {
            idx = i;
        }
    }

    if (idx == -1) {
  	    std::stringstream s;
  	    s << "No file found by the name of: '" << input_file << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));    
    }

    if ((zf_ptr=zip_fopen_index(zf->archive_, idx, 0)) == NULL) {
  	    zip_fclose(zf_ptr);
  	    std::stringstream s;
  	    s << "cannot open file #" << idx << " in " << input_file << ": archive error: " << zip_strerror(zf->archive_) << "\n";
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
  	    s << "error reading file #" << idx << " in " << input_file << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
  	    return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    #if NODE_VERSION_AT_LEAST(0,3,0)
        node::Buffer *retbuf = Buffer::New((char *)&data[0],data.size());
    #else
        node::Buffer *retbuf = Buffer::New(data.size());
        memcpy(retbuf->data(), (char *)&data[0], data.size());
    #endif
    
    zip_fclose(zf_ptr);
    return scope.Close(retbuf->handle_);
}
