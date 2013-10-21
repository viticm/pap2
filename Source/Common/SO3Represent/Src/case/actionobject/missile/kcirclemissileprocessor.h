#ifndef CASE_ACTION_OBJECT_MISSILE_K_CIRCLE_MISSILE_PROCESSOR_H
#define CASE_ACTION_OBJECT_MISSILE_K_CIRCLE_MISSILE_PROCESSOR_H

struct KRLMissile;

#if 0
class KCircleMissileProcessor
{
private:
	struct KCircleMissileParam
	{
		float	fCircleRadius;
		float	fFOV;
		float	fFOVAngleVelocity;
		float	fTerrainOffset;
        float   fLineHeight;
	};

	struct KCircleMissile
	{
		KCircleMissileParam	BeginParam;
		KCircleMissileParam	EndParam;
        D3DXVECTOR3		vPosition;
        D3DXQUATERNION  qRotation;
	};

public:
	KCircleMissileProcessor();

protected:
	~KCircleMissileProcessor();

	int Init(DWORD dwMissileParamID);
	int UnInit();
	int Update(double fTime, double fTimeLast, KRLMissile* pRLMissile);
	int IsFinished() const;
	int Start(double fTime, KRLMissile* pRLMissile);

private:
	int InitCircleMissileOrientationData(KCircleMissileParam const& BeginParam, KCircleMissileParam const& EndParam);

    HRESULT OnMovementOneSelf(float fSpeed, float fRadius, float fTime, D3DXVECTOR3& vPosition);
    HRESULT OnMovementTypeCorrect(int nMovementType, float fRatio, float fDistance, float fAngle, float& fMoveDistance);
    HRESULT OnAccelerationPercent(int nAcceleration, float fTimeDelta, float fTimeTotal, float& fPercent);

private:
    float m_fMeteorSpeed;
    float m_fMeteorRadius;
    float m_fTotalDistance;
    float m_fParabolaAngle;
    float m_fTotalModelScale;

	KCircleMissile m_aCircleMissile[MISSILE_PHASE_SFX_COUNT];
};

#endif

#endif // CASE_ACTION_OBJECT_MISSILE_K_CIRCLE_MISSILE_PROCESSOR_H