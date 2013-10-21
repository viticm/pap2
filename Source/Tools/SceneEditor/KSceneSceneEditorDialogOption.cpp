// KSceneSceneEditorDialogOption.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogOption.h"

#include "KNpcTemplateList.h"
//#include "KG3DRepresentNPC.h"
//#include "kg3dSceneSceneEditor.h"
#include "IEEditor.h"
#include "KSceneEditorDocLogical.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "KSO3World.h"
#include "KPatrolPath.h"
#include "KScene.h"
#include "SceneEditorDoc.h"
#include "KSceneSceneHelpers.h"
#include "KGHelperClasses.h"
#include "IEKG3DTerrain.h"
#include "IEKG3DSceneCheckers.h"
//#include "Global.h"
//#include "..\Source\Common\SO3World\src\KNpc.h"
// KSceneSceneEditorDialogOption dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogOption, CDialog)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KSceneSceneEditorDialogOption::KSceneSceneEditorDialogOption(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogOption::IDD, pParent)
	, m_bShowNpc(FALSE)
	, m_bShowDoodad(FALSE)
	, m_bShowWayPoint(FALSE)
	, m_bShowRefreshPoint(FALSE)
	, m_bFreezeNpc(FALSE)
	, m_bFreezeDoodad(FALSE)
	, m_bFreezeWayPoint(FALSE)
	, m_bFreezeRefreshPoint(FALSE)
	, m_nSceneID(0)
	, m_bShort(FALSE)
{
	m_lpSceneSceneEditor = NULL;
	m_lpDocLogical = NULL;
	m_lpDoc = NULL;
	m_fLODDistance = 5000.0f;
}

KSceneSceneEditorDialogOption::~KSceneSceneEditorDialogOption()
{
	//<显示选项>
	KG_CUSTOM_HELPERS::TContainerDelete(m_vecButtons);
	//</显示选项>
}


void KSceneSceneEditorDialogOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SHOWNPC, m_bShowNpc);
	DDX_Check(pDX, IDC_CHECK_SHOWDOODAD, m_bShowDoodad);
	DDX_Check(pDX, IDC_CHECK_SHOWWAYPOINT, m_bShowWayPoint);
	DDX_Check(pDX, IDC_CHECK_SHOWREFRESHPOINT, m_bShowRefreshPoint);
	DDX_Check(pDX, IDC_CHECK_FREEZENPC, m_bFreezeNpc);
	DDX_Check(pDX, IDC_CHECK_FREEZEDOODAD, m_bFreezeDoodad);
	DDX_Check(pDX, IDC_CHECK_FREEZEWAYPOINT, m_bFreezeWayPoint);
	DDX_Check(pDX, IDC_CHECK_FREEZEREFRESHPOINT, m_bFreezeRefreshPoint);
	DDX_Text(pDX,IDC_EDIT_LOD_DISTANCE,m_fLODDistance);
	//DDX_Text(pDX, IDC_EDIT_SCENEID, m_nSceneID);
	//DDV_MinMaxInt(pDX, m_nSceneID, 0, 9999);
	DDX_Check(pDX, IDC_CHECK_ZFAR, m_bShort);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogOption, CDialog)
	ON_BN_CLICKED(IDC_CHECK_SHOWNPC, &KSceneSceneEditorDialogOption::OnBnClickedCheckShownpc)
	ON_BN_CLICKED(IDC_CHECK_SHOWDOODAD, &KSceneSceneEditorDialogOption::OnBnClickedCheckShowdoodad)
	ON_BN_CLICKED(IDC_CHECK_SHOWWAYPOINT, &KSceneSceneEditorDialogOption::OnBnClickedCheckShowwaypoint)
	ON_BN_CLICKED(IDC_CHECK_SHOWREFRESHPOINT, &KSceneSceneEditorDialogOption::OnBnClickedCheckShowrefreshpoint)
	ON_BN_CLICKED(IDC_CHECK_FREEZENPC, &KSceneSceneEditorDialogOption::OnBnClickedCheckFreezenpc)
	ON_BN_CLICKED(IDC_CHECK_FREEZEDOODAD, &KSceneSceneEditorDialogOption::OnBnClickedCheckFreezedoodad)
	ON_BN_CLICKED(IDC_CHECK_FREEZEWAYPOINT, &KSceneSceneEditorDialogOption::OnBnClickedCheckFreezewaypoint)
	ON_BN_CLICKED(IDC_CHECK_FREEZEREFRESHPOINT, &KSceneSceneEditorDialogOption::OnBnClickedCheckFreezerefreshpoint)
	/*ON_BN_CLICKED(IDC_BUTTON_LOAD, &KSceneSceneEditorDialogOption::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &KSceneSceneEditorDialogOption::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_EXCHANGE, &KSceneSceneEditorDialogOption::OnBnClickedButtonExchange)
	ON_EN_CHANGE(IDC_EDIT_SCENEID, &KSceneSceneEditorDialogOption::OnEnChangeEditSceneid)
	ON_BN_CLICKED(IDC_BUTTON_CELL, &KSceneSceneEditorDialogOption::OnBnClickedButtonCell)*/
	ON_BN_CLICKED(IDC_CHECK_SHOWVISIABLERANGE, &KSceneSceneEditorDialogOption::OnBnClickedCheckShowvisiablerange)
	ON_BN_CLICKED(IDC_CHECK_SHOWATTACKRANGE, &KSceneSceneEditorDialogOption::OnBnClickedCheckShowattackrange)
	ON_EN_CHANGE(IDC_EDIT_LOD_DISTANCE, &KSceneSceneEditorDialogOption::OnEnChangeEditLodDistance)
	ON_BN_CLICKED(IDC_CHECK_ZFAR, &KSceneSceneEditorDialogOption::OnBnClickedCheckZfar)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSceneEditorDialogOption::OnBnClickedButtonSetRenderAll)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON2, &KSceneSceneEditorDialogOption::OnBnClickedApply)
	ON_BN_CLICKED(IDC_CHECK1, &KSceneSceneEditorDialogOption::OnBnEnableNormalCheck)
    ON_BN_CLICKED(IDC_CHECK_POINTLIGHT, &KSceneSceneEditorDialogOption::OnBnClickedCheckPointlight)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogOption message handlers

BOOL KSceneSceneEditorDialogOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_bShowNpc = TRUE;
	m_bShowDoodad = TRUE;
	m_bShowWayPoint = TRUE;
	m_bShowRefreshPoint = TRUE;
	((CButton*)(GetDlgItem(IDC_CHECK_SHOWVISIABLERANGE)))->SetCheck(FALSE);
	((CButton*)(GetDlgItem(IDC_CHECK_SHOWATTACKRANGE)))->SetCheck(FALSE);
	m_nSceneID = m_lpDocLogical->m_nSceneID;
	UpdateData(FALSE);

	ShowRenderOptionButtons();

    CheckDlgButton(IDC_CHECK_POINTLIGHT, g_pEngineManager->GetRenderOption(RENDER_POINT_SIGN));


	{
		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckShownpc()
{
	UpdateData(TRUE);

	if(m_bShowNpc)
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		while (i!=m_lpDocLogical->m_listNPCInfo.end())
		{ 
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;
			//int paramID = pNpc->nTempleteID; 
			//KAIParamTemplate* AIParam;
			//AIParam = g_pSO3World->m_Settings.m_AIParamTemplateList.GetTemplate(paramID);
			//int VisableRange = AIParam->nAlertRange;
			IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pNpc->pPointer);
			if(pObject)
			{
				m_lpSceneSceneEditor->AddRepresentObject(pObject, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
			}

			i++;
		}
	}
	else
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		while (i!=m_lpDocLogical->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;
			IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pNpc->pPointer);
			if(pObject)
			{
				pObject->AddRef();
				m_lpSceneSceneEditor->RemoveRepresentObject(pObject);
			}

			i++;
		}
	}
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckShowdoodad()
{
	UpdateData(TRUE);

	if(m_bShowDoodad)
	{
		list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_lpDocLogical->m_listDoodadInfo.begin();
		while (i!=m_lpDocLogical->m_listDoodadInfo.end())
		{
			KSceneEditorDocLogical::DoodadInfo* pNpc = *i;

			IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pNpc->pPointer);
			if(pObject)
			{
				m_lpSceneSceneEditor->AddRepresentObject(pObject, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
			}
			i++;
		}
	}
	else
	{
		list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_lpDocLogical->m_listDoodadInfo.begin();
		while (i!=m_lpDocLogical->m_listDoodadInfo.end())
		{
			KSceneEditorDocLogical::DoodadInfo* pNpc = *i;

			IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pNpc->pPointer);
			if(pObject)
			{
				pObject->AddRef();
				m_lpSceneSceneEditor->RemoveRepresentObject(pObject);
			}

			i++;
		}
	}
}


