// File: D3DSettings.cpp
// Desc:

#include "D3DSettings.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

int D3DSettings::GetAdapterOrdinal() const
{
	D3DDeviceCombo temp;
	GetDeviceCombo(temp);
	return temp.AdapterOrdinal;
}

D3DDEVTYPE D3DSettings::GetDeviceType() const
{
	D3DDeviceCombo temp;
	GetDeviceCombo(temp);
	return temp.DevType;
}

D3DFORMAT D3DSettings::GetBackBufferFormat() const
{
	D3DDeviceCombo temp;
	GetDeviceCombo(temp);
	return temp.BackBufferFormat;
}

void D3DSettings::SetDisplayMode(const D3DDISPLAYMODE& value)
{
	if(IsWindowed)
	{
		Windowed_DisplayMode = value;
	}
	else
	{
		Fullscreen_DisplayMode = value;
	}
}

void D3DSettings::SetDepthStencilBufferFormat(D3DFORMAT value)
{
	if(IsWindowed)
	{
		Windowed_DepthStencilBufferFormat = value;
	}
	else
	{
		Fullscreen_DepthStencilBufferFormat = value;
	}
}

void D3DSettings::SetMultisampleType(D3DMULTISAMPLE_TYPE value)
{
	if(IsWindowed)
	{
		Windowed_MultisampleType = value;
	}
	else
	{
		Fullscreen_MultisampleType = value;
	}
}

void D3DSettings::SetMultisampleQuality(DWORD value)
{
	if(IsWindowed)
	{
		Windowed_MultisampleQuality = value;
	}
	else
	{
		Fullscreen_MultisampleQuality = value;
	}
}

void D3DSettings::SetVertexProcessingType(VertexProcessingType value)
{
	if(IsWindowed)
	{
		Windowed_VertexProcessingType = value;
	}
	else
	{
		Fullscreen_VertexProcessingType = value;
	}
}

void D3DSettings::SetPresentInterval(UINT value)
{
	if(IsWindowed)
	{
		Windowed_PresentInterval = value;
	}
	else
	{
		Fullscreen_PresentInterval = value;
	}
}

}  // namespace