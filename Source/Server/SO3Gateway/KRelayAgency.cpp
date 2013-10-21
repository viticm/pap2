#include "stdafx.h"
#include "KSO3Gateway.h"
#include "GatewayDef.h"
#include "GatewayRelayProtocol.h"

using namespace std;

#define REGISTER_R2G_PROTOCOL(Protocol, Func, DataStruct)       \
    do                                                          \
    {                                                           \
        m_PakProcessor[Protocol]    = &KRelayAgency::Func;      \
        m_uPakSize[Protocol]        = sizeof(DataStruct);       \
    } while (false)


KRelayAgency::KRelayAgency()
{
    m_pPlayerManager    = NULL;
    m_pPaysysAgency     = NULL;
    m_pGateway          = NULL;

    m_nGameWorldLowerVersion = 0;
    m_nGameWorldUpperVersion = 0;
    m_dwNextPingTime         = 0;
    m_nPingCycle             = 8;
    m_piSocket               = NULL;
    m_bSocketError           = false;

    memset(m_PakProcessor, 0, sizeof(m_PakProcessor));
    memset(m_uPakSize, 0, sizeof(m_uPakSize));

    REGISTER_R2G_PROTOCOL(r2g_ping_signal, OnPingSignal, R2G_PING_SIGNAL);
    REGISTER_R2G_PROTOCOL(r2g_set_game_world_version, OnSetGameWorldVersion, R2G_SET_GAME_WORLD_VERSION);
    REGISTER_R2G_PROTOCOL(r2g_sync_account_state, OnSyncAccountState, R2G_SYNC_ACCOUNT_STATE);
    REGISTER_R2G_PROTOCOL(r2g_sync_role_list, OnSyncRoleList, R2G_SYNC_ROLE_LIST);
    REGISTER_R2G_PROTOCOL(r2g_sync_newbie_maps, OnSyncNewbieMaps, R2G_SYNC_NEWBIE_MAPS);
    REGISTER_R2G_PROTOCOL(r2g_create_role_respond, OnCreateRoleRespond, R2G_CREATE_ROLE_RESPOND);
    REGISTER_R2G_PROTOCOL(r2g_delete_role_respond, OnDeleteRoleRespond, R2G_DELETE_ROLE_RESPOND);
    REGISTER_R2G_PROTOCOL(r2g_game_login_respond, OnGameLoginRespond, R2G_GAME_LOGIN_RESPOND);
    REGISTER_R2G_PROTOCOL(r2g_player_leave_game, OnPlayerLeaveGame, R2G_PLAYER_LEAVE_GAME);
    REGISTER_R2G_PROTOCOL(r2g_change_ext_point_request, OnChangeExtPointRequest, R2G_CHANGE_EXT_POINT_REQUEST);
    REGISTER_R2G_PROTOCOL(r2g_set_charge_flag_request, OnSetChargeFlagRequest, R2G_SET_CHARGE_FLAG_REQUEST);
    REGISTER_R2G_PROTOCOL(r2g_active_present_code_request, OnActivePresentCodeRequest, R2G_ACTIVE_PRESENT_CODE_REQUEST);
    REGISTER_R2G_PROTOCOL(r2g_freeze_coin_request, OnFreezeCoinRequest, R2G_FREEZE_COIN_REQUEST);
    REGISTER_R2G_PROTOCOL(r2g_account_exchange_request, OnAccountExchangeRequest, R2G_ACCOUNT_EXCHANGE_REQUEST);
    REGISTER_R2G_PROTOCOL(r2g_rename_respond, OnRenameRespond, R2G_RENAME_RESPOND);
}

