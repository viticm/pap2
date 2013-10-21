#include "StdAfx.h"
#include "KG3DTerrainRender.h"
#include "KG3DSceneBlock.h"
#include "KG3DTerrainBlock.h"
#include "KG3DTemplates.h"
#include "KG3DTextureTable.h"
#include "KG3DTerrainDetailTexture.h"
#include "KG3DWaterEffect.h"
#include "KG3DTerrainGrassesRender.h"
#include "KG3DTerrainBlockGrass.h"
#include "ocean_surface.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DTerrainRoad.h"
#include "Terrain/KG3DTerrainRiver.h"
#include "KG3DSceneShadowMap.h"
#include "KG3DScenePointLightRender.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DGraphicsTool.h"

extern TKG3DResourcePool<KG3DTerrainBlockBlendFactorShareVB>g_cTerrainBlockBlendFactorShareVB;
extern int g_nGrassVertexBufferFillPerFrame;
extern BOOL g_bRenderBlockEdge;
extern BOOL g_bRenderWaterEdge;
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void GetSceneBlockRect(int nLevel,int nX,int nZ,D3DXVECTOR2& A,D3DXVECTOR2 &C);
BOOL g_bAlwayRenderWater;

KG3DTerrainRender::KG3DTerrainRender(void)
	:m_dwMaskTexID(0)
	,m_bRenderMask(FALSE)
	,m_bCheckVisibleCount(TRUE)
{
	ZeroMemory(m_lpLevel0Index,sizeof(LPDIRECT3DINDEXBUFFER9)*_countof(m_lpLevel0Index));
	ZeroMemory(m_lpLevel1Index,sizeof(LPDIRECT3DINDEXBUFFER9)*_countof(m_lpLevel1Index));
	ZeroMemory(m_lpLevel2Index,sizeof(LPDIRECT3DINDEXBUFFER9)*_countof(m_lpLevel2Index));
	m_lpLevel3Index = NULL;
	ZeroMemory(m_nLevel0FaceCount,sizeof(int)*_countof(m_nLevel0FaceCount));
	ZeroMemory(m_nLevel1FaceCount,sizeof(int)*_countof(m_nLevel1FaceCount));
	ZeroMemory(m_nLevel2FaceCount,sizeof(int)*_countof(m_nLevel2FaceCount));
	m_nLevel3FaceCount = 0;
 
	m_lpDeclaration_VertexOnly = NULL;
	m_lpDeclaration_Blend1 = NULL;
	//m_lpDeclaration_Blend2 = NULL;
	m_lpDeclaration_Water = NULL;

	//m_lpTerrainShader = NULL;
	//m_lpRoadShader = NULL; // add by huangjinshou 2008-6-10

	m_lpDefaultTextureDetail0 = NULL;
	m_lpDefaultTextureDetail1 = NULL;
	m_lpDefaultTextureDetail2 = NULL;
	m_lpDefaultTextureDetail3 = NULL;

	m_UVTransform[0]  = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[1]  = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[2]  = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[3]  = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[4]  = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[5]  = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[6]  = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[7]  = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[8]  = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[9]  = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[10] = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[11] = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[12] = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[13] = D3DXVECTOR4(0,0,0.0025F,0);
	m_UVTransform[14] = D3DXVECTOR4(0.0025F,0,0,0);
	m_UVTransform[15] = D3DXVECTOR4(0,0,0.0025F,0);

	ZeroMemory(m_MtlFactor,sizeof(D3DXVECTOR4) * 8 );

	m_lpMtlMgr = NULL;

	m_lpGrassRender = NULL;

	m_lpOceanSurfaceRender = NULL;

	m_lpConverMap = NULL;

    m_bWaterVisible = FALSE;

	m_lpEdgeIndex = NULL;
}

KG3DTerrainRender::~KG3DTerrainRender(void)
{
	UnInit();
}

