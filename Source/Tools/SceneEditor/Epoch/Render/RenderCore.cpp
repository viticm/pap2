// File: RenderCore.cpp
// Desc:

#include"RenderCore.h"

#include<vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

using std::vector;

namespace Render
{

void RenderCore::InitRSRegistry()
{
	m_RenderStateRegistry[RSID_STREAM] = new RSStream;
	m_RenderStateRegistry[RSID_INDEXBUFFER] = new RSIndexBuffer;
	m_RenderStateRegistry[RSID_DRAWINDEXED] = new RSDrawIndexed;
}

void RenderCore::DestroyRSRegistry()
{
	for(unsigned int i = 0; i < RSID_NUMBER; ++i)
	{
		SafeDelete(m_RenderStateRegistry[i]);
	}
}

RenderCore::RenderCore(): D3DEnumeration(), m_pD3DDevice(0)
{
	memset(&m_RCDesc, 0, sizeof(m_RCDesc));
	memset(&m_DisplayMode, 0, sizeof(m_DisplayMode));

	for(unsigned int i = 0; i < RSID_NUMBER; ++i)
	{
		m_RenderStateRegistry[i] = 0;
	}
}

/*
static D3DDEV_TYPE DeviceType(unsigned int i)
{
	switch(i)
	{
	case 0:
		return D3DDEV_HAL;

	case 1:
		return D3DDEV_SW;

	case 2:
		return D3DDEV_REF;

	default:
		return D3DDEV_HAL;
	}
}
*/

static unsigned int FormatToColorBits(D3DFORMAT fmt)
{
	switch(fmt)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_R8G8B8:
		return 32;  // Well, user think they are using 32 bits color, but actually they aren't.

	case D3DFMT_R5G6B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
		return 16;

	default:
		return 0;
	}
}

