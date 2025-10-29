#pragma once
#include <d3d11.h>
#include <windows.h>  // For HMODULE, HWND, etc.
#include <psapi.h>    // For MODULEINFO/GetModuleInformation
#include "kiero.h"    // Kiero hooking (per vcxproj)
#include "MinHook.h"  // MinHook (backend for Kiero)
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Globals
extern bool g_bMenuOpen;
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern HWND g_hwnd;

// Typedefs
typedef HRESULT(__stdcall* PresentFn)(IDXGISwapChain*, UINT, UINT);
extern PresentFn oPresent;

// Function Declarations
void SetupHooks();
void CleanupHooks();
void* FindPattern(const char* module, const char* sig);
DWORD WINAPI MainThread(LPVOID param);
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
void SetupImGui(IDXGISwapChain* pSwapChain);

// Externals from main.h (for feature integration)
extern void RunFeatures();
extern DWORD WINAPI FeatureThread(LPVOID param);