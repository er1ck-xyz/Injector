#include "GUI.h"
#include "ImGuiStyle.h"
#include <imgui.h>

GUI::GUI()
    : processPanel(std::make_unique<ProcessListPanel>()),
      injectionPanel(std::make_unique<InjectionPanel>()),
      logPanel(std::make_unique<LogPanel>()) {
}

GUI::~GUI() {
}

void GUI::Update(float deltaTime) {
    processPanel->Update(deltaTime);
    injectionPanel->Update();
    logPanel->Update();
}

void GUI::Render() {
    ImGui::SetNextWindowPos(ImVec2(50, 50));
    ImGui::SetNextWindowSize(ImVec2(1200, 700));
    ImGui::Begin("Injector Pro", &windowOpen, ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##MainTabs", ImGuiTabBarFlags_FittingPolicyResizeDown)) {

        // Aba 1: Processos
        if (ImGui::BeginTabItem("Processos")) {
            processPanel->Draw();
            injectionPanel->SetSelectedProcess(processPanel->GetSelectedPID());
            ImGui::EndTabItem();
        }

        // Aba 2: Injeo
        if (ImGui::BeginTabItem("Injeo")) {
            injectionPanel->Draw();
            ImGui::EndTabItem();
        }

        // Aba 3: Logs
        if (ImGui::BeginTabItem("Logs")) {
            logPanel->Draw();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
