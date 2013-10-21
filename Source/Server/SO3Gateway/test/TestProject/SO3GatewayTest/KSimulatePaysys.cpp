///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSimulatePaysys.cpp
// Creator  :   liuzhibiao
// Date     :   2008-10-8
// Comment  :   v0.99   
//              V0.9    2008-9-22  改变数据结构
//              v0.99   2008-12-16 优化处理
//              v1.0    2009-2-13   修改linux兼容
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol/BishopPaysysProtocol.h"
#include "KSimulatePaysys.h"

#define REGISTER_G2P_FUNCTION(id, func, data)               \
    do                                                      \
    {                                                       \
    m_ProtocolFunctions[id] = &KSimulatePaysys::func;    \
    m_uPakSize[id]          = sizeof(data);             \
    } while(false)

#define MAX_WAIT_ACCEPT     16
#define MAX_SOCKET_EVENT    (1024 + MAX_WAIT_ACCEPT)
#define MAX_GATEWAY_COUNT   10

int KSimulatePaysys::ProcessPackage(IKG_SocketStream* piSocket)
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    int                     nGatewayIndex       = 0;
    KGatewayAgency*         pGateway            = NULL;
    BYTE*                   pbyData             = NULL;
    IKG_Buffer*             piRecvPackage       = NULL;
    size_t                  uBufferSize         = 0;
    tagProtocolHeader*      pProtocolHeader     = NULL;
    BYTE                    nProtocol           = 0;
    PROTOCOL_FUNCTION       pfnProtocolFunction = NULL;
//    timeval                 TimeoutValue        = {0, 0};

    nGatewayIndex = (int)(ptrdiff_t)(piSocket->GetUserData());

    pGateway = GetGateway(nGatewayIndex);
    KGLOG_PROCESS_ERROR(pGateway);

    while (true)
    {
        KG_COM_RELEASE(piRecvPackage);

        nRetCode = piSocket->Recv(&piRecvPackage);
        if (nRetCode == -2)
        {
            break;
        }

        if (nRetCode == -1)
        {
            KGLogPrintf(KGLOG_INFO, "Connection lost: %d\n", nGatewayIndex);
            break;
        }

        KGLOG_PROCESS_ERROR(piRecvPackage);

        uBufferSize = (size_t)piRecvPackage->GetSize();
        KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(tagProtocolHeader));

        pbyData = (BYTE*)piRecvPackage->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        pProtocolHeader = (tagProtocolHeader*)pbyData;
        nProtocol       = pProtocolHeader->cProtocol;
        //KGLOG_PROCESS_ERROR(pHeader->cProtocol >= 0 && pHeader->cProtocol <= UCHAR_MAX);

        KGLOG_PROCESS_ERROR(uBufferSize >= m_uPakSize[nProtocol]);

        pfnProtocolFunction = m_ProtocolFunctions[nProtocol];
        KGLOG_PROCESS_ERROR(pfnProtocolFunction);

        pbyData         += sizeof(tagProtocolHeader);
        uBufferSize     -= sizeof(tagProtocolHeader);

        if (nProtocol != m_PaysysConfig.nNotHandleProtocol)
        {
            nRetCode = (this->*pfnProtocolFunction)(pGateway, pbyData, uBufferSize);
        } 
    }

    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}

