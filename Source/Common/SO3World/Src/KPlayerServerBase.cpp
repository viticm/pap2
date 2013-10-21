#include "stdafx.h"
#include "KPlayerServer.h"
#include "Engine/KG_Time.h"
#if defined(_SERVER)
#include "Engine/KG_CreateGUID.h"
#include "Common/CRC32.h"
#include "Common/KG_Socket.h"
#include "Common/KG_Package.h"
#include "KPlayer.h"
#include "KRelayClient.h"
#include "KRegion.h"
#include "KMath.h"
#include "KSO3World.h"
#include "Engine/inoutmac.h"

#define KG_MAX_ACCEPT_EACH_WAIT     8

int KPlayerServer::_Construct()
{
	m_nMaxConnection            = 0;
    m_nMaxClientPackPerFrame    = 0;
	m_ConnectionDataList        = NULL;

    m_nSocketEventCount         = 0;
    m_pSocketEventArray         = NULL;

    m_piSocketServer            = NULL;

    m_uAddUpFlux                = 0;
    m_uSecondFlux               = 0;
    m_dwNextFluxUpdateTime      = 0;

    m_nListenPort               = 0;
    m_dwInternetAddr            = 0;
    m_szLocalIP[0]              = '\0';

    m_nPingCycle                = 0;
    m_nClientGM                 = 0;
    m_nNextCheckPingConnection  = 0;
    m_dwTimeNow                 = 0;

    return true;
}

int KPlayerServer::_Destroy()
{
    assert(!m_piSocketServer);
	assert(!m_pSocketEventArray);
    assert(!m_ConnectionDataList); 
    
    return true;
}

// 初始化面向客户端的连接
BOOL KPlayerServer::Init(IRecorderFactory* piFactory)
{
    BOOL            bResult                 = false;
    int             nRetCode                = false;
    BOOL            bSocketServerInit       = false;
    IIniFile*	    piIniFile               = NULL;
    unsigned long   ulInternalIPAddressMask = 0;
    unsigned long   ulExternalIPAddress     = INADDR_NONE;
    unsigned long   ulInternalIPAddress     = INADDR_NONE;
    int				nListenPort             = 0;
    int 			nMaxConnection          = 0;
    int             nRecvBufferSize         = 0;
    int             nSendBufferSize         = 0;
    int             nClientGM               = 0;
    int             nMaxClientPackPerFrame  = 0;
    char            szInternalIPMask[16];
    unsigned char   byInternalMacAddress[6];
    unsigned char   byExternalMacAddress[6];
    struct          in_addr  InAddr;
    
    assert(piFactory);
    assert(g_pSO3World);
    assert(g_pSO3World->m_piRecorderTools);
    
    m_piSocketServer = piFactory->CreateSocketServer();
    KGLOG_PROCESS_ERROR(m_piSocketServer);

    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);

    memset(m_szLocalIP, 0, sizeof(m_szLocalIP));

    piIniFile->GetString("Player", "IP", "", m_szLocalIP, sizeof(m_szLocalIP));
    piIniFile->GetInteger("Player", "Port", 8111, &nListenPort);
    piIniFile->GetString("Player", "InternalIPAddressMask", "192.168.0.0", szInternalIPMask, 16);
	piIniFile->GetInteger("Player", "MaxConnection", 1024, &nMaxConnection);
	piIniFile->GetInteger("Player", "RecvBufferSize", 8000, &nRecvBufferSize);
	piIniFile->GetInteger("Player", "SendBufferSize", 64000, &nSendBufferSize); 
    piIniFile->GetInteger("Player", "PingCycle", 0, &m_nPingCycle);
    piIniFile->GetInteger("Player", "ClientGM", 0, &nClientGM);
    piIniFile->GetInteger("Player", "MaxClientPackPerFrame", 256, &nMaxClientPackPerFrame);

    if (m_szLocalIP[0] != '\0')
    {
         ulExternalIPAddress = (unsigned long)inet_addr(m_szLocalIP);
    }
    else
    {
        char* pszInternetAddr = NULL;

        ulInternalIPAddressMask = inet_addr(szInternalIPMask);
        KGLOG_PROCESS_ERROR(ulInternalIPAddressMask != INADDR_NONE);

        nRetCode = gGetMacAndIPAddress(
            byInternalMacAddress, &ulInternalIPAddress, 
            byExternalMacAddress, &ulExternalIPAddress, ulInternalIPAddressMask
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        InAddr.s_addr = ulExternalIPAddress;
        pszInternetAddr = inet_ntoa(InAddr);
        KGLOG_PROCESS_ERROR(pszInternetAddr);

        strcpy(m_szLocalIP, pszInternetAddr);
    }
    
    KGLOG_PROCESS_ERROR(nMaxConnection > 0);
    
    m_dwTimeNow = g_pSO3World->m_piRecorderTools->GetTickCount();
    m_nClientGM = nClientGM;

    memset(m_DownwardProtocolStatTable, 0, sizeof(m_DownwardProtocolStatTable));

	m_nMaxConnection = nMaxConnection;
    m_nMaxClientPackPerFrame = nMaxClientPackPerFrame;

	m_ConnectionDataList = new KConnectionData[nMaxConnection];
	KGLOG_PROCESS_ERROR(m_ConnectionDataList);

    m_ConnectionDataListFreeVector.reserve(nMaxConnection);

    for (int i = 0; i < nMaxConnection; i++)
    {
        m_ConnectionDataListFreeVector.push_back(nMaxConnection - i - 1);
    }

    m_nSocketEventCount = nMaxConnection + KG_MAX_ACCEPT_EACH_WAIT;

    m_pSocketEventArray = new KG_SOCKET_EVENT[m_nSocketEventCount];
    KGLOG_PROCESS_ERROR(m_pSocketEventArray);

    nRetCode = m_piSocketServer->Init(
        m_szLocalIP, nListenPort, KG_MAX_ACCEPT_EACH_WAIT, 
        nRecvBufferSize, nSendBufferSize, KSG_ENCODE_DECODE, NULL
    );

    KGLogPrintf(
        KGLOG_INFO, "Start service at %s:%d ... ... [%s]",
        m_szLocalIP, nListenPort, nRetCode ? "OK" : "Failed"
    );
    
    KGLOG_PROCESS_ERROR(nRetCode);
    bSocketServerInit = true;

	m_nListenPort       = nListenPort;
	m_dwInternetAddr    = ulExternalIPAddress;

    m_nNextCheckPingConnection = 0;

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bSocketServerInit)
        {
            m_piSocketServer->UnInit(NULL);
            bSocketServerInit = false;
        }
        KG_DELETE_ARRAY(m_pSocketEventArray);
    	KG_DELETE_ARRAY(m_ConnectionDataList);
        KG_COM_RELEASE(m_piSocketServer);
    }
    KG_COM_RELEASE(piIniFile);
	return bResult;
}

