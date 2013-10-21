#include "stdafx.h"
#include "KNpc.h"
#include "KSkill.h"
#include "KThreatList.h"
#include "KAISkillSelector.h"


#define MAX_SELECT_TARGET_COUNT     8   //技能选择释放的时候会考虑8个单位


#ifdef _SERVER
int StandardTargetPeriodEvaluator(KNpc* pNpc, KTarget* pTarget)
{
    int     nResult     = SKILL_SELECTOR_IGNORE;

    assert(pNpc);
    assert(pTarget);

    nResult = SKILL_SELECTOR_READY;
Exit0:
    return nResult;
}

int StandardTargetPeriodRangeEvaluator(KNpc* pNpc, KTarget* pTarget)
{
    int     nResult     = SKILL_SELECTOR_IGNORE;
    int     nRetCode    = false;
    int     nRange      = 0;
    int     nDestX      = 0;
    int     nDestY      = 0;
    int     nDestZ      = 0;

    assert(pNpc);
    assert(pTarget);

    nRetCode = pTarget->GetTarget(&nDestX, &nDestY, &nDestZ);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRange = g_GetDistance2(pNpc->m_nX, pNpc->m_nY, nDestX, nDestY);
    KG_PROCESS_ERROR(nRange > (CELL_LENGTH * CELL_LENGTH * 200));

    nResult = SKILL_SELECTOR_READY;
Exit0:
    return nResult;
}

int StandardSelfPeriodEvaluator(KNpc* pNpc, KTarget* pTarget)
{
    int nResult = SKILL_SELECTOR_IGNORE;

    assert(pNpc);
    assert(pTarget);

    pTarget->SetTarget(pNpc);

    nResult = SKILL_SELECTOR_READY;
Exit0:
    return nResult;
}

int StandardPassiveRespond(KNpc* pNpc, KTarget* pTarget)
{
    int         nResult                 = SKILL_SELECTOR_IGNORE;
    BOOL        bRetCode                = false;
    DWORD       dwRespondCharacterID    = 0;

    assert(pNpc);
    assert(pTarget);

    dwRespondCharacterID = pNpc->m_AIData.dwRespondCharacterID;

    KG_PROCESS_ERROR(dwRespondCharacterID > 0);

    pNpc->m_AIData.dwRespondCharacterID = 0;

    if (IS_PLAYER(dwRespondCharacterID))
    {
        bRetCode = pTarget->SetTarget(ttPlayer, dwRespondCharacterID);
        KG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        bRetCode = pTarget->SetTarget(ttNpc, dwRespondCharacterID);
        KG_PROCESS_ERROR(bRetCode);
    }

    nResult = SKILL_SELECTOR_CAST;
Exit0:
    return nResult;
}

int StandardSelfHPEvaluator(KNpc* pNpc, KTarget* pTarget, int nLifeRate)
{
    int     nResult     = SKILL_SELECTOR_IGNORE;
    int     nRate       = 0;

    assert(pNpc);
    assert(pTarget);

    KGLOG_PROCESS_ERROR(pNpc->m_nMaxLife > 0);
    nRate = pNpc->m_nCurrentLife * 1024 / pNpc->m_nMaxLife;
    KG_PROCESS_ERROR(nRate < nLifeRate);

    pTarget->SetTarget(pNpc);

    nResult = SKILL_SELECTOR_CAST;
Exit0:
    return nResult;
}

int StandardTargetCountEvaluator(KNpc* pNpc, KTarget* pTarget, int nSkillRate)
{
    int                     nResult     = SKILL_SELECTOR_IGNORE;
    int                     nRate       = g_Random(KILO_NUM);
    int                     nCount      = 0;
    const KSimpThreatNode*	cpNode      = NULL;

    assert(pNpc);
    assert(pTarget);

    cpNode = pNpc->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR(cpNode);

    for (int i = 0; i < MAX_SELECT_TARGET_COUNT && cpNode; i++)
    {
        nCount++;
        cpNode = pNpc->m_SimpThreatList.GetNextThreat(thtMainThreat, cpNode);
    }

    KG_PROCESS_ERROR(nRate < (nCount * nSkillRate));

    nResult = SKILL_SELECTOR_CAST;
Exit0:
    return nResult;
}