int KSimulatePaysys::Init(const KG_SIMULATE_PAYSYS_CONFIG& crSimulatePaysysConfig)
{
    int         nResult                 = false;
    int         nRetCode                = false;
//    timeval     TimeoutValue            = {0, 0};
    int         nSocketServerInitFlag   = false;
    
    m_nTimeNow                  = 0;
    m_nNextGatewayIndex         = 0;
    m_nPingCycle                = 30;
    m_bRecycleConnections       = false;
    m_nWorkThreadFlag           = false;
    m_nExitFlag                 = false;
    m_nMaxGateway               = MAX_GATEWAY_COUNT;
    m_nHadConnections           = false;
    m_pSockerServer             = NULL;
    m_pSocketEvents             = NULL;

    m_nRevChangeExtPointRequest         = false;
    m_nRevActivePresentCodeRequest      = false;
    m_nPlayerSetChargeFlagRequest       = false;

    m_nFreezeCoinRequest            = false;
    m_nAccountExchangeRequest       = false;

    memcpy(&m_PaysysConfig, &crSimulatePaysysConfig, sizeof(m_PaysysConfig));

    m_pSocketEvents = new KG_SOCKET_EVENT[MAX_SOCKET_EVENT];
    KGLOG_PROCESS_ERROR(m_pSocketEvents);

    m_pSockerServer = new KG_SocketServerAcceptor();
    KGLOG_PROCESS_ERROR(m_pSockerServer);

    nRetCode = m_pSockerServer->Init(
        m_PaysysConfig.szPaysysIPAddr, 
        m_PaysysConfig.nPaysysPort, 
        MAX_WAIT_ACCEPT, 
        1024 * 4, 1024 * 16, EASYCRYPT_ENCODE_DECODE, NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    nSocketServerInitFlag = true;

    memset(m_ProtocolFunctions, 0, sizeof(m_ProtocolFunctions));
    memset(m_uPakSize, 0, sizeof(m_uPakSize));

    REGISTER_G2P_FUNCTION(B2P_PLAYER_PASSPOD_VERIFY_EX, OnMiBaoVerifyRequest,KAccountUserPasspodVerify);
    REGISTER_G2P_FUNCTION(B2P_BISHOP_IDENTITY_VERIFY, OnGatewayVerifyRequest, KServerAccountUserLoginInfo2);
    REGISTER_G2P_FUNCTION(B2P_BISHOP_RECONNECT_IDENTITY_VERIFY, OnGateWayReVerityRequest, KServerAccountUserLoginInfo2);
    REGISTER_G2P_FUNCTION(B2P_BISHOP_LOGOUT, OnGateWayLogout, KServerAccountUserLoginInfo2);
    REGISTER_G2P_FUNCTION(B2P_IB_PLAYER_IDENTITY_VERIFY, OnAccountVerifyRequest, KAccountUserLoginInfo);
    REGISTER_G2P_FUNCTION(B2P_PLAYER_LEAVE_GAME, OnPlayerLeaveGame, KAccountUserLogout);
    REGISTER_G2P_FUNCTION(B2P_PING, OnPingRequest, PING_COMMAND);
    REGISTER_G2P_FUNCTION(B2P_GET_ZONE_CHARGE_FLAG, OnPlayerGetZoneChargeFlagRequest, KGetZoneChargeFlag);
    REGISTER_G2P_FUNCTION(B2P_PLAYER_SET_CHARGE_FLAG, OnPlayerSetChargeFlagRequest, KAccountSetChargeFlag);
    REGISTER_G2P_FUNCTION(B2P_EXT_POINTS_OPERATION, OnChangeExtPointsRequest, KAccountUserChangeExtPoint);
    REGISTER_G2P_FUNCTION(B2P_GAMEWORLD_2_PAYSYS, OnActivePresentCodeRequest, KGameworld2Paysys);
    REGISTER_G2P_FUNCTION(B2P_PLAYER_FREEZE_FEE, OnFreezeCoinRequest, KAccountFreezeFee);
    REGISTER_G2P_FUNCTION(B2P_PLAYER_EXCHANGE_EX, OnAccountExchangeRequest, KAccountExchangeEx);
    REGISTER_G2P_FUNCTION(B2P_IB_PLAYER_IDENTITY_VERIFY_SD, OnSDAccountVerifyRequest, KAccountUserLoginInfoSD);
    nRetCode = StartWork();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_nWorkThreadFlag)
        {
            m_nExitFlag = true;
            m_WorkThread.Destroy();
            m_nWorkThreadFlag = false;
        }

        if (nSocketServerInitFlag)
        {
            m_pSockerServer->UnInit(NULL);
            nSocketServerInitFlag = false;
        }
        if (m_pSockerServer)
        {
            KG_DELETE(m_pSockerServer);
        }
        if (m_pSocketEvents)
        {
            KG_DELETE_ARRAY(m_pSocketEvents);
        }
        KG_DELETE(m_pSockerServer);
    }
    return nResult;
}

void KSimulatePaysys::Activate()
{
    int  nRetCode           = 0;
    int  nRetEventCount     = 0;

    m_nTimeNow = time(NULL);

    while (true)
    {
        nRetEventCount     = 0;

        nRetCode = m_pSockerServer->Wait(MAX_SOCKET_EVENT, m_pSocketEvents, &nRetEventCount);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (nRetEventCount == 0)
            break;

        for (int i = 0; i < nRetEventCount; ++i)
        {

            if (m_pSocketEvents[i].uEventFlag & KG_SOCKET_EVENT_ACCEPT)
            {
                ProcessNewConnection(m_pSocketEvents[i].piSocket);
                KG_COM_RELEASE(m_pSocketEvents[i].piSocket);
                continue;
            }

            //if (KG_SOCKET_EVENT_IN != m_pSocketEvents[i].uEventFlag)
            //    continue;

            ASSERT(m_pSocketEvents[i].uEventFlag & KG_SOCKET_EVENT_IN);

            ProcessPackage(m_pSocketEvents[i].piSocket);

            KG_COM_RELEASE(m_pSocketEvents[i].piSocket);
        }
    }

    CheckConnectionTimeout();

    RecycleConnections();

Exit0:
    return;
}

