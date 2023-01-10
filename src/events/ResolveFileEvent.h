#pragma once
#include <filesystem>

#include "EventManager.h"
#include "Utils.h"
#include "memory/HookManager.h"
#include "memory/Memory.h"
#include "sdk/Game.h"

struct ResolveFileEvent final : IEvent<"resolveFile"> {
    const std::filesystem::path absolutePath;

    explicit ResolveFileEvent(std::filesystem::path path) : absolutePath(std::move(path)) {
    }

    void SetResolvedPath(const std::filesystem::path& path) {
        _resolvedPath = path;
    }

    void ClearResolvedPath() {
        _resolvedPath = std::nullopt;
    }

    std::optional<std::filesystem::path>& GetResolvedPath() {
        return _resolvedPath;
    }

private: 
    std::optional<std::filesystem::path> _resolvedPath = std::nullopt;
};

static inline bool should_resolve_files = true;

template <typename T>
T resolve(std::string filename, std::function<T (const std::string&)> fn) {
    if (!should_resolve_files) return fn(filename);
    
    const auto basePath = sdk::Game::GetDataPath() / "..";
    const auto curPath = std::filesystem::current_path();
    const auto path = curPath / filename;
    
    ResolveFileEvent evt { path };
    EventManager::Emit(evt);

    if (evt.GetResolvedPath()) {
        const auto resolvedPath = *evt.GetResolvedPath();
        const auto workingDir = resolvedPath.parent_path();
        current_path(workingDir);
        filename = resolvedPath.filename().generic_string();
        
        Log::Debug << "Файл " << relative(path, basePath).generic_string() << " загружен из " << relative(resolvedPath, basePath).generic_string() << Log::Endl;
    }
    const auto result = fn(filename);
    if (evt.GetResolvedPath()) current_path(curPath);
    
    return result;
}

HOOK_FN(inline void*, resolve_file, ARGS(const char* lpFileName, size_t* outBuf, char isCritical)) {
    return resolve<void*>(std::string(lpFileName), [&](auto& filename) {
        return resolve_file_orig(filename.c_str(), outBuf, isCritical);
    });
}

HOOK_FN(inline void, load_audio, ARGS(const char* lpFileName)) {
    resolve<void*>(std::string(lpFileName), [](auto& filename) {
        load_audio_orig(filename.c_str());
        return nullptr;
    });
}

inline void set_should_resolve_files(bool state) {
    should_resolve_files = state;
}

inline EventManager::Ready $resolve_file_event_hook([] {
    HookManager::RegisterHook("55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 89 45 EC 8B 45 08", HOOK_REF(resolve_file));
    HookManager::RegisterHook("55 8B EC 81 3D ? ? ? ? ? ? ? ? 7C ? E9 ? ? ? ? 8B 45", HOOK_REF(load_audio));
});