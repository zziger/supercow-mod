#pragma once
#include "modloader/mods/Module.h"

class ForwardGameLogsModule final : public Module {
public:
    ForwardGameLogsModule() :
        Module(
            "forwardGameLogs",
            "Перенаправление логов",
            "Позволяет отлключить или перенаправить логи игры.", false) {
    }

    void OnLoad(bool manual) override;

    static inline bool writeToLog = false;
    
    void RenderModuleUI() override;
};
