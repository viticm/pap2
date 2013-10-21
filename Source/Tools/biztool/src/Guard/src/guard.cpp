//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-02 17:00
//      File_base        : guard.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "client.h"
//#include "common/kinstance.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////
KGuardClient g_cGuardClient;

int main( int argc, char* args[] )
{
	//if (KInstance::IsRunning("guard"))
	//{
	//	printf("Failed to start: guard is already running");
	//	return -1;
	//} 

	//KInstance _instance("guard");

	// 	if (!_instance.Lock())
	// 	{
	// 		printf("Failed to start: guard is already running");
	// 		return -1;
	// 	}

	char szPath[MAX_PATH];
	strcpy(szPath, args[0]);

	int nCount = strlen(szPath) - 1;
	while (nCount && szPath[nCount]) 
	{
		if (szPath[nCount] == '/' ||
			szPath[nCount] == '\\' )
		{
			szPath[nCount] = 0;
			break;
		}
		nCount--;
	}

	chdir(szPath);
	if (!g_cGuardClient.Initialize())
		return -1;
	g_cGuardClient.Action();
	g_cGuardClient.UnInitialize();

	return 0;
}
