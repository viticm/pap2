#include "stdafx.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KPlayer.h"
#include "KRegion.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KMath.h"
#include "KTerrainData.h"
#include "KRelayClient.h"
#include "KNpcTeam.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "Engine/KMemory.h"
#include "Engine/KG_Time.h"

KScene::KScene()
{
#ifdef _SERVER
    m_pTerrainScene  = NULL;
    m_piNpcFile      = NULL;
    m_piDoodadFile   = NULL;
#endif
}

KScene::~KScene()
{
}

BOOL KScene::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = g_pSO3World->m_SceneSet.Register(this, ERROR_ID);
    KGLOG_PROCESS_ERROR(bRetCode);

	m_dwMapID			    = 0;
	m_nCopyIndex	        = 0;
	m_eSceneState		    = ssInvalid;
	m_szName[0]			    = '\0';
	m_szFilePath[0]		    = '\0';
    m_nBroadcastRegion      = 1;
    m_dwBanSkillMask        = 0;
    m_dwBattleRelationMask  = 0;
    m_nReviveCycle          = 0;
    m_bDoNotGoThroughRoof   = false;

    m_nInFightPlayerCount   = 0;

#ifdef _SERVER
    m_dwOwnerID             = ERROR_ID;
    m_bSaved                = false;
    m_bProgressChanged      = false;
#endif
    m_bCanPK                = false;

	memset(m_RegionList, 0, sizeof(m_RegionList));
	m_nRegionWidth		    = 0;
	m_nRegionHeight	        = 0;
    m_nTimeStamp            = 0;
	m_dwScriptID		    = 0;
    m_pPartitionTable       = NULL;

    m_DynamicObstacleSwitchTable.reset();

#ifdef _SERVER  
	m_ScriptTimerList.Init(this);
	m_EscortQuestMgr.Init(this);
    m_NpcCustomDataTable.clear();
    m_DoodadCustomDataTable.clear();
    m_NpcReviveManager.Init(this);
    m_DoodadReviveManager.Init(this);
    m_AIPatrolGroupManager.Init(this);
#endif

    bResult = true;
Exit0:
	return bResult;
}

void KScene::UnInit()
{
	DeleteAllRegion();

#ifdef _SERVER
	KG_COM_RELEASE(m_piNpcFile);
	KG_COM_RELEASE(m_piDoodadFile);

    m_NpcCustomDataTable.clear();
    m_DoodadCustomDataTable.clear();
    
    m_EscortQuestMgr.UnInit();
	m_ScriptTimerList.UnInit();

	m_NpcNickNameList.clear();
    m_DoodadNickNameList.clear();

    m_NpcReviveManager.UnInit();
    m_DoodadReviveManager.UnInit();
    m_AIPatrolGroupManager.UnInit();

    KG_COM_RELEASE(m_pTerrainScene);
#endif

    g_pSO3World->m_SceneSet.Unregister(this);
}

int KScene::Activate(void)
{
    int nInFightPlayerCount = 0;

    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
		for (int nX = 0; nX < m_nRegionWidth; nX++)
	    {
		    KRegion* pRegion = m_RegionList[nX][nY];
        
			if (pRegion == NULL)
                continue;

            #ifdef _SERVER
			if (pRegion->IsActive())
			{
				pRegion->Activate();
                nInFightPlayerCount += pRegion->m_nInFightPlayerCount;
			}
            #else
            pRegion->Activate();
            if (!pRegion->IsActive())
			{
				DelRegion(pRegion->m_nRegionX, pRegion->m_nRegionY);
			}
            #endif 
	    }
    }
    
#ifdef _SERVER
    m_NpcReviveManager.Activate();
	m_DoodadReviveManager.Activate();

	m_ScriptTimerList.Activate();

    if ((g_pSO3World->m_nGameLoop - m_dwID) % (GAME_FPS * 3) == 0)
    {
        m_EscortQuestMgr.Activate();

        if (m_bProgressChanged)
        {
            g_RelayClient.DoSaveScenePlayerList(this);
            g_RelayClient.DoSaveSceneData(this);
            m_bSaved = true;
	        
            KGLogPrintf(KGLOG_INFO, "Save map %s(%.2u, %d)\n", m_szName, m_dwMapID, m_nCopyIndex);
            
            m_bProgressChanged = false;
        }
    }
#endif

    m_nInFightPlayerCount = nInFightPlayerCount;
    return true;
}

BOOL KScene::DelRegion(int nRegionX, int nRegionY)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
    KRegion*    pRegion     = NULL;

	assert(nRegionX >= 0 && nRegionX < MAX_REGION_WIDTH);
	assert(nRegionY >= 0 && nRegionY < MAX_REGION_HEIGHT);

	pRegion = m_RegionList[nRegionX][nRegionY];
    KG_PROCESS_ERROR(pRegion); // 在客户端,大部分Region都是没有加载的,正常情况!

	bRetCode = pRegion->DeleteAllObject();
	KGLOG_PROCESS_ERROR(bRetCode);

#if defined(_CLIENT)
    // 服务端的Cell由于共享障碍的原因,另外管理
	bRetCode = pRegion->DeleteAllCell();
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

	m_RegionList[nRegionX][nRegionY] = NULL;
    
    KMemory::Delete(pRegion);
    pRegion = NULL;

    bResult = true;
Exit0:
	return bResult;
}

BOOL KScene::DeleteAllObstacle(void)
{
	BOOL bRetCode = false;

	for (int nWidth = 0; nWidth < m_nRegionWidth; nWidth++)
	{
		for (int nHeight = 0; nHeight < m_nRegionHeight; nHeight++)
		{
			KRegion* pRegion = GetRegion(nWidth, nHeight);
			if (pRegion)
			{
				bRetCode = pRegion->DeleteAllCell();
				KGLOG_CHECK_ERROR(bRetCode);
			}
		}
	}

	return true;
}


BOOL KScene::DeleteAllRegion(void)
{
	BOOL bRetCode = false;

	for (int nWidth = 0; nWidth < m_nRegionWidth; nWidth++)
	{
		for (int nHeight = 0; nHeight < m_nRegionHeight; nHeight++)
		{
			DelRegion(nWidth, nHeight);
		}
	}

	m_nRegionWidth = 0;
	m_nRegionHeight = 0;

	return true;
}

struct cell_ltcmp
{
	bool operator()(KCell* X, KCell* Y) const
	{
		return memcmp(X, Y, sizeof(KCell)) < 0;
	}
};
std::map<KCell*, int, cell_ltcmp>	mapCellList;
std::map<KCell*, int, cell_ltcmp>::iterator it;

