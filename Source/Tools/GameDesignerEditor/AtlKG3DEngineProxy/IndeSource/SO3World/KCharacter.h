/************************************************************************/
/* 角色			                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.30	Create												*/
/* Comment   :															*/
/*		角色是玩家和NPC的共同基类										*/
/************************************************************************/

#ifndef _KCHARACTER_H_
#define	_KCHARACTER_H_

#include <list>
#include <bitset>
#include "Global.h"
#include "SO3Result.h"
#include "KAttribute.h"
#include "KSceneObject.h"
#include "KThreatList.h"
#include "KItem.h"
#include "KTarget.h"
#include "KSkill.h"
#include "KSkillDebug.h"
#include "KAIVM.h"
#include "KBuffList.h"

class KPatrolPath;
class KLootList;
class KItem;

// 根据ID判断是否是玩家，玩家ID的高4位为0
#define IS_NPC(ID)    ((ID) & 0xf0000000)
#define IS_PLAYER(ID) (!IS_NPC(ID))

#define MAX_HIT_VALUE               10000
#define MAX_PARTY_EFFECT_RANGE      (40 * 64)

#define GET_SWIM_WATER_LINE(nHeight) ((nHeight) * 7 / 10)
#define IS_UNDER_WATER(nWaterLine, nCharacterHeight) ((nWaterLine) >= (nCharacterHeight) * 9 / 10)

// 该结构用于接收技能运算的结果
struct KSKILL_CALCULATE_RESULT
{
    BOOL bShield;
    BOOL bHitTarget;
    BOOL bSkillDodge;
    BOOL bCriticalStrike;
    BOOL bImmunity;
    BOOL bParry;
    BOOL bInsight;

    int  nSkillEffectResult[serTotal];   
    
    BOOL bPhysicsBlock;
    BOOL bSolarMagicBlock;
    BOOL bNeutralMagicBlock;
    BOOL bLunarMagicBlock;
    BOOL bPoisonBlock;

    BOOL bBuffResist;
    BOOL bBuffImmunity;
    int  nBuffID;
    int  nBuffLevel;

    struct
    {
        KSKILL_EFFECT_TYPE  eType;
        DWORD               dwID;
        int                 nLevel;        
    } EffectSrc;
};

// -------------- Over time action ---------------------->
// 持续一段时间的行为

enum KOT_ACTION_TYPE
{
    otActionIdle,
    otActionSkillPrepare,
    otActionSkillChannel,
    otActionRecipePrepare,
    otActionPickPrepare,
	otActionPicking,
	otActionItemSkill,
	otActionCustomPrepare,
	otActionCustomChannel,
    otActionUseItemPrepare
};

struct KProfessionSkill
{
	KTarget Target;
	DWORD dwCraftID;
	DWORD dwRecipeID;
	DWORD dwSourceItemID;	//发起生活技能的道具ID,这个有可能是配方书,也有可能是一个消耗道具
};

struct KOT_ACTION_PARAM 
{
	KOT_ACTION_TYPE	    eActionType;
	union 
	{
        KSkillRecipePointer SkillRecipePointer;
		DWORD				dwDoodadID;
		DWORD			    dwItemID;
	} Data;
    KTarget		        Target;
	KProfessionSkill	ProfessionSkill;
	int			        nEndFrame;
    union
    {
        int             nNextActFrame;      // 通道技下用这个表示下一次作用时间
        int             nPrepareFrame;      // 吟唱技下用这个表示该技能本身吟唱时间 
    };

    int                 nBeatBackCount; // 吟唱/通道被打退的计数
	DWORD				dwScriptID;		// CustomOTAction会带一个脚本
	BOOL				bIsBreakable;	// 是否会被攻击打断

#ifdef _CLIENT 
    DWORD               dwEndAniID;     // 使用道具  
#endif
};

struct KAUTO_CAST_SKILL_PARAM
{
    KSkillRecipePointer SkillRecipePointer;
    BOOL                bMelee;
    int*                pnCastCycle;
    int                 nPreCastFrame;
};

// <-------------- Over time event ----------------------

// 角色走动,寻路的结果
enum KMOVE_RESULT
{
    mrInvalid,
    mrSucceed,      // 正常移动了
    mrOutOfMap,     // 到地图边缘了,无效的区域
    mrObstacle,     // 有障碍,无法前进
    mrDeleteSelf,   // 在客户端,进入同步范围之外,把自己删除了
    mrReachTarget,  // 已经到达目标
    mrTotal
};

enum KSKILL_HIT_RESULT
{
    shrInvalid = 0,

    shrHit,
    shrMiss,
    shrDodge,
    shrParry,
    shrInsight,
    shrCriticalStrike,

    shrTotal
};


class KCharacter : public KSceneObject
{
public:
	KCharacter();
	virtual ~KCharacter();

	BOOL Init();
	void UnInit();

	void Activate(void);

public:
	// 杂项
	char						m_szName[_NAME_LEN];		// 角色名
	char						m_szTitle[TITLE_LEN];		// 称号
	CHARACTER_GENDER	        m_eGender;
	CHARACTER_KIND				m_eKind;					// 角色类型
	DWORD						m_dwClassID;				// 分类ID
	CHARACTER_RACE				m_eRace;					// 角色种族
	DWORD						m_dwForceID;			    // 势力
	int							m_nLevel;					// 角色等级
	DWORD						m_dwCharacterAction;		// 表情状态
    DWORD			            m_dwModelID;                // 模型ID

#ifdef _SERVER
    // 有的NPC是玩家专有的,只向其Owner同步
    // 之所以放到Character里面,是为了代码简单,省得到处判断IS_NPC
    DWORD                       m_dwOwner;
#endif

	DECLARE_LUA_STRING(Name, _NAME_LEN);
	DECLARE_LUA_STRING(Title, TITLE_LEN);
	DECLARE_LUA_ENUM(Gender);
	DECLARE_LUA_ENUM(Kind);
	DECLARE_LUA_DWORD(ClassID);
	DECLARE_LUA_ENUM(Race);
	DECLARE_LUA_DWORD(ForceID);
	DECLARE_LUA_INTEGER(Level);
	DECLARE_LUA_DWORD(CharacterAction);
    DECLARE_LUA_DWORD(ModelID);

#ifdef _SERVER
	DECLARE_LUA_DWORD(Owner);
#endif

    // -------------- 移动相关 -------------------------------------------------->
	CHARACTER_MOVE_STATE		m_eMoveState;				// 移动状态

    BOOL                        m_bSlip;
    int                         m_nJumpLimitFrame;

    int                         m_nVelocityXY;  // 水平速度
    int                         m_nDirectionXY; // 水平速度方向
    int                         m_nVelocityZ;   // 垂直速度

	int							m_nDestX;					
	int							m_nDestY;		

    // 收敛速度,比如在Run时,瞬时速度会受到打滑,重力等影响而变化,最终速度收敛于这个值
    int                         m_nConvergenceSpeed;

    int                         m_nRunSpeed;
    int                         m_nRunSpeedBase;           
    int                         m_nMoveSpeedPercent;    

    int			                m_nCurrentGravity;		
	int			                m_nGravity;			       
    int			                m_nGravityBase;
    int			                m_nGravityPercent;

	int							m_nJumpCount;				// 跳跃计数器
	int							m_nMaxJumpCount;		    // 最大跳跃次数
    int                         m_nDropDefence;             // 抗摔伤系数, [0 - 1024]

