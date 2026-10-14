#pragma once
#include "ProcessListPanel.h"
#include "InjectionPanel.h"
#include "LogPanel.h"
#include <memory>

class GUI {
private:
    std::unique_ptr<ProcessListPanel> processPanel;
    std::unique_ptr<InjectionPanel> injectionPanel;
    std::unique_ptr<LogPanel> logPanel;

    bool windowOpen = true;

    // Cabecalho personalizado (barra de arraste) e area de conteudo.
    void DrawTitleBar();
    void DrawContent();

public:
    GUI();
    ~GUI();

    void Update(float deltaTime);
    void Render();

    bool IsRunning() const { return windowOpen; }
};
