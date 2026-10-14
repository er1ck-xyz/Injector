#include "GUI.h"
#include "ImGuiStyle.h"
#include "NativeWindow.h"
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

// Altura do cabecalho personalizado (area de arraste).
static const float kTitleBarHeight = 54.0f;

void GUI::DrawTitleBar() {
    const NeonStyle::Palette& pal = NeonStyle::Colors();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    const ImVec2 p0 = ImGui::GetCursorScreenPos();
    const float width = ImGui::GetContentRegionAvail().x;
    const ImVec2 p1 = ImVec2(p0.x + width, p0.y + kTitleBarHeight);
    const float centerY = p0.y + kTitleBarHeight * 0.5f;

    // Fundo da barra (superficie) + hairline inferior de destaque.
    dl->AddRectFilled(p0, p1, pal.Surface);
    dl->AddLine(ImVec2(p0.x, p1.y - 1.0f), ImVec2(p1.x, p1.y - 1.0f),
                pal.AccentDim, 1.0f);

    // Area de arraste nativa: cobre toda a barra, sem botoes visiveis.
    ImGui::InvisibleButton("##titlebar_drag", ImVec2(width, kTitleBarHeight));
    if (ImGui::IsItemActivated())
        NativeWindow::StartDrag();

    // --- Logo: quadrado arredondado com gradiente azul -> ciano --------------
    const float logoSize = 28.0f;
    const ImVec2 lg0 = ImVec2(p0.x + 22.0f, centerY - logoSize * 0.5f);
    const ImVec2 lg1 = ImVec2(lg0.x + logoSize, lg0.y + logoSize);
    const ImU32 top = IM_COL32(59, 146, 246, 255);   // azul
    const ImU32 bot = IM_COL32(74, 198, 254, 255);   // ciano
    dl->AddRectFilledMultiColor(lg0, lg1, top, top, bot, bot);
    // reforca cantos arredondados por cima
    dl->AddRect(lg0, lg1, IM_COL32(255, 255, 255, 40), 8.0f, 0, 1.0f);

    // Chevron ">" branco dentro do logo.
    const float cx = lg0.x + logoSize * 0.40f;
    const float cy = centerY;
    const float ch = logoSize * 0.22f;
    const ImU32 white = IM_COL32(255, 255, 255, 235);
    dl->AddLine(ImVec2(cx - ch, cy - ch), ImVec2(cx + ch, cy), white, 2.2f);
    dl->AddLine(ImVec2(cx + ch, cy), ImVec2(cx - ch, cy + ch), white, 2.2f);

    // --- Titulo: "INJECTOR" branco + "PRO" em destaque ----------------------
    ImFont* titleFont = NeonStyle::TitleFont();
    const float titleSize = 22.0f;
    const float textX = lg1.x + 14.0f;
    const float textY = centerY - titleSize * 0.5f - 1.0f;

    const char* t1 = "INJECTOR ";
    const char* t2 = "PRO";
    ImVec2 t1sz = titleFont->CalcTextSizeA(titleSize, FLT_MAX, -1.0f, t1);
    dl->AddText(titleFont, titleSize, ImVec2(textX, textY),
                IM_COL32(240, 243, 250, 255), t1);
    dl->AddText(titleFont, titleSize, ImVec2(textX + t1sz.x, textY),
                pal.Accent, t2);

    // --- Subtitulo discreto a direita ---------------------------------------
    ImFont* bodyFont = NeonStyle::BodyFont();
    const float subSize = 14.0f;
    const char* sub = "Enterprise DLL Injection Suite";
    ImVec2 subSz = bodyFont->CalcTextSizeA(subSize, FLT_MAX, -1.0f, sub);
    dl->AddText(bodyFont, subSize,
                ImVec2(p1.x - subSz.x - 22.0f, centerY - subSize * 0.5f),
                pal.TextMuted, sub);
}

void GUI::DrawContent() {
    // Area de conteudo com padding consistente, fundo grafite (host).
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 18));
    ImGui::BeginChild("##content", ImVec2(0, 0), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoScrollbar);

    ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_FittingPolicyResizeDown;

    if (ImGui::BeginTabBar("##MainTabs", tabFlags)) {

        // Aba 1: Processos
        if (ImGui::BeginTabItem("  Processos  ")) {
            ImGui::Spacing();
            processPanel->Draw();
            injectionPanel->SetSelectedProcess(processPanel->GetSelectedPID());
            ImGui::EndTabItem();
        }

        // Aba 2: Injecao  (UTF-8: Inje\xC3\xA7\xC3\xA3o)
        if (ImGui::BeginTabItem("  Inje\xC3\xA7\xC3\xA3o  ")) {
            ImGui::Spacing();
            injectionPanel->Draw();
            ImGui::EndTabItem();
        }

        // Aba 3: Logs
        if (ImGui::BeginTabItem("  Logs  ")) {
            ImGui::Spacing();
            logPanel->Draw();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void GUI::Render() {
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);

    ImGuiWindowFlags hostFlags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##InjectorHost", nullptr, hostFlags);
    ImGui::PopStyleVar(3);

    DrawTitleBar();
    DrawContent();

    ImGui::End();
}
