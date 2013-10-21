// KSceneEditorRandomGroupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorRandomGroupDialog.h"
#include "KNpcTemplateList.h"
#include "KSO3World.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "IEEditor.h"
#include "IEKG3DSceneObjectPlacementTool.h"
#include "KSceneEditorDocLogical.h"

#define TYPE_GROUP     1
#define TYPE_GROUP_NPC 2
extern IKSO3RepresentHandler* g_pRepresentHandler;
// KSceneEditorRandomGroupDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorRandomGroupDialog, CDialog)

KSceneEditorRandomGroupDialog::KSceneEditorRandomGroupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorRandomGroupDialog::IDD, pParent)
	, m_szGroupName(_T(""))
	, m_szCurrentSelectedGroup(_T(""))
{
	m_lpSceneSceneEditor = NULL;
	m_pDocLogical = NULL;
	m_dwSelectedGroupID = 0;
	m_nCurrentSelectedNpcTempleteID = -1;
}

KSceneEditorRandomGroupDialog::~KSceneEditorRandomGroupDialog()
{
}

void KSceneEditorRandomGroupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_GROUP, m_TreeGroup);
	DDX_Control(pDX, IDC_TREE_NPC, m_TreeNpc);
	DDX_Text(pDX, IDC_EDIT_GROUPNAME, m_szGroupName);
	DDX_Text(pDX, IDC_STATIC_SELECTEDGROUP, m_szCurrentSelectedGroup);
}


BEGIN_MESSAGE_MAP(KSceneEditorRandomGroupDialog, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GROUP, &KSceneEditorRandomGroupDialog::OnTvnSelchangedTreeGroup)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneEditorRandomGroupDialog::OnTvnSelchangedTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_ADDNPC, &KSceneEditorRandomGroupDialog::OnBnClickedButtonAddnpc)
	ON_BN_CLICKED(IDC_BUTTON_REMOVENPC, &KSceneEditorRandomGroupDialog::OnBnClickedButtonRemovenpc)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ADDGROUP, &KSceneEditorRandomGroupDialog::OnBnClickedButtonAddgroup)
	ON_EN_CHANGE(IDC_EDIT_GROUPNAME, &KSceneEditorRandomGroupDialog::OnEnChangeEditGroupname)
	ON_BN_CLICKED(IDC_BUTTON_DELETEGROUP, &KSceneEditorRandomGroupDialog::OnBnClickedButtonDeletegroup)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NPC, &KSceneEditorRandomGroupDialog::OnNMDblclkTreeNpc)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_GROUP, &KSceneEditorRandomGroupDialog::OnNMDblclkTreeGroup)
END_MESSAGE_MAP()


// KSceneEditorRandomGroupDialog message handlers

void KSceneEditorRandomGroupDialog::OnTvnSelchangedTreeGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeGroup.GetSelectedItem();
	int nImage= 0;
	m_TreeGroup.GetItemImage(hItem,nImage,nImage);
	if (nImage == TYPE_GROUP)
	{
		m_dwSelectedGroupID = (DWORD)m_TreeGroup.GetItemData(hItem);
	}

	if(m_dwSelectedGroupID)
	{
		TCHAR szName[MAX_PATH];
		vector<DWORD>vecNPC;
		m_pDocLogical->GetRandomGroup(m_dwSelectedGroupID,szName,&vecNPC);

		m_szGroupName.Format("%s",szName);
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void KSceneEditorRandomGroupDialog::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	DWORD dwTemplateID = 0;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	KGLOG_PROCESS_ERROR(m_lpSceneSceneEditor);

	{
		HTREEITEM hItem = m_TreeNpc.GetSelectedItem();
		KG_PROCESS_ERROR(hItem);
		int nType = 0;
		m_TreeNpc.GetItemImage(hItem,nType,nType);

		KG_PROCESS_ERROR(nType == TYPE_GROUP_NPC);
		
		dwTemplateID = (DWORD)m_TreeNpc.GetItemData(hItem);
		CString npcname = m_TreeNpc.GetItemText(hItem);

		const KNpcTemplate* pcNpcTempate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(dwTemplateID);
		KGLOG_PROCESS_ERROR(pcNpcTempate);
	}


	m_nCurrentSelectedNpcTempleteID = dwTemplateID;
	return;
Exit0:

	m_nCurrentSelectedNpcTempleteID = -1;
}

