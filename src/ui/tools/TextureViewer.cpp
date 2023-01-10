#include <imgui.h>

#include "../UI.h"
#include "game/AssetPool.h"

namespace ui
{
    void UI::RenderTextureViewer() {
        static auto pool = game::AssetPool::GetInstance();
        const auto& io = ImGui::GetIO();
        if (ImGui::Begin("Просмотр текстур", &_textureViewerOpen)) {
            static char str0[128] = "";
            ImGui::InputText("Поиск", str0, IM_ARRAYSIZE(str0));
            
            for (auto i = 0; i < pool->assetCount; i++) {
                const auto asset = pool->assets[i];
                if (str0[0] != '\0' && std::string(asset->name).find(str0) == std::string::npos) continue;
                if (ImGui::TreeNode(asset->name, "%s (%dx%d)", asset->name, asset->width, asset->height)) {
                    const ImVec2 pos = ImGui::GetCursorScreenPos();
                    const auto aspectRatio = asset->height / (float) asset->width;
                    const auto width = std::min(ImGui::GetContentRegionAvail().x - ImGui::GetCursorPosX(), (float) asset->width);
                    auto texSize = ImVec2(width, aspectRatio * width);
                    ImGui::Image(asset->texture, texSize, { 0, 0 }, { 1, 1 }, 0xFFFFFFFF_color, 0xFFFFFFAA_color);
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        constexpr float regionSz = 64.0f;
                        constexpr float zoom = 2.0f;
                        auto off = regionSz * 0.5f;
                        const auto x = std::clamp(io.MousePos.x - pos.x, off, texSize.x - off);
                        const auto y = std::clamp(io.MousePos.y - pos.y, off, texSize.y - off);
                        ImGui::Text(
                            "X: %d Y: %d",
                            (int) ((io.MousePos.x - pos.x) / texSize.x * asset->width),
                            (int) ((io.MousePos.y - pos.y) / texSize.y * asset->height)
                        );
                        ImVec2 uv0{ (x - off) / texSize.x, (y - off) / texSize.y };
                        ImVec2 uv1{ (x + off) / texSize.x, (y + off) / texSize.y };
                        
                        if (asset->width < regionSz) {
                            uv0 = { 0, uv0.y };
                            uv1 = { 1, uv1.y };
                        }

                        if (asset->height < regionSz) {
                            uv0 = { uv0.x, 0 };
                            uv1 = { uv1.x, 1 };
                        }
                        
                        ImGui::Image(asset->texture, ImVec2(regionSz * zoom, regionSz * zoom), uv0, uv1);
                        ImGui::EndTooltip();
                    }
                    ImGui::TreePop();
                }
            }
        }
        ImGui::End();
    }
}