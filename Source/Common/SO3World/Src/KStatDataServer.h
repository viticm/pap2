#ifndef _KSTAT_DATA_SERVER_H_
#define _KSTAT_DATA_SERVER_H_

#ifdef _SERVER

#include "Engine/KMemory.h"
#include "KStatDataDef.h"
#include <list>

class KPlayer;
class KScene;

// Server自己维护的统计数据结构
struct KStatData
{
    KStatData() : m_nValue(0), m_nID(0), m_bInList(false) {}
    char    m_szName[STAT_DATA_NAME_LEN];
    int64_t m_nValue;
    int     m_nID;
    BOOL    m_bInList;
};

class KStatDataServer
{
public:
    KStatDataServer();
    ~KStatDataServer();

    BOOL Init();
    void UnInit();

    void Activate();
    void SendAllStatData();
    int  GetUpdateStatData(KSTAT_DATA_MODIFY* pStatData, int nMaxCount);
    BOOL Update(const char cszName[], int64_t nValue);
    BOOL SetNameID(const char cszName[], int nID);

public:
    void UpdateMoneyStat(KPlayer* pPlayer, int nMoney, const char cszMethod[]);
    void UpdateContributeStat(KPlayer* pPlayer, int nContribute, const char cszMethod[]);
    void UpdateSkillStat(DWORD dwSkillID, DWORD dwSkillLevel);
    void UpdateFormationStat(DWORD dwFormationID, DWORD dwFormationLevel);
    void UpdateAutoFlyStat(int nValue, const char cszMethod[]);
    void UpdateLearnProfessionLevelState(DWORD dwProfessionID, DWORD dwLevel);
    void UpdateLearnProfessionBranchState(DWORD dwProfessionID, DWORD dwBranchID);
    void UpdateRecipeCastState(DWORD dwCraftID, DWORD dwRecipeID);
    void UpdateEnterScene(KScene* pScene, KPlayer* pPlayer);
    void UpdateItemDrop(DWORD dwItemTab, DWORD dwItemIndex);
    void UpdateTrainState(int nTrain);

    void UpdateQuestStat(DWORD dwQuestID, const char cszState[]);
    void UpdateQuestRepeat(DWORD dwQuestID);
    void UpdateAssistQuest(DWORD dwQuestID);
    void UpdateMentorAssistQuest(DWORD dwQuestID);
    void UpdatePVEStat(DWORD dwVictimID, DWORD dwKillerID);

    void UpdateTalkStat(KPlayer* pPlayer, const char* cszMsgType);
    void UpdatePrestigeStat(KPlayer* pPlayer, int nPrestige, const char* cszMethod);
    void UpdateAchievementPointStat(KPlayer* pPlayer, int nPoint);
    void UpdateAchievementStat(int nID);
    void UpdateDesignationStat(BOOL bIsPrefix, int nID);
    void UpdateShopRepairItemStat(KPlayer* pPlayer, BOOL bIsAllRepair);
    void UpdateExpStat(KPlayer* pPlayer, int nAddExp, const char* cszMethod);
    void UpdateClientNormalLogout();
    void UpdateClientErrorLogout();
    void UpdateClientLoginPermit();
    void UpdateClientTimeout();

private:
    struct KStatNameLess
    {
        bool operator()(const char* pszX, const char* pszY) const
        {
            return strcmp(pszX, pszY) < 0;
        }
    };

    typedef KMemory::KAllocator<std::pair<char*, KStatData*> > KDATA_TABLE_ALLOCATOR;
    typedef std::map<char*, KStatData*, KStatNameLess, KDATA_TABLE_ALLOCATOR> KDATA_TABLE;

    KDATA_TABLE     m_DataTable;

    typedef std::list<KStatData*, KMemory::KAllocator<KStatData*> > KDATA_QUEUE;
    KDATA_QUEUE     m_DataList;

    std::set<kstring, std::less<kstring>, KMemory::KAllocator<kstring> > m_Name2IDQueue;
};
#endif // _SERVER

#endif
