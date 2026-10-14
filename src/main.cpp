#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <dxgi.h>
#include <functional>
#include <string>
#include <cstring>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <wrl/client.h>

#include "GUI.h"
#include "ImGuiStyle.h"
#include "Logger.h"
#include "NativeWindow.h"

using Microsoft::WRL::ComPtr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Handle global da janela principal, usado pela ponte NativeWindow para
// permitir o arraste a partir do cabecalho personalizado da UI.
static HWND g_MainHwnd = nullptr;

// Espessura (em pixels) da borda invisivel de redimensionamento.
static const int kResizeBorder = 6;

namespace NativeWindow {
    void StartDrag() {
        if (!g_MainHwnd)
            return;
        // Encerra qualquer captura do mouse feita pelo ImGui e delega o
        // arraste ao gerenciador de janelas do Windows (loop modal nativo).
        ReleaseCapture();
        SendMessageW(g_MainHwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }
}

class DX11Window {
private:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> deviceContext;
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    HWND hwnd;
    UINT width = 0;
    UINT height = 0;
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

    // Faz o hit-test das bordas para permitir o redimensionamento numa janela
    // sem moldura (a area central continua sendo cliente / ImGui).
    LRESULT HitTestBorders(HWND hWnd, LPARAM lParam) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        RECT rc;
        GetWindowRect(hWnd, &rc);

        const bool left   = pt.x >= rc.left   && pt.x <  rc.left   + kResizeBorder;
        const bool right  = pt.x <  rc.right   && pt.x >= rc.right  - kResizeBorder;
        const bool top    = pt.y >= rc.top     && pt.y <  rc.top    + kResizeBorder;
        const bool bottom = pt.y <  rc.bottom  && pt.y >= rc.bottom - kResizeBorder;

        if (top && left)     return HTTOPLEFT;
        if (top && right)    return HTTOPRIGHT;
        if (bottom && left)  return HTBOTTOMLEFT;
        if (bottom && right) return HTBOTTOMRIGHT;
        if (left)            return HTLEFT;
        if (right)           return HTRIGHT;
        if (top)             return HTTOP;
        if (bottom)          return HTBOTTOM;
        return HTCLIENT;
    }

    void ResizeSwapChain(UINT newWidth, UINT newHeight) {
        if (!swapChain || !device || newWidth == 0 || newHeight == 0)
            return;

        width = newWidth;
        height = newHeight;

        renderTargetView.Reset();
        swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);

        ComPtr<ID3D11Texture2D> backBuffer;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        device->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView);

        D3D11_VIEWPORT vp = {};
        vp.Width = (FLOAT)newWidth;
        vp.Height = (FLOAT)newHeight;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        deviceContext->RSSetViewports(1, &vp);
    }

    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            // Remove a moldura padrao mantendo a janela redimensionavel e com
            // sombra do DWM: a area de cliente passa a ocupar a janela inteira.
            case WM_NCCALCSIZE:
                if (wParam == TRUE)
                    return 0;
                break;

            case WM_NCHITTEST:
                return HitTestBorders(hWnd, lParam);

            case WM_SIZE:
                if (wParam != SIZE_MINIMIZED)
                    ResizeSwapChain((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
                return 0;

            // Impede que a janela fique menor que um tamanho utilizavel.
            case WM_GETMINMAXINFO: {
                MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                mmi->ptMinTrackSize.x = 900;
                mmi->ptMinTrackSize.y = 560;
                return 0;
            }

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

    bool Initialize(int w, int h, const char* title) {
        width = (UINT)w;
        height = (UINT)h;

        // Classe de janela
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.lpszClassName = L"InjectorWindow";
        RegisterClassExW(&wc);

        // Centraliza na area de trabalho
        RECT work = {};
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
        int posX = work.left + ((work.right - work.left) - w) / 2;
        int posY = work.top + ((work.bottom - work.top) - h) / 2;

        // Janela sem moldura: WS_POPUP remove barra de titulo/bordas.
        // WS_THICKFRAME preserva o redimensionamento e a sombra do DWM;
        // WS_CAPTION/WS_SYSMENU mantem animacoes e integracao nativas.
        // Sem WS_MAXIMIZEBOX: nao ha maximizar (requisito) e evita que uma
        // janela sem moldura maximizada cubra a barra de tarefas.
        DWORD style = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU |
                      WS_MINIMIZEBOX | WS_CLIPCHILDREN;

        hwnd = CreateWindowExW(
            WS_EX_APPWINDOW,
            wc.lpszClassName,
            std::wstring(title, title + strlen(title)).c_str(),
            style, posX, posY, w, h,
            NULL, NULL, wc.hInstance, NULL);

        if (!hwnd) {
            Logger::Error("Falha ao criar janela");
            return false;
        }

        g_MainHwnd = hwnd;

        // Sombra nativa do DWM numa janela sem moldura.
        MARGINS margins = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        // Device D3D11
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
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

        // Render target view
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
        io.IniFilename = nullptr; // nao persistir layout em disco

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());

        // Estilo e fontes
        NeonStyle::LoadFonts();
        NeonStyle::SetupNeonStyle();

        ShowWindow(hwnd, SW_SHOW);
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
            g_MainHwnd = nullptr;
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

        if (!renderTargetView)
            return true;

        // Cor de limpeza alinhada ao fundo grafite do tema (evita flash preto).
        float clearColor[4] = { 0.067f, 0.075f, 0.094f, 1.0f };
        deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);
        deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

        // Frame ImGui
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        renderUI();

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
    if (!window.Initialize(1200, 720, "Injector Pro")) {
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
