#pragma once
#include <filesystem>
#include <list>
#include "modloader/mods/Mod.h"

class ModManager {
    inline static const std::filesystem::path manifest_file_name = "manifest.yml";
    inline static const std::filesystem::path icon_file_name = "icon.png";
    inline static const std::filesystem::path dll_file_name = "main.dll";
    
    inline static std::filesystem::path _mods_folder;
    inline static std::list<std::shared_ptr<Mod>> _loadedMods {};
    inline static std::recursive_mutex _modMutex {};
    
    inline static std::shared_ptr<Mod> _internalMod {};

    static std::shared_ptr<Mod> GetInternalMod();

public:

    static void Init();

    static std::filesystem::path GetModsRoot();

    static void LoadMod(std::string_view modName);

    static void LoadMods();

    static void ReloadIcons();

    static void DeleteMod(std::shared_ptr<Mod> mod);

    static std::list<std::shared_ptr<Mod>> GetLoadedMods();
};
