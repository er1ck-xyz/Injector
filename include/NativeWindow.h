#pragma once

// Ponte entre a interface ImGui e a janela nativa Win32.
// Permite que a UI (sem moldura do Windows) controle o arraste da janela
// a partir de uma area de cabecalho personalizada, sem expor botoes nativos.
namespace NativeWindow {
    // Inicia o arraste nativo da janela (equivale a arrastar pela barra de titulo).
    // Deve ser chamado quando o usuario pressiona a area de arraste do cabecalho.
    void StartDrag();
}
