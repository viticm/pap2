#include "stdafx.h"
#include ".\kg3deditrotationgrid.h"
#include "kg3dgraphicstool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DEditRotationGrid::KG3DEditRotationGrid(void)
{
    m_fRotationXBegin = 0;
    m_fRotationYBegin = 0;
    m_fRotationZBegin = 0;
    m_fRotationX = 0;
    m_fRotationY = 0;
    m_fRotationZ = 0;
	m_fAngle = 0;
}

KG3DEditRotationGrid::~KG3DEditRotationGrid(void)
{
}

HRESULT KG3DEditRotationGrid::Init()
{
	OpenGridModel("Data\\editor\\旋转\\R_Y.Mesh",ROTATIONSTATE_Y);
	OpenGridModel("Data\\editor\\旋转\\R_X.Mesh",ROTATIONSTATE_X);
	OpenGridModel("Data\\editor\\旋转\\R_Z.Mesh",ROTATIONSTATE_Z);

	return S_OK;
}

float GetAngle(float x,float y)
{
	D3DXVECTOR2 A(x,y);
	float Angle = 0;
	D3DXVec2Normalize(&A,&A);
	if(A.x>0)
	{
		Angle = -asinf(A.y);
	}
	else
	{
		Angle = asinf(A.y) - D3DX_PI ;
	}
	/*if(Angle>2*D3DX_PI)
		Angle -= 2*D3DX_PI;
	if(Angle<0)
		Angle += 2*D3DX_PI;*/
	return Angle;
}

HRESULT KG3DEditRotationGrid::FrameMove(BOOL bRotate,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,float XX,float YY,float ZZ)
{
	D3DXVECTOR3 V = Origin - m_Translation;
	float K = D3DXVec3Length(&V)* 0.0005F;
	SetScaling(&D3DXVECTOR3(K,K,K));

	if(!bRotate)
	{
		CheckState(Origin,Direction);
	}
	else
	{
		D3DXPlaneIntersectLine(&m_vMousePointOnMovePlaneNow,&m_MovePlane,&Origin,&(Origin+Direction*10000));
		D3DXVECTOR3 temp = m_vMousePointOnMovePlaneNow - Origin ;
		float dir = temp.x * Direction.x ;
	
		if (dir < 0)
		{
			switch(m_dwState)
			{
			case ROTATIONSTATE_Y:
				{
					m_vMousePointOnMovePlaneNow.x = - m_vMousePointOnMovePlaneNow.x;
					m_vMousePointOnMovePlaneNow.z = - m_vMousePointOnMovePlaneNow.z;
					break;
				}
			case ROTATIONSTATE_X:
				{
					m_vMousePointOnMovePlaneNow.y = - m_vMousePointOnMovePlaneNow.y;
					m_vMousePointOnMovePlaneNow.z = - m_vMousePointOnMovePlaneNow.z;

					break;
				}
			case ROTATIONSTATE_Z:
				{
					m_vMousePointOnMovePlaneNow.x = - m_vMousePointOnMovePlaneNow.x;
					m_vMousePointOnMovePlaneNow.y = - m_vMousePointOnMovePlaneNow.y;
					break;
				}
			}
		}
		D3DXVECTOR3 vPos(0,0,0);
		GetTranslation(&vPos);
		
		D3DXVECTOR3 A = m_vMousePointOnMovePlaneBegan - vPos;
		D3DXVECTOR3 B = m_vMousePointOnMovePlaneNow - vPos;

		D3DXQUATERNION R;
		D3DXQuaternionIdentity(&R);

		switch(m_dwState)
		{
		case ROTATIONSTATE_Y:
			{
				float AngleA = GetAngle(A.x,A.z);
				float AngleB = GetAngle(B.x,B.z);
				m_fAngle = AngleB - AngleA;

                m_fRotationY = m_fAngle;
				D3DXQuaternionRotationYawPitchRoll(&R,m_fAngle,0,0);
				D3DXQuaternionNormalize(&R,&R);

				break;
			}
		case ROTATIONSTATE_X:
			{
				float AngleA = GetAngle(A.z,A.y);
				float AngleB = GetAngle(B.z,B.y);
				m_fAngle = AngleB - AngleA;

                m_fRotationX = m_fAngle;
				D3DXQuaternionRotationYawPitchRoll(&R,0,m_fAngle,0);
				D3DXQuaternionNormalize(&R,&R);

				break;
			}
		case ROTATIONSTATE_Z:
			{
				float AngleA = GetAngle(A.y,A.x);
				float AngleB = GetAngle(B.y,B.x);
				m_fAngle = AngleB - AngleA;

                m_fRotationZ = m_fAngle;
				D3DXQuaternionRotationYawPitchRoll(&R,0,0,m_fAngle);
				D3DXQuaternionNormalize(&R,&R);
				break;
			}
		}

		D3DXQUATERNION vRot;
		vRot = R*m_RotBegain;
		D3DXQuaternionNormalize(&vRot,&vRot);
		SetRotation(&vRot);

	}
	return S_OK;
}

HRESULT KG3DEditRotationGrid::ComputeMovePlane(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXVECTOR3 vPos,vNormal;
	GetTranslation(&vPos);
	switch(m_dwState)
	{
	case ROTATIONSTATE_Y:
		{	
			vNormal = D3DXVECTOR3(0,1,0);
			break;
		}
	case ROTATIONSTATE_X:
		{
			vNormal = D3DXVECTOR3(1,0,0);
			break;
		}
	case ROTATIONSTATE_Z:
		{
			vNormal = D3DXVECTOR3(0,0,1);
			break;
		}
	}

	D3DXPlaneFromPointNormal(&m_MovePlane,&vPos,&vNormal);
	GetRotation(&m_RotBegain);
	/*D3DXMATRIX mat;
	D3DXMatrixRotationQuaternion(&mat,&m_RotBegain);
	D3DXMatrixInverse(&mat,NULL,&mat);
	D3DXMatrixTranspose(&mat,&mat);
	D3DXPLANE vPlane;
	D3DXPlaneTransform(&vPlane,&m_MovePlane,&mat);*/

	D3DXPlaneIntersectLine(&m_vMousePointOnMovePlaneBegan,&m_MovePlane,&Origin,&(Origin+Direction*10000));

	return S_OK;
}

HRESULT KG3DEditRotationGrid::Render()
{
	D3DXVECTOR3 vPos;
	GetTranslation(&vPos);

	g_cGraphicsTool.DrawLine(&vPos,&m_vMousePointOnMovePlaneBegan,0xFFFF0000,0xFF00FF00);
	g_cGraphicsTool.DrawLine(&vPos,&m_vMousePointOnMovePlaneNow,0xFFFF0000,0xFF0000FF);

	{
		/*
		RECT destRect1;
				SetRect(&destRect1, 2, 250, 0, 80);*/
		
		TCHAR str[256];
		sprintf(str, "Angle: %.2f",m_fAngle * 180.0F/D3DX_PI);

		/*
		LPD3DXFONT pFont = g_cGraphicsTool.GetFont();
				if (pFont)
				{
					pFont->DrawTextA(NULL, str, -1, &destRect1, 
						DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
				}*/
		g_cGraphicsTool.DrawDebugText(str);//现在全局统一排序绘制
	}

	return KG3DEditGridBase::Render();
}