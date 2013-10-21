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
#include "Global.h"
#include "SO3Result.h"
#include "KAttribute.h"
#include "KSceneObject.h"
#include "KThreatList.h"
//#include "KBuffList.h"
#include "KItem.h"
#include "KTarget.h"

#define MAX_POSITION_RECORD	(GAME_FPS * 4)

class KCharacter;
class KPatrolPath;
//struct KSKILL_BULLET;
//class KSkill;
class KLootList;
class KItem;
class KAIControl;

//角色状态机的转移指令
enum CHARACTER_COMMAND
{
	ccInvalid = 0,

	ccTotal
};

struct KCharaterCommand
{
	CHARACTER_COMMAND	CmdKind;		// 命令C
	int					Param_X;		// 参数X
	int					Param_Y;		// 参数Y
	int					Param_Z;		// 参数Z
};

// 角色状态机的状态
enum CHARACTER_MOVE_STATE
{
	cmsInvalid = 0,				// 无效状态

	// 受控状态
	cmsOnStand,					// 站立状态
	cmsOnWalk,					// 走路状态
	cmsOnRun,					// 跑步状态
    cmsOnJump,                  // 跳跃状态
    cmsOnSwim,                  // 游泳状态
	cmsOnSit,					// 坐状态
    cmsOnMeditate,              // 打坐状态

	// 不受控状态
	cmsOnKnockedDown,			// 被击倒状态
	cmsOnKnockedBack,			// 被击退状态
	cmsOnKnockedOff,			// 被击飞状态 
    cmsOnStun,                  // 被击晕状态(眩晕)       
    cmsOnHalt,                  // 被网住或冰冻等

	cmsOnDeath,					// 死亡状态
	cmsOnDash,					// 冲刺状态

	cmsTotal
};



// 该结构用于接收技能运算的结果
//struct KSKILL_CALCULATE_RESULT
//{
//    BOOL bHitTarget;
//    BOOL bSkillDodge;
//    BOOL bCriticalStrike;
//
//    int  nSkillEffectResult[serTotal];   
//    BOOL bPhysicsBlock;
//    BOOL bSolarMagicBlock;
//    BOOL bNeutralMagicBlock;
//    BOOL bLunarMagicBlock;
//    BOOL bPoisonBlock;
//    BOOL bDebuffDodge;
//    BOOL bDebuffImmunity;
//    int  nDebuffID;
//    int  nDebuffLevel;
//    BOOL bDotDodge;
//    int  nDotID;
//    int  nDotLevel;
//    
//    struct
//    {
//        KSKILL_EFFECT_TYPE  eType;
//        DWORD               dwID;
//        int                 nLevel;        
//    } EffectSrc;
//};

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
    // ... ...
};

//struct KProfessionSkill
//{
//	KTarget Target;
//	DWORD dwCraftID;
//	DWORD dwRecipeID;
//};

//struct KOT_ACTION_PARAM 
//{
//	KOT_ACTION_TYPE	    eActionType;
//	union 
//	{
//		KSkill*         pSkill;
//		KDoodad*        pDoodad;
//		KItem*			pItem;
//	} Data;
//    KTarget		        Target;
//	KProfessionSkill	ProfessionSkill;
//	int			        nEndFrame;
//    union
//    {
//        int             nNextActFrame;  // 通道技下用这个表示下一次作用时间
//        int             nPrepareFrame;  // 吟唱技下用这个表示该技能本身吟唱时间 
//    };
//
//    int                 nBeatBackCount; // 吟唱/通道被打退的计数
//	DWORD				dwScriptID;		// CustomOTAction会带一个脚本
//	BOOL				bIsBreakable;	// 是否会被攻击打断
//};

//struct KAUTO_CAST_SKILL_PARAM
//{
//    KSkill* pSkill;
//    BOOL    bMelee;
//    int*    pnCastCycle;
//    int     nPreCastFrame;
//};

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


class KCharacter : public KSceneObject
{
//属性值
public:
	//杂项
	char						m_szName[_NAME_LEN];		// 角色名
	char						m_szTitle[TITLE_LEN];		// 称号
	CHARACTER_GENDER			m_eGender;					// 性别
	CHARACTER_KIND				m_eKind;					// 角色类型
	DWORD						m_dwClassID;				// 分类ID
	CHARACTER_RACE				m_eRace;					// 角色种族
	DWORD						m_dwForceID;			    // 势力
	int							m_nLevel;					// 角色等级
	int							m_nExperience;				// 角色经验
	int							m_nMoral;					// 角色的道德值
    DWORD                       m_dwPortraitID;             // 头像ID
	DWORD						m_dwCharacterAction;		// 表情状态

