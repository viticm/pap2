///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSimulateClient.h
// Creator  :   liuzhibiao
// Date     :   2008-10-20
// Comment  :   v1.0
//              2008.08.15	Create0.1
//              2008-9-4      V0.9    优化
//              v0.95   2008-9-22 改变数据结构
//              V1.0    2008-10-20  多客户端
///////////////////////////////////////////////////////////////

#ifndef _KSIMULATECLIENT_H_
#define _KSIMULATECLIENT_H_

#include "KG_SO3GateWayTestDef.h"
#include "GatewayClientProtocol.h"
#include <stdarg.h>

enum OPERATE_STATE
{
    osStart,
    osNotReady,
    osReady,
    osDone,
    osTotal
};

enum OPERATE_NUM
{
    onStart,
    onConnectToGateway,
    onHandShake,
    onAccountVerify,
    onMibaoVerify,
    onCreateRole,
    onDeleteRole,
    onLoginGame,
    onQueryNewbieMaps,
    onGiveUpQueue,
    onDisConnect,
    onTotal
};

class KSimulateClient
{
public:
    int    Init(const KG_SIMULATE_CLIENT_CONFIG& crSimulateClientConfig, int nTestPoint);
    void   UnInit();    
    int    Run();
    int    FillOperate(int nOperateCount, int nOperate...);
    int    FillOperateToOperateList(int nOperateNum, int nOperateState);
    int    GetReturnResult(KG_RETURN_RESULT* pResult);
    int    CheckTheAccountLockedOrNot(int* pnRevLockAccountRespondState);
    int    ModifyUserName(char szNewName[]);
    int    GetConfig(KG_SIMULATE_CLIENT_CONFIG* pClientConfig);
    int    TaskIsCompleted(int* pnTaskState);

    static int m_nIndex;
private:
    int    ProcessConnectGateway();
    int    ProcessPackage();
    int    DoConnectToGateWay();
    int    DoPingSignal();
    int    DoHandshakeRequest();
    int    DoAccountVerifyRequest();
    int    DoQueryNewbieMaps();
    int    DoCreateRoleRequest();
    int    DoDeleteRoleRequest();
    int    DoLoginGameRequest();
    int    DoGiveupQueueRequest();
    int    DoDisconnect();
    int    DoMibaoVerifyRequest();

private:
    typedef int (KSimulateClient::*PROTOCOL_FUNCTION)(
        BYTE* pbyData, 
        size_t uDataLen
    );

    PROTOCOL_FUNCTION m_ProtocolFunctions[g2c_protocol_total];
    size_t            m_uPakSize[g2c_protocol_total];

    typedef int (KSimulateClient::*USER_OPERATE_FUNCTION)();

    USER_OPERATE_FUNCTION m_UserOperateFunctions[KG_GATEWAY_RECONNECT + 1];

    enum CONNECT_STATE
    {
        csInvalid,
        csWaitingConnectOK
    };
    CONNECT_STATE m_ConnectState;

private:
    int OnPingRespond(BYTE* pbyData, size_t uDataLen);
    int OnHandshakeRespond(BYTE* pbyData, size_t uDataLen);
    int OnAccountVerifyRespond(BYTE* pbyData, size_t uDataLen);
    int OnSyncRoleList(BYTE* pbyData, size_t uDataLen);
    int OnSyncNewbieMaps(BYTE* pbyData, size_t uDataLen);
    int OnCreateRoleRespond(BYTE* pbyData, size_t uDataLen);
    int OnDeleteRoleRespond(BYTE* pbyData, size_t uDataLen);
    int OnSyncLoginKey(BYTE* pbyData, size_t uDataLen);
    int OnKickAccountNotify(BYTE* pbyData, size_t uDataLen);
    int OnAccountLockedNotify(BYTE* pbyData, size_t uDataLen);
    int OnGiveupQueueRespond(BYTE* pbyData, size_t uDataLen);
    int OnSyncQueueState(BYTE* pbyData, size_t uDataLen);
    int OnMibaoVerifyRespond(BYTE* pbyData, size_t uDataLen);

private:   
    static void RevWorkThreadFunction(void *pvParam);
    int RevThreadFunction();

    static void SendWorkThreadFunction(void *pvParam);
    int SendThreadFunction(KSimulateClient* pClient);

private:
    int                             m_nNumber;
    IKG_SocketStream*               m_piSocketStream;
    time_t                          m_nLastPingTime;
    KG_SIMULATE_CLIENT_CONFIG       m_ClientConfig;

    int                             m_nRevExitFlag;
    KThread                         m_RevWorkThread;
    int                             m_nRevWorkThreadFlag;

    int                             m_nSendExitFlag;
    KThread                         m_SendWorkThread;
    int                             m_nSendWorkThreadFlag;

    KG_RETURN_RESULT                m_RetForecastResult;
    int                             m_nHadRevLockAccountRespond;

    typedef std::vector<pair<int, USER_OPERATE_FUNCTION> > KUserOperateFunctionList;
    KUserOperateFunctionList        m_UserOperateFunctionList;
    int                             m_nOperateCount;
    int                             m_nNowDoingOperateNum;

    int                             m_nTimeOutLoop;
    int                             m_nHadCompleteTask;     // 当m_ClientConfig中的nNeedStopWhenRevAppointInfo为true时才起作用

    KMutex                          m_Mutex;
};

#endif  // _KSIMULATECLIENT_H_

