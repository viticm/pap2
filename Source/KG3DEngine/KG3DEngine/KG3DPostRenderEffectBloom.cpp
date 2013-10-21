////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectBloom.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-20 11:55:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DPostRenderEffectBloom.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRenderState.h"
#include "KG3DSceneSceneEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace PostRender
{
    void KG3DPostRenderEffectBloom::CleanUp()
    {
        SAFE_RELEASE(m_lpBloomTexture_1);
        SAFE_RELEASE(m_lpBloomTexture_2);
        SAFE_RELEASE(m_pEffect);
    }

    HRESULT KG3DPostRenderEffectBloom::ReloadFX()
    {
        HRESULT hResult = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        SAFE_RELEASE(m_pEffect);

        hRetCode = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, BLOOM_FX_FILE, NULL, NULL, 0, NULL, &m_pEffect, NULL);
        KG_COM_PROCESS_ERROR(hRetCode);
        hResult = S_OK;
Exit0:
        if (FAILED(hResult))
            SAFE_RELEASE(m_pEffect);

        return hResult;
    }

    HRESULT KG3DPostRenderEffectBloom::ApplyEffect(KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        LPDIRECT3DSURFACE9 pTargetSave = NULL;
        D3DVIEWPORT9 ViewPort;
        bool bNeedRestore = false;

        KG_PROCESS_ERROR(m_pPostRenderEffectMgr);
        KG_PROCESS_ERROR(m_bEnable);

        g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
        g_pd3dDevice->GetViewport(&ViewPort);

        bNeedRestore = true;

        hRetCode = Scene_To_SceneScaled();
        KG_COM_PROCESS_ERROR(hRetCode);

       /* hRetCode = BrightPass_To_StarSource();
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = StarSource_To_BloomSource();
        KG_COM_PROCESS_ERROR(hRetCode);*/

        hRetCode = RenderBloom();
        KG_COM_PROCESS_ERROR(hRetCode);

        hResult = S_OK;
Exit0:
        if (bNeedRestore)
        {
            g_pd3dDevice->SetRenderTarget(0, pTargetSave);
            SAFE_RELEASE(pTargetSave);

            g_pd3dDevice->SetViewport(&ViewPort);
        }

        return hResult;
    }

    HRESULT KG3DPostRenderEffectBloom::GetBackBufferDesc(D3DSURFACE_DESC& TexDesc)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        LPDIRECT3DTEXTURE9 pTexScene = NULL;
        LPDIRECT3DSURFACE9 piTexSufer = NULL;

        KG_PROCESS_ERROR(m_pPostRenderEffectMgr);

        pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
        KG_PROCESS_ERROR(pTexScene);

        hRetCode = pTexScene->GetSurfaceLevel(0, &piTexSufer);
        KG_COM_PROCESS_ERROR(hRetCode);

        piTexSufer->GetDesc(&TexDesc);

        hResult = S_OK;
