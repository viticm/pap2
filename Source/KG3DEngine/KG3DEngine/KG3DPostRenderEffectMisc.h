////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectMisc.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-4 15:11:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPOSTRENDEREFFECTMISC_H_
#define _INCLUDE_KG3DPOSTRENDEREFFECTMISC_H_

#include "KG3DPostRenderEffectBase.h"

namespace PostRender {

    class KG3DPostRenderEffectManager;
    class KG3DPostRenderEffectMisc : public KG3DPostRenderEffectBase
    {
        friend class KG3DPostRenderEffectManager;
    public :
        KG3DPostRenderEffectMisc(KG3DPostRenderEffectManager* pManager);
        virtual ~KG3DPostRenderEffectMisc();    

        
        HRESULT SetTechnique(LPCSTR Name);
        HRESULT ApplyEffect(KG3DScene* pScene);
        HRESULT ReloadFX();

    public :
        LPDIRECT3DTEXTURE9  m_lpRenderTargetTexture;
        D3DXHANDLE m_TechniqueHandel;
    };
}

#endif //_INCLUDE_KG3DPOSTRENDEREFFECTMISC_H_
