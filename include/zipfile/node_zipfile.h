#ifndef INCLUDE_ZIPFILE_NODE_ZIPFILE_H_
#define INCLUDE_ZIPFILE_NODE_ZIPFILE_H_

// libzip
extern "C" {
#include <zlib.h>
#include <errno.h>
#include <zip.h>
}

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

// stl
#include <string>
#include <vector>


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
    static void Work_ReadFile(uv_work_t* req);
    static void Work_AfterReadFile(uv_work_t* req);

    explicit ZipFile(std::string const& file_name);

 private:
    ~ZipFile();
    std::string const file_name_;
    struct zip *archive_;
    std::vector<std::string> names_;
};

#endif  // INCLUDE_ZIPFILE_NODE_ZIPFILE_H_
