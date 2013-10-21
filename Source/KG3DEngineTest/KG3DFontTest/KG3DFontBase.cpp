////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontBase.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font base
//
////////////////////////////////////////////////////////////////////////////////


#include  <ft2build.h>
#include  FT_FREETYPE_H
#include  FT_GLYPH_H
#include  FT_OUTLINE_H
//#include  FT_INTERNAL_OBJECTS_H 

#include  "KG3DFontBase.h"
#include  "KG3DFontRenderer.h"


namespace
{
	////////////////////////////////////////////////
	// KG3DFTFontLibrary
	class KG3DFTFontLibrary
	{
	private:
		////////////////////////////////////////////////
		// private
		static std::auto_ptr<KG3DFTFontLibrary> m_pImpl;


	public:
		////////////////////////////////////////////////
		// destructor
		~KG3DFTFontLibrary()
		{
			if ( 0 != m_library )
			{
				FT_Done_FreeType( m_library );
				m_library = 0;
			}
		
		}
		
		
		////////////////////////////////////////////////
		// singleton
		static KG3DFTFontLibrary* Instance()
		{
			if ( NULL == m_pImpl.get() )
			{
				KG3DFTFontLibrary* pTemp = NULL;

				KG3DFONT_NEW( pTemp, KG3DFTFontLibrary );
				if ( NULL != pTemp )
				{
					m_pImpl.reset( pTemp );
					if ( FT_Init_FreeType( &m_pImpl->m_library ) )
					{
						m_pImpl.reset();
					}
				}
			
			}


			return m_pImpl.get();
		}



	private:
		////////////////////////////////////////////////
		// constructors.
		KG3DFTFontLibrary()
		{

		}

		KG3DFTFontLibrary( KG3DFTFontLibrary const& rhs );
		KG3DFTFontLibrary& operator=( KG3DFTFontLibrary const& rhs );


	public:
		////////////////////////////////////////////////
		// member
		FT_Library	m_library;

	};


	std::auto_ptr<KG3DFTFontLibrary>  KG3DFTFontLibrary::m_pImpl;



	////////////////////////////////////////////////
	// inline 
	inline void kg3dGlyphInfill( BOOL ftVertical, UINT ftCode, INT ftIndex, KG3DFontGlyph_t* fontGlyph, FT_GlyphSlot ftGlyph )
	{
		// index
		fontGlyph->glyphIndex  = ftIndex;


		// advance
		fontGlyph->glyphAdvance[ KG3DFONT_SLOT_X ] = KG3DFONT_FIXDIV( ftGlyph->advance.x );
		fontGlyph->glyphAdvance[ KG3DFONT_SLOT_Y ] = KG3DFONT_FIXDIV( ftGlyph->advance.y );

		//if ( ( 0x4E00 > ftCode ) || ( ftCode > 0x9FFF ) )
		//{
		//	FT_Face facePtr = ftGlyph->face;
		//	FT_Size fontPSize = facePtr->size;
		//	KG3DFONT_ASSERT( NULL != fontPSize );

		//	if ( !ftVertical
		//		&& ( ftGlyph->advance.x >= ( fontPSize->metrics.max_advance / 2 ) ) )
		//	{
		//		fontGlyph->glyphAdvance[ KG3DFONT_SLOT_X ] = KG3DFONT_FIXDIV( fontPSize->metrics.max_advance / 2 );
		//	}
		//	
		//	if ( ftVertical
		//		&& ( ftGlyph->advance.y >= ( fontPSize->metrics.max_advance / 2 ) ) )
		//	{
		//		fontGlyph->glyphAdvance[ KG3DFONT_SLOT_Y ] = KG3DFONT_FIXDIV( fontPSize->metrics.max_advance / 2 );
		//	}

		//}


		// bound box
		FT_BBox bbox;

		FT_Outline_Get_CBox( &(ftGlyph->outline), &bbox );

		fontGlyph->glyphBLower[ KG3DFONT_SLOT_X ] = KG3DFONT_FIXDIV( bbox.xMin );
		fontGlyph->glyphBLower[ KG3DFONT_SLOT_Y ] = KG3DFONT_FIXDIV( bbox.yMin );

		fontGlyph->glyphBUpper[ KG3DFONT_SLOT_X ] = KG3DFONT_FIXDIV( bbox.xMax );
		fontGlyph->glyphBUpper[ KG3DFONT_SLOT_Y ] = KG3DFONT_FIXDIV( bbox.yMax );

	}


}



