// KSceneEditorAIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorAIDialog.h"
#include "IEEditor.h"

// KSceneEditorAIDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorAIDialog, CDialog)

KSceneEditorAIDialog::KSceneEditorAIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorAIDialog::IDD, pParent)
	, m_bDataAnalyse(FALSE)
{
	m_lpDocLogical = NULL;
	m_lpSceneSceneEditor = NULL;
}

KSceneEditorAIDialog::~KSceneEditorAIDialog()
{
}

void KSceneEditorAIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Check(pDX, IDC_DATASWITCH, m_bDataAnalyse);
}


BEGIN_MESSAGE_MAP(KSceneEditorAIDialog, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &KSceneEditorAIDialog::OnTvnSelchangedTree1)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &KSceneEditorAIDialog::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_CHECK_PATROL, &KSceneEditorAIDialog::OnBnClickedCheckPatrol)
	ON_BN_CLICKED(IDC_CHECK_PATROL2, &KSceneEditorAIDialog::OnBnClickedCheckPatrol2)
	ON_BN_CLICKED(IDC_DATASWITCH, &KSceneEditorAIDialog::OnBnClickedDataswitch)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &KSceneEditorAIDialog::OnNMClickTree1)
END_MESSAGE_MAP()


// KSceneEditorAIDialog message handlers

void KSceneEditorAIDialog::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneEditorAIDialog::FillTree()
{
	m_lpDocLogical->UpdataNpcAIParameters(m_lpDocLogical->m_bUseNpcWayPointSetPointer);

	TCHAR szName[MAX_PATH];
	FillMap();

	m_Tree.DeleteAllItems();

	wsprintf(szName,"全部NPC %d",m_lpDocLogical->m_listNPCInfo.size());
	HTREEITEM hRoot = m_Tree.InsertItem(szName,image_all,image_all,TVI_ROOT,TVI_LAST);

	map<DWORD,AISetWithNpc>::iterator i = m_mapNpcAiSet.begin();
	map<DWORD,AISetWithNpc>::iterator iend = m_mapNpcAiSet.end();
	while (i!=iend)
	{
		DWORD dwID = i->first;
		AISetWithNpc& vSet = i->second;

		wsprintf(szName,"AI Set %d %d",dwID,vSet.m_setNpc.size());

		HTREEITEM hAIItem = m_Tree.InsertItem(szName,image_aiset,image_aiset,hRoot,TVI_LAST);
		m_Tree.SetItemData(hAIItem,dwID);

		{
			int K = 0;
			set<KSceneEditorDocLogical::NPCInfo*>::iterator t = vSet.m_setNpc.begin();
			set<KSceneEditorDocLogical::NPCInfo*>::iterator tend = vSet.m_setNpc.end();
			while (t!=tend)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = *t;

				wsprintf(szName,"%d %d Npc %s %s",K,pNpc->nTempleteID,pNpc->szName.c_str(),pNpc->szNickName.c_str());
				
				HTREEITEM hNpcItem = m_Tree.InsertItem(szName,image_npc,image_npc,hAIItem,TVI_LAST);
				m_Tree.SetItemData(hNpcItem,(DWORD_PTR)pNpc);

				t++;
				K++;
			}

		}

		i++;
	}
	//////////////////////////////////////////////////////////////////////////
	if(m_bDataAnalyse)
	{
		for (size_t i=0;i<m_vecDataSet.size();i++)
		{
			DataSet& dataset = m_vecDataSet[i];
			
			wsprintf(szName,"序号:%d 参数名:%s",i,dataset.m_szName.c_str());
			HTREEITEM hIndex = m_Tree.InsertItem(szName,image_AIPAR,image_AIPAR,TVI_ROOT,TVI_LAST);

			map<std::string,DataCollection>::iterator t = dataset.m_mapDataCollections.begin();
			map<std::string,DataCollection>::iterator tend = dataset.m_mapDataCollections.end();
			while (t!=tend)
			{
				std::string strValue= t->first;
				DataCollection& coll = t->second;

				wsprintf(szName,"值:%s --- NPC个数:%d",strValue,coll.m_setNpc.size());
				HTREEITEM hDataCollection = m_Tree.InsertItem(szName,image_aivalue,image_aivalue,hIndex,TVI_LAST);
				m_Tree.SetItemData(hDataCollection,(DWORD_PTR)&coll);

				{
					int K = 0;
					set<KSceneEditorDocLogical::NPCInfo*>::iterator t = coll.m_setNpc.begin();
					set<KSceneEditorDocLogical::NPCInfo*>::iterator tend = coll.m_setNpc.end();
					while (t!=tend)
					{
						KSceneEditorDocLogical::NPCInfo* pNpc = *t;

						wsprintf(szName,"%d %d Npc %s %s",K,pNpc->nTempleteID,pNpc->szName.c_str(),pNpc->szNickName.c_str());

						HTREEITEM hNpcItem = m_Tree.InsertItem(szName,image_npc,image_npc,hDataCollection,TVI_LAST);
						m_Tree.SetItemData(hNpcItem,(DWORD_PTR)pNpc);

						t++;
						K++;
					}

				}

				t++;
			}

		}
	}


}

