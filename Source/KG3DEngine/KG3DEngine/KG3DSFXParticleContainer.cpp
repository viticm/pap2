////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXParticleContainer.h"
#include "KG3DSFX.h"
#include "KG3DSfxMeteorParticle.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DSFXParticleContainer::SortedRectParticle::SortedRectParticle()
{
    for (UINT i = 0; i < SFX_BM_NUM; ++i)
        m_NextFreeIndex[i] = 0;
}

KG3DSFXParticleContainer::SortedRectParticle::~SortedRectParticle()
{
    for (UINT i = 0; i < SFX_BM_NUM; ++i)
    {
        for (size_t j = 0; j < m_vecRectParticle[i].size(); ++j)
            SAFE_DELETE(m_vecRectParticle[i][j]);
        m_vecRectParticle[i].clear();
    }
}

void KG3DSFXParticleContainer::SortedRectParticle::Clear()
{
    for (UINT i = 0; i < SFX_BM_NUM; ++i)
        m_NextFreeIndex[i] = 0;
}

KG3DSFXParticle* KG3DSFXParticleContainer::SortedRectParticle::NewParticle(UINT uBlendMode, WORD wType)
{
    ASSERT(uBlendMode < SFX_BM_NUM);

    size_t uPoolSize = m_vecRectParticle[uBlendMode].size();
    if (m_NextFreeIndex[uBlendMode] >= uPoolSize)
    {
        switch (wType)
        {
        case PARTICLE_TYPE_NORMAL :
            for (size_t i = 0; i < PREALLOCATE_PARTICLE_NUMBER; ++i)
                m_vecRectParticle[uBlendMode].push_back(new KG3DSFXParticle((UINT)uPoolSize + i));
            break;
        case PARTICLE_TYPE_METEOR :
            for (size_t i = 0; i < PREALLOCATE_PARTICLE_NUMBER; ++i)
                m_vecRectParticle[uBlendMode].push_back(new KG3DSfxMeteorParticle((UINT)uPoolSize + i));
            break;
        default :
            ASSERT(false);
            break;
        }
    }

    KG3DSFXParticle* pParticle = m_vecRectParticle[uBlendMode][m_NextFreeIndex[uBlendMode]++];
    if (pParticle->GetPatricleType() == wType)
        return pParticle;

    UINT uPoolIndex = pParticle->m_uPoolIndex;
    SAFE_DELETE(pParticle);

    switch (wType)
    {
    case PARTICLE_TYPE_NORMAL :
        pParticle = new KG3DSFXParticle(uPoolIndex);
        break;
    case PARTICLE_TYPE_METEOR :
        pParticle = new KG3DSfxMeteorParticle(uPoolIndex);
        break;
    default :
        ASSERT(false);
        break;
    }

    m_vecRectParticle[uBlendMode][m_NextFreeIndex[uBlendMode] - 1] = pParticle;

    return pParticle;
}

void KG3DSFXParticleContainer::SortedRectParticle::UpdateParticle(float fCurrFrame)
{
    for (UINT i = 0; i < SFX_BM_NUM; ++i)
    {
        UINT index = 0;
        while (index < m_NextFreeIndex[i])
        {
            KG3DSFXParticle* pCurParticle = m_vecRectParticle[i][index];
            if (FAILED(pCurParticle->Update(fCurrFrame)))
            {
                KG3DSFXParticle* pEndParticle = m_vecRectParticle[i][m_NextFreeIndex[i] - 1];
                swap(pCurParticle->m_uPoolIndex, pEndParticle->m_uPoolIndex);
                swap(m_vecRectParticle[i][m_NextFreeIndex[i] - 1], m_vecRectParticle[i][index]);
                --m_NextFreeIndex[i];
            }
            else
            {
                ++index;
            }
        }
    }
}

KG3DSFXParticleContainer::KG3DSFXParticleContainer()
{
    m_dwActiveNumber	= 0;
    m_uPrimitiveNum		= 0;
    m_pRectangleVBData  = NULL;
    m_uSfxTextNum       = 0;
}

KG3DSFXParticleContainer::~KG3DSFXParticleContainer(void)
{
    for (size_t i = 0; i < m_vecSortedRectParticle.size(); ++i)
        SAFE_DELETE(m_vecSortedRectParticle[i]);
    m_vecSortedRectParticle.clear();
}

LPDIRECT3DVERTEXBUFFER9 KG3DSFXParticleContainer::m_spVertexBuffer = NULL;
int KG3DSFXParticleContainer::m_sVectorNumber = 512;

HRESULT KG3DSFXParticleContainer::OnStaticInvalidDevice()
{
    SAFE_RELEASE(m_spVertexBuffer);
    return S_OK;
}

