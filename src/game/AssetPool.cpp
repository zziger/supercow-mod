#include "AssetPool.h"

namespace game
{
    void Asset::Anonymize() const {
        const_cast<char*>(name)[0] = '\0';
    }

    void Asset::ReplaceTexture(IDirect3DTexture8* tex) {
        texture->Release();
        texture = tex;
    }

    Memory AssetPool::GetReadConstJpgMem() {
        static constexpr Memory::Pattern pat("55 8B EC 83 EC ? 89 4D ? 8B 45 ? 50 8B 4D ? E8 ? ? ? ? 89 45 ? 83 7D ? ? 74 ? 8B 45 ? E9 ? ? ? ? 6A ? 6A ? 8B 4D");
        static auto mem = pat.Search();
        return mem;
    }
    
    Memory AssetPool::GetReadConstSurfaceMem() {
        static constexpr Memory::Pattern pat("55 8B EC 83 EC ? 89 4D ? 8B 45 ? 50 8B 4D ? E8 ? ? ? ? 89 45 ? 83 7D ? ? 74 ? 8B 45 ? E9 ? ? ? ? C7 45");
        static auto mem = pat.Search();
        return mem;
    }
    
    Asset* AssetPool::GetByName(const std::string& name) const {
        for (auto i = 0; i < assetCount; i++) {
            if (assets[i]->name == name) return assets[i];
        }
            
        return nullptr;
    }

    AssetPool* AssetPool::GetInstance() {
        static constexpr Memory::Pattern pat("B9 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? A1 ? ? ? ? 8B 08"); //  mov ecx, offset AssetPool__instance
        static auto mem = pat.Search();
        return *mem.Get<AssetPool**>(1);
    }
}