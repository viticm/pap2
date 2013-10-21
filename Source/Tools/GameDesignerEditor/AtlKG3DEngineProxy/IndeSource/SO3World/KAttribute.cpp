#include "stdafx.h"
#include "KAttribute.h"
#include "StrMap.h"
#include "KSO3World.h"
#include "Engine/KMemory.h"

// 字符串映射表，这样配置表中即可以直接填写字符串，而不用填枚举的数字
DECLARE_STRING_MAP_BEGIN(ATTRIBUTE_TYPE)

	REGISTER_MAP_VALUE(atInvalid)

    // ------------- 移动相关 ------------------------------------------>
	REGISTER_MAP_VALUE(atRunSpeedBase)
	REGISTER_MAP_VALUE(atMoveSpeedPercent)
	REGISTER_MAP_VALUE(atMaxJumpCount)
    REGISTER_MAP_VALUE(atGravityBase)
    REGISTER_MAP_VALUE(atGravityPercent)
    REGISTER_MAP_VALUE(atJumpSpeedBase)
    REGISTER_MAP_VALUE(atJumpSpeedPercent)
	REGISTER_MAP_VALUE(atDropDefence)
    REGISTER_MAP_VALUE(atDivingFrameBase)
    REGISTER_MAP_VALUE(atDivingFramePercent) 
    REGISTER_MAP_VALUE(atWaterFlyAbility)

    // ------------- 潜能 ---------------------------------------------->

	REGISTER_MAP_VALUE(atStrengthBase)
    REGISTER_MAP_VALUE(atStrengthBasePercentAdd)
	REGISTER_MAP_VALUE(atAgilityBase)
    REGISTER_MAP_VALUE(atAgilityBasePercentAdd)
	REGISTER_MAP_VALUE(atVitalityBase)
    REGISTER_MAP_VALUE(atVitalityBasePercentAdd)
	REGISTER_MAP_VALUE(atSpiritBase)
    REGISTER_MAP_VALUE(atSpiritBasePercentAdd)
    REGISTER_MAP_VALUE(atSpunkBase)
    REGISTER_MAP_VALUE(atSpunkBasePercentAdd)
    REGISTER_MAP_VALUE(atTransmitTrain)
    REGISTER_MAP_VALUE(atMaxTrainValue)
    REGISTER_MAP_VALUE(atBaseVenationCof)
    REGISTER_MAP_VALUE(atPhysicsVenationCof)
    REGISTER_MAP_VALUE(atMagicVenationCof)
    REGISTER_MAP_VALUE(atAssistVenationCof)
    REGISTER_MAP_VALUE(atBasePotentialAdd)

    // ------------- 生命 ---------------------------------------------->

    REGISTER_MAP_VALUE(atCurrentLife)

    REGISTER_MAP_VALUE(atMaxLifeBase)
	REGISTER_MAP_VALUE(atMaxLifePercentAdd)
    REGISTER_MAP_VALUE(atMaxLifeAdditional)

    REGISTER_MAP_VALUE(atLifeReplenish)
    REGISTER_MAP_VALUE(atLifeReplenishPercent)
    REGISTER_MAP_VALUE(atLifeReplenishCoefficient)
    REGISTER_MAP_VALUE(atLifeReplenishExt)

    // ------------- 内力 ---------------------------------------------->

	REGISTER_MAP_VALUE(atCurrentMana)

	REGISTER_MAP_VALUE(atMaxManaBase)
    REGISTER_MAP_VALUE(atMaxManaPercentAdd)
    REGISTER_MAP_VALUE(atMaxManaAdditional)

    REGISTER_MAP_VALUE(atManaReplenish)
    REGISTER_MAP_VALUE(atManaReplenishPercent)
    REGISTER_MAP_VALUE(atManaReplenishCoefficient)
    REGISTER_MAP_VALUE(atManaReplenishExt)

    // ------------- 精力 ---------------------------------------------->

	REGISTER_MAP_VALUE(atCurrentStamina)				

    // ------------- 体力 ---------------------------------------------->

    REGISTER_MAP_VALUE(atCurrentThew)	

    // ------------- 仇恨 ---------------------------------------------->
    REGISTER_MAP_VALUE(atActiveThreatCoefficient)
    REGISTER_MAP_VALUE(atPassiveThreatCoefficient)

    // ------------- 战斗杂项 ------------------------------------------>
    REGISTER_MAP_VALUE(atDodge)    
    REGISTER_MAP_VALUE(atDodgeBaseRate)
    REGISTER_MAP_VALUE(atParryBaseRate)
    REGISTER_MAP_VALUE(atParryBase)
    REGISTER_MAP_VALUE(atParryPercent)
    REGISTER_MAP_VALUE(atParryValueBase)
    REGISTER_MAP_VALUE(atParryValuePercent)
    REGISTER_MAP_VALUE(atSense)
    REGISTER_MAP_VALUE(atStrainBase)
    REGISTER_MAP_VALUE(atStrainPercent)
    REGISTER_MAP_VALUE(atToughnessBaseRate)
    REGISTER_MAP_VALUE(atToughnessBase)
    REGISTER_MAP_VALUE(atToughnessPercent)
    REGISTER_MAP_VALUE(atDecriticalDamagePowerBaseKiloNumRate)
    REGISTER_MAP_VALUE(atCriticalDamagePowerBase)
    REGISTER_MAP_VALUE(atCriticalDamagePowerPercent)
    REGISTER_MAP_VALUE(atDecriticalDamagePowerBaseKiloNumRate)
    REGISTER_MAP_VALUE(atDecriticalDamagePowerBase)
    REGISTER_MAP_VALUE(atDecriticalDamagePowerPercent)
    REGISTER_MAP_VALUE(atPositiveShield)
    REGISTER_MAP_VALUE(atNegativeShield)
    REGISTER_MAP_VALUE(atGlobalResistPercent)
    REGISTER_MAP_VALUE(atGlobalBlock)
    REGISTER_MAP_VALUE(atGlobalDamageAbsorb)
    REGISTER_MAP_VALUE(atGlobalDamageManaShield)
    REGISTER_MAP_VALUE(atDamageToLifeForSelf)
    REGISTER_MAP_VALUE(atDamageToManaForSelf)
    REGISTER_MAP_VALUE(atBeatBackRate)
    REGISTER_MAP_VALUE(atKnockedBackRate)
    REGISTER_MAP_VALUE(atKnockedOffRate)

    REGISTER_MAP_VALUE(atImmortal)
    REGISTER_MAP_VALUE(atMountingRate)

    REGISTER_MAP_VALUE(atMeleeWeaponDamageBase)
    REGISTER_MAP_VALUE(atMeleeWeaponDamageRand)

    REGISTER_MAP_VALUE(atRangeWeaponDamageBase)
    REGISTER_MAP_VALUE(atRangeWeaponDamageRand)

    // ------------- 外功 ---------------------------------------------->

    REGISTER_MAP_VALUE(atSkillPhysicsDamage)
    REGISTER_MAP_VALUE(atSkillPhysicsDamageRand)
    REGISTER_MAP_VALUE(atSkillPhysicsDamagePercent)
    REGISTER_MAP_VALUE(atPhysicsDamageCoefficient)

    REGISTER_MAP_VALUE(atPhysicsAttackPowerBase)
    REGISTER_MAP_VALUE(atPhysicsAttackPowerPercent)

    REGISTER_MAP_VALUE(atPhysicsHitBaseRate)
    REGISTER_MAP_VALUE(atPhysicsHitValue)
    REGISTER_MAP_VALUE(atPhysicsCriticalStrikeBaseRate)
    REGISTER_MAP_VALUE(atPhysicsCriticalStrike)
    REGISTER_MAP_VALUE(atPhysicsOvercomeBase)
    REGISTER_MAP_VALUE(atPhysicsOvercomePercent)

    REGISTER_MAP_VALUE(atPhysicsResistPercent)

    REGISTER_MAP_VALUE(atPhysicsDefenceAdd)
    REGISTER_MAP_VALUE(atPhysicsDefenceMax)

    REGISTER_MAP_VALUE(atPhysicsShieldBase)
    REGISTER_MAP_VALUE(atPhysicsShieldPercent)
    REGISTER_MAP_VALUE(atPhysicsShieldAdditional)

    REGISTER_MAP_VALUE(atPhysicsReflection)
    REGISTER_MAP_VALUE(atPhysicsReflectionPercent)

    REGISTER_MAP_VALUE(atPhysicsBlock)  

    REGISTER_MAP_VALUE(atPhysicsDamageAbsorb)
    REGISTER_MAP_VALUE(atPhysicsDamageManaShield)

    // ------------- 所有内功 --------------------------------------------->
    REGISTER_MAP_VALUE(atSkillMagicDamage)
    REGISTER_MAP_VALUE(atMagicAttackPowerBase)
    REGISTER_MAP_VALUE(atMagicAttackPowerPercent)
    REGISTER_MAP_VALUE(atMagicCriticalStrike)
    REGISTER_MAP_VALUE(atMagicShield)
    REGISTER_MAP_VALUE(atMagicOvercome)

    // ------------- 阳性内功 ---------------------------------------------->
    REGISTER_MAP_VALUE(atSkillSolarDamage)
    REGISTER_MAP_VALUE(atSkillSolarDamageRand)
    REGISTER_MAP_VALUE(atSkillSolarDamagePercent)
    REGISTER_MAP_VALUE(atSolarDamageCoefficient)

    REGISTER_MAP_VALUE(atSolarAttackPowerBase)
    REGISTER_MAP_VALUE(atSolarAttackPowerPercent)

    REGISTER_MAP_VALUE(atSolarHitBaseRate)
    REGISTER_MAP_VALUE(atSolarHitValue)
    REGISTER_MAP_VALUE(atSolarCriticalStrikeBaseRate)
    REGISTER_MAP_VALUE(atSolarCriticalStrike)
    REGISTER_MAP_VALUE(atSolarOvercomeBase)
    REGISTER_MAP_VALUE(atSolarOvercomePercent)
    REGISTER_MAP_VALUE(atSolarMagicResistPercent)
    REGISTER_MAP_VALUE(atSolarMagicShieldBase)
    REGISTER_MAP_VALUE(atSolarMagicShieldPercent)
    REGISTER_MAP_VALUE(atSolarMagicReflection)
    REGISTER_MAP_VALUE(atSolarMagicReflectionPercent)
    REGISTER_MAP_VALUE(atSolarMagicBlock)
    REGISTER_MAP_VALUE(atSolarDamageAbsorb)
    REGISTER_MAP_VALUE(atSolarDamageManaShield)

    // ------------- 中性内功 ---------------------------------------------->
    REGISTER_MAP_VALUE(atSkillNeutralDamage)
    REGISTER_MAP_VALUE(atSkillNeutralDamageRand)
    REGISTER_MAP_VALUE(atSkillNeutralDamagePercent)
    REGISTER_MAP_VALUE(atNeutralDamageCoefficient)

    REGISTER_MAP_VALUE(atNeutralAttackPowerBase)
    REGISTER_MAP_VALUE(atNeutralAttackPowerPercent)

    REGISTER_MAP_VALUE(atNeutralHitBaseRate)
    REGISTER_MAP_VALUE(atNeutralHitValue)
    REGISTER_MAP_VALUE(atNeutralCriticalStrikeBaseRate)
    REGISTER_MAP_VALUE(atNeutralCriticalStrike)
    REGISTER_MAP_VALUE(atNeutralOvercomeBase)
    REGISTER_MAP_VALUE(atNeutralOvercomePercent)
    REGISTER_MAP_VALUE(atNeutralMagicResistPercent)
    REGISTER_MAP_VALUE(atNeutralMagicShieldBase)
    REGISTER_MAP_VALUE(atNeutralMagicShieldPercent)
    REGISTER_MAP_VALUE(atNeutralMagicReflection)
    REGISTER_MAP_VALUE(atNeutralMagicReflectionPercent)
    REGISTER_MAP_VALUE(atNeutralMagicBlock)
    REGISTER_MAP_VALUE(atNeutralDamageAbsorb)
    REGISTER_MAP_VALUE(atNeutralDamageManaShield)

    // ------------- 阴性内功 ---------------------------------------------->
    REGISTER_MAP_VALUE(atSkillLunarDamage)
    REGISTER_MAP_VALUE(atSkillLunarDamageRand)
    REGISTER_MAP_VALUE(atSkillLunarDamagePercent)
    REGISTER_MAP_VALUE(atLunarDamageCoefficient)

    REGISTER_MAP_VALUE(atLunarAttackPowerBase)
    REGISTER_MAP_VALUE(atLunarAttackPowerPercent)

    REGISTER_MAP_VALUE(atLunarHitBaseRate)
    REGISTER_MAP_VALUE(atLunarHitValue)
    REGISTER_MAP_VALUE(atLunarCriticalStrikeBaseRate)
    REGISTER_MAP_VALUE(atLunarCriticalStrike)
    REGISTER_MAP_VALUE(atLunarOvercomeBase)
    REGISTER_MAP_VALUE(atLunarOvercomePercent)
    REGISTER_MAP_VALUE(atLunarMagicResistPercent)
    REGISTER_MAP_VALUE(atLunarMagicShieldBase)
    REGISTER_MAP_VALUE(atLunarMagicShieldPercent)
    REGISTER_MAP_VALUE(atLunarMagicReflection)
    REGISTER_MAP_VALUE(atLunarMagicReflectionPercent)
    REGISTER_MAP_VALUE(atLunarMagicBlock)
    REGISTER_MAP_VALUE(atLunarDamageAbsorb)
    REGISTER_MAP_VALUE(atLunarDamageManaShield)

    // ------------- 毒性 -------------------------------------------------->
    REGISTER_MAP_VALUE(atSkillPoisonDamage)
    REGISTER_MAP_VALUE(atSkillPoisonDamageRand)
    REGISTER_MAP_VALUE(atSkillPoisonDamagePercent)
    REGISTER_MAP_VALUE(atPoisonDamageCoefficient)

    REGISTER_MAP_VALUE(atPoisonAttackPowerBase)
    REGISTER_MAP_VALUE(atPoisonAttackPowerPercent)

    REGISTER_MAP_VALUE(atPoisonHitBaseRate)
    REGISTER_MAP_VALUE(atPoisonHitValue)
    REGISTER_MAP_VALUE(atPoisonCriticalStrikeBaseRate)
    REGISTER_MAP_VALUE(atPoisonCriticalStrike)
    REGISTER_MAP_VALUE(atPoisonOvercomeBase)
    REGISTER_MAP_VALUE(atPoisonOvercomePercent)
    REGISTER_MAP_VALUE(atPoisonMagicResistPercent)
    REGISTER_MAP_VALUE(atPoisonMagicShieldBase)
    REGISTER_MAP_VALUE(atPoisonMagicShieldPercent)
    REGISTER_MAP_VALUE(atPoisonMagicReflection)
    REGISTER_MAP_VALUE(atPoisonMagicReflectionPercent)
    REGISTER_MAP_VALUE(atPoisonMagicBlock)
    REGISTER_MAP_VALUE(atPoisonDamageAbsorb)
    REGISTER_MAP_VALUE(atPoisonDamageManaShield)

    // ------------- 治疗 -------------------------------------------------->
    REGISTER_MAP_VALUE(atTherapyPowerBase)    
    REGISTER_MAP_VALUE(atTherapyPowerPercent)    
    REGISTER_MAP_VALUE(atSkillTherapy)
    REGISTER_MAP_VALUE(atSkillTherapyRand)
    REGISTER_MAP_VALUE(atSkillTherapyPercent) 
    REGISTER_MAP_VALUE(atTherapyCoefficient)
    REGISTER_MAP_VALUE(atBeTherapyCoefficient)

    // ------------- 新手技能有关 ------------------------------------------->
    REGISTER_MAP_VALUE(atNoneWeaponAttackSpeedBase)
    REGISTER_MAP_VALUE(atMeleeWeaponAttackSpeedBase)
    REGISTER_MAP_VALUE(atMeleeWeaponAttackSpeedPercentAdd)
    REGISTER_MAP_VALUE(atMeleeWeaponAttackSpeedAdditional)

    REGISTER_MAP_VALUE(atRangeWeaponAttackSpeedBase)
    REGISTER_MAP_VALUE(atRangeWeaponAttackSpeedPercentAdd)
    REGISTER_MAP_VALUE(atRangeWeaponAttackSpeedAdditional)

    // ----------------------------------------------------------------------
    REGISTER_MAP_VALUE(atIgnoreOtherAlarmRange)
    REGISTER_MAP_VALUE(atSelfAlarmRangeRevise)
    REGISTER_MAP_VALUE(atOtherAlarmRangeRevise)
    REGISTER_MAP_VALUE(atUnarm)

    // ---------------- 人物状态相关的属性 --------------------------------->
    REGISTER_MAP_VALUE(atDash)
    REGISTER_MAP_VALUE(atDashForward)
    REGISTER_MAP_VALUE(atDashBackward)
    REGISTER_MAP_VALUE(atDashLeft)
    REGISTER_MAP_VALUE(atDashRight)
    REGISTER_MAP_VALUE(atHalt)       
    REGISTER_MAP_VALUE(atFreeze)
    REGISTER_MAP_VALUE(atEntrap)
    REGISTER_MAP_VALUE(atFear)       
    REGISTER_MAP_VALUE(atRevive)
    REGISTER_MAP_VALUE(atJump)
    REGISTER_MAP_VALUE(atSit)

    // ------------- 击退 -------------------------------------------------->
    REGISTER_MAP_VALUE(atCallKnockedBackExhale)
    REGISTER_MAP_VALUE(atCallKnockedBackConvergence) 
    REGISTER_MAP_VALUE(atCallKnockedBackRandom)
    REGISTER_MAP_VALUE(atCallKnockedBackSrcFace)

    // ------------- 击飞 -------------------------------------------------->
    REGISTER_MAP_VALUE(atCallKnockedOffParabola)
    REGISTER_MAP_VALUE(atCallKnockedDown)
    // ------------- 滑步 -------------------------------------------------->
    REGISTER_MAP_VALUE(atCallRepulsed)
    // ------------- 技能运算 ------------------------------------------------>
    REGISTER_MAP_VALUE(atCallTherapy) 
    REGISTER_MAP_VALUE(atCallPhysicsDamage)
    REGISTER_MAP_VALUE(atCallSolarDamage)
    REGISTER_MAP_VALUE(atCallNeutralDamage)
    REGISTER_MAP_VALUE(atCallLunarDamage)
    REGISTER_MAP_VALUE(atCallPoisonDamage)
    REGISTER_MAP_VALUE(atCastSkillTargetSrc)
    REGISTER_MAP_VALUE(atCastSkillTargetDst)
    REGISTER_MAP_VALUE(atSkillEventHandler)

    // ------------ 特殊的技能魔法属性 --------------------------------------->
    REGISTER_MAP_VALUE(atPracticeKungfu)
    REGISTER_MAP_VALUE(atAccumulate)

    // ------------ Buff 处理有关 -------------------------------------------->
    REGISTER_MAP_VALUE(atCallBuff)
    REGISTER_MAP_VALUE(atCallBuffAccordingToAttraction)

    // ------------- Buff 驱散 ---------------------------------------------->
    REGISTER_MAP_VALUE(atDetachSingleBuff)

    REGISTER_MAP_VALUE(atDetachMultiGroupBuff)

    REGISTER_MAP_VALUE(atDelSingleBuffByFunctionType)

    REGISTER_MAP_VALUE(atDelMultiGroupBuffByFunctionType)

    REGISTER_MAP_VALUE(atDelSingleBuffByIDAndLevel)
    REGISTER_MAP_VALUE(atDelSingleGroupBuffByIDAndLevel)

    // ------------- 仇恨 --------------------------------------------------->
    REGISTER_MAP_VALUE(atModifyThreat)
    REGISTER_MAP_VALUE(atSpoof)

    // ------------ 禁魔 ----------------------------------------------------->
    REGISTER_MAP_VALUE(atSilence)
    REGISTER_MAP_VALUE(atSilenceAll)

    // ------------ 免疫 ----------------------------------------------------->
    REGISTER_MAP_VALUE(atImmunity)

    // ------------ 破绽 ----------------------------------------------------->
    REGISTER_MAP_VALUE(atAddWeak)
    REGISTER_MAP_VALUE(atShiftWeak)
    REGISTER_MAP_VALUE(atAddWeakShield)

    // ------------ 技能条件标志 --------------------------------------------->
    REGISTER_MAP_VALUE(atModifySkillCastFlag)

    // ------------- 执行脚本 ----------------------------------------------->
    REGISTER_MAP_VALUE(atExecuteScript)

    // ------------ 化妆 ----------------------------------------------------->
    REGISTER_MAP_VALUE(atPrinkRepresentID)
    REGISTER_MAP_VALUE(atPrinkModel)    
    REGISTER_MAP_VALUE(atPrinkForce)

    // -------------- 骑马或下马 --------------------------------------------->
    REGISTER_MAP_VALUE(atRideHorse)
    REGISTER_MAP_VALUE(atDoAction)

    REGISTER_MAP_VALUE(atAddExpPercent)
    REGISTER_MAP_VALUE(atAddReputationPercent)

    REGISTER_MAP_VALUE(atEmployerCastSkillTargetSelf)
    REGISTER_MAP_VALUE(atClearCoolDown)
    REGISTER_MAP_VALUE(atResetCoolDown)

    REGISTER_MAP_VALUE(atTransferDamageToManaPercent)

    REGISTER_MAP_VALUE(atModifyCostManaPercent)

    REGISTER_MAP_VALUE(atDirectCastMask)
    
    REGISTER_MAP_VALUE(atPull)
    REGISTER_MAP_VALUE(atTurnToSrc)
    REGISTER_MAP_VALUE(atConsumeBuff)
    REGISTER_MAP_VALUE(atTopThreat)
    REGISTER_MAP_VALUE(atFormationEffect)
    REGISTER_MAP_VALUE(atAddDamageByDstMoveState)

    REGISTER_MAP_VALUE(atTransferDamage)
    REGISTER_MAP_VALUE(atTransferTherapy)

    REGISTER_MAP_VALUE(atAddFellowshipAttraction)
    REGISTER_MAP_VALUE(atCallHelp)
    REGISTER_MAP_VALUE(atCallRequest)

    REGISTER_MAP_VALUE(atCatchCharacter)
    REGISTER_MAP_VALUE(atTransmission)

    // ---------------------------------------------------------
    REGISTER_MAP_VALUE(atVitalityToParryValueCof)
    REGISTER_MAP_VALUE(atVitalityToMaxLifeCof)
    REGISTER_MAP_VALUE(atVitalityToPhysicsAttackPowerCof)
    REGISTER_MAP_VALUE(atVitalityToSolarAttackPowerCof)
    REGISTER_MAP_VALUE(atVitalityToLunarAttackPowerCof)
    REGISTER_MAP_VALUE(atVitalityToNeutralAttackPowerCof)
    REGISTER_MAP_VALUE(atVitalityToPoisonAttackPowerCof)
    REGISTER_MAP_VALUE(atVitalityToTherapyPowerCof)
    REGISTER_MAP_VALUE(atVitalityToPhysicsShieldCof)
    REGISTER_MAP_VALUE(atSpiritToMaxManaCof)
    REGISTER_MAP_VALUE(atSpiritToSolarCriticalStrikeCof)
    REGISTER_MAP_VALUE(atSpiritToLunarCriticalStrikeCof)
    REGISTER_MAP_VALUE(atSpiritToNeutralCriticalStrikeCof)
    REGISTER_MAP_VALUE(atSpiritToPoisonCriticalStrikeCof)
    REGISTER_MAP_VALUE(atSpiritToPhysicsAttackPowerCof)
    REGISTER_MAP_VALUE(atSpiritToPhysicsCriticalStrikeCof)
    REGISTER_MAP_VALUE(atSpiritToManaReplenishCof)
    REGISTER_MAP_VALUE(atSpiritToSolarMagicShieldCof)
    REGISTER_MAP_VALUE(atSpiritToLunarMagicShieldCof)
    REGISTER_MAP_VALUE(atSpiritToNeutralMagicShieldCof)
    REGISTER_MAP_VALUE(atSpiritToPoisonMagicShieldCof)
    REGISTER_MAP_VALUE(atSpunkToManaReplenishCof)
    REGISTER_MAP_VALUE(atSpunkToSolarAttackPowerCof)
    REGISTER_MAP_VALUE(atSpunkToLunarAttackPowerCof)
    REGISTER_MAP_VALUE(atSpunkToNeutralAttackPowerCof)
    REGISTER_MAP_VALUE(atSpunkToPoisonAttackPowerCof)
    REGISTER_MAP_VALUE(atStrengthToPhysicsCriticalStrikeCof)
    REGISTER_MAP_VALUE(atStrengthToPhysicsAttackPowerCof)
    REGISTER_MAP_VALUE(atAgilityToPhysicsCriticalStrikeCof)
    // -----------------------------------------------------------

