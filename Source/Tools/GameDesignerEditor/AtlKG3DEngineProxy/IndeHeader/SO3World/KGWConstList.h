#ifndef _KGW_CONST_LIST_
#define _KGW_CONST_LIST_

#include "Global.h"

#define PLAYER_AND_QUEST_DIFF_LEVEL_COUNT 4 
#define MAX_KILL_COUNT                    11

struct KGWConstList
{
public:
	// AI
	int nMaxThreatCount;
	int nNpcSkillCommonCD;
	int nNpcCommonShortCD;
	int nAISearchByLifeConversionLevel;

	// PK
	int nPKFlagDoodadTemplateID;
	int nSlayCDIndex;
	int nWaitSlayTime;
	int nCloseSlayTime;
	int nMinDuelLevel;
	int nMinRevengeLevel;
	int nMinSlayLevel;
	int nMinBeSlayedLevel;

	// Mail
	int nMailDelaySecondOnText;
	int nMailDelaySecondOnMoney;
	int nMailDelaySecondOnItem;
	int nMailSurvivalTime;

	// Item
	int nBankPackagePrice[MAX_BANK_PACKAGE_COUNT];

	// Fellowship
	int nPartyAttractiveInterval;       // 组队增加好感度的间隔(Frame)
	int nPartyAttractiveRange;          // 组队增加好感度的距离
	int nPartyAttractiveUpLimit;        // 组队增加好感度的上限值
	int nPartyAttractiveAdd;

	int nPartyAttractiveIntervalOnlyGroup;       // 组队增加好感度的间隔(Frame)
	int nPartyAttractiveRangeOnlyGroup;          // 组队增加好感度的距离
	int nPartyAttractiveUpLimitOnlyGroup;        // 组队增加好感度的上限值
	int nPartyAttractiveAddOnlyGroup;

	int nDuelAttractiveDailyCount;      // 切磋增加好感度的每日限制
	int nDuelAttractiveUpLimit;         // 切磋增加好感度的上限值
	int nDuelAttractiveAdd;

	int nAssistAttractiveUpLimit;       // 协助任务增加好感度的上限值

	int nWhisperAttractiveDailyCount;   // 密聊增加好感度的每日限制
	int nWhisperAttractiveUpLimit;      // 密聊增加好感度的上限值
	int nWhisperAttractiveAdd;

	// 溺水而亡的时间,单位: 秒
	int nDrowningTime;
	int nSaveInterval;

	// NPC死亡后,产生尸体的延迟时间
	int nCorpseGenerationFrame;

	// 玩家可升级的最大等级
	int nPlayerMaxLevel;

	// Skill
	int nRiseFrame;
	int nSkidFrame;
	int nNoAddSkillExp;

	int nSceneChannelCoolDownID;
	int nWorldChannelCoolDownID;

	int nForceChannelCoolDownID;
	int nForceChannelDailyLimit;

	int nCampChannelCoolDownID;
	int nCampChannelDailyLimit;

	int nPlayerAndQuestDiffLevel[PLAYER_AND_QUEST_DIFF_LEVEL_COUNT];

	// 阵营威望相关的奖励与惩罚
	int nReducePrestigeOnDeath;
	int nSoloKill;
	int nLastHit;
	int nCommonKill;
	int nKillerPartyMember;
	int nSameCampKill;
	int nSameCampAssistKill;
	int nKilledCountPercent[MAX_KILL_COUNT];
	int nResetKilledCountCycle;

	// 是否使用新的使用道具机制，过渡用
	BOOL bNewUseItemStyle;

	BOOL Init();
	void UnInit();
private:
	BOOL LoadData(IIniFile* piIniFile);
};

#endif //_KGW_CONST_LIST_
