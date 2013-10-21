#include "stdafx.h"
#include "./KG3DEngineOptionCaps.h"
#include "./KG3DGraphiceEngine.h"

KG3DEngineOptionCaps g_cEngineOptionCaps;

KG3DEngineOptionCaps::KG3DEngineOptionCaps()
{
    m_AdapterModes.clear();
    m_vecMutliSampleType.clear();
    memset(m_nModelShadowType, 0, sizeof(m_nModelShadowType));

    m_nMinSampMipFilter = D3DTEXF_POINT;
    m_nMinSampMinFilter = D3DTEXF_POINT;
    m_nMinSampMagFilter = D3DTEXF_POINT;

    m_nMaxSampMipFilter = D3DTEXF_LINEAR;
    m_nMaxSampMinFilter = D3DTEXF_ANISOTROPIC;
    m_nMaxSampMagFilter = D3DTEXF_ANISOTROPIC;

    m_bRenderGrass     = TRUE;
    m_bGrassAnimation  = TRUE;
    m_bGrassAlphaBlend = TRUE;
    
    m_nMinTextureScale = 4;
    m_nMaxTextureScale = 1;
    
    m_fMinCameraDistance = 20000.00;
    m_fMaxCameraDistance = 80000.00;

    m_fMinCameraAngle = D3DX_PI / 6.0f;
    m_fMaxCameraAngle = D3DX_PI / 2.0f;
    
    m_bPostEffectEnable = TRUE;

    m_bBloomEnable      = TRUE;
    m_bHDREnable        = TRUE;
    m_bDOFEnable        = TRUE;
    m_bShockWaveEnable  = TRUE;
    m_bHSIEnable        = TRUE;
    m_bMotionBlur       = TRUE;
    m_bDetail           = TRUE;
    m_bGodRay           = TRUE;
    m_bSSAO             = TRUE;
    m_bGlobalFog        = TRUE;
    m_bCurveCMYK        = TRUE;

    m_bEnableDynamicConverMap = TRUE;
    m_bCpuSkin = TRUE;

    m_nMaxNumCpuThread = 8;
    m_nMinNumCpuThread = 1;

    m_bFlexBodySmooth = TRUE;

    m_nMaxTerrainDetail = 0;
    m_nMinTerrainDetail = 3;

    m_nMaxVegetationDensity = 1;
    m_nMinVegetationDensity = 8;

    m_nMinWaterDetail = WD_LOWER;
    m_nMaxWaterDetail = WD_FULL_EFFECTIVE;

    m_dwMaxAnisotropy = 16;
    m_dwMinAnisotropy = 1;

    m_vecShaderModel.clear();
}

KG3DEngineOptionCaps::~KG3DEngineOptionCaps()
{
    m_AdapterModes.clear();
}

UINT KG3DEngineOptionCaps::GetAllowableAdapterModeCount(UINT uAdapter, D3DFORMAT Format)
{
    UINT uCount = 0;
    KGLOG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

    uCount = g_cGraphicsEngine.m_pD3D->GetAdapterModeCount(uAdapter, Format);
Exit0:
    return uCount;
}

