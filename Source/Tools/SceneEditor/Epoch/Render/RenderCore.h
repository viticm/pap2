// File: RenderCore.h
// Desc:

#ifndef RENDERCORE_H
#define RENDERCORE_H

#include"..\Common\Common.h"
#include"..\Maths\Maths.h"
#include"RenderCommon.h"
#include"RenderState.h"
#include"Renderable.h"
#include"D3DEnumeration.h"

namespace Render
{

struct DLLAPI RenderCoreDesc
{
	HWND m_hWnd;  // The render window handle; set by client
	unsigned int m_Adapter;  // set by client
	D3DDEVTYPE m_DeviceType;  // 0: D3DDEV_HAL; 1: D3DDEV_SW; 2: D3DDEV_REF; set by client
	unsigned int m_DisplayColorBits;  // É«Éî; set by client
	DWORD m_dwBehaviors;
	D3DPRESENT_PARAMETERS m_PresentParameters;

	// Hardware requirements set in Config.ini
	bool HR_UsesMixedVP;  // Set by client
	unsigned int HR_MinStencilBits;  // Set by client
};

struct DLLAPI DSMSPair
{
	D3DFORMAT DSFormat;
	unsigned int MSID;
};

class DLLAPI RenderCore: public D3DEnumeration
{
public:
	enum
	{
		MAXNUM_DM = 100
	};

public:
	IDirect3DDevice9* m_pD3DDevice;
	
	RenderCoreDesc m_RCDesc;

	D3DAdapterInfo m_AdapterInfo;
	D3DDeviceInfo m_DeviceInfo;
	D3DDeviceCombo m_DeviceCombo;

	D3DDISPLAYMODE m_DisplayMode;

	RenderState* m_RenderStateRegistry[RSID_NUMBER];

private:
	RenderCore(const RenderCore& r) {}
	RenderCore& operator = (const RenderCore& r) { return *this; }

	void InitRSRegistry();
	void DestroyRSRegistry();

public:
	RenderCore();
	~RenderCore() { Destroy(); }

	bool Init(const RenderCoreDesc& rcdesc);
	void Destroy();

	IDirect3D9* GetD3D() const;
	IDirect3DDevice9* GetD3DDevice() const;
	void SetD3DDevice(IDirect3DDevice9* pD3DDevice) { m_pD3DDevice = pD3DDevice; }

	IDirect3DVertexDeclaration9* CreateVertexDeclaration(D3DVERTEXELEMENT9* pVertexElements);
	IDirect3DVertexBuffer9* CreateVertexBuffer(unsigned int NumOfVertices, unsigned int SizeOfVertex, DWORD dwFVF, D3DPOOL Pool = D3DPOOL_MANAGED, DWORD Usage = 0);
	IDirect3DIndexBuffer9* CreateIndexBuffer(unsigned int NumOfIndices, D3DFORMAT Format = D3DFMT_INDEX16, D3DPOOL Pool = D3DPOOL_MANAGED, DWORD Usage = 0);

	RenderState* GetCachedRenderState(unsigned int IndexOfRS) const;
	void RenderCurrentStates();  // This function is only used for drawing a single object cause there's only one DrawIndexedPrimitive() called. It's not very useful. Use primitive operations as alternative.

	
	void Clear(D3DCOLOR Color = D3DCOLOR_XRGB(0, 0, 0), float Z = 1.f, DWORD Stencil = 0, DWORD Count = 0, const D3DRECT *pRects = 0);
	void Begin() { GetD3DDevice()->BeginScene(); }

	void SetWorldMatrix(const Maths::Matrix& mtx);
	void SetViewMatrix(const Maths::Matrix& mtx);
	void SetProjectionMatrix(const Maths::Matrix& mtx);

	void SetVertexDeclaration(IDirect3DVertexDeclaration9* pVertexDeclaration) { GetD3DDevice()->SetVertexDeclaration(pVertexDeclaration); }
	void SetVertexStream(unsigned int StreamNumber, IDirect3DVertexBuffer9* pVertexBuffer, unsigned int SizeOfVertex) { GetD3DDevice()->SetStreamSource(StreamNumber, pVertexBuffer, 0, SizeOfVertex); }
	void SetIndexBuffer(LPDIRECT3DINDEXBUFFER9 pIndexBuffer) { GetD3DDevice()->SetIndices(pIndexBuffer); }
	void SetTexture(unsigned int IndexOfStage, IDirect3DTexture9* pTexture) { GetD3DDevice()->SetTexture(IndexOfStage, pTexture); }
	void SetMaterial(D3DMATERIAL9* pMaterial) { GetD3DDevice()->SetMaterial(pMaterial); }
	
	void DrawIndexedPrimitive(unsigned int NumOfVertices, unsigned int NumOfPrimitives, D3DPRIMITIVETYPE PrimitiveType = D3DPT_POINTLIST);

	void End() { GetD3DDevice()->EndScene(); }
	void Present(HWND hWnd = 0);
	void Reset() { GetD3DDevice()->Reset(&m_RCDesc.m_PresentParameters); }

private:
	bool GetBehavior(DWORD& dwBehaviors, VertexProcessingType vpt);
};

DLLAPI D3DDEVTYPE IntToDevType(unsigned int iVal);

}  // namespace

#endif