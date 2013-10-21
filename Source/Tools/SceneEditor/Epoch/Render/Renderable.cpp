// File: Renderable.cpp
// Desc:

#include"Renderable.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

Renderable& Renderable::operator = (const Renderable& r)
{
	if(&r != this)
	{
		m_Matrix = r.m_Matrix;
	}

	return *this;
}

}  // namespace