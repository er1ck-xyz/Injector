#pragma once
#include <windows.h>
#include <chrono>

class BehavioralEvasion {
public:
    // Detect EDR by behavior
    static bool DetectEDRBehavior();

    // Check API call timing
    static bool CheckAPICallTiming();

    // Monitor handle creation
    static DWORD GetHandleCount();

    // Monitor thread creation
    static int GetThreadCount(DWORD pid);

    // Wait for optimal injection window
    static void WaitForOptimalInjectionWindow();

    // Get CPU usage estimate
    static DWORD GetCPUUsageEstimate();

    // Get disk activity estimate
    static DWORD GetDiskActivityEstimate();
};
