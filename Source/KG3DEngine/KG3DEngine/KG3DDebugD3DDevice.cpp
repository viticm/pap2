#include "stdafx.h"
#include "./KG3DDebugD3DDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KDebugDirect3DDevice9 g_DebugD3DDevice;

IKG3DDebugInfo& g_GetDebugInfo()
{
	return g_DebugD3DDevice;
}


