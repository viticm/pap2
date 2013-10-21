////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectHSI.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-30 13:02:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPOSTRENDEREFFECTHSI_H_
#define _INCLUDE_KG3DPOSTRENDEREFFECTHSI_H_

#include "KG3DPostRenderEffectBase.h"

namespace PostRender {

    class KG3DPostRenderEffectManager;
    class KG3DPostRenderEffectHSI : public KG3DPostRenderEffectBase
    {
        friend class KG3DPostRenderEffectManager;
    public :
        KG3DPostRenderEffectHSI(KG3DPostRenderEffectManager* pManager);
        virtual ~KG3DPostRenderEffectHSI();    

        HRESULT SetTechnique(LPCSTR Name);
        HRESULT ApplyEffect();
        HRESULT ReloadFX();

    public :
        LPDIRECT3DTEXTURE9  m_lpRenderTargetTexture;
        D3DXHANDLE          m_TechniqueHandel;
    };
}

#endif //_INCLUDE_KG3DPOSTRENDEREFFECTHSI_H_
