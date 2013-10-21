// RoadSystermTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestShortPath.h"
#include "Engine/EngineBase.h"
#include "Engine/File.h"

int _tmain(int argc, _TCHAR* argv[])
{
    TestShortPath testShortPath;

    g_SetRootPath(NULL); 

    testShortPath.TestAll();

	return 0;
}

