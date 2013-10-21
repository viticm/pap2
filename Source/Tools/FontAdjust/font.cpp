#include "stdafx.h"
#include "./font.h"
#include "./fontrenderer.h"

extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern FontRenderer* g_pFontRenderer;

namespace
{
	class KG3DFTFontLibrary
	{
	private:
		static std::auto_ptr<KG3DFTFontLibrary> m_pImpl;

	public:
		~KG3DFTFontLibrary();
		
		static KG3DFTFontLibrary* Instance();

	private:
		KG3DFTFontLibrary();
		KG3DFTFontLibrary(KG3DFTFontLibrary const& rhs);
		KG3DFTFontLibrary& operator=(KG3DFTFontLibrary const& rhs);

	public:
		FT_Library	m_library;
	};

	std::auto_ptr<KG3DFTFontLibrary>  KG3DFTFontLibrary::m_pImpl;


	KG3DFTFontLibrary::KG3DFTFontLibrary()
	{
	}


	KG3DFTFontLibrary::~KG3DFTFontLibrary()
	{
		if (0 != m_library)
		{
			FT_Done_FreeType(m_library);
			m_library = 0;
		}
	}

	KG3DFTFontLibrary* KG3DFTFontLibrary::Instance()
	{
		if (m_pImpl.get() == NULL)
		{
			KG3DFTFontLibrary* pTemp = new KG3DFTFontLibrary;
			if (pTemp)
			{
				m_pImpl.reset(pTemp);
				if (FT_Init_FreeType(&m_pImpl->m_library))
				{
					m_pImpl.reset();
				}
			}
		}

		return m_pImpl.get();
	}


	inline float FONT_FIXDIV(float X) { return X / 64.0f; }
	inline float FONT_FIXMULT(float X) { return X * 64.0f; }

	inline int FONT_CEIL(int X)	{ return ((X + 63) & -64) / 64; }
	inline int FONT_FLOOR(int X) { return (X & -64) / 64; }

	void GlyphInfill(BOOL ftVertical, UINT ftCode, INT ftIndex, KG3DFontGlyph_t* fontGlyph, FT_GlyphSlot ftGlyph)
	{
		fontGlyph->glyphIndex  = ftIndex;

		fontGlyph->glyphAdvance[ KG3DFONT_SLOT_X ] = FONT_FIXDIV(ftGlyph->advance.x);
		fontGlyph->glyphAdvance[ KG3DFONT_SLOT_Y ] = FONT_FIXDIV(ftGlyph->advance.y);

		FT_BBox bbox;

		FT_Outline_Get_CBox(&(ftGlyph->outline), &bbox);

		fontGlyph->glyphBLower[ KG3DFONT_SLOT_X ] = FONT_FIXDIV(bbox.xMin);
		fontGlyph->glyphBLower[ KG3DFONT_SLOT_Y ] = FONT_FIXDIV(bbox.yMin);

		fontGlyph->glyphBUpper[ KG3DFONT_SLOT_X ] = FONT_FIXDIV(bbox.xMax);
		fontGlyph->glyphBUpper[ KG3DFONT_SLOT_Y ] = FONT_FIXDIV(bbox.yMax);
	}
}


Font::Font()
		:	m_fontFace(NULL),
			m_fontGlyphs(0),
			m_fontUploads(0),
			m_fontEncoding(FT_ENCODING_NONE),
			m_fontUnicode(FALSE),
			m_fontVertical(FALSE),
			m_fontKerningTable(FALSE),
			m_fontFlagDraw(FT_RENDER_MODE_NORMAL),
			m_fontFlagLoader(FT_LOAD_NO_HINTING),
			m_fontPoint(0),
			m_fontDPI(0),
			m_fontColour(0xFFFFFFFF),
			m_fontScalingX(1.0f),
			m_fontScalingY(1.0f),
			m_fontEffect(KG3DFONT_STYLE_NONE)
{
	memset(m_fontCharVector, 0, sizeof(m_fontCharVector));

	m_fontFlagDraw = FT_RENDER_MODE_NORMAL;
	m_fontFlagLoader = FT_LOAD_NO_HINTING;

	m_glyphOffX = 0.0f;
	m_glyphOffY = 0.0f;

	m_glyphTextureLast = 0;

	m_glyphTextureWidth = 0;
	m_glyphTextureHeight = 0;

	memset(m_glyphVertex, 0, sizeof(m_glyphVertex));
	memset(m_glyphTLVertex, 0, sizeof(m_glyphTLVertex));
}


Font::~Font()
{
	delete m_fontFace;
}


