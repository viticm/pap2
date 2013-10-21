// File: Render.cpp
// Desc:

#include"Render.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

static Render::RenderCore* pRC = 0;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		pRC = new Render::RenderCore();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		SafeDelete(pRC);
		break;

	default:
		return false;
	}

	return true;
}

Render::RenderCore* g_GetRenderCore()
{
	if(!pRC)
	{
		throw Common::Exception(Common::EL_BAD, "g_GetRenderCore", "pRC is not valid yet!");
	}

	return pRC;
}