    int                         m_nJumpSpeed;               // 跳跃的垂直初速度
    int                         m_nJumpSpeedBase;
    int                         m_nJumpSpeedPercent;

    int                         m_nDivingCount;
    int                         m_nDivingFrame;             // 最大潜水时间
    int                         m_nDivingFrameBase;
    int                         m_nDivingFramePercent; 

    int                         m_nWaterFlyAbility;         // 是否可以水上漂

    BOOL                        m_bTryPathState;            // 是否正在绕路过程之中
    int                         m_nTryPathSide;             // 正在往左还是往右绕路
    int                         m_nTryPathDestX;            // 绕路开始的时候的移动目标点X
    int                         m_nTryPathDestY;            // 绕路开始的时候的移动目标点Y

#ifdef _SERVER
    // 玩家或NPC释放某些要求自动朝向的技能时将此标记设为true，
    // 当玩家主动改变朝向时将此标记设为false，取消自动朝向。
    BOOL                        m_bAutoTurnFlag;
#endif

    // 自动飞行相关
    int m_nCurrentTrack;    // 当前移动轨迹。
    int m_nFromNode;        // 起始结点
    int m_nTargetCity;      // 目的城市

	DECLARE_LUA_ENUM(MoveState);

    DECLARE_LUA_INTEGER(VelocityXY);
    DECLARE_LUA_INTEGER(DirectionXY);
    DECLARE_LUA_INTEGER(VelocityZ);

	DECLARE_LUA_INTEGER(DestX);
	DECLARE_LUA_INTEGER(DestY);

    DECLARE_LUA_INTEGER(RunSpeed);
	DECLARE_LUA_INTEGER(RunSpeedBase);
	DECLARE_LUA_INTEGER(MoveSpeedPercent);
	
    DECLARE_LUA_INTEGER(MaxJumpCount);
    DECLARE_LUA_INTEGER(Gravity);
    DECLARE_LUA_INTEGER(GravityBase);
    DECLARE_LUA_INTEGER(GravityPercent);

    DECLARE_LUA_INTEGER(JumpSpeed);
    DECLARE_LUA_INTEGER(JumpSpeedBase);
    DECLARE_LUA_INTEGER(JumpSpeedPercent);

    DECLARE_LUA_INTEGER(DropDefence);

    DECLARE_LUA_INTEGER(DivingCount);         
    DECLARE_LUA_INTEGER(DivingFrame);         
    DECLARE_LUA_INTEGER(DivingFrameBase);
    DECLARE_LUA_INTEGER(DivingFramePercent); 

    DECLARE_LUA_INTEGER(WaterFlyAbility);

	// -------------- 潜能 ------------------------------------------------------>

	int							m_nCurrentStrength;			    // 力量：主要影响人物的外功伤害
	int                         m_nStrengthBase;
    int                         m_nStrengthBasePercentAdd;
    
    int							m_nCurrentAgility;				// 身法：主要影响人物的行动速度，攻击速度，命中率，闪避率。
	int                         m_nAgilityBase;
    int                         m_nAgilityBasePercentAdd;
	
    int							m_nCurrentVitality;				// 体质：主要影响人物的生命值。
	int                         m_nVitalityBase;
    int                         m_nVitalityBasePercentAdd;

    int							m_nCurrentSpirit;				// 根骨：主要影响人物的内力值，内功系伤害。
	int                         m_nSpiritBase;
    int                         m_nSpiritBasePercentAdd;
    
    int                         m_nCurrentSpunk;                // 精神: 主要影响人物的生命恢复速度和内力恢复速度.
    int                         m_nSpunkBase;
    int                         m_nSpunkBasePercentAdd;
    
	DECLARE_LUA_INTEGER(CurrentStrength);
    DECLARE_LUA_INTEGER(StrengthBase);
    DECLARE_LUA_INTEGER(StrengthBasePercentAdd);

	DECLARE_LUA_INTEGER(CurrentAgility);
    DECLARE_LUA_INTEGER(AgilityBase);
    DECLARE_LUA_INTEGER(AgilityBasePercentAdd);

    DECLARE_LUA_INTEGER(CurrentVitality);
    DECLARE_LUA_INTEGER(VitalityBase);
    DECLARE_LUA_INTEGER(VitalityBasePercentAdd);
	
    DECLARE_LUA_INTEGER(CurrentSpirit);
    DECLARE_LUA_INTEGER(SpiritBase);
    DECLARE_LUA_INTEGER(SpiritBasePercentAdd);

    DECLARE_LUA_INTEGER(CurrentSpunk);
    DECLARE_LUA_INTEGER(SpunkBase);
    DECLARE_LUA_INTEGER(SpunkBasePercentAdd);

	// --------------- 生命 ------------------------------------------------------>

    // m_nMaxLife = m_nMaxLifeBase * (1024 + m_nMaxLifePercentAdd) / 1024 + m_nMaxLiftAdditional;

    // 非战斗状态回血计算公式：
    // m_nLifeReplenish + (m_nLifeReplenish * m_nLifeReplenishCoefficient) + (m_nMaxLife * m_nLifeReplenishPercent) + m_nLifeReplenishExt
    //
    // 战斗状态回血计算公式：
    // (m_nLifeReplenish * m_nLifeReplenishCoefficient) + (m_nMaxLife * m_nLifeReplenishPercent) + m_nLifeReplenishExt

    int							m_nCurrentLife;				// 当前生命
	int							m_nMaxLife;					// 生命最大值    
    int                         m_nMaxLifeBase;             // 仅取决于职业,等级
	int							m_nMaxLifePercentAdd;	    // 生命最大值增加百分比
	int							m_nMaxLifeAdditional;		// 追加的生命最大值
	int							m_nLifeReplenish;			// 生命点数恢复速度 = m_nSpunkLifeReplenish + 其他(装备或BUFF加成等)
	int							m_nLifeReplenishPercent;	// 生命百分比恢复速度, 单位1/1024
    int                         m_nLifeReplenishCoefficient;// 生命点数恢复速度的系数，单位1/1024
    int                         m_nLifeReplenishExt;        // 附加生命恢复点数

	DECLARE_LUA_INTEGER(CurrentLife);
	DECLARE_LUA_INTEGER(MaxLife);
    DECLARE_LUA_INTEGER(MaxLifeBase);
	DECLARE_LUA_INTEGER(MaxLifePercentAdd);
	DECLARE_LUA_INTEGER(MaxLifeAdditional);
	DECLARE_LUA_INTEGER(LifeReplenish);
	DECLARE_LUA_INTEGER(LifeReplenishPercent);
    DECLARE_LUA_INTEGER(LifeReplenishCoefficient);
    DECLARE_LUA_INTEGER(LifeReplenishExt);
	

    // ---------------- 内力 ----------------------------------------------------->

	int							m_nCurrentMana;				
	int							m_nMaxMana;					
    int                         m_nMaxManaBase;             
	int							m_nMaxManaPercentAdd;		
	int							m_nMaxManaAdditional;		
	int							m_nManaReplenish;        // 回蓝点数 = m_nSpunkManareplenish + 其他(装备或BUFF加成等)
	int							m_nManaReplenishPercent;	
    int                         m_nManaReplenishCoefficient;
    int                         m_nManaReplenishExt;