HRESULT KG3DTerrainRender::Init()
{
	HRESULT hr = S_OK;
    HRESULT hResult = S_OK;
	///////////////////////////////////////////////////////////////////////////
	hr = LoadIndexBufferFromFile();
	if (FAILED(hr))
		g_OutputDebugInfo("KG3DTerrainRender LoadIndexBufferFromFile 失败");
    ///////////////////////////////////////////////////////////////////////////
	//CreateBlockIndexs();
	CreateBlockEdgeIndex();
	if(!m_lpGrassRender)
	{
		m_lpGrassRender = new KG3DTerrainGrassesRender;
		m_lpGrassRender->Init();
	}

	D3DVERTEXELEMENT9 Declaration1[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};
	hr = g_pd3dDevice->CreateVertexDeclaration(Declaration1, &m_lpDeclaration_VertexOnly);
	KGLOG_COM_PROCESS_ERROR(hr);

	D3DVERTEXELEMENT9 Declaration2[] = 
	{
		{0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0},
		{1, 0 , D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	hr = g_pd3dDevice->CreateVertexDeclaration(Declaration2, &m_lpDeclaration_Blend1);
	KGLOG_COM_PROCESS_ERROR(hr);

	/*D3DVERTEXELEMENT9 Declaration3[] = 
	{
		{0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0},
		{1, 0 , D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{2, 0 , D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};
	hr = g_pd3dDevice->CreateVertexDeclaration(Declaration3, &m_lpDeclaration_Blend2);
	KGLOG_COM_PROCESS_ERROR(hr);*/

	D3DVERTEXELEMENT9 Declaration4[] = 
	{
		{0, 0 , D3DDECLTYPE_FLOAT3  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0},
		{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
	hr = g_pd3dDevice->CreateVertexDeclaration(Declaration4, &m_lpDeclaration_Water);
	KGLOG_COM_PROCESS_ERROR(hr);


	InitShader();

	hr = g_cTextureTable.LoadResourceFromFile("data\\public\\defaultscene\\001.tga",0,TEXOP_RELOAD,(IKG3DResourceBase**)&m_lpDefaultTextureDetail0);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTextureTable.LoadResourceFromFile("data\\public\\defaultscene\\002.tga",0,TEXOP_RELOAD,(IKG3DResourceBase**)&m_lpDefaultTextureDetail1);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTextureTable.LoadResourceFromFile("data\\public\\defaultscene\\003.tga",0,TEXOP_RELOAD,(IKG3DResourceBase**)&m_lpDefaultTextureDetail2);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTextureTable.LoadResourceFromFile("data\\public\\defaultscene\\004.tga",0,TEXOP_RELOAD,(IKG3DResourceBase**)&m_lpDefaultTextureDetail3);
	KGLOG_COM_PROCESS_ERROR(hr);


	hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        KG_COM_RELEASE(m_lpDefaultTextureDetail3);
        KG_COM_RELEASE(m_lpDefaultTextureDetail2);
        KG_COM_RELEASE(m_lpDefaultTextureDetail1);
        KG_COM_RELEASE(m_lpDefaultTextureDetail0);

        KG_COM_RELEASE(m_lpDeclaration_Water);
     //   KG_COM_RELEASE(m_lpDeclaration_Blend2);
        KG_COM_RELEASE(m_lpDeclaration_Blend1);
        KG_COM_RELEASE(m_lpDeclaration_VertexOnly);
    }
	return hResult;
}

HRESULT KG3DTerrainRender::UnInit()
{
	for (int i=0; i<16;i++)
	{
		SAFE_RELEASE(m_lpLevel0Index[i]);
		SAFE_RELEASE(m_lpLevel1Index[i]);
		SAFE_RELEASE(m_lpLevel2Index[i]);
		m_nLevel0FaceCount[i] = 0;
		m_nLevel1FaceCount[i] = 0;
		m_nLevel2FaceCount[i] = 0;
	}
	SAFE_RELEASE(m_lpLevel3Index);
	m_nLevel3FaceCount = 0;
	/*for (int i=0;i<BLOCK_LOD_NUM;i++)
	{
		SAFE_RELEASE(m_lpBlockIndexBuffer[i]);
	}*/
	SAFE_RELEASE(m_lpEdgeIndex);

	KG_COM_RELEASE(m_lpDeclaration_Water);
   // KG_COM_RELEASE(m_lpDeclaration_Blend2);
	KG_COM_RELEASE(m_lpDeclaration_Blend1);
    KG_COM_RELEASE(m_lpDeclaration_VertexOnly);

	//SAFE_RELEASE(m_lpTerrainShader);
	//SAFE_RELEASE(m_lpRoadShader);//add by huangjinshou 2008-6-10

	KG_COM_RELEASE(m_lpDefaultTextureDetail0);
	KG_COM_RELEASE(m_lpDefaultTextureDetail1);
	KG_COM_RELEASE(m_lpDefaultTextureDetail2);
	KG_COM_RELEASE(m_lpDefaultTextureDetail3);

	SAFE_DELETE(m_lpGrassRender);

	m_lpMtlMgr = NULL;

	SAFE_DELETE(m_lpOceanSurfaceRender);
	m_lpConverMap = NULL;

	m_CullData.UnInit();
    m_RenderData.UnInit();
	

	CleanTerrainBlockGroup();
	return S_OK;
}

HRESULT KG3DTerrainRender::RenderTerrainBlocks_ConverMap(BOOL bAll)
{
	return m_RenderData.RenderTerrainBlocks_ConverMap(bAll,this);
}

HRESULT KG3DTerrainRenderData::RenderTerrainBlocks_ConverMap(BOOL bAll,KG3DTerrainRender* pRender)
{
    HRESULT hr = E_FAIL;

	GraphicsStruct::RenderState Zfun(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	GraphicsStruct::RenderState cw(D3DRS_CULLMODE,D3DCULL_CCW);

	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);
	{
		//这里一大堆东西移动到SetTerrainShaderParameters，这样就可以Shader参数统一起来了
		//Last modify by Chen TianHong:2008-8-8 10:13:26
		//////////////////////////////////////////////////////////////////////////

		hr = pTerrainEffect->SetTechnique("tec0");
		KGLOG_COM_CHECK_ERROR(hr);

		UINT uPass;
		pTerrainEffect->Begin(&uPass,0);
		pTerrainEffect->BeginPass(0);

		LPDIRECT3DINDEXBUFFER9 pIndex  = NULL;
		DWORD                  dwFaces = 0;
		
		g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_VertexOnly);

        for (size_t i = 0; i < m_vecTerrainBlockUnsort.size(); ++i)
        {
			KG3DTerrainBlock* pBlock = m_vecTerrainBlockUnsort[i];
			pRender->GetIndexBuffer(pBlock,pIndex,dwFaces);
			g_pd3dDevice->SetIndices(pIndex);

			LPDIRECT3DVERTEXBUFFER9 pVB = pBlock->GetGroundVertexBuffer();

			if(pVB)
			{
				g_pd3dDevice->SetStreamSource(0,pVB, 0, sizeof(VFormat::_Faces_Normal_Texture0)); 

				//pTerrainEffect->CommitChanges();
				//////////////////////////////////////////////////////////////////////////
				g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0,dwFaces);

			}
		}

		pTerrainEffect->EndPass();
		pTerrainEffect->End();


        return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRender::RenderTerrainBlocks_Detail(KG3DLightParam& lightParam,BOOL bGrass,BOOL bStone,BOOL bFrutex)
{
	return m_RenderData.RenderTerrainBlocks_Detail(lightParam,bGrass,bStone,bFrutex,this);
}

HRESULT KG3DTerrainRenderData::RenderTerrainBlocks_Detail(KG3DLightParam& lightParam,BOOL bGrass,BOOL bStone,BOOL bFrutex,KG3DTerrainRender* pRender)
{
	//GraphicsStruct::RenderState wid(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);

	pRender->SetShaderParameters(lightParam);

	if (pRender->m_bRenderMask)
		RenderMaskedTerrain(pRender);
	else
	{
		if(g_cEngineOption.nTerrainDetail > 0)
			RenderMapTerrainBlock_Low(pRender);//只用一张贴图渲染

	   if(g_cEngineOption.nTerrainDetail < 2)
			RenderMapTerrainBlock(pRender);//这个是渲染近处的
	}

	RenderTerrainBlocks_ConverMap(FALSE,pRender);//这个是渲染远处的	

	if (g_cEngineOption.bRenderGrass && g_cEngineManager.GetRenderOption( RENDER_GRASS ) && pRender->m_lpGrassRender)
	{
		pRender->m_lpGrassRender->RenderGrassColorOnly(pRender->m_lpConverMap,bGrass,bStone,bFrutex);

		if(g_cEngineOption.bGrassAlphaBlend)
			pRender->m_lpGrassRender->RenderGrassColoraAlphaBlendOnly(pRender->m_lpConverMap,bGrass,bStone,bFrutex);
	}

	RenderRoad(pRender);
	//RenderRiver();
	return S_OK;

Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRender::RenderTerrainBlocks_OnlyTexture()
{
	return m_RenderData.RenderTerrainBlocks_OnlyTexture(this);
}

HRESULT KG3DTerrainRenderData::RenderTerrainBlocks_OnlyTexture(KG3DTerrainRender* pRender)
{

	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);
	{
		HRESULT hr = E_FAIL;
		KG3DRenderState State;
		State.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		State.SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		State.SetRenderState(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
		State.SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		GraphicsStruct::RenderState rsAlphaBlend(D3DRS_ALPHABLENDENABLE, FALSE);
		GraphicsStruct::RenderState rsAlphaTest(D3DRS_ALPHATESTENABLE, FALSE);

		for (int i = 0; i < 8; i++)
		{
			State.SetSamplerState(i, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
			State.SetSamplerState(i, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
			State.SetSamplerState(i, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
			State.SetSamplerState(i, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
			State.SetSamplerState(i, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
			State.SetSamplerState(i, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		}
		{
			D3DXMATRIX matView,matProj,matViewProj;
			D3DXMATRIX matWorld;
			g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
			g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
			g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
			matViewProj = matView * matProj;

			pTerrainEffect->SetMatrix("matWVP",&matViewProj);
			pTerrainEffect->SetMatrix("matWV",&matView);
			pTerrainEffect->SetMatrix("matWorld",&matWorld);	
		}


		LPDIRECT3DINDEXBUFFER9 pIndex = NULL;
		DWORD dwFace = 0;

		{
			//////////////////////////////////////////////////////////////////////////
			TCHAR* pTechniqueName = "RenderTexture";

			hr = pTerrainEffect->SetTechnique(pTechniqueName);
			KGLOG_COM_CHECK_ERROR(hr);	

			UINT uPass;
			pTerrainEffect->Begin(&uPass,0);
			pTerrainEffect->BeginPass(0);
			g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_Blend1);
		}
		BOOL bSaveLod = g_cEngineOption.bUseTerrainLOD;
		g_cEngineOption.bUseTerrainLOD = FALSE;
		BOOL bUse = TRUE;
		for (UINT uGroup = 0; uGroup < BLOCK_GROUP_COUNT; ++uGroup)
		{
			if (uGroup >= 4 && bUse)
			{
				pTerrainEffect->EndPass();
				pTerrainEffect->BeginPass(1);
				bUse = FALSE;
			}

			unsigned long long ullPreMaterialKey = 0xFFFFFFFF;
			size_t uSize = m_aTerrainBlockGroup[uGroup].size();
			for (size_t i = 0; i < uSize; ++i)
			{
				KG3DTerrainBlock* pBlock = m_aTerrainBlockGroup[uGroup][i];
				_ASSERTE(NULL != pBlock);

				if (ullPreMaterialKey != pBlock->m_ullMaterialKey)
				{
					pRender->SetDetailTexture(pBlock); 
				}

				ullPreMaterialKey = pBlock->m_ullMaterialKey;

				pRender->GetIndexBuffer(pBlock,pIndex,dwFace);
				g_pd3dDevice->SetIndices(pIndex);
				//////////////////////////////////////////////////////////////////////////
				{
					LPDIRECT3DVERTEXBUFFER9 pVB_Vertex = pBlock->GetGroundVertexBuffer();
					LPDIRECT3DVERTEXBUFFER9 pVB_TexBlend1 =pBlock->GetBlendFactorBuffer();

					if(NULL != pVB_Vertex && NULL != pVB_TexBlend1)
					{
						g_pd3dDevice->SetStreamSource(0,pVB_Vertex   , 0, sizeof(VFormat::_Faces_Normal_Texture0)); 
						g_pd3dDevice->SetStreamSource(1,pVB_TexBlend1, 0, sizeof(float)); 
						g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0, dwFace);
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		pTerrainEffect->EndPass();
		pTerrainEffect->End();
		g_cEngineOption.bUseTerrainLOD = bSaveLod;
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRender::ReloadShaders()
{
	m_TerrainShader.Reload();
	m_RoadShader.Reload();
	return S_OK;
}

HRESULT KG3DTerrainRender::InitShader()
{
	HRESULT hr = S_OK;
	//SAFE_RELEASE(m_lpTerrainShader);
	//TCHAR rootPath[MAX_PATH];
	//TCHAR FileName[MAX_PATH];
	//g_GetRootPath(rootPath);

	//TerrainRender换成由ShaderManager来管理
	/*if(g_cEngineOption.bEnableGroundNormalMap)
		wsprintf(FileName,"Data\\public\\shader\\TerrainRender_normalmap.fx");
	else
		wsprintf(FileName,"Data\\public\\shader\\TerrainRender.fx");

	hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, FileName, 
		NULL, NULL, 0 ,
		NULL, &m_lpTerrainShader,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);*/

	DWORD dwTerrainShaderTypeUse = g_cEngineOption.bEnableGroundNormalMap ? DEFST_TERRAIN_NORMALMAP : DEFST_TERRAIN;
	m_TerrainShader = g_GetDefaultShaderGroup().GetDefaultShader(dwTerrainShaderTypeUse);

	//////Add by huangjinshou 2008-6-10
	//SAFE_RELEASE(m_lpRoadShader);
	//wsprintf(FileName,"Data\\public\\shader\\Road.fx");
	//hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, FileName, 
	//	NULL, NULL, 0 ,
	//	NULL, &m_lpRoadShader, NULL);	
	//KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	m_RoadShader = g_GetDefaultShaderGroup().GetDefaultShader(DEFST_TERRAIN_ROAD);
	///////////////////////////////////////////////////////////////
	hr = S_OK;
//Exit0:

	return hr ;
}

static float s_fViewGrassDistance = 12000.0f;

HRESULT KG3DTerrainRenderData::GroupTerrainBlock(KG3DTerrainBlock* pBlock)
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pBlock);

	if(pBlock->m_lpWaterVertex)
	{
		m_vecVertexWaterBlock.push_back(pBlock);
	}
	switch (g_cEngineOption.nTerrainDetail)
	{
	case 0 :
		{
			if( pBlock->m_nNumDetailTexture>0 && pBlock->m_nNumDetailTexture<=8)
			{
				m_aTerrainBlockGroup[pBlock->m_nNumDetailTexture - 1].push_back(pBlock);
			}
			else
			{
				m_vecTerrainBlockUnsort.push_back(pBlock);
			}
			break;
		}
	case 1 :
		{
			if(pBlock->m_nLevel == BLOCK_TYPE_SECTION)
			{
				m_vecTerrainBlockLow.push_back(pBlock);
			}
			else 
			{
				if( pBlock->m_nNumDetailTexture>0 && pBlock->m_nNumDetailTexture<=8)
				{
					m_aTerrainBlockGroup[pBlock->m_nNumDetailTexture - 1].push_back(pBlock);
				}
				else
				{
					m_vecTerrainBlockUnsort.push_back(pBlock);
				}
			}
			break;
		}
	case 2 :
	case 3 :
		m_vecTerrainBlockLow.push_back(pBlock);
		break ;
	default:
		_ASSERT(0);
	}
	
	hrResult = S_OK;
Exit0:
	return hrResult;
}

//HRESULT KG3DTerrainRender::PrepareRender(BOOL bUseFFTW)
//{
//	if(bUseFFTW && m_lpOceanSurfaceRender)
//	{
//		m_lpOceanSurfaceRender->updateHeightMapNew();
//		m_lpOceanSurfaceRender->updateGradientMap();
//	}
//	return S_OK;
//}

HRESULT KG3DTerrainRender::PostRender()
{
	//LPD3DXEFFECT pTerrainEffect = m_TerrainShader.GetEffect();
    //if (pTerrainEffect)
    //{
    //    pTerrainEffect->SetTexture("tDetail0", NULL);
    //    pTerrainEffect->SetTexture("tDetail1", NULL);
    //    pTerrainEffect->SetTexture("tDetail2", NULL);
    //    pTerrainEffect->SetTexture("tDetail3", NULL);
    //    pTerrainEffect->SetTexture("tDetail4", NULL);
    //    pTerrainEffect->SetTexture("tDetail5", NULL);
    //    pTerrainEffect->SetTexture("tDetail6", NULL);
    //    pTerrainEffect->SetTexture("tDetail7", NULL);
    //}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetTexture(2,NULL);
		g_pd3dDevice->SetTexture(3,NULL);
		g_pd3dDevice->SetTexture(4,NULL);
		g_pd3dDevice->SetTexture(5,NULL);
		g_pd3dDevice->SetTexture(6,NULL);
		g_pd3dDevice->SetTexture(7,NULL);
	}
    return S_OK;
}

HRESULT KG3DTerrainRender::OnLostDevice()
{
    HRESULT hr = S_OK;

	//<ConverMap Shared>
	//LPD3DXEFFECT pTerrainEffect = m_TerrainShader.GetEffect();

  //  if (pTerrainEffect)
  //  {
		//pTerrainEffect->SetTexture("tConver", NULL);//去掉贴图引用
		//pTerrainEffect->SetTexture("tMask", NULL);
  //  }

    if (m_lpOceanSurfaceRender)
    {
        hr = m_lpOceanSurfaceRender->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (m_lpGrassRender)
    {
        hr = m_lpGrassRender->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	{
		m_CullData.UnInit();
	}
Exit0:
    return hr;
}

HRESULT KG3DTerrainRender::OnResetDevice()
{
    HRESULT hr = S_OK;

    if (m_lpGrassRender)
    {
        hr = m_lpGrassRender->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    /*if (m_lpRoadShader)
    {
        hr = m_lpRoadShader->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }*/

    if (m_lpOceanSurfaceRender)
    {
        hr = m_lpOceanSurfaceRender->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    return hr;
}

HRESULT KG3DTerrainRender::CleanTerrainBlockGroup()
{
	return m_RenderData.CleanTerrainBlockGroup();
}

HRESULT KG3DTerrainRenderData::CleanTerrainBlockGroup()
{
	m_vecTerrainBlockLow.clear();

	for (int i = 0; i < BLOCK_GROUP_COUNT; i++)
	{
		m_aTerrainBlockGroup[i].clear();
	}
	return S_OK;
}

HRESULT KG3DTerrainRender::SetShaderParameters(const KG3DLightParam& lightParam)
{
	LPD3DXEFFECT pTerrainEffect = m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);
	LPD3DXEFFECT pRoadEffect = m_RoadShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pRoadEffect);

	//矩阵
	{
		D3DXMATRIX matView,matProj,matViewProj;
		D3DXMATRIX matWorld;

		g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
		matViewProj = matView * matProj;

		pTerrainEffect->SetMatrix("matWVP",&matViewProj);
		pTerrainEffect->SetMatrix("matWV",&matView);
		pTerrainEffect->SetMatrix("matWorld",&matWorld);


		pRoadEffect->SetMatrix("matWV", &matView);
		pRoadEffect->SetMatrix("matWVP", &matViewProj);
		pRoadEffect->SetMatrix("matWorld", &matWorld);
		
	}
	{
		//g_SetShadowInfo(pRoadEffect);//<ShadowParamMoveToShared>
		//g_SetShadowInfo(pTerrainEffect);//<ShadowParamMoveToShared>

		pRoadEffect->SetVector("LightAmbient" ,(D3DXVECTOR4*)&lightParam.sceneAmbient);
		pRoadEffect->SetVector("LightDiffuse" ,(D3DXVECTOR4*)&lightParam.diffuse);
		pRoadEffect->SetVector("LightSpecular",(D3DXVECTOR4*)&lightParam.specular);
		pRoadEffect->SetVector("LightDir",(D3DXVECTOR4*)&lightParam.dir);
	}

	/*if(m_lpConverMap)
	{
		pTerrainEffect->SetVector("vConverRect",&m_lpConverMap->GetRect());
		pTerrainEffect->SetTexture("tConver",m_lpConverMap->GetRenderTargetTex());

		pRoadEffect->SetVector("vConverRect",&m_lpConverMap->GetRect());
		pRoadEffect->SetTexture("tConver",m_lpConverMap->GetRenderTargetTex());
		
	}*///<ConverMap Shared>

	//下面两个都改成shared
	/*KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
	if (pCamera)
	{
		D3DXVECTOR3 vEyes;

		pCamera->GetPosition(&vEyes);
		pTerrainEffect->SetValue("eyes", &vEyes, sizeof(D3DXVECTOR3));
		pRoadEffect->SetValue("eyes", &vEyes, sizeof(D3DXVECTOR3));
	}*/

	/*IEKG3DScene* pCurScene = g_cGraphicsTool.GetScene();
	if(pCurScene)
	{
		KG3DScene* pSceneSolid = dynamic_cast<KG3DScene*>(pCurScene);
		KG_PROCESS_ERROR(pSceneSolid);

		pTerrainEffect->SetTexture("tEnv",pSceneSolid->GetEnvMap(NULL, 0));
	}*/

	pTerrainEffect->CommitChanges();
	//////////////////////////////////////////////////////////////////////////
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRender::RenderMapTerrainBlock()
{
	return m_RenderData.RenderMapTerrainBlock(this);
}

HRESULT KG3DTerrainRenderData::RenderMapTerrainBlock(KG3DTerrainRender* pRender)
{
    HRESULT hr = E_FAIL;
	KG3DRenderState State;	
	State.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	State.SetRenderState(D3DRS_ZFUNC,D3DCMP_EQUAL);
	State.SetRenderState(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
	State.SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	GraphicsStruct::RenderState rsAlphaBlend(D3DRS_ALPHABLENDENABLE, FALSE);
	GraphicsStruct::RenderState rsAlphaTest(D3DRS_ALPHATESTENABLE, FALSE);
	//GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);//这里不应该控制雾啊
	for (int i = 0; i < 8; i++)
	{
		State.SetSamplerState(i, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		State.SetSamplerState(i, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		State.SetSamplerState(i, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		State.SetSamplerState(i, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
		State.SetSamplerState(i, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		State.SetSamplerState(i, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	}

	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);

	if(pRender->m_lpConverMap) // 为了让高精度和低精度自动过渡 by Likan
	{
		if(g_cEngineOption.nTerrainDetail == 1)
		{
			//如果不是按高精度贴地形，按中等精度，也就是高低混合的情况
            pTerrainEffect->SetBool("blendHighNLow", true);
			pTerrainEffect->SetTexture("tTerrainMap",pRender->m_lpConverMap->GetTerrainMap());
			//IEKG3DGraphicsTool *graphicTool;
			//g_cEngineManager.GetGraphicsTool(&graphicTool);
			//IEKG3DCamera* camera = graphicTool->GetCamera();
			//D3DXVECTOR3 eyepos;
			//camera->GetPosition(&eyepos);
			//pTerrainEffect->SetValue("eyePos", &eyepos, sizeof(D3DXVECTOR3));
		}
		else
		{
			pTerrainEffect->SetBool("blendHighNLow", false);
			pTerrainEffect->SetTexture("tTerrainMap",0);
		}
	}

	LPDIRECT3DINDEXBUFFER9 pIndex = NULL;
	DWORD dwFace = 0;
	
	{
		//////////////////////////////////////////////////////////////////////////
		TCHAR* pTechniqueName = "DetailBlend";

		hr = pTerrainEffect->SetTechnique(pTechniqueName);
		KGLOG_COM_CHECK_ERROR(hr);	

		UINT uPass;
		pTerrainEffect->Begin(&uPass,0);
		pTerrainEffect->BeginPass(0);
		g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_Blend1);
	}
   
	BOOL bUse = TRUE;
    for (UINT uGroup = 0; uGroup < BLOCK_GROUP_COUNT; ++uGroup)
    {
        if (uGroup >= 4 && bUse)
        {
			pTerrainEffect->EndPass();
			pTerrainEffect->BeginPass(1);
			bUse = FALSE;
        }

		unsigned long long ullPreMaterialKey = 0xFFFFFFFF;
        size_t uSize = m_aTerrainBlockGroup[uGroup].size();
        for (size_t i = 0; i < uSize; ++i)
        {
            KG3DTerrainBlock* pBlock = m_aTerrainBlockGroup[uGroup][i];
            _ASSERTE(NULL != pBlock);
            
            if ((i == 0) || (ullPreMaterialKey != pBlock->m_ullMaterialKey))
            {
                g_ProjTextureRender.SetEffectParam(pTerrainEffect);
                pRender->SetDetailTexture(pBlock); 
            }

            ullPreMaterialKey = pBlock->m_ullMaterialKey;

			pRender->GetIndexBuffer(pBlock,pIndex,dwFace);
			g_pd3dDevice->SetIndices(pIndex);
			//////////////////////////////////////////////////////////////////////////
			g_SetPointLighting(pTerrainEffect,pBlock->m_lpPointLightInfo);

            //if(uGroup < 4)//这个时候不需要第二组BlendFactor
            {
                LPDIRECT3DVERTEXBUFFER9 pVB_Vertex = pBlock->GetGroundVertexBuffer();
                LPDIRECT3DVERTEXBUFFER9 pVB_TexBlend1 =pBlock->GetBlendFactorBuffer(/*0*/);

                if(NULL != pVB_Vertex && NULL != pVB_TexBlend1)
                {
                    g_pd3dDevice->SetStreamSource(0,pVB_Vertex   , 0, sizeof(VFormat::_Faces_Normal_Texture0)); 
                    g_pd3dDevice->SetStreamSource(1,pVB_TexBlend1, 0, sizeof(float/*D3DXVECTOR4*/)); 
                    g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0, dwFace);
                }
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
	pTerrainEffect->EndPass();
	pTerrainEffect->End();
	
	RenderBlockEdge(pRender);
		
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRender::RenderMapTerrainBlock_Low()
{
	return m_RenderData.RenderMapTerrainBlock_Low(this);
}

HRESULT KG3DTerrainRenderData::RenderMapTerrainBlock_Low(KG3DTerrainRender* pRender)
{
	HRESULT hr = E_FAIL;
	KG3DRenderState State;	
	State.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	State.SetRenderState(D3DRS_ZFUNC,D3DCMP_EQUAL);
	State.SetRenderState(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
	State.SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	GraphicsStruct::RenderState rsAlphaBlend(D3DRS_ALPHABLENDENABLE, FALSE);
	GraphicsStruct::RenderState rsAlphaTest(D3DRS_ALPHATESTENABLE, FALSE);
	GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);//这里不应该控制雾啊
	//for (int i = 0; i < 1; i++)
	{
		State.SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		State.SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		State.SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		State.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
		State.SetSamplerState(0, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		State.SetSamplerState(0, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	}

	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);

	LPDIRECT3DINDEXBUFFER9 pIndex = NULL;
	DWORD dwFace = 0;

	{
		//////////////////////////////////////////////////////////////////////////
		//TCHAR* pTechniqueName = bBlend ? "DetailBlend" : "Detail";

		hr = pTerrainEffect->SetTechnique("OneMap");
		KGLOG_COM_CHECK_ERROR(hr);	

		UINT uPass;
		pTerrainEffect->Begin(&uPass,0);
		pTerrainEffect->BeginPass(0);
		g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_VertexOnly);
		g_ProjTextureRender.SetEffectParam(pTerrainEffect);
		if(pRender->m_lpConverMap)
		  g_pd3dDevice->SetTexture(0,pRender->m_lpConverMap->GetTerrainMap());
	}

	size_t uSize = m_vecTerrainBlockLow.size();
	for (size_t i = 0; i < uSize; ++i)
	{
		KG3DTerrainBlock* pBlock = m_vecTerrainBlockLow[i];
		_ASSERTE(NULL != pBlock);

		pRender->GetIndexBuffer(pBlock,pIndex,dwFace);
		g_pd3dDevice->SetIndices(pIndex);
		//////////////////////////////////////////////////////////////////////////
		g_SetPointLighting(pTerrainEffect,pBlock->m_lpPointLightInfo);
		{
			LPDIRECT3DVERTEXBUFFER9 pVB_Vertex = pBlock->GetGroundVertexBuffer();
			if(NULL != pVB_Vertex )
			{
				g_pd3dDevice->SetStreamSource(0,pVB_Vertex   , 0, sizeof(VFormat::_Faces_Normal_Texture0)); 
				g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0, dwFace);
			}
		 }        
	}
	
	//////////////////////////////////////////////////////////////////////////
	pTerrainEffect->EndPass();
	pTerrainEffect->End();


	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainRenderData::RenderMaskedTerrain(KG3DTerrainRender* pRender)
{
	KG3DTexture* pTexture = NULL;	
	HRESULT hr = g_cTextureTable.GetResource(&pTexture, pRender->m_dwMaskTexID);
	
	KG3DRenderState State;
	State.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	State.SetRenderState(D3DRS_ZFUNC,D3DCMP_EQUAL);
	State.SetRenderState(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
	State.SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	GraphicsStruct::RenderState rsAlphaBlend(D3DRS_ALPHABLENDENABLE, FALSE);
	GraphicsStruct::RenderState rsAlphaTest(D3DRS_ALPHATESTENABLE, FALSE);

	KG_COM_PROCESS_ERROR(hr);
	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);

	LPDIRECT3DINDEXBUFFER9 pIndex = NULL;
	DWORD dwFace = 0;
	

	{
		//这里把Effect参数的设置提到外面了，多个渲染函数可以共享
		//一个参数设置函数，一次设置，不用多次了。Last modify by Chen TianHong:2008-8-8 10:27:48
		
		g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_VertexOnly);
		//g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
		pTerrainEffect->SetTexture("tMask", pTexture->GetTexture());
		pTerrainEffect->SetVector("vSceneRect", &pRender->m_vSceneRect);
		//////////////////////////////////////////////////////////////////////////
		TCHAR* pTechniqueName = "tec0";
		hr = pTerrainEffect->SetTechnique(pTechniqueName);
		KG_COM_CHECK_ERROR(hr);	

		UINT uPass;
		hr = pTerrainEffect->Begin(&uPass,0);
		KG_COM_PROCESS_ERROR(hr);
		hr = pTerrainEffect->BeginPass(1);
		if (FAILED(hr))
		{
			pTerrainEffect->End();
			goto Exit0;
		}
	}

    //for (UINT uGroup = 0; uGroup < BLOCK_GROUP_COUNT; ++uGroup)
    {
       // size_t uSize = m_aTerrainBlockGroup[uGroup].size();
		size_t uSize = m_vecTerrainBlock.size();
        for (size_t i = 0; i < uSize; ++i)
        {
            KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];
            _ASSERTE(NULL != pBlock);
			pRender->GetIndexBuffer(pBlock,pIndex,dwFace);
			g_pd3dDevice->SetIndices(pIndex);
            LPDIRECT3DVERTEXBUFFER9 pVB_Vertex = pBlock->GetGroundVertexBuffer();
            LPDIRECT3DVERTEXBUFFER9 pVB_TexBlend1 = pBlock->GetBlendFactorBuffer(/*0*/);

            if(NULL != pVB_Vertex && NULL != pVB_TexBlend1)
            {
                g_pd3dDevice->SetStreamSource(0,pVB_Vertex   , 0, sizeof(VFormat::_Faces_Normal_Texture0)); 
                g_pd3dDevice->SetStreamSource(1,pVB_TexBlend1, 0, sizeof(D3DXVECTOR4)); 
                g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0, dwFace );
            }
        }
    }
	
	pTerrainEffect->EndPass();
	pTerrainEffect->End();

	SAFE_RELEASE(pTexture);
	return S_OK;
Exit0:
	SAFE_RELEASE(pTexture);
	return E_FAIL;
}

HRESULT KG3DTerrainRender::SetDetailTexture(KG3DTerrainBlock* pBlock)
{
	BYTE byteIndex = 0;
	KG3DTerrainDetailTexture* pTexture = NULL;

	LPD3DXEFFECT pTerrainEffect = m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);
	KG_PROCESS_ERROR(pBlock);
	KG_PROCESS_ERROR(m_lpMtlMgr);
	KG_PROCESS_ERROR(g_pd3dDevice);
	

	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[0];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);
	
	if(pTexture)
	{
		if (pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail0",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(0,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[0] = pTexture->m_vUT;
		m_UVTransform[1] = pTexture->m_vVT;

		m_MtlFactor[0] = pTexture->GetMtlFactor();

		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal0",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 1 );
	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[1];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail1",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(1,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[2] = pTexture->m_vUT;
		m_UVTransform[3] = pTexture->m_vVT;

		m_MtlFactor[1] = pTexture->GetMtlFactor();

		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal1",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 2 );
	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[2];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail2",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(2,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[4] = pTexture->m_vUT;
		m_UVTransform[5] = pTexture->m_vVT;

		m_MtlFactor[2] = pTexture->GetMtlFactor();

		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal2",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 3 );
	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[3];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail3",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(3,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[6] = pTexture->m_vUT;
		m_UVTransform[7] = pTexture->m_vVT;

		m_MtlFactor[3] = pTexture->GetMtlFactor();

		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal3",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 4 );
	//////////////////////////////////////////////////////////////////////////
    //5-8
	byteIndex = pBlock->m_byteDetailTextureIndex[4];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail4",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(4,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[8] = pTexture->m_vUT;
		m_UVTransform[9] = pTexture->m_vVT;
		m_MtlFactor[4] = pTexture->GetMtlFactor();
		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal4",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 5 );
	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[5];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail5",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(5,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[10] = pTexture->m_vUT;
		m_UVTransform[11] = pTexture->m_vVT;
		m_MtlFactor[5] = pTexture->GetMtlFactor();
		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal5",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 6 );
	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[6];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail6",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(6,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[12] = pTexture->m_vUT;
		m_UVTransform[13] = pTexture->m_vVT;
		m_MtlFactor[6] = pTexture->GetMtlFactor();
		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal6",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 7 );
	//////////////////////////////////////////////////////////////////////////
	byteIndex = pBlock->m_byteDetailTextureIndex[7];
	pTexture = m_lpMtlMgr->GetDetailTexture(byteIndex);

	if(pTexture)
	{
		if(pTexture->m_lpDiffuseTexture)
			//pTerrainEffect->SetTexture("tDetail7",pTexture->m_lpDiffuseTexture->GetTexture());
			g_pd3dDevice->SetTexture(7,pTexture->m_lpDiffuseTexture->GetTexture());
		m_UVTransform[14] = pTexture->m_vUT;
		m_UVTransform[15] = pTexture->m_vVT;
		m_MtlFactor[7] = pTexture->GetMtlFactor();
		if(g_cEngineOption.bEnableGroundNormalMap && pTexture->m_lpNormalTexture)
			pTerrainEffect->SetTexture("tDetailNormal7",pTexture->m_lpNormalTexture->GetTexture());
	}
	KG_PROCESS_SUCCESS(pBlock->m_nNumDetailTexture == 8 );
	//////////////////////////////////////////////////////////////////////////
Exit1:

	pTerrainEffect->SetVectorArray("UVT",m_UVTransform,16);
	pTerrainEffect->SetVectorArray("MtlFactor",m_MtlFactor,8);
	pTerrainEffect->CommitChanges();
	return S_OK;
Exit0:

	return E_FAIL;
}

HRESULT KG3DTerrainRender::RenderWaterPrimitive()
{
	return m_RenderData.RenderWaterPrimitive(this);
}
HRESULT KG3DTerrainRenderData::RenderWaterPrimitive(KG3DTerrainRender* pRender)
{
	LPDIRECT3DINDEXBUFFER9 pIndex = NULL;
	DWORD dwFace = 0;
	DWORD dwBackAlphaTestEnable=0,dwBackAlphaBlendEnable=0;
	g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_Water);

	if(	g_bAlwayRenderWater)
	{
		g_pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE ,&dwBackAlphaTestEnable);
		g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&dwBackAlphaBlendEnable);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	}

    for (size_t i = 0; i < m_vecVertexWaterBlock.size(); ++i)
	{
		KG3DTerrainBlock* pBlock = m_vecVertexWaterBlock[i];

		if(pBlock->m_bWaterVisible || g_bAlwayRenderWater)
		{
			pRender->GetIndexBuffer(pBlock,pIndex,dwFace);
			g_pd3dDevice->SetIndices(pIndex);

			LPDIRECT3DVERTEXBUFFER9 pVB = pBlock->GetWaterVertexBuffer();
			if(pVB)
			{
				g_pd3dDevice->SetStreamSource(0,pVB, 0, sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0)); 
			}
			//////////////////////////////////////////////////////////////////////////

			g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0,dwFace);
		}
	}

	if(	g_bAlwayRenderWater)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,dwBackAlphaTestEnable);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,dwBackAlphaBlendEnable);
	}
	return S_OK;
}

HRESULT KG3DTerrainRender::RenderVertexWater(KG3DSceneSceneEditor* pEditor)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

	KG3DWaterEffect* pWaterEffect = (KG3DWaterEffect*)pEditor->GetWaterEffect();
    KG_PROCESS_ERROR(pEditor);
    KG_PROCESS_ERROR(NULL != pWaterEffect);

    hRetCode = pWaterEffect->BeginEffect(pEditor, pEditor->m_bUnderWater);
    KG_COM_PROCESS_ERROR(hRetCode);

    m_RenderData.RenderWaterPrimitive(this);

    hRetCode = pWaterEffect->EndEffect();
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0 :
    return hResult;
}


HRESULT KG3DTerrainRender::RenderWater(KG3DSceneSceneEditor* pEditor,BOOL bUseFFTW)
{
	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,0xF);

	if(bUseFFTW)
	{
		if(!m_lpOceanSurfaceRender)
			CreateOceanRender();

		//m_lpOceanSurfaceRender->updateHeightMap();

		//m_lpOceanSurfaceRender->updateGradientMap();

		m_lpOceanSurfaceRender->renderShaded(this);


		m_lpOceanSurfaceRender->displayBuffer(Buffer_GradientMap);
	}
	else
	{
        RenderVertexWater(pEditor);
	}

	m_RenderData.RenderWaterEdge(this);
	return S_OK;
}

HRESULT KG3DTerrainRender::RenderTerrainBlockDepthOnly(BOOL bTerrain,BOOL bGrass,BOOL bStone,BOOL bFrutex,BOOL bShader)
{
	return m_RenderData.RenderTerrainBlockDepthOnly(bTerrain,bGrass,bStone,bFrutex,bShader,this);
}

HRESULT KG3DTerrainRenderData::RenderTerrainBlockDepthOnly(BOOL bTerrain,BOOL bGrass,BOOL bStone,BOOL bFrutex,BOOL bShader,KG3DTerrainRender* pRender)
{
	DWORD dwNowTime = timeGetTime();
	g_pd3dDevice->SetTexture(0,NULL);
	GraphicsStruct::RenderState cull(D3DRS_CULLMODE ,D3DCULL_CCW);
	D3DXMATRIX matView,matProj,m;
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	
	LPD3DXEFFECT pTerrainEffect = pRender->m_TerrainShader.GetEffect();
	KG_PROCESS_ERROR(NULL != pTerrainEffect);

	if(bTerrain)
	{
		HRESULT hr = E_FAIL;
		m = matView * matProj;

		hr = pTerrainEffect->SetTechnique("tecZ");
		KGLOG_COM_CHECK_ERROR(hr);

        hr = pTerrainEffect->SetMatrix("matWVP",&m);
		hr = pTerrainEffect->SetMatrix("matWV",&matView);

		UINT uPass;

		hr = pTerrainEffect->Begin(&uPass,0);
		hr = pTerrainEffect->BeginPass(0);

		LPDIRECT3DINDEXBUFFER9 pIndex = NULL;
		DWORD dwFace = 0;
		
		g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_VertexOnly);
		for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
		{
			KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];
			
			pRender->GetIndexBuffer(pBlock, pIndex, dwFace);
			g_pd3dDevice->SetIndices(pIndex);

			LPDIRECT3DVERTEXBUFFER9 pVB = pBlock->GetGroundVertexBuffer();
			
			if(pVB)
			{
				g_pd3dDevice->SetStreamSource(0,pVB, 0, sizeof(VFormat::_Faces_Normal_Texture0)); 

                g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0,dwFace);
			}
		}
		hr = pTerrainEffect->EndPass();
		pTerrainEffect->End();

		RenderBlockEdge(pRender);

	}

	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);


	if(g_cEngineOption.bRenderGrass && g_cEngineManager.GetRenderOption( RENDER_GRASS ) && pRender->m_lpGrassRender)
	{
		pRender->m_lpGrassRender->RenderGrassDepthOnly(bShader,bGrass,bStone,bFrutex);
	}
Exit0:

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 30)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DTerrainRender::RenderTerrainBlockDepthOnly.",dwCost);
	}

	return S_OK;
}

HRESULT KG3DTerrainRender::CreateOceanRender()
{
	// Ocean object
	m_lpOceanSurfaceRender = new OceanSurface();
	OceanParameter ocean_param;

	//ocean_param.HeightMapSize		= 256;
	//ocean_param.MeshXN				= 128;
	//ocean_param.MeshYN				= 128;
	//ocean_param.TileX				= 8;
	//ocean_param.TileY				= 8;
	//ocean_param.UnitLength			= 1500.0f;
	//ocean_param.TimeScale			= 1.0f;
	//ocean_param.WaveAmplitude		= 1.0f;
	//ocean_param.WindDir				= D3DXVECTOR2(0.6f, 0.8f);
	//ocean_param.WindVelocity		= 700.0f;
	//ocean_param.WindDependency		= 0.1f;
	//ocean_param.ChoppyScale			= 1.0f;
	//ocean_param.ShallowColor		= D3DXVECTOR4(0.6f, 0.7f, 0.9f, 0);
	//ocean_param.DeepColor			= D3DXVECTOR4(0, 0.1f, 0.2f, 0);
	//ocean_param.ColorBlendFactor	= 0.3f;
	//ocean_param.ReflectScale		= 1.0f;
	//ocean_param.ReflFresnel			= 5.0f;
	//ocean_param.ReflFresnel			= 0.3f;
	//ocean_param.ReflDistort			= 1.5f;
	//ocean_param.RefrDistort			= 1.5f;

	ocean_param.HeightMapSize		= 512;
	ocean_param.MeshXN				= 128;
	ocean_param.MeshYN				= 128;
	ocean_param.TileX				= 4;
	ocean_param.TileY				= 4;
	ocean_param.UnitLength			= 10000.0f;
	ocean_param.TimeScale			= 1;
	ocean_param.WaveAmplitude		= 0.035f;
	ocean_param.WindDir				= D3DXVECTOR2(0.6f, 0.8f);
	ocean_param.WindVelocity		= 1000.0f;
	ocean_param.WindDependency		= 0.1f;
	ocean_param.ChoppyScale			= 0.3f;
	ocean_param.ShallowColor		= D3DXVECTOR4(0.6f, 0.7f, 0.9f, 0);
	ocean_param.DeepColor			= D3DXVECTOR4(0, 0.1f, 0.2f, 0);
	ocean_param.ColorBlendFactor	= 0.3f;
	ocean_param.ReflectScale		= 1.0f;
	ocean_param.ReflFresnel			= 5.0f;
	ocean_param.ReflFresnel			= 0.3f;
	ocean_param.ReflDistort			= 1.5f;
	ocean_param.RefrDistort			= 1.5f;

	m_lpOceanSurfaceRender->init(ocean_param);
	//m_lpOceanSurfaceRender->updateHeightMap();
	return S_OK;
}

HRESULT KG3DTerrainRenderData::RenderRoad(KG3DTerrainRender* pRender)
{
	HRESULT hr = E_FAIL;
	if(m_mapRoad.size()<=0)
		return S_OK;

	LPD3DXEFFECT pRoadEffect = pRender->m_RoadShader.GetEffect();
	
	if (!pRoadEffect)
		return hr;

	GraphicsStruct::RenderState Light(D3DRS_LIGHTING,TRUE);
	GraphicsStruct::RenderState cull(D3DRS_CULLMODE ,D3DCULL_CW);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
	GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	GraphicsStruct::RenderStateAlpha Alpha(0x80,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	UINT pass;

    hr = pRoadEffect->SetTechnique("tec0");
    KGLOG_COM_CHECK_ERROR(hr);

	hr = pRoadEffect->Begin(&pass,0);
	hr = pRoadEffect->BeginPass(0);
	map<int,KG3DTerrainRoad*>::iterator i = m_mapRoad.begin();
	map<int,KG3DTerrainRoad*>::iterator iend = m_mapRoad.end();
	if (i != iend)
	{
		g_ProjTextureRender.SetEffectParam(pRoadEffect);
	}	
	while (i!=iend)
	{
		KG3DTerrainRoad* pRoad = i->second;
		KG3DTexture* pTexture = pRoad->GetTexture();
		if(pTexture)
			hr = pRoadEffect->SetTexture("tex1",pTexture->GetTexture());
		else
			hr = pRoadEffect->SetTexture("tex1",NULL);
		g_SetPointLighting(pRoadEffect,pRoad->m_lpPointLightInfo,MAX_POINTLIGHT,false);
		pRoadEffect->CommitChanges();
		
		pRoad->Render();			
		++i;
	}
	hr = pRoadEffect->EndPass();	
	hr = pRoadEffect->End();
	if((!g_bClient) )
	{
		i = m_mapRoad.begin();
		while (i!=iend)
		{
			KG3DTerrainRoad* pRoad = i->second;
			pRoad->RenderLines(); 
			++i;
		}
	}
	return S_OK;
}
HRESULT KG3DTerrainRenderData::RenderRiver(KG3DSceneSceneEditor* pEditor)
{
	if(m_setRiver.size()<=0)
		return S_OK;

	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,0xF);
	KG3DWaterEffect* pWaterEffect = (KG3DWaterEffect*)pEditor->GetWaterEffect();
	KG_PROCESS_ERROR(NULL != pWaterEffect);	
	KG_PROCESS_ERROR(pEditor);
	{
		
		hRetCode = pWaterEffect->BeginEffectForRiver(pEditor, pEditor->m_bUnderWater);
		KG_COM_PROCESS_ERROR(hRetCode);

		set<KG3DTerrainRiver*>::iterator i = m_setRiver.begin();
		set<KG3DTerrainRiver*>::iterator iend = m_setRiver.end();

		while (i!=iend)
		{
			KG3DTerrainRiver* pRiver = *i;
			pRiver->Render();			
			++i;
		}
		hRetCode = pWaterEffect->EndEffect();
		KG_COM_PROCESS_ERROR(hRetCode);
		if((!g_bClient) )
		{
			i = m_setRiver.begin();
			while (i!=iend)
			{
				KG3DTerrainRiver* pRiver = *i;
				pRiver->RenderLines(); 
				++i;
			}
		}
	}
	
	hResult = S_OK;
Exit0:
	return hResult;

}

HRESULT KG3DTerrainRender::RenderRiver(KG3DSceneSceneEditor* pEditor)
{
	m_RenderData.RenderRiver(pEditor);
    return S_OK;
}

//void KG3DTerrainRender::CopyRenderTerrainBlock(KG3DTerrainRender* other)
//{
//    for (set<KG3DTerrainBlock*>::iterator i = other->m_setTerrainBlock.begin();
//         i != other->m_setTerrainBlock.end(); ++i)
//	{
//		KG3DTerrainBlock* pBlock = *i;
//		if (pBlock)
//		{
//            if (m_setTerrainBlockCache.find(pBlock) == m_setTerrainBlockCache.end())
//            {
//			    pBlock->AddRef();
//                m_setTerrainBlockCache.insert(pBlock);
//            }
//            m_setTerrainBlock.insert(pBlock);
//		}   
//	}
//}
HRESULT KG3DTerrainCullData::CheckUnVisible()
{
	const DWORD UNVISIBLE_COUNT = 100;

	set<KG3DTerrainBlock*>::iterator i = m_setTerrainBlockCache.begin();
	set<KG3DTerrainBlock*>::iterator iend = m_setTerrainBlockCache.end();
	while (i!=iend)
	{
		KG3DTerrainBlock* pBlock = *i;
		if(g_dwRenderCount - pBlock->GetLastVisibleFrameCount() > UNVISIBLE_COUNT)
		{
			m_vecTerrainBlockZoomOut.push_back(pBlock);

			i = m_setTerrainBlockCache.erase(i);
			continue;
		}
		++i;
	}

	return S_OK;
}

HRESULT KG3DTerrainRender::TryInputTerrainBlock(KG3DTerrainBlock* pBlock)
{
	return m_CullData.TryInputTerrainBlock(pBlock);
}

HRESULT KG3DTerrainCullData::TryInputTerrainBlock(KG3DTerrainBlock* pBlock)
{
	if(pBlock && pBlock->IsLoaded())
	{
		pair<set<KG3DTerrainBlock*>::iterator, bool> pr = m_setTerrainBlockCache.insert(pBlock);
		if (pr.second)
		{
			pBlock->AddRef();
		}

		pBlock->SetLastVisibleFrameCount(g_dwRenderCount);
	}

	return S_OK;
}

HRESULT KG3DTerrainRenderData::RemoveSortedTerrainBlock(KG3DTerrainBlock* pBlock,KG3DTerrainRender* pRender)
{
	KGLOG_PROCESS_ERROR(pBlock);

	if(pBlock->m_lpWaterVertex)
	{
        size_t i = 0;
        size_t uSize = m_vecVertexWaterBlock.size();
        for (NULL; i < uSize; ++i)
        {
            if (m_vecVertexWaterBlock[i] == pBlock)
            {
                break;
            }
        }
        if (i < uSize)
        {
            
            m_vecVertexWaterBlock[i] = m_vecVertexWaterBlock[uSize - 1];
            m_vecVertexWaterBlock.resize(uSize - 1);
        }
	}

	if( pBlock->m_nNumDetailTexture>0 && pBlock->m_nNumDetailTexture<=8)
	{
        TERRAIN_BLOCKS_GROUP *pBlockList = &m_aTerrainBlockGroup[pBlock->m_nNumDetailTexture - 1];
        TERRAIN_BLOCKS_GROUP::iterator it = find(pBlockList->begin(), pBlockList->end(), pBlock);
        if (it != pBlockList->end())
        {
            pBlockList->erase(it);
        }
	}
	else
	{
        size_t i = 0;
        size_t uSize = m_vecTerrainBlockUnsort.size();
        for (NULL; i < uSize; ++i)
        {
            if (m_vecTerrainBlockUnsort[i] == pBlock)
            {
                break;
            }
        }

        if (i < uSize)
        {
            m_vecTerrainBlockUnsort[i] = m_vecTerrainBlockUnsort[uSize - 1];
            m_vecTerrainBlockUnsort.resize(uSize - 1);
        }
	}

	//////////////////////////////////////////////////////////////////////////
	//if(m_lpGrassRender)
	//{
	//	if(pBlock->m_lpGrassData)
	//	{
	//		m_lpGrassRender->RemoveGrassBlock(pBlock->m_lpGrassData); //把草数据块添加到绘制列表里
	//	}
	//	if (pBlock->m_lpStoneData)
	//	{
	//		m_lpGrassRender->RemoveStoneBlock(pBlock->m_lpStoneData);//把碎石数据块添加到绘制列表里
	//	}
	//	if (pBlock->m_lpFrutexData)
	//	{
	//		m_lpGrassRender->RemoveFrutexBlock(pBlock->m_lpFrutexData);//把灌木数据块添加到绘制列表里
	//	}
	//}


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRenderData::InputTerrainBlock(KG3DTerrainBlock* pBlock)
{
	pBlock->AddRef();
	m_vecTerrainBlock.push_back(pBlock);
	return S_OK;
}

HRESULT KG3DTerrainCullData::PrepareRenderData(KG3DTerrainRenderData* pRenderData,KG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	set<KG3DTerrainBlock *>::iterator itIndex;

	pRenderData->CleanVecTerrainBlock();

	itIndex = m_setTerrainBlockCache.begin();
	for (NULL; itIndex != m_setTerrainBlockCache.end(); ++itIndex)
	{
		KG3DTerrainBlock* pBlock = *itIndex;
		ASSERT(pBlock);

		if(pBlock && pBlock->IsLoaded())
		{

			DWORD dwVisibleFrame = pBlock->GetLastVisibleFrameCount();

			if (dwVisibleFrame != g_dwRenderCount)
				continue;

			pRenderData->InputTerrainBlock(pBlock);
		}
	}

	pRenderData->m_vecTerrainBlockZoomOut = m_vecTerrainBlockZoomOut;
	m_vecTerrainBlockZoomOut.clear();

	//////////////////////////////////////////////////////////////////////////
	pRenderData->m_mapRoad = m_mapRoad;
	pRenderData->m_setRiver = m_setRiver;

	pRenderData->SortTerrainBlock();

	pRenderData->ProcessLOD(pOutDoorMgr);

	return S_OK;
}

HRESULT KG3DTerrainRenderData::PrepareGrass(KG3DTerrainRender* pRender)
{
	for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
	{
		KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];
		ASSERT(pBlock);

		if(pBlock->m_lpGrassData)
		{
			if(pBlock->m_fViewDistance < s_fViewGrassDistance)
				pRender->m_lpGrassRender->InputGrassBlock(pBlock->m_lpGrassData); //把草数据块添加到绘制列表里
		}
		if (pBlock->m_lpStoneData)
		{
			if(pBlock->m_fViewDistance < s_fViewGrassDistance)
				pRender->m_lpGrassRender->InputStoneBlock(pBlock->m_lpStoneData);//把碎石数据块添加到绘制列表里
		}
		if (pBlock->m_lpFrutexData)
		{
			if(pBlock->m_fViewDistance < s_fViewGrassDistance)
				pRender->m_lpGrassRender->InputFrutexBlock(pBlock->m_lpFrutexData);//把灌木数据块添加到绘制列表里
		}
	}

	pRender->m_lpGrassRender->m_dwLastAddRenderCount = g_dwRenderCount;

	return S_OK;
}

HRESULT KG3DTerrainRender::PrepareRenderData(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	DWORD dwNowTime = timeGetTime();

	m_CullData.CheckUnVisible();

	m_CullData.PrepareRenderData(&m_RenderData,pOutDoorMgr);

    if(m_bCheckVisibleCount)
	{
	    m_RenderData.PrepareGrass(this);
	}

    if(m_lpGrassRender)
    {
        m_lpGrassRender->FrameMove();
    }

	//////////////////////////////////////////////////////////////////////////
	m_RenderData.BeforeRenderProcess();

	//////////////////////////////////////////////////////////////////////////

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 5)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DTerrainRender::PrepareRenderData",dwCost);
	}


	return S_OK;
}


HRESULT KG3DTerrainRender::EnableRenderMask( BOOL bEnable, DWORD dwMaskTexID, const D3DXVECTOR4* pSceneRect)
{
	m_bRenderMask = bEnable;
	if (bEnable)
	{	
		KG_PROCESS_ERROR(NULL != pSceneRect);
		m_dwMaskTexID = dwMaskTexID;
		m_vSceneRect = *pSceneRect;
	}
	else
	{
		m_dwMaskTexID = 0;
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRender::GetWaterBoundingPlanes(KG3DWaterCuller* pWaterCuller)
{
	return m_RenderData.GetWaterBoundingPlanes(pWaterCuller,this);
}

HRESULT KG3DTerrainRenderData::GetWaterBoundingPlanes(KG3DWaterCuller* pWaterCuller,KG3DTerrainRender* pRender)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DTerrainBlock* pBlock = NULL;

    KGLOG_PROCESS_ERROR(pWaterCuller);

    KG_PROCESS_SUCCESS(!pRender->m_bWaterVisible);

    hrRetCode = pWaterCuller->BeginAddWaterBoundingPlanes();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
    {
        pBlock = m_vecTerrainBlock[i];

        map<int, KG3DTerrainBlock::_RECTANGLE>::iterator it = pBlock->m_mapWaterBoundingPlanes.begin();

        for (NULL; it != pBlock->m_mapWaterBoundingPlanes.end(); ++it)
        {
            pWaterCuller->AddWaterBoundingPlanes(it->second.vLeftBottomPos, it->second.vRightUpPos);
        }
    }

    hrRetCode = pWaterCuller->EndAddWaterBoundingPlanes();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DTerrainRender::CreateBlockEdgeIndex()
{
	HRESULT hr;
	WORD *pw;
	WORD baseindex;
	WORD p1index,p2index;
	const int NumOfLine = 16;
	WORD i,j;

	baseindex = 0;

	SAFE_RELEASE(m_lpEdgeIndex);
	hr = g_pd3dDevice->CreateIndexBuffer(16 * 4 * 2 * sizeof(WORD),D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,D3DPOOL_MANAGED, &m_lpEdgeIndex,NULL);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_lpEdgeIndex->Lock(0,16 * 4 * 2 * sizeof(WORD),(void **)&pw,0);
	KG_COM_PROCESS_ERROR(hr);

	// bottom
	for(j = 0; j < NumOfLine; ++j)
	{
		p1index = j;
		p2index = j + 1;

		pw[baseindex] = p1index;
		++baseindex;
		pw[baseindex] = p2index;
		++baseindex;
	}

	//top
	for(j = 0; j < NumOfLine; ++j)
	{
		p1index = 16 * 17 + j;
		p2index = p1index + 1;

		pw[baseindex] = p1index;
		++baseindex;
		pw[baseindex] = p2index;
		++baseindex;
	}

	//left
	for(i = 0; i < NumOfLine; ++i)
	{
		p1index = i * 17;
		p2index = (i + 1) * 17;

		pw[baseindex] = p1index;
		++baseindex;
		pw[baseindex] = p2index;
		++baseindex;
	}

	//right
	for(i = 0; i < NumOfLine; ++i)
	{
		p1index = i * 17 + 16;
		p2index = (i + 1) * 17 + 16;

		pw[baseindex] = p1index;
		++baseindex;
		pw[baseindex] = p2index;
		++baseindex;
	}

	m_lpEdgeIndex->Unlock();

	return S_OK;
Exit0:
	SAFE_RELEASE(m_lpEdgeIndex);
	return S_FALSE;
}

HRESULT KG3DTerrainRenderData::RenderBlockEdge(KG3DTerrainRender* pRender)
{
	if(!g_bRenderBlockEdge)
		return S_OK;
	if(!pRender->m_lpEdgeIndex)
		return E_FAIL;
	DWORD Light = 0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&Light);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	

	D3DXMATRIX matWorld,matSaveWorld;
	D3DXMatrixTranslation(&matWorld,0.0f,20.0f,0.0f);
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&matSaveWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);

	g_pd3dDevice->SetTexture(0,NULL);

	g_pd3dDevice->SetIndices(pRender->m_lpEdgeIndex);
	g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_VertexOnly);

    for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
	{
		KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];
		LPDIRECT3DVERTEXBUFFER9 pVB = pBlock->GetGroundVertexBuffer();
		if(pVB)
		{
			g_pd3dDevice->SetStreamSource(0,pVB, 0, sizeof(VFormat::_Faces_Normal_Texture0)); 
			g_pd3dDevice->DrawIndexedPrimitive(D3DPT_LINELIST,0,0,17 * 17,0,16 * 4);
		}
	}
	
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, Light );	
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&matSaveWorld);
	return S_OK;
}

