// KGSFXModelPage.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXModelPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGSFXModelPage dialog

IMPLEMENT_DYNAMIC(KGSFXModelPage, CPropertyPage)

KGSFXModelPage::KGSFXModelPage()
	: CPropertyPage(KGSFXModelPage::IDD)
{

}

KGSFXModelPage::~KGSFXModelPage()
{
}

void KGSFXModelPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGSFXModelPage, CPropertyPage)
END_MESSAGE_MAP()

BOOL KGSFXModelPage::OnInitDialog()
{
    int nResult = CPropertyPage::OnInitDialog();
    RECT rect = {0, 0, 250, 500};
    GetClientRect(&rect);
    m_page.Create(IDD_SFXEDITOR_MODEL, this);
    m_panel.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, rect, this, 123);
    m_panel.ShowWindow(SW_SHOW);
    m_panel.AddPanel(&m_page, "Ä£ÐÍ", TRUE);
    m_panel.UpdateUI();



    return nResult;
}

// KGSFXModelPage message handlers
