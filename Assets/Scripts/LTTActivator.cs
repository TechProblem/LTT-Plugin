using UnityEngine;

// Attach this to an always-active helper GameObject in the scene.
// Assign `targetToActivate` in the inspector to the GameObject you want activated by the native plugin.
public class LTTActivator : MonoBehaviour
{
    public GameObject targetToActivate;

    // UnitySendMessage will call this method when the plugin triggers activation.
    // The message string will contain the idle-time description.
    public void OnNativeActivate(string msg)
    {
        Debug.Log("LTTActivator received activation message: " + msg);
        if (targetToActivate != null)
        {
            targetToActivate.SetActive(true);
            Debug.Log("Activated target: " + targetToActivate.name);
        }
        else
        {
            Debug.Log("LTTActivator: no target assigned to activate.");
        }
    }
}