HRESULT KG3DTerrainRenderData::RenderWaterEdge(KG3DTerrainRender* pRender)
{
	if(!g_bRenderWaterEdge)
		return S_OK;
	if(!pRender->m_lpEdgeIndex)
		return E_FAIL;
	DWORD Light = 0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&Light);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	

	D3DXMATRIX matWorld,matSaveWorld;
	D3DXMatrixTranslation(&matWorld,0.0f,20.0f,0.0f);
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&matSaveWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetIndices(pRender->m_lpEdgeIndex);
	g_pd3dDevice->SetVertexDeclaration(pRender->m_lpDeclaration_VertexOnly);
    for (size_t j = 0; j < m_vecVertexWaterBlock.size(); ++j)
	{
		KG3DTerrainBlock* pBlock = m_vecVertexWaterBlock[j];
		LPDIRECT3DVERTEXBUFFER9 pVB = pBlock->GetWaterVertexBuffer();
		if(pVB)
		{
			g_pd3dDevice->SetStreamSource(0,pVB, 0, sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0)); 
			g_pd3dDevice->DrawIndexedPrimitive(D3DPT_LINELIST,0,0,17 * 17,0,16 * 4);
		}
	}
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, Light );	
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&matSaveWorld);
	return S_OK;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DTerrainRender)
{
	class TestRender : public KG3DTerrainRender
	{
	public:
		using KG3DTerrainRender::m_bRenderMask;
		using KG3DTerrainRender::m_dwMaskTexID;
		using KG3DTerrainRender::m_vSceneRect;
	};
	{
		TestRender Render;
		_ASSERTE(Render.m_dwMaskTexID == 0);

		D3DXVECTOR4 vSceneRect(1,1,123,123);
		HRESULT hr = Render.EnableRenderMask(TRUE, 0x12345678, NULL);
		_ASSERTE(FAILED(hr));
		_ASSERTE(Render.m_dwMaskTexID == 0);

		Render.EnableRenderMask(TRUE, 0x12345678, &vSceneRect);
		_ASSERTE(Render.m_dwMaskTexID == 0x12345678 && Render.m_vSceneRect == vSceneRect);
		
		vSceneRect.x = 0;
		Render.EnableRenderMask(TRUE, 0x12345670, &vSceneRect);
		_ASSERTE(Render.m_dwMaskTexID == 0x12345670 && Render.m_vSceneRect == vSceneRect);
		
		Render.EnableRenderMask(FALSE, 0x12345670, &vSceneRect);
		_ASSERTE(Render.m_dwMaskTexID == 0);
		
	}
}
KG_TEST_END(KG3DTerrainRender)
#endif
DWORD KG3DTerrainRender::GetVisibleGrassBlockCount()
{
	if(m_lpGrassRender)
	{
		return m_lpGrassRender->GetVisibleGrassBlockCount();
	}
	return  0;
}

