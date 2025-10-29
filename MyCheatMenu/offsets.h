#pragma once
#include <cstdint>

namespace offsets {
    // Oct 29, 2025 values - full offsets (load + base in MainThread)
    extern uintptr_t dwLocalPlayerPawn;
    extern uintptr_t dwEntityList;
    extern uintptr_t dwViewMatrix;
    constexpr auto m_iHealth = 0x334;
    constexpr auto m_vecOrigin = 0x1274;
    // Add more as needed: constexpr auto m_iTeamNum = 0x3C3;
}