#ifndef __NODE_ZIPFILE_H__
#define __NODE_ZIPFILE_H__

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

// stl
#include <string>
#include <vector>

// Threads
#include <pthread.h>

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
    static Handle<Value> Open(const Arguments& args);
    static Handle<Value> readFileSync(const Arguments& args);
    static Handle<Value> Close(const Arguments& args);
    static Handle<Value> Add_File(const Arguments& args);
    static Handle<Value> Replace_File(const Arguments& args); 
    static Handle<Value> Add_Directory(const Arguments& args);

    // Async
    static Handle<Value> Read(const Arguments& args);
    static int EIO_Read(eio_req *req);
    static int EIO_AfterRead(eio_req *req);
    static Handle<Value> Save(const Arguments& args);
    static void *Save_Thread(void *data);
    static void Save_Callback(EV_P_ ev_async *watcher, int revents);
    ZipFile(std::string const& file_name);

  protected:
    void GetNames();
    std::string const file_name;
    struct zip *archive;
    struct zip_file *file;
    int file_index;
    bool saving;

  private:
    ~ZipFile();
    bool Busy();
    std::vector<std::string> names;
};

typedef struct {
    ZipFile              *zf;
    zip_uint64_t         read;
    zip_uint64_t         len;
    char                 *data;
    Persistent<Function> cb;
} read_closure_t;

typedef struct {
    ZipFile              *zf;
    pthread_mutex_t      mutex;
    pthread_t            thread;
    bool                 done;
    Persistent<Function> save_cb;
    std::string          *error;
} save_closure_t;

#endif
