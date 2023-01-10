#include "modloader/mods/ModInfo.h"

#include <regex>
#include <yaml-cpp/yaml.h>

#include "DirectXUtils.h"
#include "Log.h"
#include "exceptions/Error.h"
#include "sdk/DirectX.h"
#include "sdk/Game.h"

void ModInfo::ReadManifest() {
    const auto filePath = basePath / "manifest.yml";
    const auto strPath = filePath.generic_string();
        
    if (!exists(filePath)) throw Error("Не удалось найти файл " + strPath);

    auto node = YAML::LoadFile(strPath);
    if (!node.IsMap()) throw Error("Неверный формат файла " + strPath);
    if (!node["id"]) throw Error("Не удалось найти id в файле " + strPath);
        
    id = node["id"].as<std::string>();
    const std::regex idRegex("^[a-zA-Z0-9_-]{2,32}$");

    if (!std::regex_match(id, idRegex)) {
        throw Error("Id в файле " + strPath + " должен состоять из символов английского алфавита, цифр, _ или - и быть длиной от 2 до 32 символов");
    }
    
    title = node["title"].as<std::string>(id);
    author = node["author"].as<std::string>("");
    version = node["version"].as<std::string>("");
    luaScript = node["lua-script"].as<std::string>("");

    const auto currentGameVersion = sdk::Game::GetGameVersion();
    const auto versions = node["game-versions"].as<std::vector<std::string>>(std::vector<std::string>{});

    if (!versions.empty()) {
        compatible = false;
        for (const auto& gameVersionString : versions) {
            auto gameVersion = sdk::Game::ParseGameVersion(gameVersionString);
            if (!gameVersion) {
                Log::Warn << "Неверная версия " << gameVersionString << " в моде " << title << Log::Endl;
                continue;
            }
            gameVersions.push_back(gameVersion);
            if (gameVersion == currentGameVersion) {
                compatible = true;
                break;
            }
        } 
    }

    if (!node["title"]) Log::Warn << "Не указано название для мода " << id << Log::Endl;
    if (!compatible) Log::Warn << "Мод " << title << " не совместим с текущей версией игры" << Log::Endl;
}

bool ModInfo::ReadIcon() {
    if (!*sdk::DirectX::d3dDevice) return false;
    if (!exists(basePath / "icon.png")) return false;
    icon = dx_utils::load_png(*sdk::DirectX::d3dDevice, (basePath / "icon.png").string().c_str());
    return true;
}
ModInfo::ModInfo(): id("invalid"), title("Invalid"), author("unknown"), version("invalid") {}


ModInfo::ModInfo(std::filesystem::path modPath, HMODULE module): basePath(std::move(modPath)), dll(module) {
    ReadManifest();
}

ModInfo::ModInfo(std::string id, std::string title, std::string author, std::string version)
    : id(std::move(id)), title(std::move(title)), author(std::move(author)), version(std::move(version)),
    basePath(""), internal(true) {
        
}