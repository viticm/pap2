#ifndef CASE_ACTION_OBJECT_MISSILE_K_FIXED_TRACK_MISSILE_PROCESSOR_H
#define CASE_ACTION_OBJECT_MISSILE_K_FIXED_TRACK_MISSILE_PROCESSOR_H

#if 0
class KFixedTrackMissileProcessor
{
private:
	struct KFixedTrackMissile
	{
        float fAngle;
        D3DXVECTOR3		vPosition;
        D3DXVECTOR3		vLastMoveDistance;
        D3DXQUATERNION  qRotation;
        D3DXVECTOR3 vFramePosition[MISSILE_FIXED_TRACK_FRAME_COUNT];
	};

public:
	KFixedTrackMissileProcessor();

protected:
	~KFixedTrackMissileProcessor();

	int Init(DWORD dwMissileParamID);
	int UnInit();
	int Update(double fTime, double fTimeLast, KRLMissile* pRLMissile);
	int IsFinished() const;
	int Start(double fTime, KRLMissile* pRLMissile);

private:
    HRESULT MissileMoveAngle(D3DXVECTOR3 vPosition, float& fAngle);
    HRESULT GetTargetOrientation(D3DXVECTOR3& vPosition, D3DXQUATERNION& qRotation) const;
    HRESULT PrepareFixedTrackMissileOrientationInitialData();

private:
    float   m_fTotalModelScale;

	KFixedTrackMissile	m_aFixedTrackMissile[MISSILE_PHASE_SFX_COUNT];
};

#endif 

#endif // CASE_ACTION_OBJECT_MISSILE_K_FIXED_TRACK_MISSILE_PROCESSOR_H