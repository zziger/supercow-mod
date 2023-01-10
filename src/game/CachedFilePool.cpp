#include "CachedFilePool.h"

#include "Utils.h"
#include "memory/Memory.h"

namespace game
{
    void CachedFile::ReplaceContent(const std::string& filepath) {
        int newSize;
        const auto newContent = utils::read_file(filepath, newSize);
        if (loaded && content) free(content);
        content = newContent;
        size = newSize;
    }
    
    CachedFilePool* CachedFilePool::GetInstance() {
        static constexpr Memory::Pattern pat("8B 0C 85 ? ? ? ? 51 E8 ? ? ? ? 83 C4 ? 85 C0 75"); //  mov ecx, cachedFiles[eax*4]
        static auto mem = pat.Search();
        return *mem.Get<CachedFilePool**>(3);
    }
    
    CachedFile* CachedFilePool::GetByName(const std::string& name) {
        static constexpr Memory::Pattern pat("55 8B EC 83 EC ? A1 ? ? ? ? 89 45 ? 8B 45 ? 50 8D 4D");
        static auto mem = pat.Search();
        return mem.Call<CachedFile*>(name.c_str());
    }
    
    bool CachedFilePool::DeleteByName(const std::string& name) {
        const auto found = GetByName(name);
        if (!found) return false;
        
        static constexpr Memory::Pattern pat("55 8B EC 83 EC ? C7 45 ? ? ? ? ? EB ? 8B 45 ? 83 C0 ? 89 45 ? 8B 4D ? 3B 0D ? ? ? ? 0F 8D ? ? ? ? 8B 55 ? 8B 04 95");
        static auto mem = pat.Search();
        return mem.Call<char>(found->content) == 1;
    }
}