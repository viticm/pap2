// MeshFlare.cpp: implementation of the KModelSFXFlare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshFlare.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


namespace ModelSFX
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KModelSFXFlare::KModelSFXFlare()
{
	m_dwNumLayer = 10;
	this->m_lpLayerPosition = NULL;
	m_bShow = FALSE;
}

KModelSFXFlare::~KModelSFXFlare()
{

}

HRESULT KModelSFXFlare::LoadMesh(LPSTR pFileName)
{
	HRESULT hr = S_OK;

	DWORD m_dNumFaces    = m_dwNumLayer * 2;
	DWORD m_dNumVertices = m_dwNumLayer * 4; 
	DWORD m_dNumSubsets  = m_dwNumLayer;
		
	SAFE_RELEASE(m_pMeshSys);
	if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED,
		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,g_pd3dDevice,&m_pMeshSys)))
	{
		return hr;
	}

	TLVertex * pVertices = NULL;
	WORD* pIndex = NULL;
	DWORD * pAttrib = NULL;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVertices)))
		return E_FAIL;
	if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
		return E_FAIL;
	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		return E_FAIL;

	//vertices
	D3DXVECTOR3 Left,Up;
	
	//Left = Axis_Left * pPat->Size;
	//Up   = Axis_Up   * pPat->Size;
	D3DXVECTOR3 Axis_Left(-100,0,0);
	D3DXVECTOR3 Axis_Up  (0, 100,0);
	for(DWORD i=0;i<m_dwNumLayer;i++)
	{	
		float Angle = i*0.3f;
		D3DXVECTOR3 Position((float)i,(float)i,(float)i*50);
		D3DCOLOR    Color = 0xFFFFFFFF;

		Left = Axis_Left * sinf(Angle+1.5708f) + Axis_Up * cosf(Angle+1.5708f);
		Up   = Axis_Left * sinf(Angle) + Axis_Up * cosf(Angle);
		
		pVertices[i*4  ].position = Position - Left + Up ;
		pVertices[i*4  ].tu = 0;pVertices[i*4  ].tv = 0;pVertices[i*4  ].diffuse = Color;

		pVertices[i*4+1].position = Position + Left + Up ;
		pVertices[i*4+1].tu = 1;pVertices[i*4+1].tv = 0;pVertices[i*4+1].diffuse = Color;

		pVertices[i*4+2].position = Position + Left - Up ;
		pVertices[i*4+2].tu = 1;pVertices[i*4+2].tv = 1;pVertices[i*4+2].diffuse = Color;
		
		pVertices[i*4+3].position = Position - Left - Up ;
		pVertices[i*4+3].tu = 0;pVertices[i*4+3].tv = 1;pVertices[i*4+3].diffuse = Color;

		//faces index
		pIndex[i*6  ] = (WORD)i*4  ;
		pIndex[i*6+1] = (WORD)i*4+2;
		pIndex[i*6+2] = (WORD)i*4+1;

		pIndex[i*6+3] = (WORD)i*4+2;
		pIndex[i*6+4] = (WORD)i*4+0;
		pIndex[i*6+5] = (WORD)i*4+3;

		pAttrib[i*2  ] = i;
		pAttrib[i*2+1] = i;
	}


	/*D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
		&m_BBox_A,&m_BBox_B);*/

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
		return E_FAIL;
	
	if (m_lpMaterial==NULL)
	{
		m_dNumMaterial = m_dNumSubsets;
		m_lpMaterial = new MATERIAL[m_dNumMaterial];
		ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
		for(i=0;i<m_dNumMaterial;i++)
		{
			m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.9f ;
			m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.9f ;
			m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.9f ;
			m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
			m_lpMaterial[i].m_sMaterial9.Ambient  = m_lpMaterial[i].m_sMaterial9.Diffuse;
			m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
			m_lpMaterial[i].m_sMaterial9.Power = 50;
		}
		TCHAR Name[256];
		wsprintf(Name,"%s%s",g_Def_ModelDirectory,"Flare.Mtl");
		LoadMaterial(Name);
		DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
			MATERIAL_OPTION_FILL_SOLID|
			MATERIAL_OPTION_SHADE_GOURAUD|
			MATERIAL_OPTION_CULL_CW|
			MATERIAL_OPTION_SPECULARENABLE|
			MATERIAL_OPTION_LIGHTING;
		for(i=0;i<m_dNumMaterial;i++)
		{
			m_lpMaterial[i].m_dOption = Def_Option;
		}
	}

	return S_OK;
}

