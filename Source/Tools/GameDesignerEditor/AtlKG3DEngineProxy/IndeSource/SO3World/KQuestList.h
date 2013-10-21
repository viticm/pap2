/************************************************************************/
/* 玩家任务表															*/
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.11.01	Create												*/
/************************************************************************/
#ifndef _KQUEST_LIST_H_
#define _KQUEST_LIST_H_

#include <list>
#include "Global.h"
#include "SO3Result.h"
#include "KQuestInfoList.h"

class KNpc;
class KPlayer;
class KItem;
class KDistrict;
class KTarget;

#define MAX_ACCEPT_QUEST_COUNT	25
#define MAX_ASSIST_QUEST_COUNT	16

#define QUEST_HIDE_LEVEL 5
#define QUEST_WHITE_LEVEL 5

enum QUEST_STATE
{
	qsInvalid = -1,

	qsUnfinished = 0,
	qsFinished,
};

enum QUEST_DATA_TYPE
{
    qdtInvalid = -1,

    qdtQuestState,
    qdtQuestList,
    qdtDailyQuest
};

struct KQuest
{
	KQuestInfo* pQuestInfo;
	DWORD		dwKillNpcCount[QUEST_PARAM_COUNT];	// 杀怪数量
	int			nQuestValue[QUEST_PARAM_COUNT * 2];	// 任务变量
	BOOL		bFailed;							// 任务是否已经失败
	time_t		nLimitTime;							// 任务的限制时间
    int         nRoundCount;                        // 任务道具轮流拾取的计数器
};

#pragma pack(1)
struct KASSIST_PRESENT_NOTIFY 
{
    DWORD       dwQuestID;
    int         nAddStamina;
    int         nAddThew;
};

struct KDB_ACCEPT_QUEST
{
	WORD        wQuestID;
	BYTE		byKillNpcCount[QUEST_PARAM_COUNT];
	int			nQuestValue[QUEST_PARAM_COUNT * 2];
	BYTE		byFailed;
	time_t		nLimitTime;
};

struct KDB_ASSIST_QUEST
{
    DWORD dwPlayerID;
    WORD  wQuestID;
};

struct KDB_DAILY_QUEST
{
    WORD wQuestID;
    time_t  nNextAcceptTime;
};

struct KDB_DAILY_QUEST_DATA
{
    BYTE            byCount;
    KDB_DAILY_QUEST DailyQuest[0];
};
#pragma pack()

class KQuestList
{
public:
	BOOL                Init(KPlayer* pPlayer);
	void                UnInit();
	void                Activate();
    
