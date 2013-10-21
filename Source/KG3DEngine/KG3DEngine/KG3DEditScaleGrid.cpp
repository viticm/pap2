// ***************************************************************
//
//  Description: 
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
#include "StdAfx.h"
#include "KG3DEditScaleGrid.h"
#include "KG3DCamera.h"
#include "kg3dgraphicstool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DEditScaleGrid::KG3DEditScaleGrid(void)
{
}

KG3DEditScaleGrid::~KG3DEditScaleGrid(void)
{
}

HRESULT KG3DEditScaleGrid::Init()
{
	OpenGridModel("Data\\editor\\缩放\\S_X.Mesh",SCALESTATE_X);
	OpenGridModel("Data\\editor\\缩放\\S_Y.Mesh",SCALESTATE_Y);
	OpenGridModel("Data\\editor\\缩放\\S_Z.Mesh",SCALESTATE_Z);
	OpenGridModel("Data\\editor\\缩放\\S_XZ.Mesh",SCALESTATE_XZ);
	OpenGridModel("Data\\editor\\缩放\\S_XY.Mesh",SCALESTATE_XY);
	OpenGridModel("Data\\editor\\缩放\\S_YZ.Mesh",SCALESTATE_YZ);
	OpenGridModel("Data\\editor\\缩放\\S_C.Mesh",SCALESTATE_XYZ);

	return S_OK;
}

D3DXVECTOR3 KG3DEditScaleGrid::GetPos(D3DXVECTOR3 Axis,D3DXVECTOR3 U,D3DXVECTOR3 R,float XX,float YY,float K)
{
	float Screen_X = D3DXVec3Dot(&Axis,&R);
	float Screen_Y = D3DXVec3Dot(&Axis,&U);

	D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
	D3DXVec2Normalize(&ScreenVec,&ScreenVec);

	float Length = ScreenVec.x*XX - ScreenVec.y*YY;
	D3DXVECTOR3 Pos = Length * Axis * K;

	return Pos;
}

HRESULT KG3DEditScaleGrid::FrameMove(BOOL bScale,KG3DCamera* pCamera,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,float XX,float YY)
{
	D3DXVECTOR3 U = pCamera->GetCameraUp();
	D3DXVECTOR3 R = pCamera->GetCameraRight();

	D3DXVECTOR3 V = Origin - m_Translation;
	//float K = D3DXVec3Length(&V)* 0.00005F;

	D3DXVECTOR3 Vec = R*XX+U*YY;

	if(!bScale)
	{
		CheckState(Origin,Direction);
	}
	else
	{
		/*D3DXVECTOR3 Scale;
		GetScaling(&Scale);

		switch(m_dwState)
		{
		case SCALESTATE_X:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(1,0,0),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}	
		case SCALESTATE_Y:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(0,1,0),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}	
		case SCALESTATE_Z:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(0,0,1),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}
		case SCALESTATE_XY:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(1,1,0),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}	
		case SCALESTATE_YZ:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(0,1,1),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}
		case SCALESTATE_XZ:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(1,0,1),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}
		case SCALESTATE_XYZ:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(1,1,1),U,R,XX,YY,K);
				Scale += Pos;
				break;
			}
		}
		SetScaling(&Scale);*/

		D3DXPlaneIntersectLine(&m_vMousePostionNow,&m_vMousePlane,&Origin,&(Origin+Direction*10000));

		D3DXVECTOR3 vPos;
		GetTranslation(&vPos);

		D3DXVECTOR3 A = m_vMousePostionBegan - vPos;
		D3DXVECTOR3 B = m_vMousePostionNow - vPos;
		D3DXVECTOR3 C = D3DXVECTOR3(1,1,1);
		switch(m_dwState)
		{
		case SCALESTATE_X:
			{
				C.x = B.x / A.x;
				break;
			}	
		case SCALESTATE_Y:
			{
				C.y = B.y / A.y;
				break;
			}	
		case SCALESTATE_Z:
			{
				C.z = B.z / A.z;
				break;
			}
		case SCALESTATE_XY:
			{
				C.x = C.y = sqrtf(B.x*B.x + B.y*B.y) / sqrtf(A.x*A.x+A.y*A.y);
				break;
			}	
		case SCALESTATE_YZ:
			{
				C.y = C.z = sqrtf(B.y*B.y + B.z*B.z) / sqrtf(A.y*A.y+A.z*A.z);
				break;
			}
		case SCALESTATE_XZ:
			{
				C.x = C.z = sqrtf(B.x*B.x + B.z*B.z) / sqrtf(A.x*A.x+A.z*A.z);
				break;
			}
		case SCALESTATE_XYZ:
			{
				C.x = C.y = C.z = sqrtf(B.x*B.x+B.y*B.y+B.z*B.z) / sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);
				break;
			}
		}
		SetScaling(&C);
	}
	return S_OK;
}

HRESULT KG3DEditScaleGrid::ComputeMovePlane(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXVECTOR3 vPos,vNormal;
	GetTranslation(&vPos);
	switch(m_dwState)
	{
	case SCALESTATE_X:
		{	
			vNormal = D3DXVECTOR3(0,0,1);
			break;
		}
	case SCALESTATE_Y:
		{
			D3DXVECTOR3 Direct_XZ = D3DXVECTOR3(Direction.x,0,Direction.z);
			D3DXVec3Normalize(&vNormal,&Direct_XZ);
			break;
		}
	case SCALESTATE_Z:
		{
			vNormal = D3DXVECTOR3(1,0,0);
			break;
		}
	case SCALESTATE_XZ:
		{
			vNormal = D3DXVECTOR3(0,1,0);
			break;
		}
	case SCALESTATE_XY:
		{
			vNormal = D3DXVECTOR3(0,0,1);
			break;
		}
	case SCALESTATE_YZ:
		{
			vNormal = D3DXVECTOR3(1,0,0);
			break;
		}
	case SCALESTATE_XYZ:
		{
			D3DXVECTOR3 Direct_XZ = D3DXVECTOR3(Direction.x,0,Direction.z);
			D3DXVec3Normalize(&vNormal,&Direct_XZ);
			break;
		}
	}

	D3DXPlaneFromPointNormal(&m_vMousePlane,&vPos,&vNormal);
	D3DXPlaneIntersectLine(&m_vMousePostionBegan,&m_vMousePlane,&Origin,&(Origin+Direction*10000));

	return S_OK;
}

HRESULT KG3DEditScaleGrid::Render()
{
	D3DXVECTOR3 vPos;
	GetTranslation(&vPos);

	g_cGraphicsTool.DrawLine(&vPos,&m_vMousePostionBegan,0xFFFF0000,0xFF00FF00);
	g_cGraphicsTool.DrawLine(&vPos,&m_vMousePostionNow,0xFFFF0000,0xFF0000FF);

	return KG3DEditGridBase::Render();
}