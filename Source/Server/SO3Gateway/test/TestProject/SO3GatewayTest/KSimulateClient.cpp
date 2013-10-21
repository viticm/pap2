///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KSimulateClient.cpp
// Creator  : liuzhibiao
// Date     : 2008-9-22
// Comment  : v0.97
//            v0.1  2008.08.15	Create  	
//            V0.96 2008年9月18日 优化
//            V0.97 2008-9-22 改变数据结构
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_SO3GateWayTestDef.h"
#include "KSimulateClient.h"
#include "KRoleDBDataDef.h"

#define REGISTER_G2C_FUNCTION(id, func, data)               \
    do                                                      \
    {                                                       \
    m_ProtocolFunctions[id] = &KSimulateClient::func;    \
    m_uPakSize[id]          = sizeof(data);             \
    } while(false)

int KSimulateClient::m_nIndex = 0;

int KSimulateClient::Init(const KG_SIMULATE_CLIENT_CONFIG& crSimulateClientConfig, int nTestPoint)
{
    int         nResult         = false;
    //int         nRetCode        = false;

    m_nNumber                   = m_nIndex; 
    m_nHadCompleteTask          = false;
    m_nNowDoingOperateNum       = 0;
    m_nOperateCount             = 0;
    m_piSocketStream            = NULL;
    m_nRevWorkThreadFlag        = false;
    m_nRevExitFlag              = true;
    m_nSendWorkThreadFlag       = false;
    m_nSendExitFlag             = true;
    m_nHadRevLockAccountRespond = false;
    m_nTimeOutLoop              = MAX_RECV_LOOP_TIMES;
    m_nIndex++;

    memcpy(&m_ClientConfig, &crSimulateClientConfig, sizeof(m_ClientConfig));
    ZeroMemory(&m_RetForecastResult, sizeof(m_RetForecastResult));
    m_RetForecastResult.nTestPoint = nTestPoint;

    memset(m_ProtocolFunctions, 0, sizeof(m_ProtocolFunctions));
    memset(m_uPakSize, 0, sizeof(m_uPakSize));

    REGISTER_G2C_FUNCTION(g2c_ping_respond, OnPingRespond, G2C_PING_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_handshake_respond, OnHandshakeRespond, G2C_HANDSHAKE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_account_verify_respond, OnAccountVerifyRespond, G2C_ACCOUNT_VERIFY_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_sync_role_list, OnSyncRoleList, G2C_SYNC_ROLE_LIST);
    REGISTER_G2C_FUNCTION(g2c_sync_newbie_maps, OnSyncNewbieMaps, G2C_SYNC_NEWBIE_MAPS);
    REGISTER_G2C_FUNCTION(g2c_create_role_respond, OnCreateRoleRespond, G2C_CREATE_ROLE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_delete_role_respond, OnDeleteRoleRespond, G2C_DELETE_ROLE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_sync_login_key, OnSyncLoginKey, G2C_SYNC_LOGIN_KEY);
    REGISTER_G2C_FUNCTION(g2c_kick_account_notify, OnKickAccountNotify, G2C_KICK_ACCOUNT_NOTIFY);
    REGISTER_G2C_FUNCTION(g2c_account_locked_notify, OnAccountLockedNotify, G2C_ACCOUNT_LOCKED_NOTIFY);
    REGISTER_G2C_FUNCTION(g2c_sync_queue_state, OnSyncQueueState, G2C_SYNC_QUEUE_STATE);
    REGISTER_G2C_FUNCTION(g2c_giveup_queue_respond, OnGiveupQueueRespond, G2C_GIVEUP_QUEUE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_mibao_verify_respond, OnMibaoVerifyRespond, G2C_MIBAO_VERIFY_RESPOND);

    m_UserOperateFunctions[KG_ROLE_ENTER_GAME]  = &KSimulateClient::DoLoginGameRequest;
    m_UserOperateFunctions[KG_CREATE_ROLE]      = &KSimulateClient::DoCreateRoleRequest;
    m_UserOperateFunctions[KG_DELETE_ROLE]      = &KSimulateClient::DoDeleteRoleRequest;
    m_UserOperateFunctions[KG_HOMETOWN_LIST]    = &KSimulateClient::DoQueryNewbieMaps;
 
    nResult = true;
//Exit0:
    return nResult;
}

