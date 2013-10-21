// KSceneSceneEditorDialogWayPoint.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogWayPoint.h"

#include "KSceneEditorDocLogical.h"
#include "IEEditor.h"
#include "KSceneHelperFuncs.h"

//#include "SceneEditorCommon.h"

// KSceneSceneEditorDialogWayPoint dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogWayPoint, CDialog)

KSceneSceneEditorDialogWayPoint::KSceneSceneEditorDialogWayPoint(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogWayPoint::IDD, pParent)
	, m_nStayFrame(0)
	, m_nStayDirection(0)
	, m_nStayAni(0)
	, m_bStayAniLoop(FALSE)
	, m_bIsRun(FALSE)
	//, m_nOrderID(1)
	, m_nPatrolPathOrderID(0)
	, m_nPatrolPathWalkSpeed(0)
{
	m_lpSceneSceneEditor = NULL;
	m_lpDocLogical = NULL;
}

KSceneSceneEditorDialogWayPoint::~KSceneSceneEditorDialogWayPoint()
{
}

void KSceneSceneEditorDialogWayPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NPC, m_Tree);
	DDX_Control(pDX, IDC_EDIT_SETNAME, m_Edit_SetName);
	DDX_Control(pDX, IDC_EDIT_STAYFRAME, m_Edit_StayFrame);
	DDX_Control(pDX, IDC_EDIT_STAYDIRECTION, m_Edit_StayDirection);
	DDX_Control(pDX, IDC_EDIT_STAYANI, m_Edit_StayAni);
	DDX_Control(pDX, IDC_CHECK_ANILOOP, m_Check_AniLoop);
	DDX_Control(pDX, IDC_CHECK_ISRUN, m_Check_IsRun);
	DDX_Control(pDX, IDC_BUTTON_BROWNSCRIPT, m_Button_BrownScript);
	DDX_Control(pDX, IDC_BUTTON_CLEARSCRIPT, m_Button_ClearScript);
	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_Edit_ScripName);
	DDX_Control(pDX, IDC_EDIT_INFO, m_Edit_PointInfo);
	DDX_Text(pDX, IDC_EDIT_STAYFRAME, m_nStayFrame);
	DDV_MinMaxInt(pDX, m_nStayFrame, 0, 90000);
	DDX_Text(pDX, IDC_EDIT_STAYDIRECTION, m_nStayDirection);
	DDV_MinMaxInt(pDX, m_nStayDirection, -1, 256);
	DDX_Text(pDX, IDC_EDIT_STAYANI, m_nStayAni);
	DDX_Check(pDX, IDC_CHECK_ANILOOP, m_bStayAniLoop);
	DDX_Check(pDX, IDC_CHECK_ISRUN, m_bIsRun);
	DDX_Control(pDX, IDC_BUTTON_DELETESET, m_Button_DeleteSet);
	DDX_Control(pDX, IDC_BUTTON_ADD2, m_Button_AddPoint);
	//DDX_Text(pDX, IDC_EDIT_ORDERID, m_nOrderID);
	DDX_Control(pDX,IDC_EDIT_ORDERID,m_Edit_OrderID);
	DDX_Text(pDX, IDC_EDIT_ORDERID, m_nPatrolPathOrderID);
	DDX_Text(pDX, IDC_EDIT_ORDERID2, m_nPatrolPathWalkSpeed);
	DDX_Control(pDX, IDC_COMBO_PathOrder, m_Combo_PathOrder);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogWayPoint, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonAdd)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneSceneEditorDialogWayPoint::OnTvnSelchangedTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ADD2, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonAdd2)
	ON_EN_CHANGE(IDC_EDIT_SETNAME, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditSetname)
	ON_EN_CHANGE(IDC_EDIT_STAYFRAME, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditStayframe)
	ON_EN_CHANGE(IDC_EDIT_STAYDIRECTION, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditStaydirection)
	ON_EN_CHANGE(IDC_EDIT_STAYANI, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditStayani)
	ON_BN_CLICKED(IDC_CHECK_ANILOOP, &KSceneSceneEditorDialogWayPoint::OnBnClickedCheckAniloop)
	ON_BN_CLICKED(IDC_CHECK_ISRUN, &KSceneSceneEditorDialogWayPoint::OnBnClickedCheckIsRun)
	ON_EN_CHANGE(IDC_EDIT_INFO, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditInfo)
	ON_EN_CHANGE(IDC_EDIT_SCRIPT, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditScript)
	ON_BN_CLICKED(IDC_BUTTON_BROWNSCRIPT, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonBrownscript)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSCRIPT, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonClearscript)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NPC, &KSceneSceneEditorDialogWayPoint::OnNMDblclkTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_DELETESET, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonDeleteset)
	ON_EN_CHANGE(IDC_EDIT_ORDERID,&KSceneSceneEditorDialogWayPoint::OnEnChangeEditOrderid)
	ON_EN_CHANGE(IDC_EDIT_ORDERID2, &KSceneSceneEditorDialogWayPoint::OnEnChangeEditOrderid2)
	ON_BN_CLICKED(IDC_BUTTON_AUTOINDEX, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonAutoindex)
	ON_CBN_SELCHANGE(IDC_COMBO_PathOrder, &KSceneSceneEditorDialogWayPoint::OnCbnSelchangeComboPathorder)
	ON_BN_CLICKED(IDC_BUTTON_CLONE, &KSceneSceneEditorDialogWayPoint::OnBnClickedButtonClone)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogWayPoint message handlers