BOOL Font::Initialize(LPCTSTR fontName, INT fontSize, UINT fontStyle, UINT fontDpi)
{
	if (KG3DFTFontLibrary::Instance() == NULL)
		return false;

	FT_Long const DEFAULT_FACE_INDEX = 0;
	FT_Face pFontFace = NULL;
	FT_Error err = 0;

	err = FT_New_Face(KG3DFTFontLibrary::Instance()->m_library, fontName, DEFAULT_FACE_INDEX, &pFontFace);
	if (err != 0)
	{
		if (err == FT_Err_Unknown_File_Format)
			_ASSERT(0 && "FT_Err_Unknown_File_Format == err");

		m_fontFace = NULL;
		return FALSE;
	}
	else
	{
		m_fontGlyphs = pFontFace->num_glyphs;
		m_fontUploads = pFontFace->num_glyphs;
		m_fontKerningTable = FT_HAS_KERNING(pFontFace);
	}


	// get face caps
	//if (!FT_HAS_VERTICAL(pFontFace))
	if (!FT_IS_SCALABLE(pFontFace))
	{
		_ASSERT(0 && "Sorry, font is not scalable.");
		return FALSE;
	}


	// get face encoding
	//m_fontFlagDraw = !fontAntialias ? FT_RENDER_MODE_MONO : FT_RENDER_MODE_NORMAL;
	m_fontEncoding = FT_ENCODING_UNICODE;
	m_fontUnicode  = FALSE;


	if (NULL != pFontFace->charmap)
	{
		for (int i = 0; i < pFontFace->num_charmaps; ++ i)
		{
			if (FT_Encoding(m_fontEncoding) == pFontFace->charmaps[i]->encoding)
			{
				m_fontUnicode = TRUE;
				break ;
			}

		}

		if (!m_fontUnicode)
		{
			 m_fontEncoding = pFontFace->charmaps[0]->encoding;
			 m_fontUnicode  = (m_fontEncoding == FT_ENCODING_UNICODE);
		}

		err = FT_Select_Charmap(pFontFace, FT_Encoding(m_fontEncoding));
		if (0 != err)
		{
			_ASSERT(0 && "FT_Select_Charmap");
			CleanUp();
			return FALSE;
		}
	}
	else
	{
		m_fontEncoding = pFontFace->charmap->encoding;
		m_fontUnicode  = (m_fontEncoding == FT_ENCODING_UNICODE);
	}
		

	// set char size
	err = FT_Set_Char_Size(pFontFace, 0L, FONT_FIXMULT(fontSize), fontDpi, fontDpi);
	if (0 != err)
	{
		// tmd;
		_ASSERT(0 && "FT_Set_Char_Size");
		CleanUp();
		return FALSE;
	}

	m_fontFace = pFontFace;
	m_fontPoint = fontSize;
	m_fontDPI = fontDpi;

	m_fontEffect = (fontStyle & (~KG3DFONT_STYLE_VERTICAL));
	m_fontVertical = fontStyle & KG3DFONT_STYLE_VERTICAL;
	m_fontFlagLoader = m_fontVertical ? m_fontFlagLoader | FT_LOAD_VERTICAL_LAYOUT : m_fontFlagLoader;

	return TRUE;
}


FLOAT Font::GetCharWidth() const
{ 
	FT_Size pFontPSize = m_fontFace->size;

	if (FT_IS_SCALABLE(m_fontFace))
	{
		return(m_fontFace->bbox.xMax - m_fontFace->bbox.xMin) *
			((FLOAT)(pFontPSize->metrics.x_ppem) / (FLOAT)(m_fontFace->units_per_EM));
	}
	else
	{
		return FONT_FIXDIV(pFontPSize->metrics.max_advance);
	}
}

FLOAT Font::GetCharHeight() const
{
	FT_Size pFontPSize = m_fontFace->size;

	if (FT_IS_SCALABLE(m_fontFace))
	{
		return (m_fontFace->bbox.yMax - m_fontFace->bbox.yMin) *
			((FLOAT)pFontPSize->metrics.y_ppem / (FLOAT)m_fontFace->units_per_EM);
	}
	else
	{
		return FONT_FIXDIV(pFontPSize->metrics.height);
	}
}

FLOAT Font::GetCharAscent() const
{
	return FONT_FIXDIV(m_fontFace->size->metrics.ascender);
}

FLOAT Font::GetCharDescent() const
{
	return FONT_FIXDIV(m_fontFace->size->metrics.descender);
}