////////////////////////////////////////////////
// define font face
#define  KG3DFACE_ERROR				L'?'
#define  KG3DFACE_POINTER			m_fontFace->pImpl_
#define  KG3DFACE_REFERENCE			( *m_fontFace->pImpl_ )


struct KG3DFontBase::KG3DFontFace_t
{
	FT_Face  pImpl_;
};


////////////////////////////////////////////////
// constructor / destructor
KG3DFontBase::KG3DFontBase( KG3DFontRenderer* pFontRenderer )
		:	m_fontRenderer( pFontRenderer ),
			m_fontFace( new KG3DFontFace_t() ),
			m_fontGlyphs( 0 ),
			m_fontUploads( 0 ),
			m_fontEncoding( FT_ENCODING_NONE ),
			m_fontUnicode( FALSE ),
			m_fontVertical( FALSE ),
			m_fontKerningTable( FALSE ),
			m_fontFlagDraw( FT_RENDER_MODE_NORMAL ),
			m_fontFlagLoader( FT_LOAD_NO_HINTING ),
			m_fontPoint( 0 ),
			m_fontDPI( 0 ),
			m_fontAscender( 0.0f ),
			m_fontDescender( 0.0f ),
			m_fontColour( 0xFFFFFFFF ),
			m_fontScalingX( 1.0f ),
			m_fontScalingY( 1.0f ),
			m_fontEffect( KG3DFONT_STYLE_NONE )
{
	//memset( m_fontCharMap, 0, sizeof( m_fontCharMap ) );
	memset( m_fontCharWide, 0, sizeof( m_fontCharWide ) );
	memset( m_fontCharVector, 0, sizeof( m_fontCharVector ) );

}


KG3DFontBase::~KG3DFontBase()
{
	delete m_fontFace;
}



