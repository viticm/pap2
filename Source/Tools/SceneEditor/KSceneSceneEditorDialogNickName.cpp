// KSceneSceneEditorDialogNickName.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogNickName.h"
#include "KSceneEditorDocLogical.h"
#include "IEEditor.h"
//#include "KGCommonWidgets.h"
#include "KSceneEditorReviveDialog.h"
#include "KSceneEditorRandomGroupDialog.h"
#include "KSceneEditorAIDialog.h"

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogNickName, CDialog)

#define IMAGE_DOODADROOT 0
#define IMAGE_NPCROOT    1
#define IMAGE_DOODADCLASS 2
#define IMAGE_NPCCLASS    3
#define IMAGE_DOODAD 4
#define IMAGE_NPC    5

KSceneSceneEditorDialogNickName::KSceneSceneEditorDialogNickName(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogNickName::IDD, pParent)
	, m_bShowAllName(false)
{
	m_lpSceneSceneEditor = NULL;
	m_lpDocLogical = NULL;
	m_hNpcRoot = NULL;
	m_hDoodRoot = NULL;
	m_bShowAllName = FALSE;
	// KSceneSceneEditorDialogNickName.cpp : implementation file
	m_lpAIDialog = NULL;
}

KSceneSceneEditorDialogNickName::~KSceneSceneEditorDialogNickName()
{
	SAFE_DELETE(m_lpAIDialog);
}

void KSceneSceneEditorDialogNickName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NPC, m_Tree);
	DDX_Control(pDX, IDC_EDIT_NICKNAME, m_Edit_NickName);
	DDX_Control(pDX, IDC_BUTTON_BROWNSCRIPT, m_Button_Brow);
	DDX_Control(pDX, IDC_BUTTON_CLEARSCRIPT, m_Button_ClearScript);
	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_Edit_Script);
	DDX_Control(pDX, IDC_COMBO_WAYPOINTSE, m_ComBo_Way);
	DDX_Control(pDX, IDC_COMBO_AITYPE, m_Combo_AIType);
	DDX_Control(pDX, IDC_BUTTON_REVIVEGROUP, m_Button_ReliveGroup);
	DDX_Control(pDX, IDC_BUTTON_RANDOMGROUP, m_Button_Random);
}

void KSceneSceneEditorDialogNickName::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	if(nSinkID==EVENTDOCLOGIGAL_UPDATEUI)
	{
		FillTree();
	}
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogNickName, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogNickName::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneSceneEditorDialogNickName::OnTvnSelchangedTreeNpc)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NPC, &KSceneSceneEditorDialogNickName::OnNMDblclkTreeNpc)
	ON_EN_CHANGE(IDC_EDIT_NICKNAME, &KSceneSceneEditorDialogNickName::OnEnChangeEditNickname)
	ON_BN_CLICKED(IDC_BUTTON_BROWNSCRIPT, &KSceneSceneEditorDialogNickName::OnBnClickedButtonBrownscript)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSCRIPT, &KSceneSceneEditorDialogNickName::OnBnClickedButtonClearscript)
	ON_EN_CHANGE(IDC_EDIT_SCRIPT, &KSceneSceneEditorDialogNickName::OnEnChangeEditScript)
	ON_CBN_SELCHANGE(IDC_COMBO_WAYPOINTSE, &KSceneSceneEditorDialogNickName::OnCbnSelchangeComboWaypointse)
	ON_BN_CLICKED(IDC_BUTTON_WAYCLEAR2, &KSceneSceneEditorDialogNickName::OnBnClickedButtonWayclear2)
	ON_CBN_SELCHANGE(IDC_COMBO_AITYPE, &KSceneSceneEditorDialogNickName::OnCbnSelchangeComboAitype)
	ON_BN_CLICKED(IDC_CHECK_SHOWALLNAME, &KSceneSceneEditorDialogNickName::OnBnClickedCheckShowallname)
	ON_BN_CLICKED(IDC_BUTTON_REVIVEGROUP, &KSceneSceneEditorDialogNickName::OnBnClickedButtonRevivegroup)
	ON_BN_CLICKED(IDC_BUTTON_RANDOMGROUP, &KSceneSceneEditorDialogNickName::OnBnClickedButtonRandomgroup)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSceneEditorDialogNickName::OnBnClickedButton1)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogNickName message handlers
