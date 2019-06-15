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
#include <stdexcept>

#define TOSTR(obj) (*Nan::Utf8String(Nan::To<v8::String>(obj).ToLocalChecked()))

using namespace v8;

Nan::Persistent<FunctionTemplate> ZipFile::constructor;

void ZipFile::Initialize(Local<Object> target) {
    Nan::HandleScope scope;
    Local<FunctionTemplate> lcons = Nan::New<FunctionTemplate>(ZipFile::New);
    lcons->InstanceTemplate()->SetInternalFieldCount(1);
    lcons->SetClassName(Nan::New("Zipfile").ToLocalChecked());

    // functions
    Nan::SetPrototypeMethod(lcons, "readFileSync", readFileSync);
    Nan::SetPrototypeMethod(lcons, "readFile", readFile);
    Nan::SetPrototypeMethod(lcons, "copyFileSync", copyFileSync);
    Nan::SetPrototypeMethod(lcons, "copyFile", copyFile);

    // properties
    Nan::SetAccessor(lcons->InstanceTemplate(), Nan::New("count").ToLocalChecked(), get_prop);
    Nan::SetAccessor(lcons->InstanceTemplate(), Nan::New("names").ToLocalChecked(), get_prop);

    Nan::Set(target, Nan::New("ZipFile").ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());
    constructor.Reset(lcons);
}

ZipFile::ZipFile(std::string const& file_name)
    : ObjectWrap(),
      file_name_(file_name),
      names_() {}

void ZipFile::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    Nan::HandleScope scope;

    if (!args.IsConstructCall())
    {
        Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
        args.GetReturnValue().SetUndefined(); return;
    }

    if (args.Length() != 1 || !args[0]->IsString())
    {
        Nan::ThrowError("first argument must be a path to a zipfile");
        args.GetReturnValue().SetUndefined(); return;
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
        Nan::ThrowError(s.str().c_str());
        args.GetReturnValue().SetUndefined(); return;
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
    args.GetReturnValue().Set(args.This());
}

void ZipFile::get_prop(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value>& args)
{
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.Holder());
    std::string a = TOSTR(property);
    if (a == "count") {
        args.GetReturnValue().Set(Nan::New<Integer>(static_cast<uint32_t>(zf->names_.size())));
    } else if (a == "names") {
        unsigned num = zf->names_.size();
        Local<Array> a = Nan::New<Array>(num);
        for (unsigned i = 0; i < num; ++i) {
            Nan::Set(a, i, Nan::New(zf->names_[i].c_str()).ToLocalChecked());
        }
        args.GetReturnValue().Set(a);
    } else {
        args.GetReturnValue().SetUndefined();
    }
}

