
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectAoe.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-8-29 10:52:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DPOSTRENDEREFFECT_AOE_H_
#define _KG3DPOSTRENDEREFFECT_AOE_H_

#include "KG3DPostRenderEffectBase.h"

#define Aoe_Effect_File TEXT("Data\\public\\shader\\aoe.fx")

namespace PostRender
{
    class KG3DPostRenderEffectManager;

    class KG3DPostRenderEffectAoe : public KG3DPostRenderEffectBase
    {
        friend class KG3DPostRenderEffectManager;

    private :
        void CleanUp();

    public :
        KG3DPostRenderEffectAoe(KG3DPostRenderEffectManager *pMgr);
        virtual ~KG3DPostRenderEffectAoe(void);

    public :
        virtual HRESULT ReloadFX();
        virtual HRESULT ApplyEffect(KG3DScene* pScene);
        virtual HRESULT ApplyEffectNoChangeTarget();
        virtual HRESULT Enable(BOOL bEnable);

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

        virtual HRESULT ReCreateTexture();
        LPDIRECT3DTEXTURE9 GetResultTexture() { return m_pResultTexture; }

    protected :
        LPDIRECT3DTEXTURE9 m_pResultTexture;
        LPDIRECT3DSURFACE9 m_pResultSurface;
        LPD3DXEFFECT  m_pEffect;
    };
};

#endif
