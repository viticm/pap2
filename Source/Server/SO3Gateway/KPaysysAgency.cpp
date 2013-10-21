#include "stdafx.h"
#include "KSO3Gateway.h"
#include "Protocol/AccountLoginDef.h"
#include "Protocol/BishopPaysysProtocol.h"
#include "Protocol/PaysysGamewordProtocal.h"

using namespace std;

#define REGISTER_P2B_PROTOCOL(Protocol, Func, DataStruct)                               \
    do                                                                                  \
    {                                                                                   \
        m_PakProcessor[Protocol]    = &KPaysysAgency::Func;                             \
        m_uPakSize[Protocol]        = sizeof(tagProtocolHeader) + sizeof(DataStruct);   \
    } while (false)


KPaysysAgency::KPaysysAgency()
{
    m_pPlayerManager             = NULL;
    m_pRelayAgency               = NULL;
    m_piSocket                   = NULL;
    m_bSocketError               = false;
    m_dwNextPingTime             = 0;
    m_fAccountVerifyTimeCost     = 0.0f;
    m_bZoneChargeFlag            = false;

    memset(m_PakProcessor, 0, sizeof(m_PakProcessor));
    memset(m_uPakSize, 0, sizeof(m_uPakSize));

    REGISTER_P2B_PROTOCOL(P2B_IB_PLAYER_IDENTITY_VERIFY_RESULT_EX, OnAccountVerifyRespond, KAccountUserReturnExtIB2);
    REGISTER_P2B_PROTOCOL(P2B_PLAYER_PASSPOD_VERIFY, OnMibaoVerifyRespond, KAccountUserPasspodVerifyRet);
    REGISTER_P2B_PROTOCOL(P2B_EXT_POINTS_OPERATION_RESULT, OnChangeExtPointRespond, KAccountUserReturnExt2);
    REGISTER_P2B_PROTOCOL(P2B_PLAYER_SET_CHARGE_FLAG_RESULT_EX, OnSetChargeFlagRespond, KAccountSetChargeFlagRetExt);
    REGISTER_P2B_PROTOCOL(P2B_GET_ZONE_CHARGE_FLAG, OnGetZoneChargeFlagRespond, KGetZoneChargeFlagRet);
    REGISTER_P2B_PROTOCOL(P2B_PAYSYS_2_GAMEWORLD, OnActivePresentCodeRespond, KPaysys2Gameworld);
    REGISTER_P2B_PROTOCOL(P2B_PLAYER_FREEZE_FEE, OnFreezeCoinRespond, KAccountFreezeFeeRet);
    REGISTER_P2B_PROTOCOL(P2B_PLAYER_EXCHANGE_EX, OnAccountExchangeRespond, KAccountExchangeExRet);
}

