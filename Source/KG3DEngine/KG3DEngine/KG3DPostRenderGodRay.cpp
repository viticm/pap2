
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderGodRay.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : Update 2007-07-05 12:49:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DPostRenderGodRay.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DScene.h"
#include "KG3DEnvironment.h"
#include "IEKG3DMeshLensFlare.h"
#include "KG3DDefaultShaderGroup.h"

    
namespace PostRender {

    KG3DPostRenderGodRay::KG3DPostRenderGodRay(KG3DPostRenderEffectManager* pMgr)
      : KG3DPostRenderEffectBase(pMgr),
        m_pRayTarget(NULL),
        m_pRaySuface(NULL),
        m_pDepthStencilSurface(NULL),
        m_pTmpTarget(NULL),
        m_pTmpSuface(NULL),
        m_dwTargetHeight(0),
        m_dwTargetWidth(0),
        m_nSample(3),
        m_fScal(0.25f)
    {
    }

    KG3DPostRenderGodRay::~KG3DPostRenderGodRay()
    {
        CleanUp();
    }

    void KG3DPostRenderGodRay::CleanUp()
    {
        UnInitTextures();
    }

    HRESULT KG3DPostRenderGodRay::Enable(BOOL bEnable)
    {
        HRESULT hResult    = E_FAIL;
        HRESULT hRetCode   = E_FAIL;
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
    Exit0 :
        if (FAILED(hResult))
        {
            if (bInitTextures)
            {
                UnInitTextures();
            }
        }

        return hResult;
    }

    HRESULT KG3DPostRenderGodRay::ReloadFX()
    {
        return S_OK;
    }
    
    HRESULT KG3DPostRenderGodRay::InitTextures()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

        UINT uTargetWidth  = (UINT)(m_dwTargetWidth * m_fScal);
        UINT uTargetHeight = (UINT)(m_dwTargetHeight * m_fScal);
        D3DSURFACE_DESC Desc;

        ASSERT(uTargetWidth > 0 && uTargetHeight > 0);
        ASSERT(m_pDepthStencilSurface == NULL);
        ASSERT(m_pRayTarget == NULL);
        ASSERT(m_pRaySuface == NULL);
        ASSERT(m_pTmpTarget == NULL);
        ASSERT(m_pTmpSuface == NULL);

