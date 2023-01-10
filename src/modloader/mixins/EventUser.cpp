#include "modloader/mixins/EventUser.h"

uint32_t EventUser::OffEvent(const uint32_t eventId) {
    return _events -= eventId;
}

void EventUser::UnloadEvents() {
    _events.Unload();
}