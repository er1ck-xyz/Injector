#include "ImGuiStyle.h"
#include <imgui.h>

namespace NeonStyle {

// --- Paleta base (tema escuro grafite + azul profundo / ciano) ---------------
static const ImVec4 kBackground   = ImVec4(0.067f, 0.075f, 0.094f, 1.00f); // grafite
static const ImVec4 kSurface      = ImVec4(0.098f, 0.110f, 0.137f, 1.00f); // cards/paineis
static const ImVec4 kSurfaceAlt   = ImVec4(0.125f, 0.141f, 0.176f, 1.00f);
static const ImVec4 kFrame        = ImVec4(0.141f, 0.157f, 0.196f, 1.00f);
static const ImVec4 kFrameHover   = ImVec4(0.180f, 0.200f, 0.247f, 1.00f);
static const ImVec4 kFrameActive  = ImVec4(0.212f, 0.235f, 0.290f, 1.00f);

static const ImVec4 kAccent       = ImVec4(0.231f, 0.573f, 0.965f, 1.00f); // azul
static const ImVec4 kAccentBright = ImVec4(0.290f, 0.776f, 0.996f, 1.00f); // ciano
static const ImVec4 kAccentDim    = ImVec4(0.231f, 0.573f, 0.965f, 0.35f);

static const ImVec4 kSuccess      = ImVec4(0.235f, 0.824f, 0.549f, 1.00f);
static const ImVec4 kWarning      = ImVec4(0.984f, 0.749f, 0.290f, 1.00f);
static const ImVec4 kDanger       = ImVec4(0.953f, 0.388f, 0.400f, 1.00f);

static const ImVec4 kText         = ImVec4(0.902f, 0.918f, 0.953f, 1.00f);
static const ImVec4 kTextMuted    = ImVec4(0.560f, 0.588f, 0.651f, 1.00f);
static const ImVec4 kBorder       = ImVec4(1.000f, 1.000f, 1.000f, 0.06f);

static ImFont* g_titleFont = nullptr;
static ImFont* g_bodyFont  = nullptr;
static Palette g_palette{};

ImFont* TitleFont() { return g_titleFont; }
ImFont* BodyFont()  { return g_bodyFont; }
const Palette& Colors() { return g_palette; }

void SetupNeonStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    // --- Geometria: cantos arredondados e espacamento consistente ------------
    style.WindowPadding     = ImVec2(0, 0);   // a janela host controla o padding
    style.FramePadding      = ImVec2(12, 7);
    style.CellPadding       = ImVec2(10, 7);
    style.ItemSpacing       = ImVec2(10, 9);
    style.ItemInnerSpacing  = ImVec2(8, 6);
    style.IndentSpacing     = 20.0f;
    style.ScrollbarSize     = 11.0f;
    style.GrabMinSize       = 10.0f;

    style.WindowBorderSize  = 0.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 0.0f;
    style.TabBarBorderSize  = 0.0f;

    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 8.0f;
    style.FrameRounding     = 6.0f;
    style.PopupRounding     = 8.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding      = 6.0f;
    style.TabRounding       = 7.0f;

