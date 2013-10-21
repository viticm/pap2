// KSceneSceneEditorDialogTrafficPoint.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogTrafficPoint.h"
#include "KSceneEditorDocLogical.h"
#include "IEEditor.h"
#include "KSO3World.h"
#include "KSceneHelperFuncs.h"
// KSceneSceneEditorDialogTrafficPoint dialog


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogTrafficPoint, CDialog)
//extern DWORD g_dwSmallRa;
//extern DWORD g_dwBigRa;
KSceneSceneEditorDialogTrafficPoint::KSceneSceneEditorDialogTrafficPoint(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogTrafficPoint::IDD, pParent)
{

}

KSceneSceneEditorDialogTrafficPoint::~KSceneSceneEditorDialogTrafficPoint()
{
}

void KSceneSceneEditorDialogTrafficPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NPC, m_Tree);
	//DDX_Control(pDX, IDC_BIGR, m_editBigR);
	//DDX_Control(pDX, IDC_SMALLR, m_editSmallR);
	DDX_Control(pDX, IDC_EDIT1, m_editCostMoney);
	DDX_Control(pDX, IDC_EDIT_VELOCITY, m_editVelocity);
	DDX_Control(pDX, IDC_EDIT_STEP, m_editStep);
	DDX_Control(pDX, IDC_EDIT4, editName);
	DDX_Control(pDX, IDC_EDIT5, editMapID);
	DDX_Control(pDX, IDC_EDIT6, editForceID);
	DDX_Control(pDX, IDC_EDIT7, editCityID);
	DDX_Control(pDX, IDC_EDIT8, editTongID);
	DDX_Control(pDX, IDC_BUTTON7, buttonUpdate);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogTrafficPoint, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDTRAFFICPOINT, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonAddTrafficpoint)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ADDTRAFFICPOINTSET, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonAddtrafficpointset)
	ON_BN_CLICKED(IDC_BUTTON_DELETETRAFFICPOINT, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonDeletetrafficpoint)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneSceneEditorDialogTrafficPoint::OnTvnSelchangedTreeNpc)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_NPC, &KSceneSceneEditorDialogTrafficPoint::OnTvnSelchangingTreeNpc)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NPC, &KSceneSceneEditorDialogTrafficPoint::OnNMDblclkTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON7, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButton7)
	ON_EN_CHANGE(IDC_EDIT4, &KSceneSceneEditorDialogTrafficPoint::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT1, &KSceneSceneEditorDialogTrafficPoint::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_VELOCITY, &KSceneSceneEditorDialogTrafficPoint::OnEnChangeEditVelocity)
	ON_EN_CHANGE(IDC_EDIT_STEP, &KSceneSceneEditorDialogTrafficPoint::OnEnChangeEditStep)
	ON_BN_CLICKED(IDC_BUTTONADDLITTLEPOINT, &KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonaddlittlepoint)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogTrafficPoint message handlers

void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonAddTrafficpoint()//加大点
{
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;
	
	{
		IEKG3DRepresentObject* pObj = BuildTrafficPoint(g_GetEngineIEInterface(), 0, 0, _T("TrafficPoint"));
		HRESULT hRetCode = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);//生命周期管理移交了
		if (FAILED(hRetCode))
		{
			SAFE_RELEASE(pObj);
		}
	}

}

