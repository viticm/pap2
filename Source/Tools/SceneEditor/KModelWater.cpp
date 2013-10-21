// KModelWater.cpp: implementation of the KModelWater class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KModelWater.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
说明：
	程序中包含了，海浪（Sea类），水纹（Water类），涟漪（Ripple）三种效果，可叠加效果
	1、海浪：网格顶点坐标及normal值变化的计算基本上完善。
		单纯的网格效果看起来较好，加上渲染后效果不佳，渲染效果有待改进；
		算法效率可进一步改进；
		暂未计算纹理动态坐标位置等渲染效果；
		没有实现细浪的效果，目前的凹凸帖图是静态的。
	2、水纹：大体上完善。可通过修改Drop()函数的代码来改变水纹的效果。
		可进一步改进渲染效果；
		改进凹凸帖图同纹理动态坐标位置可实现更好的效果；
	3、Ripple：基本上完善。
*/
namespace Water
{


inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

KModelWater::KModelWater()
{
	m_pWaterUp = NULL;
	m_pWaterDn = NULL;
	m_pibIndices   = NULL;
	//	m_pEffect       = NULL;
	m_dwNumPoly_X = 0;
	m_dwNumPoly_Z = 0;
}

KModelWater::~KModelWater()
{

}

HRESULT KModelWater::LoadMesh(LPSTR pFileName)
{
	HRESULT hr = S_OK;

	m_dwNumPoly_X = 63;     //长方形网格的长，网格数
	m_dwNumPoly_Z = 63;     //长方形网格的宽，网格数

	LPD3DXMESH pMesh = m_pWaterUp;
	LPD3DXMESH pDMesh = m_pWaterDn;

	DWORD m_dNumPloy     = m_dwNumPoly_X * m_dwNumPoly_Z;
	DWORD m_dNumFaces    = m_dNumPloy * 2;
	DWORD m_dNumVertices = (m_dwNumPoly_X+1)*(m_dwNumPoly_Z+1);

	SAFE_RELEASE(pMesh);	
	SAFE_RELEASE(pDMesh);
	WORD *pwIndices;

	//建立水面网格 
	if(FAILED(hr = g_pd3dDevice->CreateIndexBuffer(m_dNumFaces * 3 * sizeof(WORD), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pibIndices, NULL)))
	{
		return hr;
	}

	if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED|D3DXMESH_32BIT,
		D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_NORMAL|D3DFVF_TEX1,g_pd3dDevice,&pMesh)))
	{
		return hr;
	}	
	if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED|D3DXMESH_32BIT,
		D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_NORMAL|D3DFVF_TEX1,g_pd3dDevice,&pDMesh)))
	{
		return hr;
	}




	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	DWORD* pIndex = NULL;
	DWORD * pAttrib = NULL;	

	VFormat::FACES_NORMAL_TEXTURE1 * pDVers = NULL;
	DWORD* pDIndex = NULL;
	DWORD * pDAttrib = NULL;	
	if(FAILED(hr = m_pibIndices->Lock(0, m_dNumFaces *3 * sizeof(WORD), (void**) &pwIndices, D3DLOCK_DISCARD)))
		return E_FAIL;
	if (FAILED(pMesh->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;
	if (FAILED(pMesh->LockIndexBuffer (0,(void**)&pIndex)))
		return E_FAIL;
	if (FAILED(pMesh->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		return E_FAIL;

	if (FAILED(pDMesh->LockVertexBuffer(0,(void**)&pDVers)))
		return E_FAIL;
	if (FAILED(pDMesh->LockIndexBuffer (0,(void**)&pDIndex)))
		return E_FAIL;
	if (FAILED(pDMesh->LockAttributeBuffer(0,(DWORD**)&pDAttrib)))
		return E_FAIL;

	DWORD i = 0;
	float _X = 1.0f/m_dwNumPoly_X;
	float _Z = 1.0f/m_dwNumPoly_Z;

	float m_fPolyWidth  = 200;
	float m_fPolyHeight = 200;

	for(DWORD X =0;X<=m_dwNumPoly_X;X++)
	{
		for(DWORD Y =0;Y<=m_dwNumPoly_Z;Y++)
		{
			float PX = X * m_fPolyWidth;
			float PZ = Y * m_fPolyHeight;

			D3DXVECTOR2 Pos(PX,PZ);

			pVers[i].p = D3DXVECTOR3(PX,200,PZ);

			pVers[i].Normal = D3DXVECTOR3(0,1,0);

			pVers[i].tu1 = (X * _X);
			pVers[i].tv1 = (1 - Y *_Z);
			pDVers[i].p = D3DXVECTOR3(PX,0,PZ);
			pDVers[i].Normal = D3DXVECTOR3(0,1,0);
			pDVers[i].tu1 = (X * _X);
			pDVers[i].tv1 = (1 - Y *_Z);
			i++;
		}
	}

	DWORD  Weight = m_dwNumPoly_X + 1;

	for(X =0;X<m_dwNumPoly_X;X++)
	{
		for(DWORD Y =0;Y<m_dwNumPoly_Z;Y++)
		{
			DWORD PloyIndex = Y*m_dwNumPoly_X +X;

			DWORD PolyMaterialID = 0;

			DWORD Vertex_A = X    *Weight+ Y;
			DWORD Vertex_B = (X+1)*Weight+ Y;
			DWORD Vertex_C = (X+1)*Weight+(Y+1);
			DWORD Vertex_D = X    *Weight+(Y+1);

			DWORD Faces_A1 = (PloyIndex*2)*3;
			DWORD Faces_B1 = Faces_A1 + 1;
			DWORD Faces_C1 = Faces_B1 + 1;

			pIndex[Faces_A1] = Vertex_A;
			pIndex[Faces_B1] = Vertex_B;
			pIndex[Faces_C1] = Vertex_D;
			pAttrib[PloyIndex*2] = PolyMaterialID;
			pDIndex[Faces_A1] = Vertex_A;
			pDIndex[Faces_B1] = Vertex_B;
			pDIndex[Faces_C1] = Vertex_D;
			pDAttrib[PloyIndex*2] = PolyMaterialID;

			pwIndices[Faces_A1] = WORD(Vertex_A);
			pwIndices[Faces_B1] = WORD(Vertex_B);
			pwIndices[Faces_C1] = WORD(Vertex_D);

			DWORD Faces_A2 = (PloyIndex*2+1)*3;
			DWORD Faces_B2 = Faces_A2 + 1;
			DWORD Faces_C2 = Faces_B2 + 1;

			pIndex[Faces_A2] = Vertex_D;
			pIndex[Faces_B2] = Vertex_B;
			pIndex[Faces_C2] = Vertex_C;
			pAttrib[PloyIndex*2+1] = PolyMaterialID;
			pDIndex[Faces_A2] = Vertex_D;
			pDIndex[Faces_B2] = Vertex_B;
			pDIndex[Faces_C2] = Vertex_C;
			pDAttrib[PloyIndex*2+1] = PolyMaterialID;

			pwIndices[Faces_A2] = WORD(Vertex_D);
			pwIndices[Faces_B2] = WORD(Vertex_B);
			pwIndices[Faces_C2] = WORD(Vertex_C);

		}
	}	

	D3DXComputeBoundingBox((D3DXVECTOR3*)pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
		&m_BBox_A,&m_BBox_B);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pDVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
		&m_BBox_A,&m_BBox_B);

	if (FAILED(pMesh->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(pMesh->UnlockIndexBuffer()))
		return E_FAIL;
	if (FAILED(pMesh->UnlockAttributeBuffer()))
		return E_FAIL;

	if (FAILED(pDMesh->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(pDMesh->UnlockIndexBuffer()))
		return E_FAIL;
	if (FAILED(pDMesh->UnlockAttributeBuffer()))
		return E_FAIL;
	if (FAILED(m_pibIndices->Unlock()))
		return E_FAIL;
	m_pWaterUp = pMesh;		
	m_pWaterDn = pDMesh;	


	//水面网格的材质
	m_dNumMaterial = 1;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);

	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_NONE|
		MATERIAL_OPTION_SPECULARENABLE;

	for(DWORD i=0;i<m_dNumMaterial;i++)
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
	wsprintf(Name,"%s\\Water.Mtl",g_Def_ModelDirectory);
	LoadMaterial(Name);  


	//天空盒的纹理
	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\LobbyCube.dds");
	if( FAILED( hr = D3DXCreateCubeTextureFromFile( g_pd3dDevice,Name, &m_pSkyCubeTex ) ) )
		return hr; 
	//水面的纹理
	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\Water.bmp");
	if( FAILED( hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, Name, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
		D3DX_DEFAULT, 0, NULL, NULL, &m_pWaterTex) ))
		return hr;
	/*
	// Effect
	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"water.fx");
	if(FAILED(hr = D3DXCreateEffectFromFile(g_pd3dDevice, Name, NULL, NULL, 0, NULL, &m_pEffect, NULL)))
	return hr;
	m_pEffect->OnResetDevice();
	m_pEffect->SetTexture("tFLR", m_pWaterTex);
	m_pEffect->SetTexture("tENV", m_pSkyCubeTex);
	*/

	//创建水面bump纹理
	if( FAILED( InitBumpMap() ) )
		return E_FAIL;
	//	m_pEffect->SetTexture("tBump", m_pBumpMapTexture);
	WSea.Initialize(m_dwNumPoly_X,m_dwNumPoly_Z);
	Water.Initialize(m_pWaterUp,m_dwNumPoly_X,m_dwNumPoly_Z);   //初始化水纹
	Water.Drop();                                               //产生水纹  
	Ripple.Initialize(m_pWaterUp,m_dwNumPoly_X,m_dwNumPoly_Z);  //初始化涟漪纹


	return S_OK;

}

HRESULT KModelWater::InitBumpMap()
{   //创建水面bump纹理	

	HRESULT hr;
	LPDIRECT3DTEXTURE9 psBumpSrc;
	D3DSURFACE_DESC    d3dsd;
	D3DLOCKED_RECT     d3dlr;
	TCHAR Name[256];

	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\Water.bmp");
	hr = D3DXCreateTextureFromFile(g_pd3dDevice, Name, &psBumpSrc);
	if (FAILED(hr))
		return hr;

	psBumpSrc->GetLevelDesc( 0, &d3dsd );
	// Create the bumpmap's surface and texture objects
	if( FAILED( g_pd3dDevice->CreateTexture( d3dsd.Width, d3dsd.Height, 1, 0, 
		D3DFMT_V8U8, D3DPOOL_MANAGED, &m_pBumpMapTexture,NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill the bits of the new texture surface with bits from
	// a private format.
	psBumpSrc->LockRect( 0, &d3dlr, 0, 0 );
	DWORD dwSrcPitch = (DWORD)d3dlr.Pitch;
	BYTE* pSrcTopRow = (BYTE*)d3dlr.pBits;
	BYTE* pSrcCurRow = pSrcTopRow;
	BYTE* pSrcBotRow = pSrcTopRow + (dwSrcPitch * (d3dsd.Height - 1) );

	m_pBumpMapTexture->LockRect( 0, &d3dlr, 0, 0 );
	DWORD dwDstPitch = (DWORD)d3dlr.Pitch;
	BYTE* pDstTopRow = (BYTE*)d3dlr.pBits;
	BYTE* pDstCurRow = pDstTopRow;


	for( DWORD y=0; y<d3dsd.Height; y++ )
	{
		BYTE* pSrcB0; // addr of current pixel
		BYTE* pSrcB1; // addr of pixel below current pixel, wrapping to top if necessary
		BYTE* pSrcB2; // addr of pixel above current pixel, wrapping to bottom if necessary
		BYTE* pDstT;  // addr of dest pixel;

		pSrcB0 = pSrcCurRow;

		if( y == d3dsd.Height - 1)
			pSrcB1 = pSrcTopRow;
		else
			pSrcB1 = pSrcCurRow + dwSrcPitch;

		if( y == 0 )
			pSrcB2 = pSrcBotRow;
		else
			pSrcB2 = pSrcCurRow - dwSrcPitch;

		pDstT = pDstCurRow;

		for( DWORD x=0; x<d3dsd.Width; x++ )
		{
			LONG v00; // Current pixel
			LONG v01; // Pixel to the right of current pixel, wrapping to left edge if necessary
			LONG vM1; // Pixel to the left of current pixel, wrapping to right edge if necessary
			LONG v10; // Pixel one line below.
			LONG v1M; // Pixel one line above.

			v00 = *(pSrcB0+0);

			if( x == d3dsd.Width - 1 )
				v01 = *(pSrcCurRow);
			else
				v01 = *(pSrcB0+4);

			if( x == 0 )
				vM1 = *(pSrcCurRow + (4 * (d3dsd.Width - 1)));
			else
				vM1 = *(pSrcB0-4);
			v10 = *(pSrcB1+0);
			v1M = *(pSrcB2+0);

			LONG iDu = (vM1-v01); // The delta-u bump value
			LONG iDv = (v1M-v10); // The delta-v bump value


			*pDstT++ = (BYTE)(iDu / 2);
			*pDstT++ = (BYTE)(iDv / 2);


			// Move one pixel to the right (src is 32-bpp)
			pSrcB0+=4;
			pSrcB1+=4;
			pSrcB2+=4;
		}

		// Move to the next line
		pSrcCurRow += dwSrcPitch;
		pDstCurRow += dwDstPitch;
	}

	m_pBumpMapTexture->UnlockRect(0);
	psBumpSrc->UnlockRect(0);

	return S_OK;
}
HRESULT KModelWater::Render()
{

	SetMaterial(0);     //设置水面材质
	
	//	DrawMeshSubset(m_pWaterDn,0);   //绘制水底

	g_pd3dDevice->SetTexture(0, m_pBumpMapTexture);		//设置水面bump
	g_pd3dDevice->SetTexture(1, m_pWaterTex);			//水面纹理
	g_pd3dDevice->SetTexture(2, m_pSkyCubeTex);         //天空盒的折射纹理

//	float K = sinf( timeGetTime() * -0.002f) * (1+1);   //使bump纹理波动，产生水波细纹的效果（此部分功能暂未完成）
//	float H = cosf( timeGetTime() * -0.002f) * (1+1);
	float K = 1.0f;
	float H = 0.0f;

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAP );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00,   F2DW(K) );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01,   F2DW(H) );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10,   F2DW(H) );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11,   F2DW(K) );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLSCALE,  F2DW(4.0f) );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLOFFSET, F2DW(4.0f) );  

	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2,  D3DTA_CURRENT );

	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,  D3DTOP_BLENDCURRENTALPHA );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG2,  D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT3);
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSW,   D3DTADDRESS_CLAMP );  

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE); 
	/*	UINT uPasses;
	m_pEffect->Begin(&uPasses, 0);

	for(UINT uPass = 0; uPass < uPasses; uPass++)
	{
	m_pEffect->Pass(uPass);  */
	m_pWaterUp->DrawSubset(0);
	/*	}

	m_pEffect->End();*/

	//每计数400次产生一个涟漪纹，（功能测试）
	static int count		  = 0;			
	if(count%400==0){
		Ripple.Drop();  
		count = 0;
	}
	count++;	

	return S_OK;
}

