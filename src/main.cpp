#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <wrl/client.h>

#include "GUI.h"
#include "ImGuiStyle.h"
#include "Logger.h"

using Microsoft::WRL::ComPtr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class DX11Window {
private:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> deviceContext;
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    HWND hwnd;
    bool isRunning = true;

    static DX11Window* g_pThis;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        if (g_pThis) {
            return g_pThis->HandleMessage(hWnd, msg, wParam, lParam);
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_CLOSE:
            case WM_DESTROY:
                isRunning = false;
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

public:
    DX11Window() : hwnd(NULL) {
        g_pThis = this;
    }

    ~DX11Window() {
        Shutdown();
        g_pThis = NULL;
    }

    bool Initialize(int width, int height, const char* title) {
        // Criar classe de janela
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_CLASSDC;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = L"InjectorWindow";
        RegisterClassExW(&wc);

        // Criar janela
        hwnd = CreateWindowW(wc.lpszClassName,
            std::wstring(title, title + strlen(title)).c_str(),
            WS_OVERLAPPEDWINDOW, 100, 100, width, height,
            NULL, NULL, wc.hInstance, NULL);

        if (!hwnd) {
            Logger::Error("Falha ao criar janela");
            return false;
        }

        // Criar device D3D11
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
        };

        if (FAILED(D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
            featureLevelArray, 2, D3D11_SDK_VERSION,
            &sd, &swapChain, &device, &featureLevel, &deviceContext))) {
            Logger::Error("Falha ao criar D3D11 device");
            return false;
        }

        // Criar render target view
        ComPtr<ID3D11Texture2D> backBuffer;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        device->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView);

        // Viewport
        D3D11_VIEWPORT vp = {};
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        deviceContext->RSSetViewports(1, &vp);

        // Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());

        // Setup estilo
        ImGuiStyle::SetupNeonStyle();
        ImGuiStyle::LoadFonts();

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);

        Logger::Success("DX11 Window inicializado com sucesso");
        return true;
    }

    void Shutdown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (hwnd) {
            DestroyWindow(hwnd);
            hwnd = NULL;
        }
    }

    bool IsRunning() const {
        return isRunning;
    }

    bool Render(std::function<void()> renderUI) {
        MSG msg = {};
        while (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT)
                return false;
        }

        if (!isRunning)
            return false;

        // Clear
        float clearColor[4] = { 0.05f, 0.05f, 0.1f, 1.0f };
        deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);
        deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render UI
        renderUI();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swapChain->Present(1, 0);

        return true;
    }
};

DX11Window* DX11Window::g_pThis = nullptr;

int main() {
    Logger::Info("Iniciando Injector Pro");

    DX11Window window;
    if (!window.Initialize(1200, 700, "Injector Pro")) {
        Logger::Error("Falha ao inicializar janela");
        return -1;
    }

    GUI gui;

    Logger::Success("Aplicao iniciada");

    while (window.IsRunning()) {
        float deltaTime = 0.016f; // 60 FPS

        gui.Update(deltaTime);

        if (!window.Render([&gui]() {
            gui.Render();
        })) {
            break;
        }
    }

    Logger::Info("Encerrando aplicao");
    window.Shutdown();

    return 0;
}