void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}
void KSceneSceneEditorDialogTrafficPoint::FillTree()
{
	m_Tree.DeleteAllItems();
	if(!m_lpSceneSceneEditor)
		return ;
	if(!m_lpDocLogical)
		return ;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	m_lpSceneSceneEditor->ClearSetHelpLine(1);

	KG_PROCESS_ERROR(bUseSO3Logical);

	{
		TCHAR name[MAX_PATH];
		int K = 0;
		list<KSceneEditorDocLogical::TrafficPointSet*>::iterator i = m_lpDocLogical->m_listTrafficPointSet.begin();
		DWORD dwNum = 0;
		while (i!=m_lpDocLogical->m_listTrafficPointSet.end())
		{
			KSceneEditorDocLogical::TrafficPointSet* pSet = *i;
			wsprintf(name,"%d: %s %d",K,pSet->szName.c_str(),pSet->vecTrafficPoint.size());
			HTREEITEM hSet = m_Tree.InsertItem(name,0,0,TVI_ROOT,TVI_LAST);
			m_Tree.SetItemData(hSet,(DWORD_PTR)pSet);

			dwNum = m_lpSceneSceneEditor->GetHelpLineSize();
			D3DXVECTOR3 vPos = m_lpDocLogical->m_mapTrafficPoint[pSet->m_dwFromID].vPosition + D3DXVECTOR3(0,50,0);
			m_lpSceneSceneEditor->SetHelpLine(dwNum,1, 0, 1, 0xFFFFFFFF,&vPos);
			m_lpSceneSceneEditor->SetHelpLineName(dwNum,(LPSTR)pSet->szName.c_str());

			size_t s = 0;
			for(s=0;s<pSet->vecTrafficPoint.size();s++)
			{
				KSceneEditorDocLogical::TrafficLittlePoint* pPoint = pSet->vecTrafficPoint[s];
				wsprintf(name,"TrafficPoint%d",s);
				HTREEITEM hWayPoint = m_Tree.InsertItem(name,1,1,hSet,TVI_LAST);
				m_Tree.SetItemData(hWayPoint,(DWORD_PTR)pPoint);
				vPos = pPoint->vPostion + D3DXVECTOR3(0,50,0);
				m_lpSceneSceneEditor->SetHelpLine(dwNum,1, s + 1, 1, 0xFFFFFFFF, &vPos);
			}
			vPos = m_lpDocLogical->m_mapTrafficPoint[pSet->m_dwToID].vPosition + D3DXVECTOR3(0,50,0);
			m_lpSceneSceneEditor->SetHelpLine(dwNum,1, s + 1, 1, 0xFFFFFFFF,&vPos);
			m_lpSceneSceneEditor->SetHelpLineStart(dwNum,0);
			i++;
			K++;
		}
	}

Exit0:
	;
}
void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonRefresh()
{
	FillTree();
}

