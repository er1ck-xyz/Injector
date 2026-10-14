#include "ProcessManager.h"
#include "Logger.h"
#include <algorithm>
#include <cctype>

bool ProcessManager::EnumerateProcesses() {
    processes.clear();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        Logger::Error("Falha ao criar snapshot de processos");
        return false;
    }

    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &entry)) {
        Logger::Error("Falha ao obter primeiro processo");
        CloseHandle(snapshot);
        return false;
    }

    do {
        char processName[MAX_PATH];
        WideCharToMultiByte(CP_UTF8, 0, entry.szExeFile, -1,
                           processName, sizeof(processName), nullptr, nullptr);

        processes.emplace_back(processName, entry.th32ProcessID);

    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    Logger::Success("Enumerados " + std::to_string(processes.size()) + " processos");
    return true;
}

void ProcessManager::SetSearchFilter(const std::string& filter) {
    searchFilter = filter;

    std::transform(searchFilter.begin(), searchFilter.end(),
                   searchFilter.begin(), ::tolower);
}

const std::vector<ProcessInfo>& ProcessManager::GetProcesses() const {
    return processes;
}

std::vector<ProcessInfo> ProcessManager::GetFilteredProcesses() const {
    if (searchFilter.empty()) {
        return processes;
    }

    std::vector<ProcessInfo> filtered;

    for (const auto& proc : processes) {
        std::string lowerName = proc.name;
        std::transform(lowerName.begin(), lowerName.end(),
                       lowerName.begin(), ::tolower);

        if (lowerName.find(searchFilter) != std::string::npos) {
            filtered.push_back(proc);
        }
    }

    return filtered;
}

DWORD ProcessManager::GetSelectedPID() const {
    return 0;
}
