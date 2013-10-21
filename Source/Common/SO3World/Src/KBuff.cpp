#include "stdafx.h"
#include "KSkill.h"
#include "KBuff.h"
#include "KBuffList.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KPlayerServer.h"

bool operator < (const KBUFF_RECIPE_KEY& lhs, const KBUFF_RECIPE_KEY& rhs)
{
    return (memcmp(&lhs, &rhs, sizeof(KBUFF_RECIPE_KEY)) < 0);
}

bool operator == (const KBUFF_RECIPE_KEY& lhs, const KBUFF_RECIPE_KEY& rhs)
{
    return (memcmp(&lhs, &rhs, sizeof(KBUFF_RECIPE_KEY)) == 0);
}

KBuffRecipePointer::KBuffRecipePointer()
{
    m_pBuffInfo = NULL;
}

KBuff* KBuffRecipePointer::SetRecipeKey(const KBUFF_RECIPE_KEY& crRecipeKey)
{
    KBuff* pResult = NULL;

    m_BuffRecipeKey = crRecipeKey;

    m_pBuffInfo = g_pSO3World->m_BuffManager.GetBuff(m_BuffRecipeKey);
    KG_PROCESS_ERROR(m_pBuffInfo);

    m_dwSerialNumber = m_pBuffInfo->m_dwSerialNumber;

    pResult = m_pBuffInfo;
Exit0:
    return pResult;
}

KBUFF_RECIPE_KEY& KBuffRecipePointer::GetRecipeKey()
{
    return m_BuffRecipeKey;
}

void KBuffRecipePointer::ClearRecipeKey()
{
    m_BuffRecipeKey.dwID = 0;
    m_pBuffInfo = NULL;
}

KBuff* KBuffRecipePointer::GetPointer()
{
    KBuff* pResult = NULL;

    KG_PROCESS_ERROR(m_pBuffInfo);

    if (m_dwSerialNumber != m_pBuffInfo->m_dwSerialNumber)
    {
        m_pBuffInfo = g_pSO3World->m_BuffManager.GetBuff(m_BuffRecipeKey);
        KG_PROCESS_ERROR(m_pBuffInfo);

        m_dwSerialNumber = m_pBuffInfo->m_dwSerialNumber;
    }

    pResult = m_pBuffInfo;
Exit0:
    return pResult;
}

