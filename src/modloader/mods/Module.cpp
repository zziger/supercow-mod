#include "modloader/mods/Module.h"

#include "Config.h"
#include "exceptions/Error.h"

static const char* config_key = "modules";

Module::Module(std::string id, std::string name, const char* desc, const bool defaultLoaded): fullId(id), id(std::move(id)), name(std::move(name)), desc(desc), defaultLoaded(defaultLoaded) {
}

void Module::Load(bool manual) {
    try {
        OnLoad(manual);
        _loaded = true;
        if (manual) {
            const Config cfg;
            if (!cfg.data[config_key].IsMap()) cfg.data[config_key] = YAML::Node(YAML::NodeType::Map);
            cfg.data[config_key][fullId] = true;
        }
        Log::Info << "Модуль " << fullId << " загружен" << Log::Endl;
    } catch(std::exception& e) {
        Log::Error << "Ошибка загрузки модуля " << fullId << ":" << Log::Endl;
        Log::Error << e.what() << Log::Endl;
    }
}

void Module::Unload(bool manual) {
    try {
        _loaded = false;
        if (manual) {
            const Config cfg; 
            if (!cfg.data[config_key].IsMap()) cfg.data[config_key] = YAML::Node(YAML::NodeType::Map);
            cfg.data[config_key][fullId] = false;
        }
        UnloadEvents();
        UnloadHooks();
        OnUnload(manual);
        Log::Info << "Модуль " << fullId << " выгружен" << Log::Endl;
    } catch(std::exception& e) {
        Log::Error << "Ошибка выгрузки модуля " << fullId << ":" << Log::Endl;
        Log::Error << e.what() << Log::Endl;
    }
}

bool Module::IsLoaded() const {
    return _loaded;
}

bool Module::ShouldBeLoaded() const {
    const auto& node = Config::Get()[config_key];
    if (node.IsMap() && node[fullId]) {
        return node[fullId].as<bool>(defaultLoaded);
    }
    return defaultLoaded;
}

void Module::SetOwner(ModInfo owner) {
    this->owner = owner;
    this->fullId = owner.internal ? id : owner.id + "/" + id;
}