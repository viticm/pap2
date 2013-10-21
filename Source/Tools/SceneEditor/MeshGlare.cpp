// MeshGlare.cpp: implementation of the CMeshGlare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshGlare.h"
#include <algorithm>

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


KModelSFXHalo::KModelSFXHalo()
{
	T = 0;
	m_nTextureCount = 0;
	m_pVerBuffer = NULL;
	m_lpSurfaceDep = NULL;
	
	m_TextureWidth = 256;
	CameraDistance = -300;
	m_BaseColor = 0x00000000;
	Length = 100.0f; 
	m_nMaxNumHalo = 25;
}

KModelSFXHalo::~KModelSFXHalo()
{

}

HRESULT KModelSFXHalo::Render()
{
	if (!m_pBindMesh)
		return S_OK;

	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x20);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	m_dOption &=  ~MESH_BIND;
	
	SetMatrix();

	m_dOption |=  MESH_BIND;

	int N =0;
	list<HALO*>::iterator i = m_HaloList.begin();
	while(i!=m_HaloList.end())
	{
		HALO* pHalo = *i;

		g_pd3dDevice->SetTexture( 0, m_pTexture[pHalo->TextureID] );

		g_pd3dDevice->SetStreamSource( 0, m_lpVBuf,0, sizeof( VFormat::FACES_DIFFUSE_TEXTURE1 ) );
		g_pd3dDevice->SetFVF( D3DFVF_Faces_Diffuse_Texture1 );
		g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , N*2 , 2 );
		N++;
		i++;
	}

	TCHAR str[256];
	wsprintf(str,"N: %d",m_HaloList.size());
	g_cGraphicsTool.DrawTextScale(0.3f,0.5f,0.8f,0.8f,0xFF000000,16,"ËÎÌו",str,0);

	//g_cGraphicsTool.DrawScreenRect(&m_A,&m_C,m_Z,m_SprColor,m_pTexture);

	RestoreMatrix();

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	
	return S_OK;
}

HRESULT KModelSFXHalo::LoadMesh(LPSTR pFileName)
{
	HRESULT hr = S_OK;
	//////////////////////////////////////////////////////////////////////////
	/*LPMOTION pMotion = new MOTION;
	m_lpMotion.push_back(pMotion);
	pMotion = new _MotionCircle;
	m_lpMotion.push_back(pMotion);

	for(int i =0;i<5;i++)
	{
		D3DXVECTOR3 Position = D3DXVECTOR3(0,100,300 + i*300 );
		
		LPSPRITE pSpr = new SPRITE;
		
		pSpr->Size       = 200 ;
		pSpr->MotionType = 0;
		pSpr->Position   = Position;
		pSpr->Arg[0] = Position.x;
		pSpr->Arg[1] = Position.y;
		pSpr->Arg[2] = Position.z;
		
		m_lpSprite.push_back(pSpr);
	}*/

	m_nNumFaces = 2*m_nMaxNumHalo;

	CreateVerticesBuffer();

	if ( FAILED(hr = g_pd3dDevice->CreateDepthStencilSurface(m_TextureWidth,m_TextureWidth,
		D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,
		TRUE,
		&m_lpSurfaceDep,NULL)))
		return hr;

	for(int i=0;i<m_nMaxNumHalo;i++)
	{
		LPDIRECT3DTEXTURE9 pTexture = NULL;
		if ( FAILED(hr = D3DXCreateTexture(g_pd3dDevice,
			m_TextureWidth,m_TextureWidth,
			4,D3DUSAGE_RENDERTARGET|D3DUSAGE_AUTOGENMIPMAP,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pTexture)))
			return hr;
		m_pTexture.push_back(pTexture);
	}

	m_nBillBoardMethod = BILLBOARD_POSITION_ONLY;
	
	wsprintf(m_szMeshFileName,"Halo");
	UpdateVertices();

	m_dNumMaterial = 1;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
	
	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_CW|MATERIAL_OPTION_LIGHTING|
		MATERIAL_OPTION_SPECULARENABLE;
	
	for( i=0;i<(int)m_dNumMaterial;i++)
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
	wsprintf(Name,"%s\\Halo.Mtl",g_Def_ModelDirectory);
	LoadMaterial(Name);
	
	return S_OK;
}

