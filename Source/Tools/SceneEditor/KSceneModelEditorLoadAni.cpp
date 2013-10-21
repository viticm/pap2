// KSceneModelEditorLoadAni.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneModelEditorLoadAni.h"
#include "MeshEditorView.h"
#define ANIMATION_NODE_ID 0xffffffff

// KSceneModelEditorLoadAni dialog

IMPLEMENT_DYNAMIC(KSceneModelEditorLoadAni, CDialog)

KSceneModelEditorLoadAni::KSceneModelEditorLoadAni(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorLoadAni::IDD, pParent)
{
	m_strCurDir = "";
	m_strExt = ".ani";
	m_strPathName = "";
	m_pModel = NULL;
	m_pParentView = NULL;
}

KSceneModelEditorLoadAni::~KSceneModelEditorLoadAni()
{
}

void KSceneModelEditorLoadAni::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_LIST_ANI_NOTYPE,m_listAni);
	DDX_Text(pDX,IDC_EDIT_PATHNAME,m_strPathName);
}


BEGIN_MESSAGE_MAP(KSceneModelEditorLoadAni, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &KSceneModelEditorLoadAni::OnBnClickedButtonAdd)
	ON_LBN_SELCHANGE(IDC_LIST_ANI_NOTYPE, &KSceneModelEditorLoadAni::OnLbnSelchangeListAniNotype)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &KSceneModelEditorLoadAni::OnNMDblclkTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &KSceneModelEditorLoadAni::OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE1, &KSceneModelEditorLoadAni::OnTvnKeydownTree1)
END_MESSAGE_MAP()

