/************************************************************************/
/* 任务配置表															*/
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.11.01	Create												*/
/************************************************************************/
#ifndef _KQUEST_INFO_LIST_H_
#define _KQUEST_INFO_LIST_H_

#include <map>
#include <vector>
#include "Global.h"
#include "SO3ProtocolBasic.h"
#include "Luna.h"

#define QUEST_NAME_LEN					32
#define QUEST_OBJECTIVE_LEN				1026
#define QUEST_DESCRIPTION_LEN			1024
#define QUEST_UNFINISHED_DIALOGUE_LEN	1024
#define QUEST_FINISHED_DIALOGUE_LEN		1024
#define QUEST_VALUE_STR_LEN				256
#define QUEST_GOSSIP_LEN		        1024

#define MAX_QUEST_COUNT			        8192
#define QUEST_END_ITEM_COUNT            8

class ITabFile;

struct KQuestInfo
{
	DWORD			dwQuestID;
	char			szName[QUEST_NAME_LEN];
	char			szQuestClass[QUEST_NAME_LEN];				// 任务分类名称
	BOOL			bShare;										// 是否可共享
	BOOL			bRepeat;									// 是否可重复
	BOOL			bAssist;									// 是否可协助
	BOOL			bAccept;									// 是否需要接了才能交
	BOOL			bEscortQuest;                               // 是否护送任务
	int				nLevel;										// 任务等级

	char			szQuestDiff[QUEST_NAME_LEN];				// 任务难度描述		

	//接任务相关信息
	DWORD			dwStartMapID;								// 接任务Npc所在地图的ID, 暂时废弃
	DWORD			dwStartNpcTemplateID;						// 接任务Npc的模板ID
	DWORD			dwStartDoodadTemplateID;					// 接任务Doodad的模板ID
	DWORD			dwStartItemType;							// 接任务Item类型
	DWORD			dwStartItemIndex;							// 接任务Item类型

#ifdef _CLIENT
	char			szDescription[QUEST_DESCRIPTION_LEN];		// 接任务描述
	char			szObjective[QUEST_OBJECTIVE_LEN];			// 接任务目标
	int				nStartGossipType;							// 接任务闲谈类型
	char			szStartGossip[QUEST_PARAM_COUNT][QUEST_GOSSIP_LEN];
#endif
																// 接任务闲谈内容

	int				nMinLevel;									// 接任务最小等级需求
	int				nMaxLevel;									// 接任务最大等级需求
	int				nRequireGender;								// 接任务的性别需求
	int				nRequireRoleType;							// 接受任务的玩家体形需求
	DWORD			dwCoolDownID;								// CoolDown计时器ID
	BOOL			bPrequestLogic;								// true:and;false:or
	DWORD			dwPrequestID[QUEST_PARAM_COUNT];			// 前置任务ID
	DWORD			dwSubsequenceID;							// 直接后继任务ID
	int				nStartTime;									// 开始时间
	int				nEndTime;									// 结束时间

	DWORD			dwRequireSkillID;							// 需求技能
	DWORD			dwMinSkillLevel;							// 需求技能最小等级

	DWORD			dwRequireLifeSkillID;						// 需求技能
	DWORD			dwMinLifeSkillLevel;						// 需求技能最小等级
 
	DWORD			dwStartRequireItemType[QUEST_PARAM_COUNT];	// 交任务时所需道具的类型
	DWORD			dwStartRequireItemIndex[QUEST_PARAM_COUNT];	// 交任务时所需道具的类型
	DWORD			dwStartRequireItemAmount[QUEST_PARAM_COUNT];// 交任务时所需道具的数量

	DWORD			dwOfferItemType[QUEST_PARAM_COUNT];			// 接任务时提供道具的类型
	DWORD			dwOfferItemIndex[QUEST_PARAM_COUNT];		// 接任务时提供道具的类型
	DWORD			dwOfferItemAmount[QUEST_PARAM_COUNT];		// 接任务时提供道具的数量