DECLARE_STRING_MAP_END(ATTRIBUTE_TYPE)

DECLARE_STRING_MAP_BEGIN(REQUIRE_TYPE)

	REGISTER_MAP_VALUE(rqtInvalid)

	REGISTER_MAP_VALUE(rqtVitality)
	REGISTER_MAP_VALUE(rqtStrength)
	REGISTER_MAP_VALUE(rqtSpirit)
	REGISTER_MAP_VALUE(rqtAgility)
	REGISTER_MAP_VALUE(rqtLevel)
	REGISTER_MAP_VALUE(rqtFaction)
	REGISTER_MAP_VALUE(rqtGender)
	REGISTER_MAP_VALUE(rqtBodytype)

	REGISTER_MAP_VALUE(rqtTotal)

DECLARE_STRING_MAP_END(REQUIRE_TYPE)



BOOL AttributeStringToID(LPCTSTR szAttribute, int& nID)
{
	MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szAttribute, nID);

	return true;
Exit0:
	nID = 0;
	return false;
}

BOOL RequireStringToID(LPCTSTR Require, int& nID)
{
	MAP_STRING_EXTERN(REQUIRE_TYPE, Require, nID);

	return true;
Exit0:
	nID = 0;
	return false;
}

KAttribute* CloneAttributeList(const KAttribute* pHead)
{
    KAttribute* pNewHead = NULL;
    KAttribute* pNewTail = NULL;

    while (pHead)
    {
        KAttribute* pNewNode = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pNewNode);

        pNewNode->nKey      = pHead->nKey;
        pNewNode->nValue1   = pHead->nValue1;
        pNewNode->nValue2   = pHead->nValue2;
        pNewNode->pNext     = NULL;

        if (pNewHead == NULL)
        {
            pNewHead = pNewNode;
            pNewTail = pNewHead;
        }
        else
        {
            pNewTail->pNext = pNewNode;
            pNewTail = pNewTail->pNext;
        }

        pHead = pHead->pNext;
    }