void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonAddtrafficpointset()//加中间的小点
{
	HRESULT hRetCode = E_FAIL;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	IEKG3DSceneEntity* pEntityFrom = NULL;
	IEKG3DSceneEntity* pEntityTo = NULL;
	IEKG3DRepresentObject* pObject = NULL;
	IEKG3DRepresentTrafficPoint* pRepFromTrafficPoint = NULL;
	IEKG3DRepresentTrafficPoint* pRepToTrafficPoint = NULL;
	TCHAR name[MAX_PATH];
	INT nSelectedNum = 0;
	DWORD dwFromID = 0;
	DWORD dwToID = 0;
	int nSetID = 0;
	DWORD dwType = 0;
	list<KSceneEditorDocLogical::TrafficPointSet*>::iterator it;
	KSceneEditorDocLogical::TrafficPointSet* pSet = NULL;

	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem)
	{
		int nImage = 0;
		m_Tree.GetItemImage(hItem,nImage,nImage);		
		if(nImage == 0)
		{		
			KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);	
			
			{
				IEKG3DRepresentObject* pObj 
					= BuildTrafficPoint(g_GetEngineIEInterface(), pSet->m_dwID, 1, _T("TrafficPoint"));
				hRetCode = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);//生命周期管理移交了
				if (FAILED(hRetCode))
				{
					SAFE_RELEASE(pObj);
				}
			}	
			
			FillTree();
			return;
		}
	}

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	m_lpSceneSceneEditor->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;

	//m_lpSceneSceneEditor->GetSelectedEntityCount(&nSelectedNum);
	nSelectedNum = pSelTool->GetSelectionCount();
	if (nSelectedNum < 2)
	{
		MessageBox("少于两个选择物体！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}

	//m_lpSceneSceneEditor->GetSelectedEntity(0,&pEntityFrom);
	pEntityFrom = KSF::GetFirstSelectedEntity(m_lpSceneSceneEditor);
	
	hRetCode = pEntityFrom->GetType(&EntityType);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(EntityType != SCENEENTITY_OBJECT)
	{
		MessageBox("第一个交通点选择错误！非物件。","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}	
	hRetCode = pEntityFrom->GetPointer((VOID **)&pObject);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(pObject->GetType() != REPRESENTOBJECT_TRAFFICPOINT)
	{
		MessageBox("第一个非交通点！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}
	pRepFromTrafficPoint = m_lpSceneSceneEditor->GetIEKG3DRepresentTrafficPoint(pObject);
	dwType = pRepFromTrafficPoint->GetKind();
	if (dwType != 0)
	{
		MessageBox("第一个非大交通点！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}
	dwFromID = pRepFromTrafficPoint->GetSetID();
	if (m_lpDocLogical->m_mapTrafficPoint.find(dwFromID) == m_lpDocLogical->m_mapTrafficPoint.end())
	{
		MessageBox("第一个大交通点错误，表现同步问题，请联系程序！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}

	//m_lpSceneSceneEditor->GetSelectedEntity(1,&pEntityTo);
	IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
	pSelTool->GetAccessProxy(&pProxy);
	if(NULL == pProxy)
		return;

	pProxy->Reset();
	pEntityTo = pProxy->Next();
	if(NULL == pEntityTo)
		return;


	hRetCode = pEntityTo->GetType(&EntityType);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(EntityType != SCENEENTITY_OBJECT)
	{
		MessageBox("第二个交通点选择错误！非物件。","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}
	hRetCode = pEntityTo->GetPointer((VOID **)&pObject);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(pObject->GetType() != REPRESENTOBJECT_TRAFFICPOINT)
	{
		MessageBox("第二个非交通点！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}
	pRepToTrafficPoint = m_lpSceneSceneEditor->GetIEKG3DRepresentTrafficPoint(pObject);
	dwType = pRepFromTrafficPoint->GetKind();
	if (dwType != 0)
	{
		MessageBox("第二个非大交通点！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}
	dwToID = pRepToTrafficPoint->GetSetID();
	if (m_lpDocLogical->m_mapTrafficPoint.find(dwToID) == m_lpDocLogical->m_mapTrafficPoint.end())
	{
		MessageBox("第二个大交通点错误，表现同步问题，请联系程序！","请先选择两个大交通点表示你要求添加的路径的起点和重点！",MB_OK);
		return;
	}

	it = m_lpDocLogical->m_listTrafficPointSet.begin();
	while (it != m_lpDocLogical->m_listTrafficPointSet.end())
	{
		pSet = *it;
		if ((pSet->m_dwFromID == dwFromID && pSet->m_dwToID == dwToID) 
			|| (pSet->m_dwFromID == dwToID && pSet->m_dwToID == dwFromID))
		{
			break;
		}
		it++;
	}
	
	if (it == m_lpDocLogical->m_listTrafficPointSet.end())
	{
		
		g_pSO3World->m_RoadManager.AddTrack(&nSetID,dwFromID,dwToID);
		size_t t = m_lpDocLogical->m_listTrafficPointSet.size();
		wsprintf(name,"WayPointSet%d",t);
		m_lpDocLogical->AddOneTrafficPointSet(&pSet,nSetID,name);
		KG_PROCESS_ERROR(pSet);
		pSet->m_dwID = nSetID;
		pSet->m_dwFromID = dwFromID;
		pSet->m_dwToID = dwToID;
		pSet->m_dwCostMoney = 1;
		pSet->m_dwVelocity = 48;
		pSet->m_dwStep = 1000;
		m_lpDocLogical->m_mapTrackByWhat[nSetID] = "";
	}
	{
		
		IEKG3DRepresentObject* pObj = BuildTrafficPoint(g_GetEngineIEInterface(), pSet->m_dwID, 1, "TrafficPoint");
		hRetCode = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);
		if (FAILED(hRetCode))
		{
			SAFE_RELEASE(pObj);
		}
		
	}
		
	FillTree();
Exit0:
	return;
}


void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonDeletetrafficpoint()
{
//	char  szText[100];
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
	if(nImage==0)//set
	{		
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);		
		m_lpSceneSceneEditor->ClearSelectedEntityList();
		for(size_t s=0;s<pSet->vecTrafficPoint.size();s++)
		{
			KSceneEditorDocLogical::TrafficLittlePoint* pPoint = pSet->vecTrafficPoint[s];
			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
		}
		m_lpSceneSceneEditor->DeleteSelectedEntity();
		g_pSO3World->m_RoadManager.DelTrack(pSet->m_dwID);
		m_lpDocLogical->DeleteTrafficPointSet(pSet);
		FillTree();
	}
	else
	{
		KSceneEditorDocLogical::TrafficLittlePoint* pPoint = (KSceneEditorDocLogical::TrafficLittlePoint*)m_Tree.GetItemData(hItem);
		m_lpSceneSceneEditor->ClearSelectedEntityList();
		m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);	
		m_lpSceneSceneEditor->DeleteSelectedEntity();
		FillTree();
	}

	return;
}

void KSceneSceneEditorDialogTrafficPoint::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	switch(nSinkID)
	{
	case EVENTDOCLOGIGAL_TRAFFICPOINT_ADDED:
	case EVENTDOCLOGIGAL_TRAFFICPOINT_MOVED:
	case EVENTDOCLOGIGAL_TRAFFICPOINT_DELETED:
		FillTree();
		break;
	default:
		break;
	}
}

BOOL KSceneSceneEditorDialogTrafficPoint::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_lpDocLogical)
		m_lpDocLogical->Advise(this);
	FillTree();		
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogTrafficPoint::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	char  szText[100];
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
	if(nImage==0)//set
	{	
		m_editCostMoney.EnableWindow(TRUE);
		m_editVelocity.EnableWindow(TRUE);
		m_editStep.EnableWindow(TRUE);
		editName.EnableWindow(TRUE);
		editMapID.EnableWindow(TRUE);
	/*	editForceID.EnableWindow(TRUE);
		editCityID.EnableWindow(TRUE);
		editTongID.EnableWindow(TRUE);*/
		buttonUpdate.EnableWindow(TRUE);
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);
		itoa(pSet->m_dwCostMoney, szText, 10);
		m_editCostMoney.SetWindowText(szText);
		itoa(pSet->m_dwVelocity,szText,10);
		m_editVelocity.SetWindowText(szText);
		itoa(pSet->m_dwStep,szText,10);
		m_editStep.SetWindowText(szText);
		editName.SetWindowText(pSet->szName.c_str());
		editMapID.SetWindowText(m_lpDocLogical->m_mapTrackByWhat[pSet->m_dwID].c_str());
	}
	else
	{
		editName.EnableWindow(FALSE);
		editMapID.EnableWindow(FALSE);
		editForceID.EnableWindow(FALSE);
		editCityID.EnableWindow(FALSE);
		editTongID.EnableWindow(FALSE);
		m_editCostMoney.EnableWindow(FALSE);
		m_editVelocity.EnableWindow(FALSE);
		m_editStep.EnableWindow(FALSE);
		buttonUpdate.EnableWindow(FALSE);
	}
	//FillTree();
	*pResult = 0;
}

void KSceneSceneEditorDialogTrafficPoint::OnTvnSelchangingTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	return;
}

void KSceneSceneEditorDialogTrafficPoint::OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_lpSceneSceneEditor) return ;

	if(!m_lpDocLogical) return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem) return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==0)
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);

		m_lpSceneSceneEditor->ClearSelectedEntityList();

		for(size_t s=0;s < pSet->vecTrafficPoint.size();s++)
		{
			KSceneEditorDocLogical::TrafficLittlePoint* pPoint = pSet->vecTrafficPoint[s];

			m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
		}
	}
	else if(nImage==1)
	{
		KSceneEditorDocLogical::TrafficLittlePoint* pPoint = (KSceneEditorDocLogical::TrafficLittlePoint*)m_Tree.GetItemData(hItem);

		m_lpSceneSceneEditor->ClearSelectedEntityList();
		m_lpSceneSceneEditor->SetSelected(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);
	}
	*pResult = 0;
}

void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButton7()
{
	CString  szText;
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
	if(nImage==0)//set
	{	
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);
		editMapID.GetWindowText(szText);
		m_lpDocLogical->m_mapTrackByWhat[pSet->m_dwID] = szText.GetBuffer();
		//pSet->m_dwMapID = (DWORD)(atof(szText.GetBuffer()));
		//editForceID.GetWindowText(szText);
		//pSet->m_dwForceID = (DWORD)(atof(szText.GetBuffer()));
		//editCityID.GetWindowText(szText);
		//pSet->m_dwCityID = (DWORD)(atof(szText.GetBuffer()));
		//editTongID.GetWindowText(szText);
		//pSet->m_dwTongID = (DWORD)(atof(szText.GetBuffer()));		
	}
}

