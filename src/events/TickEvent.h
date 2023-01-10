#pragma once
#include "EventManager.h"
#include "Utils.h"
#include "memory/HookManager.h"
#include "memory/Memory.h"

struct BeforeTickEvent final : IEvent<"beforeTick"> {};
struct AfterTickEvent final : IEvent<"afterTick"> {};

typedef BeforeTickEvent TickEvent; 

inline int (__thiscall *render_orig)(void* this_) = nullptr;
inline int  __fastcall render(void* this_, void*) {
    EventManager::Emit(TickEvent());
    auto res = render_orig(this_);
    EventManager::Emit(AfterTickEvent());
    return res;
}

inline EventManager::Ready $tick_event_hook([] {
    HookManager::RegisterHook("55 8B EC 51 E8 ? ? ? ? 6A ?", HOOK_REF_FORCE(render));
});