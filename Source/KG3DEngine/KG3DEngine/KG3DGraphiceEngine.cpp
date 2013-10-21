// GraphiceEngine.cpp: implementation of the GraphiceEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./KG3DGraphiceEngine.h"
#include "./KG3DEngineManager.h"
#include "./KG3DDebugD3DDevice.h"
#include "KG3DGraphicsTool.h"
#include "KG3DEngineOptionCaps.h"
#include "KG3DEngineOption.h"
#include "JpegFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering devic
KG3DGraphicsEngine      g_cGraphicsEngine;

BOOL g_bUseVIM3D = FALSE;
DWORD g_dwNumViews;			// ViM
DWORD					g_xpos, g_ypos, g_width, g_height, g_adapterIdx, g_refreshRate;	// ViM
int g_nDepthTexturePackingType = 0; //0: no packing,1:a8r8b8g8 packing, 2: a16r16b16g16 packing.
extern BOOL g_b3DVision;

static HRESULT GetBackBufferDesc(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    LPDIRECT3DSURFACE9 pBackBuffer = NULL;

    KGLOG_PROCESS_ERROR(pd3dDevice);
    KGLOG_PROCESS_ERROR(pBackBufferSurfaceDesc);

    hr = pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    KGLOG_COM_PROCESS_ERROR(hr);

    memset(pBackBufferSurfaceDesc, 0, sizeof(D3DSURFACE_DESC));

    hr = pBackBuffer->GetDesc(pBackBufferSurfaceDesc);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    SAFE_RELEASE(pBackBuffer);

    return hrResult;
}

KG3DGraphicsEngine::KG3DGraphicsEngine()
: m_pD3D(NULL)
, m_vimd3d(NULL)
, m_bSupportMMX(false)
, m_bSupportSSE(false)
, m_pDeviceCallback(NULL)
, m_bDeviceLost(true)
, m_bDeviceObjectsLost(false)
, m_eShotImageType(D3DXIFF_FORCE_DWORD)
, m_nShotImageQuty(0)
{
    memset(&m_DeviceSettings, 0, sizeof(m_DeviceSettings));
	memset(&m_DisplayMode, 0, sizeof(m_DisplayMode));
	memset(&m_PresentParam, 0, sizeof(m_PresentParam));
	memset(&m_EngineCaps, 0, sizeof(KG3DEngineCaps));
}

KG3DGraphicsEngine::~KG3DGraphicsEngine()
{
	ASSERT(g_pd3dDevice == NULL);
	ASSERT(m_pD3D == NULL);
}

HRESULT KG3DGraphicsEngine::SetDeviceCallback(IKG3DeviceCallback* pCallback)
{
    m_pDeviceCallback = pCallback;

    return S_OK;
}

HRESULT KG3DGraphicsEngine::InitVIM3D(LPDIRECT3DDEVICE9 pd3dDevice)
{
    HRESULT hResult = E_FAIL;
    int nRetCode = false;
    int nInitLib = false;
    int nCreate = false;
    TCHAR szError[256];

	KG_PROCESS_SUCCESS(!g_bUseVIM3D);

    KGLOG_PROCESS_ERROR(pd3dDevice);

    nRetCode = ViMD3D_InitLib(szError);
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitLib = true;

    nRetCode = ViMD3D_GetInfo(g_dwNumViews, g_xpos, g_ypos, g_width, g_height, g_adapterIdx, g_refreshRate);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ViMD3D_Create(pd3dDevice, &m_vimd3d);
    KGLOG_PROCESS_ERROR(nRetCode);

    nCreate = true;

Exit1:
    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        if (nCreate)
        {
            ViMD3D_Release(m_vimd3d);
            m_vimd3d = NULL;
        }
        if (nInitLib)
        {
            // TODO: VIM3D 没有ViMD3D_InitLib()相应的反初始化操作，
        }
    }
    return hResult;
}

HRESULT KG3DGraphicsEngine::UnInitVIM3D()
{
    HRESULT hr = S_OK;

    if (g_bUseVIM3D)
    {
        if (m_vimd3d)
        {
            ViMD3D_Release(m_vimd3d);
            m_vimd3d = NULL;
        }
    }

    return hr;
}


