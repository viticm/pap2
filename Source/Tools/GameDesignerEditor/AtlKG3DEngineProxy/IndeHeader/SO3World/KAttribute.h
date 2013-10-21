/************************************************************************/
/* 影响其他对象的属性					                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.18	Create												*/
/************************************************************************/
#ifndef _KATTRIBUTE_H_
#define _KATTRIBUTE_H_
#include "Engine/KMemory.h"

enum ATTRIBUTE_TYPE
{
	atInvalid = 0,

    // ------------- 移动相关 ------------------------------------------>
    atRunSpeedBase,
    atMoveSpeedPercent,
    atMaxJumpCount,
    atGravityBase,
    atGravityPercent,
    atJumpSpeedBase,
    atJumpSpeedPercent,
    atDropDefence,
    atDivingFrameBase,
    atDivingFramePercent, 
    atWaterFlyAbility,

    // ------------- 潜能 ---------------------------------------------->
	atStrengthBase,
    atStrengthBasePercentAdd,
	atAgilityBase,				
    atAgilityBasePercentAdd,
	atVitalityBase,
    atVitalityBasePercentAdd,
	atSpiritBase,
    atSpiritBasePercentAdd,
    atSpunkBase,
    atSpunkBasePercentAdd,
    atTransmitTrain,
    atMaxTrainValue,
    atBaseVenationCof,
    atPhysicsVenationCof,
    atMagicVenationCof,
    atAssistVenationCof,
    atBasePotentialAdd,

    // ------------- 生命 ---------------------------------------------->
	atCurrentLife,

    atMaxLifeBase,
	atMaxLifePercentAdd,
    atMaxLifeAdditional,

    atLifeReplenish,
    atLifeReplenishPercent,
    atLifeReplenishCoefficient,
    atLifeReplenishExt,

    // ------------- 内力 ---------------------------------------------->
	atCurrentMana,				

	atMaxManaBase,			
    atMaxManaPercentAdd,			
    atMaxManaAdditional,

    atManaReplenish,
    atManaReplenishPercent,
    atManaReplenishCoefficient,
    atManaReplenishExt,

    // ------------- 精力 ---------------------------------------------->
	atCurrentStamina,				
    
    // ------------- 体力 ---------------------------------------------->
    atCurrentThew,				

    // -------------- 仇恨 --------------------------------------------->
    atActiveThreatCoefficient,
    atPassiveThreatCoefficient,

    // ------------- 战斗杂项 ------------------------------------------>
    atDodge,
    atDodgeBaseRate,
    atParryBaseRate,
    atParryBase,
    atParryPercent,
    atParryValueBase,
    atParryValuePercent,
    atSense,
    atStrainBase,
    atStrainPercent,
    atToughnessBaseRate,
    atToughnessBase,
    atToughnessPercent,
    atCriticalDamagePowerBaseKiloNumRate,
    atCriticalDamagePowerBase,
    atCriticalDamagePowerPercent,
    atDecriticalDamagePowerBaseKiloNumRate,
    atDecriticalDamagePowerBase,
    atDecriticalDamagePowerPercent,
    atPositiveShield,
    atNegativeShield,
    atGlobalResistPercent,
    atGlobalBlock,
    atGlobalDamageAbsorb,
    atGlobalDamageManaShield,
    atDamageToLifeForSelf,
    atDamageToManaForSelf,
    atBeatBackRate,
    atKnockedBackRate,
    atKnockedOffRate,

    atImmortal,
    atMountingRate,
    
    atMeleeWeaponDamageBase,
    atMeleeWeaponDamageRand,

    atRangeWeaponDamageBase,
    atRangeWeaponDamageRand,

    // ------------- 外功 ---------------------------------------------->
    atSkillPhysicsDamage,
    atSkillPhysicsDamageRand,
    atSkillPhysicsDamagePercent,
    atPhysicsDamageCoefficient,

    atPhysicsAttackPowerBase,
    atPhysicsAttackPowerPercent,
    
    atPhysicsHitBaseRate,
    atPhysicsHitValue,
    atPhysicsCriticalStrikeBaseRate,
    atPhysicsCriticalStrike,

