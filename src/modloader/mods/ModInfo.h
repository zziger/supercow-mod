﻿#pragma once
#include <filesystem>
#include <string>

#include <thirdparty/directx/d3d8.h>


struct ModInfo {
    std::string id;
    std::string title;
    std::string author;
    std::string version;
    std::filesystem::path basePath;
    LPDIRECT3DTEXTURE8 icon = nullptr; // nullptr if no icon
    HMODULE dll = nullptr;
    bool internal = false;

    void ReadManifest();
    bool ReadIcon();
    
    ModInfo();
    
    explicit ModInfo(std::filesystem::path modPath, HMODULE module);
    
    explicit ModInfo(std::string id, std::string title, std::string author, std::string version);
};