HRESULT KModelWater::FrameMove()
{
	int m_uVertices = (m_dwNumPoly_X+1)*(m_dwNumPoly_Z+1);
	int m_uIndices  = m_dwNumPoly_X*m_dwNumPoly_Z*2;

	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	VFormat::FACES_NORMAL_TEXTURE1 * pDVers = NULL;
	if (FAILED(m_pWaterUp->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;

	if (FAILED(m_pWaterDn->LockVertexBuffer(0,(void**)&pDVers)))
		return E_FAIL;

	int i=0;
	float m_fPolyWidth  = 200;
	float m_fPolyHeight = 200;

	int select;
	select=2;
	switch(select)
	{
	//海浪效果，（暂未做细浪纹效果）
	case 1:
		WSea.animate();  
		break;
	//水纹加涟漪纹的效果
	case 2:
		Water.Update();  
		Ripple.Update();
		break;
	//水纹效果
	case 3:
		Water.Update();
		break;

	default:
		break;
	}

/*
	static int count2= 0;			
	count2++;

	D3DXVECTOR4 fSin, fCos; 
	D3DXVECTOR4 vWaveDirX( 2.5f, 0.5f, -7.0f, -8.0f );
	D3DXVECTOR4 vWaveDirY( 1.0f, 1.5f, -7.0f, 1.0f );
	D3DXVECTOR4 fTime( (float)(count2)*0.75f, 0.0f, 0.0f, 0.0f);
	D3DXVECTOR4 vWaveSpeed( 0.2f, 0.15f, 0.4f, 0.4f );
	D3DXVECTOR4 vWaveHeight( 0.04f, 0.05f, 0.0025f, 0.0025f );

	D3DXVECTOR4 vAngle;
	D3DXVECTOR3 fs;
	D3DXVECTOR3 vW;
	float l;
	float p2;   
*/

	for(DWORD X =0;X<=m_dwNumPoly_X;X++)
	{
		for(DWORD Y =0;Y<=m_dwNumPoly_Z;Y++)
		{
			float PX = X * m_fPolyWidth;
			float PZ = Y * m_fPolyHeight;

			switch(select)
			{
			//海浪效果，（暂未做细浪纹效果）
			case 1:

				pVers[i].p.y = 100 +100* WSea.waterHeight[i];
				pVers[i].Normal = WSea.waterNormal[i];
				break;
			//水纹加涟漪
			case 2:

				pVers[i].p.y = 100 +Water.m_pSurface[i].fHeight + Ripple.m_pSurface[i].fHeight;
				pVers[i].Normal = Water.m_pSurface[i].vecNormal + Ripple.m_pSurface[i].vecNormal;
				D3DXVec3Normalize(&pVers[i].Normal,&pVers[i].Normal);
				pVers[i].tu1 = Water.m_pSurface[i].vecTex.x;
				pVers[i].tv1 = Water.m_pSurface[i].vecTex.y; 
				break;
			//水纹
			case 3:

				pVers[i].p.y = 100 +Water.m_pSurface[i].fHeight;
				pVers[i].Normal = Water.m_pSurface[i].vecNormal;
				pVers[i].tu1 = Water.m_pSurface[i].vecTex.x;
				pVers[i].tv1 = Water.m_pSurface[i].vecTex.y; 
				break;
/*
			//大浪效果，暂未完成
			case 4:
				vAngle.x = (modf(float(pVers[i].tu1 * vWaveDirX.x + pVers[i].tv1 * vWaveDirY.x + fTime.x * vWaveSpeed.x), &l)) * 2*3.14159265f;
				vAngle.y = modf(float(pVers[i].tu1 * vWaveDirX.y + pVers[i].tv1 * vWaveDirY.y + (fTime.y * vWaveSpeed.y)),&l) * 2*3.14159265f;
				vAngle.z = modf(float(pVers[i].tu1 * vWaveDirX.z + pVers[i].tv1 * vWaveDirY.z + (fTime.x * vWaveSpeed.z)),&l) * 2*3.14159265f;
				vAngle.w = modf(float(pVers[i].tu1 * vWaveDirX.w + pVers[i].tv1 * vWaveDirY.w + (fTime.x * vWaveSpeed.w)),&l) * 2*3.14159265f;

				fSin.x = sin( vAngle.x );
				fSin.y = sin( vAngle.y );
				fSin.z = sin( vAngle.z );
				fSin.w = sin( vAngle.w );
				fCos.x = cos( vAngle.x );
				fCos.y = cos( vAngle.y );
				fCos.z = cos( vAngle.z );
				fCos.w = cos( vAngle.w );

				p2  = (float)(2.0f * max(fabs(X - 0.5f), fabs(Y - 0.5f)));

				fSin = fSin * (1-p2)*0.2 ;
				fCos = fCos * (1-p2)*0.2 ;


				fs.x = fSin.x;
				fs.y = fSin.y;
				fs.z = fSin.z;
				vW.x = vWaveHeight.x;
				vW.y = vWaveHeight.y;
				vW.z = vWaveHeight.z
					;
				pVers[i].p = pVers[i].p + D3DXVec4Dot(&fSin, &vWaveHeight) * pVers[i].Normal;



				break;
*/

			default:
				return FALSE;
				break;
			}
			i++;
		}
	}

	if (FAILED(m_pWaterUp->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(m_pWaterDn->UnlockVertexBuffer()))
		return E_FAIL;
	return S_OK;
}

HRESULT KModelWater::CleanUp()
{
	SAFE_RELEASE(m_pWaterUp);
	SAFE_RELEASE(m_pWaterDn);
	SAFE_RELEASE(m_pibIndices);
	//	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pSkyCubeTex);
	SAFE_RELEASE(m_pWaterTex);
	SAFE_RELEASE(m_pBumpMapTexture);

	WSea.release();

	return S_OK;
}
};
