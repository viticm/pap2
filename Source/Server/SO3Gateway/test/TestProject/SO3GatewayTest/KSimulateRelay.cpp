///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSimulateRelay.cpp
// Creator  :   liuzhibiao
// Date     :   2008-9-22
// Comment  :   v0.96
//              V0.8 2008-08-26  
//              V0.95 2008-9-22 改变数据结构
//              v0.96 2008-10-7     修正bug
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KSimulateRelay.h"

#define REGISTER_G2R_FUNCTION(id, func, data)               \
	do                                                      \
	{                                                       \
	m_ProtocolFunctions[id] = &KSimulateRelay::func;    \
	m_uPakSize[id]          = sizeof(data);             \
	} while(false)

int KSimulateRelay::Init(const KG_SIMULATE_GAMECENTER_CONFIG& crSimulateRelayConfig)
{
	int                     nResult         = false;
	int                     nRetCode        = false;
	timeval                 TimeoutValue    = {0, 0};
	int                     nOpenPortFlag   = false;
    int                     nWorkThreadFlag = false;

    m_nQueueID                   = 1;
    m_nLoginInGameWorldNum       = 0;
    m_nHadRevKickInfo            = false;
    m_nExitFlag                  = false; 
    m_piSocket                   = NULL;
    nWorkThreadFlag              = false;
    m_nUserEnterGameRequestCount = 0;
    m_nDoNotRespondProtocol      = crSimulateRelayConfig.nNotRespondProtocol;

    m_nHadConnections                   = false;
    m_nRevChangeExtPointRespond         = false;
    m_nRevActivePresentCodeRespond      = false;
    m_nRevPlayerSetChargeFlagRespond    = false;
    m_nRevGetZoneChargeFlagNotify       = false;

    m_nRevFreezeCoinRespond         = false;
    m_nRevAccountExchangeRespond    = false;

    memcpy(&m_RelayConfig, &crSimulateRelayConfig, sizeof(m_RelayConfig));
    m_RelayConfig.nAllowPlayerInGameWorld = MAX_PLAYER_IN_GAMEWORLD;

	nRetCode = m_Acceptor.Open(m_RelayConfig.szRelayIPAddr, m_RelayConfig.nRelayPort);
	KGLOG_PROCESS_ERROR(nRetCode);
	nOpenPortFlag = true;

	m_Acceptor.SetTimeout(&TimeoutValue);   

	memset(m_ProtocolFunctions, 0, sizeof(m_ProtocolFunctions));
	memset(m_uPakSize, 0, sizeof(m_uPakSize));

	REGISTER_G2R_FUNCTION(g2r_query_account_state, OnQueryAccountState, G2R_QUERY_ACCOUNT_STATE);
	REGISTER_G2R_FUNCTION(g2r_query_role_list, OnQueryRoleList, G2R_QUERY_ROLE_LIST);
	REGISTER_G2R_FUNCTION(g2r_query_newbie_maps, OnQueryNewbieMaps, G2R_QUERY_NEWBIE_MAPS);
	REGISTER_G2R_FUNCTION(g2r_create_role_request, OnCreateRoleRequest, G2R_CREATE_ROLE_REQUEST);
	REGISTER_G2R_FUNCTION(g2r_delete_role_request, OnDeleteRoleRequest, G2R_DELETE_ROLE_REQUEST);
	REGISTER_G2R_FUNCTION(g2r_game_login_request, OnGameLoginRequest, G2R_GAME_LOGIN_REQUEST);
//	REGISTER_G2R_FUNCTION(g2r_ext_points_opt_respond, OnExtPointsOptRespond, G2R_EXT_POINTS_OPT_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_change_ext_point_respond, OnChangeExtPointRespond, G2R_CHANGE_EXT_POINT_RESPOND);//new
    REGISTER_G2R_FUNCTION(g2r_set_charge_flag_respond, OnSetChargeFlagRespond, G2R_SET_CHARGE_FLAG_RESPOND);//new
    REGISTER_G2R_FUNCTION(g2r_get_zone_charge_flag_notify, OnGetZoneChargeFlagNotify, G2R_GET_ZONE_CHARGE_FLAG_NOTIFY);//new
    REGISTER_G2R_FUNCTION(g2r_kick_account_notify, OnKickAccountRequest, G2R_KICK_ACCOUNT_NOTIFY);
    REGISTER_G2R_FUNCTION(g2r_ping_signal, OnPingRequest, G2R_PING_SIGNAL);
    REGISTER_G2R_FUNCTION(g2r_active_present_code_respond, OnActivePresentCodeRespond, G2R_ACTIVE_PRESENT_CODE_RESPOND);

    REGISTER_G2R_FUNCTION(g2r_freeze_coin_respond, OnFreezeCoinResond, G2R_FREEZE_COIN_RESPOND);
    REGISTER_G2R_FUNCTION(g2r_account_exchange_respond, OnAccountExchangeResond, G2R_ACCOUNT_EXCHANGE_RESPOND);

    nRetCode = m_WorkThread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    nWorkThreadFlag = true;

    nResult = true;
Exit0:
	if (!nResult)
	{
        m_nExitFlag = true;

        if (nWorkThreadFlag)
        {
            m_WorkThread.Destroy();
            nWorkThreadFlag = false;
        }
		if (nOpenPortFlag)
		{
			m_Acceptor.Close();
			nOpenPortFlag = false;
		}
	}
	return nResult;
}