static DWORD Behavior(VertexProcessingType vpt)
{
	switch(vpt)
	{
	case SOFTWARE_VP:
		return D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	case MIXED_VP:
		return D3DCREATE_MIXED_VERTEXPROCESSING;

	case HARDWARE_VP:
		return D3DCREATE_HARDWARE_VERTEXPROCESSING;

	case PURE_HARDWARE_VP:
		return D3DCREATE_PUREDEVICE;

	default:
		return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
}

bool RenderCore::GetBehavior(DWORD& dwBehaviors, VertexProcessingType vpt)
{
	unsigned int num = m_DeviceCombo.VertexProcessingTypeList.size();
	for(unsigned int i = 0; i < m_DeviceCombo.VertexProcessingTypeList.size(); ++i)
	{
		if(m_DeviceCombo.VertexProcessingTypeList[i] == vpt)
		{
			dwBehaviors = Behavior(vpt);

			return true;
		}
	}

	return false;
}

bool RenderCore::Init(const RenderCoreDesc& rcdesc)
{
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_pD3D)
	{
		// Push error-code into the error-pipeline
		return false;
	}

	m_RCDesc = rcdesc;

	AppMinStencilBits = m_RCDesc.HR_MinStencilBits;
	AppUsesDepthBuffer = true;
    AppUsesMixedVP = m_RCDesc.HR_UsesMixedVP;
	if(m_RCDesc.m_PresentParameters.Windowed)
	{
		AppRequiresWindowed = true;
	}
	else
	{
		AppRequiresFullscreen = true;
	}

	if(FAILED(Enumerate()))
	{
		// Push error-code into the error-pipeline
		return false;
	}
	else
	{
		if(m_AdapterInfoList.empty())
		{
			// Push error-code into the error-pipeline
			return false;
		}
	}

	unsigned int AdapterInfoID = 0xFFFFFFFF;
	unsigned int DeviceInfoID = 0xFFFFFFFF;
	unsigned int DeviceComboID = 0xFFFFFFFF;
	for(unsigned int iAdapter = 0; iAdapter < m_AdapterInfoList.size(); ++iAdapter)
	{
		D3DAdapterInfo AdapterInfo = m_AdapterInfoList[iAdapter];

		if(AdapterInfo.AdapterOrdinal == m_RCDesc.m_Adapter)
		{
			m_AdapterInfo = AdapterInfo;
			AdapterInfoID = iAdapter;

			if(FAILED(m_pD3D->GetAdapterDisplayMode(AdapterInfo.AdapterOrdinal, &m_DisplayMode)))
			{
				// Push error-code into error-pipeline
				return false;
			}

			for(unsigned int iDevice = 0; iDevice < m_AdapterInfo.DeviceInfoList.size(); ++iDevice)
			{
				D3DDeviceInfo DeviceInfo = m_AdapterInfo.DeviceInfoList[iDevice];

				if(DeviceInfo.DevType == m_RCDesc.m_DeviceType)
				{
					m_DeviceInfo = DeviceInfo;
					DeviceInfoID = iDevice;

					unsigned int NumOfDeviceCombos = m_DeviceInfo.DeviceComboList.size();

					for(unsigned int iDeviceCombo = 0; iDeviceCombo < m_DeviceInfo.DeviceComboList.size(); ++iDeviceCombo)
					{
						D3DDeviceCombo DeviceCombo = m_DeviceInfo.DeviceComboList[iDeviceCombo];

						if(m_RCDesc.m_PresentParameters.Windowed)  // 窗口模式
						{
							if(DeviceCombo.AdapterFormat == m_DisplayMode.Format)  // 此适配器的显示模式必须与桌面模式一致
							{
								if(DeviceCombo.IsWindowed == true)  // 此适配器必须支持桌面模式
								{
									m_RCDesc.m_DisplayColorBits = FormatToColorBits(DeviceCombo.BackBufferFormat);

									m_DeviceCombo = DeviceCombo;
									DeviceComboID = iDeviceCombo;
									break;
								}
							}
						}
						else  // 全屏幕，不要求适配器的显示模式与桌面模式一致
						{
							if(FormatToColorBits(DeviceCombo.BackBufferFormat) == m_RCDesc.m_DisplayColorBits && DeviceCombo.IsWindowed == false)
							{
								m_DeviceCombo = DeviceCombo;
								DeviceComboID = iDeviceCombo;

								break;
							}
						}
					}

					break;
				}
			}

			break;
		}
	}

	if(AdapterInfoID == 0xFFFFFFFF || DeviceInfoID == 0xFFFFFFFF || DeviceComboID == 0xFFFFFFFF)
	{
		// Push error-code into error-line
		return false;
	}


	if(m_RCDesc.HR_UsesMixedVP)
	{
		m_RCDesc.m_dwBehaviors = D3DCREATE_MIXED_VERTEXPROCESSING;
	}
	else
	{
		if(!GetBehavior(m_RCDesc.m_dwBehaviors, SOFTWARE_VP))
		{
			if(!GetBehavior(m_RCDesc.m_dwBehaviors, HARDWARE_VP))
			{
				if(!GetBehavior(m_RCDesc.m_dwBehaviors, PURE_HARDWARE_VP))
				{
					// Push error-code into error-pipeline
					return false;
				}
			}
		}
	}


	m_DisplayMode.Width = m_RCDesc.m_PresentParameters.BackBufferWidth;
	m_DisplayMode.Height = m_RCDesc.m_PresentParameters.BackBufferHeight;
	if(m_RCDesc.m_PresentParameters.Windowed)
	{
		D3DDISPLAYMODE DesktopMode;
		if(FAILED(m_pD3D->GetAdapterDisplayMode(m_AdapterInfo.AdapterOrdinal, &DesktopMode)))
		{
			// Push error-code into error-pipeline
			return false;
		}

		m_RCDesc.m_PresentParameters.BackBufferFormat = DesktopMode.Format;

		m_DisplayMode.Width = DesktopMode.Width;
		m_DisplayMode.Height = DesktopMode.Height;
		m_DisplayMode.Format = DesktopMode.Format;
	}
	else
	{
		m_RCDesc.m_PresentParameters.BackBufferFormat = m_DeviceCombo.BackBufferFormat;

		m_DisplayMode.Format = m_RCDesc.m_PresentParameters.BackBufferFormat;
	}

	m_RCDesc.m_PresentParameters.BackBufferCount = 0;

	vector<DSMSPair> AvailableDSMSPairs;
	for(unsigned int iDSF = 0; iDSF < m_DeviceCombo.DepthStencilFormatList.size(); ++iDSF)
	{
		D3DFORMAT dsf = m_DeviceCombo.DepthStencilFormatList[iDSF];

		for(unsigned int iMS = 0; iMS < m_DeviceCombo.MultiSampleTypeList.size(); ++iMS)
		{
			D3DMULTISAMPLE_TYPE ms = m_DeviceCombo.MultiSampleTypeList[iMS];

			bool bConflict = false;
			for(unsigned int iDSMSConflict = 0; iDSMSConflict < m_DeviceCombo.DSMSConflictList.size(); ++iDSMSConflict)
			{
				D3DDSMSConflict DSMSConflict = m_DeviceCombo.DSMSConflictList[iDSMSConflict];

				if(DSMSConflict.DSFormat == dsf && DSMSConflict.MSType == ms)
				{
					bConflict = true;
					break;
				}
			}
			if(!bConflict)
			{
				DSMSPair dsms = {dsf, iMS};
				AvailableDSMSPairs.push_back(dsms);
			}
		}
	}

	unsigned int itemp = AvailableDSMSPairs.size();
	if(AvailableDSMSPairs.empty())
	{
		// Push error-code into error-pipeline
		return false;
	}

	DSMSPair dsms = AvailableDSMSPairs[AvailableDSMSPairs.size() - 1];

	m_RCDesc.m_PresentParameters.MultiSampleType = m_DeviceCombo.MultiSampleTypeList[dsms.MSID];
	m_RCDesc.m_PresentParameters.MultiSampleQuality = m_DeviceCombo.MultiSampleQualityList[dsms.MSID] - 1;
	m_RCDesc.m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_RCDesc.m_PresentParameters.EnableAutoDepthStencil = true;
	m_RCDesc.m_PresentParameters.AutoDepthStencilFormat = dsms.DSFormat;
	m_RCDesc.m_PresentParameters.Flags = 0;
	if(m_RCDesc.m_PresentParameters.Windowed)
	{
		m_RCDesc.m_PresentParameters.Flags = D3DPRESENTFLAG_DEVICECLIP;
	}


	m_RCDesc.m_PresentParameters.FullScreen_RefreshRateInHz = 0;
	if(!m_RCDesc.m_PresentParameters.Windowed)
	{
		for(unsigned int iDM = 0; iDM < m_AdapterInfo.DisplayModeList.size(); ++iDM)
		{
			D3DDISPLAYMODE dm = m_AdapterInfo.DisplayModeList[iDM];
			if(dm.Width == m_DisplayMode.Width && dm.Height == m_DisplayMode.Height && dm.Format == m_DisplayMode.Format)
			{
				if(m_RCDesc.m_PresentParameters.FullScreen_RefreshRateInHz < dm.RefreshRate)
				{
					m_RCDesc.m_PresentParameters.FullScreen_RefreshRateInHz = dm.RefreshRate;
				}
			}

		}
	}
	m_RCDesc.m_PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;


	//m_RCDesc.m_PresentParameters.MultiSampleQuality = 0;
	//m_RCDesc.m_PresentParameters.SwapEffect = D3DSWAPEFFECT_COPY;

	HRESULT hr = m_pD3D->CreateDevice(m_AdapterInfo.AdapterOrdinal, m_DeviceInfo.DevType, m_RCDesc.m_hWnd, m_RCDesc.m_dwBehaviors, &m_RCDesc.m_PresentParameters, &m_pD3DDevice);
	if(FAILED(hr))
	{
		// Push error-code into the error-pipeline
		throw Common::Exception(Common::EL_BAD, "RenderCore::Initiate", "Failed to create D3DDevice!");

		return false;
	}

	// Initialize The RenderState registry
	InitRSRegistry();

	return true;
}

