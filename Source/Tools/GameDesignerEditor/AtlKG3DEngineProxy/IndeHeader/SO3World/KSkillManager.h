/************************************************************************/
/* 技能管理器															*/
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.10.18	Create												*/
/* Comment	 :															*/
/*		管理技能的全局数据，存储有所有技能所有等级的共有数据			*/
/*		技能数据分三个层次存储：										*/
/*			1 对于某个技能，和技能等级无关的数据						*/
/*			2 和技能等级相关，但是和具体玩家无关的数据					*/
/*			3 和具体玩家相关的技能数据									*/
/*		其中，1对应于KSkillInfo，2对应于KSkill，1和2都属于技能管理器管理*/
/*		而3对应于KPlayerSkill，属于玩家有效技能列表管理(KSkillList)		*/
/*		客户端和服务端的技能数据维护方式不同							*/
/*		服务端在内存中载入所有相关数据，在100M左右						*/
/*		客户端只载入全部的1类数据，2类数据做Cache处理，占用2M以下的内存	*/
/************************************************************************/
#ifndef _KSKILL_MANAGER_H_
#define _KSKILL_MANAGER_H_

#include <map>
#include "KLRUCacheMap.h"
#include "../../Source/Common/SO3World/Src/KSkill.h"

class   KCharacter;

#define MAX_ROW                     203
#define MAX_COL                     5

#define MAX_SKILLS_FOR_EACH_MASTER  256

// 为了策划方便，在Tab文件里面有些数值策划填的是字符串，程序需要读入后翻译成相应的数值
// 比如，技能的伤害类型等等，该结构是从表中读取的，未经转换的数值.
struct SKILL_TABLE_LINE 
{
    char  szSkillName[_NAME_LEN];
    DWORD dwSkillID;
    int   nSkillMark;
    DWORD dwMaxLevel;
    char  szKindType[_NAME_LEN];
    int   nRecipeType;
    int   nUIType;
    DWORD dwBelongKungfu;               // 所属武功
    DWORD dwBelongSchool;               // 所属门派
    char  szEffectType[_NAME_LEN];                  // 技能效果类型
    char  szCastMode[_NAME_LEN];
    DWORD dwWeaponRequest;
    BOOL  bCostAmmo;                    // 是否消耗弹药格子里的弹药
    char  szCostItemType[_NAME_LEN];    // 技能消耗指定物品所在的配置表
    int   nCostItemIndex;               // 技能消耗指定物品所在配置表里的索引值(Index)  
    DWORD dwMountRequestType;
    DWORD dwMountRequestDetail;
    DWORD dwMountRequestDetailLevel;
    DWORD dwMountType;
    BOOL  bIsMountable;
    BOOL  bIsPassiveSkill;
    BOOL  bIsChannelSkill;
    BOOL  bIsExpSkill;
    BOOL  bIsExactHit;                  // 是否必中(即不考虑命中率)
	BOOL  bIsInstinct;                  // 是否本能(即不用学就能用的技能)
    BOOL  bIsAutoTurn;                  // 是否需要自动朝向
    BOOL  bCheckBindBuff;               // 是否在技能检查的时候检查BindBuff
    BOOL  bCauseAbradeEquipment;        // 技能命中后,是否对技能释放者的武器和技能目标装备造成磨损
    BOOL  bCauseBeatBreak;              // 技能是否可以把别人的通道技打断
    BOOL  bCauseBeatBack;               // 技能是否可以把别人的吟唱/通道打退
    BOOL  bHasCriticalStrike;           // 技能是否考虑随即重击
    DWORD dwSkillEventMask1;
    DWORD dwSkillEventMask2;
    DWORD dwSkillCastFlag;
    DWORD dwCastMask;
    BOOL  bUseCastScript;
    BOOL  bUse3DObstacle;
    BOOL  bIgnorePositiveShield;        // 忽略正向效果屏蔽状态
    BOOL  bIgnoreNegativeShield;        // 忽略负向效果屏蔽状态
    
    BOOL  bNeedOutOfFight;
    int   nSelfMoveStateMask;
    BOOL  bSelfOnFear;

    BOOL  bBindCombatTag;               // 是否绑定冒血的动作标签，和逻辑无关。用于表现
	
    char  szSelfHorseStateRequest[_NAME_LEN];