void KSimulateRelay::UnInit()
{
    std::list<KInGameWorldPlayer*>::iterator iter = m_InGameWorldPlayerList.begin();
    std::list<KInGameWorldPlayer*>::iterator iterEnd = m_InGameWorldPlayerList.end();

    while (iter != iterEnd)
    {
        KG_DELETE(*iter);
        ++iter;
    }
    m_nHadConnections = false;

    m_nExitFlag = true;
    m_WorkThread.Destroy(); 

	KG_COM_RELEASE(m_piSocket);
	m_Acceptor.Close();

}

void KSimulateRelay::Activate()
{
	ProcessAcceptor();

	ProcessPackage();
}

int KSimulateRelay::ProcessAcceptor()
{
	int                 nResult                     = false;
//	int                 nRetCode                    = false;
	IKG_SocketStream*   piSocket                    = NULL;
	timeval             IOTimeout                   = {5, 0};
	struct in_addr      RemoteAddr                  = { 0 };
	u_short             wRemotePortNet              = 0;
	u_short             wRemotePortHost             = 0;
	char*               pszRetString                = NULL;

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

    KGLogPrintf(KGLOG_INFO, "Relay:: Gateway connected from %s:%u\n", pszRetString, wRemotePortHost);
    DoSetProtocolVersion(m_RelayConfig.nGameWorldProtocolLowerVersion, m_RelayConfig.nGameWorldProtocolUpperVersion);
    
    m_nHadConnections = true;

	nResult = true;
Exit0:
	KG_COM_RELEASE(piSocket);
	return nResult;
}

