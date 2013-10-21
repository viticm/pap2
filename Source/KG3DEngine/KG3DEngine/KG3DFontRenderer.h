////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontRenderer.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font renderer
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTRENDERER_H_
#define _INCLUDE_KG3DFONTRENDERER_H_

#include "./KG3DFontDescr.h"

class KG3DFontRenderer
{
private:
	enum { MAX_FONT = 128 };
	enum { MAX_RENDER_BATCH = 16 };

	enum { MAX_FONT_VERTEX = MAX_FONT * 4 };
	enum { MAX_FONT_VERTEX_INDEX = MAX_FONT * 6 };

	struct KRenderBatch
	{
		LPDIRECT3DTEXTURE9	pTexture;
        LPDIRECT3DVERTEXBUFFER9 pVB;

		UINT				uTLVertex;
	};

	typedef std::vector<LPDIRECT3DTEXTURE9> KTextureArray;

public:
	enum { INVALID_TEXTURE_INDEX = 0xFFFFFFFF };

public:
	KG3DFontRenderer();
	~KG3DFontRenderer();

	HRESULT Initialize(LPDIRECT3DDEVICE9 pD3DDevice);
	HRESULT CleanUp();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	HRESULT Prepare();
	HRESULT Flush();

	BOOL  BuildTexture(UINT& uIndex, UINT& uWidth, UINT& uHeight, UINT uGlyphWidth, UINT uGlyphHeight, UINT uMipmapLevel);
	void  DeleteTexture(UINT uIndex);

	BOOL  UploadTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, BYTE const* pUpload, UCHAR *puchAdjust);
	BOOL  UploadMonoTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, UINT uUploadPitch, BYTE const* pUpload);

	BOOL  UploadBoderTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, BYTE const* pUpload, UCHAR *puchAdjust);
	BOOL  UploadMonoBoderTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, UINT uUploadPitch, BYTE const* pUpload);

	HRESULT PrepareVertex(UINT uIndex, KG3DFontTLVertex* pVertex, UINT uVertex);

protected:
	HRESULT CheckD3DTextureCaps(LPDIRECT3DDEVICE9 pD3DDevice);
	HRESULT CheckD3DTextureFormat(LPDIRECT3DDEVICE9 pD3DDevice);

	HRESULT FlushTexture(UINT uIndex);
	HRESULT FlushTLVertex();

private:
    size_t GetTexturePixelByte() const;

    HRESULT CreateFontIndexBuffer();
    HRESULT DestroyFontIndexBuffer();

    HRESULT CreateFontVertexBuffer();
    HRESULT DestroyFontVertexBuffer();

protected:
	LPDIRECT3DDEVICE9	m_pD3DDevice;
	LPDIRECT3DINDEXBUFFER9 m_pFontIndexBuffer;

	KTextureArray m_aTexture;

	D3DFORMAT	m_TextureFormat;
	UINT		m_TextureFmtSkip;

	KRenderBatch	m_RenderBatch[MAX_RENDER_BATCH];
	KRenderBatch*	m_pRenderBatch;
	UINT			m_uFlushTextureIndex;
};

#endif //_INCLUDE_KG3DFONTRENDERER_H_