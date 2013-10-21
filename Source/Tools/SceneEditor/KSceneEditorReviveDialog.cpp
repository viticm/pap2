// KSceneEditorReviveDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorReviveDialog.h"
#include "KSceneEditorDocLogical.h"

// KSceneEditorReviveDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorReviveDialog, CDialog)

KSceneEditorReviveDialog::KSceneEditorReviveDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorReviveDialog::IDD, pParent)
	, m_szGroupName(_T(""))
	, m_nMin(0)
	, m_nMax(0)
	, m_szSelectedTip(_T(""))
{
	m_bNpcReliveGroup = TRUE;
	m_pDocLogical = NULL;
	m_dwCurrentSelectedID = 0;
}

KSceneEditorReviveDialog::~KSceneEditorReviveDialog()
{
}

void KSceneEditorReviveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GROUPNAME, m_szGroupName);
	DDX_Text(pDX, IDC_EDIT_MIN, m_nMin);
	DDX_Text(pDX, IDC_EDIT_MAX, m_nMax);
	DDX_Control(pDX, IDC_LIST2, m_List);
	DDX_Control(pDX, IDC_CHECK_ISRANDOM, m_Random);
	DDX_Text(pDX, IDC_STATIC_SELECTION, m_szSelectedTip);
}


BEGIN_MESSAGE_MAP(KSceneEditorReviveDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &KSceneEditorReviveDialog::OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &KSceneEditorReviveDialog::OnBnClickedButtonDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &KSceneEditorReviveDialog::OnLvnItemchangedList2)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &KSceneEditorReviveDialog::OnHdnItemclickList2)
	ON_EN_CHANGE(IDC_EDIT_GROUPNAME, &KSceneEditorReviveDialog::OnEnChangeEditGroupname)
	ON_EN_CHANGE(IDC_EDIT_MIN, &KSceneEditorReviveDialog::OnEnChangeEditMin)
	ON_EN_CHANGE(IDC_EDIT_MAX, &KSceneEditorReviveDialog::OnEnChangeEditMax)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSELECT, &KSceneEditorReviveDialog::OnBnClickedButtonClearselect)
	ON_BN_CLICKED(IDC_CHECK_ISRANDOM, &KSceneEditorReviveDialog::OnBnClickedCheckIsRandom)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KSceneEditorReviveDialog message handlers

void KSceneEditorReviveDialog::OnBnClickedButtonNew()
{
	UpdateData(TRUE);
	if(m_pDocLogical)
	{
		DWORD dwNewID = 0;
		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%s",m_szGroupName);
		if (m_bNpcReliveGroup)
		{
			m_pDocLogical->NewOneNpcReviveGroup(dwNewID,szName,m_nMin,m_nMax, m_Random.GetCheck());
		}
		else
		{
			m_pDocLogical->NewOneDoodadReviveGroup(dwNewID,szName,m_nMin,m_nMax, m_Random.GetCheck());
		}
	}
	FillList();
}

void KSceneEditorReviveDialog::OnBnClickedButtonDelete()
{
	if(m_pDocLogical)
	{
		if (m_bNpcReliveGroup)
		{
			m_pDocLogical->DeleteNpcReviveGroup(m_dwCurrentSelectedID);
		}
		else
		{
			m_pDocLogical->DeleteDoodadReviveGroup(m_dwCurrentSelectedID);
		}
	}

	m_dwCurrentSelectedID = 0;

	FillList();

}