HRESULT KG3DGraphicsEngine::InitProcessorFeature()
{
    m_bSupportMMX = ::IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE);
    m_bSupportSSE = ::IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE);

    return S_OK;
}

HRESULT KG3DGraphicsEngine::GetMultiSample(D3DMULTISAMPLE_TYPE* pMultiSampleType, PDWORD pdwMultiSampleQuality, UINT uAdapter, D3DDEVTYPE DeviceType, 
                                           BOOL bWindowed, D3DFORMAT BackBufferFormat, D3DFORMAT DepthStencilFormat)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	DWORD dwBackBufferQualityLevels = 0;
	DWORD dwDepthBufferQualityLevels = 0;

    KGLOG_PROCESS_ERROR(pMultiSampleType);
    KGLOG_PROCESS_ERROR(pdwMultiSampleQuality);

	hr = m_pD3D->CheckDeviceMultiSampleType(uAdapter, DeviceType, BackBufferFormat, bWindowed, *pMultiSampleType, &dwBackBufferQualityLevels);
    KG_COM_PROCESS_ERROR(hr);

	hr = m_pD3D->CheckDeviceMultiSampleType(uAdapter, DeviceType, DepthStencilFormat, bWindowed, *pMultiSampleType, &dwDepthBufferQualityLevels);
    KG_COM_PROCESS_ERROR(hr);

    if (dwBackBufferQualityLevels > dwDepthBufferQualityLevels)
        *pdwMultiSampleQuality = dwBackBufferQualityLevels;
    else
        *pdwMultiSampleQuality = dwDepthBufferQualityLevels;

    /*if (*pdwMultiSampleQuality > 1)
        --*pdwMultiSampleQuality;
    else*/
        *pdwMultiSampleQuality = 0;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        *pMultiSampleType = D3DMULTISAMPLE_NONE;
        *pdwMultiSampleQuality = 0;
    }
    return hrResult;
}

HRESULT KG3DGraphicsEngine::GetAdapater(PUINT puAdapter, D3DDEVTYPE* pDeviceType)
{
    HRESULT hr = S_OK;

    UINT uAdapter = 0;
    UINT uAdapterCount = 0;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

    uAdapterCount = m_pD3D->GetAdapterCount();

	for (; uAdapter < uAdapterCount; ++uAdapter)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;

		hr = m_pD3D->GetAdapterIdentifier(uAdapter, 0, &Identifier);
        KGLOG_COM_PROCESS_ERROR(hr);

        // Look for 'NVIDIA NVPerfHUD' adapter
        // If it is present, override default settings
		if (strstr(Identifier.Description, "NVPerfHUD") != 0)
		{
			DeviceType = D3DDEVTYPE_REF;
			break;
		}
		if (strcmpi(Identifier.Description,"NVIDIA PerfHUD") == 0)
		{ 
			DeviceType = D3DDEVTYPE_REF; 
			break; 
		}
	}

    if (uAdapter == uAdapterCount)
    {
        uAdapter = D3DADAPTER_DEFAULT;
        DeviceType = D3DDEVTYPE_HAL;
    }

    *puAdapter = uAdapter;
    *pDeviceType = DeviceType;

Exit0:
    return hr;
}

HRESULT KG3DGraphicsEngine::GetDepthStencilFormat(UINT uAdapter, D3DDEVTYPE DeviceType, D3DFORMAT* pDepthStencilFormat, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat)
{
    HRESULT hr = E_FAIL;

    D3DFORMAT Formats[] =
    {
        D3DFMT_D24S8,
        D3DFMT_D16,
    };

    KGLOG_PROCESS_ERROR(pDepthStencilFormat);

    for (int nIndex = 0; nIndex < _countof(Formats); ++nIndex)
    {
        hr = m_pD3D->CheckDepthStencilMatch(uAdapter, DeviceType, AdapterFormat, RenderTargetFormat, Formats[nIndex]);
        if (SUCCEEDED(hr))
        {
            *pDepthStencilFormat = Formats[nIndex];
            return S_OK;
        }
    }

Exit0:
    return E_FAIL;
}