HRESULT KG3DEngineOptionCaps::GetAllowableAdapterMode(D3DDISPLAYMODE* pAdapterMode, UINT* puCount, UINT uAdapter, D3DFORMAT Format)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    UINT uModeCount = 0;
    UINT uModeIndex = 0;

    KGLOG_PROCESS_ERROR(pAdapterMode);
    KGLOG_PROCESS_ERROR(puCount);
    KGLOG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

    uModeCount = g_cGraphicsEngine.m_pD3D->GetAdapterModeCount(uAdapter, Format);
    uModeCount = min(uModeCount, *puCount);

    for (UINT uMode = 0; uMode < uModeCount; ++uMode)
    {
        hr = g_cGraphicsEngine.m_pD3D->EnumAdapterModes(uAdapter, Format, uMode, pAdapterMode + uModeIndex);
        KGLOG_COM_CHECK_ERROR(hr);

        if (SUCCEEDED(hr))
        {
            ++uModeIndex;
        }
    }

    *puCount = uModeIndex;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DEngineOptionCaps::GetAdaperModeCaps(UINT uAdapter, D3DFORMAT Format)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    UINT uCount = 0;
    std::vector<D3DDISPLAYMODE> DisplayModes;

    m_AdapterModes.clear();

    uCount = GetAllowableAdapterModeCount(uAdapter, Format);
    KGLOG_PROCESS_ERROR(uCount != 0);

    DisplayModes.resize(uCount);

    hRetCode = GetAllowableAdapterMode(&DisplayModes[0], &uCount, uAdapter, Format);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    for (UINT uIndex = 0; uIndex < uCount; ++uIndex)
    {
        int nFind = false;
        D3DDISPLAYMODE& dm = DisplayModes[uIndex];

        for (UINT uAdapter = 0; uAdapter != m_AdapterModes.size(); ++uAdapter)
        {
            KADAPTERMODE& am = m_AdapterModes[uAdapter];

            if (dm.Width == am.uWidth && dm.Height == am.uHeight)
            {
                for (UINT uRefreshRateIndex = 0; uRefreshRateIndex != _countof(am.uRefreshRates); ++uRefreshRateIndex)
                {
                    if (am.uRefreshRates[uRefreshRateIndex] == 0)
                    {
                        am.uRefreshRates[uRefreshRateIndex] = dm.RefreshRate;
                        break;
                    }
                }
                nFind = true;
                break;
            }
        }
        if (!nFind)
        {
            KADAPTERMODE Mode;

            memset(&Mode, 0, sizeof(Mode));

            Mode.uWidth = dm.Width;
            Mode.uHeight = dm.Height;
            Mode.uRefreshRates[0] = dm.RefreshRate;

            m_AdapterModes.push_back(Mode);
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

BOOL KG3DEngineOptionCaps::CheckValidateAdaptMode(UINT uWidth, UINT uHeight)
{
    BOOL bResult = false;

    for (UINT uIndex = 0; uIndex < m_AdapterModes.size(); ++uIndex)
    {
        KADAPTERMODE am = m_AdapterModes[uIndex];
        if (am.uHeight == uHeight && am.uWidth == uWidth)
        {
            bResult = true;
            break;
        }
    }

// Exit0:
    return bResult;
}

HRESULT KG3DEngineOptionCaps::GetAdapater(PUINT puAdapter, D3DDEVTYPE* pDeviceType)
{
    HRESULT hr = S_OK;

    UINT uAdapter = D3DADAPTER_DEFAULT;
    UINT uAdapterCount = 0;
    D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

    KG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);
    uAdapterCount = g_cGraphicsEngine.m_pD3D->GetAdapterCount();

    for (; uAdapter < uAdapterCount; ++uAdapter)
    {
        D3DADAPTER_IDENTIFIER9 Identifier;

        hr = g_cGraphicsEngine.m_pD3D->GetAdapterIdentifier(uAdapter, 0, &Identifier);
        KGLOG_COM_PROCESS_ERROR(hr);

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

HRESULT KG3DEngineOptionCaps::GetDepthStencilFormat(UINT uAdapter, D3DDEVTYPE DeviceType, D3DFORMAT* pDepthStencilFormat, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DFORMAT Formats[] =
    {
        D3DFMT_D24S8,
        D3DFMT_D16,
    };

    KGLOG_PROCESS_ERROR(pDepthStencilFormat);

    for (int nIndex = 0; nIndex < _countof(Formats); ++nIndex)
    {
        hRetCode = g_cGraphicsEngine.m_pD3D->CheckDepthStencilMatch(uAdapter, DeviceType, AdapterFormat, RenderTargetFormat, Formats[nIndex]);
        if (SUCCEEDED(hRetCode))
        {
            *pDepthStencilFormat = Formats[nIndex];
            hResult = S_OK;
        }
    }

Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetMultiSampleTypeListCaps()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DFORMAT DepthStencilFormat = D3DFMT_UNKNOWN;
    D3DFORMAT DisPlayModeFormate = g_cGraphicsEngine.m_DisplayMode.Format;

    LPDIRECT3D9 pD3D = g_cGraphicsEngine.m_pD3D;
    D3DCAPS9    D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    KG_PROCESS_SUCCESS(!g_cGraphicsEngine.m_PresentParam.Windowed);
    KG_PROCESS_ERROR(pD3D);

    hRetCode = GetDepthStencilFormat(D3DCaps.AdapterOrdinal, 
        D3DCaps.DeviceType, 
        &DepthStencilFormat, 
        DisPlayModeFormate, 
        DisPlayModeFormate);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    m_vecMutliSampleType.clear();
    for (int i = 1; i <= 4; i++)
    {
        _D3DMULTISAMPLE_TYPE MultiSampleType = (_D3DMULTISAMPLE_TYPE)i;
        hRetCode = pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, D3DCaps.DeviceType,
            DisPlayModeFormate, TRUE, MultiSampleType, NULL);
        if (FAILED(hRetCode))
            continue;

        hRetCode = pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, D3DCaps.DeviceType,
            DepthStencilFormat, TRUE, MultiSampleType, NULL);
        if (FAILED(hRetCode))
            continue;

        m_vecMutliSampleType.push_back(i);
    }

Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetTextureSamplerFilterCaps(KG3DEngineOption& EngineOption)
{
    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();
    
    m_dwMinAnisotropy = 1;
    m_dwMaxAnisotropy = D3DCaps.MaxAnisotropy;

    m_nMinSampMipFilter = D3DTEXF_POINT;
    m_nMinSampMinFilter = D3DTEXF_POINT;
    m_nMinSampMagFilter = D3DTEXF_POINT;

    m_nMaxSampMipFilter = D3DTEXF_POINT;
    m_nMaxSampMinFilter = D3DTEXF_POINT;
    m_nMaxSampMagFilter = D3DTEXF_POINT;

    if (EngineOption.dwMaxAnisotropy <= 1)
    {
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
            m_nMaxSampMipFilter = D3DTEXF_LINEAR;

        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
            m_nMaxSampMinFilter = D3DTEXF_LINEAR;

        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
            m_nMaxSampMagFilter = D3DTEXF_LINEAR;
    }
    else
    {
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
            m_nMaxSampMipFilter = D3DTEXF_LINEAR;

        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
            m_nMaxSampMinFilter = D3DTEXF_ANISOTROPIC;
        else if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
            m_nMaxSampMinFilter = D3DTEXF_LINEAR;

        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
            m_nMaxSampMagFilter = D3DTEXF_ANISOTROPIC;
        else if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
            m_nMaxSampMagFilter = D3DTEXF_LINEAR;
    }
    return S_OK;
}

HRESULT KG3DEngineOptionCaps::GetRenderGrassCaps()
{
    m_bRenderGrass = TRUE;
    return S_OK;
}

HRESULT KG3DEngineOptionCaps::GetGrassAnimationCaps(KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    m_bGrassAnimation = TRUE;

    if (!EngineOption.bRenderGrass)
        m_bGrassAnimation = FALSE;
    
    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bGrassAnimation = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetGrassAlphaBlendCaps(KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bGrassAlphaBlend = TRUE;

    if (!EngineOption.bRenderGrass)
        m_bGrassAlphaBlend = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetTextureScaleCaps()
{
    HRESULT hResult = E_FAIL;

    m_nMinTextureScale = 4;
    m_nMaxTextureScale = 1;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetCameraDistanceCaps()
{
    HRESULT hResult = E_FAIL;

    m_fMinCameraDistance = 20000.00;
    m_fMaxCameraDistance = 80000.00;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetCameraAngleCaps()
{
    HRESULT hResult = E_FAIL;

    m_fMinCameraAngle = D3DX_PI / 6.0f;
    m_fMaxCameraAngle = D3DX_PI / 2.0f;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetPostEffectEnableCaps(D3DCAPS9 D3DCaps)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    m_bPostEffectEnable = TRUE;

    KG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);
    KG_PROCESS_ERROR(g_pd3dDevice);

    if (g_cGraphicsEngine.m_PresentParam.MultiSampleType != D3DMULTISAMPLE_NONE)
    {
        hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
            D3DCaps.DeviceType,D3DFMT_R32F, g_cGraphicsEngine.m_PresentParam.Windowed, 
            g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
        if (FAILED(hRetCode))
        {
            hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
                D3DCaps.DeviceType,D3DFMT_A16B16G16R16F, g_cGraphicsEngine.m_PresentParam.Windowed, 
                g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
            if (FAILED(hRetCode))
                m_bPostEffectEnable = FALSE;
        }
        if (m_bPostEffectEnable)
        {
            D3DDISPLAYMODE DisplayMode;

            g_cGraphicsEngine.m_pD3D->GetAdapterDisplayMode(D3DCaps.AdapterOrdinal, &DisplayMode);
            hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
                D3DCaps.AdapterOrdinal, 
                D3DCaps.DeviceType, 
                DisplayMode.Format, 
                D3DUSAGE_DEPTHSTENCIL, 
                D3DRTYPE_TEXTURE, 
                g_cGraphicsEngine.m_PresentParam.AutoDepthStencilFormat
                );
            if (FAILED(hRetCode))
                m_bPostEffectEnable = FALSE;
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetBloomEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT  hResult = E_FAIL;

    m_bBloomEnable = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bBloomEnable = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bBloomEnable = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        m_bBloomEnable = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetHDREnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT  hResult = E_FAIL;

    m_bHDREnable = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bHDREnable = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bHDREnable = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        m_bHDREnable = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetDOFEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT  hResult = E_FAIL;

    m_bDOFEnable = TRUE;
    if (!EngineOption.bPostEffectEnable)
        m_bDOFEnable = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bDOFEnable = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        m_bDOFEnable = FALSE;

    hResult = S_OK;
    // Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetShockWaveEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bShockWaveEnable = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bShockWaveEnable = FALSE;
    
    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bShockWaveEnable = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_bShockWaveEnable = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetHSIEnableCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bHSIEnable = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bHSIEnable = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bHSIEnable = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_bHSIEnable = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetMotionBlurCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bMotionBlur = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bMotionBlur = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bMotionBlur = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_bMotionBlur = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetDetailCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bDetail = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bDetail = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bDetail = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_bDetail = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetGodRayCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;
    
    m_bGodRay = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bGodRay = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bGodRay = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        m_bGodRay = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetSSAOCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bSSAO = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bSSAO = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bSSAO = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        m_bSSAO = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetGlobalFogCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bGlobalFog = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bGlobalFog = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bGlobalFog = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_bGlobalFog = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetCurveCMYKCaps(D3DCAPS9 D3DCaps, KG3DEngineOption& EngineOption)
{
    HRESULT hResult = E_FAIL;

    m_bCurveCMYK = TRUE;

    if (!EngineOption.bPostEffectEnable)
        m_bCurveCMYK = FALSE;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_bCurveCMYK = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_bCurveCMYK = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetPostEffectCaps(KG3DEngineOption& EngineOption)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    hRetCode = GetPostEffectEnableCaps(D3DCaps);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetBloomEnableCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetHDREnableCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetDOFEnableCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    
    hRetCode = GetShockWaveEnableCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetHSIEnableCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetMotionBlurCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetDetailCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetGodRayCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetSSAOCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetGlobalFogCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetCurveCMYKCaps(D3DCaps, EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetModelShadowTypeCaps(KG3DEngineOption& EngineOption)
{
    HRESULT hRetCode = E_FAIL;
    HRESULT hResult  = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    for (int i = 0; i < 4; i++)
        m_nModelShadowType[i] = i;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0) || 
        D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0)
    )
    {
        m_nModelShadowType[1] = -1;
        m_nModelShadowType[2] = -1;
        m_nModelShadowType[3] = -1;
    }
    if (!(D3DCaps.StencilCaps & D3DSTENCILCAPS_DECR) ||
        !(D3DCaps.StencilCaps & D3DSTENCILCAPS_INCR)
    )
    {
        m_nModelShadowType[1] = -1;
    }

    if (!EngineOption.bPostEffectEnable)
    {
        m_nModelShadowType[2] = -1;
        m_nModelShadowType[3] = -1;
    }
    else
    {
        m_nModelShadowType[1] = -1;
    }

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
    {
        m_nModelShadowType[2] = -1;
        m_nModelShadowType[3] = -1;
    }

    if (m_nModelShadowType[2] != -1 || m_nModelShadowType[3] != -1)
    {
        D3DCAPS9       DevCaps;
        D3DDISPLAYMODE DisplayMode;

        KG_PROCESS_ERROR(g_pd3dDevice);
        KG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

        hRetCode = g_pd3dDevice->GetDeviceCaps(&DevCaps);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_cGraphicsEngine.m_pD3D->GetAdapterDisplayMode(DevCaps.AdapterOrdinal, &DisplayMode);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
            DevCaps.AdapterOrdinal, 
            DevCaps.DeviceType, 
            DisplayMode.Format, 
            D3DUSAGE_DEPTHSTENCIL, 
            D3DRTYPE_TEXTURE, 
            D3DFMT_D24S8
            );
        if (FAILED(hRetCode))
        {
            hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
                DevCaps.AdapterOrdinal, 
                DevCaps.DeviceType, 
                DisplayMode.Format, 
                D3DUSAGE_DEPTHSTENCIL, 
                D3DRTYPE_TEXTURE, 
                D3DFMT_D16
                );
            if (FAILED(hRetCode))
            {
                m_nModelShadowType[2] = -1;
                m_nModelShadowType[3] = -1;
            }
        }
    }

    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        for (int i = 0; i < _countof(m_nModelShadowType); ++i)
        {
            m_nModelShadowType[i] = -1;
        }
    }
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetEnableDynamicConverMapCaps()
{
    m_bEnableDynamicConverMap = TRUE;
    return S_OK;
}

HRESULT KG3DEngineOptionCaps::GetCpuSkinCaps()
{
    HRESULT hResult = E_FAIL;    
    SYSTEM_INFO SystemInfo;

    m_bCpuSkin = TRUE;
    GetSystemInfo(&SystemInfo);

    if (SystemInfo.dwNumberOfProcessors <= 1)
        m_bCpuSkin = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetNumCpuThreadCaps()
{
    HRESULT hResult = E_FAIL;

    m_nMaxNumCpuThread = 8;
    m_nMinNumCpuThread = 1;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetFlexBodySmoothCaps()
{
    m_bFlexBodySmooth = TRUE;
    return S_OK;
}

HRESULT KG3DEngineOptionCaps::GetTerrainDetailCaps()
{
    HRESULT hResult = E_FAIL;

    m_nMaxTerrainDetail = 0;
    m_nMinTerrainDetail = 3;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetVegetationDensityCaps()
{
    HRESULT hResult = E_FAIL;

    m_nMaxVegetationDensity = 1;
    m_nMinVegetationDensity = 8;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetWaterDetailCaps()
{
    HRESULT   hResult = E_FAIL;
    D3DCAPS9  D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    m_nMinWaterDetail = WD_LOWER;
    m_nMaxWaterDetail = WD_FULL_EFFECTIVE;
    
    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        m_nMaxWaterDetail = WD_LOWER;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        m_nMaxWaterDetail = WD_LOWER;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(3, 0))
        m_nMaxWaterDetail = WD_MEDIUM;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        m_nMaxWaterDetail = WD_MEDIUM;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT KG3DEngineOptionCaps::GetEngineOptionCaps(KG3DEngineOption &EngineOption)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = GetMultiSampleTypeListCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetTextureSamplerFilterCaps(EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetRenderGrassCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetGrassAnimationCaps(EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetGrassAlphaBlendCaps(EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetTextureScaleCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetCameraDistanceCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetPostEffectCaps(EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetModelShadowTypeCaps(EngineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetCpuSkinCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetNumCpuThreadCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetFlexBodySmoothCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetTerrainDetailCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetVegetationDensityCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetWaterDetailCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetShaderModelCaps();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

std::vector<KADAPTERMODE>* KG3DEngineOptionCaps::GetAdapterModes()
{
    HRESULT hRetCode = E_FAIL;
    UINT uAdapater = D3DADAPTER_DEFAULT;
    D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

    hRetCode = GetAdapater(&uAdapater, &DeviceType);
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = GetAdaperModeCaps(uAdapater, g_cGraphicsEngine.m_DisplayMode.Format);
    KG_COM_PROCESS_ERROR(hRetCode);

Exit0:
    return &m_AdapterModes;
}

std::vector<int>* KG3DEngineOptionCaps::GetMultiSampleType()
{
    return &m_vecMutliSampleType;
}

DWORD KG3DEngineOptionCaps::GetMaxAnisotropy()
{
    return m_dwMaxAnisotropy;
}

DWORD KG3DEngineOptionCaps::GetMinAnisotropy()
{
    return m_dwMinAnisotropy;
}

BOOL KG3DEngineOptionCaps::GetRenderGrass()	
{
    return m_bRenderGrass;
}

BOOL KG3DEngineOptionCaps::GetGrassAnimation()
{
    return m_bGrassAnimation;
}

BOOL KG3DEngineOptionCaps::GetGrassAlphaBlend()
{
    return m_bGrassAlphaBlend;
}

int  KG3DEngineOptionCaps::GetMinTextureScale()
{
    return m_nMinTextureScale;
}

int  KG3DEngineOptionCaps::GetMaxTextureScale()
{
    return m_nMaxTextureScale;
}

float KG3DEngineOptionCaps::GetMinCameraDistance()
{
    return m_fMinCameraDistance;
}

float KG3DEngineOptionCaps::GetMaxCameraDistance()
{
    return m_fMaxCameraDistance;
}

float KG3DEngineOptionCaps::GetMinCameraAngle()
{
    return m_fMinCameraAngle;
}

float KG3DEngineOptionCaps::GetMaxCameraAngle()
{
    return m_fMaxCameraAngle;
}

BOOL KG3DEngineOptionCaps::GetPostEffectEnable()
{
    return m_bPostEffectEnable;
}

BOOL KG3DEngineOptionCaps::GetBloomEnable()
{
    return m_bBloomEnable;
}

BOOL KG3DEngineOptionCaps::GetHDREnable()
{
    return m_bHDREnable;
}

BOOL KG3DEngineOptionCaps::GetDOFEnable()
{
    return m_bDOFEnable;
}

BOOL KG3DEngineOptionCaps::GetShockWaveEnable()
{
    return m_bShockWaveEnable;
}

BOOL KG3DEngineOptionCaps::GetHSIEnable()
{
    return m_bHSIEnable;
}

BOOL KG3DEngineOptionCaps::GetMotionBlur()
{
    return m_bMotionBlur;
}

BOOL KG3DEngineOptionCaps::GetDetail()
{
    return m_bDetail;
}

BOOL KG3DEngineOptionCaps::GetGodRay()
{
    return m_bGodRay;
}

BOOL KG3DEngineOptionCaps::GetSSAO()
{
    return m_bSSAO;
}

BOOL KG3DEngineOptionCaps::GetGlobalFog()
{
    return m_bGlobalFog;
}

BOOL KG3DEngineOptionCaps::GetCurveCMYK()
{
    return m_bCurveCMYK;
}

int*  KG3DEngineOptionCaps::GetModelShadowType()
{
    return m_nModelShadowType;
}

BOOL KG3DEngineOptionCaps::GetEnableDynamicConverMap()
{
    return m_bEnableDynamicConverMap;
}

BOOL KG3DEngineOptionCaps::GetCpuSkin()
{
    return m_bCpuSkin;
}

int  KG3DEngineOptionCaps::GetMaxNumCpuThread()
{
    return m_nMaxNumCpuThread;
}

int  KG3DEngineOptionCaps::GetMinNumCpuThread()
{
    return m_nMinNumCpuThread;
}

BOOL KG3DEngineOptionCaps::GetFlexBodySmooth()
{
    return m_bFlexBodySmooth;
}

int  KG3DEngineOptionCaps::GetMaxTerrainDetail()
{
    return m_nMaxTerrainDetail;
}

int  KG3DEngineOptionCaps::GetMinTerrainDetail()
{
    return m_nMinTerrainDetail;
}

int  KG3DEngineOptionCaps::GetMaxVegetationDensity()
{
    return m_nMaxVegetationDensity;
}

int  KG3DEngineOptionCaps::GetMinVegetationDensity()
{
    return m_nMinVegetationDensity;
}

int  KG3DEngineOptionCaps::GetMinWaterDetail()
{
    return m_nMinWaterDetail;
}

int  KG3DEngineOptionCaps::GetMaxWaterDetail()
{
    return m_nMaxWaterDetail;
}

HRESULT KG3DEngineOptionCaps::GetShaderModelCaps()
{
    HRESULT hResult = E_FAIL;
    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    m_vecShaderModel.clear();
    m_vecShaderModel.push_back(0);

    if (D3DCaps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
        m_vecShaderModel.push_back(2);
    if (D3DCaps.PixelShaderVersion >= D3DPS_VERSION(3, 0))
        m_vecShaderModel.push_back(3);

    hResult = S_OK;
    return hResult;
}

std::vector<int>* KG3DEngineOptionCaps::GetShaderModel()
{
    return &m_vecShaderModel;
}