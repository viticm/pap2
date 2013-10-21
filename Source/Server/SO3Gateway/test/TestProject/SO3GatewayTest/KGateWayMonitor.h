///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KGateWayMonitor.h
// Creator  :   liuzhibiao
// Date     :   2008-11-14
// Comment  :   v1.0
//              v1.0    Create  
///////////////////////////////////////////////////////////////

#ifndef _KGATEWAYMONITOR_H_
#define _KGATEWAYMONITOR_H_

#include "FSEyes/kipcstream.h"
#include "FSEyes/fseye_protocol.h"

class KGateWayMonitor
{
public:
    int  Init();
    void UnInit();
    
    int GetQuitFlag() { return m_nQuitFlag; }

    int DoE2LStop();
    int DoE2LGetPid();
    int DoL2GProtocol();
    int GetWaitingQueuePlayerCount(int* pnWaitingQueuePlayer);
    int GetGatewayPid(int* pnGatewayPid);

private:
    int OnPing(BYTE* pbyData, size_t uDataLen);
    int OnL2EGetpid(BYTE* pbyData, size_t uDataLen);
    int OnL2GHeaderDef(BYTE* pbyData, size_t uDataLen);
    int RevCustomInfo(BYTE* pbyData, size_t uDataLen);
    int RevPlayCountInfo(BYTE* pbyData, size_t uDataLen);

    void Activate();

private:
    static void WorkThreadFunction(void *pvParam);
    int ThreadFunction();

    KIpcStream      m_Stream;
    time_t          m_nNextPingTime;
    time_t          m_nNextSendCustomInfoTime;
    int             m_nQuitFlag;
    int             m_nExitFlag;
    KThread         m_WorkThread;
    int             m_nConnectiontotal;
    DWORD           m_dwAccountVerifyTime;
    int             m_nWaitingQueuePlayer;
    int             m_nGatewayPid;

private:
    typedef int (KGateWayMonitor::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);
    PROTOCOL_FUNCTION m_ProtocolFunctions[fseye_protocol_maxcount + 1];

};

#endif  //_KGATEWAYMONITOR_H_

