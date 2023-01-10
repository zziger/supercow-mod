#include "modloader/mods/Mod.h"

#include "Config.h"
#include "modloader/mods/files/ModFileResolver.h"

std::shared_ptr<Module> Mod::AddModule(std::shared_ptr<Module> module) {
    modules += module;
    return module;
}

std::shared_ptr<Module> Mod::RemoveModule(std::shared_ptr<Module> module) {
    modules -= module;
    return module;
}

void Mod::OnLoad() {}

void Mod::OnUnload() {}

Mod::Mod(ModInfo info): modules(info), info(std::move(info)) {}

void Mod::RenderUI() {}

static const char* config_key = "disabledMods";

void Mod::Load(bool manual) {
    if (_loaded) return;
    try {
        OnLoad();
        modules.LoadNeeded();
        _loaded = true;
        if (manual && !info.internal) ModFileResolver::ReloadModFiles(info.basePath / "data");
        Log::Info << "Мод " << info.title << " загружен" << Log::Endl;
        const Config cfg;
        const auto& node = cfg.data[config_key];
        if (manual && node.IsSequence()) {
            std::vector<std::string> newList = {};
            for (const auto& value : node) {
                const auto& id = value.as<std::string>("");
                if (id != info.id) newList.push_back(id);
            }
            cfg.data[config_key] = newList;
        }
    } catch(std::exception& e) {
        Log::Error << "Ошибка загрузки мода " << info.id << ":" << Log::Endl;
        Log::Error << e.what() << Log::Endl;
    }
}

void Mod::Unload(bool manual) {
    if (!_loaded) return;
    _loaded = false;
    try {
        if (manual) {
            const Config cfg;
            if (!cfg.data[config_key].IsSequence()) cfg.data[config_key] = YAML::Node(YAML::NodeType::Sequence);
            cfg.data[config_key].push_back(info.id);
        }
        UnloadEvents();
        UnloadHooks();
        OnUnload();
        modules.Unload();
        if (manual && !info.internal) ModFileResolver::ReloadModFiles(info.basePath / "data");
        Log::Info << "Мод " << info.title << " выгружен" << Log::Endl;
    } catch(std::exception& e) {
        Log::Error << "Ошибка выгрузки мода " << info.id << ":" << Log::Endl;
        Log::Error << e.what() << Log::Endl;
    }
}

void Mod::UnloadModule() {
    if (!info.dll) return;
    if (!FreeLibrary(info.dll)) MessageBoxA(nullptr, "Не удалось выгрузить модуль", "SuperMod", MB_OK);
    info.dll = nullptr;
}


bool Mod::IsLoaded() const {
    return _loaded;
}

bool Mod::ShouldBeLoaded() const {
    const auto& node = Config::Get()[config_key];
    if (node.IsSequence()) {
        for (auto mod : node) {
            if (mod.as<std::string>("") == info.id) return false;
        }
    }

    return true;
}
