#include "ProcessListPanel.h"
#include "ImGuiStyle.h"
#include "Logger.h"
#include <imgui.h>

ProcessListPanel::ProcessListPanel() {
    processManager.EnumerateProcesses();
}

void ProcessListPanel::Update(float deltaTime) {
    refreshTimer += deltaTime;

    if (refreshTimer >= REFRESH_INTERVAL) {
        processManager.EnumerateProcesses();
        refreshTimer = 0.0f;
    }
}

void ProcessListPanel::Draw() {
    const NeonStyle::Palette& pal = NeonStyle::Colors();

    float availW = ImGui::GetContentRegionAvail().x;
    float btnW = 90.0f;

    ImGui::SetNextItemWidth(availW - btnW - ImGui::GetStyle().ItemSpacing.x);
    ImGui::InputTextWithHint("##ProcessSearch", "Buscar processos...",
                            searchBuffer, sizeof(searchBuffer));
    processManager.SetSearchFilter(searchBuffer);

    ImGui::SameLine();
    if (ImGui::Button("Atualizar", ImVec2(btnW, 0))) {
        processManager.EnumerateProcesses();
        refreshTimer = 0.0f;
    }

    ImGui::Spacing();

    auto filtered = processManager.GetFilteredProcesses();

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Sortable |
        ImGuiTableFlags_Resizable;

    ImVec2 outer_size = ImVec2(-1, 180);

    if (ImGui::BeginTable("ProcessTable", 3, flags, outer_size)) {
        ImGui::TableSetupColumn("Nome", ImGuiTableColumnFlags_DefaultSort, 0.0f);
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Arch", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < filtered.size(); ++i) {
            const auto& proc = filtered[i];

            ImGui::TableNextRow(ImGuiTableRowFlags_None, 26.0f);
            ImGui::PushID(static_cast<int>(i));

            if (ImGui::TableSetColumnIndex(0)) {
                bool isSelected = (selectedPID == proc.pid);

                if (isSelected) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                          IM_COL32(59, 146, 246, 50));
                }

                if (ImGui::Selectable(proc.name.c_str(), isSelected,
                                     ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedPID = proc.pid;
                    selectedName = proc.name;
                    selectedIs64 = proc.is64bit;
                    Logger::Info("Processo selecionado: " + proc.name +
                               " (PID: " + std::to_string(proc.pid) + ") " +
                               (proc.is64bit ? "x64" : "x86"));
                }
            }

            if (ImGui::TableSetColumnIndex(1)) {
                ImGui::Text("%u", proc.pid);
            }

            if (ImGui::TableSetColumnIndex(2)) {
                ImVec4 archColor = proc.is64bit
                    ? ImVec4(0.29f, 0.78f, 1.0f, 1.0f)
                    : ImVec4(0.98f, 0.75f, 0.29f, 1.0f);
                ImGui::TextColored(archColor, proc.is64bit ? "x64" : "x86");
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    if (selectedPID != 0) {
        ImGui::TextColored(ImVec4(0.29f, 0.78f, 1.0f, 1.0f),
                          "Selecionado: %s (PID: %u) %s",
                          selectedName.c_str(), selectedPID,
                          selectedIs64 ? "x64" : "x86");
    } else {
        ImGui::TextDisabled("Selecione um processo na tabela acima");
    }
}
