#ifndef WIN32_WINDOW_CPP
#define WIN32_WINDOW_CPP

#include <windows.h>

static void OnResize(void);

LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static HWND Win32CreateWindow()
{
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandleA(NULL);
    HMENU hMenu = NULL;
    char szExePath[MAX_PATH];
    GetModuleFileNameA(NULL, szExePath, MAX_PATH);
    HICON hIcon = ExtractIconA(hInstance, szExePath, 0);

    WNDCLASSA windowClass = {};
    windowClass.style = 0;
    windowClass.lpfnWndProc = StaticWindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = hIcon;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "WindowClassName";

    if (RegisterClassA(&windowClass))
    {
        // CreateWindow
        HWND hwnd = CreateWindowA(
            windowClass.lpszClassName,
            "Window Name",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            hMenu,
            hInstance,
            NULL);

        if (hwnd)
            return (hwnd);
    }
    return (NULL);
}

#endif