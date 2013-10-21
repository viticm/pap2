////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:27:18
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditMinMaxDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditMinMaxDlg, CDialog)
KGValueEditMinMaxDlg::KGValueEditMinMaxDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent /* = NULL  */
) : CDialog(KGValueEditMinMaxDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditMinMaxDlg::~KGValueEditMinMaxDlg()
{
}

void KGValueEditMinMaxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN1, m_spinButtonMin);
	DDX_Control(pDX, IDC_SPIN2, m_spinButtonMax);
	DDX_Text(pDX, IDC_MIN, m_nValueMin);
	DDX_Text(pDX, IDC_MAX, m_nValueMax);
}

BOOL KGValueEditMinMaxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString szTitle;
	CString szBound;
	CString szMin;
	CString szMax;
	int     nPos = 0;

	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_spinButtonMin.SetBuddy(GetDlgItem(IDC_MIN));
	m_spinButtonMax.SetBuddy(GetDlgItem(IDC_MAX));
	m_spinButtonMin.SetRange32(itemData.nMin, itemData.nMax);
	m_spinButtonMax.SetRange32(itemData.nMin, itemData.nMax);

	szBound = itemData.szPropertyValue;

	if (nPos != -1)
		szMin	= szBound.Tokenize(" ~ ", nPos);
	if (nPos != -1)
		szMax	= szBound.Tokenize(" ~ ", nPos);

	m_nValueMin = atoi(szMin.GetBuffer());
	m_nValueMax = atoi(szMax.GetBuffer());

	szBound.Format(" [数值范围 %d ~ %d ]", itemData.nMin, itemData.nMax);
	GetWindowText(szTitle);
	SetWindowText(
		szTitle + "_" + m_pListItem->arryItemText.GetAt(0).szItemText + szBound
	);

	UpdateData(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(KGValueEditMinMaxDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void KGValueEditMinMaxDlg::OnBnClickedOk()
{
	UpdateData();

	USER_ITEM_DATA itemData;
	CString szValue;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	if (m_nValueMin < itemData.nMin)
	{
		m_nValueMin = itemData.nMin;
	}
	if (m_nValueMax > itemData.nMax)
	{
		m_nValueMax = itemData.nMax;
	}

	if (m_nValueMin > m_nValueMax)
	{
		::MessageBox(this->m_hWnd, _T("最大值必须大于最小值"), "Error = =b", MB_OK);
		return;
	}

	szValue.Format("%d ~ %d", m_nValueMin, m_nValueMax);

	strncpy_s(
		itemData.szPropertyValue, szValue.GetBuffer(),
		sizeof(itemData.szPropertyValue)
	);
	itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] ='\0';

	m_pTabFile->WriteInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_1, m_nValueMin
	);
	m_pTabFile->WriteInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_2, m_nValueMax
	);

	m_pListItem->SetStructData(&itemData);
	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

	OnOK();
}

