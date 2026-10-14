#include "InjectionPanel.h"
#include "Logger.h"
#include <imgui.h>

InjectionPanel::InjectionPanel() {
}

void InjectionPanel::Update() {
    // Placeholder para update futuro
}

void InjectionPanel::Draw() {
    ImGui::Text("1. Selecione a DLL:");
    ImGui::Separator();

    if (ImGui::Button("Escolher DLL", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
        Logger::Info("Botão de DLL pressionado (implementação futura)");
    }

    if (!selectedDLLPath.empty()) {
        ImGui::InputText("##DLLPath", dllPathDisplay,
                        sizeof(dllPathDisplay), ImGuiInputTextFlags_ReadOnly);
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f),
                          "DLL Selecionada");
    } else {
        ImGui::TextDisabled("Nenhuma DLL selecionada");
    }

    ImGui::Spacing();
    ImGui::Text("2. Processo Selecionado:");
    ImGui::Separator();

    if (selectedProcessPID != 0) {
        ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f),
                          "PID: %u", selectedProcessPID);
    } else {
        ImGui::TextDisabled("Nenhum processo selecionado");
    }

    ImGui::Spacing();
    ImGui::Text("3. Injetar:");
    ImGui::Separator();

    bool canInject = !selectedDLLPath.empty() && selectedProcessPID != 0 && !isInjecting;

    if (!canInject) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("INJETAR DLL", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
        Logger::Info("Injeção iniciada (implementação futura)");
        isInjecting = true;
    }

    if (!canInject) {
        ImGui::EndDisabled();
    }

    if (isInjecting) {
        ImGui::ProgressBar(0.5f, ImVec2(-1, 0));
        ImGui::TextDisabled("Injetando...");
    }
}
