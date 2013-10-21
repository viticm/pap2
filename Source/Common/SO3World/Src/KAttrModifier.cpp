#include "stdafx.h"
#include "KAttrModifier.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KSceneObject.h"
#include "KMath.h"
#include "KPlayerServer.h"
#include "SO3UI/SO3GameWorldUIHandler.h"

// ------------- 移动相关 ------------------------------------------>

BOOL ProcessRunSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;    

#ifdef _SERVER
    pDstCharacter->m_nRunSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRunSpeed = pDstCharacter->m_nRunSpeedBase * 
        (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;
    
    MAKE_IN_RANGE(pDstCharacter->m_nRunSpeed, 0, MAX_VELOCITY_XY);

    pDstCharacter->UpdateMoveParameter();
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMoveSpeedPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

#ifdef _SERVER
    pDstCharacter->m_nMoveSpeedPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nRunSpeed = pDstCharacter->m_nRunSpeedBase * 
        (KILO_NUM + pDstCharacter->m_nMoveSpeedPercent) / KILO_NUM;
    MAKE_IN_RANGE(pDstCharacter->m_nRunSpeed, 0, MAX_VELOCITY_XY);   

    pDstCharacter->UpdateMoveParameter();
#endif
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMaxJumpCount(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nMaxJumpCount += (bApply ? nValue1 : -nValue1);
 
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGravityBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
#ifdef _SERVER
    pDstCharacter->m_nGravityBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nGravity = pDstCharacter->m_nGravityBase * 
        (KILO_NUM + pDstCharacter->m_nGravityPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nGravity, 0, MAX_GRAVITY);

    pDstCharacter->UpdateMoveParameter();
#endif
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGravityPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
#ifdef _SERVER
    pDstCharacter->m_nGravityPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nGravity = pDstCharacter->m_nGravityBase * 
        (KILO_NUM + pDstCharacter->m_nGravityPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nGravity, 0, MAX_GRAVITY);

    pDstCharacter->UpdateMoveParameter();
#endif

	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJumpSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nJumpSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nJumpSpeed = pDstCharacter->m_nJumpSpeedBase * 
        (KILO_NUM + pDstCharacter->m_nJumpSpeedPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nJumpSpeed, 0, MAX_VELOCITY_Z);

	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJumpSpeedPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nJumpSpeedPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nJumpSpeed = pDstCharacter->m_nJumpSpeedBase * 
        (KILO_NUM + pDstCharacter->m_nJumpSpeedPercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nJumpSpeed, 0, MAX_VELOCITY_Z);

	bResult = true;
Exit0:
    return bResult;
}


BOOL ProcessDropDefence(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nDropDefence += (bApply ? nValue1 : -nValue1);
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDivingFrameBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    int  nPreFrame  = pDstCharacter->m_nDivingFrame;
   
    pDstCharacter->m_nDivingFrameBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nDivingFrame = pDstCharacter->m_nDivingFrameBase * 
        (KILO_NUM + pDstCharacter->m_nDivingFramePercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nDivingFrame, 0, USHRT_MAX);

    if (pDstCharacter->m_nDivingFrame < nPreFrame && nPreFrame > 0)
    {
        pDstCharacter->m_nDivingCount = 
            pDstCharacter->m_nDivingFrame * pDstCharacter->m_nDivingCount / nPreFrame;

        if (pDstCharacter->m_nDivingCount < 0)
        {
            pDstCharacter->m_nDivingCount = 0;
        }
    }
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDivingFramePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    int  nPreFrame  = pDstCharacter->m_nDivingFrame;
   
    pDstCharacter->m_nDivingFramePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nDivingFrame = pDstCharacter->m_nDivingFrameBase * 
        (KILO_NUM + pDstCharacter->m_nDivingFramePercent) / KILO_NUM;

    MAKE_IN_RANGE(pDstCharacter->m_nDivingFrame, 0, USHRT_MAX);

    if (pDstCharacter->m_nDivingFrame < nPreFrame && nPreFrame > 0)
    {
        pDstCharacter->m_nDivingCount = 
            pDstCharacter->m_nDivingFrame * pDstCharacter->m_nDivingCount / nPreFrame;

        if (pDstCharacter->m_nDivingCount < 0)
        {
            pDstCharacter->m_nDivingCount = 0;
        }
    }
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessWaterFlyAbility(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nWaterFlyAbility = bApply;
    
	bResult = true;
Exit0:
    return bResult;
}

// ------------ 潜能相关属性处理函数 ------------------>

//力量X点
BOOL ProcessStrengthBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, false, 2 * pDstCharacter->m_nCurrentStrength, 0);

    pDstCharacter->m_nStrengthBase  += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nCurrentStrength = 
        pDstCharacter->m_nStrengthBase * (KILO_NUM + pDstCharacter->m_nStrengthBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentStrength = max(pDstCharacter->m_nCurrentStrength, 0);

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, true, 2 * pDstCharacter->m_nCurrentStrength, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessStrengthBasePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, false, 2 * pDstCharacter->m_nCurrentStrength, 0);

    pDstCharacter->m_nStrengthBasePercentAdd  += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nCurrentStrength = 
        pDstCharacter->m_nStrengthBase * (KILO_NUM + pDstCharacter->m_nStrengthBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentStrength = max(pDstCharacter->m_nCurrentStrength, 0);

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase](pDstCharacter, true, 2 * pDstCharacter->m_nCurrentStrength, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}

//身法X点
BOOL ProcessAgilityBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult             = false;
    int     nAddDodge           = 0;
    int     nAddCriticalStrike  = 0;

    nAddDodge           = 1 * pDstCharacter->m_nCurrentAgility;
    nAddCriticalStrike  = 3 * pDstCharacter->m_nCurrentAgility;

    g_AttributeProcessFunctions[atDodge](pDstCharacter, false, nAddDodge, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, false, nAddCriticalStrike, 0);

    pDstCharacter->m_nAgilityBase += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentAgility =
        pDstCharacter->m_nAgilityBase * (KILO_NUM + pDstCharacter->m_nAgilityBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentAgility = max(pDstCharacter->m_nCurrentAgility, 0);

    nAddDodge           = 1 * pDstCharacter->m_nCurrentAgility;
    nAddCriticalStrike  = 3 * pDstCharacter->m_nCurrentAgility;
    
    g_AttributeProcessFunctions[atDodge](pDstCharacter, true, nAddDodge, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, true, nAddCriticalStrike, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessAgilityBasePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult             = false;
    int     nAddDodge           = 0;
    int     nAddCriticalStrike  = 0;

    nAddDodge           = 1 * pDstCharacter->m_nCurrentAgility;
    nAddCriticalStrike  = 3 * pDstCharacter->m_nCurrentAgility;

    g_AttributeProcessFunctions[atDodge](pDstCharacter, false, nAddDodge, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, false, nAddCriticalStrike, 0);

    pDstCharacter->m_nAgilityBasePercentAdd += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentAgility =
        pDstCharacter->m_nAgilityBase * (KILO_NUM + pDstCharacter->m_nAgilityBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentAgility = max(pDstCharacter->m_nCurrentAgility, 0);

    nAddDodge          = 1 * pDstCharacter->m_nCurrentAgility;
    nAddCriticalStrike = 3 * pDstCharacter->m_nCurrentAgility;
    
    g_AttributeProcessFunctions[atDodge](pDstCharacter, true, nAddDodge, 0);
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](pDstCharacter, true, nAddCriticalStrike, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}


//体质X点
BOOL ProcessVitalityBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;  

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentVitality, 0);

    pDstCharacter->m_nVitalityBase += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentVitality = 
        pDstCharacter->m_nVitalityBase * (KILO_NUM + pDstCharacter->m_nVitalityBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentVitality = max(pDstCharacter->m_nCurrentVitality, 0);

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentVitality, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessVitalityBasePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;  

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentVitality, 0);

    pDstCharacter->m_nVitalityBasePercentAdd += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentVitality = 
        pDstCharacter->m_nVitalityBase * (KILO_NUM + pDstCharacter->m_nVitalityBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentVitality = max(pDstCharacter->m_nCurrentVitality, 0);

    g_AttributeProcessFunctions[atMaxLifeBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentVitality, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}


//根骨X点
BOOL ProcessSpiritBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult                = false;
    int  nSpiritCriticalStrike  = 0;

    nSpiritCriticalStrike = 1 * pDstCharacter->m_nCurrentSpirit;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentSpirit, 0);
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);

    pDstCharacter->m_nSpiritBase  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpirit = 
        pDstCharacter->m_nSpiritBase * (KILO_NUM + pDstCharacter->m_nSpiritBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentSpirit = max(pDstCharacter->m_nCurrentSpirit, 0);

    nSpiritCriticalStrike = 1 * pDstCharacter->m_nCurrentSpirit;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentSpirit, 0);    
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessSpiritBasePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult                = false;
    int  nSpiritCriticalStrike  = 0;

    nSpiritCriticalStrike = 1 * pDstCharacter->m_nCurrentSpirit;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, false, 10 * pDstCharacter->m_nCurrentSpirit, 0);
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, false, nSpiritCriticalStrike, 0);

    pDstCharacter->m_nSpiritBasePercentAdd  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpirit = 
        pDstCharacter->m_nSpiritBase * (KILO_NUM + pDstCharacter->m_nSpiritBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentSpirit = max(pDstCharacter->m_nCurrentSpirit, 0);

    nSpiritCriticalStrike = 1 * pDstCharacter->m_nCurrentSpirit;

    g_AttributeProcessFunctions[atMaxManaBase](pDstCharacter, true, 10 * pDstCharacter->m_nCurrentSpirit, 0);    
    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, true, nSpiritCriticalStrike, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
	return bResult;
}

//精神X点
BOOL ProcessSpunkBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult                = false;
    int  nSpunkLifeReplenish    = 0;
    int  nSpunkManaReplenish    = 0;

    nSpunkLifeReplenish = (pDstCharacter->m_nCurrentSpunk * 3 + 20) / 40 ;
    nSpunkManaReplenish = (pDstCharacter->m_nCurrentSpunk + 20) / 40;

    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, false, nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, false, nSpunkManaReplenish, 0);
    
    g_AttributeProcessFunctions[atSolarAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atNeutralAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atLunarAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atPoisonAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);

    pDstCharacter->m_nSpunkBase  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpunk = 
        pDstCharacter->m_nSpunkBase * (KILO_NUM + pDstCharacter->m_nSpunkBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentSpunk = max(pDstCharacter->m_nCurrentSpunk, 0);

    nSpunkLifeReplenish = (pDstCharacter->m_nCurrentSpunk * 3 + 20) / 40;
    nSpunkManaReplenish = (pDstCharacter->m_nCurrentSpunk + 20) / 40;

    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, true, nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, true, nSpunkManaReplenish, 0);

    g_AttributeProcessFunctions[atSolarAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atNeutralAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atLunarAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atPoisonAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkBasePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult                = false;
    int  nSpunkLifeReplenish    = 0;
    int  nSpunkManaReplenish    = 0;

    nSpunkLifeReplenish = (pDstCharacter->m_nCurrentSpunk * 3 + 20) / 40 ;
    nSpunkManaReplenish = (pDstCharacter->m_nCurrentSpunk + 20) / 40;

    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, false, nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, false, nSpunkManaReplenish, 0);

    g_AttributeProcessFunctions[atSolarAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atNeutralAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atLunarAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atPoisonAttackPowerBase](pDstCharacter, false, 4 * pDstCharacter->m_nCurrentSpunk, 0);

    pDstCharacter->m_nSpunkBasePercentAdd  += (bApply ? nValue1 : -nValue1);
    pDstCharacter->m_nCurrentSpunk = 
        pDstCharacter->m_nSpunkBase * (KILO_NUM + pDstCharacter->m_nSpunkBasePercentAdd) / KILO_NUM;
    pDstCharacter->m_nCurrentSpunk = max(pDstCharacter->m_nCurrentSpunk, 0);

    nSpunkLifeReplenish = (pDstCharacter->m_nCurrentSpunk * 3 + 20) / 40 ;
    nSpunkManaReplenish = (pDstCharacter->m_nCurrentSpunk + 20) / 40;
    
    g_AttributeProcessFunctions[atLifeReplenish](pDstCharacter, true, nSpunkLifeReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenish](pDstCharacter, true, nSpunkManaReplenish, 0);

    g_AttributeProcessFunctions[atSolarAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atNeutralAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atLunarAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);
    g_AttributeProcessFunctions[atPoisonAttackPowerBase](pDstCharacter, true, 4 * pDstCharacter->m_nCurrentSpunk, 0);

    pDstCharacter->UpdateAllState();

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 生命 ---------------------------------------------------->