void KSceneSceneEditorDialogNickName::FillTree()
{
	m_Tree.SelectItem(NULL);
	m_Tree.DeleteAllItems();
	m_mapDoodadType.clear();
	m_mapNPCType.clear();

	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;

	TCHAR Name[MAX_PATH];

	wsprintf(Name,"Doodad %d",m_lpDocLogical->m_listDoodadInfo.size());
	m_hDoodRoot = m_Tree.InsertItem(Name,IMAGE_DOODADROOT,IMAGE_DOODADROOT,TVI_ROOT,TVI_LAST);

	list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_lpDocLogical->m_listDoodadInfo.begin();
	while (i!=m_lpDocLogical->m_listDoodadInfo.end())
	{
		KSceneEditorDocLogical::DoodadInfo* pDoodad = *i;

		HTREEITEM hClass = NULL;
		map<int,HTREEITEM>::iterator s = m_mapDoodadType.find(pDoodad->nTempleteID);
		if(s==m_mapDoodadType.end())
		{
			wsprintf(Name,"%s",pDoodad->szName.c_str());
			hClass = m_Tree.InsertItem(Name,IMAGE_DOODADCLASS,IMAGE_DOODADCLASS,m_hDoodRoot,TVI_LAST);
			m_Tree.SetItemData(hClass,0);
			m_mapDoodadType[pDoodad->nTempleteID] = hClass;
		}
		else
		{
			hClass = s->second;
		}

		wsprintf(Name,"%s (%s)",pDoodad->szNickName.c_str(),pDoodad->szName.c_str());
		HTREEITEM hItem = m_Tree.InsertItem(Name,IMAGE_DOODAD,IMAGE_DOODAD,hClass,TVI_LAST);
		m_Tree.SetItemData(hItem,(DWORD_PTR)pDoodad);

		//////////////////////////////////////////////////////////////////////////
		DWORD dwCount =0;
		dwCount = _kg_to_smaller_size_cast<DWORD>(m_Tree.GetItemData(hClass));
		dwCount++;
		m_Tree.SetItemData(hClass,dwCount);
		wsprintf(Name,"%s %d",pDoodad->szName.c_str(),dwCount);
		m_Tree.SetItemText(hClass,Name);

		if(dwCount == 0)
		{
			m_Tree.DeleteItem(hClass);
		}	

		i++;
	}
	//////////////////////////////////////////////////////////////////////////
	wsprintf(Name,"NPC %d",m_lpDocLogical->m_listNPCInfo.size());
	m_hNpcRoot = m_Tree.InsertItem(Name,IMAGE_NPCROOT,IMAGE_NPCROOT,TVI_ROOT,TVI_LAST);

	list<KSceneEditorDocLogical::NPCInfo*>::iterator j = m_lpDocLogical->m_listNPCInfo.begin();
	while (j!=m_lpDocLogical->m_listNPCInfo.end())
	{
		KSceneEditorDocLogical::NPCInfo* pNpc = *j;

		HTREEITEM hClass = NULL;
		map<int,HTREEITEM>::iterator s = m_mapNPCType.find(pNpc->nTempleteID);
		if(s==m_mapNPCType.end())
		{
			wsprintf(Name,"%s 0",pNpc->szName.c_str());
			hClass = m_Tree.InsertItem(Name,IMAGE_NPCCLASS,IMAGE_NPCCLASS,m_hNpcRoot,TVI_LAST);
			m_Tree.SetItemData(hClass,0);
			m_mapNPCType[pNpc->nTempleteID] = hClass;
		}
		else
		{
			hClass = s->second;

		}

		if (_strcmpi(pNpc->szNickName.c_str(),"")==0 && !m_bShowAllName)
		{
			j++;
		}
		else
		{
			wsprintf(Name,"%s (%s)",pNpc->szNickName.c_str(),pNpc->szName.c_str());

			HTREEITEM hItem = m_Tree.InsertItem(Name,IMAGE_NPC,IMAGE_NPC,hClass,TVI_LAST);
			m_Tree.SetItemData(hItem,(DWORD_PTR)pNpc);

			DWORD dwCount =0;
			dwCount = _kg_to_smaller_size_cast<DWORD>(m_Tree.GetItemData(hClass));
			dwCount++;
			m_Tree.SetItemData(hClass,dwCount);
			wsprintf(Name,"%s %d",pNpc->szName.c_str(),dwCount);
			m_Tree.SetItemText(hClass,Name);

			j++;
		}
	}
}

void KSceneSceneEditorDialogNickName::OnBnClickedButtonRefresh()
{
	FillTree();
}

