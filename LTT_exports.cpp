// C wrapper exports with stable names and cdecl calling convention.
// These are simple thin wrappers that call the internal C++ implementations.

#include "LTT_internal.h"
#include <string>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <ctime>

static void DebugLogLocal(const char* msg)
{
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
}

// existing wrappers
extern "C" __declspec(dllexport) void __cdecl FileHandler()
{
    FileHandlerImpl();
}

extern "C" __declspec(dllexport) void __cdecl LTTWrite()
{
    LTTWriteImpl();
}

extern "C" __declspec(dllexport) void __cdecl LTT_main()
{
    LTTMainImpl();
}

// new exported config wrappers
extern "C" __declspec(dllexport) bool __cdecl LoadLTTConfig()
{
    return LoadLTTConfigFromFile();
}

extern "C" __declspec(dllexport) bool __cdecl SaveLTTConfig()
{
    return SaveLTTConfigToFile();
}

extern "C" __declspec(dllexport) const char* __cdecl GetLTTConfigJson()
{
    static std::string s;
    s = SerializeLTTConfig();
    return s.c_str();
}

extern "C" __declspec(dllexport) void __cdecl FileHandlerWithPath(const char* cpath)
{
    try
    {
        if (!cpath) { DebugLogLocal("FileHandlerWithPath: null path"); return; }
        std::filesystem::path p(cpath);
        auto dir = p.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);

        if (!std::filesystem::exists(p))
        {
            std::ofstream ofs(p);
            if (ofs)
            {
                ofs << "";
                ofs.close();
                DebugLogLocal((std::string("Created file: ") + p.string()).c_str());
            }
            else
            {
                DebugLogLocal((std::string("Failed to create file: ") + p.string()).c_str());
            }
        }
        else
        {
            DebugLogLocal((std::string("File exists: ") + p.string()).c_str());
        }
    }
    catch (const std::exception& e)
    {
        DebugLogLocal((std::string("FileHandlerWithPath exception: ") + e.what()).c_str());
    }
}

extern "C" __declspec(dllexport) void __cdecl LTTWriteWithPath(const char* cpath)
{
    try
    {
        if (!cpath) { DebugLogLocal("LTTWriteWithPath: null path"); return; }
        std::filesystem::path p(cpath);
        auto dir = p.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);

        std::ofstream ofs(p, std::ios::trunc);
        if (ofs)
        {
            std::time_t t = std::time(nullptr);
            char buf[64];
            ctime_s(buf, sizeof(buf), &t);
            ofs << buf;
            ofs.close();
            DebugLogLocal((std::string("Wrote time to file: ") + p.string()).c_str());
        }
        else
        {
            DebugLogLocal((std::string("Failed to open file for writing: ") + p.string()).c_str());
        }
    }
    catch (const std::exception& e)
    {
        DebugLogLocal((std::string("LTTWriteWithPath exception: ") + e.what()).c_str());
    }
}

// Append these exports to LTT_exports.cpp

extern "C" __declspec(dllexport) int __cdecl LTT_GetThresholdYears()   { return GetThresholdYears(); }
extern "C" __declspec(dllexport) void __cdecl LTT_SetThresholdYears(int v) { SetThresholdYears(v); }

extern "C" __declspec(dllexport) int __cdecl LTT_GetThresholdMonths()  { return GetThresholdMonths(); }
extern "C" __declspec(dllexport) void __cdecl LTT_SetThresholdMonths(int v) { SetThresholdMonths(v); }

extern "C" __declspec(dllexport) int __cdecl LTT_GetThresholdDays()    { return GetThresholdDays(); }
extern "C" __declspec(dllexport) void __cdecl LTT_SetThresholdDays(int v) { SetThresholdDays(v); }

extern "C" __declspec(dllexport) int __cdecl LTT_GetThresholdHours()   { return GetThresholdHours(); }
extern "C" __declspec(dllexport) void __cdecl LTT_SetThresholdHours(int v) { SetThresholdHours(v); }

extern "C" __declspec(dllexport) int __cdecl LTT_GetThresholdMinutes() { return GetThresholdMinutes(); }
extern "C" __declspec(dllexport) void __cdecl LTT_SetThresholdMinutes(int v) { SetThresholdMinutes(v); }

extern "C" __declspec(dllexport) int __cdecl LTT_GetThresholdSeconds() { return GetThresholdSeconds(); }
extern "C" __declspec(dllexport) void __cdecl LTT_SetThresholdSeconds(int v) { SetThresholdSeconds(v); }

// Directory / path
extern "C" __declspec(dllexport) const char* __cdecl LTT_GetDirectoryPath() { return GetDirectoryPath(); }
extern "C" __declspec(dllexport) void __cdecl        LTT_SetDirectoryPath(const char* p) { SetDirectoryPath(p); }

extern "C" __declspec(dllexport) const char* __cdecl LTT_GetPath() { return GetPath(); }
extern "C" __declspec(dllexport) void __cdecl        LTT_SetPath(const char* p) { SetPath(p); }

// Custom scripts
extern "C" __declspec(dllexport) int __cdecl    LTT_GetCustomScriptCount() { return GetCustomScriptCount(); }
extern "C" __declspec(dllexport) const char* __cdecl LTT_GetCustomScriptAt(int idx) { return GetCustomScriptAt(idx); }
extern "C" __declspec(dllexport) void __cdecl   LTT_AddCustomScript(const char* s) { AddCustomScript(s); }
extern "C" __declspec(dllexport) void __cdecl   LTT_ClearCustomScripts() { ClearCustomScripts(); }