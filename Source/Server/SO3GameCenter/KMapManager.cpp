#include "stdafx.h"
#include "KMapManager.h"
#include "KSO3GameCenter.h"
#include "SO3Result.h"

KMapManager::KMapManager()
{
    m_nPlayerOnlineTotal = -1;
}

BOOL KMapManager::Init()
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    time_t  nTimeNow    = time(NULL);
    
    bRetCode = LoadMapList();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadMapGroup();
    KGLOG_PROCESS_ERROR(bRetCode);

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        m_GroupTable[i].nConnIndex = 0;
    }
    
    bResult = true;
Exit0:
    return bResult;
}

void KMapManager::UnInit()
{
    m_MapTable.clear();
}

void KMapManager::Activate()
{
    HRESULT                     hrResult        = KG_S_FALSE;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piBuffer        = NULL;
    int                         nPlayerCount    = 0;

    for (KMAP_TABLE::iterator MapIter = m_MapTable.begin(); MapIter != m_MapTable.end(); ++MapIter)
    {
        KMapInfo* pMapInfo = &MapIter->second;

        pMapInfo->Activate();

        nPlayerCount += pMapInfo->m_nPlayerCount;
    }

#ifdef WIN32
    if (nPlayerCount != m_nPlayerOnlineTotal)
    {
        char szTitle[64];
        sprintf(szTitle, "Game center for JXOnline III --- %d", nPlayerCount);
        SetConsoleTitle(szTitle);
    }
#endif

    m_nPlayerOnlineTotal = nPlayerCount;

    return;
}

KMapInfo* KMapManager::GetMapInfo(DWORD dwMapID)
{
    KMAP_TABLE::iterator it = m_MapTable.find(dwMapID);
    if (it != m_MapTable.end())
    {
        return &it->second;
    }
	return NULL;
}

KMapCopy* KMapManager::GetMapCopy(DWORD dwMapID, int nMapCopyIndex)
{
    KMapInfo* pMapInfo = GetMapInfo(dwMapID);

    if (pMapInfo)
    {
        return pMapInfo->GetMapCopy(nMapCopyIndex);
    }

    return NULL;
}

void KMapManager::OnGameServerCreate(int nConnIndex)
{
    BOOL       bRetCode     = false;
    KMapGroup* pMapGroup    = NULL;

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GroupTable[i].nConnIndex == 0)
        {
            pMapGroup = &m_GroupTable[i];
            break;
        }
    }

    KG_PROCESS_ERROR(pMapGroup);

    pMapGroup->nConnIndex = nConnIndex;

    for (std::vector<KMAP_GROUP_ITEM>::iterator it = pMapGroup->MapTable.begin(); it != pMapGroup->MapTable.end(); ++it)
    {
        int          nMapCopyIndex    = 0;
        KMapInfo*    pMap             = NULL;
        KMapCopy*    pMapCopy         = NULL;

        nMapCopyIndex = it->nCopySN;

        pMap = GetMapInfo(it->dwMapID);
        KGLOG_PROCESS_ERROR(pMap);

        pMapCopy = GetMapCopy(it->dwMapID, nMapCopyIndex);
        if (pMapCopy)
        {
            assert(pMapCopy->m_eState == eMapStateOffline);
            pMapCopy->m_nConnIndex = nConnIndex;
        }
        else
        {
            pMapCopy = pMap->CreateMapCopy(nMapCopyIndex, g_pSO3GameCenter->m_nTimeNow, 0, nConnIndex, false);
            KGLOG_PROCESS_ERROR(pMapCopy);
        }

		g_pSO3GameCenter->m_GameServer.DoCreateMapNotify(nConnIndex, it->dwMapID, nMapCopyIndex);
        
        pMapCopy->m_eState = eMapStateCreating;
    }

Exit0:
    return;
}