    style.WindowTitleAlign  = ImVec2(0.0f, 0.5f);
    style.ButtonTextAlign   = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.5f);

    style.AntiAliasedLines   = true;
    style.AntiAliasedFill    = true;
    style.AntiAliasedLinesUseTex = true;

    // --- Cores ----------------------------------------------------------------
    ImVec4* c = style.Colors;

    c[ImGuiCol_Text]                 = kText;
    c[ImGuiCol_TextDisabled]         = kTextMuted;
    c[ImGuiCol_WindowBg]             = kBackground;
    c[ImGuiCol_ChildBg]              = kSurface;
    c[ImGuiCol_PopupBg]              = kSurfaceAlt;
    c[ImGuiCol_Border]               = kBorder;
    c[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);

    c[ImGuiCol_FrameBg]              = kFrame;
    c[ImGuiCol_FrameBgHovered]       = kFrameHover;
    c[ImGuiCol_FrameBgActive]        = kFrameActive;

    c[ImGuiCol_TitleBg]              = kSurface;
    c[ImGuiCol_TitleBgActive]        = kSurface;
    c[ImGuiCol_TitleBgCollapsed]     = kSurface;
    c[ImGuiCol_MenuBarBg]            = kSurface;

    c[ImGuiCol_ScrollbarBg]          = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.26f, 0.29f, 0.36f, 1.00f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.37f, 0.45f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]  = kAccent;

    c[ImGuiCol_CheckMark]            = kAccentBright;
    c[ImGuiCol_SliderGrab]           = kAccent;
    c[ImGuiCol_SliderGrabActive]     = kAccentBright;

    c[ImGuiCol_Button]               = kFrame;
    c[ImGuiCol_ButtonHovered]        = kFrameHover;
    c[ImGuiCol_ButtonActive]         = kFrameActive;

    c[ImGuiCol_Header]               = ImVec4(kAccent.x, kAccent.y, kAccent.z, 0.22f);
    c[ImGuiCol_HeaderHovered]        = ImVec4(kAccent.x, kAccent.y, kAccent.z, 0.32f);
    c[ImGuiCol_HeaderActive]         = ImVec4(kAccent.x, kAccent.y, kAccent.z, 0.45f);

    c[ImGuiCol_Separator]            = kBorder;
    c[ImGuiCol_SeparatorHovered]     = kAccentDim;
    c[ImGuiCol_SeparatorActive]      = kAccent;

    c[ImGuiCol_ResizeGrip]           = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ResizeGripHovered]    = kAccentDim;
    c[ImGuiCol_ResizeGripActive]     = kAccent;

    c[ImGuiCol_Tab]                  = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_TabHovered]           = ImVec4(kAccent.x, kAccent.y, kAccent.z, 0.18f);
    c[ImGuiCol_TabSelected]          = kFrame;
    c[ImGuiCol_TabSelectedOverline]  = kAccentBright;
    c[ImGuiCol_TabDimmed]            = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_TabDimmedSelected]    = kSurfaceAlt;
    c[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0, 0, 0, 0);

    c[ImGuiCol_TableHeaderBg]        = kSurfaceAlt;
    c[ImGuiCol_TableBorderStrong]    = kBorder;
    c[ImGuiCol_TableBorderLight]     = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);
    c[ImGuiCol_TableRowBg]           = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.018f);

    c[ImGuiCol_PlotLines]            = kAccent;
    c[ImGuiCol_PlotLinesHovered]     = kAccentBright;
    c[ImGuiCol_PlotHistogram]        = kAccent;
    c[ImGuiCol_PlotHistogramHovered] = kAccentBright;

    c[ImGuiCol_TextSelectedBg]       = ImVec4(kAccent.x, kAccent.y, kAccent.z, 0.35f);
    c[ImGuiCol_NavCursor]            = kAccent;
    c[ImGuiCol_DragDropTarget]       = kWarning;
    c[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);

    // --- Paleta exportada para os paineis ------------------------------------
    g_palette.Accent     = ImGui::GetColorU32(kAccent);
    g_palette.AccentDim  = ImGui::GetColorU32(kAccentDim);
    g_palette.Success    = ImGui::GetColorU32(kSuccess);
    g_palette.Warning    = ImGui::GetColorU32(kWarning);
    g_palette.Danger     = ImGui::GetColorU32(kDanger);
    g_palette.TextMuted  = ImGui::GetColorU32(kTextMuted);
    g_palette.Surface    = ImGui::GetColorU32(kSurface);
    g_palette.SurfaceAlt = ImGui::GetColorU32(kSurfaceAlt);
}

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    // Portugues (acentuacao) coberto por Basic Latin + Latin-1 Supplement.
    const ImWchar* ranges = io.Fonts->GetGlyphRangesDefault();

    ImFontConfig cfg;
    cfg.OversampleH = 3;
    cfg.OversampleV = 2;
    cfg.PixelSnapH  = false;

    // Tipografia profissional usando a fonte do sistema (Segoe UI do Windows,
    // sempre presente — nao e uma dependencia adicionada ao projeto).
    const char* bodyPath  = "C:/Windows/Fonts/segoeui.ttf";
    const char* titlePath = "C:/Windows/Fonts/segoeuisb.ttf"; // Segoe UI Semibold

    g_bodyFont = io.Fonts->AddFontFromFileTTF(bodyPath, 18.0f, &cfg, ranges);
    if (g_bodyFont == nullptr) {
        // Fallback seguro caso a fonte do sistema nao exista.
        g_bodyFont = io.Fonts->AddFontDefault();
    }

    g_titleFont = io.Fonts->AddFontFromFileTTF(titlePath, 22.0f, &cfg, ranges);
    if (g_titleFont == nullptr) {
        g_titleFont = io.Fonts->AddFontFromFileTTF(bodyPath, 22.0f, &cfg, ranges);
    }
    if (g_titleFont == nullptr) {
        g_titleFont = g_bodyFont;
    }

    io.Fonts->Build();
    io.FontDefault = g_bodyFont;
}

} // namespace NeonStyle
