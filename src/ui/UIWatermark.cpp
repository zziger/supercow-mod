#include "UI.h"
#include "Utils.h"

using namespace ui;

int triggered = 0;

void UI::RenderWatermark() {
    if (!showWatermark) return;
        
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 workPos = viewport->WorkPos;
    const ImVec2 workSize = viewport->WorkSize;
    constexpr float margin = 10.0f;
    float x = 0.0f, xPivot = 0.0f, y = 0.0f, yPivot = 0.0f;

    switch(watermarkPosition) {
        case TOP_LEFT:
        case TOP_CENTER:
        case TOP_RIGHT:
            y = margin;
            yPivot = 0;
            break;
        case BOTTOM_LEFT:
        case BOTTOM_CENTER:
        case BOTTOM_RIGHT:
            y = workSize.y - margin;
            yPivot = 1.f;
    }

    switch(watermarkPosition) {
        case TOP_LEFT:
        case BOTTOM_LEFT:
            x = margin;
            xPivot = 0;
            break;
        case TOP_CENTER:
        case BOTTOM_CENTER:
            x = workSize.x * 0.5f;
            xPivot = 0.5f;
            break;
        case TOP_RIGHT:
        case BOTTOM_RIGHT:
            x = workSize.x - margin;
            xPivot = 1.f;
    }
    
    ImGui::SetNextWindowPos({ workPos.x + x, workPos.y + y }, ImGuiCond_Always, { xPivot, yPivot });
    ImGui::SetNextWindowBgAlpha(watermarkBgOpacity * watermarkOpacity);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, watermarkOpacity);
    if (ImGui::Begin("watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize
                     | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove)) {
        ImGui::TextColored(0xFFCC00FF_color, "SuperMod");
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

// inline int (__thiscall *debug_orig)(void*, double, double) = nullptr;
// inline int __fastcall debug(void* this_, void*, double a, double b) {
//     triggered++;
//     return debug_orig(this_, a, b);
// }

// HOOK_FN(inline void, debug, ARGS()) {
//     MessageBoxA(nullptr, "amogus", "sussybaka", MB_OK);
//     debug_orig();
// }
//
// EventManager::Ready $debug([]() {
//     HookManager::RegisterHook("55 8B EC 51 89 4D ? E8 ? ? ? ? 8B E5 5D C3 55 8B EC 51 89 4D ? E8 ? ? ? ? 8B E5 5D C3 55 8B EC 51 89 4D ? 8B 4D", HOOK_REF_FORCE(debug));
// });