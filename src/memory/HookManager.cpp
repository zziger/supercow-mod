#include "HookManager.h"

Memory HookManager::UnregisterHook(Memory mem) {
    mem.DeactivateDetour();
    return mem;
}

