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
//#include "KDoodadTemplateList.h"
#include "KReturnPosList.h"
//#include "KDoodadClassList.h"
#include "KLevelUpList.h"
#include "KCoolDownList.h"
//#include "KWeaponTypeList.h"
#include "KRelationList.h"
#include "KFightFlagList.h"
//#include "KDoodadReliveList.h"
#include "KRevivePosList.h"
//#include "KCharacterActionList.h"
#include "KMonster.h"

#define MAX_DROP_LEVEL			16
#define MAX_DROP_CLASS			8
#define MAX_SPECIAL_DROP_LIST	2048

class KWorldSettings
{
public:
	KWorldSettings(void);
	~KWorldSettings(void);

	BOOL Init(void);
	BOOL UnInit(void);

	// 游戏世界参数,目前全部都是对象预分配的个数
	struct KWORLD_PARAM
	{
		int nSceneCount;
		int nRegionCount;
		int nCellCount;
		int nPlayerCount;
		int nNpcCount;
		int nDoodadCount;
		int nItemCount;
		int	nLootListCount;
		int nAttributeCount;
		int nThreatNodeCount;
        int nBulletCount;
        int nTradingBoxCount;
		int nNpcTeamCount;
	} m_WorldParam;

    struct ROBOT_PARAM
    {
        char szBishopIP[_NAME_LEN];
        int  nBishopPort;

        char szAccountNamePrefix[_NAME_LEN];
        int  nStartIndex;
        char szPassword[_NAME_LEN];
        
        ARRANGEMENT  nArrangement;
        int nRange;
        int nRunRate;

        int  nStartInterval;

        int  nPreLoadMap;
        int  nReconnect;
    }m_RobotParam;

	//返回点列表
	KReturnPosList		m_ReturnPosList;

	//地图相关参数
	KMapListFile		m_MapListFile;

	//Doodad模板表
	//KDoodadTemplateList m_DoodadTemplateList;

	//Doodad分类表
	//KDoodadClassList	m_DoodadClassList;

	//玩家升级数据表
	KLevelUpList		m_LevelUpList;

    KCoolDownList       m_CoolDownList;

    //KWeaponTypeList     m_WeaponTypeList;

	//阵营关系表
	KRelationList		m_RelationCampList;

	//势力关系表
	KRelationList		m_RelationForceList;

	//默认势力战斗开关与声望
	KFightFlagList		m_ForceDefaultList;

	//KCharacterActionList m_CharacterActionList;
private:
	BOOL LoadWorldParam();
    BOOL LoadRobotParam();
};

extern KWorldSettings	g_WorldSettings;

#endif	//_KSETTINGS_H_