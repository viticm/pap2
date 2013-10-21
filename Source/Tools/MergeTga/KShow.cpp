////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KShow.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-21 10:25:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KShow.h"
#include "UiTex.h"
#include "KPickBox.h"
#include "KTgaManager.h"

////////////////////////////////////////////////////////////////////////////////

KShow *KShow::ms_pSelf = NULL;

KShow::KShow()
{
    m_pBitMap = NULL;
    m_pbyData = NULL;
    m_pPen = NULL;
    m_nDrawDataHasBeenChged = true;
    m_nShowStartX = 0;
    m_nShowStartY = 0;
    m_nDesiredWidth = 512;
    m_nDesiredHeight = 512;
	m_pD3D = NULL;
	m_p3DDevice = NULL;
}

KShow::~KShow()
{
}

int KShow::Init(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;

	KG_PROCESS_ERROR(hWnd);

    ASSERT(!ms_pSelf);
    ms_pSelf = new KShow;
    KG_PROCESS_ERROR(ms_pSelf);

	nRetCode = ms_pSelf->InitDirect3D(hWnd);
	KG_PROCESS_ERROR(nRetCode);
    
    nRetCode = ms_pSelf->CreateBuffer();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = ms_pSelf->CreatePen();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = ms_pSelf->CreateBitMap();
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(ms_pSelf);
    return nResult;
}

int KShow::Exit()
{
    if (ms_pSelf)
    {
        ms_pSelf->DeletePen();
        ms_pSelf->DeleteBitMap();
        ms_pSelf->DeleteBuffer();
		ms_pSelf->UnInitDirect3D();
    }

    KG_DELETE(ms_pSelf);
    return true;
}

int KShow::InitDirect3D(HWND hWnd)
{
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3D)
		return FALSE;
	D3DPRESENT_PARAMETERS PresentParam;
	memset(&PresentParam, 0, sizeof(D3DPRESENT_PARAMETERS));

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = ::GetSystemMetrics(SM_CXSCREEN);
	d3dpp.BackBufferHeight = ::GetSystemMetrics(SM_CYSCREEN);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// Create the D3DDevice
	HRESULT hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_p3DDevice);
	if (FAILED(hr))
	{
		m_pD3D->Release();
		m_pD3D = NULL;
		return FALSE;
	}
	return TRUE;
}

LPDIRECT3DDEVICE9 KShow::GetDevice()
{
	return m_p3DDevice;
}

