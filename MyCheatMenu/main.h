#pragma once
#include <windows.h>  // Fixed: Defines WINAPI, LPVOID, DWORD - prevents redefs/syntax errors
#include "features.h"  // For feature calls
#include "log.h"       // For LOG

// Forward declare globals if needed (from hooks.h)
extern bool g_bMenuOpen;

// Run enabled features every frame (called from hkPresent)
void RunFeatures();

// Optional feature monitoring thread (start in MainThread if desired)
DWORD WINAPI FeatureThread(LPVOID param);  // Fixed: Now parses correctly