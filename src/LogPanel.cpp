#include "LogPanel.h"

LogPanel::LogPanel() {
    displayedLogs = Logger::GetLogs();
}

void LogPanel::Update() {
    displayedLogs = Logger::GetLogs();
}

ImVec4 LogPanel::GetColorForLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::Info:
            return ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
        case LogLevel::Success:
            return ImVec4(0.24f, 0.82f, 0.55f, 1.0f);
        case LogLevel::Warning:
            return ImVec4(0.98f, 0.75f, 0.29f, 1.0f);
        case LogLevel::Error:
            return ImVec4(0.95f, 0.39f, 0.40f, 1.0f);
        default:
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

const char* LogPanel::GetIconForLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::Info:    return "[i]";
        case LogLevel::Success: return "[+]";
        case LogLevel::Warning: return "[!]";
        case LogLevel::Error:   return "[x]";
        default:                return "[.]";
    }
}

void LogPanel::Draw() {
    float availW = ImGui::GetContentRegionAvail().x;

    if (ImGui::Button("Limpar", ImVec2(70, 0))) {
        Logger::Clear();
        displayedLogs.clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::SameLine();
    ImGui::TextDisabled("(%zu)", displayedLogs.size());

    ImGui::Spacing();

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchSame;

    ImVec2 outer_size = ImVec2(-1, ImGui::GetContentRegionAvail().y);

    if (ImGui::BeginTable("LogTable", 3, flags, outer_size)) {
        ImGui::TableSetupColumn("Hora", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Nivel", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("Mensagem", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < displayedLogs.size(); ++i) {
            const auto& log = displayedLogs[i];

            ImGui::TableNextRow(ImGuiTableRowFlags_None, 22.0f);
            ImGui::PushID(static_cast<int>(i));

            ImVec4 color = GetColorForLevel(log.level);

            if (ImGui::TableSetColumnIndex(0)) {
                ImGui::TextColored(ImVec4(0.56f, 0.59f, 0.65f, 1.0f),
                                  "%s", log.timestamp.c_str());
            }

            if (ImGui::TableSetColumnIndex(1)) {
                ImGui::TextColored(color, "%s", GetIconForLevel(log.level));
            }

            if (ImGui::TableSetColumnIndex(2)) {
                ImGui::TextColored(color, "%s", log.message.c_str());
            }

            ImGui::PopID();
        }

        if (autoScroll && ImGui::GetScrollY() < ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndTable();
    }
}
