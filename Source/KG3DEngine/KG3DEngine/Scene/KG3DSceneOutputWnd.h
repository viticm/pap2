#pragma once

#include "KG3DTrackCamera.h"
#include "KG3DPostRenderEffectManager.h"
#include "IEKG3DSceneOutputWnd.h"
#include "../../../Include/KG3DEngine/KG3DTypes.h"

extern KG3DEngineOption g_cEngineOption;
extern KG3DEngineEditorOption g_eEngineOption;
extern BOOL g_bUseVIM3D;
using namespace std;

struct KG3DSceneGlow//全屏柔光效果
{
	DWORD m_GateColor;
	DWORD m_AddColor;

	KG3DSceneGlow()
	{
		m_GateColor = 0xFF808080;
		m_AddColor  = 0x80FFFFFF;
	}
};

class KG3DScene;
class KG3DSceneWndManager;
class PostRender::KG3DPostRenderEffectBloom;
class KG3DSceneOutputWnd : public IEKG3DSceneOutputWnd
{
public:
	friend class KG3DSceneSceneEditor;

	KG3DSceneWndManager*	m_pMaster;
	BOOL                    m_bNeedRender;//是否进行绘制
	BOOL                    m_bPresent;//是否进行提交操作
	int                     m_nID;//数字标志
	HWND                    m_hWnd;//对应的窗口句炳
	RECT                    m_Rect;//绘制到窗口的区域
	
	TCHAR               m_szName[MAX_PATH];
	D3DVIEWPORT9		m_Viewport;
	D3DXVECTOR2			m_vec2StartCursorPos,m_vec2EndCursorPos;

	BOOL				m_bEnableOutput;///是否输出
	BOOL                m_bEnableScaleOutput;	


	LPDIRECT3DTEXTURE9*		m_pRenderTextures;	// ViM
	LPDIRECT3DSURFACE9*		m_pRenderTargets;	// ViM
	FLOAT					m_fCamTranslation;	// ViM
	FLOAT					m_fCamFixpoint;		// ViM
	BOOL m_bUseVIM3D;

public:
	enum
	{
		MAX_TYPE = 0x00ffffff,
	};
private:
	DWORD               m_dwType;
	DWORD_PTR           m_dwOption;///原来是Public的，现在变成Private
	LPVOID				m_pOptionSpace;
	INT					m_nOptionSpaceSize;
	KG3DScene*          m_lpParentScene;
public:
   

	HRESULT InitV3D();
	HRESULT UnInitV3D();

	HRESULT SetType(DWORD dwType);
	DWORD GetType();
	HRESULT SetOption(DWORD_PTR dwOption);///小心64位系统的时候，外部还是DWORD作为参数的
	DWORD_PTR GetOption();
	HRESULT	GetOptionStruct(LPVOID pOptionSpace, INT	nBufferSize, INT* nOptionSpaceSize);
	HRESULT GetTextureWndParam(OUTPUTWND_TEXTURE_PARAM* pResult);
	

public:
    const D3DVIEWPORT9* GetViewPort(){return &m_Viewport;};

    virtual IEKG3DCamera* GetIECamera();
	virtual KG3DCamera&		GetCamera();

    virtual HRESULT ResetDevice(BOOL bEnable, DWORD dwWidth, DWORD dwHeight);;
    virtual HRESULT EnablePostEffect(PostRenderEffectType Type, BOOL bEnable);
    virtual HRESULT ReloadFX();
    virtual PostRender::KG3DPostRenderEffectManager* GetPostRenderManager();

    // ---------- bloom parameter ------------
    virtual int SetBloomGate(float fValue);
    virtual int SetBloomExposure1(float fValue);
    virtual int SetBloomExposure2(float fValue);
    virtual int SetBloomGaussCoefficient(float fValue);
    virtual int SetfBloomSampleNum(int nValue);
    virtual int SetBloomDelaye(float fValue);

    virtual int SetDetailPower(float fValue);
    virtual int SetBlurPower(float fValue);
    virtual int SetBlurDistance(float fValue);


    // ---------- hsi parameter --------------
    virtual int SetHSIHueScale(float fValue);
    virtual int SetHSIHueBias(float fValue);
    virtual int SetHSISaturationScale(float fValue);
    virtual int SetHSISaturationBias(float fValue);
    virtual int SetHSIIntensityScale(float fValue);
    virtual int SetHSIIntensityBias(float fValue);

