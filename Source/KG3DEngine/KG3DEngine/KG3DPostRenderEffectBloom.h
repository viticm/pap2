////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectBloom.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-20 11:53:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPOSTRENDEREFFECTBLOOM_H_
#define _INCLUDE_KG3DPOSTRENDEREFFECTBLOOM_H_

#include "KG3DPostRenderEffectBase.h"
#include "KG3DPostRenderEffectHDR.h"

#define BLOOM_FX_FILE TEXT("Data\\public\\shader\\Bloom.fx")
#define DETAIL_FX_FILE TEXT("Data\\public\\shader\\Detail.fx")

namespace PostRender
{
    class KG3DPostRenderEffectManager;

    class KG3DPostRenderEffectBloom : public KG3DPostRenderEffectBase
    {
        friend class KG3DPostRenderEffectManager;

    private :
        HRESULT GetBackBufferDesc(D3DSURFACE_DESC& desc);
        HRESULT Scene_To_SceneScaled(); 
        HRESULT BrightPass_To_StarSource();
        HRESULT StarSource_To_BloomSource();
        HRESULT RenderBloom();

        HRESULT ClearTexture(LPDIRECT3DTEXTURE9 pTexture);
        static HRESULT DrawFullScreenQuad(
            float fLeftU, 
            float fTopV,
            float fRightU, 
            float fBottomV
            );
        void    CleanUp();

    public :
		static HRESULT DrawFullScreenQuad(CoordRect c);
        KG3DPostRenderEffectBloom(KG3DPostRenderEffectManager *pMgr);
        virtual ~KG3DPostRenderEffectBloom();

        LPDIRECT3DTEXTURE9 GetBloomTexture() { return m_lpBloomTexture_2; }

	    virtual HRESULT ReloadFX();
	    virtual HRESULT ApplyEffect(KG3DScene* pScene);
	    virtual HRESULT Enable(BOOL bEnable);

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

    private:
        HRESULT InitTextures();
        HRESULT UnInitTextures();

        UINT    GetPass();

    public :
        LPDIRECT3DTEXTURE9  m_lpBloomTexture_1;
        LPDIRECT3DTEXTURE9  m_lpBloomTexture_2;
        LPD3DXEFFECT        m_pEffect;

        DWORD m_dwCropWidth;
        DWORD m_dwCropHeight;
        
    };


	class KG3DPostRenderEffectDetail: public KG3DPostRenderEffectBase
	{
		friend class KG3DPostRenderEffectManager;

	public:
		KG3DPostRenderEffectDetail(KG3DPostRenderEffectManager *pMgr);
		virtual ~KG3DPostRenderEffectDetail();

		void CleanUp();

		virtual HRESULT GetBackBufferDesc(D3DSURFACE_DESC& TexDesc);
		virtual HRESULT ReloadFX();
		virtual HRESULT ApplyEffect(KG3DScene* pScene);
		virtual HRESULT Enable(BOOL bEnable);

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

    private:
		HRESULT RenderLocalAverage1(LPDIRECT3DTEXTURE9 pSrc,LPDIRECT3DTEXTURE9 pDest);
		HRESULT RenderTextureness1(LPDIRECT3DTEXTURE9 pDestTextureness,LPDIRECT3DTEXTURE9 pSrcText,LPDIRECT3DTEXTURE9 pSrcAverage);

		HRESULT RenderLocalAverage();
		HRESULT RenderTextureness();

		HRESULT RenderDetail();

        HRESULT InitTextures();
        HRESULT UnInitTextures();

	private:
		LPDIRECT3DTEXTURE9 m_lpTextureLocalAverage0;//本地平均
		LPDIRECT3DTEXTURE9 m_lpTextureness0;//差量

		LPDIRECT3DTEXTURE9 m_lpTextureLocalAverage1;//本地平均
		LPDIRECT3DTEXTURE9 m_lpTextureness1;//差量

		LPDIRECT3DTEXTURE9 m_lpTextureLocalAverage2;//本地平均
		LPDIRECT3DTEXTURE9 m_lpTextureness2;//差量

		LPD3DXEFFECT       m_pEffect;

		DWORD m_dwCropWidth;
		DWORD m_dwCropHeight;
    };
};

#endif //_INCLUDE_KG3DPOSTRENDEREFFECTBLOOM_H_
