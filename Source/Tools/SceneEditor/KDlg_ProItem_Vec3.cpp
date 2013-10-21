// KDlg_ProItem_Vec3.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Vec3.h"
#include ".\kdlg_proitem_vec3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Vec3 dialog
namespace PropertyBase
{


KDlg_ProItem_Vec3::KDlg_ProItem_Vec3(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Vec3::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Vec3)
		// NOTE: the ClassWizard will add member initialization here
	m_strName = "";
	m_vec3Value = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	//}}AFX_DATA_INIT

}


void KDlg_ProItem_Vec3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Vec3)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_EDIT_X, m_ctlEditVec3X);
	DDX_Control(pDX, IDC_EDIT_Y, m_ctlEditVec3Y);
	DDX_Control(pDX, IDC_EDIT_Z, m_ctlEditVec3Z);
	DDX_Control(pDX, IDC_SPIN_X, m_ctlSpinVec3X);
	DDX_Control(pDX, IDC_SPIN_Y, m_ctlSpinVec3Y);
	DDX_Control(pDX, IDC_SPIN_Z, m_ctlSpinVec3Z);

	DDX_Text(pDX, IDC_EDIT_X, m_vec3Value.x);
	DDX_Text(pDX, IDC_EDIT_Y, m_vec3Value.y);
	DDX_Text(pDX, IDC_EDIT_Z, m_vec3Value.z);

	DDX_Text(pDX, IDC_SNAME, m_strName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(KDlg_ProItem_Vec3, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Vec3)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_VSCROLL()
		ON_EN_CHANGE(IDC_EDIT_X, OnChangeEditVec3X)
		ON_EN_CHANGE(IDC_EDIT_Y, OnChangeEditVec3Y)
		ON_EN_CHANGE(IDC_EDIT_Z, OnChangeEditVec3Z)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL KDlg_ProItem_Vec3::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	m_ctlSpinVec3X.SetRange32(-999999, 999999);
	m_ctlSpinVec3Y.SetRange32(-999999, 999999);
	m_ctlSpinVec3Z.SetRange32(-999999, 999999);
	////if(m_lpStore)
	//{
	//	m_strName = m_lpProperty->scName.c_str();
	//	m_vec3Value = *((D3DXVECTOR3*)m_lpProperty->Address);

	//	m_ctlSpinVec3X.SetBuddy(&m_ctlSpinVec3X);
	//	m_ctlSpinVec3Y.SetBuddy(&m_ctlSpinVec3Y);
	//	m_ctlSpinVec3Z.SetBuddy(&m_ctlSpinVec3Z);

	//	
	//	//m_ctlSpinVec3X.Set
	//	int NowPos = (int)m_vec3Value.x;
	//	m_ctlSpinVec3X.SetPos32(NowPos);
	//	NowPos = (int)m_vec3Value.y;
	//	m_ctlSpinVec3Y.SetPos32(NowPos);
	//	NowPos = (int)m_vec3Value.z;
	//	m_ctlSpinVec3Z.SetPos32(NowPos);
	//	UpdateData(FALSE);
	//}
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Vec3 message handlers


void KDlg_ProItem_Vec3::OnChangeEditVec3X()
{
	UpdateData(TRUE);
	//RefreshData();
}

void KDlg_ProItem_Vec3::OnChangeEditVec3Y()
{
	UpdateData();
	//RefreshData();
}

void KDlg_ProItem_Vec3::OnChangeEditVec3Z()
{
	UpdateData();
	//RefreshData();
}

void KDlg_ProItem_Vec3::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSpinButtonCtrl* ctlSpin = (CSpinButtonCtrl*)pScrollBar;
	int iPos = ctlSpin->GetPos32();
	int iID = ctlSpin->GetDlgCtrlID();
	switch (iID) 
	{
	case IDC_SPIN_X:
		m_vec3Value.x = (float)iPos;
		break;
	case IDC_SPIN_Y:
		m_vec3Value.y = (float)iPos;
		break;
	case IDC_SPIN_Z:
		m_vec3Value.z = (float)iPos;
		break;
	}
	UpdateData(FALSE);
	//RefreshData();
}

}
