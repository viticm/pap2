
////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2008-4-28 17:14:16
//
////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "KG3DSFXParticleContainer.h"
#include "KG3DSfxMeteorParticle.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DSfxMeteorParticle::KG3DSfxMeteorParticle(UINT uIndex)
    : KG3DSFXParticle(uIndex)
{

}


KG3DSfxMeteorParticle::~KG3DSfxMeteorParticle()
{

}


HRESULT KG3DSfxMeteorParticle::OnLaunch(  KG3DSFX*                     pParentSFX,
                                          KG3DSFXParticleLifeTimeLine* pLifeTimeLine,
                                          SFX_PARTICLE_TYPE            eParticleType,
                                          SFX_PARTICLE_UPDATE_MODE     eParticleUpdateMode,
                                          SFX_BLEND_MODE               eBlendMode,
                                          DWORD                        dwTrackIndex, 
                                          float                        fLife,
                                          D3DXVECTOR3*                 pvPosition,
                                          D3DXVECTOR3*                 pvVelocity,
                                          float                        fFrame,
                                          DWORD                        dwRandTexDelay,
                                          KG3DSFXParticleLauncher*     pLauncher
                                          )
{
    __super::OnLaunch(pParentSFX, 
        pLifeTimeLine, 
        eParticleType,
        eParticleUpdateMode, 
        eBlendMode,
        dwTrackIndex,
        fLife, 
        pvPosition,
        pvVelocity, 
        fFrame, 
        dwRandTexDelay, 
        pLauncher);

    m_fMeteorLife = m_fLife;

    m_listMeteor.clear();

    return S_OK;
}


HRESULT KG3DSfxMeteorParticle::Update(float fCurrentFrame)
{
    float fElapsedFrame = m_fPreFrame;

    HRESULT hr = __super::Update(fCurrentFrame);

    fElapsedFrame = m_fPreFrame - fElapsedFrame;

    if (SUCCEEDED(hr))
    {
        if (fElapsedFrame <= 0)
            return S_OK;
    }
    else
    {
        if (!(m_pLifeTimeLine->m_wFlag & PARTICLE_FLAG_KEEP))
            return hr;
    }

    list<_MeteorNode>::iterator pos = m_listMeteor.end();

    for (list<_MeteorNode>::iterator i = m_listMeteor.begin();
        i != m_listMeteor.end(); ++i)
    {
        (*i).fLife -= fElapsedFrame;

        if ((*i).fLife > 0.f && pos == m_listMeteor.end())
            pos = i;
    }

    m_listMeteor.erase(m_listMeteor.begin(), pos);


    if (FAILED(hr) && m_listMeteor.size() <= 0)
        return E_FAIL;

    if (SUCCEEDED(hr))
    {
        _MeteorNode node = {m_vPosition, m_fMeteorLife};
        
        float disSq = FLT_MAX;

        if (m_listMeteor.size() > 0)
            disSq = D3DXVec3LengthSq(&(m_listMeteor.back().vPosition - node.vPosition));

        if (disSq > m_pLifeTimeLine->m_dwSmooth)
            m_listMeteor.push_back(node);
    }

    return S_OK;
}

HRESULT KG3DSfxMeteorParticle::PrepareVertexBuffer(KG3DSFXParticleContainer *pContainer)
{
    D3DXVECTOR3 vCur[4];
    D3DXVECTOR3 vNxt[4];
    FRECT rcTexture;
    int   index = 0;
    D3DXVECTOR3 vWorldPos;
    float fStepHeight = 0.f;

    for (list<_MeteorNode>::reverse_iterator i = m_listMeteor.rbegin();
        i != m_listMeteor.rend(); ++i) 
    {
        list<_MeteorNode>::reverse_iterator next = i;

        if (++next == m_listMeteor.rend())
            break;

        GetWorldPos(&vWorldPos, next->vPosition);

        if (index == 0)
        {
            GetRectInfo(&vNxt[0], &vNxt[1], &vNxt[2], &vNxt[3], &rcTexture, vWorldPos);
            GetWorldPos(&vWorldPos, i->vPosition);
            GetRectInfo(&vCur[0], &vCur[1], &vCur[2], &vCur[3], NULL, vWorldPos);
            fStepHeight = rcTexture.Height / (m_listMeteor.size() - 1);
        }
        else
        {
            GetRectInfo(&vNxt[0], &vNxt[1], &vNxt[2], &vNxt[3], NULL, vWorldPos);
        }

        FRECT rc = FRECT(0.f, fStepHeight * index, rcTexture.Width, fStepHeight);

        D3DXVECTOR3 v1 = (vCur[0] + vCur[3]) * 0.5f;
        D3DXVECTOR3 v2 = (vCur[1] + vCur[2]) * 0.5f;
        D3DXVECTOR3 v3 = (vNxt[1] + vNxt[2]) * 0.5f;
        D3DXVECTOR3 v4 = (vNxt[0] + vNxt[3]) * 0.5f;

        pContainer->PushRectangleToVB(&v1, &v2, &v3, &v4, m_dwColor, &rc);

        memcpy(vCur, vNxt, sizeof(vCur));

        ++index;
    }

    return S_OK;
}