bool SortComp(KG3DTerrainBlock *pBlockA, KG3DTerrainBlock *pBlockB)
{
    return (pBlockA->m_ullMaterialKey < pBlockB->m_ullMaterialKey);
}

HRESULT KG3DTerrainRenderData::SortTerrainBlock()
{
	CleanTerrainBlockGroup();

	m_vecVertexWaterBlock.clear();
	m_vecTerrainBlockUnsort.clear();

    for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
	{
		KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];
		GroupTerrainBlock(pBlock);
	}
	
	for (int i = 0; i < BLOCK_GROUP_COUNT; ++i)
    {
        sort(m_aTerrainBlockGroup[i].begin(), m_aTerrainBlockGroup[i].end(), SortComp);
    }
	
	return S_OK;
}

HRESULT KG3DTerrainRender::ProcessPointLighting(KG3DScenePointLightRender* pLights)
{
	return m_RenderData.ProcessPointLighting(pLights);
}

HRESULT KG3DTerrainRenderData::ProcessPointLighting(KG3DScenePointLightRender* pLights)
{
    for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
	{
		KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];

		D3DXVECTOR2 A,C;

		GetSceneBlockRect(pBlock->m_nLevel,pBlock->m_nIndex_X,pBlock->m_nIndex_Z,A,C);

		pLights->FindRefrenceLightForTerrainBlock(pBlock,A,C);
	}
	
	if (m_mapRoad.size() > 0)
	{
		map<int,KG3DTerrainRoad*>::iterator it = m_mapRoad.begin();
		while(it != m_mapRoad.end())
		{
			KG3DTerrainRoad* pRoad = it->second;
			pLights->FindRefrenceLightForRoad(pRoad);
			++it;
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainRender::GetIndexBuffer(KG3DTerrainBlock* pBlock,LPDIRECT3DINDEXBUFFER9& pIndex,DWORD& dwFaceCount)
{
	KG_PROCESS_ERROR(pBlock);
	if((g_cEngineOption.bUseTerrainLOD && pBlock->m_nLevel != BLOCK_TYPE_ZONE))
	{
		switch(pBlock->m_nLodLevel)
		{
		case 0:
			pIndex = m_lpLevel0Index[pBlock->m_nIndexIB];
			dwFaceCount = m_nLevel0FaceCount[pBlock->m_nIndexIB];
			break;
		case 1:
			pIndex = m_lpLevel1Index[pBlock->m_nIndexIB];
			dwFaceCount = m_nLevel1FaceCount[pBlock->m_nIndexIB];
			break;
		case 2:
			pIndex = m_lpLevel2Index[pBlock->m_nIndexIB];
			dwFaceCount = m_nLevel2FaceCount[pBlock->m_nIndexIB];
			break;
		case 3:
			pIndex = m_lpLevel3Index;
			dwFaceCount = m_nLevel3FaceCount;
			break;
		}
	}
	else
	{
		pIndex = m_lpLevel0Index[0];
		dwFaceCount = m_nLevel0FaceCount[0];
	}
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DTerrainRender::LoadIndexBufferFromFile()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	int nRetCode = false;
	vector<WORD> iindex0,iindex1,iindex2,iindex3;
	char name[MAX_PATH];

	for (int i = 0; i < 16; ++i)
	{
		//Load the region index of level 0; The data file name is Level1Index%i.PI,
		//although the name is Level1..,the real level of region is 0. 
		nRetCode = sprintf(name,"Data\\public\\IndexData\\Level1Index%i.PI", i);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		hrRetCode = LoadPatchIndex(name,iindex0);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

		//m_nLevel0NumOfIndex[i] = (int)iindex0.size();
		m_nLevel0FaceCount[i] = (int)iindex0.size() / 3;
		hrRetCode = CreateIndexBuffer(&m_lpLevel0Index[i],iindex0);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

		//Load the region index of level 1
		nRetCode = sprintf(name,"Data\\public\\IndexData\\Level2Index%i.PI",i);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		hrRetCode = LoadPatchIndex(name,iindex1);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

		m_nLevel1FaceCount[i] = (int)iindex1.size() / 3;
		hrRetCode = CreateIndexBuffer(&m_lpLevel1Index[i],iindex1);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

		nRetCode = sprintf(name,"Data\\public\\IndexData\\Level3Index%i.PI",i);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		hrRetCode = LoadPatchIndex(name,iindex2);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

		m_nLevel2FaceCount[i] = (int)iindex2.size() / 3;
		hrRetCode = CreateIndexBuffer(&m_lpLevel2Index[i],iindex2);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);
	}

	nRetCode = sprintf(name,"Data\\public\\IndexData\\Level4Index0.PI");
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	hrRetCode = LoadPatchIndex(name,iindex3);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

	m_nLevel3FaceCount = (int)iindex3.size() / 3;
	hrRetCode = CreateIndexBuffer(&m_lpLevel3Index, iindex3);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

	hrResult =  S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DTerrainRender::LoadPatchIndex(const char *FileName,vector<WORD> &IndexList)
{
	int nResult = false;
	unsigned long uSize = 0;;
	PatchIndexHeader pih;
	int i = 0;
	IFile *fp = NULL;
	WORD *pi = NULL;

	KGLOG_PROCESS_ERROR(FileName);

	memset((void *)&pih, 0, sizeof(PatchIndexHeader));
	fp = g_OpenFile(FileName);
	
	KGLOG_PROCESS_ERROR(fp);

	uSize = fp->Read(&pih,sizeof(PatchIndexHeader));
	KG_PROCESS_ERROR(uSize == sizeof(PatchIndexHeader));

	pi = new WORD[pih.NumOfIndex];
	KGLOG_PROCESS_ERROR(pi);

	uSize = fp->Read(pi,pih.SzieOfPerIndex * pih.NumOfIndex);
	KG_PROCESS_ERROR((int)uSize == pih.SzieOfPerIndex * pih.NumOfIndex);

	IndexList.clear();
	//reserve enough memory
	IndexList.reserve(pih.NumOfIndex);

	for (i = 0;i < pih.NumOfIndex; ++i)
		IndexList.push_back(pi[i]);

	nResult = true;
Exit0:
	if (pi)
	{
		delete []pi;
		pi = NULL;   
	}
	KG_COM_RELEASE(fp);
	return nResult ? S_OK : E_FAIL;
}

HRESULT KG3DTerrainRender::CreateIndexBuffer(LPDIRECT3DINDEXBUFFER9 *ppindex,vector<WORD> &data)
{
	WORD *wib,t;
	int NumOfIndex = (int)data.size();

	if (NumOfIndex == 0)
		return S_FALSE;

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(sizeof(WORD) * NumOfIndex,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, ppindex,NULL )))
		return S_FALSE;

	if (FAILED((*ppindex)->Lock(0,sizeof(WORD) * NumOfIndex,(void **)&wib,0)))
		return S_FALSE;

	for (int i = 0;i < NumOfIndex;++i)
	{
		t = data[i];
		wib[i] = data[i];
	}

	(*ppindex)->Unlock();

	return S_OK;
}