HRESULT KModelSFXHalo::Render2Texture(int TextIndex)
{
	if (!m_pBindMesh)
		return S_OK;

	D3DXVECTOR3 AB = (m_pBindMesh->m_BBox_B - m_pBindMesh->m_BBox_A);
	D3DXVECTOR3 Center = (m_pBindMesh->m_BBox_A+m_pBindMesh->m_BBox_B)*0.5f;
	float L = D3DXVec3Length(&AB);
	CameraDistance = -1.5f*L;
	Length = L;
	D3DXVECTOR3 BindMeshPosition;
	BindMeshPosition.x = m_pBindMesh->m_CurMatrix._41;
	BindMeshPosition.y = m_pBindMesh->m_CurMatrix._42;
	BindMeshPosition.z = m_pBindMesh->m_CurMatrix._43;

	/*for(int S=0;S<m_lpSprite.size();S++)
	{
		LPSPRITE pSpr = m_lpSprite[S];
		pSpr->Size = L *0.65f;
	}*/

	//////////////////////////////////////////////////////////////////////////
	
	D3DVIEWPORT9 Viewport;
	D3DXMATRIX   Projection;
	D3DXMATRIX   View;
	g_pd3dDevice->GetViewport(&Viewport);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&Projection);
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&View);
	D3DXVECTOR3 BoxPos[8];
	GraphicsStruct::_BoundingBox Box;
	Box.PositionA = m_pBindMesh->m_BBox_A;
	Box.PositionB = m_pBindMesh->m_BBox_B;
	BoundingBox_Project(BoxPos,&Box,&Viewport,&Projection,&View,&m_pBindMesh->m_CurMatrix);
	D3DXComputeBoundingBox(BoxPos,8,sizeof(D3DXVECTOR3),
					&Box.PositionA,&Box.PositionB);
	m_A = D3DXVECTOR2(Box.PositionA.x,Box.PositionA.y);
	m_C = D3DXVECTOR2(Box.PositionB.x,Box.PositionB.y);
	D3DXVECTOR2 SrceenCenter = 0.5f*(m_A+m_C);
	m_A = (m_A - SrceenCenter)*1.5f+SrceenCenter;
	m_C = (m_C - SrceenCenter)*1.5f+SrceenCenter;
	m_Z = Box.PositionB.z;
	//////////////////////////////////////////////////////////////////////////
	
	HRESULT hr = S_OK;
	D3DVIEWPORT9 ViewPort,ViewPortSave;
	ViewPort.X = ViewPort.Y = 0;
	ViewPort.Width = ViewPort.Height = m_TextureWidth;
	ViewPort.MinZ = 0;
	ViewPort.MaxZ = 1.0f;


	g_pd3dDevice->GetViewport(&ViewPortSave);

	LPDIRECT3DSURFACE9 lpSurface = NULL;
	LPDIRECT3DSURFACE9 lpSurfaceDepSave = NULL;
	LPDIRECT3DSURFACE9 lpSurfaceSave = NULL;
	if (FAILED(hr = m_pTexture[TextIndex]->GetSurfaceLevel(0,&lpSurface)))
		return hr;
	lpSurface->Release();


	g_pd3dDevice->GetRenderTarget(0,&lpSurfaceSave);
	lpSurfaceSave->Release();
	g_pd3dDevice->GetDepthStencilSurface(&lpSurfaceDepSave);
	lpSurfaceDepSave->Release();
   
	float th = timeGetTime()*0.001f;
	D3DXVECTOR3 Front;
	g_cGraphicsTool.GetCameraFront(&Front); 
	
	D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt(0,0,0);
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView,matSave;
	   
	D3DXMATRIXA16 matProj,matProjSave;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1, 1000.0f );
    g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjSave );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXVECTOR3 Pos =  Center + BindMeshPosition;
	vEyePt = Front * CameraDistance + Pos;
	vLookatPt = Pos;
	
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->GetTransform( D3DTS_VIEW, &matSave );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	g_pd3dDevice->SetRenderTarget(0,lpSurface);
 	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpSurfaceDep);
	
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER/*|D3DCLEAR_STENCIL*/, 
                         m_BaseColor, 1.0f, 0 );

	//m_pTextureRender->BeginScene(lpSurface,&ViewPort);
	g_pd3dDevice->BeginScene();

	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER );

	//m_pBindMesh->m_dOption |= MESH_NOMATERAIL;

	m_pBindMesh->Render();
	//m_pBindMesh->DrawBBox();

	//m_pBindMesh->m_dOption &=  ~MESH_NOMATERAIL;

	//m_pTextureRender->EndScene(D3DX_FILTER_NONE);
	g_pd3dDevice->EndScene();
	
	g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);
	g_pd3dDevice->SetDepthStencilSurface(lpSurfaceDepSave);
	g_pd3dDevice->SetViewport(&ViewPortSave);
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matSave );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjSave );

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	//m_pTexture->SetAutoGenFilterType(D3DTEXF_POINT);
	//m_pTexture->GenerateMipSubLevels();
	
	return S_OK;
}

