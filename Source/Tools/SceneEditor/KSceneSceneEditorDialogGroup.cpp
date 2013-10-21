// KSceneSceneEditorDialogGroup.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogGroup.h"
#include "KSceneEditorDocLogical.h"
#include "IEEditor.h"
#include "KSceneHelperFuncs.h"
#include "Global.h"

#define DOODADGROUPROOT  0
#define NPCGROUPROOT     1
#define DOODADGROUP      2
#define NPCGROUP         3
#define DOODADGROUPREFRESHPOINT 4
#define NPCGROUPREFRESHPOINT    5
#define DOODADGROUPDOODAD       6
#define NPCGROUPNPC       7
#define AIGROUP_ROOT      8
#define AIGROUP           9
#define AIGROUP_NPC       10
#define NPC               11
#define DOODAD            12
#define NPCRANDOMGROUP    13
#define NPCRANDOMGROUPROOT    14
#define NPCAIPARAMETERSGROUP        15
#define NPCAIPARAMETERSGROUPROOT    16
#define PROGRESSGROUPROOT			17
#define PROGRESSGROUPNPCS			18
#define PROGRESSGROUPNPC			19
#define PROGRESSGROUPDOODADS		20
#define PROGRESSGROUPDOODAD			21


IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogGroup, CDialog)

KSceneSceneEditorDialogGroup::KSceneSceneEditorDialogGroup(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogGroup::IDD, pParent)
	, m_nDirection(0)
	, m_nAIArrayID(0)
{
	m_lpSceneSceneEditor = NULL;
	m_lpDocLogical = NULL;

	m_hDoodadRoot = NULL;
	m_hNPCRoot = NULL;
	m_hAIRoot =NULL;
}

KSceneSceneEditorDialogGroup::~KSceneSceneEditorDialogGroup()
{
}

void KSceneSceneEditorDialogGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NPC, m_Tree);
	DDX_Control(pDX, IDC_EDIT_DIRECTION, m_Edit_Direction);
	DDX_Control(pDX, IDC_BUTTON_ADD2, m_Button_AddPoint);
	DDX_Control(pDX, IDC_BUTTON_ADDSELECTEDENTITY, m_Button_AddSelectedEntity);
	DDX_Control(pDX, IDC_EDIT_SETNAME, m_Edit_SetName);
	DDX_Control(pDX, IDC_BUTTON_ADDSELECTEDENTITY2, m_Button_RemoveSelectedEntity);
	DDX_Text(pDX, IDC_EDIT_DIRECTION, m_nDirection);
	DDV_MinMaxInt(pDX, m_nDirection, -1, 256);
	DDX_Control(pDX, IDC_CHECK_KING, m_Check_King);
	DDX_Control(pDX, IDC_COMBO_WAYPOINTSE, m_Combo_Way);
	DDX_Control(pDX, IDC_EDIT_MATRIXFILE, m_Edit_ArrayFile);
	DDX_Control(pDX, IDC_BUTTON_BROWN, m_Button_ArrayBrown);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_Button_ArrayClear);
	DDX_Control(pDX, IDC_BUTTON_FITGROUP, m_Button_FitGroup);
	DDX_Control(pDX, IDC_BUTTON_FITARRAY, m_Button_FitArray);
	DDX_Control(pDX, IDC_EDIT_ARRAYID, m_Edit_AIArrayID);
	DDX_Text(pDX, IDC_EDIT_ARRAYID, m_nAIArrayID);
	DDV_MinMaxInt(pDX, m_nAIArrayID, 0, 10000);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogGroup, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDDOODADGROUP, &KSceneSceneEditorDialogGroup::OnBnClickedButtonAdddoodadgroup)
	ON_BN_CLICKED(IDC_BUTTON_ADDNPCGROUP, &KSceneSceneEditorDialogGroup::OnBnClickedButtonAddnpcgroup)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogGroup::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ADD2, &KSceneSceneEditorDialogGroup::OnBnClickedButtonAdd2)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneSceneEditorDialogGroup::OnTvnSelchangedTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_ADDSELECTEDENTITY, &KSceneSceneEditorDialogGroup::OnBnClickedButtonAddselectedentity)
	ON_EN_CHANGE(IDC_EDIT_DIRECTION, &KSceneSceneEditorDialogGroup::OnEnChangeEditDirection)
	ON_EN_CHANGE(IDC_EDIT_SETNAME, &KSceneSceneEditorDialogGroup::OnEnChangeEditSetname)
	ON_BN_CLICKED(IDC_BUTTON_ADDSELECTEDENTITY2, &KSceneSceneEditorDialogGroup::OnBnClickedButtonAddselectedentity2)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NPC, &KSceneSceneEditorDialogGroup::OnNMDblclkTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &KSceneSceneEditorDialogGroup::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADDAIGROUP, &KSceneSceneEditorDialogGroup::OnBnClickedButtonAddaigroup)
	ON_BN_CLICKED(IDC_CHECK_KING, &KSceneSceneEditorDialogGroup::OnBnClickedCheckKing)
	ON_CBN_SELCHANGE(IDC_COMBO_WAYPOINTSE, &KSceneSceneEditorDialogGroup::OnCbnSelchangeComboWaypointse)
	ON_BN_CLICKED(IDC_BUTTON_BROWN, &KSceneSceneEditorDialogGroup::OnBnClickedButtonBrown)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &KSceneSceneEditorDialogGroup::OnBnClickedButtonClear)
	ON_EN_CHANGE(IDC_EDIT_MATRIXFILE, &KSceneSceneEditorDialogGroup::OnEnChangeEditMatrixfile)
	ON_BN_CLICKED(IDC_BUTTON_FITGROUP, &KSceneSceneEditorDialogGroup::OnBnClickedButtonFitgroup)
	ON_BN_CLICKED(IDC_BUTTON_FITARRAY, &KSceneSceneEditorDialogGroup::OnBnClickedButtonFitarray)
	ON_EN_CHANGE(IDC_EDIT_ARRAYID, &KSceneSceneEditorDialogGroup::OnEnChangeEditArrayid)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogGroup message handlers