Exit0:
        KG_COM_RELEASE(piTexSufer);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectBloom::ClearTexture(LPDIRECT3DTEXTURE9 pTexture)
    {
        HRESULT hr = S_OK;
        LPDIRECT3DSURFACE9 pSurface = NULL;

        hr = pTexture->GetSurfaceLevel(0, &pSurface);
        if(SUCCEEDED(hr))
            g_pd3dDevice->ColorFill(pSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0));

        SAFE_RELEASE(pSurface);
        return S_OK;
    }

    UINT KG3DPostRenderEffectBloom::GetPass()
    {
        if (KG3DPostRenderEffectManager::m_sEffectParams.nBloomSampleNum < 2 || KG3DPostRenderEffectManager::m_sEffectParams.nBloomSampleNum > 16)
            return 0;

        return (KG3DPostRenderEffectManager::m_sEffectParams.nBloomSampleNum / 2) - 1;
    }

    HRESULT KG3DPostRenderEffectBloom::DrawFullScreenQuad(CoordRect c)
    { 
        return DrawFullScreenQuad( c.fLeftU, c.fTopV, c.fRightU, c.fBottomV ); 
    }

    HRESULT KG3DPostRenderEffectBloom::DrawFullScreenQuad(
        float fLeftU, float fTopV, float fRightU, float fBottomV
    )
    {
        D3DSURFACE_DESC dtdsdRT;
        LPDIRECT3DSURFACE9 pSurfRT = NULL;
        KG3DRenderState RenderState;

        g_pd3dDevice->GetRenderTarget(0, &pSurfRT);
        pSurfRT->GetDesc(&dtdsdRT);

        FLOAT fWidth5 = (FLOAT)dtdsdRT.Width - 0.5f;
        FLOAT fHeight5 = (FLOAT)dtdsdRT.Height - 0.5f;

        ScreenVertexTex svQuad[4];

        svQuad[0].p = D3DXVECTOR4(-0.5f, -0.5f, 0.5f, 1.0f);
        svQuad[0].t = D3DXVECTOR2(fLeftU, fTopV);

        svQuad[1].p = D3DXVECTOR4(fWidth5, -0.5f, 0.5f, 1.0f);
        svQuad[1].t = D3DXVECTOR2(fRightU, fTopV);

        svQuad[2].p = D3DXVECTOR4(-0.5f, fHeight5, 0.5f, 1.0f);
        svQuad[2].t = D3DXVECTOR2(fLeftU, fBottomV);

        svQuad[3].p = D3DXVECTOR4(fWidth5, fHeight5, 0.5f, 1.0f);
        svQuad[3].t = D3DXVECTOR2(fRightU, fBottomV);

		RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        g_pd3dDevice->SetFVF(ScreenVertexTex::FVF);
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertexTex));

        SAFE_RELEASE(pSurfRT);
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectBloom::Scene_To_SceneScaled() 
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

        LPDIRECT3DSURFACE9 pSurfScaledScene = NULL;
        LPDIRECT3DTEXTURE9 pTexScene = NULL;
        //D3DSURFACE_DESC    BackBufferDesc;
        CoordRect          coords;
        KG3DRenderState    Rs;
        
        UINT uiPassCount, uiPass;   
		float fMaxU = 0.f;
		float fMaxV = 0.f;

        pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
        KG_PROCESS_ERROR(pTexScene);

        hRetCode = m_lpBloomTexture_1->GetSurfaceLevel(0, &pSurfScaledScene);
        KG_COM_PROCESS_ERROR(hRetCode);

        //hRetCode = GetBackBufferDesc(BackBufferDesc);
        //KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pEffect->SetTechnique("DownScale4x4");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pEffect->SetFloat(TEXT("Gate"), KG3DPostRenderEffectManager::m_sEffectParams.fBloomGate);

        coords.fLeftU   = 0.0f;
        coords.fTopV    = 0.0f;
        coords.fRightU  = 1.0f;
        coords.fBottomV = 1.0f;

        //KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( 
        //    BackBufferDesc.Width, BackBufferDesc.Height, avSampleOffsets 
        //    );
		KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( 
			m_dwCropWidth, m_dwCropHeight, avSampleOffsets 
			);
        m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
				

		m_pPostRenderEffectMgr->GetDepthTexCoordScale(&fMaxU, &fMaxV);
		hRetCode = m_pEffect->SetFloat(TEXT("MaxU"), fMaxU);
		hRetCode = m_pEffect->SetFloat(TEXT("MaxV"), fMaxV);

        g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene );
        g_pd3dDevice->SetTexture( 0, pTexScene );
        Rs.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
 
        hRetCode = m_pEffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

		//D3DSURFACE_DESC dtdsdRT;
		//LPDIRECT3DSURFACE9 pSurfRT = NULL;
		//g_pd3dDevice->GetRenderTarget(0, &pSurfRT);
		//pSurfRT->GetDesc(&dtdsdRT);
		//FLOAT fWidth5 = (FLOAT)dtdsdRT.Width;
		//FLOAT fHeight5 = (FLOAT)dtdsdRT.Height;
        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pEffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
			//PostRender::DrawFullScreenQuad(coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV ,0,0,fWidth5,fHeight5);
            m_pEffect->EndPass();
        }

        m_pEffect->End();

		if (false)
		{
			D3DXSaveTextureToFile("e:\\test.bmp",D3DXIFF_BMP ,m_lpBloomTexture_1,NULL);
		}


        hResult = S_OK;