BOOL ProcessCurrentLife(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);
    
	pDstCharacter->m_nCurrentLife += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxLifeBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
	pDstCharacter->m_nMaxLifeBase += (bApply ? nValue1 : -nValue1);

	pDstCharacter->UpdateBaseState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxLifePercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxLifePercentAdd += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateBaseState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxLifeAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxLifeAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateBaseState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessLifeReplenish(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nLifeReplenish += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLifeReplenishPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nLifeReplenishPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLifeReplenishCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLifeReplenishCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLifeReplenishExt(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLifeReplenishExt += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 内力 ---------------------------------------------------->

BOOL ProcessCurrentMana(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);
        
	pDstCharacter->m_nCurrentMana += (bApply ? nValue1 : -nValue1);

    MAKE_IN_RANGE(pDstCharacter->m_nCurrentMana, 0, pDstCharacter->m_nMaxMana);

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxManaBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
	pDstCharacter->m_nMaxManaBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateBaseState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxManaPercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxManaPercentAdd += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateBaseState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessMaxManaAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

	pDstCharacter->m_nMaxManaAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateBaseState();

    bResult = true;
Exit0:
	return bResult;
}

BOOL ProcessManaReplenish(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nManaReplenish += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessManaReplenishPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nManaReplenishPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessManaReplenishCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nManaReplenishCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessManaReplenishExt(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nManaReplenishExt += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 怒气 ---------------------------------------------------->
BOOL ProcessCurrentRage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nCurrentRage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMaxRage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nMaxRage += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRageReplenish(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRageReplenish += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 精力 ---------------------------------------------------->
BOOL ProcessCurrentStamina(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = IS_PLAYER(pDstCharacter->m_dwID);
    KG_PROCESS_ERROR(bRetCode);

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nCurrentStamina += nValue1;

    MAKE_IN_RANGE(pPlayer->m_nCurrentStamina, 0, pPlayer->m_nMaxStamina);
        
    bResult = true;
Exit0:
    return bResult;
}

// ---------------- 体力 ---------------------------------------------------->
BOOL ProcessCurrentThew(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

    KGLOG_PROCESS_ERROR(bApply);
        
    bRetCode = IS_PLAYER(pDstCharacter->m_dwID);
    KG_PROCESS_ERROR(bRetCode);

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nCurrentThew += nValue1;

    MAKE_IN_RANGE(pPlayer->m_nCurrentThew, 0, pPlayer->m_nMaxThew);

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
// ------------------ 仇恨 -------------------------------------------------->
BOOL ProcessActiveThreatCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    switch (nValue1)
    {
    case thtMainThreat:
        pDstCharacter->m_nActiveBaseThreatCoefficient       += (bApply) ? nValue2 : -nValue2;
        pDstCharacter->m_nActiveDamageThreatCoefficient     += (bApply) ? nValue2 : -nValue2;
        pDstCharacter->m_nActiveTherapyThreatCoefficient    += (bApply) ? nValue2 : -nValue2;
        break;
    case thtBaseThreat:
        pDstCharacter->m_nActiveBaseThreatCoefficient       += (bApply) ? nValue2 : -nValue2;
        break;
    case thtDamageThreat:
        pDstCharacter->m_nActiveDamageThreatCoefficient     += (bApply) ? nValue2 : -nValue2;
        break;
    case thtTherapyThreat:
        pDstCharacter->m_nActiveTherapyThreatCoefficient    += (bApply) ? nValue2 : -nValue2;
        break;

    default:
        KGLogPrintf(KGLOG_ERR, "Invalid Threat Type!");
        goto Exit0;
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPassiveThreatCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    switch (nValue1)
    {
    case thtMainThreat:
        pDstCharacter->m_nPassiveBaseThreatCoefficient       += (bApply) ? nValue2 : -nValue2;
        pDstCharacter->m_nPassiveDamageThreatCoefficient     += (bApply) ? nValue2 : -nValue2;
        pDstCharacter->m_nPassiveTherapyThreatCoefficient    += (bApply) ? nValue2 : -nValue2;
        break;
    case thtBaseThreat:
        pDstCharacter->m_nPassiveBaseThreatCoefficient       += (bApply) ? nValue2 : -nValue2;
        break;
    case thtDamageThreat:
        pDstCharacter->m_nPassiveDamageThreatCoefficient     += (bApply) ? nValue2 : -nValue2;
        break;
    case thtTherapyThreat:
        pDstCharacter->m_nPassiveTherapyThreatCoefficient    += (bApply) ? nValue2 : -nValue2;
        break;

    default:
        KGLogPrintf(KGLOG_ERR, "Invalid Threat Type!");
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

// ------------------ 战斗杂项 ----------------------------------------------->

BOOL ProcessDodge(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;  

    pDstCharacter->m_nDodge += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDodgeBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nDodgeBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessParryBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nParryBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessParryBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nParryBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nParry =
        pDstCharacter->m_nParryBase * pDstCharacter->m_nParryPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessParryPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nParryPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nParry =
        pDstCharacter->m_nParryBase * pDstCharacter->m_nParryPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessParryValueBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nParryValueBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllBattleState();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessParryValuePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nParryValuePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllBattleState();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSense(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSense += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessStrainBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nStrainBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nStrain =
        pDstCharacter->m_nStrainBase * pDstCharacter->m_nStrainPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessStrainPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nStrainPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nStrain =
        pDstCharacter->m_nStrainBase * pDstCharacter->m_nStrainPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessToughnessBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nToughnessBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessToughnessBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nToughnessBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nToughness =
        pDstCharacter->m_nToughnessBase * pDstCharacter->m_nToughnessPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessToughnessPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nToughnessPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nToughness =
        pDstCharacter->m_nToughnessBase * pDstCharacter->m_nToughnessPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDecriticalDamagePowerBaseKiloNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nDecriticalDamagePowerBaseKiloNumRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDecriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nDecriticalDamagePowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nDecriticalDamagePower =
        pDstCharacter->m_nDecriticalDamagePowerBase * pDstCharacter->m_nDecriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDecriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nDecriticalDamagePowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nDecriticalDamagePower =
        pDstCharacter->m_nDecriticalDamagePowerBase * pDstCharacter->m_nDecriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

// 修为
BOOL ProcessTransmitTrain(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nTransmitValue  = nValue1;
    KPlayer*    pDstPlayer      = NULL;
    KPlayer*    pSrcPlayer      = NULL;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc);

    KGLOG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));
    KGLOG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_dwID));

#ifdef _SERVER
    pDstPlayer = (KPlayer*)pDstCharacter;
    pSrcPlayer = (KPlayer*)pDstCharacter->m_pSkillAttributeParam->pSkillSrc;

    nTransmitValue = min(pSrcPlayer->m_nCurrentTrainValue, nTransmitValue);

    if (nTransmitValue > 0)
    {
        int nAddTrain = nTransmitValue * nValue2 / KILO_NUM;
        nAddTrain = min((pDstPlayer->m_nMaxTrainValue - pDstPlayer->m_nCurrentTrainValue), nAddTrain);

        if (nAddTrain != nTransmitValue * nValue2 / KILO_NUM) // 并不是每次都需要修改，
        {                                                     // 否则会造成不需要修改的时候不等于nValue1的情况
            nTransmitValue = min(nAddTrain * KILO_NUM / nValue2, nValue1); // 有可能受对方上限的影响，导致支出方的支出修为修改。
        }

        KG_PROCESS_ERROR(nAddTrain > 0 && nTransmitValue > 0);

        bRetCode = pSrcPlayer->AddTrain(-nTransmitValue);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = pDstPlayer->AddTrain(nAddTrain);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMaxTrainValue(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(nValue1 >= 0);

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        pPlayer->m_nMaxTrainValue += (bApply ? nValue1 : -nValue1);

    #ifdef _CLIENT
        {
            KUIEventTrainValueUpdate param;
            KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

            param.nAddTrain = 0;
            g_pGameWorldUIHandler->OnTrainValueUpdate(param);
        }
    #endif
    }
    // NPC没修为这回事

    bResult = true;
Exit0:
    return bResult;
}

// 筋脉
BOOL ProcessBaseVenationCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        pPlayer->m_nBaseVenationCof += (bApply ? nValue1 : -nValue1);
    }
    // NPC没筋脉这回事

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsVenationCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        pPlayer->m_nPhysicsVenationCof += (bApply ? nValue1 : -nValue1);
    }
    // NPC没筋脉这回事

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicVenationCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        pPlayer->m_nMagicVenationCof += (bApply ? nValue1 : -nValue1);
    }
    // NPC没筋脉这回事

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAssistVenationCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        pPlayer->m_nAssistVenationCof += (bApply ? nValue1 : -nValue1);
    }
    // NPC没筋脉这回事

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessImmortal(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult = false;

    pDstCharacter->m_bImmortal = bApply;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMountingRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult = false;

    pDstCharacter->m_nMountingRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPositiveShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_bPositiveShield = bApply;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNegativeShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_bNegativeShield = bApply;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGlobalResistPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atPhysicsResistPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atSolarMagicResistPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralMagicResistPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarMagicResistPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonMagicResistPercent](pDstCharacter, bApply, nValue1, nValue2);

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

BOOL ProcessGlobalDamageAbsorb(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;   

    pDstCharacter->m_nGlobalDamageAbsorb += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nGlobalDamageAbsorb < 0)
    {
        pDstCharacter->m_nGlobalDamageAbsorb = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessGlobalDamageManaShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;  

    pDstCharacter->m_nGlobalDamageManaShield += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nGlobalDamageManaShield < 0)
    {
        pDstCharacter->m_nGlobalDamageManaShield = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDamageToLifeForSelf(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nDamageToLifeForSelf += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDamageToManaForSelf(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nDamageToManaForSelf += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessBeatBackRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nBeatBackRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessKnockedBackRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
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

BOOL ProcessKnockedDownRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nKnockedDownRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPullRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPullRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRepulsedRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRepulsedRate += (bApply ? nValue1 : -nValue1);

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

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsAttackPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsAttackPowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

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

BOOL ProcessPhysicsHitBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nPhysicsHitBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsHitValue(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nPhysicsHitValue += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsCriticalStrikeBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsCriticalStrikeBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nPhysicsCriticalStrikeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsCriticalDamagePowerBaseKlioNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsCriticalDamagePowerBaseKiloNumRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsCriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsCriticalDamagePowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsCriticalDamagePower =
        pDstCharacter->m_nPhysicsCriticalDamagePowerBase * pDstCharacter->m_nPhysicsCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsCriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsCriticalDamagePowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsCriticalDamagePower =
        pDstCharacter->m_nPhysicsCriticalDamagePowerBase * pDstCharacter->m_nPhysicsCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsOvercomeBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsOvercomeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsOvercome =
        pDstCharacter->m_nPhysicsOvercomeBase * pDstCharacter->m_nPhysicsOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsOvercomePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPhysicsOvercomePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPhysicsOvercome =
        pDstCharacter->m_nPhysicsOvercomeBase * pDstCharacter->m_nPhysicsOvercomePercent / KILO_NUM;

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

BOOL ProcessPhysicsDefenceMax(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsDefenceMax += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsShieldBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsShieldBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsShieldPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsShieldPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsShieldAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsShieldAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsReflection(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsReflectionPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsBlock(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDamageAbsorb(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
   
    pDstCharacter->m_nPhysicsDamageAbsorb += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nPhysicsDamageAbsorb < 0)
    {
        pDstCharacter->m_nPhysicsDamageAbsorb = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPhysicsDamageManaShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;    

    pDstCharacter->m_nPhysicsDamageManaShield += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nPhysicsDamageManaShield < 0)
    {
        pDstCharacter->m_nPhysicsDamageManaShield = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}


// ------------------ 作用基本潜能 ----------------------------------------------->
BOOL ProcessBasePotentialAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atStrengthBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atAgilityBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atVitalityBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atSpiritBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atSpunkBase](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}
// ------------------ 作用所有内功 ----------------------------------------------->
BOOL ProcessSkillMagicDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSkillSolarDamage](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atSkillNeutralDamage](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atSkillLunarDamage](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atSkillPoisonDamage](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarAttackPowerBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralAttackPowerBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarAttackPowerBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonAttackPowerBase](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicAttackPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarAttackPowerPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralAttackPowerPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarAttackPowerPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonAttackPowerPercent](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarCriticalStrike](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarCriticalStrike](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarMagicShieldBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralMagicShieldBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarMagicShieldBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonMagicShieldBase](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicCriticalDamagePowerBaseKiloNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarCriticalDamagePowerBaseKiloNumRate](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralCriticalDamagePowerBaseKiloNumRate](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarCriticalDamagePowerBaseKiloNumRate](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonCriticalDamagePowerBaseKiloNumRate](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicCriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarCriticalDamagePowerBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralCriticalDamagePowerBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarCriticalDamagePowerBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonCriticalDamagePowerBase](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicCriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarCriticalDamagePowerPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralCriticalDamagePowerPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarCriticalDamagePowerPercent](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonCriticalDamagePowerPercent](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicOvercome(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarOvercomeBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralOvercomeBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarOvercomeBase](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonOvercomeBase](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicHitBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarHitBaseRate](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralHitBaseRate](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarHitBaseRate](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonHitBaseRate](pDstCharacter, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMagicHitValue(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    g_AttributeProcessFunctions[atSolarHitValue](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atNeutralHitValue](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atLunarHitValue](pDstCharacter, bApply, nValue1, nValue2);
    g_AttributeProcessFunctions[atPoisonHitValue](pDstCharacter, bApply, nValue1, nValue2);

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

BOOL ProcessSolarDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;    

    pDstCharacter->m_nSolarDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessSolarAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarAttackPowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessSolarAttackPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarAttackPowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarHitBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarHitBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarHitValue(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    pDstCharacter->m_nSolarHitValue += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarCriticalStrikeBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarCriticalStrikeBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarCriticalStrikeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarCriticalDamagePowerBaseKlioNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarCriticalDamagePowerBaseKiloNumRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarCriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarCriticalDamagePowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nSolarCriticalDamagePower =
        pDstCharacter->m_nSolarCriticalDamagePowerBase * pDstCharacter->m_nSolarCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarCriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarCriticalDamagePowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nSolarCriticalDamagePower =
        pDstCharacter->m_nSolarCriticalDamagePowerBase * pDstCharacter->m_nSolarCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarOvercomeBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarOvercomeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nSolarOvercome =
        pDstCharacter->m_nSolarOvercomeBase * pDstCharacter->m_nSolarOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarOvercomePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSolarOvercomePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nSolarOvercome =
        pDstCharacter->m_nSolarOvercomeBase * pDstCharacter->m_nSolarOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicResistPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicShieldBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicShieldBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicShieldPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicShieldPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicReflection(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicReflectionPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarMagicBlock(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarDamageAbsorb(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarDamageAbsorb += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nSolarDamageAbsorb < 0)
    {
        pDstCharacter->m_nSolarDamageAbsorb = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSolarDamageManaShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nSolarDamageManaShield += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nSolarDamageManaShield < 0)
    {
        pDstCharacter->m_nSolarDamageManaShield = 0;
    }

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

BOOL ProcessNeutralDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessNeutralAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralAttackPowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessNeutralAttackPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralAttackPowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralHitBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralHitBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralHitValue(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralHitValue += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralCriticalStrikeBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralCriticalStrikeBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralCriticalStrikeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralCriticalDamagePowerBaseKlioNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralCriticalDamagePowerBaseKiloNumRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralCriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralCriticalDamagePowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nNeutralCriticalDamagePower =
        pDstCharacter->m_nNeutralCriticalDamagePowerBase * pDstCharacter->m_nNeutralCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralCriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralCriticalDamagePowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nNeutralCriticalDamagePower =
        pDstCharacter->m_nNeutralCriticalDamagePowerBase * pDstCharacter->m_nNeutralCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralOvercomeBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralOvercomeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nNeutralOvercome =
        pDstCharacter->m_nNeutralOvercomeBase * pDstCharacter->m_nNeutralOvercomePercent  / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralOvercomePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNeutralOvercomePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nNeutralOvercome =
        pDstCharacter->m_nNeutralOvercomeBase * pDstCharacter->m_nNeutralOvercomePercent  / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicResistPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicShieldBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicShieldBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicShieldPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicShieldPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicReflection(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicReflectionPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralMagicBlock(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralDamageAbsorb(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralDamageAbsorb += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nNeutralDamageAbsorb < 0)
    {
        pDstCharacter->m_nNeutralDamageAbsorb = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNeutralDamageManaShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nNeutralDamageManaShield += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nNeutralDamageManaShield < 0)
    {
        pDstCharacter->m_nNeutralDamageManaShield = 0;
    }

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

BOOL ProcessLunarDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessLunarAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarAttackPowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessLunarAttackPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarAttackPowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}   

BOOL ProcessLunarHitBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarHitBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarHitValue(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    
    pDstCharacter->m_nLunarHitValue += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarCriticalStrikeBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarCriticalStrikeBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarCriticalStrikeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarCriticalDamagePowerBaseKlioNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarCriticalDamagePowerBaseKiloNumRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarCriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarCriticalDamagePowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nLunarCriticalDamagePower =
        pDstCharacter->m_nLunarCriticalDamagePowerBase * pDstCharacter->m_nLunarCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarCriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarCriticalDamagePowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nLunarCriticalDamagePower =
        pDstCharacter->m_nLunarCriticalDamagePowerBase * pDstCharacter->m_nLunarCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarOvercomeBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarOvercomeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nLunarOvercome = 
        pDstCharacter->m_nLunarOvercomeBase * pDstCharacter->m_nLunarOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarOvercomePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nLunarOvercomePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nLunarOvercome = 
        pDstCharacter->m_nLunarOvercomeBase * pDstCharacter->m_nLunarOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicResistPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicShieldBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicShieldBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicShieldPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicShieldPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicReflection(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicReflectionPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarMagicBlock(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarDamageAbsorb(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarDamageAbsorb += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nLunarDamageAbsorb < 0)
    {
        pDstCharacter->m_nLunarDamageAbsorb = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessLunarDamageManaShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nLunarDamageManaShield += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nLunarDamageManaShield < 0)
    {
        pDstCharacter->m_nLunarDamageManaShield = 0;
    }

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

BOOL ProcessPoisonDamageCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonDamageCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
} 

BOOL ProcessPoisonAttackPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonAttackPowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonAttackPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonAttackPowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonHitBaseRate(KCharacter*  pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonHitBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonHitValue(KCharacter*  pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonHitValue += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonCriticalStrikeBaseRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonCriticalStrikeBaseRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonCriticalStrike(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonCriticalStrikeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonCriticalDamagePowerBaseKiloNumRate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonCriticalDamagePowerBaseKiloNumRate += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonCriticalDamagePowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonCriticalDamagePowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPoisonCriticalDamagePower =
        pDstCharacter->m_nPoisonCriticalDamagePowerBase * pDstCharacter->m_nPoisonCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonCriticalDamagePowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonCriticalDamagePowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPoisonCriticalDamagePower =
        pDstCharacter->m_nPoisonCriticalDamagePowerBase * pDstCharacter->m_nPoisonCriticalDamagePowerPercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonOvercomeBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonOvercomeBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPoisonOvercome =
        pDstCharacter->m_nPoisonOvercomeBase * pDstCharacter->m_nPoisonOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonOvercomePercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nPoisonOvercomePercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->m_nPoisonOvercome =
        pDstCharacter->m_nPoisonOvercomeBase * pDstCharacter->m_nPoisonOvercomePercent / KILO_NUM;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicResistPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicResistPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicShieldBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicShieldBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicShieldPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicShieldPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicReflection(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicReflection += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicReflectionPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicReflectionPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonMagicBlock(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonMagicBlock += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonDamageAbsorb(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonDamageAbsorb += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nPoisonDamageAbsorb < 0)
    {
        pDstCharacter->m_nPoisonDamageAbsorb = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPoisonDamageManaShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    pDstCharacter->m_nPoisonDamageManaShield += (bApply ? nValue1 : -nValue1);

    if (pDstCharacter->m_nPoisonDamageManaShield < 0)
    {
        pDstCharacter->m_nPoisonDamageManaShield = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 治疗 ------------------------------------------------------------>
BOOL ProcessTherapyPowerBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
        
    pDstCharacter->m_nTherapyPowerBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTherapyPowerPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nTherapyPowerPercent += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillTherapy(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillTherapy += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillTherapyRand(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillTherapyRand += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSkillTherapyPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSkillTherapyPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;

}

BOOL ProcessTherapyCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nTherapyCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessBeTherapyCoefficient(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nBeTherapyCoefficient += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessNoneWeaponAttackSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nNoneWeaponAttackSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponAttackSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nMeleeWeaponAttackSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponAttackSpeedPercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nMeleeWeaponAttackSpeedPercentAdd += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMeleeWeaponAttackSpeedAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nMeleeWeaponAttackSpeedAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponAttackSpeedBase(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRangeWeaponAttackSpeedBase += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponAttackSpeedPercentAdd(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRangeWeaponAttackSpeedPercentAdd += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRangeWeaponAttackSpeedAdditional(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nRangeWeaponAttackSpeedAdditional += (bApply ? nValue1 : -nValue1);

    pDstCharacter->UpdateWeaponAttackSpeed();

    bResult = true;
Exit0:
    return bResult;
}


BOOL ProcessIgnoreOtherAlarmRange(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nIgnoreOtherAlarmRange += (bApply ? 1 : -1);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_nIgnoreOtherAlarmRange >= 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSelfAlarmRangeRevise(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nSelfAlarmRangeRevise += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessOtherAlarmRangeRevise(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    pDstCharacter->m_nOtherAlarmRangeRevise += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessUnarm(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult  = false;
    BOOL    bRetCode = false;

    pDstCharacter->m_nUnarmCounter += bApply ? 1 : -1;
    assert(pDstCharacter->m_nUnarmCounter >= 0);

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        if (pDstCharacter->m_nUnarmCounter == 1)
        {
            KPlayer* pPlayer = (KPlayer*)pDstCharacter;
            KItem*  pItem    = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
            if (pItem && pItem->m_nCurrentDurability > 0)
            {
                bRetCode = pPlayer->m_ItemList.UnApplyAttrib(pItem);
                KGLOG_CHECK_ERROR(bRetCode);
            }
        }
        else if (pDstCharacter->m_nUnarmCounter == 0)
        {
            KPlayer* pPlayer = (KPlayer*)pDstCharacter;
            KItem*  pItem    = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
            if (pItem && pItem->m_nCurrentDurability > 0)
            {
                bRetCode = pPlayer->m_ItemList.ApplyAttrib(pItem);
                KGLOG_CHECK_ERROR(bRetCode);
            }
        }
        //else 其他情况不处理
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
// ------------- 冲锋 ------------------------------------------------------------>
BOOL ProcessDash(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KCharacter* pSkillSrc       = NULL;
    int         nSrcZ           = 0;
    int         nDstZ           = 0;
    int         nDeltaZ         = 0;
    int         nDeltaX         = 0;
    int         nDeltaY         = 0;
    int         nDeltaXY        = 0;
    int         nDeltaXYZ       = 0;
    int         nFrame          = 0;
    int         nDirectionXY    = 0;  
	int         nVelocityXY     = 0;
    int         nVelocityZ      = 0;

    KGLOG_PROCESS_ERROR(nValue1 > 0);

    if (nValue1 > MAX_VELOCITY_XY)
    {
        nValue1 = MAX_VELOCITY_XY;
    }

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    pSkillSrc = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc && pSkillSrc != pDstCharacter);
    KGLOG_PROCESS_ERROR(pSkillSrc->m_pCell && pDstCharacter->m_pCell);

    nSrcZ = pSkillSrc->m_nZ;
    nDstZ = pDstCharacter->m_nZ;

    nDeltaZ = ZPOINT_TO_XYPOINT(nDstZ - nSrcZ);

    nDeltaX = pDstCharacter->m_nX - pSkillSrc->m_nX;
    nDeltaY = pDstCharacter->m_nY - pSkillSrc->m_nY;

    nDeltaXY = (int)sqrt((double)(nDeltaX * nDeltaX + nDeltaY * nDeltaY));
    nDeltaXY -= pSkillSrc->m_nTouchRange + pDstCharacter->m_nTouchRange + 3 * CELL_LENGTH;
    nDeltaXY = max(0, nDeltaXY);

    nDeltaXYZ = (int)sqrt((double)(nDeltaX * nDeltaX + nDeltaY * nDeltaY + nDeltaZ * nDeltaZ));
    KGLOG_PROCESS_ERROR(nDeltaXYZ > 0);

    nDirectionXY = g_GetDirection(0, 0, nDeltaX, nDeltaY);

    nVelocityXY = nValue1 * nDeltaXY / nDeltaXYZ;
    nVelocityZ = XYPOINT_TO_ZPOINT(nValue1 * nDeltaZ / nDeltaXYZ);

    nFrame = (nDeltaXYZ + nValue1 / 2) / nValue1;

	bRetCode = pSkillSrc->Dash(nFrame, nDirectionXY, nVelocityXY, nVelocityZ);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDashForward(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;    
    int     nFrame      = nValue1;
	int     nVelocityXY = nValue2;

	bRetCode = pDstCharacter->Dash(
        nFrame, pDstCharacter->m_nFaceDirection, 
        nVelocityXY, pDstCharacter->m_nVelocityZ
    );
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDashBackward(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;    
    int     nFrame      = nValue1;
	int     nVelocityXY = nValue2;

	bRetCode = pDstCharacter->Dash(
        nFrame, pDstCharacter->m_nFaceDirection + DIRECTION_COUNT / 2, 
        nVelocityXY, pDstCharacter->m_nVelocityZ
    );
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDashLeft(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;    
    int     nFrame      = nValue1;
	int     nVelocityXY = nValue2;

	bRetCode = pDstCharacter->Dash(
        nFrame, pDstCharacter->m_nFaceDirection + DIRECTION_COUNT / 4, 
        nVelocityXY, pDstCharacter->m_nVelocityZ
    );
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDashRight(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;    
    int     nFrame      = nValue1;
	int     nVelocityXY = nValue2;

	bRetCode = pDstCharacter->Dash(
        nFrame, pDstCharacter->m_nFaceDirection - DIRECTION_COUNT / 4, 
        nVelocityXY, pDstCharacter->m_nVelocityZ
    );
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDashToDstBack(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KCharacter* pSkillSrc       = NULL;
    int         nSrcZ           = 0;
    int         nDstZ           = 0;
    int         nDeltaZ         = 0;
    int         nDeltaX         = 0;
    int         nDeltaY         = 0;
    int         nDeltaXY        = 0;
    int         nDeltaXYZ       = 0;
    int         nFrame          = 0;
    int         nDirectionXY    = 0;  
	int         nVelocityXY     = 0;
    int         nVelocityZ      = 0;

    KGLOG_PROCESS_ERROR(nValue1 > 0);

    if (nValue1 > MAX_VELOCITY_XY)
    {
        nValue1 = MAX_VELOCITY_XY;
    }

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    pSkillSrc = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc && pSkillSrc != pDstCharacter);
    KGLOG_PROCESS_ERROR(pSkillSrc->m_pCell && pDstCharacter->m_pCell);

    nSrcZ = pSkillSrc->m_nZ;
    nDstZ = pDstCharacter->m_nZ;

    nDeltaZ = ZPOINT_TO_XYPOINT(nDstZ - nSrcZ);

    nDeltaX = pDstCharacter->m_nX - pSkillSrc->m_nX;
    nDeltaY = pDstCharacter->m_nY - pSkillSrc->m_nY;

    nDeltaXY = (int)sqrt((double)(nDeltaX * nDeltaX + nDeltaY * nDeltaY));
    nDeltaXY += pSkillSrc->m_nTouchRange + pDstCharacter->m_nTouchRange + 3 * CELL_LENGTH;
    nDeltaXY = max(0, nDeltaXY);

    nDeltaXYZ = (int)sqrt((double)(nDeltaX * nDeltaX + nDeltaY * nDeltaY + nDeltaZ * nDeltaZ));
    KGLOG_PROCESS_ERROR(nDeltaXYZ > 0);

    nDirectionXY = g_GetDirection(0, 0, nDeltaX, nDeltaY);

    nVelocityXY = nValue1 * nDeltaXY / nDeltaXYZ;
    nVelocityZ = XYPOINT_TO_ZPOINT(nValue1 * nDeltaZ / nDeltaXYZ);

    nFrame = (nDeltaXYZ + nValue1 / 2) / nValue1;

	bRetCode = pSkillSrc->Dash(nFrame, nDirectionXY, nVelocityXY, nVelocityZ);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

#endif

// ------------- 击晕, 眩晕 ------------------------------------------------------->
BOOL ProcessHalt(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    
    if (bApply)
    {
        pDstCharacter->Halt();
    }
    else
    {
        if (pDstCharacter->m_eMoveState == cmsOnHalt)
        {
            pDstCharacter->Stop();
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessFreeze(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        pDstCharacter->Freeze();
    }
    else
    {
        if (pDstCharacter->m_eMoveState == cmsOnFreeze)
        {
            pDstCharacter->Stop();
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessEntrap(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        pDstCharacter->Entrap();
    }
    else
    {
        if (pDstCharacter->m_eMoveState == cmsOnEntrap)
        {
            pDstCharacter->Stop();
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessFear(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
   
    if (bApply)
    {
        pDstCharacter->m_nFearCounter++;
        pDstCharacter->ProcessFear(nValue1);

        if (IS_PLAYER(pDstCharacter->m_dwID))
        {
            ((KPlayer*)pDstCharacter)->m_MoveCtrl.bDisable = true;
        }
    }
    else
    {
        pDstCharacter->m_nFearCounter--;

        if (pDstCharacter->m_nFearCounter == 0)
        {
            #ifdef _SERVER
            pDstCharacter->Stop();
            pDstCharacter->m_AIVM.FireEvent(aevOnOutFear, pDstCharacter->m_dwID, 0);
            #endif

            if (IS_PLAYER(pDstCharacter->m_dwID))
            {
                ((KPlayer*)pDstCharacter)->m_MoveCtrl.bDisable = false;
            }
        }

        KGLOG_PROCESS_ERROR(pDstCharacter->m_nFearCounter >= 0);
    }
    
    bResult = true;
Exit0:
    return bResult;
}
#ifdef _SERVER
BOOL ProcessRevive(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessRevive(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessJump(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KGLOG_PROCESS_ERROR(bApply);

    // TODO: 这里,跳跃的方向等还要进一步确定怎么计算

    pDstCharacter->Jump(false, pDstCharacter->m_nFaceDirection, true);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSit(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->SitDown(true);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackExhale(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    KGLOG_PROCESS_ERROR(bApply);
    
    pDstCharacter->ProcessKnockedBackExhale(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackConvergence(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;
    
    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessKnockedBackConvergence(nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackRandom(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessKnockedBackRandom(nValue1, nValue2);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallKnockedBackSrcFace(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    
    KGLOG_PROCESS_ERROR(bApply);

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

BOOL ProcessCallKnockedDown(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessKnockedDown(nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 滑步 ------------------------------------------------------------->
BOOL ProcessRepulsed(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);
    pDstCharacter->ProcessRepulsed(nValue1);

    bResult = true;
Exit0:
    return bResult;
}

// ------------- 技能运算 --------------------------------------------------------->
BOOL ProcessCallTherapy(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;
    
    
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pDstSkillCalculateResult);
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessTherapy(nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallPhysicsDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KGLOG_PROCESS_ERROR(bApply);
    
    pDstCharacter->ProcessDamage(sktPhysics, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallSolarDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessDamage(sktSolarMagic, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallNeutralDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessDamage(sktNeutralMagic, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallLunarDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessDamage(sktLunarMagic, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallPoisonDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult  = false;

    
    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->ProcessDamage(sktPoison, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCastSkillTargetSrc(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    DWORD       dwSkillID           = (DWORD)nValue1;
    DWORD       dwSkillLevel        = (DWORD)nValue2;
    KCharacter *pTargetCharacter    = NULL;
    KTarget     Target;

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    pTargetCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pTargetCharacter);
    
    bRetCode = Target.SetTarget(pDstCharacter);
    KGLOG_PROCESS_ERROR(bRetCode);

    pDstCharacter->CastSkillSub(pDstCharacter, dwSkillID, dwSkillLevel, Target);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCastSkillTargetDst(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    DWORD       dwSkillID           = (DWORD)nValue1;
    DWORD       dwSkillLevel        = (DWORD)nValue2;
    KCharacter *pTargetCharacter    = NULL;
    KTarget     Target;

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    pTargetCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillDst;
    assert(pTargetCharacter);

    bRetCode = Target.SetTarget(pTargetCharacter);
    KG_PROCESS_ERROR(bRetCode);

    pDstCharacter->CastSkillSub(pDstCharacter, dwSkillID, dwSkillLevel, Target);

    bResult = true;
Exit0:
    return bResult;
}

struct KIS_SAME_EVENT_ID 
{
    inline bool operator()(const KUSER_SKILL_EVENT& crValue)
    {
        return crValue.dwEventID == dwEventID;
    }

    DWORD dwEventID;
};

BOOL ProcessPracticeKungfu(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    /*DWORD           dwKungFuID      = nValue1;
    DWORD           dwKungFuLevel   = 0;
    DWORD           dwExp           = 0;
    KSkill*         pSkill          = NULL;
    KPlayer*        pPlayer         = NULL;
    int             nCostQiPoint    = 0;
    KKUNG_FU_EXP*   pKungFuExp      = NULL;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));
    KGLOG_PROCESS_ERROR(pDstCharacter->m_eKind == ckPlayer);
    
    pPlayer = (KPlayer*)pDstCharacter;

    dwKungFuLevel = pPlayer->m_SkillList.GetSkillLevel(dwKungFuID);
    KGLOG_PROCESS_ERROR(dwKungFuLevel > 0);

    bRetCode = g_pSO3World->m_SkillManager.GetKungFuExp(dwKungFuID, dwKungFuLevel, &pKungFuExp);
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
*/
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAccumulate(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int Value2)
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;

    KGLOG_PROCESS_ERROR(bApply);

    pDstCharacter->m_nAccumulateValue += nValue1;

    MAKE_IN_RANGE(pDstCharacter->m_nAccumulateValue, 0, MAX_ACCUMULATE_VALUE);

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        g_PlayerServer.DoSyncAccumulateValue((KPlayer*)pDstCharacter);
    }

    pDstCharacter->OnSkillEvent(NULL, seAccumulate, pDstCharacter->m_SkillEventVector, pDstCharacter, pDstCharacter);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallBuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    DWORD   dwCasterID      = 0;
    int     nCasterLevel    = 0;

    KBUFF_RECIPE_KEY BuffRecipeKey;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    dwCasterID      = pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID;
    nCasterLevel    = pDstCharacter->m_pSkillAttributeParam->nSkillSrcLevel;

    memset(&BuffRecipeKey, 0, sizeof(BuffRecipeKey));

    BuffRecipeKey.dwID      = nValue1;
    BuffRecipeKey.nLevel    = nValue2;

    bRetCode = pDstCharacter->m_BuffList.CallBuff(dwCasterID, nCasterLevel, BuffRecipeKey, 0, 0);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

static int _GetBuffLevelAccordingToAttraction(KCharacter* pSrcCharacter, KCharacter* pDstCharacter, int nRadius)
{
    int             nResult             = 0;
    BOOL            bRetCode            = false;
    int             nAttractionLevel    = 0;
    KGFellowship*   pFellowship         = NULL;

    assert(pSrcCharacter->m_dwID != ERROR_ID);
    assert(pDstCharacter->m_dwID != ERROR_ID);

    bRetCode = IS_PLAYER(pSrcCharacter->m_dwID);
    KG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(pSrcCharacter->m_pScene);

    bRetCode = IS_PLAYER(pDstCharacter->m_dwID);
    KG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pScene);

    if (pSrcCharacter->m_dwID == pDstCharacter->m_dwID)
    {
        KPlayer*        pPlayer             = (KPlayer*)pSrcCharacter;
        KTeam*          pTeam               = NULL;
        int             nMaxAttractionLevel = 0;
        DWORD           dwTeamID            = pPlayer->GetCurrentTeamID();
        int             nGroupIndex         = pPlayer->GetCurrentTeamGroupIndex();

        pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
        KG_PROCESS_ERROR_RET_CODE(pTeam, 1);

        KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < pTeam->nGroupNum);
        
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
            it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
        )
        {
            KTEAM_MEMBER_INFO*  pMemberInfo          = &(*it);
            KPlayer*            pPlayerMember       = NULL;
            int                 nAttraction         = 0;
            int                 nTmpAttractionLevel = 0;

            assert(pMemberInfo);

            if (pSrcCharacter->m_dwID == pMemberInfo->dwMemberID)
                continue;

            if (pSrcCharacter->m_pScene->m_dwMapID != pMemberInfo->dwMapID)
                continue;

            pPlayerMember = g_pSO3World->m_PlayerSet.GetObj(pMemberInfo->dwMemberID);
            if (!pPlayerMember)
                continue;

            bRetCode = g_InRange(
                static_cast<KSceneObject*>(pSrcCharacter), 
                static_cast<KSceneObject*>(pPlayerMember), 
                nRadius
            );
            if (!bRetCode)
                continue;

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pSrcCharacter->m_dwID, pMemberInfo->dwMemberID);
            if (pFellowship)
                nAttraction += pFellowship->m_nAttraction;

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pMemberInfo->dwMemberID, pSrcCharacter->m_dwID);
            if (pFellowship)
                nAttraction += pFellowship->m_nAttraction;

            nTmpAttractionLevel = KGFellowshipMgr::AttractionValue2Level(nAttraction, 2);

            if (nTmpAttractionLevel > nMaxAttractionLevel)
                nMaxAttractionLevel = nTmpAttractionLevel;
        }

        nAttractionLevel = nMaxAttractionLevel;
    }
    else
    {
        int nAttraction = 0;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pSrcCharacter->m_dwID, pDstCharacter->m_dwID);
        if (pFellowship)
            nAttraction += pFellowship->m_nAttraction;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pDstCharacter->m_dwID, pSrcCharacter->m_dwID);
        if (pFellowship)
            nAttraction += pFellowship->m_nAttraction;

        nAttractionLevel = KGFellowshipMgr::AttractionValue2Level(nAttraction, 2);
    }

    MAKE_IN_RANGE(nAttractionLevel, 1, 7);
    nResult = nAttractionLevel;
Exit0:
    return nResult;
}

BOOL ProcessBuffAccordingToAttraction(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    int                 nBuffLevel      = 0;
    KSkill*             pSkill          = NULL;
    KCharacter*         pSrcCharacter   = NULL;
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    pSrcCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSrcCharacter);

    pSkill = pDstCharacter->m_pSkillAttributeParam->SkillRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pSkill);

    nBuffLevel = _GetBuffLevelAccordingToAttraction(pSrcCharacter, pDstCharacter, pSkill->m_nAreaRadius);
    KGLOG_PROCESS_ERROR(nBuffLevel > 0);

    memset(&BuffRecipeKey, 0, sizeof(BuffRecipeKey));

    BuffRecipeKey.dwID      = nValue1;
    BuffRecipeKey.nLevel    = nBuffLevel;

    bRetCode = pDstCharacter->m_BuffList.CallBuff(pSrcCharacter->m_dwID, pSrcCharacter->m_nLevel, BuffRecipeKey, 0, 0);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_StatDataServer.UpdateFormationStat(nValue1, nBuffLevel);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachSingleBuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachSingleBuff(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDetachMultiGroupBuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->m_BuffList.DetachMultiBuff(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDelSingleBuffByFunctionType(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->m_BuffList.DelSingleBuffByFunctionType(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDelMultiGroupBuffByFunctionType(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->m_BuffList.DelMultiBuffByFunctionType(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDelSingleBuffByIDAndLevel(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->m_BuffList.DelSingleBuff(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDelSingleGroupBuffByIDAndLevel(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->m_BuffList.DelSingleGroupBuff(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(bRetCode);

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

BOOL ProcessSpoof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    DWORD       dwSrcCharacter  = 0;
    KCharacter* pSrcCharacter   = NULL;

    KGLOG_PROCESS_ERROR(bApply);

    dwSrcCharacter = pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID;
    KGLOG_PROCESS_ERROR(dwSrcCharacter && "Attribute Error.");

    bRetCode = IS_PLAYER(dwSrcCharacter);
    if (bRetCode)
    {
        pSrcCharacter = g_pSO3World->m_PlayerSet.GetObj(dwSrcCharacter);
    }
    else
    {
        pSrcCharacter = g_pSO3World->m_NpcSet.GetObj(dwSrcCharacter);
    }
    KGLOG_PROCESS_ERROR(pSrcCharacter);

    bRetCode = IS_PLAYER(pDstCharacter->m_dwID);
    if (bRetCode)
    {
        TARGET_TYPE eTargetType = ttInvalid;
        int nDirection = g_GetDirection(pDstCharacter->m_nX, pDstCharacter->m_nY, pSrcCharacter->m_nX, pSrcCharacter->m_nY);
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        pDstCharacter->TurnTo(nDirection, true);
        if (IS_PLAYER(pSrcCharacter->m_dwID))
        {
            eTargetType = ttPlayer;
        }
        else
        {
            eTargetType = ttNpc;
        }
        pDstCharacter->SelectTarget(eTargetType, pSrcCharacter->m_dwID);
    }
    else
    {
        const KSimpThreatNode*  cpThreatNode     = pDstCharacter->m_SimpThreatList.GetFirstThreat(thtMainThreat);
        const KSimpThreatNode*  cpSrcThreatNode  = pDstCharacter->m_SimpThreatList.FindThreat(pSrcCharacter);
        int                     nThreat          = 0;

        if (cpThreatNode)
            nThreat = cpThreatNode->nThreat[thtMainThreat];

        if (cpSrcThreatNode)
            nThreat -= cpSrcThreatNode->nThreat[thtMainThreat];

        nThreat = max(nThreat, 0);
        pDstCharacter->m_SimpThreatList.ModifyThreat(thtMainThreat, pSrcCharacter, nThreat);
    }
    
    bResult = true;
Exit0:
    return bResult;
}

#endif

BOOL ProcessSkillEventHandler(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
#ifdef _SERVER
    int nAddRef  = 0;
    int nEventID = nValue1;
    KIS_SAME_EVENT_ID   IsSameEventID;
    KCharacter::KUSER_SKILL_EVENT_VECTOR::iterator it;

    nAddRef = bApply ? 1 : -1;

    IsSameEventID.dwEventID = nEventID;
    it = std::find_if(pDstCharacter->m_SkillEventVector.begin(), pDstCharacter->m_SkillEventVector.end(), IsSameEventID);

    if (it != pDstCharacter->m_SkillEventVector.end())
    {
        it->nRefCount += nAddRef;
        if (it->nRefCount <= 0)
        {
            *it = pDstCharacter->m_SkillEventVector.back();
            pDstCharacter->m_SkillEventVector.pop_back();
        }
    }
    else
    {
        if (nAddRef != 1)
        {
            KGLogPrintf(KGLOG_ERR, "[SKILL] Error, try to erase a skill event(%d), but not found in character!", nEventID);
            goto Exit0;
        }

        KUSER_SKILL_EVENT UserSkillEvent;
        KSKILL_EVENT* pEventInfo = g_pSO3World->m_SkillManager.GetEventInfo(nEventID);
        KGLOG_PROCESS_ERROR(pEventInfo);

        UserSkillEvent.dwEventID  = nEventID;
        UserSkillEvent.nRefCount  = 1;
        UserSkillEvent.SkillEvent = *pEventInfo;
        pDstCharacter->m_SkillEventVector.push_back(UserSkillEvent);
    }

Exit0:
#endif
    return true;
}


BOOL ProcessSilence(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(nValue1 > sktInvalid && nValue1 < sktTotal);

    if (bApply)
    {
        KGLOG_PROCESS_ERROR(pDstCharacter->m_nSilenceRefCounter[nValue1 - 1] < CHAR_MAX);
        pDstCharacter->m_nSilenceRefCounter[nValue1 - 1]++;
    }
    else
    {
        KGLOG_PROCESS_ERROR(pDstCharacter->m_nSilenceRefCounter[nValue1 - 1] > 0);
        pDstCharacter->m_nSilenceRefCounter[nValue1 - 1]--;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSilenceAll(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nProcess   = sktInvalid;

    for (nProcess = sktInvalid + 1; nProcess < sktTotal; nProcess++)
    {
        bRetCode = ProcessSilence(pDstCharacter, bApply, nProcess, 0);
        KG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        // If we can not silent(de-silent) all kinds of skills, then we do not silent(de-silent) any.
        for (nProcess--; nProcess > sktInvalid; nProcess--)
        {
            ProcessSilence(pDstCharacter, !bApply, nProcess, 0);
        }
    }

    return bResult;
}

BOOL ProcessImmunity(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nValue1 > sftInvalid && nValue1 < sftTotal);

    if (bApply)
    {
        bRetCode = pDstCharacter->m_ImmunityFlag.test(nValue1 - 1);
        KGLOG_PROCESS_ERROR(!bRetCode);
        pDstCharacter->m_ImmunityFlag.set(nValue1 - 1);
    }
    else
    {
        bRetCode = pDstCharacter->m_ImmunityFlag.test(nValue1 - 1);
        KGLOG_PROCESS_ERROR(bRetCode);
        pDstCharacter->m_ImmunityFlag.reset(nValue1 - 1);
    }

    bResult = true;
Exit0:
    return bResult;
}

#define USE_WEAK_SHIELD(WeakChangeVal, WeakShieldVal)   \
    do                                                  \
    {                                                   \
        if (WeakChangeVal <= 0)                         \
        {                                               \
            break;                                      \
        }                                               \
                                                        \
        if (WeakChangeVal > WeakShieldVal)              \
        {                                               \
            WeakChangeVal -= WeakShieldVal;             \
        }                                               \
        else                                            \
        {                                               \
            WeakChangeVal = 0;                          \
        }                                               \
    } while(false)

BOOL ProcessAddWeak(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(nValue1 > swpInvalid && nValue1 < swpTotal && nValue1 != swpNone);

    KGLOG_PROCESS_ERROR(bApply);

    switch(nValue1)
    {
    case swpUpper:
        USE_WEAK_SHIELD(nValue2, pDstCharacter->m_nUpperWeakShield);

        pDstCharacter->m_nUpperWeak += nValue2;
        MAKE_IN_RANGE(pDstCharacter->m_nUpperWeak,  0, WEAK_MAX);
        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nUpperWeakLevel, pDstCharacter->m_nUpperWeak);
        break;

    case swpMiddle:
        USE_WEAK_SHIELD(nValue2, pDstCharacter->m_nMiddleWeakShield);

        pDstCharacter->m_nMiddleWeak += nValue2;
        MAKE_IN_RANGE(pDstCharacter->m_nMiddleWeak, 0, WEAK_MAX);
        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nMiddleWeakLevel, pDstCharacter->m_nMiddleWeak);
        break;

    case swpLower:
        USE_WEAK_SHIELD(nValue2, pDstCharacter->m_nLowerWeakShield);

        pDstCharacter->m_nLowerWeak += nValue2;
        MAKE_IN_RANGE(pDstCharacter->m_nLowerWeak,  0, WEAK_MAX);
        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nLowerWeakLevel, pDstCharacter->m_nLowerWeak);
        break;

    default:
        assert(false);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessShiftWeak(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;

#ifdef _SERVER
    int nMaxWeakPos     = 0;
    int nMaxWeakValue   = 0;
    int nTempWeak       = 0;

    KGLOG_PROCESS_ERROR(bApply);

    if (pDstCharacter->m_nUpperWeak > nMaxWeakValue)
    {
        nMaxWeakValue = pDstCharacter->m_nUpperWeak;
        nMaxWeakPos = 1;
    }
    if (pDstCharacter->m_nMiddleWeak > nMaxWeakValue)
    {
        nMaxWeakValue = pDstCharacter->m_nMiddleWeak;
        nMaxWeakPos = 2;
    }
    if (pDstCharacter->m_nLowerWeak > nMaxWeakValue)
    {
        nMaxWeakValue = pDstCharacter->m_nLowerWeak;
        nMaxWeakPos = 3;
    }

    switch(nMaxWeakPos)
    {
    case 1:
        nTempWeak = pDstCharacter->m_nUpperWeak;
        pDstCharacter->m_nUpperWeak = pDstCharacter->m_nMiddleWeak;
        pDstCharacter->m_nMiddleWeak = nTempWeak;

        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nUpperWeakLevel, pDstCharacter->m_nUpperWeak);
        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nMiddleWeakLevel, pDstCharacter->m_nMiddleWeak);
        break;
    case 2:
        nTempWeak = pDstCharacter->m_nMiddleWeak;
        pDstCharacter->m_nMiddleWeak = pDstCharacter->m_nLowerWeak;
        pDstCharacter->m_nLowerWeak = nTempWeak;

        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nMiddleWeakLevel, pDstCharacter->m_nMiddleWeak);
        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nLowerWeakLevel, pDstCharacter->m_nLowerWeak);
        break;
    case 3:
        nTempWeak = pDstCharacter->m_nLowerWeak;
        pDstCharacter->m_nLowerWeak = pDstCharacter->m_nUpperWeak;
        pDstCharacter->m_nUpperWeak = nTempWeak;

        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nLowerWeakLevel, pDstCharacter->m_nLowerWeak);
        WEAK_VALUE_TO_LEVEL(pDstCharacter->m_nUpperWeakLevel, pDstCharacter->m_nUpperWeak);
        break;
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAddWeakShield(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;

    KGLOG_PROCESS_ERROR(nValue1 > swpInvalid && nValue1 < swpTotal && nValue1 != swpNone);

    if (!bApply)
    {
        nValue2 = -nValue2;
    }

    switch(nValue1)
    {
    case swpUpper:
        pDstCharacter->m_nUpperWeakShield  += nValue2;
        if (pDstCharacter->m_nUpperWeakShield < 0)
        {
            pDstCharacter->m_nUpperWeakShield = 0;
        }
        break;
    case swpMiddle:
        pDstCharacter->m_nMiddleWeakShield += nValue2;
        if (pDstCharacter->m_nMiddleWeakShield < 0)
        {
            pDstCharacter->m_nMiddleWeakShield = 0;
        }
        break;
    case swpLower:
        pDstCharacter->m_nLowerWeakShield  += nValue2;
        if (pDstCharacter->m_nLowerWeakShield < 0)
        {
            pDstCharacter->m_nLowerWeakShield = 0;
        }
        break;
    default:
        assert(false);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessModifySkillCastFlag(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult    = false;
    BOOL bFlag      = false;

    KGLOG_PROCESS_ERROR(nValue1 >= 0 && nValue1 < sizeof(DWORD) * CHAR_BIT);

    if (bApply)
    {
        bFlag = nValue2;
    }
    else
    {
        bFlag = !nValue2;
    }

    if (bFlag)
    {
        pDstCharacter->m_dwSkillCastFlag |= 1 << nValue1;
    }
    else
    {
        pDstCharacter->m_dwSkillCastFlag &= ~(1 << nValue1);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessExecuteScript(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    DWORD           dwScriptID      = nValue1;
    int             nLuaTopIndex    = 0;
    Lua_State*      L               = NULL;
    
#ifdef _SERVER
    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

	if (bApply)
	{  		
		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

		g_pSO3World->m_ScriptCenter.PushValueToStack(pDstCharacter->m_dwID);

        g_pSO3World->m_ScriptCenter.CallFunction(dwScriptID, "Apply", 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
    }
    else
    {		
		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);
	
		g_pSO3World->m_ScriptCenter.PushValueToStack(pDstCharacter->m_dwID);

        g_pSO3World->m_ScriptCenter.CallFunction(dwScriptID, "UnApply", 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPrinkRepresentID(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    KPlayer*        pPlayer         = NULL;
    DWORD           dwEquipPos      = 0;
    KItem*          pItem           = 0;
    int             nOldRepresentID = ERROR_ID;
    
#ifdef _SERVER
    KGLOG_PROCESS_ERROR(nValue1 >= 0 && nValue1 < perRepresentCount);
    
    pPlayer = (KPlayer*)pDstCharacter;
    
    if (bApply)
    {
        pPlayer->m_wRepresentId[nValue1] = (WORD)nValue2;

        g_PlayerServer.DoSyncEquipRepresent(pPlayer, nValue1, nValue2);

        pPlayer->m_dwRepresentIdLock |= (0x1 << nValue1);
    }
    else
    {
        pPlayer->m_dwRepresentIdLock &= ~(0x1 << nValue1);
        
        bRetCode = pPlayer->m_ItemList.GetEquipPos(nValue1, &dwEquipPos);
        KG_PROCESS_SUCCESS(!bRetCode);
        
        pItem = pPlayer->m_ItemList.GetItem(ibEquip, dwEquipPos);
        
        if (pItem)
        {
            nOldRepresentID = pItem->m_nRepresentID;
        }

        pPlayer->m_wRepresentId[nValue1] = (WORD)nOldRepresentID;

        g_PlayerServer.DoSyncEquipRepresent(pPlayer, nValue1, nOldRepresentID);
    }

#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessPrinkModel(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        if (bApply)
        {
            pDstCharacter->m_dwModelID = (DWORD)nValue1;
        }
        else
        {
            pDstCharacter->m_dwModelID = 0;        
        }        
    }
    else
    {
        KNpc* pNpc = (KNpc*)pDstCharacter;
        
        if (bApply)
        {
            pNpc->m_dwModelID = (DWORD)nValue1;
        }
        else
        {
            pNpc->m_dwModelID = pNpc->m_pTemplate->dwRepresentID[0];        
        }
    }   

#ifdef _SERVER
    g_PlayerServer.DoCharacterChangeModel(pDstCharacter);
#endif

    return true;
}

BOOL ProcessPrinkForce(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        pPlayer = (KPlayer*)pDstCharacter;

        if (bApply)
        {
            bRetCode = pPlayer->m_ReputeList.SetPrinkForce(nValue1, nValue2);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
            bRetCode = pPlayer->m_ReputeList.SetPrinkForce(MAX_FORCE_COUNT, 0);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessRideHorse(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        pPlayer = (KPlayer*)pDstCharacter;

        if (bApply)
        {
            bRetCode = pPlayer->RideHorse();
            KG_PROCESS_ERROR(bRetCode);
        }
        else
        {
            bRetCode = pPlayer->DownHorse();
            KG_PROCESS_ERROR(bRetCode);
        }
    }
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDoAction(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    
    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = pDstCharacter->Action(0, nValue1);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAddExpPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

#ifdef _SERVER
    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        pPlayer = (KPlayer*)pDstCharacter;
        
        pPlayer->m_nExpPercent += (bApply ? nValue1 : -nValue1);
    }
#endif
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAddReputationPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

#ifdef _SERVER
    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        pPlayer = (KPlayer*)pDstCharacter;

        pPlayer->m_nReputationPercent += (bApply ? nValue1 : -nValue1);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessEmployerCastSkillTargetSelf(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nX              = 0;
    int         nY              = 0;
    int         nZ              = 0;
    KCharacter* pSrcCharacter   = NULL;
    KNpc*       pDstNpc         = NULL;
    KSkill*     pSkill          = NULL;
    KTarget     Target;

#ifdef _SERVER
    bRetCode = IS_NPC(pDstCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pDstNpc = (KNpc*)pDstCharacter;

    KGLOG_PROCESS_ERROR(pDstNpc->m_dwEmployer != ERROR_ID);

    bRetCode = IS_PLAYER(pDstNpc->m_dwEmployer);

    if (bRetCode)
    {
        pSrcCharacter = g_pSO3World->m_PlayerSet.GetObj(pDstNpc->m_dwEmployer);
    }
    else
    {
        pSrcCharacter = g_pSO3World->m_NpcSet.GetObj(pDstNpc->m_dwEmployer);
    }
    KG_PROCESS_ERROR(pSrcCharacter); // 找不到雇主了，可能是下线了

    KG_PROCESS_ERROR(pSrcCharacter->m_pScene);
    KGLOG_PROCESS_ERROR(pDstNpc->m_pScene);
    KGLOG_PROCESS_ERROR(pDstNpc->m_pScene == pSrcCharacter->m_pScene);

    bRetCode = pDstNpc->GetAbsoluteCoordinate(&nX, &nY, &nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    Target.SetTarget(nX, nY, nZ);

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(nValue1, nValue2);
    KGLOG_PROCESS_ERROR(pSkill);

    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode == scmPointArea);

    bRetCode = pSrcCharacter->CastSkillSub(pSrcCharacter, (DWORD)nValue1, (DWORD)nValue2, Target);
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessClearCoolDown(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

    KGLOG_PROCESS_ERROR(bApply);

    bRetCode = IS_PLAYER(pDstCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_TimerList.ClearTimer(nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessResetCoolDown(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;

    KGLOG_PROCESS_ERROR(bApply);

#ifdef _SERVER
    bRetCode = IS_PLAYER(pDstCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer = (KPlayer*)pDstCharacter;

    if (!nValue2)
    {
        nValue2 = pPlayer->GetCDValue(nValue1);
        KGLOG_PROCESS_ERROR(nValue2 > 0);
    }

    pPlayer->m_TimerList.ResetTimer(nValue1, nValue2);
#endif // _SERVER

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTransferDamageToManaPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;

    pDstCharacter->m_nTransferDamageToManaPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessModifyCostManaPercent(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL    bResult = false;

    pDstCharacter->m_nModifyCostManaPercent += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessDirectCastMask(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        pDstCharacter->m_dwDirectCastMask |= nValue1;
    }
    else
    {
        pDstCharacter->m_dwDirectCastMask &= ~nValue1;
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL ProcessPull(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    KCharacter* pSkillSrc   = NULL;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    pSkillSrc = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    pDstCharacter->ProcessPull(pSkillSrc, nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTurnToSrc(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    KCharacter* pSrcCharacter   = NULL;
    int         nTurnDirection  = 0;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    if (IS_PLAYER(pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID))
    {
        pSrcCharacter = g_pSO3World->m_PlayerSet.GetObj(pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID);
    }
    else
    {
        pSrcCharacter = g_pSO3World->m_NpcSet.GetObj(pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID);
    }
    KGLOG_PROCESS_ERROR(pSrcCharacter);

    nTurnDirection = g_GetDirection(pDstCharacter->m_nX, pDstCharacter->m_nY, pSrcCharacter->m_nX, pSrcCharacter->m_nY);

    pDstCharacter->TurnTo(nTurnDirection, true);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessConsumeBuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc);

    pDstCharacter->m_BuffList.ConsumeBuff(pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID, (DWORD)nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTopThreat(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(IS_NPC(pDstCharacter->m_dwID));

    if (bApply)
    {
        KCharacter* pSrcCharacter = NULL;

        KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

        pSrcCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
        KGLOG_PROCESS_ERROR(pSrcCharacter);

        pDstCharacter->m_SimpThreatList.StickThreat(pSrcCharacter);
    }
    else
    {
        pDstCharacter->m_SimpThreatList.UnstickThreat();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessZeroThreat(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL                    bResult         = false;
    KCharacter*             pSrcCharacter   = NULL;

    KG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    pSrcCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSrcCharacter);

    if (pSrcCharacter == pDstCharacter)
        pSrcCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillDst;

    pDstCharacter->m_SimpThreatList.ZeroThreat(pSrcCharacter);

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL ProcessFormationEffect(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

#ifdef _CLIENT
    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        if (bApply)
        {
            pPlayer->m_dwFormationEffectID      = (DWORD)nValue1;
            pPlayer->m_nFormationEffectLevel    = nValue2;
        }
        else
        {
            pPlayer->m_dwFormationEffectID      = 0;
            pPlayer->m_nFormationEffectLevel    = 0;
        }
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAddDamageByDstMoveState(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        pDstCharacter->m_dwAddDamageByDstMoveStateMask      = nValue1;
        pDstCharacter->m_nAddDamagePercentByDstMoveState    = nValue2;
    }
    else
    {
        pDstCharacter->m_dwAddDamageByDstMoveStateMask      = 0;
        pDstCharacter->m_nAddDamagePercentByDstMoveState    = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTransferDamage(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

        pDstCharacter->m_dwTransferDamageToCharacter = pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID;
        pDstCharacter->m_nTransferDamagePercent      = nValue1;
    }
    else
    {
        pDstCharacter->m_dwTransferDamageToCharacter = ERROR_ID;
        pDstCharacter->m_nTransferDamagePercent      = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTransferTherapy(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    if (bApply)
    {
        KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

        pDstCharacter->m_dwTransferTherapyToCharacter = pDstCharacter->m_pSkillAttributeParam->dwSkillSrcID;
        pDstCharacter->m_nTransferTherapyPercent      = nValue1;
    }
    else
    {
        pDstCharacter->m_dwTransferTherapyToCharacter = ERROR_ID;
        pDstCharacter->m_nTransferTherapyPercent      = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAddFellowshipAttraction(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    KG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc);
    KG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillDst);

    g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(
        pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_dwID,
        pDstCharacter->m_pSkillAttributeParam->pSkillDst->m_dwID,
        nValue1
    );
#endif
        
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallHelp(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult     = false;
    BOOL bRetCode    = false;

#ifdef _SERVER
    const KSimpThreatNode* cpThreatNode = NULL;

    KGLOG_PROCESS_ERROR(bApply);

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc);

    KG_PROCESS_ERROR(pDstCharacter->m_dwForceID == pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_dwForceID);

    cpThreatNode = pDstCharacter->m_SimpThreatList.GetFirstThreat(thtMainThreat);

    KG_PROCESS_SUCCESS(cpThreatNode);

    cpThreatNode = pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    while (cpThreatNode)
    {
        bRetCode = pDstCharacter->m_SimpThreatList.ModifyThreat(thtSpecialThreat, cpThreatNode->pCharacter, 0);
        KG_PROCESS_ERROR(bRetCode);

        cpThreatNode = pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
    }
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCallRequest(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc);

    pDstCharacter->m_AIData.dwRespondCharacterID = pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_dwID;

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessCatchCharacter(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(bApply);

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_pScene);
    KGLOG_PROCESS_ERROR(IS_NPC(pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_dwID));

    pDstCharacter->MoveTo(
        pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_nX, pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_nY,
        pDstCharacter->m_pSkillAttributeParam->pSkillSrc->m_nZ, false, true
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessTransmission(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    DWORD       dwMapID     = 0;
    int         nX          = 0;
    int         nY          = 0;
    int         nZ          = 0;
    KPlayer*    pPlayer     = NULL;

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    bRetCode = g_pSO3World->m_TransmissionList.GetTranslatePosition(nValue1, &dwMapID, &nX, &nY, &nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->SwitchMap(dwMapID, 0, nX, nY, nZ); 
#endif

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL ProcessStealth(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    pDstCharacter->Stealth(bApply);

    return true;
}
#endif

BOOL ProcessVitalityToParryValueCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToParryValueCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllBattleState();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToMaxLifeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToMaxLifeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateBaseState();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToPhysicsAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToPhysicsAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToSolarAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToSolarAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToLunarAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToLunarAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToNeutralAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToNeutralAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToPoisonAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToPoisonAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToTherapyPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToTherapyPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessVitalityToPhysicsShieldCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nVitalityToPhysicsShieldCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToMaxManaCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToMaxManaCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateBaseState();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToSolarCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToSolarCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToLunarCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToLunarCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToNeutralCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToNeutralCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToPoisonCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToPoisonCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToPhysicsAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToPhysicsAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToPhysicsCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToPhysicsCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToManaReplenishCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToManaReplenishCof += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToSolarMagicShieldCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToSolarMagicShieldCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToLunarMagicShieldCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToLunarMagicShieldCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToNeutralMagicShieldCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToNeutralMagicShieldCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpiritToPoisonMagicShieldCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpiritToPoisonMagicShieldCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllShieldValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkToManaReplenishCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpunkToManaReplenishCof += (bApply ? nValue1 : -nValue1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkToSolarAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpunkToSolarAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkToLunarAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpunkToLunarAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkToNeutralAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpunkToNeutralAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessSpunkToPoisonAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nSpunkToPoisonAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessStrengthToPhysicsCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nStrengthToPhysicsCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessStrengthToPhysicsAttackPowerCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nStrengthToPhysicsAttackPowerCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllAttackPowerValue();

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessAgilityToPhysicsCriticalStrikeCof(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    pPlayer->m_nAgilityToPhysicsCriticalStrikeCof += (bApply ? nValue1 : -nValue1);

    pPlayer->UpdateAllCriticalStrikeValue();

    bResult = true;
Exit0:
    return bResult;
}

#define MAX_FOUND_COUNT 64
struct KFindTarget
{
    KFindTarget()
    {
        m_nFoundCout    = 0;
        m_pCharacter    = NULL;
        m_nRelationMask = 0;
        m_nRange        = 0;
    }

    BOOL operator () (KCharacter* pCharacter)
    {
        int nRelation = 0;
        int nDistance = 0;

        assert(m_pCharacter);
        assert(pCharacter);

        if (m_nFoundCout >= MAX_FOUND_COUNT)
            return false;

        nDistance = g_GetDistance3(
            pCharacter->m_nX, pCharacter->m_nY, ZPOINT_TO_XYPOINT(pCharacter->m_nZ),
            m_pCharacter->m_nX, m_pCharacter->m_nY, ZPOINT_TO_XYPOINT(m_pCharacter->m_nZ)
        );
        if (nDistance > m_nRange)
            return true;

        nRelation = g_GetRelation(m_pCharacter, pCharacter);
        if (nRelation & m_nRelationMask)
        {
            m_dwFoundID[m_nFoundCout] = pCharacter->m_dwID;
            m_nFoundCout++;
        }

        return true;
    }

    int         m_nFoundCout;
    int         m_nRelationMask;
    int         m_nRange;
    KCharacter* m_pCharacter;
    DWORD       m_dwFoundID[MAX_FOUND_COUNT];
};

BOOL ProcessAutoSelectTarget(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult         = false;
    KSkill*     pSkill          = NULL;
    int         nSelectIndex    = 0;
    KFindTarget FindTarget;

    KGLOG_PROCESS_ERROR(pDstCharacter->m_pScene);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pRegion);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    pSkill = pDstCharacter->m_pSkillAttributeParam->SkillRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pSkill);

    FindTarget.m_nRelationMask = pSkill->m_pBaseInfo->nTargetRelationMask;
    FindTarget.m_pCharacter    = pDstCharacter;
    FindTarget.m_nRange        = pSkill->m_nAreaRadius;

    pDstCharacter->m_pScene->TraverseRangePlayer(pDstCharacter->m_pRegion, FindTarget);
    pDstCharacter->m_pScene->TraverseRangeNpc(pDstCharacter->m_pRegion, FindTarget);

    KG_PROCESS_ERROR(FindTarget.m_nFoundCout > 0);

    nSelectIndex = g_Random(FindTarget.m_nFoundCout);

    assert(nSelectIndex < FindTarget.m_nFoundCout);
    if (IS_PLAYER(FindTarget.m_dwFoundID[nSelectIndex]))
    {
        pDstCharacter->SelectTarget(ttPlayer, FindTarget.m_dwFoundID[nSelectIndex]);
    }
    else
    {
        pDstCharacter->SelectTarget(ttNpc, FindTarget.m_dwFoundID[nSelectIndex]);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMentorBuff(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KCharacter*         pSrcCharacter   = NULL;

#ifdef _SERVER
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    KGLOG_PROCESS_ERROR(bApply);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pSkillAttributeParam);

    pSrcCharacter = pDstCharacter->m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSrcCharacter);

    bRetCode = g_pSO3World->m_MentorCache.IsMentorship(pSrcCharacter->m_dwID, pDstCharacter->m_dwID);
    KG_PROCESS_ERROR(bRetCode);

    memset(&BuffRecipeKey, 0, sizeof(BuffRecipeKey));

    BuffRecipeKey.dwID      = (DWORD)nValue1;
    BuffRecipeKey.nLevel    = nValue2;

    bRetCode = pDstCharacter->m_BuffList.CallBuff(pSrcCharacter->m_dwID, pSrcCharacter->m_nLevel, BuffRecipeKey, 0, 0);
    KGLOG_PROCESS_ERROR(bRetCode);
#endif // _SERVER

    bResult = true;
Exit0:
    return bResult;
}

BOOL ProcessMentorFormationEffect(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL bResult = false;

#ifdef _CLIENT
    if (IS_PLAYER(pDstCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pDstCharacter;

        if (bApply)
        {
            pPlayer->m_dwMentorFormationEffectID      = (DWORD)nValue1;
            pPlayer->m_nMentorFormationEffectLevel    = nValue2;
        }
        else
        {
            pPlayer->m_dwMentorFormationEffectID      = 0;
            pPlayer->m_nMentorFormationEffectLevel    = 0;
        }
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}
#undef MAX_FOUND_COUNT

BOOL ProcessAdjustProfessionLevel(KCharacter* pDstCharacter, BOOL bApply, int nValue1, int nValue2)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    KPlayer*    pPlayer             = NULL;
    int         nProfessionLevel    = 0;
    int         nAdjustLevel        = 0;

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(IS_PLAYER(pDstCharacter->m_dwID));

    pPlayer = (KPlayer*)pDstCharacter;

    nAdjustLevel     = pPlayer->m_ProfessionList.GetProfessionAdjustLevel(nValue1);

    nAdjustLevel    += (bApply ? nValue2 : -nValue2);
    
    bRetCode = pPlayer->m_ProfessionList.SetAdjustLevel(nValue1, nAdjustLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncProfessionAdjustLevel(pPlayer->m_nConnIndex, nValue1, nAdjustLevel);

#endif
    bResult = true;
Exit0:
    return bResult;
}

// --------------------------------------------------------------------------------

ATTR_PROCESS_FUN g_AttributeProcessFunctions[atTotal] = { NULL };

BOOL InitAttributeFunctions()
{
	memset(g_AttributeProcessFunctions, 0, sizeof(g_AttributeProcessFunctions));

    // ------------- 移动相关 ------------------------------------------>

    g_AttributeProcessFunctions[atRunSpeedBase]                 = ProcessRunSpeedBase;
    g_AttributeProcessFunctions[atMoveSpeedPercent]             = ProcessMoveSpeedPercent;
    g_AttributeProcessFunctions[atMaxJumpCount]                 = ProcessMaxJumpCount;
    g_AttributeProcessFunctions[atGravityBase]                  = ProcessGravityBase;
    g_AttributeProcessFunctions[atGravityPercent]               = ProcessGravityPercent;
    g_AttributeProcessFunctions[atJumpSpeedBase]                = ProcessJumpSpeedBase;
    g_AttributeProcessFunctions[atJumpSpeedPercent]             = ProcessJumpSpeedPercent;
    g_AttributeProcessFunctions[atDropDefence]                  = ProcessDropDefence;
    g_AttributeProcessFunctions[atDivingFrameBase]              = ProcessDivingFrameBase;
    g_AttributeProcessFunctions[atDivingFramePercent]           = ProcessDivingFramePercent;
    g_AttributeProcessFunctions[atWaterFlyAbility]              = ProcessWaterFlyAbility;

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
    g_AttributeProcessFunctions[atTransmitTrain]                = ProcessTransmitTrain;
    g_AttributeProcessFunctions[atMaxTrainValue]                = ProcessMaxTrainValue;
    g_AttributeProcessFunctions[atBaseVenationCof]              = ProcessBaseVenationCof;
    g_AttributeProcessFunctions[atPhysicsVenationCof]           = ProcessPhysicsVenationCof;
    g_AttributeProcessFunctions[atMagicVenationCof]             = ProcessMagicVenationCof;
    g_AttributeProcessFunctions[atAssistVenationCof]            = ProcessAssistVenationCof;
    g_AttributeProcessFunctions[atBasePotentialAdd]             = ProcessBasePotentialAdd;

    // ------------- 生命 ---------------------------------------------->
	g_AttributeProcessFunctions[atCurrentLife]                  = ProcessCurrentLife;
    g_AttributeProcessFunctions[atMaxLifeBase]                  = ProcessMaxLifeBase;
	g_AttributeProcessFunctions[atMaxLifePercentAdd]            = ProcessMaxLifePercentAdd;
    g_AttributeProcessFunctions[atMaxLifeAdditional]            = ProcessMaxLifeAdditional;
    g_AttributeProcessFunctions[atLifeReplenish]                = ProcessLifeReplenish;
    g_AttributeProcessFunctions[atLifeReplenishPercent]         = ProcessLifeReplenishPercent;
    g_AttributeProcessFunctions[atLifeReplenishCoefficient]     = ProcessLifeReplenishCoefficient;
    g_AttributeProcessFunctions[atLifeReplenishExt]             = ProcessLifeReplenishExt;

    // ------------- 内力 ---------------------------------------------->
	g_AttributeProcessFunctions[atCurrentMana]                  = ProcessCurrentMana;
    g_AttributeProcessFunctions[atMaxManaBase]                  = ProcessMaxManaBase;
	g_AttributeProcessFunctions[atMaxManaPercentAdd]            = ProcessMaxManaPercentAdd;
    g_AttributeProcessFunctions[atMaxManaAdditional]            = ProcessMaxManaAdditional;
    g_AttributeProcessFunctions[atManaReplenish]                = ProcessManaReplenish;
    g_AttributeProcessFunctions[atManaReplenishPercent]         = ProcessManaReplenishPercent;
    g_AttributeProcessFunctions[atManaReplenishCoefficient]     = ProcessManaReplenishCoefficient;
    g_AttributeProcessFunctions[atManaReplenishExt]             = ProcessManaReplenishExt;

    // ------------- 怒气 -------------------------------------------------->
    g_AttributeProcessFunctions[atCurrentRage]                  = ProcessCurrentRage;
    g_AttributeProcessFunctions[atMaxRage]                      = ProcessMaxRage;
    g_AttributeProcessFunctions[atRageReplenish]                = ProcessRageReplenish;

    g_AttributeProcessFunctions[atCurrentStamina]               = ProcessCurrentStamina;
    g_AttributeProcessFunctions[atCurrentThew]                  = ProcessCurrentThew;

#ifdef _SERVER
    // ------------- 仇恨 -------------------------------------------------->
    g_AttributeProcessFunctions[atActiveThreatCoefficient]      = ProcessActiveThreatCoefficient;
    g_AttributeProcessFunctions[atPassiveThreatCoefficient]     = ProcessPassiveThreatCoefficient;
#endif

    // ------------- 战斗杂项 ---------------------------------------------->
    g_AttributeProcessFunctions[atDodge]                        = ProcessDodge;
    g_AttributeProcessFunctions[atDodgeBaseRate]                = ProcessDodgeBaseRate;
    g_AttributeProcessFunctions[atParryBaseRate]                = ProcessParryBaseRate;
    g_AttributeProcessFunctions[atParryBase]                    = ProcessParryBase;
    g_AttributeProcessFunctions[atParryPercent]                 = ProcessParryPercent;
    g_AttributeProcessFunctions[atParryValueBase]               = ProcessParryValueBase;
    g_AttributeProcessFunctions[atParryValuePercent]            = ProcessParryValuePercent;
    g_AttributeProcessFunctions[atSense]                        = ProcessSense;
    g_AttributeProcessFunctions[atStrainBase]                   = ProcessStrainBase;
    g_AttributeProcessFunctions[atToughnessBaseRate]            = ProcessToughnessBaseRate;
    g_AttributeProcessFunctions[atStrainPercent]                = ProcessStrainPercent;
    g_AttributeProcessFunctions[atToughnessBase]                = ProcessToughnessBase;
    g_AttributeProcessFunctions[atToughnessPercent]             = ProcessToughnessPercent;
    g_AttributeProcessFunctions[atDecriticalDamagePowerBaseKiloNumRate]     = ProcessDecriticalDamagePowerBaseKiloNumRate;
    g_AttributeProcessFunctions[atDecriticalDamagePowerBase]    = ProcessDecriticalDamagePowerBase;
    g_AttributeProcessFunctions[atDecriticalDamagePowerPercent] = ProcessDecriticalDamagePowerPercent;
    g_AttributeProcessFunctions[atPositiveShield]               = ProcessPositiveShield;
    g_AttributeProcessFunctions[atNegativeShield]               = ProcessNegativeShield;
    g_AttributeProcessFunctions[atGlobalResistPercent]          = ProcessGlobalResistPercent;
    g_AttributeProcessFunctions[atGlobalBlock]                  = ProcessGlobalBlock;
    g_AttributeProcessFunctions[atGlobalDamageAbsorb]           = ProcessGlobalDamageAbsorb;
    g_AttributeProcessFunctions[atGlobalDamageManaShield]       = ProcessGlobalDamageManaShield;
    g_AttributeProcessFunctions[atDamageToLifeForSelf]          = ProcessDamageToLifeForSelf;
    g_AttributeProcessFunctions[atDamageToManaForSelf]          = ProcessDamageToManaForSelf;
    g_AttributeProcessFunctions[atBeatBackRate]                 = ProcessBeatBackRate;
    g_AttributeProcessFunctions[atKnockedBackRate]              = ProcessKnockedBackRate;
    g_AttributeProcessFunctions[atKnockedOffRate]               = ProcessKnockedOffRate;
    g_AttributeProcessFunctions[atKnockedDownRate]              = ProcessKnockedDownRate;
    g_AttributeProcessFunctions[atPullRate]                     = ProcessPullRate;
    g_AttributeProcessFunctions[atRepulsedRate]                 = ProcessRepulsedRate;

    g_AttributeProcessFunctions[atImmortal]                     = ProcessImmortal;
    g_AttributeProcessFunctions[atMountingRate]                 = ProcessMountingRate;
    
    g_AttributeProcessFunctions[atMeleeWeaponDamageBase]        = ProcessMeleeWeaponDamageBase;
    g_AttributeProcessFunctions[atMeleeWeaponDamageRand]        = ProcessMeleeWeaponDamageRand;

    g_AttributeProcessFunctions[atRangeWeaponDamageBase]        = ProcessRangeWeaponDamageBase;
    g_AttributeProcessFunctions[atRangeWeaponDamageRand]        = ProcessRangeWeaponDamageRand;

    // ------------- 外功 ----------------------------------------------------------

    g_AttributeProcessFunctions[atSkillPhysicsDamage]           = ProcessSkillPhysicsDamage;
    g_AttributeProcessFunctions[atSkillPhysicsDamageRand]       = ProcessSkillPhysicsDamageRand;
    g_AttributeProcessFunctions[atSkillPhysicsDamagePercent]    = ProcessSkillPhysicsDamagePercent;
    g_AttributeProcessFunctions[atPhysicsDamageCoefficient]     = ProcessPhysicsDamageCoefficient;

    g_AttributeProcessFunctions[atPhysicsAttackPowerBase]       = ProcessPhysicsAttackPowerBase;
    g_AttributeProcessFunctions[atPhysicsAttackPowerPercent]    = ProcessPhysicsAttackPowerPercent;

    g_AttributeProcessFunctions[atPhysicsHitBaseRate]           = ProcessPhysicsHitBaseRate;
    g_AttributeProcessFunctions[atPhysicsHitValue]              = ProcessPhysicsHitValue;
    g_AttributeProcessFunctions[atPhysicsCriticalStrikeBaseRate] = ProcessPhysicsCriticalStrikeBaseRate;
    g_AttributeProcessFunctions[atPhysicsCriticalStrike]        = ProcessPhysicsCriticalStrike;

    g_AttributeProcessFunctions[atPhysicsCriticalDamagePowerBaseKiloNumRate]    = ProcessPhysicsCriticalDamagePowerBaseKlioNumRate;
    g_AttributeProcessFunctions[atPhysicsCriticalDamagePowerBase]               = ProcessPhysicsCriticalDamagePowerBase;
    g_AttributeProcessFunctions[atPhysicsCriticalDamagePowerPercent]            = ProcessPhysicsCriticalDamagePowerPercent;

    g_AttributeProcessFunctions[atPhysicsOvercomeBase]          = ProcessPhysicsOvercomeBase;
    g_AttributeProcessFunctions[atPhysicsOvercomePercent]       = ProcessPhysicsOvercomePercent;

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
    
    // ----------- 作用所有内功 -------------------------------------------->
    g_AttributeProcessFunctions[atSkillMagicDamage]             = ProcessSkillMagicDamage;
    g_AttributeProcessFunctions[atMagicAttackPowerBase]         = ProcessMagicAttackPowerBase;
    g_AttributeProcessFunctions[atMagicAttackPowerPercent]      = ProcessMagicAttackPowerPercent;
    g_AttributeProcessFunctions[atMagicCriticalStrike]          = ProcessMagicCriticalStrike;
    g_AttributeProcessFunctions[atMagicShield]                  = ProcessMagicShield;
    g_AttributeProcessFunctions[atMagicCriticalDamagePowerBaseKiloNumRate]  = ProcessMagicCriticalDamagePowerBaseKiloNumRate;
    g_AttributeProcessFunctions[atMagicCriticalDamagePowerBase]             = ProcessMagicCriticalDamagePowerBase;
    g_AttributeProcessFunctions[atMagicCriticalDamagePowerPercent]          = ProcessMagicCriticalDamagePowerPercent;
    g_AttributeProcessFunctions[atMagicOvercome]                = ProcessMagicOvercome;
    g_AttributeProcessFunctions[atMagicHitBaseRate]             = ProcessMagicHitBaseRate;
    g_AttributeProcessFunctions[atMagicHitValue]                = ProcessMagicHitValue;


    // ------------- 阳性内功 ---------------------------------------------->
    g_AttributeProcessFunctions[atSkillSolarDamage]             = ProcessSkillSolarDamage;      
    g_AttributeProcessFunctions[atSkillSolarDamageRand]         = ProcessSkillSolarDamageRand;       
    g_AttributeProcessFunctions[atSkillSolarDamagePercent]      = ProcessSkillSolarDamagePercent;
    g_AttributeProcessFunctions[atSolarDamageCoefficient]       = ProcessSolarDamageCoefficient; 

    g_AttributeProcessFunctions[atSolarAttackPowerBase]         = ProcessSolarAttackPowerBase;
    g_AttributeProcessFunctions[atSolarAttackPowerPercent]      = ProcessSolarAttackPowerPercent;

    g_AttributeProcessFunctions[atSolarHitBaseRate]             = ProcessSolarHitBaseRate;
    g_AttributeProcessFunctions[atSolarHitValue]                = ProcessSolarHitValue;
    g_AttributeProcessFunctions[atSolarCriticalStrikeBaseRate]  = ProcessSolarCriticalStrikeBaseRate;
    g_AttributeProcessFunctions[atSolarCriticalStrike]          = ProcessSolarCriticalStrike;

    g_AttributeProcessFunctions[atSolarCriticalDamagePowerBaseKiloNumRate]    = ProcessSolarCriticalDamagePowerBaseKlioNumRate;
    g_AttributeProcessFunctions[atSolarCriticalDamagePowerBase]               = ProcessSolarCriticalDamagePowerBase;
    g_AttributeProcessFunctions[atSolarCriticalDamagePowerPercent]            = ProcessSolarCriticalDamagePowerPercent;

    g_AttributeProcessFunctions[atSolarOvercomeBase]            = ProcessSolarOvercomeBase;
    g_AttributeProcessFunctions[atSolarOvercomePercent]         = ProcessSolarOvercomePercent;
    g_AttributeProcessFunctions[atSolarMagicResistPercent]      = ProcessSolarMagicResistPercent;
    g_AttributeProcessFunctions[atSolarMagicShieldBase]         = ProcessSolarMagicShieldBase;
    g_AttributeProcessFunctions[atSolarMagicShieldPercent]      = ProcessSolarMagicShieldPercent;
    g_AttributeProcessFunctions[atSolarMagicReflection]         = ProcessSolarMagicReflection;
    g_AttributeProcessFunctions[atSolarMagicReflectionPercent]  = ProcessSolarMagicReflectionPercent;
    g_AttributeProcessFunctions[atSolarMagicBlock]              = ProcessSolarMagicBlock;
    g_AttributeProcessFunctions[atSolarDamageAbsorb]            = ProcessSolarDamageAbsorb;
    g_AttributeProcessFunctions[atSolarDamageManaShield]        = ProcessSolarDamageManaShield;

    // ------------- 中性内功 ---------------------------------------------->
    g_AttributeProcessFunctions[atSkillNeutralDamage]            = ProcessSkillNeutralDamage;   
    g_AttributeProcessFunctions[atSkillNeutralDamageRand]        = ProcessSkillNeutralDamageRand;   
    g_AttributeProcessFunctions[atSkillNeutralDamagePercent]     = ProcessSkillNeutralDamagePercent;
    g_AttributeProcessFunctions[atNeutralDamageCoefficient]      = ProcessNeutralDamageCoefficient; 

    g_AttributeProcessFunctions[atNeutralAttackPowerBase]        = ProcessNeutralAttackPowerBase;
    g_AttributeProcessFunctions[atNeutralAttackPowerPercent]     = ProcessNeutralAttackPowerPercent;

    g_AttributeProcessFunctions[atNeutralHitBaseRate]            = ProcessNeutralHitBaseRate;
    g_AttributeProcessFunctions[atNeutralHitValue]               = ProcessNeutralHitValue;
    g_AttributeProcessFunctions[atNeutralCriticalStrikeBaseRate] = ProcessNeutralCriticalStrikeBaseRate;
    g_AttributeProcessFunctions[atNeutralCriticalStrike]         = ProcessNeutralCriticalStrike;

    g_AttributeProcessFunctions[atNeutralCriticalDamagePowerBaseKiloNumRate]    = ProcessNeutralCriticalDamagePowerBaseKlioNumRate;
    g_AttributeProcessFunctions[atNeutralCriticalDamagePowerBase]               = ProcessNeutralCriticalDamagePowerBase;
    g_AttributeProcessFunctions[atNeutralCriticalDamagePowerPercent]            = ProcessNeutralCriticalDamagePowerPercent;

    g_AttributeProcessFunctions[atNeutralOvercomeBase]           = ProcessNeutralOvercomeBase;
    g_AttributeProcessFunctions[atNeutralOvercomePercent]        = ProcessNeutralOvercomePercent;
    g_AttributeProcessFunctions[atNeutralMagicResistPercent]     = ProcessNeutralMagicResistPercent;
    g_AttributeProcessFunctions[atNeutralMagicShieldBase]        = ProcessNeutralMagicShieldBase;
    g_AttributeProcessFunctions[atNeutralMagicShieldPercent]     = ProcessNeutralMagicShieldPercent;
    g_AttributeProcessFunctions[atNeutralMagicReflection]        = ProcessNeutralMagicReflection;
    g_AttributeProcessFunctions[atNeutralMagicReflectionPercent] = ProcessNeutralMagicReflectionPercent;
    g_AttributeProcessFunctions[atNeutralMagicBlock]             = ProcessNeutralMagicBlock;
    g_AttributeProcessFunctions[atNeutralDamageAbsorb]           = ProcessNeutralDamageAbsorb;
    g_AttributeProcessFunctions[atNeutralDamageManaShield]       = ProcessNeutralDamageManaShield;

    // ------------- 阴性内功 ---------------------------------------------->
    g_AttributeProcessFunctions[atSkillLunarDamage]              = ProcessSkillLunarDamage;    
    g_AttributeProcessFunctions[atSkillLunarDamageRand]          = ProcessSkillLunarDamageRand;  
    g_AttributeProcessFunctions[atSkillLunarDamagePercent]       = ProcessSkillLunarDamagePercent;
    g_AttributeProcessFunctions[atLunarDamageCoefficient]        = ProcessLunarDamageCoefficient; 

    g_AttributeProcessFunctions[atLunarAttackPowerBase]          = ProcessLunarAttackPowerBase;   
    g_AttributeProcessFunctions[atLunarAttackPowerPercent]       = ProcessLunarAttackPowerPercent;

    g_AttributeProcessFunctions[atLunarHitBaseRate]              = ProcessLunarHitBaseRate;
    g_AttributeProcessFunctions[atLunarHitValue]                 = ProcessLunarHitValue;
    g_AttributeProcessFunctions[atLunarCriticalStrikeBaseRate]   = ProcessLunarCriticalStrikeBaseRate;
    g_AttributeProcessFunctions[atLunarCriticalStrike]           = ProcessLunarCriticalStrike;

    g_AttributeProcessFunctions[atLunarCriticalDamagePowerBaseKiloNumRate]    = ProcessLunarCriticalDamagePowerBaseKlioNumRate;
    g_AttributeProcessFunctions[atLunarCriticalDamagePowerBase]               = ProcessLunarCriticalDamagePowerBase;
    g_AttributeProcessFunctions[atLunarCriticalDamagePowerPercent]            = ProcessLunarCriticalDamagePowerPercent;

    g_AttributeProcessFunctions[atLunarOvercomeBase]             = ProcessLunarOvercomeBase;
    g_AttributeProcessFunctions[atLunarOvercomePercent]          = ProcessLunarOvercomePercent;
    g_AttributeProcessFunctions[atLunarMagicResistPercent]       = ProcessLunarMagicResistPercent;
    g_AttributeProcessFunctions[atLunarMagicShieldBase]          = ProcessLunarMagicShieldBase;
    g_AttributeProcessFunctions[atLunarMagicShieldPercent]       = ProcessLunarMagicShieldPercent;
    g_AttributeProcessFunctions[atLunarMagicReflection]          = ProcessLunarMagicReflection;
    g_AttributeProcessFunctions[atLunarMagicReflectionPercent]   = ProcessLunarMagicReflectionPercent;
    g_AttributeProcessFunctions[atLunarMagicBlock]               = ProcessLunarMagicBlock;
    g_AttributeProcessFunctions[atLunarDamageAbsorb]             = ProcessLunarDamageAbsorb;
    g_AttributeProcessFunctions[atLunarDamageManaShield]         = ProcessLunarDamageManaShield;

    // ------------- 毒性 -------------------------------------------------->
    g_AttributeProcessFunctions[atSkillPoisonDamage]             = ProcessSkillPoisonDamage;   
    g_AttributeProcessFunctions[atSkillPoisonDamageRand]         = ProcessSkillPoisonDamageRand;   
    g_AttributeProcessFunctions[atSkillPoisonDamagePercent]      = ProcessSkillPoisonDamagePercent;
    g_AttributeProcessFunctions[atPoisonDamageCoefficient]       = ProcessPoisonDamageCoefficient; 

    g_AttributeProcessFunctions[atPoisonAttackPowerBase]         = ProcessPoisonAttackPowerBase;
    g_AttributeProcessFunctions[atPoisonAttackPowerPercent]      = ProcessPoisonAttackPowerPercent;

    g_AttributeProcessFunctions[atPoisonHitBaseRate]             = ProcessPoisonHitBaseRate;
    g_AttributeProcessFunctions[atPoisonHitValue]                = ProcessPoisonHitValue;
    g_AttributeProcessFunctions[atPoisonCriticalStrikeBaseRate]  = ProcessPoisonCriticalStrikeBaseRate;
    g_AttributeProcessFunctions[atPoisonCriticalStrike]          = ProcessPoisonCriticalStrike;

    g_AttributeProcessFunctions[atPoisonCriticalDamagePowerBaseKiloNumRate]    = ProcessPoisonCriticalDamagePowerBaseKiloNumRate;
    g_AttributeProcessFunctions[atPoisonCriticalDamagePowerBase]               = ProcessPoisonCriticalDamagePowerBase;
    g_AttributeProcessFunctions[atPoisonCriticalDamagePowerPercent]            = ProcessPoisonCriticalDamagePowerPercent;

    g_AttributeProcessFunctions[atPoisonOvercomeBase]            = ProcessPoisonOvercomeBase;
    g_AttributeProcessFunctions[atPoisonOvercomePercent]         = ProcessPoisonOvercomePercent;
    g_AttributeProcessFunctions[atPoisonMagicResistPercent]      = ProcessPoisonMagicResistPercent;
    g_AttributeProcessFunctions[atPoisonMagicShieldBase]         = ProcessPoisonMagicShieldBase;
    g_AttributeProcessFunctions[atPoisonMagicShieldPercent]      = ProcessPoisonMagicShieldPercent;
    g_AttributeProcessFunctions[atPoisonMagicReflection]         = ProcessPoisonMagicReflection;
    g_AttributeProcessFunctions[atPoisonMagicReflectionPercent]  = ProcessPoisonMagicReflectionPercent;
    g_AttributeProcessFunctions[atPoisonMagicBlock]              = ProcessPoisonMagicBlock;
    g_AttributeProcessFunctions[atPoisonDamageAbsorb]            = ProcessPoisonDamageAbsorb;
    g_AttributeProcessFunctions[atPoisonDamageManaShield]        = ProcessPoisonDamageManaShield;

    // ------------- 治疗 -------------------------------------------------->
    g_AttributeProcessFunctions[atTherapyPowerBase]              = ProcessTherapyPowerBase;   
    g_AttributeProcessFunctions[atTherapyPowerPercent]           = ProcessTherapyPowerPercent;   
    g_AttributeProcessFunctions[atSkillTherapy]                  = ProcessSkillTherapy;  
    g_AttributeProcessFunctions[atSkillTherapyRand]              = ProcessSkillTherapyRand;  
    g_AttributeProcessFunctions[atSkillTherapyPercent]           = ProcessSkillTherapyPercent;
    g_AttributeProcessFunctions[atTherapyCoefficient]            = ProcessTherapyCoefficient;
    g_AttributeProcessFunctions[atBeTherapyCoefficient]          = ProcessBeTherapyCoefficient;

    // ------------- 新手技能有关 ------------------------------------------>
    g_AttributeProcessFunctions[atNoneWeaponAttackSpeedBase]        = ProcessNoneWeaponAttackSpeedBase;
    g_AttributeProcessFunctions[atMeleeWeaponAttackSpeedBase]       = ProcessMeleeWeaponAttackSpeedBase;
    g_AttributeProcessFunctions[atMeleeWeaponAttackSpeedPercentAdd] = ProcessMeleeWeaponAttackSpeedPercentAdd;
    g_AttributeProcessFunctions[atMeleeWeaponAttackSpeedAdditional] = ProcessMeleeWeaponAttackSpeedAdditional;

    g_AttributeProcessFunctions[atRangeWeaponAttackSpeedBase]       = ProcessRangeWeaponAttackSpeedBase;
    g_AttributeProcessFunctions[atRangeWeaponAttackSpeedPercentAdd] = ProcessRangeWeaponAttackSpeedPercentAdd;
    g_AttributeProcessFunctions[atRangeWeaponAttackSpeedAdditional] = ProcessRangeWeaponAttackSpeedAdditional;

    g_AttributeProcessFunctions[atIgnoreOtherAlarmRange]            = ProcessIgnoreOtherAlarmRange;
    g_AttributeProcessFunctions[atSelfAlarmRangeRevise]             = ProcessSelfAlarmRangeRevise;
    g_AttributeProcessFunctions[atOtherAlarmRangeRevise]            = ProcessOtherAlarmRangeRevise;
    g_AttributeProcessFunctions[atUnarm]                            = ProcessUnarm;

#ifdef _SERVER 
    // ---------------- 人物状态相关的属性 --------------------------------->
    g_AttributeProcessFunctions[atDash]                          = ProcessDash;
    g_AttributeProcessFunctions[atDashForward]                   = ProcessDashForward;
    g_AttributeProcessFunctions[atDashBackward]                  = ProcessDashBackward;
    g_AttributeProcessFunctions[atDashLeft]                      = ProcessDashLeft;
    g_AttributeProcessFunctions[atDashRight]                     = ProcessDashRight;
    g_AttributeProcessFunctions[atDashToDstBack]                 = ProcessDashToDstBack;
#endif

    g_AttributeProcessFunctions[atHalt]                          = ProcessHalt;
    g_AttributeProcessFunctions[atFreeze]                        = ProcessFreeze;
    g_AttributeProcessFunctions[atEntrap]                        = ProcessEntrap;
    g_AttributeProcessFunctions[atFear]                          = ProcessFear;

#ifdef _SERVER
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
    g_AttributeProcessFunctions[atCallKnockedDown]               = ProcessCallKnockedDown;
    //-------------- 滑步 -------------------------------------------------->
    g_AttributeProcessFunctions[atCallRepulsed]                  = ProcessRepulsed;
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
    g_AttributeProcessFunctions[atAccumulate]                    = ProcessAccumulate;

    g_AttributeProcessFunctions[atCallBuff]                      = ProcessCallBuff;
    g_AttributeProcessFunctions[atCallBuffAccordingToAttraction] = ProcessBuffAccordingToAttraction;

    // ------------------------- BUFF 驱散 ---------------------------------------------->
    g_AttributeProcessFunctions[atDetachSingleBuff]              = ProcessDetachSingleBuff;

    g_AttributeProcessFunctions[atDetachMultiGroupBuff]          = ProcessDetachMultiGroupBuff;

    g_AttributeProcessFunctions[atDelSingleBuffByFunctionType]         = ProcessDelSingleBuffByFunctionType;

    g_AttributeProcessFunctions[atDelMultiGroupBuffByFunctionType]     = ProcessDelMultiGroupBuffByFunctionType;

    g_AttributeProcessFunctions[atDelSingleBuffByIDAndLevel]           = ProcessDelSingleBuffByIDAndLevel;
    g_AttributeProcessFunctions[atDelSingleGroupBuffByIDAndLevel]      = ProcessDelSingleGroupBuffByIDAndLevel;

    // -------------------------- 仇恨 ---------------------------------------------------->
    g_AttributeProcessFunctions[atModifyThreat]                  = ProcessModifyThreat;
    g_AttributeProcessFunctions[atSpoof]                         = ProcessSpoof;
#endif

    // ------------ 禁魔 ----------------------------------------------------->
    g_AttributeProcessFunctions[atSilence]                       = ProcessSilence;
    g_AttributeProcessFunctions[atSilenceAll]                    = ProcessSilenceAll;

    // ------------ 免疫 ----------------------------------------------------->
    g_AttributeProcessFunctions[atImmunity]                      = ProcessImmunity;

    // ------------ 破绽 ----------------------------------------------------->
    g_AttributeProcessFunctions[atAddWeak]                       = ProcessAddWeak;
    g_AttributeProcessFunctions[atShiftWeak]                     = ProcessShiftWeak;
    g_AttributeProcessFunctions[atAddWeakShield]                 = ProcessAddWeakShield;

    g_AttributeProcessFunctions[atModifySkillCastFlag]           = ProcessModifySkillCastFlag;

    g_AttributeProcessFunctions[atExecuteScript]                 = ProcessExecuteScript;

    g_AttributeProcessFunctions[atPrinkRepresentID]              = ProcessPrinkRepresentID;
    g_AttributeProcessFunctions[atPrinkModel]                    = ProcessPrinkModel;
    
    g_AttributeProcessFunctions[atPrinkForce]                    = ProcessPrinkForce;

    g_AttributeProcessFunctions[atRideHorse]                     = ProcessRideHorse;
    g_AttributeProcessFunctions[atDoAction]                      = ProcessDoAction;
    
    g_AttributeProcessFunctions[atAddExpPercent]                 = ProcessAddExpPercent;
    g_AttributeProcessFunctions[atAddReputationPercent]          = ProcessAddReputationPercent;

    g_AttributeProcessFunctions[atEmployerCastSkillTargetSelf]   = ProcessEmployerCastSkillTargetSelf;

    g_AttributeProcessFunctions[atClearCoolDown]                 = ProcessClearCoolDown;
    g_AttributeProcessFunctions[atResetCoolDown]                 = ProcessResetCoolDown;

    g_AttributeProcessFunctions[atTransferDamageToManaPercent]   = ProcessTransferDamageToManaPercent;
    
    g_AttributeProcessFunctions[atModifyCostManaPercent]         = ProcessModifyCostManaPercent;

    g_AttributeProcessFunctions[atDirectCastMask]                = ProcessDirectCastMask;

#ifdef _SERVER
    g_AttributeProcessFunctions[atPull]                          = ProcessPull;
    g_AttributeProcessFunctions[atTurnToSrc]                     = ProcessTurnToSrc;
    g_AttributeProcessFunctions[atConsumeBuff]                   = ProcessConsumeBuff;
    g_AttributeProcessFunctions[atTopThreat]                     = ProcessTopThreat;
    g_AttributeProcessFunctions[atZeroThreat]                    = ProcessZeroThreat;
#endif

    g_AttributeProcessFunctions[atFormationEffect]               = ProcessFormationEffect;

    g_AttributeProcessFunctions[atAddDamageByDstMoveState]       = ProcessAddDamageByDstMoveState;

    g_AttributeProcessFunctions[atTransferDamage]                = ProcessTransferDamage;
    g_AttributeProcessFunctions[atTransferTherapy]               = ProcessTransferTherapy;

    g_AttributeProcessFunctions[atAddFellowshipAttraction]       = ProcessAddFellowshipAttraction;

    g_AttributeProcessFunctions[atCallHelp]                      = ProcessCallHelp;
    g_AttributeProcessFunctions[atCallRequest]                   = ProcessCallRequest;
    g_AttributeProcessFunctions[atCatchCharacter]                = ProcessCatchCharacter;
    g_AttributeProcessFunctions[atTransmission]                  = ProcessTransmission;

#ifdef _SERVER
    g_AttributeProcessFunctions[atStealth]                       = ProcessStealth;
#endif

    g_AttributeProcessFunctions[atVitalityToParryValueCof]              = ProcessVitalityToParryValueCof;
    g_AttributeProcessFunctions[atVitalityToMaxLifeCof]                 = ProcessVitalityToMaxLifeCof;
    g_AttributeProcessFunctions[atVitalityToPhysicsAttackPowerCof]      = ProcessVitalityToPhysicsAttackPowerCof;
    g_AttributeProcessFunctions[atVitalityToSolarAttackPowerCof]        = ProcessVitalityToSolarAttackPowerCof;
    g_AttributeProcessFunctions[atVitalityToLunarAttackPowerCof]        = ProcessVitalityToLunarAttackPowerCof;
    g_AttributeProcessFunctions[atVitalityToNeutralAttackPowerCof]      = ProcessVitalityToNeutralAttackPowerCof;
    g_AttributeProcessFunctions[atVitalityToPoisonAttackPowerCof]       = ProcessVitalityToPoisonAttackPowerCof;
    g_AttributeProcessFunctions[atVitalityToTherapyPowerCof]            = ProcessVitalityToTherapyPowerCof;
    g_AttributeProcessFunctions[atVitalityToPhysicsShieldCof]           = ProcessVitalityToPhysicsShieldCof;
    g_AttributeProcessFunctions[atSpiritToMaxManaCof]                   = ProcessSpiritToMaxManaCof;
    g_AttributeProcessFunctions[atSpiritToSolarCriticalStrikeCof]       = ProcessSpiritToSolarCriticalStrikeCof;
    g_AttributeProcessFunctions[atSpiritToLunarCriticalStrikeCof]       = ProcessSpiritToLunarCriticalStrikeCof;
    g_AttributeProcessFunctions[atSpiritToNeutralCriticalStrikeCof]     = ProcessSpiritToNeutralCriticalStrikeCof;
    g_AttributeProcessFunctions[atSpiritToPoisonCriticalStrikeCof]      = ProcessSpiritToPoisonCriticalStrikeCof;
    g_AttributeProcessFunctions[atSpiritToPhysicsAttackPowerCof]        = ProcessSpiritToPhysicsAttackPowerCof;
    g_AttributeProcessFunctions[atSpiritToPhysicsCriticalStrikeCof]     = ProcessSpiritToPhysicsCriticalStrikeCof;
    g_AttributeProcessFunctions[atSpiritToManaReplenishCof]             = ProcessSpiritToManaReplenishCof;
    g_AttributeProcessFunctions[atSpiritToSolarMagicShieldCof]          = ProcessSpiritToSolarMagicShieldCof;
    g_AttributeProcessFunctions[atSpiritToLunarMagicShieldCof]          = ProcessSpiritToLunarMagicShieldCof;
    g_AttributeProcessFunctions[atSpiritToNeutralMagicShieldCof]        = ProcessSpiritToNeutralMagicShieldCof;
    g_AttributeProcessFunctions[atSpiritToPoisonMagicShieldCof]         = ProcessSpiritToPoisonMagicShieldCof;
    g_AttributeProcessFunctions[atSpunkToManaReplenishCof]              = ProcessSpunkToManaReplenishCof;
    g_AttributeProcessFunctions[atSpunkToSolarAttackPowerCof]           = ProcessSpunkToSolarAttackPowerCof;
    g_AttributeProcessFunctions[atSpunkToLunarAttackPowerCof]           = ProcessSpunkToLunarAttackPowerCof;
    g_AttributeProcessFunctions[atSpunkToNeutralAttackPowerCof]         = ProcessSpunkToNeutralAttackPowerCof;
    g_AttributeProcessFunctions[atSpunkToPoisonAttackPowerCof]          = ProcessSpunkToPoisonAttackPowerCof;
    g_AttributeProcessFunctions[atStrengthToPhysicsCriticalStrikeCof]   = ProcessStrengthToPhysicsCriticalStrikeCof;
    g_AttributeProcessFunctions[atStrengthToPhysicsAttackPowerCof]      = ProcessStrengthToPhysicsAttackPowerCof;
    g_AttributeProcessFunctions[atAgilityToPhysicsCriticalStrikeCof]    = ProcessAgilityToPhysicsCriticalStrikeCof;

    g_AttributeProcessFunctions[atAutoSelectTarget]                 = ProcessAutoSelectTarget;
    g_AttributeProcessFunctions[atCallMentorBuff]                   = ProcessMentorBuff;
    g_AttributeProcessFunctions[atMentorFormationEffect]            = ProcessMentorFormationEffect;

    g_AttributeProcessFunctions[atAdjustProfessionLevel]            = ProcessAdjustProfessionLevel;

    return true;
}
