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
#include "KGValueEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGValueEditIntDlg, CDialog)
KGValueEditIntDlg::KGValueEditIntDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEditIntDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditIntDlg::~KGValueEditIntDlg()
{
}

void KGValueEditIntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN, m_spinButton);
	DDX_Text(pDX, IDC_VALUE, m_nValue);
}

BOOL KGValueEditIntDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szTitle;
	CString szBound;

	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	GetWindowText(szTitle);
	SetWindowText(szTitle + "_" + m_pListItem->arryItemText.GetAt(0).szItemText);

	m_spinButton.SetBuddy(GetDlgItem(IDC_VALUE));
	m_spinButton.SetRange32(itemData.nMin, itemData.nMax);

	/*m_nValue = atoi(itemData.szPropertyValue);*/
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, itemData.szPropertyName, 0, &m_nValue);

	szBound.Format("[ÊýÖµ·¶Î§ %d ~ %d ]", itemData.nMin, itemData.nMax);

	UpdateData(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(KGValueEditIntDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void KGValueEditIntDlg::OnBnClickedOk()
{
	CString stringText;

	GetDlgItem(IDC_VALUE)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(IDC_VALUE)->SetWindowText(stringText);

	UpdateData();
	USER_ITEM_DATA itemData;
	CString szValue;

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	if (m_nValue < itemData.nMin)
	{
		m_nValue = itemData.nMin;
	}
	if (m_nValue > itemData.nMax)
	{
		m_nValue = itemData.nMax;
	}

	szValue.Format("%d", m_nValue);

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szPropertyName, szValue
	);

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

