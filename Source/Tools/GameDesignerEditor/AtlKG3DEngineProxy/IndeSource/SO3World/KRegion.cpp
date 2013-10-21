#include "stdafx.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSceneObjNode.h"
#include "KMath.h"
#include "KPlayerServer.h"
#include "KSO3World.h"
#include "KGodClient.h"
#include "KTerrainData.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "Engine/KMemory.h"
#include "INpcFile.h"
#include "IDoodadFile.h"

#define NPC_SYNC_INTERVAL		(GAME_FPS / 8)
#define PLAYER_SYNC_INTERVAL	(GAME_FPS)
#define	DOODAD_SYNC_INTERVAL	(GAME_FPS / 4)


KRegion::KRegion()
{
    m_pScene = NULL;
}

KRegion::~KRegion()
{
}

BOOL KRegion::Init(KScene* pScene, int nRegionX, int nRegionY)
{
    BOOL           bResult       = false;
    KTerrainScene* pTerrainScene = NULL;

    assert(pScene);
    assert(nRegionX >= 0);
    assert(nRegionY >= 0);
	assert(m_PlayerList.IsEmpty());
	assert(m_NpcList.IsEmpty());
	assert(m_DoodadList.IsEmpty());

    m_pScene                = pScene;
    m_nRegionX              = nRegionX;
    m_nRegionY              = nRegionY;
    m_nLeftCell             = nRegionX * REGION_GRID_WIDTH;
    m_nBottomCell           = nRegionY * REGION_GRID_HEIGHT;
    m_nLeft                 = m_nLeftCell * CELL_LENGTH;
    m_nBottom               = m_nBottomCell * CELL_LENGTH;
	m_nActiveFrame          = 0;
    m_nInFightPlayerCount   = 0;

#ifdef _CLIENT
    memset(m_Cells, 0, sizeof(m_Cells));
    m_pCells = m_Cells;

	memset(m_dwScriptList, 0, sizeof(m_dwScriptList));
    m_pdwScriptList = m_dwScriptList;
#endif

#ifdef _SERVER
    pTerrainScene = pScene->m_pTerrainScene;
    KGLOG_PROCESS_ERROR(pTerrainScene);

    m_pCells = pTerrainScene->GetRegionTerrainData(nRegionX, nRegionY);
    KGLOG_PROCESS_ERROR(m_pCells);

    m_pdwScriptList = pTerrainScene->GetRegionScriptData(nRegionX, nRegionY);
    KGLOG_PROCESS_ERROR(m_pdwScriptList);
#endif

    bResult = true;
Exit0:
	return bResult;
}

void KRegion::UnInit()
{
    m_pScene        = NULL;
    m_pCells        = NULL;
    m_pdwScriptList = NULL;
}

void KRegion::Activate(void)
{
	BOOL            bRetCode            = false;
	KSceneObjNode*  pSceneObjNode       = NULL;
    int             nInFightPlayerCount = 0;

	pSceneObjNode = (KSceneObjNode*)m_NpcList.GetHead();
	while (pSceneObjNode != NULL)
	{
		KSceneObjNode*  pNext = (KSceneObjNode*)pSceneObjNode->GetNext();		
		KNpc*           pNpc  = (KNpc*)(pSceneObjNode->m_pSceneObject);

		assert(pNpc);

		pNpc->Activate();

		pSceneObjNode = pNext;
	}

	pSceneObjNode = (KSceneObjNode*)m_DoodadList.GetHead();
	while (pSceneObjNode != NULL)
	{
		KSceneObjNode*  pNext = (KSceneObjNode*)pSceneObjNode->GetNext();
		KDoodad*        pDoodad = (KDoodad*)(pSceneObjNode->m_pSceneObject);

		assert(pDoodad);		

		pDoodad->Activate();

		pSceneObjNode = pNext;
	}


	pSceneObjNode = (KSceneObjNode*)m_PlayerList.GetHead();
	while (pSceneObjNode != NULL)
	{
		KSceneObjNode*  pNext   = (KSceneObjNode*)pSceneObjNode->GetNext();
		KPlayer*        pPlayer = (KPlayer*)(pSceneObjNode->m_pSceneObject);

		assert(pPlayer);		

		pPlayer->Activate();

        if (pPlayer->m_bFightState)
            nInFightPlayerCount++;

		pSceneObjNode = pNext;
	}

    m_nInFightPlayerCount = nInFightPlayerCount;
}

BOOL KRegion::LoadTerrain()
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
    char            szFileName[MAX_PATH];

    assert(m_pScene);

    snprintf(
        szFileName, sizeof(szFileName), "%s/%s/v_%03d/%03d_Region.%s", 
        MAP_DIR, m_pScene->m_szName, m_nRegionY, m_nRegionX, MAP_FILE_EXT
    );
    szFileName[sizeof(szFileName) - 1] = '\0';

    piFile = g_OpenFile(szFileName);
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

    if (pFileHeader->nVersion != eTerrainDataVersion7)
    {
        KGLogPrintf(KGLOG_ERR, "Unsupported map version (%d) at file: (%s)\n", pFileHeader->nVersion, szFileName);
        goto Exit0;
    }
    
    bRetCode = LoadTerrainBufferV7(pbyOffset, uLeftBytes);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    KG_COM_RELEASE(piFile);
    return bResult;
}

