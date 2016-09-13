
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
    std::string buffer;
    buffer.resize(size);
    DWORD result = WideCharToMultiByte(CP_ACP,
                                       0,
                                       s.c_str(),
                                       -1,
                                       &buffer[0],
                                       size,
                                       NULL,
                                       NULL);
    if (result == 0) {
      // This should never happen.
      fprintf(stderr, "Could not convert arguments to utf8.");
    }
    return buffer;
}

std::wstring utf8ToWide( std::string const& s )
{
    int ws_len, r;
    ws_len = MultiByteToWideChar(CP_UTF8,
                               0,
                               s.c_str(),
                               -1,
                               NULL,
                               0);
    std::wstring buffer;
    buffer.resize(ws_len * sizeof(WCHAR));
    r = MultiByteToWideChar(CP_UTF8,
                          0,
                          s.c_str(),
                          -1,
                          &buffer[0],
                          ws_len);
    if (r != ws_len) {
      // This should never happen.
      fprintf(stderr, "Could not do anything useful.");
      exit(1);
    }
}