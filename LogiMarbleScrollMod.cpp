#include <iostream>
#include <vector>
#include <Windows.h>

#define SCROLLMOUSEDOWN 4
#define SCROLLMOUSEUP 8

LRESULT CALLBACK targetWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool scrollingActive = false;

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = targetWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("SmoothMouseScroll");

    if (!RegisterClass(&wc))
        return -1;

    HWND targetWindow = CreateWindowEx(0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!targetWindow)
        return -1;

    //register the monitoring device
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01; //Mouse
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK;
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

    /// <summary>
    ///  Actually scroll!
    /// </summary>
    /// <returns></returns>
    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 1 * 120, 0);

    return 0;
}

LRESULT CALLBACK targetWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // print out the values that I need
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
                        scrollingActive = true; break;
                    case SCROLLMOUSEUP:
                        scrollingActive = false; break;
                    }

                    if (scrollingActive) {
                        //std::cout << raw->data.mouse.lLastY << std::endl;
                        mouse_event(MOUSEEVENTF_WHEEL, 0, 0, raw->data.mouse.lLastY, 0);
                        return 1;
                    }
                }
            }
        }
        return 0;
    }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}