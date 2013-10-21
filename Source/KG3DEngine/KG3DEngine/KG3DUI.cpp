#include "stdafx.h"
#include "./KG3DUI.h"
#include "./KG3DGraphiceEngine.h"
#include "./KG3DTextureTable.h"
#include "./KG3DModelTable.h"
#include "./KG3DEngineManager.h"
#include "./KG3DFrustum.h"
#include "./KG3DFontTexture.h"
#include "./KG3DFontRenderer.h"
#include "./KG3DUIMiniScene.h"
#include "./KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------------------------------------------
namespace 
{ 
	// log: gameart  2006-03-17 
	// font manager 

	KG3DFontRenderer* g_pFontRenderer = NULL;
	KG3DFontTexture* g_FontTable[RS2_MAX_FONT_ITEM_NUM] = { NULL };

	DWORD const FVF_2D_TEX1_VERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;


	DWORD const NUMS_2D_DIFFUSE_VERTEX = 1024;
	DWORD const NUMS_2D_TEX1_VERTEX = 2048;
	DWORD const NUMS_2D_TEX2_VERTEX = 128;

}

KG3DUIRenderStateGuard::KG3DUIRenderStateGuard()
: m_bSaved(FALSE)
{
}

KG3DUIRenderStateGuard::KG3DUIRenderStateGuard(D3DRENDERSTATETYPE type, DWORD dwValue)
{
	Save(type, dwValue);
}

KG3DUIRenderStateGuard::~KG3DUIRenderStateGuard()
{
	Restore();
}

