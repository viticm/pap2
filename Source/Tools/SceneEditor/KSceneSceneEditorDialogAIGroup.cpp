// KSceneSceneEditorDialogAIGroup.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogAIGroup.h"

// KSceneSceneEditorDialogAIGroup dialog
#include "IEEditor.h"
#include "KSceneEditorDocLogical.h"

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogAIGroup, CDialog)

KSceneSceneEditorDialogAIGroup::KSceneSceneEditorDialogAIGroup(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogAIGroup::IDD, pParent)
{
	m_lpSceneSceneEditor = NULL;
	m_lpDocLogical = NULL;

}

KSceneSceneEditorDialogAIGroup::~KSceneSceneEditorDialogAIGroup()
{
}

void KSceneSceneEditorDialogAIGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NPC, m_Tree);
	DDX_Control(pDX, IDC_EDIT_GROUPNAME, m_Edit_GroupName);
	DDX_Control(pDX, IDC_CHECK_KING, m_Check_King);
	DDX_Control(pDX, IDC_COMBO_WAYPOINTSE, m_Combo_Way);
	DDX_Control(pDX, IDC_BUTTON_BROWN, m_Button_ArrayBrown);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_Button_ArrayClear);
	DDX_Control(pDX, IDC_EDIT_MATRIXFILE, m_Edit_ArrayFile);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogAIGroup, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_FIT, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonFit)
	ON_BN_CLICKED(IDC_CHECK_KING, &KSceneSceneEditorDialogAIGroup::OnBnClickedCheckKing)
	ON_BN_CLICKED(IDC_BUTTON_ADDSET, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonAddset)
	ON_BN_CLICKED(IDC_BUTTON_REMOVESET, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonRemoveset)
	ON_EN_CHANGE(IDC_EDIT_GROUPNAME, &KSceneSceneEditorDialogAIGroup::OnEnChangeEditGroupname)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneSceneEditorDialogAIGroup::OnTvnSelchangedTreeNpc)
	ON_CBN_SELCHANGE(IDC_COMBO_WAYPOINTSE, &KSceneSceneEditorDialogAIGroup::OnCbnSelchangeComboWaypointse)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NPC, &KSceneSceneEditorDialogAIGroup::OnNMDblclkTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_BROWN, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonBrown)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &KSceneSceneEditorDialogAIGroup::OnBnClickedButtonClear)
	ON_EN_CHANGE(IDC_EDIT_MATRIXFILE, &KSceneSceneEditorDialogAIGroup::OnEnChangeEditMatrixfile)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogAIGroup message handlers

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonRefresh()
{
	FillTree();
}

void KSceneSceneEditorDialogAIGroup::FillTree()
{
	m_Tree.DeleteAllItems();

	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;

	TCHAR Name[MAX_PATH];

	list<KSceneEditorDocLogical::AIGroup*>::iterator i = m_lpDocLogical->m_listAIGroup.begin();
	while (i!=m_lpDocLogical->m_listAIGroup.end())
	{
		KSceneEditorDocLogical::AIGroup* pGroup = *i;

		HTREEITEM hGroup = m_Tree.InsertItem(pGroup->szName.c_str(),0,0,TVI_ROOT,TVI_LAST);
		m_Tree.SetItemData(hGroup,(DWORD_PTR)pGroup);

        size_t uNpcInfoSize = pGroup->vecNpcInfo.size();

		for(size_t s = 0; s < uNpcInfoSize; ++s)
		{
			KSceneEditorDocLogical::NPCInfo* pNPC = pGroup->vecNpcInfo[s];
			
			if(s == (size_t)(pGroup->nKing))
				wsprintf(Name,"%s King",pNPC->szName.c_str());
			else
				wsprintf(Name,"%s",pNPC->szName.c_str());

			HTREEITEM hNPC = m_Tree.InsertItem(Name,1,1,hGroup,TVI_LAST);
			m_Tree.SetItemData(hNPC,(DWORD_PTR)pNPC);
		}
		i++;
	}

}

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonAdd()
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

	if(nImage==0)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		m_lpSceneSceneEditor->AddSelectedEntityToAIGroup(pGroup->dwSetID);
	}
}

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonRemove()
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

	if(nImage==0)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		m_lpSceneSceneEditor->RemoveSelectedEntityFromAIGroup(pGroup->dwSetID);
	}

}

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonFit()
{
	// TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogAIGroup::OnBnClickedCheckKing()
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

		if(nImage==1)
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

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonAddset()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(!m_lpDocLogical)
		return;
	KSceneEditorDocLogical::AIGroup* pGroup = NULL;
	m_lpDocLogical->AddOneAIGroup(&pGroup,"AI Group");
	FillTree();
}

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonRemoveset()
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

	if(nImage==0)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		m_lpDocLogical->DeleteAIGroup(pGroup);
		FillTree();
	}
}

