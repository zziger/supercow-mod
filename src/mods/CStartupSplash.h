#pragma once
#include "CConfig.h"
#include "CGameApis.h"
#include "Utils.h"
#include "module/IMod.h"
#include "module/IPatch.h"

class CStartupSplash final : public IMod {
public:
    void Load() override {
        IMod::Load();
        CGameApis::ShowLowerMessage(U16(GetMsg("Ваш текст")));
    }

    static std::string GetMsg(std::string def) {
        auto cfg = CConfig::Instance().cfg;
        if (!cfg.contains("startupSplashMessage")) return def;
        return cfg["startupSplashMessage"].get<std::string>();
    }

    std::string GetName() override {
        return "STARTUP_SPLASH";
    }

    bool HandleCommand(Command command) override {
        if (command.cmd == "setStartupSplash") {
            CConfig::Instance().cfg["startupSplashMessage"] = command.rawArgs;
            CConfig::Instance().Save();
            CGameApis::ShowLowerMessage(U16(command.rawArgs));
            return true;
        }

        return false;
    }
    
    void LogDescription(const bool loaded) override {
        if (!loaded) return;
        Log::Instance() << Log::Color::GRAY << "    Текущий текст сплеша: " << GetMsg("не установлен") << Log::Endl;
        Log::Instance() << Log::Color::GRAY << "    Доступные команды:" << Log::Endl;
        Log::Instance() << Log::Color::GRAY << "    - setStartupSplash\tУстанавливает текст сплеша" << Log::Endl;
    }
};