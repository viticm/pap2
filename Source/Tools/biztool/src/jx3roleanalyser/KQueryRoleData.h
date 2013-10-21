/***************************************************
Filename   :   KQueryRoleData.h
CreatedBy  :   Monkey(Hou Rui)
CreatedOn  :   2008年12月26日 
Description:   从数据库中查询角色全部信息
**************************************************/

#ifndef KQUERYROLEDATA_H
#define KQUERYROLEDATA_H

#define KGLOG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)

#define KGLOG_ERR 3
#include <string>
using namespace std;
#include "Engine/EngineBase.h"
#include "Engine/FileType.h"
#include "KGPublic.h"
#include "KGD_Interface.h"
#include "KRoleDBDataDef.h"
#include "Luna.h"
#include "KSkillDef.h"
#include "KSkillList.h"
#include "KSkillRecipeList.h"
#include "KItem.h"
#include "KRecipeList.h"
#include "KQuestInfoList.h"
#include "KQuestList.h"
#include "KProfessionList.h"
#include "Engine/Random.h"
#include "KBuffList.h"
#include "KReputation.h"
#include "KCDTimerList.h"
#include "KGPQ.h"
#include "KDesignation.h"
#include "KFellowship.h"
//#include "Engine/kglog.h"
#include "KCustomData.h"
#include "KProbability.h"
//#include "KAntiFarmer.h"

struct ItemData 
{
    unsigned long   m_dwMoney;
    unsigned short  m_wEnabledBankPackageCount;
    unsigned short  m_wItemCount;

    struct KITEM_DB_HEADEREX 
    {
        unsigned char byBox;
        unsigned char byPos;
        unsigned char byDataLen;
        union
        {
            KCOMMON_ITEM_DATA commonEquipData;
            KCUSTOM_EQUI_DATA customEquipData;
//            KTRAIN_EQUIP_DATA trainEquipData;
        };
    } ItemArray[0];
};

struct QuestData 
{
    unsigned char              m_dwQuestState[MAX_QUEST_COUNT];
    unsigned char              m_AcceptQuestCount;
    KDB_ACCEPT_QUEST           m_AcceptQuest[MAX_ACCEPT_QUEST_COUNT];
    unsigned char              m_AssistQuestCount;
    unsigned char              m_AssistDailyCount;
    KDB_ASSIST_QUEST           m_AssistQuest[MAX_ASSIST_QUEST_COUNT];
    unsigned char              m_DailyQuestCount;
    KDB_DAILY_QUEST            m_DailyQuest[MAX_ACCEPT_QUEST_COUNT];
};

struct UserPreferences 
{
    int  m_nSize;
    unsigned char m_byUserPreferences[MAX_USER_PREFERENCES_LEN];
};

struct BookList 
{
    int  m_nSize;
    unsigned char m_byBookList[MAX_READ_BOOK_ID];
};

typedef std::map<int, int> KCOUNTER_TABLE;

struct AchievementData
{
    int                                         m_nPoint;       // 成就积分
    int                                         m_nRecord;      // 游戏完成度

    KCustomData<MAX_ACHIEVEMENT_ID / CHAR_BIT>  m_BitSet;       // 成就bitset
    KCOUNTER_TABLE                              m_CounterTable; // 计数器表
};

struct DesignationData
{
    int                                         m_nCurrentPrefix;
    int                                         m_nCurrentPostfix;
    int                                         m_nGenerationIndex;
    int                                         m_nNameInForceIndex;
    bool                                        m_bDisplayFlag;
    KCustomData<DESIGNATIONFIX_DATA_SIZE>       m_AcquiredPrefix;
    KCustomData<DESIGNATIONFIX_DATA_SIZE>       m_AcquiredPostfix;
};

#pragma	pack(1)

struct KMENTOR_DATA_ON_PLAYER_NOW_USE
{
    int     nAcquiredMentorValue;
    time_t  nLastEvokeMentorTime;
    BYTE    byEvokeMentorCount;
    BYTE    byMaxApprenticeCount;

    BYTE    byReserved[22];
};

