#ifndef __NODE_ZIPFILE_H__
#define __NODE_ZIPFILE_H__

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

// stl
#include <string>
#include <vector>

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

    // Sync
    static Handle<Value> readFileSync(const Arguments& args);
    
    // Async
    static Handle<Value> readFile(const Arguments& args);
    static int EIO_ReadFile(eio_req *req);
    static int EIO_AfterReadFile(eio_req *req);
    
    ZipFile(std::string const& file_name);

  private:
    ~ZipFile();
    std::string const file_name_;
    struct zip *archive_;
    std::vector<std::string> names_;
};

#endif
