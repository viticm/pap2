////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneObjectPlacementTool.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-1 11:34:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DSceneObjectPlacementTool.h"
//#include "KG3DSceneSceneEditor.h"
#include "KG3DRepresentObject.h"
#include "KG3DRepresentObjectSet.h"
#include "IEKG3DSceneOutputWnd.h"
#include "KG3DIntersection.h"

#include "KG3DRepresentObjectTable.h"
#include "KG3DModelTable.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "PhysiscSystem/KG3DPhysiscScene.h"
#include "KG3DRepresentObjectNode.h"
#include "KG3DTerrainRoad.h"
#include "Terrain/KG3DTerrainRiver.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DRepresentObjectwall.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTerrainRender.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////

KG3DSceneObjectPlacementTool::KG3DSceneObjectPlacementTool(KG3DSceneSceneEditor* pEditor)
:m_pObjInHandle(NULL)
//,m_pObjSetInHandle(NULL)
,m_pSceneEditor(NULL)
,m_dwLastPlaceTime(0)
{
	_ASSERTE(pEditor);
	HRESULT hr = pEditor->RegisterListener(KM_EXECUTE_ACTION, this);
	VERIFY(SUCCEEDED(hr));
	m_pSceneEditor = pEditor;

	pEditor->RegisterListener(KM_STATE_CHANGE, this);
}

KG3DSceneObjectPlacementTool::~KG3DSceneObjectPlacementTool()
{
	RemoveObjectInHandle();
}

KG3DCommonObject* KG3DSceneObjectPlacementTool::Build( LPCTSTR lpFile, DWORD_PTR pScene, DWORD_PTR )
{
	KG3DSceneObjectPlacementTool* p = NULL;
	KG_PROCESS_ERROR(NULL != pScene);
	p = new KG3DSceneObjectPlacementTool(reinterpret_cast<KG3DSceneSceneEditor*>(pScene));
	KG_PROCESS_ERROR(NULL != p);
	{
		HRESULT hr = p->Init();
		KG_COM_PROCESS_ERROR(hr);
	}
	return p;
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}

LPVOID STDMETHODCALLTYPE KG3DSceneObjectPlacementTool::ToAnotherInterface( DWORD_PTR Param)
{
	IEKG3DSceneObjectPlacementTool* pPlacement = this;
	return reinterpret_cast<LPVOID>(pPlacement);
}

ULONG STDMETHODCALLTYPE KG3DSceneObjectPlacementTool::GetType()
{
	return KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL;
}

