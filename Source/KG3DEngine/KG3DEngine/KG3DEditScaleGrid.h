// ***************************************************************
//
//  Description: Scale Tool
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-04-27
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#ifndef __H_KG3DSCALEGRID__
#define __H_KG3DSCALEGRID__

#define SCALESTATE_NONE 0
#define SCALESTATE_X    1
#define SCALESTATE_Y    2
#define SCALESTATE_Z    3
#define SCALESTATE_XY   4
#define SCALESTATE_YZ   5
#define SCALESTATE_XZ   6
#define SCALESTATE_XYZ  7

#include "KG3DEditGridBase.h"
class KG3DCamera;

class KG3DEditScaleGrid
    : public KG3DEditGridBase
{

	D3DXVECTOR3 GetPos(D3DXVECTOR3 Axis,D3DXVECTOR3 U,D3DXVECTOR3 R,float XX,float YY,float K);

	D3DXPLANE m_vMousePlane;//限制鼠标交点的平面
	D3DXVECTOR3 m_vMousePostionBegan;//初始时候鼠标交点的位置
	D3DXVECTOR3 m_vMousePostionNow;//现在鼠标和平面交点的位置
public:
	virtual HRESULT Render();

	HRESULT ComputeMovePlane(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);

	HRESULT Init();
	HRESULT FrameMove(BOOL bScale,KG3DCamera* pCamera,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,float XX,float YY);

public:
    KG3DEditScaleGrid(void);
    ~KG3DEditScaleGrid(void);
};

#endif