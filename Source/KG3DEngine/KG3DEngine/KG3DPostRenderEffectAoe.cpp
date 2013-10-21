
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectAoe.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-8-29 10:52:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DPostRenderEffectAoe.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DScene.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DGraphiceEngine.h"

namespace PostRender
{
    void KG3DPostRenderEffectAoe::CleanUp()
    {
        SAFE_RELEASE(m_pEffect);
        SAFE_RELEASE(m_pResultSurface);
        SAFE_RELEASE(m_pResultTexture);
    }

    KG3DPostRenderEffectAoe::KG3DPostRenderEffectAoe(KG3DPostRenderEffectManager *pMgr)
        : KG3DPostRenderEffectBase(pMgr),
          m_pResultTexture(NULL),
          m_pResultSurface(NULL),
          m_pEffect(NULL)
    {
    }

    KG3DPostRenderEffectAoe::~KG3DPostRenderEffectAoe()
    {
        CleanUp();
    }

    HRESULT KG3DPostRenderEffectAoe::ReloadFX()
    {
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

        return KG3DD3DXCreateEffectFromFile(g_pd3dDevice, Aoe_Effect_File, macroPackType, NULL, 0, NULL, &m_pEffect, NULL);

    }

    HRESULT KG3DPostRenderEffectAoe::ApplyEffect(KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        LPDIRECT3DSURFACE9 pRenderTargetSave = NULL;
        D3DVIEWPORT9 ViewportSave;

        D3DXMATRIX matViewProj;
        D3DXMATRIX matView;
        D3DXMATRIX matProj;
        D3DXMATRIX matInv;
		float fMaxU = 1.0;
		float fMaxV = 1.0;

        const int PASS1_MAX_TEX_NUM = 4;
        D3DXVECTOR4 info[PASS1_MAX_TEX_NUM];
        D3DXCOLOR   diff[PASS1_MAX_TEX_NUM];
        D3DXVECTOR4 texInfo[PASS1_MAX_TEX_NUM];
        float       limt[PASS1_MAX_TEX_NUM];
        size_t iaoe = 0;

        UINT uPass = 0;
        BOOL bSetRenderTarget = FALSE;
        BOOL bSetViewport = FALSE;
        BOOL bBeginEffect = FALSE;

        KG_PROCESS_ERROR(m_pResultSurface);
        KG_PROCESS_ERROR(pScene);
        KG_PROCESS_ERROR(m_pEffect);

        hRetCode = g_pd3dDevice->GetRenderTarget(0, &pRenderTargetSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->GetViewport(&ViewportSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->SetRenderTarget(0, m_pResultSurface);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSetRenderTarget = TRUE;

        hRetCode = g_pd3dDevice->SetViewport(&ViewportSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSetViewport = TRUE;

        hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        KG_PROCESS_ERROR(g_ProjTextureRender.TexNumber() > 0);

        hRetCode = m_pEffect->SetTechnique(TEXT("aoe"));
        KG_COM_PROCESS_ERROR(hRetCode);



        g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
        g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

        matViewProj = matView * matProj;
        D3DXMatrixInverse(&matInv, NULL, &matViewProj);

        hRetCode = m_pEffect->SetMatrix(TEXT("g_MatViewPorjInv"), &matInv);
        hRetCode = m_pEffect->SetMatrix(TEXT("g_MatViewPorj"),    &matViewProj);
		m_pPostRenderEffectMgr->GetDepthTexCoordScale(&fMaxU, &fMaxV);
        hRetCode = m_pEffect->SetFloat(TEXT("MaxU"), fMaxU);
        hRetCode = m_pEffect->SetFloat(TEXT("MaxV"), fMaxV);

        hRetCode = m_pEffect->Begin(&uPass, 0);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bBeginEffect = TRUE;

        hRetCode = g_pd3dDevice->SetTexture(0, m_pPostRenderEffectMgr->GetDepthTexture());
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        while (iaoe < g_ProjTextureRender.TexNumber())
        {
            UINT i = 0;

			//////////////////////////////////////////////////////////////////////////
			g_ProjTextureRender.m_Lock.Lock();

            while (i < PASS1_MAX_TEX_NUM)
            {
                if (iaoe >= g_ProjTextureRender.TexNumber())
                    break;

                g_ProjTextureRender.GetInfo(&info[i], iaoe);
                g_ProjTextureRender.GetDiff(&diff[i], iaoe);
                g_ProjTextureRender.GetLimt(&limt[i], iaoe);
                g_ProjTextureRender.GetTexFrameInfo(&texInfo[i], iaoe);
                g_pd3dDevice->SetTexture(static_cast<DWORD>(i + 1), g_ProjTextureRender.GetTexture(iaoe));

                ++i;
                ++iaoe;
            }

			g_ProjTextureRender.m_Lock.Unlock();
			//////////////////////////////////////////////////////////////////////////

            hRetCode = m_pEffect->SetValue(TEXT("aoe_info"), info, sizeof(info));
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = m_pEffect->SetValue(TEXT("aoe_diff"), diff, sizeof(diff));
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = m_pEffect->SetValue(TEXT("aoe_limt"), limt, sizeof(limt));
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = m_pEffect->SetValue(TEXT("tex_info"), texInfo, sizeof(texInfo));
            KG_COM_PROCESS_ERROR(hRetCode);

            if (SUCCEEDED(hRetCode = m_pEffect->BeginPass(i - 1)))
            {
                DrawFullScreenQuad(
                    0.0f, 0.0f, 
                    m_pPostRenderEffectMgr->m_fScalw, 
                    m_pPostRenderEffectMgr->m_fScalh, 
                    static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.X), 
                    static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.Y),
                    static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.Width), 
                    static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.Height),
                    FALSE,
                    0.f
                    );
                m_pEffect->EndPass();
            }
        }

