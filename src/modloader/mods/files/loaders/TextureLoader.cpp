#include <regex>
#include <queue>

#include "DirectXUtils.h"
#include "Utils.h"
#include "../ModFileResolver.h"
#include "events/ResolveFileEvent.h"
#include "game/AssetPool.h"
#include "game/CachedFilePool.h"
#include "sdk/DirectX.h"


void ModFileResolver::LoadTexture(std::filesystem::path texturePath) {
    // Log::Debug << "Reloading texture " << texturePath << Log::Endl;

    const auto filename = GetPoolFileName(texturePath.filename().generic_string());

    const auto pool = game::AssetPool::GetInstance();
    game::Asset* asset = pool->GetByName(texturePath.filename().generic_string());
    if (!asset) return;

    asset->Anonymize();
    const auto cwd = std::filesystem::current_path();
    current_path(texturePath.parent_path());

    game::CachedFilePool::DeleteByName(filename);
    game::Asset* newAsset;
    set_should_resolve_files(false);
    if (asset->constTex == 1) {
        newAsset = game::AssetPool::GetReadConstJpgMem().Get<game::Asset* (__thiscall *)(game::AssetPool*, const char*, int, int)>()(pool, filename.c_str(), asset->origWidth, asset->origHeight);
    } else if (asset->constTex == 2) {
        newAsset = game::AssetPool::GetReadConstSurfaceMem().Get<game::Asset* (__thiscall *)(game::AssetPool*, const char*, int, int)>()(pool, filename.c_str(), asset->origWidth, asset->origHeight);
    } else {
        static constexpr Memory::Pattern loadTex("55 8B EC 51 89 4D ? 6A ? 6A ? 6A");
        static auto mem = loadTex.Search();
        newAsset = mem.Get<game::Asset* (__thiscall *)(game::AssetPool*, const char*)>()(pool, filename.c_str());
    }
    set_should_resolve_files(true);
    // } else {
    // }
    current_path(cwd);
    asset->texture->Release();
    memcpy(asset, newAsset, sizeof(game::Asset));
    memset(newAsset, 0, sizeof(game::Asset));
    pool->assetCount--;

}