int	KShow::UnInitDirect3D()
{
	if (m_p3DDevice)
	{
		m_p3DDevice->Release();
		m_p3DDevice = NULL;
	}

	if (m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	return TRUE;
}


KShow &KShow::GetSelf()
{
    ASSERT(ms_pSelf);
    return *ms_pSelf;
}

int KShow::CreateBuffer()
{
    int nResult = false;

    ASSERT(!m_pbyData);
    m_pbyData = new BYTE[SHOW_BUFFER_WIDTH * SHOW_BUFFER_HEIGHT * 4];
    KG_PROCESS_ERROR(m_pbyData);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(m_pbyData);
    return nResult;
}

int KShow::DeleteBuffer()
{
    KG_DELETE(m_pbyData);
    return true;
}

int KShow::Clear()
{
    int nResult = false;
    KG_PROCESS_ERROR(m_pbyData);

    memset(m_pbyData, 0, SHOW_BUFFER_WIDTH * SHOW_BUFFER_HEIGHT * 4);
    m_aRect.clear();
    m_nDrawDataHasBeenChged = true;

    nResult = true;
Exit0:
    return nResult;
}

int KShow::AppendBuffer(KTgaBlock *pBlock)
{
    int nResult = false;
    int i = 0; 
    int j = 0;
    int nLeft = 0;
    int nTop = 0;
    int nWidth = 0;
    int nHeight = 0;
    BYTE *pbyDest = NULL;
    BYTE *pbySource = NULL;
    KG_PROCESS_ERROR(pBlock);
    KG_PROCESS_ERROR(m_pbyData);

    nLeft = pBlock->GetLeft();
    nTop = pBlock->GetTop();
    nWidth = pBlock->GetWidth();
    nHeight = pBlock->GetHeight();
    pbySource = pBlock->GetBufferPtr();
    KG_PROCESS_ERROR(pbySource); 

    //KG_PROCESS_ERROR(nLeft >= 0 && nLeft + nWidth <= SHOW_BUFFER_WIDTH);
    //KG_PROCESS_ERROR(nLeft >= 0 && nTop + nHeight <= SHOW_BUFFER_HEIGHT);

    for (i = 0; i < nHeight; ++i) 
    {
        pbyDest = m_pbyData + ((i + nTop) * SHOW_BUFFER_WIDTH + nLeft) * 4;
        memcpy(pbyDest, pbySource, nWidth * 4);
        pbySource += (nWidth * 4);
    }

    m_nDrawDataHasBeenChged = true;

    nResult = true;
Exit0:
    return nResult;
}

int KShow::AppendBuffer(KTgaBlock *pBlock, RECT &Rect)
{
	int nResult = false;
	int i = 0; 
	int j = 0;
	int nLeft = 0;
	int nTop = 0;
	int nWidth = 0;
	int nHeight = 0;
	BYTE *pbyDest = NULL;
	BYTE *pbySource = NULL;
	KG_PROCESS_ERROR(pBlock);
	KG_PROCESS_ERROR(m_pbyData);

	nLeft = Rect.left;
	nTop = Rect.top;
	nWidth = Rect.right;
	nHeight = Rect.bottom;
	pbySource = pBlock->GetBufferPtr();
	KG_PROCESS_ERROR(pbySource); 

	KG_PROCESS_ERROR(nLeft + nWidth <= SHOW_BUFFER_WIDTH);
	KG_PROCESS_ERROR(nTop + nHeight <= SHOW_BUFFER_HEIGHT);

	for (i = 0; i < nHeight; ++i) 
	{
		pbyDest = m_pbyData + ((i + nTop) * SHOW_BUFFER_WIDTH + nLeft) * 4;
		memcpy(pbyDest, pbySource, nWidth * 4);
		pbySource += (nWidth * 4);
	}

	m_nDrawDataHasBeenChged = true;

	nResult = true;
Exit0:
	return nResult;
}

int KShow::AppendShowRect(RECT &Rect)
{
    m_nDrawDataHasBeenChged = true;
    m_aRect.push_back(Rect);
    return true;
}

int KShow::CreateBitMap()
{
    int nResult = false;
    BITMAPINFO bmpInfo;
    KTgaBlock *pBlock = NULL;

    DeleteBitMap();

    KG_PROCESS_ERROR(m_pbyData);

    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biClrImportant = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biCompression = 0;
    bmpInfo.bmiHeader.biHeight = 0 - SHOW_BUFFER_HEIGHT;   
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biSizeImage = SHOW_BUFFER_HEIGHT * SHOW_BUFFER_WIDTH * sizeof(DWORD);
    bmpInfo.bmiHeader.biWidth = SHOW_BUFFER_WIDTH;
    bmpInfo.bmiHeader.biXPelsPerMeter = 2834;
    bmpInfo.bmiHeader.biYPelsPerMeter = 2834;
    bmpInfo.bmiColors[0].rgbBlue = 255;
    bmpInfo.bmiColors[0].rgbGreen = 255;
    bmpInfo.bmiColors[0].rgbRed = 255;
    bmpInfo.bmiColors[0].rgbReserved = 0;
    m_pBitMap = Gdiplus::Bitmap::FromBITMAPINFO(&bmpInfo, m_pbyData);
    KG_PROCESS_ERROR(m_pBitMap);

    m_nDrawDataHasBeenChged = true;
    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(m_pBitMap);
    return nResult;
}

int KShow::DeleteBitMap()
{
    KG_DELETE(m_pBitMap);
    return true;
}

int KShow::CreatePen()
{
    int nResult = false;

    DeletePen();
    m_pPen = new Gdiplus::Pen(Gdiplus::Color(0, 255, 0), 1);
    KG_PROCESS_ERROR(m_pPen);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(m_pPen);
    return nResult;
}

int KShow::DeletePen()
{
    KG_DELETE(m_pPen);
    return true;
}

int KShow::SetNeedReDraw()
{
    m_nDrawDataHasBeenChged = true;
    return true;
}

int KShow::ClearNeedReDraw()
{
    m_nDrawDataHasBeenChged = false;
    return true;
}

int KShow::PaintWindow(HWND hWnd, Gdiplus::Graphics *pGraphics)
{
    int nResult = false;
    int x = 0;
    int y = 0;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pGraphics);
    KG_PROCESS_ERROR(m_pBitMap);
    KG_PROCESS_ERROR(m_pPen);

    x = -KShow::GetSelf().GetShowStartX();
    y = -KShow::GetSelf().GetShowStartY();

    if (m_nDrawDataHasBeenChged)
    {
		pGraphics->Clear(Gdiplus::Color(0, 0, 0));
        pGraphics->DrawImage(m_pBitMap, x, y); 
        m_nDrawDataHasBeenChged = false;
    }
    else
    {
        RECT rcRestor;
        KPickBox::GetSelf().GetLastDrawRect(rcRestor);
        //pGraphics
        pGraphics->DrawImage(
            m_pBitMap, rcRestor.left + x, rcRestor.top + y, 
            rcRestor.left, rcRestor.top, rcRestor.right, rcRestor.bottom, Gdiplus::UnitPixel
        );
    }

    for (KShowRectArray::iterator it = m_aRect.begin(); it != m_aRect.end(); ++it) 
    {
        RECT &DrawRect = (*it);
        pGraphics->DrawRectangle(m_pPen, DrawRect.left + x, DrawRect.top + y, DrawRect.right - 1, DrawRect.bottom - 1);
    }

    m_pPen->SetColor(Gdiplus::Color(0, 0, 255));
    pGraphics->DrawRectangle(m_pPen, x, y, m_nDesiredWidth, m_nDesiredHeight);
    m_pPen->SetColor(Gdiplus::Color(0, 255, 0));

    nResult = true;
Exit0:
    return nResult;
}