        hResult = S_OK;
Exit0 :
        if (bBeginEffect)
        {
            hRetCode = m_pEffect->End();
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }

        //hRetCode = g_ProjTextureRender.RemoveAllProjSource();
        //KGLOG_COM_CHECK_ERROR(hRetCode);

        if (bSetRenderTarget)
        {
            hRetCode = g_pd3dDevice->SetRenderTarget(0, pRenderTargetSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }
        SAFE_RELEASE(pRenderTargetSave);

        if (bSetViewport)
        {
            hRetCode = g_pd3dDevice->SetViewport(&ViewportSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }

        if (false)
            D3DXSaveTextureToFile("f:\\a.bmp", D3DXIFF_BMP, m_pResultTexture, NULL);

        return hResult; 
    }

    HRESULT KG3DPostRenderEffectAoe::ApplyEffectNoChangeTarget()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        D3DXMATRIX matViewProj;
        D3DXMATRIX matView;
        D3DXMATRIX matProj;
        D3DXMATRIX matInv;
        float fMaxU = 1.0;
        float fMaxV = 1.0;

        const int PASS1_MAX_TEX_NUM = 4;
        D3DXVECTOR4 info[PASS1_MAX_TEX_NUM];
        D3DXCOLOR   diff[PASS1_MAX_TEX_NUM];
        D3DXVECTOR4 texInfo[PASS1_MAX_TEX_NUM];
        float       limt[PASS1_MAX_TEX_NUM];
        size_t      iaoe = 0;
        UINT        uPass = 0;
        bool        bBegin = false;

        KG_PROCESS_ERROR(g_ProjTextureRender.TexNumber() > 0);
        KG_PROCESS_ERROR(m_pEffect);

        hRetCode = m_pEffect->SetTechnique(TEXT("aoe"));
        KG_COM_PROCESS_ERROR(hRetCode);

        g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
        g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

        matViewProj = matView * matProj;
        D3DXMatrixInverse(&matInv, NULL, &matViewProj);
		m_pPostRenderEffectMgr->GetDepthTexCoordScale(&fMaxU, &fMaxV);
        hRetCode = m_pEffect->SetMatrix(TEXT("g_MatViewPorjInv"), &matInv);
        hRetCode = m_pEffect->SetMatrix(TEXT("g_MatViewPorj"),    &matViewProj);
        hRetCode = m_pEffect->SetFloat(TEXT("MaxU"), fMaxU);
        hRetCode = m_pEffect->SetFloat(TEXT("MaxV"), fMaxV);

        if (SUCCEEDED(m_pEffect->Begin(&uPass, 0)))
        {
            bBegin = true;

            hRetCode = g_pd3dDevice->SetTexture(0, m_pPostRenderEffectMgr->GetDepthTexture());
            KGLOG_COM_PROCESS_ERROR(hRetCode);


            while (iaoe < g_ProjTextureRender.TexNumber())
            {
                UINT i = 0;

				//////////////////////////////////////////////////////////////////////////
				g_ProjTextureRender.m_Lock.Lock();

                while (i < PASS1_MAX_TEX_NUM)
                {
                    if (iaoe >= g_ProjTextureRender.TexNumber())
                        break;

                    g_ProjTextureRender.GetInfo(&info[i], iaoe);
                    g_ProjTextureRender.GetDiff(&diff[i], iaoe);
                    g_ProjTextureRender.GetLimt(&limt[i], iaoe);
                    g_ProjTextureRender.GetTexFrameInfo(&texInfo[i], iaoe);
                    g_pd3dDevice->SetTexture(static_cast<DWORD>(i + 1), g_ProjTextureRender.GetTexture(iaoe));

                    ++i;
                    ++iaoe;
                }

				g_ProjTextureRender.m_Lock.Unlock();
				//////////////////////////////////////////////////////////////////////////

                hRetCode = m_pEffect->SetValue(TEXT("aoe_info"), info, sizeof(info));
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = m_pEffect->SetValue(TEXT("aoe_diff"), diff, sizeof(diff));
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = m_pEffect->SetValue(TEXT("aoe_limt"), limt, sizeof(limt));
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = m_pEffect->SetValue(TEXT("tex_info"), texInfo, sizeof(texInfo));
                KG_COM_PROCESS_ERROR(hRetCode);

                if (SUCCEEDED(hRetCode = m_pEffect->BeginPass(i - 1)))
                {
                    DrawFullScreenQuad(
                        0.f, 
                        0.f, 
                        fMaxU, 
                        fMaxV, 
                        static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.X), 
                        static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.Y),
                        static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.Width), 
                        static_cast<float>(m_pPostRenderEffectMgr->m_ViewportSave.Height),
                        TRUE,
                        0.f
                        );
                    m_pEffect->EndPass();
                }
            }

            hRetCode = m_pEffect->End();
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }
        else
            KG_PROCESS_ERROR(false);

        hResult = S_OK;