void KG3DUIRenderStateGuard::Save(D3DRENDERSTATETYPE type)
{
    HRESULT hr = E_FAIL;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

	m_type = type;

	hr = g_pd3dDevice->GetRenderState(type, &m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

Exit0:
    return;
}

void KG3DUIRenderStateGuard::Save(D3DRENDERSTATETYPE type, DWORD dwValue)
{
    HRESULT hr = E_FAIL;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    ASSERT(!m_bSaved);

	m_type = type;

	hr = g_pd3dDevice->GetRenderState(type, &m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

	hr = g_pd3dDevice->SetRenderState(type, dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

void KG3DUIRenderStateGuard::Restore()
{
    HRESULT hr = E_FAIL;

    KG_PROCESS_SUCCESS(!m_bSaved);

    m_bSaved = FALSE;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	hr = g_pd3dDevice->SetRenderState(m_type, m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
Exit0:
    return;
}


KG3DUITextureStageStateGuard::KG3DUITextureStageStateGuard()
: m_dwStage(0)
, m_bSaved(FALSE)
{
}

KG3DUITextureStageStateGuard::KG3DUITextureStageStateGuard(DWORD dwStage, D3DTEXTURESTAGESTATETYPE type, DWORD dwValue)
{
	Save(dwStage, type, dwValue);
}

KG3DUITextureStageStateGuard::~KG3DUITextureStageStateGuard()
{
	Restore();
}

void KG3DUITextureStageStateGuard::Save(DWORD dwStage, D3DTEXTURESTAGESTATETYPE type)
{
    HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    ASSERT(!m_bSaved);

    m_dwStage = dwStage;
    m_type = type;

    hr = g_pd3dDevice->GetTextureStageState(m_dwStage, type, &m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

Exit0:
    return;
}

void KG3DUITextureStageStateGuard::Save(DWORD dwStage, D3DTEXTURESTAGESTATETYPE type, DWORD dwValue)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    ASSERT(!m_bSaved);

    m_dwStage = dwStage;
    m_type = type;

    hr = g_pd3dDevice->GetTextureStageState(m_dwStage, type, &m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

    hr = g_pd3dDevice->SetTextureStageState(m_dwStage, type, dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_bSaved)
        {
            m_bSaved = FALSE;

            hr = g_pd3dDevice->SetTextureStageState(m_dwStage, m_type, m_dwValue);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
}

void KG3DUITextureStageStateGuard::Restore()
{
    HRESULT hr = E_FAIL;

    KG_PROCESS_SUCCESS(!m_bSaved);

    m_bSaved = FALSE;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->SetTextureStageState(m_dwStage, m_type, m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
Exit0:
    return;
}

KG3DUISamplerStateGuard::KG3DUISamplerStateGuard()
: m_dwSampler(0)
, m_bSaved(FALSE)
{
}

KG3DUISamplerStateGuard::KG3DUISamplerStateGuard(DWORD dwSampler, D3DSAMPLERSTATETYPE  type, DWORD dwValue)
{
	Save(dwSampler, type, dwValue);
}

KG3DUISamplerStateGuard::~KG3DUISamplerStateGuard()
{
	Restore();
}

void KG3DUISamplerStateGuard::Save(DWORD dwSampler, D3DSAMPLERSTATETYPE type)
{
    HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    ASSERT(!m_bSaved);

    m_dwSampler = dwSampler;
    m_type = type;

    hr = g_pd3dDevice->GetSamplerState(m_dwSampler, type, &m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

Exit0:
    return;
}

void KG3DUISamplerStateGuard::Save(DWORD dwSampler, D3DSAMPLERSTATETYPE type, DWORD dwValue)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    ASSERT(!m_bSaved);

    m_dwSampler = dwSampler;
    m_type = type;

    hr = g_pd3dDevice->GetSamplerState(m_dwSampler, type, &m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

    hr = g_pd3dDevice->SetSamplerState(m_dwSampler, type, dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_bSaved)
        {
            m_bSaved = FALSE;

            hr = g_pd3dDevice->SetSamplerState(m_dwSampler, m_type, m_dwValue);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
}

void KG3DUISamplerStateGuard::Restore()
{
    HRESULT hr = E_FAIL;

    KG_PROCESS_SUCCESS(!m_bSaved);

    m_bSaved = FALSE;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->SetSamplerState(m_dwSampler, m_type, m_dwValue);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
Exit0:
    return;
}

KG3DUIViewportGuard::KG3DUIViewportGuard()
: m_bSaved(FALSE)
{
}

KG3DUIViewportGuard::KG3DUIViewportGuard(D3DVIEWPORT9 const& Viewport)
{
	Save(Viewport);
}

KG3DUIViewportGuard::~KG3DUIViewportGuard()
{
	Restore();
}

void KG3DUIViewportGuard::Save()
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->GetViewport(&m_Viewport);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

Exit0:
    return;
}

void KG3DUIViewportGuard::Save(D3DVIEWPORT9 const& Viewport)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->GetViewport(&m_Viewport);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_bSaved = TRUE;

    hr = g_pd3dDevice->SetViewport(&Viewport);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_bSaved)
        {
            m_bSaved = FALSE;

            hr = g_pd3dDevice->SetViewport(&m_Viewport);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
}

void KG3DUIViewportGuard::Set(D3DVIEWPORT9 const& Viewport)
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->SetViewport(&Viewport);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

void KG3DUIViewportGuard::Restore()
{
	HRESULT hr = E_FAIL;

    KG_PROCESS_SUCCESS(!m_bSaved);

    m_bSaved = FALSE;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->SetViewport(&m_Viewport);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
Exit0:
    return;
}

KG3DUITextureRenderTargetGuard::KG3DUITextureRenderTargetGuard()
: m_dwRenderTargetIndex(0)
, m_pTexture(NULL)
, m_pDepthStencilSurface(NULL)
, m_pSurface(NULL)
, m_pSurfaceOld(NULL)
, m_pDepthStencilSurfaceOld(NULL)
, m_bSaved(FALSE)
{
}

KG3DUITextureRenderTargetGuard::~KG3DUITextureRenderTargetGuard()
{
	Restore();
}

void KG3DUITextureRenderTargetGuard::Save(DWORD dwRenderTargetIndex, LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2* pv2Min, D3DXVECTOR2* pv2Max, UINT uMipmapLevel)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	D3DSURFACE_DESC Desc;
	D3DVIEWPORT9 Viewport;
	D3DXMATRIX mProjection;
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);
	KGLOG_PROCESS_ERROR(pTexture);

	m_dwRenderTargetIndex = dwRenderTargetIndex;
	m_pTexture = pTexture;

	hr = m_pTexture->GetLevelDesc(uMipmapLevel, &Desc);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = m_pTexture->GetSurfaceLevel(uMipmapLevel, &m_pSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->GetRenderTarget(m_dwRenderTargetIndex, &m_pSurfaceOld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->GetDepthStencilSurface(&m_pDepthStencilSurfaceOld);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	m_ViewportGuard.Save();

    m_bSaved = TRUE;

    ASSERT(m_pDepthStencilSurface == NULL);

    hr = g_pd3dDevice->CreateDepthStencilSurface(Desc.Width, Desc.Height, g_cGraphicsEngine.GetPresentParameters().AutoDepthStencilFormat, 
        Desc.MultiSampleType, Desc.MultiSampleQuality, FALSE, &m_pDepthStencilSurface, NULL);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->SetRenderTarget(m_dwRenderTargetIndex, m_pSurface);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetDepthStencilSurface(m_pDepthStencilSurface);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	if (pv2Min == NULL && pv2Max == NULL)
	{
		Viewport.X = 0;
		Viewport.Y = 0;
		Viewport.Width = Desc.Width;
		Viewport.Height = Desc.Height;
		Viewport.MinZ = 0.0f;
		Viewport.MaxZ = 1.0f;

		m_ViewportGuard.Set(Viewport);

		hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}
	else
	{
		D3DRECT Rect;

		ASSERT(pv2Min);
		ASSERT(pv2Max);

		Rect.x1 = (LONG)(pv2Min->x * Desc.Width);
		Rect.y1 = (LONG)(pv2Min->y * Desc.Height);
		Rect.x2 = (LONG)(pv2Max->x * Desc.Width);
		Rect.y2 = (LONG)(pv2Max->y * Desc.Height);

		Viewport.X = Rect.x1;
		Viewport.Y = Rect.y1;
		Viewport.Width = Rect.x2 - Rect.x1;
		Viewport.Height = Rect.y2 - Rect.y1;
		Viewport.MinZ = 0.0f;
		Viewport.MaxZ = 1.0f;

		m_ViewportGuard.Set(Viewport);

		hr = g_pd3dDevice->Clear(1, &Rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}

    hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_mProjection);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &m_mWorld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_VIEW, &m_mView);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    D3DXMatrixOrthoLH(&mProjection, (float)Viewport.Width, -(float)Viewport.Height, 0.0f, 1.0f);

	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixTranslation(&mWorld, Viewport.Width * -0.5f, Viewport.Height * -0.5f, 1.0f);

	D3DXMatrixIdentity(&mView);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mProjection);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &mWorld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &mView);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        m_bSaved = FALSE;

        if (m_pDepthStencilSurfaceOld)
        {
            hr = g_pd3dDevice->SetDepthStencilSurface(m_pDepthStencilSurfaceOld);
            KGLOG_COM_CHECK_ERROR(hr);

            SAFE_RELEASE(m_pDepthStencilSurfaceOld);
        }

        if (m_pSurfaceOld)
        {
            hr = g_pd3dDevice->SetRenderTarget(m_dwRenderTargetIndex, m_pSurfaceOld);
            KGLOG_COM_CHECK_ERROR(hr);

            m_ViewportGuard.Restore();

            SAFE_RELEASE(m_pSurfaceOld);
        }

        SAFE_RELEASE(m_pDepthStencilSurface);
        SAFE_RELEASE(m_pSurface);
    }
}

void KG3DUITextureRenderTargetGuard::Restore()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_SUCCESS(!m_bSaved);

    m_bSaved = FALSE;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    if (m_pDepthStencilSurfaceOld)
    {
        hr = g_pd3dDevice->SetDepthStencilSurface(m_pDepthStencilSurfaceOld);
        KGLOG_COM_CHECK_ERROR(hr);

        SAFE_RELEASE(m_pDepthStencilSurfaceOld);
    }

    if (m_pSurfaceOld)
    {
        hr = g_pd3dDevice->SetRenderTarget(m_dwRenderTargetIndex, m_pSurfaceOld);
        KGLOG_COM_CHECK_ERROR(hr);

        m_ViewportGuard.Restore();

        SAFE_RELEASE(m_pSurfaceOld);
    }

    SAFE_RELEASE(m_pDepthStencilSurface);
    SAFE_RELEASE(m_pSurface);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mProjection);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_mView);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return;
}


HRESULT KG3DUITextureRenderTargetGuard::OnLostDevice()
{
    ASSERT(m_pDepthStencilSurface == NULL);
    return S_OK;
}

HRESULT KG3DUITextureRenderTargetGuard::OnResetDevice()
{
    ASSERT(m_pDepthStencilSurface == NULL);
    return S_OK;
}

KG3DUIRenderTargetStateGuard::KG3DUIRenderTargetStateGuard()
: m_p3DTexture(NULL)
{
}


KG3DUIRenderTargetStateGuard::~KG3DUIRenderTargetStateGuard()
{
    ASSERT(m_p3DTexture == NULL);
}


void KG3DUIRenderTargetStateGuard::Save(LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2* pvMin, D3DXVECTOR2* pvMax, UINT uMipmapLevel)
{
	m_aRenderStateGuard[ 0].Save(D3DRS_ALPHATESTENABLE, FALSE);
	m_aRenderStateGuard[ 1].Save(D3DRS_ALPHABLENDENABLE, TRUE);
	m_aRenderStateGuard[ 2].Save(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_aRenderStateGuard[ 3].Save(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_aRenderStateGuard[ 4].Save(D3DRS_LIGHTING, FALSE);
	m_aRenderStateGuard[ 5].Save(D3DRS_CULLMODE, D3DCULL_NONE);
	m_aRenderStateGuard[ 6].Save(D3DRS_FOGENABLE, FALSE);
	m_aRenderStateGuard[ 7].Save(D3DRS_ZENABLE, D3DZB_FALSE);
	m_aRenderStateGuard[ 8].Save(D3DRS_ZWRITEENABLE, D3DZB_FALSE);
	m_aRenderStateGuard[ 9].Save(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	m_aRenderStateGuard[10].Save(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
	m_aRenderStateGuard[11].Save(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	m_aTextureStage0StateGuard[0].Save(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	m_aTextureStage0StateGuard[1].Save(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_aTextureStage0StateGuard[2].Save(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_aTextureStage0StateGuard[3].Save(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_aTextureStage0StateGuard[4].Save(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_aTextureStage0StateGuard[5].Save(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_aTextureStage0StateGuard[6].Save(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_aTextureStage1StateGuard[0].Save(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_aTextureStage1StateGuard[1].Save(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_aSampler0StateGuard[0].Save(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_aSampler0StateGuard[1].Save(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_aSampler0StateGuard[2].Save(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	m_TextureRenderTargetGuard0.Save(0, pTexture, pvMin, pvMax, uMipmapLevel);

    g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->SetTexture(1, NULL);
}


void KG3DUIRenderTargetStateGuard::Save(DWORD dwTextureID, D3DXVECTOR2* pvMin, D3DXVECTOR2* pvMax, UINT uMipmapLevel)
{
	HRESULT hr = E_FAIL;

    ASSERT(m_p3DTexture == NULL);

	hr = g_cTextureTable.GetResource(&m_p3DTexture, dwTextureID);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	KGLOG_PROCESS_ERROR(m_p3DTexture);

	Save(m_p3DTexture->GetTexture(), pvMin, pvMax, uMipmapLevel);

Exit0:
	return;
}

void KG3DUIRenderTargetStateGuard::Restore()
{
	m_TextureRenderTargetGuard0.Restore();

	SAFE_RELEASE(m_p3DTexture);

	for (int nIndex = 0; nIndex < SAMPLER_0_STATE_GUARD_COUNT; ++nIndex)
		m_aSampler0StateGuard[nIndex].Restore();

	for (int nIndex = 0; nIndex < TEXTURE_STAGE_1_STATE_GUARD_COUNT; ++nIndex)
		m_aTextureStage1StateGuard[nIndex].Restore();

	for (int nIndex = 0; nIndex < TEXTURE_STAGE_0_STATE_GUARD_COUNT; ++nIndex)
		m_aTextureStage0StateGuard[nIndex].Restore();

	for (int nIndex = 0; nIndex < RENDER_STATE_GUARD_COUNT; ++nIndex)
		m_aRenderStateGuard[nIndex].Restore();
}

HRESULT KG3DUIRenderTargetStateGuard::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_TextureRenderTargetGuard0.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DUIRenderTargetStateGuard::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_TextureRenderTargetGuard0.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

//---------------------------------------------------------------------------------------------------------------------------
IKG3DUI& g_GetUI()
{
	static KG3DUI s_cUI;
	return s_cUI;
}


//--------------------------------------------------------------------------------------------------------------------------
KG3DUI::KG3DUI()
: m_pCallback(NULL)
, m_pUITexture(NULL)
, m_DiffuseVertex( NUMS_2D_DIFFUSE_VERTEX )
, m_Tex1Vertex( NUMS_2D_TEX1_VERTEX )
, m_uViewportWidth(1280)
, m_uViewportHeight(960)
#ifdef ENABLE_VIDEO_MEMORY_INFO
, m_pDirectDraw(NULL)
#endif
#ifdef ENABLE_KG3DUI_EFFECT
, m_pEffect(NULL)
#endif
, m_DrawState(DRAW_STATE_NONE)
{
}

KG3DUI::~KG3DUI()
{
#ifdef ENABLE_KG3DUI_EFFECT
	ASSERT(m_pEffect == NULL);
#endif
    ASSERT(m_pUITexture == NULL);
}

HRESULT KG3DUI::UnloadAllFont()
{
	for (DWORD dwFontID = 0; dwFontID < RS2_MAX_FONT_ITEM_NUM; ++dwFontID)
	{
		if (g_FontTable[dwFontID])
		{
			g_FontTable[dwFontID]->CleanUp();
			SAFE_DELETE(g_FontTable[dwFontID]);
		}
	}

	return S_OK;
}


HRESULT KG3DUI::DrawMultiImageToBuffer(KG3DUIImage2 const* pBuffer, size_t nCount, UINT uWidth, UINT uHeight, 
									   BYTE* pData, size_t uLength, BOOL bUpToDown)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hr = E_FAIL;
	LPDIRECT3DSURFACE9 pSurfaceDst = NULL;
	LPDIRECT3DSURFACE9 pSurfaceSrc = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	D3DLOCKED_RECT rcLock;
    BOOL bLockSurfaceDst = FALSE;
    BOOL bBeginRenderToTexture = FALSE;
    BOOL bBeginScene = FALSE;

	ASSERT(pBuffer);
	ASSERT(uWidth * uHeight * sizeof(DWORD) == uLength);

	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(g_pd3dDevice);
	KG_PROCESS_ERROR(nCount > 0);

	hr = D3DXCreateTexture(g_pd3dDevice, uWidth, uHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = BeginRenderToTexture(pTexture, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

    bBeginRenderToTexture = TRUE;

    hr = BeginScene();
	KGLOG_COM_PROCESS_ERROR(hr);

    bBeginScene = TRUE;

    hr = DrawMultiImage(pBuffer, nCount);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = EndScene();
	KGLOG_COM_PROCESS_ERROR(hr);
    
    bBeginScene = FALSE;

	hr = EndRenderToTexture();
	KGLOG_COM_PROCESS_ERROR(hr);

    bBeginRenderToTexture = FALSE;

	hr = pTexture->GetSurfaceLevel(0, &pSurfaceSrc);
	KGLOG_COM_PROCESS_ERROR(hr);

	ASSERT(pSurfaceSrc);

	hr = g_pd3dDevice->CreateOffscreenPlainSurface(uWidth, uHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurfaceDst, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	ASSERT(pSurfaceDst);

	hr = g_pd3dDevice->GetRenderTargetData(pSurfaceSrc, pSurfaceDst);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = pSurfaceDst->LockRect(&rcLock, NULL, D3DLOCK_READONLY);
	KGLOG_COM_PROCESS_ERROR(hr);
	KGLOG_PROCESS_ERROR(rcLock.pBits);

    bLockSurfaceDst = TRUE;

	if (bUpToDown)
	{
		BYTE *pSrc = (BYTE *)(rcLock.pBits);
		for (DWORD i = 0; i < uHeight; ++i)
		{
			memcpy(pData, pSrc, rcLock.Pitch);

			pData += rcLock.Pitch;
			pSrc += rcLock.Pitch;
		}
	}
	else
	{
		BYTE *pSrc = (BYTE *)(rcLock.pBits);
		pData += uLength;
		for (DWORD i = 0; i < uHeight; ++i)
		{
			pData -= rcLock.Pitch;
			memcpy(pData, pSrc, rcLock.Pitch);
			pSrc += rcLock.Pitch;
		}
	}

	hrResult = S_OK;
Exit0:
    if (bLockSurfaceDst)
    {
	    hr = pSurfaceDst->UnlockRect();
        KGLOG_COM_CHECK_ERROR(hr);

        bLockSurfaceDst = FALSE;
    }

    if (bBeginScene)
    {
        hr = EndScene();
        KGLOG_COM_CHECK_ERROR(hr);

        bBeginScene = FALSE;
    }

    if (bBeginRenderToTexture)
    {
        hr = EndRenderToTexture();
        KGLOG_COM_CHECK_ERROR(hr);

        bBeginRenderToTexture = FALSE;
    }

	SAFE_RELEASE(pSurfaceDst);
	SAFE_RELEASE(pSurfaceSrc);
    SAFE_RELEASE(pTexture);
	return hrResult;
}

HRESULT KG3DUI::SetDeviceCallback(IKG3DeviceCallback* pCallback)
{
    m_pCallback = pCallback;

    return S_OK;
}

HRESULT KG3DUI::LoadFont(DWORD dwFontID, LPCSTR szFilePath, float fPixel, DWORD dwStyle)
{
	HRESULT hr = E_FAIL;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(szFilePath);
	KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));
	KGLOG_PROCESS_ERROR(dwFontID < RS2_MAX_FONT_ITEM_NUM);

	hr = UnloadFont(dwFontID);
	KGLOG_COM_PROCESS_ERROR(hr);

	g_FontTable[dwFontID] = new KG3DFontTexture(g_pFontRenderer);
	KGLOG_PROCESS_ERROR(g_FontTable[dwFontID]);

    bRetCode = g_FontTable[dwFontID]->Initialize(szFilePath, fPixel, dwStyle);
	KGLOG_PROCESS_ERROR(bRetCode);

	return S_OK;
Exit0:
	if (dwFontID < RS2_MAX_FONT_ITEM_NUM)
		SAFE_DELETE(g_FontTable[dwFontID]);
	return E_FAIL;
}

HRESULT KG3DUI::LoadImage(LPCTSTR szFilePath, LPDWORD pdwImageID)
{
	BOOL bRetCode = FALSE;

#ifdef DEBUG
	LPCTSTR pcszFileName = szFilePath;
	while (pcszFileName && *pcszFileName != _T('\0'))
	{
		if (*pcszFileName == _T('/'))
			ASSERT(NULL && "File have unfomat symbols! contact hu changyin!");
		if (*pcszFileName >= _T('A') && *pcszFileName <= _T('Z'))
			ASSERT(NULL && "File have unfomat symbols! contact hu changyin!");
		pcszFileName = ::CharNext(pcszFileName);
	}
#endif

	bRetCode = m_pUITexture->CreateImageFromFile(szFilePath, pdwImageID);
	KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

#if 0
	{	// Test gray
		DWORD dwNewImageID = 0;

		bRetCode = m_pUITexture->DuplicateImage(*pdwImageID, &dwNewImageID);
		KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

		bRetCode = m_pUITexture->GrayImage(dwNewImageID);
		KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

		m_pUITexture->ReleaseImage(*pdwImageID);

		*pdwImageID = dwNewImageID;
	}
#endif

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::CreateImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags)
{
	BOOL bRetCode = FALSE;

	ASSERT(pdwImageID);
	ASSERT(m_pUITexture);

	bRetCode = m_pUITexture->CreateImage(uWidth, uHeight, pdwImageID, dwFlags);
	KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::DuplicateImage(DWORD dwImageID, DWORD dwFlags, LPDWORD pdwNewImageID)
{
	BOOL bRetCode = FALSE;

	ASSERT(pdwNewImageID);
	ASSERT(m_pUITexture);

	bRetCode = m_pUITexture->DuplicateImage(dwImageID, dwFlags, pdwNewImageID);
	KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::GrayImage(DWORD dwImageID, LPDWORD pdwNewImageID)
{
	BOOL bRetCode = FALSE;

	ASSERT(pdwNewImageID);
	ASSERT(m_pUITexture);

	bRetCode = m_pUITexture->DuplicateImage(dwImageID, 0, pdwNewImageID);
	KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

	bRetCode = m_pUITexture->GrayImage(*pdwNewImageID);
	KGLOG_PROCESS_ERROR_CONSOLE(bRetCode);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::MaskImage(DWORD dwImageID, DWORD dwMaskImageID)
{
    struct KMaskImageVertex
	{
		FLOAT x, y, z;
		FLOAT tu,tv;
	};

    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	LPDIRECT3DTEXTURE9 pMaskTexture = NULL;
	KG3DUIImageRect Rect = { 0.0f, 0.0f, 0.0f, 0.0f };
	KG3DUIImageRect MaskRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    KG3DUITextureStageStateGuard TextureStage0StateGuards[6];
	KG3DUIRenderStateGuard RenderStateGuards[11];
    KG3DUISamplerStateGuard Sampler0StateGuards[2];
	KG3DUIViewportGuard ViewportGuard;
	LPDIRECT3DSURFACE9 pSurface = NULL;
	LPDIRECT3DSURFACE9 pSurfaceOld = NULL;
	LPDIRECT3DSURFACE9 pDepthStencilSurface = NULL;
	LPDIRECT3DSURFACE9 pDepthStencilSurfaceOld = NULL;
	D3DVIEWPORT9 Viewport;
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DSURFACE_DESC Desc;
    KMaskImageVertex Vertex[6];
    D3DXVECTOR2 A;
    D3DXVECTOR2 C;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	ChangeDrawState(DRAW_STATE_NONE);

    pTexture = m_pUITexture->GetImageTexture(dwImageID, Rect, NULL);
	KG_PROCESS_ERROR(pTexture);

    pTexture->PreLoad();

	pMaskTexture = m_pUITexture->GetImageTexture(dwMaskImageID, MaskRect, NULL);
	KG_PROCESS_ERROR(pMaskTexture);

    pMaskTexture->PreLoad();

    TextureStage0StateGuards[0].Save(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    TextureStage0StateGuards[1].Save(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    TextureStage0StateGuards[2].Save(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    TextureStage0StateGuards[3].Save(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    TextureStage0StateGuards[4].Save(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    TextureStage0StateGuards[5].Save(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

    RenderStateGuards[0].Save(D3DRS_ALPHABLENDENABLE, TRUE);
	RenderStateGuards[1].Save(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    RenderStateGuards[2].Save(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
    RenderStateGuards[3].Save(D3DRS_DESTBLENDALPHA, D3DBLEND_DESTALPHA);
    RenderStateGuards[4].Save(D3DRS_BLENDOPALPHA, D3DBLENDOP_MIN);
    RenderStateGuards[5].Save(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
	RenderStateGuards[6].Save(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderStateGuards[7].Save(D3DRS_ALPHATESTENABLE, FALSE);
	RenderStateGuards[8].Save(D3DRS_ZENABLE, D3DZB_FALSE);
	RenderStateGuards[9].Save(D3DRS_LIGHTING, FALSE);
	RenderStateGuards[10].Save(D3DRS_FOGENABLE, FALSE);

    Sampler0StateGuards[0].Save(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    Sampler0StateGuards[1].Save(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	hr = pTexture->GetSurfaceLevel(0, &pSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->GetRenderTarget(0, &pSurfaceOld);
	KGLOG_COM_CHECK_ERROR(hr);

	ViewportGuard.Save();

	hr = pTexture->GetLevelDesc(0, &Desc);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetRenderTarget(0, pSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

    ASSERT(pDepthStencilSurface == NULL);

    hr = g_pd3dDevice->CreateDepthStencilSurface(Desc.Width, Desc.Height, g_cGraphicsEngine.GetPresentParameters().AutoDepthStencilFormat, 
        Desc.MultiSampleType, Desc.MultiSampleQuality, FALSE, &pDepthStencilSurface, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthStencilSurfaceOld);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetDepthStencilSurface(pDepthStencilSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    Viewport.X = (DWORD)(Desc.Width * Rect.left);
    Viewport.Y = (DWORD)(Desc.Height * Rect.top);
    Viewport.Width = (DWORD)(Desc.Width * Rect.width);
    Viewport.Height = (DWORD)(Desc.Height * Rect.height);
    Viewport.MinZ = 0.0f;
    Viewport.MaxZ = 1.0f;

    m_ViewportGuard.Set(Viewport);

    hr = g_pd3dDevice->BeginScene();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTexture(0, pMaskTexture);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    A.x = (float)Desc.Width * Rect.left;
    A.y = (float)Desc.Height * Rect.top;
    C.x = (float)Desc.Width * (Rect.left + Rect.width);
    C.y = (float)Desc.Height * (Rect.top + Rect.height);

    Vertex[0].x = C.x; Vertex[0].y = A.y; Vertex[0].z = 0.0f; Vertex[0].tu = 1.0f; Vertex[0].tv = 0.0f;
    Vertex[1].x = A.x; Vertex[1].y = A.y; Vertex[1].z = 0.0f; Vertex[1].tu = 0.0f; Vertex[1].tv = 0.0f;
    Vertex[2].x = A.x; Vertex[2].y = C.y; Vertex[2].z = 0.0f; Vertex[2].tu = 0.0f; Vertex[2].tv = 1.0f;

    Vertex[3].x = C.x; Vertex[3].y = A.y; Vertex[3].z = 0.0f; Vertex[3].tu = 1.0f; Vertex[3].tv = 0.0f;
    Vertex[4].x = A.x; Vertex[4].y = C.y; Vertex[4].z = 0.0f; Vertex[4].tu = 0.0f; Vertex[4].tv = 1.0f;
    Vertex[5].x = C.x; Vertex[5].y = C.y; Vertex[5].z = 0.0f; Vertex[5].tu = 1.0f; Vertex[5].tv = 1.0f;

    hr = g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, Vertex, sizeof(Vertex[0]));
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hrResult = S_OK;
Exit0:
	if (g_pd3dDevice)
    {
        hr = g_pd3dDevice->EndScene();
        KGLOG_COM_CHECK_ERROR(hr);

        if (pDepthStencilSurfaceOld)
        {
            hr = g_pd3dDevice->SetDepthStencilSurface(pDepthStencilSurfaceOld);
            KGLOG_COM_CHECK_ERROR(hr);

            SAFE_RELEASE(pDepthStencilSurfaceOld);
        }
        if (pSurfaceOld)
        {
            hr = g_pd3dDevice->SetRenderTarget(0, pSurfaceOld);
            KGLOG_COM_CHECK_ERROR(hr);

            ViewportGuard.Restore();

            SAFE_RELEASE(pSurfaceOld);
        }

        SAFE_RELEASE(pDepthStencilSurface);
        SAFE_RELEASE(pSurface);
    }
	return hrResult;
}


HRESULT KG3DUI::RegisterSliceImageSize(UINT uWidth, UINT uHeight)
{
	ASSERT(m_pUITexture);

	m_pUITexture->RegisterSliceSize(uWidth, uHeight);

	return S_OK;
}

HRESULT KG3DUI::SetupD3DDeviceCursor()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    if (!g_cGraphicsEngine.GetPresentParameters().Windowed)
    {   
        SetCursor(NULL); // Turn off Windows cursor in full screen mode

        hr = SetD3D9DeviceCursor((HCURSOR)(ULONG_PTR)GetClassLongPtr(m_hWnd, GCLP_HCURSOR));
        KGLOG_COM_PROCESS_ERROR(hr);

        nRetCode = g_pd3dDevice->ShowCursor(TRUE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (!g_cGraphicsEngine.GetPresentParameters().Windowed && IsClipD3D9DeviceCursor())
    {
        RECT rcWindow;
        
        nRetCode = GetWindowRect(m_hWnd, &rcWindow);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = ClipCursor(&rcWindow);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = ClipCursor(NULL);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        nRetCode = ClipCursor(NULL);
        KGLOG_CHECK_ERROR(nRetCode);
    }
    return hrResult;
}

HRESULT KG3DUI::SetD3D9DeviceCursor(HCURSOR hCursor)
{
    return S_OK;
}

HRESULT KG3DUI::SetD3D9DeviceCursorPosition(INT nX, INT nY, DWORD dwFlags)
{
    return S_OK;
}

HRESULT KG3DUI::ShowD3D9DeviceCursor(BOOL bShow)
{
    return S_OK;
}

HRESULT KG3DUI::ClipD3DDeviceCursor(BOOL bClip)
{
    return S_OK;
}

BOOL KG3DUI::IsShowD3D9DeviceCursor() const
{
    return FALSE;
}

BOOL KG3DUI::IsClipD3D9DeviceCursor() const
{
    return FALSE;
}

HRESULT KG3DUI::UnloadFont(DWORD dwFontID)
{
	if (g_FontTable[dwFontID])
	{
		g_FontTable[dwFontID]->CleanUp();
		SAFE_DELETE(g_FontTable[dwFontID]);
	}

	return S_OK;
}

HRESULT KG3DUI::UnloadImage(DWORD dwImageID)
{
	KG_ASSERT_EXIT(m_pUITexture);

	m_pUITexture->ReleaseImage(dwImageID);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DUI::UnloadAllImage()
{
	KG_ASSERT_EXIT(m_pUITexture);

	m_pUITexture->OnResetDevice();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DUI::SaveImage(LPCTSTR szFilePath, DWORD dwImageID)
{
    static struct KFileFormat
    {
        D3DXIMAGE_FILEFORMAT FileFormat;
        LPCTSTR szName;
    } 
    s_FileFormats[] =
    {
        { D3DXIFF_BMP, _T("bmp") },
        { D3DXIFF_JPG, _T("jpg") },
        { D3DXIFF_TGA, _T("tga") },
        { D3DXIFF_PNG, _T("png") },
        { D3DXIFF_DDS, _T("dds") },
        { D3DXIFF_PPM, _T("ppm") },
        { D3DXIFF_DIB, _T("dib") },
        { D3DXIFF_HDR, _T("hdr") },
        { D3DXIFF_PFM, _T("pfm") },
    };

    HRESULT hr = E_FAIL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    KG3DUIImageRect Rect;
    LPCTSTR szDelimiter = NULL;
    TCHAR szFileExtName[8];

	KG_ASSERT_EXIT(m_pUITexture);

    KGLOG_PROCESS_ERROR(szFilePath);
    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

    szDelimiter = _tcsrchr(szFilePath, _T('.'));
    KGLOG_PROCESS_ERROR(szDelimiter);
    KGLOG_PROCESS_ERROR(*(szDelimiter + 1) != _T('\0'));

    _tcsncpy(szFileExtName, szDelimiter + 1, _countof(szFileExtName));

	pTexture = m_pUITexture->GetImageTexture(dwImageID, Rect, NULL);
    KGLOG_PROCESS_ERROR(pTexture);

    for (size_t nIndex = 0; nIndex < _countof(s_FileFormats); ++nIndex)
    {
        if (_tcsicmp(szFileExtName, s_FileFormats[nIndex].szName) == 0)
        {
            hr = D3DXSaveTextureToFile(szFilePath, s_FileFormats[nIndex].FileFormat, pTexture, NULL);
            KGLOG_COM_PROCESS_ERROR(hr);

        	return S_OK;
        }
    }

Exit0:
    KGLogPrintf(KGLOG_ERR, "KG3DUI::SaveImage(%s, %u) only support bmp,jpg,tga,png,dds,ppm,dib,hdr,pfm\n", szFilePath, dwImageID);
	return hr;
}

HRESULT KG3DUI::CreateScene(IKG3DScene** pp3DScene)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DUIMiniScene* p3DUIMiniScene = NULL;
    int nSceneInitFlag = false;

	ASSERT(pp3DScene);
	ASSERT(*pp3DScene == NULL);

	p3DUIMiniScene = new KG3DUIMiniScene;
	KGLOG_PROCESS_ERROR_CONSOLE(p3DUIMiniScene);

    hrRetCode = p3DUIMiniScene->Init();
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hrRetCode);

    nSceneInitFlag = true;

    m_MiniScenePtrList.push_back(p3DUIMiniScene);

	*pp3DScene = p3DUIMiniScene;
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nSceneInitFlag)
        {
            p3DUIMiniScene->UnInit();
            nSceneInitFlag = false;
        }
        KG_DELETE(p3DUIMiniScene);
    }

	return hrResult;
}


HRESULT KG3DUI::UnloadScene(IKG3DScene* p3DScene)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KG3DUIMiniScene *p3DUIMiniScene = NULL;

	ASSERT(p3DScene);

    p3DUIMiniScene = (KG3DUIMiniScene *)p3DScene;

    m_MiniScenePtrList.remove(p3DUIMiniScene);

    hr = p3DUIMiniScene->UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    KG_DELETE(p3DUIMiniScene);
	return hrResult;
}


HRESULT KG3DUI::DrawUserdataToImage(DWORD dwImageID, KG3DUIDrawUserdata DrawUserdata,void* pUserData)
{
	KG_ASSERT_EXIT(m_pUITexture);

	m_pUITexture->DrawUserdataToImage(dwImageID, DrawUserdata, pUserData);

Exit0:
	return S_OK;
}


HRESULT KG3DUI::DrawOleToImage(LPUNKNOWN pUnknown, DWORD dwImageID)
{
    HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	HDC hdc = NULL;
	KG3DUIImageRect ImageRect = { 0.0f, 0.0f, 0.0f, 0.0f };
	KG3DUIImageInfo Info;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	LPDIRECT3DSURFACE9 pSurface = NULL;
    int nMode = 0;
    RECT rect;
    BOOL bSetMapMode = FALSE;

	KG_ASSERT_EXIT(m_pUITexture);

	m_pUITexture->GetImageInfo(dwImageID, Info);

	KGLOG_PROCESS_ERROR(pUnknown);

	pTexture = m_pUITexture->GetImageTexture(dwImageID, ImageRect, NULL);
	KGLOG_PROCESS_ERROR(pTexture);

	pTexture->GetSurfaceLevel(0, &pSurface);
	KGLOG_PROCESS_ERROR(pSurface);

	hRetCode = pSurface->GetDC(&hdc);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    nMode = ::SetMapMode(hdc, MM_TEXT);

    bSetMapMode = TRUE;

    rect.left = (LONG)Info.uLeft;
    rect.right = (LONG)(Info.uLeft + Info.uWidth);
    rect.top = (LONG)Info.uTop;
    rect.bottom = (LONG)(Info.uTop + Info.uHeight);

    hRetCode = ::OleDraw(pUnknown, DVASPECT_CONTENT, hdc, &rect);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    if (hdc)
    {
        if (bSetMapMode)
            ::SetMapMode(hdc, nMode);

        pSurface->ReleaseDC(hdc);
    }

	SAFE_RELEASE(pSurface);

	return hResult;
}


HRESULT KG3DUI::GetImageData(DWORD dwImageID, UINT uWidth, UINT uHeight, BYTE* pData, UINT uLength, BOOL bUpToDown) const
{
	BOOL bRetCode = FALSE;

	KG_ASSERT_EXIT(pData);
	KG_ASSERT_EXIT(m_pUITexture);

	bRetCode = m_pUITexture->GetImageData(dwImageID, uWidth, uHeight, pData, uLength, bUpToDown);
	KGLOG_PROCESS_ERROR(bRetCode);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::GetImageSize(DWORD dwImageID, UINT* puWidth, UINT* puHeight) const
{
	KG3DUIImageInfo Info;

	KG_ASSERT_EXIT(puWidth);
	KG_ASSERT_EXIT(puHeight);
	KG_ASSERT_EXIT(m_pUITexture);

	m_pUITexture->GetImageInfo(dwImageID, Info);

	*puWidth = Info.uWidth;
	*puHeight = Info.uHeight;

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::FillMultiRectPartTypeTimer(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, D3DCOLOR diffuse, float fRatio, K2DDiffuseVertex* pVertex, int& nTriCount)
{
	ASSERT(pVertex);

	D3DXVECTOR2 const& A = v2Min;
	D3DXVECTOR2 const& C = v2Max;
	D3DXVECTOR2 const E = (A + C) / 2.0f;	// Center

	pVertex[0].x = E.x; pVertex[0].y = A.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse;
	pVertex[2].x = E.x; pVertex[2].y = E.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse;

	pVertex[3].x = A.x; pVertex[3].y = A.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse;
	pVertex[4].x = A.x; pVertex[4].y = C.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse;
	pVertex[5].x = E.x; pVertex[5].y = E.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse;

	pVertex[6].x = A.x; pVertex[6].y = C.y; pVertex[6].z = 0.0f; pVertex[6].color = diffuse;
	pVertex[7].x = C.x; pVertex[7].y = C.y; pVertex[7].z = 0.0f; pVertex[7].color = diffuse;
	pVertex[8].x = E.x; pVertex[8].y = E.y; pVertex[8].z = 0.0f; pVertex[8].color = diffuse;

	pVertex[ 9].x = C.x; pVertex[ 9].y = C.y; pVertex[ 9].z = 0.0f; pVertex[ 9].color = diffuse;
	pVertex[10].x = C.x; pVertex[10].y = A.y; pVertex[10].z = 0.0f; pVertex[10].color = diffuse;
	pVertex[11].x = E.x; pVertex[11].y = E.y; pVertex[11].z = 0.0f; pVertex[11].color = diffuse;

	pVertex[12].x = C.x; pVertex[12].y = A.y; pVertex[12].z = 0.0f; pVertex[12].color = diffuse;
	pVertex[13].x = E.x; pVertex[13].y = A.y; pVertex[13].z = 0.0f; pVertex[13].color = diffuse;
	pVertex[14].x = E.x; pVertex[14].y = E.y; pVertex[14].z = 0.0f; pVertex[14].color = diffuse;

	int const nOctant = static_cast<int>((fRatio + 0.125f) * 4.0f) % 4;

	float const fAngle = fRatio * D3DX_PI * 2.0f;
	float const fSemidiameter = (E.y - A.y) * (4.0f / D3DX_PI);
	D3DXVECTOR2 const F(sinf(fAngle) * fSemidiameter, cosf(fAngle) * fSemidiameter);	// cross point

	float aLength[4];
	aLength[0] = E.y - A.y;
	aLength[1] = C.x - E.x;
	aLength[2] = A.y - E.y;
	aLength[3] = E.x - C.x;

	D3DXVECTOR2 G;

	switch (nOctant)
	{
	case 0:	/* fall through */
	case 2:
		G.y = aLength[nOctant];
		G.x = F.x * aLength[nOctant] / F.y;
		break;
	case 1:	/* fall through */
	case 3:
		G.x = aLength[nOctant];
		G.y = F.y * aLength[nOctant] / F.x;
		break;
	};

	nTriCount = fRatio >= 0.875f ? 1 : 5 - nOctant;

	// The first edge of triangle, the second vertex
	pVertex[nTriCount * 3 - 2].x = E.x + G.x;
	pVertex[nTriCount * 3 - 2].y = E.y - G.y;

	return S_OK;
}


HRESULT KG3DUI::FillMultiRectPartTypeXMinToMax(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, D3DCOLOR diffuse, float fRatio, K2DDiffuseVertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const& A = v2Min;
	D3DXVECTOR2 const C = D3DXVECTOR2(v2Min.x + (v2Max.x - v2Min.x) * fRatio, v2Max.y);
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse;

	nTriCount = 2;

	return S_OK;
}


HRESULT KG3DUI::FillMultiRectPartTypeXMaxToMin(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, D3DCOLOR diffuse, float fRatio, K2DDiffuseVertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const A = D3DXVECTOR2(v2Max.x - (v2Max.x - v2Min.x) * fRatio, v2Min.y);
	D3DXVECTOR2 const& C = v2Max;
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse;

	nTriCount = 2;

	return S_OK;
}


HRESULT KG3DUI::FillMultiRectPartTypeYMinToMax(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, D3DCOLOR diffuse, float fRatio, K2DDiffuseVertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const& A = v2Min;
	D3DXVECTOR2 const C = D3DXVECTOR2(v2Min.x, v2Min.y + (v2Max.y - v2Min.y) * fRatio);
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse;

	nTriCount = 2;

	return S_OK;
}


HRESULT KG3DUI::FillMultiRectPartTypeYMaxToMin(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, D3DCOLOR diffuse, float fRatio, K2DDiffuseVertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const A =  D3DXVECTOR2(v2Min.x, v2Max.y - (v2Max.y - v2Min.y) * fRatio);
	D3DXVECTOR2 const& C = v2Max;
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse;

	nTriCount = 2;

	return S_OK;
}


HRESULT KG3DUI::FillTriangleFan(KG3DUIVertex2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	K2DDiffuseVertex* pVer = NULL;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 3);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	ChangeDrawState(DRAW_STATE_FILL);

	if (m_DiffuseVertex.Lock((UINT)nCount, (void**)&pVer))
	{
		ASSERT((UINT)nCount == m_DiffuseVertex.GetLockSize());

		m_nDiffuseVertexLocked = m_DiffuseVertex.GetLockStart();

		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			pVer[nIndex].x = pBuffer[nIndex].v2Vertex.x; 
			pVer[nIndex].y = pBuffer[nIndex].v2Vertex.y;
			pVer[nIndex].z = 0.0f; 
			pVer[nIndex].color = pBuffer[nIndex].crDiffuse;
		}

		if (m_DiffuseVertex.Unlock())
		{
			hr = m_DiffuseVertex.DrawPrimitive(D3DPT_TRIANGLEFAN, m_nDiffuseVertexLocked, (UINT)nCount - 2);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::DrawMultiLine(KG3DUILine2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	K2DDiffuseVertex * pVer = NULL;
	UINT const uLockSize = static_cast<UINT>(2 * nCount);

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	ChangeDrawState(DRAW_STATE_LINE);

	if (m_DiffuseVertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_DiffuseVertex.GetLockSize());
		m_nDiffuseVertexLocked = m_DiffuseVertex.GetLockStart();

		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			D3DXVECTOR2 const& v2Min = pBuffer->v2Min;
			D3DXVECTOR2 const& v2Max = pBuffer->v2Max;

			pVer[0].x = v2Min.x; pVer[0].y = v2Min.y; pVer[0].z = 0.0f; pVer[0].color = pBuffer->crMin;
			pVer[1].x = v2Max.x; pVer[1].y = v2Max.y; pVer[1].z = 0.0f; pVer[1].color = pBuffer->crMax;

			++pBuffer;
			pVer += 2;
		}

		if (m_DiffuseVertex.Unlock())
		{
			hr = m_DiffuseVertex.DrawPrimitive(D3DPT_LINELIST, m_nDiffuseVertexLocked, static_cast<UINT>(nCount));
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}

HRESULT KG3DUI::DrawMultiRect(KG3DUIRect2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	K2DDiffuseVertex * pVer = NULL;
	UINT const uLockSize = (UINT)(2 * 4 * nCount);

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	ChangeDrawState(DRAW_STATE_LINE);

	if ( m_DiffuseVertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_DiffuseVertex.GetLockSize());
		m_nDiffuseVertexLocked = m_DiffuseVertex.GetLockStart();

		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			D3DXVECTOR2 const& v2Min = pBuffer->v2Min;
			D3DXVECTOR2 const& v2Max = pBuffer->v2Max;

			pVer[0].x = v2Min.x; pVer[0].y = v2Min.y; pVer[0].z = 0.0f; pVer[0].color = pBuffer->crDiffuse;
			pVer[1].x = v2Min.x; pVer[1].y = v2Max.y; pVer[1].z = 0.0f; pVer[1].color = pBuffer->crDiffuse;

			pVer[2].x = v2Min.x; pVer[2].y = v2Min.y; pVer[2].z = 0.0f; pVer[2].color = pBuffer->crDiffuse;
			pVer[3].x = v2Max.x; pVer[3].y = v2Min.y; pVer[3].z = 0.0f; pVer[3].color = pBuffer->crDiffuse;

			pVer[4].x = v2Min.x; pVer[4].y = v2Max.y; pVer[4].z = 0.0f; pVer[4].color = pBuffer->crDiffuse;
			pVer[5].x = v2Max.x; pVer[5].y = v2Max.y; pVer[5].z = 0.0f; pVer[5].color = pBuffer->crDiffuse;

			pVer[6].x = v2Max.x; pVer[6].y = v2Min.y; pVer[6].z = 0.0f; pVer[6].color = pBuffer->crDiffuse;
			pVer[7].x = v2Max.x; pVer[7].y = v2Max.y; pVer[7].z = 0.0f; pVer[7].color = pBuffer->crDiffuse;

			++pBuffer;
			pVer += 8;
		}

		if (m_DiffuseVertex.Unlock())
		{
			hr = m_DiffuseVertex.DrawPrimitive(D3DPT_LINELIST, m_nDiffuseVertexLocked, 4 * (UINT)nCount);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}

HRESULT KG3DUI::ScreenShot(const char szFilePath[], D3DXIMAGE_FILEFORMAT eType, int nQuality)
{
    return g_cGraphicsEngine.ScreenShot(szFilePath, eType, nQuality);
}

HRESULT KG3DUI::LoadText(KG3DUIText2 const& text)
{
	KGLOG_PROCESS_ERROR(g_pFontRenderer);

	ASSERT(text.dwFontID < RS2_MAX_FONT_ITEM_NUM );

	KG3DFontTexture* pFont = g_FontTable[text.dwFontID];
	if (pFont)
	{
		DWORD dwEffect = 0;

		ASSERT(text.nTextLength == -1 || text.nTextLength >= 0);
		ASSERT(text.wszText[0] != L'\0');
		ASSERT(text.dwFontID < RS2_MAX_FONT_ITEM_NUM);

		if (text.byBorder)
		{
			pFont->SetTextBorder(text.crBorder, text.byBorder);
			dwEffect |= KG3DUI_TEXT_STYLE_BORDER;
		}
		if (text.byShadow)
		{
			pFont->SetTextShadow(text.crShadow, text.byShadow);
			dwEffect |= KG3DUI_TEXT_STYLE_SHADOW;
		}

		pFont->SetTextScaling(text.fScale, text.fScale);	

		pFont->SetTextEffect(dwEffect);			

		pFont->LoadText(text.wszText);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DUI::DrawMultiText(KG3DUIText2 const* pBuffer, size_t nCount)
{
	KGLOG_PROCESS_ERROR(g_pFontRenderer);
	KGLOG_PROCESS_ERROR(pBuffer);

	ChangeDrawState(DRAW_STATE_TEXT);

	g_pFontRenderer->Prepare();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		KG3DUIText2 const& Text = pBuffer[nIndex];

		ASSERT(Text.dwFontID < RS2_MAX_FONT_ITEM_NUM );

		KG3DFontTexture* pFont = g_FontTable[Text.dwFontID];
		if (pFont)
        {
            DWORD dwEffect = 0;

            ASSERT(Text.nTextLength == -1 || Text.nTextLength >= 0);
            ASSERT(Text.wszText[0] != L'\0');
            ASSERT(Text.dwFontID < RS2_MAX_FONT_ITEM_NUM);

            if (Text.byBorder)
            {
                pFont->SetTextBorder(Text.crBorder, Text.byBorder);
                dwEffect |= KG3DUI_TEXT_STYLE_BORDER;
            }
            if (Text.byShadow)
            {
                pFont->SetTextShadow(Text.crShadow, Text.byShadow);
                dwEffect |= KG3DUI_TEXT_STYLE_SHADOW;
            }

            pFont->SetTextColor(Text.crFont);
            pFont->SetTextAlpha(Text.byAlpha);
            pFont->SetTextScaling(Text.fScale, Text.fScale);	

            pFont->SetTextEffect(dwEffect);			

            pFont->Draw2DText(Text.v2Min.x, Text.v2Min.y, Text.wszText, Text.fSpacing);
        }
	}

	g_pFontRenderer->Flush();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DUI::FillMultiRect(KG3DUIRect2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	K2DDiffuseVertex* pVer = NULL;
	UINT const uLockSize = static_cast<UINT>(6 * nCount);

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	ChangeDrawState(DRAW_STATE_FILL);

	if (m_DiffuseVertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_DiffuseVertex.GetLockSize());

		m_nDiffuseVertexLocked = m_DiffuseVertex.GetLockStart();

		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			D3DCOLOR const diffuse = pBuffer->crDiffuse;;
			D3DXVECTOR2 const& A(pBuffer->v2Min);
			D3DXVECTOR2 const& C(pBuffer->v2Max);
			D3DXVECTOR2 const B(C.x, A.y);
			D3DXVECTOR2 const D(A.x, C.y);

			pVer[0].x = B.x; pVer[0].y = B.y; pVer[0].z = 0.0f; pVer[0].color = diffuse;
			pVer[1].x = A.x; pVer[1].y = A.y; pVer[1].z = 0.0f; pVer[1].color = diffuse;
			pVer[2].x = D.x; pVer[2].y = D.y; pVer[2].z = 0.0f; pVer[2].color = diffuse;

			pVer[3].x = B.x; pVer[3].y = B.y; pVer[3].z = 0.0f; pVer[3].color = diffuse;
			pVer[4].x = D.x; pVer[4].y = D.y; pVer[4].z = 0.0f; pVer[4].color = diffuse;
			pVer[5].x = C.x; pVer[5].y = C.y; pVer[5].z = 0.0f; pVer[5].color = diffuse;

			++pBuffer;
			pVer += 6;
		}

		if (m_DiffuseVertex.Unlock())
		{
			hr = m_DiffuseVertex.DrawPrimitive(D3DPT_TRIANGLELIST, m_nDiffuseVertexLocked, static_cast<UINT>(2 * nCount));
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}

HRESULT KG3DUI::DrawMultiImageImpl(KG3DUIImage2 const* pBuffer, size_t nCount, size_t nOffset, DWORD dwImageID)
{
	HRESULT hr = E_FAIL;
	K2DTex1Vertex* pVer = NULL;
	BYTE const* pByteBuffer = NULL;
	//int nTriTotalCount = 0;
	UINT const uLockSize = static_cast<UINT>(6 * nCount);
	KG3DUIImageRect Rect = { 0.0f, 0.0f, 0.0f, 0.0f };
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	// flush texture
	pTexture = m_pUITexture->GetImageTexture(dwImageID, Rect, NULL);
	KG_PROCESS_ERROR(pTexture);

	if (m_Tex1Vertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_Tex1Vertex.GetLockSize());
		m_nTex1VertexLocked = m_Tex1Vertex.GetLockStart();

		pByteBuffer = reinterpret_cast<BYTE const*>(pBuffer);
		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			KG3DUIImage2 const* pValue = reinterpret_cast<KG3DUIImage2 const*>(pByteBuffer);
			D3DXVECTOR2 const& A = pValue->v2Min;
			D3DXVECTOR2 const& C = pValue->v2Max;
			D3DXVECTOR2 const B(C.x, A.y);
			D3DXVECTOR2 const D(A.x, C.y);
			D3DXCOLOR const diffuse = pValue->crDiffuse;

			float const UA = pValue->v2UVMin.x * Rect.width + Rect.left;
			float const UC = pValue->v2UVMax.x * Rect.width + Rect.left;
			float const VA = pValue->v2UVMin.y * Rect.height + Rect.top;
			float const VC = pValue->v2UVMax.y * Rect.height + Rect.top;

			pVer[0].x = B.x; pVer[0].y = B.y; pVer[0].z = 0.0f; pVer[0].color = diffuse; pVer[0].tu = UC; pVer[0].tv = VA;
			pVer[1].x = A.x; pVer[1].y = A.y; pVer[1].z = 0.0f; pVer[1].color = diffuse; pVer[1].tu = UA; pVer[1].tv = VA;
			pVer[2].x = D.x; pVer[2].y = D.y; pVer[2].z = 0.0f; pVer[2].color = diffuse; pVer[2].tu = UA; pVer[2].tv = VC;

			pVer[3].x = B.x; pVer[3].y = B.y; pVer[3].z = 0.0f; pVer[3].color = diffuse; pVer[3].tu = UC; pVer[3].tv = VA;
			pVer[4].x = D.x; pVer[4].y = D.y; pVer[4].z = 0.0f; pVer[4].color = diffuse; pVer[4].tu = UA; pVer[4].tv = VC;
			pVer[5].x = C.x; pVer[5].y = C.y; pVer[5].z = 0.0f; pVer[5].color = diffuse; pVer[5].tu = UC; pVer[5].tv = VC;

			pByteBuffer += nOffset;
			pVer += 6;
		}

		if (m_Tex1Vertex.Unlock())
		{
#ifdef ENABLE_KG3DUI_EFFECT
			if (m_pEffect)
			{
				UINT uPasses;
				m_pEffect->Begin(&uPasses, 0);
				for (UINT uPass = 0; uPass < uPasses; ++uPass)
				{
					m_pEffect->BeginPass(uPass);

					hr = m_Tex1Vertex.DrawPrimitive(D3DPT_TRIANGLELIST, pTexture, m_nTex1VertexLocked, static_cast<UINT>(2 * nCount));
					KGLOG_COM_CHECK_ERROR(hr);

					m_pEffect->EndPass();
				}

				m_pEffect->End();
			}
			else
#endif
			{
				hr = m_Tex1Vertex.DrawPrimitive(D3DPT_TRIANGLELIST, pTexture, m_nTex1VertexLocked, static_cast<UINT>(2 * nCount));
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
			}
		}
	}

	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::FillMultiImagePartTypeTimer(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, 
											D3DXVECTOR2 const& v2UVMin, D3DXVECTOR2 const& v2UVMax, 
											D3DXCOLOR crDiffuse, float fRatio, K2DTex1Vertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const& A = v2Min;
	D3DXVECTOR2 const& C = v2Max;
	D3DXVECTOR2 const& UVA = v2UVMin;
	D3DXVECTOR2 const& UVC = v2UVMax;
	D3DXVECTOR2 const E = (A + C) / 2.0f;		// Center
	D3DXVECTOR2 const UVE = (UVA + UVC) / 2.0f;	// Texture center
	D3DXCOLOR const diffuse = crDiffuse;

	ASSERT(pVertex);

	pVertex[ 0].x = E.x; pVertex[ 0].y = A.y; pVertex[ 0].z = 0.0f; pVertex[ 0].color = diffuse; pVertex[ 0].tu = UVE.x; pVertex[ 0].tv = UVA.y;
	pVertex[ 1].x = A.x; pVertex[ 1].y = A.y; pVertex[ 1].z = 0.0f; pVertex[ 1].color = diffuse; pVertex[ 1].tu = UVA.x; pVertex[ 1].tv = UVA.y;
	pVertex[ 2].x = E.x; pVertex[ 2].y = E.y; pVertex[ 2].z = 0.0f; pVertex[ 2].color = diffuse; pVertex[ 2].tu = UVE.x; pVertex[ 2].tv = UVE.y;

	pVertex[ 3].x = A.x; pVertex[ 3].y = A.y; pVertex[ 3].z = 0.0f; pVertex[ 3].color = diffuse; pVertex[ 3].tu = UVA.x; pVertex[ 3].tv = UVA.y;
	pVertex[ 4].x = A.x; pVertex[ 4].y = C.y; pVertex[ 4].z = 0.0f; pVertex[ 4].color = diffuse; pVertex[ 4].tu = UVA.x; pVertex[ 4].tv = UVC.y;
	pVertex[ 5].x = E.x; pVertex[ 5].y = E.y; pVertex[ 5].z = 0.0f; pVertex[ 5].color = diffuse; pVertex[ 5].tu = UVE.x; pVertex[ 5].tv = UVE.y;

	pVertex[ 6].x = A.x; pVertex[ 6].y = C.y; pVertex[ 6].z = 0.0f; pVertex[ 6].color = diffuse; pVertex[ 6].tu = UVA.x; pVertex[ 6].tv = UVC.y;
	pVertex[ 7].x = C.x; pVertex[ 7].y = C.y; pVertex[ 7].z = 0.0f; pVertex[ 7].color = diffuse; pVertex[ 7].tu = UVC.x; pVertex[ 7].tv = UVC.y;
	pVertex[ 8].x = E.x; pVertex[ 8].y = E.y; pVertex[ 8].z = 0.0f; pVertex[ 8].color = diffuse; pVertex[ 8].tu = UVE.x; pVertex[ 8].tv = UVE.y;

	pVertex[ 9].x = C.x; pVertex[ 9].y = C.y; pVertex[ 9].z = 0.0f; pVertex[ 9].color = diffuse; pVertex[ 9].tu = UVC.x; pVertex[ 9].tv = UVC.y;
	pVertex[10].x = C.x; pVertex[10].y = A.y; pVertex[10].z = 0.0f; pVertex[10].color = diffuse; pVertex[10].tu = UVC.x; pVertex[10].tv = UVA.y;
	pVertex[11].x = E.x; pVertex[11].y = E.y; pVertex[11].z = 0.0f; pVertex[11].color = diffuse; pVertex[11].tu = UVE.x; pVertex[11].tv = UVE.y;

	pVertex[12].x = C.x; pVertex[12].y = A.y; pVertex[12].z = 0.0f; pVertex[12].color = diffuse; pVertex[12].tu = UVC.x; pVertex[12].tv = UVA.y;
	pVertex[13].x = E.x; pVertex[13].y = A.y; pVertex[13].z = 0.0f; pVertex[13].color = diffuse; pVertex[13].tu = UVE.x; pVertex[13].tv = UVA.y;
	pVertex[14].x = E.x; pVertex[14].y = E.y; pVertex[14].z = 0.0f; pVertex[14].color = diffuse; pVertex[14].tu = UVE.x; pVertex[14].tv = UVE.y;

	int const nOctant = static_cast<int>((fRatio + 0.125f) * 4.0f) % 4;

	float const fAngle = fRatio * D3DX_PI * 2.0f;
	float const fSemidiameter = (E.y - A.y) * (4.0f / D3DX_PI);
	D3DXVECTOR2 const F(sinf(fAngle) * fSemidiameter, cosf(fAngle) * fSemidiameter);	// cross point

	float aLength[4];
	aLength[0] = E.y - A.y;
	aLength[1] = C.x - E.x;
	aLength[2] = A.y - E.y;
	aLength[3] = E.x - C.x;

	D3DXVECTOR2 G;

	switch (nOctant)
	{
	case 0:	/* fall through */
	case 2:
		G.y = aLength[nOctant];
		G.x = F.x * aLength[nOctant] / F.y;
		break;
	case 1:	/* fall through */
	case 3:
		G.x = aLength[nOctant];
		G.y = F.y * aLength[nOctant] / F.x;
		break;
	};

	D3DXVECTOR2 const UVG(G.x / (C.x - E.x) * (UVC.x - UVE.x), G.y / (C.y - E.y) * (UVC.y - UVE.y));

	nTriCount = fRatio >= 0.875f ? 1 : 5 - nOctant;

	// The first edge of triangle, the second vertex
	pVertex[nTriCount * 3 - 2].x = E.x + G.x;
	pVertex[nTriCount * 3 - 2].y = E.y - G.y;
	pVertex[nTriCount * 3 - 2].tu = UVE.x + UVG.x;
	pVertex[nTriCount * 3 - 2].tv = UVE.y - UVG.y;

#if 0
	ATLTRACE("[%f, %f]\n", pVertex[nTriCount * 3 - 2].tu, pVertex[nTriCount * 3 - 2].tv);
#endif

	return S_OK;
}


HRESULT KG3DUI::FillMultiImagePartTypeXMinToMax(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, 
												D3DXVECTOR2 const& v2UVMin, D3DXVECTOR2 const& v2UVMax, 
												D3DXCOLOR crDiffuse, float fRatio, K2DTex1Vertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const& A = v2Min;
	D3DXVECTOR2 const C = D3DXVECTOR2(v2Min.x + (v2Max.x - v2Min.x) * fRatio, v2Max.y);
	D3DXVECTOR2 const& UVA = v2UVMin;
	D3DXVECTOR2 const UVC = D3DXVECTOR2(v2UVMin.x + (v2UVMax.x - v2UVMin.x) * fRatio, v2UVMax.y);
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);
	D3DXCOLOR const diffuse = crDiffuse;

	ASSERT(pVertex);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse; pVertex[0].tu = UVC.x; pVertex[0].tv = UVA.y;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse; pVertex[1].tu = UVA.x; pVertex[1].tv = UVA.y;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse; pVertex[2].tu = UVA.x; pVertex[2].tv = UVC.y;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse; pVertex[3].tu = UVC.x; pVertex[3].tv = UVA.y;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse; pVertex[4].tu = UVA.x; pVertex[4].tv = UVC.y;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse; pVertex[5].tu = UVC.x; pVertex[5].tv = UVC.y;

	return S_OK;
}


HRESULT KG3DUI::FillMultiImagePartTypeXMaxToMin(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, 
												D3DXVECTOR2 const& v2UVMin, D3DXVECTOR2 const& v2UVMax, 
												D3DXCOLOR crDiffuse, float fRatio, K2DTex1Vertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const A = D3DXVECTOR2(v2Max.x - (v2Max.x - v2Min.x) * fRatio, v2Min.y);
	D3DXVECTOR2 const& C = v2Max;
	D3DXVECTOR2 const UVA = D3DXVECTOR2(v2UVMax.x - (v2UVMax.x - v2UVMin.x) * fRatio, v2UVMin.y);
	D3DXVECTOR2 const& UVC = v2UVMax;
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);
	D3DXCOLOR const diffuse = crDiffuse;

	ASSERT(pVertex);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse; pVertex[0].tu = UVC.x; pVertex[0].tv = UVA.y;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse; pVertex[1].tu = UVA.x; pVertex[1].tv = UVA.y;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse; pVertex[2].tu = UVA.x; pVertex[2].tv = UVC.y;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse; pVertex[3].tu = UVC.x; pVertex[3].tv = UVA.y;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse; pVertex[4].tu = UVA.x; pVertex[4].tv = UVC.y;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse; pVertex[5].tu = UVC.x; pVertex[5].tv = UVC.y;

	return S_OK;
}


HRESULT KG3DUI::FillMultiImagePartTypeYMinToMax(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, 
												D3DXVECTOR2 const& v2UVMin, D3DXVECTOR2 const& v2UVMax, 
												D3DXCOLOR crDiffuse, float fRatio, K2DTex1Vertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const& A = v2Min;
	D3DXVECTOR2 const C = D3DXVECTOR2(v2Min.x, v2Min.y + (v2Max.y - v2Min.y) * fRatio);
	D3DXVECTOR2 const& UVA = v2UVMin;
	D3DXVECTOR2 const UVC = D3DXVECTOR2(v2UVMin.x, v2UVMin.y + (v2UVMax.y - v2UVMin.y) * fRatio);
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);
	D3DXCOLOR const diffuse = crDiffuse;

	ASSERT(pVertex);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse; pVertex[0].tu = UVC.x; pVertex[0].tv = UVA.y;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse; pVertex[1].tu = UVA.x; pVertex[1].tv = UVA.y;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse; pVertex[2].tu = UVA.x; pVertex[2].tv = UVC.y;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse; pVertex[3].tu = UVC.x; pVertex[3].tv = UVA.y;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse; pVertex[4].tu = UVA.x; pVertex[4].tv = UVC.y;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse; pVertex[5].tu = UVC.x; pVertex[5].tv = UVC.y;

	return S_OK;
}


HRESULT KG3DUI::FillMultiImagePartTypeYMaxToMin(D3DXVECTOR2 const& v2Min, D3DXVECTOR2 const& v2Max, 
												D3DXVECTOR2 const& v2UVMin, D3DXVECTOR2 const& v2UVMax, 
												D3DXCOLOR crDiffuse, float fRatio, K2DTex1Vertex* pVertex, int& nTriCount)
{
	D3DXVECTOR2 const A = D3DXVECTOR2(v2Min.x, v2Max.y - (v2Max.y - v2Min.y) * fRatio);
	D3DXVECTOR2 const& C = v2Max;
	D3DXVECTOR2 const UVA = D3DXVECTOR2(v2UVMin.x, v2UVMax.y - (v2UVMax.y - v2UVMin.y) * fRatio);
	D3DXVECTOR2 const& UVC = v2UVMax;
	D3DXVECTOR2 const B(C.x, A.y);
	D3DXVECTOR2 const D(A.x, C.y);
	D3DXCOLOR const diffuse = crDiffuse;

	ASSERT(pVertex);

	pVertex[0].x = B.x; pVertex[0].y = B.y; pVertex[0].z = 0.0f; pVertex[0].color = diffuse; pVertex[0].tu = UVC.x; pVertex[0].tv = UVA.y;
	pVertex[1].x = A.x; pVertex[1].y = A.y; pVertex[1].z = 0.0f; pVertex[1].color = diffuse; pVertex[1].tu = UVA.x; pVertex[1].tv = UVA.y;
	pVertex[2].x = D.x; pVertex[2].y = D.y; pVertex[2].z = 0.0f; pVertex[2].color = diffuse; pVertex[2].tu = UVA.x; pVertex[2].tv = UVC.y;

	pVertex[3].x = B.x; pVertex[3].y = B.y; pVertex[3].z = 0.0f; pVertex[3].color = diffuse; pVertex[3].tu = UVC.x; pVertex[3].tv = UVA.y;
	pVertex[4].x = D.x; pVertex[4].y = D.y; pVertex[4].z = 0.0f; pVertex[4].color = diffuse; pVertex[4].tu = UVA.x; pVertex[4].tv = UVC.y;
	pVertex[5].x = C.x; pVertex[5].y = C.y; pVertex[5].z = 0.0f; pVertex[5].color = diffuse; pVertex[5].tu = UVC.x; pVertex[5].tv = UVC.y;

	return S_OK;
}


HRESULT KG3DUI::DrawMultiImagePartImpl(KG3DUIImagePart2 const* pBuffer, size_t nCount, size_t nOffset, DWORD dwImageID)
{
	HRESULT hr = E_FAIL;
	K2DTex1Vertex* pVer = NULL;
	BYTE const* pByteBuffer = NULL;
	int nTriTotalCount = 0;
	UINT const uLockSize = static_cast<UINT>(3 * 5 * nCount);
	KG3DUIImageRect Rect = { 0.0f, 0.0f, 0.0f, 0.0f };
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(nOffset > 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	// flush texture
	pTexture = m_pUITexture->GetImageTexture(dwImageID, Rect, NULL);
	KG_PROCESS_ERROR(pTexture != NULL);

    if (m_Tex1Vertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_Tex1Vertex.GetLockSize());
		m_nTex1VertexLocked = m_Tex1Vertex.GetLockStart();

		pByteBuffer = reinterpret_cast<BYTE const*>(pBuffer);
		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			KG3DUIImagePart2 const* pConValue = reinterpret_cast<KG3DUIImagePart2 const*>(pByteBuffer);
			KG3DUIImagePart2* pValue = const_cast<KG3DUIImagePart2*>(pConValue);
			float const	fRatio = max(min(pValue->fRatio, 1.0f), 0.0f);
			int nTriCount = 0;

			if (pValue->v2Max.x - pValue->v2Min.x == 0.0f || pValue->v2Max.y - pValue->v2Min.y == 0.0f)
				continue;

			D3DXVECTOR2 vMin(pValue->v2UVMin.x * Rect.width + Rect.left, pValue->v2UVMin.y * Rect.height + Rect.top);
			D3DXVECTOR2 vMax(pValue->v2UVMax.x * Rect.width + Rect.left, pValue->v2UVMax.y * Rect.height + Rect.top);

			switch (pValue->dwType)
			{
			case KG3DUI_PART_TYPE_TIMER:
				FillMultiImagePartTypeTimer(pValue->v2Min, pValue->v2Max, vMin, vMax, pValue->crDiffuse, fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_X_MIN_TO_MAX:
				FillMultiImagePartTypeXMinToMax(pValue->v2Min, pValue->v2Max, vMin, vMax, pValue->crDiffuse, fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_X_MAX_TO_MIN:
				FillMultiImagePartTypeXMaxToMin(pValue->v2Min, pValue->v2Max, vMin, vMax, pValue->crDiffuse, fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_Y_MIN_TO_MAX:
				FillMultiImagePartTypeYMinToMax(pValue->v2Min, pValue->v2Max, vMin, vMax, pValue->crDiffuse, fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_Y_MAX_TO_MIN:
				FillMultiImagePartTypeYMaxToMin(pValue->v2Min, pValue->v2Max, vMin, vMax, pValue->crDiffuse, fRatio, pVer, nTriCount);
				break;
			}

			pByteBuffer += nOffset;
			pVer += nTriCount * 3;

			nTriTotalCount += nTriCount;
		}

		if (m_Tex1Vertex.Unlock())
		{
			hr = m_Tex1Vertex.DrawPrimitive(D3DPT_TRIANGLELIST, pTexture, m_nTex1VertexLocked, nTriTotalCount);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::DrawMultiImageRotateImpl(KG3DUIImageRotate2 const* pBuffer, size_t nCount, size_t nOffset, DWORD dwImageID)
{
	HRESULT hr = E_FAIL;
	K2DTex1Vertex* pVer = NULL;
	BYTE const* pByteBuffer = NULL;
	UINT const uLockSize = static_cast<UINT>(6 * nCount);
	KG3DUIImageRect Rect = { 0.0f, 0.0f, 0.0f, 0.0f };
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	// flush texture
	pTexture = m_pUITexture->GetImageTexture(dwImageID, Rect, NULL);
	KG_PROCESS_ERROR(pTexture != NULL);

    if (m_Tex1Vertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_Tex1Vertex.GetLockSize());
		m_nTex1VertexLocked = m_Tex1Vertex.GetLockStart();

		pByteBuffer = reinterpret_cast<BYTE const*>(pBuffer);
		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			KG3DUIImageRotate2 const* pValue = reinterpret_cast<KG3DUIImageRotate2 const*>(pByteBuffer);
			D3DXVECTOR2 A(pValue->v2Min);
			D3DXVECTOR2 C(pValue->v2Max);
			D3DXVECTOR2 B(C.x, A.y);
			D3DXVECTOR2 D(A.x, C.y);
			D3DXVECTOR2 E((A + C) / 2.0f);
			D3DXCOLOR const diffuse = pValue->crDiffuse;
			float const fRotationCW = pValue->fRotationCW;
			D3DXMATRIX mRotation;

			A -= E;
			B -= E;
			C -= E;
			D -= E;

			D3DXMatrixRotationZ(&mRotation, fRotationCW);
			D3DXVec2TransformCoord(&A, &A, &mRotation);
			D3DXVec2TransformCoord(&B, &B, &mRotation);
			D3DXVec2TransformCoord(&C, &C, &mRotation);
			D3DXVec2TransformCoord(&D, &D, &mRotation);

			A += E;
			B += E;
			C += E;
			D += E;

			float const UA = pValue->v2UVMin.x * Rect.width + Rect.left;
			float const UC = pValue->v2UVMax.x * Rect.width + Rect.left;
			float const VA = pValue->v2UVMin.y * Rect.height + Rect.top;
			float const VC = pValue->v2UVMax.y * Rect.height + Rect.top;

			pVer[0].x = B.x; pVer[0].y = B.y; pVer[0].z = 0.0f; pVer[0].color = diffuse; pVer[0].tu = UC; pVer[0].tv = VA;
			pVer[1].x = A.x; pVer[1].y = A.y; pVer[1].z = 0.0f; pVer[1].color = diffuse; pVer[1].tu = UA; pVer[1].tv = VA;
			pVer[2].x = D.x; pVer[2].y = D.y; pVer[2].z = 0.0f; pVer[2].color = diffuse; pVer[2].tu = UA; pVer[2].tv = VC;

			pVer[3].x = B.x; pVer[3].y = B.y; pVer[3].z = 0.0f; pVer[3].color = diffuse; pVer[3].tu = UC; pVer[3].tv = VA;
			pVer[4].x = D.x; pVer[4].y = D.y; pVer[4].z = 0.0f; pVer[4].color = diffuse; pVer[4].tu = UA; pVer[4].tv = VC;
			pVer[5].x = C.x; pVer[5].y = C.y; pVer[5].z = 0.0f; pVer[5].color = diffuse; pVer[5].tu = UC; pVer[5].tv = VC;

			pByteBuffer += nOffset;
			pVer += 6;
		}

		if (m_Tex1Vertex.Unlock())
		{
			hr = m_Tex1Vertex.DrawPrimitive(D3DPT_TRIANGLELIST, pTexture, m_nTex1VertexLocked, static_cast<UINT>(2 * nCount));
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::DrawMultiImage(KG3DUIImage2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	DWORD dwLastImageID = 0;
	size_t nLastIndex = 0;
	size_t nIndex = 0;
	KG3DUIImage2 const* pValue = pBuffer;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);

	KG_PROCESS_SUCCESS(nCount == 0);

	ChangeDrawState(DRAW_STATE_IMAGE);

	dwLastImageID = pBuffer[0].dwImageID;

	for (; nIndex != nCount; ++nIndex)
	{
		DWORD const dwImageID = pValue->dwImageID;

		if (dwImageID != dwLastImageID)
		{
			hr = DrawMultiImageImpl(pBuffer + nLastIndex, nIndex - nLastIndex, sizeof(KG3DUIImage2), dwLastImageID);
			KG_COM_PROCESS_ERROR(hr);

			dwLastImageID = dwImageID;

			nLastIndex = nIndex;
		}

		++pValue;
	}

	hr = DrawMultiImageImpl(pBuffer + nLastIndex, nIndex - nLastIndex, sizeof(KG3DUIImage2), dwLastImageID);
	KG_COM_PROCESS_ERROR(hr);

Exit1:
	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::DrawMultiScene(KG3DUIScene2 const* pBuffer, size_t nCount)
{
	HRESULT hr = S_OK;

    D3DPERF_BeginEvent(0x00FF00FF, L"KG3DUI::DrawMultiScene");

    KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);

	KG_PROCESS_SUCCESS(nCount == 0);

	ChangeDrawState(DRAW_STATE_NONE);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mSceneProjection);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_mSceneWorld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_mSceneView);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);


	{
		KG3DUIRenderStateGuard aRenderStateGuard[14];
		KG3DUITextureStageStateGuard aTextureStage0StateGuard[8];
		KG3DUITextureStageStateGuard aTextureStage1StateGuard[2];
		KG3DUISamplerStateGuard aSampler0StateGuard[5];
		KG3DUISamplerStateGuard aSampler1StateGuard[5];
		KG3DUISamplerStateGuard aSampler2StateGuard[5];
		KG3DUIViewportGuard	ViewportGuard;

		aRenderStateGuard[ 0].Save(D3DRS_ALPHATESTENABLE);
		aRenderStateGuard[ 1].Save(D3DRS_ALPHABLENDENABLE);
		aRenderStateGuard[ 2].Save(D3DRS_SRCBLEND);
		aRenderStateGuard[ 3].Save(D3DRS_DESTBLEND);
		aRenderStateGuard[ 4].Save(D3DRS_LIGHTING);
		aRenderStateGuard[ 5].Save(D3DRS_CULLMODE);
		aRenderStateGuard[ 6].Save(D3DRS_FOGENABLE);
		aRenderStateGuard[ 7].Save(D3DRS_ZENABLE);
		aRenderStateGuard[ 8].Save(D3DRS_SEPARATEALPHABLENDENABLE);
		aRenderStateGuard[ 9].Save(D3DRS_SRCBLENDALPHA);
		aRenderStateGuard[10].Save(D3DRS_DESTBLENDALPHA);
		aRenderStateGuard[11].Save(D3DRS_ALPHAFUNC);
		aRenderStateGuard[12].Save(D3DRS_ZWRITEENABLE);
		aRenderStateGuard[13].Save(D3DRS_DEPTHBIAS, 0);

		aTextureStage0StateGuard[0].Save(0, D3DTSS_TEXCOORDINDEX);
		aTextureStage0StateGuard[1].Save(0, D3DTSS_COLOROP);
		aTextureStage0StateGuard[2].Save(0, D3DTSS_ALPHAOP);
		aTextureStage0StateGuard[3].Save(0, D3DTSS_COLORARG1);
		aTextureStage0StateGuard[4].Save(0, D3DTSS_COLORARG2);
		aTextureStage0StateGuard[5].Save(0, D3DTSS_ALPHAARG1);
		aTextureStage0StateGuard[6].Save(0, D3DTSS_ALPHAARG2);
		aTextureStage0StateGuard[7].Save(0, D3DTSS_TEXTURETRANSFORMFLAGS);

		aTextureStage1StateGuard[0].Save(1, D3DTSS_COLOROP);
		aTextureStage1StateGuard[1].Save(1, D3DTSS_ALPHAOP);

		//aSampler0StateGuard[0].Save(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		//aSampler0StateGuard[1].Save(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		//aSampler0StateGuard[2].Save(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		//aSampler0StateGuard[3].Save(0, D3DSAMP_MIPMAPLODBIAS, 0);

		//aSampler1StateGuard[0].Save(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		//aSampler1StateGuard[1].Save(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		//aSampler1StateGuard[2].Save(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		//aSampler1StateGuard[3].Save(1, D3DSAMP_MIPMAPLODBIAS, 0);

		//aSampler2StateGuard[0].Save(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		//aSampler2StateGuard[1].Save(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		//aSampler2StateGuard[2].Save(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		//aSampler2StateGuard[3].Save(2, D3DSAMP_MIPMAPLODBIAS, 0);

		aSampler0StateGuard[0].Save(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		aSampler0StateGuard[1].Save(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		aSampler0StateGuard[2].Save(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		aSampler0StateGuard[3].Save(0, D3DSAMP_MIPMAPLODBIAS, 0);
		aSampler0StateGuard[4].Save(0, D3DSAMP_MAXANISOTROPY, g_cEngineOption.dwMaxAnisotropy);

		aSampler1StateGuard[0].Save(1, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		aSampler1StateGuard[1].Save(1, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		aSampler1StateGuard[2].Save(1, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		aSampler1StateGuard[3].Save(1, D3DSAMP_MIPMAPLODBIAS, 0);
		aSampler1StateGuard[4].Save(1, D3DSAMP_MAXANISOTROPY, g_cEngineOption.dwMaxAnisotropy);

		aSampler2StateGuard[0].Save(2, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		aSampler2StateGuard[1].Save(2, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		aSampler2StateGuard[2].Save(2, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		aSampler2StateGuard[3].Save(2, D3DSAMP_MIPMAPLODBIAS, 0);
		aSampler2StateGuard[4].Save(2, D3DSAMP_MAXANISOTROPY, g_cEngineOption.dwMaxAnisotropy);

		ViewportGuard.Save();

		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			KG3DUIScene2 const& item = pBuffer[nIndex];
			D3DXVECTOR2 const* pA = &item.v2Min;
			D3DXVECTOR2 const* pC = &item.v2Max;
			DWORD const dwFlags = item.dwFlags;
			IKG3DScene* p3DScene = item.p3DScene;

			_ASSERTE(p3DScene);

			hr = p3DScene->RenderToScreen(pA, pC, NULL, NULL, 0.0f, dwFlags);	//如果这里调用次数很多的话，最好加上BeginRenderToScreen, EndRenderToScreen来一次设置所有状态
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mUIProjection);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_mUIWorld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_mUIView);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

Exit1:
	hr = S_OK;
Exit0:
    D3DPERF_EndEvent();
    return hr;
}

HRESULT KG3DUI::FillMultiRectPart(KG3DUIRectPart2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	K2DDiffuseVertex* pVer = NULL;
	UINT const uLockSize = static_cast<UINT>(3 * 5 * nCount);
	int nTriTotalCount = 0;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);
	KGLOG_PROCESS_ERROR(g_pd3dDevice);

	ChangeDrawState(DRAW_STATE_FILL);

	if (m_DiffuseVertex.Lock(uLockSize, (void**)&pVer))
	{
		ASSERT(uLockSize == m_DiffuseVertex.GetLockSize());
		m_nDiffuseVertexLocked = m_DiffuseVertex.GetLockStart();

		for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
		{
			int nTriCount = 0;

			switch (pBuffer->dwType)
			{
			case KG3DUI_PART_TYPE_TIMER:
				FillMultiRectPartTypeTimer(pBuffer->v2Min, pBuffer->v2Max, pBuffer->crDiffuse, pBuffer->fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_X_MIN_TO_MAX:
				FillMultiRectPartTypeXMinToMax(pBuffer->v2Min, pBuffer->v2Max, pBuffer->crDiffuse, pBuffer->fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_X_MAX_TO_MIN:
				FillMultiRectPartTypeXMaxToMin(pBuffer->v2Min, pBuffer->v2Max, pBuffer->crDiffuse, pBuffer->fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_Y_MIN_TO_MAX:
				FillMultiRectPartTypeYMinToMax(pBuffer->v2Min, pBuffer->v2Max, pBuffer->crDiffuse, pBuffer->fRatio, pVer, nTriCount);
				break;
			case KG3DUI_PART_TYPE_Y_MAX_TO_MIN:
				FillMultiRectPartTypeYMaxToMin(pBuffer->v2Min, pBuffer->v2Max, pBuffer->crDiffuse, pBuffer->fRatio, pVer, nTriCount);
				break;
			}

			++pBuffer;
			pVer += nTriCount * 3;

			nTriTotalCount += nTriCount;
		}

		if (m_DiffuseVertex.Unlock())
		{
			hr = m_DiffuseVertex.DrawPrimitive(D3DPT_TRIANGLELIST, m_nDiffuseVertexLocked, nTriTotalCount);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::DrawMultiImagePart(KG3DUIImagePart2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	DWORD dwLastImageID = 0;
	size_t nLastIndex = 0;
	size_t nIndex = 0;
	KG3DUIImagePart2 const* pValue = pBuffer;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);

	KG_PROCESS_SUCCESS(nCount == 0);

	ChangeDrawState(DRAW_STATE_IMAGE);

	dwLastImageID = pBuffer[0].dwImageID;

	for (; nIndex != nCount; ++nIndex)
	{
		DWORD const dwImageID = pValue->dwImageID;

		if (dwImageID != dwLastImageID)
		{
			hr = DrawMultiImagePartImpl(pBuffer + nLastIndex, nIndex - nLastIndex, sizeof(KG3DUIImagePart2), dwLastImageID);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			dwLastImageID = dwImageID;

			nLastIndex = nIndex;
		}

		++pValue;
	}

	hr = DrawMultiImagePartImpl(pBuffer + nLastIndex, nIndex - nLastIndex, sizeof(KG3DUIImagePart2), dwLastImageID);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

Exit1:
	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::DrawMultiImageRotate(KG3DUIImageRotate2 const* pBuffer, size_t nCount)
{
	HRESULT hr = E_FAIL;
	DWORD dwLastImageID = 0;
	size_t nLastIndex = 0;
	size_t nIndex = 0;
	KG3DUIImageRotate2 const* pValue = pBuffer;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(nCount >= 0);

	KG_PROCESS_SUCCESS(nCount == 0);

	ChangeDrawState(DRAW_STATE_IMAGE);

	dwLastImageID = pBuffer[0].dwImageID;

	for (; nIndex != nCount; ++nIndex)
	{
		DWORD const dwImageID = pValue->dwImageID;

		if (dwImageID != dwLastImageID)
		{
			hr = DrawMultiImageRotateImpl(pBuffer + nLastIndex, nIndex - nLastIndex, sizeof(KG3DUIImageRotate2), dwLastImageID);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			dwLastImageID = dwImageID;

			nLastIndex = nIndex;
		}

		++pValue;
	}

	hr = DrawMultiImageRotateImpl(pBuffer + nLastIndex, nIndex - nLastIndex, sizeof(KG3DUIImageRotate2), dwLastImageID);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

Exit1:
	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::BeginRenderToTexture(LPDIRECT3DTEXTURE9 pTexture, UINT uMipmapLevel)
{
	m_RenderToTextureState.Save(pTexture, NULL, NULL, uMipmapLevel);
	return S_OK;
}


HRESULT KG3DUI::BeginRenderToTexture(DWORD dwTextureID, UINT uMipmapLevel)
{
    HRESULT hr = E_FAIL;

    hr = g_cGraphicsEngine.CheckDevice();
    KG_COM_PROCESS_ERROR(hr);

    m_RenderToTextureState.Save(dwTextureID, NULL, NULL, uMipmapLevel);

Exit0:
	return hr;
}


HRESULT KG3DUI::EndRenderToTexture()
{
	ChangeDrawState(DRAW_STATE_NONE);

	m_RenderToTextureState.Restore();
	return S_OK;
}


HRESULT KG3DUI::BeginRenderToImage(DWORD dwImageID)
{
    HRESULT hResult = E_FAIL;
	HRESULT hr = E_FAIL;
	D3DXVECTOR2 v2Min;
	D3DXVECTOR2 v2Max;
	KG3DUIImageRect ImageRect;
	LPDIRECT3DTEXTURE9 pTexture = NULL;

    hr = g_cGraphicsEngine.CheckDevice();
    KG_COM_PROCESS_ERROR(hr);

    pTexture = m_pUITexture->GetImageTexture(dwImageID, ImageRect, NULL);
	KGLOG_PROCESS_ERROR_CONSOLE(pTexture);

	v2Min = D3DXVECTOR2(ImageRect.left, ImageRect.top);
	v2Max = D3DXVECTOR2(ImageRect.left + ImageRect.width, ImageRect.top + ImageRect.height);

	m_RenderToImageState.Save(pTexture, &v2Min, &v2Max, 0);

    hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DUI::EndRenderToImage()
{
	ChangeDrawState(DRAW_STATE_NONE);

	m_RenderToImageState.Restore();

	return S_OK;
}

HRESULT KG3DUI::BeginStencil()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(g_pd3dDevice);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000000F);

	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NEVER);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DUI::SetStencilFunc()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(g_pd3dDevice);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF,  0x00000001);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DUI::EndStencil()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(g_pd3dDevice);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DUI::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;
	return S_OK;
}

HRESULT KG3DUI::OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bInitUITexture = FALSE;
    BOOL bInitFontRenderer = FALSE;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    ASSERT(m_pUITexture == NULL);

	m_pUITexture = new KG3DUITexture;
	KGLOG_PROCESS_ERROR(m_pUITexture);

	hr = m_pUITexture->Initialize(g_pd3dDevice);
	KGLOG_COM_PROCESS_ERROR(hr);

    bInitUITexture = TRUE;

    ASSERT(g_pFontRenderer == NULL);

	g_pFontRenderer = new KG3DFontRenderer();
	KGLOG_PROCESS_ERROR(g_pFontRenderer);

	hr = g_pFontRenderer->Initialize(g_pd3dDevice);
	KGLOG_COM_PROCESS_ERROR(hr);

    bInitFontRenderer = TRUE;

#ifdef ENABLE_VIDEO_MEMORY_INFO
	hr = DirectDrawCreateEx(NULL, (void**)&m_pDirectDraw, IID_IDirectDraw7, NULL);
	if (SUCCEEDED(hr))
		hr = m_pDirectDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL);   
#endif

#ifdef ENABLE_KG3DUI_EFFECT
	{
		LPD3DXBUFFER pErrMsg = NULL;
		hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, _T("data\\public\\shader\\gray.fx"), NULL, NULL, D3DXSHADER_DEBUG | D3DXSHADER_SKIPVALIDATION, NULL, &m_pEffect, &pErrMsg);
		if (FAILED(hr) && pErrMsg)
		{
			LPCTSTR szErrMsg = (LPCTSTR)pErrMsg->GetBufferPointer();
			::MessageBox(NULL, szErrMsg, _T("ERROR"), MB_OK);
			pErrMsg->Release();
		}
	}

    if (m_pEffect)
    {
		hr = m_pEffect->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }
#endif

    if (m_pCallback)
    {
        hr = m_pCallback->OnCreateDevice(pd3dDevice, pBackBufferSurfaceDesc, pPresentParameters);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
#ifdef ENABLE_VIDEO_MEMORY_INFO
	SAFE_RELEASE(m_pDirectDraw);
#endif

#ifdef ENABLE_KG3DUI_EFFECT
        SAFE_RELEASE(m_pEffect);
#endif
        if (bInitFontRenderer)
        {
            hr = g_pFontRenderer->CleanUp();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        SAFE_DELETE(g_pFontRenderer);

        if (bInitUITexture)
        {
            hr = m_pUITexture->CleanUp();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        SAFE_DELETE(m_pUITexture);
    }
	return hrResult;
}

HRESULT KG3DUI::OnDestroyDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pCallback)
    {
        hr = m_pCallback->OnDestroyDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

#ifdef ENABLE_VIDEO_MEMORY_INFO
	SAFE_RELEASE(m_pDirectDraw);
#endif

#ifdef ENABLE_KG3DUI_EFFECT
	SAFE_RELEASE(m_pEffect);
#endif
    if (m_pUITexture)
    {
        hr = m_pUITexture->CleanUp();
        KGLOG_COM_PROCESS_ERROR(hr);

        SAFE_DELETE(m_pUITexture);
    }

    if (g_pFontRenderer)
    {
        hr = g_pFontRenderer->CleanUp();
        KGLOG_COM_PROCESS_ERROR(hr);

        SAFE_DELETE(g_pFontRenderer);
    }

    m_DiffuseVertex.Release();
    m_Tex1Vertex.Release();

    for (KG3DUIMiniScenePtrList::iterator it = m_MiniScenePtrList.begin(); it != m_MiniScenePtrList.end(); ++it)
    {
        KG3DUIMiniScene* p3DUIMiniScene = *it;

        hr = p3DUIMiniScene->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    ASSERT(m_MiniScenePtrList.empty());

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DUI::OnLostDevice()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pCallback)
    {
        hr = m_pCallback->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

#ifdef ENABLE_KG3DUI_EFFECT
	if (m_pEffect)
    {
		hr = m_pEffect->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }
#endif

	hr = InvalidateFont();
	KGLOG_COM_PROCESS_ERROR(hr);

	m_Tex1Vertex.OnLostDevice();
	m_DiffuseVertex.OnLostDevice();

	if (m_pUITexture)
    {
		hr = m_pUITexture->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = m_RenderToTextureState.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderToImageState.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    for (KG3DUIMiniScenePtrList::iterator it = m_MiniScenePtrList.begin(); it != m_MiniScenePtrList.end(); ++it)
    {
        KG3DUIMiniScene* p3DUIMiniScene = *it;

        hr = p3DUIMiniScene->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DUI::OnResetDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, 
                              D3DPRESENT_PARAMETERS const* pPresentParameters)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    for (KG3DUIMiniScenePtrList::iterator it = m_MiniScenePtrList.begin(); it != m_MiniScenePtrList.end(); ++it)
    {
        KG3DUIMiniScene* p3DUIMiniScene = *it;

        hr = p3DUIMiniScene->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = m_RenderToImageState.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderToTextureState.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = RestoreDeviceFont();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_Tex1Vertex.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_DiffuseVertex.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	if (m_pUITexture)
    {
		hr = m_pUITexture->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

#ifdef ENABLE_KG3DUI_EFFECT
	if (m_pEffect)
    {
		hr = m_pEffect->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }
#endif

    if (m_pCallback)
    {
        hr = m_pCallback->OnResetDevice(pd3dDevice, pBackBufferSurfaceDesc, pPresentParameters);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
	    OnLostDevice();
    }
	return hrResult;
}


HRESULT KG3DUI::InvalidateFont()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	for (int i = 0; i < _countof(g_FontTable); ++i)
	{
		if (g_FontTable[i])
        {
            hr = g_FontTable[i]->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
	}

	if (g_pFontRenderer)
	{
        hr = g_pFontRenderer->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
	}

    hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DUI::RestoreDeviceFont()
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; ++i)
	{
		if (g_FontTable[i])
        {
            hr = g_FontTable[i]->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
	}

    if (g_pFontRenderer)
    {
        hr = g_pFontRenderer->OnResetDevice();
	    KGLOG_COM_PROCESS_ERROR(hr);
    }

	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DUI::SetViewportSize(UINT uWidth, UINT uHeight)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    RECT rcViewport = { 0, 0, uWidth, uHeight };
    FLOAT cx = 0.0f;
    FLOAT cy = 0.0f;

	KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_cGraphicsEngine.SetViewportSize(uWidth, uHeight);
    KGLOG_COM_PROCESS_ERROR(hr);

	m_uViewportWidth = uWidth;
	m_uViewportHeight = uHeight;

    cx = (FLOAT)m_uViewportWidth;
    cy = (FLOAT)m_uViewportHeight;

	D3DXMatrixOrthoLH(&m_mUIProjection, cx, -cy, 0.0f, 1.0f);

	D3DXMatrixIdentity(&m_mUIWorld);
	D3DXMatrixTranslation(&m_mUIWorld, cx * -0.5f, cy * -0.5f, 1.0f);

	D3DXMatrixIdentity(&m_mUIView);

    hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_mSceneProjection);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &m_mSceneWorld);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_VIEW, &m_mSceneView);
	KGLOG_COM_PROCESS_ERROR(hr);

    g_cEngineManager.MovieViewport(rcViewport);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DUI::UpdateMultiSceneViewport(KG3DUIScene2 const* pBuffer, size_t nCount)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    for (size_t nIndex = 0; nIndex != nCount; ++nIndex)
    {
        KG3DUIScene2 const& item = pBuffer[nIndex];
        IKG3DScene* p3DScene = item.p3DScene;

        RECT rcViewport = 
        { 
            (LONG)(item.v2Min.x + 0.5f), 
            (LONG)(item.v2Min.y + 0.5f), 
            (LONG)(item.v2Max.x + 0.5f), 
            (LONG)(item.v2Max.y + 0.5f)
        };

        _ASSERTE(p3DScene);

        hr = p3DScene->OnResizeWindow(&rcViewport);
        KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DUI::GetDeviceSettings(KG3DDeviceSettings* pSettings)
{
    HRESULT hr = S_OK;

    KGLOG_PROCESS_ERROR(pSettings);

    hr = g_cGraphicsEngine.GetDeviceSettings(pSettings);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

HRESULT KG3DUI::SetDeviceSettings(KG3DDeviceSettings* pSettings)
{
#if 0
    KGLogPrintf(KGLOG_INFO, "SetDeviceSettings()\n");
    KGLogPrintf(KGLOG_INFO, "\t%u x %u x %u %uHz\n", pSettings->uWidth, pSettings->uHeight, pSettings->uColorDepth, pSettings->uRefreshRate);
    KGLogPrintf(KGLOG_INFO, "\tFullScreen:%d Panauision:%d ExclusiveMode:%d\n", pSettings->bFullScreen, pSettings->bPanauision, pSettings->bExclusiveMode);
    KGLogPrintf(KGLOG_INFO, "\tTripleBuffering:%d VSync:%d\n", pSettings->bTripleBuffering, pSettings->bVSync);
    KGLogPrintf(KGLOG_INFO, "\tMultiSample Type:%d Quality:%d\n", pSettings->nMultiSampleType, pSettings->dwMultiSampleQuality);
#endif

    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pSettings);

    hr = g_cGraphicsEngine.SetDeviceSettings(pSettings);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = SetViewportSize(pSettings->uWidth, pSettings->uHeight);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

UINT KG3DUI::GetAvailableTextureMem() const
{
	ASSERT(g_pd3dDevice);
	return g_pd3dDevice->GetAvailableTextureMem();
}

VideoMemoryInfo KG3DUI::GetVideoMemeryInfo() const
{
	VideoMemoryInfo Info;

	ZeroMemory(&Info, sizeof(Info));

#ifdef ENABLE_VIDEO_MEMORY_INFO
	ASSERT(g_pd3dDevice);

	if (m_pDirectDraw)
	{
		DDSCAPS2 ddscaps2;
		DWORD    dwTotal;
		DWORD    dwFree;

		ZeroMemory(&ddscaps2, sizeof(ddscaps2));

		ddscaps2.dwCaps =  DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
		m_pDirectDraw->GetAvailableVidMem(&ddscaps2, &dwTotal, &dwFree);
		Info.dwLocalTatal = dwTotal;
		Info.dwLocalFree  = dwFree;

		ddscaps2.dwCaps =  DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
		m_pDirectDraw->GetAvailableVidMem(&ddscaps2, &dwTotal, &dwFree);
		Info.dwNonLocalTatal = dwTotal;
		Info.dwNonLocalFree  = dwFree;

		ddscaps2.dwCaps =  DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
		m_pDirectDraw->GetAvailableVidMem(&ddscaps2, &dwTotal, &dwFree);
		Info.dwTextureTatal = dwTotal;
		Info.dwTexturnFree  = dwFree;

		ddscaps2.dwCaps =  DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
		m_pDirectDraw->GetAvailableVidMem(&ddscaps2, &dwTotal, &dwFree);
		Info.dwD3DTotal = dwTotal;
		Info.dwD3DFree  = dwFree;
	}
#endif

	return Info;
}


void KG3DUI::GetUITextureUsage(KG3DUITextureUsage& Usage) const
{
	if (m_pUITexture)
	{
		Usage.uMemoryUsage = m_pUITexture->GetMemoryUsage();
		Usage.uTextureUsage = m_pUITexture->GetTextureUsage();
		Usage.uSliceUsage = m_pUITexture->GetSliceUsage();
	}
	else
	{
		memset(&Usage, 0, sizeof(KG3DUITextureUsage));
	}
}


HRESULT KG3DUI::GetCharHeight(DWORD dwFontID, FLOAT fScale, FLOAT& fHeight) const
{
	KG3DFontTexture* pFont = NULL;

	pFont = g_FontTable[dwFontID];
	KGLOG_PROCESS_ERROR(pFont);

	fHeight = pFont->GetCharHeight() * fScale;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DUI::GetCharWidth(DWORD dwFontID, FLOAT fScale, FLOAT& fWidth) const
{
	KG3DFontTexture* pFont = NULL;

	pFont = g_FontTable[dwFontID];
	KGLOG_PROCESS_ERROR(pFont);

	fWidth = pFont->GetCharWidth() * fScale;

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::GetTextExtent(DWORD dwFontID, LPCWSTR wszText, int nTextLength, FLOAT &fWidth, FLOAT &fHeight, FLOAT fSpace, FLOAT fScale, BOOL bBorder) const
{
	BOOL bRetCode = FALSE;
	KG3DFontTexture* pFont = NULL;

	ASSERT(wszText);

	pFont = g_FontTable[dwFontID];
	KGLOG_PROCESS_ERROR(pFont);
	KGLOG_PROCESS_ERROR(g_pFontRenderer);

	if (nTextLength == 0)
	{
		fWidth = 0;
		fHeight = 0;
	}
	else
	{
		pFont->SetTextScaling(fScale, fScale);

		bRetCode = pFont->GetTextExtent(wszText, nTextLength, fWidth, fHeight, fSpace, bBorder);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DUI::GetTextPosExtent(DWORD dwFontID, LPCWSTR wszText, int nTextLength, FLOAT fCursorPos, LPINT lpCharPos, FLOAT fSpace, FLOAT fScale, BOOL bBorder) const
{
	BOOL bRetCode = FALSE;
	KG3DFontTexture* pFont = NULL;

	ASSERT(wszText);
	ASSERT(lpCharPos);

	pFont = g_FontTable[dwFontID];
	KGLOG_PROCESS_ERROR(pFont);
	KGLOG_PROCESS_ERROR(g_pFontRenderer);

	if (nTextLength == 0)
	{
		*lpCharPos = 0;
	}
	else
	{
		pFont->SetTextScaling(fScale, fScale);

		bRetCode = pFont->GetTextPosExtent(wszText, nTextLength, fCursorPos, lpCharPos, fSpace, bBorder);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DUI::BeginScene()
{
	HRESULT hr = E_FAIL;

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->BeginScene();
	KGLOG_COM_PROCESS_ERROR(hr);

    D3DPERF_BeginEvent(0x00FF00FF, L"KG3DUI::Draw Scene");

Exit0:
    return hr;
}

HRESULT KG3DUI::EndScene()
{
	HRESULT hr = E_FAIL;

    D3DPERF_EndEvent();

    KGLOG_PROCESS_ERROR(g_pd3dDevice);

    hr = g_pd3dDevice->EndScene();
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

HRESULT KG3DUI::BeginRenderToScreen()
{
    HRESULT hr = E_FAIL;
	D3DVIEWPORT9 ViewPort = { 0, 0, m_uViewportWidth, m_uViewportHeight, 0.0f, 1.0f	};
	D3DRECT Rect = { 0, 0, m_uViewportWidth, m_uViewportHeight };

    hr = g_cGraphicsEngine.CheckDevice();
    KG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); // Default
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE); // Default
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU); // Default
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); // Default
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); // Default
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT); // Default
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); // Default
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT); // Default
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTexture(0, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTexture(1, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTexture(2, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetViewport(&ViewPort);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->Clear(1, &Rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_mSceneProjection);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &m_mSceneWorld);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->GetTransform(D3DTS_VIEW, &m_mSceneView);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mUIProjection);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_mUIWorld);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_mUIView);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DUI::EndRenderToScreen()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	g_cEngineManager.MoviePresent();

	ChangeDrawState(DRAW_STATE_NONE);

    hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mSceneProjection);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_mSceneWorld);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_mSceneView);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DUI::Present()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    RECT rect;
    
    rect.left = 0;
    rect.top = 0;
    rect.right = m_uViewportWidth;
    rect.bottom = m_uViewportHeight;

    hr = g_cGraphicsEngine.Present(&rect, NULL, m_hWnd, NULL);
    KG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

UINT KG3DUI::GetAllowableAdapterModeCount() const
{
	UINT uCount = 0;

    KGLOG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

	uCount = g_cGraphicsEngine.m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);

Exit0:
    return uCount;
}

HRESULT KG3DUI::GetAllowableAdapterMode(D3DDISPLAYMODE* pAdapterMode, UINT* puCount) const
{
    HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	UINT uModeCount = 0;
    UINT uModeIndex = 0;

    KGLOG_PROCESS_ERROR(pAdapterMode);
    KGLOG_PROCESS_ERROR(puCount);
	KGLOG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

	uModeCount = g_cGraphicsEngine.m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
    uModeCount = min(uModeCount, *puCount);

	for (UINT uMode = 0; uMode < uModeCount; ++uMode)
	{
		hr = g_cGraphicsEngine.m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, uMode, pAdapterMode + uModeIndex);
        KGLOG_COM_CHECK_ERROR(hr);

        if (SUCCEEDED(hr))
        {
            ++uModeIndex;
        }
	}

    *puCount = uModeIndex;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

void KG3DUI::ChangeDrawState(KDrawState DrawState)
{
#if 0
	static LPCSTR s_szNames[] =
	{
		"DRAW_STATE_NONE",
		"DRAW_STATE_LINE",
		"DRAW_STATE_FILL",
		"DRAW_STATE_TEXT",
		"DRAW_STATE_IMAGE",
		"DRAW_STATE_SCENE",
	};

	KGLogPrintf(KGLOG_INFO, "KG3DUI::ChangeDrawState(%s)\n", s_szNames[DrawState]);
#endif

	if (DrawState == m_DrawState)
		return;

	switch (m_DrawState)
	{
	case DRAW_STATE_NONE:
		break;
	case DRAW_STATE_LINE:
		m_aRenderStateGuard[0].Restore();
		break;
	case DRAW_STATE_FILL:
		break;
	case DRAW_STATE_TEXT:
		m_aTextureStage0StateGuard[0].Restore();
		m_aTextureStage0StateGuard[1].Restore();
		m_aTextureStage0StateGuard[2].Restore();
		m_aTextureStage0StateGuard[3].Restore();
		m_aTextureStage0StateGuard[4].Restore();

		m_aTextureStage1StateGuard[0].Restore();
		m_aTextureStage1StateGuard[1].Restore();

		m_aRenderStateGuard[0].Restore();
		m_aRenderStateGuard[1].Restore();
		m_aRenderStateGuard[2].Restore();

		m_aSampler0StateGuard[0].Restore();
		m_aSampler0StateGuard[1].Restore();
		m_aSampler0StateGuard[2].Restore();
		break;
	case DRAW_STATE_IMAGE:
		m_aTextureStage1StateGuard[0].Restore();
		m_aTextureStage1StateGuard[1].Restore();
		m_aTextureStage1StateGuard[2].Restore();
		m_aTextureStage1StateGuard[3].Restore();
		m_aTextureStage1StateGuard[4].Restore();
		m_aTextureStage1StateGuard[5].Restore();

		m_aRenderStateGuard[0].Restore();
		m_aRenderStateGuard[1].Restore();
		m_aRenderStateGuard[2].Restore();

		m_aSampler0StateGuard[0].Restore();
		m_aSampler0StateGuard[1].Restore();
		break;
	case DRAW_STATE_SCENE:
		m_ViewportGuard.Restore();

		m_aRenderStateGuard[ 0].Restore();
		m_aRenderStateGuard[ 1].Restore();
		m_aRenderStateGuard[ 2].Restore();
		m_aRenderStateGuard[ 3].Restore();
		m_aRenderStateGuard[ 4].Restore();
		m_aRenderStateGuard[ 5].Restore();
		m_aRenderStateGuard[ 6].Restore();
		m_aRenderStateGuard[ 7].Restore();
		m_aRenderStateGuard[ 8].Restore();
		m_aRenderStateGuard[ 9].Restore();
		m_aRenderStateGuard[10].Restore();
		m_aRenderStateGuard[11].Restore();
		m_aRenderStateGuard[12].Restore();
		m_aRenderStateGuard[13].Restore();

		m_aTextureStage0StateGuard[0].Restore();
		m_aTextureStage0StateGuard[1].Restore();
		m_aTextureStage0StateGuard[2].Restore();
		m_aTextureStage0StateGuard[3].Restore();
		m_aTextureStage0StateGuard[4].Restore();
		m_aTextureStage0StateGuard[5].Restore();
		m_aTextureStage0StateGuard[6].Restore();
		m_aTextureStage0StateGuard[7].Restore();

		m_aTextureStage1StateGuard[0].Restore();
		m_aTextureStage1StateGuard[1].Restore();

		m_aSampler0StateGuard[0].Restore();
		m_aSampler0StateGuard[1].Restore();
		m_aSampler0StateGuard[2].Restore();
		m_aSampler0StateGuard[3].Restore();

		m_aSampler1StateGuard[0].Restore();
		m_aSampler1StateGuard[1].Restore();
		m_aSampler1StateGuard[2].Restore();
		m_aSampler1StateGuard[3].Restore();

		m_aSampler2StateGuard[0].Restore();
		m_aSampler2StateGuard[1].Restore();
		m_aSampler2StateGuard[2].Restore();
		m_aSampler2StateGuard[3].Restore();

        break;
	default:
		ASSERT(0);
		break;
	};

	switch (DrawState)
	{
	case DRAW_STATE_NONE:
		break;
	case DRAW_STATE_LINE:
		g_pd3dDevice->SetTexture(0, NULL);
		g_pd3dDevice->SetTexture(1, NULL);

		m_aRenderStateGuard[0].Save(D3DRS_ALPHABLENDENABLE, FALSE);
		break;
	case DRAW_STATE_FILL:
		g_pd3dDevice->SetTexture(0, NULL);
		g_pd3dDevice->SetTexture(1, NULL);
		break;
	case DRAW_STATE_TEXT:
		m_aTextureStage0StateGuard[0].Save(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_aTextureStage0StateGuard[1].Save(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_aTextureStage0StateGuard[2].Save(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_aTextureStage0StateGuard[3].Save(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_aTextureStage0StateGuard[4].Save(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_aTextureStage1StateGuard[0].Save(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_aTextureStage1StateGuard[1].Save(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_aRenderStateGuard[0].Save(D3DRS_ALPHABLENDENABLE, TRUE);
		m_aRenderStateGuard[1].Save(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_aRenderStateGuard[2].Save(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_aSampler0StateGuard[0].Save(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_aSampler0StateGuard[1].Save(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_aSampler0StateGuard[2].Save(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		break;
	case DRAW_STATE_IMAGE:
		m_aTextureStage1StateGuard[0].Save(1, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		m_aTextureStage1StateGuard[1].Save(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_aTextureStage1StateGuard[2].Save(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_aTextureStage1StateGuard[3].Save(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		m_aTextureStage1StateGuard[4].Save(1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_aTextureStage1StateGuard[5].Save(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

		m_aRenderStateGuard[0].Save(D3DRS_ALPHABLENDENABLE, TRUE);
		m_aRenderStateGuard[1].Save(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
		m_aRenderStateGuard[2].Save(D3DRS_DESTBLENDALPHA, D3DBLEND_DESTALPHA);

		m_aSampler0StateGuard[0].Save(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_aSampler0StateGuard[1].Save(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		break;
	case DRAW_STATE_SCENE:
		m_aRenderStateGuard[ 0].Save(D3DRS_ALPHATESTENABLE);
		m_aRenderStateGuard[ 1].Save(D3DRS_ALPHABLENDENABLE);
		m_aRenderStateGuard[ 2].Save(D3DRS_SRCBLEND);
		m_aRenderStateGuard[ 3].Save(D3DRS_DESTBLEND);
		m_aRenderStateGuard[ 4].Save(D3DRS_LIGHTING);
		m_aRenderStateGuard[ 5].Save(D3DRS_CULLMODE);
		m_aRenderStateGuard[ 6].Save(D3DRS_FOGENABLE);
		m_aRenderStateGuard[ 7].Save(D3DRS_ZENABLE);
		m_aRenderStateGuard[ 8].Save(D3DRS_SEPARATEALPHABLENDENABLE);
		m_aRenderStateGuard[ 9].Save(D3DRS_SRCBLENDALPHA);
		m_aRenderStateGuard[10].Save(D3DRS_DESTBLENDALPHA);
		m_aRenderStateGuard[11].Save(D3DRS_ALPHAFUNC);
		m_aRenderStateGuard[12].Save(D3DRS_ZWRITEENABLE);
		m_aRenderStateGuard[13].Save(D3DRS_DEPTHBIAS, 0);

		m_aTextureStage0StateGuard[0].Save(0, D3DTSS_TEXCOORDINDEX);
		m_aTextureStage0StateGuard[1].Save(0, D3DTSS_COLOROP);
		m_aTextureStage0StateGuard[2].Save(0, D3DTSS_ALPHAOP);
		m_aTextureStage0StateGuard[3].Save(0, D3DTSS_COLORARG1);
		m_aTextureStage0StateGuard[4].Save(0, D3DTSS_COLORARG2);
		m_aTextureStage0StateGuard[5].Save(0, D3DTSS_ALPHAARG1);
		m_aTextureStage0StateGuard[6].Save(0, D3DTSS_ALPHAARG2);
		m_aTextureStage0StateGuard[7].Save(0, D3DTSS_TEXTURETRANSFORMFLAGS);

		m_aTextureStage1StateGuard[0].Save(1, D3DTSS_COLOROP);
		m_aTextureStage1StateGuard[1].Save(1, D3DTSS_ALPHAOP);

		m_aSampler0StateGuard[0].Save(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_aSampler0StateGuard[1].Save(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_aSampler0StateGuard[2].Save(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		m_aSampler0StateGuard[3].Save(0, D3DSAMP_MIPMAPLODBIAS, 0);

		m_aSampler1StateGuard[0].Save(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_aSampler1StateGuard[1].Save(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_aSampler1StateGuard[2].Save(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		m_aSampler1StateGuard[3].Save(1, D3DSAMP_MIPMAPLODBIAS, 0);

		m_aSampler2StateGuard[0].Save(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_aSampler2StateGuard[1].Save(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_aSampler2StateGuard[2].Save(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		m_aSampler2StateGuard[3].Save(2, D3DSAMP_MIPMAPLODBIAS, 0);

		m_ViewportGuard.Save();
		break;
	default:
		ASSERT(0);
		break;
	};

	m_DrawState = DrawState;
}

#if defined(DEBUG) | defined(_DEBUG)
LPDIRECT3DDEVICE9 KG3DUI::GetDevice()
{
	return g_pd3dDevice;
}
#endif


