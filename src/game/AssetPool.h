#pragma once
#include <string>

#include "memory/Memory.h"
#include "thirdparty/directx/d3d8.h"

namespace game
{
    struct Asset {
        const char name[119]; // ingame has length of 128, 9 bytes went for my custom fields
        char constTex; // my custom field for asset swap impl
        int origWidth; // my custom field for asset swap impl
        int origHeight; // my custom field for asset swap impl
        int width;
        int height;
        IDirect3DTexture8* texture;
        byte isTga;
        byte isPoolDefault;
        D3DFORMAT format;

        void Anonymize() const;

        void ReplaceTexture(IDirect3DTexture8* tex);
    };
    
    class AssetPool {
    public:
        virtual ~AssetPool() = default;
        AssetPool() = delete;
        
        Asset* assets[1024];
        int assetCount;

        static Memory GetReadConstJpgMem();
        static Memory GetReadConstSurfaceMem();

        [[nodiscard]] Asset* GetByName(const std::string& name) const;

        static AssetPool* GetInstance();
    };
}