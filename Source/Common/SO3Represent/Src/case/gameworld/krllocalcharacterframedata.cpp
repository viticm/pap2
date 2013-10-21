#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KSO3World.h"
#include "./krllocalcharacterframedata.h"
#include "../actionobject/krlscene.h"
#include "../actionobject/krlcharacter.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"

KRLLocalCharacterFrameData::KRLLocalCharacterFrameData()
: m_nCurrentFrameDataIndex(0)
, m_fPositionBeginOffsetY(0.0f)
, m_fPositionEndOffsetY(0.0f)
, m_fPositionDeltaY(0.0f)
#ifdef ENABLE_RL_INVALIDATE_FRAMEDATA_ADJUST
, m_bInvalidateFrameData(FALSE)
, m_vInvalidateAdjustPositionDelta(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
, m_vInvalidateAdjustPositionDeltaSpeed(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
, m_fInvalidateAdjustDirecitonDelta(0.0f)
, m_fInvalidateAdjustDirecitonDeltaSpeed(0.0f)
#endif
{
    memset(&m_Interpolated, 0, sizeof(m_Interpolated));

    memset(m_FrameData, 0, sizeof(m_FrameData));
}

int KRLLocalCharacterFrameData::GetCurrentFrameDataIndex() const
{
    return m_nCurrentFrameDataIndex;
}

int KRLLocalCharacterFrameData::GetNextFrameDataIndex() const
{
    return (GetCurrentFrameDataIndex() + 1) % _countof(m_FrameData);
}

int KRLLocalCharacterFrameData::GetFrameDataIndex(int nGameLoop) const
{
    for (int nIndex = 0; nIndex < _countof(m_FrameData); ++nIndex)
    {
        if (m_FrameData[nIndex].nGameLoop == nGameLoop)
        {
            return nIndex;
        }
    }
    return m_nCurrentFrameDataIndex;
}

void KRLLocalCharacterFrameData::AdvanceFrameDataIndex()
{
    m_nCurrentFrameDataIndex = GetNextFrameDataIndex();
}

int KRLLocalCharacterFrameData::UpdateFrameData(int nGameLoop, int nVisible)
{
    int nRetCode = false;
    int nResult = false;
    int nDeltaLoop = 0;
    int nGameworldGameLoop = 0;
    int nCurrent = 0;
    int nNext = 0;
    KRLCharacterPositionData PositionData;

    nCurrent = GetCurrentFrameDataIndex();
    nNext = GetNextFrameDataIndex();

    nGameworldGameLoop = g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop;

    nDeltaLoop = nGameworldGameLoop - m_FrameData[nCurrent].nGameLoop;
    if (nDeltaLoop > 0)
    {
        // 逻辑帧超前，更新到当前逻辑帧

        while (nDeltaLoop-- > 1)
        {
            nRetCode = GetCharacterPositionData(&PositionData, m_pCharacter);
            KGLOG_PROCESS_ERROR(nRetCode);

            memcpy(m_FrameData + nNext, m_FrameData + nCurrent, sizeof(m_FrameData[0]));
            m_FrameData[nNext].nGameLoop = m_FrameData[nCurrent].nGameLoop + 1;

            AdvanceFrameDataIndex();

            nCurrent = GetCurrentFrameDataIndex();
            nNext = GetNextFrameDataIndex();
        }

        nRetCode = GetCharacterPositionData(&PositionData, m_pCharacter);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetFrameData(m_FrameData + nNext, m_FrameData + nCurrent, 
            m_pCharacter, &PositionData, m_FrameData[nCurrent].nGameLoop + 1, nVisible);
        KGLOG_PROCESS_ERROR(nRetCode);

        AdvanceFrameDataIndex();

        nCurrent = GetCurrentFrameDataIndex();
        nNext = GetNextFrameDataIndex();
    }

    nDeltaLoop = nGameLoop + 1 - m_FrameData[nCurrent].nGameLoop;
    if (nDeltaLoop > 0)
    {
        // 表现帧超前，预计算到当前表现帧
        if (g_pRL->m_TraceOption.bTracePrefetchCharacterPosition)
        {
            printf("[RL] prefetch character position %d\n", nDeltaLoop);
        }

        while (nDeltaLoop-- > 0)
        {
            nRetCode = PrefetchCharacterPositionData(&PositionData, m_pCharacter, 
                m_FrameData[nCurrent].nGameLoop - nGameworldGameLoop + 1);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = GetFrameData(m_FrameData + nNext, m_FrameData + nCurrent, 
                m_pCharacter, &PositionData, m_FrameData[nCurrent].nGameLoop + 1, nVisible);
            KGLOG_PROCESS_ERROR(nRetCode);

            AdvanceFrameDataIndex();

            nCurrent = GetCurrentFrameDataIndex();
            nNext = GetNextFrameDataIndex();
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLLocalCharacterFrameData::InvalidateFrameData(int nGameLoop)
{
    m_nCurrentFrameDataIndex = GetFrameDataIndex(nGameLoop);

#ifdef ENABLE_RL_INVALIDATE_FRAMEDATA_ADJUST
    m_bInvalidateFrameData = TRUE;
#endif

    return true;
}

static int GetJumpState(KRLCharacterPositionData* pCurrentPositionData, 
                        KRLCharacterPositionData* pNextPositionData,
                        int* pnBeginJump, int* pnDoubleJump)
{
    ASSERT(pCurrentPositionData);
    ASSERT(pNextPositionData);
    ASSERT(pnBeginJump);
    ASSERT(pnDoubleJump);

    if (pCurrentPositionData->nMoveState != cmsOnJump && 
        pNextPositionData->nMoveState == cmsOnJump)
    {
        *pnBeginJump = true;
    }
    else 
    {
        if (pCurrentPositionData->nJumpCount > 0 && 
            pNextPositionData->nJumpCount > 0 &&
            pCurrentPositionData->nJumpCount != pNextPositionData->nJumpCount)
        {
            *pnDoubleJump = true;
        }
    }

    return true;
}

static int UpdateJumpOffsetY(int nBeginJump, int nDoubleJump, float fY,
                             KRLCharacterPositionData* pCurrentPositionData, KRLCharacterPositionData* pNextPositionData,
                             KCharacter* pCharacter, 
                             float* pfBeginOffsetY, float* pfEndOffsetY, float* pfDeltaY,
                             float* fFrameCurrentY, float* fFrameNextY)
{
    int nRetCode = false;
    int nResult = false;

    ASSERT(pCurrentPositionData);
    ASSERT(pNextPositionData);
    ASSERT(pCharacter);
    ASSERT(pfBeginOffsetY);
    ASSERT(pfEndOffsetY);
    ASSERT(pfDeltaY);
    ASSERT(fFrameCurrentY);
    ASSERT(fFrameNextY);

    KG_PROCESS_SUCCESS(!nBeginJump && !nDoubleJump);

    // 人物坐标是，贴地表时取地表坐标，不贴地表时取逻辑坐标转化成3D坐标，但是逻辑坐标>=地表坐标，
    // 所以当发生贴地表和不贴地表之间的转化时，会有误差，需要做平滑处理

    if (nBeginJump)
    {
        nRetCode = GetEndJumpOffsetData(pNextPositionData, pCharacter, 
            pfBeginOffsetY, pfEndOffsetY, pfDeltaY);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = GetEndDoubleJumpOffsetData(pNextPositionData, fY, pCharacter, 
            pfBeginOffsetY, pfEndOffsetY, pfDeltaY);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (*pfBeginOffsetY < -64.0f)
    {
        *pfBeginOffsetY = 0.0f;
        *pfEndOffsetY = 0.0f;
        *pfDeltaY = 0.0f;
    }
    else
    {
        *fFrameCurrentY = *fFrameNextY;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static int UpdatePositionDeltaY(float* pfLeftY, float* pfRightY, float* pfBeginOffsetY, float* pfEndOffsetY, float* pfDeltaY)
{
    int nRetCode = false;
    int nResult = false;

    ASSERT(pfLeftY);
    ASSERT(pfRightY);
    ASSERT(pfBeginOffsetY);
    ASSERT(pfEndOffsetY);
    ASSERT(pfDeltaY);

    KG_PROCESS_SUCCESS(*pfDeltaY == 0.0f);

    *pfLeftY += *pfBeginOffsetY;
    *pfRightY += *pfBeginOffsetY;

    *pfBeginOffsetY += *pfDeltaY * (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

    if (*pfDeltaY > 0.0f)
    {
        if (*pfBeginOffsetY >= *pfEndOffsetY)
        {
            *pfBeginOffsetY = 0.0f;
            *pfDeltaY = 0.0f;
        }
    }
    else
    {
        if (*pfBeginOffsetY <= *pfEndOffsetY)
        {
            *pfBeginOffsetY = 0.0f;
            *pfDeltaY = 0.0f;
        }
    }

Exit1:
    nResult = true;
    return nResult;
}

int KRLLocalCharacterFrameData::Interpolate(int nVisible, int nForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    int nCurrent = 0;
    int nNext = 0;
    int nPrev = 0;
    float fDeltaL = 0.0f;
    float fDeltaR = 0.0f;
    float fFaceDirection = 0.0f;
    double fElapsedTime = 0.0;
    double fFrame = 0.0;
    int nFrame = 0;
    int nBeginJump = false;
    int nDoubleJump = false;
    KRLCharacterPositionData* pCurrentPositionData = NULL;
    KRLCharacterPositionData* pNextPositionData = NULL;
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vPositionL;
    D3DXVECTOR3 vPositionR;

    KG_PROCESS_SUCCESS(!g_pRL->m_bRunning);
    KG_PROCESS_SUCCESS(!nForceUpdate && m_Current.nMoveState == cmsOnDeath);

    fElapsedTime = g_pRL->m_fTime - g_pRL->m_fFrameTimeStart;
    fFrame = fElapsedTime / (1000.0 / GAME_FPS);

    nFrame = (int)fFrame;

    nRetCode = UpdateFrameData(nFrame, nVisible);
    KGLOG_PROCESS_ERROR(nRetCode);

    nCurrent = GetFrameDataIndex(nFrame);
    nNext = (nCurrent + 1) % _countof(m_FrameData);
    nPrev = (nCurrent - 1 + _countof(m_FrameData)) % _countof(m_FrameData);

    pCurrentPositionData = &m_FrameData[nCurrent].PositionData;
    pNextPositionData = &m_FrameData[nNext].PositionData;

    nRetCode = GetJumpState(pCurrentPositionData, pNextPositionData, &nBeginJump, &nDoubleJump);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateJumpOffsetY(nBeginJump, nDoubleJump, m_Interpolated.vPosition.y,
                             pCurrentPositionData, pNextPositionData, m_pCharacter, 
                             &m_fPositionBeginOffsetY, &m_fPositionEndOffsetY, &m_fPositionDeltaY,
                             &m_FrameData[nCurrent].vPosition.y, &m_FrameData[nNext].vPosition.y);
    KGLOG_PROCESS_ERROR(nRetCode);

    vPositionL = m_FrameData[nCurrent].vPosition;
    vPositionR = m_FrameData[nNext].vPosition;

    nRetCode = UpdatePositionDeltaY(&vPositionL.y, &vPositionR.y, &m_fPositionBeginOffsetY, &m_fPositionEndOffsetY, &m_fPositionDeltaY);
    KGLOG_PROCESS_ERROR(nRetCode);

    fDeltaL = (float)(fElapsedTime - m_FrameData[nCurrent].nGameLoop * (1000.0 / GAME_FPS));
    fDeltaR = (float)(-fElapsedTime + m_FrameData[nNext].nGameLoop * (1000.0 / GAME_FPS));

    if (m_FrameData[nCurrent].PositionData.nMoveState == cmsOnDeath && 
        m_FrameData[nNext].PositionData.nMoveState != cmsOnDeath)
    {
        vPositionL = vPositionR;
        m_Interpolated.vPosition = vPositionR;
    }

    nRetCode = InterpolatePosition(vPosition, vPositionL, vPositionR, fDeltaL, fDeltaR);
    KG_CHECK_ERROR(nRetCode);

    nRetCode = InterpolateDirection(&fFaceDirection, 
        m_FrameData[nCurrent].PositionData.nFaceDirection,
        m_FrameData[nNext].PositionData.nFaceDirection,
        fDeltaL, fDeltaR);
    KG_CHECK_ERROR(nRetCode);

#ifdef ENABLE_RL_SWIM_Y_ADJUST
    m_fWaterDelta = m_FrameData[nCurrent].fWaterDelta;
#endif

    if (!nForceUpdate)
    {
        float fDistance = 0.0f;
        BOOL bAdjust = FALSE;

        fDistance = D3DXVec3Length(&(m_Interpolated.vPosition - vPosition));
        if (fDistance < 0.5f)
        {
            vPosition = m_Interpolated.vPosition;
        }

        nRetCode = AdjustPosition(m_pCharacter, vPosition, 1000.0f, &bAdjust);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bAdjust)
        {
            vPosition = m_Interpolated.vPosition;
        }
    }

#ifdef ENABLE_RL_INVALIDATE_FRAMEDATA_ADJUST
    if (m_bInvalidateFrameData)
    {
        static int const RL_INVALIDATE_FRAMEDATA_FRAMECOUNT = 32;

        m_bInvalidateFrameData = FALSE;

        m_vInvalidateAdjustPositionDelta = vPosition - m_Interpolated.vPosition;
        m_fInvalidateAdjustDirecitonDelta = SubtractDirection(fFaceDirection, m_Interpolated.fFaceDirection);

        m_vInvalidateAdjustPositionDeltaSpeed = m_vInvalidateAdjustPositionDelta / RL_INVALIDATE_FRAMEDATA_FRAMECOUNT;
        m_fInvalidateAdjustDirecitonDeltaSpeed = m_fInvalidateAdjustDirecitonDelta / RL_INVALIDATE_FRAMEDATA_FRAMECOUNT;

        if (abs(m_fInvalidateAdjustDirecitonDelta) < D3DX_PI / 360.0f || D3DXVec3Length(&m_vInvalidateAdjustPositionDelta) < 0.5f)
        {
            m_fInvalidateAdjustDirecitonDelta = 0.0f;
        }
    }

    if (m_fInvalidateAdjustDirecitonDelta == 0.0f)
    {
        m_Interpolated.vPosition = vPosition;
        m_Interpolated.fFaceDirection = fFaceDirection;
    }
    else
    {
        m_vInvalidateAdjustPositionDelta -= m_vInvalidateAdjustPositionDeltaSpeed;
        m_fInvalidateAdjustDirecitonDelta -= m_fInvalidateAdjustDirecitonDeltaSpeed;

        if (abs(m_fInvalidateAdjustDirecitonDelta) < D3DX_PI / 360.0f || D3DXVec3Length(&m_vInvalidateAdjustPositionDelta) < 0.5f)
        {
            m_vInvalidateAdjustPositionDelta = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            m_fInvalidateAdjustDirecitonDelta = 0.0f;
        }

        m_Interpolated.vPosition = vPosition - m_vInvalidateAdjustPositionDelta;
        m_Interpolated.fFaceDirection = fFaceDirection - m_fInvalidateAdjustDirecitonDelta;
    }
#else
    m_Interpolated.vPosition = vPosition;
    m_Interpolated.fFaceDirection = fFaceDirection;
#endif // ENABLE_RL_INVALIDATE_FRAMEDATA_ADJUST

    if (g_pRL->m_CurrentTraceOption.bTraceCharacter)
    {
        nRetCode = TraceCharacter(&g_pRL->m_TraceCharacter, m_Interpolated.vPosition, m_Interpolated.fFaceDirection, m_Interpolated.fPitchDirection);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

D3DXVECTOR3 KRLLocalCharacterFrameData::GetCurrentPosition() const
{
    D3DXVECTOR3 vPosition(m_Interpolated.vPosition);

    if (m_pCharacter && IS_PLAYER(m_pCharacter->m_dwID))
    {
#ifdef ENABLE_RL_SWIM_Y_ADJUST
        if (m_fWaterDelta != 0.0f)
        {
            vPosition.y += m_fWaterDelta;
        }
#endif
    }

    return vPosition;
}

