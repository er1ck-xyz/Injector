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
    NativeWindow::UpdateDrag();
    processPanel->Update(deltaTime);
    injectionPanel->Update();
    logPanel->Update();

    injectionPanel->SetSelectedProcess(
        processPanel->GetSelectedPID(),
        processPanel->GetSelectedName(),
        processPanel->IsSelectedProcess64()
    );
}

static const float kTitleBarHeight = 54.0f;

void GUI::DrawTitleBar() {
    const NeonStyle::Palette& pal = NeonStyle::Colors();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    const ImVec2 p0 = ImGui::GetCursorScreenPos();
    const float width = ImGui::GetContentRegionAvail().x;
    const ImVec2 p1 = ImVec2(p0.x + width, p0.y + kTitleBarHeight);
    const float centerY = p0.y + kTitleBarHeight * 0.5f;

    dl->AddRectFilled(p0, p1, pal.Surface);
    dl->AddLine(ImVec2(p0.x, p1.y - 1.0f), ImVec2(p1.x, p1.y - 1.0f),
                pal.AccentDim, 1.0f);

    // --- Close / Minimize buttons (right side) ---
    const float btnSize = 18.0f;
    const float btnPad = 16.0f;

    // Close button
    {
        ImVec2 closePos(p1.x - btnPad - btnSize, centerY - btnSize * 0.5f);
        ImGui::SetCursorScreenPos(closePos);
        ImGui::PushID("##close");
        if (ImGui::InvisibleButton("##close_btn", ImVec2(btnSize, btnSize))) {
            windowOpen = false;
            PostQuitMessage(0);
        }
        ImU32 closeCol = ImGui::IsItemHovered()
            ? IM_COL32(235, 87, 87, 255) : IM_COL32(180, 185, 200, 180);
        ImVec2 c(closePos.x + btnSize * 0.5f, closePos.y + btnSize * 0.5f);
        float s = 5.0f;
        dl->AddLine(ImVec2(c.x - s, c.y - s), ImVec2(c.x + s, c.y + s), closeCol, 1.8f);
        dl->AddLine(ImVec2(c.x + s, c.y - s), ImVec2(c.x - s, c.y + s), closeCol, 1.8f);
        ImGui::PopID();
    }

    // Minimize button
    {
        ImVec2 minPos(p1.x - btnPad - btnSize - 30.0f, centerY - btnSize * 0.5f);
        ImGui::SetCursorScreenPos(minPos);
        ImGui::PushID("##min");
        if (ImGui::InvisibleButton("##min_btn", ImVec2(btnSize, btnSize))) {
            HWND hwnd = NativeWindow::GetHandle();
            if (hwnd) ShowWindow(hwnd, SW_MINIMIZE);
        }
        ImU32 minCol = ImGui::IsItemHovered()
            ? IM_COL32(230, 235, 250, 255) : IM_COL32(180, 185, 200, 180);
        ImVec2 c(minPos.x + btnSize * 0.5f, minPos.y + btnSize * 0.5f);
        dl->AddLine(ImVec2(c.x - 5.0f, c.y), ImVec2(c.x + 5.0f, c.y), minCol, 1.8f);
        ImGui::PopID();
    }

    // Drag area (everything except the buttons)
    ImGui::SetCursorScreenPos(p0);
    float dragW = width - btnPad - btnSize - 30.0f - btnSize - 10.0f;
    ImGui::InvisibleButton("##titlebar_drag", ImVec2(dragW, kTitleBarHeight));
    if (ImGui::IsItemActivated())
        NativeWindow::StartDrag();

    // --- Logo ---
    const float logoSize = 28.0f;
    const ImVec2 lg0 = ImVec2(p0.x + 22.0f, centerY - logoSize * 0.5f);
    const ImVec2 lg1 = ImVec2(lg0.x + logoSize, lg0.y + logoSize);
    const ImU32 top = IM_COL32(59, 146, 246, 255);
    const ImU32 bot = IM_COL32(74, 198, 254, 255);
    dl->AddRectFilledMultiColor(lg0, lg1, top, top, bot, bot);
    dl->AddRect(lg0, lg1, IM_COL32(255, 255, 255, 40), 8.0f, 0, 1.0f);

    const float cx = lg0.x + logoSize * 0.40f;
    const float cy = centerY;
    const float ch = logoSize * 0.22f;
    const ImU32 white = IM_COL32(255, 255, 255, 235);
    dl->AddLine(ImVec2(cx - ch, cy - ch), ImVec2(cx + ch, cy), white, 2.2f);
    dl->AddLine(ImVec2(cx + ch, cy), ImVec2(cx - ch, cy + ch), white, 2.2f);

    // --- Title ---
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

    // --- Subtitle ---
    ImFont* bodyFont = NeonStyle::BodyFont();
    const float subSize = 13.0f;
    const char* sub = "Enterprise DLL Injection Suite";
    ImVec2 subSz = bodyFont->CalcTextSizeA(subSize, FLT_MAX, -1.0f, sub);
    float subX = p1.x - subSz.x - btnPad - btnSize - 30.0f - btnSize - 20.0f;
    dl->AddText(bodyFont, subSize,
                ImVec2(subX, centerY - subSize * 0.5f),
                pal.TextMuted, sub);
}

void GUI::DrawSectionHeader(const char* label) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;

    ImFont* font = NeonStyle::BodyFont();
    float fontSize = 13.0f;
    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, -1.0f, label);

    float lineY = pos.y + textSize.y * 0.5f;
    ImU32 lineColor = IM_COL32(59, 146, 246, 50);
    ImU32 textColor = IM_COL32(143, 150, 166, 200);

    dl->AddLine(ImVec2(pos.x, lineY), ImVec2(pos.x + 6, lineY), lineColor, 1.0f);
    dl->AddText(font, fontSize, ImVec2(pos.x + 12, pos.y), textColor, label);
    float textEnd = pos.x + 12 + textSize.x + 6;
    dl->AddLine(ImVec2(textEnd, lineY), ImVec2(pos.x + width, lineY), lineColor, 1.0f);

    ImGui::Dummy(ImVec2(0, textSize.y + 6));
}

void GUI::DrawContent() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 14));
    ImGui::BeginChild("##content", ImVec2(0, 0), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoScrollbar);

    // === PROCESS SECTION ===
    DrawSectionHeader("PROCESSO ALVO");
    processPanel->Draw();

    ImGui::Spacing();
    ImGui::Spacing();

    // === DLL + OPTIONS (two columns) ===
    if (ImGui::BeginTable("##payload_opts", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("##dll", ImGuiTableColumnFlags_WidthStretch, 0.62f);
        ImGui::TableSetupColumn("##opt", ImGuiTableColumnFlags_WidthStretch, 0.38f);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
        DrawSectionHeader("DLL PAYLOAD");
        injectionPanel->DrawDLLSection();
        ImGui::PopStyleVar();

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
        DrawSectionHeader("OPCOES");
        injectionPanel->DrawOptionsSection();
        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // === INJECT BUTTON + PROGRESS ===
    injectionPanel->DrawInjectSection();

    ImGui::Spacing();
    ImGui::Spacing();

    // === LOG SECTION ===
    DrawSectionHeader("LOG DE ATIVIDADE");
    logPanel->Draw();

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