	DECLARE_LUA_INTEGER(CurrentMana);
	DECLARE_LUA_INTEGER(MaxMana);
    DECLARE_LUA_INTEGER(MaxManaBase);
	DECLARE_LUA_INTEGER(MaxManaPercentAdd);
	DECLARE_LUA_INTEGER(MaxManaAdditional);
	DECLARE_LUA_INTEGER(ManaReplenish);
	DECLARE_LUA_INTEGER(ManaReplenishPercent);
    DECLARE_LUA_INTEGER(ManaReplenishCoefficient);
    DECLARE_LUA_INTEGER(ManaReplenishExt);

    // --------------- 战斗杂项 --------------------------------------------------->
	int							m_nDodge;			        // 闪避点数 = m_Dodege
    int                         m_nDodgeBaseRate;
    BOOL                        m_bFightState;
    BOOL                        m_bSheathFlag;              // 武器位置标记,true表示不在手上,false表示在手上
    BOOL                        m_bSystemShield;            // 系统无敌盾
    int                         m_nPositiveShield;          // 正面效果屏蔽,屏蔽所有正向效果(如加血,Buff)技能
    int                         m_nNegativeShield;          // 负面效果屏蔽,屏蔽所有负向效果(如伤害, Debuff)技能
    int                         m_nGlobalBlock;             // 无类型格挡
    int                         m_nGlobalDamageAbsorb;      // 无类型限制的吸收伤害
	int							m_nGlobalDamageManaShield;  // 无类型限制的内力抵消伤害
    int                         m_nDamageToLifeForSelf;     // 把伤害转为自己的生命
    int                         m_nDamageToManaForSelf;     // 把伤害转为自己的内力
    int                         m_nBeatBackRate;
    int							m_nStunStrikeRate;		    // 击晕率
    int                         m_nKnockedBackRate;         // 被击退率
    int							m_nKnockedOffRate;		    // 被击飞率

    BOOL                        m_bImmortal;                // 不会死去

    int                         m_nMountingRate;            // 被打下马的抵抗概率

    int                         m_nAccumulateValue;         // 蓄气值

    int                         m_nParryBaseRate;
    int                         m_nParry;                   // 招架点数
    int                         m_nParryBase;
    int                         m_nParryPercent;
    int                         m_nParryValue;              // 招架值，决定招架减免的伤害
    int                         m_nParryValueBase;
    int                         m_nParryValuePercent;
    int                         m_nSense;                   // 洞察值 与NPC的识破有关
    int                         m_nStrain;                  // 应变值 和Player抵抗Npc的识破有关
    int                         m_nStrainBase;
    int                         m_nStrainPercent;
    int                         m_nToughnessBaseRate;
    int                         m_nToughness;               // 韧性值 用来减少被会心的几率
    int                         m_nToughnessBase;
    int                         m_nToughnessPercent;

    int                         m_nCriticalDamagePowerBaseKiloNumRate;
    int                         m_nCriticalDamagePower;     // 增加会心伤害
    int                         m_nCriticalDamagePowerBase;
    int                         m_nCriticalDamagePowerPercent;
    int                         m_nDecriticalDamagePowerBaseKiloNumRate;
    int                         m_nDecriticalDamagePower;   // 减少会心伤害
    int                         m_nDecriticalDamagePowerBase;
    int                         m_nDecriticalDamagePowerPercent;

    int                         m_nTransferDamageToManaPercent; //伤害转为内力

    int                         m_nModifyCostManaPercent;   // 人物技能的消耗修正

    DWORD                       m_dwAddDamageByDstMoveStateMask;    // 根据目标的移动状态增加伤害的掩码
    int                         m_nAddDamagePercentByDstMoveState;  // 根据目标的移动状态增加伤害的百分比

    DWORD                       m_dwTransferDamageToCharacter;
    int                         m_nTransferDamagePercent;

    DWORD                       m_dwTransferTherapyToCharacter;
    int                         m_nTransferTherapyPercent;

    // -->[破绽]
#ifdef _SERVER
    int                         m_nUpperWeak;               // 上段破绽
    int                         m_nMiddleWeak;              // 中段破绽
    int                         m_nLowerWeak;               // 下段破绽
#endif

    int                         m_nUpperWeakShield;         // 上段破绽盾
    int                         m_nMiddleWeakShield;        // 中段破绽盾
    int                         m_nLowerWeakShield;         // 下段破绽盾

    int                         m_nUpperWeakLevel;         // 破绽级别（0，低，中，高）
    int                         m_nMiddleWeakLevel;
    int                         m_nLowerWeakLevel;

    DECLARE_LUA_INTEGER(UpperWeakLevel);
    DECLARE_LUA_INTEGER(MiddleWeakLevel);
    DECLARE_LUA_INTEGER(LowerWeakLevel);

    // [破绽]<--

    // 偷取敌方生命百分比: 指对敌方造成伤害后，伤害中的多少比例转化为自己的生命,单位(1/1024)
    // 偷取敌方内力百分比: 比如玩家A被B攻击,A受到最终伤害1000点,这时,A可以偷取B的内力(1000 * m_nStealManaPercent / 1024)

	DECLARE_LUA_INTEGER(Dodge);
    DECLARE_LUA_INTEGER(DodgeBaseRate);
    DECLARE_LUA_BOOL(FightState);
    DECLARE_LUA_BOOL(SheathFlag);
    DECLARE_LUA_INTEGER(PositiveShield);
    DECLARE_LUA_INTEGER(NegativeShield);
    DECLARE_LUA_INTEGER(GlobalBlock);
    DECLARE_LUA_INTEGER(GlobalDamageAbsorb);
	DECLARE_LUA_INTEGER(GlobalDamageManaShield);
    DECLARE_LUA_INTEGER(DamageToLifeForSelf);
    DECLARE_LUA_INTEGER(DamageToManaForSelf);
    DECLARE_LUA_INTEGER(BeatBackRate);
    DECLARE_LUA_INTEGER(StunStrikeRate);
    DECLARE_LUA_INTEGER(KnockedBackRate);
    DECLARE_LUA_INTEGER(KnockedOffRate);
    
    DECLARE_LUA_INTEGER(ParryBaseRate);
    DECLARE_LUA_INTEGER(Parry);
    DECLARE_LUA_INTEGER(Sense);
    DECLARE_LUA_INTEGER(Strain);
    DECLARE_LUA_INTEGER(ToughnessBaseRate);
    DECLARE_LUA_INTEGER(Toughness);
    DECLARE_LUA_INTEGER(CriticalDamagePowerBaseKiloNumRate);
    DECLARE_LUA_INTEGER(CriticalDamagePower);
    DECLARE_LUA_INTEGER(DecriticalDamagePowerBaseKiloNumRate);
    DECLARE_LUA_INTEGER(DecriticalDamagePower);
    DECLARE_LUA_INTEGER(ParryValue);

    DECLARE_LUA_INTEGER(MountingRate);
    DECLARE_LUA_INTEGER(AccumulateValue);

    // -------------- 外功 ------------------------------------------------------>
	int m_nMeleeWeaponDamageBase;
	int m_nMeleeWeaponDamageRand;

	DECLARE_LUA_INTEGER(MeleeWeaponDamageBase);
	DECLARE_LUA_INTEGER(MeleeWeaponDamageRand);

    int m_nRangeWeaponDamageBase;
    int m_nRangeWeaponDamageRand;

	DECLARE_LUA_INTEGER(RangeWeaponDamageBase);
	DECLARE_LUA_INTEGER(RangeWeaponDamageRand);

    int m_nSkillPhysicsDamage;
    int m_nSkillPhysicsDamageRand;
    int m_nSkillPhysicsDamagePercent;
    int m_nPhysicsDamageCoefficient;     // 外功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nPhysicsAttackPower;
    int m_nPhysicsAttackPowerBase;
    int m_nPhysicsAttackPowerPercent;

