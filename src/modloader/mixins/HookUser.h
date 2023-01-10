#pragma once
#include "memory/HookManager.h"
#include "modloader/containers/HookContainer.h"

class HookUser {
    HookContainer _hooks {};
    
protected:
    template<class T, std::size_t Size>
    Memory RegisterHook(const char (&pattern)[Size], T* fn, T** orig) {
        const Memory mem = HookManager::RegisterHook(pattern, fn, orig);
        return _hooks += mem;
    }

    Memory UnregisterHook(Memory mem);

public:
    virtual ~HookUser() = default;
    void UnloadHooks();
};
