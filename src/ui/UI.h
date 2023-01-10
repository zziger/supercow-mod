#pragma once
#include "UIFonts.h"
#include "events/WindowEvent.h"
#include "imgui/imgui.h"

namespace ui
{
    enum WatermarkPosition : int {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER,
        BOTTOM_RIGHT
    };
    
    class UI {

        static void LoadFonts(const ImGuiIO& io);

        static void RenderWatermark();

        static void RenderModsTab();
        
        static void RenderSettingsTab();
        
        static void RenderToolsTab();
        
        static void RenderMenu();

        static void OnWindowEvent(WindowEvent& evt);

        static void InitImGui();
    
        static void Render();

        static void RenderWindows();

        static inline bool _textureViewerOpen = false;
        static void RenderTextureViewer();

    public:
        static inline bool initialized = false;
        static inline bool menuOpen = false;
        
        static inline bool showWatermark = true;
        static inline WatermarkPosition watermarkPosition = TOP_CENTER;
        static inline float watermarkOpacity = 1.0f;
        static inline float watermarkBgOpacity = 0.35f;
        static inline UIFonts* fonts = nullptr;

        static void Init();
    };
}