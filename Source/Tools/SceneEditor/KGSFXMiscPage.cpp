// KGSFXMiscPage.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXMiscPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGSFXMiscPage dialog

IMPLEMENT_DYNAMIC(KGSFXMiscPage, CPropertyPage)

KGSFXMiscPage::KGSFXMiscPage()
	: CPropertyPage(KGSFXMiscPage::IDD)
{

}

KGSFXMiscPage::~KGSFXMiscPage()
{
}

void KGSFXMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGSFXMiscPage, CPropertyPage)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL KGSFXMiscPage::OnInitDialog()
{
    int nResult = CPropertyPage::OnInitDialog();
    RECT rect = {0, 0, 250, 500};
    GetClientRect(&rect);
    m_pageTrack.Create(IDD_SFXEDITOR_TRACK, this);
    m_pageMeteor.Create(IDD_SFXEDITOR_METEOR, this);
    m_pageMagicParpet.Create(IDD_SFXEDITOR_MAGICPARPET, this);
    m_pageChannel.Create(IDD_SFXEDITOR_CHANNEL, this);
    m_pageSound.Create(IDD_SFXEDITOR_SOUND, this);
    m_panel.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, rect, this, 123);
    m_panel.ShowWindow(SW_SHOW);
    m_panel.AddPanel(&m_pageMeteor, "拖尾", TRUE);
    m_panel.AddPanel(&m_pageMagicParpet, "AOE", TRUE);
    m_panel.AddPanel(&m_pageTrack, "轨道", TRUE);
    m_panel.AddPanel(&m_pageChannel, "导引",TRUE);
    m_panel.AddPanel(&m_pageSound, "音效", TRUE);
    m_panel.UpdateUI();



    return nResult;
}

void KGSFXMiscPage::OnSize(UINT nType, int cx, int cy)
{
    RECT rect;
    GetClientRect(&rect);
    rect.bottom -= 10;
    if (::IsWindow(m_panel.m_hWnd))
        m_panel.MoveWindow(&rect);
    CPropertyPage::OnSize(nType, cx, cy);
}
// KGSFXMiscPage message handlers
