////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontBase.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font def
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTBASE_H_
#define _INCLUDE_KG3DFONTBASE_H_


#include  "KG3DFontDescr.h"



//////////////////////////////////////////////////////////////////////
//   class  :   KG3DFontBase
//   brief  :   font abstract base class
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class KG3DFontRenderer;
class KG3DFontBase
{
public:
	////////////////////////////////////////////////
	// constructor / destructor
	KG3DFontBase( KG3DFontRenderer* pFontRenderer );
	virtual ~KG3DFontBase();


	////////////////////////////////////////////////
	// build / cleanup
	// 想要使用某种风格必须在创建以及初始化的时候就指定
	BOOL  Initialize( LPCTSTR fontName, INT fontSize, UINT fontStyle, UINT fontDpi = KG3DFONT_DEFAULT_DPI );
	BOOL  CleanUp();


	////////////////////////////////////////////////
	// preload range : preload ansi char glyph
	// preload text  : preload any char glyph
	UINT  PreloadRange( UCHAR First = 0x20, UCHAR Last = 0x7F );
	UINT  PreloadText( LPCTSTR lpchText, int cchText );


	////////////////////////////////////////////////
	// draw text
	// 三维文本，二维文本
	void  DrawText( FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FLOAT cchSpace = 0.0f );
	void  Draw3DText( FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FLOAT cchSpace = 0.0f );


	////////////////////////////////////////////////
	// get / set text member
	void  SetTextAlpha( BYTE alpha );
	void  SetTextColour( UINT colour );
	void  SetTextScaling( FLOAT scaleX = 1.0f, FLOAT scaleY = 1.0f );

	BYTE  GetTextAlpha( ) const;
	UINT  GetTextColour( ) const;
	void  GetTextScaling( FLOAT& scaleX, FLOAT& scaleY ) const;

	BOOL  GetTextExtent( LPCTSTR lpchText, int cchText, FLOAT &fWidth, FLOAT &fHeight, FLOAT cchSpace = 0.0f );
	BOOL  GetTextPosExtent( LPCTSTR lpchText, int cchText, FLOAT fCursorPos, LPINT lpCharPos, FLOAT cchSpace = 0.0f );


	////////////////////////////////////////////////
	// get / set text 效果
	void  SetTextEffect( UINT effect );
	void  SetTextBorder( UINT color, UINT weight );
	void  SetTextProjection( UINT color, UINT weight );

	UINT  GetTextEffect( ) const;
	void  GetTextBorder( UINT& color, UINT& weight ) const;
	void  GetTextProjection( UINT& color, UINT& weight ) const;


	////////////////////////////////////////////////
	// get char width / height
	FLOAT GetCharWidth() const;
	FLOAT GetCharHeight() const;

	FLOAT GetCharAscent() const;
	FLOAT GetCharDescent() const;


protected:
	////////////////////////////////////////////////
	// draw font member
	virtual void  drawText( FLOAT penX, FLOAT penY, WCHAR chText ) = 0;
	virtual void  drawTextEx( FLOAT penX, FLOAT penY, WCHAR chText ) = 0;


	////////////////////////////////////////////////
	// glyph font member
	virtual bool  charGlyphMake( KG3DFontGlyph_t** fontGlyph ) = 0;
	virtual bool  charGlyphInfill( UINT charCode, void* ftGlyph, KG3DFontGlyph_t* fontGlyph ) = 0;
	virtual bool  charGlyphDestroy( ) = 0;


protected:
	////////////////////////////////////////////////
	// init member
	void  initMember();
	void  initSizeMember();


	////////////////////////////////////////////////
	// draw text
	typedef void  (KG3DFontBase::*FN_DRAWGLYPH)( FLOAT penX, FLOAT penY, WCHAR chText );

	void  drawNormalText( FLOAT baseX, FLOAT baseY, LPCWSTR lpchText, FN_DRAWGLYPH fnDrawText );
	void  drawStandardText( FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText );
	void  drawVerticalText( FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText );


	////////////////////////////////////////////////
	// char member
	LPCWSTR charToUnicode( LPCTSTR lpchText, int cchText );
	LPCWSTR	charToUnicode( INT& nUText, LPCTSTR lpchText, INT cchText );

	KG3DFontGlyph_t* charToGlyphSlot( UINT charCode );
	INT   charToGlyphIndex( UINT charCode );
	INT   charToIndex( UINT charCode );


	////////////////////////////////////////////////
	// glyph font member
	bool  charGlyphHit( UINT charCode );
	bool  charGlyphAdvance( UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY );

	bool  charGlyphAdvance( UINT chText, FLOAT& advX, FLOAT& advY );
	bool  charGlyphKernAdvance( UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY );


	////////////////////////////////////////////////
	// get usage / budget
	INT   getGlyphUsage() const;
	INT   getGlyphBudget() const;


protected:
	////////////////////////////////////////////////
	// face member
	struct KG3DFontFace_t;

