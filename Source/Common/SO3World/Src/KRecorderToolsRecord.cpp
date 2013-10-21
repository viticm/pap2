#include "stdafx.h"
#include "Engine/KG_Time.h"
#include "Engine/KG_CreateGUID.h"
#include "KSO3World.h"
#include "KProtocolRecorderBase.h"
#include "KRecorderToolsRecord.h"

#ifdef WIN32
#include <conio.h>
#else
#include <curses.h>
#endif

DWORD KRecorderToolsRecord::GetTickCount()
{
    DWORD               dwRetTime       = 0;
    DWORD               dwCurrentTime   = 0;
    KGET_TICKCOUNT      GetTickCount;

    assert(g_pSO3World);
    assert(m_piRecorder);

    dwCurrentTime = ::KG_GetTickCount();
    
    GetTickCount.uType  = ertGetTickCount;
    GetTickCount.dwTime = dwCurrentTime;

    m_piRecorder->Push(&GetTickCount, sizeof(GetTickCount));

    dwRetTime = dwCurrentTime;
Exit0:
    return dwRetTime;
}

time_t  KRecorderToolsRecord::GetTime()
{
    time_t              nRetTime        = 0;
    time_t              nCurrentTime    = 0;
    KGET_TIME           GetTime;

    assert(g_pSO3World);
    assert(m_piRecorder);

    nCurrentTime = ::time(NULL);
    
    GetTime.uType = ertGetTime;
    GetTime.nTime = nCurrentTime;

    m_piRecorder->Push(&GetTime, sizeof(GetTime));

    nRetTime = nCurrentTime;
Exit0:
    return nRetTime;
}

BOOL KRecorderToolsRecord::CreateGUID(GUID* pGuid)
{
    BOOL                bResult         = false;
    int                 nOperatResult   = false;
    GUID                guid            = {0, 0, 0, 0};
    KGET_GUID           GetGUID;

    assert(pGuid);
    assert(g_pSO3World);
    assert(m_piRecorder);

    nOperatResult = KG_CreateGUID(&guid);
    
    GetGUID.uType = ertGUID;
    GetGUID.guid  = guid; 

    m_piRecorder->Push(&GetGUID, sizeof(GetGUID));

    *pGuid = guid;
Exit0:
    return nOperatResult;
}

