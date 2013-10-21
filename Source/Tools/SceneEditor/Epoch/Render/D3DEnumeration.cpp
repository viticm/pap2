//-----------------------------------------------------------------------------
// File: D3DEnumeration.cpp
//
// Desc: Enumerates D3D adapters, devices, modes, etc.
//       Modified by Dolaham
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include<windows.h>
//#include"DXUtil.h"
#include"D3DEnumeration.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

//-----------------------------------------------------------------------------
// Name: ColorChannelBits
// Desc: Returns the number of color channel bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
static UINT ColorChannelBits(D3DFORMAT fmt)
{
    switch( fmt )
    {
        case D3DFMT_R8G8B8:
            return 8;
        case D3DFMT_A8R8G8B8:
            return 8;
        case D3DFMT_X8R8G8B8:
            return 8;
        case D3DFMT_R5G6B5:
            return 5;
        case D3DFMT_X1R5G5B5:
            return 5;
        case D3DFMT_A1R5G5B5:
            return 5;
        case D3DFMT_A4R4G4B4:
            return 4;
        case D3DFMT_R3G3B2:
            return 2;
        case D3DFMT_A8R3G3B2:
            return 2;
        case D3DFMT_X4R4G4B4:
            return 4;
        case D3DFMT_A2B10G10R10:
            return 10;
        case D3DFMT_A2R10G10B10:
            return 10;
        default:
            return 0;
    }
}


//-----------------------------------------------------------------------------
// Name: AlphaChannelBits
// Desc: Returns the number of alpha channel bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
static UINT AlphaChannelBits( D3DFORMAT fmt )
{
    switch( fmt )
    {
        case D3DFMT_R8G8B8:
            return 0;
        case D3DFMT_A8R8G8B8:
            return 8;
        case D3DFMT_X8R8G8B8:
            return 0;
        case D3DFMT_R5G6B5:
            return 0;
        case D3DFMT_X1R5G5B5:
            return 0;
        case D3DFMT_A1R5G5B5:
            return 1;
        case D3DFMT_A4R4G4B4:
            return 4;
        case D3DFMT_R3G3B2:
            return 0;
        case D3DFMT_A8R3G3B2:
            return 8;
        case D3DFMT_X4R4G4B4:
            return 0;
        case D3DFMT_A2B10G10R10:
            return 2;
        case D3DFMT_A2R10G10B10:
            return 2;
        default:
            return 0;
    }
}




//-----------------------------------------------------------------------------
// Name: DepthBits
// Desc: Returns the number of depth bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
static UINT DepthBits( D3DFORMAT fmt )
{
    switch( fmt )
    {
        case D3DFMT_D16:
            return 16;
        case D3DFMT_D15S1:
            return 15;
        case D3DFMT_D24X8:
            return 24;
        case D3DFMT_D24S8:
            return 24;
        case D3DFMT_D24X4S4:
            return 24;
        case D3DFMT_D32:
            return 32;
        default:
            return 0;
    }
}




//-----------------------------------------------------------------------------
// Name: StencilBits
// Desc: Returns the number of stencil bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
static UINT StencilBits( D3DFORMAT fmt )
{
    switch( fmt )
    {
        case D3DFMT_D16:
            return 0;
        case D3DFMT_D15S1:
            return 1;
        case D3DFMT_D24X8:
            return 0;
        case D3DFMT_D24S8:
            return 8;
        case D3DFMT_D24X4S4:
            return 4;
        case D3DFMT_D32:
            return 0;
        default:
            return 0;
    }
}


//-----------------------------------------------------------------------------
// Name: SortModesCallback
// Desc: Used to sort D3DDISPLAYMODEs
//-----------------------------------------------------------------------------
static int __cdecl SortModesCallback( const void* arg1, const void* arg2 )
{
    D3DDISPLAYMODE* pdm1 = (D3DDISPLAYMODE*)arg1;
    D3DDISPLAYMODE* pdm2 = (D3DDISPLAYMODE*)arg2;

	if (pdm1->Format > pdm2->Format)
        return 1;
    if (pdm1->Format < pdm2->Format)
        return -1;

    if (pdm1->Width > pdm2->Width)
        return 1;
    if (pdm1->Width < pdm2->Width)
        return -1;

    if (pdm1->Height > pdm2->Height)
        return 1;
    if (pdm1->Height < pdm2->Height)
        return -1;

    if (pdm1->RefreshRate > pdm2->RefreshRate)
        return 1;
    if (pdm1->RefreshRate < pdm2->RefreshRate)
        return -1;

    return 0;
}



