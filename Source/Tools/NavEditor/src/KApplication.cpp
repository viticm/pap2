#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include <dxerr.h>
#include "KApplication.h"
#include "WinProc.h"
#include "KCamera.h"
#include "KMainPlayer.h"
#include "KScene.h"
#include "XScene.h"

using namespace std;

#pragma warning(disable: 4996)

//MessageBox(NULL, DXGetErrorString(hrCode), "error", MB_OK | MB_ICONINFORMATION);


KApplication::KApplication(void)
{
    m_hInstance             = NULL;
    m_hWnd                  = NULL;
    m_dwStartTime           = 0;
    m_dwFrame               = 0;
    m_piDevice              = NULL;
    m_nPreMouseX            = 0;
    m_nPreMouseY            = 0;
}

KApplication::~KApplication(void)
{
}

BOOL KApplication::Setup(HINSTANCE hInstance, int nCmdShow)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    m_hInstance = hInstance;

    bRetCode = SetupWindow(nCmdShow);
    KG_PROCESS_ERROR(bRetCode);

    m_dwStartTime = GetTickCount();
    srand(m_dwStartTime);    

    m_bRunFlag = true;

    bResult = true;
Exit0:
    return bResult;
}

void KApplication::Clear()
{
    KG_COM_RELEASE(m_piDevice);
}

void KApplication::Run()
{
    while (m_bRunFlag)
    {
        ProcessMessage();

        FrameUpdate();
    }
}

BOOL KApplication::SetupWindow(int nCmdShow)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    ATOM        WinClassID      = 0;
    HWND        hWnd            = NULL;
    TCHAR       szTitle[]       = TEXT("Grid view");			
    TCHAR       szWindowClass[] = TEXT("GridViewMainWindowr");		
	WNDCLASSEX  wcex;

	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	WinClassID = RegisterClassEx(&wcex);
    KG_PROCESS_ERROR(WinClassID);   

    hWnd = CreateWindow(
        szWindowClass, szTitle, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, 
        m_hInstance, NULL
    );
    KG_PROCESS_ERROR(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    m_hWnd = hWnd;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KApplication::ResetDevice()
{
    BOOL                    bResult     = false;
    IDirect3D9*             piD3D9      = NULL;
    HRESULT                 hRetCode    = E_FAIL;
    D3DPRESENT_PARAMETERS   param;

    KG_PROCESS_ERROR(m_hWnd);

    param.BackBufferWidth               = 0;
    param.BackBufferHeight              = 0;
    param.BackBufferFormat              = D3DFMT_A8R8G8B8;
    param.BackBufferCount               = 1;
    param.MultiSampleType               = D3DMULTISAMPLE_4_SAMPLES;
    param.MultiSampleQuality            = 0;
    param.SwapEffect                    = D3DSWAPEFFECT_DISCARD;
    param.hDeviceWindow                 = m_hWnd;
    param.Windowed                      = true;
    param.EnableAutoDepthStencil        = true;
    param.AutoDepthStencilFormat        = D3DFMT_D24S8;
    param.Flags                         = 0;
    param.FullScreen_RefreshRateInHz    = D3DPRESENT_RATE_DEFAULT;
    param.PresentationInterval          = D3DPRESENT_INTERVAL_DEFAULT;

    if (!m_piDevice)
    {
        piD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
        KG_PROCESS_ERROR(piD3D9);

        hRetCode = piD3D9->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &param, &m_piDevice
        );
        KG_PROCESS_ERROR(!FAILED(hRetCode));
    }
    else
    {
        hRetCode = m_piDevice->Reset(&param);
        KG_PROCESS_ERROR(!FAILED(hRetCode));
    }

    bResult = true;
Exit0:
    if (!bResult && FAILED(hRetCode) && hRetCode != E_FAIL)
    {
        MessageBox(NULL, DXGetErrorString(hRetCode), "error", MB_OK | MB_ICONINFORMATION);
    }
    KG_COM_RELEASE(piD3D9);
    return bResult;
}

void KApplication::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    int nX = GET_X_LPARAM(lParam);
    int nY = GET_Y_LPARAM(lParam);

    if (wParam & MK_RBUTTON && m_nPreMouseX > 0 && m_nPreMouseY > 0)
    {
        g_Camera.OnPitchingGrag((nY - m_nPreMouseY));
        g_Camera.OnRotatingGrag((nX - m_nPreMouseX));
    }

    m_nPreMouseX = nX;
    m_nPreMouseY = nY;
}

