#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KSO3World.h"
#include "./krlremotecharacterframedata.h"
#include "../actionobject/krlscene.h"
#include "../actionobject/krlcharacter.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"


static BOOL OnFloor(KCharacter* pCharacter)
{
    BOOL bOnFloor = FALSE;
    int nRelativeHeight = 0;

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pCharacter->m_pCell);

    nRelativeHeight = pCharacter->m_nZ - pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;

    switch (pCharacter->m_eMoveState)
    {
    case cmsOnStand:
    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnSit:
        bOnFloor = TRUE;
        break;
    case cmsOnKnockedBack:
    case cmsOnKnockedOff:
    case cmsOnHalt:
    case cmsOnFreeze:
    case cmsOnEntrap:
    case cmsOnDeath:
    case cmsOnDash:
    case cmsOnAutoFly:
    case cmsOnRepulsed:
    case cmsOnRise:
    case cmsOnSkid:
        bOnFloor = nRelativeHeight < XYPOINT_TO_ZPOINT(CELL_LENGTH * 2);
        break;
    case cmsInvalid:
    case cmsOnJump:
    case cmsOnSwim:
    case cmsOnSwimJump:
    case cmsOnFloat:
    case cmsOnKnockedDown:
    case cmsOnPull:
        bOnFloor = FALSE;
        break;
    default:
        ASSERT(0);
    }

Exit0:
    return bOnFloor;
}

KRLRemoteCharacterFrameData::KRLRemoteCharacterFrameData()
: m_nLastRecord(0)
, m_nRecordCount(0)
, m_nX(0)
, m_nY(0)
, m_nZ(0)
, m_bOnFloor(FALSE)
, m_fPositionOffsetY(0.0f)

{
    memset(m_FrameData, 0, sizeof(m_FrameData));
}

int KRLRemoteCharacterFrameData::Interpolate(int nVisible, int nForceUpdate)
{
    int nResult = false;
    double fStartTime = 0.0;
    double fElapsedTime = 0.0;
    double fDelayTime = 0.0;
	int nIter  = 0;
    int nIndex = 0;
    int nNext  = 0;

    KG_PROCESS_SUCCESS(!g_pRL->m_bRunning);

    KG_PROCESS_SUCCESS(!nForceUpdate && m_Current.nMoveState == cmsOnDeath);

    if (m_nRecordCount == 0)
        UpdateFrameData(g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, nVisible);

    fDelayTime = g_pRL->m_TableList.GetNumber(RL_NUMBER_REMOTE_CHARACTER_FRAMEDATA_DELAY_TIME);
    fStartTime = g_pRL->m_fFrameTimeStart;
    fElapsedTime = g_pRL->m_fTime - fStartTime - fDelayTime;

    for (nIter = m_nLastRecord - m_nRecordCount; nIter != m_nLastRecord - 1; ++nIter)
    {
        nIndex = nIter % _countof(m_FrameData);
        nNext  = (nIter + 1) % _countof(m_FrameData);

        if (m_FrameData[nIndex].fFrameTime <= fElapsedTime && m_FrameData[nNext].fFrameTime > fElapsedTime)
        {
            break;
        }
    }

    if (nIter != m_nLastRecord - 1)
	{
		KRLRemoteCharacterPositionData& FrameDataL = m_FrameData[nIndex];
		KRLRemoteCharacterPositionData& FrameDataR = m_FrameData[nNext];

		float fDeltaL = (float)(fElapsedTime - FrameDataL.fFrameTime);
		float fDeltaR = (float)(FrameDataR.fFrameTime - fElapsedTime);
        float fDeltaRange = fDeltaL + fDeltaR;

        ASSERT(abs(fDeltaRange) > FLT_EPSILON);

        float fDeltaRangeReciprocal = 1.0f / fDeltaRange;
        fDeltaL *= fDeltaRangeReciprocal;
        fDeltaR *= fDeltaRangeReciprocal;

        m_Interpolated.vPosition = FrameDataL.vPosition * fDeltaR + FrameDataR.vPosition * fDeltaL;
        m_Interpolated.fFaceDirection = FrameDataR.fFaceDirection;
        m_Interpolated.fPitchDirection = FrameDataR.fPitchDirection;
	}
	else
    {
        KRLRemoteCharacterPositionData& FrameData = m_FrameData[(m_nLastRecord - 1) % _countof(m_FrameData)];

        m_Interpolated.vPosition = FrameData.vPosition;
        m_Interpolated.fFaceDirection = FrameData.fFaceDirection;
        m_Interpolated.fPitchDirection = FrameData.fPitchDirection;

        //printf("[RL] Frame data out of range (%.0f) !\n", m_FrameData[nIndex].fFrameTime - fElapseTime);
    }

Exit1:
    nResult = true;
    return nResult;
}

