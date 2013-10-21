////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditBoolDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:27:18
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditBoolDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditBoolDlg, CDialog)
KGValueEditBoolDlg::KGValueEditBoolDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent /* = NULL  */
) : CDialog(KGValueEditBoolDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditBoolDlg::~KGValueEditBoolDlg()
{
}

void KGValueEditBoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_comboxValue);
}

BOOL KGValueEditBoolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szTitle;
	char szUncaseText[256];
	char szTureText[256];
	char szFalseText[256];
	*szUncaseText = '\0';
	*szTureText   = '\0';
	*szFalseText  = '\0';
	USER_ITEM_DATA itemData;
	int nRetCode = false;
	int nValue   = 0;

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	GetWindowText(szTitle);
	SetWindowText(szTitle + "_" + m_pListItem->arryItemText.GetAt(0).szItemText);

	nRetCode = m_pIniFile->GetString(
		itemData.szPropertyName, "True", _T(""), szTureText, sizeof(szTureText)
	);
	if (CString(szTureText) != "")
	{
		m_comboxValue.AddString(szTureText);
		m_pszState[0] = _T("1");
	}

	nRetCode = m_pIniFile->GetString(
		itemData.szPropertyName, "False", _T(""),
		szFalseText, sizeof(szFalseText)
	);
	if (CString(szFalseText) != "")
	{
		m_comboxValue.AddString(szFalseText);
		m_pszState[1] = _T("0");
	}

	nRetCode = m_pIniFile->GetString(
		itemData.szPropertyName, "Uncase", _T(""),
		szUncaseText, sizeof(szUncaseText)
	);
	if (CString(szUncaseText) != "")
	{
		m_comboxValue.AddString(szUncaseText);
		m_pszState[2] = _T("-1");
	}

	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szPropertyName, 0, &nValue
	);

	if (nValue == 1)
	{
		m_comboxValue.SelectString(0, szTureText);
	}
	else if (nValue == 0)
	{
		m_comboxValue.SelectString(0, szFalseText);
	}
	else
	{
		m_comboxValue.SelectString(0, szUncaseText);
	}

	UpdateData(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(KGValueEditBoolDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void KGValueEditBoolDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;

	UpdateData();

	USER_ITEM_DATA itemData;
	CString szValue;
	int nIndex = m_comboxValue.GetCurSel();

	KG_PROCESS_ERROR(nIndex != -1);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));
	szValue = m_pszState[nIndex];

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szPropertyName, szValue
	);

	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

	nResult = true;
Exit0:
	OnOK();
}

