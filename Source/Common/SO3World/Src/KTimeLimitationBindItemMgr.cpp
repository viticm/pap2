#include "stdafx.h"
#include "KTimeLimitationBindItemMgr.h"
#include "KSO3World.h"
#include "KScene.h"

#define LIMITATION_BIND_ITEM_TIME 30 * 60

BOOL KTimeLimitationBindItemMgr::Init(KScene* pScene)
{
    BOOL bResult = false;
    
    assert(pScene);

    m_pScene = pScene;

    bResult = true;
Exit0:
    return bResult;
}

void KTimeLimitationBindItemMgr::Activate()
{
    MAP_LIMITATION_BIND_ITEM::iterator it;
    if (g_pSO3World->m_nGameLoop % (GAME_FPS * 5) != 0)
    {
        goto Exit0;
    }

    it = m_mapLimitationBindItem.begin();
    while (it != m_mapLimitationBindItem.end())
    {
        if (g_pSO3World->m_nCurrentTime >= it->second.nEndTime)
        {
            m_mapLimitationBindItem.erase(it++);
            continue;
        }
        
        if (it->second.vecPlayerList.empty())
        {
            m_mapLimitationBindItem.erase(it++);
            continue;
        }

#ifdef _SERVER

        if (it->second.bDataChanged)
        {
            DoSyncTimeLimitationBindItemInfo(it->first, &it->second);
            it->second.bDataChanged = false;
        }
#endif

        ++it;
    }

Exit0:
    return;
};

BOOL KTimeLimitationBindItemMgr::AddItem(DWORD dwItemID, DWORD dwOwnerID, int nLeftTime)
{
    BOOL bResult = false;
    pair<MAP_LIMITATION_BIND_ITEM::iterator, bool> itRes;

    itRes = m_mapLimitationBindItem.insert(make_pair<DWORD, KTIME_LIMITATION_BIND_ITEM>(dwItemID, KTIME_LIMITATION_BIND_ITEM()));
    KG_PROCESS_ERROR(itRes.second);

    if (nLeftTime == 0)
    {
        nLeftTime = LIMITATION_BIND_ITEM_TIME;
    }
    
    itRes.first->second.dwOwnerID = dwOwnerID;
    itRes.first->second.nEndTime = g_pSO3World->m_nCurrentTime + nLeftTime;
    itRes.first->second.bDataChanged = true;

    bResult = true;
Exit0:
    return bResult;
}

void KTimeLimitationBindItemMgr::DelItem(DWORD dwItemID)
{
    m_mapLimitationBindItem.erase(dwItemID);
}

BOOL KTimeLimitationBindItemMgr::IsPlayerCanHaveTheItem(DWORD dwItemID, DWORD dwPlayerID)
{
    BOOL bResult = false;
    MAP_LIMITATION_BIND_ITEM::iterator it;
    std::vector<DWORD>::iterator vecIt;

    it = m_mapLimitationBindItem.find(dwItemID);
    KG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());

    vecIt = std::find(it->second.vecPlayerList.begin(), it->second.vecPlayerList.end(), dwPlayerID);
    KG_PROCESS_ERROR(vecIt != it->second.vecPlayerList.end());

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTimeLimitationBindItemMgr::AddPlayer(DWORD dwItemID, DWORD dwPlayerID)
{
    BOOL bResult = false;
    DWORD dwLeftTime = 0;
    MAP_LIMITATION_BIND_ITEM::iterator it;

    it = m_mapLimitationBindItem.find(dwItemID);
    KGLOG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());

    it->second.vecPlayerList.push_back(dwPlayerID);
    it->second.bDataChanged = true;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTimeLimitationBindItemMgr::RemovePlayer(DWORD dwItemID, DWORD dwPlayerID)
{
    BOOL bResult = false;
    MAP_LIMITATION_BIND_ITEM::iterator it;
    std::vector<DWORD>::iterator vecIt;

    it = m_mapLimitationBindItem.find(dwItemID);
    KGLOG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());
    
    vecIt = std::find(it->second.vecPlayerList.begin(), it->second.vecPlayerList.end(), dwPlayerID);
    KG_PROCESS_ERROR(vecIt != it->second.vecPlayerList.end());

    it->second.vecPlayerList.erase(vecIt);
    it->second.bDataChanged = true;
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTimeLimitationBindItemMgr::GetAllPlayer(DWORD dwItemID, std::vector<DWORD> &vecPlayers)
{
    BOOL bResult = false;
    MAP_LIMITATION_BIND_ITEM::iterator it;

    it = m_mapLimitationBindItem.find(dwItemID);
    KGLOG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());

    vecPlayers = it->second.vecPlayerList;

    bResult = true;
