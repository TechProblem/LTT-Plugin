// #include "pch.h"
#include "LTT.h"
#include <windows.h>
#include <string>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ctime>


static std::string GetModuleDirectory()
{
    char path[MAX_PATH] = {0};
    HMODULE hm = NULL;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(&GetModuleDirectory), &hm))
    {
        GetModuleFileNameA(hm, path, MAX_PATH);
        std::string s(path);
        auto pos = s.find_last_of("\\/");
        if (pos != std::string::npos) s = s.substr(0, pos);
        return s;
    }
    GetCurrentDirectoryA(MAX_PATH, path);
    return std::string(path);
}

static std::string MakeTextPath()
{
    auto dir = GetModuleDirectory();
    std::string file = dir + "\\Text.txt";
    return file;
}

static void DebugLog(const char* msg)
{
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
}

// Removed duplicate exported definitions.
// Use LTT_exports.cpp (wrappers) -> LTT_internal.cpp (implementations)
// to provide the DLL exports and internal implementations.