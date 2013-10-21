#include "stdafx.h"
#include "./krlskillsequence.h"
#include "../../SO3Represent.h"

int InitSkillSequence(KRLSkillSequence* pRLSkillSequence)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pRLSkillSequence);

    pRLSkillSequence->nLastSkillAnimationIndex = -1;
    pRLSkillSequence->dwCastSkillResetPeriod = (DWORD)g_pRL->m_TableList.GetNumber(RL_NUMBER_CAST_SKILL_RESET_PERIOD);
    pRLSkillSequence->dwLastSkillID = 0;
    pRLSkillSequence->dwLastSkillLevel = 0;
    pRLSkillSequence->dwLastCastSkillTime = 0;

    nResult = true;
Exit0:
    return nResult;
}

int GetSkillNextAnimationID(KRLSkillSequence* pRLSkillSequence, KRLSkillSequnceParam const* pParam, PDWORD pdwAnimationID)
{
    int nResult = false;
    int nLoop = 0;
    DWORD dwTimeNow = 0;

    KGLOG_PROCESS_ERROR(pRLSkillSequence);
    KGLOG_PROCESS_ERROR(pParam);
    KGLOG_PROCESS_ERROR(pdwAnimationID);

    dwTimeNow = g_pRL->m_dwGameLoop;

    if (pRLSkillSequence->dwLastCastSkillTime != 0)
    {
        if (dwTimeNow - pRLSkillSequence->dwLastCastSkillTime > pRLSkillSequence->dwCastSkillResetPeriod)
        {
            pRLSkillSequence->nLastSkillAnimationIndex = -1;
            pRLSkillSequence->dwLastSkillID = 0;
            pRLSkillSequence->dwLastSkillLevel = 0;
            pRLSkillSequence->dwLastCastSkillTime = 0;
        }
    }

    pRLSkillSequence->dwLastCastSkillTime = dwTimeNow;

    if (pParam->dwSkillID != pRLSkillSequence->dwLastSkillID || pParam->dwSkillLevel != pRLSkillSequence->dwLastSkillLevel)
    {
        pRLSkillSequence->nLastSkillAnimationIndex = 0;
        pRLSkillSequence->dwLastSkillID = pParam->dwSkillID;
        pRLSkillSequence->dwLastSkillLevel = pParam->dwSkillLevel;
    }
    else
    {
        ++pRLSkillSequence->nLastSkillAnimationIndex;
        pRLSkillSequence->nLastSkillAnimationIndex %= 4;
    }

    for (; nLoop < RL_SKILL_SEQUENCE_COUNT; ++nLoop)
    {
        *pdwAnimationID = pParam->dwAnimationIDs[pRLSkillSequence->nLastSkillAnimationIndex];

        if (*pdwAnimationID > 0)
            break;

        ++pRLSkillSequence->nLastSkillAnimationIndex;
        pRLSkillSequence->nLastSkillAnimationIndex %= 4;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pdwAnimationID)
        {
            *pdwAnimationID = 0;
        }

        if (pRLSkillSequence)
        {
            pRLSkillSequence->nLastSkillAnimationIndex = -1;
            pRLSkillSequence->dwLastSkillID = 0;
            pRLSkillSequence->dwLastSkillLevel = 0;
            pRLSkillSequence->dwLastCastSkillTime = 0;
        }
    }
    return nResult;
}