Exit0:
        g_pd3dDevice->SetTexture(0, NULL);

        SAFE_RELEASE( pSurfScaledScene ); 
        return hResult;
    }

    HRESULT KG3DPostRenderEffectBloom::BrightPass_To_StarSource()
    {
        HRESULT hResult  = E_FAIL;
//        HRESULT hRetCode = E_FAIL;
//
//        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
//        D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];
//
//        CoordRect       coords;
//        D3DSURFACE_DESC desc;
//
//        PDIRECT3DSURFACE9 pSurfStarSource;
//        hRetCode = m_pTexStarSource->GetSurfaceLevel( 0, &pSurfStarSource );
//        KG_COM_PROCESS_ERROR(hRetCode);
//
//        coords.fLeftU   = 0.0f;
//        coords.fTopV    = 0.0f;
//        coords.fRightU  = 1.0f;
//        coords.fBottomV = 1.0f;
//
//        hRetCode = m_pTexSceneScaled->GetLevelDesc( 0, &desc );
//        KG_COM_PROCESS_ERROR(hRetCode);
//
//        KG3DPostRenderEffectHDR::GetSampleOffsets_GaussBlur5x5( 
//            desc.Width, desc.Height, avSampleOffsets, avSampleWeights 
//            );
//        m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
//        m_pEffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));
//
//        m_pEffect->SetTechnique("GaussBlur5x5");
//
//        g_pd3dDevice->SetRenderTarget( 0, pSurfStarSource );
//        g_pd3dDevice->SetTexture( 0, m_pTexSceneScaled );
//        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//
//        UINT uiPassCount, uiPass;       
//        hRetCode = m_pEffect->Begin(&uiPassCount, 0);
//        KG_COM_PROCESS_ERROR(hRetCode);
//
//        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
//        {
//            m_pEffect->BeginPass(uiPass);
//            DrawFullScreenQuad( coords );
//            m_pEffect->EndPass();
//        }
//
//        m_pEffect->End();
//
//        hResult = S_OK;
//Exit0:
//        SAFE_RELEASE( pSurfStarSource);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectBloom::StarSource_To_BloomSource()
    {
        HRESULT hResult  = E_FAIL;

//        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
//        PDIRECT3DSURFACE9 pSurfBloomSource;
//        CoordRect       coords;
//        D3DSURFACE_DESC desc;
//
//        hRetCode = m_pTexBloomSource->GetSurfaceLevel( 0, &pSurfBloomSource );
//        KG_COM_PROCESS_ERROR(hRetCode);
//
//        coords.fLeftU   = 0.0f;
//        coords.fTopV    = 0.0f;
//        coords.fRightU  = 1.0f;
//        coords.fBottomV = 1.0f;
//
//        hRetCode = m_pTexSceneScaled->GetLevelDesc( 0, &desc );
//        KG_COM_PROCESS_ERROR(hRetCode);
//
//        KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale2x2( 
//            desc.Width, desc.Height, avSampleOffsets
//            );
//        m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
//
//        m_pEffect->SetTechnique("DownScale2x2");
//
//        g_pd3dDevice->SetRenderTarget( 0, pSurfBloomSource );
//        g_pd3dDevice->SetTexture( 0, m_pTexStarSource );
//        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//
//        UINT uiPassCount, uiPass;       
//        hRetCode = m_pEffect->Begin(&uiPassCount, 0);
//        KG_COM_PROCESS_ERROR(hRetCode);
//
//        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
//        {
//            m_pEffect->BeginPass(uiPass);
//            DrawFullScreenQuad( coords );
//            m_pEffect->EndPass();
//        }
//
//        m_pEffect->End();
//      
//        hResult = S_OK;
//Exit0:
//        SAFE_RELEASE( pSurfBloomSource);
        return hResult;
    }
    
    HRESULT KG3DPostRenderEffectBloom::RenderBloom()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        UINT uiPassCount, uiPass;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
        FLOAT       afSampleOffsets[MAX_SAMPLES];
        D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];

        LPDIRECT3DSURFACE9 lpBloomSurfface_1 = NULL;
        LPDIRECT3DSURFACE9 lpBloomSurfface_2 = NULL;

        D3DSURFACE_DESC   desc;
        CoordRect         coords;

        KG3DRenderState Rs;

        hRetCode = m_lpBloomTexture_1->GetSurfaceLevel(0, &lpBloomSurfface_1);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_lpBloomTexture_2->GetSurfaceLevel(0, &lpBloomSurfface_2);
        KG_COM_PROCESS_ERROR(hRetCode);

        coords.fLeftU   = 0.0f;
        coords.fTopV    = 0.0f;
        coords.fRightU  = 1.0f;
        coords.fBottomV = 1.0f;

        hRetCode = m_lpBloomTexture_1->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pEffect->SetTechnique("GaussBlur5x5");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        hRetCode = KG3DPostRenderEffectHDR::GetSampleOffsets_GaussBlur5x5(
            desc.Width, 
            desc.Height, 
            avSampleOffsets, 
            avSampleWeights, 
            1.0f 
            );

        m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pEffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        g_pd3dDevice->SetRenderTarget(0, lpBloomSurfface_2 );
        g_pd3dDevice->SetTexture( 0, m_lpBloomTexture_1 );
        Rs.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        Rs.SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

        hRetCode = m_pEffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pEffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pEffect->EndPass();
        }
        m_pEffect->End();


        // bloom

        hRetCode = m_lpBloomTexture_2->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetSampleOffsets_Bloom( 
            desc.Width, 
            afSampleOffsets, 
            avSampleWeights, 
            KG3DPostRenderEffectManager::m_sEffectParams.fBloomGaussCoefficient,
            2.0f, 
            KG3DPostRenderEffectManager::m_sEffectParams.nBloomSampleNum
            );
        for(int i = 0; i < MAX_SAMPLES; i++ )
            avSampleOffsets[i] = D3DXVECTOR2(afSampleOffsets[i], 0.0f);

        hRetCode = m_pEffect->SetTechnique("Bloom");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pEffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        g_pd3dDevice->SetRenderTarget(0, lpBloomSurfface_1);
        g_pd3dDevice->SetTexture( 0, m_lpBloomTexture_2 );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


        hRetCode = m_pEffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

       // for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pEffect->BeginPass(GetPass());
            DrawFullScreenQuad( coords );
            m_pEffect->EndPass();
        }
        m_pEffect->End();

        hRetCode = m_lpBloomTexture_1->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetSampleOffsets_Bloom(
            desc.Height, 
            afSampleOffsets, 
            avSampleWeights, 
            KG3DPostRenderEffectManager::m_sEffectParams.fBloomGaussCoefficient,//3.0f, 
            2.0f,
            KG3DPostRenderEffectManager::m_sEffectParams.nBloomSampleNum
            );
        for (int i = 0; i < MAX_SAMPLES; i++ )
            avSampleOffsets[i] = D3DXVECTOR2( 0.0f, afSampleOffsets[i] );

        hRetCode = m_pEffect->SetTechnique("Bloom");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pEffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        g_pd3dDevice->SetRenderTarget(0, lpBloomSurfface_2);
        g_pd3dDevice->SetTexture(0, m_lpBloomTexture_1);
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


        hRetCode = m_pEffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        //for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pEffect->BeginPass(GetPass());
            DrawFullScreenQuad( coords );
            m_pEffect->EndPass();
        }

        m_pEffect->End();

        hResult = S_OK;