BOOL  Font::CleanUp()
{
	// glyph destroy
	for (size_t i = 0; i < m_glyphTextureVector.size(); ++ i)
	{
		g_pFontRenderer->DeleteTexture(m_glyphTextureVector.at(i));
	}

	m_glyphTextureLast = 0;
	m_glyphTextureVector.clear();


	// reset font face
	if (NULL != m_fontFace)
	{
		FT_Done_Face(m_fontFace);
		m_fontFace= NULL;
	}


	// init member
	m_fontGlyphs = 0;
	m_fontUploads = 0;
	m_fontUnicode = TRUE;
	m_fontKerningTable = FALSE;

	m_fontFlagLoader &= (~FT_LOAD_NO_HINTING);

	for (int i = 0; i < KG3DFONT_GLYPH_LIMIT; ++i)
	{
		KG3DFontGlyph_t* iter = m_fontCharVector[i];
		if (NULL != iter)
		{
			delete iter;
		}
	}

	memset(m_fontCharVector, 0, sizeof(m_fontCharVector));


	m_fontPoint			= KG3DFONT_DEFAULT_POINT;
	m_fontDPI			= KG3DFONT_DEFAULT_DPI;

	m_fontColour		= 0xFFFFFFFF;
	m_fontScalingX		= 1.0f;
	m_fontScalingY		= 1.0f;


	return TRUE;
}



////////////////////////////////////////////////
// preload text
UINT  Font::PreloadRange(UCHAR First /*= 0x20*/, UCHAR Last /*= 0x7F*/)
{
	// check range
	UINT nPreloadText = 0;


	if ((First >= 0x20)
		&& (Last <= 0x7F))
	{
		// generating range
		UCHAR  charRange = Last - First + 1;
		CHAR   charGlyph[ 0x80 - 0x20 ] = {0};

		std::fill(charGlyph, charGlyph + charRange, First);
		for (int i = 0; i < charRange; ++ i)
		{
			charGlyph[i] += static_cast<CHAR>(i);
		}


		// commit text
		nPreloadText = PreloadText(charGlyph, charRange);

	}


	return nPreloadText;
}

UINT  Font::PreloadText(LPCSTR lpchText, int cchText)
{
	// check valid
	if ((NULL == lpchText)
		|| (-1 >  cchText)
		|| (0  == cchText))
	{
		return 0;
	}


	// draw text
	LPCWSTR lpchStr = charToUnicode(lpchText, cchText);
	UINT    nPreloadText = 0;

	while (*lpchStr)
	{
		// get char code
		if (charGlyphHit(* lpchStr ++))
		{
			++ nPreloadText;
		}

	}


	return nPreloadText;
}


void  Font::DrawText(FLOAT baseX, FLOAT baseY, LPCSTR lpchText, int cchText, FLOAT cchSpace)
{
	if ((NULL == lpchText)
		|| (-1 >  cchText)
		|| (0  == cchText))
	{
		return ;
	}


	if (!m_fontVertical)
	{
		m_fontSpaceX = cchSpace * m_fontScalingX;
		m_fontSpaceY = 0.0f;
		drawStandardText(baseX, baseY, lpchText, cchText, &Font::DrawCharEx);
	}
	else
	{
		m_fontSpaceX = 0.0f;
		m_fontSpaceY = cchSpace * m_fontScalingY;
		drawVerticalText(baseX, baseY, lpchText, cchText, &Font::DrawCharEx);
	}

}

void  Font::Draw3DText(FLOAT baseX, FLOAT baseY, LPCSTR lpchText, int cchText, FLOAT cchSpace)
{
	if ((NULL == lpchText)
		|| (-1 >  cchText)
		|| (0  == cchText))
	{
		return ;
	}


	if (!m_fontVertical)
	{
		m_fontSpaceX = cchSpace * m_fontScalingX;
		m_fontSpaceY = 0.0f;
		drawStandardText(baseX, baseY, lpchText, cchText, &Font::DrawChar);
	}
	else
	{
		m_fontSpaceX = 0.0f;
		m_fontSpaceY = cchSpace * m_fontScalingY;
		drawVerticalText(baseX, baseY, lpchText, cchText, &Font::DrawChar);
	}

}



