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


#include  "KG3DFontBase.h"



//////////////////////////////////////////////////////////////////////
//   class  :   KG3DFontRenderer
//   brief  :   font renderer class
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class KG3DFontRenderer
{
public:
	////////////////////////////////////////////////
	// constructor / destructor
	KG3DFontRenderer( );
	virtual ~KG3DFontRenderer();


	////////////////////////////////////////////////
	// initialize / cleanup
	bool  Initialize( LPDIRECT3DDEVICE9 pD3DDevice );
	bool  CleanUp();


	////////////////////////////////////////////////
	// prepare / flush
	bool  Prepare( );
	void  Flush( );


	////////////////////////////////////////////////
	// texture
	void  DestroyTexture();
	void  DeleteTexture( UINT Index );

	bool  BuildTexture( UINT& Index, UINT& Width, UINT& Height, UINT glyphW, UINT glyphH );
	bool  UploadTexture( UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, BYTE const* pUpload );
	bool  UploadMonoTexture( UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, UINT pitchSrc, BYTE const* pUpload );


	////////////////////////////////////////////////
	// vertex buffer
	void  PrepareVeretex( UINT TexIndex, KG3DFontVertex_t* pVertex, USHORT nums );
	void  PrepareVeretex( UINT TexIndex, KG3DFontTLVertex_t* pVertex, USHORT nums );


	////////////////////////////////////////////////
	// effect
	// 现在没有使用，用作扩展功能
	void  ApplyNoneEffect( );
	void  ApplyBorderEffect( UINT color, UINT weight );


protected:
	////////////////////////////////////////////////
	// check d3d format
	void  checkD3DTextureCaps( LPDIRECT3DDEVICE9 pD3DDevice );
	void  checkD3DTextureFormat( LPDIRECT3DDEVICE9 pD3DDevice );


	////////////////////////////////////////////////
	// effect
	void  effectLoad();
	void  effectUnload();

	UINT  effectBegin( );
	void  effectEnd( );

	bool  effectBeginPass( UINT pass );
	void  effectEndPass( );


	////////////////////////////////////////////////
	// flush
	void  flushTexture( UINT TexIndex );
	void  flushVertex();
	void  flushTLVertex();

	void  flushEffectVertex();
	void  flushEffectTLVertex();


protected:
	////////////////////////////////////////////////
	// d3d device
	LPDIRECT3DDEVICE9	m_pD3DDevice;


	////////////////////////////////////////////////
	// effect fx
	LPD3DXEFFECT		m_pEffectCur;
	LPD3DXEFFECT		m_pEffectShader;


	////////////////////////////////////////////////
	// texture buffer
	std::vector< LPDIRECT3DTEXTURE9 >  m_TextureVector;
	UINT		m_TextureLast;

	D3DFORMAT	m_TextureFmt;
	UINT		m_TextureFmtSkip;

	UINT		m_TextureWidth;
	UINT		m_TextureHeight;


	////////////////////////////////////////////////
	// index / vertex buffer
	USHORT		m_nNumIndexs;
	USHORT		m_nNumTLIndexs;

	USHORT		m_nNumVertexs;
	USHORT		m_nNumTLVertexs;

	USHORT					m_pIndexBuffer[KG3DFONT_NUMS_INDEX];
	USHORT					m_pTLIndexBuffer[KG3DFONT_NUMS_INDEX];

	KG3DFontVertex_t		m_pVertexBuffer[KG3DFONT_NUMS_VERTEX];
	KG3DFontTLVertex_t		m_pTLVertexBuffer[KG3DFONT_NUMS_TLVERTEX];

};



#endif //_INCLUDE_KG3DFONTRENDERER_H_