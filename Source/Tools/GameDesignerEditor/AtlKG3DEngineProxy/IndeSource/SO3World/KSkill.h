/************************************************************************/
/* Skill								                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.14	Create												*/
/************************************************************************/

#ifndef _KSKILL_H_
#define _KSKILL_H_

#include "Global.h"
#include "KBaseObject.h"
#include "SO3Result.h"
#include "list"
#include "KTarget.h"
#include "KMath.h"
#include "KBuff.h"

class   KAttribute;
class   KRegion;
struct  KCell;
class   ITabFile;
struct  KTarget;
class   KSceneObject;
class   KCharacter;
class   KNpc;
class   KDoodad;
class   KPlayer;
class   KSkill;

#define MAX_SKILL_COOL_DOWN_TIMER       3
#define MAX_SKILL_BIND_BUFF_COUNT       4
#define PHYSICS_AP_TO_DPS               10
#define MAGIC_AP_TO_DPS                 12
#define THERAPY_AP_TO_DPS               12

#define WEAK_MINOR                      40
#define WEAK_MAJOR                      80
#define WEAK_MAX                        100

#define MAX_SKILL_REICPE_COUNT          4
#define MAX_BUFF_REICPE_COUNT           1

#define MAX_ACCUMULATE_VALUE            10                          // 聚气的最小单元时间为0.5秒
#define MAX_ACCUMULATE_SUB_COUNT        MAX_ACCUMULATE_VALUE + 1    // 聚气分10 + 1(蓄满以后)个单元

#define MAX_CHAIN_TARGET                21

#define SAME_SCHOOL_KUNGFU_CD_INDEX     0
#define OTHER_SCHOOL_KUNGFU_CD_INDEX    1


// 该枚举描述了技能的作用效果类型
enum KSKILL_EFFECT_TYPE
{
    setInvalid,
    setSkill,
    setBuff,
    setTotal
};

// 技能分类，按技能应用：ApplyType
enum KSKILL_APPLY_TYPE
{
    satInvalid = 0,

    satPlayerSkill,     // 主角技能
    satNpcSkill,        // Npc技能
    satGearSkill,       // 装备技能
    satItemSkill,       // Item技能
    satSystemSkill,     // 系统技能
    satMissionSkill,    // 任务相关技能
    satTestSkill,       // 测试技能

    satTotal
};

// 技能分类，按技能类型：KindType
enum KSKILL_KIND_TYPE
{
    sktInvalid = 0,

    sktPhysics,         // 外功类型
    sktSolarMagic,      // 阳性内功类型
    sktNeutralMagic,    // 中性内功类型
    sktLunarMagic,      // 阴性内功类型
    sktPoison,          // 毒属性
    sktNone,            // 无属性

    sktTotal
};

// 技能分类，按技能功能性质：FunctionType
enum KSKILL_FUNCTION_TYPE
{
    sftInvalid = 0,

    sftNormal,          // 普通
    sftSlow,            // 迟缓
    sftFear,            // 恐惧
    sftHalt,            // 定身
    sftSilence,         // 禁言
    sftChaos,           // 混乱
    sftCharm,           // 魅惑
    sftStun,            // 击晕
    sftEnmity,          // 仇恨
    sftBoolding,        // 流血
    sftDaze,            // 眩晕

    sftDamage,          // 伤害

    sftTotal
};

// 技能分类，按门派流派：GenreType
enum KSILL_GENRE_TYPE
{
    sgtInvalid = 0,

    sgtTotal
};

// 技能对骑马状态的要求
enum SKILL_HORSE_STATE_CONDITION
{
    shcInvalid = 0,
    shcAny,
    shcHorse,
    shcNotHorse,
    shcTotal
};


// 技能的释放方式
enum KSKILL_CAST_MODE
{
    scmInvalid,
    scmSector,          // 扇形范围,扇心固定在自己
    scmCasterArea,      // 以自己为中心的圆形区域
    scmTargetArea,      // 以目标为中心的圆形区域
    scmPointArea,
    scmCasterSingle,    // 对单体对象(限于自己)施放
    scmTargetSingle,    // 对单体对象(指定目标)施放
    scmPoint,           // 对坐标点施放
	scmItem,			// 对道具释放
    scmTargetChain,     // 对目标释放链状技能
    scmTargetLeader,    // 对阵眼释放技能
    scmPartyArea,       // 小队的范围技能
    scmTotal
};

enum KSKILL_WEAPON_REQUEST_TYPE
{
    swrtInvalid = 0,

    swrtMeleeWeapon,
    swrtRangeWeapon,