    DWORD           dwRequireSchoolMask;                        // 接任务时的门派限制
    BOOL            bRequireReputeAll;                          // 接任务时声望限制的逻辑关系(true: AND, false: OR)
    DWORD           dwRequireForceID[QUEST_PARAM_COUNT];
    int             nReputeLevelMin[QUEST_PARAM_COUNT];
    int             nReputeLevelMax[QUEST_PARAM_COUNT];

	//交任务相关信息
	DWORD			dwEndMapID;									//交任务Npc所在的地图ID, 暂时废弃
	DWORD			dwEndNpcTemplateID;							//交任务Npc的模板ID
	DWORD			dwEndDoodadTemplateID;						//交任务Doodad的模板ID
	DWORD			dwEndItemType;								//交任务Item类型
	DWORD			dwEndItemIndex;								//交任务Item类型

#ifdef _CLIENT
	char			szDunningDialogue[QUEST_UNFINISHED_DIALOGUE_LEN];
	char			szUnfinishedDialogue[QUEST_UNFINISHED_DIALOGUE_LEN];
    char			szFinishedDialogue[QUEST_FINISHED_DIALOGUE_LEN];
	int				nEndGossipType;								//交任务闲谈类型
	char			szEndGossip[QUEST_PARAM_COUNT][QUEST_GOSSIP_LEN];
                                                                //交任务闲谈内容
#endif

	int				nFinishTime;								//完成时间限制

	DWORD			dwEndRequireItemType[QUEST_END_ITEM_COUNT];	//交任务时所需道具的类型
	DWORD			dwEndRequireItemIndex[QUEST_END_ITEM_COUNT];	//交任务时所需道具的类型
	DWORD			dwEndRequireItemAmount[QUEST_END_ITEM_COUNT];	//交任务时所需道具的数量
	BOOL			bIsDeleteEndRequireItem[QUEST_END_ITEM_COUNT]; //交任务或者删除任务的时候是否删除这个道具
	DWORD			dwDropItemNpcTemplateID[QUEST_END_ITEM_COUNT];		//掉落交任务道具的Npc模板ID
	DWORD			dwDropItemDoodadTemplateID[QUEST_END_ITEM_COUNT];	//掉落交任务道具的doodad模板ID
	DWORD			dwDropItemRate[QUEST_END_ITEM_COUNT];			//掉落任务道具的几率
	BOOL			bDropForEach[QUEST_END_ITEM_COUNT];			//是否每人都获得一个

	int				nCostMoney;									//交任务需求的金钱

	DWORD			dwKillNpcTemplateID[QUEST_PARAM_COUNT];		//交任务时所需杀死的Npc模板ID
	DWORD			dwKillNpcAmount[QUEST_PARAM_COUNT];			//交任务时所需杀死的Npc数量

	int				nQuestValue[QUEST_PARAM_COUNT * 2];			//任务变量
#ifdef _CLIENT
	char			szQuestValueStr[QUEST_PARAM_COUNT * 2][QUEST_VALUE_STR_LEN]; //任务变量字符串
#endif

	DWORD			dwRelationDoodadID[QUEST_PARAM_COUNT * 2];  // 任务相关联的DoodadID

	BOOL			bHidePresent;								// 是否隐藏奖励
	BOOL			bPresentAll[2];								// 第一组(1-4)道具是否全给或者只给一件
	DWORD			dwPresentItemType[QUEST_PARAM_COUNT * 2];	// 交任务时奖励道具的类型
	DWORD			dwPresentItemIndex[QUEST_PARAM_COUNT * 2];	// 交任务时奖励道具的类型
	DWORD			dwPresentItemAmount[QUEST_PARAM_COUNT * 2];	// 交任务时奖励道具的数量

	int				nPresentExp;								// 交任务时奖励的经验
	int				nPresentMoney;								// 交任务时奖励的金钱数量
	int				nPresentSkill;								// 奖励的技能ID

    int             nPresentAssistThew;                         // 协助任务奖励体力
    int             nPresentAssistStamina;                      // 协助任务奖励精力
    int             nPresentAssistFriendship;                   // 协助任务奖励好感度

