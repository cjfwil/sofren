#ifndef SOFREN_D3D9_CPP
#define SOFREN_D3D9_CPP

#include <windows.h>
#include <d3d9.h>

IDirect3DTexture9 *texture;
D3DLOCKED_RECT lockedRect;
IDirect3DDevice9 *device = NULL;
IDirect3DVertexBuffer9 *vertexBuffer = NULL;
const DWORD VertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;

int tWidth = 320;
int tHeight = 240;

struct Vertex
{
    float x, y, z;
    float u, v;
};

// todo: error checking for all of this
static bool InitD3D9SoftwareRenderer(HWND hwnd)
{    
    HRESULT hr = S_OK;
    RECT rect = {};
    GetClientRect(hwnd, &rect);
    int clientWidth = rect.right - rect.left;
    int clientHeight = rect.bottom - rect.top;

    IDirect3D9 *d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
    D3DCAPS9 caps;
    hr = d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
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
        return (false);
    }
    d3d->Release();

    Vertex *vertices;

    if (vertexBuffer != NULL)
    {
        vertexBuffer->Release();
    }

    if (FAILED(device->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, VertexFVF, D3DPOOL_MANAGED, &vertexBuffer, NULL)))
    {
        MessageBoxA(NULL, "Could not Create d3d9 vertex buffer.", "Error", MB_ICONERROR);
        return (false);
    }
    float xMult = ((float)clientHeight / (float)clientWidth) * (tWidth / (float)tHeight);
    hr = vertexBuffer->Lock(0, 0, (void **)&vertices, 0);
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
    hr = vertexBuffer->Unlock();

    hr = device->CreateTexture(tWidth, tHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, NULL);
    if (FAILED(hr))
    {
        MessageBoxA(NULL, "Could not Create d3d9 texture.", "Error", MB_ICONERROR);
        return (false);
    }

    hr = texture->LockRect(0, &lockedRect, NULL, 0);

    unsigned int *textureData = (unsigned int *)lockedRect.pBits;
    for (unsigned int x = 0; x < tWidth; ++x)
    {
        for (unsigned int y = 0; y < tHeight; ++y)
        {
            unsigned char r = x ^ y;
            textureData[x + y * tWidth] = 0xff000000 | (r << 16);
        }
    }

    hr = texture->UnlockRect(0);
    hr = device->SetTexture(0, texture);

    hr = device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    hr = device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    hr = device->SetRenderState(D3DRS_LIGHTING, false);
    return (true);
}

#endif