    swrtTotal
};

enum KSKILL_WEAK_POS
{
    swpInvalid = 0,

    swpUpper,
    swpMiddle,
    swpLower,
    swpNone,

    swpTotal
};

enum KSKILL_WEAK_LEVEL
{
    swlNone = 0,
    swlMinor,
    selMajor,
    selMax
};

enum KSKILL_COMMON_ACTIVE_MODE
{
    scamInvalid = 0,

    scamNone,
    scamMelee,
    scamRange,

    scamTotal
};

enum KSKILL_CAST_EFFECT_TYPE
{
    scetInvalid = 0,

    scetHarmful,
    scetNeutral,
    scetBeneficial,

    scetTotal
};

#define WEAK_VALUE_TO_LEVEL(WeakLevel, WeakValue)                       \
    do                                                                  \
    {                                                                   \
        if ((WeakValue) == 0)                                           \
            (WeakLevel) = swlNone;                                      \
        else if ((WeakValue) > 0 && (WeakValue) <= WEAK_MINOR)          \
            (WeakLevel) = swlMinor;                                     \
        else if ((WeakValue) > WEAK_MINOR && (WeakValue) <= WEAK_MAJOR) \
            (WeakLevel) = selMajor;                                     \
        else if ((WeakValue) > WEAK_MAJOR)                              \
            (WeakLevel) = selMax;                                       \
    } while(false)

//////////////////////////////////////////////////////////////////////////
#define MAKE_RECIPE_KEY(recipekey, recipeid, recipelevel)               \
    do                                                                  \
    {                                                                   \
        DWORD dwID    = (DWORD)(recipeid);                              \
        DWORD dwLevel = (DWORD)(recipelevel);                           \
        assert(dwID    <= USHRT_MAX);                                   \
        assert(dwLevel <= USHRT_MAX);                                   \
        (recipekey) =                                                   \
            (dwID << (sizeof(unsigned short) * CHAR_BIT)) |             \
            (dwLevel & USHRT_MAX);                                      \
    } while(false)

#define GET_RECIPE_ID_OF_KEY(recipeid, recipekey)                       \
    do                                                                  \
    {                                                                   \
        DWORD dwKey = (recipekey);                                      \
        (recipeid) = (dwKey >> (sizeof(unsigned short) * CHAR_BIT));    \
    } while(false)

#define GET_RECIPE_LEVEL_OF_KEY(recipelevel, recipekey)                 \
    do                                                                  \
    {                                                                   \
        DWORD dwKey = (recipekey);                                      \
        (recipelevel) = (dwKey & USHRT_MAX);                            \
    } while(false)


struct KSKILL_RECIPE_KEY
{
    DWORD   dwSkillID;
    DWORD   dwSkillLevel;
    DWORD   dwRecipeKey[MAX_SKILL_REICPE_COUNT];
};

bool operator<(const KSKILL_RECIPE_KEY& lhs, const KSKILL_RECIPE_KEY& rhs);
bool operator==(const KSKILL_RECIPE_KEY& lhs, const KSKILL_RECIPE_KEY& rhs);

enum KSKILL_RECIPE_RESULT_CODE 
{
    srrInvalid = 0,

    srrSuccess,
    srrFailed,
    srrErrorInFight,

    srrTotal
};

//////////////////////////////////////////////////////////////////////////

struct KSKILL_RECIPE_BASE_INFO
{
    BOOL        bIsSystemRecipe;

    DWORD       dwSkillID;
    DWORD       dwSkillLevel;

    int         nSkillRecipeType;

    int         nPrepareFramesAdd;
    int         nPrepareFramesPercent;

    int         nCoolDownAdd[MAX_SKILL_COOL_DOWN_TIMER];

    int         nMinRadiusAdd;
    int         nMaxRadiusAdd;

    int         nCostLifeAddPercent;
    int         nCostManaAddPercent;
    int         nCostStaminaAdd;

    int         nDamageAddPercent;

    char	    szScriptFile[MAX_PATH];	// 秘笈对技能的影响脚本文件。策划可以在脚本中添加魔法属性等等。
                                        // 所有在秘笈脚本中的魔法属性添加在属性链表的前端。
};

//////////////////////////////////////////////////////////////////////////

// Buff检查链表
enum KBUFF_COMPARE_FLAG
{
    bcfInvailed,

    bcfEqual,
    bcfGreaterEqual,

    bcfTotal
};

