#include "stdafx.h"
#include "KGatewayAgency.h"
#include "KSO3GameCenter.h"
#include "GatewayRelayProtocol.h"

#define REGISTER_G2R_FUNCTION(id, func, data)               \
    do                                                      \
    {                                                       \
        m_ProtocolFunctions[id] = &KGatewayAgency::func;    \
        m_uPakSize[id]          = sizeof(data);             \
    } while(false)

KGatewayAgency::KGatewayAgency()
{
     m_piSocket         = NULL;
     m_nLastPingTime    = 0;
     m_nPingCycle       = 30;
     m_bQuiting         = false;
}

KGatewayAgency::~KGatewayAgency()
{
}

BOOL KGatewayAgency::Init()
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    timeval             TimeoutValue    = {0, 0};
    int                 nOpenPortFlag   = false;
    
    m_nPingCycle = g_pSO3GameCenter->m_Settings.m_nGatewayPingCycle;

    bRetCode = m_Acceptor.Open(g_pSO3GameCenter->m_Settings.m_szGatewayIP, g_pSO3GameCenter->m_Settings.m_nGatewayPort);
    KGLOG_PROCESS_ERROR(bRetCode);
    nOpenPortFlag = true;

    m_Acceptor.SetTimeout(&TimeoutValue);   

    memset(m_ProtocolFunctions, 0, sizeof(m_ProtocolFunctions));
    memset(m_uPakSize, 0, sizeof(m_uPakSize));

    REGISTER_G2R_FUNCTION(g2r_ping_signal, OnPingSignal, G2R_PING_SIGNAL);
    REGISTER_G2R_FUNCTION(g2r_query_account_state, OnQueryAccountState, G2R_QUERY_ACCOUNT_STATE);
    REGISTER_G2R_FUNCTION(g2r_query_role_list, OnQueryRoleList, G2R_QUERY_ROLE_LIST);
    REGISTER_G2R_FUNCTION(g2r_query_newbie_maps, OnQueryNewbieMaps, G2R_QUERY_NEWBIE_MAPS);
    REGISTER_G2R_FUNCTION(g2r_kick_account_notify, OnKickAccountNotify, G2R_KICK_ACCOUNT_NOTIFY);
    REGISTER_G2R_FUNCTION(g2r_create_role_request, OnCreateRoleRequest, G2R_CREATE_ROLE_REQUEST);
    REGISTER_G2R_FUNCTION(g2r_delete_role_request, OnDeleteRoleRequest, G2R_DELETE_ROLE_REQUEST);
    REGISTER_G2R_FUNCTION(g2r_game_login_request, OnGameLoginRequest, G2R_GAME_LOGIN_REQUEST);
    REGISTER_G2R_FUNCTION(g2r_change_ext_point_respond, OnChangeExtPointRespond, G2R_CHANGE_EXT_POINT_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_set_charge_flag_respond, OnSetChargeFlagRespond, G2R_SET_CHARGE_FLAG_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_get_zone_charge_flag_notify, OnGetZoneChargeFlagNotify, G2R_GET_ZONE_CHARGE_FLAG_NOTIFY);
    REGISTER_G2R_FUNCTION(g2r_active_present_code_respond, OnActivePresentCodeRespond, G2R_ACTIVE_PRESENT_CODE_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_freeze_coin_respond, OnFreezeCoinRespond, G2R_FREEZE_COIN_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_account_exchange_respond, OnAccountExchangeRespond, G2R_ACCOUNT_EXCHANGE_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_rename_request, OnRenameRequest, G2R_RENAME_REQUEST);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (nOpenPortFlag)
        {
            m_Acceptor.Close();
            nOpenPortFlag = false;
        }
    }
    return bResult;
}

void KGatewayAgency::UnInit()
{
    KG_COM_RELEASE(m_piSocket);
    m_Acceptor.Close();
}

void KGatewayAgency::Activate()
{
    ProcessAcceptor();

    ProcessPackage();

    CheckTimeout();
}

BOOL KGatewayAgency::ProcessAcceptor()
{
    BOOL                bResult                     = false;
    BOOL                bRetCode                    = false;
    IKG_SocketStream*   piSocket                    = NULL;
    timeval             IOTimeout                   = {5, 0};
    struct in_addr      RemoteAddr                  = { 0 };
    u_short             wRemotePortNet              = 0;
    u_short             wRemotePortHost             = 0;
    char*               pszRetString                = NULL;
    int                 nLowerProtocolVersion       = 0;
    int                 nUpperProtocolVersion       = 0;

    KG_PROCESS_ERROR(m_piSocket == NULL);

    piSocket = m_Acceptor.Accept();
    KG_PROCESS_ERROR(piSocket);

    piSocket->SetTimeout(&IOTimeout);

    piSocket->GetRemoteAddress(&RemoteAddr, &wRemotePortNet);

    wRemotePortHost = ntohs(wRemotePortNet);

    pszRetString = inet_ntoa(RemoteAddr);
    KGLOG_PROCESS_ERROR(pszRetString);

    m_piSocket = piSocket;
    m_piSocket->AddRef();
    
    m_nLastPingTime = g_pSO3GameCenter->m_nTimeNow;

    bRetCode = g_pSO3GameCenter->m_GameServer.GetGameWorldProtocolVersion(
        &nLowerProtocolVersion, &nUpperProtocolVersion
    );
    if (bRetCode)
    {        
        DoSetGameWorldVersion(nLowerProtocolVersion, nUpperProtocolVersion);
    }

    KGLogPrintf(KGLOG_INFO, "Gateway connected from %s:%u\n", pszRetString, wRemotePortHost);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSocket);
    return bResult;
}

