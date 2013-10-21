////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KBattleFieldQueue.h
//  Version     : 1.0
//  Creator     : Wang Tao
//  Create Date : 2009-02-27 08:50:31
//  Comment     : 战场排队相关代码实现
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KBATTLE_FIELD_QUEUE_H_
#define _KBATTLE_FIELD_QUEUE_H_ 1

#include <list>
#include <map>
#include "SO3GlobalDef.h"

#define QUEUE_PER_BATTLE        4
#define MAX_QUEUE_PER_PLAYER    3


class KBattleFieldQueueManager
{
public:
    KBattleFieldQueueManager();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL JoinQueue(DWORD dwRoleID, DWORD dwMapID, int64_t nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin);
    BOOL LeaveQueue(DWORD dwRoleID, DWORD dwMapID);

    BOOL GetCanJoinRole(int* pnRetCount, DWORD* pdwRetRoleList, int nMaxCount, DWORD dwMapID, int64_t nCopyIndex, DWORD dwGroup);

private:
    typedef KMemory::KAllocator<DWORD> KPLAYER_LIST_ALLOC;
    typedef std::list<DWORD, KPLAYER_LIST_ALLOC> KPLAYER_LIST;

    struct KQueueNode
    {
        int64_t         nDstCopyIndex;
        time_t          nJoinTime;        
        KPLAYER_LIST    PlayerList;
    };

    typedef KMemory::KAllocator<KQueueNode> KPLAYER_QUEUE_ALLOC;
    typedef std::list<KQueueNode, KPLAYER_QUEUE_ALLOC> KPLAYER_QUEUE;

    struct KBattleField
    {
        DWORD           dwMapID;
        int             nMinJoinLevel;
        int             nMaxJoinLevel;
        KPLAYER_QUEUE   QueueList[QUEUE_PER_BATTLE];
        DWORD           dwForcMask[QUEUE_PER_BATTLE];
        time_t          nAvgQueueTime[QUEUE_PER_BATTLE];
    };

    typedef KMemory::KAllocator<KBattleField> KQUEUE_ALLOC;
    typedef std::map<DWORD, KBattleField, less<DWORD>, KQUEUE_ALLOC> KQUEUE_TABLE; 

    KQUEUE_TABLE m_QueueTable;

private:
    BOOL LoadBattleFieldQueueTab(const char* szFileName);

    void ProcessBattleField(KBattleField* pBattleField);
    void ProcessPlayerQueue(KBattleField* pBattleField, int nQueueIndex);

    BOOL IsPlayerInBattleField(KBattleField* pBattleField, DWORD dwRoleID);
    int  CountRoleJoinedQueue(DWORD dwRoleID);

    BOOL AddPlayerToNode(DWORD dwRoleID, KBattleField* pBattleField, DWORD dwGroupID, KQueueNode* pQueueNode);
};

#endif