	KG3DFontRenderer* m_fontRenderer;
	KG3DFontFace_t*	 m_fontFace;
	
	TCHAR			 m_fontPaths[MAX_PATH];
	UINT			 m_fontGlyphs;
	UINT			 m_fontUploads;
	UINT			 m_fontEncoding;

	BOOL			 m_fontUnicode;
	BOOL			 m_fontVertical;
	BOOL			 m_fontKerningTable;

	UINT			 m_fontFlagDraw;
	UINT			 m_fontFlagLoader;


	////////////////////////////////////////////////
	// size member
	UINT			 m_fontPoint;
	UINT			 m_fontDPI;

	FLOAT			 m_fontWidth;
	FLOAT			 m_fontHeight;

	FLOAT			 m_fontAscender;
	FLOAT			 m_fontDescender;


	////////////////////////////////////////////////
	// render member
	D3DCOLOR		 m_fontColour;
	FLOAT			 m_fontSpaceX;
	FLOAT			 m_fontSpaceY;

	FLOAT			 m_fontScalingX;
	FLOAT			 m_fontScalingY;

	
	UINT			 m_fontEffect;
	UINT			 m_fontBorderCr;
	INT				 m_fontBorderWeight;
	
	UINT			 m_fontProjectionCr;
	INT			     m_fontProjectionWeight;


	////////////////////////////////////////////////
	// glyphs member
	//typedef	std::vector< KG3DFontGlyph_t* >  FontVector_t;
	//typedef	stdext::hash_map< UINT, UINT >  FontMap_t;
	typedef	WCHAR			 FontUText_t[KG3DFONT_GLYPH_NUMS];
	//typedef INT				 FontMap_t[KG3DFONT_GLYPH_NUMS];
	typedef	KG3DFontGlyph_t* FontVector_t[ KG3DFONT_GLYPH_LIMIT ];

	//typedef USHORT FontMap_t[ KG3DFONT_GLYPH_LIMIT ];


	//FontMap_t		 m_fontCharMap;
	FontUText_t		 m_fontCharWide;
	FontVector_t	 m_fontCharVector;


};



////////////////////////////////////////////////
// set text member
inline void  KG3DFontBase::SetTextAlpha( BYTE alpha )
{
	m_fontColour &= 0xFFFFFF;
	m_fontColour |= ( ( ( alpha ) & 0xFF) << 24 );

}

inline void  KG3DFontBase::SetTextColour( UINT color )
{
	m_fontColour &= 0xFF000000;
	m_fontColour |= ( ( color ) & 0xFFFFFF );
}

inline void  KG3DFontBase::SetTextScaling( FLOAT scaleX, FLOAT scaleY )
{
	m_fontScalingX = scaleX;
	m_fontScalingY = scaleY;
}

inline BYTE  KG3DFontBase::GetTextAlpha( ) const
{
	return (BYTE)( ( m_fontColour & 0xFF000000 ) >> 24 );
}

inline UINT  KG3DFontBase::GetTextColour( ) const
{
	return ( m_fontColour & 0xFFFFFF );
}

inline void KG3DFontBase::GetTextScaling( FLOAT& scaleX, FLOAT& scaleY ) const
{
	scaleX = m_fontScalingX;
	scaleY = m_fontScalingY;
}



////////////////////////////////////////////////
// get / set text 效果
inline void  KG3DFontBase::SetTextEffect( UINT effect )
{
	m_fontEffect = ( effect & (~KG3DFONT_STYLE_VERTICAL ) );
}

inline void  KG3DFontBase::SetTextBorder( UINT color, UINT weight )
{
	// 限制边框大小
	KG3DFONT_ASSERT( 2 >= weight );
	m_fontBorderCr		=	color;
	m_fontBorderWeight	=	weight;
}

inline void  KG3DFontBase::SetTextProjection( UINT color, UINT weight )
{
	m_fontProjectionCr		=	color;
	m_fontProjectionWeight	=	weight;
}

inline UINT  KG3DFontBase::GetTextEffect( ) const
{
	return	m_fontEffect;
}

inline void  KG3DFontBase::GetTextBorder( UINT& color, UINT& weight ) const
{
	color	=	m_fontBorderCr;
	weight	=	m_fontBorderWeight;
}

inline void  KG3DFontBase::GetTextProjection( UINT& color, UINT& weight ) const
{
	color	=	m_fontProjectionCr;
	weight	=	m_fontProjectionWeight;
}



////////////////////////////////////////////////
// get char width / height
inline FLOAT KG3DFontBase::GetCharWidth() const
{
	return m_fontWidth;
}

inline FLOAT KG3DFontBase::GetCharHeight() const
{
	return m_fontHeight;
}


inline FLOAT KG3DFontBase::GetCharAscent() const
{
	return m_fontAscender;
}

inline FLOAT KG3DFontBase::GetCharDescent() const
{
	return m_fontDescender;
}




#endif //_INCLUDE_KG3DFONTBASE_H_