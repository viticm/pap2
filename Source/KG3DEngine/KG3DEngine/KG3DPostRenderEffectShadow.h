////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectShadow.h
//  Version     : 1.0
//  Creator     : Zhao Xinyu
//  Create Date : 
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPOSTRENDEREFFECTSHADOW_H_
#define _INCLUDE_KG3DPOSTRENDEREFFECTSHADOW_H_

////////////////////////////////////////////////////////////////////////////////


#include "KG3DPostRenderEffectBase.h"

class KG3DSceneShadowMap;
class KG3DPostRenderEffectManager;
class KG3DCamera;

#define SHADOW_FXFILE_PATH "Data\\Public\\Shader\\ZPostRender.fx"

namespace PostRender
{
    class KG3DPostRenderEffectShadow : public KG3DPostRenderEffectBase
    {
    public :
        KG3DPostRenderEffectShadow(KG3DPostRenderEffectManager* pMgr);
        virtual ~KG3DPostRenderEffectShadow(void);

        virtual HRESULT Enable(BOOL bEnable);
        virtual HRESULT ApplyEffect(KG3DScene* pScene);
        virtual HRESULT ReloadFX();

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

        HRESULT ProcessShadowMap();
        HRESULT ProcessPostRenderShadow();

        LPDIRECT3DTEXTURE9 GetDepthTexture()   { return m_lpDepthTexture; }

		HRESULT SetParentScene(KG3DScene* pScene)
		{
			m_lpParentScene = pScene;
			return S_OK;
		}
    private:
        HRESULT InitTextures();
        HRESULT UnInitTextures();
		HRESULT InitJitter();
		HRESULT UnInitJitter();
		float GetGaussianDistribution( float x, float y, float rho );
		void GetGaussianOffsets( bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
			D3DXVECTOR2* vSampleOffsets, float* fSampleWeights );

    public:
        KG3DSceneShadowMap* m_lpShadowMap;

    private:
        LPDIRECT3DTEXTURE9  m_lpDepthTexture;   // use the color texture to store depth
        LPDIRECT3DSURFACE9  m_lpDepthSurface;

		LPDIRECT3DTEXTURE9  m_pBlurTexture;
		LPDIRECT3DSURFACE9  m_pSMColorSurface;
		LPDIRECT3DSURFACE9  m_pTempDepthSurface;

	    LPDIRECT3DVOLUMETEXTURE9 m_lpJitterTexture;
		float m_fSoftness;
		float m_fJitter;

        DWORD m_dwShadowmapSize;

        LPD3DXEFFECT m_lpEffect;
		KG3DScene*   m_lpParentScene;

		DWORD              m_dwTargetWidth;
		DWORD              m_dwTargetHeight;
		BOOL         m_bJustResetEnable;
    };
}

#endif //_INCLUDE_KG3DPOSTRENDEREFFECTSHADOW_H_
