#pragma once
#include <filesystem>

#include "ModInfo.h"
#include "modloader/containers/ModuleContainer.h"
#include "modloader/mixins/EventUser.h"
#include "modloader/mixins/HookUser.h"
#include "modloader/mods/Module.h"

template <class ModuleType>
concept simple_module = std::derived_from<ModuleType, Module> && std::constructible_from<ModuleType> && requires { std::make_shared<ModuleType>(); };

class Mod : public HookUser, public EventUser {
    bool _loaded = false;
    
protected:
    std::shared_ptr<Module> AddModule(std::shared_ptr<Module> module);
    std::shared_ptr<Module> RemoveModule(std::shared_ptr<Module> module);

    template<simple_module... Modules> 
    void AddModules() {
        (AddModule(std::make_shared<Modules>()), ...);
    }
    
    virtual void OnLoad();
    virtual void OnUnload();

public:
    explicit Mod(ModInfo info);

    ModuleContainer modules;
    ModInfo info;
    
    void Load(bool manual);
    void Unload(bool manual);
    void UnloadModule();
    bool IsLoaded() const;
    bool ShouldBeLoaded() const;
    
    virtual void RenderUI();
};

typedef std::shared_ptr<Mod> (*create_mod_fn)(ModInfo& info);