Exit0:
    return bResult;
}

void KTimeLimitationBindItemMgr::OnPlayerLeaveScene(DWORD dwPlayerID)
{
    MAP_LIMITATION_BIND_ITEM::iterator it = m_mapLimitationBindItem.begin();
    std::vector<DWORD>::iterator vecIt;

    while (it != m_mapLimitationBindItem.end())
    {
        vecIt = std::find(it->second.vecPlayerList.begin(), it->second.vecPlayerList.end(), dwPlayerID);
        
        if (vecIt != it->second.vecPlayerList.end())
        {
            it->second.vecPlayerList.erase(vecIt);    
            it->second.bDataChanged = true;
        }

        ++it;
    }
}

int KTimeLimitationBindItemMgr::GetLeftTime(DWORD dwItemID)
{
    int nLeftTime = 0;
    MAP_LIMITATION_BIND_ITEM::iterator it;

    it = m_mapLimitationBindItem.find(dwItemID);
    KG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());
    
    if (it->second.nEndTime > g_pSO3World->m_nCurrentTime)
    {
        nLeftTime = (int)(it->second.nEndTime - g_pSO3World->m_nCurrentTime);
    }

Exit0:
    return nLeftTime;
}

DWORD KTimeLimitationBindItemMgr::GetOwner(DWORD dwItemID)
{
    DWORD dwItemOwner = ERROR_ID;
    MAP_LIMITATION_BIND_ITEM::iterator it;

    it = m_mapLimitationBindItem.find(dwItemID);
    KGLOG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());

    dwItemOwner = it->second.dwOwnerID;
Exit0:
    return dwItemOwner;
}

BOOL KTimeLimitationBindItemMgr::SetOwner(DWORD dwItemID, DWORD dwOwnerID)
{
    BOOL bResult = false;
    MAP_LIMITATION_BIND_ITEM::iterator it;

    it = m_mapLimitationBindItem.find(dwItemID);
    KGLOG_PROCESS_ERROR(it != m_mapLimitationBindItem.end());

    it->second.dwOwnerID = dwOwnerID;
    it->second.bDataChanged = true;

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
void KTimeLimitationBindItemMgr::DoSyncTimeLimitationBindItemInfo(DWORD dwItemID, KTIME_LIMITATION_BIND_ITEM* pItemInfo)
{
    KPlayer*    pOwner          = NULL;
    const char* pcszFunction    = "TimeLimitationBindItemChanged";
    int         nLeftTime       = 0;
    BYTE*       pbyTableBegin   = NULL;
    BYTE*       pbyRetCode      = NULL;

    assert(pItemInfo);

    pOwner = g_pSO3World->m_PlayerSet.GetObj(pItemInfo->dwOwnerID);
    KG_PROCESS_ERROR(pOwner);

    if (pItemInfo->nEndTime > g_pSO3World->m_nCurrentTime)
    {
        nLeftTime = (int)(pItemInfo->nEndTime - g_pSO3World->m_nCurrentTime);
    }

    g_pSO3World->m_ScriptServer.Reset();
    g_pSO3World->m_ScriptServer.PushNumber(dwItemID);
    g_pSO3World->m_ScriptServer.PushNumber(nLeftTime);

    pbyTableBegin = g_pSO3World->m_ScriptServer.NewTable();
    KGLOG_PROCESS_ERROR(pbyTableBegin);

    for (size_t i = 0; i < pItemInfo->vecPlayerList.size(); ++i)
    {
        pbyRetCode = g_pSO3World->m_ScriptServer.PushNumber(i);
        KGLOG_PROCESS_ERROR(pbyRetCode);

        pbyRetCode = g_pSO3World->m_ScriptServer.PushNumber(pItemInfo->vecPlayerList[i]);
        KGLOG_PROCESS_ERROR(pbyRetCode);

        pbyRetCode = g_pSO3World->m_ScriptServer.SetTable(pbyTableBegin, pbyRetCode);
        KGLOG_PROCESS_ERROR(pbyRetCode);
    }
    
    g_pSO3World->m_ScriptServer.Call(pOwner->m_nConnIndex, pcszFunction);

Exit0:
    return;
}
#endif
