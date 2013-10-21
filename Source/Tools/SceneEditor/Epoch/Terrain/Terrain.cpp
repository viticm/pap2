// File: Terrain.cpp
// Desc:

#include"Terrain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

static Terrain::TerrainTree* pTT = 0;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//pTT = new KTerrainTree();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		SafeDelete(pTT);
		break;

	default:
		return false;
	}

	return true;
}

Terrain::TerrainTree* g_GetTerrainTree()
{
	if(!pTT)
	{
		throw Common::Exception(Common::EL_BAD, "g_GetTerrainTree", "pTT is not valid yet!");
	}

	return pTT;
}

void g_SetTerrainTree(Terrain::TerrainTree* TT)
{
	SafeDelete(pTT);

	pTT = TT;
}