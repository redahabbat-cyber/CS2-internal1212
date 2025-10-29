#include <windows.h>
#include <fstream>
#define LOG(msg) { std::ofstream log("C:\\cheat_log.txt", std::ios::app); log << msg << std::endl; log.close(); }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        LOG("DLL Attached! HModule: " << std::hex << hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);  // Your init thread
        break;
    case DLL_PROCESS_DETACH:
        LOG("DLL Detached!");
        break;
    }
    return TRUE;
}

DWORD WINAPI MainThread(HMODULE hModule) {
    LOG("MainThread started");
    // Your hook init here
    while (true) {
        if (GetAsyncKeyState(VK_INSERT) & 1) {  // Toggle on press
            LOG("Insert pressed! Toggling menu.");
            // Your menu toggle code
        }
        Sleep(10);  // Don't spam
    }
    return 0;
}