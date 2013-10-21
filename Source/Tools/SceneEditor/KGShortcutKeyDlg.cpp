// KGShortcutKey.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGShortcutKeyDlg.h"


// KGShortcutKeyDlg dialog

IMPLEMENT_DYNAMIC(KGShortcutKeyDlg, CDialog)

KGShortcutKeyDlg::KGShortcutKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGShortcutKeyDlg::IDD, pParent)
{

}

KGShortcutKeyDlg::~KGShortcutKeyDlg()
{
}

void KGShortcutKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_KeyList);
}


BEGIN_MESSAGE_MAP(KGShortcutKeyDlg, CDialog)
END_MESSAGE_MAP()
BOOL KGShortcutKeyDlg::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();
	m_KeyList.InsertColumn(0, _T("Key"), LVCFMT_CENTER, 100);
	m_KeyList.InsertColumn(1, _T("Use"), LVCFMT_CENTER, 200);
	m_KeyList.SetExtendedStyle(m_KeyList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return bRet;
}
BOOL KGShortcutKeyDlg::FillKeyList()
{
	BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	ITabFile*	piTabFile   = NULL;
	//	char        szFilePath[MAX_PATH];
	char	szKey[32];
	char	szUse[MAX_PATH];
	
	piTabFile = g_OpenTabFile("\\data\\editor\\editordata\\ShortcutKey.tab");
	KGLOG_PROCESS_ERROR(piTabFile);
	
	m_KeyList.DeleteAllItems();
	for (int nRowIndex = 2; nRowIndex <= piTabFile->GetHeight(); nRowIndex++)
	{
		//KGKeyParams  KeyParam;

		//bRetCode = piTabFile->GetInteger(nRowIndex, "ID", 0, (int*)&KeyParam.nID);
		//KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetString(nRowIndex, "Key", "", szKey, sizeof(szKey));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetString(nRowIndex, "Use", "", szUse, sizeof(szUse));
		KGLOG_PROCESS_ERROR(bRetCode);
		//m_KeyList.InsertItem(nRowIndex,szKey);
		//m_KeyList.SetItemText(nRowIndex,1,szUse);
		m_KeyList.InsertItem(nRowIndex - 2,TEXT(""));
		m_KeyList.SetItemText(nRowIndex - 2,0,szKey);
		m_KeyList.SetItemText(nRowIndex-2,1,szUse);

	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;

}
// KGShortcutKeyDlg message handlers
