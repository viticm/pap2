#include "StdAfx.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFXSelectTexture.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DSceneBlock.h"
#include "KG3DSFXMagicParpet.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DTextureTable.h"
#include "KG3DSFXSelectTexture.h"
#include "KG3DCamera.h"
#include "./KG3DGraphiceEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace {

    inline LPDIRECT3DTEXTURE9 SafeGetTexture(KG3DTexture* pTexture)
    {
        if (pTexture)
            return pTexture->GetTexture();

        return NULL;
    }

    template<int texSize> struct RangeSquare
    {
        void FromBBox(const AABBOX& box)
        {
            D3DXVECTOR3 c = box.GetCenter();
            
            center.x = c.x;
            center.y = c.z;

            radius = max(box.GetLength(), box.GetWidth());
            gene = KG3DProjTextureRender::Proj_Tex_Size / radius;
            radius /= 2.f;
        }

        void Ajust(D3DXVECTOR2& cnt, float& rad)
        {
            cnt -= center - D3DXVECTOR2(radius, radius);
            cnt *= gene;
            rad *= gene;
        }
        
        D3DXVECTOR2 center;
        float radius;
        float gene;
    };

    struct ScreenQuad
    {
        D3DXVECTOR4 p;
        D3DCOLOR    c;
        D3DXVECTOR2 t; 

        enum {FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1};
    };

    /*inline void DrawScreenQuad(const D3DXVECTOR2& center, float fRadius, DWORD color)
    { 
        ScreenQuad v[4];

        v[0].p = D3DXVECTOR4(center.x - fRadius - 0.5f, center.y - fRadius - 0.5f, 0.f, 1.f);
        v[0].c = color;
        v[0].t = D3DXVECTOR2(0.f, 0.f);
        v[1].p = D3DXVECTOR4(center.x + fRadius - 0.5f, center.y - fRadius - 0.5f, 0.f, 1.f);
        v[1].c = color;
        v[1].t = D3DXVECTOR2(1.f, 0.f);
        v[2].p = D3DXVECTOR4(center.x - fRadius - 0.5f, center.y + fRadius - 0.5f, 0.f, 1.f);
        v[2].c = color;
        v[2].t = D3DXVECTOR2(0.f, 1.f);
        v[3].p = D3DXVECTOR4(center.x + fRadius - 0.5f, center.y + fRadius - 0.5f, 0.f, 1.f);
        v[3].c = color;
        v[3].t = D3DXVECTOR2(1.f, 1.f);

        g_pd3dDevice->SetFVF(ScreenQuad::FVF);
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(ScreenQuad));

    }*/
}

KG3DProjTextureRender g_ProjTextureRender;

KG3DProjTextureRender::KG3DProjTextureRender()
    : m_pSelector(NULL)
{
}

KG3DProjTextureRender::~KG3DProjTextureRender()
{
    Clear();
}

HRESULT KG3DProjTextureRender::Init()
{
    Clear();
    return S_OK;
}

HRESULT KG3DProjTextureRender::Clear()
{
    m_pSelector = NULL;

    return S_OK;
}

HRESULT KG3DProjTextureRender::CheckInputProjectSource(KG3DModel* mdl)
{
    ASSERT(mdl);

    if (!mdl || mdl->GetType() != MESHTYPE_SFX || mdl->GetAlpha() <= FLT_EPSILON)
        return S_OK;

    KG3DSFX* sfx = (KG3DSFX*)mdl;

    for (DWORD a = 0; a < sfx->GetMagicParpetNum(); ++a)
    {
        KG3DSFXMagicParpet* aoe = (KG3DSFXMagicParpet*)sfx->GetMagicParpet(a);
        AddProjTextureSource(aoe);
    }


    return S_OK;
}

HRESULT KG3DProjTextureRender::AddProjTextureSource(KG3DSFXMagicParpet* pAoe)
{
	HRESULT hr = E_FAIL;

	m_Lock.Lock();

    if (pAoe->GetType() == AOE_SELECTOR)
    {
        m_pSelector = pAoe;
    }
    else if (pAoe->GetType() == AOE_PROJECT)
    {
		KGLOG_PROCESS_ERROR(m_aAoes.size()<Max_Tex_Num);

		for (size_t s=0;s<m_aAoes.size();s++)
		{
			KG_PROCESS_ERROR( m_aAoes[s] != pAoe );
		}
        m_aAoes.push_back(pAoe);
    }
    else
    {

    }
	hr = S_OK;
Exit0:
	m_Lock.Unlock();
    return hr;
}