Exit0:
        SAFE_RELEASE( lpBloomSurfface_1 );
        SAFE_RELEASE( lpBloomSurfface_2 );

        return hResult;
    }

    HRESULT KG3DPostRenderEffectBloom::InitTextures()
    {
        HRESULT hr = S_OK;

		UnInitTextures();

        ASSERT(m_dwCropWidth / 8 > 0 && m_dwCropHeight / 8 > 0);
        ASSERT(m_lpBloomTexture_1 == NULL);
        ASSERT(m_lpBloomTexture_2 == NULL);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 8, m_dwCropHeight / 8, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpBloomTexture_1, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 8, m_dwCropHeight / 8, 1,
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpBloomTexture_2, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        ClearTexture(m_lpBloomTexture_1);
        ClearTexture(m_lpBloomTexture_2);

Exit0:
        if (FAILED(hr))
        {
            UnInitTextures();
        }
        return hr;
    }

    HRESULT KG3DPostRenderEffectBloom::UnInitTextures()
    {
        HRESULT hr = S_OK;

        SAFE_RELEASE(m_lpBloomTexture_1);
        SAFE_RELEASE(m_lpBloomTexture_2);

        return hr;
    }

    HRESULT KG3DPostRenderEffectBloom::OnLostDevice()
    {
        HRESULT hr = S_OK;
        
        if (m_pEffect)
        {
            hr = m_pEffect->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hr = UnInitTextures();
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return hr;
    }

    HRESULT KG3DPostRenderEffectBloom::OnResetDevice()
    {
        HRESULT hr = S_OK;
        BOOL bInitTextures = FALSE;

        if (m_bEnable)
        {
            hr = InitTextures();
            KGLOG_COM_PROCESS_ERROR(hr);

            bInitTextures = TRUE;
        }

        if (m_pEffect)
        {
            hr = m_pEffect->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

Exit0:
        if (FAILED(hr))
        {
            if (bInitTextures)
            {
                UnInitTextures();
            }
        }
        return hr;
    }

    HRESULT KG3DPostRenderEffectBloom::Enable(BOOL bEnable)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
        BOOL bInitTextures = FALSE;

        DWORD dwCropWidthSave = m_dwCropWidth;
        DWORD dwCropHeightSave = m_dwCropHeight;

        if (bEnable)
        {
			DWORD dwHeight = 0,dwWidth = 0;
			IEKG3DScene* pScene = g_cGraphicsTool.GetScene();
			if (pScene)
			{
				IEKG3DSceneOutputWnd* pOutputWindow = NULL;
				pScene->GetCurOutputWnd(&pOutputWindow);
				if (pOutputWindow)
				{
					pOutputWindow->GetViewPortWidthHeight(&dwWidth,&dwHeight);
				}
			}
			if (m_bEnable)
            {
                if ((dwWidth  == m_dwCropWidth) &&
                    (dwHeight == m_dwCropHeight))
                {
                    return S_OK;
                }
            }

            CleanUp();

            m_dwCropWidth  = dwWidth;
            m_dwCropHeight = dwHeight;

            hRetCode = InitTextures();
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            bInitTextures = TRUE;

            hRetCode = ReloadFX();
            KG_COM_PROCESS_ERROR(hRetCode);
        }
        else
        {
            CleanUp();
        }

        m_bEnable = bEnable;

        hResult = S_OK;
Exit0:
        if (FAILED(hResult))
        {
            if (bInitTextures)
            {
                UnInitTextures();
                m_dwCropWidth = dwCropWidthSave;
                m_dwCropHeight = dwCropHeightSave; 
            }
        }
        return hResult;
    }

    KG3DPostRenderEffectBloom::KG3DPostRenderEffectBloom(KG3DPostRenderEffectManager *pMgr)
        : KG3DPostRenderEffectBase(pMgr)
    {
        m_lpBloomTexture_1 = NULL;
        m_lpBloomTexture_2 = NULL;
        m_pEffect          = NULL;

        m_dwCropWidth  = 0;
        m_dwCropHeight = 0;

    }

    KG3DPostRenderEffectBloom::~KG3DPostRenderEffectBloom()
    {
        CleanUp();
    }



	KG3DPostRenderEffectDetail::KG3DPostRenderEffectDetail(KG3DPostRenderEffectManager *pMgr)
		: KG3DPostRenderEffectBase(pMgr)
	{
		m_lpTextureLocalAverage0 = NULL;
		m_lpTextureness0 = NULL;
		m_lpTextureLocalAverage1 = NULL;
		m_lpTextureness1 = NULL;
		m_lpTextureLocalAverage2 = NULL;
		m_lpTextureness2 = NULL;
		m_pEffect = NULL;
		m_dwCropWidth = 0;
		m_dwCropHeight = 0;
	}

	KG3DPostRenderEffectDetail:: ~KG3DPostRenderEffectDetail()
	{
		CleanUp();
	}

	HRESULT KG3DPostRenderEffectDetail::ReloadFX()
	{
		HRESULT hResult = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		SAFE_RELEASE(m_pEffect);

		hRetCode = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, DETAIL_FX_FILE, NULL, NULL, 0, NULL, &m_pEffect, NULL);
		KG_COM_PROCESS_ERROR(hRetCode);

		hResult = S_OK;
Exit0:
		if (FAILED(hResult))
			SAFE_RELEASE(m_pEffect);

		return hResult;
	}

	HRESULT KG3DPostRenderEffectDetail::ApplyEffect(KG3DScene* pScene)
	{   
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;


		KG_PROCESS_ERROR(m_pPostRenderEffectMgr);
		KG_PROCESS_ERROR(m_bEnable);

		hRetCode = RenderLocalAverage();
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = RenderTextureness();
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = RenderLocalAverage1(m_lpTextureLocalAverage0,m_lpTextureLocalAverage1);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = RenderTextureness1(m_lpTextureness1,m_lpTextureLocalAverage0,m_lpTextureLocalAverage1);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = RenderLocalAverage1(m_lpTextureLocalAverage1,m_lpTextureLocalAverage2);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = RenderTextureness1(m_lpTextureness2,m_lpTextureLocalAverage1,m_lpTextureLocalAverage2);
		KG_COM_PROCESS_ERROR(hRetCode);
		/* hRetCode = BrightPass_To_StarSource();
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = StarSource_To_BloomSource();
		KG_COM_PROCESS_ERROR(hRetCode);*/

		//hRetCode = RenderBloom();
		//KG_COM_PROCESS_ERROR(hRetCode);

		hResult = S_OK;
Exit0:
		return hResult;
	}

	HRESULT KG3DPostRenderEffectDetail::GetBackBufferDesc(D3DSURFACE_DESC& TexDesc)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		LPDIRECT3DTEXTURE9 pTexScene = NULL;
		LPDIRECT3DSURFACE9 pTexSufer = NULL;

		KG_PROCESS_ERROR(m_pPostRenderEffectMgr);

		pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
		KG_PROCESS_ERROR(pTexScene);

		hRetCode = pTexScene->GetSurfaceLevel(0, &pTexSufer);
		KG_COM_PROCESS_ERROR(hRetCode);

		pTexSufer->GetDesc(&TexDesc);

		hResult = S_OK;