	DECLARE_LUA_STRING(Name, _NAME_LEN);
	DECLARE_LUA_STRING(Title, TITLE_LEN);
	DECLARE_LUA_ENUM(Gender);
	DECLARE_LUA_ENUM(Kind);
	DECLARE_LUA_DWORD(ClassID);
	DECLARE_LUA_ENUM(Race);
	DECLARE_LUA_DWORD(ForceID);
	DECLARE_LUA_INTEGER(Level);
	DECLARE_LUA_INTEGER(Experience);
	DECLARE_LUA_INTEGER(Moral);
	DECLARE_LUA_DWORD(CharacterAction);
    // -------------- 移动相关 -------------------------------------------------->
	CHARACTER_MOVE_STATE		m_eMoveState;				// 移动状态

    int                         m_nVelocityXY;  // 水平速度
    int                         m_nDirectionXY; // 水平速度方向
    int                         m_nVelocityZ;   // 垂直速度

	int							m_nDestX;					
	int							m_nDestY;		

    // 收敛速度,比如在Run时,瞬时速度会受到打滑,重力等影响而变化,最终速度收敛于这个值
    int                         m_nConvergenceSpeed;

    int                         m_nWalkSpeed;
    int                         m_nRunSpeed;
    int                         m_nSwimSpeed;           

    int                         m_nWalkSpeedBase;           
    int                         m_nRunSpeedBase;           
    int                         m_nSwimSpeedBase;

    int                         m_nMoveSpeedPercent;    

    int			                m_nCurrentGravity;		
	int			                m_nGravity;			       
    int			                m_nGravityBase;
    int			                m_nGravityPercent;

	int							m_nJumpCount;				// 跳跃计数器
	int							m_nMaxJumpCount;		    // 最大跳跃次数
    int                         m_nDropDefence;             // 抗摔伤系数, [0 - 1024]

    int                         m_nJumpSpeedXY;             // 跳跃的水平初速度
    int                         m_nJumpSpeedXYBase;  
    int                         m_nJumpSpeedXYPercent;

    int                         m_nJumpSpeedZ;              // 跳跃的垂直初速度
    int                         m_nJumpSpeedZBase;
    int                         m_nJumpSpeedZPercent;

    int                         m_nDivingCount;
    int                         m_nDivingFrame;             // 最大潜水时间
    int                         m_nDivingFrameBase;
    int                         m_nDivingFramePercent; 

    int                         m_nSwimAbility;             // 是否会游泳
    int                         m_nWaterFlyAbility;         // 是否可以水上漂

    BOOL                        m_bTryPathState;            // 是否正在绕路过程之中
    int                         m_nTryPathSide;             // 正在往左还是往右绕路
    int                         m_nTryPathDestX;            // 绕路开始的时候的移动目标点X
    int                         m_nTryPathDestY;            // 绕路开始的时候的移动目标点Y

    int                         m_nSubjectiveMoveDirection; // 主观移动意愿的方向,用于表现逻辑做动作切换的

	DECLARE_LUA_ENUM(MoveState);

    DECLARE_LUA_INTEGER(VelocityXY);
    DECLARE_LUA_INTEGER(DirectionXY);
    DECLARE_LUA_INTEGER(VelocityZ);

	DECLARE_LUA_INTEGER(DestX);
	DECLARE_LUA_INTEGER(DestY);

    DECLARE_LUA_INTEGER(WalkSpeed);
    DECLARE_LUA_INTEGER(RunSpeed);
    DECLARE_LUA_INTEGER(SwimSpeed);

    DECLARE_LUA_INTEGER(WalkSpeedBase);
	DECLARE_LUA_INTEGER(RunSpeedBase);
    DECLARE_LUA_INTEGER(SwimSpeedBase);

	DECLARE_LUA_INTEGER(MoveSpeedPercent);
	
    DECLARE_LUA_INTEGER(MaxJumpCount);
    DECLARE_LUA_INTEGER(Gravity);
    DECLARE_LUA_INTEGER(GravityBase);
    DECLARE_LUA_INTEGER(GravityPercent);

    DECLARE_LUA_INTEGER(JumpSpeedXY);
    DECLARE_LUA_INTEGER(JumpSpeedXYBase);
    DECLARE_LUA_INTEGER(JumpSpeedXYPercent);

    DECLARE_LUA_INTEGER(JumpSpeedZ);
    DECLARE_LUA_INTEGER(JumpSpeedZBase);
    DECLARE_LUA_INTEGER(JumpSpeedZPercent);