HRESULT KG3DTerrainRenderData::ProcessLOD(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	if(!g_cEngineOption.bUseTerrainLOD)
		return S_OK;
	if(!pOutDoorMgr)
		return E_FAIL;
	KG3DTerrainBlock* pNeighborBlock[4];
	int nNeighborLevel[4];
    for (size_t i = 0; i < m_vecTerrainBlock.size(); ++i)
	{
		KG3DTerrainBlock* pTerrainBlock = m_vecTerrainBlock[i];
	
		if (pTerrainBlock->m_nLevel != BLOCK_TYPE_ZONE)
		{
			/*pBlock_Up*/   
			pNeighborBlock[0] = pOutDoorMgr->GetTerrainBlock(pTerrainBlock->m_nLevel,pTerrainBlock->m_nIndex_X,pTerrainBlock->m_nIndex_Z+1);
			if(pNeighborBlock[0])
				pNeighborBlock[0]->AddRef();
			/*pBlock_Right*/
			pNeighborBlock[1] = pOutDoorMgr->GetTerrainBlock(pTerrainBlock->m_nLevel,pTerrainBlock->m_nIndex_X+1,pTerrainBlock->m_nIndex_Z);
			if(pNeighborBlock[1])
				pNeighborBlock[1]->AddRef();
			/*pBlock_Down */
			pNeighborBlock[2] = pOutDoorMgr->GetTerrainBlock(pTerrainBlock->m_nLevel,pTerrainBlock->m_nIndex_X,pTerrainBlock->m_nIndex_Z-1);
			if(pNeighborBlock[2])
				pNeighborBlock[2]->AddRef();
			/*pBlock_Left */
			pNeighborBlock[3] = pOutDoorMgr->GetTerrainBlock(pTerrainBlock->m_nLevel,pTerrainBlock->m_nIndex_X-1,pTerrainBlock->m_nIndex_Z);
			if(pNeighborBlock[3])
				pNeighborBlock[3]->AddRef();
			for (int j=0; j<4;j++)
			{
				if(pNeighborBlock[j] == NULL)
					nNeighborLevel[j] = 0;
				else
				{
					if(pNeighborBlock[j]->m_nLodLevel > pTerrainBlock->m_nLodLevel)
						nNeighborLevel[j] = 1;
					else
						nNeighborLevel[j] = 0;
				}
				SAFE_RELEASE(pNeighborBlock[j]);
			}
			pTerrainBlock->m_nIndexIB = nNeighborLevel[0] + (nNeighborLevel[1] << 1) + (nNeighborLevel[2] << 2) + (nNeighborLevel[3] << 3);
		}
		
		
	}
	return S_OK;
}


