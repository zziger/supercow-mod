/// Some game textures have hard-coded dimensions. Those have some problems and weirdnesses.
/// First - created DirectX texture size is bigger than the image itself (actual texture size is nearest bigger power of 2 for each dimension)
/// Second - internal algorithm supports only up to 1023 pixels in size for one dimension
/// Even if texture dimensions are just overridden with the new real ones, if the aspect ratio is different - UVs won't line up at render
///
/// This patch recalculates image's "bigger" texture size in the way so UVs will line up, and extends limit up to 65536
/// It also adds some meta data to the game::Asset instance, so it's possible to swap the texture correctly on runtime

#include "Utils.h"
#include "events/EventManager.h"
#include "memory/HookManager.h"
#include <queue>

#include "DirectXUtils.h"
#include "events/ResolveFileEvent.h"
#include "game/AssetPool.h"

static int squarify_dimensions_algorithm(int a1) {
    int v3 = 1;
    for (auto i = 2; i < 18; i++) { // 12 in original fn
        v3 *= 2;
        if (a1 <= v3) return v3;
    }
    return 0;
}

static std::queue<int> overriden_dimensions_queue {};

HOOK_FN_CONV(int, squarify_dimensions, ARGS(int a1), __cdecl) {
    if (!overriden_dimensions_queue.empty()) {
        const auto res = overriden_dimensions_queue.front();
        overriden_dimensions_queue.pop();
        return res;
    }

    return squarify_dimensions_algorithm(a1);
}

void override_squared_dimensions(char* name, int& width, int& height) {
    const auto curPath = std::filesystem::current_path();
    const auto path = curPath / name;
    
    ResolveFileEvent evt { path };
    EventManager::Emit(evt);
    const auto resolved = evt.GetResolvedPath();
    const auto final = resolved ? *resolved : path;

    int realWidth = 0, realHeight = 0;
    dx_utils::get_image_dimensions(final.generic_string(), realWidth, realHeight);
    overriden_dimensions_queue.push(squarify_dimensions_algorithm(height) / (float) height * realHeight);
    overriden_dimensions_queue.push(squarify_dimensions_algorithm(width) / (float) width * realWidth);

    width = realWidth;
    height = realHeight;
}

void clear_override() {
    if (!overriden_dimensions_queue.empty()) std::queue<int>().swap(overriden_dimensions_queue);
}

game::Asset* (__thiscall * read_const_jpg_orig)(game::AssetPool* this_, char* name, int width, int height) = nullptr;
game::Asset* __fastcall read_const_jpg(game::AssetPool* this_, void*, char* name, int width, int height) {
    if (const auto existing = this_->GetByName(name)) return existing;

    int newWidth = width, newHeight = height;
    override_squared_dimensions(name, newWidth, newHeight);
    const auto asset = read_const_jpg_orig(this_, name, newWidth, newHeight);
    clear_override();
    
    asset->constTex = 1;
    asset->origWidth = width;
    asset->origHeight = height;
    return asset;
}

game::Asset* (__thiscall * read_const_surface_orig)(game::AssetPool* this_, char* name, int width, int height) = nullptr;
game::Asset* __fastcall read_const_surface(game::AssetPool* this_, void*, char* name, int width, int height) {
    if (const auto existing = this_->GetByName(name)) return existing;

    int newWidth = width, newHeight = height;
    override_squared_dimensions(name, newWidth, newHeight);
    const auto asset = read_const_surface_orig(this_, name, newWidth, newHeight);
    clear_override();
    
    asset->constTex = 2;
    asset->origWidth = width;
    asset->origHeight = height;
    return asset;
}

inline EventManager::Ready $const_size_texture_path([] {
    HookManager::RegisterHook(game::AssetPool::GetReadConstJpgMem(), HOOK_REF_FORCE(read_const_jpg));
    HookManager::RegisterHook(game::AssetPool::GetReadConstSurfaceMem(), HOOK_REF_FORCE(read_const_surface));
    HookManager::RegisterHook("55 8B EC 83 EC ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? EB ? 8B 45 ? 83 C0 ? 89 45 ? 83 7D ? ? 7D", HOOK_REF(squarify_dimensions));
});

