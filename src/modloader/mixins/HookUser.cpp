#include "modloader/mixins/HookUser.h"
#include "memory/HookManager.h"


Memory HookUser::UnregisterHook(const Memory mem) {
    return _hooks -= HookManager::UnregisterHook(mem);
}

void HookUser::UnloadHooks() {
    _hooks.Unload();
}