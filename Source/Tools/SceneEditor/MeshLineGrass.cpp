// MeshLineGrass.cpp: implementation of the KModelSFXLineGrass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshLineGrass.h"

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
KModelSFXLineGrass::KModelSFXLineGrass()
{
	m_nNumLinePerGrass = 100;
}

KModelSFXLineGrass::~KModelSFXLineGrass()
{

}

HRESULT KModelSFXLineGrass::LoadMesh(LPSTR pFileName)
{
	
	//////////////////////////////////////////////////////////////////////////
	LPMOTION pMotion = new MOTION;
	m_lpMotion.push_back(pMotion);
	pMotion = new _MotionCircle;
	m_lpMotion.push_back(pMotion);

	DWORD m_dwNumObject = 400;
	
	DWORD Row =0,K =0;
	DWORD RowLength = (DWORD) sqrt((float)(m_dwNumObject));

	for(DWORD i=0;i<m_dwNumObject;i++)
	{
		K++;
		float X = (rand()%1000)*0.001f*500 - 250;
		float Y = (rand()%1000)*0.001f*500 - 250;
		
		if (K>RowLength)
		{
			Row++;
			K=0;
		}
		X = Row * 100.0f - RowLength*50 + (rand()%1000)*0.001f*50;
		Y = K   * 100.0f - RowLength*50 + (rand()%1000)*0.001f*50;
		float Size = (rand()%1000)*0.001f*5+15;

		D3DXVECTOR3 Position = D3DXVECTOR3(X,0,Y);

		LPSPRITE pSpr = new SPRITE;

		pSpr->Width    = Size ;
		pSpr->Height = Size;
		pSpr->MotionType = 0;
		pSpr->Position   = Position;
		pSpr->Arg[0] = Position.x;
		pSpr->Arg[1] = Position.y;
		pSpr->Arg[2] = Position.z;

		m_lpSprite.push_back(pSpr);
		m_nNumFaces = m_nNumLinePerGrass*m_lpSprite.size();

	}

	CreateVerticesBuffer();
	
	wsprintf(m_szMeshFileName,"Line Grass");
	UpdateVertices();

	m_dNumMaterial = 1;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
	
	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_CW|
		MATERIAL_OPTION_SPECULARENABLE;
	
	for( i=0;i<m_dNumMaterial;i++)
	{
		m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Power = 15;
		m_lpMaterial[i].m_dOption = Def_Option;
	}
	TCHAR Name[256];
	wsprintf(Name,"%s\\BillBoard.Mtl",g_Def_ModelDirectory);
	LoadMaterial(Name);

	return S_OK;
}

HRESULT KModelSFXLineGrass::Render()
{
	SetMatrix();
		
	KModel::SetMaterial(0);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,TRUE);
	float  F = 1.0f;

	g_pd3dDevice->SetStreamSource( 0, m_lpVBuf,0, sizeof( LINE ) );
	g_pd3dDevice->SetFVF( LINE_FVF );
	g_pd3dDevice->DrawPrimitive  ( D3DPT_POINTLIST , 0 , m_nNumFaces );

	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,FALSE);
	RestoreMatrix();
	return S_OK;
}

HRESULT KModelSFXLineGrass::CreateVerticesBuffer()
{
	SAFE_RELEASE(m_lpVBuf);
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( m_nNumFaces*sizeof(LINE),
			0,LINE_FVF,
			D3DPOOL_MANAGED, &m_lpVBuf,NULL ) ) )
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT KModelSFXLineGrass::UpdateVertices()
{
	LINE * pVer = NULL;
	if ( FAILED( m_lpVBuf->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;

	int nPoly = 9;
	float Radius = 130;
	D3DXVECTOR3 X_Base(1,0,0);
	D3DXVECTOR3 Y_Base(0,0,1);

	if (m_nBillBoardMethod==BILLBOARD_POSITION_ONLY)
	{
		g_cGraphicsTool.GetCameraLeftUp(&X_Base,&Y_Base);
	}
	
	SortSprites();
	
	for(int i =0;i<(int)m_lpSprite.size();i++)
	{
		LPSPRITE     pSprite = m_lpSprite[i];
		LPMOTION     pMotion = m_lpMotion[pSprite->MotionType];
		
		pMotion->Apply(pSprite);

		float f = timeGetTime()*m_fCircleSpeed;
		float Radius = m_fCircleRadius;

		D3DXVECTOR3  PolyCenter = pSprite->Position;
	
		float T = timeGetTime()*0.001f;
		float KK = (PolyCenter.x + PolyCenter.z)*0.01f;
		float Left = sinf(KK*0.1f+T)*2.5f+8.5f;
		for(int S=0;S<m_nNumLinePerGrass;S++)
		{
			D3DXVECTOR3 Pos_A = PolyCenter + (float)S * D3DXVECTOR3 (S*Left*0.001f,pSprite->Width*0.03f,0);

			D3DCOLOR Color_A = RGB(100,S*20+150,100)|0xFF000000;
			
			float ddd = (S-50.0f)/50.0f;
			ddd *= ddd;

			int VI = i*m_nNumLinePerGrass+S;
			pVer[VI  ].Pos = Pos_A;
			pVer[VI  ].Color = Color_A;
			pVer[VI  ].PSize = 1 + (1-ddd)*10;
		}
	}
	
	m_lpVBuf->Unlock();
	return S_OK;
}

HRESULT KModelSFXLineGrass::FrameMove()
{
	UpdateVertices();
	return S_OK;
}
};