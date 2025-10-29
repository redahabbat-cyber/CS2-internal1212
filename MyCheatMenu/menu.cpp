#include <windows.h> // ADDED: For PostQuitMessage
#include "imgui.h"
#include "globals.h"
#include "offsets.h"

void RenderMenu()
{
    ImGui::Begin("CS2 Cheese v1.0", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Status: Injected & Running");
    ImGui::Separator();

    if (g_localPlayer)
    {
        int health = *(int*)((uintptr_t)g_localPlayer + offsets::m_iHealth);
        ImGui::Text("Health: %d", health);
    }
    else
    {
        ImGui::Text("Health: N/A (not in game)");
    }

    ImGui::Separator();
    ImGui::Text("Features (add in main.cpp):");
    ImGui::BulletText("Aimbot");
    ImGui::BulletText("ESP");
    ImGui::BulletText("Triggerbot");

    ImGui::Separator();
    if (ImGui::Button("Unload Cheat (END key)"))
        PostQuitMessage(0); // Now works with <windows.h>

    ImGui::End();
}