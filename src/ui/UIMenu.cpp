#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <windows.h>
#include <shellapi.h>

#include "Config.h"
#include "Console.h"
#include "DirectXUtils.h"
#include "thirdparty/IconsFontAwesome5Brands.h"
#include "thirdparty/IconsMaterialDesign.h"
#include "UI.h"
#include "modloader/mods/ModManager.h"
#include "ImGuiWidgets.h"
#include "game/AssetPool.h"
#include "sdk/Game.h"

using namespace ui;

std::string tabNames[] = { "Моды", "Настройки", "Инструменты" };

static int currentTab = 0;


void Tab(const char* icon, int index) {
    if (widgets::MenuTabSelectable(tabNames[index].c_str(), icon, currentTab == index)) currentTab = index;
}

void Link(const char* title, const char* icon, const char* url) {
    if (widgets::MenuTabSelectable(title, icon, false)) {
        ShellExecuteA(nullptr, "open", url, nullptr, nullptr , SW_SHOWNORMAL);
    }
}


void UI::RenderModsTab() {
    static std::optional<std::shared_ptr<Mod>> activeMod;
    
    auto width = ImGui::GetContentRegionMax().x / 2;
    ImGui::BeginChild("mods", ImVec2(width, 0), true);

    for (const auto& loadedMod : ModManager::GetLoadedMods()) {
        if (!loadedMod) continue;
        auto info = loadedMod->info;
        if (widgets::ModSelectable(info.id.c_str(), info.title.c_str(), info.author.c_str(),
            info.version.c_str(), info.icon, activeMod && (*activeMod)->info.id == info.id, !loadedMod->IsLoaded()))
            activeMod = loadedMod;
    }
    
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("modContent", ImVec2(width, 0), false);

    if (activeMod) {
        auto mod = *activeMod;
        auto info = mod->info;
        if (info.icon) {
            ImGui::Image(info.icon, { 50, 50 });
            ImGui::SameLine();
        }
        
        ImGui::BeginGroup();
        ImGui::Text("%s", info.title.c_str());
        if (info.version != "") ImGui::Text("%s", info.version.c_str());
        ImGui::EndGroup();

        if (info.author != "") ImGui::Text("Автор: %s", info.author.c_str());
        
        if (!mod->info.internal) {
            ImGui::Text("Состояние:");
        
            ImGui::SameLine();
            if (mod->IsLoaded()) ImGui::TextColored(0x77EE77FF_color, "Включен");
            else ImGui::TextColored(0xEE7777FF_color, "Выключен");

            if (!mod->info.compatible) {
                ImGui::PushStyleColor(ImGuiCol_Text, 0xFFA000FF_color);
                ImGui::TextWrapped("Мод не совместим с текущей версией игры!");
                ImGui::PopStyleColor();
            }

            const auto currentVersion = sdk::Game::GetGameVersion();
            if (!mod->info.gameVersions.empty()) {
                if (ImGui::TreeNode("Поддерживаемые версии игры:")) {
                    for (auto gameVersion : mod->info.gameVersions) {
                        ImGui::Text(sdk::Game::SerializeGameVersion(gameVersion).c_str());
                        if (currentVersion == gameVersion) {
                            ImGui::SameLine();
                            widgets::HelpMarker("Текущая версия", ICON_MD_DONE);
                        }
                    } 
                    ImGui::TreePop();   
                }
            }

            if (ImGui::Button(mod->IsLoaded() ? "Выключить" : "Включить")) {
                if (mod->IsLoaded()) mod->Unload(true);
                else mod->Load(true);
            }

            ImGui::SameLine();
            if (ImGui::Button("Открыть папку")) {
                ShellExecuteA(nullptr, "explore", mod->info.basePath.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }

            widgets::styles::PushButtonDanger();
            if (ImGui::Button("Удалить")) {
                ImGui::OpenPopup("Удаление мода");
            };
            widgets::styles::PopButtonDanger();

            if (ImGui::BeginPopupModal("Удаление мода", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Вы уверены, что хотите удалить мод %s?", mod->info.title);

                ImGui::Spacing();
                widgets::styles::PushButtonDanger();
                if (ImGui::Button("Да, удалить")) {
                    ModManager::DeleteMod(mod);
                    activeMod = std::nullopt;
                }
                widgets::styles::PopButtonDanger();
                ImGui::SetItemDefaultFocus();
    
                ImGui::SameLine();
                if (ImGui::Button("Отмена")) {
                    ImGui::CloseCurrentPopup();
                }
    
                ImGui::EndPopup();
            }
        }

        if (mod->modules.items.size() != 0) {
            ImGui::Separator();
            ImGui::Text("Модули:");
        }
        auto& modules = mod->modules.items;
        for (auto module : modules) {
            if (!module) continue;
            ImGui::PushID(module->fullId.c_str());
            auto state = module->IsLoaded();
            ImGui::Checkbox(module->name.c_str(), &state);

            if (module->desc != nullptr) {
                ImGui::SameLine();
                widgets::HelpMarker(module->desc);
            }
            
            if (state != module->IsLoaded()) {
                if (state) module->Load(true);
                else module->Unload(true);
            }

            module->RenderModuleUI();
            ImGui::PopID();
        }
        
        ImGui::Separator();
        mod->RenderUI();
    } else {
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::Selectable("Выберите мод из\nсписка слева", false, ImGuiSelectableFlags_Disabled, ImGui::GetContentRegionAvail());
        ImGui::PopStyleVar();
    }
    
    ImGui::EndChild();
}

void UI::RenderSettingsTab() {
    auto width = ImGui::GetContentRegionMax().x / 2;
    ImGui::PushItemWidth(width);

    ImGui::PushID("watermark");
    ImGui::Text("Водяной знак");
    if (ImGui::Checkbox("Отображать", &showWatermark)) {
        const Config cfg;
        cfg.data["watermark"]["show"] = showWatermark;
    }
    
    if (showWatermark) {
        const char* positions[] = { "Сверху слева", "Сверху по центру", "Сверху справа", "Снизу слева", "Снизу по центру", "Снизу справа" };
        if (ImGui::Combo("Позиция", (int*) &watermarkPosition, positions, IM_ARRAYSIZE(positions))) {
            const Config cfg;
            cfg.data["watermark"]["position"] = (int) watermarkPosition;
        }

        if (ImGui::SliderFloat("Непрозрачность", &watermarkOpacity, 0, 1)) {
            const Config cfg;
            cfg.data["watermark"]["opacity"] = watermarkOpacity;
        }
        
        if (ImGui::SliderFloat("Непрозрачность фона", &watermarkBgOpacity, 0, 1)) {
            const Config cfg;
            cfg.data["watermark"]["bgOpacity"] = watermarkBgOpacity;
        }
    }
    ImGui::PopID();

    ImGui::PushID("console");
    ImGui::Text("Консоль");
    bool consoleEnabled = Console::enabled;
    if (ImGui::Checkbox("Отображать", &consoleEnabled)) {
        const Config cfg;
        cfg.data["console"] = consoleEnabled;
        if (consoleEnabled) Console::Enable();
        else Console::Disable();
    }
    ImGui::PopID();
}

void UI::RenderToolsTab() {
    if (ImGui::Button("Перезапустить игру")) {
        ImGui::OpenPopup("Перезапуск игры");
    }
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Перезапуск игры", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Вы уверены, что хотите перезапустить игру?");
    
        if (ImGui::Button("Да")) {
            sdk::Game::Restart();
        }
        ImGui::SetItemDefaultFocus();
    
        ImGui::SameLine();
        if (ImGui::Button("Отмена")) {
            ImGui::CloseCurrentPopup();
        }
    
        ImGui::EndPopup();
    }

    if (ImGui::Button("Просмотр текстур")) {
        _textureViewerOpen = true;
    }

    // auto pool = game::AssetPool::GetInstance();
    // auto asset = pool->GetByName("particles.tga");
    // ImGui::Image(asset->texture, ImGui::GetContentRegionMax());
}

