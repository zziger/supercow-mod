#include "MenuPool.h"

#include "memory/Memory.h"

namespace game
{
    MenuPool* MenuPool::GetInstance() {
        static constexpr Memory::Pattern pat("81 C1 ? ? ? ? 89 4D ? 8B 55 ? 52 8B 45 ? 50 E8"); //  add ecx, offset menuPool
        static auto mem = pat.Search();
        return *mem.Get<MenuPool**>(2);
    }
    
    Menu* MenuPool::GetByName(wchar_t* name) {

        for (auto i = 0; i < menusCount; i++) {
            if (wcscmp(name, menus[i].name) == 0) return &menus[i];
        }

        return nullptr;
    }
}