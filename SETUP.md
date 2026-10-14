# Setup do Projeto Injector Pro

## 1️⃣ Pré-Requisitos

### Windows
- Windows 10/11 (x64)
- Visual Studio 2022 Community (ou superior)
- Git para Windows

### Verificar Instalação

```bash
# Verificar Visual Studio
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

# Verificar Git
git --version
```

## 2️⃣ Preparar o Projeto

### Clonar Repositório
```bash
git clone <seu-repo> Injector
cd Injector
```

### Inicializar Submodules
```bash
git submodule update --init --recursive
```

Isso baixará ImGui (~10 MB).

### Verificar Estrutura
```bash
# Verificar se tudo foi baixado
dir external\imgui\imgui.cpp    # Deve existir
dir include\*.h                  # Deve ter 7 headers
dir src\*.cpp                    # Deve ter 8 sources
```

## 3️⃣ Compilar com Visual Studio

### Opção A: IDE
1. Abrir `Injector.sln`
2. Selecionar **Release** e **x64**
3. Clicar em Build → Build Solution
4. Verificar Output window para erros

### Opção B: Command Line
```bash
# Abrir Developer Command Prompt
cd C:\Users\erick.vessoli\Projetos\Injector

# Build Debug
msbuild Injector.sln /p:Configuration=Debug /p:Platform=x64 /m

# Build Release
msbuild Injector.sln /p:Configuration=Release /p:Platform=x64 /m
```

O executável será criado em: `bin\Release\Injector.exe`

## 4️⃣ Resolver Problemas Comuns

### Erro: "Cannot find imgui.cpp"
```bash
# Reinicializar submodules
git submodule deinit -all
git submodule update --init --recursive
```

### Erro: "D3D11.h not found"
- Instalar Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
- Na instalação do VS, marcar "Windows 10/11 SDK"

### Erro: "LNK1104: cannot open file 'd3d11.lib'"
- Verificar: Project Properties → VC++ Directories
- Adicionar: `C:\Program Files (x86)\Windows Kits\10\Lib\10.0.xxxxx\um\x64`

### Compilação lenta
- Usar `/m` flag: `msbuild Injector.sln /m` (multi-core)
- Usar Incremental Link Time Code Generation: OFF

## 5️⃣ Estrutura de Pastas Esperada

```
Injector/
├── .git/                 # Repositório Git
├── .gitignore           # Arquivo de ignore
├── .gitmodules          # Submodules
├── bin/
│   ├── Debug/
│   └── Release/         # Executáveis compilados
├── build/               # Arquivos intermediários CMake (se usar)
├── external/
│   └── imgui/           # ImGui (git submodule)
│       ├── imgui.cpp
│       ├── imgui.h
│       ├── backends/
│       │   ├── imgui_impl_dx11.cpp
│       │   ├── imgui_impl_win32.cpp
│       │   └── ...
│       └── ...
├── include/             # Headers do projeto
│   ├── GUI.h
│   ├── Logger.h
│   ├── ProcessManager.h
│   └── ...
├── src/                 # Implementações
│   ├── main.cpp
│   ├── GUI.cpp
│   ├── Logger.cpp
│   └── ...
├── obj/                 # Arquivos objeto (criados durante build)
├── CMakeLists.txt       # Para CMake (opcional)
├── Injector.sln         # Solução Visual Studio
├── Injector.vcxproj     # Projeto Visual Studio
├── README.md
└── SETUP.md
```

## 6️⃣ Testar Depois de Compilar

```bash
# Executar
./bin/Release/Injector.exe

# Você deve ver:
# ✓ Janela ImGui abrindo
# ✓ Console com logs coloridos
# ✓ Aba "Processos", "Injeo", "Logs"
```

## 7️⃣ Próximas Etapas

### Para Desenvolvimento
1. Adicionar novos painéis em `src/`
2. Adicionar headers em `include/`
3. Atualizar `CMakeLists.txt` com novos arquivos
4. Recompilar

### Para Funcionalidades
1. Implementar `FileDialog` para seleção de DLL
2. Implementar `MemoryInjector` (injeção real)
3. Adicionar barra de progresso
4. Testes com antivírus

## 📚 Referências

- ImGui: https://github.com/ocornut/imgui
- DirectX 11: https://docs.microsoft.com/en-us/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11
- Windows Process: https://docs.microsoft.com/en-us/windows/win32/procthread/about-processes-and-threads