    int m_nPhysicsHitBaseRate;
    int m_nPhysicsHitValue;
    //int m_nAgilityCriticalStrike;        // 身法影响的外功致命一击值
    int m_nPhysicsCriticalStrikeBaseRate;
    int m_nPhysicsCriticalStrike;        // 外功致命一击值 = m_nAgilityCriticalStrike + 其他(装备或BUFF加成等)
    int m_nPhysicsCriticalStrikeBase;

    int	m_nPhysicsResistPercent;	

    // 外功克性,针对削弱目标的外功防御
    int m_nPhysicsOvercome;
    int m_nPhysicsOvercomeBase;
    int m_nPhysicsOvercomePercent;
             
    // 外功防御
    // int m_nPhysicsDefence;
    int	m_nPhysicsDefenceAdd;       
	int	m_nPhysicsDefenceMax;	

	// 外功防御力, 用于计算"m_nPhysicsDefence"
	int	m_nPhysicsShield; 
    int	m_nPhysicsShieldBase;      
    int	m_nPhysicsShieldPercent;
    int	m_nPhysicsShieldAdditional;	

    // 外功反弹,该值是直接反弹给攻击者的伤害点数,但是并不会减少自己受到的伤害数值
    // 外功反弹百分比,该值会减少自己所受伤害，并将伤害反弹给对方, 单位(1/1024),数值 [0, 1024]

	int	m_nPhysicsReflection;		
	int	m_nPhysicsReflectionPercent;

    int m_nPhysicsBlock;

    int m_nPhysicsDamageAbsorb;    
	int	m_nPhysicsDamageManaShield; 

	DECLARE_LUA_INTEGER(PhysicsAttackPower);
    DECLARE_LUA_INTEGER(SkillPhysicsDamageRand);
    DECLARE_LUA_INTEGER(PhysicsHitValue);
    DECLARE_LUA_INTEGER(PhysicsHitBaseRate);
    DECLARE_LUA_INTEGER(PhysicsCriticalStrike);
    DECLARE_LUA_INTEGER(PhysicsCriticalStrikeBaseRate);
    DECLARE_LUA_INTEGER(PhysicsResistPercent);
    DECLARE_LUA_INTEGER(PhysicsOvercome);
    DECLARE_LUA_INTEGER(PhysicsShield);
    DECLARE_LUA_INTEGER(PhysicsReflection);
    DECLARE_LUA_INTEGER(PhysicsReflectionPercent);
    DECLARE_LUA_INTEGER(PhysicsBlock);
    DECLARE_LUA_INTEGER(PhysicsDamageAbsorb);
   	DECLARE_LUA_INTEGER(PhysicsDamageManaShield);

    // -------------- 阳性内功 ------------------------------------------------------>

    int m_nSkillSolarDamage;
    int m_nSkillSolarDamageRand;
    int m_nSkillSolarDamagePercent;
    int m_nSolarDamageCoefficient;      // 阳性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nSolarAttackPower;
    int m_nSolarAttackPowerBase;
    int m_nSolarAttackPowerPercent;

    int m_nSolarHitBaseRate;
    int m_nSolarHitValue;
    int m_nSolarCriticalStrikeBaseRate;
    int m_nSolarCriticalStrike;
    int m_nSolarCriticalStrikeBase;

    int m_nSolarOvercome;
    int m_nSolarOvercomeBase;
    int m_nSolarOvercomePercent;

	int	m_nSolarMagicResistPercent;	    // 抵抗百分比

	int m_nSolarMagicShield;			// 防御力,这个数值最终会用来查表获得防御系数
    int m_nSolarMagicShieldBase;
    int m_nSolarMagicShieldPercent;

	int	m_nSolarMagicReflection;
	int	m_nSolarMagicReflectionPercent;

    int m_nSolarMagicBlock;

    int m_nSolarDamageAbsorb;
	int	m_nSolarDamageManaShield;

    DECLARE_LUA_INTEGER(SolarAttackPower);
    DECLARE_LUA_INTEGER(SolarHitValue);
    DECLARE_LUA_INTEGER(SolarHitBaseRate);
    DECLARE_LUA_INTEGER(SolarCriticalStrike);
    DECLARE_LUA_INTEGER(SolarCriticalStrikeBaseRate);
    DECLARE_LUA_INTEGER(SolarOvercome);
    DECLARE_LUA_INTEGER(SolarMagicShield);
    DECLARE_LUA_INTEGER(SolarMagicResistPercent);
    DECLARE_LUA_INTEGER(SolarMagicReflection);		
    DECLARE_LUA_INTEGER(SolarMagicReflectionPercent);
    DECLARE_LUA_INTEGER(SolarMagicBlock);
    DECLARE_LUA_INTEGER(SolarDamageAbsorb);     
    DECLARE_LUA_INTEGER(SolarDamageManaShield); 

    // -------------- 中性内功 ------------------------------------------------------>
    int m_nSkillNeutralDamage;
    int m_nSkillNeutralDamageRand;
    int m_nSkillNeutralDamagePercent;	
    int m_nNeutralDamageCoefficient;        // 中性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)        


    int m_nNeutralAttackPower;
    int m_nNeutralAttackPowerBase;
    int m_nNeutralAttackPowerPercent;

    int m_nNeutralHitBaseRate;
    int m_nNeutralHitValue;               
    int m_nNeutralCriticalStrikeBaseRate;
    int m_nNeutralCriticalStrike;
    int m_nNeutralCriticalStrikeBase;

    int m_nNeutralOvercome;
    int m_nNeutralOvercomeBase;
    int m_nNeutralOvercomePercent;

    int	m_nNeutralMagicResistPercent;	    // 抵抗百分比

    int m_nNeutralMagicShield;			    // 防御力,这个数值最终会用来查表获得防御系数
    int m_nNeutralMagicShieldBase;
    int m_nNeutralMagicShieldPercent;

    int	m_nNeutralMagicReflection;		
    int	m_nNeutralMagicReflectionPercent;	

    int m_nNeutralMagicBlock;             

    int m_nNeutralDamageAbsorb;     
    int	m_nNeutralDamageManaShield; 

    DECLARE_LUA_INTEGER(NeutralAttackPower);
    DECLARE_LUA_INTEGER(NeutralHitValue);
    DECLARE_LUA_INTEGER(NeutralHitBaseRate);
    DECLARE_LUA_INTEGER(NeutralCriticalStrike);
    DECLARE_LUA_INTEGER(NeutralCriticalStrikeBaseRate);
    DECLARE_LUA_INTEGER(NeutralOvercome);
    DECLARE_LUA_INTEGER(NeutralMagicShield);
    DECLARE_LUA_INTEGER(NeutralMagicResistPercent);
    DECLARE_LUA_INTEGER(NeutralMagicReflection);		
    DECLARE_LUA_INTEGER(NeutralMagicReflectionPercent);	
    DECLARE_LUA_INTEGER(NeutralMagicBlock);
    DECLARE_LUA_INTEGER(NeutralDamageAbsorb);     
    DECLARE_LUA_INTEGER(NeutralDamageManaShield); 

    // -------------- 阴性内功 ------------------------------------------------------>
    int m_nSkillLunarDamage;
    int m_nSkillLunarDamageRand;
    int m_nSkillLunarDamagePercent;	
    int m_nLunarDamageCoefficient;          // 阴性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nLunarAttackPower;
    int m_nLunarAttackPowerBase;
    int m_nLunarAttackPowerPercent;