void RenderCore::Destroy()
{
	DestroyRSRegistry();

	SafeRelease(m_pD3DDevice);
	SafeRelease(m_pD3D);
}

IDirect3D9* RenderCore::GetD3D() const
{
	if(!m_pD3D)
	{
		// Push error-code into the error-pipeline
	}

	return m_pD3D;
}

IDirect3DDevice9* RenderCore::GetD3DDevice() const
{
	if(!m_pD3DDevice)
	{
		// Push error-code into the error-pipeline
		throw Common::Exception(Common::EL_BAD, "RenderCore::GetD3DDevice", "D3DDevice is not valid yet!");
	}

	return m_pD3DDevice;
}

IDirect3DVertexDeclaration9* RenderCore::CreateVertexDeclaration(D3DVERTEXELEMENT9* pVertexElements)
{
	IDirect3DVertexDeclaration9* ret = 0;
	if(FAILED(GetD3DDevice()->CreateVertexDeclaration(pVertexElements, &ret)))
	{
		// Push error-code into the error-pipeline
		return 0;
	}

	return ret;
}

IDirect3DVertexBuffer9* RenderCore::CreateVertexBuffer(unsigned int NumOfVertices, unsigned int SizeOfVertex, DWORD dwFVF, D3DPOOL Pool, DWORD Usage)
{
	IDirect3DVertexBuffer9* ret = 0;
	if(FAILED(GetD3DDevice()->CreateVertexBuffer(NumOfVertices * SizeOfVertex, Usage, dwFVF, Pool, &ret, 0)))
	{
		// Push error-code into the error-pipeline
		return 0;
	}

	return ret;
}

