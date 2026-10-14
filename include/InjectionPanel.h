#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <atomic>
#include "EnterpriseInjector.h"

class InjectionPanel {
private:
    std::vector<std::string> dllPaths;
    int selectedDllIndex = -1;
    DWORD targetPID = 0;
    std::string targetProcessName;
    bool targetIs64 = true;

    std::atomic<int> injectionState{0};
    std::string statusMessage;
    float statusTimer = 0.0f;

    bool evasionEnabled = true;
    bool cloakingEnabled = true;
    bool cleanupEnabled = true;
    int injectionMethod = 0;

    EnterpriseInjector injector;

    void OpenFileDialog();
    void DoInjection();

public:
    InjectionPanel();
    ~InjectionPanel();

    void Update();

    void DrawDLLSection();
    void DrawOptionsSection();
    void DrawInjectSection();

    void SetSelectedProcess(DWORD pid, const std::string& name, bool is64 = true);
};
