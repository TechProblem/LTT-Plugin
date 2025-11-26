#include "LTT_internal.h"
#include <windows.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <ctime>

// --- original content preserved ---
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
    std::string file = dir + "\\Text.txt";
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

void LTTMainImpl()
{
    DebugLog("LTT_main called (impl).");
    FileHandlerImpl();
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

// Append these implementations to LTT_internal.cpp (after g_config definition)

#include <string>

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