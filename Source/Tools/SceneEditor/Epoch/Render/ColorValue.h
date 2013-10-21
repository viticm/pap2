// File: ColorValue.h
// Desc:

#ifndef COLORVALUE_H
#define COLORVALUE_H

#include"..\Common\DLLAPI.h"
#include"..\Maths\MathsCommon.h"

namespace Render
{

class DLLAPI A8R8G8B8Color;
class DLLAPI R5G6B5Color;

class DLLAPI ColorValue
{
public:
	float r, g, b, a;

public:
	ColorValue(float fA = 1.f, float fR = 1.f, float fG = 1.f, float fB = 1.f): r(Maths::ClampColor(fR)), g(Maths::ClampColor(fG)), b(Maths::ClampColor(fB)), a(Maths::ClampColor(fA)) {}
	ColorValue(const ColorValue& cv): r(cv.r), g(cv.g), b(cv.b), a(cv.a) {}
	ColorValue& operator = (const ColorValue& cv);
	~ColorValue() {}

	ColorValue(const A8R8G8B8Color& argb);
	ColorValue(const R5G6B5Color& rgb);
};

inline bool operator == (const ColorValue& a, const ColorValue& b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

inline bool operator != (const ColorValue& a, const ColorValue& b)
{
	return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

}  // namespace

#endif