//-----------------------------------------------------------------------------
// Name: D3DEnumeration constructor
// Desc:
//-----------------------------------------------------------------------------
D3DEnumeration::D3DEnumeration(): m_pD3D(0)
{
	ConfirmDeviceCallback = 0;  // The device confirm function pointer
    AppMinFullscreenWidth = 640;
    AppMinFullscreenHeight = 480;
    AppMinColorChannelBits = 5;
    AppMinAlphaChannelBits = 0;
    AppMinDepthBits = 15;
    AppMinStencilBits = 0;
    AppUsesDepthBuffer = false;
    AppUsesMixedVP = false;
    AppRequiresWindowed = false;
    AppRequiresFullscreen = false;
}



//-----------------------------------------------------------------------------
// Name: Enumerate
// Desc: Enumerates available D3D adapters, devices, modes, etc.
//-----------------------------------------------------------------------------
HRESULT D3DEnumeration::Enumerate()
{
	if(!m_pD3D)
	{
		return E_FAIL;
	}

    HRESULT hr;

	m_AllowedAdapterFormatList.push_back(D3DFMT_X8R8G8B8);
	m_AllowedAdapterFormatList.push_back(D3DFMT_R5G6B5);
	m_AllowedAdapterFormatList.push_back(D3DFMT_X1R5G5B5);
	m_AllowedAdapterFormatList.push_back(D3DFMT_A2R10G10B10);  // Another 32 bits display format, 10 bits per channel, cool!!!

    UINT numAdapters = m_pD3D->GetAdapterCount();
	for(UINT adapterOrdinal = 0; adapterOrdinal < numAdapters; ++adapterOrdinal)
    {
		D3DAdapterInfo AdapterInfo;

        AdapterInfo.AdapterOrdinal = adapterOrdinal;
		m_pD3D->GetAdapterIdentifier(adapterOrdinal, 0, &AdapterInfo.AdapterIdentifier);

        // Get list of all display modes on this adapter.
        // Also build a temporary list of all display adapter formats.
        vector<D3DFORMAT> adapterFormatList;
        for( UINT iaaf = 0; iaaf < m_AllowedAdapterFormatList.size(); ++iaaf)
        {
            D3DFORMAT allowedAdapterFormat = m_AllowedAdapterFormatList[iaaf];

            UINT numAdapterModes = m_pD3D->GetAdapterModeCount(adapterOrdinal, allowedAdapterFormat);
            for(UINT mode = 0; mode < numAdapterModes; ++mode)
            {
                D3DDISPLAYMODE displayMode;
                m_pD3D->EnumAdapterModes( adapterOrdinal, allowedAdapterFormat, mode, &displayMode);
                if( displayMode.Width < AppMinFullscreenWidth ||
                    displayMode.Height < AppMinFullscreenHeight ||
                    ColorChannelBits(displayMode.Format) < AppMinColorChannelBits )
                {
                    continue;
                }

                AdapterInfo.DisplayModeList.push_back(displayMode);
				bool bContain = false;
				for(unsigned int i = 0; i < adapterFormatList.size(); ++i)
				{
					if(adapterFormatList[i] == displayMode.Format)
					{
						bContain = true;
						break;
					}
				}
				if(!bContain)
				{
					adapterFormatList.push_back(displayMode.Format);
				}
            }
        }

        // Sort displaymode list
		qsort(&AdapterInfo.DisplayModeList[0], AdapterInfo.DisplayModeList.size(), sizeof(D3DDISPLAYMODE), SortModesCallback);

		// Get info for each device on this adapter
        if(FAILED(hr = EnumerateDevices(AdapterInfo, adapterFormatList)))
        {
            return hr;
        }

        // If at least one device on this adapter is available and compatible
        // with the app, add the adapterInfo to the list
        if(AdapterInfo.DeviceInfoList.size() > 0)
		{
			m_AdapterInfoList.push_back(AdapterInfo);
		}
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: EnumerateDevices
// Desc: Enumerates D3D devices for a particular adapter.
//-----------------------------------------------------------------------------
HRESULT D3DEnumeration::EnumerateDevices(D3DAdapterInfo& AdapterInfo, const vector<D3DFORMAT>& AdapterFormatList)
{
	const D3DDEVTYPE devTypeArray[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_SW, D3DDEVTYPE_REF };
	const UINT devTypeArrayCount = sizeof(devTypeArray) / sizeof(devTypeArray[0]);
    HRESULT hr;

	for(UINT idt = 0; idt < devTypeArrayCount; ++idt)
    {
        D3DDeviceInfo DeviceInfo;

		DeviceInfo.AdapterOrdinal = AdapterInfo.AdapterOrdinal;
        DeviceInfo.DevType = devTypeArray[idt];
        if(FAILED(m_pD3D->GetDeviceCaps(AdapterInfo.AdapterOrdinal, DeviceInfo.DevType, &DeviceInfo.Caps)))
        {
			continue;
        }

        // Get info for each devicecombo on this device
        if(FAILED(hr = EnumerateDeviceCombos(DeviceInfo, AdapterFormatList)))
        {
			return hr;
        }

        // If at least one devicecombo for this device is found,
        // add the deviceInfo to the list
        if(DeviceInfo.DeviceComboList.empty())
        {
			continue;
		}
		AdapterInfo.DeviceInfoList.push_back(DeviceInfo);
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: EnumerateDeviceCombos
// Desc: Enumerates DeviceCombos for a particular device.
//-----------------------------------------------------------------------------
HRESULT D3DEnumeration::EnumerateDeviceCombos(D3DDeviceInfo& DeviceInfo, const vector<D3DFORMAT>& AdapterFormatList)
{
    const D3DFORMAT backBufferFormatArray[] = {
		D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10,
		D3DFMT_R5G6B5, D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5 };
    const UINT backBufferFormatArrayCount = sizeof(backBufferFormatArray) / sizeof(backBufferFormatArray[0]);
    bool isWindowedArray[] = { false, true };

    // See which adapter formats are supported by this device
    for(UINT iaf = 0; iaf < AdapterFormatList.size(); ++iaf)
    {
		D3DFORMAT adapterFormat = AdapterFormatList[iaf];

        for( UINT ibbf = 0; ibbf < backBufferFormatArrayCount; ++ibbf)
        {
			D3DFORMAT backBufferFormat = backBufferFormatArray[ibbf];
            if(AlphaChannelBits(backBufferFormat) < AppMinAlphaChannelBits)
			{
				continue;
			}

            for(UINT iiw = 0; iiw < 2; ++iiw)
            {
				bool isWindowed = isWindowedArray[iiw];

				if(!isWindowed && AppRequiresWindowed)
				{
					continue;
				}
                if(isWindowed && AppRequiresFullscreen)
				{
					continue;
				}
				if(FAILED(m_pD3D->CheckDeviceType(DeviceInfo.AdapterOrdinal, DeviceInfo.DevType, adapterFormat, backBufferFormat, isWindowed)))
                {
					continue;
				}
                // At this point, we have an adapter/device/adapterformat/backbufferformat/iswindowed
                // DeviceCombo that is supported by the system.  We still need to confirm that it's
                // compatible with the app, and find one or more suitable depth/stencil buffer format,
                // multisample type, vertex processing type, and present interval.
                D3DDeviceCombo DeviceCombo;

				DeviceCombo.AdapterOrdinal = DeviceInfo.AdapterOrdinal;
				DeviceCombo.DevType = DeviceInfo.DevType;
				DeviceCombo.AdapterFormat = adapterFormat;
				DeviceCombo.BackBufferFormat = backBufferFormat;
				DeviceCombo.IsWindowed = isWindowed;
				if(AppUsesDepthBuffer)
				{
					BuildDepthStencilFormatList(DeviceCombo);
					if(DeviceCombo.DepthStencilFormatList.empty())
                    {
						continue;
					}
				}
                BuildMultiSampleTypeList(DeviceCombo);
				if(DeviceCombo.MultiSampleTypeList.empty())
                {
					continue;
				}
                BuildDSMSConflictList(DeviceCombo);
				BuildVertexProcessingTypeList(DeviceInfo, DeviceCombo);
				if(DeviceCombo.VertexProcessingTypeList.empty())
				{
					continue;
				}
				BuildPresentIntervalList(DeviceInfo, DeviceCombo);

				DeviceInfo.DeviceComboList.push_back(DeviceCombo);
			}
		}
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: BuildDepthStencilFormatList
// Desc: Adds all depth/stencil formats that are compatible with the device
//       and app to the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumeration::BuildDepthStencilFormatList(D3DDeviceCombo& DeviceCombo)
{
	const D3DFORMAT depthStencilFormatArray[] =
	{
		D3DFMT_D16,
		D3DFMT_D15S1,
		D3DFMT_D24X8,
		D3DFMT_D24S8,
		D3DFMT_D24X4S4,
		D3DFMT_D32,
	};
	const UINT depthStencilFormatArrayCount = sizeof(depthStencilFormatArray) / sizeof(depthStencilFormatArray[0]);

	for(UINT idsf = 0; idsf < depthStencilFormatArrayCount; ++idsf)
    {
		D3DFORMAT depthStencilFmt = depthStencilFormatArray[idsf];
		if(DepthBits(depthStencilFmt) < AppMinDepthBits)
		{
			continue;
		}
		if(StencilBits(depthStencilFmt) < AppMinStencilBits)
		{
			continue;
		}

		if(SUCCEEDED(m_pD3D->CheckDeviceFormat(DeviceCombo.AdapterOrdinal, DeviceCombo.DevType, DeviceCombo.AdapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, depthStencilFmt)))
		{
			if(SUCCEEDED(m_pD3D->CheckDepthStencilMatch(DeviceCombo.AdapterOrdinal, DeviceCombo.DevType, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat, depthStencilFmt)))
			{
				DeviceCombo.DepthStencilFormatList.push_back(depthStencilFmt);
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: BuildMultiSampleTypeList
// Desc: Adds all multisample types that are compatible with the device and app to
//       the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumeration::BuildMultiSampleTypeList(D3DDeviceCombo& DeviceCombo)
{
	const D3DMULTISAMPLE_TYPE msTypeArray[] = {
		D3DMULTISAMPLE_NONE,
		D3DMULTISAMPLE_NONMASKABLE,
		D3DMULTISAMPLE_2_SAMPLES,
		D3DMULTISAMPLE_3_SAMPLES,
		D3DMULTISAMPLE_4_SAMPLES,
		D3DMULTISAMPLE_5_SAMPLES,
		D3DMULTISAMPLE_6_SAMPLES,
		D3DMULTISAMPLE_7_SAMPLES,
		D3DMULTISAMPLE_8_SAMPLES,
		D3DMULTISAMPLE_9_SAMPLES,
		D3DMULTISAMPLE_10_SAMPLES,
		D3DMULTISAMPLE_11_SAMPLES,
		D3DMULTISAMPLE_12_SAMPLES,
		D3DMULTISAMPLE_13_SAMPLES,
		D3DMULTISAMPLE_14_SAMPLES,
		D3DMULTISAMPLE_15_SAMPLES,
		D3DMULTISAMPLE_16_SAMPLES,
	};
	const UINT msTypeArrayCount = sizeof(msTypeArray) / sizeof(msTypeArray[0]);

    for(UINT imst = 0; imst < msTypeArrayCount; ++imst)
    {
		D3DMULTISAMPLE_TYPE msType = msTypeArray[imst];
		DWORD msQuality = 0;

        if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(DeviceCombo.AdapterOrdinal, DeviceCombo.DevType,
			DeviceCombo.BackBufferFormat, DeviceCombo.IsWindowed, msType, &msQuality)))
		{
			DeviceCombo.MultiSampleTypeList.push_back(msType);
			DeviceCombo.MultiSampleQualityList.push_back(msQuality);
		}
	}
}




//-----------------------------------------------------------------------------
// Name: BuildDSMSConflictList
// Desc: Find any conflicts between the available depth/stencil formats and
//       multisample types.
//-----------------------------------------------------------------------------
void D3DEnumeration::BuildDSMSConflictList(D3DDeviceCombo& DeviceCombo)
{
	for(UINT ids = 0; ids < DeviceCombo.DepthStencilFormatList.size(); ++ids)
	{
		D3DFORMAT dsFmt = DeviceCombo.DepthStencilFormatList[ids];
		for(UINT ims = 0; ims < DeviceCombo.MultiSampleTypeList.size(); ++ims)
		{
			D3DMULTISAMPLE_TYPE msType = DeviceCombo.MultiSampleTypeList[ims];

			if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(DeviceCombo.AdapterOrdinal, DeviceCombo.DevType,
				DeviceCombo.BackBufferFormat, DeviceCombo.IsWindowed, msType, 0)))
			{
				if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(DeviceCombo.AdapterOrdinal, DeviceCombo.DevType,
					dsFmt, DeviceCombo.IsWindowed, msType, 0)))
				{
					continue;
				}
			}

			D3DDSMSConflict DSMSConflict;
			DSMSConflict.DSFormat = dsFmt;
			DSMSConflict.MSType = msType;
			DeviceCombo.DSMSConflictList.push_back(DSMSConflict);
		}
    }
}




//-----------------------------------------------------------------------------
// Name: BuildVertexProcessingTypeList
// Desc: Adds all vertex processing types that are compatible with the device
//       and app to the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumeration::BuildVertexProcessingTypeList(const D3DDeviceInfo& DeviceInfo, D3DDeviceCombo& DeviceCombo)
{
	if((DeviceInfo.Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
	{
		if((DeviceInfo.Caps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0)
		{
			if(ConfirmDeviceCallback == 0 || ConfirmDeviceCallback(&DeviceInfo.Caps, PURE_HARDWARE_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat))
			{
				DeviceCombo.VertexProcessingTypeList.push_back(PURE_HARDWARE_VP);
			}
        }
        if(ConfirmDeviceCallback == 0 || ConfirmDeviceCallback(&DeviceInfo.Caps, HARDWARE_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat))
		{
			DeviceCombo.VertexProcessingTypeList.push_back(HARDWARE_VP);
		}
		if(AppUsesMixedVP && (ConfirmDeviceCallback == 0 || ConfirmDeviceCallback(&DeviceInfo.Caps, MIXED_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat)))
		{
			DeviceCombo.VertexProcessingTypeList.push_back(MIXED_VP);
        }
    }
    if(ConfirmDeviceCallback == 0 || ConfirmDeviceCallback(&DeviceInfo.Caps, SOFTWARE_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat))
	{
		DeviceCombo.VertexProcessingTypeList.push_back(SOFTWARE_VP);
	}
}




//-----------------------------------------------------------------------------
// Name: BuildPresentIntervalList
// Desc: Adds all present intervals that are compatible with the device and app
//       to the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumeration::BuildPresentIntervalList(const D3DDeviceInfo& DeviceInfo, D3DDeviceCombo& DeviceCombo)
{
	const UINT piArray[] = {
		D3DPRESENT_INTERVAL_IMMEDIATE,
		D3DPRESENT_INTERVAL_DEFAULT,
		D3DPRESENT_INTERVAL_ONE,
		D3DPRESENT_INTERVAL_TWO,
		D3DPRESENT_INTERVAL_THREE,
		D3DPRESENT_INTERVAL_FOUR,
	};
	const UINT piArrayCount = sizeof(piArray) / sizeof(piArray[0]);

	for(UINT ipi = 0; ipi < piArrayCount; ++ipi)
	{
		UINT pi = piArray[ipi];

		if(DeviceCombo.IsWindowed)
		{
			if(pi == D3DPRESENT_INTERVAL_TWO ||
				pi == D3DPRESENT_INTERVAL_THREE ||
				pi == D3DPRESENT_INTERVAL_FOUR)
			{
				// These intervals are not supported in windowed mode.
				continue;
			}
		}
		// Note that D3DPRESENT_INTERVAL_DEFAULT is zero, so you
		// can't do a caps check for it -- it is always available.
		if(pi == D3DPRESENT_INTERVAL_DEFAULT ||
			(DeviceInfo.Caps.PresentationIntervals & pi))
		{
			DeviceCombo.PresentIntervalList.push_back(pi);
		}
	}
}

}  // namespace