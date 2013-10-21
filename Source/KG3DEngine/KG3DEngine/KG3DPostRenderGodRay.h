////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderGodRay.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : Update 2007-07-05 12:49:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DPOSTRENDERGODRAY_H_
#define _KG3DPOSTRENDERGODRAY_H_

#include "KG3DPostRenderEffectBase.h"

class KG3DSceneShadowMap;
class KG3DCamera;

namespace PostRender {

    class KG3DPostRenderGodRay : public KG3DPostRenderEffectBase
    {
        friend class KG3DPostRenderEffectManager;
    public :
        KG3DPostRenderGodRay(KG3DPostRenderEffectManager* pMgr);
        virtual ~KG3DPostRenderGodRay();

    public :
        virtual HRESULT Enable(BOOL bEnable);
        virtual HRESULT ApplyEffect(KG3DScene* pScene);
        virtual HRESULT ReloadFX();

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

        void    CleanUp();

        LPDIRECT3DTEXTURE9 GetRayTexture()  { return m_pRayTarget; }
        LPDIRECT3DTEXTURE9 GetTmpTexture()  { return m_pTmpTarget; }
        
        bool    BeActive()                  { return m_bActive;    };

    private :
        bool    BeActive(KG3DScene* pScene);

        HRESULT InitTextures();
        HRESULT UnInitTextures();

    private :
        LPDIRECT3DTEXTURE9 m_pRayTarget;
        LPDIRECT3DSURFACE9 m_pRaySuface;
        LPDIRECT3DSURFACE9 m_pDepthStencilSurface;

        LPDIRECT3DTEXTURE9 m_pTmpTarget;
        LPDIRECT3DSURFACE9 m_pTmpSuface;

        DWORD              m_dwTargetWidth;
        DWORD              m_dwTargetHeight;

        int                m_nSample;
        bool               m_bActive;

        float              m_fScal;

        D3DVIEWPORT9       m_Viewport;
    };
}

#endif