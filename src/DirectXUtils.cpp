#include "DirectXUtils.h"

#include <format>
#include <vector>

#include "exceptions/Error.h"
#include <thirdparty/lodepng/lodepng.h>

namespace dx_utils
{
    LPDIRECT3DTEXTURE8 load_png(IDirect3DDevice8* device, const char* filename) {
        std::vector<uint8_t> buf;
        uint32_t width, height;
        
        const auto readRes = lodepng::decode(buf, width, height, filename);
        if (readRes != 0) throw Error("Не удалось прочитать файл " + std::string(filename));
        
        LPDIRECT3DTEXTURE8 texture = nullptr;
        const auto res = device->CreateTexture(width, height, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture);
        
        if (res < 0) throw Error(std::format("Не удалось создать текстуру. Ошибка: {}", res));
        if (texture == nullptr) throw Error("Не удалось создать текстуру");
        D3DLOCKED_RECT bits;
        
        if (FAILED(texture->LockRect(0, &bits, nullptr, 0))) throw Error("Не удалось создать текстуру. Ошибка: Не удалось заблокировать текстуру для записи");
        
        for (uint32_t i = 0; i < width * height; i++) {
            const auto out = (uint8_t*) bits.pBits; 
            out[i * 4 + 3] = buf[i * 4 + 3];
            out[i * 4 + 2] = buf[i * 4 + 0];
            out[i * 4 + 1] = buf[i * 4 + 1];
            out[i * 4 + 0] = buf[i * 4 + 2];
        }

        if (FAILED(texture->UnlockRect(0))) throw Error("Не удалось создать текстуру. Ошибка: Не удалось разблокировать текстуру после записи");
        
        return texture;
    }
}