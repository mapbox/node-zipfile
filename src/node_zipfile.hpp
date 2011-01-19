#ifndef __NODE_ZIPFILE_H__
#define __NODE_ZIPFILE_H__

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

// stl
#include <string>

// libzip
#include <zlib.h>
#include <errno.h>
#include <zip.h>
//#include <zipint.h>

using namespace v8;
using namespace node;

class ZipFile: public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor;
    static void Initialize(Handle<Object> target);
    static Handle<Value> New(const Arguments &args);
    
    static Handle<Value> get_prop(Local<String> property,
                         const AccessorInfo& info);

    ZipFile();

  private:
    ~ZipFile();
    struct zip *archive_;
};

#endif