#ifdef _SERVER
BOOL KScene::ServerLoad()
{
    BOOL            bResult         = false;
	BOOL            bRetCode        = false;
    KRegion*        pRegion         = NULL;
    BOOL            bRegionInitFlag = false;
    INpcFile*       piNpcFile       = NULL;
    IDoodadFile*    piDoodadFile    = NULL;

    assert(m_pTerrainScene);
    assert(m_pPartitionTable);

    bRetCode = LoadBaseInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

    if (!m_pTerrainScene->m_bLoadFlag)
    {
        bRetCode = m_pTerrainScene->Create(m_nRegionWidth, m_nRegionHeight);
        KGLOG_PROCESS_ERROR(bRetCode);

        //LoadPartitionTable();
    }

    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
	    for (int nX = 0; nX < m_nRegionWidth; nX++)
	    {
		    pRegion = KMemory::New<KRegion>();
		    KGLOG_PROCESS_ERROR(pRegion);

		    bRetCode = pRegion->Init(this, nX, nY);
		    KGLOG_PROCESS_ERROR(bRetCode);
            bRegionInitFlag = true;

            if (!m_pTerrainScene->m_bLoadFlag)
            {
	            bRetCode = pRegion->LoadTerrain();
                KGLOG_PROCESS_ERROR(bRetCode);
            }

		    bRetCode = SetRegion(nX, nY, pRegion);
            KGLOG_PROCESS_ERROR(bRetCode);

            pRegion            = NULL;
            bRegionInitFlag    = false;
	    }
    }

    m_pTerrainScene->m_bLoadFlag = true;

    bRetCode = m_NpcReviveManager.Load(m_szName);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_DoodadReviveManager.Load(m_szName);
    KGLOG_PROCESS_ERROR(bRetCode);

    piNpcFile = ::CreateNpcFileInterface();
    KGLOG_PROCESS_ERROR(piNpcFile);

    piDoodadFile = ::CreateDoodadFileInterface();
    KGLOG_PROCESS_ERROR(piDoodadFile);
    
    bRetCode = piNpcFile->Load(m_szName);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piDoodadFile->Load(m_szName);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_piNpcFile = piNpcFile;
    m_piNpcFile->AddRef();

    m_piDoodadFile = piDoodadFile;
    m_piDoodadFile->AddRef();    

    bResult = true;
Exit0:
    if (bRegionInitFlag)
    {
        pRegion->UnInit();
        bRegionInitFlag = false;
    }

    if (pRegion)
    {
        KMemory::Delete(pRegion);
        pRegion = NULL;
    }

    KG_COM_RELEASE(piNpcFile);
    KG_COM_RELEASE(piDoodadFile);
    return bResult;
}

BOOL KScene::CallInitScript()
{
    BOOL bRetCode   = false;
	char szFileName[PATH_MAX];

	snprintf(szFileName, sizeof(szFileName), "scripts/Map/%s/%s.lua", m_szName, m_szName);
    szFileName[sizeof(szFileName) - 1] = '\0';
	
	m_dwScriptID = g_FileNameHash(szFileName);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_INIT_SCENE))
	{
		int nTopIndex = 0;

		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

		g_pSO3World->m_ScriptCenter.PushValueToStack(this);
		g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_INIT_SCENE, 0);

		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}

    return true;
}

#endif


#if defined(_CLIENT)

BOOL KScene::EditorLoad(FUNC_LOADPROGRESS pProgress)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

    bRetCode = LoadBaseInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

	int Totalcount = m_nRegionHeight * m_nRegionWidth;
	int count = 0;

	for (int nY = 0; nY < m_nRegionHeight; nY++)
	{
		for (int nX = 0; nX < m_nRegionWidth; nX++)
		{
			KRegion*            pRegion = m_RegionList[nX][nY];

            assert(pRegion);

			count++;
			if(pProgress)
			{
				TCHAR name[MAX_PATH];
				wsprintf(name,"Load Logical Region %d of %d",count,Totalcount);
				(*pProgress)(name, count * 1.0F / Totalcount );
			}

			bRetCode = pRegion->LoadTerrain();
			KGLOG_PROCESS_ERROR(bRetCode);
		}
	}

    bResult = true;
Exit0:
    return bResult;
}

#endif


BOOL KScene::Save(const char cszName[], FUNC_LOADPROGRESS pProgress)
{
    BOOL        bResult                     = false;
	BOOL        bRetCode                    = false;

	strncpy(m_szName, cszName, _NAME_LEN);
	m_szName[_NAME_LEN - 1] = 0;

	int nTotalCount = m_nRegionHeight * m_nRegionWidth;
	int nCount = 0;

	for (int nY = 0; nY < m_nRegionHeight; nY++)
	{
		for (int nX = 0; nX < m_nRegionWidth; nX++)
		{
			KRegion* pRegion = m_RegionList[nX][nY];
			
			nCount++;
			if(pProgress)
			{
				(*pProgress)("Save Logical Region",nCount * 1.0F / nTotalCount);
			}

			if (pRegion == NULL)
				continue;

			bRetCode = pRegion->SaveTerrain();
            KGLOG_PROCESS_ERROR(bRetCode);
		}
	}

    bRetCode = SaveBaseInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

static const char szMapBaseInfoSection[]	= "BaseInfo";
static const char szTimeStamp[]             = "TimeStamp";
static const char szRegionCountX[]          = "RegionCountX";
static const char szRegionCountY[]          = "RegionCountY";

BOOL KScene::SaveBaseInfo()
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    IIniFile*   piFile                  = NULL;
    char        szFileName[MAX_PATH];

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.cfg", MAP_DIR, m_szName, m_szName);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piFile = g_CreateIniFile();
	KGLOG_PROCESS_ERROR(piFile);

#ifdef _SERVER
    assert(g_pSO3World && g_pSO3World->m_piRecorderTools);
    m_nTimeStamp = g_pSO3World->m_piRecorderTools->GetTime();
#else
    m_nTimeStamp = KG_GetTickCount();
#endif

	bRetCode = piFile->WriteStruct(szMapBaseInfoSection, szTimeStamp, &m_nTimeStamp, sizeof(m_nTimeStamp));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piFile->WriteInteger(szMapBaseInfoSection, szRegionCountX, m_nRegionWidth);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piFile->WriteInteger(szMapBaseInfoSection, szRegionCountY, m_nRegionHeight);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piFile->Save(szFileName);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piFile);
    return bResult;
}

BOOL KScene::LoadBaseInfo()
{
    BOOL        bResult                 = false;
    int         nRetCode                = 0;
    IIniFile*   piFile                  = NULL;
    char        szFileName[MAX_PATH];

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.cfg", MAP_DIR, m_szName, m_szName);
    szFileName[sizeof(szFileName) - 1] = '\0';

    // 如果打不开这个文件,说明这个场景尚未保存过逻辑数据
	piFile = g_OpenIniFile(szFileName);
    KG_PROCESS_ERROR(piFile);

    nRetCode = piFile->GetStruct(szMapBaseInfoSection, szTimeStamp, &m_nTimeStamp, sizeof(m_nTimeStamp));
    //KGLOG_PROCESS_ERROR(nRetCode == 1);

	nRetCode = piFile->GetInteger(szMapBaseInfoSection, szRegionCountX, 0, &m_nRegionWidth);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	nRetCode = piFile->GetInteger(szMapBaseInfoSection, szRegionCountY, 0, &m_nRegionHeight);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	KGLOG_PROCESS_ERROR(m_nRegionWidth > 0 && m_nRegionWidth <= MAX_REGION_WIDTH);
	KGLOG_PROCESS_ERROR(m_nRegionHeight > 0 && m_nRegionHeight <= MAX_REGION_HEIGHT);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piFile);
    return bResult;
}