Exit0 :
        if (bBegin)
            m_pEffect->End();
        return hResult; 
    }

    HRESULT KG3DPostRenderEffectAoe::ReCreateTexture()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        SAFE_RELEASE(m_pResultSurface);
        SAFE_RELEASE(m_pResultTexture);

        hRetCode = D3DXCreateTexture(
            g_pd3dDevice,
            m_pPostRenderEffectMgr->m_dwPostRenderWidth, 
            m_pPostRenderEffectMgr->m_dwPostRenderHeight, 
            1, 
            D3DUSAGE_RENDERTARGET, 
            D3DFMT_A8R8G8B8, 
            D3DPOOL_DEFAULT, 
            &m_pResultTexture
            );
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = m_pResultTexture->GetSurfaceLevel(0, &m_pResultSurface);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->ColorFill(m_pResultSurface, NULL, D3DCOLOR_ARGB(0, 0, 0, 0));
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hResult = S_OK;
Exit0 :
        if (FAILED(hResult))
        {
            SAFE_RELEASE(m_pResultSurface);
            SAFE_RELEASE(m_pResultTexture);
        }
        return hResult;
    }

    HRESULT KG3DPostRenderEffectAoe::Enable(BOOL bEnable)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        if (bEnable == m_bEnable)
            return S_OK;

        CleanUp();

        if (bEnable)
        {
            hRetCode = ReCreateTexture();
            KG_COM_PROCESS_ERROR(hRetCode);
            ReloadFX();
        }

        m_bEnable = bEnable;
       
        hResult = S_OK;
Exit0 :
        return hResult;
    }

    HRESULT KG3DPostRenderEffectAoe::OnLostDevice()
    {
        CleanUp();
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectAoe::OnResetDevice()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        if (m_bEnable)
        {
            hRetCode = ReCreateTexture();
            KG_COM_PROCESS_ERROR(hRetCode);
            hRetCode = ReloadFX();
            KG_COM_PROCESS_ERROR(hRetCode);
        }

        hResult = S_OK;
Exit0 :
        return hResult;
    }

}