#pragma once
#include "modloader/mods/Module.h"

class RenderUnfocusedModule final : public Module {
public:
    RenderUnfocusedModule() :
        Module(
            "renderUnfocused",
            "Рендер вне фокуса",
            "Разрешает игре рендериться когда окно не в фокусе.\n\n"
            "До 1.0.0 модуль назывался RENDER_UNFOCUSED", false) {
    }

    void OnLoad(bool manual) override;
};
