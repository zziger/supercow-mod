#pragma once
#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

class Config {
    inline static std::filesystem::path _cfgPath {};
    inline static YAML::Node _cfg { YAML::NodeType::Map };
    inline static std::recursive_mutex _mutex {};
    
public:
    static void Init();

    static YAML::Node& Get() {
        return _cfg;
    }

    static void Save();

    YAML::Node& data;

    // todo optional lock somehow

    Config() : data(_cfg) {
    } 

    ~Config() {
        Save();
    }
};