// KSceneSceneEditorDialogWayPoint.cpp : implementation file
//
void KSceneSceneEditorDialogWayPoint::FillTree()
{
	m_Tree.DeleteAllItems();
	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;

	map<KSceneEditorDocLogical::WayPointSet*,HTREEITEM>maphItemWayPointSet;

	//////////////////////////////////////////////////////////////////////////
	m_lpSceneSceneEditor->ClearSetHelpLine(0);

	TCHAR name[MAX_PATH];
	DWORD dwNum = 0;
	int K = 0;
	map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
	while (i!=m_lpDocLogical->m_mapWayPointSet.end())
	{
		DWORD dwID = i->first;
		KSceneEditorDocLogical::WayPointSet* pSet = i->second;
		wsprintf(name,"%d: %s %d",pSet->nIndex-1,pSet->szName.c_str(),pSet->vectorWayPoint.size());
		HTREEITEM hSet = m_Tree.InsertItem(name,image_WayPointSet,image_WayPointSet,TVI_ROOT,TVI_LAST);
		m_Tree.SetItemData(hSet,(DWORD_PTR)pSet);
		
		maphItemWayPointSet[pSet] = hSet;

		dwNum = m_lpSceneSceneEditor->GetHelpLineSize();
		DWORD R = rand()%256;
		DWORD G = rand()%256;
		DWORD B = rand()%256;

		D3DXCOLOR vColor = 0xFF000000 | R<<24 | G<<16 | B;
		//////////////////////////////////////////////////////////////////////////
		for(size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];
			wsprintf(name,"WayPoint %s",pPoint->szInfo.c_str());
			HTREEITEM hWayPoint = m_Tree.InsertItem(name,image_WayPoint,image_WayPoint,hSet,TVI_LAST);
			m_Tree.SetItemData(hWayPoint,(DWORD_PTR)pPoint);

			IEKG3DRepresentWayPoint* pWayPoint = m_lpSceneSceneEditor->GetIEKG3DRepresentWayPoint(pPoint->pPointer);
			if(s==0)
				pWayPoint->SetWayPointType(0);
			else if(s==pSet->vectorWayPoint.size()-1)
				pWayPoint->SetWayPointType(2);
			else
				pWayPoint->SetWayPointType(1);

			D3DXVECTOR3 vPos = pPoint->vPosition + D3DXVECTOR3(0,50,0);
			
			m_lpSceneSceneEditor->SetHelpLine(dwNum,0 ,s, 1, vColor, &vPos);
		}
		m_lpSceneSceneEditor->SetHelpLineName(dwNum,(LPSTR)pSet->szName.c_str());

		i++;
		//K++;
	}
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;
			if(pNpc->pWayPointSet)
			{
				map<KSceneEditorDocLogical::WayPointSet*,HTREEITEM>::iterator sFind = maphItemWayPointSet.find(pNpc->pWayPointSet);
				if(sFind!=maphItemWayPointSet.end())
				{
					int nNpcPathIndex = 0;
					m_lpDocLogical->GetAIParam(pNpc->m_cAIParameterSet,"PatrolOrderIndex",nNpcPathIndex);
					HTREEITEM hSet = sFind->second;
					wsprintf(name,"Npc %s %s %d",pNpc->szName.c_str(),pNpc->szNickName.c_str(),nNpcPathIndex);
					HTREEITEM hNpc = m_Tree.InsertItem(name,image_Npc,image_Npc,hSet,TVI_LAST);
					m_Tree.SetItemData(hNpc,(DWORD_PTR)pNpc);
				}
			}

			i++;
		}
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonAdd()
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	size_t t = m_lpDocLogical->m_mapWayPointSet.size();
	TCHAR name[MAX_PATH];
	wsprintf(name,"WayPointSet%d",t);

	KSceneEditorDocLogical::WayPointSet* pSet = NULL;
	m_lpDocLogical->AddOneWayPointSet(&pSet,name,TRUE);

	FillTree();

}

