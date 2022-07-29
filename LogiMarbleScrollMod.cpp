#include <iostream>
#include <vector>
#include <Windows.h>
#include <windowsx.h>

#define SCROLLMOUSEDOWN 4
#define SCROLLMOUSEUP 8

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam);

HHOOK gHook;
bool scrollingActive = false;
POINT scrollingCursorPos;
POINT lastCursorPos;

int reportInterval = 0;

int main()
{
    MSG msg;
    gHook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc, GetModuleHandle(0), 0);
    std::cout << "Running!" << std::endl;
    while (1)
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(10);
    }

    return 0;
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* hookStruct = (MSLLHOOKSTRUCT*)lParam;

    if (code == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_MOUSEMOVE: {
            short dy = hookStruct->pt.y - scrollingCursorPos.y;

            if (scrollingActive)
            {
                hookStruct->mouseData = dy << 16;
                std::cout << (short)(hookStruct->mouseData >> 16) << std::endl;
                CallNextHookEx(gHook, code, WM_MOUSEWHEEL, (LPARAM)hookStruct);
                return 1;
            }
            
            break;
        }
        case WM_RBUTTONDOWN: {
            scrollingActive = true;
            scrollingCursorPos = hookStruct->pt;
            return 1;
        }
        case WM_MOUSEWHEEL: {
            std::cout << (short)(hookStruct->mouseData >> 16) << std::endl;
            break;
        }
        case WM_RBUTTONUP: {
            scrollingActive = false;
            return 1;
        }
        default:
            break;
        }


    }

    return CallNextHookEx(gHook, code, wParam, lParam);
}