    DECLARE_LUA_INTEGER(DropDefence);

    DECLARE_LUA_INTEGER(DivingCount);         
    DECLARE_LUA_INTEGER(DivingFrame);         
    DECLARE_LUA_INTEGER(DivingFrameBase);
    DECLARE_LUA_INTEGER(DivingFramePercent); 

    DECLARE_LUA_INTEGER(SwimAbility);
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

    int							m_nCurrentLife;				// 当前生命
	int							m_nMaxLife;					// 生命最大值    
    int                         m_nMaxLifeBase;             // 仅取决于职业,等级
	int							m_nMaxLifePercentAdd;	    // 生命最大值增加百分比
	int							m_nMaxLifeAdditional;		// 追加的生命最大值
	int							m_nLifeReplenish;			// 生命点数恢复速度 = m_nSpunkLifeReplenish + 其他(装备或BUFF加成等)
    int                         m_nSpunkLifeReplenish;      // 精神影响的生命恢复点数
	int							m_nLifeReplenishPercent;	// 生命百分比恢复速度, 单位1/1024

	DECLARE_LUA_INTEGER(CurrentLife);
	DECLARE_LUA_INTEGER(MaxLife);
    DECLARE_LUA_INTEGER(MaxLifeBase);
	DECLARE_LUA_INTEGER(MaxLifePercentAdd);
	DECLARE_LUA_INTEGER(MaxLifeAdditional);
	DECLARE_LUA_INTEGER(LifeReplenish);
	DECLARE_LUA_INTEGER(LifeReplenishPercent);
	

    // ---------------- 内力 ----------------------------------------------------->

	int							m_nCurrentMana;				
	int							m_nMaxMana;					
    int                         m_nMaxManaBase;             
	int							m_nMaxManaPercentAdd;		
	int							m_nMaxManaAdditional;		
	int							m_nManaReplenish;        // 回蓝点数 = m_nSpunkManareplenish + 其他(装备或BUFF加成等)
    int                         m_nSpunkManareplenish;   // 精神影响的回蓝点数
	int							m_nManaReplenishPercent;	

	DECLARE_LUA_INTEGER(CurrentMana);
	DECLARE_LUA_INTEGER(MaxMana);
    DECLARE_LUA_INTEGER(MaxManaBase);
	DECLARE_LUA_INTEGER(MaxManaPercentAdd);
	DECLARE_LUA_INTEGER(MaxManaAdditional);
	DECLARE_LUA_INTEGER(ManaReplenish);
	DECLARE_LUA_INTEGER(ManaReplenishPercent);

    // ---------------- 怒气 ----------------------------------------------------->

	int							m_nCurrentRage;				
	int							m_nMaxRage;					
    int                         m_nMaxRageBase;
	int							m_nMaxRagePercentAdd;		
	int							m_nMaxRageAdditional;		
	int							m_nRageReplenish;			
    int							m_nRageReplenishPercent;	

	DECLARE_LUA_INTEGER(CurrentRage);
	DECLARE_LUA_INTEGER(MaxRage);
    DECLARE_LUA_INTEGER(MaxRageBase);
	DECLARE_LUA_INTEGER(MaxRagePercentAdd);
	DECLARE_LUA_INTEGER(MaxRageAdditional);
	DECLARE_LUA_INTEGER(RageReplenish);
    DECLARE_LUA_INTEGER(RageReplenishPercent);

    // --------------- 战斗杂项 --------------------------------------------------->
	int							m_nDodge;			        // 闪避点数 = m_nAgilityDodege + 其他(装备或BUFF加成等)
    int                         m_nAgilityDodege;           // 身法影响的闪避点数
    BOOL                        m_bFightState;
    BOOL                        m_bSheathFlag;
    int                         m_nPositiveShield;          // 正面效果屏蔽,屏蔽所有正向效果(如加血,Buff)技能
    int                         m_nNegativeShield;          // 负面效果屏蔽,屏蔽所有负向效果(如伤害, Debuff)技能
    int                         m_nGlobalBlock;             // 无类型格挡
    int                         m_nGlobalDamageAbsorb;      // 无类型限制的吸收伤害
	int							m_nGlobalDamageManaShield;  // 无类型限制的内力抵消伤害
	int							m_nStealLifePercent;	    // 偷取敌方生命百分比	
	int							m_nStealManaPercent;        // 偷取敌方内力百分比
    int                         m_nBeatBackRate;
    int							m_nStunStrikeRate;		    // 击晕率
    int                         m_nKnockedBackRate;         // 被击倒率
    int							m_nKnockedOffRate;		    // 被击飞率

