////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectHDR.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-8 10:52:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DPostRenderEffectHDR.h"
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
    //
    // ------------------ tool function ---------------------
    //

    HRESULT KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( 
        DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[] 
    )
    {
        if( NULL == avSampleOffsets )
            return E_INVALIDARG;

        float tU = 1.0f / dwWidth;
        float tV = 1.0f / dwHeight;

        int index=0;
        for( int y=0; y < 4; y++ )
        {
            for( int x=0; x < 4; x++ )
            {
                avSampleOffsets[ index ].x = (x - 1.5f) * tU;
                avSampleOffsets[ index ].y = (y - 1.5f) * tV;

                index++;
            }
        }
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectHDR::GetSampleOffsets_GaussBlur5x5(
        DWORD        dwD3DTexWidth,
        DWORD        dwD3DTexHeight,
        D3DXVECTOR2* avTexCoordOffset,
        D3DXVECTOR4* avSampleWeight,
        FLOAT        fMultiplier
    )
    {
        float tu = 1.0f / (float)dwD3DTexWidth ;
        float tv = 1.0f / (float)dwD3DTexHeight ;

        D3DXVECTOR4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

        float totalWeight = 0.0f;
        int index=0;
        for( int x = -2; x <= 2; x++ )
        {
            for( int y = -2; y <= 2; y++ )
            {
                if( abs(x) + abs(y) > 2 )
                    continue;

                avTexCoordOffset[index] = D3DXVECTOR2( x * tu, y * tv );
                avSampleWeight[index] = vWhite * GaussianDistribution( (float)x, (float)y, 1.0f );
                totalWeight += avSampleWeight[index].x;

                index++;
            }
        }

        for( int i=0; i < index; i++ )
        {
            avSampleWeight[i] /= totalWeight;
            avSampleWeight[i] *= fMultiplier;
        }

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale2x2( 
        DWORD dwWidth, 
        DWORD dwHeight, 
        D3DXVECTOR2 avSampleOffsets[] 
    )
    {
        if( NULL == avSampleOffsets )
            return E_INVALIDARG;

        float tU = 1.0f / dwWidth;
        float tV = 1.0f / dwHeight;

        int index=0;
        for( int y=0; y < 2; y++ )
        {
            for( int x=0; x < 2; x++ )
            {
                avSampleOffsets[ index ].x = (x - 0.5f) * tU;
                avSampleOffsets[ index ].y = (y - 0.5f) * tV;

                index++;
            }
        }

        return S_OK;
    }


    HRESULT KG3DPostRenderEffectHDR::ClearTexture( LPDIRECT3DTEXTURE9 pTexture )
    {
        HRESULT hr = S_OK;
        LPDIRECT3DSURFACE9 pSurface = NULL;

        hr = pTexture->GetSurfaceLevel( 0, &pSurface );
        if( SUCCEEDED(hr) )
            g_pd3dDevice->ColorFill( pSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0) );

        SAFE_RELEASE( pSurface );
        return hr;
    }

    HRESULT KG3DPostRenderEffectHDR::DrawFullScreenQuad(CoordRect c)
    { 
        return DrawFullScreenQuad( c.fLeftU, c.fTopV, c.fRightU, c.fBottomV ); 
    }

    HRESULT KG3DPostRenderEffectHDR::DrawFullScreenQuad(
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

    HRESULT KG3DPostRenderEffectHDR::Scene_To_SceneScaled() 
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

        PDIRECT3DSURFACE9  pSurfScaledScene = NULL;
        LPDIRECT3DTEXTURE9 pTexScene = NULL;
        D3DSURFACE_DESC    BackBufferDesc;
		//float fScalU = 1.0f,fScalV = 1.0f;
		//DWORD dwHeight = 0,dwWidth = 0;
		//float tU = 0, tV = 0;
		CoordRect coords = CoordRect();
		    
        pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
        KG_PROCESS_ERROR(pTexScene);

        // get the new render target surface
        hRetCode = m_pTexSceneScaled->GetSurfaceLevel(0, &pSurfScaledScene);
        KG_COM_PROCESS_ERROR(hRetCode);

        GetBackBufferDesc(BackBufferDesc);

        hRetCode = m_pHDREffect->SetTechnique("DownScale4x4");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        // place the rectangle in the center of the back buffer surface
        RECT rectSrc;
        rectSrc.left   = (BackBufferDesc.Width  - m_dwCropWidth)  / 2;
        rectSrc.top    = (BackBufferDesc.Height - m_dwCropHeight) / 2;
        rectSrc.right  = rectSrc.left + m_dwCropWidth;
        rectSrc.bottom = rectSrc.top  + m_dwCropHeight;

        // get the texture coordinates for the render target
        GetTextureCoords(pTexScene, &rectSrc, m_pTexSceneScaled, NULL, &coords);

        // get the sample offsets used within the pixel shader
        GetSampleOffsets_DownScale4x4( BackBufferDesc.Width, BackBufferDesc.Height, avSampleOffsets );
        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
     
        g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene );
        g_pd3dDevice->SetTexture( 0, pTexScene );
		/*
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );*/
		GraphicsStruct::SamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);

        UINT uiPassCount, uiPass;       
        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
   
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();

		if (false)
		{
			HRESULT hr = D3DXSaveTextureToFile("e:\\test.bmp",D3DXIFF_BMP ,pTexScene,NULL);
			hr = D3DXSaveTextureToFile("e:\\test2.bmp",D3DXIFF_BMP,m_pTexSceneScaled,NULL);
		}
        hResult = S_OK;
Exit0:
		g_pd3dDevice->SetTexture(0, NULL);

        SAFE_RELEASE(pSurfScaledScene); 
		//SAFE_RELEASE(pTexScene);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::MeasureLuminance()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        UINT uiPassCount, uiPass;
        int index;
        
        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

        DWORD dwCurTexture = NUM_TONEMAP_TEXTURES - 1;

        // Sample log average luminance
        PDIRECT3DSURFACE9 apSurfToneMap[NUM_TONEMAP_TEXTURES] = {0};

        D3DSURFACE_DESC desc;
        float tU, tV;

        // Retrieve the tonemap surfaces
        for(int i = 0; i < NUM_TONEMAP_TEXTURES; i++ )
        {
            hRetCode = m_apTexToneMap[i]->GetSurfaceLevel( 0, &apSurfToneMap[i] );
            KG_COM_PROCESS_ERROR(hRetCode);
        }


        m_apTexToneMap[dwCurTexture]->GetLevelDesc( 0, &desc );

        // Initialize the sample offsets for the initial luminance pass.

        tU = 1.0f / (3.0f * desc.Width);
        tV = 1.0f / (3.0f * desc.Height);

        index = 0;
        for( int x = -1; x <= 1; x++ )
        {
            for( int y = -1; y <= 1; y++ )
            {
                avSampleOffsets[index].x = x * tU;
                avSampleOffsets[index].y = y * tV;

                index++;
            }
        }

        hRetCode = m_pHDREffect->SetTechnique("SampleAvgLum");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

        g_pd3dDevice->SetRenderTarget(0, apSurfToneMap[dwCurTexture]);
        g_pd3dDevice->SetTexture(0, m_pTexSceneScaled);
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();
        
        dwCurTexture--;

        // Initialize the sample offsets for the iterative luminance passes
        while (dwCurTexture > 0)
        {
            m_apTexToneMap[dwCurTexture+1]->GetLevelDesc( 0, &desc );
            GetSampleOffsets_DownScale4x4( desc.Width, desc.Height, avSampleOffsets );

            hRetCode = m_pHDREffect->SetTechnique("ResampleAvgLum");
            KGLOG_COM_CHECK_ERROR(hRetCode);

            m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

            g_pd3dDevice->SetRenderTarget(0, apSurfToneMap[dwCurTexture]);
            g_pd3dDevice->SetTexture(0, m_apTexToneMap[dwCurTexture + 1]);
            g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


            hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
            KG_COM_PROCESS_ERROR(hRetCode);

            for (uiPass = 0; uiPass < uiPassCount; uiPass++)
            {
                m_pHDREffect->BeginPass(uiPass);
                DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);
                m_pHDREffect->EndPass();
            }

            m_pHDREffect->End();
            dwCurTexture--;
        }

        m_apTexToneMap[1]->GetLevelDesc( 0, &desc );
        GetSampleOffsets_DownScale4x4( desc.Width, desc.Height, avSampleOffsets );

        hRetCode = m_pHDREffect->SetTechnique("ResampleAvgLumExp");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

        g_pd3dDevice->SetRenderTarget(0, apSurfToneMap[0]);
        g_pd3dDevice->SetTexture(0, m_apTexToneMap[1]);
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();


        hResult = S_OK;
