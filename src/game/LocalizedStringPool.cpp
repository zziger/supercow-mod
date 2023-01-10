#include "LocalizedStringPool.h"

#include "memory/Memory.h"

namespace game
{
    LocalizedStringPool* LocalizedStringPool::GetInstance() {
        static constexpr Memory::Pattern pat("05 ? ? ? ? 89 45 ? 8B 0D ? ? ? ? 83 C1 ? 89 0D ? ? ? ? 8B 55 ? 52 8B 45"); // add eax, offset localizedStrings 
        static auto mem = pat.Search();
        return *mem.Get<LocalizedStringPool**>(1);    
    }
}