////////////////////////////////////////////////
// get char width / height
BOOL  Font::GetTextExtent(LPCTSTR lpchText, int cchText, FLOAT &fWidth, FLOAT &fHeight, FLOAT cchSpace)
{
	// check valid
	if ((NULL == lpchText)
		|| (-1 >  cchText)
		|| (0  == cchText))
	{
		return FALSE;
	}


	// draw text
	LPCWSTR lpchStr = charToUnicode(lpchText, cchText);
	FLOAT   penX = 0.0f, penY = 0.0f;
	FLOAT	spaceX = cchSpace * m_fontScalingX;
	FLOAT	spaceY = cchSpace * m_fontScalingY;


	while (*lpchStr)
	{
		// get char code
		WCHAR const charCode = * lpchStr ++;
		WCHAR const charNextCode = * lpchStr;


		// hinting and draw
		FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;

		if (charGlyphHit(charCode) && charGlyphAdvance(charCode, charNextCode, glyphAdvX, glyphAdvY))
		{
		}


		penX += glyphAdvX * m_fontScalingX + spaceX;
		penY += glyphAdvY * m_fontScalingY + spaceY;
	}


	fWidth = ( m_fontVertical ? GetCharWidth()  * m_fontScalingX: penX);
	fHeight = (!m_fontVertical ? GetCharHeight() * m_fontScalingY: penY);

	return TRUE;
}

BOOL Font::GetTextPosExtent(LPCTSTR lpchText, int cchText, FLOAT fCursorPos, LPINT lpCharPos, FLOAT cchSpace)
{
	// check valid
	if (lpCharPos == NULL || lpchText == NULL || cchText < -1 || cchText == 0)
		return FALSE;

	// draw text
	LPCWSTR lpchStr = charToUnicode(lpchText, cchText);
	LPCTSTR lpchToken = lpchText;

	FLOAT penX = 0.0f, penEndX = fCursorPos;
	FLOAT scalingX = (m_fontVertical ? m_fontScalingY : m_fontScalingX);
	FLOAT spaceX = cchSpace * scalingX;

	for (; *lpchStr; ++ lpchStr, lpchToken = ::CharNext(lpchToken))
	{
		WCHAR const charCode = *lpchStr;
		WCHAR const charNextCode = * (lpchStr + 1);

		// hinting and draw
		FLOAT glyphAdvX = 0.0f, glyphAdvY = 0.0f;

		if (charGlyphHit(charCode) && charGlyphAdvance(charCode, charNextCode, glyphAdvX, glyphAdvY))
		{
		}


		penX += (m_fontVertical ? glyphAdvY : glyphAdvX) * scalingX + spaceX;

		if (penX > penEndX + 0.0001f)
		{
			break ;
		}
	}


	*lpCharPos = (INT)(lpchToken - lpchText);
	return TRUE;
}


////////////////////////////////////////////////
// draw text
// 三维文本，二维文本
void  Font::drawNormalText(FLOAT baseX, FLOAT baseY, LPCWSTR lpchText, FN_DRAWGLYPH fnDrawText)
{
	FLOAT penX = baseX;
	FLOAT penY = baseY;

	_ASSERT(lpchText);

	while (*lpchText)
	{
		// get char code
		WCHAR const charCode = * lpchText ++;
		WCHAR const charNextCode = * lpchText;

		FLOAT glyphAdvX = 0.0f;
		FLOAT glyphAdvY = 0.0f;

		if (charGlyphHit(charCode) && charGlyphAdvance(charCode, charNextCode, glyphAdvX, glyphAdvY))
		{
			(this->*fnDrawText)(penX, penY, charCode);
		}
		else
		{
			charGlyphHit(L'?');
			(this->*fnDrawText)(penX, penY, L'?');
			glyphAdvX = this->GetCharWidth();
		}

		penX += glyphAdvX * this->m_fontScalingX + m_fontSpaceX;
		penY += glyphAdvY * this->m_fontScalingY + m_fontSpaceY;
	}
}

void  Font::drawStandardText(FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText)
{
	LPCWSTR lpchStr = charToUnicode(lpchText, cchText);
	D3DCOLOR fontColour = GetTextColour();
	BYTE fontAlpha  = GetTextAlpha();

	if (m_fontEffect & KG3DFONT_STYLE_PROJECTION)
	{
		SetTextColor(m_fontProjectionColor);

		drawNormalText(baseX + (m_fontProjectionWeight << 1), baseY + m_fontProjectionWeight, lpchStr, fnDrawText);
	}
	if (m_fontEffect & KG3DFONT_STYLE_BORDER)
	{
		SetTextColor(m_fontBorderColor);

		if (1 == m_fontBorderWeight)
		{
			for (int y = -m_fontBorderWeight; y <= m_fontBorderWeight; ++ y)
			{
				for (int x = -m_fontBorderWeight; x <= m_fontBorderWeight; ++ x)
				{
					if ((0 == x) && (0 == y))
					{
						continue ;
					}
					
					drawNormalText(baseX + x, baseY + y, lpchStr, fnDrawText);
				}
			}
		}
		else
		{
			int xBegin = -m_fontBorderWeight + 1;
			int xEnd = m_fontBorderWeight - 1;

			for (int x = xBegin; x <= xEnd; ++ x)
				drawNormalText(baseX + x, baseY - m_fontBorderWeight, lpchStr, fnDrawText);

			for (int x = xBegin; x <= xEnd; ++ x)
				drawNormalText(baseX + x, baseY + m_fontBorderWeight, lpchStr, fnDrawText);

			int yBegin	= -m_fontBorderWeight + 1;
			int yEnd = m_fontBorderWeight - 1;

			for (int y = yBegin; y <= yEnd; ++ y)
			{
				for (int x = -m_fontBorderWeight; x <= m_fontBorderWeight; ++ x)
				{
					if ((0 == x) && (0 == y))
					{
						continue ;
					}

					drawNormalText(baseX + x, baseY + y, lpchStr, fnDrawText);
				}
			}
		}
	}
	
	SetTextAlpha(fontAlpha);
	SetTextColor(fontColour);

	drawNormalText(baseX, baseY, lpchStr, fnDrawText);
}

