// KDlg_ProItem_Bool.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Bool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Bool dialog
namespace PropertyBase
{

KDlg_ProItem_Bool::KDlg_ProItem_Bool(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Bool::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Bool)
	m_Bool = FALSE;
	//}}AFX_DATA_INIT
	//m_lpStoreBool = NULL;
	//m_lpProperty = NULL;
}


void KDlg_ProItem_Bool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Bool)
	DDX_Control(pDX, IDC_CHECK1, m_Check);
	DDX_Check(pDX, IDC_CHECK1, m_Bool);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Bool, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Bool)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Bool message handlers

void KDlg_ProItem_Bool::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//RefreshData();
}

BOOL KDlg_ProItem_Bool::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if(m_lpStoreBool)
	//{
	//	m_Check.SetWindowText(m_lpProperty->scName.c_str());
	//	m_Bool = m_lpStoreBool->bValue;
	//	UpdateData(FALSE);
	//}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
}