struct KSKILL_REQUIRE_BUFF_NODE
{
    DWORD               dwBuffID;
    int                 nStackNum;
    KBUFF_COMPARE_FLAG  eStackCompareFlag;
    int                 nBuffLevel;
    KBUFF_COMPARE_FLAG  eLevelCompareFlag;
};

// 蓄气技子技能释放信息

struct KSKILL_ACCUMULATE
{
    DWORD   dwSubSkillID;
    DWORD   dwSubSkillLevel;
};

// 链状技能中目标的结构
struct KSKILL_CHAIN_TARGET
{
    KSKILL_CHAIN_TARGET() {};
    KSKILL_CHAIN_TARGET(KCharacter* pTargetCharacter, int nTargetDistance) 
                    : pCharacter(pTargetCharacter), nDistance(nTargetDistance) {};

    KCharacter* pCharacter;
    int         nDistance;
};

// 1. 子弹是记录在发技能者身上的
// 2. 一个子弹可以作用多个人
// 3. 发技能者自己死亡后子弹并不清除
// 4. 发技能者自己换地图时要清除所有子弹

struct KUSER_SKILL_EVENT;

class KSkillRecipePointer 
{
public:
    KSkill*             SetRecipeKey(const KSKILL_RECIPE_KEY& crRecipeKey);
    KSKILL_RECIPE_KEY&  GetRecipeKey();
    void                ClearRecipeKey();
    KSkill*             GetPointer();

private:
    KSkill*             m_pSkill;
    DWORD               m_dwSkillSN;
    KSKILL_RECIPE_KEY   m_SkillRecipeKey;
};

struct KSKILL_BULLET
{
    DWORD       dwBulletID;             // 子弹ID，生成子弹的时候，系统为每个子弹分配一个唯一的ID

    KSkillRecipePointer SkillRecipePointer;   // 技能指针对象，
                                              // 使用前必须先比较结构体中记录的SN和pSkill所指对象的SN只否匹配，
                                              // 如不匹配则说明指针已经无效，必须用SkillRecipeKey重新GetSkill。
    
    KCharacter* pSkillSrc;
    DWORD       dwSkillSrcID;           // 技能释放者的ID
    int         nSkillSrcLevel;
    KTarget     Target;                 // 只可能是几种情况: NpcPointer, PlayerPointer, DoodadPointer, Coordination
    DWORD       dwTargetID;             // 当Target记录为指针的时候，需要验证ID
    int         nEndFrame;      

    int         nPhysicsHitValue;
    int         nSolarHitValue;
    int         nLunarHitValue;
    int         nNeutralHitValue;
    int         nPoisonHitValue;

    int         nCriticalStrike;        // 重击率

    std::vector<KUSER_SKILL_EVENT> SkillEventVector; // 技能释放者身上技能事件的拷贝
    
    int         nPhysicsDamage;         // 外功伤害, 包括技能本身的伤害与人物身上其他属性造成的附加伤害
    int         nSolarDamage;      
    int         nNeutralDamage;    
    int         nLunarDamage;      
    int         nPoisonDamage; 
    int         nTherapy;	    
    
    int         nDotPhysicsAP;          // 附加到Dot上的外功伤害, 不包括Dot本身的伤害
    int         nDotSolarAP;      
    int         nDotNeutralAP;    
    int         nDotLunarAP;      
    int         nDotPoisonAP; 
    int         nDotTherapyAP;	       

    int         nDamageToLifeForSelf;     // 把伤害转为自己的生命
    int         nDamageToManaForSelf;     // 把伤害转为自己的内力

    int         nBaseThreatCoefficient;
    int         nDamageThreatCoefficient;
    int         nTherapyThreatCoefficient;

    struct KSKILL_BULLET* pNext;
};


// 注意,武功在实现上也是一个特殊的技能,如果一个Skill的dwBelongKungfu为0,那就表示它实际上是个武功

struct KSKILL_BASE_INFO
{
	char	szSkillName[_NAME_LEN];
    DWORD	dwSkillID;
    int     nSkillMark;             // PQ贡献度统计用的施法分数
	DWORD	dwMaxLevel;				// 技能等级上限,基本等级合法范围 [1, dwMaxLevel]
    
    int     nApplyType;             // 技能分类，按技能应用：ApplyType
    int     nKindType;              // 技能分类，按技能类型：KindType 
    int     nRecipeType;            // 技能秘笈匹配分类
    // int     nFunctionType;          // 技能分类，按技能功能性质：FunctionType
    int     nGenreType;             // 技能分类，按门派流派：

    int     nUIType;                // 表示该技能属于内功还是外功还是其他...

