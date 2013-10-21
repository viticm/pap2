#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./krlcharacterframedata.h"
#include "../actionobject/krlscene.h"
#include "../actionobject/krlcharacter.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"


int GetSkillBuffData(KCharacter* pCharacter, DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT])
{
    int nRetCode = false;
    int nResult = false;
    IKGSO3WorldClient* pSO3WorldClient = NULL;

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    nRetCode = pSO3WorldClient->GetRepresentState(pCharacter, brpTrample, dwSkillBuffs[SKILL_BUFF_PART_BOTTOM]);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pSO3WorldClient->GetRepresentState(pCharacter, brpBody, dwSkillBuffs[SKILL_BUFF_PART_MIDDLE]);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pSO3WorldClient->GetRepresentState(pCharacter, brpTopHead, dwSkillBuffs[SKILL_BUFF_PART_TOP]);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(dwSkillBuffs, 0, sizeof(DWORD) * SKILL_BUFF_PART_COUNT);
    }
    return nResult;
}

int GetWaterHeight(KCharacter* pCharacter, D3DXVECTOR3 vPosition, float& fWaterHeight)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;
    D3DXVECTOR3 vWaterPosition;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pRepresentObject);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KG_ASSERT_EXIT(pRLCharacter);

    pRLScene = pRLCharacter->m_RenderData.GetRLScene();
    KG_ASSERT_EXIT(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KG_ASSERT_EXIT(p3DScene);

    hr = p3DScene->GetWaterHeight(&vWaterPosition, &vPosition);
    KG_COM_PROCESS_ERROR(hr);

    fWaterHeight = vWaterPosition.y;

    nResult = true;
Exit0:
    if (!nResult)
    {
        fWaterHeight = 0.0f;
    }
    return nResult;
}

int GetFloorHeight(KCharacter* pCharacter, D3DXVECTOR3 vPosition, float& fFloorHeight)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pRepresentObject);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KG_ASSERT_EXIT(pRLCharacter);

    pRLScene = pRLCharacter->m_RenderData.GetRLScene();
    KG_ASSERT_EXIT(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KG_ASSERT_EXIT(p3DScene);

    hr = p3DScene->GetFloor(&vPosition, &fFloorHeight);
    KG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    if (!nResult)
    {
        fFloorHeight = 0.0f;
    }
    return nResult;
}

int GetCharacterPositionData(KRLCharacterPositionData* pData, KCharacter* pCharacter)
{
    int nResult = false;
    IKGSO3WorldClient* pSO3WorldClient = NULL;

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pCharacter->m_pCell);

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    pData->nSubjectiveVelocity = pSO3WorldClient->GetSubjectiveVelocity(pCharacter);
    pData->nDirectionXY = pCharacter->m_nDirectionXY;
    pData->nFaceDirection = pCharacter->m_nFaceDirection;
    pData->nX = pCharacter->m_nX;
    pData->nY = pCharacter->m_nY;
    pData->nZ = pCharacter->m_nZ;
    pData->nSubjectiveDirection;
    pData->nJumpCount = pCharacter->m_nJumpCount;
    pData->nMoveState = pCharacter->m_eMoveState;
    pData->bSlip = pCharacter->m_bSlip;
    pData->bWater = pCharacter->m_pCell->m_BaseInfo.dwCellType == ctWater;
    pData->nCellHighLayer = (int)pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;

    nResult = true;
Exit0:
    return nResult;
}

int PrefetchCharacterPositionData(KRLCharacterPositionData* pData, KCharacter* pCharacter, int nGameLoop)
{
    int nRetCode = false;
    int nResult = false;
    IKGSO3WorldClient* pSO3WorldClient = NULL;
    KPOSITION_RECORD PositionRecord;

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    KGLOG_PROCESS_ERROR(pCharacter);

    nRetCode = pSO3WorldClient->BeginForwardCharacterPosition(pCharacter, &PositionRecord);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pSO3WorldClient->ForwardCharacterPosition(pCharacter, nGameLoop);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetCharacterPositionData(pData, pCharacter);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (pSO3WorldClient && pCharacter)
    {
        nRetCode = pSO3WorldClient->EndForwardCharacterPosition(pCharacter, &PositionRecord);
        KGLOG_CHECK_ERROR(nRetCode);
    }
    return nResult;
}

