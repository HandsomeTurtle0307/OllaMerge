/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "windoweffects.h"

#include <QWidget>

#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace {

// Mirrors the undocumented WINDOWCOMPOSITIONATTRIBDATA / ACCENT_POLICY structs
// used by user32's SetWindowCompositionAttribute. The API is loaded at runtime,
// so no dwmapi/user32 import library or newer SDK header is required.
struct AccentPolicy
{
    int state;
    int flags;
    DWORD gradientColor;
    DWORD animationId;
};

struct CompositionAttribData
{
    int attribute;
    PVOID data;
    SIZE_T size;
};

using SetWindowCompositionAttributeFn = BOOL(WINAPI *)(HWND, CompositionAttribData *);
using DwmSetWindowAttributeFn = HRESULT(WINAPI *)(HWND, DWORD, LPCVOID, DWORD);

SetWindowCompositionAttributeFn setWindowCompositionAttribute()
{
    static const auto function = []() -> SetWindowCompositionAttributeFn {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32) {
            return nullptr;
        }
        return reinterpret_cast<SetWindowCompositionAttributeFn>(
            GetProcAddress(user32, "SetWindowCompositionAttribute"));
    }();
    return function;
}

DwmSetWindowAttributeFn dwmSetWindowAttribute()
{
    static const auto function = []() -> DwmSetWindowAttributeFn {
        HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
        if (!dwmapi) {
            return nullptr;
        }
        return reinterpret_cast<DwmSetWindowAttributeFn>(
            GetProcAddress(dwmapi, "DwmSetWindowAttribute"));
    }();
    return function;
}

void applyAcrylicTint(HWND hwnd, bool dark)
{
    const auto setter = setWindowCompositionAttribute();
    if (!setter) {
        return;
    }

    AccentPolicy policy = {};
    policy.state = 4; // ACCENT_ENABLE_ACRYLICBLURBEHIND
    policy.flags = 0;
    // AABBGGRR: a subtle theme-colored tint on top of the blur.
    policy.gradientColor = dark ? 0x55271811u : 0x55FBF5F3u;

    CompositionAttribData data = {};
    data.attribute = 19; // WCA_ACCENT_POLICY
    data.data = &policy;
    data.size = sizeof(policy);

    setter(hwnd, &data);
}

} // namespace
#endif // Q_OS_WIN

void WindowEffects::apply(QWidget *window, bool dark)
{
#ifdef Q_OS_WIN
    if (!window) {
        return;
    }

    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    if (!hwnd) {
        return;
    }

    if (const auto dwm = dwmSetWindowAttribute()) {
        BOOL darkTitleBar = dark ? TRUE : FALSE;
        // Windows 10 2004+ uses attribute 20; earlier 1809 builds use 19.
        if (FAILED(dwm(hwnd, 20, &darkTitleBar, sizeof(darkTitleBar)))) {
            dwm(hwnd, 19, &darkTitleBar, sizeof(darkTitleBar));
        }

        const int cornerPreference = 2; // DWMWCP_ROUND
        dwm(hwnd, 33, &cornerPreference, sizeof(cornerPreference));
    }

    applyAcrylicTint(hwnd, dark);
#else
    Q_UNUSED(window);
    Q_UNUSED(dark);
#endif
}
