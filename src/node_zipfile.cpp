#include "node_zipfile.hpp"

#include <node_version.h>

// libzip
extern "C" {
 #include <zlib.h>
 #include <errno.h>
 #include <zip.h>
}

#ifdef _WINDOWS
#include "unicode_conversion_helpers.hpp"
#endif

// std
#include <sstream>
#include <iostream>
#include <algorithm>
#include <fstream>

#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

using namespace v8;

Persistent<FunctionTemplate> ZipFile::constructor;

void ZipFile::Initialize(Handle<Object> target) {
    NanScope();
    Local<FunctionTemplate> lcons = NanNew<FunctionTemplate>(ZipFile::New);
    lcons->InstanceTemplate()->SetInternalFieldCount(1);
    lcons->SetClassName(NanNew("Zipfile"));

    // functions
    NODE_SET_PROTOTYPE_METHOD(lcons, "readFileSync", readFileSync);
    NODE_SET_PROTOTYPE_METHOD(lcons, "readFile", readFile);
    NODE_SET_PROTOTYPE_METHOD(lcons, "copyFileSync", copyFileSync);
    NODE_SET_PROTOTYPE_METHOD(lcons, "copyFile", copyFile);

    // properties
    lcons->InstanceTemplate()->SetAccessor(NanNew("count"), get_prop);
    lcons->InstanceTemplate()->SetAccessor(NanNew("names"), get_prop);

    target->Set(NanNew("ZipFile"),lcons->GetFunction());
    NanAssignPersistent(constructor, lcons);
}

ZipFile::ZipFile(std::string const& file_name)
    : ObjectWrap(),
      file_name_(file_name),
      names_() {}

