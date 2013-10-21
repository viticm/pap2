#ifndef CASE_ACTIONOBJECT_MISSILE_KLIGHTNINGMISSILEPROCESSOR_H
#define CASE_ACTIONOBJECT_MISSILE_KLIGHTNINGMISSILEPROCESSOR_H

#if 0
class KLightningMissileProcessor
{
private:
	struct KLightningMissile
	{
		D3DXVECTOR3	vPositionCaster;
        D3DXVECTOR3	vPositionTarget;
	};

public:
	KLightningMissileProcessor();

protected:
	~KLightningMissileProcessor();

	int Init(DWORD dwMissileParamID);
	int UnInit();
	int Update(double fTime, double fTimeLast, KRLMissile* pRLMissile);
	int IsFinished() const;
	int Start(double fTime, KRLMissile* pRLMissile);

private:
	HRESULT GetTargetAndCasterPosition(KLightningMissile& Missile) const;

	BOOL CalcCasterOrientation(D3DXVECTOR3& vPosition, float& fAngle, KCaster const& Caster) const;
	BOOL CalcTargetOrientation(D3DXVECTOR3& vPosition, float& fAngle, KCaster const& Caster, KTargetData* pTarget) const;

private:
    float m_fTotalModleScale;

    KLightningMissile m_aFulminationMissile[MISSILE_PHASE_SFX_COUNT];
};

#endif

#endif // CASE_ACTIONOBJECT_MISSILE_KLIGHTNINGMISSILEPROCESSOR_H