    DWORD   dwBelongKungfu;         // 所属武功
    DWORD   dwBelongSchool;         // 所属门派

    int     nCastMode;              // KSKILL_CAST_MODE, 释放方式

    DWORD   dwWeaponRequest;
    BOOL    bCostAmmo;              // 是否消耗弹药格子里的弹药
    int     nCostItemType;          // 技能消耗指定物品所在的配置表
    int     nCostItemIndex;         // 技能消耗指定物品所在配置表里的索引值(Index)
    DWORD   dwMountRequestType;     // 技能需要的内功类型，只有在不需要内功的时候才开始判断这个类型，一般和门派有关
    DWORD   dwMountRequestDetail;   // 技能需要的内功，指定一个内功
    DWORD   dwMountRequestDetailLevel;  // 技能需要的内功等级
    DWORD   dwMountType;

    BOOL    bIsMountable;
	BOOL    bIsPassiveSkill;
    BOOL    bIsChannelSkill;
    BOOL    bIsExpSkill;
    BOOL    bIsExactHit;            // 是否必中(即不考虑命中率)
    BOOL    bIsInstinct;            // 是否本能(即不用学就能用的技能)
    BOOL    bIsAutoTurn;            // 是否需要自动朝向
    
    int     nEffectType;            // 技能效果，分为有益，有害，中性
    BOOL    bCheckBindBuff;

    BOOL    bCauseAbradeEquipment;  // 技能命中后,是否对技能释放者的武器和技能目标装备造成磨损
    BOOL    bCauseBeatBreak;        // 技能是否可以把别人的通道技打断
    BOOL    bCauseBeatBack;         // 技能是否可以把别人的吟唱/通道打退
    BOOL    bHasCriticalStrike;     // 技能是否考虑随即重击

    DWORD   dwSkillEventMask1;
    DWORD   dwSkillEventMask2;
    DWORD   dwSkillCastFlag;
    DWORD   dwCastMask;             // 技能释放分类，用作某几类技能是否受人物身上特定属性影响

    BOOL    bUseCastScript;
    BOOL    bUse3DObstacle;

    BOOL    bIgnorePositiveShield;
    BOOL    bIgnoreNegativeShield;

    BOOL    bNeedOutOfFight;
    int     nSelfMoveStateMask;
    BOOL    bSelfOnFear;

    int     nSelfHorseStateRequest;
    
    BOOL    bTargetTypePlayer;
    BOOL    bTargetTypeNpc;
    int     nTargetRelationMask;

    BOOL    bBindCombatTag;         // 是否绑定冒血的动作标签，和逻辑无关。用于表现

    int     nTargetMoveStateMask;
    BOOL    bTargetOnFear;

    int     nTargetHorseStateRequest;

    int     nCommonSkillActiveMode;
    BOOL    bAutoSelectTarget;
    int     nNPCWeaponType;
    DWORD   dwMapBanMask;
	char	szScriptFile[MAX_PATH];	// 技能脚本文件
};

struct KSKILL_DISPLAY_INFO
{
    int     nCastTime;
    int     nCoolDown[MAX_SKILL_COOL_DOWN_TIMER];
    int     nAreaRadius;
    int     nMinRadius;
    int     nMaxRadius;
    int     nCostMana;
	int		nCostLife;

    int     nMinPhysicsDamage;
    int     nMaxPhysicsDamage;
    int     nPhysicsDamagePercent;

    int     nMinSolarDamage;
    int     nMaxSolarDamage;
    int     nSolarDamagePercent;

    int     nMinNeutralDamage;
    int     nMaxNeutralDamage;
    int     nNeutralDamagePercent;

    int     nMinLunarDamage;
    int     nMaxLunarDamage;
    int     nLunarDamagePercent;

    int     nMinPoisonDamage;
    int     nMaxPoisonDamage;
    int     nPoisonDamagePercent;

    int     nMinTherapy;
    int     nMaxTherapy;
    int     nTherapyPercent;

    int     nBuffCount[MAX_SKILL_BIND_BUFF_COUNT];
    int     nBuffIntervalFrame[MAX_SKILL_BIND_BUFF_COUNT];
    int     nBuffDamage[MAX_SKILL_BIND_BUFF_COUNT];
    int     nBuffTherapy[MAX_SKILL_BIND_BUFF_COUNT];
};

class KSkill
{
public:
    KSkill();
    ~KSkill();

    BOOL Init(KSKILL_BASE_INFO *pBaseInfo, DWORD dwLevel);
    void UnInit();

    BOOL CallInitializeScript();
    BOOL CallLevelUpScript(KPlayer* pPlayer);

