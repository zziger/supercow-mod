﻿#include "ModImplInternal.h"

#include <UpdateManager.h>
#include <events/D3dInitEvent.h>
#include <game/textures/PngLoader.h>
#include <modloader/mod/Mod.h>
#include <ui/NotificationManager.h>
#include <ui/styles/styles.h>

void ModImplInternal::OnEnabled()
{
    forward_game_logs_module.Init();
}

void ModImplInternal::OnDisabled()
{
}

void ModImplInternal::OnTick()
{
}

void ModImplInternal::RenderUI()
{
    auto state = UpdateManager::GetUpdateState();
    if (state.stage != UpdateManager::UpdateState::IDLE)
    {
        ui::styles::warning::BeginPanel("update");
        UpdateManager::RenderMessage();
        ui::styles::warning::EndPanel();
        return;
    }
    ImGui::Spacing();
    ImGui::Separator();
    forward_game_logs_module.Render();
}

std::shared_ptr<modloader::Mod> ModImplInternal::CreateMod()
{
    auto info = std::make_shared<modloader::ModInfo>("$internal");
    info->title = "SuperMod";
    info->author = "zziger";
    info->version = semver::version::parse(SUPERMOD_VERSION);
    info->description = "Встроенные в мод патчи игры";

    EventManager::On<D3dInitEvent>([info]
    {
        const auto iconData = *utils::read_resource(RES_LOGO);
        const std::vector<byte> iconBuf(iconData.begin(), iconData.end());
        vector2ui iconSize {};
        if (const auto iconTex = PngLoader::LoadPngBuf(iconBuf, iconSize, { 1, 1 }))
        {
            const auto assetPool = game::AssetPool::Instance();
            const auto asset = assetPool->LoadAsset(iconTex, "$mod:icon:$internal", false, iconSize);
            info->icon = modloader::ModIcon(assetPool->MakeOwned(asset));
        }
    });

    auto mod = std::make_shared<modloader::Mod>(info, std::make_unique<ModImplInternal>());
    mod->SetFlag(modloader::Mod::Flag::INTERNAL);
    mod->Toggle(true);
    mod->Update();

    return mod;
}


