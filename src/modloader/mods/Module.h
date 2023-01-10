#pragma once
#include "modloader/mods/ModInfo.h"
#include "modloader/mixins/EventUser.h"
#include "modloader/mixins/HookUser.h"

class Module : public HookUser, public EventUser {
    bool _loaded = false;
    
protected:
    Module(std::string id, std::string name, const char* desc = nullptr, bool defaultLoaded = false);

    virtual void OnLoad(bool manual) {}
    virtual void OnUnload(bool manual) {}
    
public:
    std::string fullId;
    const std::string id;
    const std::string name;
    const char* desc;
    const bool defaultLoaded;

    virtual void RenderModuleUI() {}
    
    void Load(bool manual);
    void Unload(bool manual);
    bool IsLoaded() const;
    bool ShouldBeLoaded() const;

    ModInfo owner {};
    void SetOwner(ModInfo);
};