void  Font::drawVerticalText(FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText)
{
	drawStandardText(baseX, baseY, lpchText, cchText, fnDrawText);
}

LPCWSTR Font::charToUnicode(LPCSTR lpchText, int cchText)
{
	static WCHAR wcszBuffer[KG3DFONT_GLYPH_NUMS];

	INT nWideChar = 0;
		
	wcszBuffer[0] = L'\0';
	nWideChar = ::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, lpchText, cchText, wcszBuffer, _countof(wcszBuffer));
	wcszBuffer[nWideChar] = L'\0';

	return nWideChar == 0 ? NULL : wcszBuffer;
}


KG3DFontGlyph_t* Font::charToGlyphSlot(UINT charCode)
{
	return m_fontCharVector[charCode];
}

INT Font::charToGlyphIndex(UINT charCode)
{
	return FT_Get_Char_Index(m_fontFace, charCode);
}

INT Font::charToIndex(UINT charCode)
{
	if (NULL != m_fontCharVector[charCode])
		return charCode;

	return 0;
}


bool Font::charGlyphHit(UINT charCode)
{
	INT const index = charToIndex(charCode);
	if (0 != index)
		return true;

	INT glyphIndex = FT_Get_Char_Index(m_fontFace, charCode);
	if (glyphIndex == 0)
		return false;

	FT_Error flagErr = FT_Load_Glyph(m_fontFace, glyphIndex, m_fontFlagLoader);

	if (flagErr > 0)
		return false;

	KG3DFontGlyph_t* pFontGlyph = NULL;
	FT_GlyphSlot ftGlyph = m_fontFace->glyph;

	pFontGlyph = new(std::nothrow) KG3DGlyphItem_t;
	if (pFontGlyph == NULL)
		return false;

	GlyphInfill(m_fontVertical, charCode, glyphIndex, pFontGlyph, ftGlyph);
	if (!charGlyphInfill(charCode, ftGlyph, pFontGlyph))
	{
		delete pFontGlyph;
		return false;
	}

	m_fontCharVector[charCode] = pFontGlyph;
	return true;
}

bool Font::charGlyphAdvance(UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY)
{
	FLOAT glyphAdvX = 0.0f;
	FLOAT glyphAdvY = 0.0f;

	if (!charGlyphAdvance(chText, glyphAdvX, glyphAdvY))
		return false;

	FLOAT glyphKernX = 0.0f, glyphKernY = 0.0f;
	UINT  glyphLeft = charToGlyphIndex(chText);
	UINT  glyphRight = charToGlyphIndex(chNext);

	if (charGlyphKernAdvance(chText, chNext, glyphKernX, glyphKernY))
	{
		advX = glyphKernX + glyphAdvX;
		advY = glyphKernY + glyphAdvY;
	}
	else
	{
		advX = glyphAdvX;
		advY = glyphAdvY;
	}

	return true;
}


bool Font::charGlyphAdvance(UINT chText, FLOAT& advX, FLOAT& advY)
{
	KG3DFontGlyph_t* pGlyphSlot = charToGlyphSlot(chText);

	if (pGlyphSlot)
	{
		advX = pGlyphSlot->glyphAdvance[ KG3DFONT_SLOT_X ];
		advY = pGlyphSlot->glyphAdvance[ KG3DFONT_SLOT_Y ];
	}
	else
	{
		advX = 0.0f;
		advY = 0.0f;
	}

	return pGlyphSlot != NULL;
}


