// -*- C++ -*-

#ifndef FONT_H
#define FONT_H

#include "./fonttypes.h"
#include  <ft2build.h>
#include  FT_FREETYPE_H
#include  FT_GLYPH_H
#include  FT_OUTLINE_H

// font style
#define  KG3DFONT_STYLE_NONE			0x00
#define  KG3DFONT_STYLE_BORDER			0x01
#define  KG3DFONT_STYLE_VERTICAL		0x02
#define  KG3DFONT_STYLE_PROJECTION		0x04

enum
{
	KG3DFONT_SLOT_X		= 0,
	KG3DFONT_SLOT_Y		= 1,
};

////////////////////////////////////////////////
// font point / dpi
UINT  const  KG3DFONT_DEFAULT_DPI		= 96;
UINT  const  KG3DFONT_DEFAULT_POINT		= 16;




////////////////////////////////////////////////
// font glyph
// 参考教育部 1998 年出版品之常用字统计资料库，
// 此资料库统计 1579771 个字词，其中常用字共有 5063 个。
// 不过，出现率最高的前 80% 只有 641 个字，
// 考虑到前瞻性，变态性(有些国家的语言变态的说) 等等，
// 这里使用 1024 作为最大缓冲大小。

UINT  const  KG3DFONT_GLYPH_NUMS	= 1024;
UINT  const  KG3DFONT_GLYPH_LIMIT	= 0xFFFF;



struct KG3DFontGlyph_t
{
	INT		glyphIndex;
	INT		glyphFormat;

	FLOAT	glyphAdvance[2];	// the advance distance for this glyph
	FLOAT	glyphVAdvance[2];	// the advance distance for this glyph

	FLOAT	glyphBLower[2];
	FLOAT	glyphBUpper[2];		// the bounding box of this glyph.
};


class FontRenderer;
class Font
{
private:
	struct KG3DGlyphItem_t : public KG3DFontGlyph_t
	{
		INT		textureId;
		FLOAT	textureUV[2][2];
		FLOAT	borderUV[2][2];

		FLOAT	xOffset;
		FLOAT	yOffset;

		FLOAT	uploadTop;
		FLOAT	uploadLeft;
		FLOAT	uploadWidth;
		FLOAT	uploadHeight;
	};

	typedef std::vector<UINT>  GlyphTexVector_t;

public:
	Font();
	~Font();

	BOOL  Initialize(LPCTSTR fontName, INT fontSize, UINT fontStyle, UINT fontDpi = KG3DFONT_DEFAULT_DPI);
	BOOL  CleanUp();

	UINT  PreloadRange(UCHAR First = 0x20, UCHAR Last = 0x7F);
	UINT  PreloadText(LPCTSTR lpchText, int cchText);

	void  DrawText(FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FLOAT cchSpace = 0.0f);
	void  Draw3DText(FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FLOAT cchSpace = 0.0f);

	void  SetTextAlpha(BYTE alpha);
	void  SetTextColor(UINT colour);
	void  SetTextScaling(FLOAT scaleX = 1.0f, FLOAT scaleY = 1.0f);

	BYTE  GetTextAlpha() const;
	UINT  GetTextColour() const;
	void  GetTextScaling(FLOAT& scaleX, FLOAT& scaleY) const;

	BOOL  GetTextExtent(LPCTSTR lpchText, int cchText, FLOAT &fWidth, FLOAT &fHeight, FLOAT cchSpace = 0.0f);
	BOOL  GetTextPosExtent(LPCTSTR lpchText, int cchText, FLOAT fCursorPos, LPINT lpCharPos, FLOAT cchSpace = 0.0f);

	void  SetTextEffect(UINT effect);
	void  SetTextBorder(UINT color, UINT weight);
	void  SetTextProjection(UINT color, UINT weight);

	UINT  GetTextEffect() const;
	void  GetTextBorder(UINT& color, UINT& weight) const;
	void  GetTextProjection(UINT& color, UINT& weight) const;

	FLOAT GetCharWidth() const;
	FLOAT GetCharHeight() const;

	FLOAT GetCharAscent() const;
	FLOAT GetCharDescent() const;

private:
	void DrawChar(FLOAT penX, FLOAT penY, WCHAR chText);
	void DrawCharEx(FLOAT penX, FLOAT penY, WCHAR chText);

	bool charGlyphInfill(UINT charCode, void* ftGlyph, KG3DFontGlyph_t* fontGlyph);

protected:
	typedef void (Font::*FN_DRAWGLYPH)(FLOAT penX, FLOAT penY, WCHAR chText);

