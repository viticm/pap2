// KDlgTagProcessFrontEnd.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgTagProcessFrontEnd.h"


// KDlgTagProcessFrontEnd dialog

IMPLEMENT_DYNAMIC(KDlgTagProcessFrontEnd, CDialog)

KDlgTagProcessFrontEnd::KDlgTagProcessFrontEnd(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgTagProcessFrontEnd::IDD, pParent)
	, m_strNewDire(_T(""))
{

}

KDlgTagProcessFrontEnd::~KDlgTagProcessFrontEnd()
{
}

void KDlgTagProcessFrontEnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIR, m_strNewDire);
	DDX_Control(pDX, IDC_LIST_SELDIR, m_listDir);
	DDX_Control(pDX, IDC_EDIT_DIR, m_editDir);
}


BEGIN_MESSAGE_MAP(KDlgTagProcessFrontEnd, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_SELDIR, &KDlgTagProcessFrontEnd::OnLbnDblclkListSeldir)
	ON_BN_CLICKED(IDC_BUTTON_OK, &KDlgTagProcessFrontEnd::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// KDlgTagProcessFrontEnd message handlers

void KDlgTagProcessFrontEnd::OnLbnDblclkListSeldir()
{
	int nCurSel = m_listDir.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	DWORD dwHash = static_cast<DWORD>(m_listDir.GetItemData(nCurSel));
	m_setDirs.erase(dwHash);
	m_listDir.DeleteString(nCurSel);
Exit0:
	;
}

void KDlgTagProcessFrontEnd::OnOK()
{
	UpdateData();
	CString strLower;
	KG_PROCESS_SUCCESS(m_strNewDire == "");
	
	strLower = m_strNewDire;
	strLower.MakeLower();
	DWORD dwHash = g_FileNameHash(strLower);
	if (!m_setDirs.count(dwHash))
	{
		int nIndex = m_listDir.AddString(m_strNewDire);
		m_listDir.SetItemData(nIndex, dwHash);
		m_setDirs.insert(dwHash);
	}
Exit1:
	;
}

void KDlgTagProcessFrontEnd::OnBnClickedButtonOk()
{
	for (int i = 0; i < m_listDir.GetCount(); i++)
	{
		CString strPath;
		m_listDir.GetText(i, strPath);
		m_Paths.push_back(strPath);
	}
	CDialog::OnOK();
}