////////////////////////////////////////////////
// build / cleanup
BOOL  KG3DFontBase::Initialize( LPCTSTR fontName, INT fontSize, UINT fontStyle, UINT fontDpi )
//BOOL  KG3DFontBase::BuildFont( LPCTSTR fontName, INT fontSize, UINT fontDpi )
{
	// check valid
	if ( NULL == KG3DFTFontLibrary::Instance() )
	{
		return false;
	}


	// new face
	FT_Long const DEFAULT_FACE_INDEX = 0;
	FT_Face pFontFace = NULL;
	FT_Error err = 0;


	_tcsncpy( m_fontPaths, fontName, sizeof(m_fontPaths) );
	err = FT_New_Face( KG3DFTFontLibrary::Instance()->m_library, m_fontPaths, DEFAULT_FACE_INDEX, &pFontFace );
	if ( 0 != err)
	{
		if ( FT_Err_Unknown_File_Format == err )
		{
			KG3DFONT_MSG_ASSERT( "FT_Err_Unknown_File_Format == err" );
		}


		KG3DFACE_POINTER = NULL;
		return FALSE;
	}
	else
	{
		m_fontGlyphs = pFontFace->num_glyphs;
		m_fontUploads = pFontFace->num_glyphs;
		m_fontKerningTable = FT_HAS_KERNING( pFontFace );
	}


	// get face caps
	//if ( !FT_HAS_VERTICAL( pFontFace ) )
	if ( !FT_IS_SCALABLE( pFontFace ) )
	{
		KG3DFONT_MSG_ASSERT( "Sorry, font is not scalable." );
		return FALSE;
	}


	// get face encoding
	//m_fontFlagDraw = !fontAntialias ? FT_RENDER_MODE_MONO : FT_RENDER_MODE_NORMAL;
	m_fontEncoding = FT_ENCODING_UNICODE;
	m_fontUnicode  = FALSE;


	if ( NULL != pFontFace->charmap )
	{
		for ( int i = 0; i < pFontFace->num_charmaps; ++ i )
		{
			if ( FT_Encoding( m_fontEncoding ) == pFontFace->charmaps[i]->encoding )
			{
				m_fontUnicode = TRUE;
				break ;
			}

		}

		if ( !m_fontUnicode )
		{
			 m_fontEncoding = pFontFace->charmaps[0]->encoding;
			 m_fontUnicode  = ( m_fontEncoding == FT_ENCODING_UNICODE );
		}


		err = FT_Select_Charmap( pFontFace, FT_Encoding( m_fontEncoding ) );
		if ( 0 != err )
		{
			// tmd;
			KG3DFONT_MSG_ASSERT( "FT_Select_Charmap" );
			CleanUp();
			return FALSE;
		}


	}
	else
	{
		m_fontEncoding = pFontFace->charmap->encoding;
		m_fontUnicode  = ( m_fontEncoding == FT_ENCODING_UNICODE );
	}
		

	// set char size
	err = FT_Set_Char_Size( pFontFace, 0L, KG3DFONT_FIXMULT( fontSize ), fontDpi, fontDpi );
	if ( 0 != err )
	{
		// tmd;
		KG3DFONT_MSG_ASSERT( "FT_Set_Char_Size" );
		CleanUp();
		return FALSE;
	}


	KG3DFACE_POINTER	 = pFontFace;
	m_fontPoint			 = fontSize;
	m_fontDPI			 = fontDpi;


	m_fontEffect		 = ( fontStyle & (~KG3DFONT_STYLE_VERTICAL ) );
	m_fontVertical		 = fontStyle & KG3DFONT_STYLE_VERTICAL;
	m_fontFlagLoader	 = m_fontVertical ? m_fontFlagLoader | FT_LOAD_VERTICAL_LAYOUT : m_fontFlagLoader;



	// get char prop
	FT_Size fontPSize = ( NULL != KG3DFACE_POINTER ) ?  KG3DFACE_POINTER->size : NULL;
	KG3DFONT_ASSERT( NULL != fontPSize );

	if ( FT_IS_SCALABLE( KG3DFACE_POINTER ) )
	{
		m_fontWidth  = ( KG3DFACE_POINTER->bbox.xMax - KG3DFACE_POINTER->bbox.xMin )
			* ( (FLOAT)( fontPSize->metrics.x_ppem ) / (FLOAT)( KG3DFACE_POINTER->units_per_EM ) );
		m_fontHeight = ( KG3DFACE_POINTER->bbox.yMax - KG3DFACE_POINTER->bbox.yMin )
			* ( (FLOAT)fontPSize->metrics.y_ppem / (FLOAT)KG3DFACE_POINTER->units_per_EM );
	}
	else
	{
		m_fontWidth  = KG3DFONT_FIXDIV( fontPSize->metrics.max_advance );
		m_fontHeight = KG3DFONT_FIXDIV( fontPSize->metrics.height );
	}


	m_fontAscender   = KG3DFONT_FIXDIV( KG3DFACE_POINTER->size->metrics.ascender );
	m_fontDescender  = KG3DFONT_FIXDIV( KG3DFACE_POINTER->size->metrics.descender );



	return TRUE;
}

//
//BOOL  KG3DFontBase::BuildVerticalFont( LPCTSTR fontName, INT fontSize, UINT fontDpi )
//{
//	if ( !BuildFont( fontName, fontSize, fontDpi ) )
//	{
//		KG3DFONT_MSG_ASSERT( "BuildVerticalFont" );
//		return FALSE;
//	}
//
//
//	m_fontVertical		 = TRUE;
//	m_fontFlagLoader	|= FT_LOAD_VERTICAL_LAYOUT;
//
//	return TRUE;
//}


