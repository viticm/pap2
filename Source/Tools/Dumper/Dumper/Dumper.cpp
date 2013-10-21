// Dumper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Dumper.h"
#include "KDumper.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HINSTANCE g_hInst = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = (HINSTANCE)hModule;
        KDumper::ms_Dumper.OnProcessAttach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:   
        break;
    case DLL_PROCESS_DETACH:
        KDumper::ms_Dumper.OnProcessDetach();
        break;
    default:
        break;
    }   
    return TRUE;
}

DUMPER_API int SetDumperParameter(const DUMPER_PARAMETER *pDumperParam)
{
    int nResult  = FALSE;
    int nRetCode = FALSE;

    KG_PROCESS_ERROR(pDumperParam);
    nRetCode = KDumper::ms_Dumper.SetServerAddress(pDumperParam->szIP, pDumperParam->nPort);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = KDumper::ms_Dumper.SetProductVersion(pDumperParam->szProductVersion);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

DUMPER_API int SetLogParamter(TCHAR szPath[], TCHAR szIdent[])
{
    int nResult = FALSE;
    int nRetCode = FALSE;

    KG_PROCESS_ERROR(szPath);
    KG_PROCESS_ERROR(szIdent);

    nRetCode = KDumper::ms_Dumper.SetLogPath(szPath, szIdent);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