void KSceneSceneEditorDialogOption::OnBnClickedCheckShowwaypoint()
{
	UpdateData(TRUE);

	if(m_bShowWayPoint)
	{
		map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
		while (i!=m_lpDocLogical->m_mapWayPointSet.end())
		{
			KSceneEditorDocLogical::WayPointSet* pSet = i->second;

			for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
			{
				KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];
				IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pPoint->pPointer);
				if(pObject)
				{
					m_lpSceneSceneEditor->AddRepresentObject(pObject, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
				}
			}
			i++;
		}
	}
	else
	{
		map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
		while (i!=m_lpDocLogical->m_mapWayPointSet.end())
		{
			KSceneEditorDocLogical::WayPointSet* pSet = i->second;

			for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
			{
				KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];
				IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pPoint->pPointer);
				if(pObject)
				{
					pObject->AddRef();
					m_lpSceneSceneEditor->RemoveRepresentObject(pObject);
				}
			}
			i++;
		}
		m_lpSceneSceneEditor->ClearAllHelpLine();
	}
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckShowrefreshpoint()
{
	UpdateData(TRUE);

	/*if(m_bShowRefreshPoint)
	{
		{
			list<KSceneEditorDocLogical::NPCRefreshPointSet*>::iterator i = m_lpDocLogical->m_listNPCRefreshPointSet.begin();
			while (i!=m_lpDocLogical->m_listNPCRefreshPointSet.end())
			{
				KSceneEditorDocLogical::NPCRefreshPointSet* pSet = *i;

				list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
				while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
				{
					KSceneEditorDocLogical::RefreshPoint* pPoint = *j;
					m_lpSceneSceneEditor->AddRenderEntity(SCENEENTITY_OBJECT,pPoint->pPointer);
					j++;
				}
				i++;
			}
		}
		{
			list<KSceneEditorDocLogical::DoodadRefreshPointSet*>::iterator i = m_lpDocLogical->m_listDoodadRefreshPointSet.begin();
			while (i!=m_lpDocLogical->m_listDoodadRefreshPointSet.end())
			{
				KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = *i;

				list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
				while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
				{
					KSceneEditorDocLogical::RefreshPoint* pPoint = *j;
					m_lpSceneSceneEditor->AddRenderEntity(SCENEENTITY_OBJECT,pPoint->pPointer);
					j++;
				}
				i++;
			}
		}
	}
	else
	{
		{
			list<KSceneEditorDocLogical::NPCRefreshPointSet*>::iterator i = m_lpDocLogical->m_listNPCRefreshPointSet.begin();
			while (i!=m_lpDocLogical->m_listNPCRefreshPointSet.end())
			{
				KSceneEditorDocLogical::NPCRefreshPointSet* pSet = *i;

				list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
				while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
				{
					KSceneEditorDocLogical::RefreshPoint* pPoint = *j;
					m_lpSceneSceneEditor->RemoveRenderEntity(SCENEENTITY_OBJECT,pPoint->pPointer);
					j++;
				}
				i++;
			}
		}
		{
			list<KSceneEditorDocLogical::DoodadRefreshPointSet*>::iterator i = m_lpDocLogical->m_listDoodadRefreshPointSet.begin();
			while (i!=m_lpDocLogical->m_listDoodadRefreshPointSet.end())
			{
				KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = *i;

				list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
				while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
				{
					KSceneEditorDocLogical::RefreshPoint* pPoint = *j;
					m_lpSceneSceneEditor->RemoveRenderEntity(SCENEENTITY_OBJECT,pPoint->pPointer);
					j++;
				}
				i++;
			}
		}
	}*/
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckFreezenpc()
{
	UpdateData(TRUE);

	IEKG3DSceneEditorBase* pEditorBase = NULL;
	m_lpSceneSceneEditor->GetSceneEditorBase(&pEditorBase);
	if(NULL == pEditorBase)//<Selectable Refactor>
		return;

	if(!m_bFreezeNpc)
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		while (i!=m_lpDocLogical->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

			//m_lpSceneSceneEditor->SetSelectable(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);//<Selectable Refactor>
			pEditorBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pNpc->pPointer, TRUE);

			i++;
		}
	}
	else
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpDocLogical->m_listNPCInfo.begin();
		while (i!=m_lpDocLogical->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

			//m_lpSceneSceneEditor->SetSelectable(SCENEENTITY_OBJECT,pNpc->pPointer, FALSE);//<Selectable Refactor>
			pEditorBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pNpc->pPointer, FALSE);


			i++;
		}
	}
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckFreezedoodad()
{
	UpdateData(TRUE);

	IEKG3DSceneEditorBase* pEditorBase = NULL;
	m_lpSceneSceneEditor->GetSceneEditorBase(&pEditorBase);
	if(NULL == pEditorBase)//<Selectable Refactor>
		return;

	if(!m_bFreezeDoodad)
	{
		list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_lpDocLogical->m_listDoodadInfo.begin();
		while (i!=m_lpDocLogical->m_listDoodadInfo.end())
		{
			KSceneEditorDocLogical::DoodadInfo* pNpc = *i;

			//m_lpSceneSceneEditor->SetSelectable(SCENEENTITY_OBJECT,pNpc->pPointer, TRUE);//<Selectable Refactor>
			pEditorBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pNpc->pPointer, TRUE);

			i++;
		}
	}
	else
	{
		list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_lpDocLogical->m_listDoodadInfo.begin();
		while (i!=m_lpDocLogical->m_listDoodadInfo.end())
		{
			KSceneEditorDocLogical::DoodadInfo* pNpc = *i;

			//m_lpSceneSceneEditor->SetSelectable(SCENEENTITY_OBJECT,pNpc->pPointer, FALSE);//<Selectable Refactor>
			pEditorBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pNpc->pPointer, FALSE);

			i++;
		}
	}
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckFreezewaypoint()
{
	UpdateData(TRUE);

	IEKG3DSceneEditorBase* pEditorBase = NULL;
	m_lpSceneSceneEditor->GetSceneEditorBase(&pEditorBase);
	if(NULL == pEditorBase)//<Selectable Refactor>
		return;


	if(!m_bFreezeWayPoint)
	{
		map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
		while (i!=m_lpDocLogical->m_mapWayPointSet.end())
		{
			KSceneEditorDocLogical::WayPointSet* pSet = i->second;

			for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
			{
				KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];
				//m_lpSceneSceneEditor->SetSelectable(SCENEENTITY_OBJECT,pPoint->pPointer, TRUE);//<Selectable Refactor>
				pEditorBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pPoint->pPointer, TRUE);
			}
			i++;
		}
	}
	else
	{
		map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpDocLogical->m_mapWayPointSet.begin();
		while (i!=m_lpDocLogical->m_mapWayPointSet.end())
		{
			KSceneEditorDocLogical::WayPointSet* pSet = i->second;

			for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
			{
				KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];
				//m_lpSceneSceneEditor->SetSelectable(SCENEENTITY_OBJECT,pPoint->pPointer, FALSE);//<Selectable Refactor>
				pEditorBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pPoint->pPointer, FALSE);
			}
			i++;
		}
	}
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckFreezerefreshpoint()
{
	UpdateData(TRUE);

	
}