HRESULT KG3DSFXParticleContainer::OnStaticRestoreDevice()
{
	DWORD dwThreadID = GetCurrentThreadId();
	ASSERT(dwThreadID == g_dwMainThreadID);

    return g_pd3dDevice->CreateVertexBuffer(
        m_sVectorNumber * sizeof(VFormat::_Faces_Diffuse_Texture1),
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
        D3DFVF_Faces_Diffuse_Texture1,
        D3DPOOL_DEFAULT,
        &m_spVertexBuffer,
        NULL 
        );
}

void KG3DSFXParticleContainer::Reset()
{
    for (size_t i = 0; i < m_vecSortedRectParticle.size(); ++i)
        m_vecSortedRectParticle[i]->Clear();
}

void KG3DSFXParticleContainer::SetRenderState(KG3DRenderState& R, UINT eBlendMode)
{
    switch (eBlendMode)
    {
    case SFX_BM_SRCALPHA_INVSRCALPHA :
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;

    case SFX_BM_SRCALPHA_ONE :
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;

    case SFX_BM_SRCALPHA_INVSRCCOLOR :
        R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
        R.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        R.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        break;

    case SFX_BM_SRCALPHA_SCREEN :
        R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        R.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        R.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
        break;

    default :
        ASSERT(false);
        break;
    }
}

HRESULT KG3DSFXParticleContainer::RenderParticle(const KG3DSFX* pSfx, float fCurrentFrame)
{
    ASSERT(pSfx);

    if (!m_spVertexBuffer || !pSfx)
        return E_FAIL;

    KG3DTexture *pTexture = NULL;
    KG3DRenderState R;

    m_dwActiveNumber = 0;


    /*
     * update all acitve particle  
     */

    UpdateParticle(fCurrentFrame);
 

    /*
     * render, order by texture, blend mode
     */

    g_pd3dDevice->SetStreamSource(0, m_spVertexBuffer, 0, sizeof(VFormat::_Faces_Diffuse_Texture1));
    g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);

    for (size_t i = 0; i < m_uSfxTextNum; ++i)
    {
        pSfx->GetTexture((UINT)i, &pTexture);

        if (pTexture && pTexture->IsResourceReady())
		{
			pTexture->SetTexture(g_pd3dDevice,0);

			for (UINT j = 0; j < SFX_BM_NUM; ++j)
			{
				size_t uRectNum = 0;

				SetRenderState(R, j);

				PrepareVertexBuffer(m_vecSortedRectParticle[i]->m_vecRectParticle[j], 
					m_vecSortedRectParticle[i]->m_NextFreeIndex[j], &uRectNum);

				if (m_uPrimitiveNum == 0)
				{
					R.Restore();
					continue;
				}

				ASSERT(m_uPrimitiveNum == uRectNum* 2);
				ASSERT(m_uPrimitiveNum < 65535);

				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_uPrimitiveNum);

				m_dwActiveNumber += m_uPrimitiveNum;
				m_uPrimitiveNum  = 0;

				R.Restore();
			}
		}
    }

    g_pd3dDevice->SetStreamSource(0, 0, 0, 0);

    return S_OK;
}

UINT KG3DSFXParticleContainer::GetParticleCount() const
{
    UINT uNum = 0;

    for(UINT i = 0; i < m_uSfxTextNum; ++i)
        for (UINT j = 0; j < SFX_BM_NUM; ++j)
            for (UINT t = 0; t < m_vecSortedRectParticle[i]->m_NextFreeIndex[j]; ++t)
                uNum += (UINT)m_vecSortedRectParticle[i]->m_vecRectParticle[j][t]->GetRectNum();

    return uNum;
}

