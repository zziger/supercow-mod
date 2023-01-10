#pragma once
#include <imgui-dx8/imgui_impl_dx8.h>

#include "Config.h"
#include "data.h"
#include "Utils.h"
#include "events/GameLoadedEvent.h"
#include "events/TickEvent.h"
#include "events/WindowEvent.h"
#include "modloader/mods/Module.h"
#include "sdk/DirectX.h"
#include "sdk/Game.h"
#include "thirdparty/IconsMaterialDesign.h"
#include "ui/ImGuiWidgets.h"

class AdaptiveResolutionModule final : public Module {
public:
    AdaptiveResolutionModule() :
        Module(
            "adaptiveResolution",
            "Адаптивное разрешение",
            "Позволяет игре рендериться в другом разрешении,\n"
            "а так же разрешает менять размер окна.\n\n"
            "До 1.0.0 модуль назывался WIDESCREEN_FIX", true) {
    }

    enum resolution_mode {
        window,
        screen,
        custom
    };

    static vector2i GetTargetResolution();
    
    static constexpr int required_styles = WS_MAXIMIZEBOX | WS_SIZEBOX;
    
    void OnLoad(bool manual) override;

    void OnUnload(bool manual) override;

    void RenderModuleUI() override;
};