int KShow::PaintWindow(HWND hWnd)
{
	RECT rcClient;
	D3DVIEWPORT9 Viewport;
	RECT rcRect;
	KG_PROCESS_ERROR(m_p3DDevice);
	KG_PROCESS_ERROR(hWnd);

	::GetClientRect(hWnd, &rcClient);

	m_p3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	Viewport.X = 0;
	Viewport.Y = 0;
	Viewport.Width = rcClient.right - rcClient.left;
	Viewport.Height = rcClient.bottom - rcClient.top;
	Viewport.MinZ = 0.0f;
	Viewport.MaxZ = 1.0f;
	m_p3DDevice->SetViewport(&Viewport);
	KG_COM_PROCESS_ERROR(m_p3DDevice->BeginScene());

	DrawDesiredRect();

	KTgaManager::GetSelf().DrawImage();
	KTgaManager::GetSelf().DrawLine(0xFF0000FF);

	KTgaManager::GetSelf().DrawImage(KPickBox::GetSelf().GetPickBlockIndex());
	KTgaManager::GetSelf().DrawLine(KPickBox::GetSelf().GetPickBlockIndex(), 0xFFFF0000);

	KPickBox::GetSelf().PaintWindow(hWnd);

	m_p3DDevice->EndScene();
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = rcClient.right - rcClient.left;
	rcRect.bottom = rcClient.bottom - rcClient.top;
	m_p3DDevice->Present(&rcRect, &rcRect, NULL, NULL);

	return TRUE;
Exit0:
	return FALSE;
}

int KShow::DrawDesiredRect()
{
	struct _VS
	{
		D3DXVECTOR4 p;
		DWORD dwDiffuse;
	};

	LPDIRECT3DVERTEXBUFFER9 pVS = NULL;
	HRESULT hr = E_FAIL;
	int nDeltaX = 0;
	int nDeltaY = 0;
	KTgaBlock * pBlock = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	_VS* data = NULL;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	KG_PROCESS_ERROR(pDevice);

	nDeltaX = GetShowStartX();
	nDeltaY = GetShowStartY();

	hr = pDevice->CreateVertexBuffer(8 * sizeof(_VS), 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &pVS, NULL);
	KG_COM_PROCESS_ERROR(hr);

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, NULL);

	if (SUCCEEDED(pVS->Lock(0, 0, (void**)&data, 0)))
	{
		data[0].p = D3DXVECTOR4(- nDeltaX, - nDeltaY, 0.f, 1.f);
		data[0].dwDiffuse = 0xFF00FF00;
		data[1].p = D3DXVECTOR4(data[0].p.x + m_nDesiredWidth, data[0].p.y, 0.f, 1.f);
		data[1].dwDiffuse = 0xFF00FF00;

		data[2].p = data[1].p;
		data[2].dwDiffuse = 0xFF00FF00;
		data[3].p = D3DXVECTOR4(data[1].p.x, data[0].p.y + m_nDesiredHeight, 0.f, 1.f);
		data[3].dwDiffuse = 0xFF00FF00;

		data[4].p = data[3].p;
		data[4].dwDiffuse = 0xFF00FF00;
		data[5].p = D3DXVECTOR4(data[0].p.x, data[3].p.y, 0.f, 1.f);
		data[5].dwDiffuse = 0xFF00FF00;

		data[6].p = data[5].p;
		data[6].dwDiffuse = 0xFF00FF00;
		data[7].p = data[0].p;
		data[7].dwDiffuse = 0xFF00FF00;

		pVS->Unlock();

		pDevice->SetStreamSource(0, pVS, 0, sizeof(_VS));
		pDevice->DrawPrimitive(D3DPT_LINELIST, 0, 4);
	}

Exit0:
	KG_COM_RELEASE(pVS);
	return true;
}

int KShow::GetShowStartX()
{
    return m_nShowStartX;
}

int KShow::GetShowStartY()
{
    return m_nShowStartY;
}

int KShow::SetShowStartX(int x)
{
    m_nShowStartX = x;
    return true;
}

int KShow::SetShowStartY(int y)
{
    m_nShowStartY = y;
    return true;
}

int KShow::SetDesiredWidth(int nWidth)
{
    m_nDesiredWidth = nWidth;
    return true;
}

int KShow::SetDesiredHeight(int nHeight)
{
    m_nDesiredHeight = nHeight;
    return true;
}

int KShow::GetDesiredWidth()
{
    return m_nDesiredWidth;
}

int KShow::GetDesiredHeight()
{
    return m_nDesiredHeight;
}