Exit0:
        for(int i = 0; i < NUM_TONEMAP_TEXTURES; i++ )
            SAFE_RELEASE(apSurfToneMap[i]);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::CalculateAdaptation()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        UINT uiPass, uiPassCount;

        PDIRECT3DTEXTURE9 pTexSwap = m_pTexAdaptedLuminanceLast;
        PDIRECT3DSURFACE9 pSurfAdaptedLum = NULL;
        
        m_pTexAdaptedLuminanceLast = m_pTexAdaptedLuminanceCur;
        m_pTexAdaptedLuminanceCur = pTexSwap;

        hRetCode = m_pTexAdaptedLuminanceCur->GetSurfaceLevel(0, &pSurfAdaptedLum);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pHDREffect->SetTechnique("CalculateAdaptedLum");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pHDREffect->SetFloat("g_fElapsedTime", g_cGraphicsTool.GetTinyTime() / 1000.0f);

        g_pd3dDevice->SetRenderTarget(0, pSurfAdaptedLum);
        g_pd3dDevice->SetTexture(0, m_pTexAdaptedLuminanceLast);
        g_pd3dDevice->SetTexture(1, m_apTexToneMap[0]);
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();

        hResult = S_OK;
Exit0:
        SAFE_RELEASE( pSurfAdaptedLum );
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::SceneScaled_To_BrightPass()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
        D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];

        PDIRECT3DSURFACE9 pSurfBrightPass;
        D3DSURFACE_DESC desc;

        RECT rectSrc;
        RECT rectDest;

        hRetCode = m_pTexBrightPass->GetSurfaceLevel( 0, &pSurfBrightPass );
        KG_COM_PROCESS_ERROR(hRetCode);
        
        m_pTexSceneScaled->GetLevelDesc( 0, &desc );

        hRetCode = GetSampleOffsets_GaussBlur5x5(
            desc.Width, 
            desc.Height, 
            avSampleOffsets,
            avSampleWeights 
            );
        KG_COM_PROCESS_ERROR(hRetCode);

        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets) );
        m_pHDREffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights) );

        GetTextureRect(m_pTexSceneScaled, &rectSrc);
        InflateRect(&rectSrc, -1, -1);

        GetTextureRect(m_pTexBrightPass, &rectDest);
        InflateRect(&rectDest, -1, -1);

        CoordRect coords;
        GetTextureCoords(m_pTexSceneScaled, &rectSrc, m_pTexBrightPass, &rectDest, &coords);

        hRetCode = m_pHDREffect->SetTechnique("BrightPassFilter");
        KGLOG_COM_CHECK_ERROR(hRetCode);
		m_pHDREffect->SetFloat("BrightGate", PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fHDRBrightGate);
		m_pHDREffect->SetFloat("Rectify", PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fHDRRectify);
		m_pHDREffect->SetFloat("GrayLevel", PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fHDRGrayLevel);
        g_pd3dDevice->SetRenderTarget( 0, pSurfBrightPass );
        g_pd3dDevice->SetTexture( 0, m_pTexSceneScaled );
        g_pd3dDevice->SetTexture( 1, m_pTexAdaptedLuminanceCur );
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
        g_pd3dDevice->SetScissorRect( &rectDest );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

        UINT uiPass, uiPassCount;
        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

        hResult = S_OK;
Exit0:
        SAFE_RELEASE( pSurfBrightPass );
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::BrightPass_To_StarSource()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
        D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];

        RECT            rectDest;
        CoordRect       coords;
        D3DSURFACE_DESC desc;

        PDIRECT3DSURFACE9 pSurfStarSource;
        hRetCode = m_pTexStarSource->GetSurfaceLevel( 0, &pSurfStarSource );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetTextureRect(m_pTexStarSource, &rectDest);
        InflateRect(&rectDest, -1, -1);

        GetTextureCoords(m_pTexBrightPass, NULL, m_pTexStarSource, &rectDest, &coords);

        hRetCode = m_pTexBrightPass->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetSampleOffsets_GaussBlur5x5( desc.Width, desc.Height, avSampleOffsets, avSampleWeights );
        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pHDREffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        hRetCode = m_pHDREffect->SetTechnique("GaussBlur5x5");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        g_pd3dDevice->SetRenderTarget( 0, pSurfStarSource );
        g_pd3dDevice->SetTexture( 0, m_pTexBrightPass );
        g_pd3dDevice->SetScissorRect( &rectDest );
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
        //g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        //g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        //g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		GraphicsStruct::SamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);

        UINT uiPassCount, uiPass;       
        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

        hResult = S_OK;
Exit0:
        SAFE_RELEASE( pSurfStarSource);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::StarSource_To_BloomSource()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
        PDIRECT3DSURFACE9 pSurfBloomSource;
        RECT            rectSrc;
        RECT            rectDest;
        CoordRect       coords;
        D3DSURFACE_DESC desc;

        hRetCode = m_pTexBloomSource->GetSurfaceLevel( 0, &pSurfBloomSource );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetTextureRect(m_pTexStarSource, &rectSrc);
        InflateRect(&rectSrc, -1, -1);

        GetTextureRect(m_pTexBloomSource, &rectDest);
        InflateRect(&rectDest, -1, -1);

        GetTextureCoords(m_pTexStarSource, &rectSrc, m_pTexBloomSource, &rectDest, &coords );

        hRetCode = m_pTexBrightPass->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetSampleOffsets_DownScale2x2( desc.Width, desc.Height, avSampleOffsets );
        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

        hRetCode = m_pHDREffect->SetTechnique("DownScale2x2");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        g_pd3dDevice->SetRenderTarget( 0, pSurfBloomSource );
        g_pd3dDevice->SetTexture( 0, m_pTexStarSource );
        g_pd3dDevice->SetScissorRect( &rectDest );
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
        //g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        //g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        //g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		GraphicsStruct::SamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);

        UINT uiPassCount, uiPass;       
        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

        hResult = S_OK;
