#include "StdAfx.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DFrustum.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneBlock.h"
#include "KG3DTemplates.h"
#include "KG3DTextureTable.h"
#include "KG3DHeightMap.h"
#include "KG3DTerrainBlockModifier.h"
#include "KG3DTerrainDetailTexture.h"
#include "KG3DTerrainBlockGrass.h"
#include "KG3DRepresentObjectTable.h"

#include "KG3DSceneBlock.h"
#include "KG3DTerrainBlock.h"
#include "KG3DSceneBlockLoader.h"
#include "KG3DMemoryFile.h"

#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DEngineManager.h"
#include "KG3DRepresentObjectSoundBall.h"
#include "KG3DBlockTraverseData.h"

#include "KG3DTerrainRender.h"
#include "KG3DTerrainRoad.h"
#include "Terrain/KG3DTerrainRiver.h"

#include "KG3DRepresentObjectPVS.h"
#include "KG3DScene.h"
#include "KG3DSceneShadowMap.h"
#include "kg3dscenesceneeditor.h"

#include "Terrain/KG3DOutDoorAssistantFuncs.h"	//GetObjectCount

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DLoadPostProcessor g_LoadPostProcessor;

extern int GetSceneRegionIndex(float In);
extern int GetSceneSectionIndex(float In);
extern TKG3DResourcePool<KG3DSceneZone>g_cSceneZonePool;
extern BOOL g_bClient;
extern BOOL g_bDesign;
extern float g_fMaxVisableDistance;
extern BOOL g_bCanCreateTerrainLess128x128;
////////////////////////////////////////////////////////////////////////////////
const DWORD TERRAIN_UNLOAD_FRAME_INTERVAL = 500;

////////////////////////////////////////////////////////////////////////////////
extern TKG3DResourcePool<KG3DEntityObjectShell>g_cObjectShellPool;

////////////////////////////////////////////////////////////////////////////////
KG3DSceneOutDoorSpaceMgr::KG3DSceneOutDoorSpaceMgr(KG3DScene& Scene)
	:m_Scene(Scene)
{
	m_ulRefCount = 0;

	m_lpDataInterface = NULL;

	m_nNumZone_X = 0;
	m_nNumZone_Z = 0;
	m_nZoneStart_X = 0;
	m_nZoneStart_Z = 0;

	m_lpZones = NULL;
	m_lpDetailMtls = new KG3DTerrainDetailMaterialMgr(this);
	m_lpDetailMtls->Init();

	m_lpSceneBlockLoader = new KG3DSceneBlockLoader(*this);

	m_bImportOldScene = FALSE;

//	m_pGroundHeight = NULL;	
	ZeroMemory(m_uSetHandle,sizeof(m_uSetHandle));
	ZeroMemory(m_lpSetPointer,sizeof(m_lpSetPointer));
	m_nTotalLoadStep = 0;
	m_nCurrentLoadStep = 0;

	m_lpRoadMgr = new KG3DTerrainRoadMgr(this);
	m_lpRiverMgr= new KG3DTerrainRiverMgr(this);

	m_lpConverMap = new KG3DTerrainConverMap;
	m_uConvermapHandle = 0;


	m_lpTerrainCopyHeightData = NULL;
	m_lpTerrainCopyBlendData = NULL;
	m_nTerrainCopyWidth = 0;
	m_nTerrainCopyHeight = 0;
	m_lpTerrainCopyBrush = NULL;
	m_fTerrainCopyHeightMax = 0;
	m_fTerrainCopyHeightMin = 0;
	m_fTerrainCopyHeightAve = 0;
	m_fTerrainCopyRotation = 0.25F;
	m_vTerrainCopyScale = D3DXVECTOR3(1,1,1);
	m_ppTerrainCopyTexturePath = NULL; 
	m_nNumTerrainCopyTexture = 0;
	m_pRealTextureIndex = NULL;

	m_lpSceneEditor = NULL;
	//m_uLoadingDataInMutilThread = 0;

	m_dwLastTimeCheckMemoryUse_EntityShell = 0;

	m_bTryPauseMultiThreadLoad = FALSE;
	m_bPauseMultiThreadLoad = FALSE;
	m_bCreateTerrainLess128x128 = FALSE;
	m_nXRegionNum = 0;
	m_nYRegionNum = 0;
}