HRESULT KModelSFXFlare::FrameMove()
{
	TLVertex * pVertices = NULL;
	D3DXVECTOR3 CameraLeft;
	D3DXVECTOR3 CameraFront;
	D3DXVECTOR3 CameraUp;
	D3DXVECTOR3 CameraPos;
	g_cGraphicsTool.GetCameraLeftUp(&CameraLeft,&CameraUp);
	g_cGraphicsTool.GetCameraFront(&CameraFront);
	g_cGraphicsTool.GetCameraPos(&CameraPos);

	D3DXVECTOR3 LightDir;
	g_cGraphicsTool.GetSunDirection(LightDir);

	D3DXVECTOR3 LookAt = CameraPos + CameraFront*600;
	D3DXVECTOR3 LightPos = CameraPos - LightDir * 3000;
	D3DXVECTOR3 LL = LightPos - LookAt;
	D3DXVec3Normalize(&LL,&LL);
	
	float E_Angle = D3DXVec3Dot(&LL,&CameraFront);
	/*if (E_Angle<=0.3f)
	{
		m_bShow = FALSE;
		return S_OK;
	}*/
	
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVertices)))
		return E_FAIL;
	//vertices
	D3DXVECTOR3 Left,Up;
	
	//Left = Axis_Left * pPat->Size;
	//Up   = Axis_Up   * pPat->Size;
	//D3DXVECTOR3 Axis_Left(-30,0,0);
	//D3DXVECTOR3 Axis_Up(0,30,0);
	D3DXVECTOR3 StartPos = LookAt + 7.5*350*LL;

	for(DWORD i=0;i<m_dwNumLayer;i++)
	{	
		float KK = (i-5)*(i-5)*0.2f+1.0f;
		D3DXVECTOR3 Axis_Left = 100*KK*CameraLeft;
		D3DXVECTOR3 Axis_Up   = 100*KK*CameraUp;
		float Angle = KK*E_Angle;
		D3DXVECTOR3 Position = -LL*350.0f*(float)i + StartPos;
		D3DCOLOR    Color = 0xFFFFFFFF;

		Left = Axis_Left * sinf(Angle+1.5708f) + Axis_Up * cosf(Angle+1.5708f);
		Up   = Axis_Left * sinf(Angle) + Axis_Up * cosf(Angle);
		
		pVertices[i*4  ].position = Position - Left + Up ;
		pVertices[i*4  ].tu = 0;pVertices[i*4  ].tv = 0;pVertices[i*4  ].diffuse = Color;

		pVertices[i*4+1].position = Position + Left + Up ;
		pVertices[i*4+1].tu = 1;pVertices[i*4+1].tv = 0;pVertices[i*4+1].diffuse = Color;

		pVertices[i*4+2].position = Position + Left - Up ;
		pVertices[i*4+2].tu = 1;pVertices[i*4+2].tv = 1;pVertices[i*4+2].diffuse = Color;
		
		pVertices[i*4+3].position = Position - Left - Up ;
		pVertices[i*4+3].tu = 0;pVertices[i*4+3].tv = 1;pVertices[i*4+3].diffuse = Color;
	}

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;
	m_bShow = TRUE;

	return S_OK;
}

HRESULT KModelSFXFlare::Render()
{
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	if (m_bShow)
		KModel::Render();

	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x20);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}
};