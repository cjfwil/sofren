#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")

#include <windows.h>
#include "win32_window.cpp"

int main(void)
{
    HWND hwnd = Win32CreateWindow();
    ShowWindow(hwnd, SW_SHOWDEFAULT);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_QUIT)
        {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (0);
}