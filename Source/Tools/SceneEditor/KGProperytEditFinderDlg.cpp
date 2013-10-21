// KGProperytEditFinderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGProperytEditFinderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////



// KGProperytEditFinderDlg dialog

IMPLEMENT_DYNAMIC(KGProperytEditFinderDlg, CDialog)

KGProperytEditFinderDlg::KGProperytEditFinderDlg(CWnd* pParent, LPKGLISTITEM pItem)
	: CDialog(KGProperytEditFinderDlg::IDD, pParent)
{
	m_pListFrame = (KGPropertyListCtrl*)pParent;
	m_pListItem  = pItem;
	m_nBeginFindFLag = true;
}

KGProperytEditFinderDlg::~KGProperytEditFinderDlg()
{
}

void KGProperytEditFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_comboFindString);
}


BEGIN_MESSAGE_MAP(KGProperytEditFinderDlg, CDialog)
	ON_BN_CLICKED(ID_FIND, &KGProperytEditFinderDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO, &KGProperytEditFinderDlg::OnCbnSelchangeCombo)
	ON_CBN_EDITCHANGE(IDC_COMBO, &KGProperytEditFinderDlg::OnCbnEditchangeCombo)
END_MESSAGE_MAP()

void KGProperytEditFinderDlg::OnBnClickedOk()
{
	CString szFindText = "";
	CSize size;
	CRect rect;
	int nItemIndex;
	INT_PTR nStringCount = 0;
	GetDlgItem(ID_FIND)->SetWindowText(_T("查找下一个"));

	m_comboFindString.GetWindowText(szFindText);

	if (szFindText != "" && m_nBeginFindFLag && m_comboFindString.FindString(0, szFindText) == -1)
	{
		nStringCount = m_pListFrame->m_arrayFindString.GetCount();
		if (nStringCount > 10)
		{
			int nIndex = m_comboFindString.FindString(0,
				m_pListFrame->m_arrayFindString.GetAt(nStringCount - 1)
			);
			m_comboFindString.DeleteString(nIndex);
			m_pListFrame->m_arrayFindString.RemoveAt(nStringCount - 1);
		}
		m_comboFindString.AddString(szFindText);
		m_pListFrame->m_arrayFindString.Add(szFindText);
	}

	if (m_nBeginFindFLag)
	{
		m_pListFrame->FindItemByText(szFindText);
		m_nBeginFindFLag = false;
	}

	LPKGLISTITEM pFindItem = m_pListFrame->FindNextItem();
	if (pFindItem)
	{
		nItemIndex = m_pListFrame->FindItemPos(pFindItem);
		m_pListFrame->GetItemRect(nItemIndex, &rect, true);
		m_pListFrame->SetItemState(
			nItemIndex,
			LVIS_SELECTED | LVIS_FOCUSED,
			LVIS_SELECTED | LVIS_FOCUSED
		);
		size.cy = (nItemIndex - m_pListFrame->GetTopIndex()) * rect.Height();
		//if (size.cy >= 1)
		//	size.cy -= 4 * rect.Height() ;
		//else
		//	size.cy += 4 * rect.Height();
		m_pListFrame->Scroll(size);
	}
	else
	{
		::MessageBox(
			this->m_hWnd, _T("==查找完成=="), _T("查找完成"), MB_OK | MB_ICONINFORMATION
		);
	}

}

BOOL KGProperytEditFinderDlg::OnInitDialog()
{
	INT_PTR nFindStringCount = m_pListFrame->m_arrayFindString.GetCount();
	INT_PTR i = 0;
	CDialog::OnInitDialog();
	for (i = 0; i < nFindStringCount; i++)
	{
		CString szString = m_pListFrame->m_arrayFindString.GetAt(i);
		m_comboFindString.AddString(szString);
	}
	return true;
}

void KGProperytEditFinderDlg::OnCbnSelchangeCombo()
{
	m_nBeginFindFLag = true;
}

void KGProperytEditFinderDlg::OnCbnEditchangeCombo()
{
	m_nBeginFindFLag = true;
}