        hr = g_pd3dDevice->CreateTexture(uTargetWidth, uTargetHeight, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRayTarget, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateTexture(uTargetWidth, uTargetHeight, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTmpTarget, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_pRayTarget->GetSurfaceLevel(0, &m_pRaySuface);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_pTmpTarget->GetSurfaceLevel(0, &m_pTmpSuface);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_pRaySuface->GetDesc(&Desc);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->CreateDepthStencilSurface(uTargetWidth, uTargetHeight, g_cGraphicsEngine.GetPresentParameters().AutoDepthStencilFormat, 
            Desc.MultiSampleType, Desc.MultiSampleQuality, FALSE, &m_pDepthStencilSurface, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hrResult = S_OK;
Exit0:
        if (FAILED(hrResult))
        {
            UnInitTextures();
        }

        return hrResult;
    }

    HRESULT KG3DPostRenderGodRay::UnInitTextures()
    {
        HRESULT hr = S_OK;

        SAFE_RELEASE(m_pDepthStencilSurface);
        SAFE_RELEASE(m_pRaySuface);
        SAFE_RELEASE(m_pTmpSuface);
        SAFE_RELEASE(m_pRayTarget);
        SAFE_RELEASE(m_pTmpTarget);

        return hr;
    }

    HRESULT KG3DPostRenderGodRay::OnLostDevice()
    {
        HRESULT hr = S_OK;

        hr = UnInitTextures();
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return hr;
    }

    HRESULT KG3DPostRenderGodRay::OnResetDevice()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;
        BOOL bInitTextures = FALSE;

        if (m_bEnable)
        {
			hr = UnInitTextures();

            hr = InitTextures();
            KGLOG_COM_PROCESS_ERROR(hr);

            bInitTextures = TRUE;
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

    bool KG3DPostRenderGodRay::BeActive(KG3DScene* pScene)
    {
        //D3DXVECTOR3	vSunDir = g_cGraphicsTool.GetSunLight(*pScene).GetDirection();
		D3DXVECTOR3 vSunDir = g_cGraphicsTool.GetSunLightParam(*pScene).dir;
        D3DXVec3Normalize(&vSunDir, &vSunDir);

        KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
        if (!pCamera)
            return false;

        D3DXVECTOR3 vCamDir = pCamera->GetCameraFront();
        D3DXVec3Normalize(&vCamDir, &vCamDir);

        float fAng = D3DX_PI - acosf(D3DXVec3Dot(&vSunDir, &vCamDir));

        float fAngLimit = KG3DPostRenderEffectManager::m_sEffectParams.fLimitAngle * 0.5f;

        if (fAng > fAngLimit)
            return false;

        return true;
    }

    HRESULT KG3DPostRenderGodRay::ApplyEffect(KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
		D3DSURFACE_DESC ray_desc;
        LPDIRECT3DSURFACE9 pDepthStencilSurfaceSave = NULL;
        BOOL bSaveViewport = FALSE;
        BOOL bSetRenderTarget = FALSE;
        BOOL bSetDepthStencilSurface = FALSE;

        LPDIRECT3DSURFACE9 pTargetSave  = NULL;
        D3DVIEWPORT9 ViewPortSave;
        KG3DRenderState R;

        float fMaxU = 0.f;
        float fMaxV = 0.f;

        m_Viewport.X = 0;
        m_Viewport.Y = 0;
        m_Viewport.MinZ = 0.f;
        m_Viewport.MaxZ = 1.f;

        KG_PROCESS_ERROR(BeActive(pScene));

		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
		KG_PROCESS_ERROR(pEffect);

        KG_PROCESS_ERROR(m_pRaySuface);
        KG_PROCESS_ERROR(m_pDepthStencilSurface);

		hRetCode = m_pRaySuface->GetDesc(&ray_desc);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		m_Viewport.Width  = (DWORD)(ray_desc.Width);
		m_Viewport.Height = (DWORD)(ray_desc.Height);

        hRetCode = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->GetDepthStencilSurface(&pDepthStencilSurfaceSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->GetViewport(&ViewPortSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSaveViewport = TRUE;

        R.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        R.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        R.SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
        
        hRetCode = pEffect->SetTechnique("FinalGodRay");
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        m_pPostRenderEffectMgr->GetDepthTexCoordScale(&fMaxU, &fMaxV);

        hRetCode = pEffect->SetFloat(TEXT("MaxU"), fMaxU);
        hRetCode = pEffect->SetFloat(TEXT("MaxV"), fMaxV);

        // pass 1

        hRetCode = g_pd3dDevice->SetRenderTarget(0, m_pRaySuface);
        KG_COM_PROCESS_ERROR(hRetCode);

        bSetRenderTarget = TRUE;

        hRetCode = g_pd3dDevice->SetDepthStencilSurface(m_pDepthStencilSurface);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSetDepthStencilSurface = TRUE;

        hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xff000000, 1.0f, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->SetTexture(0, m_pPostRenderEffectMgr->GetRenderTexture());// m_pTmpTarget);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->SetTexture(1, m_pPostRenderEffectMgr->GetDepthTexture());
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->SetViewport(&m_Viewport);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = pEffect->SetFloat("fScal", m_fScal);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pPostRenderEffectMgr->Draw(1, 0.f, m_Viewport);
        KG_COM_PROCESS_ERROR(hRetCode);

        // pass 2

        hRetCode = pEffect->SetVector("vSunDirScreen", &m_pPostRenderEffectMgr->GetSunScreenPos(pScene));
        KG_COM_PROCESS_ERROR(hRetCode);
       
        for (int i = 0; i < KG3DPostRenderEffectManager::m_sEffectParams.nGodRaySampleNum; ++i)
        {
            swap(m_pRayTarget, m_pTmpTarget);
            swap(m_pRaySuface, m_pTmpSuface);

            hRetCode = g_pd3dDevice->SetRenderTarget(0, m_pRaySuface);
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = g_pd3dDevice->SetViewport(&m_Viewport);
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = g_pd3dDevice->SetTexture(0, m_pTmpTarget);
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = m_pPostRenderEffectMgr->Draw(2, 0.f, m_Viewport);
            KG_COM_PROCESS_ERROR(hRetCode);
        }

        hResult = S_OK;
Exit0 :
        if (bSetDepthStencilSurface)
        {
            hRetCode = g_pd3dDevice->SetDepthStencilSurface(pDepthStencilSurfaceSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }

        SAFE_RELEASE(pDepthStencilSurfaceSave);

        if (bSetRenderTarget)
        {
            hRetCode = g_pd3dDevice->SetRenderTarget(0, pTargetSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }

        SAFE_RELEASE(pTargetSave);

        if (bSaveViewport)
        {
            hRetCode = g_pd3dDevice->SetViewport(&ViewPortSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }

        if (false)
        {
            D3DXSaveTextureToFile("d:\\a.bmp",  D3DXIFF_BMP, m_pRayTarget, 0);
            D3DXSaveTextureToFile("d:\\a.bmp",  D3DXIFF_BMP, m_pTmpTarget, 0);
            D3DXSaveTextureToFile("d:\\d.bmp",  D3DXIFF_BMP, m_pPostRenderEffectMgr->GetDepthTexture(), 0);
        }

        return hResult;
    }

}