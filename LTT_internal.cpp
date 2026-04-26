#include "LTT_internal.h"
#include <windows.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iostream>
extern struct UnityEngine_ScriptableObjects; // Forward declaration for UnityEngine.ScriptableObjects

// helper to dynamically resolve UnitySendMessage at runtime (avoids link errors)
static void CallUnitySendMessage(const char* obj, const char* method, const char* msg)
{
    HMODULE hm = GetModuleHandleA("UnityPlayer.dll");
    if (!hm) hm = GetModuleHandleA("UnityEditor.dll");
    if (!hm) return; // not running inside Unity player/editor
    typedef void(__cdecl *USM_t)(const char*, const char*, const char*);
    auto fn = (USM_t)GetProcAddress(hm, "UnitySendMessage");
    if (fn) fn(obj, method, msg);
}

// forward declarations for unity target strings (defined later)
extern std::string g_unity_target;
extern std::string g_unity_method;



// --- original content ---
static std::string GetModuleDirectory()
{
    char path[MAX_PATH] = { 0 };
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
    std::string file = dir + "\\LTTTime.txt";
    return file;
}

static void DebugLog(const char* msg)
{
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
}

// Internal implementations (no extern "C", no dllexport)
void FileHandlerImpl()
{
    try
    {
        std::string path = MakeTextPath();
        if (!std::filesystem::exists(path))
        {
            std::ofstream ofs(path);
            if (ofs)
            {
                ofs << "";
                ofs.close();
                DebugLog((std::string("Created file: ") + path).c_str());
            }
            else
            {
                DebugLog((std::string("Failed to create file: ") + path).c_str());
            }
        }
        else
        {
            DebugLog((std::string("File exists: ") + path).c_str());
        }
    }
    catch (const std::exception& e)
    {
        DebugLog((std::string("FileHandlerImpl exception: ") + e.what()).c_str());
    }
}
void ScriptRunner(UnityEngine_ScriptableObjects* scriptableObject)
{
    if (!scriptableObject) { DebugLog("ScriptRunner: null scriptableObject"); return; }
    std::string s = std::to_string(reinterpret_cast<std::uintptr_t>(scriptableObject));
    system(s.c_str());
    DebugLog((std::string("Executing custom script: ") + s).c_str());

}
void LTTMainImpl()
{
    LTTReadImpl();
    FileHandlerImpl();
    DebugLog("LTT_main called (impl).");

    if (IsFileTimePastThreshold)
    {
        // log how long the player has been idle and indicate script execution
        DebugLog((std::string("Player idle for: ") + GetIdleTimeString()).c_str());
        DebugLog("File time is past threshold! Scripts will be executed if runCustom is true.");
        // If configured, call into Unity to activate a game object.
        if (runCustom && !g_unity_target.empty() && !g_unity_method.empty())
        {
            try {
                CallUnitySendMessage(g_unity_target.c_str(), g_unity_method.c_str(), GetIdleTimeString());
                DebugLog((std::string("UnitySendMessage called to ") + g_unity_target + ":" + g_unity_method).c_str());
            }
            catch (...) {
                DebugLog("UnitySendMessage call failed (exception)");
            }
        }
    }

}

void LTTReadImpl()
{
    try
    {
        std::string path = MakeTextPath();
        if (std::filesystem::exists(path))
        {
            std::ifstream ifs(path);
            if (ifs)
            {
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();
                DebugLog((std::string("Read from file: ") + path + "\nContent:\n" + content).c_str());

                // Compute last-write time and compare against configured thresholds
                try {
                    auto ftime = std::filesystem::last_write_time(path);
                    // convert file_time_type to system_clock::time_point
                    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
                    time_t file_time_t = std::chrono::system_clock::to_time_t(sctp);
                    time_t now_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                    // build threshold in seconds (approximate months=30 days, years=365 days)
                    long long thresh_secs = 0;
                    thresh_secs += static_cast<long long>(GetThresholdYears())   * 365LL * 24LL * 3600LL;
                    thresh_secs += static_cast<long long>(GetThresholdMonths())  * 30LL  * 24LL * 3600LL;
                    thresh_secs += static_cast<long long>(GetThresholdDays())    * 24LL  * 3600LL;
                    thresh_secs += static_cast<long long>(GetThresholdHours())   * 3600LL;
                    thresh_secs += static_cast<long long>(GetThresholdMinutes()) * 60LL;
                    thresh_secs += static_cast<long long>(GetThresholdSeconds());

                    if (thresh_secs <= 0) {
                        IsFileTimePastThreshold = false;
                        DebugLog("Thresholds are zero or negative; not past threshold.");
                    } else {
                        time_t expiry = static_cast<time_t>(file_time_t + thresh_secs);
                        if (now_t >= expiry) {
                            IsFileTimePastThreshold = true;
                            DebugLog("File timestamp is older than threshold: IsFileTimePastThreshold = true");
                        } else {
                            IsFileTimePastThreshold = false;
                            DebugLog("File timestamp is not older than threshold: IsFileTimePastThreshold = false");
                        }
                    }
                }
                catch (const std::exception& e) {
                    DebugLog((std::string("Time comparison error: ") + e.what()).c_str());
                }

            }
            else
            {
                DebugLog((std::string("Failed to open file for reading: ") + path).c_str());
            }
        }
        else
        {
            DebugLog((std::string("File does not exist: ") + path).c_str());
        }
    }
    catch (const std::exception& e)
    {
        DebugLog((std::string("LTTReadImpl exception: ") + e.what()).c_str());
    }
}