    // 偷取敌方生命百分比: 指对敌方造成伤害后，伤害中的多少比例转化为自己的生命,单位(1/1024)
    // 偷取敌方内力百分比: 比如玩家A被B攻击,A受到最终伤害1000点,这时,A可以偷取B的内力(1000 * m_nStealManaPercent / 1024)

	DECLARE_LUA_INTEGER(Dodge);
    DECLARE_LUA_BOOL(FightState);
    DECLARE_LUA_BOOL(SheathFlag);
    DECLARE_LUA_INTEGER(PositiveShield);
    DECLARE_LUA_INTEGER(NegativeShield);
    DECLARE_LUA_INTEGER(GlobalBlock);
    DECLARE_LUA_INTEGER(GlobalDamageAbsorb);
	DECLARE_LUA_INTEGER(GlobalDamageManaShield);
    DECLARE_LUA_INTEGER(StealLifePercent);
    DECLARE_LUA_INTEGER(StealManaPercent);
    DECLARE_LUA_INTEGER(BeatBackRate);
    DECLARE_LUA_INTEGER(StunStrikeRate);
    DECLARE_LUA_INTEGER(KnockedBackRate);
    DECLARE_LUA_INTEGER(KnockedOffRate);

    int m_nPhysicsAttackPower[wdtTotal];        // 基础武学对外功伤害的提升
    int m_nMagicAttackPower[wdtTotal];          // 基础武学对内功伤害的提升


    // -------------- 外功 ------------------------------------------------------>

    int m_nMeleeWeaponDamageBase;
    int m_nMeleeWeaponDamageRand;

    int m_nRangeWeaponDamageBase;
    int m_nRangeWeaponDamageRand;

    int m_nSkillPhysicsDamage;
    int m_nSkillPhysicsDamageRand;
    int m_nSkillPhysicsDamagePercent;
    int m_nPhysicsAttackPowerBase;
    int m_nPhysicsDamageCoefficient;     // 外功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nPhysicsAttackHit;      
    int m_nAgilityCriticalStrike;        // 身法影响的外功致命一击值
    int m_nPhysicsCriticalStrike;        // 外功致命一击值 = m_nAgilityCriticalStrike + 其他(装备或BUFF加成等)

    int	m_nPhysicsResistPercent;	
             
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

	DECLARE_LUA_INTEGER(PhysicsAttackPowerBase);    
    DECLARE_LUA_INTEGER(PhysicsAttackHit);
    DECLARE_LUA_INTEGER(PhysicsCriticalStrike);
    DECLARE_LUA_INTEGER(PhysicsResistPercent);
    DECLARE_LUA_INTEGER(PhysicsShield);
    DECLARE_LUA_INTEGER(PhysicsReflection);		
    DECLARE_LUA_INTEGER(PhysicsReflectionPercent);
    DECLARE_LUA_INTEGER(PhysicsBlock);
    DECLARE_LUA_INTEGER(PhysicsDamageAbsorb);    
   	DECLARE_LUA_INTEGER(PhysicsDamageManaShield); 

    // -------------- 内功 ----------------------------------------------------------
    int m_nSpunkAttackPower;
    // m_nMeleeSpunkAttackPowerPercent = m_nMeleeSpunkAttackPower / (m_nMeleeSpunkAttackPower + 5 * m_nLevel + 5)
    int m_nMeleeSpunkAttackPowerPercent;
    int m_nMeleeSpunkAttackPower;
    int m_nRangeSpunkAttackPowerPercent;
    int m_nRangeSpunkAttackPower;
    // 根骨影响的内功致命一击值; 各种内功的致命一击值 = m_nSpiritCriticalStrike + 其他(装备或BUFF加成等)
    int m_nSpiritCriticalStrike;            

    DECLARE_LUA_INTEGER(SpunkAttackPower);

    // -------------- 阳性内功 ------------------------------------------------------>

    int m_nSkillSolarDamage;
    int m_nSkillSolarDamageRand;
    int m_nSkillSolarDamagePercent;	
    int m_nSolarAttackPowerBase;
    int m_nSolarDamageCoefficient;      // 阳性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)
	
    int m_nSolarMagicHit;               
    int m_nSolarCriticalStrike;         

	int	m_nSolarMagicResistPercent;	    // 抵抗百分比

	int m_nSolarMagicShield;			// 防御力,这个数值最终会用来查表获得防御系数

	int	m_nSolarMagicReflection;		
	int	m_nSolarMagicReflectionPercent;	

