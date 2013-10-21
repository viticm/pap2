#include "stdafx.h"
#include "KApexProxy.h"
#include "Common/KG_Socket.h"
#include "KSO3GameCenter.h"

KApexProxy::KApexProxy()
{
    m_piSocketStream = NULL;
    m_bSendErrorFlag = false;
    m_nLastPingTime  = 0;
}

BOOL KApexProxy::Init()
{
    m_bSendErrorFlag = false;
    m_piSocketStream = NULL;

    Connect(g_pSO3GameCenter->m_Settings.m_szApexServerIP, g_pSO3GameCenter->m_Settings.m_nApexServerPort);

    return true;
}

void KApexProxy::UnInit()
{
    Disconnect();
}

BOOL KApexProxy::Connect(const char szIP[], int nPort)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    timeval             TimeoutValue    = {0, 0};
    KG_SocketConnector  Connector;

    assert(m_piSocketStream == NULL);

    m_piSocketStream = Connector.Connect(szIP, nPort);
    KG_PROCESS_ERROR(m_piSocketStream);

    bRetCode = m_piSocketStream->SetTimeout(&TimeoutValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_bSendErrorFlag = false;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_COM_RELEASE(m_piSocketStream);
    }

    KGLogPrintf(
        KGLOG_INFO, "[ApexProxy] Connect to %s : %d ... ... [%s]", szIP, nPort, bResult ? "Ok" : "Failed"
    );


    return bResult;
}

void KApexProxy::Disconnect()
{
    KG_COM_RELEASE(m_piSocketStream);

    KGLogPrintf(KGLOG_INFO, "[ApexProxy] Apex Disconnected.");
}

