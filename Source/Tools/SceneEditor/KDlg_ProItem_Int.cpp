// KDlg_ProItem_Int.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Int.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Int dialog
namespace PropertyBase
{

KDlg_ProItem_Int::KDlg_ProItem_Int(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Int::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Int)
	m_Int = 0;
	m_Name = _T("");
	//}}AFX_DATA_INIT
	//m_lpStoreInt = NULL;
	//m_lpProperty = NULL;
}


void KDlg_ProItem_Int::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Int)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Text(pDX, IDC_EDIT1, m_Int);
	DDX_Text(pDX, IDC_SNAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Int, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Int)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Int message handlers

BOOL KDlg_ProItem_Int::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if(m_lpStoreInt)
	//{
	//	m_Name = m_lpProperty->scName.c_str();
	//	m_Int = m_lpStoreInt->Value;

	//	m_Spin.SetBuddy(&m_Edit);
	//	m_Spin.SetRange32(m_lpStoreInt->Min,m_lpStoreInt->Max);
	//	m_Spin.SetPos32(m_lpStoreInt->Value);

	//	UpdateData(FALSE);
	//}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_Int::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_Int = m_Spin.GetPos32();
	UpdateData(FALSE);
	//RefreshData();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void KDlg_ProItem_Int::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpStoreInt)
	//{
	//	if(m_Int<m_lpStoreInt->Min)
	//	{
	//		m_Int = m_lpStoreInt->Min;
	//	}
	//	if(m_Int>m_lpStoreInt->Max)
	//	{
	//		m_Int = m_lpStoreInt->Max;
	//	}
	//}
	//RefreshData();
}
}