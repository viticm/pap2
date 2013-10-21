// File: ColorValue.cpp
// Desc:

#include"ColorValue.h"
#include"A8R8G8B8Color.h"
#include"R5G6B5Color.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

ColorValue& ColorValue::operator = (const ColorValue& cv)
{
	if(&cv != this)
	{
		r = cv.r;
		g = cv.g;
		b = cv.b;
		a = cv.a;
	}

	return *this;
}

ColorValue::ColorValue(const A8R8G8B8Color& argb)
{
	r = argb.GetRed() / 255.f;
	g = argb.GetGreen() / 255.f;
	b = argb.GetBlue() / 255.f;
	a = argb.GetAlpha() / 255.f;
}

ColorValue::ColorValue(const R5G6B5Color& rgb)
{
	r = rgb.GetRed() / 31.f;
	g = rgb.GetGreen() / 63.f;
	b = rgb.GetBlue() / 31.f;
	a = 1.f;
}

}  // namespace