// File: A8R8G8B8Color.cpp
// Desc:

#include"A8R8G8B8Color.h"
#include"ColorValue.h"

namespace Render
{

A8R8G8B8Color& A8R8G8B8Color::operator = (const A8R8G8B8Color& argb)
{
	if(&argb != this)
	{
		m_Color = argb.m_Color;
	}

	return *this;
}

A8R8G8B8Color::A8R8G8B8Color(const ColorValue& cv)
{
	m_Alpha = Clamp(cv.a * 255);
	m_Red = Clamp(cv.r * 255);
	m_Green = Clamp(cv.g * 255);
	m_Blue = Clamp(cv.b * 255);
}

}  // namespace