//void KSceneSceneEditorDialogOption::OnBnClickedButtonLoad()
//{
//	CFileDialog dlg(true, "LogicalDoc", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网三逻辑数据文件(*.LogicalDoc)|*.LogicalDoc||");
//	if(dlg.DoModal() != IDOK)
//		return;
//	TCHAR FileName[MAX_PATH];
//	wsprintf(FileName,"%s",dlg.GetPathName());
//
//	m_lpDocLogical->DeleteAllDoodad();
//	m_lpDocLogical->DeleteAllDoodadRefreshPointSet();
//	m_lpDocLogical->DeleteAllNPC();
//	m_lpDocLogical->DeleteAllNPCRefreshPointSet();
//	m_lpDocLogical->DeleteAllWayPointSet();
//	m_lpDocLogical->DeleteAllAIGroup();
//
//	m_lpDocLogical->LoadFromIniFile(FileName,&ShowProgress);
//
//	m_lpDoc->AddRepresentNPC();
//	m_lpDoc->AddRepresentDoodad();
//	m_lpDoc->AddRepresentRefreshPoint();
//	m_lpDoc->AddRepresentWayPoint();
//
//	m_nSceneID = m_lpDocLogical->m_nSceneID;
//	UpdateData(FALSE);
//}
//
//void KSceneSceneEditorDialogOption::OnBnClickedButtonSave()
//{
//	CFileDialog dlg(true, "LogicalDoc", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网三逻辑数据文件(*.LogicalDoc)|*.LogicalDoc||");
//	if(dlg.DoModal() != IDOK)
//		return;
//	TCHAR FileName[MAX_PATH];
//	wsprintf(FileName,"%s",dlg.GetPathName());
//
//	m_lpDocLogical->SaveToIniFile(FileName);
//
//}
//
//
//
//void KSceneSceneEditorDialogOption::OnBnClickedButtonExchange()
//{
//	CFileDialog dlg(true, "Map", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网三3D场景文件(*.Map)|*.Map||");
//	if(dlg.DoModal() != IDOK)
//		return;
//	TCHAR FileName[MAX_PATH];
//	wsprintf(FileName,"%s",dlg.GetPathName());
//
//	m_lpDoc->ExchangeLogicalData(FileName);
//}
//
//void KSceneSceneEditorDialogOption::OnEnChangeEditSceneid()
//{
//	UpdateData(TRUE);
//	m_lpDocLogical->m_nSceneID = m_nSceneID;
//}
//
//void KSceneSceneEditorDialogOption::OnBnClickedButtonCell()
//{
//	if(!m_lpDoc)
//		return;
//	m_lpDoc->TransformatCell(TRUE);
//}

