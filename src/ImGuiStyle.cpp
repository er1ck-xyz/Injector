#include "ImGuiStyle.h"
#include <imgui.h>

namespace ImGuiStyle {

void SetupNeonStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(16, 16);
    style.FramePadding = ImVec2(12, 8);
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 4);

    style.FrameRounding = 6.0f;
    style.WindowRounding = 8.0f;
    style.PopupRounding = 6.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;

    ImVec4 darkBg = ImVec4(0.05f, 0.05f, 0.12f, 1.0f);
    ImVec4 neonCyan = ImVec4(0.0f, 0.9f, 1.0f, 1.0f);
    ImVec4 neonPurple = ImVec4(0.8f, 0.0f, 1.0f, 1.0f);
    ImVec4 neonGreen = ImVec4(0.0f, 1.0f, 0.5f, 1.0f);
    ImVec4 neonRed = ImVec4(1.0f, 0.1f, 0.3f, 1.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = darkBg;
    colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.08f, 0.15f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.12f, 0.2f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.25f, 1.0f);

    colors[ImGuiCol_Button] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = neonPurple * ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_ButtonActive] = neonPurple * ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    colors[ImGuiCol_Header] = ImVec4(0.12f, 0.12f, 0.22f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = neonCyan * ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_HeaderActive] = neonCyan * ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    colors[ImGuiCol_TabHovered] = neonCyan * ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TabActive] = neonCyan * ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = neonCyan * ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    colors[ImGuiCol_CheckMark] = neonGreen;
    colors[ImGuiCol_SliderGrab] = neonCyan;
    colors[ImGuiCol_SliderGrabActive] = neonPurple;
    colors[ImGuiCol_PlotLinesHovered] = neonGreen;

    colors[ImGuiCol_Border] = neonCyan * ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_Separator] = neonPurple * ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_SeparatorHovered] = neonPurple * ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_SeparatorActive] = neonPurple * ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
}

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();

    // Font padrão (use do sistema)
    io.Fonts->AddFontDefault();
}

} // namespace ImGuiStyle
