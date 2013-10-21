#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditFloatDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditFloatDlg, CDialog)

KGValueEditFloatDlg::KGValueEditFloatDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem,
	SHOW_TYPE eType /* = FT_FLOAT */, CWnd* pParent /* = NULL  */
) : CDialog(KGValueEditFloatDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
	m_eShowType  = eType;
}

KGValueEditFloatDlg::~KGValueEditFloatDlg()
{
}

void KGValueEditFloatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN, m_spinButton);
}

BOOL KGValueEditFloatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szTitle;
	CString szBound;
	CString szValue;

	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	GetWindowText(szTitle);

	/*m_spinButton.SetBuddy(GetDlgItem(IDC_VALUE));*/
	m_spinButton.SetRange32(itemData.nMin, itemData.nMax);

	m_nPecentageValue = (int)(atof(itemData.szPropertyValue) * 100);

	szValue.Format(_T("%d%%"), m_nPecentageValue);
	GetDlgItem(IDC_VALUE)->SetWindowText(szValue);

	szBound.Format(" [ÊýÖµ·¶Î§ %d%% ~ %d%% ]", itemData.nMin, itemData.nMax);

	SetWindowText(
		szTitle + "_" + m_pListItem->arryItemText.GetAt(0).szItemText
		+ szBound
	);

	UpdateData(false);

	return TRUE;
}


BEGIN_MESSAGE_MAP(KGValueEditFloatDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT, &KGValueEditFloatDlg::OnEnChangeValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, &KGValueEditFloatDlg::OnDeltaposSpin)
END_MESSAGE_MAP()

void KGValueEditFloatDlg::OnBnClickedOk()
{
	UpdateData();
	USER_ITEM_DATA itemData;
	CString szEditText = _T("");
	CString szValue;
	float	fValue = 0;

	GetDlgItem(IDC_VALUE)->GetWindowText(szEditText);
	szEditText.Replace(_T("%"), _T(""));
	fValue = (float)atoi(szEditText.GetBuffer()) / 100;

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	if (fValue < itemData.nMin * 0.01f)
	{
		fValue = itemData.nMin * 0.01f;
	}
	if (fValue > itemData.nMax * 0.01f)
	{
		fValue = itemData.nMax * 0.01f;
	}

	szValue.Format("%f", fValue);

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

void KGValueEditFloatDlg::OnEnChangeValue()
{

}

void KGValueEditFloatDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CString szValue = _T("");

	m_nPecentageValue += pNMUpDown->iDelta;

	//if (m_nPecentageValue > 100)
	//	m_nPecentageValue = 100;
	if (m_nPecentageValue < 0)
		m_nPecentageValue = 0;

	szValue.Format(_T("%d%%"), m_nPecentageValue);
	GetDlgItem(IDC_VALUE)->SetWindowText(szValue);

	*pResult = 0;
}