bool  Font::charGlyphKernAdvance(UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY)
{
	FT_Error err = 0;

	if (m_fontKerningTable && chText && chNext)
	{
		FT_Vector kernAdvance = { 0, 0 } ;

		err = FT_Get_Kerning(m_fontFace, chText, chNext, FT_KERNING_UNFITTED, &kernAdvance);
		if (0 == err)
		{   
			advX = FONT_FIXDIV(static_cast<FLOAT>(kernAdvance.x));
			advY = FONT_FIXDIV(static_cast<FLOAT>(kernAdvance.y));
		}
	}
	else
	{
		advX = 0.0f;
		advY = 0.0f;
	}

	return (0 == err);
}


INT Font::getGlyphUsage() const
{
	return m_fontGlyphs - m_fontUploads;
}

INT Font::getGlyphBudget() const
{
	return m_fontGlyphs - m_fontUploads;
}


// wrapper vertex
#define  FILL_VERTEX(vertex, nx, ny, ncolor, nu, nv)	\
	vertex.x = nx;			\
	vertex.y = ny;			\
	vertex.z = 1.0f;		\
	vertex.color = ncolor;	\
	vertex.u = nu;			\
	vertex.v = nv;


#define  FILL_TLVERTEX(vertex, nx, ny, ncolor, nu, nv)	\
	vertex.x = nx;			\
	vertex.y = ny;			\
	vertex.z = 0.0f;		\
	vertex.w = 1.0f;		\
	vertex.color = ncolor;	\
	vertex.u = nu;			\
	vertex.v = nv;



void  Font::DrawChar(FLOAT penX, FLOAT penY, WCHAR chText)
{
	KG3DGlyphItem_t* pFontGlyph = (KG3DGlyphItem_t*)charToGlyphSlot(chText);
	if (pFontGlyph)
	{
		FLOAT const fontScalingW = pFontGlyph->uploadWidth * m_fontScalingX;
		FLOAT const fontScalingH = pFontGlyph->uploadHeight * m_fontScalingY;

		penX += pFontGlyph->xOffset * m_fontScalingX;
		penY += pFontGlyph->yOffset * m_fontScalingY;

		if ((m_fontVertical) && (((0x4E00 > chText) || (chText > 0x9FFF))))
		{
			FILL_VERTEX(m_glyphVertex[0], penX, penY, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[1][1]);
			FILL_VERTEX(m_glyphVertex[1], penX + fontScalingH, penY, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[0][1]);
			FILL_VERTEX(m_glyphVertex[2], penX + fontScalingH, penY + fontScalingW, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[0][1]);
			FILL_VERTEX(m_glyphVertex[3], penX, penY + fontScalingW, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[1][1]);
		}
		else
		{
			FILL_VERTEX(m_glyphVertex[0], penX, penY, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[0][1]);
			FILL_VERTEX(m_glyphVertex[1], penX + fontScalingW, penY, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[0][1]);
			FILL_VERTEX(m_glyphVertex[2], penX + fontScalingW, penY + fontScalingH, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[1][1]);
			FILL_VERTEX(m_glyphVertex[3], penX, penY + fontScalingH, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[1][1]);
		}

		g_pFontRenderer->PrepareVeretex(pFontGlyph->textureId, m_glyphVertex, _countof(m_glyphVertex));
	}
}

