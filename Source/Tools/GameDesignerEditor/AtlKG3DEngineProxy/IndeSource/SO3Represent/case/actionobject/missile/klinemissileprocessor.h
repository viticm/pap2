#ifndef CASE_ACTION_OBJECT_MISSILE_K_LINE_MISSILE_PROCESSOR_H
#define CASE_ACTION_OBJECT_MISSILE_K_LINE_MISSILE_PROCESSOR_H

#if 0
class KLineMissileProcessor
{
private:
	struct KLineMissile
	{
		float fBeginLineWidth;
		float fEndLineWidth;
		float fAngle;
        D3DXVECTOR3		vPosition;
        D3DXQUATERNION  qRotation;
	};

public:
	KLineMissileProcessor();

protected:
	~KLineMissileProcessor();

	int Init(DWORD dwMissileParamID);
	int UnInit();
	int Update(double fTime, double fTimeLast, KRLMissile* pRLMissile);
	int IsFinished() const;
	int Start(double fTime, KRLMissile* pRLMissile);

private:
	int InitLineMissileOrientationData();

    HRESULT OnMovementOneSelf(float fSpeed, float fRadius, float fTime, D3DXVECTOR3& vPosition);
    HRESULT OnMovementTypeCorrect(int nMovementType, float fRatio, float fDistance, float fAngle, float& fMoveDistance);
    HRESULT OnAccelerationPercent(int nAcceleration, float fTimeDelta, float fTimeTotal, float& fPercent);

private:
    float   m_fMeteorSpeed;
    float   m_fMeteorRadius;
    float   m_fTotalDistance;
    float   m_fParabolaAngle;
    float   m_fRotationFrame;
    float   m_fTotalModleScale;

	float	m_fBeginTerrainOffset;
	float	m_fBeginLineHeight;
	float	m_fBeginLineWidth;
	float	m_fEndTerrainOffset;
	float	m_fEndLineHeight;
	float	m_fEndLineWidth;

	KLineMissile	m_aLineMissile[MISSILE_PHASE_SFX_COUNT];
};

#endif

#endif // CASE_ACTION_OBJECT_MISSILE_K_LINE_MISSILE_PROCESSOR_H