void KSceneSceneEditorDialogTrafficPoint::OnEnChangeEdit4()
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

	if(nImage==0)
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);
		TCHAR Name[MAX_PATH];
		editName.GetWindowText(Name,MAX_PATH);
		pSet->szName = Name;

		int K = 0;
		list<KSceneEditorDocLogical::TrafficPointSet*>::iterator i=m_lpDocLogical->m_listTrafficPointSet.begin();
		list<KSceneEditorDocLogical::TrafficPointSet*>::iterator iend=m_lpDocLogical->m_listTrafficPointSet.end();
		while (i!=iend)
		{
			if(pSet == *i)
				break;
			i++;
			K++;
		}

		wsprintf(Name,"%d: %s %d",K,pSet->szName.c_str(),pSet->vecTrafficPoint.size());
		m_Tree.SetItemText(hItem,Name);
	}
}

void KSceneSceneEditorDialogTrafficPoint::OnEnChangeEdit1()
{
	CString  szText;
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	if(nImage==0)
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);
		m_editCostMoney.GetWindowText(szText);
		pSet->m_dwCostMoney = (DWORD)(atof(szText.GetBuffer()));
	}
}

void KSceneSceneEditorDialogTrafficPoint::OnEnChangeEditVelocity()
{
	CString  szText;
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);
	if(nImage==0)
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);
		m_editVelocity.GetWindowText(szText);
		pSet->m_dwVelocity = (DWORD)(atof(szText.GetBuffer()));
	}

}

