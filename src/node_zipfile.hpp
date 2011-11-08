#ifndef __NODE_ZIPFILE_H__
#define __NODE_ZIPFILE_H__

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include <node_version.h>

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
    static void EIO_ReadFile(uv_work_t* req);
    static void EIO_AfterReadFile(uv_work_t* req);
    
    ZipFile(std::string const& file_name);

  private:
    ~ZipFile();
    std::string const file_name_;
    struct zip *archive_;
    std::vector<std::string> names_;
};

#endif
