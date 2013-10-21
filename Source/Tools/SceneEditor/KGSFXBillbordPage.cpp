// KGSFXBillbordPage.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXBillbordPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGSFXBillbordPage dialog

IMPLEMENT_DYNAMIC(KGSFXBillbordPage, CPropertyPage)

KGSFXBillbordPage::KGSFXBillbordPage()
	: CPropertyPage(KGSFXBillbordPage::IDD)
{

}

KGSFXBillbordPage::~KGSFXBillbordPage()
{
}

void KGSFXBillbordPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGSFXBillbordPage, CPropertyPage)
END_MESSAGE_MAP()

BOOL KGSFXBillbordPage::OnInitDialog()
{
    int nResult = CPropertyPage::OnInitDialog();
    RECT rect = {0, 0, 250, 500};
    GetClientRect(&rect);
    m_page.Create(IDD_SFXEDITOR_BILLBOARD, this);
    m_panel.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, rect, this, 123);
    m_panel.ShowWindow(SW_SHOW);
    m_panel.AddPanel(&m_page, "¹«¸æ°æ", TRUE);
    m_panel.UpdateUI();



    return nResult;
}


// KGSFXBillbordPage message handlers
