# Injector Pro - Projeto Educacional

Interface gráfica moderna com ImGui para estudo de programação de sistemas no Windows.

## 📋 Requisitos

- **Visual Studio 2022** (Community Edition ou superior)
- **Windows 10/11 x64**
- **DirectX 11** (pré-instalado no Windows)

## 🏗️ Estrutura do Projeto

```
Injector/
├── include/          # Headers
├── src/              # Implementações
├── external/
│   └── imgui/        # Interface ImGui (git submodule)
├── Injector.sln      # Solução Visual Studio
└── Injector.vcxproj  # Arquivo de projeto
```

## 🚀 Como Compilar

### Opção 1: Visual Studio IDE

1. Abrir `Injector.sln` no Visual Studio 2022
2. Build → Build Solution (Ctrl+Shift+B)
3. Executar (F5)

### Opção 2: Linha de Comando

```bash
# Build Debug
msbuild Injector.sln /p:Configuration=Debug /p:Platform=x64

# Build Release
msbuild Injector.sln /p:Configuration=Release /p:Platform=x64
```

## 📦 Dependências

### Incluídas
- **ImGui** (git submodule)
- DirectX 11 SDK (Windows)
- C++ Standard Library

### Nota sobre CMake
O projeto usa `.vcxproj` e `.sln` para compatibilidade máxima com Visual Studio.
Se preferir usar CMake no futuro, o `CMakeLists.txt` está disponível.

## 🎯 Objetivo Educacional

Este projeto é para aprender:
- ✓ Programação de sistemas no Windows
- ✓ Interface gráfica com ImGui + DirectX 11
- ✓ Gerenciamento de processos (API Windows)
- ✓ Conceitos de injeção de DLL (educacional)
- ✓ Arquitetura modular de software

## ⚖️ Avisos Legais

**USO EDUCACIONAL APENAS**
- Não usar contra sistemas sem autorização
- Respeitar leis de segurança cibernética
- Este é um projeto de estudo, não de produção

## 📝 Próximos Passos

1. Implementar selecionador de DLL (FileDialog)
2. Adicionar thread de injeção
3. Implementar Logger com cores
4. Expandir para múltiplos métodos de injeção

## 👨‍💻 Desenvolvimento

Estrutura modular permite adicionar novos componentes facilmente:

```cpp
// Novo painel
class NovoPanel {
    void Draw();
    void Update();
};

// Adicionar à GUI.h
std::unique_ptr<NovoPanel> novoPanel;
```

## 🔧 Troubleshooting

**Erro de compilação sobre imgui:**
- Certifique-se de que git submodule foi inicializado: `git submodule update --init --recursive`

**Erro de DirectX 11:**
- Instale Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/

**Erro ao executar:**
- Verifique se está em Windows 10/11 x64
- Verifique se D3D11.dll está disponível (cmd: `dxdiag`)
