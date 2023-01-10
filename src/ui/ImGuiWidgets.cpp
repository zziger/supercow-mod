#include "ImGuiWidgets.h"

#include "UI.h"
#include "events/WindowEvent.h"
#include <imgui/imgui_internal.h>

#include "thirdparty/IconsMaterialDesign.h"

namespace ui::widgets
{
    void TextCentered(const std::string text) {
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text.c_str());
    }

    bool MenuTabSelectable(const char* title, const char* icon, const bool selected) {
        ImGui::PushID(title);
        const auto value = ImGui::Selectable("", selected, ImGuiSelectableFlags_SetNavIdOnHover);
        ImGui::SameLine();
        TextCentered(icon);
        ImGui::PushFont(UI::fonts->fontDefault);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip(title);
        ImGui::PopFont();
        ImGui::Spacing();
        ImGui::PopID();
        return value;
    }

    bool ModSelectable(const char* id, const char* title, const char* author, const char* ver, LPDIRECT3DTEXTURE8 icon, bool selected, bool disabled) {
        ImGui::PushID(id);
        const auto style = ImGui::GetStyle();
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        auto dc = window->DC;

        auto lh = ImGui::GetTextLineHeight();
            
        auto textSize = ImGui::CalcTextSize("\n ", nullptr, false);
        auto elementHeight = textSize.y;
        auto verSize = ImGui::CalcTextSize(ver, nullptr, false);
            
        ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
        const auto val = ImGui::Selectable("", selected, 0, { 0, elementHeight });
        ImGui::Spacing();
        ImVec2 endCursorPos = ImGui::GetCursorScreenPos();

        ImGui::SameLine();

        ImGui::SetCursorScreenPos(startCursorPos);

        if (icon) {
            ImGui::Image(icon, { elementHeight, elementHeight }, { 0, 0 }, { 1, 1 }, disabled ? 0x808080FF_color : 0xFFFFFFFF_color);
            
            ImGui::SameLine();
        
            ImGui::SetCursorScreenPos({ startCursorPos.x + elementHeight + style.ItemInnerSpacing.x * 2, startCursorPos.y });
        }
            
        ImGui::BeginGroup();
        const auto availableWidth = window->WorkRect.Max.x - window->DC.CursorPos.x;
        const auto basePos = window->DC.CursorPos;

        if (disabled) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        
        {
            // moving mod name half a line down if no author is specified
            ImVec2 pos = { window->DC.CursorPos.x, window->DC.CursorPos.y + (strlen(author) ? 0 : lh / 2.f) };
            ImVec2 size = { availableWidth, lh };
            ImGui::RenderTextEllipsis(draw_list, pos, { pos.x + size.x, pos.y + size.y }, pos.x + size.x, pos.x + size.x, title, nullptr, nullptr);
        }

        {
            ImVec2 pos = { window->DC.CursorPos.x, window->DC.CursorPos.y + lh };
            ImVec2 size = { std::max(0.f, availableWidth - verSize.x - style.ItemSpacing.x), lh };
            ImGui::RenderTextEllipsis(draw_list, pos, { pos.x + size.x, pos.y + size.y }, pos.x + size.x, pos.x + size.x, author, nullptr, nullptr);
        }
            
        {
            ImVec2 pos = { window->DC.CursorPos.x + availableWidth - verSize.x, window->DC.CursorPos.y + lh };
            ImVec2 size = { verSize.x, lh };
            ImGui::RenderTextClipped(pos, { pos.x + size.x, pos.y + size.y }, ver, nullptr, nullptr);
        }
        if (disabled) ImGui::PopStyleColor();

        ImGui::EndGroup();
        ImGui::SetCursorScreenPos(endCursorPos);
        ImGui::PopID();
        return val;
    }

    void HelpMarker(const char* desc, const char* icon) {
        ImGui::TextDisabled(icon);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    namespace styles
    {
        void PushButtonDanger() {
            ImGui::PushStyleColor(ImGuiCol_Button, 0x993D3DFF_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xB33636FF_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xCC2929FF_color);
        }
        void PopButtonDanger() {
            ImGui::PopStyleColor(3);
        }
    }
}