//-----------------------------------------------------------------------------
// File: D3DSettings.h
//
// Desc: Settings class and change-settings dialog class for the Direct3D 
//       samples framework library.
//       Modified by Dolaham
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DSETTINGS_H
#define D3DSETTINGS_H

#include"D3DEnumeration.h"

namespace Render
{

//-----------------------------------------------------------------------------
// Name: class CD3DSettings
// Desc: Current D3D settings: adapter, device, mode, formats, etc.
//-----------------------------------------------------------------------------
class D3DSettings
{
protected:
    bool IsWindowed;

    D3DAdapterInfo Windowed_AdapterInfo;
    D3DDeviceInfo Windowed_DeviceInfo;
    D3DDeviceCombo Windowed_DeviceCombo;

    D3DDISPLAYMODE Windowed_DisplayMode; // not changable by the user
    D3DFORMAT Windowed_DepthStencilBufferFormat;
    D3DMULTISAMPLE_TYPE Windowed_MultisampleType;
    DWORD Windowed_MultisampleQuality;
    VertexProcessingType Windowed_VertexProcessingType;
    UINT Windowed_PresentInterval;
    int Windowed_Width;
    int Windowed_Height;

    D3DAdapterInfo Fullscreen_AdapterInfo;
    D3DDeviceInfo Fullscreen_DeviceInfo;
    D3DDeviceCombo Fullscreen_DeviceCombo;

    D3DDISPLAYMODE Fullscreen_DisplayMode; // changable by the user
    D3DFORMAT Fullscreen_DepthStencilBufferFormat;
    D3DMULTISAMPLE_TYPE Fullscreen_MultisampleType;
    DWORD Fullscreen_MultisampleQuality;
    VertexProcessingType Fullscreen_VertexProcessingType;
    UINT Fullscreen_PresentInterval;

public:
	void GetAdapterInfo(D3DAdapterInfo& AdapterInfo) const;
    void GetDeviceInfo(D3DDeviceInfo& DeviceInfo) const;
    void GetDeviceCombo(D3DDeviceCombo& DeviceCombo) const;

    int GetAdapterOrdinal() const;
    D3DDEVTYPE GetDeviceType() const;
    D3DFORMAT GetBackBufferFormat() const;

    void GetDisplayMode(D3DDISPLAYMODE& DisplayMode) const { DisplayMode = IsWindowed? Windowed_DisplayMode: Fullscreen_DisplayMode; }
    void SetDisplayMode(const D3DDISPLAYMODE& value);

    D3DFORMAT GetDepthStencilBufferFormat() const { return IsWindowed? Windowed_DepthStencilBufferFormat : Fullscreen_DepthStencilBufferFormat; }
    void SetDepthStencilBufferFormat(D3DFORMAT value);

    D3DMULTISAMPLE_TYPE GetMultisampleType() const { return IsWindowed? Windowed_MultisampleType : Fullscreen_MultisampleType; }
    void SetMultisampleType(D3DMULTISAMPLE_TYPE value);

    DWORD GetMultisampleQuality() const { return IsWindowed? Windowed_MultisampleQuality : Fullscreen_MultisampleQuality; }
    void SetMultisampleQuality(DWORD value);

    VertexProcessingType GetVertexProcessingType() const { return IsWindowed ? Windowed_VertexProcessingType : Fullscreen_VertexProcessingType; }
    void SetVertexProcessingType(VertexProcessingType value);

    UINT GetPresentInterval() const { return IsWindowed? Windowed_PresentInterval : Fullscreen_PresentInterval; }
    void SetPresentInterval(UINT value);
};

}  // namespace

#endif