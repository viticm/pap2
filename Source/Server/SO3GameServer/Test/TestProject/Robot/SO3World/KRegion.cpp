#include "stdafx.h"
#include "KObjectManager.h"
#include "KRegion.h"
#include "KScene.h"
#include "KDoodad.h"
#include "KSO3World.h"
#include "KSceneObjNode.h"
#include "KMath.h"
#include "KWorldSettings.h"

#define NPC_SYNC_INTERVAL		(GAME_FPS / 8)
#define PLAYER_SYNC_INTERVAL	(GAME_FPS)
#define	DOODAD_SYNC_INTERVAL	(GAME_FPS / 4)


KRegion::KRegion(void)
{
}

KRegion::~KRegion(void)
{
}

BOOL KRegion::Init(KScene* pScene, int nRegionX, int nRegionY)
{
    ASSERT(pScene);
    ASSERT(nRegionX >= 0);
    ASSERT(nRegionY >= 0);

    m_pScene                = pScene;
    m_nRegionX              = nRegionX;
    m_nRegionY              = nRegionY;
    m_nLeftCell             = nRegionX * REGION_GRID_WIDTH;
    m_nBottomCell           = nRegionY * REGION_GRID_HEIGHT;
    m_nLeft                 = m_nLeftCell * CELL_LENGTH;
    m_nBottom               = m_nBottomCell * CELL_LENGTH;

	m_nActive               = 0;

	memset(m_Cells, 0, sizeof(m_Cells));
	memset(m_pConnectRegion, NULL, sizeof(m_pConnectRegion));
	memset(m_dwScriptList, 0, sizeof(m_dwScriptList));

	return true;
}

void KRegion::UnInit()
{
    m_pScene = NULL;
}

BOOL KRegion::ClientLoad()
{
    BOOL                bResult    = false;
    BOOL                bRetCode   = false;
    KClientObjAlloctor  ObjAlloctor;
    char                szFileName[MAX_PATH];

    snprintf(
        szFileName, sizeof(szFileName), "%s/%s/v_%03d/%03d_Region_C.%s", 
        MAP_DIR, m_pScene->m_szName, m_nRegionY, m_nRegionX, MAP_FILE_EXT
    );
    szFileName[sizeof(szFileName) - 1] = '\0';

    bRetCode = LoadTerrainData(szFileName, ObjAlloctor);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


BOOL KRegion::LoadTerrainData(const char cszFileName[], KObjAlloctor& rObjectAlloctor)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    IFile*          piFile              = NULL;
    IKG_Buffer*     piBuffer            = NULL;
    BYTE*           pbyBuffer           = NULL;
    size_t          uFileSize           = 0;
    size_t          uReadBytes          = 0;
    size_t          uLeftBytes          = 0;
    BYTE*           pbyOffset           = NULL;
    KRegionHeader*  pFileHeader         = NULL;
    size_t          uBaseCellInfoSize   = sizeof(KCell::KBaseInfo) + sizeof(WORD);
    KCell*          pAllocCell          = NULL;
    int             nExtCellCount       = 0;
    size_t          uExtCellInfoSize    = sizeof(int) * 2 + sizeof(KCell::KBaseInfo) + sizeof(WORD) * 2;

    piFile = g_OpenFile(cszFileName);
    KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    piBuffer = KG_MemoryCreateBuffer((unsigned)uFileSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

	uReadBytes = piFile->Read(pbyBuffer, (unsigned long)uFileSize);
	KGLOG_PROCESS_ERROR(uReadBytes == uFileSize);

    KG_COM_RELEASE(piFile);

    pbyOffset  = pbyBuffer;
    uLeftBytes = uReadBytes;

    KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(KRegionHeader));
	pFileHeader = (KRegionHeader*)pbyOffset;
	pbyOffset  += sizeof(KRegionHeader);
    uLeftBytes -= sizeof(KRegionHeader);

	KGLOG_PROCESS_ERROR(pFileHeader->nRegionX == m_nRegionX);
	KGLOG_PROCESS_ERROR(pFileHeader->nRegionY == m_nRegionY);

    KGLOG_PROCESS_ERROR(uLeftBytes >= uBaseCellInfoSize * REGION_GRID_WIDTH * REGION_GRID_HEIGHT);
    uLeftBytes -= uBaseCellInfoSize * REGION_GRID_WIDTH * REGION_GRID_HEIGHT;

	for (int nCellY = 0; nCellY < REGION_GRID_HEIGHT; nCellY++)
	{
		for (int nCellX = 0; nCellX < REGION_GRID_WIDTH; nCellX++)
		{
			KCell*              pCell       = &m_Cells[nCellX][nCellY];
			KCell::KBaseInfo*   pBaseInfo   = (KCell::KBaseInfo*)pbyOffset;

            pCell->m_BaseInfo  = *pBaseInfo;
			pCell->m_wLowLayer = 0;
            pbyOffset += sizeof(KCell::KBaseInfo);

			pCell->m_wHighLayer = *(WORD*)pbyOffset;
            pbyOffset += sizeof(WORD);
		}
	}


    KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(int));
    uLeftBytes -= sizeof(int);

    nExtCellCount = *(int*)pbyOffset;
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(nExtCellCount >= 0);
    KGLOG_PROCESS_ERROR(uLeftBytes >= uExtCellInfoSize * nExtCellCount);
    uLeftBytes -= uExtCellInfoSize * nExtCellCount;

	for (int nIndex = 0; nIndex < nExtCellCount; nIndex++)
	{
        int                 nCellX      = 0;
        int                 nCellY      = 0;
        KCell::KBaseInfo*   pBaseInfo   = NULL;

		pAllocCell = rObjectAlloctor.NewCell();
		KGLOG_PROCESS_ERROR(pAllocCell);

		nCellX = *(int*)pbyOffset;
		pbyOffset += sizeof(int);

		nCellY = *(int*)pbyOffset;
		pbyOffset += sizeof(int);

		pBaseInfo = (KCell::KBaseInfo*)pbyOffset;
		pbyOffset += sizeof(KCell::KBaseInfo);

		pAllocCell->m_BaseInfo = *pBaseInfo;

        pAllocCell->m_wHighLayer = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);

        pAllocCell->m_wLowLayer = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);


		bRetCode = AddObstacle(nCellX, nCellY, pAllocCell);
		KGLOG_PROCESS_ERROR(bRetCode);

        pAllocCell = NULL;
	}

    
	if (uLeftBytes >= sizeof(m_dwScriptList))
    {
	    memcpy(m_dwScriptList, pbyOffset, sizeof(m_dwScriptList));
        pbyOffset  += sizeof(m_dwScriptList);
        uLeftBytes -= sizeof(m_dwScriptList);
    }

    KGLOG_PROCESS_ERROR(uLeftBytes == 0);

    bResult = true;