void KSceneSceneEditorDialogNickName::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case IMAGE_DOODADCLASS:
	case IMAGE_NPCCLASS:
		{
			m_Edit_NickName.SetWindowText("");
			m_Button_ReliveGroup.SetWindowText("重生组ID");

			m_Button_Random.SetWindowText("无参数");

		}
		break;
	case IMAGE_DOODAD:
		{
			m_ComBo_Way.EnableWindow(FALSE);
			m_Combo_AIType.EnableWindow(FALSE);

			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			m_Edit_NickName.SetWindowText(pDoodad->szNickName.c_str());

			TCHAR szName[MAX_PATH];
			wsprintf(szName,"重生组ID %d",pDoodad->dwDoodadReliveID);
			m_Button_ReliveGroup.SetWindowText(szName);

			m_Button_Random.SetWindowText("无参数");

		}
		break;
	case IMAGE_NPC:
		{
			m_ComBo_Way.EnableWindow(TRUE);
			//m_Combo_AIType.EnableWindow(TRUE);

			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			m_Edit_NickName.SetWindowText(pNpc->szNickName.c_str());

			FillCombo();

			if(!pNpc->pWayPointSet)
			{
				m_ComBo_Way.SetCurSel(-1);
			}
			else
			{
				int nCount = m_ComBo_Way.GetCount();
				for (int i=0;i<=nCount;i++)
				{
					KSceneEditorDocLogical::WayPointSet* pSet = 
						(KSceneEditorDocLogical::WayPointSet*) m_ComBo_Way.GetItemData(i);
					if(pSet==pNpc->pWayPointSet)
					{
						m_ComBo_Way.SetCurSel(i);
						break;
					}
				}
			}

			//FillAICombo();
			//{
			//	int nCount = m_Combo_AIType.GetCount();
			//	for (int i=0;i<=nCount;i++)
			//	{
			//		int nAIType = (int) m_Combo_AIType.GetItemData(i);
			//		if(nAIType==pNpc->nAIType)
			//		{
			//			m_Combo_AIType.SetCurSel(i);
			//			break;
			//		}
			//	}
			//}

			TCHAR szName[MAX_PATH];
			wsprintf(szName,"重生组ID %d",pNpc->dwNpcReliveID);
			m_Button_ReliveGroup.SetWindowText(szName);
			wsprintf(szName,"随机组ID %d",pNpc->dwNpcRandomID);
			m_Button_Random.SetWindowText(szName);

		}
		break;
	}
}

BOOL KSceneSceneEditorDialogNickName::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_lpDocLogical)
		m_lpDocLogical->Advise(this);

	FillTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogNickName::OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	m_lpSceneSceneEditor->ClearSelectedEntityList();

	switch(nImage)
	{
	case IMAGE_DOODAD:
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			if(pDoodad->pPointer)
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pDoodad->pPointer, TRUE);

			m_Edit_NickName.SetWindowText(pDoodad->szNickName.c_str());
			m_Edit_Script.SetWindowText(pDoodad->szScriptName.c_str());
			PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ, 0);
		}
		break;
	case IMAGE_DOODADCLASS:
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
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			if(pNpc->pPointer)
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);

			m_Edit_NickName.SetWindowText(pNpc->szNickName.c_str());
			m_Edit_Script.SetWindowText(pNpc->szScriptName.c_str());
			PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ,0);
		}
	    break;
	case IMAGE_NPCCLASS:
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

void KSceneSceneEditorDialogNickName::OnEnChangeEditNickname()
{
	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	TCHAR Name[MAX_PATH];
	m_Edit_NickName.GetWindowText(Name,MAX_PATH);

	switch(nImage)
	{
	case IMAGE_DOODAD:
		{
			m_ComBo_Way.EnableWindow(FALSE);

			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			pDoodad->szNickName = Name;

			wsprintf(Name,"%s (%s)",pDoodad->szNickName.c_str(),pDoodad->szName.c_str());
			m_Tree.SetItemText(hItem,Name);
		}
		break;
	case IMAGE_NPC:
		{
			m_ComBo_Way.EnableWindow(TRUE);

			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			pNpc->szNickName = Name;

			wsprintf(Name,"%s (%s)",pNpc->szNickName.c_str(),pNpc->szName.c_str());
			m_Tree.SetItemText(hItem,Name);

			FillCombo();

			if(!pNpc->pWayPointSet)
			{
				m_ComBo_Way.SetCurSel(-1);
			}
			else
			{
				int nCount = m_ComBo_Way.GetCount();
				for (int i=0;i<=nCount;i++)
				{
					KSceneEditorDocLogical::WayPointSet* pSet = 
						(KSceneEditorDocLogical::WayPointSet*) m_ComBo_Way.GetItemData(i);
					if(pSet==pNpc->pWayPointSet)
					{
						m_ComBo_Way.SetCurSel(i);
						break;
					}
				}
			}

		}
		break;
	}
}

