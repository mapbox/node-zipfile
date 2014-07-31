
#include <Windows.h>
#include <string>
#include <cstdio>

std::string wstring2string(const std::wstring& s)
{
    DWORD size = WideCharToMultiByte(CP_UTF8,
                                     0,
                                     s.c_str(),
                                     -1,
                                     NULL,
                                     0,
                                     NULL,
                                     NULL);
    if (size == 0) {
      // This should never happen.
      fprintf(stderr, "Could not convert arguments to utf8.");
    }
    char * buf_ptr = new char [size];
    DWORD result = WideCharToMultiByte(CP_ACP,
                                       0,
                                       s.c_str(),
                                       -1,
                                       buf_ptr,
                                       size,
                                       NULL,
                                       NULL);
    if (result == 0) {
      // This should never happen.
      fprintf(stderr, "Could not convert arguments to utf8.");
    }
    std::string r(buf_ptr);
    delete buf_ptr;
    return r;
}

std::wstring utf8ToWide( std::string const& s )
{
    int ws_len, r;
    WCHAR* ws;
    ws_len = MultiByteToWideChar(CP_UTF8,
                               0,
                               s.c_str(),
                               -1,
                               NULL,
                               0);
    ws = new wchar_t [ws_len * sizeof(WCHAR)];
    if (ws == NULL) {
      // This should never happen.
      fprintf(stderr, "Could not convert arguments from utf8.");
      exit(1);
    }
    r = MultiByteToWideChar(CP_UTF8,
                          0,
                          s.c_str(),
                          -1,
                          ws,
                          ws_len);
    if (r != ws_len) {
      // This should never happen.
      fprintf(stderr, "Could not do anything useful.");
      exit(1);
    }
    std::wstring rt(ws);
    delete ws;
    return rt;
}