void  Font::DrawCharEx(FLOAT penX, FLOAT penY, WCHAR chText)
{
	KG3DGlyphItem_t* pFontGlyph = (KG3DGlyphItem_t*)charToGlyphSlot(chText);
	if (pFontGlyph)
	{
		FLOAT const fontScalingW =  pFontGlyph->uploadWidth * m_fontScalingX;
		FLOAT const fontScalingH =  pFontGlyph->uploadHeight * m_fontScalingY;

		penX += pFontGlyph->xOffset * m_fontScalingX;
		penY += pFontGlyph->yOffset * m_fontScalingY;
		//  ISO10646所有字面中，目前僅有第0、第1和第2字面真正收容
		//	編碼字元。ISO10646的BMP和Unicode的編碼字元，依其UCS-2編
		//	碼序介紹如下：

		//	(1)0000～007Fh：基本拉丁字母區。其中0000～001Fh為C0控
		//	制碼，0020h為空格（space），0021～007Eh為ASCII圖形
		//	字元，007Fh為控制碼DEL。事實上，這128個字元碼只要
		//	把前8個位元去掉就可變成習見的8位元形式的ASCII碼。

		//	(2)0080～00A0h：控制碼區。其中0080～009Fh為C1控制碼，
		//	00A0h為不中斷空格（no-break space）。

		//	(3)00A1～1FFFh：拼音文字區。收容除基本拉丁字母以外的各種
		//	拼音文字字元，包括歐洲各國語言、希臘文、斯拉夫語文、
		//	希伯來文、阿拉伯文、亞美尼亞文、印度各地方言、馬來文、
		//	泰文、寮文、柬普寨文、滿文、蒙文、藏文、印地安語文等。

		//	(4)2000～28FFh：符號區。收容各種符號，包括標點符號、上下
		//	標、錢幣符號、數字、箭頭、數學符號、工程符號、光學辨
		//	識符號、帶圈或帶括符的文數字、表格繪製符號、地理圖示、
		//	盲用點字、裝飾圖形等。

		//	(5)2E80～33FFh：中日韓符號區。收容康熙字典部首、中日韓輔
		//	助部首、注音符號、日本假名、韓文音符，中日韓的符號、
		//	標點、帶圈或帶括符文數字、月份，以及日本的假名組合、
		//	單位、年號、月份、日期、時間等。

		//	(6)3400～4DFFh：中日韓認同表意文字擴充A區，總計收容6,582
		//	個中日韓漢字。

		//	(7)4E00～9FFFh：中日韓認同表意文字區，總計收容20,902個中
		//	日韓漢字。

		//	(8)A000～A4FFh：彝族文字區，收容中國南方彝族文字和字根。

		//	(9)AC00～D7FFh：韓文拼音組合字區，收容以韓文音符拼成的文
		//	字。

		//	(10)D800～DFFFh：S區，專用於UTF-16，詳情後敘。

		//	(11)E000～F8FFh：專用字區，其內容WG2不予規定，保留供使用
		//	者自行添加ISO10646未收容的字元。

		//	(12)F900～FAFFh：中日韓相容表意文字區，總計收容302個中日
		//	韓漢字。何謂相容表意文字，留待後敘。

		//	(13)FB00～FFFDh：文字表現形式區，收容組合拉丁文字、希伯來
		//	文、阿拉伯文、中日韓直式標點、小符號、半形符號、全形
		//	符號等。

		float adjX = (int)penX - 0.5f;
		float adjY = (int)penY - 0.5f;

		if ((m_fontVertical) && (((0x4E00 > chText) || (chText > 0x9FFF))))
		{
			FILL_TLVERTEX(m_glyphTLVertex[0], adjX, adjY, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[1][1]);
			FILL_TLVERTEX(m_glyphTLVertex[1], adjX + fontScalingH, adjY, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[0][1]);
			FILL_TLVERTEX(m_glyphTLVertex[2], adjX + fontScalingH, adjY + fontScalingW, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[0][1]);
			FILL_TLVERTEX(m_glyphTLVertex[3], adjX, adjY + fontScalingW, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[1][1]);
		}
		else
		{
			FILL_TLVERTEX(m_glyphTLVertex[0], adjX, adjY, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[0][1]);
			FILL_TLVERTEX(m_glyphTLVertex[1], adjX + fontScalingW, adjY, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[0][1]);
			FILL_TLVERTEX(m_glyphTLVertex[2], adjX + fontScalingW, adjY + fontScalingH, m_fontColour, pFontGlyph->textureUV[1][0], pFontGlyph->textureUV[1][1]);
			FILL_TLVERTEX(m_glyphTLVertex[3], adjX, adjY + fontScalingH, m_fontColour, pFontGlyph->textureUV[0][0], pFontGlyph->textureUV[1][1]);
		}

		g_pFontRenderer->PrepareVeretex(pFontGlyph->textureId, m_glyphTLVertex, _countof(m_glyphTLVertex));
	}
}