void KSceneSceneEditorDialogWayPoint::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
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
	//DWORD_PTR dwValue = m_Tree.GetItemData(hItem);

	if(nImage==image_WayPointSet)//set
	{
		m_Edit_SetName.EnableWindow(TRUE);
		m_Edit_OrderID.EnableWindow(TRUE);
		m_Edit_StayFrame.EnableWindow(FALSE);
		m_Edit_StayDirection.EnableWindow(FALSE);
		m_Edit_StayAni.EnableWindow(FALSE);
		m_Check_AniLoop.EnableWindow(FALSE);
		m_Check_IsRun.EnableWindow(FALSE);
		m_Button_BrownScript.EnableWindow(FALSE);
		m_Button_ClearScript.EnableWindow(FALSE);
		m_Edit_ScripName.EnableWindow(FALSE);
		m_Edit_PointInfo.EnableWindow(FALSE);
		m_Button_DeleteSet.EnableWindow(TRUE);
		m_Button_AddPoint.EnableWindow(TRUE);

		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);
	
		//TCHAR Name[MAX_PATH];
		//wsprintf(Name,"%s %d",pSet->szName.c_str(),pSet->vectorWayPoint.size());

		m_Edit_SetName.SetWindowText(pSet->szName.c_str());
		m_nPatrolPathOrderID = pSet->nPatrolPathOrderID;
		m_nPatrolPathWalkSpeed = pSet->nPatrolPathWalkSpeed;

		m_Combo_PathOrder.SetCurSel(m_nPatrolPathOrderID - 1);

		UpdateData(FALSE);
	}
	else if(nImage==image_WayPoint)
	{
		m_Edit_SetName.EnableWindow(FALSE);
		m_Edit_OrderID.EnableWindow(FALSE);
		m_Edit_StayFrame.EnableWindow(TRUE);
		m_Edit_StayDirection.EnableWindow(TRUE);
		m_Edit_StayAni.EnableWindow(TRUE);
		m_Check_AniLoop.EnableWindow(TRUE);
		m_Check_IsRun.EnableWindow(TRUE);
		m_Button_BrownScript.EnableWindow(TRUE);
		m_Button_ClearScript.EnableWindow(TRUE);
		m_Edit_ScripName.EnableWindow(TRUE);
		m_Edit_PointInfo.EnableWindow(TRUE);
		m_Button_DeleteSet.EnableWindow(FALSE);
		m_Button_AddPoint.EnableWindow(FALSE);

		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);

		m_nStayFrame = pPoint->nStayFrame;
		m_nStayDirection = pPoint->nStayDirection;
		m_nStayAni = pPoint->nStayAnimation;
		m_bStayAniLoop = pPoint->bStayAniLoop;
		m_bIsRun = pPoint->bIsRun;

		UpdateData(FALSE);

		m_Edit_ScripName.SetWindowText(pPoint->szScriptName.c_str());
		m_Edit_PointInfo.SetWindowText(pPoint->szInfo.c_str());
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonRefresh()
{
	FillTree();
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonAdd2()
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

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);
		DWORD dwSetID = pSet->dwSetID;
		IEKG3DRepresentObject* pObj = BuildWayPoint(g_GetEngineIEInterface(), dwSetID, _T("WayPoint"));
		HRESULT hr = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pObj);
		}
	}
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditSetname()
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

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);
		TCHAR Name[MAX_PATH];
		m_Edit_SetName.GetWindowText(Name,MAX_PATH);
		pSet->szName = Name;

		map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i =m_lpDocLogical->m_mapWayPointSet.find(pSet->dwSetID);
		map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator iend = m_lpDocLogical->m_mapWayPointSet.end();
		if (i!=iend)
		{
			wsprintf(Name,"%d: %s %d",pSet->nIndex-1,pSet->szName.c_str(),pSet->vectorWayPoint.size());
			m_Tree.SetItemText(hItem,Name);
		}
	}
}

