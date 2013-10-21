#include "stdafx.h"
#include "KMapInfo.h"
#include "KSO3GameCenter.h"

KMapInfo::KMapInfo()
{
    m_szName[0]         = '\0';
    m_szSequenceName[0] = '\0';
    m_nSNGenerator      = 0;
    m_nType             = emtInvalid;
    m_nMinPlayerCount   = 0;
    m_nMaxPlayerCount   = 0;
    m_nMaxCopyCount     = 0;
    m_nKeepTime         = 0;
    m_bManualReset      = false;
    m_nRefreshCycle     = 0;
    m_nNextRefreshTime  = 0;
    m_bUseLastEntry     = false;
    m_nLimitTimes       = 0;
    m_nPlayerCount      = 0;
}

void KMapInfo::Activate()
{
    if (m_nRefreshCycle > 0 && g_pSO3GameCenter->m_nTimeNow > m_nNextRefreshTime)
    {
        Refresh();
        m_nNextRefreshTime = (g_pSO3GameCenter->m_nTimeNow / m_nRefreshCycle + 1) * m_nRefreshCycle;
    }

    m_nPlayerCount = 0;

    for (KMAP_COPY_TABLE::iterator it = m_CopyTable.begin(); it != m_CopyTable.end(); NULL)
    {
        KMapCopy* pMapCopy = &it->second;

        assert(pMapCopy->m_nPlayerCount >= 0);

        m_nPlayerCount += pMapCopy->m_nPlayerCount + (int)pMapCopy->m_CGCallbackList.size();

        pMapCopy->Activate();

        if (pMapCopy->m_bIsChanged)
        {
            g_pSO3GameCenter->m_MiscDB.DoSaveMapCopyInfo(m_dwMapID, pMapCopy->m_nCopyIndex);
            pMapCopy->m_nLastSaveTime = g_pSO3GameCenter->m_nTimeNow;
            pMapCopy->m_bIsChanged    = false;
        }

        if (pMapCopy->m_bAutoDelete && pMapCopy->m_eState == eMapStateNormal)
        {
            if (pMapCopy->m_nUnloadTime == 0)
            {
                if (pMapCopy->m_nPlayerCount < m_nMinPlayerCount)
                {
                    g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayerPrepare(
                        pMapCopy->m_nConnIndex, m_dwMapID, it->first, bpcMapUnload, m_nKeepTime * 60
                    );

                    pMapCopy->m_nUnloadTime = g_pSO3GameCenter->m_nTimeNow + m_nKeepTime * 60;
                }
            }
            else
            {
                if (g_pSO3GameCenter->m_nTimeNow > pMapCopy->m_nUnloadTime)
                {
                    g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayer(pMapCopy->m_nConnIndex, m_dwMapID, it->first);

                    pMapCopy->m_eState = eMapStateUnloading;
                }
                else if (pMapCopy->m_nPlayerCount >= m_nMinPlayerCount)
                {
                    g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayerPrepare(
                        pMapCopy->m_nConnIndex, m_dwMapID, it->first, bpcCancelBanish, 0
                    );
                    pMapCopy->m_nUnloadTime = 0;
                }
                else
                {
                    if (g_pSO3GameCenter->m_nWorkLoop % (3 * GAME_FPS) == 0)
                    {
                        time_t nKeepTime = pMapCopy->m_nUnloadTime - g_pSO3GameCenter->m_nTimeNow;

                        g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayerPrepare(
                            pMapCopy->m_nConnIndex, m_dwMapID, it->first, bpcMapUnload, (int)nKeepTime
                        );
                    }
                }
            }
        }

        if (pMapCopy->m_eState == eMapStateRefreshing)
        {
            if (g_pSO3GameCenter->m_nTimeNow > pMapCopy->m_nRefreshTime)
            {
                g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayer(pMapCopy->m_nConnIndex, m_dwMapID, it->first);
                pMapCopy->m_nRefreshTime += BANISH_PLAYER_WAIT_SECONDS;
            }

            if (pMapCopy->m_nPlayerCount == 0)
            {
                g_pSO3GameCenter->m_GameServer.DoDeleteMapNotify(pMapCopy->m_nConnIndex, m_dwMapID, it->first);

                g_pSO3GameCenter->m_MiscDB.DoDeleteMapCopyInfo(m_dwMapID, it->first);

                m_CopyTable.erase(it++);
                continue;
            }
        }

        if (pMapCopy->m_eState == eMapStateUnloading && pMapCopy->m_nPlayerCount == 0)
        {
            g_pSO3GameCenter->m_GameServer.DoDeleteMapNotify(pMapCopy->m_nConnIndex, m_dwMapID, it->first);

            if (pMapCopy->m_piSceneData)
            {
                pMapCopy->m_nConnIndex  = 0;
                pMapCopy->m_eState      = eMapStateOffline;
                pMapCopy->m_nUnloadTime = 0;
            }
            else
            {
                m_CopyTable.erase(it++);
                continue;
            }
        }

        ++it;
    }
}