HRESULT KG3DGraphicsEngine::InitDevice(UINT uAdapter, D3DDEVTYPE DeviceType, HWND hwnd)
{
    HRESULT hr = S_OK;
    DWORD dwBehaviorFlags = 0;//D3DCREATE_MULTITHREADED;
	D3DPRESENT_PARAMETERS PresentParam;
	memcpy(&PresentParam, &m_PresentParam, sizeof(D3DPRESENT_PARAMETERS));
	
	_ASSERTE(g_pd3dDevice == NULL);

    hr = m_pD3D->CreateDevice(uAdapter, 
		DeviceType, 
		hwnd, 
		D3DCREATE_MIXED_VERTEXPROCESSING | dwBehaviorFlags, 
		&PresentParam, 
		&g_pd3dDevice);
    KG_COM_PROCESS_SUCCESS(hr);

	memcpy(&PresentParam, 
		&m_PresentParam, 
		sizeof(D3DPRESENT_PARAMETERS));
    hr = m_pD3D->CreateDevice(uAdapter, 
		DeviceType, 
		hwnd, 
		D3DCREATE_HARDWARE_VERTEXPROCESSING | dwBehaviorFlags, 
		&PresentParam, 
		&g_pd3dDevice);
    KG_COM_PROCESS_SUCCESS(hr);

	memcpy(&PresentParam, 
		&m_PresentParam, 
		sizeof(D3DPRESENT_PARAMETERS));
    hr = m_pD3D->CreateDevice(uAdapter, 
		DeviceType, 
		hwnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | dwBehaviorFlags, 
		&PresentParam, 
		&g_pd3dDevice);
    KG_COM_PROCESS_SUCCESS(hr);

    KGLOG_COM_PROCESS_ERROR(hr);
    KGLOG_PROCESS_ERROR(g_pd3dDevice);

Exit1:
	memcpy(&m_PresentParam, &PresentParam, sizeof(D3DPRESENT_PARAMETERS));
	//检查设备能力
	{
		ZeroMemory(&m_EngineCaps, sizeof(m_EngineCaps));
		HRESULT hrTemp = g_pd3dDevice->GetDeviceCaps(&m_EngineCaps.D3DCaps);
		if (SUCCEEDED(hrTemp))
		{
			for (size_t i = 0; i < _countof(m_EngineCaps.IsShadowTypeAvailable); ++i)
			{
				m_EngineCaps.IsShadowTypeAvailable[i] = TRUE;
			}
            m_EngineCaps.IsShadowTypeAvailable[EM_MODEL_SHADOW_TYPE_LOW] = 
                (m_EngineCaps.D3DCaps.StencilCaps & D3DSTENCILCAPS_INCR) && 
                (m_EngineCaps.D3DCaps.StencilCaps & D3DSTENCILCAPS_DECR);
			//其它类型的影子的能力检查还没有写
		}
		else
		{
			KGLogPrintf(KGLOG_WARNING, "无法得到设备信息，可能显示不正常");
		}
	}

Exit0:
    if (hr == D3DERR_DEVICELOST)
        m_bDeviceLost = true;
    else
        KGLOG_COM_CHECK_ERROR(hr);
    return hr;
}

HRESULT KG3DGraphicsEngine::InitDebugDevice()
{
    HRESULT hr = S_OK;

	if (g_DebugD3DDevice.IsEnable())
	{
		g_DebugD3DDevice.SetDevice(g_pd3dDevice);
		g_pd3dDevice = &g_DebugD3DDevice; // release new g_pd3dDevice will be pass to old g_pd3dDevice
	}

    return hr;
}

