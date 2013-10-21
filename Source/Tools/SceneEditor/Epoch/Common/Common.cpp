// File: Common.cpp
// Desc:

#include"Common.h"

#include<windows.h>

static Common::DateTimeSystem* pDTS = 0;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		pDTS = new Common::DateTimeSystem;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		SafeDelete(pDTS);
		break;

	default:
		return false;
	}

	return true;
}

Common::DateTimeSystem* g_GetDateTimeSystem()
{
	if(!pDTS)
	{
		throw Common::Exception(Common::EL_BAD, "g_GetDateTimeSystem", "pDTS is not valid yet!");
	}

	return pDTS;
}