Exit0:
    if (pAllocCell)
    {
        rObjectAlloctor.DeleteCell(pAllocCell);
        pAllocCell = NULL;
    }
    KG_COM_RELEASE(piBuffer);
    KG_COM_RELEASE(piFile);
    return bResult;
}



BOOL KRegion::LoadObjectsData(const char cszFileName[], KObjAlloctor& rObjectAlloctor)
{
    BOOL        bResult                 = false;
	BOOL        bRetCode                = FALSE;
//	IFile*      piFile                  = NULL;
//    size_t      uFileSize               = 0;
//	size_t      uReadBytes              = 0;
//    IKG_Buffer* piBuffer                = NULL;
//    BYTE*       pbyBuffer               = NULL;
//    size_t      uLeftBytes              = 0;
//    BYTE*       pbyOffset               = NULL;
//    KObjHeader* pFileHeader             = NULL;
//    KNpc*       pAllocNpc               = NULL;
//    BOOL        bAllocNpcInitFlag       = false;
//    KDoodad*    pAllocDoodad            = NULL;
//    BOOL        bAllocDoodadInitFlag    = false;
//
//	piFile = g_OpenFile(cszFileName);
//	KGLOG_PROCESS_ERROR(piFile);
//
//    uFileSize = piFile->Size();
//    KGLOG_PROCESS_ERROR(uFileSize > 0);
//
//    piBuffer = KG_MemoryCreateBuffer((unsigned long)uFileSize);
//    KGLOG_PROCESS_ERROR(piBuffer);
//
//    pbyBuffer = (BYTE*)piBuffer->GetData();
//    KGLOG_PROCESS_ERROR(pbyBuffer);
//    
//	uReadBytes = piFile->Read(pbyBuffer, (unsigned long)uFileSize);
//	KGLOG_PROCESS_ERROR(uReadBytes == uFileSize);
//
//    KG_COM_RELEASE(piFile);
//
//    uLeftBytes  = uReadBytes;
//    pbyOffset   = pbyBuffer;
//	
//    KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(KObjHeader));
//    uLeftBytes -= sizeof(KObjHeader);
//
//	pFileHeader = (KObjHeader*)pbyOffset;
//    pbyOffset  += sizeof(KObjHeader);
//
//	if (pFileHeader->nVersion != mfvCurrentVersion)
//	{
//		KGLOG_PROCESS_ERROR(pFileHeader->nVersion == mfvVersion1_0_3);
//
//		KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(KNpcData_Old) * pFileHeader->dwNpcTotal);
//		uLeftBytes -= sizeof(KNpcData_Old) * pFileHeader->dwNpcTotal;
//
//		for (DWORD dwIndex = 0; dwIndex < pFileHeader->dwNpcTotal; dwIndex++)
//		{
//			KNpcData_Old* pNpcData = (KNpcData_Old*)pbyOffset;
//
//			pbyOffset += sizeof(KNpcData_Old);
//
//			pAllocNpc = rObjectAlloctor.NewServerNpc();
//			KGLOG_PROCESS_ERROR(pAllocNpc);
//
//			bRetCode = pAllocNpc->Init();
//			KGLOG_PROCESS_ERROR(bRetCode);
//			bAllocNpcInitFlag = true;
//
////			bRetCode = g_WorldSettings.m_NpcTemplateList.LoadFromTemplate(pNpcData->dwTemplateID, pAllocNpc);
////			KGLOG_PROCESS_ERROR(bRetCode);
//
//			pAllocNpc->m_nX				 = pNpcData->nX;
//			pAllocNpc->m_nY				 = pNpcData->nY;
//			pAllocNpc->m_nZ				 = pNpcData->nZ;
//			pAllocNpc->m_dwTemplateID	 = pNpcData->dwTemplateID;
//			pAllocNpc->m_nLevel			 = pNpcData->nLevel;
//			pAllocNpc->m_eKind			 = (CHARACTER_KIND)pNpcData->nKind;
//			pAllocNpc->m_dwScriptID		 = pNpcData->dwScriptID;
//			pAllocNpc->m_nFaceDirection  = pNpcData->byFaceDirection;
//			strncpy(pAllocNpc->m_szName, pNpcData->szName, _NAME_LEN);
//			pAllocNpc->m_szName[_NAME_LEN - 1] = 0;
//
//			//新地图格式仍然不能使用， 这里不支持设置AI
//			//bRetCode = pAllocNpc->m_AIController.SetAIType((AI_TYPE)pNpcData->byAIType);
//			//KGLOG_PROCESS_ERROR(bRetCode);
//
////			pAllocNpc->m_AIController.SetOriginPoint(pNpcData->nX, pNpcData->nY, pNpcData->nZ, pNpcData->byFaceDirection);
//
//			//TODO:添加Npc小队
//			//KNpcTeam* pNpcTeam = g_WorldSettings.m_NpcTeamList.GetNpcTeam(m_pScene->m_dwMapID, pNpcData->dwTeamID);
//	
//#ifdef _SERVER
//			pAllocNpc->m_nExp = g_WorldSettings.m_NpcExpTable.GetExp(pAllocNpc->m_nLevel, pAllocNpc->m_nIntensity);
//			if (pAllocNpc->m_nExp == 0)
//			{
//				KGLogPrintf(
//					KGLOG_WARNING, 
//					"[KRegion] Load npc, get exp failed: szName = \"%s\", nLevel = %d, nIntensity = %d\n",
//					pAllocNpc->m_szName, pAllocNpc->m_nLevel, pAllocNpc->m_nIntensity
//				);
//			}
//#endif
//
//			bRetCode = g_SO3World.AddNpc(pAllocNpc, m_pScene, pNpcData->nX, pNpcData->nY, pNpcData->nZ);
//			KGLOG_PROCESS_ERROR(bRetCode);
//
//			pAllocNpc           = NULL;
//			bAllocNpcInitFlag   = false;
//		}
//
//		KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(KDoodadData_Old) * pFileHeader->dwDoodadTotal);
//		uLeftBytes -= sizeof(KDoodadData_Old) * pFileHeader->dwDoodadTotal;
//
//		for (DWORD dwIndex = 0; dwIndex < pFileHeader->dwDoodadTotal; dwIndex++)
//		{
//			KDoodadData_Old* pDoodadData = (KDoodadData_Old*)pbyOffset;
//
//			pbyOffset += sizeof(KDoodadData_Old);
//
//			pAllocDoodad = rObjectAlloctor.NewServerDoodad();
//			KGLOG_PROCESS_ERROR(pAllocDoodad);
//
//			bRetCode = pAllocDoodad->Init();
//			KGLOG_PROCESS_ERROR(bRetCode);
//			bAllocDoodadInitFlag = true;
//
//			bRetCode = g_WorldSettings.m_DoodadTemplateList.LoadFromTemplate(pDoodadData->dwTemplateID, pAllocDoodad);
//			KGLOG_PROCESS_ERROR(bRetCode);
//
//			pAllocDoodad->m_dwScriptID      = pDoodadData->dwScriptID;
//			pAllocDoodad->m_nFaceDirection  = pDoodadData->byFaceDirection;
//			strncpy(pAllocDoodad->m_szName, pDoodadData->szName, sizeof(pAllocDoodad->m_szName));
//			pAllocDoodad->m_szName[sizeof(pAllocDoodad->m_szName) - 1] = 0;
//
//			bRetCode = g_SO3World.AddDoodad(pAllocDoodad, m_pScene, pDoodadData->nX, pDoodadData->nY, pDoodadData->nZ);
//			KGLOG_PROCESS_ERROR(bRetCode);
//
//			pAllocDoodad         = NULL;
//			bAllocDoodadInitFlag = false;
//		}
//	}
//	else // end of "if(pFileHeader->nVersion != mfvCurrentVersion)"
//	{
//		KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(KNPC_DATA) * pFileHeader->dwNpcTotal);
//		uLeftBytes -= sizeof(KNPC_DATA) * pFileHeader->dwNpcTotal;
//
//		for (DWORD dwIndex = 0; dwIndex < pFileHeader->dwNpcTotal; dwIndex++)
//		{
//			KNPC_DATA* pNpcData = (KNPC_DATA*)pbyOffset;
//
//			pbyOffset += sizeof(KNPC_DATA);
//
//			pAllocNpc = rObjectAlloctor.NewServerNpc();
//			KGLOG_PROCESS_ERROR(pAllocNpc);
//
//			bRetCode = pAllocNpc->Init();
//			KGLOG_PROCESS_ERROR(bRetCode);
//			bAllocNpcInitFlag = true;
//
//			bRetCode = pAllocNpc->Load(*pNpcData);
//			KGLOG_PROCESS_ERROR(bRetCode);
//			
//			bRetCode = g_SO3World.AddNpc(pAllocNpc, m_pScene, pNpcData->nX, pNpcData->nY, pNpcData->nZ);
//			KGLOG_PROCESS_ERROR(bRetCode);
//
//			pAllocNpc           = NULL;
//			bAllocNpcInitFlag   = false;
//		}
//
//		KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(KDOODAD_DATA) * pFileHeader->dwDoodadTotal);
//		uLeftBytes -= sizeof(KDOODAD_DATA) * pFileHeader->dwDoodadTotal;
//
//		for (DWORD dwIndex = 0; dwIndex < pFileHeader->dwDoodadTotal; dwIndex++)
//		{
//			KDOODAD_DATA* pDoodadData = (KDOODAD_DATA*)pbyOffset;
//
//			pbyOffset += sizeof(KDOODAD_DATA);
//
//			pAllocDoodad = rObjectAlloctor.NewServerDoodad();
//			KGLOG_PROCESS_ERROR(pAllocDoodad);
//
//			bRetCode = pAllocDoodad->Init();
//			KGLOG_PROCESS_ERROR(bRetCode);
//			bAllocDoodadInitFlag = true;
//
//			bRetCode = pAllocDoodad->Load(*pDoodadData);
//			KGLOG_PROCESS_ERROR(bRetCode);
//
//			bRetCode = g_SO3World.AddDoodad(pAllocDoodad, m_pScene, pDoodadData->nX, pDoodadData->nY, pDoodadData->nZ);
//			KGLOG_PROCESS_ERROR(bRetCode);
//
//			pAllocDoodad         = NULL;
//			bAllocDoodadInitFlag = false;
//		}		
//	}
//
//    KGLOG_PROCESS_ERROR(uLeftBytes == 0);

    bResult = true;
Exit0:

 //   if (bAllocNpcInitFlag)
 //   {
 //       ASSERT(pAllocNpc);
 //       pAllocNpc->UnInit();
 //       bAllocNpcInitFlag = false;
 //   }

 //   if (pAllocNpc)
 //   {
 //       rObjectAlloctor.DeleteServerNpc(pAllocNpc);
 //       pAllocNpc = NULL;
 //   }

 //   if (bAllocDoodadInitFlag)
 //   {
 //       ASSERT(pAllocDoodad);
 //       pAllocDoodad->UnInit();
 //       bAllocDoodadInitFlag = false;
 //   }

 //   if (pAllocDoodad)
 //   {
 //       rObjectAlloctor.DeleteServerDoodad(pAllocDoodad);
 //       pAllocDoodad = NULL;
 //   }


 //   KG_COM_RELEASE(piBuffer);
	//KG_COM_RELEASE(piFile);
	return bResult;
}



