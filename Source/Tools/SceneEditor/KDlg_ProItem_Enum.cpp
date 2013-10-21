// KDlg_ProItem_Enum.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Enum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Enum dialog
namespace PropertyBase
{

KDlg_ProItem_Enum::KDlg_ProItem_Enum(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Enum::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Enum)
	m_Value = -1;
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_Enum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Enum)
	DDX_Control(pDX, IDC_COMBO1, m_Combo);
	DDX_CBIndex(pDX, IDC_COMBO1, m_Value);
	DDX_Text(pDX, IDC_SNAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Enum, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Enum)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Enum message handlers

BOOL KDlg_ProItem_Enum::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if(m_lpStore)
	//{
	//	m_Name = m_lpProperty->scName.c_str();
	//	int* pValue = (int*)m_lpProperty->Address;

	//	for(DWORD i=0;i<m_lpStore->vecEnumItem.size();i++)
	//	{
	//		PropertyBase::_VarProperty::_EnumItem* pItem = &m_lpStore->vecEnumItem[i];
	//		int id = m_Combo.AddString((pItem->Text.c_str()));
	//		m_Combo.SetItemData(id,pItem->Value);
	//		
	//		if(pItem->Value == *pValue)
	//		{
	//			m_Value = i;
	//		}
	//	}

	//	UpdateData(FALSE);
	//}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_Enum::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//RefreshData();

}
}