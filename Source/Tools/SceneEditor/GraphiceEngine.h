// GraphiceEngine.h: interface for the GraphiceEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICEENGINE_H__6BC653F2_CA98_407E_A0C0_84C407C7ED88__INCLUDED_)
#define AFX_GRAPHICEENGINE_H__6BC653F2_CA98_407E_A0C0_84C407C7ED88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "scene.h"
#include "Camera.h"	// Added by ClassView

class KGraphicsEngine  
{
public:
	//some public struct of engine
	struct D3DModeInfo
	{
		DWORD      Width;      // Screen width in this mode
		DWORD      Height;     // Screen height in this mode
		D3DFORMAT  Format;     // Pixel format in this mode
		DWORD      dwBehavior; // Hardware / Software / Mixed vertex processing
		D3DFORMAT  DepthStencilFormat; // Which depth/stencil format to use with this mode
	};

	struct D3DDeviceInfo
	{
		// Device data
		D3DDEVTYPE   DeviceType;      // Reference, HAL, etc.
		D3DCAPS9     d3dCaps;         // Capabilities of this device
		const TCHAR* strDesc;         // Name of this device
		BOOL         bCanDoWindowed;  // Whether this device can work in windowed mode

		// Modes for this device
		DWORD        dwNumModes;
		D3DModeInfo  modes[150];

		// Current state
		DWORD        dwCurrentMode;
		BOOL         bWindowed;
		D3DMULTISAMPLE_TYPE MultiSampleType;
	};

	struct D3DAdapterInfo
	{
		// Adapter data
		D3DADAPTER_IDENTIFIER9   d3dAdapterIdentifier;
		D3DDISPLAYMODE d3ddmDesktop;      // Desktop display mode for this adapter

		// Devices for this adapter
		DWORD          dwNumDevices;
		D3DDeviceInfo  devices[5];

		// Current state
		DWORD          dwCurrentDevice;
	};

protected:
    // Internal variables for the state of the app
    D3DAdapterInfo    m_Adapters[10];
    DWORD             m_dwNumAdapters;
    DWORD             m_dwAdapter;
    BOOL              m_bWindowed;
    BOOL              m_bActive;
    BOOL              m_bReady;

	DWORD             m_BaseWinStyle;
    
	// Overridable variables for the app
    TCHAR*            m_strWindowTitle;    // Title for the app's window
    BOOL              m_bUseDepthBuffer;   // Whether to autocreate depthbuffer
    DWORD             m_dwMinDepthBits;    // Minimum number of bits needed in depth buffer
    DWORD             m_dwMinStencilBits;  // Minimum number of bits needed in stencil buffer
    DWORD             m_dwCreationWidth;   // Width used to create window
    DWORD             m_dwCreationHeight;  // Height used to create window
    BOOL              m_bShowCursorWhenFullscreen; // Whether to show cursor when fullscreen
	
	
	D3DDISPLAYMODE           m_DisplayMode;

	LPSCENE    m_lpCurScene;
	HWND       m_LastRenderWindow;
public:
	BOOL    m_bUseMotionBlur;
	DWORD   m_BufferTextureID;
	DWORD   m_SmallerTextureID;
	DWORD   m_BlurTextureID;
	D3DPRESENT_PARAMETERS    m_PresentParam;

	RECT    m_RenderWindowRect;
public:
	HRESULT BackBufferRelease();
	HRESULT BackBufferCreate();

	HRESULT Reset();
	HRESULT EnableMotionBlur(BOOL Value);
	HRESULT Toggle2Windowed();
	HRESULT Toggle2FullScreen();
	HRESULT SetRenderWindow(HWND hRenderWindow);
//	HRESULT GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* RayDir,POINT Cursor);//GetPickRay在这里没有检索到任何引用
//  HRESULT GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* RayDir );
	HRESULT UpdateLight();
	GraphicsStruct::KCamera m_cCamera;
	HRESULT SetCurScene(LPSCENE pScene);
	HRESULT Render();
	HRESULT PreRender();

	HRESULT InitializeWindowed(HWND hBaseWindow, HWND hRenderWindow);
	KGraphicsEngine();
	virtual ~KGraphicsEngine();

	void SetCurCamera(GraphicsStruct::KCamera* pCamera);
	GraphicsStruct::KCamera* GetCurCamera(void);
	HRESULT CleanUp(void);
};

extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern LPDIRECT3D9        g_pD3D      ; // Used to create the D3DDevice
extern DWORD              g_dwRenderCount;
extern KGraphicsEngine    g_GraphiceEngine;

#endif // !defined(AFX_GRAPHICEENGINE_H__6BC653F2_CA98_407E_A0C0_84C407C7ED88__INCLUDED_)