void _copyFile(std::string const& from, std::string const& to, ZipFile* zf)
{
    int idx = -1;
    std::vector<std::string> const& names = zf->names();
    std::vector<std::string>::const_iterator it = std::find(names.begin(), names.end(), from);
    if (it != names.end()) {
        idx = distance(names.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << from << "\n";
        throw std::runtime_error(s.str().c_str());
    }

    int err;
    char errstr[1024];
    struct zip *za = NULL;
    std::string const& zip_filename = zf->file_name();
    if ((za=zip_open(zip_filename.c_str(), ZIP_CHECKCONS, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << zip_filename << " error: " << errstr << "\n";
        if (za) zip_close(za);
        throw std::runtime_error(s.str().c_str());
    }

    FILE * fo = fopen(to.c_str(), "wb");
    if (!fo) {
        std::stringstream s;
        s << "Could not open for writing: '" << to << "\n";
        throw std::runtime_error(s.str().c_str());
    }

    struct zip_file *zf_ptr = NULL;

    if ((zf_ptr=zip_fopen_index(za, idx, 0)) == NULL) {
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << from << ": archive error: " << zip_strerror(za) << "\n";
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        fclose(fo);
        throw std::runtime_error(s.str().c_str());
    }

    struct zip_stat st;
    zip_stat_index(za, idx, 0, &st);

    std::size_t buf_len = 1000000; // 1 MB chunk
    char * buf = new char[buf_len];
    zip_int64_t result = 0;

    while ((result=zip_fread(zf_ptr, buf, buf_len)) > 0) {
        fwrite(buf,1,result,fo);
    }

    delete [] buf;

    fclose(fo);

    if (result < 0) {
        std::stringstream s;
        s << "error reading file #" << idx << " in " << from << ": archive error: " << zip_file_strerror(zf_ptr) << "\n";
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        throw std::runtime_error(s.str().c_str());
    }
    if (zf_ptr) zip_fclose(zf_ptr);
    if (za) zip_close(za);
}

void ZipFile::copyFileSync(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    Nan::HandleScope scope;

    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString())
    {
        Nan::ThrowError("requires two args: first argument must be a filename inside the zip and second must be a filename to write to.");
        args.GetReturnValue().SetUndefined(); return;
    }

    std::string from = TOSTR(args[0]);
    std::string to = TOSTR(args[1]);
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());
    try {
        _copyFile(from,to,zf);
    } catch (std::exception const& ex) {
        Nan::ThrowError(ex.what());
    }
    args.GetReturnValue().SetUndefined();
}

struct copy_file_baton {
    uv_work_t request;
    ZipFile* zf;
    std::string from;
    std::string to;
    std::string error_name;
    Nan::Persistent<Function> cb;
};

void ZipFile::copyFile(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    Nan::HandleScope scope;
    if (args.Length() < 3)
    {
        Nan::ThrowError("requires three arguments: filename inside the zip, a filename to write to, and a callback");
        args.GetReturnValue().SetUndefined(); return;
    }

    if (!args[0]->IsString() || !args[1]->IsString())
    {
        Nan::ThrowError("first argument must be a filename inside the zip and second must be a filename to write to.");
        args.GetReturnValue().SetUndefined(); return;
    }

    // ensure function callback
    Local<Value> callback = args[args.Length() - 1];
    if (!callback->IsFunction()) {
        Nan::ThrowTypeError("last argument must be a callback function");
        args.GetReturnValue().SetUndefined(); return;
    }
    std::string from = TOSTR(args[0]);
    std::string to = TOSTR(args[1]);
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());

    copy_file_baton *closure = new copy_file_baton();
    closure->request.data = closure;
    closure->zf = zf;
    closure->from = from;
    closure->to = to;
    closure->cb.Reset(callback.As<Function>());
    uv_queue_work(uv_default_loop(), &closure->request, Work_CopyFile, (uv_after_work_cb)Work_AfterCopyFile);
    zf->Ref();
    args.GetReturnValue().SetUndefined();
}


void ZipFile::Work_CopyFile(uv_work_t* req) {
    copy_file_baton * closure = static_cast<copy_file_baton *>(req->data);
    try {
        _copyFile(closure->from,closure->to,closure->zf);
    } catch (std::exception const& ex) {
        closure->error_name = ex.what();
    }
}

void ZipFile::Work_AfterCopyFile(uv_work_t* req, int status) {
    Nan::HandleScope scope;
    copy_file_baton *closure = static_cast<copy_file_baton *>(req->data);
    if (!closure->error_name.empty()) {
        Nan::AsyncResource ar("ZipFile::Work_AfterCopyFile");
        Local<Value> argv[1] = { Nan::Error(closure->error_name.c_str()) };
        ar.runInAsyncScope(Nan::GetCurrentContext()->Global(), Nan::New(closure->cb), 1, argv);
    } else {
        Nan::AsyncResource ar("ZipFile::Work_AfterCopyFile");
        Local<Value> argv[1] = { Nan::Null() };
        ar.runInAsyncScope(Nan::GetCurrentContext()->Global(), Nan::New(closure->cb), 1, argv);
    }
    closure->zf->Unref();
    closure->cb.Reset();
    delete closure;
}

void ZipFile::readFileSync(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    Nan::HandleScope scope;

    if (args.Length() != 1 || !args[0]->IsString())
    {
        Nan::ThrowError("first argument must be a file name inside the zip");
        args.GetReturnValue().SetUndefined(); return;
    }

    std::string name = TOSTR(args[0]);
    ZipFile* zf = ObjectWrap::Unwrap<ZipFile>(args.This());
    int idx = -1;
    std::vector<std::string> const& names = zf->names();
    std::vector<std::string>::const_iterator it = std::find(names.begin(), names.end(), name);
    if (it != names.end()) {
        idx = distance(names.begin(), it);
    }

    if (idx == -1) {
        std::stringstream s;
        s << "No file found by the name of: '" << name << "\n";
        Nan::ThrowError(s.str().c_str());
        args.GetReturnValue().SetUndefined(); return;
    }

    int err;
    char errstr[1024];
    struct zip *za = NULL;
    if ((za=zip_open(zf->file_name_.c_str(), ZIP_CHECKCONS, &err)) == NULL) {
        zip_error_to_str(errstr, sizeof(errstr), err, errno);
        std::stringstream s;
        s << "cannot open file: " << zf->file_name_ << " error: " << errstr << "\n";
        if (za) zip_close(za);
        Nan::ThrowError(s.str().c_str());
        args.GetReturnValue().SetUndefined(); return;
    }

    struct zip_file *zf_ptr = NULL;

    if ((zf_ptr=zip_fopen_index(za, idx, 0)) == NULL) {
        if (zf_ptr) zip_fclose(zf_ptr);
        if (za) zip_close(za);
        std::stringstream s;
        s << "cannot open file #" << idx << " in " << name << ": archive error: " << zip_strerror(za) << "\n";
        Nan::ThrowError(s.str().c_str());
        args.GetReturnValue().SetUndefined(); return;
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
        Nan::ThrowError(s.str().c_str());
        args.GetReturnValue().SetUndefined(); return;
    }
    Local<Object> retbuf = Nan::CopyBuffer(reinterpret_cast<char *>(&data[0]), data.size()).ToLocalChecked();
    if (zf_ptr) zip_fclose(zf_ptr);
    if (za) zip_close(za);
    args.GetReturnValue().Set(retbuf);
}

