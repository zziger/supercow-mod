/// This patch allows to start multiple game instances at the same time
/// It's important for game restart feature implementation

#include "Utils.h"
#include "events/EventManager.h"
#include "memory/HookManager.h"

HOOK_FN_CONV(inline int, can_start_game, ARGS(const char* lpName), __cdecl) {
    return 1;
}

inline EventManager::Ready $allow_multiple_instances_patch([] {
    HookManager::RegisterHook("55 8B EC 51 8B 45 ? 50 6A ? 6A ? FF 15", HOOK_REF(can_start_game));
});