void KSceneEditorAIDialog::FillMap()
{
	m_mapNpcAiSet.clear();
	KGLOG_PROCESS_ERROR(m_lpDocLogical);

	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::NPCInfo* pInfo = *i;

			map<DWORD,AISetWithNpc>::iterator tFind = m_mapNpcAiSet.find(pInfo->dwNpcAISet);
			if(tFind == m_mapNpcAiSet.end())
			{
				AISetWithNpc vSet;
				vSet.dwNpcAISet = pInfo->dwNpcAISet;
				vSet.m_setNpc.insert(pInfo);
				m_mapNpcAiSet[pInfo->dwNpcAISet] = vSet;
			}
			else
			{
				AISetWithNpc& vSet = tFind->second;
				vSet.m_setNpc.insert(pInfo);
			}

			i++;
		}

	}

Exit0:
	;
}

void KSceneEditorAIDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		FillTree();
	}
}

void KSceneEditorAIDialog::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneEditorAIDialog::FillList(set<KSceneEditorDocLogical::NPCInfo*>* psetNpc)
{
	m_ListCtrl.DeleteAllItems();

	int nIndex = 0;
	set<KSceneEditorDocLogical::NPCInfo*>::iterator i = psetNpc->begin();
	set<KSceneEditorDocLogical::NPCInfo*>::iterator iend = psetNpc->end();
	while (i!=iend)
	{
		KSceneEditorDocLogical::NPCInfo* pNpc = *i;

		{
			TCHAR szBuf[256];
			LV_ITEM lvItem;
			{
				wsprintf( szBuf, _T( "%s" ), pNpc->szName.c_str() );
				lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
				lvItem.iItem = nIndex;      
				lvItem.iSubItem = 0;
				lvItem.iImage   = 0;
				lvItem.pszText = szBuf; 
				int iHandle = m_ListCtrl.InsertItem( &lvItem );
				m_ListCtrl.SetItemData(iHandle,nIndex); 
			}

			ASSERT(pNpc->m_cAIParameterSet.vecParameters.size());

			for (size_t t=0;t<pNpc->m_cAIParameterSet.vecParameters.size();t++)
			{
				KSceneEditorDocLogical::AIParameter& aipar = pNpc->m_cAIParameterSet.vecParameters[t];
				{
					wsprintf( szBuf, _T( "%s" ) , aipar.strValue );
					lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
					lvItem.iItem = nIndex;      
					lvItem.iSubItem = (int)t+1;
					lvItem.iImage   = 0;
					lvItem.pszText = szBuf; 
					m_ListCtrl.SetItem(&lvItem );
				}
			}
			nIndex++;
		}
		i++;
	}
}
BOOL KSceneEditorAIDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_lpDocLogical)
	{
		m_ListCtrl.SetExtendedStyle(
			m_ListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
			);

		TCHAR cluName[MAX_PATH];
		wsprintf(cluName,"NPC name ");
		m_ListCtrl.InsertColumn(0, cluName,	LVCFMT_CENTER, 70);

		for (int i=0;i<(int)m_lpDocLogical->m_AIParameterDefine.vecParameters.size();i++)
		{
			KSceneEditorDocLogical::AIParameter& aipar = m_lpDocLogical->m_AIParameterDefine.vecParameters[i];
			wsprintf(cluName,"%s",aipar.strName.c_str());

			m_ListCtrl.InsertColumn(i+1, cluName,	LVCFMT_CENTER, 70);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditorAIDialog::OnBnClickedCheckPatrol()
{
	UpdateData(TRUE);
}

void KSceneEditorAIDialog::OnBnClickedCheckPatrol2()
{
	UpdateData(TRUE);
}

void KSceneEditorAIDialog::OnBnClickedDataswitch()
{
	UpdateData(TRUE);
	if(m_bDataAnalyse)
	{
		AnalyseNpcAI();
	}
	FillTree();
}

void KSceneEditorAIDialog::AnalyseNpcAI()
{
	m_vecDataSet.clear();

	size_t t = m_lpDocLogical->m_AIParameterDefine.vecParameters.size();
	
	m_vecDataSet.resize(t);
	for (size_t i=0;i<t;i++)
	{
		KSceneEditorDocLogical::AIParameter& par = m_lpDocLogical->m_AIParameterDefine.vecParameters[i];
		m_vecDataSet[i].m_szName = par.strName;
	}
	//////////////////////////////////////////////////////////////////////////list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
	list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
	list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
	while (i!=iend)
	{
		KSceneEditorDocLogical::NPCInfo* pInfo = *i;

		for (size_t s =0;s<pInfo->m_cAIParameterSet.vecParameters.size();s++)
		{
			KSceneEditorDocLogical::AIParameter& par = pInfo->m_cAIParameterSet.vecParameters[s];

			DataSet& destSet = m_vecDataSet[s];
			
			map<std::string,DataCollection>::iterator iFind = destSet.m_mapDataCollections.find(par.strValue);
			if(iFind==destSet.m_mapDataCollections.end())
			{
				DataCollection newCollect;
				newCollect.m_setNpc.insert(pInfo);
				destSet.m_mapDataCollections[par.strValue] = newCollect;
			}
			else
			{
				DataCollection& collect = iFind->second;
				collect.m_setNpc.insert(pInfo);
			}
		}

		i++;
	}
}

void KSceneEditorAIDialog::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem)
	{
		int nType = 0;
		m_Tree.GetItemImage(hItem,nType,nType);

		if(nType == image_all)
		{
			set<KSceneEditorDocLogical::NPCInfo*>setNpc;
			list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
			list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
			while (i!=iend)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = *i;
				setNpc.insert(pNpc);
				i++;
			}
			FillList(&setNpc);
		}
		else if(nType == image_aiset)
		{
			DWORD AISetID = (DWORD)m_Tree.GetItemData(hItem);

			map<DWORD,AISetWithNpc>::iterator i = m_mapNpcAiSet.find(AISetID);
			if(i!=m_mapNpcAiSet.end())
			{
				AISetWithNpc& aiset = i->second;

				FillList(&aiset.m_setNpc);

				m_lpSceneSceneEditor->ClearSelectedEntityList();
				set<KSceneEditorDocLogical::NPCInfo*>::iterator t = aiset.m_setNpc.begin();
				set<KSceneEditorDocLogical::NPCInfo*>::iterator tend = aiset.m_setNpc.end();

				while(t!=tend)
				{
					KSceneEditorDocLogical::NPCInfo* pNpc = *t;
					if(pNpc->pPointer)
						m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
					t++;
				}
			}
		}
		else if(nType == image_npc)
		{


			KSceneEditorDocLogical::NPCInfo* pInfo = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);
			set<KSceneEditorDocLogical::NPCInfo*>setNpc;
			setNpc.insert(pInfo);
			FillList(&setNpc);

			m_lpSceneSceneEditor->ClearSelectedEntityList();
			if(pInfo->pPointer)
			{
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pInfo->pPointer, TRUE);
			};	
		}
		else if(nType == image_aivalue)
		{
			DataCollection* pCollection = (DataCollection*)m_Tree.GetItemData(hItem);
			if(pCollection)
			{
				FillList(&pCollection->m_setNpc);
			}

			m_lpSceneSceneEditor->ClearSelectedEntityList();
			set<KSceneEditorDocLogical::NPCInfo*>::iterator t = pCollection->m_setNpc.begin();
			set<KSceneEditorDocLogical::NPCInfo*>::iterator tend = pCollection->m_setNpc.end();

			while(t!=tend)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = *t;
				if(pNpc->pPointer)
					m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
				t++;
			}
		}	
	}
}
