#include "stdafx.h"
#include "SO3World.h"
#include "KSO3World.h"
#include "SO3World.h"
#include "IKGSO3WorldClient.h"
#include "SO3RepresentInterface.h"
#include "SO3UI/ISO3UI.h"
#include "IEOther.h"
#include "IEEditor.h"
#include "AtlBase.h"
#include "ProgressForm.h"
#include "TopViewForm.h"
#include "Stolen/KG3DSceneLogicalEditorBase.h"
#include "Stolen/KSceneEditorDocLogical.h"
#include "KG3DRTTIType.h"
#include "LoadLogicalObj.h"
#include "Stolen/KSceneHelperFuncs.h"


extern IEKG3DEngineManager* g_pEngineManager;
// extern IEKG3DGraphicsTool* g_pGraphicsTool;
// extern LPDIRECT3DDEVICE9 g_pd3dDevice;
// extern IKG3DRTTITypeManager* g_pKG3DRTTITypeManager; ///<RTTI_INTERFACE>
extern IKSO3RepresentHandler* g_pRepresentHandler;
extern IEKG3DSceneSceneEditor* g_pSceneEditor;
extern KSceneEditorDocLogical* g_pDocLogical;

void LoadRepresentNPC()
{
	INT nRetCode = false;
	HRESULT hrRetCode = NULL;
	DWORD dwErrorCount = 0;
	list<KSceneEditorDocLogical::NPCInfo*>::iterator i;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	if (!bUseSO3Logical)
	{
		return;
	}

	int nTotalCount = (int)g_pDocLogical->m_listNPCInfo.size();
	int nCount = 0;
	i = g_pDocLogical->m_listNPCInfo.begin();
	while (i!=g_pDocLogical->m_listNPCInfo.end())
	{
		KSceneEditorDocLogical::NPCInfo* pNpc = *i;

		nCount++;
		ShowProgress("Input NPC",nCount * 1.0F / nTotalCount);

		if(!pNpc->pPointer)
		{
			const KNpcTemplate* pcNpcTempate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pNpc->nTempleteID);

			if (pcNpcTempate && pcNpcTempate->dwTemplateID != 0)
			{
				//const KAIParamTemplate* pcAITempate = g_pSO3World->m_Settings.m_AIParamTemplateList.GetTemplate(pcNpcTempate->nAIParamTemplateID);

				DWORD dwRepresentID = pNpc->dwRepresentID;//NpcTempate.dwRepresentID[0];


				// 因为一个物件对应多个表现ID是后来加的新功能
				// 所以在旧的编辑器保存逻辑信息的ini里没有表现
				// id这个项,这个时候它的值是0,使用第一个表现id
				if (dwRepresentID == 0)
				{
					dwRepresentID = pcNpcTempate->dwRepresentID[0];

					//  修复
					pNpc->dwRepresentID = pcNpcTempate->dwRepresentID[0];
				}

				if(bUseSO3Logical)
				{
					KRepresentNPCEquipResource NPCRes;
					KRepresentNPCAnimationResource AniRes;
					BOOL bNpcExist = TRUE;
					IEKG3DRepresentObject* pObj = NULL;
					TCHAR szMDLName[MAX_PATH];
					TCHAR szAniName[MAX_PATH];
					TCHAR szFaceName[MAX_PATH];
					TCHAR szFaceMtlName[MAX_PATH];
					float fAlertRange = 100;
					float fAttackRange = 100;

					hrRetCode = g_pRepresentHandler->GetNPCEquipResource(dwRepresentID,&NPCRes);
					if(FAILED(hrRetCode))
					{
						KGLogPrintf(KGLOG_ERR, "g_pRepresentHandler->GetNPCEquipResource Failed!\n");
						bNpcExist = FALSE;
					}
					hrRetCode = g_pRepresentHandler->GetNPCAnimationResource(dwRepresentID,24,&AniRes);
					if(FAILED(hrRetCode))
					{
						KGLogPrintf(KGLOG_ERR, "g_pRepresentHandler->GetNPCAnimationResource() Failed!\n");
						bNpcExist = FALSE;
					}

					if(bNpcExist)
					{
						LPCTSTR pMDLName = NPCRes.szMDLFilePath;
						LPCTSTR pAniName = AniRes.szAniFilePath;

						LPCTSTR pFaceMesh = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMeshFilePath;
						LPCTSTR pFaceMtl = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMtlFilePath;

						//aAnimation[MODEL_DOODAD_ANI_BEGIN][0].szMeshFilePath;
						//LPCTSTR pMtlName = NPCRes.aAnimation[MODEL_DOODAD_ANI_BEGIN][0].szMtlFilePath;
						//LPCTSTR pAniName = NPCRes.aAnimation[MODEL_DOODAD_ANI_BEGIN][0].szAniFilePath;
						fAlertRange = (float)(0) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
						fAttackRange = (float)(0) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;

						wsprintf(szMDLName,"%s",pMDLName);
						wsprintf(szAniName,"%s",pAniName);
						wsprintf(szFaceName,"%s",pFaceMesh);
						wsprintf(szFaceMtlName,"%s",pFaceMtl);
					}
					else
					{
						wsprintf(szMDLName,"data\\public\\scenerunner\\scenerunner.Mesh");
						wsprintf(szAniName,"data\\public\\scenerunner\\scenerunner.Ani");
						wsprintf(szFaceName,"");
						wsprintf(szFaceMtlName,"");
					}

					//新的载入不需要用放置工具倒腾
					{
						pObj = BuildRepresentNPC(g_pEngineManager, pNpc->nTempleteID
							, pcNpcTempate->szName, szMDLName, szAniName, szFaceName, szFaceMtlName
							, pcNpcTempate->nLevel, pcNpcTempate->nKind, /*pcNpcTempate->nAIType, */
							NPCRes.fModelScale, pcNpcTempate->dwRepresentID[0],fAlertRange,fAttackRange,pNpc->dwNpcReliveID,pNpc->dwNpcRandomID,
							pNpc->dwNpcAISet
							);
						if (pObj)
						{
							pObj->SetTranslation(&pNpc->vPosition);
							pObj->SetRotation(&pNpc->vRotation);
							ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_TO_PHYSICS;
							hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, uAddOption, NULL);
							//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, TRUE);//生命周期现在由Scene接管了
							if (FAILED(hrRetCode))
							{
								if(NULL != pcNpcTempate->szName)
								{
									KGLogPrintf(KGLOG_ERR, "NPC添加到场景失败:%s", pcNpcTempate->szName);
								}
								SAFE_RELEASE(pObj);
							}
							else
							{
								pNpc->pPointer = pObj;
							}

						}
					}
				}
			}	
			else//如查在NPC列表中找不到对应的NPC，则加载一个默认模型（scenerunner.Mesh），使得在辑器中能选中该NPC在场景中删掉。add by huangjinshou 2008-8-1
			{
				IEKG3DRepresentObject* pObj = NULL;
				TCHAR szMDLName[MAX_PATH];
				TCHAR szAniName[MAX_PATH];
				TCHAR szFaceName[MAX_PATH];
				TCHAR szFaceMtlName[MAX_PATH];
				float fAlertRange = 100;
				float fAttackRange = 100;
				wsprintf(szMDLName,"data\\public\\scenerunner\\scenerunner.Mesh");
				wsprintf(szAniName,"");//data\\public\\scenerunner\\scenerunner.Ani
				wsprintf(szFaceName,"");
				wsprintf(szFaceMtlName,"");
				{
					dwErrorCount++;
					pObj = BuildRepresentNPC(g_pEngineManager, pNpc->nTempleteID
						, pNpc->szName.c_str(), szMDLName, szAniName, szFaceName, szFaceMtlName
						, pNpc->nLevel, pNpc->nKind, /*pNpc->nAIType, */
						1.0f, pNpc->dwRepresentID,fAlertRange,fAttackRange,pNpc->dwNpcReliveID,pNpc->dwNpcRandomID,pNpc->dwNpcAISet
						);
					if (pObj)
					{
						D3DXVECTOR3 vPos(0,0,0);
						//D3DXQUATERNION vRot;
						//D3DXQuaternionIdentity(&vRot);
						pObj->SetTranslation(&vPos);
						//pObj->SetRotation(&vRot);

						//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, TRUE);//生命周期现在由Scene接管了
						ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_TO_PHYSICS;
						hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, uAddOption, NULL);
						//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, TRUE);//生命周期现在由Scene接管了
						if (FAILED(hrRetCode))
						{
							SAFE_RELEASE(pObj);
						}
						else
						{
							pNpc->pPointer = pObj;
						}

					}
				}
			}
		}
		i++;
	}
