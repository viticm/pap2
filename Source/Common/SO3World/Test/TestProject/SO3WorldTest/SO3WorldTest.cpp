// SO3WorldTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "./KSUnitTest/KSCppUnit.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = false;

	printf("\n");	//Force a newline
	nRetCode = KSCppUnitTest("SO3WorldTest", ".\\logs\\SO3World", HTML_OUTPUT);
	if(nRetCode)
		printf("All test are tested passed ...");
	
	return 0;
}

