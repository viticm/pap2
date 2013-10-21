#include "stdafx.h"
#include "./KG3DEngineOption.h"
#include "./KG3DGraphiceEngine.h"

HRESULT ValidateTextureSamplerFilter(KG3DEngineOption& Option)
{
    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    switch (Option.nSampMinFilter)
    {
    default:
    case D3DTEXF_GAUSSIANQUAD:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD)
        {
            Option.nSampMinFilter = D3DTEXF_GAUSSIANQUAD;
            break;
        }
    case D3DTEXF_PYRAMIDALQUAD:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD)
        {
            Option.nSampMinFilter = D3DTEXF_PYRAMIDALQUAD;
            break;
        }
    case D3DTEXF_ANISOTROPIC:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
        {
            Option.nSampMinFilter = D3DTEXF_ANISOTROPIC;
            break;
        }
    case D3DTEXF_LINEAR:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
        {
            Option.nSampMinFilter = D3DTEXF_LINEAR;
            break;
        }
    case D3DTEXF_POINT:
    case D3DTEXF_NONE:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)
        {
            Option.nSampMinFilter = D3DTEXF_POINT;
            break;
        }
    }

    switch (Option.nSampMagFilter)
    {
    default:
    case D3DTEXF_GAUSSIANQUAD:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD)
        {
            Option.nSampMagFilter = D3DTEXF_GAUSSIANQUAD;
            break;
        }
    case D3DTEXF_PYRAMIDALQUAD:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD)
        {
            Option.nSampMagFilter = D3DTEXF_PYRAMIDALQUAD;
            break;
        }
    case D3DTEXF_ANISOTROPIC:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
        {
            Option.nSampMagFilter = D3DTEXF_ANISOTROPIC;
            break;
        }
    case D3DTEXF_LINEAR:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
        {
            Option.nSampMagFilter = D3DTEXF_LINEAR;
            break;
        }
    case D3DTEXF_POINT:
    case D3DTEXF_NONE:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)
        {
            Option.nSampMagFilter = D3DTEXF_POINT;
            break;
        }
    }

    switch (Option.nSampMipFilter)
    {
    default:
    case D3DTEXF_LINEAR:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
        {
            Option.nSampMipFilter = D3DTEXF_LINEAR;
            break;
        }
    case D3DTEXF_POINT:
    case D3DTEXF_NONE:
        if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
        {
            Option.nSampMipFilter = D3DTEXF_POINT;
            break;
        }
    }

    return S_OK;
}

HRESULT ValidateTextureMaxAnisotory(KG3DEngineOption& Option)
{
	D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();
	if (D3DCaps.MaxAnisotropy < Option.dwMaxAnisotropy)
		Option.dwMaxAnisotropy = D3DCaps.MaxAnisotropy;

    if (Option.dwMaxAnisotropy < 1)
        Option.dwMaxAnisotropy = 1;

	if (Option.dwMaxAnisotropy <= 1)
	{
		if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
		{
			Option.nSampMinFilter = D3DTEXF_LINEAR;
		}
		else 
		{
			Option.nSampMinFilter = D3DTEXF_POINT;
		}

		if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
		{
			Option.nSampMagFilter = D3DTEXF_LINEAR;		
		}
		else
		{
			Option.nSampMagFilter = D3DTEXF_POINT;
		}

		if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		{
			Option.nSampMipFilter = D3DTEXF_LINEAR;
		}
		else
		{
			Option.nSampMipFilter = D3DTEXF_POINT;
		}

	}
	else
	{
		if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
		{
			Option.nSampMinFilter = D3DTEXF_ANISOTROPIC;
		}
		else if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
		{
			Option.nSampMinFilter = D3DTEXF_LINEAR;
		}
		else 
		{
			Option.nSampMinFilter = D3DTEXF_POINT;
		}

		if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
		{
			Option.nSampMagFilter = D3DTEXF_ANISOTROPIC;
		}
		else if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
		{
			Option.nSampMagFilter = D3DTEXF_LINEAR;		
		}
		else
		{
			Option.nSampMagFilter = D3DTEXF_POINT;
		}

		if (D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		{
			Option.nSampMipFilter = D3DTEXF_LINEAR;
		}
		else
		{
			Option.nSampMipFilter = D3DTEXF_POINT;
		}
	}
	return S_OK;
}
HRESULT ValidateTextureOption(KG3DEngineOption& Option)
{
	if (Option.nTextureScale <= 0)
		Option.nTextureScale = 2;

    return S_OK;
}

HRESULT ValidateCameraOption(KG3DEngineOption& Option)
{
	if (Option.fCameraDistance < 20000.0f)
        Option.fCameraDistance = 20000.0f;

    if (Option.fCameraDistance > 80000.0f)
        Option.fCameraDistance = 80000.0f;

    if (Option.fCameraAngle < D3DX_PI / 6.0f)
        Option.fCameraAngle = D3DX_PI / 6.0f;

    if (Option.fCameraAngle > D3DX_PI / 2.0f)
        Option.fCameraAngle = D3DX_PI / 2.0f;

    return S_OK;
}

HRESULT ValidateBloomOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bBloomEnable)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bBloomEnable = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
            Option.bBloomEnable = FALSE;
    }

    return S_OK;
}