void KSceneSceneEditorDialogWayPoint::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditStayframe()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		pPoint->nStayFrame = m_nStayFrame;
	}
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditStaydirection()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		pPoint->nStayDirection = m_nStayDirection;
	}
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditStayani()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		pPoint->nStayAnimation = m_nStayAni;
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedCheckAniloop()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		pPoint->bStayAniLoop = m_bStayAniLoop;
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedCheckIsRun()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	UpdateData(TRUE);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		pPoint->bIsRun = m_bIsRun;
	}
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditInfo()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		TCHAR Name[MAX_PATH];
		m_Edit_PointInfo.GetWindowText(Name,MAX_PATH);
		pPoint->szInfo = Name;

		TCHAR szname[MAX_PATH];
		wsprintf(szname,"WayPoint %s",pPoint->szInfo.c_str());
		m_Tree.SetItemText(hItem,szname);
	}
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditScript()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		TCHAR Name[MAX_PATH];
		m_Edit_ScripName.GetWindowText(Name,MAX_PATH);
		pPoint->szScriptName = Name;
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonBrownscript()
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    TCHAR szAppDir[MAX_PATH] = { 0 };
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);

		CFileDialog dlg(true, "Lua", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D脚本文件(*.Lua)|*.Lua||");
		if(dlg.DoModal() == IDOK)
		{
			TCHAR FileName[MAX_PATH];
			TCHAR RetFileName[MAX_PATH];
			wsprintf(FileName,"%s",dlg.GetPathName());
            pManager->GetRelativePath(g_szDefWorkDirectory,FileName,RetFileName);

			pPoint->szScriptName = RetFileName;
			m_Edit_ScripName.SetWindowText(RetFileName);
		}
	}

}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonClearscript()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		pPoint->szScriptName = "";
		m_Edit_ScripName.SetWindowText("");

	}
}
void KSceneSceneEditorDialogWayPoint::OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);

		m_lpSceneSceneEditor->ClearSelectedEntityList();
		
		for(size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];
			
			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
		}
	}
	else if(nImage==image_WayPoint)
	{
		KSceneEditorDocLogical::WayPoint* pPoint = (KSceneEditorDocLogical::WayPoint*)m_Tree.GetItemData(hItem);
		
		m_lpSceneSceneEditor->ClearSelectedEntityList();
		m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
	}
	else if(nImage==image_Npc)
	{
		KSceneEditorDocLogical::NPCInfo* pNpc = (KSceneEditorDocLogical::NPCInfo*)m_Tree.GetItemData(hItem);

		m_lpSceneSceneEditor->ClearSelectedEntityList();
		if(pNpc->pPointer)
		{
			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);
		}
	}
}

void KSceneSceneEditorDialogWayPoint::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	switch(nSinkID)
	{
	case EVENTDOCLOGIGAL_WAYPOINT_ADDED:
	case EVENTDOCLOGIGAL_WAYPOINT_DELETED:
	case EVENTDOCLOGIGAL_WAYPOINT_MOVED:
	case EVENTDOCLOGIGAL_WAYPOINT_ROTATED:
		FillTree();
		break;
	default:
	    break;
	}
}

