#pragma once
#include <cstdint>  // For uintptr_t if needed

namespace features {
    extern bool bESP;
    extern bool bChams;
    extern bool bWallhack;      // Added
    extern bool bAimbot;
    extern float aimSmooth;
    extern int aimFOV;          // Added
    extern bool bBunnyHop;
    extern bool bNoRecoil;      // Added

    void RunESP();
    void RunAimbot();
    void RunBunnyHop();
    void RunNoRecoil();         // Added
}

// Vector3 struct (add here to avoid undeclared)
struct Vector3 {
    float x, y, z;
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }
};