    BOOL  bTargetTypePlayer;
    BOOL  bTargetTypeNpc;
    BOOL  bTargetRelationNone;
    BOOL  bTargetRelationSelf;
    BOOL  bTargetRelationAlly;
    BOOL  bTargetRelationEnemy;
    BOOL  bTargetRelationNeutrality;
    BOOL  bTargetRelationParty;

    int   nTargetMoveStateMask;
    BOOL  bTargetOnFear;

    char  szTargetHorseStateRequest[_NAME_LEN];

    char  szCommonSkillActiveMode[_NAME_LEN];
    BOOL  bAutoSelectTarget;
    int   nNPCWeaponType;
    DWORD dwMapBanMask;
    char  szScriptFile[MAX_PATH];
};


struct KKUNG_FU_EXP
{
    int nExpAdd;
    int nCostQiPoint;
};

struct KSKILL_MASTER_KEY 
{
    DWORD   dwMasterID;
    DWORD   dwSkillID;
    DWORD   dwSkillLevel;
};

struct KSKILL_ID_LEVEL 
{
    DWORD   dwSkillID;
    DWORD   dwSkillLevel;
};

inline bool operator < (const KSKILL_MASTER_KEY& crLv, const KSKILL_MASTER_KEY& crRv)
{
    if (crLv.dwMasterID != crRv.dwMasterID)
        return crLv.dwMasterID < crRv.dwMasterID;

    if (crLv.dwSkillID != crRv.dwSkillID)
        return crLv.dwSkillID < crRv.dwSkillID;

    return crLv.dwSkillLevel < crRv.dwSkillLevel;
}

struct KSKILL_LEARNING_INFO 
{
    DWORD   dwRequireExp;
    int     nRequirePlayerLevel;
    DWORD   dwReputationID;
    int     nReputationLevel;
    int     nPrice;
    int     nCostTrain;
};



class KSkillManager
{
public:
	KSkillManager(void);

	BOOL    Init(void);
	void    UnInit(void);

    KSkill* GetSkill_RAW(DWORD dwSkillID, DWORD dwSkillLevel);      // 不论是技能还是武功都可获得
    KSkill* GetSkill_RAW_S(DWORD dwSkillID, DWORD dwSkillLevel);    // 只能获得技能
    KSkill* GetSkill_K(DWORD dwSkillID, DWORD dwSkillLevel);        // 只能获得武功

    BOOL    GetSkillRecipeKey(KSKILL_RECIPE_KEY* pSkillRecipeKey, DWORD dwSkillID, DWORD dwSkillLevel, KCharacter* pCharacter);
    KSkill* GetSkill(const KSKILL_RECIPE_KEY& crSkillRecipeKey);    // 只能获得技能

    KSKILL_RECIPE_BASE_INFO* GetSkillRecipeBaseInfo(DWORD dwRecipeKey);

    KSKILL_EVENT*       GetEventInfo(int nID);

    int     GetDefenceCoefficient(int nRow, int nProbability);

    BOOL    GetMasterSkillList(DWORD dwMasterID, size_t uMaxArrayCount, size_t* puRetArrayCount, KSKILL_ID_LEVEL* pRetSkillListArray);
    KSKILL_LEARNING_INFO*   GetSkillLearningInfo(DWORD dwMasterID, DWORD dwSkillID, DWORD dwSkillLevel);

    BOOL    IsInstinct(DWORD dwSkillID, DWORD dwSkillLevel);
    BOOL    IsAutoTurn(DWORD dwSkillID, DWORD dwSkillLevel);
    BOOL    IsAccumulate(DWORD dwSkillID, DWORD dwSkillLevel);

    BOOL    IsCommonSkill(DWORD dwSkillID);                            // 检查是否为新手技能
    BOOL    WeaponMapCommonSkill(int nWeaponType, DWORD& rdwSkillID);  // 只对新手技能有效
    int     CommonSkillMapWeapon(DWORD dwSkillID);                     // 只对新手技能有效
    BOOL    WeaponMapBaseKungfu(int nWeaponType, DWORD& rdwKungfuID);

    BOOL    GetKungFuExp(DWORD dwKungFuID, DWORD dwKungFuLevel, KKUNG_FU_EXP** ppKungFuExp);

    #ifdef _SERVER
    BOOL    OpenVenation(KPlayer* pPlayer, int nVenationID);
    #endif

public:
#ifdef _SERVER
    DWORD m_dwBulletIDIndex;                                    // 子弹的ID从1开始, 0代表无效ID
#endif

