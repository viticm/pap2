// KDlg_ProItem_Float.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Float dialog
namespace PropertyBase
{

KDlg_ProItem_Float::KDlg_ProItem_Float(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Float::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Float)
	m_Value = 0.0f;
	m_Name = _T("");
	TotalStep = 0;
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_Float::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Float)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Text(pDX, IDC_EDIT1, m_Value);
	DDX_Text(pDX, IDC_SNAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Float, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Float)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_WM_VSCROLL()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Float message handlers

BOOL KDlg_ProItem_Float::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if(m_lpStore)
	//{
	//	m_Name = m_lpProperty->scName.c_str();
	//	m_Value = m_lpStore->Value;

	//	m_Spin.SetBuddy(&m_Edit);

	//	TotalStep = (int)((m_lpStore->Max - m_lpStore->Min) / m_lpStore->Step);
	//	m_Spin.SetRange32(0,TotalStep);
	//	int NowPos = (int)((m_lpStore->Value - m_lpStore->Min) / m_lpStore->Step);
	//	m_Spin.SetPos32(NowPos);

	//	UpdateData(FALSE);
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_Float::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(m_lpStore)
	//{
	//	if(m_Value<m_lpStore->Min)
	//	{
	//		m_Value = m_lpStore->Min;
	//	}
	//	if(m_Value>m_lpStore->Max)
	//	{
	//		m_Value = m_lpStore->Max;
	//	}
	//}
	//RefreshData();
}

void KDlg_ProItem_Float::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//if(m_lpStore)
	//{
	//	int Pos = m_Spin.GetPos32();
	//	m_Value = m_lpStore->Min + Pos * m_lpStore->Step;
	//	UpdateData(FALSE);
	//	RefreshData();
	//}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void KDlg_ProItem_Float::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	//CDialog::OnClose();
}
}