Exit0:
        KG_COM_RELEASE(pTexSufer);
		return hResult;
	}

	void KG3DPostRenderEffectDetail::CleanUp()
	{
        UnInitTextures();

        SAFE_RELEASE(m_pEffect);
	}

    HRESULT KG3DPostRenderEffectDetail::UnInitTextures()
    {
		SAFE_RELEASE(m_lpTextureLocalAverage0);
		SAFE_RELEASE(m_lpTextureness0);
		SAFE_RELEASE(m_lpTextureLocalAverage1);
		SAFE_RELEASE(m_lpTextureness1);
		SAFE_RELEASE(m_lpTextureLocalAverage2);
		SAFE_RELEASE(m_lpTextureness2);

        m_dwCropHeight = 0;
        m_dwCropWidth  = 0;

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectDetail::InitTextures()
    {
        HRESULT hr = S_OK;

		ASSERT(m_lpTextureLocalAverage0 == NULL);
		ASSERT(m_lpTextureness0 == NULL);
		ASSERT(m_lpTextureLocalAverage1 == NULL);
		ASSERT(m_lpTextureness1 == NULL);
		ASSERT(m_lpTextureLocalAverage2 == NULL);
		ASSERT(m_lpTextureness2 == NULL);

        m_dwCropWidth = m_pPostRenderEffectMgr->GetWidth();
        m_dwCropHeight = m_pPostRenderEffectMgr->GetHeight();

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 2, m_dwCropHeight / 2, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTextureLocalAverage0, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth, m_dwCropHeight, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTextureness0, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 4,  m_dwCropHeight / 4, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTextureLocalAverage1, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 2, m_dwCropHeight / 2, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTextureness1, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 8, m_dwCropHeight / 8, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTextureLocalAverage2, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr= g_pd3dDevice->CreateTexture(m_dwCropWidth / 4, m_dwCropHeight/4, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTextureness2, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        if (FAILED(hr))
        {
            UnInitTextures();
        }
        return hr;
    }

	HRESULT KG3DPostRenderEffectDetail::Enable(BOOL bEnable)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;
        BOOL bInitTextures = FALSE;

        if (bEnable)
        {
            if (m_bEnable)
            {
                if ((m_pPostRenderEffectMgr->GetWidth() == m_dwCropWidth) &&
                    (m_pPostRenderEffectMgr->GetHeight() == m_dwCropHeight))
                {
                    return S_OK;
                }
            }

            CleanUp();

            hRetCode = InitTextures();
			KGLOG_COM_PROCESS_ERROR(hRetCode);

            bInitTextures = TRUE;

            hRetCode = ReloadFX();
            KGLOG_COM_PROCESS_ERROR(hRetCode);
        }
        else
        {
            CleanUp();
        }

		m_bEnable = bEnable;

		hResult = S_OK;
Exit0:
        if (FAILED(hResult))
        {
            if (bInitTextures)
            {
                UnInitTextures();
            }
        }
		return hResult;
	}

    HRESULT KG3DPostRenderEffectDetail::OnLostDevice()
    {
        HRESULT hr = S_OK;

        if (m_pEffect)
        {
            hr = m_pEffect->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hr = UnInitTextures();
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return hr;
    }

    HRESULT KG3DPostRenderEffectDetail::OnResetDevice()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;
        BOOL bInitTextures = FALSE;

        if (m_bEnable)
        {
			hr = UnInitTextures();
			KGLOG_COM_PROCESS_ERROR(hr);

            hr = InitTextures();
            KGLOG_COM_PROCESS_ERROR(hr);

            bInitTextures = TRUE;
        }

        if (m_pEffect)
        {
            hr = m_pEffect->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hrResult = S_OK;
Exit0:
        if (FAILED(hrResult))
        {
            if (bInitTextures)
            {
                UnInitTextures();
            }
        }
        return hrResult;
    }

	HRESULT KG3DPostRenderEffectDetail::RenderLocalAverage()
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

		LPDIRECT3DSURFACE9 pSurfScaledScene = NULL;
		LPDIRECT3DTEXTURE9 pTexScene = NULL;
		D3DSURFACE_DESC    BackBufferDesc;
		CoordRect          coords;
        KG3DRenderState    Rs;

		UINT uiPassCount, uiPass;      

		pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
		KG_PROCESS_ERROR(pTexScene);

		hRetCode = m_lpTextureLocalAverage0->GetSurfaceLevel(0, &pSurfScaledScene);
		KG_COM_PROCESS_ERROR(hRetCode);

		GetBackBufferDesc(BackBufferDesc);

        hRetCode = m_pEffect->SetTechnique("LocalAverage");
        KGLOG_COM_CHECK_ERROR(hRetCode);

		coords.fLeftU   = 0.0f;
		coords.fTopV    = 0.0f;
		coords.fRightU  = 1.0f;
		coords.fBottomV = 1.0f;

		KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( 
			BackBufferDesc.Width, BackBufferDesc.Height, avSampleOffsets 
			);
		m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

		g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene );
		g_pd3dDevice->SetTexture( 0, pTexScene );
		Rs.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

		hRetCode = m_pEffect->Begin(&uiPassCount, 0);
		KG_COM_PROCESS_ERROR(hRetCode);

		hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
		for (uiPass = 0; uiPass < uiPassCount; uiPass++)
		{
			m_pEffect->BeginPass(uiPass);
			KG3DPostRenderEffectBloom::DrawFullScreenQuad( coords );
			m_pEffect->EndPass();
		}

		m_pEffect->End();

		hResult = S_OK;
Exit0:
		g_pd3dDevice->SetTexture(0, NULL);

		SAFE_RELEASE( pSurfScaledScene ); 
		return hResult;
	}

	HRESULT KG3DPostRenderEffectDetail::RenderLocalAverage1(LPDIRECT3DTEXTURE9 pSrc,LPDIRECT3DTEXTURE9 pDest)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

		LPDIRECT3DSURFACE9 pSurfScaledScene = NULL;
		LPDIRECT3DTEXTURE9 pTexScene = NULL;
		D3DSURFACE_DESC    BackBufferDesc;
		CoordRect          coords;
        KG3DRenderState    Rs;

		UINT uiPassCount, uiPass;      

		pTexScene = pSrc;
		KG_PROCESS_ERROR(pTexScene);

		hRetCode = pDest->GetSurfaceLevel(0, &pSurfScaledScene);
		KG_COM_PROCESS_ERROR(hRetCode);

		pDest->GetLevelDesc(0,&BackBufferDesc);

		hRetCode = m_pEffect->SetTechnique("LocalAverage");
        KGLOG_COM_CHECK_ERROR(hRetCode);

		coords.fLeftU   = 0.0f;
		coords.fTopV    = 0.0f;
		coords.fRightU  = 1.0f;
		coords.fBottomV = 1.0f;

		KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( 
			BackBufferDesc.Width, BackBufferDesc.Height, avSampleOffsets 
			);
		m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

		g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene );
		g_pd3dDevice->SetTexture( 0, pTexScene );
		Rs.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

		hRetCode = m_pEffect->Begin(&uiPassCount, 0);
		KG_COM_PROCESS_ERROR(hRetCode);

		hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
		for (uiPass = 0; uiPass < uiPassCount; uiPass++)
		{
			m_pEffect->BeginPass(uiPass);
			KG3DPostRenderEffectBloom::DrawFullScreenQuad( coords );
			m_pEffect->EndPass();
		}

		m_pEffect->End();

		hResult = S_OK;