BOOL KScene::LoadPartitionTable()
{
    BOOL        bResult                 = false;
    int         nRetCode                = 0;
    ITabFile*   piFile                  = NULL;
    int         nTableHeight            = 0;
    char        szScriptFile[MAX_PATH];
    char        szFileName[MAX_PATH];

    assert(m_pPartitionTable);

	snprintf(szFileName, sizeof(szFileName), "%s/%s/Partition.tab", MAP_DIR, m_szName, m_szName);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piFile = g_OpenTabFile(szFileName);
	KG_PROCESS_ERROR(piFile);

    nTableHeight = piFile->GetHeight();

    for (int nLine = 2; nLine <= nTableHeight; nLine++)
    {
        int         nPartitionIndex         = 0;
        DWORD		dwScriptID              = 0;
        int		    nForbiddenRide          = 0;
        int         nForbiddenPass          = 0;
        int         nForbiddenPK            = 0;
        int         nRevivePosition         = 0;

        nRetCode = piFile->GetInteger(nLine, "Index", 0, &nPartitionIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piFile->GetString(nLine, "Script", "", szScriptFile, sizeof(szScriptFile));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piFile->GetInteger(nLine, "ForbiddenRide", 0, &nForbiddenRide);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piFile->GetInteger(nLine, "ForbiddenPass", 0, &nForbiddenPass);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piFile->GetInteger(nLine, "ForbiddenPK", 0, &nForbiddenPK);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piFile->GetInteger(nLine, "RevivePosition", 0, &nRevivePosition);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nPartitionIndex >= 0 && nPartitionIndex < MAX_PARTITION_COUNT);

        if (szScriptFile[0] != '\0')
        {
            dwScriptID = g_FileNameHash(szScriptFile);
        }

        m_pPartitionTable[nPartitionIndex].dwScriptID       = dwScriptID;
        m_pPartitionTable[nPartitionIndex].dwForbiddenRide  = nForbiddenRide;
        m_pPartitionTable[nPartitionIndex].dwForbiddenPass  = nForbiddenPass;
        m_pPartitionTable[nPartitionIndex].dwForbiddenPK    = nForbiddenPK;
        m_pPartitionTable[nPartitionIndex].dwRevivePosition = nRevivePosition;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piFile);
    return bResult;
}

BOOL KScene::ValidateRegions(int nRegionX, int nRegionY)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    int             nLeft               = nRegionX - m_nBroadcastRegion;
    int             nRight              = nRegionX + m_nBroadcastRegion;
    int             nBottom             = nRegionY - m_nBroadcastRegion;
    int             nTop                = nRegionY + m_nBroadcastRegion;
    KRegion*        pAllocRegion        = NULL;
    BOOL            bAllocRegionInit    = false;

	for (int nX = nLeft; nX <= nRight; nX++)
	{
		for (int nY = nBottom; nY <= nTop; nY++)
		{
            if (nX < 0 || nX >= m_nRegionWidth)
                continue;

            if (nY < 0 || nY >= m_nRegionHeight)
                continue;
			
            KRegion* pRegion = GetRegion(nX, nY);
            if (pRegion != NULL)
            {
                pRegion->SetActive();
                continue;
            }

		    pAllocRegion = KMemory::New<KRegion>();
		    KGLOG_PROCESS_ERROR(pAllocRegion);

		    bRetCode = pAllocRegion->Init(this, nX, nY);
		    KGLOG_PROCESS_ERROR(bRetCode);
            bAllocRegionInit = true;

		    bRetCode = pAllocRegion->LoadTerrain();
		    KGLOG_PROCESS_ERROR(bRetCode);

		    bRetCode = SetRegion(nX, nY, pAllocRegion);
		    KGLOG_PROCESS_ERROR(bRetCode);
            
            pAllocRegion->SetActive();

            pAllocRegion        = NULL;
            bAllocRegionInit    = false;
		}
	}

    bResult = true;
Exit0:
    if (bAllocRegionInit)
    {
        pAllocRegion->UnInit();
        bAllocRegionInit = false;
    }

    if (pAllocRegion)
    {
        KMemory::Delete(pAllocRegion);
        pAllocRegion = NULL;
    }
    return bResult;
}

BOOL KScene::SetCellData(int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, WORD wLowLayer, WORD wHighLayer)
{
	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;
    KCell* pCell = NULL;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	pCell = pRegion->GetLowestObstacle(nXCell, nYCell);

    KGLOG_PROCESS_ERROR(pCell);

	memcpy(&(pCell->m_BaseInfo), pBaseInfo, sizeof(KCell::KBaseInfo));
	pCell->m_wLowLayer = wLowLayer;
	pCell->m_wHighLayer = wHighLayer;

	return true;

Exit0:

	return false;
}

BOOL KScene::GetCellData(int nXCell, int nYCell, KCell::KBaseInfo& rBaseInfo, WORD& rwLowLayer, WORD& rwHighLayer)
{
    BOOL        bResult     = false;
    KCell*      pCell       = NULL;
	KRegion*    pRegion     = NULL;

    pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
	KGLOG_PROCESS_ERROR(pRegion);

	pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);
    KGLOG_PROCESS_ERROR(pCell);

	memcpy(&rBaseInfo, &(pCell->m_BaseInfo), sizeof(KCell::KBaseInfo));
	rwLowLayer = pCell->m_wLowLayer;
	rwHighLayer = pCell->m_wHighLayer;

    bResult = true;
Exit0:
	return bResult;
}


BOOL KScene::GetObstacle(int nXCell, int nYCell, int nZ, KCell::KBaseInfo& rBaseInfo, 
						 WORD& rwLowLayer, WORD& rwHighLayer)
{
	BOOL bRetCode = false;

	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;
    KCell* pCell = NULL;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	pCell = pRegion->GetObstacle(nXCell, nYCell, nZ);
	KGLOG_PROCESS_ERROR(pCell);

	memcpy(&rBaseInfo, &(pCell->m_BaseInfo), sizeof(KCell::KBaseInfo));
	rwLowLayer = pCell->m_wLowLayer;
	rwHighLayer = pCell->m_wHighLayer;

	return true;

Exit0:

	return false;
}

BOOL KScene::CheckObstacle(int nXCell, int nYCell, int nLayer)
{
	BOOL bRetCode = false;

	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KG_PROCESS_SUCCESS(!pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	return pRegion->CheckObstacle(nXCell, nYCell, nLayer);

Exit1: 
	return true;
}


BOOL KScene::ClearAllScript()
{
    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
        for (int nX = 0; nX < m_nRegionWidth; nX++)
        {
            KRegion* pRegion = GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->ClearAllScript();
            }
        }
    }

    return true;
}

BOOL KScene::ClearScriptSpecial(DWORD dwScriptID)
{
    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
        for (int nX = 0; nX < m_nRegionWidth; nX++)
        {
            KRegion* pRegion = GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->ClearScriptSpecial(dwScriptID);
            }
        }
    }

    return true;
}


