// GraphiceEngine.h: interface for the GraphiceEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICEENGINE_H__6BC653F2_CA98_407E_A0C0_84C407C7ED88__INCLUDED_)
#define AFX_GRAPHICEENGINE_H__6BC653F2_CA98_407E_A0C0_84C407C7ED88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ViMD3DLib/include/ViMD3DLib.h"
#include "../../../Include/KG3DEngine/KG3DInterface.h"


class KG3DGraphicsEngine  
{
public:
    KG3DGraphicsEngine();
	~KG3DGraphicsEngine();

	HRESULT Init(KG3DDeviceSettings const* pSettings, BOOL bEnableDebug);
	HRESULT UnInit();

    HRESULT SetDeviceCallback(IKG3DeviceCallback* pCallback);

    BOOL IsFullScreen() const;

    HRESULT GetDeviceSettings(KG3DDeviceSettings* pSettings);
	HRESULT SetDeviceSettings(KG3DDeviceSettings* pSettings);

    HRESULT SetViewportSize(UINT uWidth, UINT uHeight);

    HRESULT ScreenShot(const char szFilePath[], D3DXIMAGE_FILEFORMAT eType, int nQuality);  
    HRESULT ScreenShotImpl();

    HRESULT Present(CONST RECT * pSourceRect, CONST RECT * pDestRect, HWND hDestWindowOverride, CONST RGNDATA * pDirtyRegion);

    HRESULT CheckDevice();

	const KG3DEngineCaps& GetCaps();
	const D3DCAPS9&	GetD3DCaps();
	const D3DPRESENT_PARAMETERS& GetPresentParameters();

private:
    HRESULT InitProcessorFeature();
    HRESULT InitDevice(UINT uAdapter, D3DDEVTYPE DeviceType, HWND hwnd);
    HRESULT InitDebugDevice();

    HRESULT GetAdapater(PUINT puAdapter, D3DDEVTYPE* pDeviceType);
    HRESULT GetDepthStencilFormat(UINT uAdapter, D3DDEVTYPE DeviceType, D3DFORMAT* pDepthStencilFormat, 
        D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat);
    HRESULT GetMultiSample(D3DMULTISAMPLE_TYPE* pMultiSampleType, PDWORD pdwMultiSampleQuality, UINT uAdapter, D3DDEVTYPE DeviceType, 
        BOOL bWindowed, D3DFORMAT BackBufferFormat, D3DFORMAT DepthStencilFormat);

    HRESULT InitVIM3D(LPDIRECT3DDEVICE9 pd3dDevice);
    HRESULT UnInitVIM3D();

    HRESULT SetPresentParam(UINT uAdapter, D3DDEVTYPE DeviceType, D3DPRESENT_PARAMETERS* pParameters, KG3DDeviceSettings* pSettings);

    HRESULT Reset();

	int CheckDepthTexturePackingType(UINT uAdapter, D3DDEVTYPE DeviceType);

public:
    LPDIRECT3D9 m_pD3D;

    VIMD3D m_vimd3d;

    int m_bSupportMMX;
    int m_bSupportSSE;

    KG3DDeviceSettings m_DeviceSettings;

	D3DDISPLAYMODE m_DisplayMode;

    KG3DEngineCaps	m_EngineCaps;
	D3DPRESENT_PARAMETERS m_PresentParam;

private:
    IKG3DeviceCallback* m_pDeviceCallback;

    char m_szShotPath[MAX_PATH];
    D3DXIMAGE_FILEFORMAT m_eShotImageType;
    int m_nShotImageQuty;

    int m_bDeviceLost;
    int m_bDeviceObjectsLost;

    D3DSURFACE_DESC m_BackBufferSurfaceDesc;
};

//const D3DCAPS9&     g_GetD3DCaps();//改成用上面的g_cGraphicsEngine.GetD3DCaps(); 如
extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern KG3DGraphicsEngine g_cGraphicsEngine;
extern int g_nDepthTexturePackingType;

#endif // !defined(AFX_GRAPHICEENGINE_H__6BC653F2_CA98_407E_A0C0_84C407C7ED88__INCLUDED_)
