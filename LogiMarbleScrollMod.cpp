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
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

int sendAmount = 0;
LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* hookStruct = (MSLLHOOKSTRUCT*)lParam;

    // Ignore injected events
    if (code < 0 || (hookStruct->flags & (LLMHF_INJECTED | LLKHF_INJECTED | LLMHF_LOWER_IL_INJECTED | LLKHF_LOWER_IL_INJECTED))) {
        return CallNextHookEx(gHook, code, wParam, lParam);
    }

    if (code == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_MOUSEMOVE: {
            int dy = hookStruct->pt.y - scrollingCursorPos.y;

            if (scrollingActive)
            {
                if (sendAmount > 80)
                    break;
                INPUT input;
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_WHEEL;
                input.mi.time = NULL; //Windows will do the timestamp
                input.mi.mouseData = (DWORD)dy; //A positive value indicates that the wheel was rotated forward, away from the user; a negative value indicates that the wheel was rotated backward, toward the user. One wheel click is defined as WHEEL_DELTA, which is 120.
                input.mi.dx = scrollingCursorPos.x;
                input.mi.dy = scrollingCursorPos.y;
                input.mi.dwExtraInfo = GetMessageExtraInfo();
                sendAmount ++;

                auto result = SendInput(1, &input, sizeof(INPUT));
                return 1;
            }
            
            break;
        }
        case WM_RBUTTONDOWN: {
            scrollingActive = true;
            scrollingCursorPos = hookStruct->pt;
            sendAmount = 0;
            return 1;
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