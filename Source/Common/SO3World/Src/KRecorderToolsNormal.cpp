#include "stdafx.h"
#include "Engine/KG_Time.h"
#include "Engine/KG_CreateGUID.h"
#include "KRecorderToolsNormal.h"

#ifdef WIN32
#include <conio.h>
#else
#include <curses.h>
#endif

DWORD KRecorderToolsNormal::GetTickCount()
{
    return ::KG_GetTickCount();
}

time_t  KRecorderToolsNormal::GetTime()
{
    return ::time(NULL);
}

BOOL KRecorderToolsNormal::CreateGUID(GUID* pGuid)
{
    BOOL    bResult  = false;
    int     nRetCode = false;

    assert(pGuid);
    
    nRetCode = KG_CreateGUID(pGuid);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

