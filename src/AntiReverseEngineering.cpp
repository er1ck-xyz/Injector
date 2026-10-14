#include "AntiReverseEngineering.h"
#include "Logger.h"
#include <tlhelp32.h>
#include <psapi.h>

bool AntiReverseEngineering::IsDebuggerPresent() {
    return ::IsDebuggerPresent() == TRUE;
}

bool AntiReverseEngineering::IsDebuggerAttached() {
    BOOL isDebugged = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugged);
    return isDebugged == TRUE;
}

bool AntiReverseEngineering::IsUnderDynamoRIO() {
    // Check for DynamoRIO DLL
    HMODULE hDynamoRIO = GetModuleHandleA("dynamorio.dll");
    if (hDynamoRIO) {
        Logger::Warning("DynamoRIO detectado!");
        return true;
    }

    // Check environment variables
    const char* envVar = getenv("DYNAMORIO_HOME");
    if (envVar) {
        Logger::Warning("Variável DYNAMORIO_HOME encontrada!");
        return true;
    }

    // Check for mutex
    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE,
                              "Global\\dynamorio_mutex");
    if (hMutex) {
        CloseHandle(hMutex);
        Logger::Warning("Mutex DynamoRIO detectado!");
        return true;
    }

    return false;
}

bool AntiReverseEngineering::IsUnderPinTool() {
    // Check for pin.exe as parent
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
    DWORD currentPID = GetCurrentProcessId();
    DWORD parentPID = 0;

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (pe.th32ProcessID == currentPID) {
                parentPID = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);

    if (parentPID == 0) return false;

    // Get parent process name
    HANDLE hParent = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE, parentPID);
    if (hParent) {
        char parentName[MAX_PATH] = {0};
        GetModuleFileNameExA(hParent, NULL, parentName, MAX_PATH);

        if (strstr(parentName, "pin.exe")) {
            CloseHandle(hParent);
            Logger::Warning("Pin Tool detectado!");
            return true;
        }

        CloseHandle(hParent);
    }

    return false;
}

bool AntiReverseEngineering::IsUnderDrMemory() {
    HMODULE hDrMemory = GetModuleHandleA("drmemory.dll");
    if (hDrMemory) {
        Logger::Warning("DrMemory detectado!");
        return true;
    }

    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE,
                              "Global\\drmemory_mutex");
    if (hMutex) {
        CloseHandle(hMutex);
        Logger::Warning("Mutex DrMemory detectado!");
        return true;
    }

    return false;
}

bool AntiReverseEngineering::IsRunningInVM() {
    if (DetectVirtualBox() || DetectVMware() || HasHypervisor()) {
        Logger::Warning("VM detectada!");
        return true;
    }
    return false;
}

bool AntiReverseEngineering::HasHypervisor() {
    int cpuid[4] = {0};

    __cpuid(cpuid, 1);

    // Hypervisor Present Bit (ECX bit 31)
    if (cpuid[2] & 0x80000000) {
        Logger::Warning("Hypervisor Present Bit detectado!");
        return true;
    }

    return false;
}

bool AntiReverseEngineering::DetectVirtualBox() {
    // Check registry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

        char systemIdentifier[256] = {0};
        DWORD size = sizeof(systemIdentifier);

        if (RegQueryValueExA(hKey, "SystemIdentifier", NULL, NULL,
            (LPBYTE)systemIdentifier, &size) == ERROR_SUCCESS) {

            if (strstr(systemIdentifier, "VBOX")) {
                RegCloseKey(hKey);
                Logger::Warning("VirtualBox detectada!");
                return true;
            }
        }

        RegCloseKey(hKey);
    }

    // Check driver files
    if (GetFileAttributesA("C:\\Windows\\System32\\VBoxMouse.sys") !=
        INVALID_FILE_ATTRIBUTES) {
        Logger::Warning("VBoxMouse.sys encontrado!");
        return true;
    }

    // Check device
    HANDLE hDevice = CreateFileA("\\\\.\\VBoxGuest", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
        Logger::Warning("VBoxGuest device encontrado!");
        return true;
    }

    return false;
}

bool AntiReverseEngineering::DetectVMware() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

        char systemIdentifier[256] = {0};
        DWORD size = sizeof(systemIdentifier);

        if (RegQueryValueExA(hKey, "SystemIdentifier", NULL, NULL,
            (LPBYTE)systemIdentifier, &size) == ERROR_SUCCESS) {

            if (strstr(systemIdentifier, "VMWARE")) {
                RegCloseKey(hKey);
                Logger::Warning("VMware detectada!");
                return true;
            }
        }

        RegCloseKey(hKey);
    }

    // Check driver
    if (GetFileAttributesA("C:\\Windows\\System32\\vmmouse.sys") !=
        INVALID_FILE_ATTRIBUTES) {
        Logger::Warning("vmmouse.sys encontrado!");
        return true;
    }

    // Check device
    HANDLE hDevice = CreateFileA("\\\\.\\vmci", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
        Logger::Warning("VMCI device encontrado!");
        return true;
    }

    return false;
}

bool AntiReverseEngineering::DetectProcessMonitor() {
    const char* tools[] = {"Procmon.exe", "procmon64.exe", "Procmon64.exe"};

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};

    if (Process32First(hSnapshot, &pe)) {
        do {
            for (const char* tool : tools) {
                if (_stricmp(pe.szExeFile, tool) == 0) {
                    CloseHandle(hSnapshot);
                    Logger::Warning("Process Monitor detectado!");
                    return true;
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return false;
}

bool AntiReverseEngineering::DetectWireshark() {
    const char* tools[] = {"Wireshark.exe", "wireshark.exe"};

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};

    if (Process32First(hSnapshot, &pe)) {
        do {
            for (const char* tool : tools) {
                if (_stricmp(pe.szExeFile, tool) == 0) {
                    CloseHandle(hSnapshot);
                    Logger::Warning("Wireshark detectado!");
                    return true;
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return false;
}

bool AntiReverseEngineering::DetectIDA() {
    const char* tools[] = {"ida.exe", "ida64.exe", "IDA.exe", "IDA64.exe"};

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};

    if (Process32First(hSnapshot, &pe)) {
        do {
            for (const char* tool : tools) {
                if (_stricmp(pe.szExeFile, tool) == 0) {
                    CloseHandle(hSnapshot);
                    Logger::Warning("IDA Pro detectado!");
                    return true;
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return false;
}

bool AntiReverseEngineering::ProtectAgainstInstrumentation() {
    // Check multiple detection methods
    bool detected = false;

    if (IsDebuggerPresent()) {
        Logger::Warning("Debugger detectado");
        detected = true;
    }

    if (IsUnderDynamoRIO()) {
        detected = true;
    }

    if (IsUnderPinTool()) {
        detected = true;
    }

    if (IsRunningInVM()) {
        detected = true;
    }

    if (DetectProcessMonitor()) {
        detected = true;
    }

    return detected;
}
