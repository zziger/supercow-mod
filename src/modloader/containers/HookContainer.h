#pragma once
#include <mutex>
#include <unordered_set>

#include "memory/Memory.h"

struct HookContainer {
    std::unordered_set<Memory> items {};

    Memory operator += (Memory mem);
    
    Memory operator -= (Memory mem);

    void Unload();

private:
    inline static std::recursive_mutex _mutex {};
};
