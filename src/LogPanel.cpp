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
            return ImVec4(0.0f, 1.0f, 0.5f, 1.0f);
        case LogLevel::Warning:
            return ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
        case LogLevel::Error:
            return ImVec4(1.0f, 0.2f, 0.3f, 1.0f);
        default:
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

const char* LogPanel::GetIconForLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::Info:    return "i";
        case LogLevel::Success: return "+";
        case LogLevel::Warning: return "!";
        case LogLevel::Error:   return "x";
        default:                return ".";
    }
}

void LogPanel::Draw() {
    ImGui::SetNextItemWidth(-1);
    ImGui::TextUnformatted("Logs");

    ImGui::Spacing();

    if (ImGui::Button("Limpar Logs", ImVec2(120, 0))) {
        Logger::Clear();
        displayedLogs.clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::SameLine();
    ImGui::TextDisabled("(%zu logs)", displayedLogs.size());

    ImGui::Separator();

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchSame;

    ImVec2 outer_size = ImVec2(-1, ImGui::GetContentRegionAvail().y);

    if (ImGui::BeginTable("LogTable", 3, flags, outer_size)) {
        ImGui::TableSetupColumn("Hora", ImGuiTableColumnFlags_WidthFixed, 120);
        ImGui::TableSetupColumn("Nvel", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Mensagem", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (int i = static_cast<int>(displayedLogs.size()) - 1; i >= 0; --i) {
            const auto& log = displayedLogs[i];

            ImGui::TableNextRow(ImGuiTableRowFlags_None, 24.0f);
            ImGui::PushID(i);

            ImVec4 color = GetColorForLevel(log.level);

            if (ImGui::TableSetColumnIndex(0)) {
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                                  log.timestamp.c_str());
            }

            if (ImGui::TableSetColumnIndex(1)) {
                ImGui::TextColored(color, "%s", GetIconForLevel(log.level));
            }

            if (ImGui::TableSetColumnIndex(2)) {
                ImGui::TextColored(color, log.message.c_str());
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}
