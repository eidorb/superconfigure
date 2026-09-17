/*
 * Sokol Windows backend calls user32/gdi32/shell32 symbols that cosmocc
 * 4.0.2 does not IAT-import (they landed on cosmopolitan master after
 * the 4.0.2 tag). Resolve from host DLLs on first use.
 * Never called on Linux; returns 0 if the pointer is missing.
 */
#include <cosmo.h>
#include <libc/dce.h>
#include <libc/nt/dll.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __x86_64__
#define MSABI __attribute__((ms_abi))
#else
#define MSABI
#endif

static void *winapi(const char *dll, const char *name) {
    if (!IsWindows()) {
        return 0;
    }
    intptr_t h = GetModuleHandle(dll);
    if (!h) {
        return 0;
    }
    void *p = GetProcAddress(h, name);
    if (!p) {
        char wname[128];
        snprintf(wname, sizeof(wname), "%sW", name);
        p = GetProcAddress(h, wname);
    }
    return p;
}

intptr_t MonitorFromWindow(intptr_t hwnd, uint32_t flags) {
    static MSABI intptr_t (*fn)(intptr_t, uint32_t);
    if (!fn)
        fn = (MSABI intptr_t(*)(intptr_t, uint32_t))winapi("user32.dll", "MonitorFromWindow");
    return fn ? fn(hwnd, flags) : 0;
}
intptr_t MonitorFromPoint(int64_t pt, uint32_t flags) {
    static MSABI intptr_t (*fn)(int64_t, uint32_t);
    if (!fn)
        fn = (MSABI intptr_t(*)(int64_t, uint32_t))winapi("user32.dll", "MonitorFromPoint");
    return fn ? fn(pt, flags) : 0;
}
int32_t GetMonitorInfo(intptr_t mon, void *info) {
    static MSABI int32_t (*fn)(intptr_t, void *);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t, void *))winapi("user32.dll", "GetMonitorInfoW");
    return fn ? fn(mon, info) : 0;
}
int32_t AdjustWindowRectEx(void *rc, uint32_t style, int32_t menu, uint32_t ex) {
    static MSABI int32_t (*fn)(void *, uint32_t, int32_t, uint32_t);
    if (!fn)
        fn = (MSABI int32_t(*)(void *, uint32_t, int32_t, uint32_t))winapi(
            "user32.dll", "AdjustWindowRectEx");
    return fn ? fn(rc, style, menu, ex) : 0;
}
intptr_t SetWindowLongPtr(intptr_t hwnd, int32_t idx, intptr_t val) {
    static MSABI intptr_t (*fn)(intptr_t, int32_t, intptr_t);
    if (!fn)
        fn = (MSABI intptr_t(*)(intptr_t, int32_t, intptr_t))winapi("user32.dll",
                                                                   "SetWindowLongPtrW");
    return fn ? fn(hwnd, idx, val) : 0;
}
intptr_t WindowFromPoint(int64_t pt) {
    static MSABI intptr_t (*fn)(int64_t);
    if (!fn)
        fn = (MSABI intptr_t(*)(int64_t))winapi("user32.dll", "WindowFromPoint");
    return fn ? fn(pt) : 0;
}
int32_t ClientToScreen(intptr_t hwnd, void *pt) {
    static MSABI int32_t (*fn)(intptr_t, void *);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t, void *))winapi("user32.dll", "ClientToScreen");
    return fn ? fn(hwnd, pt) : 0;
}
int32_t ScreenToClient(intptr_t hwnd, void *pt) {
    static MSABI int32_t (*fn)(intptr_t, void *);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t, void *))winapi("user32.dll", "ScreenToClient");
    return fn ? fn(hwnd, pt) : 0;
}
int32_t PtInRect(const void *rc, int64_t pt) {
    static MSABI int32_t (*fn)(const void *, int64_t);
    if (!fn)
        fn = (MSABI int32_t(*)(const void *, int64_t))winapi("user32.dll", "PtInRect");
    return fn ? fn(rc, pt) : 0;
}
int32_t ClipCursor(const void *rc) {
    static MSABI int32_t (*fn)(const void *);
    if (!fn)
        fn = (MSABI int32_t(*)(const void *))winapi("user32.dll", "ClipCursor");
    return fn ? fn(rc) : 0;
}
int32_t SetCursorPos(int32_t x, int32_t y) {
    static MSABI int32_t (*fn)(int32_t, int32_t);
    if (!fn)
        fn = (MSABI int32_t(*)(int32_t, int32_t))winapi("user32.dll", "SetCursorPos");
    return fn ? fn(x, y) : 0;
}
int32_t GetSystemMetrics(int32_t n) {
    static MSABI int32_t (*fn)(int32_t);
    if (!fn)
        fn = (MSABI int32_t(*)(int32_t))winapi("user32.dll", "GetSystemMetrics");
    return fn ? fn(n) : 0;
}
int16_t GetAsyncKeyState(int32_t vk) {
    static MSABI int16_t (*fn)(int32_t);
    if (!fn)
        fn = (MSABI int16_t(*)(int32_t))winapi("user32.dll", "GetAsyncKeyState");
    return fn ? fn(vk) : 0;
}
int32_t TrackMouseEvent(void *tme) {
    static MSABI int32_t (*fn)(void *);
    if (!fn)
        fn = (MSABI int32_t(*)(void *))winapi("user32.dll", "TrackMouseEvent");
    return fn ? fn(tme) : 0;
}
int32_t RegisterRawInputDevices(void *devs, uint32_t n, uint32_t cb) {
    static MSABI int32_t (*fn)(void *, uint32_t, uint32_t);
    if (!fn)
        fn = (MSABI int32_t(*)(void *, uint32_t, uint32_t))winapi(
            "user32.dll", "RegisterRawInputDevices");
    return fn ? fn(devs, n, cb) : 0;
}
uint32_t GetRawInputData(intptr_t hraw, uint32_t cmd, void *data, uint32_t *size, uint32_t hsz) {
    static MSABI uint32_t (*fn)(intptr_t, uint32_t, void *, uint32_t *, uint32_t);
    if (!fn)
        fn = (MSABI uint32_t(*)(intptr_t, uint32_t, void *, uint32_t *, uint32_t))winapi(
            "user32.dll", "GetRawInputData");
    return fn ? fn(hraw, cmd, data, size, hsz) : 0;
}
int32_t PostMessage(intptr_t hwnd, uint32_t msg, uintptr_t wp, intptr_t lp) {
    static MSABI int32_t (*fn)(intptr_t, uint32_t, uintptr_t, intptr_t);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t, uint32_t, uintptr_t, intptr_t))winapi(
            "user32.dll", "PostMessageW");
    return fn ? fn(hwnd, msg, wp, lp) : 0;
}
int32_t UnregisterClass(const void *name, intptr_t inst) {
    static MSABI int32_t (*fn)(const void *, intptr_t);
    if (!fn)
        fn = (MSABI int32_t(*)(const void *, intptr_t))winapi("user32.dll",
                                                             "UnregisterClassW");
    return fn ? fn(name, inst) : 0;
}
int32_t OpenClipboard(intptr_t hwnd) {
    static MSABI int32_t (*fn)(intptr_t);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t))winapi("user32.dll", "OpenClipboard");
    return fn ? fn(hwnd) : 0;
}
int32_t CloseClipboard(void) {
    static MSABI int32_t (*fn)(void);
    if (!fn)
        fn = (MSABI int32_t(*)(void))winapi("user32.dll", "CloseClipboard");
    return fn ? fn() : 0;
}
int32_t EmptyClipboard(void) {
    static MSABI int32_t (*fn)(void);
    if (!fn)
        fn = (MSABI int32_t(*)(void))winapi("user32.dll", "EmptyClipboard");
    return fn ? fn() : 0;
}
intptr_t SetClipboardData(uint32_t fmt, intptr_t mem) {
    static MSABI intptr_t (*fn)(uint32_t, intptr_t);
    if (!fn)
        fn = (MSABI intptr_t(*)(uint32_t, intptr_t))winapi("user32.dll", "SetClipboardData");
    return fn ? fn(fmt, mem) : 0;
}
intptr_t GetClipboardData(uint32_t fmt) {
    static MSABI intptr_t (*fn)(uint32_t);
    if (!fn)
        fn = (MSABI intptr_t(*)(uint32_t))winapi("user32.dll", "GetClipboardData");
    return fn ? fn(fmt) : 0;
}