NAN_METHOD(ZipFile::New)
{
    NanScope();

    if (!args.IsConstructCall())
    {
        NanThrowError("Cannot call constructor as function, you need to use 'new' keyword");
        NanReturnUndefined();
    }

    if (args.Length() != 1 || !args[0]->IsString())
    {
        NanThrowError("first argument must be a path to a zipfile");
        NanReturnUndefined();
    }

    std::string input_file = TOSTR(args[0]);
#ifdef _WINDOWS
	input_file = wstring2string(utf8ToWide(input_file));
#endif
	int err;
    char errstr[1024];
    struct zip *za = NULL;
    if ((za=zip_open(input_file.c_str(), ZIP_CHECKCONS, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << input_file << " error: " << errstr << "\n";
        if (za) zip_close(za);
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    ZipFile* zf = new ZipFile(input_file);

    int num = zip_get_num_files(za);
    zf->names_.reserve(num);
    int i = 0;
    for (i = 0; i < num; i++) {
        struct zip_stat st;
        zip_stat_index(za, i, 0, &st);
		std::string name = st.name;
        zf->names_.push_back(name);
    }
    if (za) zip_close(za);
    zf->Wrap(args.This());
    NanReturnValue(args.This());
}

NAN_GETTER(ZipFile::get_prop)
{
    NanScope();
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.Holder());
    std::string a = TOSTR(property);
    if (a == "count") {
        NanReturnValue(NanNew<Integer>(zf->names_.size()));
    } else if (a == "names") {
        unsigned num = zf->names_.size();
        Local<Array> a = NanNew<Array>(num);
        for (unsigned i = 0; i < num; ++i) {
            a->Set(i, NanNew(zf->names_[i].c_str()));
        }
        NanReturnValue(a);
    }
    NanReturnUndefined();
}

NAN_METHOD(ZipFile::copyFileSync)
{
    NanScope();

    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString())
    {
        NanThrowError("requires two args: first argument must be a filename inside the zip and second must be a filename to write to.");
        NanReturnUndefined();
    }


    std::string name = TOSTR(args[0]);
    std::string to_name = TOSTR(args[1]);
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());
    int idx = -1;
    std::vector<std::string>::iterator it = std::find(zf->names_.begin(), zf->names_.end(), name);
    if (it != zf->names_.end()) {
        idx = distance(zf->names_.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << name << "\n";
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    int err;
    char errstr[1024];
    struct zip *za = NULL;
    if ((za=zip_open(zf->file_name_.c_str(), ZIP_CHECKCONS, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << zf->file_name_ << " error: " << errstr << "\n";
        if (za) zip_close(za);
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

/*

    std::ofstream file_out(to_name.c_str(), std::ios::out| std::ios::trunc|std::ios::binary);
    if (!file_out)
    {
        std::stringstream s;
        s << "Could not open for writing: '" << to_name << "\n";
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }
*/

    FILE * fo = fopen(to_name.c_str(), "wb");
    if (!fo) {
        std::stringstream s;
        s << "Could not open for writing with C IO: '" << to_name << "\n";
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    struct zip_file *zf_ptr = NULL;

    if ((zf_ptr=zip_fopen_index(za, idx, 0)) == NULL) {
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << name << ": archive error: " << zip_strerror(za) << "\n";
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    struct zip_stat st;
    zip_stat_index(za, idx, 0, &st);
    
/*
    char buf[1000000*1]; // read 10 MB chunks
    std::size_t buf_len = sizeof(buf);
    zip_int64_t result = 0;
    while ((result=zip_fread(zf_ptr, buf, buf_len)) > 0) {
        file_out.write(buf,result);
    }
*/

    std::size_t buf_len = 1000000*1;
    std::unique_ptr<char[]> buf(new char[buf_len]); // read 10 MB chunks
    zip_int64_t result = 0;

    while ((result=zip_fread(zf_ptr, buf.get(), buf_len)) > 0) {
        fwrite(buf.get(),1,result,fo);
    }

    fclose(fo);

    if (result < 0) {
        std::stringstream s;
        s << "error reading file #" << idx << " in " << name << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }
    if (zf_ptr) zip_fclose(zf_ptr);
    if (za) zip_close(za);
    NanReturnUndefined();
}

NAN_METHOD(ZipFile::copyFile)
{
    NanScope();
    NanThrowError("Async copyFile not yet implemented");
    NanReturnUndefined();
}

NAN_METHOD(ZipFile::readFileSync)
{
    NanScope();

    if (args.Length() != 1 || !args[0]->IsString())
    {
        NanThrowError("first argument must be a file name inside the zip");
        NanReturnUndefined();
    }

    std::string name = TOSTR(args[0]);
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());
    int idx = -1;
    std::vector<std::string>::iterator it = std::find(zf->names_.begin(), zf->names_.end(), name);
    if (it != zf->names_.end()) {
        idx = distance(zf->names_.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << name << "\n";
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    int err;
    char errstr[1024];
    struct zip *za = NULL;
    if ((za=zip_open(zf->file_name_.c_str(), ZIP_CHECKCONS, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << zf->file_name_ << " error: " << errstr << "\n";
        if (za) zip_close(za);
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    struct zip_file *zf_ptr = NULL;

    if ((zf_ptr=zip_fopen_index(za, idx, 0)) == NULL) {
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << name << ": archive error: " << zip_strerror(za) << "\n";
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    struct zip_stat st;
    zip_stat_index(za, idx, 0, &st);
  
    std::vector<unsigned char> data;
    data.clear();
    data.resize(st.size);

    int result = 0;
    result = static_cast<int>(zip_fread(zf_ptr, reinterpret_cast<void*> (&data[0]), data.size()));

    if (result < 0) {
        std::stringstream s;
        s << "error reading file #" << idx << " in " << name << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }
    Local<Object> retbuf = NanNewBufferHandle(reinterpret_cast<char *>(&data[0]), data.size());
    if (zf_ptr) zip_fclose(zf_ptr);
    if (za) zip_close(za);
    NanReturnValue(retbuf);
}

typedef struct {
    uv_work_t request;
    ZipFile* zf;
    struct zip *za;
    std::string name;
    bool error;
    std::string error_name;
    std::vector<unsigned char> data;
    Persistent<Function> cb;
} closure_t;


NAN_METHOD(ZipFile::readFile)
{
    NanScope();
    if (args.Length() < 2)
    {
        NanThrowError("requires two arguments, the name of a file and a callback");
        NanReturnUndefined();
    }

    // first arg must be name
    if (!args[0]->IsString())
    {
        NanThrowError("first argument must be a file name inside the zip");
        NanReturnUndefined();
    }

    // ensure function callback
    Local<Value> callback = args[args.Length() - 1];
    if (!callback->IsFunction()) {
        NanThrowTypeError("last argument must be a callback function");
        NanReturnUndefined();
    }

    std::string name = TOSTR(args[0]);
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());
    closure_t *closure = new closure_t();
    closure->request.data = closure;
    closure->za = NULL;

    // libzip is not threadsafe so we open a new zip archive for each thread
    int err;
    char errstr[1024];
    if ((closure->za=zip_open(zf->file_name_.c_str() , ZIP_CHECKCONS, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << zf->file_name_ << " error: " << errstr << "\n";
        if (closure->za) zip_close(closure->za);
        NanDisposePersistent(closure->cb);
        delete closure;
        NanThrowError(s.str().c_str());
        NanReturnUndefined();
    }

    closure->zf = zf;
    closure->error = false;
    closure->name = name;
    NanAssignPersistent(closure->cb, callback.As<Function>());
    uv_queue_work(uv_default_loop(), &closure->request, Work_ReadFile, (uv_after_work_cb)Work_AfterReadFile);
    zf->Ref();
    NanReturnUndefined();
}


void ZipFile::Work_ReadFile(uv_work_t* req) {
    closure_t *closure = static_cast<closure_t *>(req->data);
    struct zip_file *zf_ptr = NULL;
    int idx = -1;
    std::vector<std::string>::iterator it = std::find(closure->zf->names_.begin(),
                                                      closure->zf->names_.end(),
                                                      closure->name);
    if (it != closure->zf->names_.end()) {
        idx = distance(closure->zf->names_.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << closure->name << "\n";
        closure->error = true;
        closure->error_name = s.str();
    } else {
        if ((zf_ptr = zip_fopen_index(closure->za, idx, 0)) == NULL) {
            std::stringstream s;
            s << "cannot open file #" << idx << " in "
              << closure->name << ": archive error: " << zip_strerror(closure->za) << "\n";
            closure->error = true;
            closure->error_name = s.str();
        } else {
            struct zip_stat st;
            zip_stat_index(closure->za, idx, 0, &st);
            closure->data.clear();
            closure->data.resize(st.size);

            int result =  0;
            result = static_cast<int>(zip_fread(zf_ptr, reinterpret_cast<void*> (&closure->data[0]), closure->data.size()));

            if (result < 0) {
                std::stringstream s;
                s << "error reading file #" << idx << " in "
                  << closure->name << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
                closure->error = true;
                closure->error_name = s.str();
            }
        }
        if (zf_ptr) zip_fclose(zf_ptr);
    }
}

void ZipFile::Work_AfterReadFile(uv_work_t* req) {
    NanScope();

    closure_t *closure = static_cast<closure_t *>(req->data);

    if (closure->error) {
        Local<Value> argv[1] = { NanError(closure->error_name.c_str()) };
        NanMakeCallback(NanGetCurrentContext()->Global(), NanNew(closure->cb), 1, argv);
    } else {
        Local<Object> retbuf = NanNewBufferHandle(reinterpret_cast<char *>(&closure->data[0]), closure->data.size());
        Local<Value> argv[2] = { NanNull(), retbuf };
        NanMakeCallback(NanGetCurrentContext()->Global(), NanNew(closure->cb), 2, argv);
    }

    if (closure->za) zip_close(closure->za);
    closure->zf->Unref();
    NanDisposePersistent(closure->cb);
    delete closure;
}

extern "C" {
    static void init(Handle<Object> target) {
        ZipFile::Initialize(target);

        // node-zipfile version
        target->Set(NanNew("version"), NanNew("0.3.1"));

        // versions of deps
        Local<Object> versions = NanNew<Object>();
        versions->Set(NanNew("node"), NanNew(NODE_VERSION+1));
        versions->Set(NanNew("v8"), NanNew(V8::GetVersion()));
        target->Set(NanNew("versions"), versions);
    }
    #define MAKE_MODULE(_modname) NODE_MODULE( _modname, init)
    MAKE_MODULE(MODULE_NAME)
}

