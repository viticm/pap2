
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : sysinfo.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "def.h"
#include "ifimple.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}

#endif


extern "C" EXPORT_API void CreatePlugInterface(IGuardPlugin** ppPlug)
{
	static KSysPlugin _SI;
	*ppPlug = &_SI;
}