Exit0:
	if(dwErrorCount>0)
	{
		TCHAR strError[MAX_PATH];
		wsprintf(strError,"在NPC列表中找不到对应的NPC,有%i个!\n全部被放到地图的（0，0，0）点，请手动删除冇用的NPC",dwErrorCount);
		MessageBox(NULL,strError,"提示",MB_ICONWARNING);
	}
	return;
}

void LoadRepresentDoodad()
{
	INT nRetCode = false;
	HRESULT hrRetCode = NULL;
	IEKG3DScene* pScene = NULL;
	DWORD dwErrorCount = 0;
	
	list<KSceneEditorDocLogical::DoodadInfo*>::iterator i;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	if (!bUseSO3Logical)
	{
		return;
	}

	KG_PROCESS_ERROR(bUseSO3Logical);
	int nTotalCount = (int)g_pDocLogical->m_listDoodadInfo.size();
	int nCount =0;
	i = g_pDocLogical->m_listDoodadInfo.begin();
	while (i!=g_pDocLogical->m_listDoodadInfo.end())
	{
		KSceneEditorDocLogical::DoodadInfo* pDoodad = *i;

		const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pDoodad->nTempleteID);
		if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0)
		{
			KRepresentDoodadResource DoodadRes;

			nCount++;
			ShowProgress("Input Doodad",nCount * 1.0F / nTotalCount);

			g_pRepresentHandler->GetDoodadResource(pcDoodadTemplate->m_dwRepresentID,&DoodadRes);

			LPCTSTR pMeshName = DoodadRes.aAnimation[0].szMeshFilePath;
			LPCTSTR pMtlName = DoodadRes.aAnimation[0].szMtlFilePath;
			LPCTSTR pAniName = DoodadRes.aAnimation[0].szAniFilePath;

			TCHAR szMeshName[MAX_PATH];
			TCHAR szMtlName[MAX_PATH];
			TCHAR szAniName[MAX_PATH];
			wsprintf(szMeshName,"%s",pMeshName);
			wsprintf(szMtlName,"%s",pMtlName);
			wsprintf(szAniName,"%s",pAniName);
			//新的方式，直接Add，不需要用放置工具倒腾
			{
				IEKG3DRepresentObject* pObj = BuildRepresentDoodad(g_pEngineManager
					, pDoodad->nTempleteID, pcDoodadTemplate->m_szName
					, szMeshName, szMtlName, szAniName,pDoodad->dwDoodadReliveID
					, pDoodad->bUseDefaultObstacle, pDoodad->bGenerateObstacle
				);

				if(pObj)
				{
					pObj->SetTranslation(&pDoodad->vPosition);
					pObj->SetRotation(&pDoodad->vRotation);
					hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
					//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, TRUE);
					if (FAILED(hrRetCode))
					{
						SAFE_RELEASE(pObj);
					}
					else
					{
						pDoodad->pPointer = pObj;
					}					
				}

			}
		}
		else//如果在Doodad列表中找不到对应的Doodad，则加载一个默认模型（scenerunner.Mesh），使得在辑器中能选中该Doodad在场景中删掉。add by huangjinshou 2008-8-1
		{
			TCHAR szMeshName[MAX_PATH];
			TCHAR szMtlName[MAX_PATH];
			TCHAR szAniName[MAX_PATH];
			wsprintf(szMeshName,"data\\public\\scenerunner\\scenerunner.Mesh");
			wsprintf(szMtlName,"");
			wsprintf(szAniName,"");//data\\public\\scenerunner\\scenerunner.Ani
			//新的方式，直接Add，不需要用放置工具倒腾
			{
				IEKG3DRepresentObject* pObj = BuildRepresentDoodad(g_pEngineManager
					, pDoodad->nTempleteID, pDoodad->szName.c_str()
					, szMeshName, szMtlName, szAniName,pDoodad->dwDoodadReliveID
					, pDoodad->bUseDefaultObstacle, pDoodad->bGenerateObstacle
					);
				dwErrorCount++;

				if(pObj)
				{
					pObj->SetTranslation(&pDoodad->vPosition);
					pObj->SetRotation(&pDoodad->vRotation);
					//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE, FALSE, TRUE);
					hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
					if (FAILED(hrRetCode))
					{
						SAFE_RELEASE(pObj);
					}
					else
					{
						pDoodad->pPointer = pObj;
					}					
				}

			}
		}
		i++;
	}
