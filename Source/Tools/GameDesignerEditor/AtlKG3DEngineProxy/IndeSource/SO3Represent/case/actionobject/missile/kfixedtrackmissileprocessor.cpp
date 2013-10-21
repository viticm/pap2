#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./kfixedtrackmissileprocessor.h"
#include "../krlmissile.h"
#include "../../gameworld/ktabletransform.h"
#include "../../../common/kmathtools.h"
#include "../../../SO3Represent.h"

#if 0
KFixedTrackMissileProcessor::KFixedTrackMissileProcessor()
: m_fTotalModelScale(0.0f)
{
}

KFixedTrackMissileProcessor::~KFixedTrackMissileProcessor()
{
}

int KFixedTrackMissileProcessor::Init()
{
    return true;
}

int KFixedTrackMissileProcessor::UnInit()
{
    return true;
}

int KFixedTrackMissileProcessor::Update(double fTime, double fTimeLast, KRLMissile* pRLMissile)
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    int nSFXModelCount = 0;
    double fTimeDelta = 0.0;
    double fTimeTotal = 0.0;

    ASSERT(m_pRLMissile);
    ASSERT(m_pMissilePhaseModel);

    nSFXModelCount = GetSFXModelCount();
    fTimeTotal = GetTimeTotal();
    fTimeDelta = fTime - GetTimeStart();

    if (fTimeDelta <= fTimeTotal)
    {
        nRetCode = abs(fTimeTotal) > FLT_EPSILON;
        KGLOG_PROCESS_ERROR(nRetCode);

        int nFrameIndex = static_cast<int>(fTimeDelta * GAME_FPS / 1000.0f) % MISSILE_FIXED_TRACK_FRAME_COUNT;

        for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
        {
            D3DXVECTOR3 vPosition(0.0f, 0.0f, 0.0f);
            D3DXVECTOR3 vPositionTarget(0.0f, 0.0f, 0.0f);
            D3DXVECTOR3 vLocalPosition(0.0f, 0.0f, 0.0f);
            D3DXQUATERNION qRotationTarget;
            D3DXQUATERNION qRotationLH;
            KFixedTrackMissile& Missile = m_aFixedTrackMissile[nSFXModel];
            float fModelScale = static_cast<float>(m_fTotalModelScale * (fTimeDelta / fTimeTotal) + m_pMissilePhaseModel->fModelScaleBegin);

            int nFrameX = static_cast<int>(m_aFixedTrackMissile[nSFXModel].vFramePosition[nFrameIndex].x);
            int nFrameY = static_cast<int>(m_aFixedTrackMissile[nSFXModel].vFramePosition[nFrameIndex].y);
            int nFrameZ = static_cast<int>(m_aFixedTrackMissile[nSFXModel].vFramePosition[nFrameIndex].z);

            hr = GetTargetOrientation(vPositionTarget, qRotationTarget);
            KGLOG_COM_PROCESS_ERROR(hr);

            GameWorldCellPointToScenePoint(vPosition, nFrameX, nFrameY, nFrameZ, FALSE);

            hr = MissileMoveAngle(vPosition - Missile.vLastMoveDistance, m_aFixedTrackMissile[nSFXModel].fAngle);
            KGLOG_COM_PROCESS_ERROR(hr);

            Missile.vLastMoveDistance = vPosition;
            YawToQuatLH(qRotationLH, m_aFixedTrackMissile[nSFXModel].fAngle);

            for (int nModelType = 0; nModelType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nModelType)
            {
                hr = SetSFXModelOrientation(nModelType, nSFXModel, fModelScale, vLocalPosition, vPosition, qRotationLH);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
            Missile.vPosition = vLocalPosition;
        }
    }

    return true;
Exit0:
    return false;
}

int KFixedTrackMissileProcessor::IsFinished() const
{
    return false;
}

