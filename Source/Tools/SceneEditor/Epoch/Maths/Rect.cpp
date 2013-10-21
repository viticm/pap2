// File: Rect.cpp
// Desc:

#include"Rect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Maths
{

Rect& Rect::operator = (const Rect& r)
{
	if(&r != this)
	{
		m_uiLeft = r.m_uiLeft;
		m_uiTop = r.m_uiTop;
		m_uiWidth = r.m_uiWidth;
		m_uiHeight = r.m_uiHeight;
	}

	return *this;
}

//bool Rect::IsPointInside(const Vector3F& v) const
//{
//	Vector2I vMin(m_uiLeft, m_uiTop)
//}

}  // namespace