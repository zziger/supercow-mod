#include "modloader/containers/HookContainer.h"

#include "memory/HookManager.h"

Memory HookContainer::operator+=(const Memory mem) {
    std::lock_guard lock(_mutex);
    items.emplace(mem);
    return mem;
}

Memory HookContainer::operator-=(const Memory mem) {
    std::lock_guard lock(_mutex);
    items.erase(mem);
    return mem;
}

void HookContainer::Unload() {
    std::lock_guard lock(_mutex);
    for (const auto& hook : items) {
        HookManager::UnregisterHook(hook);
    }
    items.clear();
}