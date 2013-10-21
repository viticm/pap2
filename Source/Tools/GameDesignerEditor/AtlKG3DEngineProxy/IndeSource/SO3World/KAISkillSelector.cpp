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
    int     nResult     = SKILL_SELECTOR_IGNORE;
    BOOL    bRetCode    = false;

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

        if (eOTActionType == otActionSkillPrepare || eOTActionType == otActionSkillChannel || eOTActionType == otActionItemSkill)
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

#endif // _SERVER