    int m_nLunarHitBaseRate;               
    int m_nLunarHitValue;               
    int m_nLunarCriticalStrikeBaseRate;         
    int m_nLunarCriticalStrike;         
    int m_nLunarCriticalStrikeBase;

    int m_nLunarOvercome;                   // 克性
    int m_nLunarOvercomeBase;
    int m_nLunarOvercomePercent;

    int	m_nLunarMagicResistPercent;	        // 抵抗百分比

    int m_nLunarMagicShield;			    // 防御力,这个数值最终会用来查表获得防御系数
    int m_nLunarMagicShieldBase;
    int m_nLunarMagicShieldPercent;

    int	m_nLunarMagicReflection;		
    int	m_nLunarMagicReflectionPercent;	

    int m_nLunarMagicBlock;             

    int m_nLunarDamageAbsorb;     
    int	m_nLunarDamageManaShield; 

    DECLARE_LUA_INTEGER(LunarAttackPower);
    DECLARE_LUA_INTEGER(LunarHitValue);
    DECLARE_LUA_INTEGER(LunarHitBaseRate);
    DECLARE_LUA_INTEGER(LunarCriticalStrike);
    DECLARE_LUA_INTEGER(LunarCriticalStrikeBaseRate);
    DECLARE_LUA_INTEGER(LunarOvercome);
    DECLARE_LUA_INTEGER(LunarMagicShield);
    DECLARE_LUA_INTEGER(LunarMagicResistPercent);
    DECLARE_LUA_INTEGER(LunarMagicReflection);		
    DECLARE_LUA_INTEGER(LunarMagicReflectionPercent);
    DECLARE_LUA_INTEGER(LunarMagicBlock);
    DECLARE_LUA_INTEGER(LunarDamageAbsorb);     
    DECLARE_LUA_INTEGER(LunarDamageManaShield); 

    // -------------- 毒性 ---------------------------------------------------------->

    int m_nSkillPoisonDamage;
    int m_nSkillPoisonDamageRand;
    int m_nSkillPoisonDamagePercent;	
    int m_nPoisonDamageCoefficient;         // 毒性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nPoisonAttackPower;
    int m_nPoisonAttackPowerBase;
    int m_nPoisonAttackPowerPercent;

    int m_nPoisonHitBaseRate;
    int m_nPoisonHitValue;
    int m_nPoisonCriticalStrikeBaseRate;         
    int m_nPoisonCriticalStrike;         
    int m_nPoisonCriticalStrikeBase;         

    int m_nPoisonOvercome;                  // 克性
    int m_nPoisonOvercomeBase;
    int m_nPoisonOvercomePercent;

    int	m_nPoisonMagicResistPercent;	    // 抵抗百分比

    int m_nPoisonMagicShield;			    // 防御力,这个数值最终会用来查表获得防御系数
    int m_nPoisonMagicShieldBase;
    int m_nPoisonMagicShieldPercent;

    int	m_nPoisonMagicReflection;		
    int	m_nPoisonMagicReflectionPercent;	

    int m_nPoisonMagicBlock;             

    int m_nPoisonDamageAbsorb;     
    int	m_nPoisonDamageManaShield; 

    DECLARE_LUA_INTEGER(PoisonAttackPower);
    DECLARE_LUA_INTEGER(PoisonHitValue);
    DECLARE_LUA_INTEGER(PoisonHitBaseRate);
    DECLARE_LUA_INTEGER(PoisonCriticalStrike);
    DECLARE_LUA_INTEGER(PoisonCriticalStrikeBaseRate);
    DECLARE_LUA_INTEGER(PoisonOvercome);
    DECLARE_LUA_INTEGER(PoisonMagicShield);
    DECLARE_LUA_INTEGER(PoisonMagicResistPercent);
    DECLARE_LUA_INTEGER(PoisonMagicReflection);		
    DECLARE_LUA_INTEGER(PoisonMagicReflectionPercent);	
    DECLARE_LUA_INTEGER(PoisonMagicBlock);
    DECLARE_LUA_INTEGER(PoisonDamageAbsorb);     
    DECLARE_LUA_INTEGER(PoisonDamageManaShield); 

    // ---------------------------- 治疗(生命回复) -------------------------------------------------
    int m_nTherapyPower;
    int m_nTherapyPowerBase;            // 基础治疗能力
    int m_nTherapyPowerPercent;
    int m_nSkillTherapy; 		        // 技能相关的治疗效果
    int m_nSkillTherapyRand;            
    int m_nSkillTherapyPercent; 
    int m_nTherapyCoefficient;          // 治疗效果加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nBeTherapyCoefficient;        // 被治疗加成系数

    DECLARE_LUA_INTEGER(TherapyPower);
    DECLARE_LUA_INTEGER(SkillTherapy);

    // --------------------------- 新手技能有关 ----------------------------------------------------
    
    int                         m_nNoneWeaponAttackSpeedBase;           // 空手时拳头的攻击速度
    // 当人物身上没有装备近程武器时,就用m_nNoneWeaponAttackSpeedBase替代m_nMeleeWeaponAttackSpeedBase来计算
    // m_nMeleeWeaponAttackSpeed的值.
    int                         m_nMeleeWeaponAttackSpeed;              // 近战攻击速度, 单位 帧/次数
    int                         m_nMeleeWeaponAttackSpeedBase;          // 近战攻击速度基础, 单位 次数/(1024*帧),如果这个值为512,则就是说 512/1024 = 0.5次/帧 
    int                         m_nMeleeWeaponAttackSpeedPercentAdd;
    int                         m_nMeleeWeaponAttackSpeedAdditional;
    
    int                         m_nRangeWeaponAttackSpeed;              // 远程攻击速度, 单位 帧/次数
    int                         m_nRangeWeaponAttackSpeedBase;          // 远程攻击速度基础, 单位 次数/帧
    int                         m_nRangeWeaponAttackSpeedPercentAdd;
    int                         m_nRangeWeaponAttackSpeedAdditional;

    DECLARE_LUA_INTEGER(NoneWeaponAttackSpeedBase);
    DECLARE_LUA_INTEGER(MeleeWeaponAttackSpeed);
    DECLARE_LUA_INTEGER(MeleeWeaponAttackSpeedBase);
    DECLARE_LUA_INTEGER(MeleeWeaponAttackSpeedPercentAdd);
    DECLARE_LUA_INTEGER(MeleeWeaponAttackSpeedAdditional);

    DECLARE_LUA_INTEGER(RangeWeaponAttackSpeed);
    DECLARE_LUA_INTEGER(RangeWeaponAttackSpeedBase);
    DECLARE_LUA_INTEGER(RangeWeaponAttackSpeedPercentAdd);
    DECLARE_LUA_INTEGER(RangeWeaponAttackSpeedAdditional);
    // ---------------------------------------------------------------------------------------------

    int                         m_nIgnoreOtherAlarmRange;           // 忽视他人(NPC)警戒范围
    int                         m_nSelfAlarmRangeRevise;            // 对自身警戒范围的修正; 一般用于NPC
    int                         m_nOtherAlarmRangeRevise;           // 对他人警戒范围的修正 
    int                         m_nResistBehaveConstraintPercent;   // 角色脱离现有行为受控状态的概率, 基数为1024
    int                         m_nUnarmCounter;

