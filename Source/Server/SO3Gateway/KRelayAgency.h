#ifndef _KRELAYAGENCY_H_ 
#define _KRELAYAGENCY_H_ 

#include "GatewayRelayProtocol.h"

class KPlayerManager;
class KPaysysAgency;
class KSO3Gateway;
class KQueueManager;
class KSndaAgency;

class KRelayAgency 
{
public:
    KRelayAgency();

    BOOL Init();
    void UnInit();

    void Activate();

private:
    BOOL ProcessPackage();
    BOOL Send(IKG_Buffer* piBuffer);
    
public:
    BOOL GetGameWorldVersion(int* pnLowerVersion, int* pnUpperVersion);

    BOOL IsAccountBlocked(const char cszAcc[]);

    void AttachModule(KPlayerManager* pModule) { m_pPlayerManager = pModule; }
    void AttachModule(KPaysysAgency* pModule) { m_pPaysysAgency = pModule; }
    void AttachModule(KSO3Gateway* pModule) { m_pGateway = pModule; }
    void AttachModule(KQueueManager* pModule) { m_pQueueManager = pModule; }
    void AttachModule(KSndaAgency* pModule) { m_pSndaAgency = pModule; }

public:
    BOOL DoPingSignal();
    BOOL DoQueryAccountState(KPlayerAgency* pPlayer);
    BOOL DoKickAccountNotify(KPlayerAgency* pPlayer);
    BOOL DoQueryRoleList(KPlayerAgency* pPlayer);
    BOOL DoQueryNewbieMaps(int nPlayerIndex);
    BOOL DoCreateRoleRequest(int nPlayerIndex, const char cszAcc[], BYTE* pbyData, size_t uDataLen);
    BOOL DoDeleteRoleRequest(KPlayerAgency* pPlayer, DWORD dwRoleID);
    BOOL DoGameLoginRequest(KPlayerAgency* pPlayer, DWORD dwRoleID, BYTE* pbyData, size_t uDataLen);
    BOOL DoChangeExtPointRespond(BOOL bSucceed, DWORD dwRoleID);
    BOOL DoSetChargeFlagRespond(
        BOOL bSucceed, DWORD dwRoleID, int nChargeFlag, const tagExtPointInfo& rExtPointInfo,
        DWORD dwEndTimeOfFee
    );
    BOOL DoGetZoneChargeFlagNotify(int nZoneChargeFlag);
    BOOL DoActivePresentCodeRespond(BOOL bSucceed, DWORD dwRoleID, const char* pszPresentCode, DWORD dwPresentType);
    BOOL DoFreezeCoinRespond(BOOL bSucceed, DWORD dwRequestID, BOOL bFreezeFlag, int nCoin);
    BOOL DoAccountExchangeRespond(BOOL bSucceed, DWORD dwRequestID);
    BOOL DoRenameRequest(int nPlayerIndex, DWORD dwRoleID, char szNewName[]);

private:
    void OnPingSignal(BYTE* pbyData, size_t uDataLen);
    void OnSetGameWorldVersion(BYTE* pbyData, size_t uDataLen);
    void OnSyncAccountState(BYTE* pbyData, size_t uDataLen);
    void OnSyncRoleList(BYTE* pbyData, size_t uDataLen);
    void OnCreateRoleRespond(BYTE* pbyData, size_t uDataLen);
    void OnDeleteRoleRespond(BYTE* pbyData, size_t uDataLen);
    void OnGameLoginRespond(BYTE* pbyData, size_t uDataLen);
    void OnSyncNewbieMaps(BYTE* pbyData, size_t uDataLen);
    void OnPlayerLeaveGame(BYTE* pbyData, size_t uDataLen);
    void OnChangeExtPointRequest(BYTE* pbyData, size_t uDataLen);
    void OnSetChargeFlagRequest(BYTE* pbyData, size_t uDataLen);
    void OnActivePresentCodeRequest(BYTE* pbyData, size_t uDataLen);
    void OnFreezeCoinRequest(BYTE* pbyData, size_t uDataLen);
    void OnAccountExchangeRequest(BYTE* pbyData, size_t uDataLen);
    void OnRenameRespond(BYTE* pbyData, size_t uDataLen);

private:
    int     m_nGameWorldLowerVersion;
    int     m_nGameWorldUpperVersion;
    DWORD   m_dwNextPingTime;
    int     m_nPingCycle;

    KPlayerManager* m_pPlayerManager;
    KPaysysAgency*  m_pPaysysAgency;
    KSO3Gateway*    m_pGateway;
    KQueueManager*  m_pQueueManager;
    KSndaAgency*    m_pSndaAgency;

private:
    typedef void (KRelayAgency::*PR2G_PROTOCOL_FUNC)(BYTE* pbyData, size_t uDataLen);

    PR2G_PROTOCOL_FUNC m_PakProcessor[r2g_protocol_total];
    size_t             m_uPakSize[r2g_protocol_total];

    IKG_SocketStream*   m_piSocket;
    BOOL                m_bSocketError;
};

#endif
