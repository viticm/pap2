#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "kg3dstructs.h"
#include "KG3DEditGridBase.h"

#define MOVESTATE_NONE 0
#define MOVESTATE_X       1
#define MOVESTATE_Y       2
#define MOVESTATE_Z       3
#define MOVESTATE_XY      4
#define MOVESTATE_YZ      5
#define MOVESTATE_XZ      6
#define MOVESTATE_XYZ     7

class KG3DModel;
class KG3DCamera;

using namespace std;

class KG3DEditMoveGrid : public KG3DEditGridBase 
{

public:
	D3DXPLANE   m_MovePlane;//移动的限制平面
	D3DXVECTOR3 m_vPositionBegan;
	D3DXVECTOR3 m_vMousePointOnMovePlaneBegan;//移动开始时候鼠标在限制平面上的焦点
	D3DXVECTOR3 m_vMousePointOnMovePlaneNow;

	D3DXVECTOR3 m_LastPosition;

	virtual HRESULT Render();

	HRESULT ComputeMovePlane(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);

	HRESULT FrameMove(BOOL bMove,KG3DCamera* pCamera,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,float XX,float YY);
	D3DXVECTOR3 GetPos(D3DXVECTOR3 Axis,D3DXVECTOR3 U,D3DXVECTOR3 R,float XX,float YY,float K);

	HRESULT Init();

	KG3DEditMoveGrid(void);
	virtual ~KG3DEditMoveGrid(void);
};