DWORD KG3DTerrainRender::GetVisibleTerrainBlockCount()
{
	return m_RenderData.GetVisibleTerrainBlockCount();
}

	
HRESULT KG3DTerrainCullData::UnInit()
{
	//////////////////////////////////////////////////////////////////////////
	{
		set<KG3DTerrainBlock*>::iterator i = m_setTerrainBlockCache.begin();
		set<KG3DTerrainBlock*>::iterator iend = m_setTerrainBlockCache.end();
		while(i!=iend)
		{
			KG3DTerrainBlock* pBlock = *i;
			pBlock->UnPrepareRender();
			SAFE_RELEASE(pBlock);
			++i;
		}
		m_setTerrainBlockCache.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		for (size_t i=0;i<m_vecTerrainBlockZoomOut.size();i++)
		{
			KG3DTerrainBlock* pBlock = m_vecTerrainBlockZoomOut[i];
			pBlock->UnPrepareRender();
			SAFE_RELEASE(pBlock);
		}
		m_vecTerrainBlockZoomOut.clear();
	}
	
	////////////////////////////////////////////////////////
	{
		set<KG3DTerrainRiver*>::iterator it = m_setRiver.begin();
		while(it != m_setRiver.end())
		{
			KG3DTerrainRiver* pRiver = *it;
			SAFE_RELEASE(pRiver);
			++it;
		}
		m_setRiver.clear();
	}
	/////////////////////////////////////////////////////////
	{
		map<int,KG3DTerrainRoad*>::iterator it = m_mapRoad.begin();
		while(it != m_mapRoad.end())
		{
			KG3DTerrainRoad* pRoad = it->second;
			SAFE_RELEASE(pRoad);
			++it;
		}
		m_mapRoad.clear();
	}
	return S_OK;
}