int KFixedTrackMissileProcessor::Start(double fTime, KRLMissile* pRLMissile)
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    KSkill const* pSkill = NULL;

    float fDistance = 0.0f;
    float fBulletVelocity = 0.0f;

    D3DXVECTOR3 vDistance(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vPositionTarget(0.0f, 0.0f, 0.0f);
    D3DXQUATERNION qRotationTarget;
    D3DXQUATERNION qRotationCaster;

    ASSERT(m_pRLMissile);
    ASSERT(m_pMissilePhaseModel);

    pSkill = g_pRL->m_pSO3WorldClient->GetSkill(m_pRLMissile->m_Param.Result.dwSkillID, m_pRLMissile->m_Param.Result.dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkill);

    // Track Parameter
    m_fTotalModelScale = m_pMissilePhaseModel->fModelScaleEnd - m_pMissilePhaseModel->fModelScaleBegin;

    // Velocity
    fBulletVelocity = ToSceneVelocity(pSkill->m_nBulletVelocity);

    nRetCode = abs(fBulletVelocity) > FLT_EPSILON;
    KG_PROCESS_SUCCESS(!nRetCode);

    hr = GetTargetOrientation(vPositionTarget, qRotationTarget);
    KGLOG_COM_PROCESS_ERROR(hr);

    YawToQuatLH(qRotationCaster, m_fAngleCaster);

    vDistance = vPositionTarget - m_vPositionCaster;
    fDistance = D3DXVec3Length(&vDistance);
    KGLOG_PROCESS_ERROR(fDistance >= FLT_EPSILON);

    hr = PrepareFixedTrackMissileOrientationInitialData();
    KGLOG_COM_PROCESS_ERROR(hr);

    SetTimeTotal(fDistance / fBulletVelocity);

    nRetCode = KTimelineMissileProcessor::OnStart(fTime);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    return true;
Exit0:
    return false;
}

HRESULT KFixedTrackMissileProcessor::GetTargetOrientation(D3DXVECTOR3& vPosition, D3DXQUATERNION& qRotation) const
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    KCaster const* pCaster = NULL;
    KTargetData const* pTarget = NULL;
    D3DXVECTOR3 vPositionCaster(0.0f, 0.0f, 0.0f);
    float fAngleTarget = 0.0f;

    KGLOG_PROCESS_ERROR(m_pRLMissile);

    pCaster = &m_pRLMissile->m_Param.Result.Caster;
    KGLOG_PROCESS_ERROR(pCaster);

    pTarget = &m_pRLMissile->m_Param.Result.Target;
    KGLOG_PROCESS_ERROR(pTarget);

    nRetCode = CalcTargetOrientation(vPosition, fAngleTarget, *pCaster, const_cast<KTargetData*>(pTarget));
    KGLOG_PROCESS_ERROR(nRetCode);

    YawToQuatLH(qRotation, fAngleTarget);

    return S_OK;
Exit0:
    return hr;
}

HRESULT KFixedTrackMissileProcessor::MissileMoveAngle(D3DXVECTOR3 vPosition, float& fAngle)
{
    if (fabsf(vPosition.x) <= FLT_EPSILON && fabsf(vPosition.z) <= FLT_EPSILON)
        return S_OK;

    if (fabsf(vPosition.x) <= FLT_EPSILON)
    {
        if (vPosition.z >= FLT_EPSILON)
            fAngle = D3DX_PI;
        else
            fAngle = 0.0f;
    }
    else if (fabsf(vPosition.z) <= FLT_EPSILON)
    {
        if (vPosition.x >= FLT_EPSILON)
            fAngle = D3DX_PI / -2.0f;
        else
            fAngle = D3DX_PI / 2.0f;
    }
    else
    {
        fAngle = YawLHToModelLH(atanf(vPosition.z / vPosition.x));
        if (vPosition.z < FLT_EPSILON)
            fAngle = YawLHToModelLH(fAngle);
    }

    return S_OK;
}

HRESULT KFixedTrackMissileProcessor::PrepareFixedTrackMissileOrientationInitialData()
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    int nSFXModelCount = 0;
    KMissilePhaseFixedTrackModel* pFixeTrackModel = NULL;

    ASSERT(m_pRLMissile);
    ASSERT(m_pMissilePhaseModel);

    pFixeTrackModel = (KMissilePhaseFixedTrackModel*)m_pMissilePhaseModel;

    nSFXModelCount = GetSFXModelCount();

    for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
    {
        m_aFixedTrackMissile[nSFXModel].fAngle = m_fAngleCaster;
        if (pFixeTrackModel->nFixedTrackID[nSFXModel] > 0)
        {
            hr = LoadFixedTrackMissileFrameFile(pFixeTrackModel->nFixedTrackID[nSFXModel], &m_aFixedTrackMissile[nSFXModel].vFramePosition[0]);
            KGLOG_COM_PROCESS_ERROR(hr);
        }            
    }

    return S_OK;
Exit0:
    return hr;
}

#endif