#include "node_zipfile.hpp"

// stl
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <set>

#include <node_buffer.h>
#include <node_version.h>

#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

Persistent<FunctionTemplate> ZipFile::constructor;

static ev_async notifier;
static std::set<save_closure_t *> saving_closures;

void ZipFile::Initialize(Handle<Object> target) {

    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ZipFile::New));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(String::NewSymbol("ZipFile"));

    // functions
    //NODE_SET_PROTOTYPE_METHOD(constructor, "readFileSync", readFileSync);
    NODE_SET_METHOD(target, "open", Open);
    NODE_SET_METHOD(target, "read", Read);
    NODE_SET_METHOD(target, "close", Close);
    NODE_SET_METHOD(target, "addFile", Add_File);
    NODE_SET_METHOD(target, "replaceFile", Replace_File);
    NODE_SET_METHOD(target, "save", Save);

    // properties
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("count"), get_prop);
    constructor->InstanceTemplate()->SetAccessor(String::NewSymbol("names"), get_prop);

    // Initiate async handler
    ev_async_init(&notifier, Save_Callback);
    ev_async_start(EV_DEFAULT_UC_ &notifier);

    target->Set(String::NewSymbol("ZipFile"),constructor->GetFunction());
}

ZipFile::ZipFile(std::string const& file_name) :
  ObjectWrap(),
  file_name(file_name),
  archive(),
  file(NULL),
  saving(false),
  names() {}

ZipFile::~ZipFile() {
    assert(file == NULL);
    ev_async_stop(EV_DEFAULT_UC_ &notifier);
    zip_close(archive);
}

bool ZipFile::Busy() {
  if (this->file != NULL || this->saving) return true;
  return false;
}

