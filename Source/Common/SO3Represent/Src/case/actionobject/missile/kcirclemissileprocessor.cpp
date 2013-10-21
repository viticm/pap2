#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./kcirclemissileprocessor.h"
#include "../krlmissile.h"
#include "../../../common/kmathtools.h"
#include "../../../SO3Represent.h"

#if 0
KCircleMissileProcessor::KCircleMissileProcessor()
: m_fMeteorSpeed(0.0f)
, m_fMeteorRadius(0.0f)
, m_fTotalDistance(0.0f)
, m_fParabolaAngle(0.0f)
, m_fTotalModelScale(0.0f)
{
}

KCircleMissileProcessor::~KCircleMissileProcessor()
{
}

int KCircleMissileProcessor::Update(double fTime, double fTimeLast, KRLMissile* pRLMissile)
{
	HRESULT hr = E_FAIL;
	int nRetCode = false;
	int nSFXModelCount = 0;
	double fTimeDelta = 0.0f;
	double fTimeTotal = 0.0f;
    KMissilePhaseCircleModel* pCircleModel = NULL;

	ASSERT(m_pRLMissile);
	ASSERT(m_pMissilePhaseModel);

    pCircleModel = (KMissilePhaseCircleModel*)m_pMissilePhaseModel;
	nSFXModelCount = GetSFXModelCount();

	fTimeTotal = GetTimeTotal();

	fTimeDelta = fTime - GetTimeStart();
	if (fTimeDelta <= fTimeTotal)
	{
		nRetCode = abs(fTimeTotal) > FLT_EPSILON;
		KGLOG_PROCESS_ERROR(nRetCode);

		for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
		{
            float fDistance = 0.0f;
            float fDistanceReciprocal = 1.0f;
            float fMovementDelta = 0.0f;
            float fModelScale = 0.0f;
            float fAccelerationPercent = 0.0f;

			D3DXVECTOR3 vPosition(0.0f, 0.0f, 0.0f);
            D3DXVECTOR3 vLocalPosition(0.0f, 0.0f, 0.0f);
			D3DXQUATERNION qRotationLH;
            
            KCircleMissile& Missile = m_aCircleMissile[nSFXModel];
			KCircleMissileParam& BeginParam = Missile.BeginParam;
			KCircleMissileParam& EndParam = Missile.EndParam;
			KCircleMissileParam	CurrentParam;
			KCircleMissileParam	TotalParam;

            hr = OnAccelerationPercent(pCircleModel->nAcceleration, static_cast<float>(fTimeDelta), static_cast<float>(fTimeTotal), fAccelerationPercent);
            KGLOG_COM_PROCESS_ERROR(hr);

            hr = OnMovementTypeCorrect(pCircleModel->nMovementType, static_cast<float>(fAccelerationPercent), m_fTotalDistance, m_fParabolaAngle, fMovementDelta);;
            KGLOG_COM_PROCESS_ERROR(hr);

            hr = OnMovementOneSelf(m_fMeteorSpeed, m_fMeteorRadius, static_cast<float>(fTimeDelta), vLocalPosition);
            KGLOG_COM_PROCESS_ERROR(hr);

            fModelScale = static_cast<float>(m_fTotalModelScale * (fAccelerationPercent) + pCircleModel->fModelScaleBegin);

            TotalParam.fCircleRadius = EndParam.fCircleRadius - BeginParam.fCircleRadius;
            TotalParam.fFOV = EndParam.fFOV - BeginParam.fFOV;
            TotalParam.fFOVAngleVelocity = EndParam.fFOVAngleVelocity - BeginParam.fFOVAngleVelocity;
            TotalParam.fTerrainOffset = EndParam.fTerrainOffset - BeginParam.fTerrainOffset;
            TotalParam.fLineHeight = EndParam.fLineHeight - BeginParam.fLineHeight;

            CurrentParam.fCircleRadius = static_cast<float>(BeginParam.fCircleRadius + TotalParam.fCircleRadius * fAccelerationPercent);
            CurrentParam.fLineHeight = static_cast<float>(BeginParam.fLineHeight + TotalParam.fLineHeight * fAccelerationPercent);
            CurrentParam.fTerrainOffset = static_cast<float>(BeginParam.fTerrainOffset + TotalParam.fTerrainOffset * fAccelerationPercent);
            CurrentParam.fFOVAngleVelocity = static_cast<float>(BeginParam.fFOVAngleVelocity + TotalParam.fFOVAngleVelocity * fAccelerationPercent);

			if (abs(CurrentParam.fFOVAngleVelocity) <= FLT_EPSILON)
			{
				CurrentParam.fFOV = static_cast<float>(BeginParam.fFOV + TotalParam.fFOV * fAccelerationPercent);
			}
			else
			{
				BeginParam.fFOV += CurrentParam.fFOVAngleVelocity;
				CurrentParam.fFOV = BeginParam.fFOV;
			}

            if (pCircleModel->nMissileDirection == MISSILE_DIRECTION_HORIZONTAL)
            {
                vPosition.x = CurrentParam.fCircleRadius * cosf(CurrentParam.fFOV);
                vPosition.z = CurrentParam.fCircleRadius * sinf(CurrentParam.fFOV) + CurrentParam.fLineHeight + vLocalPosition.z;
                vPosition.y = CurrentParam.fTerrainOffset + vLocalPosition.y;

                fDistance = sqrtf(vPosition.x * vPosition.x + vPosition.z * vPosition.z);

                if (abs(fDistance) > FLT_EPSILON)
                    fDistanceReciprocal = 1.0f / fDistance;

                vPosition.x *= (fMovementDelta + fDistance) * fDistanceReciprocal;
                vPosition.z *= (fMovementDelta + fDistance) * fDistanceReciprocal;
            }
            else
            {
                vPosition.x = CurrentParam.fCircleRadius * cosf(CurrentParam.fFOV) + vLocalPosition.y;
                vPosition.z = CurrentParam.fLineHeight + vLocalPosition.z;
                vPosition.y = CurrentParam.fCircleRadius * sinf(CurrentParam.fFOV) + CurrentParam.fTerrainOffset;

                fDistance = sqrtf(vPosition.x * vPosition.x + vPosition.y * vPosition.y);

                if (abs(fDistance) > FLT_EPSILON)
                    fDistanceReciprocal = 1.0f / fDistance;

                vPosition.x *= (fMovementDelta + fDistance) * fDistanceReciprocal;
                vPosition.y *= (fMovementDelta + fDistance) * fDistanceReciprocal;
            }			

			YawToQuatLH(qRotationLH, CurrentParam.fFOV);

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

int KCircleMissileProcessor::IsFinished() const
{
	if (KTimelineMissileProcessor::OnIsFinished())
		return true;

	return false;
}

int KCircleMissileProcessor::Start(double fTime, KRLMissile* pRLMissile)
{
	int nRetCode = false;
    int nResult = false;
	KSkill const* pSkill = NULL;
    KMissilePhaseCircleModel* pCircleModel = NULL;
	KCircleMissileParam BeginParam;
	KCircleMissileParam EndParam;

	float fBulletVelocity = 0.0f;

    ASSERT(m_pRLMissile);
    ASSERT(m_pMissilePhaseModel);

    pCircleModel = (KMissilePhaseCircleModel*)m_pMissilePhaseModel;

    pSkill = g_pRL->m_pSO3WorldClient->GetSkill(m_pRLMissile->m_Param.Result.dwSkillID, m_pRLMissile->m_Param.Result.dwSkillLevel);
	KGLOG_PROCESS_ERROR(pSkill);

	// begin
    BeginParam.fLineHeight = ToSceneLength(pCircleModel->nLineHeightBegin);
	BeginParam.fCircleRadius = ToSceneLength(pCircleModel->nCircleRadiusBegin);
	BeginParam.fTerrainOffset = ToSceneLength(pCircleModel->nTerrainOffsetBegin);
	BeginParam.fFOV = ToSceneAngle(pCircleModel->nFOVBegin);
	BeginParam.fFOVAngleVelocity = ToSceneAngleVelocity(pCircleModel->nFOVAngleVelocityBegin);

	// end
    EndParam.fLineHeight = ToSceneLength(pCircleModel->nLineHeightEnd);
	EndParam.fCircleRadius = ToSceneLength(pCircleModel->nCircleRadiusEnd);
	EndParam.fTerrainOffset = ToSceneLength(pCircleModel->nTerrainOffsetEnd);
	EndParam.fFOV = ToSceneAngle(pCircleModel->nFOVEnd);
	EndParam.fFOVAngleVelocity = ToSceneAngleVelocity(pCircleModel->nFOVAngleVelocityEnd);

	// Bullet
	fBulletVelocity = ToSceneVelocity(pSkill->m_nBulletVelocity);

    nRetCode = abs(fBulletVelocity) > FLT_EPSILON;
    KG_PROCESS_SUCCESS(!nRetCode);

    // Track Parameter
    m_fMeteorSpeed = fBulletVelocity * pCircleModel->fSpeedScale;
    m_fMeteorRadius = ToSceneLength(pCircleModel->nMeteorRadius);
    m_fTotalModelScale = pCircleModel->fModelScaleEnd - pCircleModel->fModelScaleBegin;
    m_fParabolaAngle = ToSceneAngle(pCircleModel->nParabolaAngle);

	m_fTotalDistance = EndParam.fLineHeight - BeginParam.fLineHeight;

	nRetCode = InitCircleMissileOrientationData(BeginParam, EndParam);
	KGLOG_PROCESS_ERROR(nRetCode);

    if (m_fTotalDistance <= FLT_EPSILON)
        SetTimeTotal(pCircleModel->fPervasionTime);
    else
        SetTimeTotal(m_fTotalDistance / fBulletVelocity);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}
int KCircleMissileProcessor::Init()
{
	return true;
}

int KCircleMissileProcessor::UnInit()
{
	return true;
}

int KCircleMissileProcessor::InitCircleMissileOrientationData(KCircleMissileParam const& BeginParam, KCircleMissileParam const& EndParam)
{
	int nSymmetricalType = SYMMETRICAL_TYPE_ODD;
	int nSFXModelCount = 0;
	float fTotalFOVBegin = 0.0f;
	float fTotalFOVEnd = 0.0f;
	float fDeltaFOVBegin = 0.0f;
	float fDeltaFOVEnd = 0.0f;

	ASSERT(m_pRLMissile);
	ASSERT(m_pMissilePhaseModel);

	nSymmetricalType = m_pMissilePhaseModel->nSymmetricalType;

	nSFXModelCount = GetSFXModelCount();

	fTotalFOVBegin = BeginParam.fFOV;
	fTotalFOVEnd = EndParam.fFOV;
	fDeltaFOVBegin = fTotalFOVBegin / static_cast<float>(nSFXModelCount);
	fDeltaFOVEnd = fTotalFOVEnd / static_cast<float>(nSFXModelCount);

	if (nSymmetricalType == SYMMETRICAL_TYPE_ODD)
	{
		for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
		{
			KCircleMissileParam& SFXBeginParam = m_aCircleMissile[nSFXModel].BeginParam;
			KCircleMissileParam& SFXEndParam = m_aCircleMissile[nSFXModel].EndParam;

			SFXBeginParam.fCircleRadius = BeginParam.fCircleRadius;
			SFXBeginParam.fFOV = -fTotalFOVBegin / 2.0f + fDeltaFOVBegin * static_cast<float>(nSFXModel);
			SFXBeginParam.fFOVAngleVelocity = BeginParam.fFOVAngleVelocity;
			SFXBeginParam.fTerrainOffset = BeginParam.fTerrainOffset;
            SFXBeginParam.fLineHeight = BeginParam.fLineHeight;

			SFXEndParam.fCircleRadius = EndParam.fCircleRadius;
			SFXEndParam.fFOV = -fTotalFOVEnd / 2.0f + fDeltaFOVEnd * static_cast<float>(nSFXModel);
			SFXEndParam.fFOVAngleVelocity = EndParam.fFOVAngleVelocity;
			SFXEndParam.fTerrainOffset = EndParam.fTerrainOffset;
            SFXEndParam.fLineHeight = EndParam.fLineHeight;
		}
	}
	else
	{
		int nHalfSFXModelCount = nSFXModelCount / 2;

		for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
		{
			KCircleMissileParam& SFXBeginParam = m_aCircleMissile[nSFXModel].BeginParam;
			KCircleMissileParam& SFXEndParam = m_aCircleMissile[nSFXModel].EndParam;

			SFXBeginParam.fCircleRadius = BeginParam.fCircleRadius;
			SFXBeginParam.fFOV = fDeltaFOVBegin * static_cast<float>(nSFXModel - nHalfSFXModelCount);
			SFXBeginParam.fFOVAngleVelocity = BeginParam.fFOVAngleVelocity;
			SFXBeginParam.fTerrainOffset = BeginParam.fTerrainOffset;
            SFXBeginParam.fLineHeight = BeginParam.fLineHeight;

			SFXEndParam.fCircleRadius = EndParam.fCircleRadius;
			SFXEndParam.fFOV = fDeltaFOVEnd * static_cast<float>(nSFXModel - nHalfSFXModelCount);
			SFXEndParam.fFOVAngleVelocity = EndParam.fFOVAngleVelocity;
			SFXEndParam.fTerrainOffset = EndParam.fTerrainOffset;
            SFXEndParam.fLineHeight = EndParam.fLineHeight;
		}
	}

	return true;
}

HRESULT KCircleMissileProcessor::OnMovementTypeCorrect(int nMovementType, float fRatio, float fDistance, float fAngle, float& fMoveDistance)
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

HRESULT KCircleMissileProcessor::OnAccelerationPercent(int nAcceleration, float fTimeDelta, float fTimeTotal, float& fPercent)
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

HRESULT KCircleMissileProcessor::OnMovementOneSelf(float fSpeed, float fRadius, float fTime, D3DXVECTOR3& vPosition)
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