#include "InjectionPanel.h"
#include "ImGuiStyle.h"
#include "NativeWindow.h"
#include "Logger.h"
#include <imgui.h>
#include <commdlg.h>
#include <thread>
#include <algorithm>

#pragma comment(lib, "comdlg32.lib")

InjectionPanel::InjectionPanel() {
}

InjectionPanel::~InjectionPanel() {
}

void InjectionPanel::Update() {
    int st = injectionState.load();
    if (st == 2 || st == 3) {
        statusTimer += 0.016f;
        if (statusTimer > 5.0f) {
            injectionState.store(0);
            statusTimer = 0.0f;
        }
    }
}

void InjectionPanel::SetSelectedProcess(DWORD pid, const std::string& name, bool is64) {
    targetPID = pid;
    targetProcessName = name;
    targetIs64 = is64;
}

void InjectionPanel::OpenFileDialog() {
    wchar_t fileBuf[4096] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NativeWindow::GetHandle();
    ofn.lpstrFilter = L"DLL Files (*.dll)\0*.dll\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileBuf;
    ofn.nMaxFile = sizeof(fileBuf) / sizeof(wchar_t);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    ofn.lpstrTitle = L"Selecionar DLL para Injetar";

    if (!GetOpenFileNameW(&ofn))
        return;

    if (fileBuf[ofn.nFileOffset - 1] == L'\0') {
        std::wstring dir(fileBuf);
        wchar_t* p = fileBuf + dir.size() + 1;
        while (*p) {
            std::wstring full = dir + L"\\" + p;
            std::string path(full.begin(), full.end());
            if (std::find(dllPaths.begin(), dllPaths.end(), path) == dllPaths.end()) {
                dllPaths.push_back(path);
                Logger::Info("DLL adicionada: " + path);
            }
            p += wcslen(p) + 1;
        }
    } else {
        std::wstring wpath(fileBuf);
        std::string path(wpath.begin(), wpath.end());
        if (std::find(dllPaths.begin(), dllPaths.end(), path) == dllPaths.end()) {
            dllPaths.push_back(path);
            Logger::Info("DLL adicionada: " + path);
        }
    }

    if (!dllPaths.empty() && selectedDllIndex < 0)
        selectedDllIndex = 0;
}

void InjectionPanel::DoInjection() {
    injector.SetEvasionEnabled(evasionEnabled);
    injector.SetCloakingEnabled(cloakingEnabled);
    injector.SetCleanupEnabled(cleanupEnabled);

    Logger::Info("Iniciando injecao em " + targetProcessName +
                 " (PID: " + std::to_string(targetPID) + ")");

    for (size_t i = 0; i < dllPaths.size(); ++i) {
        const auto& dll = dllPaths[i];
        size_t slash = dll.find_last_of("\\/");
        std::string name = (slash != std::string::npos) ? dll.substr(slash + 1) : dll;

        Logger::Info("Injetando (" + std::to_string(i + 1) + "/" +
                     std::to_string(dllPaths.size()) + "): " + name);

        if (!injector.InjectWithFullEvasion(targetPID, dll)) {
            statusMessage = "Falha ao injetar: " + name;
            Logger::Error(statusMessage);
            injectionState.store(3);
            return;
        }
        Logger::Success("DLL injetada: " + name);
    }

    if (dllPaths.size() == 1) {
        size_t slash = dllPaths[0].find_last_of("\\/");
        std::string name = (slash != std::string::npos) ? dllPaths[0].substr(slash + 1) : dllPaths[0];
        statusMessage = name + " injetada com sucesso!";
    } else {
        statusMessage = std::to_string(dllPaths.size()) + " DLLs injetadas com sucesso!";
    }
    Logger::Success(statusMessage);
    injectionState.store(2);
}

