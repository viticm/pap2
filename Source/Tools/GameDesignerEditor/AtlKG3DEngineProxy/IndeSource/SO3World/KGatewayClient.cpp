#include "stdafx.h"
#ifdef _CLIENT
#include <conio.h>
#include "KGatewayClient.h"
#include "KSO3World.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KPlayerClient.h"
#include "KRoleDBDataDef.h"
#include "GatewayClientProtocol.h"
#include "GatewayDef.h"
#include "Engine/KMemory.h"

#define REGISTER_G2C_FUNCTION(id, func, data)               \
    do                                                      \
    {                                                       \
        m_ProtocolFunctions[id] = &KGatewayClient::func;    \
        m_uPakSize[id]          = sizeof(data);             \
    } while(false)

KGatewayClient g_GatewayClient;

KGatewayClient::KGatewayClient(void)
{
    memset(m_szLoginIP, 0, sizeof(m_szLoginIP));
    memset(m_szAccountName, 0, sizeof(m_szAccountName));
    memset(&m_Password, 0, sizeof(m_Password));
    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));

    m_piSocketStream     = NULL;
    m_usLoginPort        = 0;
    m_ConnectState       = csInvalid;
    m_nLastPingTime      = 0;
    m_dwLeftTime         = 0;
    
    memset(m_ProtocolFunctions, 0, sizeof(m_ProtocolFunctions));
    memset(m_uPakSize, 0, sizeof(m_uPakSize));

    REGISTER_G2C_FUNCTION(g2c_ping_respond, OnPingRespond, G2C_PING_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_handshake_respond, OnHandshakeRespond, G2C_HANDSHAKE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_account_locked_notify, OnAccountLockedNotify, G2C_ACCOUNT_LOCKED_NOTIFY);
    REGISTER_G2C_FUNCTION(g2c_account_verify_respond, OnAccountVerifyRespond, G2C_ACCOUNT_VERIFY_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_kick_account_notify, OnKickAccountNotify, G2C_KICK_ACCOUNT_NOTIFY);
    REGISTER_G2C_FUNCTION(g2c_sync_role_list, OnSyncRoleList, G2C_SYNC_ROLE_LIST);
    REGISTER_G2C_FUNCTION(g2c_sync_newbie_maps, OnSyncNewbieMaps, G2C_SYNC_NEWBIE_MAPS);
    REGISTER_G2C_FUNCTION(g2c_create_role_respond, OnCreateRoleRespond, G2C_CREATE_ROLE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_delete_role_respond, OnDeleteRoleRespond, G2C_DELETE_ROLE_RESPOND);
    REGISTER_G2C_FUNCTION(g2c_sync_login_key, OnSyncLoginKey, G2C_SYNC_LOGIN_KEY);
    REGISTER_G2C_FUNCTION(g2c_sync_queue_state, OnSyncQueueState, G2C_SYNC_QUEUE_STATE);
    REGISTER_G2C_FUNCTION(g2c_giveup_queue_respond, OnGiveupQueueRespond, G2C_GIVEUP_QUEUE_RESPOND);
}

KGatewayClient::~KGatewayClient(void)
{
    KG_COM_RELEASE(m_piSocketStream);
}

void KGatewayClient::Activate()
{   
    ProcessConnectGateway();

    ProcessPackage();

    DoPingSignal();

    return;
}

void KGatewayClient::ProcessConnectGateway()
{
    int     nResult     = false;
    int     nRetCode    = false;
    timeval Timeout     = {0, 0};

    KG_PROCESS_SUCCESS(m_piSocketStream == NULL);
    KG_PROCESS_SUCCESS(m_ConnectState != csWaitingConnectOK);
                
    nRetCode = m_piSocketStream->CheckCanSend(&Timeout);
    KGLOG_PROCESS_ERROR(nRetCode >= 0);

    KG_PROCESS_SUCCESS(nRetCode == 0); // time out

    DoHandshakeRequest();
    
    m_ConnectState = csInvalid;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        m_ConnectState = csInvalid;
        KG_COM_RELEASE(m_piSocketStream);

        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_UNABLE_TO_CONNECT_SERVER };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }
    }
    return;
}

