#include "main.h"  // Self-include for decls
#include "offsets.h"
#include "log.h"
#include <windows.h>

extern bool g_bMenuOpen;  // From hooks.h

void RunFeatures() {
    if (features::bESP) features::RunESP();
    if (features::bAimbot) features::RunAimbot();
    if (features::bBunnyHop) features::RunBunnyHop();
    if (features::bNoRecoil) features::RunNoRecoil();
    // Add stubs for others
}

DWORD WINAPI FeatureThread(LPVOID param) {
    LOG("FeatureThread started");
    while (true) {
        uintptr_t localPlayer = *(uintptr_t*)(offsets::dwLocalPlayerPawn);
        if (localPlayer) {
            int health = *(int*)(localPlayer + offsets::m_iHealth);
            LOG("Local Health: " + std::to_string(health));
        }
        Sleep(5000);
    }
    return 0;
}