BOOL KRegion::LoadTerrainBufferV7(const BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult                 = false;
    BOOL            bRetCode                = false;
    size_t          uLeftBytes              = uDataLen;
    const BYTE*     pbyOffset               = pbyData;
    size_t          uBaseCellInfoSize       = sizeof(KCell::KBaseInfo) + sizeof(WORD);
    KCell*          pAllocNormalCell        = NULL;
    KDynamicCell*   pAllocDynamicCell       = NULL;
    int             nExtNormalCellCount     = 0;
    size_t          uExtNormalCellDataSize  = sizeof(BYTE) * 2 + sizeof(KCell::KBaseInfo) + sizeof(WORD) * 2;
    int             nExtDynamicCellCount    = 0;
    size_t          uExtDynamicCellDataSize = sizeof(BYTE) * 2 + sizeof(KCell::KBaseInfo) + sizeof(WORD) * 2 + sizeof(WORD);

    KGLOG_PROCESS_ERROR(uLeftBytes >= uBaseCellInfoSize * REGION_GRID_WIDTH * REGION_GRID_HEIGHT);
    uLeftBytes -= uBaseCellInfoSize * REGION_GRID_WIDTH * REGION_GRID_HEIGHT;

	for (int nCellY = 0; nCellY < REGION_GRID_HEIGHT; nCellY++)
	{
		for (int nCellX = 0; nCellX < REGION_GRID_WIDTH; nCellX++)
		{
			KCell*              pCell       = GetLowestObstacle(nCellX, nCellY);
			KCell::KBaseInfo*   pBaseInfo   = (KCell::KBaseInfo*)pbyOffset;

            pCell->m_BaseInfo           = *pBaseInfo;
            pCell->m_BaseInfo.dwDynamic = 0; 
			pCell->m_wLowLayer          = 0;

            pbyOffset += sizeof(KCell::KBaseInfo);

			pCell->m_wHighLayer = *(WORD*)pbyOffset;
            pbyOffset += sizeof(WORD);
		}
	}

    // -------- 读取静态扩展Cell---------------------------------

    KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(int));
    uLeftBytes -= sizeof(int);

    nExtNormalCellCount = *(int*)pbyOffset;
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(nExtNormalCellCount >= 0);
    KGLOG_PROCESS_ERROR(uLeftBytes >= nExtNormalCellCount * uExtNormalCellDataSize);
    uLeftBytes -= nExtNormalCellCount * uExtNormalCellDataSize;

	for (int nIndex = 0; nIndex < nExtNormalCellCount; nIndex++)
	{
        int                 nCellX      = 0;
        int                 nCellY      = 0;
        KCell::KBaseInfo*   pBaseInfo   = NULL;

		pAllocNormalCell = KMemory::RawNew<KCell>();
		KGLOG_PROCESS_ERROR(pAllocNormalCell);

		nCellX = *(BYTE*)pbyOffset;
		pbyOffset += sizeof(BYTE);

		nCellY = *(BYTE*)pbyOffset;
		pbyOffset += sizeof(BYTE);

		pBaseInfo = (KCell::KBaseInfo*)pbyOffset;
		pbyOffset += sizeof(KCell::KBaseInfo);

		pAllocNormalCell->m_BaseInfo = *pBaseInfo;
        pAllocNormalCell->m_BaseInfo.dwDynamic = 0;

        pAllocNormalCell->m_wHighLayer = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);

        pAllocNormalCell->m_wLowLayer = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);

		bRetCode = AddObstacle(nCellX, nCellY, pAllocNormalCell);
		KGLOG_PROCESS_ERROR(bRetCode);

        pAllocNormalCell = NULL;
	}

    // -------- 读取动态扩展Cell---------------------------------

    KGLOG_PROCESS_ERROR(uLeftBytes >= sizeof(int));
    uLeftBytes -= sizeof(int);

    nExtDynamicCellCount = *(int*)pbyOffset;
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(nExtDynamicCellCount >= 0);
    KGLOG_PROCESS_ERROR(uLeftBytes >= nExtDynamicCellCount * uExtDynamicCellDataSize);
    uLeftBytes -= nExtDynamicCellCount * uExtDynamicCellDataSize;

	for (int nIndex = 0; nIndex < nExtDynamicCellCount; nIndex++)
	{
        int                 nCellX      = 0;
        int                 nCellY      = 0;
        KCell::KBaseInfo*   pBaseInfo   = NULL;

		pAllocDynamicCell = KMemory::RawNew<KDynamicCell>();
		KGLOG_PROCESS_ERROR(pAllocDynamicCell);

		nCellX = *(BYTE*)pbyOffset;
		pbyOffset += sizeof(BYTE);

		nCellY = *(BYTE*)pbyOffset;
		pbyOffset += sizeof(BYTE);

		pBaseInfo = (KCell::KBaseInfo*)pbyOffset;
		pbyOffset += sizeof(KCell::KBaseInfo);

		pAllocDynamicCell->m_BaseInfo = *pBaseInfo;
        pAllocDynamicCell->m_BaseInfo.dwDynamic = 1;

        pAllocDynamicCell->m_wHighLayer = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);

        pAllocDynamicCell->m_wLowLayer = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);

        pAllocDynamicCell->m_wGroupIndex = *(WORD*)pbyOffset;
		pbyOffset += sizeof(WORD);

		bRetCode = AddObstacle(nCellX, nCellY, pAllocDynamicCell);
		KGLOG_PROCESS_ERROR(bRetCode);

        pAllocDynamicCell = NULL;
	}
    
    
	if (uLeftBytes >= SCRIPT_DATA_SIZE)
    {
	    memcpy(m_pdwScriptList, pbyOffset, SCRIPT_DATA_SIZE);
        pbyOffset  += SCRIPT_DATA_SIZE;
        uLeftBytes -= SCRIPT_DATA_SIZE;
    }

    KGLOG_PROCESS_ERROR(uLeftBytes == 0);

    bResult = true;
Exit0:
    if (pAllocNormalCell)
    {
        KMemory::RawDelete(pAllocNormalCell);
        pAllocNormalCell = NULL;
    }

    if (pAllocDynamicCell)
    {
        KMemory::RawDelete(pAllocDynamicCell);
        pAllocDynamicCell = NULL;
    }
    return bResult;
}

