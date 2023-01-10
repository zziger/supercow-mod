#include "ModFileResolver.h"
#include "Utils.h"
#include "sdk/Game.h"

void ModFileResolver::FileListenerThread() {
    auto sus = (std::filesystem::current_path() / "mods").generic_string();
    const HANDLE hDir = CreateFileA(
        sus.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        nullptr, 
        OPEN_EXISTING, 
        FILE_FLAG_BACKUP_SEMANTICS, 
        nullptr);

    byte buf[1024];
    DWORD dwBytesReturned = 0;   

    while(listenerEnabled)
    {
        if(
            ReadDirectoryChangesW(
                hDir,
                &buf,
                sizeof buf,
                true,
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
                &dwBytesReturned,
                nullptr,
                nullptr
            )
        )
        {
            BYTE* ptr = buf;
            while(true) {
                const auto info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(ptr);
                auto widePath = std::wstring(info->FileName, info->FileName + info->FileNameLength / sizeof(char16_t));
                auto path = std::filesystem::path(convert.to_bytes((char16_t*) widePath.c_str()));
                auto newPath = std::filesystem::path();

                auto foundData = false;
                for(const auto& el : path) {
                    if (el != "data" && !foundData) continue;
                    foundData = true;
                    newPath = newPath / el;
                }
                if (!foundData) continue;
                
                std::lock_guard lock(_reloadMutex);
                filesToReload.emplace(newPath.generic_string());
                
                if (!info->NextEntryOffset) break;
                ptr += info->NextEntryOffset;
            }
        }
        else
        {
            Log::Debug << "FAILED! " << GetLastError() << Log::Endl;   
        }
    }

    Log::Info << "File listener finished" << Log::Endl;
}