HRESULT KG3DSFXParticleContainer::PushRectangleToVB(D3DXVECTOR3* pVecter1,
                                                    D3DXVECTOR3* pVecter2,
                                                    D3DXVECTOR3* pVecter3,
                                                    D3DXVECTOR3* pVecter4,
                                                    D3DCOLOR     dwColor,
                                                    FRECT*       pTexCoord
                                                    )
{
    ASSERT(pVecter1);
    ASSERT(pVecter2);
    ASSERT(pVecter3);
    ASSERT(pVecter4);
    ASSERT(pTexCoord);

	KGLOG_PROCESS_ERROR(m_pRectangleVBData);
	ASSERT((int) m_uPrimitiveNum*3+5 < m_sVectorNumber);
	KGLOG_PROCESS_ERROR( (int) m_uPrimitiveNum*3+5 < m_sVectorNumber );
	
    m_pRectangleVBData[m_uPrimitiveNum * 3].p       = (*pVecter1);
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 1].p   = (*pVecter2);
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 2].p   = (*pVecter3);

    m_pRectangleVBData[m_uPrimitiveNum * 3 + 3].p   = (*pVecter1);
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 4].p   = (*pVecter3);
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 5].p   = (*pVecter4);

    m_pRectangleVBData[m_uPrimitiveNum * 3].diffuse = dwColor;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 1].diffuse = dwColor;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 2].diffuse = dwColor;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 3].diffuse = dwColor;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 4].diffuse = dwColor;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 5].diffuse = dwColor;

    m_pRectangleVBData[m_uPrimitiveNum * 3].tu1 = pTexCoord->Left;
    m_pRectangleVBData[m_uPrimitiveNum * 3].tv1 = pTexCoord->Bottom - pTexCoord->Height;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 1].tu1 = pTexCoord->Left + pTexCoord->Width;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 1].tv1 = pTexCoord->Bottom - pTexCoord->Height;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 2].tu1 = pTexCoord->Left + pTexCoord->Width;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 2].tv1 = pTexCoord->Bottom;

    m_pRectangleVBData[m_uPrimitiveNum * 3 + 3].tu1 = pTexCoord->Left;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 3].tv1 = pTexCoord->Bottom - pTexCoord->Height;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 4].tu1 = pTexCoord->Left + pTexCoord->Width;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 4].tv1 = pTexCoord->Bottom;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 5].tu1 = pTexCoord->Left;
    m_pRectangleVBData[m_uPrimitiveNum * 3 + 5].tv1 = pTexCoord->Bottom;

    m_uPrimitiveNum += 2;

    return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSFXParticleContainer::PrepareVertexBuffer(const vector<KG3DSFXParticle*>& vecParticles, size_t uSize, size_t* pRectNum)
{
	DWORD dwID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(dwID==g_dwMainThreadID);

    ASSERT(m_spVertexBuffer);
    
    if (uSize <= 0)
        return S_OK;

    size_t uRectNum = 0;

    for (size_t i = 0; i < uSize; ++i)
        uRectNum += vecParticles[i]->GetRectNum();

    if (pRectNum)
        *pRectNum = uRectNum;

	if (g_pd3dDevice->TestCooperativeLevel() == D3D_OK)
	{
		if (static_cast<int>(uRectNum * 6) > m_sVectorNumber)
		{
			m_sVectorNumber = static_cast<int>(uRectNum * 6);

			DWORD dwThreadID = GetCurrentThreadId();
			ASSERT(dwThreadID == g_dwMainThreadID);

			SAFE_RELEASE(m_spVertexBuffer);  
			if (FAILED(g_pd3dDevice->CreateVertexBuffer(
				m_sVectorNumber * sizeof(VFormat::_Faces_Diffuse_Texture1),
				D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
				D3DFVF_Faces_Diffuse_Texture1,
				D3DPOOL_DEFAULT,
				&m_spVertexBuffer,
				NULL ))
				)
				return E_FAIL;
			g_pd3dDevice->SetStreamSource(0, m_spVertexBuffer, 0, sizeof(VFormat::_Faces_Diffuse_Texture1));
		}

		if (FAILED(m_spVertexBuffer->Lock(0, (UINT)uRectNum * 6 * sizeof(VFormat::_Faces_Diffuse_Texture1), (void**)&m_pRectangleVBData, D3DLOCK_DISCARD)))
			return E_FAIL;

		for (size_t i = 0; i < uSize; ++i)
			vecParticles[i]->PrepareVertexBuffer(this);

		m_spVertexBuffer->Unlock();
	}

Exit0:
    return S_OK;
}

HRESULT KG3DSFXParticleContainer::UpdateParticle(float fCurrentFrame)
{
    for (size_t i = 0; i < m_vecSortedRectParticle.size(); ++i)
        m_vecSortedRectParticle[i]->UpdateParticle(fCurrentFrame);

    return S_OK;
}

HRESULT KG3DSFXParticleContainer::SetTextureNum(UINT uTextureNum)
{
    if (m_uSfxTextNum == uTextureNum)
        return S_OK;

    for (UINT i = uTextureNum; i < m_uSfxTextNum; ++i)
        SAFE_DELETE(m_vecSortedRectParticle[i]);

    m_vecSortedRectParticle.resize(uTextureNum);

    for (UINT i = m_uSfxTextNum; i < uTextureNum; ++i)
        m_vecSortedRectParticle[i] = new SortedRectParticle();

    m_uSfxTextNum = uTextureNum;
    
    return S_OK;
}

KG3DSFXParticle* KG3DSFXParticleContainer::NewParticle(UINT uTexIndex, UINT uBlendMode, WORD wType)
{
    //ASSERT(uTexIndex < m_uSfxTextNum);
    
    if (uTexIndex >= m_uSfxTextNum)
        uTexIndex = m_uSfxTextNum - 1; 

    return m_vecSortedRectParticle[uTexIndex]->NewParticle(uBlendMode, wType);
}
