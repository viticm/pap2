////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorPropertySheet.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:17:15
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorPropertySheet.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KSceneSFXEditorPropertySheet, KGPropertySheet)

KSceneSFXEditorPropertySheet::KSceneSFXEditorPropertySheet(CWnd* pParentWnd)
    :KGPropertySheet(/*AFX_IDS_APP_TITLE,pParentWnd*/)
{
	//AddPage(&m_pageParticle);
 //   AddPage(&m_pageBillboard);
 //   //AddPage(&m_pageTrack);
 //   AddPage(&m_pageModel);
 //   //AddPage(&m_pageMeteor);
 //   //AddPage(&m_pageMagicParpet);
 //   AddPage(&m_pageMisc);
 //   AddPage(&m_pageBind);


}

KSceneSFXEditorPropertySheet::~KSceneSFXEditorPropertySheet(void)
{
}

BOOL KSceneSFXEditorPropertySheet::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_LBUTTONDBLCLK && GetOwner())
        GetOwner()->PostMessage(WM_LBUTTONDBLCLK);
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        if (GetActivePage() == &m_pageParticle)
            m_pageParticle.m_particlePage.OnMouseWheel(0, HIWORD(pMsg->wParam), 0);
    }
    //    GetActivePage()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    return KGPropertySheet::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(KSceneSFXEditorPropertySheet, KGPropertySheet)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int KSceneSFXEditorPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (KGPropertySheet::OnCreate(lpCreateStruct) == -1)
        return -1;

    AddPage(&m_pageParticle, m_pageParticle.IDD);
    AddPage(&m_pageBillboard, m_pageBillboard.IDD);
    AddPage(&m_pageModel, m_pageModel.IDD);
    AddPage(&m_pageMisc, m_pageMisc.IDD);
    AddPage(&m_pageBind, m_pageBind.IDD);
    AddPage(&m_pageExp, m_pageExp.IDD);

    /*
    AddPage(&m_GeoPane, m_GeoPane.IDD);
    AddPage(&m_MfyPane, m_MfyPane.IDD);
    */

    return 0;
}

void KSceneSFXEditorPropertySheet::OnPaint()
{
    KGPropertySheet::OnPaint();
    //CClientDC dc(this);
    //RECT rect;
    //GetClientRect(&rect);
    //CPen pen(PS_SOLID, 4, RGB(100, 100, 100));//RGB(240, 200, 85));
    //dc.SelectObject(&pen);
    //dc.MoveTo(rect.left, rect.top);
    //dc.LineTo(rect.left, rect.bottom);

    //
    //CTabCtrl* pTabCtrl = GetTabControl();
    //if (pTabCtrl)
    //{
    //    RECT rect;
    //    pTabCtrl->GetItemRect(pTabCtrl->GetItemCount() - 1, &rect);
    //}
}

void KSceneSFXEditorPropertySheet::OnSize(UINT nType, int cx, int cy)
{
    //RECT rect;
    //GetClientRect(&rect);
    //rect.top += 30;
    //rect.left += 10;
    //rect.bottom -= 20;
    //rect.right;
    //if (::IsWindow(m_pageParticle.m_hWnd))
    //    m_pageParticle.MoveWindow(&rect);
    //if (::IsWindow(m_pageBillboard.m_hWnd))
    //    m_pageBillboard.MoveWindow(&rect);
    ////if (::IsWindow(m_pageTrack.m_hWnd))
    ////    m_pageTrack.MoveWindow(&rect);
    //if (::IsWindow(m_pageModel.m_hWnd))
    //    m_pageModel.MoveWindow(&rect);
    //if (::IsWindow(m_pageMisc.m_hWnd))
    //    m_pageMisc.MoveWindow(&rect);
    //if (::IsWindow(m_pageBind.m_hWnd))
    //    m_pageBind.MoveWindow(&rect);
    //if (::IsWindow(m_pageMagicParpet.m_hWnd))
    //    m_pageMagicParpet.MoveWindow(&rect);
    return KGPropertySheet::OnSize(nType, cx, cy);
}