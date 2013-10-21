#ifndef _KTONG_OPERATION_HISTORY_H_
#define _KTONG_OPERATION_HISTORY_H_

#include "KTongDef.h"
#include <deque>

static size_t s_uHistoryListMaxSize[ohtTotal] = 
{
    MAX_MEMBER_MANAGE_RECORD_CONUT,
    MAX_FUND_RECORD_COUNT,
    MAX_TECH_TREE_RECORD_COUNT,
    MAX_MISSIOM_RECORD_COUNT,
    MAX_REPERTORY_RECORD_COUNT
};

#pragma pack(1)

struct KTONG_HISTORY
{
    BYTE    byRecordType;
    time_t  nTime;
};

// ------------ 人事变动 ----------------

struct KTONG_HISTORY_CHANGE_GROUP_BASE_RIGHT : KTONG_HISTORY
{
    DWORD   dwOperator;
    BYTE    byTargetGroup;
    BYTE    byRightOffset;
    BYTE    byIsOpen;
};

struct KTONG_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT : KTONG_HISTORY
{
    DWORD   dwOperator;
    BYTE    bySrcGroup;
    BYTE    byDstGroup;
    BYTE    byRightOffset;
    BYTE    bIsOpen;
};

struct KTONG_HISTORY_CHANGE_MEMBER_GROUP : KTONG_HISTORY
{
    DWORD   dwOperator;
    DWORD   dwTargetMember;
    BYTE    bySrcGroup;
    BYTE    byDstGroup;
};

struct KTONG_HISTORY_JOIN_OR_QUIT : KTONG_HISTORY
{
    DWORD dwMemberID;
};

struct KTONG_HISTORY_KICK_OUT_MEMBER : KTONG_HISTORY
{
    DWORD dwOperator;
    DWORD dwTargetMember;
};

// ------------ 资金变动 ----------------
struct KTONG_HISTORY_CHANGE_GROUP_WAGE : KTONG_HISTORY
{
    DWORD   dwOperator;
    BYTE    byTargetGroup;
    DWORD   dwOldWage;
    DWORD   dwNewWage;
};

struct KTONG_HISTORY_PAY_SALARY : KTONG_HISTORY
{
    DWORD dwOperator;
    DWORD dwTotalSalary;
};

struct KTONG_HISTORY_SAVE_FUND : KTONG_HISTORY
{
    DWORD dwOperator;
    DWORD dwSaveSilver;
};

// ------------ 科技树和道具出产 ------------

struct KTONG_HISTORY_ACTIVATE_TECHTREE : KTONG_HISTORY
{
    DWORD   dwOperator;
    BYTE    byNode;
    BYTE    byLevel;
    DWORD   dwConsumeDevelopmentPoint;
    DWORD   dwConsumeFund;
};

struct KTONG_HISTORY_PRODUCT_ITEM : KTONG_HISTORY
{
    DWORD dwOperator;
    DWORD dwRecipeID;
    DWORD dwCount;
    DWORD dwConsumeDevelopmentPoint;
    DWORD dwConsumeFund;
};

// ------------ 帮会活动 ----------------

struct KTONG_HISTORY_MISSION_START : KTONG_HISTORY
{
    DWORD   dwOperator;
    DWORD   dwMission;
    time_t  nMissionStartTime;
};

struct KTONG_HISTORY_MISSION_END : KTONG_HISTORY
{
    DWORD   dwMission;
};

struct KTONG_HISTORY_WAR_START : KTONG_HISTORY
{
    DWORD   dwOperator;
    DWORD   dwEnemyTong;
};

struct KTONG_HISTORY_WAR_END : KTONG_HISTORY
{
    DWORD   dwEnemyTong;
};

struct KTONG_HISTORY_ACHIEVEMENT : KTONG_HISTORY
{
    DWORD   dwAchievement;
};

// ------------ 帮会仓库 ----------------

struct KTONG_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM : KTONG_HISTORY
{
    DWORD   dwOperator;
    BYTE    byTabType;
    WORD    wTabIndex;
    WORD    wStackNum;
};

