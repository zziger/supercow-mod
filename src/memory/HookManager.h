#pragma once
#include <string>

#include "Memory.h"

class HookManager {
public:
    template <class T, std::size_t Size>
    static Memory RegisterHook(const char(& pattern)[Size], T* fn, T** orig) {
        static Memory::Pattern pat(pattern); // todo constexpr
        static auto mem = pat.Search();
        mem.Detour(fn, orig);
        return mem;
    }
    
    template <class T>
    static Memory RegisterHook(Memory mem, T* fn, T** orig) {
        mem.Detour(fn, orig);
        return mem;
    }

    static Memory UnregisterHook(Memory mem);
};