	DWORD			dwAffectForceID[QUEST_PARAM_COUNT];			// 交任务影响的势力
	int				nAffectForceValue[QUEST_PARAM_COUNT];		// 交任务影响的势力友好度

	int				nRepeatCutPercent;							// 重复做任务时奖励的削减百分比

	char			szScriptName[MAX_PATH];						// 脚本文件路径

    int             nRequireCampMask;                           // 阵营需求; 第0,1,2位分别对应中立、正义、邪恶
    int             nPresentPrestige;                           // 任务奖励威望
    int             nPresentContribution;                       // 任务奖励贡献值
    int             nPresentTrain;                              // 任务奖励修为
#ifdef _SERVER
    int             nAchievementID;                             // 任务成就
#endif

//Lua脚本接口
	DECLARE_LUA_CLASS(KQuestInfo);

	DECLARE_LUA_STRUCT_STRING(Name, QUEST_NAME_LEN, szName);
	DECLARE_LUA_STRUCT_STRING(QuestClass, QUEST_NAME_LEN, szQuestClass);
	DECLARE_LUA_STRUCT_BOOL(Share, bShare);					    // 是否可共享
	DECLARE_LUA_STRUCT_BOOL(Repeat, bRepeat);					// 是否可重复
	DECLARE_LUA_STRUCT_BOOL(Assist, bAssist);					// 是否可协助
	DECLARE_LUA_STRUCT_BOOL(Accept, bAccept);					// 是否先接才能交
	DECLARE_LUA_STRUCT_INTEGER(Level, nLevel);					// 任务等级

	DECLARE_LUA_STRUCT_STRING(QuestDiff, QUEST_NAME_LEN, szQuestDiff);

	DECLARE_LUA_STRUCT_DWORD(StartMapID, dwStartMapID);							//接任务Npc所在地图的ID
	DECLARE_LUA_STRUCT_DWORD(StartNpcTemplateID, dwStartNpcTemplateID);			//接任务Npc的ID
	DECLARE_LUA_STRUCT_DWORD(StartItemType, dwStartItemType);					//接任务Item类型
	DECLARE_LUA_STRUCT_DWORD(StartItemIndex, dwStartItemIndex);					//接任务Item类型

#ifdef _CLIENT
	DECLARE_LUA_STRUCT_STRING(Objective, QUEST_OBJECTIVE_LEN, szObjective);		//接任务时的Npc对话内容
    DECLARE_LUA_STRUCT_STRING(Description, QUEST_DESCRIPTION_LEN, szDescription);
	DECLARE_LUA_STRUCT_INTEGER(StartGossipType, nStartGossipType);						//接任务闲谈类型
	DECLARE_LUA_STRUCT_STRING(StartGossip1, QUEST_GOSSIP_LEN, szStartGossip[0]);		//接任务闲谈内容
	DECLARE_LUA_STRUCT_STRING(StartGossip2, QUEST_GOSSIP_LEN, szStartGossip[1]);		//接任务闲谈内容
	DECLARE_LUA_STRUCT_STRING(StartGossip3, QUEST_GOSSIP_LEN, szStartGossip[2]);		//接任务闲谈内容
	DECLARE_LUA_STRUCT_STRING(StartGossip4, QUEST_GOSSIP_LEN, szStartGossip[3]);		//接任务闲谈内容
#endif

    DECLARE_LUA_STRUCT_DWORD(RequireSchoolMask, dwRequireSchoolMask);

    DECLARE_LUA_STRUCT_BOOL(RequireReputeAll, bRequireReputeAll);
    DECLARE_LUA_STRUCT_DWORD(RequireForceID1, dwRequireForceID[0]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMin1, nReputeLevelMin[0]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMax1, nReputeLevelMax[0]);
    DECLARE_LUA_STRUCT_DWORD(RequireForceID2, dwRequireForceID[1]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMin2, nReputeLevelMin[1]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMax2, nReputeLevelMax[1]);
    DECLARE_LUA_STRUCT_DWORD(RequireForceID3, dwRequireForceID[2]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMin3, nReputeLevelMin[2]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMax3, nReputeLevelMax[2]);
    DECLARE_LUA_STRUCT_DWORD(RequireForceID4, dwRequireForceID[3]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMin4, nReputeLevelMin[3]);
    DECLARE_LUA_STRUCT_INTEGER(ReputeLevelMax4, nReputeLevelMax[3]);

