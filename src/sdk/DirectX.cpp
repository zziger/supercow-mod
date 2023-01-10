#include "DirectX.h"

#include "events/EventManager.h"
#include "events/TickEvent.h"

namespace sdk
{
    void DirectX::Init() {
        constexpr Memory::Pattern pushD3dDevicePointer("68 ? ? ? ? 68 ? ? ? ? 6A ? 8B 4D 08");
        d3dDevice = *pushD3dDevicePointer.Search().Get<IDirect3DDevice8***>(1);
    }

    static inline bool deviceResetRequested = false;
    void DirectX::ResetDevice() {
        if (deviceResetRequested) return;
        deviceResetRequested = true;
        static auto reset = resetD3dDevicePat.Search();
        static auto wasReset = deviceWasResetPat.Search();
        EventManager::Once<AfterTickEvent>([] {
            reset.Call();
            **wasReset.Get<byte**>(2) = 1;
            deviceResetRequested = false;
        });
    }
}