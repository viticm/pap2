////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditForceDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-11 16:47:58
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditForceDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditForceDlg, CDialog)
KGValueEditForceDlg::KGValueEditForceDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent
): CDialog(KGValueEditForceDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditForceDlg::~KGValueEditForceDlg()
{
}

void KGValueEditForceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_comboxForce);
}


BEGIN_MESSAGE_MAP(KGValueEditForceDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BOOL KGValueEditForceDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	char szKeyValue_1[256];
	char szKeyValue_2[256];
	char szKeyName_1[256];
	char szKeyName_2[256];

	*szKeyValue_1 = '\0';
	*szKeyValue_2 = '\0';
	*szKeyName_1  = '\0';
	*szKeyName_2  = '\0';

	USER_ITEM_DATA itemData;

	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);
	KGLOG_PROCESS_ERROR(m_pListItem);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	while (
		m_pIniFile->GetNextKey(_T("Force"), szKeyName_1, szKeyName_1)
	)
	{
		m_pIniFile->GetString(
			_T("Force"), szKeyName_1, _T(""), szKeyName_2, sizeof(szKeyName_2)
		);
		m_pIniFile->GetString(
			szKeyName_2, _T("Name"), _T(""), szKeyValue_1, sizeof(szKeyValue_1)
		);
		m_comboxForce.AddString(szKeyValue_1);
		m_pIniFile->GetString(
			szKeyName_2, _T("Id"), _T(""), szKeyValue_2, sizeof(szKeyValue_2)
		);
		if (CString(szKeyValue_2) == itemData.szPropertyValue)
		{
			m_comboxForce.SelectString(0, szKeyValue_1);
		}
	}

	m_comboxForce.SetFocus();

	nResult = true;
Exit0:
	return nResult;
}

void KGValueEditForceDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	char szKeyValue_1[256];
	char szKeyValue_2[256];
	char szKeyName_1[256];
	char szKeyName_2[256];

	*szKeyValue_1 = '\0';
	*szKeyValue_2 = '\0';
	*szKeyName_1  = '\0';
	*szKeyName_2  = '\0';

	CString szValue = _T("");
	int nIndex = 0;;

	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);
	KGLOG_PROCESS_ERROR(m_pListItem);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	UpdateData();

	nIndex = m_comboxForce.GetCurSel();
	KGLOG_PROCESS_ERROR(nIndex != -1);

	m_comboxForce.GetLBText(nIndex, szValue);

	while (m_pIniFile->GetNextKey(_T("Force"), szKeyName_1, szKeyName_1))
	{
		m_pIniFile->GetString(
			_T("Force"), szKeyName_1, _T(""), szKeyName_2, sizeof(szKeyName_2)
		);
		m_pIniFile->GetString(
			szKeyName_2, _T("Name"), _T(""), szKeyValue_1, sizeof(szKeyValue_1)
		);
		if (CString(szKeyValue_1) == szValue)
		{
			m_pIniFile->GetString(
				szKeyName_2, _T("Id"), _T(""), szKeyValue_2, sizeof(szKeyValue_2)
			);
			break;
		}
	}

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szPropertyName, szKeyValue_2
	);

	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

	nResult = true;
Exit0:
	return OnOK();
}