void KApplication::ProcessMessage()
{
    BOOL bRetCode = false;
    MSG  msg;

    while (true)
    {
        bRetCode = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
        if (!bRetCode)
            return;

        if (msg.message == WM_QUIT)
        {
            m_bRunFlag = false;
            return;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void KApplication::FrameUpdate()
{
    BOOL        bRetCode    = false;
    HRESULT     hRetCode    = E_FAIL;
    DWORD       dwTime      = GetTickCount();
    D3DXVECTOR3 vHeight(0.0f, 2.0f, 0.0f);
    D3DXVECTOR3 vLookAt;
    D3DXVECTOR3 vEye;
    D3DXMATRIX  Matrix;
    KDrawInfo   DrawInfo;
    RECT        rect;
    KDrawList   DrawList;

    if ((dwTime - m_dwStartTime) * GAME_FPS < 1000 * m_dwFrame)
    {
        Sleep(10);
        return;
    }

    if (m_piDevice == NULL)
    {
        ResetDevice();
        return;
    }

    hRetCode = m_piDevice->TestCooperativeLevel();
    if (hRetCode != D3D_OK)
    {
        if (hRetCode == D3DERR_DEVICENOTRESET)
            ResetDevice();

        return;
    }

    m_dwFrame++;

    g_Player.FrameUpdate();
    g_Camera.FrameUpdate();

    vLookAt = g_Player.m_vPosition + vHeight;
    vEye    = vLookAt - g_Camera.m_vLook;
    
    D3DXMatrixLookAtLH(&Matrix, &vEye, &vLookAt, &g_Camera.m_vUp);
    m_piDevice->SetTransform(D3DTS_VIEW, &Matrix);

    GetClientRect(m_hWnd, &rect);

    D3DXMatrixPerspectiveFovLH(
        &Matrix, D3DX_PI * 0.35f, (float)rect.right / (float)rect.bottom,
        1.0f, 1000.0f
    );

    m_piDevice->SetTransform(D3DTS_PROJECTION, &Matrix);
    m_piDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_piDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    m_piDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(50, 50, 50), 1.0f, 0);

    m_piDevice->BeginScene();

    SetLight();

    g_Scene.Draw(m_piDevice);
    g_Player.Draw(m_piDevice);

    m_piDevice->EndScene();

    m_piDevice->Present(NULL, NULL, NULL, NULL);

    if (m_dwFrame % 50 == 0)
    {
        char szMsg[50];

        dwTime = GetTickCount() - dwTime;

        sprintf(szMsg, "Frame cost: %lu", dwTime);

        SetWindowText(m_hWnd, szMsg);
    }
}

BOOL KApplication::Draw(const KDrawInfo& crDrawInfo)
{
    BOOL                    bResult         = false;
    HRESULT                 hRetCode        = E_FAIL;
    IDirect3DVertexBuffer9* piVertexBuffer  = NULL;
    IDirect3DIndexBuffer9*  piIndexBuffer   = NULL;
    void*                   pvBuffer        = NULL;
    D3DXMATRIX              Matrix;

    D3DXMatrixTranslation(&Matrix, crDrawInfo.vOffset.x, crDrawInfo.vOffset.y, crDrawInfo.vOffset.z);
    m_piDevice->SetTransform(D3DTS_WORLD, &Matrix);

    hRetCode = m_piDevice->CreateVertexBuffer(
        crDrawInfo.nVertexSize * crDrawInfo.nVertexCount, D3DUSAGE_DYNAMIC, 
        crDrawInfo.dwVertexFVF, D3DPOOL_DEFAULT, &piVertexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    hRetCode = piVertexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    memcpy(pvBuffer, crDrawInfo.pvVetexData, crDrawInfo.nVertexSize * crDrawInfo.nVertexCount);
    piVertexBuffer->Unlock();

    hRetCode = m_piDevice->CreateIndexBuffer(
        sizeof(int) * 3 * crDrawInfo.nTriangleCount, D3DUSAGE_DYNAMIC, 
        D3DFMT_INDEX32, D3DPOOL_DEFAULT, &piIndexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    hRetCode = piIndexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(hRetCode));
    
    memcpy(pvBuffer, crDrawInfo.pvTriangleData, sizeof(int) * 3 * crDrawInfo.nTriangleCount);
    piIndexBuffer->Unlock();

    m_piDevice->SetStreamSource(0, piVertexBuffer, 0, crDrawInfo.nVertexSize);
    m_piDevice->SetIndices(piIndexBuffer);
    m_piDevice->SetFVF(crDrawInfo.dwVertexFVF);
    m_piDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, crDrawInfo.nVertexCount, 0, crDrawInfo.nTriangleCount);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIndexBuffer);
    KG_COM_RELEASE(piVertexBuffer);
    return bResult;
}

BOOL KApplication::SetLight()
{
    D3DCOLORVALUE           mDif    = { 0.6f, 0.6f, 0.6f, 1.0f };
    D3DCOLORVALUE           mAmb    = { 0.3f, 0.3f, 0.3f, 1.0f };
    D3DCOLORVALUE           mSpe    = { 0.4f, 0.4f, 0.4f, 1.0f };
    D3DCOLORVALUE           mEmi    = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3DCOLORVALUE           lDif    = { 0.6f, 0.6f, 0.6f, 1.0f };
    D3DCOLORVALUE           lAmb    = { 0.6f, 0.6f, 0.6f, 1.0f };
    D3DCOLORVALUE           lSpe    = { 0.0f, 0.0f, 0.0f, 0.0f };
    D3DXVECTOR3             vPos(60.0f, 20.0f, 60.0f);
    D3DXVECTOR3             vDir = -vPos;
    D3DLIGHT9               light;
    D3DMATERIAL9            material;

    ZeroMemory(&light, sizeof(light));

    D3DXVec3Normalize(&vDir, &vDir);

    light.Type          = D3DLIGHT_DIRECTIONAL;
    light.Diffuse       = lDif;
    light.Ambient       = lAmb;
    light.Specular      = lSpe;
    light.Position      = vPos;
    light.Direction     = vDir;
    light.Attenuation0  = 0.8f;
    light.Attenuation1  = 0.0f;
    light.Attenuation2  = 0.0f;
    light.Range         = 1000.0f;

    m_piDevice->SetLight(0, &light);
    m_piDevice->LightEnable(0, true);
    m_piDevice->SetRenderState(D3DRS_LIGHTING, true);
    m_piDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));

    material.Diffuse    = mDif;
    material.Ambient    = mAmb;
    material.Specular   = mSpe;
    material.Emissive   = mEmi;
    material.Power      = 0.3f;

    m_piDevice->SetMaterial(&material);

    return true;
}
