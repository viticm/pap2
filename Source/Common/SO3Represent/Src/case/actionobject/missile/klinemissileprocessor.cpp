#include "stdafx.h"
#include "./klinemissileprocessor.h"
#include "../krlmissile.h"
#include "../../../common/kmathtools.h"
#include "../../../SO3Represent.h"
#include "SO3World/IKGSO3WorldClient.h"

#if 0
KLineMissileProcessor::KLineMissileProcessor()
: m_fBeginTerrainOffset(0.0f)
, m_fBeginLineHeight(0.0f)
, m_fBeginLineWidth(0.0f)
, m_fEndTerrainOffset(0.0f)
, m_fEndLineHeight(0.0f)
, m_fEndLineWidth(0.0f)
, m_fMeteorSpeed(0.0f)
, m_fMeteorRadius(0.0f)
, m_fTotalDistance(0.0f)
, m_fParabolaAngle(0.0f)
, m_fRotationFrame(0.0f)
, m_fTotalModleScale(0.0f)
{
}

KLineMissileProcessor::~KLineMissileProcessor()
{
}

int KLineMissileProcessor::Init()
{
	return true;
}

int KLineMissileProcessor::UnInit()
{
	return true;
}

int KLineMissileProcessor::Update(double fTime, double fTimeLast, KRLMissile* pRLMissile)
{
	HRESULT hr = E_FAIL;
	int nRetCode = false;
	int nSFXModelCount = 0;
	double fTimeDelta = 0.0;
	double fTimeTotal = 0.0;
    KMissilePhaseLineModel* pLineModel = NULL;

	ASSERT(m_pRLMissile);
	ASSERT(m_pMissilePhaseModel);

    pLineModel = (KMissilePhaseLineModel*)m_pMissilePhaseModel;

	nSFXModelCount = GetSFXModelCount();

	fTimeTotal = GetTimeTotal();

	fTimeDelta = fTime - GetTimeStart();
	if (fTimeDelta <= fTimeTotal)
	{
		nRetCode = abs(fTimeTotal) > FLT_EPSILON;
		KGLOG_PROCESS_ERROR(nRetCode);

		for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
		{
			D3DXVECTOR3 vPosition(0.0f, 0.0f, 0.0f);
            D3DXVECTOR3 vLocalPosition(0.0f, 0.0f, 0.0f);
			D3DXQUATERNION qRotationLH;
            KLineMissile& Missile = m_aLineMissile[nSFXModel];            

			float fTotalLineWidth = 0.0f;
			float fTotalLineHeight = 0.0f;
			float fTotalTerrainOffset = 0.0f;
            float fModelScale = 0.0f;
            float fMovementDelta = 0.0f;
            float fAccelerationPercent = 0.0f;

            hr = OnAccelerationPercent(pLineModel->nAcceleration, static_cast<float>(fTimeDelta), static_cast<float>(fTimeTotal), fAccelerationPercent);
            KGLOG_COM_PROCESS_ERROR(hr);

            hr = OnMovementTypeCorrect(m_pMissilePhaseModel->nMovementType, static_cast<float>(fAccelerationPercent), m_fTotalDistance, m_fParabolaAngle, fMovementDelta);
            KGLOG_COM_PROCESS_ERROR(hr);

            hr = OnMovementOneSelf(m_fMeteorSpeed, m_fMeteorRadius, static_cast<float>(fTimeDelta), vLocalPosition);
            KGLOG_COM_PROCESS_ERROR(hr);

            fTotalLineWidth = Missile.fEndLineWidth - Missile.fBeginLineWidth;
            fTotalLineHeight = m_fEndLineHeight - m_fBeginLineHeight;
            fTotalTerrainOffset = m_fEndTerrainOffset - m_fBeginTerrainOffset;
            fModelScale = static_cast<float>(m_fTotalModleScale * fAccelerationPercent + m_pMissilePhaseModel->fModelScaleBegin);

            if (m_pMissilePhaseModel->nMissileDirection == MISSILE_DIRECTION_HORIZONTAL)
            {
                vPosition.x = static_cast<float>(Missile.fBeginLineWidth + fTotalLineWidth * fAccelerationPercent);
                vPosition.z = static_cast<float>(m_fBeginLineHeight + fTotalLineHeight * fAccelerationPercent + vLocalPosition.z);
                vPosition.y = static_cast<float>(m_fBeginTerrainOffset + fTotalTerrainOffset * fAccelerationPercent + fMovementDelta + vLocalPosition.y);
            }
            else
            {
                vPosition.x = Missile.fBeginLineWidth + fMovementDelta + vLocalPosition.y;
                vPosition.z = static_cast<float>(m_fBeginLineHeight + fTotalLineHeight * fAccelerationPercent + vLocalPosition.z);
                vPosition.y = static_cast<float>(m_fBeginTerrainOffset + (fTotalTerrainOffset + fTotalLineWidth) * fAccelerationPercent);
            }

            Missile.fAngle += m_fRotationFrame;
			YawToQuatLH(qRotationLH, Missile.fAngle);

            for (int nModelType = 0; nModelType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nModelType)
            {
                hr = SetSFXModelOrientation(nModelType, nSFXModel, fModelScale, vLocalPosition, vPosition, qRotationLH);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
            Missile.vPosition = vLocalPosition;
		}
	}

	nRetCode = KTimelineMissileProcessor::OnUpdate(fTime, fTimeLast);
	KGLOG_PROCESS_ERROR(nRetCode);

	return true;
Exit0:
	return false;
}

int KLineMissileProcessor::IsFinished() const
{
	return false;
}

int KLineMissileProcessor::Start(double fTime, KRLMissile* pRLMissile)
{
	int nRetCode = false;
    int nResult = false;
	KSkill const* pSkill = NULL;
    KMissilePhaseLineModel* pLineModel;

	float fBulletVelocity = 0.0f;

	ASSERT(m_pRLMissile);
	ASSERT(m_pMissilePhaseModel);

    pLineModel = (KMissilePhaseLineModel*)m_pMissilePhaseModel;

    pSkill = g_pRL->m_pSO3WorldClient->GetSkill(m_pRLMissile->m_Param.Result.dwSkillID, m_pRLMissile->m_Param.Result.dwSkillLevel);
	KGLOG_PROCESS_ERROR(pSkill);

    // Begin
	m_fBeginTerrainOffset = ToSceneLength(pLineModel->nTerrainOffsetBegin);
	m_fBeginLineHeight = ToSceneLength(pLineModel->nLineHeightBegin);
	m_fBeginLineWidth = ToSceneLength(pLineModel->nLineWidthBegin);

    // End
	m_fEndTerrainOffset = ToSceneLength(pLineModel->nTerrainOffsetEnd);
	m_fEndLineHeight = ToSceneLength(pLineModel->nLineHeightEnd);
	m_fEndLineWidth = ToSceneLength(pLineModel->nLineWidthEnd);

    // Velocity
	fBulletVelocity = ToSceneVelocity(pSkill->m_nBulletVelocity);

    nRetCode = abs(fBulletVelocity) > FLT_EPSILON;
    KG_PROCESS_SUCCESS(!nRetCode);

    // Track Parameter
    m_fMeteorSpeed = fBulletVelocity * pLineModel->fSpeedScale;
    m_fMeteorRadius = ToSceneLength(pLineModel->nMeteorRadius);
    m_fRotationFrame = pLineModel->fRotationFrame;
    if (abs(m_fRotationFrame) > FLT_EPSILON)
        m_fRotationFrame = D3DX_PI / m_fRotationFrame;
    m_fTotalModleScale = m_pMissilePhaseModel->fModelScaleEnd - m_pMissilePhaseModel->fModelScaleBegin;
    m_fParabolaAngle = ToSceneAngle(pLineModel->nParabolaAngle);

    m_fTotalDistance = m_fEndLineHeight - m_fBeginLineHeight;

	nRetCode = InitLineMissileOrientationData();
	KGLOG_PROCESS_ERROR(nRetCode);

    if (m_fTotalDistance <= FLT_EPSILON)
        SetTimeTotal(pLineModel->fPervasionTime);
    else
        SetTimeTotal(m_fTotalDistance / fBulletVelocity);

	nRetCode = KTimelineMissileProcessor::OnStart(fTime);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KLineMissileProcessor::InitLineMissileOrientationData()
{
	int nRetCode = false;
	int nSFXModelCount = 0;
	float fBeginDeltaLineWidth = 0.0f;
	float fEndDeltaLineWidth = 0.0f;
	float fHalfArcTangent = 0.0f;
	float fDeltaArcTangent = 0.0f;

	ASSERT(m_pRLMissile);
	ASSERT(m_pMissilePhaseModel);

	nSFXModelCount = GetSFXModelCount();

	fBeginDeltaLineWidth = m_fBeginLineWidth;
	fEndDeltaLineWidth = m_fEndLineWidth;
	fHalfArcTangent = atan2f((m_fEndLineWidth - m_fBeginLineWidth) * 0.5f, m_fEndLineHeight - m_fBeginLineHeight);
	fDeltaArcTangent = fHalfArcTangent * 2.0f;

    if (nSFXModelCount >= 2)
    {
        fBeginDeltaLineWidth /= static_cast<float>(nSFXModelCount - 1);
        fEndDeltaLineWidth /= static_cast<float>(nSFXModelCount - 1);
        fDeltaArcTangent /= static_cast<float>(nSFXModelCount - 1);
    }

    for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
    {
        m_aLineMissile[nSFXModel].fBeginLineWidth = m_fBeginLineWidth / 2.0f - fBeginDeltaLineWidth * static_cast<float>(nSFXModel);
        m_aLineMissile[nSFXModel].fEndLineWidth = m_fEndLineWidth / 2.0f - fEndDeltaLineWidth * static_cast<float>(nSFXModel);
        m_aLineMissile[nSFXModel].fAngle = YawLHToModelLH(fHalfArcTangent - fDeltaArcTangent * static_cast<float>(nSFXModel));
    }

	return true;
}

HRESULT KLineMissileProcessor::OnMovementTypeCorrect(int nMovementType, float fRatio, float fDistance, float fAngle, float& fMoveDistance)
{
    switch (nMovementType)
    {
    case MOVEMENT_TYPE_STRAIGHT:
        fMoveDistance = 0.0f;
        break;
    case MOVEMENT_TYPE_PARABOLA:
        fMoveDistance = fDistance * (1.0f - cosf(fAngle)) / (2.0f * sinf(fAngle));
        if (fRatio >= 0.5f)
            fRatio = 1.0f - fRatio;
        fMoveDistance *= fRatio;
        break;
    case MOVEMENT_TYPE_RANDOM:
        fMoveDistance = 0.0f;
        // TO DO :
        break;
    }
    return S_OK;
}

HRESULT KLineMissileProcessor::OnAccelerationPercent(int nAcceleration, float fTimeDelta, float fTimeTotal, float& fPercent)
{
    switch(nAcceleration)
    {
    case -1:
        fPercent = fTimeDelta * (2.0f * fTimeTotal - fTimeDelta) / (fTimeTotal * fTimeTotal);
        break;
    case 0:
        fPercent = fTimeDelta / fTimeTotal;
        break;
    case 1:
        fPercent = (fTimeDelta * fTimeDelta) / (fTimeTotal * fTimeTotal);
        break;
    }
    return S_OK;
}

HRESULT KLineMissileProcessor::OnMovementOneSelf(float fSpeed, float fRadius, float fTime, D3DXVECTOR3& vPosition)
{
    float fAngleXY = 0.0f;

    if (fRadius <= FLT_EPSILON || fSpeed <= FLT_EPSILON)
        return S_OK;

    fAngleXY = fSpeed * fTime / fRadius;
    while (fAngleXY >= 2.0f * D3DX_PI)
        fAngleXY -= 2.0f * D3DX_PI;

    vPosition.y = fRadius * sinf(fAngleXY);
    vPosition.z = fRadius * cosf(fAngleXY);

    if (abs(vPosition.y) <= FLT_EPSILON)
        vPosition.y = 0.0f;
    if (abs(vPosition.z) <= FLT_EPSILON)
        vPosition.z = 0.0f;

    return S_OK;
}

#endif