void KGatewayClient::ProcessPackage()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    IKG_Buffer*             piRecvPackage       = NULL;
    size_t                  uBufferSize         = 0;
    tagProtocolHeader*      pProtocolHeader     = NULL;
    PROTOCOL_FUNCTION       pfnProtocolFunction = NULL;
    int                     nProtocol           = 0;
    timeval                 TimeoutValue        = {0, 0};

    KG_PROCESS_SUCCESS(m_piSocketStream == NULL || m_ConnectState == csWaitingConnectOK);

    nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
    KG_PROCESS_SUCCESS(nRetCode == 0);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nRetCode = m_piSocketStream->Recv(&piRecvPackage);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    KGLOG_PROCESS_ERROR(piRecvPackage);

    uBufferSize = (size_t)piRecvPackage->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(tagProtocolHeader));

    pProtocolHeader = (tagProtocolHeader*)piRecvPackage->GetData();
    KGLOG_PROCESS_ERROR(pProtocolHeader);
    
    nProtocol = pProtocolHeader->cProtocol;
    KGLOG_PROCESS_ERROR(nProtocol >= 0 && nProtocol <= UCHAR_MAX);

    KGLOG_PROCESS_ERROR(uBufferSize >= m_uPakSize[nProtocol]);
    
    pfnProtocolFunction = m_ProtocolFunctions[nProtocol];
    KGLOG_PROCESS_ERROR(pfnProtocolFunction);

    (this->*pfnProtocolFunction)((BYTE*)pProtocolHeader, uBufferSize);

Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    if (!nResult)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_MISS_CONNECTION };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }

        KG_COM_RELEASE(m_piSocketStream);
    }
    return;
}

int KGatewayClient::SetGatewayAddress(const char cszLoginIP[], unsigned short usLoginPort)
{
    assert(cszLoginIP);
    strncpy(m_szLoginIP, cszLoginIP, sizeof(m_szLoginIP));
    m_szLoginIP[sizeof(m_szLoginIP) - 1] = '\0';
    m_usLoginPort       = usLoginPort;
    
    return true;
}

int KGatewayClient::BeginLogin(const char cszAccountName[], const char cszPasswordMD5[], const char cszActiveCode[])
{
    int nResult  = false;
    int nRetCode = false;

    assert(cszAccountName);
    assert(cszPasswordMD5);
    //assert(cszActiveCode); could be null

    strncpy(m_szAccountName, cszAccountName, sizeof(m_szAccountName));
    m_szAccountName[sizeof(m_szAccountName) - 1] = '\0';

    strncpy(m_Password.szPassword, cszPasswordMD5, sizeof(m_Password.szPassword));
    m_Password.szPassword[sizeof(m_Password.szPassword) - 1] = '\0';
    
    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));
    if (cszActiveCode)
    {
        strncpy(m_szActivateCode, cszActiveCode, sizeof(m_szActivateCode));
        m_szActivateCode[sizeof(m_szActivateCode) - 1] = '\0';
    }
    
    nRetCode = ConnectGateway();
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nResult = true;
Exit0:
    return nResult;
}

int KGatewayClient::GetRoleListItem(UI_ROLE_LIST_ITEM *pRoleListItem, int nRoleIndex)
{
    int                 nResult    = false;
    int                 nRoleCount = 0;

    assert(pRoleListItem);
    assert(nRoleIndex >= 0);
    nRoleCount = (int)m_RoleList.size();
    assert(nRoleIndex < nRoleCount);

    *pRoleListItem = m_RoleList[nRoleIndex];
    
    nResult = true;
//Exit0:
    return nResult;
}

int KGatewayClient::QueryHometownList()
{
    DoQueryHometownListRequest();

    return true;
}

int KGatewayClient::RoleLogin(const char cszRoleName[])
{
    int nResult  = false;
    int nRetCode = false;
    ROLE_LIST::iterator it;

    assert(cszRoleName);

    for (it = m_RoleList.begin(); it != m_RoleList.end(); ++it) 
    {
        nRetCode = strcmp(it->szRoleName, cszRoleName);
        if (!nRetCode)
            break;
    }

    if (it == m_RoleList.end()) 
    {
        assert(!"Selected role not in list");
        goto Exit0;
    }
        
    DoLoginGameRequest(it->dwRoleID);

    nResult = true;
Exit0:
    return nResult;
}