int StandardChannelingBreak(KNpc* pNpc, KTarget* pTarget, int nSkillRate)
{
    int                     nResult     = SKILL_SELECTOR_IGNORE;
    BOOL                    bRetCode    = false;
    int                     nRate       = g_Random(KILO_NUM);
    const KSimpThreatNode*  cpNode	    = NULL;

    assert(pNpc);
    assert(pTarget);

    KG_PROCESS_ERROR(nRate < nSkillRate);

    cpNode = pNpc->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR(cpNode);

    for (int i = 0; i < MAX_SELECT_TARGET_COUNT && cpNode; i++)
    {
        KOT_ACTION_TYPE eOTActionType = cpNode->pCharacter->m_OTActionParam.eActionType;

        if (
            eOTActionType == otActionSkillPrepare || 
            eOTActionType == otActionSkillChannel || 
            eOTActionType == otActionUseItemPrepare
        )
        {
            pTarget->SetTarget(cpNode->pCharacter);

            nResult = SKILL_SELECTOR_CAST;
            break;
        }

        cpNode = pNpc->m_SimpThreatList.GetNextThreat(thtMainThreat, cpNode);
    }

Exit0:
    return nResult;
}

int StandardNThreat(KNpc* pNpc, KTarget* pTarget, THREAT_TYPE eThreatType, int nThreatIndex)
{
    int                     nResult         = SKILL_SELECTOR_IGNORE;
    BOOL                    bRetCode        = false;
    int                     nThreatCount    = 0;
    const KSimpThreatNode*  cpThreatNode    = NULL;

    assert(pNpc);
    assert(pTarget);
    KGLOG_PROCESS_ERROR(nThreatIndex != 0);

    bRetCode = pNpc->m_SimpThreatList.GetThreatListCount(eThreatType, nThreatCount);
    KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(nThreatCount > 0);

    if (nThreatIndex > 0)
    {
        nThreatIndex = min(nThreatCount, nThreatIndex);
    }
    else
    {
        nThreatIndex = max(1, nThreatCount + nThreatIndex + 1);
    }

    cpThreatNode = pNpc->m_SimpThreatList.GetNthThreat(eThreatType, nThreatIndex);
    KGLOG_PROCESS_ERROR(cpThreatNode);

    pTarget->SetTarget(cpThreatNode->pCharacter);

    nResult = SKILL_SELECTOR_CAST_SPECIAL;
Exit0:
    return nResult;
}

int StandardRandomTargetByBase(KNpc* pNpc, KTarget* pTarget, BOOL bExceptSelectTarget)
{
    BOOL                    bResult         = false;
    int                     nResult         = SKILL_SELECTOR_IGNORE;
    int                     nCount          = 0;
    const KSimpThreatNode*  cpThreatNode    = NULL;

    assert(pNpc);
    assert(pTarget);

    cpThreatNode = pNpc->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    while (cpThreatNode)
    {
        pNpc->m_AIData.TargetGroup[nCount++] = cpThreatNode->pCharacter;

        if (nCount >= AI_TARGET_GROUP_MAX_COUNT)
            break;

        cpThreatNode = pNpc->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
    }
    pNpc->m_AIData.nTargetGroupCount = nCount;

    bResult = FilterTargetGroupByBase(pNpc, bExceptSelectTarget);
    KG_PROCESS_ERROR(bResult);

    nCount = g_Random(pNpc->m_AIData.nTargetGroupCount);

    for (int nIndex = 0; nIndex < pNpc->m_AIData.nTargetGroupCount; nIndex++)
    {
        KCharacter* pCharacter = pNpc->m_AIData.TargetGroup[nIndex];

        if (!pCharacter)
            continue;

        if (nCount-- == 0)
        {
            pTarget->SetTarget(pCharacter);
            break;
        }
    }

    nResult = SKILL_SELECTOR_CAST_SPECIAL;
Exit0:
    return nResult;
}

