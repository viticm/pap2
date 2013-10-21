#include "StdAfx.h"
#include ".\kg3deditmovegrid.h"
#include ".\kg3dmodeltable.h"
#include ".\kg3denginemanager.h"
#include "kg3dgraphicstool.h"
#include "kg3dmodel.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DEditMoveGrid::KG3DEditMoveGrid(void)
{
	m_dwState = MOVESTATE_NONE;
	m_LastPosition = D3DXVECTOR3(0,0,0);
}

KG3DEditMoveGrid::~KG3DEditMoveGrid(void)
{
}


HRESULT KG3DEditMoveGrid::Init()
{
	OpenGridModel("Data\\editor\\移动\\M_X.Mesh",MOVESTATE_X);
	OpenGridModel("Data\\editor\\移动\\M_Y.Mesh",MOVESTATE_Y);
	OpenGridModel("Data\\editor\\移动\\M_Z.Mesh",MOVESTATE_Z);
	OpenGridModel("Data\\editor\\移动\\M_XZ.Mesh",MOVESTATE_XZ);
	OpenGridModel("Data\\editor\\移动\\M_XY.Mesh",MOVESTATE_XY);
	OpenGridModel("Data\\editor\\移动\\M_YZ.Mesh",MOVESTATE_YZ);
	OpenGridModel("Data\\editor\\移动\\M_C.Mesh",MOVESTATE_XYZ);
	return S_OK;
}

D3DXVECTOR3 KG3DEditMoveGrid::GetPos(D3DXVECTOR3 Axis,D3DXVECTOR3 U,D3DXVECTOR3 R,float XX,float YY,float K)
{
	float Screen_X = D3DXVec3Dot(&Axis,&R);
	float Screen_Y = D3DXVec3Dot(&Axis,&U);

	D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
	D3DXVec2Normalize(&ScreenVec,&ScreenVec);

	float Length = ScreenVec.x*XX - ScreenVec.y*YY;
	D3DXVECTOR3 Pos = Length * Axis * K * 2;

	return Pos;
}

HRESULT KG3DEditMoveGrid::FrameMove(BOOL bMove,KG3DCamera* pCamera,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,float XX,float YY)
{
	m_LastPosition = m_Translation;

	D3DXVECTOR3 U = pCamera->GetCameraUp();
	D3DXVECTOR3 R = pCamera->GetCameraRight();

	D3DXVECTOR3 V = Origin - m_Translation;
	float K = D3DXVec3Length(&V)* 0.0005F;
	SetScaling(&D3DXVECTOR3(K,K,K));

	D3DXVECTOR3 Vec = R*XX+U*YY;

	if(!bMove)
	{
		CheckState(Origin,Direction);
	}
	else
	{
		/*D3DXVECTOR3 Trans;
		GetTranslation(&Trans);
		switch(m_dwState)
		{
		case MOVESTATE_X:
			{		
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(1,0,0),U,R,XX,YY,K);
				Trans += Pos;
				break;
			}
		case MOVESTATE_Y:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(0,1,0),U,R,XX,YY,K);
				Trans += Pos;
				break;
			}
		case MOVESTATE_Z:
			{
				D3DXVECTOR3 Pos = GetPos(D3DXVECTOR3(0,0,1),U,R,XX,YY,K);
				Trans += Pos;
				break;
			}
		case MOVESTATE_XZ:
			{
				D3DXVECTOR3 Pos1 = GetPos(D3DXVECTOR3(1,0,0),U,R,XX,YY,K);
				D3DXVECTOR3 Pos2 = GetPos(D3DXVECTOR3(0,0,1),U,R,XX,YY,K);
				Trans += Pos1 + Pos2;
				break;
			}
		case MOVESTATE_XY:
			{
				D3DXVECTOR3 Pos1 = GetPos(D3DXVECTOR3(1,0,0),U,R,XX,YY,K);
				D3DXVECTOR3 Pos2 = GetPos(D3DXVECTOR3(0,1,0),U,R,XX,YY,K);
				Trans += Pos1 + Pos2;
				break;
			}
		case MOVESTATE_YZ:
			{
				D3DXVECTOR3 Pos1 = GetPos(D3DXVECTOR3(0,1,0),U,R,XX,YY,K);
				D3DXVECTOR3 Pos2 = GetPos(D3DXVECTOR3(0,0,1),U,R,XX,YY,K);
				Trans += Pos1 + Pos2;
				break;
			}
		case MOVESTATE_XYZ:
			{
				D3DXVECTOR3 Pos1 = GetPos(U,U,R,XX,YY,K);
				D3DXVECTOR3 Pos2 = GetPos(R,U,R,XX,YY,K);
				Trans += Pos1 + Pos2;
				break;
			}
		}
		SetTranslation(&Trans);*/
		D3DXPlaneIntersectLine(&m_vMousePointOnMovePlaneNow,&m_MovePlane,&Origin,&(Origin+Direction*10000));

		D3DXVECTOR3 Trans = m_vMousePointOnMovePlaneNow - m_vMousePointOnMovePlaneBegan;
		switch(m_dwState)
		{
		case MOVESTATE_X:
			{		
				Trans.y = Trans.z =0;
				break;
			}
		case MOVESTATE_Y:
			{
				Trans.x = Trans.z =0;
				break;
			}
		case MOVESTATE_Z:
			{
				Trans.x = Trans.y =0;
				break;
			}
		case MOVESTATE_XZ:
			{
				Trans.y = 0;
				break;
			}
		case MOVESTATE_XY:
			{
				Trans.z = 0;
				break;
			}
		case MOVESTATE_YZ:
			{
				Trans.x = 0;
				break;
			}
		case MOVESTATE_XYZ:
			{
				break;
			}
		}

		D3DXVECTOR3 vPos = Trans + m_vPositionBegan;
		SetTranslation(&vPos);
	}
	return S_OK;
}