HRESULT KSceneEditorReviveDialog::FillList()
{
	m_List.DeleteAllItems();
	
	if(!m_pDocLogical)
		return E_FAIL;

	map<DWORD,KSceneEditorDocLogical::ReviveGroup>* pMap = NULL;
	if(m_bNpcReliveGroup)
	{
		pMap = &m_pDocLogical->m_mapNpcReviveGroup;
	}
	else
	{
		pMap = &m_pDocLogical->m_mapDoodadReviveGroup;
	}

	int K = 0;
	map<DWORD,KSceneEditorDocLogical::ReviveGroup>::iterator i = pMap->begin();
	map<DWORD,KSceneEditorDocLogical::ReviveGroup>::iterator iend = pMap->end();
	while (i!=iend)
	{
		KSceneEditorDocLogical::ReviveGroup& Group = i->second;

		TCHAR szName[MAX_PATH];

		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem = K;      
		lvItem.iImage   = 0;
		lvItem.pszText = szName; 

		{
			wsprintf(szName,"%d",Group.dwGroupID);
			lvItem.iSubItem = 0;
			int index = m_List.InsertItem( &lvItem );
			m_List.SetItemData(index,Group.dwGroupID); 
		}
		{
			wsprintf(szName,"%s",Group.szName.c_str());    
			lvItem.iSubItem = 1;
			m_List.SetItem(&lvItem );
		}
		{
			wsprintf(szName,"%d",Group.nMinNpcCount);
			lvItem.iSubItem = 2;
			m_List.SetItem(&lvItem );
		}
		{
			wsprintf(szName,"%d",Group.nMaxNpcCount);
			lvItem.iSubItem = 3;
			m_List.SetItem(&lvItem );
		}
		{
			wsprintf(szName, "%d", Group.nRandom);
			lvItem.iSubItem = 4;
			m_List.SetItem(&lvItem);
		}
		i++;
		K++;
	}
	
	return S_OK;
}

BOOL KSceneEditorReviveDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_List.InsertColumn(0, _T("ID"  ),	LVCFMT_CENTER, 40);
	m_List.InsertColumn(1, _T("Name"),	LVCFMT_CENTER, 90);
	m_List.InsertColumn(2, _T("Min" ),	LVCFMT_CENTER, 40);
	m_List.InsertColumn(3, _T("Max" ),	LVCFMT_CENTER, 40);
	m_List.InsertColumn(4, _T("IsRandom" ),	LVCFMT_CENTER, 80);
	m_List.SetExtendedStyle(
		m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
		);
	m_Random.SetCheck(false);
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditorReviveDialog::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_pDocLogical)
		return ;	
	int nRandom;
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nIndex = m_List.GetNextSelectedItem(pos);

		m_dwCurrentSelectedID = (DWORD)m_List.GetItemData(nIndex);
	}

	if(m_dwCurrentSelectedID)
	{
		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%s",m_szGroupName);

		if(m_bNpcReliveGroup)
		{
			m_pDocLogical->GetNpcReviveGroup(m_dwCurrentSelectedID,szName,m_nMin,m_nMax, nRandom);
		}
		else
		{
			m_pDocLogical->GetDoodadReviveGroup(m_dwCurrentSelectedID,szName,m_nMin,m_nMax, nRandom);
		}
		m_szGroupName.Format("%s",szName);

		m_szSelectedTip.Format("当前选择的 ReliveGroup ID 为 %d",m_dwCurrentSelectedID);
		m_Random.SetCheck(nRandom);
		UpdateData(FALSE);
	}
	else
	{
		m_szSelectedTip.Format("当前选择的 ReliveGroup ID 为空");
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void KSceneEditorReviveDialog::OnHdnItemclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{


	*pResult = 0;
}

HRESULT KSceneEditorReviveDialog::ModifyGroup()
{
	if(!m_pDocLogical)
		return E_FAIL;

	UpdateData(TRUE);

	if(m_dwCurrentSelectedID)
	{
		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%s",m_szGroupName);

		if(m_bNpcReliveGroup)
		{
			m_pDocLogical->SetNpcReviveGroup(m_dwCurrentSelectedID,szName,m_nMin,m_nMax, m_Random.GetCheck());
		}
		else
		{
			m_pDocLogical->SetDoodadReviveGroup(m_dwCurrentSelectedID,szName,m_nMin,m_nMax, m_Random.GetCheck());
		}
	}
	FillList();
	return S_OK;
}
void KSceneEditorReviveDialog::OnEnChangeEditGroupname()
{
	ModifyGroup();
}

void KSceneEditorReviveDialog::OnEnChangeEditMin()
{
	ModifyGroup();
}

void KSceneEditorReviveDialog::OnEnChangeEditMax()
{
	ModifyGroup();
}

void KSceneEditorReviveDialog::OnBnClickedButtonClearselect()
{
	m_dwCurrentSelectedID = 0;
	m_szSelectedTip.Format("当前选择的 ReliveGroup ID 为空");
	UpdateData(FALSE);
}

void KSceneEditorReviveDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	FillList();
}

void KSceneEditorReviveDialog::SetCurSelectedID(DWORD dwSelectedID)
{
	m_dwCurrentSelectedID = dwSelectedID;
}

void KSceneEditorReviveDialog::OnBnClickedCheckIsRandom()
{
	ModifyGroup();
}