int KSimulateRelay::DoCreateRoleRespond(int nSucceed, int nPlayerIndex)
{
    int                         nResult             = false;
    int                         nRetCode            = false;
    IKG_Buffer*                 piSendBuffer        = false;
    size_t                      uSendBufferSize     = 0;
    R2G_CREATE_ROLE_RESPOND*    pRespond            = NULL;

    KGLOG_PROCESS_ERROR(m_piSocket);

    uSendBufferSize = sizeof(R2G_CREATE_ROLE_RESPOND);

    if (nSucceed == eCreateRoleSucceed)
    {
        uSendBufferSize += sizeof(KROLE_LIST_INFO); 
    }

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uSendBufferSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_CREATE_ROLE_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_create_role_respond;
    pRespond->nPlayerIndex  = nPlayerIndex;
    //pRespond->byCode        = (BYTE)(nSucceed ? eCreateRoleSucceed : eCreateRoleInvalidRoleName);
    pRespond->byCode        = (BYTE)nSucceed;
    pRespond->dwRoleID      = m_RelayConfig.nGetRoleListCount;
    if (nSucceed == eCreateRoleSucceed)
    {
        memset(pRespond->byData, 0, sizeof(KROLE_LIST_INFO));
    }

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::DoSetProtocolVersion(int nLowerVersion, int nUpperVersion)
{
    int                             nResult             = false;
    int                             nRetCode            = false;
    IKG_Buffer*                     piSendBuffer        = false;
    size_t                          uSendBufferSize     = 0;
    R2G_SET_GAME_WORLD_VERSION*     pRespond            = NULL;

    KG_PROCESS_ERROR(m_piSocket);

    uSendBufferSize = sizeof(R2G_SET_GAME_WORLD_VERSION);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uSendBufferSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_SET_GAME_WORLD_VERSION*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_set_game_world_version;
    pRespond->nLowerVersion = nLowerVersion;
    pRespond->nUpperVersion = nUpperVersion;

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::DoReplyLoginData(DWORD dwPacketIdentity, DWORD dwRoleID, int nRespondCode)
{
    int                         nResult             = false;
    int                         nRetCode            = false;
    IKG_Buffer*                 piSendBuffer        = false;
    size_t                      uSendBufferSize     = 0;
    R2G_GAME_LOGIN_RESPOND*     pRespond            = NULL;
    GUID                        SimulateGuid;

    KGLOG_PROCESS_ERROR(m_piSocket);

    uSendBufferSize = sizeof(R2G_GAME_LOGIN_RESPOND);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uSendBufferSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_GAME_LOGIN_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    //虚拟数据
    SimulateGuid.Data1 = 1;
    SimulateGuid.Data2 = 1;
    SimulateGuid.Data3 = 1;

    pRespond->byProtocol    = r2g_game_login_respond;
    pRespond->byCode        = (BYTE)nRespondCode;
    pRespond->dwIP          = 1;
    pRespond->dwRoleID      = dwRoleID;
    pRespond->dwQueueID     = m_nQueueID;
    pRespond->guid          = SimulateGuid;
    pRespond->nPlayerIndex  = dwPacketIdentity;
    pRespond->nPort         = 1;

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::DoDeleteRoleRespond(DWORD dwPacketIdentity, int nRespondCode)
{
    int                         nResult             = false;
    int                         nRetCode            = false;
    IKG_Buffer*                 piSendBuffer        = false;
    size_t                      uSendBufferSize     = 0;
    R2G_DELETE_ROLE_RESPOND*    pRespond            = NULL;

    KGLOG_PROCESS_ERROR(m_piSocket);

    uSendBufferSize = sizeof(R2G_DELETE_ROLE_RESPOND);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uSendBufferSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_DELETE_ROLE_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_delete_role_respond;
    pRespond->nPlayerIndex  = dwPacketIdentity;
    pRespond->byCode      = nRespondCode;
    pRespond->dwRoleID      = 0;

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::DoSyncRoleList(int nPlayerIndex, int nRoleIndex, int nRoleCount, KROLE_LIST_INFO* pBaseData)
{
    int                         nResult             = false;
    int                         nRetCode            = false;
    IKG_Buffer*                 piSendBuffer        = false;
    size_t                      uSendBufferSize     = 0;
    R2G_SYNC_ROLE_LIST*         pRespond            = NULL;

    KGLOG_PROCESS_ERROR(m_piSocket);

    uSendBufferSize = sizeof(R2G_SYNC_ROLE_LIST);
    if (nRoleCount > 0)
    {
        ASSERT(pBaseData);
        ASSERT(nRoleIndex > 0 && nRoleIndex <= nRoleCount);
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
    //pRespond->nFreezeTime   = 0;
    pRespond->dwRoleID      = ERROR_ID;

    if (pBaseData != NULL)
    {
        pRespond->dwRoleID = pBaseData->dwPlayerID;
        memcpy(pRespond->byData, pBaseData, sizeof(KROLE_LIST_INFO));
    }

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::DoSyncNewbieMaps(int nPlayerIndex)
{
    int                         nResult                 = false;
    int                         nRetCode                = 0;
    IKG_Buffer*                 piSendBuffer            = NULL;
    R2G_SYNC_NEWBIE_MAPS*       pRespond                = NULL;
    KNEWBIE_MAP_COPY_INFO*      pMapCopyInfo            = NULL;
    int                         nSimulateMapCopyCount   = m_RelayConfig.nMapCount;

    KG_PROCESS_ERROR(m_piSocket);

    KG_COM_RELEASE(piSendBuffer);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2G_SYNC_NEWBIE_MAPS) + sizeof(KNEWBIE_MAP_COPY_INFO) * nSimulateMapCopyCount));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2G_SYNC_NEWBIE_MAPS*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->byProtocol    = r2g_sync_newbie_maps;
    pRespond->nPlayerIndex  = nPlayerIndex;
    pRespond->dwMapID       = 1;//默认ID为1
    pRespond->nCopyCount    = nSimulateMapCopyCount;

    if (nSimulateMapCopyCount > 0)
    {
        pMapCopyInfo = (KNEWBIE_MAP_COPY_INFO*)((BYTE*)pRespond + sizeof(R2G_SYNC_NEWBIE_MAPS));
        KGLOG_PROCESS_ERROR(pMapCopyInfo);

        memset(pMapCopyInfo, 0, sizeof(KNEWBIE_MAP_COPY_INFO) * nSimulateMapCopyCount);
    }

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::OnQueryAccountState(BYTE* pbyData, size_t uDataLen)
{
    int                          nResult             = false;
    int                          nRetCode            = false;
    IKG_Buffer*                  piSendBuffer        = NULL;
    G2R_QUERY_ACCOUNT_STATE*     pQueryAccountState  = (G2R_QUERY_ACCOUNT_STATE*)pbyData;
    R2G_SYNC_ACCOUNT_STATE*      pSyncAccountState   = NULL;

    pQueryAccountState->szAccount[sizeof(pQueryAccountState->szAccount) - 1] = '\0';

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2G_SYNC_ACCOUNT_STATE));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSyncAccountState = (R2G_SYNC_ACCOUNT_STATE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSyncAccountState);

    pSyncAccountState->byProtocol   = r2g_sync_account_state;
    pSyncAccountState->nPlayerIndex = pQueryAccountState->nPlayerIndex;    
    pSyncAccountState->bOnline      = m_RelayConfig.nUserOnlineResult;

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::OnQueryRoleList(BYTE* pbyData, size_t uDataLen)
{
    int                     nResult             = false;
    BYTE*                   pbyPos              = NULL;
    IKG_Buffer*             piSendBuffer        = NULL;
    KROLE_LIST_INFO*        pBaseData           = NULL;
    int                     nRoleCount          = 0;
    G2R_QUERY_ROLE_LIST*    pQueryRoleList      = (G2R_QUERY_ROLE_LIST*)pbyData;

    pQueryRoleList->szAccount[sizeof(pQueryRoleList->szAccount) - 1] = '\0';

    nRoleCount = m_RelayConfig.nGetRoleListCount;
    if (nRoleCount > 0)
    {
        piSendBuffer = KG_MemoryCreateBuffer(sizeof(KROLE_LIST_INFO) * nRoleCount);
        KGLOG_PROCESS_ERROR(piSendBuffer);

        pbyPos = (BYTE*)piSendBuffer->GetData();
        KGLOG_PROCESS_ERROR(pbyPos);
    }
    
    if (nRoleCount == 0)
    {
        DoSyncRoleList(pQueryRoleList->nPlayerIndex, 0, 0, NULL);
    }
    else
    {
        for (int nRoleIndex = 0; nRoleIndex < nRoleCount; nRoleIndex++)
        {
            pBaseData = (KROLE_LIST_INFO*)pbyPos;
            pBaseData->BaseInfo.byLevel = 0x3;
            pBaseData->BaseInfo.cRoleType = '6';
            pBaseData->BaseInfo.CurrentPos.byFaceDirection = '6';
            pBaseData->dwPlayerID = nRoleIndex;
            strncpy(pBaseData->szAccount, "AccountYeah", sizeof(pBaseData->szAccount));
            strncpy(pBaseData->szRoleName, "RoleNameyaah", sizeof(pBaseData->szRoleName));

            DoSyncRoleList(pQueryRoleList->nPlayerIndex, nRoleIndex + 1, nRoleCount, (KROLE_LIST_INFO*)pbyPos);
            pbyPos += sizeof(KROLE_LIST_INFO);
        }
    }

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KSimulateRelay::OnQueryNewbieMaps(BYTE* pbyData, size_t uDataLen)
{
    G2R_QUERY_NEWBIE_MAPS*  pRequest = (G2R_QUERY_NEWBIE_MAPS*)pbyData;

    DoSyncNewbieMaps(pRequest->nPlayerIndex);

    return true;
}

int KSimulateRelay::OnCreateRoleRequest(BYTE* pbyData, size_t uDataLen)
{
	int                       nResult   = false;
    G2R_CREATE_ROLE_REQUEST*  pRequest  = (G2R_CREATE_ROLE_REQUEST*)pbyData;

    DoCreateRoleRespond(m_RelayConfig.nCreatRoleResult, pRequest->nPlayerIndex);

	nResult = true;
// Exit0:
	return nResult;
}

int KSimulateRelay::OnDeleteRoleRequest(BYTE* pbyData, size_t uDataLen)
{
    int                       nResult   = false;
    G2R_DELETE_ROLE_REQUEST*  pRequest  = (G2R_DELETE_ROLE_REQUEST*)pbyData;

    DoDeleteRoleRespond(pRequest->nPlayerIndex, m_RelayConfig.nDeleteRoleResult);

    nResult = true;
// Exit0:
    return nResult;
}

int KSimulateRelay::OnGameLoginRequest(BYTE* pbyData, size_t uDataLen)
{
    int                       nResult            = false;
//    int                       nRetCode           = false;
    G2R_GAME_LOGIN_REQUEST*   pRequest           = (G2R_GAME_LOGIN_REQUEST*)pbyData;
    KInGameWorldPlayer*       pInGameWorldPlayer = NULL;

    if (m_RelayConfig.nEnterGameResult == eGameLoginSucceed)
    {
        if (m_RelayConfig.nAllowPlayerInGameWorld == 0)
        {
            m_RelayConfig.nEnterGameResult = eGameLoginQueueWait;
        }
        else
        {
            m_RelayConfig.nAllowPlayerInGameWorld--;
            // 记录
            pInGameWorldPlayer = new KInGameWorldPlayer;
            KGLOG_PROCESS_ERROR(pInGameWorldPlayer);

            pInGameWorldPlayer->nGameWorldMapID = pRequest->dwQueueID;
            strncpy(pInGameWorldPlayer->szAccount, pRequest->szAccountName, sizeof(pInGameWorldPlayer->szAccount));
            m_nLoginInGameWorldNum++;
            pInGameWorldPlayer->nNumInGameWorld = m_nLoginInGameWorldNum;
            m_InGameWorldPlayerList.push_back(pInGameWorldPlayer);
        }
    }

    DoReplyLoginData(pRequest->nPlayerIndex, pRequest->dwRoleID, m_RelayConfig.nEnterGameResult);
    m_nUserEnterGameRequestCount++;

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateRelay::OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen)//new
{
    BOOL                            bResult     = false;
//    G2R_CHANGE_EXT_POINT_RESPOND*   pRespond    = (G2R_CHANGE_EXT_POINT_RESPOND*)pbyData;
    //KRole*                          pRole       = NULL;

    KGLogPrintf(KGLOG_INFO, "Relay, OnChangeExtPointRespond\n");

    m_nRevChangeExtPointRespond     = true;

    //KGLOG_PROCESS_ERROR();
    //pRespond->bSucceed;
    //pRespond->dwRoleID;

    bResult = true;
//Exit0:
    return bResult;
}

int KSimulateRelay::OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen)//new
{
    BOOL                            bResult     = false;
    G2R_SET_CHARGE_FLAG_RESPOND*    pRespond    = (G2R_SET_CHARGE_FLAG_RESPOND*)pbyData;
    //KRole*                          pRole       = NULL;

    KGLogPrintf(KGLOG_INFO, "Relay, OnSetChargeFlagRespond\n");
    //pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwRoleID);
    //KGLOG_PROCESS_ERROR(pRole);

    //pRequest->byProtocol        = g2r_set_charge_flag_respond;
    //pRequest->bSucceed          = bSucceed;
    //pRequest->dwRoleID          = dwRoleID;
    //pRequest->nChargeFlag       = nChargeFlag;
    //pRequest->ExtPointInfo      = rExtPointInfo;
    //pRequest->dwEndOfDayTime    = dwEndOfDayTime;
    //pRequest->dwLeftTimeOfPoint = dwLeftTimeOfPoint;

    m_nRevPlayerSetChargeFlagRespond   = true;

    if (!pRespond->bSucceed)
    {
        //KGLogPrintf(
        //    KGLOG_ERR, "Set ChargeFlag failed! Acc = %s, Role = %s",
        //    pRole->m_pszAccount, pRole->m_szName
        //    );
        goto Exit0;
    }

    //g_pSO3GameCenter->m_GameServer.DoSetChargeFlagRespond(
    //    pRole->m_nConnIndex, pRole->m_dwPlayerID, pRespond->nChargeFlag, pRespond->ExtPointInfo,
    //    pRespond->dwEndOfDayTime, pRespond->dwLeftTimeOfPoint
    //    );

    bResult = true;
Exit0:
    return bResult;
}

int KSimulateRelay::OnGetZoneChargeFlagNotify(BYTE* pbyData, size_t uDataLen)//new
{
    BOOL                                bResult     = false;
//    G2R_GET_ZONE_CHARGE_FLAG_NOTIFY*    pNotify  = (G2R_GET_ZONE_CHARGE_FLAG_NOTIFY*)pbyData;

    KGLogPrintf(KGLOG_INFO, "Relay, OnGetZoneChargeFlagNotify\n");

    m_nRevGetZoneChargeFlagNotify = true;

    bResult = true;
//Exit0:
    return bResult;
}

int KSimulateRelay::ProcessPackage()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    int                     nConnectionAlive    = false;
    IKG_Buffer*             piPackage           = NULL;
    PROTOCOL_FUNCTION       ProtocolFunc        = NULL;
    BYTE*                   pbyData             = NULL;
    size_t                  uDataLen            = 0;
    KGR_PROTOCOL_HEADER*    pHeader             = NULL;

    KG_PROCESS_ERROR(m_piSocket);

    nConnectionAlive = m_piSocket->IsAlive();
    KGLOG_PROCESS_ERROR(nConnectionAlive);

    while (true)
    {
        timeval TimeoutValue = {0, 0};

        nRetCode = m_piSocket->CheckCanRecv(&TimeoutValue);
        if (nRetCode == -1)
        {
            nConnectionAlive = false;
            goto Exit0;
        }

        if (nRetCode == 0)
            break;

        KG_COM_RELEASE(piPackage);

        nRetCode = m_piSocket->Recv(&piPackage);
        if (nRetCode != 1)
        {
            nConnectionAlive = false;
            goto Exit0;
        }

        KGLOG_PROCESS_ERROR(piPackage);

        pbyData = (BYTE*)piPackage->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        uDataLen = piPackage->GetSize();
        KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KGR_PROTOCOL_HEADER));

        pHeader = (KGR_PROTOCOL_HEADER*)pbyData;
        KGLOG_PROCESS_ERROR(pHeader->byProtocol > g2r_protocol_begin);
        KGLOG_PROCESS_ERROR(pHeader->byProtocol < g2r_protocol_total);

        ProtocolFunc = m_ProtocolFunctions[pHeader->byProtocol];
        if (ProtocolFunc == NULL)
            continue;
        if (pHeader->byProtocol != m_nDoNotRespondProtocol)
        {
            (this->*ProtocolFunc)(pbyData, uDataLen);
        }       
    }

    nResult = true;
Exit0:
    if (m_piSocket && !nConnectionAlive)
    {
        KGLogPrintf(KGLOG_INFO, "[Gateway] Connection lost !\n");
        KG_COM_RELEASE(m_piSocket);
    }
    KG_COM_RELEASE(piPackage);
    return nResult;
}

