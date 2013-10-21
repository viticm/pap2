#include "stdafx.h"
#include "KSO3Gateway.h"

KPlayerQueue::KPlayerQueue()
{
    m_bUpdate           = false;
    m_nNextRetryTime    = 0;
    m_nNextUpdateTime   = 0;
}

BOOL KPlayerQueue::Enqueue(int nPlayerIndex, DWORD dwRoleID)
{
    KQueueNode QueuePlayer;

    QueuePlayer.nPlayerIndex    = nPlayerIndex;
    QueuePlayer.dwRoleID        = dwRoleID;

    m_PlayerQueue.push_back(QueuePlayer);

    m_bUpdate = true;

    return true;
}

BOOL KPlayerQueue::Dequeue(int nPlayerIndex)
{
    for (KPLAYER_QUEUE::iterator it = m_PlayerQueue.begin(); it != m_PlayerQueue.end(); ++it)
    {
        if (it->nPlayerIndex == nPlayerIndex)
        {
            m_PlayerQueue.erase(it);
            m_bUpdate = true;
            return true;
        }
    }

    return false;
}

KQueueManager::KQueueManager()
{
    m_pPlayerManager    = NULL;
    m_pRelayAgency      = NULL;
}

void KQueueManager::Activate()
{
    time_t          nTimeNow    = time(NULL);
        
    for (KQUEUE_TABLE::iterator it = m_QueueTable.begin(); it != m_QueueTable.end(); NULL)
    {
        KPlayerQueue* pPlayerQueue = &it->second;

        if (pPlayerQueue->m_PlayerQueue.empty())
        {
            m_QueueTable.erase(it++);
            continue;
        }

        if (nTimeNow >= pPlayerQueue->m_nNextRetryTime)
        {
            RetryLogin(pPlayerQueue);
            pPlayerQueue->m_nNextRetryTime = nTimeNow + 3;
        }

        if (nTimeNow >= pPlayerQueue->m_nNextUpdateTime && pPlayerQueue->m_bUpdate)
        {
            CheckUpdate(pPlayerQueue);
            pPlayerQueue->m_bUpdate         = false;
            pPlayerQueue->m_nNextUpdateTime = nTimeNow + 2;
        }

        ++it;
    }
}

int KQueueManager::GetWaitingQueuePlayerCount()
{
    int nPlayerCount = 0;

    for (KQUEUE_TABLE::iterator it = m_QueueTable.begin(); it != m_QueueTable.end(); ++it)
    {
        nPlayerCount += (int)it->second.m_PlayerQueue.size();
    }
    
    return nPlayerCount;
}

KPlayerQueue* KQueueManager::CreateQueue(DWORD dwID)
{
    KPlayerQueue*                           pResult = NULL;
    std::pair<KQUEUE_TABLE::iterator, BOOL> InsRet;

    KGLOG_PROCESS_ERROR(dwID != INVALID_QUEUE_ID);

    InsRet = m_QueueTable.insert(std::make_pair(dwID, KPlayerQueue()));
    if (InsRet.second)
    {
        pResult = &(InsRet.first->second);
    }

Exit0:
    return pResult;
}

BOOL KQueueManager::RetryLogin(KPlayerQueue* pPlayerQueue)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KPlayerAgency*              pPlayer         = NULL;
    BYTE*                       pbyRoleData     = NULL;
    size_t                      uRoleDataLen    = 0;
    KQueueNode                  QueuePlayer;
    KROLE_LIST::const_iterator  it;

    assert(!pPlayerQueue->m_PlayerQueue.empty());

    QueuePlayer = pPlayerQueue->m_PlayerQueue.front();

    pPlayer = m_pPlayerManager->GetPlayer(QueuePlayer.nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_SUCCESS(pPlayer->nState != easQueueWaiting);

    // retry login
    it = pPlayer->RoleList.find(QueuePlayer.dwRoleID);
    KGLOG_PROCESS_ERROR(it != pPlayer->RoleList.end());

    pbyRoleData = (BYTE*)(it->second->GetData());
    KGLOG_PROCESS_ERROR(pbyRoleData);

    uRoleDataLen = it->second->GetSize();

    m_pRelayAgency->DoGameLoginRequest(pPlayer, QueuePlayer.dwRoleID, pbyRoleData, uRoleDataLen);

    pPlayer->nState = easWaitForLoginPermission;

Exit1:
    bResult = true;
Exit0:
    return bResult;
} 

BOOL KQueueManager::CheckUpdate(KPlayerQueue* pPlayerQueue)
{
    BOOL                    bResult     = false;
    KPlayerAgency*          pPlayer     = NULL;
    int                     nPosition   = 0;
    const KPLAYER_QUEUE&    PlayerQueue = pPlayerQueue->m_PlayerQueue;

    for (KPLAYER_QUEUE::const_iterator it = PlayerQueue.begin(); it != PlayerQueue.end(); ++it)
    {
        pPlayer = m_pPlayerManager->GetPlayer(it->nPlayerIndex);
        KGLOG_PROCESS_ERROR(pPlayer);

        m_pPlayerManager->DoSyncQueueState(pPlayer, nPosition);
        nPosition++;
    }

    bResult = true;
Exit0:
    return bResult;
}
