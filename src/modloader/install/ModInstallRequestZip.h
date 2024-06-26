#pragma once
#include <memory>
#include <mutex>
#include <utility>
#include <filesystem>
#include <zip_file.h>
#include <modloader/mod/info/ModInfo.h>
#include <modloader/mod/info/ModInfoFilesystem.h>
#include <utils/OwnedZip.h>

#include "ModInstallRequest.h"

namespace modloader {
    class ModInstallRequestZip final : public ModInstallRequest {
    public:
        enum class State
        {
            IDLE,
            INSTALLING,
            FINISHED
        };

    private:
        std::mutex mutex;

        std::atomic<State> state = State::IDLE;
        std::atomic<float> progressValue = 0.0f;
        std::string error; // lock with mutex
        Progress progress {};

        std::shared_ptr<OwnedZip> zip;
        std::string zipPath;

    public:
        explicit ModInstallRequestZip(const std::shared_ptr<ModInfo>& modInfo, const std::shared_ptr<OwnedZip>& zip, std::string zipPath)
            : ModInstallRequest(modInfo), zip(zip), zipPath(std::move(zipPath))
        {
        }

        void Install() override;
        void Cancel() override;
        bool IsFinished() override { return state == State::FINISHED; }
        bool IsInstalling() override { return state == State::INSTALLING; }
        std::string GetError() override;
        Progress GetProgress() override;

        static std::vector<std::shared_ptr<ModInstallRequest>> FromZip(const std::filesystem::path& path, bool remove = false);
    };
}