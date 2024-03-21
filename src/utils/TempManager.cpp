#include "TempManager.h"

#include <Log.h>
#include <sdk/Game.h>

std::filesystem::path TempManager::GetTempRoot()
{
    return sdk::Game::GetRootPath() / TEMP_DIR;
}

void TempManager::Init()
{
    rand.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    remove_all(GetTempRoot());
}

std::filesystem::path TempManager::GetTempDir(const bool create)
{
    auto randchar = []() -> char
    {
        constexpr char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::uniform_int_distribution<> d1(0, sizeof(charset) - 2);
        return charset[d1(rand)];
    };

    if (!exists(GetTempRoot())) create_directories(GetTempRoot());

    std::filesystem::path path;
    do
    {
        std::string str(TEMP_LENGTH, 0);
        std::generate_n(str.begin(), TEMP_LENGTH, randchar);
        path = GetTempRoot() / str;
    } while(exists(path));

    if (create) create_directories(path);
    return path;
}

void TempManager::RemoveTempDir(const std::filesystem::path& path)
{
    try
    {
        if (exists(path)) remove_all(path);
        Cleanup();
    }
    catch(const std::exception& err)
    {
        Log::Warn << "Failed to remove temp directory " << path.string() << ": " << err.what() << Log::Endl;
    }
}

void TempManager::Cleanup()
{
    const auto root = GetTempRoot();
    if (exists(root) && is_directory(root) && is_empty(root))
    {
        remove_all(root);
    }
}
