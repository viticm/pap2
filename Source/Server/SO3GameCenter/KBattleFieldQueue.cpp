////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KBattleFieldQueue.cpp
//  Version     : 1.0
//  Creator     : Wang Tao
//  Create Date : 2009-02-27 08:50:31
//  Comment     : 战场排队相关代码实现
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KGPublic.h"
#include "KSO3GameCenter.h"
#include "KPartyCenter.h"
#include "KBattleFieldQueue.h"

#define BATTLE_FIELD_QUEUE_TAB "BattleFieldQueue.tab"

KBattleFieldQueueManager::KBattleFieldQueueManager()
{
}

BOOL KBattleFieldQueueManager::Init()
{
    BOOL bResult                = false;
    int  nRetCode               = 0;
    char szFileName[MAX_PATH];
    
    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, BATTLE_FIELD_QUEUE_TAB);
    KGLOG_PROCESS_ERROR(nRetCode >= 0 && nRetCode <= sizeof(szFileName));

    nRetCode = LoadBattleFieldQueueTab(szFileName);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KBattleFieldQueueManager::UnInit()
{
    m_QueueTable.clear();
}

void KBattleFieldQueueManager::Activate()
{
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 3) == 0)
    {
        for (KQUEUE_TABLE::iterator it = m_QueueTable.begin(); it != m_QueueTable.end(); ++it)
        {
            ProcessBattleField(&it->second);
        }
    }
}

