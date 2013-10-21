#ifndef _KQUEUEMANAGER_H_ 
#define _KQUEUEMANAGER_H_ 

#include <map>
#include <list>

struct KQueueNode 
{
    int     nPlayerIndex;
    DWORD   dwRoleID;
};

typedef std::list<KQueueNode> KPLAYER_QUEUE;

struct KPlayerQueue 
{
    KPlayerQueue();
    BOOL Enqueue(int nPlayerIndex, DWORD dwRoleID);
    BOOL Dequeue(int nPlayerIndex);

    BOOL            m_bUpdate;
    time_t          m_nNextRetryTime;
    time_t          m_nNextUpdateTime;
    KPLAYER_QUEUE   m_PlayerQueue;
};

class KPlayerManager;
class KRelayAgency;

class KQueueManager
{
public:
    KQueueManager();

    void Activate();

    void AttachModule(KPlayerManager* pModule) { m_pPlayerManager = pModule; }
    void AttachModule(KRelayAgency* pModule) { m_pRelayAgency = pModule; }

    KPlayerQueue* GetQueue(DWORD dwID)
    {
        KQUEUE_TABLE::iterator it = m_QueueTable.find(dwID);
        if (it != m_QueueTable.end())
        {
            return &it->second;
        }
        return NULL;
    }

    int GetWaitingQueuePlayerCount();

    KPlayerQueue* CreateQueue(DWORD dwID);

private:
    BOOL RetryLogin(KPlayerQueue* pPlayerQueue);
    BOOL CheckUpdate(KPlayerQueue* pPlayerQueue);

private:
    typedef std::map<DWORD, KPlayerQueue> KQUEUE_TABLE;
    KQUEUE_TABLE    m_QueueTable;

    KPlayerManager* m_pPlayerManager;
    KRelayAgency*   m_pRelayAgency;
};

#endif
