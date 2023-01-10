#pragma once
#include <filesystem>
#include <string>

namespace game
{
    struct CachedFile {
        char filename[128];
        int size;
        void* content;
        int loaded;

        void ReplaceContent(const std::string& filepath);
    };

    class CachedFilePool {
    public:
        CachedFile* files[1024];
        int filesCount;

        static CachedFilePool* GetInstance();

        [[nodiscard]] static CachedFile* GetByName(const std::string& name);
        static bool DeleteByName(const std::string& name);
    };
}