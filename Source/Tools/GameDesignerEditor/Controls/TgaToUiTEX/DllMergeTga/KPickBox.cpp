////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KPickBox.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-20 17:07:07
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <atlconv.h>

#include "KPickBox.h"
#include "UiTex.h"
#include "KTgaManager.h"
#include "KShow.h"


////////////////////////////////////////////////////////////////////////////////

KPickBox *KPickBox::ms_pSelf = NULL;

KPickBox::KPickBox()
{
    m_nTgaBlockIndex = -1;
    m_pPen = NULL;
    m_pBitMap = NULL;
}

KPickBox::~KPickBox()
{
}

int KPickBox::Init()
{
    int nResult = false;
    int nRetCode = false;

    ASSERT(!ms_pSelf);
    ms_pSelf = new KPickBox;
    KG_PROCESS_ERROR(ms_pSelf);
    nRetCode = ms_pSelf->CreatePen();
    KG_PROCESS_ERROR(nRetCode);
    

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(ms_pSelf);
    return nResult;
}

int KPickBox::Exit()
{
    if (ms_pSelf)
    {
        ms_pSelf->DeleteBitMap();
        ms_pSelf->DeletePen();
    }
    KG_DELETE(ms_pSelf);
    return true;
}

KPickBox &KPickBox::GetSelf()
{
    ASSERT(ms_pSelf);
    return *ms_pSelf;
}

int KPickBox::Clear()
{
    m_nTgaBlockIndex = -1;
    DeleteBitMap();
    return true;
}

int KPickBox::Pick(int nIndex)
{
    int nResult = false;
    int nRetCode = false;
    KTgaBlock *pBlock = NULL;

    m_nTgaBlockIndex = nIndex;

    KG_DELETE(m_pBitMap);
    nRetCode = CreateBitMap();
    KG_PROCESS_ERROR(nRetCode);

    pBlock = KTgaManager::GetSelf().Get(m_nTgaBlockIndex);
    KG_PROCESS_ERROR(pBlock);
    pBlock->ClearPosHasBeenSeted();

    //KTgaManager::GetSelf().UpDataListView();
    KTgaManager::GetSelf().UpDataShow();

    nResult = true;
Exit0:
    if (!nResult)
        m_nTgaBlockIndex = -1;
    return true;
}

int KPickBox::Drop()
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KTgaManager::GetSelf().TryDropBlock(m_nTgaBlockIndex);
    KG_PROCESS_ERROR(nRetCode);

    KTgaManager::GetSelf().UpDataListView();
    KTgaManager::GetSelf().UpDataShow();

    DeleteBitMap();
    m_nTgaBlockIndex = -1;

    nResult = true;
Exit0:
    return nResult;
}

int KPickBox::GetPickBlockIndex()
{
    return m_nTgaBlockIndex;
}

int KPickBox::CreateBitMap()
{
    int nResult = false;
    BITMAPINFO bmpInfo;
    KTgaBlock *pBlock = NULL;

    pBlock = KTgaManager::GetSelf().Get(m_nTgaBlockIndex);
    KG_PROCESS_ERROR(pBlock);

    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biClrImportant = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biCompression = 0;
    bmpInfo.bmiHeader.biHeight = 0 - pBlock->GetHeight();   
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biSizeImage = pBlock->GetHeight() * pBlock->GetWidth() * sizeof(DWORD);
    bmpInfo.bmiHeader.biWidth = pBlock->GetWidth();
    bmpInfo.bmiHeader.biXPelsPerMeter = 2834;
    bmpInfo.bmiHeader.biYPelsPerMeter = 2834;
    bmpInfo.bmiColors[0].rgbBlue = 255;
    bmpInfo.bmiColors[0].rgbGreen = 255;
    bmpInfo.bmiColors[0].rgbRed = 255;
    bmpInfo.bmiColors[0].rgbReserved = 0;
    m_pBitMap = Gdiplus::Bitmap::FromBITMAPINFO(&bmpInfo, pBlock->GetBufferPtr());
    KG_PROCESS_ERROR(m_pBitMap);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(m_pBitMap);
    return nResult;
}

int KPickBox::DeleteBitMap()
{
    KG_DELETE(m_pBitMap);
    return true;
}

int KPickBox::CreatePen()
{
    int nResult = false;

    DeletePen();
    m_pPen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0), 1);
    KG_PROCESS_ERROR(m_pPen);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(m_pPen);
    return nResult;
}

int KPickBox::DeletePen()
{
    KG_DELETE(m_pPen);
    return true;
}

int KPickBox::CalculatePickBlockPos(HWND hWnd)
{
    int nResult = false;
    POINT msPos;
    KTgaBlock *pBlock = NULL;
    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(m_nTgaBlockIndex >= 0);

    ::GetCursorPos(&msPos);
    ::ScreenToClient(hWnd, &msPos);
    msPos.x += KShow::GetSelf().GetShowStartX();
    msPos.y += KShow::GetSelf().GetShowStartY();

    pBlock = KTgaManager::GetSelf().Get(m_nTgaBlockIndex);
    if (pBlock)
    {
        msPos.x -= (pBlock->GetWidth() / 2);
        msPos.y -= (pBlock->GetHeight() / 2);
    }
    KTgaManager::GetSelf().CalculateBlockPos(m_nTgaBlockIndex, msPos);

    nResult = true;
Exit0:
    return nResult;
}

