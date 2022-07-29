#include <iostream>
#include <vector>
#include <Windows.h>

#define SCROLLMOUSEDOWN 4
#define SCROLLMOUSEUP 8

LRESULT CALLBACK targetWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool scrollingActive = false;
POINT scrollingCursorPos;

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = targetWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("LogiMarbleScrollMod");

    if (!RegisterClass(&wc))
        return -1;

    HWND targetWindow = CreateWindowEx(0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!targetWindow)
        return -1;

    //register the monitoring device
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01; //Mouse
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK | RIDEV_NOLEGACY | RIDEV_CAPTUREMOUSE | RIDEV_EXINPUTSINK;
    rid.hwndTarget = targetWindow;

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
        return -1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(targetWindow);

    return 0;
}

LRESULT CALLBACK targetWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INPUT: {
        UINT dataSize;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first
        if (dataSize > 0)
        {
            std::vector<BYTE> rawdata(dataSize);

            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
            {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.data());
                if (raw->header.dwType == RIM_TYPEMOUSE)
                {

                    switch (raw->data.mouse.ulButtons)
                    {
                    case SCROLLMOUSEDOWN:
                        scrollingActive = true;
                        GetCursorPos(&scrollingCursorPos);
                        break;
                    case SCROLLMOUSEUP:
                        scrollingActive = false; break;
                    }

                    if (scrollingActive) {
                        mouse_event(MOUSEEVENTF_WHEEL, 0, 0, raw->data.mouse.lLastY, 0);
                        SetCursorPos(scrollingCursorPos.x, scrollingCursorPos.y);
                        return 0;
                    }
                }
            }
        }
    }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}