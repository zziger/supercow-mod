#pragma once
#include "memory/Memory.h"
#include "thirdparty/directx/d3d8.h"

namespace sdk
{
    class DirectX {
    public:
        static inline IDirect3DDevice8** d3dDevice = nullptr;
        static constexpr Memory::Pattern resetD3dDevicePat{ "55 8B EC 83 EC ? E8 ? ? ? ? A1" };
        static constexpr Memory::Pattern deviceWasResetPat{ "C6 05 ? ? ? ? ? 0F B6 0D ? ? ? ? 85 C9 75 ? 83 3D" }; // mov deviceWasReset, 1

        static void Init();
        static void ResetDevice();
    };
}