    // -------------------------------- 禁魔相关 ---------------------------------------------------
    char                        m_nSilenceRefCounter[sktTotal - 1]; // 根据技能的 KindType 禁魔。

    // -------------------------------- 免疫相关 ---------------------------------------------------
    std::bitset<sftTotal>       m_ImmunityFlag;// 根据BUFF的 FunctionType 免疫。

    // ------------------------------ 技能条件标志 -------------------------------------------------
    DWORD                       m_dwSkillCastFlag;

    // --------------- 记录唯一作用的BUFF此时作用的角色ID ------------------------------------
    DWORD                       m_dwBuffEffectSoleTargetID;

public:
    // m_dwRepresentState描述了角色身上的魔法特效状态(其实是三个特效ID的组合), 一共描述了三个效果:
    // 1. 脚底下踩的(从右到左0-4位).
    // 2. 身上缠绕的(从右到左5-9位)
    // 3. 头上顶着的(从右到左10-14位)
	DWORD   					m_dwRepresentState;

    BOOL    GetRepresentState(KBUFF_REPRESENT_POS nPos, DWORD&   rdwRepresentID);

public:

#ifdef _CLIENT
    KThreatList				m_ThreatList;				        // 仇恨表
#endif  //_CLIENT

#ifdef _SERVER
    KSimpThreatList         m_SimpThreatList;

	int						m_nThreatTime;				        // 仇恨消退间隔

    int                     m_nActiveBaseThreatCoefficient;     // 主动产生的基础仇恨系数
    int                     m_nActiveDamageThreatCoefficient;   // 主动产生的伤害仇恨系数
    int                     m_nActiveTherapyThreatCoefficient;  // 主动产生的治疗仇恨系数
    int                     m_nPassiveBaseThreatCoefficient;    // 被动产生的基础仇恨系数
    int                     m_nPassiveDamageThreatCoefficient;  // 被动产生的伤害仇恨系数
    int                     m_nPassiveTherapyThreatCoefficient; // 被动产生的治疗仇恨系数

	int						m_nOverThreatPercent;
    BOOL                    m_bToDie;
    DWORD                   m_dwKillerID;         
    BOOL ModifyThreat(THREAT_TYPE eType, int nThreatValue);       // 用于技能直接修改对方仇恨值

    int  AdjustThreat(THREAT_TYPE eTheatType, int nThreatValue);                           
#endif	//_SERVER

    // 新AI相关
public:
    struct KCharacterAIData 
    {
        int             nAIType;

        int             nOriginPosX;
        int             nOriginPosY;
        int             nOriginPosZ;
        int             nOriginDirection;

        int             nReturnPosX;
        int             nReturnPosY;
        int             nReturnPosZ;

        int             nPatrolPathID;
        KPatrolPath*    pPatrolPath;
        int             nPatrolOrderIndex;

        int             nEscapeCount;

        static DWORD    dwAIEventSrc;
        static int      nAIEventParam;

        int             nCannotAttactCount;
        int             nSkillFailedCount;
        int             nSkillType;
        DWORD           dwRespondCharacterID; // 记录呼叫者的ID
    }   m_AIData;
    // DECLARE_LUA_DWORD(AIEventSrc);
    DWORD getAIEventSrc(void)   {return m_AIData.dwAIEventSrc;};
    // DECLARE_LUA_INTEGER(AIEventParam);
    DWORD getAIEventParam(void) {return m_AIData.nAIEventParam;};
    
public:
#ifdef _CLIENT
    BOOL PrepareSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, int nPrepareFrame, int nEndFrame);
#endif

#ifdef _SERVER
    SKILL_RESULT_CODE CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget);
    SKILL_RESULT_CODE CastSkillSub(KCharacter* pLogicCaster, DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget);
    SKILL_RESULT_CODE CastSkillAccumulate(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget);
#else
    SKILL_RESULT_CODE CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, DWORD dwBulletID);
    SKILL_RESULT_CODE CastSkillSub(KCharacter* pLogicCaster, DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, DWORD dwBulletID);
#endif
    BOOL ProcessCommonSkillActive(const KSKILL_RECIPE_KEY& crSkillRecipeKey);

    inline int GetWalkSpeed()
    {
        if (IS_PLAYER(m_dwID))
        {
            return m_nRunSpeed * 6 / 20;
        }
        return m_nRunSpeed * 6 / 28;
    }

    // 强制停止状态(到Stand),不论原来是什么状态
	BOOL Stop(void);			
    // 该函数用于修正坐标范围
    BOOL EmendateDestCoordinate(int& nDestX, int& nDestY); 

    BOOL Stand(BOOL bSyncSelf);
	BOOL WalkTo(int nDestX, int nDestY, BOOL bBroadcast, int nWalkSpeed = 0);   // 当WalkSpeed等于0时，取Character身上的走路速度
	BOOL RunTo(int nDestX, int nDestY, BOOL bBroadcast, int nRunSpeed = 0);   // 当RunSpeed等于0时，取Character身上的跑步速度
    BOOL SwimTo(int nDestX, int nDestY, BOOL bBroadcast);			
	BOOL Jump(BOOL bStandJump, int nJumpDirection, BOOL bSyncSelf);
	BOOL SitDown(BOOL bSyncSelf);
    BOOL AutoFly(int nFromNode, int nToCity);

	BOOL Dash(int nFrame, int nDirectionXY, int nVelocityXY, int nVelocityZ);
    BOOL Pull(int nFrame, int nDirectionXY, int nVelocityXY, int nVelocityZ);
    BOOL Repulsed(int nFrame, int nDirectionXY, int nVelocityXY);
	BOOL KnockedDown(int nFrame);
	BOOL KnockedBack(int nDirection, int nDistance, int nVelocityXY);
	BOOL KnockedOff(int nDirection, int nDistance, int nVelocityXY);
    BOOL Halt();                                                // 眩晕
    BOOL Freeze();                                              // 定身
    BOOL Entrap();

	BOOL CheckDie();								            // 死亡

	BOOL Action(DWORD dwCharacterID, DWORD dwActionType);		// 动作

	BOOL TurnTo(int nNewDirection, BOOL bBroadcast);
    BOOL PitchTo(int nNewDirection, BOOL bBroadcast);

    BOOL MoveTo(int nX, int nY, int nZ, BOOL bIgnoreBlock, BOOL bBroadcast);

    BOOL GoToHell(DWORD dwKiller = ERROR_ID);

	const char* GetName();

protected:
	BOOL						m_bOnTalk;					// 是否正在和NPC对话
   
public:
    int                         m_nFearCounter;             // 恐惧计数器
	int							m_nMoveFrameCounter;

public:
    BOOL SelectTarget(TARGET_TYPE eTargetType, DWORD dwTargetID);

    KTarget                     m_SelectTarget;
    KTarget                     m_SkillTarget;

public:
    BOOL CheckFear();

    // 状态处理函数
    BOOL OnStand(){ return true; }
    BOOL OnWalk(){ return true; }
    BOOL OnRun(){ return true; }
    BOOL OnJump(){ return true; }
    BOOL OnSwimJump(){ return true; }
    BOOL OnSit(){ return true; }
    BOOL OnPassiveMove();
    BOOL OnDash();
    BOOL OnPull();
    BOOL OnRepulsed();
    BOOL OnHalt(){ return true; }
    BOOL OnFreeze(){ return true; }
    BOOL OnEntrap(){ return true; }
    BOOL OnDeath();

    BOOL OnRise();
    BOOL OnSkid();

	void SetMoveState(int nState);