void KSceneSceneEditorDialogNickName::OnBnClickedButtonBrownscript()
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    TCHAR szAppName[MAX_PATH] = { 0 };
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	CFileDialog dlg(true, "Lua", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D脚本文件(*.Lua)|*.Lua||");
	if(dlg.DoModal() != IDOK)
		return;

	TCHAR FileName[MAX_PATH];
	TCHAR RetFileName[MAX_PATH];
	wsprintf(FileName,"%s",dlg.GetPathName());
    pManager->GetDefWorkDirectory(szAppName, MAX_PATH);

    pManager->GetRelativePath(szAppName, FileName,RetFileName);
	m_Edit_Script.SetWindowText(RetFileName);

	switch(nImage)
	{
	case IMAGE_DOODAD:
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			pDoodad->szScriptName = RetFileName;
		}
		break;
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			pNpc->szScriptName = RetFileName;
		}
		break;
	}
}

void KSceneSceneEditorDialogNickName::OnBnClickedButtonClearscript()
{
	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	m_Edit_Script.SetWindowText("");

	switch(nImage)
	{
	case IMAGE_DOODAD:
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			pDoodad->szScriptName = "";
		}
		break;
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			pNpc->szScriptName = "";
		}
		break;
	}
}

void KSceneSceneEditorDialogNickName::OnEnChangeEditScript()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	TCHAR Name[MAX_PATH];
	m_Edit_Script.GetWindowText(Name,MAX_PATH);

	switch(nImage)
	{
	case IMAGE_DOODAD:
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);
			pDoodad->szScriptName = Name;
		}
		break;
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			pNpc->szScriptName = Name;
		}
		break;
	}
}

void KSceneSceneEditorDialogNickName::OnCbnSelchangeComboWaypointse()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);

			int nIndex = m_ComBo_Way.GetCurSel();

			KSceneEditorDocLogical::WayPointSet* pSet = 
				(KSceneEditorDocLogical::WayPointSet*) m_ComBo_Way.GetItemData(nIndex);
			pNpc->pWayPointSet = pSet;
			m_lpDocLogical->UpdataOneNpcAIParameters(pNpc,m_lpDocLogical->m_bUseNpcWayPointSetPointer);
		}
		break;
	}
}

void KSceneSceneEditorDialogNickName::FillCombo()
{
	int nCount = m_ComBo_Way.GetCount();
	for (int i=0;i<=nCount;i++)
	{
		m_ComBo_Way.DeleteString(0);
	}

	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
	while (i!=m_lpDocLogical->m_mapWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = i->second;

		int nIndex = m_ComBo_Way.AddString(pSet->szName.c_str());
		m_ComBo_Way.SetItemData(nIndex,(DWORD_PTR)pSet);
		i++;
	}
}

void KSceneSceneEditorDialogNickName::OnBnClickedButtonWayclear2()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			m_ComBo_Way.SetCurSel(-1);
			pNpc->pWayPointSet = NULL;
			m_lpDocLogical->UpdataOneNpcAIParameters(pNpc,m_lpDocLogical->m_bUseNpcWayPointSetPointer);
		}
		break;
	}
}

void KSceneSceneEditorDialogNickName::FillAICombo()
{
	int nCount = m_Combo_AIType.GetCount();
	for (int i=0;i<=nCount;i++)
	{
		m_Combo_AIType.DeleteString(0);
	}

	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	int nIndex = 0;

	nIndex = m_Combo_AIType.AddString("aitInvalid");
	m_Combo_AIType.SetItemData(nIndex,(DWORD_PTR)aitInvalid);

	nIndex = m_Combo_AIType.AddString("aitBase");
	m_Combo_AIType.SetItemData(nIndex,(DWORD_PTR)aitBase);

	nIndex = m_Combo_AIType.AddString("aitPlayer");
	m_Combo_AIType.SetItemData(nIndex,(DWORD_PTR)aitPlayer);

	//nIndex = m_Combo_AIType.AddString("aitWolf");
	//m_Combo_AIType.SetItemData(nIndex,(DWORD_PTR)aitWolf);

	nIndex = m_Combo_AIType.AddString("aitTotal");
	m_Combo_AIType.SetItemData(nIndex,(DWORD_PTR)aitTotal);
}

