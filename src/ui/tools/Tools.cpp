#include "../UI.h"

namespace ui
{
    void UI::RenderWindows() {
        if (_textureViewerOpen) RenderTextureViewer();
    }
}