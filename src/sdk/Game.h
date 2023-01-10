#pragma once
#include <fstream>
#include <optional>

#include "thirdparty/LuaContext.h"
#include "thirdparty/directx/d3d8.h"

namespace sdk
{
    class Game {
    public:
    
        static inline HWND* window = nullptr;

        static void Init();

        static void Restart();

        static bool IsGameLoaded();

        static std::filesystem::path GetRootPath();
        static std::filesystem::path GetDataPath();
        static std::filesystem::path GetModsPath();

        static uint64_t GetGameVersion();

        static std::string SerializeGameVersion(uint64_t version);
        static uint64_t ParseGameVersion(const std::string& version);

        static void AddToLua(LuaContext& context);
    private:
        inline static std::optional<std::filesystem::path> _rootPath;
    };
}