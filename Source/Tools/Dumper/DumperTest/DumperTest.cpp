// DumperTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

//#pragma comment(lib,"../DumpClient_dll/debug/dumpclient_dlld.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	LoadLibrary(_T("dumperd.dll"));

	*((int *)0) = 5;

	return 0;
}

