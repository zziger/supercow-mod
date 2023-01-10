#include <chrono>
#include "FpsLimitModule.h"

#include "Config.h"
#include "events/GameLoadedEvent.h"
#include "events/TickEvent.h"

using namespace std::chrono;
using namespace std::chrono_literals;

void FpsLimitModule::OnLoad(bool manual) {
    frameStart = time_point_cast<milliseconds>(high_resolution_clock::now());
    UpdateFrameTimeLimit();
    
    OnEvent<BeforeTickEvent>([this] {
        frameStart = time_point_cast<milliseconds>(high_resolution_clock::now());
    });

    OnEvent<AfterTickEvent>([this]() {
        if (!sdk::Game::IsGameLoaded()) return;
        auto end = high_resolution_clock::now();
        auto time = end - frameStart;
        if (time < frameTimeLimit) std::this_thread::sleep_for(frameTimeLimit - time);
    });
}

void FpsLimitModule::RenderModuleUI() {
    if (ImGui::TreeNode("Настройки")) {
        if (ImGui::SliderInt("Лимит FPS", &fpsLimit, 15, 200)) {
            const Config cfg;
            cfg.data[id]["limit"] = fpsLimit;
            UpdateFrameTimeLimit();
        }
        ImGui::TreePop();
    }
}
void FpsLimitModule::UpdateFrameTimeLimit() {
    frameTimeLimit = milliseconds((int) std::floor(1000.0 / 120));
}