IDirect3DIndexBuffer9* RenderCore::CreateIndexBuffer(unsigned int NumOfIndices, D3DFORMAT Format, D3DPOOL Pool, DWORD Usage)
{
	IDirect3DIndexBuffer9* ret = 0;
	if(FAILED(GetD3DDevice()->CreateIndexBuffer(NumOfIndices * GetSizeOfIndex(Format), Usage, Format, Pool, &ret, 0)))
	{
		// Push error-code into the error-pipeline
		return 0;
	}

	return ret;
}

RenderState* RenderCore::GetCachedRenderState(unsigned int IndexOfRS) const
{
	if(IndexOfRS >= RSID_NUMBER)
	{
		// Push error-code into error-pipeline
		return 0;
	}

	return m_RenderStateRegistry[IndexOfRS];
}

void RenderCore::RenderCurrentStates()
{
	Clear();
	Begin();

	for(unsigned int i = 0; i < RSID_NUMBER; ++i)
	{
		m_RenderStateRegistry[i]->Apply(true);
	}

	RSDrawIndexed* pRSDI= static_cast<RSDrawIndexed*>(m_RenderStateRegistry[RSID_DRAWINDEXED]);
	if(pRSDI && pRSDI->IsValid())
	{
		DrawIndexedPrimitive(pRSDI->m_NumOfVertices, pRSDI->m_NumOfPrimitives, pRSDI->m_PrimitiveType);
	}

	End();
}

void RenderCore::Clear(D3DCOLOR Color, float Z, DWORD Stencil, DWORD Count, const D3DRECT *pRects)
{

	DWORD Flags = D3DCLEAR_TARGET;
	if(m_RCDesc.m_PresentParameters.EnableAutoDepthStencil)
	{
		Flags |= D3DCLEAR_ZBUFFER;
	}

	GetD3DDevice()->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

void RenderCore::SetWorldMatrix(const Maths::Matrix& mtx)
{
	D3DMATRIX dmtx;
	mtx.ToD3DMATRIX(dmtx);
	GetD3DDevice()->SetTransform(D3DTS_WORLD, &dmtx);
}

void RenderCore::SetViewMatrix(const Maths::Matrix& mtx)
{
	D3DMATRIX dmtx;
	mtx.ToD3DMATRIX(dmtx);
	GetD3DDevice()->SetTransform(D3DTS_VIEW, &dmtx);
}

void RenderCore::SetProjectionMatrix(const Maths::Matrix& mtx)
{
	D3DMATRIX dmtx;
	mtx.ToD3DMATRIX(dmtx);
	GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &dmtx);
}

