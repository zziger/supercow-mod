#include "Config.h"

#include <filesystem>

void Config::Init() {
    _cfgPath = std::filesystem::current_path() / "modcfg.yml";
    if (exists(_cfgPath)) _cfg = YAML::LoadFile(_cfgPath.string());
}

void Config::Save() {
    std::ofstream fstream(_cfgPath);
    fstream << _cfg;
}