////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditMapNpcDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:27:18
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditMapNpcDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditMapNpcDlg, CDialog)
KGValueEditMapNpcDlg::KGValueEditMapNpcDlg(
	KGPropertyListCtrl* pList, int nItem, ITabFile* pFile,
	int nCurRow, CWnd* pParent
) : CDialog(KGValueEditMapNpcDlg::IDD, pParent)
{
	m_pTabFile = pFile;
	m_pList    = pList;
	m_nItem    = nItem;
	m_nCurRow  = nCurRow;
}

KGValueEditMapNpcDlg::~KGValueEditMapNpcDlg()
{
}

void KGValueEditMapNpcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL KGValueEditMapNpcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString szTitle;
/*
	ITEM_INFO info = m_pList->GetInfo(m_nItem);

	GetWindowText(szTitle);
	SetWindowText(szTitle + "_" + info.szPate);
	*/
	/*
	m_comboxValue.AddString(info.szStateTrue);
	m_comboxValue.AddString(info.szStateFalse);

	if (info.szValue == "1")
	{
		m_comboxValue.SelectString(0, info.szStateTrue);
	}
	else
	{
		m_comboxValue.SelectString(0, info.szStateFalse);
	}

	if (!info.nNeedFlag)
	{
		GetDlgItem(IDC_VALUE)->EnableWindow(false);
	}
	*/
	UpdateData(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(KGValueEditMapNpcDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void KGValueEditMapNpcDlg::OnBnClickedOk()
{
	UpdateData();
	//ITEM_INFO info = m_pList->GetInfo(m_nItem);

	CString szValue;
//	int     nValue;
	//m_comboxValue.GetWindowText(szValue);
/*
	if (szValue == info.szStateTrue)
	{
		nValue = 1;
	}
	else
	{
		nValue = 0;
	}

	m_pList->SetItemText(m_nItem, 1, szValue.GetBuffer());
	nValue ? info.szValue = "1" : info.szValue = "0";
	m_pList->SetInfo(m_nItem, info);
	m_pTabFile->WriteInteger(m_nCurRow, info.szName, nValue);
*/
	OnOK();
}