Exit0:
	if(dwErrorCount>0)
	{
		TCHAR strError[MAX_PATH];
		wsprintf(strError,"在Doodad列表中找不到对应的Doodad,有%i个!\n全部被放到地图的（0，0，0）点，请手动删除无用的Doodad",dwErrorCount);
		MessageBox(NULL,strError,"提示",MB_ICONWARNING);
	}
	return;
}

void LoadRepresentWayPoint()
{
	HRESULT hrRetCode = NULL;
	IEKG3DScene* pScene = NULL;
	
	map<DWORD, KSceneEditorDocLogical::WayPointSet*>::iterator iter;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	KG_PROCESS_ERROR(bUseSO3Logical);

	int nTotalCount = (int)g_pDocLogical->m_mapWayPointSet.size();
	int nCount =0;

	iter = g_pDocLogical->m_mapWayPointSet.begin();
	while (iter != g_pDocLogical->m_mapWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = iter->second;

		nCount++;
		ShowProgress("Input WayPointSet",nCount * 1.0F / nTotalCount);

		for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];

			DWORD dwSetID = pSet->dwSetID;

			/*
			g_pSceneEditor->SetEditorWayPointInHand(dwSetID,"WayPoint");
			g_pSceneEditor->DirectPutInHandObject(&pPoint->pPointer,pPoint->vPosition,pPoint->vRotation);*/
			IEKG3DRepresentObject* pObj = BuildWayPoint(g_pEngineManager, dwSetID, _T("WayPoint"));
			if (pObj)
			{
				pObj->SetTranslation(&pPoint->vPosition);
				pObj->SetRotation(&pPoint->vRotation);

				hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
				//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE, FALSE, TRUE);
				if (FAILED(hrRetCode))
				{
					SAFE_RELEASE(pObj);
				}
				else
				{
					pPoint->pPointer = pObj;
				}
			}
		}
		iter++;
	}
