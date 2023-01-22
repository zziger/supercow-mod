#include "UI.h"
#include "events/EventManager.h"
#include "events/GameLoadedEvent.h"
#include "events/TickEvent.h"
#include "events/WindowEvent.h"
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui-dx8/imgui_impl_dx8.h>
#include "Config.h"
#include "Utils.h"
#include "events/UIRenderEvent.h"
#include "modloader/mods/ModManager.h"
#include "sdk/DirectX.h"
#include "sdk/Game.h"


using namespace ui;


void UI::LoadFonts(const ImGuiIO& io) {
    fonts = new UIFonts(io);
}

static std::string imguiIniFilename;
static std::string imguiLogFilename;

void UI::InitImGui() {
    if (initialized) return;
        
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
        
    ImGuiIO& io = ImGui::GetIO();

    imguiIniFilename = (sdk::Game::GetRootPath() / "imgui.ini").string();
    io.IniFilename = imguiIniFilename.c_str();
    
    imguiLogFilename = (sdk::Game::GetRootPath() / "imguilog.txt").string();
    io.LogFilename = imguiLogFilename.c_str();
        
    LoadFonts(io);
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        
    ImGui_ImplWin32_Init(*sdk::Game::window);
    ImGui_ImplDX8_Init(*sdk::DirectX::d3dDevice);
    
    io.IniFilename = imguiIniFilename.c_str();
    io.LogFilename = imguiLogFilename.c_str();
        
    initialized = true;
}

void UI::Render() {
    if (!initialized) return;
    
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = imguiIniFilename.c_str();
    io.LogFilename = imguiLogFilename.c_str();

    if (menuOpen) io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    else io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false))
        menuOpen = !menuOpen;

    ImGui_ImplDX8_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    try {
        RenderWatermark();
        RenderMenu();

        for (auto mod : ModManager::GetMods()) {
            if (!mod->IsEnabled()) continue;
            mod->Render();
        }

        EventManager::Emit(UiRenderEvent());
    } catch(std::exception& e) {
        Log::Error << "Произошла ошибка в отрисовке кадра: " << e.what() << Log::Endl;
    }

    ImGui::ErrorCheckEndFrameRecover([](void*, const char* format, ...) {
        va_list va;
        va_start(va, format);
        char buffer[1024];
        vsprintf_s(buffer, 1024, format, va);
        Log::Warn << "Ошибка стека ImGui:\n\t" << buffer << Log::Endl;
    }, nullptr);
    ImGui::EndFrame();

    const auto d3dDevice = *sdk::DirectX::d3dDevice;
    if (d3dDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());
        d3dDevice->EndScene();
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UI::OnWindowEvent(WindowEvent& evt) {
    if (ImGui_ImplWin32_WndProcHandler(evt.hWnd, evt.msg, evt.wParam, evt.lParam)) return evt.Cancel();
    if (!initialized) return;
    
    const ImGuiIO& io = ImGui::GetIO();
    
    if (io.WantCaptureMouse || menuOpen)
        if (evt.msg >= 0x200 && evt.msg <= 0x20a) // mouse events
            return evt.Cancel();
    
    if (io.WantCaptureKeyboard)
        if (evt.msg == WM_KEYDOWN || evt.msg == WM_KEYUP)
            return evt.Cancel();
}

static inline int (__thiscall *AssetPool__freeAssetsFromD3d_orig)(void* this_) = nullptr;
static int __fastcall AssetPool__freeAssetsFromD3d(void* this_, void*) {
    ImGui_ImplDX8_InvalidateDeviceObjects();
    return AssetPool__freeAssetsFromD3d_orig(this_);
}

void UI::Init() {
    EventManager::On<WindowEvent>(OnWindowEvent);
    EventManager::On<TickEvent>([] {
        InitImGui();
        Render();
    });

    HookManager::RegisterHook("55 8B EC 83 EC ? 89 4D ? C7 45 ? ? ? ? ? EB ? 8B 45 ? 83 C0 ? 89 45 ? 8B 4D ? 8B 55 ? 3B 91 ? ? ? ? 7D ? 8B 45 ? 8B 4D ? 8B 54 81 ? 0F B6 82",
        HOOK_REF_FORCE(AssetPool__freeAssetsFromD3d));

    auto watermarkCfg = Config::Get()["watermark"];
    showWatermark = watermarkCfg["show"].as<bool>(true);
    watermarkPosition = (WatermarkPosition) watermarkCfg["position"].as<int>(TOP_CENTER);
    watermarkOpacity = watermarkCfg["opacity"].as<float>(1.0f);
    watermarkBgOpacity = watermarkCfg["bgOpacity"].as<float>(0.35f);
}

inline EventManager::Ready $([] {
    UI::Init();
});