void KSceneEditorRandomGroupDialog::OnBnClickedButtonAddnpc()
{
	if((m_nCurrentSelectedNpcTempleteID!=-1)&&(m_dwSelectedGroupID))
	{
		TCHAR szName[MAX_PATH];
		vector<DWORD>vecNPC;
		m_pDocLogical->GetRandomGroup(m_dwSelectedGroupID,szName,&vecNPC);

		vecNPC.push_back(m_nCurrentSelectedNpcTempleteID);

		m_pDocLogical->SetRandomGroup(m_dwSelectedGroupID,szName,&vecNPC);

		UpdataGroup(m_dwSelectedGroupID);
	}
}

void KSceneEditorRandomGroupDialog::OnBnClickedButtonRemovenpc()
{
	HTREEITEM hItem = m_TreeGroup.GetSelectedItem();
	if(!hItem)
		return ;
	int nImage = 0;
	m_TreeGroup.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case TYPE_GROUP_NPC:
		{
			DWORD dwNPCIndex= (DWORD)m_TreeGroup.GetItemData(hItem);

			HTREEITEM hGroup = m_TreeGroup.GetParentItem(hItem);
			DWORD dwGroupID = (DWORD)m_TreeGroup.GetItemData(hGroup);

			TCHAR szName[MAX_PATH];
			vector<DWORD>vecNPC;
			m_pDocLogical->GetRandomGroup(dwGroupID,szName,&vecNPC);

			
			DWORD K = 0;
			vector<DWORD>::iterator i = vecNPC.begin();
			vector<DWORD>::iterator iend = vecNPC.end();
			while (i!=iend)
			{
				if(K==dwNPCIndex)
				{
					vecNPC.erase(i);
					break;
				}
				i++;
				K++;
			}

			m_pDocLogical->SetRandomGroup(dwGroupID,szName,&vecNPC);

			UpdataGroup(dwGroupID);
		}
		break;
	case TYPE_GROUP:
	    break;
	}
}

void KSceneEditorRandomGroupDialog::FillNpcTree()
{
	HRESULT hRetCode = S_OK;
	if(!m_lpSceneSceneEditor)
		return ;

	if (g_pSO3World == NULL)
		return;

	m_TreeNpc.DeleteAllItems();
	m_mapType.clear();

	TCHAR Name[MAX_PATH];

	HTREEITEM hClass = NULL;
	int nCount = g_pSO3World->m_Settings.m_NpcTemplateList.GetNpcTemplateCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		const KNpcTemplate* pcNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplateByIndex(nIndex);

		if (pcNpcTemplate && pcNpcTemplate->dwTemplateID != 0)
		{
			map<CString,HTREEITEM>::iterator s = m_mapType.find(pcNpcTemplate->szMapName);
			if(s==m_mapType.end())
			{
				//wsprintf(Name,"NPC类型%s",pcNpcTemplate->szMapName);
				hClass = m_TreeNpc.InsertItem(pcNpcTemplate->szMapName,TYPE_GROUP,TYPE_GROUP,NULL,TVI_LAST);
				m_TreeNpc.SetItemData(hClass,0);
				m_mapType[pcNpcTemplate->szMapName] = hClass;
			}
			else
			{
				hClass = s->second;
			}
			HTREEITEM hItem = m_TreeNpc.InsertItem(pcNpcTemplate->szName,TYPE_GROUP_NPC,TYPE_GROUP_NPC,hClass,TVI_LAST);
			m_TreeNpc.SetItemData(hItem, pcNpcTemplate->dwTemplateID);

			//////////////////////////////////////////////////////////////////////////
			DWORD dwCount =0;
			dwCount = _kg_to_smaller_size_cast<DWORD>(m_TreeNpc.GetItemData(hClass));
			dwCount++;
			m_TreeNpc.SetItemData(hClass,dwCount);
			wsprintf(Name,"%s （%d）",pcNpcTemplate->szMapName,dwCount);
			m_TreeNpc.SetItemText(hClass,Name);
		}
	}

};
BOOL KSceneEditorRandomGroupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	FillNpcTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditorRandomGroupDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	FillNpcTree();

	FillRandomGroupTree();
	// TODO: Add your message handler code here
}

