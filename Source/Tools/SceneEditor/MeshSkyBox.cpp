// MeshSkyBox.cpp: implementation of the KModelSFXSkyBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "MeshSkyBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace ModelSFX
{
KModelSFXSkyBox::KModelSFXSkyBox()
{
	m_Cubemap = NULL;
}

KModelSFXSkyBox::~KModelSFXSkyBox()
{

}

HRESULT KModelSFXSkyBox::LoadMesh(LPSTR pFileName)
{
	HRESULT hr = S_OK;

	TCHAR Name[256];

	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"Sky.Mesh");
	if (FAILED(KModel::LoadMesh    (Name)))
		return E_FAIL;
	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"Sky.Mtl");
	if (FAILED(KModel::LoadMaterial(Name)))
		return E_FAIL;

	D3DXVECTOR3 P_Center = (m_BBox_A+m_BBox_B)*-0.5f;
	Absolute_Move(&P_Center);

	D3DVERTEXELEMENT9 decl[] =
    {
        { 0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // v0
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },   // v3
        { 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, // v7
		D3DDECL_END()
    };

    if ( FAILED( g_pd3dDevice->CreateVertexDeclaration( decl, &g_pVertexDeclaration ) ) )
		return E_FAIL;
	
	LPD3DXMESH pMesh = NULL;
	hr = m_pMeshSys->CloneMesh(D3DXMESH_MANAGED, decl, g_pd3dDevice, &pMesh);
    if (FAILED(hr))
	{
		MessageBox(g_hBaseWnd,"Create Sky Box Failed in CloneMesh!","SceneEditor",0);
		return E_FAIL;
    }
	hr = D3DXComputeNormals(pMesh,NULL);
    if (FAILED(hr))
		return E_FAIL;

	m_pMeshSys->Release();
	m_pMeshSys = pMesh;
	
	struct _Ver 
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR3 Nor;
		D3DXVECTOR3 Tex;
	}* pVers = NULL;

	D3DXVECTOR3 Center = (m_BBox_A + m_BBox_B)*0.5f;
	
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;
	
	for(DWORD i=0;i<m_pMeshSys->GetNumVertices();i++)
	{
		D3DXVECTOR3 PO = pVers[i].Pos - Center;
		D3DXVec3Normalize(&pVers[i].Tex,&PO);
	}

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;

	g_cTextureTable.Get1NewTexture(&m_Cubemap,0);

	g_cFileFinder.FindFile(Name,"LobbyCube.dds");
	if (FAILED(m_Cubemap->LoadCubeMap(Name)))
	{
		MessageBox(g_hBaseWnd,"Create Sky Box Failed in LoadCubeMap!","SceneEditor",0);
	}

	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_CW|
		MATERIAL_OPTION_SPECULARENABLE;
	for(i=0;i<m_dNumMaterial;i++)
	{
		m_lpMaterial[i].m_dOption = Def_Option;
	}

	m_lpMaterial[0].m_dTextureIDs[0] = m_Cubemap->ID;

	OptimizeUnSkinedMesh();
	D3DXMatrixScaling(&m_Matrix,10000,10000,10000);

	return S_OK;
}

HRESULT KModelSFXSkyBox::Render()
{
	//DWORD Fog = TRUE;
	//g_pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &Fog );
	//g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	float Scal = 10000;
	D3DXMatrixScaling(&m_Matrix,Scal,Scal,Scal);
	
	D3DXVECTOR3 CameraPos;
	g_cGraphicsTool.GetCameraPos(&CameraPos);
	float Center_Y = (m_BBox_B.y - m_BBox_A.y)*0.5f;
		
	m_Matrix._41 =  CameraPos.x;
	m_Matrix._42 =  CameraPos.y; //+ (Center_Y);
	m_Matrix._43 =  CameraPos.z;
		
	g_pd3dDevice->SetTextureStageState(0,
		D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT3 );
	DWORD Cull = 0;
	g_pd3dDevice->GetRenderState( D3DRS_CULLMODE,  &Cull );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );

	KModel::Render();
	/*SetMatrix();

	DrawMeshSubset(m_pMeshOpt,0);

	RestoreMatrix();*/

	g_pd3dDevice->SetTextureStageState(0,
		D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2 );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   Cull );

	//g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, Fog );

	return S_OK;
}
HRESULT KModelSFXSkyBox::GetSkyTexture(LPTEXTURE* ppTexture)
{
	(*ppTexture) = m_Cubemap;
	return S_OK;
}

};