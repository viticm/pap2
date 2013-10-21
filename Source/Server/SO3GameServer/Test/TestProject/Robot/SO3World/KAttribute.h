/************************************************************************/
/* 影响其他对象的属性					                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.18	Create												*/
/************************************************************************/
#ifndef _KATTRIBUTE_H_
#define _KATTRIBUTE_H_


enum ATTRIBUTE_TYPE
{
	atInvalid = 0,

    // ------------- 移动相关 ------------------------------------------>
    atWalkSpeedBase,
    atRunSpeedBase,
    atSwimSpeedBase,
    atMoveSpeedPercent,
    atMaxJumpCount,
    atGravityBase,
    atGravityPercent,
    atJumpSpeedXYBase,
    atJumpSpeedXYPercent,
    atJumpSpeedZBase,
    atJumpSpeedZPercent,
    atDropDefence,
    atDivingFrameBase,
    atDivingFramePercent, 
    atSwimAbility,      
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

    // ------------- 生命 ---------------------------------------------->

	atCurrentLife,				

    atMaxLifeBase,              
	atMaxLifePercentAdd,		
    atMaxLifeAdditional,      

    atLifeReplenish,
    atLifeReplenishPercent,		

    // ------------- 内力 ---------------------------------------------->

	atCurrentMana,				

	atMaxManaBase,			
    atMaxManaPercentAdd,			
    atMaxManaAdditional,

    atManaReplenish,
    atManaReplenishPercent,

    // ------------- 体力 ---------------------------------------------->

	atCurrentStamina,				

    atMaxStaminaBase,              
	atMaxStaminaPercentAdd,		
    atMaxStaminaAdditional,      

    atStaminaReplenish,
    atStaminaReplenishPercent,	

    atQiToSkillExpPercentAdd,

    // ------------- 怒气 ---------------------------------------------->

	atCurrentRage,				

    atMaxRageBase,              
	atMaxRagePercentAdd,		
    atMaxRageAdditional,      

    atRageReplenish,
    atRageReplenishPercent,		

    // -------------- 仇恨 --------------------------------------------->
    atGainPositiveThreatCoefficient,
    atGainNegativeThreatCoefficient,
    atImpairPositiveThreatCoefficient,
    atImpairNegativeThreatCoefficient,

    // ------------- 战斗杂项 ------------------------------------------>
    atDodge,
    atPositiveShield,
    atNegativeShield,
    atGlobalBlock,
    atGlobalDamageAbsorb,
    atGlobalDamageManaShield,
    atStealLifePercent,
    atStealManaPercent,
    atBeatBackRate,
    atKnockedBackRate,
    atKnockedOffRate,
    
    atPhysicsAttackPower,
    atMagicAttackPower,

    atMeleeWeaponDamageBase,
    atMeleeWeaponDamageRand,

    atRangeWeaponDamageBase,
    atRangeWeaponDamageRand,

    // ------------- 外功 ---------------------------------------------->
    
    atSkillPhysicsDamage,
    atSkillPhysicsDamageRand,
    atSkillPhysicsDamagePercent,
    atPhysicsAttackPowerBase,
    atPhysicsDamageCoefficient,

    
    atPhysicsAttackHit,
    atPhysicsCriticalStrike,

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
    // ------------- 内功 -------------------------------------------------->
    atSpunkAttackPower,
    atMeleeSpunkAttackPower,
    atRangeSpunkAttackPower,

    // ------------- 阳性内功 ---------------------------------------------->
    atSkillSolarDamage,
    atSkillSolarDamageRand,
    atSkillSolarDamagePercent,
    atSolarAttackPowerBase,
    atSolarDamageCoefficient,

    atSolarMagicHit,
    atSolarCriticalStrike,
    atSolarMagicResistPercent,
    atSolarMagicShield,
    atSolarMagicReflection,
    atSolarMagicReflectionPercent,
    atSolarMagicBlock,
    atSolarDamageAbsorb,
    atSolarDamageManaShield,

    // ------------- 中性内功 ---------------------------------------------->
    atSkillNeutralDamage,
    atSkillNeutralDamageRand,
    atSkillNeutralDamagePercent,
    atNeutralAttackPowerBase,
    atNeutralDamageCoefficient,

    atNeutralMagicHit,
    atNeutralCriticalStrike,
    atNeutralMagicResistPercent,
    atNeutralMagicShield,
    atNeutralMagicReflection,
    atNeutralMagicReflectionPercent,
    atNeutralMagicBlock,
    atNeutralDamageAbsorb,
    atNeutralDamageManaShield,