BOOL KPlayerServer::UnInit(void)
{
    for (int nConnIndex = 0; nConnIndex < m_nMaxConnection; nConnIndex++)
    {
        if (m_ConnectionDataList[nConnIndex].piSocketStream)
        {
            Shutdown(nConnIndex);
        }
    }

    DumpPakckageStat();
    
    if (m_piSocketServer)
    {
        m_piSocketServer->UnInit(NULL);
    }
    KG_COM_RELEASE(m_piSocketServer);

    KG_DELETE_ARRAY(m_pSocketEventArray);
    m_nSocketEventCount = 0;

    KG_DELETE_ARRAY(m_ConnectionDataList);
    m_nMaxConnection = 0;

	return true;
}

void KPlayerServer::Breathe(void)
{
	int             nRetCode    = false;

    assert(m_piSocketServer);
    assert(g_pSO3World && g_pSO3World->m_piRecorderTools);

    m_dwTimeNow   = g_pSO3World->m_piRecorderTools->GetTickCount();

    if (m_dwTimeNow > m_dwNextFluxUpdateTime)
    {
        m_uSecondFlux = m_uAddUpFlux;
        m_uAddUpFlux  = 0;

        m_dwNextFluxUpdateTime = m_dwTimeNow + 1000;
    }

    if (m_ConnectionDataList[m_nNextCheckPingConnection].piSocketStream)
    {
        int nIdleTime = m_dwTimeNow - m_ConnectionDataList[m_nNextCheckPingConnection].dwLastPingTime;

        if (nIdleTime > m_nPingCycle * 1000)
        {
            KGLogPrintf(KGLOG_ERR, "Connection timeout: %d\n", m_nNextCheckPingConnection);

            Detach(m_nNextCheckPingConnection);
            Shutdown(m_nNextCheckPingConnection);

            g_pSO3World->m_StatDataServer.UpdateClientTimeout();
        }
    }

    m_nNextCheckPingConnection = (m_nNextCheckPingConnection + 1) % m_nMaxConnection;

    while (true)
    {
        int                 nEventCount     = 0;
        KG_SOCKET_EVENT*    pSocketEvent    = NULL;
        KG_SOCKET_EVENT*    pSocketEventEnd = NULL;

        nRetCode = m_piSocketServer->Wait(m_nSocketEventCount, m_pSocketEventArray, &nEventCount);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (nEventCount == 0)
            break;

        pSocketEventEnd = m_pSocketEventArray + nEventCount;
        for (pSocketEvent = m_pSocketEventArray; pSocketEvent < pSocketEventEnd; pSocketEvent++)
        {
            // Process new connection accept
            // KG_SOCKET_EVENT_ACCEPT event must be processed before any other event, 
            // because Maybe uEventFlag = KG_SOCKET_EVENT_ACCEPT | KG_SOCKET_EVENT_IN.
            if (pSocketEvent->uEventFlag & KG_SOCKET_EVENT_ACCEPT)
            {
                ProcessNewConnection(pSocketEvent->piSocket);
                KG_COM_RELEASE(pSocketEvent->piSocket);
                continue;
            }

            if (!(pSocketEvent->uEventFlag & KG_SOCKET_EVENT_IN))
            {
                KGLogPrintf(KGLOG_DEBUG, "Unexpected socket event: %u", pSocketEvent->uEventFlag);
                KG_COM_RELEASE(pSocketEvent->piSocket);
                continue;
            }

            ProcessPackage(pSocketEvent->piSocket);

            KG_COM_RELEASE(pSocketEvent->piSocket);
        }
	}

Exit0:
    return;
}