int KSimulateRelay::OnKickAccountRequest(BYTE* pbyData, size_t uDataLen)
{
    int nResult    = false;
    //int nRetCode   = false;

    // 成功收到踢号信息
    m_nHadRevKickInfo = true;

    nResult    = true;
//Exit0:
    return true;
}

int KSimulateRelay::OnPingRequest(BYTE* pbyData, size_t uDataLen)
{
    int nResult    = false;
    int nRetCode   = false;

    nRetCode  = DoPingRespond();
    KG_PROCESS_ERROR(nRetCode);

    nResult    = true;
Exit0:
    return true;
    
}

int KSimulateRelay::DoPingRespond()
{
    int                         nResult                         = false;
    int                         nRetCode                        = false;
    IKG_Buffer*                 piSendBuffer                    = NULL;
    BYTE*                       pbySendBuffer                   = NULL;
    tagProtocolHeader*          ptagProtocolHeader              = NULL;

    KGLOG_PROCESS_ERROR(m_piSocket);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2G_PING_SIGNAL)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pbySendBuffer = (BYTE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbySendBuffer);

    ptagProtocolHeader = (tagProtocolHeader*)pbySendBuffer;
    ptagProtocolHeader->cProtocol = r2g_ping_signal;

    nRetCode = m_piSocket->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