BOOL KSceneModelEditorLoadAni::OnInitDialog()
{
	CDialog::OnInitDialog();

	CreateTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
// KSceneModelEditorLoadAni message handlers
void KSceneModelEditorLoadAni::CreateTree()
{
	TCHAR strRootName[MAX_PATH];
	TCHAR strIniPathName[MAX_PATH];
	IIniFile* pIniFile = NULL;
	
	TCHAR strNodeName[MAX_PATH];
	TCHAR strKeyName[MAX_PATH];
	int nNode = 0;
	int nTypeNum = 0;
	int nID = 0;
	
	m_Tree.DeleteAllItems();
	sprintf(strIniPathName, "%sData\\public\\LoadAni.ini", g_szDefWorkDirectory);
	
	pIniFile = g_OpenIniFile(strIniPathName, false, false);
	KG_PROCESS_ERROR(pIniFile);

	pIniFile->GetString("Root", "Name","", strRootName,sizeof(strRootName));
	pIniFile->GetInteger("Root", "nNodeNum", 0, &nNode);
	pIniFile->GetInteger("Root", "ID", 0, &nID);
	HTREEITEM hItem = m_Tree.InsertItem(strRootName,0,0,TVI_ROOT);
	m_ID2Item[nID] = hItem;
	m_Tree.SetItemData(hItem, (DWORD_PTR)nID);
	for(int i = 0; i < nNode ;i++)
	{
		sprintf(strKeyName, "Node%d", i);
		pIniFile->GetString("Root", strKeyName, "", strNodeName, sizeof(strNodeName));
		CreateTypeNode(pIniFile, strNodeName, hItem);
	}
	FillTree();
Exit0:
	SAFE_RELEASE(pIniFile);
}

void KSceneModelEditorLoadAni::CreateTypeNode(IIniFile* pIniFile,TCHAR* pTitle,HTREEITEM ParentNode)
{
	TCHAR strName[MAX_PATH];
	TCHAR strKeyName[MAX_PATH];
	int nNode = 0;
	int nID = 0;
	HTREEITEM hItem = m_Tree.InsertItem(pTitle,0,0,ParentNode);
	pIniFile->GetInteger(pTitle, "nNodeNum",0,&nNode);
	pIniFile->GetInteger(pTitle, "ID", 0, &nID);
	m_ID2Item[nID] = hItem;
	m_Tree.SetItemData(hItem, (DWORD_PTR)nID);
	for(int i=0; i< nNode ;i++)
	{
		sprintf(strKeyName, "Node%d",i);
		pIniFile->GetString(pTitle, strKeyName, "", strName, sizeof(strName));
		CreateTypeNode(pIniFile, strName, hItem);
	}
}

void KSceneModelEditorLoadAni::ClearAniList()
{
	while(m_listAni.GetCount())
	{
		m_listAni.DeleteString(0);
	}
}

void KSceneModelEditorLoadAni::FillTree()
{
	ClearAniList();
	BOOL bWorking = TRUE;
	CFileFind Find;
	TCHAR strFiles[MAX_PATH];
	TCHAR strIniPathName[MAX_PATH];
	IIniFile* pIniFile = NULL;
	int nID = -1;
	sprintf_s(strFiles,MAX_PATH,"%s*%s",m_strCurDir,m_strExt);
	KG_PROCESS_ERROR(Find.FindFile(strFiles));

	sprintf(strIniPathName, "%sTypeTable.ini", m_strCurDir);
	pIniFile = g_OpenIniFile(strIniPathName, false, false);
	KG_PROCESS_ERROR(pIniFile);
	while (bWorking)
	{
		bWorking = Find.FindNextFile();
		CString strAniFileName = Find.GetFileTitle();//Find.GetFileName();
		if(pIniFile)
		{	
			pIniFile->GetInteger("Type", strAniFileName, -1, &nID);
			if(nID != -1 && m_ID2Item.count(nID))
			{
				CString TypeName;
				HTREEITEM hItem = m_ID2Item[nID];
				HTREEITEM hInsert = m_Tree.InsertItem(strAniFileName, 0, 0,hItem);
				m_Tree.SetItemData(hInsert, ANIMATION_NODE_ID);
			}
			else 
			{
				//如果没有符合的ID的接点
				m_listAni.AddString(strAniFileName);
			}
		}
		else
		{
			//如果没有相应的配置文件
			m_listAni.AddString(strAniFileName);
		}
	};
	Find.Close();
Exit0:
	SAFE_RELEASE(pIniFile);
}

void KSceneModelEditorLoadAni::OnBnClickedButtonAdd()
{
	HTREEITEM hItem = NULL;
	HTREEITEM hItemChild = NULL;
	DWORD dwID = 0;
	int nType=0;
	IIniFile* pIniFile = NULL;
	
	int nSelCount = m_listAni.GetSelCount();
	CArray<int,int> aryListBoxSel;
	CString strAnimationName;

	aryListBoxSel.SetSize(nSelCount);
	m_listAni.GetSelItems(nSelCount, aryListBoxSel.GetData()); 

	hItem = m_Tree.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	hItemChild = m_Tree.GetChildItem(hItem);
	while (hItemChild)
	{
		KG_PROCESS_SUCCESS(static_cast<DWORD>(m_Tree.GetItemData(hItemChild)) != ANIMATION_NODE_ID);
		hItemChild = m_Tree.GetNextSiblingItem(hItemChild);
	}

	dwID = static_cast<DWORD>(m_Tree.GetItemData(hItem));
	
	while (dwID == ANIMATION_NODE_ID)
	{
		hItem = m_Tree.GetParentItem(hItem);
		dwID = static_cast<DWORD>(m_Tree.GetItemData(hItem));
	}
	
	for (int i = nSelCount - 1; i >= 0; i--)
	{
		m_listAni.GetText(aryListBoxSel[i], strAnimationName);
		hItemChild = m_Tree.InsertItem(strAnimationName, 0, 0, hItem);
		m_Tree.SetItemData(hItemChild, ANIMATION_NODE_ID);
		m_Tree.Expand(hItem, TVE_EXPAND);
		m_listAni.DeleteString(aryListBoxSel[i]);
	}
Exit1:
Exit0:
	;
}

HTREEITEM KSceneModelEditorLoadAni::FindTreeItembyString(CString strText, HTREEITEM hStartItem)
{
	HTREEITEM FindNode = NULL;

	//没有数据返回NULL
	int nCount = m_Tree.GetCount();
	if ( !nCount ) return NULL;

	if (!hStartItem)
		hStartItem = m_Tree.GetRootItem();

	FindNode = hStartItem;

	CString ItemText =m_Tree.GetItemText(FindNode);
	int i= ItemText.CompareNoCase(strText);

	if( i == 0)
		return FindNode;

	HTREEITEM BrotherNode = FindNode;
	if(m_Tree.ItemHasChildren(FindNode))
	{
		FindNode = m_Tree.GetChildItem(FindNode);
		FindNode = FindTreeItembyString(strText, FindNode); 
		KG_PROCESS_SUCCESS(FindNode);
	}

	FindNode = m_Tree.GetNextSiblingItem(BrotherNode);
	KG_PROCESS_ERROR(FindNode);

	FindNode = FindTreeItembyString(strText, FindNode);
	KG_PROCESS_SUCCESS(FindNode);

Exit0:
	return NULL;
Exit1:
	return FindNode;
}

void KSceneModelEditorLoadAni::OnLbnSelchangeListAniNotype()
{
	TCHAR strFullPath[MAX_PATH];
	CString strFileName;
	int nCurSel = m_listAni.GetCurSel();
	KG_PROCESS_ERROR(m_pModel);
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	m_listAni.GetText(nCurSel, strFileName);
	sprintf_s(strFullPath, "%s%s%s", m_strCurDir, strFileName, m_strExt);

	m_pModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE,
		strFullPath,
		1.0f,
		0,
		NULL,
		NULL,
		ANICTL_PRIMARY);

	m_strPathName = strFullPath;
	UpdateData(FALSE);
Exit0:
	;
}