BOOL  KG3DFontBase::CleanUp()
{
	// glyph destroy
	if ( !charGlyphDestroy() )
	{

	}


	// reset font face
	if ( NULL != KG3DFACE_POINTER )
	{
		FT_Done_Face( KG3DFACE_POINTER );
		KG3DFACE_POINTER = NULL;
	}


	// init member
	initMember();
	initSizeMember();


	return TRUE;
}



////////////////////////////////////////////////
// preload text
UINT  KG3DFontBase::PreloadRange( UCHAR First /*= 0x20*/, UCHAR Last /*= 0x7F*/ )
{
	// check range
	UINT nPreloadText = 0;


	if ( ( First >= 0x20 )
		&& ( Last <= 0x7F ) )
	{
		// generating range
		UCHAR  charRange = Last - First + 1;
		CHAR   charGlyph[ 0x80 - 0x20 ] = {0};

		std::fill( charGlyph, charGlyph + charRange, First );
		for ( int i = 0; i < charRange; ++ i )
		{
			charGlyph[i] += static_cast<CHAR>( i );
		}


		// commit text
		nPreloadText = PreloadText( charGlyph, charRange );

	}


	return nPreloadText;
}

UINT  KG3DFontBase::PreloadText( LPCSTR lpchText, int cchText )
{
	// check valid
	if ( ( NULL == lpchText )
		|| ( -1 >  cchText )
		|| ( 0  == cchText ) )
	{
		return 0;
	}


	// draw text
	LPCWSTR lpchStr = charToUnicode( lpchText, cchText );
	UINT    nPreloadText = 0;

	while ( *lpchStr )
	{
		// get char code
		if ( charGlyphHit( * lpchStr ++ ) )
		{
			++ nPreloadText;
		}

	}


	return nPreloadText;
}



////////////////////////////////////////////////
// draw text
//void  KG3DFontBase::DrawText( LPCSTR lpchText, int cchText )
//{
//	// check valid
//	if ( ( NULL == lpchText )
//		|| ( 0 >= cchText ) )
//	{
//		return ;
//	}
//
//
//	// draw text
//	LPCWSTR lpchStr = charToUnicode( lpchText, cchText );
//	FLOAT   penX = 0.0f, penY = 0.0f;
//	FLOAT   penSkipY = this->GetCharHeight();
//
//
//	while ( *lpchStr )
//	{
//		// get char code
//		WCHAR const charCode = * lpchStr ++;
//		WCHAR const charNextCode = * lpchStr;
//
//
//		// hinting and draw
//		FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;
//
//
//		if ( charGlyphHit( charCode )
//			&& charGlyphAdvance( charCode, charNextCode, glyphAdvX, glyphAdvY ) )
//		{
//			drawText( penX, penY, charCode );
//		}
//#ifdef _DEBUG
//		else
//		{
//			drawText( penX, penY, L'?' );
//			glyphAdvX = this->GetCharWidth();
//		}
//#endif
//
//
//		penX += glyphAdvX * this->m_fontScalingX;
//		penY += glyphAdvY * this->m_fontScalingY;
//
//	}
//
//}
//
void  KG3DFontBase::DrawText( FLOAT baseX, FLOAT baseY, LPCSTR lpchText, int cchText, FLOAT cchSpace )
{
	if ( ( NULL == lpchText )
		|| ( -1 >  cchText )
		|| ( 0  == cchText ) )
	{
		return ;
	}


	if ( !m_fontVertical )
	{
		m_fontSpaceX = cchSpace * m_fontScalingX;
		m_fontSpaceY = 0.0f;
		drawStandardText( baseX, baseY, lpchText, cchText, &KG3DFontBase::drawTextEx );
	}
	else
	{
		m_fontSpaceX = 0.0f;
		m_fontSpaceY = cchSpace * m_fontScalingY;
		drawVerticalText( baseX, baseY, lpchText, cchText, &KG3DFontBase::drawTextEx );
	}

}

