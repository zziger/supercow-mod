﻿#pragma once
#include "EventManager.h"
#include "Utils.h"
#include "memory/HookManager.h"
#include "memory/Memory.h"
#include "sdk/Game.h"
#include <spdlog/spdlog.h>

struct StartExecutionEvent final : IEvent<"startExecution", StartExecutionEvent> {};

HOOK_FN_CONV(inline int, can_start_game, ARGS(const char* lpName), __cdecl) {
    spdlog::trace("StartExecution event");
    EventManager::Emit(StartExecutionEvent{});
    return 1;
}

inline void hook_start_execution() {
    HookManager::RegisterHook("55 8B EC 51 8B 45 ? 50 6A ? 6A ? FF 15", HOOK_REF(can_start_game));
}