    BOOL                LoadQuestState(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);
    BOOL                LoadQuestList(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);
    BOOL                LoadDailyQuest(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);

#ifdef _SERVER
    BOOL                SaveQuestState(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL                SaveQuestList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL                SaveDailyQuest(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);

    BOOL                LoadAssistQuestList(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);
    BOOL                SaveAssistQuestList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
#endif

    QUEST_RESULT_CODE   CanAccept(KQuestInfo* pQuestInfo, KTarget* pTarget);
    time_t              GetDailyQuestNextAcceptTime(int nDailyQuestCycle, int nDailyQuestOffset);
    QUEST_RESULT_CODE   CanAcceptDailyQuest(KQuestInfo* pQuestInfo, KNpc* pAcceptNpc);
    void                CheckDailyQuestFinished(KQuestInfo* pQuestInfo);
    int                 GetFinishedDailyQuestCount();

#ifdef _SERVER
	QUEST_RESULT_CODE   Accept(KTarget* pTarget, DWORD dwQuestID, BOOL bIgnoreRules = false);
    QUEST_RESULT_CODE   Finish(DWORD dwQuestID, KTarget& rTarget, int nPresentChoice1, int nPresentChoice2, BOOL bIgnoreRules = false);
    void                OnKillNpc(DWORD dwTemplateID);
    BOOL                ResetAssistDailyCount();
    int                 GetQuestRoundCount(DWORD dwQuestID);
    BOOL                SetQuestRoundCount(DWORD dwQuestID, int nQuestRoundCount);
#else
    QUEST_RESULT_CODE   Accept(DWORD dwQuestID, BOOL bDailyQuest);
    QUEST_RESULT_CODE   Finish(DWORD dwQuestID);
	void                UpdateNpcQuestMark(int nQuestID);
    void                OnUpdateItem(DWORD dwType, DWORD dwIndex, BOOL bGetItem);

    void                SetAssistDailyCount(int nValue);
    int                 GetAssistDailyCount();
#endif

	QUEST_RESULT_CODE   Cancel(int nQuestIndex);
    BOOL                ClearQuest(DWORD dwQuestID);
	QUEST_RESULT_CODE   CanFinish(KQuestInfo* pQuestInfo, KTarget* pTarget = NULL);
	int                 GetAcceptedCount(void);
	QUEST_STATE	        GetQuestState(DWORD dwQuestID);

    // 获取任务阶段 -1：异常 0：还没接任务 1：已接未完成 2：完成但没交 3：已经交任务
	int                 GetQuestPhase(DWORD dwQuestID);
	DWORD               GetQuestID(int nQuestIndex);
	int                 GetQuestIndex(DWORD dwQuestID);
	int                 GetQuestValue(int nQuestIndex, int nValueIndex);
	BOOL                SetQuestValue(int nQuestIndex, int nValueIndex, int nNewValue);
	DWORD               GetKillNpcCount(int nQuestIndex, int nCountIndex);
	BOOL                SetKillNpcCount(int nQuestIndex, int nCountIndex, int nNewCount);
	BOOL                SetQuestFailedFlag(int nQuestIndex, BOOL bFlag);
	BOOL                GetQuestFailedFlag(int nQuestIndex);
	int                 GetLastTime(int nQuestIndex);
#ifdef _SERVER
    BOOL                RegisterPartyAssistance(DWORD dwQuestID);
#endif
	BOOL				GetQuestExpAttenuation(KQuestInfo* pQuestInfo, int* pnExp, int* pnRepeatCutPercent, int* pnLevelCutPercent);
    
    BOOL                IsQuestEndItemEnoughByIndex(KQuestInfo* pQuestInfo, int nIndex, BOOL bAllPackage);
    
    void                ClearQuestList();

private:
    BOOL                IsQuestEndItemEnough(KQuestInfo* pQuestInfo, BOOL bAllPackage);
    BOOL                IsQuestEndBookMemorized(KQuestInfo* pQuestInfo);

    void                ProcessLimitTimeQuest();
    void                ProcessDailyQuest();
    QUEST_RESULT_CODE   CommonCheck(KQuestInfo* pQuestInfo);

#ifdef _SERVER
	QUEST_RESULT_CODE   Present(KQuestInfo* pQuestInfo, int nPresentChoice1, int nPresentChoice2);
    BOOL                AssistPresent(KQuestInfo* pQuestInfo);
    BOOL                CheckFreeRoomForPresent(KQuestInfo* pQuestInfo);
    QUEST_RESULT_CODE   CanPresentItems(KQuestInfo* pQuestInfo, int nPresentChoice1, int nPresentChoice2);
    QUEST_RESULT_CODE   CanPresentItem(DWORD dwTabType, DWORD dwIndex, int nAmount);
    QUEST_RESULT_CODE   PresentItems(KQuestInfo* pQuestInfo, int nPresentChoice1, int nPresentChoice2);
    QUEST_RESULT_CODE   PresentItem(DWORD dwTabType, DWORD dwIndex, int nAmount);
    BOOL                UnregisterQuestKillNpc(KQuestInfo* pQuestInfo);
    BOOL                CostQuestRequireItem(KQuestInfo* pQuestInfo);
    BOOL                CostQuestOfferItem(KQuestInfo* pQuestInfo);
    BOOL                RegisterQuestAssistInfo(DWORD dwPlayerID, DWORD dwQuestID);
#endif
    
private:
	BYTE	    m_byQuestStateList[MAX_QUEST_COUNT];
	KPlayer*    m_pPlayer;
	KQuest	    m_AcceptedQuestList[MAX_ACCEPT_QUEST_COUNT];
	int		    m_nAcceptedCount;
    int         m_nAssistDailyCount;

#ifdef _SERVER
    struct KASSIST_QUEST 
    {
        DWORD dwPlayerID; // 表示我受到此人的协助
        DWORD dwQuestID;  // 表示此人协助我做了哪个任务
    };
    
    typedef std::list<KASSIST_QUEST> KASSIST_QUEST_LIST;
    KASSIST_QUEST_LIST m_AssistQuestList;
#endif

	struct KQuestRequirement
	{
		int			nQuestIndex;
		int			nParamIndex;
	};

	typedef std::multimap<DWORD, KQuestRequirement>		MAP_NPC_2_QUEST;
	MAP_NPC_2_QUEST				m_mapNpc2Quest;

    struct KDAILY_QUEST
    {
       DWORD  dwQuestID;
       time_t nNextAcceptTime;
    };
       
    KDAILY_QUEST m_DailyQuest[MAX_ACCEPT_QUEST_COUNT];

#ifdef _CLIENT
	struct KUpdateQuestMarkFunc
	{
		KUpdateQuestMarkFunc(void)
		{
			m_nQuestID = -1;
		}
		BOOL operator()(KNpc* pNpc);

		int m_nQuestID;
	}   m_UpdateQuestMarkFunc;
#endif
};
#endif