void KMapInfo::Refresh()
{
    for (KMAP_COPY_TABLE::iterator it = m_CopyTable.begin(); it != m_CopyTable.end(); ++it)
    {
        KMapCopy* pMapCopy = &it->second;

        if (pMapCopy->m_eState == eMapStateNormal || pMapCopy->m_eState == eMapStateOffline)
        {
            // 通知驱赶该副本中的玩家
            g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayerPrepare(
                pMapCopy->m_nConnIndex, m_dwMapID, pMapCopy->m_nCopyIndex, bpcMapRefresh, DELETE_SCENE_WAIT_SECONDS
            );

            pMapCopy->m_eState       = eMapStateRefreshing;
            pMapCopy->m_nRefreshTime = g_pSO3GameCenter->m_nTimeNow + DELETE_SCENE_WAIT_SECONDS;
        }
    }
}

KMapCopy* KMapInfo::CreateMapCopy(
    int     nMapCopyIndex,
    time_t  nCreateTime,
    time_t  nLastSaveTime,
    int     nConnIndex, 
    BOOL    bAutoDelete
)
{
    KMapCopy    CopyInfo;
    std::pair<KMAP_COPY_TABLE::iterator, BOOL> InsRet;

    CopyInfo.m_nCopyIndex             = nMapCopyIndex;
    CopyInfo.m_eState                 = eMapStateInvalid;
    CopyInfo.m_bAutoDelete            = bAutoDelete;
    CopyInfo.m_nCreateTime            = nCreateTime;
    CopyInfo.m_nUnloadTime            = 0;
    CopyInfo.m_nRefreshTime           = 0;
    CopyInfo.m_nLastSaveTime          = nLastSaveTime;
    CopyInfo.m_dwOwnerID              = ERROR_ID;
    CopyInfo.m_nConnIndex             = nConnIndex;
    CopyInfo.m_nPlayerCount           = 0;
    CopyInfo.m_nMinPlayerCount        = m_nMinPlayerCount;
    CopyInfo.m_piSceneData            = NULL;
    CopyInfo.m_bIsChanged             = false;
    CopyInfo.m_dwQueueID              = INVALID_QUEUE_ID;
    CopyInfo.m_nLastTryLoginTime      = 0;

    InsRet = m_CopyTable.insert(std::make_pair(nMapCopyIndex, CopyInfo));

    if (InsRet.second)
    {
        return &(InsRet.first->second);
    }

    return NULL;
}

BOOL KMapInfo::DeleteMapCopy(int nMapCopyIndex)
{
    KMAP_COPY_TABLE::iterator it = m_CopyTable.find(nMapCopyIndex);
    if (it != m_CopyTable.end())
    {
        m_CopyTable.erase(it);
        return true;
    }
    return false;
}

void KMapInfo::DeleteAllCopyOnConnection(int nConnection)
{
    for (KMAP_COPY_TABLE::iterator it = m_CopyTable.begin(); it != m_CopyTable.end(); NULL)
    {
        KMapCopy* pMapCopy = &it->second;

        if (pMapCopy->m_nConnIndex == nConnection)
        {
            KGLogPrintf(
                KGLOG_INFO, "Delete map copy %s(%u, %d)\n", 
                m_szName, m_dwMapID, it->first
            );
            
            if (pMapCopy->m_piSceneData != NULL)
            {
                pMapCopy->m_eState      = eMapStateOffline;
                pMapCopy->m_nConnIndex  = 0;
                pMapCopy->m_nUnloadTime = 0;
            }
            else
            {
                m_CopyTable.erase(it++);
                continue;
            }
        }
        ++it;
    }
}

int KMapInfo::GetOwnerCopy(DWORD dwPlayerID)
{
    int nResult = 0;

    for (KMAP_COPY_TABLE::iterator it = m_CopyTable.begin(); it != m_CopyTable.end(); ++it)
    {
        if (it->second.m_dwOwnerID == dwPlayerID)
        {
            nResult = it->first;
            break;
        }
    }
    return nResult;
}

int KMapInfo::GetCopyIndexByPlayerList(DWORD dwPlayerID)
{
    for (KMAP_COPY_TABLE::iterator it = m_CopyTable.begin(); it != m_CopyTable.end(); ++it)
    {
        KMAP_COPY_PLAYER_LIST::iterator PlayerIter = it->second.m_PlayerList.find(dwPlayerID);
        if (PlayerIter != it->second.m_PlayerList.end())
        {
            return it->first;
        }
    }
    return 0;
}

DWORD KMapInfo::GetNewOwnerForPlayerEnter(KRole* pRole, int nCopyIndex)
{
    DWORD           dwOwnerID   = ERROR_ID;
    KTeam*          pTeam       = NULL;
    KRole*          pLeader     = NULL;  
    KMapCopy*       pMapCopy    = NULL;
    KRole*          pOldOwner   = NULL;

    assert(pRole);

    KG_PROCESS_ERROR(m_nType == emtDungeon);

    pMapCopy = GetMapCopy(nCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    if (pMapCopy->m_dwOwnerID != ERROR_ID)
    {
        pOldOwner = g_pSO3GameCenter->m_RoleManager.GetRole(pMapCopy->m_dwOwnerID);
    }

    if (pRole->m_dwTeamID == ERROR_ID)
    {
        if (pOldOwner == NULL)
        {
            dwOwnerID = pRole->m_dwPlayerID;
        }
        else
        {
            dwOwnerID = pMapCopy->m_dwOwnerID;
        }

        goto Exit0;
    }

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pRole->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);        

    if (pOldOwner == NULL || pOldOwner->m_dwTeamID == pRole->m_dwTeamID)
    {
        dwOwnerID = pTeam->dwAuthority[tatLeader];
    }
    else
    {
        dwOwnerID = pMapCopy->m_dwOwnerID;
    }

Exit0:
    return dwOwnerID;
}

