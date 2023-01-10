#include "AdaptiveResolutionModule.h"

vector2i AdaptiveResolutionModule::GetTargetResolution() {
    auto cfgBlock = Config::Get()["adaptiveResolution"];
    const auto mode = cfgBlock["mode"].as<int>(window); 
    vector2i res {};
    RECT rect;
        
    switch((resolution_mode) mode) {
        case window:
            GetClientRect(*sdk::Game::window, &rect);
            res = { rect.right - rect.left, rect.bottom - rect.top };
            break;
        case screen:
            static HWND desktopWnd = GetDesktopWindow();
            GetWindowRect(desktopWnd, &rect);
            res = { rect.right - rect.left, rect.bottom - rect.top };
            break;
        case custom:
            res = { cfgBlock["width"].as<int>(800), cfgBlock["width"].as<int>(600) };
    }

    return res;
}

HOOK_FN(inline static int, setup_d3d_params, ARGS())
{
    static constexpr Memory::Pattern paramsSetPat("89 15 ? ? ? ? 8B 41 04");
    static constexpr Memory::Pattern set32BitPat("C7 05 ? ? ? ? ? ? ? ? D9 ? ? ? ? ? D8 ? ? ? ? ? E8");
        
    static int* set32BitPtr = *set32BitPat.Search().Get<int**>(2);
    *set32BitPtr = 1;
        
    static int* ptr = *paramsSetPat.Search().Get<int**>(2);
    const auto value = setup_d3d_params_orig();
        
    const auto [x, y] = AdaptiveResolutionModule::GetTargetResolution();
    Log::Debug << "Setting render params to " << x << "x" << y << Log::Endl;
        
    ptr[0] = x;
    ptr[1] = y;
    ptr[2] = 21;
        
    return value;
}


void AdaptiveResolutionModule::OnLoad(const bool manual) {
    RegisterHook("55 8B EC 51 C7 45 FC ? ? ? ? EB ? 8B 45 FC 83 C0 ? 89 45 FC 8B 4D FC 3B 0D ? ? ? ? 7D ? 8B 55 FC 8B 04 D5",
                 HOOK_REF(setup_d3d_params));
        
    OnEvent<WindowEvent>([](const WindowEvent& evt) {
        if (evt.hWnd != *sdk::Game::window) return;
            
        if (evt.msg == WM_SIZE) sdk::DirectX::ResetDevice();
            
        if (evt.msg == WM_STYLECHANGED && evt.wParam == GWL_STYLE) {
            const auto style = (STYLESTRUCT*) evt.lParam;
            if ((style->styleNew & required_styles) != required_styles)
                SetWindowLongA(evt.hWnd, GWL_STYLE, style->styleNew | required_styles);
        }
            
        if (evt.msg == WM_ACTIVATE) {
            const auto oldStyle = GetWindowLongA(*sdk::Game::window, GWL_STYLE);
            if ((oldStyle & required_styles) != required_styles)
                SetWindowLongA(*sdk::Game::window, GWL_STYLE, oldStyle | required_styles);
        }
    });
        
    if (manual) {
        sdk::DirectX::ResetDevice();
        SetWindowLongA(*sdk::Game::window, GWL_STYLE, GetWindowLongA(*sdk::Game::window, GWL_STYLE) | required_styles);
    }
}

void AdaptiveResolutionModule::OnUnload(const bool manual) {
    if (manual) sdk::DirectX::ResetDevice();
}

void AdaptiveResolutionModule::RenderModuleUI() {
    if (ImGui::TreeNode("Настройки")) {
        ImGui::Text("Разрешение");
        auto cfgBlock = Config::Get()[id];

        static bool changed = false;
        static int mode = cfgBlock["mode"].as<int>(0);
        if (ImGui::RadioButton("Разрешение окна", &mode, 0)) changed = true;
        ImGui::SameLine();
        ui::widgets::HelpMarker("Рекомендуемый вариант", ICON_MD_SETTINGS_SUGGEST);
        if (ImGui::RadioButton("Разрешение экрана", &mode, 1)) changed = true;
        if (ImGui::RadioButton("Ввести вручную", &mode, 2)) changed = true;;

        static int res[2] = { cfgBlock["width"].as<int>(800), cfgBlock["height"].as<int>(600) };
        
        if (mode == custom) {
            if (ImGui::InputInt2("##resolution", res)) changed = true;
        }

        if (changed) {
            if (ImGui::Button("Сохранить")) {
                const Config cfg;
                cfg.data[id]["mode"] = mode;
                cfg.data[id]["width"] = res[0];
                cfg.data[id]["height"] = res[1];
                sdk::DirectX::ResetDevice();
                changed = false;
            }
        }
        ImGui::TreePop();
    }
}