void  KG3DFontBase::Draw3DText( FLOAT baseX, FLOAT baseY, LPCSTR lpchText, int cchText, FLOAT cchSpace )
{
	if ( ( NULL == lpchText )
		|| ( -1 >  cchText )
		|| ( 0  == cchText ) )
	{
		return ;
	}


	if ( !m_fontVertical )
	{
		m_fontSpaceX = cchSpace * m_fontScalingX;
		m_fontSpaceY = 0.0f;
		drawStandardText( baseX, baseY, lpchText, cchText, &KG3DFontBase::drawText );
	}
	else
	{
		m_fontSpaceX = 0.0f;
		m_fontSpaceY = cchSpace * m_fontScalingY;
		drawVerticalText( baseX, baseY, lpchText, cchText, &KG3DFontBase::drawText );
	}

}



////////////////////////////////////////////////
// get char width / height
BOOL  KG3DFontBase::GetTextExtent( LPCTSTR lpchText, int cchText, FLOAT &fWidth, FLOAT &fHeight, FLOAT cchSpace )
{
	// check valid
	if ( ( NULL == lpchText )
		|| ( -1 >  cchText )
		|| ( 0  == cchText ) )
	{
		return FALSE;
	}


	// draw text
	LPCWSTR lpchStr = charToUnicode( lpchText, cchText );
	FLOAT   penX = 0.0f, penY = 0.0f;
	FLOAT	spaceX = cchSpace * m_fontScalingX;
	FLOAT	spaceY = cchSpace * m_fontScalingY;


	while ( *lpchStr )
	{
		// get char code
		WCHAR const charCode = * lpchStr ++;
		WCHAR const charNextCode = * lpchStr;


		// hinting and draw
		FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;

		if ( charGlyphHit( charCode )
			&& charGlyphAdvance( charCode, charNextCode, glyphAdvX, glyphAdvY ) )
		{
			//KG3DFontGlyph_t* pFontGlyph = charToGlyphSlot( charCode );
			//ASSERT( NULL != pFontGlyph );

		}


		penX += glyphAdvX * this->m_fontScalingX + spaceX;
		penY += glyphAdvY * this->m_fontScalingY + spaceY;

	}


	fWidth = (  m_fontVertical ? this->GetCharWidth()  * this->m_fontScalingX: penX );
	fHeight = ( !m_fontVertical ? this->GetCharHeight() * this->m_fontScalingY: penY );

	return TRUE;
}

BOOL  KG3DFontBase::GetTextPosExtent( LPCTSTR lpchText, int cchText, FLOAT fCursorPos, LPINT lpCharPos, FLOAT cchSpace )
{
	// check valid
	if ( ( NULL == lpCharPos )
		|| ( NULL == lpchText )
		|| ( -1 >  cchText )
		|| ( 0  == cchText ) )
	{
		return FALSE;
	}


	// draw text
	LPCWSTR lpchStr = charToUnicode( lpchText, cchText );
	LPCTSTR lpchToken = lpchText;

	FLOAT   penX = 0.0f, penEndX = fCursorPos;
	FLOAT	scalingX = ( m_fontVertical ? m_fontScalingY : m_fontScalingX );
	FLOAT	spaceX = cchSpace * scalingX;

	for ( ; *lpchStr; ++ lpchStr, lpchToken = ::CharNext( lpchToken ) )
	{
		// get char code
		WCHAR const charCode = * lpchStr;
		WCHAR const charNextCode = * ( lpchStr + 1 );


		// hinting and draw
		FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;

		if ( charGlyphHit( charCode )
			&& charGlyphAdvance( charCode, charNextCode, glyphAdvX, glyphAdvY ) )
		{

		}


		penX += ( m_fontVertical ? glyphAdvY : glyphAdvX ) * scalingX + spaceX;
		//if ( penX >= penEndX )
		if ( penX > penEndX + 0.0001f)
		{
			break ;
		}


	}


	*lpCharPos = (INT)( lpchToken - lpchText );
	return TRUE;
}


