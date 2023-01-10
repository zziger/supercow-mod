#pragma once
#include <mutex>
#include <unordered_set>

#include "events/EventManager.h"

struct EventContainer {
    std::unordered_set<uint32_t> items {};

    uint32_t operator += (uint32_t eventId);

    uint32_t operator -= (uint32_t eventId);

    void Unload();

private:
    inline static std::recursive_mutex _mutex {};
};