BOOL OnFloor(KRLCharacterPositionData* pData)
{
    BOOL bOnFloor = FALSE;
    int nRelativeHeight = 0;

    KGLOG_PROCESS_ERROR(pData);

    switch (pData->nMoveState)
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
        nRelativeHeight = pData->nZ - pData->nCellHighLayer;
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

#ifdef ENABLE_RL_SWIM_Y_ADJUST
float GetWaterDelta(IKG3DScene* p3DScene, KRLCharacterPositionData* pPositionData, 
                    D3DXVECTOR3 vCharacterPosition, int nCharacterHeight, float fX, float fY, float fZ)
{
    static float const WATER_DELTA_PATCH = 7.5f;

    int nRetCode = false;
    HRESULT hr = E_FAIL;
    float fWaterDelta = 0.0f;
    float f3DWaterHeight = 0.0f;
    float fGWWaterHeight = 0.0f;
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vWaterPosition;

    ASSERT(p3DScene);

    KGLOG_PROCESS_ERROR(pPositionData);

    KG_PROCESS_ERROR(pPositionData->nMoveState == cmsOnSwim || pPositionData->nMoveState == cmsOnFloat || pPositionData->nMoveState == cmsOnSwimJump);
    KG_PROCESS_ERROR(pPositionData->bWater);
    KG_PROCESS_ERROR(pPositionData->nZ + nCharacterHeight > pPositionData->nCellHighLayer); // 这里其实可能会有一个bug，就是上面的Cell可能还是水。

    hr = p3DScene->GetWaterHeight(&vWaterPosition, &vCharacterPosition);
    KG_COM_PROCESS_ERROR(hr);

    f3DWaterHeight = vWaterPosition.y;

    fGWWaterHeight = (float)pPositionData->nCellHighLayer;

    hr = GameWorldPositionToScenePosition(p3DScene, vPosition, fX, fY, fGWWaterHeight, FALSE, FALSE, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    fGWWaterHeight = vPosition.y;

    fWaterDelta = f3DWaterHeight - fGWWaterHeight - WATER_DELTA_PATCH;

Exit0:
    return fWaterDelta;
}
#endif

int GetFrameData(KRLCharacterPositionFrameData* pFrameData, KRLCharacterPositionFrameData const* pPrevFrameData, KCharacter* pCharacter,
                 KRLCharacterPositionData* pPositionData, int nGameLoop, int nVisible)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nPositionChanged = false;
    int nUsePrevPosition = false;

    KGLOG_PROCESS_ERROR(pFrameData);
    KGLOG_PROCESS_ERROR(pPrevFrameData);
    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pPositionData);

    memcpy(&pFrameData->PositionData, pPositionData, sizeof(pFrameData->PositionData));

    pFrameData->nGameLoop = nGameLoop;

    pFrameData->bOnFloor = OnFloor(pPositionData);

    if (pPrevFrameData != pFrameData ||
        pPrevFrameData->PositionData.nX != pFrameData->PositionData.nX || 
        pPrevFrameData->PositionData.nY != pFrameData->PositionData.nY || 
        pPrevFrameData->PositionData.nZ != pFrameData->PositionData.nZ)
    {
        nPositionChanged = true;
    }

    if (!nPositionChanged && 
        pPrevFrameData->bOnFloor == pFrameData->bOnFloor && 
        (!nVisible || (nGameLoop % 0xFF) == (pCharacter->m_dwID & 0xFF)))
    {
        // 坐标相同，使用前一次的坐标，但每隔一秒，强制刷新

        pFrameData->vPosition = pPrevFrameData->vPosition;

        nUsePrevPosition = true;
    }

    if (!nUsePrevPosition)
    {
        KRLCharacter* pRLCharacter = NULL;
        KRLScene* pRLScene = NULL;
        IKG3DScene* p3DScene = NULL;

        float fX = (float)pFrameData->PositionData.nX;
        float fY = (float)pFrameData->PositionData.nY;
        float fZ = (float)pFrameData->PositionData.nZ;

        KG_ASSERT_EXIT(pCharacter->m_pRepresentObject);

        pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
        KG_ASSERT_EXIT(pRLCharacter);

        pRLScene = pRLCharacter->m_RenderData.GetRLScene();
        KG_ASSERT_EXIT(pRLScene);

        p3DScene = pRLScene->m_p3DScene;
        KG_ASSERT_EXIT(p3DScene);

        hr = GameWorldPositionToScenePosition(
            p3DScene, 
            pFrameData->vPosition, 
            fX, fY, fZ, 
            pFrameData->bOnFloor && nVisible, 
            pPositionData->bSlip, 
            IS_PLAYER(pCharacter->m_dwID));
        KGLOG_COM_PROCESS_ERROR(hr);

#ifdef ENABLE_RL_SWIM_Y_ADJUST
        pFrameData->fWaterDelta = GetWaterDelta(p3DScene, pPositionData, pFrameData->vPosition, pCharacter->m_nHeight, fX, fY, fZ);
#endif
    }

    nResult = true;
Exit0:
    return nResult;
}

