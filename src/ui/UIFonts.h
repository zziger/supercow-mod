#pragma once
#include <imgui/imgui.h>

namespace ui
{
    struct UIFonts {
        ImFont* fontDefault;
        ImFont* fontDefault2X;
        ImFont* fontFab;
        ImFont* fontFab2X;

        explicit UIFonts(const ImGuiIO& io);
    };
}