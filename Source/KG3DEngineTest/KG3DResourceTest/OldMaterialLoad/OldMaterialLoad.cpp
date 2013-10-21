// OldMaterialLoad.cpp : Defines the entry point for the DLL application.
//

// #include "stdafx.h"

#include "OldMaterialLoad.h"
#include "KGLoadController.h"

extern KGLoadController g_cKGLoadController;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD   ul_reason_for_call,
                       LPVOID  lpReserved
					 )
{
	int nResult  = false;
	int nRetCode = false;

	nResult = true;
Exit0:
	return nResult;
}

int GetLoadController(void** pController)
{
	*pController = &g_cKGLoadController;
	return true;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

