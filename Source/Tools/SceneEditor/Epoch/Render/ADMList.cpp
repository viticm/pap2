// File: ADMList.cpp
// Desc:

#include"ADMList.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

int SortModesCallback(const VOID* arg1, const VOID* arg2)
{
	D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
	D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

	if(p1->Format > p2->Format) return -1;
	if(p1->Format < p2->Format) return +1;
	if(p1->Width < p2->Width) return -1;
	if(p1->Width > p2->Width) return +1;
	if(p1->Height < p2->Height) return -1;
	if(p1->Height > p2->Height) return +1;

	return 0;
}

bool FindDepthStencilFormat(IDirect3D9* pD3D, DWORD dwDepthBits,
							DWORD dwStencilBits, int iAdapter,
							D3DDEVTYPE DeviceType,
							D3DFORMAT TargetFormat,  // Display Mode Format
							D3DFORMAT* pDepthStencilFormat)
{
	if(!pD3D)
	{
		return false;
	}

	if(dwDepthBits == 16 && dwStencilBits == 0)  // D3DFMT_D16
	{
		if(SUCCEEDED(pD3D->CheckDeviceFormat(iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			D3DFMT_D16)))
		{
			if(SUCCEEDED(pD3D->CheckDepthStencilMatch(iAdapter,
				DeviceType, TargetFormat, TargetFormat, D3DFMT_D16)))
			{
				*pDepthStencilFormat = D3DFMT_D16;

				return true;
			}
		}
	}
	else if(dwDepthBits == 15 && dwStencilBits == 1)  // D3DFMT_D15S1
	{
		if(SUCCEEDED(pD3D->CheckDeviceFormat(iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			D3DFMT_D15S1)))
		{
			if(SUCCEEDED(pD3D->CheckDepthStencilMatch(iAdapter,
				DeviceType, TargetFormat, TargetFormat, D3DFMT_D15S1)))
			{
				*pDepthStencilFormat = D3DFMT_D15S1;

				return true;
			}
		}
	}
	else if(dwDepthBits == 24 && dwStencilBits == 0)  // D3DFMT_D24X8
	{
		if(SUCCEEDED(pD3D->CheckDeviceFormat(iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			D3DFMT_D24X8)))
		{
			if(SUCCEEDED(pD3D->CheckDepthStencilMatch(iAdapter,
				DeviceType, TargetFormat, TargetFormat, D3DFMT_D24X8)))
			{
				*pDepthStencilFormat = D3DFMT_D24X8;

				return true;
			}
		}
	}
	else if(dwDepthBits == 24 && dwStencilBits == 8)  // D3DFMT_D24S8
	{
		if(SUCCEEDED(pD3D->CheckDeviceFormat(iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			D3DFMT_D24S8)))
		{
			if(SUCCEEDED(pD3D->CheckDepthStencilMatch(iAdapter,
				DeviceType, TargetFormat, TargetFormat, D3DFMT_D24S8)))
			{
				*pDepthStencilFormat = D3DFMT_D24S8;

				return true;
			}
		}
	}
	else if(dwDepthBits == 24 && dwStencilBits == 4)  // D3DFMT_D24X4S4
	{
		if(SUCCEEDED(pD3D->CheckDeviceFormat(iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			D3DFMT_D24X4S4)))
		{
			if(SUCCEEDED(pD3D->CheckDepthStencilMatch(iAdapter,
				DeviceType, TargetFormat, TargetFormat, D3DFMT_D24X4S4)))
			{
				*pDepthStencilFormat = D3DFMT_D24X4S4;

				return true;
			}
		}
	}
	else if(dwDepthBits == 32 && dwStencilBits == 0)  // D3DFMT_D32
	{
		if(SUCCEEDED(pD3D->CheckDeviceFormat(iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			D3DFMT_D32)))
		{
			if(SUCCEEDED(pD3D->CheckDepthStencilMatch(iAdapter,
				DeviceType, TargetFormat, TargetFormat, D3DFMT_D32)))
			{
				*pDepthStencilFormat = D3DFMT_D32;

				return true;
			}
		}
	}

	return false;
}