int KRLRemoteCharacterFrameData::UpdateFrameData(int nGameLoop, int nVisible)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nStartLoop = 0;
    int nElapsedLoop = 0;
	int nX = 0;
    int nY = 0;
    int nZ = 0;
    float fX = 0.0f;
    float fY = 0.0f;
    float fZ = 0.0f;
    float fOffsetY = 0.0f;
    BOOL bOnFloor = FALSE;
    KRLRemoteCharacterPositionData* pLastFrameData = NULL;
    KRLRemoteCharacterPositionData* pFrameData = NULL;

	KG_PROCESS_SUCCESS(m_pCharacter == NULL || m_pCharacter->m_pCell == NULL);

    if (m_nLastRecord > 0)
        pLastFrameData = m_FrameData + ((m_nLastRecord - 1) % _countof(m_FrameData));

    pFrameData = m_FrameData + (m_nLastRecord++ % _countof(m_FrameData));

    if (m_nRecordCount < _countof(m_FrameData))
        ++m_nRecordCount;

	nX = m_pCharacter->m_nX;
    nY = m_pCharacter->m_nY;
    nZ = m_pCharacter->m_nZ;
    bOnFloor = OnFloor(m_pCharacter);

    nStartLoop = g_pRL->m_nStartLoop;
    nElapsedLoop = g_pRL->m_pSO3World->m_nGameLoop - nStartLoop;

    // 坐标相同，使用前一次的坐标，但每隔一秒，强制刷新
    if (m_nX == nX && m_nY == nY && m_nZ == nZ && 
        m_bOnFloor == bOnFloor && pLastFrameData && 
        (!nVisible || (nGameLoop % 0xFF) == (m_pCharacter->m_dwID & 0xFF)))
    {
        pFrameData->vPosition = pLastFrameData->vPosition;
    }
    else
    {
        KRLCharacter* pRLCharacter = NULL;
        KRLScene* pRLScene = NULL;
        IKG3DScene* p3DScene = NULL;

        m_nX = nX;
        m_nY = nY;
        m_nZ = nZ;

        fX = (float)nX;
        fY = (float)nY;
        fZ = (float)nZ;

        KG_ASSERT_EXIT(m_pCharacter->m_pRepresentObject);

        pRLCharacter = reinterpret_cast<KRLCharacter*>(m_pCharacter->m_pRepresentObject);
        KG_ASSERT_EXIT(pRLCharacter);

        pRLScene = pRLCharacter->m_RenderData.GetRLScene();
        KG_ASSERT_EXIT(pRLScene);

        p3DScene = pRLScene->m_p3DScene;
        KG_ASSERT_EXIT(p3DScene);

        hr = GameWorldPositionToScenePosition(p3DScene, pFrameData->vPosition, fX, fY, fZ, 
            bOnFloor && nVisible, m_pCharacter->m_bSlip, IS_PLAYER(m_pCharacter->m_dwID));
        KGLOG_COM_PROCESS_ERROR(hr);

        fOffsetY = 0.0f;
        
        if (pLastFrameData)
            fOffsetY = pFrameData->vPosition.y - pLastFrameData->vPosition.y;

        if (abs(fOffsetY) > 64.0f)
        {
            bOnFloor = FALSE;

            hr = GameWorldPositionToScenePosition(p3DScene, pFrameData->vPosition, fX, fY, fZ, 
                bOnFloor && nVisible, m_pCharacter->m_bSlip, IS_PLAYER(m_pCharacter->m_dwID));
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        if (m_bOnFloor != bOnFloor && pLastFrameData)
        {
            // 人物坐标是，贴地表时取地表坐标，不贴地表时取逻辑坐标转化成3D坐标，但是逻辑坐标>=地表坐标，
            // 所以当发生贴地表和不贴地表之间的转化时，会有误差，需要做平滑处理

            m_fPositionOffsetY = pFrameData->vPosition.y - pLastFrameData->vPosition.y;

            m_bOnFloor = bOnFloor;
        }
    }

    pFrameData->fFrameTime = (double)nElapsedLoop * (1000.0 / 16.0);
    pFrameData->fPitchDirection = (float)m_pCharacter->m_nPitchDirection;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacterFrameData::UpdatePositionOffset()
{
    int nResult = false;

    static float const POSITION_OFFSET_DELTA = 4.0f;

    if (m_fPositionOffsetY > POSITION_OFFSET_DELTA)
    {
        m_fPositionOffsetY -= POSITION_OFFSET_DELTA;
    }
    else if (m_fPositionOffsetY < -POSITION_OFFSET_DELTA)
    {
        m_fPositionOffsetY += POSITION_OFFSET_DELTA;
    }
    else
    {
        m_fPositionOffsetY = 0.0f;
    }

    nResult = true;
    return nResult;
}

D3DXVECTOR3 KRLRemoteCharacterFrameData::GetCurrentPosition() const
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

    if (m_fPositionOffsetY != 0.0f)
    {
        vPosition.y += m_fPositionOffsetY;
    }

    return vPosition;
}