void KSceneSceneEditorDialogOption::OnBnClickedCheckShowvisiablerange()
{
	BOOL bcheck = ((CButton*)(GetDlgItem(IDC_CHECK_SHOWVISIABLERANGE)))->GetCheck();
	if(m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor->EnableRenderAlertRange(bcheck);
	}
}

void KSceneSceneEditorDialogOption::OnBnClickedCheckShowattackrange()
{
	BOOL bcheck = ((CButton*)(GetDlgItem(IDC_CHECK_SHOWATTACKRANGE)))->GetCheck();
	if(m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor->EnableRenderAttackRange(bcheck);
	}
}

void KSceneSceneEditorDialogOption::OnEnChangeEditLodDistance()
{
	TCHAR sz[256];
	GetDlgItemText(IDC_EDIT_LOD_DISTANCE, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("请键入一个数值", "警告");
		return;
	}
	UpdateData(TRUE);
	/*if(m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->SetfModelLodLevel(m_fLODDistance);*/

	KG3DEngineOption Option;
	HRESULT hr = g_pEngineManager->GetEngineOption(&Option);
	if (SUCCEEDED(hr))
	{
		Option.fModelLODSwitchGates[0] = m_fLODDistance;
		g_pEngineManager->SetEngineOption(&Option);
	}	
}
void KSceneSceneEditorDialogOption::OnBnClickedCheckZfar()
{
	UpdateData(TRUE);

	if (m_lpSceneSceneEditor)
	{
		if(m_bShort)
			m_lpSceneSceneEditor->SetZFar(5000);
		else
			m_lpSceneSceneEditor->SetZFar(100000);
	}
}

struct TypeRenderOptionDesc 
{
	RENDER_OPTION m_RenderOption;
	LPCTSTR	m_tcsDesc;
};

//修改这个可以自动生成界面
TypeRenderOptionDesc RENDER_OPTION_MAP[] = 
{
	{RENDER_TERRAIN, _T("地形")},
	{RENDER_GRASS, _T("草")},
	{RENDER_ENVIRONMENT, _T("环境效果")},
	{RENDER_WATER, _T("水")},
	{RENDER_ENTITY, _T("Entity")},

	{RENDER_TERR_TOPVIEW, _T("地形TopView")},
	{RENDER_GRASS_TOPVIEW, _T("草TopView")},
	{RENDER_ENVIRONMENT_TOPVIEW, _T("环境效果TV")},
	{RENDER_WATER_TOPVIEW, _T("水TV")},
	{RENDER_ENTITY_TOPVIEW, _T("EntityTV")},
	{RENDER_TERREDGE_TOPVIEW, _T("地形边TV")},
	
	{RENDER_TREE, _T("树")},
	{RENDER_INFO, _T("渲染信息")},
	{RENDER_VIDEO_MEMERY_INFO, _T("Video信息")},

	{RENDER_STONE, _T("石头")},
	{RENDER_FRUTEX, _T("灌木")},
	{RENDER_FPSHISTROY, _T("FPS历史")},


};