//重设地图大小
BOOL KScene::Resize(int nRegionWidth, int nRegionHeight, KRESIZE_SCENE_ALIGN_TYPE eAlignType)
{
	BOOL bRetCode = false;

    assert(nRegionWidth > 0);
    assert(nRegionHeight > 0);
	KGLOG_PROCESS_ERROR(nRegionWidth <= MAX_REGION_WIDTH);
	KGLOG_PROCESS_ERROR(nRegionHeight <= MAX_REGION_HEIGHT);
	
	switch (eAlignType)
	{
	case eResizeSceneAlignLeftTop:
		break;
	case eResizeSceneAlignLeftBottom:
		for (int nX = nRegionWidth; nX < m_nRegionWidth; nX++)
		{
			for (int nY = 0; nY < m_nRegionHeight; nY++)
			{
				bRetCode = DelRegion(nX, nY);
				KGLOG_PROCESS_ERROR(bRetCode);
			}
		}

		for (int nX = 0; nX < nRegionWidth; nX++)
		{
			for (int nY = nRegionHeight; nY < m_nRegionHeight; nY++)
			{
				bRetCode = DelRegion(nX, nY);
				KGLOG_PROCESS_ERROR(bRetCode);
			}
		}

		for (int nX = m_nRegionWidth; nX < nRegionWidth; nX++)
		{
			for (int nY = 0; nY < nRegionHeight; nY++)
			{
                KRegion* pRegion = NULL;

				KGLOG_PROCESS_ERROR(m_RegionList[nX][nY] == NULL);

				pRegion = KMemory::New<KRegion>();
				KGLOG_PROCESS_ERROR(pRegion);

				bRetCode = pRegion->Init(this, nX, nY);
				KGLOG_PROCESS_ERROR(bRetCode);

				bRetCode = SetRegion(nX, nY, pRegion);
				KGLOG_PROCESS_ERROR(bRetCode);

                pRegion->SetActive();
			}
		}

		for (int nX = 0; nX < m_nRegionWidth; nX++)
		{
			for (int nY = m_nRegionHeight; nY < nRegionHeight; nY++)
			{
                KRegion* pRegion = NULL;

				KGLOG_PROCESS_ERROR(m_RegionList[nX][nY] == NULL);

				pRegion = KMemory::New<KRegion>();
				KGLOG_PROCESS_ERROR(pRegion);

				bRetCode = pRegion->Init(this, nX, nY);
				KGLOG_PROCESS_ERROR(bRetCode);

				bRetCode = SetRegion(nX, nY, pRegion);
				KGLOG_PROCESS_ERROR(bRetCode);

                pRegion->SetActive();
			}
		}
		break;
	case eResizeSceneAlignRightTop:
		break;
	case eResizeSceneAlignRightBottom:
		break;
	}

	m_nRegionWidth = nRegionWidth;
	m_nRegionHeight = nRegionHeight;

	return true;

Exit0:

	return false;
}

BOOL KScene::IsPointInSceneSpace(int nX, int nY, int nZ)
{
     if (nX < 0 || nX >= CELL_LENGTH * REGION_GRID_WIDTH * m_nRegionWidth)
         return false;

     if (nY < 0 || nY >= CELL_LENGTH * REGION_GRID_HEIGHT * m_nRegionHeight)
         return false;

     if (nZ < 0 || nZ > MAX_Z_POINT)
         return false;

     return true;
}

BOOL KScene::SetRegionWidth(int nWidth)
{
	KGLOG_PROCESS_ERROR(nWidth <= MAX_REGION_WIDTH);

	m_nRegionWidth = nWidth;

	return true;

Exit0:

	return false;
}

BOOL KScene::SetRegionHeight(int nHeight)
{
	KGLOG_PROCESS_ERROR(nHeight <= MAX_REGION_HEIGHT);

	m_nRegionHeight = nHeight;

	return true;

Exit0:

	return false;
}

int KScene::GetRegionWidth(void)
{
	return m_nRegionWidth;
}

int KScene::GetRegionHeight(void)
{
	return m_nRegionHeight;
}

BOOL KScene::SetRegion(int nRegionX, int nRegionY, KRegion* pRegion)
{
	assert(pRegion);
	assert(nRegionX < MAX_REGION_WIDTH);
	assert(nRegionY < MAX_REGION_HEIGHT);
    assert(m_RegionList[nRegionX][nRegionY] == NULL);

	m_RegionList[nRegionX][nRegionY] = pRegion;

	return true;
}

KRegion* KScene::GetRegion(int nRegionX, int nRegionY)
{
	if (nRegionX >= 0 && nRegionX < m_nRegionWidth && nRegionY >= 0 && nRegionY < m_nRegionHeight)
    {
		return m_RegionList[nRegionX][nRegionY];
    }
	return NULL;
}