BOOL KGatewayAgency::ProcessPackage()
{
    BOOL                    bResult             = false;
    int                     nRetCode            = 0;
    IKG_Buffer*             piRecvPackage       = NULL;
    size_t                  uBufferSize         = 0;

    while (m_piSocket)
    {
        timeval                 TimeoutValue        = {0, 0};
        PROTOCOL_FUNCTION       pfnProtocolFunction = NULL;
        KGR_PROTOCOL_HEADER*    pProtocolHeader     = NULL;
        int                     nProtocol           = 0;

        nRetCode = m_piSocket->CheckCanRecv(&TimeoutValue);
        if (nRetCode == 0)
            break;

        KGLOG_PROCESS_ERROR(nRetCode == 1);

        KG_COM_RELEASE(piRecvPackage);

        nRetCode = m_piSocket->Recv(&piRecvPackage);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        KGLOG_PROCESS_ERROR(piRecvPackage);

        m_nLastPingTime = g_pSO3GameCenter->m_nTimeNow;

        uBufferSize = (size_t)piRecvPackage->GetSize();
        KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KGR_PROTOCOL_HEADER));

        pProtocolHeader = (KGR_PROTOCOL_HEADER*)piRecvPackage->GetData();
        KGLOG_PROCESS_ERROR(pProtocolHeader);
        
        nProtocol = pProtocolHeader->byProtocol;
        KGLOG_PROCESS_ERROR(nProtocol > g2r_protocol_begin);
        KGLOG_PROCESS_ERROR(nProtocol < g2r_protocol_total);

        KGLOG_PROCESS_ERROR(uBufferSize >= m_uPakSize[nProtocol]);

        pfnProtocolFunction = m_ProtocolFunctions[nProtocol];
        KGLOG_PROCESS_ERROR(pfnProtocolFunction);

        (this->*pfnProtocolFunction)((BYTE*)pProtocolHeader, uBufferSize);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_piSocket)
        {
            KGLogPrintf(KGLOG_INFO, "[Gateway] Connection lost !\n");
        }

        KG_COM_RELEASE(m_piSocket);
    }
    KG_COM_RELEASE(piRecvPackage);
    return bResult;
}

BOOL KGatewayAgency::CheckTimeout()
{
    DWORD   dwTimeNow = 0;

    KG_PROCESS_ERROR(m_piSocket);

    if (g_pSO3GameCenter->m_nTimeNow > m_nLastPingTime + m_nPingCycle)
    {
        KGLogPrintf(KGLOG_INFO, "[Gateway] connection timeout !\n");
        
        KG_COM_RELEASE(m_piSocket);
    }

Exit0:
    return true;
}