Exit0:
	return;
}

void LoadRepresentTrafficPoint()
{
	HRESULT hrRetCode = NULL;
	IEKG3DScene* pScene = NULL;
	
	list<KSceneEditorDocLogical::TrafficPointSet*>::iterator i;
	map<DWORD,KSceneEditorDocLogical::TrafficPoint>::iterator iPoint;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	KG_PROCESS_ERROR(bUseSO3Logical);

	KGLOG_PROCESS_ERROR(g_pDocLogical);
	int nTotalCount = (int)g_pDocLogical->m_listTrafficPointSet.size();
	int nCount =0;


	i = g_pDocLogical->m_listTrafficPointSet.begin();
	while (i!=g_pDocLogical->m_listTrafficPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *i;

		nCount++;
		ShowProgress("Input TrafficPointSet",nCount * 1.0F / nTotalCount);

		for (size_t s=0;s<pSet->vecTrafficPoint.size();s++)
		{
			KSceneEditorDocLogical::TrafficLittlePoint* pPoint = pSet->vecTrafficPoint[s];

			/*
			DWORD dwSetID = pSet->m_dwID;
			//DWORD dwType = pPoint->m_dwType;
			g_pSceneEditor->SetEditorTrafficPointInHand(dwSetID,1,"TrafficPoint");
			D3DXQUATERNION vRotation(0,0,0,0);
			g_pSceneEditor->DirectPutInHandObject(&pPoint->pPointer,pPoint->vPostion,vRotation);*/


			IEKG3DRepresentObject* pObj = BuildTrafficPoint(g_pEngineManager, pSet->m_dwID, 1, _T("TrafficPoint"));
			if (pObj)
			{
				pObj->SetTranslation(&pPoint->vPosition);
				//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE, FALSE, TRUE);
				hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
				if (FAILED(hrRetCode))
				{
					SAFE_RELEASE(pObj);
				}
				else
				{
					pPoint->pPointer = pObj;
				}

			}
		}
		i++;
	}
	nTotalCount = (int)g_pDocLogical->m_mapTrafficPoint.size();
	nCount =0;

	iPoint = g_pDocLogical->m_mapTrafficPoint.begin();
	while (iPoint != g_pDocLogical->m_mapTrafficPoint.end())
	{
		//KSceneEditorDocLogical::TrafficPoint* pPoint = i;

		nCount++;
		ShowProgress("Input TrafficPoint",nCount * 1.0F / nTotalCount);
		/*
		g_pSceneEditor->SetEditorTrafficPointInHand(iPoint->first,0,"TrafficPoint");
		D3DXQUATERNION vRotation(0,0,0,0);
		g_pSceneEditor->DirectPutInHandObject(&((iPoint->second).pPointer),(iPoint->second).vPosition,vRotation);*/

		KSceneEditorDocLogical::TrafficPoint& Point = iPoint->second;

		IEKG3DRepresentObject* pObj = BuildTrafficPoint(g_pEngineManager, iPoint->first, 0, _T("TrafficPoint"));

		if (pObj)
		{
			pObj->SetTranslation(&Point.vPosition);
			//hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE, FALSE, TRUE);
			hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
			if (FAILED(hrRetCode))
			{
				SAFE_RELEASE(pObj);
			}
			else
			{
				Point.pPointer = pObj;
			}
		}

		iPoint++;
	}

Exit0:
	return;
}

