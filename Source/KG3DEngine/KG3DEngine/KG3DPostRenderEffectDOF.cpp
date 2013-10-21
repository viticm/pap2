#include "StdAfx.h"
#include "KG3DPostRenderEffectDOF.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRenderState.h"
#include "KG3DSceneSceneEditor.h"

extern int g_nDepthTexturePackingType;

namespace PostRender {
	KG3DPostRenderEffectDOF::KG3DPostRenderEffectDOF(KG3DPostRenderEffectManager *pMgr)
		        : KG3DPostRenderEffectBase(pMgr)
	{
		m_pEffect = NULL;
		m_pTexDOFDepth = NULL;
		m_pTexSceneScaled = NULL;
		m_pBlurTexture = NULL;
	}

	KG3DPostRenderEffectDOF::~KG3DPostRenderEffectDOF(void)
	{
		CleanUp();
	}
	HRESULT KG3DPostRenderEffectDOF::Enable(BOOL bEnable)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;
		BOOL bInitTextures = FALSE;

		DWORD dwWidthSave = m_dwWidth;
		DWORD dwHeightSave = m_dwHeight;

		if (bEnable)
		{
			//CleanUp();

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
				if ((dwWidth  == m_dwWidth) &&
					(dwHeight == m_dwHeight))
				{
					return S_OK;
				}
			}
			CleanUp();
			m_dwHeight = dwHeight;
			m_dwWidth = dwWidth;

			m_dwCropHeight = dwHeight - dwHeight % 8;
			m_dwCropWidth = dwWidth - dwWidth % 8;

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
				m_dwWidth = dwWidthSave;
				m_dwHeight = dwHeightSave; 
			}
		}
		return hResult;
	}
	HRESULT KG3DPostRenderEffectDOF::ApplyEffect(KG3DScene* pScene)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		LPDIRECT3DSURFACE9 pTargetSave   = NULL;
		D3DVIEWPORT9       ViewPortSave;

		hRetCode = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
		hRetCode = g_pd3dDevice->GetViewport(&ViewPortSave);

		KG_PROCESS_ERROR(m_pPostRenderEffectMgr);
		KG_PROCESS_ERROR(m_bEnable);

		hRetCode = ComputeDOFDepth();
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = Scene_To_SceneScaled();
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = ScaledGaussianBlur();
		KG_COM_PROCESS_ERROR(hRetCode);

		hResult = S_OK;
Exit0:
		hRetCode = g_pd3dDevice->SetRenderTarget(0, pTargetSave);
		hRetCode = g_pd3dDevice->SetViewport(&ViewPortSave);
		SAFE_RELEASE(pTargetSave);
		for (int i = 0; i < 8; ++i)
			g_pd3dDevice->SetTexture(i, NULL);
		return hResult;
	}

	HRESULT KG3DPostRenderEffectDOF::ReloadFX()
	{
		HRESULT hResult = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		SAFE_RELEASE(m_pEffect);

		LPCTSTR strPackType = NULL;
		switch (g_nDepthTexturePackingType)
		{
		case 0:
			strPackType = _T("0");
			break;
		case 1:
			strPackType = _T("1");
			break;
		default:
			strPackType = _T("0");
			break;
		}
		D3DXMACRO macroPackType[] = {
			{_T("PACKTYPE"), strPackType},	
			{NULL, NULL},
		};

		hRetCode = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, DOF_FX_FILE, macroPackType, NULL, 0, NULL, &m_pEffect, NULL);
		KG_COM_PROCESS_ERROR(hRetCode);
		hResult = S_OK;