int KSimulatePaysys::OnSDAccountVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    KAccountUserLoginInfoSD*    pKSDAccountUserLoginInfo    = (KAccountUserLoginInfoSD*)pbyData;
    std::pair<KClientPlayerAccountTable::iterator, BOOL> InsRet;

    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForDisconnect);

    if (m_PaysysConfig.nGatewayLoginResult == ACTION_SUCCESS)
    {
        KClientPlayerAccountTable::iterator it = m_ClientPlayerAccountTable.find(pKSDAccountUserLoginInfo->Account);
        if (it != m_ClientPlayerAccountTable.end())
        {
            m_ClientPlayerAccountTable[pKSDAccountUserLoginInfo->Account] = false;
        }
        else
        {
            m_ClientPlayerAccountTable.insert(std::make_pair(pKSDAccountUserLoginInfo->Account, false));
        }
    }

    if (m_PaysysConfig.nNotRespondProtocol != B2P_IB_PLAYER_IDENTITY_VERIFY_SD)
    {

        nRetCode = DoSDAccountVerifyRespond(pGateway, pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);

    }

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnAccountVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    KAccountUserLoginInfo*      pKAccountUserLoginInfo      = (KAccountUserLoginInfo*)pbyData;
    std::pair<KClientPlayerAccountTable::iterator, BOOL> InsRet;

    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForDisconnect);

    if (m_PaysysConfig.nGatewayLoginResult == ACTION_SUCCESS)
    {
        KClientPlayerAccountTable::iterator it = m_ClientPlayerAccountTable.find(pKAccountUserLoginInfo->Account);
        if (it != m_ClientPlayerAccountTable.end())
        {
            m_ClientPlayerAccountTable[pKAccountUserLoginInfo->Account] = false;
        }
        else
        {
            m_ClientPlayerAccountTable.insert(std::make_pair(pKAccountUserLoginInfo->Account, false));
        }
    }

    if (m_PaysysConfig.nNotRespondProtocol != B2P_IB_PLAYER_IDENTITY_VERIFY)
    {

        nRetCode = DoAccountVerifyRespond(pGateway, pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);

    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnMiBaoVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    //KAccountUserPasspodVerifyExt*  pKAccountUserLoginInfo      = (KAccountUserPasspodVerifyExt*)pbyData;

    std::pair<KClientPlayerAccountTable::iterator, BOOL> InsRet;

    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForDisconnect);

    if (m_PaysysConfig.nNotRespondProtocol != B2P_PLAYER_PASSPOD_VERIFY)
    {
        nRetCode = DoMiBaoVerifyRespond(pGateway, pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnPlayerLeaveGame(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int nResult                             = false;
    // int nRetCode                           = false;
    KAccountUserLogout* pAccountUserLogout  = (KAccountUserLogout*)pbyData;
    KClientPlayerAccountTable::iterator it;

    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForDisconnect);
    // 玩家登出信息到达
    pGateway->nHadRevUserLoginOut = true;

    it  = m_ClientPlayerAccountTable.find(pAccountUserLogout->Account);
    KGLOG_PROCESS_ERROR(it != m_ClientPlayerAccountTable.end()); //正常情况下是应该找到之前登陆的用户名的

    m_ClientPlayerAccountTable[pAccountUserLogout->Account] = true;

    KGLogPrintf(KGLOG_INFO, "Paysys: %s Gateway Send %s had Leaved Game", pGateway->szAccount, pAccountUserLogout->Account);

    nResult = true;

Exit0:

    return nResult;
}

int KSimulatePaysys::OnGatewayVerifyRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    KServerAccountUserLoginInfo2*   pRequest            = (KServerAccountUserLoginInfo2*)pbyData;
    KLockedGatewayTable::iterator   itAccountTable;
    KGatewayTable::iterator         itGatewayTable;

    ASSERT(pGateway);
    ASSERT(pbyData);

    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForGatewayAccountPassword);
    KGLOG_PROCESS_SUCCESS(m_PaysysConfig.nGatewayLoginResult != ACTION_SUCCESS);

    pRequest->szAccountName[sizeof(pRequest->szAccountName) - 1] = '\0';
    pRequest->szPassWord[sizeof(pRequest->szPassWord) - 1]       = '\0';

    nRetCode = CheckLockGatewayTable(pRequest->szAccountName, false, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    strncpy(pGateway->szAccount, pRequest->szAccountName, sizeof(pGateway->szAccount));
    pGateway->szAccount[sizeof(pGateway->szAccount) - 1] = '\0';
    strncpy(pGateway->szPsw, pRequest->szPassWord, sizeof(pGateway->szPsw));
    pGateway->szPsw[sizeof(pGateway->szPsw) - 1] = '\0';

    m_LockedGatewayTable.insert(pRequest->szAccountName);
    pGateway->nSameGatewayConnectCount++;

Exit1:
    pGateway->nState  = easGWWaitForDisconnect;

    if (m_PaysysConfig.nNotRespondProtocol != B2P_BISHOP_IDENTITY_VERIFY)
    {
        nRetCode = DoGatewayVerifyRespond(pGateway, m_PaysysConfig.nGatewayLoginResult);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::CheckLockGatewayTable(
    char szGatewayAccountName[],
    int  nKeepTheGatewayCount, 
    int* pnPreGatewayCount
)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    KLockedGatewayTable::iterator   itAccountTable;
    KGatewayTable::iterator         itGatewayTable;
    KGatewayAgency*                 pTempGateway        = NULL;

    KGLOG_PROCESS_ERROR(szGatewayAccountName);
    if (nKeepTheGatewayCount)
    {
        KGLOG_PROCESS_ERROR(pnPreGatewayCount);
    }

    itAccountTable = m_LockedGatewayTable.find(szGatewayAccountName);
    if (itAccountTable != m_LockedGatewayTable.end())
    {
        // 断开连接
        for (
            itGatewayTable =  m_GatewayTable.begin(); 
            itGatewayTable != m_GatewayTable.end(); 
            ++itGatewayTable
        )
        {
            pTempGateway= &itGatewayTable->second;

            nRetCode = strcmp(pTempGateway->szAccount, szGatewayAccountName);
            if (nRetCode == 0)
            {
                if (nKeepTheGatewayCount)
                {
                    *pnPreGatewayCount = pTempGateway->nSameGatewayConnectCount;
                }
                OnDisconnect(pTempGateway);
                break;
            }

        }
        m_LockedGatewayTable.erase(itAccountTable);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnGateWayReVerityRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    KServerAccountUserLoginInfo2*   pRequest            = (KServerAccountUserLoginInfo2*)pbyData;
    KLockedGatewayTable::iterator   itAccountTable;
    KGatewayTable::iterator         itGatewayTable;
    int                             nPreGatewayCount    = 0;

    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForGatewayAccountPassword);
    KGLOG_PROCESS_SUCCESS(m_PaysysConfig.nGatewayLoginResult != ACTION_SUCCESS);

    pRequest->szAccountName[sizeof(pRequest->szAccountName) - 1] = '\0';
    pRequest->szPassWord[sizeof(pRequest->szPassWord) - 1]       = '\0';

    nRetCode = CheckLockGatewayTable(pRequest->szAccountName, true, &nPreGatewayCount);
    KGLOG_PROCESS_ERROR(nRetCode);

    strncpy(pGateway->szAccount, pRequest->szAccountName, sizeof(pGateway->szAccount));
    pGateway->szAccount[sizeof(pGateway->szAccount) - 1] = '\0';
    strncpy(pGateway->szPsw, pRequest->szPassWord, sizeof(pGateway->szPsw));
    pGateway->szPsw[sizeof(pGateway->szPsw) - 1] = '\0';

    m_LockedGatewayTable.insert(pRequest->szAccountName);
    pGateway->nSameGatewayConnectCount = nPreGatewayCount;
    pGateway->nSameGatewayConnectCount++;

Exit1:
    pGateway->nState  = easGWWaitForDisconnect;

    if (m_PaysysConfig.nNotRespondProtocol != B2P_BISHOP_RECONNECT_IDENTITY_VERIFY)
    {
        nRetCode = DoGatewayReVerityRespond(pGateway, m_PaysysConfig.nGatewayLoginResult);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnGateWayLogout(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                             nResult   = false;
    //int                             nRetCode  = false;
    KServerAccountUserLoginInfo2*   pRequest  = (KServerAccountUserLoginInfo2*)pbyData;
    
    KGLOG_PROCESS_ERROR(pGateway->nState == easGWWaitForDisconnect);
    KGLOG_PROCESS_ERROR(pRequest->Size == sizeof(KServerAccountUserLoginInfo2));

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnPingRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int     nResult   = false;
    //int   nRetCode  = false;

    pGateway->nLastPingTime = m_nTimeNow;

    if (m_PaysysConfig.nNotRespondProtocol != B2P_PING)
    {
        DoPingRespond(pGateway);
    }
    
    nResult = true;
//Exit0:
    return nResult;
}

int KSimulatePaysys::DoSDAccountVerifyRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    tagProtocolHeader*          ptagProtocolHeader          = NULL;
    KAccountUserReturnExtIB2*   pAccountUserReturnExtIB2     = NULL;
    IKG_Buffer*                 piSendBuffer                = NULL;
    BYTE*                       pbySendBuffer               = NULL;
    KAccountUserLoginInfo*      pKAccountUserLoginInfo      = (KAccountUserLoginInfo*)pbyData;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserReturnExtIB2)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_IB_PLAYER_IDENTITY_VERIFY_RESULT_EX;

    pAccountUserReturnExtIB2 = (KAccountUserReturnExtIB2*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pAccountUserReturnExtIB2->nReturn       = m_PaysysConfig.nUserLoginResult;
    pAccountUserReturnExtIB2->Operate       = pKAccountUserLoginInfo->Operate;
    pAccountUserReturnExtIB2->Type          = m_PaysysConfig.wType;
    pAccountUserReturnExtIB2->byReserve[45] = m_PaysysConfig.byMibaoMode;

    if(m_PaysysConfig.nNotRespondSubProtocol  ==  m_PaysysConfig.wType)
    {
        goto Exit1;
    }
    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::DoAccountVerifyRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    tagProtocolHeader*          ptagProtocolHeader          = NULL;
    KAccountUserReturnExtIB2*   pAccountUserReturnExtIB2     = NULL;
    IKG_Buffer*                 piSendBuffer                = NULL;
    BYTE*                       pbySendBuffer               = NULL;
    KAccountUserLoginInfo*      pKAccountUserLoginInfo      = (KAccountUserLoginInfo*)pbyData;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserReturnExtIB2)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_IB_PLAYER_IDENTITY_VERIFY_RESULT_EX;

    pAccountUserReturnExtIB2 = (KAccountUserReturnExtIB2*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pAccountUserReturnExtIB2->nReturn       = m_PaysysConfig.nUserLoginResult;
    pAccountUserReturnExtIB2->Operate       = pKAccountUserLoginInfo->Operate;
    pAccountUserReturnExtIB2->Type          = m_PaysysConfig.wType;
    pAccountUserReturnExtIB2->byReserve[45] = m_PaysysConfig.byMibaoMode;

    if(m_PaysysConfig.nNotRespondSubProtocol  ==  m_PaysysConfig.wType)
    {
        goto Exit1;
    }
    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::DoGatewayVerifyRespond(KGatewayAgency* pGateway, int nLoginResult)
{
    int                         nResult                         = false;
    int                         nRetCode                        = false;
    IKG_Buffer*                 piSendBuffer                    = NULL;
    BYTE*                       pbySendBuffer                   = NULL;
    tagProtocolHeader*          ptagProtocolHeader              = NULL;
    KAccountUserReturnVerify*   pKAccountUserReturnVerify       = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserReturnVerify)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_BISHOP_IDENTITY_VERIFY_RESULT;

    pKAccountUserReturnVerify = (KAccountUserReturnVerify*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pKAccountUserReturnVerify->nReturn = nLoginResult;

    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::DoGatewayReVerityRespond(KGatewayAgency* pGateway, int nLoginResult)
{
    int     nResult     = false;
    int     nRetCode    = false;
    
    nRetCode = DoGatewayVerifyRespond(pGateway, nLoginResult);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return true;
}

int KSimulatePaysys::DoPingRespond(KGatewayAgency* pGateway)
{
    int                 nResult                 = false;
    //int                 nRetCode                = false;
    //IKG_Buffer*         piSendBuffer            = NULL;
    //BYTE*               pbySendBuffer           = NULL;
    //tagPING_COMMAND*    pPindPak    = NULL;
    //DWORD               dwTimeNow   = 0;

    //KGLOG_PROCESS_ERROR(pGateway);

    //dwTimeNow = KG_GetTickCount();
    //piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagPING_COMMAND)));
    //KGLOG_PROCESS_ERROR(piSendBuffer);

    //pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    //KGLOG_PROCESS_ERROR(pbySendBuffer);

    //pPindPak = (tagPING_COMMAND*)pbySendBuffer;
    //pPindPak->cProtocol = P2B_PING;
    //pPindPak->dwTime    = dwTimeNow;

    //nRetCode = pGateway->piSocket->Send(piSendBuffer);
    //KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
//Exit0:
    //KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

void KSimulatePaysys::UnInit()
{
    if (m_nWorkThreadFlag)
    {
        m_nExitFlag = true;
        m_WorkThread.Destroy();
        m_nWorkThreadFlag = false;
    }

    for (KGatewayTable::iterator it = m_GatewayTable.begin(); it != m_GatewayTable.end(); ++it)
    {
        KGatewayAgency* pGateway= &it->second;

        KG_COM_RELEASE(pGateway->piSocket);
    }

    m_GatewayTable.clear();

    m_pSockerServer->UnInit(NULL);

    m_nHadConnections = false;

    KG_DELETE(m_pSockerServer);
    KG_DELETE_ARRAY(m_pSocketEvents);
}

int KSimulatePaysys::AllDisConnected(int nCloseThread)
{
    int nResult                 = false;
    //int nRetCode    = false;
    KGatewayAgency* pGateway    = NULL;

    if (nCloseThread && m_nWorkThreadFlag)
    {
        m_nExitFlag = true;
        m_WorkThread.Destroy();
        m_nWorkThreadFlag = false;
    }

    for (KGatewayTable::iterator it = m_GatewayTable.begin(); it != m_GatewayTable.end(); ++it)
    {
        pGateway                        =  &it->second;
        
        pGateway->nState                = easGWInvalid;
        pGateway->nHadRevUserLoginOut   = false;
        KG_COM_RELEASE(pGateway->piSocket);
    }

    m_GatewayTable.clear();

    nResult = true;
//Exit0:
    return nResult;
}

void KSimulatePaysys::WorkThreadFunction(void *pvParam)
{
    KSimulatePaysys *pThis = (KSimulatePaysys *)pvParam;

    ASSERT(pThis);

    pThis->ThreadFunction();
}

int KSimulatePaysys::ThreadFunction()
{
    int     nResult     = false;
//    int     nRetCode    = false;
  
    while (!m_nExitFlag)
    {      
        Activate();

        KGThread_Sleep(10);
    }  

    nResult = true;
//Exit0:
    return nResult;
}

int KSimulatePaysys::StartWork()
{
    int nResult     = false;
    int nRetCode    = false;
    
    KG_PROCESS_ERROR(!m_nWorkThreadFlag);

    m_nExitFlag       = false;
    nRetCode          = m_WorkThread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    m_nWorkThreadFlag = true;
    
    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::ProcessNewConnection(IKG_SocketStream* piSocket)
{
    int              nResult                 = false;
    int              nRetCode                = false;
    u_short          uRemotePort             = 0;
    const char*      pcszIP                  = NULL;
    KGatewayAgency*  pGateway                = NULL;
    struct in_addr   RemoteIP;
    std::pair<KGatewayTable::iterator, BOOL> InsRet;
    
    ASSERT(piSocket);

    KGLOG_PROCESS_ERROR(m_GatewayTable.size() < (size_t)m_nMaxGateway);

    nRetCode = piSocket->GetRemoteAddress(&RemoteIP, &uRemotePort);
    KGLOG_PROCESS_ERROR(nRetCode);

    pcszIP = inet_ntoa(RemoteIP);
    KGLOG_PROCESS_ERROR(pcszIP);

    while (true)
    {
        KGatewayTable::iterator it = m_GatewayTable.find(m_nNextGatewayIndex);
        if (it == m_GatewayTable.end())
        {
            break;
        }
        m_nNextGatewayIndex++;
    }

    InsRet = m_GatewayTable.insert(std::make_pair(m_nNextGatewayIndex, KGatewayAgency()));
    KGLOG_PROCESS_ERROR(InsRet.second);

    pGateway = &InsRet.first->second;

    pGateway->nState             = easGWWaitForGatewayAccountPassword;
    pGateway->nIndex             = m_nNextGatewayIndex;
    pGateway->piSocket           = piSocket;
    pGateway->nLastPingTime      = m_nTimeNow;

    pGateway->szAccount[0]       = '\0';
    pGateway->szPsw[0]           = '\0';

    pGateway->piSocket->AddRef();

    nRetCode = piSocket->SetUserData((void*)(ptrdiff_t)m_nNextGatewayIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLogPrintf(KGLOG_INFO, "Paysys:: New connection from %s:%u, index = %d\n", pcszIP, uRemotePort, m_nNextGatewayIndex);

    m_nHadConnections = true;

    m_nNextGatewayIndex++;

    nResult = true;
Exit0:
    return nResult;
}

void KSimulatePaysys::CheckConnectionTimeout()
{
    KGatewayAgency* pGateway = NULL;

    KG_PROCESS_ERROR(m_nTimeNow > m_nNextPingCheck);
    m_nNextPingCheck = m_nTimeNow + 8;

    for (KGatewayTable::iterator it = m_GatewayTable.begin(); it != m_GatewayTable.end(); ++it)
    {
        pGateway = &it->second;

        if (m_nTimeNow > pGateway->nLastPingTime + m_nPingCycle)
        {
            KGLogPrintf(KGLOG_INFO, "Connection timeout: %d\n", it->first);
            OnDisconnect(pGateway);
        }
    }

Exit0:
    return;
}

void KSimulatePaysys::OnDisconnect(KGatewayAgency* pGateway)
{
//    int             nRetCode        = 0;

    ASSERT(pGateway);

    pGateway->nState = easGWInvalid;
//Exit0:
    m_bRecycleConnections = true;
    return;
}

void KSimulatePaysys::RecycleConnections()
{
    KG_PROCESS_ERROR(m_bRecycleConnections);
    m_bRecycleConnections = false;

    for (KGatewayTable::iterator it = m_GatewayTable.begin(); it != m_GatewayTable.end(); NULL)
    {
        KGatewayAgency* pGateway = &(it->second);

        if (pGateway->nState != easGWInvalid)
        {
            ++it;
            continue;
        }

        KG_COM_RELEASE(pGateway->piSocket);
        m_GatewayTable.erase(it++);
    }

Exit0:
    return;
}

int KSimulatePaysys::HadRevUserLoginOut(char szClientPlayerAccountName[], int* pnHadRevUserLoginOut)
{
    int                     nResult     = false;
//    int                     nRetCode    = false;
    KClientPlayerAccountTable::iterator it;

    KG_PROCESS_ERROR(szClientPlayerAccountName);
    KG_PROCESS_ERROR(pnHadRevUserLoginOut);

    it = m_ClientPlayerAccountTable.find(szClientPlayerAccountName);

    if (it != m_ClientPlayerAccountTable.end())
    {
        *pnHadRevUserLoginOut = m_ClientPlayerAccountTable[szClientPlayerAccountName];
    }
    else
    {
        *pnHadRevUserLoginOut = false;
    }

    nResult  = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::GetGatewayConnectCount(char szGatewayAccountName[], int* pnConnectCount)
{
    int                     nResult     = false;
//    int                     nRetCode    = false;
    KGatewayAgency*         pGateway    = NULL;

    KG_PROCESS_ERROR(szGatewayAccountName);
    KG_PROCESS_ERROR(pnConnectCount);

    pGateway = GetGateway(szGatewayAccountName);

    if (pGateway)
    {
        *pnConnectCount = pGateway->nSameGatewayConnectCount;
    }

    nResult  = true;
Exit0:
    return nResult;
}

KGatewayAgency* KSimulatePaysys::GetGateway(char szGatewayAccountName[])
{
    int                     nRetCode    = false;
    KGatewayTable::iterator it          = m_GatewayTable.begin();
    KGatewayAgency*         pGateway    = NULL;
    int                     nHadFound   = false;

    ASSERT(szGatewayAccountName);
    while (it != m_GatewayTable.end())
    {
        pGateway = &(it->second);

        nRetCode = strcmp(szGatewayAccountName, pGateway->szAccount);
        if (nRetCode == 0)
        {
            nHadFound = true;
            break;
        }  
        ++it;
    }

    if (!nHadFound)
    {
       pGateway = NULL;
    }

    return pGateway;
}

int KSimulatePaysys::HadConnections(int* pnHadConnected)
{
    int    nResult  = false;
    //int   nRetCode  = false;

   *pnHadConnected = m_nHadConnections;

   nResult  = true;
//Exit0:
   return nResult;
}

int KSimulatePaysys::OnPlayerGetZoneChargeFlagRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
//    KGetZoneChargeFlag*             pRequest            = (KGetZoneChargeFlag*)pbyData;

    nRetCode = DoGetZoneChargeFlagRespond(pGateway);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::DoSetChargeFlagRespond(KGatewayAgency* pGateway)
{
    int                                 nResult                     = false;
    int                                 nRetCode                    = false;
    tagProtocolHeader*                  ptagProtocolHeader          = NULL;
    IKG_Buffer*                         piSendBuffer                = NULL;
    BYTE*                               pbySendBuffer               = NULL;
    KAccountSetChargeFlagRetExt*        pAccountSetChargeFlagRetEx    = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountSetChargeFlagRetExt)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_PLAYER_SET_CHARGE_FLAG_RESULT_EX;

    pAccountSetChargeFlagRetEx = (KAccountSetChargeFlagRetExt*)(pbySendBuffer + sizeof(tagProtocolHeader));

    //pAccountSetChargeFlagRet->Account;
    //pAccountSetChargeFlagRet->dwEndOfDayTime;
    //pAccountSetChargeFlagRet->nChargeFlag;
    //pAccountSetChargeFlagRet->nExtPoint;
    //pAccountSetChargeFlagRet->nExtPoint1;
    //pAccountSetChargeFlagRet->nExtPoint2;
    //pAccountSetChargeFlagRet->nExtPoint3;
    //pAccountSetChargeFlagRet->nExtPoint4;
    //pAccountSetChargeFlagRet->nExtPoint5;
    //pAccountSetChargeFlagRet->nExtPoint6;
    //pAccountSetChargeFlagRet->nExtPoint7;
    //pAccountSetChargeFlagRet->nLeftTimeOfPoint;
    //pAccountSetChargeFlagRet->nResult;
    //pAccountSetChargeFlagRet->Operate;
    //pAccountSetChargeFlagRet->Size;
    //pAccountSetChargeFlagRet->Type;
    //pAccountSetChargeFlagRet->Version;

    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;

}

int KSimulatePaysys::DoGetZoneChargeFlagRespond(KGatewayAgency* pGateway)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    tagProtocolHeader*          ptagProtocolHeader          = NULL;
    IKG_Buffer*                 piSendBuffer                = NULL;
    BYTE*                       pbySendBuffer               = NULL;
    KGetZoneChargeFlagRet*      pGetZoneChargeFlagRet       = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KGetZoneChargeFlagRet)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_GET_ZONE_CHARGE_FLAG;

    pGetZoneChargeFlagRet = (KGetZoneChargeFlagRet*)(pbySendBuffer + sizeof(tagProtocolHeader));
    pGetZoneChargeFlagRet->nResult = ACTION_SUCCESS;
    pGetZoneChargeFlagRet->nZoneChargeFlag = 0;//修改收费模式标记.

    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::OnPlayerSetChargeFlagRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    //KGetZoneChargeFlag*             pRequest            = (KGetZoneChargeFlag*)pbyData;

    m_nPlayerSetChargeFlagRequest       = true;

    nRetCode = DoSetChargeFlagRespond(pGateway);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnChangeExtPointsRequest(KGatewayAgency* pGateway,BYTE* pbyData, size_t uDataLen)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    //KGetZoneChargeFlag*             pRequest            = (KGetZoneChargeFlag*)pbyData;

    m_nRevChangeExtPointRequest   = true;

    nRetCode = DoChangeExtPointRespond(pGateway);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnActivePresentCodeRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                             nResult                     = false;
    int                             nRetCode                    = false;
    KGameworld2Paysys*              pGameworld2Paysys           = NULL;
    KAccountActivePresentCode*      pAccountActivePresentCode   = NULL;
    
    KGLOG_PROCESS_ERROR(uDataLen == (sizeof(KGameworld2Paysys) + sizeof(KAccountActivePresentCode)));

    pGameworld2Paysys = (KGameworld2Paysys*)pbyData;
    pAccountActivePresentCode = (KAccountActivePresentCode*)pGameworld2Paysys->byData;

    m_nRevActivePresentCodeRequest      = true;

    nRetCode = DoActivePresentCodeRespond(pGateway, pAccountActivePresentCode->Account, pAccountActivePresentCode->PresentCode, pGameworld2Paysys->Operate, true);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::DoChangeExtPointRespond(KGatewayAgency* pGateway)
{
    int                         nResult                     = false;
    int                         nRetCode                    = false;
    tagProtocolHeader*          ptagProtocolHeader          = NULL;
    IKG_Buffer*                 piSendBuffer                = NULL;
    BYTE*                       pbySendBuffer               = NULL;
    KAccountUserReturnExt2*     pAccountUserReturnExt2      = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserReturnExt2)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_EXT_POINTS_OPERATION_RESULT;

    pAccountUserReturnExt2 = (KAccountUserReturnExt2*)(pbySendBuffer + sizeof(tagProtocolHeader));

    //pAccountUserReturnExt2.Account;
    //pAccountUserReturnExt2.nPlayerDataIndex;
    pAccountUserReturnExt2->nReturn = ACTION_SUCCESS;
    //pAccountUserReturnExt2.Operate;
    //pAccountUserReturnExt2.Size;
    //pAccountUserReturnExt2.Type;
    //pAccountUserReturnExt2.uFlag;
    //pAccountUserReturnExt2.Version;

    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::SetAccountVerifyRespondType(WORD wType, BYTE byMibaoMode, int nUserLoginResult)
{
    m_PaysysConfig.wType = wType;
    m_PaysysConfig.byMibaoMode = byMibaoMode;
    m_PaysysConfig.nUserLoginResult = nUserLoginResult;
    return true;
}

int KSimulatePaysys::SetNotHandleProtocol(int nNotHandleProtocol)
{
    m_PaysysConfig.nNotRespondProtocol = nNotHandleProtocol;
    return true;
}

int KSimulatePaysys::DoActivePresentCodeRespond(KGatewayAgency* pGateway, char* szAccount, char* pPresentCode, DWORD dwRoleID, int nPCResult)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    tagProtocolHeader*              ptagProtocolHeader  = NULL;
    IKG_Buffer*                     piSendBuffer        = NULL;
    BYTE*                           pbySendBuffer       = NULL;
    KPaysys2Gameworld*              pPak                = NULL;
    KAccountActivePresentCodeRet*   pRespond            = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KPaysys2Gameworld) + sizeof(KAccountActivePresentCodeRet)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pPak               = (KPaysys2Gameworld*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pRespond = (KAccountActivePresentCodeRet*) pPak->byData;

    ptagProtocolHeader->cProtocol = P2B_PAYSYS_2_GAMEWORLD;
    pPak->Type                    = Paysys2Gameworld;
    pPak->Operate                 = dwRoleID;
    pPak->uDataSize               = sizeof(KAccountActivePresentCodeRet);

    strncpy(pRespond->Account, szAccount, sizeof(pRespond->Account));
    pRespond->Account[sizeof(pRespond->Account) - 1] = '\0';

    strncpy(pRespond->PresentCode, pPresentCode, sizeof(pRespond->PresentCode));
    pRespond->PresentCode[sizeof(pRespond->PresentCode) - 1] = '\0';

    pRespond->nResult = nPCResult;
    //pRespond->dwActiveIP = ;
    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "Paysys ::  DoActivePresentCodeRespond()  [%s]\n", nResult ? "OK" : "Failed");

    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::DoMiBaoVerifyRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                             nResult                     = false;
    int                             nRetCode                    = false;
    tagProtocolHeader*              ptagProtocolHeader          = NULL;
    IKG_Buffer*                     piSendBuffer                = NULL;
    BYTE*                           pbySendBuffer               = NULL;
    KAccountUserPasspodVerify*      pAccountUserPasspodVerify   = (KAccountUserPasspodVerify*)pbyData;
    KAccountUserPasspodVerifyRet*   pRespond                    = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserPasspodVerifyRet)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_PLAYER_PASSPOD_VERIFY;

    pRespond = (KAccountUserPasspodVerifyRet*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pRespond->nReturn = m_PaysysConfig.nMibaoResult;
    pRespond->Operate = pAccountUserPasspodVerify->Operate;
    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

//Exit1:
    nResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "Paysys ::  DoMiBaoVerifyRespond()  [%s]\n", nResult ? "OK" : "Failed");

    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulatePaysys::OnFreezeCoinRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                nResult             = false;
    int                nRetCode            = false;
    //KAccountFreezeFee* pAccountFreezeFee   = (KAccountFreezeFee*)pbyData; 

    m_nFreezeCoinRequest  = true;

    nRetCode = DoFreezeCoinRespond(pGateway, pbyData, uDataLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::OnAccountExchangeRequest(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    //KAccountExchangeEx*     pAccountExchangeEx  = (KAccountExchangeEx*)pbyData;

    m_nAccountExchangeRequest = true;

    nRetCode = DoAccountExchangeRespond(pGateway, pbyData, uDataLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KSimulatePaysys::DoFreezeCoinRespond(KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen)
{
    int                             nResult                 = false;
    int                             nRetCode                = false;
    tagProtocolHeader*              ptagProtocolHeader      = NULL;
    IKG_Buffer*                     piSendBuffer            = NULL;
    BYTE*                           pbySendBuffer           = NULL;
    KAccountFreezeFee*              pAccountFreezeFee       = (KAccountFreezeFee*)pbyData; 
    KAccountFreezeFeeRet*           pRespond                = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountFreezeFeeRet)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_PLAYER_FREEZE_FEE;

    pRespond = (KAccountFreezeFeeRet*)(pbySendBuffer + sizeof(tagProtocolHeader));

    strncpy(pRespond->Account, pAccountFreezeFee->Account, sizeof(pRespond->Account));
    pRespond->Account[sizeof(pRespond->Account) - 1] = '\0';

    pRespond->dwFreezeFlag      = FREEZE;
    pRespond->nResult           = ACTION_SUCCESS;
    pRespond->Operate           = pAccountFreezeFee->Operate;
    pRespond->dwFreezeAmount    = pAccountFreezeFee->dwFreezeAmount;

    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    //Exit1:
    nResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "Paysys ::  DoFreezeCoinRespond()  [%s]\n", nResult ? "OK" : "Failed");

    KG_COM_RELEASE(piSendBuffer);
    return nResult;

}

int KSimulatePaysys::DoAccountExchangeRespond( KGatewayAgency* pGateway, BYTE* pbyData, size_t uDataLen )
{
    int                             nResult                     = false;
    int                             nRetCode                    = false;
    tagProtocolHeader*              ptagProtocolHeader          = NULL;
    IKG_Buffer*                     piSendBuffer                = NULL;
    BYTE*                           pbySendBuffer               = NULL;
    KAccountExchangeEx*             pAccountExchangeEx  = (KAccountExchangeEx*)pbyData;
    KAccountExchangeExRet*          pRespond                    = NULL;

    KGLOG_PROCESS_ERROR(pGateway);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountExchangeExRet)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = P2B_PLAYER_EXCHANGE_EX;

    pRespond = (KAccountExchangeExRet*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pRespond->nResult   = ACTION_SUCCESS;
    pRespond->Operate   = pAccountExchangeEx->Operate;

    nRetCode = pGateway->piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    //Exit1:
    nResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "Paysys ::  DoAccountExchangeRespond()  [%s]\n", nResult ? "OK" : "Failed");

    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}
