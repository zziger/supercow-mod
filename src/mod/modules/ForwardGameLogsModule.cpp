#include "ForwardGameLogsModule.h"

#include "Config.h"
#include "Utils.h"

HOOK_FN(int, debug_log, ARGS(char* format, ...)) {
    if (!ForwardGameLogsModule::writeToLog) return 0;
    va_list va;
    va_start(va, format);
    char buffer[1024];
    vsprintf_s(buffer, 1024, format, va);
    Log::Game << buffer << Log::Endl;
    return 0;
}

void ForwardGameLogsModule::OnLoad(bool manual) {
    RegisterHook("55 8B EC 83 EC ? 0F B6 05 ? ? ? ? 85 C0 75 ? E9 ? ? ? ? 8D 4D", HOOK_REF(debug_log));
    writeToLog = Config::Get()[id]["writeToLog"].as<bool>(false);
}

void ForwardGameLogsModule::RenderModuleUI() {
    if (ImGui::TreeNode("Настройки")) {
        if (ImGui::Checkbox("Перенаправить в лог мода", &writeToLog)) {
            const Config cfg;
            cfg.data[id]["writeToLog"] = writeToLog;
        }
        ImGui::TreePop();
    }
}