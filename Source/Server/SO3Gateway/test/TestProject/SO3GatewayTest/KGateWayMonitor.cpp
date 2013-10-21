///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KGateWayMonitor.cpp
// Creator  :   liuzhibiao
// Date     :   2008-11-14
// Comment  :   v1.0
//              v1.0    Create  
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGateWayMonitor.h"

#define REGISTER_GWMONITOR_FUNCTION(id, func)              \
    do                                                  \
    {                                                   \
    m_ProtocolFunctions[id] = &KGateWayMonitor::func;  \
    } while(false)

int KGateWayMonitor::Init()
{
    int    nResult          = false;
    int    nRetCode         = false;
    int    nWorkThreadFlag  = false;

    m_nConnectiontotal      = 0;
    m_dwAccountVerifyTime   = 0;
    m_nWaitingQueuePlayer   = 0;
    m_nGatewayPid           = 0;
    
    nRetCode = m_Stream.Initialize(emKGUARDSERVER_FIRSTID + emKGUARDSERVER_BISHOP, 200 * 1024, true);
    KG_PROCESS_ERROR(nRetCode);

    REGISTER_GWMONITOR_FUNCTION(l2e_ping_def, OnPing);
    REGISTER_GWMONITOR_FUNCTION(l2e_getpid_def, OnL2EGetpid);
    REGISTER_GWMONITOR_FUNCTION(l2e_update_custom_info_def, RevCustomInfo);
    REGISTER_GWMONITOR_FUNCTION(l2e_PlayerCount_def, RevPlayCountInfo);
    REGISTER_GWMONITOR_FUNCTION(l2e_header_def,OnL2GHeaderDef);

    m_nExitFlag = false;
    nRetCode = m_WorkThread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    nWorkThreadFlag = true;

    nResult = true;
Exit0:
    if (!nResult)
    {
        m_nExitFlag = true;

        if (nWorkThreadFlag)
        {
            m_WorkThread.Destroy();
            nWorkThreadFlag = false;
        }
    }
    return nResult;
}

void KGateWayMonitor::UnInit()
{
    m_nExitFlag = true;
    m_WorkThread.Destroy(); 
    m_Stream.UnInitialize();
}

int KGateWayMonitor::DoE2LStop()
{
    int    nResult  = false;
    //int    nRetCode = false;
    KProtocolHead ptl;

    ptl.wProtocol   = e2l_exit_def;
    ptl.wServer     = 0;

    m_Stream.PutPack(&ptl, sizeof(KProtocolHead));

    nResult = true;
//Exit0:
    return nResult;
}

void KGateWayMonitor::Activate()
{
    BYTE*               pbyData             = NULL;
    size_t              nPackLen            = 0;
    BYTE                byProtocol          = 0;
    PROTOCOL_FUNCTION   ProtocolFunction    = NULL;
//    time_t              nTimeNow            = time(NULL);   

    while (true)
    {
        pbyData = (BYTE*)m_Stream.GetPack(nPackLen);

        if (!pbyData)
        {
            break;
        }

        byProtocol       = *pbyData;
        ProtocolFunction = m_ProtocolFunctions[byProtocol];

        if (ProtocolFunction)
        {
            (this->*ProtocolFunction)(pbyData, nPackLen);
        }
        else
        {
            printf("Invalid FSEyes protocol : %d\n", byProtocol);
        }

        m_Stream.PopPack();
    }
}

int KGateWayMonitor::OnPing(BYTE* pbyData, size_t uDataLen)
{
    int                 nResult  = false;
//    int    nRetCode = false;
    KSubProtocolHead*   sPing    = (KSubProtocolHead*)pbyData;

    KGLOG_PROCESS_ERROR(sPing->wSubProtocol == l2e_ping_def);

    nResult = true;
Exit0:
    return nResult;
}

void KGateWayMonitor::WorkThreadFunction(void *pvParam)
{
    KGateWayMonitor *pThis = (KGateWayMonitor *)pvParam;

    ASSERT(pThis);

    pThis->ThreadFunction();
}

int KGateWayMonitor::ThreadFunction()
{
    int nResult  = false;    
    //int nRetCode = false;

    while (!m_nExitFlag)
    {
        Activate();
        KGThread_Sleep(10);
    }  

    nResult = true;
    //Exit0:
    return nResult;
}

#define MAX_CUSTOM_INFO_PAK_E 1024

/*
-------------------------------------
Connection total ... .... %d
Waiting queue ... ... ... %d
Account verify respond .. %u ms
-------------------------------------
*/