HRESULT ValidateHDROption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
	if (Option.bHDR)
	{
		if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
			Option.bHDR = FALSE;
		if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
			Option.bHDR = FALSE;
	}

	return S_OK;
}

HRESULT ValidateDOFOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
	if (Option.bDOF)
	{
		if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
			Option.bDOF = FALSE;
		if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
			Option.bDOF = FALSE;
	}

	return S_OK;
}

HRESULT ValidateShockWaveOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bShockWaveEnable)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bShockWaveEnable = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
            Option.bShockWaveEnable = FALSE;
    }
    return S_OK;
}

HRESULT ValidateHSIOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bHSIEnable)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bHSIEnable = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
            Option.bHSIEnable = FALSE;
    }
    return S_OK;
}

HRESULT ValidateMotionBlurOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bMotionBlur)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bMotionBlur = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
            Option.bMotionBlur = FALSE;
    }
    return S_OK;
}

HRESULT ValidateDetailOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bDetail)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bDetail = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
            Option.bDetail = FALSE;
    }
    return S_OK;
}

HRESULT ValidateGodRayOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bGodRay)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bGodRay = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
            Option.bGodRay = FALSE;
    }
    return S_OK;
}

HRESULT ValidateSSAOOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bSSAO)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bSSAO = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
            Option.bSSAO = FALSE;
    }
    return S_OK;
}

HRESULT ValidateGlobalFogOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bGlobalFog)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bGlobalFog = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
            Option.bGlobalFog = FALSE;
    }
    return S_OK;
}

HRESULT ValidateCurveCMYKOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option)
{
    if (Option.bCurveCMYK)
    {
        if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
            Option.bCurveCMYK = FALSE;
        if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
            Option.bCurveCMYK = FALSE;
    }
    return S_OK;
}

HRESULT ValidatePostEffectOption(KG3DEngineOption& Option)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

	if (Option.bPostEffectEnable && g_cGraphicsEngine.m_PresentParam.MultiSampleType != D3DMULTISAMPLE_NONE)
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
				Option.bPostEffectEnable = FALSE;
		}
		if (Option.bPostEffectEnable)
		{
			D3DDISPLAYMODE     displayMode;
			KG_PROCESS_ERROR(g_pd3dDevice);
			KG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

			g_cGraphicsEngine.m_pD3D->GetAdapterDisplayMode(D3DCaps.AdapterOrdinal, &displayMode);
			hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
				D3DCaps.AdapterOrdinal, 
				D3DCaps.DeviceType, 
				displayMode.Format, 
				D3DUSAGE_DEPTHSTENCIL, 
				D3DRTYPE_TEXTURE, 
				g_cGraphicsEngine.m_PresentParam.AutoDepthStencilFormat
				);
			if (FAILED(hRetCode))
				Option.bPostEffectEnable = FALSE;
		}
	}

    hRetCode = ValidateBloomOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = ValidateHDROption(D3DCaps, Option);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateShockWaveOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateHSIOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateMotionBlurOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateDetailOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateGodRayOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateSSAOOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateGlobalFogOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = ValidateDOFOption(D3DCaps, Option);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateCurveCMYKOption(D3DCaps, Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT ValidateModelShadowTypeOption(KG3DEngineOption& Option)
{
    HRESULT hRetCode = E_FAIL;
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
	if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0) && EM_MODEL_SHADOW_TYPE_MIDDLE <= Option.ModelShadowType)
		Option.ModelShadowType = EM_MODEL_SHADOW_TYPE_LOW;

	if (EM_MODEL_SHADOW_TYPE_MIDDLE <= Option.ModelShadowType)
	{
		D3DDISPLAYMODE     displayMode;
		D3DCAPS9           devCaps;
		KG_PROCESS_ERROR(g_pd3dDevice);
		KG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

		hRetCode = g_pd3dDevice->GetDeviceCaps(&devCaps);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = g_cGraphicsEngine.m_pD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
			devCaps.AdapterOrdinal, 
			devCaps.DeviceType, 
			displayMode.Format, 
			D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_TEXTURE, 
			D3DFMT_D24S8
			);
		if (FAILED(hRetCode))
		{
			hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
				devCaps.AdapterOrdinal, 
				devCaps.DeviceType, 
				displayMode.Format, 
				D3DUSAGE_DEPTHSTENCIL, 
				D3DRTYPE_TEXTURE, 
				D3DFMT_D16
				);
			if (FAILED(hRetCode))
				Option.ModelShadowType = EM_MODEL_SHADOW_TYPE_LOW;
		}
	}

   	if(EM_MODEL_SHADOW_TYPE_LOW == Option.ModelShadowType)
	{
		bool bStencilAvailable = (D3DCaps.StencilCaps & D3DSTENCILCAPS_INCR) 
			&& (D3DCaps.StencilCaps & D3DSTENCILCAPS_DECR);
		if(! bStencilAvailable)
		{
			 Option.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
		}
	}

    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        Option.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
    }
    return hResult;
}

