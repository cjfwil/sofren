#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "d3d9.lib")

#include <stdio.h>

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
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

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
        d3dpp.BackBufferWidth = width;
        d3dpp.BackBufferHeight = height;
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

        // float vertices[] = {
        //     0.0f, 0.5f, 0.0f, 1.0f,
        //     0.5f, -0.5f, 0.0f, 1.0f,
        //     -0.5f, -0.5f, 0.0f, 1.0f};
        struct Vertex
        {
            float x, y, z;
        };
        Vertex *vertices;
        // float vertices[9] = {};

        const DWORD VertexFVF = D3DFVF_XYZ;
        IDirect3DVertexBuffer9 *vertexBuffer;
        if (FAILED(device->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, VertexFVF, D3DPOOL_MANAGED, &vertexBuffer, NULL)))
        {
            MessageBoxA(NULL, "Could not Create d3d9 vertex buffer.", "Error", MB_ICONERROR);
            return (1);
        }
        vertexBuffer->Lock(0, 0, (void **)&vertices, 0);
        // memcpy(vertices, vertices, sizeof(vertices));
        vertices[0].x = -0.5f;
        vertices[0].y = 0.5f;
        vertices[0].z = 0.0f;

        vertices[1].x = 0.5f;
        vertices[1].y = 0.5f;
        vertices[1].z = 0.0f;

        vertices[2].x = -0.5f;
        vertices[2].y = -0.5f;
        vertices[2].z = 0.0f;

        vertices[3].x = 0.5f;
        vertices[3].y = -0.5f;
        vertices[3].z = 0.0f;

        vertexBuffer->Unlock();

        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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

            device->Clear(0, 0,
                          D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                          0x00000077, 1.0f, 0);

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