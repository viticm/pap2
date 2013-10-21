// KSceneEditorAntiPortalSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorAntiPortalSetupDialog.h"
#include "IEEditor.h"

// KSceneEditorAntiPortalSetupDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorAntiPortalSetupDialog, CDialog)

KSceneEditorAntiPortalSetupDialog::KSceneEditorAntiPortalSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorAntiPortalSetupDialog::IDD, pParent)
	, m_fWidthBottom(0)
	, m_fWidthTop(0)
	, m_fHeight(0)
	, m_nType(0)
{
	m_lpSceneEditor = NULL;
	m_lpAntiPortal = NULL;
}

KSceneEditorAntiPortalSetupDialog::~KSceneEditorAntiPortalSetupDialog()
{
}

void KSceneEditorAntiPortalSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComboxBox_Type);
	DDX_Text(pDX, IDC_EDIT1, m_fWidthBottom);
	DDX_Text(pDX, IDC_EDIT7, m_fWidthTop);
	DDX_Text(pDX, IDC_EDIT8, m_fHeight);
	DDX_CBIndex(pDX, IDC_COMBO1, m_nType);
}


BEGIN_MESSAGE_MAP(KSceneEditorAntiPortalSetupDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneEditorAntiPortalSetupDialog::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &KSceneEditorAntiPortalSetupDialog::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT1, &KSceneEditorAntiPortalSetupDialog::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT7, &KSceneEditorAntiPortalSetupDialog::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, &KSceneEditorAntiPortalSetupDialog::OnEnChangeEdit8)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KSceneEditorAntiPortalSetupDialog message handlers

BOOL KSceneEditorAntiPortalSetupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ComboxBox_Type.AddString("0 单向矩形");
	m_ComboxBox_Type.AddString("1 双向矩形");
	m_ComboxBox_Type.AddString("2 圆柱");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditorAntiPortalSetupDialog::OnBnClickedButton1()
{
	if(!m_lpSceneEditor)
		return;
	
	HRESULT hr = m_lpSceneEditor->ConvertCurrentSelectedObjectToAntiPortal(&m_lpAntiPortal);
	GetData();
}

void KSceneEditorAntiPortalSetupDialog::OnCbnSelchangeCombo1()
{
	SetData();
}

void KSceneEditorAntiPortalSetupDialog::GetData()
{
	if(!m_lpSceneEditor)
		return;
	if(!m_lpAntiPortal)
		return;

	m_nType = (int)m_lpAntiPortal->GetPortalType();

	m_lpAntiPortal->GetData(&m_fWidthBottom,&m_fWidthTop,&m_fHeight);
	
	UpdateData(FALSE);
}

void KSceneEditorAntiPortalSetupDialog::SetData()
{
	if(!m_lpSceneEditor)
		return;
	if(!m_lpAntiPortal)
		return;

	UpdateData(TRUE);
	m_lpAntiPortal->SetPortalType(m_nType);
	m_lpAntiPortal->SetData(m_fWidthBottom,m_fWidthTop,m_fHeight);

}
void KSceneEditorAntiPortalSetupDialog::OnEnChangeEdit1()
{
	SetData();
}

void KSceneEditorAntiPortalSetupDialog::OnEnChangeEdit7()
{
	SetData();
}

void KSceneEditorAntiPortalSetupDialog::OnEnChangeEdit8()
{
	SetData();
}

void KSceneEditorAntiPortalSetupDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(!m_lpSceneEditor)
		return;
	m_lpSceneEditor->GetCurrentSelectedObjectAntiPortal(&m_lpAntiPortal);

	GetData();
	// TODO: Add your message handler code here
}
