#include "menu.h"
#include "features.h"
#include "log.h"
#include <windows.h>

extern bool g_bMenuOpen;

void RenderMenu() {
    if (!g_bMenuOpen) return;

    ImGui::Begin("My Cheat Menu v1.212 - CS2 Internal", &g_bMenuOpen, ImGuiWindowFlags_MenuBar);
    ImGui::Text("Status: Injected & Ready | Press INSERT to toggle");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Visuals")) {
        ImGui::Checkbox("ESP Boxes", &features::bESP);
        ImGui::Checkbox("Chams", &features::bChams);
        ImGui::Checkbox("Wallhack", &features::bWallhack);
    }

    if (ImGui::CollapsingHeader("Aimbot")) {
        ImGui::Checkbox("Enabled", &features::bAimbot);
        ImGui::SliderFloat("Smoothness", &features::aimSmooth, 0.1f, 2.0f, "%.1f");
        ImGui::SliderInt("FOV", &features::aimFOV, 1, 180);
    }

    if (ImGui::CollapsingHeader("Misc")) {
        ImGui::Checkbox("BunnyHop", &features::bBunnyHop);
        ImGui::Checkbox("No Recoil", &features::bNoRecoil);
        if (ImGui::Button("Unload Cheat")) {
            HMODULE hMod = GetModuleHandle(nullptr);
            if (hMod) FreeLibraryAndExitThread(hMod, 0);
            LOG("Unload button pressed - Exiting");
        }
    }

    ImGui::Separator();
    ImGui::Text("Build: Oct 29, 2025 | Test on -insecure");
    ImGui::End();
}