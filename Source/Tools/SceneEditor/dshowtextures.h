//-----------------------------------------------------------------------------
// File: DShowTextures.h
//
// Desc: DirectShow sample code - adds support for DirectShow videos playing 
//       on a DirectX 8.0 texture surface. Turns the D3D texture tutorial into 
//       a recreation of the VideoTex sample from previous versions of DirectX.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "./DShowBase/streams.h"

//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;
//-----------------------------------------------------------------------------
extern LPDIRECT3D9             g_pD3D;       // Used to create the D3DDevice
extern LPDIRECT3DDEVICE9       g_pd3dDevice; // Our rendering device
extern LPDIRECT3DVERTEXBUFFER9 g_pVB;        // Buffer to hold vertices
extern LPDIRECT3DTEXTURE9      g_pTexture;   // Our texture

#define MAX_TEXTURE_COORD 2
//-----------------------------------------------------------------------------
// CTextureRenderer Class Declarations
//-----------------------------------------------------------------------------
class KMovieTexture;

class CTextureRenderer : public CBaseVideoRenderer
{
	friend class KMovieTexture;
public:
    CTextureRenderer(LPUNKNOWN pUnk,HRESULT *phr, IDirect3DTexture9** ppTexture);
    ~CTextureRenderer();

public:
    HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
    HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
    HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample
	IDirect3DTexture9* GetTexture() { return *m_ppTexture;}
	HRESULT RecreateTexture(D3DSURFACE_DESC& desc);
	
	BOOL      m_bUseDynamicTextures;
    LONG      m_lVidWidth;   // Video width
    LONG      m_lVidHeight;  // Video Height
    LONG      m_lVidPitch;   // Video Pitch
	float     m_fResize[MAX_TEXTURE_COORD];
	D3DFORMAT m_TextureFormat;

private:
	IDirect3DTexture9** m_ppTexture;
};


