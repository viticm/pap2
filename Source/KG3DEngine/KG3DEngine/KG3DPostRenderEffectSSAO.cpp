
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectSSAO.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : Update 2007-07-05 12:49:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "KG3DPostRenderEffectSSAO.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DDefaultShaderGroup.h"

namespace PostRender {

    static float random(float s, float d)
    {
        float k = (float)rand() / (float)RAND_MAX;
        return s + k * (d - s);
    }


KG3DPostRenderEffectSSAO::KG3DPostRenderEffectSSAO(KG3DPostRenderEffectManager* pMgr)
: KG3DPostRenderEffectBase(pMgr)
, m_pBlurTexture(NULL)
, m_pBlurSurface(NULL)
, m_pSSAOTexture(NULL)
, m_pSSAOSurface(NULL)
, m_dwTargetWidth(0)
, m_dwTargetHeight(0)
, m_fBlurWeight(1.f)
, m_pNoiseTexture(NULL)
{
}

KG3DPostRenderEffectSSAO::~KG3DPostRenderEffectSSAO()
{
    CleanUp();
}

HRESULT KG3DPostRenderEffectSSAO::Enable(BOOL bEnable)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    BOOL bInitTextures = FALSE;

    if (bEnable)
    {  
        if (m_bEnable)
        {
            if ((m_pPostRenderEffectMgr->GetWidth()  == m_dwTargetWidth) &&
                (m_pPostRenderEffectMgr->GetHeight() == m_dwTargetHeight))
            {
                return S_OK;
            }
        }

        CleanUp();

        m_dwTargetWidth  = m_pPostRenderEffectMgr->GetWidth();
        m_dwTargetHeight = m_pPostRenderEffectMgr->GetHeight();

        hRetCode = InitTextures();
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bInitTextures = TRUE;
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

HRESULT KG3DPostRenderEffectSSAO::ApplyEffect(KG3DScene* pScene)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DXMATRIX matProj;
    D3DXMATRIX matInv;

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    matProj = pCamera->GetProjectionMatrix();

    LPD3DXEFFECT       pEffect       = NULL;
    LPDIRECT3DSURFACE9 pTargetSave   = NULL;
    D3DVIEWPORT9       ViewPortSave;

    m_Viewport.X = 0;
    m_Viewport.Y = 0;
    m_Viewport.Width  = (DWORD)(m_dwTargetWidth * m_pPostRenderEffectMgr->m_fScalw);
    m_Viewport.Height = (DWORD)(m_dwTargetHeight * m_pPostRenderEffectMgr->m_fScalh);
    m_Viewport.MinZ = 0.f;
    m_Viewport.MaxZ = 1.f;

	hRetCode = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
	hRetCode = g_pd3dDevice->GetViewport(&ViewPortSave);

    KG_PROCESS_ERROR(m_pPostRenderEffectMgr);

	pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
	//pEffect = m_pPostRenderEffectMgr->m_pEffect;//<PostRenderShadowMove>
    KG_PROCESS_ERROR(pEffect);

    KG_PROCESS_ERROR(m_pPostRenderEffectMgr->m_lpShareRenderTarger);

    hRetCode = g_pd3dDevice->SetTexture(0, m_pPostRenderEffectMgr->GetRenderTexture());
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = g_pd3dDevice->SetTexture(1, m_pPostRenderEffectMgr->GetDepthTexture());
    KG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = g_pd3dDevice->SetTexture(3, m_pNoiseTexture);
	KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pEffect->SetTechnique(TEXT("FinalSSAO"));
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pEffect->SetMatrix(TEXT("g_Proj"), &matProj);
    D3DXMatrixInverse(&matInv, NULL, &matProj);
    hRetCode = pEffect->SetMatrix(TEXT("g_ProjInv"), &matInv);

    float MaxU = m_pPostRenderEffectMgr->m_fScalw;
    float MaxV = m_pPostRenderEffectMgr->m_fScalh;

    hRetCode = pEffect->SetFloat(TEXT("MaxU"), MaxU);
    hRetCode = pEffect->SetFloat(TEXT("MaxV"), MaxV);

	hRetCode = pEffect->SetFloat(TEXT("fWidth"), static_cast<FLOAT>(m_Viewport.Width));
    hRetCode = pEffect->SetFloat(TEXT("fHeight"), static_cast<FLOAT>(m_Viewport.Height));

    hRetCode = pEffect->SetFloat(TEXT("radius"), KG3DPostRenderEffectManager::m_sEffectParams.fSSAOSampleRadius);
    hRetCode = pEffect->SetFloat(TEXT("dark"),   KG3DPostRenderEffectManager::m_sEffectParams.fSSAODarkFact);

    static D3DXVECTOR3 vRandOffset[32];
    static bool bInit = false;
    if (!bInit)
    {
        int i = 0;

        
		vRandOffset[i++] = D3DXVECTOR3(-1.f, -1.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3( 0.f, -1.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3( 1.f, -1.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3(-1.f,  0.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3( 1.f,  0.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3(-1.f,  1.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3( 0.f,  1.f, 1.f);
		vRandOffset[i++] = D3DXVECTOR3( 1.f,  1.f, 1.f);//8

		vRandOffset[i++] = D3DXVECTOR3(-2.f, -2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3(-1.f, -2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 0.f, -2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 1.f, -2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 2.f, -2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3(-2.f, -1.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 2.f, -1.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3(-2.f,  0.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 2.f,  0.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3(-2.f,  1.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 2.f,  1.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3(-2.f,  2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3(-1.f,  2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 0.f,  2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 1.f,  2.f, 0.5f);
		vRandOffset[i++] = D3DXVECTOR3( 2.f,  2.f, 0.5f);//24
       

        while (i < 32)
        {
            vRandOffset[i] = D3DXVECTOR3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
            D3DXVec3Normalize(&vRandOffset[i], &vRandOffset[i]);
            vRandOffset[i] *= random(0.1f, 1.f);
            ++i;
        }

        bInit = true;
    }

    hRetCode = pEffect->SetValue(TEXT("random"), vRandOffset, (UINT)sizeof(vRandOffset));
    KG_COM_PROCESS_ERROR(hRetCode);

    UINT uPass = 0;

    switch (KG3DPostRenderEffectManager::m_sEffectParams.nSSAOLevel)
    {
    case SSAO_LEVEL_LOWER :
        uPass = 1;
        break;
    case SSAO_LEVEL_NORMAL :
        uPass = 2;
        break;
    case SSAO_LEVEL_HIGH :
        uPass = 3;
        break;
    default :
        uPass = 2;
        break;
    }

    hRetCode = g_pd3dDevice->SetRenderTarget(0, m_pSSAOSurface);
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = g_pd3dDevice->SetViewport(&m_Viewport);
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = m_pPostRenderEffectMgr->Draw(uPass, 0.f, m_Viewport);
    KG_COM_PROCESS_ERROR(hRetCode);


    // blur

	/*hRetCode = pEffect->SetFloat(TEXT("fBiasU"),  KG3DPostRenderEffectManager::m_sEffectParams.fBlurWeight * m_pPostRenderEffectMgr->m_fScalw / (float)m_dwTargetWidth);
	hRetCode = pEffect->SetFloat(TEXT("fBiasV"),  KG3DPostRenderEffectManager::m_sEffectParams.fBlurWeight * m_pPostRenderEffectMgr->m_fScalh / (float)m_dwTargetHeight);*/
	{
		FLOAT fU = KG3DPostRenderEffectManager::m_sEffectParams.fBlurWeight * m_pPostRenderEffectMgr->m_fScalw / (float)m_dwTargetWidth;
		FLOAT fV = KG3DPostRenderEffectManager::m_sEffectParams.fBlurWeight * m_pPostRenderEffectMgr->m_fScalh / (float)m_dwTargetHeight;

		hRetCode = pEffect->SetValue("vBiasUV", &D3DXVECTOR2(fU, fV), sizeof(D3DXVECTOR2));
	}

    hRetCode = g_pd3dDevice->SetRenderTarget(0, m_pBlurSurface);
    hRetCode = g_pd3dDevice->SetViewport(&m_Viewport);
    hRetCode = g_pd3dDevice->SetTexture(0, m_pSSAOTexture);
	hRetCode = g_pd3dDevice->SetTexture(1, m_pPostRenderEffectMgr->GetDepthTexture());
	KG_COM_PROCESS_ERROR(hRetCode);

	{
		//GraphicsStruct::RenderStateAlpha Alpha(0x0,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

		hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
		KG_COM_PROCESS_ERROR(hRetCode);

		m_pPostRenderEffectMgr->Draw(4, 0.f, m_Viewport);

	}
    hRetCode = g_pd3dDevice->SetRenderTarget(0, m_pSSAOSurface);
    hRetCode = g_pd3dDevice->SetViewport(&m_Viewport);
    hRetCode = g_pd3dDevice->SetTexture(0, m_pBlurTexture);

    hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
    KG_COM_PROCESS_ERROR(hRetCode);

    m_pPostRenderEffectMgr->Draw(5, 0.f, m_Viewport);
    hResult = S_OK;
Exit0:
	hRetCode = g_pd3dDevice->SetRenderTarget(0, pTargetSave);
	hRetCode = g_pd3dDevice->SetViewport(&ViewPortSave);

	g_pd3dDevice->SetTexture(0, NULL);
	g_pd3dDevice->SetTexture(1, NULL);
    SAFE_RELEASE(pTargetSave);
    return hResult;
}

HRESULT KG3DPostRenderEffectSSAO::ReloadFX()
{
    return S_OK;
}

HRESULT KG3DPostRenderEffectSSAO::InitTextures()
{
    HRESULT hr = S_OK;

    ASSERT(m_dwTargetWidth > 0 && m_dwTargetHeight > 0);
    ASSERT(m_pBlurTexture == NULL);
    ASSERT(m_pBlurSurface == NULL);
    ASSERT(m_pSSAOTexture == NULL);
    ASSERT(m_pSSAOSurface == NULL);
	ASSERT(m_pNoiseTexture == NULL);

    hr = g_pd3dDevice->CreateTexture((UINT)m_dwTargetWidth, (UINT)m_dwTargetHeight, 1, 
        D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &m_pBlurTexture, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->CreateTexture((UINT)m_dwTargetWidth, (UINT)m_dwTargetHeight, 1, 
        D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &m_pSSAOTexture, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pBlurTexture->GetSurfaceLevel(0, &m_pBlurSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pSSAOTexture->GetSurfaceLevel(0, &m_pSSAOSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

	//hr = D3DXCreateTextureFromFile(g_pd3dDevice,"data\\public\\noise4x4.dds",&m_pNoiseTexture);
	hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice,"data\\public\\noise4x4.dds",&m_pNoiseTexture);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

HRESULT KG3DPostRenderEffectSSAO::UnInitTextures()
{
    HRESULT hr = S_OK;

    SAFE_RELEASE(m_pBlurSurface);
    SAFE_RELEASE(m_pBlurTexture);
    SAFE_RELEASE(m_pSSAOTexture);
    SAFE_RELEASE(m_pSSAOSurface);
	SAFE_RELEASE(m_pNoiseTexture);

    return hr;
}

HRESULT KG3DPostRenderEffectSSAO::OnLostDevice()
{
    HRESULT hr = S_OK;

    hr = UnInitTextures();
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

HRESULT KG3DPostRenderEffectSSAO::OnResetDevice()
{
    HRESULT hr = S_OK;
    BOOL bInitTextures = FALSE;

    if (m_bEnable)
    {
		hr = UnInitTextures();
		KGLOG_COM_PROCESS_ERROR(hr);

        hr = InitTextures();
        KGLOG_COM_PROCESS_ERROR(hr);

        bInitTextures = TRUE;
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

void KG3DPostRenderEffectSSAO::CleanUp()
{
    UnInitTextures();
}

}