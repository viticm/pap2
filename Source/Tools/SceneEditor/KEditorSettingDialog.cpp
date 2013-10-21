// KEditorSettingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingDialog dialog


KEditorSettingDialog::KEditorSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KEditorSettingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KEditorSettingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KEditorSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingDialog, CDialog)
	//{{AFX_MSG_MAP(KEditorSettingDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &KEditorSettingDialog::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingDialog message handlers

BOOL KEditorSettingDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_PropertySheet.AddPage(&m_PropertyPageGeneral);
	m_PropertySheet.AddPage(&m_PropertyPageView);
	m_PropertySheet.AddPage(&m_PropertyPageColor);
	m_PropertySheet.AddPage(&m_PropertyPageMapTest);
	m_PropertySheet.AddPage(&m_PropertyPageVideo);
	m_PropertySheet.AddPage(&m_PropertyPageMusic);
	m_PropertySheet.AddPage(&m_cameraSetting);
	
	//显示非摸态的属性表
	m_PropertySheet.Create(this, WS_CHILD | WS_VISIBLE, 0);

	// 当对话框搜索下一个Tab项时，WS_EX_CONTROLPARENT标记避免了死循环发生的可能性
	//他可以让对话框搜索到属性表中的子窗体控件，就象对话框窗体上的其他普通控件一样。
	m_PropertySheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);

	//允许用户TAB键切换到属性表
	m_PropertySheet.ModifyStyle( 0, WS_TABSTOP );

	//显示属性表
	CRect rcSheet;
	GetDlgItem( IDC_PROPSHEET )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );

	// Subtract 7 from the left and top limits of the frame to account for the 
	// property sheet's margin.  This magic number works for the default font used
	// by the resource editor.
	m_PropertySheet.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KEditorSettingDialog::OnBnClickedOk()
{
	if (m_cameraSetting)
		m_cameraSetting.Update();
	OnOK();
}