enum
{
	max_render_options = IDC_BN_OPTION_BEGIN - IDC_BN_OPTION_END + 1,
};

inline UINT GetID_Of_RenderOption(size_t i){return IDC_BN_OPTION_BEGIN + static_cast<UINT>(i);}
inline size_t GetOrder_Of_RenderOptionButtonID(UINT_PTR ID){return static_cast<size_t>(ID - IDC_BN_OPTION_BEGIN);}

static BOOL stbCreationFailed = FALSE;

BOOL KSceneSceneEditorDialogOption::OnCommand( WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = CDialog::OnCommand(wParam, lParam);
	int nCode = HIWORD(wParam);
	if (nCode == BN_CLICKED )
	{
		UINT uID = LOWORD(wParam);
		if (uID >= IDC_BN_OPTION_BEGIN && uID <= IDC_BN_OPTION_END)
		{
			size_t Order = GetOrder_Of_RenderOptionButtonID(uID);
			CButton* pBn = m_vecButtons[Order];
			_ASSERTE(pBn);
			RENDER_OPTION RenderOption = RENDER_OPTION_MAP[Order].m_RenderOption;
#if defined(DEBUG) | defined(_DEBUG)
			{
				HWND hWnd = (HWND)lParam;
				if (hWnd)
				{
					_ASSERTE(hWnd == pBn->GetSafeHwnd());
				}
			}
#endif
			if(! bRet)
			{
				//不能响应动态创建的按钮的点击的话，只能自己响应了
				pBn->SetCheck(pBn->GetCheck() == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);
			}

			BOOL bChecked = pBn->GetCheck() == BST_CHECKED ? TRUE : FALSE;
			if (NULL != g_pEngineManager)
			{
				bChecked ? g_pEngineManager->SetRenderOption(RenderOption, TRUE) 
					: g_pEngineManager->SetRenderOption(RenderOption, FALSE);
			}
		}
	}
	return bRet;
	
}

VOID KSceneSceneEditorDialogOption::ShowRenderOptionButtons()
{
	_ASSERTE(IDC_BN_OPTION_BEGIN < IDC_BN_OPTION_END && _T("需要依赖这两个ID的大小来保证一系列Button的动态\
创建，如果触发这个断言就要检查ID"));
	_ASSERTE(_countof(RENDER_OPTION_MAP) <= max_render_options);
	
	if (m_vecButtons.empty() && ! stbCreationFailed)
	{
		CWnd* pWnd = this->GetDlgItem(IDC_BUTTON1);
		KG_PROCESS_ERROR(pWnd);
		CRect standardRect;
		pWnd->GetWindowRect(&standardRect);
		this->ScreenToClient(&standardRect);
		for (size_t i = 0; i < _countof(RENDER_OPTION_MAP); ++i)
		{
			CRect PosRect(
				CPoint(standardRect.left + static_cast<ULONG>(i % 2) * standardRect.Width()/2
					, static_cast<ULONG>(i / 2) * standardRect.Height() + standardRect.bottom + 2)
				,CSize(standardRect.Width()/2 - 2
					,standardRect.Height())
				);
			CButton* pBn = new CButton;
			KG_PROCESS_ERROR(pBn);
			BOOL bRet = pBn->Create(RENDER_OPTION_MAP[i].m_tcsDesc, WS_VISIBLE | WS_CHILD | BS_CHECKBOX
				, PosRect, this, GetID_Of_RenderOption(i));
			if(! bRet)
			{
				delete pBn;
				goto Exit0;
			}
			m_vecButtons.push_back(pBn);
		}
	}

	_ASSERTE(m_vecButtons.size() == _countof(RENDER_OPTION_MAP));

	if (NULL != g_pEngineManager)
	{
		for (size_t i = 0; i < _countof(RENDER_OPTION_MAP); ++i)
		{
			RENDER_OPTION uFlag = RENDER_OPTION_MAP[i].m_RenderOption;
			BOOL bRet = g_pEngineManager->GetRenderOption(uFlag);
			
			CButton& CkButton = *m_vecButtons[i];

			CkButton.SetCheck(bRet ? BST_CHECKED : BST_UNCHECKED);
		}
	}
		
	
	return;
Exit0:
	stbCreationFailed = TRUE;
	KG_CUSTOM_HELPERS::TContainerDelete(m_vecButtons);
	return;
}
void KSceneSceneEditorDialogOption::OnBnClickedButtonSetRenderAll()
{
	// TODO: Add your control notification handler code here
	if (NULL != g_pEngineManager)
	{
		g_pEngineManager->SetRenderOption(RENDER_ALL, TRUE);
		ShowRenderOptionButtons();
	}
}