KG3DSceneOutDoorSpaceMgr::~KG3DSceneOutDoorSpaceMgr(void)
{
	SAFE_DELETE_ARRAY(m_lpTerrainCopyHeightData);
	SAFE_DELETE_ARRAY(m_lpTerrainCopyBlendData);
	SAFE_RELEASE(m_lpTerrainCopyBrush);
	SAFE_DELETE_ARRAY(m_pRealTextureIndex);
	for (int i = 0; i < m_nNumTerrainCopyTexture; i++)
	{
		SAFE_DELETE_ARRAY(m_ppTerrainCopyTexturePath[i]);
	}
	SAFE_DELETE_ARRAY(m_ppTerrainCopyTexturePath);
	CleanUp();
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_lpConverMap)
    {
        hr = m_lpConverMap->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_lpConverMap)
    {
        hr = m_lpConverMap->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSceneOutDoorSpaceMgr::InitOutDoor(int nXStart,int nZStart,int nXCount,int nZCount,LPCTSTR pFileName)
{
	CreateDataInterface(pFileName,TRUE);
	
	return Resize(nXStart,nZStart,nXCount,nZCount);
}

HRESULT KG3DSceneOutDoorSpaceMgr::StopMultiThreadLoad()
{
	if(m_lpSceneBlockLoader)
	{
		m_lpSceneBlockLoader->m_bQuitLoad = TRUE;
		m_lpSceneBlockLoader->CleanUp();
	}

	DWORD bOutOfTime = FALSE;
	DWORD dwStartTime = timeGetTime();
	while (m_ulRefCount>1 && !bOutOfTime)//在此等待多线程加载的结束
	{
		Sleep(10);
		if(timeGetTime() - dwStartTime > 2000)
			bOutOfTime = TRUE;//等待2秒后强制退出
	}
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::ClearSoundBallSet()
{
	set<KG3DRepresentObjectSoundBall*>::iterator i = m_setObjectSoundBall.begin();
	set<KG3DRepresentObjectSoundBall*>::iterator iend = m_setObjectSoundBall.end();
	while (i!=iend)
	{
		KG3DRepresentObjectSoundBall* pSoundBall = *i;
		pSoundBall->FrameMove(m_vCameraView);
		SAFE_RELEASE(pSoundBall);
		i++;
	}
	m_setObjectSoundBall.clear();
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CleanUp()
{
	ClearSoundBallSet();

	m_mapObjectShell.UnInit();
	//m_mapObjectShellMultiThreadLoaded.UnInit();

	m_nNumZone_X = 0;
	m_nNumZone_Z = 0;
	m_nZoneStart_X = 0;
	m_nZoneStart_Z = 0;

	{
		list<KG3DEntityObjectShell*>::iterator i = m_listObjectShellNoHandle.begin();
		list<KG3DEntityObjectShell*>::iterator iend = m_listObjectShellNoHandle.end();
		while (i!=iend)
		{
			KG3DEntityObjectShell* pShell = *i;
			if(pShell->m_lpObject)
			{
				pShell->m_lpObject->LeavePvs();
			}
			g_cObjectShellPool.FreeResource(pShell);
			i++;
		}		
		m_listObjectShellNoHandle.clear();
	}		

	SAFE_DELETE_ARRAY(m_lpZones);
	//////////////////////////////////////////////////////////////////////////
	set<KG3DSceneZone*>::iterator i = m_setZone.begin();
	set<KG3DSceneZone*>::iterator iend = m_setZone.end();
	while (i!=iend)
	{
		KG3DSceneZone* pZone = *i;

		g_cSceneZonePool.FreeResource(pZone);

		i++;
	}
	m_setZone.clear();

	SAFE_DELETE(m_lpRoadMgr);
	SAFE_DELETE(m_lpRiverMgr);

	if(m_lpDetailMtls)
		m_lpDetailMtls->UnInit();
	SAFE_DELETE(m_lpDetailMtls);

	SAFE_DELETE(m_lpSceneBlockLoader);
//	SAFE_DELETE(m_pGroundHeight);
	SAFE_RELEASE(m_lpSetPointer[MAP_GRASS]);
	SAFE_RELEASE(m_lpSetPointer[MAP_STONE]);
	SAFE_RELEASE(m_lpSetPointer[MAP_FRUTEX]);
	DeleteDataInterface();

	SAFE_DELETE(m_lpConverMap);
	m_uConvermapHandle = 0;
	return S_OK;
}
//
//
//void KG3DSceneOutDoorSpaceMgr::InputObjectShell(KG3DSceneEntityContainer* pContainer, KG3DEntityObjectShell* objShell)
//{
//    if (!objShell || !pContainer)
//        return;
//
//    D3DXVECTOR3 BoxPos[2] = {objShell->m_vBBox_A, objShell->m_vBBox_B};
//
//
//    /* first try to input to pvs object */
//
//    for (std::set<KG3DEntityObjectShell*>::iterator i = pContainer->setObjectShell.begin();
//        i != pContainer->setObjectShell.end(); ++i)
//    {
//        KG3DRepresentObject* obj = (*i)->m_lpObject;
//
//        if (!obj)
//            continue;
//
//        if (obj->GetType() == REPRESENTOBJECT_PVS)
//        {
//            KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)obj;
//
//            if (pvs->IsPositionsInSpace(BoxPos, 2))
//            {
//                KG3DSceneEntity e(SCENEENTITY_OBJECT, objShell->m_lpObject);
//                pvs->InputEntity(e, m_pScene);
//                return;
//            }
//        }
//    }
//
//
//    /* can not input to pvs object,
//       so input to container */
//
//    pContainer->setObjectShell.insert(objShell);
//    objShell->m_setRefrenceContainer.insert(pContainer);
//    float x = pContainer->m_BBox.A.x;
//    float z = pContainer->m_BBox.A.z;
//    pContainer->m_BBox.AddPosition(D3DXVECTOR3(x, objShell->m_vBBox_A.y, z));
//    pContainer->m_BBox.AddPosition(D3DXVECTOR3(x, objShell->m_vBBox_B.y, z));
//    pContainer->ComputeBBox(FALSE);
//
//
//    /* if the objshell is a pvs object
//       then link to the outdoor mgr */
//
//    if (objShell->m_lpObject->GetType() == REPRESENTOBJECT_PVS)
//    {
//        KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)objShell->m_lpObject;
//
//        pvs->m_uHandle = m_pScene->GetOneNewSpaceNodeHandle();
//        KG3DSceneSpaceNode::ConnectTwoNodes(this, pvs);
//        m_pScene->RegisterSpaceNode(pvs);
//    }
//
//
//}


HRESULT KG3DSceneOutDoorSpaceMgr::Resize(int nXStart,int nZStart,int nXCount,int nZCount)
{
	SAFE_DELETE_ARRAY(m_lpZones);
	m_lpZones = new KG3DSceneZone*[nXCount * nZCount];
	KGLOG_PROCESS_ERROR(m_lpZones);
	ZeroMemory(m_lpZones,sizeof(KG3DSceneZone*)*nXCount*nZCount);

	m_nNumZone_X = nXCount;
	m_nNumZone_Z = nZCount;
	m_nZoneStart_X = nXStart;
	m_nZoneStart_Z = nZStart;

	{
		set<KG3DSceneZone*>::iterator i = m_setZone.begin();
		set<KG3DSceneZone*>::iterator iend = m_setZone.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;

			InputZoneToArray(pZone);

			i++;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DSceneOutDoorSpaceMgr::InputZoneToArray(KG3DSceneZone* pZone)
{
	KGLOG_PROCESS_ERROR(pZone);
	KGLOG_PROCESS_ERROR(pZone->GetIndexX() >= m_nZoneStart_X);
	KGLOG_PROCESS_ERROR(pZone->GetIndexZ() >= m_nZoneStart_Z);
	KGLOG_PROCESS_ERROR(pZone->GetIndexX() < m_nZoneStart_X + m_nNumZone_X);
	KGLOG_PROCESS_ERROR(pZone->GetIndexZ() < m_nZoneStart_Z + m_nNumZone_Z);

	INT nIndex =  (pZone->GetIndexZ() - m_nZoneStart_Z) * m_nNumZone_X + (pZone->GetIndexX() - m_nZoneStart_X);
	m_lpZones[nIndex] = pZone;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::NewOneZone(KG3DSceneZone** ppZone,int nX,int nZ)
{
	HRESULT hr = S_OK;
	POINT Index = {nX,nZ};

	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_CreateSceneBlock(BLOCK_TYPE_ZONE,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	(*ppZone) = GetZone(nX,nZ);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneOutDoorSpaceMgr::DeleteZone(KG3DSceneZone* pZone)
{
	POINT Index = {pZone->GetIndexX(),pZone->GetIndexZ()};

	KGLOG_PROCESS_ERROR(pZone);
	KGLOG_PROCESS_ERROR(pZone->GetIndexX() >= m_nZoneStart_X);
	KGLOG_PROCESS_ERROR(pZone->GetIndexZ() >= m_nZoneStart_Z);
	KGLOG_PROCESS_ERROR(pZone->GetIndexX() < m_nZoneStart_X + m_nNumZone_X);
	KGLOG_PROCESS_ERROR(pZone->GetIndexZ() < m_nZoneStart_Z + m_nNumZone_Z);

	HRESULT hr = S_OK;

	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_DeleteSceneBlock(BLOCK_TYPE_ZONE,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetVisibleSceneZone(
    vector<KG3DSceneEntityContainer *> *pvecContainer,
    KG3DTerrainRender *pTerrainRender,
    map<int,KG3DTerrainRoad *> *pmapRoad,   // 要绘制的道路列表
    set<KG3DTerrainRiver *> *psetRiver,         // 要绘制的河流列表
    const KG3DFrustum *pFrustum,
    const D3DXVECTOR3 &vPosCamera,
    int nLevelDeep)
{
	KGLOG_PROCESS_ERROR(pvecContainer);
	KGLOG_PROCESS_ERROR(pTerrainRender);
	KGLOG_PROCESS_ERROR(pFrustum);

	int i,X,Z;

	int XStartZoneIndex = 0;
	int XEndZoneIndex = 0;
	int ZStartZoneIndex =0;
	int ZEndZoneIndex = 0;

	KG3DSceneZone* pZone = NULL;

	int nXCameraZone,nZCameraZone;
	nXCameraZone = GetZoneIndex(vPosCamera.x);
	nZCameraZone = GetZoneIndex(vPosCamera.z);

	float AX,AZ,BX,BZ;
	AX = AZ = 1000000;
	BX = BZ = -1000000;
	for (i = 0;i < 8;i++)
	{
		if(i==0)
		{
			AX = BX = pFrustum->GetPoint(i).x;
			AZ = BZ = pFrustum->GetPoint(i).z;
		}
		else
		{
			AX = min(pFrustum->GetPoint(i).x,AX);
			AZ = min(pFrustum->GetPoint(i).z,AZ);
			BX = max(pFrustum->GetPoint(i).x,BX);
			BZ = max(pFrustum->GetPoint(i).z,BZ);
		}
	}

	XStartZoneIndex = max(m_nZoneStart_X , GetZoneIndex(AX));
	ZStartZoneIndex = max(m_nZoneStart_Z , GetZoneIndex(AZ));
	XEndZoneIndex = min((m_nZoneStart_X + m_nNumZone_X - 1) , GetZoneIndex(BX));
	ZEndZoneIndex = min((m_nZoneStart_Z + m_nNumZone_Z - 1) , GetZoneIndex(BZ));


	for (Z = ZStartZoneIndex; Z <= ZEndZoneIndex; Z++)
	{
		for (X = XStartZoneIndex; X <= XEndZoneIndex; X++)
		{
			pZone = GetZone(X,Z);

            if (!pZone || (pZone->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH))
                continue;

            pZone->m_Data.m_eFrustumIntersectType = (KG3DFrustum::INTERSECT_TYPE)pFrustum->Intersect(pZone->GetAABBox());

            if (pZone->m_Data.m_eFrustumIntersectType != KG3DFrustum::OUTSIDE)
            {
                if (pZone->m_dwLastCullCounter != g_dwCullCounter)
                {
                    pvecContainer->push_back(pZone);
                    pZone->m_dwLastCullCounter = g_dwCullCounter;
                }

				if(pZone->HasSectionDetail())
				{
					int xx = pZone->GetIndexX() - nXCameraZone;
					int zz = pZone->GetIndexZ() - nZCameraZone;
					if ((xx*xx + zz*zz <3)&& ( nLevelDeep>0 ))//如果离摄像机近，则判断是否绘制SECTION，否则只绘制ZONE
					{
						pZone->GetVisiableSceneSections(
                            pvecContainer,
                            pTerrainRender,
                            pmapRoad,
                            psetRiver,
                            pFrustum,
                            vPosCamera,
                            nLevelDeep
                        );
					}	
				}
				
				if(pZone->m_lpTerrainBlock)
				{
					pZone->m_lpTerrainBlock->GetVisibleRoad(pmapRoad,pFrustum);
					pZone->m_lpTerrainBlock->GetVisibleRiver(psetRiver,pFrustum);
				}
			}				
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


/*
int KG3DSceneOutDoorSpaceMgr::GetZoneIndex(float In)
{
	int iIn;
	if(In<0)
	{
		int K;
		iIn = (int)In;
		if(iIn % SCENEZONEWIDTH == 0)
			K = static_cast<int>(iIn / SCENEZONEWIDTH);
		else
			K = static_cast<int>(iIn / SCENEZONEWIDTH - 1);

		return K;
	}
	else
	{
		int K = (int)(In/SCENEZONEWIDTH);
		return K;
	}
}*/


KG3DSceneZone* KG3DSceneOutDoorSpaceMgr::GetZone(int nX,int nZ)
{
	KG_PROCESS_ERROR(m_lpZones);

	KG_PROCESS_ERROR(nX >= m_nZoneStart_X && nZ >= m_nZoneStart_Z 
		&& nX < m_nZoneStart_X + m_nNumZone_X
		&& nZ < m_nZoneStart_Z + m_nNumZone_Z);

	INT nIndex =  (nZ - m_nZoneStart_Z) * m_nNumZone_X + (nX - m_nZoneStart_X);
	return m_lpZones[nIndex];
Exit0:
	return NULL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RenderAllZoneBox()
{
	set<KG3DSceneZone*>::iterator i = m_setZone.begin();
	set<KG3DSceneZone*>::iterator iend = m_setZone.end();
	while (i!=iend)
	{
		KG3DSceneZone* pZone = *i;
		g_cGraphicsTool.DrawAABBox(&pZone->GetAABBox(),0xFFFF0000);
		i++;
	}
	return S_OK;
}

KG3DSceneSection* KG3DSceneOutDoorSpaceMgr::GetSection(int nX,int nZ)
{
	int nX_Zone = nX / SCENESECTIONNUMREGION;
	int nZ_Zone = nZ / SCENESECTIONNUMREGION;

	KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
	KG_PROCESS_ERROR(pZone);

	int nX_Local = nX  - nX_Zone  * SCENESECTIONNUMREGION;
	int nZ_Local = nZ  - nZ_Zone  * SCENESECTIONNUMREGION;

	KG_PROCESS_ERROR( nX_Local>= 0 );
	KG_PROCESS_ERROR( nZ_Local>= 0 );
	KG_PROCESS_ERROR( nX_Local < SCENESECTIONNUMREGION );
	KG_PROCESS_ERROR( nZ_Local < SCENESECTIONNUMREGION );

	int nIndex = nZ_Local * SCENESECTIONNUMREGION + nX_Local;
	return pZone->m_lpSections[nIndex];
Exit0:
	return NULL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::NewOneSection(KG3DSceneSection** ppNewSection,int nSectionX,int nSectionZ)
{
	HRESULT hr = S_OK;
	POINT Index = {nSectionX,nSectionZ};

	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_CreateSceneBlock(BLOCK_TYPE_SECTION,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	(*ppNewSection) = GetSection(nSectionX,nSectionZ);
	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DSceneOutDoorSpaceMgr::DeleteSection(KG3DSceneSection* pSection)
{
	HRESULT hr = S_OK;
	KGLOG_PROCESS_ERROR(pSection);
	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	
	{
		POINT Index = {pSection->m_nIndexX,pSection->m_nIndexZ};

		hr = m_lpDataInterface->Request_DeleteSceneBlock(BLOCK_TYPE_SECTION,Index);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return S_OK;
Exit0:
	return E_FAIL;

}


KG3DSceneRegion* KG3DSceneOutDoorSpaceMgr::GetRegion(int nX,int nZ)
{
	int nX_Section = nX / SCENESECTIONNUMREGION;
	int nZ_Section = nZ / SCENESECTIONNUMREGION;

	KG3DSceneSection* pSection = GetSection( nX_Section ,nZ_Section );
	KG_PROCESS_ERROR(pSection);

	int nX_Local = nX  - nX_Section  * SCENESECTIONNUMREGION;
	int nZ_Local = nZ  - nZ_Section  * SCENESECTIONNUMREGION;
	KG_PROCESS_ERROR( nX_Local>= 0 );
	KG_PROCESS_ERROR( nZ_Local>= 0 );
	KG_PROCESS_ERROR( nX_Local < SCENESECTIONNUMREGION );
	KG_PROCESS_ERROR( nZ_Local < SCENESECTIONNUMREGION );

	int nIndex = nZ_Local * SCENESECTIONNUMREGION + nX_Local;
	return pSection->m_lpRegions[nIndex];
Exit0:
	return NULL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::NewOneRegion(KG3DSceneRegion** pNewRegion,int nRegionX,int nRegionZ)
{
	HRESULT hr = S_OK;
	POINT Index = {nRegionX,nRegionZ};

	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_CreateSceneBlock(BLOCK_TYPE_REGION,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	(*pNewRegion) = GetRegion(nRegionX,nRegionZ);

	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DSceneOutDoorSpaceMgr::DeleteRegion(KG3DSceneRegion* pRegion)
{
	HRESULT hr = S_OK;
	POINT Index;
	KGLOG_PROCESS_ERROR(pRegion);
	Index.x = pRegion->m_nIndexX;
	Index.y = pRegion->m_nIndexZ;

	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_DeleteSceneBlock(BLOCK_TYPE_REGION,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DSceneOutDoorSpaceMgr::SelectZone(vector<POINT>*pvecZoneIndex,D3DXVECTOR2 A,D3DXVECTOR2 C,BOOL bUnDeleted)//bUnDeleted 选择的区域是否已经存在
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetZoneIndex(A.x);
	ZStartIndex = GetZoneIndex(A.y);
	XEndIndex   = GetZoneIndex(C.x);
	ZEndIndex   = GetZoneIndex(C.y);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			POINT const tempPT = { X, Z };
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);

			if(bUnDeleted)
			{
				BOOL bExist = m_lpDataInterface->GetSceneBlock(BLOCK_TYPE_ZONE,tempPT);
				
				if(bExist)
					pvecZoneIndex->push_back(tempPT);
			}
			else
			{
				pvecZoneIndex->push_back(tempPT);
			}
		}
	}
Exit1:

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SelectZone(vector<KG3DSceneZone*>*pvecZone,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int XStartZoneIndex,ZStartZoneIndex,XEndZoneIndex,ZEndZoneIndex;

	XStartZoneIndex = max(m_nZoneStart_X , GetZoneIndex(A.x));
	ZStartZoneIndex = max(m_nZoneStart_Z , GetZoneIndex(A.y));
	XEndZoneIndex = min((m_nZoneStart_X + m_nNumZone_X) , GetZoneIndex(C.x));
	ZEndZoneIndex = min((m_nZoneStart_Z + m_nNumZone_Z) , GetZoneIndex(C.y));

	for (int Z = ZStartZoneIndex; Z <= ZEndZoneIndex; Z++)
	{
		for (int X = XStartZoneIndex; X <= XEndZoneIndex; X++)
		{
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
			KG3DSceneZone* pZone = GetZone(X,Z);
			if (pZone)
			{
				pvecZone->push_back(pZone);		
			}
		}
	}
Exit1:

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SelectSection(vector<POINT>*pvecSectionIndex,D3DXVECTOR2 A,D3DXVECTOR2 C,BOOL bUnDeleted)
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetSceneSectionIndex(A.x);
	ZStartIndex = GetSceneSectionIndex(A.y);
	XEndIndex   = GetSceneSectionIndex(C.x);
	ZEndIndex   = GetSceneSectionIndex(C.y);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			POINT const tempPT = { X, Z };
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);

			if(bUnDeleted)
			{
				BOOL bExist = m_lpDataInterface->GetSceneBlock(BLOCK_TYPE_SECTION,tempPT);

				if(bExist)
					pvecSectionIndex->push_back(tempPT);		
			}
			else
			{
				pvecSectionIndex->push_back(tempPT);		
			}
		}
	}
Exit1:

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SelectSection(vector<KG3DSceneSection*>*pvecSection,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetSceneSectionIndex(A.x);
	ZStartIndex = GetSceneSectionIndex(A.y);
	XEndIndex   = GetSceneSectionIndex(C.x);
	ZEndIndex   = GetSceneSectionIndex(C.y);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
			KG3DSceneSection* pSection = GetSection(X,Z);
			if (pSection)
			{
				pvecSection->push_back(pSection);		
			}
		}
	}
Exit1:

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SelectRegion(vector<POINT>*pvecRegionIndex,D3DXVECTOR2 A,D3DXVECTOR2 C,BOOL bUnDeleted)
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetSceneRegionIndex(A.x);
	ZStartIndex = GetSceneRegionIndex(A.y);
	XEndIndex   = GetSceneRegionIndex(C.x);
	ZEndIndex   = GetSceneRegionIndex(C.y);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			POINT const tempPT = { X, Z };
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);

			if(bUnDeleted)
			{
				BOOL bExist = m_lpDataInterface->GetSceneBlock(BLOCK_TYPE_REGION,tempPT);

				if(bExist)
					pvecRegionIndex->push_back(tempPT);		
			}
			else
			{
				pvecRegionIndex->push_back(tempPT);		
			}
		}
	}
Exit1:

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SelectRegion(vector<KG3DSceneRegion*>*pvecRegion,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetSceneRegionIndex(A.x);
	ZStartIndex = GetSceneRegionIndex(A.y);
	XEndIndex   = GetSceneRegionIndex(C.x);
	ZEndIndex   = GetSceneRegionIndex(C.y);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);

			KG3DSceneRegion* pRegion = GetRegion(X,Z);
			if (pRegion)
			{
				pvecRegion->push_back(pRegion);		
			}
		}
	}
Exit1:
	return S_OK;
}



HRESULT KG3DSceneOutDoorSpaceMgr::SetTerrainConverTexture(LPSTR pFileName,D3DXVECTOR2 A,D3DXVECTOR2 C,BOOL bZone,BOOL bSection,BOOL bRegion)
{
	HRESULT hr = E_FAIL;
	vector<POINT>vecRegionIndex;
	vector<POINT>vecSectionIndex;
	vector<POINT>vecZoneIndex;
	KG3DMemoryFile* pMemFile = g_cMemoryFilePool.RequestResource();
	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	KGLOG_PROCESS_ERROR(pMemFile);

	if(m_lpConverMap)
	{
		m_lpConverMap->m_szTextureFileName = pFileName;
		/*m_lpConverMap->m_vRect.x = A.x;
		m_lpConverMap->m_vRect.y = A.y;
		m_lpConverMap->m_vRect.z = C.x - A.x;
		m_lpConverMap->m_vRect.w = C.y - A.y;*/
		m_lpConverMap->SetRectVector(A.x, A.y, C.x - A.x, C.y - A.y);

		DWORD dwSize = 0;

		m_lpConverMap->SaveToInformationBuffer(dwSize,pMemFile);

		if(m_uConvermapHandle == 0)
		{
			hr = m_lpDataInterface->Request_CreateNewTerrainInfo    (m_uConvermapHandle,Terrain_Info_Convermap,dwSize,pMemFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		else
		{
			hr = m_lpDataInterface->Request_ModifyTerrainInformation(m_uConvermapHandle,Terrain_Info_Convermap,dwSize,pMemFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}

	/*if(bZone)
	{
		SelectZone(&vecZoneIndex,A,C,FALSE);

		hr = g_cTerrainBlockModifier.SetBlockConverTexture(BLOCK_TYPE_ZONE  ,&vecZoneIndex  ,m_uConvermapHandle,A,C,m_lpDataInterface);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	if(bSection)
	{
		SelectRegion(&vecSectionIndex,A,C,FALSE);

		hr = g_cTerrainBlockModifier.SetBlockConverTexture(BLOCK_TYPE_SECTION,&vecSectionIndex,m_uConvermapHandle,A,C,m_lpDataInterface);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	if(bRegion)
	{
		SelectRegion(&vecRegionIndex,A,C,FALSE);

		hr = g_cTerrainBlockModifier.SetBlockConverTexture(BLOCK_TYPE_REGION,&vecRegionIndex,m_uConvermapHandle,A,C,m_lpDataInterface);
		KGLOG_COM_PROCESS_ERROR(hr);
	}*/

	hr = S_OK;
Exit0:
	if(pMemFile)
	{
		g_cMemoryFilePool.FreeResource(pMemFile);
		pMemFile = NULL;
	}
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetZoneHeight(LPSTR pFileName,float fLowest,float fTotal,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	KG3DHeightMap vHeightMap;
	vHeightMap.CreateFromBmpFile(pFileName);
	vHeightMap.SetHeightScale(fLowest,fTotal);

	SetZoneHeight(&vHeightMap,A,C);

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetZoneWaterHeight(LPSTR pFileName,float fLowest,float fTotal,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	KG3DHeightMap vHeightMap;
	vHeightMap.CreateFromBmpFile(pFileName);
	vHeightMap.SetHeightScale(fLowest,fTotal);

	SetZoneWaterHeight(&vHeightMap,A,C);

	return S_OK;
}


HRESULT KG3DSceneOutDoorSpaceMgr::SetSectionHeight(LPSTR pFileName,float fLowest,float fTotal,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	KG3DHeightMap vHeightMap;
	vHeightMap.CreateFromBmpFile(pFileName);
	vHeightMap.SetHeightScale(fLowest,fTotal);

	SetSectionHeight(&vHeightMap,A,C);

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetSectionWaterHeight(LPSTR pFileName,float fLowest,float fTotal,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	KG3DHeightMap vHeightMap;
	vHeightMap.CreateFromBmpFile(pFileName);
	vHeightMap.SetHeightScale(fLowest,fTotal);

	SetSectionWaterHeight(&vHeightMap,A,C);

	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::SetRegionHeight(LPSTR pFileName,float fLowest,float fTotal,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	KG3DHeightMap vHeightMap;
	vHeightMap.CreateFromBmpFile(pFileName);
	vHeightMap.SetHeightScale(fLowest,fTotal);

	SetRegionHeight(&vHeightMap,A,C);

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetZoneHeight(KG3DHeightMap* pHeight,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	vector<POINT>vecZone;
	SelectZone(&vecZone,A,C,TRUE);

	/*vector<KG3DTerrainBlock*>vecBlock;
	for (size_t i=0;i<vecZone.size();i++)
	{
		KG3DSceneZone* pZone = vecZone[i];
		if(pZone->m_lpTerrainBlock)
		{
			vecBlock.push_back(pZone->m_lpTerrainBlock);
		}
	}*/

	g_cTerrainBlockModifier.SetBlockHeight(BLOCK_TYPE_ZONE,&vecZone,pHeight,A,C,this/*m_lpDataInterface*/);

	/*for (size_t i=0;i<vecZone.size();i++)
	{
		KG3DSceneZone* pZone = vecZone[i];
		pZone->ComputeBBox();


		KG3DSceneZone* pNear[4];
		ZeroMemory(pNear,sizeof(KG3DSceneZone*)*4);

		pNear[0] = GetZone(pZone->m_nIndexX-1,pZone->m_nIndexZ  );
		pNear[1] = GetZone(pZone->m_nIndexX+1,pZone->m_nIndexZ  );
		pNear[2] = GetZone(pZone->m_nIndexX  ,pZone->m_nIndexZ+1);
		pNear[3] = GetZone(pZone->m_nIndexX  ,pZone->m_nIndexZ-1);

		pZone->WedTerrainNormal(pNear[0],pNear[1],pNear[2],pNear[3]);
	}*/
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetZoneWaterHeight(KG3DHeightMap* pHeight,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	vector<POINT>vecZone;
	SelectZone(&vecZone,A,C,TRUE);
	g_cTerrainBlockModifier.SetBlockWaterHeight(BLOCK_TYPE_ZONE,&vecZone,pHeight,A,C,m_lpDataInterface);
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetSectionHeight(KG3DHeightMap* pHeight,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	vector<POINT>vecSection;
	SelectSection(&vecSection,A,C,TRUE);
	g_cTerrainBlockModifier.SetBlockHeight(BLOCK_TYPE_SECTION,&vecSection,pHeight,A,C,this/*m_lpDataInterface*/);

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetSectionWaterHeight(KG3DHeightMap* pHeight,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	vector<POINT>vecSection;
	SelectSection(&vecSection,A,C,TRUE);
	
	g_cTerrainBlockModifier.SetBlockWaterHeight(BLOCK_TYPE_SECTION,&vecSection,pHeight,A,C,m_lpDataInterface);

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetRegionHeight(KG3DHeightMap* pHeight,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	vector<POINT>vecRegion;
	SelectRegion(&vecRegion,A,C,TRUE);
	g_cTerrainBlockModifier.SetBlockHeight(BLOCK_TYPE_REGION,&vecRegion,pHeight,A,C,this/*m_lpDataInterface*/);

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetRegionWaterHeight(KG3DHeightMap* pHeight,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	vector<POINT>vecRegion;
	SelectRegion(&vecRegion,A,C,TRUE);

	g_cTerrainBlockModifier.SetBlockWaterHeight(BLOCK_TYPE_REGION,&vecRegion,pHeight,A,C,m_lpDataInterface);

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
float KG3DSceneOutDoorSpaceMgr::GetGroundHeight(float fX,float fZ)
{
	int nX_Zone = GetZoneIndex(fX);
	int nZ_Zone = GetZoneIndex(fZ);
	KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
	if(pZone)
		return pZone->GetGroundHeight(fX,fZ);
	else
		return 0;
}


DWORD KG3DSceneOutDoorSpaceMgr::GetGroundType(const D3DXVECTOR2 &vec2Pos)
{
	DWORD dwReturn = INVALID_GROUNDTYPE;
	int nX_Zone = GetZoneIndex(vec2Pos.x);
	int nZ_Zone = GetZoneIndex(vec2Pos.y);
	std::vector<KG3DSceneSection*> Section;
	std::vector<KG3DSceneRegion*> Regions;
	KG3DSceneZone *pZone = GetZone(nX_Zone, nZ_Zone);
	KG_PROCESS_ERROR(pZone);

	pZone->SelectSection(&Section, vec2Pos, vec2Pos);
	KG_PROCESS_ERROR(Section.size());

	Section[0]->SelectRegion(&Regions, vec2Pos, vec2Pos);
	KG_PROCESS_ERROR(Regions.size());

	dwReturn = Regions[0]->GetTerrainType(vec2Pos);

Exit0:
	return dwReturn;
}

float KG3DSceneOutDoorSpaceMgr::GetGroundHeightEx(float fX,float fZ)
{
	int nX_Zone = GetZoneIndex(fX);
	int nZ_Zone = GetZoneIndex(fZ);
	LoadTerrainBlockByPos(fX,fZ);
	KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
	if(pZone)
		return pZone->GetGroundHeight(fX,fZ);
	else
		return 0;
}

BYTE KG3DSceneOutDoorSpaceMgr::GetDetailTextureIndex(float fX,float fZ)
{
	int nX_Zone = GetZoneIndex(fX);
	int nZ_Zone = GetZoneIndex(fZ);
	KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
	if(pZone)
		return pZone->GetDetailTextureIndex(fX,fZ);
	else
		return 0;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetWaterHeight(float& fHeight,float x,float z)
{
	int nX_Zone = GetZoneIndex(x);
	int nZ_Zone = GetZoneIndex(z);

	KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
	if(pZone)
		return pZone->GetWaterHeight(fHeight,x,z);
	else
		return E_FAIL;
}


HRESULT KG3DSceneOutDoorSpaceMgr::CalcFrustum(const TypeFrustums& vInputFrustum, DWORD dwLookMode)
{
    //m_vFrustum = vInputFrustum;
	SetFrustums(vInputFrustum);

    /*if (m_vFrustum.size() > 0)
        KG3DSceneSpaceNode::SetTopVisableNode(this);*/

	if (GetFrustums().size() > 0)
	{
		KG3DSceneSpaceNode::SetTopVisableNode(this);
	}

    return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::LoadDefaultTerrain( KG3DTERRAIN_LOAD_PARAM* pLoadParam )
{
    CreateDataInterface(TEXT(""),TRUE);
	
    int nWidth  = pLoadParam->nWidth;
    int nHeight = pLoadParam->nHeight;

	DWORD dwOption = 0;
	if(pLoadParam->nSection)
		dwOption |= CREATE_SECTION;
	if(pLoadParam->nRegion)
		dwOption |= CREATE_REGION;
	if(pLoadParam->nZoneTerrainBlock)
		dwOption |= CREATE_ZONE_TERRAINBLOCK;
	if(pLoadParam->nSectionTerrainBlock)
		dwOption |= CREATE_SECTION_TERRAINBLOCK;
	if(pLoadParam->nRegionTerrainBlock)
		dwOption |= CREATE_REGION_TERRAINBLOCK;
    
    // add by zhangzhixiong(2009.11.17)
	int nXRegionNum = 0,nYRegionNum = 0;
	bool bXLessOneSection = FALSE,bYLessOneSection = FALSE;
	if(g_bCanCreateTerrainLess128x128)
	{
		if(nWidth < EM_SCENE_SECTION_WIDTH / 100)
		{
			if(nWidth < EM_SCENE_REGION_WIDTH / 100)
				nWidth = EM_SCENE_REGION_WIDTH / 100;

			nXRegionNum = nWidth / (EM_SCENE_REGION_WIDTH / 100);
			m_nXRegionNum = nXRegionNum;
			bXLessOneSection = TRUE;
			m_bCreateTerrainLess128x128 = TRUE;
		}
		if(nHeight < EM_SCENE_SECTION_WIDTH / 100)
		{
			if(nHeight < EM_SCENE_REGION_WIDTH / 100)
				nHeight = EM_SCENE_REGION_WIDTH / 100;

			nYRegionNum = nHeight / (EM_SCENE_REGION_WIDTH / 100);
			m_nYRegionNum = nYRegionNum;
			bYLessOneSection = TRUE;
			m_bCreateTerrainLess128x128 = TRUE;
		}
	}
	// end

    if (nWidth <128 )
        nWidth = 128;

    if (nHeight <128)
        nHeight = 128;

	int nXSectionNum  = nWidth / 128;
	int nZSectionNum = nHeight / 128;
	
	nWidth  = nXSectionNum * SCENESECTIONWIDTH;
	nHeight = nZSectionNum * SCENESECTIONWIDTH;

	D3DXVECTOR2 A =D3DXVECTOR2(0,0);
    D3DXVECTOR2 C = D3DXVECTOR2(A.x + nWidth, A.y + nHeight);

    int nX_Zone = GetZoneIndex(C.x -100);
    int nZ_Zone = GetZoneIndex(C.y -100) ;

    for (int nX=0;nX<=nX_Zone;nX++)
    {
        for (int nZ=0;nZ<=nZ_Zone;nZ++)
        {
            KG3DSceneZone* pNewZone = NULL;
            NewOneZone(&pNewZone,nX,nZ);
            CreateSubDetailForZone(nX,nZ,nXSectionNum,nZSectionNum,dwOption);
        }
    }

	int MapWidth = nXSectionNum * 128;
	int MapHeight= nZSectionNum * 128;

    // add by zhangzhixiong(2009.11.17)
	if(bXLessOneSection)
		MapWidth = nXRegionNum * EM_SCENE_REGION_WIDTH / 100;
	if(bYLessOneSection)
		MapHeight = nYRegionNum * EM_SCENE_REGION_WIDTH / 100;
	// end

	if (pLoadParam->nHeightMapWidth)
	{
		MapWidth = pLoadParam->nHeightMapWidth ;
		MapHeight = pLoadParam->nHeightMapWidth;
	}
	///////创建地形
	{
		KG3DHeightMap vGroundHeight;
		if(pLoadParam->nHeightMapWidth)
		{
			vGroundHeight.CreateFromR32File(pLoadParam->nHeightMapWidth,pLoadParam->nHeightMapWidth,pLoadParam->strHeightMapName.c_str());
			vGroundHeight.SetHeightScale((float)pLoadParam->nHeightLowest,(float)pLoadParam->nHeightTotal);
		}
		else
		{
			vGroundHeight.Init(MapWidth,MapHeight);
			vGroundHeight.SetHeightScale(0,1);
		}
		vGroundHeight.ComputeNormal();
		if(pLoadParam->nRegion)
			SetRegionHeight(&vGroundHeight,A,C);
		SetSectionHeight(&vGroundHeight,A,C);
		SetZoneHeight(&vGroundHeight,A,C);
		
		
	}
	//////创建水面
	{
		KG3DHeightMap vWaterHeight;
		vWaterHeight.Init(MapWidth, MapHeight);
		vWaterHeight.SetHeightScale(0, 1);

		for (int nZ=0;nZ<MapHeight;nZ++)
			for (int nX=0;nX<MapWidth;nX++)
			{
				(*vWaterHeight.GetVertexPointer(nX,nZ)) = -50;//static_cast<float>(-1.0363697e-021);
				(*vWaterHeight.GetVertexNormalPointer(nX,nZ)) = D3DXVECTOR3(0,1,0);
			}

		//vWaterHeight.ComputeNormal();

		SetZoneWaterHeight(&vWaterHeight,A,C);
		SetSectionWaterHeight(&vWaterHeight,A,C);

		if(pLoadParam->nRegion)
			SetRegionWaterHeight(&vWaterHeight,A,C);
	}	
	
	
	SetTerrainConverTexture("data\\public\\ConverMap.tga",A,C,TRUE,TRUE,pLoadParam->nRegion);

    UINT uHandleMtlMgr = 0;
    {
        DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			m_lpDetailMtls->SaveToInformationBuffer(dwLength,pFile);
			m_lpDataInterface->Request_CreateNewTerrainInfo(uHandleMtlMgr,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
			KG3DTerrainDetailTexture* pTexture = NULL;
			m_lpDetailMtls->AddDetailTexture(&pTexture, "data\\public\\defaultscene\\001.TGA");
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
    }
	
	if (pLoadParam->nRegionTerrainBlock)
	{
		vector<POINT> vecRegion;
		SelectRegion(&vecRegion, A, C,TRUE);
		for (size_t i = 0; i < vecRegion.size(); ++i)
		{
			BYTE byteDetailTextureIndex[TERRAIN_BLOCK_MAX_DETAILMAP] = {0};
			BYTE byteTextureIndex[17 * 17] = {0};
			DWORD dwVersion = 0;
			int nNumDetail = 1;

			KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
			if(pFile)
			{
				//pFile->Reset();
				pFile->write(&dwVersion,sizeof(DWORD));
				pFile->write(&uHandleMtlMgr,sizeof(UINT));
				pFile->write(byteDetailTextureIndex,sizeof(byteDetailTextureIndex));
				pFile->write(&nNumDetail, sizeof(int));
				pFile->write(byteTextureIndex,sizeof(BYTE)*17*17);

				DWORD dwLength = pFile->GetLength();
				POINT Index = {vecRegion[i].x, vecRegion[i].y};
				m_lpDataInterface->Request_ModifyTerrainBlock(
					BLOCK_TYPE_REGION, Index,Terrain_Block_DetailMtl, dwLength,pFile->GetBuffer()
					);
				g_cMemoryFilePool.FreeResource(pFile);
				pFile = NULL;
			}
		}	
	}

    vector<POINT> vecScetion;
    SelectSection(&vecScetion, A, C,TRUE);
    for (size_t i = 0; i < vecScetion.size(); ++i)
    {
        BYTE byteDetailTextureIndex[TERRAIN_BLOCK_MAX_DETAILMAP] = {0};
        BYTE byteTextureIndex[17 * 17] = {0};
        DWORD dwVersion = 0;
        int nNumDetail = 1;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			//pFile->Reset();
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&uHandleMtlMgr,sizeof(UINT));
			pFile->write(byteDetailTextureIndex,sizeof(byteDetailTextureIndex));
			pFile->write(&nNumDetail, sizeof(int));
			pFile->write(byteTextureIndex,sizeof(BYTE)*17*17);

			DWORD dwLength = pFile->GetLength();
			POINT Index = {vecScetion[i].x, vecScetion[i].y};
			m_lpDataInterface->Request_ModifyTerrainBlock(
				BLOCK_TYPE_SECTION, Index,Terrain_Block_DetailMtl, dwLength,pFile->GetBuffer()
				);
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
    }

    vector<POINT> vecZone;
    SelectZone(&vecZone, A, C,TRUE);
    for (size_t i = 0; i < vecZone.size(); ++i)
    {
        BYTE byteDetailTextureIndex[TERRAIN_BLOCK_MAX_DETAILMAP] = {0};
        BYTE byteTextureIndex[17 * 17] = {0};
        DWORD dwVersion = 0;
        int nNumDetail = 1;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			//pFile->Reset();
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&uHandleMtlMgr,sizeof(UINT));
			pFile->write(byteDetailTextureIndex,sizeof(byteDetailTextureIndex));
			pFile->write(&nNumDetail, sizeof(int));
			pFile->write(byteTextureIndex,sizeof(BYTE)*17*17);

			DWORD dwLength = pFile->GetLength();
			POINT Index = {vecZone[i].x, vecZone[i].y};
			m_lpDataInterface->Request_ModifyTerrainBlock(
				BLOCK_TYPE_ZONE, Index,Terrain_Block_DetailMtl, dwLength,pFile->GetBuffer()
				);
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
    }
	////草//////////////////////////
	KG3DTerrainGrassSet* pNewGrassSet = new KG3DTerrainGrassSet;//自动释放资源
	ASSERT(pNewGrassSet);
	pNewGrassSet->AddRef();
	SAFE_RELEASE(m_lpSetPointer[MAP_GRASS]);
	m_lpSetPointer[MAP_GRASS] = pNewGrassSet;
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pNewGrassSet->SaveToInformationBuffer(dwLength,pFile);
			m_lpDataInterface->Request_CreateNewTerrainInfo(m_uSetHandle[MAP_GRASS],Terrain_Info_GrassSet,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	///////碎石/////////////////////////////////////////////
	KG3DTerrainGrassSet* pNewStoneSet = new KG3DTerrainGrassSet;//自动释放资源
	ASSERT(pNewStoneSet);
	pNewStoneSet->AddRef();
	SAFE_RELEASE(m_lpSetPointer[MAP_STONE]);
	m_lpSetPointer[MAP_STONE] = pNewStoneSet;
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pNewStoneSet->SaveToInformationBuffer(dwLength,pFile);
			m_lpDataInterface->Request_CreateNewTerrainInfo(m_uSetHandle[MAP_STONE],Terrain_Info_StoneSet,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}

	////灌木///////////////////////////////////////////////
	KG3DTerrainGrassSet* pNewFrutexSet = new KG3DTerrainGrassSet;//自动释放资源
	ASSERT(pNewFrutexSet);
	pNewFrutexSet->AddRef();
	SAFE_RELEASE(m_lpSetPointer[MAP_FRUTEX]);
	m_lpSetPointer[MAP_FRUTEX] = pNewFrutexSet;
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{		
			pNewFrutexSet->SaveToInformationBuffer(dwLength,pFile);
			m_lpDataInterface->Request_CreateNewTerrainInfo(m_uSetHandle[MAP_FRUTEX],Terrain_Info_FrutexSet,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////
	if(m_lpRoadMgr)
	{	
		UINT uHandleRoadMgr = 0;
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{		
			m_lpRoadMgr->SaveToInformationBuffer(dwLength,pFile);
			m_lpDataInterface->Request_CreateNewTerrainInfo(uHandleRoadMgr,Terrain_Info_RoadMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}  
	if(m_lpRiverMgr)
	{
		UINT uHandleRiverMgr = 0;
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{		
			m_lpRiverMgr->SaveToInformationBuffer(dwLength,pFile);
			m_lpDataInterface->Request_CreateNewTerrainInfo(uHandleRiverMgr,Terrain_Info_RiverMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}  

	if(m_lpConverMap)
	{
		m_lpConverMap->m_fLowest = (float)pLoadParam->nHeightLowest;
		m_lpConverMap->m_fHeightTotal = (float)pLoadParam->nHeightTotal;
	}
    return S_OK;
}


HRESULT KG3DSceneOutDoorSpaceMgr::ReplaceEntity(vector<UINT>& uEntityHandles, KG3DRepresentObject* pObject, bool all,  OUT vector<UINT>& allNeedReplaceHandles)
{
	HRESULT hr = E_FAIL;
	KG3DMemoryFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pObject);

	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	DWORD dwInfoType = pObject->GetType();
	pFile = g_cMemoryFilePool.RequestResource();

	if(pFile)
	{	
		hr = pObject->SaveToInformationBuffer(pFile);
		KG_COM_PROCESS_ERROR(hr);
		DWORD dwInfoLen = pFile->GetLength();//记录写的长度

		hr = m_lpDataInterface->Request_ReplaceEntity(uEntityHandles,dwInfoType,dwInfoLen,pFile->GetBuffer(), all, allNeedReplaceHandles);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	hr = S_OK;

Exit0:

	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}

	return hr;
}

//重新动态加载实体
HRESULT KG3DSceneOutDoorSpaceMgr::ReloadEntiyInShell(UINT uHandle, KG3DRepresentObject** lppObj)
{
   HRESULT hr = E_FAIL;
   KG3DEntityObjectShell * objectshell = m_mapObjectShell.Find(uHandle);
   KG_COM_PROCESS_ERROR(objectshell->TryDynamicUnLoad());
   KG_COM_PROCESS_ERROR(objectshell->TryDynamicLoad(0));
   *lppObj = objectshell->m_lpObject;
   hr = S_OK;
Exit0:
   return hr;
}

/*
如果bToDataInterface，那么会在内部重新创建出一个Object，可以用ppCreatedObject来得到这个新创建的Object。
ppCreatedObject可以为NULL。返回的Object不加引用计数。
*/
HRESULT KG3DSceneOutDoorSpaceMgr::InputRepresentObject(KG3DRepresentObject* pObject, int bForceLoad,BOOL bToDataInterface, KG3DRepresentObject** ppCreatedObject)
{
	HRESULT hr = S_OK;

	D3DXVECTOR3 vPos;
	//KG3DSceneZone* pZone = NULL;
	KG3DEntityTransformation vTrans;
	KG3DMemoryFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pObject);

    /* first free the obj from pvs */
    // CheckReleaseFromPvs(pObject);
    
    pObject->ComputeBBox();


	if (pObject->IsSpecial())
	{
		return m_vErrorRegion.InputRepresentObject(pObject);
	}
	vTrans.m_Translation = pObject->m_Translation;
	vTrans.m_Rotation = pObject->m_Rotation;
	vTrans.m_Scaling = pObject->m_Scaling;
	vTrans.m_vBox_A = pObject->m_BBox.A;
	vTrans.m_vBox_B = pObject->m_BBox.B;	
    vTrans.m_vWorldPos = pObject->GetWorldPos();


	if(bToDataInterface)
	{
        pObject->m_bSleep = false;
		pObject->FrameMove();//<SLEEP_REFRACTOR>
		pObject->ComputeBBox();
		vTrans.m_vBox_A = pObject->m_BBox.A;
		vTrans.m_vBox_B = pObject->m_BBox.B;	

		KGLOG_PROCESS_ERROR(m_lpDataInterface);

		//DWORD dwInfoType = 0;
		DWORD dwInfoType = pObject->GetType();

		pFile = g_cMemoryFilePool.RequestResource();
		UINT uHandleEntityInfo = 0;
		if(pFile)
		{	
			hr = pObject->SaveToInformationBuffer(pFile);
			KG_COM_PROCESS_ERROR(hr);
			DWORD dwInfoLen = pFile->GetLength();//记录写的长度


			hr = m_lpDataInterface->Request_CreateNewEntityInfo(uHandleEntityInfo,dwInfoType,dwInfoLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		UINT uHandleEntity = 0;
		hr = m_lpDataInterface->Request_CreateNewEntity(&uHandleEntity,uHandleEntityInfo, bForceLoad);
		KGLOG_COM_PROCESS_ERROR(hr);


		m_lpDataInterface->Request_ModifyEntityTransform(uHandleEntity,vTrans);

		if (NULL != ppCreatedObject)
		{
			KG3DEntityObjectShell* pShell = m_mapObjectShell.Find(uHandleEntity);
			if(pShell)
			{
				_ASSERTE(NULL != pShell);
				_ASSERTE(NULL != pShell->m_lpObject);
				*ppCreatedObject = pShell->m_lpObject;
			}
		}
	} 
	else
	{
		KG3DEntityObjectShell* pNewShell = g_cObjectShellPool.RequestResource();
		KGLOG_PROCESS_ERROR(pNewShell);

		hr = pNewShell->Init();
		KGLOG_COM_PROCESS_ERROR(hr);

		pObject->SetHandleHeldBySpaceMgr((UINT)-1);
		m_listObjectShellNoHandle.push_back(pNewShell);
		pNewShell->m_lpObject = pObject;
		pNewShell->m_uHandle = (UINT)-1;
		pNewShell->m_bLoaded = TRUE;
		pNewShell->m_lpSpaceMgr = this;

		pNewShell->m_vBBox_A = vTrans.m_vBox_A;
		pNewShell->m_vBBox_B = vTrans.m_vBox_B;
		pNewShell->m_vPosition = vTrans.m_Translation;
		pNewShell->m_fDiameter = max(700.0F,D3DXVec3Length(&(vTrans.m_vBox_B - vTrans.m_vBox_A)));
		hr = InputObjectShell(pNewShell, TRUE);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr  = S_OK;
Exit0:
	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}

	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::DelRepresentObject( UINT Handle )
{
	m_lpDataInterface->Request_DeleteEntity(Handle);
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RemoveRepresentObject(KG3DRepresentObject* pObject)
{
	if (pObject->GetType() == REPRESENTOBJECT_SOUNDBALL)
	{
		KG3DRepresentObjectSoundBall *pSoundBall = static_cast<KG3DRepresentObjectSoundBall*>(pObject);
		std::set<KG3DRepresentObjectSoundBall*>::iterator itFind = m_setObjectSoundBall.find(pSoundBall);
		if (itFind != m_setObjectSoundBall.end())
		{
			(*itFind)->Release();
			m_setObjectSoundBall.erase(itFind);
		}
	}
	if (pObject->IsSpecial())
	{
		pObject->OnRemoveFromContainer(&m_vErrorRegion);
		m_vErrorRegion.setObject.erase(pObject);
		return m_vErrorRegion.RemoveRepresentObject(pObject);
	}

	if(pObject->GetHandleHeldBySpaceMgr()==-1)
	{
		//m_listObjectShellNoHandle.remove()
		list<KG3DEntityObjectShell*>::iterator i = m_listObjectShellNoHandle.begin();//没有HANDLE的OBJECT对象
		list<KG3DEntityObjectShell*>::iterator iend = m_listObjectShellNoHandle.end();//没有HANDLE的OBJECT对象
		while (i!=iend)
		{
			KG3DEntityObjectShell* pShell =*i;
			if (pShell->m_lpObject == pObject)
			{
                pObject->LeavePvs();
				pShell->RemoveFromAllRefrenceContainer();

				pShell->UnInit();

				g_cObjectShellPool.FreeResource(pShell);

				i  =m_listObjectShellNoHandle.erase(i);
			}
			else
				i++;
		}


		return S_OK;
	}
	else
	{
		return RemoveRepresentObject(pObject->GetHandleHeldBySpaceMgr());
	}
}


//////////////////////////////////////////////////////////////////////////

HRESULT KG3DSceneOutDoorSpaceMgr::LoadOneObjectShell(KG3DEntityObjectShell* pShell,DWORD dwOption)
{
	HRESULT hr = S_OK;
	DWORD dwNowTime = timeGetTime();
	DWORD dwCost1 = 0;
	DWORD dwCost2 = 0;
	DWORD dwCost3 = 0;

	DWORD dwDataType = 0;
	DWORD dwDataLen = 0;
	KG3DEntityTransformation vTrans;
	KG3DRepresentObject* pNewObject = NULL;
	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	KGLOG_PROCESS_ERROR(pFile);

	KGLOG_PROCESS_ERROR(pShell);

	hr = m_lpDataInterface->GetEntityInformation(pShell->m_uHandle,&dwDataType,&dwDataLen,pFile->GetBuffer(),pFile->GetMaxLength());
	KGLOG_COM_PROCESS_ERROR(hr);

	ASSERT(dwDataLen);
	
	dwCost1 = timeGetTime() - dwNowTime;

	SAFE_RELEASE(pShell->m_lpObject);

	hr = g_GetObjectTable().Get1NewResource(&pNewObject,dwDataType,0);
	KGLOG_COM_PROCESS_ERROR(hr);
	KGLOG_PROCESS_ERROR(pNewObject);

	hr = pNewObject->LoadFromInformationBuffer(pFile,dwOption);
	KGLOG_COM_PROCESS_ERROR(hr);

	dwCost2 = timeGetTime() - dwNowTime;

	_ASSERTE(pNewObject->GetType() == dwDataType);//检查没有创建错

	hr = m_lpDataInterface->GetEntityTransformation(pShell->m_uHandle,vTrans);
	KGLOG_COM_PROCESS_ERROR(hr);

	pNewObject->SetHandleHeldBySpaceMgr(pShell->m_uHandle);
	
	dwCost3 = timeGetTime() - dwNowTime;

	if(pNewObject)
	{
		pNewObject->SetTranslation(&vTrans.m_Translation);
		pNewObject->SetScaling(&vTrans.m_Scaling);
		pNewObject->SetRotation(&vTrans.m_Rotation);

        /* mush do this to update the world matrix */
        pNewObject->m_bSleep = false;
        //pShell->m_lpObject->FrameMove();

		pNewObject->m_BBox.A = vTrans.m_vBox_A;
		pNewObject->m_BBox.B =  vTrans.m_vBox_B;
	}

	pShell->m_vBBox_A = vTrans.m_vBox_A;
	pShell->m_vBBox_B = vTrans.m_vBox_B;
	pShell->m_vPosition = vTrans.m_vWorldPos;
	pShell->m_fDiameter =max(700.0F, D3DXVec3Length(&(vTrans.m_vBox_B - vTrans.m_vBox_A)));

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 30)
	{
		//KGLogPrintf(KGLOG_ERR, "%d 1:%d 2:%d 3:%d KG3DSceneOutDoorSpaceMgr::LoadOneObjectShell.",dwCost,
		//	dwCost1,dwCost2,dwCost3);
	}

	pShell->m_lpObject = pNewObject;

    g_LoadPostProcessor.InputRepresentObject(pNewObject);

	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
	return S_OK;
Exit0:
	SAFE_RELEASE(pNewObject);
	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
	return E_FAIL;
}

//HRESULT KG3DSceneOutDoorSpaceMgr::UnLoadOneObjectShell(KG3DEntityObjectShell* pShell)
//{
//	KGLOG_PROCESS_ERROR(pShell);
//	
//	pShell->RemoveFromAllRefrenceContainer();
//
//	if(pShell->m_uHandle==INVALID_OBJSHELL_HANDLE)
//	{
//		m_listObjectShellNoHandle.remove(pShell);
//	}
//	else
//	{
//		TypeShellMap::iterator i = m_mapObjectShell.m_map.find(pShell->m_uHandle);
//		if(i!=m_mapObjectShell.m_map.end())
//		{
//			m_mapObjectShell.m_map.erase(i);
//		}
//	}
//
//	SAFE_RELEASE(pShell->m_lpObject);
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

HRESULT KG3DSceneOutDoorSpaceMgr::CreateDataInterface(LPCTSTR pFileName,BOOL bNewScene)
{
	HRESULT hr = S_OK;

	if(!m_lpDataInterface)
	{
		m_lpDataInterface = new KG3DSceneDataInterfaceCore();
		KGLOG_PROCESS_ERROR(m_lpDataInterface);

		hr = m_lpDataInterface->Init(pFileName,bNewScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_lpDataInterface->AddEventConnection(this);

	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::DeleteDataInterface()
{
	SAFE_DELETE(m_lpDataInterface);
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnEntityCreate(UINT uHandle, int bForceLoad)
{
	HRESULT hr = S_OK;

	if (!m_bImportOldScene && !bForceLoad)//如果是新格式，物件会在看到的时候做动态加载，这里就不用进行任何动作了
		return hr;

	if (m_mapObjectShell.IsExist(uHandle))
	{
		KGLogPrintf(KGLOG_ERR,"m_mapObjectShell 已经存在要创建的物件！");
		KGLOG_PROCESS_ERROR(0);//找到了，说明m_mapObjectShell里有DATABASE里没有的数据，这是错的
	}
	else
	{
		KG3DEntityObjectShell* pNewShell = g_cObjectShellPool.RequestResource();
		KGLOG_PROCESS_ERROR(pNewShell);

		hr = pNewShell->Init();
		KGLOG_COM_PROCESS_ERROR(hr);

		m_mapObjectShell.InputShell(uHandle,pNewShell);
		pNewShell->m_uHandle = uHandle;
		pNewShell->m_lpSpaceMgr = this;

		pNewShell->TryDynamicLoad(0);

	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnEntityDelete(UINT uHandle)
{
	if (m_mapObjectShell.IsExist(uHandle))
	{
		KG3DEntityObjectShell* pShell = m_mapObjectShell.Find(uHandle);
		KGLOG_PROCESS_ERROR(pShell);

        CheckReleaseFromPvs(pShell->m_lpObject);
        
		pShell->RemoveFromAllRefrenceContainer();

		m_mapObjectShell.Release(&g_cObjectShellPool,uHandle);

	}
	else
	{
		KGLOG_PROCESS_ERROR(0);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneOutDoorSpaceMgr::OnEntityTransformModified(UINT uHandle, BOOL bCheckInput2Pvs)
{
	HRESULT hr = S_OK;

	KG3DEntityObjectShell* pShell = m_mapObjectShell.Find(uHandle);
	if (pShell)
	{
		pShell->RemoveFromAllRefrenceContainer();

		KG3DEntityTransformation vTrans;
		hr = m_lpDataInterface->GetEntityTransformation(uHandle,vTrans);
		KGLOG_COM_PROCESS_ERROR(hr);

		if(pShell->m_lpObject)
		{
			pShell->m_lpObject->SetTranslation(&vTrans.m_Translation);
			pShell->m_lpObject->SetScaling(&vTrans.m_Scaling);
			pShell->m_lpObject->SetRotation(&vTrans.m_Rotation);
            pShell->m_lpObject->m_bSleep = false;
            pShell->m_lpObject->FrameMove();//<SLEEP_REFRACTOR>

			pShell->m_lpObject->m_BBox.A = vTrans.m_vBox_A;
			pShell->m_lpObject->m_BBox.B = vTrans.m_vBox_B;
		}
		pShell->m_vBBox_A = vTrans.m_vBox_A;
		pShell->m_vBBox_B = vTrans.m_vBox_B;
		pShell->m_vPosition = vTrans.m_vWorldPos;
		//pShell->m_vPosition = vTrans.m_Translation;
		pShell->m_fDiameter = max(700.0F,D3DXVec3Length(&(vTrans.m_vBox_B - vTrans.m_vBox_A)));

		hr = InputObjectShell(pShell, bCheckInput2Pvs);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		;//没有找到说明物件没有被动态加载，什么也不用做KGLOG_PROCESS_ERROR(0);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnEntityInformationModified(UINT uHandle)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////
HRESULT KG3DSceneOutDoorSpaceMgr::OnSceneBlockCreate(int nLevel,POINT Index)
{
	HRESULT hr = S_OK;

	/*if(!m_bImportOldScene)
		return hr;*/

	switch(nLevel)
	{
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
			if(!pZone)
			{
				KG3DSceneZone* pNewZone = g_cSceneZonePool.RequestResource();
				KGLOG_PROCESS_ERROR(pNewZone);

				pNewZone->Init(Index.x,Index.y);

				InputNewZone(pNewZone);
			}
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
			if(!pSection)
			{
				int nSectionX = Index.x;
				int nSectionZ = Index.y;

				int nX_Zone = nSectionX / SCENESECTIONNUMREGION;
				int nZ_Zone = nSectionZ / SCENESECTIONNUMREGION;

				KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
				KG_PROCESS_ERROR(pZone);

				int nX_Local = nSectionX  - nX_Zone  * SCENESECTIONNUMREGION;
				int nZ_Local = nSectionZ  - nZ_Zone  * SCENESECTIONNUMREGION;
				KG_PROCESS_ERROR( nX_Local>= 0 );
				KG_PROCESS_ERROR( nZ_Local>= 0 );
				KG_PROCESS_ERROR( nX_Local < SCENESECTIONNUMREGION );
				KG_PROCESS_ERROR( nZ_Local < SCENESECTIONNUMREGION );

				KG3DSceneSection* pNewSection = NULL;

				hr = pZone->NewOneSection(&pNewSection,nSectionX,nSectionZ,nX_Local,nZ_Local);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
		break;
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(Index.x,Index.y);
			if(!pRegion)
			{
				int nRegionX = Index.x;
				int nRegionZ = Index.y;

				int nX_Section = nRegionX / SCENESECTIONNUMREGION;
				int nZ_Section = nRegionZ / SCENESECTIONNUMREGION;

				KG3DSceneSection* pSection = GetSection( nX_Section ,nZ_Section);
				KG_PROCESS_ERROR(pSection);

				int nX_Local = nRegionX  - nX_Section  * SCENESECTIONNUMREGION;
				int nZ_Local = nRegionZ  - nZ_Section  * SCENESECTIONNUMREGION;
				KG_PROCESS_ERROR( nX_Local>= 0 );
				KG_PROCESS_ERROR( nZ_Local>= 0 );
				KG_PROCESS_ERROR( nX_Local < SCENESECTIONNUMREGION );
				KG_PROCESS_ERROR( nZ_Local < SCENESECTIONNUMREGION );

				KG3DSceneRegion* pNewRegion = NULL;
				hr = pSection->NewOneRegion(&pNewRegion,nRegionX,nRegionZ,nX_Local,nZ_Local);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
		break;
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnSceneBlockDelete(int nLevel,POINT Index)
{
	switch(nLevel)
	{
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(Index.x,Index.y);

			KGLOG_PROCESS_ERROR(pZone);

			INT nIndex =  (pZone->GetIndexZ() - m_nZoneStart_Z) * m_nNumZone_X + (pZone->GetIndexX() - m_nZoneStart_X);
			m_lpZones[nIndex] = NULL;

			m_setZone.erase(pZone);
			g_cSceneZonePool.FreeResource(pZone);
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pSection);

			int nX_Zone = Index.x / SCENESECTIONNUMREGION;
			int nZ_Zone = Index.y / SCENESECTIONNUMREGION;

			KG3DSceneZone* pZone = GetZone( nX_Zone ,nZ_Zone);
			KG_PROCESS_ERROR(pZone);

			int nX_Local = pSection->m_nIndexX  - nX_Zone  * SCENESECTIONNUMREGION;
			int nZ_Local = pSection->m_nIndexZ  - nZ_Zone  * SCENESECTIONNUMREGION;
			KG_PROCESS_ERROR( nX_Local>= 0 );
			KG_PROCESS_ERROR( nZ_Local>= 0 );
			KG_PROCESS_ERROR( nX_Local < SCENESECTIONNUMREGION );
			KG_PROCESS_ERROR( nZ_Local < SCENESECTIONNUMREGION );

			pZone->DeleteSection(pSection,pSection->m_nIndexX,pSection->m_nIndexZ,nX_Local,nZ_Local);
		}
		break;
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pRegion);

			int nX_Section = pRegion->m_nIndexX / SCENESECTIONNUMREGION;
			int nZ_Section = pRegion->m_nIndexZ / SCENESECTIONNUMREGION;

			KG3DSceneSection* pSection = GetSection( nX_Section ,nZ_Section );
			KG_PROCESS_ERROR(pSection);

			int nX_Local = pRegion->m_nIndexX  - nX_Section  * SCENESECTIONNUMREGION;
			int nZ_Local = pRegion->m_nIndexZ  - nZ_Section  * SCENESECTIONNUMREGION;
			KG_PROCESS_ERROR( nX_Local>= 0 );
			KG_PROCESS_ERROR( nZ_Local>= 0 );
			KG_PROCESS_ERROR( nX_Local < SCENESECTIONNUMREGION );
			KG_PROCESS_ERROR( nZ_Local < SCENESECTIONNUMREGION );

			pSection->DeleteRegion(pRegion,pRegion->m_nIndexX,pRegion->m_nIndexZ,nX_Local,nZ_Local);
		}
		break;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnTerrainBlockCreate(int nLevel,POINT Index)
{
	HRESULT hr = S_OK;
	switch(nLevel)
	{
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pZone);

			pZone->SetTerrainLoadState(BLOCK_LOAD_NODATA);
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pSection);
			pSection->SetTerrainLoadState(BLOCK_LOAD_NODATA);
		}
		break;
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pRegion);
			pRegion->SetTerrainLoadState(BLOCK_LOAD_NODATA);
		}
		break;
	}

	if(!m_bImportOldScene)
		return hr;

	switch(nLevel)
	{
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pZone);

			hr = pZone->CreateTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pSection);

			hr = pSection->CreateTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pRegion);

			hr = pRegion->CreateTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnTerrainBlockDelete(int nLevel,POINT Index)
{
	HRESULT hr = S_OK;

	switch(nLevel)
	{
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pZone);

			hr = pZone->DeleteTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pSection);

			hr = pSection->DeleteTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(Index.x,Index.y);
			KGLOG_PROCESS_ERROR(pRegion);

			hr = pRegion->DeleteTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnTerrainBlockModified(int nLevel,POINT Index,DWORD dwDataType)
{
	HRESULT hr = S_OK;

	switch(nLevel)
	{
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
			KG_PROCESS_ERROR(pZone);

			if(pZone->m_lpTerrainBlock)
			{
				hr = pZone->m_lpTerrainBlock->OnTerrainBlockModified(dwDataType,m_lpDataInterface,this,FALSE);
				KGLOG_COM_PROCESS_ERROR(hr);
			}

			pZone->ComputeBBox(TRUE);
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
			KG_PROCESS_ERROR(pSection);

			if(pSection->m_lpTerrainBlock)
			{
				hr = pSection->m_lpTerrainBlock->OnTerrainBlockModified(dwDataType,m_lpDataInterface,this,FALSE);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
			pSection->ComputeBBox(TRUE);
		}
		break;
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(Index.x,Index.y);
			KG_PROCESS_ERROR(pRegion);

			if(pRegion->m_lpTerrainBlock)
			{
				hr = pRegion->m_lpTerrainBlock->OnTerrainBlockModified(dwDataType,m_lpDataInterface,this,FALSE);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
			pRegion->ComputeBBox(TRUE);
		}
		break;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs)
{
    D3DXVECTOR2 A,C;
    vector<KG3DSceneZone*>vecZone;

    // KG_PROCESS_SUCCESS(CheckInputToPvs(pShell));      

	KGLOG_PROCESS_ERROR(pShell);

	pShell->SetVisibleCount(g_dwRenderCount);

	A = D3DXVECTOR2(pShell->m_vBBox_A.x,pShell->m_vBBox_A.z);
	C = D3DXVECTOR2(pShell->m_vBBox_B.x,pShell->m_vBBox_B.z);

	SelectZone(&vecZone,A,C);

//	KG_PROCESS_ERROR(! vecZone.empty());

    if (vecZone.empty())
    {
        GetErrorRegion().InputObjectShell(pShell,bCheck2Pvs);
        return S_OK;
    }

	for (size_t i=0;i<vecZone.size();i++)
	{
		KG3DSceneZone* pZone = vecZone[i];
		pZone->InputObjectShell(pShell, bCheck2Pvs);
	}

	
	return S_OK;
Exit0:


	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CreateTerrainBlock(int nLevel,int nX,int nZ)
{
	HRESULT hr = S_OK;
	POINT Index = {nX,nZ};
	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_CreateTerrainBlock(nLevel,Index);
	KGLOG_COM_PROCESS_ERROR(hr);
		
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::DeleteTerrainBlock(int nLevel,int nX,int nZ)
{
	HRESULT hr = S_OK;
	POINT Index = {nX,nZ};
	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->Request_DeleteTerrainBlock(nLevel,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::CreateSubDetailForZone(int nX,int nZ,int nXSectionNum,int nZSectionNum,DWORD dwOption)
{
	if (nXSectionNum < 1)
		nXSectionNum = 1;
	if (nZSectionNum < 1)
		nZSectionNum = 1;

	int nXend = nXSectionNum - nX * SCENESECTIONNUMREGION;
	int nZend = nZSectionNum - nZ * SCENESECTIONNUMREGION;

	if((nXend <= 0) ||(nZend <= 0))
		return S_OK;

	nXend = min((int)SCENESECTIONNUMREGION,nXend);
	nZend = min((int)SCENESECTIONNUMREGION,nZend);

	if(dwOption & CREATE_ZONE_TERRAINBLOCK)
		CreateTerrainBlock(BLOCK_TYPE_ZONE,nX,nZ);

	if (dwOption & CREATE_SECTION)
	{
		for (int z = 0;z<nZend;z++)
		{
			for (int x = 0;x<nXend;x++)
			{
				int nX_Index = nX * SCENESECTIONNUMREGION + x;
				int nZ_Index = nZ * SCENESECTIONNUMREGION + z;

				KG3DSceneSection* pNewSection = NULL;

				NewOneSection(&pNewSection,nX_Index,nZ_Index);

				CreateSubDetailForSection(nX_Index,nZ_Index,dwOption);
			}
		}
	}
	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::CreateSubDetailForZone(int nX,int nZ,DWORD dwOption)
{
	if(dwOption & CREATE_ZONE_TERRAINBLOCK)
		CreateTerrainBlock(BLOCK_TYPE_ZONE,nX,nZ);

	if (dwOption & CREATE_SECTION)
	{
		for (int z = 0;z<SCENESECTIONNUMREGION;z++)
		{
			for (int x = 0;x<SCENESECTIONNUMREGION;x++)
			{
				int nX_Index = nX * SCENESECTIONNUMREGION + x;
				int nZ_Index = nZ * SCENESECTIONNUMREGION + z;

				KG3DSceneSection* pNewSection = NULL;
				
				NewOneSection(&pNewSection,nX_Index,nZ_Index);

				CreateSubDetailForSection(nX_Index,nZ_Index,dwOption);
			}
		}
	}
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CreateSubDetailForSection(int nX,int nZ,DWORD dwOption)
{
	//KG3DSceneSection* pSection = GetSection(nX,nZ);
	//KGLOG_PROCESS_ERROR(pSection);
	if (dwOption & CREATE_SECTION_TERRAINBLOCK)
	{
		CreateTerrainBlock(BLOCK_TYPE_SECTION,nX,nZ);
	}

	// add by zhangzhixiong(2009.11.17)
	int nXRegionNum = SCENESECTIONNUMREGION,nYRegionNum = SCENESECTIONNUMREGION;
	if(m_bCreateTerrainLess128x128)
	{
        nXRegionNum = m_nXRegionNum;
		nYRegionNum = m_nYRegionNum;
		m_bCreateTerrainLess128x128 = FALSE;
	}

	if(dwOption & CREATE_REGION)
	{
		//Modify by zhangzhixiong(2009.11.17)
		for (int z = 0;z<nXRegionNum;z++)
		{
			for (int x = 0;x<nYRegionNum;x++)
			{
				int nX_Index = nX * SCENESECTIONNUMREGION + x;
				int nZ_Index = nZ * SCENESECTIONNUMREGION + z;

				KG3DSceneRegion* pNewRegion = NULL;

				NewOneRegion(&pNewRegion,nX_Index,nZ_Index);

				if(dwOption & CREATE_REGION_TERRAINBLOCK)
				{
					CreateTerrainBlock(BLOCK_TYPE_REGION,nX_Index,nZ_Index);
				}
			}
		}
	}

	return S_OK;
}


HRESULT KG3DSceneOutDoorSpaceMgr::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	HRESULT hr = E_FAIL;
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	DWORD dwNowTime = timeGetTime();

	wsprintf(path_buffer,"%s",cszFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	if(m_lpRoadMgr)
	{
		TCHAR szRoadDir[MAX_PATH];
		wsprintf(szRoadDir,"%s%s%sRoad\\",drive,dir,fname);
		m_lpRoadMgr->m_strFilePath = szRoadDir;
	}
	if(m_lpRiverMgr)
	{
		TCHAR szRiverDir[MAX_PATH];
		wsprintf(szRiverDir,"%s%s%sRiver\\",drive,dir,fname);
		m_lpRiverMgr->m_strFilePath = szRiverDir;
	}

	hr = CreateDataInterface(cszFileName,FALSE);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpDataInterface->LoadFromFile(cszFileName,0,uOption);
	KGLOG_COM_PROCESS_ERROR(hr);


	{
		D3DXVECTOR4 vRect = m_lpConverMap->GetRect();
		D3DXVECTOR4 vZero(0, 0, 0, 0);
		if (MT_VECTOR_EQUAL(vRect, vZero, 0.1))
		{
			//错误的Rect，需要修正
			D3DXVECTOR2 vLowerLeft, vUpperRight;
			this->GetSceneRect(&vLowerLeft, &vUpperRight);
			m_lpConverMap->SetRectVector(vLowerLeft.x, vLowerLeft.y, vUpperRight.x - vLowerLeft.x
										, vUpperRight.y - vLowerLeft.y);
		}
		
		TCHAR szTerrainMap[MAX_PATH];
		wsprintf(szTerrainMap,"%s%s%s_TerrainMap.tga",drive,dir,fname);
		m_lpConverMap->SetTerrainMapName(szTerrainMap);
		if (g_cEngineOption.nTerrainDetail > 0)
		{
			m_lpConverMap->LoadTerrainMap(szTerrainMap);
		}
		
	}

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneOutDoorSpaceMgr::LoadFromFile cost %d",dwCost);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SaveToFile(const char cszFileName[], unsigned uOption)
{
	HRESULT hr = E_FAIL;

	//uOption有用，用来指定存盘的文件
	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	hr = m_lpDataInterface->SaveToFile(cszFileName,uOption);
	KGLOG_COM_PROCESS_ERROR(hr);
	{
		TCHAR strDrive[MAX_PATH];
		TCHAR strDir[MAX_PATH];
		TCHAR strFileName[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath(cszFileName, strDrive, strDir, strFileName, strExt);
		if (m_lpRoadMgr)
		{	
			TCHAR Path_Buffer[256];
			wsprintf(Path_Buffer,"%s%s%s",strDrive, strDir,strFileName);
			hr = m_lpRoadMgr->SaveToFile(Path_Buffer);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		if (m_lpRiverMgr)
		{	
			TCHAR Path_Buffer[256];
			wsprintf(Path_Buffer,"%s%s%s",strDrive, strDir,strFileName);
			hr = m_lpRiverMgr->SaveToFile(Path_Buffer);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::InputNewZone(KG3DSceneZone* pNewZone)
{
	m_setZone.insert(pNewZone);

	HRESULT hr = E_FAIL;
	BOOL bNeedResize = FALSE;

	int nX = pNewZone->GetIndexX();
	int nZ = pNewZone->GetIndexZ();

	int nZoneStart_X = m_nZoneStart_X;
	int nZoneStart_Z = m_nZoneStart_Z;
	int nZoneEnd_X = m_nZoneStart_X + m_nNumZone_X;
	int nZoneEnd_Z = m_nZoneStart_Z + m_nNumZone_Z;

	if( nX < m_nZoneStart_X )
	{
		bNeedResize = TRUE;
		nZoneStart_X = nX;
	}
	if( nZ < m_nZoneStart_Z )
	{
		bNeedResize = TRUE;
		nZoneStart_Z = nZ;
	}
	if( nX >= m_nZoneStart_X + m_nNumZone_X)
	{
		bNeedResize = TRUE;
		nZoneEnd_X = nX;
	}
	if( nZ >= m_nZoneStart_Z + m_nNumZone_Z)
	{
		bNeedResize = TRUE;
		nZoneEnd_Z = nZ;
	}

	if(bNeedResize)
	{
		int nNumZond_X = nZoneEnd_X - nZoneStart_X + 1;
		int nNumZond_Z = nZoneEnd_Z - nZoneStart_Z + 1;

		hr = Resize(nZoneStart_X,nZoneStart_Z,nNumZond_X,nNumZond_Z);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		hr = InputZoneToArray( pNewZone );
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::ProcessDynamicLoad(BOOL bForceSetRect,D3DXVECTOR2 vPosition,D3DXVECTOR2 vDir,float fRZone,float fRSection,float fRRegion,KG3DSceneSceneEditor* pEditor)
{
    static DWORD s_dwLastProecssDynamicLoadFrame = g_dwRenderCount;
	float fSound = 5000;
	KGLOG_PROCESS_ERROR(m_lpSceneBlockLoader);

	m_lpSceneBlockLoader->m_fRZone    = fRZone;
	m_lpSceneBlockLoader->m_fRSection = fRSection;
	m_lpSceneBlockLoader->m_fRRegion  = fRRegion; 
	m_lpSceneBlockLoader->m_DynamicLoadPositionNow = vPosition;
	m_lpSceneBlockLoader->m_DynamicLoadDirection = vDir;

	if (bForceSetRect)
	{
		D3DXVECTOR2 A_Zone,C_Zone;
		D3DXVECTOR2 A_Section,C_Section;
		D3DXVECTOR2 A_Region,C_Region;
		D3DXVECTOR2 A_Sound,C_Sound;

		A_Zone    = vPosition - D3DXVECTOR2(fRZone,fRZone);
		C_Zone    = vPosition + D3DXVECTOR2(fRZone,fRZone);

		/*A_Section = vPosition - D3DXVECTOR2(fRSection,fRSection);
		C_Section = vPosition + D3DXVECTOR2(fRSection,fRSection);
		
		A_Region  = vPosition - D3DXVECTOR2(fRRegion,fRRegion);
		C_Region  = vPosition + D3DXVECTOR2(fRRegion,fRRegion);*/
		m_lpSceneBlockLoader->GetLoadRect(vPosition,A_Section,C_Section,A_Region,C_Region);
		A_Sound   = vPosition - D3DXVECTOR2(fSound,fSound);
		C_Sound   = vPosition + D3DXVECTOR2(fSound,fSound);
		
		m_lpSceneBlockLoader->SetLoadRect(A_Zone,C_Zone,A_Section,C_Section,A_Region,C_Region,FALSE);

		m_lpSceneBlockLoader->m_DynamicLoadPositionLast = vPosition;

		//////////////////////////////////////////////////////////////////////////
		m_nTotalLoadStep = 0;
		m_nCurrentLoadStep = 0;

		m_nTotalLoadStep += (int)m_lpSceneBlockLoader->m_LoadList.m_listSceneObjectRegionToLoad .size();
		m_nTotalLoadStep += (int)m_lpSceneBlockLoader->m_LoadList.m_listSceneObjectSectionToLoad.size();
		m_nTotalLoadStep += (int)m_lpSceneBlockLoader->m_LoadList.m_listSceneObjectZoneToLoad   .size();
		
	}
	else
	{
        BOOL bNeedForceLoad = (g_dwRenderCount - s_dwLastProecssDynamicLoadFrame) >= TERRAIN_UNLOAD_FRAME_INTERVAL;
        if (bNeedForceLoad)
        {
            D3DXVECTOR2 A_Zone,C_Zone;
            D3DXVECTOR2 A_Section,C_Section;
            D3DXVECTOR2 A_Region,C_Region;
            m_lpSceneBlockLoader->m_fRRegion  = 20000; 

            A_Zone = vPosition - D3DXVECTOR2(fRZone,fRZone);
            C_Zone = vPosition + D3DXVECTOR2(fRZone,fRZone);

            m_lpSceneBlockLoader->GetLoadRect(vPosition,A_Section,C_Section,A_Region,C_Region);

            m_lpSceneBlockLoader->SetLoadRect(A_Zone,C_Zone,A_Section,C_Section,A_Region,C_Region,FALSE);
            m_lpSceneBlockLoader->ForceLoad();
            m_lpSceneBlockLoader->m_DynamicLoadPositionLast = vPosition;
        }
        else
        {
		    D3DXVECTOR2 vA = m_lpSceneBlockLoader->m_DynamicLoadPositionNow - m_lpSceneBlockLoader->m_DynamicLoadPositionLast;
		    float rA = D3DXVec2LengthSq(&vA);
		    float S = 25600;

		    if ((rA >= S))
		    {
			    m_lpSceneBlockLoader->m_bNeedRefreshLoadList = TRUE;
			    m_lpSceneBlockLoader->m_DynamicLoadPositionLast = m_lpSceneBlockLoader->m_DynamicLoadPositionNow;
		    }
        }
	}

    s_dwLastProecssDynamicLoadFrame = g_dwRenderCount;
	
	return S_OK;
Exit0:
	return E_FAIL;
}


bool KG3DSceneOutDoorSpaceMgr::CheckNeedLoad(UINT uHandle)
{
    if (uHandle == INVALID_OBJSHELL_HANDLE)
        return false;

    UINT uParent = GetEntityParent(uHandle);

    if (uParent == INVALID_OBJSHELL_HANDLE)
        return true;

	KG3DEntityObjectShell* pShell = m_mapObjectShell.Find(uParent);

    if (!pShell)
        return false;

    if (pShell->m_lpObject && pShell->m_lpObject->GetType() == REPRESENTOBJECT_PVS)
    {
        KG3DSceneSpaceNode* pNode = pShell->m_lpObject->Conver2SpaceNode();
        if (pNode)
        {
            if (pNode->GetFrustums().size() <= 0)
                return false;
        }
    }

    return true;
}

HRESULT KG3DSceneOutDoorSpaceMgr::DoDynamicLoadEntityContainer(KG3DSceneEntityContainer* pCon,BOOL bLoadAll,DWORD dwOption)
{
	//pCon->Clear();
	static UINT azHandles[1024];
	DWORD dwCount = 0;
	static UINT azHandlesToErase[1024];
	DWORD dwCountErase = 0;

	pCon->m_Data.GetUnLoadObjectShellHandles(dwCount,azHandles,1024);

	for (DWORD i=0;i<dwCount;++i)
	{
		UINT uHandle = azHandles[i];

		if (CheckNeedLoad(uHandle))
		{
			DoDynamicLoadOneEntity(uHandle,dwOption);
		}
		else
		{
			continue;
		}

		if(!bLoadAll)
		{
			azHandlesToErase[dwCountErase++] = uHandle;
			break;
		}
	}

	if(bLoadAll)
		pCon->m_Data.ClearUnLoadObjectShellHandles();
	else
		pCon->m_Data.EraseUnLoadObjectShellHandles(dwCountErase,azHandlesToErase);


	return S_OK;
}


HRESULT KG3DSceneOutDoorSpaceMgr::DoDynamicLoadOneEntity(UINT uEntityHandle,DWORD dwOption)
{

	HRESULT hr = E_FAIL;
	KG_USE_ARGUMENT(hr);

	DWORD dwNowTime = timeGetTime();

	DWORD dwThreadID = ::GetCurrentThreadId();

	if(dwThreadID == g_dwMainThreadID)
	{
		if (!m_mapObjectShell.IsExist(uEntityHandle))
		{
			KG3DEntityObjectShell* pNewShell = g_cObjectShellPool.RequestResource();
			KGLOG_PROCESS_ERROR(pNewShell);

			pNewShell->Init();

			pNewShell->m_uHandle = uEntityHandle;
			pNewShell->m_lpSpaceMgr = this;
			

			pNewShell->TryDynamicLoad(dwOption);

			pNewShell->RemoveFromAllRefrenceContainer();

			//这里要重新计算一次BBox，因为保存的Shell的Box可能不是正确的。注意下面几个函数的顺序，不要搞错
			if (NULL != pNewShell->m_lpObject )
			{
				pNewShell->m_lpObject->FrameMove();
				if (pNewShell->m_lpObject->IsLoaded())
				{
					pNewShell->m_lpObject->ComputeBBox();
					//pNewShell->m_vBBox_A = pNewShell->m_lpObject->m_BBox.A;
					//pNewShell->m_vBBox_B = pNewShell->m_lpObject->m_BBox.B;
				}
			}
						
			m_mapObjectShell.InputShell(uEntityHandle,pNewShell);

			hr = InputObjectShell(pNewShell, FALSE);
			KGLOG_COM_PROCESS_ERROR(hr);

		}
	}
	else
	{
		BOOL bExist = m_mapObjectShell.IsExist(uEntityHandle);
		if(!bExist)
		{
			KG3DEntityObjectShell* pNewShell = g_cObjectShellPool.RequestResource();
			KGLOG_PROCESS_ERROR(pNewShell);

			pNewShell->Init();

			m_mapObjectShell.InputShell(uEntityHandle,pNewShell);
			pNewShell->m_uHandle = uEntityHandle;
			pNewShell->m_lpSpaceMgr = this;

			pNewShell->TryDynamicLoad(dwOption);

			pNewShell->RemoveFromAllRefrenceContainer();
			InputObjectShell(pNewShell,TRUE);
			//这里不用算BBox，动态加载的物件在FrameMove，OutPutEntityShell之后再计算才是正确的
		}
	}

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 30)
	{
		//KGLogPrintf(KGLOG_ERR, "%d 1:%d KG3DSceneBlockLoader::DoDynamicLoadOneEntity.",dwCost,cost1);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::OnTerrainInformationModified(UINT uInfoHandle,DWORD dwDataType)
{
	HRESULT hr = E_FAIL;
	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	KGLOG_PROCESS_ERROR(pFile);

	switch(dwDataType)
	{
	case Terrain_Info_Convermap:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			m_uConvermapHandle = uInfoHandle;

			KGLOG_PROCESS_ERROR(m_lpConverMap);
			hr = m_lpConverMap->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);

			m_lpConverMap->EnableDynamic(TRUE);
			break;
		}      
	case Terrain_Info_DetailMtlMgr:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			m_lpDetailMtls->m_uHandle = uInfoHandle;
			hr = m_lpDetailMtls->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case Terrain_Info_GrassSet:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			m_uSetHandle[MAP_GRASS] = uInfoHandle;
			KG3DTerrainGrassSet* pGrassSet = m_lpSetPointer[MAP_GRASS];
			if(!pGrassSet)
			{
				hr = E_FAIL;
				pGrassSet = new KG3DTerrainGrassSet;
				KGLOG_PROCESS_ERROR(pGrassSet);
				pGrassSet->AddRef();
				pGrassSet->Init();
				m_lpSetPointer[MAP_GRASS] = pGrassSet;
			}

			hr = pGrassSet->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case Terrain_Info_StoneSet:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			m_uSetHandle[MAP_STONE] = uInfoHandle;
			KG3DTerrainGrassSet* pStoneSet = m_lpSetPointer[MAP_STONE];
			if(!pStoneSet)
			{
				hr = E_FAIL;
				pStoneSet = new KG3DTerrainGrassSet;
				KGLOG_PROCESS_ERROR(pStoneSet);
				pStoneSet->AddRef();
				pStoneSet->Init();
				m_lpSetPointer[MAP_STONE] = pStoneSet;
			}

			hr = pStoneSet->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case Terrain_Info_FrutexSet:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			m_uSetHandle[MAP_FRUTEX] = uInfoHandle;
			KG3DTerrainGrassSet* pFrutexSet = m_lpSetPointer[MAP_FRUTEX];
			if(!pFrutexSet)
			{
				hr = E_FAIL;
				pFrutexSet = new KG3DTerrainGrassSet;
				KGLOG_PROCESS_ERROR(pFrutexSet);
				pFrutexSet->AddRef();
				pFrutexSet->Init();
				m_lpSetPointer[MAP_FRUTEX] = pFrutexSet;
			}

			hr = pFrutexSet->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case Terrain_Info_RoadMgr:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			m_lpRoadMgr->m_uHandle = uInfoHandle;
			hr = m_lpRoadMgr->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	case Terrain_Info_RiverMgr:
		{
			DWORD dwType = 0;
			DWORD dwLen = 0;
			hr = m_lpDataInterface->GetTerrainInformation(uInfoHandle,&dwType,&dwLen,pFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			m_lpRiverMgr->m_uHandle = uInfoHandle;
			hr = m_lpRiverMgr->LoadFromInformationBuffer(dwLen,pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		break;
	}


	hr = S_OK;
Exit0:
	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
	return hr;
}


void KG3DSceneOutDoorSpaceMgr::GetObjectShellIntersectLine2D(set<KG3DEntityObjectShell*>* plistShell,
                                                             const D3DXVECTOR2& A, 
                                                             const D3DXVECTOR2& C 
                                                             )
{
    int x0 = GetZoneIndex(A.x);
    int z0 = GetZoneIndex(A.y);
    int x1 = GetZoneIndex(C.x);
    int z1 = GetZoneIndex(C.y);

    int dx = x1 - x0;
    int dz = z1 - z0;

    D3DXVECTOR2 s = D3DXVECTOR2(static_cast<FLOAT>(x0), static_cast<FLOAT>(z0));
    D3DXVECTOR2 d = D3DXVECTOR2(static_cast<FLOAT>(x1), static_cast<FLOAT>(z1));

    int cnt = max(abs(dx), abs(dz));

    D3DXVECTOR2 r;

    int x;
    int y;

    /*
     * get objectshell from zone
     */

    if (cnt)
    {
        for (int i = 0; i <= cnt; i++)
        {

            D3DXVec2Lerp(&r, &s, &d, (float)i / (float)cnt);

            if (r.x > 0.f)
                x = (int)(r.x + 0.0005f);
            else
                x = (int)(r.x - 0.0005f);

            if (r.y > 0.f)
                y = (int)(r.y + 0.0005f);
            else
                y = (int)(r.y - 0.0005f);

            KG3DSceneZone* pZone = GetZone(x, y);

            if (pZone)
            {
				pZone->m_Data.GetAllObjectShell(plistShell);
            }
        }	 
    }
    else
    {
        KG3DSceneZone* pZone = GetZone(static_cast<int>(s.x), static_cast<int>(s.y));

        if (pZone)
        {
			pZone->m_Data.GetAllObjectShell(plistShell);
        }
    }


    /*
     * get objectshell from section
     */

    x0 = GetSceneSectionIndex(A.x);
    z0 = GetSceneSectionIndex(A.y);
    x1 = GetSceneSectionIndex(C.x);
    z1 = GetSceneSectionIndex(C.y);

    dx = x1 - x0;
    dz = z1 - z0;

    s = D3DXVECTOR2(static_cast<FLOAT>(x0), static_cast<FLOAT>(z0));
    d = D3DXVECTOR2(static_cast<FLOAT>(x1), static_cast<FLOAT>(z1));

    cnt = max(abs(dx), abs(dz));

    if (cnt)
    {
        for (int i = 0; i <= cnt; i++)
        {

            D3DXVec2Lerp(&r, &s, &d, (float)i / (float)cnt);

            if (r.x > 0.f)
                x = (int)(r.x + 0.0005f);
            else
                x = (int)(r.x - 0.0005f);

            if (r.y > 0.f)
                y = (int)(r.y + 0.0005f);
            else
                y = (int)(r.y - 0.0005f);

            KG3DSceneSection* pSection = GetSection(x, y);

            if (pSection)
            {
				pSection->m_Data.GetAllObjectShell(plistShell);
            }
        }	 
    }
    else
    {
        KG3DSceneSection* pSection = GetSection(static_cast<int>(s.x), static_cast<int>(s.y));

        if (pSection)
        {
			pSection->m_Data.GetAllObjectShell(plistShell);
        }
    }


    /*
     * get objectshell from region
     */

    x0 = GetSceneRegionIndex(A.x);
    z0 = GetSceneRegionIndex(A.y);
    x1 = GetSceneRegionIndex(C.x);
    z1 = GetSceneRegionIndex(C.y);

    dx = x1 - x0;
    dz = z1 - z0;

    s = D3DXVECTOR2(static_cast<FLOAT>(x0), static_cast<FLOAT>(z0));
    d = D3DXVECTOR2(static_cast<FLOAT>(x1), static_cast<FLOAT>(z1));

    cnt = max(abs(dx), abs(dz));

    if (cnt)
    {
        for (int i = 0; i <= cnt; i++)
        {

            D3DXVec2Lerp(&r, &s, &d, (float)i / (float)cnt);

            if (r.x > 0.f)
                x = (int)(r.x + 0.0005f);
            else
                x = (int)(r.x - 0.0005f);

            if (r.y > 0.f)
                y = (int)(r.y + 0.0005f);
            else
                y = (int)(r.y - 0.0005f);

            KG3DSceneRegion* pRegion = GetRegion(x, y);

            if (pRegion)
            {
				pRegion->m_Data.GetAllObjectShell(plistShell);
            }
        }	 
    }
    else
    {
        KG3DSceneRegion* pRegion = GetRegion(static_cast<int>(s.x), static_cast<int>(s.y));

        if (pRegion)
        {
			pRegion->m_Data.GetAllObjectShell(plistShell);
        }
    }

}

/*
BOOL KG3DSceneOutDoorSpaceMgr::RayIntersection( D3DXVECTOR3 vSrc, D3DXVECTOR3 vNormalizedDir, D3DXVECTOR3& vRet)
{
	return m_Funcs.RayIntersection(vSrc, vNormalizedDir, vRet);
}*/


HRESULT KG3DSceneOutDoorSpaceMgr::CheckInput2Pvs(KG3DModel* pModel)
{
    if (!pModel)
        return E_FAIL;

    D3DXVECTOR3 vPos = D3DXVECTOR3(pModel->m_matWorld._41, pModel->m_matWorld._42, pModel->m_matWorld._43);
    vPos.y += (pModel->GetBBox().GetHeight() / 2);

    //D3DXVECTOR3 vPos = pModel->GetBBox().GetCenter();

    
    /* check zone first */

    int nIndexX = GetSceneZoneIndex(vPos.x);
    int nIndexZ = GetSceneZoneIndex(vPos.z);

    KG3DSceneZone* pZone = GetZone(nIndexX, nIndexZ);

    if (!pZone)
        return E_FAIL;

	if(SUCCEEDED(pZone->m_Data.CheckInput2Pvs(vPos,pModel)))
		return S_OK;

    /* check section */

    nIndexX = GetSceneSectionIndex(vPos.x);
    nIndexZ = GetSceneSectionIndex(vPos.z);

    KG3DSceneSection* pSection = GetSection(nIndexX, nIndexZ);

    if (!pSection)
        return E_FAIL;

	if(SUCCEEDED(pSection->m_Data.CheckInput2Pvs(vPos,pModel)))
		return S_OK;

    /* check region */

    nIndexX = GetSceneRegionIndex(vPos.x);
    nIndexZ = GetSceneRegionIndex(vPos.z);

    KG3DSceneRegion* pRegion = GetRegion(nIndexX, nIndexZ);

    if (!pRegion)
        return E_FAIL;

	if(SUCCEEDED(pRegion->m_Data.CheckInput2Pvs(vPos,pModel)))
		return S_OK;

    return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::InputModel(KG3DModel* pModel)
{
	D3DXVECTOR3 vPos(0,0,0);
	KG3DSceneRegion* pRegion = NULL;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(pModel);
	KGLOG_PROCESS_ERROR(dwThreadID==g_dwMainThreadID);

	pModel->LeaveSceneRegion(TRUE);
    pModel->LeavePvs(TRUE);

    if (SUCCEEDED(CheckInput2Pvs(pModel)))
        return S_OK;

	pModel->GetTranslation(&vPos);
	int nIndex_X = GetSceneRegionIndex(vPos.x);
	int nIndex_Z = GetSceneRegionIndex(vPos.z);

	pRegion = GetRegion(nIndex_X,nIndex_Z);
	KG_PROCESS_ERROR(pRegion);

	pRegion->Add(pModel);
	/*
	pModel->EnterSceneRegion(pRegion);
	
		if (pModel->m_Translation.y >= pRegion->GetAABBox().B.y )
		{
			pRegion->m_BBox.B.y = pModel->m_Translation.y + 1000;
	
			//pRegion->m_BBox.Extract(pRegion->m_BoxPos);
		}*/
	
	return S_OK;
Exit0:
	if(pModel)
	{
		//pModel->EnterSceneRegion(&m_vErrorRegion);
		m_vErrorRegion.Add(pModel);
	}
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RemoveModel(KG3DModel* pModel)
{
	KGLOG_PROCESS_ERROR(pModel);
	pModel->LeaveSceneRegion(TRUE);
    pModel->LeavePvs(TRUE);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetObjectShellIntersectPoint2D(list<KG3DEntityObjectShell*>*plistShell,D3DXVECTOR2& vPos)
{
	KGLOG_PROCESS_ERROR(plistShell);
	//Zone
	{
		int nIndex_Zone_X = GetZoneIndex(vPos.x);
		int nIndex_Zone_Z = GetZoneIndex(vPos.y);

		KG3DSceneZone* pZone = GetZone(nIndex_Zone_X,nIndex_Zone_Z);
		if(pZone)
		{
			pZone->m_Data.GetObjectShellIntersectPoint2D(plistShell,vPos);
		}
	}
	//Section
	{
		int nIndex_Section_X = GetSceneSectionIndex(vPos.x);
		int nIndex_Section_Z = GetSceneSectionIndex(vPos.y);

		KG3DSceneSection* pSection = GetSection(nIndex_Section_X,nIndex_Section_Z);
		if(pSection)
		{
			pSection->m_Data.GetObjectShellIntersectPoint2D(plistShell,vPos);
		}
	}
	//Region
	{
		int nIndex_Region_X = GetSceneRegionIndex(vPos.x);
		int nIndex_Region_Z = GetSceneRegionIndex(vPos.y);

		KG3DSceneRegion* pRegion = GetRegion(nIndex_Region_X,nIndex_Region_Z);
		if(pRegion)
		{
			pRegion->m_Data.GetObjectShellIntersectPoint2D(plistShell,vPos);
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

float KG3DSceneOutDoorSpaceMgr::GetLoadingProcess()
{
	m_lpSceneBlockLoader->FrameMove();

	int nCount = 0;

	nCount += (int)m_lpSceneBlockLoader->m_LoadList.m_listSceneObjectRegionToLoad .size();
	nCount += (int)m_lpSceneBlockLoader->m_LoadList.m_listSceneObjectSectionToLoad.size();
	nCount += (int)m_lpSceneBlockLoader->m_LoadList.m_listSceneObjectZoneToLoad   .size();

	if(m_nTotalLoadStep == 0)
	{
		return 1;
	}
	else
	{
		return 1 - nCount * 1.0F / m_nTotalLoadStep;
	}
}

HRESULT KG3DSceneOutDoorSpaceMgr::RenderBrush()
{
	g_cTerrainBlockModifier.SetBrushPosition(this);
	g_cTerrainBlockModifier.RenderBrush();
	return S_OK;
}

static void GetTriangleNormal(D3DXVECTOR3& vec3Normal, 
							  const D3DXVECTOR3& vec3A,
							  const D3DXVECTOR3& vec3B,
							  const D3DXVECTOR3& vec3C)
{
	D3DXVec3Cross(&vec3Normal, &(vec3A - vec3B), &(vec3C - vec3B));
	D3DXVec3Normalize(&vec3Normal, &vec3Normal);
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetRegionNormal(D3DXVECTOR3& vec3Normal, float fX, float fZ)
{
	HRESULT hr = E_FAIL;
	VFormat::_Faces_Normal_Texture0* pRegionData = NULL;

	int nIndex[3];
	float fSlope = 0.0f;
	float fOffsetX = 0;
	float fOffsetZ = 0;

	int nCellX = 0;
	int nCellZ = 0;

	int nRegionX = GetSceneRegionIndex(fX);
	int nRegionZ = GetSceneRegionIndex(fZ);

	KG3DSceneRegion* pRegion = GetRegion(nRegionX,nRegionZ);

	KG_PROCESS_ERROR(pRegion);
    KG_PROCESS_ERROR(pRegion->m_lpTerrainBlock);
	KG_PROCESS_ERROR(pRegion->m_lpTerrainBlock->m_Vertexs);

	pRegionData = pRegion->m_lpTerrainBlock->m_Vertexs;

	fOffsetX = fX - nRegionX * SCENEREGIONWIDTH;
	fOffsetZ = fZ - nRegionZ * SCENEREGIONWIDTH;

	nCellX = static_cast<int>(fOffsetX / 100);
	nCellZ = static_cast<int>(fOffsetZ / 100);

	nIndex[0] = nCellZ * 17 + nCellX;

	fSlope = fOffsetX / fOffsetZ;

	if (fSlope > 0.0f)
	{
		nIndex[1] = (nCellZ + 1) * 17 + nCellX;
		nIndex[2] = (nCellZ + 1) * 17 + nCellX + 1;
	}
	else
	{
		nIndex[1] = (nCellZ + 1) * 17 + nCellX + 1;
		nIndex[2] = nCellZ * 17 + nCellX + 1;
	}

	GetTriangleNormal(vec3Normal, 
		pRegionData[nIndex[0]].p, 
		pRegionData[nIndex[1]].p,
		pRegionData[nIndex[2]].p);

	vec3Normal = -vec3Normal;

	hr = S_OK;
Exit0:

	return hr;
}
HRESULT KG3DSceneOutDoorSpaceMgr::FindPattern(LPCSTR strFileName, INT *pnRetIndex, DWORD dwType)
{
	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	KG3DTerrainGrassSet* pSet = m_lpSetPointer[dwType];
	if(!pSet)
	{	
		return E_FAIL;
	}
	return pSet->LoadPattern(strFileName,*pnRetIndex);
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::SetGrassSetTexture(LPCSTR strFileName,DWORD dwType)
{
	HRESULT hr = E_FAIL;
	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	KGLOG_PROCESS_ERROR(pFile);

	KG3DTerrainGrassSet* pSet = m_lpSetPointer[dwType];
	if (pSet)
	{
		DWORD dwLength = 0;
		pSet->SetTexture(strFileName);
		pSet->SaveToInformationBuffer(dwLength,pFile);
		switch(dwType)
		{
		case MAP_GRASS:
			m_lpDataInterface->Request_ModifyTerrainInformation(m_uSetHandle[MAP_GRASS],Terrain_Info_GrassSet,dwLength,pFile->GetBuffer());
			break;
		case MAP_STONE:
			m_lpDataInterface->Request_ModifyTerrainInformation(m_uSetHandle[MAP_STONE],Terrain_Info_StoneSet,dwLength,pFile->GetBuffer());
		    break;
		case MAP_FRUTEX:
			m_lpDataInterface->Request_ModifyTerrainInformation(m_uSetHandle[MAP_FRUTEX],Terrain_Info_FrutexSet,dwLength,pFile->GetBuffer());
		    break;
		}
	}
	hr = S_OK;
Exit0:
	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
	return hr;
}
std::string KG3DSceneOutDoorSpaceMgr::GetGrassSetTextureName(DWORD dwType)
{
	std::string strName = ""; 
	KG3DTerrainGrassSet* pSet = m_lpSetPointer[dwType];
	if (pSet)
	{
		strName = pSet->m_strTextureFileName;
	}
	return strName;
}
HRESULT KG3DSceneOutDoorSpaceMgr::SetCurGrassSet(DWORD dwType)
{
	if(dwType > 2)
		return E_FAIL;
	g_cTerrainBlockModifier.SetType(dwType);
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::Traverse( KG3DEntityCtVisitor& visitor, D3DXVECTOR2 vSrc, D3DXVECTOR2 vDes )
{	
	//这个算法要改的话要很小心
	KG3DBlockTraverseData	RegionData = KG3DBlockTraverseData::Build(vSrc, vDes, (FLOAT)SCENEREGIONWIDTH);

	INT nLastSectionX = INT_MIN, nLastSectionY = INT_MIN, nLastZoneX = INT_MIN, nLastZoneY = INT_MIN;

	bool bBreakRegionInTheSameSection = false;
	bool bBreakSectionInTheSameZone = false;

	KG3DSceneEntityContainer* pLastRegionVisited = NULL;
	KG3DSceneEntityContainer* pLastSectionVisited = NULL;
	KG3DSceneEntityContainer* pLastZoneVisited = NULL;
	KG3DSceneEntityContainer* pCurRegion = NULL;
	KG3DSceneEntityContainer* pCurSection = NULL;
	KG3DSceneEntityContainer* pCurZone = NULL;
	for (INT iRegion = 0; iRegion < RegionData.m_nStep; ++iRegion)
	{
		D3DXVECTOR2 vRegionIndex;
		if (iRegion % 2 == 1)
		{
			vRegionIndex = RegionData.m_vStartIndex 
				+ (FLOAT)((iRegion-1)/2) * (RegionData.m_vIndexStep1+RegionData.m_vIndexStep2)
				+ RegionData.m_vIndexStep1;	
		}
		else
		{
			vRegionIndex = RegionData.m_vStartIndex 
				+ (FLOAT)(iRegion/2) * (RegionData.m_vIndexStep1+RegionData.m_vIndexStep2);
		}

		pCurRegion = this->GetRegion(GetSceneRegionIndex(vRegionIndex.x * SCENEREGIONWIDTH)
				, GetSceneRegionIndex(vRegionIndex.y* SCENEREGIONWIDTH));
		
		//由Region的位置逆推出Section和Zone，如果不同就放入pCurXX中
		{
			D3DXVECTOR2 vPos = vRegionIndex * (FLOAT)SCENEREGIONWIDTH;
			INT nSectionX = GetSceneSectionIndex(vPos.x);
			INT nSectionY = GetSceneSectionIndex(vPos.y);
			//检查Section
			if (nSectionX != nLastSectionX || nSectionY != nLastSectionY)
			{
				bBreakRegionInTheSameSection = false;

				nLastSectionX = nSectionX;
				nLastSectionY = nSectionY;

				pCurSection = this->GetSection(nSectionX, nSectionY);

				//检查Zone
				INT nZoneX = GetSceneZoneIndex(vPos.x);
				INT nZoneY = GetSceneZoneIndex(vPos.y);

				if (nZoneX != nLastZoneX || nZoneY != nLastZoneY)
				{
					bBreakSectionInTheSameZone = false;

					nLastZoneX = nZoneX;
					nLastZoneY = nZoneY;

					pCurZone = this->GetZone(nZoneX, nZoneY);

				}
			}
		}
		
		//访问Region，Section，Zone，因为大小的原因，Region的频率会比较高的
		{
			if (NULL != pCurRegion && pLastRegionVisited != pCurRegion && ! bBreakRegionInTheSameSection)
			{
				pLastRegionVisited = pCurRegion;

				HRESULT uCode = visitor.Accept(*pCurRegion);
				KG_COM_PROCESS_ERROR(uCode);
				if (uCode == EM_VISITOR_BREAK)
				{
					bBreakRegionInTheSameSection = true;
				}
			}

			if (NULL != pCurSection && pLastSectionVisited != pCurSection && ! bBreakSectionInTheSameZone)
			{
				pLastSectionVisited = pCurSection;

				HRESULT uCode = visitor.Accept(*pCurSection);
				KG_COM_PROCESS_ERROR(uCode);
				if (uCode == EM_VISITOR_BREAK)
				{
					bBreakSectionInTheSameZone = true;
				}
			}

			if (NULL != pCurZone && pLastZoneVisited != pCurZone)
			{
				pLastZoneVisited = pCurZone;

				HRESULT uCode = visitor.Accept(*pCurZone);
				KG_COM_PROCESS_ERROR(uCode);
				if (uCode == EM_VISITOR_BREAK)
				{
					goto Exit1;//Zone都Break那就跳出了
				}
			}
		}
		
	}//for
Exit0:
Exit1:
	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::Traverse( KG3DEntityCtVisitor& visitor, FLOAT fLeft, FLOAT fBottom, FLOAT fRight, FLOAT fUp )
{
	HRESULT hr = E_FAIL;
	{
		INT nEndX = GetSceneRegionIndex(fRight) + 1;
		INT nEndZ = GetSceneRegionIndex(fUp) + 1;
		for (INT nX = GetSceneRegionIndex(fLeft); nX < nEndX; ++nX)
		{
			for (INT nZ = GetSceneRegionIndex(fBottom); nZ < nEndZ; ++nZ)
			{
				KG3DSceneEntityContainer* p = this->GetRegion(nX, nZ);
				if(NULL != p)
				{
					hr = visitor.Accept(*p);
					KG_COM_PROCESS_ERROR(hr);
				}
			}
		}
	}
	{
		INT nEndX = GetSceneSectionIndex(fRight) + 1;
		INT nEndZ = GetSceneSectionIndex(fUp) + 1;
		for (INT nX = GetSceneSectionIndex(fLeft); nX < nEndX; ++nX)
		{
			for (INT nZ = GetSceneSectionIndex(fBottom); nZ < nEndZ; ++nZ)
			{
				KG3DSceneEntityContainer* p = this->GetSection(nX, nZ);
				if(NULL != p)
				{
					hr = visitor.Accept(*p);
					KG_COM_PROCESS_ERROR(hr);
				}
			}
		}
	}
	{
		INT nEndX = GetSceneZoneIndex(fRight) + 1;
		INT nEndZ = GetSceneZoneIndex(fUp) + 1;
		for (INT nX = GetSceneZoneIndex(fLeft); nX < nEndX; ++nX)
		{
			for (INT nZ = GetSceneZoneIndex(fBottom); nZ < nEndZ; ++nZ)
			{
				KG3DSceneEntityContainer* p = this->GetZone(nX, nZ);
				if(NULL != p)
				{
					hr = visitor.Accept(*p);
					KG_COM_PROCESS_ERROR(hr);
				}
			}
		}
	}
Exit0:
	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::Traverse( KG3DEntityCtVisitor& visitor, const KG3DFrustum& Frustum )
{
	//把Frumstrum扫过的平面区域算出来，然后在这个平面区域上遍历
	INT nXMin = 0, nXMax = 0
		, nZMin = 0, nZMax = 0;
	{
		FLOAT fMaxX = -FLT_MAX, fMaxZ = -FLT_MAX
			, fMinX = FLT_MAX, fMinZ = FLT_MAX;
		for (INT i = 0; i < Frustum.GetPointCount(); ++i)
		{
			const D3DXVECTOR3& PT = Frustum.GetPoint(i);
			if(PT.x < fMinX)
				fMinX = PT.x;
			if(PT.x > fMaxX)
				fMaxX = PT.x;
			if(PT.z < fMinZ)
				fMinZ = PT.z;
			if(PT.z > fMaxZ)
				fMaxZ = PT.z;
		}

		nXMin = GetSceneZoneIndex(fMinX);
		nXMax = GetSceneZoneIndex(fMaxX);
		nZMin = GetSceneZoneIndex(fMinZ);
		nZMax = GetSceneZoneIndex(fMaxZ);
	}

	_ASSERTE(nXMin <= nXMax && nZMin <= nZMax);
	for(INT ZoneZ = nZMin; ZoneZ <= nZMax; ++ZoneZ)
	{
		for (INT ZoneX = nXMin; ZoneX <= nXMax; ++ZoneX)
		{
			KG3DSceneZone* pZone = GetZone(ZoneX, ZoneZ);
			if(NULL == pZone)
				continue;

			BOOL bNeedTraverse = pZone->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH
				&& Frustum.IsAABBoxVisible(pZone->GetAABBox());
			if(! bNeedTraverse)
				continue;

			struct Visitor : public KG3DEntityCtVisitor
			{
				const KG3DFrustum* m_pFrustrum;
				KG3DEntityCtVisitor* m_pOuterVisiter;
				virtual HRESULT Accept(KG3DSceneEntityContainer& Container)
				{
					if (Container.GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH
						&& m_pFrustrum->IsAABBoxVisible(Container.GetAABBox()))
					{
						return m_pOuterVisiter->Accept(Container);
					}
					return EM_VISITOR_CONTINUE;
				}
			};
			//遍历Zone下面的Section和Region
			Visitor visitorInner;
			visitorInner.m_pFrustrum = &Frustum;
			visitorInner.m_pOuterVisiter = &visitor;
			HRESULT hr = pZone->TraverseSubContainer(visitorInner);
			KG_COM_PROCESS_ERROR(hr);
			if(hr == EM_VISITOR_BREAK)
				goto Exit1;

			//深度优先，所以最后再遍历Zone
			hr = visitor.Accept(*pZone);
			KG_COM_PROCESS_ERROR(hr);
			if(hr == EM_VISITOR_BREAK)
				goto Exit1;
		}
	}
Exit1:
Exit0:
	return S_OK;	
}



HRESULT KG3DSceneOutDoorSpaceMgr::GetTerrainRect(D3DXVECTOR2& vA,D3DXVECTOR2& vC)
{
	vA.x = (float)(m_nZoneStart_X * SCENEZONEWIDTH);
	vA.y = (float)(m_nZoneStart_Z * SCENEZONEWIDTH);
	vC.x = vA.x + m_nNumZone_X * SCENEZONEWIDTH;
	vC.y = vA.y + m_nNumZone_Z * SCENEZONEWIDTH;

	return S_OK;
}


DWORD KG3DSceneOutDoorSpaceMgr::GetDataInterfaceVersion()
{
    if (m_lpDataInterface)
        return m_lpDataInterface->GetVersion();
    return 0;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetEntityParent(UINT uHandle, UINT uParent)
{
    if (!m_lpDataInterface)
        return E_FAIL;

    return m_lpDataInterface->SetEntityParent(uHandle, uParent);
}

UINT KG3DSceneOutDoorSpaceMgr::GetEntityParent(UINT uHandle)
{
    if (!m_lpDataInterface)
        return INVALID_OBJSHELL_HANDLE;

    return m_lpDataInterface->GetEntityParent(uHandle);
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetDetailTextureUV(INT nIndex,FLOAT* pfScaleU,FLOAT* pfScaleV,FLOAT* pfEnv,FLOAT* pfSpecular,FLOAT* pfEmissive)
{
	return m_lpDetailMtls->GetDetailTextureUV(nIndex,pfScaleU,pfScaleV,pfEnv,pfSpecular,pfEmissive);
}
HRESULT KG3DSceneOutDoorSpaceMgr::SetDetailTextureUV(INT nIndex,FLOAT m_fScaleU,FLOAT m_fScaleV,FLOAT   fEnv,FLOAT   fSpecular,FLOAT   fEmissive)
{
	return m_lpDetailMtls->SetDetailTextureUV(nIndex,m_fScaleU,m_fScaleV,fEnv,fSpecular,fEmissive);
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetDetailTextureCoordIndex(INT nIndex,BYTE& byCoord)
{
	return m_lpDetailMtls->GetDetailTextureCoordIndex(nIndex,byCoord);
}
HRESULT KG3DSceneOutDoorSpaceMgr::SetDetailTextureCoordIndex(INT nIndex,BYTE byCoord)
{
	return m_lpDetailMtls->SetDetailTextureCoordIndex(nIndex,byCoord);
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex)
{
	return m_lpDetailMtls->GetDetailTextureGrassData(byIndex,pbGrass,pbyGrassTexIndex,pbyPatternIndex);
}
HRESULT KG3DSceneOutDoorSpaceMgr::SetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex,BOOL bSave)
{		
	return m_lpDetailMtls->SetDetailTextureGrassData(byIndex,pbGrass,pbyGrassTexIndex,pbyPatternIndex,bSave);
}
HRESULT KG3DSceneOutDoorSpaceMgr::ChangeDetailTexture(BYTE byIndex,LPCTSTR pFileName)
{
	return m_lpDetailMtls->ChangeDetailTexture(byIndex,pFileName);
}
HRESULT KG3DSceneOutDoorSpaceMgr::AddDetailTextureToMaterialMgr(BYTE& OutIndex,LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	KG3DTerrainDetailTexture* pDetailTexture = NULL;
	hr = m_lpDetailMtls->AddDetailTexture(&pDetailTexture,pFileName);
	KG_COM_PROCESS_ERROR(hr);
	OutIndex = pDetailTexture->m_byteIndex;
Exit0:
	return hr;
}
HRESULT KG3DSceneOutDoorSpaceMgr::DelDetailTextureFromMaterialMgr(BYTE byIndex)
{
	return m_lpDetailMtls->DelDetailTexture(byIndex);
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetAllDetailTextureFromMaterialMgr(map<BYTE,std::string>* pmapDetailTex) 
{
	return m_lpDetailMtls->GetAllDetailTexture(pmapDetailTex);
}

HRESULT KG3DSceneOutDoorSpaceMgr::RequestObjectInformationChange(KG3DRepresentObject* pObject)
{
    if (!pObject)
        return E_FAIL;

	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	if(pFile)
	{
		pObject->SaveToInformationBuffer(pFile);
		DWORD dwInfoLen = pFile->GetLength();
		DWORD dwDataType = pObject->GetType();
		UINT  uHandle = pObject->GetHandleHeldBySpaceMgr();
		m_lpDataInterface->Request_ModifyEntityInformation(uHandle,dwDataType,dwInfoLen,pFile->GetBuffer());

		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
   // m_lpDataInterface->Request_ModifyEntityInformation(pObject);

    return S_OK;
}

void KG3DSceneOutDoorSpaceMgr::CheckReleaseFromPvs(KG3DRepresentObject* obj)
{
    if (!obj)
        return;

    if (obj->GetType() == REPRESENTOBJECT_PVS)
    {
        KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)obj;

        if (pvs->GetParent())
            KG3DSceneSpaceNode::DisConnectTwoNode(pvs->GetParent(), pvs);

        GetScene().UnRegisterSpaceNode(pvs);
    }


    /*
    KG3DRepresentObject* pParent = obj->GetParentObject();

    if (pParent)
    {
        if (pParent->GetType() == REPRESENTOBJECT_PVS)
        {
            KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)pParent;
            pvs->DelObject(obj, &m_Scene);

          //  KG3DSceneEntity e(SCENEENTITY_OBJECT, obj);
          //  pvs->RemoveEntity(e, &GetScene());
        }
    }
    */
    SetEntityParent(obj->GetHandleHeldBySpaceMgr(), INVALID_OBJSHELL_HANDLE);
	obj->LeavePvs();
}

/* this is call by editor when the object have translation */
HRESULT KG3DSceneOutDoorSpaceMgr::RequestObjectTransformationChange(KG3DRepresentObject* pObject)
{
	KG3DEntityTransformation vTrans;

	KGLOG_PROCESS_ERROR(pObject);

    /* first free the obj from pvs */
    CheckReleaseFromPvs(pObject);

	pObject->ComputeBBox();

	vTrans.m_Translation = pObject->m_Translation;
	vTrans.m_Rotation = pObject->m_Rotation;
	vTrans.m_Scaling = pObject->m_Scaling;
	vTrans.m_vBox_A = pObject->m_BBox.A;
	vTrans.m_vBox_B = pObject->m_BBox.B;	
    vTrans.m_vWorldPos = pObject->GetWorldPos();


	if(pObject->GetHandleHeldBySpaceMgr()==-1)
	{
		//m_listObjectShellNoHandle.remove()
		list<KG3DEntityObjectShell*>::iterator i = m_listObjectShellNoHandle.begin();//没有HANDLE的OBJECT对象
		list<KG3DEntityObjectShell*>::iterator iend = m_listObjectShellNoHandle.end();//没有HANDLE的OBJECT对象
		while (i!=iend)
		{
			KG3DEntityObjectShell* pShell =*i;
			if (pShell->m_lpObject == pObject)
			{
				pShell->RemoveFromAllRefrenceContainer();

				pShell->m_vBBox_A = vTrans.m_vBox_A;
				pShell->m_vBBox_B = vTrans.m_vBox_B;
				pShell->m_vPosition = vTrans.m_vWorldPos;
				pShell->m_fDiameter = max(700.0F,D3DXVec3Length(&(vTrans.m_vBox_B - vTrans.m_vBox_A)));
				
				InputObjectShell(pShell, TRUE);
			}

			i++;
		}

	}
	else
	{
		m_lpDataInterface->Request_ModifyEntityTransform(pObject->GetHandleHeldBySpaceMgr(),vTrans);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CheckConnectNode(KG3DEntityObjectShell* objShell)
{
    HRESULT hResult = E_FAIL;

    KG3DRepresentObjectPVS* pPvsObj = NULL; 
//    KG3DScene* pScene = NULL;

    KG_PROCESS_ERROR(objShell);
    KG_PROCESS_ERROR(objShell->m_lpObject);

    SetEntityParent(objShell->m_uHandle, INVALID_OBJSHELL_HANDLE);
    KG_PROCESS_ERROR(objShell->m_lpObject->GetType() == REPRESENTOBJECT_PVS);

	{
		static KMutex m_lock;
		m_lock.Lock();

		pPvsObj = (KG3DRepresentObjectPVS*)objShell->m_lpObject;

        CheckReleaseFromPvs(pPvsObj);

		UINT uNewHandle = GetScene().GetOneNewSpaceNodeHandle();
		pPvsObj->SetHandle(uNewHandle);
		KG3DSceneSpaceNode::ConnectTwoNodes(this, pPvsObj);
		GetScene().RegisterSpaceNode(pPvsObj);
		m_lock.Unlock();
	}

    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DSceneOutDoorSpaceMgr::FindUnLoadedObjectShellInRect(set<KG3DEntityObjectShell*>*psetShells,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	KGLOG_PROCESS_ERROR(psetShells);

	psetShells->clear();

	{
		vector<KG3DSceneZone*>vecZone;
		SelectZone(&vecZone,A,C);
		for (size_t i=0;i<vecZone.size();i++)
		{
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
			KG3DSceneZone* pZone = vecZone[i];

			pZone->m_Data.FindUnLoadedObjectShellInRect(psetShells);		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		vector<KG3DSceneSection*>vecSection;
		SelectSection(&vecSection,A,C);
		for (size_t i=0;i<vecSection.size();i++)
		{
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
			KG3DSceneSection* pSection = vecSection[i];

			pSection->m_Data.FindUnLoadedObjectShellInRect(psetShells);	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		vector<KG3DSceneRegion*>vecRegion;
		SelectRegion(&vecRegion,A,C);
		for (size_t i=0;i<vecRegion.size();i++)
		{
			KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
			KG3DSceneRegion* pRegion = vecRegion[i];

			pRegion->m_Data.FindUnLoadedObjectShellInRect(psetShells);
		}
	}
	KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
	//////////////////////////////////////////////////////////////////////////
	{
		m_vErrorRegion.m_Data.FindUnLoadedObjectShellInRect(psetShells);
	}
Exit1:
Exit0:
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::FindSoundBall(D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int XStartZoneIndex = 0;
	int XEndZoneIndex = 0;
	int ZStartZoneIndex =0;
	int ZEndZoneIndex = 0;
	int X,Z;
	KG3DSceneZone* pZone = NULL;

	//ClearSoundBallSet();

	//////////////////////////////////////////////////////////////////////////

	XStartZoneIndex = GetZoneIndex(A.x);
	ZStartZoneIndex = GetZoneIndex(A.y);
	XEndZoneIndex   = GetZoneIndex(C.x);
	ZEndZoneIndex   = GetZoneIndex(C.y);

	//////////////////////////////////////////////////////////////////////////
	struct Visitor : public KG3DRepresentObjectVisitor
	{
		DWORD m_dwType;
		std::set<KG3DRepresentObjectSoundBall*>*m_pSet;
		virtual HRESULT Accept(KG3DRepresentObject& Obj)
		{
			if(Obj.GetType() == m_dwType)
			{
				KG3DRepresentObjectSoundBall* pSoundBall = reinterpret_cast<KG3DRepresentObjectSoundBall*>( &Obj );

				pair<std::set<KG3DRepresentObjectSoundBall*>::iterator, bool> InsertPari = m_pSet->insert(pSoundBall);
				if (InsertPari.second)
				{
					//如果之前不没有insert过那么需要AddRef
					pSoundBall->AddRef();
				}
			}
			return S_OK;
		}
	}visitor;
    visitor.m_dwType = REPRESENTOBJECT_SOUNDBALL;
	visitor.m_pSet = &m_setObjectSoundBall;
	//////////////////////////////////////////////////////////////////////////
	for (Z = ZStartZoneIndex; Z <= ZEndZoneIndex; Z++)
	{
		for (X = XStartZoneIndex; X <= XEndZoneIndex; X++)
		{
			pZone = GetZone(X,Z);
			if(pZone)
			{
				pZone->FindSoundBall(visitor,A,C);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//{
	//	set<KG3DRepresentObjectSoundBall*>::iterator i = m_setObjectSoundBall.begin();
	//	set<KG3DRepresentObjectSoundBall*>::iterator iend = m_setObjectSoundBall.end();
	//	while (i!=iend)
	//	{
	//		KG3DRepresentObjectSoundBall* pSoundBall = *i;
	//		if(pSoundBall)
	//			pSoundBall->AddRef();
	//		i++;
	//	}
	//}
		
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::ProcessSoundBallMultiThread()
{
	D3DXVECTOR3 vSpeed(0,0,0);
	D3DXVECTOR3 vTop(0, 1, 0);

	set<KG3DRepresentObjectSoundBall*>::iterator i = m_setObjectSoundBall.begin();
	set<KG3DRepresentObjectSoundBall*>::iterator iend = m_setObjectSoundBall.end();
	while (i != iend)
	{
		KG3DRepresentObjectSoundBall* pSound = *i;
		pSound->FrameMove(m_vCameraView);
		i++;
	}

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::ProcessSoundBall(KG3DCamera* pCamera)
{
	pCamera->GetPosition(&m_vCameraPosition);
	m_vCameraFront = pCamera->GetCameraFront();
	m_vCameraLeft = pCamera->GetCameraRight();
	m_vCameraView = pCamera->GetViewMatrix();
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RequestCreateSceneBlock(int nLevel,list<POINT>*plistBlock,BOOL bCreateTerrain)
{
	KGLOG_PROCESS_ERROR(plistBlock);
	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	HRESULT hr = E_FAIL;
	{
		list<POINT>::iterator i = plistBlock->begin();
		list<POINT>::iterator iend = plistBlock->end();
		while (i!=iend)
		{
			POINT Index = *i;

			BOOL bBlockExist = m_lpDataInterface->GetSceneBlock(nLevel,Index);
			if(!bBlockExist)
			{
				hr = m_lpDataInterface->Request_CreateSceneBlock(nLevel,Index);
				KGLOG_COM_PROCESS_ERROR(hr);
			}

			if (bCreateTerrain)
			{
				hr = m_lpDataInterface->Request_CreateTerrainBlock(nLevel,Index);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
			i++;
		}
	}

	hr = S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RequestDeleteSceneBlock(int nLevel,list<POINT>*plistBlock,BOOL bDeleteTerrainOnly)
{
	KGLOG_PROCESS_ERROR(plistBlock);
	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	HRESULT hr = E_FAIL;
	{
		list<POINT>::iterator i = plistBlock->begin();
		list<POINT>::iterator iend = plistBlock->end();
		while (i!=iend)
		{
			POINT Index = *i;

			BOOL bBlockExist = m_lpDataInterface->GetSceneBlock(nLevel,Index);
			if(bBlockExist)
			{
				if (bDeleteTerrainOnly)
				{
					if( m_lpDataInterface->GetTerrainBlock(nLevel,Index))
					{
						hr = m_lpDataInterface->Request_DeleteTerrainBlock(nLevel,Index);
						KGLOG_COM_PROCESS_ERROR(hr);
					}
				}
				else
				{
					hr = m_lpDataInterface->Request_DeleteSceneBlock(nLevel,Index);
					KGLOG_COM_PROCESS_ERROR(hr);
				}
			}

			++i;
		}
	}
	hr = S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DSceneOutDoorSpaceMgr::GetEntityIntersectRect(KG3DSceneEntityList*plistEntity,D3DXVECTOR2& A,D3DXVECTOR2& C,BOOL bShellMustLoaded)
{
	{
		vector<KG3DSceneZone*>vecZone;
		SelectZone(&vecZone,A,C);
		for (size_t i=0;i<vecZone.size();++i)
		{
			KG3DSceneZone* pZone = vecZone[i];
			pZone->m_Data.GetEntityIntersectRect(plistEntity,A,C,bShellMustLoaded);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		vector<KG3DSceneSection*>vecSection;
		SelectSection(&vecSection,A,C);
		for (size_t i=0;i<vecSection.size();++i)
		{
			KG3DSceneSection* pSection = vecSection[i];
			pSection->m_Data.GetEntityIntersectRect(plistEntity,A,C,bShellMustLoaded);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		vector<KG3DSceneRegion*>vecRegion;
		SelectRegion(&vecRegion,A,C);
		for (size_t i=0;i<vecRegion.size();++i)
		{
			KG3DSceneRegion* pRegion = vecRegion[i];
			pRegion->m_Data.GetEntityIntersectRect(plistEntity,A,C,bShellMustLoaded);	
		}
	}
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetSceneRect(int& nX_Start,int& nZ_Start,int& nX_Width,int& nZ_Width)
{
	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	{
		D3DXVECTOR2 A,C;
		m_lpDataInterface->GetSceneRect(A,C);


		nX_Start = (int)(A.x/100);
		nZ_Start = (int)(A.y/100);
		nX_Width = (int)((C.x - A.x)/100);
		nZ_Width = (int)((C.y - A.y)/100);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetSceneRect( D3DXVECTOR2* pLowerLeft, D3DXVECTOR2* pUpperRight )
{
	if(NULL != m_lpDataInterface && NULL != pLowerLeft && NULL != pUpperRight)
	{
		return m_lpDataInterface->GetSceneRect(*pLowerLeft, *pUpperRight);
	}
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::ForceLoadAllTerrainBlock()
{
	D3DXVECTOR2 A,C;

	KGLOG_PROCESS_ERROR(m_lpDataInterface);
	{
		m_lpDataInterface->GetSceneRect(A,C);
		m_lpSceneBlockLoader->SetLoadRect(A,C,A,C,A,C,FALSE);
		m_lpSceneBlockLoader->ForceLoad();
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::ForceLoadTerrainBlock(D3DXVECTOR2 A, D3DXVECTOR2 C)
{
	m_lpSceneBlockLoader->SetLoadRect(A,C,A,C,A,C,FALSE);
	m_lpSceneBlockLoader->ForceLoad();
	return S_OK;

}
HRESULT KG3DSceneOutDoorSpaceMgr::ForceLoadEntity(D3DXVECTOR2 A, D3DXVECTOR2 C)
{
	m_lpSceneBlockLoader->SetLoadRect(A,C,A,C,A,C,FALSE);
	m_lpSceneBlockLoader->ForceLoad();
	CheckEntityFroceLoad();
	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::ForceLoadAllEntity()
{
	D3DXVECTOR2 A,C;

	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	{
		m_lpDataInterface->GetSceneRect(A,C);
		m_lpSceneBlockLoader->SetLoadRect(A,C,A,C,A,C,FALSE);
		m_lpSceneBlockLoader->ForceLoad();
		CheckEntityFroceLoad();
	}

	return S_OK;
Exit0:
	return E_FAIL;
}
void KG3DSceneOutDoorSpaceMgr::CheckEntityFroceLoad()
{
	//<加载计数>
	static KG3DMessageSceneLoadingPercent PercentStruct;
	ZeroMemory(&PercentStruct, sizeof(PercentStruct));
	PercentStruct.TerrainLoadingPercent = 1;
	PercentStruct.ObjectLoadingPercent = 1;

	size_t TotalRestLoadDemand = m_mapObjectShell.GetSize();
	size_t CurRestLoaded = 0;
	enum{em_send_message_step = 10};
	KG3DMessageSender& Sender =this->GetSceneMessageSender(KM_LOADING_PERCENT);
	KG3DMessage MessageToSend(KM_SCENE_LOADING_PERCENT, (DWORD_PTR)&PercentStruct, NULL);
	BOOL bSentPercent = Sender.GetReceiverCount() > 0;
	//</加载计数>

	static KG3DEntityObjectShell*  sShells[40960];
	DWORD dwCount = 0;
	m_mapObjectShell.GetEntityShell(sShells,dwCount,40960);

	if(!g_bClient)
	{
		TCHAR szMessage[MAX_PATH];
		wsprintf(szMessage,"Totoal Entity %d Loaded",dwCount);

		typedef int (__stdcall *MESSAGEBOXTIMEROUTA)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
		HMODULE hUser32 = GetModuleHandle("user32.dll");
		MESSAGEBOXTIMEROUTA MessageBoxTimeoutA = NULL;
		BOOL bGotAPI = FALSE;
		if (hUser32)
		{
			MessageBoxTimeoutA = (MESSAGEBOXTIMEROUTA)GetProcAddress(hUser32, "MessageBoxTimeoutA");
			if (MessageBoxTimeoutA)
				bGotAPI = TRUE;
		}

		if (bGotAPI && g_bDesign)
			MessageBoxTimeoutA(NULL, szMessage, "Force Load Complete!", 0, 0, 5900);
		else
			MessageBox(NULL,szMessage,"Force Load Complete!",0);
	}

	for (DWORD i=0;i<dwCount;i++)
	{
		KG3DEntityObjectShell* pShell = sShells[i];

		KG3DRepresentObject *pObject = pShell->m_lpObject;
		if (!pObject)
		{
			pShell->TryDynamicLoad(0);
		}
		if(pObject)
		{
			int nModelNum = pObject->GetModelCount();
			for(int i = 0 ;i< nModelNum;i++ )
			{
				KG3DModel *pModel = NULL;
				HRESULT hResult = pObject->GetModel(pModel, i,FALSE);
				if (SUCCEEDED(hResult))
				{
					g_nNumTextureDynamicLoadedFrame = 0;
					ASSERT(pModel);
					//如果mesh加载在Finish但是没有做postload处理， 这次调用就会做处理
					//ASSERT(pModel->IsResourceReady());
					if (!pModel->IsResourceReady(FALSE))
					{
						//如果还是没有完成， 说明模型阻塞在动态加载队列中， 强制使用单线程加载
						pModel->FrameMove();//这里有少数会失败，好像是因为要FrameMove一次之后，才处理，上面直接这样好了
						
						ASSERT(pModel->IsResourceReady(FALSE));
					}
				}
			}
		
			pObject->FrameMove();
			pObject->ComputeBBox();
			pShell->m_vBBox_A = pObject->m_BBox.A;
			pShell->m_vBBox_B = pObject->m_BBox.B;
			
			pShell->RemoveFromAllRefrenceContainer();
			InputObjectShell(pShell,TRUE);
		}


		if (bSentPercent)
		{
			++CurRestLoaded;
			if (CurRestLoaded % em_send_message_step == 0)
			{
				PercentStruct.RestLoadingPercent = (FLOAT)CurRestLoaded / (FLOAT)TotalRestLoadDemand;
				Sender.PostMessage(MessageToSend);
			}

		}

	}
}



/////////道路//////////////////////////
HRESULT KG3DSceneOutDoorSpaceMgr::GetTerrainRoadMgr(IEKG3DTerrainRoadMgr** ppRoadMgr)
{
	KG_PROCESS_ERROR(m_lpRoadMgr);

	(*ppRoadMgr) = (IEKG3DTerrainRoadMgr*)m_lpRoadMgr;

	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetVisibleRoad(list<KG3DTerrainRoad*>* plistRoad,const KG3DFrustum* pFrustum)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpRoadMgr);
	KGLOG_PROCESS_ERROR(pFrustum);
//	m_lpRoadMgr->GetVisibleRoad(plistRoad,pFrustum);	
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSceneOutDoorSpaceMgr::EraseRoad(KG3DTerrainRoad* pRoad)
{
	KG_PROCESS_ERROR(pRoad);
	for(DWORD i=0; i< pRoad->m_vecParentTerrainBlock.size();i++)
	{
		int nLevel = pRoad->m_vecParentTerrainBlock[i].nLevel;
		POINT Index = pRoad->m_vecParentTerrainBlock[i].Index;
		switch(nLevel)
		{
		case BLOCK_TYPE_SECTION:
			{
				KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
				if(pSection && pSection->m_lpTerrainBlock)
					pSection->m_lpTerrainBlock->EraseRoad(pRoad->m_uHandle,m_lpDataInterface);
			}
			break;
		case BLOCK_TYPE_ZONE:
			{
				KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
				if(pZone && pZone->m_lpTerrainBlock)
					pZone->m_lpTerrainBlock->EraseRoad(pRoad->m_uHandle,m_lpDataInterface);
			}
			break;
		}
	}
	pRoad->m_vecParentTerrainBlock.clear();
Exit0:
	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::InputRoad(KG3DTerrainRoad* pRoad)
{
	D3DXVECTOR2 A,C;
	D3DXVECTOR3 vPos;
	vector<KG3DSceneZone*> vecZone;
	vector<KG3DSceneSection*> vecSection;
	KG_PROCESS_ERROR(pRoad);
	EraseRoad(pRoad);
	pRoad->m_BBox.GetXZRect(A,C);
	vPos = pRoad->m_BBox.GetCenter();
	float width = C.x - A.x;
	float Height = C.y - A.y;

	if ((width > SCENESECTIONWIDTH*4) || (Height > SCENESECTIONWIDTH)*4)
	{
		SelectZone(&vecZone,A,C);
		for(int nZone = 0; nZone < (int)vecZone.size();nZone++)
		{
			KG3DSceneZone* pZone =vecZone[nZone];
			if(pZone && pZone->m_lpTerrainBlock)
			{
				pZone->m_lpTerrainBlock->InputRoad(pRoad,m_lpDataInterface);
			}
		}
	}
	else
	{
		SelectSection(&vecSection,A,C);
		for(int nSection = 0; nSection < (int)vecSection.size();nSection++)
		{
			KG3DSceneSection* pSection =vecSection[nSection];
			if(pSection && pSection->m_lpTerrainBlock)
			{
				pSection->m_lpTerrainBlock->InputRoad(pRoad,m_lpDataInterface);
			}
		}		
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RefreshRoadRiver(BOOL bRefreshRoad,BOOL bRefreshRiver)
{
	if( bRefreshRoad && m_lpRoadMgr)
		m_lpRoadMgr->RefreshCurSerpentine();
	if (bRefreshRiver && m_lpRiverMgr)
		m_lpRiverMgr->RefreshCurSerpentine();
	return S_OK;
}

/////////河流//////////////////////////
HRESULT KG3DSceneOutDoorSpaceMgr::GetTerrainRiverMgr(IEKG3DTerrainRiverMgr** ppRiverMgr)
{
	KG_PROCESS_ERROR(m_lpRiverMgr);

	(*ppRiverMgr) = (IEKG3DTerrainRiverMgr*)m_lpRiverMgr;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::EraseRiver(KG3DTerrainRiver* pRiver)
{
	KG_PROCESS_ERROR(pRiver);
	for(DWORD i=0; i< pRiver->m_vecParentTerrainBlock.size();i++)
	{
		int nLevel = pRiver->m_vecParentTerrainBlock[i].nLevel;
		POINT Index = pRiver->m_vecParentTerrainBlock[i].Index;
		switch(nLevel)
		{
		case BLOCK_TYPE_SECTION:
			{
				KG3DSceneSection* pSection = GetSection(Index.x,Index.y);
				if(pSection && pSection->m_lpTerrainBlock)
					pSection->m_lpTerrainBlock->EraseRiver(pRiver->m_uHandle,m_lpDataInterface);
			}
			break;
		case BLOCK_TYPE_ZONE:
			{
				KG3DSceneZone* pZone = GetZone(Index.x,Index.y);
				if(pZone && pZone->m_lpTerrainBlock)
					pZone->m_lpTerrainBlock->EraseRiver(pRiver->m_uHandle,m_lpDataInterface);
			}
			break;
		}
	}
	pRiver->m_vecParentTerrainBlock.clear();
Exit0:
	return S_OK;
}
HRESULT KG3DSceneOutDoorSpaceMgr::InputRiver(KG3DTerrainRiver* pRiver)
{
	D3DXVECTOR2 A,C;
	D3DXVECTOR3 vPos;
	vector<KG3DSceneZone*> vecZone;
	vector<KG3DSceneSection*> vecSection;
	KG_PROCESS_ERROR(pRiver);
	EraseRiver(pRiver);
	pRiver->m_BBox.GetXZRect(A,C);
	vPos = pRiver->m_BBox.GetCenter();
	float width = C.x - A.x;
	float Height = C.y - A.y;

	if ((width > SCENESECTIONWIDTH*4) || (Height > SCENESECTIONWIDTH*4))
	{
		SelectZone(&vecZone,A,C);
		for(int nZone = 0; nZone < (int)vecZone.size();nZone++)
		{
			KG3DSceneZone* pZone =vecZone[nZone];
			if(pZone && pZone->m_lpTerrainBlock)
			{
				pZone->m_lpTerrainBlock->InputRiver(pRiver,m_lpDataInterface);;
			}
		}
	}
	else
	{
		SelectSection(&vecSection,A,C);
		for(int nSection = 0; nSection < (int)vecSection.size();nSection++)
		{
			KG3DSceneSection* pSection =vecSection[nSection];
			if(pSection && pSection->m_lpTerrainBlock)
			{
				pSection->m_lpTerrainBlock->InputRiver(pRiver,m_lpDataInterface);
			}
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//地形高度图导出导入相关
HRESULT KG3DSceneOutDoorSpaceMgr::ExprotTerrainHeightMap(LPCSTR pFileName,DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	KG3DHeightMap vHeightMap;

	KGLOG_PROCESS_ERROR(pFileName);
	KGLOG_PROCESS_ERROR(m_lpConverMap);

	if(m_lpConverMap->m_dwHeightMapSize == 0)
		m_lpConverMap->ComputeHeightMapSize();

	ForceLoadAllTerrainBlock();

	DWORD dwSize = m_lpConverMap->m_dwHeightMapSize;
	hr = vHeightMap.Init(dwSize,dwSize);
	KGLOG_COM_PROCESS_ERROR(hr);

	vHeightMap.SetHeightScale(0,1);

	for (DWORD Z=0;Z<dwSize;Z++)
	{
		for (DWORD X=0;X<dwSize;X++)
		{
			float xx = X * 100 + m_lpConverMap->GetRect().x;
			float zz = Z * 100 + m_lpConverMap->GetRect().y;

			float H = this->GetGroundHeight(xx,zz);
			float S = (H - m_lpConverMap->m_fLowest) / m_lpConverMap->m_fHeightTotal;

			float* p = vHeightMap.GetVertexPointer(X,Z);
			(*p) = S;
		}
	}

	hr = vHeightMap.SaveToR32File(dwSize,dwSize,pFileName);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::ImportTerrainHeightMap(LPCSTR pFileName,DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	KG3DHeightMap vHeightMap;

	KGLOG_PROCESS_ERROR(pFileName);
	KGLOG_PROCESS_ERROR(m_lpConverMap);

	if(m_lpConverMap->m_dwHeightMapSize == 0)
		m_lpConverMap->ComputeHeightMapSize();

	DWORD dwSize = m_lpConverMap->m_dwHeightMapSize;


	hr = vHeightMap.CreateFromR32File(dwSize,dwSize,pFileName);
	KGLOG_COM_PROCESS_ERROR(hr);

	vHeightMap.SetHeightScale(m_lpConverMap->m_fLowest,m_lpConverMap->m_fHeightTotal);
	vHeightMap.ComputeNormal();
	//vHeightMap.TranslateAbsoluteData();
	{
		D3DXVECTOR2 A(m_lpConverMap->GetRect().x,m_lpConverMap->GetRect().y);
		D3DXVECTOR2 C = A + D3DXVECTOR2(dwSize*100.0F,dwSize*100.0F);

		SetRegionHeight(&vHeightMap,A,C);
		SetSectionHeight(&vHeightMap,A,C);
		SetZoneHeight(&vHeightMap,A,C);
		
		
	}
	
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneOutDoorSpaceMgr::ApplyBaseMapToSelectedSceneBlock(LPTSTR pBaseMap)
{
	HRESULT hr = E_FAIL;
	DWORD dwLength = 0;
	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	KGLOG_PROCESS_ERROR(pFile);
	KGLOG_PROCESS_ERROR(m_lpConverMap);
	KGLOG_PROCESS_ERROR(pBaseMap);
	KGLOG_PROCESS_ERROR(m_lpDataInterface);

	m_lpConverMap->m_szTextureFileName = pBaseMap;

	{
		/*if(m_lpConverMap->m_dwHeightMapSize == 0)
			m_lpConverMap->ComputeHeightMapSize();

		DWORD dwSize = m_lpConverMap->m_dwHeightMapSize;
		m_lpConverMap->m_vAC.z = dwSize * 100.0F;
		m_lpConverMap->m_vAC.w = dwSize * 100.0F;*/

		D3DXVECTOR2 A,C;
		m_lpDataInterface->GetSceneRect(A,C);

		D3DXVECTOR4 vRectVector = m_lpConverMap->GetRect();
		/*m_lpConverMap->m_vRect.z = C.x - A.x;
		m_lpConverMap->m_vRect.w = (C.y - A.y)*-1;*/
		m_lpConverMap->SetRectVector(vRectVector.x, vRectVector.y, C.x - A.x, (C.y - A.y)*(-1));
	}
	hr = m_lpConverMap->SaveToInformationBuffer(dwLength,pFile);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(m_uConvermapHandle == 0)
	{
		hr = m_lpDataInterface->Request_CreateNewTerrainInfo    (m_uConvermapHandle,Terrain_Info_Convermap,dwLength,pFile->GetBuffer());
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		hr = m_lpDataInterface->Request_ModifyTerrainInformation(m_uConvermapHandle,Terrain_Info_Convermap,dwLength,pFile->GetBuffer());
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = S_OK;
Exit0:
	if(pFile)
	{
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetSceneHeightInfo(int* pLowest,int* pHeightTotal)
{
	if(m_lpConverMap)
	{
		(*pLowest) = (int)(m_lpConverMap->m_fLowest);
		(*pHeightTotal) = (int)(m_lpConverMap->m_fHeightTotal);
	}
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::SetSceneHeightInfo(int  nLowest,int  nHeightTotal)
{
	if(m_lpConverMap)
	{
		m_lpConverMap->m_fLowest = (float)nLowest;
		m_lpConverMap->m_fHeightTotal = (float)nHeightTotal;
	}
	return S_OK;
}

////////////////////liumingod 地形复制函数
HRESULT KG3DSceneOutDoorSpaceMgr::CopySelectedTerrain(D3DXVECTOR3 vA,D3DXVECTOR3 vC)
{
	D3DXVECTOR2 A,C;
	HRESULT hr =  E_FAIL;
	float cell = 100;
	int S = 0;
	int nXStart = (int)(vA.x/cell);
	int nXEnd = (int)(vC.x/cell);
	int nZStart = (int)(vA.z/cell);
	int nZEnd = (int)(vC.z/cell);
	float fAllHeight = 0;

	int nNumTexture = 0;
	map<BYTE,std::string> mapDetailTex;
	int byteIndexTable[256];

	m_nTerrainCopyWidth = nXEnd - nXStart;
	m_nTerrainCopyHeight = nZEnd - nZStart;

	//记录开始位置
	m_vecCopyPos.x = vA.x;
	m_vecCopyPos.y = 0.0f;
	m_vecCopyPos.z = vA.z;

	KGLOG_PROCESS_ERROR(m_nTerrainCopyWidth > 0);
	KGLOG_PROCESS_ERROR(m_nTerrainCopyHeight > 0);

	SAFE_DELETE_ARRAY(m_lpTerrainCopyHeightData);
	m_lpTerrainCopyHeightData = new float[m_nTerrainCopyWidth * m_nTerrainCopyHeight];
	KGLOG_PROCESS_ERROR(m_lpTerrainCopyHeightData);

	SAFE_DELETE_ARRAY(m_lpTerrainCopyBlendData);
	m_lpTerrainCopyBlendData = new BYTE[m_nTerrainCopyWidth * m_nTerrainCopyHeight];
	KGLOG_PROCESS_ERROR(m_lpTerrainCopyBlendData);

	SAFE_DELETE_ARRAY(m_pRealTextureIndex);
	for (int i = 0; i < m_nNumTerrainCopyTexture; i++)
	{
		SAFE_DELETE_ARRAY(m_ppTerrainCopyTexturePath[i]);
	}
	SAFE_DELETE_ARRAY(m_ppTerrainCopyTexturePath);
	
	m_pRealTextureIndex = new BYTE[256];
	ZeroMemory(m_pRealTextureIndex,sizeof(BYTE) * 256);
	m_ppTerrainCopyTexturePath = new char*[256];
	ZeroMemory(m_ppTerrainCopyTexturePath,sizeof(char*) * 256);
	ZeroMemory(byteIndexTable,sizeof(byteIndexTable));

	GetAllDetailTextureFromMaterialMgr(&mapDetailTex);

	for (int z=nZStart;z<nZEnd;z++)
	{
		for (int x=nXStart;x<nXEnd;x++)
		{
			int nIndex = (z - nZStart)*m_nTerrainCopyWidth + (x - nXStart);
			float xx = x*cell;
			float zz = z*cell;

			m_lpTerrainCopyHeightData[nIndex] = GetGroundHeight(xx,zz);
			//m_lpTerrainCopyBlendData[nIndex] = GetDetailTextureIndex(xx,zz);
			BYTE nTextureIndex = GetDetailTextureIndex(xx,zz);
			if (byteIndexTable[nTextureIndex] == 0)
			{
				byteIndexTable[nTextureIndex] = nNumTexture + 1;//mark!
				m_lpTerrainCopyBlendData[nIndex] = _kg_to_smaller_size_cast<BYTE>(nNumTexture);
				m_pRealTextureIndex[nNumTexture] = nTextureIndex;
				m_ppTerrainCopyTexturePath[nNumTexture] = new char[MAX_PATH];
				strcpy(m_ppTerrainCopyTexturePath[nNumTexture],mapDetailTex[nTextureIndex].c_str());
				nNumTexture++;
			}
			else
			{
				m_lpTerrainCopyBlendData[nIndex] = _kg_to_smaller_size_cast<BYTE>(byteIndexTable[nTextureIndex] - 1);	
			}
			fAllHeight += m_lpTerrainCopyHeightData[nIndex];
			if(S==0)
			{
				m_fTerrainCopyHeightMin = m_fTerrainCopyHeightMax = m_lpTerrainCopyHeightData[nIndex];
			}
			else
			{
				m_fTerrainCopyHeightMin = min(m_fTerrainCopyHeightMin,m_lpTerrainCopyHeightData[nIndex]);
				m_fTerrainCopyHeightMax = max(m_fTerrainCopyHeightMax,m_lpTerrainCopyHeightData[nIndex]);
			}
			S++;
		}
	}
	m_fTerrainCopyHeightAve = fAllHeight / S;
	m_nNumTerrainCopyTexture = nNumTexture;
	InitTerrainCopyBrush();
	hr = S_OK;
Exit0:
	return hr;
}

// 0 Outside 
// 1 Between 
// 2 Inside
int KG3DSceneOutDoorSpaceMgr::PosInWhichArea(D3DXVECTOR3& vPos,float fWidth)
{
	D3DXMATRIX matInvese;
	D3DXVECTOR3 vecPos;
	float cell = 100;
	int nExtendCell = (int)(fWidth / cell);
	D3DXMatrixInverse(&matInvese,NULL,&m_matCopyBrush);
	D3DXVec3TransformCoord(&vecPos,&vPos,&matInvese);

	int nXIndex2 = (int)(( vecPos.x + m_nTerrainCopyWidth * cell * 0.5F)/cell);
	int nZIndex2 = (int)(( vecPos.z + m_nTerrainCopyHeight * cell * 0.5F)/cell);
	if ((nXIndex2 >=0 )&&( nXIndex2 < m_nTerrainCopyWidth)&&
		(nZIndex2 >= 0)&&( nZIndex2 < m_nTerrainCopyHeight))
		return 2;
	else if ((vecPos.x > -(m_nTerrainCopyWidth * cell * 0.5F + nExtendCell * cell)) && (vecPos.x < m_nTerrainCopyWidth * cell * 0.5F + nExtendCell * cell) 
		&& (vecPos.z > -(m_nTerrainCopyHeight * cell * 0.5F + nExtendCell * cell)) && (vecPos.z < m_nTerrainCopyHeight * cell * 0.5F + nExtendCell * cell))
		return 1;
	else 
		return 0;
}

HRESULT KG3DSceneOutDoorSpaceMgr::PasteSelectedTerrain(DWORD dwMixType,bool bIsPasteBlend,bool bIsMax,float fWeight,float fWidth,AABBOX* bBox)
{
	HRESULT hr = E_FAIL;
	//return E_FAIL;
	vector<KG3DSceneRegion*>vecRegion;
	D3DXVECTOR2 A,C;
	D3DXVECTOR2 AA,CC;
	D3DXMATRIX matSave,mat,matT,matR,matS,matInvese;
	AABBOX bbox;
	D3DXVECTOR3 BosPos[8];
	KGLOG_PROCESS_ERROR(m_lpTerrainCopyHeightData);
	D3DXMatrixInverse(&matInvese,NULL,&m_matCopyBrush);

	float cell = 100;
	int nExtendCell = (int)(fWidth / cell);
	A.x = 0 - m_nTerrainCopyWidth * cell*0.5F;
	A.y = 0 - m_nTerrainCopyHeight * cell*0.5F;
	C.x = 0 + m_nTerrainCopyWidth * cell*0.5F;
	C.y = 0 + m_nTerrainCopyHeight * cell*0.5F;
	A.x -= nExtendCell * cell;
	A.y -= nExtendCell * cell;
	C.x += nExtendCell * cell;
	C.y += nExtendCell * cell;

	bbox.Clear();
	bbox.AddPosition(D3DXVECTOR3(A.x,0,A.y));
	bbox.AddPosition(D3DXVECTOR3(C.x,0,C.y));
	bbox.TransformCoord(BosPos,&m_matCopyBrush);
	bbox.Clear();
	bbox.AddPosition(BosPos,8);
	*bBox= bbox;

	int nXStart = (int)(bbox.A.x/cell);
	int nXEnd = (int)(bbox.B.x/cell);
	int nZStart = (int)(bbox.A.z/cell);
	int nZEnd = (int)(bbox.B.z/cell);

	nXStart = max(m_nZoneStart_X * 1024,nXStart);
	nXEnd   = min((m_nZoneStart_X + m_nNumZone_X) * 1024,nXEnd);
	nZStart = max(m_nZoneStart_Z * 1024,nZStart);
	nZEnd   = min((m_nZoneStart_Z + m_nNumZone_Z) * 1024,nZEnd);
	if (bIsPasteBlend)
	{
		D3DXVECTOR2 vec2StartPos = D3DXVECTOR2(nXStart * cell,nZStart * cell);
		D3DXVECTOR2 vec2EndPos = D3DXVECTOR2(nXEnd * cell,nZEnd * cell);
		g_cTerrainBlockModifier.FillDetailTextureIndexMap(vec2StartPos,vec2EndPos,this);
		BYTE* pTextureIndexMap = g_cTerrainBlockModifier.GetDetailTextureIndexMap();
		
		for (int z=nZStart;z<=nZEnd;z++)
		{
			for (int x=nXStart;x<=nXEnd;x++)
			{
				float xx = x*cell;
				float zz = z*cell;

				D3DXVECTOR3 vPos(xx,0,zz);
				D3DXVECTOR3 vPosSrc(0,0,0);
				D3DXVECTOR3 vecPosOut(0,0,0);
				D3DXVECTOR3 vecPosIn(0,0,0);

				int nHeighMap_X = g_cTerrainBlockModifier.GetTextureIndexMapStartIndexX();
				int nHeighMap_Z = g_cTerrainBlockModifier.GetTextureIndexMapStartIndexZ();
				int nXRegion = x - nHeighMap_X;
				int nZRegion = z - nHeighMap_Z;

				nXRegion = max(nXRegion,0);
				nXRegion = min(nXRegion,g_cTerrainBlockModifier.GetTextureIndexMapWidth()-1);
				nZRegion = max(nZRegion,0);
				nZRegion = min(nZRegion,g_cTerrainBlockModifier.GetTextureIndexMapHeight()-1);

				//float* pHGround =  pHeightMap->GetVertexPointer(nXRegion,nZRegion);//m_pGroundHeight->GetVertexPointer(x,z);
				//float fHGround = GetGroundHeight(xx,zz);
				//float fHGround = 0;
				BYTE* pTextureIndex = &pTextureIndexMap[nZRegion * g_cTerrainBlockModifier.GetTextureIndexMapWidth() + nXRegion];
				D3DXVec3TransformCoord(&vPosSrc,&vPos,&matInvese);
				//BYTE nTextureIndex = GetDetailTextureIndex(xx,zz);
				if ((vPosSrc.x > -(m_nTerrainCopyWidth * cell * 0.5F)) && (vPosSrc.x < m_nTerrainCopyWidth * cell * 0.5F) 
					&& (vPosSrc.z > -(m_nTerrainCopyHeight * cell * 0.5F)) && (vPosSrc.z < m_nTerrainCopyHeight * cell * 0.5F))
				{
					int nXIndex = (int)(( vPosSrc.x + m_nTerrainCopyWidth * cell * 0.5F)/cell);
					int nZIndex = (int)(( vPosSrc.z + m_nTerrainCopyHeight * cell * 0.5F)/cell);
					if ((nXIndex>=0)&&( nXIndex<m_nTerrainCopyWidth)&&
						(nZIndex>=0)&&( nZIndex<m_nTerrainCopyHeight))
					{
						int nIndex = nZIndex * m_nTerrainCopyWidth + nXIndex;
						BYTE nTextureIndex = m_pRealTextureIndex[m_lpTerrainCopyBlendData[nIndex]];
						*pTextureIndex = nTextureIndex;
					}
				}
			}
		}
		
		g_cTerrainBlockModifier.UpdateDetailTextureIndex(this);
	}
	if ((dwMixType != -1))
	{
		D3DXVECTOR2 vec2StartPos = D3DXVECTOR2(nXStart * cell,nZStart * cell);
		D3DXVECTOR2 vec2EndPos = D3DXVECTOR2(nXEnd * cell,nZEnd * cell);
		g_cTerrainBlockModifier.FillHeightMap(vec2StartPos,vec2EndPos,this);
		KG3DHeightMap* pHeightMap = g_cTerrainBlockModifier.GetHeightMap();

		for (int z=nZStart;z<=nZEnd;z++)
		{
			for (int x=nXStart;x<=nXEnd;x++)
			{
				float xx = x*cell;
				float zz = z*cell;

				D3DXVECTOR3 vPos(xx,0,zz);
				D3DXVECTOR3 vPosSrc(0,0,0);
				D3DXVECTOR3 vecPosOut(0,0,0);
				D3DXVECTOR3 vecPosIn(0,0,0);

				int nHeighMap_X = g_cTerrainBlockModifier.GetHegihtMapStartIndexX(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;
				int nHeighMap_Z = g_cTerrainBlockModifier.GetHegihtMapStartIndexZ(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;
				int nXRegion = x - nHeighMap_X;
				int nZRegion = z - nHeighMap_Z;

				nXRegion = max(nXRegion,0);
				nXRegion = min(nXRegion,pHeightMap->GetMapWidth()-1);
				nZRegion = max(nZRegion,0);
				nZRegion = min(nZRegion,pHeightMap->GetMapHeight()-1);

				float* pHGround =  pHeightMap->GetVertexPointer(nXRegion,nZRegion);//m_pGroundHeight->GetVertexPointer(x,z);
				float fHGround = GetGroundHeight(xx,zz);
				//float fHGround = 0;
				D3DXVec3TransformCoord(&vPosSrc,&vPos,&matInvese);
				if ((vPosSrc.x > -(m_nTerrainCopyWidth * cell * 0.5F + nExtendCell * cell)) && (vPosSrc.x < m_nTerrainCopyWidth * cell * 0.5F + nExtendCell * cell) 
					&& (vPosSrc.z > -(m_nTerrainCopyHeight * cell * 0.5F + nExtendCell * cell)) && (vPosSrc.z < m_nTerrainCopyHeight * cell * 0.5F + nExtendCell * cell))
				{
					int nXIndex = (int)(( vPosSrc.x + m_nTerrainCopyWidth * cell * 0.5F) / cell);
					int nZIndex = (int)(( vPosSrc.z + m_nTerrainCopyHeight * cell * 0.5F) / cell);
					if ((nXIndex>=0)&&( nXIndex<m_nTerrainCopyWidth)&&
					   (nZIndex>=0)&&( nZIndex<m_nTerrainCopyHeight))
					{
						int nIndex = nZIndex * m_nTerrainCopyWidth + nXIndex;
						float H = m_lpTerrainCopyHeightData[nIndex];
						if (dwMixType == 0 || dwMixType == 1)
						{
							if (dwMixType == 0)
							{
								float fMineWeight = 1 - fWeight;
								if (fMineWeight < 0)
									fMineWeight = 0;
								//H = (*pHGround) * fMineWeight + H * fWeight;
								H = fHGround * fMineWeight + H * fWeight;
							}
							else
							{
								//H = (*pHGround) + H * fWeight;
								H = fHGround + H * fWeight;
							}

							if (bIsMax)
							{
								if((*pHGround)< H )
									(*pHGround) = H;
							}
							else
								(*pHGround) = H;
						}
						else if (dwMixType == 2)
						{
							H = H - m_fTerrainCopyHeightAve;
							(*pHGround) = fHGround + fWeight * H;

						}
					}
					else
					{
						float fv = vPosSrc.z / vPosSrc.x;
						float fAbsfv = fv;
						float fSrcIn = (C.y - nExtendCell * cell) / (C.x - nExtendCell * cell);
						float fSrcOut = C.y / C.x;
						if (fAbsfv < 0) 
							fAbsfv = -fAbsfv;	
						if (fAbsfv > fSrcIn)
						{
							if (vPosSrc.z > 0)
								vecPosIn.z = C.y - nExtendCell * cell;
							else
								vecPosIn.z = -C.y + nExtendCell * cell;
							vecPosIn.x = vecPosIn.z / fv;				
						}
						else
						{
							if (vPosSrc.x > 0)
								vecPosIn.x = C.x - nExtendCell * cell;
							else
								vecPosIn.x = -C.x + nExtendCell * cell;
							vecPosIn.z = vecPosIn.x * fv;
						}

						if (fAbsfv > fSrcOut)
						{
							if (vPosSrc.z > 0)
								vecPosOut.z = C.y;
							else
								vecPosOut.z = -C.y;
							vecPosOut.x = vecPosOut.z / fv;		
						}
						else
						{
							if (vPosSrc.x > 0)
								vecPosOut.x = C.x;
							else
								vecPosOut.x = -C.x;
							vecPosOut.z = vecPosOut.x * fv;
						}

						int nXIndex = (int)(( vecPosIn.x + m_nTerrainCopyWidth * cell*0.5F) / cell);
						int nZIndex = (int)(( vecPosIn.z + m_nTerrainCopyHeight * cell*0.5F) / cell);

						float fHIn = 0;
						float fHOut =0;

						nXIndex = max(0,nXIndex);
						nXIndex = min(m_nTerrainCopyWidth - 1,nXIndex);
						nZIndex = max(0,nZIndex);
						nZIndex = min(m_nTerrainCopyHeight - 1,nZIndex);				
						int nIndex = nZIndex * m_nTerrainCopyWidth + nXIndex;
						fHIn = m_lpTerrainCopyHeightData[nIndex];

						D3DXVECTOR3 vecTransformation;
						D3DXVec3TransformCoord(&vecTransformation,&vecPosOut,&m_matCopyBrush);
						vecTransformation.x = max(m_nZoneStart_X * 1024 * cell,vecTransformation.x);
						vecTransformation.x = min((m_nZoneStart_X + m_nNumZone_X) * 1024 * cell,vecTransformation.x);
						vecTransformation.z = max(m_nZoneStart_Z * 1024 * cell,vecTransformation.z);
						vecTransformation.z = min((m_nZoneStart_Z + m_nNumZone_Z) * 1024 * cell,vecTransformation.z);
						fHOut = GetGroundHeight(vecTransformation.x,vecTransformation.z);
						//fHOut = 0;
						//float* pHGround = NULL ;//m_pGroundHeight->GetVertexPointer(x,z);
						float fHeights = fHIn - (fHIn - fHOut) * ((vecPosIn.x - vPosSrc.x) / (vecPosIn.x - vecPosOut.x));
						*pHGround = fHeights;
					}
				}
			}
		}
		pHeightMap->ComputeNormal();
		g_cTerrainBlockModifier.UpdateTerrainBlockHeight(this);
	}

	AA.x = bbox.A.x;
	AA.y = bbox.A.z;
	CC.x = bbox.B.x;
	CC.y = bbox.B.z;
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::InitTerrainCopyBrush()
{
	HRESULT hr = E_FAIL;
	DWORD dwVertex = m_nTerrainCopyWidth * m_nTerrainCopyHeight;
	VFormat::_Faces_Texture1 *pVers = NULL;
	//VFormat::_Faces_Normal_Texture0 *pVers = NULL;
	DWORD dwFace = 0;
	DWORD *pIndex   = NULL;
	DWORD *pAttrib  = NULL;
	SAFE_RELEASE(m_lpTerrainCopyBrush);
	KG_PROCESS_ERROR((m_nTerrainCopyWidth > 1) && (m_nTerrainCopyHeight > 1));
	dwFace = (m_nTerrainCopyWidth-1) * (m_nTerrainCopyHeight-1) * 2;


	
	hr = D3DXCreateMeshFVF(
		dwFace,
		dwVertex,
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		D3DFVF_XYZ |D3DFVF_TEX1,
		g_pd3dDevice,
		&m_lpTerrainCopyBrush
		);
    
	
	//KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	//KG3DTerrainRender* terrainRender = pScene->m_EntityMgr->GetTerrainRender();
	
	//g_pd3dDevice->SetVertexDeclaration(terrainRender->m_lpDeclaration_VertexOnly);


	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpTerrainCopyBrush->LockVertexBuffer(0, (void**)&pVers);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpTerrainCopyBrush->LockIndexBuffer(0, (void**)&pIndex);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpTerrainCopyBrush->LockAttributeBuffer(0, (DWORD**)&pAttrib);
	KGLOG_COM_PROCESS_ERROR(hr);

	float cell = 100;

	for (int z=0;z<m_nTerrainCopyHeight;z++)
	{
		for (int x=0;x<m_nTerrainCopyWidth;x++)
		{
			int nIndex = z * m_nTerrainCopyWidth + x;

			float H = m_lpTerrainCopyHeightData[nIndex] + 10;
			//float fTextureIndex = m_lpTerrainCopyBlendData[nIndex];
			float PX = x*cell - m_nTerrainCopyWidth * 0.5F * cell;
			float PZ = z*cell - m_nTerrainCopyHeight* 0.5F * cell;

			pVers[nIndex].p = D3DXVECTOR3(PX,H,PZ);

			//KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
			//KG3DTerrainRender* terrainRender = pScene->m_EntityMgr.GetTerrainRender();
			
			pVers[nIndex].tu1 = 0.0f;
			pVers[nIndex].tv1 = 0.0f;
			if(m_lpConverMap)
			{
			  float _x = PX + m_vecCopyPos.x + m_nTerrainCopyWidth * 0.5F * cell;
			  float _z = PZ + m_vecCopyPos.z + m_nTerrainCopyHeight* 0.5F * cell;

			  pVers[nIndex].tu1 = 1.0f - (_x  - m_lpConverMap->GetRect().x) / m_lpConverMap->GetRect().z;
			  pVers[nIndex].tv1 = 1.0f - (_z  - m_lpConverMap->GetRect().y) / m_lpConverMap->GetRect().w;
			}

			//DWORD dwColor = (DWORD)(0x0);
			//if (m_fTerrainCopyHeightMax - m_fTerrainCopyHeightMin != 0)
			//	dwColor = (DWORD)(0xFF * 0.9F * (H - m_fTerrainCopyHeightMin)/(m_fTerrainCopyHeightMax - m_fTerrainCopyHeightMin));
			//pVers[nIndex].diffuse = D3DXCOLOR(1.0f / 8.0f * fTextureIndex,1.0f / 8.0f * fTextureIndex,1.0f / 8.0f * fTextureIndex,1);//0x90000000 | (dwColor<<16) | (dwColor<<8) | (dwColor);
			//pVers[nIndex].tu1     = x*1.0f/m_nTerrainCopyWidth;
			//pVers[nIndex].tv1     = z*1.0f/m_nTerrainCopyHeight;

		}
	}

	DWORD Weight = m_nTerrainCopyWidth - 1;
	for (int z=0;z<m_nTerrainCopyHeight-1;z++)
	{
		for (int x=0;x<m_nTerrainCopyWidth-1;x++)
		{
			DWORD Vertex_A = z    *m_nTerrainCopyWidth+ x;
			DWORD Vertex_B = (z+1)*m_nTerrainCopyWidth+ x;
			DWORD Vertex_C = (z+1)*m_nTerrainCopyWidth+(x+1);
			DWORD Vertex_D = z    *m_nTerrainCopyWidth+(x+1);

			DWORD PloyIndex = z*Weight +x;

			DWORD Faces_A1 = (PloyIndex*2)*3;
			DWORD Faces_B1 = Faces_A1 + 1;
			DWORD Faces_C1 = Faces_B1 + 1;
			DWORD Faces_A2 = (PloyIndex*2+1)*3;
			DWORD Faces_B2 = Faces_A2 + 1;
			DWORD Faces_C2 = Faces_B2 + 1;

			pIndex[Faces_A1] = Vertex_A;
			pIndex[Faces_C1] = Vertex_B;
			pIndex[Faces_B1] = Vertex_D;
			pAttrib[PloyIndex*2] = 0;

			pIndex[Faces_A2] = Vertex_D;
			pIndex[Faces_C2] = Vertex_B;
			pIndex[Faces_B2] = Vertex_C;
			pAttrib[PloyIndex*2+1] = 0;

		}
	}

	hr = S_OK;
Exit0:
	if (pAttrib)
	{
		hr = m_lpTerrainCopyBrush->UnlockAttributeBuffer();
		pAttrib = NULL;
	}


	if (pIndex)
	{
		hr = m_lpTerrainCopyBrush->UnlockIndexBuffer();
		pIndex = NULL;
	}

	if (pVers)
	{
		hr = m_lpTerrainCopyBrush->UnlockVertexBuffer();
		pVers = NULL;
	}
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::RenderTerrainCopyBrush()
{
	HRESULT hr = E_FAIL;
	//D3DXMATRIX matSave,mat,matT,matR,matS;
	D3DXMATRIX matSave;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&matSave);
	KG_PROCESS_ERROR(m_lpTerrainCopyBrush);
	g_pd3dDevice->SetTexture(0,NULL);
	{
		GraphicsStruct::RenderStateCullMode cull(D3DCULL_NONE);
		//GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		GraphicsStruct::RenderState Alpha(D3DRS_ALPHABLENDENABLE,FALSE);
		GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);
		GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
		D3DMATERIAL9 materialNull,materialOld;
		ZeroMemory(&materialNull,sizeof(materialNull));
		materialNull.Diffuse.a = 1.0;
		materialNull.Diffuse.r = 1.0;
		materialNull.Diffuse.g = 1.0;
		materialNull.Diffuse.b = 1.0;
		g_pd3dDevice->GetMaterial(&materialOld);
		g_pd3dDevice->SetMaterial(&materialNull);
		

		//D3DXVECTOR3 vInter;
		//GetIntersectionPoint(&vInter);
		//D3DXMatrixTranslation(&matT,vInter.x,vInter.y,vInter.z);
		//D3DXMatrixRotationY(&matR,m_fTerrainCopyRotation);
		//D3DXMatrixScaling(&matS,m_vTerrainCopyScale.x,m_vTerrainCopyScale.y,m_vTerrainCopyScale.z);

		//mat = matS * matR * matT ;
		g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_matCopyBrush);
		g_pd3dDevice->SetTexture(0, m_lpConverMap->GetTerrainMap());
		m_lpTerrainCopyBrush->DrawSubset(0);
		
		g_pd3dDevice->SetMaterial(&materialOld);
	}

	hr = S_OK;
Exit0:
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&matSave);
	return hr;
}


//HRESULT KG3DSceneOutDoorSpaceMgr::EnableRealTimeShadowMap(BOOL bEnable,KG3DSceneEntityMgr* pEntityMgr)
//{
//	if(bEnable)
//	{
//		if(!m_lpShadowMap)
//		{
//			m_lpShadowMap = new KG3DSceneShadowMap;
//			KGLOG_PROCESS_ERROR(m_lpShadowMap);
//
//			m_lpShadowMap->Init(512,pEntityMgr);
//		}
//	}
//	else
//	{
//		SAFE_DELETE(m_lpShadowMap);
//	}
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

//HRESULT KG3DSceneOutDoorSpaceMgr::ProcessShadowMap(KG3DCamera* pCamera,D3DXVECTOR4& vLight,KG3DSceneSceneEditor* pEditor)
//{
//
//	BOOL bNeedRender = TRUE;
//
//	D3DXVECTOR3 vPosition;
//	pCamera->GetLookAtPosition(&vPosition);
//	D3DXVECTOR3 vFront = pCamera->GetCameraFront();
//	D3DXMATRIX matView_C,matProj_C;
//	D3DXVECTOR3 vNormal;
//	matView_C = pCamera->GetViewMatrix();
//	matProj_C = pCamera->GetProjectionMatrix();
//	D3DXMATRIX matA = matView_C * matProj_C;
//	D3DXMATRIX matCameraVPInv;
//	D3DXMatrixInverse(&matCameraVPInv,NULL,&matA);
//	//m_lpEffectPostRender->SetMatrix("matCameraVPInv",&matCameraVPInv);
//
//	KGLOG_PROCESS_ERROR(m_lpShadowMap);
//
//	{
//		D3DXVECTOR3 V = vPosition - m_lpShadowMap->m_vPositionLast;
//		float Len = D3DXVec3LengthSq(&V);
//		if(Len<360000)
//			bNeedRender = FALSE;
//		else
//			m_lpShadowMap->m_vPositionLast = vPosition;
//	}
//
//	if(1)
//	{
//		//float k = timeGetTime() * 0.0000002F;
//
//		//vNormal = D3DXVECTOR3(cosf(k),0.5F,sinf(k));//D3DXVECTOR3(vLight.x,vLight.y,vLight.z);		
//		vNormal = g_cGraphicsTool.GetSunLight(*(KG3DScene*)pEditor).GetDirection();
//		D3DXVec3Normalize(&vNormal,&(-vNormal));
//
//		D3DXMATRIX matVP[3];
//		for (int i=0;i<3;i++)
//		{
//			if(i==0 || bNeedRender)
//			{
//				float fSzie = m_lpShadowMap->m_ShadowLevel[i].m_fSize;
//				float fLength = 10000;
//
//				D3DXVECTOR3 vC = vPosition + vNormal * fLength;
//				/*m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetPosition(vC);
//				m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetLookAtPosition(vPosition);
//				m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetUpDirection(D3DXVECTOR3(0,1,0));
//				m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetOrthogonal(fSzie,fSzie,100,fLength*1.5F);
//				m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetCamera();*/
//
//				KG3DCamera& Cam = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight;
//				Cam.SetPositionLookAtUp(&vC, &vPosition, &D3DXVECTOR3(0,1,0));
//				Cam.SetOrthogonal(fSzie,fSzie,100,fLength*1.5F);
//				D3DXMATRIX matView_L,matProj_L;
//				matView_L = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.GetViewMatrix();
//				matProj_L = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.GetProjectionMatrix();
//
//				matVP[i] = matView_L * matProj_L;
//				m_lpShadowMap->m_ShadowLevel[i].m_matVP = matVP[i];
//			}
//		}
//
//		//m_lpEffectPostRender->SetMatrix("matLightVP1",&matVP[0]);
//		//m_lpEffectPostRender->SetMatrix("matLightVP2",&matVP[1]);
//		//m_lpEffectPostRender->SetMatrix("matLightVP3",&matVP[2]);	
//
//		g_cGraphicsTool.GetShareDepthSurface512();
//
//		for (int i=0;i<3;i++)
//		{
//			if(i==0 || bNeedRender)
//			{
//				LPDIRECT3DSURFACE9 pColorSurface = g_cGraphicsTool.GetShareColorSurface512();
//
//				m_lpShadowMap->m_ShadowLevel[i].ProcessShadowMap(&m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight,
//					m_lpShadowMap->m_vPositionLast,
//					pEditor,
//					m_lpShadowMap,
//					pColorSurface);
//			}
//		}
//	}
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

//HRESULT KG3DSceneOutDoorSpaceMgr::ShowShadowMapScreen()
//{
//	if(!m_lpShadowMap)
//		return E_FAIL;
//
//	g_cGraphicsTool.DrawScreenRect(&D3DXVECTOR2(0,0),&D3DXVECTOR2(512,512),0,0xFFFFFFFF,
//		m_lpShadowMap->m_ShadowLevel[0].m_lpSMShadowMap);
//
//	return S_OK;
//}

KG3DMessageSender& KG3DSceneOutDoorSpaceMgr::GetSceneMessageSender(const KM_MESSEGE& Type )
{
	return m_Scene.GetMessageSender(Type);
}

ULONG KG3DSceneOutDoorSpaceMgr::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DSceneOutDoorSpaceMgr::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}


HRESULT KG3DSceneOutDoorSpaceMgr::ComputeAllEntityBBox()
{
	static KG3DEntityObjectShell*  sShells[4096];
	DWORD dwCount = 0;
	m_mapObjectShell.GetEntityShell(sShells,dwCount,4096);

	for (DWORD i=0;i<dwCount;i++)
	{
		KG3DEntityObjectShell* pShell = sShells[i];
		KG3DRepresentObject *pObject = pShell->m_lpObject;
		if (pObject)
		{
			pObject->FrameMove();//调FrameMove是为了模型的世界坐标被更新。以确保后面的计算正确。//<SLEEP_REFRACTOR>
			RequestObjectTransformationChange(pObject);
		}
	}
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CheckMemoryUseMultiThread_TerrainBlock(KG3DSceneSceneEditor* pEditor)
{
	DWORD dwNow = timeGetTime();
	KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);

	KG_PROCESS_SUCCESS(dwNow - m_dwLastTimeCheckMemoryUse_EntityShell < 100);

	m_dwLastTimeCheckMemoryUse_EntityShell = dwNow;

	if(g_cEngineOption.bEnableDynamicUnload)
	{
		set<KG3DSceneZone*>::iterator i = m_setZone.begin();
		set<KG3DSceneZone*>::iterator iend = m_setZone.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;

			pZone->CheckMemoryUseMultiThread(TERRAIN_UNLOAD_FRAME_INTERVAL, &m_bTryPauseMultiThreadLoad);
			
			/*if(
                pZone->m_lpTerrainBlock && pZone->m_lpTerrainBlock->IsLoaded() &&
                (pZone->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH) && 
                ((g_dwRenderCount - pZone->GetVisibleCount()) > TERRAIN_UNLOAD_FRAME_INTERVAL)
            )
			{
				pZone->SetTerrainLoadState(BLOCK_LOAD_NODATA);
				SAFE_RELEASE(pZone->m_lpTerrainBlock);
			}*/
			i++;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if(m_lpDataInterface)
	{
		m_lpDataInterface->CheckMemoryUseMultiThread(10000,&m_bTryPauseMultiThreadLoad);
	}	


Exit1:
	return S_OK;
}

HRESULT MultiThreadShellMap::CheckMemoryUse_EntityShell(BOOL* pbStop,int nDestCount,set<KG3DRepresentObjectSoundBall*>*psetObjectSoundBall,vector<KG3DRepresentObject*>*pvecObjectToRelease)
{	
	int nCount = 0;
	static const DWORD ENTITY_SHELL_UNLOAD_FRAME_INTERVAL = 1000;

	TypeShellMap::iterator i = m_map.begin();
	TypeShellMap::iterator iend = m_map.end();
	while (i!=iend)
	{
		KG_PROCESS_SUCCESS(*pbStop);

		KG3DEntityObjectShell* pShell = i->second;

		if(pShell && (pShell->m_uHandle != -1) && pShell->m_bLoaded)
		{
			if(g_dwRenderCount - pShell->GetVisibleCount() >= ENTITY_SHELL_UNLOAD_FRAME_INTERVAL)
			{
				if(pShell->m_lpObject)
				{
					if (pShell->m_lpObject->GetType() == REPRESENTOBJECT_SOUNDBALL)
					{
						KG3DRepresentObjectSoundBall *pSoundBall = static_cast<KG3DRepresentObjectSoundBall*>(pShell->m_lpObject);
						std::set<KG3DRepresentObjectSoundBall*>::iterator it = psetObjectSoundBall->find(pSoundBall);
						if (it != psetObjectSoundBall->end())
						{
							psetObjectSoundBall->erase(it);
							SAFE_RELEASE(pSoundBall);
						}
					}
					if (pShell->m_lpObject->IsLoaded())
					{
						pShell->m_lpObject->AddRef();
						pvecObjectToRelease->push_back(pShell->m_lpObject);
					}
				}

				pShell->TryDynamicUnLoad();
				
				nCount++;

				if(nCount>=nDestCount)
					break;
			}
		}

		i++;
	}

Exit1:
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CheckMemoryUse_EntityShell(BOOL* pbStop,int nDestCount)
{
	DWORD dwNow = timeGetTime();

	vector<KG3DRepresentObject*>vecObjectToRelease;

	m_mapObjectShell.CheckMemoryUse_EntityShell(pbStop,nDestCount,&m_setObjectSoundBall,&vecObjectToRelease);

	if(m_lpSceneEditor)
	{
		for (size_t i=0;i<vecObjectToRelease.size();i++)
		{
			KG3DRepresentObject* pObject = vecObjectToRelease[i];
			KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);

			m_lpSceneEditor->SetSelectable(Entity, FALSE);

			m_lpSceneEditor->GetSelectionTool().SetSelected(Entity, FALSE);
			m_lpSceneEditor->m_listRenderEntity.remove(Entity);

			CheckReleaseFromPvs(pObject);
			SAFE_RELEASE(pObject);
		}
		vecObjectToRelease.clear();
	}

	DWORD dwCost = timeGetTime() - dwNow;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 10)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DSceneOutDoorSpaceMgr::CheckMemoryUse_EntityShell.",dwCost);
	}
	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::CheckMemoryUse(KG3DSceneSceneEditor* pEditor)
{
	if(g_dwRenderCount % 20 > 0)
		return S_OK;

	DWORD dwNow = timeGetTime();
	//////////////////////////////////////////////////////////////////////////

	DWORD dwCost = timeGetTime() - dwNow;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 30)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DSceneOutDoorSpaceMgr::CheckMemoryUseMultiThread.",dwCost);
	}

	return S_OK;
}

HRESULT KG3DSceneOutDoorSpaceMgr::LoadTerrainBlockByPos(float fx, float fz)
{
	KG_PROCESS_ERROR(m_lpSceneBlockLoader);
	m_lpSceneBlockLoader->LoadTerrainBlockByPos(fx,fz);
	return S_OK;
Exit0:
	return E_FAIL;
}

DWORD KG3DSceneOutDoorSpaceMgr::GetLoadedObjectShellCount()
{
	DWORD dwT = 0;

	static KG3DEntityObjectShell*  sShells[4096];
	DWORD dwCount = 0;
	m_mapObjectShell.GetEntityShell(sShells,dwCount,4096);

	for (DWORD i=0;i<dwCount;i++)
	{
		KG3DEntityObjectShell* pShell = sShells[i];
		if(pShell && pShell->m_bLoaded)
			dwT++;

	}

	return dwT;
}

HRESULT KG3DSceneOutDoorSpaceMgr::PauseMultiThreadLoad(BOOL nEnable)
{
	if(nEnable && g_eEngineOption.bUseMultiThreadLoad)
	{
		m_bTryPauseMultiThreadLoad = TRUE;

		while (!m_bPauseMultiThreadLoad)//等待标志位设置
		{
			Sleep(100);
		}
	}
	else
	{
		m_bPauseMultiThreadLoad = nEnable;
	}

	return S_OK;
}


HRESULT KG3DSceneOutDoorSpaceMgr::ProcessMultiThread()
{
    //ProcessSoundBallMultiThread();
    //CheckMemoryUseMultiThread_TerrainBlock(NULL);

	KG_PROCESS_SUCCESS(m_bPauseMultiThreadLoad);
	KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);

	if(m_lpSceneBlockLoader)
	{
		m_lpSceneBlockLoader->MultiThreadLoad();
	}
Exit1:
	if(m_bTryPauseMultiThreadLoad)
	{
		m_bPauseMultiThreadLoad = TRUE;
		m_bTryPauseMultiThreadLoad = FALSE;
	}	
	
	return S_OK;
}

ULONG KG3DSceneOutDoorSpaceMgr::GetObjectCount( DWORD dwTypeFilter )
{
	return KG3D_OUTDOOR::GetObjectCount(*this, dwTypeFilter);
}

KG3DTerrainBlock* KG3DSceneOutDoorSpaceMgr::GetTerrainBlock(int nLevel,int nIndexX,int nIndexZ)
{
	switch(nLevel)
	{
	case BLOCK_TYPE_REGION:
		{
			KG3DSceneRegion* pRegion = GetRegion(nIndexX,nIndexZ);
			if (pRegion)
				return pRegion->m_lpTerrainBlock;
		}
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = GetSection(nIndexX,nIndexZ);
			if (pSection)
				return pSection->m_lpTerrainBlock;
		}
	    break;
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = GetZone(nIndexX,nIndexZ);
			if (pZone)
				return pZone->m_lpTerrainBlock;
		}
	    break;
	}
	return NULL;
}

KG3DEntityObjectShell* KG3DSceneOutDoorSpaceMgr::GetObjShellByHandle(UINT uHandle)
{
    return m_mapObjectShell.Find(uHandle);
}

HRESULT KG3DSceneOutDoorSpaceMgr::FrameMove(DWORD dwLoadLimit)
{
	DWORD dwTime = timeGetTime();

	//static KG3DEntityObjectShell*  sShells[4096];
	//DWORD dwCount = 0;
	//m_mapObjectShellMultiThreadLoaded.OutPutEntityShell(sShells,dwCount,min(dwLoadLimit,(DWORD)4096));

	//for (DWORD i=0;i<dwCount;i++)
	//{
	//	KG3DEntityObjectShell* pShell = sShells[i];
	//	UINT uHandle = pShell->m_uHandle;

	//	//这里要重新计算一次BBox，因为保存的Shell的Box可能不是正确的。
	//	//OutputEntityShell必须保证下面两个assert
	//	_ASSERTE(NULL != pShell->m_lpObject);
	//	_ASSERTE(pShell->m_lpObject->IsLoaded());
	//	pShell->m_lpObject->ComputeBBox();
	//	pShell->m_vBBox_A = pShell->m_lpObject->m_BBox.A;
	//	pShell->m_vBBox_B = pShell->m_lpObject->m_BBox.B;	

	//	m_mapObjectShell.InputShell(uHandle,pShell);

	//	pShell->RemoveFromAllRefrenceContainer();
	//	InputObjectShell(pShell,FALSE);
	//}

	ProcessSoundBallMultiThread();

	DWORD dwCost = timeGetTime() - dwTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DSceneOutDoorSpaceMgr::FrameMove  ",dwCost);
	}
	return S_OK;
}

BOOL KG3DSceneOutDoorSpaceMgr::CheckTerrainBlockExist(int nLevel,POINT& Index)
{
	BOOL bExist = FALSE;
	if(m_lpDataInterface)
		bExist = m_lpDataInterface->GetTerrainBlock(nLevel,Index);
	return bExist;
}
#ifdef KG3D_AUTO_TEST 
HRESULT KG3DSceneOutDoorSpaceMgr::GetAllEntityInfoHandle(vector<UINT>* vecHandle)
{
	if (m_lpDataInterface)
	{
		return m_lpDataInterface->GetAllEntityInformationHandle(vecHandle);
	}
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetEntityInfoformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpDataInterface);
	hr = m_lpDataInterface->GetEntityInfoByHandle(uHandle,pdwDataType,pdwLength,pData,dwMaxLen);
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetAllTerrainInfoHandle(vector<UINT>* vecHandle)
{
	if (m_lpDataInterface)
	{
		return m_lpDataInterface->GetAllTerrianInformationHandle(vecHandle);
	}
	return E_FAIL;
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetTerrainInfoformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpDataInterface);
	
	hr = m_lpDataInterface->GetTerrainInformation(uHandle,pdwDataType,pdwLength,pData);
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetEntityData(map<UINT,KGEntityData>* mapEntityData)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpDataInterface);
	hr = m_lpDataInterface->GetEntityData(mapEntityData);
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::GetAllSceneBlock(vector<SceneBlockIndex>* vecSceneBlockIndex)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpDataInterface);
	hr = m_lpDataInterface->GetAllSceneBlock(vecSceneBlockIndex);
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}
HRESULT KG3DSceneOutDoorSpaceMgr::GetSceneBlockData(int nLevel,POINT Index,DWORD& dwLenght,PVOID pData)//获取地形数据，调用者自己负责该数据的创建和删除，该函数背部完成数据复制
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpDataInterface);
	hr = m_lpDataInterface->GetSceneBlockData(nLevel,Index,dwLenght,pData);
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}
#endif

HRESULT MultiThreadShellMap::UnInit()
{
	TypeShellMap::iterator i = m_map.begin();
	TypeShellMap::iterator iend = m_map.end();
	while (i!=iend)
	{
		KG3DEntityObjectShell* pShell = i->second;
		g_cObjectShellPool.FreeResource(pShell);
		i++;
	}	
	m_map.clear();

	return S_OK;
}

BOOL MultiThreadShellMap::IsExist(UINT uHandle)
{
	m_vlock.Lock();

	BOOL bExist = FALSE;
	TypeShellMap::iterator iFind = m_map.find(uHandle);
	if (iFind != m_map.end())
	{
		bExist =  TRUE;
	}
	m_vlock.Unlock();

	return bExist;
}

HRESULT MultiThreadShellMap::InputShell(UINT uHandle,KG3DEntityObjectShell* pShell)
{
	m_vlock.Lock();

	//m_map[uHandle] = pShell;
	TypeShellMap::_Pairib ib = m_map.insert(TypeShellMap::value_type(uHandle, pShell));
	//_ASSERTE(ib.second && _T("如果插入不成功，代表原来那个Key值那已经有了一个Shell了，直接写m_map[uHandle] = pShell会覆盖原来的Shell，造成泄漏和其它后果，所以应该没有重复的Handle，且应该插入成功"));

	m_vlock.Unlock();

	return S_OK;
}

HRESULT MultiThreadShellMap::OutPutEntityShell(KG3DEntityObjectShell* pShellArray[],DWORD& dwCount,DWORD dwMaxCount)
{
	dwCount = 0;
	m_vlock.Lock();

	TypeShellMap::iterator i = m_map.begin();
	TypeShellMap::iterator iend = m_map.end();
	while (i!=iend)
	{
		if(dwCount < dwMaxCount)
		{
			KG3DEntityObjectShell* pShell = i->second;

			if(pShell->m_lpObject)
			{
				pShell->m_lpObject->FrameMove();

				if(pShell->m_lpObject->IsLoaded())
				{
					pShellArray[dwCount] = pShell;
					dwCount++;
					i = m_map.erase(i);

					continue;
				}
			}

			i++;
		}
		else
		{
			break;
		}
	}

	m_vlock.Unlock();

	return S_OK;
}

HRESULT MultiThreadShellMap::GetEntityShell(KG3DEntityObjectShell* pShellArray[],DWORD& dwCount,DWORD dwMaxCount)
{
	dwCount = 0;
	m_vlock.Lock();

	TypeShellMap::iterator i = m_map.begin();
	TypeShellMap::iterator iend = m_map.end();
	while (i!=iend)
	{
		if(dwCount < dwMaxCount)
		{
			KG3DEntityObjectShell* pShell = i->second;

			if(pShell->m_lpObject)
			{
				pShellArray[dwCount] = pShell;
				dwCount++;
			}
				
			i++;
		}
		else
		{
			break;
		}
	}

	m_vlock.Unlock();

	return S_OK;
}

KG3DEntityObjectShell* MultiThreadShellMap::Find(UINT uHandle)
{
	KG3DEntityObjectShell* pShell = NULL;
	m_vlock.Lock();

	TypeShellMap::iterator i = m_map.find(uHandle);
	if(i!=m_map.end())
	{
		pShell = i->second;
	}

	m_vlock.Unlock();
	return pShell;
}

HRESULT MultiThreadShellMap::Release(TKG3DResourcePool<KG3DEntityObjectShell>* pObjectShellPool,UINT uHandle)
{
	KG3DEntityObjectShell* pShell = NULL;
	HRESULT hr = E_FAIL;
	m_vlock.Lock();

	TypeShellMap::iterator i = m_map.find(uHandle);
	if(i!=m_map.end())
	{
		pShell = i->second;
		pObjectShellPool->FreeResource(pShell);
		m_map.erase(i);
		hr = S_OK;
	}

	m_vlock.Unlock();

	return hr;
}

HRESULT KG3DSceneOutDoorSpaceMgr::DoDynamicUnloadEntityShells()
{
	if(g_cEngineOption.bEnableDynamicUnload)
	{
		CheckMemoryUse_EntityShell(&m_bTryPauseMultiThreadLoad,5);

        CheckMemoryUseMultiThread_TerrainBlock(NULL);
	}

	return S_OK;
}

KG3DLoadPostProcessor::KG3DLoadPostProcessor()
{

}
KG3DLoadPostProcessor::~KG3DLoadPostProcessor()
{
    ASSERT(m_Models.size() == 0);
    ASSERT(m_RepresentObjects.size() == 0);
}

HRESULT KG3DLoadPostProcessor::Init()
{
    return S_OK;
}

HRESULT KG3DLoadPostProcessor::UnInt()
{   
    m_Lock.Lock();
    m_Models.insert(m_Models.end(), m_ModelsCache.begin(), m_ModelsCache.end());
    m_ModelsCache.clear();

    m_RepresentObjects.insert(m_RepresentObjects.end(), m_RepresentObjectsCache.begin(), m_RepresentObjectsCache.end());
    m_RepresentObjectsCache.clear();
    m_Lock.Unlock();

    for (size_t i = 0; i < m_Models.size(); ++i)
    {
        m_Models[i]->Release();
    }
    m_Models.clear();

    //////////////////////////////////////////////////////////////////////////
    for (size_t i = 0; i < m_RepresentObjects.size(); ++i)
    {
        m_RepresentObjects[i]->Release();
    }
    m_RepresentObjects.clear();

    return S_OK;
}

HRESULT KG3DLoadPostProcessor::InputModel(KG3DModel *pModel)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pModel);
    
    m_Lock.Lock();
    pModel->AddRef();
    m_ModelsCache.push_back(pModel);
    m_Lock.Unlock();
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DLoadPostProcessor::InputRepresentObject(KG3DRepresentObject *pObject)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pObject);

    m_Lock.Lock();
    pObject->AddRef();
    m_RepresentObjectsCache.push_back(pObject);
    m_Lock.Unlock();
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DLoadPostProcessor::FrameMove()
{
    HRESULT hrResult = E_FAIL;
    KG3DModel *pModel = NULL;
    KG3DRepresentObject *pObject = NULL;

    if ((!m_ModelsCache.empty()) || (!m_RepresentObjectsCache.empty()))
    {
        m_Lock.Lock();

        m_Models.insert(m_Models.end(), m_ModelsCache.begin(), m_ModelsCache.end());
        m_ModelsCache.clear();

        m_RepresentObjects.insert(m_RepresentObjects.end(), m_RepresentObjectsCache.begin(), m_RepresentObjectsCache.end());
        m_RepresentObjectsCache.clear();
        
        m_Lock.Unlock();
    }

    for (size_t i = 0; i < m_Models.size(); NULL)
    {
        pModel = m_Models[i];

        if ((!pModel) || (QueryRef(pModel) == 1))
        {
            m_Models[i] = m_Models[m_Models.size() - 1];
            m_Models.resize(m_Models.size() - 1);
            SAFE_RELEASE(pModel);

            continue;
        }

        pModel->LoadPostProcess();
        if (pModel->IsLoaded())
        {
            pModel->ComputeBBox();
            m_Models[i] = m_Models[m_Models.size() - 1];
            m_Models.resize(m_Models.size() - 1);
            pModel->Release();

            continue;
        }
        ++i;
    }

    //////////////////////////////////////////////////////////////////////////
    for (size_t i = 0; i < m_RepresentObjects.size(); NULL)
    {
        pObject = m_RepresentObjects[i];

        if ((!pObject) || (QueryRef(pObject) == 1))
        {
            m_RepresentObjects[i] = m_RepresentObjects[m_RepresentObjects.size() - 1];
            m_RepresentObjects.resize(m_RepresentObjects.size() - 1);
            SAFE_RELEASE(pObject);

            continue;
        }
        
		pObject->LoadPostProcess();
		
        if (pObject->IsLoaded())
        {
            pObject->ComputeBBox();
            m_RepresentObjects[i] = m_RepresentObjects[m_RepresentObjects.size() - 1];
            m_RepresentObjects.resize(m_RepresentObjects.size() - 1);
            pObject->Release();

            continue;
        }   
        ++i;
    }

//Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}