bool  Font::charGlyphInfill(UINT charCode, void* ftGlyph, KG3DFontGlyph_t* fontGlyph)
{
	FT_GlyphSlot pGlyphSlot = (FT_GlyphSlot)ftGlyph;
	KG3DGlyphItem_t* pGlyphFont = (KG3DGlyphItem_t *)fontGlyph;
	FT_Error ftErr   = 0;

	_ASSERT(NULL != pGlyphSlot);
	_ASSERT(NULL != pGlyphFont);

	if (NULL == ftGlyph)
		return false;

	ftErr = FT_Render_Glyph(pGlyphSlot, FT_Render_Mode(m_fontFlagDraw));
	if (ftErr || pGlyphSlot->format != FT_GLYPH_FORMAT_BITMAP)
		return false;

	// request glyph texture
	pGlyphFont->textureId = glyphTextureRequest();
	if (0 != pGlyphFont->textureId)
	{
		// upload sub texture
		FT_Bitmap glyphBitmap = pGlyphSlot->bitmap;

		pGlyphFont->xOffset   = static_cast< FLOAT >(pGlyphSlot->bitmap_left);
		pGlyphFont->yOffset   = static_cast< FLOAT >(GetCharAscent() - pGlyphSlot->bitmap_top);

		if (m_fontVertical)
			std::swap(pGlyphFont->xOffset, pGlyphFont->yOffset);


		pGlyphFont->uploadTop  = static_cast< FLOAT >(m_glyphOffY);
		pGlyphFont->uploadLeft = static_cast< FLOAT >(m_glyphOffX);

		pGlyphFont->uploadWidth = static_cast< FLOAT >(glyphBitmap.width);
		pGlyphFont->uploadHeight = static_cast< FLOAT >(glyphBitmap.rows);

		pGlyphFont->textureUV[0][0] = static_cast<float>(m_glyphOffX) / static_cast<float>(m_glyphTextureWidth);
		pGlyphFont->textureUV[0][1] = static_cast<float>(m_glyphOffY) / static_cast<float>(m_glyphTextureHeight);

		pGlyphFont->textureUV[1][0] = static_cast<float>(m_glyphOffX + glyphBitmap.width) / static_cast<float>(m_glyphTextureWidth);
		pGlyphFont->textureUV[1][1] = static_cast<float>(m_glyphOffY + glyphBitmap.rows ) / static_cast<float>(m_glyphTextureHeight);

		// empty glyph?
		// 如果轮廓线没有被上载，FreeType 使用的位图渲染模式为 1bit/pixel
		if (((0 == pGlyphSlot->outline.n_contours)
			&& (0 == pGlyphSlot->outline.n_points))
			|| (FT_PIXEL_MODE_MONO == glyphBitmap.pixel_mode))
		{
			if (!g_pFontRenderer->UploadMonoTexture(pGlyphFont->textureId, m_glyphOffX, m_glyphOffY, 
				static_cast< UINT >(pGlyphFont->uploadWidth),
				static_cast< UINT >(pGlyphFont->uploadHeight), 
				static_cast< UINT >(glyphBitmap.pitch),
				glyphBitmap.buffer))
			{
				_ASSERT(!"UploadMonoTexture()");
			}
		}
		else
		{
			if (!g_pFontRenderer->UploadTexture(pGlyphFont->textureId, m_glyphOffX, m_glyphOffY, 
				static_cast< UINT >(pGlyphFont->uploadWidth),
				static_cast< UINT >(pGlyphFont->uploadHeight), 
				glyphBitmap.buffer))
			{
				_ASSERT(!"UploadTexture()");
			}
		}
	}

	m_glyphOffX += static_cast<INT>(pGlyphFont->uploadWidth);
	m_glyphOffX += KG3DFONT_DEFAULT_PADDING;

	--m_fontUploads;

	return true;
}


UINT  Font::glyphTextureRequest()
{
	INT glyphW = static_cast<INT>(GetCharWidth());
	INT glyphH = static_cast<INT>(GetCharHeight());

	UINT  uploadIndex = 0;

	if (m_glyphTextureVector.empty())
	{
		m_glyphTextureWidth = (KG3DFONT_GLYPH_NUMS * glyphW);
		m_glyphTextureHeight = KG3DFONT_GLYPH_NUMS;

		if (!g_pFontRenderer->BuildTexture(uploadIndex, m_glyphTextureWidth, m_glyphTextureHeight, glyphW, glyphH))
			_ASSERT(0 && "BuildTexture()");

		m_glyphOffX = m_glyphOffY = KG3DFONT_DEFAULT_PADDING;
		m_glyphTextureLast = uploadIndex;
		m_glyphTextureVector.push_back(m_glyphTextureLast);

	}
	else if (m_glyphOffX >= (static_cast< INT >(m_glyphTextureWidth) - glyphW))
	{
		m_glyphOffX = KG3DFONT_DEFAULT_PADDING;
		m_glyphOffY += glyphH + KG3DFONT_DEFAULT_PADDING;

		if (m_glyphOffY >= (static_cast< INT >(m_glyphTextureHeight) - glyphH))
		{
			if (!g_pFontRenderer->BuildTexture(uploadIndex, m_glyphTextureWidth, m_glyphTextureHeight, glyphW, glyphH))
				_ASSERT(0 && "BuildTexture TWO()");

			m_glyphOffY = KG3DFONT_DEFAULT_PADDING;
			m_glyphTextureLast = uploadIndex;
			m_glyphTextureVector.push_back(m_glyphTextureLast);
		}
	}

	return m_glyphTextureLast;
}