    atPhysicsOvercomeBase,
    atPhysicsOvercomePercent,

    atPhysicsResistPercent,

    atPhysicsDefenceAdd,
    atPhysicsDefenceMax,

    atPhysicsShieldBase,
    atPhysicsShieldPercent,
    atPhysicsShieldAdditional,

    atPhysicsReflection,
    atPhysicsReflectionPercent,

    atPhysicsBlock,  

    atPhysicsDamageAbsorb,
    atPhysicsDamageManaShield,

    // ------------- 所有内功 ---------------------------------------------->
    atSkillMagicDamage,
    atMagicAttackPowerBase,
    atMagicAttackPowerPercent,
    atMagicCriticalStrike,
    atMagicShield,
    atMagicOvercome,

    // ------------- 阳性内功 ---------------------------------------------->
    atSkillSolarDamage,
    atSkillSolarDamageRand,
    atSkillSolarDamagePercent,
    atSolarDamageCoefficient,

    atSolarAttackPowerBase,
    atSolarAttackPowerPercent,

    atSolarHitBaseRate,
    atSolarHitValue,
    atSolarCriticalStrikeBaseRate,
    atSolarCriticalStrike,
    atSolarOvercomeBase,
    atSolarOvercomePercent,
    atSolarMagicResistPercent,
    atSolarMagicShieldBase,
    atSolarMagicShieldPercent,
    atSolarMagicReflection,
    atSolarMagicReflectionPercent,
    atSolarMagicBlock,
    atSolarDamageAbsorb,
    atSolarDamageManaShield,

    // ------------- 中性内功 ---------------------------------------------->
    atSkillNeutralDamage,
    atSkillNeutralDamageRand,
    atSkillNeutralDamagePercent,
    atNeutralDamageCoefficient,

    atNeutralAttackPowerBase,
    atNeutralAttackPowerPercent,

    atNeutralHitBaseRate,
    atNeutralHitValue,
    atNeutralCriticalStrikeBaseRate,
    atNeutralCriticalStrike,
    atNeutralOvercomeBase,
    atNeutralOvercomePercent,
    atNeutralMagicResistPercent,
    atNeutralMagicShieldBase,
    atNeutralMagicShieldPercent,
    atNeutralMagicReflection,
    atNeutralMagicReflectionPercent,
    atNeutralMagicBlock,
    atNeutralDamageAbsorb,
    atNeutralDamageManaShield,

    // ------------- 阴性内功 ---------------------------------------------->
    atSkillLunarDamage,
    atSkillLunarDamageRand,
    atSkillLunarDamagePercent,
    atLunarDamageCoefficient,

    atLunarAttackPowerBase,
    atLunarAttackPowerPercent,

    atLunarHitBaseRate,
    atLunarHitValue,
    atLunarCriticalStrikeBaseRate,
    atLunarCriticalStrike,
    atLunarOvercomeBase,
    atLunarOvercomePercent,
    atLunarMagicResistPercent,
    atLunarMagicShieldBase,
    atLunarMagicShieldPercent,
    atLunarMagicReflection,
    atLunarMagicReflectionPercent,
    atLunarMagicBlock,
    atLunarDamageAbsorb,
    atLunarDamageManaShield,

    // ------------- 毒性 -------------------------------------------------->
    atSkillPoisonDamage,
    atSkillPoisonDamageRand,
    atSkillPoisonDamagePercent,
    atPoisonDamageCoefficient,

    atPoisonAttackPowerBase,
    atPoisonAttackPowerPercent,

    atPoisonHitBaseRate,
    atPoisonHitValue,
    atPoisonCriticalStrikeBaseRate,
    atPoisonCriticalStrike,
    atPoisonOvercomeBase,
    atPoisonOvercomePercent,
    atPoisonMagicResistPercent,
    atPoisonMagicShieldBase,
    atPoisonMagicShieldPercent,
    atPoisonMagicReflection,
    atPoisonMagicReflectionPercent,
    atPoisonMagicBlock,
    atPoisonDamageAbsorb,
    atPoisonDamageManaShield,

