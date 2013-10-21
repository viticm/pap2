#include "stdafx.h"
#include "KAttrModifier.h"
#include "KSO3World.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KWorldSettings.h"
#include "KItemList.h"
//#include "KSkillManager.h"

// ------------- 移动相关 ------------------------------------------>

BOOL ProcessWalkSpeedBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;    

    pDstCharacter->m_nWalkSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nWalkSpeed = pDstCharacter->m_nWalkSpeedBase * 
        (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nWalkSpeed, 0, MAX_VELOCITY_XY);
    pDstCharacter->UpdateMoveParameter();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRunSpeedBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;    

    pDstCharacter->m_nRunSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRunSpeed = pDstCharacter->m_nRunSpeedBase * (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;
    
    MAKE_IN_RANGE(pDstCharacter->m_nRunSpeed, 0, MAX_VELOCITY_XY);

    pDstCharacter->UpdateMoveParameter();
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSwimSpeedBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nSwimSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nSwimSpeed = pDstCharacter->m_nSwimSpeedBase * (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nSwimSpeed, 0, MAX_VELOCITY_XY);

    pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}
BOOL ProcessMoveSpeedPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nMoveSpeedPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nWalkSpeed = pDstCharacter->m_nWalkSpeedBase * (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;
    MAKE_IN_RANGE(pDstCharacter->m_nWalkSpeed, 0, MAX_VELOCITY_XY);
    
    pDstCharacter->m_nRunSpeed = pDstCharacter->m_nRunSpeedBase * (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;
    MAKE_IN_RANGE(pDstCharacter->m_nRunSpeed, 0, MAX_VELOCITY_XY);   

    pDstCharacter->m_nSwimSpeed = pDstCharacter->m_nSwimSpeedBase * (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;
    MAKE_IN_RANGE(pDstCharacter->m_nSwimSpeed, 0, MAX_VELOCITY_XY);

    pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMaxJumpCount(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nMaxJumpCount += (bApply ? nValue1 : -nValue1);
 
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGravityBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nGravityBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nGravity = pDstCharacter->m_nGravityBase * (KILO_NUM + pDstCharacter->m_nGravityPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nGravity, 0, MAX_GRAVITY);

    pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGravityPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nGravityPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nGravity = pDstCharacter->m_nGravityBase * 
        (KILO_NUM + pDstCharacter->m_nGravityPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nGravity, 0, MAX_GRAVITY);

    pDstCharacter->UpdateMoveParameter();

	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJumpSpeedXYBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nJumpSpeedXYBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nJumpSpeedXY = pDstCharacter->m_nJumpSpeedXYBase * 
        (KILO_NUM + pDstCharacter->m_nJumpSpeedXYPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nJumpSpeedXY, 0, MAX_VELOCITY_XY);

    // 跳跃速度不立即刷新,否则很麻烦
    //pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJumpSpeedXYPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nJumpSpeedXYPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nJumpSpeedXY = pDstCharacter->m_nJumpSpeedXYBase * 
        (KILO_NUM + pDstCharacter->m_nJumpSpeedXYPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nJumpSpeedXY, 0, MAX_VELOCITY_XY);

    //pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJumpSpeedZBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nJumpSpeedZBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nJumpSpeedZ = pDstCharacter->m_nJumpSpeedZBase * 
        (KILO_NUM + pDstCharacter->m_nJumpSpeedZPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nJumpSpeedZ, 0, MAX_VELOCITY_Z);

    // Z轴速度无法立即刷新
    //pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJumpSpeedZPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nJumpSpeedZPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nJumpSpeedZ = pDstCharacter->m_nJumpSpeedZBase * 
        (KILO_NUM + pDstCharacter->m_nJumpSpeedZPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nJumpSpeedZ, 0, MAX_VELOCITY_Z);

    //pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}


BOOL ProcessDropDefence(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nDropDefence += (bApply ? nValue1 : -nValue1);
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDivingFrameBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nDivingFrameBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nDivingFrame = pDstCharacter->m_nDivingFrameBase * (KILO_NUM + pDstCharacter->m_nDivingFramePercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nDivingFrame, 0, USHRT_MAX);
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDivingFramePercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nDivingFramePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nDivingFrame = pDstCharacter->m_nDivingFrameBase * (KILO_NUM + pDstCharacter->m_nDivingFramePercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nDivingFrame, 0, USHRT_MAX);
    
	bResult = true;
Exit0:
    return bResult;
}



BOOL ProcessSwimAbility(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSwimAbility += bApply ? 1 : -1;
    pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessWaterFlyAbility(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nWaterFlyAbility += bApply ? 1 : -1;
    pDstCharacter->UpdateMoveParameter();
    
	bResult = true;
Exit0:
    return bResult;
}

// ------------ 潜能相关属性处理函数 ------------------>

//力量X点
BOOL ProcessStrengthBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, false, 2 * pDstCharacter->m_nCurrentStrength, 0);

    pDstCharacter->m_nStrengthBase  += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nCurrentStrength = 
        pDstCharacter->m_nStrengthBase * (KILO_NUM + pDstCharacter->m_nStrengthBasePercentAdd) / KILO_NUM;

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, true, 2 * pDstCharacter->m_nCurrentStrength, 0);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessStrengthBasePercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, false, 2 * pDstCharacter->m_nCurrentStrength, 0);

    pDstCharacter->m_nStrengthBasePercentAdd  += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nCurrentStrength = 
        pDstCharacter->m_nStrengthBase * (KILO_NUM + pDstCharacter->m_nStrengthBasePercentAdd) / KILO_NUM;

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, true, 2 * pDstCharacter->m_nCurrentStrength, 0);

    bResult = true;
Exit0:
	return bResult;
}

//身法X点
BOOL ProcessAgilityBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult         = false;

    g_AttributeProcessFunctions[atDodge](pDstCharacter, false, pDstCharacter->m_nAgilityDodege, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, false, pDstCharacter->m_nAgilityCriticalStrike, 0);

    pDstCharacter->m_nAgilityBase += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentAgility =
        pDstCharacter->m_nAgilityBase * (KILO_NUM + pDstCharacter->m_nAgilityBasePercentAdd) / KILO_NUM;

    pDstCharacter->m_nAgilityDodege         = (4 * pDstCharacter->m_nCurrentAgility + 3) / 6 + 2;
    pDstCharacter->m_nAgilityCriticalStrike = (4 * pDstCharacter->m_nCurrentAgility + 3) / 6 + 3;
    
    g_AttributeProcessFunctions[atDodge](pDstCharacter, true, pDstCharacter->m_nAgilityDodege, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, true, pDstCharacter->m_nAgilityCriticalStrike, 0);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessAgilityBasePercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult         = false;

    g_AttributeProcessFunctions[atDodge](pDstCharacter, false, pDstCharacter->m_nAgilityDodege, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, false, pDstCharacter->m_nAgilityCriticalStrike, 0);

    pDstCharacter->m_nAgilityBasePercentAdd += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentAgility =
        pDstCharacter->m_nAgilityBase * (KILO_NUM + pDstCharacter->m_nAgilityBasePercentAdd) / KILO_NUM;

    pDstCharacter->m_nAgilityDodege         = (4 * pDstCharacter->m_nCurrentAgility + 3) / 6 + 2;
    pDstCharacter->m_nAgilityCriticalStrike = (4 * pDstCharacter->m_nCurrentAgility + 3) / 6 + 3;
    
    g_AttributeProcessFunctions[atDodge](pDstCharacter, true, pDstCharacter->m_nAgilityDodege, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, true, pDstCharacter->m_nAgilityCriticalStrike, 0);

    bResult = true;