HRESULT KG3DSceneObjectPlacementTool::SetObjectInHandle( IEKG3DRepresentObject* pObject, BOOL bReleaseWhenAnotherObjectIsSettedToHandle )
{
	KG3DRepresentObject* p = dynamic_cast<KG3DRepresentObject*>(pObject);
	KG_PROCESS_ERROR(NULL != p);
	{
		RemoveObjectInHandle();
		m_pObjInHandle = p;
		m_BitSet.Set<EM_RELEASE_WHEN_SET_HANDLE>(bReleaseWhenAnotherObjectIsSettedToHandle);

		if (m_pSceneEditor)
		{
			m_pSceneEditor->SetEditState(SCENESTATE_PLACEOBJ);
		}
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

//HRESULT KG3DSceneObjectPlacementTool::SetObjectSetInHandle( IEKG3DRepresentObjectSet* pObject, BOOL bReleaseWhenAnotherObjectIsSettedToHandle )
//{
//	KG3DRepresentObjectSet* p = dynamic_cast<KG3DRepresentObjectSet*>(pObject);
//	KG_PROCESS_ERROR(NULL != p);
//	{
//		RemoveObjectInHandle();
//		m_pObjSetInHandle = p;
//		m_BitSet.Set<EM_RELEASE_WHEN_SET_HANDLE>(bReleaseWhenAnotherObjectIsSettedToHandle);
//		if (m_pSceneEditor)
//		{
//			m_pSceneEditor->SetEditState(SCENESTATE_PLACEOBJ);
//		}
//		return S_OK;
//	}
//Exit0:
//	return E_FAIL;
//}

HRESULT KG3DSceneObjectPlacementTool::GetObjectInHandle( IEKG3DRepresentObject** ppObject/*, IEKG3DRepresentObjectSet** ppObjectSet*/)
{
	if (NULL != m_pObjInHandle )
	{
		//_ASSERTE(NULL == m_pObjSetInHandle);
		KG_PROCESS_ERROR(NULL != ppObject);
		*ppObject = m_pObjInHandle;
		return S_OK;
	}
	/*else if (NULL != m_pObjSetInHandle)
	{
		_ASSERTE(NULL == m_pObjInHandle);
		KG_PROCESS_ERROR(NULL != ppObjectSet);
		*ppObjectSet = m_pObjSetInHandle;
		return S_OK;
	}*/
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneObjectPlacementTool::RemoveObjectInHandle()
{
	if (m_BitSet.Is<EM_RELEASE_WHEN_SET_HANDLE>())
	{
		SAFE_RELEASE(m_pObjInHandle);
		//SAFE_RELEASE(m_pObjSetInHandle);
	}
	else
	{
		m_pObjInHandle = NULL;
		//m_pObjSetInHandle = NULL;
	}
	//这里不要对Scene进行SetState，小心死循环
	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DSceneObjectPlacementTool::Render()
{
	if (m_pObjInHandle)
	{
		m_pObjInHandle->RenderHierarchy();
	}
	/*else if(m_pObjSetInHandle)
	{
		m_pObjSetInHandle->Render(RENDER_NONE);
	}*/
	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DSceneObjectPlacementTool::FrameMove()
{
	KG_PROCESS_ERROR(NULL != m_pSceneEditor);
	KG_PROCESS_SUCCESS(m_pSceneEditor->GetEditState() != SCENESTATE_PLACEOBJ);//如果不是这种状态的话，就不用同步HandleObject和鼠标位置
	KG_PROCESS_SUCCESS(NULL == m_pObjInHandle /*&& NULL == m_pObjSetInHandle*/);
	{
		KG3DScene* pScene = m_pSceneEditor;
		KG3DSceneOutputWnd* pWnd = pScene->GetPrimaryWindow();
		KG_PROCESS_ERROR(NULL != pWnd);

		D3DXVECTOR3 vInter(0, 0, 0);
		D3DXVECTOR3 vSrc;
		D3DXVECTOR3 vNormalizedDir;
		pWnd->GetPickRay(&vSrc, &vNormalizedDir, NULL);

        BOOL bFoundInter = FALSE;

		//新的碰撞接口
		if (m_pSceneEditor->IsUseCrossPlayMode())//连对象也检测
		{
			FLOAT fTempRet = FLT_MAX;
			bFoundInter = ::RayIntersection(*m_pSceneEditor, vSrc, vNormalizedDir, &fTempRet, NULL, NULL);
			if (bFoundInter)
			{
				vInter = vSrc + vNormalizedDir * fTempRet;
			}
		}
		else
		{
			FLOAT fTempRet = FLT_MAX;
			bFoundInter = ::RayIntersectionTerrain(*m_pSceneEditor, vSrc, vNormalizedDir, &fTempRet);
			if (bFoundInter)
			{
				vInter = vSrc + vNormalizedDir * fTempRet;
			}
		}

		KG_PROCESS_ERROR(bFoundInter);

		if (m_pObjInHandle)
		{	

			if (m_pObjInHandle->GetType() == REPRESENTOBJECT_TRAFFICPOINT)
			{
				D3DXVECTOR3 vecWaterHeigt;
				m_pSceneEditor->GetWaterHeight(&vecWaterHeigt,&vInter);
				if (vInter.y < vecWaterHeigt.y)
					vInter.y = vecWaterHeigt.y;
			}
			m_pObjInHandle->SetTranslation(&vInter);
		}
	}
	if (m_pObjInHandle)
	{	
		m_pObjInHandle->FrameMove();
	}
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

void __stdcall KG3DSceneObjectPlacementTool::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	KG_PROCESS_ERROR(NULL != m_pSceneEditor);
	switch(Message.m_uMessage)
	{
	/*
		case KM_RENDER:
				this->Render();
				break;
			case KM_FRAMEMOVE:
				this->FrameMove();
				break;*/
		
	case KM_STATE_CHANGE:
		{
			DWORD_PTR NewState = Message.m_dwWParam;
			if (NewState != SCENESTATE_PLACEOBJ)
			{
				RemoveObjectInHandle();//切换State，去掉Object
			}
		}
	    break;
	case KM_EXECUTE_ACTION:
		{
			if (m_pSceneEditor->GetEditState() == SCENESTATE_PLACEOBJ)
			{
				KG3DMessageActionExecutionStruct* p = reinterpret_cast<KG3DMessageActionExecutionStruct*>(Message.m_dwWParam);
				_ASSERTE(NULL != p);
				if(p->nActionID == EXEACTION_LEFE_KEY_DOWN && p->nExecState == 0)
				{
					this->DoPlaceObject();
				}
			}	
		}
		break;
	case KM_MSG_SENDER_UNREGISTERED:
	case KM_MSG_SENDER_DESTRUCTED:
		{
			//这两个消息只能是SceneEditor发过来的
			m_pSceneEditor = NULL;
		}
	    break;
	default:
	    break;
	}
Exit0:
	KG3DMessageMSGListner::OnReceiveMessage(Message, pSender);//不是非得调用这个的
}


HRESULT KG3DSceneObjectPlacementTool::DoPlaceObject()
{
	HRESULT hr = E_FAIL;
	BOOL bToFile = TRUE;
	KG3DRepresentObject *pObject = NULL;
	{
		DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		enum
		{
			EM_MIN_PLACE_OBJ_TIME_GAP = 300,//300毫秒之内不能放Obj 
		};
		KG_PROCESS_ERROR(dwNowTime - m_dwLastPlaceTime > EM_MIN_PLACE_OBJ_TIME_GAP);
		m_dwLastPlaceTime = dwNowTime;
	}
	KG_PROCESS_ERROR(m_pObjInHandle != NULL);
	KG_PROCESS_ERROR(m_pObjInHandle->IsResourceReady() );//不Ready的话，会出错

	//hr = g_cObjectTable.GetCopy(&pObject,m_pObjInHandle);
	//KG_COM_PROCESS_ERROR(hr);

	//pObject = m_pObjInHandle;

	KG_PROCESS_ERROR(m_pSceneEditor);
	hr = g_GetObjectTable().GetCopy(&pObject,m_pObjInHandle);
	KG_COM_PROCESS_ERROR(hr);

	hr = DoPlaceSpecObject(pObject,bToFile);
	if(bToFile)
		SAFE_RELEASE(pObject);
Exit0:
	return hr;
}

HRESULT KG3DSceneObjectPlacementTool::DoPlaceObjectByOther(IEKG3DRepresentObject *pInObject, IEKG3DRepresentObject **pOutObj) //by likan
{
	D3DXMATRIX mat;
	HRESULT hr = E_FAIL;
	BOOL bToFile = FALSE;
	KG3DRepresentObject *pObject = NULL;
	KG_PROCESS_ERROR(m_pObjInHandle != NULL);
	KG_PROCESS_ERROR(m_pObjInHandle->IsResourceReady() );//不Ready的话，会出错

	pObject = m_pObjInHandle;

	KG3DRepresentObject *inObj = (KG3DRepresentObject *)pInObject;

	KG_PROCESS_ERROR(m_pSceneEditor);
	hr = g_GetObjectTable().GetCopy(&pObject,m_pObjInHandle);
	KG_COM_PROCESS_ERROR(hr);
	KG_PROCESS_ERROR(m_pSceneEditor);

    //把参照模型transformation赋值过去
	pObject->m_Translation = inObj->m_Translation;
	pObject->m_Rotation = inObj->m_Rotation;
	pObject->m_Scaling = inObj->m_Scaling;
	pObject->m_BBox.A = inObj->m_BBox.A;
	pObject->m_BBox.B = inObj->m_BBox.B;
	pObject->m_Matrix = inObj->m_Matrix;

	hr=DoPlaceSpecObject(pObject,bToFile);
	_ASSERT(!bToFile);
	pObject->ComputeDynamicBBox();
    *pOutObj = (IEKG3DRepresentObject *)pObject;



	KG_COM_PROCESS_ERROR(hr);
	return hr;

 Exit0:
	  return E_FAIL;
}


HRESULT KG3DSceneObjectPlacementTool::DoPlaceSpecObject(KG3DRepresentObject *pObject,BOOL& bToFile)	// by likan ,把原来的DoPlaceObject方法分成两部分，这部分要复用，改成私有的了
{
	HRESULT hr = E_FAIL;
	BOOL bIsObjectAddedToScene = FALSE;

	KG_PROCESS_ERROR(m_pSceneEditor);
	// 自动绑上在和模型同目录中和它同名字的bind文件　
	for (DWORD n = 0; n < pObject->GetModelCount(); n++)
	{
		KG3DModel* pModel = NULL;
		if (SUCCEEDED(pObject->GetModel(pModel, n,FALSE)))
		{

			TCHAR szFilePath[MAX_PATH];
			sprintf(szFilePath, TEXT("%s"), pModel->m_scName.c_str());
			TCHAR* pStr = strrchr(szFilePath, '.');
			if (pStr)
			{
				KG3DSFXSuede* pSuede = NULL;
				strncpy(pStr, TEXT(".bind\0"), sizeof(TEXT(".bind\0")));
				if(g_IsFileExist(szFilePath))
				{
					if (SUCCEEDED(g_cModelTable.LoadResourceFromFile(szFilePath, 0, MLO_TEXTURE_MULTI, (IKG3DResourceBase**)&pSuede)))
					{
						pSuede->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
						if (SUCCEEDED(pSuede->BindToBone(pModel, "")))
							m_pSceneEditor->GetVecSFXSuede().push_back(pSuede);
						else
							SAFE_RELEASE(pSuede);
					}
				}
			}
		}
	}

	/*BOOL */bToFile = TRUE;
	if(pObject->GetType()==REPRESENTOBJECT_NPC)
	{
		bToFile = FALSE;
	}
	else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
	{
		bToFile = FALSE;
	}
	else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
	{
		bToFile = FALSE;
	}	
	else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
	{
		bToFile = FALSE;
	}
	else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
	{
		bToFile = FALSE;
	}
	else if (pObject->GetType() == REPRESENTOBJECT_NODE)
	{
		bToFile = FALSE;
	}
	else if (pObject->GetType() == REPRESENTOBJECT_WALL)
	{
		bToFile = FALSE;//暂时不要放到文件中
	}
	else if(pObject->GetType() == REPRESENTOBJECT_POLY)
	{
		bToFile = FALSE;
	}
	//hr = m_pSceneEditor->AddRepresentObject(pObject,TRUE, FALSE,bToFile);	//生命周期由Scene管理
	ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE;
	if(bToFile)
		uAddOption |= EM_SCENE_ADD_OPTION_TO_FILE;
    
	hr = m_pSceneEditor->AddRepresentObject(pObject, uAddOption,  NULL);

	KG_COM_PROCESS_ERROR(hr);
	bIsObjectAddedToScene = TRUE;

	//////////////////////////////////////////////////////////////////////////

	/*if(pObject->GetType()==REPRESENTOBJECT_DEFAULT)
	{
		KG3DModel* pModel = NULL;
		if(SUCCEEDED(pObject->GetModel(pModel,0,FALSE)))
		{
			KG3DPhysiscScene* pPhysiscScene = m_pSceneEditor->GetPhysiscScene();
			pModel->SetPhysXScene(pPhysiscScene);
		}
	}*/

	if(pObject->GetType()==REPRESENTOBJECT_NPC)
	{
		m_pSceneEditor->ForceFireEvent2(EVENTEDITORNPCPLACED,0,0,pObject);
	}
	else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
	{
		m_pSceneEditor->ForceFireEvent2(EVENTEDITORDOODADPLACED,0,0,pObject);
	}
	else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
	{
		m_pSceneEditor->ForceFireEvent2(EVENTEDITORWAYPOINTPLACED,0,0,pObject);
	}	
	else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
	{
		m_pSceneEditor->ForceFireEvent2(EVENTEDITORTRAFFICPOINTPLACED,0,0,pObject);
	}
	else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
	{
		m_pSceneEditor->ForceFireEvent2(EVENTEDITORREFRESHPOINTPLACED,0,0,pObject);
	}
	else if(pObject->GetType()==REPRESENTOBJECT_POLY)
	{
		IEKG3DRepresentPoly* pPoly = static_cast<IEKG3DRepresentPoly*>(static_cast<KG3DRepresentPoly*>(pObject));
		m_pSceneEditor->ForceFireEvent2(EVENTEDITORPOLYPLACED,0,0,pPoly);
	}
	if(pObject->GetType() == REPRESENTOBJECT_NODE)
	{

		KG3DRepresentObjectNode* pObjectNode = static_cast<KG3DRepresentObjectNode*>(pObject);
		KGLOG_PROCESS_ERROR(pObjectNode);
		DWORD dwNodeType = pObjectNode->GetNodeType();
		D3DXVECTOR3 vPos;
		pObjectNode->GetTranslation(&vPos);
		switch(dwNodeType)
		{
		case NODETYPE_ROAD:
			{

				KG3DTerrainRoadMgr* pRoadMgr = NULL;
				KG_PROCESS_ERROR(m_pSceneEditor->m_lpSceneOutDoorSpaceMgr);
				pRoadMgr =  m_pSceneEditor->m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr;
				KG_PROCESS_ERROR(pRoadMgr);
				pRoadMgr->AddNodeToCurSerpentine(pObjectNode,vPos);				
				break;
			}
		case NODETYPE_RIVER:
			{
				KG3DTerrainRiverMgr* pRiverMgr = NULL;
				KG_PROCESS_ERROR(m_pSceneEditor->m_lpSceneOutDoorSpaceMgr);
				pRiverMgr =  m_pSceneEditor->m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr;
				KG_PROCESS_ERROR(pRiverMgr);
				hr = pRiverMgr->AddNodeToCurSerpentine(pObjectNode,vPos);
				break;
			}
		case NODETYPE_WALLKEYPOINT:
			{
				KG3DRepresentObjectWall* pWall = NULL;
				int nWall= 0;
				int nKey = 0;
				pObjectNode->GetWall(pWall,nWall,nKey);
				if(pWall)
				{
					pWall->ModifyWallKeyPoint(pWall,nWall,nKey,vPos);
				}

				KG3DRepresentObjectNode* pHandleNode = static_cast<KG3DRepresentObjectNode*>(m_pObjInHandle);
				if(pHandleNode)
				{
					pHandleNode->SetWall(pWall,nWall,nKey+1);
				}
				break;
			}
		}

	}

	//pObject->OnAddToScene(m_pSceneEditor);

	return S_OK;
Exit0:
	if (bIsObjectAddedToScene)
	{
		if(m_pSceneEditor)
			m_pSceneEditor->RemoveRepresentObject(pObject);
	}
	else
	{
		//	SAFE_RELEASE(pObject);
	}

	return E_FAIL;
}


