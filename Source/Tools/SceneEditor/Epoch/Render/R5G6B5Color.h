// File: R5G6B5Color.h
// Desc:

#ifndef R5G6B5COLOR_H
#define R5G6B5COLOR_H

#include"..\Common\DLLAPI.h"
#include"..\Maths\MathsCommon.h"

namespace Render
{

class DLLAPI A8R8G8B8Color;
class DLLAPI ColorValue;

class DLLAPI R5G6B5Color
{
private:
	union
	{
		unsigned short m_Color;
		
		struct
		{
			unsigned short m_Red: 5;
			unsigned short m_Green: 6;
			unsigned short m_Blue: 5;
		};
	};

public:
	R5G6B5Color(): m_Color(0x0000) {}  // Black color default
	R5G6B5Color(short sRed, short sGreen, short sBlue);
	R5G6B5Color(const R5G6B5Color& rgb): m_Color(rgb.m_Color) {}
	R5G6B5Color& operator = (const R5G6B5Color& rgb);

	R5G6B5Color(const A8R8G8B8Color& argb);
	R5G6B5Color(const ColorValue& cv);

	unsigned short GetColor() const { return m_Color; }
	unsigned short GetRed() const { return m_Red; }
	unsigned short GetGreen() const { return m_Green; }
	unsigned short GetBlue() const { return m_Blue; }

	void SetColor(unsigned short Color) { m_Color = Color; }
	void SetRed(short sRed) { m_Red = Maths::ClampColor(sRed, 5); }
	void SetGreen(short sGreen) { m_Green = Maths::ClampColor(sGreen, 6); }
	void SetBlue(short sBlue) { m_Blue = Maths::ClampColor(sBlue, 5); }
};

typedef R5G6B5Color RGB565;

inline RGB565 MakeRGB565(short sRed, short sGreen, short sBlue)
{
	return R5G6B5Color(sRed, sGreen, sBlue);
}

}  // namespace

#endif