#include "StdAfx.h"
#include "KG3DSceneBlockLoader.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DTerrainBlock.h"
#include "KG3DEngineManager.h"
#include "KG3DRepresentObject.h"
#include "KG3DMessenger.h"
#include "KG3DRepresentObjectSoundBall.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TKG3DResourcePool<KG3DSceneZone>g_cSceneZonePool;

//TKG3DMultiTheadBuffer<SceneBlockList>g_cSceneBlockList;

#define  MAX_OUTDOORMGR   32

KG3DSceneOutDoorSpaceMgr* g_cOutDoorSpaceMgrRegister[MAX_OUTDOORMGR];

HRESULT InitOutDoorSpaceMgrRegister()
{
	ZeroMemory(g_cOutDoorSpaceMgrRegister,sizeof(KG3DSceneOutDoorSpaceMgr*)*MAX_OUTDOORMGR);
	return S_OK;
}

HRESULT UnInitOutDoorSpaceMgrRegister()
{
	for (int i=0;i<MAX_OUTDOORMGR;i++)
	{
		if( g_cOutDoorSpaceMgrRegister[i] )
		{
			SAFE_RELEASE(g_cOutDoorSpaceMgrRegister[i]);
		}
	}

	return S_OK;
}

HRESULT RegisterOutDoorSpaceMgr(KG3DSceneOutDoorSpaceMgr* pMgr)
{
	KGLOG_PROCESS_ERROR(pMgr);

	for (int i=0;i<MAX_OUTDOORMGR;i++)
	{
		if( g_cOutDoorSpaceMgrRegister[i] == NULL )
		{
			pMgr->AddRef();
			g_cOutDoorSpaceMgrRegister[i] = pMgr;

			return S_OK;
		}
	}

Exit0:
	return E_FAIL;
}

HRESULT UnRegisterOutDoorSpaceMgr(KG3DSceneOutDoorSpaceMgr* pMgr)
{
	KGLOG_PROCESS_ERROR(pMgr);

	for (int i=0;i<MAX_OUTDOORMGR;i++)
	{
		if( g_cOutDoorSpaceMgrRegister[i] == pMgr )
		{
			g_cOutDoorSpaceMgrRegister[i] = NULL;
			pMgr->Release();

			return S_OK;
		}
	}

Exit0:
	return E_FAIL;
}


HRESULT g_MultiThreadSceneBlockLoadUnLoad()
{
	for (int i=0;i<MAX_OUTDOORMGR;i++)
	{
		KG3DSceneOutDoorSpaceMgr* pMgr = g_cOutDoorSpaceMgrRegister[i];
		if(pMgr && !pMgr->m_bPauseMultiThreadLoad)
		{
            pMgr->AddRef();
			pMgr->ProcessMultiThread(); 
			pMgr->Release();
		}
	}
	return S_OK;

//	SceneBlockList* pList = NULL;
//	SceneBlockList* pListNext = NULL;
//	if(SUCCEEDED(g_cSceneBlockList.FindList_DataOK(pList,pListNext)))
//	{
//		if(pList && pList->m_lpSceneBlockLoader)
//		{
//			KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//
//			{
//				list<KG3DSceneZone*>::iterator i = pList->m_listTerrainZone.begin();
//				list<KG3DSceneZone*>::iterator iend = pList->m_listTerrainZone.end();
//				while (i!=iend)
//				{
//					KG3DSceneZone* pZone = *i;
//
//					pList->m_lpSceneBlockLoader->DynamicLoadTerrainZone(pZone,TRUE);
//
//					++i;
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//			//////////////////////////////////////////////////////////////////////////
//			{
//				list<KG3DSceneZone*>::iterator i = pList->m_listSceneZone.begin();
//				list<KG3DSceneZone*>::iterator iend = pList->m_listSceneZone.end();
//				while (i!=iend)
//				{
//					KG3DSceneZone* pZone = *i;
//
//					pList->m_lpSceneBlockLoader->DynamicLoadObjectZone (pZone,TRUE);
//					++i;
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//			//////////////////////////////////////////////////////////////////////////
//			{
//				list<KG3DSceneSection*>::iterator i = pList->m_listTerrainSection.begin();
//				list<KG3DSceneSection*>::iterator iend = pList->m_listTerrainSection.end();
//				while (i!=iend)
//				{
//					KG3DSceneSection* pSection = *i;
//
//					pList->m_lpSceneBlockLoader->DynamicLoadTerrainSection(pSection,TRUE);
//					
//					++i;
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//			//////////////////////////////////////////////////////////////////////////
//			{
//				list<KG3DSceneSection*>::iterator i = pList->m_listSceneSection.begin();
//				list<KG3DSceneSection*>::iterator iend = pList->m_listSceneSection.end();
//				while (i!=iend)
//				{
//					KG3DSceneSection* pSection = *i;
//
//					pList->m_lpSceneBlockLoader->DynamicLoadObjectSection (pSection,TRUE);
//					++i;
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//			//////////////////////////////////////////////////////////////////////////			{
//			{	list<KG3DSceneRegion*>::iterator i = pList->m_listTerrainRegion.begin();
//				list<KG3DSceneRegion*>::iterator iend = pList->m_listTerrainRegion.end();
//				while (i!=iend)
//				{
//					KG3DSceneRegion* pRegion = *i;
//
//					pList->m_lpSceneBlockLoader->DynamicLoadTerrainRegion(pRegion,TRUE);
//
//					++i;
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//			//////////////////////////////////////////////////////////////////////////
//			{
//				list<KG3DSceneRegion*>::iterator i = pList->m_listSceneRegion.begin();
//				list<KG3DSceneRegion*>::iterator iend = pList->m_listSceneRegion.end();
//				while (i!=iend)
//				{
//					KG3DSceneRegion* pRegion = *i;
//
//					pList->m_lpSceneBlockLoader->DynamicLoadObjectRegion (pRegion,TRUE);
//
//					++i;
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//			//////////////////////////////////////////////////////////////////////////
//			{
//				DWORD dwOption = OBJLOAD_MULTITHEAD;
//
//				for (size_t i=0;i<pList->m_vecObjectShell.size();i++)
//				{
//					KG3DEntityObjectShell* pShell = pList->m_vecObjectShell[i];
//					if (pShell)
//					{
//						pShell->TryDynamicLoad(dwOption);
//					}
//
//					KG_PROCESS_SUCCESS(pListNext->m_dwState == BLOCK_LIST_STATE_DATAOK);
//					KG_PROCESS_SUCCESS(pList->m_lpSceneBlockLoader->m_bQuitLoad);
//				}
//			}
//
//Exit1:
//			pList->m_listTerrainRegion.clear();
//			pList->m_listTerrainSection.clear();
//			pList->m_listTerrainZone.clear();
//
//			pList->m_listSceneRegion.clear();
//			pList->m_listSceneSection.clear();
//			pList->m_listSceneZone.clear();
//			pList->m_vecObjectShell.clear();
//
//			//pList->m_lpSceneBlockLoader->CheckMemoryUse(NULL);
//
//			pList->m_lpSceneBlockLoader->OnMultiThreadEnd();
//			pList->m_lpSceneBlockLoader = NULL;
//			pList->m_dwState = BLOCK_LIST_STATE_NODATA;
//		}
//	}
//	return S_OK;
}