////////////////////////////////////////////////
// init member
void  KG3DFontBase::initMember()
{
	// init member
	m_fontPaths[0]		= TEXT('\0');
	m_fontGlyphs		= 0;
	m_fontUploads		= 0;
	m_fontUnicode		= TRUE;
	m_fontKerningTable	= FALSE;

	m_fontFlagLoader	&= (~FT_LOAD_NO_HINTING);


	// clear glyph
	//memset( m_fontCharVector, 0, sizeof( m_fontCharVector ) );
	//memset( m_fontCharMap, 0, sizeof( m_fontCharMap ) );
	//for each ( KG3DFontGlyph_t* iter in  m_fontCharVector )
	KG3DFontGlyph_t* iter = NULL;
	for ( int i = 0; i < KG3DFONT_GLYPH_LIMIT; ++ i )
	{
		iter = m_fontCharVector[i];
		if ( NULL != iter )
		{
			delete iter;
		}
	}

	//memset( m_fontCharMap, 0, sizeof( m_fontCharMap ) );
	memset( m_fontCharWide, 0, sizeof( m_fontCharWide ) );
	memset( m_fontCharVector, 0, sizeof( m_fontCharVector ) );
	//m_fontCharMap.clear();
	//m_fontCharVector.clear();

}

void  KG3DFontBase::initSizeMember()
{
	m_fontPoint			= KG3DFONT_DEFAULT_POINT;
	m_fontDPI			= KG3DFONT_DEFAULT_DPI;

	m_fontAscender		= 0.0f;
	m_fontDescender		= 0.0f;

	m_fontColour		= 0xFFFFFFFF;
	m_fontScalingX		= 1.0f;
	m_fontScalingY		= 1.0f;

}



////////////////////////////////////////////////
// draw text
// 三维文本，二维文本
void  KG3DFontBase::drawNormalText( FLOAT baseX, FLOAT baseY, LPCWSTR lpchText, FN_DRAWGLYPH fnDrawText )
{
	// draw text
	FLOAT penX = baseX, penY = baseY;


	while ( *lpchText )
	{
		// get char code
		WCHAR const charCode = * lpchText ++;
		WCHAR const charNextCode = * lpchText;


		// hinting and draw
		FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;


		if ( charGlyphHit( charCode )
			&& charGlyphAdvance( charCode, charNextCode, glyphAdvX, glyphAdvY ) )
		{
			(this->*fnDrawText)( penX, penY, charCode );
		}
#ifdef _DEBUG
		else
		{
			charGlyphHit( KG3DFACE_ERROR );
			(this->*fnDrawText)( penX, penY, KG3DFACE_ERROR );
			glyphAdvX = this->GetCharWidth();
		}
#endif


		penX += glyphAdvX * this->m_fontScalingX + m_fontSpaceX;
		penY += glyphAdvY * this->m_fontScalingY + m_fontSpaceY;

	}

}

