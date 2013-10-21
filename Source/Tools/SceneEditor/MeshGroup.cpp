// MeshGroup.cpp: implementation of the KModelGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "MeshGroup.h"

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
KModelGroup::KModelGroup()
{
	m_pTexture = NULL; // Textures for Render targer mesh
	m_pVerBuffer = NULL;
	m_lpSurfaceDep = NULL;
	
	m_TextureWidth = 256;
	CameraDistance = -300;
	m_BaseColor = 0x00000000;
	Length = 100.0f; 
}

KModelGroup::~KModelGroup()
{

}

HRESULT KModelGroup::LoadMesh(LPSTR pFileName)
{
	HRESULT hr = S_OK;
	if (FAILED(hr = KModel::LoadMesh(pFileName)))
	{
		return hr;
	}
	
	D3DXVECTOR3 AB = (m_BBox_B - m_BBox_A);	
	float L = D3DXVec3Length(&AB);
	CameraDistance = -1.5f*L;
	Length = L;
	
	DWORD m_dwNumObject = 400;
	
	//////////////////////////////////////////////////////////////////////////
	LPMOTION pMotion = new MOTION;
	m_lpMotion.push_back(pMotion);
	pMotion = new _MotionCircle;
	m_lpMotion.push_back(pMotion);

	DWORD Row =0,K =0;
	DWORD RowLength = (DWORD) sqrt((float)(m_dwNumObject));

	for(DWORD i=0;i<m_dwNumObject;i++)
	{
		//m_lpObjects[i] = new OBJECT;

		K++;
		float X = (rand()%1000)*0.001f*1000 - 500;
		float Y = (rand()%1000)*0.001f*1000 - 500;
		
		if (K>RowLength)
		{
			Row++;
			K=0;
		}
		X = Row * 100.0f - RowLength*50 + (rand()%1000)*0.001f*100;
		Y = K   * 100.0f - RowLength*50 + (rand()%1000)*0.001f*100;
		float Size = 100;

		D3DXVECTOR3 Position = D3DXVECTOR3(X,100,Y);

		LPSPRITE pSpr = new SPRITE;

		pSpr->Width       = Size ;
		pSpr->Height = Size;
		pSpr->MotionType = 0;
		pSpr->Position   = Position;
		pSpr->Arg[0] = Position.x;
		pSpr->Arg[1] = Position.y;
		pSpr->Arg[2] = Position.z;

		m_lpSprite.push_back(pSpr);
		m_nNumFaces = 2*m_lpSprite.size();

	}

	
	/*DWORD Size = 6*sizeof(OBJECTVERTEX)*m_dwNumObject;
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( Size ,
         0, D3DFVF_OBJECTVERTEX, D3DPOOL_MANAGED, &m_pVerBuffer,NULL ) ) )
		 return E_FAIL;*/
	CreateVerticesBuffer();

	if ( FAILED(hr = g_pd3dDevice->CreateDepthStencilSurface(m_TextureWidth,m_TextureWidth,
		D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,
		TRUE,
		&m_lpSurfaceDep,NULL)))
		return hr;

	if ( FAILED(hr = D3DXCreateTexture(g_pd3dDevice,
		m_TextureWidth,m_TextureWidth,
		4,D3DUSAGE_RENDERTARGET|D3DUSAGE_AUTOGENMIPMAP,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pTexture)))
		return hr;

	m_nBillBoardMethod = BILLBOARD_POSITION_ONLY;
	
	wsprintf(m_szMeshFileName,"Group");
	UpdateVertices();

	return S_OK;
}

HRESULT KModelGroup::Render()
{
	
	g_pd3dDevice->SetTexture( 0, m_pTexture );
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x20);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	SetMatrix();
	
	g_pd3dDevice->SetStreamSource( 0, m_lpVBuf,0, sizeof( VFormat::FACES_DIFFUSE_TEXTURE1 ) );
	g_pd3dDevice->SetFVF( D3DFVF_Faces_Diffuse_Texture1 );
	g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 , m_nNumFaces );

	RestoreMatrix();

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	return S_OK;
}

