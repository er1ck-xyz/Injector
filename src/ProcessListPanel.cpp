#include "ProcessListPanel.h"
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
    ImGui::SetNextItemWidth(-1);

    ImGui::InputTextWithHint("##ProcessSearch",
                            "Buscar processos...",
                            searchBuffer, sizeof(searchBuffer));

    processManager.SetSearchFilter(searchBuffer);

    ImGui::Spacing();

    if (ImGui::Button("Atualizar Processos", ImVec2(-1, 0))) {
        processManager.EnumerateProcesses();
    }

    ImGui::Spacing();

    auto filtered = processManager.GetFilteredProcesses();
    ImGui::TextDisabled("Total: %zu processos", filtered.size());

    ImGui::Separator();

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Sortable |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Reorderable;

    ImVec2 outer_size = ImVec2(-1, ImGui::GetContentRegionAvail().y - 80);

    if (ImGui::BeginTable("ProcessTable", 2, flags, outer_size)) {
        ImGui::TableSetupColumn("Nome do Processo", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < filtered.size(); ++i) {
            const auto& proc = filtered[i];

            ImGui::TableNextRow(ImGuiTableRowFlags_None, 30.0f);
            ImGui::PushID(static_cast<int>(i));

            if (ImGui::TableSetColumnIndex(0)) {
                bool isSelected = (selectedPID == proc.pid);

                if (isSelected) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                          ImGui::GetColorU32(ImVec4(0.8f, 0.0f, 1.0f, 0.3f)));
                }

                if (ImGui::Selectable(proc.name.c_str(), isSelected,
                                     ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedPID = proc.pid;
                    Logger::Info("Processo selecionado: " + proc.name + " (PID: " +
                               std::to_string(proc.pid) + ")");
                }
            }

            if (ImGui::TableSetColumnIndex(1)) {
                ImGui::Text("%u", proc.pid);
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::Separator();

    if (selectedPID != 0) {
        ImGui::BeginChild("##SelectedInfo", ImVec2(-1, 60), true);
        ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f),
                          "Processo Selecionado");
        ImGui::Text("PID: %u", selectedPID);
        ImGui::EndChild();
    } else {
        ImGui::BeginChild("##NoSelection", ImVec2(-1, 60), true);
        ImGui::TextDisabled("Selecione um processo na tabela");
        ImGui::EndChild();
    }
}