void KSceneSceneEditorDialogGroup::OnBnClickedButtonAdddoodadgroup()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	//KSceneEditorDocLogical::DoodadRefreshPointSet* pNewSet = NULL;
	//m_lpDocLogical->AddDoodadOneRefreshPointSet(&pNewSet,"Doodad Refresh Group");

	FillTree();
}


void KSceneSceneEditorDialogGroup::OnBnClickedButtonAddnpcgroup()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	//KSceneEditorDocLogical::NPCRefreshPointSet* pNewSet = NULL;
	//m_lpDocLogical->AddNPCOneRefreshPointSet(&pNewSet,"NPC Refresh Group");

	FillTree();

}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonRefresh()
{
	FillTree();
}

void KSceneSceneEditorDialogGroup::BuildOrFindProgressTreeItems(int progressid, HTREEITEM* hItemNPCs, HTREEITEM* hItemDOODADs)
{
	TCHAR Name[MAX_PATH];
	map<int,HTREEITEM>::iterator iFound = m_mapProgressGroupTreeItem.find(progressid);
	if (iFound == m_mapProgressGroupTreeItem.end())
	{
		wsprintf(Name,"%i",progressid);
		HTREEITEM hItemProgress = m_Tree.InsertItem(Name,PROGRESSGROUPROOT,PROGRESSGROUPROOT,m_hProgressRoot,TVI_LAST);
		m_mapProgressGroupTreeItem[progressid] = hItemProgress;

		*hItemNPCs = m_Tree.InsertItem("NPC",PROGRESSGROUPNPCS,PROGRESSGROUPNPCS,hItemProgress,TVI_LAST);
		*hItemDOODADs = m_Tree.InsertItem("Doodad",PROGRESSGROUPDOODADS,PROGRESSGROUPDOODADS,hItemProgress,TVI_LAST);
	}
	else
	{
		HTREEITEM hItemProgress = iFound->second;
		*hItemNPCs = m_Tree.GetChildItem(hItemProgress);
		*hItemDOODADs = m_Tree.GetNextItem(*hItemNPCs, TVGN_NEXT);
	}
}

