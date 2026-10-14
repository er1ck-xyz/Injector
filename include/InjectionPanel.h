#pragma once
#include <windows.h>
#include <string>

class InjectionPanel {
private:
    std::string selectedDLLPath;
    DWORD selectedProcessPID = 0;
    char dllPathDisplay[260] = {0};
    int injectionProgress = 0;
    bool isInjecting = false;

public:
    InjectionPanel();

    void Draw();
    void Update();

    void SetSelectedProcess(DWORD pid) { selectedProcessPID = pid; }
};