Handle<Value> ZipFile::New(const Arguments& args)
{
    if (!args.IsConstructCall())
        return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(Exception::TypeError(
          String::New("first argument must be a path to a zipfile")));

    std::string input_file = TOSTR(args[0]);
    struct zip *za;
    int err;
    char errstr[1024];
    if ((za=zip_open(input_file.c_str(), ZIP_CREATE, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << input_file << " error: " << errstr << "\n";
        return ThrowException(Exception::Error(
            String::New(s.str().c_str())));
    }

    ZipFile* zf = new ZipFile(input_file);

    zf->archive = za;
    zf->GetNames();
    zf->Wrap(args.This());
    return args.This();

}

void ZipFile::GetNames()
{
    int num = zip_get_num_files(this->archive);
    this->names.clear();
    this->names.reserve(num);
    int i = 0;
    for (i=0; i<num; i++) {
        struct zip_stat st;
        zip_stat_index(this->archive, i, 0, &st);
        this->names.push_back(st.name);
    }
}

Handle<Value> ZipFile::get_prop(Local<String> property,
                         const AccessorInfo& info)
{
    HandleScope scope;
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(info.This());
    std::string a = TOSTR(property);
    if (a == "count") {
          return scope.Close(Integer::New(zf->names.size()));
    }
    if (a == "names") {
          unsigned num = zf->names.size();
          Local<Array> a = Array::New(num);
          for (unsigned i = 0; i < num; ++i )
          {
              a->Set(i,String::New(zf->names[i].c_str()));
          }
          
          return scope.Close(a);
    }
    return Undefined();    
}

Handle<Value> ZipFile::Open(const Arguments& args)
{
    if (args.Length() != 2 || !args[1]->IsString())
        return ThrowException(Exception::TypeError(
          String::New("Argument must be a file name inside the zip")));

    std::string name = TOSTR(args[1]);
  
    // TODO - enforce valid index
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (zf->Busy())
      return ThrowException(Exception::Error(String::New("Zipfile already in use..")));

    int idx = -1;
    
    std::vector<std::string>::iterator it = std::find(zf->names.begin(), zf->names.end(), name);
    if (it!=zf->names.end()) {
        idx = distance(zf->names.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << name << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    if ((zf->file=zip_fopen_index(zf->archive, idx, 0)) == NULL) {
        zip_fclose(zf->file);
        zf->file = NULL;
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << name << ": archive error: " << zip_strerror(zf->archive) << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    return Undefined();
}

Handle<Value> ZipFile::Close(const Arguments& args)
{
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (zf->file == NULL)
      return ThrowException(Exception::Error(String::New("No file opened!")));

    if (zip_fclose(zf->file) != 0) {
        std::stringstream s;
        s << "Error while closing zip file: " << zip_file_strerror(zf->file) << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    zf->file = NULL;

    return Undefined();
}

/* zipfile.read(buffer, pos, len, cb) -> cb(bytesRead, error) */
Handle<Value> ZipFile::Read(const Arguments& args)
{
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (!Buffer::HasInstance(args[1])) {
      return ThrowException(Exception::Error(
                  String::New("Second argument needs to be a buffer")));
    }
    Local<Object> buffer_obj = args[1]->ToObject();
    char *buffer_data = Buffer::Data(buffer_obj);
    size_t buffer_length = Buffer::Length(buffer_obj);

    zip_uint64_t off = args[2]->Int32Value();
    if (off >= buffer_length) {
      return ThrowException(Exception::Error(
            String::New("Offset is out of bounds")));
    }

    zip_uint64_t len = args[3]->Int32Value();
    if (off + len > buffer_length) {
      return ThrowException(Exception::Error(
            String::New("Length is extends beyond buffer")));
    }

    Local<Value> cb = args[4];
    if (!cb->IsFunction())
      return ThrowException(Exception::Error(
            String::New("Fourth argument should be a callback function.")));

    read_closure_t *closure = new read_closure_t();
    closure->zf = zf;
    closure->read = 0;
    closure->data = buffer_data+off;
    closure->len = len;
    closure->cb = Persistent<Function>::New(Handle<Function>::Cast(args[4]));

    eio_custom(EIO_Read, EIO_PRI_DEFAULT, EIO_AfterRead, closure);
    zf->Ref();
    ev_ref(EV_DEFAULT_UC);

    return Undefined();
}

int ZipFile::EIO_Read(eio_req *req)
{
    read_closure_t *closure = static_cast<read_closure_t *>(req->data);

    closure->read = zip_fread(closure->zf->file, closure->data, closure->len);

    return 0;
}

int ZipFile::EIO_AfterRead(eio_req *req)
{
    read_closure_t *closure = static_cast<read_closure_t *>(req->data);

    Handle<Value> argv[2];

    if (closure->read < 0) {
      std::stringstream s;
      s << "Error while reading zip file: " << zip_file_strerror(closure->zf->file) << "\n";
      argv[0] = Exception::Error(String::New(s.str().c_str()));
      argv[1] = Undefined();
    } else {
      argv[0] = Undefined();
      argv[1] = Integer::New(closure->read);
    }

    closure->cb->Call(Context::GetCurrent()->Global(), 2, argv);

    closure->zf->Unref();
    closure->cb.Dispose();
    delete closure;
    
    ev_unref(EV_DEFAULT_UC);

    return 0;
}

/* zipfile.addFile(zipfile, nameInArchive, name, offset, len) */
Handle<Value> ZipFile::Add_File(const Arguments& args)
{
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    struct zip_source *source;

    if (zf->Busy())
      return ThrowException(Exception::Error(String::New("Zipfile already in use..")));

    if (!args[1]->IsString())
      return ThrowException(Exception::TypeError(
                 String::New("Argument must be a file name.")));
    std::string archive_file = TOSTR(args[1]);

    if (!args[2]->IsString())
      return ThrowException(Exception::TypeError(
                 String::New("Argument must be a file name.")));
    std::string name = TOSTR(args[2]);

    zip_int64_t off = args[3]->Int32Value();
    zip_int64_t len = args[4]->Int32Value();

    source = zip_source_file(zf->archive, name.c_str(), off, len);
    if (source == NULL) {
      std::stringstream s;
      s << "Error while adding file " << name << " to zip archive: " << zip_strerror(zf->archive) << "\n";
      return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }
 
    int ret = zip_add(zf->archive, archive_file.c_str(), source); 
    if (ret < 0) {
      zip_source_free(source);
      std::stringstream s;
      s << "Error while adding file " << name << " to zip archive: " << zip_strerror(zf->archive) << "\n";
      return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    zf->GetNames();

    return Undefined();
}

/* zipfile.replaceFile(zipfile, nameInArchive, name, offset, len) */
Handle<Value> ZipFile::Replace_File(const Arguments& args)
{
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    struct zip_source *source;

    if (zf->Busy())
      return ThrowException(Exception::Error(String::New("Zipfile already in use..")));

    if (!args[1]->IsString())
      return ThrowException(Exception::TypeError(
                 String::New("Argument must be a file name.")));
    std::string archive_file = TOSTR(args[1]);

    if (!args[2]->IsString())
      return ThrowException(Exception::TypeError(
                 String::New("Argument must be a file name.")));
    std::string name = TOSTR(args[2]);

    zip_int64_t off = args[3]->Int32Value();
    zip_int64_t len = args[4]->Int32Value();

    int idx = -1;
    
    std::vector<std::string>::iterator it = std::find(zf->names.begin(), zf->names.end(), archive_file);
    if (it!=zf->names.end()) {
        idx = distance(zf->names.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << archive_file << "\n";
        return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    source = zip_source_file(zf->archive, name.c_str(), off, len);
    if (source == NULL) {
      std::stringstream s;
      s << "Error while replacing file " << name << " in zip archive: " << zip_strerror(zf->archive) << "\n";
      return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    int ret = zip_replace(zf->archive, idx, source);        
    if (ret < 0) {
      zip_source_free(source);
      std::stringstream s;
      s << "Error while replacing file " << name << " in zip archive: " << zip_strerror(zf->archive) << "\n";
      return ThrowException(Exception::Error(String::New(s.str().c_str())));
    }

    return Undefined();
}

void *ZipFile::Save_Thread(void *data) {
    save_closure_t *save = (save_closure_t *)data;

    int ret = zip_close(save->zf->archive);
   
    pthread_mutex_lock(&save->mutex);
    
    if (ret < 0) {
      std::stringstream s;
      s << "Error while saving in zip archive: " << zip_strerror(save->zf->archive) << "\n";
      save->error = new std::string(s.str());
    } else {
      // We assume that reopening is ok here..
      int err;
      save->zf->archive = zip_open(save->zf->file_name.c_str(), ZIP_CREATE, &err);
    }
    
    save->done = true;
   
    pthread_mutex_unlock(&save->mutex);

    ev_async_send(EV_DEFAULT_UC_ &notifier);

    return NULL;
}

/* zipfile.save(zipfile, callback) */
Handle<Value> ZipFile::Save(const Arguments& args)
{
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args[0]->ToObject());

    if (zf->Busy())
      return ThrowException(Exception::Error(String::New("Zipfile already in use..")));

    Local<Value> cb = args[1];
    if (!cb->IsFunction())
      return ThrowException(Exception::Error(
            String::New("Second argument should be a callback function.")));

    save_closure_t *save = new save_closure_t;

    save->done = false;
    save->zf = zf;
    save->error = NULL;
    save->save_cb = Persistent<Function>::New(Handle<Function>::Cast(cb));
    pthread_mutex_init(&save->mutex, NULL);

    saving_closures.insert(save);

    zf->saving = true;
    zf->Ref();

    ev_ref(EV_DEFAULT_UC);
    pthread_create(&save->thread, NULL, Save_Thread, save);
    
    return Undefined();
}

// callback that runs javascript in main thread
void ZipFile::Save_Callback(EV_P_ ev_async *watcher, int revents)
{
    std::set<save_closure_t *>::iterator it;
    save_closure_t *save;
    Handle<Value> argv[1];

    assert(watcher == &notifier);
    assert(revents == EV_ASYNC);

    std::set<save_closure_t *> remove;

    for(it = saving_closures.begin(); it != saving_closures.end(); it++) {
      save = *it;
      pthread_mutex_lock(&save->mutex);
      if (save->done) {
        save->zf->saving = false;

        if (save->error != NULL) 
          argv[0] = Exception::Error(String::New(save->error->c_str()));
        else
          argv[0] = Undefined();
        
        save->save_cb->Call(Context::GetCurrent()->Global(), 1, argv);
        remove.insert(save);
      }
      pthread_mutex_unlock(&save->mutex);
    }

  // Now, let's do some janitoring !
  for(it = remove.begin(); it != remove.end(); it++) {
    save = *it;
    // Now, let's do some janitoring !
    saving_closures.erase(save);
    if (save->error != NULL) delete save->error;
    save->zf->Unref();
    save->save_cb.Dispose();
    pthread_mutex_destroy(&save->mutex);
    delete save;
  }

  ev_unref(EV_DEFAULT_UC);
}