int StandardRandomTargetByRange(KNpc* pNpc, KTarget* pTarget, int nMinRange, int nMaxRange, BOOL bExceptSelectTarget)
{
    BOOL                    bResult         = false;
    int                     nResult         = SKILL_SELECTOR_IGNORE;
    int                     nCount          = 0;
    const KSimpThreatNode*  cpThreatNode    = NULL;

    assert(pNpc);
    assert(pTarget);
    KGLOG_PROCESS_ERROR(nMinRange <= nMaxRange);
    KGLOG_PROCESS_ERROR(nMinRange >= 0 && nMaxRange >= 0);

    cpThreatNode = pNpc->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    while (cpThreatNode)
    {
        pNpc->m_AIData.TargetGroup[nCount++] = cpThreatNode->pCharacter;

        if (nCount >= AI_TARGET_GROUP_MAX_COUNT)
            break;

        cpThreatNode = pNpc->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
    }
    pNpc->m_AIData.nTargetGroupCount = nCount;
    
    bResult = FilterTargetGroupByRange(pNpc, bExceptSelectTarget, nMinRange, nMaxRange);
    KG_PROCESS_ERROR(bResult);

    nCount = g_Random(pNpc->m_AIData.nTargetGroupCount);

    for (int nIndex = 0; nIndex < pNpc->m_AIData.nTargetGroupCount; nIndex++)
    {
        KCharacter* pCharacter = pNpc->m_AIData.TargetGroup[nIndex];

        if (!pCharacter)
            continue;

        if (nCount-- == 0)
        {
            pTarget->SetTarget(pCharacter);
            break;
        }
    }

    nResult = SKILL_SELECTOR_CAST_SPECIAL;
Exit0:
    return nResult;
}

