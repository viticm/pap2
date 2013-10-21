#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include "./fonttypes.h"

class FontRenderer
{
public:
	FontRenderer();
	virtual ~FontRenderer();

	bool  Initialize();
	bool  CleanUp();

	bool  Prepare();
	void  Flush();

	void  DestroyTexture();
	void  DeleteTexture(UINT Index);

	bool  BuildTexture(UINT& Index, UINT& Width, UINT& Height, UINT glyphW, UINT glyphH);
	bool  UploadTexture(UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, BYTE const* pUpload);
	bool  UploadMonoTexture(UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, UINT pitchSrc, BYTE const* pUpload);

	void  PrepareVeretex(UINT TexIndex, FontVertex* pVertex, USHORT nums);
	void  PrepareVeretex(UINT TexIndex, FontTLVertex* pVertex, USHORT nums);

protected:
	void  checkD3DTextureCaps(LPDIRECT3DDEVICE9 pD3DDevice);
	void  checkD3DTextureFormat(LPDIRECT3DDEVICE9 pD3DDevice);

	void  flushTexture(UINT TexIndex);
	void  flushVertex();
	void  flushTLVertex();

protected:
	std::vector<LPDIRECT3DTEXTURE9>  m_TextureVector;
	UINT		m_TextureLast;

	D3DFORMAT	m_TextureFmt;
	UINT		m_TextureFmtSkip;

	UINT		m_TextureWidth;
	UINT		m_TextureHeight;

	USHORT		m_nNumIndexs;
	USHORT		m_nNumTLIndexs;

	USHORT		m_nNumVertexs;
	USHORT		m_nNumTLVertexs;

	USHORT		m_pIndexBuffer[KG3DFONT_NUMS_INDEX];
	USHORT		m_pTLIndexBuffer[KG3DFONT_NUMS_INDEX];

	FontVertex		m_pVertexBuffer[KG3DFONT_NUMS_VERTEX];
	FontTLVertex	m_pTLVertexBuffer[KG3DFONT_NUMS_TLVERTEX];
};


#endif // FONT_RENDERER_H

