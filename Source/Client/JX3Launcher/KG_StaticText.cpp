////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_StaticText.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-6  17:07:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_StaticText.h"

KG_StaticText::KG_StaticText()
{
    m_TextColor = GetSysColor(COLOR_BTNTEXT); // default text color
}

KG_StaticText::~KG_StaticText()
{
}

BEGIN_MESSAGE_MAP(KG_StaticText, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void KG_StaticText::SetWindowText(LPCTSTR lpszString)
{
    CRect rect;

    if (m_strText.Compare(lpszString) == 0)
        return;

    m_strText = lpszString;
    Invalidate(TRUE);    

    GetWindowRect(&rect);
    GetParent()->ScreenToClient(&rect);
    GetParent()->RedrawWindow(rect);
}

COLORREF KG_StaticText::SetTextColor(COLORREF Color)
{
    COLORREF PreviousColor = m_TextColor;
    m_TextColor = Color;
    Invalidate(TRUE);
    return PreviousColor;
}

void KG_StaticText::OnPaint() 
{
    CPaintDC PaintDC(this);
    CRect rect;

    GetClientRect(&rect);

    PaintDC.SelectObject(GetStockObject(DEFAULT_GUI_FONT)); // default font.
    PaintDC.SetBkMode(TRANSPARENT);
    PaintDC.SetTextColor(m_TextColor);
    PaintDC.DrawText(m_strText, rect, DT_WORDBREAK | DT_VCENTER |DT_END_ELLIPSIS);
}
