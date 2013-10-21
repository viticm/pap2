////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectManager.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : Update 2006-12-31 12:49:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPOSTRENDEREFFECTMANAGER_H_
#define _INCLUDE_KG3DPOSTRENDEREFFECTMANAGER_H_

#include "../../../Include/KG3DEngine/KG3DTypes.h"
#include "KG3DPostRenderEffectBloom.h" 
#include "KG3DPostRenderEffectShadow.h"
#include "KG3DPostRenderEffectShockWave.h"
#include "KG3DPostRenderGodRay.h"
#include "KG3DPostRenderEffectSSAO.h"
#include "KG3DPostRenderEffectAoe.h"
#include "KG3DPostRenderEffectDOF.h"

#define SCREEN_EFFECT_FX_PATH  TEXT("Data\\public\\shader\\PostRenderFinalPass.fx")
#define Curve_Stype_Name_Size 32

extern KG3DEngineEditorOption g_eEngineOption;

class KG3DSceneSceneEditor;
class KG3DSFX;

namespace PostRender
{        
    class KG3DPostRenderEffectManager
    {
        friend class KG3DPostRenderEffectBase;
        friend class KG3DPostRenderEffectBloom;
        friend class KG3DPostRenderEffectHDR;
        friend class KG3DPostRenderEffectShockWave;
        friend class KG3DPostRenderEffectDetail;
        friend class KG3DPostRenderEffectShadow;
        friend class KG3DPostRenderGodRay;
        friend class KG3DPostRenderEffectSSAO;
        friend class KG3DPostRenderEffectAoe;
		friend class KG3DPostRenderEffectDOF;

    protected :

        LPDIRECT3DTEXTURE9  m_lpDepthTexture;        // depth texture use to shadow and motion blue
        LPDIRECT3DSURFACE9  m_lpDepthTextureSave;    // depth texture use to save old depth surface 
        LPDIRECT3DTEXTURE9  m_lpRenderTargetTexture; // render targer texture use to post render
        LPDIRECT3DSURFACE9  m_lpRenderTargetSave;    // render targer use to save old render targer
        LPDIRECT3DTEXTURE9  m_lpShareRenderTarger;   // the share render use to each effect
        LPDIRECT3DTEXTURE9  m_lpSwapTexture;         // the texture use to swap render target
		LPDIRECT3DSURFACE9  m_lpMultiSampleSurface;
		LPDIRECT3DSURFACE9  m_lpMultiSampleDepth;
		LPDIRECT3DTEXTURE9  m_lpNoMultiSampleDepth; //  For MultiSample PostRender Depth;
		//LPDIRECT3DSURFACE9  m_lpNoMultiSampleDepthSave; //  For MultiSample PostRender Depth Save;
        LPDIRECT3DTEXTURE9  m_lpCurveTexture;       

		//LPDIRECT3DSURFACE9  m_lpDepthStencilBuffer;
       
        D3DVIEWPORT9        m_ViewportSave;          // store the view port begin post effect apply   
        D3DXMATRIX          m_matPrevViewProj;

	    DWORD               m_dwPostRenderWidth;
	    DWORD               m_dwPostRenderHeight;

        KG3DPostRenderEffectBloom     m_BloomEffect;
		KG3DPostRenderEffectDetail    m_DetailEffect;
        KG3DPostRenderEffectShadow    m_ShadowEffect;
        KG3DPostRenderEffectShockWave m_ShockWareEffect;
        KG3DPostRenderGodRay          m_GodRayEffect;
        KG3DPostRenderEffectSSAO      m_SSAOEffect;
        KG3DPostRenderEffectAoe       m_AoeEffect;
		KG3DPostRenderEffectHDR       m_HDREffect;
		KG3DPostRenderEffectDOF       m_DOFEffect;

        BOOL                m_bSaveToFile;
        DWORD               m_dwCurrentFrame;
        BOOL                m_bEnable;

        float               m_fScalw;
        float               m_fScalh;

    public :
		//LPD3DXEFFECT        m_pEffect;//<PostRenderShadowMove>
        float               m_fExposure;
        float               m_fExposureSave;         
        DWORD               m_dwPrevTime;
        float               m_fAngel;
        DWORD               m_dwAlphaEnable;
        DWORD               m_dwCurrType;
        BOOL                m_bTargetRestore;


        struct _ColorCurve
        {
            char szName[Curve_Stype_Name_Size];
            
            vector<D3DXVECTOR2> vRed;
            vector<D3DXVECTOR2> vGreen;
            vector<D3DXVECTOR2> vBlue;

