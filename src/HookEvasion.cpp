#include "HookEvasion.h"
#include "Logger.h"
#include <psapi.h>

static BOOL SafeReadBytes(LPVOID dst, LPVOID src, SIZE_T size) {
    __try {
        memcpy(dst, src, size);
        return TRUE;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return FALSE;
    }
}

bool HookEvasion::IsFunctionHooked(LPVOID funcAddress) {
    if (!funcAddress) return false;

    BYTE firstBytes[16] = {0};

    if (!SafeReadBytes(firstBytes, funcAddress, sizeof(firstBytes)))
        return false;

    // Pattern 1: JMP far (FF 25)
    if (firstBytes[0] == 0xFF && firstBytes[1] == 0x25) {
        Logger::Warning("Hook detectado: JMP FAR");
        return true;
    }

    // Pattern 2: MOV RAX + JMP
    if (firstBytes[0] == 0x48 && firstBytes[1] == 0xB8) {
        Logger::Warning("Hook detectado: MOV RAX");
        return true;
    }

    // Pattern 3: PUSH + RET
    if (firstBytes[0] == 0x68) {
        Logger::Warning("Hook detectado: PUSH RET");
        return true;
    }

    // Pattern 4: JMP rel32 (EB)
    if (firstBytes[0] == 0xEB) {
        Logger::Warning("Hook detectado: JMP rel32");
        return true;
    }

    // Pattern 5: JMP rel8 (E9)
    if (firstBytes[0] == 0xE9) {
        Logger::Warning("Hook detectado: JMP rel8");
        return true;
    }

    return false;
}

LPVOID HookEvasion::GetUnhookedFunctionAddress(const char* moduleName,
                                              const char* functionName) {
    HMODULE hOriginalModule = GetModuleHandleA(moduleName);
    if (!hOriginalModule) return NULL;

    // Get module path
    char modulePath[MAX_PATH] = {0};
    GetModuleFileNameA(hOriginalModule, modulePath, MAX_PATH);

    // Load fresh from disk
    HMODULE hFreshModule = LoadLibraryA(modulePath);
    if (!hFreshModule) return NULL;

    // Get function from fresh module (unhookead!)
    LPVOID pUnhookedFunc = GetProcAddress(hFreshModule, functionName);

    if (pUnhookedFunc) {
        Logger::Success("Função unhookada obtida: " + std::string(functionName));
    }

    // Note: NOT freeing hFreshModule to keep it loaded

    return pUnhookedFunc;
}

bool HookEvasion::RestoreHookedFunction(LPVOID funcAddress,
                                       const char* moduleName,
                                       const char* functionName) {
    char systemPath[MAX_PATH] = {0};
    GetSystemDirectoryA(systemPath, MAX_PATH);
    strcat_s(systemPath, MAX_PATH, "\\");
    strcat_s(systemPath, MAX_PATH, moduleName);

    // Read original bytes from disk (simplified)
    // In practice, parse PE and extract original function bytes

    BYTE originalBytes[16] = {0x90};  // NOP as placeholder
    BYTE currentBytes[16] = {0};

    memcpy(currentBytes, funcAddress, sizeof(currentBytes));

    if (memcmp(currentBytes, originalBytes, sizeof(originalBytes)) != 0) {
        Logger::Warning("Função modificada, restorando...");

        DWORD oldProtect = 0;
        VirtualProtect(funcAddress, sizeof(originalBytes),
                      PAGE_EXECUTE_READWRITE, &oldProtect);

        memcpy(funcAddress, originalBytes, sizeof(originalBytes));

        VirtualProtect(funcAddress, sizeof(originalBytes),
                      oldProtect, &oldProtect);

        FlushInstructionCache(GetCurrentProcess(), funcAddress,
                             sizeof(originalBytes));

        Logger::Success("Função restaurada");
        return true;
    }

    return true;
}

LPVOID HookEvasion::UnhookViaExportTable(HMODULE hModule,
                                       const char* functionName) {
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return NULL;

    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)
        ((ULONG_PTR)hModule + pDosHeader->e_lfanew);

    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)
        ((ULONG_PTR)hModule + pNtHeaders->OptionalHeader
        .DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    if (!pExportDir) return NULL;

    PDWORD pAddressOfNames = (PDWORD)
        ((ULONG_PTR)hModule + pExportDir->AddressOfNames);

    for (DWORD i = 0; i < pExportDir->NumberOfNames; i++) {
        const char* pName = (const char*)
            ((ULONG_PTR)hModule + pAddressOfNames[i]);

        if (strcmp(pName, functionName) == 0) {
            PWORD pAddressOfNameOrdinals = (PWORD)
                ((ULONG_PTR)hModule + pExportDir->AddressOfNameOrdinals);

            WORD ordinal = pAddressOfNameOrdinals[i];

            PDWORD pAddressOfFunctions = (PDWORD)
                ((ULONG_PTR)hModule + pExportDir->AddressOfFunctions);

            DWORD funcRVA = pAddressOfFunctions[ordinal];

            Logger::Success("Função encontrada via export table parsing");
            return (LPVOID)((ULONG_PTR)hModule + funcRVA);
        }
    }

    return NULL;
}

HANDLE HookEvasion::SafeOpenProcess(DWORD dwDesiredAccess,
                                   BOOL bInheritHandle,
                                   DWORD dwProcessId) {
    // Try unhookead version first
    LPVOID pUnhooked = GetUnhookedFunctionAddress("kernel32.dll", "OpenProcess");

    if (pUnhooked) {
        typedef HANDLE(WINAPI* pfnOpenProcess)(DWORD, BOOL, DWORD);
        pfnOpenProcess pFunc = (pfnOpenProcess)pUnhooked;

        HANDLE hResult = pFunc(dwDesiredAccess, bInheritHandle, dwProcessId);

        if (hResult) {
            Logger::Success("SafeOpenProcess bem-sucedida");
            return hResult;
        }
    }

    // Fallback to normal
    Logger::Warning("Fallback para OpenProcess normal");
    return OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}
