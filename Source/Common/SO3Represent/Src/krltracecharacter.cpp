#include "stdafx.h"
#include "./krltracecharacter.h"
#include "./SO3Represent.h"
#include "./common/kmathtools.h"
#include "SO3World/KSO3World.h"

int TraceCharacter(KRLTraceCharacter* pRLTraceCharacter, D3DXVECTOR3 vPosition, float fFaceDirection, float fPitchDirection)
{
    int nRetCode = false;
    int nResult = false;
    BOOL bPositionChanged = FALSE;
    BOOL bFaceDirectionChanged = FALSE;
    BOOL bPitchDirectionChanged = FALSE;

    KGLOG_PROCESS_ERROR(pRLTraceCharacter);

    if (pRLTraceCharacter->vPosition != vPosition)
    {
        pRLTraceCharacter->vPosition = vPosition;

        bPositionChanged = TRUE;
    }

    if (pRLTraceCharacter->fFaceDirection != fFaceDirection)
    {
        pRLTraceCharacter->fFaceDirection = fFaceDirection;

        bFaceDirectionChanged = TRUE;
    }

    if (pRLTraceCharacter->fPitchDirection != fPitchDirection)
    {
        pRLTraceCharacter->fPitchDirection = fPitchDirection;

        bPitchDirectionChanged = TRUE;
    }

    if (bPositionChanged || bFaceDirectionChanged || bPitchDirectionChanged)
    {
        pRLTraceCharacter->bPositionChanged = bPositionChanged;

        double fTime = GetPerformanceTickCount();

        pRLTraceCharacter->fChangedPeriod += fTime - pRLTraceCharacter->fChangedTime;
        pRLTraceCharacter->fChangedTime = fTime;

        pRLTraceCharacter->fChangedLength += D3DXVec3Length(&(pRLTraceCharacter->vChangedPosition - vPosition));
        pRLTraceCharacter->vChangedPosition = vPosition;

        printf("[RL] %f %d character %d%d%d %.4f %.4f %.4f\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, 
            bPositionChanged, bFaceDirectionChanged, bPitchDirectionChanged, 
            pRLTraceCharacter->fChangedPeriod,
            pRLTraceCharacter->fChangedLength,
            pRLTraceCharacter->fChangedPeriod);
    }

    if (!bPositionChanged && pRLTraceCharacter->bPositionChanged)
    {
        pRLTraceCharacter->fChangedLength = 0.0f;
        pRLTraceCharacter->fChangedPeriod = 0.0;
        pRLTraceCharacter->fChangedTime = GetPerformanceTickCount();
    }

    nResult = true;
Exit0:
    return nResult;
}