void KMapManager::OnGameServerDestroy(int nConnIndex)
{
    BOOL bRetCode = false;

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GroupTable[i].nConnIndex == nConnIndex)
        {
            m_GroupTable[i].nConnIndex = 0;
        }
    }

    for (KMAP_TABLE::iterator MapIter = m_MapTable.begin(); MapIter != m_MapTable.end(); ++MapIter)
    {
        MapIter->second.DeleteAllCopyOnConnection(nConnIndex);
    }
}

BOOL KMapManager::IsMapCopyInPreloadGroup(DWORD dwMapID, int nCopySN)
{
    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        KMapGroup* pMapGroup    = m_GroupTable + i;
        int        nGroupSize   = (int)pMapGroup->MapTable.size();

        for (int j = 0; j < nGroupSize; j++)
        {
            if (pMapGroup->MapTable[j].dwMapID == dwMapID && pMapGroup->MapTable[j].nCopySN == nCopySN)
            {
                return true;
            }
        }
    }

    return false;
}

BOOL KMapManager::ResetAllMap(DWORD dwPlayerID, std::vector<DWORD>& vecFailedMap)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    
    for (KMAP_TABLE::iterator MapIter = m_MapTable.begin(); MapIter != m_MapTable.end(); ++MapIter)
    {
        KMapInfo* pMapInfo = &MapIter->second;

        if (pMapInfo->m_nType != emtDungeon)
            continue;

        bRetCode = ResetMap(dwPlayerID, pMapInfo->m_dwMapID, 0);
        if (!bRetCode)
        {
            vecFailedMap.push_back(pMapInfo->m_dwMapID);
        }
    }

    bResult = true;
//Exit0:
    return bResult;
}