void KSceneSceneEditorDialogNickName::OnCbnSelchangeComboAitype()
{
	//if(!m_lpSceneSceneEditor) return ;

	//if(!m_lpDocLogical) return ;

	//HTREEITEM hItem = m_Tree.GetSelectedItem();
	//if(!hItem) return ;

	//int nImage = 0;
	//m_Tree.GetItemImage(hItem,nImage,nImage);

	//switch(nImage)
	//{
	//case IMAGE_NPC:
	//	{
	//		KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);

	//		int nIndex = m_Combo_AIType.GetCurSel();

	//		int nAIType = (int)m_Combo_AIType.GetItemData(nIndex);
	//		pNpc->nAIType = nAIType;
	//	}
	//	break;
	//}

}

void KSceneSceneEditorDialogNickName::OnBnClickedCheckShowallname()
{
	m_bShowAllName =((CButton*)(GetDlgItem(IDC_CHECK_SHOWALLNAME)))->GetCheck();
	FillTree();
}

void KSceneSceneEditorDialogNickName::OnBnClickedButtonRevivegroup()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	TCHAR Name[MAX_PATH];
	m_Edit_Script.GetWindowText(Name,MAX_PATH);

	switch(nImage)
	{
	case IMAGE_DOODAD:
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hItem);

			KSceneEditorReviveDialog dlg;
			dlg.SetDocLogical(m_lpDocLogical,FALSE);
			dlg.SetCurSelectedID(pDoodad->dwDoodadReliveID);
			if(dlg.DoModal() != IDOK)
				return ;
			pDoodad->dwDoodadReliveID = dlg.m_dwCurrentSelectedID;
			if(pDoodad->pPointer)
			{
				IEKG3DRepresentDoodad* pRepDoodad = m_lpSceneSceneEditor->GetIEKG3DRepresentDoodad(pDoodad->pPointer);
				if(pRepDoodad)
					pRepDoodad->SetReliveID(dlg.m_dwCurrentSelectedID);
			}
			TCHAR szName[MAX_PATH];
			wsprintf(szName,"重生组ID %d",pDoodad->dwDoodadReliveID );
			m_Button_ReliveGroup.SetWindowText(szName);

		}
		break;
	case IMAGE_DOODADCLASS:
		{
			KSceneEditorReviveDialog dlg;
			dlg.SetDocLogical(m_lpDocLogical,FALSE);
			if(dlg.DoModal() != IDOK)
				return ;

			HTREEITEM hNpcItem = m_Tree.GetNextItem(hItem,TVGN_CHILD);
			while (hNpcItem)
			{
				KSceneEditorDocLogical::DoodadInfo* pDoodad = (KSceneEditorDocLogical::DoodadInfo*)m_Tree.GetItemData(hNpcItem);
				pDoodad->dwDoodadReliveID = dlg.m_dwCurrentSelectedID;
				if(pDoodad->pPointer)
				{
					IEKG3DRepresentDoodad* pRepDoodad = m_lpSceneSceneEditor->GetIEKG3DRepresentDoodad(pDoodad->pPointer);
					if(pRepDoodad)
						pRepDoodad->SetReliveID(dlg.m_dwCurrentSelectedID);
				}
				hNpcItem = m_Tree.GetNextItem(hNpcItem,TVGN_NEXT);
			}

			TCHAR szName[MAX_PATH];
			wsprintf(szName,"重生组ID %d",dlg.m_dwCurrentSelectedID);
			m_Button_ReliveGroup.SetWindowText(szName);
		}
		break;
	case IMAGE_NPC:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);

			KSceneEditorReviveDialog dlg;
			dlg.SetDocLogical(m_lpDocLogical,TRUE);
			dlg.SetCurSelectedID(pNpc->dwNpcReliveID);
			if(dlg.DoModal() != IDOK)
				return ;

			pNpc->dwNpcReliveID = dlg.m_dwCurrentSelectedID;
			if(pNpc->pPointer)
			{
				IEKG3DRepresentNPC* pRepNPC = m_lpSceneSceneEditor->GetIEKG3DRepresentNPC(pNpc->pPointer);
				if(pRepNPC)
					pRepNPC->SetReliveID(dlg.m_dwCurrentSelectedID);
			}
			TCHAR szName[MAX_PATH];
			wsprintf(szName,"重生组ID %d",pNpc->dwNpcReliveID );
			m_Button_ReliveGroup.SetWindowText(szName);
		}
		break;
	case IMAGE_NPCCLASS:
		{
			KSceneEditorReviveDialog dlg;
			dlg.SetDocLogical(m_lpDocLogical,TRUE);
			if(dlg.DoModal() != IDOK)
				return ;

			HTREEITEM hNpcItem = m_Tree.GetNextItem(hItem,TVGN_CHILD);
			while (hNpcItem)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hNpcItem);
				pNpc->dwNpcReliveID = dlg.m_dwCurrentSelectedID;
				if(pNpc->pPointer)
				{
					IEKG3DRepresentNPC* pRepNpc = m_lpSceneSceneEditor->GetIEKG3DRepresentNPC(pNpc->pPointer);
					if(pRepNpc)
						pRepNpc->SetReliveID(dlg.m_dwCurrentSelectedID);
				}
				hNpcItem = m_Tree.GetNextItem(hNpcItem,TVGN_NEXT);
			}

			TCHAR szName[MAX_PATH];
			wsprintf(szName,"重生组ID %d",dlg.m_dwCurrentSelectedID);
			m_Button_ReliveGroup.SetWindowText(szName);
		}
	}

}

