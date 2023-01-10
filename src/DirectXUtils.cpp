#include "DirectXUtils.h"

#include <format>
#include <vector>
#include <fstream>

#include "exceptions/Error.h"
#include <thirdparty/lodepng/lodepng.h>

#include "data.h"
#include "Log.h"
#include "thirdparty/nanojpeg.h"

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

    LPDIRECT3DTEXTURE8 load_jpg(IDirect3DDevice8* device, const char* filename) {
        // std::vector<uint8_t> buf;

        std::ifstream fstream{ filename, std::ios::binary | std::ios::ate };
        int size = fstream.tellg();
        fstream.seekg(0, std::ios::beg);
        std::vector<char> buffer;
        buffer.resize(size);
        fstream.read(buffer.data(), size);


        const auto readRes = njDecode(buffer.data(), buffer.size());
        if (readRes != 0) throw Error(std::format("Не удалось прочитать файл {}. Ошибка: {}", filename, (int) readRes));

        uint32_t width = njGetWidth(), height = njGetHeight();
        auto buf = njGetImage();

        Log::Debug << "Sussy " << width << " " << height << Log::Endl;

        LPDIRECT3DTEXTURE8 texture = nullptr;
        const auto res = device->CreateTexture(width, height, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture);

        if (res < 0) throw Error(std::format("Не удалось создать текстуру. Ошибка: {}", res));
        if (texture == nullptr) throw Error("Не удалось создать текстуру");
        D3DLOCKED_RECT bits;

        if (FAILED(texture->LockRect(0, &bits, nullptr, 0))) throw Error("Не удалось создать текстуру. Ошибка: Не удалось заблокировать текстуру для записи");

        for (uint32_t i = 0; i < width * height; i++) {
            const auto out = (uint8_t*) bits.pBits;
            out[i * 4 + 3] = 255;
            out[i * 4 + 2] = buf[i * 3 + 0];
            out[i * 4 + 1] = buf[i * 3 + 1];
            out[i * 4 + 0] = buf[i * 3 + 2];
        }

        if (FAILED(texture->UnlockRect(0))) throw Error("Не удалось создать текстуру. Ошибка: Не удалось разблокировать текстуру после записи");

        return texture;
    }

    bool get_image_dimensions(const std::string& filename, int& x, int& y) {
        std::ifstream file { filename, std::ios::binary | std::ios::ate };
        const auto len = file.tellg();
        if (len < 24) return false;
        file.seekg(0, std::ios::beg);

        // Strategy:
        // reading GIF dimensions requires the first 10 bytes of the file
        // reading PNG dimensions requires the first 24 bytes of the file
        // reading JPEG dimensions requires scanning through jpeg chunks
        // In all formats, the file is at least 24 bytes big, so we'll read that always
        std::vector<byte> buf(24);
        file.read((char*) buf.data(), 24);

        // For JPEGs, we need to read the first 12 bytes of each chunk.
        // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
        if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 && buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F') {
            long pos = 2;
            while (buf[2] == 0xFF) {
                if (buf[3] == 0xC0 || buf[3] == 0xC1 || buf[3] == 0xC2 || buf[3] == 0xC3 || buf[3] == 0xC9 || buf[3] == 0xCA || buf[3] == 0xCB) break;
                pos += 2 + (buf[4] << 8) + buf[5];
                if (pos + 12 > len) break;
                file.seekg(pos, std::ios::beg);
                file.read((char*) buf.data() + 2, 12);
            }
        }

        // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
        if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF) {
            y = (buf[7] << 8) + buf[8];
            x = (buf[9] << 8) + buf[10];
            return true;
        }

        // GIF: first three bytes say "GIF", next three give version number. Then dimensions
        if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F') {
            x = buf[6] + (buf[7] << 8);
            y = buf[8] + (buf[9] << 8);
            return true;
        }

        // PNG: the first frame is by definition an IHDR frame, which gives dimensions
        if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A
            && buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R') {
            x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
            y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
            return true;
        }

        return false;
    }
}