int KGatewayClient::DeleteRole(const char cszRoleName[], const char cszSecPasswordMD5[])
{
    int   nResult    = false;
    DWORD dwPlayerID = ERROR_ID;

    assert(cszRoleName);
    
    dwPlayerID = GetRoleID(cszRoleName);
    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);

    DoDeleteRoleRequest(dwPlayerID);
    
    nResult = true;
Exit0:
    return nResult;
}

int KGatewayClient::CreateRole(const UI_ROLE_CREATE_PARAM &crRoleCreateParam)
{
    DoCreateRoleRequest(crRoleCreateParam);
    
    return true;
}

int KGatewayClient::GetRoleCount()
{
    return (int)m_RoleList.size();
}

DWORD KGatewayClient::GetLeftTime()
{
    return m_dwLeftTime;
}

time_t KGatewayClient::GetLastLoginTime()
{
    return m_nLastLoginTime;
}

DWORD KGatewayClient::GetLastLoginIP()
{
    return m_dwLastLoginIP;
}

int KGatewayClient::ReLogin()
{
    int nResult  = false;
    int nRetCode = false;

	assert(m_szAccountName && m_szAccountName[0]);
	assert(m_Password.szPassword[0]);

	memset(m_szActivateCode, 0, sizeof(m_szActivateCode));
    
    nRetCode = ConnectGateway();
    KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
    return nResult;
}

void KGatewayClient::Reset()
{
    KG_COM_RELEASE(m_piSocketStream);
    m_ConnectState = csInvalid;
    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));
}

BOOL KGatewayClient::AddRoleToRoleList(KROLE_LIST_INFO* pRoleListInfo)
{
    UI_ROLE_LIST_ITEM RoleListItem;
    
    assert(pRoleListInfo);

    strncpy(RoleListItem.szAccountName, pRoleListInfo->szAccount, sizeof(RoleListItem.szAccountName));
    RoleListItem.szAccountName[sizeof(RoleListItem.szAccountName) - 1] = '\0';

    strncpy(RoleListItem.szRoleName, pRoleListInfo->szRoleName, sizeof(RoleListItem.szRoleName));
    RoleListItem.szRoleName[sizeof(RoleListItem.szRoleName) - 1] = '\0';
    
    RoleListItem.dwRoleID       = pRoleListInfo->dwPlayerID;
    RoleListItem.nRoleType      = pRoleListInfo->BaseInfo.cRoleType;
    RoleListItem.nLevel         = pRoleListInfo->BaseInfo.byLevel;
    RoleListItem.dwMapID        = pRoleListInfo->BaseInfo.CurrentPos.dwMapID;
    RoleListItem.nMapCopyIndex  = pRoleListInfo->BaseInfo.CurrentPos.nMapCopyIndex;
    RoleListItem.nLastSaveTime  = pRoleListInfo->BaseInfo.nLastSaveTime;
    RoleListItem.nTotalGameTime = pRoleListInfo->BaseInfo.nTotalGameTime;
    RoleListItem.nFreezeTime    = pRoleListInfo->nFreezeTime;

    memcpy(RoleListItem.wRepresentID, pRoleListInfo->BaseInfo.wRepresentId, sizeof(RoleListItem.wRepresentID));

    KGLogPrintf(
        KGLOG_INFO, "[GatewayClient] Update role list: %s:%u\n", 
        pRoleListInfo->szRoleName, pRoleListInfo->dwPlayerID
    );

    m_RoleList.push_back(RoleListItem);

    return true;
}

DWORD KGatewayClient::GetRoleID(const char* cszRoleName)
{
    int   nRetCode    = 0;
    DWORD dwRetRoleID = ERROR_ID;
    
    assert(cszRoleName);

    for (ROLE_LIST::iterator it = m_RoleList.begin(); it != m_RoleList.end(); ++it)
    {
        nRetCode = strncmp(it->szRoleName, cszRoleName, sizeof(it->szRoleName));
        if (nRetCode == 0)
        {
            dwRetRoleID = it->dwRoleID;
            break;
        }
    }
    return dwRetRoleID;
}