    int m_nSolarMagicBlock;             

    int m_nSolarDamageAbsorb;     
	int	m_nSolarDamageManaShield; 

    DECLARE_LUA_INTEGER(SolarMagicHit);
    DECLARE_LUA_INTEGER(SolarCriticalStrike);
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
    int m_nNeutralAttackPowerBase;
    int m_nNeutralDamageCoefficient;        // 中性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)        

    int m_nNeutralMagicHit;               
    int m_nNeutralCriticalStrike;         

    int	m_nNeutralMagicResistPercent;	    // 抵抗百分比

    int m_nNeutralMagicShield;			    // 防御力,这个数值最终会用来查表获得防御系数

    int	m_nNeutralMagicReflection;		
    int	m_nNeutralMagicReflectionPercent;	

    int m_nNeutralMagicBlock;             

    int m_nNeutralDamageAbsorb;     
    int	m_nNeutralDamageManaShield; 

    DECLARE_LUA_INTEGER(NeutralMagicHit);
    DECLARE_LUA_INTEGER(NeutralCriticalStrike);
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
    int m_nLunarAttackPowerBase;
    int m_nLunarDamageCoefficient;          // 阴性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nLunarMagicHit;               
    int m_nLunarCriticalStrike;         

    int	m_nLunarMagicResistPercent;	        // 抵抗百分比

    int m_nLunarMagicShield;			    // 防御力,这个数值最终会用来查表获得防御系数

    int	m_nLunarMagicReflection;		
    int	m_nLunarMagicReflectionPercent;	

    int m_nLunarMagicBlock;             

    int m_nLunarDamageAbsorb;     
    int	m_nLunarDamageManaShield; 

    DECLARE_LUA_INTEGER(LunarMagicHit);
    DECLARE_LUA_INTEGER(LunarCriticalStrike);
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
    int m_nPoisonAttackPowerBase;
    int m_nPoisonDamageCoefficient;         // 毒性内功伤害加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

    int m_nPoisonMagicHit;               
    int m_nPoisonCriticalStrike;         

    int	m_nPoisonMagicResistPercent;	    // 抵抗百分比

    int m_nPoisonMagicShield;			    // 防御力,这个数值最终会用来查表获得防御系数

    int	m_nPoisonMagicReflection;		
    int	m_nPoisonMagicReflectionPercent;	

    int m_nPoisonMagicBlock;             

    int m_nPoisonDamageAbsorb;     
    int	m_nPoisonDamageManaShield; 

    DECLARE_LUA_INTEGER(PoisonMagicHit);
    DECLARE_LUA_INTEGER(PoisonCriticalStrike);
    DECLARE_LUA_INTEGER(PoisonMagicShield);
    DECLARE_LUA_INTEGER(PoisonMagicResistPercent);
    DECLARE_LUA_INTEGER(PoisonMagicReflection);		
    DECLARE_LUA_INTEGER(PoisonMagicReflectionPercent);	
    DECLARE_LUA_INTEGER(PoisonMagicBlock);
    DECLARE_LUA_INTEGER(PoisonDamageAbsorb);     
    DECLARE_LUA_INTEGER(PoisonDamageManaShield); 

    // ---------------------------- 治疗(生命回复) -------------------------------------------------
    int m_nTherapyPowerBase;            // 基础治疗能力
    int m_nSkillTherapy; 		        // 技能相关的治疗效果
    int m_nSkillTherapyRand;            
    int m_nSkillTherapyPercent; 
    int m_nTherapyCoefficient;          // 治疗效果加成系数, 在伤害计算流程的最后会乘上这个系数(所取的数值是被伤害人身上的)

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

    int                         m_nIgnoreOtherAlarmRange;             // 忽视他人(NPC)警戒范围
    int                         m_nSelfAlarmRangeRevise;              // 对自身警戒范围的修正; 一般用于NPC
    int                         m_nOtherAlarmRangeRevise;             // 对他人警戒范围的修正 

public:
    // m_dwRepresentState描述了角色身上的魔法特效状态(其实是三个特效ID的组合), 一共描述了三个效果:
    // 1. 脚底下踩的(从右到左0-4位).
    // 2. 身上缠绕的(从右到左5-9位)
    // 3. 头上顶着的(从右到左10-14位)
	DWORD   					m_dwRepresentState;

    BOOL    GetRepresentState(KBUFF_REPRESENT_POS nPos, DWORD&   rdwRepresentID);

public:
	KThreatList				m_ThreatList;				    // 仇恨表

//状态机
public:
    //BOOL PrepareSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, int nFrame);
    //SKILL_RESULT_CODE CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, DWORD dwBulletID);
    //SKILL_RESULT_CODE CastSkillSub(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, DWORD dwBulletID);

