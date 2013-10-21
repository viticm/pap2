// gms.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "guardpluginterface.h"
#include "kclient.h"
#include "kgmsclient.h"
#include "gmsplug.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#ifdef WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

#ifdef __linux
#define EXPORT_API
#endif

extern "C" EXPORT_API void CreatePlugInterface(IGuardPlugin** ppPlug)
{
	static KGMSPlug plug;
	*ppPlug = &plug;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