            float  fKC;
            float  fKM;
            float  fKY;
        };

        static vector<_ColorCurve> m_svCurveStype;
        int                 m_nActiveStype;

        static PostRenderParams m_sEffectParams;

    private :
        float               m_fTranDir;
        float               m_fRate;
        float               m_fShockPower;
        int                 m_nShockPowerSgn;

        LPDIRECT3DTEXTURE9  m_pFinalPassTexture;
        LPDIRECT3DTEXTURE9  m_pFinalPassTarget;
		KG3DScene*          m_lpParentScene;
    public :

		HRESULT GetShadowMask(LPDIRECT3DTEXTURE9* ppTexture);       
		/* reset post effect device, etc whene render window is resize
           bEnable  : enable or disable the post effect
           dwWidth  : the render window's width
           dwHeight : the render window's height */
        HRESULT ResetDevice(BOOL bEnable, DWORD dwWidth, DWORD dwHeight, BOOL bFocus,BOOL bScaleOutput);


        /* turn on / off the appointed post effect */
        HRESULT EnablePostEffect(PostRenderEffectType eType, BOOL bEnable,BOOL bScaleOutput);

        
        /* set the post effect render targer and depth targer */
        HRESULT RenderTargetSet(DWORD dwAlphaEnable = FALSE);


        /* restore the main render targer after apply 
           post effect or post effect error return */
        HRESULT RenderTargetRestore();


        HRESULT Render(KG3DScene* pScene);

		HRESULT PreColorRender(KG3DScene* pScene);
		HRESULT PreZRender(KG3DScene* pScene);
		BOOL NeedPreZRender();
		BOOL NeedPreColorRender();

        /* apply effects */
        HRESULT ApplyEffect(PostRenderEffectType eType, KG3DScene* pScene);

        /* aoe is especial */
        HRESULT RenderAoe();


        /* POST_EFFECT_SHADOW      = 1 << 0,
           POST_EFFECT_DETAIL      = 1 << 1,
           POST_EFFECT_SOCKWARE    = 1 << 2,
           POST_EFFECT_GODRAY      = 1 << 3,
           POST_EFFECT_HSI         = 1 << 4,
           POST_EFFECT_BLOOM       = 1 << 5,
           POST_EFFECT_MOTIONBLUE  = 1 << 6
        */
        bool HasEffect(PostRenderEffectType e)      { return !!(m_dwCurrType & e);          }
        bool NotHasEffect(PostRenderEffectType e)   { return !(m_dwCurrType & e);           }
        void Enable(PostRenderEffectType e)         { m_dwCurrType |= e;                    }
        void Disable(PostRenderEffectType e)        { m_dwCurrType &= ~e;                   }
        bool HasNextEffect(DWORD dwType)            { return m_dwCurrType >= (dwType << 1); }

        /* set the render target and texture for the pointer effect 
           and set the effect parameter */        
        HRESULT SetEffectParameter(DWORD dwType, KG3DScene* pScene);

        /* Synchronous post effect option */
        void UpdateEngineOption();


        /* check if need depth texture */
        bool NeedDepthTexture();


        /* check if need share texture */
        bool NeedSwapTexture();


        /* load direct3d effect fx file for post render */
        HRESULT LoadFxFile();
        HRESULT ReloadFX();


        /* Device Invalid and Restore */
        HRESULT OnLostDevice();
        HRESULT OnResetDevice(BOOL bScaleOutput);


        /* create the depth texture */
        HRESULT CreateDepthTexture(int nWidth, int nHeight);


        LPDIRECT3DTEXTURE9 GetDepthTexture()     { return m_lpDepthTexture;         }
        LPDIRECT3DSURFACE9 GetSaveRenderTarget() { return m_lpRenderTargetSave;     }
        LPDIRECT3DTEXTURE9 GetRenderTexture()    { return m_lpRenderTargetTexture;  }
        LPDIRECT3DTEXTURE9 GetShareTexture();
        LPDIRECT3DSURFACE9 GetShareSurface();
        

        BOOL    IsSaveToFile()                  { return m_bSaveToFile;         }
        void    EnableSaveToFile(BOOL bEnable)  { m_bSaveToFile = bEnable;      }
        void    SetCurrentFrame(DWORD dwFrame)  {  m_dwCurrentFrame = dwFrame;  }
        BOOL    IsEnablePostRender()            { return m_bEnable;             }
        DWORD   GetWidth()                      { return m_dwPostRenderWidth;   }
        DWORD   GetHeight()                     { return m_dwPostRenderHeight;  }
        DWORD   GetViewportWidth() const        { return m_ViewportSave.Width;  } 
        DWORD   GetViewportHeight() const       { return m_ViewportSave.Height; }
        void    GetDepthTexCoordScale(float* pU, float* pV);