Exit0:
	return bResult;
}


//体质X点
BOOL ProcessVitalityBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;  

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentVitality, 0);

    pDstCharacter->m_nVitalityBase += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentVitality = 
        pDstCharacter->m_nVitalityBase * (KILO_NUM + pDstCharacter->m_nVitalityBasePercentAdd) / KILO_NUM;

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentVitality, 0);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessVitalityBasePercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;  

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentVitality, 0);

    pDstCharacter->m_nVitalityBasePercentAdd += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentVitality = 
        pDstCharacter->m_nVitalityBase * (KILO_NUM + pDstCharacter->m_nVitalityBasePercentAdd) / KILO_NUM;

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentVitality, 0);

    bResult = true;
Exit0:
	return bResult;
}


//根骨X点
BOOL ProcessSpiritBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult              = false;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentSpirit, 0);
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);

    pDstCharacter->m_nSpiritBase  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpirit = 
        pDstCharacter->m_nSpiritBase * (KILO_NUM + pDstCharacter->m_nSpiritBasePercentAdd) / KILO_NUM;

    pDstCharacter->m_nSpiritCriticalStrike = (4 * pDstCharacter->m_nCurrentSpirit + 3) / 6 + 3;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentSpirit, 0);    
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessSpiritBasePercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult              = false;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentSpirit, 0);
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, false, pDstCharacter->m_nSpiritCriticalStrike, 0);

    pDstCharacter->m_nSpiritBasePercentAdd  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpirit = 
        pDstCharacter->m_nSpiritBase * (KILO_NUM + pDstCharacter->m_nSpiritBasePercentAdd) / KILO_NUM;

    pDstCharacter->m_nSpiritCriticalStrike = (4 * pDstCharacter->m_nCurrentSpirit + 3) / 6 + 3;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentSpirit, 0);    
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, true, pDstCharacter->m_nSpiritCriticalStrike, 0);

    bResult = true;
Exit0:
	return bResult;
}

//精神X点
BOOL ProcessSpunkBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult        = false;

    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, false, pDstCharacter->m_nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, false, pDstCharacter->m_nSpunkManareplenish, 0);
    
    g_AttributeProcessFunctions[atSpunkAttackPower](pDstCharacter, false, 2 * pDstCharacter->m_nCurrentSpunk, 0);
    
    pDstCharacter->m_nSpunkBase  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpunk = 
        pDstCharacter->m_nSpunkBase * (KILO_NUM + pDstCharacter->m_nSpunkBasePercentAdd) / KILO_NUM;
    
    pDstCharacter->m_nSpunkLifeReplenish = (pDstCharacter->m_nCurrentSpunk + 5) / 10 + 1;
    pDstCharacter->m_nSpunkManareplenish = (2 * pDstCharacter->m_nCurrentSpunk + 5) / 10 + 4;
    
    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, true, pDstCharacter->m_nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, true, pDstCharacter->m_nSpunkManareplenish, 0);

    g_AttributeProcessFunctions[atSpunkAttackPower](pDstCharacter, true, 2 * pDstCharacter->m_nCurrentSpunk, 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkBasePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult        = false;

    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, false, pDstCharacter->m_nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, false, pDstCharacter->m_nSpunkManareplenish, 0);

    g_AttributeProcessFunctions[atSpunkAttackPower](pDstCharacter, false, 2 * pDstCharacter->m_nCurrentSpunk, 0);

    pDstCharacter->m_nSpunkBasePercentAdd  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpunk = 
        pDstCharacter->m_nSpunkBase * (KILO_NUM + pDstCharacter->m_nSpunkBasePercentAdd) / KILO_NUM;
    
    pDstCharacter->m_nSpunkLifeReplenish = (pDstCharacter->m_nCurrentSpunk + 5) / 10 + 1;
    pDstCharacter->m_nSpunkManareplenish = (2 * pDstCharacter->m_nCurrentSpunk + 5) / 10 + 4;
    
    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, true, pDstCharacter->m_nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, true, pDstCharacter->m_nSpunkManareplenish, 0);

    g_AttributeProcessFunctions[atSpunkAttackPower](pDstCharacter, true, 2 * pDstCharacter->m_nCurrentSpunk, 0);

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 生命 ---------------------------------------------------->

