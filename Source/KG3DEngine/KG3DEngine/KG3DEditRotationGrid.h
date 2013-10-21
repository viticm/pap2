#pragma once
#include "kg3deditgridbase.h"

#define ROTATIONSTATE_NONE 0
#define ROTATIONSTATE_X         1
#define ROTATIONSTATE_Y         2
#define ROTATIONSTATE_Z         3

class KG3DEditRotationGrid :
	public KG3DEditGridBase
{
	D3DXPLANE   m_MovePlane;//移动的限制平面
	D3DXVECTOR3 m_vMousePointOnMovePlaneBegan;//移动开始时候鼠标在限制平面上的焦点
	D3DXVECTOR3 m_vMousePointOnMovePlaneNow;

	float m_fAngle;
public:
	virtual HRESULT Render();
	HRESULT ComputeMovePlane(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);

	D3DXQUATERNION m_RotBegain;
	HRESULT FrameMove(BOOL bRotate,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,float XX,float YY,float ZZ);
	HRESULT Init();
	KG3DEditRotationGrid(void);
	virtual ~KG3DEditRotationGrid(void);

    float   m_fRotationXBegin;
    float   m_fRotationYBegin;
    float   m_fRotationZBegin;
    float   m_fRotationX;
    float   m_fRotationY;
    float   m_fRotationZ;
};