Exit0:
    return pNewHead;
}

BOOL CloneAttribListToEnd(KAttribute** ppDestAttribList, KAttribute* pCloneAttribList)
{
    BOOL            bResult         = false;
    KAttribute*     pLoopAttribute  = NULL;
    KAttribute*     pTmpAttribList  = NULL;
    KAttribute**    ppLastAttribute = NULL;

    assert(ppDestAttribList);

    ppLastAttribute = &pTmpAttribList;
    for (pLoopAttribute = pCloneAttribList; pLoopAttribute != NULL; pLoopAttribute = pLoopAttribute->pNext)
    {
        KAttribute* pTmpAttrib = NULL;

        pTmpAttrib = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pTmpAttrib);

        pTmpAttrib->nKey    = pLoopAttribute->nKey;
        pTmpAttrib->nValue1 = pLoopAttribute->nValue1;
        pTmpAttrib->nValue2 = pLoopAttribute->nValue2;
        pTmpAttrib->pNext   = NULL;

        *ppLastAttribute    = pTmpAttrib;
        ppLastAttribute     = &(pTmpAttrib->pNext);
        pTmpAttrib          = NULL;
    }

    if (!(*ppDestAttribList))
    {
        *ppDestAttribList    = pTmpAttribList;
        pTmpAttribList      = NULL;
        goto Exit1;
    }

    pLoopAttribute = *ppDestAttribList;
    while (pLoopAttribute->pNext)
    {
        pLoopAttribute = pLoopAttribute->pNext;
    }
    pLoopAttribute->pNext = pTmpAttribList;
    pTmpAttribList = NULL;

Exit1:
    bResult = true;
Exit0:
    FREE_ATTRIB_LIST(pTmpAttribList);
    return bResult;
}