int KPickBox::PaintWindow(HWND hWnd, Gdiplus::Graphics *pGraphics)
{
    int nResult = false;
    int x = -KShow::GetSelf().GetShowStartX();
    int y = -KShow::GetSelf().GetShowStartY();
    KTgaBlock *pBlock = NULL;

    m_rcLastDrawRect.left = 0;
    m_rcLastDrawRect.top = 0;
    m_rcLastDrawRect.right = 0;
    m_rcLastDrawRect.bottom = 0;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pGraphics);
    KG_PROCESS_ERROR(m_pBitMap);
    KG_PROCESS_ERROR(m_pPen);

    pBlock = KTgaManager::GetSelf().Get(m_nTgaBlockIndex);
    KG_PROCESS_ERROR(pBlock);

    m_rcLastDrawRect.left = pBlock->GetLeft();
    m_rcLastDrawRect.top = pBlock->GetTop();
    m_rcLastDrawRect.right = pBlock->GetWidth();
    m_rcLastDrawRect.bottom = pBlock->GetHeight();

    pGraphics->DrawImage(m_pBitMap, m_rcLastDrawRect.left + x, m_rcLastDrawRect.top + y); 
    pGraphics->DrawRectangle(
        m_pPen, m_rcLastDrawRect.left + x, m_rcLastDrawRect.top + y, 
        m_rcLastDrawRect.right - 1, m_rcLastDrawRect.bottom - 1
    );

    nResult = true;
Exit0:
	//绘制提示信息
	{
		POINT msPos;
		::GetCursorPos(&msPos);
		::ScreenToClient(hWnd, &msPos);
		int nIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
			msPos.x + KShow::GetSelf().GetShowStartX(), 
			msPos.y + KShow::GetSelf().GetShowStartY()
			);
		pBlock = KTgaManager::GetSelf().Get(nIndex);
		if (pBlock)
		{
			TCHAR szTip[128];
			szTip[0] = _T('\0');
			int nOrgIndex = pBlock->GetOriginalIndex();
			if (nOrgIndex < 0)
				nOrgIndex = nIndex;
			LPCTSTR pcszName = pBlock->GetName();
			LPCTSTR pcszClip = NULL;
			if (pcszName)
				pcszClip = _tcsrchr(pcszName, _T('\\'));
			if (pcszClip)
				pcszName = pcszClip + 1;
			if (pcszName)
				pcszClip = _tcsrchr(pcszName, _T('/'));
			if (pcszClip)
				pcszName = pcszClip + 1;
			if (!pcszName)
				pcszName = _T("无名字");
			_sntprintf(szTip, sizeof(szTip) / sizeof(szTip[0]), "%s%s\n%s%d", 
				"名字：", pcszName,
				"序号：", nOrgIndex
				);

			RECT rc;
			rc.left = msPos.x + 20;
			rc.top = msPos.y + 20;
			rc.right = rc.left + 300;
			rc.bottom = rc.top + 200;
			HDC hDc = GetDC(hWnd);
			DrawText(hDc, szTip, (int)_tcslen(szTip), &rc, NULL);
			ReleaseDC(hWnd, hDc);

			if (m_rcLastDrawRect.left == m_rcLastDrawRect.right && 
				m_rcLastDrawRect.top == m_rcLastDrawRect.bottom)
			{
				m_rcLastDrawRect.left = rc.left;
				m_rcLastDrawRect.right = rc.right;
				m_rcLastDrawRect.top = rc.top;
				m_rcLastDrawRect.bottom = rc.bottom;
 			}
			else
			{
				if (m_rcLastDrawRect.left > rc.left)
					m_rcLastDrawRect.left = rc.left;
				if (m_rcLastDrawRect.right < rc.right)
					m_rcLastDrawRect.right = rc.right;
				if (m_rcLastDrawRect.top > rc.top)
					m_rcLastDrawRect.top = rc.top;
				if (m_rcLastDrawRect.bottom < rc.bottom)
					m_rcLastDrawRect.bottom = rc.bottom;
			}

			pGraphics->DrawRectangle(
				m_pPen, pBlock->GetLeft() + x, pBlock->GetTop() + y, 
				pBlock->GetWidth() - 1, pBlock->GetHeight() - 1
				);
		}
	}

    return nResult;
}

int KPickBox::GetLastDrawRect(RECT &rcRetRect)
{
    rcRetRect.left = m_rcLastDrawRect.left;
    rcRetRect.top = m_rcLastDrawRect.top;
    rcRetRect.right = m_rcLastDrawRect.right;
    rcRetRect.bottom = m_rcLastDrawRect.bottom;

    return true;
}