#include "stdafx.h"
#include "./KStartTimeEvaluation.h"
#include "./KPlayerClient.h"
#include "./KSO3World.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"

#if defined(_CLIENT)

int InitStartTimeEvaluation(KStartTimeEvaluation* pStartTimeEvaluation)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pStartTimeEvaluation);

    pStartTimeEvaluation->dwFirstStartTime = 0;
    pStartTimeEvaluation->dwEvaluationCount = 0;
    pStartTimeEvaluation->lfFirstStartTime = 0.0;
    pStartTimeEvaluation->lfNetStartTime = 0.0;

    nResult = true;
Exit0:
    return nResult;
}

int UpdateStartTimeEvaluation(KStartTimeEvaluation* pStartTimeEvaluation, DWORD dwTimeNow)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwNewStartTime  = 0;
	double lfNewNetStartTime = 0.0;
    double lfTimeNow = 0.0;
    double lfNewNetStartTimeWeight = 0.0;
    int nGameLoop = 0;

    KGLOG_PROCESS_ERROR(pStartTimeEvaluation);

    KG_ASSERT_EXIT(g_pSO3World);

    nGameLoop = g_PlayerClient.GetServerLoop() - g_pSO3World->m_nStartLoop;

    lfTimeNow = dwTimeNow;

    if (pStartTimeEvaluation->dwFirstStartTime == 0)
    {
        pStartTimeEvaluation->lfFirstStartTime = lfTimeNow;
        pStartTimeEvaluation->dwEvaluationCount = 0;
        pStartTimeEvaluation->dwFirstStartTime = dwTimeNow;
    }

	lfNewNetStartTime = lfTimeNow - (double)nGameLoop * 1000 / GAME_FPS - pStartTimeEvaluation->lfFirstStartTime;

    if (pStartTimeEvaluation->dwEvaluationCount++ < GAME_FPS * 2)
        lfNewNetStartTimeWeight = 1.0;
    else
        lfNewNetStartTimeWeight = 0.1;

    pStartTimeEvaluation->lfNetStartTime = pStartTimeEvaluation->lfNetStartTime * (1.0 - lfNewNetStartTimeWeight) + lfNewNetStartTime * lfNewNetStartTimeWeight;

    dwNewStartTime = (DWORD)pStartTimeEvaluation->lfNetStartTime + pStartTimeEvaluation->dwFirstStartTime;

    if (g_pSO3World->m_dwStartTime != dwNewStartTime)
    {
        g_pSO3World->m_dwStartTime = dwNewStartTime;

        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnResetFrameData(g_pSO3World->m_nStartLoop, g_pSO3World->m_dwStartTime);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

#endif	//_CLIENT