void KSceneSceneEditorDialogAIGroup::OnEnChangeEditGroupname()
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

	if(nImage==0)
	{
		UpdateData(TRUE);
		TCHAR Name[MAX_PATH];
		m_Edit_GroupName.GetWindowText(Name,MAX_PATH);

		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		pGroup->szName = Name;
		m_Tree.SetItemText(hItem,Name);
	}
}

void KSceneSceneEditorDialogAIGroup::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	switch(nImage)
	{
	case 0:
		{
			m_Edit_GroupName.EnableWindow(TRUE);
			m_Edit_ArrayFile.EnableWindow(TRUE);
			m_Check_King.EnableWindow(FALSE);
			m_Combo_Way.EnableWindow(TRUE);
			m_Button_ArrayBrown.EnableWindow(TRUE);
			m_Button_ArrayClear.EnableWindow(TRUE);

			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
			m_Edit_GroupName.SetWindowText(pGroup->szName.c_str());
			m_Edit_ArrayFile.SetWindowText(pGroup->szFileName.c_str());

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

		}
		break;
	case 1:
		{
			m_Edit_GroupName.EnableWindow(FALSE);
			m_Edit_ArrayFile.EnableWindow(FALSE);
			m_Check_King.EnableWindow(TRUE);
			m_Combo_Way.EnableWindow(FALSE);
			m_Button_ArrayBrown.EnableWindow(FALSE);
			m_Button_ArrayClear.EnableWindow(FALSE);

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
	default:
	    break;
	}
}

void KSceneSceneEditorDialogAIGroup::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	if(nSinkID==EVENTDOCLOGIGAL_UPDATEUI)
	{
		FillTree();
		FillCombo();
	}
}

BOOL KSceneSceneEditorDialogAIGroup::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_lpDocLogical)
		m_lpDocLogical->Advise(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogAIGroup::OnCbnSelchangeComboWaypointse()
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
	case 0:
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

void KSceneSceneEditorDialogAIGroup::FillCombo()
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

void KSceneSceneEditorDialogAIGroup::OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

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

	switch(nImage)
	{
	case 0:
		{
			KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
			for (size_t i=0;i<pGroup->vecNpcInfo.size();i++)
			{
				KSceneEditorDocLogical::NPCInfo* pNpc = pGroup->vecNpcInfo[i];
				m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
			}
		}
		break;
	case 1:
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)(m_Tree.GetItemData(hItem));
			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
		}
		break;
	default:
		break;
	}
}

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonBrown()
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    TCHAR szAppDirectory[MAX_PATH] = { 0 };

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

	if(nImage == 0)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		
		CFileDialog dlg(true, "Tab", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "½£Íø3DÕóÐÍÎÄ¼þ(*.Tab)|*.Tab||");
		if(dlg.DoModal() == IDOK)
		{
			TCHAR FileName[MAX_PATH];
			TCHAR RetFileName[MAX_PATH];
			wsprintf(FileName,"%s",dlg.GetPathName());
            pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
			pManager->GetRelativePath(szAppDirectory,FileName,RetFileName);

			pGroup->szFileName = RetFileName;
			m_Edit_ArrayFile.SetWindowText(RetFileName);
		}
	}
}

void KSceneSceneEditorDialogAIGroup::OnBnClickedButtonClear()
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

	if(nImage == 0)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		pGroup->szFileName = "";
		m_Edit_ArrayFile.SetWindowText("");
	}
}

void KSceneSceneEditorDialogAIGroup::OnEnChangeEditMatrixfile()
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

	if(nImage == 0)
	{
		KSceneEditorDocLogical::AIGroup* pGroup = (KSceneEditorDocLogical::AIGroup*)(m_Tree.GetItemData(hItem));
		TCHAR Name[MAX_PATH];
		m_Edit_ArrayFile.GetWindowText(Name,MAX_PATH);
		pGroup->szFileName = Name;
	}
}
