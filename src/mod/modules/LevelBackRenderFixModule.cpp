#include "LevelBackRenderFixModule.h"

#include "Config.h"
#include "Utils.h"

HOOK_FN(char, prepare_levelback_stuff, ARGS()) {
    prepare_levelback_stuff_orig();
    return 0;
}

void LevelBackRenderFixModule::OnLoad(bool manual) {
    RegisterHook("55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 89 45 ? 68 ? ? ? ? 6A ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? 0F B6 05", HOOK_REF(prepare_levelback_stuff));
}