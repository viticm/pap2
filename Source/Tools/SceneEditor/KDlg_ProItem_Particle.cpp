// KDlg_ProItem_Particle.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Particle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Particle dialog
namespace PropertyBase
{

KDlg_ProItem_Particle::KDlg_ProItem_Particle(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Particle::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Particle)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_Particle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Particle)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Particle, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Particle)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Particle message handlers

void KDlg_ProItem_Particle::OnButton1() 
{
	KDlg_PropertyPage::_Group* pGroup = &m_lpPropertyPage->m_vecGroup[1];
	BOOL Ex = pGroup->m_bExtracted;
	pGroup->Clear();//group 的界面

	PropertyBase::_PropertyGroup* pProGroup = &m_lpPropertyPage->m_lpProperPage->vecGroup[1];
	pProGroup->Clear();
	pProGroup->AddPropertyInt("ssss",&Ex,0,0);//group的数据
	pProGroup->AddPropertyInt("ssss",&Ex,0,0);
	pProGroup->AddPropertyInt("ssss",&Ex,0,0);
	pProGroup->AddPropertyInt("ssss",&Ex,0,0);

	//refresh
	pGroup->Create(1,pProGroup,m_lpPropertyPage);
	pGroup->CreateItems(pProGroup,m_lpPropertyPage);
	pGroup->SetExtracted(Ex);

	CRect m_Rect;
	m_lpPropertyPage->GetWindowRect(&m_Rect);
	m_lpPropertyPage->OnMove(m_Rect.left,m_Rect.right);

}

}