    // ------------- 治疗 -------------------------------------------------->
    atTherapyPowerBase,    
    atTherapyPowerPercent,
    atSkillTherapy,
    atSkillTherapyRand,
    atSkillTherapyPercent, 
    atTherapyCoefficient,
    atBeTherapyCoefficient,

    // ------------- 新手技能有关 ------------------------------------------>
    atNoneWeaponAttackSpeedBase,
    atMeleeWeaponAttackSpeedBase,
    atMeleeWeaponAttackSpeedPercentAdd,
    atMeleeWeaponAttackSpeedAdditional,

    atRangeWeaponAttackSpeedBase,
    atRangeWeaponAttackSpeedPercentAdd,
    atRangeWeaponAttackSpeedAdditional,

    atIgnoreOtherAlarmRange,
    atSelfAlarmRangeRevise,     
    atOtherAlarmRangeRevise,
    atUnarm,                    // 缴械

    // -------------- 人物状态相关的属性 -------------------------------->
    atDash,         // 冲锋
    atDashForward,
    atDashBackward,
    atDashLeft,
    atDashRight,
    atHalt,         // 击晕
    atFreeze,       // 定身
    atEntrap,       // 不能移动
    atFear,         // 恐惧, 注意：该魔法属性只能用在DEBUFF，BUFF或作为技能上的要回滚的魔法属性
    atRevive,       // 复活
    atJump,         // 由技能发起的跳跃(而不是又空格操作引发的跳跃)
    atSit,          // 由技能发起的坐下

    // ------------ 击退 ---------------------------------------------------->
    atCallKnockedBackExhale,
    atCallKnockedBackConvergence,  
    atCallKnockedBackRandom,
    atCallKnockedBackSrcFace,
    
    // ------------- 击飞 --------------------------------------------------->
    atCallKnockedOffParabola,
    atCallKnockedDown,

    // ------------- 滑步 ---------------------------------------------------->
    atCallRepulsed,

    // ------------- 技能运算 ------------------------------------------------>
    atCallTherapy, 
    atCallPhysicsDamage,
    atCallSolarDamage,
    atCallNeutralDamage,
    atCallLunarDamage,
    atCallPoisonDamage,
    atCastSkillTargetSrc,
    atCastSkillTargetDst,

    atSkillEventHandler,

    // ------------ 特殊的技能魔法属性 --------------------------------------->
    atPracticeKungfu,       // 修炼武功
    atAccumulate,           // 聚气状态

    // ------------ Buff 处理有关 -------------------------------------------->
    atCallBuff,
    atCallBuffAccordingToAttraction,

    // ------------ Buff 驱散 ------------------------------------------------>
    atDetachSingleBuff,

    atDetachMultiGroupBuff,

    atDelSingleBuffByFunctionType,

    atDelMultiGroupBuffByFunctionType,

    atDelSingleBuffByIDAndLevel,
    atDelSingleGroupBuffByIDAndLevel,

    // ------------ 仇恨 ----------------------------------------------------->
    atModifyThreat,
    atSpoof,
    
    // ------------ 禁魔 ----------------------------------------------------->
    atSilence,
    atSilenceAll,

    // ------------ 免疫 ----------------------------------------------------->
    atImmunity,

    // ------------ 破绽 ----------------------------------------------------->
    atAddWeak,
    atShiftWeak,
    atAddWeakShield,

    // ------------ 技能条件标志 --------------------------------------------->
    atModifySkillCastFlag,

    // ------------ 执行脚本 ------------------------------------------------->
    atExecuteScript,
    
    // ------------ 化妆 ----------------------------------------------------->
    atPrinkRepresentID,
    atPrinkModel,    
    atPrinkForce,
    
    // ------------ 骑马 ----------------------------------------------------->
    atRideHorse,

    // ------------ 表情 ----------------------------------------------------->
    atDoAction,

    atAddExpPercent,
    atAddReputationPercent,

    atEmployerCastSkillTargetSelf,

    atClearCoolDown,
    atResetCoolDown,

    atTransferDamageToManaPercent,

    atModifyCostManaPercent,

    atDirectCastMask,