void KSceneSceneEditorDialogTrafficPoint::OnEnChangeEditStep()
{

	CString  szText;
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	int nImage = 0;
	m_Tree.GetItemImage(hItem,nImage,nImage);
	if(nImage == 0)
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = (KSceneEditorDocLogical::TrafficPointSet*)m_Tree.GetItemData(hItem);
		m_editStep.GetWindowText(szText);
		pSet->m_dwStep = (DWORD)(atof(szText.GetBuffer()));
	}
}

void KSceneSceneEditorDialogTrafficPoint::OnBnClickedButtonaddlittlepoint()
{
	CString  szText;
	D3DXVECTOR3 vPos1,vPos2;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	IEKG3DSceneEntity* pEntityFrom = NULL;
	HRESULT hRetCode = E_FAIL;
	KSceneEditorDocLogical::TrafficLittlePoint* pPoint = NULL;
	IEKG3DRepresentTrafficPoint* pRepFromTrafficPoint = NULL;
	KSceneEditorDocLogical::TrafficPointSet* pSet = NULL;
	IEKG3DRepresentObject* pObject = NULL;
	DWORD dwType = 0;
	int nTempID = 0;
	DWORD dwSetID = 0; 
	if(!m_lpSceneSceneEditor)
		return ;

	if(!m_lpDocLogical)
		return ;

	//HRESULT hResult = m_lpSceneSceneEditor->GetSelectedEntity(0,&pEntityFrom);

	pEntityFrom = KSF::GetFirstSelectedEntity(m_lpSceneSceneEditor);

	//if (FAILED(hResult))
	if(NULL == pEntityFrom)
	{
		MessageBox("请先选择一个交通点。","请先选择一个将在之前插入的小交通点！",MB_OK);
		return;	
	}
	hRetCode = pEntityFrom->GetType(&EntityType);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(EntityType != SCENEENTITY_OBJECT)
	{
		MessageBox("交通点选择错误！非物件。","请先选择一个将在之前插入的小交通点！",MB_OK);
		return;
	}	
	hRetCode = pEntityFrom->GetPointer((VOID **)&pObject);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(pObject->GetType() != REPRESENTOBJECT_TRAFFICPOINT)
	{
		MessageBox("非交通点！","请先选择一个将在之前插入的小交通点！",MB_OK);
		return;
	}
	pRepFromTrafficPoint = m_lpSceneSceneEditor->GetIEKG3DRepresentTrafficPoint(pObject);
	dwType = pRepFromTrafficPoint->GetKind();
	if (dwType == 0)
	{
		MessageBox("非小交通点！","请先选择一个将在之前插入的小交通点！",MB_OK);
		return;
	}
	
	pRepFromTrafficPoint->GetTranslation(&vPos1);		
	dwSetID = pRepFromTrafficPoint->GetSetID();
	m_lpDocLogical->GetTrafficPointSetByID(&pSet,dwSetID);
	KG_PROCESS_ERROR(pSet);
	pSet->FindTrafficPointByEditorWayPoint(&pPoint,&nTempID,pObject);
	if (pPoint != NULL)
	{
		if (nTempID == 0)
		{
			vPos2 = m_lpDocLogical->m_mapTrafficPoint[pSet->m_dwFromID].vPosition;
		}
		else
		{
			vPos2 = (pSet->vecTrafficPoint[nTempID - 1])->vPostion;
		}
		IEKG3DRepresentObject* pNewRepPoint = NULL;
		KSceneEditorDocLogical::TrafficLittlePoint* pNewPoint = NULL;
		D3DXVECTOR3 vMidPos = (vPos1 + vPos2) / 2;
		m_lpSceneSceneEditor->AddTrafficPoint(&pNewRepPoint,vMidPos,1,dwSetID);
		pSet->InsertTrafficPoint(&pNewPoint,nTempID,vMidPos,pNewRepPoint);
		FillTree();
	}
Exit0:
	return;
}
