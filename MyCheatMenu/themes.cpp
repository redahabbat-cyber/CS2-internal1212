// themes.cpp
#include "imgui.h"
#include "themes.h"

void Theme::Dark()
{
    ImGui::StyleColorsDark();
    auto& s = ImGui::GetStyle();
    s.WindowRounding = s.FrameRounding = s.GrabRounding = 0.0f;
}
