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
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static void Initialize(v8::Local<v8::Object> target);
    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void get_prop(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value>& args);
    static void copyFile(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void Work_CopyFile(uv_work_t* req);
    static void Work_AfterCopyFile(uv_work_t* req, int status);
    static void copyFileSync(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void readFileSync(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void readFile(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void Work_ReadFile(uv_work_t* req);
    static void Work_AfterReadFile(uv_work_t* req, int status);
    ZipFile(std::string const& file_name);
    std::vector<std::string> const& names() const { return names_; }
    std::string const& file_name() const { return file_name_; }
 private:
    std::string file_name_;
    std::vector<std::string> names_;
};

#endif  // INCLUDE_ZIPFILE_NODE_ZIPFILE_H_
