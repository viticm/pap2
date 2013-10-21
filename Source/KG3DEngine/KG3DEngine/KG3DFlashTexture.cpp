
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFlashTexture.cpp
//  Version     : 1.0
//  Creator     : Chiyer
//  Create Date : 2007-9-20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DFlashTexture.h"
#include ".\kg3denginemanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *flash_clsid_c;
//CComModule _Module;

HRESULT KG3DFlashTexture::LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
{
    if (!g_pd3dDevice || !LParam)
        return E_FAIL;

    int nWidth  = 512; 
    int nHeight = 512;

    if (WParam)
    {
        nWidth  = LOWORD(WParam);
        nHeight = HIWORD(WParam);
    }

    if (FAILED(Initialize(g_pd3dDevice, nWidth, nHeight)))
        return E_FAIL;

    m_hParentWnd = (HWND)LParam;
 
    AtlAxWinInit();

    if (m_pAxWindow == NULL)
        m_pAxWindow = new CAxWindow();

    if (m_pAxWindow->IsWindow())
        m_pAxWindow->DestroyWindow();

    RECT Rect = {0, 0, m_nWidth, m_nHeight};
    AdjustWindowRect(&Rect, WS_CHILD, FALSE);

    m_pAxWindow->Create(m_hParentWnd, &Rect, flash_clsid_c, WS_CHILD);
    m_pAxWindow->SetParent(m_hParentWnd);

    m_pAxWindow->QueryControl(&m_pIUnknow);

    if (!m_pIUnknow)
        return E_FAIL;

    m_pIUnknow->QueryInterface(GetIid(), (void **)&m_pShockwaveFlash);
    if (!m_pShockwaveFlash)
        return E_FAIL;

    unsigned short wName[256];
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)cszFileName , -1, (LPWSTR)wName, 256);

    if (m_pShockwaveFlash->LoadMovie(0,(BSTR)wName) != NOERROR)
        return E_FAIL;

    m_pShockwaveFlash->QueryInterface(IID_IViewObjectEx, (void**)&m_pView);
    if (!m_pView)
        return E_FAIL;


    HDC hDC = GetDC(GetDesktopWindow());
    m_CompatibleDc = CreateCompatibleDC(hDC);	

    BITMAPINFO Info;
    memset(&Info,0, sizeof(Info));
    Info.bmiHeader.biSize        = sizeof(Info);
    Info.bmiHeader.biPlanes      = 1;
    Info.bmiHeader.biBitCount    = 32;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biHeight      = m_nHeight;
    Info.bmiHeader.biWidth       = m_nWidth;

    m_hBitmap = CreateDIBSection(m_CompatibleDc, &Info, DIB_RGB_COLORS, &m_pFrameBuffer, NULL, 0);
    SelectObject(m_CompatibleDc, m_hBitmap);

    SetMapMode(m_CompatibleDc, MM_TEXT);

    m_pShockwaveFlash->put_BackgroundColor(0);

    Loop();
    Play();

    ReleaseDC(GetDesktopWindow(), hDC);

    return S_OK;

}

HRESULT KG3DFlashTexture::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
    HWND hWnd = NULL;
    extern KG3DEngineManager g_cEngineManager;
    g_cEngineManager.GetBaseWnd(&hWnd);
    return LoadFromFileEx(
        cszFileName,
        uFileNameHash,
        uOption,
        NULL,
        (DWORD_PTR)hWnd
        );
}

void KG3DFlashTexture::UpdateBuffer()
{
    DrawFrame();

    if (!m_pD3DTexture) 
    {
        m_pD3DDevice->CreateTexture(
            m_nWidth,
            m_nHeight,
            1,
            0,
            D3DFMT_A8R8G8B8,
            D3DPOOL_MANAGED,
            &m_pD3DTexture,
            0
            );
    }

    if (!m_pD3DTexture)
        return;

    D3DLOCKED_RECT LockRect;
    if (SUCCEEDED(m_pD3DTexture->LockRect(0, &LockRect, NULL, 0)))
    {
        memcpy(LockRect.pBits, m_pFrameBuffer, m_nWidth * m_nHeight * 4);
        m_pD3DTexture->UnlockRect(0);
    }

    return;
}


HRESULT KG3DFlashTexture::Initialize(LPDIRECT3DDEVICE9 pD3DDevice, int nWidth, int nHeight)
{
    m_pD3DDevice = pD3DDevice;

    IShockwaveFlash* pShockwaveFlash = NULL;

    CoInitialize(NULL);
    CoCreateInstance(GetClsid(), NULL, CLSCTX_INPROC_SERVER, GetIid(), (void**)&pShockwaveFlash);

    if (!pShockwaveFlash)
    {
        HMODULE hLib = LoadLibrary("Flash9b.ocx");

        if (!hLib)
            return E_FAIL;

        CoInitialize(NULL);

        void (__stdcall *lpDllEntryPoint)(void);
        (FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, _T("DllRegisterServer"));
        if (!lpDllEntryPoint)
        {
            FreeLibrary(hLib);
            CoUninitialize();
            return E_FAIL;
        }

        (*lpDllEntryPoint)();				
      
        FreeLibrary(hLib);
        CoUninitialize();

    }

    m_nHeight = nHeight;
    m_nWidth  = nWidth;

    if (pShockwaveFlash)
        pShockwaveFlash->Release();

    return S_OK;
}

