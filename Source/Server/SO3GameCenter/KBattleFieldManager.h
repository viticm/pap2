#ifndef _KBATTLEFIELD_MANAGER_H_ 
#define _KBATTLEFIELD_MANAGER_H_ 

#include <list>
#include <map>
#include "SO3GlobalDef.h"
#include "SO3Result.h"

#define MAX_QUEUE_PER_PLAYER        3
#define TIME_OUT_OFF_LINE           (900) // 15分钟
#define TIME_OUT_WAIT_BATTLE_FIELD  (120) // 2分钟
#define MAX_BLACKLIST_TIME          (60 * 15)
#define TIME_OUT_QUEUE_OFF_LINE     (60 * 60) // 1小时

#define GET_ENTER_POS_FUNCTION      "GetEnterPos"
#define GET_LEAVE_POS_FUNCTION      "GetLeavePos"
#define CHECK_JOIN_TIME_FUNCTION    "CheckJoinTime"

class KBattleFieldManager  
{
public:
    KBattleFieldManager();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL IsBattleField(DWORD dwMapID);

    // 排队管理
    BATTLE_FIELD_RESULT_CODE JoinQueue(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin, DWORD dwJoinValue);
    BOOL LeaveQueue(DWORD dwRoleID, DWORD dwMapID);

    // 战场管理
    BOOL CheckPermission(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex);

    BOOL ConfirmEnter(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex);

    BOOL GetBattleFieldList(DWORD dwRoleID, DWORD dwMapID);

    int  GetBattleFieldSide(DWORD dwMapID, int nCopyIndex, DWORD dwRoleID);

    DWORD GetBattleFieldJoinSign(DWORD dwMapID, int nCopyIndex, DWORD dwRoleID);

    BOOL GetEnterBattlePos(int* pnRetX, int* pnRetY, int* pnRetZ, DWORD dwMapID, int nGroupID, DWORD dwJoinValue);
    BOOL GetLeaveBattlePos(DWORD* pdwRetMapID, int* pnRetX, int* pnRetY, int* pnRetZ, DWORD dwMapID, int nGroupID, DWORD dwJoinValue);

    BOOL AddInBlackList(DWORD dwRoleID, int nTime);
    void LeaveOutBlackList(DWORD dwRoleID);

private:
    struct KBASE_INFO 
    {
        DWORD dwMapID;
        int   nMinJoinLevel;
        int   nMaxJoinLevel;
        int   nMaxPlayerPerSide;
        int   nMinPlayerPerSide;
        int   nMaxBattleFieldCopyCount;
        BOOL  bEnableGroup[MAX_BATTLE_SIDE];
        DWORD dwForcMask[MAX_BATTLE_SIDE];
        KCAMP eCampRequire[MAX_BATTLE_SIDE];
        DWORD dwPQTemplateID;
        char  szScript[MAX_PATH];
    };
    typedef KMemory::KAllocator<std::pair<DWORD, KBASE_INFO> > KBASE_INFO_TABLE_ALLOC;
    typedef std::map<DWORD, KBASE_INFO, less<DWORD>, KBASE_INFO_TABLE_ALLOC> KBASE_INFO_TABLE;

    KBASE_INFO_TABLE m_BaseInfoTable;

    struct KROLE_INFO
    {
        DWORD   dwRoleID;
        DWORD   dwJoinValue;
        time_t  nLastActiveTime;
        time_t  nJoinTime;
    };

    typedef KMemory::KAllocator<KROLE_INFO> KROLE_LIST_ALLOC;
    typedef std::list<KROLE_INFO, KROLE_LIST_ALLOC> KROLE_LIST;

    struct KQUEUE_NODE
    {
        int         nDstCopyIndex;
        KROLE_LIST  RoleList;
    };

    typedef KMemory::KAllocator<KQUEUE_NODE> KROLE_QUEUE_ALLOC;
    typedef std::list<KQUEUE_NODE, KROLE_QUEUE_ALLOC> KROLE_QUEUE;

    struct KQUEUE
    {
        DWORD       dwMapID;
        int         nMinJoinLevel;
        int         nMaxJoinLevel;
        DWORD       dwForcMask[MAX_BATTLE_SIDE];
        KCAMP       eCampRequire[MAX_BATTLE_SIDE];
        time_t      nAvgQueueTime[MAX_BATTLE_SIDE];
        KROLE_QUEUE QueueList[MAX_BATTLE_SIDE];
    };

    typedef KMemory::KAllocator<std::pair<DWORD, KQUEUE> > KQUEUE_TABLE_ALLOC;
    typedef std::map<DWORD, KQUEUE, less<DWORD>, KQUEUE_TABLE_ALLOC> KQUEUE_TABLE;

