#pragma once

#include <string>
#include <vector>
struct UnityEngine_ScriptableObjects; // Forward declaration for UnityEngine.ScriptableObjects

// internal configuration kept inside the plugin
struct LTTConfig
{
    int threshold_years = 0;
    int threshold_months = 0;
    int threshold_days = 0;
    int threshold_hours = 0;
    int threshold_minutes = 0;
    int threshold_seconds = 0;

    std::string Directory_path;
    std::string path;
    std::vector<std::string> custom_scripts;

    void reset()
    {
        threshold_years = threshold_months = threshold_days = 0;
        threshold_hours = threshold_minutes = threshold_seconds = 0;
        Directory_path.clear();
        path.clear();
        custom_scripts.clear();
    }
};


void ScriptRunner(UnityEngine_ScriptableObjects* scriptableObject);

// internal implementation prototypes
void FileHandlerImpl();
void LTTMainImpl();
void LTTWriteImpl();
void LTTReadImpl();

// internal config APIs (C++ use)
const LTTConfig& GetLTTConfig();
bool LoadLTTConfigFromFile();
bool SaveLTTConfigToFile();
std::string SerializeLTTConfig(); // returns a textual (JSON-like) representation

// Threshold accessors
int GetThresholdYears();
void SetThresholdYears(int v);

int GetThresholdMonths();
void SetThresholdMonths(int v);

int GetThresholdDays();
void SetThresholdDays(int v);

int GetThresholdHours();
void SetThresholdHours(int v);

int GetThresholdMinutes();
void SetThresholdMinutes(int v);

int GetThresholdSeconds();
void SetThresholdSeconds(int v);

// Directory/path accessors
const char* GetDirectoryPath();
void SetDirectoryPath(const char* p);

const char* GetPath();
void SetPath(const char* p);

// Custom scripts helpers
int GetCustomScriptCount();
const char* GetCustomScriptAt(int index);
void AddCustomScript(const char* s);
void ClearCustomScripts();

// runtime flags access
bool GetRunCustom();
void SetRunCustom(bool v);

// Idle time helpers (seconds since the LTT time file was last written)
long long GetIdleSeconds();
// Returns a human-readable duration string (owned by the callee)
const char* GetIdleTimeString();

// Unity activation target helpers
void SetUnityTarget(const char* name);
void SetUnityMethod(const char* name);

// NEW: checking helpers
// Returns true if the timestamp in the configured path is older than the thresholds
extern bool IsFileTimePastThreshold;
extern bool runCustom;

