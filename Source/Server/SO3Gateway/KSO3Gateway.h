#ifndef _KSO3GATEWAY_H_
#define _KSO3GATEWAY_H_

#include "KPaysysAgency.h"
#include "KPlayerManager.h"
#include "KRelayAgency.h"
#include "KQueueManager.h"
#include "KGatewayEyes.h"
#include "KSndaAgency.h"

#define GATEWAY_CONFIG_FILE "gateway.ini"

class KSO3Gateway
{
public:
    KSO3Gateway();

    BOOL Init();
    void UnInit();

    void Run();

    void Quit() { m_bRunFlag = false; }

public:
    time_t              m_nTimeNow;
    BOOL                m_bSndaAgent;

private:
    BOOL                LoadConfig();

private:
    char                m_szLocale[64];
    BOOL                m_bRunFlag;
    KPaysysAgency       m_PaysysAgency;
    KRelayAgency        m_RelayAgency;
    KPlayerManager      m_PlayerManager;
    KQueueManager       m_QueueManager;
    KGatewayEyes        m_Eyes;
    KSndaAgency         m_SndaAgency;
};

#endif
