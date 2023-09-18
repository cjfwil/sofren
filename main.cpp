#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "d3d9.lib")

#include <stdio.h>
#include <math.h>

#include <windows.h>
#include <d3d9.h>
#include "win32_window.cpp"

int main(void)
{
    HRESULT hr = NULL;
    HWND hwnd = Win32CreateWindow();
    if (hwnd)
    {
        RECT rect = {};
        GetClientRect(hwnd, &rect);
        int clientWidth = rect.right - rect.left;
        int clientHeight = rect.bottom - rect.top;

        int tWidth = 320;
        int tHeight = 240;

        IDirect3D9 *d3d = Direct3DCreate9(D3D_SDK_VERSION);
        IDirect3DDevice9 *device = NULL;

        D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
        D3DCAPS9 caps;
        d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
        int vertexproc = NULL;
        if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
            vertexproc = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        else
            vertexproc = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

        D3DPRESENT_PARAMETERS d3dpp = {};
        d3dpp.BackBufferWidth = clientWidth;
        d3dpp.BackBufferHeight = clientHeight;
        d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
        d3dpp.BackBufferCount = 1;
        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.MultiSampleQuality = NULL;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = hwnd;
        d3dpp.Windowed = TRUE;
        d3dpp.EnableAutoDepthStencil = true;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
        d3dpp.Flags = NULL;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        if (FAILED(hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, vertexproc, &d3dpp, &device)))
        {
            MessageBoxA(NULL, "Could not Create d3d9 device.", "Error", MB_ICONERROR);
            return (1);
        }
        d3d->Release();

        struct Vertex
        {
            float x, y, z;
            float u, v;
        };
        Vertex *vertices;

        const DWORD VertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;
        IDirect3DVertexBuffer9 *vertexBuffer = NULL;

        if (vertexBuffer != NULL)
        {
            vertexBuffer->Release();
        }

        if (FAILED(device->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, VertexFVF, D3DPOOL_MANAGED, &vertexBuffer, NULL)))
        {
            MessageBoxA(NULL, "Could not Create d3d9 vertex buffer.", "Error", MB_ICONERROR);
            return (1);
        }
        float xMult = ((float)clientHeight / (float)clientWidth) * (tWidth / (float)tHeight);
        vertexBuffer->Lock(0, 0, (void **)&vertices, 0);
        vertices[0].x = -1.0f * xMult;
        vertices[0].y = 1.0f;
        vertices[0].z = 0.0f;
        vertices[0].u = 0.0f;
        vertices[0].v = 0.0f;

        vertices[1].x = 1.0f * xMult;
        vertices[1].y = 1.0f;
        vertices[1].z = 0.0f;
        vertices[1].u = 1.0f;
        vertices[1].v = 0.0f;

        vertices[2].x = -1.0f * xMult;
        vertices[2].y = -1.0f;
        vertices[2].z = 0.0f;
        vertices[2].u = 0.0f;
        vertices[2].v = 1.0f;

        vertices[3].x = 1.0f * xMult;
        vertices[3].y = -1.0f;
        vertices[3].z = 0.0f;
        vertices[3].u = 1.0f;
        vertices[3].v = 1.0f;
        vertexBuffer->Unlock();

        IDirect3DTexture9 *texture;
        hr = device->CreateTexture(tWidth, tHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, NULL);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, "Could not Create d3d9 texture.", "Error", MB_ICONERROR);
            return (1);
        }
        D3DLOCKED_RECT lockedRect;
        texture->LockRect(0, &lockedRect, NULL, 0);

        unsigned int *textureData = (unsigned int *)lockedRect.pBits;
        for (unsigned int x = 0; x < tWidth; ++x)
        {
            for (unsigned int y = 0; y < tHeight; ++y)
            {
                unsigned char r = x ^ y;
                textureData[x + y * tWidth] = 0xff000000 | (r << 16);
            }
        }
        texture->UnlockRect(0);
        device->SetTexture(0, texture);

        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        device->SetRenderState(D3DRS_LIGHTING, false);

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

            texture->LockRect(0, &lockedRect, NULL, 0);

            unsigned int *textureData = (unsigned int *)lockedRect.pBits;
            for (unsigned int i = 0; i < tWidth * tHeight; ++i)
            {
                textureData[i] = 0x00111111;
            }
            // lodeV raycast implementation
            int worldMap[24][24] =
                {
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

            static double posX = 22, posY = 12;      // x and y start position
            static double dirX = -1, dirY = 0;       // initial direction vector
            static double planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane
            double moveSpeed = 0.016667f * 5.0;     // the constant value is in squares/second
            double rotSpeed = 0.016667f * 3.0;      // the constant value is in radians/second

            if (up)
            {
                if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false)
                    posX += dirX * moveSpeed;
                if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false)
                    posY += dirY * moveSpeed;
            }
            if (down)
            {
                if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false)
                    posX -= dirX * moveSpeed;
                if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false)
                    posY -= dirY * moveSpeed;
            }
            if (right)
            {                   
                // both camera direction and camera plane must be rotated
                double oldDirX = dirX;
                dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
                dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
                planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
            }
            if (left)
            {
                // both camera direction and camera plane must be rotated
                double oldDirX = dirX;
                dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
                dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
                planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
            }

            for (int x = 0; x < tWidth; x++)
            {
                // calculate ray position and direction
                double cameraX = 2 * x / double(tWidth) - 1; // x-coordinate in camera space
                double rayDirX = dirX + planeX * cameraX;
                double rayDirY = dirY + planeY * cameraX;

                // which box of the map we're in
                int mapX = int(posX);
                int mapY = int(posY);

                // length of ray from current position to next x or y-side
                double sideDistX;
                double sideDistY;

                // length of ray from one x or y-side to next x or y-side
                double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
                double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
                double perpWallDist;

                // what direction to step in x or y-direction (either +1 or -1)
                int stepX;
                int stepY;

                int hit = 0; // was there a wall hit?
                int side;    // was a NS or a EW wall hit?

                // calculate step and initial sideDist
                if (rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (posX - mapX) * deltaDistX;
                }
                else
                {
                    stepX = 1;
                    sideDistX = (mapX + 1.0 - posX) * deltaDistX;
                }
                if (rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (posY - mapY) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapY + 1.0 - posY) * deltaDistY;
                }

                // perform DDA
                while (hit == 0)
                {
                    // jump to next map square, either in x-direction, or in y-direction
                    if (sideDistX < sideDistY)
                    {
                        sideDistX += deltaDistX;
                        mapX += stepX;
                        side = 0;
                    }
                    else
                    {
                        sideDistY += deltaDistY;
                        mapY += stepY;
                        side = 1;
                    }
                    // Check if ray has hit a wall
                    if (worldMap[mapX][mapY] > 0)
                        hit = 1;
                }
                // Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
                if (side == 0)
                    perpWallDist = (sideDistX - deltaDistX);
                else
                    perpWallDist = (sideDistY - deltaDistY);

                // Calculate height of line to draw on screen
                int lineHeight = (int)(tHeight / perpWallDist);

                // calculate lowest and highest pixel to fill in current stripe
                int drawStart = -lineHeight / 2 + tHeight / 2;
                if (drawStart < 0)
                    drawStart = 0;
                int drawEnd = lineHeight / 2 + tHeight / 2;
                if (drawEnd >= tHeight)
                    drawEnd = tHeight - 1;

                // choose wall color
                unsigned int color;
                switch (worldMap[mapX][mapY])
                {
                case 1:
                    color = 0xffff0000;
                    break; // red
                case 2:
                    color = 0xff00ff00;
                    break; // green
                case 3:
                    color = 0xff0000ff;
                    break; // blue
                case 4:
                    color = 0xffffffff;
                    break; // white
                default:
                    color = 0xffffff00;
                    break; // yellow
                }

                // give x and y sides different brightness
                if (side == 1)
                {              
                    unsigned char r = ((color & 0x00ff0000) >> 16)/2;
                    unsigned char g = ((color & 0x0000ff00) >> 8)/2;
                    unsigned char b = (color & 0x000000ff)/2;      

                    color = r << 16 | g << 8 | b;
                }

                // draw the pixels of the stripe as a vertical line
                for (int y = 0; y < drawEnd - drawStart; ++y)
                {
                    int newY = y + (tHeight - (drawEnd - drawStart)) / 2;
                    textureData[x + newY * tWidth] = color;
                }                
            }
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
    return (0);
}