	BOOL Stop(void);			
    // 该函数用于修正坐标范围
    BOOL EmendateDestCoordinate(int& nDestX, int& nDestY); 

    BOOL Stand(BOOL bForceMove = false);
	BOOL WalkTo(int nDestX, int nDestY, BOOL bForceMove = false);
	BOOL RunTo(int nDestX, int nDestY, BOOL bForceMove = false);			
    BOOL SwimTo(int nDestX, int nDestY, int nDestZ, BOOL bForceMove = false);			
	BOOL Jump(BOOL bStandJump, int nJumpDirection, BOOL bForceMove = false);
	BOOL SitDown(BOOL bForceMove = false);

	BOOL Dash(int nDirection, int nDistance, int nVelocityXY);
	BOOL KnockedDown(int nFrame);
	BOOL KnockedBack(int nDirection, int nDistance, int nVelocityXY);
	BOOL KnockedOff(int nDirection, int nDistance, int nVelocityXY);
    BOOL Stun();                                                // 被击晕或眩晕      
    BOOL Halt();                                                // 网住或冰冻等。。

    BOOL Meditate();                                            // 打坐
	BOOL Die(KCharacter* pKiller);								// 死亡
    BOOL Revive();

	BOOL Say(char* pszText);									// 说话(屏幕近聊)
	BOOL Action(DWORD dwCharacterID, BYTE byActionType);		// 动作
	BOOL Turn(int nNewDirection);
    BOOL Pitch(int nNewDirection);
	//BOOL Open(KDoodad* pDoodad);								// 开启
    BOOL MoveTo(int nX, int nY, int nZ);

protected:
	BOOL						m_bOnTalk;					// 是否正在和NPC对话
   
public:
 //   int                         m_nStunCounter;             // 击晕计数器(眩晕)
 //   int                         m_nHaltCounter;             // 定身计数器
	  int						  m_nMoveFrameCounter;

public:
    KTarget                     m_SelectTarget;
    KTarget                     m_SkillTarget;

public:

    // 状态处理函数
    BOOL OnStand(){ return true; }
    BOOL OnWalk(){ return true; }
    BOOL OnRun(){ return true; }
    BOOL OnJump(){ return true; }
    BOOL OnSit(){ return true; }
    BOOL OnMeditate(){ return true; }
    BOOL OnPassiveMove();
    BOOL OnStun(){ return true; }
    BOOL OnHalt(){ return true; }
    BOOL OnDeath();

	// 状态转移指令
	void DoStand();
	void DoWalk();
	void DoRun();
    void DoSwim();
    void DoJump();
	void DoSit();
    void DoMeditate();

	void DoDash();              // 冲锋
	void DoKnockedDown();       // 击倒
	void DoKnockedBack();		// 击退
	void DoKnockedOff();		// 击飞
    void DoStun();
    void DoHalt();
	void DoDeath(int nDelayFrame);

public:

 //   // ------------- 持续一段时间的行为 ---------------------->
	//void DoOTAction(KOT_ACTION_PARAM& rParam);

	//void DoActionIdle(void);

 //   void DoAutoCastSkill(KAUTO_CAST_SKILL_PARAM& param);

 //   void DoAutoCastSkillIdle(void);

 //   void DoSkillPrepare(KSkill* pSkill, int nPrepareFrame);

	//void DoSkillChannel(KSkill* pSkill, int nChannelFrame);

 //   void DoRecipePrepare(KProfessionSkill* pProfessionSkill, int nFrame);

	//void DoPickPrepare(KDoodad* pDoodad, int nFrame);

	//void DoPicking(KDoodad* pDoodad);

	//void DoCustomOTActionPrepare(BOOL bIsBreakable, int nFrame, KTarget& crTarget, DWORD dwScriptID);

	//void DoCustomOTActionChannel(BOOL bIsBreakable, int nFrame, KTarget& crTarget, DWORD dwScriptID);

 //   KOT_ACTION_PARAM            m_OTActionParam;
 //   KAUTO_CAST_SKILL_PARAM      m_AutoCastSkillParam;

 //   void ProcessOTAction();
 //   void ProcessAutoCastSkill();

 //   BOOL OnSkillPrepare();
 //   BOOL OnSkillChannel();
 //   BOOL OnRecipePrepare();
 //   BOOL OnPickPrepare();
	//BOOL OnPicking();
	//BOOL OnItemSkill();
	//BOOL OnCustomOTActionPrepare();
	//BOOL OnCustomOTActionChannel();

