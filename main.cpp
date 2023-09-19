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
    HWND hwnd = Win32CreateWindow();
    if (hwnd)
    {
        if (InitD3D9SoftwareRenderer(hwnd))
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

                texture->LockRect(0, &lockedRect, NULL, 0);

                unsigned int *textureData = (unsigned int *)lockedRect.pBits;
                for (unsigned int i = 0; i < tWidth * tHeight; ++i)
                {
                    textureData[i] = 0x00111111;
                }

                LodeVRaycast(tWidth, tHeight, textureData, worldMap);

                texture->UnlockRect(0);
                device->SetTexture(0, texture);

                device->Clear(0, 0,
                              D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                              0x00000000, 1.0f, 0);

                device->BeginScene();
                device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));
                device->SetFVF(VertexFVF);
                device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
                device->EndScene();

                hr = device->Present(0, 0, 0, 0);
            }
            vertexBuffer->Release();
            device->Release();
        }
    }
    return (0);
}