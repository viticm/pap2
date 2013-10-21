
#include "StdAfx.h"
#include "KG3DPostRenderEffectShockWave.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DScene.h"
#include "KG3DTextureTable.h"
#include "KG3DTexture.h"
#include "KG3DWaterEffect.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace PostRender
{
    void KG3DPostRenderEffectShockWave::CleanUp()
    {
        SAFE_RELEASE(m_pBumpTexture);
        SAFE_RELEASE(m_pShaftTexture);
    }

    KG3DPostRenderEffectShockWave::KG3DPostRenderEffectShockWave(KG3DPostRenderEffectManager *pMgr)
        : KG3DPostRenderEffectBase(pMgr),
          m_pBumpTexture(NULL),
          m_pShaftTexture(NULL)
    {
    }

    KG3DPostRenderEffectShockWave::~KG3DPostRenderEffectShockWave(void)
    {
        CleanUp();
    }

    HRESULT KG3DPostRenderEffectShockWave::ReloadFX()
    {
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectShockWave::ApplyEffect(KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        LPDIRECT3DSURFACE9 pRenderTarget = NULL;
        LPDIRECT3DSURFACE9 pRenderTargetSave = NULL;
        D3DVIEWPORT9 ViewPort;
        bool bNeedRestore = false;

        KG_PROCESS_ERROR(pScene);

        pRenderTarget = m_pPostRenderEffectMgr->GetShareSurface();
        KG_PROCESS_ERROR(pRenderTarget);

        g_pd3dDevice->GetRenderTarget(0, &pRenderTargetSave);
        g_pd3dDevice->GetViewport(&ViewPort);

        bNeedRestore = true;

        hRetCode = g_pd3dDevice->SetRenderTarget(0, pRenderTarget);
        KG_COM_PROCESS_ERROR(hRetCode);

        g_pd3dDevice->SetViewport(&ViewPort);

        hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = pScene->ShockWaveRender();
        KG_COM_PROCESS_ERROR(hRetCode);

        hResult = S_OK;
Exit0 :
        if (bNeedRestore)
        {
            g_pd3dDevice->SetRenderTarget(0, pRenderTargetSave);
            g_pd3dDevice->SetViewport(&ViewPort);
        }

        SAFE_RELEASE(pRenderTargetSave);
        SAFE_RELEASE(pRenderTarget);
        
        return hResult;
    }

    HRESULT KG3DPostRenderEffectShockWave::Enable(BOOL bEnable)
    {
        if (bEnable == m_bEnable)
            return S_OK;
        
        m_bEnable = bEnable;

        if (m_bEnable)
        {
            HRESULT hr = g_cTextureTable.LoadResourceFromFile(SHOCK_WAVE_BUMP_FILE, 0, TEXOP_COMMON, (IKG3DResourceBase **)&m_pBumpTexture);
            g_cTextureTable.LoadResourceFromFile(WATER_SHAFT_FILE, 0, TEXOP_COMMON, (IKG3DResourceBase **)&m_pShaftTexture);
            return hr;
        }
        else
        {
            SAFE_RELEASE(m_pBumpTexture);
            SAFE_RELEASE(m_pShaftTexture);
        }

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectShockWave::OnLostDevice()
    {
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectShockWave::OnResetDevice()
    {
        return S_OK;
    }

    LPDIRECT3DTEXTURE9 KG3DPostRenderEffectShockWave::GetShockWaveTexture()
    {
        return m_pPostRenderEffectMgr->GetShareTexture();
    }

    LPDIRECT3DTEXTURE9 KG3DPostRenderEffectShockWave::GetWaveBumpTexture()
    {
        return m_pBumpTexture ? m_pBumpTexture->GetTexture() : NULL;
    }

    LPDIRECT3DTEXTURE9 KG3DPostRenderEffectShockWave::GetWaterShaftTexture()
    {
        return m_pShaftTexture ? m_pShaftTexture->GetTexture() : NULL;
    }

}