void  KG3DFontBase::drawStandardText( FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText )
{
	// draw text
	LPCWSTR lpchStr = charToUnicode( lpchText, cchText );
	D3DCOLOR fontColour = this->GetTextColour();
	BYTE	 fontAlpha  = this->GetTextAlpha();


	if ( m_fontEffect & KG3DFONT_STYLE_PROJECTION )
	{
		//this->SetTextAlpha( 0x64 );
		this->SetTextColour( m_fontProjectionCr );

		drawNormalText( baseX + ( m_fontProjectionWeight << 1 ), baseY + m_fontProjectionWeight, lpchStr, fnDrawText );

	}
	if ( m_fontEffect & KG3DFONT_STYLE_BORDER )
	{
		//this->SetTextAlpha( 0xFF );
		this->SetTextColour( m_fontBorderCr );

		if ( 1 == m_fontBorderWeight )
		{
			for ( int y = -m_fontBorderWeight; y <= m_fontBorderWeight; ++ y )
			{
				for ( int x = -m_fontBorderWeight; x <= m_fontBorderWeight; ++ x )
				{
					if ( ( 0 == x ) && ( 0 == y ) )
					{
						continue ;
					}
					
					drawNormalText( baseX + x, baseY + y, lpchStr, fnDrawText );
				}
			}

		}
		else
		{
			int xBegin	= -m_fontBorderWeight + 1;
			int xEnd	= m_fontBorderWeight - 1;

			for ( int x = xBegin; x <= xEnd; ++ x )
			{
				drawNormalText( baseX + x, baseY - m_fontBorderWeight, lpchStr, fnDrawText );
			}

			for ( int x = xBegin; x <= xEnd; ++ x )
			{
				drawNormalText( baseX + x, baseY + m_fontBorderWeight, lpchStr, fnDrawText );
			}



			int yBegin	= -m_fontBorderWeight + 1;
			int yEnd	= m_fontBorderWeight - 1;

			for ( int y = yBegin; y <= yEnd; ++ y )
			{
				for ( int x = -m_fontBorderWeight; x <= m_fontBorderWeight; ++ x )
				{
					if ( ( 0 == x ) && ( 0 == y ) )
					{
						continue ;
					}


					drawNormalText( baseX + x, baseY + y, lpchStr, fnDrawText );
				}

			}


		}

	}
	


	this->SetTextAlpha( fontAlpha );
	this->SetTextColour( fontColour );

	drawNormalText( baseX, baseY, lpchStr, fnDrawText );

}

void  KG3DFontBase::drawVerticalText( FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText )
{
	drawStandardText( baseX, baseY, lpchText, cchText, fnDrawText );
}



////////////////////////////////////////////////
// char member
LPCWSTR KG3DFontBase::charToUnicode( LPCSTR lpchText, int cchText )
{
	INT wideChars = 0;
		
	m_fontCharWide[wideChars] = L'\0';
	wideChars = ::MultiByteToWideChar( CP_ACP,MB_PRECOMPOSED, lpchText, cchText, m_fontCharWide, _countof( m_fontCharWide ) );
	m_fontCharWide[wideChars] = L'\0';


	return ( wideChars == 0 ) ? NULL : m_fontCharWide;
}


LPCWSTR	KG3DFontBase::charToUnicode( INT& nUText, LPCTSTR lpchText, INT cchText )
{
	nUText = 0;
	m_fontCharWide[nUText] = L'\0';
	nUText = ::MultiByteToWideChar( CP_ACP,MB_PRECOMPOSED, lpchText, cchText, m_fontCharWide, _countof( m_fontCharWide ) );
	m_fontCharWide[nUText] = L'\0';


	return ( nUText == 0 ) ? NULL : m_fontCharWide;

	//LPCWSTR lpStr = charToUnicode( lpchText, cchText );
	//LPINT   lpGlyph = m_fontCharMap;

	//for ( nUText = 0; *lpStr; ++ nUText )
	//{
	//	* lpGlyph ++ = charToGlyphIndex( * lpStr ++ );
	//}


	//* lpGlyph = 0;
	//return ( nUText == 0 ) ? NULL : m_fontCharMap;
}



KG3DFontGlyph_t* KG3DFontBase::charToGlyphSlot( UINT charCode )
{
	/*INT index = charToIndex( charCode );
	if ( 0 < index )*/
	{
		return m_fontCharVector[ charCode ];
	}


	return NULL;
}

INT   KG3DFontBase::charToGlyphIndex( UINT charCode )
{
	return FT_Get_Char_Index( KG3DFACE_POINTER, charCode );
}

INT   KG3DFontBase::charToIndex( UINT charCode )
{
	//FontMap_t::iterator iter = m_fontCharMap.find( charCode );

	//if ( iter != m_fontCharMap.end() )
	//{
	//	return iter->second;
	//}
	if ( NULL != m_fontCharVector[charCode] )
	{
		return charCode;
	}


	return 0;
}


