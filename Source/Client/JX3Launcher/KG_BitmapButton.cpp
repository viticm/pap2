////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_BitmapButton.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-6  16:18:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_BitmapButton.h"

KG_BitmapButton::KG_BitmapButton()
{
    m_TextColor = GetSysColor(COLOR_BTNTEXT); // default button text color
    m_nButtonDownFlag = false;
    m_nMouseInFlag    = false;
}

KG_BitmapButton::~KG_BitmapButton()
{
}

BEGIN_MESSAGE_MAP(KG_BitmapButton, CButton)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_KEYDOWN()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

BOOL KG_BitmapButton::LoadBitmaps(UINT nNormal, UINT nDown, UINT nOver)
{
    int nResult  = FALSE;
    int nRetCode = FALSE;

    // Free previous allocated bitmap
    m_bmpNormal.DeleteObject();	
    m_bmpDown.DeleteObject();
    m_bmpOver.DeleteObject();

    if (nNormal > 0)
    {
        nRetCode = m_bmpNormal.LoadBitmap(nNormal);
        if (!nRetCode)
            goto Exit0;
    }

    if (nDown > 0)
    {
        nRetCode = m_bmpDown.LoadBitmap(nDown);
        if (!nRetCode)
            goto Exit0;
    }

    if (nOver)
    {
        nRetCode = m_bmpOver.LoadBitmap(nOver);
        if (!nRetCode)
            goto Exit0;
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

COLORREF KG_BitmapButton::SetTextColor(COLORREF NewColor)
{
    COLORREF PrevoisColor = m_TextColor;
    m_TextColor = NewColor;
    return PrevoisColor;
}

void KG_BitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    CDC *pDC = NULL;
    CBitmap *pBitmap = NULL;
    BITMAP bmp;
    UINT nState = 0;
    CRect rect;
    CDC cDC;
    CString strText;

    ASSERT (lpDrawItemStruct);

    pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    pDC->SetBkMode(TRANSPARENT);

    nState = lpDrawItemStruct->itemState;
    if ((nState & ODS_SELECTED) && m_bmpDown.m_hObject)
        pBitmap = &m_bmpDown;
    else if (m_nMouseInFlag && m_bmpOver.m_hObject)
        pBitmap = &m_bmpOver;
    else
        pBitmap = &m_bmpNormal;

    cDC.CreateCompatibleDC(pDC);
    cDC.SelectObject(pBitmap);

    GetClientRect(&rect);

    pBitmap->GetBitmap(&bmp);
    pDC->StretchBlt(
        0, 0, rect.Width(), rect.Height(), &cDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY
    );

    GetWindowText(strText);
    pDC->SetTextColor(m_TextColor);
    pDC->DrawText(strText, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

void KG_BitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
    int nRetCode = false;
    int nTrackingFlag = false;
    CWnd *pParent = NULL;
    TRACKMOUSEEVENT TrackMouseEvent = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0};

    if (m_nButtonDownFlag && (::GetCapture() == m_hWnd))
        nTrackingFlag = true;

    if (!nTrackingFlag && !m_nMouseInFlag)
    {       
        nRetCode = ::_TrackMouseEvent(&TrackMouseEvent);
        if (nRetCode)
        {
            pParent = GetParent();
            if (pParent)
                pParent->PostMessage(MY_MSG_MOUSE_IN_BUTTON, 0, (LPARAM)this);

            m_nMouseInFlag = true;
            Invalidate(TRUE);
        }
    }

    //Forward this event to superclass
    CButton::OnMouseMove(nFlags, point);
}

LRESULT KG_BitmapButton::OnMouseLeave(WPARAM, LPARAM)
{
    CWnd *pParent = NULL;

    ASSERT (m_nMouseInFlag);
    
    pParent = GetParent();
    if (pParent)
        pParent->PostMessage(MY_MSG_MOUSE_LEAVE_BUTTON, 0, (LPARAM)this);
    
    m_nMouseInFlag = false;
    Invalidate(TRUE);
    return NULL;
}

BOOL KG_BitmapButton::OnClicked() 
{
    m_nButtonDownFlag = false;
    return FALSE;   // do not stop in this handler, forward to parent
}

void KG_BitmapButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    int nRetCode = false;
    TRACKMOUSEEVENT TrackMouseEvent = {sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, m_hWnd, 0};
    
    //If we are tracking this button, cancel it
    if (m_nMouseInFlag)
    {
        nRetCode = ::_TrackMouseEvent(&TrackMouseEvent);
        if (nRetCode)
            m_nMouseInFlag = false;
    }

    m_nButtonDownFlag = true;

    //Default-process the message
    CButton::OnLButtonDown(nFlags, point);
}