BOOL KRegion::AddObstacle(int nXCell, int nYCell, KCell* pCell)
{
	BOOL bFound = FALSE;

	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);
	KGLOG_PROCESS_ERROR(pCell);

	KCell* pInsertPos = &m_Cells[nXCell][nYCell];

	//查找插入的位置
	while (!bFound)
	{
		KGLOG_PROCESS_ERROR(pCell->m_wLowLayer >= pInsertPos->m_wHighLayer);

		if (pInsertPos->m_pNext)
		{
			if (pCell->m_wLowLayer >= pInsertPos->m_wHighLayer &&
				pCell->m_wHighLayer <= pInsertPos->m_pNext->m_wLowLayer)
			{
				bFound = TRUE;
			}
			else
			{
				pInsertPos = pInsertPos->m_pNext;
			}
		}
		else
		{			
			bFound = TRUE;
		}
	}

	//插入链表
	pCell->m_pNext = pInsertPos->m_pNext;
	pInsertPos->m_pNext = pCell;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KRegion::AddObstacle(int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, WORD wLowLayer, WORD wHighLayer)
{
    BOOL   bResult          = false;
	KCell* pCellNode        = NULL;
    KCell* pLastLowerCell   = NULL;
    KCell* pPreNode         = NULL;
    KCell* pNewCell         = NULL;

	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);
    ASSERT(wLowLayer < wHighLayer);

	pCellNode = &m_Cells[nXCell][nYCell];

    // 找到最后一个低面<=新Cell的节点,将新Cell插入到它的后面
    if (pBaseInfo->dwFaceUp)
    {
        // 对于朝上的Cell,尽量放在上面
        while (pCellNode)
        {
            if (pCellNode->m_wLowLayer > wLowLayer)
                break;
            pLastLowerCell = pCellNode;
            pCellNode = pCellNode->m_pNext;
        }
    }
    else
    {
        // 对于朝下的Cell,尽量放在下面
        while (pCellNode)
        {
            if (pCellNode->m_wLowLayer >= wLowLayer)
                break;
            pLastLowerCell = pCellNode;
            pCellNode = pCellNode->m_pNext;
        }
    }

    // 因为至少有个[0, 0]的基本Cell,所以不可能为空
    ASSERT(pLastLowerCell);

    pNewCell = g_ObjectManager.m_CellSet.New();
    KGLOG_PROCESS_ERROR(pNewCell);

    pNewCell->m_wLowLayer   = wLowLayer;
    pNewCell->m_wHighLayer  = wHighLayer;
    pNewCell->m_BaseInfo    = *pBaseInfo;

    pNewCell->m_pNext       = pLastLowerCell->m_pNext;
    pLastLowerCell->m_pNext = pNewCell;

    bResult = true;
