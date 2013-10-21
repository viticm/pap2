#ifndef COMMON_K_MOVEMENT_CONTROLLER_H
#define COMMON_K_MOVEMENT_CONTROLLER_H

class KMovementController
{
public:
	KMovementController();
	~KMovementController();

	void Reset(); 

	void OnBegin();
	void OnMove(D3DXVECTOR3 vDelta);
	void OnEnd();

	BOOL IsBeingDragged() const;

	void SetYaw(float fYaw);
	void SetPitch(float fPitch);
	void SetRoll(float fRoll);
	float GetYaw() const;
	float GetPitch() const;
	float GetRoll() const;

private:
	float m_fYaw;
	float m_fPitch;
	float m_fRoll;

	BOOL m_bDrag;
};

#endif // COMMON_K_MOVEMENT_CONTROLLER_H