    atPull,
    atTurnToSrc,
    atConsumeBuff,
    atTopThreat,

    atFormationEffect,

    atAddDamageByDstMoveState,

    atTransferDamage,
    atTransferTherapy,

    atAddFellowshipAttraction,

    atCallHelp,
    atCallRequest,
    atCatchCharacter,
    atTransmission,

    // ----------------------------
    atVitalityToParryValueCof,
    atVitalityToMaxLifeCof,
    atVitalityToPhysicsAttackPowerCof,
    atVitalityToSolarAttackPowerCof,
    atVitalityToLunarAttackPowerCof,
    atVitalityToNeutralAttackPowerCof,
    atVitalityToPoisonAttackPowerCof,
    atVitalityToTherapyPowerCof,
    atVitalityToPhysicsShieldCof,
    atSpiritToMaxManaCof,
    atSpiritToSolarCriticalStrikeCof,
    atSpiritToLunarCriticalStrikeCof,
    atSpiritToNeutralCriticalStrikeCof,
    atSpiritToPoisonCriticalStrikeCof,
    atSpiritToPhysicsAttackPowerCof,
    atSpiritToPhysicsCriticalStrikeCof,
    atSpiritToManaReplenishCof,
    atSpiritToSolarMagicShieldCof,
    atSpiritToLunarMagicShieldCof,
    atSpiritToNeutralMagicShieldCof,
    atSpiritToPoisonMagicShieldCof,
    atSpunkToManaReplenishCof,
    atSpunkToSolarAttackPowerCof,
    atSpunkToLunarAttackPowerCof,
    atSpunkToNeutralAttackPowerCof,
    atSpunkToPoisonAttackPowerCof,
    atStrengthToPhysicsCriticalStrikeCof,
    atStrengthToPhysicsAttackPowerCof,
    atAgilityToPhysicsCriticalStrikeCof,
    // ----------------------------------

	atTotal
};

enum REQUIRE_TYPE
{
	rqtInvalid = 0,

	rqtVitality,
	rqtStrength,
	rqtSpirit,
	rqtAgility,
	rqtLevel,
	rqtFaction,
	rqtGender,
	rqtBodytype,

	rqtTotal
};

enum ATTRIBUTE_EFFECT_MODE
{
    aemInvalid,
    aemEffectToSelfAndRollback,
    aemEffectToSelfNotRollback,
    aemEffectToDestAndRollback,
    aemEffectToDestNotRollback,
    aemEffectToDestOnMinorWeak,
    aemEffectToDestOnMajorWeak,
    aemEffectToDestOnMaxWeak,
    aemTotal
};


struct KAttribData
{
	int				nKey;
	int				nValue1;
	int				nValue2;
};

struct KAttribute : KAttribData
{
	KAttribute*		pNext;
};

#define ADD_ATTRIBUTE_TO_END(pList, pNode)  \
    do                                      \
    {                                       \
        if (pList == NULL)                  \
        {                                   \
            pList = pNode;                  \
        }                                   \
        else                                \
        {                                   \
            KAttribute *_pNode = pList;     \
            while (_pNode->pNext)           \
            {                               \
                _pNode = _pNode->pNext;     \
            }                               \
            _pNode->pNext = pNode;          \
        }                                   \
    }                                       \
    while (false)

#define FREE_ATTRIB_LIST(pAttrib)                           \
    do                                                      \
    {                                                       \
        while (pAttrib)                                     \
        {                                                   \
            KAttribute *pTmp = pAttrib;                     \
            pAttrib = pAttrib->pNext;                       \
            KMemory::Delete(pTmp);                      \
            pTmp = NULL;                                    \
        }                                                   \
    } while (false)


BOOL AttributeStringToID(LPCTSTR szAttribute, int& nID);
BOOL RequireStringToID(LPCTSTR Require, int& nID);

KAttribute* CloneAttributeList(const KAttribute* pHead);
BOOL CloneAttribListToEnd(KAttribute** ppDestAttribList, KAttribute* pCloneAttribList);

#define KATTRIBUTE_STRING_LEN                   64

#endif	//_KATTRIBUTE_H_