void KSceneEditorRandomGroupDialog::FillRandomGroupTree()
{
	if(!m_pDocLogical)
		return;

	if (g_pSO3World == NULL)
		return;

	m_mapGroupItem.clear();
	m_TreeGroup.DeleteAllItems();

	map<DWORD,KSceneEditorDocLogical::RandomGroup>::iterator i = m_pDocLogical->m_mapRandomGroup.begin();
	map<DWORD,KSceneEditorDocLogical::RandomGroup>::iterator iend = m_pDocLogical->m_mapRandomGroup.end();
	while (i!=iend)
	{
		KSceneEditorDocLogical::RandomGroup& Group = i->second;

		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%d %s",Group.dwGroupID,Group.szName.c_str());

		HTREEITEM hGroup = m_TreeGroup.InsertItem(szName,TYPE_GROUP,TYPE_GROUP,TVI_ROOT,TVI_LAST);
		m_TreeGroup.SetItemData(hGroup,Group.dwGroupID);

		m_mapGroupItem[Group.dwGroupID] = hGroup;

		for (size_t s=0;s<Group.vecNpcTempleteID.size();s++)
		{
			DWORD dwNpcID = Group.vecNpcTempleteID[s];
			const KNpcTemplate* pcNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(dwNpcID);
			
			if(pcNpcTemplate)
				wsprintf(szName,"%d %d %s",s,dwNpcID,pcNpcTemplate->szName);
			else
				wsprintf(szName,"%d %d 获取失败的NPC",s,dwNpcID);

			HTREEITEM hItem = m_TreeGroup.InsertItem(szName,TYPE_GROUP_NPC,TYPE_GROUP_NPC,hGroup,TVI_LAST);
			m_TreeGroup.SetItemData(hItem,s);
		}
		i++;
	}

}


void KSceneEditorRandomGroupDialog::OnBnClickedButtonAddgroup()
{
	if(!m_pDocLogical)
		return;

	DWORD dwID = 0;
	m_pDocLogical->NewOneRandomGroup(dwID);

	FillRandomGroupTree();
}

void KSceneEditorRandomGroupDialog::OnEnChangeEditGroupname()
{
	UpdateData(TRUE);

	if(m_dwSelectedGroupID)
	{
		TCHAR szName[MAX_PATH];
		vector<DWORD>vecNPC;
		m_pDocLogical->GetRandomGroup(m_dwSelectedGroupID,szName,&vecNPC);

		wsprintf(szName,"%s",m_szGroupName);
		m_pDocLogical->SetRandomGroup(m_dwSelectedGroupID,szName,&vecNPC);

		FillRandomGroupTree();
	}
}

void KSceneEditorRandomGroupDialog::UpdataGroup(DWORD dwGroupID)
{
	map<DWORD,HTREEITEM>::iterator iFind = m_mapGroupItem.find(dwGroupID);
	if(iFind != m_mapGroupItem.end())
	{
		HTREEITEM hGroup = iFind->second;
		
		HTREEITEM hItem = m_TreeGroup.GetNextItem(hGroup,TVGN_CHILD);
		while (hItem)
		{
			HTREEITEM LastItem  = hItem;
			hItem = m_TreeGroup.GetNextItem(hItem,TVGN_NEXT);
			m_TreeGroup.DeleteItem(LastItem);
		}

		TCHAR szName[MAX_PATH];
		vector<DWORD>vecNPC;
		m_pDocLogical->GetRandomGroup(dwGroupID,szName,&vecNPC);

		for (size_t s=0;s<vecNPC.size();s++)
		{
			DWORD dwNpcID = vecNPC[s];
			const KNpcTemplate* pcNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(dwNpcID);

			if(pcNpcTemplate)
				wsprintf(szName,"%d %d %s",s,dwNpcID,pcNpcTemplate->szName);
			else
				wsprintf(szName,"%d %d 获取失败的NPC",s,dwNpcID);

			HTREEITEM hItem = m_TreeGroup.InsertItem(szName,TYPE_GROUP_NPC,TYPE_GROUP_NPC,hGroup,TVI_LAST);
			m_TreeGroup.SetItemData(hItem,s);
		}
	}
}


void KSceneEditorRandomGroupDialog::OnBnClickedButtonDeletegroup()
{
	if(m_dwSelectedGroupID)
	{
		m_pDocLogical->DeleteRandomGroup(m_dwSelectedGroupID);
		m_dwSelectedGroupID = (DWORD)(-1);

		FillRandomGroupTree();
	}
}

void KSceneEditorRandomGroupDialog::OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->OnBnClickedButtonAddnpc();

	*pResult = 0;
}

void KSceneEditorRandomGroupDialog::OnNMDblclkTreeGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->OnBnClickedButtonRemovenpc();
	*pResult = 0;
}