void KSimulateRelay::WorkThreadFunction(void *pvParam)
{
    KSimulateRelay *pThis = (KSimulateRelay *)pvParam;

    ASSERT(pThis);

    pThis->ThreadFunction();
}

int KSimulateRelay::ThreadFunction()
{
    int nResult  = false;    
    //int nRetCode = false;

    while (!m_nExitFlag)
    {
        Activate();
        KGThread_Sleep(100);
    }  

    nResult = true;
//Exit0:
    return nResult;
}

int KSimulateRelay::SetWhenRevDoNotRespond(int nG2RProtocol)
{
    int nResult  = false;    
    //int nRetCode = false;

    KGLOG_PROCESS_ERROR(nG2RProtocol >= g2r_protocol_begin);
    KGLOG_PROCESS_ERROR(nG2RProtocol <= g2r_protocol_total);

    m_nDoNotRespondProtocol = nG2RProtocol;

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateRelay::SetGameLogResult(int nEnterGameResult)
{
    int  nResult   = false;

    KGLOG_PROCESS_ERROR(nEnterGameResult >= eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nEnterGameResult <= eGameLoginUnknownError);

    m_RelayConfig.nEnterGameResult = nEnterGameResult;

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateRelay::SetAllowLoginInGameWorldCount(int nAllowCount)
{
    int  nResult   = false;
    
    KGLOG_PROCESS_ERROR(nAllowCount >= 0);

    m_RelayConfig.nAllowPlayerInGameWorld = nAllowCount;

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateRelay::GetPlayer(KInGameWorldPlayer* pInGameWorldPlayer, char szAccountName[])
{
    int                                         nResult                 = false;
    int                                         nRetCode                = false;
    KInGameWorldPlayer*                         pTempInGameWorldPlayer  = NULL;
    std::list<KInGameWorldPlayer*>::iterator    it                      = m_InGameWorldPlayerList.begin();
    int                                         nHadFound               = false;

    KGLOG_PROCESS_ERROR(szAccountName);

    while (it != m_InGameWorldPlayerList.end())
    {
        pTempInGameWorldPlayer = *it;
        nRetCode = strcmp(szAccountName, pTempInGameWorldPlayer->szAccount);
        if (nRetCode == 0)
        {
            nHadFound = true;
            break;
        }  
        ++it;
    }

    if (nHadFound)
    {
        (*pInGameWorldPlayer).nGameWorldMapID = pTempInGameWorldPlayer->nGameWorldMapID;
        (*pInGameWorldPlayer).nNumInGameWorld = pTempInGameWorldPlayer->nNumInGameWorld;
        strncpy(pInGameWorldPlayer->szAccount, pTempInGameWorldPlayer->szAccount, sizeof(pInGameWorldPlayer->szAccount));
        (*pInGameWorldPlayer).szAccount[sizeof((*pInGameWorldPlayer).szAccount) - 1] = '\0';
    }

    nResult = true;
Exit0:
    return nResult;
}

int KSimulateRelay::ChangeTheLoginMap(int nMapID)
{
    int  nResult   = false;

    m_nQueueID  = nMapID;

    nResult = true;
//Exit0:
    return nResult;
}


int KSimulateRelay::HadConnections(int* pnHadConnected)
{
    int    nResult  = false;
    //int   nRetCode  = false;

    *pnHadConnected = m_nHadConnections;

    nResult  = true;
//Exit0:
    return nResult;
}

BOOL KSimulateRelay::DoChangeExtPointRequest()
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_CHANGE_EXT_POINT_REQUEST*   pRequest = NULL;
    const char*                     pszAccountName  = "aaaaa"; 
    DWORD                           dwRoleID        = 2233;
    WORD                            wExtPointIndex  = 5; 
    WORD                            wChangeValue    = 4;
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

    bRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_INFO, "Relay, DoChangeExtPointRequest\n");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KSimulateRelay::DoSetChargeFlagRequest()
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_SET_CHARGE_FLAG_REQUEST*    pRequest = NULL;
    const char* pszAccountName      = "0000"; 
    DWORD dwRoleID = 3333; 
    DWORD dwLastLoginIP = 123456; 
    int nChargeFlag = 2;
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

    bRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_INFO, "Relay, DoSetChargeFlagRequest\n");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

int KSimulateRelay::DoActivePresentCoodeRequest()
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    IKG_Buffer*                         piBuffer        = NULL;
    R2G_ACTIVE_PRESENT_CODE_REQUEST*    pRequest        = NULL;
    const char*                         pszAccountName  = "0000"; 

    DWORD                               dwRoleID        = 3333; 
    assert(pszAccountName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_ACTIVE_PRESENT_CODE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_ACTIVE_PRESENT_CODE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = r2g_active_present_code_request;

    strncpy(pRequest->szAccount, pszAccountName, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';
    strncpy(pRequest->szPresentCode, "1234567890", sizeof(pRequest->szPresentCode));
    pRequest->szPresentCode[sizeof(pRequest->szPresentCode) - 1] = '\0';
    pRequest->dwActiveIP    = 999;
    pRequest->dwRoleID      = dwRoleID;

    bRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_INFO, "Relay, DoActivePresentCoodeRequest\n");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

int KSimulateRelay::OnActivePresentCodeRespond( BYTE* pbyData, size_t uDataLen )
{
    BOOL                            bResult     = false;
//    G2R_ACTIVE_PRESENT_CODE_RESPOND*   pRespond    = (G2R_ACTIVE_PRESENT_CODE_RESPOND*)pbyData;
    //KRole*                          pRole       = NULL;

    KGLogPrintf(KGLOG_INFO, "Relay, OnActivePresentCodeRespond\n");

    m_nRevActivePresentCodeRespond     = true;

    bResult = true;
//Exit0:
    return bResult;
}

int KSimulateRelay::DoFreezeCoinRequest()
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_FREEZE_COIN_REQUEST*        pRequest = NULL;
    const char* pszAccountName      = "0000"; 

    DWORD   dwRoleID = 3333; 

    assert(pszAccountName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_FREEZE_COIN_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_FREEZE_COIN_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = r2g_freeze_coin_request;

    strncpy(pRequest->szAccount, pszAccountName, sizeof(pRequest->szAccount));
    pRequest->szAccount[sizeof(pRequest->szAccount) - 1] = '\0';

    pRequest->dwRequestID = dwRoleID;

    bRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_INFO, "Relay, DoFreezeCoinRequest\n");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

int KSimulateRelay::OnFreezeCoinResond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    //    G2R_FREEZE_COIN_RESPOND*   pRespond    = (G2R_FREEZE_COIN_RESPOND*)pbyData;
    //KRole*                          pRole       = NULL;

    KGLogPrintf(KGLOG_INFO, "Relay, OnFreezeCoinResond\n");

    m_nRevFreezeCoinRespond         = true;

    bResult = true;
    //Exit0:
    return bResult;
}

int KSimulateRelay::DoAccountExchangeRequest()
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    R2G_ACCOUNT_EXCHANGE_REQUEST*   pRequest = NULL;
    const char* pszAccountName      = "0000"; 

//    DWORD dwRoleID = 3333; 
    assert(pszAccountName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2G_ACCOUNT_EXCHANGE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (R2G_ACCOUNT_EXCHANGE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = r2g_account_exchange_request;

    bRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_INFO, "Relay, DoAccountExchangeRequest\n");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

int KSimulateRelay::OnAccountExchangeResond( BYTE* pbyData, size_t uDataLen )
{
    BOOL                            bResult     = false;
    //    G2R_ACCOUNT_EXCHANGE_RESPOND*   pRespond    = (G2R_ACCOUNT_EXCHANGE_RESPOND*)pbyData;
    //KRole*                          pRole       = NULL;

    KGLogPrintf(KGLOG_INFO, "Relay, OnAccountExchangeResond\n");

    m_nRevAccountExchangeRespond    = true;

    bResult = true;
    //Exit0:
    return bResult;
}
