//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG3DEngineOptionCaps.h
//  Version     :   1.0
//  Creater     :   ShenLiqiao
//  Date        :   2009/3/2 09:18:26
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DENGINEOPTIONCAPS_H_
#define _KG3DENGINEOPTIONCAPS_H_

#include <Windows.h>
#include <vector>
#include "KG3DInterface.h"

class KG3DEngineOptionCaps : IKG3DEngineOptionCaps
{
public:
    KG3DEngineOptionCaps();
    ~KG3DEngineOptionCaps();

    virtual HRESULT GetEngineOptionCaps(KG3DEngineOption& EngineOption);

    HRESULT GetAdaperModeCaps(UINT uAdapter, D3DFORMAT Format);
    BOOL CheckValidateAdaptMode(UINT uWidth, UINT uHeight);

    virtual std::vector<KADAPTERMODE>* GetAdapterModes();
    virtual std::vector<int>* GetMultiSampleType();
    virtual DWORD GetMaxAnisotropy();
    virtual DWORD GetMinAnisotropy();
    virtual BOOL GetRenderGrass();
    virtual BOOL GetGrassAnimation();
    virtual BOOL GetGrassAlphaBlend();
    virtual int  GetMinTextureScale();
    virtual int  GetMaxTextureScale();
    virtual float GetMinCameraDistance();
    virtual float GetMaxCameraDistance();
    virtual float GetMinCameraAngle();
    virtual float GetMaxCameraAngle();
    virtual BOOL GetPostEffectEnable();
    virtual BOOL GetBloomEnable();
    virtual BOOL GetHDREnable();
    virtual BOOL GetDOFEnable();
    virtual BOOL GetShockWaveEnable();
    virtual BOOL GetHSIEnable();
    virtual BOOL GetMotionBlur();
    virtual BOOL GetDetail();
    virtual BOOL GetGodRay();
    virtual BOOL GetSSAO();
    virtual BOOL GetGlobalFog();
    virtual BOOL GetCurveCMYK();
    virtual int*  GetModelShadowType();
    virtual BOOL GetEnableDynamicConverMap();
    virtual BOOL GetCpuSkin();
    virtual int  GetMaxNumCpuThread();
    virtual int  GetMinNumCpuThread();
    virtual BOOL GetFlexBodySmooth();
    virtual int  GetMaxTerrainDetail();
    virtual int  GetMinTerrainDetail();
    virtual int  GetMaxVegetationDensity();
    virtual int  GetMinVegetationDensity();
    virtual int  GetMinWaterDetail();
    virtual int  GetMaxWaterDetail();
    virtual std::vector<int>* GetShaderModel();

protected:

private:

    vector<KADAPTERMODE> m_AdapterModes;
    std::vector<int>     m_vecMutliSampleType;    

    int m_nMinSampMipFilter;
    int m_nMinSampMinFilter;
    int m_nMinSampMagFilter;

    int m_nMaxSampMipFilter;
    int m_nMaxSampMinFilter;
    int m_nMaxSampMagFilter;

    DWORD m_dwMinAnisotropy;
    DWORD m_dwMaxAnisotropy;

    BOOL m_bRenderGrass;				//是否渲染草
    BOOL m_bGrassAnimation;			//是否对草播放动画
    BOOL m_bGrassAlphaBlend;
    int  m_nMinTextureScale;			//材质精度
    int  m_nMaxTextureScale;

    float m_fMinCameraDistance;     //摄像机距离
    float m_fMaxCameraDistance;
    float m_fMinCameraAngle;        //摄像机视角
    float m_fMaxCameraAngle;

    // 如果客户端要提供二次渲染的选项，请在config文件中将此设为1
    BOOL m_bPostEffectEnable;		//是否开启二次渲染

    //二次渲染的选项
    BOOL m_bBloomEnable;			//是否柔化
    BOOL m_bHDREnable;
    BOOL m_bDOFEnable;
    BOOL m_bShockWaveEnable;		//是否ShockWave
    BOOL m_bHSIEnable;				//是否HSI着色
    BOOL m_bMotionBlur;
    BOOL m_bDetail;
    BOOL m_bGodRay;
    BOOL m_bSSAO;
    BOOL m_bGlobalFog;
    BOOL m_bCurveCMYK;

    //模型阴影的选项
    int  m_nModelShadowType[4];		//类型，0表示关闭，1表示用脚底片片，2表示实时光影

    BOOL m_bEnableDynamicConverMap;
    BOOL m_bCpuSkin;

    int  m_nMaxNumCpuThread;
    int  m_nMinNumCpuThread;

    BOOL m_bFlexBodySmooth;//平滑柔体

    int  m_nMaxTerrainDetail;
    int  m_nMinTerrainDetail;
    int  m_nMaxVegetationDensity;
    int  m_nMinVegetationDensity;
    int  m_nMinWaterDetail;
    int  m_nMaxWaterDetail;
    std::vector<int> m_vecShaderModel;

    HRESULT GetMultiSampleTypeListCaps();
    HRESULT GetTextureSamplerFilterCaps(KG3DEngineOption& EngineOption);

    HRESULT GetRenderGrassCaps();
    HRESULT GetGrassAnimationCaps(KG3DEngineOption& EngineOption);
    HRESULT GetGrassAlphaBlendCaps(KG3DEngineOption& EngineOption);
    HRESULT GetTextureScaleCaps();
    HRESULT GetCameraDistanceCaps();
    HRESULT GetCameraAngleCaps();

    HRESULT GetPostEffectCaps(KG3DEngineOption& EngineOption);
    HRESULT GetModelShadowTypeCaps(KG3DEngineOption& EngineOption);
    HRESULT GetEnableDynamicConverMapCaps();
    HRESULT GetCpuSkinCaps();
    HRESULT GetNumCpuThreadCaps();
    HRESULT GetFlexBodySmoothCaps();
    HRESULT GetTerrainDetailCaps();
    HRESULT GetVegetationDensityCaps();
    HRESULT GetWaterDetailCaps();

    UINT    GetAllowableAdapterModeCount(UINT uAdapter, D3DFORMAT Format);
    HRESULT GetAllowableAdapterMode(D3DDISPLAYMODE* pAdapterMode, UINT* puCount, UINT uAdapter, D3DFORMAT Format); 
    HRESULT GetDepthStencilFormat(UINT uAdapter, D3DDEVTYPE DeviceType, D3DFORMAT* pDepthStencilFormat, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat);
    HRESULT GetAdapater(PUINT puAdapter, D3DDEVTYPE* pDeviceType);

    HRESULT GetPostEffectEnableCaps(D3DCAPS9 D3DCaps);
    HRESULT GetBloomEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetHDREnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetDOFEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetShockWaveEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetHSIEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetMotionBlurCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetDetailCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetGodRayCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetSSAOCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetGlobalFogCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);
    HRESULT GetCurveCMYKCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption);

    HRESULT GetShaderModelCaps();
};

#endif  // _KG3DENGINEOPTIONCAPS_H_

extern KG3DEngineOptionCaps g_cEngineOptionCaps;