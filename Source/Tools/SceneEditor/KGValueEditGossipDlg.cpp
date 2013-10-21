// KGValueEditGossipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditGossipDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGValueEditGossipDlg dialog

IMPLEMENT_DYNAMIC(KGValueEditGossipDlg, CDialog)

KGValueEditGossipDlg::KGValueEditGossipDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEditGossipDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditGossipDlg::~KGValueEditGossipDlg()
{
}

void KGValueEditGossipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabCtrl);
	DDX_Text(pDX, IDC_EDIT, m_szText);
	DDX_Control(pDX, IDC_COMBO, m_comboType);
}


BEGIN_MESSAGE_MAP(KGValueEditGossipDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, &KGValueEditGossipDlg::OnTcnSelchangingTab)
	ON_BN_CLICKED(IDOK, &KGValueEditGossipDlg::OnBnClickedOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &KGValueEditGossipDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// KGValueEditGossipDlg message handlers

BOOL KGValueEditGossipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	USER_ITEM_DATA itemData;
	int  nType = -1;
	char szText[1024];
	*szText = '\0';

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_tabCtrl.InsertItem(0, _T("对话泡泡一"));
	m_tabCtrl.InsertItem(1, _T("对话泡泡二"));
	m_tabCtrl.InsertItem(2, _T("对话泡泡三"));
	m_tabCtrl.InsertItem(3, _T("对话泡泡四"));

	m_pIniFile->GetString(
		itemData.szParamName_1, _T("True"), "", m_szTypeTrue, sizeof(m_szTypeTrue)
	);
	m_pIniFile->GetString(
		itemData.szParamName_1, _T("False"), "", m_szTypeFalse, sizeof(m_szTypeFalse)
	);

	m_comboType.AddString(m_szTypeTrue);
	m_comboType.AddString(m_szTypeFalse);

	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_1, 0, &nType
	);

	if (nType == 1)
	{
		m_comboType.SelectString(0, m_szTypeTrue);
	}
	else if (nType == 0)
	{
		m_comboType.SelectString(0, m_szTypeFalse);
	}

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_2, _T(""),
		szText, sizeof(szText)
	);
	m_szGossipText[0] = szText;

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_3, _T(""),
		szText, sizeof(szText)
	);
	m_szGossipText[1] = szText;

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_4, _T(""),
		szText, sizeof(szText)
	);
	m_szGossipText[2] = szText;

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_5, _T(""),
		szText, sizeof(szText)
	);
	m_szGossipText[3] = szText;

	m_szGossipText[0].Replace("\\n", "\r\n");
	m_szGossipText[1].Replace("\\n", "\r\n");
	m_szGossipText[2].Replace("\\n", "\r\n");
	m_szGossipText[3].Replace("\\n", "\r\n");

	m_szText = m_szGossipText[0];

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KGValueEditGossipDlg::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSel = m_tabCtrl.GetCurSel();

	UpdateData();

	m_szGossipText[nSel] = m_szText;

	*pResult = 0;
}

void KGValueEditGossipDlg::OnBnClickedOk()
{
	int nSel = m_tabCtrl.GetCurSel();
	CString szType;
	CString szValue;
	int nType = 0;
	USER_ITEM_DATA itemData;

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	UpdateData();

	m_szGossipText[nSel] = m_szText;

	m_comboType.GetWindowText(szType);

	if (szType == CString(m_szTypeTrue))
	{
		nType = 1;
	}
	else if (szType == CString(m_szTypeFalse))
	{
		nType = 0;
	}
	else
	{
		nType = -1;
	}

	m_szGossipText[0].Replace("\r\n", "\\n");
	m_szGossipText[1].Replace("\r\n", "\\n");
	m_szGossipText[2].Replace("\r\n", "\\n");
	m_szGossipText[3].Replace("\r\n", "\\n");

	m_pTabFile->WriteInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_1, nType
	);

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_2, m_szGossipText[0]
	);
	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_3, m_szGossipText[1]
	);
	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_4, m_szGossipText[2]
	);
	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_5, m_szGossipText[3]
	);

	szValue = _T("泡泡类型 : ") + szType + _T(" 　泡泡内容　: ..........");

	strncpy_s(
		itemData.szPropertyValue, szValue.GetBuffer(),
		sizeof(itemData.szPropertyValue)
	);
	itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] ='\0';

	m_pListItem->SetStructData(&itemData);
	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

	OnOK();
}

void KGValueEditGossipDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSel = m_tabCtrl.GetCurSel();

	m_szText = m_szGossipText[nSel];

	UpdateData(false);

	*pResult = 0;
}
