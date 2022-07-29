#include <iostream>
#include <vector>
#include <Windows.h>

#define SCROLLMOUSEDOWN 4
#define SCROLLMOUSEUP 8

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam);

HHOOK gHook;
bool scrollingActive = false;
POINT scrollingCursorPos;

int main()
{
    MSG msg;
    gHook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc,
        GetModuleHandle(0), 0);
    while (1)
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_MOUSEMOVE: break;
        case WM_RBUTTONDOWN: return 1;
        case WM_RBUTTONUP: return 1;
        default:
            break;
        }


    }

    return CallNextHookEx(gHook, code, wParam, lParam);
}