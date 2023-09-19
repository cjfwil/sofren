#ifndef SOFREN_D3D9_CPP
#define SOFREN_D3D9_CPP

#include <windows.h>
#include <d3d9.h>

#define SOFREN_DEFAULT_INTERNAL_BUFFER_WIDTH 320
#define SOFREN_DEFAULT_INTERNAL_BUFFER_HEIGHT 240

struct sofren_d3d9
{
    int internalBufferWidth;
    int internalBufferHeight;
    IDirect3DTexture9 *internalBufferTexture;

    D3DLOCKED_RECT lockedRect;
    IDirect3DDevice9 *device = NULL;
    IDirect3DVertexBuffer9 *screenVertexBuffer = NULL;
    const DWORD screenVertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;

    bool success = false;
};

struct sofren_vertex
{
    float x, y, z;
    float u, v;
};

struct sofren_screen_buffer
{
    int width;
    int height;
    unsigned int *data;
};

static sofren_screen_buffer D3D9SofRenBegin(sofren_d3d9 sr, unsigned int clearColour)
{
    sofren_screen_buffer result = {};
    result.width = sr.internalBufferWidth;
    result.height = sr.internalBufferHeight;
    sr.internalBufferTexture->LockRect(0, &sr.lockedRect, NULL, 0);
    result.data = (unsigned int *)sr.lockedRect.pBits;
    for (unsigned int i = 0; i < result.width * result.height; ++i)
    {
        result.data[i] = 0x00111111;
    }
    return (result);
}

static void D3D9SofRenEnd(sofren_d3d9 sr)
{
    sr.internalBufferTexture->UnlockRect(0);
}

static void D3D9SofRenBlit(sofren_d3d9 sr)
{
    sr.device->SetTexture(0, sr.internalBufferTexture);

    sr.device->Clear(0, 0,
                     D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                     0x00000000, 1.0f, 0);

    sr.device->BeginScene();
    sr.device->SetStreamSource(0, sr.screenVertexBuffer, 0, sizeof(sofren_vertex));
    sr.device->SetFVF(sr.screenVertexFVF);
    sr.device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    sr.device->EndScene();

    sr.device->Present(0, 0, 0, 0);
}

// todo: error checking for all of this
static sofren_d3d9 D3D9SofRenInit(HWND hwnd, int internalBufferWidth=SOFREN_DEFAULT_INTERNAL_BUFFER_WIDTH, int internalBufferHeight=SOFREN_DEFAULT_INTERNAL_BUFFER_HEIGHT)
{
    sofren_d3d9 sr = {};
    sr.internalBufferWidth = internalBufferWidth;
    sr.internalBufferHeight = internalBufferHeight;
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

    if (!FAILED(hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, vertexproc, &d3dpp, &sr.device)))
    {
        d3d->Release();

        sofren_vertex *vertices;

        if (sr.screenVertexBuffer != NULL)
        {
            sr.screenVertexBuffer->Release();
        }

        if (!FAILED(sr.device->CreateVertexBuffer(4 * sizeof(sofren_vertex), D3DUSAGE_WRITEONLY, sr.screenVertexFVF, D3DPOOL_MANAGED, &sr.screenVertexBuffer, NULL)))
        {
            float xMult = 1.0f;
            float yMult = 1.0f;
            if (clientHeight <= clientWidth) {
                xMult = ((float)clientHeight / (float)clientWidth) * (sr.internalBufferWidth / (float)sr.internalBufferHeight);
            } else {
                yMult = ((float)clientWidth / (float)clientHeight) * (sr.internalBufferHeight / (float)sr.internalBufferWidth);
            }
            hr = sr.screenVertexBuffer->Lock(0, 0, (void **)&vertices, 0);
            vertices[0].x = -1.0f * xMult;
            vertices[0].y = 1.0f * yMult;
            vertices[0].z = 0.0f;
            vertices[0].u = 0.0f;
            vertices[0].v = 0.0f;

            vertices[1].x = 1.0f * xMult;
            vertices[1].y = 1.0f * yMult;
            vertices[1].z = 0.0f;
            vertices[1].u = 1.0f;
            vertices[1].v = 0.0f;

            vertices[2].x = -1.0f * xMult;
            vertices[2].y = -1.0f * yMult;
            vertices[2].z = 0.0f;
            vertices[2].u = 0.0f;
            vertices[2].v = 1.0f;

            vertices[3].x = 1.0f * xMult;
            vertices[3].y = -1.0f * yMult;
            vertices[3].z = 0.0f;
            vertices[3].u = 1.0f;
            vertices[3].v = 1.0f;
            hr = sr.screenVertexBuffer->Unlock();

            hr = sr.device->CreateTexture(sr.internalBufferWidth, sr.internalBufferHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &sr.internalBufferTexture, NULL);
            if (!FAILED(hr))
            {

                hr = sr.device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
                hr = sr.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                hr = sr.device->SetRenderState(D3DRS_LIGHTING, false);
                sr.success = true;
            }
            else
            {
                MessageBoxA(NULL, "Could not Create d3d9 texture.", "Error", MB_ICONERROR);
            }
        }
        else
        {
            MessageBoxA(NULL, "Could not Create d3d9 vertex buffer.", "Error", MB_ICONERROR);
        }
    }
    else
    {
        MessageBoxA(NULL, "Could not Create d3d9 device.", "Error", MB_ICONERROR);
    }
    return (sr);
}

static void D3D9SofRenRelease(sofren_d3d9 sr)
{
    sr.screenVertexBuffer->Release();
    sr.device->Release();
    sr.internalBufferTexture->Release();
}

#endif