#include <windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <direct.h>  // For _getcwd
#include "log.h"     // Your LOG macro
#include "hooks.h"   // For MainThread

std::ofstream g_log;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        MessageBoxA(nullptr, "DLL Attached - Starting log setup", "MyCheatMenu", MB_OK | MB_ICONINFORMATION);

        // Get current working dir for log (simple, always works)
        char cwd[MAX_PATH];
        if (_getcwd(cwd, sizeof(cwd)) == nullptr) {
            strcpy_s(cwd, "C:\\temp");  // Ultimate fallback
        }
        strcat_s(cwd, "\\cs2_cheat_log.txt");

        MessageBoxA(nullptr, cwd, "Log Path Attempt", MB_OK);  // Debug: Shows path

        g_log.open(cwd, std::ios::app);
        if (!g_log.is_open()) {
            MessageBoxA(nullptr, "Log open FAILED - Manual check cwd!", "Error", MB_OK | MB_ICONERROR);
            return FALSE;  // Abort
        }

        LOG("DLL Attached - Log opened at " << cwd);

        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        break;
    }
    case DLL_PROCESS_DETACH:
        LOG("DLL Detaching");
        CleanupHooks();
        g_log.close();
        break;
    }
    return TRUE;
}