BOOL KApexProxy::Breathe()
{
    BOOL        bResult            = false;
    int         nRetCode           = false;
    IKG_Buffer* piBuffer           = NULL;
    BOOL        bConnectionAlive   = true;
    BYTE*       pbyRecvData        = NULL;
    size_t      uRecvDataLen       = 0;

    // if (m_piSocketStream == NULL) // Try Connect
    // {
    //     nRetCode = Connect(
    //         g_pSO3GameCenter->m_Settings.m_szApexServerIP,
    //         g_pSO3GameCenter->m_Settings.m_nApexServerPort
    //     );
    //     KG_PROCESS_ERROR(nRetCode);
    // }

    KG_PROCESS_ERROR(m_piSocketStream);

    // assert(m_piSocketStream);

    if (m_bSendErrorFlag)
    {
        bConnectionAlive = false;
        m_bSendErrorFlag = false;
        goto Exit0;
    }

    while (true)
    {
        timeval TimeVal = {0, 0};

        if (g_pSO3GameCenter->m_nTimeNow - m_nLastPingTime > g_pSO3GameCenter->m_Settings.m_nApexPingCycle)
        {
            DoPingSignal();
            m_nLastPingTime = g_pSO3GameCenter->m_nTimeNow;
        }

        nRetCode = m_piSocketStream->CheckCanRecv(&TimeVal);
        if (nRetCode == -1)
        {
            bConnectionAlive = false;
            goto Exit0;
        }
        if (nRetCode == 0)
        {
            break;
        }

        KGLOG_PROCESS_ERROR(nRetCode == 1);

        KG_COM_RELEASE(piBuffer);

        nRetCode = m_piSocketStream->Recv(&piBuffer);
        if (nRetCode == -1)
        {
            bConnectionAlive = false;
            goto Exit0;
        }
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        pbyRecvData = (BYTE*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pbyRecvData);

        uRecvDataLen = piBuffer->GetSize();

        ProcessRecvData(pbyRecvData, uRecvDataLen);
    }

    ClearTimeoutKickNode();

    bResult = true;
Exit0:
    if (m_piSocketStream && !bConnectionAlive)
    {
        KGLogPrintf(KGLOG_INFO, "[ApexProxy] Connection lost.");
        KG_COM_RELEASE(m_piSocketStream);
    }

    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::Send(IKG_Buffer* piBuffer)
{
    BOOL bResult    = false;
    int  nRetCode   = 0;

    KG_PROCESS_ERROR(m_piSocketStream);

    nRetCode = m_piSocketStream->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    if (!bResult)
        m_bSendErrorFlag = true;

    return bResult;
}

//////////////////////////////////////////////////////////////////////////
BOOL KApexProxy::DoPingSignal()
{
    BOOL                bResult     = false;
    APEX_PING_SIGNAL*   pPingSignal = NULL;
    IKG_Buffer*         piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_PING_SIGNAL));
    KGLOG_PROCESS_ERROR(piBuffer);

    pPingSignal = (APEX_PING_SIGNAL*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPingSignal);

    pPingSignal->cMsgId  = 'C';
    pPingSignal->nSendId = 0;

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoRoleLogin(DWORD dwRoleID)
{
    BOOL                bResult     = false;
    APEX_ROLE_LOGIN*    pRoleLogin  = NULL;
    IKG_Buffer*         piBuffer    = NULL;
    KRole*              pRole       = NULL;
    size_t              nNameLen    = 0;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_ROLE_LOGIN));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRoleLogin = (APEX_ROLE_LOGIN*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRoleLogin);

    pRoleLogin->cMsgId  = 'L';
    pRoleLogin->nSendId = (int)dwRoleID;

    strncpy(pRoleLogin->szName, pRole->m_szName, sizeof(pRoleLogin->szName));
    pRoleLogin->szName[sizeof(pRoleLogin->szName) - 1] = '\0';

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoRoleLogout(DWORD dwRoleID)
{
    BOOL                bResult     = false;
    APEX_ROLE_LOGOUT*   pRoleLogout = NULL;
    IKG_Buffer*         piBuffer    = NULL;
    KRole*              pRole       = NULL;
    size_t              nNameLen    = 0;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_ROLE_LOGOUT));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRoleLogout = (APEX_ROLE_LOGOUT*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRoleLogout);

    pRoleLogout->cMsgId  = 'G';
    pRoleLogout->nSendId = (int)dwRoleID;

    strncpy(pRoleLogout->szName, pRole->m_szName, sizeof(pRoleLogout->szName));
    pRoleLogout->szName[sizeof(pRoleLogout->szName) - 1] = '\0';

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoSyncRoleIP(DWORD dwRoleID, int nIPAddr)
{
    BOOL                    bResult         = false;
    APEX_SYNC_ROLE_IPADDR*  pSyncRoleIPAddr = NULL;
    IKG_Buffer*             piBuffer        = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_SYNC_ROLE_IPADDR));
    KGLOG_PROCESS_ERROR(piBuffer);

    pSyncRoleIPAddr = (APEX_SYNC_ROLE_IPADDR*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSyncRoleIPAddr);

    pSyncRoleIPAddr->cMsgId         = 'S';
    pSyncRoleIPAddr->nSendId        = (int)dwRoleID;
    pSyncRoleIPAddr->byUserIPFlag   = 0x01;
    pSyncRoleIPAddr->nIPAddr        = nIPAddr;

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoReturnClientRet(DWORD dwRoleID, int nRetCode)
{
    BOOL                    bResult         = false;
    APEX_CLIENT_RETCODE*    pClientRetCode  = NULL;
    IKG_Buffer*             piBuffer        = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_CLIENT_RETCODE));
    KGLOG_PROCESS_ERROR(piBuffer);

    pClientRetCode = (APEX_CLIENT_RETCODE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pClientRetCode);

    pClientRetCode->cMsgId      = 'R';
    pClientRetCode->nSendId     = (int)dwRoleID;
    pClientRetCode->nRetCode    = nRetCode;

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoSyncUserData(DWORD dwRoleID, BYTE* pbyData, size_t uDataSize)
{
    BOOL                    bResult         = false;
    APEX_SYNC_USER_DATA*    pSyncUserData   = NULL;
    IKG_Buffer*             piBuffer        = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_SYNC_USER_DATA) + uDataSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pSyncUserData = (APEX_SYNC_USER_DATA*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSyncUserData);

    pSyncUserData->cMsgId   = 'T';
    pSyncUserData->nSendId  = dwRoleID;
    memcpy(pSyncUserData->byData, pbyData, uDataSize);

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoEntryServer(DWORD dwRoleID, int nGSIndex)
{
    BOOL                bResult         = false;
    APEX_SWITCH_SERVER* pSwitchServer   = NULL;
    IKG_Buffer*         piBuffer        = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_SWITCH_SERVER));
    KGLOG_PROCESS_ERROR(piBuffer);

    pSwitchServer = (APEX_SWITCH_SERVER*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSwitchServer);

    pSwitchServer->cMsgId       = 'H';
    pSwitchServer->nSendId      = dwRoleID;
    pSwitchServer->nGS_Index    = nGSIndex;
    pSwitchServer->nGS_State    = 0;

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KApexProxy::DoLeaveServer(DWORD dwRoleID, int nGSIndex)
{
    BOOL                bResult         = false;
    APEX_SWITCH_SERVER* pSwitchServer   = NULL;
    IKG_Buffer*         piBuffer        = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(APEX_SWITCH_SERVER));
    KGLOG_PROCESS_ERROR(piBuffer);

    pSwitchServer = (APEX_SWITCH_SERVER*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSwitchServer);

    pSwitchServer->cMsgId       = 'H';
    pSwitchServer->nSendId      = dwRoleID;
    pSwitchServer->nGS_Index    = nGSIndex;
    pSwitchServer->nGS_State    = 1;

    Send(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

//////////////////////////////////////////////////////////////////////////

void KApexProxy::ProcessRecvData(BYTE* pbyData, size_t uDataSize)
{
    APEX_PROTOCOL_HEADER* pProtocolHeader = (APEX_PROTOCOL_HEADER*)pbyData;

    switch (pProtocolHeader->cMsgId)
    {
    case 'D':
        OnRecvUserData(pbyData, uDataSize);
        break;

    case 'K':
        OnKickRole(pbyData, uDataSize);
        break;

    default:
        KGLogPrintf(KGLOG_ERR, "[ApexProxy] Message Type Error.");
        goto Exit0;
    }

Exit0:
    return;
}

void KApexProxy::OnRecvUserData(BYTE* pbyData, size_t uDataSize)
{
    APEX_SYNC_USER_DATA* pSyncUserData = (APEX_SYNC_USER_DATA*)pbyData;
    size_t               nSize         = uDataSize - sizeof(APEX_SYNC_USER_DATA);

    KGLOG_PROCESS_ERROR(uDataSize >= sizeof(APEX_SYNC_USER_DATA));

    g_pSO3GameCenter->m_GameServer.DoApexProtocol(pSyncUserData->nSendId, pSyncUserData->byData, nSize);

Exit0:
    return;
}

void KApexProxy::OnKickRole(BYTE* pbyData, size_t uDataSize)
{
    BOOL                    bRetCode          = false;
    APEX_KICK_ROLE*         pKickRole         = (APEX_KICK_ROLE*)pbyData;
    KRole*                  pRole             = NULL;
    BOOL                    bImmediatelyKick  = false;
    KICK_TABLE::iterator    itFind;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pKickRole->nSendId);
    KGLOG_PROCESS_ERROR(pRole);

    bRetCode = pRole->IsOnline();
    KG_PROCESS_ERROR(bRetCode);

    switch (pKickRole->nActionType)
    {
    case KICK_ACTION_ID_NO_DLL:
    case KICK_ACTION_ID_LOAD_ERROR:
        bImmediatelyKick = true;
        break;
    default:
        break;
    }

    if (bImmediatelyKick)
    {
        goto Exit1;
    }

    itFind = m_KickTable.find(pRole->m_dwPlayerID);
    if (itFind == m_KickTable.end())
    {
        KROLE_KICK_NODE KickNode;

        KickNode.nLastRefreshTime = g_pSO3GameCenter->m_nTimeNow;
        KickNode.nKickCount       = 1;

        m_KickTable.insert(make_pair(pRole->m_dwPlayerID, KickNode));

        itFind = m_KickTable.find(pRole->m_dwPlayerID);
    }

    KGLOG_PROCESS_ERROR(itFind != m_KickTable.end());

    if (itFind->second.nKickCount < g_pSO3GameCenter->m_Settings.m_nMaxKickCount)
    {
        itFind->second.nLastRefreshTime = g_pSO3GameCenter->m_nTimeNow;
        itFind->second.nKickCount++;
        goto Exit0;
    }

Exit1:
    KGLogPrintf(KGLOG_INFO, "[ApexProxy] Try Kickout Player:[%s(%lu)].", pRole->m_szName, pRole->m_dwPlayerID);

    g_pSO3GameCenter->m_LogServerAgency.LogApexKickRole(pRole, pKickRole->nActionType);

    g_pSO3GameCenter->m_GameServer.DoKickAccountNotify(pRole->m_dwPlayerID);

    DoRoleLogout(pRole->m_dwPlayerID);

Exit0:
    return;
}

void KApexProxy::ClearTimeoutKickNode()
{
    for (
        KICK_TABLE::iterator it = m_KickTable.begin();
        it != m_KickTable.end(); NULL
    )
    {
        if (g_pSO3GameCenter->m_nTimeNow - it->second.nLastRefreshTime > KICK_REFRESH_TIME)
        {
            m_KickTable.erase(it++);
            continue;
        }
        ++it;
    }
}