	SKILL_RESULT_CODE CanCast(KCharacter *pSrcCharacter, KTarget &rTarget);
    SKILL_RESULT_CODE CanCastOnPrepare(KCharacter *pSrcCharacter, KTarget &rTarget);
	SKILL_RESULT_CODE Cast(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);


public:
    KSKILL_BASE_INFO*   m_pBaseInfo;			// 基础技能信息

    DWORD               m_dwSerialNumber;       // 技能对象初始化时确定的一个唯一标识。

	DWORD				m_dwLevel;		        // 技能等级

    // --------- 技能领悟 ------------------------------>
	DWORD				m_dwLevelUpExp;
    int 				m_nPlayerLevelLimit;    // 学得该技能等级限制 
    int                 m_nExpAddOdds;          // 技能熟练度增长概率

    // --------- 技能消耗 ------------------------------>
	int	                m_nCostLife;
	int	                m_nCostMana;
	int					m_nCostStamina;	

    // <--------- 技能消耗 ------------------------------

	int					m_nPrepareFrames;		// 吟唱帧数

	int					m_nThreatRevisePercent;	// 仇恨修正百分比

	int					m_nMinRadius;		    // 最小距离    
    int					m_nMaxRadius;		    // 最大距离
    // m_nAreaRadius是什么东西?
    // 考虑类似与仍手榴弹的技能,m_nMaxRadius表示了手榴弹能仍多远,而m_nAreaRadius则表示了手榴弹的爆炸半径
    int                 m_nAreaRadius;         

	int					m_nAngleRange;		    // 范围攻击的扇形伤害角度范围

    int                 m_nBulletVelocity;      // 子弹速度

    int                 m_nWeaponDamagePercent; // 对武器伤害值的加成
    
    int                 m_nBreakRate;           // 打断的概率
	int					m_nBrokenRate;		    // 被打断的概率(基数1024)

    int                 m_nBeatBackRate;        // 打退概率
    
    int                 m_nChannelFrame;        // 通道技能持续时间
    int                 m_nChannelInterval;     // 通道技间隔时间

    int                 m_nTargetCountLimit;    // 技能作用目标数量限制，主要用于范围技; (小于0 代表目标数量不限制)

    int                 m_nAttackWeakPos;

    int                 m_nDamageAddPercent;

    // 阵法相关
    BOOL                m_bIsFormationSkill;            // 如果为true，此技能只能由阵眼（队长）释放，当此项为false时，不考虑下面两个参数。
    int                 m_nFormationRange;              // 结阵的范围，以阵眼为圆心。
    int                 m_nLeastFormationPopulation;    // 结阵的范围的最少队员数（包括队长）

    int                 m_nDismountingRate;             // 打落下马率

    int                 m_nBaseThreat;                  // 技能产生的基础仇恨

    // 蓄气计相关
    BOOL                m_bIsAccumulate;
    KSKILL_ACCUMULATE   m_SubsectionSkill[MAX_ACCUMULATE_SUB_COUNT];

    // 链状技能相关
    int                 m_nChainDepth;
    int                 m_nChainBranch;
    
    int                 m_nDamageToLifeForParty;    // 把伤害转为小队的生命
    int                 m_nDamageToManaForParty;    // 把伤害转为小队的内力

    int                 m_nTargetLifePercentMax;    // 对目标血量的需求，技能需求目标血量区间的上限
    int                 m_nTargetLifePercentMin;    // 对目标血量的需求，技能需求目标血量区间的下限

    int                 m_nSelfLifePercentMax;      // 对自身血量的需求，技能需求自身血量区间的上限
    int                 m_nSelfLifePercentMin;      // 对自身血量的需求，技能需求自身血量区间的下限

	KAttribute*		    m_pAttrbuteEffectToSelfAndRollback;
	KAttribute*		    m_pAttrbuteEffectToSelfNotRollback;
	KAttribute*		    m_pAttrbuteEffectToDestAndRollback;
    KAttribute*		    m_pAttrbuteEffectToDestNotRollback;
    KAttribute*		    m_pAttrbuteEffectToDestOnMinorWeak;
    KAttribute*		    m_pAttrbuteEffectToDestOnMajorWeak;
    KAttribute*		    m_pAttrbuteEffectToDestOnMaxWeak;

    KBuffRecipePointer  m_BindBuffs[MAX_SKILL_BIND_BUFF_COUNT];