HRESULT KG3DGraphicsEngine::Init(KG3DDeviceSettings const* pSettings, BOOL bEnableDebug)
{
    HRESULT hResult = E_FAIL;
    HRESULT hr = E_FAIL;
	UINT uAdapter = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
    KG3DDeviceSettings Settings;
	int nInitVIM3D = false;

    KGLOG_PROCESS_ERROR(pSettings);

    hr = InitProcessorFeature();
    KGLOG_COM_PROCESS_ERROR(hr);

    ASSERT(m_pD3D == NULL);

	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    KGLOG_PROCESS_ERROR(m_pD3D);

    memcpy(&Settings, pSettings, sizeof(KG3DDeviceSettings));

    hr = GetAdapater(&uAdapter, &DeviceType);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pD3D->GetAdapterDisplayMode(uAdapter, &m_DisplayMode);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cEngineOptionCaps.GetAdaperModeCaps(uAdapter, m_DisplayMode.Format);
    KGLOG_COM_PROCESS_ERROR(hr);

    memset(&m_PresentParam, 0, sizeof(m_PresentParam));

    hr = SetPresentParam(uAdapter, DeviceType, &m_PresentParam, &Settings);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = InitDevice(uAdapter, DeviceType, g_hBaseWnd);
    KGLOG_COM_PROCESS_ERROR(hr);

	g_GetDefaultShaderGroup().CheckShaderModel();
    if (m_PresentParam.MultiSampleType == D3DMULTISAMPLE_NONE && !g_b3DVision)
    {
        hr = g_pd3dDevice->SetDialogBoxMode(!m_PresentParam.Windowed);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (bEnableDebug)
    {
        g_DebugD3DDevice.Enable(bEnableDebug);

        hr = InitDebugDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = InitVIM3D(g_pd3dDevice);
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitVIM3D = true;

    m_bDeviceLost = false;
    m_bDeviceObjectsLost = false;

    memcpy(&m_DeviceSettings, &Settings, sizeof(KG3DDeviceSettings));

	//确实是否要使用多目标渲染
	g_cEngineOption.bUseRenderTarget_DepthTexture = g_cGraphicsEngine.m_PresentParam.MultiSampleType != D3DMULTISAMPLE_NONE;
	g_nDepthTexturePackingType = 0;
	if (g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		g_nDepthTexturePackingType = CheckDepthTexturePackingType(uAdapter,DeviceType);
		LPCTSTR strPackType = NULL;
		switch (g_nDepthTexturePackingType)
		{
		case 0:
			strPackType = _T("0");
			break;
		case 1:
			strPackType = _T("1");
			break;
		default:
			strPackType = _T("0");
			break;
		}
		D3DXMACRO macroPackType[] = {
			{_T("PACKTYPE"), strPackType},	
			{NULL, NULL},
		};
		D3DXMACRO_ARRAY macroArray = {macroPackType, _countof(macroPackType)};
		g_GetDefaultShaderGroup().ModifyGlobalMacro(macroArray);
	}

	//g_cEngineOption.bUseMultiRenderTarget = TRUE;

    hr = GetBackBufferDesc(g_pd3dDevice, &m_BackBufferSurfaceDesc);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (m_pDeviceCallback)
    {
        hr = m_pDeviceCallback->OnCreateDevice(g_pd3dDevice, &m_BackBufferSurfaceDesc, &m_PresentParam);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        if (nInitVIM3D)
        {
            UnInitVIM3D();
        }

        SAFE_RELEASE(g_pd3dDevice);
        SAFE_RELEASE(m_pD3D);
    }
	return hResult;
}

HRESULT KG3DGraphicsEngine::ScreenShot(const char szFilePath[], D3DXIMAGE_FILEFORMAT eType, int nQuality)
{
    _tcsncpy(m_szShotPath, szFilePath, sizeof(m_szShotPath));
    m_eShotImageType = eType;
    clip(nQuality, 0, 100);
    m_nShotImageQuty = nQuality;
    return S_OK;
}

HRESULT KG3DGraphicsEngine::ScreenShotImpl()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    IDirect3DSurface9* pFrameSurface = NULL;
    IDirect3DTexture9* pTextureSys = NULL;
    ID3DXBuffer* pBuffer = NULL;
    D3DVIEWPORT9 ViewPort;
    RECT RectView;

    ASSERT(m_eShotImageType != D3DXIFF_FORCE_DWORD);

    hRetCode = g_pd3dDevice->GetRenderTarget(0, &pFrameSurface);
    KG_COM_PROCESS_ERROR(hRetCode);

    g_pd3dDevice->GetViewport(&ViewPort);
    SetRect(&RectView, ViewPort.X, ViewPort.Y, ViewPort.X + ViewPort.Width, ViewPort.Y + ViewPort.Height);

    if (m_eShotImageType == D3DXIFF_JPG)
    {
        D3DSURFACE_DESC SurfaceDesc;
        D3DLOCKED_RECT LockRect;
        
        hRetCode = D3DXSaveSurfaceToFileInMemory(&pBuffer, D3DXIFF_BMP, pFrameSurface, NULL, &RectView);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, 
            pBuffer->GetBufferPointer(), 
            pBuffer->GetBufferSize(),
            ViewPort.Width,
            ViewPort.Height,
            1,
            0,
            D3DFMT_A8R8G8B8,
            D3DPOOL_SCRATCH,
            D3DX_DEFAULT,
            D3DX_DEFAULT,
            0,
            NULL,
            NULL,
            &pTextureSys);
        KG_COM_PROCESS_ERROR(hRetCode);


        /*
        hRetCode = D3DXSaveSurfaceToFile(TEXT("__temp_shot.bmp"), D3DXIFF_BMP, pFrameSurface, NULL, &RectView);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = D3DXCreateTextureFromFileEx(g_pd3dDevice, 
            TEXT("__temp_shot.bmp"), 
            ViewPort.Width,
            ViewPort.Height,
            1,
            0,
            D3DFMT_A8R8G8B8,
            D3DPOOL_SCRATCH,
            D3DX_DEFAULT,
            D3DX_DEFAULT,
            0,
            NULL,
            NULL,
            &pTextureSys);
        KG_COM_PROCESS_ERROR(hRetCode); */

        pTextureSys->GetLevelDesc(0, &SurfaceDesc);

        if (SUCCEEDED(pTextureSys->LockRect(0, &LockRect, NULL, D3DLOCK_NOSYSLOCK)))
        {
            struct _Rgba { BYTE r; BYTE g; BYTE b; BYTE a;};
            struct _Rgb  { BYTE b; BYTE g; BYTE r; };
            _Rgb* pRgb = new _Rgb[SurfaceDesc.Height * SurfaceDesc.Width];

            for (UINT v = 0; v < SurfaceDesc.Height; ++v)
            {
                _Rgba* pRgba = (_Rgba*)((BYTE*)LockRect.pBits + LockRect.Pitch * v);

                for (UINT u = 0; u < SurfaceDesc.Width; ++u)
                {
                    pRgb[v * SurfaceDesc.Width + u].r = pRgba[u].r;
                    pRgb[v * SurfaceDesc.Width + u].g = pRgba[u].g;
                    pRgb[v * SurfaceDesc.Width + u].b = pRgba[u].b;
                }
            }

            JpegFile::RGBToJpegFile(m_szShotPath, (BYTE*)pRgb, SurfaceDesc.Width, SurfaceDesc.Height, TRUE, m_nShotImageQuty);

            delete[] pRgb;
            pTextureSys->UnlockRect(0);
        }
        else
        {
            KG_PROCESS_ERROR(FALSE);
        }
    }
    else
    {
        hRetCode = D3DXSaveSurfaceToFile(m_szShotPath, m_eShotImageType, pFrameSurface, NULL, &RectView);
        KG_COM_PROCESS_ERROR(hRetCode);
    }

    hResult = S_OK;
Exit0 :
    SAFE_RELEASE(pBuffer);
    SAFE_RELEASE(pTextureSys);
    SAFE_RELEASE(pFrameSurface);
    return hResult;
}

