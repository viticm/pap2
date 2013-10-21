// KGOldEngineController.cpp : Defines the entry point for the DLL application.
//

//#include "stdafx.h"

#include "KGOldEngineController.h"
#include "KG3DEngineController.h"

extern KG3DEngineController g_cKG3DEngineController;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

int GetController(void** pController)
{
	*pController = &g_cKG3DEngineController;
	return true;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif