////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_Angel.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:52:14
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_Angel.h"
#include "SO3GlobalDef.h"
#include "SO3ProtocolBasic.h"
#include "KG_RoleInfo3.h"
#include "KG_Statistic3.h"
#include "KG_RoleProcessor3.h"
#include "KG_BishopProcessor3.h"
#include "Engine/KMutex.h"
#include "Engine/KG_InterlockedVariable.h"

using namespace std;

static long gs_lInitRefCout = 0;  // reference count for init time

////////////////////////////////////////////////////////////////////////////////
int __stdcall KG_AngelInit(void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;
    int nKGMemoryInitFlag = false;
    static KMutex s_InitLock;

    s_InitLock.Lock();

    KG_USE_ARGUMENT(pvContext);

    if (gs_lInitRefCout == 0)
    {
        nRetCode = KG_MemoryInit(NULL);
        KGLOG_PROCESS_ERROR(nRetCode && "KG_MemoryInit()");
        nKGMemoryInitFlag = true;
    }

//Exit1:
    ++gs_lInitRefCout;
    nResult = true;
Exit0:
    s_InitLock.Unlock();
    if (!nResult)
    {
        if (nKGMemoryInitFlag)
        {
            KG_MemoryUnInit(NULL);
            nKGMemoryInitFlag = true;
        }
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int __stdcall KG_GetAngelVersion(
    unsigned *puMainVersion, unsigned *puSubVersion
)
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(puMainVersion);
    KGLOG_PROCESS_ERROR(puSubVersion);

    *puMainVersion = 3; // sword3
    *puSubVersion  = 2;
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}


////////////////////////////////////////////////////////////////////////////////
IKG_RoleInfo *__stdcall KG_CreateRoleInfo()
{
    int nResult  = false;
    int nRetCode = false;
    KG_RoleInfo3 *piRoleDataProcessor = NULL;

    piRoleDataProcessor = new(nothrow) KG_RoleInfo3;
    KGLOG_PROCESS_ERROR(piRoleDataProcessor);

//Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_COM_RELEASE(piRoleDataProcessor);
    }
    return piRoleDataProcessor;
}

//////////////////////////////////////////////////////////////////////////
IKG_Statistic *__stdcall KG_CreateStatistic()
{
    int nResult  = false;
    int nRetCode = false;
    KG_Statistic3 *piStatistic2 = NULL;

    piStatistic2 = new (nothrow) KG_Statistic3;
    KGLOG_PROCESS_ERROR(piStatistic2);

//Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_COM_RELEASE(piStatistic2);
    }
    return piStatistic2;
}

//////////////////////////////////////////////////////////////////////////
IKG_RoleProcessor *__stdcall KG_CreateRoleProcessor()
{
    int nResult  = false;
    int nRetCode = false;
    KG_RoleProcessor3 *piRoleProcessor2 = NULL;

    piRoleProcessor2 = new (nothrow) KG_RoleProcessor3;
    KGLOG_PROCESS_ERROR(piRoleProcessor2);

    //Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_COM_RELEASE(piRoleProcessor2);
    }
    return piRoleProcessor2;
}


////////////////////////////////////////////////////////////////////////////////
IKG_BishopProcessor *__stdcall KG_CreateBishopProcessor()
{
    int nResult  = false;
    int nRetCode = false;
    IKG_BishopProcessor *piBishopProcessor = NULL;

    piBishopProcessor = new (nothrow) KG_BishopProcessor3;
    KGLOG_PROCESS_ERROR(piBishopProcessor);

//Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_COM_RELEASE(piBishopProcessor);
    }
    return piBishopProcessor;
}

////////////////////////////////////////////////////////////////////////////
int __stdcall KG_AngelUnInit(void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;
    long lNewCount = 0;

    KG_USE_ARGUMENT(pvContext);

    lNewCount = KG_InterlockedDecrement(&gs_lInitRefCout);

    if (lNewCount < 0)
    {
        ASSERT(!"Unexpected call of KG_AngelUnInit()");
        KGLogPrintf(KGLOG_DEBUG, "Unexpected call of KG_AngelUnInit()");
        KGLOG_PROCESS_ERROR(false);
    }
    else if (lNewCount == 0)
    {
        KG_MemoryUnInit(NULL);
    }
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}
