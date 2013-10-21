// File: R5G6B5Color.cpp
// Desc:

#include"R5G6B5Color.h"
#include"A8R8G8B8Color.h"
#include"ColorValue.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

R5G6B5Color::R5G6B5Color(short sRed, short sGreen, short sBlue)
{
	m_Red = Maths::ClampColor(sRed, 5);
	m_Green = Maths::ClampColor(sGreen, 6);
	m_Blue = Maths::ClampColor(sBlue, 5);
}

R5G6B5Color& R5G6B5Color::operator = (const R5G6B5Color& rgb)
{
	if(&rgb != this)
	{
		m_Color = rgb.m_Color;
	}

	return *this;
}

R5G6B5Color::R5G6B5Color(const A8R8G8B8Color& argb)
{
	m_Red = argb.GetRed() / 8;
	m_Green = argb.GetGreen() / 4;
	m_Blue = argb.GetBlue() / 8;
}

R5G6B5Color::R5G6B5Color(const ColorValue& cv)
{
	m_Red = Maths::ClampColor(cv.r * 255 / 8, 5);
	m_Green = Maths::ClampColor(cv.g * 255 / 4, 6);
	m_Blue = Maths::ClampColor(cv.b * 255 / 8, 5);
}

}  // namespace