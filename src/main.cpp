#include <iostream>
#include <locale>
#include <Windows.h>
#include <shellapi.h>
#include <thread>

#include "Utils.h"
#include "main.h"

#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "Config.h"
#include "Console.h"
#include "CrashHandler.h"
#include "ui/UI.h"
#include "Log.h"
#include "events/EventManager.h"
#include "events/ResolveFileEvent.h"
#include "events/GameLoadedEvent.h"
#include "memory/HookManager.h"
#include "memory/Memory.h"
#include "modloader/mods/files/ModFileResolver.h"
#include "modloader/mods/ModManager.h"
#include "sdk/Game.h"

void init_memory() {
    const auto base = (uintptr_t) GetModuleHandle(nullptr);
    
    Memory::Base() = base;
    Memory::OnPatternNotFound([](const std::string& pattern) {
        Log::Error << "Failed to find pattern " << pattern << Log::Endl;
    });
    Memory::InitCacheStorage(new MemoryCacheStorage());
}

void init() {
    auto cwd = std::filesystem::current_path();
    Config::Init(); 
    Console::Initialize();
    Log::Info << "Загрузка SuperMod " << VERSION << " by zziger..." << Log::Endl;

    MH_Initialize();
    sdk::Game::Init();
    Memory::RunHooks();

    ModManager::Init();
    ModManager::LoadMods();
    ModFileResolver::Init();
    EventManager::Emit(ReadyEvent());
    
    EventManager::On<GameLoadedEvent>([] {
        Log::Info << "Game startup!" << Log::Endl;
        
        ModManager::ReloadIcons();
        DragAcceptFiles(*sdk::Game::window, true);
    });

    
    EventManager::On<WindowEvent>([](auto ev) {
        if (ev.msg == WM_DROPFILES) {
            auto drop = (HDROP) (ev.wParam);
            
            uint32_t nCntFiles= DragQueryFileA( drop, -1, 0,0 );
            Log::Debug << nCntFiles << Log::Endl;
                      
            for (int j=0; j<nCntFiles; j++ ) {
                char szBuf[MAX_PATH];
                ::DragQueryFileA( drop, j, szBuf, sizeof(szBuf) );
                Log::Debug << szBuf << Log::Endl;
            }

            DragFinish(drop);
        }
    });
}

BOOL APIENTRY main(HMODULE, const DWORD ulReasonForCall, LPVOID) {
    if (ulReasonForCall == DLL_PROCESS_ATTACH) {
        init_memory();
        init_crash_handler();
        init();

        Log::Info << "Mod initialized!" << Log::Endl;
    }

    if (ulReasonForCall == DLL_PROCESS_DETACH) {
        Console::Disable();
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
    
    return TRUE;
}