    KQUEUE_TABLE m_QueueTable;

    struct KSOLDER
    {
        DWORD   dwRoleID;
        time_t  nJoinTime;
        time_t  nLastOnlineTime;
        int     dwGroupID;
        DWORD   dwJoinValue;
        BOOL    bConfirmEnter;
    };

    typedef KMemory::KAllocator<KSOLDER> KSOLDER_LIST_ALLOC;
    typedef std::list<KSOLDER, KSOLDER_LIST_ALLOC> KSOLDER_LIST;

    struct KBATTLE_FIELD
    {
        DWORD           dwMapID;
        int             nCopyIndex;
        DWORD           dwPQID;
        int             nMinPlayerPerSide;
        int             nMaxPlayerPerSide;
        DWORD           dwBattleTeam[MAX_BATTLE_SIDE];
        KSOLDER_LIST    PlayingList;
        KSOLDER_LIST    WaitingList;
    };

    typedef KMemory::KAllocator<std::pair<int, KBATTLE_FIELD> > KBATTLE_FIELD_COPY_TABLE_ALLOC;
    typedef std::map<int, KBATTLE_FIELD, less<int>, KBATTLE_FIELD_COPY_TABLE_ALLOC> KBATTLE_FIELD_COPY_TABLE;


    typedef KMemory::KAllocator<std::pair<DWORD, KBATTLE_FIELD_COPY_TABLE> > KBATTLE_FIELD_TABLE_ALLOC;
    typedef std::map<DWORD, KBATTLE_FIELD_COPY_TABLE, less<DWORD>, KBATTLE_FIELD_TABLE_ALLOC> KBATTLE_FIELD_TABLE;

    KBATTLE_FIELD_TABLE m_BattleFieldTable;

    struct KBLACK_LIST_NODE
    {
        DWORD   dwRoleID;
        time_t  nDelTime;
    };
    typedef KMemory::KAllocator<KBLACK_LIST_NODE> KBLACK_LIST_ALLOC;
    typedef std::list<KBLACK_LIST_NODE, KBLACK_LIST_ALLOC> KBLACK_LIST;

    KBLACK_LIST m_BlackList;

private:
    DWORD       m_dwCreatingMapID;
    int         m_nCreatingMapCopyIndex;

private:
    KBASE_INFO* GetBattleFieldBaseInfo(DWORD dwMapID);
    KBATTLE_FIELD* GetBattleField(DWORD dwMapID, int nCopyIndex);

    BOOL LoadBattleFieldTable(const char cszFileName[]);
    BOOL CreateQueue();

    BOOL SearchRoleInQueue(
        KROLE_LIST* pSearchResult, int nCopyIndex, 
        KROLE_QUEUE::iterator itBegin, KROLE_QUEUE::iterator itEnd,
        int nMinRequire, int nMaxRequire
    );
    BOOL GetCanJoinRole(KROLE_LIST* pRoleList, int nMinCount, int nMaxCount, DWORD dwMapID, int nCopyIndex, DWORD dwGroup);

    void CalcAvgQueueTime(DWORD dwMapID, DWORD dwGroup, const KROLE_LIST& rRoleList);

    BOOL CheckJoinTime(DWORD dwMapID);

    BATTLE_FIELD_RESULT_CODE CanJoinQueue(DWORD dwRoleID, KQUEUE* pQueue, DWORD dwGroupID);
    BOOL IsRoleInQueue(KQUEUE* pQueue, DWORD dwRoleID);
    int  CountRoleJoinedQueue(DWORD dwRoleID);

    void ProcessQueue(KQUEUE* pQueue);

    DWORD CreateBattleFieldPQ(DWORD dwMapID, int nCopyIndex);
    void DestroyBattleFieldPQ(DWORD dwMapID, int nCopyIndex);

    // 1. 各方玩家人数的判断与补人
    void ProcessBattleFieldCopyTable(DWORD dwMapID, KBATTLE_FIELD_COPY_TABLE* pBattleFieldCopyTable);
    void ProcessBattleField(KBATTLE_FIELD* pBattleField);

    // 2. 根据情况新开战场
    void ProcessNewBattleField(DWORD dwMapID);

    // BlackList
    void ProcessBlackList();

    BOOL IsInBlackList(DWORD dwRoleID);

    BOOL JoinBattleTeam(DWORD dwRoleID, DWORD dwTeamID);
    BOOL LeaveBattleTeam(DWORD dwRoleID, DWORD dwTeamID);
};

#endif
