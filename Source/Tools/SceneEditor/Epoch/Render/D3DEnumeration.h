//-----------------------------------------------------------------------------
// File: D3DEnumeration.h
//
// Desc: Enumerates D3D adapters, devices, modes, etc.
//       Modified by Dolaham
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef D3DENUMERATION_H
#define D3DENUMERATION_H

#include"..\Common\Common.h"

#include<vector>
using std::vector;

#include<d3d9.h>

namespace Render
{

//-----------------------------------------------------------------------------
// Name: enum VertexProcessingType
// Desc: Enumeration of all possible D3D vertex processing types.
//-----------------------------------------------------------------------------
enum VertexProcessingType
{
    SOFTWARE_VP,
    MIXED_VP,
    HARDWARE_VP,
    PURE_HARDWARE_VP
};


struct DLLAPI D3DDeviceInfo;
//-----------------------------------------------------------------------------
// Name: struct D3DAdapterInfo
// Desc: Info about a display adapter.
//-----------------------------------------------------------------------------
struct DLLAPI D3DAdapterInfo
{
    int AdapterOrdinal;
    D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
    vector<D3DDISPLAYMODE> DisplayModeList; // List of D3DDISPLAYMODEs
    vector<D3DDeviceInfo> DeviceInfoList; // List of D3DDeviceInfo pointers
};


struct DLLAPI D3DDeviceCombo;
//-----------------------------------------------------------------------------
// Name: struct D3DDeviceInfo
// Desc: Info about a D3D device, including a list of D3DDeviceCombos (see below) 
//       that work with the device.
//-----------------------------------------------------------------------------
struct DLLAPI D3DDeviceInfo
{
    int AdapterOrdinal;
    D3DDEVTYPE DevType;
    D3DCAPS9 Caps;
    vector<D3DDeviceCombo> DeviceComboList; // List of D3DDeviceCombo pointers
};


//-----------------------------------------------------------------------------
// Name: struct D3DDSMSConflict
// Desc: A depth/stencil buffer format that is incompatible with a
//       multisample type.
//-----------------------------------------------------------------------------
struct DLLAPI D3DDSMSConflict
{
    D3DFORMAT DSFormat;
    D3DMULTISAMPLE_TYPE MSType;
};


//-----------------------------------------------------------------------------
// Name: struct D3DDeviceCombo
// Desc: A combination of adapter format, back buffer format, and windowed/fullscreen 
//       that is compatible with a particular D3D device (and the app).
//-----------------------------------------------------------------------------
struct DLLAPI D3DDeviceCombo
{
    int AdapterOrdinal;
    D3DDEVTYPE DevType;
    D3DFORMAT AdapterFormat;
    D3DFORMAT BackBufferFormat;
    bool IsWindowed;
    vector<D3DFORMAT> DepthStencilFormatList; // List of D3DFORMATs
    vector<D3DMULTISAMPLE_TYPE> MultiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
    vector<DWORD> MultiSampleQualityList; // List of DWORDs (number of quality 
                                         // levels for each multisample type)
    vector<D3DDSMSConflict> DSMSConflictList; // List of D3DDSMSConflicts
    vector<VertexProcessingType> VertexProcessingTypeList; // List of VertexProcessingTypes
    vector<UINT> PresentIntervalList; // List of D3DPRESENT_INTERVALs
};


typedef bool(* CONFIRMDEVICECALLBACK)(const D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat);


//-----------------------------------------------------------------------------
// Name: class CD3DEnumeration
// Desc: Enumerates available D3D adapters, devices, modes, etc.
//-----------------------------------------------------------------------------
class DLLAPI D3DEnumeration
{
protected:
    IDirect3D9* m_pD3D;

public:
	vector<D3DAdapterInfo> m_AdapterInfoList;
    // The following variables can be used to limit what modes, formats, 
    // etc. are enumerated.  Set them to the values you want before calling
    // Enumerate().
    CONFIRMDEVICECALLBACK ConfirmDeviceCallback;
    UINT AppMinFullscreenWidth;
    UINT AppMinFullscreenHeight;
    UINT AppMinColorChannelBits; // min color bits per channel in adapter format
    UINT AppMinAlphaChannelBits; // min alpha bits per pixel in back buffer format
    UINT AppMinDepthBits;
    UINT AppMinStencilBits;
    bool AppUsesDepthBuffer;
    bool AppUsesMixedVP; // whether app can take advantage of mixed vp mode
    bool AppRequiresWindowed;
    bool AppRequiresFullscreen;
    vector<D3DFORMAT> m_AllowedAdapterFormatList; // list of D3DFORMATs

public:
	D3DEnumeration();
    virtual ~D3DEnumeration() {}

protected:
    HRESULT Enumerate();

private:
    HRESULT EnumerateDevices(D3DAdapterInfo& AdapterInfo, const vector<D3DFORMAT>& AdapterFormatList);
    HRESULT EnumerateDeviceCombos(D3DDeviceInfo& DeviceInfo, const vector<D3DFORMAT>& AdapterFormatList);
    void BuildDepthStencilFormatList(D3DDeviceCombo& DeviceCombo);
    void BuildMultiSampleTypeList(D3DDeviceCombo& DeviceCombo);
    void BuildDSMSConflictList(D3DDeviceCombo& DeviceCombo);
    void BuildVertexProcessingTypeList(const D3DDeviceInfo& DeviceInfo, D3DDeviceCombo& DeviceCombo);
    void BuildPresentIntervalList(const D3DDeviceInfo& DeviceInfo, D3DDeviceCombo& DeviceCombo);
};

}  // namespace

#endif