KG3DSceneBlockLoader::KG3DSceneBlockLoader(KG3DSceneOutDoorSpaceMgr& OutDoor)
:m_OutDoor(OutDoor)
{
	m_dwEntityLoadCount = 0;
	m_dwLoadLimit = 1;
	m_bQuitLoad = FALSE;
	m_bNeedRefreshLoadList = FALSE;

	m_DynamicLoadPositionLast = D3DXVECTOR2(0,0);
	m_DynamicLoadPositionNow  = D3DXVECTOR2(0,0);
	m_fRZone = 0;
	m_fRSection = 0;
	m_fRRegion = 0;
}

KG3DSceneBlockLoader::~KG3DSceneBlockLoader()
{
	;
}

HRESULT KG3DSceneBlockLoader::MultiThreadLoad()
{
	if(m_bNeedRefreshLoadList)
	{
		float fSound = g_SoundBallConfig.m_fMaxListenRange;
		D3DXVECTOR2 A_Zone,C_Zone;
		D3DXVECTOR2 A_Section,C_Section;
		D3DXVECTOR2 A_Region,C_Region;
		D3DXVECTOR2 A_Sound,C_Sound;

		D3DXVECTOR2 vPosition = m_DynamicLoadPositionLast;

		A_Zone    = vPosition - D3DXVECTOR2(m_fRZone,m_fRZone);
		C_Zone    = vPosition + D3DXVECTOR2(m_fRZone,m_fRZone);

	/*	A_Section = vPosition - D3DXVECTOR2(m_fRSection,m_fRSection);
		C_Section = vPosition + D3DXVECTOR2(m_fRSection,m_fRSection);
		
		A_Region  = vPosition - D3DXVECTOR2(m_fRRegion,m_fRRegion);
		C_Region  = vPosition + D3DXVECTOR2(m_fRRegion,m_fRRegion);*/
	
		GetLoadRect(vPosition,A_Section,C_Section,A_Region,C_Region);
		A_Sound   = vPosition - D3DXVECTOR2(fSound,fSound);
		C_Sound   = vPosition + D3DXVECTOR2(fSound,fSound);
	
		SetLoadRect(A_Zone,C_Zone,A_Section,C_Section,A_Region,C_Region,TRUE);
		
		m_bNeedRefreshLoadList = FALSE;

		m_OutDoor.FindSoundBall(A_Sound,C_Sound);
	}

	LoadList* pList = &m_LoadListMultiThread;
	KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwOption = OBJLOAD_MULTITHEAD;

		set<KG3DEntityObjectShell*>::iterator i = pList->m_setObjectShellToLoad.begin();
		set<KG3DEntityObjectShell*>::iterator iend = pList->m_setObjectShellToLoad.end();

		while (i!=iend)
		{
			KG3DEntityObjectShell* pShell = *i;
			if (pShell)
			{
				HRESULT hr = pShell->TryDynamicLoad(dwOption);
				KGLOG_COM_CHECK_ERROR(hr);
			}
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
			++i;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneZone*>::iterator i = pList->m_listTerrainZoneToLoad.begin();
		list<KG3DSceneZone*>::iterator iend = pList->m_listTerrainZoneToLoad.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;

			DynamicLoadTerrainZone(pZone,TRUE);

			++i;
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneZone*>::iterator i = pList->m_listSceneObjectZoneToLoad.begin();
		list<KG3DSceneZone*>::iterator iend = pList->m_listSceneObjectZoneToLoad.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;

			DynamicLoadObjectZone (pZone,TRUE);
			++i;
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneSection*>::iterator i = pList->m_listTerrainSectionToLoad.begin();
		list<KG3DSceneSection*>::iterator iend = pList->m_listTerrainSectionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneSection* pSection = *i;

			DynamicLoadTerrainSection(pSection,TRUE);

			++i;
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneSection*>::iterator i = pList->m_listSceneObjectSectionToLoad.begin();
		list<KG3DSceneSection*>::iterator iend = pList->m_listSceneObjectSectionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneSection* pSection = *i;

			DynamicLoadObjectSection (pSection,TRUE);
			++i;
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
		}
	}
	//////////////////////////////////////////////////////////////////////////			{
	{	
		list<KG3DSceneRegion*>::iterator i = pList->m_listTerrainRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = pList->m_listTerrainRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;

			DynamicLoadTerrainRegion(pRegion,TRUE);

			++i;
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneRegion*>::iterator i = pList->m_listSceneObjectRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = pList->m_listSceneObjectRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;

			DynamicLoadObjectRegion (pRegion,TRUE);

			++i;
			KG_PROCESS_SUCCESS(m_bNeedRefreshLoadList);
			KG_PROCESS_SUCCESS(m_OutDoor.m_bTryPauseMultiThreadLoad);
		}
	}
	//////////////////////////////////////////////////////////////////////////


Exit1:
	pList->CleanUp();

	return S_OK;
}

HRESULT KG3DSceneBlockLoader::CheckMemoryUse(KG3DSceneSceneEditor* pEditor)
{
	m_OutDoor.CheckMemoryUseMultiThread_TerrainBlock(pEditor);
	return S_OK;
}