// 用以计算历史记录的存盘最大耗费
union KTONG_MAX_SIZE_HISTORY_DATA
{
    KTONG_HISTORY_CHANGE_GROUP_BASE_RIGHT       BaseRight;
    KTONG_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT    AdvanceRight;
    KTONG_HISTORY_CHANGE_MEMBER_GROUP           ChangeGroup;
    KTONG_HISTORY_JOIN_OR_QUIT                  JoinOrQuit;
    KTONG_HISTORY_KICK_OUT_MEMBER               KickOut;
    KTONG_HISTORY_CHANGE_GROUP_WAGE             ChangeWage;
    KTONG_HISTORY_PAY_SALARY                    PaySalary;
    KTONG_HISTORY_SAVE_FUND                     SaveFund;
    KTONG_HISTORY_ACTIVATE_TECHTREE             TechTree;
    KTONG_HISTORY_PRODUCT_ITEM                  Product;
    KTONG_HISTORY_MISSION_START                 MissionStart;
    KTONG_HISTORY_MISSION_END                   MissionEnd;
    KTONG_HISTORY_WAR_START                     WarStart;
    KTONG_HISTORY_WAR_END                       WarEnd;
    KTONG_HISTORY_ACHIEVEMENT                   Achievement;
    KTONG_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM    Repertpry;
};

#pragma pack()

class KTongHistory
{
public:
    typedef KMemory::KAllocator<KTONG_HISTORY*>KTONG_HISTOR_POINTER_ALLOCTOR;
    typedef deque<KTONG_HISTORY*, KTONG_HISTOR_POINTER_ALLOCTOR> KTONG_HISTORY_LIST;
    KTONG_HISTORY_LIST  m_RecordList[ohtTotal];
    DWORD               m_dwIndex[ohtTotal];
public:

    BOOL Init();
    void UnInit();
    static void Config();

    BOOL Load(BYTE* pbyData, size_t uDataLen);
    BOOL Sava(BYTE* pbyBuffer, size_t uBufferSize, size_t* puRetUsedSize);

    void AddRecord(KTONG_OPERATION_HISTORY_TYPE eType, KTONG_HISTORY* pRecord);
// ------------ 人事变动 ----------------
    BOOL RecordChangeGroupBaseRight(DWORD dwOperator, int nTargetGroup, int nRightOffset, BOOL bIsOpen);
    BOOL RecordChangeGroupAdvanceRight(DWORD dwOperator, int nSrcGroup, int nDstGroup, int nRightOffset, BOOL bIsOpen);
    BOOL RecordChangeMemberGroup(DWORD dwOperator, DWORD dwTargetMember, int nSrcGroup, int nDstGroup);
    BOOL RecordJoin(DWORD dwMember);
    BOOL RecordQuit(DWORD dwMember);
    BOOL RecordKickout(DWORD dwOperator, DWORD dwTargetMember);
// ------------ 资金变动 ----------------
    BOOL RecordChangeGroupWage(DWORD dwOperator, int nTargetGroup, int nOldWage, int nNewWage);
    BOOL RecordPaySalary(DWORD dwOperator, int nTotalSalary);
    BOOL RecordSaveFund(DWORD dwOperator, int nSilver);
// -------- 科技树和道具出产 ------------  
    BOOL RecordActivateTechTree(DWORD dwOperator, int nNode, int nLevel, int nDevelopmentPoint, int nFund);
    BOOL RecordProductItem(DWORD dwOperator, DWORD dwRecipeID, int nCount, int nDevelopmentPoint, int nFund);
// ------------ 帮会活动 ----------------
    BOOL RecordMissionStart(DWORD dwOperator, DWORD dwMission ,time_t nMissionStartTime);
    BOOL RecordMissionComplete(DWORD dwMission);
    BOOL RecordMissionFailed(DWORD dwMission);
    BOOL RecordWarStart(DWORD dwOperator, DWORD dwEnemyTong);
    BOOL RecordWarWin(DWORD dwEnemyTong);
    BOOL RecordWarLose(DWORD dwEnemyTong);
    BOOL RecordAchievement(DWORD dwAchievement);
// ------------ 帮会仓库 ----------------
    BOOL RecordTakeRepertoryItem(DWORD dwOperator, DWORD dwItemType, DWORD dwItemIndex, int nStackNum);
    BOOL RecordPutRepertoryItem(DWORD dwOperator, DWORD dwItemType, DWORD dwItemIndex, int nStackNum);

    BOOL SyncRecord(DWORD dwPlayerID, KTONG_OPERATION_HISTORY_TYPE eType, DWORD dwRangeButton, DWORD dwRangeTop);
};

#endif // _KTONG_OPERATION_HISTORY_H_
