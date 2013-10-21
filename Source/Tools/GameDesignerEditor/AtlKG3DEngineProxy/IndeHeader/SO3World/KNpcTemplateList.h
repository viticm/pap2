/************************************************************************/
/* 场景装饰物模板列表					                            */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.04.25	Create												*/
/* Comment	 :															*/
/*		封装了对NPC模板配置文件的读写和Cache						*/
/************************************************************************/
#ifndef _KNPC_TEMPLATE_LIST_
#define _KNPC_TEMPLATE_LIST_

#include "Global.h"
#include "Luna.h"

#include <map>

#define MAX_NPC_REPRESENT_ID_COUNT 10
#define MAX_NPC_DIALOG_COUNT 3			//Npc身上的随机对话数量
#define MAX_NPC_DIALOG_LEN	1024
#define MAX_OPTION_TEXT_LEN	128
#define MAX_NPC_REPUTATION	4			//每个Npc可以影响的声望数量

class KNpc;

struct KNpcTemplate
{
	DWORD						dwTemplateID;
	char						szName[_NAME_LEN];		    // 角色名
	char						szTitle[TITLE_LEN];			// 称号
	char						szMapName[_NAME_LEN];		// 所属地图
	int							nKind;					    // 见CHARACTER_KIND
	DWORD						dwForceID;				    // 所属势力
    int                         nSpecies;                   // NPC物种，见KNPC_SPECIES定义
	int							nLevel;					    // 角色基准等级
	int							nAdjustLevel;				// 角色等级的生成模式，0不浮动，1向上，2向下，3上下浮动
	int							nHeight;					// Npc高度
	MOVE_MODE					eMoveMode;					// Npc的移动模式				

    int                         nTouchRange;                //受击范围，触及范围，类似于物体半径
    int                         nIntensity;                 //强度
    int                         nReviveTime;                //重生时间

	BOOL						bSelectable;				// 能否点选
	BOOL						bCanSeeLifeBar;				// 能否显示血条
    BOOL                        bCanSeeName;                // 是否显示名字

	int							nMaxLife;				    // 最大生命值
	int							nLifeReplenish;			    // 生命点数回复速度
	int							nLifeReplenishPercent;	    // 生命百分比回复速度

	int							nMaxMana;				    // 最大内力值
	int							nManaReplenish;			
	int							nManaReplenishPercent;	

    int                         nWalkSpeed;
    int                         nRunSpeed;
    int                         nJumpSpeed;

    int                         nMeleeWeaponDamageBase;     
    int                         nMeleeWeaponDamageRand;     
    int                         nRangeWeaponDamageBase;
    int                         nRangeWeaponDamageRand;

	int							nDodge;			            //闪避点数
    int                         nParry;                     //招架几率
    int                         nParryValue;                //招架值，和减免伤害相关
    int                         nSense;                     //洞察值
    int                         nHitBase;                   //命中基础值

    int                         nPhysicsCriticalStrike;     //外功重击概率 
    int                         nSolarCriticalStrike;       //阳性内功重击概率
    int                         nNeutralCriticalStrike;     //中性内功重击概率
    int                         nLunarCriticalStrike;       //阴性内功重击概率
    int                         nPoisonCriticalStrike;      //毒性内功重击概率

    int							nPhysicsShieldBase;         //外功防御力
    int							nSolarMagicDefence;			//阳性内功防御系数
    int							nNeutralMagicDefence;		//中性内功防御系数
    int							nLunarMagicDefence;			//阴性内功防御系数
    int							nPoisonMagicDefence;		//毒性内功防御系数

	int							nAIType;					//AI类型
	int							nAIParamTemplateID;			//AI参数模板

	int							nCorpseDoodadID;			//尸体模板ID
	int							nDropClassID;				//npc分类掉落中使用		

	DWORD						dwRepresentID[MAX_NPC_REPRESENT_ID_COUNT];
	int							nNpcDialogID;				//Npc说话用的ID,对应表SmartDialog.tab

    DWORD                       dwImmunityMask;             // Boss属性 免疫掩码

#if defined(_SERVER)
    BOOL                        bDropNotQuestItemFlag;      // 表示这个NPC帮玩家打其他NPC,其他NPC是否要掉落非任务道具
	char						szDropName[MAX_DROP_PER_NPC][MAX_PATH];//掉落表文件名
	int							nDropCount[MAX_DROP_PER_NPC];

	DWORD						dwSkillIDList[MAX_NPC_AI_SKILL];		//AI技能ID
	DWORD						dwSkillLevelList[MAX_NPC_AI_SKILL];		//AI技能等级
	int							nSkillCastInterval[MAX_NPC_AI_SKILL];	//AI技能间隔
	int							nAISkillType[MAX_NPC_AI_SKILL];			//AI技能类型
	int							nSkillRate[MAX_NPC_AI_SKILL];			//AI技能释放的概率
    int							nSkillAniFrame[MAX_NPC_AI_SKILL];       //AI技能动作帧数
    int							nSkillRestFrame[MAX_NPC_AI_SKILL];      //AI技能停止帧数