	//BOOL OnBreakPickPrepare(KOT_ACTION_PARAM& rNewAction);
	//BOOL OnBreakPicking(KOT_ACTION_PARAM& rNewAction);

    // <------------- 持续一段时间的行为 ----------------------

public:

	BOOL ForwardPosition(int nForwardFrame);

    BOOL UpdateMoveParameter();

private:

	void ProcessMove();

    void ProcessFriction();          // 摩擦阻力计算
    void ProcessGradientSlip();      // 斜坡打滑计算
    void ProcessAcceleration();      // 自身加速度计算  

    void ProcessHorizontalMove();    // 水平移动
    void ProcessVerticalMove();      // 垂直移动

	BOOL CellMove(int nStepLength, int nDirection);
	int  TryMove(int nStepLength, int nDirection, int* pnObstacleDirection);

public:
    // 获取吃水深度, 在水中是,返回一个大于0的值表示吃水深度,否则返回0
    // 吃水深度指水面与人物脚下的高度差
    int GetWaterline();          

    // 取得脚底下地表的绝对高度,当然,水不算"地"
    int GetEarthPos();

    // 获取人物主观的移动意愿方向
    int GetSubjectiveDirection();

private:

public:
	KCharacter();
	virtual ~KCharacter();

	BOOL Init(void);
	BOOL UnInit(void);
	BOOL Activate(void);

public:
	BOOL ApplyAttribute(KAttribute* pAttr);
	BOOL UnApplyAttribute(KAttribute* pAttr);

private:
    int  m_nNextReplenishFrame;
    void AttribReplenish();
    void ReviseAttribValue();
    void CheckFightFlag();

	virtual BOOL ChangeRegion(KRegion* pRegion) = 0;
	virtual BOOL ChangeCell(KCell* pCell) = 0;

	// 处理本循环的指令，对于玩家来说是指令列表，对于NPC来说是调用NPCAI
	virtual BOOL GetCommand() = 0;
	// 处理角色的状态机
	void RunStateMachine();

	friend class KAIController;
	friend class KAIBase;
	//friend class KSkill;
	friend class KBuffList;
	friend class KItemList;

public:
    // 技能属性链表运算流程的参数
    //struct  KSKILL_ATTRIBUTE_PARAM
    //{
    //    KSkill*         pSkill;
    //    DWORD           dwSkillSrcID;
    //    int             nSkillSrcLevel;
    //    KCharacter*     pSkillSrc;
    //    KCharacter*     pSkillDst;
    //    BOOL            bCriticalStrike;       // 是否暴击
    //    // 发技能者的参数,伤害计算时要用,特别是Dot
    //    KSKILL_BULLET*  pBullet;
    //    int             nPhysicsDamage;         // 外功伤害点数
    //    int             nSolarDamage;           // 阳性内功伤害
    //    int             nNeutralDamage;         // 中性内功伤害
    //    int             nLunarDamage;           // 阴性内功伤害
    //    int             nPoisonDamage;          // 毒性伤害点数
    //    int             nStealLifePercent;      // 偷取对方生命点数
    //    int             nStealManaPercent;      // 偷取对方内力点数
    //    int             nTherapy;               // 治疗能力
    //    int             nGainThreatCoefficient;
    //    int             nImpairThreatCoefficient;
    //    BOOL            bDot;
    //};

    //KSKILL_ATTRIBUTE_PARAM*  m_pSkillAttributeParam;


    //KSKILL_CALCULATE_RESULT* m_pSrcSkillCalculateResult;
    //KSKILL_CALCULATE_RESULT* m_pDstSkillCalculateResult;

