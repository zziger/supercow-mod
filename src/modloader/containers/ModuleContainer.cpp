#include "modloader/containers/ModuleContainer.h"

std::shared_ptr<Module> ModuleContainer::operator+=(std::shared_ptr<Module> module) {
    std::lock_guard lock(_mutex);
    items.emplace(module);
    module->SetOwner(owner);
    return module;
}

std::shared_ptr<Module> ModuleContainer::operator-=(std::shared_ptr<Module> module) {
    std::lock_guard lock(_mutex);
    items.erase(module);
    return module;
}

void ModuleContainer::LoadNeeded() const {
    std::lock_guard lock(_mutex);
    
    for (const auto& module : items) {
        if (module->ShouldBeLoaded()) module->Load(false);
    }
}

void ModuleContainer::Unload() {
    std::lock_guard lock(_mutex);
        
    for (const auto& module : items) {
        if (module->IsLoaded()) module->Unload(false);
    }

    items.clear();
}

ModuleContainer::ModuleContainer(ModInfo owner): owner(owner) {
}