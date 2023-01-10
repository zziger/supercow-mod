#include "modloader/containers/EventContainer.h"

uint32_t EventContainer::operator+=(const uint32_t eventId) {
    std::lock_guard lock(_mutex);
    items.emplace(eventId);
    return eventId;
}

uint32_t EventContainer::operator-=(const uint32_t eventId) {
    std::lock_guard lock(_mutex);
    items.erase(eventId);
    return eventId;
}
void EventContainer::Unload() {
    std::lock_guard lock(_mutex);
    for (const auto& eventId : items) {
        EventManager::Off(eventId);
    }
    items.clear();
}