void KSceneSceneEditorDialogGroup::FillTree()
{
	m_Tree.DeleteAllItems();

	TCHAR Name[MAX_PATH];

	wsprintf(Name,"Doodad Relive Groups %d",m_lpDocLogical->m_mapDoodadReviveGroup.size());
	m_hDoodadRoot = m_Tree.InsertItem(Name,DOODADGROUPROOT,DOODADGROUPROOT,TVI_ROOT,TVI_LAST);

	wsprintf(Name,"NPC Relive Groups %d",m_lpDocLogical->m_mapNpcReviveGroup.size());
	m_hNPCRoot = m_Tree.InsertItem(Name,NPCGROUPROOT,NPCGROUPROOT,TVI_ROOT,TVI_LAST);


	wsprintf(Name,"Npc Random Groups %d",m_lpDocLogical->m_mapRandomGroup.size());
	m_hNPCRandomRoot = m_Tree.InsertItem(Name,NPCRANDOMGROUPROOT,NPCRANDOMGROUPROOT,TVI_ROOT,TVI_LAST);

	wsprintf(Name,"Npc AI Parameters Groups %d",m_lpDocLogical->m_mapNpcAISceneParameterSet.size());
	m_hNPCAIParameterRoot = m_Tree.InsertItem(Name,NPCAIPARAMETERSGROUPROOT,NPCAIPARAMETERSGROUPROOT,TVI_ROOT,TVI_LAST);

	wsprintf(Name,"AI Groups %d",m_lpDocLogical->m_listAIGroup.size());
	m_hAIRoot = m_Tree.InsertItem(Name,AIGROUP_ROOT,AIGROUP_ROOT,TVI_ROOT,TVI_LAST);

	wsprintf(Name,"副本进度组");
	m_hProgressRoot = m_Tree.InsertItem(Name,PROGRESSGROUPROOT,PROGRESSGROUPROOT,TVI_ROOT,TVI_LAST);

	//////////////////////////////////////////////////////////////////////////
	{
		m_mapDoodadReliveGroupTreeItem.clear();

		map<DWORD,KSceneEditorDocLogical::ReviveGroup>::iterator i = m_lpDocLogical->m_mapDoodadReviveGroup.begin();
		map<DWORD,KSceneEditorDocLogical::ReviveGroup>::iterator iend = m_lpDocLogical->m_mapDoodadReviveGroup.end();
		while (i!=iend)
		{
			DWORD dwID = i->first;
			KSceneEditorDocLogical::ReviveGroup& group = i->second;

			wsprintf(Name,"%s %d",group.szName.c_str(),dwID);
			HTREEITEM hItemSet = m_Tree.InsertItem(Name,DOODADGROUP,DOODADGROUP,m_hDoodadRoot,TVI_LAST);
			m_Tree.SetItemData(hItemSet,dwID);

			m_mapDoodadReliveGroupTreeItem[dwID] = hItemSet;
			i++;
		}

		wsprintf(Name,"未分组");
		HTREEITEM hItemSet = m_Tree.InsertItem(Name,DOODADGROUP,DOODADGROUP,m_hDoodadRoot,TVI_LAST);
		m_Tree.SetItemData(hItemSet,0);
		m_mapDoodadReliveGroupTreeItem[0] = hItemSet;

	}
	{
		list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_lpDocLogical->m_listDoodadInfo.begin();
		list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_lpDocLogical->m_listDoodadInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = *i;

			map<DWORD,HTREEITEM>::iterator iFind = m_mapDoodadReliveGroupTreeItem.find(pDoodad->dwDoodadReliveID);
			if(iFind!=m_mapDoodadReliveGroupTreeItem.end())
			{
				HTREEITEM hGroup = iFind->second;

				wsprintf(Name,"%s",pDoodad->szName.c_str());
				HTREEITEM hItemSet = m_Tree.InsertItem(Name,DOODAD,DOODAD,hGroup,TVI_LAST);
				m_Tree.SetItemData(hItemSet,(DWORD_PTR)pDoodad);
			}

			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		m_mapNpcReliveGroupTreeItem.clear();

		map<DWORD,KSceneEditorDocLogical::ReviveGroup>::iterator i    = m_lpDocLogical->m_mapNpcReviveGroup.begin();
		map<DWORD,KSceneEditorDocLogical::ReviveGroup>::iterator iend = m_lpDocLogical->m_mapNpcReviveGroup.end();
		while (i!=iend)
		{
			DWORD dwID = i->first;
			KSceneEditorDocLogical::ReviveGroup& group = i->second;

			wsprintf(Name,"%s %d",group.szName.c_str(),dwID);
			HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPCGROUP,NPCGROUP,m_hNPCRoot,TVI_LAST);
			m_Tree.SetItemData(hItemSet,dwID);

			m_mapNpcReliveGroupTreeItem[dwID] = hItemSet;
			i++;
		}

		wsprintf(Name,"未分组");
		HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPCGROUP,NPCGROUP,m_hNPCRoot,TVI_LAST);
		m_Tree.SetItemData(hItemSet,0);
		m_mapNpcReliveGroupTreeItem[0] = hItemSet;

	}
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i    = m_lpDocLogical->m_listNPCInfo.begin();
		list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

			map<DWORD,HTREEITEM>::iterator iFind = m_mapNpcReliveGroupTreeItem.find(pNpc->dwNpcReliveID);
			if(iFind!=m_mapNpcReliveGroupTreeItem.end())
			{
				HTREEITEM hGroup = iFind->second;

				wsprintf(Name,"%s",pNpc->szName.c_str());
				HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPC,NPC,hGroup,TVI_LAST);
				m_Tree.SetItemData(hItemSet,(DWORD_PTR)pNpc);
			}

			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		m_mapNpcRandomGroupTreeItem.clear();

		map<DWORD,KSceneEditorDocLogical::RandomGroup>::iterator i    = m_lpDocLogical->m_mapRandomGroup.begin();
		map<DWORD,KSceneEditorDocLogical::RandomGroup>::iterator iend = m_lpDocLogical->m_mapRandomGroup.end();
		while (i!=iend)
		{
			DWORD dwID = i->first;
			KSceneEditorDocLogical::RandomGroup& group = i->second;

			wsprintf(Name,"%s %d",group.szName.c_str(),dwID);
			HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPCRANDOMGROUP,NPCRANDOMGROUP,m_hNPCRandomRoot,TVI_LAST);
			m_Tree.SetItemData(hItemSet,dwID);

			m_mapNpcRandomGroupTreeItem[dwID] = hItemSet;
			i++;
		}

		wsprintf(Name,"未分组");
		HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPCRANDOMGROUP,NPCRANDOMGROUP,m_hNPCRandomRoot,TVI_LAST);
		m_Tree.SetItemData(hItemSet,0);
		m_mapNpcRandomGroupTreeItem[0] = hItemSet;

	}
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i    = m_lpDocLogical->m_listNPCInfo.begin();
		list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

			map<DWORD,HTREEITEM>::iterator iFind = m_mapNpcRandomGroupTreeItem.find(pNpc->dwNpcRandomID);
			if(iFind!=m_mapNpcRandomGroupTreeItem.end())
			{
				HTREEITEM hGroup = iFind->second;

				wsprintf(Name,"%s",pNpc->szName.c_str());
				HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPC,NPC,hGroup,TVI_LAST);
				m_Tree.SetItemData(hItemSet,(DWORD_PTR)pNpc);
			}

			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		m_mapNpcAIParameterTreeItem.clear();

		map<DWORD,KSceneEditorDocLogical::AIParameterSet>::iterator i    = m_lpDocLogical->m_mapNpcAISceneParameterSet.begin();
		map<DWORD,KSceneEditorDocLogical::AIParameterSet>::iterator iend = m_lpDocLogical->m_mapNpcAISceneParameterSet.end();
		while (i!=iend)
		{
			DWORD dwID = i->first;
			KSceneEditorDocLogical::AIParameterSet& group = i->second;

			wsprintf(Name,"%s %d",group.szName.c_str(),dwID);
			HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPCAIPARAMETERSGROUP,NPCAIPARAMETERSGROUP,m_hNPCAIParameterRoot,TVI_LAST);
			m_Tree.SetItemData(hItemSet,dwID);

			m_mapNpcAIParameterTreeItem[dwID] = hItemSet;
			i++;
		}

		wsprintf(Name,"未分组");
		HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPCAIPARAMETERSGROUP,NPCAIPARAMETERSGROUP,m_hNPCAIParameterRoot,TVI_LAST);
		m_Tree.SetItemData(hItemSet,0);
		m_mapNpcAIParameterTreeItem[0] = hItemSet;

	}
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i    = m_lpDocLogical->m_listNPCInfo.begin();
		list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

			map<DWORD,HTREEITEM>::iterator iFind = m_mapNpcAIParameterTreeItem.find(pNpc->dwNpcAISet);
			if(iFind!=m_mapNpcAIParameterTreeItem.end())
			{
				HTREEITEM hGroup = iFind->second;

				wsprintf(Name,"%s",pNpc->szName.c_str());
				HTREEITEM hItemSet = m_Tree.InsertItem(Name,NPC,NPC,hGroup,TVI_LAST);
				m_Tree.SetItemData(hItemSet,(DWORD_PTR)pNpc);
			}

			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KSceneEditorDocLogical::AIGroup*>::iterator i = m_lpDocLogical->m_listAIGroup.begin();
		while (i!=m_lpDocLogical->m_listAIGroup.end())
		{
			KSceneEditorDocLogical::AIGroup* pGroup = *i;

			HTREEITEM hGroup = m_Tree.InsertItem(pGroup->szName.c_str(),AIGROUP,AIGROUP,m_hAIRoot,TVI_LAST);
			m_Tree.SetItemData(hGroup,(DWORD_PTR)pGroup);

			for(size_t s =0;s<pGroup->vecNpcInfo.size();s++)
			{
				KSceneEditorDocLogical::NPCInfo* pNPC = pGroup->vecNpcInfo[s];

				if((int)s==pGroup->nKing)
					wsprintf(Name,"%s King",pNPC->szName.c_str());
				else
					wsprintf(Name,"%s",pNPC->szName.c_str());

				HTREEITEM hNPC = m_Tree.InsertItem(Name,AIGROUP_NPC,AIGROUP_NPC,hGroup,TVI_LAST);
				m_Tree.SetItemData(hNPC,(DWORD_PTR)pNPC);
			}
			i++;
		}
	}

	// 副本进度ID组的显示, 包括npc和doodad
	m_mapProgressGroupTreeItem.clear();
	// npc
	for (list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_lpDocLogical->m_listNPCInfo.begin(); iter != m_lpDocLogical->m_listNPCInfo.end(); iter++)
	{
		KSceneEditorDocLogical::NPCInfo* npcInfo = *iter;
		int progressid = npcInfo->nProgressID;
		if (progressid == INVALID_SCENE_PROGRESS_ID)
			continue;

		HTREEITEM hItemNPCs = NULL, hItemDOODADs = NULL;
		BuildOrFindProgressTreeItems(progressid, &hItemNPCs, &hItemDOODADs);

		wsprintf(Name,"%s",npcInfo->szName.c_str());
		HTREEITEM hItemThisNpc = m_Tree.InsertItem(Name, PROGRESSGROUPNPC, PROGRESSGROUPNPC, hItemNPCs, TVI_LAST);
		m_Tree.SetItemData(hItemThisNpc, (DWORD_PTR)npcInfo);
	}
	// doodad
	for (list<KSceneEditorDocLogical::DoodadInfo*>::iterator iter = m_lpDocLogical->m_listDoodadInfo.begin(); iter != m_lpDocLogical->m_listDoodadInfo.end(); iter++)
	{
		KSceneEditorDocLogical::DoodadInfo* doodadinfo = *iter;
		int progressid = doodadinfo->nProgressID;
		if (progressid == INVALID_SCENE_PROGRESS_ID)
			continue;

		HTREEITEM hItemNPCs = NULL, hItemDOODADs = NULL;
		BuildOrFindProgressTreeItems(progressid, &hItemNPCs, &hItemDOODADs);

		wsprintf(Name,"%s",doodadinfo->szName.c_str());
		HTREEITEM hItemThisDoodad = m_Tree.InsertItem(Name, PROGRESSGROUPDOODAD, PROGRESSGROUPDOODAD, hItemDOODADs, TVI_LAST);
		m_Tree.SetItemData(hItemThisDoodad, (DWORD_PTR)doodadinfo);
	}

	m_Tree.Expand(m_hDoodadRoot, TVE_EXPAND);
	m_Tree.Expand(m_hNPCRoot, TVE_EXPAND);
	m_Tree.Expand(m_hAIRoot, TVE_EXPAND);
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonAdd2()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case DOODADGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			IEKG3DRepresentObject* pObj = BuildRefreshPoint(g_GetEngineIEInterface(), 0, dwSetID, _T("Refresh Point"));
			HRESULT hr = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);
			if (FAILED(hr))
			{
				SAFE_RELEASE(pObj);
			}
		}
		break;
	case NPCGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			IEKG3DRepresentObject* pObj = BuildRefreshPoint(g_GetEngineIEInterface(), 1, dwSetID, _T("Refresh Point"));
			HRESULT hr = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);
			if (FAILED(hr))
			{
				SAFE_RELEASE(pObj);
			}
			
		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	/*case DOODADGROUP:
		{
			m_Button_AddPoint.EnableWindow(TRUE);
			m_Button_AddSelectedEntity.EnableWindow(TRUE);
			m_Button_RemoveSelectedEntity.EnableWindow(TRUE);
			m_Edit_Direction.EnableWindow(FALSE);
			m_Edit_SetName.EnableWindow(TRUE);
			m_Edit_ArrayFile.EnableWindow(FALSE);
			m_Check_King.EnableWindow(FALSE);
			m_Combo_Way.EnableWindow(FALSE);
			m_Button_ArrayBrown.EnableWindow(FALSE);
			m_Button_ArrayClear.EnableWindow(FALSE);
			m_Edit_AIArrayID.EnableWindow(FALSE);
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = NULL;
			if(SUCCEEDED(m_lpDocLogical->GetDoodadRefreshPointSetByID(&pSet,dwSetID)))
			{
				m_Edit_SetName.SetWindowText(pSet->szName.c_str());
			}
		}
		break;
	case NPCGROUP:
		{
			m_Button_AddPoint.EnableWindow(TRUE);
			m_Button_AddSelectedEntity.EnableWindow(TRUE);
			m_Button_RemoveSelectedEntity.EnableWindow(TRUE);
			m_Edit_Direction.EnableWindow(FALSE);
			m_Edit_SetName.EnableWindow(TRUE);
			m_Edit_ArrayFile.EnableWindow(FALSE);
			m_Check_King.EnableWindow(FALSE);
			m_Combo_Way.EnableWindow(FALSE);
			m_Button_ArrayBrown.EnableWindow(FALSE);
			m_Button_ArrayClear.EnableWindow(FALSE);
			m_Edit_AIArrayID.EnableWindow(FALSE);

			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			KSceneEditorDocLogical::NPCRefreshPointSet* pSet = NULL;
			if(SUCCEEDED(m_lpDocLogical->GetNPCRefreshPointSetByID(&pSet,dwSetID)))
			{
				m_Edit_SetName.SetWindowText(pSet->szName.c_str());
			}
		}
		break;
	case DOODADGROUPREFRESHPOINT:
	case NPCGROUPREFRESHPOINT:
		{
			m_Button_AddPoint.EnableWindow(FALSE);
			m_Button_AddSelectedEntity.EnableWindow(FALSE);
			m_Button_RemoveSelectedEntity.EnableWindow(FALSE);
			m_Edit_Direction.EnableWindow(TRUE);
			m_Edit_SetName.EnableWindow(FALSE);
			m_Edit_ArrayFile.EnableWindow(FALSE);
			m_Check_King.EnableWindow(FALSE);
			m_Combo_Way.EnableWindow(FALSE);
			m_Button_ArrayBrown.EnableWindow(FALSE);
			m_Button_ArrayClear.EnableWindow(FALSE);
			m_Edit_AIArrayID.EnableWindow(FALSE);


			KSceneEditorDocLogical::RefreshPoint* pPoint = (KSceneEditorDocLogical::RefreshPoint*)m_Tree.GetItemData(hItem);
			m_nDirection = pPoint->nDirection;
			UpdateData(FALSE);
		}
		break;*/
	case AIGROUP:
		{
			m_Button_AddPoint.EnableWindow(FALSE);
			m_Button_AddSelectedEntity.EnableWindow(TRUE);
			m_Button_RemoveSelectedEntity.EnableWindow(TRUE);
			m_Edit_SetName.EnableWindow(TRUE);
			m_Edit_ArrayFile.EnableWindow(TRUE);
			m_Check_King.EnableWindow(FALSE);
			m_Combo_Way.EnableWindow(TRUE);
			m_Button_ArrayBrown.EnableWindow(TRUE);
			m_Button_ArrayClear.EnableWindow(TRUE);
			m_Edit_AIArrayID.EnableWindow(TRUE);

			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
			m_Edit_SetName.SetWindowText(pGroup->szName.c_str());
			m_Edit_ArrayFile.SetWindowText(pGroup->szFileName.c_str());

			m_nAIArrayID = pGroup->nArrayIndex;

			FillCombo();

			if(!pGroup->pWayPointSet)
			{
				m_Combo_Way.SetCurSel(-1);
			}
			else
			{
				int nCount = m_Combo_Way.GetCount();
				for (int i=0;i<=nCount;i++)
				{
					KSceneEditorDocLogical::WayPointSet* pSet = 
						(KSceneEditorDocLogical::WayPointSet*) m_Combo_Way.GetItemData(i);
					if(pSet==pGroup->pWayPointSet)
					{
						m_Combo_Way.SetCurSel(i);
						break;
					}
				}
			}
			UpdateData(FALSE);
		}
		break;
	case AIGROUP_NPC:
		{
			m_Button_AddPoint.EnableWindow(FALSE);
			m_Button_AddSelectedEntity.EnableWindow(FALSE);
			m_Button_RemoveSelectedEntity.EnableWindow(FALSE);
			m_Edit_SetName.EnableWindow(FALSE);
			m_Edit_ArrayFile.EnableWindow(FALSE);
			m_Check_King.EnableWindow(TRUE);
			m_Combo_Way.EnableWindow(FALSE);
			m_Button_ArrayBrown.EnableWindow(FALSE);
			m_Button_ArrayClear.EnableWindow(FALSE);
			m_Edit_AIArrayID.EnableWindow(FALSE);

			HTREEITEM hGroup = m_Tree.GetParentItem(hItem);
			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hGroup));
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)(m_Tree.GetItemData(hItem));

			for (size_t i=0;i<pGroup->vecNpcInfo.size();i++)
			{
				if((pGroup->nKing==(int)i)&&(pNpc==pGroup->vecNpcInfo[i]))
				{
					m_Check_King.SetCheck(TRUE);
				}
				else
				{
					m_Check_King.SetCheck(FALSE);
				}
			}
		}
		break;

	// 副本进度ID组
	case PROGRESSGROUPNPC:
	case PROGRESSGROUPDOODAD:
		{
			D3DXVECTOR3 objpos;
			IEKG3DRepresentObject* pPointer = NULL;
			if (nImage == PROGRESSGROUPNPC)
			{
				KSceneEditorDocLogical::NPCInfo* npcinfo = (KSceneEditorDocLogical::NPCInfo*)(m_Tree.GetItemData(hItem));
				objpos = npcinfo->vPosition;
				pPointer = (IEKG3DRepresentObject*)npcinfo->pPointer;
			}
			else if (nImage == PROGRESSGROUPDOODAD)
			{
				KSceneEditorDocLogical::DoodadInfo* doodadinfo = (KSceneEditorDocLogical::DoodadInfo*)(m_Tree.GetItemData(hItem));
				objpos = doodadinfo->vPosition;
				pPointer = (IEKG3DRepresentObject*)doodadinfo->pPointer;
			}

			AtlVector3 pos, lookat;
			m_lpSceneSceneEditor->GetCameraPosLookat(&pos, &lookat);
			AtlVector3 delta;
			delta.x = lookat.x - pos.x; delta.y = lookat.y - pos.y; delta.z = lookat.z - pos.z;
			AtlVector3 newPos, newLookat;
			newLookat.x = objpos.x; newLookat.y = objpos.y; newLookat.z = objpos.z;
			newPos.x = newLookat.x - delta.x; newPos.y = newLookat.y - delta.y; newPos.z = newLookat.z - delta.z;
			m_lpSceneSceneEditor->SetCameraPosLookat(&newPos, &newLookat);

			m_lpSceneSceneEditor->UpdateSelectableEntityList(5);

			m_lpSceneSceneEditor->ClearSelectedEntityList();
			m_lpSceneSceneEditor->SetSelected(pPointer, TRUE);
		}

		break;

	default:
		m_Button_AddPoint.EnableWindow(FALSE);
		m_Button_AddSelectedEntity.EnableWindow(FALSE);
		m_Button_RemoveSelectedEntity.EnableWindow(FALSE);
		m_Edit_Direction.EnableWindow(FALSE);
		m_Edit_SetName.EnableWindow(FALSE);
		m_Edit_ArrayFile.EnableWindow(FALSE);
		m_Check_King.EnableWindow(FALSE);
		m_Combo_Way.EnableWindow(FALSE);
		m_Button_ArrayBrown.EnableWindow(FALSE);
		m_Button_ArrayClear.EnableWindow(FALSE);
		m_Edit_AIArrayID.EnableWindow(FALSE);
		break;
	}

}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonAddselectedentity()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case DOODADGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->AddSelectedEntityToDoodadRefreshSet(dwSetID);
		}
		break;
	case NPCGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->AddSelectedEntityToNPCRefreshSet(dwSetID);
		}
		break;
	case AIGROUP:
		{
			KSceneEditorDocLogical::AIGroup* pSet = (KSceneEditorDocLogical::AIGroup*)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->AddSelectedEntityToAIGroup(pSet->dwSetID);
		}
		break;
	}

}

