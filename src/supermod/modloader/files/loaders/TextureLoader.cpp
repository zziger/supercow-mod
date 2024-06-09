#include <supermod/modloader/files/loaders/TextureLoader.hpp>

#include <supermod/game/AssetPool.hpp>
#include <supermod/sdk/DirectX.hpp>

namespace modloader
{
bool TextureLoader::Load(const std::filesystem::path& path)
{
    return game::AssetPool::Instance()->ReloadGameAsset(path.filename().string());
}
} // namespace modloader