KG3DFlashTexture::KG3DFlashTexture()
{
    m_pShockwaveFlash = NULL;
    m_pAxWindow       = NULL;
    m_pIUnknow        = NULL;
    m_pView           = NULL;
    m_pD3DTexture     = NULL;
    m_uRef            = 0;
    m_nWidth          = 0;
    m_nHeight         = 0;
    m_pBuffer         = 0;
    m_bPlaying        = false;
    m_pBuffer         = NULL;
    m_lLastFrame      = 0;
}

void RemoveWarningC4505(ATL::CAxHostWindow* pA)
{
	IID a;
	if(pA) 
	{
		pA->QueryInterface(a, NULL);
		pA->AddRef();
		pA->Release();
	}

}

KG3DFlashTexture::~KG3DFlashTexture()
{
    if (m_pShockwaveFlash)
        m_pShockwaveFlash->Release();
    m_pShockwaveFlash = NULL;

    if (m_pIUnknow)
        m_pIUnknow->Release();
    m_pIUnknow  = NULL;

    if (m_pView)
        m_pView->Release();
    m_pView = NULL;

    if (m_pAxWindow)
	{
		RemoveWarningC4505(NULL);
        delete m_pAxWindow;
	}

    if (m_pD3DTexture)
        m_pD3DTexture->Release();

    if (m_pBuffer)
        free(m_pBuffer);

    DeleteObject(m_hBitmap);
    DeleteDC(m_CompatibleDc);

}

long KG3DFlashTexture::DrawFrame()
{
    if (m_pShockwaveFlash == NULL)
        return -1;

    long lCurFrame = m_lLastFrame;
    long NumFrame;
    long Tatol;

    VARIANT_BOOL  bPlaying;

    m_pShockwaveFlash->CurrentFrame(&lCurFrame);

    m_pShockwaveFlash->get_Playing(&bPlaying);
    m_pShockwaveFlash->get_TotalFrames(&Tatol);
    m_pShockwaveFlash->get_FrameNum(&NumFrame);

    RECTL Rect = {0, 0, m_nWidth, m_nHeight};

    if (lCurFrame != m_lLastFrame)
    {
        m_lLastFrame = lCurFrame;
        m_pView->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, m_CompatibleDc, &Rect, &Rect, NULL, NULL); 
        return lCurFrame;
    }
    else if (lCurFrame == Tatol)
    {		
        VARIANT_BOOL bLoop;
        m_pShockwaveFlash->get_Loop(&bLoop);
        if (bLoop)
            m_pShockwaveFlash->GotoFrame(0);
    }
    else
    {
        m_pView->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, m_CompatibleDc, &Rect, &Rect, NULL, NULL); 
       /* VARIANT_BOOL bPlay;
        m_pShockwaveFlash->IsPlaying(&bPlay);

        if (m_bPlaying && !bPlay)
            m_pShockwaveFlash->Play();*/
    }

    return -1;

}

BYTE* KG3DFlashTexture::GetBitmapBuffer()
{
    if (!m_pFrameBuffer)
        return NULL;

    int nSize = 32 / 8 * m_nWidth * m_nHeight;
    int fhsize = sizeof(BITMAPFILEHEADER);
    int bisize = sizeof(BITMAPINFOHEADER);

    if (!m_pBuffer)
    {
        BITMAPFILEHEADER bmfh;
        BITMAPINFOHEADER info;

        memset ( &bmfh, 0, sizeof ( BITMAPFILEHEADER ) );
        memset ( &info, 0, sizeof ( BITMAPINFOHEADER ) );

        bmfh.bfType     = 0x4d42;      // 0x4d42 = 'BM'
        bmfh.bfReserved1 = 0;
        bmfh.bfReserved2 = 0;
        bmfh.bfSize      = sizeof(BITMAPFILEHEADER) +  sizeof(BITMAPINFOHEADER) + nSize;
        bmfh.bfOffBits   = 0x36;    // (54) size of headers

        info.biSize      = sizeof ( BITMAPINFOHEADER );
        info.biWidth     = m_nWidth;
        info.biHeight      = m_nHeight;
        info.biPlanes        = 1;
        info.biBitCount      = 32;
        info.biCompression   = BI_RGB;
        info.biSizeImage     = 0;
        info.biXPelsPerMeter = 0x0ec4;    
        info.biYPelsPerMeter = 0x0ec4;
        info.biClrUsed       = 0;
        info.biClrImportant  = 0;

        m_pBuffer = (BYTE*)malloc(fhsize + bisize + nSize);

        memcpy(m_pBuffer, &bmfh, fhsize);
        memcpy(m_pBuffer + fhsize, &info, bisize);
    }

    return m_pBuffer;
}

void KG3DFlashTexture::Play()
{
    if (m_pShockwaveFlash)
        m_pShockwaveFlash->Play();
    m_bPlaying = true;
}

void KG3DFlashTexture::Stop()
{
    if (m_pShockwaveFlash)
        m_pShockwaveFlash->Stop();
    m_bPlaying = false;
}

void KG3DFlashTexture::Loop()
{
    if (m_pShockwaveFlash)
        m_pShockwaveFlash->put_Loop(true);
}

void KG3DFlashTexture::Once()
{
    if (m_pShockwaveFlash)
        m_pShockwaveFlash->put_Loop(false);
}
