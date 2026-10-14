#pragma once
#include "ProcessManager.h"
#include <string>

class ProcessListPanel {
private:
    ProcessManager processManager;
    DWORD selectedPID = 0;
    std::string selectedName;
    bool selectedIs64 = true;
    char searchBuffer[256] = {0};
    bool needsRefresh = true;
    float refreshTimer = 0.0f;
    static constexpr float REFRESH_INTERVAL = 2.0f;

public:
    ProcessListPanel();

    void Draw();
    void Update(float deltaTime);

    DWORD GetSelectedPID() const { return selectedPID; }
    const std::string& GetSelectedName() const { return selectedName; }
    bool IsSelectedProcess64() const { return selectedIs64; }
    bool HasProcessSelected() const { return selectedPID != 0; }
};
