#pragma once
#include "EventManager.h"
#include "Utils.h"
#include "memory/HookManager.h"
#include "memory/Memory.h"

struct WindowEvent final : ICancellableEvent<"windowEvent"> {
    HWND hWnd;
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;

    WindowEvent(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
        : hWnd(hWnd), msg(msg), wParam(wParam), lParam(lParam) {}
};

inline int (__stdcall *wndproc_orig)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = nullptr;
inline int wndproc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) {
    WindowEvent evt { hWnd, msg, wParam, lParam };
    EventManager::Emit(evt);

    if (evt.IsCancelled()) 
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    
    return wndproc_orig(hWnd, msg, wParam, lParam);
}

inline EventManager::Ready $window_event_hook([] {
    HookManager::RegisterHook("55 8B EC 51 8B 45 0C 89 45 FC", HOOK_REF_FORCE(wndproc));
});