    DECLARE_LUA_STRUCT_INTEGER(MinLevel, nMinLevel);			//接任务最小等级需求
	DECLARE_LUA_STRUCT_INTEGER(MaxLevel, nMaxLevel);			//接任务最大等级需求
	DECLARE_LUA_STRUCT_INTEGER(RequireGender, nRequireGender);	//接任务的性别需求
	DECLARE_LUA_STRUCT_INTEGER(RequireRoleType, nRequireRoleType); //接任务的体型需求
	DECLARE_LUA_STRUCT_DWORD(CoolDownID, dwCoolDownID);		//CoolDown计时器ID
	DECLARE_LUA_STRUCT_BOOL(PrequestLogic, bPrequestLogic);
	DECLARE_LUA_STRUCT_DWORD(PrequestID1, dwPrequestID[0]);			//前置任务ID
	DECLARE_LUA_STRUCT_DWORD(PrequestID2, dwPrequestID[1]);			//前置任务ID
	DECLARE_LUA_STRUCT_DWORD(PrequestID3, dwPrequestID[2]);			//前置任务ID
	DECLARE_LUA_STRUCT_DWORD(PrequestID4, dwPrequestID[3]);			//前置任务ID
	DECLARE_LUA_STRUCT_INTEGER(StartTime, nStartTime);			//开始时间
	DECLARE_LUA_STRUCT_INTEGER(EndTime, nEndTime);				//结束时间
	DECLARE_LUA_STRUCT_DWORD(SubsequenceID, dwSubsequenceID);	//直接后继任务ID   

	DECLARE_LUA_STRUCT_DWORD(RequireSkillID, dwRequireSkillID);	//需求技能
	DECLARE_LUA_STRUCT_DWORD(MinSkillLevel, dwMinSkillLevel);	//需求技能最小等级

	DECLARE_LUA_STRUCT_DWORD(RequireLifeSkillID, dwRequireLifeSkillID);		//需求技能
	DECLARE_LUA_STRUCT_DWORD(MinLifeSkillLevel, dwMinLifeSkillLevel);		//需求技能最小等级

