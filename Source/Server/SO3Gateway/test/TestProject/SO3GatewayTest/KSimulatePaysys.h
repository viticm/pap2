///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSimulatePaysys.h
// Creator  :   liuzhibiao
// Date     :   2008-10-8
// Comment  :   v0.98   
//              V0.9    2008-9-22 改变数据结构
//              v0.98   2008-10-8 改为多连接
///////////////////////////////////////////////////////////////

#ifndef _KSIMULATEPAYSYS_H_ 
#define _KSIMULATEPAYSYS_H_ 

#include "KG_SO3GateWayTestDef.h"
#include "Protocol/AccountLoginDef.h"
#include "Protocol/PaysysGamewordProtocal.h"

enum KGatewayAccountState
{
    easGWInvalid, 
    easGWWaitForGatewayAccountPassword,          // 等待Gateway传来用户名,密码
    easGWWaitForDisconnect,                      // 等待Gateway主动断线
    easGWTotal
};

struct KGatewayAgency
{
    char                          szAccount[LOGIN_USER_ACCOUNT_MAX_LEN];
    char                          szPsw[LOGIN_USER_PASSWORD_MAX_LEN];

    KGatewayAccountState          nState;
    int                           nIndex;
    IKG_SocketStream*             piSocket;
    time_t                        nLastPingTime;
    int                           nHadRevUserLoginOut;
    int                           nSameGatewayConnectCount;
};

class KSimulatePaysys
{
public:
    int  Init(const KG_SIMULATE_PAYSYS_CONFIG& crSimulatePaysysConfig);
    int  HadRevUserLoginOut(char szClientPlayerAccountName[], int* pnHadRevUserLoginOut);
    void UnInit();
    void Activate();
    int  AllDisConnected(int nCloseThread);
    int  StartWork();
    int  GetGatewayConnectCount(char szGatewayAccountName[], int* pnConnectCount);
    int  HadConnections(int* pnHadConnected);
    int  SetAccountVerifyRespondType(WORD wType, BYTE byMibaoMode, int nUserLoginResult);
    int  SetNotHandleProtocol(int nNotHandleProtocol);

    KGatewayAgency* GetGateway(int nIndex)
    {
        KGatewayTable::iterator it = m_GatewayTable.find(nIndex);
        if (it != m_GatewayTable.end())
        {
            return &it->second;
        }
        return NULL;
    }

public:
    int ProcessPackage(IKG_SocketStream* piSocket);
    int ProcessNewConnection(IKG_SocketStream* piSocket);
public:
    int DoAccountVerifyRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int DoSDAccountVerifyRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int DoGatewayVerifyRespond(KGatewayAgency* pGateway, int nLoginResult);
    int DoGatewayReVerityRespond(KGatewayAgency* pGateway, int nLoginResult);
    int DoPingRespond(KGatewayAgency* pGateway);
    int DoSetChargeFlagRespond(KGatewayAgency* pGateway);
    int DoGetZoneChargeFlagRespond(KGatewayAgency* pGateway);
    int DoActivePresentCodeRespond(KGatewayAgency* pGateway, char* szAccount, char* pPresentCode, DWORD dwRoleID, int nPCResult);
    int DoChangeExtPointRespond(KGatewayAgency* pGateway);
    int DoMiBaoVerifyRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int DoFreezeCoinRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int DoAccountExchangeRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);

    //方便测试用:
    int m_nRevChangeExtPointRequest;
    int m_nRevActivePresentCodeRequest;
    int m_nPlayerSetChargeFlagRequest;
    
    int m_nFreezeCoinRequest;
    int m_nAccountExchangeRequest;

private:   
    static void WorkThreadFunction(void *pvParam);
    int ThreadFunction();

private:
    typedef int (KSimulatePaysys::*PROTOCOL_FUNCTION)(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);

    PROTOCOL_FUNCTION m_ProtocolFunctions[UCHAR_MAX + 1];
    size_t            m_uPakSize[UCHAR_MAX + 1];

private:
    
    //int OnDoMibaoVerifyRequest(KGatewayAgency* pGateway, const char* pszMibaoPassword);//new
    //int OnChangeExtPointsRequest(const char* pszAccountName, DWORD dwRoleID, WORD wExtPointIndex, WORD wChangeValue);//new
    //int OnSetChargeFlagRequest(const char* pszAccountName, DWORD dwRoleID, DWORD dwLastLoginIP, int nChargeFlag);//new
    int OnMiBaoVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnAccountVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnSDAccountVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnPlayerLeaveGame(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnGatewayVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnGateWayReVerityRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnGateWayLogout(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnPingRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    void OnDisconnect(KGatewayAgency* pGateway);
    int OnPlayerGetZoneChargeFlagRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen);//new
    int OnPlayerSetChargeFlagRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen);//new
    int OnChangeExtPointsRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen);//new
    int OnActivePresentCodeRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen);//new
    void CheckConnectionTimeout();
    void RecycleConnections();
    int  CheckLockGatewayTable(
        char szGatewayAccountName[], 
        int nKeepTheGatewayCount, 
        int* pnPreGatewayCount
    );

    int OnFreezeCoinRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);
    int OnAccountExchangeRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen);

    KGatewayAgency* GetGateway(char szGatewayAccountName[]);

private:
    typedef std::map<int, KGatewayAgency> KGatewayTable;
    KGatewayTable                   m_GatewayTable;

    struct KAccountCompareLess
    {
        BOOL operator()(const string& crX, const string& crY) const
        {
            return STR_CASE_CMP(crX.c_str(), crY.c_str()) < 0;
        }
    };

    typedef std::set<string, KAccountCompareLess> KLockedGatewayTable;
    KLockedGatewayTable             m_LockedGatewayTable;

    typedef std::map<string, int> KClientPlayerAccountTable;
    KClientPlayerAccountTable       m_ClientPlayerAccountTable;

    int                             m_nNextGatewayIndex;
    int                             m_nMaxGateway;
    KG_SocketServerAcceptor*        m_pSockerServer;
    KG_SOCKET_EVENT*                m_pSocketEvents;
    KG_SIMULATE_PAYSYS_CONFIG       m_PaysysConfig;
    int                             m_nExitFlag;
    KThread                         m_WorkThread;
    int                             m_nWorkThreadFlag;
    time_t                          m_nTimeNow;
    time_t                          m_nNextPingCheck;
    int                             m_nPingCycle;
    int                             m_bRecycleConnections;
    int                             m_nHadConnections;
};

#endif // _KSIMULATEPAYSYS_H_
