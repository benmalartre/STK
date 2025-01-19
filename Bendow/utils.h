
#ifndef TX_UTILS_H
#define TX_UTILS_H

#include <string>


#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

std::string getExecutablePath()
{
#ifdef _WIN32
    wchar_t buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring path(buffer);
    return std::string(path.begin(), path.end());
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
#endif
}

#endif