void KSceneSceneEditorDialogWayPoint::FillPathOrderCombo(int nID,LPSTR pName)
{
	int nIndex = m_Combo_PathOrder.AddString(pName);
	m_Combo_PathOrder.SetItemData(nIndex,nID);

}
BOOL KSceneSceneEditorDialogWayPoint::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_lpDocLogical)
		m_lpDocLogical->Advise(this);

	FillPathOrderCombo(1,"1 三角阵");
	FillPathOrderCombo(2,"2 七星阵");
	FillPathOrderCombo(3,"3 八人两排巡逻");
	FillPathOrderCombo(4,"4 十人两排巡逻");
	FillPathOrderCombo(5,"5 六人两排巡逻");
	FillPathOrderCombo(6,"6 二人两排巡逻");
	FillPathOrderCombo(7,"7 五人无序游客阵型");
	FillPathOrderCombo(8,"8 五动物无序线形阵型");
	FillPathOrderCombo(9,"9 6人表现用单排线性阵型");
	FillPathOrderCombo(10,"10 3人表现用单排线性阵型");
	FillPathOrderCombo(11,"11 10人凌乱阵型");
	FillPathOrderCombo(12,"12 十人两排巡逻");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonDeleteset()
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);

		m_lpSceneSceneEditor->ClearSelectedEntityList();

		for(size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];

			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
		}

		m_lpSceneSceneEditor->DeleteSelectedEntity();

		m_lpDocLogical->DeleteWayPointSet(pSet);

		FillTree();
	}
}

void KSceneSceneEditorDialogWayPoint::OnEnChangeEditOrderid()
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;
	UpdateData(TRUE);
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);
		pSet->nPatrolPathOrderID = m_nPatrolPathOrderID;
		m_Combo_PathOrder.SetCurSel(m_nPatrolPathOrderID-1);
	}
}
void KSceneSceneEditorDialogWayPoint::OnEnChangeEditOrderid2()
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;
	UpdateData(TRUE);
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);
		pSet->nPatrolPathWalkSpeed = m_nPatrolPathWalkSpeed;
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonAutoindex()
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;
	UpdateData(TRUE);
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);

		vector<KSceneEditorDocLogical::NPCInfo*>vecNpc;

		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_lpDocLogical->m_listNPCInfo.end();
		while (i!=iend)
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;
			if(pNpc->pWayPointSet == pSet)
			{
				vecNpc.push_back(pNpc);
			}
			i++;
		}

		for (size_t t=0;t<vecNpc.size();t++)
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = vecNpc[t];
			m_lpDocLogical->SetAIParam(pNpc->m_cAIParameterSet,"PatrolOrderIndex",(int)t);
			pNpc->dwNpcAISet = m_lpDocLogical->FindAIParameterSet(pNpc->nTempleteID,pNpc->m_cAIParameterSet);
		}

		FillTree();
	}
}

void KSceneSceneEditorDialogWayPoint::OnCbnSelchangeComboPathorder()
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;
	UpdateData(TRUE);
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		int nIndex = m_Combo_PathOrder.GetCurSel();
		if(nIndex)
		{
			KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);

			m_nPatrolPathOrderID = (int)m_Combo_PathOrder.GetItemData(nIndex);
			pSet->nPatrolPathOrderID = m_nPatrolPathOrderID;
			UpdateData(FALSE);
		}
	}
}

void KSceneSceneEditorDialogWayPoint::OnBnClickedButtonClone()
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;
	UpdateData(TRUE);
	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==image_WayPointSet)
	{
		KSceneEditorDocLogical::WayPointSet* pSet = (KSceneEditorDocLogical::WayPointSet*)m_Tree.GetItemData(hItem);

		KSceneEditorDocLogical::WayPointSet* pNewSet = pSet;
		m_lpDocLogical->AddOneWayPointSet(&pNewSet,pSet->szName.c_str(),TRUE);

		pNewSet->Clone(pSet,m_lpSceneSceneEditor);

		m_lpDocLogical->SortWayPointSet();

		FillTree();
	}
}