HRESULT KModelSFXHalo::FrameMove()
{
	if (!(m_dOption & MESH_BIND))
	{
		return S_OK;
	}

	D3DXVECTOR3 BindMeshPosition;
	BindMeshPosition.x = m_pBindMesh->m_CurMatrix._41;
	BindMeshPosition.y = m_pBindMesh->m_CurMatrix._42;
	BindMeshPosition.z = m_pBindMesh->m_CurMatrix._43;
	
	D3DXVECTOR3 Dis_A = BindMeshPosition - m_CurPosition;
	float Error = D3DXVec3LengthSq(&Dis_A);
	float HHH1 = 1000;
	T ++;
	if (T%4==0)
	{
		HALO* Temp = new HALO;
		Temp->Position = BindMeshPosition + D3DXVECTOR3(0,0,300);
		AddHalo(Temp);
		
		Render2Texture(Temp->TextureID);
	}
	//////////////////////////////////////////////////////////////////////////

	//m_SprColor =  Color;

	UpdateVertices();
	return S_OK;
}

HRESULT KModelSFXHalo::AddHalo(HALO* NewHalo)
{
	m_CurPosition = NewHalo->Position;
	NewHalo->TextureID = m_nTextureCount%m_nMaxNumHalo;
	m_nTextureCount++;

	m_HaloList.push_back(NewHalo);

	

	if ((int)m_HaloList.size()>=m_nMaxNumHalo)
	{
		m_HaloList.erase(m_HaloList.begin());
	}
	return S_OK;
}

HRESULT KModelSFXHalo::Remove1Halo()
{
	if (m_HaloList.size()>0)
	{
		list<HALO*>::iterator i = m_HaloList.begin();
		HALO* pHalo = *i;
		SAFE_DELETE(pHalo);
		m_HaloList.erase(m_HaloList.begin());
	}
	return S_OK;
}

HRESULT KModelSFXHalo::UpdateVertices()
{
	VFormat::FACES_DIFFUSE_TEXTURE1 * pVer = NULL;
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
	
	//SortHalo();
	

	int N =0;
	list<HALO*>::iterator i = m_HaloList.begin();
	while(i!=m_HaloList.end())
	{
		HALO* pHalo = *i;

		D3DXVECTOR3  PolyCenter = pHalo->Position;
	
		D3DXVECTOR3 XXX = X_Base * 100;
		D3DXVECTOR3 YYY = Y_Base * 100;

		D3DCOLOR Color = ColorValue2ref(m_lpMaterial[0].m_sMaterial9.Diffuse);
		float JJJ = ((N+1)*1.0f/m_HaloList.size())*255;
		DWORD Alpha = ((DWORD)(JJJ))<<24;
		Color |= Alpha;


		int VI = N*6;
		pVer[VI  ].p = PolyCenter - XXX - YYY;pVer[VI  ].diffuse = Color;pVer[VI  ].tu1 = 0;pVer[VI  ].tv1 = 1;
		pVer[VI+1].p = PolyCenter + XXX - YYY;pVer[VI+1].diffuse = Color;pVer[VI+1].tu1 = 1;pVer[VI+1].tv1 = 1;
		pVer[VI+2].p = PolyCenter - XXX + YYY;pVer[VI+2].diffuse = Color;pVer[VI+2].tu1 = 0;pVer[VI+2].tv1 = 0;
		pVer[VI+3].p = PolyCenter - XXX + YYY;pVer[VI+3].diffuse = Color;pVer[VI+3].tu1 = 0;pVer[VI+3].tv1 = 0;
		pVer[VI+4].p = PolyCenter + XXX - YYY;pVer[VI+4].diffuse = Color;pVer[VI+4].tu1 = 1;pVer[VI+4].tv1 = 1;
		pVer[VI+5].p = PolyCenter + XXX + YYY;pVer[VI+5].diffuse = Color;pVer[VI+5].tu1 = 1;pVer[VI+5].tv1 = 0;
		
		N++;
		i++;
	}
	
	m_lpVBuf->Unlock();
	return S_OK;
}

bool HaloCmp(KModelSFXHalo::HALO* S1,KModelSFXHalo::HALO* S2)
{
	return S1->Sequence > S2->Sequence;
}

HRESULT KModelSFXHalo::SortHalo()
{
	D3DXVECTOR3 CameraPos,CameraFront;
	
	g_cGraphicsTool.GetCameraPos  (&CameraPos);
	g_cGraphicsTool.GetCameraFront(&CameraFront);
	
	list<HALO*>::iterator i = m_HaloList.begin();
	while(i!=m_HaloList.end())
	{
		HALO* pHalo = *i;
		D3DXVECTOR3 Vec = pHalo->Position - CameraPos;
		pHalo->Sequence = D3DXVec3Dot(&Vec,&CameraFront);
		i++;
	}
	m_HaloList.sort();

	return S_OK;
}
};