int PrefetchEndJumpCharacterPositionData(KRLCharacterPositionData* pData, KCharacter* pCharacter, int* pnGameLoop)
{
    int nRetCode = false;
    int nResult = false;
    int nGameLoop = 0;
    IKGSO3WorldClient* pSO3WorldClient = NULL;
    KPOSITION_RECORD PositionRecord;

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    KGLOG_PROCESS_ERROR(pCharacter);

    nRetCode = pSO3WorldClient->BeginForwardCharacterPosition(pCharacter, &PositionRecord);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetCharacterPositionData(pData, pCharacter);
    KGLOG_PROCESS_ERROR(nRetCode);

    do
    {
        nRetCode = pSO3WorldClient->ForwardCharacterPosition(pCharacter, 1);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetCharacterPositionData(pData, pCharacter);
        KGLOG_PROCESS_ERROR(nRetCode);

        ++nGameLoop;
    }
    while (!OnFloor(pData) && nGameLoop < 10 * GAME_FPS);

    if (pnGameLoop)
        *pnGameLoop = nGameLoop;

    nResult = true;
Exit0:
    if (pSO3WorldClient && pCharacter)
    {
        nRetCode = pSO3WorldClient->EndForwardCharacterPosition(pCharacter, &PositionRecord);
        KGLOG_CHECK_ERROR(nRetCode);
    }
    return nResult;
}

