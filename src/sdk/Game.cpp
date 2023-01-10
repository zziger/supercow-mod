#include "Game.h"

#include <filesystem>

#include "DirectX.h"
#include "Log.h"
#include "Utils.h"
#include "game/AssetPool.h"
#include "memory/HookManager.h"
#include "memory/Memory.h"

namespace sdk
{
    void Game::Init() {
        constexpr Memory::Pattern movWindow("A3 ? ? ? ? 83 3D ? ? ? ? ? 74 ? 68 ? ? ? ? E8 ? ? ? ?");
        window = *movWindow.Search().Get<HWND**>(1);

        sdk::DirectX::Init();
    }
    
    void Game::Restart() {
        char buf[MAX_PATH];
        GetModuleFileNameA(nullptr, buf, MAX_PATH);
        std::filesystem::current_path(GetDataPath() / "..");
        
        STARTUPINFOA si(0);
        si.cb = sizeof(STARTUPINFOA);
        PROCESS_INFORMATION	pi(nullptr);
        
        CreateProcessA(nullptr, buf, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi);
        ExitProcess(0);
    }
    
    bool Game::IsGameLoaded() {
        static constexpr Memory::Pattern gameLoadFinishedPat("C6 05 ? ? ? ? ? E8 ? ? ? ? 68"); // mov gameLoadFinished, 1
        const auto mem = gameLoadFinishedPat.Search();
        const auto ptr = *mem.Get<bool**>(2);
        if (ptr == nullptr) return false;
        return ptr;
    }

    std::filesystem::path Game::GetRootPath() {
        if (_rootPath) return *_rootPath;

        const auto hModule = GetModuleHandleA(nullptr);
        if (hModule != nullptr)
        {
            char path[MAX_PATH];
            GetModuleFileNameA(hModule, path, sizeof path);
            auto rootPath = std::filesystem::path(path).parent_path(); 
            _rootPath = rootPath;
            return rootPath;
        }
        
        return std::filesystem::path {};
    }

    std::filesystem::path Game::GetDataPath() {
        return GetRootPath() / "data";
    }

    std::filesystem::path Game::GetModsPath() {
        return GetRootPath() / "mods";
    }
    
    uint64_t Game::GetGameVersion() {
        const auto dosHeader = (IMAGE_DOS_HEADER*) GetModuleHandleA(nullptr);
        const auto ntHeaders = (IMAGE_NT_HEADERS*) ((byte*)dosHeader + dosHeader->e_lfanew);
        return ntHeaders->FileHeader.TimeDateStamp;
    }
    
    std::string Game::SerializeGameVersion(uint64_t version) {
        auto timestamp = (std::chrono::sys_seconds) (std::chrono::seconds) version;
        return std::format("{:%d.%m.%Y %T}", timestamp);
    }
    
    uint64_t Game::ParseGameVersion(const std::string& version) {
        std::chrono::sys_seconds timestamp;
        std::istringstream in(version);
        in >> parse("%d.%m.%Y %T", timestamp);
        return timestamp.time_since_epoch().count();
    }

    void Game::AddToLua(LuaContext& context) {
        context.writeVariable("Game", Game{});
        context.registerStaticFunction<Game>("restart", Restart);
        context.registerStaticFunction<Game>("isGameLoaded", IsGameLoaded);
        context.registerStaticFunction<Game>("getDataPath", GetDataPath);
        context.registerStaticFunction<Game>("getModsPath", GetModsPath);
        context.registerStaticFunction<Game>("getGameVersion", GetGameVersion);
        context.registerStaticFunction<Game>("serializeGameVersion", SerializeGameVersion);
        context.registerStaticFunction<Game>("parseGameVersion", ParseGameVersion);
    }
}