    //// 该结构用于抽象不同类型的伤害运算参数
    //struct KDAMAGE_CALCULATE_PARAM
    //{
    //    KCharacter*  pSkillSrc;
    //    BOOL         bCriticalStrikeFlag;           // 是否暴击
    //    int          nDamageValue;                  // 攻击数值
    //    int          nDstResistPercent;             // 抗性百分比
    //    int          nDstDefenceCoefficient;        // 防御系数
    //    int*         pnDstMaxTypedDamageAbsorb;     // 有类型的伤害吸收
    //    int*         pnDstMaxTypedDamageManaShield; // 有类型的ManaShield
    //    int          nDstPointReflection;           // 点数反弹
    //    int          nDstPercentReflection;         // 百分比反弹
    //    int          nSrcStealLifePercent;
    //    int          nSrcStealManaPercent;
    //    int*         pnSrcRecvTypeDamage;           // 对源的有类型伤害存放到这里
    //    int*         pnDstRecvTypeDamage;           // 对目标的有类型伤害存放到这里
    //};
    //
    //// 击退
    //BOOL ProcessKnockedBackExhale(int nDistance, int nVelocityXY);       // 水平方向上被击退(方向背向着技能释放者)   
    //BOOL ProcessKnockedBackConvergence(int nDistance, int nVelocityXY);  // 水平方向上被击退(方向向着技能释放者)  
    //BOOL ProcessKnockedBackRandom(int nDistance, int nVelocityXY);       // 水平方向上被击退(方向随机)   
    //BOOL ProcessKnockedBackSrcFace(int nDistance, int nVelocityXY);      // 水平方向上被击退(方向为技能释放者的朝向)       
    //// 击飞
    //BOOL ProcessKnockedOffParabola(int nDistance, int nVelocityXY);      // 呈抛物线被击飞(水平上的方向向着技能释放者)  
    //// 冲锋
    //BOOL ProcessDash(int nVelocityXY);
    //// 复活
    //BOOL ProcessRevive(int nLife, int nMana);
    //BOOL ProcessTherapy(int nTherapyValue);
    //BOOL ProcessDamage(int nDamageType, int nDamageValue);

    //BOOL ProcessBuff(KSKILL_BUFF_INFO* pBuff);          // 处理技能上带的BUFF 
    //BOOL ProcessBuff(int nID, int nLevel);              // 处理魔法属性调用的BUFF
    //BOOL ProcessDebuff(KSKILL_DEBUFF_INFO* pDebuff);
    //BOOL ProcessDebuff(int nID, int nLevel);
    //BOOL ProcessDot(KSKILL_DOT_INFO* pDot);
    //BOOL ProcessDot(int nID, int nLevel);
    //BOOL ProcessHot(KSKILL_HOT_INFO* pHot);
    //BOOL ProcessHot(int nID, int nLevel);
private:
    //BOOL CalculateDamage(const KDAMAGE_CALCULATE_PARAM &crParam);

    //BOOL ConcludeResult(DWORD dwBulletID, BOOL bCriticalStrikeFlag);

public:

    //KBuffList m_BuffList;
    //void ApplyDot(const KUSER_DOT_INFO& crDotInfo);
    //void ApplyHot(const KUSER_HOT_INFO& crHotInfo);
    //BOOL CancelBuff(int nStyle, int nIndex);

public:
 //   void OnOTActionBeatBack(int nFrame);
 //   void OnDamage(KCharacter* pAttackter, int nDamage);

	DWORD GetMapID();

	//BOOL CanDialog(KCharacter* pCharacter);
	//BOOL CanDoAction(KCharacter* pCharacter, BYTE byAction);

public:
	//脚本接口函数
    int LuaSetTarget(Lua_State* L);
	int LuaGetTarget(Lua_State* L);
    int LuaCastSkillXYZ(Lua_State* L);
	int LuaCastSkill(Lua_State* L);

	int LuaOpen(Lua_State* L);

	int LuaStop(Lua_State* L);
    int LuaStand(Lua_State* L);
	int LuaRunTo(Lua_State* L);
	int LuaWalkTo(Lua_State* L);
    int LuaSwimTo(Lua_State* L);
	int LuaJump(Lua_State* L);
	int LuaDash(Lua_State* L);

	int LuaPatrol(Lua_State* L);

	int LuaSitDown(Lua_State* L);
	int LuaDie(Lua_State* L);

	int LuaSay(Lua_State* L);
	int LuaTurnTo(Lua_State* L);
    int LuaPitchTo(Lua_State* L);
	int LuaDoAction(Lua_State* L);

	int LuaSetAIType(Lua_State* L);

    int LuaGetBuffList(Lua_State* L);
    int LuaGetDebuffList(Lua_State* L);
    int LuaGetDotList(Lua_State* L);
    int LuaGetHotList(Lua_State* L);

    int LuaCancelBuff(Lua_State* L);

    int LuaSetSheath(Lua_State* L);

	int LuaGetMapID(Lua_State* L);
	int LuaCanDialog(Lua_State* L);
};

//根据ID判断是否是玩家，玩家ID的高4位为0
inline BOOL g_IsPlayer(DWORD dwCharacterID)
{
	if (dwCharacterID <= 0x0FFFFFFF)
		return TRUE;
	else
		return FALSE;
}

int g_GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter);

#endif	//_KCHARACTER_H_