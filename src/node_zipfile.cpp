#include "node_zipfile.hpp"

Persistent<FunctionTemplate> ZipFile::constructor;

void ZipFile::Initialize(Handle<Object> target) {

    HandleScope scope;
  
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ZipFile::New));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(String::NewSymbol("ZipFile"));

    target->Set(String::NewSymbol("ZipFile"),constructor->GetFunction());
}

ZipFile::ZipFile(std::string const& name) :
  ObjectWrap(),
  archive_() {}

ZipFile::~ZipFile() {}

Handle<Value> ZipFile::New(const Arguments& args)
{
  HandleScope scope;
}