Exit0:
        SAFE_RELEASE( pSurfBloomSource);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::RenderBloom()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        UINT uiPassCount, uiPass;

        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
        FLOAT       afSampleOffsets[MAX_SAMPLES];
        D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];

        LPDIRECT3DSURFACE9 pSurfScaledHDR = NULL;
        LPDIRECT3DSURFACE9 pSurfBloom     = NULL;
        LPDIRECT3DSURFACE9 pSurfTempBloom = NULL;
        LPDIRECT3DSURFACE9 pSurfBloomSource = NULL;

        D3DSURFACE_DESC   desc;
        RECT              rectSrc;
        RECT              rectDest;
        CoordRect         coords;

        m_pTexSceneScaled->GetSurfaceLevel(0, &pSurfScaledHDR);
        m_apTexBloom[0]->GetSurfaceLevel(0, &pSurfBloom);
        m_apTexBloom[1]->GetSurfaceLevel(0, &pSurfTempBloom);
        m_apTexBloom[2]->GetSurfaceLevel(0, &pSurfBloomSource);

        g_pd3dDevice->ColorFill(pSurfBloom, NULL, D3DCOLOR_ARGB(0, 0, 0, 0));

        KG_PROCESS_ERROR(m_fGlareLuminance > 0.0f);
        KG_PROCESS_ERROR(m_fBloomLuminance > 0.0f);

        GetTextureRect(m_pTexBloomSource, &rectSrc);
        InflateRect(&rectSrc, -1, -1);

        GetTextureRect(m_apTexBloom[2], &rectDest);
        InflateRect(&rectDest, -1, -1);

        GetTextureCoords(m_pTexBloomSource, &rectSrc, m_apTexBloom[2], &rectDest, &coords);

        hRetCode = m_pTexBloomSource->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pHDREffect->SetTechnique("GaussBlur5x5");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        hRetCode = GetSampleOffsets_GaussBlur5x5(
            desc.Width, 
            desc.Height, 
            avSampleOffsets, 
            avSampleWeights, 
            1.0f 
            );

        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pHDREffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        g_pd3dDevice->SetRenderTarget(0, pSurfBloomSource );
        g_pd3dDevice->SetTexture( 0, m_pTexBloomSource );
        g_pd3dDevice->SetScissorRect( &rectDest );
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }
        m_pHDREffect->End();
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

        hRetCode = m_apTexBloom[2]->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetSampleOffsets_Bloom( 
            desc.Width, 
            afSampleOffsets, 
            avSampleWeights, 
            3.0f, 
            2.0f 
            );
        for(int i = 0; i < MAX_SAMPLES; i++ )
            avSampleOffsets[i] = D3DXVECTOR2(afSampleOffsets[i], 0.0f);
       
        hRetCode = m_pHDREffect->SetTechnique("Bloom");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pHDREffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        g_pd3dDevice->SetRenderTarget(0, pSurfTempBloom);
        g_pd3dDevice->SetTexture( 0, m_apTexBloom[2] );
        g_pd3dDevice->SetScissorRect( &rectDest );
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


        m_pHDREffect->Begin(&uiPassCount, 0);
        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }
        m_pHDREffect->End();
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );


        hRetCode = m_apTexBloom[1]->GetLevelDesc( 0, &desc );
        KG_COM_PROCESS_ERROR(hRetCode);

        GetSampleOffsets_Bloom(
            desc.Height, 
            afSampleOffsets, 
            avSampleWeights, 
            3.0f, 
            2.0f 
            );
        for (int i = 0; i < MAX_SAMPLES; i++ )
            avSampleOffsets[i] = D3DXVECTOR2( 0.0f, afSampleOffsets[i] );

        GetTextureRect(m_apTexBloom[1], &rectSrc);
        InflateRect(&rectSrc, -1, -1);

        GetTextureCoords(m_apTexBloom[1], &rectSrc, m_apTexBloom[0], NULL, &coords);

        hRetCode = m_pHDREffect->SetTechnique("Bloom");
        KGLOG_COM_CHECK_ERROR(hRetCode);

        m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pHDREffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

        g_pd3dDevice->SetRenderTarget(0, pSurfBloom);
        g_pd3dDevice->SetTexture(0, m_apTexBloom[1]);
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


        hRetCode = m_pHDREffect->Begin(&uiPassCount, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( coords );
            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();

        hResult = S_OK;

Exit0:
        SAFE_RELEASE( pSurfBloomSource );
        SAFE_RELEASE( pSurfTempBloom );
        SAFE_RELEASE( pSurfBloom );
        SAFE_RELEASE( pSurfScaledHDR );

        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::RenderStar()
    {
        //HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
		HRESULT hr = E_FAIL;

        UINT uiPassCount, uiPass;
        int i, d, p, s; // Loop variables
		const CStarDef& starDef = m_GlareDef.m_starDef ;
        const float fTanFoV = atanf(D3DX_PI/8) ;
        const D3DXVECTOR4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
        static const int s_maxPasses = 3 ;
        static const int nSamples = 8 ;
        static D3DXVECTOR4 s_aaColor[s_maxPasses][8] ;
        static const D3DXCOLOR s_colorWhite(0.63f, 0.63f, 0.63f, 0.0f) ;
		KG3DRenderState Rs;
		PDIRECT3DSURFACE9 apSurfStar[NUM_STAR_TEXTURES] = {0};
		PDIRECT3DSURFACE9 pSurfSource = NULL;
		PDIRECT3DSURFACE9 pSurfDest = NULL;

        LPDIRECT3DSURFACE9 pSurfStar = NULL;
        hRetCode = m_apTexStar[0]->GetSurfaceLevel( 0, &pSurfStar );
        KG_COM_PROCESS_ERROR(hRetCode);

        g_pd3dDevice->ColorFill( pSurfStar, NULL, D3DCOLOR_ARGB(0, 0, 0, 0) );

        KG_PROCESS_ERROR(m_fGlareLuminance > 0.0f);
        KG_PROCESS_ERROR(m_fStarLuminance  > 0.0f);

        D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];
        D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
	
        Rs.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE) ;
        Rs.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE) ;

        // Set aside all the star texture surfaces as a convenience
        for( i=0; i < NUM_STAR_TEXTURES; i++ )
        {
            hr = m_apTexStar[i]->GetSurfaceLevel( 0, &apSurfStar[i] );
			KG_COM_PROCESS_ERROR(hr);
        }

        // Get the source texture dimensions
        hr = m_pTexStarSource->GetSurfaceLevel( 0, &pSurfSource );
		KG_COM_PROCESS_ERROR(hr);

        D3DSURFACE_DESC desc;
        hr = pSurfSource->GetDesc( &desc );
		KG_COM_PROCESS_ERROR(hr);


        float srcW;
        srcW = (FLOAT) desc.Width;
        float srcH;
        srcH= (FLOAT) desc.Height;



        for (p = 0 ; p < s_maxPasses ; p ++)
        {
            float ratio;
            ratio = (float)(p + 1) / (float)s_maxPasses ;

            for (s = 0 ; s < nSamples ; s ++)
            {
                D3DXCOLOR chromaticAberrColor ;
                D3DXColorLerp(&chromaticAberrColor,
                    &( CStarDef::GetChromaticAberrationColor(s) ),
                    &s_colorWhite,
                    ratio) ;

                D3DXColorLerp( (D3DXCOLOR*)&( s_aaColor[p][s] ),
                    &s_colorWhite, &chromaticAberrColor,
                    m_GlareDef.m_fChromaticAberration ) ;
            }
        }

        float radOffset;
        radOffset = m_GlareDef.m_fStarInclination + starDef.m_fInclination ;


        PDIRECT3DTEXTURE9 pTexSource;


        // Direction loop
        for (d = 0 ; d < starDef.m_nStarLines ; d ++)
        {
            CONST STARLINE& starLine = starDef.m_pStarLine[d] ;

            pTexSource = m_pTexStarSource;

            float rad;
            rad = radOffset + starLine.fInclination ;
            float sn, cs;
            sn = sinf(rad), cs = cosf(rad) ;
            D3DXVECTOR2 vtStepUV;
            vtStepUV.x = sn / srcW * starLine.fSampleLength ;
            vtStepUV.y = cs / srcH * starLine.fSampleLength ;

            float attnPowScale;
            attnPowScale = (fTanFoV + 0.1f) * 1.0f *
                (160.0f + 120.0f) / (srcW + srcH) * 1.2f ;

            // 1 direction expansion loop
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;

            int iWorkTexture;
            iWorkTexture = 1 ;
            for (p = 0 ; p < starLine.nPasses ; p ++)
            {
				//代码逻辑暂时还是有保证肯定是有效访问的。
                if (p == starLine.nPasses - 1)
                {
                    // Last pass move to other work buffer
					ASSERT(d + 4 < NUM_STAR_TEXTURES);
                    pSurfDest = apSurfStar[d+4];
                }
                else {
					ASSERT(iWorkTexture < NUM_STAR_TEXTURES);
                    pSurfDest = apSurfStar[iWorkTexture];
                }

                // Sampling configration for each stage
                for (i = 0 ; i < nSamples ; i ++)
                {
                    float lum;
                    lum = powf( starLine.fAttenuation, attnPowScale * i );

                    avSampleWeights[i] = s_aaColor[starLine.nPasses - 1 - p][i] *
                        lum * (p+1.0f) * 0.5f ;


                    // Offset of sampling coordinate
                    avSampleOffsets[i].x = vtStepUV.x * i ;
                    avSampleOffsets[i].y = vtStepUV.y * i ;
                    if ( fabs(avSampleOffsets[i].x) >= 0.9f ||
                        fabs(avSampleOffsets[i].y) >= 0.9f )
                    {
                        avSampleOffsets[i].x = 0.0f ;
                        avSampleOffsets[i].y = 0.0f ;
                        avSampleWeights[i] *= 0.0f ;
                    }

                }


                m_pHDREffect->SetTechnique("Star");
                m_pHDREffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
                m_pHDREffect->SetVectorArray("g_avSampleWeights", avSampleWeights, nSamples);

                g_pd3dDevice->SetRenderTarget( 0, pSurfDest );
                g_pd3dDevice->SetTexture( 0, pTexSource );
                g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
                g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


                hr = m_pHDREffect->Begin(&uiPassCount, 0);
                KG_COM_PROCESS_ERROR(hr);

                for (uiPass = 0; uiPass < uiPassCount; uiPass++)
                {
                    m_pHDREffect->BeginPass(uiPass);

                    // Draw a fullscreen quad to sample the RT
                    DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

                    m_pHDREffect->EndPass();
                }

                m_pHDREffect->End();

                // Setup next expansion
                vtStepUV *= nSamples ;
                attnPowScale *= nSamples ;

                // Set the work drawn just before to next texture source.
                pTexSource = m_apTexStar[iWorkTexture];

                iWorkTexture += 1 ;
                if (iWorkTexture > 2) {
                    iWorkTexture = 1 ;
                }

            }
        }


        pSurfDest = apSurfStar[0];


        for( i=0; i < starDef.m_nStarLines; i++ )
        {
            g_pd3dDevice->SetTexture( i, m_apTexStar[i+4] );
            g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            g_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

            avSampleWeights[i] = vWhite * 1.0f / (FLOAT) starDef.m_nStarLines;
        }

        CHAR strTechnique[256];
        _snprintf( strTechnique, 256, "MergeTextures_%d", starDef.m_nStarLines );
        strTechnique[255] = 0;

        m_pHDREffect->SetTechnique(strTechnique);

        m_pHDREffect->SetVectorArray("g_avSampleWeights", avSampleWeights, starDef.m_nStarLines);

        g_pd3dDevice->SetRenderTarget( 0, pSurfDest );

        hr = m_pHDREffect->Begin(&uiPassCount, 0);
		KG_COM_PROCESS_ERROR(hr);

        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);

            // Draw a fullscreen quad to sample the RT
            DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

            m_pHDREffect->EndPass();
        }

        m_pHDREffect->End();


        hr = S_OK;
	Exit0:
        SAFE_RELEASE( pSurfSource );
        for( i=0; i < NUM_STAR_TEXTURES; i++ )
        {
            SAFE_RELEASE( apSurfStar[i] );
        }
        SAFE_RELEASE( pSurfStar );

        return hr;
        //return S_OK;
    }

    KG3DPostRenderEffectHDR::KG3DPostRenderEffectHDR(KG3DPostRenderEffectManager *pMgr)
        : KG3DPostRenderEffectBase(pMgr)
    {
        m_pTexSceneScaled           = NULL; 
        m_pHDREffect                = NULL;
        m_pTexAdaptedLuminanceCur   = NULL;
        m_pTexAdaptedLuminanceLast  = NULL;

        for(int i = 0; i < NUM_TONEMAP_TEXTURES; i++)
            m_apTexToneMap[i] = NULL;

        m_pTexBrightPass            = NULL; 
        m_pTexStarSource            = NULL;       
        m_pTexBloomSource           = NULL;

        for(int i = 0; i < NUM_BLOOM_TEXTURES; i++)
            m_apTexBloom[i] = NULL;

        for (int i = 0; i < NUM_STAR_TEXTURES; i++)
            m_apTexStar[i] = NULL;

        m_fGlareLuminance = 1.0f;
        m_fBloomLuminance = 2.0f;
        m_fStarLuminance  = 1.5f;
		UpdateStarType();
    }
    KG3DPostRenderEffectHDR::~KG3DPostRenderEffectHDR(void)
    {
        CleanUp();
    }

    void KG3DPostRenderEffectHDR::CleanUp()
    {
        UnInitTextures();
    }

	void  KG3DPostRenderEffectHDR::UpdateStarType()
	{
		m_eGlareType = *((EGLARELIBTYPE*)(&(PostRender::KG3DPostRenderEffectManager::m_sEffectParams.nHDRStarType)));
		m_GlareDef.Initialize(m_eGlareType);
	}
    HRESULT KG3DPostRenderEffectHDR::GetBackBufferDesc(D3DSURFACE_DESC& TexDesc)
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
		TexDesc.Width = dwWidth;
		TexDesc.Height = dwHeight;

        hResult = S_OK;