int KGateWayMonitor::RevCustomInfo(BYTE* pbyData, size_t uDataLen)
{
    int                        nResult                      = false;
//    int                        nRetCode                     = false;
    size_t                     uStrLen                      = 0;
    l2e_update_custom_info*    sCustomInfo                  = (l2e_update_custom_info*)pbyData;
    char*                      pszCustomInfo                = NULL;
    int                        nConnectiontotal             = 0;
    DWORD                      dwAccountVerifyTime          = 0;
    int                        nWaitingQueuePlayer          = 0;
    char                       szConnectTotle[]             = "-------------------------------------\nConnection total ... ....";
    char                       szWaitQueue[]                = "Waiting queue ... ... ...";
    char                       szAccountVerifyTime[]        = "Account verify respond ..";
    int                        nConnectTotleStrBeginPos     = 0;
    int                        nWaitQueueBeginPos           = 0;
    int                        nAccountVerifyTimeBeginPos   = 0;
    char*                      pszBegin                     = NULL;
    char                       szTemp[MAX_CUSTOM_INFO_PAK_E]                  ;

    KGLOG_PROCESS_ERROR(sCustomInfo->wSubProtocol == l2e_update_custom_info_def);  
    pszCustomInfo = (char*)sCustomInfo->byData;
    
    nConnectTotleStrBeginPos    = sizeof(szConnectTotle);
    nWaitQueueBeginPos          = sizeof(szWaitQueue);
    nAccountVerifyTimeBeginPos  = sizeof(szAccountVerifyTime);
    
    pszBegin = pszCustomInfo + nConnectTotleStrBeginPos;
    sscanf(pszBegin, "%d", &nConnectiontotal);
    uStrLen = snprintf(szTemp, MAX_CUSTOM_INFO_PAK_E, "-------------------------------------\nConnection total ... .... %d\n", nConnectiontotal);
    KGLOG_PROCESS_ERROR(uStrLen > 0);
    KGLOG_PROCESS_ERROR(uStrLen < MAX_CUSTOM_INFO_PAK_E);

    nWaitQueueBeginPos  = nWaitQueueBeginPos + (int)strlen(szTemp);
    pszBegin            = pszCustomInfo + nWaitQueueBeginPos;
    sscanf(pszBegin, "%d", &nWaitingQueuePlayer);

    uStrLen = snprintf(szTemp, MAX_CUSTOM_INFO_PAK_E, "-------------------------------------\nConnection total ... .... %d\nWaiting queue ... ... ... %d\n", nConnectiontotal, nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(uStrLen > 0);
    KGLOG_PROCESS_ERROR(uStrLen < MAX_CUSTOM_INFO_PAK_E);

    nAccountVerifyTimeBeginPos  = nAccountVerifyTimeBeginPos + (int)strlen(szTemp);
    pszBegin                    = pszCustomInfo + nAccountVerifyTimeBeginPos;
    sscanf(pszBegin, "%u", &dwAccountVerifyTime);

    m_nConnectiontotal      = nConnectiontotal;
    m_nWaitingQueuePlayer   = nWaitingQueuePlayer;
    m_dwAccountVerifyTime   = dwAccountVerifyTime;

    nResult = true;
Exit0:
    return nResult;
}

int KGateWayMonitor::RevPlayCountInfo(BYTE* pbyData, size_t uDataLen)
{
    int                        nResult                      = false;
    //int                        nRetCode                     = false;

    nResult = true;
//Exit0:
    return nResult;

}

int KGateWayMonitor::GetWaitingQueuePlayerCount(int* pnWaitingQueuePlayer)
{
    int  nResult   = false;
    //int  nRetCode  = false;

    KGLOG_PROCESS_ERROR(pnWaitingQueuePlayer);

    *pnWaitingQueuePlayer = m_nWaitingQueuePlayer;

    nResult = true;
Exit0:
    return nResult;
}

int KGateWayMonitor::DoE2LGetPid()
{
    int    nResult  = false;
    //int    nRetCode = false;
    KProtocolHead ptl;

    ptl.wProtocol   = e2l_getpid_def;
    ptl.wServer     = 0;

    m_Stream.PutPack(&ptl, sizeof(KProtocolHead));

    nResult = true;
    //Exit0:
    return nResult;
}

int KGateWayMonitor::OnL2EGetpid(BYTE* pbyData, size_t uDataLen)
{
    int                 nResult  = false;
    //    int    nRetCode = false;
    l2e_getpid*   psMsg    = (l2e_getpid*)pbyData;

    KGLOG_PROCESS_ERROR(psMsg->wProtocol == l2e_getpid_def);

    m_nGatewayPid = psMsg->nPid;

    nResult = true;
Exit0:
    return nResult;
}

int KGateWayMonitor::GetGatewayPid(int* pnGatewayPid)
{
    int                 nResult  = false;
    //    int    nRetCode = false;
    
    KGLOG_PROCESS_ERROR(pnGatewayPid);

    *pnGatewayPid = m_nGatewayPid;

    nResult = true;
Exit0:
    return nResult;
}

int KGateWayMonitor::OnL2GHeaderDef(BYTE* pbyData, size_t uDataLen)
{
    int                 nResult  = false;
    //    int    nRetCode = false;
    PROTOCOL_FUNCTION   ProtocolFunction    = NULL;
    KSubProtocolHead* pProtocolHead = (KSubProtocolHead*)pbyData;
    KGLOG_PROCESS_ERROR(pProtocolHead->wProtocol == l2e_header_def);
    
    ProtocolFunction = m_ProtocolFunctions[pProtocolHead->wSubProtocol];

    if (ProtocolFunction)
    {
        (this->*ProtocolFunction)(pbyData, uDataLen);
    }
    else
    {
        printf("Invalid FSEyes SubProtocol : %d\n", pProtocolHead->wSubProtocol);
    }

    nResult = true;
Exit0:
    return nResult;
}
