#include "stdafx.h"
#include "KSO3World.h"
#include "KProtocolRecorderBase.h"
#include "KRecorderToolsReplay.h"

DWORD KRecorderToolsReplay::GetTickCount()
{
    DWORD               dwRetTickCount  = 0;
    BOOL                bRetCode        = false;
    DWORD               dwTickCount     = 0;
    KGET_TICKCOUNT      GetTickCount;

    bRetCode = m_piReplayer->Pop(&GetTickCount, sizeof(GetTickCount));
    KGLOG_PROCESS_ERROR(bRetCode);
    
    assert(GetTickCount.uType == ertGetTickCount);

    dwRetTickCount = GetTickCount.dwTime;
Exit0:
    return dwRetTickCount;
}

time_t KRecorderToolsReplay::GetTime()
{
    time_t              nRetTime   = 0;
    BOOL                bRetCode   = false;
    KGET_TIME           GetTime;

    bRetCode = m_piReplayer->Pop(&GetTime, sizeof(GetTime));
    KGLOG_PROCESS_ERROR(bRetCode);
    
    assert(GetTime.uType == ertGetTime);

    nRetTime = GetTime.nTime;
Exit0:
    return nRetTime;
}

BOOL KRecorderToolsReplay::CreateGUID(GUID* pGuid)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KGET_GUID           GetGUID;

    assert(pGuid);
    
    bRetCode = m_piReplayer->Pop(&GetGUID, sizeof(GetGUID));
    KGLOG_PROCESS_ERROR(bRetCode);

    assert(GetGUID.uType == ertGUID);
    
    KGLOG_PROCESS_ERROR(GetGUID.guid.Data1 != 0); // equal zero means failed

    *pGuid = GetGUID.guid;

    bResult = true;
Exit0:
    return bResult;
}
