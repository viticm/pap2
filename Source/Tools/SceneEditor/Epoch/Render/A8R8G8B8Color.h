// File: A8R8G8B8Color.h
// Desc:

#ifndef A8R8G8B8Color_H
#define A8R8G8B8Color_H

#include"..\Common\DLLAPI.h"
#include"..\Maths\MathsCommon.h"

namespace Render
{

class DLLAPI ColorValue;

class DLLAPI A8R8G8B8Color
{
private:
	union
	{
		unsigned int m_Color;

		struct
		{
			unsigned int m_Alpha: 8;
			unsigned int m_Red: 8;
			unsigned int m_Green: 8;
			unsigned int m_Blue: 8;
		};
	};

public:
	A8R8G8B8Color(): m_Color(0x00000000) {}  // Black color default
	A8R8G8B8Color(int iAlpha, int iRed, int iGreen, int iBlue): m_Alpha(Clamp(iAlpha)), m_Red(Clamp(iRed)), m_Green(Clamp(iGreen)), m_Blue(Clamp(iBlue)) {}
	A8R8G8B8Color(const A8R8G8B8Color& argb): m_Color(argb.m_Color) {}
	A8R8G8B8Color& operator = (const A8R8G8B8Color& argb);

	A8R8G8B8Color(const ColorValue& cv);

	unsigned int GetColor() const { return m_Color; }
	unsigned int GetAlpha() const { return m_Alpha; }
	unsigned int GetRed() const { return m_Red; }
	unsigned int GetGreen() const { return m_Green; }
	unsigned int GetBlue() const { return m_Blue; }

	void SetColor(unsigned int uiColor) { m_Color = uiColor; }
	void SetAlpha(int iAlpha) { m_Alpha = Clamp(iAlpha); }
	void SetRed(int iRed) { m_Red = Clamp(iRed); }
	void SetGreen(int iGreen) { m_Green = Clamp(iGreen); }
	void SetBlue(int iBlue) { m_Blue = Clamp(iBlue); }

	static unsigned int Clamp(int iOriginal) { return Maths::ClampColor(iOriginal, 8); }
};

typedef A8R8G8B8Color ARGB;
typedef A8R8G8B8Color XRGB;

inline ARGB MakeARGB(int iAlpha, int iRed, int iGreen, int iBlue)
{
	return A8R8G8B8Color(iAlpha, iRed, iGreen, iBlue);
}

inline XRGB MakeXRGB(int iRed, int iGreen, int iBlue)
{
	return A8R8G8B8Color(0xFF, iRed, iGreen, iBlue);
}

}  // namespace

#endif