    virtual int SetActiveCurveRedPoints(D3DXVECTOR2* pts, int num)    { return m_RenderManager.SetActiveCurveRedPoints(pts, num); }
    virtual int SetActiveCurveGreedPoints(D3DXVECTOR2* pts, int num)  { return m_RenderManager.SetActiveCurveGreedPoints(pts, num); }
    virtual int SetActiveCurveBluePoints(D3DXVECTOR2* pts, int num)   { return m_RenderManager.SetActiveCurveBluePoints(pts, num); }

    virtual D3DXVECTOR2 GetSplinePoint(int stype, int channel, int index) { return m_RenderManager.GetSplinePoint(stype, channel, index); }
    virtual int GetSplinePointCount(int stype, int channel)               { return m_RenderManager.GetSplinePointCount(stype, channel);   }
    virtual int GetStypeCount()                                           { return m_RenderManager.GetStypeCount(); }
    virtual int SetActiveStype(int index, int bFlush)                     { return SUCCEEDED(m_RenderManager.SetActiveStype(index, bFlush)); }
    virtual int GetActiveStype()                                          { return m_RenderManager.GetActiveStype(); }
    virtual int GetStypeName(int index, char szName[])                    { return m_RenderManager.GetStypeName(index, szName); }
    virtual int SetStypeName(int index, char szName[])                    { return m_RenderManager.SetStypeName(index, szName);  }  
    virtual int CloneActiveStype()                                        { return m_RenderManager.CloneActiveStype(); }
    virtual int DelActiveStype()                                          { return m_RenderManager.DelActiveStype();   }
    virtual int ResetActiveStype(DWORD channel)                           { return m_RenderManager.ResetActiveStype(channel); }

    virtual int SetCMYK_KC(float k);
    virtual int SetCMYK_KM(float k);
    virtual int SetCMYK_KY(float k);

    // ---------- shock wave -----------------
    virtual int SetShockWavePower(float fValue);

    // god ray
    virtual int SetGodRaySampleNum(int nValue);
    virtual int SetGodRayDensity(float fValue);
    virtual int SetGodRayWeight(float fValue);
    virtual int SetGodRayDecay(float fValue);
    virtual int SetGodRayExposure(float fValue);
    virtual int SetGodRayAngle(float fValue);

    virtual int SetSSAOLevel(int nLevel);
    virtual int SetSSAOSampRadius(float radius);
    virtual int SetSSAODarkFact(float dark);
    virtual int SetSSAOBlurWeight(float w);

	virtual int SetFogDensityV(float fValue);
    virtual int SetFogSlopeThreshold(float fValue);
    virtual int SetFogHeightFalloff(float fValue);
    virtual int SetFogcolor(D3DXVECTOR4 vecColor);

    virtual PostRender::KG3DPostRenderEffectBloom* GetBloomEffect();

	// ---------- shadow map -----------------
	virtual int SetEnabledAreaShadow(BOOL bEnabled);
	virtual void GetDepthTexCoordScale(float* pU, float* pV){return m_RenderManager.GetDepthTexCoordScale(pU,pV);}

	virtual int SetShadowSoftness(float fValue);
	virtual int SetShadowJitter(float fValue);

	//HDR
	virtual int SetHDRBrightGate(float fValue);
	virtual int SetHDRRectify(float fValue);
	virtual int SetHDRGrayLevel(float fValue);
	virtual int SetHDRStarType(int fValue);
	virtual int SetHDREnableBlueShift(BOOL fValue);
	virtual int SetHDRBloomScale(float fValue);
	virtual int SetHDRStarScale(float fValue);
	virtual int GetHDRStarTypeCount()                    { return m_RenderManager.GetHDRStarTypeCount(); }
	virtual int GetHDRStarName(int index, char szName[]) { return m_RenderManager.GetHDRStarName(index, szName); }

    ////////////////////////////////////////////////////////////////////////////////
    virtual HRESULT GetCameraStation(IEKG3DCameraStation **ppiStation);
    virtual HRESULT InitCameraStation(const char pcszFileName[]);
    ////////////////////////////////////////////////////////////////////////////////
    virtual HRESULT GetViewPortWidthHeight(DWORD *pdwRetWidth, DWORD *pdwRetHeight);
    virtual HRESULT GetRect(RECT *pRetRect);

