#pragma once
#include "Config.h"
#include "modloader/mods/Module.h"

class FpsLimitModule final : public Module {
public:
    FpsLimitModule() :
        Module(
            "fpsLimit",
            "Ограничение FPS",
            "Позволяет ограничить максимальное количество кадров в секунду.\n\n"
            "До 1.0.0 модуль назывался FPS_LIMIT", false) {
        fpsLimit = Config::Get()[id]["limit"].as<int>(120);
    }

    int fpsLimit;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> frameStart;
    
    void OnLoad(bool manual) override;

    void RenderModuleUI() override;
    
private:
    std::chrono::milliseconds frameTimeLimit;

    void UpdateFrameTimeLimit();
};
