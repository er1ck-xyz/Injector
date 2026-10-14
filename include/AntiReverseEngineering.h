#pragma once
#include <windows.h>
#include <string>

class AntiReverseEngineering {
public:
    // Debugger detection
    static bool IsDebuggerPresent();
    static bool IsDebuggerAttached();

    // Tools detection
    static bool IsUnderDynamoRIO();
    static bool IsUnderPinTool();
    static bool IsUnderDrMemory();
    static bool IsRunningInVM();

    // Virtualization detection
    static bool HasHypervisor();
    static bool DetectVirtualBox();
    static bool DetectVMware();

    // Monitoring tools
    static bool DetectProcessMonitor();
    static bool DetectWireshark();
    static bool DetectIDA();

    // Anti-ptrace style
    static bool ProtectAgainstInstrumentation();
};