    typedef std::vector<KSKILL_REQUIRE_BUFF_NODE> KSKILL_REQUIRE_BUFF_VECTOR;
    KSKILL_REQUIRE_BUFF_VECTOR m_SelfRequireBuffVector;
    KSKILL_REQUIRE_BUFF_VECTOR m_DestRequireBuffVector;

    DWORD               m_dwPublicCoolDownID;
    DWORD               m_dwCoolDownID[MAX_SKILL_COOL_DOWN_TIMER];
    int                 m_nCoolDownAlter[MAX_SKILL_COOL_DOWN_TIMER];

public:

    BOOL IsAutoTurn();

    SKILL_RESULT_CODE CheckWeaponRequest(KPlayer* pCaster);
    SKILL_RESULT_CODE CheckMountRequest(KPlayer* pCaster);

    SKILL_RESULT_CODE CheckCoolDown(KPlayer* pCaster);
    BOOL              ResetPublicCooldown(KPlayer* pCaster);
    BOOL              ClearPublicCooldown(KPlayer* pCaster);
    BOOL              ResetNormalCooldown(KPlayer* pCaster);

	SKILL_RESULT_CODE CheckCost(KCharacter* pCaster);

#ifdef _SERVER
    void              DoCost(KCharacter* pCaster);
#endif

    SKILL_RESULT_CODE CheckSelfBuff(KCharacter* pCaster);
    SKILL_RESULT_CODE CheckTargetBuff(KTarget& rTarget);
    SKILL_RESULT_CODE CheckTargetType(KCharacter* pCaster, KTarget& rTarget);
    SKILL_RESULT_CODE CheckFormationRequire(KCharacter* pCaster);
    SKILL_RESULT_CODE CheckSelfState(KCharacter* pCaster);
    SKILL_RESULT_CODE CheckTargetState(KTarget& rTarget);
	SKILL_RESULT_CODE CheckTargetRange(KCharacter* pCaster, KTarget& rTarget, int nFixPlayerCastSkillRange = 0, int nFixNpcCastSkillRange = 0);
    SKILL_RESULT_CODE CheckTargetBuffList(KCharacter* pCaster, KTarget& rTarget);


    SKILL_RESULT_CODE CallCastScript(KCharacter* pCaster, SKILL_RESULT_CODE nPreResult);

    // 自动选择一个适合释放该技能的目标
    BOOL AutoSelectTarget(KCharacter* pCaster); 

public:
#ifdef _SERVER

