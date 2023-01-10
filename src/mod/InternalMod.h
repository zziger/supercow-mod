#pragma once
#include "modloader/mods/Mod.h"
#include "modules/AdaptiveResolutionModule.h"
#include "modules/ForwardGameLogsModule.h"
#include "modules/FpsLimitModule.h"
#include "modules/LevelBackRenderFixModule.h"
#include "modules/RenderUnfocusedModule.h"

class InternalMod final : public Mod {
public:
    explicit InternalMod() : Mod(ModInfo { "$internal", "SuperMod", "zziger", VERSION }) {}

    void OnLoad() override {
        AddModules<AdaptiveResolutionModule, LevelBackRenderFixModule, FpsLimitModule, RenderUnfocusedModule, ForwardGameLogsModule>();
    }
};