BOOL KScene::AddNpc(KNpc* pNpc)
{
	BOOL		bResult		= false;
	BOOL		bRetCode	= false;
	KRegion*	pRegion		= NULL;

#ifdef _SERVER
    pNpc->ClearBullet();
    pNpc->InitDialogFlag();
#endif

	pRegion = GetRegionByPoint(pNpc->m_nX, pNpc->m_nY);
    KGLOG_PROCESS_ERROR(pRegion);

	bRetCode = pRegion->AddNpc(pNpc);
	KGLOG_PROCESS_ERROR(bRetCode);

	pNpc->m_pScene = this;

#ifdef _SERVER
    RegisterObjectViewSync<KRegisterNewNpcFunc, KNpc>(pNpc);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KScene::RemoveNpc(KNpc* pNpc)
{
	BOOL        bRetCode = false;
    KRegion*    pRegion  = NULL;

	pRegion = pNpc->m_pRegion;
    assert(pRegion);

	pRegion->RemoveNpc(pNpc);

	pNpc->m_pScene = NULL;

	return true;
}

BOOL KScene::AddDoodad(KDoodad* pDoodad)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KRegion*    pRegion     = NULL;

	pRegion = GetRegionByPoint(pDoodad->m_nX, pDoodad->m_nY);
	KG_PROCESS_ERROR(pRegion);

	bRetCode = pRegion->AddDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_pScene = this;

    SetDynamicObstacleState(pDoodad->m_nObstacleGroup, true);

#ifdef _SERVER
    RegisterObjectViewSync<KRegisterNewDoodadFunc, KDoodad>(pDoodad);
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KScene::RemoveDoodad(KDoodad* pDoodad)
{
    BOOL        bResult    = false;
	BOOL        bRetCode   = false;

    KGLOG_PROCESS_ERROR(pDoodad->m_pRegion);

	bRetCode = pDoodad->m_pRegion->RemoveDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_pScene = NULL;

    SetDynamicObstacleState(pDoodad->m_nObstacleGroup, false);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KScene::AddPlayer(KPlayer* pPlayer)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KRegion*    pRegion     = NULL;

    // 为什么在AddPlayer时清除子弹列表(而不是在RemovePlayer时清除)?
    // 1. 之所以要去清除子弹,是为了避免在场景A中生成的子弹到场景B中去爆炸---否则你可以在一个非战斗地图的门口丢出一个炸弹,
    //    然后迅速进入非战斗地图,这样炸弹就会在非战斗地图里面爆炸(子弹是绑定在施放者身上的).
    // 2. 之所以不在RemovePlayer时清除,是因为在RemovePlayer的时候,可能还处在一个子弹调用流程中,会破坏子弹链表的数据.
#ifdef _SERVER
    pPlayer->ClearBullet();

    for (KPQID_FLAG_VECTOR::iterator it = m_PQIDFlagVector.begin(), itEnd = m_PQIDFlagVector.end(); it != itEnd; ++it)
    {
        KGPQ* pPQ = NULL;

        if (!it->second)
            continue;
        
        pPQ = g_pSO3World->m_PQManager.GetPQ(it->first);
        if (!pPQ)
            continue;

        pPlayer->m_PQList.AddPQ(pPQ->m_dwPQID);

        if (pPQ->m_eOwnerType != pqotBattleField)
            continue;

        g_PlayerServer.DoSyncBattleFieldPQInfo(
            pPlayer->m_nConnIndex, pPQ->m_dwPQID, pPQ->m_pPQTemplate->dwTemplateID, 
            (int)(g_pSO3World->m_nCurrentTime - pPQ->m_nBeginTime), 
            pPQ->m_pPQTemplate->nEndTime == 0 ? -1 : (int)(pPQ->m_nBeginTime + pPQ->m_pPQTemplate->nEndTime - g_pSO3World->m_nCurrentTime)
        );

        for (int i = 0; i < PQ_PARAM_COUNT; i++)
            g_PlayerServer.DoSyncBattleFieldObjective(pPlayer->m_nConnIndex, i, pPQ->m_nPQValue[i], pPQ->m_pPQTemplate->nPQValue[i]);

        if (!pPQ->m_pPQTemplate->bStatistics)
            continue;

        pPQ->m_StatisticsMap[pPlayer->m_dwID];
    }
#endif

	pPlayer->m_pScene = this;

	pRegion = GetRegionByPoint(pPlayer->m_nX, pPlayer->m_nY);
	KGLOG_PROCESS_ERROR(pRegion);

	bRetCode = pRegion->AddPlayer(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    ValidateRegions(pRegion->m_nRegionX, pRegion->m_nRegionY);
    pPlayer->RegisterViewObject(NULL, pRegion);
#endif

#ifdef _CLIENT
	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnAddCharacter(pPlayer);

    if (g_pGameWorldUIHandler)
    {
		KUIEventPlayerEnterScene param = { pPlayer->m_dwID };
		g_pGameWorldUIHandler->OnPlayerEnterScene(param);
	}
#endif

    KGLogPrintf(
        KGLOG_INFO, "Player \"%s\" enter scene \"%s\" (%.2u,%d)\n", 
        pPlayer->m_szName, m_szName, m_dwMapID, m_nCopyIndex
    );

#ifdef _SERVER
    g_RelayClient.DoPlayerEnterSceneNotify(pPlayer->m_dwID, m_dwMapID, m_nCopyIndex);

    // 玩家禁止回滚到进入场景之前
    pPlayer->m_nRecordCount = 0;
#endif

	bResult = true;
Exit0:
	return bResult;
}

void KScene::CallEnterSceneScript(KPlayer* pPlayer)
{
    BOOL bRetCode = false;

    assert(pPlayer);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_ENTER_SCENE))
	{
		int nTopIndex = 0;
		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

		g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
		g_pSO3World->m_ScriptCenter.PushValueToStack(this);

		g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_ENTER_SCENE, 0);

		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}
}

BOOL KScene::RemovePlayer(KPlayer* pPlayer)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KRegion*    pRegion     = pPlayer->m_pRegion;

	assert(pRegion);

	// 触发脚本事件
	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_LEAVE_SCENE))
	{
		int nTopIndex = 0;
		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

		g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
		g_pSO3World->m_ScriptCenter.PushValueToStack(this);

		g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_LEAVE_SCENE, 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}

    bRetCode = pRegion->RemovePlayer(pPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_pScene = NULL;

#ifdef _SERVER
    for (KPQID_FLAG_VECTOR::iterator it = m_PQIDFlagVector.begin(), itEnd = m_PQIDFlagVector.end(); it != itEnd; ++it)
    {
        if (it->second)
            pPlayer->m_PQList.DelPQ(it->first);
    }
#endif

    bResult = true;
Exit0:
	return bResult;
}

void KScene::DeleteAllObjects()
{
    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
        for (int nX = 0; nX < m_nRegionWidth; nX++)
        {
            KRegion* pRegion = GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->DeleteAllObject();
            }
        }
    }
}

void KScene::DeleteAllNpc()
{
    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
        for (int nX = 0; nX < m_nRegionWidth; nX++)
        {
            KRegion* pRegion = GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->DeleteAllNpc();
            }
        }
    }
}

void KScene::DeleteAllDoodad()
{
    for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
        for (int nX = 0; nX < m_nRegionWidth; nX++)
        {
            KRegion* pRegion = GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->DeleteAllDoodad();
            }
        }
    }
}

BOOL KScene::SetDynamicObstacleState(int nGroupIndex, BOOL bState)
{
    if (nGroupIndex >= 0 && nGroupIndex < MAX_OBSTACLE_GROUP)
    {
        m_DynamicObstacleSwitchTable[nGroupIndex] = bState;
        return true;
    }

    return false;
}

BOOL KScene::GetDynamicObstacleState(int nGroupIndex)
{
    if (nGroupIndex >= 0 && nGroupIndex < MAX_OBSTACLE_GROUP)
    {
        return m_DynamicObstacleSwitchTable[nGroupIndex];
    }
    return false;
}

#ifdef _SERVER