Exit0:
	return bResult;
}

KCell* KRegion::RemoveObstacle(int nXCell, int nYCell, int nZ)
{
	BOOL bFound = FALSE;

	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);

	KCell* pRemovePos = &(m_Cells[nXCell][nYCell]);

	while (!bFound)
	{
		KG_PROCESS_ERROR(pRemovePos->m_pNext);
		if ((nZ / POINT_PER_ALTITUDE) <= pRemovePos->m_pNext->m_wHighLayer)
		{
			KG_PROCESS_ERROR((nZ / POINT_PER_ALTITUDE) >= pRemovePos->m_pNext->m_wLowLayer);
			bFound = TRUE;
		}
		else
		{
			pRemovePos = pRemovePos->m_pNext;
		}
	}

	KG_PROCESS_ERROR(pRemovePos->m_pNext);

	KCell* pCell = pRemovePos->m_pNext;
	pRemovePos->m_pNext = pRemovePos->m_pNext->m_pNext;

	return pCell;

Exit0:

	return NULL;
}


BOOL KRegion::GetBaseCellInfo(int nXCell, int nYCell, KCell* pCell)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(nXCell >= 0 && nXCell < REGION_GRID_WIDTH);
    KGLOG_PROCESS_ERROR(nYCell >= 0 && nYCell < REGION_GRID_HEIGHT);

	*pCell = m_Cells[nXCell][nYCell];

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::SetBaseCellInfo(int nXCell, int nYCell, KCell& rCell)
{
    BOOL    bResult = false;
    KCell*  pCell   = NULL;

    KGLOG_PROCESS_ERROR(nXCell >= 0 && nXCell < REGION_GRID_WIDTH);
    KGLOG_PROCESS_ERROR(nYCell >= 0 && nYCell < REGION_GRID_HEIGHT);

	pCell = &m_Cells[nXCell][nYCell];

    pCell->m_wLowLayer  = rCell.m_wLowLayer;
    pCell->m_wHighLayer = rCell.m_wHighLayer;
    pCell->m_BaseInfo   = rCell.m_BaseInfo;

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::MergeObstacleData()
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    KCell*      pLowestCell         = NULL;
    KCell*      pProcessCell        = NULL;
    KCell*      pPreNode            = NULL;

	for (int nYCell = 0; nYCell < REGION_GRID_HEIGHT; nYCell++)
	{
		for (int nXCell = 0; nXCell < REGION_GRID_WIDTH; nXCell++)
		{
            // ---------- 合并高度交叉Cell ------------------------------

            pLowestCell = &m_Cells[nXCell][nYCell];

            pProcessCell = pLowestCell;

            while (pProcessCell)
            {
                KCell*  pCellNode             = pProcessCell->m_pNext;
                DWORD   dwBarrierDirection    = pProcessCell->m_BaseInfo.dwBarrierDirection;
                DWORD   dwBarrierGradient     = pProcessCell->m_BaseInfo.dwGradientDegree;
                DWORD   dwFaceUp              = pProcessCell->m_BaseInfo.dwFaceUp;

                while (pCellNode)
                {
                    if (pCellNode->m_wLowLayer > pProcessCell->m_wHighLayer)
                        break;

                    if (pCellNode->m_BaseInfo.dwFaceUp != dwFaceUp)
                    {
                        dwFaceUp = 0;
                    }
                    
                    ASSERT(pCellNode->m_wLowLayer >= pProcessCell->m_wLowLayer);

                    if (pCellNode->m_wHighLayer > pProcessCell->m_wHighLayer)
                    {
                        pProcessCell->m_wHighLayer       = pCellNode->m_wHighLayer;
                        pProcessCell->m_BaseInfo         = pCellNode->m_BaseInfo;
                    }

                    if (pCellNode->m_BaseInfo.dwGradientDegree > dwBarrierGradient)
                    {
                        dwBarrierDirection = pCellNode->m_BaseInfo.dwBarrierDirection;
                        dwBarrierGradient = pCellNode->m_BaseInfo.dwGradientDegree;
                    }

                    pProcessCell->m_pNext = pCellNode->m_pNext;

                    g_ObjectManager.m_CellSet.Delete(pCellNode);

                    pCellNode = pProcessCell->m_pNext;
                }

                pProcessCell->m_BaseInfo.dwBarrierDirection = dwBarrierDirection;
                pProcessCell->m_BaseInfo.dwFaceUp           = dwFaceUp;

                pProcessCell = pProcessCell->m_pNext;
            }

            // ---------- 合并上下面Cell ------------------------------
            
            
            
            pPreNode     = NULL;
            pProcessCell = pLowestCell;
            
            while (pProcessCell)
            {
                if (pProcessCell->m_BaseInfo.dwFaceUp && pPreNode != NULL)
                {
                    pProcessCell->m_BaseInfo.dwFaceUp = 0;

                    pPreNode->m_wHighLayer  = pProcessCell->m_wHighLayer;
                    pPreNode->m_BaseInfo    = pProcessCell->m_BaseInfo;
                    pPreNode->m_pNext       = pProcessCell->m_pNext;

                    g_ObjectManager.m_CellSet.Delete(pProcessCell);
                    
                    pProcessCell = pPreNode->m_pNext;
                    continue;
                }

                pProcessCell->m_BaseInfo.dwFaceUp = 0;

                pPreNode     = pProcessCell;
                pProcessCell = pProcessCell->m_pNext;
            }  
            
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRegion::SetWaterHeight(int nXCell, int nYCell, WORD wWaterHeight)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCellNode   = NULL;
    KCell*  pPreNode    = NULL;

    KGLOG_PROCESS_ERROR(nXCell >= 0 && nXCell < REGION_GRID_WIDTH);
    KGLOG_PROCESS_ERROR(nYCell >= 0 && nYCell < REGION_GRID_HEIGHT);

    // 清除原来的水的Cell
    pCellNode = &m_Cells[nXCell][nYCell];

    while (pCellNode)
    {
        if (pCellNode->m_BaseInfo.dwCellType == ctWater && pPreNode)
        {
            pPreNode->m_pNext = pCellNode->m_pNext;
            g_ObjectManager.m_CellSet.Delete(pCellNode);
            pCellNode = pPreNode->m_pNext;
            continue;
        }

        pPreNode = pCellNode;
        pCellNode = pCellNode->m_pNext;
    }

    // 在水平面以下的Cell空隙之间填满水Cell

    pCellNode = &m_Cells[nXCell][nYCell];

    while (pCellNode)
    {
        KCell*  pNext               = NULL;
        WORD    wWaterCellHighLayer = 0;

        if (pCellNode->m_wHighLayer >= wWaterHeight)
            break;

        ASSERT(pCellNode->m_wLowLayer < wWaterHeight);

        pNext = pCellNode->m_pNext;

		if (pNext && pNext->m_wLowLayer < wWaterHeight)
		{
			wWaterCellHighLayer = pNext->m_wLowLayer;
		}
		else
		{
			wWaterCellHighLayer = wWaterHeight;
		}

        if (wWaterCellHighLayer > pCellNode->m_wHighLayer)
        {
		    KCell* pNewCell = NULL;
            
            pNewCell = g_ObjectManager.m_CellSet.New();
            KGLOG_PROCESS_ERROR(pNewCell);

		    ZeroMemory(&pNewCell->m_BaseInfo, sizeof(KCell::KBaseInfo));

		    pNewCell->m_BaseInfo.dwCellType            = ctWater;

            pNewCell->SetGradientDegree(0);
            pNewCell->SetGradientDirection(0);
            pNewCell->SetBarrierDirection(0);

		    pNewCell->m_wLowLayer   = pCellNode->m_wHighLayer;
            pNewCell->m_wHighLayer  = wWaterCellHighLayer;

            pNewCell->m_pNext   = pCellNode->m_pNext;
            pCellNode->m_pNext  = pNewCell;

            pCellNode = pNewCell->m_pNext;
            continue;
        }

        pCellNode = pCellNode->m_pNext;
    }

	bResult = true;
Exit0:
	return bResult;
}


// 获取障碍
KCell* KRegion::GetObstacle(int nXCell, int nYCell, int nZ)
{
	KCell* pCell = &(m_Cells[nXCell][nYCell]);

    ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);

	while (pCell)
	{
		if (nZ >= pCell->m_wLowLayer * POINT_PER_ALTITUDE && nZ <= pCell->m_wHighLayer * POINT_PER_ALTITUDE)
		{
			break;
		}
	    pCell = pCell->m_pNext;
	}

	return pCell;
}

KCell* KRegion::GetLowestObstacle(int nXCell, int nYCell)
{
	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);

    return &(m_Cells[nXCell][nYCell]);
}

