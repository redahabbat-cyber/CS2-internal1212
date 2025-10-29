// main.cpp – INTERNAL DLL (Kiero + ImGui DX11)
#include <windows.h>
#include <thread>
#include <chrono>

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "kiero.h"
#include "minhook.h"

#include "offsets.h"
#include "globals.h"
#include "menu.h"

// === GLOBALS ===
HWND                    g_hwnd = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
bool                    g_showMenu = false;
bool                    g_initialized = false;
bool                    g_running = true;

// === PRESENT HOOK ===
using Present_t = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
static Present_t oPresent = nullptr;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!g_initialized)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice)))
        {
            g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);

            DXGI_SWAP_CHAIN_DESC sd{};
            pSwapChain->GetDesc(&sd);
            g_hwnd = sd.OutputWindow;

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;

            ImGui_ImplWin32_Init(g_hwnd);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
            ImGui::StyleColorsDark();

            g_initialized = true;
        }
        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Toggle menu
    if (GetAsyncKeyState(VK_INSERT) & 1)
        g_showMenu = !g_showMenu;

    // Render menu (FIXED: added parentheses)
    if (g_showMenu)
        RenderMenu();

    // 60 FPS cheat loop
    static auto last_tick = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count() >= 16)
    {
        last_tick = now;

        uintptr_t client = (uintptr_t)GetModuleHandleA("client.dll");
        if (client && offsets::dwLocalPlayer)
            g_localPlayer = *(C_BasePlayer**)(client + offsets::dwLocalPlayer);

        // === ADD YOUR FEATURES HERE ===
        // RunAimbot();
        // RunESP();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

// === SETUP ===
void SetupHooks()
{
    MH_Initialize();
    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        kiero::bind(8, (void**)&oPresent, hkPresent);
}

// === CLEANUP ===
void Cleanup()
{
    kiero::shutdown();
    MH_Uninitialize();

    if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
    if (g_pd3dDevice)        g_pd3dDevice->Release();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// === THREAD ===
void CheatThread(HMODULE hModule)
{
    SetupHooks();

    while (!GetModuleHandleA("client.dll"))
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (g_running)
    {
        if (GetAsyncKeyState(VK_END) & 1)
            g_running = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Cleanup();
    FreeLibraryAndExitThread(hModule, 0);
}

// === DLL ENTRY ===
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CheatThread, hModule, 0, nullptr);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        g_running = false;
    }
    return TRUE;
}