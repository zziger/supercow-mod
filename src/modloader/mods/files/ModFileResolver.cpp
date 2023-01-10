#include "ModFileResolver.h"

#include <regex>

#include "DirectXUtils.h"
#include "../ModManager.h"
#include "events/EventManager.h"
#include "events/ResolveFileEvent.h"
#include "events/TickEvent.h"
#include "game/AssetPool.h"
#include "sdk/DirectX.h"

std::optional<std::filesystem::path> ModFileResolver::ResolveFile(const std::filesystem::path& path) {
    auto mods = ModManager::GetLoadedMods();
    for (auto el = mods.rbegin(); el != mods.rend(); ++el) {
        const auto resolvedPath = ResolveModFile(*el, path);
        if (!resolvedPath) continue;

        auto rel = relative(*resolvedPath, sdk::Game::GetDataPath() / "..");
        // Log::Debug << "Resolved file " << *resolvedPath << Log::Endl;
        return *resolvedPath;
    }

    return std::nullopt;
}

std::filesystem::path ModFileResolver::ResolveFileOrOriginal(const std::filesystem::path& path) {
    const auto resolvedPath = ResolveFile(path);
    return resolvedPath ? *resolvedPath : path;
}

std::optional<std::filesystem::path> ModFileResolver::ResolveModFile(const std::shared_ptr<Mod>& mod, const std::filesystem::path& path) {
    if (!mod || mod->info.internal || !mod->IsLoaded()) return std::nullopt;
    
    const auto filePath = relative(path, sdk::Game::GetDataPath());

    const auto modFilePath = mod->info.basePath / "data" / filePath;
        
    if (exists(modFilePath)) return modFilePath;
    return std::nullopt;
}


void ModFileResolver::Init() {
    if (_initialized) return;
    EventManager::On<ResolveFileEvent>([](auto& evt) {
        auto path = ResolveFile(evt.absolutePath);
        if (path) evt.SetResolvedPath(*path);
    });
    EventManager::On<AfterTickEvent>([]() {
        std::lock_guard lock(_reloadMutex);
        for (auto toReload : filesToReload) {
            auto path = sdk::Game::GetDataPath() / ".." / toReload;
            LoadFile(path);
        }
        filesToReload.clear();
    });
    ToggleFileListener(true);
    _initialized = true;
}

void ModFileResolver::ToggleFileListener(bool state) {
    if (listenerEnabled == state) return;
    if (!state) {
        listenerEnabled = false;
        return;
    }
    
    listenerEnabled = true;
    CreateThread(nullptr, 0, [](LPVOID _) -> DWORD {
        FileListenerThread();
        return 0;
    }, nullptr, 0, nullptr);
}

std::string ModFileResolver::GetPoolFileName(const std::string& filename) {
    static auto tgaRegex = std::regex("^_a_(.*?)\\.jpg$");
    static auto excludeRegex = std::regex("^_a_back\\d+\\.jpg$");

    if (!filename.starts_with("_a_")) return filename;
    if (std::regex_match(filename, excludeRegex)) return filename;
    return std::regex_replace(filename, tgaRegex, "$1.tga");
}

void ModFileResolver::LoadFile(const std::filesystem::path filepath) {
    auto extension = filepath.extension().generic_string();
    std::ranges::transform(extension, extension.begin(), tolower);

    if (extension == ".jpg" || extension == ".png" || extension == ".tga") LoadTexture(ResolveFileOrOriginal(filepath));
    else Log::Warn << "Загрузка файлов " << extension << " не поддерживается" << Log::Endl;
}

void ModFileResolver::ReloadModFiles(std::filesystem::path dataFolder) {
    if (!exists(dataFolder)) return;
    
    const auto it = std::filesystem::recursive_directory_iterator(dataFolder);
    for (auto& file : it) {
        if (file.is_directory()) continue;
        auto relPath = relative(file.path(), dataFolder);
        try {
            LoadFile(sdk::Game::GetDataPath() / relPath);
        } catch(std::exception& e) {
            Log::Warn << "Не удалось загрузить файл " << relPath.generic_string() << ": " << e.what() << Log::Endl;
        }
    }
}