BOOL KRegion::SaveTerrain()
{
    BOOL            bResult                 = false;
	int             nRetCode                = false;
    size_t          uDataLen                = 0;
	size_t          uWriteNum               = 0;
    size_t          uBaseCellDataSize       = sizeof(KCell::KBaseInfo) + sizeof(WORD);
    size_t          uExtNormalCellDataSize  = sizeof(BYTE) * 2 + sizeof(KCell::KBaseInfo) + sizeof(WORD) * 2;
    size_t          uExtDynamicCellDataSize = sizeof(BYTE) * 2 + sizeof(KCell::KBaseInfo) + sizeof(WORD) * 2 + sizeof(WORD);
    int             nExtendNormalCellCount  = 0;
    int             nExtendDynamicCellCount = 0;
    IKG_Buffer*     piBuffer                = NULL;
    BYTE*           pbyBuffer               = NULL;
	KRegionHeader*  pRegionHeader           = NULL;
    BYTE*           pbyBaseCellData         = NULL;
    int*            pnExtNormalCellCount    = NULL;
    int*            pnExtDynamicCellCount   = NULL;
    BYTE*           pbyExtNormalCellData    = NULL;
    BYTE*           pbyExtDynamicCellData   = NULL;
    DWORD*          pdwScriptData           = NULL;
    FILE*           pFile                   = NULL;
    char            szRoot[MAX_PATH];
	char            szPath[MAX_PATH];

	for (int nCellY = 0; nCellY < REGION_GRID_HEIGHT; nCellY++)
	{
		for (int nCellX = 0; nCellX < REGION_GRID_WIDTH; nCellX++)
		{
			KCell* pBaseCell    = GetLowestObstacle(nCellX, nCellY);
            KCell* pExtendCell  = pBaseCell->m_pNext;

            while (pExtendCell)
            {
                if (pExtendCell->m_BaseInfo.dwDynamic)
                {
                    nExtendDynamicCellCount++;
                }
                else
                {
                    nExtendNormalCellCount++;
                }

                pExtendCell = pExtendCell->m_pNext;
            }
		}
	}

    uDataLen += sizeof(KRegionHeader);
    uDataLen += uBaseCellDataSize * REGION_GRID_WIDTH * REGION_GRID_HEIGHT;
    uDataLen += sizeof(int);
    uDataLen += uExtNormalCellDataSize * nExtendNormalCellCount;
    uDataLen += sizeof(int);
    uDataLen += uExtDynamicCellDataSize * nExtendDynamicCellCount;
    uDataLen += SCRIPT_DATA_SIZE;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    pRegionHeader = (KRegionHeader*)pbyBuffer;
    pbyBaseCellData = pbyBuffer + sizeof(KRegionHeader);
    pnExtNormalCellCount = (int*)(pbyBaseCellData + uBaseCellDataSize * REGION_GRID_WIDTH * REGION_GRID_HEIGHT);
    pbyExtNormalCellData = (BYTE*)pnExtNormalCellCount + sizeof(int);
    pnExtDynamicCellCount = (int*)(pbyExtNormalCellData + uExtNormalCellDataSize * nExtendNormalCellCount);
    pbyExtDynamicCellData = (BYTE*)pnExtDynamicCellCount + sizeof(int);
    pdwScriptData = (DWORD*)(pbyExtDynamicCellData + uExtDynamicCellDataSize * nExtendDynamicCellCount);

	pRegionHeader->nVersion     = eTerrainDataVersion7;
	pRegionHeader->nRegionX     = m_nRegionX;
	pRegionHeader->nRegionY     = m_nRegionY;
	pRegionHeader->nReserved    = 0;

	for (int nCellY = 0; nCellY < REGION_GRID_HEIGHT; nCellY++)
	{
		for (int nCellX = 0; nCellX < REGION_GRID_WIDTH; nCellX++)
		{
			KCell*              pCell           = GetLowestObstacle(nCellX, nCellY);
            KCell::KBaseInfo*   pBaseInfo       = NULL;
            WORD*               pwLowLayer      = NULL;
            WORD*               pwHighLayer     = NULL;
            WORD*               pwGroupIndex    = NULL;

            pBaseInfo = (KCell::KBaseInfo*)pbyBaseCellData;
            pbyBaseCellData+= sizeof(KCell::KBaseInfo);
            *pBaseInfo = pCell->m_BaseInfo;

            pwHighLayer = (WORD*)pbyBaseCellData;
            pbyBaseCellData += sizeof(WORD);
            *pwHighLayer = pCell->m_wHighLayer;
         
            pCell = pCell->m_pNext;

            while (pCell)
            {
                if (pCell->m_BaseInfo.dwDynamic)
                {
                    *pbyExtDynamicCellData = (BYTE)nCellX;
                    pbyExtDynamicCellData++;

                    *pbyExtDynamicCellData = (BYTE)nCellY;
                    pbyExtDynamicCellData++;

                    pBaseInfo = (KCell::KBaseInfo*)pbyExtDynamicCellData;
                    pbyExtDynamicCellData += sizeof(KCell::KBaseInfo);
                    *pBaseInfo = pCell->m_BaseInfo;

                    pwHighLayer = (WORD*)pbyExtDynamicCellData;
                    pbyExtDynamicCellData += sizeof(WORD);
                    *pwHighLayer = pCell->m_wHighLayer;

                    pwLowLayer = (WORD*)pbyExtDynamicCellData;
                    pbyExtDynamicCellData += sizeof(WORD);
                    *pwLowLayer = pCell->m_wLowLayer;

                    pwGroupIndex = (WORD*)pbyExtDynamicCellData;
                    pbyExtDynamicCellData += sizeof(WORD);
                    *pwGroupIndex = ((KDynamicCell*)pCell)->m_wGroupIndex;
                }
                else
                {
                    *pbyExtNormalCellData = (BYTE)nCellX;
                    pbyExtNormalCellData++;

                    *pbyExtNormalCellData = (BYTE)nCellY;
                    pbyExtNormalCellData++;

                    pBaseInfo = (KCell::KBaseInfo*)pbyExtNormalCellData;
                    pbyExtNormalCellData += sizeof(KCell::KBaseInfo);
                    *pBaseInfo = pCell->m_BaseInfo;

                    pwHighLayer = (WORD*)pbyExtNormalCellData;
                    pbyExtNormalCellData += sizeof(WORD);
                    *pwHighLayer = pCell->m_wHighLayer;

                    pwLowLayer = (WORD*)pbyExtNormalCellData;
                    pbyExtNormalCellData += sizeof(WORD);
                    *pwLowLayer = pCell->m_wLowLayer;
                }

                pCell = pCell->m_pNext;
            }
		}
	}

    *pnExtNormalCellCount = nExtendNormalCellCount;
    *pnExtDynamicCellCount  = nExtendDynamicCellCount;

    memcpy(pdwScriptData, m_pdwScriptList, SCRIPT_DATA_SIZE);

	g_GetRootPath(szRoot);

	nRetCode = snprintf(szPath, sizeof(szPath), "%s/%s", szRoot, MAP_DIR);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));
#ifdef WIN32
	mkdir(szPath);
#else
    mkdir(szPath, S_IRUSR | S_IWUSR | S_IXUSR);
#endif

	nRetCode = snprintf(szPath, sizeof(szPath), "%s/%s/%s", szRoot, MAP_DIR, m_pScene->m_szName);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));
#ifdef WIN32
    mkdir(szPath);
#else
    mkdir(szPath, S_IRUSR | S_IWUSR | S_IXUSR);
#endif

	nRetCode = snprintf(szPath, sizeof(szPath), "%s/%s/%s/v_%03d", szRoot, MAP_DIR, m_pScene->m_szName, m_nRegionY);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));
#ifdef WIN32
    mkdir(szPath);
#else
    mkdir(szPath, S_IRUSR | S_IWUSR | S_IXUSR);
#endif

	nRetCode = snprintf(
        szPath, sizeof(szPath), 
        "%s/%s/%s/v_%03d/%03d_Region.%s", 
        szRoot, MAP_DIR, m_pScene->m_szName, m_nRegionY, m_nRegionX, MAP_FILE_EXT
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

    nRetCode = CheckUpdate(szPath, pbyBuffer, uDataLen);
    if (nRetCode)
    {
	    pFile = fopen(szPath, "wb");
	    KGLOG_PROCESS_ERROR(pFile);

        uWriteNum = fwrite(pbyBuffer, uDataLen, 1, pFile);
        KGLOG_PROCESS_ERROR(uWriteNum == 1);
    }

    bResult = true;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    KG_COM_RELEASE(piBuffer);
	return bResult;
}