void KSceneSceneEditorDialogGroup::OnEnChangeEditDirection()
{
	UpdateData(TRUE);

	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case DOODADGROUPREFRESHPOINT:
	case NPCGROUPREFRESHPOINT:
		{
			KSceneEditorDocLogical::RefreshPoint* pPoint = (KSceneEditorDocLogical::RefreshPoint*)m_Tree.GetItemData(hItem);
			pPoint->nDirection = m_nDirection;
		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	switch(nSinkID)
	{
	case EVENTDOCLOGIGAL_REFRESHPOINT_ADDED:
	case EVENTDOCLOGIGAL_REFRESHPOINT_DELETED:
	case EVENTDOCLOGIGAL_REFRESHPOINT_MOVED:
	case EVENTDOCLOGIGAL_REFRESHPOINT_ROTATED:
		FillTree();
	    break;
	case EVENTDOCLOGIGAL_UPDATEUI:
		FillTree();
		break;
	default:
	    break;
	}
}
BOOL KSceneSceneEditorDialogGroup::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_lpDocLogical)
		m_lpDocLogical->Advise(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogGroup::OnEnChangeEditSetname()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	TCHAR Name[MAX_PATH];

	switch(nImage)
	{
	/*case DOODADGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = NULL;
			if(SUCCEEDED(m_lpDocLogical->GetDoodadRefreshPointSetByID(&pSet,dwSetID)))
			{
				m_Edit_SetName.GetWindowText(Name,MAX_PATH);
				pSet->szName = Name;

				wsprintf(Name,"%s %d/%d",pSet->szName.c_str(),pSet->listDoodadInfo.size(),pSet->listRefreshPoint.listRefreshPoint.size());
				m_Tree.SetItemText(hItem,Name);
			}
		}
		break;
	case NPCGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			KSceneEditorDocLogical::NPCRefreshPointSet* pSet = NULL;
			if(SUCCEEDED(m_lpDocLogical->GetNPCRefreshPointSetByID(&pSet,dwSetID)))
			{
				m_Edit_SetName.GetWindowText(Name,MAX_PATH);
				pSet->szName = Name;

				wsprintf(Name,"%s %d/%d",pSet->szName.c_str(),pSet->listNPCInfo.size(),pSet->listRefreshPoint.listRefreshPoint.size());
				m_Tree.SetItemText(hItem,Name);

			}
		}
		break;*/
	case AIGROUP:
		{
			KSceneEditorDocLogical::AIGroup* pSet = (KSceneEditorDocLogical::AIGroup*)m_Tree.GetItemData(hItem);

			m_Edit_SetName.GetWindowText(Name,MAX_PATH);
			pSet->szName = Name;

			wsprintf(Name,"%s %d",pSet->szName.c_str(),pSet->vecNpcInfo.size());
			m_Tree.SetItemText(hItem,Name);
		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonAddselectedentity2()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case DOODADGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->RemoveSelectedEntityToDoodadRefreshSet(dwSetID);
		}
		break;
	case NPCGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->RemoveSelectedEntityToNPCRefreshSet(dwSetID);
		}
		break;
	case AIGROUP:
		{
			KSceneEditorDocLogical::AIGroup* pSet = (KSceneEditorDocLogical::AIGroup*)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->RemoveSelectedEntityFromAIGroup(pSet->dwSetID);
		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	m_lpSceneSceneEditor->ClearSelectedEntityList();
	switch(nImage)
	{
	case AIGROUP:
		{
			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
			for (size_t i=0;i<pGroup->vecNpcInfo.size();i++)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = pGroup->vecNpcInfo[i];
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
			}
		}
		break;
	case AIGROUP_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)(m_Tree.GetItemData(hItem));
			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
		}
		break;
	case DOODAD:
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			if(pDoodad->pPointer)
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pDoodad->pPointer, TRUE);
			PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ, 0);	
		}
		break;
	case NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			if(pNpc->pPointer)
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
			PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ,0);
		}
		break;
	case DOODADGROUP:
		{
			HTREEITEM hNpcItem = m_Tree.GetNextItem(hItem,TVGN_CHILD);
			while (hNpcItem)
			{
				KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hNpcItem);
				if(pDoodad->pPointer)
					m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pDoodad->pPointer, TRUE);

				hNpcItem = m_Tree.GetNextItem(hNpcItem,TVGN_NEXT);
			}
			PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ, 0);
		}
		break;
	case NPCAIPARAMETERSGROUP:
	case NPCRANDOMGROUP:
	case NPCGROUP:
		{
			HTREEITEM hNpcItem = m_Tree.GetNextItem(hItem,TVGN_CHILD);
			while (hNpcItem)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hNpcItem);
				if(pNpc->pPointer)
					m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);

				hNpcItem = m_Tree.GetNextItem(hNpcItem,TVGN_NEXT);
			}
			PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ, 0);
		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonDelete()
{
	if(!m_lpDocLogical)
		return;
	if(!m_lpSceneSceneEditor)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	m_lpSceneSceneEditor->ClearSelectedEntityList();
	switch(nImage)
	{
	/*case DOODADGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = NULL;
			if(SUCCEEDED(m_lpDocLogical->GetDoodadRefreshPointSetByID(&pSet,dwSetID)))
			{
				list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
				while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
				{
					KSceneEditorDocLogical::RefreshPoint* pPoint = *j;
					m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
					j++;
				}
				m_lpSceneSceneEditor->DeleteSelectedEntity();
				m_lpDocLogical->DeleteDoodadRefreshPointSet(pSet);

				FillTree();

			}
		}
		break;
	case NPCGROUP:
		{
			DWORD dwSetID = (DWORD)m_Tree.GetItemData(hItem);
			m_lpSceneSceneEditor->RemoveSelectedEntityToNPCRefreshSet(dwSetID);
			KSceneEditorDocLogical::NPCRefreshPointSet* pSet = NULL;
			if(SUCCEEDED(m_lpDocLogical->GetNPCRefreshPointSetByID(&pSet,dwSetID)))
			{
				list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
				while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
				{
					KSceneEditorDocLogical::RefreshPoint* pPoint = *j;
					m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
					j++;
				}
				m_lpSceneSceneEditor->DeleteSelectedEntity();
				m_lpDocLogical->DeleteNPCRefreshPointSet(pSet);

				FillTree();
			}
		}
		break;*/
	case AIGROUP:
		{
			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
			m_lpDocLogical->DeleteAIGroup(pGroup);
			FillTree();
		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonAddaigroup()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;
	KSceneEditorDocLogical::AIGroup* pGroup = NULL;
	m_lpDocLogical->AddOneAIGroup(&pGroup,"AI Group");
	FillTree();
}

void KSceneSceneEditorDialogGroup::OnBnClickedCheckKing()
{
	BOOL bCheck = m_Check_King.GetCheck();
	if(bCheck)
	{
		if(!m_lpSceneSceneEditor)
			return;
		if(!m_lpDocLogical)
			return;

		HTREEITEM hItem = m_Tree.GetSelectedItem();
		if(!hItem)
			return;
		int nImage = 0;
		m_Tree.GetItemImage(hItem,nImage,nImage);

		if(nImage==AIGROUP_NPC)
		{
			HTREEITEM hGroup = m_Tree.GetParentItem(hItem);
			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hGroup));
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)(m_Tree.GetItemData(hItem));

			for (size_t i=0;i<pGroup->vecNpcInfo.size();i++)
			{
				if(pNpc==pGroup->vecNpcInfo[i])
				{
					pGroup->nKing = (int)i;
				}
			}
		}
		FillTree();
	}
}

