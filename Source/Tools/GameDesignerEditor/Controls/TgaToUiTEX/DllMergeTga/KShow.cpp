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
    m_nDesiredWidth = 256;
    m_nDesiredHeight = 256;
}

KShow::~KShow()
{
}

int KShow::Init()
{
    int nResult = false;
    int nRetCode = false;

    ASSERT(!ms_pSelf);
    ms_pSelf = new KShow;
    KG_PROCESS_ERROR(ms_pSelf);
    
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
    }
    KG_DELETE(ms_pSelf);
    return true;
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

