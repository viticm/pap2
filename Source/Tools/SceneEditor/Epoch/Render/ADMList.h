// File: ADMList.h
// Desc: ADM: Adapter & Device & Mode

#ifndef ADMLIST_H
#define ADMLIST_H

#include"..\Common\String.h"

#include"RenderCommon.h"

#include<vector>
using std::vector;

namespace Render
{

struct DLLAPI ModeInfo
{
	// 分辨率
	DWORD Width;
	DWORD Height;
	
	D3DFORMAT Format;  // Pixel format
	UINT RefreshRate;
	
	DWORD dwBehavior;  // Hardware / software / mixed vertex processing
	D3DFORMAT DepthStencilFormat;
};

struct DLLAPI DeviceInfo
{
	D3DDEVTYPE DeviceType;  // HAL, Reference, etc
	D3DCAPS9 Caps;  // Capabilities of this device
	Common::String sDesc;  // name;

	// Modes of this device
	vector<ModeInfo> vtModes;
	DWORD dwCurrentMode;
	D3DMULTISAMPLE_TYPE MultiSampleType;
};

// Driver info
struct DLLAPI AdapterInfo
{
	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;

	vector<DeviceInfo> vtDevices;
	DWORD dwCurrentDevice;
};

struct DLLAPI GraphicsInfo
{
	vector<AdapterInfo> m_vtAdapters;

	DWORD m_dwWidth, m_dwHeight;  // 默认分辨率 for Menu display
	vector<D3DFORMAT> m_vtFormats;  // 想要的色深

	DWORD m_dwDepthBits;  // 默认DepthStencil缓冲格式为D3DFMT_D16 (无Stencil缓冲区)
	DWORD m_dwStencilBits;

	GraphicsInfo()
	{
		m_vtFormats.push_back(D3DFMT_A8R8G8B8);  // 32bits
		m_vtFormats.push_back(D3DFMT_R5G6B5);  // 16bits (565)
		m_vtFormats.push_back(D3DFMT_R8G8B8);  // 24bits
	}

	GraphicsInfo(DWORD width, DWORD height, DWORD db = 16, DWORD sb = 0)
	{
		m_dwWidth = width;
		m_dwHeight = height;
		m_vtFormats.push_back(D3DFMT_A8R8G8B8);  // 32bits
		m_vtFormats.push_back(D3DFMT_R5G6B5);  // 16bits (565)
		m_vtFormats.push_back(D3DFMT_R8G8B8);  // 24bits

		m_dwDepthBits = db;
		m_dwStencilBits = sb;
	}
};

DLLAPI HRESULT ConfirmDevice(D3DCAPS9* Caps, DWORD dwBehavior, D3DFORMAT Format);
DLLAPI void BuildList(IDirect3D9* pD3D, GraphicsInfo* pGI);

}  // namespace

#endif