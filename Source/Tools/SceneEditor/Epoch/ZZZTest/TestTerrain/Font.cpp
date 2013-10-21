// File: Font.cpp
// Desc:

#include"Font.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

Font::Font(const Common::String& sFontName, int Height, bool bBold, bool bItalic, bool bUnderlined): m_pFont(0)
{
	Init(sFontName, Height, bBold, bItalic, bUnderlined);
}

void Font::Init(const Common::String& sFontName, int Height, bool bBold, bool bItalic, bool bUnderlined)
{
	HFONT hFont;

	int Weight = FW_NORMAL;
	DWORD dwItalic = 0;
	DWORD dwUnderlined = 0;

	if(bBold)
	{
		Weight = FW_BOLD;
	}

	if(bItalic)
	{
		dwItalic = 1;
	}

	if(bUnderlined)
	{
		dwUnderlined = 1;
	}

	hFont = CreateFont(Height, 0, 0, 0, Weight, dwItalic, dwUnderlined, 0, ANSI_CHARSET, 0, 0, 0, 0, sFontName);

	D3DXCreateFont(g_GetRenderCore()->GetD3DDevice(), hFont, &m_pFont);
}

void Font::DrawSingleText(const Common::String& sText, int x, int y, D3DCOLOR Colour)
{
	RECT Rect;

	Rect.left = x;
	Rect.top = y;
	Rect.right = 0;
	Rect.bottom = 0;

	m_pFont->Begin();
	m_pFont->DrawText(sText, -1, &Rect, DT_CALCRECT, 0);			//Calculate the size of the rect needed
	m_pFont->DrawText(sText, -1, &Rect, DT_LEFT, Colour);	//Draw the text
	m_pFont->End();
}

void Font::Begin()
{
	m_pFont->Begin();
}

void Font::DrawText(const Common::String& sText, int x, int y, D3DCOLOR Colour)
{
	RECT Rect;

	Rect.left = x;
	Rect.top = y;
	Rect.right = 0;
	Rect.bottom = 0;

	m_pFont->DrawText(sText, -1, &Rect, DT_CALCRECT, 0);			//Calculate the size of the rect needed
	m_pFont->DrawText(sText, -1, &Rect, DT_LEFT, Colour);	//Draw the text
}

void Font::End()
{
	m_pFont->End();
}