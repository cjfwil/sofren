#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "d3d9.lib")

#include <stdio.h>
#include <math.h>

#include "win32_window.cpp"
#include "sofren_d3d9.cpp"

#include "raycast.cpp"

int main(void)
{
    HRESULT hr = NULL;
    HWND hwnd = Win32CreateWindow("D3D9 Software Renderer Library Demo (USE ARROW KEYS TO MOVE)");
    if (hwnd)
    {
        sofren_d3d9 sr = D3D9SofRenInit(hwnd, 320, 240);
        if (sr.success)
        {
            ShowWindow(hwnd, SW_SHOWDEFAULT);

            bool up = false;
            bool down = false;
            bool left = false;
            bool right = false;

            MSG msg;
            while (GetMessage(&msg, NULL, 0, 0))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }
                if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
                {
                    switch (msg.wParam)
                    {
                    case VK_UP:
                        up = (msg.message == WM_KEYDOWN);
                        break;
                    case VK_DOWN:
                        down = (msg.message == WM_KEYDOWN);
                        break;
                    case VK_LEFT:
                        left = (msg.message == WM_KEYDOWN);
                        break;
                    case VK_RIGHT:
                        right = (msg.message == WM_KEYDOWN);
                        break;

                    default:
                        break;
                    }
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                LodeVRaycastMove(up, down, left, right);

                sofren_screen_buffer screenBuffer = D3D9SofRenBegin(sr, 0x00111111);

                LodeVRaycastDemo(screenBuffer.width, screenBuffer.height, screenBuffer.data);

                D3D9SofRenEnd(sr);

                D3D9SofRenBlit(sr);
            }
            D3D9SofRenRelease(sr);
        }
    }
    return (0);
}