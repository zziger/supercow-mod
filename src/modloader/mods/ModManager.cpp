#include "modloader/mods/ModManager.h"

#include "DirectXUtils.h"
#include "exceptions/Error.h"
#include "files/ModFileResolver.h"
#include "mod/InternalMod.h"
#include "scripting/LuaMod.h"
#include "sdk/DirectX.h"
#include "sdk/Game.h"

std::shared_ptr<Mod> ModManager::GetInternalMod() {
    return _internalMod;
}

void ModManager::Init() {
    _mods_folder = std::filesystem::current_path() / "mods";
        
    const auto ptr = std::make_shared<InternalMod>();
    ptr->Load(false);
    _loadedMods.push_back(ptr);
}

std::filesystem::path ModManager::GetModsRoot() {
    return _mods_folder;
}

void ModManager::LoadMod(const std::string_view modName) {
    std::lock_guard lock(_modMutex);
        
    const std::filesystem::path modBase = _mods_folder / modName;
    if (!exists(modBase / manifest_file_name)) {
        throw Error("Не удалось найти папку " + (modBase / manifest_file_name).generic_string());
    }

    create_mod_fn createMod = nullptr;
    HMODULE module = nullptr;
        
    if (exists(modBase / dll_file_name)) {
        module = LoadLibraryExA((modBase / dll_file_name).string().c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (module == nullptr) {
            throw Error("Не удалось загрузить файл " + (modBase / dll_file_name).string());
        }

        createMod = (create_mod_fn) (void*) GetProcAddress(module, "create_mod");
    }

    auto info = ModInfo(modBase, module);
    info.ReadIcon();
    
    const auto mod = createMod ? createMod(info) : info.luaScript == "" ? std::make_shared<Mod>(info) : std::make_shared<LuaMod>(info);

    if (mod->ShouldBeLoaded()) mod->Load(false);
        
    _loadedMods.push_back(mod);
        
}

void ModManager::LoadMods() {
    std::lock_guard lock(_modMutex);
        
    const auto it = std::filesystem::directory_iterator(_mods_folder);
    for (auto& file : it) {
        if (!file.is_directory()) continue;
        const auto modName = file.path().filename().generic_string();
        try {
            LoadMod(modName);
        } catch(std::exception& e) {
            Log::Error << "Ошибка загрузки мода " << modName << ":" << Log::Endl;
            Log::Error << e.what() << Log::Endl;
        }
    }
}

void ModManager::ReloadIcons() {
    if (!*sdk::DirectX::d3dDevice) return;
    std::lock_guard lock(_modMutex);

    for (const auto loadedMod : _loadedMods) {
        try {
            loadedMod->info.ReadIcon();
        } catch(std::exception&  e) {
            Log::Warn << "Ошибка загрузки иконки мода " << loadedMod->info.id << ":" << Log::Endl;
            Log::Warn << e.what() << Log::Endl;
        }
    }

}

void ModManager::DeleteMod(std::shared_ptr<Mod> mod) {
    std::lock_guard lock(_modMutex);

    _loadedMods.remove(mod);
    mod->Unload(false);
    mod->UnloadModule();
    remove_all(mod->info.basePath);
}

std::list<std::shared_ptr<Mod>> ModManager::GetLoadedMods() {
    std::lock_guard lock(_modMutex);

    return _loadedMods;
}