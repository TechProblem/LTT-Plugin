using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LTTPlugin : MonoBehaviour
{

    // Use persistentDataPath so it works in Editor and builds
    public string directoryPath;
    public string path;

    public int Threshold_years;
    public int Threshold_months;
    public int Threshold_days;
    public int Threshold_hours;
    public int Threshold_minutes;
    public int Threshold_seconds;
    public GameObject Custom_Scripts;

    

    void Awake()
    {
        DontDestroyOnLoad(this.gameObject);
        // initialize paths at runtime
        directoryPath = Path.Combine(Application.persistentDataPath, "LTTFiles");
        if (!Directory.Exists(directoryPath)) Directory.CreateDirectory(directoryPath);
        path = Path.Combine(directoryPath, "LTTTime.txt");

        Debug.Log("LTTPlugin: Using directory path: " + directoryPath);
        
        try
        {
            LTTNative.DirectoryPath = directoryPath;
            LTTNative.Path = path;

            // ensure file exists in native code
            LTTNative.EnsureFile(path); // calls FileHandlerWithPath
           // LTT_main();
        }
        catch (DllNotFoundException e)
        {
            Debug.LogError("LTTPlugin: DllNotFoundException: " + e.Message);
        }
    }

    private void OnApplicationQuit()
    {
        try
        {
            LTTNative.WriteTime(path);
            //LTTWrite();
            Debug.Log("Application is quitting, time written to file.");
        }
        catch (DllNotFoundException e)
        {
            Debug.LogError("LTTPlugin: DllNotFoundException on quit: " + e.Message);
        }
    }

    void Start() { }
    void Update() { }
}