void KSceneSceneEditorDialogNickName::OnBnClickedButtonRandomgroup()
{
	{
		if(!m_lpSceneSceneEditor) return ;

		if(!m_lpDocLogical) return ;

		HTREEITEM hItem = m_Tree.GetSelectedItem();
		if(!hItem) return ;

		int nImage = 0;
		m_Tree.GetItemImage(hItem,nImage,nImage);

		switch(nImage)
		{
		case IMAGE_NPC:
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);

				KSceneEditorRandomGroupDialog Dlg;
				Dlg.SetSceneEditor(m_lpSceneSceneEditor,m_lpDocLogical);
				Dlg.DoModal();

				pNpc->dwNpcRandomID = Dlg.m_dwSelectedGroupID;
				if(pNpc->pPointer)
				{
					IEKG3DRepresentNPC* pRepNpc = m_lpSceneSceneEditor->GetIEKG3DRepresentNPC(pNpc->pPointer);
					if(pRepNpc)
						pRepNpc->SetRandomID(Dlg.m_dwSelectedGroupID);
				}
				TCHAR szName[MAX_PATH];
				wsprintf(szName,"随机组ID %d",pNpc->dwNpcRandomID );
				m_Button_Random.SetWindowText(szName);
			}
			break;
		case IMAGE_NPCCLASS:
			{
				KSceneEditorRandomGroupDialog Dlg;
				Dlg.SetSceneEditor(m_lpSceneSceneEditor,m_lpDocLogical);
				Dlg.DoModal();

				HTREEITEM hNpcItem = m_Tree.GetNextItem(hItem,TVGN_CHILD);
				while (hNpcItem)
				{
					KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hNpcItem);
					pNpc->dwNpcRandomID = Dlg.m_dwSelectedGroupID;
					if(pNpc->pPointer)
					{
						IEKG3DRepresentNPC* pRepNpc = m_lpSceneSceneEditor->GetIEKG3DRepresentNPC(pNpc->pPointer);
						if(pRepNpc)
							pRepNpc->SetRandomID(Dlg.m_dwSelectedGroupID);
					}

					hNpcItem = m_Tree.GetNextItem(hNpcItem,TVGN_NEXT);
				}

				TCHAR szName[MAX_PATH];
				wsprintf(szName,"随机组ID %d",Dlg.m_dwSelectedGroupID);
				m_Button_Random.SetWindowText(szName);
			}
		}
	}

}

void KSceneSceneEditorDialogNickName::OnBnClickedButton1()
{
	if(!m_lpAIDialog)
	{
		m_lpAIDialog = new KSceneEditorAIDialog;
		KGLOG_PROCESS_ERROR(m_lpAIDialog);
		m_lpAIDialog->SetDocLogical(m_lpDocLogical);
		m_lpAIDialog->SetSceneEditor(m_lpSceneSceneEditor);
		m_lpAIDialog->Create(KSceneEditorAIDialog::IDD,this);
	}

	m_lpAIDialog->ShowWindow(SW_SHOWNORMAL);
Exit0:
	;
}