void InjectionPanel::DrawDLLSection() {
    float availW = ImGui::GetContentRegionAvail().x;
    float btnW = (availW - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

    if (ImGui::Button("+ Adicionar DLL", ImVec2(btnW, 0)))
        OpenFileDialog();

    ImGui::SameLine();

    bool canRemove = selectedDllIndex >= 0 &&
                     selectedDllIndex < static_cast<int>(dllPaths.size());
    if (!canRemove) ImGui::BeginDisabled();
    if (ImGui::Button("- Remover", ImVec2(btnW, 0))) {
        Logger::Info("DLL removida: " + dllPaths[selectedDllIndex]);
        dllPaths.erase(dllPaths.begin() + selectedDllIndex);
        if (selectedDllIndex >= static_cast<int>(dllPaths.size()))
            selectedDllIndex = static_cast<int>(dllPaths.size()) - 1;
    }
    if (!canRemove) ImGui::EndDisabled();

    ImGui::Spacing();

    ImVec2 listSize = ImVec2(-1, 100);

    if (dllPaths.empty()) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.098f, 0.110f, 0.137f, 1.0f));
        ImGui::BeginChild("##dll_empty", listSize, ImGuiChildFlags_Borders);
        float h = ImGui::GetContentRegionAvail().y;
        ImGui::SetCursorPosY(h * 0.5f - ImGui::GetTextLineHeight() * 0.5f);
        float tw = ImGui::CalcTextSize("Clique em '+ Adicionar DLL' para selecionar").x;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - tw) * 0.5f);
        ImGui::TextDisabled("Clique em '+ Adicionar DLL' para selecionar");
        ImGui::EndChild();
        ImGui::PopStyleColor();
        return;
    }

    if (ImGui::BeginListBox("##dll_list", listSize)) {
        for (int i = 0; i < static_cast<int>(dllPaths.size()); ++i) {
            size_t slash = dllPaths[i].find_last_of("\\/");
            std::string display = (slash != std::string::npos)
                ? dllPaths[i].substr(slash + 1)
                : dllPaths[i];

            bool selected = (i == selectedDllIndex);
            if (ImGui::Selectable(display.c_str(), selected))
                selectedDllIndex = i;
        }
        ImGui::EndListBox();
    }
}

void InjectionPanel::DrawOptionsSection() {
    const char* methods[] = { "CreateRemoteThread", "NtCreateThreadEx", "Manual Map" };

    ImGui::SetNextItemWidth(-1);
    ImGui::Combo("##method", &injectionMethod, methods, IM_ARRAYSIZE(methods));

    if (injectionMethod != 0) {
        ImGui::TextColored(ImVec4(0.98f, 0.75f, 0.29f, 1.0f), "Em breve");
        injectionMethod = 0;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox("Evasao", &evasionEnabled);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Anti-debug, anti-VM, evasao de hooks");

    ImGui::Checkbox("Cloaking", &cloakingEnabled);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Ofuscacao de memoria, protecao de strings");

    ImGui::Checkbox("Limpeza", &cleanupEnabled);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Remover artefatos, limpar event logs");
}

void InjectionPanel::DrawInjectSection() {
    int st = injectionState.load();
    bool canInject = !dllPaths.empty() && targetPID != 0 && st == 0;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    if (st == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.50f, 0.90f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.58f, 0.98f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.42f, 0.78f, 1.0f));
    }

    if (!canInject && st == 0) ImGui::BeginDisabled();

    float btnH = 42.0f;
    if (ImGui::Button(st == 1 ? "INJETANDO..." : "INJETAR DLL",
                      ImVec2(-1, btnH))) {
        if (canInject) {
            injectionState.store(1);
            statusTimer = 0.0f;
            std::thread([this]() { DoInjection(); }).detach();
        }
    }

    if (!canInject && st == 0) ImGui::EndDisabled();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    if (st == 1) {
        float time = static_cast<float>(ImGui::GetTime());
        float progress = (sinf(time * 3.0f) + 1.0f) * 0.5f;
        ImGui::ProgressBar(progress, ImVec2(-1, 4));
    } else if (st == 2) {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.24f, 0.82f, 0.55f, 1.0f));
        ImGui::ProgressBar(1.0f, ImVec2(-1, 4));
        ImGui::PopStyleColor();
        ImGui::TextColored(ImVec4(0.24f, 0.82f, 0.55f, 1.0f), "%s",
                          statusMessage.c_str());
    } else if (st == 3) {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.95f, 0.39f, 0.40f, 1.0f));
        ImGui::ProgressBar(1.0f, ImVec2(-1, 4));
        ImGui::PopStyleColor();
        ImGui::TextColored(ImVec4(0.95f, 0.39f, 0.40f, 1.0f), "%s",
                          statusMessage.c_str());
    }
}