Exit0:
		g_pd3dDevice->SetTexture(0, NULL);

        SAFE_RELEASE( pSurfScaledScene ); 
		return hResult;
	}

	HRESULT KG3DPostRenderEffectDetail::RenderTextureness()
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		LPDIRECT3DSURFACE9 pSurfScaledScene = NULL;
		LPDIRECT3DTEXTURE9 pTexScene = NULL;
		D3DSURFACE_DESC    BackBufferDesc;
		CoordRect          coords;
        KG3DRenderState    Rs;

		UINT uiPassCount, uiPass;      

		pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
		KG_PROCESS_ERROR(pTexScene);

		hRetCode = m_lpTextureness0->GetSurfaceLevel(0, &pSurfScaledScene);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = GetBackBufferDesc(BackBufferDesc);
        KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = m_pEffect->SetTechnique("Textureness");
        KGLOG_COM_CHECK_ERROR(hRetCode);

		coords.fLeftU   = 0.0f;
		coords.fTopV    = 0.0f;
		coords.fRightU  = 1.0f;
		coords.fBottomV = 1.0f;

		g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene );
		g_pd3dDevice->SetTexture( 0, pTexScene );
		g_pd3dDevice->SetTexture( 1, m_lpTextureLocalAverage0 );
		Rs.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

		hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);

		hRetCode = m_pEffect->Begin(&uiPassCount, 0);
		KG_COM_PROCESS_ERROR(hRetCode);

		for (uiPass = 0; uiPass < uiPassCount; uiPass++)
		{
			m_pEffect->BeginPass(uiPass);
			KG3DPostRenderEffectBloom::DrawFullScreenQuad( coords );
			m_pEffect->EndPass();
		}

		m_pEffect->End();

		hResult = S_OK;