HRESULT KG3DProjTextureRender::DelProjTextureSource(KG3DSFXMagicParpet* pAoe)
{
	HRESULT hr = E_FAIL;

	m_Lock.Lock();

    if (pAoe == m_pSelector)
    {
        m_pSelector = NULL;
    }
    else
    {
		vector<KG3DSFXMagicParpet*>::iterator i = m_aAoes.begin();
		vector<KG3DSFXMagicParpet*>::iterator iend = m_aAoes.end();
		while (i!=iend)
		{
			KG3DSFXMagicParpet* pParpet = *i;
			if(pParpet == pAoe)
			{
				i = m_aAoes.erase(i);
				break;
			}
			else
			{
				i++;
			}
		}
    }

	hr = S_OK;
//Exit0:
	m_Lock.Unlock();

    return hr;
}

HRESULT KG3DProjTextureRender::RemoveAllProjSource()
{
	m_Lock.Lock();

    m_pSelector = NULL;
	m_aAoes.clear();

	m_Lock.Unlock();
    return S_OK;
}

HRESULT KG3DProjTextureRender::Update()
{
    return S_OK;
}

bool KG3DProjTextureRender::NeedProject(AABBOX& CheckBox)
{
    if (!m_pSelector)
        return false;

    return m_pSelector->CalcBBox().IsTwoBoxIntersect(CheckBox);
}

HRESULT KG3DProjTextureRender::SetEffectParam(LPD3DXEFFECT pEffect)
{
    if (!pEffect)
        return E_FAIL;

    if (m_pSelector)
    {
        D3DXVECTOR3 vCenter = m_pSelector->GetWorldPos();
        pEffect->SetValue("proj_center", &vCenter, sizeof(vCenter));
        pEffect->SetFloat("proj_size", (float)m_pSelector->GetRadius() * 2.f);
        pEffect->SetTexture("SelTexture", SafeGetTexture((KG3DTexture*)m_pSelector->GetTexture()));
    }
    else
    {
        D3DXVECTOR3 vCenter = D3DXVECTOR3(0.f, 10000000.f, 0.f);
		KG3DTexture* pText = g_cTextureTable.GetNullTexture();

        pEffect->SetValue("proj_center", &vCenter, sizeof(vCenter));
        pEffect->SetFloat("proj_size", 0.000001f);//proj_size在里面做除数，不能传0值进去，不然ATI的卡会出现问题（如地表变黑）
		pEffect->SetTexture("SelTexture", pText ? pText->GetTexture() : NULL);
    }

    return S_OK;
}

void KG3DProjTextureRender::GetInfo(D3DXVECTOR4* info, size_t index)
{
    if (!info)
        return;

    if (index < 0 || index >= (int)m_aAoes.size())
        return;

	if(m_aAoes[index])
		m_aAoes[index]->GetInfo(info);
}

void KG3DProjTextureRender::GetDiff(D3DXCOLOR* diff, size_t index)
{
    if (!diff)
        return;

    if (index < 0 || index >= (int)m_aAoes.size())
        return;

    (*diff) = m_aAoes[index]->GetCurrColor();
}
    
void KG3DProjTextureRender::GetLimt(float* limt, size_t index)
{
    if (!limt)
        return;

    if (index < 0 || index >= (int)m_aAoes.size())
        return;

    (*limt) = m_aAoes[index]->GetHeightLimit();
}

void KG3DProjTextureRender::GetTexFrameInfo(D3DXVECTOR4* info, size_t index)
{
    if (!info)
        return;

    if (index < 0 || index >= (int)m_aAoes.size())
        return;

    m_aAoes[index]->GetTexFrameInfo(info);
}

LPDIRECT3DTEXTURE9 KG3DProjTextureRender::GetTexture(size_t index)
{
    if (index < 0 || index >= (int)m_aAoes.size())
        return NULL;

    return SafeGetTexture(static_cast<KG3DTexture*>(m_aAoes[index]->GetTexture()));
}

HRESULT KG3DProjTextureRender::OnLostDevice()
{
    Clear();
    return S_OK;
}

HRESULT KG3DProjTextureRender::OnResetDevice()
{
    Init();
    return S_OK;
}
