#ifndef INCLUDE_ZIPFILE_NODE_ZIPFILE_H_
#define INCLUDE_ZIPFILE_NODE_ZIPFILE_H_

// v8
#include <v8.h>

// node
#include <node.h>
#include <nan.h>
#include <node_object_wrap.h>

// stl
#include <string>
#include <vector>

class ZipFile: public node::ObjectWrap {
 public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Initialize(v8::Handle<v8::Object> target);
    static NAN_METHOD(New);
    static NAN_GETTER(get_prop);
    static NAN_METHOD(copyFile);
    static NAN_METHOD(copyFileSync);
    static NAN_METHOD(readFileSync);
    static NAN_METHOD(readFile);
    static void Work_ReadFile(uv_work_t* req);
    static void Work_AfterReadFile(uv_work_t* req);
    ZipFile(std::string const& file_name);
    std::vector<std::string> const& names() const { return names_; }
    std::string const& file_name() const { return file_name_; }
 private:
    std::string file_name_;
    std::vector<std::string> names_;
};

#endif  // INCLUDE_ZIPFILE_NODE_ZIPFILE_H_
