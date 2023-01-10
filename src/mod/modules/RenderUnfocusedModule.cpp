#include "RenderUnfocusedModule.h"

#include "events/WindowEvent.h"

void RenderUnfocusedModule::OnLoad(bool manual) {
    OnEvent<WindowEvent>([](auto& evt) {
        if (evt.msg == WM_ACTIVATE && evt.wParam == WA_INACTIVE) evt.Cancel();
    });
}