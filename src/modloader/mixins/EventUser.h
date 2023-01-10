#pragma once
#include "events/EventManager.h"
#include "modloader/containers/EventContainer.h"

class EventUser {
    EventContainer _events {};
    
protected:
    template <std::derived_from<IAnyEvent> Event>
    uint32_t OnEvent(std::function<void(Event&)> fn) {
        const uint32_t eventId = EventManager::On<Event>(fn);
        return _events += eventId;
    }

    template <std::derived_from<IAnyEvent> Event>
    uint32_t OnEvent(const std::function<void()>& fn) {
        const uint32_t eventId = EventManager::On<Event>(fn);
        return _events += eventId;
    }

    uint32_t OffEvent(uint32_t eventId);

public:
    virtual ~EventUser() = default;
    void UnloadEvents();
};