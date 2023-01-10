#pragma once
#include <mutex>
#include <set>
#include <unordered_set>

#include "modloader/mods/Module.h"

struct ModuleContainer {
    std::set<std::shared_ptr<Module>> items {};

    std::shared_ptr<Module> operator += (std::shared_ptr<Module> module);

    std::shared_ptr<Module> operator -= (std::shared_ptr<Module> module);

    void LoadNeeded() const;
    void Unload();

    ModInfo owner;

    explicit ModuleContainer(ModInfo owner);

private:
    inline static std::recursive_mutex _mutex {};
};
