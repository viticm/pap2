// File: TestDateTimeSystem.cpp
// Desc:

#include"..\..\Common\Common.h"

#include<iostream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


void main()
{
	unsigned int ms = g_GetDateTimeSystem()->GetTimeValueMS_HR();
}