HRESULT KG3DSceneBlockLoader::DynamicLoadTerrainZone(KG3DSceneZone* pZone,BOOL bMultiThread)
{
	KGLOG_PROCESS_ERROR(pZone);

	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);

	pZone->SetVisibleCount(g_dwRenderCount);

	if(pZone->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
	{

		POINT Index = { pZone->GetIndexX(), pZone->GetIndexZ() };

		BOOL bTerrain = m_OutDoor.m_lpDataInterface->GetTerrainBlock(BLOCK_TYPE_ZONE,Index);

		if(bTerrain)//创建地形
		{
			HRESULT hr = pZone->CreateTerrainBlock();
			KGLOG_COM_PROCESS_ERROR(hr);

			vector<DWORD>vecDataType;
			m_OutDoor.m_lpDataInterface->GetAllTerrainBlockDataType(BLOCK_TYPE_ZONE,Index,&vecDataType);

			for (size_t i=0;i<vecDataType.size();i++)
			{
				DWORD dwType = vecDataType[i];

				pZone->m_lpTerrainBlock->OnTerrainBlockModified(dwType,m_OutDoor.m_lpDataInterface,&m_OutDoor,bMultiThread);
			}

			pZone->ComputeBBox(TRUE);
			pZone->m_lpTerrainBlock->m_bLoaded = TRUE;
			//pZone->m_lpTerrainBlock->CheckWaterVisible();//这个要在m_bLoaded设为TRUE后，才调。不然取水面高度和地表高度来判断水是否可见就不正确了
		}

		pZone->SetTerrainLoadState(BLOCK_LOAD_TERRAIN_FINISH);
	}

	if(pZone->GetObjectLoadState() < BLOCK_LOAD_LISTOK)
	{

		POINT Index = { pZone->GetIndexX(), pZone->GetIndexZ() };

		set<UINT>setHandle;
		m_OutDoor.m_lpDataInterface->GetEntityOnSceneBlock(&setHandle,BLOCK_TYPE_ZONE,Index);

		pZone->m_Data.SetUnLoadObjectShellHandles(&setHandle);

		pZone->SetObjectLoadState(BLOCK_LOAD_LISTOK);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneBlockLoader::DynamicLoadTerrainSection(KG3DSceneSection* pSection,BOOL bMultiThread)
{
	KGLOG_PROCESS_ERROR(pSection);
	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);

	pSection->SetVisibleCount(g_dwRenderCount);

	if (pSection->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
	{
		POINT Index = { pSection->m_nIndexX, pSection->m_nIndexZ };

		BOOL bTerrain = m_OutDoor.m_lpDataInterface->GetTerrainBlock(BLOCK_TYPE_SECTION,Index);

		if(bTerrain)
		{
			pSection->CreateTerrainBlock();

			vector<DWORD>vecDataType;
			m_OutDoor.m_lpDataInterface->GetAllTerrainBlockDataType(BLOCK_TYPE_SECTION,Index,&vecDataType);

			for (size_t i=0;i<vecDataType.size();i++)
			{
				DWORD dwType = vecDataType[i];
				pSection->m_lpTerrainBlock->OnTerrainBlockModified(dwType,m_OutDoor.m_lpDataInterface,&m_OutDoor,bMultiThread);
			}
			pSection->ComputeBBox(TRUE);
			pSection->m_lpTerrainBlock->m_bLoaded = TRUE;
			//pSection->m_lpTerrainBlock->CheckWaterVisible();//这个要在m_bLoaded设为TRUE后，才调。不然取水面高度和地表高度来判断水是否可见就不正确了
		}

		pSection->SetTerrainLoadState(BLOCK_LOAD_TERRAIN_FINISH);
	}

	if (pSection->GetObjectLoadState() < BLOCK_LOAD_LISTOK)
	{
		POINT Index = { pSection->m_nIndexX, pSection->m_nIndexZ };

		set<UINT>setHandle;
		m_OutDoor.m_lpDataInterface->GetEntityOnSceneBlock(&setHandle,BLOCK_TYPE_SECTION,Index);
		pSection->m_Data.SetUnLoadObjectShellHandles(&setHandle);

		pSection->SetObjectLoadState(BLOCK_LOAD_LISTOK);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::DynamicLoadTerrainRegion(KG3DSceneRegion* pRegion,BOOL bMultiThread)
{
	KGLOG_PROCESS_ERROR(pRegion);

	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);
	
	pRegion->SetVisibleCount(g_dwRenderCount);

	if(pRegion->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
	{
		POINT Index = { pRegion->m_nIndexX, pRegion->m_nIndexZ };

		BOOL bTerrain = m_OutDoor.m_lpDataInterface->GetTerrainBlock(BLOCK_TYPE_REGION,Index);

		if(bTerrain && !pRegion->m_lpTerrainBlock)
		{

			pRegion->CreateTerrainBlock();

			vector<DWORD>vecDataType;
			m_OutDoor.m_lpDataInterface->GetAllTerrainBlockDataType(BLOCK_TYPE_REGION,Index,&vecDataType);

			for (size_t i=0;i<vecDataType.size();i++)
			{
				DWORD dwType = vecDataType[i];
				pRegion->m_lpTerrainBlock->OnTerrainBlockModified(dwType,m_OutDoor.m_lpDataInterface,&m_OutDoor,bMultiThread);
			}

			pRegion->ComputeBBox(TRUE);
			pRegion->m_lpTerrainBlock->m_bLoaded = TRUE;
			//pRegion->m_lpTerrainBlock->CheckWaterVisible();//这个要在m_bLoaded设为TRUE后，才调。不然取水面高度和地表高度来判断水是否可见就不正确了
		}

		pRegion->SetTerrainLoadState(BLOCK_LOAD_TERRAIN_FINISH);
		{
			UINT SectionIndex_X = pRegion->m_nIndexX / SCENESECTIONNUMREGION;
			UINT SectionIndex_Z = pRegion->m_nIndexZ / SCENESECTIONNUMREGION;

			KG3DSceneSection* pSection = m_OutDoor.GetSection(SectionIndex_X,SectionIndex_Z);
			if(pSection)
			{
				pSection->CheckRegionLoad();
			}
		}
	}

	if(pRegion->GetObjectLoadState() < BLOCK_LOAD_LISTOK)
	{
		POINT Index = { pRegion->m_nIndexX, pRegion->m_nIndexZ };

		set<UINT>setHandle;
		m_OutDoor.m_lpDataInterface->GetEntityOnSceneBlock(&setHandle,BLOCK_TYPE_REGION,Index);
		pRegion->m_Data.SetUnLoadObjectShellHandles(&setHandle);

		pRegion->SetObjectLoadState(BLOCK_LOAD_LISTOK);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::ComputeLoadZoneByRect(LoadList* pList,D3DXVECTOR2& A_Zone,D3DXVECTOR2& C_Zone,
													D3DXVECTOR2& A_Section,D3DXVECTOR2& C_Section,
													D3DXVECTOR2& A_Region,D3DXVECTOR2& C_Region)
{
	int XStartZoneIndex = 0;
	int XEndZoneIndex = 0;
	int ZStartZoneIndex =0;
	int ZEndZoneIndex = 0;
	int X,Z;
	KG3DSceneZone* pZone = NULL;

	//////////////////////////////////////////////////////////////////////////
	pList->CleanUp();

	XStartZoneIndex = GetZoneIndex(A_Zone.x);
	ZStartZoneIndex = GetZoneIndex(A_Zone.y);
	XEndZoneIndex   = GetZoneIndex(C_Zone.x);
	ZEndZoneIndex   = GetZoneIndex(C_Zone.y);

	for (Z = ZStartZoneIndex; Z <= ZEndZoneIndex; Z++)
	{
		for (X = XStartZoneIndex; X <= XEndZoneIndex; X++)
		{
			pZone = m_OutDoor.GetZone(X,Z);

			if(pZone)
			{
				pZone->SetVisibleCount(g_dwRenderCount);

				if (pZone->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
					pList->m_listTerrainZoneToLoad.push_back(pZone);

				if (pZone->GetObjectLoadState() < BLOCK_LOAD_ALL_FINISH)
					pList->m_listSceneObjectZoneToLoad.push_back(pZone);
			}

			if(pZone)
			{
				ComputeLoadSectionByRect(pList,A_Section,C_Section,A_Region,C_Region,pZone);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	return S_OK;

}

HRESULT KG3DSceneBlockLoader::ComputeLoadSectionByRect(LoadList* pList,D3DXVECTOR2& A_Section,D3DXVECTOR2& C_Section,
													   D3DXVECTOR2& A_Region,D3DXVECTOR2& C_Region,
													   KG3DSceneZone* pZone)
{
	KGLOG_PROCESS_ERROR(pZone);
	//////////////////////////////////////////////////////////////////////////
	//创建section
	for (int nZ = 0;nZ < SCENESECTIONNUMREGION ;nZ++)
	{
		for (int nX = 0;nX < SCENESECTIONNUMREGION ;nX++)
		{
			int nIndexSectionX = pZone->GetIndexX() * SCENESECTIONNUMREGION + nX;
			int nIndexSectionZ = pZone->GetIndexZ() * SCENESECTIONNUMREGION + nZ;

			D3DXVECTOR4 rect = GetSceneBlockRect(BLOCK_TYPE_SECTION,nIndexSectionX,nIndexSectionZ);
			BOOL bInRect = IsIntersectBlockRect(rect,A_Section,C_Section);

			if(bInRect)
			{
				//POINT SectionIndex = { nIndexSectionX, nIndexSectionZ };

				KG3DSceneSection* pSection = m_OutDoor.GetSection(nIndexSectionX,nIndexSectionZ);

				/*if(!pSection)
				{
					BOOL bExist = m_OutDoor.m_lpDataInterface->GetSceneBlock(BLOCK_TYPE_SECTION,SectionIndex);

					if(bExist)
					{
						pZone->NewOneSection(&pSection,nIndexSectionX,nIndexSectionZ,nX,nZ);
					}
				}*/

				if(pSection)
				{
					pSection->SetVisibleCount(g_dwRenderCount);

					if(pSection->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
						pList->m_listTerrainSectionToLoad.push_back(pSection);

					if (pSection->GetObjectLoadState() < BLOCK_LOAD_ALL_FINISH)
						pList->m_listSceneObjectSectionToLoad.push_back(pSection);

					//if(pSection->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
					{
						ComputeLoadRegionByRect(pList,A_Region,C_Region,pSection);
					}
				}

			}
		}
	}
	//////////////////////////////////////////////////////////////////////////


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::ComputeLoadRegionByRect(LoadList* pList,D3DXVECTOR2& A,D3DXVECTOR2& C,KG3DSceneSection* pSection)
{
	KGLOG_PROCESS_ERROR(pSection);
	//////////////////////////////////////////////////////////////////////////
	for (int nZ = 0;nZ < SCENESECTIONNUMREGION ;nZ++)
	{
		for (int nX = 0;nX < SCENESECTIONNUMREGION ;nX++)
		{
			int nIndexRegionX = pSection->m_nIndexX * SCENESECTIONNUMREGION + nX;
			int nIndexRegionZ = pSection->m_nIndexZ * SCENESECTIONNUMREGION + nZ;

			D3DXVECTOR4 rect = GetSceneBlockRect(BLOCK_TYPE_REGION,nIndexRegionX,nIndexRegionZ);
			BOOL bInRect = IsIntersectBlockRect(rect,A,C);

			if(bInRect)
			{
				//POINT RegionIndex = { nIndexRegionX, nIndexRegionZ };

				KG3DSceneRegion* pRegion = m_OutDoor.GetRegion(nIndexRegionX,nIndexRegionZ);
				/*if(!pRegion)
				{
					BOOL bExist = m_OutDoor.m_lpDataInterface->GetSceneBlock(BLOCK_TYPE_REGION,RegionIndex);

					if(bExist)
					{
						pSection->NewOneRegion(&pRegion,nIndexRegionX,nIndexRegionZ,nX,nZ);
					}
				}*/

				if(pRegion )
				{
					pRegion->SetVisibleCount(g_dwRenderCount);

					if(pRegion->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
						pList->m_listTerrainRegionToLoad.push_back(pRegion);

					if (pRegion->GetObjectLoadState() < BLOCK_LOAD_ALL_FINISH)
						pList->m_listSceneObjectRegionToLoad.push_back(pRegion);
				}
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////


	return S_OK;
Exit0:
	return E_FAIL;
}

//HRESULT KG3DSceneBlockLoader::TryPushLoadList(BOOL bMultThread)
//{
//	//CleanUp();
//
//	if(bMultThread && g_eEngineOption.bUseMultiThreadLoad)
//	{
//		SceneBlockList* pList = NULL;
//		SceneBlockList* pListNext = NULL;
//		if(SUCCEEDED(g_cSceneBlockList.FindList_NoData(pList,pListNext)))
//		{
//			if(pList)
//			{
//				{
//					list<KG3DSceneRegion*>::iterator i = m_listTerrainRegionToLoad.begin();
//					list<KG3DSceneRegion*>::iterator iend = m_listTerrainRegionToLoad.end();
//					while (i!=iend)
//					{
//						KG3DSceneRegion* pRegion = *i;
//						if(pRegion->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
//						{
//							pList->m_listTerrainRegion.push_back(pRegion);
//						}
//						++i;
//					}
//				}
//				//////////////////////////////////////////////////////////////////////////
//				{
//					list<KG3DSceneSection*>::iterator i = m_listTerrainSectionToLoad.begin();
//					list<KG3DSceneSection*>::iterator iend = m_listTerrainSectionToLoad.end();
//					while (i!=iend)
//					{
//						KG3DSceneSection* pSection = *i;
//						if(pSection->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
//						{
//							pList->m_listTerrainSection.push_back(pSection);
//						}
//						++i;
//					}
//				}
//				//////////////////////////////////////////////////////////////////////////
//				{
//					list<KG3DSceneZone*>::iterator i = m_listTerrainZoneToLoad.begin();
//					list<KG3DSceneZone*>::iterator iend = m_listTerrainZoneToLoad.end();
//					while (i!=iend)
//					{
//						KG3DSceneZone* pZone = *i;
//						if(pZone->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
//						{
//							pList->m_listTerrainZone.push_back(pZone);
//						}
//						++i;
//					}
//				}
//				//////////////////////////////////////////////////////////////////////////
//				//////////////////////////////////////////////////////////////////////////
//				{
//					list<KG3DSceneRegion*>::iterator i = m_listSceneObjectRegionToLoad.begin();
//					list<KG3DSceneRegion*>::iterator iend = m_listSceneObjectRegionToLoad.end();
//					while (i!=iend)
//					{
//						KG3DSceneRegion* pRegion = *i;
//						if(pRegion->GetObjectLoadState() < BLOCK_LOAD_ALL_FINISH)
//						{
//							pList->m_listSceneRegion.push_back(pRegion);
//						}
//						++i;
//					}
//				}
//				//////////////////////////////////////////////////////////////////////////
//				{
//					list<KG3DSceneSection*>::iterator i = m_listSceneObjectSectionToLoad.begin();
//					list<KG3DSceneSection*>::iterator iend = m_listSceneObjectSectionToLoad.end();
//					while (i!=iend)
//					{
//						KG3DSceneSection* pSection = *i;
//						if(pSection->GetObjectLoadState() < BLOCK_LOAD_ALL_FINISH)
//						{
//							pList->m_listSceneSection.push_back(pSection);
//						}
//						++i;
//					}
//				}
//				//////////////////////////////////////////////////////////////////////////
//				{
//					list<KG3DSceneZone*>::iterator i = m_listSceneObjectZoneToLoad.begin();
//					list<KG3DSceneZone*>::iterator iend = m_listSceneObjectZoneToLoad.end();
//					while (i!=iend)
//					{
//						KG3DSceneZone* pZone = *i;
//						if(pZone->GetObjectLoadState() < BLOCK_LOAD_ALL_FINISH)
//						{
//							pList->m_listSceneZone.push_back(pZone);
//						}
//						++i;
//					}
//				}
//
//
//				pList->m_vecObjectShell = m_vecObjectShellToLoad;
//
//				pList->m_lpSceneBlockLoader = this;
//				m_OutDoor.AddRef();
//
//				pList->m_dwState = BLOCK_LIST_STATE_DATAOK;
//			}
//		}
//
//		m_listTerrainRegionToLoad.clear();
//		m_listTerrainSectionToLoad.clear();
//		m_listTerrainZoneToLoad.clear();
//
//		m_listSceneObjectRegionToLoad.clear();
//		m_listSceneObjectSectionToLoad.clear();
//		m_listSceneObjectZoneToLoad.clear();
//		m_vecObjectShellToLoad.clear();
//	}
//
//
//
//	return S_OK;
//}

HRESULT KG3DSceneBlockLoader::SetLoadRect(D3DXVECTOR2& A_Zone,D3DXVECTOR2& C_Zone,
										  D3DXVECTOR2& A_Section,D3DXVECTOR2& C_Section,
										  D3DXVECTOR2& A_Region,D3DXVECTOR2& C_Region,BOOL bMultThread)
{
	
	D3DXVECTOR2 vSceneA,vSceneC;
	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);

	LoadList* pList = &m_LoadList;
	if(bMultThread)
		pList = &m_LoadListMultiThread;

 	ComputeLoadZoneByRect(pList,A_Zone,C_Zone,A_Section,C_Section,A_Region,C_Region);

	m_OutDoor.FindUnLoadedObjectShellInRect(&pList->m_setObjectShellToLoad,A_Section,C_Section);

	if(bMultThread)
		pList->Sort(m_DynamicLoadPositionNow,m_DynamicLoadDirection);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::ForceLoad()
{
	while(!m_LoadListMultiThread.IsEmpty())
	{
		Sleep(100);
	}

	/*while (unload)
	{
		FrameMove(4096);
		unload = m_mapObjectShellMultiThreadLoaded.GetSize();
	}*/

	//<加载计数>
	static KG3DMessageSceneLoadingPercent PercentStruct;
	ZeroMemory(&PercentStruct, sizeof(PercentStruct));
	size_t TotalTerrainLoadDemand = 0, CurTerrainLoaded = 0;
	size_t TotalObjectLoadDemand = 0, CurObjLoaded = 0;
	KG3DMessage MessageToSend(KM_SCENE_LOADING_PERCENT, (DWORD_PTR)&PercentStruct, NULL);
	KG3DMessageSender& Sender = m_OutDoor.GetSceneMessageSender(KM_LOADING_PERCENT);
	BOOL bSentPercent = Sender.GetReceiverCount() > 0;
	enum
	{
		em_send_message_step = 10,
	};
	if (bSentPercent)
	{
		TotalTerrainLoadDemand = m_LoadList.m_listTerrainZoneToLoad.size() 
			+ m_LoadList.m_listTerrainSectionToLoad.size() + m_LoadList.m_listTerrainRegionToLoad.size();

		TotalObjectLoadDemand = m_LoadList.m_listSceneObjectZoneToLoad.size()+ m_LoadList.m_listSceneObjectSectionToLoad.size()
			+ m_LoadList.m_listSceneObjectRegionToLoad.size() + m_LoadList.m_setObjectShellToLoad.size();
	}
	//</加载计数>
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneZone*>::iterator i = m_LoadList.m_listTerrainZoneToLoad.begin();
		list<KG3DSceneZone*>::iterator iend = m_LoadList.m_listTerrainZoneToLoad.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;

			while (pZone->GetTerrainLoadState() != BLOCK_LOAD_TERRAIN_FINISH)
			{
				DynamicLoadTerrainZone(pZone,FALSE);
			}
			++i;
			//<加载计数>
			if(bSentPercent)
			{
				++CurTerrainLoaded;
				if(CurTerrainLoaded % em_send_message_step == 0)
				{
					PercentStruct.TerrainLoadingPercent = (FLOAT)CurTerrainLoaded / (FLOAT)TotalTerrainLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
			//</加载计数>
		}
		m_LoadList.m_listTerrainZoneToLoad.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneSection*>::iterator i = m_LoadList.m_listTerrainSectionToLoad.begin();
		list<KG3DSceneSection*>::iterator iend = m_LoadList.m_listTerrainSectionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneSection* pSection = *i;

			while (pSection->GetTerrainLoadState() != BLOCK_LOAD_TERRAIN_FINISH)
			{
				DynamicLoadTerrainSection(pSection,FALSE);
			}
			++i;
			//<加载计数>
			if(bSentPercent)
			{
				++CurTerrainLoaded;
				if(CurTerrainLoaded % em_send_message_step == 0)
				{
					PercentStruct.TerrainLoadingPercent = (FLOAT)CurTerrainLoaded / (FLOAT)TotalTerrainLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
			//</加载计数>
		}
		m_LoadList.m_listTerrainSectionToLoad.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneRegion*>::iterator i = m_LoadList.m_listTerrainRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = m_LoadList.m_listTerrainRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;

			while (pRegion->GetTerrainLoadState() != BLOCK_LOAD_TERRAIN_FINISH)
			{
				DynamicLoadTerrainRegion(pRegion,FALSE);
			}
			++i;
			//<加载计数>
			if(bSentPercent)
			{
				++CurTerrainLoaded;
				if(CurTerrainLoaded % em_send_message_step == 0)
				{
					PercentStruct.TerrainLoadingPercent = (FLOAT)CurTerrainLoaded / (FLOAT)TotalTerrainLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
			//</加载计数>
		}
		m_LoadList.m_listTerrainRegionToLoad.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneZone*>::iterator i = m_LoadList.m_listSceneObjectZoneToLoad.begin();
		list<KG3DSceneZone*>::iterator iend = m_LoadList.m_listSceneObjectZoneToLoad.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;
			while (pZone->GetObjectLoadState() != BLOCK_LOAD_ALL_FINISH)
			{
				DynamicLoadObjectZone(pZone,FALSE);
			}
			++i;
			//<加载计数>
			if(bSentPercent)
			{
				++CurObjLoaded;
				if(CurObjLoaded % em_send_message_step == 0)
				{
					PercentStruct.ObjectLoadingPercent = (FLOAT)CurObjLoaded / (FLOAT)TotalObjectLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
			//</加载计数>
		}
		m_LoadList.m_listSceneObjectZoneToLoad.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneSection*>::iterator i = m_LoadList.m_listSceneObjectSectionToLoad.begin();
		list<KG3DSceneSection*>::iterator iend = m_LoadList.m_listSceneObjectSectionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneSection* pSection = *i;
			while (pSection->GetObjectLoadState() != BLOCK_LOAD_ALL_FINISH)
			{
				DynamicLoadObjectSection(pSection,FALSE);
			}
			++i;
			//<加载计数>
			if(bSentPercent)
			{
				++CurObjLoaded;
				if(CurObjLoaded % em_send_message_step == 0)
				{
					PercentStruct.ObjectLoadingPercent = (FLOAT)CurObjLoaded / (FLOAT)TotalObjectLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
			//</加载计数>
		}
		m_LoadList.m_listSceneObjectSectionToLoad.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneRegion*>::iterator i = m_LoadList.m_listSceneObjectRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = m_LoadList.m_listSceneObjectRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;
			while (pRegion->GetObjectLoadState() != BLOCK_LOAD_ALL_FINISH)
			{
				DynamicLoadObjectRegion(pRegion,FALSE);
			}
			++i;
			if(bSentPercent)
			{
				++CurObjLoaded;
				if(CurObjLoaded % em_send_message_step == 0)
				{
					PercentStruct.ObjectLoadingPercent = (FLOAT)CurObjLoaded / (FLOAT)TotalObjectLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
		}
		m_LoadList.m_listSceneObjectRegionToLoad.clear();
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	{	//把被动态御截掉的物件加载 add by huangjinshou 2009-4-24
		DWORD dwOption = OBJLOAD_NONE;
		set<KG3DEntityObjectShell*>::iterator i = m_LoadList.m_setObjectShellToLoad.begin();
		set<KG3DEntityObjectShell*>::iterator iend = m_LoadList.m_setObjectShellToLoad.end();
		while(i!=iend)
		{
			KG3DEntityObjectShell* pShell = *i;
			if (pShell)
			{
				pShell->TryDynamicLoad(dwOption);
			}
			++i;
			//<加载计数>
			if(bSentPercent)
			{
				++CurObjLoaded;
				if(CurObjLoaded % em_send_message_step == 0)
				{
					PercentStruct.ObjectLoadingPercent = (FLOAT)CurObjLoaded / (FLOAT)TotalObjectLoadDemand;
					Sender.PostMessage(MessageToSend);
				}
			}
			//</加载计数>
		}
		m_LoadList.m_setObjectShellToLoad.clear();
	}

	return S_OK;
}

HRESULT KG3DSceneBlockLoader::FrameMove()
{
	KGLOG_PROCESS_ERROR(m_OutDoor.m_bPauseMultiThreadLoad);

	ProcessDynamicLoadTerrain();
	ProcessDynamicLoadObject();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::ProcessDynamicLoadTerrain()
{
	//if(g_eEngineOption.bUseMultiThreadLoad)
	//	return S_OK;

	m_dwEntityLoadCount = 0;
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneZone*>::iterator i = m_LoadList.m_listTerrainZoneToLoad.begin();
		list<KG3DSceneZone*>::iterator iend = m_LoadList.m_listTerrainZoneToLoad.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;
			if(m_dwEntityLoadCount <= m_dwLoadLimit)
			{
				DynamicLoadTerrainZone(pZone,FALSE);

				m_dwEntityLoadCount++;

				if(pZone->GetTerrainLoadState() == BLOCK_LOAD_TERRAIN_FINISH )
					i = m_LoadList.m_listTerrainZoneToLoad.erase(i);
				else
					++i;
			}
			else
				break;
		}
	}

	if(m_dwEntityLoadCount>m_dwLoadLimit)
		return S_OK;
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneSection*>::iterator i = m_LoadList.m_listTerrainSectionToLoad.begin();
		list<KG3DSceneSection*>::iterator iend = m_LoadList.m_listTerrainSectionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneSection* pSection = *i;
			if(m_dwEntityLoadCount <= m_dwLoadLimit)
			{
				DynamicLoadTerrainSection(pSection,FALSE);

				m_dwEntityLoadCount++;

				if(pSection->GetTerrainLoadState() == BLOCK_LOAD_TERRAIN_FINISH )
					i = m_LoadList.m_listTerrainSectionToLoad.erase(i);
				else 
					++i;
			}
			else
				break;
		}
	}

	if(m_dwEntityLoadCount>m_dwLoadLimit)
		return S_OK;

	//////////////////////////////////////////////////////////////////////////
	//if(!g_eEngineOption.bUseMultiThreadLoad)
	{
		list<KG3DSceneRegion*>::iterator i = m_LoadList.m_listTerrainRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = m_LoadList.m_listTerrainRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;
			if(m_dwEntityLoadCount <= m_dwLoadLimit)
			{
				DynamicLoadTerrainRegion(pRegion,FALSE);

				m_dwEntityLoadCount++;

				if(pRegion->GetTerrainLoadState() == BLOCK_LOAD_TERRAIN_FINISH )
					i = m_LoadList.m_listTerrainRegionToLoad.erase(i);
				else
					++i;
			}
			else
				break;
		}
	}

	return S_OK;
}

HRESULT KG3DSceneBlockLoader::ProcessDynamicLoadObject()
{
	//if(g_eEngineOption.bUseMultiThreadLoad)
	//	return S_OK;

	DWORD dwNowTime = timeGetTime();

	m_dwEntityLoadCount = 0;
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneZone*>::iterator i = m_LoadList.m_listSceneObjectZoneToLoad.begin();
		list<KG3DSceneZone*>::iterator iend = m_LoadList.m_listSceneObjectZoneToLoad.end();
		while (i!=iend)
		{
			KG3DSceneZone* pZone = *i;
			if(m_dwEntityLoadCount <= m_dwLoadLimit)
			{
				DynamicLoadObjectZone(pZone,FALSE);

				m_dwEntityLoadCount++;

				if(pZone->GetObjectLoadState() == BLOCK_LOAD_ALL_FINISH )
					i = m_LoadList.m_listSceneObjectZoneToLoad.erase(i);
				else
					++i;
			}
			else
				break;
		}
	}

	if(m_dwEntityLoadCount>m_dwLoadLimit)
		return S_OK;
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneSection*>::iterator i = m_LoadList.m_listSceneObjectSectionToLoad.begin();
		list<KG3DSceneSection*>::iterator iend = m_LoadList.m_listSceneObjectSectionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneSection* pSection = *i;
			if(m_dwEntityLoadCount <= m_dwLoadLimit)
			{
				DynamicLoadObjectSection(pSection,FALSE);

				m_dwEntityLoadCount++;

				if(pSection->GetObjectLoadState() == BLOCK_LOAD_ALL_FINISH )
					i = m_LoadList.m_listSceneObjectSectionToLoad.erase(i);
				else 
					++i;
			}
			else
				break;
		}
	}

	if(m_dwEntityLoadCount>m_dwLoadLimit)
		return S_OK;

	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneRegion*>::iterator i = m_LoadList.m_listSceneObjectRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = m_LoadList.m_listSceneObjectRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;
			if(m_dwEntityLoadCount <= m_dwLoadLimit)
			{
				DynamicLoadObjectRegion(pRegion,FALSE);

				m_dwEntityLoadCount++;

				if(pRegion->GetObjectLoadState() == BLOCK_LOAD_ALL_FINISH )
					i = m_LoadList.m_listSceneObjectRegionToLoad.erase(i);
				else
					++i;
			}
			else
				break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwOption = OBJLOAD_NONE;

		set<KG3DEntityObjectShell*>::iterator i = m_LoadList.m_setObjectShellToLoad.begin();
		set<KG3DEntityObjectShell*>::iterator iend = m_LoadList.m_setObjectShellToLoad.end();
		while(i!=iend)
		{
			KG3DEntityObjectShell* pShell = *i;
			if (pShell)
			{
				pShell->TryDynamicLoad(dwOption);
			}
			++i;
		}
		m_LoadList.m_setObjectShellToLoad.clear();
	}
	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 50)
	{
		//KGLogPrintf(KGLOG_ERR, "%d KG3DSceneBlockLoader::ProcessDynamicLoadObject.",dwCost);
	}
	return S_OK;
}

HRESULT KG3DSceneBlockLoader::DynamicLoadObjectZone(KG3DSceneZone* pZone,BOOL bMultiThread)
{
	KGLOG_PROCESS_ERROR(pZone);
	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);

	pZone->SetVisibleCount(g_dwRenderCount);

	DWORD dwOption = OBJLOAD_NONE;
	if(bMultiThread)
		dwOption = OBJLOAD_MULTITHEAD;

	if(pZone->GetObjectLoadState() >= BLOCK_LOAD_LISTOK)
	{
		DWORD dwCount = pZone->m_Data.GetUnLoadObjectShellHandlesSize();
		if(dwCount)
		{
			m_OutDoor.DoDynamicLoadEntityContainer(pZone,TRUE,dwOption);
		}

		dwCount =  pZone->m_Data.GetUnLoadObjectShellHandlesSize();

		if(dwCount == 0)
			pZone->SetObjectLoadState(BLOCK_LOAD_ALL_FINISH);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneBlockLoader::DynamicLoadObjectSection(KG3DSceneSection* pSection,BOOL bMultiThread)
{
	KGLOG_PROCESS_ERROR(pSection);
	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);
	
	pSection->SetVisibleCount(g_dwRenderCount);

	DWORD dwOption = OBJLOAD_NONE;
	if(bMultiThread)
		dwOption = OBJLOAD_MULTITHEAD;

	if (pSection->GetObjectLoadState() >= BLOCK_LOAD_LISTOK)
	{
		//////////////////////////////////////////////////////////////////////////
		DWORD dwCount = pSection->m_Data.GetUnLoadObjectShellHandlesSize();
		if(dwCount)
		{
			m_OutDoor.DoDynamicLoadEntityContainer(pSection,TRUE,dwOption);
		}

		dwCount = pSection->m_Data.GetUnLoadObjectShellHandlesSize();
		if(dwCount == 0)
			pSection->SetObjectLoadState(BLOCK_LOAD_ALL_FINISH);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::DynamicLoadObjectRegion(KG3DSceneRegion* pRegion,BOOL bMultiThread)
{
	KGLOG_PROCESS_ERROR(pRegion);

	KGLOG_PROCESS_ERROR(m_OutDoor.m_lpDataInterface);

	pRegion->SetVisibleCount(g_dwRenderCount);

	DWORD dwOption = OBJLOAD_NONE;
	if(bMultiThread)
		dwOption = OBJLOAD_MULTITHEAD;

	if(pRegion->GetObjectLoadState() >= BLOCK_LOAD_LISTOK)
	{
		//////////////////////////////////////////////////////////////////////////
		DWORD dwCount = pRegion->m_Data.GetUnLoadObjectShellHandlesSize();
		if(dwCount)
		{
			m_OutDoor.DoDynamicLoadEntityContainer(pRegion,TRUE,dwOption);
		}

		dwCount = pRegion->m_Data.GetUnLoadObjectShellHandlesSize();
		if(dwCount == 0)
			pRegion->SetObjectLoadState(BLOCK_LOAD_ALL_FINISH);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockLoader::OnMultiThreadEnd()
{
	m_OutDoor.Release();
	return S_OK;
}

HRESULT KG3DSceneBlockLoader::LoadList::CleanUp()
{
	m_listSceneObjectZoneToLoad.clear();
	m_listSceneObjectSectionToLoad.clear();
	m_listSceneObjectRegionToLoad.clear();

	m_listTerrainZoneToLoad.clear();
	m_listTerrainSectionToLoad.clear();
	m_listTerrainRegionToLoad.clear();
	
	m_setObjectShellToLoad.clear();
	//TryPushLoadList(TRUE);

	return S_OK;
}
BOOL KG3DSceneBlockLoader::LoadList::IsEmpty()
{
	if(m_listSceneObjectZoneToLoad.size() > 0 )
		return FALSE;
	if (m_listSceneObjectSectionToLoad.size()> 0)
		return FALSE;
	if (m_listSceneObjectRegionToLoad.size()> 0)
		return FALSE;
	if (m_listTerrainZoneToLoad.size()> 0)
		return FALSE;
	if (m_listTerrainSectionToLoad.size()> 0)
		return FALSE;
	if (m_listTerrainRegionToLoad.size()> 0)
		return FALSE;
	if (m_setObjectShellToLoad.size()>0)
		return FALSE;
	return TRUE;
}
HRESULT KG3DSceneBlockLoader::CleanUp()
{
	m_LoadList.CleanUp();
	m_LoadListMultiThread.CleanUp();
	return S_OK;
}

HRESULT KG3DSceneBlockLoader::LoadTerrainBlockByPos(float fx,float fz)
{
	KG3DSceneZone* pZone = NULL;
	KG3DSceneSection* pSection = NULL;
	KG3DSceneRegion* pRegion = NULL;
	int nZoneX    =	GetZoneIndex(fx);
	int nZoneZ    =	GetZoneIndex(fz); 
	int nSectionX = GetSceneSectionIndex(fx);
	int nSectionZ = GetSceneSectionIndex(fz);
	int nRegionX  = GetSceneRegionIndex(fx);
	int nRegionZ  = GetSceneRegionIndex(fz);
	
	m_OutDoor.PauseMultiThreadLoad(TRUE);

	pZone = m_OutDoor.GetZone(nZoneX,nZoneZ);
	if (pZone)
	{
		while (pZone->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
		{
			DynamicLoadTerrainZone(pZone,FALSE);
		}
	}

	pSection = m_OutDoor.GetSection(nSectionX,nSectionZ);
	if (pSection)
	{
		while (pSection->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
		{
			DynamicLoadTerrainSection(pSection,FALSE);
		}
	}

	pRegion = m_OutDoor.GetRegion(nRegionX,nRegionZ);
	if (pRegion)
	{
		while (pRegion->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
		{
			DynamicLoadTerrainRegion(pRegion,FALSE);
		}
	}

	m_OutDoor.PauseMultiThreadLoad(FALSE);
	return S_OK;
}

DWORD KG3DSceneBlockLoader::GetLoadRect(D3DXVECTOR2 vPos, D3DXVECTOR2& vSectionA,D3DXVECTOR2& vSectionC,D3DXVECTOR2& vRegionA,D3DXVECTOR2& vRegionC)
{
	//DWORD dwTime = timeGetTime();
	DWORD dwSign = 0;
	//D3DXVECTOR2 vPos = m_DynamicLoadPositionNow;
	D3DXVECTOR2 vDir = m_DynamicLoadDirection;
	float fRadian = atan(vDir.y / vDir.x);
	const float fVerge = D3DX_PI/4.0f;
	const float fRange = 12000.0f;
	//m_fRSection = 30000.f;
	if ( MT_FLOAT_EQUAL(vDir.x , 0.0f) )
	{
		if (vDir.y > 0)
			dwSign = 1;
		else if(vDir.y < 0)
			dwSign = 3;		
	}
	else if (vDir.x > 0 && vDir.y >= 0)
	{
		if(fRadian >= fVerge)
			dwSign = 1;
		else
			dwSign = 4;
	}
	else if (vDir.x < 0 && vDir.y >= 0)
	{
		if(fRadian >= -fVerge)
			dwSign = 2;
		else
			dwSign = 1;
	}
	else if (vDir.x < 0 && vDir.y <= 0)
	{
		if(fRadian >= fVerge)
			dwSign = 3;
		else
			dwSign = 2;
	}
	else if (vDir.x > 0 && vDir.y <= 0)
	{
		if(fRadian >= -fVerge)
			dwSign = 4;
		else
			dwSign = 3;
	}

	switch(dwSign)
	{
	case 1:
		{
			vRegionA = vPos - D3DXVECTOR2(m_fRRegion,fRange);
			vRegionC = vPos + D3DXVECTOR2(m_fRRegion,m_fRRegion);

			vSectionA = vPos - D3DXVECTOR2(m_fRSection,0);
			vSectionC = vPos + D3DXVECTOR2(m_fRSection,m_fRSection);
		}
		break;
	case 2:
		{
			vRegionA = vPos - D3DXVECTOR2(m_fRRegion,m_fRRegion);
			vRegionC = vPos + D3DXVECTOR2(fRange,m_fRRegion);

			vSectionA = vPos - D3DXVECTOR2(m_fRSection,m_fRSection);
			vSectionC = vPos + D3DXVECTOR2(0,m_fRSection);
		}
		break;
	case 3:
		{
			vRegionA = vPos - D3DXVECTOR2(m_fRRegion,m_fRRegion);
			vRegionC = vPos + D3DXVECTOR2(m_fRRegion,fRange);

			vSectionA = vPos - D3DXVECTOR2(m_fRSection,m_fRSection);
			vSectionC = vPos + D3DXVECTOR2(m_fRSection,0);
		}
	    break;
	case 4:
		{
			vRegionA = vPos - D3DXVECTOR2(fRange,m_fRRegion);
			vRegionC = vPos + D3DXVECTOR2(m_fRRegion,m_fRRegion);

			vSectionA = vPos - D3DXVECTOR2(0,m_fRSection);
			vSectionC = vPos + D3DXVECTOR2(m_fRSection,m_fRSection);
		}
	    break;
	default:
		{
			vRegionA = vPos;
			vRegionC = vPos;

			vSectionA = vPos - D3DXVECTOR2(m_fRSection,m_fRSection);
			vSectionC = vPos + D3DXVECTOR2(m_fRSection,m_fRSection);
		}
	    break;
	}
	//DWORD dwTimeCount = timeGetTime() -dwTime;
	//ATLTRACE("GetLoadRect Use Time : %i\n",dwTimeCount);
	return dwSign;
}
//#pragma push_macro("D3DXPI")
//#if defined(D3DX_PI)
//#undef D3DX_PI
//#define D3DX_PI ((float)3.1415926535f)
//#endif
//
//#if defined(KG_ENABLE_TDD)
//KG_TEST_BEGIN(KG3DSceneBlockLoader)
//{
//	{
//		KG3DSceneOutDoorSpaceMgr* pMgr = reinterpret_cast<KG3DSceneOutDoorSpaceMgr*>(0x00000001);
//
//		KG3DSceneBlockLoader TempLoader(*pMgr);//创建一个假的引用给构造函数，只要没有用到就不会出事。
//		D3DXVECTOR2 A,C;
//		D3DXVECTOR2 vDir[17];
//		vDir[0] = D3DXVECTOR2(0,0);														// 0
//		vDir[1] = D3DXVECTOR2(cos(0.0f),sin(0.0f));										// 4
//		vDir[2] = D3DXVECTOR2(cos(D3DX_PI/6.0f),sin(D3DX_PI/6.0f));						// 4
//		vDir[3] = D3DXVECTOR2(cos(D3DX_PI/4.0f),sin(D3DX_PI/4.0f));                     // 1
//		vDir[4] = D3DXVECTOR2(cos(D3DX_PI * 2.0f/6.0f),sin(D3DX_PI * 2.0f/6.0f));       // 1
//		vDir[5] = D3DXVECTOR2(cos(D3DX_PI/2.0f),sin(D3DX_PI/2.0f));                     // 1
//		vDir[6] = D3DXVECTOR2(cos(D3DX_PI * 4.0f /6.0f),sin(D3DX_PI * 4.0f /6.0f));     // 1
//		vDir[7] = D3DXVECTOR2(cos(D3DX_PI * 3.0f /4.0f),sin(D3DX_PI * 3.0f /4.0f));     // 2
//		vDir[8] = D3DXVECTOR2(cos(D3DX_PI * 5.0f /6.0f),sin(D3DX_PI * 5.0f /6.0f));     // 2
//		vDir[9] = D3DXVECTOR2(cos(D3DX_PI),sin(D3DX_PI));                               // 2
//		vDir[10] = D3DXVECTOR2(cos(D3DX_PI * 7.0f /6.0f),sin(D3DX_PI * 7.0f /6.0f));    // 2
//		vDir[11] = D3DXVECTOR2(cos(D3DX_PI * 5.0f /4.0f),sin(D3DX_PI * 5.0f /4.0f));    // 3
//		vDir[12] = D3DXVECTOR2(cos(D3DX_PI * 8.0f /6.0f),sin(D3DX_PI * 8.0f /6.0f));    // 3
//		vDir[13] = D3DXVECTOR2(cos(D3DX_PI * 3.0f /2.0f),sin(D3DX_PI * 3.0f /2.0f));    // 3
//		vDir[14] = D3DXVECTOR2(cos(D3DX_PI * 10.0f /6.0f),sin(D3DX_PI * 10.0f /6.0f));  // 3
//		vDir[15] = D3DXVECTOR2(cos(D3DX_PI * 7.0f /4.0f),sin(D3DX_PI * 7.0f /4.0f));    // 4
//		vDir[16] = D3DXVECTOR2(cos(D3DX_PI * 11.0f /6.0f),sin(D3DX_PI * 1.1f /6.0f));   // 4
//		DWORD dwTest[17] = {0,4,4,1,1,1,1,2,2,2,2,3,3,3,3,4,4} ;
//		DWORD dwSign = 10;
//		TempLoader.m_DynamicLoadPositionNow = D3DXVECTOR2(0,0);
//		for (int i = 0; i< 17;i++)
//		{
//			
//			TempLoader.m_DynamicLoadDirection =vDir[i];
//			DWORD dwSign=0;
//
//			dwSign = TempLoader.GetLoadRect(A,C,1000);
//			_ASSERT(dwSign ==dwTest[i]);
//		}
//		dwSign = 0;
//	}
//
//	
//}
//KG_TEST_END(KG3DSceneBlockLoader)
//#endif
//
//#pragma pop_macro("D3DXPI")
bool SortSceneRegion(KG3DSceneRegion*& regionA,KG3DSceneRegion*& regionB)
{
	return regionA->m_fSortValue < regionB->m_fSortValue;
}

HRESULT KG3DSceneBlockLoader::LoadList::Sort(D3DXVECTOR2& vCameraPos,D3DXVECTOR2& vDirection)
{
	{	
		list<KG3DSceneRegion*>::iterator i = m_listTerrainRegionToLoad.begin();
		list<KG3DSceneRegion*>::iterator iend = m_listTerrainRegionToLoad.end();
		while (i!=iend)
		{
			KG3DSceneRegion* pRegion = *i;
			D3DXVECTOR3 V = pRegion->GetAABBox().GetCenter();
			D3DXVECTOR2 V2D = D3DXVECTOR2(V.x,V.z) - vCameraPos;

			float S = D3DXVec2Dot(&V2D,&vDirection);
			float K =  D3DXVec2LengthSq(&V2D);
			
			if(S<=0)
				K*= 10;

			pRegion->m_fSortValue = K;

			++i;
		}
		m_listTerrainRegionToLoad.sort(SortSceneRegion);
	}
	return S_OK;
}