BOOL ProcessCurrentLife(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    // 该属性不可回滚
    KG_PROCESS_ERROR(bApply);
    
    // 注意: 通过这个属性修改函数进行的生命值加减无法进入战斗统计流程!
    // 如果要进入战斗统计流程，应该用：atCallUntypedHealth
	pDstCharacter->m_nCurrentLife += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxLifeBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
	pDstCharacter->m_nMaxLifeBase += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxLife = 
		pDstCharacter->m_nMaxLifeBase * (KILO_NUM + pDstCharacter->m_nMaxLifePercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxLifeAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxLifePercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxLifePercentAdd += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxLife = 
		pDstCharacter->m_nMaxLifeBase * (KILO_NUM + pDstCharacter->m_nMaxLifePercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxLifeAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxLifeAdditional(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxLifeAdditional += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxLife = 
		pDstCharacter->m_nMaxLifeBase * (KILO_NUM + pDstCharacter->m_nMaxLifePercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxLifeAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessLifeReplenish(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nLifeReplenish += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLifeReplenishPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nLifeReplenishPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 内力 ---------------------------------------------------->

BOOL ProcessCurrentMana(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(bApply);
        
	pDstCharacter->m_nCurrentMana += (bApply ? nValue1 : -nValue1);

    MAKE_IN_RANGE(pDstCharacter->m_nCurrentMana, 0, pDstCharacter->m_nMaxMana);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxManaBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
	pDstCharacter->m_nMaxManaBase += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxMana = 
		pDstCharacter->m_nMaxManaBase * (KILO_NUM + pDstCharacter->m_nMaxManaPercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxManaAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxManaPercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxManaPercentAdd += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxMana = 
		pDstCharacter->m_nMaxManaBase * (KILO_NUM + pDstCharacter->m_nMaxManaPercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxManaAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxManaAdditional(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxManaAdditional += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxMana = 
		pDstCharacter->m_nMaxManaBase * (KILO_NUM + pDstCharacter->m_nMaxManaPercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxManaAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessManaReplenish(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nManaReplenish += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessManaReplenishPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nManaReplenishPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessQiToSkillExpPercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(pDstCharacter->m_eKind == ckPlayer);

    ((KPlayer*)pDstCharacter)->m_nQiToSkillExpPercentAdd += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 怒气 ---------------------------------------------------->

BOOL ProcessCurrentRage(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KG_PROCESS_ERROR(bApply);
    
	pDstCharacter->m_nCurrentRage += (bApply ? nValue1 : -nValue1);

    MAKE_IN_RANGE(pDstCharacter->m_nCurrentRage, 0, pDstCharacter->m_nMaxRage);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxRageBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
	pDstCharacter->m_nMaxRageBase += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxRage = 
		pDstCharacter->m_nMaxRageBase * (KILO_NUM + pDstCharacter->m_nMaxRagePercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxRageAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxRagePercentAdd(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;  

	pDstCharacter->m_nMaxRagePercentAdd += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxRage = 
		pDstCharacter->m_nMaxRageBase * (KILO_NUM + pDstCharacter->m_nMaxRagePercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxRageAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxRageAdditional(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxRageAdditional += (bApply ? nValue1 : -nValue1);

	pDstCharacter->m_nMaxRage = 
		pDstCharacter->m_nMaxRageBase * (KILO_NUM + pDstCharacter->m_nMaxRagePercentAdd) / KILO_NUM + 
        pDstCharacter->m_nMaxRageAdditional;

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessRageReplenish(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nRageReplenish += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRageReplenishPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;   

    pDstCharacter->m_nRageReplenishPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
// ------------------ 仇恨 -------------------------------------------------->
BOOL ProcessGainPositiveThreatCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nGainPositiveThreatCoefficient += (bApply ? nValue1 : - nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGainNegativeThreatCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nGainNegativeThreatCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessImpairPositiveThreatCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nImpairPositiveThreatCoefficient += (bApply ? nValue1 : - nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessImpairNegativeThreatCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nImpairNegativeThreatCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}
#endif

// ------------------ 战斗杂项 ----------------------------------------------->

BOOL ProcessDodge(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;  

    pDstCharacter->m_nDodge += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPositiveShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPositiveShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNegativeShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nNegativeShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGlobalBlock(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nGlobalBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGlobalDamageAbsorb(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;   

    KG_PROCESS_ERROR(bApply);

    pDstCharacter->m_nGlobalDamageAbsorb += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGlobalDamageManaShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;  

    KG_PROCESS_ERROR(bApply);

    pDstCharacter->m_nGlobalDamageManaShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessStealLifePercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nStealLifePercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessStealManaPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nStealManaPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessBeatBackRate(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nBeatBackRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessKnockedBackRate(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nKnockedBackRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessKnockedOffRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult = false;
   
    pDstCharacter->m_nKnockedOffRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsAttackPower(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    // nValue1 = wdtWand, wdtSword ...
    
    KG_ASSERT_EXIT(nValue1 < wdtTotal && nValue1 >= 0);
    // nValue2转为m_nPhysicsAttackPower公式待定,先如下写着
    pDstCharacter->m_nPhysicsAttackPower[nValue1] += (bApply ? nValue2 : -nValue2);

    bResult = true;
Exit0:
    return bResult;
}
BOOL ProcessMagicAttackPower(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    // nValue1 = wdtWand, wdtSword ...
    
    KG_ASSERT_EXIT(nValue1 < wdtTotal && nValue1 >= 0);
    // nValue2转为m_nMagicAttackPower公式待定,先如下写着
    pDstCharacter->m_nMagicAttackPower[nValue1] += (bApply ? nValue2 : -nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponDamageBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;       
    
    pDstCharacter->m_nMeleeWeaponDamageBase += (bApply ? nValue1 : -nValue1);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;       

    pDstCharacter->m_nMeleeWeaponDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponDamageBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    
    
    pDstCharacter->m_nRangeWeaponDamageBase += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nRangeWeaponDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillPhysicsDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nSkillPhysicsDamage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillPhysicsDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nSkillPhysicsDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillPhysicsDamagePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nSkillPhysicsDamagePercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nPhysicsAttackPowerBase += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nPhysicsDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsAttackHit(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nPhysicsAttackHit += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nPhysicsCriticalStrike += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsResistPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDefenceAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsDefenceAdd += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDefenceMax(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsDefenceMax += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsShieldBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsShieldBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsShield = 
        pDstCharacter->m_nPhysicsShieldBase * (KILO_NUM + pDstCharacter->m_nPhysicsShieldPercent) / KILO_NUM + 
        pDstCharacter->m_nPhysicsShieldAdditional;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsShieldPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsShieldPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsShield = 
        pDstCharacter->m_nPhysicsShieldBase * (KILO_NUM + pDstCharacter->m_nPhysicsShieldPercent) / KILO_NUM + 
        pDstCharacter->m_nPhysicsShieldAdditional;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsShieldAdditional(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsShieldAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsShield = 
        pDstCharacter->m_nPhysicsShieldBase * (KILO_NUM + pDstCharacter->m_nPhysicsShieldPercent) / KILO_NUM + 
        pDstCharacter->m_nPhysicsShieldAdditional;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsReflection(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsReflectionPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsBlock(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDamageAbsorb(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsDamageAbsorb += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDamageManaShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsDamageManaShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------------ 内功 ------------------------------------------------------->
BOOL ProcessSpunkAttackPower(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nSpunkAttackPower += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}
BOOL ProcessMeleeSpunkAttackPower(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nMeleeSpunkAttackPower += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nMeleeSpunkAttackPowerPercent = 
        KILO_NUM * pDstCharacter->m_nMeleeSpunkAttackPower / (pDstCharacter->m_nMeleeSpunkAttackPower + 5 * pDstCharacter->m_nLevel + 5);

    bResult = true;
Exit0:
    return bResult;
}
BOOL ProcessRangeSpunkAttackPower(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nRangeSpunkAttackPower += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRangeSpunkAttackPowerPercent = 
        KILO_NUM * pDstCharacter->m_nRangeSpunkAttackPower / (pDstCharacter->m_nRangeSpunkAttackPower + 5 * pDstCharacter->m_nLevel + 5);

    bResult = true;
Exit0:
    return bResult;
}

// ------------------ 阳性内功 --------------------------------------------------->

BOOL ProcessSkillSolarDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nSkillSolarDamage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSkillSolarDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nSkillSolarDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}  

BOOL ProcessSkillSolarDamagePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nSkillSolarDamagePercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nSolarAttackPowerBase += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSolarDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nSolarDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessSolarMagicHit(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nSolarMagicHit += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    
    
    pDstCharacter->m_nSolarCriticalStrike += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicResistPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicReflection(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicReflectionPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicBlock(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarDamageAbsorb(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarDamageAbsorb += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarDamageManaShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarDamageManaShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------------ 中性内功 --------------------------------------------------->

BOOL ProcessSkillNeutralDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillNeutralDamage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSkillNeutralDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillNeutralDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSkillNeutralDamagePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillNeutralDamagePercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralAttackPowerBase += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessNeutralDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessNeutralMagicHit(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicHit += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralCriticalStrike += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicResistPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicReflection(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicReflectionPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicBlock(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralDamageAbsorb(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralDamageAbsorb += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralDamageManaShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralDamageManaShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------------ 阴性内功 --------------------------------------------------->

BOOL ProcessSkillLunarDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillLunarDamage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSkillLunarDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillLunarDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSkillLunarDamagePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillLunarDamagePercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarAttackPowerBase += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessLunarDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessLunarMagicHit(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    
    

    pDstCharacter->m_nLunarMagicHit += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarCriticalStrike += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicResistPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicReflection(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicReflectionPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicBlock(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarDamageAbsorb(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarDamageAbsorb += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarDamageManaShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarDamageManaShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------------ 毒性 ------------------------------------------------------->

BOOL ProcessSkillPoisonDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillPoisonDamage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSkillPoisonDamageRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillPoisonDamageRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}  

BOOL ProcessSkillPoisonDamagePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillPoisonDamagePercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonAttackPowerBase += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessPoisonDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessPoisonMagicHit(KCharacter*  pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonMagicHit += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonCriticalStrike += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicResistPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicReflection(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicReflectionPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicBlock(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonDamageAbsorb(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonDamageAbsorb += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonDamageManaShield(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonDamageManaShield += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 治疗 ------------------------------------------------------------>
BOOL ProcessTherapyPowerBase(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
        
    pDstCharacter->m_nTherapyPowerBase += (bApply ? nValue1 : -nValue1);

Exit0:
    return bResult;
}

BOOL ProcessSkillTherapy(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillTherapy += (bApply ? nValue1 : -nValue1);

Exit0:
    return bResult;
}

BOOL ProcessSkillTherapyRand(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillTherapyRand += (bApply ? nValue1 : -nValue1);

Exit0:
    return bResult;
}

BOOL ProcessSkillTherapyPercent(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillTherapyPercent += (bApply ? nValue1 : -nValue1);

Exit0:
    return bResult = true;

}

BOOL ProcessTherapyCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nTherapyCoefficient += (bApply ? nValue1 : -nValue1);

Exit0:
    return bResult = true;
}

BOOL ProcessNoneWeaponAttackSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

        
    pDstCharacter->m_nNoneWeaponAttackSpeedBase += (bApply ? nValue1 : -nValue1);

    if (g_IsPlayer(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;
        KItem* pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);    
        if (!pItem)
        {
            pDstCharacter->m_nMeleeWeaponAttackSpeed = 
                pDstCharacter->m_nNoneWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
                pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
           
            if (pDstCharacter->m_nMeleeWeaponAttackSpeed)
                pDstCharacter->m_nMeleeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nMeleeWeaponAttackSpeed;
            else
                pDstCharacter->m_nMeleeWeaponAttackSpeed = INT_MAX;
        }
    }
    // else Npc就不管了

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponAttackSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        pDstCharacter->m_nMeleeWeaponAttackSpeed = 
            nValue1 * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
            pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
    }
    else
    {
        pDstCharacter->m_nMeleeWeaponAttackSpeed = 
            pDstCharacter->m_nNoneWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
            pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
    }

    // 将单位次数/帧,转为帧/次
    if (pDstCharacter->m_nMeleeWeaponAttackSpeed)
        pDstCharacter->m_nMeleeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nMeleeWeaponAttackSpeed;
    else
        pDstCharacter->m_nMeleeWeaponAttackSpeed = INT_MAX;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponAttackSpeedPercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd += (bApply ? nValue1 : -nValue1);
    
    if (g_IsPlayer(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;
        KItem* pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
        if (pItem)
        {
            pDstCharacter->m_nMeleeWeaponAttackSpeed = 
                pDstCharacter->m_nMeleeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
                pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
        }
        else
        {
            pDstCharacter->m_nMeleeWeaponAttackSpeed = 
                pDstCharacter->m_nNoneWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
                pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
        }
    }
    else
    {
        pDstCharacter->m_nMeleeWeaponAttackSpeed = 
            pDstCharacter->m_nMeleeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
            pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
    }
    // 将单位次数/帧,转为帧/次
    if (pDstCharacter->m_nMeleeWeaponAttackSpeed)
        pDstCharacter->m_nMeleeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nMeleeWeaponAttackSpeed;
    else
        pDstCharacter->m_nMeleeWeaponAttackSpeed = INT_MAX;
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponAttackSpeedAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional += (bApply ? nValue1 : -nValue1);

    if (g_IsPlayer(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;
        KItem* pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
        if (pItem)
        {
            pDstCharacter->m_nMeleeWeaponAttackSpeed = 
                pDstCharacter->m_nMeleeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
                pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
        }
        else
        {
            pDstCharacter->m_nMeleeWeaponAttackSpeed = 
                pDstCharacter->m_nNoneWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
                pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
        }
    }
    else
    {
        pDstCharacter->m_nMeleeWeaponAttackSpeed = 
            pDstCharacter->m_nMeleeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
            pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional;
    }
    // 将单位次数/帧,转为帧/次
    if (pDstCharacter->m_nMeleeWeaponAttackSpeed)
        pDstCharacter->m_nMeleeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nMeleeWeaponAttackSpeed;
    else
        pDstCharacter->m_nMeleeWeaponAttackSpeed = INT_MAX;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessIgnoreOtherAlarmRange(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_ASSERT_EXIT(pDstCharacter);

    pDstCharacter->m_nIgnoreOtherAlarmRange += (bApply ? 1 : -1);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_nIgnoreOtherAlarmRange >= 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSelfAlarmRangeRevise(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_ASSERT_EXIT(pDstCharacter);

    pDstCharacter->m_nSelfAlarmRangeRevise += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessOtherAlarmRangeRevise(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_ASSERT_EXIT(pDstCharacter);

    pDstCharacter->m_nOtherAlarmRangeRevise += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponAttackSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRangeWeaponAttackSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRangeWeaponAttackSpeed = 
        pDstCharacter->m_nRangeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nRangeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
        pDstCharacter->m_nRangeWeaponAttackSpeedAdditional;
    
    // 将单位次数/帧,转为帧/次
    if (pDstCharacter->m_nRangeWeaponAttackSpeed)
        pDstCharacter->m_nRangeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nRangeWeaponAttackSpeed; 
    else
        pDstCharacter->m_nRangeWeaponAttackSpeed = INT_MAX;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponAttackSpeedPercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRangeWeaponAttackSpeedPercentAdd += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRangeWeaponAttackSpeed = 
        pDstCharacter->m_nRangeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nRangeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
        pDstCharacter->m_nRangeWeaponAttackSpeedAdditional;

    // 将单位次数/帧,转为帧/次
    if (pDstCharacter->m_nRangeWeaponAttackSpeed)
        pDstCharacter->m_nRangeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nRangeWeaponAttackSpeed; 
    else
        pDstCharacter->m_nRangeWeaponAttackSpeed = INT_MAX;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponAttackSpeedAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRangeWeaponAttackSpeedAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRangeWeaponAttackSpeed = 
        pDstCharacter->m_nRangeWeaponAttackSpeedBase * (KILO_NUM + pDstCharacter->m_nRangeWeaponAttackSpeedPercentAdd) / KILO_NUM + 
        pDstCharacter->m_nRangeWeaponAttackSpeedAdditional;

    // 将单位次数/帧,转为帧/次
    if (pDstCharacter->m_nRangeWeaponAttackSpeed)
        pDstCharacter->m_nRangeWeaponAttackSpeed = KILO_NUM / pDstCharacter->m_nRangeWeaponAttackSpeed; 
    else
        pDstCharacter->m_nRangeWeaponAttackSpeed = INT_MAX;

    bResult = true;
Exit0:
    return bResult;
}


#ifdef _SERVER
// 打坐
BOOL ProcessMeditate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    
    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->Meditate();
    KG_PROCESS_ERROR(bRetCode);

    // 添加Buff, 打坐时要一个Buff来加快其回血,回蓝速度
    pDstCharacter->ProcessBuff(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 冲锋 ------------------------------------------------------------>
BOOL ProcessDash(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;    
    
    KG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessDash(nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 击晕, 眩晕 ------------------------------------------------------->
BOOL ProcessStun(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;    
   
    if (bApply)
    {    
        ++pDstCharacter->m_nStunCounter;          
        pDstCharacter->Stun();
    }
    else
    {
        --pDstCharacter->m_nStunCounter;    

        KGLOG_CHECK_ERROR(pDstCharacter->m_nStunCounter >= 0);
        
        if (pDstCharacter->m_nStunCounter == 0)
        {
            if (pDstCharacter->m_nHaltCounter > 0)
            {
                pDstCharacter->Halt();
            }
            else
            {
                pDstCharacter->Stop();
            }
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessHalt(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
       
    if (bApply)
    {
        ++pDstCharacter->m_nHaltCounter;
        pDstCharacter->Halt();
    }
    else
    {
        --pDstCharacter->m_nHaltCounter;
        KGLOG_CHECK_ERROR(pDstCharacter->m_nHaltCounter >= 0);

        if (pDstCharacter->m_nHaltCounter == 0)
        {
            pDstCharacter->Stop();
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRevive(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessRevive(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJump(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KG_ASSERT_EXIT(bApply);

    // TODO: 这里,跳跃的方向等还要进一步确定怎么计算

    pDstCharacter->Jump(false, pDstCharacter->m_nFaceDirection, true);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSit(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->SitDown(true);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackExhale(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KG_ASSERT_EXIT(bApply);
    
    pDstCharacter->ProcessKnockedBackExhale(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackConvergence(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessKnockedBackConvergence(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackRandom(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessKnockedBackRandom(nValue1, nValue2);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackSrcFace(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessKnockedBackSrcFace(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedOffParabola(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessKnockedOffParabola(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 技能运算 --------------------------------------------------------->
BOOL ProcessCallTherapy(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    
    
    KG_ASSERT_EXIT(pDstCharacter->m_pDstSkillCalculateResult);
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessTherapy(nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallPhysicsDamage(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KG_ASSERT_EXIT(bApply);
    
    pDstCharacter->ProcessDamage(sdtPhysicsDamage, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallSolarDamage(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessDamage(sdtSolarMagicDamage, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallNeutralDamage(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessDamage(sdtNeutralMagicDamage, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallLunarDamage(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessDamage(sdtLunarMagicDamage, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallPoisonDamage(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KG_ASSERT_EXIT(bApply);

    pDstCharacter->ProcessDamage(sdtPoisonDamage, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCastSkillTargetSrc(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    DWORD       dwSkillID           = (DWORD)nValue1;
    DWORD       dwSkillLevel        = (DWORD)nValue2;
    KCharacter *pTargetCharacter    = NULL;
    KTarget     Target;

    KG_ASSERT_EXIT(pDstCharacter->m_pSkillAttributeParam);
    pTargetCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KG_ASSERT_EXIT(pTargetCharacter);
    
    bRetCode = Target.SetTarget(pDstCharacter);
    KGLOG_PROCESS_ERROR(bRetCode);

    pDstCharacter->CastSkillSub(dwSkillID, dwSkillLevel, Target);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCastSkillTargetDst(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    DWORD       dwSkillID           = (DWORD)nValue1;
    DWORD       dwSkillLevel        = (DWORD)nValue2;
    KCharacter *pTargetCharacter    = NULL;
    KTarget     Target;

    KG_ASSERT_EXIT(pDstCharacter->m_pSkillAttributeParam);
    pTargetCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillDst;
    KG_ASSERT_EXIT(pTargetCharacter);

    bRetCode = Target.SetTarget(pTargetCharacter);
    KG_PROCESS_ERROR(bRetCode);

    pDstCharacter->CastSkillSub(dwSkillID, dwSkillLevel, Target);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillEventHandler(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    int               nAddRef    = 0;
    int               nEventID   = nValue1;
    KCharacter::KUSER_SKILL_EVENT_MAP::iterator it;

    nAddRef = bApply ? 1 : -1;
    it = pDstCharacter->m_SkillEventMap.find(nEventID);
    
    if (it != pDstCharacter->m_SkillEventMap.end())
    {
        it->second.nRefCount += nAddRef;
        if (it->second.nRefCount <= 0)
        {
            pDstCharacter->m_SkillEventMap.erase(it);
        }
    }
    else
    {
        KG_ASSERT_EXIT(nAddRef == 1);
        KSKILL_EVENT* pEventInfo = g_SkillManager.GetEventInfo(nEventID);
        KGLOG_PROCESS_ERROR(pEventInfo);
        KUSER_SKILL_EVENT UserSkillEvent;
        UserSkillEvent.nRefCount = 1;
        UserSkillEvent.SkillEvent = *pEventInfo;
        pDstCharacter->m_SkillEventMap[nEventID] = UserSkillEvent;
    }    

Exit0:
    return true;
}

BOOL ProcessPracticeKungfu(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    DWORD           dwKungFuID      = nValue1;
    DWORD           dwKungFuLevel   = 0;
    DWORD           dwExp           = 0;
    KSkill*         pSkill          = NULL;
    KPlayer*        pPlayer         = NULL;
    int             nCostQiPoint    = 0;
    KKUNG_FU_EXP*   pKungFuExp      = NULL;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_eKind == ckPlayer);
    
    pPlayer = (KPlayer*)pDstCharacter;

    dwKungFuLevel = pPlayer->m_SkillList.GetSkillLevel(dwKungFuID);
    KGLOG_PROCESS_ERROR(dwKungFuLevel > 0);

    bRetCode = g_SkillManager.GetKungFuExp(dwKungFuID, dwKungFuLevel, &pKungFuExp);
    KGLOG_PROCESS_ERROR(bRetCode);

    dwExp = pKungFuExp->nExpAdd;
    nCostQiPoint = pKungFuExp->nCostQiPoint;
    
    if (pPlayer->m_bOnPracticeRoom && pPlayer->m_nCurrentQi > 0)
    {
        nCostQiPoint = min(pPlayer->m_nCurrentQi, nCostQiPoint);
        KGLOG_PROCESS_ERROR(nCostQiPoint >= 0);

        dwExp = dwExp + dwExp * nCostQiPoint * pPlayer->m_nQiToSkillExpPercentAdd / KILO_NUM;
        pPlayer->SetQi(pPlayer->m_nCurrentQi - nCostQiPoint);
    }
    KGLOG_PROCESS_ERROR(dwExp >= 0);

    pPlayer->m_SkillList.AddSkillExp(dwKungFuID, dwExp);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessBuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    
    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->ProcessBuff(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDebuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->ProcessDebuff(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDot(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->ProcessDot(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessHot(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->ProcessHot(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachSingleBuff(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);
    
    bRetCode = pDstCharacter->m_BuffList.DetachSingleBuff(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachSingleDebuff(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachSingleDebuff(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachSingleGroupDot(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachSingleGroupDot(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachSingleGroupHot(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachSingleGroupHot(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachMultiGroupBuff(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);
    
    bRetCode = pDstCharacter->m_BuffList.DetachMultiGroupBuff(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachMultiGroupDebuff(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachMultiGroupDebuff(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachMultiGroupDot(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachMultiGroupDot(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachMultiGroupHot(KCharacter *pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    KG_ASSERT_EXIT(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachMultiGroupHot(nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessModifyThreat(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult  = false;
    BOOL        bRetCode = false;
    
    bRetCode = pDstCharacter->ModifyThreat((THREAT_TYPE)nValue1, nValue2);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessExecuteScript(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    DWORD           dwSrciptID      = nValue1;
    int             nLuaTopIndex    = 0;
    Lua_State*      L               = NULL;
    KLuaScriptEx*   pScript         = NULL;
    
    KG_ASSERT_EXIT(pDstCharacter);

    pScript = g_ScriptCenter.GetScript(dwSrciptID);
    KGLOG_PROCESS_ERROR(pScript);

	if (bApply)
	{  
		bRetCode = pScript->IsFuncExist("Apply");
		KG_PROCESS_ERROR(bRetCode);
		
		pScript->SafeCallBegin(&nLuaTopIndex);
        pScript->CallFunction("Apply", 0, "d", pDstCharacter->m_dwID);
		pScript->SafeCallEnd(nLuaTopIndex);
    }
    else
    {
		bRetCode = pScript->IsFuncExist("UnApply");
		KG_PROCESS_ERROR(bRetCode);
		
		pScript->SafeCallBegin(&nLuaTopIndex);
        pScript->CallFunction("UnApply", 0, "d", pDstCharacter->m_dwID);
		pScript->SafeCallEnd(nLuaTopIndex);
    }
      
    bResult = true;
Exit0:
    return bResult;
}

#endif

// --------------------------------------------------------------------------------

ATTR_PROCESS_FUN g_AttributeProcessFunctions[atTotal] = { NULL };

BOOL InitAttributeFunctions()
{
	memset(g_AttributeProcessFunctions, 0, sizeof(g_AttributeProcessFunctions));

    // ------------- 移动相关 ------------------------------------------>
    g_AttributeProcessFunctions[atWalkSpeedBase]                = ProcessWalkSpeedBase;
    g_AttributeProcessFunctions[atRunSpeedBase]                 = ProcessRunSpeedBase;
    g_AttributeProcessFunctions[atSwimSpeedBase]                = ProcessSwimSpeedBase;
    g_AttributeProcessFunctions[atMoveSpeedPercent]             = ProcessMoveSpeedPercent;
    g_AttributeProcessFunctions[atMaxJumpCount]                 = ProcessMaxJumpCount;
    g_AttributeProcessFunctions[atGravityBase]                  = ProcessGravityBase;
    g_AttributeProcessFunctions[atGravityPercent]               = ProcessGravityPercent;
    g_AttributeProcessFunctions[atJumpSpeedXYBase]              = ProcessJumpSpeedXYBase;
    g_AttributeProcessFunctions[atJumpSpeedXYPercent]           = ProcessJumpSpeedXYPercent;
    g_AttributeProcessFunctions[atJumpSpeedZBase]               = ProcessJumpSpeedZBase;
    g_AttributeProcessFunctions[atJumpSpeedZPercent]            = ProcessJumpSpeedZPercent;
    g_AttributeProcessFunctions[atDropDefence]                  = ProcessDropDefence;
    g_AttributeProcessFunctions[atDivingFrameBase]              = ProcessDivingFrameBase;
    g_AttributeProcessFunctions[atDivingFramePercent]           = ProcessDivingFramePercent;


    // ------------- 潜能 ---------------------------------------------->
    g_AttributeProcessFunctions[atStrengthBase]                 = ProcessStrengthBase;
    g_AttributeProcessFunctions[atStrengthBasePercentAdd]       = ProcessStrengthBasePercentAdd;
    g_AttributeProcessFunctions[atAgilityBase]                  = ProcessAgilityBase;
    g_AttributeProcessFunctions[atAgilityBasePercentAdd]        = ProcessAgilityBasePercentAdd;
    g_AttributeProcessFunctions[atVitalityBase]                 = ProcessVitalityBase;
    g_AttributeProcessFunctions[atVitalityBasePercentAdd]       = ProcessVitalityBasePercentAdd;
    g_AttributeProcessFunctions[atSpiritBase]                   = ProcessSpiritBase;
    g_AttributeProcessFunctions[atSpiritBasePercentAdd]         = ProcessSpiritBasePercentAdd;
    g_AttributeProcessFunctions[atSpunkBase]                    = ProcessSpunkBase;
    g_AttributeProcessFunctions[atSpunkBasePercentAdd]          = ProcessSpunkBasePercentAdd;

    // ------------- 生命 ---------------------------------------------->
	g_AttributeProcessFunctions[atCurrentLife]                  = ProcessCurrentLife;
    g_AttributeProcessFunctions[atMaxLifeBase]                  = ProcessMaxLifeBase;
	g_AttributeProcessFunctions[atMaxLifePercentAdd]            = ProcessMaxLifePercentAdd;
    g_AttributeProcessFunctions[atMaxLifeAdditional]            = ProcessMaxLifeAdditional;
    g_AttributeProcessFunctions[atLifeReplenish]                = ProcessLifeReplenish;
    g_AttributeProcessFunctions[atLifeReplenishPercent]         = ProcessLifeReplenishPercent;

    // ------------- 内力 ---------------------------------------------->
	g_AttributeProcessFunctions[atCurrentMana]                  = ProcessCurrentMana;
    g_AttributeProcessFunctions[atMaxManaBase]                  = ProcessMaxManaBase;
	g_AttributeProcessFunctions[atMaxManaPercentAdd]            = ProcessMaxManaPercentAdd;
    g_AttributeProcessFunctions[atMaxManaAdditional]            = ProcessMaxManaAdditional;
    g_AttributeProcessFunctions[atManaReplenish]                = ProcessManaReplenish;
    g_AttributeProcessFunctions[atManaReplenishPercent]         = ProcessManaReplenishPercent;

    g_AttributeProcessFunctions[atQiToSkillExpPercentAdd]       = ProcessQiToSkillExpPercentAdd;
    // ------------- 怒气 ---------------------------------------------->
	g_AttributeProcessFunctions[atCurrentRage]                  = ProcessCurrentRage;
    g_AttributeProcessFunctions[atMaxRageBase]                  = ProcessMaxRageBase;
	g_AttributeProcessFunctions[atMaxRagePercentAdd]            = ProcessMaxRagePercentAdd;
    g_AttributeProcessFunctions[atMaxRageAdditional]            = ProcessMaxRageAdditional;
    g_AttributeProcessFunctions[atRageReplenish]                = ProcessRageReplenish;
    g_AttributeProcessFunctions[atRageReplenishPercent]         = ProcessRageReplenishPercent;

#ifdef _SERVER
    // ------------- 仇恨 -------------------------------------------------->
    g_AttributeProcessFunctions[atGainPositiveThreatCoefficient]    = ProcessGainPositiveThreatCoefficient;
    g_AttributeProcessFunctions[atGainNegativeThreatCoefficient]    = ProcessGainNegativeThreatCoefficient;
    g_AttributeProcessFunctions[atImpairPositiveThreatCoefficient]  = ProcessImpairPositiveThreatCoefficient;
    g_AttributeProcessFunctions[atImpairNegativeThreatCoefficient]  = ProcessImpairNegativeThreatCoefficient;
#endif

    // ------------- 战斗杂项 ---------------------------------------------->
    g_AttributeProcessFunctions[atDodge]                        = ProcessDodge;
    g_AttributeProcessFunctions[atPositiveShield]               = ProcessPositiveShield;
    g_AttributeProcessFunctions[atNegativeShield]               = ProcessNegativeShield;
    g_AttributeProcessFunctions[atGlobalBlock]                  = ProcessGlobalBlock;
    g_AttributeProcessFunctions[atGlobalDamageAbsorb]           = ProcessGlobalDamageAbsorb;
    g_AttributeProcessFunctions[atGlobalDamageManaShield]       = ProcessGlobalDamageManaShield;
    g_AttributeProcessFunctions[atStealLifePercent]             = ProcessStealLifePercent;
    g_AttributeProcessFunctions[atStealManaPercent]             = ProcessStealManaPercent;
    g_AttributeProcessFunctions[atBeatBackRate]                 = ProcessBeatBackRate;
    g_AttributeProcessFunctions[atKnockedBackRate]              = ProcessKnockedBackRate;
    g_AttributeProcessFunctions[atKnockedOffRate]               = ProcessKnockedOffRate;
    
    g_AttributeProcessFunctions[atPhysicsAttackPower]           = ProcessPhysicsAttackPower;
    g_AttributeProcessFunctions[atMagicAttackPower]             = ProcessMagicAttackPower;

    g_AttributeProcessFunctions[atMeleeWeaponDamageBase]        = ProcessMeleeWeaponDamageBase;
    g_AttributeProcessFunctions[atMeleeWeaponDamageRand]        = ProcessMeleeWeaponDamageRand;

    g_AttributeProcessFunctions[atRangeWeaponDamageBase]        = ProcessRangeWeaponDamageBase;
    g_AttributeProcessFunctions[atRangeWeaponDamageRand]        = ProcessRangeWeaponDamageRand;

    // ------------- 外功 ----------------------------------------------------------s

    g_AttributeProcessFunctions[atSkillPhysicsDamage]           = ProcessSkillPhysicsDamage;
    g_AttributeProcessFunctions[atSkillPhysicsDamageRand]       = ProcessSkillPhysicsDamageRand;
    g_AttributeProcessFunctions[atSkillPhysicsDamagePercent]    = ProcessSkillPhysicsDamagePercent;
    g_AttributeProcessFunctions[atPhysicsAttackPowerBase]       = ProcessPhysicsAttackPowerBase;
    g_AttributeProcessFunctions[atPhysicsDamageCoefficient]     = ProcessPhysicsDamageCoefficient;

    g_AttributeProcessFunctions[atPhysicsAttackHit]             = ProcessPhysicsAttackHit;
    g_AttributeProcessFunctions[atPhysicsCriticalStrike]        = ProcessPhysicsCriticalStrike;

    g_AttributeProcessFunctions[atPhysicsResistPercent]         = ProcessPhysicsResistPercent;

    g_AttributeProcessFunctions[atPhysicsDefenceAdd]            = ProcessPhysicsDefenceAdd;
    g_AttributeProcessFunctions[atPhysicsDefenceMax]            = ProcessPhysicsDefenceMax;

    g_AttributeProcessFunctions[atPhysicsShieldBase]            = ProcessPhysicsShieldBase;
    g_AttributeProcessFunctions[atPhysicsShieldPercent]         = ProcessPhysicsShieldPercent;
    g_AttributeProcessFunctions[atPhysicsShieldAdditional]      = ProcessPhysicsShieldAdditional;

    g_AttributeProcessFunctions[atPhysicsReflection]            = ProcessPhysicsReflection;
    g_AttributeProcessFunctions[atPhysicsReflectionPercent]     = ProcessPhysicsReflectionPercent;

    g_AttributeProcessFunctions[atPhysicsBlock]                 = ProcessPhysicsBlock;

    g_AttributeProcessFunctions[atPhysicsDamageAbsorb]          = ProcessPhysicsDamageAbsorb;
    g_AttributeProcessFunctions[atPhysicsDamageManaShield]      = ProcessPhysicsDamageManaShield;
    
    // ------------- 内功 -------------------------------------------------->
    g_AttributeProcessFunctions[atSpunkAttackPower]             = ProcessSpunkAttackPower;
    g_AttributeProcessFunctions[atMeleeSpunkAttackPower]        = ProcessMeleeSpunkAttackPower;
    g_AttributeProcessFunctions[atRangeSpunkAttackPower]        = ProcessRangeSpunkAttackPower;

    // ------------- 阳性内功 ---------------------------------------------->
    g_AttributeProcessFunctions[atSkillSolarDamage]             = ProcessSkillSolarDamage;      
    g_AttributeProcessFunctions[atSkillSolarDamageRand]         = ProcessSkillSolarDamageRand;       
    g_AttributeProcessFunctions[atSkillSolarDamagePercent]      = ProcessSkillSolarDamagePercent;
    g_AttributeProcessFunctions[atSolarAttackPowerBase]         = ProcessSolarAttackPowerBase;   
    g_AttributeProcessFunctions[atSolarDamageCoefficient]       = ProcessSolarDamageCoefficient; 

    g_AttributeProcessFunctions[atSolarMagicHit]                = ProcessSolarMagicHit;
    g_AttributeProcessFunctions[atSolarCriticalStrike]          = ProcessSolarCriticalStrike;
    g_AttributeProcessFunctions[atSolarMagicResistPercent]      = ProcessSolarMagicResistPercent;
    g_AttributeProcessFunctions[atSolarMagicShield]             = ProcessSolarMagicShield;
    g_AttributeProcessFunctions[atSolarMagicReflection]         = ProcessSolarMagicReflection;
    g_AttributeProcessFunctions[atSolarMagicReflectionPercent]  = ProcessSolarMagicReflectionPercent;
    g_AttributeProcessFunctions[atSolarMagicBlock]              = ProcessSolarMagicBlock;
    g_AttributeProcessFunctions[atSolarDamageAbsorb]            = ProcessSolarDamageAbsorb;
    g_AttributeProcessFunctions[atSolarDamageManaShield]        = ProcessSolarDamageManaShield;

    // ------------- 中性内功 ---------------------------------------------->
    g_AttributeProcessFunctions[atSkillNeutralDamage]            = ProcessSkillNeutralDamage;   
    g_AttributeProcessFunctions[atSkillNeutralDamageRand]        = ProcessSkillNeutralDamageRand;   
    g_AttributeProcessFunctions[atSkillNeutralDamagePercent]     = ProcessSkillNeutralDamagePercent;
    g_AttributeProcessFunctions[atNeutralAttackPowerBase]        = ProcessNeutralAttackPowerBase;   
    g_AttributeProcessFunctions[atNeutralDamageCoefficient]      = ProcessNeutralDamageCoefficient; 

    g_AttributeProcessFunctions[atNeutralMagicHit]               = ProcessNeutralMagicHit;
    g_AttributeProcessFunctions[atNeutralCriticalStrike]         = ProcessNeutralCriticalStrike;
    g_AttributeProcessFunctions[atNeutralMagicResistPercent]     = ProcessNeutralMagicResistPercent;
    g_AttributeProcessFunctions[atNeutralMagicShield]            = ProcessNeutralMagicShield;
    g_AttributeProcessFunctions[atNeutralMagicReflection]        = ProcessNeutralMagicReflection;
    g_AttributeProcessFunctions[atNeutralMagicReflectionPercent] = ProcessNeutralMagicReflectionPercent;
    g_AttributeProcessFunctions[atNeutralMagicBlock]             = ProcessNeutralMagicBlock;
    g_AttributeProcessFunctions[atNeutralDamageAbsorb]           = ProcessNeutralDamageAbsorb;
    g_AttributeProcessFunctions[atNeutralDamageManaShield]       = ProcessNeutralDamageManaShield;

    // ------------- 阴性内功 ---------------------------------------------->
    g_AttributeProcessFunctions[atSkillLunarDamage]              = ProcessSkillLunarDamage;    
    g_AttributeProcessFunctions[atSkillLunarDamageRand]          = ProcessSkillLunarDamageRand;  
    g_AttributeProcessFunctions[atSkillLunarDamagePercent]       = ProcessSkillLunarDamagePercent;
    g_AttributeProcessFunctions[atLunarAttackPowerBase]          = ProcessLunarAttackPowerBase;   
    g_AttributeProcessFunctions[atLunarDamageCoefficient]        = ProcessLunarDamageCoefficient; 

    g_AttributeProcessFunctions[atLunarMagicHit]                 = ProcessLunarMagicHit;
    g_AttributeProcessFunctions[atLunarCriticalStrike]           = ProcessLunarCriticalStrike;
    g_AttributeProcessFunctions[atLunarMagicResistPercent]       = ProcessLunarMagicResistPercent;
    g_AttributeProcessFunctions[atLunarMagicShield]              = ProcessLunarMagicShield;
    g_AttributeProcessFunctions[atLunarMagicReflection]          = ProcessLunarMagicReflection;
    g_AttributeProcessFunctions[atLunarMagicReflectionPercent]   = ProcessLunarMagicReflectionPercent;
    g_AttributeProcessFunctions[atLunarMagicBlock]               = ProcessLunarMagicBlock;
    g_AttributeProcessFunctions[atLunarDamageAbsorb]             = ProcessLunarDamageAbsorb;
    g_AttributeProcessFunctions[atLunarDamageManaShield]         = ProcessLunarDamageManaShield;

    // ------------- 毒性 -------------------------------------------------->
    g_AttributeProcessFunctions[atSkillPoisonDamage]             = ProcessSkillPoisonDamage;   
    g_AttributeProcessFunctions[atSkillPoisonDamageRand]         = ProcessSkillPoisonDamageRand;   
    g_AttributeProcessFunctions[atSkillPoisonDamagePercent]      = ProcessSkillPoisonDamagePercent;
    g_AttributeProcessFunctions[atPoisonAttackPowerBase]         = ProcessPoisonAttackPowerBase;   
    g_AttributeProcessFunctions[atPoisonDamageCoefficient]       = ProcessPoisonDamageCoefficient; 

    g_AttributeProcessFunctions[atPoisonMagicHit]                = ProcessPoisonMagicHit;
    g_AttributeProcessFunctions[atPoisonCriticalStrike]          = ProcessPoisonCriticalStrike;
    g_AttributeProcessFunctions[atPoisonMagicResistPercent]      = ProcessPoisonMagicResistPercent;
    g_AttributeProcessFunctions[atPoisonMagicShield]             = ProcessPoisonMagicShield;
    g_AttributeProcessFunctions[atPoisonMagicReflection]         = ProcessPoisonMagicReflection;
    g_AttributeProcessFunctions[atPoisonMagicReflectionPercent]  = ProcessPoisonMagicReflectionPercent;
    g_AttributeProcessFunctions[atPoisonMagicBlock]              = ProcessPoisonMagicBlock;
    g_AttributeProcessFunctions[atPoisonDamageAbsorb]            = ProcessPoisonDamageAbsorb;
    g_AttributeProcessFunctions[atPoisonDamageManaShield]        = ProcessPoisonDamageManaShield;

    // ------------- 治疗 -------------------------------------------------->
    g_AttributeProcessFunctions[atTherapyPowerBase]              = ProcessTherapyPowerBase;   
    g_AttributeProcessFunctions[atSkillTherapy]                  = ProcessSkillTherapy;  
    g_AttributeProcessFunctions[atSkillTherapyRand]              = ProcessSkillTherapyRand;  
    g_AttributeProcessFunctions[atSkillTherapyPercent]           = ProcessSkillTherapyPercent;
    g_AttributeProcessFunctions[atTherapyCoefficient]            = ProcessTherapyCoefficient;   

    // ------------- 新手技能有关 ------------------------------------------>
    g_AttributeProcessFunctions[atNoneWeaponAttackSpeedBase]        = ProcessNoneWeaponAttackSpeedBase;
    g_AttributeProcessFunctions[atMeleeWeaponAttackSpeedBase]       = ProcessMeleeWeaponAttackSpeedBase;
    g_AttributeProcessFunctions[atMeleeWeaponAttackSpeedPercentAdd] = ProcessMeleeWeaponAttackSpeedPercentAdd;
    g_AttributeProcessFunctions[atMeleeWeaponAttackSpeedAdditional] = ProcessMeleeWeaponAttackSpeedAdditional;

    g_AttributeProcessFunctions[atRangeWeaponAttackSpeedBase]       = ProcessMeleeWeaponAttackSpeedBase;
    g_AttributeProcessFunctions[atRangeWeaponAttackSpeedPercentAdd] = ProcessMeleeWeaponAttackSpeedPercentAdd;
    g_AttributeProcessFunctions[atRangeWeaponAttackSpeedAdditional] = ProcessMeleeWeaponAttackSpeedAdditional;

    g_AttributeProcessFunctions[atIgnoreOtherAlarmRange]            = ProcessIgnoreOtherAlarmRange;
    g_AttributeProcessFunctions[atSelfAlarmRangeRevise]             = ProcessSelfAlarmRangeRevise;
    g_AttributeProcessFunctions[atOtherAlarmRangeRevise]            = ProcessOtherAlarmRangeRevise;

#ifdef _SERVER
    // ---------------- 人物状态相关的属性 --------------------------------->
    g_AttributeProcessFunctions[atMeditate]                      = ProcessMeditate;
    g_AttributeProcessFunctions[atDash]                          = ProcessDash;
    g_AttributeProcessFunctions[atStun]                          = ProcessStun;
    g_AttributeProcessFunctions[atHalt]                          = ProcessHalt;
    g_AttributeProcessFunctions[atRevive]                        = ProcessRevive;
    g_AttributeProcessFunctions[atJump]                          = ProcessJump;
    g_AttributeProcessFunctions[atSit]                           = ProcessSit;

    // -------------- 击退 ------------------------------------------------->
    g_AttributeProcessFunctions[atCallKnockedBackExhale]         = ProcessCallKnockedBackExhale;
    g_AttributeProcessFunctions[atCallKnockedBackConvergence]    = ProcessCallKnockedBackConvergence;
    g_AttributeProcessFunctions[atCallKnockedBackRandom]         = ProcessCallKnockedBackRandom;
    g_AttributeProcessFunctions[atCallKnockedBackSrcFace]        = ProcessCallKnockedBackSrcFace;

    // -------------- 击飞 ------------------------------------------------->
    g_AttributeProcessFunctions[atCallKnockedOffParabola]        = ProcessCallKnockedOffParabola; 

    // ------------- 技能运算 --------------------------------------------->
    g_AttributeProcessFunctions[atCallTherapy]                   = ProcessCallTherapy;
    g_AttributeProcessFunctions[atCallPhysicsDamage]             = ProcessCallPhysicsDamage;
    g_AttributeProcessFunctions[atCallSolarDamage]               = ProcessCallSolarDamage;
    g_AttributeProcessFunctions[atCallNeutralDamage]             = ProcessCallNeutralDamage;
    g_AttributeProcessFunctions[atCallLunarDamage]               = ProcessCallLunarDamage;
    g_AttributeProcessFunctions[atCallPoisonDamage]              = ProcessCallPoisonDamage;
    g_AttributeProcessFunctions[atCastSkillTargetSrc]            = ProcessCastSkillTargetSrc;
    g_AttributeProcessFunctions[atCastSkillTargetDst]            = ProcessCastSkillTargetDst;

    g_AttributeProcessFunctions[atSkillEventHandler]             = ProcessSkillEventHandler;
    
    // ------------- 特殊的技能魔法属性 ------------------------------------------------>
    g_AttributeProcessFunctions[atPracticeKungfu]                = ProcessPracticeKungfu;
   

    g_AttributeProcessFunctions[atCallBuff]                      = ProcessBuff;
    g_AttributeProcessFunctions[atCallDebuff]                    = ProcessDebuff;
    g_AttributeProcessFunctions[atCallDot]                       = ProcessDot;
    g_AttributeProcessFunctions[atCallHot]                       = ProcessHot;
    
    // ------------------------- BUFF 驱散 ---------------------------------------------->
    g_AttributeProcessFunctions[atDetachSingleBuff]              = ProcessDetachSingleBuff;
    g_AttributeProcessFunctions[atDetachSingleDebuff]            = ProcessDetachSingleDebuff;
    g_AttributeProcessFunctions[atDetachSingleGroupDot]          = ProcessDetachSingleGroupDot;
    g_AttributeProcessFunctions[atDetachSingleGroupHot]          = ProcessDetachSingleGroupHot;

    g_AttributeProcessFunctions[atDetachMultiGroupBuff]          = ProcessDetachMultiGroupBuff;
    g_AttributeProcessFunctions[atDetachMultiGroupDebuff]        = ProcessDetachMultiGroupDebuff;
    g_AttributeProcessFunctions[atDetachMultiGroupDot]           = ProcessDetachMultiGroupDot;
    g_AttributeProcessFunctions[atDetachMultiGroupHot]           = ProcessDetachMultiGroupHot;

    // -------------------------- 仇恨 ---------------------------------------------------->
    g_AttributeProcessFunctions[atModifyThreat]                  = ProcessModifyThreat;

    g_AttributeProcessFunctions[atExecuteScript]                 = ProcessExecuteScript;

#endif

	return TRUE;
}