Exit0:
		if (FAILED(hResult))
			SAFE_RELEASE(m_pEffect);

		return hResult;
	}

	HRESULT KG3DPostRenderEffectDOF::OnLostDevice()
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
	HRESULT KG3DPostRenderEffectDOF::OnResetDevice()
	{
		HRESULT hr = S_OK;
		BOOL bInitTextures = FALSE;

		if (m_bEnable)
		{
			hr = UnInitTextures();
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

	void    KG3DPostRenderEffectDOF::CleanUp()
	{
		 UnInitTextures();
		 SAFE_RELEASE(m_pEffect);
	}

	HRESULT KG3DPostRenderEffectDOF::GetBackBufferDesc(D3DSURFACE_DESC& TexDesc)
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


	HRESULT KG3DPostRenderEffectDOF::InitTextures()
	{
		DWORD dwHeight = 0,dwWidth = 0;
		HRESULT hr = S_OK;
		ASSERT(m_pTexDOFDepth == NULL);
		ASSERT(m_pTexSceneScaled == NULL);
		ASSERT(m_pBlurTexture == NULL);
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
		m_dwHeight = dwHeight;
		m_dwWidth = dwWidth;
		m_dwCropHeight = dwHeight - dwHeight % 8;
		m_dwCropWidth = dwWidth - dwWidth % 8;
		//D3DSURFACE_DESC TexDesc;
		//GetBackBufferDesc(TexDesc);


		hr = g_pd3dDevice->CreateTexture(m_dwWidth, m_dwHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexDOFDepth, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		KGLOG_PROCESS_ERROR(m_dwCropWidth  / 4 > 0);
		KGLOG_PROCESS_ERROR(m_dwCropHeight / 4 > 0);

		hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 4, m_dwCropHeight / 4, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexSceneScaled, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->CreateTexture(m_dwCropWidth  / 4, m_dwCropHeight / 4, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pBlurTexture, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
		if (FAILED(hr))
		{
			UnInitTextures();
		}
		return hr;
	}

	HRESULT KG3DPostRenderEffectDOF::UnInitTextures()
	{
		SAFE_RELEASE(m_pTexDOFDepth);
		SAFE_RELEASE(m_pTexSceneScaled);
		SAFE_RELEASE(m_pBlurTexture);
		m_dwHeight = 0;
		m_dwWidth  = 0;
		m_dwCropHeight = 0;
		m_dwCropWidth = 0;
		return S_OK;
	}

	HRESULT KG3DPostRenderEffectDOF::ComputeDOFDepth()
	{
		HRESULT hRetCode = S_OK;
		float fOffsetU = 1.0,fOffsetV = 1.0;
		float fScalU = 1.0,fScalV = 1.0;
		KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

		if (!pCamera)
			return E_FAIL;
		D3DXMATRIX matProj,matProjInv;
		matProj = pCamera->GetProjectionMatrix();
		D3DXMatrixInverse(&matProjInv,NULL,&matProj);
		m_pEffect->SetMatrix("matCameraPInv",&matProjInv);

		m_pPostRenderEffectMgr->GetDepthTexCoordScale(&fScalU, &fScalV);
		fOffsetU = 0.5f / m_dwWidth;
		fOffsetV = 0.5f / m_dwHeight;
		m_pEffect->SetValue("OffsetU",&fOffsetU,sizeof(float));
		m_pEffect->SetValue("OffsetV",&fOffsetV,sizeof(float));
		m_pEffect->SetValue("ScalU",&fScalU,sizeof(float));
		m_pEffect->SetValue("ScalV",&fScalV,sizeof(float));

		D3DXVECTOR3 vecCameraPos,vecLookAtPos,vecLen;
		pCamera->GetPosition(&vecCameraPos);
		if (g_bClient)
		{
			KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
			if (pScene)
			{
				pScene->GetFocus(&vecLookAtPos);
			}
			else
			{
				pCamera->GetLookAtPosition(&vecLookAtPos);
			}
		}
		else
		{
			pCamera->GetLookAtPosition(&vecLookAtPos);
		}

		vecLen = vecLookAtPos - vecCameraPos;
		float fDis = D3DXVec3Length(&vecLen);
		
		float fCameraNear = 0,fCameraFar = 0,fFovy = 0,fAspect = 0;
		pCamera->GetPerspective(&fFovy,&fAspect,&fCameraNear,&fCameraFar);
		D3DXVECTOR4 vDofParams;
		vDofParams.x = fCameraNear;
		vDofParams.y = fCameraNear + (fDis > 150 ? fDis - 150 : 0);
		vDofParams.z = fCameraFar;
		vDofParams.w = 1.0f;
		m_pEffect->SetVector("vDofParams",&vDofParams);
		m_pEffect->SetTexture("tMainDepth", m_pPostRenderEffectMgr->GetDepthTexture());
		LPDIRECT3DSURFACE9 pSurface = NULL;
		m_pTexDOFDepth->GetSurfaceLevel(0,&pSurface);
		hRetCode = g_pd3dDevice->SetRenderTarget(0, pSurface);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		

		if (SUCCEEDED(m_pEffect->SetTechnique("DOF")))
		{
			UINT uPass = 0;
			m_pEffect->Begin(&uPass,0);
			m_pEffect->BeginPass(0);			
			g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwWidth * 1.0F,m_dwHeight * 1.0F),0,0xFFFFFFFF);
			m_pEffect->EndPass();
			m_pEffect->End();
		}
		m_pEffect->SetTexture("tMainDepth", NULL);
Exit0:
		SAFE_RELEASE(pSurface);
		return S_OK;
	}
	HRESULT KG3DPostRenderEffectDOF::Scene_To_SceneScaled()
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
		D3DXVECTOR2 avSampleWeight[MAX_SAMPLES];

		PDIRECT3DSURFACE9  pSurfScaledScene = NULL;
		LPDIRECT3DTEXTURE9 pTexScene = NULL;
		D3DSURFACE_DESC    BackBufferDesc;
		CoordRect coords = CoordRect();
		CoordRect coords2 = CoordRect();

		pTexScene = m_pPostRenderEffectMgr->GetRenderTexture();
		KG_PROCESS_ERROR(pTexScene);

		// get the new render target surface
		hRetCode = m_pTexSceneScaled->GetSurfaceLevel(0, &pSurfScaledScene);
		KG_COM_PROCESS_ERROR(hRetCode);

		GetBackBufferDesc(BackBufferDesc);

		hRetCode = m_pEffect->SetTechnique("DownScale4x4");
		KGLOG_COM_CHECK_ERROR(hRetCode);

		// place the rectangle in the center of the back buffer surface
		RECT rectSrc;
		rectSrc.left   = (BackBufferDesc.Width  - m_dwCropWidth)  / 2;
		rectSrc.top    = (BackBufferDesc.Height - m_dwCropHeight) / 2;
		rectSrc.right  = rectSrc.left + m_dwCropWidth;
		rectSrc.bottom = rectSrc.top  + m_dwCropHeight;

		// get the texture coordinates for the render target
		GetTextureCoords(pTexScene, &rectSrc, m_pTexSceneScaled, NULL, &coords);
		GetTextureCoords(m_pTexDOFDepth, &rectSrc, m_pTexSceneScaled, NULL, &coords2);

		// get the sample offsets used within the pixel shader
		KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( m_pPostRenderEffectMgr->GetWidth(), m_pPostRenderEffectMgr->GetHeight(), avSampleOffsets );
		m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

		KG3DPostRenderEffectHDR::GetSampleOffsets_DownScale4x4( BackBufferDesc.Width, BackBufferDesc.Height, avSampleWeight );
		m_pEffect->SetValue("g_avSampleWeights", avSampleWeight, sizeof(avSampleWeight));

		g_pd3dDevice->SetRenderTarget( 0, pSurfScaledScene);
		m_pEffect->SetTexture( "tMainColor", pTexScene );
		m_pEffect->SetTexture( "tDOFDepth",  m_pTexDOFDepth);
		GraphicsStruct::SamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		GraphicsStruct::SamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
		GraphicsStruct::SamplerState(1,D3DSAMP_MINFILTER,D3DTEXF_POINT);
		GraphicsStruct::SamplerState(1,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		GraphicsStruct::SamplerState(1,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
		UINT uiPassCount, uiPass;       
		hRetCode = m_pEffect->Begin(&uiPassCount, 0);
		KG_COM_PROCESS_ERROR(hRetCode);

		for (uiPass = 0; uiPass < uiPassCount; uiPass++)
		{
			m_pEffect->BeginPass(uiPass);
			DrawFullScreenQuad2TexCoord( coords ,coords2);
			m_pEffect->EndPass();
		}
		m_pEffect->End();
		m_pEffect->SetTexture("tMainColor",NULL);
		m_pEffect->SetTexture("tDOFDepth",NULL);
		hResult = S_OK;
Exit0:
		SAFE_RELEASE(pSurfScaledScene); 
		return hResult;
	}

	HRESULT KG3DPostRenderEffectDOF::ScaledGaussianBlur()
	{
		HRESULT hRetCode  = E_FAIL;
		D3DXVECTOR4 vec4PixSize;

		LPDIRECT3DSURFACE9 pBlurSurface = NULL;
		m_pBlurTexture->GetSurfaceLevel(0,&pBlurSurface);
		hRetCode = g_pd3dDevice->SetRenderTarget(0, pBlurSurface);
		hRetCode = m_pEffect->SetTexture("tSceneScale", m_pTexSceneScaled);
		KG_COM_PROCESS_ERROR(hRetCode);

		vec4PixSize.x = 1.0f / (m_dwCropWidth  / 4);
		vec4PixSize.y = 1.0f / (m_dwCropHeight / 4);
		m_pEffect->SetVector("pixSize",&vec4PixSize);
		{
			hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
			KG_COM_PROCESS_ERROR(hRetCode);

			if (SUCCEEDED(m_pEffect->SetTechnique("Gussblur")))
			{
				UINT uPass = 0;
				m_pEffect->Begin(&uPass,0);
				m_pEffect->BeginPass(0);
				g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwCropWidth  / 4.0f ,m_dwCropHeight  / 4.0f),0,0xFFFFFFFF);

				m_pEffect->EndPass();
				m_pEffect->End();
			}

		}
		m_pEffect->SetTexture("tSceneScale", NULL);
Exit0:
		SAFE_RELEASE(pBlurSurface);
		m_pEffect->SetTexture("tSceneScale", NULL);
		return S_OK;
	}

	HRESULT KG3DPostRenderEffectDOF::GetTextureCoords( 
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
	HRESULT KG3DPostRenderEffectDOF::DrawFullScreenQuad2TexCoord(CoordRect c,CoordRect c2)
	{ 
		return DrawFullScreenQuad2TexCoord( c.fLeftU, c.fTopV, c.fRightU, c.fBottomV , c2.fLeftU, c2.fTopV, c2.fRightU, c2.fBottomV ); 
	}

	HRESULT KG3DPostRenderEffectDOF::DrawFullScreenQuad2TexCoord(
		float fLeftU, float fTopV, float fRightU, float fBottomV,float fLeftU2, float fTopV2, float fRightU2, float fBottomV2
		)
	{
		D3DSURFACE_DESC dtdsdRT;
		LPDIRECT3DSURFACE9 pSurfRT = NULL;
		KG3DRenderState RenderState;

		g_pd3dDevice->GetRenderTarget(0, &pSurfRT);
		pSurfRT->GetDesc(&dtdsdRT);

		FLOAT fWidth5 = (FLOAT)dtdsdRT.Width - 0.5f;
		FLOAT fHeight5 = (FLOAT)dtdsdRT.Height - 0.5f;

		ScreenVertex2Tex svQuad[4];

		svQuad[0].p = D3DXVECTOR4(-0.5f, -0.5f, 0.5f, 1.0f);
		svQuad[0].t = D3DXVECTOR2(fLeftU, fTopV);
		svQuad[0].s = D3DXVECTOR2(fLeftU2, fTopV2);

		svQuad[1].p = D3DXVECTOR4(fWidth5, -0.5f, 0.5f, 1.0f);
		svQuad[1].t = D3DXVECTOR2(fRightU, fTopV);
		svQuad[1].s = D3DXVECTOR2(fRightU2, fTopV2);

		svQuad[2].p = D3DXVECTOR4(-0.5f, fHeight5, 0.5f, 1.0f);
		svQuad[2].t = D3DXVECTOR2(fLeftU, fBottomV);
		svQuad[2].s = D3DXVECTOR2(fLeftU2, fBottomV2);

		svQuad[3].p = D3DXVECTOR4(fWidth5, fHeight5, 0.5f, 1.0f);
		svQuad[3].t = D3DXVECTOR2(fRightU, fBottomV);
		svQuad[3].s = D3DXVECTOR2(fRightU2, fBottomV2);

		RenderState.SetRenderState(D3DRS_ZENABLE, FALSE);
		RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
		RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		g_pd3dDevice->SetFVF(ScreenVertex2Tex::FVF);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex2Tex));
		SAFE_RELEASE(pSurfRT);
		return S_OK;
	}
	float KG3DPostRenderEffectDOF::GetGaussianDistribution( float x, float y, float rho )
	{
		float g = 1.0f / sqrt( 2.0f * 3.141592654f * rho * rho );
		return g * exp( -(x * x + y * y) / (2 * rho * rho) );
	}

	void KG3DPostRenderEffectDOF::GetGaussianOffsets( bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
		D3DXVECTOR2* vSampleOffsets, float* fSampleWeights )
	{
		// Get the center texel offset and weight
		fSampleWeights[0] = 1.0f * GetGaussianDistribution( 0, 0, 2.0f );
		vSampleOffsets[0] = D3DXVECTOR2( 0.0f, 0.0f );
		// Get the offsets and weights for the remaining taps
		if( bHorizontal )
		{
			for( int i = 1; i < 15; i += 2 )
			{
				vSampleOffsets[i + 0] = D3DXVECTOR2( i * vViewportTexelSize.x, 0.0f );
				vSampleOffsets[i + 1] = D3DXVECTOR2( -i * vViewportTexelSize.x, 0.0f );
				fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution( float(i + 0), 0.0f, 3.0f );
				fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution( float(i + 1), 0.0f, 3.0f );
			}
		}
		else 
		{
			for( int i = 1; i < 15; i += 2 )
			{
				vSampleOffsets[i + 0] = D3DXVECTOR2( 0.0f, i * vViewportTexelSize.y );
				vSampleOffsets[i + 1] = D3DXVECTOR2( 0.0f, -i * vViewportTexelSize.y );
				fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution( 0.0f, float(i + 0), 3.0f );
				fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution( 0.0f, float(i + 1), 3.0f );
			}
		}
	}
}


