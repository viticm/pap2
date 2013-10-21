// File: Assert.cpp
// Desc:

#include"Assert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Common
{

void Assert(bool b, const String& s)
{
#ifdef ENGINE_DEBUG_VERSION
	if(!b)
	{
		throw Exception(EL_BAD, "Assert Failure", s);
	}
#endif
}

}