BOOL KBattleFieldQueueManager::JoinQueue(DWORD dwRoleID, DWORD dwMapID, int64_t nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KBattleField*           pBattleField    = NULL;
    KQUEUE_TABLE::iterator  itFind;
    KQueueNode              QueueNode;

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pBattleField = &itFind->second;
    assert(pBattleField);

    QueueNode.nDstCopyIndex = nCopyIndex;
    QueueNode.nJoinTime     = g_pSO3GameCenter->m_nTimeNow;

    if (bTeamJoin)
    {
        const KPARTY*   cpParty = NULL;
        const KRole*    cpRole   = NULL;

        cpRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
        KGLOG_PROCESS_ERROR(cpRole);

        cpParty = g_pSO3GameCenter->m_PartyCenter.GetPartyInfo(cpRole->m_dwPartyID);
        KGLOG_PROCESS_ERROR(cpParty);

        for (KPARTY_PLAYER_LIST::const_iterator it = cpParty->PlayerList.begin(); it != cpParty->PlayerList.end(); ++it)
        {
            LeaveQueue(*it, dwMapID);

            bRetCode = AddPlayerToNode(*it, pBattleField, dwGroupID, &QueueNode);
            KG_PROCESS_ERROR(bRetCode);
        }
    }
    else
    {
        LeaveQueue(dwRoleID, dwMapID);

        bRetCode = AddPlayerToNode(dwRoleID, pBattleField, dwGroupID, &QueueNode);
        KG_PROCESS_ERROR(bRetCode);
    }

    pBattleField->QueueList[dwGroupID].push_back(QueueNode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldQueueManager::LeaveQueue(DWORD dwRoleID, DWORD dwMapID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KBattleField*           pBattleField    = NULL;
    KQUEUE_TABLE::iterator  itFind;

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pBattleField = &itFind->second;
    assert(pBattleField);

    for (int i = 0; i < QUEUE_PER_BATTLE; i++)
    {
        for (KPLAYER_QUEUE::iterator it = pBattleField->QueueList[i].begin(); it != pBattleField->QueueList[i].end(); ++it)
        {
            for (KPLAYER_LIST::iterator ListIter = it->PlayerList.begin(); ListIter != it->PlayerList.end(); ++ListIter)
            {
                if (*ListIter == dwRoleID)
                {
                    *ListIter = ERROR_ID;
                    goto Exit1;
                }
            }
        }
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldQueueManager::GetCanJoinRole(
    int* pnRetCount, DWORD* pdwRetRoleList, int nMaxCount,
    DWORD dwMapID, int64_t nCopyIndex, DWORD dwGroup
)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    int                     nCount          = 0;
    KBattleField*           pBattleField    = NULL;
    KQUEUE_TABLE::iterator  itFind;

    assert(pnRetCount);
    assert(pdwRetRoleList);

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pBattleField = &itFind->second;
    assert(pBattleField);

    for (KPLAYER_QUEUE::iterator it = pBattleField->QueueList[dwGroup].begin(); it != pBattleField->QueueList[dwGroup].end(); ++it)
    {
        if (it->nDstCopyIndex != 0 && it->nDstCopyIndex != nCopyIndex)
            continue;

        if (nCount + (int)it->PlayerList.size() > nMaxCount)
            continue;

        for (KPLAYER_LIST::iterator ListIter = it->PlayerList.begin(); ListIter != it->PlayerList.end(); ++ListIter)
        {
            if (*ListIter == ERROR_ID)
                continue;

            *(pdwRetRoleList + nCount) = *ListIter;
            nCount++;
        }

        if (nCount == nMaxCount)
            break;
    }

    *pnRetCount = nCount;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldQueueManager::LoadBattleFieldQueueTab(const char* szFileName)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    ITabFile*   piTabFile   = NULL;
    int         nHieght     = 0;
    
    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHieght = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHieght > 1);

    for (int nIndex = 2; nIndex <= nHieght; nIndex++)
    {
        DWORD           dwMapID = 0;
        KBattleField    BattleField;

        bRetCode = piTabFile->GetInteger(nIndex, "MapID", 0, (int*)&dwMapID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get MapID");

        BattleField.dwMapID = dwMapID;

        bRetCode = piTabFile->GetInteger(nIndex, "MinLevelRequire", 0, &BattleField.nMinJoinLevel);
        KGLOG_PROCESS_ERROR(bRetCode && "Get MinLevelRequire");

        bRetCode = piTabFile->GetInteger(nIndex, "MaxLevelRequire", 0, &BattleField.nMaxJoinLevel);
        KGLOG_PROCESS_ERROR(bRetCode && "Get MaxLevelRequire");

        for (int i = 0; i < QUEUE_PER_BATTLE; i++)
        {
            int  nRetCode               = 0;
            char szColName[_NAME_LEN];

            nRetCode = snprintf(szColName, sizeof(szColName), "ForceMask%d", i + 1);
            KGLOG_PROCESS_ERROR(nRetCode >= 0 && nRetCode < sizeof(szColName));

            bRetCode = piTabFile->GetInteger(nIndex, szColName, 0, (int*)&BattleField.dwForcMask[i]);
            KGLOG_PROCESS_ERROR(bRetCode && "Get ForceMask");

            BattleField.QueueList[i].clear();
            BattleField.nAvgQueueTime[i] = 0;
        }

        m_QueueTable.insert(std::make_pair(dwMapID, BattleField));
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KBattleFieldQueueManager::ProcessBattleField(KBattleField* pBattleField)
{
    for (int i = 0; i < QUEUE_PER_BATTLE; i++)
    {
        ProcessPlayerQueue(pBattleField, i);
    }
}

void KBattleFieldQueueManager::ProcessPlayerQueue(KBattleField* pBattleField, int nQueueIndex)
{
    KPLAYER_QUEUE& PlayerQueue = pBattleField->QueueList[nQueueIndex];

    for (KPLAYER_QUEUE::iterator it = PlayerQueue.begin(); it != PlayerQueue.end(); NULL)
    {
        for (KPLAYER_LIST::iterator ListIter = it->PlayerList.begin(); ListIter != it->PlayerList.end(); NULL)
        {
            KRole* pRole = g_pSO3GameCenter->m_RoleManager.GetRole(*ListIter);
            if (pRole == NULL || pRole->GetRoleState() == rsOffline)
            {
                it->PlayerList.erase(ListIter++);
                continue;
            }

            g_pSO3GameCenter->m_GameServer.DoBattleFieldQueueNotify(
                pRole->m_nConnIndex, *ListIter, pBattleField->nAvgQueueTime[nQueueIndex],
                it->nJoinTime, pBattleField->dwMapID, it->nDstCopyIndex
            );

            ++ListIter;
        }

        if (it->PlayerList.empty())
        {
            PlayerQueue.erase(it++);
            continue;
        }

        ++it;
    }
}

BOOL KBattleFieldQueueManager::IsPlayerInBattleField(KBattleField* pBattleField, DWORD dwRoleID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KQUEUE_TABLE::iterator  itFind;

    assert(pBattleField);

    for (int i = 0; i < QUEUE_PER_BATTLE; i++)
    {
        for (KPLAYER_QUEUE::iterator it = pBattleField->QueueList[i].begin(); it != pBattleField->QueueList[i].end(); ++it)
        {
            for (KPLAYER_LIST::iterator ListIter = it->PlayerList.begin(); ListIter != it->PlayerList.end(); ++ListIter)
            {
                if (*ListIter == dwRoleID)
                {
                    bResult = true;
                    goto Exit0;
                }
            }
        }
    }

Exit0:
    return bResult;
}

int KBattleFieldQueueManager::CountRoleJoinedQueue(DWORD dwRoleID)
{
    BOOL bRetCode = false;
    int  nCount   = 0;

    for (KQUEUE_TABLE::iterator it = m_QueueTable.begin(); it != m_QueueTable.end(); ++it)
    {
        bRetCode = IsPlayerInBattleField(&it->second, dwRoleID);
        if (bRetCode)
        {
            nCount++;
        }
    }

    return nCount;
}

BOOL KBattleFieldQueueManager::AddPlayerToNode(DWORD dwRoleID, KBattleField* pBattleField, DWORD dwGroupID, KQueueNode* pQueueNode)
{
    BOOL    bResult         = false;
    KRole*  pRole           = NULL;
    DWORD   dwForceMask     = 0;
    int     nJoinedQueue    = 0;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(
        pRole->m_byLevel >= pBattleField->nMinJoinLevel && pRole->m_byLevel <= pBattleField->nMaxJoinLevel
    );

    assert(pRole->m_byForceID < sizeof(DWORD) * CHAR_BIT);
    dwForceMask = (1 << pRole->m_byForceID);
    KG_PROCESS_ERROR(pBattleField->dwForcMask[dwGroupID] & dwForceMask);

    nJoinedQueue = CountRoleJoinedQueue(dwRoleID);
    KG_PROCESS_ERROR(nJoinedQueue < MAX_QUEUE_PER_PLAYER);

    pQueueNode->PlayerList.push_back(dwRoleID);

    bResult = true;
Exit0:
    return bResult;
}