public:

    // ------------- 持续一段时间的行为 ---------------------->
	BOOL DoOTAction(KOT_ACTION_PARAM& rParam);
	void DoActionIdle(void);
    void DoAutoCastSkill(KAUTO_CAST_SKILL_PARAM& param);
    void DoAutoCastSkillIdle(void);
    BOOL DoSkillPrepare(KSkillRecipePointer& rSkillRecipePointer, int nPrepareFrame, int nEndFrame);
	BOOL DoSkillChannel(KSkillRecipePointer& rSkillRecipePointer, int nChannelFrame);
    BOOL DoRecipePrepare(KProfessionSkill* pProfessionSkill, int nFrame);
	BOOL DoPickPrepare(KDoodad* pDoodad, int nFrame);
	BOOL DoPicking(KDoodad* pDoodad);
	BOOL DoCustomOTActionPrepare(BOOL bIsBreakable, int nFrame, KTarget& crTarget, DWORD dwScriptID);
	BOOL DoCustomOTActionChannel(BOOL bIsBreakable, int nFrame, KTarget& crTarget, DWORD dwScriptID);

    KOT_ACTION_PARAM            m_OTActionParam;
    KAUTO_CAST_SKILL_PARAM      m_AutoCastSkillParam;

    // -------------- 对吟唱修改的掩码，和技能相对应 -------------->
    DWORD   m_dwDirectCastMask;

    void ProcessOTAction();
    void ProcessAutoCastSkill();

#ifdef _SERVER
    void DoAutoTurn();                      // 技能自动朝向代码，不是Action状态机的组成部分
	void DoSitDown(KDoodad* pDoodad);		// 坐到一个Doodad上，与打坐不同，不改变移动状态，也不影响回血 
#endif

    BOOL OnSkillPrepare();
    BOOL OnSkillChannel();
    BOOL OnRecipePrepare();
    BOOL OnPickPrepare();
	BOOL OnPicking();

#ifdef _SERVER
	BOOL OnItemSkill();
#endif
    void OnUseItemPrepare();

	BOOL OnCustomOTActionPrepare();
	BOOL OnCustomOTActionChannel();

	BOOL OnBreakPickPrepare(KOT_ACTION_PARAM& rNewAction);
	BOOL OnBreakPicking(KOT_ACTION_PARAM& rNewAction);

    // <------------- 持续一段时间的行为 ----------------------

public:
    BOOL UpdateMoveParameter();

    void InvalidateMoveState(int nDelayFrame, BOOL bMoveSyncSelf);

	void ProcessMove();

private:

    void ProcessAutoFly();

    int  GetFrictionDirection();
    BOOL ProcessAcceleration();      // 自身加速度计算  

    void ProcessHorizontalMove();               // 水平移动
    void ProcessHorizontalMoveStupid();
    void ProcessHorizontalMoveSmart(); 

    void ProcessVerticalMove();             // 垂直移动
    void ProcessDrowning();

	BOOL CellMoveStupid(int nStepLength);
    BOOL CellMoveSmart(int nStepLength);

	int  TryMove(int nStepLength, int nDirection, int* pnObstacleDirection);
    BOOL IgnoreDynamicObstacle();
    void ProcessDynamicObstacle();

    BOOL ProcessDropDamage();
    BOOL ProcessDropSpeed();

    BOOL CheckSlip();

    int  m_nMoveSyncFrame;
    BOOL m_bMoveSyncSelf;

public:
    // 获取吃水深度, 在水中是,返回一个大于0的值表示吃水深度,否则返回0
    // 吃水深度指水面与人物脚下的高度差
    int GetWaterline();          

    // 获取人物主观的移动意愿方向
    int GetSubjectiveDirection();

    // 获取主观移动速度
    int GetSubjectiveVelocity();

public:
	void ApplyAttribute(KAttribute* pAttr);
	void UnApplyAttribute(KAttribute* pAttr);

    BOOL CallAttributeFunction(int nKey, BOOL bApply, int nValue1, int nValue2);

    void UpdateWeaponAttackSpeed();
    void UpdateBaseState(); // Life & Mana
    void UpdateAllAttackPowerValue();
    void UpdateAllShieldValue();
    void UpdateAllCriticalStrikeValue();

	BOOL SetForceID(DWORD dwForceID);

    int  m_nNextReplenishFrame;
    void AttribReplenish();
    void ReviseAttribValue();
    void CheckFightFlag();

	virtual BOOL ChangeRegion(KRegion* pRegion) = 0;

	// 处理角色的状态机
	void RunStateMachine();

	friend class KAIController;
	friend class KAIBase;
	friend class KSkill;
	friend class KBuffList;
	friend class KItemList;

#ifdef _SERVER
public:
    void AddBullet(KSKILL_BULLET* pBullet);
	BOOL ApplyBullet(KSKILL_BULLET *pBullet);
    void ClearBullet();

    void CheckBullet();
    void ProcessBullet(KSKILL_BULLET *pBullet);
    
    KSKILL_BULLET* m_pBulletListHead;

private:
    KSKILL_HIT_RESULT JudgeSkillHit(KSKILL_BULLET* pBullet);
#endif

public:
    // 技能属性链表运算流程的参数

    struct KSKILL_ATTRIBUTE_PARAM 
    {
        KSkillRecipePointer SkillRecipePointer;
        DWORD               dwSkillSrcID;
        int                 nSkillSrcLevel;
        KCharacter*         pSkillSrc;
        KCharacter*         pSkillDst;
        BOOL                bCriticalStrike;       // 是否暴击
        BOOL                bParray;               // 是否被招架
        BOOL                bInsight;              // 是否被识破
        // 发技能者的参数,伤害计算时要用,特别是Dot
        KSKILL_BULLET*      pBullet;
        int                 nPhysicsDamage;         // 外功伤害点数
        int                 nSolarDamage;           // 阳性内功伤害
        int                 nNeutralDamage;         // 中性内功伤害
        int                 nLunarDamage;           // 阴性内功伤害
        int                 nPoisonDamage;          // 毒性伤害点数
        int                 nDamgeToLifeForSelf;    // 偷取对方生命点数
        int                 nDamgeToManaForSelf;    // 偷取对方内力点数
        int                 nTherapy;               // 治疗能力

        int                 nBaseThreatCoefficient;
        int                 nDamageThreatCoefficient;
        int                 nTherapyThreatCoefficient;

        BOOL                bDot;
        int                 nStackNum;

        int                 nDamageAddPercent;  // 技能秘笈对伤害的加成，不分类型。
    };

    KSKILL_ATTRIBUTE_PARAM* m_pSkillAttributeParam;

    KSKILL_CALCULATE_RESULT* m_pSrcSkillCalculateResult;
    KSKILL_CALCULATE_RESULT* m_pDstSkillCalculateResult;

    int m_nSkillStackDepth;

    // 该结构用于抽象不同类型的伤害运算参数
    struct KDAMAGE_CALCULATE_PARAM
    {
        KCharacter*  pSkillSrc;
        BOOL         bCriticalStrikeFlag;           // 是否暴击
        int          nDamageValue;                  // 攻击数值
        int          nDstResistPercent;             // 抗性百分比
        int          nDstDefenceCoefficient;        // 防御系数
        int*         pnDstMaxTypedDamageAbsorb;     // 有类型的伤害吸收
        int*         pnDstMaxTypedDamageManaShield; // 有类型的ManaShield
        int          nDstPointReflection;           // 点数反弹
        int          nDstPercentReflection;         // 百分比反弹
        int          nDamageToLifeForSrc;           // 伤害转化为释放者的生命
        int          nDamageToManaForSrc;           // 伤害转化为释放者的内力
        int          nDamageToLifeForSrcParty;      // 伤害转化为释放者的小队所有成员的生命
        int          nDamageToManaForSrcParty;      // 伤害转化为释放者的小队所有成员的内力
        int*         pnDstTypedDamage;              // 对目标的有类型伤害存放到这里
        int          nDstTypedDamageCoefficient;
    };

    // 击倒
    BOOL ProcessKnockedDown(int nFrame);
    // 击退
    BOOL ProcessKnockedBackExhale(int nDistance, int nVelocityXY);       // 水平方向上被击退(方向背向着技能释放者)   
    BOOL ProcessKnockedBackConvergence(int nDistance, int nVelocityXY);  // 水平方向上被击退(方向向着技能释放者)  
    BOOL ProcessKnockedBackRandom(int nDistance, int nVelocityXY);       // 水平方向上被击退(方向随机)   
    BOOL ProcessKnockedBackSrcFace(int nDistance, int nVelocityXY);      // 水平方向上被击退(方向为技能释放者的朝向)       
    // 击飞
    BOOL ProcessKnockedOffParabola(int nDistance, int nVelocityXY);      // 呈抛物线被击飞(水平上的方向向着技能释放者)  
    // 被抓到身边
    BOOL ProcessPull(KCharacter* pCharacter, int nVelocityXY);
    // 滑步处理
    BOOL ProcessRepulsed(int nFrame);
    // 恐惧
    BOOL ProcessFear(int nDistance);
    // 复活
    BOOL ProcessRevive(int nLife, int nMana);
    BOOL ProcessTherapy(int nTherapyValue);
    BOOL ProcessDamage(int nDamageType, int nDamageValue);

    BOOL ConcludeResult(DWORD dwBulletID, BOOL bCriticalStrikeFlag, int* pnRetDamageValue);