void LTTWriteImpl()
{
    try
    {
        std::string path = MakeTextPath();
        std::ofstream ofs(path, std::ios::trunc);
        if (ofs)
        {
            std::time_t t = std::time(nullptr);
            char buf[64];
            ctime_s(buf, sizeof(buf), &t);
            ofs << buf;
            ofs.close();
            DebugLog((std::string("Wrote time to file: ") + path).c_str());
        }
        else
        {
            DebugLog((std::string("Failed to open file for writing: ") + path).c_str());
        }
    }
    catch (const std::exception& e)
    {
        DebugLog((std::string("LTTWriteImpl exception: ") + e.what()).c_str());
    }
}
// --- end original content ---

// Added config implementation
#include <vector>
#include <sstream>
#include <algorithm>

// static config kept inside the plugin
static LTTConfig g_config;

// Definitions for flags declared as extern in header
bool IsFileTimePastThreshold = false;
bool runCustom = false;

// Unity target/method to call when activating
static std::string g_unity_target;
static std::string g_unity_method;

void SetUnityTarget(const char* name) { g_unity_target = (name ? name : ""); }
void SetUnityMethod(const char* name) { g_unity_method = (name ? name : ""); }

// Return whether native will run custom scripts automatically
bool GetRunCustom() { return runCustom; }
void SetRunCustom(bool v) { runCustom = v; }