BOOL KMapManager::ResetMap(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex)
{   
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KMapCopy*   pMapCopy        = NULL;
    KMapInfo*   pMapInfo        = NULL;
    
    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);
    
    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    KG_PROCESS_ERROR(pMapInfo->m_bManualReset); // 该地图必须能够手动重置
    
    if (nMapCopyIndex == 0)
    {
        nMapCopyIndex = pMapInfo->GetOwnerCopy(dwPlayerID);
        KG_PROCESS_SUCCESS(nMapCopyIndex == 0);
    }
    
    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(dwMapID, nMapCopyIndex);
    KG_PROCESS_SUCCESS(pMapCopy == NULL);

    KGLOG_PROCESS_ERROR(dwPlayerID == pMapCopy->m_dwOwnerID); // 只有副本的所有者才能重置

    bRetCode = pMapCopy->CanReset();
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_GameServer.DoDeleteMapNotify(pMapCopy->m_nConnIndex, dwMapID, nMapCopyIndex);
    pMapInfo->DeleteMapCopy(nMapCopyIndex);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KMapManager::IncreaseMapCopyPlayerCount(DWORD dwMapID, int nCopyIndex)
{
    BOOL                        bResult  = false;
    KMapCopy*                   pMapCopy = NULL;

    pMapCopy = GetMapCopy(dwMapID, nCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);
    
    pMapCopy->m_nPlayerCount++;
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KMapManager::DecreaseMapCopyPlayerCount(DWORD dwMapID, int nCopyIndex)
{
    BOOL                        bResult  = false;
    KMapCopy*                   pMapCopy = NULL;

    pMapCopy = GetMapCopy(dwMapID, nCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);
    
    pMapCopy->m_nPlayerCount--;
    
    assert(pMapCopy->m_nPlayerCount >= 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMapManager::SetMapCopyOwner(DWORD dwMapID, int nCopyIndex, DWORD dwOwnerID)
{
    BOOL                        bResult  = false;
    KMapInfo*                   pMapInfo = NULL;
    KMapCopy*                   pMapCopy = NULL;
    
    pMapInfo = GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    pMapCopy = pMapInfo->GetMapCopy(nCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    if (dwOwnerID == pMapCopy->m_dwOwnerID || pMapInfo->m_nType != emtDungeon)
    {
        goto Exit1;
    }

    pMapCopy->m_dwOwnerID = dwOwnerID;

    g_pSO3GameCenter->m_GameServer.DoSetMapCopyOwner(
        pMapCopy->m_nConnIndex, dwMapID, nCopyIndex, dwOwnerID
    );
    
Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KMapManager::IsMapCopyCountLimit(KMapInfo* pMapInfo)
{
    BOOL    bResult        = false;
    int     nLoadCopyCount = 0;

    KGLOG_PROCESS_ERROR(pMapInfo);

    for (
        KMAP_COPY_TABLE::iterator it = pMapInfo->m_CopyTable.begin();
        it != pMapInfo->m_CopyTable.end();
        ++it
    )
    {
        KMAP_COPY_STATE eState = it->second.m_eState;
        if (eState == eMapStateCreating || eState == eMapStateNormal)
        {
            nLoadCopyCount++;
        }
    }

    KG_PROCESS_ERROR(nLoadCopyCount >= pMapInfo->m_nMaxCopyCount);

    bResult = true;
Exit0:
    return bResult;
}

int KMapManager::FindPlayerCopyIndex(KRole* pRole, KRole* pLeader, DWORD dwMapID)
{
    int         nResult             = 0;
    int         nMapCopyIndex       = 0;
    int         nLeaderCopyIndex    = 0;
    KMapInfo*   pMapInfo            = NULL;

    assert(pRole);

    pMapInfo = GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    // 规则1: 找A是否在某个副本的"玩家列表"里面,如果找到,则进入.
    nMapCopyIndex = pMapInfo->GetCopyIndexByPlayerList(pRole->m_dwPlayerID);
    KG_PROCESS_SUCCESS(nMapCopyIndex != 0);
    
    // 规则2: 找A的队长是否在某个副本的"玩家列表"里面,并且队长当前也在副本里面,如果找到,则进入.
    if (pRole->m_dwTeamID != ERROR_ID)
    {
        assert(pLeader);

        if (pLeader != pRole)
        {
            nLeaderCopyIndex = pMapInfo->GetCopyIndexByPlayerList(pLeader->m_dwPlayerID);
            if (nLeaderCopyIndex != 0)
            {
                if (pLeader->m_dwMapID == dwMapID && pLeader->m_nMapCopyIndex == nLeaderCopyIndex)
                {
                    nMapCopyIndex = nLeaderCopyIndex;
                    goto Exit1;
                }
            }
        }
    }
    
    // 规则3: 找是否有某个副本的Owner是队长,并且没有存档,如果找到,则进入.
    if (pRole->m_dwTeamID != ERROR_ID)
    {
        assert(pLeader);

        for (KMAP_COPY_TABLE::iterator it = pMapInfo->m_CopyTable.begin(); it != pMapInfo->m_CopyTable.end(); ++it)
        {
            if (it->second.m_dwOwnerID == pLeader->m_dwPlayerID)
            {
                if (it->second.m_piSceneData == NULL)
                {
                    nMapCopyIndex = it->first;
                    goto Exit1;
                }
            }
        }
    }

    // 规则4: 找是否有某个副本的Owner是自己,如果有,则进入.
    nMapCopyIndex = pMapInfo->GetOwnerCopy(pRole->m_dwPlayerID);
    
Exit1:
    nResult = nMapCopyIndex;
Exit0:
    return nResult;
}

void KMapManager::OnPlayerEnterMapCopy(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex)
{
    BOOL        bRetCode  = false;
    DWORD       dwOwnerID = ERROR_ID;
    KMapInfo*   pMapInfo  = NULL;
    KRole*      pRole     = NULL;
    
    pMapInfo = GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    KG_PROCESS_ERROR(pMapInfo->m_nType == emtDungeon);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    dwOwnerID = pMapInfo->GetNewOwnerForPlayerEnter(pRole, nMapCopyIndex);

    SetMapCopyOwner(dwMapID, nMapCopyIndex, dwOwnerID);

    for (KMAP_COPY_TABLE::iterator it = pMapInfo->m_CopyTable.begin(); it != pMapInfo->m_CopyTable.end(); ++it)
    {
        KMapCopy* pMapCopy      = &it->second;
        int       nTmpCopyIndex = it->first;

        if (pMapCopy->m_dwOwnerID != dwPlayerID)
            continue;

        if (nTmpCopyIndex == nMapCopyIndex)
            continue;

        if (pMapCopy->m_eState != eMapStateNormal)
            continue;

        if (pMapCopy->m_piSceneData != NULL)
            continue;

        g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayerPrepare(
            pMapCopy->m_nConnIndex, dwMapID, nTmpCopyIndex, bpcMapRefresh, DELETE_SCENE_WAIT_SECONDS
        );

        pMapCopy->m_eState       = eMapStateRefreshing;
        pMapCopy->m_nRefreshTime = g_pSO3GameCenter->m_nTimeNow + DELETE_SCENE_WAIT_SECONDS;
    }

Exit0:
    return;
}

void KMapManager::GetMapList(std::vector<DWORD>& rRetMap)
{
    for (KMAP_TABLE::iterator MapIter = m_MapTable.begin(); MapIter != m_MapTable.end(); ++MapIter)
    {
        rRetMap.push_back(MapIter->first);
    }
}

void KMapManager::ResetPlayerEnterNewCopyTime()
{
    for (KMAP_TABLE::iterator MapIter = m_MapTable.begin(); MapIter != m_MapTable.end(); ++MapIter)
    {
        MapIter->second.m_PlayerEnterNewCopyTimes.clear();
    }
}

BOOL KMapManager::LoadMapList()
{
    BOOL        bResult                 = false;
	int         nRetCode                = false;
	ITabFile*   piTabFile               = NULL;
    int         nTableHeight            = 0;
    DWORD       dwMapID                 = 0;
	time_t      nCurTime                = time(NULL);
    char        szFileName[MAX_PATH];
    KMapInfo    MapInfo;
    
	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, MAP_LIST_FILE_NAME);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[MapData] Failed to open file [\"%s\"]\n", szFileName);
        goto Exit0;
    }

    nTableHeight = piTabFile->GetHeight();

	for (int nLine = 2; nLine <= nTableHeight; nLine++)
	{
		nRetCode = piTabFile->GetInteger(nLine, "ID", 0, (int*)&dwMapID);
		KGLOG_PROCESS_ERROR(nRetCode == 1);
        KGLOG_PROCESS_ERROR(dwMapID <= MAX_MAP_ID);

		nRetCode = piTabFile->GetString(nLine, "Name", "", MapInfo.m_szName, sizeof(MapInfo.m_szName));
		KGLOG_PROCESS_ERROR(nRetCode == 1);
        
        nRetCode = piTabFile->GetInteger(nLine, "Type", 0, &MapInfo.m_nType);
		KGLOG_PROCESS_ERROR(nRetCode == 1);
        KGLOG_PROCESS_ERROR(MapInfo.m_nType > emtInvalid && MapInfo.m_nType < emtTotal);

		nRetCode = piTabFile->GetInteger(nLine, "MaxCopyCount", 0, &MapInfo.m_nMaxCopyCount);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

        if (MapInfo.m_nType == emtNormalMap)
        {
            KGLOG_PROCESS_ERROR(MapInfo.m_nMaxCopyCount == 1 && "NormalScene can be only one!");  // 正常场景只能预加载一个！
        }

		nRetCode = piTabFile->GetInteger(nLine, "MinPlayerCount", 0, &MapInfo.m_nMinPlayerCount);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = piTabFile->GetInteger(nLine, "MaxPlayerCount", 0, &MapInfo.m_nMaxPlayerCount);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = piTabFile->GetInteger(nLine, "KeepTime", 0, &MapInfo.m_nKeepTime);
		KGLOG_PROCESS_ERROR(nRetCode == 1);
        
        nRetCode = piTabFile->GetInteger(nLine, "ManualReset", 0, &MapInfo.m_bManualReset);
		KGLOG_PROCESS_ERROR(nRetCode == 1);
        
        nRetCode = piTabFile->GetInteger(nLine, "RefreshCycle", 0, &MapInfo.m_nRefreshCycle);
		KGLOG_PROCESS_ERROR(nRetCode == 1);
        
        nRetCode = piTabFile->GetInteger(nLine, "UseLastEntry", 0, &MapInfo.m_bUseLastEntry);
        KGLOG_PROCESS_ERROR(nRetCode == 1);
        
        nRetCode = piTabFile->GetInteger(nLine, "LimitTimes", 0, &MapInfo.m_nLimitTimes);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "LoginIntervalTime", 0, &MapInfo.m_nLoginIntervalTime);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PreQueuePlayerCount", 0, &MapInfo.m_nPreQueuePlayerCount);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        MapInfo.m_dwMapID           = dwMapID;
        MapInfo.m_nSNGenerator      = 1;
        MapInfo.m_nNextRefreshTime  = 0;
        MapInfo.m_nRefreshCycle    *= 60;

        snprintf(MapInfo.m_szSequenceName, sizeof(MapInfo.m_szSequenceName), "%s%lu", "Map", MapInfo.m_dwMapID);
        MapInfo.m_szSequenceName[sizeof(MapInfo.m_szSequenceName) - 1] = '\0';

        if (MapInfo.m_nRefreshCycle > 0)
        {
            MapInfo.m_nNextRefreshTime = (nCurTime / MapInfo.m_nRefreshCycle + 1) * MapInfo.m_nRefreshCycle;
        }

        m_MapTable[dwMapID] = MapInfo;  
	}

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KMapManager::LoadMapGroup()
{
    BOOL        bResult         = false;
	int         nRetCode        = false;
	ITabFile*   piTabFile       = NULL;
    int         nTabWidth       = 0;
    int         nTabHeight      = 0;
	char        szFileName[MAX_PATH];
	KMapInfo    MapInfo;

	snprintf(szFileName, sizeof(szFileName), "%s/MapGroup.tab", SETTING_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[MapData] Failed to open file [\"%s\"]\n", szFileName);
        goto Exit0;
    }

    nTabWidth    = piTabFile->GetWidth();
    nTabHeight   = piTabFile->GetHeight();

    KGLOG_PROCESS_ERROR(nTabWidth <= MAX_GS_COUNT);

    for (int nRow = 1; nRow <= nTabWidth; nRow++)
    {
        char szGroupName[_NAME_LEN];

        snprintf(szGroupName, sizeof(szGroupName), "Group%d", nRow);
        szGroupName[sizeof(szGroupName) - 1] = '\0';

	    for (int nLine = 2; nLine <= nTabHeight; nLine++)
	    {
            DWORD           dwMapID     = 0;
            KMapInfo*       pMapInfo    = NULL;
            KMAP_GROUP_ITEM GroupItem;

		    nRetCode = piTabFile->GetInteger(nLine, szGroupName, 0, (int*)&dwMapID);
		    if (nRetCode != 1)
                break;

            KMAP_TABLE::iterator it = m_MapTable.find(dwMapID);
            if (it == m_MapTable.end())
            {
                KGLogPrintf(KGLOG_WARNING, "[MapData] Invalid Map ID %u in group %d!\n", dwMapID, nRow);
                continue;
            }

            pMapInfo = &it->second;

            KGLOG_PROCESS_ERROR(pMapInfo->m_nType != emtBattleField);
            KGLOG_PROCESS_ERROR(pMapInfo->m_nType != emtDungeon);

            if (pMapInfo->m_nSNGenerator > pMapInfo->m_nMaxCopyCount)
            {
                KGLogPrintf(KGLOG_WARNING, "[MapData] Too many copies preload for map %u !\n", dwMapID);
                continue;
            }

            GroupItem.dwMapID = dwMapID;
            GroupItem.nCopySN = pMapInfo->m_nSNGenerator++;

            m_GroupTable[nRow - 1].MapTable.push_back(GroupItem);
	    }
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
	return bResult;
}
