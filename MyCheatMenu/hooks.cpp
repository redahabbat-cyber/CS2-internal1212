#pragma warning(disable:4305 4309)  // Suppress XorStr warnings

#include "hooks.h"
#include "menu.h"
#include "main.h"  // RunFeatures
#include "offsets.h"
#include "log.h"
#include "kiero.h"
#include <TlHelp32.h>
#include <psapi.h>
#include <sstream>

using namespace std;

bool g_bMenuOpen = false;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
HWND g_hwnd = nullptr;
PresentFn oPresent = nullptr;

// Simplified XOR
std::string XorStr(const char* str, char key = 0xDE) {
    std::string result = str;
    for (auto& c : result) c ^= key;
    return result;
}

// Sig scanner (fallback)
void* FindPattern(const char* moduleName, const char* signature) {
    HMODULE hMod = GetModuleHandleA(moduleName);
    if (!hMod) return nullptr;
    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(MODULEINFO))) return nullptr;
    BYTE* base = static_cast<BYTE*>(modInfo.lpBaseOfDll);
    DWORD size = modInfo.SizeOfImage;
    int sigLen = static_cast<int>(strlen(signature));

    for (DWORD i = 0; i < size - sigLen; ++i) {
        bool found = true;
        for (int j = 0; j < sigLen; ++j) {
            if (signature[j] != '?' && base[i + j] != static_cast<BYTE>(signature[j])) {
                found = false;
                break;
            }
        }
        if (found) return base + i;
    }
    return nullptr;
}

void SetupImGui(IDXGISwapChain* pSwapChain) {
    if (g_pd3dDevice) return;

    if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice))) {
        g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);

        if (!g_hwnd) {
            LOG("ERROR: No HWND for ImGui - skipping init");
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplWin32_Init(g_hwnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
        LOG("ImGui Initialized Successfully");
    }
    else {
        LOG("ERROR: Failed to get D3D11 Device");
    }
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool inited = false;
    if (!inited) {
        SetupImGui(pSwapChain);
        inited = true;
    }

    if (g_pd3dDevice) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (GetForegroundWindow() != g_hwnd) return oPresent(pSwapChain, SyncInterval, Flags);

        static bool lastInsert = false;
        bool currentInsert = GetAsyncKeyState(VK_INSERT) & 0x8000;
        if (currentInsert && !lastInsert) {
            g_bMenuOpen = !g_bMenuOpen;
            LOG("KEY: Menu toggled to " + to_string(g_bMenuOpen));
        }
        lastInsert = currentInsert;

        ImGui::GetBackgroundDrawList()->AddText(ImVec2(10, 10), IM_COL32(255, 0, 0, 255), "CHEAT ACTIVE");

        std::ostringstream oss;
        oss << "Menu: " << (g_bMenuOpen ? "ON" : "OFF");
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(10, 30), IM_COL32(255, 255, 255, 255), oss.str().c_str());

        if (g_bMenuOpen) RenderMenu();
        RunFeatures();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void SetupHooks() {
    LOG("Setting up hooks with Kiero...");
    if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) {
        LOG("Kiero failed - MinHook fallback");
        MH_Initialize();
        const char* presentSig = "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 57 41 54 41 55 41 56 41 57 48 83 EC 40 0F 29 70 B8 0F 29 78 A0";
        void* addr = FindPattern("dxgi.dll", presentSig);
        if (addr) {
            MH_CreateHook(addr, hkPresent, reinterpret_cast<LPVOID*>(&oPresent));
            MH_EnableHook(addr);
            LOG("MinHook Present hooked at 0x" + to_string(reinterpret_cast<uintptr_t>(addr)));
        }
        else {
            LOG("ERROR: No Present hook - update sig!");
        }
        return;
    }

    void** pOriginal = (void**)&oPresent;
    auto status = kiero::bind(8, pOriginal, (void*)hkPresent);
    if (status == kiero::Status::Success) {
        LOG("HOOK: Present bound via Kiero");
    }
    else {
        LOG("ERROR: Kiero bind failed - status: " + to_string(static_cast<int>(status)));
    }
}

void CleanupHooks() {
    if (g_pd3dDevice) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        g_pd3dDevice->Release();
        g_pd3dDeviceContext->Release();
        g_pd3dDevice = nullptr;
        g_pd3dDeviceContext = nullptr;
    }
    kiero::shutdown();
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    LOG("Hooks cleaned up");
}

DWORD WINAPI MainThread(LPVOID param) {
    LOG("MainThread started - Enumerating modules for client.dll");
    DWORD startTime = GetTickCount();
    uintptr_t clientBase = 0;

    while ((GetTickCount() - startTime) < 30000) {
        HMODULE hMods[1024];
        DWORD cbNeeded;
        if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
            int numMods = cbNeeded / sizeof(HMODULE);
            for (int i = 0; i < numMods; i++) {
                char modName[MAX_PATH];
                if (GetModuleBaseNameA(GetCurrentProcess(), hMods[i], modName, sizeof(modName))) {
                    if (_stricmp(modName, "client.dll") == 0) {
                        clientBase = reinterpret_cast<uintptr_t>(hMods[i]);
                        LOG("client.dll found via enum - base: 0x" + to_string(clientBase));
                        break;
                    }
                }
            }
            if (clientBase) {
                break;
            }
        }

        if (GetModuleHandleA("engine2.dll") || GetModuleHandleA("tier0.dll")) {
            LOG("Alt module found - continuing enum");
        }

        Sleep(500);
    }

    if (!clientBase) {
        LOG("ERROR: Timeout - client.dll not found. Inject after full map load.");
        return 1;
    }

    LOG("Loading offsets from client.dll base");
    offsets::dwLocalPlayerPawn = clientBase + 0x1AF4A20;
    offsets::dwEntityList = clientBase + 0x1E019A0;
    offsets::dwViewMatrix = clientBase + 0x1D21800;
    LOG("Offsets loaded: LocalPawn=0x" + to_string(offsets::dwLocalPlayerPawn) +
        ", EntityList=0x" + to_string(offsets::dwEntityList));

    // Simple HWND find (no enum)
    g_hwnd = FindWindowA("SDL_app", nullptr);
    if (!g_hwnd) {
        g_hwnd = FindWindowA(nullptr, "Counter-Strike 2");
    }
    if (g_hwnd) {
        LOG("Found CS2 window: SDL_app");
    }
    else {
        LOG("ERROR: No CS2 window found");
    }

    SetupHooks();

    CreateThread(nullptr, 0, FeatureThread, nullptr, 0, nullptr);

    while (true) {
        Sleep(1000);
    }
    return 0;
}