HRESULT KModelGroup::FrameMove()
{
	//sort for alpha blend
	//Sort();
	
	Render2Texture();
	
	/*OBJECTVERTEX* pVertices;
	if ( FAILED( m_pVerBuffer->Lock( 0, sizeof(OBJECT)*m_dwNumObject*6,
		(void**)&pVertices, D3DLOCK_DISCARD  ) ) )
		return E_FAIL;*/
	D3DXVECTOR3 Axis_Left,Axis_Up;
	g_cGraphicsTool.GetCameraLeftUp(&Axis_Left,&Axis_Up);
	Axis_Left *= Length;
	Axis_Up   *= Length;


	float th = timeGetTime()*0.0005f;
	D3DXVECTOR3 LightPos(1000*cosf(th),0,1000*sinf(th));
	
	/*for(DWORD i=0;i<m_dwNumObject;i++)
	{
		D3DCOLOR Color = 0xAAAAAAAA;
		LPOBJECT pPat = m_lpObjects[i];
		
		D3DXVECTOR3 OL = LightPos - pPat->Position;
		OL.y = 0;
		float LS = D3DXVec3LengthSq(&OL);
		if (LS<=160000)
		{
			float L = sqrtf(LS);
			WORD R = 255 - (WORD)(L/400.0f*255) + 0xAA; 
			if (R>255) R = 255;
			Color = RGB(R,R,R);
		}

		D3DXVECTOR3 Left,Up;

		Left = Axis_Left * pPat->Size;
		Up   = Axis_Up   * pPat->Size;

		/*Left = Axis_Left * sinf(pPat->Angle+1.5708f) + Axis_Up * cosf(pPat->Angle+1.5708f);
		Up = Axis_Left * sinf(pPat->Angle) + Axis_Up * cosf(pPat->Angle);*/

		/*pVertices[i*6  ].position = pPat->Position - Left + Up ;
		pVertices[i*6  ].tu = 0;pVertices[i*6  ].tv = 0;pVertices[i*6  ].Color = Color;
		pVertices[i*6+1].position = pPat->Position + Left + Up ;
		pVertices[i*6+1].tu = 1;pVertices[i*6+1].tv = 0;pVertices[i*6+1].Color = Color;
		pVertices[i*6+2].position = pPat->Position - Left - Up;
		pVertices[i*6+2].tu = 0;pVertices[i*6+2].tv = 1;pVertices[i*6+2].Color = Color;
		pVertices[i*6+3].position = pPat->Position - Left - Up;
		pVertices[i*6+3].tu = 0;pVertices[i*6+3].tv = 1;pVertices[i*6+3].Color = Color;
		pVertices[i*6+4].position = pPat->Position + Left + Up ;
		pVertices[i*6+4].tu = 1;pVertices[i*6+4].tv = 0;pVertices[i*6+4].Color = Color;
		pVertices[i*6+5].position = pPat->Position + Left - Up;
		pVertices[i*6+5].tu = 1;pVertices[i*6+5].tv = 1;pVertices[i*6+5].Color = Color;
	}

	m_pVerBuffer->Unlock();*/
	UpdateVertices();

	return S_OK;
}

HRESULT KModelGroup::Render2Texture()
{
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
	if (FAILED(hr = m_pTexture->GetSurfaceLevel(0,&lpSurface)))
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

	vEyePt = Front * CameraDistance;
	vLookatPt = (m_BBox_A+m_BBox_B)*0.5f;
	
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

	KModel::Render();
	
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
	m_pTexture->GenerateMipSubLevels();

	return S_OK;
}

//DEL HRESULT KModelGroup::Sort()
//DEL {
//DEL 	/*LPOBJECT pObject_i,pObject_j;
//DEL 	
//DEL 	D3DXVECTOR3 CameraPos(0,0,0);
//DEL 	D3DXVECTOR3 CameraFront;
//DEL 	g_cGraphicsTool.GetCameraFront(&CameraFront);
//DEL 	g_cGraphicsTool.GetCameraPos(&CameraPos);
//DEL 
//DEL 	for(DWORD i=0;i<m_dwNumObject;i++)
//DEL 	{
//DEL 		pObject_i = m_lpObjects[i];
//DEL 
//DEL 		D3DXVECTOR3 PO = m_lpObjects[i]->Position - CameraPos;
//DEL 		pObject_i->Distance = D3DXVec3Dot(&PO,&CameraFront);
//DEL 	}
//DEL 
//DEL 	for( i=0;i<m_dwNumObject;i++)
//DEL 	{
//DEL 		pObject_i = m_lpObjects[i];
//DEL 		for(DWORD j=i+1;j<m_dwNumObject;j++)
//DEL 		{
//DEL 			pObject_j = m_lpObjects[j];
//DEL 			if (pObject_j->Distance > pObject_i->Distance)
//DEL 			{
//DEL 				LPOBJECT Temp  = m_lpObjects[i];
//DEL 				m_lpObjects[i] = m_lpObjects[j];
//DEL 				m_lpObjects[j] = Temp;
//DEL 			}
//DEL 		}
//DEL 	}*/
//DEL 
//DEL 	return S_OK;
//DEL }
};