KCell* KRegion::GetTopmostObstacle(int nXCell, int nYCell)
{
    KCell* pCell = &(m_Cells[nXCell][nYCell]);

	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);

    while (pCell->m_pNext)
    {
        pCell = pCell->m_pNext;
    }

    return pCell;
}

KCell* KRegion::GetLowerObstacle(int nXCell, int nYCell, int nZ)
{
    KCell*  pResult     = NULL;
	BOOL    bFound      = FALSE;
    KCell*  pCellNode   = NULL;
    int     nAltitude   = nZ / POINT_PER_ALTITUDE;

    // 从最下面的一个Cell开始找,找到最上一个 "上表面<=nZ" 的Cell,也就是链表中最后一个 "上表面<=nZ" 的Cell
    // 如果所有Cell上表面都高于nZ,则返回NULL

	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);

	pCellNode = &(m_Cells[nXCell][nYCell]);

	while (pCellNode)
	{
        // Cell是越来越高的,不用往后面找了
		if (pCellNode->m_wHighLayer > nAltitude)
            break;
		pResult		= pCellNode;
	    pCellNode	= pCellNode->m_pNext;
	}

Exit0:
	return pResult;
}

KCell* KRegion::GetAdaptedCellForObj(
    int nXCell, int nYCell, int nFootPosZ, int nClimb, int nHeight, 
    int* pnRelativeZ, int *pnObstacleDirection
)
{
    KCell*  pResult         = NULL;
    KCell*  pPossibleCell   = NULL;
    KCell*  pCellNode       = NULL;
    int     nRelativeZ      = 0;

	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
 	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);
    ASSERT(pnRelativeZ);

	pCellNode = &m_Cells[nXCell][nYCell];

    while (pCellNode)
    {
        if (pCellNode->m_BaseInfo.dwCellType == ctWater)
        {
            if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE > nFootPosZ + nClimb)
                break;

            pPossibleCell = NULL;

            if (!pCellNode->m_BaseInfo.dwBlockCharacter)
            {
                KCell* pNextRealCell = NULL;
                KCell* pIter         = pCellNode->m_pNext;

                // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
                while (pIter)
                {
                    if (pIter->m_BaseInfo.dwCellType != ctWater)
                    {
                        pNextRealCell = pIter;
                        break;
                    }
                    pIter = pIter->m_pNext;
                }

                if (pNextRealCell)
                {
                    if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ)
                    {
                        if (pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE - nFootPosZ >= nHeight)
                        {
                            pPossibleCell   = pCellNode;
                            nRelativeZ      = nFootPosZ - pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
                        }
                    }
                    else
                    {
                        if ((pNextRealCell->m_wLowLayer - pCellNode->m_wLowLayer) * POINT_PER_ALTITUDE >= nHeight)
                        {
                            pPossibleCell   = pCellNode;
                            nRelativeZ      = (pCellNode->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE;
                        }
                    }
                }
                else
                {
                    pPossibleCell = pCellNode;
                    if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ)
                    {
                        nRelativeZ = nFootPosZ - pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
                    }
                    else
                    {
                        nRelativeZ = (pCellNode->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE;
                    }
                }
            }
        }
        else
        {
            if (pCellNode->m_wHighLayer * POINT_PER_ALTITUDE > nFootPosZ + nClimb)
            {
                if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ + nHeight)
                {
                    // 注意,如果Cell的下表面比头顶还高,那就不能把它视为一个障碍,它只是太高了而已
                    // 而如果是通常的障碍,我们可能还需要通过取它的法线方向来决定怎么绕过它
                    // 斜度较低的斜坡视为没有障碍方向
                    if (pnObstacleDirection)
                    {
                        *pnObstacleDirection = pCellNode->GetBarrierDirection();
                    }
                }
                break;
            }

            pPossibleCell = NULL;

            if (!pCellNode->m_BaseInfo.dwBlockCharacter)
            {
                KCell* pNextRealCell = NULL;
                KCell* pIter         = pCellNode->m_pNext;

                // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
                while (pIter)
                {
                    if (pIter->m_BaseInfo.dwCellType != ctWater)
                    {
                        pNextRealCell = pIter;
                        break;
                    }
                    pIter = pIter->m_pNext;
                }

                if (pNextRealCell)
                {
                    if (pCellNode->m_wHighLayer * POINT_PER_ALTITUDE < nFootPosZ)
                    {
                        if (pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE - nFootPosZ >= nHeight)
                        {
                            pPossibleCell   = pCellNode;
                            nRelativeZ      = nFootPosZ - pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
                        }
                    }
                    else
                    {
                        if ((pNextRealCell->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE >= nHeight)
                        {
                            pPossibleCell   = pCellNode;
                            nRelativeZ      = 0;
                        }
                    }
                }
                else
                {
                    pPossibleCell = pCellNode;
                    if (pCellNode->m_wHighLayer * POINT_PER_ALTITUDE < nFootPosZ)
                    {
                        nRelativeZ  = nFootPosZ - pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
                    }
                    else
                    {
                        nRelativeZ = 0;
                    }
                }
            }
        }

	    pCellNode	= pCellNode->m_pNext;
    }

    KG_PROCESS_ERROR(pPossibleCell);

    pResult         = pPossibleCell;
    *pnRelativeZ    = nRelativeZ;