// Helper to compute seconds since the timestamp file was last written
long long GetIdleSeconds()
{
    try {
        std::string path = MakeTextPath();
        if (!std::filesystem::exists(path)) return 0;
        auto ftime = std::filesystem::last_write_time(path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
        time_t file_time_t = std::chrono::system_clock::to_time_t(sctp);
        time_t now_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return static_cast<long long>(std::difftime(now_t, file_time_t));
    }
    catch (...) { return 0; }
}

// Return a static, human-readable string describing idle time.
const char* GetIdleTimeString()
{
    static std::string s;
    try {
        long long secs = GetIdleSeconds();
        if (secs <= 0) { s = "0 seconds"; return s.c_str(); }
        long long years = secs / (365LL*24LL*3600LL); secs %= (365LL*24LL*3600LL);
        long long months = secs / (30LL*24LL*3600LL); secs %= (30LL*24LL*3600LL);
        long long days = secs / (24LL*3600LL); secs %= (24LL*3600LL);
        long long hours = secs / 3600LL; secs %= 3600LL;
        long long minutes = secs / 60LL; secs %= 60LL;
        std::ostringstream oss;
        bool first = true;
        auto append = [&](long long v, const char* name){ if (v>0){ if(!first) oss<<", "; oss << v << " " << name; if(v>1) oss<<"s"; first=false; }};
        append(years, "year"); append(months, "month"); append(days, "day"); append(hours, "hour"); append(minutes, "minute"); if(!first) oss<<", "; oss << secs << " second" << (secs!=1?"s":"");
        s = oss.str();
        return s.c_str();
    }
    catch (...) { s = "(error computing idle time)"; return s.c_str(); }
}



// trim helpers
static inline std::string trim(const std::string& s)
{
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return {};
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static bool parse_kv_line(const std::string& line, std::string& key, std::string& value)
{
    auto pos = line.find('=');
    if (pos == std::string::npos) return false;
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    return !key.empty();
}

static std::string MakeConfigPath()
{
    auto dir = GetModuleDirectory();
    return dir + "\\ltt_config.txt";
}

const LTTConfig& GetLTTConfig()
{
    return g_config;
}

bool LoadLTTConfigFromFile()
{
    try
    {
        auto cfgPath = MakeConfigPath();
        if (!std::filesystem::exists(cfgPath)) return false;
        std::ifstream ifs(cfgPath);
        if (!ifs) return false;
        std::string line;
        while (std::getline(ifs, line))
        {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            std::string key, value;
            if (!parse_kv_line(line, key, value)) continue;

            if (key == "threshold_years")   g_config.threshold_years   = std::stoi(value);
            else if (key == "threshold_months")  g_config.threshold_months  = std::stoi(value);
            else if (key == "threshold_days")    g_config.threshold_days    = std::stoi(value);
            else if (key == "threshold_hours")   g_config.threshold_hours   = std::stoi(value);
            else if (key == "threshold_minutes") g_config.threshold_minutes = std::stoi(value);
            else if (key == "threshold_seconds") g_config.threshold_seconds = std::stoi(value);
            else if (key == "Directory_path")    g_config.Directory_path    = value;
            else if (key == "path")              g_config.path              = value;
            else if (key == "custom_script")     g_config.custom_scripts.push_back(value);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool SaveLTTConfigToFile()
{
    try
    {
        auto cfgPath = MakeConfigPath();
        std::ofstream ofs(cfgPath, std::ios::trunc);
        if (!ofs) return false;
        ofs << "# ltt_config.txt - key=value, comments with #\n";
        ofs << "threshold_years="   << g_config.threshold_years   << "\n";
        ofs << "threshold_months="  << g_config.threshold_months  << "\n";
        ofs << "threshold_days="    << g_config.threshold_days    << "\n";
        ofs << "threshold_hours="   << g_config.threshold_hours   << "\n";
        ofs << "threshold_minutes=" << g_config.threshold_minutes << "\n";
        ofs << "threshold_seconds=" << g_config.threshold_seconds << "\n";
        if (!g_config.Directory_path.empty()) ofs << "Directory_path=" << g_config.Directory_path << "\n";
        if (!g_config.path.empty())           ofs << "path=" << g_config.path << "\n";
        for (auto &s : g_config.custom_scripts) ofs << "custom_script=" << s << "\n";
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// serialize to a small JSON-like string for cross-boundary retrieval
std::string SerializeLTTConfig()
{
    std::ostringstream oss;
    oss << "{";
    oss << "\"threshold_years\":" << g_config.threshold_years << ",";
    oss << "\"threshold_months\":" << g_config.threshold_months << ",";
    oss << "\"threshold_days\":" << g_config.threshold_days << ",";
    oss << "\"threshold_hours\":" << g_config.threshold_hours << ",";
    oss << "\"threshold_minutes\":" << g_config.threshold_minutes << ",";
    oss << "\"threshold_seconds\":" << g_config.threshold_seconds << ",";
    oss << "\"Directory_path\":\"" << g_config.Directory_path << "\",";
    oss << "\"path\":\"" << g_config.path << "\",";
    oss << "\"custom_scripts\":[";
    for (size_t i = 0; i < g_config.custom_scripts.size(); ++i)
    {
        if (i) oss << ",";
        oss << "\"" << g_config.custom_scripts[i] << "\"";
    }
    oss << "]";
    oss << "}";
    return oss.str();
}



// Thresholds
int GetThresholdYears()               { return g_config.threshold_years; }
void SetThresholdYears(int v)         { g_config.threshold_years = v; }

int GetThresholdMonths()              { return g_config.threshold_months; }
void SetThresholdMonths(int v)        { g_config.threshold_months = v; }

int GetThresholdDays()                { return g_config.threshold_days; }
void SetThresholdDays(int v)          { g_config.threshold_days = v; }

int GetThresholdHours()               { return g_config.threshold_hours; }
void SetThresholdHours(int v)         { g_config.threshold_hours = v; }

int GetThresholdMinutes()             { return g_config.threshold_minutes; }
void SetThresholdMinutes(int v)       { g_config.threshold_minutes = v; }

int GetThresholdSeconds()             { return g_config.threshold_seconds; }
void SetThresholdSeconds(int v)       { g_config.threshold_seconds = v; }

// Directory/path
const char* GetDirectoryPath()        { return g_config.Directory_path.c_str(); }
void SetDirectoryPath(const char* p)  { g_config.Directory_path = (p ? p : ""); }

const char* GetPath()                 { return g_config.path.c_str(); }
void SetPath(const char* p)           { g_config.path = (p ? p : ""); }

// Custom scripts
int GetCustomScriptCount() {
    return static_cast<int>(g_config.custom_scripts.size());
}
const char* GetCustomScriptAt(int index) {
    if (index < 0 || index >= static_cast<int>(g_config.custom_scripts.size())) return "";
    return g_config.custom_scripts[index].c_str();
}
void AddCustomScript(const char* s) {
    if (s && *s) g_config.custom_scripts.emplace_back(s);
}
void ClearCustomScripts() {
    g_config.custom_scripts.clear();
}
