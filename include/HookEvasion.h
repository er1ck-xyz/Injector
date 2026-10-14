#pragma once
#include <windows.h>
#include <string>

class HookEvasion {
public:
    // Detect if function is hooked
    static bool IsFunctionHooked(LPVOID funcAddress);

    // Get unhookable function address
    static LPVOID GetUnhookedFunctionAddress(
        const char* moduleName,
        const char* functionName
    );

    // Restore hooked function
    static bool RestoreHookedFunction(
        LPVOID funcAddress,
        const char* moduleName,
        const char* functionName
    );

    // Unhook via export table parsing
    static LPVOID UnhookViaExportTable(HMODULE hModule,
                                      const char* functionName);

    // Safe API call
    static HANDLE SafeOpenProcess(DWORD dwDesiredAccess,
                                 BOOL bInheritHandle,
                                 DWORD dwProcessId);
};