void RenderCore::DrawIndexedPrimitive(unsigned int NumOfVertices, unsigned int NumOfPrimitives, D3DPRIMITIVETYPE PrimitiveType)
{
	PrimitiveType = (PrimitiveType == D3DPT_POINTLIST)? static_cast<RSDrawIndexed*>(m_RenderStateRegistry[RSID_DRAWINDEXED])->m_PrimitiveType: PrimitiveType;
	GetD3DDevice()->DrawIndexedPrimitive(PrimitiveType, 0, 0, NumOfVertices, 0, NumOfPrimitives);
}

void RenderCore::Present(HWND hWnd)
{
	HRESULT hr = GetD3DDevice()->Present(0, 0, hWnd, 0);
}

/*
D3DFORMAT RenderCore::GetAValidDepthStencilFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat)
{
	if(!m_pD3D)
	{
		return D3DFMT_D16;
	}

	static D3DFORMAT DepthStencilFormats[7] = {D3DFMT_D32, D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D24X8, D3DFMT_D24X4S4, D3DFMT_D24FS8, D3DFMT_D16};

	for(unsigned int i = 0; i < 7; ++i)
	{
		if(SUCCEEDED(m_pD3D->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormats[i])))
		{
			return DepthStencilFormats[i];
		}
	}

	return D3DFMT_D16;
}
*/

/*
void RenderCore::GetValidDisplayMode(RenderCoreDesc& rcdesc)
{
	D3DDISPLAYMODE DMs[MAXNUM_DM];
	unsigned int NumOfDMs = 0;
	memset((void*)DMs, 0, sizeof(D3DDISPLAYMODE) * MAXNUM_DM);

	D3DDISPLAYMODE DisplayMode;
	DisplayMode.Width = rcdesc.m_PresentParameters.BackBufferWidth;
	DisplayMode.Height = rcdesc.m_PresentParameters.BackBufferHeight;
	DisplayMode.Format = rcdesc.m_PresentParameters.BackBufferFormat;

	if(rcdesc.m_PresentParameters.Windowed)
	{
		m_pD3D->GetAdapterDisplayMode(rcdesc.m_Adapter, &DisplayMode);
	}

	NumOfDMs = m_pD3D->GetAdapterModeCount(rcdesc.m_Adapter, DisplayMode.Format);

	unsigned int i = 0;
	for(; i < NumOfDMs; ++i)
	{
		m_pD3D->EnumAdapterModes(rcdesc.m_Adapter, DisplayMode.Format, 0, DMs);
	}

	HRESULT hr;

	for(i = 0; i < NumOfDMs; ++i)
	{
		if(DMs[i].Width == DisplayMode.Width && DMs[i].Height == DisplayMode.Height)
		{
			if(hr = m_pD3D->CheckDeviceType(rcdesc.m_Adapter, rcdesc.m_DeviceType, DisplayMode.Format, rcdesc.m_PresentParameters.BackBufferFormat, rcdesc.m_PresentParameters.Windowed))
			{
				MessageBox(0, "", "Get One", MB_OK);
			}
		}
	}

	throw Common::Exception(Common::EL_BAD, "Just exit", "Just exit");

	m_NumOfDMs = m_pD3D->GetAdapterModeCount(Adapter, BackBufferFormat);

	for(unsigned int i = 0; i < m_NumOfDMs; ++i)
	{
		m_pD3D->EnumAdapterModes(Adapter, BackBufferFormat, i, DMs);
		for(unsigned int j = 0; i < m_NumOfDMs)
	}
}

*/

D3DDEVTYPE IntToDevType(unsigned int iVal)
{
	switch(iVal)
	{
	case 0:
		return D3DDEVTYPE_HAL;

	case 1:
		return D3DDEVTYPE_SW;

	case 2:
		return D3DDEVTYPE_REF;

	default:
		return D3DDEVTYPE_HAL;
	}
}

}  // namespace