private:
    BOOL CalculateDamage(const KDAMAGE_CALCULATE_PARAM &crParam);

public:
    KBuffList m_BuffList;

public:
#ifdef _SERVER
    typedef std::vector<KUSER_SKILL_EVENT> KUSER_SKILL_EVENT_VECTOR;
    KUSER_SKILL_EVENT_VECTOR    m_SkillEventVector;

    // 注意: "KUSER_SKILL_EVENT_VECTOR SkillEvent"不是引用传值是故意这样写的
    // 因为技能事件处罚技能可能去修改原来的Vector
    void OnSkillEvent(
        KSkill* pSkill, int nEventType, KUSER_SKILL_EVENT_VECTOR SkillEvent, 
        KCharacter* pCaster, KCharacter* pTarget
    );
    void CheckBeatBreak(KSkill* pSkill);    // 打断, 现在只对通道技有效
    void CheckBeatBack();
#endif
    void OnOTActionBeatBack(int nFrame);
    void OnDamage(KCharacter* pAttackter, int nDamage);

	DWORD GetMapID();

	BOOL CanDialog(KCharacter* pCharacter);
	BOOL CanDoAction(KCharacter* pCharacter, DWORD dwAction);

#ifdef _SERVER
public:
	int m_nLastBroadcastLife;
#endif

    // -------------------------- 调试 --------------------------------------------->
    KSkillDebug             m_SkillDebug;

    int LuaAttach(Lua_State* L);
    int LuaDetach(Lua_State* L);
    int LuaDetachAll(Lua_State* L);
    // <-----------------------------------------------------------------------------

    KAIVM   m_AIVM;

public:
	// 脚本接口函数
    int LuaSetModelID(Lua_State* L);
    int LuaSetTarget(Lua_State* L);
	int LuaGetTarget(Lua_State* L);
    int LuaCastSkillXYZ(Lua_State* L);
	int LuaCastSkill(Lua_State* L);

    void HostStand();
	void HostJump(BOOL bStandJump, int nDirectionXY);
	void HostDash(int nDirection, int nDistance, int nVelocityXY);
	void HostTurnTo(int nNewDirection);
    void HostPitchTo(int nNewDirection);

    int LuaAutoFly(Lua_State* L);
	int LuaStop(Lua_State* L);
    int LuaStand(Lua_State* L);
	int LuaRunTo(Lua_State* L);
	int LuaWalkTo(Lua_State* L);
    int LuaSwimTo(Lua_State* L);
	int LuaJump(Lua_State* L);
	int LuaDash(Lua_State* L);
	int LuaTurnTo(Lua_State* L);
    int LuaPitchTo(Lua_State* L);

	int LuaPatrol(Lua_State* L);

	int LuaSitDown(Lua_State* L);
	int LuaDie(Lua_State* L);

	int LuaDoAction(Lua_State* L);

    int LuaGetAIType(Lua_State* L);
	int LuaSetAIType(Lua_State* L);
    int LuaSetAIDebug(Lua_State* L);

#ifdef _CLIENT
    int LuaGetBuffList(Lua_State* L);
#endif

    int LuaCancelBuff(Lua_State* L);

    int LuaSetSheath(Lua_State* L);

	int LuaGetMapID(Lua_State* L);
	int LuaCanDialog(Lua_State* L);

#ifdef _SERVER
    int LuaAddBuff(Lua_State* L);

    int LuaGetBuff(Lua_State* L);

    int LuaDelBuff(Lua_State* L);
    int LuaDelGroupBuff(Lua_State* L);

    int LuaDelMultiGroupBuffByFunctionType(Lua_State* L);

    int LuaSetBuffNextActiveFrame(Lua_State* L);
    int LuaSetBuffLeftActiveCount(Lua_State* L);

	int LuaClearAllThreat(Lua_State* L);

	int LuaSetForceID(Lua_State* L);

	int LuaSetPosition(Lua_State* L);
#endif

    int LuaFireAIEvent(Lua_State* L);

#ifdef _SERVER
    int LuaSetCustomInteger4(Lua_State* L);
    int LuaGetCustomInteger4(Lua_State* L);
    int LuaSetCustomInteger2(Lua_State* L);
    int LuaGetCustomInteger2(Lua_State* L);
    int LuaSetCustomInteger1(Lua_State* L);
    int LuaGetCustomInteger1(Lua_State* L);

    int LuaSetCustomUnsigned4(Lua_State* L);
    int LuaGetCustomUnsigned4(Lua_State* L);
    int LuaSetCustomUnsigned2(Lua_State* L);
    int LuaGetCustomUnsigned2(Lua_State* L);
    int LuaSetCustomUnsigned1(Lua_State* L);
    int LuaGetCustomUnsigned1(Lua_State* L);
    
    int LuaSetCustomBoolean(Lua_State* L);
    int LuaGetCustomBoolean(Lua_State* L);

    // 偏移量, 第几个Bit(从左到右, 从0开始计数), 数值为true/false
    int LuaSetCustomBitValue(Lua_State* L);
    int LuaGetCustomBitValue(Lua_State* L);

    int LuaSetCustomString(Lua_State* L);
    int LuaGetCustomString(Lua_State* L);
#endif

    int LuaGetSkillPrepareState(Lua_State* L);
#ifdef _SERVER
    int LuaTraceSelectTargetThreat(Lua_State* L);
#endif

    int LuaSetAIOriginPos(Lua_State* L);
    int LuaGetAIOriginPos(Lua_State* L);
};

int g_GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter);


#endif	//_KCHARACTER_H_
