#ifndef _KLOOT_LIST_H_
#define _KLOOT_LIST_H_

#include "Global.h"
#include "SO3Result.h"

//拾取保护时间，时间到后任何人都可以拾取
#define FREE_LOOT_TIME		(GAME_FPS * 300)
//掷骰子时间，时间到后，当前最高数字的人获得拾取权利
#define LOOT_ROLL_TIME		(GAME_FPS * 120)
#define ROLL_ITEM_NUMBER	100

class KItem;
class KPlayer;
class KNpc;
class KDoodad;

struct KLootItem
{
	KItem*		pItem;
	int			nRollCount;
	KPlayer*	pWinner;
	int			nRandom;
	DWORD		dwFlag[MAX_PARTY_SIZE];			
	BOOL		bCanOTFreeLoot;					//FALSE表示即使过保护时间，或者拾取方式为自由拾取，也只能由winner来拾取(用来限制某些任务物品)
	BOOL		bDisplay;
};

class KLootList
{
public:
	KLootList(void);
	~KLootList(void);

	BOOL Init(void);
	BOOL UnInit(void);

#ifdef _SERVER
	//每个游戏循环做检查，看保护时间是否已到
	BOOL CheckGameLoop(void);
#endif //_SERVER

	//往拾取表中加入物品，返回序号，从零开始计数
	int AddItem(KItem* pItem);
	BOOL RemoveItem(KItem* pItem);
	//添加money
	BOOL AddMoney(int nMoney);
	BOOL SetMoney(int nMoney);
	inline int GetMoney();
	
	// 最后一个参数表示是否该道具能在超时后变成自由拾取
	BOOL SetWinner(int nItemIndex, KPlayer *pWinner, BOOL bCanOTFreeLoot);

	//获取表中物品总数
	int GetItemCount(void);
	//loot后的剩余物品数
	inline int GetSize(void);
	KLootItem* GetLootItem(DWORD dwLootIndex);
	BOOL CanLoot(DWORD dwLootIndex, KPlayer* pPlayer);
	BOOL CanRoll(DWORD dwRollIndex, KPlayer* pPlayer, BOOL bCheckRollFlag);

	BOOL IsVisible(KPlayer* pPlayer);

	inline BOOL SetLootMode(int nLootMode);
	inline PARTY_LOOT_MODE GetLootMode();

	inline BOOL SetRollQuality(int nRollQuality);
	inline int GetRollQuality();

	void OnOpen();
	void OnClose();

#if defined(_SERVER)
	// 生成m_pLooterList
	BOOL PreProcess(KPlayer *pAttacker, KNpc *pNpc);

	BOOL SetOwnerInParty();
	BOOL IsEmpty();

	//拾取表中的物品
	SKILL_RESULT_CODE LootOrRollItem(KPlayer* pLooter, DWORD dwDoodadID, DWORD dwItemID, BOOL bCancel);
	SKILL_RESULT_CODE LootMoney(KPlayer *pPlayer, KDoodad *pDoodad);
	SKILL_RESULT_CODE LeaderDistributeItem(KPlayer* pLeader, KPlayer* pDstPlayer, KDoodad* pDoodad, DWORD dwItemID);
#endif	//_SERVER

	BOOL IsInLooterList(KPlayer* pPlayer);

	KPlayer*	m_pLooterList[MAX_PARTY_SIZE]; //所有参与者，队长模式下有被分配权，保护时间结束后有自由拾取权
	int			m_nLooterCount; //m_pLooterList中的玩家数量
	KDoodad*	m_pDoodad;
private:
	KLootItem	m_LootList[MAX_LOOT_SIZE];
	int			m_nItemCount;
	int			m_nLootedCount;
	int			m_nGameLoop;
	int			m_nMoney;
	int			m_nLootMode;
	int			m_nRollQuality;
	BOOL		m_bIsOpened;

	BOOL		m_bOverLootRollTime;
	BOOL		m_bOverFreeLootTime;
};

inline int KLootList::GetSize()
{
	return m_nItemCount - m_nLootedCount;
}

inline int KLootList::GetMoney()
{
	return m_nMoney;
}

inline BOOL KLootList::SetLootMode(int nLootMode)
{
	KGLOG_PROCESS_ERROR(nLootMode > plmInvalid && nLootMode < plmTotal);

	m_nLootMode = nLootMode;
	
	return TRUE;
Exit0:
	return FALSE;
}

inline PARTY_LOOT_MODE KLootList::GetLootMode()
{
	return (PARTY_LOOT_MODE)m_nLootMode;
}

inline BOOL KLootList::SetRollQuality(int nRollQuality)
{
	KGLOG_PROCESS_ERROR(nRollQuality >= 0);

	m_nRollQuality = nRollQuality;

	return TRUE;
Exit0:
	return FALSE;
}

inline int KLootList::GetRollQuality()
{
	return m_nRollQuality;
}

#endif	//_KLOOT_LIST_H_