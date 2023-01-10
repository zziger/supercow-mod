#pragma once
#include "modloader/mods/Module.h"

class LevelBackRenderFixModule final : public Module {
public:
    LevelBackRenderFixModule() :
        Module(
            "levelBackRenderFix",
            "Фикс фона уровней",
            "Исправляет случайное пропадание фона на уровне.\nНе отревершен до конца, привести к каким-то проблемам", false) {
    }

    void OnLoad(bool manual) override;
};
