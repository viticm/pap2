// File: Rect.h
// Desc:

#ifndef RECT_H
#define RECT_H

#include"..\Common\DLLAPI.h"

//#include"Vector.h"

namespace Maths
{

class DLLAPI Rect
{
public:
	unsigned int m_uiLeft;
	unsigned int m_uiTop;
	unsigned int m_uiWidth;
	unsigned int m_uiHeight;

public:
	Rect(unsigned int uiLeft = 0, unsigned int uiTop = 0, unsigned int uiWidth = 0, unsigned int uiHeight = 0): m_uiLeft(uiLeft), m_uiTop(uiTop), m_uiWidth(uiWidth), m_uiHeight(uiHeight) {}
	Rect(const Rect& r): m_uiLeft(r.m_uiLeft), m_uiTop(r.m_uiTop), m_uiWidth(r.m_uiWidth), m_uiHeight(r.m_uiHeight) {}
	Rect& operator = (const Rect& r);

	//bool IsPointInside(const Vector3F& v) const;
};

}  // namespace

#endif