BOOL KRegion::CheckUpdate(const char cszFileName[], BYTE* pbyNewData, size_t uNewDataLen)
{
    BOOL            bResult             = true;
    int             nRetCode            = false;
    IFile*          piFile              = NULL;
    IKG_Buffer*     piBuffer            = NULL;
    BYTE*           pbyBuffer           = NULL;
    size_t          uFileSize           = 0;
    size_t          uReadBytes          = 0;

    
    piFile = g_OpenFile(cszFileName);
    KG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    piBuffer = KG_MemoryCreateBuffer((unsigned)uFileSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

	uReadBytes = piFile->Read(pbyBuffer, (unsigned long)uFileSize);
	KGLOG_PROCESS_ERROR(uReadBytes == uFileSize);

    // 如果数据长度都不一样,那么肯定要更新
    if (uFileSize == uNewDataLen)
    {
        nRetCode = memcmp(pbyBuffer, pbyNewData, uNewDataLen);
        if (nRetCode == 0)
        {
            bResult = false;
        }
    }    

Exit0:
    KG_COM_RELEASE(piBuffer);
    KG_COM_RELEASE(piFile);
    return bResult;
}

void KRegion::SetActive()
{ 
    m_nActiveFrame = g_pSO3World->m_nGameLoop; 
}

BOOL KRegion::IsActive()
{
#ifdef _SERVER
    return (g_pSO3World->m_nGameLoop < m_nActiveFrame + GAME_FPS * 3); 
#else
    return (g_pSO3World->m_nGameLoop < m_nActiveFrame + GAME_FPS / 2); 
#endif
}

BOOL KRegion::AddObstacle(int nXCell, int nYCell, KCell* pCell)
{
	BOOL bFound = false;

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);
	assert(pCell);

	KCell* pInsertPos = GetLowestObstacle(nXCell, nYCell);

	//查找插入的位置
	while (!bFound)
	{
		KGLOG_PROCESS_ERROR(pCell->m_wLowLayer >= pInsertPos->m_wHighLayer);

		if (pInsertPos->m_pNext)
		{
			if (pCell->m_wLowLayer >= pInsertPos->m_wHighLayer &&
				pCell->m_wHighLayer <= pInsertPos->m_pNext->m_wLowLayer)
			{
				bFound = true;
			}
			else
			{
				pInsertPos = pInsertPos->m_pNext;
			}
		}
		else
		{			
			bFound = true;
		}
	}

	//插入链表
	pCell->m_pNext = pInsertPos->m_pNext;
	pInsertPos->m_pNext = pCell;

	return true;

Exit0:

	return false;
}

BOOL KRegion::AddObstacle(int nXCell, int nYCell, const KCell& crCellInfo, int nDynamicObstacleGroupIndex)
{
    BOOL   bResult              = false;
	KCell* pCellNode            = NULL;
    KCell* pLastLowerCell       = NULL;
    KCell* pPreNode             = NULL;
    KCell* pNewCell             = NULL;
    DWORD  dwDynamicCellFlag    = 0;

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	pCellNode = GetLowestObstacle(nXCell, nYCell);

    // 找到最后一个低面<=新Cell的节点,将新Cell插入到它的后面
    if (crCellInfo.m_BaseInfo.dwFaceUp)
    {
        // 对于朝上的Cell,尽量放在上面
        while (pCellNode)
        {
            if (pCellNode->m_wLowLayer > crCellInfo.m_wLowLayer)
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
            if (pCellNode->m_wLowLayer >= crCellInfo.m_wLowLayer)
                break;
            pLastLowerCell = pCellNode;
            pCellNode = pCellNode->m_pNext;
        }
    }

    // 因为至少有个[0, 0]的基本Cell,所以不可能为空
    assert(pLastLowerCell);

    if (nDynamicObstacleGroupIndex != INVALID_OBSTACLE_GROUP)
    {
        KDynamicCell* pDynamicCell = KMemory::RawNew<KDynamicCell>();
        KGLOG_PROCESS_ERROR(pDynamicCell);

        pDynamicCell->m_wGroupIndex = (WORD)nDynamicObstacleGroupIndex;
        dwDynamicCellFlag = 1;
        pNewCell = pDynamicCell;
    }
    else
    {
        pNewCell = KMemory::RawNew<KCell>();
        KGLOG_PROCESS_ERROR(pNewCell);
    }

    pNewCell->m_wLowLayer   = crCellInfo.m_wLowLayer;
    pNewCell->m_wHighLayer  = crCellInfo.m_wHighLayer;
    pNewCell->m_BaseInfo    = crCellInfo.m_BaseInfo;

    pNewCell->m_BaseInfo.dwDynamic = dwDynamicCellFlag;

    pNewCell->m_pNext       = pLastLowerCell->m_pNext;
    pLastLowerCell->m_pNext = pNewCell;

    bResult = true;
Exit0:
	return bResult;
}


KCell* KRegion::RemoveObstacle(int nXCell, int nYCell, int nZ)
{
	BOOL    bFound      = false;
    KCell*  pCell       = NULL;
    KCell*  pRemovePos  = NULL;

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	pRemovePos = GetLowestObstacle(nXCell, nYCell);

	while (!bFound)
	{
		KG_PROCESS_ERROR(pRemovePos->m_pNext);
		if ((nZ / POINT_PER_ALTITUDE) <= pRemovePos->m_pNext->m_wHighLayer)
		{
			KG_PROCESS_ERROR((nZ / POINT_PER_ALTITUDE) >= pRemovePos->m_pNext->m_wLowLayer);
			bFound = true;
		}
		else
		{
			pRemovePos = pRemovePos->m_pNext;
		}
	}

	KG_PROCESS_ERROR(pRemovePos->m_pNext);

	pCell = pRemovePos->m_pNext;
	pRemovePos->m_pNext = pRemovePos->m_pNext->m_pNext;

	return pCell;

Exit0:

	return NULL;
}


