using System;
using System.Runtime.InteropServices;
using UnityEngine;

public static class LTTNative
{
    // Core exports
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)]
    private static extern void FileHandler();

    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)]
    private static extern void LTT_main();

    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)]
    private static extern void LTTWrite();

    // Path-taking helpers
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void FileHandlerWithPath([MarshalAs(UnmanagedType.LPStr)] string path);

    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void LTTWriteWithPath([MarshalAs(UnmanagedType.LPStr)] string path);

    // Config
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    private static extern bool LoadLTTConfig();

    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    private static extern bool SaveLTTConfig();

    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr GetLTTConfigJson();

    // Thresholds
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetThresholdYears();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetThresholdYears(int v);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetThresholdMonths();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetThresholdMonths(int v);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetThresholdDays();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetThresholdDays(int v);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetThresholdHours();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetThresholdHours(int v);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetThresholdMinutes();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetThresholdMinutes(int v);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetThresholdSeconds();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetThresholdSeconds(int v);

    // Paths
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern IntPtr LTT_GetDirectoryPath();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void LTT_SetDirectoryPath([MarshalAs(UnmanagedType.LPStr)] string p);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern IntPtr LTT_GetPath();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void LTT_SetPath([MarshalAs(UnmanagedType.LPStr)] string p);

    // Custom scripts
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern int LTT_GetCustomScriptCount();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern IntPtr LTT_GetCustomScriptAt(int idx);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void LTT_AddCustomScript([MarshalAs(UnmanagedType.LPStr)] string s);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_ClearCustomScripts();

    // Runtime flags and idle-time helpers
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern bool LTT_GetRunCustom();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern void LTT_SetRunCustom([MarshalAs(UnmanagedType.I1)] bool v);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern long LTT_GetIdleSeconds();
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl)] private static extern IntPtr LTT_GetIdleTimeString();
    // Unity activation helpers
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)] private static extern void LTT_SetUnityTarget([MarshalAs(UnmanagedType.LPStr)] string name);
    [DllImport("LTT", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)] private static extern void LTT_SetUnityMethod([MarshalAs(UnmanagedType.LPStr)] string name);

    // Public-friendly API
    private static string PtrToString(IntPtr p) => p == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(p);

    public static void EnsureFile() => FileHandler();
    public static void EnsureFile(string path) => FileHandlerWithPath(path);

    public static void WriteTime() => LTTWrite();
    public static void WriteTime(string path) => LTTWriteWithPath(path);

    public static bool LoadConfig() => LoadLTTConfig();
    public static bool SaveConfig() => SaveLTTConfig();
    public static string GetConfigJson() => PtrToString(GetLTTConfigJson());

    public static int ThresholdYears { get => LTT_GetThresholdYears(); set => LTT_SetThresholdYears(value); }
    public static int ThresholdMonths { get => LTT_GetThresholdMonths(); set => LTT_SetThresholdMonths(value); }
    public static int ThresholdDays { get => LTT_GetThresholdDays(); set => LTT_SetThresholdDays(value); }
    public static int ThresholdHours { get => LTT_GetThresholdHours(); set => LTT_SetThresholdHours(value); }
    public static int ThresholdMinutes { get => LTT_GetThresholdMinutes(); set => LTT_SetThresholdMinutes(value); }
    public static int ThresholdSeconds { get => LTT_GetThresholdSeconds(); set => LTT_SetThresholdSeconds(value); }

    public static string DirectoryPath { get => PtrToString(LTT_GetDirectoryPath()); set => LTT_SetDirectoryPath(value); }
    public static string Path { get => PtrToString(LTT_GetPath()); set => LTT_SetPath(value); }

    public static string[] GetCustomScripts()
    {
        int n = LTT_GetCustomScriptCount();
        if (n <= 0) return Array.Empty<string>();
        var arr = new string[n];
        for (int i = 0; i < n; ++i) arr[i] = PtrToString(LTT_GetCustomScriptAt(i));
        return arr;
    }
    public static void AddCustomScript(string s) => LTT_AddCustomScript(s);
    public static void ClearCustomScripts() => LTT_ClearCustomScripts();

    // Expose runCustom and idle-time to managed code
    public static bool RunCustom { get => LTT_GetRunCustom(); set => LTT_SetRunCustom(value); }
    public static long IdleSeconds { get => LTT_GetIdleSeconds(); }
    public static string IdleTimeString { get => PtrToString(LTT_GetIdleTimeString()); }

    // Set the GameObject name and method UnitySendMessage will call
    public static void SetUnityTarget(string name) => LTT_SetUnityTarget(name);
    public static void SetUnityMethod(string name) => LTT_SetUnityMethod(name);

    // Developer helper: log idle time to Unity console and indicate whether scripts will run
    public static void DevLogIdle()
    {
        Debug.Log($"LTT DevLog - Player idle for: {IdleTimeString}");
        Debug.Log($"LTT DevLog - runCustom = {RunCustom}");
    }
}