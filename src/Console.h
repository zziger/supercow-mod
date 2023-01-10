#pragma once
#include <regex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "Config.h"
#include "data.h"
#include "Log.h"
#include "Utils.h"

class Console {
public:
    
    static void Initialize() {
        if (Config::Get()["console"].as<bool>(false)) Enable();
    }

    inline static bool enabled = false;
    inline static FILE* stdinFile = nullptr;
    inline static FILE* stdoutFile = nullptr;

    static void Enable() {
        if (enabled) return;
        enabled = true;
        AllocConsole();
        freopen_s(&stdinFile, "CONIN$", "r", stdin);
        freopen_s(&stdoutFile, "CONOUT$", "w", stdout);
        SetConsoleTitle(L"SuperCow mod console");
        SetConsoleOutputCP( CP_UTF8);
        SetConsoleCP( CP_UTF8);
    }

    static void Disable() {
        if (!enabled) return;
        enabled = false;
        if (stdinFile != nullptr) fclose(stdinFile);
        if (stdoutFile != nullptr) fclose(stdoutFile);
        FreeConsole();
        auto wnd = GetConsoleWindow();
        if (wnd != nullptr) CloseWindow(wnd);
    }
};