BOOL KScene::DeleteNpcNickname(KNpc* pNpc)
{
    BOOL 						bResult = false;
    KNICK_NAME_LIST::iterator 	it;

    it = m_NpcNickNameList.find(pNpc->m_szName);
    KG_PROCESS_ERROR(it != m_NpcNickNameList.end());

    KG_PROCESS_ERROR(it->second == pNpc->m_dwID);

    m_NpcNickNameList.erase(it);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::DeleteDoodadNickname(KDoodad* pDoodad)
{
    BOOL 						bResult = false;
    KNICK_NAME_LIST::iterator 	it;

    it = m_DoodadNickNameList.find(pDoodad->m_szName);
    KG_PROCESS_ERROR(it != m_DoodadNickNameList.end());

    KG_PROCESS_ERROR(it->second == pDoodad->m_dwID);

    m_DoodadNickNameList.erase(it);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::AddReviveNpc(KNpc* pNpc, int nReviveTime)
{
	BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KNpcTeam*   pNpcTeam    = NULL;

    assert(pNpc);

    bRetCode = m_NpcReviveManager.AddNpc(pNpc, nReviveTime);
    KGLOG_PROCESS_ERROR(bRetCode && "Add ReviveList");

    bResult = true;
Exit0:
	return bResult;
}


BOOL KScene::AddReviveDoodad(KDoodad* pDoodad, BOOL bIgnoreReliveTime/* = false*/)
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
    int                 nReviveTime     = 0;
	KDoodadTemplate*    pDoodadTemplate = NULL;

	assert(pDoodad);

	pDoodadTemplate = pDoodad->m_pTemplate;
	if (pDoodadTemplate == NULL)
	{
		pDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pDoodad->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pDoodadTemplate);
	}
	
	if (!bIgnoreReliveTime)
    {
        nReviveTime = pDoodadTemplate->m_nReviveDelay;
    }

    bRetCode = m_DoodadReviveManager.AddDoodad(pDoodad, nReviveTime);
    KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KScene::CompleteLoading()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    
    bRetCode = ActivateAllRegions();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CompleteNpcLoading();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CompleteDoodadLoading();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::ActivateAllRegions()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KRegion*    pRegion     = NULL;

    for (int nY = 0; nY < m_nRegionHeight; ++nY)
    {
	    for (int nX = 0; nX < m_nRegionWidth; ++nX)
	    {
		    pRegion = GetRegion(nX, nY);
            KGLOG_PROCESS_ERROR(pRegion);

            pRegion->SetActive();
	    }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::CompleteNpcLoading()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    KNpc*           pNpc            = NULL;
    BOOL            bNpcRegistered  = false;
    BOOL            bNpcInitFlag    = false;
    int             nProgressID     = INVALID_SCENE_PROGRESS_ID;
    KNPC_DATA       NpcData;
    

    assert(m_piNpcFile);

    while (m_piNpcFile->Pop(&NpcData))
    {
        nProgressID = NpcData.nProgressID;

        if (nProgressID == INVALID_SCENE_PROGRESS_ID)
        {
            KNpcTemplate*   pTemplate       = NULL;
            
            pTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(NpcData.dwTemplateID);
            if (pTemplate == NULL)
            {
                KGLogPrintf(KGLOG_ERR, "Invalid npc template %lu in scene %s", NpcData.dwTemplateID, m_szName);
                continue; 
            }

            nProgressID = pTemplate->nProgressID;
        }
        
        if (nProgressID != INVALID_SCENE_PROGRESS_ID && m_bSaved)
        {
            BOOL bProgressFlag = false;

            KGLOG_PROCESS_ERROR(nProgressID >= 0 && nProgressID <= MAX_SCENE_PROGRESS_ID);

            bRetCode = m_ProgressData.GetBit(nProgressID, &bProgressFlag);
            KGLOG_PROCESS_ERROR(bRetCode);

            if (bProgressFlag) // 进度已经设置，该NPC不应加入场景
            {
                continue;
            }
        }

        bRetCode = IsPointInSceneSpace(NpcData.nX, NpcData.nY, NpcData.nZ);
        if (!bRetCode)
        {
            KGLogPrintf(
                KGLOG_WARNING, "Invalid npc coordinate: (%d, %d, %d)", 
                NpcData.nX, NpcData.nY, NpcData.nZ
            );
            continue;
        }
        
        pNpc = KMemory::New<KNpc>();
        KGLOG_PROCESS_ERROR(pNpc);

        bRetCode = g_pSO3World->m_NpcSet.Register(pNpc, ERROR_ID);
        KGLOG_PROCESS_ERROR(bRetCode);
        bNpcRegistered = true;

        bRetCode = pNpc->Init();
        KGLOG_PROCESS_ERROR(bRetCode);
        bNpcInitFlag = true;

        bRetCode = m_NpcReviveManager.LoadNpc(pNpc, NpcData);
        KGLOG_PROCESS_ERROR(bRetCode);

        if (NpcData.szNickName[0] != '\0')
        {
		    std::map<string, DWORD>::iterator it = m_NpcNickNameList.find(NpcData.szNickName);
		    if (it != m_NpcNickNameList.end())
		    {
                KGLogPrintf(KGLOG_WARNING, "Reduplicated npc nickname: %s", NpcData.szNickName);
		    }
            m_NpcNickNameList[NpcData.szNickName] = pNpc->m_dwID;
        }

        pNpc            = NULL;
        bNpcRegistered  = false;
        bNpcInitFlag    = false;
    }

    bRetCode = m_NpcReviveManager.CompleteLoading();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:

    KG_COM_RELEASE(m_piNpcFile);

    if (!bResult)
    {
        if (bNpcInitFlag)
        {
            pNpc->UnInit();
            bNpcInitFlag = false;
        }

        if (bNpcRegistered)
        {
            g_pSO3World->m_NpcSet.Unregister(pNpc);
            bNpcRegistered = false;
        }

        if (pNpc)
        {
            KMemory::Delete(pNpc);
            pNpc = NULL;
        }
    }
    return bResult;
}

BOOL KScene::CompleteDoodadLoading()
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;
    KDoodad*            pDoodad             = NULL;
    BOOL                bDoodadRegistered   = false;
    BOOL                bDoodadInitFlag     = false;
    int                 nProgressID         = INVALID_SCENE_PROGRESS_ID;
    KDOODAD_DATA        DoodadData;

    assert(m_piDoodadFile);

    while (m_piDoodadFile->Pop(&DoodadData))
    {
        nProgressID = DoodadData.nProgressID;

        if (nProgressID == INVALID_SCENE_PROGRESS_ID)
        {
            KDoodadTemplate* pTemplate = NULL;

            pTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(DoodadData.dwTemplateID);
            if (pTemplate == NULL)
            {
                KGLogPrintf(KGLOG_ERR, "Invalid doodad template %lu in scene %s", DoodadData.dwTemplateID, m_szName);
                continue; 
            }

            nProgressID = pTemplate->m_nProgressID;
        }
        
        if (nProgressID != INVALID_SCENE_PROGRESS_ID && m_bSaved)
        {
            BOOL bProgressFlag = false;

            KGLOG_PROCESS_ERROR(nProgressID >= 0 && nProgressID <= MAX_SCENE_PROGRESS_ID);

            bRetCode = m_ProgressData.GetBit(nProgressID, &bProgressFlag);
            KGLOG_PROCESS_ERROR(bRetCode);

            if (bProgressFlag) // 进度已经设置，该Doodad不应加入场景
            {
                continue;
            }
        }

        bRetCode = IsPointInSceneSpace(DoodadData.nX, DoodadData.nY, DoodadData.nZ);
        if (!bRetCode)
        {
            KGLogPrintf(
                KGLOG_WARNING, "Invalid doodad coordinate: (%d, %d, %d)", 
                DoodadData.nX, DoodadData.nY, DoodadData.nZ
            );
            continue;
        }

        pDoodad = KMemory::New<KDoodad>();
        KGLOG_PROCESS_ERROR(pDoodad);

        bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
        KGLOG_PROCESS_ERROR(bRetCode);
        bDoodadRegistered = true;

        bRetCode = pDoodad->Init();
        KGLOG_PROCESS_ERROR(bRetCode);
        bDoodadInitFlag = true;

        bRetCode = pDoodad->Load(DoodadData);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = AddReviveDoodad(pDoodad, true);
        KGLOG_PROCESS_ERROR(bRetCode);
        
        if (DoodadData.szNickName[0] != '\0')
        {
		    std::map<string, DWORD>::iterator it = m_DoodadNickNameList.find(DoodadData.szNickName);
		    if (it != m_DoodadNickNameList.end())
		    {
                KGLogPrintf(KGLOG_WARNING, "Reduplicated doodad nickname: %s", DoodadData.szNickName);
		    }
            m_DoodadNickNameList[DoodadData.szNickName] = pDoodad->m_dwID;
        }

        pDoodad            = NULL;
        bDoodadRegistered  = false;
        bDoodadInitFlag    = false;
    }

    bRetCode = m_DoodadReviveManager.CompleteLoading();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:

    KG_COM_RELEASE(m_piDoodadFile);

    if (!bResult)
    {
        if (bDoodadInitFlag)
        {
            pDoodad->UnInit();
            bDoodadInitFlag = false;
        }

        if (bDoodadRegistered)
        {
            g_pSO3World->m_DoodadSet.Unregister(pDoodad);
            bDoodadRegistered = false;
        }

        if (pDoodad)
        {
            KMemory::Delete(pDoodad);
            pDoodad = NULL;
        }
    }
    return bResult;
}
#endif