        /* bloom parameter */
        KG3DPostRenderEffectBloom*  GetBloomEffect() { return &m_BloomEffect; } 

        int SetBloomGate(float fValue)               { m_sEffectParams.fBloomGate = fValue; return true;        }
        int SetBloomExposure1(float fValue)          { m_sEffectParams.fBloomExposure1 = fValue; return true; }
        int SetBloomExposure2(float fValue)          { m_sEffectParams.fBloomExposure2 = fValue; return true; }
        int SetBloomGaussCoefficient(float fValue)   { m_sEffectParams.fBloomGaussCoefficient = fValue; return true; }
        int SetfBloomSampleNum(int nValue)           { m_sEffectParams.nBloomSampleNum = nValue; return true; }
        int SetBloomDelaye(float fValue)             { m_sEffectParams.fBloomDelay = fValue; return true; }

        int SetDetailPower(float fValue)             { m_sEffectParams.fDetailPower = fValue; return true; }
        int SetBlurPower(float fValue)               { m_sEffectParams.fBlurPower = fValue; return true; }
        int SetBlurDistance(float fValue)            { m_sEffectParams.fBlurDistance = fValue; return true; }
        
        /* hsi parameter */
        int SetHSIHueScale(float fValue)              { m_sEffectParams.fHueScale = fValue; return true; }
        int SetHSIHueBias(float fValue)               { m_sEffectParams.fHueBias = fValue; return true;  }
        int SetHSISaturationScale(float fValue)       { m_sEffectParams.fSaturationScale = fValue; return true; }
        int SetHSISaturationBias(float fValue)        { m_sEffectParams.fSaturationBias = fValue; return true; }
        int SetHSIIntensityScale(float fValue)        { m_sEffectParams.fIntensityScale = fValue; return true; }
        int SetHSIIntensityBias(float fValue)         { m_sEffectParams.fIntensityBias = fValue; return true;  }

		int SetShadowSoftness(float fValue)           { m_sEffectParams.fSoftness = fValue; return true; }
		int SetShadowJitter(float fValue)             { m_sEffectParams.fJitter = fValue; return true;  }

        int SetActiveCurveRedPoints(D3DXVECTOR2* pts, int num);
        int SetActiveCurveGreedPoints(D3DXVECTOR2* pts, int num);
        int SetActiveCurveBluePoints(D3DXVECTOR2* pts, int num);

        int SetCMYK_KC(float k);
        int SetCMYK_KM(float k);
        int SetCMYK_KY(float k);

        static int GetStypeName(int index, char szName[]);
        static int SetStypeName(int index, char szName[]);

        D3DXVECTOR2 GetSplinePoint(int stype, int channel, int index);
        int GetSplinePointCount(int stype, int channel);
        static int GetStypeCount()                          { return (int)m_svCurveStype.size(); }
        HRESULT SetActiveStype(int index, int bFlush);
        int     GetActiveStype()                     { return m_nActiveStype; }
        int CloneActiveStype();
        int DelActiveStype();          
        int ResetActiveStype(DWORD channel);         


       
        /* sockware parameter */
        int SetShockWavePower(float fValue)          { m_sEffectParams.fShockWavePower = fValue; return true; }

        int SetGodRaySampleNum(int nValue)           { m_sEffectParams.nGodRaySampleNum = nValue; return true; }
        int SetGodRayDensity(float fValue)           { m_sEffectParams.fGodRayDensity = fValue; return true; }
        int SetGodRayWeight(float fValue)            { m_sEffectParams.fGodRayWeight = fValue; return true; }
        int SetGodRayDecay(float fValue)             { m_sEffectParams.fGodRayDecay = fValue; return true; }
        int SetGodRayExposure(float fValue)          { m_sEffectParams.fGodRayExposure = fValue; return true; }
        int SetGodRayLimitAngle(float fValue)        { m_sEffectParams.fLimitAngle = fValue; return true; }

        /* ssao parameter */
        int SetSSAOlevel(int nLevel)                 { m_sEffectParams.nSSAOLevel = nLevel; return true; }
        int SetSSAOSampRadius(float radius)          { m_sEffectParams.fSSAOSampleRadius = radius; return true; }
        int SetSSAODarkFact(float dark)              { m_sEffectParams.fSSAODarkFact = dark; return true; }
        int SetSSAOBlurWeight(float w)               { m_sEffectParams.fBlurWeight = w; return true; }