	DECLARE_LUA_STRUCT_DWORD(StartRequireItemType1, dwStartRequireItemType[0]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemIndex1, dwStartRequireItemIndex[0]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemAmount1, dwStartRequireItemAmount[0]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemType2, dwStartRequireItemType[1]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemIndex2, dwStartRequireItemIndex[1]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemAmount2, dwStartRequireItemAmount[1]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemType3, dwStartRequireItemType[2]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemIndex3, dwStartRequireItemIndex[2]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemAmount3, dwStartRequireItemAmount[2]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemType4, dwStartRequireItemType[3]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemIndex4, dwStartRequireItemIndex[3]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(StartRequireItemAmount4, dwStartRequireItemAmount[3]);	//交任务时所需道具的数量

	DECLARE_LUA_STRUCT_DWORD(OfferItemType1, dwOfferItemType[0]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemIndex1, dwOfferItemIndex[0]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemAmount1, dwOfferItemAmount[0]);		//接任务后提供道具的数量
	DECLARE_LUA_STRUCT_DWORD(OfferItemType2, dwOfferItemType[1]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemIndex2, dwOfferItemIndex[1]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemAmount2, dwOfferItemAmount[1]);		//接任务后提供道具的数量
	DECLARE_LUA_STRUCT_DWORD(OfferItemType3, dwOfferItemType[2]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemIndex3, dwOfferItemIndex[2]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemAmount3, dwOfferItemAmount[2]);		//接任务后提供道具的数量
	DECLARE_LUA_STRUCT_DWORD(OfferItemType4, dwOfferItemType[3]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemIndex4, dwOfferItemIndex[3]);			//接任务后提供道具的类型
	DECLARE_LUA_STRUCT_DWORD(OfferItemAmount4, dwOfferItemAmount[3]);		//接任务后提供道具的数量

	DECLARE_LUA_STRUCT_DWORD(EndMapID, dwEndMapID);							//交任务Npc所在的地图ID
	DECLARE_LUA_STRUCT_DWORD(EndNpcTemplateID, dwEndNpcTemplateID);			//交任务Npc的ID
    DECLARE_LUA_STRUCT_DWORD(EndDoodadTemplateID, dwEndDoodadTemplateID);   //交任务Doodad的ID  
	DECLARE_LUA_STRUCT_DWORD(EndItemType, dwEndItemType);					//接任务Item类型
	DECLARE_LUA_STRUCT_DWORD(EndItemIndex, dwEndItemIndex);					//接任务Item类型

#ifdef _CLIENT
	DECLARE_LUA_STRUCT_STRING(DunningDialogue, QUEST_UNFINISHED_DIALOGUE_LEN, szDunningDialogue);
	DECLARE_LUA_STRUCT_STRING(UnfinishedDialogue, QUEST_UNFINISHED_DIALOGUE_LEN, szUnfinishedDialogue);
	DECLARE_LUA_STRUCT_STRING(FinishedDialogue, QUEST_FINISHED_DIALOGUE_LEN, szFinishedDialogue);
	DECLARE_LUA_STRUCT_INTEGER(EndGossipType, nEndGossipType);					//交任务闲谈类型
	DECLARE_LUA_STRUCT_STRING(EndGossip1, QUEST_GOSSIP_LEN, szEndGossip[0]);	//交任务闲谈内容
	DECLARE_LUA_STRUCT_STRING(EndGossip2, QUEST_GOSSIP_LEN, szEndGossip[1]);	//交任务闲谈内容
	DECLARE_LUA_STRUCT_STRING(EndGossip3, QUEST_GOSSIP_LEN, szEndGossip[2]);	//交任务闲谈内容
	DECLARE_LUA_STRUCT_STRING(EndGossip4, QUEST_GOSSIP_LEN, szEndGossip[3]);	//交任务闲谈内容
#endif

	DECLARE_LUA_STRUCT_INTEGER(FinishTime, nFinishTime);						//完成时间限制

	//---------------------------------------------------------------------------------->
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemType1, dwEndRequireItemType[0]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex1, dwEndRequireItemIndex[0]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount1, dwEndRequireItemAmount[0]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem1, bIsDeleteEndRequireItem[0]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID1, dwDropItemNpcTemplateID[0]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID1, dwDropItemDoodadTemplateID[0]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate1, dwDropItemRate[0]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach1, bDropForEach[0]);						//是否每人掉落一个

	DECLARE_LUA_STRUCT_DWORD(EndRequireItemType2, dwEndRequireItemType[1]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex2, dwEndRequireItemIndex[1]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount2, dwEndRequireItemAmount[1]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem2, bIsDeleteEndRequireItem[1]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID2, dwDropItemNpcTemplateID[1]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID2, dwDropItemDoodadTemplateID[1]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate2, dwDropItemRate[1]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach2, bDropForEach[1]);						//是否每人掉落一个

	DECLARE_LUA_STRUCT_DWORD(EndRequireItemType3, dwEndRequireItemType[2]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex3, dwEndRequireItemIndex[2]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount3, dwEndRequireItemAmount[2]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem3, bIsDeleteEndRequireItem[2]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID3, dwDropItemNpcTemplateID[2]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID3, dwDropItemDoodadTemplateID[2]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate3, dwDropItemRate[2]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach3, bDropForEach[2]);						//是否每人掉落一个