BOOL KMapInfo::CheckPlayerEnterNewCopyTimes(DWORD dwPlayerID, int nNewCopyIndex)
{
    BOOL                                bResult                     = false;
    KNEW_MAP_LIMIT_TABLE::iterator      it;

    if ((m_nType != emtDungeon && m_nType != emtBattleField) || m_nLimitTimes == 0)
    {
        goto Exit1;
    }

    it = m_PlayerEnterNewCopyTimes.find(dwPlayerID);
    if (it == m_PlayerEnterNewCopyTimes.end())
    {
        goto Exit1;
    }
    
    KG_PROCESS_SUCCESS(it->second.nCopyIndex == nNewCopyIndex); // 如果进的是同一个副本则允许进入
    KG_PROCESS_ERROR(it->second.nTimes < m_nLimitTimes); // 超过场景的进入次数限制则不允许进入

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KMapInfo::AddPlayerEnterNewCopyTimes(DWORD dwPlayerID, int nNewCopyIndex)
{
    BOOL                                bResult                     = false;
    KNEW_MAP_LIMIT_TABLE::iterator      it;

    if ((m_nType != emtDungeon && m_nType != emtBattleField) || m_nLimitTimes == 0)
    {
        goto Exit0;
    }

    it = m_PlayerEnterNewCopyTimes.find(dwPlayerID);
    if (it == m_PlayerEnterNewCopyTimes.end())
    {
        KPLAYER_ENTER_NEW_COPY_TIMES EnterTimes;
        EnterTimes.nCopyIndex = nNewCopyIndex;
        EnterTimes.nTimes     = 1;

        m_PlayerEnterNewCopyTimes[dwPlayerID] = EnterTimes;
    }
    else
    {
        if (it->second.nCopyIndex != nNewCopyIndex)
        {
            ++it->second.nTimes;
            it->second.nCopyIndex = nNewCopyIndex;
        }
    }

Exit0:
    return;
}

BOOL KMapInfo::GetPlayerSavedCopys(DWORD dwPlayerID, std::vector<int>& vecCopys)
{
    BOOL bResult = false;
    KMAP_COPY_PLAYER_LIST::iterator PlayerIt;

    KMAP_COPY_TABLE::iterator CopyIter = m_CopyTable.begin();
    for (;CopyIter != m_CopyTable.end(); ++CopyIter)
    {
        PlayerIt = CopyIter->second.m_PlayerList.find(dwPlayerID);
        if (PlayerIt != CopyIter->second.m_PlayerList.end())
        {
            vecCopys.push_back(CopyIter->first);
        }
    }

    bResult = true;
//Exit0:
    return bResult;
}

int KMapInfo::LuaGetMapCopyList(Lua_State* L)
{
    int nCount  = 1;

    Lua_NewTable(L);

    for (KMAP_COPY_TABLE::iterator it = m_CopyTable.begin(); it != m_CopyTable.end(); ++it)
    {
        Lua_PushNumber(L, nCount);
        Lua_PushNumber(L, it->second.m_nCopyIndex);

        Lua_SetTable(L, -3);
        nCount++;
    }

    return 1;
}

int KMapInfo::LuaGetMapCopy(Lua_State* L)
{
    int         nTopIndex       = 0;
    int         nMapCopyIndex   = 0;
    KMapCopy*   pMapCopy        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nMapCopyIndex = (int)Lua_ValueToNumber(L ,1);

    pMapCopy = GetMapCopy(nMapCopyIndex);
    KG_PROCESS_ERROR(pMapCopy);

Exit0:
    if (pMapCopy)
    {
        pMapCopy->LuaGetObj(L);
    }
    else
    {
        Lua_PushNil(L);
    }
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KMapInfo)

    REGISTER_LUA_DWORD_READONLY(KMapInfo, MapID)
    REGISTER_LUA_STRING_READONLY(KMapInfo, Name)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, SNGenerator)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, Type)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, MinPlayerCount)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, MaxPlayerCount)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, MaxCopyCount)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, KeepTime)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, RefreshCycle)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, LimitTimes)
    REGISTER_LUA_TIME_READONLY(KMapInfo, NextRefreshTime)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, PlayerCount)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, LoginIntervalTime)
    REGISTER_LUA_INTEGER_READONLY(KMapInfo, PreQueuePlayerCount)
    REGISTER_LUA_FUNC(KMapInfo, GetMapCopyList)
    REGISTER_LUA_FUNC(KMapInfo, GetMapCopy)

DEFINE_LUA_CLASS_END(KMapInfo)

