#include "ImGuiStyle.h"
#include <imgui.h>

static inline ImVec4 MulColor(ImVec4 c, float f) {
    return ImVec4(c.x * f, c.y * f, c.z * f, c.w);
}

namespace NeonStyle {

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

    ImVec4 darkBg     = ImVec4(0.05f, 0.05f, 0.12f, 1.0f);
    ImVec4 neonCyan   = ImVec4(0.0f,  0.9f,  1.0f,  1.0f);
    ImVec4 neonPurple = ImVec4(0.8f,  0.0f,  1.0f,  1.0f);
    ImVec4 neonGreen  = ImVec4(0.0f,  1.0f,  0.5f,  1.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]        = darkBg;
    colors[ImGuiCol_FrameBg]         = ImVec4(0.08f, 0.08f, 0.15f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.12f, 0.12f, 0.20f, 1.0f);
    colors[ImGuiCol_FrameBgActive]   = ImVec4(0.15f, 0.15f, 0.25f, 1.0f);

    colors[ImGuiCol_Button]          = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
    colors[ImGuiCol_ButtonHovered]   = MulColor(neonPurple, 0.3f);
    colors[ImGuiCol_ButtonActive]    = MulColor(neonPurple, 0.5f);

    colors[ImGuiCol_Header]          = ImVec4(0.12f, 0.12f, 0.22f, 1.0f);
    colors[ImGuiCol_HeaderHovered]   = MulColor(neonCyan, 0.3f);
    colors[ImGuiCol_HeaderActive]    = MulColor(neonCyan, 0.5f);

    colors[ImGuiCol_TabHovered]         = MulColor(neonCyan, 0.2f);
    colors[ImGuiCol_TabActive]          = MulColor(neonCyan, 0.4f);
    colors[ImGuiCol_TabUnfocusedActive] = MulColor(neonCyan, 0.15f);

    colors[ImGuiCol_Text]            = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    colors[ImGuiCol_TextDisabled]    = ImVec4(0.5f,  0.5f,  0.5f,  1.0f);

    colors[ImGuiCol_CheckMark]       = neonGreen;
    colors[ImGuiCol_SliderGrab]      = neonCyan;
    colors[ImGuiCol_SliderGrabActive]= neonPurple;
    colors[ImGuiCol_PlotLinesHovered]= neonGreen;

    colors[ImGuiCol_Border]          = MulColor(neonCyan,   0.2f);
    colors[ImGuiCol_BorderShadow]    = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_Separator]       = MulColor(neonPurple, 0.3f);
    colors[ImGuiCol_SeparatorHovered]= MulColor(neonPurple, 0.5f);
    colors[ImGuiCol_SeparatorActive] = MulColor(neonPurple, 0.8f);
}

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
}

} // namespace NeonStyle
