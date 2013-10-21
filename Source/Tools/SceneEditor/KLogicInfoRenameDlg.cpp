// KLogicInfoRenameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KLogicInfoRenameDlg.h"


// KLogicInfoRenameDlg dialog

IMPLEMENT_DYNAMIC(KLogicInfoRenameDlg, CDialog)

KLogicInfoRenameDlg::KLogicInfoRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KLogicInfoRenameDlg::IDD, pParent)
{

}

KLogicInfoRenameDlg::~KLogicInfoRenameDlg()
{
}

void KLogicInfoRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(KLogicInfoRenameDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &KLogicInfoRenameDlg::OnBnClickedButtonOK)
	ON_BN_CLICKED(IDC_BUTTON2, &KLogicInfoRenameDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// KLogicInfoRenameDlg message handlers

BOOL KLogicInfoRenameDlg::OnInitDialog(void)
{
	BOOL bResult = CDialog::OnInitDialog();
	m_Edit.SetWindowText(m_strEditText);
	return bResult;
}

void KLogicInfoRenameDlg::OnBnClickedButtonOK()
{
	// TODO: Add your control notification handler code here
	m_Edit.GetWindowText(m_strEditText);
	CDialog::OnOK();
}

void KLogicInfoRenameDlg::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