void BuildList(IDirect3D9* pD3D, GraphicsInfo* pGI)
{
	if(!pD3D)
	{
		return;
	}

	const DWORD dwNumDeviceTypes = 1;
	const char* szDeviceDescs[] = {"HAL"};
	const D3DDEVTYPE DeviceTypes[] = {D3DDEVTYPE_HAL};

	const int MAXNUMDISPLAYMODES = 100;
	const int MAXNUMFORMATS = 30;

	bool bHalExists = false;
	bool bHalIsSampleCompatible = false;

	int iNumAdapters = pD3D->GetAdapterCount();
	if(iNumAdapters <= 0)
	{
		return;
	}

	int iAdapter = 0;
	for(; iAdapter < iNumAdapters; ++iAdapter)
	{
		AdapterInfo ai;
		pD3D->GetAdapterIdentifier(iAdapter, 0, &ai.AdapterIdentifier);
		ai.dwCurrentDevice = 0;

		D3DDISPLAYMODE DisplayModes[MAXNUMDISPLAYMODES];  // of this adpater
		D3DFORMAT Formats[MAXNUMFORMATS];  // of this adpater
		DWORD dwNumDisplayModes = 0;  // of this adpater
		DWORD dwNumFormats = 0;  // of this adpater

		DWORD dwNumAdapterModes = pD3D->GetAdapterModeCount(iAdapter, 0);
		DWORD dwAdapterMode = 0;
		for(; dwAdapterMode < dwNumAdapterModes; ++dwAdapterMode)  // 填充显示模式和格式的数组
		{
			D3DDISPLAYMODE dm;
			pD3D->EnumAdapterModes(iAdapter, dwAdapterMode, &dm);

			// 只要分辨率大于等于640*480的16bits高彩(565)、24bits真彩、32bits真彩
			if(dm.Width < 640 || dm.Height < 480 || (dm.Format != D3DFMT_R5G6B5 && dm.Format != D3DFMT_R8G8B8 && dm.Format != D3DFMT_A8R8G8B8))  // && dm.Format != D3DFMT_X1R5G5B5 && dm.Format != FMT_A1R5G5B5 && dm.Format != D3DFMT_R8G8B8 && dm.Format != D3DFMT_A8R8G8B8D3D)
			{
				continue;
			}

			DWORD m = 0;
			for(; m < dwNumDisplayModes; ++m)
			{
				if(DisplayModes[m].Width == dm.Width && DisplayModes[m].Height == dm.Height && DisplayModes[m].Format == dm.Format)
				{
					if(DisplayModes[m].RefreshRate < dm.RefreshRate)
					{
						DisplayModes[m].RefreshRate = dm.RefreshRate;
					}

					break;
				}
			}

			// 找到了一个新的显示模式
			if(m == dwNumDisplayModes)
			{
				DisplayModes[m].Width = dm.Width;
				DisplayModes[m].Height = dm.Height;
				DisplayModes[m].Format = dm.Format;
				DisplayModes[m].RefreshRate = dm.RefreshRate;
				++dwNumDisplayModes;

				DWORD f = 0;
				for(; f < dwNumFormats; ++f)
				{
					if(Formats[f] == dm.Format)
					{
						break;
					}
				}

				// 找到了一个新的格式
				if(f == dwNumFormats)
				{
					Formats[dwNumFormats++] = dm.Format;
				}
			}
		}

		qsort(DisplayModes, dwNumDisplayModes, sizeof(D3DDISPLAYMODE), SortModesCallback);

		// We have already get all display formats wanted
		int iDevice = 0;
		for(; iDevice < dwNumDeviceTypes; ++iDevice)
		{
			DeviceInfo di;
			di.DeviceType = DeviceTypes[iDevice];
			pD3D->GetDeviceCaps(iAdapter, DeviceTypes[iDevice], &di.Caps);
			di.sDesc = string(szDeviceDescs[iDevice]);
			di.dwCurrentMode = 0;
			di.MultiSampleType = D3DMULTISAMPLE_NONE;  // Here is not correct!

			bool bFormatConfirmed[MAXNUMFORMATS];
			DWORD dwBehaviors[MAXNUMFORMATS];
			D3DFORMAT DepthStencilFormats[MAXNUMFORMATS];

			DWORD f = 0;
			for(; f < dwNumFormats; ++f)  // 检查格式是否合适并填充DepthStencilFormats数组
			{
				bFormatConfirmed[f] = false;
				DepthStencilFormats[f] = D3DFMT_UNKNOWN;

				if(FAILED(pD3D->CheckDeviceType(iAdapter, di.DeviceType, Formats[f], Formats[f], NULL)))
				{
					continue;
				}

				if(di.DeviceType == D3DDEVTYPE_HAL)
				{
					bHalExists = true;
				}

				if(di.Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
				{
					if(di.Caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
					{
						dwBehaviors[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;

						if(SUCCEEDED(ConfirmDevice(&di.Caps, dwBehaviors[f], Formats[f])))
						{
							bFormatConfirmed[f] = true;
						}
					}

					if(!bFormatConfirmed[f])
					{
						dwBehaviors[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

						if(SUCCEEDED(ConfirmDevice(&di.Caps, dwBehaviors[f], Formats[f])))
						{
							bFormatConfirmed[f] = true;
						}
					}

					if(!bFormatConfirmed[f])
					{
						dwBehaviors[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

						if(SUCCEEDED(ConfirmDevice(&di.Caps, dwBehaviors[f], Formats[f])))
						{
							bFormatConfirmed[f] = true;
						}
					}
				}

				if(!bFormatConfirmed[f])
				{
					dwBehaviors[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

					if(SUCCEEDED(ConfirmDevice(&di.Caps, dwBehaviors[f], Formats[f])))
					{
						bFormatConfirmed[f] = true;
					}
				}

				if(bFormatConfirmed[f])
				{
					if(!FindDepthStencilFormat(pD3D, pGI->m_dwDepthBits, pGI->m_dwStencilBits, iAdapter, di.DeviceType, Formats[f], &DepthStencilFormats[f]))
					{
						bFormatConfirmed[f] = false;
					}
				}
			}

			// 检查是否找到了合适的格式
			for(f = 0; f < dwNumFormats; ++f)
			{
				if(bFormatConfirmed[f])
				{
					break;
				}
			}
			if(f == dwNumFormats)
			{
				throw GameError("[BuildList] No valid format detected!");
			}

			DWORD d = 0;
			for(; d < dwNumDisplayModes; ++d)
			{
				DWORD f = 0;
				for(; f < dwNumFormats; ++f)
				{
					if(DisplayModes[d].Format == Formats[f])
					{
						if(bFormatConfirmed[f])
						{
							ModeInfo mi;

							mi.Width = DisplayModes[d].Width;
							mi.Height = DisplayModes[d].Height;
							mi.Format = DisplayModes[d].Format;
							mi.RefreshRate = DisplayModes[d].RefreshRate;
							mi.dwBehavior = dwBehaviors[f];
							mi.DepthStencilFormat = DepthStencilFormats[f];

							di.vtModes.push_back(mi);

							if(di.DeviceType == D3DDEVTYPE_HAL)
							{
								bHalIsSampleCompatible = true;
							}
						}
					}
				}
			}

			bool bSizeValid = false;
			for(d = 0; d < di.vtModes.size(); ++d)
			{
				if(di.vtModes[d].Width >= pGI->m_dwWidth && di.vtModes[d].Height >= pGI->m_dwHeight)
				{
					di.dwCurrentMode = d;
					bSizeValid = true;

					if(di.vtModes[d].Format == D3DFMT_R5G6B5)
					{
						break;
					}
				}
			}

			if(!bSizeValid)
			{
				throw GameError("[BuildList] Default display mode for menu invalid!");
			}

			if(di.vtModes.size() > 0)
			{
				ai.vtDevices.push_back(di);
			}
		}

		if(ai.vtDevices.size() > 0)
		{
			pGI->m_vtAdapters.push_back(ai);
		}
	}

	if(pGI->m_vtAdapters.size() <= 0)
	{
		throw GameError("[BuildList] No compatible devices!");
	}
}

}  // namespace