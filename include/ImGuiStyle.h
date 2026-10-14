#pragma once

struct ImFont;

namespace NeonStyle {
    void SetupNeonStyle();
    void LoadFonts();

    // Fontes carregadas em LoadFonts(). Podem retornar nullptr (usa-se a padrao).
    ImFont* TitleFont();  // Fonte maior/destaque para titulos e cabecalho.
    ImFont* BodyFont();   // Fonte padrao do corpo da interface.

    // Paleta do tema, reutilizavel pelos paineis para manter consistencia.
    struct Palette {
        unsigned int Accent;        // Azul/ciano de destaque (ImU32)
        unsigned int AccentDim;     // Destaque suave
        unsigned int Success;       // Verde (estado positivo)
        unsigned int Warning;       // Ambar
        unsigned int Danger;        // Vermelho
        unsigned int TextMuted;     // Texto secundario
        unsigned int Surface;       // Fundo de superficie/card
        unsigned int SurfaceAlt;    // Fundo alternativo
    };
    const Palette& Colors();
}