HRESULT KG3DTerrainRenderData::CleanVecTerrainBlock()
{
	for (size_t i=0;i<m_vecTerrainBlock.size();i++)
	{
		SAFE_RELEASE(m_vecTerrainBlock[i]);
	}
	m_vecTerrainBlock.clear();
	return S_OK;
}

HRESULT KG3DTerrainRenderData::UnInit()
{
	for (size_t i=0;i<m_vecTerrainBlockZoomOut.size();i++)
	{
		SAFE_RELEASE(m_vecTerrainBlockZoomOut[i]);
	}
	m_vecTerrainBlockZoomOut.clear();

	CleanVecTerrainBlock();
	CleanTerrainBlockGroup();
	return S_OK;
}

HRESULT KG3DTerrainRender::CleanRoad()
{
	map<int,KG3DTerrainRoad*>::iterator it = m_CullData.m_mapRoad.begin();
	while(it != m_CullData.m_mapRoad.end())
	{
		KG3DTerrainRoad* pRoad = it->second;
		SAFE_RELEASE(pRoad);
		++it;
	}
	m_CullData.m_mapRoad.clear();
	return S_OK;
}

HRESULT KG3DTerrainRender::CleanRiver()
{
	set<KG3DTerrainRiver*>::iterator it = m_CullData.m_setRiver.begin();
	while(it != m_CullData.m_setRiver.end())
	{
		KG3DTerrainRiver* pRiver = *it;
		SAFE_RELEASE(pRiver);
		++it;
	}
	m_CullData.m_setRiver.clear();
	return S_OK;
}