#pragma pack()

struct RoleDataAll
{
    //Todo:构造函数
    KRoleBaseInfo                      m_BaseData;
    KDB_SKILL_DATA*                    m_pSkillData;
    KG_SKILL_RECIPE_LIST_DB_DATA_V2*   m_pSkillRecipeList;
    ItemData*                          m_pItemData;
    QuestData                          m_QuestData;
    KProfessionDBData*                 m_pProfessionData;
    unsigned char                      m_byRecipeState[MAX_CRAFT_COUNT][MAX_RECIPE_ID];
    //KRecipeDBData*                   m_pRecipeData;
    KROLE_STATE_INFO                   m_StateInfo;
    KBUFF_DB_DATA*                     m_pBuffDBData;
    KFORCE_DB_DATA*                    m_pForceDBData;
    UserPreferences                    m_UserPreferences;
    BookList                           m_BookList;
    COOLDOWN_LIST_DB_DATA*             m_pCooldownListDBData;
    KROLE_ROAD_INFO*                   m_pRoleRoadInfo;
    KGPQ_DB_LIST*                      m_pPQList;
    int                                m_bHeroFlag;
    KCustomData<MAX_MAP_ID_DATA_SIZE>  m_HeroMapCopyOpenFlag;
    AchievementData                    m_AchievementData;
    DesignationData                    m_DesignationData;

    KProbability<uint64_t, MILLION_NUM, 128>    m_QuestRand;
    KProbability<uint64_t, 1024, 32>            m_VenationRand;

//    KAntiFarmer                         m_AntiFarmer;
    KMENTOR_DATA_ON_PLAYER_NOW_USE     m_MontorData;
    bool                               m_DataBlockExist[rbtTotal + 1];
};

class KQueryRoleData
{
public:
    KQueryRoleData(void);
    ~KQueryRoleData(void);

public:
    int ParseRoleBaseData(unsigned char* pbyData, size_t uDataLen);
    int ParseRoleData(unsigned char* pbyData, size_t uDataLen);
    int ParseFellowshipData();
    int ParseMailData();
    int ParseMapcopyData();

public:
    RoleDataAll m_roleDataAll;

private:
    int ParseSkillList(unsigned char* pbyData, size_t uDataLen);
    int ParseSkillRecipeList(unsigned char* pbyData, size_t uDataLen);
    int ParseItemList(unsigned char* pbyData, size_t uDataLen);
    int ParseQuestList(unsigned char* pbyData, size_t uDataLen);
    int ParseProfessionList(unsigned char* pbyData, size_t uDataLen);
    int ParseRecipeListV0(unsigned char* pbyData, size_t uDataLen);
    int ParseRecipeListV1(unsigned char* pbyData, size_t uDataLen);
    int ParseStateInfo(unsigned char* pbyData, size_t uDataLen);
    int ParseBuffList(unsigned char* pbyData, size_t uDataLen);
    int ParseRepute(unsigned char* pbyData, size_t uDataLen);
    int ParseUserPreferences(unsigned char* pbyData, size_t uDataLen);
    int ParseBookState(unsigned char* pbyData, size_t uDataLen);
    int ParseCoolDownTimer(unsigned char* pbyData, size_t uDataLen);
    int ParseRoadOpenList(unsigned char* pbyData, size_t uDataLen);
    int ParseCustomData(unsigned char* pbyData, size_t uDataLen);
    int ParseVisitedMap(unsigned char* pbyData, size_t uDataLen);
    int ParsePQList(unsigned char* pbyData, size_t uDataLen);
    int ParseHeroData(unsigned char* pbyData, size_t uDataLen);
    int ParseAchievementData(unsigned char* pbyData, size_t uDataLen);
    int ParseDesignationData(unsigned char* pbyData, size_t uDataLen);
    int ParseRandData(unsigned char* pbyData, size_t uDataLen);
    //int ParseAntiFarmer(unsigned char* pbyData, size_t uDataLen);
    int PaserMentorData(unsigned char* pbyData, size_t uDataLen);
};

#endif //KQUERYROLEDATA_H