    DWORD m_dwSkillSNGenerator;

    typedef std::map<DWORD, KSKILL_RECIPE_BASE_INFO>        KSKILL_RECIPE_BASE_INFO_MAP;
    KSKILL_RECIPE_BASE_INFO_MAP     m_SkillRecipeBaseInfoMap;

private:

    BOOL LoadSkillTableLine(ITabFile* pSkillFile, int nRow, SKILL_TABLE_LINE *pTableLineInfo, const SKILL_TABLE_LINE &crDefaultValue);


    BOOL TableStringToNumber(const char cszString[], int *pnRetCode);  
    BOOL TranslateTableInfo(KSKILL_BASE_INFO *pSkillBaseInfo,  const SKILL_TABLE_LINE &crTableLineInfo);

    BOOL LoadSkillDataFull(DWORD dwSkillID);

    void ClearnUp();

private:
    // - Skill ---------------------------------------------------------------------------------------------

    typedef std::map<DWORD, KSKILL_BASE_INFO>	            SKILL_BASE_INFO_LIST;
    SKILL_BASE_INFO_LIST            m_SkillBaseInfoList;

	typedef std::map<DWORD, KSkill*>	                    SKILL_LIST;
	SKILL_LIST	                    m_SkillList;

    typedef KLRUCahceMap<KSKILL_RECIPE_KEY, KSkill, KDefaultUnInitEraseFunc<KSKILL_RECIPE_KEY, KSkill> > KSKILL_RECIPE_CACHE;
    KSKILL_RECIPE_CACHE             m_SkillRecipeCache;

    // - Skill Event ---------------------------------------------------------------------------------------

    typedef std::map<int, KSKILL_EVENT> KSKILL_EVENT_MAP;
    KSKILL_EVENT_MAP                m_SkillEventList;

    typedef std::map<KSKILL_MASTER_KEY, KSKILL_LEARNING_INFO> KSKILL_LEARNING_INFO_MAP;
    KSKILL_LEARNING_INFO_MAP        m_SkillLearningInfoMap;

    struct KSKILL 
    {
        DWORD dwCommonSkillID;
        DWORD dwBaseKungfuID;
    };

    typedef std::map<int, KSKILL> KWEAPON_MAP_SKILL;
    KWEAPON_MAP_SKILL m_WeaponMapSkill;

    typedef std::map<uint64_t, KKUNG_FU_EXP> KKUNG_FU_EXP_LIST;
    KKUNG_FU_EXP_LIST m_KungFuExpList;

    int m_nDefenceCoefficient[MAX_ROW][MAX_COL];

    BOOL LoadAttribute(ITabFile *piTabFile, int nRow, int nIndex, int &rnKey, int &rnValueA, int &rnValueB);

    BOOL LoadSkillEvent();
    BOOL LoadEventLine(ITabFile* piTabFile, int nLine);
    
    BOOL LoadWeaponMapSkill();
    BOOL LoadWeaponMapSkillLine(ITabFile* piTabFile, int nLine);    

    BOOL LoadDefenceCoefficientTable(const char* szTabFile);

    BOOL LoadKungFuExpTable(const char* szTabFile);
    BOOL LoadSkillRealizationTable(const char* szTabFile);
    BOOL LoadSubSkillLearningTable(DWORD dwMasterID, const char* szTabFile);
    BOOL LoadSkillLearningTable(const char* szTabFile);

    BOOL LoadSkillRecipeBaseLine(ITabFile* piTabFile, int nIndex, DWORD* pdwRecipeKey, KSKILL_RECIPE_BASE_INFO* pValue, const KSKILL_RECIPE_BASE_INFO& crDefault);
    BOOL LoadSkillRecipeBaseInfo(const char* szTabFile);

    // --------------------------- 错误检查 -------------------------
    BOOL CheckError();
    BOOL CheckSkillParam(KSkill* pSkill);
    BOOL CheckKungfuParam(KSkill* pSkill);
    BOOL CheckMountRequest(KSkill* pSkill);
    BOOL CheckEvent();

private:
    int m_nCacheHitRatio;
    int m_nCacheMissRatio;
    int m_nCacheHitCount;
    int m_nCacheMissCount;
};
#endif	//_KSKILL_MANAGER_H_