int32_t DescribePixelFormat(intptr_t hdc, int32_t ipfd, uint32_t n, void *pfd) {
    static MSABI int32_t (*fn)(intptr_t, int32_t, uint32_t, void *);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t, int32_t, uint32_t, void *))winapi(
            "gdi32.dll", "DescribePixelFormat");
    return fn ? fn(hdc, ipfd, n, pfd) : 0;
}

void *GlobalLock(intptr_t mem) {
    static MSABI void *(*fn)(intptr_t);
    if (!fn)
        fn = (MSABI void *(*)(intptr_t))winapi("kernel32.dll", "GlobalLock");
    return fn ? fn(mem) : 0;
}
int32_t GlobalUnlock(intptr_t mem) {
    static MSABI int32_t (*fn)(intptr_t);
    if (!fn)
        fn = (MSABI int32_t(*)(intptr_t))winapi("kernel32.dll", "GlobalUnlock");
    return fn ? fn(mem) : 0;
}

uint32_t DragQueryFile(intptr_t drop, uint32_t i, void *buf, uint32_t len) {
    static MSABI uint32_t (*fn)(intptr_t, uint32_t, void *, uint32_t);
    if (!fn)
        fn = (MSABI uint32_t(*)(intptr_t, uint32_t, void *, uint32_t))winapi(
            "shell32.dll", "DragQueryFileW");
    return fn ? fn(drop, i, buf, len) : 0;
}
void DragFinish(intptr_t drop) {
    static MSABI void (*fn)(intptr_t);
    if (!fn)
        fn = (MSABI void (*)(intptr_t))winapi("shell32.dll", "DragFinish");
    if (fn)
        fn(drop);
}
void DragAcceptFiles(intptr_t hwnd, int32_t accept) {
    static MSABI void (*fn)(intptr_t, int32_t);
    if (!fn)
        fn = (MSABI void (*)(intptr_t, int32_t))winapi("shell32.dll", "DragAcceptFiles");
    if (fn)
        fn(hwnd, accept);
}
void *CommandLineToArgv(const void *cmd, int *argc) {
    static MSABI void *(*fn)(const void *, int *);
    if (!fn)
        fn = (MSABI void *(*)(const void *, int *))winapi("shell32.dll",
                                                         "CommandLineToArgvW");
    return fn ? fn(cmd, argc) : 0;
}