BOOL KGatewayClient::ConnectGateway()
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    timeval                     TimeoutValue    = {60, 0};
    KG_SocketConnector          Connector;

    assert(m_piSocketStream == NULL);
    m_piSocketStream = Connector.ConnectSecurity(m_szLoginIP, m_usLoginPort, KSG_ENCODE_DECODE);

    KGLogPrintf(
        KGLOG_INFO, 
        "[KGatewayClient] Connect to %s : %u ... ... [%s]", 
        m_szLoginIP, m_usLoginPort, m_piSocketStream ? "OK" : "Failed"
    );

    if (m_piSocketStream == NULL)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_UNABLE_TO_CONNECT_SERVER };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }
        goto Exit0;
    }

    nRetCode = m_piSocketStream->SetTimeout(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    m_ConnectState = csWaitingConnectOK;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayClient::Send(IKG_Buffer* piSendBuffer)
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;

    assert(piSendBuffer);

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGatewayClient::DoPingSignal()
{
    BOOL             bResult      = false;
    int              nRetCode     = 0;
    time_t           nNowTime     = 0;
    IKG_Buffer*      piSendBuffer = NULL;
    C2G_PING_SIGNAL* pPing        = NULL;

    KG_PROCESS_SUCCESS(m_piSocketStream == NULL);

    nNowTime = time(NULL);

    KG_PROCESS_SUCCESS(nNowTime < m_nLastPingTime + 8);
    m_nLastPingTime = nNowTime;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(C2G_PING_SIGNAL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPing = (C2G_PING_SIGNAL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPing);

    pPing->byProtocol = c2g_ping_signal;
    pPing->dwTime     = (DWORD)nNowTime;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoHandshakeRequest()
{
    BOOL                                bResult         = false;
    int                                 nRetCode        = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    C2G_HANDSHAKE_REQUEST*  pRequest        = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_HANDSHAKE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_HANDSHAKE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol        = c2g_handshake_request;
    pRequest->nGameWorldVersion = GAME_WORLD_CURRENT_VERSION;
    pRequest->nGatewayVersion   = GATEWAY_VERSION;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoAccountVerifyRequest()
{
    BOOL                         bResult       = false;
    int                          nRetCode      = false;
    IKG_Buffer*                  piSendBuffer  = NULL;
    C2G_ACCOUNT_VERIFY_REQUEST*  pRequest      = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_ACCOUNT_VERIFY_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_ACCOUNT_VERIFY_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol  = c2g_account_verify_request;

    strncpy(pRequest->szAcc, m_szAccountName, sizeof(pRequest->szAcc));
    pRequest->szAcc[sizeof(pRequest->szAcc) - 1] = '\0';

    strncpy(pRequest->szPsw, m_Password.szPassword, sizeof(pRequest->szPsw));
    pRequest->szPsw[sizeof(pRequest->szPsw) - 1] = '\0';

    strncpy(pRequest->szAct, m_szActivateCode, sizeof(pRequest->szAct));
    pRequest->szAct[sizeof(pRequest->szAct) - 1] = '\0';

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoCreateRoleRequest(const UI_ROLE_CREATE_PARAM &crRoleCreateParam)
{
    BOOL                        bResult          = false;
    int                         nRetCode         = false;
    IKG_Buffer*                 piSendBuffer     = NULL;
    C2G_CREATE_ROLE_REQUEST*    pRequest         = NULL;
    KUSER_ROLE_GENERATOR_PARAM* pRoleCreateParam = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_CREATE_ROLE_REQUEST)  + sizeof(KUSER_ROLE_GENERATOR_PARAM));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_CREATE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = c2g_create_role_request;

    pRoleCreateParam = (KUSER_ROLE_GENERATOR_PARAM*)pRequest->byData;

    strncpy(pRoleCreateParam->szRoleName, crRoleCreateParam.szRoleName, sizeof(pRoleCreateParam->szRoleName));
    pRoleCreateParam->szRoleName[sizeof(pRoleCreateParam->szRoleName) - 1] = '\0';

    pRoleCreateParam->cRoleType     = (char)crRoleCreateParam.nRoleType;
    pRoleCreateParam->dwMapID       = crRoleCreateParam.dwMapID;
    pRoleCreateParam->nMapCopyIndex = crRoleCreateParam.nMapCopyIndex;

    memcpy(
        pRoleCreateParam->wRepresentId, 
        crRoleCreateParam.wRepresentId, 
        sizeof(pRoleCreateParam->wRepresentId)
    );

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    bResult = true;
Exit0:
    if (!bResult) 
    {  
        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_MISS_CONNECTION };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }

        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoDeleteRoleRequest(DWORD dwRoleID)
{
    BOOL                        bResult          = false;
    int                         nRetCode         = false;
    C2G_DELETE_ROLE_REQUEST*    pDeleteRole      = NULL;
    IKG_Buffer*                 piSendBuffer     = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_DELETE_ROLE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDeleteRole = (C2G_DELETE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDeleteRole);
    
    pDeleteRole->byProtocol = c2g_delete_role_request;
    pDeleteRole->dwRoleID   = dwRoleID;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    bResult = true;
Exit0:
    if (!bResult) 
    {  
        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_MISS_CONNECTION };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoLoginGameRequest(DWORD dwRoleID)
{
    BOOL                        bResult      = false;
    int                         nRetCode     = 0;
    C2G_GAME_LOGIN_REQUEST*     pRoleLogin   = NULL;
    IKG_Buffer*                 piSendBuffer = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(C2G_GAME_LOGIN_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRoleLogin = (C2G_GAME_LOGIN_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRoleLogin);

    pRoleLogin->byProtocol  = c2g_game_login_request;
    pRoleLogin->dwRoleID    = dwRoleID;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (!bResult) 
	{  
        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_MISS_CONNECTION };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }

        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoQueryHometownListRequest()
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    C2G_QUERY_NEWBIE_MAPS*      pRequest        = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(C2G_QUERY_NEWBIE_MAPS));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_QUERY_NEWBIE_MAPS*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = c2g_query_newbie_maps;
    pRequest->dwReserved = 0;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_HometownList.clear();

    bResult = true;
Exit0:
    if (!bResult) 
	{  
        if (g_pGameWorldUIHandler)
        {
            KUIEventLoginNotify param = { LPUM_MISS_CONNECTION };
            g_pGameWorldUIHandler->OnLoginNotify(param);
        }

        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewayClient::DoGiveupQueueRequest()
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    C2G_GIVEUP_QUEUE_REQUEST*   pRequest        = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(C2G_GIVEUP_QUEUE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (C2G_GIVEUP_QUEUE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocol = c2g_giveup_queue_request;
    pRequest->byNothing  = 0;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

//////////////////////////////////////////////////////////////////////////

void KGatewayClient::OnPingRespond(BYTE* pbyData, size_t uDataLen)
{
}

void KGatewayClient::OnHandshakeRespond(BYTE* pbyData, size_t uDataLen)
{
    G2C_HANDSHAKE_RESPOND*   pRespond  = (G2C_HANDSHAKE_RESPOND*)pbyData;
    KUIEventLoginNotify      UINotify  = {LPUM_LOGIN_VERIFY_UNKNOWN_ERROR};
   
    switch (pRespond->byCode)
    {
    case ghcHandshakeSucceed:
        DoAccountVerifyRequest();
        break;

    case ghcGatewayVersionError: 
        UINotify.nEvent = LPUM_UNMATCHED_PROTOCOL_VERSION;
        break;

    case ghcGameWorldMaintenance:
        UINotify.nEvent = LPUM_SYSTEM_MAINTENANCE;
        break;

    case ghcAccountSystemLost:
        UINotify.nEvent = LPUM_HANDSHAKE_ACCOUNT_SYSTEM_LOST;
        break;

    case ghcGameWorldVersionError:  
        UINotify.nEvent = LPUM_UNMATCHED_PROTOCOL_VERSION;
        break;

    default:
        UINotify.nEvent = LPUM_LOGIN_VERIFY_UNKNOWN_ERROR;
        break;
    }

    if (g_pGameWorldUIHandler && pRespond->byCode != ghcHandshakeSucceed)
    {
        g_pGameWorldUIHandler->OnLoginNotify(UINotify);
    }
}

void KGatewayClient::OnAccountLockedNotify(BYTE* pbyData, size_t uDataLen)
{
    KUIEventLoginNotify UINotify = { LPUM_LOGIN_VERIFY_ALREADY_IN_GATEWAY };

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(UINotify);
    }
    return; 
}

void KGatewayClient::OnAccountVerifyRespond(BYTE* pbyData, size_t uDataLen)
{
}

void KGatewayClient::OnKickAccountNotify(BYTE* pbyData, size_t uDataLen)
{
    KUIEventLoginNotify UINotify = { LPUM_LOGIN_VERIFY_IN_GAME };

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(UINotify);
    }
    return; 
}

void KGatewayClient::OnSyncRoleList(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult         = false;
    int                     nRetCode        = 0;
    G2C_SYNC_ROLE_LIST*     pRoleList       = (G2C_SYNC_ROLE_LIST*)pbyData;
    KROLE_LIST_INFO*        pRoleListInfo   = NULL;
    KUIEventLoginNotify     UINotify        = { LPUM_MISS_CONNECTION };
    
    if (pRoleList->nRoleCount > 0)
    {
        KGLOG_PROCESS_ERROR(uDataLen == sizeof(G2C_SYNC_ROLE_LIST) + sizeof(KROLE_LIST_INFO));

        pRoleListInfo = (KROLE_LIST_INFO*)pRoleList->byData;

        pRoleListInfo->szAccount[sizeof(pRoleListInfo->szAccount) - 1]      = '\0';
        pRoleListInfo->szRoleName[sizeof(pRoleListInfo->szRoleName) - 1]    = '\0';

        nRetCode = AddRoleToRoleList(pRoleListInfo);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    UINotify.nEvent = LPUM_GET_ROLE_LIST_SUCCESS;
    
    bResult = true;
Exit0:
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(UINotify);

        if (bResult && pRoleList->nRoleIndex == pRoleList->nRoleCount)
        {
            UINotify.nEvent = LPUM_GET_ALL_ROLE_LIST_SUCCESS;

            g_pGameWorldUIHandler->OnLoginNotify(UINotify);
        }
    }
    return;  
}

void KGatewayClient::OnSyncNewbieMaps(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = 0;
    G2C_SYNC_NEWBIE_MAPS*       pRespond        = (G2C_SYNC_NEWBIE_MAPS*)pbyData;
    int                         nCopyCount      = pRespond->nCopyCount;
    KNEWBIE_MAP_COPY_INFO*      pCopyInfo       = pRespond->CopyInfo;
    KUIEventLoginNotify         Param           = { LPUM_MISS_CONNECTION };
    KUI_HOMETOWN_MAP_INFO       MapInfo;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(G2C_SYNC_NEWBIE_MAPS) + sizeof(KNEWBIE_MAP_COPY_INFO) * nCopyCount);

    MapInfo.dwMapID = pRespond->dwMapID;

    for (int nIndex = 0; nIndex < nCopyCount; nIndex++)
    {
        KUI_HOMETOWN_COPY_INFO CopyInfo;

        CopyInfo.dwCopyIndex = (DWORD)pCopyInfo[nIndex].nCopyIndex;
        CopyInfo.nLoadFactor = pCopyInfo[nIndex].nLoadFactor;

        MapInfo.LoadFactorTable.push_back(CopyInfo);
    }

    m_HometownList.push_back(MapInfo);

	Param.nEvent = LPUM_UPDATE_HOMETOWN_LIST;

    bResult = true;
Exit0:
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(Param);
    }
    return;  
}

void KGatewayClient::OnCreateRoleRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult             = false;
    int                         nRetCode            = 0;
    G2C_CREATE_ROLE_RESPOND*    pCreateRoleRespond  = (G2C_CREATE_ROLE_RESPOND*)pbyData;
    KUIEventLoginNotify         Param               = { LPUM_MISS_CONNECTION };

    switch (pCreateRoleRespond->byCode)
    {
    case eCreateRoleSucceed:
        {
            KROLE_LIST_INFO* pRoleListInfo = NULL;

            KGLOG_PROCESS_ERROR(uDataLen == sizeof(G2C_CREATE_ROLE_RESPOND) + sizeof(KROLE_LIST_INFO));

            pRoleListInfo = (KROLE_LIST_INFO*)pCreateRoleRespond->byData;

            pRoleListInfo->szAccount[sizeof(pRoleListInfo->szAccount) - 1] = '\0';
            pRoleListInfo->szRoleName[sizeof(pRoleListInfo->szRoleName) - 1] = '\0';

            nRetCode = AddRoleToRoleList(pRoleListInfo);
            KGLOG_PROCESS_ERROR(nRetCode);

            // -- µÇÂ¼ÓÎÏ· --        
            Param.nEvent = LPUM_CREATE_ROLE_SUCCESS;

            DoLoginGameRequest(pRoleListInfo->dwPlayerID);
        }
        break;

    case eCreateRoleNameAlreadyExist:
        Param.nEvent = LPUM_CREATE_ROLE_NAME_EXIST;
        break;

    case eCreateRoleInvalidRoleName:
        Param.nEvent = LPUM_CREATE_ROLE_INVALID_NAME;
        break;

    case eCreateRoleUnableToCreate:
        Param.nEvent = LPUM_CREATE_ROLE_UNABLE_TO_CREATE;
        break;

    default:
        break;
    }

    bResult = true;
Exit0:
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(Param);
    }
    return;
}

void KGatewayClient::OnDeleteRoleRespond(BYTE* pbyData, size_t uDataLen)
{
    return;
}

void KGatewayClient::OnSyncLoginKey(BYTE* pbyData, size_t uDataLen)
{
    int                         nResult             = false;
    int                         nRetCode            = false;
    G2C_SYNC_LOGIN_KEY*         pGameLoginInfo      = (G2C_SYNC_LOGIN_KEY*)pbyData;
    const char*                 pcszGameServerIP    = NULL;
    KUIEventLoginNotify         Param               = { LPUM_MISS_CONNECTION }; 
    struct in_addr              GameServerAddress;

    switch (pGameLoginInfo->byCode)
    {
    case eGameLoginSucceed:
        Param.nEvent = LPUM_REQUEST_LOGIN_GAME_SUCCESS;
        KG_COM_RELEASE(m_piSocketStream);
        break;

    case eGameLoginSystemMaintenance:
        Param.nEvent = LPUM_REQUEST_LOGIN_GAME_MAINTENANCE;
        break;

    case eGameLoginOverload:
        Param.nEvent = LPUM_REQUEST_LOGIN_GAME_OVERLOAD;
        break;

    case eGameLoginRoleFreeze:
        Param.nEvent = LPUM_REQUEST_LOGIN_GAME_ROLEFREEZE;
        break;

    case eGameLoginUnknownError:
        Param.nEvent = LPUM_REQUEST_LOGIN_GAME_UNKNOWN_ERROR;
        break;

    default:
        break;
    }

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(Param);
    }

    if (pGameLoginInfo->byCode == eGameLoginSucceed)
    {
        GameServerAddress.s_addr = pGameLoginInfo->dwIP;
        pcszGameServerIP = inet_ntoa(GameServerAddress);
        KGLOG_PROCESS_ERROR(pcszGameServerIP);

        nRetCode = g_PlayerClient.Connect(pcszGameServerIP, pGameLoginInfo->nPort);
        if (nRetCode) 
        {
            g_PlayerClient.DoHandshakeRequest(pGameLoginInfo->dwRoleID, pGameLoginInfo->guid);
        }
    }

    nResult = true;
Exit0:
    //KG_COM_RELEASE(m_piSocketStream);
    return;
}

void KGatewayClient::OnSyncQueueState(BYTE* pbyData, size_t uDataLen)
{
    G2C_SYNC_QUEUE_STATE* pQueueState = (G2C_SYNC_QUEUE_STATE*)pbyData;

    cprintf("Queue position: %d\n", pQueueState->nPosition);

    if (g_pGameWorldUIHandler)
    {
        KUIEventLoginQueueState Param = { pQueueState->nPosition }; 
        g_pGameWorldUIHandler->OnLoginQueueState(Param);
    }
}

void KGatewayClient::OnGiveupQueueRespond(BYTE* pbyData, size_t uDataLen)
{
    KUIEventLoginNotify         Param               = { LPUM_GIVEUP_QUEUE_SUCCESS }; 

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnLoginNotify(Param);
    }

    return;
}

#endif  // _CLIENT
