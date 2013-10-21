#ifndef _KPAYSYSAGENCY_H_ 
#define _KPAYSYSAGENCY_H_ 

struct  KPlayerAgency;
class   KPlayerManager;
class   KRelayAgency;

class KPaysysAgency
{
public:
    KPaysysAgency();

    BOOL Init();
    void UnInit();

    void Activate();

    void AttachModule(KPlayerManager* pModule) { m_pPlayerManager = pModule; }
    void AttachModule(KRelayAgency* pModule) { m_pRelayAgency = pModule; }

    BOOL IsAvailable() { return (m_piSocket != NULL); }

    DWORD GetAccountVerifyTimeCost() { return (DWORD)m_fAccountVerifyTimeCost; }

private:
    BOOL ProcessPackage();
    BOOL ProcessCache();
    BOOL Send(IKG_Buffer* piBuffer);

    BOOL Connect(BOOL bNormalLogin);
    BOOL Disconnect();

private:
    BOOL GenerateMatrixPosition(KPlayerAgency* pPlayer);

private:
    BOOL        m_bRunFlag;
    KThread     m_WorkThread;
    int         ThreadFunction();
    static void WorkThreadFunction(void* pvParam); 

public:
    BOOL DoPingSignal();
    BOOL DoAccountVerifyRequest(KPlayerAgency* pPlayer);
    BOOL DoAccountVerifySDRequest(KPlayerAgency* pPlayer);
    BOOL DoMibaoVerifyRequest(KPlayerAgency* pPlayer, const char* pszMibaoPassword);
    BOOL DoAccountLogoutNotify(const char cszAcc[]);
    BOOL DoChangeExtPointsRequest(const char* pszAccountName, DWORD dwRoleID, WORD wExtPointIndex, WORD wChangeValue);
    BOOL DoSetChargeFlagRequest(const char* pszAccountName, DWORD dwRoleID, DWORD dwLastLoginIP, int nChargeFlag);
    BOOL DoActivePresentCodeRequest(const char* pszAccountName, DWORD dwRoleID, const char* pszPresentCode, DWORD dwActiveIP);
    BOOL DoFreezeCoinRequest(DWORD dwRequestID, const char* pszAccountName, BOOL bFreezeFlag, int nCoin);
    BOOL DoAccountExchangeRequest(
        DWORD dwRequestID, const char* pszSrcAccount, const char* pszDstAccount, DWORD dwExchangeType,
        DWORD dwDstAmount, DWORD dwSrcAmount
    );

private:
    typedef void (KPaysysAgency::*PP2G_PROTOCOL_FUNC)(BYTE* pbyData, size_t uDataLen);

    PP2G_PROTOCOL_FUNC m_PakProcessor[UCHAR_MAX + 1];
    size_t             m_uPakSize[UCHAR_MAX + 1];

private:
    void OnAccountVerifyRespond(BYTE* pbyData, size_t uDataLen);
    void OnMibaoVerifyRespond(BYTE* pbyData, size_t uDataLen);
    void OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen);
    void OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen);
    void OnGetZoneChargeFlagRespond(BYTE* pbyData, size_t uDataLen);
    void OnActivePresentCodeRespond(BYTE* pbyData, size_t uDataLen);
    void OnFreezeCoinRespond(BYTE* pbyData, size_t uDataLen);
    void OnAccountExchangeRespond(BYTE* pbyData, size_t uDataLen);

private:
    KPlayerManager*         m_pPlayerManager;
    KRelayAgency*           m_pRelayAgency;

    IKG_SocketStream*       m_piSocket;
    BOOL                    m_bSocketError;
    DWORD                   m_dwNextPingTime;
    float                   m_fAccountVerifyTimeCost;

    typedef std::list<IKG_Buffer*> KCacheTable;
    KCacheTable             m_Cache;

    char                    m_szIP[_NAME_LEN];
    int                     m_nPort;
    char                    m_szUserName[_NAME_LEN];
    char                    m_szPassword[_NAME_LEN];
    int                     m_nPingCycle;
    char                    m_szLocalIP[_NAME_LEN];
    char                    m_szInternalIPMask[_NAME_LEN];

public:
    BOOL                    m_bZoneChargeFlag;
};

#endif
