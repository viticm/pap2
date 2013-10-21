// KGValueEditGetValueDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditGetValueDlg.h"
#include ".\kgvalueeditgetvaluedlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KGValueEditGetValueDlg 对话框

IMPLEMENT_DYNAMIC(KGValueEditGetValueDlg, CDialog)
KGValueEditGetValueDlg::KGValueEditGetValueDlg(CWnd* pParent, CString szTitle, int nValueSize)
	: CDialog(KGValueEditGetValueDlg::IDD, pParent)
{
	m_szTitle = szTitle;
	m_nValueSize = nValueSize;
}

KGValueEditGetValueDlg::~KGValueEditGetValueDlg()
{
}

BOOL KGValueEditGetValueDlg::OnInitDialog()
{
	SetWindowText(m_szTitle);
	return TRUE;
}

void KGValueEditGetValueDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	GetDlgItem(IDC_EDIT)->SetFocus();
}

void KGValueEditGetValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_szValue);
}

CString KGValueEditGetValueDlg::GetValue()
{
	//UpdateData();
	return m_szValue;
}

BEGIN_MESSAGE_MAP(KGValueEditGetValueDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void KGValueEditGetValueDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;

	CString szErrorMessage = _T("");

	UpdateData();

	if (m_szValue.GetLength() > m_nValueSize)
	{
		szErrorMessage.Format(
			_T("文本长度不可大于 %d !!"), m_nValueSize
		);
		::MessageBox(
			this->m_hWnd, szErrorMessage.GetBuffer(), _T("Error!!"),
			MB_OK | MB_ICONERROR
		);
		KG_PROCESS_ERROR(false);
	}

	OnOK();
Exit0:
	return;
}