void KSimulateClient::UnInit()
{
    if (m_nRevWorkThreadFlag)
    {
        m_nRevExitFlag = true;
        m_RevWorkThread.Destroy();
        m_nRevWorkThreadFlag = false;
    }

    if (m_nSendWorkThreadFlag)
    {
        m_nSendExitFlag = true;
        m_SendWorkThread.Destroy();
        m_nSendWorkThreadFlag = false;
    }

    m_UserOperateFunctionList.clear();

    KG_COM_RELEASE(m_piSocketStream);
}

int KSimulateClient::ProcessConnectGateway()
{
    int     nResult     = false;
    int     nRetCode    = false;
    timeval Timeout     = {0, 0};
    
    KG_PROCESS_SUCCESS(m_piSocketStream == NULL);
    KG_PROCESS_SUCCESS(m_ConnectState != csWaitingConnectOK);

    nRetCode = m_piSocketStream->CheckCanSend(&Timeout);
    KGLOG_PROCESS_ERROR(nRetCode >= 0);

    KG_PROCESS_SUCCESS(nRetCode == 0); // time out

    nRetCode = DoHandshakeRequest();
    KGLOG_PROCESS_ERROR(nRetCode);

    m_ConnectState = csInvalid;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        m_ConnectState = csInvalid;
        KG_COM_RELEASE(m_piSocketStream);
    }
    return nResult;
}

