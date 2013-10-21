// Camera.cpp: implementation of the GraphicsStruct::KCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace GraphicsStruct
{

KCamera::KCamera()
{
	Position = D3DXVECTOR3(200,500,200);
	ViewPoint = D3DXVECTOR3(0,0,0);
	DefUp = D3DXVECTOR3(0,1,0);
	Field = D3DX_PI/4;
	zNear = 10.0f;
	zFar = 300.0f;
	Aspect = 1.0f;
	m_bPerspect = TRUE;
}

KCamera::~KCamera()
{

}

HRESULT KCamera::SetCamera()
{
	/*D3DXMATRIX matView;

	D3DXMatrixLookAtLH(&matView,
		&Position,
		&ViewPoint,
		&DefUp);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXMATRIX matProj;
	
	if(m_bPerspect)
	{
		D3DXMatrixPerspectiveFovLH( &matProj, Field, Aspect , zNear, zFar );
		g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	}
	else
	{
		D3DXVECTOR3 Ver = Position - ViewPoint;
		float Width = D3DXVec3Length(&Ver);
		float Height = Width / Aspect;

		D3DXMatrixOrthoLH(&matProj,Width,Height,zNear*0.5f,zFar);
		g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	}*/
	return S_OK;
}

};