	DECLARE_LUA_STRUCT_DWORD(EndRequireItemType4, dwEndRequireItemType[3]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex4, dwEndRequireItemIndex[3]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount4, dwEndRequireItemAmount[3]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem4, bIsDeleteEndRequireItem[3]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID4, dwDropItemNpcTemplateID[3]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID4, dwDropItemDoodadTemplateID[3]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate4, dwDropItemRate[3]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach4, bDropForEach[3]);						//是否每人掉落一个

    DECLARE_LUA_STRUCT_DWORD(EndRequireItemType5, dwEndRequireItemType[4]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex5, dwEndRequireItemIndex[4]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount5, dwEndRequireItemAmount[4]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem5, bIsDeleteEndRequireItem[4]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID5, dwDropItemNpcTemplateID[4]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID5, dwDropItemDoodadTemplateID[4]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate5, dwDropItemRate[4]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach5, bDropForEach[4]);						//是否每人掉落一个

    DECLARE_LUA_STRUCT_DWORD(EndRequireItemType6, dwEndRequireItemType[5]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex6, dwEndRequireItemIndex[5]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount6, dwEndRequireItemAmount[5]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem6, bIsDeleteEndRequireItem[5]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID6, dwDropItemNpcTemplateID[5]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID6, dwDropItemDoodadTemplateID[5]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate6, dwDropItemRate[5]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach6, bDropForEach[5]);						//是否每人掉落一个

    DECLARE_LUA_STRUCT_DWORD(EndRequireItemType7, dwEndRequireItemType[6]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex7, dwEndRequireItemIndex[6]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount7, dwEndRequireItemAmount[6]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem7, bIsDeleteEndRequireItem[6]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID7, dwDropItemNpcTemplateID[6]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID7, dwDropItemDoodadTemplateID[6]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate7, dwDropItemRate[6]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach7, bDropForEach[6]);						//是否每人掉落一个

    DECLARE_LUA_STRUCT_DWORD(EndRequireItemType8, dwEndRequireItemType[7]);			//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemIndex8, dwEndRequireItemIndex[7]);		//交任务时所需道具的类型
	DECLARE_LUA_STRUCT_DWORD(EndRequireItemAmount8, dwEndRequireItemAmount[7]);	//交任务时所需道具的数量
	DECLARE_LUA_STRUCT_BOOL(IsDeleteEndRequireItem8, bIsDeleteEndRequireItem[7]);  //是否删除道具
	DECLARE_LUA_STRUCT_DWORD(DropItemNpcTemplateID8, dwDropItemNpcTemplateID[7]);		//掉落交任务道具的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemDoodadTemplateID8, dwDropItemDoodadTemplateID[7]);	//掉落交任务道具的doodad模板ID
	DECLARE_LUA_STRUCT_DWORD(DropItemRate8, dwDropItemRate[7]);					//掉落任务道具的几率
	DECLARE_LUA_STRUCT_BOOL(DropForEach8, bDropForEach[7]);						//是否每人掉落一个
	//<------------------------------------------------------------------------------------

	DECLARE_LUA_STRUCT_INTEGER(CostMoney, nCostMoney);							//消耗的金钱

	DECLARE_LUA_STRUCT_DWORD(KillNpcTemplateID1, dwKillNpcTemplateID[0]);		//交任务时所需杀死的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(KillNpcAmount1, dwKillNpcAmount[0]);				//交任务时所需杀死的Npc数量
	DECLARE_LUA_STRUCT_DWORD(KillNpcTemplateID2, dwKillNpcTemplateID[1]);		//交任务时所需杀死的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(KillNpcAmount2, dwKillNpcAmount[1]);				//交任务时所需杀死的Npc数量
	DECLARE_LUA_STRUCT_DWORD(KillNpcTemplateID3, dwKillNpcTemplateID[2]);		//交任务时所需杀死的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(KillNpcAmount3, dwKillNpcAmount[2]);				//交任务时所需杀死的Npc数量
	DECLARE_LUA_STRUCT_DWORD(KillNpcTemplateID4, dwKillNpcTemplateID[3]);		//交任务时所需杀死的Npc模板ID
	DECLARE_LUA_STRUCT_DWORD(KillNpcAmount4, dwKillNpcAmount[3]);				//交任务时所需杀死的Npc数量