Exit0:
		g_pd3dDevice->SetTexture(0, NULL);

		SAFE_RELEASE( pSurfScaledScene ); 
		return hResult;
	}


	HRESULT KG3DPostRenderEffectDetail::RenderTextureness1(LPDIRECT3DTEXTURE9 pDest,LPDIRECT3DTEXTURE9 pSrcText,LPDIRECT3DTEXTURE9 pSrcAverage)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		LPDIRECT3DSURFACE9 pSurfScaledScene = NULL;
		LPDIRECT3DTEXTURE9 pTexScene = NULL;
		D3DSURFACE_DESC    BackBufferDesc;
		CoordRect          coords;
        KG3DRenderState    Rs;

		UINT uiPassCount, uiPass;      

		pTexScene = pSrcText;
		KG_PROCESS_ERROR(pTexScene);

		hRetCode = pDest->GetSurfaceLevel(0, &pSurfScaledScene);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = GetBackBufferDesc(BackBufferDesc);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pEffect->SetTechnique("Textureness");
        KGLOG_COM_CHECK_ERROR(hRetCode);

		coords.fLeftU   = 0.0f;
		coords.fTopV    = 0.0f;
		coords.fRightU  = 1.0f;
		coords.fBottomV = 1.0f;

		g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene );
		g_pd3dDevice->SetTexture( 0, pTexScene );
		g_pd3dDevice->SetTexture( 1, pSrcAverage );
		Rs.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

		hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);

		hRetCode = m_pEffect->Begin(&uiPassCount, 0);
		KG_COM_PROCESS_ERROR(hRetCode);

		for (uiPass = 0; uiPass < uiPassCount; uiPass++)
		{
			m_pEffect->BeginPass(uiPass);
			KG3DPostRenderEffectBloom::DrawFullScreenQuad( coords );
			m_pEffect->EndPass();
		}

		m_pEffect->End();

		hResult = S_OK;
Exit0:
		g_pd3dDevice->SetTexture(0, NULL);
		g_pd3dDevice->SetTexture(1, NULL);

        SAFE_RELEASE( pSurfScaledScene ); 
		return hResult;
	}

	HRESULT KG3DPostRenderEffectDetail::RenderDetail()
	{
		HRESULT hResult  = E_FAIL;

		return hResult;
	}
}