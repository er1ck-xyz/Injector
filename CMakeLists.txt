cmake_minimum_required(VERSION 3.16)
project(Injector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# DirectX 11
find_package(directx-headers REQUIRED)
find_package(d3d11 REQUIRED)

# ImGui
set(IMGUI_PATH "${CMAKE_SOURCE_DIR}/external/imgui")

if(NOT EXISTS ${IMGUI_PATH})
    message(FATAL_ERROR "ImGui not found. Run: git submodule update --init --recursive")
endif()

add_library(imgui STATIC
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/backends/imgui_impl_dx11.cpp
    ${IMGUI_PATH}/backends/imgui_impl_win32.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_PATH}
    ${IMGUI_PATH}/backends
)

# Main executable
add_executable(Injector
    src/main.cpp
    src/ImGuiStyle.cpp
    src/GUI.cpp
    src/ProcessListPanel.cpp
    src/ProcessManager.cpp
    src/InjectionPanel.cpp
    src/LogPanel.cpp
    src/Logger.cpp
    src/EnterpriseInjector.cpp
    src/StringEncryption.cpp
    src/AntiReverseEngineering.cpp
    src/HookEvasion.cpp
    src/BehavioralEvasion.cpp
    src/MemoryForensicsEvasion.cpp
    src/ArtifactElimination.cpp
)

target_include_directories(Injector PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_PATH}
    ${IMGUI_PATH}/backends
)

target_link_libraries(Injector PRIVATE
    imgui
    d3d11
    dxgi
    d3dcompiler
)

# Windows specific
if(WIN32)
    target_link_libraries(Injector PRIVATE
        Psapi
        User32
        Dwmapi
        Comdlg32
    )
endif()