void LoadRepresentRefreshPoint()
{
	HRESULT hrRetCode = NULL;
	IEKG3DScene* pScene = NULL;
	
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	KG_PROCESS_ERROR(bUseSO3Logical);
	//{
	//	int nTotalCount = (int)m_lpLogicalData->m_listNPCRefreshPointSet.size();
	//	int nCount =0;

	//	list<KSceneEditorDocLogical::NPCRefreshPointSet*>::iterator i = m_lpLogicalData->m_listNPCRefreshPointSet.begin();
	//	while (i!=m_lpLogicalData->m_listNPCRefreshPointSet.end())
	//	{
	//		KSceneEditorDocLogical::NPCRefreshPointSet* pSet = *i;

	//		nCount++;
	//		ShowProgress("Input NPCRefreshPointSet",nCount * 1.0F / nTotalCount);

	//		list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
	//		while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
	//		{
	//			KSceneEditorDocLogical::RefreshPoint* pPoint = *j;

	//			/*
	//			g_pSceneEditor->SetEditorRefreshPointInHand(1,pSet->dwSetID,"Refresh Point");
	//							g_pSceneEditor->DirectPutInHandObject(&pPoint->pPointer,pPoint->vPosition,pPoint->vRotation);*/
	//			
	//			IEKG3DRepresentObject* pObj = BuildRefreshPoint(g_pEngineManager, 1, pSet->dwSetID, _T("Refresh Point"));
	//			if (pObj)
	//			{
	//				pObj->SetTranslation(&pPoint->vPosition);
	//				pObj->SetRotation(&pPoint->vRotation);
	//				hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE);
	//				if(FAILED(hrRetCode))
	//				{
	//					SAFE_RELEASE(pObj);
	//				}
	//				else
	//				{
	//					pPoint->pPointer = pObj;
	//				}
	//			}

	//			j++;
	//		}
	//		i++;
	//	}
	//}
	//{
	//	int nTotalCount = (int)m_lpLogicalData->m_listDoodadRefreshPointSet.size();
	//	int nCount =0;

	//	list<KSceneEditorDocLogical::DoodadRefreshPointSet*>::iterator i = m_lpLogicalData->m_listDoodadRefreshPointSet.begin();
	//	while (i!=m_lpLogicalData->m_listDoodadRefreshPointSet.end())
	//	{
	//		KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = *i;

	//		nCount++;
	//		ShowProgress("Input DoodadRefreshPointSet",nCount * 1.0F / nTotalCount);

	//		list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
	//		while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
	//		{
	//			KSceneEditorDocLogical::RefreshPoint* pPoint = *j;

	//			/*
	//			g_pSceneEditor->SetEditorRefreshPointInHand(0,pSet->dwSetID,"Refresh Point");
	//							g_pSceneEditor->DirectPutInHandObject(&pPoint->pPointer,pPoint->vPosition,pPoint->vRotation);*/
	//			IEKG3DRepresentObject* pObj = BuildRefreshPoint(g_pEngineManager, 0, pSet->dwSetID, _T("Refresh Point"));
	//			if (pObj)
	//			{
	//				pObj->SetTranslation(&pPoint->vPosition);
	//				pObj->SetRotation(&pPoint->vRotation);
	//				hrRetCode = g_pSceneEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE);
	//				if(FAILED(hrRetCode))
	//				{
	//					SAFE_RELEASE(pObj);
	//				}
	//				else
	//				{
	//					pPoint->pPointer = pObj;
	//				}
	//			}

	//			j++;
	//		}
	//		i++;
	//	}
	//}
Exit0:
	return;
}
