// KDlg_ProItem_String.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_String.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_String dialog
namespace PropertyBase
{

KDlg_ProItem_String::KDlg_ProItem_String(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_String::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_String)
	m_Value = _T("");
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_String::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_String)
	DDX_Text(pDX, IDC_EDIT1, m_Value);
	DDX_Text(pDX, IDC_SNAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_String, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_String)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_String message handlers

BOOL KDlg_ProItem_String::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if(m_lpProperty)
	//{
	//	m_Name = m_lpProperty->scName.c_str();
	//	LPSTR pStr = NULL;
	//	if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STRING)
	//	{
	//		pStr = (LPSTR) m_lpProperty->Address;
	//		m_Value =  pStr;
	//	}
	//	else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STLSTRING)
	//	{
	//		m_Value = ((string*) m_lpProperty->Address)->c_str();
	//	}
	//	UpdateData(FALSE);
	//}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_String::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	//CDialog::OnClose();
}

void KDlg_ProItem_String::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//RefreshData();	
}
}