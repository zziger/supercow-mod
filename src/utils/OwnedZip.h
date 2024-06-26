#pragma once
#include <filesystem>
#include <optional>
#include <zip_file.h>

class OwnedZip {
public:
    std::filesystem::path path;
    miniz_cpp::zip_file* zip;
    bool remove;

    OwnedZip(const std::filesystem::path& path, bool remove = false) : path(path), remove(remove)
    {
        zip = new miniz_cpp::zip_file(path.string());
    }

    ~OwnedZip()
    {
        delete zip;
        if (remove) std::filesystem::remove(path);
    }
};