	SKILL_RESULT_CODE CastOnSector(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnCasterArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnTargetArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnPointArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnCasterSingle(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnTargetSingle(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnPoint(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnTargetChain(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnTargetLeader(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey);
    SKILL_RESULT_CODE CastOnPartyArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey);

    BOOL SetupBullet(KSKILL_BULLET *pBullet, KCharacter* pCaster, KTarget &rTarget, const KSKILL_RECIPE_KEY& crRecipeKey);

    SKILL_RESULT_CODE ApplyOnSector(KSKILL_BULLET* pBullet);
    SKILL_RESULT_CODE ApplyOnArea(KSKILL_BULLET* pBullet, int nX, int nY);
    SKILL_RESULT_CODE ApplyOnSingle(KSKILL_BULLET* pBullet);
    SKILL_RESULT_CODE ApplyOnPoint(KSKILL_BULLET* pBullet);
    SKILL_RESULT_CODE ApplyOnChain(KSKILL_BULLET* pBullet);
    
#endif

public:
#ifdef _CLIENT
    int GetSkillDisplayInfo(KSKILL_DISPLAY_INFO* pSkillDisplayInfo);
#endif


public:
    //脚本接口
    DECLARE_LUA_CLASS(KSkill);

    DECLARE_LUA_STRUCT_STRING(SkillName,             sizeof(m_pBaseInfo->szSkillName),  (char *)m_pBaseInfo->szSkillName);
    DECLARE_LUA_STRUCT_DWORD(SkillID,                m_pBaseInfo->dwSkillID);
    DECLARE_LUA_STRUCT_DWORD(MaxLevel,               m_pBaseInfo->dwMaxLevel);
	//DECLARE_LUA_STRUCT_INTEGER(FunctionType,         m_pBaseInfo->nFunctionType);
    DECLARE_LUA_STRUCT_INTEGER(UIType,               m_pBaseInfo->nUIType);
    DECLARE_LUA_STRUCT_DWORD(BelongKungfu,           m_pBaseInfo->dwBelongKungfu);
    DECLARE_LUA_STRUCT_DWORD(BelongSchool,           m_pBaseInfo->dwBelongSchool);
    DECLARE_LUA_STRUCT_INTEGER(CastMode,             m_pBaseInfo->nCastMode);
    DECLARE_LUA_STRUCT_DWORD(WeaponRequest,          m_pBaseInfo->dwWeaponRequest);
    DECLARE_LUA_STRUCT_INTEGER(CostItemType,         m_pBaseInfo->nCostItemType);
    DECLARE_LUA_STRUCT_INTEGER(CostItemIndex,        m_pBaseInfo->nCostItemIndex);
    DECLARE_LUA_STRUCT_DWORD(MountRequestType,       m_pBaseInfo->dwMountRequestType);
    DECLARE_LUA_STRUCT_DWORD(MountRequestDetail,     m_pBaseInfo->dwMountRequestDetail);
    DECLARE_LUA_STRUCT_DWORD(MountRequestDetailLevel, m_pBaseInfo->dwMountRequestDetailLevel);
    DECLARE_LUA_STRUCT_DWORD(MountType,              m_pBaseInfo->dwMountType);
    DECLARE_LUA_STRUCT_BOOL(IsMountAble,             m_pBaseInfo->bIsMountable);
    DECLARE_LUA_STRUCT_BOOL(IsPassiveSkill,          m_pBaseInfo->bIsPassiveSkill);
    DECLARE_LUA_STRUCT_BOOL(IsChannelSkill,          m_pBaseInfo->bIsChannelSkill);
    DECLARE_LUA_STRUCT_BOOL(IsExpSkill,              m_pBaseInfo->bIsExpSkill);
    DECLARE_LUA_STRUCT_BOOL(IsExactHit,              m_pBaseInfo->bIsExactHit);
    DECLARE_LUA_STRUCT_BOOL(IsInstinct,              m_pBaseInfo->bIsInstinct);
    DECLARE_LUA_STRUCT_BOOL(IsAutoTurn,              m_pBaseInfo->bIsAutoTurn);
    DECLARE_LUA_STRUCT_INTEGER(EffectType,           m_pBaseInfo->nEffectType);
    DECLARE_LUA_STRUCT_BOOL(CauseAbradeEquipment,    m_pBaseInfo->bCauseAbradeEquipment);
    DECLARE_LUA_STRUCT_BOOL(UseCastScript,           m_pBaseInfo->bUseCastScript);
    
    DECLARE_LUA_DWORD(Level);
    DECLARE_LUA_DWORD(LevelUpExp);
    DECLARE_LUA_INTEGER(PlayerLevelLimit);
    DECLARE_LUA_INTEGER(ExpAddOdds);

    DECLARE_LUA_INTEGER(CostLife);
    DECLARE_LUA_INTEGER(CostMana);
    DECLARE_LUA_INTEGER(CostStamina);

    DECLARE_LUA_INTEGER(PrepareFrames);	

    DECLARE_LUA_INTEGER(ThreatRevisePercent);

    DECLARE_LUA_INTEGER(MinRadius);	
    DECLARE_LUA_INTEGER(MaxRadius);	
    DECLARE_LUA_INTEGER(AreaRadius);	

    DECLARE_LUA_INTEGER(AngleRange);		

    DECLARE_LUA_INTEGER(BulletVelocity);  
    DECLARE_LUA_INTEGER(WeaponDamagePercent);
    
    DECLARE_LUA_INTEGER(BreakRate);	
    DECLARE_LUA_INTEGER(BrokenRate);

    DECLARE_LUA_INTEGER(BeatBackRate);

    DECLARE_LUA_INTEGER(ChannelFrame);		
    DECLARE_LUA_INTEGER(ChannelInterval);	

    DECLARE_LUA_INTEGER(TargetCountLimit);

    DECLARE_LUA_INTEGER(AttackWeakPos);

    DECLARE_LUA_BOOL(IsFormationSkill);
    DECLARE_LUA_INTEGER(FormationRange);
    DECLARE_LUA_INTEGER(LeastFormationPopulation);

    DECLARE_LUA_BOOL(IsAccumulate);

    DECLARE_LUA_INTEGER(DismountingRate);
    DECLARE_LUA_INTEGER(BaseThreat);

    DECLARE_LUA_INTEGER(ChainBranch);
    DECLARE_LUA_INTEGER(ChainDepth);

    DECLARE_LUA_INTEGER(DamageToLifeForParty);
    DECLARE_LUA_INTEGER(DamageToManaForParty);

    DECLARE_LUA_INTEGER(TargetLifePercentMax);
    DECLARE_LUA_INTEGER(TargetLifePercentMin);

    DECLARE_LUA_INTEGER(SelfLifePercentMax);
    DECLARE_LUA_INTEGER(SelfLifePercentMin);

    int LuaAddAttribute(Lua_State* L);
    int LuaGetAttribute(Lua_State* L);

    int LuaAddSlowCheckSelfBuff(Lua_State* L);
    int LuaAddSlowCheckDestBuff(Lua_State* L);

    int LuaBindBuff(Lua_State* L);

    int LuaGetBuff(Lua_State* L);
    int LuaGetDebuff(Lua_State* L);
    int LuaGetDot(Lua_State* L);
    int LuaGetHot(Lua_State* L);

    int LuaSetBuffRecipe(Lua_State* L);
    int LuaSetDebuffRecipe(Lua_State* L);
    int LuaSetDotRecipe(Lua_State* L);
    int LuaSetHotRecipe(Lua_State* L);

    int LuaSetPublicCoolDown(Lua_State* L);
    int LuaSetNormalCoolDown(Lua_State* L);
    int LuaGetNormalCooldownCount(Lua_State* L);
    int LuaGetNormalCooldownID(Lua_State* L);

    int LuaSetSubsectionSkill(Lua_State* L);

#ifdef _CLIENT
    int LuaUITestCast(Lua_State* L);

    int LuaCheckWeaponRequest(Lua_State* L);
    int LuaCheckMountRequest(Lua_State* L);
    int LuaCheckCostItemRequest(Lua_State* L);
    int LuaCheckDistance(Lua_State* L);
    int LuaCheckAngle(Lua_State* L);
    int LuaCheckCasterState(Lua_State* L);
    int LuaCheckTargetState(Lua_State* L);

    int _LuaCheckCostItemRequest(KPlayer* pPlayer);
    int _LuaCheckDistance(KCharacter* pCaster, KTarget* pTarget);
    int _LuaCheckAngle(KCharacter* pCaster, KTarget* pTarget);
#endif
};

struct KSkillSectorTravFunc
{	
    KSKILL_BULLET* pBullet;
    int nLeftCount;
    BOOL CanApply(KTarget &rTarget);
    BOOL operator()(KCharacter *pCharacter);
	BOOL operator()(KDoodad *pDoodad);
};


struct KSkillAreaTravFunc
{
    KSKILL_BULLET*  pBullet;
    int             nLeftCount;
    BOOL            bTargetArea;
    int             nTargetType;  // 用来记录TargetArea开始作用的目标类型
    DWORD           dwTargetID;   // 用来记录TargetArea开始作用的目标ID
    BOOL            CanApply(KTarget &rTarget);
    BOOL            operator()(KCharacter *pCharacter);
	BOOL            operator()(KDoodad *pDoodad);
};

// --------------------- 技能事件 -------------------------------->

enum KSKILL_EVENT_TYPE
{
    seInvalid,
    seCast,
    seBeCast,
    seHit,
    seBeHit,
    seDodge,
    seBeDodge,
    seCriticalStrike,
    seBeCriticalStrike,
    seBlock,
    seBeBlock,
    seAccumulate,
    seHitOTAction,
    seBeHitOTAction,
    seKill,
    seBeKill,
    seOverHeal,
    seBeOverHeal,
    seMiss,
    seBeMiss,
    seTotal
};

// 技能事件的处理流程实际上就是按照预设的参数调用另外一个技能
// 设在A->B施放技能的过程中触发技能事件X
// 则下面的枚举对于技能事件处理流程来说,esttCaster指这个技能应该以A为目标,esttTarget指这个技能应该以B为目标
enum KEVENT_SKILL_CASTER_TARGET_TYPE
{
    escttInvalid,
    escttCaster,
    escttTarget,
    escttTotal
};

struct KSKILL_EVENT
{
    int     nEventType;
    int     nOdds;  // 触发概率,百分数, 1 / 1024.
    DWORD   dwEventMask1;
    DWORD   dwEventMask2;
    DWORD   dwSkillID;
    DWORD   dwSkillLevel;
    int     nCasterType;
    int     nTargetType;
};

struct KUSER_SKILL_EVENT
{
    DWORD           dwEventID;
    int             nRefCount;
    KSKILL_EVENT    SkillEvent;
};

// 为了与 AI 配合，技能的距离判断是用圆柱体的方式，而不是球体。
BOOL SkillInRange(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ, int nRange);
IN_RANGE_RESULT SkillInRange(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ, int nMinRange, int nMaxRange);

// <--------------------- 技能事件 --------------------------------

#endif	//_KSKILL_H_