		/* fog parameter */
        int SetFogDensityV(float fValue)             { m_sEffectParams.fFogDensityV = fValue; return true; }
        int SetFogSlopeThreshold(float fValue)       { m_sEffectParams.fSlopeThreshold = fValue; return true; }
        int SetFogHeightFalloff(float fValue)        { m_sEffectParams.fHeightFalloff = fValue; return true; }
        int SetFogcolor(D3DXVECTOR4 vecColor)        { m_sEffectParams.vecFogcolor = vecColor; return true; }

		/* HDR parameter */
		int SetHDRBrightGate(float fValue)           { m_sEffectParams.fHDRBrightGate = fValue; return true; }
		int SetHDRRectify(float fValue)              { m_sEffectParams.fHDRRectify = fValue; return true; }
		int SetHDRGrayLevel(float fValue)            { m_sEffectParams.fHDRGrayLevel = fValue; return true; }
		int SetHDRStarType(int fValue)               { m_sEffectParams.nHDRStarType = fValue; m_HDREffect.UpdateStarType();return true; }
		int SetHDREnableBlueShift(BOOL fValue)       { m_sEffectParams.bHDREnableBlueShift = fValue; return true; }
		int SetHDRBloomScale(float fValue)           { m_sEffectParams.fHDRBloomScale = fValue; return true; }
		int SetHDRStarScale(float fValue)            { m_sEffectParams.fHDRStarScale = fValue; return true; }
		int GetHDRStarTypeCount()                    { return m_HDREffect.GetHDRStarTypeCount(); }
		int GetHDRStarName(int index, char szName[]) { return m_HDREffect.GetHDRStarName(index, szName); }

        LPDIRECT3DSURFACE9 GetGodRaySuface()         { return m_GodRayEffect.m_pTmpSuface; }
        D3DXVECTOR4 GetSunScreenPos(KG3DScene* pScene);

		//
		int SetEnabledAreaShadow(BOOL bEnabled)       {m_sEffectParams.bEnableAreaShadow = bEnabled; return true;}

		HRESULT SetParentScene(KG3DScene* pScene);
		KG3DScene* GetParentScene() { return m_lpParentScene; }
			
        static HRESULT StaticConfigLoad();
        static HRESULT StaticConfigSave();

        static HRESULT LoadParams(LPCSTR pszFileName);
        static HRESULT SaveParams(LPCSTR pszFileName);

        static HRESULT LoadCurveStype();
        static HRESULT SaveCurveStype();

    private :

        LPDIRECT3DTEXTURE9 CreateDepth(int, int);//暂时把Width和Height屏蔽，使用BackBuffer大小的Depth，不然有的RT会比Depth大，D3D会出错
		LPDIRECT3DTEXTURE9 CreateHALDepth(int,int);

        /* compute exposure for bloom */
        HRESULT ComputeExposure();

        /* get effect number */
        int GetEffectNum();

        /* draw scene */
        HRESULT Draw(UINT pass, float z);
        HRESULT Draw(UINT pass, float z, D3DVIEWPORT9& vp);

        HRESULT DrawFullRect(UINT pass, const D3DVIEWPORT9& vp, float z);

        int _SetCurveRed(double pts[]);              
        int _SetCurveGreed(double pts[]);             
        int _SetCurveBlue(double pts[]); 

    public:
	    KG3DPostRenderEffectManager();
	    virtual ~KG3DPostRenderEffectManager();
    };

    void  DrawFullScreenQuad(float U1, float V1, float U2, float V2, float fX, float fY, float fWidth, float fHeight, DWORD dwEnableAlpha = FALSE, float z = 0.f);
    void  GetSampleOffsets_DownScale2x2_Lum(DWORD dwSrcWidth, DWORD dwSrcHeight, DWORD dwDestWidth, DWORD dwDestHeight, D3DXVECTOR2 avSampleOffsets[]);
    void  GetSampleOffsets_DownScale3x3(DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[]);
    float GaussianDistribution(float x, float y, float rho);
    void  GetSampleOffsets_Bloom(DWORD dwD3DTexSize, float afTexCoordOffset[15], D3DXVECTOR4* avColorWeight, float fDeviation, float fMultiplier, int num = 16);
};

#endif //_INCLUDE_KG3DPOSTRENDEREFFECTMANAGER_H_
