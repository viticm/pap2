/************************************************************************/
/* 游戏世界配置文件列表					                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.04.25	Create												*/
/************************************************************************/
#ifndef _KSETTINGS_H_
#define _KSETTINGS_H_

#include "KMapListFile.h"
#include "KNpcTemplateList.h"
#include "KDoodadTemplateList.h"
#include "KQuestInfoList.h"
#include "KNpcClassList.h"
#include "KDoodadClassList.h"
#include "KPatrolPathList.h"
#include "KLevelUpList.h"
#include "KCoolDownList.h"
#include "KWeaponTypeList.h"
#include "KRelationList.h"
#include "KFightFlagList.h"
#include "KNpcOrderList.h"
#include "KNpcTeamList.h"
#include "KCharacterActionList.h"
#include "KSmartDialogList.h"
#include "KReputationLimit.h"
#include "KGWConstList.h"
#include "KGMList.h"
#include "KAchievementInfoList.h"

#define MAX_DROP_LEVEL			16
#define MAX_DROP_CLASS			8
#define MAX_SPECIAL_DROP_LIST	2048

#define KMAIL_COST              30

class KWorldSettings
{
public:
	BOOL Init(void);
	BOOL UnInit(void);

	KGWConstList		m_ConstList;

	//地图相关参数
	KMapListFile		m_MapListFile;

	//任务设定表
	KQuestInfoList		m_QuestInfoList;

	//Npc模板表
	KNpcTemplateList	m_NpcTemplateList;
	//Doodad模板表
	KDoodadTemplateList m_DoodadTemplateList;

	//Npc分类表
	KNpcClassList		m_NpcClassList;

	//Doodad分类表
	KDoodadClassList	m_DoodadClassList;

	//巡逻路径表
	KPatrolPathList		m_PatrolPathList;

	//玩家升级数据表
	KLevelUpList		m_LevelUpList;

    KCoolDownList       m_CoolDownList;

    KWeaponTypeList     m_WeaponTypeList;

	//阵营关系表
	KRelationList		m_RelationCampList;

	//势力关系表
	KRelationList		m_RelationForceList;

	//默认势力战斗开关与声望
	KFightFlagList		m_ForceDefaultList;

	//声望等级经验表
	KReputationLimit	m_ReputeLimit;

	//表情系统
	KCharacterActionList m_CharacterActionList;

	//Npc说话
	KSmartDialogList	m_SmartDialogList;

#ifdef _SERVER
    KGMList             m_GMList;
#endif

	KNpcOrderManager    m_OrderManager;
	KNpcTeamList		m_NpcTeamList;

    KAchievementInfoList m_AchievementInfoList;
};

#endif	//_KSETTINGS_H_
