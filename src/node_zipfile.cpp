#include "node_zipfile.hpp"

#include <sstream>

#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

Persistent<FunctionTemplate> ZipFile::constructor;

void ZipFile::Initialize(Handle<Object> target) {

    HandleScope scope;
  
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ZipFile::New));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(String::NewSymbol("ZipFile"));

    // properties
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("count"), get_prop);
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("names"), get_prop);

    target->Set(String::NewSymbol("ZipFile"),constructor->GetFunction());
}

ZipFile::ZipFile() :
  ObjectWrap(),
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

    ZipFile* zf = new ZipFile();
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