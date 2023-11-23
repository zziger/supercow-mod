#include "Utils.h"
#include "events/EventManager.h"
#include "memory/HookManager.h"

BOOL (__stdcall *SetCurrentDirectoryAOrig)(LPCSTR lpPathName) = nullptr;
BOOL __stdcall SetCurrentDirectoryAHook(LPCSTR lpPathName)
{
    try
    {
        std::filesystem::current_path(lpPathName);
        return true;
    } catch(...)
    {
        return false;
    }
}

DWORD (__stdcall *GetCurrentDirectoryAOrig)(DWORD nBufferLength, LPSTR lpBuffer) = nullptr;
DWORD __stdcall GetCurrentDirectoryAHook(DWORD nBufferLength, LPSTR lpBuffer)
{
    try
    {
        auto cwd = std::filesystem::current_path();
        if (lpBuffer)
        {
            auto len = cwd.string().size();
            if (len > nBufferLength)
            {
                return len;
            }
            memcpy(lpBuffer, cwd.string().c_str(), len);
            lpBuffer[len] = '\0';
        }
        return cwd.string().size();
    } catch(...)
    {
        return E_FAIL;
    }
}

inline EventManager::Ready $cyrillic_path_patch([] {
    HookManager::RegisterHook(Memory(SetCurrentDirectoryA), SetCurrentDirectoryAHook, &SetCurrentDirectoryAOrig);
    HookManager::RegisterHook(Memory(GetCurrentDirectoryA), GetCurrentDirectoryAHook, &GetCurrentDirectoryAOrig);
});