HRESULT KG3DGraphicsEngine::Present(CONST RECT * pSourceRect, CONST RECT * pDestRect, HWND hDestWindowOverride, CONST RGNDATA * pDirtyRegion)
{
    HRESULT hr = S_OK;

    /*
      Screen Shot
    */
    if (m_eShotImageType != D3DXIFF_FORCE_DWORD)
    {
        ScreenShotImpl();
        m_eShotImageType = D3DXIFF_FORCE_DWORD;
    }

    /*
      Present 
     */

    hr = g_pd3dDevice->Present(m_PresentParam.Windowed ? pSourceRect : NULL, pDestRect, hDestWindowOverride, pDirtyRegion);
	ASSERT(hr != D3DERR_DRIVERINTERNALERROR);

    if (hr == D3DERR_DEVICELOST)
        m_bDeviceLost = true;
    else
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

HRESULT KG3DGraphicsEngine::Reset()
{
    HRESULT hr = E_FAIL;

    if (!m_bDeviceObjectsLost)
    {
        if (m_pDeviceCallback)
        {
            hr = m_pDeviceCallback->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        m_bDeviceObjectsLost = true;
    }

    if (m_bDeviceObjectsLost)
    {
        hr = g_pd3dDevice->Reset(&m_PresentParam);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = g_pd3dDevice->EvictManagedResources();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = GetBackBufferDesc(g_pd3dDevice, &m_BackBufferSurfaceDesc);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (m_pDeviceCallback)
        {
            D3DLIGHT9 Light;

            hr = m_pDeviceCallback->OnResetDevice(g_pd3dDevice, &m_BackBufferSurfaceDesc, &m_PresentParam);
            KGLOG_COM_PROCESS_ERROR(hr);

            m_bDeviceLost = false;

            memset(&Light, 0, sizeof(Light));

            Light.Type = D3DLIGHT_DIRECTIONAL;
            Light.Direction.x = 1.0f;
            Light.Direction.y = 0.0f;
            Light.Direction.z = 0.0f;

            hr = g_pd3dDevice->SetLight(0, &Light);
            KGLOG_COM_PROCESS_ERROR(hr);

        }
        m_bDeviceObjectsLost = false;
    }

Exit0:
    if (hr == D3DERR_DEVICELOST)
        m_bDeviceLost = true;
    else
        KGLOG_COM_CHECK_ERROR(hr);
    return hr;
}

HRESULT KG3DGraphicsEngine::CheckDevice()
{
	HRESULT hr = S_OK;
    
    if (m_bDeviceLost)
    {
        hr = g_pd3dDevice->TestCooperativeLevel();
        switch (hr)
        {
        case D3D_OK:
            break;
        case D3DERR_DEVICELOST:
            Sleep(50);
            break;
        case D3DERR_DEVICENOTRESET:
            hr = Reset();
            KG_COM_PROCESS_ERROR(hr);
            break;
        case D3DERR_DRIVERINTERNALERROR:
            KGLogPrintf(KGLOG_ERR, "D3DERR_DRIVERINTERNALERROR\n");
            break;
        }
    }

Exit0:
    if (hr == D3DERR_DEVICELOST)
        m_bDeviceLost = true;
    else
        KGLOG_COM_CHECK_ERROR(hr);
	return hr;
}

BOOL KG3DGraphicsEngine::IsFullScreen() const
{
    return !m_PresentParam.Windowed;
}

HRESULT KG3DGraphicsEngine::SetPresentParam(UINT uAdapter, D3DDEVTYPE DeviceType, D3DPRESENT_PARAMETERS* pParameters, KG3DDeviceSettings* pSettings)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;
	D3DFORMAT DepthStencilFormat = D3DFMT_UNKNOWN;
	D3DMULTISAMPLE_TYPE MultiSampleType = D3DMULTISAMPLE_NONE;
    DWORD dwMultiSampleQuality = 0;

    KGLOG_PROCESS_ERROR(pParameters);
    KGLOG_PROCESS_ERROR(pSettings);

    memset(pParameters, 0, sizeof(D3DPRESENT_PARAMETERS));
    
    hr = GetDepthStencilFormat(uAdapter, DeviceType, &DepthStencilFormat, m_DisplayMode.Format, m_DisplayMode.Format);
    KGLOG_COM_PROCESS_ERROR(hr);

    MultiSampleType = (D3DMULTISAMPLE_TYPE)pSettings->nMultiSampleType;
    dwMultiSampleQuality = pSettings->dwMultiSampleQuality;

    if (pSettings->bExclusiveMode)
    {
        m_PresentParam.Windowed = FALSE;
        m_PresentParam.BackBufferCount = pSettings->bTripleBuffering ? 2 : 1;
        m_PresentParam.PresentationInterval = pSettings->bVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
        m_PresentParam.FullScreen_RefreshRateInHz = pSettings->uRefreshRate;
		if (!g_b3DVision)
			m_PresentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

        if (!g_cEngineOptionCaps.CheckValidateAdaptMode(pSettings->uWidth, pSettings->uHeight))
        {
            pSettings->uWidth  = m_DisplayMode.Width;
            pSettings->uHeight = m_DisplayMode.Height;
        }
        MultiSampleType = D3DMULTISAMPLE_NONE;
        dwMultiSampleQuality = 0;
		m_PresentParam.BackBufferFormat = D3DFMT_X8R8G8B8;
    }
    else
    {
        pSettings->uWidth = max(pSettings->uWidth, m_DisplayMode.Width);
        pSettings->uHeight = max(pSettings->uHeight, m_DisplayMode.Height);

        m_PresentParam.Windowed = TRUE ;
        m_PresentParam.BackBufferCount = 0;
        m_PresentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        m_PresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

        if (pSettings->bLockableBackBuffer)
            m_PresentParam.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        else
            m_PresentParam.Flags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

        GetMultiSample(&MultiSampleType, &dwMultiSampleQuality, uAdapter, DeviceType, m_PresentParam.Windowed, m_DisplayMode.Format, DepthStencilFormat);
		m_PresentParam.BackBufferFormat = D3DFMT_A8R8G8B8;
    }
	
	m_PresentParam.BackBufferWidth = pSettings->uWidth;
	m_PresentParam.BackBufferHeight = pSettings->uHeight;

    m_PresentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_PresentParam.EnableAutoDepthStencil = TRUE;
    m_PresentParam.AutoDepthStencilFormat = DepthStencilFormat;
    m_PresentParam.hDeviceWindow = g_hBaseWnd;

    m_PresentParam.MultiSampleType = MultiSampleType;
    m_PresentParam.MultiSampleQuality = dwMultiSampleQuality;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DGraphicsEngine::GetDeviceSettings(KG3DDeviceSettings* pSettings)
{
    memset(pSettings, 0, sizeof(KG3DDeviceSettings));

    pSettings->uWidth = m_PresentParam.BackBufferWidth;
    pSettings->uHeight = m_PresentParam.BackBufferHeight;
    pSettings->bFullScreen = FALSE;
    pSettings->bPanauision = FALSE;
    pSettings->bExclusiveMode = !m_PresentParam.Windowed;
    pSettings->bTripleBuffering = m_PresentParam.BackBufferCount == 2;
    pSettings->bVSync = m_PresentParam.PresentationInterval == D3DPRESENT_INTERVAL_ONE;
    pSettings->uRefreshRate = m_PresentParam.FullScreen_RefreshRateInHz;
    pSettings->uColorDepth = 32; // TODO:
    pSettings->nMultiSampleType = m_PresentParam.MultiSampleType;
    pSettings->dwMultiSampleQuality = m_PresentParam.MultiSampleQuality;

    return S_OK;
}

HRESULT KG3DGraphicsEngine::SetDeviceSettings(KG3DDeviceSettings* pSettings)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    UINT uAdapter = D3DADAPTER_DEFAULT;
    D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
    KG3DDeviceSettings Settings;

    KGLOG_PROCESS_ERROR(pSettings);

	pSettings->nMultiSampleType = min(4,pSettings->nMultiSampleType);

    memcpy(&Settings, pSettings, sizeof(KG3DDeviceSettings));

    hr = GetAdapater(&uAdapter, &DeviceType);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cEngineOptionCaps.GetAdaperModeCaps(uAdapter, m_DisplayMode.Format);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = SetPresentParam(uAdapter, DeviceType, &m_PresentParam, &Settings);
    KGLOG_COM_PROCESS_ERROR(hr);

	memcpy(&m_DeviceSettings, &Settings, sizeof(m_DeviceSettings));
	g_cEngineOption.bUseRenderTarget_DepthTexture = g_cGraphicsEngine.m_PresentParam.MultiSampleType != D3DMULTISAMPLE_NONE;
	
	//Testing Packing Type;
	g_nDepthTexturePackingType = 0;
	if (g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		g_nDepthTexturePackingType = CheckDepthTexturePackingType(uAdapter,DeviceType);
		LPCTSTR strPackType = NULL;
		switch (g_nDepthTexturePackingType)
		{
		case 0:
			strPackType = _T("0");
			break;
		case 1:
			strPackType = _T("1");
			break;
		default:
			strPackType = _T("0");
			break;
		}
		D3DXMACRO macroPackType[] = {
			{_T("PACKTYPE"), strPackType},	
			{NULL, NULL},
		};
		D3DXMACRO_ARRAY macroArray = {macroPackType, _countof(macroPackType)};
		g_GetDefaultShaderGroup().ModifyGlobalMacro(macroArray);
	}

	//要求原来的设备还在。虽然是在创建设备前，但是因为改变了抗锯齿，而抗锯齿和其他postrender会有冲突，所以，先检查。
	g_cEngineManager.SetEngineOption(&g_cEngineOption);

    hr = Reset();
    if (hr == D3DERR_DEVICELOST)
    {
        do
        {
            hr = CheckDevice();
        }
        while (hr == D3DERR_DEVICELOST);
    }
    KG_COM_PROCESS_ERROR(hr);

	//////////////////
//	DWORD dwHeight = 0,dwWidth = 0;
	IEKG3DScene* pScene = g_cGraphicsTool.GetScene();
	if (pScene)
	{
		IEKG3DSceneOutputWnd* pOutputWindow = NULL;
		pScene->GetCurOutputWnd(&pOutputWindow);
		if (pOutputWindow)
		{
			KG3DSceneOutputWnd* pWnd = static_cast<KG3DSceneOutputWnd*>(pOutputWindow);
			pWnd->OnResizeWindow(&(pWnd->m_Rect));
		}
	}
	/////////////////
    if (m_PresentParam.MultiSampleType == D3DMULTISAMPLE_NONE && !g_b3DVision)
    {
        hr = g_pd3dDevice->SetDialogBoxMode(!m_PresentParam.Windowed);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (hr == D3DERR_DEVICELOST)
            m_bDeviceLost = true;
        else
            KGLOG_COM_CHECK_ERROR(hr);

        hrResult = hr;
    }
	if (pSettings)
		memcpy(pSettings, &m_DeviceSettings, sizeof(m_DeviceSettings));
	return hrResult;
}

HRESULT KG3DGraphicsEngine::SetViewportSize(UINT uWidth, UINT uHeight)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (uWidth > m_DeviceSettings.uWidth || uHeight > m_DeviceSettings.uHeight)
    {
        KG3DDeviceSettings Settings;

        memcpy(&Settings, &m_DeviceSettings, sizeof(KG3DDeviceSettings));

        Settings.uWidth = uWidth;
        Settings.uHeight = uHeight;

        hr = SetDeviceSettings(&Settings);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DGraphicsEngine::UnInit()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pDeviceCallback)
    {
        hr = m_pDeviceCallback->OnDestroyDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = UnInitVIM3D();
    KGLOG_COM_PROCESS_ERROR(hr);

	int k = g_pd3dDevice->AddRef();
	k = g_pd3dDevice->Release();
	VERIFY_RELEASE(g_pd3dDevice);	//如果到这里，就表明有资源没有释放干净

    VERIFY_RELEASE(m_pD3D);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

const KG3DEngineCaps& KG3DGraphicsEngine::GetCaps()
{
	return m_EngineCaps;
}

const D3DCAPS9& KG3DGraphicsEngine::GetD3DCaps()
{
	return m_EngineCaps.D3DCaps;
}

const D3DPRESENT_PARAMETERS& KG3DGraphicsEngine::GetPresentParameters()
{
	return m_PresentParam;
}

int KG3DGraphicsEngine::CheckDepthTexturePackingType(UINT uAdapter, D3DDEVTYPE DeviceType)
{
	HRESULT hr = E_FAIL;
	int nType = 0;
	D3DFORMAT fmtFomat = D3DFMT_R32F;
	hr = m_pD3D->CheckDeviceMultiSampleType(uAdapter, DeviceType,fmtFomat, m_PresentParam.Windowed, 
		m_PresentParam.MultiSampleType, NULL);
	if (FAILED(hr))
	{
		fmtFomat = D3DFMT_A8R8G8B8;
		hr = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(uAdapter, 
			DeviceType,fmtFomat, m_PresentParam.Windowed, 
			m_PresentParam.MultiSampleType, NULL);
		if (FAILED(hr))
		{
			fmtFomat = D3DFMT_A16B16G16R16F;
			hr = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(uAdapter, 
				DeviceType,fmtFomat, m_PresentParam.Windowed, 
				m_PresentParam.MultiSampleType, NULL);
			if (FAILED(hr))
			{
				nType = 0;
			}
			else
			{
				nType = 2;
			}
		}
		else
		{
			nType = 1;
		}
	}
	else
	{
		nType = 0;
	}
	return nType;
}