	virtual HRESULT Enable(BOOL bEnable);
	virtual HRESULT IsEnable(BOOL* bRetEnable);
public:
    BOOL IsSaveToFile()
    {
        return m_RenderManager.IsSaveToFile();
    }
    void EnableSaveToFile(BOOL bEnable)
    {
        return m_RenderManager.EnableSaveToFile(bEnable);
    }
    void SetCurrentFrame(DWORD dwFrame)
    {
        return m_RenderManager.SetCurrentFrame(dwFrame);
    }
    BOOL IsEnablePostRender()
    {
        return m_RenderManager.IsEnablePostRender();
    }
    HRESULT RenderPostEffect(KG3DScene* pScene)
    {
        return m_RenderManager.Render(pScene);
    }
    HRESULT RenderTargetSet(DWORD dwAlphaEnable = FALSE)
    {
        return m_RenderManager.RenderTargetSet(dwAlphaEnable);
    }
    HRESULT RenderTargetRestore()
    {
        return m_RenderManager.RenderTargetRestore();
    }

	HRESULT PreColorRender(KG3DScene* pScene)
	{
		return m_RenderManager.PreColorRender(pScene);
	}
	HRESULT PreZRender(KG3DScene* pScene)
	{
		return m_RenderManager.PreZRender(pScene);
	}

	BOOL NeedPreZRender()
	{
		return m_RenderManager.NeedPreZRender();
	}

	BOOL NeedPreColorRender()
	{
		return m_RenderManager.NeedPreColorRender();
	}
	//Just for in value,Not for output.
	HRESULT GetShadowMask(LPDIRECT3DTEXTURE9* ppTexture)
	{
		return m_RenderManager.GetShadowMask(ppTexture);
	}

public:	//下面全是RenderManager的转调，是否可以直接把RenderManager传出去？
	HRESULT OnLostDevice();

	HRESULT OnResetDevice();
public:
	KG3DSceneOutputWnd( KG3DSceneWndManager* pMaster );

	
	virtual ~KG3DSceneOutputWnd(void);

	void OnResizeWindow(PRECT pRect = NULL);

	/*GetPickRay系列*/
	virtual HRESULT GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* pNormalizedDir,D3DXVECTOR2 * RayOrig2D);//vSrc是在近平面上的。得到隐式直线P(t) = P + Dir * t;其中Dir已经归一化
	//HRESULT GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* pNormalizedDir,const D3DXVECTOR2 &RayOrig2D);//vSrc是在近平面上的。得到隐式直线P(t) = P + Dir * t;其中Dir已经归一化
	HRESULT GetMatrixViewProjViewport(D3DXMATRIX& matViewProjViewport);
	HRESULT GetPickRayWindowsPos(D3DXVECTOR2& vRayPos);	//屏幕坐标
	HRESULT GetPickRayFromWindowsPos(const D3DXVECTOR2& vPosWindow, D3DXVECTOR3& vSrc, D3DXVECTOR3& vNormalizedDir);

	HRESULT GetNear_Far_Point(D3DXVECTOR3 &Near,D3DXVECTOR3 &Far);
	HRESULT GetCursorScreenPos(D3DXVECTOR2 &ScreenPos);	//得到的是百分比屏幕坐标


	HRESULT Present();
	HRESULT RenderWindowBegin(BOOL bSetViewPort);
	HRESULT RenderWindowEnd();
protected:
	static void InitCamera(BYTE* pBuffer, INT BufferSize, bool bTrackCameraOrSimpleCamera);
	static void UnInitCamera(BYTE* pBuffer, INT BufferSize);
private:
    PostRender::KG3DPostRenderEffectManager m_RenderManager;

	
private://用于Camera的多态，外部可以决定得到一个什么相机，是带有弹簧和遮挡的TrackCamera还是Camera
	enum
	{
		EM_CAMERA_BUFFER_SIZE = sizeof(KG3DTrackCamera) > sizeof(KG3DCamera) 
					? sizeof(KG3DTrackCamera) : sizeof(KG3DCamera),
	};
	BYTE	m_CameraBuffer[EM_CAMERA_BUFFER_SIZE];
	KG3DCamera* m_pCamera;	//可以实现多态，视乎创建参数，可以是一个TrackCamera也可以是一个普通Camera
};
