using System.Runtime.InteropServices;
using UnityEngine;

// Attach this to an always-active helper GameObject in the scene.
// Assign `targetToActivate` in the inspector to the GameObject you want activated by the native plugin.
public class LTTActivator : MonoBehaviour
{

    public GameObject TargetToActivate;
    public bool RunCustom = LTTNative.RunCustom;

    public int GetThresholdyears;
    public int GetThresholdmonths;
    public int GetThresholddays;
    public int GetThresholdhours;
    public int GetThresholdminutes;
    public int GetThresholdseconds;
    // Return the values set on this component (inspector fields).
    // The previous implementation returned the native plugin thresholds which
    // made comparisons always compare the same values.
    public int GetThresholdYears() => GetThresholdyears;
    public int GetThresholdMonths() => GetThresholdmonths;
    public int GetThresholdDays() => GetThresholddays;
    public int GetThresholdHours() => GetThresholdhours;
    public int GetThresholdMinutes() => GetThresholdminutes;
    public int GetThresholdSeconds() => GetThresholdseconds;

    private long GetThresholdTotalSeconds()
    {
        // Approximate conversions for months/years
        long seconds = 0;
        seconds += (long)GetThresholdSeconds();
        seconds += (long)GetThresholdMinutes() * 60L;
        seconds += (long)GetThresholdHours() * 3600L;
        seconds += (long)GetThresholdDays() * 86400L;
        seconds += (long)GetThresholdMonths() * 30L * 86400L; // 30 days/month
        seconds += (long)GetThresholdYears() * 365L * 86400L; // 365 days/year
        return seconds;
    }


    // UnitySendMessage will call this method when the plugin triggers activation.
    // The message string will contain the idle-time description.
    public void OnNativeActivate(string msg)
    {
        Debug.Log("LTTActivator received activation message: " + msg);
        if (TargetToActivate != null)
        {
            LTTNative.DevLogIdle();
            if (RunCustom)
            {
                long idle = LTTNative.IdleSeconds;
                long threshold = GetThresholdTotalSeconds();
                if (idle >= threshold)
                {
                    Debug.Log($"LTTActivator: RunCustom is true, idle {LTTNative.IdleTimeString} >= threshold, activating target.");
                    TargetToActivate.SetActive(true);
                    Debug.Log("Activated target: " + TargetToActivate.name);
                }
                else
                {
                    Debug.Log($"LTTActivator: RunCustom is true, idle {LTTNative.IdleTimeString} has not exceeded threshold of {GetThresholdYears()}y {GetThresholdMonths()}m {GetThresholdDays()}d {GetThresholdHours()}h {GetThresholdMinutes()}m {GetThresholdSeconds()}s, not activating target.");
                }
            }
            else
            {
                Debug.Log("LTTActivator: RunCustom is false, activating target immediately.");
                TargetToActivate.SetActive(true);
            }
        }
        else
        {
            Debug.Log("LTTActivator: no target assigned to activate.");
        }
    }

    private void Start()
    {
        LTTNative.SetUnityMethod("OnNativeActivate");
        if (TargetToActivate != null)
        {
            LTTNative.SetUnityTarget(TargetToActivate.name);
            OnNativeActivate("LTTActivator started, registered target '" + TargetToActivate.name + "' method 'OnNativeActivate' runCustom=" + RunCustom);
        }
        else
        {
            OnNativeActivate("LTTActivator started, no target assigned. runCustom=" + RunCustom);
        }
    }
    private void OnApplicationQuit()
    {
        LTTNative.WriteTime();
    }
}