BOOL KBuff::Apply(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode)
{
    BOOL bResult        = false;;
    int  nApplyCount    = 0;

    nApplyCount = rBuffListNode.nStackNum;
    if (m_bIsCountable)
    {
        nApplyCount = 1;
    }

    for (int i = 0; i < nApplyCount; i++)
    {
        pCharacter->ApplyAttribute(m_pBeginAttribList);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuff::UnApply(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode)
{
    BOOL bResult        = false;
    int  nApplyCount    = 0;

    nApplyCount = rBuffListNode.nStackNum;
    if (m_bIsCountable)
    {
        nApplyCount = 1;
    }

    for (int i = 0; i < nApplyCount; i++)
    {
        pCharacter->UnApplyAttribute(m_pBeginAttribList);
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KBuff::Active(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    if (m_pActiveAttribList)
    {
        bRetCode = ApplyAttributeNotRollBack(pCharacter, rBuffListNode, m_pActiveAttribList);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuff::EndTime(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    if (m_pEndTimeAttribList)
    {
        bRetCode = ApplyAttributeNotRollBack(pCharacter, rBuffListNode, m_pEndTimeAttribList);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuff::ApplyAttributeNotRollBack(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode, KAttribute* pAttribute)
{
    BOOL                                bResult                 = false;
    BOOL                                bRetCode                = false;
    int                                 nDamageValue            = 0;
    int                                 nDenominator            = 0;
    int                                 nCriticalStrikeValue    = 0;
    int                                 nDefCriticalStrikeValue = 0;
    int                                 nRandomValue            = 0;
    BOOL                                bShield                 = false;
    BOOL                                bForceSync              = false;
    KCharacter*                         pCharacterSrc           = NULL;
    KBuff*                              pBuff                   = NULL;

    KCharacter::KSKILL_ATTRIBUTE_PARAM* pOldDstAttributeParam   = NULL;
    KCharacter::KSKILL_ATTRIBUTE_PARAM  AttributeParam;

    KSKILL_CALCULATE_RESULT*            pOldSrcResult           = NULL;
    KSKILL_CALCULATE_RESULT*            pOldDstResult           = NULL;
    KSKILL_CALCULATE_RESULT             SrcResult;
    KSKILL_CALCULATE_RESULT             DstResult;

    assert(pCharacter);
    assert(pAttribute);

    memset(&AttributeParam, 0, sizeof(AttributeParam));
    memset(&SrcResult, 0, sizeof(SrcResult));
    memset(&DstResult, 0, sizeof(DstResult));

    pOldDstAttributeParam = pCharacter->m_pSkillAttributeParam;
    pCharacter->m_pSkillAttributeParam = &AttributeParam;

    pOldSrcResult = pCharacter->m_pSrcSkillCalculateResult;
    pOldDstResult = pCharacter->m_pDstSkillCalculateResult;

    pCharacter->m_pDstSkillCalculateResult = &DstResult;
    pCharacter->m_pSrcSkillCalculateResult = &SrcResult;

    pBuff = rBuffListNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    SrcResult.EffectSrc.eType   = setBuff;
    SrcResult.EffectSrc.dwID    = pBuff->m_dwID;
    SrcResult.EffectSrc.nLevel  = pBuff->m_nLevel;

    DstResult.EffectSrc.eType   = setBuff;
    DstResult.EffectSrc.dwID    = pBuff->m_dwID;
    DstResult.EffectSrc.nLevel  = pBuff->m_nLevel;

    AttributeParam.SkillRecipePointer = rBuffListNode.SkillRecipePointer;

    bRetCode = IS_PLAYER(rBuffListNode.dwSkillSrcID);
    if (bRetCode)
    {
        pCharacterSrc = g_pSO3World->m_PlayerSet.GetObj(rBuffListNode.dwSkillSrcID);
    }
    else
    {
        pCharacterSrc = g_pSO3World->m_NpcSet.GetObj(rBuffListNode.dwSkillSrcID);
    }

    AttributeParam.dwSkillSrcID             = rBuffListNode.dwSkillSrcID;
    AttributeParam.nSkillSrcLevel           = rBuffListNode.nSkillSrcLevel;
    AttributeParam.pSkillDst                = pCharacter;
    AttributeParam.pSkillSrc                = pCharacterSrc;

    AttributeParam.nPhysicsDamage           = rBuffListNode.nPhysicsDamage;
    AttributeParam.nSolarDamage             = rBuffListNode.nSolarDamage;
    AttributeParam.nNeutralDamage           = rBuffListNode.nNeutralDamage;
    AttributeParam.nLunarDamage             = rBuffListNode.nLunarDamage;
    AttributeParam.nPoisonDamage            = rBuffListNode.nPoisonDamage;
    AttributeParam.nTherapy                 = rBuffListNode.nTherapy;
    AttributeParam.nDamageAddPercent        = pBuff->m_nDamageAddPercent;

    if (pCharacter->m_bSystemShield)
    {
        DstResult.bShield   = true;
        bForceSync          = true;

        goto Exit1;
    }

    // 计算暴击
    nCriticalStrikeValue                    = rBuffListNode.nCriticalStrike;

    if (!pBuff->m_bCanCancel)   // debuff和dot要计算自身韧性
    {
        nDenominator = pCharacter->m_nToughness + 74 * pCharacter->m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        nDefCriticalStrikeValue = pCharacter->m_nToughnessBaseRate + MAX_HIT_VALUE * pCharacter->m_nToughness / nDenominator;
    }

    nCriticalStrikeValue = max((nCriticalStrikeValue - nDefCriticalStrikeValue), 0);
    
    AttributeParam.bCriticalStrike          = false;

    nRandomValue = g_Random(MAX_HIT_VALUE);
    if (nRandomValue <= nCriticalStrikeValue)
    {
        AttributeParam.bCriticalStrike      = true;
    }
    
    AttributeParam.nStackNum                = rBuffListNode.nStackNum;

    AttributeParam.nBaseThreatCoefficient       = rBuffListNode.nBaseThreatCoefficient;
    AttributeParam.nDamageThreatCoefficient     = rBuffListNode.nDamageThreatCoefficient;
    AttributeParam.nTherapyThreatCoefficient    = rBuffListNode.nTherapyThreatCoefficient;

    AttributeParam.nDamgeToLifeForSelf      = rBuffListNode.nStealLifePercent;
    AttributeParam.nDamgeToManaForSelf      = rBuffListNode.nStealManaPercent;

    pCharacter->m_pDstSkillCalculateResult->bHitTarget = true;
    pCharacter->m_pSrcSkillCalculateResult->bHitTarget = true;
    
    pCharacter->m_pDstSkillCalculateResult->bCriticalStrike = AttributeParam.bCriticalStrike;
    pCharacter->m_pSrcSkillCalculateResult->bCriticalStrike = AttributeParam.bCriticalStrike;

    pCharacter->ApplyAttribute(pAttribute);

    pCharacter->ConcludeResult(0, AttributeParam.bCriticalStrike, &nDamageValue);


Exit1:
    if (nDamageValue || bForceSync)
    {
        g_PlayerServer.DoSkillEffectResult(pCharacterSrc, pCharacter, pCharacter, pCharacter->m_pDstSkillCalculateResult, 0);
        g_PlayerServer.DoSyncBehitRepresent(pCharacterSrc, pCharacter, pCharacter, pCharacter->m_pDstSkillCalculateResult, 0);
    }

    bResult = true;
Exit0:
    pCharacter->m_pSkillAttributeParam      = pOldDstAttributeParam;
    pCharacter->m_pDstSkillCalculateResult  = pOldDstResult;
    pCharacter->m_pSrcSkillCalculateResult  = pOldSrcResult;

    return bResult;
}
#endif

DEFINE_LUA_CLASS_BEGIN(KBuff)
    REGISTER_LUA_INTEGER(KBuff, Level)
    REGISTER_LUA_INTEGER(KBuff, Count)
    REGISTER_LUA_INTEGER(KBuff, Interval)
DEFINE_LUA_CLASS_END(KBuff)