BOOL KScene::PreprocessCalculateData()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = ReviseCalculateGradient();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CalculateObstacleRange();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::ReviseCalculateGradient()
{
    BOOL        bResult             = false;    
    KCell*      pLowestCell         = NULL;
	KRegion*    pProcessRegion      = NULL;
    KCell*      pProcessCell        = NULL;
    int         nProcessPointX      = 0;
    int         nProcessPointY      = 0;
    int         nLowerPointX        = 0;
    int         nLowerPointY        = 0;
    int         nLowerCellX         = 0;
    int         nLowerCellY         = 0;
	KRegion*    pLowerRegion        = NULL;
    KCell*      pLowerCell          = NULL;
    int         nGradientDirection  = 0;
    int         nSceneMaxPointX     = 0;
    int         nSceneMaxPointY     = 0;
    int         nRelativeHeight     = 0;
    int         nDistanceXY         = 0;
    int         nGradientDegree     = 0;
    int         nOriginalGradient   = 0;

    nSceneMaxPointX = m_nRegionWidth * REGION_GRID_WIDTH * CELL_LENGTH;
    nSceneMaxPointY = m_nRegionHeight * REGION_GRID_HEIGHT * CELL_LENGTH;

	for (int nYCell = 0; nYCell < m_nRegionHeight * REGION_GRID_HEIGHT; nYCell++)
	{
		for (int nXCell = 0; nXCell < m_nRegionWidth * REGION_GRID_WIDTH; nXCell++)
		{
            pProcessRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
			KGLOG_PROCESS_ERROR(pProcessRegion);

			pLowestCell = pProcessRegion->GetLowestObstacle(
                nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT
            );
            KGLOG_PROCESS_ERROR(pLowestCell);

            for (pProcessCell = pLowestCell; pProcessCell != NULL; pProcessCell = pProcessCell->m_pNext)
            {
                if (pProcessCell->m_BaseInfo.dwCellType == ctWater)
                    continue;

                nOriginalGradient = pProcessCell->GetGradientDegree();
                if (!nOriginalGradient)
                    continue;

                nGradientDirection = pProcessCell->GetGradientDirection();
                
                nLowerPointX = CELL_LENGTH * nXCell + CELL_LENGTH / 2 + 
                    CELL_LENGTH * g_Cos(nGradientDirection) / SIN_COS_NUMBER;

                nLowerPointY = CELL_LENGTH * nYCell + CELL_LENGTH / 2 + 
                    CELL_LENGTH * g_Sin(nGradientDirection) / SIN_COS_NUMBER;

                if (nLowerPointX < 0 || nLowerPointX >= nSceneMaxPointX)
                    continue;

                if (nLowerPointY < 0 || nLowerPointY >= nSceneMaxPointY)
                    continue;

                nLowerCellX = nLowerPointX / CELL_LENGTH;
                nLowerCellY = nLowerPointY / CELL_LENGTH;

                nLowerPointX = nLowerCellX * CELL_LENGTH + CELL_LENGTH / 2;
                nLowerPointY = nLowerCellY * CELL_LENGTH + CELL_LENGTH / 2;

                pLowerRegion = GetRegionByPoint(nLowerPointX, nLowerPointY);
                KGLOG_PROCESS_ERROR(pLowerRegion);

                pLowerCell = pLowerRegion->GetAdaptedCellForObj(
                    nLowerCellX % REGION_GRID_WIDTH, nLowerCellY % REGION_GRID_HEIGHT,
                    pProcessCell->m_wHighLayer * POINT_PER_ALTITUDE,
                    0, 0, false,
                    &nRelativeHeight, NULL
                );
                if (!pLowerCell)
                {
                    pProcessCell->SetGradientDegree(0);
                    continue;
                }

                if (pLowerCell->m_BaseInfo.dwCellType == ctWater)
                {
                    nRelativeHeight += (pLowerCell->m_wHighLayer - pLowerCell->m_wLowLayer) * POINT_PER_ALTITUDE;
                }

                nProcessPointX = nXCell * CELL_LENGTH + CELL_LENGTH / 2;
                nProcessPointY = nYCell * CELL_LENGTH + CELL_LENGTH / 2;

                nDistanceXY = (int)sqrt(
                    (float)(nLowerPointX - nProcessPointX) * (nLowerPointX - nProcessPointX) +
                    (float)(nLowerPointY - nProcessPointY) * (nLowerPointY - nProcessPointY)
                );

                nGradientDegree = g_GetDirection(
                    0, 0, XYPOINT_TO_ZPOINT(nDistanceXY), nRelativeHeight
                );

                MAKE_IN_RANGE(nGradientDegree, 0, DIRECTION_COUNT / 4 - 1);
                
                pProcessCell->SetGradientDegree(nGradientDegree);
            }
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::CalculateObstacleRange()
{
	int nLastDistance = 0;
	for (int nYCell = 0; nYCell < m_nRegionHeight * REGION_GRID_HEIGHT; nYCell++)
	{
		for (int nXCell = 0; nXCell < m_nRegionWidth * REGION_GRID_WIDTH; nXCell++)
		{
			KRegion* pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
			KGLOG_PROCESS_ERROR(pRegion);

			KCell* pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);
			
			int nDistance = (nLastDistance - 1) >= 0 ? nLastDistance : 0;
			while (nDistance < 64)
			{
				BOOL bIsObstacle = false;

				for (int nDestX = nXCell - nDistance; nDestX < nXCell + nDistance; nDestX++)
				{
					int nDestY = nYCell - nDistance;
					KRegion* pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = pDestRegion->GetLowestObstacle(nDestX % REGION_GRID_WIDTH, nDestY % REGION_GRID_HEIGHT);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || pDestCell->m_pNext != NULL)
						{
							bIsObstacle = true;
							break;
						}
					}

					nDestY = nYCell + nDistance;
					pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = pDestRegion->GetLowestObstacle(nDestX % REGION_GRID_WIDTH, nDestY % REGION_GRID_HEIGHT);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || pDestCell->m_pNext != NULL)
						{
							bIsObstacle = true;
							break;
						}
					}
				}
				if (bIsObstacle)
					break;

				for (int nDestY = nYCell - nDistance; nDestY < nYCell + nDistance; nDestY++)
				{
					int nDestX = nXCell - nDistance;
					KRegion* pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = pDestRegion->GetLowestObstacle(nDestX % REGION_GRID_WIDTH, nDestY % REGION_GRID_HEIGHT);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || pDestCell->m_pNext != NULL)
						{
							bIsObstacle = true;
							break;
						}
					}

					nDestX = nXCell + nDistance;
					pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = pDestRegion->GetLowestObstacle(nDestX % REGION_GRID_WIDTH, nDestY % REGION_GRID_HEIGHT);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || pDestCell->m_pNext != NULL)
						{
							bIsObstacle = true;
							break;
						}
					}
				}
				if (bIsObstacle)
					break;

				nDistance++;
			}

			pCell->m_BaseInfo.dwNoObstacleRange = nDistance < 64 ? nDistance : 63;
			nLastDistance = nDistance;
		}
	}

	return true;

