#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"

class KDebugDirect3DDevice9 : public IDirect3DDevice9, public IKG3DDebugInfo
{
public:
	KDebugDirect3DDevice9()
		:	m_pDevice(NULL), 
			m_bEnable(false),
			m_nTotalTextures(0),
			m_nTotalIndexBuffers(0),
			m_nTotalVertexBuffers(0),
			m_nTotalStateChanges(0),
			m_nTotalTextureChanges(0)
	{}

	void SetDevice(LPDIRECT3DDEVICE9 pDevice)
	{ m_pDevice = pDevice; }

	virtual void Enable(BOOL bEnable)
	{ m_bEnable = bEnable; }

	virtual BOOL IsEnable() const
	{ return m_bEnable; }

	virtual int GetTotalStateChanges() const
	{ return m_nTotalStateChanges; }

	virtual void ResetTotalStateChanges()
	{ m_nTotalStateChanges = 0; }

	//---------------------------------------------------------------------------------------------------------------------
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj)
	{ return m_pDevice->QueryInterface(riid, ppvObj); }

	STDMETHOD_(ULONG,AddRef)(THIS)
	{ return m_pDevice->AddRef(); }

	STDMETHOD_(ULONG,Release)(THIS)
	{ return m_pDevice->Release(); }

	STDMETHOD(TestCooperativeLevel)(THIS)
	{ return m_pDevice->TestCooperativeLevel(); }

	STDMETHOD_(UINT, GetAvailableTextureMem)(THIS)
	{ return m_pDevice->GetAvailableTextureMem(); }

	STDMETHOD(EvictManagedResources)(THIS)
	{ return m_pDevice->EvictManagedResources(); }

	STDMETHOD(GetDirect3D)(THIS_ IDirect3D9** ppD3D9)
	{ return m_pDevice->GetDirect3D(ppD3D9); }

	STDMETHOD(GetDeviceCaps)(THIS_ D3DCAPS9* pCaps)
	{ return m_pDevice->GetDeviceCaps(pCaps); }

	STDMETHOD(GetDisplayMode)(THIS_ UINT iSwapChain,D3DDISPLAYMODE* pMode)
	{ return m_pDevice->GetDisplayMode(iSwapChain, pMode); }

	STDMETHOD(GetCreationParameters)(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters)
	{ return m_pDevice->GetCreationParameters(pParameters); }

	STDMETHOD(SetCursorProperties)(THIS_ UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap) 
	{ return m_pDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap); }

	STDMETHOD_(void, SetCursorPosition)(THIS_ int X,int Y,DWORD Flags) 
	{ return m_pDevice->SetCursorPosition(X, Y, Flags); }

	STDMETHOD_(BOOL, ShowCursor)(THIS_ BOOL bShow) 
	{ return m_pDevice->ShowCursor(bShow); }

	STDMETHOD(CreateAdditionalSwapChain)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain) 
	{ return m_pDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain); }

	STDMETHOD(GetSwapChain)(THIS_ UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) 
	{ return m_pDevice->GetSwapChain(iSwapChain, pSwapChain); }

	STDMETHOD_(UINT, GetNumberOfSwapChains)(THIS) 
	{ return m_pDevice->GetNumberOfSwapChains(); }

	STDMETHOD(Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) 
	{ return m_pDevice->Reset(pPresentationParameters); }

	STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) 
	{ return m_pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion); }

	STDMETHOD(GetBackBuffer)(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) 
	{ return m_pDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer); }

	STDMETHOD(GetRasterStatus)(THIS_ UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) 
	{ return m_pDevice->GetRasterStatus(iSwapChain, pRasterStatus); }

	STDMETHOD(SetDialogBoxMode)(THIS_ BOOL bEnableDialogs) 
	{ return m_pDevice->SetDialogBoxMode(bEnableDialogs); }

	STDMETHOD_(void, SetGammaRamp)(THIS_ UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp) 
	{ return m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp); }

	STDMETHOD_(void, GetGammaRamp)(THIS_ UINT iSwapChain,D3DGAMMARAMP* pRamp) 
	{ return m_pDevice->GetGammaRamp(iSwapChain, pRamp); }

	STDMETHOD(CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle) 
	{ 
		++ m_nTotalTextures;
		return m_pDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle); 
	}

	STDMETHOD(CreateVolumeTexture)(THIS_ UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) 
	{ return m_pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle); }

	STDMETHOD(CreateCubeTexture)(THIS_ UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) 
	{ return m_pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle); }

	STDMETHOD(CreateVertexBuffer)(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle) 
	{ 
        if (!(Usage & D3DUSAGE_WRITEONLY) && (Pool & D3DPOOL_DEFAULT))
            KGLogPrintf(KGLOG_ERR, "CreateVertexBuffer() D3DPOOL_DEFAULT without D3DUSAGE_WRITEONLY\n");
		++ m_nTotalVertexBuffers;
		m_nTotalVBUsedMems += ::D3DXGetFVFVertexSize( FVF ) * Length;
		return m_pDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer,  pSharedHandle); 
	}

	STDMETHOD(CreateIndexBuffer)(THIS_ UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) 
	{ 
        if (!(Usage & D3DUSAGE_WRITEONLY) && (Pool & D3DPOOL_DEFAULT))
            KGLogPrintf(KGLOG_ERR, "CreateIndexBuffer() D3DPOOL_DEFAULT without D3DUSAGE_WRITEONLY\n");
		++ m_nTotalIndexBuffers;
		m_nTotalIBUsedMems +=  ( ( Format == D3DFMT_INDEX16 ) ? 2 : 4 ) * Length;
		return m_pDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle); 
	}

	STDMETHOD(CreateRenderTarget)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) 
	{ 
		++ m_nTotalRenderTextures;
		return m_pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle); 
	}

	STDMETHOD(CreateDepthStencilSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) 
	{ 
		++ m_nTotalDepthStencilTextures;
		return m_pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle); 
	}

	STDMETHOD(UpdateSurface)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) 
	{ return m_pDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint); }

	STDMETHOD(UpdateTexture)(THIS_ IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) 
	{ return m_pDevice->UpdateTexture(pSourceTexture, pDestinationTexture); }

	STDMETHOD(GetRenderTargetData)(THIS_ IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface) 
	{ return m_pDevice->GetRenderTargetData(pRenderTarget, pDestSurface); }

	STDMETHOD(GetFrontBufferData)(THIS_ UINT iSwapChain,IDirect3DSurface9* pDestSurface) 
	{ return m_pDevice->GetFrontBufferData(iSwapChain, pDestSurface); }

	STDMETHOD(StretchRect)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter) 
	{ return m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter); }

	STDMETHOD(ColorFill)(THIS_ IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color) 
	{ return m_pDevice->ColorFill(pSurface, pRect, color); }

	STDMETHOD(CreateOffscreenPlainSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) 
	{ return m_pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle); }

	STDMETHOD(SetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget) 
	{ return m_pDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget); }

	STDMETHOD(GetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget) 
	{ return m_pDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget); }

	STDMETHOD(SetDepthStencilSurface)(THIS_ IDirect3DSurface9* pNewZStencil) 
	{ return m_pDevice->SetDepthStencilSurface(pNewZStencil); }

	STDMETHOD(GetDepthStencilSurface)(THIS_ IDirect3DSurface9** ppZStencilSurface) 
	{ return m_pDevice->GetDepthStencilSurface(ppZStencilSurface); }

	STDMETHOD(BeginScene)(THIS) 
	{ return m_pDevice->BeginScene(); }

	STDMETHOD(EndScene)(THIS) 
	{ return m_pDevice->EndScene(); }

	STDMETHOD(Clear)(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil) 
	{ return m_pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil); }

	STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetTransform(State, pMatrix); 
	}

	STDMETHOD(GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) 
	{ return m_pDevice->GetTransform(State, pMatrix); }

	STDMETHOD(MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) 
	{ return m_pDevice->MultiplyTransform(State, pMatrix); }

	STDMETHOD(SetViewport)(THIS_ CONST D3DVIEWPORT9* pViewport) 
	{ return m_pDevice->SetViewport(pViewport); }

	STDMETHOD(GetViewport)(THIS_ D3DVIEWPORT9* pViewport) 
	{ return m_pDevice->GetViewport(pViewport); }

	STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetMaterial(pMaterial); 
	}

	STDMETHOD(GetMaterial)(THIS_ D3DMATERIAL9* pMaterial) 
	{ return m_pDevice->GetMaterial(pMaterial); }

	STDMETHOD(SetLight)(THIS_ DWORD Index,CONST D3DLIGHT9* pLight) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetLight(Index, pLight); 
	}

	STDMETHOD(GetLight)(THIS_ DWORD Index,D3DLIGHT9* pLight) 
	{ return m_pDevice->GetLight(Index, pLight); }

	STDMETHOD(LightEnable)(THIS_ DWORD Index,BOOL Enable) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->LightEnable(Index, Enable); 
	}

	STDMETHOD(GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable) 
	{ return m_pDevice->GetLightEnable(Index, pEnable); }

	STDMETHOD(SetClipPlane)(THIS_ DWORD Index,CONST float* pPlane) 
	{ return m_pDevice->SetClipPlane(Index, pPlane); }

	STDMETHOD(GetClipPlane)(THIS_ DWORD Index,float* pPlane) 
	{ return m_pDevice->GetClipPlane(Index, pPlane); }

	STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetRenderState(State, Value); 
	}

	STDMETHOD(GetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue) 
	{ return m_pDevice->GetRenderState(State, pValue); }

	STDMETHOD(CreateStateBlock)(THIS_ D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) 
	{ return m_pDevice->CreateStateBlock(Type, ppSB); }

	STDMETHOD(BeginStateBlock)(THIS) 
	{ return m_pDevice->BeginStateBlock(); }

	STDMETHOD(EndStateBlock)(THIS_ IDirect3DStateBlock9** ppSB) 
	{ return m_pDevice->EndStateBlock(ppSB); }

	STDMETHOD(SetClipStatus)(THIS_ CONST D3DCLIPSTATUS9* pClipStatus) 
	{ return m_pDevice->SetClipStatus(pClipStatus); }

	STDMETHOD(GetClipStatus)(THIS_ D3DCLIPSTATUS9* pClipStatus) 
	{ return m_pDevice->GetClipStatus(pClipStatus); }

	STDMETHOD(GetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9** ppTexture) 
	{ return m_pDevice->GetTexture(Stage, ppTexture); }

	STDMETHOD(SetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9* pTexture) 
	{ 
		++m_nTotalTextureChanges;
		return m_pDevice->SetTexture(Stage, pTexture); 
	}

	STDMETHOD(GetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) 
	{ return m_pDevice->GetTextureStageState(Stage, Type, pValue); }

	STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetTextureStageState(Stage, Type, Value); 
	}

	STDMETHOD(GetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) 
	{ return m_pDevice->GetSamplerState(Sampler, Type, pValue); }

	STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetSamplerState(Sampler, Type, Value); 
	}

	STDMETHOD(ValidateDevice)(THIS_ DWORD* pNumPasses) 
	{ return m_pDevice->ValidateDevice(pNumPasses); }

	STDMETHOD(SetPaletteEntries)(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries) 
	{ return m_pDevice->SetPaletteEntries(PaletteNumber, pEntries); }

	STDMETHOD(GetPaletteEntries)(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries) 
	{ return m_pDevice->GetPaletteEntries(PaletteNumber, pEntries); }

	STDMETHOD(SetCurrentTexturePalette)(THIS_ UINT PaletteNumber) 
	{ return m_pDevice->SetCurrentTexturePalette(PaletteNumber); }

	STDMETHOD(GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber) 
	{ return m_pDevice->GetCurrentTexturePalette(PaletteNumber); }

	STDMETHOD(SetScissorRect)(THIS_ CONST RECT* pRect) 
	{ return m_pDevice->SetScissorRect(pRect); }

	STDMETHOD(GetScissorRect)(THIS_ RECT* pRect) 
	{ return m_pDevice->GetScissorRect(pRect); }

	STDMETHOD(SetSoftwareVertexProcessing)(THIS_ BOOL bSoftware) 
	{ return m_pDevice->SetSoftwareVertexProcessing(bSoftware); }

	STDMETHOD_(BOOL, GetSoftwareVertexProcessing)(THIS) 
	{ return m_pDevice->GetSoftwareVertexProcessing(); }

	STDMETHOD(SetNPatchMode)(THIS_ float nSegments) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetNPatchMode(nSegments); 
	}

	STDMETHOD_(float, GetNPatchMode)(THIS) 
	{ return m_pDevice->GetNPatchMode(); }

	STDMETHOD(DrawPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) 
	{ return m_pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); }

	STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount) 
	{ return m_pDevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount); }

	STDMETHOD(DrawPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) 
	{ return m_pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); }

	STDMETHOD(DrawIndexedPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) 
	{ return m_pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); }

	STDMETHOD(ProcessVertices)(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags) 
	{ return m_pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags); }

	STDMETHOD(CreateVertexDeclaration)(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl) 
	{ return m_pDevice->CreateVertexDeclaration(pVertexElements, ppDecl); }

	STDMETHOD(SetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9* pDecl) 
	{ return m_pDevice->SetVertexDeclaration(pDecl); }

	STDMETHOD(GetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9** ppDecl) 
	{ return m_pDevice->GetVertexDeclaration(ppDecl); }

	STDMETHOD(SetFVF)(THIS_ DWORD FVF) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetFVF(FVF); 
	}

	STDMETHOD(GetFVF)(THIS_ DWORD* pFVF) 
	{ return m_pDevice->GetFVF(pFVF); }

	STDMETHOD(CreateVertexShader)(THIS_ CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader) 
	{ return m_pDevice->CreateVertexShader(pFunction, ppShader); }

	STDMETHOD(SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetVertexShader(pShader); 
	}

	STDMETHOD(GetVertexShader)(THIS_ IDirect3DVertexShader9** ppShader) 
	{ return m_pDevice->GetVertexShader(ppShader); }

	STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); 
	}

	STDMETHOD(GetVertexShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) 
	{ return m_pDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }

	STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); 
	}

	STDMETHOD(GetVertexShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) 
	{ return m_pDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }

	STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); 
	}

	STDMETHOD(GetVertexShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) 
	{ return m_pDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }

	STDMETHOD(SetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) 
	{ return m_pDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride); }

	STDMETHOD(GetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride) 
	{ return m_pDevice->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride); }

	STDMETHOD(SetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT Setting) 
	{ return m_pDevice->SetStreamSourceFreq(StreamNumber, Setting); }

	STDMETHOD(GetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT* pSetting) 
	{ return m_pDevice->GetStreamSourceFreq(StreamNumber, pSetting); }

	STDMETHOD(SetIndices)(THIS_ IDirect3DIndexBuffer9* pIndexData) 
	{ return m_pDevice->SetIndices(pIndexData); }

	STDMETHOD(GetIndices)(THIS_ IDirect3DIndexBuffer9** ppIndexData) 
	{ return m_pDevice->GetIndices(ppIndexData); }

	STDMETHOD(CreatePixelShader)(THIS_ CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader) 
	{ return m_pDevice->CreatePixelShader(pFunction, ppShader); }

	STDMETHOD(SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader) 
	{ return m_pDevice->SetPixelShader(pShader); }

	STDMETHOD(GetPixelShader)(THIS_ IDirect3DPixelShader9** ppShader) 
	{ return m_pDevice->GetPixelShader(ppShader); }

	STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); 
	}

	STDMETHOD(GetPixelShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) 
	{ return m_pDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }

	STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); 
	}

	STDMETHOD(GetPixelShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) 
	{ return m_pDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }

	STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) 
	{ 
		++m_nTotalStateChanges;
		return m_pDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); 
	}

	STDMETHOD(GetPixelShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) 
	{ return m_pDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }

	STDMETHOD(DrawRectPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo) 
	{ return m_pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo); }

	STDMETHOD(DrawTriPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo) 
	{ return m_pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo); }

	STDMETHOD(DeletePatch)(THIS_ UINT Handle) 
	{ return m_pDevice->DeletePatch(Handle); }

	STDMETHOD(CreateQuery)(THIS_ D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) 
	{ return m_pDevice->CreateQuery(Type, ppQuery); }

public:
	LPDIRECT3DDEVICE9	m_pDevice;
	BOOL				m_bEnable;

	int					m_nTotalTextures;
	int					m_nTotalRenderTextures;
	int					m_nTotalDepthStencilTextures;

	int					m_nTotalIBUsedMems;
	int					m_nTotalVBUsedMems;

	int					m_nTotalIndexBuffers;
	int					m_nTotalVertexBuffers;

	int					m_nTotalStateChanges;
	int					m_nTotalTextureChanges;

};

extern KDebugDirect3DDevice9 g_DebugD3DDevice;


