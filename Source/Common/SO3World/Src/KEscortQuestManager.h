#ifndef _KESCORT_QUEST_MANAGER_H_
#define _KESCORT_QUEST_MANAGER_H_
#include <map>

#ifdef _SERVER

class KScene;
class KPlayer;
class KNpc;

enum ESCORT_QUEST_STATE
{
    eqsInvalid = 0,
	
	eqsWaitStart,
	eqsRunning,
    eqsFinished,
    eqsClosed,

	eqsTotal
};

struct KEscortQuest
{
	DWORD				dwNpcAcceptID;                  // 接护送任务的NPC
	DWORD             	dwNpcEscortID;                  // 护送的NPC
	DWORD             	dwPlayerID[MAX_PARTY_SIZE];     // 参与者
	BOOL				bValid[MAX_PARTY_SIZE];		    // 任务进行中标志
	int             	nTimer;                         // 倒计时
	int               	nEndFrames;                     // 任务的截止帧
	ESCORT_QUEST_STATE  eQuestState;                    // 是否在活动的标记
};

class KEscortQuestManager
{
public:
    BOOL Init(KScene* pScene);
    void UnInit(){};

    void Activate();

    BOOL AddEscortQuest(DWORD dwQuestID, KPlayer* pPlayerAccept, KNpc* pNpcAccept);
    BOOL CanAcceptEscortQuest(DWORD dwQuestID);
    BOOL AddPlayer(DWORD dwQuestID, DWORD dwPlayerID);
    BOOL StartEscortQuest(DWORD dwQuestID);
    BOOL SetEscortNpc(DWORD dwQuestID, DWORD dwNpcEscortID);
    BOOL SetEscortQuestEndFrames(DWORD dwQuestID, int nFrames);
    BOOL SetEscortQuestValue(DWORD dwQuestID, int nIndex, int nValue);
    BOOL AddPlayerBuff(DWORD dwQuestID, DWORD dwBuffID, int nBuffLevel);
    BOOL DelPlayerBuff(DWORD dwQuestID, DWORD dwBuffID, int nBuffLevel);
    BOOL SetPlayerFailed(KPlayer* pPlayer, DWORD dwQuestID);
    BOOL CloseEscortQuest(DWORD dwQuestID, int nFrames);
    BOOL EndEscortQuest(DWORD dwQuestID);

    const KEscortQuest* GetEscortQuest(DWORD dwQuestID);

private:
    BOOL InvitePlayerInTeam(DWORD dwQuestID, KPlayer* pPlayerAccept);
    void OnQuestWaitStart(DWORD dwQuestID, KEscortQuest* pEscortQuest);
    void OnQuestRunning(DWORD dwQuestID, KEscortQuest* pEscortQuest);

private:
    typedef std::map<DWORD, KEscortQuest> MAP_ESCORT_QUEST;
    MAP_ESCORT_QUEST m_EscortQuestMap;

    KScene* m_pScene;
};
#endif
#endif
