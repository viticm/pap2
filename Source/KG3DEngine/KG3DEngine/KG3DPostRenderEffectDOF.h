#ifndef _KG3DPOSTRENDEREFFECTDOF_H_
#define _KG3DPOSTRENDEREFFECTDOF_H_

#include "KG3DPostRenderEffectBase.h"

#define DOF_FX_FILE TEXT("Data\\public\\shader\\DOF.fx")
namespace PostRender {
	class KG3DPostRenderEffectDOF : public KG3DPostRenderEffectBase
	{
		friend class KG3DPostRenderEffectManager;
	public:
		KG3DPostRenderEffectDOF(KG3DPostRenderEffectManager *pMgr);
		virtual ~KG3DPostRenderEffectDOF(void);
	public :
		virtual HRESULT Enable(BOOL bEnable);
		virtual HRESULT ApplyEffect(KG3DScene* pScene);
		virtual HRESULT ReloadFX();

		virtual HRESULT OnLostDevice();
		virtual HRESULT OnResetDevice();

		void    CleanUp();
	public :
		LPD3DXEFFECT        m_pEffect;


	private :
		HRESULT InitTextures();
		HRESULT UnInitTextures();

		HRESULT ComputeDOFDepth();
		HRESULT Scene_To_SceneScaled(); 
		HRESULT ScaledGaussianBlur();
		HRESULT GetBackBufferDesc(D3DSURFACE_DESC& TexDesc);
		HRESULT GetTextureCoords(  
			PDIRECT3DTEXTURE9 pTexSrc,  
			RECT*             pRectSrc, 
			PDIRECT3DTEXTURE9 pTexDest, 
			RECT*             pRectDest, 
			CoordRect*        pCoords 
			);

		DWORD m_dwCropWidth;
		DWORD m_dwCropHeight;
		DWORD m_dwWidth;
		DWORD m_dwHeight;
		PDIRECT3DTEXTURE9  m_pTexDOFDepth; 
		PDIRECT3DTEXTURE9  m_pTexSceneScaled;  
		PDIRECT3DTEXTURE9  m_pBlurTexture;
		HRESULT DrawFullScreenQuad2TexCoord(CoordRect c,CoordRect c2);
		HRESULT DrawFullScreenQuad2TexCoord(
			float fLeftU, float fTopV, float fRightU, float fBottomV,float fLeftU2, float fTopV2, float fRightU2, float fBottomV2
			);
		float GetGaussianDistribution( float x, float y, float rho );
		void GetGaussianOffsets( bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
			D3DXVECTOR2* vSampleOffsets, float* fSampleWeights );
	};
}
#endif