HRESULT ValidateGrassAnimationOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bGrassAnimation = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateGrassAlphaBlendOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    if (!Option.bRenderGrass)
        Option.bGrassAlphaBlend = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterReflectionOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bWaterReflection = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.bWaterReflection = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterReflectEnvironmentOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bWaterReflectEnvironment = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.bWaterReflectEnvironment = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterReflectGroundOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bWaterReflectGround = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.bWaterReflectGround = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterReflectNPCOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bWaterReflectNPC = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.bWaterReflectNPC = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterReflectEntityOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bWaterReflectEntity = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.bWaterReflectEntity = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterRefractionOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.bWaterRefraction = FALSE;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.bWaterRefraction = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateWaterOption(KG3DEngineOption& Option)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = ValidateWaterReflectionOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateWaterReflectEnvironmentOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateWaterReflectGroundOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateWaterReflectNPCOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    
    hRetCode = ValidateWaterReflectEntityOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateWaterRefractionOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT ValidateWaterDetailOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

    if (Option.nWaterDetail < 0)
        Option.nWaterDetail = 0;

    if (Option.nWaterDetail > 4)
        Option.nWaterDetail = 4;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(2, 0))
        Option.nWaterDetail = 0;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
        Option.nWaterDetail = 0;

    if (D3DCaps.VertexShaderVersion < D3DVS_VERSION(3, 0) &&
        Option.nWaterDetail >= WD_HIGH
    )
        Option.nWaterDetail = WD_MEDIUM;

    if (D3DCaps.PixelShaderVersion < D3DPS_VERSION(3, 0) &&
        Option.nWaterDetail >= WD_HIGH
    )
        Option.nWaterDetail = WD_MEDIUM;
    
    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateVegetationDensityOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;
    
    if (Option.nVegetationDensity > 8)
        Option.nVegetationDensity = 8;

    if (Option.nVegetationDensity < 1)
        Option.nVegetationDensity = 1;
    
    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateCpuSkinOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;
    SYSTEM_INFO SystemInfo;

    GetSystemInfo(&SystemInfo);

    if (SystemInfo.dwNumberOfProcessors <= 1)
        Option.bCpuSkin = FALSE;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateNumCpuThreadOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    if (Option.nNumCpuThread > 8)
        Option.nNumCpuThread = 8;

    if (Option.nNumCpuThread < 1)
        Option.nNumCpuThread = 1;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateTerrainDetailOption(KG3DEngineOption& Option)
{
    HRESULT hResult = E_FAIL;

    if (Option.nTerrainDetail > 3)
        Option.nTerrainDetail = 3;

    if (Option.nTerrainDetail < 0)
        Option.nTerrainDetail = 0;

    hResult = S_OK;
// Exit0:
    return hResult;
}

HRESULT ValidateAfterInitDeviceEngineOption(KG3DEngineOption& Option)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = ValidateCameraOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateTextureOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    //hRetCode = ValidateTextureSamplerFilter(Option);
    //KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = ValidateTextureMaxAnisotory(Option);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidatePostEffectOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateModelShadowTypeOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateGrassAnimationOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateGrassAlphaBlendOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateWaterOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateWaterDetailOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateVegetationDensityOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateCpuSkinOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateNumCpuThreadOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ValidateTerrainDetailOption(Option);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT ValidateEngineOption(KG3DEngineOption& Option)
{
    HRESULT hr = E_FAIL;

    hr = ValidateCameraOption(Option);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = ValidateTextureOption(Option);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

