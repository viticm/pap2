// File: Use.cpp
// Desc:

//#include"..\Common\Common.h"

#include<iostream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


void PrintHello()
{
	cout<<"Hello, world!"<<endl;
}

void main()
{
	PrintHello();
}