BOOL KRelayAgency::Init()
{
    BOOL                  bResult               = false;
    int                   nRetCode              = false;
    IIniFile*             piIniFile             = NULL;
    IKG_SocketStream*     piSocket              = NULL;
    int                   nPort                 = 0;
    char                  szIP[_NAME_LEN];
    KG_SocketConnector    Connector;
    timeval               TimeoutValue;

    piIniFile = g_OpenIniFile(GATEWAY_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString("Relay", "IP", "", szIP, sizeof(szIP));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger("Relay", "Port", 0, &nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger("Relay", "PingCycle", 20, &m_nPingCycle);
    KGLOG_PROCESS_ERROR(nRetCode);

    piSocket = Connector.Connect(szIP, nPort);
    KGLOG_PROCESS_ERROR(piSocket);

    TimeoutValue.tv_sec  = 8;
    TimeoutValue.tv_usec = 0;

    nRetCode = piSocket->SetTimeout(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_piSocket = piSocket;
    m_piSocket->AddRef();

    bResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "Connect to Relay %s:%d ... ... [%s]\n", szIP, nPort, bResult ? "OK" : "Failed");
    KG_COM_RELEASE(piSocket);
    KG_COM_RELEASE(piIniFile);
    return bResult;
}

void KRelayAgency::UnInit()
{
    KG_COM_RELEASE(m_piSocket);
}

void KRelayAgency::Activate()
{
    ProcessPackage();

    DoPingSignal();
}

BOOL KRelayAgency::ProcessPackage()
{
    BOOL                    bResult             = false;
    int                     nRetCode            = false;
    IKG_Buffer*             piPackage           = NULL;
    PR2G_PROTOCOL_FUNC      ProtocolFunc        = NULL;
    BYTE*                   pbyData             = NULL;
    size_t                  uDataLen            = 0;
    KGR_PROTOCOL_HEADER*    pHeader             = NULL;

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
		KG_PROCESS_ERROR(nRetCode == 1);

        KG_COM_RELEASE(piPackage);

        nRetCode = m_piSocket->Recv(&piPackage);
        if (nRetCode == -1)
        {
            m_bSocketError = true;
            goto Exit0;
        }
        KG_PROCESS_ERROR(nRetCode == 1);

        KGLOG_PROCESS_ERROR(piPackage);

        pbyData = (BYTE*)piPackage->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        uDataLen = piPackage->GetSize();
        KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KGR_PROTOCOL_HEADER));

        pHeader = (KGR_PROTOCOL_HEADER*)pbyData;
        KGLOG_PROCESS_ERROR(pHeader->byProtocol > r2g_protocol_begin);
        KGLOG_PROCESS_ERROR(pHeader->byProtocol < r2g_protocol_total);

        ProtocolFunc = m_PakProcessor[pHeader->byProtocol];
        if (ProtocolFunc == NULL)
            continue;

        (this->*ProtocolFunc)(pbyData, uDataLen);
    }

    bResult = true;