	void  drawNormalText(FLOAT baseX, FLOAT baseY, LPCWSTR lpchText, FN_DRAWGLYPH fnDrawText);
	void  drawStandardText(FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText);
	void  drawVerticalText(FLOAT baseX, FLOAT baseY, LPCTSTR lpchText, int cchText, FN_DRAWGLYPH fnDrawText);

	LPCWSTR charToUnicode(LPCTSTR lpchText, int cchText);

	KG3DFontGlyph_t* charToGlyphSlot(UINT charCode);
	INT   charToGlyphIndex(UINT charCode);
	INT   charToIndex(UINT charCode);

	bool  charGlyphHit(UINT charCode);
	bool  charGlyphAdvance(UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY);

	bool  charGlyphAdvance(UINT chText, FLOAT& advX, FLOAT& advY);
	bool  charGlyphKernAdvance(UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY);

	INT   getGlyphUsage() const;
	INT   getGlyphBudget() const;

	UINT  glyphTextureRequest();

protected:
	FT_Face m_fontFace;
	
	UINT	m_fontGlyphs;
	UINT	m_fontUploads;
	UINT	m_fontEncoding;

	BOOL	m_fontUnicode;
	BOOL	m_fontVertical;
	BOOL	m_fontKerningTable;

	UINT	m_fontFlagDraw;
	UINT	m_fontFlagLoader;

	UINT	m_fontPoint;
	UINT	m_fontDPI;

	D3DCOLOR	m_fontColour;
	FLOAT		m_fontSpaceX;
	FLOAT		m_fontSpaceY;

	FLOAT		m_fontScalingX;
	FLOAT		m_fontScalingY;

	UINT		m_fontEffect;
	UINT		m_fontBorderColor;
	INT			m_fontBorderWeight;
	
	UINT		m_fontProjectionColor;
	INT			m_fontProjectionWeight;

	KG3DFontGlyph_t*	m_fontCharVector[KG3DFONT_GLYPH_LIMIT];

	// Texture font
	INT					m_glyphOffX;
	INT					m_glyphOffY;

	UINT				m_glyphTextureLast;
	GlyphTexVector_t	m_glyphTextureVector;

	UINT				m_glyphTextureWidth;
	UINT				m_glyphTextureHeight;

	FontVertex			m_glyphVertex[4];
	FontTLVertex		m_glyphTLVertex[4];
};

inline void  Font::SetTextAlpha(BYTE alpha)
{
	m_fontColour &= 0xFFFFFF;
	m_fontColour |= (((alpha) & 0xFF) << 24);
}

inline void  Font::SetTextColor(UINT color)
{
	m_fontColour &= 0xFF000000;
	m_fontColour |= ((color) & 0xFFFFFF);
}

inline void  Font::SetTextScaling(FLOAT scaleX, FLOAT scaleY)
{
	m_fontScalingX = scaleX;
	m_fontScalingY = scaleY;
}

inline BYTE  Font::GetTextAlpha() const
{
	return (BYTE)((m_fontColour & 0xFF000000) >> 24);
}

inline UINT  Font::GetTextColour() const
{
	return (m_fontColour & 0xFFFFFF);
}

inline void Font::GetTextScaling(FLOAT& scaleX, FLOAT& scaleY) const
{
	scaleX = m_fontScalingX;
	scaleY = m_fontScalingY;
}

inline void Font::SetTextEffect(UINT effect)
{
	m_fontEffect = (effect & (~KG3DFONT_STYLE_VERTICAL));
}

inline void Font::SetTextBorder(UINT color, UINT weight)
{
	// 限制边框大小
	_ASSERT(weight <= 2);

	m_fontBorderColor =	color;
	m_fontBorderWeight = weight;
}

inline void Font::SetTextProjection(UINT color, UINT weight)
{
	m_fontProjectionColor = color;
	m_fontProjectionWeight = weight;
}

inline UINT Font::GetTextEffect() const { return m_fontEffect; }

inline void Font::GetTextBorder(UINT& color, UINT& weight) const
{
	color = m_fontBorderColor;
	weight = m_fontBorderWeight;
}

inline void Font::GetTextProjection(UINT& color, UINT& weight) const
{
	color =	m_fontProjectionColor;
	weight = m_fontProjectionWeight;
}


#endif // FONT_H
// -*- C++ -*-