int KSimulateClient::DoHandshakeRequest()
{
    int                        nResult         = false;
    int                        nRetCode        = false;
    IKG_Buffer*                piSendBuffer    = NULL;
    C2G_HANDSHAKE_REQUEST*     pRequest        = NULL;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_HANDSHAKE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_HANDSHAKE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol        = c2g_handshake_request;
    pRequest->nGameWorldVersion = m_ClientConfig.nClientGameWorldVersion;
    pRequest->nGatewayVersion   = m_ClientConfig.nClientGatewayVersion;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    
    pInsRet->first = osDone;
    m_nNowDoingOperateNum++;
    
    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoHandshakeRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoPingSignal()
{
    int              nResult      = false;
    int              nRetCode     = 0;
    time_t           nNowTime     = 0;
    IKG_Buffer*      piSendBuffer = NULL;
    C2G_PING_SIGNAL* pPing        = NULL;

    KG_PROCESS_ERROR(m_piSocketStream != NULL);
    nNowTime = time(NULL);

    KG_PROCESS_ERROR(nNowTime > m_nLastPingTime + 8);
    m_nLastPingTime = nNowTime;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(C2G_PING_SIGNAL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPing = (C2G_PING_SIGNAL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPing);

    pPing->byProtocol = c2g_ping_signal;
    pPing->dwTime     = (DWORD)nNowTime;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoAccountVerifyRequest()
{
    int                          nResult       = false;
    int                          nRetCode      = false;
    IKG_Buffer*                  piSendBuffer  = NULL;
    C2G_ACCOUNT_VERIFY_REQUEST*  pRequest      = NULL;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_ACCOUNT_VERIFY_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_ACCOUNT_VERIFY_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol  = c2g_account_verify_request;

    strncpy(
        pRequest->szAcc, 
        m_ClientConfig.szClientAccountName, 
        sizeof(pRequest->szAcc)
    );
    pRequest->szAcc[sizeof(pRequest->szAcc) - 1] = '\0';

    strncpy(
        pRequest->szPsw, 
        m_ClientConfig.szClientAccountPassword.szPassword, 
        sizeof(pRequest->szPsw)
    );
    pRequest->szPsw[sizeof(pRequest->szPsw) - 1] = '\0';

    strncpy(
        pRequest->szAct, 
        m_ClientConfig.szClientActivateCode,
        sizeof(pRequest->szAct)
    );
    pRequest->szAct[sizeof(pRequest->szAct) - 1] = '\0';

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pInsRet->first = osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoAccountVerifyRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoCreateRoleRequest()
{
    int                         nResult          = false;
    int                         nRetCode         = false;
    IKG_Buffer*                 piSendBuffer     = NULL;
    C2G_CREATE_ROLE_REQUEST*    pRequest         = NULL;
    KUSER_ROLE_GENERATOR_PARAM* pRoleCreateParam = NULL;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_CREATE_ROLE_REQUEST)  + sizeof(KUSER_ROLE_GENERATOR_PARAM));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_CREATE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = c2g_create_role_request;

    pRoleCreateParam = (KUSER_ROLE_GENERATOR_PARAM*)pRequest->byData;

    strncpy(pRoleCreateParam->szRoleName, "角色名", sizeof(pRoleCreateParam->szRoleName));
    pRoleCreateParam->szRoleName[sizeof(pRoleCreateParam->szRoleName) - 1] = '\0';

    pRoleCreateParam->cRoleType     = 1;
    pRoleCreateParam->dwMapID       = 0;
    pRoleCreateParam->nMapCopyIndex = 0;

    memset(pRoleCreateParam->wRepresentId, 0, sizeof(pRoleCreateParam->wRepresentId));

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pInsRet->first = osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoCreateRoleRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoDeleteRoleRequest()
{
    int                         nResult          = false;
    int                         nRetCode         = false;
    C2G_DELETE_ROLE_REQUEST*    pDeleteRole      = NULL;
    IKG_Buffer*                 piSendBuffer     = NULL;
    DWORD                       dwRoleID         = m_ClientConfig.nDelRoleID;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_DELETE_ROLE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDeleteRole = (C2G_DELETE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDeleteRole);

    pDeleteRole->byProtocol = c2g_delete_role_request;
    pDeleteRole->dwRoleID   = dwRoleID;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pInsRet->first = osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoDeleteRoleRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoLoginGameRequest()
{
    int                         nResult      = false;
    int                         nRetCode     = 0;
    C2G_GAME_LOGIN_REQUEST*     pRoleLogin   = NULL;
    IKG_Buffer*                 piSendBuffer = NULL;
    DWORD                       dwRoleID     = 0;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_GAME_LOGIN_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRoleLogin = (C2G_GAME_LOGIN_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRoleLogin);

    pRoleLogin->byProtocol  = c2g_game_login_request;
    pRoleLogin->dwRoleID    = dwRoleID;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pInsRet->first = osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoLoginGameRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoConnectToGateWay()
{
    int                     nResult         = false;
    int                     nRetCode        = false;
    timeval                 TimeoutValue    = {60, 0};
    KG_SocketConnector      Connector;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KG_PROCESS_ERROR(m_piSocketStream == NULL);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    m_piSocketStream = Connector.ConnectSecurity(
        m_ClientConfig.szConnectIPAddr, 
        m_ClientConfig.nConnectPort, 
        KSG_ENCODE_DECODE
    );
    KGLogPrintf(
        KGLOG_INFO, 
        "[KGatewayClient] Connect to %s : %u ... ... [%s]", 
        m_ClientConfig.szConnectIPAddr, 
        m_ClientConfig.nConnectPort, 
        m_piSocketStream ? "OK" : "Failed"
    );
    KG_PROCESS_ERROR(m_piSocketStream != NULL);

    nRetCode = m_piSocketStream->SetTimeout(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    pInsRet->first =osDone;
    m_nNowDoingOperateNum++;
    if (m_nNowDoingOperateNum < m_nOperateCount)
    {    
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;     
    }

    m_ConnectState = csWaitingConnectOK;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoConnectToGateWay\n", m_nNumber);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateClient::DoQueryNewbieMaps()
{
    int                          nResult       = false;
    int                          nRetCode      = false;
    IKG_Buffer*                  piSendBuffer  = NULL;
    C2G_QUERY_NEWBIE_MAPS*       pRequest      = NULL;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_QUERY_NEWBIE_MAPS));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_QUERY_NEWBIE_MAPS*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol  = c2g_query_newbie_maps;

    pRequest->dwReserved = 0;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pInsRet->first =osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoQueryNewbieMaps\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateClient::DoDisconnect()
{
    int nResult = false;
    //int nRetCode = false;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    m_Mutex.Lock();

    KG_COM_RELEASE(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);
    pInsRet->first = osDone;

    m_nNowDoingOperateNum++;
    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }
    else
    {
        m_nHadCompleteTask = true;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoDisconnect\n", m_nNumber);

    nResult = true;
Exit0:
    m_Mutex.Unlock();
    return nResult;
}

int KSimulateClient::ProcessPackage()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    IKG_Buffer*             piRecvPackage       = NULL;
    size_t                  uBufferSize         = 0;
    tagProtocolHeader*      pProtocolHeader     = NULL;
    PROTOCOL_FUNCTION       pfnProtocolFunction = NULL;
    int                     nProtocol           = 0;

    m_Mutex.Lock();

    KG_PROCESS_SUCCESS(m_piSocketStream == NULL || m_ConnectState == csWaitingConnectOK);

    while (true)
    {
        timeval TimeoutValue = {0, 1};

        nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
        if (nRetCode == -1)
        {
            break;
        }

        if (nRetCode == 0)
            break;

        KG_COM_RELEASE(piRecvPackage);

        nRetCode = m_piSocketStream->Recv(&piRecvPackage);
        if (nRetCode != 1)
        {
            KG_COM_RELEASE(m_piSocketStream);
            break;
        }
        KGLOG_PROCESS_ERROR(piRecvPackage);

        m_nTimeOutLoop  = MAX_RECV_LOOP_TIMES;

        uBufferSize = (size_t)piRecvPackage->GetSize();
        KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(tagProtocolHeader));

        pProtocolHeader = (tagProtocolHeader*)piRecvPackage->GetData();
        KGLOG_PROCESS_ERROR(pProtocolHeader);

        nProtocol = pProtocolHeader->cProtocol;
        KGLOG_PROCESS_ERROR(nProtocol >= 0 && nProtocol <= UCHAR_MAX);

        KGLOG_PROCESS_ERROR(uBufferSize >= m_uPakSize[nProtocol]);

        pfnProtocolFunction = m_ProtocolFunctions[nProtocol];
        KGLOG_PROCESS_ERROR(pfnProtocolFunction);

        nRetCode = (this->*pfnProtocolFunction)((BYTE*)pProtocolHeader, uBufferSize);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    m_Mutex.Unlock();
    KG_COM_RELEASE(piRecvPackage);
    if (!nResult)
    {
        KG_COM_RELEASE(m_piSocketStream);
    }
    return nResult;
}

int KSimulateClient::DoGiveupQueueRequest()
{
    int                          nResult       = false;
    int                          nRetCode      = false;
    IKG_Buffer*                  piSendBuffer  = NULL;
    C2G_GIVEUP_QUEUE_REQUEST*    pRequest      = NULL;
    std::pair<int, USER_OPERATE_FUNCTION> InsRet;

    KG_PROCESS_ERROR(m_piSocketStream);

    InsRet = m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(InsRet.first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_GIVEUP_QUEUE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_GIVEUP_QUEUE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol  = c2g_giveup_queue_request;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    
    InsRet.first = osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoGiveupQueueRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

void KSimulateClient::RevWorkThreadFunction(void* pvParam)
{
    KSimulateClient* pThis = (KSimulateClient*)pvParam;

    ASSERT(pThis);

    pThis->RevThreadFunction();
}

int KSimulateClient::RevThreadFunction()
{
    int     nResult     = false;
    //int     nRetCode    = false;

    while (!m_nRevExitFlag)
    {    
        ProcessPackage();
        
        if (m_nHadCompleteTask)
        {
            continue;
        }

        m_nTimeOutLoop--;
        if (m_nTimeOutLoop == 0)
        {
            if (m_ClientConfig.nNeedTestTimeOut)
            {
                m_RetForecastResult.nResult = CLIENT_REV_TIMEOUT;
                m_nRevExitFlag = true;
                m_nSendExitFlag = true;
                break;
            }  
        }

        KGThread_Sleep(100);
    }  

    nResult = true;

    return nResult;
}

void KSimulateClient::SendWorkThreadFunction(void* pvParam)
{
    KSimulateClient* pThis = (KSimulateClient*)pvParam;

    ASSERT(pThis);

    pThis->SendThreadFunction(pThis);
}

int KSimulateClient::SendThreadFunction(KSimulateClient* pClient)
{
    int     nResult     = false;
//    int     nRetCode    = false;
    std::pair<int, USER_OPERATE_FUNCTION> InsRet;
    USER_OPERATE_FUNCTION pfnUserOperateFunction = NULL;

    KGLOG_PROCESS_ERROR(m_nOperateCount > 0);

    while (!m_nSendExitFlag)
    {   
        if(m_nNowDoingOperateNum >= m_nOperateCount)
        {
            KGThread_Sleep(100);
            continue;
        }
      
        InsRet = m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pfnUserOperateFunction = InsRet.second;

        if (InsRet.first == osReady)
        {
            (pClient->*pfnUserOperateFunction)();
        }

        if (m_ClientConfig.nNeedSendPing)
        {
            DoPingSignal();
        }

        KGThread_Sleep(300);
    }  

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateClient::OnPingRespond(BYTE* pbyData, size_t uDataLen)
{
    //收到ping回应
    int nResult     = false;
    //int nRetCode    = false;

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::OnHandshakeRespond(BYTE* pbyData, size_t uDataLen)
{
    int                      nResult    = false;
    //int nRetCode                      = false;
    G2C_HANDSHAKE_RESPOND*   pRespond   = (G2C_HANDSHAKE_RESPOND*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    switch (pRespond->byCode)
    {
    case ghcHandshakeSucceed:
        //          握手成功
        if (m_nNowDoingOperateNum < m_nOperateCount)
        {    
            pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
            pInsRet->first = osReady;
        }
        break;

    case ghcGatewayVersionError: 
        //          网关版本不匹配,请更新客户端
        break;

    case ghcGameWorldMaintenance:
        //          系统正在维护
        break;

    case ghcAccountSystemLost:
        //          帐号系统(paysys)连接丢失
        break;

    case ghcGameWorldVersionError:  
        //          游戏世界版本不匹配,请更新客户端
        break;

    default:
        //          未知错误
        break;
    }

    if (m_RetForecastResult.nTestPoint == KG_CLIENT_HANDSHAKE) 
    {
        m_RetForecastResult.nResult = pRespond->byCode;

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {    
            m_nHadCompleteTask = true;
        }
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnHandshakeRespond\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::OnAccountVerifyRespond(BYTE* pbyData, size_t uDataLen)
{
    int                             nResult     = false;
    //int nRetCode                                = false;
    G2C_ACCOUNT_VERIFY_RESPOND*     pRespond    = (G2C_ACCOUNT_VERIFY_RESPOND*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    switch (pRespond->nCode)
    {
    case ACTION_SUCCESS:
        //          验证成功
        if (m_nNowDoingOperateNum < m_nOperateCount)
        {
            pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
            pInsRet->first = osReady;
        }
        break;

    case E_ACCOUNT_OR_PASSWORD:
        //          帐户或密码错误
        break;

    default:
        //          未知错误
        break;
    }

    if (m_RetForecastResult.nTestPoint == KG_USER_LOGIN) 
    {
        m_RetForecastResult.nResult = pRespond->nCode;

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {    
            m_nHadCompleteTask = true;
        }
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnAccountVerifyRespond\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::OnSyncRoleList(BYTE* pbyData, size_t uDataLen)
{
    int                     nResult         = false;
//    int                     nRetCode        = 0;
    G2C_SYNC_ROLE_LIST*     pRoleList       = (G2C_SYNC_ROLE_LIST*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (
        (pRoleList->nRoleCount == pRoleList->nRoleIndex) &&
        ((m_RetForecastResult.nTestPoint) == KG_ROLE_LIST)
    )
    {
        m_RetForecastResult.nResult = pRoleList->nRoleCount;

        if (m_nNowDoingOperateNum < m_nOperateCount)
        {
            pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
            pInsRet->first = osReady;
        }
        else
        {
            m_nHadCompleteTask = true;
        }
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnSyncRoleList\n", m_nNumber);

    nResult = true;
//Exit0:
    return nResult;  
}

int KSimulateClient::OnSyncNewbieMaps(BYTE* pbyData, size_t uDataLen)
{
    int                         nResult         = false;
    //int                         nRetCode        = 0;
    G2C_SYNC_NEWBIE_MAPS*       pRespond        = (G2C_SYNC_NEWBIE_MAPS*)pbyData;
    int                         nCopyCount      = pRespond->nCopyCount;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(G2C_SYNC_NEWBIE_MAPS) + sizeof(KNEWBIE_MAP_COPY_INFO) * nCopyCount);

    if (m_RetForecastResult.nTestPoint == KG_HOMETOWN_LIST)
    {
        m_RetForecastResult.nResult = nCopyCount;  // 保存结果

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {
            m_nHadCompleteTask = true;
        }
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }
    
    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnSyncNewbieMaps\n", m_nNumber);

    nResult = true;
Exit0: 
    return nResult; 
}

int KSimulateClient::OnCreateRoleRespond(BYTE* pbyData, size_t uDataLen)
{
    int                         nResult             = false;
    //int                         nRetCode            = 0;
    G2C_CREATE_ROLE_RESPOND*    pCreateRoleRespond  = (G2C_CREATE_ROLE_RESPOND*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (pCreateRoleRespond->byCode == eCreateRoleSucceed)
    {
        //创建成功
        KGLOG_PROCESS_ERROR(uDataLen == sizeof(G2C_CREATE_ROLE_RESPOND) + sizeof(KROLE_LIST_INFO));
    }

    if (m_RetForecastResult.nTestPoint == KG_CREATE_ROLE)
    {
        m_RetForecastResult.nResult = pCreateRoleRespond->byCode; // 保存结果

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {
            m_nHadCompleteTask = true;
        }
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnCreateRoleRespond\n", m_nNumber);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateClient::OnDeleteRoleRespond(BYTE* pbyData, size_t uDataLen)
{
    int                         nResult             = false;
    //int                         nRetCode            = false;
    G2C_DELETE_ROLE_RESPOND*    pDeleteRoleRespond  = (G2C_DELETE_ROLE_RESPOND*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (m_RetForecastResult.nTestPoint == KG_DELETE_ROLE)
    {
        m_RetForecastResult.nResult = pDeleteRoleRespond->byCode; // 保存结果

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {
            m_nHadCompleteTask = true;
        }
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnDeleteRoleRespond\n", m_nNumber);

    nResult = true;
    //Exit0:   
    return nResult;
}

int KSimulateClient::OnSyncLoginKey(BYTE* pbyData, size_t uDataLen)
{
    int                         nResult             = false;
    //int                         nRetCode            = false;
    G2C_SYNC_LOGIN_KEY*         pGameLoginInfo      = (G2C_SYNC_LOGIN_KEY*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    switch (pGameLoginInfo->byCode)
    {
    case eGameLoginSucceed:
        //          成功
        break;

    case eGameLoginSystemMaintenance:
        //        系统维护
        break;

    case eGameLoginQueueWait:
        //        人数已满,需要排队
        break;
    case eGameLoginOverload:
        //          系统过载
        break;
    case eGameLoginRoleFreeze:
        //        角色已冻结
        break;
    case eGameLoginUnknownError:
        //        未知错误
        break;

    default:
        break;
    }

    if (m_RetForecastResult.nTestPoint == KG_ROLE_ENTER_GAME)
    {
        m_RetForecastResult.nResult = pGameLoginInfo->byCode;  // 保存结果

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {
            m_nHadCompleteTask = true;
        }
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnSyncLoginKey\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::OnAccountLockedNotify(BYTE* pbyData, size_t uDataLen)
{
    int                            nResult  = false;
    //int nRetCode                            = false;
//    G2C_ACCOUNT_LOCKED_NOTIFY*     pRespond = (G2C_ACCOUNT_LOCKED_NOTIFY*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (m_RetForecastResult.nTestPoint == KG_USER_LOGIN)
    {
        m_RetForecastResult.nResult =  USER_ACCOUNT_LOCKED; // 保存结果

        m_nHadCompleteTask = true;
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    m_nHadRevLockAccountRespond = true;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnAccountLockedNotify\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::OnKickAccountNotify(BYTE* pbyData, size_t uDataLen)
{
    int                         nResult    = false;
    //int nRetCode                            = false;
//    G2C_KICK_ACCOUNT_NOTIFY*    pRespond   = (G2C_KICK_ACCOUNT_NOTIFY*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (m_RetForecastResult.nTestPoint == KG_USER_RETRY)
    {
        m_RetForecastResult.nResult = CLIENT_HAD_KICK;
    }
    
    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnKickAccountNotify\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::OnSyncQueueState(BYTE* pbyData, size_t uDataLen)
{
    int                      nResult    = false;
//    int                      nRetCode   = false;
    G2C_SYNC_QUEUE_STATE*    pRespond   = (G2C_SYNC_QUEUE_STATE*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (m_RetForecastResult.nTestPoint == KG_ROLE_ENTER_GAME) 
    {
        m_RetForecastResult.nResult = CLIENT_REV_LOGINGAME_STATE_INFO;
        m_RetForecastResult.nTypeResult = pRespond->nPosition;
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnSyncQueueState\n", m_nNumber);

    nResult = true;
//Exit0:
    return nResult;
}

int KSimulateClient::OnGiveupQueueRespond(BYTE* pbyData, size_t uDataLen)
{
    int                          nResult    = false;
    //int nRetCode                      = false;
//    G2C_GIVEUP_QUEUE_RESPOND*    pRespond   = (G2C_GIVEUP_QUEUE_RESPOND*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    if (m_RetForecastResult.nTestPoint == KG_ROLE_ENTER_GAME) 
    {
        m_RetForecastResult.nResult = CLIENT_REV_GIVEUP_QUEUE_RESPOND;
        m_RetForecastResult.nTypeResult = 0;

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {
            m_nHadCompleteTask = true;
        }
    }

    if (m_nNowDoingOperateNum < m_nOperateCount)
    {
        pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
        pInsRet->first = osReady;
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnGiveupQueueRespond\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::Run()
{
    int nResult    = false;
    int nRetCode   = false;

    if (!m_nRevWorkThreadFlag)
    {
        m_nRevExitFlag       = false;
        nRetCode             = m_RevWorkThread.Create(RevWorkThreadFunction, this);
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nRevWorkThreadFlag = true;
    }

    if (!m_nSendWorkThreadFlag)
    {
        m_nSendExitFlag       = false;
        nRetCode              = m_SendWorkThread.Create(SendWorkThreadFunction, this);
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nSendWorkThreadFlag = true;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateClient::FillOperate(int nOperateCount, int nOperate...)
{
    int     nResult         = false;
    //int nRetCode            = false;
    va_list ap;
    int     nTempOperate    = 0;

    KGLOG_PROCESS_ERROR(nOperateCount > 0);
    KGLOG_PROCESS_ERROR(nOperate > onStart);
    KGLOG_PROCESS_ERROR(nOperate < onTotal);

    FillOperateToOperateList(nOperate, osReady);
    va_start(ap, nOperate);
    for(int i = 1; i < nOperateCount; i++)
    { 
        nTempOperate = va_arg(ap,int);
        KGLOG_PROCESS_ERROR(nTempOperate > onStart);
        KGLOG_PROCESS_ERROR(nTempOperate < onTotal);

        FillOperateToOperateList(nTempOperate, osReady);
    }
    va_end(ap);

    nResult = true;
Exit0:
    return nResult; 
}

int KSimulateClient::FillOperateToOperateList(int nOperateNum, int nOperateState)
{
    int     nResult         = false;
    //int nRetCode            = false;

    KGLOG_PROCESS_ERROR(nOperateNum > onStart);
    KGLOG_PROCESS_ERROR(nOperateNum < onTotal);

    switch (nOperateNum)
    {
    case onConnectToGateway:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoConnectToGateWay));
        break;
    case onHandShake:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::ProcessConnectGateway));
        break;
    case onAccountVerify:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoAccountVerifyRequest));
        break;
    case onMibaoVerify:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoMibaoVerifyRequest));
        break;
    case onCreateRole:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoCreateRoleRequest));
        break;
    case onDeleteRole:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoDeleteRoleRequest));
        break;
    case onLoginGame:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoLoginGameRequest));
        break;
    case onQueryNewbieMaps:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoQueryNewbieMaps));
        break;
    case onGiveUpQueue:
        m_UserOperateFunctionList.push_back(make_pair(nOperateState, &KSimulateClient::DoGiveupQueueRequest));
        break;
    case onDisConnect:
        m_UserOperateFunctionList.push_back(make_pair(osReady, &KSimulateClient::DoDisconnect));
    default:
        break;
    }

    m_nOperateCount++;
    m_nHadCompleteTask = false;

    nResult = true;
Exit0:
    return nResult; 
}

int KSimulateClient::GetReturnResult(KG_RETURN_RESULT* pResult)
{
    int  nResult    = false;
    //int nRetCode    = false;

    KGLOG_PROCESS_ERROR(pResult);
    
    memcpy(pResult, &m_RetForecastResult, sizeof(*pResult));

    nResult = true;
Exit0:
    return nResult; 
}

int KSimulateClient::ModifyUserName(char szNewName[])
{
    int  nResult    = false;
    //int nRetCode    = false;

    KGLOG_PROCESS_ERROR(szNewName);
    strncpy(m_ClientConfig.szClientAccountName, szNewName, sizeof(m_ClientConfig.szClientAccountName));
    m_ClientConfig.szClientAccountName[sizeof(m_ClientConfig.szClientAccountName) - 1] = '\0';
    
    nResult = true;
Exit0:
    return nResult; 
}

int KSimulateClient::CheckTheAccountLockedOrNot(int* pnRevLockAccountRespondState)
{
    int  nResult    = false;
    //int nRetCode    = false;

    KGLOG_PROCESS_ERROR(pnRevLockAccountRespondState);

    *pnRevLockAccountRespondState = m_nHadRevLockAccountRespond;

    nResult = true;
Exit0:
    return nResult; 
}

int KSimulateClient::TaskIsCompleted(int* pnTaskState)
{
    int  nResult    = false;
    //int nRetCode    = false;

    KGLOG_PROCESS_ERROR(pnTaskState);

    if (m_ClientConfig.nNeedStopWhenRevAppointInfo)
    {
        *pnTaskState = m_nHadCompleteTask;
    }
    else
    {
        *pnTaskState = false;
    }

    nResult = true;
Exit0:
    return nResult;  
}

int KSimulateClient::GetConfig(KG_SIMULATE_CLIENT_CONFIG* pClientConfig)
{
    int  nResult    = false;
    //int nRetCode    = false;

    KGLOG_PROCESS_ERROR(pClientConfig);

    memcpy(pClientConfig, &m_ClientConfig, sizeof(KG_SIMULATE_CLIENT_CONFIG));

    nResult = true;
Exit0:
    return nResult; 
}

int KSimulateClient::OnMibaoVerifyRespond(BYTE* pbyData, size_t uDataLen)
{
    int                             nResult     = false;
    //int nRetCode                                = false;
    G2C_MIBAO_VERIFY_RESPOND*     pRespond    = (G2C_MIBAO_VERIFY_RESPOND*)pbyData;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    switch (pRespond->nCode)
    {
    case ACTION_SUCCESS:
        //          验证成功
        if (m_nNowDoingOperateNum < m_nOperateCount)
        {
            pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
            pInsRet->first = osReady;
        }
        break;

    case E_ACCOUNT_OR_PASSWORD:
        //          帐户或密码错误
        break;

    default:
        //          未知错误
        break;
    }

    if (m_RetForecastResult.nTestPoint == KG_USER_MIBAO) 
    {
        m_RetForecastResult.nResult = pRespond->nCode;

        if (m_nNowDoingOperateNum >= m_nOperateCount)
        {    
            m_nHadCompleteTask = true;
        }
    }

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, OnMibaoVerifyRespond\n", m_nNumber);

    nResult = true;
    //Exit0:
    return nResult;
}

int KSimulateClient::DoMibaoVerifyRequest()
{
    int                          nResult       = false;
    int                          nRetCode      = false;
    IKG_Buffer*                  piSendBuffer  = NULL;
    C2G_MIBAO_VERIFY_REQUEST*    pRequest      = NULL;
    std::pair<int, USER_OPERATE_FUNCTION>* pInsRet;

    KG_PROCESS_ERROR(m_piSocketStream);

    pInsRet = &m_UserOperateFunctionList[m_nNowDoingOperateNum];
    KGLOG_PROCESS_ERROR(pInsRet->first == osReady);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_MIBAO_VERIFY_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_MIBAO_VERIFY_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol  = c2g_mibao_verify_request;

    strncpy(
        pRequest->szMibaoPassword, 
        m_ClientConfig.szClientAccountName, 
        sizeof(pRequest->szMibaoPassword)
    );
    pRequest->szMibaoPassword[sizeof(pRequest->szMibaoPassword) - 1] = '\0';

    //strncpy(
    //    pRequest->szPsw, 
    //    m_ClientConfig.szClientAccountPassword.szPassword, 
    //    sizeof(pRequest->szPsw)
    //    );
    //pRequest->szPsw[sizeof(pRequest->szPsw) - 1] = '\0';

    //strncpy(
    //    pRequest->szAct, 
    //    m_ClientConfig.szClientActivateCode,
    //    sizeof(pRequest->szAct)
    //    );
    //pRequest->szAct[sizeof(pRequest->szAct) - 1] = '\0';

    KGThread_Sleep(1000);
    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pInsRet->first = osDone;
    m_nNowDoingOperateNum++;

    KGLogPrintf(KGLOG_INFO, "ClientNum:%d, DoMibaoVerifyRequest\n", m_nNumber);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}