Exit0:
    if (m_piSocket && m_bSocketError)
    {
        nRetCode = m_piSocket->GetLastError();

        KGLogPrintf(KGLOG_ERR, "[Relay] Connection lost, ErrCode = %d\n", nRetCode);

        m_pGateway->Quit();
        KG_COM_RELEASE(m_piSocket);
    }
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayAgency::Send(IKG_Buffer* piBuffer)
{
    BOOL bResult    = false;
    int  nRetCode   = 0;

    assert(piBuffer);
    KGLOG_PROCESS_ERROR(m_piSocket);

    nRetCode = m_piSocket->Send(piBuffer);
    if (nRetCode != 1)
    {
        m_bSocketError = true;
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRelayAgency::GetGameWorldVersion(int* pnLowerVersion, int* pnUpperVersion)
{
    if (m_nGameWorldLowerVersion == 0 && m_nGameWorldUpperVersion == 0)
    {
        return false;
    }

    *pnLowerVersion = m_nGameWorldLowerVersion;
    *pnUpperVersion = m_nGameWorldUpperVersion;

    return true;
}

BOOL KRelayAgency::IsAccountBlocked(const char cszAcc[])
{
    return false;
}

BOOL KRelayAgency::DoPingSignal()
{
    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    DWORD               dwNowTime       = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_PING_SIGNAL*    pPing           = NULL;

    KG_PROCESS_ERROR(m_piSocket);

    dwNowTime = KG_GetTickCount();

    KG_PROCESS_ERROR(dwNowTime >= m_dwNextPingTime);

    m_dwNextPingTime = dwNowTime + 1000 * m_nPingCycle;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_PING_SIGNAL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPing = (G2R_PING_SIGNAL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPing);

    pPing->byProtocol   = g2r_ping_signal;
    pPing->dwTime       = dwNowTime;

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoQueryAccountState(KPlayerAgency* pPlayer)
{
    BOOL                      bResult            = false;
    BOOL                      bRetCode           = false;
    IKG_Buffer*               piSendBuffer       = NULL;
    G2R_QUERY_ACCOUNT_STATE*  pQueryAccountState = NULL;

    assert(pPlayer);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_QUERY_ACCOUNT_STATE));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pQueryAccountState = (G2R_QUERY_ACCOUNT_STATE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pQueryAccountState);

    pQueryAccountState->byProtocol      = g2r_query_account_state;
    pQueryAccountState->nPlayerIndex    = pPlayer->nIndex;

    strncpy(pQueryAccountState->szAccount, pPlayer->szAccount, sizeof(pQueryAccountState->szAccount));
    pQueryAccountState->szAccount[sizeof(pQueryAccountState->szAccount) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoKickAccountNotify(KPlayerAgency* pPlayer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    G2R_KICK_ACCOUNT_NOTIFY*    pNotify         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_KICK_ACCOUNT_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);
    
    pNotify = (G2R_KICK_ACCOUNT_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->byProtocol = g2r_kick_account_notify;

    strncpy(pNotify->szAccount, pPlayer->szAccount, sizeof(pNotify->szAccount));
    pNotify->szAccount[sizeof(pNotify->szAccount) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoQueryRoleList(KPlayerAgency* pPlayer)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;    
    IKG_Buffer*             piSendBuffer    = NULL;
    G2R_QUERY_ROLE_LIST*    pRequest        = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_QUERY_ROLE_LIST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (G2R_QUERY_ROLE_LIST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol    = g2r_query_role_list;
    pRequest->nPlayerIndex  = pPlayer->nIndex;

    strncpy(pRequest->szAccount, pPlayer->szAccount, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoQueryNewbieMaps(int nPlayerIndex)
{
    BOOL                    bResult      = false;
    BOOL                    bRetCode     = false;
    IKG_Buffer*             piSendBuffer = NULL;
    G2R_QUERY_NEWBIE_MAPS*  pRequest     = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_QUERY_NEWBIE_MAPS));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (G2R_QUERY_NEWBIE_MAPS*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol   = g2r_query_newbie_maps;
    pRequest->nPlayerIndex = nPlayerIndex;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoCreateRoleRequest(int nPlayerIndex, const char cszAcc[], BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    G2R_CREATE_ROLE_REQUEST*    pRequest        = NULL;
    IKG_Buffer*                 piSendBuffer    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(unsigned(sizeof(G2R_CREATE_ROLE_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (G2R_CREATE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol    = g2r_create_role_request;
    pRequest->nPlayerIndex  = nPlayerIndex;

    strncpy(pRequest->szAccount, cszAcc, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    memcpy(pRequest->byData, pbyData, uDataLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoDeleteRoleRequest(KPlayerAgency* pPlayer, DWORD dwRoleID)
{
    BOOL                        bResult      = false;
    BOOL                        bRetCode     = false;
    G2R_DELETE_ROLE_REQUEST*    pRequest     = NULL;
    IKG_Buffer*                 piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_DELETE_ROLE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (G2R_DELETE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol    = g2r_delete_role_request;
    pRequest->nPlayerIndex  = pPlayer->nIndex;
    pRequest->dwRoleID      = dwRoleID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoGameLoginRequest(KPlayerAgency* pPlayer, DWORD dwRoleID, BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult      = false;
    BOOL                        bRetCode     = false;
    G2R_GAME_LOGIN_REQUEST*     pRequest     = NULL;
    IKG_Buffer*                 piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_GAME_LOGIN_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (G2R_GAME_LOGIN_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol    = g2r_game_login_request;
    pRequest->nPlayerIndex  = pPlayer->nIndex;

    strncpy(pRequest->szAccountName, pPlayer->szAccount, sizeof(pRequest->szAccountName));
    pRequest->szAccountName[sizeof(pRequest->szAccountName) - 1] = '\0';

    pRequest->dwRoleID              = dwRoleID;
    pRequest->dwQueueID             = pPlayer->dwQueueID;
    pRequest->nChargeFlag           = pPlayer->nChargeFlag;
    pRequest->ExtPointInfo          = pPlayer->ExtPointInfo;
    pRequest->dwEndTimeOfFee        = pPlayer->dwEndTimeOfFee;
    pRequest->dwCoin                = pPlayer->dwCoin;
    pRequest->dwLimitPlayTimeFlag   = pPlayer->dwLimitPlayTimeFlag;
    pRequest->dwLimitOnlineSecond   = pPlayer->dwLimitOnlineSecond;
    pRequest->dwLimitOfflineSecond  = pPlayer->dwLimitOfflineSecond;

    memcpy(pRequest->byData, pbyData, uDataLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoChangeExtPointRespond(BOOL bSucceed, DWORD dwRoleID)
{
    BOOL                            bResult      = false;
    BOOL                            bRetCode     = false;
    G2R_CHANGE_EXT_POINT_RESPOND*   pRespond     = NULL;
    IKG_Buffer*                     piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_CHANGE_EXT_POINT_RESPOND)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (G2R_CHANGE_EXT_POINT_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = g2r_change_ext_point_respond;
    pRespond->bSucceed      = bSucceed;
    pRespond->dwRoleID      = dwRoleID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoSetChargeFlagRespond(
    BOOL bSucceed, DWORD dwRoleID, int nChargeFlag, const tagExtPointInfo& rExtPointInfo,
    DWORD dwEndTimeOfFee
)
{
    BOOL                            bResult      = false;
    BOOL                            bRetCode     = false;
    G2R_SET_CHARGE_FLAG_RESPOND*    pRequest     = NULL;
    IKG_Buffer*                     piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_SET_CHARGE_FLAG_RESPOND)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (G2R_SET_CHARGE_FLAG_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol        = g2r_set_charge_flag_respond;
    pRequest->bSucceed          = bSucceed;
    pRequest->dwRoleID          = dwRoleID;
    pRequest->nChargeFlag       = nChargeFlag;
    pRequest->ExtPointInfo      = rExtPointInfo;
    pRequest->dwEndTimeOfFee    = dwEndTimeOfFee;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoGetZoneChargeFlagNotify(int nZoneChargeFlag)
{
    BOOL                                bResult      = false;
    BOOL                                bRetCode     = false;
    G2R_GET_ZONE_CHARGE_FLAG_NOTIFY*    pNotify      = NULL;
    IKG_Buffer*                         piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_GET_ZONE_CHARGE_FLAG_NOTIFY)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pNotify = (G2R_GET_ZONE_CHARGE_FLAG_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->byProtocol         = g2r_get_zone_charge_flag_notify;
    pNotify->nZoneChargeFlag    = nZoneChargeFlag;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoActivePresentCodeRespond(BOOL bSucceed, DWORD dwRoleID, const char* pszPresentCode, DWORD dwPresentType)
{
    BOOL                                bResult      = false;
    BOOL                                bRetCode     = false;
    G2R_ACTIVE_PRESENT_CODE_RESPOND*    pRespond     = NULL;
    IKG_Buffer*                         piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_ACTIVE_PRESENT_CODE_RESPOND)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (G2R_ACTIVE_PRESENT_CODE_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = g2r_active_present_code_respond;
    pRespond->bSucceed      = bSucceed;
    pRespond->dwRoleID      = dwRoleID;
    pRespond->dwPresentType = dwPresentType;

    strncpy(pRespond->szPresentCode, pszPresentCode, sizeof(pRespond->szPresentCode));
    pRespond->szPresentCode[sizeof(pRespond->szPresentCode) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoFreezeCoinRespond(BOOL bSucceed, DWORD dwRequestID, BOOL bFreezeFlag, int nCoin)
{
    BOOL                        bResult      = false;
    BOOL                        bRetCode     = false;
    G2R_FREEZE_COIN_RESPOND*    pRespond     = NULL;
    IKG_Buffer*                 piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_FREEZE_COIN_RESPOND)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (G2R_FREEZE_COIN_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = g2r_freeze_coin_respond;
    pRespond->bSucceed      = bSucceed;
    pRespond->dwRequestID   = dwRequestID;
    pRespond->bFreezeFlag   = bFreezeFlag;
    pRespond->nCoin         = nCoin;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoAccountExchangeRespond(BOOL bSucceed, DWORD dwRequestID)
{
    BOOL                            bResult      = false;
    BOOL                            bRetCode     = false;
    G2R_ACCOUNT_EXCHANGE_RESPOND*   pRespond     = NULL;
    IKG_Buffer*                     piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(G2R_ACCOUNT_EXCHANGE_RESPOND)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (G2R_ACCOUNT_EXCHANGE_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = g2r_account_exchange_respond;
    pRespond->bSucceed      = bSucceed;
    pRespond->dwRequestID   = dwRequestID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayAgency::DoRenameRequest(int nPlayerIndex, DWORD dwRoleID, char szNewName[])
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    G2R_RENAME_REQUEST* pRenameRequest  = NULL;
    IKG_Buffer*         piSendBuffer    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_RENAME_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRenameRequest = (G2R_RENAME_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRenameRequest);

    pRenameRequest->byProtocol      = g2r_rename_request;
    pRenameRequest->nPlayerIndex    = nPlayerIndex;
    pRenameRequest->dwRoleID        = dwRoleID;
    
    strncpy(pRenameRequest->szNewName, szNewName, sizeof(pRenameRequest->szNewName));
    pRenameRequest->szNewName[sizeof(pRenameRequest->szNewName) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}


void KRelayAgency::OnPingSignal(BYTE* pbyData, size_t uDataLen)
{
}

void KRelayAgency::OnSetGameWorldVersion(BYTE* pbyData, size_t uDataLen)
{
    R2G_SET_GAME_WORLD_VERSION*     pSetVersion = (R2G_SET_GAME_WORLD_VERSION*)pbyData;

    m_nGameWorldLowerVersion = pSetVersion->nLowerVersion;
    m_nGameWorldUpperVersion = pSetVersion->nUpperVersion;
}

void KRelayAgency::OnSyncAccountState(BYTE* pbyData, size_t uDataLen)
{
    R2G_SYNC_ACCOUNT_STATE* pRespond = (R2G_SYNC_ACCOUNT_STATE*)pbyData;
    KPlayerAgency*          pPlayer  = NULL;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(pPlayer->nState == easQueryAccountState);

    pPlayer->bAccountInGame = pRespond->bOnline;

    if (m_pGateway->m_bSndaAgent)
    {
        pPlayer->nState = easWaitForSDOAVerify;
        m_pSndaAgency->DoTokenVerifyRequest(pPlayer);
    }
    else
    {
        pPlayer->dwVerifyPostTime = KG_GetTickCount();

        //pPlayer->nState = easWaitForPaysysVerify;
       // m_pPaysysAgency->DoAccountVerifyRequest(pPlayer);
		DoQueryRoleList(pPlayer);
		pPlayer->nState = easWaitForRoleList;
    }

Exit0:
    return;
}

void KRelayAgency::OnSyncRoleList(BYTE* pbyData, size_t uDataLen)
{
    R2G_SYNC_ROLE_LIST* pRespond        = (R2G_SYNC_ROLE_LIST*)pbyData;
    KPlayerAgency*      pPlayer         = NULL;
    IKG_Buffer*         piRoleData      = NULL;
    BYTE*               pbyBuffer       = NULL;
    BYTE*               pbyRoleData     = NULL;
    size_t              uRoleDataLen    = 0;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForRoleList);

    uRoleDataLen = uDataLen - sizeof(R2G_SYNC_ROLE_LIST);

    if (pRespond->nRoleIndex > 0 && uRoleDataLen > 0)
    {
        std::pair<KROLE_LIST::iterator, BOOL> InsRet;

        pbyRoleData = pRespond->byData;

        piRoleData = KG_MemoryCreateBuffer((unsigned)uRoleDataLen);
        KGLOG_PROCESS_ERROR(piRoleData);

        pbyBuffer = (BYTE*)piRoleData->GetData();
        KGLOG_PROCESS_ERROR(pbyBuffer);

        memcpy(pbyBuffer, pbyRoleData, uRoleDataLen);

        InsRet = pPlayer->RoleList.insert(std::make_pair(pRespond->dwRoleID, piRoleData));
        KGLOG_PROCESS_ERROR(InsRet.second);
        
        InsRet.first->second->AddRef();
    }

    m_pPlayerManager->DoSyncRoleList(
        pPlayer, pRespond->nRoleIndex, pRespond->nRoleCount, pbyRoleData, uRoleDataLen
    );

    if (pRespond->nRoleIndex == pRespond->nRoleCount)
    {
        pPlayer->nState = easWaitForPlayerOperation;
    }

Exit0:
    KG_COM_RELEASE(piRoleData);
    return;
}

void KRelayAgency::OnCreateRoleRespond(BYTE* pbyData, size_t uDataLen)
{
    R2G_CREATE_ROLE_RESPOND*    pRespond        = (R2G_CREATE_ROLE_RESPOND*)pbyData;
    KPlayerAgency*              pPlayer         = NULL;
    BYTE*                       pbyBuffer       = NULL;
    IKG_Buffer*                 piRoleData      = NULL;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForCreateRoleRespond);

    if (pRespond->byCode == eCreateRoleSucceed)
    {
        size_t                                  uRoleDataLen    = 0;
        std::pair<KROLE_LIST::iterator, BOOL>   InsRet;

        uRoleDataLen = uDataLen - sizeof(R2G_CREATE_ROLE_RESPOND);
        KGLOG_PROCESS_ERROR(uRoleDataLen > 0);

        piRoleData = KG_MemoryCreateBuffer((unsigned)uRoleDataLen);
        KGLOG_PROCESS_ERROR(piRoleData);

        pbyBuffer = (BYTE*)piRoleData->GetData();
        KGLOG_PROCESS_ERROR(pbyBuffer);

        memcpy(pbyBuffer, pRespond->byData, uRoleDataLen);

        InsRet = pPlayer->RoleList.insert(std::make_pair(pRespond->dwRoleID, piRoleData));
        KGLOG_PROCESS_ERROR(InsRet.second);

        InsRet.first->second->AddRef();

        m_pPlayerManager->DoCreateRoleRespond(pPlayer, pRespond->byCode, pRespond->dwRoleID, pRespond->byData, uRoleDataLen);
    }
    else
    {
        m_pPlayerManager->DoCreateRoleRespond(pPlayer, pRespond->byCode, pRespond->dwRoleID, NULL, 0);
    }

    pPlayer->nState = easWaitForPlayerOperation;
Exit0:
    KG_COM_RELEASE(piRoleData);
    return;
}

void KRelayAgency::OnDeleteRoleRespond(BYTE* pbyData, size_t uDataLen)
{
    R2G_DELETE_ROLE_RESPOND*    pRespond = (R2G_DELETE_ROLE_RESPOND*)pbyData;
    KPlayerAgency*              pPlayer;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForDeleteRoleRespond);

    if (pRespond->byCode == eDeleteRoleSucceed)
    {
        KROLE_LIST::iterator it = pPlayer->RoleList.find(pRespond->dwRoleID);

        KGLOG_PROCESS_ERROR(it != pPlayer->RoleList.end());

        KG_COM_RELEASE(it->second);

        pPlayer->RoleList.erase(it);
    }

    m_pPlayerManager->DoDeleteRoleRespond(pPlayer, pRespond->byCode, pRespond->dwRoleID, pRespond->nDeleteTime);

    pPlayer->nState = easWaitForPlayerOperation;
Exit0:
    return;
}

void KRelayAgency::OnGameLoginRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bRetCode     = false;
    R2G_GAME_LOGIN_RESPOND* pRespond     = (R2G_GAME_LOGIN_RESPOND*)pbyData;
    KPlayerAgency*          pPlayer      = NULL;
    KPlayerQueue*           pPlayerQueue = NULL;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForLoginPermission);

    switch (pRespond->byCode)
    {
    case eGameLoginSucceed:
        if (pPlayer->dwQueueID != INVALID_QUEUE_ID)
        {
            pPlayerQueue = m_pQueueManager->GetQueue(pPlayer->dwQueueID);
            KGLOG_PROCESS_ERROR(pPlayerQueue);

            bRetCode = pPlayerQueue->Dequeue(pPlayer->nIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            pPlayer->dwQueueID = INVALID_QUEUE_ID;

            pPlayerQueue->m_nNextRetryTime = 0;
        }

        m_pPlayerManager->UnlockAccount(pPlayer);

        m_pPlayerManager->DoGameLoginRespond(
            pPlayer, pRespond->byCode, pRespond->dwRoleID, 
            pRespond->dwIP, pRespond->nPort, pRespond->guid
        );

        pPlayer->nState = easWaitForDisconnect;
        break;

    case eGameLoginQueueWait:
        if (pPlayer->dwQueueID != INVALID_QUEUE_ID)
        {
            if (pRespond->dwQueueID != pPlayer->dwQueueID)
            {
                pPlayerQueue = m_pQueueManager->GetQueue(pPlayer->dwQueueID);
                KGLOG_PROCESS_ERROR(pPlayerQueue);

                bRetCode = pPlayerQueue->Dequeue(pPlayer->nIndex);
                KGLOG_PROCESS_ERROR(bRetCode);

                pPlayer->dwQueueID = INVALID_QUEUE_ID;

                pPlayerQueue->m_nNextRetryTime = 0;

                pPlayerQueue = m_pQueueManager->GetQueue(pRespond->dwQueueID);
                if (!pPlayerQueue)
                {
                    pPlayerQueue = m_pQueueManager->CreateQueue(pRespond->dwQueueID);
                    KGLOG_PROCESS_ERROR(pPlayerQueue);
                }

                bRetCode = pPlayerQueue->Enqueue(pPlayer->nIndex, pRespond->dwRoleID);
                KGLOG_PROCESS_ERROR(bRetCode);

                pPlayer->dwQueueID = pRespond->dwQueueID;
            }
        }
        else
        {
            pPlayerQueue = m_pQueueManager->GetQueue(pRespond->dwQueueID);
            if (!pPlayerQueue)
            {
                pPlayerQueue = m_pQueueManager->CreateQueue(pRespond->dwQueueID);
                KGLOG_PROCESS_ERROR(pPlayerQueue);
            }

            bRetCode = pPlayerQueue->Enqueue(pPlayer->nIndex, pRespond->dwRoleID);
            KGLOG_PROCESS_ERROR(bRetCode);

            pPlayer->dwQueueID = pRespond->dwQueueID;
        }

        pPlayer->nState = easQueueWaiting;
        break;

    case eGameLoginSystemMaintenance:
    case eGameLoginRoleFreeze:
    case eGameLoginUnknownError:
    case eGameLoginOverload:
        if (pPlayer->dwQueueID != INVALID_QUEUE_ID)
        {
            pPlayerQueue = m_pQueueManager->GetQueue(pPlayer->dwQueueID);
            KGLOG_PROCESS_ERROR(pPlayerQueue);

            bRetCode = pPlayerQueue->Dequeue(pPlayer->nIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            pPlayer->dwQueueID = INVALID_QUEUE_ID;
        }

        m_pPlayerManager->DoGameLoginRespond(
            pPlayer, pRespond->byCode, pRespond->dwRoleID, 
            pRespond->dwIP, pRespond->nPort, pRespond->guid
        );

        pPlayer->nState = easWaitForPlayerOperation;
        break;

    default:
        KGLogPrintf(KGLOG_ERR, "Unexpected login respond: %d\n", pRespond->byCode);
        break;
    }

Exit0:
    return;
}

void KRelayAgency::OnSyncNewbieMaps(BYTE* pbyData, size_t uDataLen)
{
    R2G_SYNC_NEWBIE_MAPS*   pRespond    = (R2G_SYNC_NEWBIE_MAPS*)pbyData;
    KPlayerAgency*          pPlayer     = NULL;
    size_t                  uCheckSize  = 0;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForNewbieMapRespond);

    uCheckSize = sizeof(R2G_SYNC_NEWBIE_MAPS) + sizeof(KNEWBIE_MAP_COPY_INFO) * pRespond->nCopyCount;

    KGLOG_PROCESS_ERROR(uDataLen == uCheckSize);
    
    m_pPlayerManager->DoSyncNewbieMaps(
        pPlayer, pRespond->dwMapID, pRespond->CopyInfo, pRespond->nCopyCount
    );

    pPlayer->nState = easWaitForPlayerOperation;
Exit0:
    return;
}

void KRelayAgency::OnPlayerLeaveGame(BYTE* pbyData, size_t uDataLen)
{
    R2G_PLAYER_LEAVE_GAME*  pNotify = (R2G_PLAYER_LEAVE_GAME*)pbyData;

    m_pPaysysAgency->DoAccountLogoutNotify(pNotify->szAccount);
}

void KRelayAgency::OnChangeExtPointRequest(BYTE* pbyData, size_t uDataLen)
{
    R2G_CHANGE_EXT_POINT_REQUEST*   pRequest = (R2G_CHANGE_EXT_POINT_REQUEST*)pbyData;

    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    KGLOG_PROCESS_ERROR(pRequest->wExtPointIndex < MAX_EXT_POINT_COUNT);

    m_pPaysysAgency->DoChangeExtPointsRequest(
        pRequest->szAccount, pRequest->dwRoleID, pRequest->wExtPointIndex, pRequest->wChangeValue
    );

Exit0:
    return;
}

void KRelayAgency::OnSetChargeFlagRequest(BYTE* pbyData, size_t uDataLen)
{
    R2G_SET_CHARGE_FLAG_REQUEST*   pRequest = (R2G_SET_CHARGE_FLAG_REQUEST*)pbyData;

    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    m_pPaysysAgency->DoSetChargeFlagRequest(
        pRequest->szAccount, pRequest->dwRoleID, pRequest->dwLastLoginIP, pRequest->nChargeFlag
    );
}

void KRelayAgency::OnActivePresentCodeRequest(BYTE* pbyData, size_t uDataLen)
{
    R2G_ACTIVE_PRESENT_CODE_REQUEST* pRequest = (R2G_ACTIVE_PRESENT_CODE_REQUEST*)pbyData;

    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';
    pRequest->szPresentCode[sizeof(pRequest->szPresentCode) - 1] = '\0';

    m_pPaysysAgency->DoActivePresentCodeRequest(
        pRequest->szAccount, pRequest->dwRoleID, pRequest->szPresentCode, pRequest->dwActiveIP
    );
}

void KRelayAgency::OnFreezeCoinRequest(BYTE* pbyData, size_t uDataLen)
{
    R2G_FREEZE_COIN_REQUEST* pRequest = (R2G_FREEZE_COIN_REQUEST*)pbyData;

    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    m_pPaysysAgency->DoFreezeCoinRequest(
        pRequest->dwRequestID, pRequest->szAccount, pRequest->bFreezeFlag, pRequest->nCoin
    );
}

void KRelayAgency::OnAccountExchangeRequest(BYTE* pbyData, size_t uDataLen)
{
    R2G_ACCOUNT_EXCHANGE_REQUEST* pRequest = (R2G_ACCOUNT_EXCHANGE_REQUEST*)pbyData;

    pRequest->szSrcAccount[sizeof(pRequest->szSrcAccount) - 1] = '\0';
    pRequest->szDstAccount[sizeof(pRequest->szDstAccount) - 1] = '\0';

    m_pPaysysAgency->DoAccountExchangeRequest(
        pRequest->dwRequestID, pRequest->szSrcAccount, pRequest->szDstAccount, pRequest->dwExchangeType,
        pRequest->dwDstAmount, pRequest->dwSrcAmount
    );
}

void KRelayAgency::OnRenameRespond(BYTE* pbyData, size_t uDataLen)
{
    R2G_RENAME_RESPOND*     pRespond        = (R2G_RENAME_RESPOND*)pbyData;
    KPlayerAgency*          pPlayer         = NULL;
    BYTE*                   pbyRoleListInfo = NULL;
    size_t                  uSendDataLen    = 0;
    IKG_Buffer*             piBuffer        = NULL;
    KROLE_LIST::iterator    itFind;

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->nState == easWaitForRenameRespond);

    switch (pRespond->byCode)
    {
    case eRenameSucceed:
        {
            std::pair<KROLE_LIST::iterator, BOOL> InsRet;

            itFind = pPlayer->RoleList.find(pRespond->dwRoleID);
            KGLOG_PROCESS_ERROR(itFind != pPlayer->RoleList.end());

            KG_COM_RELEASE(itFind->second);

            pPlayer->RoleList.erase(itFind);

            uSendDataLen = uDataLen - sizeof(R2G_RENAME_RESPOND);
            piBuffer = KG_MemoryCreateBuffer((unsigned)uSendDataLen);
            KGLOG_PROCESS_ERROR(piBuffer);

            pbyRoleListInfo = (BYTE*)piBuffer->GetData();

            memcpy(pbyRoleListInfo, pRespond->byData, uSendDataLen);

            InsRet = pPlayer->RoleList.insert(std::make_pair(pRespond->dwRoleID, piBuffer));
            KGLOG_PROCESS_ERROR(InsRet.second);

            InsRet.first->second->AddRef();

            m_pPlayerManager->DoRenameRespond(
                pPlayer, pRespond->byCode, pRespond->dwRoleID, pbyRoleListInfo, uSendDataLen
            );
        }

        pPlayer->nState = easWaitForPlayerOperation;

        break;

    case eRenameNewNameTooLong:
    case eRenameNewNameTooShort:
    case eRenameNameAlreadyExist:
    case eRenameNewNameError:
    case eRenameUnknownError:
        m_pPlayerManager->DoRenameRespond(
            pPlayer, pRespond->byCode, pRespond->dwRoleID, NULL, 0
        );

        pPlayer->nState = easWaitForPlayerOperation;

        break;

    default:
        KGLogPrintf(KGLOG_ERR, "Unexpected rename respond: %d\n", pRespond->byCode);
        break;
    }
Exit0:
    KG_COM_RELEASE(piBuffer);
    return;
}