int GetEndJumpOffsetData(KRLCharacterPositionData const* pBeginJumpPositionData, KCharacter* pCharacter, 
                         float* pfBeginOffsetY, float* pfEndOffsetY, float* pfDeltaY)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nGameLoop = 0;
    D3DXVECTOR3 v3DPosition;
    D3DXVECTOR3 vGWPosition;
    KRLCharacterPositionData EndJumpPositionData;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;
    float fBeginJumpOffsetY = 0.0f;
    float fEndJumpOffsetY = 0.0f;

    KGLOG_PROCESS_ERROR(pBeginJumpPositionData);
    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pfBeginOffsetY);
    KGLOG_PROCESS_ERROR(pfEndOffsetY);
    KGLOG_PROCESS_ERROR(pfDeltaY);

    nRetCode = PrefetchEndJumpCharacterPositionData(&EndJumpPositionData, pCharacter, &nGameLoop);
    KGLOG_PROCESS_ERROR(nRetCode);

    KG_ASSERT_EXIT(pCharacter->m_pRepresentObject);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KG_ASSERT_EXIT(pRLCharacter);

    pRLScene = pRLCharacter->m_RenderData.GetRLScene();
    KG_ASSERT_EXIT(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KG_ASSERT_EXIT(p3DScene);

    // Begin jump
    if (pBeginJumpPositionData->bWater)
    {
        fBeginJumpOffsetY = 0.0f;
    }
    else
    {
        hr = GameWorldPositionToScenePosition(p3DScene, v3DPosition, 
            (float)pBeginJumpPositionData->nX, 
            (float)pBeginJumpPositionData->nY, 
            (float)pBeginJumpPositionData->nZ, 
            TRUE, pBeginJumpPositionData->bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = GameWorldPositionToScenePosition(p3DScene, vGWPosition, 
            (float)pBeginJumpPositionData->nX, 
            (float)pBeginJumpPositionData->nY, 
            (float)pBeginJumpPositionData->nZ, 
            FALSE, pBeginJumpPositionData->bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        fBeginJumpOffsetY = v3DPosition.y - vGWPosition.y;
    }

    // End jump
    if (EndJumpPositionData.bWater)
    {
        fEndJumpOffsetY = 0.0f;
    }
    else
    {
        hr = GameWorldPositionToScenePosition(p3DScene, v3DPosition, 
            (float)EndJumpPositionData.nX, 
            (float)EndJumpPositionData.nY, 
            (float)EndJumpPositionData.nZ, 
            TRUE, EndJumpPositionData.bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = GameWorldPositionToScenePosition(p3DScene, vGWPosition, 
            (float)EndJumpPositionData.nX, 
            (float)EndJumpPositionData.nY, 
            (float)EndJumpPositionData.nZ, 
            FALSE, EndJumpPositionData.bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        fEndJumpOffsetY = v3DPosition.y - vGWPosition.y;
    }

    // Fetch result
    KG_ASSERT_EXIT(nGameLoop > 0);

    *pfBeginOffsetY = fBeginJumpOffsetY;
    *pfEndOffsetY = fEndJumpOffsetY;
    *pfDeltaY = (fEndJumpOffsetY - fBeginJumpOffsetY) / ((float)nGameLoop * (1000.0f / GAME_FPS));

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pfBeginOffsetY)
            *pfBeginOffsetY = 0.0f;
        if (pfDeltaY)
            *pfDeltaY = 0.0f;
    }
    return nResult;
}

int GetEndDoubleJumpOffsetData(KRLCharacterPositionData const* pBeginJumpPositionData, 
                               float fBeginY, KCharacter* pCharacter, 
                               float* pfBeginOffsetY, float* pfEndOffsetY, float* pfDeltaY)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nGameLoop = 0;
    D3DXVECTOR3 v3DPosition;
    D3DXVECTOR3 vGWPosition;
    KRLCharacterPositionData EndJumpPositionData;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;
    float fBeginJumpOffsetY = 0.0f;
    float fEndJumpOffsetY = 0.0f;

    KGLOG_PROCESS_ERROR(pBeginJumpPositionData);
    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pfBeginOffsetY);
    KGLOG_PROCESS_ERROR(pfEndOffsetY);
    KGLOG_PROCESS_ERROR(pfDeltaY);

    nRetCode = PrefetchEndJumpCharacterPositionData(&EndJumpPositionData, pCharacter, &nGameLoop);
    KGLOG_PROCESS_ERROR(nRetCode);

    KG_ASSERT_EXIT(pCharacter->m_pRepresentObject);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KG_ASSERT_EXIT(pRLCharacter);

    pRLScene = pRLCharacter->m_RenderData.GetRLScene();
    KG_ASSERT_EXIT(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KG_ASSERT_EXIT(p3DScene);

    // Begin jump
    if (pBeginJumpPositionData->bWater)
    {
        fBeginJumpOffsetY = 0.0f;
    }
    else
    {
        hr = GameWorldPositionToScenePosition(p3DScene, vGWPosition, 
            (float)pBeginJumpPositionData->nX, 
            (float)pBeginJumpPositionData->nY, 
            (float)pBeginJumpPositionData->nZ, 
            FALSE, pBeginJumpPositionData->bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        fBeginJumpOffsetY = fBeginY - vGWPosition.y;
    }

    // End jump
    if (EndJumpPositionData.bWater)
    {
        fEndJumpOffsetY = 0.0f;
    }
    else
    {
        hr = GameWorldPositionToScenePosition(p3DScene, v3DPosition, 
            (float)EndJumpPositionData.nX, 
            (float)EndJumpPositionData.nY, 
            (float)EndJumpPositionData.nZ, 
            TRUE, EndJumpPositionData.bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = GameWorldPositionToScenePosition(p3DScene, vGWPosition, 
            (float)EndJumpPositionData.nX, 
            (float)EndJumpPositionData.nY, 
            (float)EndJumpPositionData.nZ, 
            FALSE, EndJumpPositionData.bSlip, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        fEndJumpOffsetY = v3DPosition.y - vGWPosition.y;
    }

    // Fetch result
    KG_ASSERT_EXIT(nGameLoop > 0);

    *pfBeginOffsetY = fBeginJumpOffsetY;
    *pfEndOffsetY = fEndJumpOffsetY;
    *pfDeltaY = (fEndJumpOffsetY - fBeginJumpOffsetY) / ((float)nGameLoop * (1000.0f / GAME_FPS));

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pfBeginOffsetY)
            *pfBeginOffsetY = 0.0f;
        if (pfDeltaY)
            *pfDeltaY = 0.0f;
    }
    return nResult;
}

int AdjustPosition(KCharacter* pCharacter, D3DXVECTOR3 vPosition, float fLengthEpsilon, BOOL* pbAdjust)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;
    D3DXVECTOR3 vGWPosition;
    float fLength = 0.0f;

    KG_ASSERT_EXIT(pbAdjust);
    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pRepresentObject);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KG_ASSERT_EXIT(pRLCharacter);

    pRLScene = pRLCharacter->m_RenderData.GetRLScene();
    KG_ASSERT_EXIT(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KG_ASSERT_EXIT(p3DScene);

    hr = GameWorldPositionToScenePosition(p3DScene, vGWPosition, 
        (float)pCharacter->m_nX, 
        (float)pCharacter->m_nY, 
        (float)pCharacter->m_nZ, 
        FALSE, pCharacter->m_bSlip, TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    fLength = D3DXVec3Length(&(vPosition - vGWPosition));
    if (fLength > fLengthEpsilon)
        *pbAdjust = TRUE;
    else
        *pbAdjust = FALSE;

    nResult = true;
Exit0:
    return nResult;
}

KRLCharacterFrameData::KRLCharacterFrameData()
: m_pCharacter(NULL)
#ifdef ENABLE_RL_SWIM_Y_ADJUST
, m_fWaterDelta(0.0f)
#endif
{
	memset(&m_Current, 0, sizeof(m_Current));
	memset(&m_Previous, 0, sizeof(m_Previous));
    memset(&m_Interpolated, 0, sizeof(m_Interpolated));
}

void KRLCharacterFrameData::Init(KCharacter* pCharacter)
{
	m_pCharacter = pCharacter;
}

BOOL KRLCharacterFrameData::IsLifeChanged() const
{
    return abs(m_Previous.fLifePercentage - m_Current.fLifePercentage) > FLT_EPSILON;
}

int KRLCharacterFrameData::UpdateCurrentRepresentFrameData(int nVisible, D3DXVECTOR3 vPosition)
{
    int nRetCode = false;
    int nResult = false;
	IKGSO3WorldClient* pSO3WorldClient = NULL;
    D3DXVECTOR3 vCharacterPosition;
    float fWaterHeight = 0.0f;
    float fFloorHeight = 0.0f;

    KG_PROCESS_SUCCESS(m_pCharacter == NULL);
    KG_PROCESS_SUCCESS(m_pCharacter->m_pCell == NULL);
 
	pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    // Common

    UpdateCurrentMoveState();
    UpdateCurrentOTAction();
    UpdateCurrentFightFlag();

    m_Current.nDirectionXY = m_pCharacter->m_nDirectionXY;
    m_Current.nFaceDirection = m_pCharacter->m_nFaceDirection;
    m_Current.nX = m_pCharacter->m_nX;
    m_Current.nY = m_pCharacter->m_nY;
    m_Current.nZ = m_pCharacter->m_nZ;
    m_Current.nJumpCount = m_pCharacter->m_nJumpCount;
    m_Current.fLifePercentage = max(0.0f, min(1.0f, (float)m_pCharacter->m_nCurrentLife / (float)m_pCharacter->m_nMaxLife));
    m_Current.nVelocityXY = pSO3WorldClient->GetSubjectiveVelocity(m_pCharacter);
    m_Current.bSlip = m_pCharacter->m_bSlip;
    m_Current.fFaceDirection = YawRHToLH(ToSceneAngle(m_pCharacter->m_nFaceDirection));
    m_Current.fSubjectiveDirection = YawRHToLH(ToSceneAngle(pSO3WorldClient->GetSubjectiveDirection(m_pCharacter)));

    if (nVisible)
    {
        nRetCode = GetWaterHeight(m_pCharacter, vPosition, fWaterHeight);
        KG_CHECK_ERROR(nRetCode);

        nRetCode = GetFloorHeight(m_pCharacter, vPosition, fFloorHeight);
        KG_CHECK_ERROR(nRetCode);

        m_Current.bAboveWater = AboveWater(fWaterHeight, fFloorHeight, vPosition.y);
    }
    else
    {
        m_Current.bAboveWater = FALSE;
    }

    if (m_pCharacter->m_eMoveState == cmsOnAutoFly)
        m_Current.nVehicleTrack = m_pCharacter->m_nCurrentTrack;
    else
        m_Current.nVehicleTrack = 0;

    nRetCode = GetSkillBuffData(m_pCharacter, m_Current.dwSkillBuffs);
    KGLOG_CHECK_ERROR(nRetCode);

    // Player
    if (IS_PLAYER(m_pCharacter->m_dwID))
    {
        int nWaterLine = 0;
        KPlayer* pPlayer = (KPlayer*)m_pCharacter;

        nWaterLine = pSO3WorldClient->GetWaterline(m_pCharacter);

        m_Current.nSchoolID = (RL_SCHOOL)pPlayer->m_dwSchoolID;
        m_Current.bOnRide = pPlayer->m_bOnHorse && pPlayer->m_dwModelID == 0;
        m_Current.nTurn = pPlayer->m_MoveCtrl.nTurn;
        m_Current.bUnderWater = IS_UNDER_WATER(nWaterLine, m_pCharacter->m_nHeight);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterFrameData::UpdateRepresentFrameData(int nVisible, D3DXVECTOR3 vPosition)
{
    int nRetCode = false;
    int nResult = false;

	m_Previous = m_Current;

    nRetCode = UpdateCurrentRepresentFrameData(nVisible, vPosition);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacterFrameData::UpdateSkillBuff()
{
    if (m_pCharacter)
    {
        if (m_pCharacter->m_bFightState)
		    m_Current.nFightFlag = RL_FIGHT_FLAG_FIGHT;
        else
            m_Current.nFightFlag = RL_FIGHT_FLAG_NONE;

        GetSkillBuffData(m_pCharacter, m_Current.dwSkillBuffs);
    }
}

void KRLCharacterFrameData::UpdateCurrentMoveState()
{
    if (m_pCharacter)
    {
        m_Current.nMoveState = m_pCharacter->m_eMoveState;
    }
}

void KRLCharacterFrameData::UpdateCurrentOTAction()
{
    if (m_pCharacter)
    {
        m_Current.nOTAction = m_pCharacter->m_OTActionParam.eActionType;
    }
}

void KRLCharacterFrameData::UpdateCurrentFightFlag()
{
    if (m_pCharacter)
    {
        if (m_pCharacter->m_bFightState)
            m_Current.nFightFlag = RL_FIGHT_FLAG_FIGHT;
        else
            m_Current.nFightFlag = RL_FIGHT_FLAG_NONE;

        m_Current.bSheathFlag = m_pCharacter->m_bSheathFlag;
    }
}

void KRLCharacterFrameData::UpdateCurrentDirection()
{
    IKGSO3WorldClient* pSO3WorldClient = g_pRL->m_pSO3WorldClient;

    if (m_pCharacter && pSO3WorldClient)
    {
        m_Current.nVelocityXY = pSO3WorldClient->GetSubjectiveVelocity(m_pCharacter);
        m_Current.fFaceDirection = YawRHToLH(ToSceneAngle(m_pCharacter->m_nFaceDirection));
        m_Current.fSubjectiveDirection = YawRHToLH(ToSceneAngle(pSO3WorldClient->GetSubjectiveDirection(m_pCharacter)));
	}
}

int KRLCharacterFrameData::AboveWater(float fWaterHeight, float fFloorHeight, float fCharacterY) const
{
    int nResult = false;

    if (fWaterHeight > fFloorHeight)
    {
        float fHeight = 0.0f;
        float fDelta = 0.0f;

        // 身高的 7 / 10 认为入水，水上漂的条件是在水面上，并且没有入水，水面一个身高内

        KG_PROCESS_ERROR(m_pCharacter);

        fHeight = (float)m_pCharacter->m_nHeight * ((float)LOGICAL_CELL_CM_LENGTH / CELL_LENGTH / 8.0f);
        fDelta = fCharacterY - fWaterHeight;

        if (fDelta > fHeight * -0.7f && fDelta < fHeight)
        {
            nResult = true;
        }
    }

Exit0:
    return nResult;
}