void KSceneSceneEditorDialogGroup::OnCbnSelchangeComboWaypointse()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case AIGROUP:
		{
			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
			int nIndex = m_Combo_Way.GetCurSel();
			KSceneEditorDocLogical::WayPointSet* pSet = 
				(KSceneEditorDocLogical::WayPointSet*) m_Combo_Way.GetItemData(nIndex);
			pGroup->pWayPointSet = pSet;

		}
		break;
	}
}

void KSceneSceneEditorDialogGroup::FillCombo()
{
	int nCount = m_Combo_Way.GetCount();
	for (int i=0;i<=nCount;i++)
	{
		m_Combo_Way.DeleteString(0);
	}

	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
	while (i!=m_lpDocLogical->m_mapWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = i->second;

		int nIndex = m_Combo_Way.AddString(pSet->szName.c_str());
		m_Combo_Way.SetItemData(nIndex,(DWORD_PTR)pSet);
		i++;
	}
}
void KSceneSceneEditorDialogGroup::OnBnClickedButtonBrown()
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    TCHAR szAppName[MAX_PATH] = { 0 };
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	m_lpSceneSceneEditor->ClearSelectedEntityList();

	if(nImage == AIGROUP)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));

		CFileDialog dlg(true, "Tab", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D阵型文件(*.Tab)|*.Tab||");
		if(dlg.DoModal() == IDOK)
		{
			TCHAR FileName[MAX_PATH];
			TCHAR RetFileName[MAX_PATH];
			wsprintf(FileName,"%s",dlg.GetPathName());
            pManager->GetDefWorkDirectory(szAppName, MAX_PATH);
            pManager->GetRelativePath(szAppName,FileName,RetFileName);

			pGroup->szFileName = RetFileName;
			m_Edit_ArrayFile.SetWindowText(RetFileName);
		}
	}
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonClear()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage == AIGROUP)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		pGroup->szFileName = "";
		m_Edit_ArrayFile.SetWindowText("");
	}
}

void KSceneSceneEditorDialogGroup::OnEnChangeEditMatrixfile()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

	if(nImage == AIGROUP)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		TCHAR Name[MAX_PATH];
		m_Edit_ArrayFile.GetWindowText(Name,MAX_PATH);
		pGroup->szFileName = Name;
	}
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonFitgroup()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

//Exit0:
    return;
}

void KSceneSceneEditorDialogGroup::OnBnClickedButtonFitarray()
{
	// TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogGroup::OnEnChangeEditArrayid()
{
	UpdateData(TRUE);

	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage == AIGROUP)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		pGroup->nArrayIndex = m_nAIArrayID;
	}
}