Exit0:
    return pResult;
}

KCell* KRegion::GetProximalCellForObj(int nXCell, int nYCell, int nFootPosZ, int nHeight, int* pnRelativeZ)
{
    KCell*  pResult         = NULL;
    KCell*  pPossibleCell   = NULL;
    KCell*  pCellNode       = NULL;
    int     nRelativeZ      = 0;
    int     nAbsolutePosZ   = 0;

    ASSERT(pnRelativeZ);
	ASSERT(nXCell >= 0);
	ASSERT(nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0);
	ASSERT(nYCell < REGION_GRID_HEIGHT);


    pCellNode = &(m_Cells[nXCell][nYCell]);
    while (pCellNode)
    {
        int    nTempPosZ     = 0;
        BOOL   bStandable    = false;

        if (pCellNode->m_BaseInfo.dwCellType == ctWater)
        {
            if (!pCellNode->m_BaseInfo.dwBlockCharacter)
            {
                KCell* pNextRealCell = NULL;
                KCell* pIter         = pCellNode->m_pNext;

                // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
                while (pIter)
                {
                    if (pIter->m_BaseInfo.dwCellType != ctWater)
                    {
                        pNextRealCell = pIter;
                        break;
                    }
                    pIter = pIter->m_pNext;
                }

                if (pNextRealCell)
                {
                    if ((pNextRealCell->m_wLowLayer - pCellNode->m_wLowLayer) * POINT_PER_ALTITUDE >= nHeight)
                    {
                        bStandable = true;

                        if (nFootPosZ > pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE)
                        {
                            nTempPosZ = pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE - nHeight;
                        }
                        else if (nFootPosZ < pCellNode->m_wLowLayer * POINT_PER_ALTITUDE)
                        {
                            nTempPosZ = pCellNode->m_wLowLayer * POINT_PER_ALTITUDE;
                        }
                        else
                        {
                            nTempPosZ = nFootPosZ;
                        }
                    }
                }
                else
                {
                    bStandable = true;

                    if (nFootPosZ > pCellNode->m_wLowLayer * POINT_PER_ALTITUDE)
                    {
                        nTempPosZ = nFootPosZ;
                    }
                    else
                    {
                        nTempPosZ = pCellNode->m_wLowLayer * POINT_PER_ALTITUDE;
                    }                        
                }
            }
        }
        else
        {
            if (!pCellNode->m_BaseInfo.dwBlockCharacter)
            {
                KCell* pNextRealCell = NULL;
                KCell* pIter         = pCellNode->m_pNext;

                // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
                while (pIter)
                {
                    if (pIter->m_BaseInfo.dwCellType != ctWater)
                    {
                        pNextRealCell = pIter;
                        break;
                    }
                    pIter = pIter->m_pNext;
                }

                if (pNextRealCell)
                {
                    if ((pNextRealCell->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE >= nHeight)
                    {
                        bStandable = true;

                        if (nFootPosZ > pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE)
                        {
                            nTempPosZ = pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE - nHeight;
                        }
                        else if (nFootPosZ < pCellNode->m_wHighLayer * POINT_PER_ALTITUDE)
                        {
                            nTempPosZ = pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
                        }
                        else
                        {
                            nTempPosZ = nFootPosZ;
                        }
                    }
                }
                else
                {
                    bStandable = true;

                    if (nFootPosZ > pCellNode->m_wHighLayer * POINT_PER_ALTITUDE)
                    {
                        nTempPosZ = nFootPosZ;
                    }
                    else
                    {
                        nTempPosZ = pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
                    }                        
                }
            }
        }

        if (bStandable)
        {
            if (pPossibleCell)
            {
                // 如果之前已经找到Cell了,则比较看相比是否更接近nFootZ
                if (abs(nTempPosZ - nFootPosZ) <= abs(nAbsolutePosZ - nFootPosZ))
                {
                    pPossibleCell = pCellNode;
                    nAbsolutePosZ = nTempPosZ;
                }
                else
                {
                    // 由于Cell没有交错的,所以,如果前面已经找到其他Cell,而这个Cell离nFootZ更远,那么后面其他的Cell也就没必要找了
                    break;
                }
            }
            else
            {
                pPossibleCell = pCellNode;
                nAbsolutePosZ = nTempPosZ;
            }
        }

        pCellNode = pCellNode->m_pNext;
    }


    KG_PROCESS_ERROR(pPossibleCell);

    nRelativeZ      = nAbsolutePosZ - pPossibleCell->m_wHighLayer * POINT_PER_ALTITUDE;

    pResult         = pPossibleCell;
    *pnRelativeZ    = nRelativeZ;
Exit0:
    return pResult;
}


BOOL KRegion::CheckObstacle(int nXCell, int nYCell, int nLayer)
{
	BOOL bFound = FALSE;

	KG_PROCESS_SUCCESS(nXCell < 0);
	KG_PROCESS_SUCCESS(nXCell >= REGION_GRID_WIDTH);
	KG_PROCESS_SUCCESS(nYCell < 0);
	KG_PROCESS_SUCCESS(nYCell >= REGION_GRID_HEIGHT);

	KCell* pCell = &(m_Cells[nXCell][nYCell]);
	while (pCell)
	{
		if (nLayer >= pCell->m_wLowLayer)
		{
			if (nLayer < pCell->m_wHighLayer)
				return TRUE;
			else
				pCell = pCell->m_pNext;
		}
		else
		{
			return FALSE;
		}
	}

Exit0:
	return FALSE;

Exit1:
	// 非法区域全算障碍
	return TRUE;
}

DWORD KRegion::GetScriptID(int nXCell, int nYCell)
{
	ASSERT(nXCell >= 0 && nXCell < REGION_GRID_WIDTH);
	ASSERT(nYCell >= 0 && nYCell < REGION_GRID_HEIGHT);

	return m_dwScriptList[m_Cells[nXCell][nYCell].m_BaseInfo.dwScriptIndex];
}

DWORD KRegion::GetScriptIDByIndex(DWORD dwIndex)
{
	DWORD dwResult = 0;
	KGLOG_PROCESS_ERROR(dwIndex < SCRIPT_COUNT_PER_REGION);
	dwResult = m_dwScriptList[dwIndex];
Exit0:
	return dwResult;
}

BOOL KRegion::SetScriptID(KCell* pCell, DWORD dwScriptID)
{
	ASSERT(pCell);

	int nScriptIndex = 1;
	int nInsertIndex = 0;

	if (!dwScriptID)
	{
		pCell->m_BaseInfo.dwScriptIndex = 0;
		goto Exit1;
	}

	for (; nScriptIndex < SCRIPT_COUNT_PER_REGION; nScriptIndex++)
	{
		if (m_dwScriptList[nScriptIndex] == dwScriptID)
		{
			pCell->m_BaseInfo.dwScriptIndex = nScriptIndex;
			break;
		}
		if (nInsertIndex == 0 && m_dwScriptList[nScriptIndex] == 0)
		{
			nInsertIndex = nScriptIndex;
		}
	}

	if (nScriptIndex == SCRIPT_COUNT_PER_REGION)
	{
		KGLOG_PROCESS_ERROR(nInsertIndex > 0 && nInsertIndex < SCRIPT_COUNT_PER_REGION);
		m_dwScriptList[nInsertIndex] = dwScriptID;
		pCell->m_BaseInfo.dwScriptIndex = nInsertIndex;
	}

Exit1:

	return TRUE;

Exit0:

	return FALSE;
}



BOOL KRegion::AddNpc(KNpc* pNpc)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCell       = NULL;
    int     nFootZ      = pNpc->m_nZ;
    int     nNewPosZ    = 0;
    int     nRelativeZ  = 0;

	pCell = GetProximalCellForObj(pNpc->m_nXCell, pNpc->m_nYCell, nFootZ, pNpc->m_nHeight, &nRelativeZ);
    KGLOG_PROCESS_ERROR(pCell);

    pNpc->m_nZ = nRelativeZ;

	bRetCode = pCell->AddNpc(pNpc);
	KGLOG_PROCESS_ERROR(bRetCode);

	pNpc->m_pRegion = this;

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::AddDoodad(KDoodad* pDoodad)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCell       = NULL;
    int     nFootZ      = pDoodad->m_nZ;
    int     nRelativeZ  = 0;

	pCell = GetProximalCellForObj(pDoodad->m_nXCell, pDoodad->m_nYCell, pDoodad->m_nZ, pDoodad->m_nHeight, &nRelativeZ);
    KGLOG_PROCESS_ERROR(pCell);

    pDoodad->m_nZ = nRelativeZ;

	bRetCode = pCell->AddDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_pRegion = this;

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::AddPlayer(KPlayer* pPlayer)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCell       = NULL;
    int     nFootZ      = pPlayer->m_nZ;
    int     nRelativeZ  = 0;

	pCell = GetProximalCellForObj(pPlayer->m_nXCell, pPlayer->m_nYCell, nFootZ, pPlayer->m_nHeight, &nRelativeZ);
    KGLOG_PROCESS_ERROR(pCell);

    pPlayer->m_nZ = nRelativeZ;

	bRetCode = pCell->AddPlayer(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_pRegion = this;

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::RemoveNpc(KNpc* pNpc)
{
	BOOL bRetCode = false;

	ASSERT(pNpc->m_pCell);

	bRetCode = pNpc->m_pCell->RemoveNpc(pNpc);
	ASSERT(bRetCode);

	pNpc->m_pRegion = NULL;

	return true;
}

BOOL KRegion::RemoveDoodad(KDoodad* pDoodad)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pDoodad);
	KGLOG_PROCESS_ERROR(pDoodad->m_pCell);

	bRetCode = pDoodad->m_pCell->RemoveDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_pRegion = NULL;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KRegion::RemovePlayer(KPlayer* pPlayer)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pPlayer);
	KGLOG_PROCESS_ERROR(pPlayer->m_pCell);

	bRetCode = pPlayer->m_pCell->RemovePlayer(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_pRegion = NULL;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KRegion::DeleteAllCell(void)
{
	for (int nXCell = 0; nXCell < REGION_GRID_WIDTH; nXCell++)
	{
		for (int nYCell = 0; nYCell < REGION_GRID_HEIGHT; nYCell++)
		{
			KCell* pCell = &(m_Cells[nXCell][nYCell]);

			while (pCell->m_pNext != NULL)
			{
				KCell* pTemp = pCell->m_pNext;
				pCell->m_pNext = pCell->m_pNext->m_pNext;
				g_ObjectManager.m_CellSet.Delete(pTemp);
			}
		}
	}

	return TRUE;
}


int KRegion::RecycleScriptIndex()
{
	int nResult = 0;
	int nScriptCount[SCRIPT_COUNT_PER_REGION];
	ZeroMemory(nScriptCount, sizeof(nScriptCount));

	for (int x = 0; x < REGION_GRID_WIDTH; x++)
	{
		for (int y = 0; y < REGION_GRID_HEIGHT; y++)
		{
			nScriptCount[m_Cells[x][y].m_BaseInfo.dwScriptIndex]++;
		}
	}
	for (int i = 1; i < SCRIPT_COUNT_PER_REGION; i++)
	{
		if (nScriptCount[i] == 0)
		{
			m_dwScriptList[i] = 0;
		}
		if (m_dwScriptList[i] == 0)
		{
			nResult++;
		}	
	}
	return nResult;
}
