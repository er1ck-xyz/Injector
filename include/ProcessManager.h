#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>

struct ProcessInfo {
    std::string name;
    DWORD pid;

    ProcessInfo(const std::string& n, DWORD p) : name(n), pid(p) {}
};

class ProcessManager {
private:
    std::vector<ProcessInfo> processes;
    std::string searchFilter;

public:
    bool EnumerateProcesses();

    void SetSearchFilter(const std::string& filter);

    const std::vector<ProcessInfo>& GetProcesses() const;
    std::vector<ProcessInfo> GetFilteredProcesses() const;
    DWORD GetSelectedPID() const;
};