map<int,KG3DTerrainRoad*>* KG3DTerrainRender::GetRoadMap()
{
	return &m_CullData.m_mapRoad;
}

set<KG3DTerrainRiver*>* KG3DTerrainRender::GetRiverSet()
{
	return &m_CullData.m_setRiver;
}

KG3DTerrainRenderData::KG3DTerrainRenderData()
{
	;
}

KG3DTerrainRenderData::~KG3DTerrainRenderData()
{
	UnInit();
}

KG3DTerrainCullData::KG3DTerrainCullData()
{
	;
}

KG3DTerrainCullData::~KG3DTerrainCullData()
{
	UnInit();
}

HRESULT KG3DTerrainRenderData::BeforeRenderProcess()
{
	{
		for (size_t i=0;i<m_vecTerrainBlockZoomOut.size();i++)
		{
			KG3DTerrainBlock* pBlock = m_vecTerrainBlockZoomOut[i];
			if(pBlock)
			{
				pBlock->UnPrepareRender();
				pBlock->Release();
			}
		}
		m_vecTerrainBlockZoomOut.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	{
		for (size_t i=0;i<m_vecTerrainBlock.size();i++)
		{
			KG3DTerrainBlock* pBlock = m_vecTerrainBlock[i];
			if(pBlock)
				pBlock->PrepareRender();
		}
	}

	return S_OK;
}


DWORD KG3DTerrainRender::GetDetailTextureInfo(DWORD dwDetailIndex)
{
	DWORD dwReturn = INVALID_GROUNDTYPE;
	KG_PROCESS_ERROR(m_lpMtlMgr);

	KG3DTerrainDetailTexture *pTexture = m_lpMtlMgr->GetDetailTexture((BYTE)dwDetailIndex);
	KG_PROCESS_ERROR(pTexture);

	dwReturn = pTexture->m_dwMaterialType;
Exit0:
	return dwReturn;
}