HRESULT KG3DEditMoveGrid::ComputeMovePlane(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXVECTOR3 vPos,vNormal;
	GetTranslation(&vPos);
	switch(m_dwState)
	{
	case MOVESTATE_X:
		{	
			vNormal = D3DXVECTOR3(0,0,1);
			break;
		}
	case MOVESTATE_Y:
		{
			D3DXVECTOR3 Direct_XZ = D3DXVECTOR3(Direction.x,0,Direction.z);
			D3DXVec3Normalize(&vNormal,&Direct_XZ);
			break;
		}
	case MOVESTATE_Z:
		{
			vNormal = D3DXVECTOR3(1,0,0);
			break;
		}
	case MOVESTATE_XZ:
		{
			vNormal = D3DXVECTOR3(0,1,0);
			break;
		}
	case MOVESTATE_XY:
		{
			vNormal = D3DXVECTOR3(0,0,1);
			break;
		}
	case MOVESTATE_YZ:
		{
			vNormal = D3DXVECTOR3(1,0,0);
			break;
		}
	case MOVESTATE_XYZ:
		{
			D3DXVECTOR3 Direct_XZ = D3DXVECTOR3(Direction.x,0,Direction.z);
			D3DXVec3Normalize(&vNormal,&Direct_XZ);
			break;
		}
	}

	m_vPositionBegan = vPos;
	D3DXPlaneFromPointNormal(&m_MovePlane,&vPos,&vNormal);
	D3DXPlaneIntersectLine(&m_vMousePointOnMovePlaneBegan,&m_MovePlane,&Origin,&(Origin+Direction*10000));

	return S_OK;
}

HRESULT KG3DEditMoveGrid::Render()
{
	D3DXVECTOR3 vPos = m_vPositionBegan;
	//GetTranslation(&vPos);

	g_cGraphicsTool.DrawLine(&vPos,&m_vMousePointOnMovePlaneBegan,0xFFFF0000,0xFF00FF00);
	g_cGraphicsTool.DrawLine(&vPos,&m_vMousePointOnMovePlaneNow,0xFFFF0000,0xFF0000FF);

	return KG3DEditGridBase::Render();
}