	int							nThreatTime;				            //仇恨消退间隔
	int							nOverThreatPercent;
    int                         nPursuitRange;                          //追击距离

	//------  怪物掉钱  ------------------------------------------------->
    int                         nExp;
	int							nMinMoney;
	int							nMaxMoney;
	int							nMoneyRate;
	DWORD						dwReputeID[MAX_NPC_REPUTATION];
	int							nReputeValue[MAX_NPC_REPUTATION];
	int							nReputeLowLimit[MAX_NPC_REPUTATION];
	int							nReputeHighLimit[MAX_NPC_REPUTATION];



	//<-------------------------------------------------------------------

	//------  OpenWindow ------------------------------------------------->
	char						szDialog[MAX_NPC_DIALOG_COUNT][MAX_NPC_DIALOG_LEN];
	int							nDialogRate[MAX_NPC_DIALOG_COUNT];
	int							nTotalRate;
	char						szDialogAfterQuest[MAX_NPC_DIALOG_LEN];
	DWORD						dwDialogQuestID;

    int 						nShopTemplateID;
	DWORD                       dwMasterID;
	DWORD						dwCraftMasterID;
	BOOL						bHasBank;
	BOOL						bHasMailBox;
    BOOL                        bHasAuction;

	char						szShopOptionText[MAX_OPTION_TEXT_LEN];
	char						szMasterOptionText[MAX_OPTION_TEXT_LEN];
	char						szCraftMasterOptionText[MAX_OPTION_TEXT_LEN];
	char						szBankOptionText[MAX_OPTION_TEXT_LEN];
	char						szMailBoxOptionText[MAX_OPTION_TEXT_LEN];
    char                        szAuctionOptionText[MAX_OPTION_TEXT_LEN];

    // OpenWindows 时各个模块对声望等级的需求
    int                         nShopRequireReputeLevel;
    int                         nMasterRequireReputeLevel;
    int                         nCraftMasterRequireReputeLevel;
    int                         nBankRequireReputeLevel;
    int                         nMailBoxRequireReputeLevel;
    int                         nQuestRequireReputeLevel;
    int                         nAuctionRequireReputeLevel;

	//<---------------------------------------------------------------------
#endif	//_SERVER
    int                         nDailyQuestCycle;
    int                         nDailyQuestOffset;
    BOOL                        bIsRandomDailyQuest;

	char						szScriptName[MAX_SCIRPT_NAME_LEN];
    
#ifdef _SERVER
    int                         nProgressID;
    int                         nCampLootPrestige; // 阵营掩码, 0,1,2位分别表示对应阵营能否加威望
    int                         nPrestige;
    int                         nContribution;
    int                         nAchievementID;     // 击杀后给的成就ID
#endif

	DECLARE_LUA_CLASS(KNpcTemplate);

	DECLARE_LUA_STRUCT_DWORD(TemplateID, dwTemplateID);
	DECLARE_LUA_STRUCT_STRING(Name, _NAME_LEN, szName);
	DECLARE_LUA_STRUCT_INTEGER(Kind, nKind);		
	DECLARE_LUA_STRUCT_DWORD(ForceID, dwForceID);
	DECLARE_LUA_STRUCT_INTEGER(Species, nSpecies);
	DECLARE_LUA_STRUCT_INTEGER(Level, nLevel);	
	DECLARE_LUA_STRUCT_INTEGER(Height, nHeight);
};

class KNpcTemplateList
{
public:
	KNpcTemplateList(void);
	~KNpcTemplateList(void);

	BOOL Init(void);
	BOOL UnInit(void);

	BOOL LoadFromTemplate(DWORD dwNpcTemplateID, KNpc* pNpc);

	KNpcTemplate* GetTemplate(DWORD dwNpcTemplateID);

	KNpcTemplate* GetTemplateByIndex(int nIndex);

	int GetNpcTemplateCount();

private:
	typedef std::map<DWORD, KNpcTemplate*> KNPC_TEMPLATE_LIST;
	KNPC_TEMPLATE_LIST	m_mapNpcTemplateList;
	KNpcTemplate		m_DefaultNpcTemplate;
	KNpcTemplate*		m_pNpcTemplateList;
	int					m_nNpcTemplateCount;

	BOOL LoadNpcTemplate(int nIndex, ITabFile* piTabFile, KNpcTemplate& fNpcTemplate);
};

// Npc死后给出的经验由其强度和经验查表决定


#endif	//_KNPC_TEMPLATE_LIST_