BOOL KPaysysAgency::Init()
{
    BOOL      bResult   = false;
    int       nRetCode  = false;
    IIniFile* piIniFile = NULL;

    piIniFile = g_OpenIniFile(GATEWAY_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString("Paysys", "IP", "", m_szIP, sizeof(m_szIP));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger("Paysys", "Port", 0, &m_nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("Paysys", "UserName", "", m_szUserName, sizeof(m_szUserName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("Paysys", "Password", "", m_szPassword, sizeof(m_szPassword));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger("Paysys", "PingCycle", 2000, &m_nPingCycle);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("Paysys", "LocalIP", "", m_szLocalIP, sizeof(m_szLocalIP));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("Paysys", "InternalIPMask", "", 
        m_szInternalIPMask, sizeof(m_szInternalIPMask)
    );
    KGLOG_PROCESS_ERROR(nRetCode);
	goto Exit0;

    nRetCode = Connect(true);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_bRunFlag = true;

    nRetCode = m_WorkThread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);

	bResult = true;
    return bResult;
}

void KPaysysAgency::UnInit()
{
    m_bRunFlag = false;
    m_WorkThread.Destroy();

    for (KCacheTable::iterator it = m_Cache.begin(); it != m_Cache.end(); ++it)
    {
        KG_COM_RELEASE(*it);
    }

    m_Cache.clear();

    Disconnect();
}

void KPaysysAgency::Activate()
{
    //DoPingSignal();

    //ProcessCache();

    //ProcessPackage();
}

BOOL KPaysysAgency::ProcessPackage()
{
    BOOL                    bResult             = false;
    int                     nRetCode            = false;
    IKG_Buffer*             piPackage           = NULL;
    PP2G_PROTOCOL_FUNC      ProtocolFunc        = NULL;
    BYTE*                   pbyData             = NULL;
    size_t                  uDataLen            = 0;
    tagProtocolHeader*      pHeader             = NULL;

    KG_PROCESS_ERROR(m_piSocket);

    while (true)
    {
        timeval TimeoutValue = {0, 0};

        nRetCode = m_piSocket->CheckCanRecv(&TimeoutValue);
        if (nRetCode == -1)
        {
            m_bSocketError = true;
            goto Exit0;
        }

        if (nRetCode == 0)
            break;

        KG_COM_RELEASE(piPackage);

        nRetCode = m_piSocket->Recv(&piPackage);
        if (nRetCode != 1)
        {
            m_bSocketError = true;
            goto Exit0;
        }

        KGLOG_PROCESS_ERROR(piPackage);

        pbyData = (BYTE*)piPackage->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        uDataLen = piPackage->GetSize();
        KGLOG_PROCESS_ERROR(uDataLen >= sizeof(tagProtocolHeader));

        pHeader = (tagProtocolHeader*)pbyData;
        //KGLOG_PROCESS_ERROR(pHeader->cProtocol >= 0 && pHeader->cProtocol <= UCHAR_MAX);

        ProtocolFunc = m_PakProcessor[pHeader->cProtocol];
        if (ProtocolFunc == NULL)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(uDataLen >= m_uPakSize[pHeader->cProtocol]);

        pbyData     += sizeof(tagProtocolHeader);
        uDataLen    -= sizeof(tagProtocolHeader);

        (this->*ProtocolFunc)(pbyData, uDataLen);
    }

    bResult = true;
Exit0:
    if (m_piSocket && m_bSocketError)
    {
        KGLogPrintf(KGLOG_ERR, "[Paysys] Connection lost !\n");
        KG_COM_RELEASE(m_piSocket);
    }
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KPaysysAgency::ProcessCache()
{
    BOOL    bResult    = false;
    int     nRetCode   = false;
    int     nSendCount = 0;

    KG_PROCESS_ERROR(m_piSocket);

    while (!m_Cache.empty())
    {
        IKG_Buffer* piBuffer = m_Cache.front();

        nRetCode = m_piSocket->Send(piBuffer);
        KG_PROCESS_ERROR(nRetCode == 1);

        KG_COM_RELEASE(piBuffer);

        m_Cache.pop_front();
        nSendCount++;

        if (nSendCount >= 8)
            break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPaysysAgency::Send(IKG_Buffer* piBuffer)
{
    BOOL bResult    = false;
    int  nRetCode   = 0;

    assert(piBuffer);

    KG_PROCESS_ERROR(m_piSocket);

    KG_PROCESS_ERROR(m_Cache.empty());

    nRetCode = m_piSocket->Send(piBuffer);
    if (nRetCode != 1)
    {
        m_bSocketError = true;
        goto Exit0;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        piBuffer->AddRef();
        m_Cache.push_back(piBuffer);
    }
    return bResult;
}

#define  PAYSYS_IDENTIFY                0xAEFC07B5

BOOL KPaysysAgency::Connect(BOOL bNormalLogin)
{
    BOOL                            bResult                 = false;
    int                             nRetCode                = false;
    size_t                          uBufferSize             = 0;
    IKG_Buffer*                     piBuffer                = NULL;
    BYTE*                           pbyBuffer               = NULL;
    tagProtocolHeader*              pHeader                 = NULL;
    IKG_SocketStream*               piSocket                = NULL;
    KServerAccountUserLoginInfo2*   pLoginInfo              = NULL;
    KAccountUserReturnVerify*       pVerifyReturn           = NULL;
    KGetZoneChargeFlag*             pGetZoneCharge          = NULL;
    unsigned long                   ulInternalIPAddress     = INADDR_NONE;
    unsigned long                   ulExternalIPAddress     = INADDR_NONE;
    unsigned long                   ulLocalIPAddress        = INADDR_NONE;
    unsigned long                   ulInternalIPMask        = INADDR_NONE;
    unsigned char                   byInternalMacAddress[6];
    unsigned char                   byExternalMacAddress[6];
    KG_SocketConnector              Connector;    
    timeval                         TimeoutValue;

    assert(m_piSocket == NULL);

    piSocket = Connector.ConnectSecurity(m_szIP, m_nPort, EASYCRYPT_ENCODE_DECODE);
    KGLOG_PROCESS_ERROR(piSocket);       

    TimeoutValue.tv_sec  = 8;
    TimeoutValue.tv_usec = 0;

    nRetCode = piSocket->SetTimeout(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    uBufferSize = sizeof(tagProtocolHeader) + sizeof(KServerAccountUserLoginInfo2);
    piBuffer = KG_MemoryCreateBuffer((unsigned)uBufferSize);
    KGLOG_PROCESS_ERROR(piBuffer);
    
    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    pHeader = (tagProtocolHeader*)pbyBuffer;

    pHeader->cProtocol = bNormalLogin ? B2P_BISHOP_IDENTITY_VERIFY : B2P_BISHOP_RECONNECT_IDENTITY_VERIFY;

    pLoginInfo = (KServerAccountUserLoginInfo2*)(pbyBuffer + sizeof(tagProtocolHeader));   

    pLoginInfo->Size         = sizeof(KServerAccountUserLoginInfo2);
    pLoginInfo->Type         = ServerAccountUserLoginInfo;
    pLoginInfo->Version      = ACCOUNT_CURRENT_VERSION;
    pLoginInfo->Operate      = PAYSYS_IDENTIFY;    
    pLoginInfo->nReserved    = 0;

    strncpy(pLoginInfo->szAccountName, m_szUserName, sizeof(pLoginInfo->szAccountName));
    pLoginInfo->szAccountName[sizeof(pLoginInfo->szAccountName) - 1] = '\0';

    KG_EDStringToMD5String(pLoginInfo->szPassWord, m_szPassword);

    memset(byInternalMacAddress, 0, sizeof(byInternalMacAddress));
    memset(byExternalMacAddress, 0, sizeof(byExternalMacAddress));

    ulInternalIPMask = inet_addr(m_szInternalIPMask);
    KGLOG_PROCESS_ERROR(ulInternalIPMask != INADDR_NONE || m_szInternalIPMask[0] == '\0');

    if (ulInternalIPMask == 0 || ulInternalIPMask == INADDR_NONE)
    {
        ulInternalIPMask = 0x0000a8c0;
    }

    ulInternalIPMask &= 0x0000ffff;

    nRetCode = gGetMacAndIPAddress(
        byInternalMacAddress, &ulInternalIPAddress, byExternalMacAddress, 
        &ulExternalIPAddress, ulInternalIPMask
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    ulLocalIPAddress = inet_addr(m_szLocalIP);
    KGLOG_PROCESS_ERROR(ulLocalIPAddress != INADDR_NONE || m_szLocalIP[0] == '\0') ;

    pLoginInfo->dwUserIP = INADDR_NONE;
    // If not config LocalIPAddress in config file, or config LocalIPAddress to ExternalIPAddress,
    // Then fill pInfo->MacAddress with ExternalMacAddress
    if ((ulLocalIPAddress == 0) || (ulLocalIPAddress == INADDR_NONE) || (ulLocalIPAddress == ulExternalIPAddress))
    {
        assert(sizeof(pLoginInfo->byMacAddress) == sizeof(byExternalMacAddress));
        pLoginInfo->dwUserIP = ulExternalIPAddress;
        memcpy(pLoginInfo->byMacAddress, byExternalMacAddress, sizeof(byExternalMacAddress));
    }
    else if (ulLocalIPAddress == ulInternalIPAddress)
    {
        assert(sizeof(pLoginInfo->byMacAddress) == sizeof(byInternalMacAddress));
        pLoginInfo->dwUserIP = ulInternalIPAddress;
        memcpy(pLoginInfo->byMacAddress, byInternalMacAddress, sizeof(byInternalMacAddress));
    }

    KGLOG_PROCESS_ERROR(pLoginInfo->dwUserIP != INADDR_NONE);

    nRetCode = piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    
    while (m_bRunFlag)
    {
        KG_COM_RELEASE(piBuffer);
        nRetCode = piSocket->Recv(&piBuffer);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        assert(piBuffer);

        uBufferSize = piBuffer->GetSize();
        KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(tagProtocolHeader));

        pbyBuffer = (BYTE*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pbyBuffer);

        pHeader = (tagProtocolHeader*)pbyBuffer;
        
        if (pHeader->cProtocol == P2B_PING)
        {
            continue;
        }
                
        KGLOG_PROCESS_ERROR(pHeader->cProtocol == P2B_BISHOP_IDENTITY_VERIFY_RESULT);
        KGLOG_PROCESS_ERROR(uBufferSize == sizeof(tagProtocolHeader) + sizeof(KAccountUserReturnVerify));

        pVerifyReturn = (KAccountUserReturnVerify*)(pbyBuffer + sizeof(tagProtocolHeader));

        KGLOG_PROCESS_ERROR(pVerifyReturn->nReturn == ACTION_SUCCESS);
        
        break;
    }

    // get zone charge flag
    KG_COM_RELEASE(piBuffer);
    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KGetZoneChargeFlag)));
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    pHeader = (tagProtocolHeader*)pbyBuffer;
    pGetZoneCharge = (KGetZoneChargeFlag*)(pbyBuffer + sizeof(tagProtocolHeader));

    pHeader->cProtocol      = B2P_GET_ZONE_CHARGE_FLAG;
    pGetZoneCharge->Size    = sizeof(KGetZoneChargeFlag);
    pGetZoneCharge->Type    = GetZoneChargeFlag;
    pGetZoneCharge->Version = ACCOUNT_CURRENT_VERSION;
    pGetZoneCharge->Operate = 0;

    strncpy(pGetZoneCharge->szGatewayName, m_szUserName, sizeof(pGetZoneCharge->szGatewayName));
    pGetZoneCharge->szGatewayName[sizeof(pGetZoneCharge->szGatewayName) - 1] = '\0';

    nRetCode = piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_bSocketError = false;

    // 小心: 这里不能写成 "m_piSocket = piSocket; m_piSocket->AddRef(); ", 那样会导致线程安全隐患
    piSocket->AddRef();
    m_piSocket = piSocket;
    
    bResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "Connect to Paysys %s:%d ... ... [%s]\n", m_szIP, m_nPort, bResult ? "OK" : "Failed");
    KG_COM_RELEASE(piBuffer);
    KG_COM_RELEASE(piSocket);    
    return bResult;
}

BOOL KPaysysAgency::Disconnect()
{
    BOOL                            bResult         = false;
    int                             nRetCode        = false;
    size_t                          uBufferSize     = 0;
    IKG_Buffer*                     piBuffer        = NULL;
    BYTE*                           pbyBuffer       = NULL;
    tagProtocolHeader*              pHeader         = NULL;
    KServerAccountUserLoginInfo2*   pLoginInfo      = NULL;

    KG_PROCESS_ERROR(m_piSocket);

    uBufferSize = sizeof(tagProtocolHeader) + (unsigned)sizeof(KServerAccountUserLoginInfo2);

    piBuffer = KG_MemoryCreateBuffer((unsigned)uBufferSize);
    KGLOG_PROCESS_ERROR(piBuffer);
    
    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    memset(pbyBuffer, 0, uBufferSize);

    pHeader = (tagProtocolHeader*)pbyBuffer;

    pHeader->cProtocol = B2P_BISHOP_LOGOUT;

    pLoginInfo = (KServerAccountUserLoginInfo2*)(pbyBuffer + sizeof(tagProtocolHeader));   

    pLoginInfo->Size         = sizeof(KServerAccountUserLoginInfo2);
    pLoginInfo->Type         = ServerAccountUserLoginInfo;
    pLoginInfo->Version      = ACCOUNT_CURRENT_VERSION;
    pLoginInfo->Operate      = PAYSYS_IDENTIFY;    
    pLoginInfo->nReserved    = 0;

    strncpy(pLoginInfo->szAccountName, m_szUserName, sizeof(pLoginInfo->szAccountName));
    pLoginInfo->szAccountName[sizeof(pLoginInfo->szAccountName) - 1] = '\0';

    KG_EDStringToMD5String(pLoginInfo->szPassWord, m_szPassword);
    
    nRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    
    // make sure data is send to paysys
    KGThread_Sleep(2000);

    KGLogPrintf(KGLOG_INFO, "[Paysys] Logout !");
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(m_piSocket);
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KPaysysAgency::GenerateMatrixPosition(KPlayerAgency* pPlayer)
{
    assert(pPlayer);

    pPlayer->szMatrixPosition[0] = (char)('A' + g_Random(8));
    pPlayer->szMatrixPosition[1] = (char)('0' + g_Random(10));
    pPlayer->szMatrixPosition[2] = (char)('A' + g_Random(8));
    pPlayer->szMatrixPosition[3] = (char)('0' + g_Random(10));
    pPlayer->szMatrixPosition[4] = (char)('A' + g_Random(8));
    pPlayer->szMatrixPosition[5] = (char)('0' + g_Random(10));
    pPlayer->szMatrixPosition[6] = '\0';

    return true;
}

void KPaysysAgency::WorkThreadFunction(void* pvParam)
{
    KPaysysAgency* pThis = (KPaysysAgency*)pvParam;

    assert(pThis);
    
    pThis->ThreadFunction();
}

int KPaysysAgency::ThreadFunction()
{
    int     nRetCode            = false;
    time_t  nNextConnectTime    = 0;
    

    while (m_bRunFlag)
    {
        time_t nTimeNow = 0;

        if (m_piSocket != NULL)
        {
            KGThread_Sleep(300);
            continue;
        }

        nTimeNow = time(NULL);

        if (nTimeNow >= nNextConnectTime)
        {
            Connect(false);
            nNextConnectTime = nTimeNow + 10;
        }
    }

    return 0;
}

BOOL KPaysysAgency::DoPingSignal()
{
    BOOL                bResult     = false;
    int                 nRetCode    = 0;
    IKG_Buffer*         piPackage   = NULL;
    tagPING_COMMAND*    pPindPak    = NULL;
    DWORD               dwTimeNow   = 0;

    KG_PROCESS_ERROR(m_piSocket);

    dwTimeNow = KG_GetTickCount();
    KG_PROCESS_ERROR(dwTimeNow >= m_dwNextPingTime);

    piPackage = KG_MemoryCreateBuffer(sizeof(tagPING_COMMAND));
    KGLOG_PROCESS_ERROR(piPackage);
    
    pPindPak = (tagPING_COMMAND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pPindPak);

    pPindPak->ProtocolType  = B2P_PING;
    pPindPak->dwTime        = dwTimeNow;

    nRetCode = m_piSocket->Send(piPackage);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_dwNextPingTime = dwTimeNow + m_nPingCycle * 1000;

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KPaysysAgency::DoAccountVerifyRequest(KPlayerAgency* pPlayer)
{
    BOOL                    bResult                 = false;
    int                     nRetCode                = false;
    size_t                  uBufferLen              = 0;
    IKG_Buffer*             piSendBuffer            = NULL;
    BYTE*                   pbySendBuffer           = NULL;
    tagProtocolHeader*      ptagProtocolHeader      = NULL;
    KAccountUserLoginInfo*  pKAccountUserLoginInfo  = NULL;
    u_short                 uClientPort             = 0;  
    struct in_addr          ClientIP;
    
    uBufferLen = sizeof(tagProtocolHeader) + sizeof(KAccountUserLoginInfo);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uBufferLen);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    memset(pbySendBuffer, 0, uBufferLen);

    nRetCode = pPlayer->piSocket->GetRemoteAddress(&ClientIP, &uClientPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = B2P_IB_PLAYER_IDENTITY_VERIFY;

    pKAccountUserLoginInfo = (KAccountUserLoginInfo*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pKAccountUserLoginInfo->Size     = sizeof(KAccountUserLoginInfo);
    pKAccountUserLoginInfo->Type     = AccountUserLoginInfo;
    pKAccountUserLoginInfo->Version  = ACCOUNT_CURRENT_VERSION;
    pKAccountUserLoginInfo->Operate  = (DWORD)pPlayer->nIndex;
    pKAccountUserLoginInfo->nLogout  = (BYTE)pPlayer->bAccountInGame;
    pKAccountUserLoginInfo->UserIP   = ClientIP.s_addr;
    pKAccountUserLoginInfo->UserPort = uClientPort;

    strncpy(pKAccountUserLoginInfo->Account, pPlayer->szAccount, sizeof(pKAccountUserLoginInfo->Account));
    pKAccountUserLoginInfo->Account[sizeof(pKAccountUserLoginInfo->Account) - 1] = '\0';

    strncpy(pKAccountUserLoginInfo->Password, pPlayer->szPsw, sizeof(pKAccountUserLoginInfo->Password));
    pKAccountUserLoginInfo->Password[sizeof(pKAccountUserLoginInfo->Password) - 1] = '\0';

    strncpy(pKAccountUserLoginInfo->szActivateCode, pPlayer->szAct, sizeof(pKAccountUserLoginInfo->szActivateCode));
    pKAccountUserLoginInfo->szActivateCode[sizeof(pKAccountUserLoginInfo->szActivateCode) - 1] = '\0';
    
    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoAccountVerifySDRequest(KPlayerAgency* pPlayer)
{
    BOOL                        bResult                 = false;
    int                         nRetCode                = false;
    size_t                      uBufferLen              = 0;
    IKG_Buffer*                 piSendBuffer            = NULL;
    BYTE*                       pbySendBuffer           = NULL;
    tagProtocolHeader*          ptagProtocolHeader      = NULL;
    KAccountUserLoginInfoSD*    pKAccountUserLoginInfo  = NULL;
    u_short                     uClientPort             = 0;  
    struct in_addr              ClientIP;

    uBufferLen = sizeof(tagProtocolHeader) + sizeof(KAccountUserLoginInfoSD);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uBufferLen);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    memset(pbySendBuffer, 0, uBufferLen);

    nRetCode = pPlayer->piSocket->GetRemoteAddress(&ClientIP, &uClientPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = B2P_IB_PLAYER_IDENTITY_VERIFY_SD;

    pKAccountUserLoginInfo = (KAccountUserLoginInfoSD*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pKAccountUserLoginInfo->Size     = sizeof(KAccountUserLoginInfoSD);
    pKAccountUserLoginInfo->Type     = AccountUserLoginInfo;
    pKAccountUserLoginInfo->Version  = ACCOUNT_CURRENT_VERSION;
    pKAccountUserLoginInfo->Operate  = (DWORD)pPlayer->nIndex;
    pKAccountUserLoginInfo->nLogout  = (BYTE)pPlayer->bAccountInGame;
    pKAccountUserLoginInfo->UserIP   = ClientIP.s_addr;
    pKAccountUserLoginInfo->UserPort = uClientPort;

    strncpy(pKAccountUserLoginInfo->Account, pPlayer->szAccount, sizeof(pKAccountUserLoginInfo->Account));
    pKAccountUserLoginInfo->Account[sizeof(pKAccountUserLoginInfo->Account) - 1] = '\0';

    strncpy(pKAccountUserLoginInfo->Password, pPlayer->szPsw, sizeof(pKAccountUserLoginInfo->Password));
    pKAccountUserLoginInfo->Password[sizeof(pKAccountUserLoginInfo->Password) - 1] = '\0';

    strncpy(pKAccountUserLoginInfo->szActivateCode, pPlayer->szAct, sizeof(pKAccountUserLoginInfo->szActivateCode));
    pKAccountUserLoginInfo->szActivateCode[sizeof(pKAccountUserLoginInfo->szActivateCode) - 1] = '\0';

    strncpy(pKAccountUserLoginInfo->szIDCard, pPlayer->szIDCard, sizeof(pKAccountUserLoginInfo->szIDCard));
    pKAccountUserLoginInfo->szIDCard[sizeof(pKAccountUserLoginInfo->szIDCard) - 1] = '\0';

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoMibaoVerifyRequest(KPlayerAgency* pPlayer, const char* pszMibaoPassword)
{
    BOOL                            bResult             = false;
    int                             nRetCode            = false;
    IKG_Buffer*                     piSendBuffer        = NULL;
    BYTE*                           pbySendBuffer       = NULL;
    tagProtocolHeader*              ptagProtocolHeader  = NULL;
    KAccountUserPasspodVerifyExt*   pRequest            = NULL;

    assert(pszMibaoPassword);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserPasspodVerifyExt)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;

    ptagProtocolHeader->cProtocol = B2P_PLAYER_PASSPOD_VERIFY_EX;

    pRequest = (KAccountUserPasspodVerifyExt*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pRequest->Size                = sizeof(KAccountUserPasspodVerifyExt);
    pRequest->Type                = AccountUserPasspodVerifyEx;
    pRequest->Version             = ACCOUNT_CURRENT_VERSION;
    pRequest->Operate             = (DWORD)pPlayer->nIndex;
    pRequest->byPasspodVersion    = PASSPODVERSION_ZP3;
    pRequest->byPasspodMode       = pPlayer->byMibaoMode;
    pRequest->byLogoutIfFailed    = 0;
    pRequest->byLogoutDeductFlag  = 0;

    strncpy(pRequest->Account, pPlayer->szAccount, sizeof(pRequest->Account));
    pRequest->Account[sizeof(pRequest->Account) - 1] = '\0';

    if (pPlayer->byMibaoMode == PASSPODMODE_ZPTOKEN)
    {
        strncpy(pRequest->szTokenPassword, pszMibaoPassword, sizeof(pRequest->szTokenPassword));
        pRequest->szTokenPassword[sizeof(pRequest->szTokenPassword) - 1] = '\0';
    }
    else if (pPlayer->byMibaoMode == PASSPODMODE_ZPMATRIX)
    {
        strncpy(pRequest->szMatrixPosition, pPlayer->szMatrixPosition, sizeof(pRequest->szMatrixPosition));
        pRequest->szMatrixPosition[sizeof(pRequest->szMatrixPosition) - 1] = '\0';

        strncpy(pRequest->szMatrixPassword, pszMibaoPassword, sizeof(pRequest->szMatrixPassword));
        pRequest->szMatrixPassword[sizeof(pRequest->szMatrixPassword) - 1] = '\0';
    }

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoAccountLogoutNotify(const char cszAcc[])
{
    BOOL                bResult         = false;
    int                 nRetCode        = false;
    IKG_Buffer*         piSendBuffer    = NULL;
    BYTE*               pbySendBuffer   = NULL;
    tagProtocolHeader*  pProtocolHeader = NULL;
    KAccountUserLogout* pKAccountLogout = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(tagProtocolHeader) + sizeof(KAccountUserLogout));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    pProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pProtocolHeader->cProtocol = B2P_PLAYER_LEAVE_GAME;

    pKAccountLogout = (KAccountUserLogout*)(pbySendBuffer + sizeof(tagProtocolHeader));

    strncpy(pKAccountLogout->Account, cszAcc, sizeof(pKAccountLogout->Account));
    pKAccountLogout->Account[sizeof(pKAccountLogout->Account) - 1] = '\0';

    pKAccountLogout->Size      = sizeof(KAccountUserLogout);
    pKAccountLogout->Type      = AccountUserLogout;
    pKAccountLogout->Version   = ACCOUNT_CURRENT_VERSION;
    pKAccountLogout->nExtPoint = 0;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoChangeExtPointsRequest(const char* pszAccountName, DWORD dwRoleID, WORD wExtPointIndex, WORD wChangeValue)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    BYTE*                       pbySendBuffer   = NULL;
    tagProtocolHeader*          pProtocolHeader = NULL;
    KAccountUserChangeExtPoint* pRequest        = NULL;
    
    assert(pszAccountName);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountUserChangeExtPoint)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    pProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pRequest = (KAccountUserChangeExtPoint*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pProtocolHeader->cProtocol  = B2P_EXT_POINTS_OPERATION;
    pRequest->Size              = sizeof(KAccountUserChangeExtPoint);
    pRequest->Type              = AccountChangeExtPoint;
    pRequest->Version           = ACCOUNT_CURRENT_VERSION;
    pRequest->Operate           = 0;

    strncpy(pRequest->Account, pszAccountName, sizeof(pRequest->Account));
    pRequest->Account[sizeof(pRequest->Account) - 1] = '\0';

    pRequest->uExtPointIndex    = (unsigned)wExtPointIndex;
    pRequest->nChangeValue      = (int)wChangeValue;
    pRequest->nPlayerDataIndex  = (int)dwRoleID;
    pRequest->uFlag             = 0;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoSetChargeFlagRequest(const char* pszAccountName, DWORD dwRoleID, DWORD dwLastLoginIP, int nChargeFlag)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    BYTE*                       pbySendBuffer   = NULL;
    tagProtocolHeader*          pProtocolHeader = NULL;
    KAccountSetChargeFlag*      pRequest        = NULL;

    assert(pszAccountName);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountSetChargeFlag)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    pProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pRequest = (KAccountSetChargeFlag*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pProtocolHeader->cProtocol  = B2P_PLAYER_SET_CHARGE_FLAG;
    pRequest->Size              = sizeof(KAccountSetChargeFlag);
    pRequest->Type              = AccountUserSetChargeFlag;
    pRequest->Version           = ACCOUNT_CURRENT_VERSION;
    pRequest->Operate           = dwRoleID;

    strncpy(pRequest->Account, pszAccountName, sizeof(pRequest->Account));
    pRequest->Account[sizeof(pRequest->Account) - 1] = '\0';

    pRequest->dwLastLoginIP = dwLastLoginIP;
    pRequest->nChargeFlag   = nChargeFlag;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoActivePresentCodeRequest(
    const char* pszAccountName, DWORD dwRoleID, const char* pszPresentCode, DWORD dwActiveIP
)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    BYTE*                       pbySendBuffer   = NULL;
    tagProtocolHeader*          pProtocolHeader = NULL;
    KGameworld2Paysys*          pPakHeader      = NULL;
    KAccountActivePresentCode*  pRequest        = NULL;

    assert(pszAccountName);
    assert(pszPresentCode);

    piSendBuffer = KG_MemoryCreateBuffer(
        (unsigned)(sizeof(tagProtocolHeader) + sizeof(KGameworld2Paysys) + sizeof(KAccountActivePresentCode))
    );
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    pProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pPakHeader = (KGameworld2Paysys*)(pbySendBuffer + sizeof(tagProtocolHeader));
    pRequest = (KAccountActivePresentCode*)(pPakHeader->byData);

    pProtocolHeader->cProtocol  = B2P_GAMEWORLD_2_PAYSYS;
    pPakHeader->Size            = sizeof(KGameworld2Paysys) + sizeof(KAccountActivePresentCode);
    pPakHeader->Type            = Gameworld2Paysys;
    pPakHeader->Version         = ACCOUNT_CURRENT_VERSION;
    pPakHeader->Operate         = dwRoleID;
    pPakHeader->uDataSize       = sizeof(KAccountActivePresentCode);
    pRequest->cProtocol         = G2P_PLAYER_ACTIVE_PRESENT_CODE;
    pRequest->dwActiveIP        = dwActiveIP;

    strncpy(pRequest->Account, pszAccountName, sizeof(pRequest->Account));
    pRequest->Account[sizeof(pRequest->Account) - 1] = '\0';

    strncpy(pRequest->PresentCode, pszPresentCode, sizeof(pRequest->PresentCode));
    pRequest->PresentCode[sizeof(pRequest->PresentCode) - 1] = '\0';

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoFreezeCoinRequest(DWORD dwRequestID, const char* pszAccountName, BOOL bFreezeFlag, int nCoin)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    BYTE*                       pbySendBuffer   = NULL;
    tagProtocolHeader*          pProtocolHeader = NULL;
    KAccountFreezeFee*          pRequest        = NULL;

    assert(pszAccountName);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountFreezeFee)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    pProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pRequest = (KAccountFreezeFee*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pProtocolHeader->cProtocol  = B2P_PLAYER_FREEZE_FEE;
    pRequest->Size              = sizeof(KAccountFreezeFee);
    pRequest->Type              = AccountUserFreezeFee;
    pRequest->Version           = ACCOUNT_CURRENT_VERSION;
    pRequest->Operate           = dwRequestID;

    strncpy(pRequest->Account, pszAccountName, sizeof(pRequest->Account));
    pRequest->Account[sizeof(pRequest->Account) - 1] = '\0';

    pRequest->llFreezeGUID      = 0;
    pRequest->dwFreezeFlag      = bFreezeFlag ? FREEZE : UNFREEZE;
    pRequest->dwFreezeType      = FREEZE_COIN;
    pRequest->dwExtPoint        = 0;
    pRequest->dwFreezeAmount    = (DWORD)nCoin;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KPaysysAgency::DoAccountExchangeRequest(
    DWORD dwRequestID, const char* pszSrcAccount, const char* pszDstAccount, DWORD dwExchangeType,
    DWORD dwDstAmount, DWORD dwSrcAmount
)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    BYTE*                       pbySendBuffer   = NULL;
    tagProtocolHeader*          pProtocolHeader = NULL;
    KAccountExchangeEx*         pRequest        = NULL;

    assert(pszSrcAccount);
    assert(pszDstAccount);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(tagProtocolHeader) + sizeof(KAccountExchangeEx)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    pProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    pRequest = (KAccountExchangeEx*)(pbySendBuffer + sizeof(tagProtocolHeader));

    pProtocolHeader->cProtocol  = B2P_PLAYER_EXCHANGE_EX;
    pRequest->Size              = sizeof(KAccountExchangeEx);
    pRequest->Type              = AccountUserExchangeEx;
    pRequest->Version           = ACCOUNT_CURRENT_VERSION;
    pRequest->Operate           = dwRequestID;

    strncpy(pRequest->szSrcAccount, pszSrcAccount, sizeof(pRequest->szSrcAccount));
    pRequest->szSrcAccount[sizeof(pRequest->szSrcAccount) - 1] = '\0';

    strncpy(pRequest->szDestAccount, pszDstAccount, sizeof(pRequest->szDestAccount));
    pRequest->szDestAccount[sizeof(pRequest->szDestAccount) - 1] = '\0';

    pRequest->dwExchangeType    = dwExchangeType;
    pRequest->dwDestAmount      = dwDstAmount;
    pRequest->dwSrcAmount       = dwSrcAmount;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

void KPaysysAgency::OnAccountVerifyRespond(BYTE* pbyData, size_t uDataLen)
{
    KAccountUserReturnExtIB2*   pRespond           = (KAccountUserReturnExtIB2*)pbyData;
    KPlayerAgency*              pPlayer            = NULL;
    DWORD                       dwVerifyTimeCost   = 0;

    pPlayer = m_pPlayerManager->GetPlayer((int)pRespond->Operate);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForPaysysVerify);

    pPlayer->nChargeFlag               = *(int*)(pRespond->byReserve + 12);

    pPlayer->ExtPointInfo.nExtPoint[0] = (short)pRespond->nExtPointArray[0];
    pPlayer->ExtPointInfo.nExtPoint[1] = (short)pRespond->nExtPointArray[1];
    pPlayer->ExtPointInfo.nExtPoint[2] = (short)pRespond->nExtPointArray[2];
    pPlayer->ExtPointInfo.nExtPoint[3] = (short)pRespond->nExtPointArray[3];
    pPlayer->ExtPointInfo.nExtPoint[4] = (short)pRespond->nExtPointArray[4];
    pPlayer->ExtPointInfo.nExtPoint[5] = (short)pRespond->nExtPointArray[5];
    pPlayer->ExtPointInfo.nExtPoint[6] = (short)pRespond->nExtPointArray[6];
    pPlayer->ExtPointInfo.nExtPoint[7] = (short)pRespond->nExtPointArray[7];
    
    pPlayer->dwEndOfDayTime            = pRespond->dwEndOfDayTime;
    pPlayer->dwLeftTimeOfPoint         = pRespond->dwLeftTimeOfPoint;
    pPlayer->dwLeftTimeOfDay           = pRespond->nLeftTimeOfDay;
    pPlayer->dwEndTimeOfFee            = pRespond->dwEndTimeOfFee;
    pPlayer->dwCoin                    = pRespond->dwLeftMoney;
    pPlayer->dwLimitPlayTimeFlag       = *(DWORD*)pRespond->byReserve;
    pPlayer->dwLimitOnlineSecond       = *(DWORD*)(pRespond->byReserve + 4);
    pPlayer->dwLimitOfflineSecond      = *(DWORD*)(pRespond->byReserve + 8);
    pPlayer->dwLoginTime               = pRespond->dwLoginTime;
    pPlayer->dwLastLoginTime           = pRespond->dwLastLoginTime;
    pPlayer->dwLastLoginIP             = pRespond->dwLastLoginIP;

    pPlayer->byMibaoMode               = *(pRespond->byReserve + 45);

    dwVerifyTimeCost = KG_GetTickCount() - pPlayer->dwVerifyPostTime;

    m_fAccountVerifyTimeCost = m_fAccountVerifyTimeCost * 0.9f + (float)dwVerifyTimeCost * 0.1f;

    if (pPlayer->byMibaoMode == PASSPODMODE_ZPMATRIX)
    {
        GenerateMatrixPosition(pPlayer);
    }

    m_pPlayerManager->DoAccountVerifyRespond(pPlayer, pRespond->nReturn);

    if (pRespond->nReturn != ACTION_SUCCESS)
    {
        m_pPlayerManager->UnlockAccount(pPlayer);
        pPlayer->nState = easWaitForVerifyInfo;
        goto Exit0;
    }

    if (pPlayer->bAccountInGame)
    {
        m_pRelayAgency->DoKickAccountNotify(pPlayer);
        m_pPlayerManager->DoKickAccountNotify(pPlayer);
        m_pPlayerManager->UnlockAccount(pPlayer);
        pPlayer->nState = easWaitForDisconnect;
    }
    else
    {
        if (pPlayer->byMibaoMode == PASSPODMODE_UNBIND)
        {
            m_pRelayAgency->DoQueryRoleList(pPlayer);
            pPlayer->nState = easWaitForRoleList;
        }
        else
        {
            pPlayer->nState = easWaitForMibaoPassword;
        }
    }

Exit0:
    return;
}

void KPaysysAgency::OnMibaoVerifyRespond(BYTE* pbyData, size_t uDataLen)
{
    KAccountUserPasspodVerifyRet*   pRespond            = (KAccountUserPasspodVerifyRet*)pbyData;
    KPlayerAgency*                  pPlayer             = NULL;
    DWORD                           dwVerifyTimeCost    = 0;

    pPlayer = m_pPlayerManager->GetPlayer((int)pRespond->Operate);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForMibaoVerify);

    dwVerifyTimeCost = KG_GetTickCount() - pPlayer->dwVerifyPostTime;

    m_fAccountVerifyTimeCost = m_fAccountVerifyTimeCost * 0.9f + (float)dwVerifyTimeCost * 0.1f;

    if (pRespond->nReturn != ACTION_SUCCESS && pPlayer->byMibaoMode == PASSPODMODE_ZPMATRIX)
    {
        GenerateMatrixPosition(pPlayer);
    }

    m_pPlayerManager->DoMibaoVerifyRespond(pPlayer, pRespond->nReturn);

    if (pRespond->nReturn != ACTION_SUCCESS)
    {
        pPlayer->nState = easWaitForMibaoPassword;
    }
    else
    {
        m_pRelayAgency->DoQueryRoleList(pPlayer);
        pPlayer->nState = easWaitForRoleList;
    }

Exit0:
    return;
}

void KPaysysAgency::OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen)
{
    KAccountUserReturnExt2* pRespond = (KAccountUserReturnExt2*)pbyData;
    BOOL                    bSucceed = false;

    pRespond->Account[sizeof(pRespond->Account) - 1] = '\0';

    KGLogPrintf(
        KGLOG_INFO, "[Paysys] Change ExtPoint result: Acc = %s, RoleID = %d, RetCode = %d",
        pRespond->Account, pRespond->nPlayerDataIndex, pRespond->nReturn
    );

    bSucceed = (pRespond->nReturn == ACTION_SUCCESS);

    m_pRelayAgency->DoChangeExtPointRespond(bSucceed, (DWORD)(pRespond->nPlayerDataIndex));
}

void KPaysysAgency::OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen)
{
    KAccountSetChargeFlagRetExt*    pRespond    = (KAccountSetChargeFlagRetExt*)pbyData;
    BOOL                            bSucceed    = false;
    tagExtPointInfo                 ExtPointInfo;

    bSucceed = ((pRespond->nResult == ACTION_SUCCESS) || (pRespond->nResult == E_ACCOUNT_NODEPOSIT));

    ExtPointInfo.nExtPoint[0] = (short)pRespond->nExtPoint;
    ExtPointInfo.nExtPoint[1] = (short)pRespond->nExtPoint1;
    ExtPointInfo.nExtPoint[2] = (short)pRespond->nExtPoint2;
    ExtPointInfo.nExtPoint[3] = (short)pRespond->nExtPoint3;
    ExtPointInfo.nExtPoint[4] = (short)pRespond->nExtPoint4;
    ExtPointInfo.nExtPoint[5] = (short)pRespond->nExtPoint5;
    ExtPointInfo.nExtPoint[6] = (short)pRespond->nExtPoint6;
    ExtPointInfo.nExtPoint[7] = (short)pRespond->nExtPoint7;

    m_pRelayAgency->DoSetChargeFlagRespond(
        bSucceed, pRespond->Operate, pRespond->nChargeFlag, ExtPointInfo,
        pRespond->dwEndTimeOfFee
    );
}

void KPaysysAgency::OnGetZoneChargeFlagRespond(BYTE* pbyData, size_t uDataLen)
{
    KGetZoneChargeFlagRet*  pRespond = (KGetZoneChargeFlagRet*)pbyData;

    if (pRespond->nResult != ACTION_SUCCESS)
    {
        KGLogPrintf(KGLOG_ERR, "[Paysys] Get ZoneChargeFlag failed! RetCode = %d", pRespond->nResult);
        goto Exit0;
    }

    m_bZoneChargeFlag = pRespond->nZoneChargeFlag;

    m_pRelayAgency->DoGetZoneChargeFlagNotify(pRespond->nZoneChargeFlag);

Exit0:
    return;
}

void KPaysysAgency::OnActivePresentCodeRespond(BYTE* pbyData, size_t uDataLen)
{
    KPaysys2Gameworld*              pPak        = (KPaysys2Gameworld*)pbyData;
    KAccountActivePresentCodeRet*   pRespond    = NULL;
    BOOL                            bSucceed    = false;

    KGLOG_PROCESS_ERROR(pPak->Type == Paysys2Gameworld);
    KGLOG_PROCESS_ERROR(pPak->uDataSize == sizeof(KAccountActivePresentCodeRet));
    pRespond = (KAccountActivePresentCodeRet*)pPak->byData;

    pRespond->Account[sizeof(pRespond->Account) - 1] = '\0';
    pRespond->PresentCode[sizeof(pRespond->PresentCode) - 1] = '\0';

    KGLogPrintf(
        KGLOG_INFO, "[Paysys] Active PresentCode result: Acc = %s, RoleID = %d, PresentCode = %s, RetCode = %d",
        pRespond->Account, pPak->Operate, pRespond->PresentCode, pRespond->nResult
    );

    bSucceed = (pRespond->nResult == ACTION_SUCCESS);

    m_pRelayAgency->DoActivePresentCodeRespond(
        bSucceed, pPak->Operate, pRespond->PresentCode, pRespond->dwPresentType
    );

Exit0:
    return;
}

void KPaysysAgency::OnFreezeCoinRespond(BYTE* pbyData, size_t uDataLen)
{
    KAccountFreezeFeeRet*   pRespond    = (KAccountFreezeFeeRet*)pbyData;
    BOOL                    bSucceed    = false;
    BOOL                    bFreezeFlag = false;

    bSucceed = (pRespond->nResult == ACTION_SUCCESS);
    bFreezeFlag = (pRespond->dwFreezeFlag == FREEZE);

    m_pRelayAgency->DoFreezeCoinRespond(bSucceed, pRespond->Operate, bFreezeFlag, (int)pRespond->dwFreezeAmount);
}

void KPaysysAgency::OnAccountExchangeRespond(BYTE* pbyData, size_t uDataLen)
{
    KAccountExchangeExRet*  pRespond = (KAccountExchangeExRet*)pbyData;
    BOOL                    bSucceed = false;

    bSucceed = (pRespond->nResult == ACTION_SUCCESS);

    m_pRelayAgency->DoAccountExchangeRespond(bSucceed, pRespond->Operate);
}
