
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectSSAO.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : Update 2007-07-05 12:49:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DPOSTRENDEREFFECTSSAO_H_
#define _KG3DPOSTRENDEREFFECTSSAO_H_

#include "KG3DPostRenderEffectBase.h"

namespace PostRender {

    class KG3DPostRenderEffectSSAO : public KG3DPostRenderEffectBase
    {
    public :
        virtual HRESULT Enable(BOOL bEnable);
        virtual HRESULT ApplyEffect(KG3DScene* pScene);
        virtual HRESULT ReloadFX();

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

        void    CleanUp();

        LPDIRECT3DTEXTURE9 GetSSAOTexture()     { return m_pSSAOTexture; }

    public :
        KG3DPostRenderEffectSSAO(KG3DPostRenderEffectManager* pMgr);
        virtual ~KG3DPostRenderEffectSSAO();

    private:
        HRESULT InitTextures();
        HRESULT UnInitTextures();

    private :
        LPDIRECT3DTEXTURE9 m_pBlurTexture;
        LPDIRECT3DSURFACE9 m_pBlurSurface;
        LPDIRECT3DTEXTURE9 m_pSSAOTexture;
        LPDIRECT3DSURFACE9 m_pSSAOSurface;

		LPDIRECT3DTEXTURE9 m_pNoiseTexture;

        D3DVIEWPORT9       m_Viewport;

        DWORD              m_dwTargetWidth;
        DWORD              m_dwTargetHeight;

        float              m_fBlurWeight;
    };

}

#endif