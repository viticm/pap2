////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditStringDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:27:18
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditStringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGValueEditStringDlg, CDialog)
KGValueEditStringDlg::KGValueEditStringDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent /* = NULL  */
) : CDialog(KGValueEditStringDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditStringDlg::~KGValueEditStringDlg()
{
}

void KGValueEditStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_stringValue);
}

BOOL KGValueEditStringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szTitle;
	CString szLength;

	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	szLength.Format("%d", itemData.nLength);

	GetWindowText(szTitle);
	SetWindowText(
		szTitle + "_" + m_pListItem->arryItemText.GetAt(0).szItemText +
		" [字符串长度小于 " + szLength + " ]"
	);
	GetWindowText(m_stringPreTitle);
	m_stringValue = itemData.szPropertyValue;
	m_stringValue.Replace("\\n", "\r\n");
	((CEdit*)(GetDlgItem(IDC_EDIT)))->SetLimitText(itemData.nLength);

	UpdateData(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(KGValueEditStringDlg, CDialog)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT, &KGValueEditStringDlg::OnEnChangeValue)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void KGValueEditStringDlg::OnEnChangeValue()
{
	UpdateData();
	CString stringText  = _T("");
	stringText.Format(
		"%s  已输入字符数 : %d", m_stringPreTitle.GetBuffer(), m_stringValue.GetLength()
	);
	SetWindowText(stringText);
}

void KGValueEditStringDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rectClient;
	CRect rectText;
	CRect rectOK;
	CRect rectCN;

	GetClientRect(&rectClient);

	rectText = rectClient;
	rectText.top  += 2;
	rectText.left += 2;
	rectText.right -= 2;
	rectText.bottom -= 40;

	if (GetDlgItem(IDC_EDIT))
		GetDlgItem(IDC_EDIT)->MoveWindow(&rectText);


	rectCN = rectClient;
	rectCN.right -= 2;
	rectCN.left = rectCN.right - 80;
	rectCN.bottom -= 4;
	rectCN.top = rectCN.bottom - 25;

	if (GetDlgItem(IDCANCEL))
		GetDlgItem(IDCANCEL)->MoveWindow(&rectCN);

	rectOK = rectCN;
	rectOK.right = rectOK.left - 4;
	rectOK.left  = rectOK.right - 80;

	if (GetDlgItem(IDOK))
		GetDlgItem(IDOK)->MoveWindow(&rectOK);

}

void KGValueEditStringDlg::OnBnClickedOk()
{
	UpdateData();

	USER_ITEM_DATA itemData;
	int nStringLength = 0;

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_stringValue.Replace("\r\n", "\\n");
	m_stringValue.Trim();
	nStringLength = m_stringValue.GetLength();

	if (m_stringValue.GetLength() <= itemData.nLength && m_stringValue.GetLength() >= 0)
	{
		strncpy_s(
			itemData.szPropertyValue, m_stringValue.GetBuffer(),
			sizeof(itemData.szPropertyValue)
		);
		itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] ='\0';

		m_pListItem->SetStructData(&itemData);

		m_pTabFile->WriteString(
			itemData.nRowsIndexInFile, itemData.szPropertyName, itemData.szPropertyValue
		);
		if (m_pParant)
			m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

	}
	else if (m_stringValue.GetLength() < 1 && itemData.nNeedFlag)
	{
		CString szValue;
		szValue.Format("%d", itemData.nLength);
		szValue = _T("此项为必填值，不允许为空!! ");
		::MessageBox(this->m_hWnd, szValue.GetBuffer(), "Error", MB_OK);

		return;
	}
	else if (m_stringValue.GetLength() > itemData.nLength)
	{
		CString szValue;
		szValue.Format("%d", itemData.nLength);
		szValue = _T("写入的字符传长度必须小于 ") + szValue + " !";
		::MessageBox(this->m_hWnd, szValue.GetBuffer(), "Error", MB_OK);

		return;
	}

	OnOK();
}