BOOL KPlayerServer::Attach(KPlayer* pPlayer, int nConnIndex)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    struct in_addr          RemoteAddr      = { 0 };
    u_short                 wRemotePortNet  = 0;
    char*                   pszRetString    = NULL;

    assert(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);
    assert(m_ConnectionDataList[nConnIndex].piSocketStream);
    assert(m_ConnectionDataList[nConnIndex].pPlayer == NULL);

    assert(pPlayer->m_nConnIndex == -1);
    assert(pPlayer->m_eGameStatus == gsWaitForConnect);

	m_ConnectionDataList[nConnIndex].pPlayer = pPlayer;

	pPlayer->m_nConnIndex   = nConnIndex;

    m_ConnectionDataList[nConnIndex].piSocketStream->GetRemoteAddress(&RemoteAddr, &wRemotePortNet);

    pszRetString = inet_ntoa(RemoteAddr);
    KGLOG_PROCESS_ERROR(pszRetString);

    strncpy(pPlayer->m_szClientIP, pszRetString, sizeof(pPlayer->m_szClientIP));
    pPlayer->m_szClientIP[sizeof(pPlayer->m_szClientIP) - 1] = '\0';

    KGLogPrintf(
        KGLOG_INFO, "Bind %s:(%s, %u) to connection(%d)\n", 
        pPlayer->m_szAccount, pPlayer->m_szName, pPlayer->m_dwID, nConnIndex
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::Detach(int nConnIndex)
{
    BOOL        bResult     = false;
    KPlayer*    pPlayer     = NULL;

    assert(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);

    pPlayer = m_ConnectionDataList[nConnIndex].pPlayer;
    KG_PROCESS_ERROR(pPlayer);

	m_ConnectionDataList[nConnIndex].pPlayer = NULL;

    g_LogClient.LogPlayerLogout(pPlayer->m_szAccount, pPlayer->m_szName);

	pPlayer->m_nConnIndex       = -1;
    pPlayer->m_MoveCtrl.bMove   = false;
    pPlayer->m_MoveCtrl.nTurn   = 0;

	switch (pPlayer->m_eGameStatus)
	{
	case gsPlaying:
        pPlayer->m_nTimer      = 0;
		pPlayer->m_eGameStatus = gsDeleting;
		break;

	case gsWaitForPermit:
	case gsWaitForRoleData:
	case gsWaitForLoginLoading:
    case gsWaitForNewMapLoading:
		g_pSO3World->DelPlayer(pPlayer);
		break;

	case gsSearchMap:
		g_pSO3World->DelPlayer(pPlayer);
		break;					

	case gsWaitForTransmissionSave:
    case gsWaitForTransmissionGuid:
		// 已经存盘,直接删除
		g_pSO3World->DelPlayer(pPlayer);
		break;

	default:
        KGLogPrintf(
            KGLOG_ERR, "Unexpected GameStatus %d for player %s when connection close !",
            pPlayer->m_eGameStatus, pPlayer->m_szName
        );
        break;
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::Send(int nConnIndex, void* pvData, size_t uDataLen)
{
    int                         nResult     = false;
    int                         nRetCode    = false;
    EXTERNAL_PROTOCOL_HEADER*   pPakHeader  = (EXTERNAL_PROTOCOL_HEADER*)pvData;
    KConnectionData*            pConnection = NULL;

    assert(pPakHeader->byProtocolID > gs_client_connection_begin);
    assert(pPakHeader->byProtocolID < gs_client_connection_end);

    KG_PROCESS_ERROR(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);

    pConnection = &m_ConnectionDataList[nConnIndex];
    KG_PROCESS_ERROR(pConnection->piSocketStream);

    m_DownwardProtocolStatTable[pPakHeader->byProtocolID].dwPackCount++;
    m_DownwardProtocolStatTable[pPakHeader->byProtocolID].nTotalSize += uDataLen;

    m_uAddUpFlux += uDataLen;

    nRetCode = pConnection->SocketPacker.Send(
        pConnection->piSocketStream, (unsigned)uDataLen, (const unsigned char*)pvData
    );
    KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KPlayerServer::FlushSend()
{
    for (int i = 0; i < m_nMaxConnection; i++)
    {
        if (m_ConnectionDataList[i].piSocketStream)
        {
            m_ConnectionDataList[i].SocketPacker.FlushSend(m_ConnectionDataList[i].piSocketStream);
        }
    }
}

BOOL KPlayerServer::Shutdown(int nConnIndex)
{
    assert(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);

    assert(m_ConnectionDataList[nConnIndex].piSocketStream);

    m_ConnectionDataList[nConnIndex].piSocketStream->SetUserData((void*)(ptrdiff_t)-1);

    KG_COM_RELEASE(m_ConnectionDataList[nConnIndex].piSocketStream);

    m_ConnectionDataListFreeVector.push_back(nConnIndex);

    KGLogPrintf(KGLOG_INFO, "Shutdown connection: %d\n", nConnIndex);

    return true;
}

BOOL KPlayerServer::ProcessNewConnection(IKG_SocketStream* piSocket)
{
    BOOL            bResult         = false;
    int             nRetCode        = 0;
    int             nConnIndex      = -1;
    u_short         uRemotePort     = 0;
    const char*     pcszIP          = NULL;
    struct in_addr  RemoteIP;

    assert(piSocket);

    nRetCode = m_ConnectionDataListFreeVector.empty();
    KG_PROCESS_ERROR(!nRetCode);

    nRetCode = piSocket->GetRemoteAddress(&RemoteIP, &uRemotePort);
    KGLOG_PROCESS_ERROR(nRetCode);

    pcszIP = inet_ntoa(RemoteIP);
    KGLOG_PROCESS_ERROR(pcszIP);

    nConnIndex = m_ConnectionDataListFreeVector.back();
    m_ConnectionDataListFreeVector.pop_back();

    assert(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);

    nRetCode = piSocket->SetUserData((void*)(ptrdiff_t)nConnIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_ConnectionDataList[nConnIndex].piSocketStream = piSocket;
    m_ConnectionDataList[nConnIndex].piSocketStream->AddRef();

    m_ConnectionDataList[nConnIndex].SocketPacker.Reset();
    m_ConnectionDataList[nConnIndex].dwLastPingTime = m_dwTimeNow;
    m_ConnectionDataList[nConnIndex].pPlayer        = NULL;

    KGLogPrintf(KGLOG_INFO, "New connection from %s:%u, index = %d\n", pcszIP, uRemotePort, nConnIndex);

    bResult = true;
Exit0:
    if (!bResult)
    {
        piSocket->SetUserData((void *)(ptrdiff_t)-1);
    }
    return bResult;
}

BOOL KPlayerServer::ProcessPackage(IKG_SocketStream* piSocket)
{
    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    int                 nConnIndex      = -1;
    IKG_Buffer*         piBuffer        = NULL;
    BYTE*               pbyData         = NULL;
    unsigned            uDataLen        = 0;
    int                 nPackCount      = 0;
    BOOL                bShutDownFlag   = false;

    assert(piSocket);

    nConnIndex = (int)(ptrdiff_t)(piSocket->GetUserData());
    KGLOG_PROCESS_ERROR(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);

    while (true)
    {
        KG_COM_RELEASE(piBuffer);

        if (nPackCount++ >= m_nMaxClientPackPerFrame)
        {
            KGLogPrintf(KGLOG_INFO, "Client send too many packs, connection: %d\n", nConnIndex);

            bShutDownFlag = true;
            break;
        }

        nRetCode = piSocket->Recv(&piBuffer);
        if (nRetCode == -2)
        {
            break;
        }

        if (nRetCode == -1)
        {
            KPlayer* pPlayer = GetPlayerByConnection(nConnIndex);

            if (pPlayer && pPlayer->m_eGameStatus == gsPlaying)
            {
                // 如果是玩家登出的,断线时状态就不可能是gsPlaying
                // 所以这里一定是意外断线
                g_pSO3World->m_StatDataServer.UpdateClientErrorLogout();

                g_RelayClient.DoMapCopyKeepPlayer(pPlayer);
            }

            KGLogPrintf(KGLOG_INFO, "Connection lost: %d\n", nConnIndex);

            bShutDownFlag = true;
            break;
        }

        KGLOG_PROCESS_ERROR(piBuffer);

        pbyData   = (BYTE*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        uDataLen = piBuffer->GetSize();

        nRetCode  = CheckPackage(pbyData, uDataLen);
        if (!nRetCode)
        {
		    KGLogPrintf(KGLOG_INFO, "Pak error, connection: %d\n", nConnIndex);

            bShutDownFlag = true;
            break;
        }

        KPlayer*                 pPlayer = GetPlayerByConnection(nConnIndex);
	    UPWARDS_PROTOCOL_HEADER* pHeader = (UPWARDS_PROTOCOL_HEADER*)pbyData;

        m_ConnectionDataList[nConnIndex].dwLastPingTime = m_dwTimeNow;

        if (pPlayer)
        {
            // 帧数不可能比前面的还低, 帧数不可能超过服务端一定范围(客户端帧平衡算法决定的)
            if (
                (pHeader->nFrame < pPlayer->m_nLastClientFrame) || 
                (pHeader->nFrame > g_pSO3World->m_nGameLoop + GAME_FPS / 2)
            )
            {
	            KGLogPrintf(
                    KGLOG_ERR, "Frame error(%d): last frame = %d, pak frame = %d , server frame = %d\n", 
		            nConnIndex, pPlayer->m_nLastClientFrame, pHeader->nFrame, g_pSO3World->m_nGameLoop
                );

                bShutDownFlag = true;
                break;
            }

			if (pPlayer->m_eGameStatus != gsPlaying)
			{
				if (pHeader->byProtocolID != c2s_apply_enter_scene && pHeader->byProtocolID != c2s_ping_signal)
                {
					continue;
				}
			}
        }
        else
        {
            // 玩家指针为空,这时候只允许极少数几个协议上来
            if (pHeader->byProtocolID != c2s_handshake_request && pHeader->byProtocolID != c2s_ping_signal)
            {
                continue;
            }
        }

	    (this->*m_ProcessProtocolFuns[pHeader->byProtocolID])(
            (char*)pbyData, (int)uDataLen, nConnIndex, pHeader->nFrame
        );

        if (pPlayer)
        {
            pPlayer->m_nLastClientFrame = pHeader->nFrame;
        }
    }

    bResult = true;
Exit0:
    if (bShutDownFlag)
    {
        Detach(nConnIndex);
        Shutdown(nConnIndex);
        bShutDownFlag = false;
    }
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KPlayerServer::DumpPakckageStat()
{
    BOOL  bResult   = false;
    FILE* pFile     = NULL;

    pFile = fopen("PackageStat.tab", "w");
    KG_PROCESS_ERROR(pFile);

    fprintf(pFile, "Protocol\tPackageCount\tTotalSize\n");

    for (int nProtocol = gs_client_connection_begin + 1; nProtocol < gs_client_connection_end; nProtocol++)
    {
        KPROTOCOL_STAT_INFO* pStatInfo = m_DownwardProtocolStatTable + nProtocol;

        fprintf(pFile, "%d\t%lu\t%lld\n", nProtocol, pStatInfo->dwPackCount, pStatInfo->nTotalSize);
    }

    bResult = true;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return bResult;
}

BOOL KPlayerServer::CheckPackage(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult = false;
	UPWARDS_PROTOCOL_HEADER*    pHeader = (UPWARDS_PROTOCOL_HEADER*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(UPWARDS_PROTOCOL_HEADER));

    KGLOG_PROCESS_ERROR(pHeader->byProtocolID > client_gs_connection_begin);

    KGLOG_PROCESS_ERROR(pHeader->byProtocolID < client_gs_connection_end);

	if (m_nProtocolSize[pHeader->byProtocolID] == UNDEFINED_PROTOCOL_SIZE)
	{
        KGLOG_PROCESS_ERROR(uDataLen >= sizeof(UNDEFINED_SIZE_UPWARDS_HEADER));
	}
    else
    {
        KGLOG_PROCESS_ERROR(uDataLen == (size_t)m_nProtocolSize[pHeader->byProtocolID]);
    }

    KGLOG_PROCESS_ERROR(m_ProcessProtocolFuns[pHeader->byProtocolID]);

    bResult = true;
Exit0:
	return bResult;
}

void KPlayerServer::TeamBroadcast(DWORD dwTeamID, void* pvData, size_t uSize)
{
    BOOL           bRetCode = false;
    KTeamBroadcast Func;

    Func.m_dwTeamID = dwTeamID;
    Func.m_pvData   = pvData;
    Func.m_uSize    = uSize;

    bRetCode = g_pSO3World->m_TeamServer.Traverse(Func);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

BOOL KPlayerServer::TeamBroadcastTalk(DWORD dwTeamID, DWORD dwTalkerID, void* pvData, size_t uSize)
{
    BOOL        bResult         = false;
    int         nTeamGroupIndex = 0;
    KTeam*      pTeam           = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    nTeamGroupIndex = g_pSO3World->m_TeamServer.GetMemberGroupIndex(dwTeamID, dwTalkerID);
    KGLOG_PROCESS_ERROR(nTeamGroupIndex >= 0);
    KGLOG_PROCESS_ERROR(nTeamGroupIndex < pTeam->nGroupNum);

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nTeamGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nTeamGroupIndex].MemberList.end(); ++it
    )
    {
        KPlayer*        pMember     = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
        KGBlackNode*    pBlackNode  = NULL;

        if (!pMember)
            continue;

        pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(pMember->m_dwID, dwTalkerID);
        if (pBlackNode)
            continue;

        Send(pMember->m_nConnIndex, pvData, uSize);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::RaidBroadcastTalk(DWORD dwTeamID, DWORD dwTalkerID, void* pvData, size_t uSize)
{
    BOOL    bResult = false;
    KTeam*  pTeam   = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer*        pMember     = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
            KGBlackNode*    pBlackNode  = NULL;

            if (!pMember)
                continue;

            pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(pMember->m_dwID, dwTalkerID);
            if (pBlackNode)
                continue;

            Send(pMember->m_nConnIndex, pvData, uSize);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

KBroadcastFunc::KBroadcastFunc()
{
    m_pvData            = NULL;
    m_uSize             = 0;
    m_nMaxSyncCount     = MAX_SYNC_PER_REGION;
    m_dwExcept          = ERROR_ID;
    m_pCenterObject     = NULL;
    m_nRange            = 0;
    m_dwExclusive       = ERROR_ID;
}

BOOL KBroadcastFunc::operator()(KPlayer* pPlayer)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(pPlayer);

    KG_PROCESS_SUCCESS(pPlayer->m_dwID == m_dwExcept);
	KG_PROCESS_SUCCESS(pPlayer->m_nConnIndex == -1);

    if (m_dwExclusive != ERROR_ID && pPlayer->m_dwID != m_dwExclusive)
    {
        goto Exit1;
    }

    if (m_nRange > 0 && m_pCenterObject)
    {
        bRetCode = g_InRange(m_pCenterObject, pPlayer, m_nRange);
        KG_PROCESS_SUCCESS(!bRetCode);
    }

	KG_PROCESS_ERROR(m_nMaxSyncCount > 0);
	m_nMaxSyncCount--;

	g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uSize);

Exit1:
    bResult = true;
Exit0:
	return bResult;
}


KBroadcastBeHitFunc::KBroadcastBeHitFunc()
{
    m_pvData        = NULL;
    m_uSize         = 0;
    m_nMaxSyncCount = 0;
    m_pExceptSrc    = NULL;
    m_pExceptDst    = NULL;
}

BOOL KBroadcastBeHitFunc::operator()(KPlayer* pPlayer)
{
    BOOL    bResult    = false;
	BOOL    bRetCode   = false;
    DWORD   dwTeamID   = ERROR_ID;

    assert(pPlayer);

    dwTeamID = pPlayer->GetCurrentTeamID();

    if (m_pExceptSrc)
    {
        DWORD dwExceptSrcTeamID =  m_pExceptSrc->GetCurrentTeamID();

        KG_PROCESS_SUCCESS(m_pExceptSrc == pPlayer);

        KG_PROCESS_SUCCESS(
            dwTeamID != ERROR_ID && 
            dwTeamID == dwExceptSrcTeamID
        );
    }

    if (m_pExceptDst)
    {
        DWORD dwExceptDstTeamID =  m_pExceptDst->GetCurrentTeamID();

        KG_PROCESS_SUCCESS(m_pExceptDst == pPlayer);

        KG_PROCESS_SUCCESS(
            dwTeamID != ERROR_ID &&
            dwTeamID == dwExceptDstTeamID
        );
    }

	KG_PROCESS_ERROR(m_nMaxSyncCount > 0);
	m_nMaxSyncCount--;

    g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uSize);

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

KBroadcastTalkFunc::KBroadcastTalkFunc()
{
    m_pvData        = NULL;
    m_uSize         = 0;
    m_nMaxSyncCount = 0;
    m_dwTalkerID    = ERROR_ID;
    m_pCenterObject = NULL;
    m_nRange        = 0;
    m_dwExclusive   = ERROR_ID;
}

BOOL KBroadcastTalkFunc::operator()(KPlayer* pPlayer)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KGBlackNode*    pBlackNode  = NULL;

    assert(pPlayer);

    if (m_dwExclusive != ERROR_ID && pPlayer->m_dwID != m_dwExclusive)
    {
        goto Exit1;
    }

    if (m_dwTalkerID)
    {
        pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(pPlayer->m_dwID, m_dwTalkerID);
        KG_PROCESS_SUCCESS(pBlackNode);
    }

    if (m_nRange > 0 && m_pCenterObject)
    {
        bRetCode = g_InRange(m_pCenterObject, pPlayer, m_nRange);
        KG_PROCESS_SUCCESS(!bRetCode);
    }

    KG_PROCESS_ERROR(m_nMaxSyncCount > 0);
    m_nMaxSyncCount--;

    g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uSize);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KBroadcastTargetFunc::operator ()(KPlayer* pPlayer)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    TARGET_TYPE     eTargetType     = ttInvalid;
    DWORD           dwTargetID      = 0;

    assert(pPlayer);
    KGLOG_PROCESS_ERROR(m_pSrcCharacter);

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
    if (eTargetType == ttNpc || eTargetType == ttPlayer)
    {
        dwTargetID = pPlayer->m_SelectTarget.GetTargetID();
    }

    if (m_pSrcCharacter->m_dwID == dwTargetID)
    {
        g_PlayerServer.DoSetTarget(pPlayer, m_pSrcCharacter);
    }

    bResult = true;
Exit0:
    return bResult;
}

KTeamBroadcast::KTeamBroadcast()
{
    m_dwTeamID      = ERROR_ID;
    m_dwExceptID    = ERROR_ID;
    m_pvData        = NULL;
    m_uSize         = 0;
}

BOOL KTeamBroadcast::operator () (KTEAM_MEMBER_INFO* pMemberInfo)
{
    KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(pMemberInfo->dwMemberID);
    if (!pMember)
        return true;

    g_PlayerServer.Send(pMember->m_nConnIndex, m_pvData, m_uSize);

    return true;
}

#endif	//_SERVER
