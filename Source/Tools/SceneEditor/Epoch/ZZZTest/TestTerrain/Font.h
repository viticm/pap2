// File: Font.h
// Desc:

#ifndef FONT_H
#define FONT_H

#include"..\..\Render\Render.h"

class Font
{
public:
	Font(): m_pFont(0) {}
	Font(const Common::String& sFontName, int Height, bool bBold, bool bItalic, bool bUnderlined);
	~Font() { SafeRelease(m_pFont); }

	void Init(const Common::String& sFontName, int Height, bool bBold, bool bItalic, bool bUnderlined);

	void DrawSingleText(const Common::String& sText, int x, int y, D3DCOLOR Colour);

	void Begin();
	void DrawText(const Common::String& sText, int x, int y, D3DCOLOR Colour);
	void End();
private:
	ID3DXFont* m_pFont;
};

#endif