BOOL KGatewayAgency::DoSetGameWorldVersion(int nLowerVersion, int nUpperVersion)
{
    BOOL                                bResult          = false;
    BOOL                                bRetCode         = false;
    R2G_SET_GAME_WORLD_VERSION*         pSetVersion      = NULL;
    IKG_Buffer*                         piSendBuffer     = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_SET_GAME_WORLD_VERSION));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSetVersion = (R2G_SET_GAME_WORLD_VERSION*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSetVersion);

    pSetVersion->byProtocol         = r2g_set_game_world_version;
    pSetVersion->nLowerVersion      = nLowerVersion;
    pSetVersion->nUpperVersion      = nUpperVersion;
    
    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoSyncRoleList(
    int nPlayerIndex, int nRoleIndex, int nRoleCount, DWORD dwRoleID, KRoleBaseInfo* pBaseInfo
)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    BOOL                        bRename             = false;
    IKG_Buffer*                 piSendBuffer        = NULL;
    size_t                      uSendBufferSize     = 0;
    R2G_SYNC_ROLE_LIST*         pRespond            = NULL;

    uSendBufferSize = sizeof(R2G_SYNC_ROLE_LIST);
    if (pBaseInfo != NULL)
    {
        uSendBufferSize += sizeof(KROLE_LIST_INFO);
    }

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uSendBufferSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_SYNC_ROLE_LIST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_sync_role_list;
    pRespond->nPlayerIndex  = nPlayerIndex;
    pRespond->nRoleIndex    = nRoleIndex;
    pRespond->nRoleCount    = nRoleCount;
    pRespond->dwRoleID      = ERROR_ID;

    if (pBaseInfo != NULL)
    {
        KRole*              pRole       = NULL;
        KROLE_LIST_INFO*    pListInfo   = (KROLE_LIST_INFO*)pRespond->byData;
        
        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
        KGLOG_PROCESS_ERROR(pRole);
        
        pRespond->dwRoleID      = dwRoleID;
        
        bRename = g_pSO3GameCenter->m_RoleManager.IsCanRename(dwRoleID);
        pListInfo->byCanRename = (BYTE)bRename;

        pListInfo->BaseInfo    = *pBaseInfo;
        pListInfo->dwPlayerID  = dwRoleID;
        
        strncpy(pListInfo->szRoleName, pRole->m_szName, sizeof(pListInfo->szRoleName));
        pListInfo->szRoleName[sizeof(pListInfo->szRoleName) - 1] = '\0';
        
        strncpy(pListInfo->szAccount, pRole->m_pszAccount, sizeof(pListInfo->szAccount));
        pListInfo->szAccount[sizeof(pListInfo->szAccount) - 1] = '\0';

        pListInfo->nFreezeTime = g_pSO3GameCenter->m_RoleManager.GetRoleFreezeTime(dwRoleID);
        pListInfo->nDeleteTime = g_pSO3GameCenter->m_RoleManager.GetRoleDeleteTime(dwRoleID);
    }

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoSyncNewbieMaps(int nPlayerIndex)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    const KHOMETOWN_MAP_TABLE*  pcHometownMaps  = NULL;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2G_SYNC_NEWBIE_MAPS*       pRespond        = NULL;

    pcHometownMaps = g_pSO3GameCenter->m_HometownList.GetHometownMapTable();
    KGLOG_PROCESS_ERROR(pcHometownMaps);

    for (KHOMETOWN_MAP_TABLE::const_iterator it = pcHometownMaps->begin(); it != pcHometownMaps->end(); ++it)
    {
        KNEWBIE_MAP_COPY_INFO*  pCopyInfo    = NULL;
        KMapInfo*               pMap         = NULL;
        int                     nCopyCount   = 0;

        pMap = g_pSO3GameCenter->m_MapManager.GetMapInfo(*it);
        KGLOG_PROCESS_ERROR(pMap);

        nCopyCount = (int)pMap->m_CopyTable.size();
        KGLOG_PROCESS_ERROR(nCopyCount <= UCHAR_MAX);

        KG_COM_RELEASE(piSendBuffer);

        piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2G_SYNC_NEWBIE_MAPS) + sizeof(KNEWBIE_MAP_COPY_INFO) * nCopyCount));
        KGLOG_PROCESS_ERROR(piSendBuffer);

        pRespond = (R2G_SYNC_NEWBIE_MAPS*)piSendBuffer->GetData();
        KGLOG_PROCESS_ERROR(pRespond);

        pRespond->byProtocol    = r2g_sync_newbie_maps;
        pRespond->nPlayerIndex  = nPlayerIndex;
        pRespond->dwMapID       = *it;
        pRespond->nCopyCount    = nCopyCount;

        pCopyInfo = (KNEWBIE_MAP_COPY_INFO*)pRespond->CopyInfo;

        for (KMAP_COPY_TABLE::iterator CopyIter = pMap->m_CopyTable.begin(); CopyIter != pMap->m_CopyTable.end(); ++CopyIter)
        {
            int nPlayerCount = CopyIter->second.m_nPlayerCount;
            int nMaxPlayer   = pMap->m_nMaxPlayerCount;

            KGLOG_PROCESS_ERROR(nMaxPlayer > 0);

            pCopyInfo->nCopyIndex  = CopyIter->first;
            pCopyInfo->nLoadFactor = nPlayerCount * 100 / nMaxPlayer;

            pCopyInfo++;
        }

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoCreateRoleRespond(
    BYTE byCode, unsigned long ulIdentity, KROLE_LIST_INFO* pRoleListInfo
)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    IKG_Buffer*                 piSendBuffer        = NULL;
    unsigned                    uSendBufferSize     = 0;
    R2G_CREATE_ROLE_RESPOND*    pRespond            = NULL;

    uSendBufferSize = sizeof(R2G_CREATE_ROLE_RESPOND);

    if (byCode == eCreateRoleSucceed)
    {
        uSendBufferSize += sizeof(KROLE_LIST_INFO); 
    }

    piSendBuffer = KG_MemoryCreateBuffer(uSendBufferSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_CREATE_ROLE_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_create_role_respond;
    pRespond->nPlayerIndex  = (int)ulIdentity;
    pRespond->byCode        = byCode;
    pRespond->dwRoleID      = pRoleListInfo ? pRoleListInfo->dwPlayerID : ERROR_ID;

    if (byCode == eCreateRoleSucceed)
    {
        memcpy(pRespond->byData, pRoleListInfo, sizeof(KROLE_LIST_INFO));
    }

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoDeleteRoleRespond(int nPlayerIndex, DWORD dwRoleID, int nCode, time_t nDeleteTime)
{
    BOOL                        bResult            = false;
    BOOL                        bRetCode           = false;
    IKG_Buffer*                 piSendBuffer       = NULL;
    R2G_DELETE_ROLE_RESPOND*    pDeleteRoleRespond = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_DELETE_ROLE_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDeleteRoleRespond = (R2G_DELETE_ROLE_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDeleteRoleRespond);

    pDeleteRoleRespond->byProtocol      = r2g_delete_role_respond;
    pDeleteRoleRespond->nPlayerIndex    = nPlayerIndex;
    pDeleteRoleRespond->byCode          = (BYTE)nCode;
    pDeleteRoleRespond->dwRoleID        = dwRoleID;
    pDeleteRoleRespond->nDeleteTime     = nDeleteTime;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoGameLoginRespond(
    int nPlayerIndex, int nRespondCode, DWORD dwRoleID, DWORD dwQueueID, int nLoginIP, int nLoginPort, GUID Guid
)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    R2G_GAME_LOGIN_RESPOND* pRespond        = NULL;
    IKG_Buffer*             piSendBuffer    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2G_GAME_LOGIN_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_GAME_LOGIN_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_game_login_respond;
    pRespond->nPlayerIndex  = nPlayerIndex;
    pRespond->byCode        = (BYTE)nRespondCode;
    pRespond->dwRoleID      = dwRoleID;
    pRespond->dwQueueID     = dwQueueID;
    pRespond->dwIP          = nLoginIP;
    pRespond->nPort         = nLoginPort;
    pRespond->guid          = Guid;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoPlayerLeaveGame(const char cszAccountName[])
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    R2G_PLAYER_LEAVE_GAME*  pNotify         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_PLAYER_LEAVE_GAME));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pNotify = (R2G_PLAYER_LEAVE_GAME*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->byProtocol = r2g_player_leave_game;

    strncpy(pNotify->szAccount, cszAccountName, sizeof(pNotify->szAccount));
    pNotify->szAccount[sizeof(pNotify->szAccount) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoChangeExtPointRequest(const char* pszAccountName, DWORD dwRoleID, WORD wExtPointIndex, WORD wChangeValue)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_CHANGE_EXT_POINT_REQUEST*   pRequest = NULL;

    assert(pszAccountName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_CHANGE_EXT_POINT_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_CHANGE_EXT_POINT_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = r2g_change_ext_point_request;

    strncpy(pRequest->szAccount, pszAccountName, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    pRequest->dwRoleID          = dwRoleID;
    pRequest->wExtPointIndex    = wExtPointIndex;
    pRequest->wChangeValue      = wChangeValue;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoSetChargeFlagRequest(const char* pszAccountName, DWORD dwRoleID, DWORD dwLastLoginIP, int nChargeFlag)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_SET_CHARGE_FLAG_REQUEST*    pRequest = NULL;

    assert(pszAccountName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_SET_CHARGE_FLAG_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_SET_CHARGE_FLAG_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = r2g_set_charge_flag_request;

    strncpy(pRequest->szAccount, pszAccountName, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    pRequest->dwRoleID      = dwRoleID;
    pRequest->dwLastLoginIP = dwLastLoginIP;
    pRequest->nChargeFlag   = nChargeFlag;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoActivePresentCodeRequest(const char* pszAccountName, DWORD dwRoleID, const char* pszPresentCode, DWORD dwActiveIP)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    R2G_ACTIVE_PRESENT_CODE_REQUEST*    pRequest = NULL;

    assert(pszAccountName);
    assert(pszPresentCode);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_ACTIVE_PRESENT_CODE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_ACTIVE_PRESENT_CODE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = r2g_active_present_code_request;

    strncpy(pRequest->szAccount, pszAccountName, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    strncpy(pRequest->szPresentCode, pszPresentCode, sizeof(pRequest->szPresentCode));
    pRequest->szPresentCode[sizeof(pRequest->szPresentCode) - 1] = '\0';

    pRequest->dwRoleID      = dwRoleID;
    pRequest->dwActiveIP    = dwActiveIP;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoFreezeCoinRequest(DWORD dwRequestID, const char* pszAccountName, BOOL bFreezeFlag, int nCoin)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    R2G_FREEZE_COIN_REQUEST*    pRequest = NULL;

    assert(pszAccountName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_FREEZE_COIN_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_FREEZE_COIN_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol    = r2g_freeze_coin_request;
    pRequest->dwRequestID   = dwRequestID;

    strncpy(pRequest->szAccount, pszAccountName, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    pRequest->bFreezeFlag   = bFreezeFlag;
    pRequest->nCoin         = nCoin;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoAccountExchangeRequest(
    DWORD dwRequestID, const char* pszSrcAccount, const char* pszDstAccount, DWORD dwExchangeType,
    DWORD dwDstAmount, DWORD dwSrcAmount
)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_ACCOUNT_EXCHANGE_REQUEST*   pRequest = NULL;

    assert(pszSrcAccount);
    assert(pszDstAccount);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_ACCOUNT_EXCHANGE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_ACCOUNT_EXCHANGE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol    = r2g_account_exchange_request;
    pRequest->dwRequestID   = dwRequestID;

    strncpy(pRequest->szSrcAccount, pszSrcAccount, sizeof(pRequest->szSrcAccount));
    pRequest->szSrcAccount[sizeof(pRequest->szSrcAccount) - 1] = '\0';

    strncpy(pRequest->szDstAccount, pszDstAccount, sizeof(pRequest->szDstAccount));
    pRequest->szDstAccount[sizeof(pRequest->szDstAccount) - 1] = '\0';

    pRequest->dwExchangeType    = dwExchangeType;
    pRequest->dwDstAmount       = dwDstAmount;
    pRequest->dwSrcAmount       = dwSrcAmount;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGatewayAgency::DoRenameRespond(BYTE byCode, int nPlayerIndex, DWORD dwRoleID, KROLE_LIST_INFO* pRoleListInfo)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    IKG_Buffer*         piBuffer        = NULL;
    R2G_RENAME_RESPOND* pRenameRespond  = NULL;
    size_t              uSendSize       = 0;

    uSendSize = sizeof(R2G_RENAME_RESPOND);

    if (pRoleListInfo)
    {
        uSendSize += sizeof(KROLE_LIST_INFO);
    }

    piBuffer = KG_MemoryCreateBuffer((unsigned)uSendSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pRenameRespond = (R2G_RENAME_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRenameRespond);

    pRenameRespond->byProtocol      = r2g_rename_respond;
    pRenameRespond->nPlayerIndex    = nPlayerIndex;
    pRenameRespond->byCode          = byCode;
    pRenameRespond->dwRoleID        = dwRoleID;

    if (pRoleListInfo)
    {
        pRoleListInfo->nDeleteTime  = g_pSO3GameCenter->m_RoleManager.GetRoleDeleteTime(dwRoleID);
        pRoleListInfo->nFreezeTime  = g_pSO3GameCenter->m_RoleManager.GetRoleFreezeTime(dwRoleID);
        pRoleListInfo->byCanRename  = (BYTE)g_pSO3GameCenter->m_RoleManager.IsCanRename(dwRoleID);

        memcpy(pRenameRespond->byData, pRoleListInfo, sizeof(KROLE_LIST_INFO));
    }

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGatewayAgency::Send(IKG_Buffer* piBuffer)
{
    BOOL bResult  = false;
    int  nRetCode = false;

    assert(piBuffer);

    KG_PROCESS_ERROR(m_piSocket);

    nRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnPingSignal(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    G2R_PING_SIGNAL*    pPing           = (G2R_PING_SIGNAL*)pbyData;
    IKG_Buffer*         piSendBuffer    = NULL;
    R2G_PING_SIGNAL*    pPingRespond    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_PING_SIGNAL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPingRespond = (R2G_PING_SIGNAL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPingRespond);

    pPingRespond->byProtocol    = r2g_ping_signal;
    pPingRespond->dwTime        = pPing->dwTime;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::OnQueryAccountState(BYTE* pbyData, size_t uDataLen)
{
    BOOL                         bResult             = false;
    BOOL                         bRetCode            = false;
    IKG_Buffer*                  piSendBuffer        = NULL;
    KRole*                       pOnlineRole         = NULL;
    G2R_QUERY_ACCOUNT_STATE*     pQueryAccountState  = (G2R_QUERY_ACCOUNT_STATE*)pbyData;
    R2G_SYNC_ACCOUNT_STATE*      pSyncAccountState   = NULL;

    pQueryAccountState->szAccount[sizeof(pQueryAccountState->szAccount) - 1] = '\0';

    pOnlineRole = g_pSO3GameCenter->m_RoleManager.GetOnlineRoleForAccount(pQueryAccountState->szAccount);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2G_SYNC_ACCOUNT_STATE));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSyncAccountState = (R2G_SYNC_ACCOUNT_STATE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSyncAccountState);

    pSyncAccountState->byProtocol   = r2g_sync_account_state;
    pSyncAccountState->nPlayerIndex = pQueryAccountState->nPlayerIndex;
    pSyncAccountState->bOnline      = (pOnlineRole != NULL);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayAgency::OnQueryRoleList(BYTE* pbyData, size_t uDataLen)
{
    BOOL                   bResult         = false;
    G2R_QUERY_ROLE_LIST*   pQuery          = (G2R_QUERY_ROLE_LIST*)pbyData;
    KAccount*              pAccount        = NULL;
    int                    nRoleCount      = 0;

    pQuery->szAccount[sizeof(pQuery->szAccount) - 1] = '\0';

    pAccount = g_pSO3GameCenter->m_RoleManager.GetAccount(pQuery->szAccount);
    if (pAccount == NULL)
    {
        DoSyncRoleList(pQuery->nPlayerIndex, 0, 0, 0, NULL);
        goto Exit0;
    }

    nRoleCount = (int)pAccount->vRoleList.size();
    if (nRoleCount == 0)
    {
        DoSyncRoleList(pQuery->nPlayerIndex, 0, 0, 0, NULL);
        goto Exit0;
    }

    for (int i = 0; i < nRoleCount; i++)
    {
        KRole* pRole = pAccount->vRoleList[i];

        assert(pRole);

        g_pSO3GameCenter->m_RoleDB.DoQueryRoleBaseData(
            pRole->m_dwPlayerID, pQuery->nPlayerIndex, i + 1, nRoleCount
        );
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnQueryNewbieMaps(BYTE* pbyData, size_t uDataLen)
{
    G2R_QUERY_NEWBIE_MAPS*  pRequest        = (G2R_QUERY_NEWBIE_MAPS*)pbyData;

    DoSyncNewbieMaps(pRequest->nPlayerIndex);

    return true;
}

BOOL KGatewayAgency::OnKickAccountNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult         = false;
    G2R_KICK_ACCOUNT_NOTIFY*    pNotify         = (G2R_KICK_ACCOUNT_NOTIFY*)pbyData;
    KRole*                      pOnlineRole     = NULL;

    pNotify->szAccount[sizeof(pNotify->szAccount) - 1] = '\0';

    pOnlineRole = g_pSO3GameCenter->m_RoleManager.GetOnlineRoleForAccount(pNotify->szAccount);
    KGLOG_PROCESS_ERROR(pOnlineRole);

    g_pSO3GameCenter->m_GameServer.DoKickAccountNotify(pOnlineRole->m_dwPlayerID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnCreateRoleRequest(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult             = false;
    int                                 nRetCode            = 0;
    G2R_CREATE_ROLE_REQUEST*            pRequest            = (G2R_CREATE_ROLE_REQUEST*)pbyData;
    KUSER_ROLE_GENERATOR_PARAM*         pCreateRoleParam    = NULL;
    KMapInfo*                           pMap                = NULL;
    int                                 nMapCopyIndex       = 0;
    const KMapCopy*                     pcMapCopy           = NULL;
    KRole*                              pRole               = NULL;
    KDB_ROLE_GENERATOR_PARAM            RoleCreateParam;
    KBornPointInfo                      BornPointInfo;
    int                                 nRoleNameLen;

    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(G2R_CREATE_ROLE_REQUEST) + sizeof(KUSER_ROLE_GENERATOR_PARAM));

    pCreateRoleParam = (KUSER_ROLE_GENERATOR_PARAM*)pRequest->byData;

    pCreateRoleParam->szRoleName[sizeof(pCreateRoleParam->szRoleName) - 1] = '\0';

    nRoleNameLen = (int)strlen(pCreateRoleParam->szRoleName);
    if (nRoleNameLen > g_pSO3GameCenter->m_Settings.m_nMaxRoleNameLength)
    {
        g_pSO3GameCenter->m_Gateway.DoCreateRoleRespond(
            eCreateRoleNameTooLong, pRequest->nPlayerIndex, NULL
        );
        goto Exit1;
    }

    if (nRoleNameLen < g_pSO3GameCenter->m_Settings.m_nMinRoleNameLength)
    {
        g_pSO3GameCenter->m_Gateway.DoCreateRoleRespond(
            eCreateRoleNameTooShort, pRequest->nPlayerIndex, NULL
        );
        goto Exit1;
    }

    nRetCode = g_pSO3GameCenter->m_piTextFilter->Check(pCreateRoleParam->szRoleName);
    if (!nRetCode)
    {
        g_pSO3GameCenter->m_Gateway.DoCreateRoleRespond(
            eCreateRoleInvalidRoleName, pRequest->nPlayerIndex, NULL
        );
        goto Exit1;
    }
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pCreateRoleParam->szRoleName);
    if (pRole != NULL)
    {
        g_pSO3GameCenter->m_Gateway.DoCreateRoleRespond(
            eCreateRoleNameAlreadyExist, pRequest->nPlayerIndex, NULL
        );
        goto Exit1;
    }

    // 检查Role type
    KGLOG_PROCESS_ERROR(pCreateRoleParam->cRoleType > rtInvalid && pCreateRoleParam->cRoleType < rtTotal);

    // 检查MapID是不是新手村
    pMap = g_pSO3GameCenter->m_MapManager.GetMapInfo(pCreateRoleParam->dwMapID);
    KGLOG_PROCESS_ERROR(pMap && pMap->m_nType == emtBirthMap);

    nMapCopyIndex = pCreateRoleParam->nMapCopyIndex;

    // 副本ID为0表示服务器自动选择副本
    if (nMapCopyIndex == 0)
    {
        typedef std::vector<int, KMemory::KAllocator<int> > KMapCopyTable;
        int             nPlayerCount = -1;
        KMapCopyTable   vMapCopys;

        for (int nIndex = 1; nIndex < pMap->m_nSNGenerator; nIndex++)
        {
            pcMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pCreateRoleParam->dwMapID, nIndex);

            if (pcMapCopy == NULL || pcMapCopy->m_eState != eMapStateNormal)
            {
                continue;
            }

            vMapCopys.push_back(nIndex);

            // 选一个不需要预排队的 人数最大的 新手村
            if (pcMapCopy->m_nPlayerCount > nPlayerCount && pcMapCopy->m_nPlayerCount < pMap->m_nPreQueuePlayerCount)
            {
                nMapCopyIndex     = nIndex;
                nPlayerCount      = pcMapCopy->m_nPlayerCount;
            }
        }

        // 如果所有新手村都需要排队, 则随机挑选一个
        if (nPlayerCount == -1 && vMapCopys.size() > 0)
        {
            int nIndex = (int)(KG_Rand() % vMapCopys.size());

            nMapCopyIndex = vMapCopys[nIndex];
        }
    }

    pcMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pCreateRoleParam->dwMapID, nMapCopyIndex);
    if (pcMapCopy == NULL || pcMapCopy->m_eState != eMapStateNormal)
    {
        // 找不到合适的新手村副本,不能创建角色
        DoCreateRoleRespond(eCreateRoleUnableToCreate, pRequest->nPlayerIndex, NULL);
        goto Exit1;
    }

    // 获取对应的出身点坐标
    nRetCode = g_pSO3GameCenter->m_HometownList.GetBornPointInfo(
        &BornPointInfo, pCreateRoleParam->dwMapID, pCreateRoleParam->cRoleType
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    RoleCreateParam.nX             = BornPointInfo.nPosX;
    RoleCreateParam.nY             = BornPointInfo.nPosY;
    RoleCreateParam.nZ             = BornPointInfo.nPosZ;
    RoleCreateParam.byDirection    = (BYTE)BornPointInfo.nDirection;
    RoleCreateParam.UserParam      = *pCreateRoleParam;

    RoleCreateParam.UserParam.nMapCopyIndex  = nMapCopyIndex;

    strncpy(RoleCreateParam.szAccountName, pRequest->szAccount, sizeof(RoleCreateParam.szAccountName));
    RoleCreateParam.szAccountName[sizeof(RoleCreateParam.szAccountName) - 1] = '\0';

    g_pSO3GameCenter->m_RoleDB.DoCreateRole(&RoleCreateParam, pRequest->nPlayerIndex);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnDeleteRoleRequest(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    int                         nRetCode            = 0;
    KRole*                      pRole               = NULL;
    KTong*                      pTong               = NULL; 
    G2R_DELETE_ROLE_REQUEST*    pDeleteRoleRequest  = (G2R_DELETE_ROLE_REQUEST*)pbyData;
    time_t                      nEndTime            = 0;
    time_t                      nFreezeTime         = 0;
    int                         nCode               = eDeleteRoleUnknownError;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pDeleteRoleRequest->dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    pTong = g_pSO3GameCenter->m_TongManager.GetTong(pRole->m_dwTongID);
    if (pTong && pTong->m_dwMaster == pRole->m_dwPlayerID)
    {
        nCode = eDeleteRoleTongMaster;
        goto Exit0;
    }

    nFreezeTime = g_pSO3GameCenter->m_RoleManager.GetRoleFreezeTime(pRole->m_dwPlayerID);
    if (nFreezeTime > g_pSO3GameCenter->m_nTimeNow)
    {
        nCode = eDeleteRoleFreezeRole;
        goto Exit0;
    }

    KGLogPrintf(KGLOG_DEBUG, "Apply delete role %s.", pRole->m_szName);

    if (pRole->m_byLevel < g_pSO3GameCenter->m_Settings.m_nDeleteProtectLevel)
    {
        bRetCode = g_pSO3GameCenter->m_RoleManager.DelRole(pDeleteRoleRequest->dwRoleID);
        KG_PROCESS_ERROR(bRetCode);

        nCode = eDeleteRoleSucceed;
    }
    else
    {
        nEndTime = g_pSO3GameCenter->m_nTimeNow + g_pSO3GameCenter->m_Settings.m_nDeleteDelayTime;

        g_pSO3GameCenter->m_RoleManager.RegisterDelRole(pDeleteRoleRequest->dwRoleID, nEndTime);

        g_pSO3GameCenter->m_MiscDB.DoUpdateRoleDeleteList(pDeleteRoleRequest->dwRoleID, nEndTime);

        nCode = eDeleteRoleDelay;
    }

    bResult = true;
Exit0:
    DoDeleteRoleRespond(pDeleteRoleRequest->nPlayerIndex, pDeleteRoleRequest->dwRoleID, nCode, nEndTime);
    return bResult;
}

BOOL KGatewayAgency::OnGameLoginRequest(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    G2R_GAME_LOGIN_REQUEST*     pRequest        = (G2R_GAME_LOGIN_REQUEST*)pbyData;
    time_t                      nFreezeTime     = 0;
    int                         nNameLen        = 0;
    KRole*                      pRole           = NULL;
    KROLE_LIST_INFO*            pRoleListInfo   = NULL;
    KACCOUNT_LOGIN_INFO         AccInfo;

    if (m_bQuiting)
    {
        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pRequest->nPlayerIndex, eGameLoginSystemMaintenance, pRequest->dwRoleID, INVALID_QUEUE_ID, 0, 0, GUID()
        );
        goto Exit0;
    }

    nFreezeTime = g_pSO3GameCenter->m_RoleManager.GetRoleFreezeTime(pRequest->dwRoleID);
    if (nFreezeTime > g_pSO3GameCenter->m_nTimeNow)
    {
        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pRequest->nPlayerIndex, eGameLoginRoleFreeze, pRequest->dwRoleID, INVALID_QUEUE_ID, 0, 0, GUID()
        );
        goto Exit0;
    }

    g_pSO3GameCenter->m_RoleManager.UnregisterDelRole(pRequest->dwRoleID);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    KGLOG_PROCESS_ERROR(pRole->GetRoleState() == rsOffline);

    AccInfo.nChargeFlag             = pRequest->nChargeFlag;
    AccInfo.ExtPointInfo            = pRequest->ExtPointInfo;
    AccInfo.dwEndTimeOfFee          = pRequest->dwEndTimeOfFee;
    AccInfo.dwCoin                  = pRequest->dwCoin;
    AccInfo.dwLimitPlayTimeFlag     = pRequest->dwLimitPlayTimeFlag;
    AccInfo.dwLimitOnlineSecond     = pRequest->dwLimitOnlineSecond;
    AccInfo.dwLimitOfflineSecond    = pRequest->dwLimitOfflineSecond;
    AccInfo.nIdentity               = pRequest->nPlayerIndex;
    
    pRoleListInfo = (KROLE_LIST_INFO*)pRequest->byData;
    
    assert(pRoleListInfo);

    g_pSO3GameCenter->m_GameServer.ProcessRoleLogin(
        &AccInfo, pRoleListInfo->dwPlayerID, &pRoleListInfo->BaseInfo, pRequest->dwQueueID
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    G2R_CHANGE_EXT_POINT_RESPOND*   pRespond    = (G2R_CHANGE_EXT_POINT_RESPOND*)pbyData;
    KRole*                          pRole       = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    KGLogPrintf(
        KGLOG_INFO, "Change ExtPoint %s: Acc = %s, Role = %s, ID = %u",
        pRespond->bSucceed ? "succeed" : "failed", 
        pRole->m_pszAccount, pRole->m_szName, pRole->m_dwPlayerID
    );

    g_pSO3GameCenter->m_GameServer.DoChangeExtPointRespond(
        pRole->m_nConnIndex, pRole->m_dwPlayerID, pRespond->bSucceed
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    G2R_SET_CHARGE_FLAG_RESPOND*    pRespond    = (G2R_SET_CHARGE_FLAG_RESPOND*)pbyData;
    KRole*                          pRole       = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    if (!pRespond->bSucceed)
    {
        KGLogPrintf(
            KGLOG_ERR, "Set ChargeFlag failed! Acc = %s, Role = %s",
            pRole->m_pszAccount, pRole->m_szName
        );
        goto Exit0;
    }

    g_pSO3GameCenter->m_GameServer.DoSetChargeFlagRespond(
        pRole->m_nConnIndex, pRole->m_dwPlayerID, pRespond->nChargeFlag, pRespond->ExtPointInfo,
        pRespond->dwEndTimeOfFee
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnGetZoneChargeFlagNotify(BYTE* pbyData, size_t uDataLen)
{
    G2R_GET_ZONE_CHARGE_FLAG_NOTIFY*    pNotify = (G2R_GET_ZONE_CHARGE_FLAG_NOTIFY*)pbyData;

    g_pSO3GameCenter->m_nZoneChargeFlag = pNotify->nZoneChargeFlag;

    g_pSO3GameCenter->m_GameServer.DoSyncZoneChargeFlag(-1, pNotify->nZoneChargeFlag);

    if (pNotify->nZoneChargeFlag && g_pSO3GameCenter->m_GlobalSystemValueManager.m_GameWorldChargeTime.nValue == 0)
    {
        g_pSO3GameCenter->m_GlobalSystemValueManager.m_GameWorldChargeTime.nValue = (int)time(NULL);
        g_pSO3GameCenter->m_GlobalSystemValueManager.m_GameWorldChargeTime.bUpdate = true;
        g_pSO3GameCenter->m_GlobalSystemValueManager.m_bNeedSyncToServer = true;
    }

    return true;
}

BOOL KGatewayAgency::OnActivePresentCodeRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult     = false;
    G2R_ACTIVE_PRESENT_CODE_RESPOND*    pRespond    = (G2R_ACTIVE_PRESENT_CODE_RESPOND*)pbyData;
    KRole*                              pRole       = NULL;

    pRespond->szPresentCode[sizeof(pRespond->szPresentCode) - 1] = '\0';

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    KGLogPrintf(
        KGLOG_INFO, "Active PresentCode %s: Acc = %s, Role = %s, ID = %u, PresentCode = %s",
        pRespond->bSucceed ? "succeed" : "failed", 
        pRole->m_pszAccount, pRole->m_szName, pRole->m_dwPlayerID, pRespond->szPresentCode
    );

    g_pSO3GameCenter->m_GameServer.DoActivePresentCodeRespond(
        pRole->m_nConnIndex, pRole->m_dwPlayerID, pRespond->bSucceed,
        pRespond->szPresentCode, pRespond->dwPresentType
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnFreezeCoinRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    G2R_FREEZE_COIN_RESPOND*    pRespond    = (G2R_FREEZE_COIN_RESPOND*)pbyData;

    if (pRespond->bFreezeFlag)
    {
        g_pSO3GameCenter->m_GameCardManager.OnPaysysFreezeCoinRespond(pRespond->bSucceed, pRespond->dwRequestID);
    }
    else
    {
        KRole* pRole = NULL;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwRequestID);
        KGLOG_PROCESS_ERROR(pRole);

        g_pSO3GameCenter->m_LogServerAgency.LogUnfreezeCoin(pRole, pRespond->nCoin, pRespond->bSucceed ? "succeed" : "failed");

        g_pSO3GameCenter->m_GameServer.DoGameCardCancelRespond(pRespond->dwRequestID, (BYTE)(pRespond->bSucceed ? gorSucceed : gorUnFreezeCoinFailed));
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayAgency::OnAccountExchangeRespond(BYTE* pbyData, size_t uDataLen)
{
    G2R_ACCOUNT_EXCHANGE_RESPOND* pRespond = (G2R_ACCOUNT_EXCHANGE_RESPOND*)pbyData;

    g_pSO3GameCenter->m_GameCardManager.OnPaysysAccountExchangeRespond(pRespond->bSucceed, pRespond->dwRequestID);

    return true;
}

BOOL KGatewayAgency::OnRenameRequest(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    G2R_RENAME_REQUEST*     pRenameRequest  = (G2R_RENAME_REQUEST*)pbyData;
    KRole*                  pRole           = NULL;
    size_t                  uParamDataLen   = 0;
    int                     nNewNameLen     = 0;

    bRetCode = g_pSO3GameCenter->m_RoleManager.IsCanRename(pRenameRequest->dwRoleID);
    KG_PROCESS_ERROR(bRetCode);

    pRenameRequest->szNewName[sizeof(pRenameRequest->szNewName) - 1] = '\0';

    nNewNameLen = (int)strlen(pRenameRequest->szNewName);
    if (nNewNameLen > g_pSO3GameCenter->m_Settings.m_nMaxRoleNameLength)
    {
        DoRenameRespond(
            eRenameNewNameTooLong, pRenameRequest->nPlayerIndex,
            pRenameRequest->dwRoleID, NULL
        );
        goto Exit0;
    }

    if (nNewNameLen < g_pSO3GameCenter->m_Settings.m_nMinRoleNameLength)
    {
        DoRenameRespond(
            eRenameNewNameTooShort, pRenameRequest->nPlayerIndex,
            pRenameRequest->dwRoleID, NULL
        );
        goto Exit0;
    }

    bRetCode = g_pSO3GameCenter->m_piTextFilter->Check(pRenameRequest->szNewName);
    if (!bRetCode)
    {
        DoRenameRespond(
            eRenameNewNameError, pRenameRequest->nPlayerIndex,
            pRenameRequest->dwRoleID, NULL
        );
        goto Exit0;
    }

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRenameRequest->szNewName);
    if (pRole)
    {
        DoRenameRespond(
            eRenameNameAlreadyExist, pRenameRequest->nPlayerIndex, 
            pRenameRequest->dwRoleID, NULL
        );
        goto Exit0;
    }

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRenameRequest->dwRoleID);
    if (!pRole)
    {
        DoRenameRespond(
            eRenameUnknownError, pRenameRequest->nPlayerIndex, 
            pRenameRequest->dwRoleID, NULL
        );
        goto Exit0;
    }

    KGLOG_PROCESS_ERROR(pRole->GetRoleState() == rsOffline);

    g_pSO3GameCenter->m_RoleDB.DoRename(
        pRenameRequest->dwRoleID, pRenameRequest->nPlayerIndex, pRenameRequest->szNewName
    );

    bResult = true;
Exit0:
    return bResult;
}