	DECLARE_LUA_STRUCT_INTEGER(QuestValue1, nQuestValue[0]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue2, nQuestValue[1]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue3, nQuestValue[2]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue4, nQuestValue[3]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue5, nQuestValue[4]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue6, nQuestValue[5]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue7, nQuestValue[6]);						//任务变量
	DECLARE_LUA_STRUCT_INTEGER(QuestValue8, nQuestValue[7]);						//任务变量

#ifdef _CLIENT
	DECLARE_LUA_STRUCT_STRING(QuestValueStr1,QUEST_VALUE_STR_LEN ,szQuestValueStr[0]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr2,QUEST_VALUE_STR_LEN ,szQuestValueStr[1]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr3,QUEST_VALUE_STR_LEN ,szQuestValueStr[2]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr4,QUEST_VALUE_STR_LEN ,szQuestValueStr[3]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr5,QUEST_VALUE_STR_LEN ,szQuestValueStr[4]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr6,QUEST_VALUE_STR_LEN ,szQuestValueStr[5]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr7,QUEST_VALUE_STR_LEN ,szQuestValueStr[6]);
	DECLARE_LUA_STRUCT_STRING(QuestValueStr8,QUEST_VALUE_STR_LEN ,szQuestValueStr[7]);
#endif

	DECLARE_LUA_STRUCT_INTEGER(HidePresent, bHidePresent);			//是否隐藏奖励

	DECLARE_LUA_STRUCT_INTEGER(PresentAll1, bPresentAll[0]);		//第一组(1-4)道具是否全给或者只给一件
	DECLARE_LUA_STRUCT_INTEGER(PresentAll2, bPresentAll[1]);		//第二组(5-8)道具是否全给或者只给一件
	
	DECLARE_LUA_STRUCT_DWORD(PresentItemType1, dwPresentItemType[0]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex1, dwPresentItemIndex[0]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount1, dwPresentItemAmount[0]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType2, dwPresentItemType[1]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex2, dwPresentItemIndex[1]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount2, dwPresentItemAmount[1]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType3, dwPresentItemType[2]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex3, dwPresentItemIndex[2]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount3, dwPresentItemAmount[2]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType4, dwPresentItemType[3]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex4, dwPresentItemIndex[3]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount4, dwPresentItemAmount[3]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType5, dwPresentItemType[4]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex5, dwPresentItemIndex[4]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount5, dwPresentItemAmount[4]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType6, dwPresentItemType[5]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex6, dwPresentItemIndex[5]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount6, dwPresentItemAmount[5]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType7, dwPresentItemType[6]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex7, dwPresentItemIndex[6]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount7, dwPresentItemAmount[6]);	//交任务时奖励道具的数量
	DECLARE_LUA_STRUCT_DWORD(PresentItemType8, dwPresentItemType[7]);		//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemIndex8, dwPresentItemIndex[7]);	//交任务时奖励道具的类型
	DECLARE_LUA_STRUCT_DWORD(PresentItemAmount8, dwPresentItemAmount[7]);	//交任务时奖励道具的数量

	DECLARE_LUA_STRUCT_INTEGER(PresentExp, nPresentExp);					//交任务时奖励的经验
	DECLARE_LUA_STRUCT_INTEGER(PresentMoney, nPresentMoney);				//交任务时奖励的金钱数量
	DECLARE_LUA_STRUCT_INTEGER(PresentSkill, nPresentSkill);				//交任务时的技能奖励

    DECLARE_LUA_STRUCT_INTEGER(PresentAssistThew, nPresentAssistThew);
    DECLARE_LUA_STRUCT_INTEGER(PresentAssistStamina, nPresentAssistStamina);
    DECLARE_LUA_STRUCT_INTEGER(PresentAssistFriendship, nPresentAssistFriendship);