Exit0:
        KG_COM_RELEASE(pTexSufer);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::GetTextureCoords( 
        PDIRECT3DTEXTURE9 pTexSrc,  RECT* pRectSrc, 
        PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, 
        CoordRect* pCoords 
    )
    {
        HRESULT hr = S_OK;
        D3DSURFACE_DESC desc;
        float tU, tV;

        if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
            return E_INVALIDARG;

        pCoords->fLeftU   = 0.0f;
        pCoords->fTopV    = 0.0f;
        pCoords->fRightU  = 1.0f; 
        pCoords->fBottomV = 1.0f;

        if( pRectSrc != NULL )
        {
            hr = pTexSrc->GetLevelDesc( 0, &desc );
            if( FAILED(hr) )
                return hr;

            tU = 1.0f / desc.Width;
            tV = 1.0f / desc.Height;

            pCoords->fLeftU   += pRectSrc->left * tU;
            pCoords->fTopV    += pRectSrc->top * tV;
            pCoords->fRightU  -= (desc.Width - pRectSrc->right) * tU;
            pCoords->fBottomV -= (desc.Height - pRectSrc->bottom) * tV;
        }

        if( pRectDest != NULL )
        {
            hr = pTexDest->GetLevelDesc( 0, &desc );
            if( FAILED(hr) )
                return hr;

            tU = 1.0f / desc.Width;
            tV = 1.0f / desc.Height;

            pCoords->fLeftU   -= pRectDest->left * tU;
            pCoords->fTopV    -= pRectDest->top * tV;
            pCoords->fRightU  += (desc.Width - pRectDest->right) * tU;
            pCoords->fBottomV += (desc.Height - pRectDest->bottom) * tV;
        }

        return S_OK;
    }
    
    HRESULT KG3DPostRenderEffectHDR::GetTextureRect(PDIRECT3DTEXTURE9 pTexture, RECT* pRect)
    {
        HRESULT hr = S_OK;

        if( pTexture == NULL || pRect == NULL )
            return E_INVALIDARG;

        D3DSURFACE_DESC desc;
        hr = pTexture->GetLevelDesc( 0, &desc );
        if( FAILED(hr) )
            return hr;

        pRect->left = 0;
        pRect->top = 0;
        pRect->right = desc.Width;
        pRect->bottom = desc.Height;

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectHDR::Enable(BOOL bEnable)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
        BOOL bInitTextures = FALSE;
        
        if (bEnable)
        {
            D3DSURFACE_DESC DescBack;
            
            hRetCode = GetBackBufferDesc(DescBack);
            KG_COM_PROCESS_ERROR(hRetCode);
			
			DWORD dwHeight = 0,dwWidth = 0;
            dwWidth = DescBack.Width - DescBack.Width % 8;
            dwHeight = DescBack.Height - DescBack.Height % 8;

			if (m_bEnable)
			{
				if ((dwWidth  == m_dwCropWidth) &&
					(dwHeight == m_dwCropHeight))
				{
					return S_OK;
				}
			}
			
			CleanUp();

			m_dwCropWidth = dwWidth;
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
            }
        }
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::ApplyEffect(KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

       // LPDIRECT3DSURFACE9 pSurfLDR  = NULL;
        //LPDIRECT3DTEXTURE9 pTexScene = NULL;
		//UINT uiPassCount = 0;
		//UINT uiPass = 0;


		LPDIRECT3DSURFACE9 pTargetSave   = NULL;
		D3DVIEWPORT9       ViewPortSave;

		hRetCode = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
		hRetCode = g_pd3dDevice->GetViewport(&ViewPortSave);

        KG_PROCESS_ERROR(m_pPostRenderEffectMgr);
        KG_PROCESS_ERROR(m_bEnable);

        //pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
        //KG_PROCESS_ERROR(pTexScene);
      
        // Create a scaled copy of the scene
        hRetCode = Scene_To_SceneScaled();
        KG_COM_PROCESS_ERROR(hRetCode);
		
        // Setup tone mapping technique
        hRetCode = MeasureLuminance();
        KG_COM_PROCESS_ERROR(hRetCode);

        // Calculate the current luminance adaptation level
        hRetCode = CalculateAdaptation();
        KG_COM_PROCESS_ERROR(hRetCode);
		
		
        
        hRetCode = SceneScaled_To_BrightPass();
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = BrightPass_To_StarSource();
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = StarSource_To_BloomSource();
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = RenderBloom();
        KG_COM_PROCESS_ERROR(hRetCode);
		
        hRetCode = RenderStar();
        KG_COM_PROCESS_ERROR(hRetCode);
        

		/*
        m_pHDREffect->SetTechnique("FinalScenePass");
        m_pHDREffect->SetFloat("g_fMiddleGray", 0.5f);

        g_pd3dDevice->SetRenderTarget(0, pSurfLDR);
        g_pd3dDevice->SetTexture( 0, pTexScene );
        g_pd3dDevice->SetTexture( 1, m_apTexBloom[0] );
        g_pd3dDevice->SetTexture( 2, m_apTexStar[0] );
        g_pd3dDevice->SetTexture( 3, m_pTexAdaptedLuminanceCur );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        g_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        g_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pHDREffect->Begin(&uiPassCount, 0);
        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            m_pHDREffect->BeginPass(uiPass);
            DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );
            m_pHDREffect->EndPass();
        }
        m_pHDREffect->End();
		*/
		
        hResult = S_OK;
Exit0:
        //SAFE_RELEASE(pSurfLDR);

		hRetCode = g_pd3dDevice->SetRenderTarget(0, pTargetSave);
		hRetCode = g_pd3dDevice->SetViewport(&ViewPortSave);
		SAFE_RELEASE(pTargetSave);
		for (int i = 0; i < 8; ++i)
			g_pd3dDevice->SetTexture(i, NULL);

        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::ReloadFX()
    {
        HRESULT hResult = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        SAFE_RELEASE(m_pHDREffect);

        hRetCode = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, HDR_FX_FILE, NULL, NULL, 0, NULL, &m_pHDREffect, NULL);
        KG_COM_PROCESS_ERROR(hRetCode);

        hResult = S_OK;
Exit0:
        if (FAILED(hResult))
            SAFE_RELEASE(m_pHDREffect);

        return hResult;
    }

    HRESULT KG3DPostRenderEffectHDR::InitTextures()
    {
        HRESULT hr = S_OK;

        KGLOG_PROCESS_ERROR(m_dwCropWidth  / 4 > 0);
        KGLOG_PROCESS_ERROR(m_dwCropHeight / 4 > 0);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 4, m_dwCropHeight / 4, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pTexSceneScaled, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        for (int i = 0; i < _countof(m_apTexToneMap); ++i)
        {
            int iSampleLen = 1 << (2 * i);

            hr = g_pd3dDevice->CreateTexture(iSampleLen, iSampleLen, 1, 
                D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, m_apTexToneMap + i, NULL);
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hr = g_pd3dDevice->CreateTexture(1, 1, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pTexAdaptedLuminanceCur, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(1, 1, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pTexAdaptedLuminanceLast, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 4 + 2, m_dwCropHeight / 4 + 2, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexBrightPass, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 4 + 2, m_dwCropHeight / 4 + 2, 1,
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexStarSource, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 8 + 2, m_dwCropHeight / 8 + 2, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexBloomSource, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        for (int i = 1; i < _countof(m_apTexBloom); ++i)
        {
            hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 8 + 2, m_dwCropHeight / 8 + 2, 1, 
                D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_apTexBloom[i], NULL);
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 8, m_dwCropHeight / 8, 1,
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_apTexBloom[0], NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        for (int i = 0; i < _countof(m_apTexStar); ++i)
        {
            hr = g_pd3dDevice->CreateTexture(m_dwCropWidth / 4, m_dwCropHeight / 4, 1,
                D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_apTexStar[i], NULL);
            KG_COM_PROCESS_ERROR(hr);
        }

        ClearTexture(m_pTexAdaptedLuminanceCur);
        ClearTexture(m_pTexAdaptedLuminanceLast);
        ClearTexture(m_pTexBloomSource);
        ClearTexture(m_pTexBrightPass);
        ClearTexture(m_pTexStarSource);

        for (int i = 0; i < NUM_BLOOM_TEXTURES; ++i)
        {
            ClearTexture(m_apTexBloom[i]);
        }

Exit0:
        if (FAILED(hr))
        {
            UnInitTextures();
        }
        return hr;
    }

    HRESULT KG3DPostRenderEffectHDR::UnInitTextures()
    {
        HRESULT hr = S_OK;
		
		SAFE_RELEASE(m_pTexSceneScaled);
        SAFE_RELEASE(m_pHDREffect);
        SAFE_RELEASE(m_pTexAdaptedLuminanceCur);
        SAFE_RELEASE(m_pTexAdaptedLuminanceLast);
        
        SAFE_RELEASE(m_pTexBrightPass);
        SAFE_RELEASE(m_pTexStarSource);
        SAFE_RELEASE(m_pTexBloomSource);

        for (int i = 0; i < _countof(m_apTexToneMap); ++i)
            SAFE_RELEASE(m_apTexToneMap[i]);

        for(int i = 0; i < _countof(m_apTexBloom); ++i)
            SAFE_RELEASE(m_apTexBloom[i]);

        for (int i = 0; i < _countof(m_apTexStar); i++)
            SAFE_RELEASE(m_apTexStar[i]);

        return hr;
    }

    HRESULT KG3DPostRenderEffectHDR::OnLostDevice()
    {
        HRESULT hr = S_OK;

        if (m_pHDREffect)
        {
            hr = m_pHDREffect->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hr = UnInitTextures();
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return hr;
    }

    HRESULT KG3DPostRenderEffectHDR::OnResetDevice()
    {
        HRESULT hr = S_OK;
        BOOL bInitTextures = FALSE;

        if (m_bEnable)
        {
            hr = UnInitTextures();
			hr = InitTextures();
            KGLOG_COM_PROCESS_ERROR(hr);

            bInitTextures = TRUE;
			hr = ReloadFX();
			KG_COM_PROCESS_ERROR(hr);
        }

        if (m_pHDREffect)
        {
            hr = m_pHDREffect->OnResetDevice();
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
	int KG3DPostRenderEffectHDR::GetHDRStarTypeCount()
	{
		return PostRender::NUM_GLARELIBTYPES;
	}
	int KG3DPostRenderEffectHDR::GetHDRStarName(int index, char szName[])
	{
		switch (index)
		{
		case GLT_DISABLE:
			strcpy(szName,"Disable");
			break;
		case GLT_CAMERA:
			strcpy(szName,"Camera");
			break;
		case GLT_NATURAL:
			strcpy(szName,"Natural");
			break;
		case GLT_CHEAPLENS:
			strcpy(szName,"Cheap Lens");
			break;
		case GLT_FILTER_CROSSSCREEN:
			strcpy(szName,"Cross Screen");
			break;
		case GLT_FILTER_CROSSSCREEN_SPECTRAL:
			strcpy(szName,"Cross Screen Spectral");
			break;
		case GLT_FILTER_SNOWCROSS:
			strcpy(szName,"Snow Cross");
			break;
		case GLT_FILTER_SNOWCROSS_SPECTRAL:
			strcpy(szName,"Snow Cross Spectral");
			break;
		case GLT_FILTER_SUNNYCROSS:
			strcpy(szName,"Sunny Cross");
			break;
		case GLT_FILTER_SUNNYCROSS_SPECTRAL:
			strcpy(szName,"Sunny Cross Spectral");
			break;
		case GLT_CINECAM_VERTICALSLITS:
			strcpy(szName,"Vertical Slits");
			break;
		case GLT_CINECAM_HORIZONTALSLITS:
			strcpy(szName,"Horizontal Slits");
			break;
		default:
			break;
		}
		return true;
	}


	#define _Rad    D3DXToRadian

	// Static star library information
	static STARDEF s_aLibStarDef[NUM_BASESTARLIBTYPES] =
	{
		//  star name               lines   passes  length  attn    rotate          bRotate
		{   TEXT("Disable"),        0,      0,      0.0f,   0.0f,   _Rad(00.0f),    false,  },  // STLT_DISABLE

		{   TEXT("Cross"),          4,      3,      1.0f,   0.85f,  _Rad(0.0f),     true,   },  // STLT_CROSS
		{   TEXT("CrossFilter"),    4,      3,      1.0f,   0.95f,  _Rad(0.0f),     true,   },  // STLT_CROSS
		{   TEXT("snowCross"),      6,      3,      1.0f,   0.96f,  _Rad(20.0f),    true,   },  // STLT_SNOWCROSS
		{   TEXT("Vertical"),       2,      3,      1.0f,   0.96f,  _Rad(00.0f),    false,  },  // STLT_VERTICAL
	} ;
	static int s_nLibStarDefs = sizeof(s_aLibStarDef) / sizeof(STARDEF) ;


	// Static glare library information
	static GLAREDEF s_aLibGlareDef[NUM_GLARELIBTYPES] =
	{
		//  glare name                              glare   bloom   ghost   distort star    star type
		//  rotate          C.A     current after   ai lum
		{   TEXT("Disable"),                        0.0f,   0.0f,   0.0f,   0.01f,  0.0f,   STLT_DISABLE,
			_Rad(0.0f),     0.5f,   0.00f,  0.00f,  0.0f,  },   // GLT_DISABLE

		{   TEXT("Camera"),                         1.5f,   1.2f,   1.0f,   0.00f,  1.0f,   STLT_CROSS,
			_Rad(00.0f),    0.5f,   0.25f,  0.90f,  1.0f,  },   // GLT_CAMERA
		{   TEXT("Natural Bloom"),                  1.5f,   1.2f,   0.0f,   0.00f,  0.0f,   STLT_DISABLE,
			_Rad(00.0f),    0.0f,   0.40f,  0.85f,  0.5f,  },   // GLT_NATURAL
		{   TEXT("Cheap Lens Camera"),              1.25f,  2.0f,   1.5f,   0.05f,  2.0f,   STLT_CROSS,
			_Rad(00.0f),    0.5f,   0.18f,  0.95f,  1.0f,  },   // GLT_CHEAPLENS
	/*  
		{   TEXT("Afterimage"),                     1.5f,   1.2f,   0.5f,   0.00f,  0.7f,   STLT_CROSS,
			_Rad(00.0f),    0.5f,   0.1f,   0.98f,  2.0f,  },   // GLT_AFTERIMAGE
	*/
		{   TEXT("Cross Screen Filter"),            1.0f,   2.0f,   1.7f,   0.00f,  1.5f,   STLT_CROSSFILTER,
			_Rad(25.0f),    0.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_FILTER_CROSSSCREEN
		{   TEXT("Spectral Cross Filter"),          1.0f,   2.0f,   1.7f,   0.00f,  1.8f,   STLT_CROSSFILTER,
			_Rad(70.0f),    1.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_FILTER_CROSSSCREEN_SPECTRAL
		{   TEXT("Snow Cross Filter"),              1.0f,   2.0f,   1.7f,   0.00f,  1.5f,   STLT_SNOWCROSS,
			_Rad(10.0f),    0.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_FILTER_SNOWCROSS
		{   TEXT("Spectral Snow Cross"),            1.0f,   2.0f,   1.7f,   0.00f,  1.8f,   STLT_SNOWCROSS,
			_Rad(40.0f),    1.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_FILTER_SNOWCROSS_SPECTRAL
		{   TEXT("Sunny Cross Filter"),             1.0f,   2.0f,   1.7f,   0.00f,  1.5f,   STLT_SUNNYCROSS,
			_Rad(00.0f),    0.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_FILTER_SUNNYCROSS
		{   TEXT("Spectral Sunny Cross"),           1.0f,   2.0f,   1.7f,   0.00f,  1.8f,   STLT_SUNNYCROSS,
			_Rad(45.0f),    1.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_FILTER_SUNNYCROSS_SPECTRAL
		{   TEXT("Cine Camera Vertical Slits"),     1.0f,   2.0f,   1.5f,   0.00f,  1.0f,   STLT_VERTICAL,
			_Rad(90.0f),    0.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_CINECAM_VERTICALSLIT
		{   TEXT("Cine Camera Horizontal Slits"),   1.0f,   2.0f,   1.5f,   0.00f,  1.0f,   STLT_VERTICAL,
			_Rad(00.0f),    0.5f,   0.20f,  0.93f,  1.0f,  },   // GLT_CINECAM_HORIZONTALSLIT
	} ;
	static int s_nLibGlareDefs = sizeof(s_aLibGlareDef) / sizeof(GLAREDEF) ;



	//----------------------------------------------------------
	// Information object for star generation 

	CStarDef    *CStarDef::ms_pStarLib  = NULL ;
	D3DXCOLOR   CStarDef::ms_avChromaticAberrationColor[] ;


	CStarDef::CStarDef()
	{
		Construct() ;
	}

	CStarDef::CStarDef(const CStarDef& src)
	{
		Construct() ;
		Initialize(src) ;
	}

	CStarDef::~CStarDef()
	{
		Destruct() ;
	}


	HRESULT CStarDef::Construct()
	{
		ZeroMemory( m_strStarName, sizeof(m_strStarName) );

		m_nStarLines    = 0 ;
		m_pStarLine     = NULL ;
		m_fInclination  = 0.0f ;

		m_bRotation     = false ;

		return S_OK ;
	}

	void CStarDef::Destruct()
	{
		Release() ;
	}

	void CStarDef::Release()
	{
		SAFE_DELETE_ARRAY(m_pStarLine) ;
		m_nStarLines = 0 ;
	}


	HRESULT CStarDef::Initialize(const CStarDef& src)
	{
		if (&src == this) {
			return S_OK ;
		}

		// Release the data
		Release() ;

		// Copy the data from source
		StringCchCopy( m_strStarName, 256, src.m_strStarName );
		m_nStarLines    = src.m_nStarLines ;
		m_fInclination  = src.m_fInclination ;
		m_bRotation     = src.m_bRotation ;

		m_pStarLine = new STARLINE[m_nStarLines];
		if( m_pStarLine == NULL )
			return E_OUTOFMEMORY;
	        
		for (int i = 0 ; i < m_nStarLines ; i ++) {
			m_pStarLine[i] = src.m_pStarLine[i] ;
		}

		return S_OK ;
	}


	/// generic simple star generation
	HRESULT CStarDef::Initialize(const TCHAR *szStarName,
								 int nStarLines,
								 int nPasses,
								 float fSampleLength,
								 float fAttenuation,
								 float fInclination,
								 bool bRotation)
	{
		// Release the data
		Release() ;

		// Copy from parameters
		StringCchCopy( m_strStarName, 256, szStarName );
		m_nStarLines    = nStarLines ;
		m_fInclination  = fInclination ;
		m_bRotation     = bRotation ;

		m_pStarLine = new STARLINE[m_nStarLines] ;
		if( m_pStarLine == NULL )
			return E_OUTOFMEMORY;

		float fInc = D3DXToRadian(360.0f / (float)m_nStarLines) ;
		for (int i = 0 ; i < m_nStarLines ; i ++)
		{
			m_pStarLine[i].nPasses          = nPasses ;
			m_pStarLine[i].fSampleLength    = fSampleLength ;
			m_pStarLine[i].fAttenuation     = fAttenuation ;
			m_pStarLine[i].fInclination     = fInc * (float)i ;
		}

		return S_OK ;
	}


	/// Specific start generation
	//  Sunny cross filter
	HRESULT CStarDef::Initialize_SunnyCrossFilter(const TCHAR *szStarName,
												  float fSampleLength,
												  float fAttenuation,
												  float fLongAttenuation,
												  float fInclination)
	{
		// Release the data
		Release() ;

		// Create parameters
		StringCchCopy( m_strStarName, 256, szStarName );
		m_nStarLines    = 8 ;
		m_fInclination  = fInclination ;
	//  m_bRotation     = true ;
		m_bRotation     = false ;

		m_pStarLine = new STARLINE[m_nStarLines] ;
		if( m_pStarLine == NULL )
			return E_OUTOFMEMORY;
	        
		float fInc = D3DXToRadian(360.0f / (float)m_nStarLines) ;
		for (int i = 0 ; i < m_nStarLines ; i ++)
		{
			m_pStarLine[i].fSampleLength    = fSampleLength ;
			m_pStarLine[i].fInclination     = fInc * (float)i + D3DXToRadian(0.0f) ;

			if ( 0 == (i % 2) ) {
				m_pStarLine[i].nPasses      = 3 ;
				m_pStarLine[i].fAttenuation = fLongAttenuation ;    // long
			}
			else {
				m_pStarLine[i].nPasses      = 3 ;
				m_pStarLine[i].fAttenuation = fAttenuation ;
			}
		}

		return S_OK ;
	}

	HRESULT CStarDef::InitializeStaticStarLibs()
	{
		if (ms_pStarLib) {
			return S_OK ;
		}

		ms_pStarLib = new CStarDef[NUM_STARLIBTYPES] ;
		if( ms_pStarLib == NULL )
			return E_OUTOFMEMORY;

		// Create basic form
		for (int i = 0 ; i < NUM_BASESTARLIBTYPES ; i ++) {
			ms_pStarLib[i].Initialize(s_aLibStarDef[i]) ;
		}

		// Create special form
		// Sunny cross filter
		ms_pStarLib[STLT_SUNNYCROSS].Initialize_SunnyCrossFilter() ;

		// Initialize color aberration table
	/*
		{
			D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f),
			D3DXCOLOR(1.0f, 0.2f, 0.2f,  0.0f),
			D3DXCOLOR(0.2f, 0.6f, 0.2f,  0.0f),
			D3DXCOLOR(0.2f, 0.2f, 1.0f,  0.0f),
		} ;
	*/
		D3DXCOLOR avColor[8] =
		{
	/*
			D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f),
			D3DXCOLOR(0.3f, 0.3f, 0.8f,  0.0f),
			D3DXCOLOR(0.2f, 0.2f, 1.0f,  0.0f),
			D3DXCOLOR(0.2f, 0.4f, 0.5f,  0.0f),
			D3DXCOLOR(0.2f, 0.6f, 0.2f,  0.0f),
			D3DXCOLOR(0.5f, 0.4f, 0.2f,  0.0f),
			D3DXCOLOR(0.7f, 0.3f, 0.2f,  0.0f),
			D3DXCOLOR(1.0f, 0.2f, 0.2f,  0.0f),
	*/

			D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f), // w
			D3DXCOLOR(0.8f, 0.3f, 0.3f,  0.0f),
			D3DXCOLOR(1.0f, 0.2f, 0.2f,  0.0f), // r
			D3DXCOLOR(0.5f, 0.2f, 0.6f,  0.0f),
			D3DXCOLOR(0.2f, 0.2f, 1.0f,  0.0f), // b
			D3DXCOLOR(0.2f, 0.3f, 0.7f,  0.0f),
			D3DXCOLOR(0.2f, 0.6f, 0.2f,  0.0f), // g
			D3DXCOLOR(0.3f, 0.5f, 0.3f,  0.0f),
		} ;

		memcpy( ms_avChromaticAberrationColor, avColor, sizeof(D3DXCOLOR) * 8 ) ;
	/*
		ms_avChromaticAberrationColor[0] = D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f) ;
		ms_avChromaticAberrationColor[1] = D3DXCOLOR(0.7f, 0.3f, 0.3f,  0.0f) ;
		ms_avChromaticAberrationColor[2] = D3DXCOLOR(1.0f, 0.2f, 0.2f,  0.0f) ;
		ms_avChromaticAberrationColor[3] = D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f) ;
		ms_avChromaticAberrationColor[4] = D3DXCOLOR(0.2f, 0.6f, 0.2f,  0.0f) ;
		ms_avChromaticAberrationColor[5] = D3DXCOLOR(0.2f, 0.4f, 0.5f,  0.0f) ;
		ms_avChromaticAberrationColor[6] = D3DXCOLOR(0.2f, 0.3f, 0.8f,  0.0f) ;
		ms_avChromaticAberrationColor[7] = D3DXCOLOR(0.2f, 0.2f, 1.0f,  0.0f) ;
	*/
		return S_OK ;
	}

	HRESULT CStarDef::DeleteStaticStarLibs()
	{
		// Delete all libraries
		SAFE_DELETE_ARRAY( ms_pStarLib ) ;

		return S_OK ;
	}



	//----------------------------------------------------------
	// Glare definition

	CGlareDef   *CGlareDef::ms_pGlareLib    = NULL ;

	CGlareDef::CGlareDef()
	{
		Construct() ;
	}

	CGlareDef::CGlareDef(const CGlareDef& src)
	{
		Construct() ;
		Initialize(src) ;
	}

	CGlareDef::~CGlareDef()
	{
		Destruct() ;
	}


	HRESULT CGlareDef::Construct()
	{
		ZeroMemory( m_strGlareName, sizeof(m_strGlareName) );

		m_fGlareLuminance   = 0.0f ;
		m_fBloomLuminance   = 0.0f ;
		m_fGhostLuminance   = 0.0f ;
		m_fStarLuminance    = 0.0f ;
		m_fStarInclination  = 0.0f ;
		m_fChromaticAberration  = 0.0f ;

		m_fAfterimageSensitivity    = 0.0f ;
		m_fAfterimageRatio          = 0.0f ;
		m_fAfterimageLuminance      = 0.0f ;

		return S_OK ;
	}

	void CGlareDef::Destruct()
	{
		m_starDef.Release() ;
	}

	void CGlareDef::Release()
	{
	}

	HRESULT CGlareDef::Initialize(const CGlareDef& src)
	{
		if (&src == this) {
			return S_OK ;
		}

		// Release the data
		Release() ;

		// Copy data from source
		StringCchCopy( m_strGlareName, 256, src.m_strGlareName );
		m_fGlareLuminance   = src.m_fGlareLuminance ;
		m_fBloomLuminance   = src.m_fBloomLuminance ;
		m_fGhostLuminance   = src.m_fGhostLuminance ;
		m_fGhostDistortion  = src.m_fGhostDistortion ;
		m_fStarLuminance    = src.m_fStarLuminance ;
		m_fStarInclination  = src.m_fStarInclination ;
		m_fChromaticAberration  = src.m_fChromaticAberration ;

		m_fAfterimageSensitivity    = src.m_fAfterimageLuminance ;
		m_fAfterimageRatio          = src.m_fAfterimageRatio;
		m_fAfterimageLuminance      = src.m_fAfterimageLuminance ;

		m_starDef           = src.m_starDef ;

		return S_OK ;
	}


	HRESULT CGlareDef::Initialize(const TCHAR *szStarName,
								  float fGlareLuminance,
								  float fBloomLuminance,
								  float fGhostLuminance,
								  float fGhostDistortion,
								  float fStarLuminance,
								  ESTARLIBTYPE eStarType,
								  float fStarInclination,
								  float fChromaticAberration,
								  float fAfterimageSensitivity, // Current weight
								  float fAfterimageRatio,       // Afterimage weight
								  float fAfterimageLuminance)
	{
		// Release the data
		Release() ;

		// Create parameters
		StringCchCopy( m_strGlareName, 256, szStarName );
		m_fGlareLuminance   = fGlareLuminance ;

		m_fBloomLuminance   = fBloomLuminance ;
		m_fGhostLuminance   = fGhostLuminance ;
		m_fGhostDistortion  = fGhostDistortion ;
		m_fStarLuminance    = fStarLuminance ;
		m_fStarInclination  = fStarInclination ;
		m_fChromaticAberration  = fChromaticAberration ;

		m_fAfterimageSensitivity    = fAfterimageSensitivity ;
		m_fAfterimageRatio          = fAfterimageRatio ;
		m_fAfterimageLuminance      = fAfterimageLuminance ;

		// Create star form data
		m_starDef           = CStarDef::GetLib(eStarType) ;

		return S_OK ;
	}


	HRESULT CGlareDef::InitializeStaticGlareLibs()
	{
		if (ms_pGlareLib) {
			return S_OK ;
		}

		CStarDef::InitializeStaticStarLibs() ;
		ms_pGlareLib = new CGlareDef[NUM_GLARELIBTYPES] ;
		if( ms_pGlareLib == NULL )
			return E_OUTOFMEMORY;

		// Create glare form
		for (int i = 0 ; i < NUM_GLARELIBTYPES ; i ++) {
			ms_pGlareLib[i].Initialize(s_aLibGlareDef[i]) ;
		}

		return S_OK ;
	}


	HRESULT CGlareDef::DeleteStaticGlareLibs()
	{
		// Delete all libraries
		SAFE_DELETE_ARRAY( ms_pGlareLib ) ;

		return S_OK ;
	}


	//----------------------------------------------------------

	// Dummy to generate static object for glare
	__CGlare_GenerateStaticObjects __CGlare_GenerateStaticObjects::ms_staticObject ;

}