Exit0:

	return false;
}

BOOL KScene::IsVisible(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ)
{
    BOOL        bResult             = false;    // 默认有障碍
    KRegion*    pSrcRegion          = NULL;
    KCell*      pSrcCell            = NULL;
    int         nSrcCellX           = 0;
    int         nSrcCellY           = 0;
    DWORD       dwSrcRegionX        = 0;
    DWORD       dwSrcRegionY        = 0;
    int         nDstCellX           = 0;   
    int         nDstCellY           = 0;   
    DWORD       dwDstRegionX        = 0;
    DWORD       dwDstRegionY        = 0;
    KRegion*    pDstRegion          = NULL;
    KCell*      pDstCell            = NULL;
    int         nNoObstacleRange    = 0;
    int         nDeltaX             = 0;
    int         nDeltaY             = 0;
    int         nLastAltitude       = 0;
    int         nProportionX        = 0;
    int         nProportionY        = 0;
    int         nProportionZ        = 0;
    KRegion*    pRegion             = NULL;
    KCell*      pCell               = NULL; 
    int         nAltitude           = 0;
    int         nXCell              = 0;
    int         nYCell              = 0;
    int         nZCell              = 0;
    DWORD       dwRegionX           = 0;
    DWORD       dwRegionY           = 0;
    DWORD       dwRegionZ           = 0;

    nSrcCellX       = nSrcX / CELL_LENGTH;
    nSrcCellY       = nSrcY / CELL_LENGTH;
	dwSrcRegionX    = nSrcCellX / REGION_GRID_WIDTH;
	dwSrcRegionY    = nSrcCellY / REGION_GRID_HEIGHT;

    pSrcRegion     = GetRegion(dwSrcRegionX, dwSrcRegionY);
    KG_PROCESS_ERROR(pSrcRegion);

	pSrcCell = pSrcRegion->GetLowestObstacle(nSrcCellX % REGION_GRID_WIDTH, nSrcCellY % REGION_GRID_HEIGHT);
	KG_PROCESS_ERROR(pSrcCell);

    nDstCellX       = nDstX / CELL_LENGTH;
    nDstCellY       = nDstY / CELL_LENGTH;
	dwDstRegionX    = nDstCellX / REGION_GRID_WIDTH;
	dwDstRegionY    = nDstCellY / REGION_GRID_HEIGHT;

    pDstRegion = GetRegion(dwDstRegionX, dwDstRegionY);
    KG_PROCESS_ERROR(pDstRegion);

	pDstCell = pDstRegion->GetLowestObstacle(nDstCellX % REGION_GRID_WIDTH, nDstCellY % REGION_GRID_HEIGHT);
	KG_PROCESS_ERROR(pDstCell);

	if (nSrcCellX == nDstCellX && nSrcCellY == nDstCellY)
	{
        // 同一个Cell
        goto Exit1;
	}
  
	// 检测是否在无障碍范围内
	nNoObstacleRange = 
		pSrcCell->m_BaseInfo.dwNoObstacleRange > pDstCell->m_BaseInfo.dwNoObstacleRange ?
		pSrcCell->m_BaseInfo.dwNoObstacleRange : pDstCell->m_BaseInfo.dwNoObstacleRange;

   
    // 采用这个判断时会跳过地形因素造成的不可见,所以暂时去掉
    /*   
	if (abs(nSrcCellX - nDstCellX) < nNoObstacleRange && abs(nSrcCellY - nDstCellY) < nNoObstacleRange)
    {
        // 没有障碍
        goto Exit0;
    }
    */

	//int nDirection = g_GetDirection(nSrcCellX, nSrcCellY, nDstCellX, nDstCellY);

	//if ((nDirection < (DIRECTION_COUNT * 1 / 8) || nDirection > (DIRECTION_COUNT * 7 / 8)) ||
	//	(nDirection > (DIRECTION_COUNT * 3 / 8) && nDirection < (DIRECTION_COUNT * 5 / 8)))

    nDeltaX         = abs(nDstCellX - nSrcCellX);
    nDeltaY         = abs(nDstCellY - nSrcCellY);
	nLastAltitude   = nSrcZ;

    if (nDeltaX > nDeltaY)
	{
		//计算Y轴和Z轴上的比例

        KGLOG_PROCESS_ERROR(nDeltaX);

		nProportionY = (nDstCellY - nSrcCellY) * SIN_COS_NUMBER / (nDstCellX - nSrcCellX);
		nProportionZ = (nDstZ - nSrcZ) * SIN_COS_NUMBER / (nDstCellX - nSrcCellX);

		if ((nSrcCellX < nDstCellX) && (nSrcZ <= nDstZ))
		{
			for (nXCell = (nSrcCellX + 1); nXCell <= nDstCellX; nXCell++)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
				KG_PROCESS_ERROR(pRegion);
					
                pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
                    pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;
			}
		}
        else if ((nSrcCellX < nDstCellX) && (nSrcZ > nDstZ))
        {
			for (nXCell = (nSrcCellX + 1); nXCell <= nDstCellX; nXCell++)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
				KG_PROCESS_ERROR(pRegion);
					
                pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

                nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;
			}
        }
		else if ((nSrcCellX > nDstCellX) && (nSrcZ <= nDstZ))
		{
			for (nXCell = (nSrcCellX - 1); nXCell >= nDstCellX; nXCell--)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);
					
                pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;				
			}
		}
        else if ((nSrcCellX > nDstCellX) && (nSrcZ > nDstZ))
        {
			for (nXCell = (nSrcCellX - 1); nXCell >= nDstCellX; nXCell--)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);
					
                pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;				
			}
        }
	}
	else
	{
		//计算X轴和Z轴的比例

        KGLOG_PROCESS_ERROR(nDeltaY);

		nProportionX = (nDstCellX - nSrcCellX) * SIN_COS_NUMBER / (nDstCellY - nSrcCellY);
		nProportionZ = (nDstZ - nSrcZ) * SIN_COS_NUMBER / (nDstCellY - nSrcCellY);

		if ((nSrcCellY < nDstCellY) && (nSrcZ <= nDstZ))
		{
			for (nYCell = (nSrcCellY + 1); nYCell <= nDstCellY; nYCell++)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

                nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;
			}
		}
        else if ((nSrcCellY < nDstCellY) && (nSrcZ > nDstZ))
        {
			for (nYCell = (nSrcCellY + 1); nYCell <= nDstCellY; nYCell++)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;
			}
        }
		else  if ((nSrcCellY > nDstCellY) && (nSrcZ <= nDstZ))
		{
			for (nYCell = (nSrcCellY - 1); nYCell >= nDstCellY; nYCell--)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude ||
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;
			}
		}
        else if ((nSrcCellY > nDstCellY) && (nSrcZ > nDstZ))
        {
			for (nYCell = (nSrcCellY - 1); nYCell >= nDstCellY; nYCell--)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = pRegion->GetLowestObstacle(nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude ||
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pRegion->GetNextCell(pCell, false);
				}
				nLastAltitude = nAltitude;
			}
        }
	}    

Exit1:
    bResult = true;
Exit0:
    return bResult;
}