    // ------------- 阴性内功 ---------------------------------------------->
    atSkillLunarDamage,
    atSkillLunarDamageRand,
    atSkillLunarDamagePercent,
    atLunarAttackPowerBase,
    atLunarDamageCoefficient,

    atLunarMagicHit,
    atLunarCriticalStrike,
    atLunarMagicResistPercent,
    atLunarMagicShield,
    atLunarMagicReflection,
    atLunarMagicReflectionPercent,
    atLunarMagicBlock,
    atLunarDamageAbsorb,
    atLunarDamageManaShield,

    // ------------- 毒性 -------------------------------------------------->
    atSkillPoisonDamage,
    atSkillPoisonDamageRand,
    atSkillPoisonDamagePercent,
    atPoisonAttackPowerBase,
    atPoisonDamageCoefficient,

    atPoisonMagicHit,
    atPoisonCriticalStrike,
    atPoisonMagicResistPercent,
    atPoisonMagicShield,
    atPoisonMagicReflection,
    atPoisonMagicReflectionPercent,
    atPoisonMagicBlock,
    atPoisonDamageAbsorb,
    atPoisonDamageManaShield,

    // ------------- 治疗 -------------------------------------------------->
    atTherapyPowerBase,    
    atSkillTherapy,
    atSkillTherapyRand,
    atSkillTherapyPercent, 
    atTherapyCoefficient,

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

    // -------------- 人物状态相关的属性 -------------------------------->
    atMeditate,     // 打坐
    atDash,         // 冲锋
    atStun,         // 击晕, 眩晕
    atHalt,         // 定身，就是人物不能移动，其他操作正常，如使用道具等  
    atRevive,       // 复活
    atJump,         // 由技能发起的跳跃(而不是又空格操作引发的跳跃)
    atSit,          // 有技能发起的坐下
    // ------------ 击退 ---------------------------------------------------->
    atCallKnockedBackExhale,
    atCallKnockedBackConvergence,  
    atCallKnockedBackRandom,
    atCallKnockedBackSrcFace,
    
    // ------------- 击飞 --------------------------------------------------->
    atCallKnockedOffParabola,
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

    // ------------ Buff 处理有关 -------------------------------------------->
    atCallBuff,
    atCallDebuff,
    atCallDot,
    atCallHot,

    // ------------ Buff 驱散 ------------------------------------------------>
    atDetachSingleBuff,
    atDetachSingleDebuff,
    atDetachSingleGroupDot,
    atDetachSingleGroupHot,
    
    atDetachMultiGroupBuff,
    atDetachMultiGroupDebuff,
    atDetachMultiGroupDot,
    atDetachMultiGroupHot,

    // ------------ 仇恨 ----------------------------------------------------->
    atModifyThreat,
    
    // ------------ 执行脚本 ------------------------------------------------->
    atExecuteScript,

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

	rqtTotal,
};

enum ATTRIBUTE_EFFECT_MODE
{
    aemInvalid,
    aemEffectToSelfAndRollback,
    aemEffectToSelfNotRollback,
    aemEffectToDestAndRollback,
    aemEffectToDestNotRollback,
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
        if (!pList)                         \
        {                                   \
            pList = pNode;                  \
            pNode->pNext = NULL;            \
        }                                   \
        else                                \
        {                                   \
            KAttribute *_pNode = pList;     \
            while (_pNode->pNext)           \
            {                               \
                _pNode = _pNode->pNext;     \
            }                               \
            _pNode->pNext = pNode;          \
            pNode->pNext = NULL;            \
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
            g_ObjectManager.m_AttributeSet.Delete(pTmp);    \
            pTmp = NULL;                                    \
        }                                                   \
    } while (false)


#define MAKE_IN_RANGE(Value, MinValue, MaxValue)        \
    do                                                  \
    {                                                   \
        ASSERT(MinValue <= MaxValue);                   \
        if ((Value) > (MaxValue)) (Value) = (MaxValue); \
        if ((Value) < (MinValue)) (Value) = (MinValue); \
    }                                                   \
    while (false)



BOOL AttributeStringToID(LPCTSTR szAttribute, int& nID);
BOOL RequireStringToID(LPCTSTR Require, int& nID);

#endif	//_KATTRIBUTE_H_