typedef struct {
    uv_work_t request;
    ZipFile* zf;
    struct zip *za;
    std::string name;
    bool error;
    std::string error_name;
    std::vector<unsigned char> data;
    Nan::Persistent<Function> cb;
} closure_t;


void ZipFile::readFile(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    Nan::HandleScope scope;
    if (args.Length() < 2)
    {
        Nan::ThrowError("requires two arguments, the name of a file and a callback");
        args.GetReturnValue().SetUndefined(); return;
    }

    // first arg must be name
    if (!args[0]->IsString())
    {
        Nan::ThrowError("first argument must be a file name inside the zip");
        args.GetReturnValue().SetUndefined(); return;
    }

    // ensure function callback
    Local<Value> callback = args[args.Length() - 1];
    if (!callback->IsFunction()) {
        Nan::ThrowTypeError("last argument must be a callback function");
        args.GetReturnValue().SetUndefined(); return;
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
        closure->cb.Reset();
        delete closure;
        Nan::ThrowError(s.str().c_str());
        args.GetReturnValue().SetUndefined(); return;
    }

    closure->zf = zf;
    closure->error = false;
    closure->name = name;
    closure->cb.Reset(callback.As<Function>());
    uv_queue_work(uv_default_loop(), &closure->request, Work_ReadFile, (uv_after_work_cb)Work_AfterReadFile);
    zf->Ref();
    args.GetReturnValue().SetUndefined();
}


void ZipFile::Work_ReadFile(uv_work_t* req) {
    closure_t *closure = static_cast<closure_t *>(req->data);
    struct zip_file *zf_ptr = NULL;
    int idx = -1;
    std::vector<std::string> const& names = closure->zf->names();
    std::vector<std::string>::const_iterator it = std::find(names.begin(),
                                                      names.end(),
                                                      closure->name);
    if (it != names.end()) {
        idx = distance(names.begin(), it);
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

void ZipFile::Work_AfterReadFile(uv_work_t* req, int status) {
    Nan::HandleScope scope;

    closure_t *closure = static_cast<closure_t *>(req->data);

    if (closure->error) {
        Nan::AsyncResource ar("ZipFile::Work_AfterReadFile");
        Local<Value> argv[1] = { Nan::Error(closure->error_name.c_str()) };
        ar.runInAsyncScope(Nan::GetCurrentContext()->Global(), Nan::New(closure->cb), 1, argv);
    } else {
        Nan::AsyncResource ar("ZipFile::Work_AfterReadFile");
        Local<Object> retbuf = Nan::CopyBuffer(reinterpret_cast<char *>(&closure->data[0]), closure->data.size()).ToLocalChecked();
        Local<Value> argv[2] = { Nan::Null(), retbuf };
        ar.runInAsyncScope(Nan::GetCurrentContext()->Global(), Nan::New(closure->cb), 2, argv);
    }

    if (closure->za) zip_close(closure->za);
    closure->zf->Unref();
    closure->cb.Reset();
    delete closure;
}

NAN_MODULE_INIT(init) {
    ZipFile::Initialize(target);

    // node-zipfile version
    Nan::Set(target, Nan::New("version").ToLocalChecked(), Nan::New("0.5.8").ToLocalChecked());

    // versions of deps
    Local<Object> versions = Nan::New<Object>();
    Nan::Set(versions, Nan::New("node").ToLocalChecked(), Nan::New(NODE_VERSION+1).ToLocalChecked());
    Nan::Set(versions, Nan::New("v8").ToLocalChecked(), Nan::New(V8::GetVersion()).ToLocalChecked());
    Nan::Set(target, Nan::New("versions").ToLocalChecked(), versions);
}

NAN_MODULE_WORKER_ENABLED(zipfile, init);