////////////////////////////////////////////////
// glyph font member
bool  KG3DFontBase::charGlyphHit( UINT charCode )
{
	// char hit ?
	INT const index = charToIndex( charCode );
	if ( 0 == index )
	{
		// get glyph index
		INT glyphIndex = FT_Get_Char_Index( KG3DFACE_POINTER, charCode );
		if ( 0 == glyphIndex ) // not undefined character code
		{
			return false;
		}


		// glyph load
		FT_Error flagErr = FT_Load_Glyph( KG3DFACE_POINTER, glyphIndex, m_fontFlagLoader );

		if ( 0 < flagErr )
		{
			return false;
		}


		// glyph mark
		KG3DFontGlyph_t* fontGlyph = NULL;
		FT_GlyphSlot	ftGlyph = KG3DFACE_POINTER->glyph;


		if ( !charGlyphMake( &fontGlyph ) )
		{
			return false;
		}


		kg3dGlyphInfill( m_fontVertical, charCode, glyphIndex, fontGlyph, ftGlyph );
		if ( !charGlyphInfill( charCode, ftGlyph, fontGlyph ) )
		{
			delete fontGlyph;
			return false;
		}


		m_fontCharVector[ charCode ] = fontGlyph;
		//m_fontCharVector.push_back( fontGlyph );
		//m_fontCharMap.insert( FontMap_t::value_type( charCode, UINT( m_fontCharVector.size() ) ) );

	}


	return true;
}

bool  KG3DFontBase::charGlyphAdvance( UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY )
{
	// adjust pen
	FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;

	if ( charGlyphAdvance( chText, glyphAdvX, glyphAdvY ) )
	{
		FLOAT glyphKernX = 0.0f, glyphKernY = 0.0f;
		UINT  glyphLeft = charToGlyphIndex( chText );
		UINT  glyphRight = charToGlyphIndex( chNext );

		if ( charGlyphKernAdvance( chText, chNext, glyphKernX, glyphKernY ) )
		{
			advX = glyphKernX + glyphAdvX;
			advY = glyphKernY + glyphAdvY;
		}
		else
		{
			advX = glyphAdvX;
			advY = glyphAdvY;
		}

	}


	return true;
}



bool  KG3DFontBase::charGlyphAdvance( UINT chText, FLOAT& advX, FLOAT& advY )
{
	KG3DFontGlyph_t* glyphSlot = charToGlyphSlot( chText );

	if ( NULL != glyphSlot )
	{
		advX = glyphSlot->glyphAdvance[ KG3DFONT_SLOT_X ];
		advY = glyphSlot->glyphAdvance[ KG3DFONT_SLOT_Y ];
	}
	else
	{
		advX = 0.0f;
		advY = 0.0f;

	}


	return ( NULL != glyphSlot );
}


bool  KG3DFontBase::charGlyphKernAdvance( UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY )
{
	FT_Error err = 0;

	if ( m_fontKerningTable && chText && chNext )
	{
		FT_Vector kernAdvance = { 0, 0 } ;

		err = FT_Get_Kerning( KG3DFACE_POINTER, chText, chNext, FT_KERNING_UNFITTED, &kernAdvance );
		if ( 0 == err )
		{   
			advX = KG3DFONT_FIXDIV( static_cast<FLOAT>( kernAdvance.x) );
			advY = KG3DFONT_FIXDIV( static_cast<FLOAT>( kernAdvance.y) );
		}

	}
	else
	{
		advX = 0.0f;
		advY = 0.0f;

	}


	return ( 0 == err );
}



////////////////////////////////////////////////
// get member
INT   KG3DFontBase::getGlyphUsage() const
{
	return ( m_fontGlyphs - m_fontUploads );//INT( m_fontCharVector.size() );
}

INT   KG3DFontBase::getGlyphBudget() const
{
	return ( m_fontGlyphs - m_fontUploads );//INT( m_fontCharVector.size() );
}

