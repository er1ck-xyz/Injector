#pragma once
#include "Logger.h"
#include <imgui.h>
#include <vector>

class LogPanel {
private:
    std::vector<LogMessage> displayedLogs;
    bool autoScroll = true;

public:
    LogPanel();

    void Draw();
    void Update();

private:
    ImVec4 GetColorForLevel(LogLevel level) const;
    const char* GetIconForLevel(LogLevel level) const;
};