void KSceneSceneEditorDialogOption::OnShowWindow( BOOL bShow, UINT nStatus )
{
	SyncInnerToInterface();

	IEKG3DSceneCheckers* p = KSH::GetEngineInterfaceEx<IEKG3DSceneCheckers>();
	if(NULL != p)
	{
		ULONG IsChecked = p->IsCheckNormalEnable();
		this->CheckDlgButton(IDC_CHECK1, IsChecked ? BST_CHECKED : BST_UNCHECKED);
	}

	return CDialog::OnShowWindow(bShow, nStatus);
}



VOID KSceneSceneEditorDialogOption::SyncInterfaceToInner()
{
	CString st1, st2;
	this->GetDlgItemText(IDC_EDIT1, st1);
	this->GetDlgItemText(IDC_EDIT2, st2);
	FLOAT fRet1 = 0, fRet2 = 0;
	BOOL bRet = KG_CUSTOM_HELPERS::TStringToValue(st1, fRet1)
		&& KG_CUSTOM_HELPERS::TStringToValue(st2, fRet2);
	if (bRet)
	{
		fRet1 /= 180;
		fRet2 /= 180;
		fRet1 *= D3DX_PI;
		fRet2 *= D3DX_PI;
		if (NULL != m_lpSceneSceneEditor)
		{
			m_lpSceneSceneEditor->SetLODProperties(fRet1, fRet2);
		}
	}
}

VOID KSceneSceneEditorDialogOption::SyncInnerToInterface()
{
	FLOAT fLODH2M = 0;
	FLOAT FLODM2L = 0;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR (NULL != m_lpSceneSceneEditor);

	hr = m_lpSceneSceneEditor->GetLODProperties(&fLODH2M, &FLODM2L);
	KG_COM_PROCESS_ERROR(hr);
	{
		CWnd* p = this->GetDlgItem(IDC_EDIT_LOD_DISTANCE);
		if (p)
		{
			CString TempStr;
			KG3DEngineOption Op;
			g_pEngineManager->GetEngineOption(&Op);
			KG_CUSTOM_HELPERS::TValueToString(Op.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE], TempStr);
			p->SetWindowText(TempStr);
		}
	}
	{
		CWnd* p = this->GetDlgItem(IDC_EDIT1);
		if (p)
		{
			std::tstring TempStr; 
			KG_CUSTOM_HELPERS::TValueToString((fLODH2M/D3DX_PI*180.0f), TempStr);
			p->SetWindowText(TempStr.c_str());
		}
	}

	{
		CWnd* p = this->GetDlgItem(IDC_EDIT2);
		if (p)
		{
			std::tstring TempStr;
			KG_CUSTOM_HELPERS::TValueToString((FLODM2L/D3DX_PI*180.0f), TempStr);
			p->SetWindowText(TempStr.c_str());
		}
	}
Exit0:
	return;
}

void KSceneSceneEditorDialogOption::OnBnClickedApply()
{
	SyncInterfaceToInner();
	SyncInnerToInterface();
}

void KSceneSceneEditorDialogOption::OnBnEnableNormalCheck()
{
	IEKG3DSceneCheckers* p = KSH::GetEngineInterfaceEx<IEKG3DSceneCheckers>();
	if(NULL == p)
		return;

	if (BST_CHECKED == this->IsDlgButtonChecked(IDC_CHECK1))
	{
		p->EnableCheckNormal(TRUE);
	}
	else
	{
		p->EnableCheckNormal(FALSE);
	}
	ULONG IsChecked = p->IsCheckNormalEnable();
	this->CheckDlgButton(IDC_CHECK1, IsChecked ? BST_CHECKED : BST_UNCHECKED);
}


void KSceneSceneEditorDialogOption::OnBnClickedCheckPointlight()
{
    g_pEngineManager->SetRenderOption(RENDER_POINT_SIGN, !!IsDlgButtonChecked(IDC_CHECK_POINTLIGHT));
}