BOOL KRegion::GetBaseCellInfo(int nXCell, int nYCell, KCell* pCell)
{
    BOOL    bResult     = false;
    KCell*  pBaseCell   = NULL;

    KGLOG_PROCESS_ERROR(nXCell >= 0 && nXCell < REGION_GRID_WIDTH);
    KGLOG_PROCESS_ERROR(nYCell >= 0 && nYCell < REGION_GRID_HEIGHT);

    pBaseCell = GetLowestObstacle(nXCell, nYCell);
    KGLOG_PROCESS_ERROR(pBaseCell);

	*pCell = *pBaseCell;

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

	pCell = GetLowestObstacle(nXCell, nYCell);

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

	for (int nYCell = 0; nYCell < REGION_GRID_HEIGHT; nYCell++)
	{
		for (int nXCell = 0; nXCell < REGION_GRID_WIDTH; nXCell++)
		{
            KCell* pLowestCell  = GetLowestObstacle(nXCell, nYCell);
            KCell* pPreNode     = NULL;
            KCell* pCellNode    = NULL;
            KCell* pDynamicList = NULL;
            KCell* pLastDynamic = NULL;

            KGLOG_PROCESS_ERROR(pLowestCell && pLowestCell->m_BaseInfo.dwDynamic == 0);

            // 分离动态Cell
            pCellNode = pLowestCell;
            pPreNode  = NULL;

            while (pCellNode)
            {
                if (pCellNode->m_BaseInfo.dwDynamic)
                {
                    pPreNode->m_pNext   = pCellNode->m_pNext;
                    pCellNode->m_pNext  = NULL;

                    if (pLastDynamic)
                    {
                        pLastDynamic->m_pNext = pCellNode;
                    }
                    else
                    {
                        pDynamicList = pCellNode;
                    }

                    pLastDynamic = pCellNode;

                    pCellNode = pPreNode->m_pNext;
                    continue;
                }

                pPreNode  = pCellNode;
                pCellNode = pCellNode->m_pNext;
            }

            MergeCellList(pLowestCell);

            MergeCellList(pDynamicList);

            // 将动态Cell合并到正常Cell
            pCellNode = pDynamicList;
            while (pCellNode)
            {
                KCell* pNext = pCellNode->m_pNext;

                ProcessDynamicCell(pLowestCell, (KDynamicCell*)pCellNode);

                KMemory::RawDelete((KDynamicCell*)pCellNode);

                pCellNode = pNext;
            }
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

void KRegion::MergeCellList(KCell* pHead)
{
    KCell*      pLowestCell         = NULL;
    KCell*      pProcessCell        = NULL;
    KCell*      pPreNode            = NULL;

    pProcessCell = pHead;

    while (pProcessCell)
    {
        KCell*  pCellNode             = pProcessCell->m_pNext;
        DWORD   dwBarrierDirection    = pProcessCell->m_BaseInfo.dwBarrierDirection;
        DWORD   dwFaceUp              = pProcessCell->m_BaseInfo.dwFaceUp;
        int     nProcessCount         = 1;

        while (pCellNode)
        {
            if (pCellNode->m_wLowLayer > pProcessCell->m_wHighLayer)
                break;

            if (pCellNode->m_BaseInfo.dwFaceUp != dwFaceUp)
            {
                dwFaceUp = 0;
            }
            
            assert(pCellNode->m_wLowLayer >= pProcessCell->m_wLowLayer);

            if (pCellNode->m_wHighLayer > pProcessCell->m_wHighLayer)
            {
                pProcessCell->m_wHighLayer       = pCellNode->m_wHighLayer;
                pProcessCell->m_BaseInfo         = pCellNode->m_BaseInfo;
            }

            dwBarrierDirection += pCellNode->m_BaseInfo.dwBarrierDirection;
            nProcessCount++;

            pProcessCell->m_pNext = pCellNode->m_pNext;

            if (pCellNode->m_BaseInfo.dwDynamic)
            {
                KMemory::RawDelete((KDynamicCell*)pCellNode);
            }
            else
            {
                KMemory::RawDelete(pCellNode);
            }

            pCellNode = pProcessCell->m_pNext;
        }

        pProcessCell->m_BaseInfo.dwBarrierDirection = dwBarrierDirection / nProcessCount;
        pProcessCell->m_BaseInfo.dwFaceUp           = dwFaceUp;

        pProcessCell = pProcessCell->m_pNext;
    }

    // ---------- 合并上下面Cell ------------------------------
    pPreNode     = NULL;
    pProcessCell = pHead;
    
    while (pProcessCell)
    {
        if (pProcessCell->m_BaseInfo.dwFaceUp && pPreNode != NULL)
        {
            pProcessCell->m_BaseInfo.dwFaceUp = 0;

            pPreNode->m_wHighLayer  = pProcessCell->m_wHighLayer;
            pPreNode->m_BaseInfo    = pProcessCell->m_BaseInfo;
            pPreNode->m_pNext       = pProcessCell->m_pNext;

            if (pProcessCell->m_BaseInfo.dwDynamic)
            {
                KMemory::RawDelete((KDynamicCell*)pProcessCell);
            }
            else
            {
                KMemory::RawDelete(pProcessCell);
            }
            
            pProcessCell = pPreNode->m_pNext;
            continue;
        }

        pProcessCell->m_BaseInfo.dwFaceUp = 0;

        pPreNode     = pProcessCell;
        pProcessCell = pProcessCell->m_pNext;
    }  
}

BOOL KRegion::ProcessDynamicCell(KCell* pHead, const KDynamicCell* pcCell)
{
    BOOL    bResult     = false;
    KCell*  pCellNode   = NULL;
    KCell*  pNextNode   = NULL;

    pCellNode = pHead;

    while (pCellNode)
    {
        pNextNode = pCellNode->m_pNext;

        if (pNextNode == NULL || pNextNode->m_wLowLayer >= pcCell->m_wLowLayer)
        {
            if (pCellNode->m_wHighLayer <= pcCell->m_wHighLayer)
            {
                WORD wLowerLayer = max(pcCell->m_wLowLayer, pCellNode->m_wHighLayer);
                WORD wHigherLayer = pcCell->m_wHighLayer;

                if (pNextNode && pNextNode->m_wLowLayer < wHigherLayer)
                {
                    wHigherLayer = pNextNode->m_wLowLayer;
                }

                if (wLowerLayer < wHigherLayer)
                {
                    KDynamicCell* pNewCell = KMemory::RawNew<KDynamicCell>();

                    KGLOG_PROCESS_ERROR(pNewCell);

                    pNewCell->m_wLowLayer = wLowerLayer;
                    pNewCell->m_wHighLayer = wHigherLayer;
                    pNewCell->m_BaseInfo = pcCell->m_BaseInfo;
                    pNewCell->m_wGroupIndex = pcCell->m_wGroupIndex;

                    pCellNode->m_pNext = pNewCell;
                    pNewCell->m_pNext = pNextNode;
                    
                    pCellNode = pNextNode;

                    continue;
                }
            }
        }

        pCellNode = pCellNode->m_pNext;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRegion::SetWaterHeight(int nXCell, int nYCell, WORD wWaterHeight)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pPreNode    = NULL;
    KCell*  pCellNode   = NULL;

    KGLOG_PROCESS_ERROR(nXCell >= 0 && nXCell < REGION_GRID_WIDTH);
    KGLOG_PROCESS_ERROR(nYCell >= 0 && nYCell < REGION_GRID_HEIGHT);

    // 清除原来的水的Cell
    pCellNode = GetLowestObstacle(nXCell, nYCell);

    while (pCellNode)
    {
        if (pCellNode->m_BaseInfo.dwCellType == ctWater && pPreNode)
        {
            pPreNode->m_pNext = pCellNode->m_pNext;
            assert(!pCellNode->m_BaseInfo.dwDynamic);
            KMemory::RawDelete(pCellNode);
            pCellNode = pPreNode->m_pNext;
            continue;
        }

        pPreNode = pCellNode;
        pCellNode = pCellNode->m_pNext;
    }

    // 在水平面以下的Cell空隙之间填满水Cell

    pCellNode = GetLowestObstacle(nXCell, nYCell);

    while (pCellNode)
    {
        KCell*  pNext               = NULL;
        WORD    wWaterCellHighLayer = 0;

        if (pCellNode->m_wHighLayer >= wWaterHeight)
            break;

        assert(pCellNode->m_wLowLayer < wWaterHeight);

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
                        
            pNewCell = KMemory::RawNew<KCell>();
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
	KCell* pCell = GetLowestObstacle(nXCell, nYCell);

    assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

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
	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

    return m_pCells + REGION_GRID_WIDTH * nYCell + nXCell;
}

KCell* KRegion::GetLowerObstacle(int nXCell, int nYCell, int nZ)
{
    KCell*  pResult     = NULL;
	BOOL    bFound      = false;
    KCell*  pCellNode   = NULL;
    int     nAltitude   = nZ / POINT_PER_ALTITUDE;

    // 从最下面的一个Cell开始找,找到最上一个 "上表面<=nZ" 的Cell,也就是链表中最后一个 "上表面<=nZ" 的Cell
    // 如果所有Cell上表面都高于nZ,则返回NULL

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	pCellNode = GetLowestObstacle(nXCell, nYCell);

	while (pCellNode)
	{
        // Cell是越来越高的,不用往后面找了
		if (pCellNode->m_wHighLayer > nAltitude)
            break;

		pResult		= pCellNode;
	    pCellNode	= GetNextCell(pCellNode, false);
	}

Exit0:
	return pResult;
}

KCell* KRegion::GetAdaptedCellForObj(
    int nXCell, int nYCell, int nFootPosZ, int nClimb, 
    int nHeight, BOOL bIgnoreDynamicSwitch,
    int* pnRelativeZ, int *pnObstacleDirection
)
{
    KCell*  pResult         = NULL;
    KCell*  pPossibleCell   = NULL;
    int     nFootPosMin     = INT_MAX;
    int     nRelativeZ      = 0;

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
 	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);
    assert(pnRelativeZ);

    for (
        KCell* pCellNode = GetLowestObstacle(nXCell, nYCell); 
        pCellNode != NULL;
        pCellNode = GetNextCell(pCellNode, bIgnoreDynamicSwitch)
    )
    {
        if (pCellNode->m_BaseInfo.dwCellType == ctWater)
        {
            if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE > nFootPosZ + nClimb)
                break;

            if (pCellNode->m_BaseInfo.dwBlockCharacter)
                continue;
            
            KCell* pNextRealCell = NULL;
            KCell* pIter         = GetNextCell(pCellNode, bIgnoreDynamicSwitch);

            // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
            while (pIter)
            {
                if (pIter->m_BaseInfo.dwCellType != ctWater)
                {
                    pNextRealCell = pIter;
                    break;
                }
                pIter = GetNextCell(pIter, bIgnoreDynamicSwitch);
            }

            if (pNextRealCell)
            {
                if (pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ + nHeight / 2)
                    continue;

                if ((pNextRealCell->m_wLowLayer - pCellNode->m_wLowLayer) * POINT_PER_ALTITUDE < nHeight)
                    continue;
            }

            pPossibleCell = pCellNode;

            if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ)
                nRelativeZ = nFootPosZ - pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
            else
                nRelativeZ = (pCellNode->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE;
        }
        else
        {
            if (pCellNode->m_wHighLayer * POINT_PER_ALTITUDE > nFootPosZ + nClimb)
            {
                if (pCellNode->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ + nHeight && pnObstacleDirection)
                    *pnObstacleDirection = pCellNode->GetBarrierDirection();
                break;
            }

            if (pCellNode->m_BaseInfo.dwBlockCharacter)
                continue;
            
            KCell* pNextRealCell = NULL;
            KCell* pIter         = GetNextCell(pCellNode, bIgnoreDynamicSwitch);

            // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
            while (pIter)
            {
                if (pIter->m_BaseInfo.dwCellType != ctWater)
                {
                    pNextRealCell = pIter;
                    break;
                }
                pIter = GetNextCell(pIter, bIgnoreDynamicSwitch);
            }

            if (pNextRealCell)
            {
                if (pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE < nFootPosZ + nHeight / 2)  // Can't go down
                    continue;

                if ((pNextRealCell->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE < nHeight)   // Not enough space
                    continue;
            }

            if (pCellNode->m_wHighLayer * POINT_PER_ALTITUDE >= nFootPosMin)    // Go the lowest possible cell
                break;

            nFootPosMin = pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
            pPossibleCell = pCellNode;

            if (pCellNode->m_wHighLayer * POINT_PER_ALTITUDE < nFootPosZ)
                nRelativeZ  = nFootPosZ - pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
            else
                nRelativeZ = 0;
        }
    }

    KG_PROCESS_ERROR(pPossibleCell);

    pResult         = pPossibleCell;
    *pnRelativeZ    = nRelativeZ;
Exit0:
    return pResult;
}

KCell* KRegion::GetProximalCellForObj(
    int nXCell, int nYCell, int nFootPosZ, int nHeight, 
    BOOL bIgnoreDynamicSwitch,
    BOOL bIgnoreBlock, 
    int* pnRelativeZ
)
{
    KCell*  pResult         = NULL;
    KCell*  pPossibleCell   = NULL;
    KCell*  pCellNode       = NULL;
    int     nRelativeZ      = 0;
    int     nAbsolutePosZ   = 0;

    assert(pnRelativeZ);
	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

    pCellNode = GetLowestObstacle(nXCell, nYCell);
    while (pCellNode)
    {
        int    nTempPosZ     = 0;
        BOOL   bStandable    = false;

        if (pCellNode->m_BaseInfo.dwCellType == ctWater)
        {
            if (bIgnoreBlock || !pCellNode->m_BaseInfo.dwBlockCharacter)
            {
                KCell* pNextRealCell = NULL;
                KCell* pIter         = GetNextCell(pCellNode, bIgnoreDynamicSwitch);

                // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
                while (pIter)
                {
                    if (pIter->m_BaseInfo.dwCellType != ctWater)
                    {
                        pNextRealCell = pIter;
                        break;
                    }
                    pIter = GetNextCell(pIter, bIgnoreDynamicSwitch);
                }

                if (pNextRealCell)
                {
                    if ((pNextRealCell->m_wLowLayer - pCellNode->m_wLowLayer) * POINT_PER_ALTITUDE >= nHeight)
                    {
                        bStandable = true;

                        if (nFootPosZ >= pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE - nHeight)
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
            if (bIgnoreBlock || !pCellNode->m_BaseInfo.dwBlockCharacter)
            {
                KCell* pNextRealCell = NULL;
                KCell* pIter         = GetNextCell(pCellNode, bIgnoreDynamicSwitch);

                // 找到下一个非水的Cell,看与这个Cell之间的空间够不够
                while (pIter)
                {
                    if (pIter->m_BaseInfo.dwCellType != ctWater)
                    {
                        pNextRealCell = pIter;
                        break;
                    }
                    pIter = GetNextCell(pIter, bIgnoreDynamicSwitch);
                }

                if (pNextRealCell)
                {
                    if ((pNextRealCell->m_wLowLayer - pCellNode->m_wHighLayer) * POINT_PER_ALTITUDE >= nHeight)
                    {
                        bStandable = true;

                        if (nFootPosZ >= pNextRealCell->m_wLowLayer * POINT_PER_ALTITUDE - nHeight)
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

        pCellNode = GetNextCell(pCellNode, bIgnoreDynamicSwitch);
    }


    KG_PROCESS_ERROR(pPossibleCell);

    nRelativeZ      = nAbsolutePosZ - pPossibleCell->m_wHighLayer * POINT_PER_ALTITUDE;

    pResult         = pPossibleCell;
    *pnRelativeZ    = nRelativeZ;
Exit0:
    return pResult;
}

KCell* KRegion::GetPreCell(int nCellX, int nCellY, KCell* pCell)
{
    KCell* pCellNode = GetLowestObstacle(nCellX, nCellY);

    while (pCellNode && pCellNode->m_pNext != pCell)
    {
        pCellNode = pCellNode->m_pNext;
    }

    if (pCellNode && pCellNode->m_BaseInfo.dwDynamic)
    {
        KDynamicCell* pDynamicCell = (KDynamicCell*)pCellNode;

        if (!m_pScene->GetDynamicObstacleState(pDynamicCell->m_wGroupIndex))
        {
            return GetPreCell(nCellX, nCellY, pCellNode);
        }
    }

    return pCellNode;
}

KCell* KRegion::GetNextCell(KCell* pCell, BOOL bIgnoreDynamicSwitch)
{
    KCell*  pCellNode     = NULL;

    pCellNode = pCell->m_pNext;

    while (pCellNode && pCellNode->m_BaseInfo.dwDynamic)
    {
        // 忽略动态障碍时,动态障碍一律视为disable
        if (!bIgnoreDynamicSwitch)
        {
            KDynamicCell* pDynamicCell   = (KDynamicCell*)pCellNode;
            BOOL bDynamicSwitch = m_pScene->GetDynamicObstacleState(pDynamicCell->m_wGroupIndex);
            if (bDynamicSwitch)
            {
                return pCellNode;
            }
        }

        pCellNode = pCellNode->m_pNext;
    }

    return pCellNode;
}

BOOL KRegion::CheckObstacle(int nXCell, int nYCell, int nLayer)
{
	BOOL    bFound = false;
    KCell*  pCell  = NULL;

	KG_PROCESS_SUCCESS(nXCell < 0);
	KG_PROCESS_SUCCESS(nXCell >= REGION_GRID_WIDTH);
	KG_PROCESS_SUCCESS(nYCell < 0);
	KG_PROCESS_SUCCESS(nYCell >= REGION_GRID_HEIGHT);

	pCell = GetLowestObstacle(nXCell, nYCell);
	while (pCell)
	{
		if (nLayer >= pCell->m_wLowLayer)
		{
			if (nLayer < pCell->m_wHighLayer)
				return true;
			else
				pCell = pCell->m_pNext;
		}
		else
		{
			return false;
		}
	}

Exit0:
	return false;

Exit1:
	// 非法区域全算障碍
	return true;
}


DWORD KRegion::GetScriptIDByIndex(DWORD dwIndex)
{
    assert(m_pdwScriptList);

	if (dwIndex < SCRIPT_COUNT_PER_REGION)
    {
	    return m_pdwScriptList[dwIndex];
    }

    return 0;
}

BOOL KRegion::SetScriptID(KCell* pCell, DWORD dwScriptID)
{
	assert(pCell);

	int nScriptIndex = 1;
	int nInsertIndex = 0;

	if (!dwScriptID)
	{
		pCell->m_BaseInfo.dwScriptIndex = 0;
		goto Exit1;
	}

	for (; nScriptIndex < SCRIPT_COUNT_PER_REGION; nScriptIndex++)
	{
		if (m_pdwScriptList[nScriptIndex] == dwScriptID)
		{
			pCell->m_BaseInfo.dwScriptIndex = nScriptIndex;
			break;
		}
		if (nInsertIndex == 0 && m_pdwScriptList[nScriptIndex] == 0)
		{
			nInsertIndex = nScriptIndex;
		}
	}

	if (nScriptIndex == SCRIPT_COUNT_PER_REGION)
	{
		KGLOG_PROCESS_ERROR(nInsertIndex > 0 && nInsertIndex < SCRIPT_COUNT_PER_REGION);
		m_pdwScriptList[nInsertIndex] = dwScriptID;
		pCell->m_BaseInfo.dwScriptIndex = nInsertIndex;
	}

Exit1:

	return true;

Exit0:

	return false;
}

int KRegion::RecycleScriptIndex()
{
	int nResult = 0;
	int nScriptRefCount[SCRIPT_COUNT_PER_REGION];

	memset(nScriptRefCount, 0, sizeof(nScriptRefCount));

    // 检查每个脚本被引用了多少次
	for (int nCellY = 0; nCellY < REGION_GRID_HEIGHT; nCellY++)
	{
		for (int nCellX = 0; nCellX < REGION_GRID_WIDTH; nCellX++)
		{
            KCell* pCell = GetLowestObstacle(nCellX, nCellY);
            while (pCell)
            {
                int nIndex = pCell->m_BaseInfo.dwScriptIndex;
                assert(nIndex >= 0 && nIndex < SCRIPT_COUNT_PER_REGION);
			    nScriptRefCount[nIndex]++;
                pCell = pCell->m_pNext;
            }
		}
	}

	for (int i = 1; i < SCRIPT_COUNT_PER_REGION; i++)
	{
		if (nScriptRefCount[i] == 0)
		{
			m_pdwScriptList[i] = 0;
		}

		if (m_pdwScriptList[i] == 0)
		{
			nResult++;
		}	
	}

	return nResult;
}


BOOL KRegion::ClearAllScript()
{
    for (int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
    {
        for (int nX = 0; nX < REGION_GRID_WIDTH; nX++)
        {
            KCell* pCell = GetLowestObstacle(nX, nY);

            while (pCell)
            {
                pCell->m_BaseInfo.dwScriptIndex = 0;
                pCell = pCell->m_pNext;
            }
        }
    }

    memset(m_pdwScriptList, 0, SCRIPT_DATA_SIZE);

    return true;
}

BOOL KRegion::ClearScriptSpecial(DWORD dwScriptID)
{
    BOOL    bResult     = false;    
    DWORD   dwIndex     = 0;

    KG_PROCESS_ERROR(dwScriptID != 0);

    for (int i = 1; i < SCRIPT_COUNT_PER_REGION; i++)
    {
        if (m_pdwScriptList[i] == dwScriptID)
        {
            m_pdwScriptList[i]   = 0;
            dwIndex             = (DWORD)i;
            break;
        }
    }

    KG_PROCESS_ERROR(dwIndex >= 1 && dwIndex < SCRIPT_COUNT_PER_REGION);

    for (int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
    {
        for (int nX = 0; nX < REGION_GRID_WIDTH; nX++)
        {
            KCell* pCell = GetLowestObstacle(nX, nY);

            while (pCell)
            {
                if (pCell->m_BaseInfo.dwScriptIndex == dwIndex)
                {
                    pCell->m_BaseInfo.dwScriptIndex = 0;
                }
                pCell = pCell->m_pNext;
            }
        }
    }

    bResult = true;
Exit0:
    return bResult;
}


BOOL KRegion::AddNpc(KNpc* pNpc)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCell       = NULL;
    int     nRelativeZ  = 0;

    assert(IS_NPC(pNpc->m_dwID));

#ifdef _SERVER
	if (pNpc->m_bFightState || pNpc->m_AIData.nPatrolPathID > 0)
	{
		SetActive();
	}
#endif
	pCell = GetProximalCellForObj(
        pNpc->m_nXCell, pNpc->m_nYCell, pNpc->m_nZ, 
        pNpc->m_nHeight, false, false, 
        &nRelativeZ
    );
    KGLOG_PROCESS_ERROR(pCell);

	m_NpcList.AddTail((KNode*)&pNpc->m_RegionObjNode);
	pNpc->m_pRegion = this;

    pNpc->m_nZ = pCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

	pNpc->SetCell(pCell);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::AddDoodad(KDoodad* pDoodad)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCell       = NULL;
    int     nRelativeZ  = 0;

	pCell = GetProximalCellForObj(
        pDoodad->m_nXCell, pDoodad->m_nYCell, pDoodad->m_nZ, 
        pDoodad->m_nHeight, false, false, 
        &nRelativeZ
    );
    KGLOG_PROCESS_ERROR(pCell);

	m_DoodadList.AddTail((KNode*)&pDoodad->m_RegionObjNode);
	pDoodad->m_pRegion = this;

    // 保证编辑器中的坐标位置就是场景中的坐标位置，不要根据CELL信息修正。
    //pDoodad->m_nZ = pCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

    pDoodad->SetCell(pCell);
    
    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::AddPlayer(KPlayer* pPlayer)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    KCell*  pCell       = NULL;
    int     nRelativeZ  = 0;

    assert(IS_PLAYER(pPlayer->m_dwID));

	pCell = GetProximalCellForObj(
        pPlayer->m_nXCell, pPlayer->m_nYCell, pPlayer->m_nZ, 
        pPlayer->m_nHeight, false, true, 
        &nRelativeZ
    );
    KGLOG_PROCESS_ERROR(pCell);

	m_PlayerList.AddTail((KNode*)&pPlayer->m_RegionObjNode);
	pPlayer->m_pRegion = this;

    pPlayer->m_nZ = pCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

    pPlayer->SetCell(pCell);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::RemoveNpc(KNpc* pNpc)
{
    pNpc->SetCell(NULL);

	pNpc->m_RegionObjNode.Remove();
	pNpc->m_pRegion = NULL;

	return true;
}

BOOL KRegion::RemoveDoodad(KDoodad* pDoodad)
{
    pDoodad->SetCell(NULL);

	pDoodad->m_RegionObjNode.Remove();
	pDoodad->m_pRegion = NULL;

	return true;
}

BOOL KRegion::RemovePlayer(KPlayer* pPlayer)
{
    pPlayer->SetCell(NULL);

    pPlayer->m_RegionObjNode.Remove();
	pPlayer->m_pRegion = NULL;

    return true;
}

BOOL KRegion::DeleteAllNpc(void)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KSceneObjNode* pNode = (KSceneObjNode*)m_NpcList.GetHead();
	while (pNode != NULL)
	{
		KSceneObjNode* pNextNode = (KSceneObjNode*)pNode->GetNext();

		KNpc* pNpc = (KNpc*)(pNode->m_pSceneObject);
		KGLOG_PROCESS_ERROR(pNpc);

		bRetCode = g_pSO3World->DeleteNpc(pNpc);
		KGLOG_PROCESS_ERROR(bRetCode);

		pNode = pNextNode;
	}

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::DeleteAllDoodad(void)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KSceneObjNode* pNode = (KSceneObjNode*)m_DoodadList.GetHead();
	while (pNode != NULL)
	{
		KSceneObjNode* pNextNode = (KSceneObjNode*)pNode->GetNext();

		KDoodad* pDoodad = (KDoodad*)(pNode->m_pSceneObject);
		KGLOG_PROCESS_ERROR(pDoodad);

		bRetCode = g_pSO3World->DelDoodad(pDoodad);
		KGLOG_PROCESS_ERROR(bRetCode);

		pNode = pNextNode;
	}

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::DeleteAllPlayer(void)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KSceneObjNode* pNode = (KSceneObjNode*)m_PlayerList.GetHead();
	while (pNode != NULL)
	{
		KSceneObjNode* pNextNode = (KSceneObjNode*)pNode->GetNext();

		KPlayer* pPlayer = (KPlayer*)(pNode->m_pSceneObject);
		KGLOG_PROCESS_ERROR(pPlayer);

		bRetCode = g_pSO3World->DelPlayer(pPlayer);
		KGLOG_PROCESS_ERROR(bRetCode);

		pNode = pNextNode;
	}

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRegion::DeleteAllObject(void)
{
	BOOL bRetCode = false;

	bRetCode = DeleteAllNpc();
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = DeleteAllDoodad();
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = DeleteAllPlayer();
	KGLOG_CHECK_ERROR(bRetCode);

	return true;
}

BOOL KRegion::DeleteAllCell(void)
{
    BOOL bResult = false;

	for (int nXCell = 0; nXCell < REGION_GRID_WIDTH; nXCell++)
	{
		for (int nYCell = 0; nYCell < REGION_GRID_HEIGHT; nYCell++)
		{
			KCell* pBaseCell = GetLowestObstacle(nXCell, nYCell);
            KCell* pCell     = NULL;

            KGLOG_PROCESS_ERROR(pBaseCell);

            pCell = pBaseCell->m_pNext;

			while (pCell)
			{
				KCell* pNext = pCell->m_pNext;

                if (pCell->m_BaseInfo.dwDynamic)
                {
                    KMemory::RawDelete((KDynamicCell*)pCell);
                }
                else
                {
                    KMemory::RawDelete(pCell);
                }

                pCell = pNext;
			}

            pBaseCell->m_pNext = NULL;
		}
	}

    bResult = true;
Exit0:
	return bResult;
}

#ifdef _SERVER
BOOL KRegisterNewNpcFunc::operator () (KPlayer* pPlayer)
{
    if (m_pNpc->m_dwOwner != ERROR_ID && m_pNpc->m_dwOwner != pPlayer->m_dwID)
        return true;

    std::pair<KPlayer::KVIEW_SYNC_TABLE::iterator, BOOL> InsRet = 
        pPlayer->m_NpcSyncTable.insert(m_pNpc->m_dwID);
    if (InsRet.second)
    {
        pPlayer->m_NpcSyncQueue.push_back(KPlayer::KViewObject(m_pNpc->m_dwID, g_pSO3World->m_nGameLoop + VIEW_SYNC_DELAY));
    }

    if (m_pNpc->m_dwOwner != ERROR_ID)
    {
        // 不可能有多个Owner,不用再遍历下去了
        return false;
    }

    return true;
}

BOOL KRegisterNewDoodadFunc::operator () (KPlayer* pPlayer)
{
    std::pair<KPlayer::KVIEW_SYNC_TABLE::iterator, BOOL> InsRet = 
        pPlayer->m_DoodadSyncTable.insert(m_pDoodad->m_dwID);
    if (InsRet.second)
    {
        pPlayer->m_DoodadSyncQueue.push_back(KPlayer::KViewObject(m_pDoodad->m_dwID, g_pSO3World->m_nGameLoop));
    }

    return true;
}

#endif
