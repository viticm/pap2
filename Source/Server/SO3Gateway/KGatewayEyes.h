#ifndef _KGATEWAYEYES_H_
#define _KGATEWAYEYES_H_

#include "FSEyes/kipcstream.h"
#include "FSEyes/fseye_protocol.h"

class KPlayerManager;
class KPaysysAgency;
class KQueueManager;

class KGatewayEyes
{
public:
    KGatewayEyes();
    ~KGatewayEyes();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL GetQuitFlag() { return m_bQuitFlag; }

    void AttachModule(KPlayerManager* pModule) { m_pPlayerManager = pModule; }
    void AttachModule(KPaysysAgency* pModule) { m_pPaysysAgency = pModule; }
    void AttachModule(KQueueManager* pModule) { m_pQueueManager = pModule; }

private:
    KIpcStream      m_Stream;
    time_t          m_nNextPingTime;
    time_t          m_nNextSendPlayerCountTime;
    time_t          m_nNextSendCustomInfoTime;
    BOOL            m_bQuitFlag;

    KPlayerManager* m_pPlayerManager;
    KPaysysAgency*  m_pPaysysAgency;
    KQueueManager*  m_pQueueManager;

private:
    typedef void (KGatewayEyes::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);
    PROTOCOL_FUNCTION m_ProtocolFunctions[fseye_protocol_maxcount + 1];

    void OnE2LStop(BYTE* pbyData, size_t uDataLen);
    void OnE2LGetPid(BYTE* pbyData, size_t uDataLen);
    void OnG2LProtocol(BYTE* pbyData, size_t uDataLen);

private:
    void Ping();
    void SendPlayerCount();
    void SendCustomInfo();
};

#endif