void UI::RenderMenu() {
    RenderWindows();
    
    if (!menuOpen) return;
    
    const auto mods = ModManager::GetLoadedMods();

    constexpr auto lowerElements = 2;
    constexpr auto upperElements = 3;

    const auto style = ImGui::GetStyle();
    const auto spacing = style.ItemSpacing;
    constexpr auto elements = lowerElements + upperElements;
    const auto fontSize = fonts->fontDefault2X->FontSize;
    const auto elementSize = fontSize + spacing.y * 2;
    const auto menuSize = elementSize * (elements + 0.5f);
    const auto headerSize = ImGui::GetFontSize();
    
    ImGui::ShowDemoWindow();
    ImGui::SetNextWindowSize(ImVec2(700, 300), ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(ImVec2(500, menuSize + headerSize), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::Begin(std::format("SuperMod / {}###MainWindow", tabNames[currentTab]).c_str())) {
        ImGui::BeginChild("menu", ImVec2(fontSize, 0));
        
        ImGui::BeginChild("upper", ImVec2(fontSize, (-fontSize - spacing.y * 2) * 2));
        ImGui::PushFont(UI::fonts->fontDefault2X);
        Tab(ICON_MD_FORMAT_LIST_BULLETED, 0);
        Tab(ICON_MD_SETTINGS, 1);
        Tab(ICON_MD_BUILD, 2);
        ImGui::PopFont();
        ImGui::EndChild();
        
        ImGui::PushFont(UI::fonts->fontFab2X);
        Link("Discord сервер", ICON_FA_DISCORD, "https://discord.gg/UcSzFC9tCB");
        Link("GitHub репозиторий", ICON_FA_GITHUB, "https://github.com/zziger/supercow-mod");
        ImGui::PopFont();
        
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("content", ImVec2(0, 0));

        switch (currentTab) {
            case 0: UI::RenderModsTab(); break;
            case 1: UI::RenderSettingsTab(); break;
            case 2: UI::RenderToolsTab(); break;
        }
        
        ImGui::EndChild();
    }
    ImGui::End();
}