int StandardRandomTargetByNearest(KNpc* pNpc, KTarget* pTarget, BOOL bExceptSelectTarget)
{
    BOOL                    bResult         = false;
    int                     nResult         = SKILL_SELECTOR_IGNORE;
    int                     nCount          = 0;
    const KSimpThreatNode*  cpThreatNode    = NULL;

    assert(pNpc);
    assert(pTarget);

    cpThreatNode = pNpc->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    while (cpThreatNode)
    {
        pNpc->m_AIData.TargetGroup[nCount++] = cpThreatNode->pCharacter;

        if (nCount >= AI_TARGET_GROUP_MAX_COUNT)
            break;

        cpThreatNode = pNpc->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
    }
    pNpc->m_AIData.nTargetGroupCount = nCount;

    bResult = FilterTargetGroupByNearest(pNpc, bExceptSelectTarget);
    KG_PROCESS_ERROR(bResult);

    pTarget->SetTarget(pNpc->m_AIData.TargetGroup[0]);

    nResult = SKILL_SELECTOR_CAST_SPECIAL;
Exit0:
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
// 以下为各种筛选函数
BOOL FilterTargetGroupByBase(KCharacter* pSelf, BOOL bExceptSelectTarget)
{
    BOOL        bResult                                 = false;
    int         nCount                                  = 0;
    KCharacter* TempGroup[AI_TARGET_GROUP_MAX_COUNT]    = {NULL};

    assert(pSelf);

    for (int nIndex = 0; nIndex < pSelf->m_AIData.nTargetGroupCount; nIndex++)
    {
        BOOL        bEligibleTarget = true;
        KCharacter* pTarget         = pSelf->m_AIData.TargetGroup[nIndex];

        if (!pTarget)
            continue;

        if (bExceptSelectTarget)
            bEligibleTarget = pSelf->m_SelectTarget.m_dwID != pTarget->m_dwID;

        if (bEligibleTarget)
            TempGroup[nCount++] = pSelf->m_AIData.TargetGroup[nIndex];
    }
    KG_PROCESS_ERROR(nCount > 0);

    memset(pSelf->m_AIData.TargetGroup, 0, sizeof(pSelf->m_AIData.TargetGroup));
    pSelf->m_AIData.nTargetGroupCount = nCount;
    for (int nIndex = 0; nIndex < nCount; nIndex++)
        pSelf->m_AIData.TargetGroup[nIndex] = TempGroup[nIndex];

    bResult = true;
Exit0:
    if (!bResult)
    {
        memset(pSelf->m_AIData.TargetGroup, 0, sizeof(pSelf->m_AIData.TargetGroup));
        pSelf->m_AIData.nTargetGroupCount = 0;
    }
    return bResult;
}

BOOL FilterTargetGroupByRange(KCharacter* pSelf, BOOL bExceptSelectTarget, int nMinRange, int nMaxRange)
{
    BOOL        bResult                                 = false;
    int         nCount                                  = 0;
    int         nDistance2                              = 0;
    KCharacter* TempGroup[AI_TARGET_GROUP_MAX_COUNT]    = {NULL};

    assert(pSelf);
    KGLOG_PROCESS_ERROR(nMinRange <= nMaxRange);
    KGLOG_PROCESS_ERROR(nMinRange >= 0 && nMaxRange >= 0);

    for (int nIndex = 0; nIndex < pSelf->m_AIData.nTargetGroupCount; nIndex++)
    {
        BOOL        bEligibleTarget = true;
        KCharacter* pTarget         = pSelf->m_AIData.TargetGroup[nIndex];

        if (!pTarget)
            continue;

        if (bExceptSelectTarget)
            bEligibleTarget = pSelf->m_SelectTarget.m_dwID != pTarget->m_dwID;

        nDistance2 = g_GetDistance2(pSelf->m_nX, pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);

        if (bEligibleTarget && nDistance2 >= nMinRange * nMinRange && nDistance2 <= nMaxRange * nMaxRange)
            TempGroup[nCount++] = pSelf->m_AIData.TargetGroup[nIndex];
    }
    KG_PROCESS_ERROR(nCount > 0);

    memset(pSelf->m_AIData.TargetGroup, 0, sizeof(pSelf->m_AIData.TargetGroup));
    pSelf->m_AIData.nTargetGroupCount = nCount;
    for (int nIndex = 0; nIndex < nCount; nIndex++)
        pSelf->m_AIData.TargetGroup[nIndex] = TempGroup[nIndex];

    bResult = true;
Exit0:
    if (!bResult)
    {
        memset(pSelf->m_AIData.TargetGroup, 0, sizeof(pSelf->m_AIData.TargetGroup));
        pSelf->m_AIData.nTargetGroupCount = 0;
    }
    return bResult;
}

BOOL FilterTargetGroupByNearest(KCharacter* pSelf, BOOL bExceptSelectTarget)
{
    BOOL        bResult             = false;
    int         nDistance2          = 0;
    int         nNearestDistance2   = INT_MAX;
    KCharacter* pTempCharacter      = NULL;

    assert(pSelf);

    for (int nIndex = 0; nIndex < pSelf->m_AIData.nTargetGroupCount; nIndex++)
    {
        BOOL        bEligibleTarget = true;
        KCharacter* pTarget         = pSelf->m_AIData.TargetGroup[nIndex];

        if (!pTarget)
            continue;

        if (bExceptSelectTarget)
            bEligibleTarget = pSelf->m_SelectTarget.m_dwID != pTarget->m_dwID;

        nDistance2 = g_GetDistance2(pSelf->m_nX, pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);

        if (bEligibleTarget && nNearestDistance2 > nDistance2)
        {
            pTempCharacter      = pSelf->m_AIData.TargetGroup[nIndex];
            nNearestDistance2   = nDistance2;
        }
    }
    KG_PROCESS_ERROR(pTempCharacter);

    memset(pSelf->m_AIData.TargetGroup, 0, sizeof(pSelf->m_AIData.TargetGroup));
    pSelf->m_AIData.TargetGroup[0]      = pTempCharacter;
    pSelf->m_AIData.nTargetGroupCount   = 1;

    bResult = true;
Exit0:
    if (!bResult)
    {
        memset(pSelf->m_AIData.TargetGroup, 0, sizeof(pSelf->m_AIData.TargetGroup));
        pSelf->m_AIData.nTargetGroupCount = 0;
    }
    return bResult;
}

#endif // _SERVER
