#include "StdAfx.h"
#include "KG3DSceneShadowMap.h"
#include "KG3DSceneEntityMgr.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DTerrainRender.h"
#include "KG3DModelPointLight.h"
#include "KG3DScenePvsEditor.h"
#include "KG3DRenderTargetTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL g_bForceDisableCull = FALSE;

KG3DSceneShadowMap::KG3DSceneShadowMap(void)
{
    m_bInit = FALSE;
}

KG3DSceneShadowMap::~KG3DSceneShadowMap(void)
{
}

KG3DShadowMapLevel::KG3DShadowMapLevel()
{
	m_lpSMShadowMap = NULL;
	m_lpSMShadowMapSurface = NULL;

	m_lpShadowMapEntityMgr = NULL;

	m_lpSMColorMap = NULL;
	m_lpSMColorSurface = NULL;
	m_bUseINTZ = FALSE;

}

KG3DShadowMapLevel::~KG3DShadowMapLevel()
{
	UnInit();
}

HRESULT KG3DShadowMapLevel::Init(DWORD dwSize)
{
	HRESULT hr = S_OK;

	m_dwShadowmapSize = dwSize;
	nIndex = 0;
	SAFE_RELEASE(m_lpSMColorSurface);
	SAFE_RELEASE(m_lpSMColorMap);
	SAFE_RELEASE(m_lpSMShadowMapSurface);
	SAFE_RELEASE(m_lpSMShadowMap);
	hr = OnResetDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(m_lpSMShadowMapSurface);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DShadowMapLevel::Init(KG3DSceneShadowMap* pShadow,int nLevel,KG3DSceneEntityMgr* pSrcMgr)
{
	HRESULT hr = S_OK;

	nIndex = nLevel;
	SAFE_RELEASE(m_lpSMShadowMapSurface);
	SAFE_RELEASE(m_lpSMShadowMap);
    hr = OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(m_lpSMShadowMapSurface);

	if(!m_lpShadowMapEntityMgr)
	{
		m_lpShadowMapEntityMgr = new KG3DSceneEntityMgrShadow;
		m_lpShadowMapEntityMgr->Init( pSrcMgr );

		m_lpShadowMapEntityMgr->m_lpTerrainRender->EnableCheckVisibleCount(FALSE);
		m_lpShadowMapEntityMgr->m_bGetEntityFromContainerDirect = TRUE;

		if(nLevel == 0)
			m_lpShadowMapEntityMgr->SetVisibleGate(D3DX_PI*0.000F ,D3DX_PI*0.01F);
		else if(nLevel == 1)
			m_lpShadowMapEntityMgr->SetVisibleGate(D3DX_PI*0.08F ,D3DX_PI*0.01F);
		else
			m_lpShadowMapEntityMgr->SetVisibleGate(D3DX_PI*0.16F ,D3DX_PI*0.05F);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DShadowMapLevel::UnInit()
{
	SAFE_RELEASE(m_lpSMShadowMapSurface);
	SAFE_RELEASE(m_lpSMShadowMap);
	
	SAFE_DELETE(m_lpShadowMapEntityMgr);

	return S_OK;
}

HRESULT KG3DShadowMapLevel::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_lpShadowMapEntityMgr)
    {
        hr = m_lpShadowMapEntityMgr->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	SAFE_RELEASE(m_lpSMShadowMapSurface);
	SAFE_RELEASE(m_lpSMShadowMap);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DShadowMapLevel::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DFORMAT format = D3DFMT_UNKNOWN;
	
    ASSERT(m_dwShadowmapSize > 0);
    ASSERT(m_lpSMShadowMap == NULL);
    ASSERT(m_lpSMShadowMapSurface == NULL);

    if (m_lpShadowMapEntityMgr)
    {
        hr = m_lpShadowMapEntityMgr->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    
	format = D3DFMT_D24S8;

	if(m_bUseINTZ)
		format = (D3DFORMAT)MAKEFOURCC('I','N','T','Z');

    hr = g_pd3dDevice->CreateTexture(m_dwShadowmapSize, m_dwShadowmapSize, 1, D3DUSAGE_DEPTHSTENCIL,format, 
        D3DPOOL_DEFAULT, &m_lpSMShadowMap, NULL);
	if (FAILED(hr))
	{
		format = D3DFMT_D16;
		hr = g_pd3dDevice->CreateTexture(m_dwShadowmapSize, m_dwShadowmapSize, 1, D3DUSAGE_DEPTHSTENCIL,format, 
			D3DPOOL_DEFAULT, &m_lpSMShadowMap, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = m_lpSMShadowMap->GetSurfaceLevel(0, &m_lpSMShadowMapSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(m_bNeedColorSurface)
	{
		hr = g_pd3dDevice->CreateTexture(m_dwShadowmapSize,m_dwShadowmapSize,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,&m_lpSMColorMap,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpSMColorMap->GetSurfaceLevel(0, &m_lpSMColorSurface);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	ASSERT(m_lpSMShadowMapSurface);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DShadowMapLevel::ProcessShadowMap(KG3DCamera* pCameraLight,D3DXVECTOR3& vFocusPos,KG3DSceneSceneEditor* pEditor,KG3DSceneShadowMap* pShadow,LPDIRECT3DSURFACE9 pColorSur)
{
    HRESULT hResult = E_FAIL;
	HRESULT hr = S_OK;
	LPDIRECT3DSURFACE9 pDepthSave = NULL;
	LPDIRECT3DSURFACE9 pTargetSave = NULL;
	D3DVIEWPORT9 ViewportSave;
	D3DVIEWPORT9 Viewport;
    BOOL bSetRenderTarget = FALSE;
    BOOL bSetDepthStencilSurface = FALSE;
    BOOL bSetViewport = FALSE;
	//float fDepthBias = 0.0002f;
	//float fBiasSlope = 0.5f;
	//g_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS,*(DWORD*)&fDepthBias);
	//g_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,*(DWORD*)&fBiasSlope);

	pCameraLight->SetCamera();

	m_lpShadowMapEntityMgr->FrameMove(pEditor,FALSE);
	if(g_dwRenderCount % 2 < 1)
		m_lpShadowMapEntityMgr->GetVisibleEntityEx(pCameraLight,pEditor,vFocusPos);
	//////////////////////////////////////////////////////////////////////////

	Viewport.X = 0;
    Viewport.Y = 0;
	Viewport.Height = m_dwShadowmapSize;
    Viewport.Width = m_dwShadowmapSize;
	Viewport.MaxZ = 1;
    Viewport.MinZ = 0;

	hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetViewport(&ViewportSave);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetRenderTarget(0, pColorSur);
	KGLOG_COM_PROCESS_ERROR(hr);

    bSetRenderTarget = TRUE;

	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpSMShadowMapSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

    bSetDepthStencilSurface = TRUE;

	hr = g_pd3dDevice->SetViewport(&Viewport);
    KGLOG_COM_PROCESS_ERROR(hr);

    bSetViewport = TRUE;

	//////////////////////////////////////////////////////////////////////////
	//hr = g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	//KGLOG_COM_PROCESS_ERROR(hr);

	//hr = g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xFF000000, 1, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

	{
		GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);

		{
			GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA);
			GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
			GraphicsStruct::RenderState ZEnable(D3DRS_ZENABLE,TRUE);
			GraphicsStruct::RenderState lightEnable(D3DRS_LIGHTING,FALSE);
			GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
			//GraphicsStruct::RenderState cullState(D3DRS_CULLMODE,D3DCULL_CW);
			if (g_cEngineOption.ModelShadowType >= EM_MODEL_SHADOW_TYPE_HIGH)
			{
				if(nIndex ==0)
				{
					m_lpShadowMapEntityMgr->RenderForShadowMap(
						pEditor,
						TRUE,
						g_cEngineManager.GetRenderOption(RENDER_TREE),
						g_cEngineManager.GetRenderOption(RENDER_ENTITY) && g_cEngineManager.GetRenderOption(RENDER_NORL_MESH),
						g_cEngineManager.GetRenderOption(RENDER_ENTITY) && g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH)
						);

					if(pEditor->m_pRepresentNPC)
					{
						pEditor->m_pRepresentNPC->Render(0);
					}
				}
				else
				{
					m_lpShadowMapEntityMgr->RenderForShadowMap(
						pEditor,
						TRUE,
						g_cEngineManager.GetRenderOption(RENDER_TREE),
						g_cEngineManager.GetRenderOption(RENDER_ENTITY) && g_cEngineManager.GetRenderOption(RENDER_NORL_MESH),
						FALSE
						);
				}
			}
			else if (g_cEngineOption.ModelShadowType == EM_MODEL_SHADOW_TYPE_MIDDLE)
			{
				if(nIndex ==0)
				{
					m_lpShadowMapEntityMgr->RenderForShadowMap(
						pEditor,
						FALSE,
						FALSE,
						FALSE,
						g_cEngineManager.GetRenderOption(RENDER_ENTITY) && g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH)
						);

					if(pEditor->m_pRepresentNPC)
					{
						pEditor->m_pRepresentNPC->Render(0);
					}
				}

			}

		}
	}

	g_RenderRenderTwiceInfo();
    m_lpShadowMapEntityMgr->RenderSpecialAlphaModel();

	hResult = S_OK;
Exit0:
    
    if (bSetRenderTarget)
    {
        hr = g_pd3dDevice->SetRenderTarget(0,pTargetSave);
        KGLOG_COM_CHECK_ERROR(hr);
    }
    KG_COM_RELEASE(pTargetSave);

    if (bSetDepthStencilSurface)
    {
    	hr = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
        KGLOG_COM_CHECK_ERROR(hr);
    }
    KG_COM_RELEASE(pDepthSave);

    if (bSetViewport)
    {
        hr = g_pd3dDevice->SetViewport(&ViewportSave);
        KGLOG_COM_CHECK_ERROR(hr);
    }
	//g_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS,0);
	//g_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
	return hResult;
}


HRESULT KG3DShadowMapLevel::ProcessShadowMapWithPVS(KG3DRepresentObjectPVS* pPVS)
{
	HRESULT hResult = E_FAIL;
	HRESULT hr = S_OK;
	LPDIRECT3DSURFACE9 pDepthSave = NULL;
	LPDIRECT3DSURFACE9 pTargetSave = NULL;
	D3DVIEWPORT9 ViewportSave;
	D3DVIEWPORT9 Viewport;
	BOOL bSetRenderTarget = FALSE;
	BOOL bSetDepthStencilSurface = FALSE;
	BOOL bSetViewport = FALSE;

	m_vCameraLight.SetCamera();

	//////////////////////////////////////////////////////////////////////////

	Viewport.X = 0;
	Viewport.Y = 0;
	Viewport.Height = m_dwShadowmapSize;
	Viewport.Width = m_dwShadowmapSize;
	Viewport.MaxZ = 1;
	Viewport.MinZ = 0;

	hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetViewport(&ViewportSave);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetRenderTarget(0, m_lpSMColorSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

	bSetRenderTarget = TRUE;

	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpSMShadowMapSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

	bSetDepthStencilSurface = TRUE;

	hr = g_pd3dDevice->SetViewport(&Viewport);
	KGLOG_COM_PROCESS_ERROR(hr);

	bSetViewport = TRUE;

	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->BeginScene();

	hr = g_pd3dDevice->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xFF000000, 1, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

	{
		GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
		g_bForceDisableCull = TRUE;

		pPVS->RenderHierarchy(FALSE,NULL);

		g_bForceDisableCull = FALSE;
	}

	g_pd3dDevice->EndScene();

	hResult = S_OK;
Exit0:

	if (bSetRenderTarget)
	{
		hr = g_pd3dDevice->SetRenderTarget(0,pTargetSave);
		KGLOG_COM_CHECK_ERROR(hr);
	}
	KG_COM_RELEASE(pTargetSave);

	if (bSetDepthStencilSurface)
	{
		hr = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
		KGLOG_COM_CHECK_ERROR(hr);
	}
	KG_COM_RELEASE(pDepthSave);

	if (bSetViewport)
	{
		hr = g_pd3dDevice->SetViewport(&ViewportSave);
		KGLOG_COM_CHECK_ERROR(hr);
	}

	return hResult;
}

HRESULT KG3DSceneShadowMap::Init(DWORD dwShadowMapSize,KG3DSceneEntityMgr* pEntityMgr)
{
	HRESULT hResult = E_FAIL;
	HRESULT hr = E_FAIL;
	m_ShadowLevel[0].m_fSize = 2000;
	m_ShadowLevel[1].m_fSize = 12000;
	m_ShadowLevel[2].m_fSize = 40000;
	m_ShadowLevel[0].m_dwShadowmapSize = dwShadowMapSize;
	m_ShadowLevel[1].m_dwShadowmapSize = dwShadowMapSize;
	m_ShadowLevel[2].m_dwShadowmapSize = dwShadowMapSize;

	for (int nIndex = 0; nIndex < _countof(m_ShadowLevel); ++nIndex)
	{
		hr = m_ShadowLevel[nIndex].Init(this, nIndex , pEntityMgr);
		if (FAILED(hr))
		{
			for (int i = 0; i < nIndex; i++)
				m_ShadowLevel[i].UnInit();
			m_bInit = FALSE;
			KG_PROCESS_ERROR(0);
		}
	}
	m_vPositionLast = D3DXVECTOR3(0,0,0);

    m_bInit = TRUE;
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneShadowMap::UnInit()
{
	for (int nIndex = _countof(m_ShadowLevel) - 1; nIndex >= 0; --nIndex)
	{
		m_ShadowLevel[nIndex].UnInit();
	}

    m_bInit = FALSE;
	return S_OK;
}

HRESULT KG3DSceneShadowMap::OnLostDevice()
{
    HRESULT hr = S_OK;

	for (int nIndex = 0; nIndex < _countof(m_ShadowLevel); ++nIndex)
	{
		hr = m_ShadowLevel[nIndex].OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
	}

Exit0:
    return hr;
}

HRESULT KG3DSceneShadowMap::OnResetDevice()
{
    HRESULT hr = S_OK;

	for (int nIndex = _countof(m_ShadowLevel) - 1; nIndex >= 0; --nIndex)
	{
		hr = m_ShadowLevel[nIndex].OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
	}

Exit0:
    return hr;
}


//HRESULT KG3DSceneShadowMap::ShadowLevel::SmoothPostRenderShadow(KG3DSceneSceneEditor* pEditor)
//{
//	HRESULT hr = S_OK;
//	LPDIRECT3DSURFACE9 pDepthSave = NULL;
//	LPDIRECT3DSURFACE9 pTargetSave = NULL;
//	D3DVIEWPORT9 viewPortSave;
//	D3DVIEWPORT9 viewPort;
//	viewPort.X = viewPort.Y = 0;
//	viewPort.Height = viewPort.Width = m_dwShadowmapSize;
//	viewPort.MaxZ = 1;viewPort.MinZ =0;
//
//	hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
//	hr = pDepthSave->Release();
//
//	hr = g_pd3dDevice->GetRenderTarget(0,&pTargetSave);
//	hr = pTargetSave->Release();
//
//	g_pd3dDevice->GetViewport(&viewPortSave);
//
//	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpSMShadowMapSurface);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	D3DXVECTOR4 UV[9];
//	float x = 1.0F/m_dwShadowmapSize;
//	float y = 1.0F/m_dwShadowmapSize;
//
//	UV[0] = D3DXVECTOR4( 0, 0,0,0.3F);
//	UV[1] = D3DXVECTOR4(-x, 0,0,0.3F);
//	UV[2] = D3DXVECTOR4( x, 0,0,0.3F);
//	UV[3] = D3DXVECTOR4( 0,-y,0,0.3F);
//	UV[4] = D3DXVECTOR4( 0, y,0,0.3F);
//	UV[5] = D3DXVECTOR4(-x, y,0,0.3F);
//	UV[6] = D3DXVECTOR4( x, y,0,0.3F);
//	UV[7] = D3DXVECTOR4( x,-y,0,0.3F);
//	UV[8] = D3DXVECTOR4( x,-y,0,0.8F);
//
//	pEditor->m_lpEffectPostRender->SetVectorArray("g_avSampleOffsets",UV,9);
//	//////////////////////////////////////////////////////////////////////////
//
//	//hr = g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET , 0x0, 1, 0);
//	{
//		GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
//
//		GraphicsStruct::RenderState ZCmp_1(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
//		GraphicsStruct::RenderState ZWrite_1(D3DRS_ZWRITEENABLE,FALSE);
//		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
//
//		//for (int i=0;i<3;i++)
//		{
//			{
//				hr = g_pd3dDevice->SetRenderTarget(0,m_lpSMShadowMapColorSurface2);
//				KGLOG_COM_PROCESS_ERROR(hr);
//
//				hr = g_pd3dDevice->BeginScene();
//				g_pd3dDevice->SetViewport(&viewPort);
//
//				pEditor->m_lpEffectPostRender->SetTexture("tMainColor" ,m_lpSMShadowMapColor1);
//				pEditor->m_lpEffectPostRender->SetTechnique("tec2");
//
//				UINT uPass = 0;
//				pEditor->m_lpEffectPostRender->Begin(&uPass,0);
//				pEditor->m_lpEffectPostRender->BeginPass(0);
//
//				g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
//
//				pEditor->m_lpEffectPostRender->EndPass();
//				pEditor->m_lpEffectPostRender->End();
//
//				hr = g_pd3dDevice->EndScene();
//			}
//			{
//				hr = g_pd3dDevice->SetRenderTarget(0,m_lpSMShadowMapColorSurface1);
//				KGLOG_COM_PROCESS_ERROR(hr);
//
//				hr = g_pd3dDevice->BeginScene();
//				g_pd3dDevice->SetViewport(&viewPort);
//
//				pEditor->m_lpEffectPostRender->SetTexture("tMainColor" ,m_lpSMShadowMapColor2);
//				pEditor->m_lpEffectPostRender->SetTechnique("tec2");
//
//				UINT uPass = 0;
//				pEditor->m_lpEffectPostRender->Begin(&uPass,0);
//				pEditor->m_lpEffectPostRender->BeginPass(0);
//
//				g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
//
//				pEditor->m_lpEffectPostRender->EndPass();
//				pEditor->m_lpEffectPostRender->End();
//
//				hr = g_pd3dDevice->EndScene();
//			}
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//
//	hr = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
//	hr = g_pd3dDevice->SetRenderTarget(0,pTargetSave);
//	g_pd3dDevice->SetViewport(&viewPortSave);
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

HRESULT KG3DSceneShadowMap::SetParametersForShader(LPD3DXEFFECT pShader)
{
	KGLOG_PROCESS_ERROR(pShader);

	pShader->SetTexture("tShadowmap1",m_ShadowLevel[0].m_lpSMShadowMap);
	pShader->SetTexture("tShadowmap2",m_ShadowLevel[1].m_lpSMShadowMap);
	pShader->SetTexture("tShadowmap3",m_ShadowLevel[2].m_lpSMShadowMap);

	pShader->SetMatrix("matLightVP1",&m_ShadowLevel[0].m_matVP);
	pShader->SetMatrix("matLightVP2",&m_ShadowLevel[1].m_matVP);
	pShader->SetMatrix("matLightVP3",&m_ShadowLevel[2].m_matVP);
    /*
	D3DXVECTOR4 UV[9];
	float x = 1.0F/m_ShadowLevel[0].m_dwShadowmapSize;
	float y = 1.0F/m_ShadowLevel[0].m_dwShadowmapSize;

	UV[0] = D3DXVECTOR4( 0, 0,0,0.3F);
	UV[1] = D3DXVECTOR4(-x, 0,0,0.3F);
	UV[2] = D3DXVECTOR4( x, 0,0,0.3F);
	UV[3] = D3DXVECTOR4( 0,-y,0,0.3F);
	UV[4] = D3DXVECTOR4( 0, y,0,0.3F);
	UV[5] = D3DXVECTOR4(-x, y,0,0.3F);
	UV[6] = D3DXVECTOR4( x, y,0,0.3F);
	UV[7] = D3DXVECTOR4( x,-y,0,0.3F);
	UV[8] = D3DXVECTOR4( x,-y,0,0.8F);

	pShader->SetVectorArray("g_avSampleOffsets",UV,9);
    */
	return S_OK;
Exit0:
	return E_FAIL;
}


KG3DPointLightShadow::KG3DPointLightShadow()
{
	m_lpDepth = NULL;
}

KG3DPointLightShadow::~KG3DPointLightShadow()
{
	SAFE_RELEASE(m_lpDepth);
}

HRESULT KG3DPointLightShadow::Init(DWORD dwSize)
{
	HRESULT hr = E_FAIL;

	m_vShadowMap.m_bNeedColorSurface = TRUE;
	m_vShadowMap.m_bUseINTZ = TRUE;

	m_vShadowMap.Init(dwSize);

	hr = g_pd3dDevice->CreateDepthStencilSurface(dwSize,dwSize,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,FALSE,&m_lpDepth,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DPointLightShadow::RenderForCubeShadowmap()
{
	return S_OK;
}

extern void D3DUtil_GetCubeMapViewMatrix( DWORD dwFace,D3DXVECTOR3 vEyePt,KG3DCamera& vCamera );

HRESULT KG3DPointLightShadow::ComputeCubeMapForPosition(D3DXVECTOR3 vPosition,KG3DScenePvsEditor* pScene,LPDIRECT3DCUBETEXTURE9 pCubeColor,LPDIRECT3DCUBETEXTURE9 pCubeDepth,KG3DRenderTargetTool* pToolColor,KG3DRenderTargetTool* pToolDepth)
{
	HRESULT hr = E_FAIL;


	for( UINT i = 0; i < 6; i++ ) 
	{ 

		// Set the view transform for this cubemap surface 
		D3DUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES) i ,vPosition,m_vShadowMap.m_vCameraLight); 

		m_vShadowMap.m_vCameraLight.SetPerspective(D3DX_PI / 2,1,5,40000 );
		m_vShadowMap.m_vCameraLight.SetCamera();

		m_vShadowMap.ProcessShadowMapWithPVS((KG3DRepresentObjectPVS*)pScene->GetPvs());

		//////////////////////////////////////////////////////////////////////////
		if(pCubeDepth)
		{
			LPDIRECT3DSURFACE9 pDest = NULL;
			hr = pCubeDepth->GetCubeMapSurface((D3DCUBEMAP_FACES)i,0,&pDest);
			KGLOG_COM_PROCESS_ERROR(hr);

			ExportToCubeSurface(pToolDepth,pDest,TRUE);

			SAFE_RELEASE(pDest);
		}

		if(pCubeColor)
		{
			LPDIRECT3DSURFACE9 pDest = NULL;
			hr = pCubeColor->GetCubeMapSurface((D3DCUBEMAP_FACES)i,0,&pDest);
			KGLOG_COM_PROCESS_ERROR(hr);

			ExportToCubeSurface(pToolColor,pDest,FALSE);

			SAFE_RELEASE(pDest);
		}
	} 

	//////////////////////////////////////////////////////////////////////////

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DPointLightShadow::ComputeCubeMapForPointLight(KG3DModelPointLight* pLight,KG3DScenePvsEditor* pScene,KG3DRenderTargetTool* pTool)
{
	//////////////////////////////////////////////////////////////////////////
	KGLOG_PROCESS_ERROR(pLight);
	KGLOG_PROCESS_ERROR(pLight->m_lpCubeTexture);
	{

		D3DXVECTOR3 vPosition;
		vPosition.x = pLight->m_matWorld._41;
		vPosition.y = pLight->m_matWorld._42;
		vPosition.z = pLight->m_matWorld._43;

		return ComputeCubeMapForPosition(vPosition,pScene,
			NULL,pLight->m_lpCubeTexture,
			NULL,pTool);
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DPointLightShadow::ExportToCubeSurface(KG3DRenderTargetTool* pTool,LPDIRECT3DSURFACE9 pDestSurface,BOOL bOutputDepth)
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(pTool);

	pTool->BeginNewRenderTarget(NULL,NULL,NULL);
	//////////////////////////////////////////////////////////////////////////
	hr = g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00808080, 1, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

	{
		D3DXVECTOR2 A(0,0);
		D3DXVECTOR2 C(m_vShadowMap.m_dwShadowmapSize,m_vShadowMap.m_dwShadowmapSize);

		if(bOutputDepth)
		{
			D3DXMATRIX matInv;
			D3DXMatrixInverse(&matInv,NULL,&m_vShadowMap.m_vCameraLight.GetProjectionMatrix());

			LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_LINEARDEPTHOUTPUT);

			pShader->SetMatrix("invProj",&matInv);

			pShader->SetTechnique("tec0");
			UINT uPass = 0;
			pShader->Begin(&uPass,0);
			pShader->BeginPass(0);

			hr = g_pd3dDevice->BeginScene();
			KGLOG_COM_PROCESS_ERROR(hr);

			g_cGraphicsTool.DrawScreenRect(&A,&C,0,0xFFFFFFFF,m_vShadowMap.m_lpSMShadowMap);

			hr = g_pd3dDevice->EndScene();
			KGLOG_COM_PROCESS_ERROR(hr);

			pShader->EndPass();
			pShader->End();
		}
		else
		{
			g_cGraphicsTool.DrawScreenRect(&A,&C,0,0xFFFFFFFF,m_vShadowMap.m_lpSMColorMap);
		}
	}

	hr = S_OK;

Exit0:

	pTool->EndNewRenderTarget();

	pTool->ExportRenderData(pDestSurface,FALSE);

	return hr;
}