	DECLARE_LUA_STRUCT_DWORD(AffectForceID1, dwAffectForceID[0]);			//交任务影响的势力
	DECLARE_LUA_STRUCT_INTEGER(AffectForceValue1, nAffectForceValue[0]);	//交任务影响的势力友好度
	DECLARE_LUA_STRUCT_DWORD(AffectForceID2, dwAffectForceID[1]);			//交任务影响的势力
	DECLARE_LUA_STRUCT_INTEGER(AffectForceValue2, nAffectForceValue[1]);	//交任务影响的势力友好度
	DECLARE_LUA_STRUCT_DWORD(AffectForceID3, dwAffectForceID[2]);			//交任务影响的势力
	DECLARE_LUA_STRUCT_INTEGER(AffectForceValue3, nAffectForceValue[2]);	//交任务影响的势力友好度
	DECLARE_LUA_STRUCT_DWORD(AffectForceID4, dwAffectForceID[3]);			//交任务影响的势力
	DECLARE_LUA_STRUCT_INTEGER(AffectForceValue4, nAffectForceValue[3]);	//交任务影响的势力友好度

	DECLARE_LUA_STRUCT_INTEGER(RepeatCutPercent, nRepeatCutPercent);		//重复做任务时奖励的削减百分比

    DECLARE_LUA_STRUCT_INTEGER(RequireCampMask, nRequireCampMask);

    DECLARE_LUA_STRUCT_INTEGER(PresentPrestige, nPresentPrestige);
    DECLARE_LUA_STRUCT_INTEGER(PresentContribution, nPresentContribution);
    DECLARE_LUA_STRUCT_INTEGER(PresentTrain, nPresentTrain);

	int LuaGetHortation(Lua_State* L);
};

class KQuestInfoList
{
public:
	BOOL Init();
	void UnInit();

	KQuestInfo* GetQuestInfo(DWORD dwQuestID);

	int GetNpcQuestString(DWORD dwMapID, DWORD dwNpcTemplateID, char* pszBuffer, size_t uBufferSize);
	BOOL GetNpcQuest(DWORD dwNpcTemplateID, std::vector<DWORD>* pvQuestID);
    BOOL IsNpcQuestExist(DWORD dwNpcTemplateID);

private:
	typedef std::map<DWORD, KQuestInfo> MAP_ID_2_QUEST_INFO;
	MAP_ID_2_QUEST_INFO					m_mapID2QuestInfo;

#ifdef _SERVER
	typedef std::multimap<DWORD, DWORD>	MAP_NPC_ID_2_QUEST_ID;
	MAP_NPC_ID_2_QUEST_ID				m_mapNpcID2QuestID;						// 保存掉落任务道具的NPC
#endif

	typedef std::multimap<int64_t, DWORD>	MAP_NPC_2_QUEST_INFO;
	MAP_NPC_2_QUEST_INFO				m_mapNpc2QuestInfo;						//保存起始与完成的NPC

	typedef std::multimap<DWORD, DWORD> MAP_DOODAD_ID_2_QUEST_ID;
	MAP_DOODAD_ID_2_QUEST_ID			m_mapDoodadID2QuestID;					//保存掉落任务道具的Doodad

	typedef std::multimap<DWORD, DWORD> MAP_RDOODAD_ID_2_QUEST_ID;
	MAP_RDOODAD_ID_2_QUEST_ID			m_mapRDoodadID2QuestID;					//保存关联Doodad

	KQuestInfo	m_DefaultQuestInfo;		//默认设定

    // 从配置文件中读取dwQuestID号的任务信息
    BOOL LoadQuestInfoByID(ITabFile* pTabFile, DWORD dwQuestID, KQuestInfo* pQuestInfo);
    // 从配置文件中读取第nIndex行的任务信息
	BOOL LoadQuestInfo(ITabFile* piTabFile, int nIndex, KQuestInfo* pQuestInfo);
    // 根据Quest信息填写Map信息
    BOOL MakeQuestMap(KQuestInfo* cpQuestInfo);

	friend class KDropCenter;
    friend class KDoodad;
};

#endif	//_KQUEST_INFO_LIST_H_