void KSceneModelEditorLoadAni::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	BOOL bHasChildren = m_Tree.ItemHasChildren(hItem);
	if(hItem && !bHasChildren)
	{
		CString ItemText =m_Tree.GetItemText(hItem);
		m_strPathName = m_strCurDir + ItemText + m_strExt;
		if (m_pModel)
		{
			m_pModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE,
				m_strPathName,
				1.0f,
				0,
				NULL,
				NULL,
				ANICTL_PRIMARY);
		}
		UpdateData(FALSE);
		OnOK();
	}

}

void KSceneModelEditorLoadAni::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	BOOL bHasChildren = m_Tree.ItemHasChildren(hItem);
	if(hItem && !bHasChildren)
	{
		CString ItemText = m_Tree.GetItemText(hItem);
		m_strPathName = m_strCurDir + ItemText + m_strExt;
		if (m_pModel)
		{
			m_pModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE,
				m_strPathName,
				1.0f,
				0,
				NULL,
				NULL,
				ANICTL_PRIMARY);
		}
		UpdateData(FALSE);
	}
}

void KSceneModelEditorLoadAni::OnOK()
{
}

void KSceneModelEditorLoadAni::OnCancel()
{
	TCHAR strIniPathName[MAX_PATH];
	map<int, HTREEITEM>::iterator it = m_ID2Item.begin();
	map<int, HTREEITEM>::iterator itEnd = m_ID2Item.end();
	sprintf(strIniPathName, "%sTypeTable.ini", m_strCurDir);
	IIniFile *pIniFile = g_OpenIniFile(strIniPathName, false, true);
	KG_PROCESS_ERROR(pIniFile);

	pIniFile->Clear();
	while (it != itEnd)
	{
		int nID = it->first;
		HTREEITEM hItem = it->second;
		hItem = m_Tree.GetChildItem(hItem);
		while (hItem)
		{
			if (static_cast<DWORD>(m_Tree.GetItemData(hItem)) == ANIMATION_NODE_ID)
			{
				CString strAnimationName = m_Tree.GetItemText(hItem);
				pIniFile->WriteInteger("Type", strAnimationName, nID);
			}
			hItem = m_Tree.GetNextSiblingItem(hItem);
		}
		it++;
	}
	pIniFile->Save(strIniPathName);
Exit0:
	SAFE_RELEASE(pIniFile);
	m_pParentView->m_lpDialogSkillAnimation = NULL;
	DestroyWindow();
}

void KSceneModelEditorLoadAni::OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	
	switch(pTVKeyDown->wVKey)
	{
	case VK_DELETE:
		{
			HTREEITEM hItem = m_Tree.GetSelectedItem();
			CString strAnimationName;
			KG_PROCESS_SUCCESS(static_cast<DWORD_PTR>(m_Tree.GetItemData(hItem) != ANIMATION_NODE_ID));
			
			strAnimationName = m_Tree.GetItemText(hItem);
			m_listAni.AddString(strAnimationName);
			m_Tree.DeleteItem(hItem);
			break;
		}
	}
Exit1:
	*pResult = 0;
}

void KSceneModelEditorLoadAni::PostNcDestroy()
{
	delete this;
}