#include "StdAfx.h"
#include "KGatewaySimulator.h"
#include "TestDef.h"
#include "KGameCenterWrapper.h"
#include <time.h>



#define  GAME_WORLD_CURRENT_VERSION 20
#define  GAME_WORLD_LOWEST_VERSION  20
#define  MAXCOUNT 100

KGatewaySimulator::KGatewaySimulator(void)
{
}

KGatewaySimulator::~KGatewaySimulator(void)
{
	UnInit();
}

KGatewaySimulator* KGatewaySimulator::Instance()
{
	static KGatewaySimulator gt;
	return &gt;
}

BOOL KGatewaySimulator::CheckSetGameWorldVersion(int nLowerVersion, int nUpperVersion)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_SET_GAME_WORLD_VERSION* pSetGameWorldVersion = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_set_game_world_version);
    KG_PROCESS_ERROR(piRecvPackage);
    
    pSetGameWorldVersion = (R2G_SET_GAME_WORLD_VERSION*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pSetGameWorldVersion);

    KG_PROCESS_ERROR(pSetGameWorldVersion->nLowerVersion == nLowerVersion);
    KG_PROCESS_ERROR(pSetGameWorldVersion->nUpperVersion == nUpperVersion);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}

BOOL KGatewaySimulator::CheckSyncRoleList(int nRoleIndex, int nRoleCount, KROLE_LIST_INFO* pBaseData)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_SYNC_ROLE_LIST* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_sync_role_list);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_SYNC_ROLE_LIST*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);

    KG_PROCESS_ERROR(pRespond->nRoleIndex == nRoleIndex);
    KG_PROCESS_ERROR(pRespond->nRoleCount == nRoleCount);

    if (pRespond->byData && pBaseData)
    {
        memcpy(pBaseData, pRespond->byData, sizeof(KROLE_LIST_INFO));
    }

    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}

BOOL KGatewaySimulator::CheckCreateRoleRespond(KCREATE_ROLE_RESPOND_CODE eCode, KROLE_LIST_INFO* pBaseData)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_CREATE_ROLE_RESPOND* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_create_role_respond);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_CREATE_ROLE_RESPOND*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);
	
	if (pRespond->byCode != eCode)
	{
		KG_PROCESS_ERROR("except %d actual %d\n", pRespond->byCode, eCode);
		goto Exit0;
	}
    KG_PROCESS_ERROR(pRespond->byCode == eCode);
    //KG_PROCESS_ERROR(pRespond->dwRoleID == dwRoleID);
 
    if ((pRespond->byCode == eCreateRoleSucceed) && pBaseData)
    {
        memcpy(pBaseData, pRespond->byData, sizeof(KROLE_LIST_INFO));

    }

    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}


BOOL KGatewaySimulator::CheckDeleteRoleRespond(DWORD dwRoleID, BOOL bSucceed)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_DELETE_ROLE_RESPOND* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_delete_role_respond);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_DELETE_ROLE_RESPOND*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);

    KG_PROCESS_ERROR(pRespond->byCode == bSucceed);
    KG_PROCESS_ERROR(pRespond->dwRoleID == dwRoleID);


  nResult = true;
Exit0:
  KG_COM_RELEASE(piRecvPackage);
     return nResult;
}


BOOL KGatewaySimulator::CheckReplyLoginData(DWORD dwRoleID, int nRespondCode)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_GAME_LOGIN_RESPOND* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_game_login_respond);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_GAME_LOGIN_RESPOND*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);

    KG_PROCESS_ERROR(pRespond->byCode == nRespondCode);
    KG_PROCESS_ERROR(pRespond->dwRoleID == dwRoleID);
    
    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}


bool KGatewaySimulator::DoPingSignal()
{
    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_PING_SIGNAL*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_PING_SIGNAL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_PING_SIGNAL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_ping_signal;
    pSend->dwTime        = (DWORD)time(NULL);

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DoQueryAccountState(char szAccount[])
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_QUERY_ACCOUNT_STATE*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_QUERY_ACCOUNT_STATE));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_QUERY_ACCOUNT_STATE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_query_account_state;
    strncpy(pSend->szAccount, szAccount, sizeof(pSend->szAccount));

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DonQueryRoleList(char szAccount[])
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_QUERY_ROLE_LIST*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_QUERY_ROLE_LIST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_QUERY_ROLE_LIST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_query_role_list;
    strncpy(pSend->szAccount, szAccount, sizeof(pSend->szAccount));

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DoQueryNewbieMaps()
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL; 
    G2R_QUERY_NEWBIE_MAPS*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_QUERY_NEWBIE_MAPS));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_QUERY_NEWBIE_MAPS*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_query_newbie_maps;
    //pSend->nPlayerIndex = nPlayerIndex;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DoKickAccountNotify(char szAccount[])
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_KICK_ACCOUNT_NOTIFY*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_KICK_ACCOUNT_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_KICK_ACCOUNT_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_kick_account_notify;
    strncpy(pSend->szAccount, szAccount, sizeof(pSend->szAccount));

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DoCreateRoleRequest(char szAccount[], KUSER_ROLE_GENERATOR_PARAM& stRoleGenParam)
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_CREATE_ROLE_REQUEST*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_CREATE_ROLE_REQUEST) + sizeof(KUSER_ROLE_GENERATOR_PARAM));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_CREATE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_create_role_request;
    //pSend->nPlayerIndex = nPlayerIndex;
    strncpy(pSend->szAccount, szAccount, sizeof(pSend->szAccount));
    memcpy(pSend->byData, &stRoleGenParam, sizeof(stRoleGenParam));

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DoDeleteRoleRequest(DWORD   dwRoleID)
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_DELETE_ROLE_REQUEST*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_DELETE_ROLE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_DELETE_ROLE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_delete_role_request;
    //pSend->nPlayerIndex = nPlayerIndex;
    pSend->dwRoleID = dwRoleID;

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode );

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::DoGameLoginRequest(char szAccountName[], DWORD dwRoleID, KROLE_LIST_INFO& BaseData, DWORD dwQueueID)
{

    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    IKG_Buffer*         piSendBuffer    = NULL;
    G2R_GAME_LOGIN_REQUEST*    pSend    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(G2R_GAME_LOGIN_REQUEST) + sizeof(KROLE_LIST_INFO));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSend = (G2R_GAME_LOGIN_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSend);

    pSend->byProtocol    = g2r_game_login_request;
    //pSend->nPlayerIndex = nPlayerIndex;
    pSend->dwRoleID = dwRoleID;
	pSend->dwQueueID = dwQueueID;
    strncpy(pSend->szAccountName, szAccountName, sizeof(pSend->szAccountName));
    memcpy(pSend->byData, &BaseData, sizeof(KROLE_LIST_INFO));

    nRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGatewaySimulator::CheckPlayerLeaveGame(const char cszAccountName[])
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_PLAYER_LEAVE_GAME* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_player_leave_game);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_PLAYER_LEAVE_GAME*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);

    nRetCode = strncmp(pRespond->szAccount, cszAccountName, sizeof(pRespond->szAccount));
    KG_PROCESS_ERROR(nRetCode == 0);


    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}

BOOL KGatewaySimulator::CheckGameLoginRespond(R2G_GAME_LOGIN_RESPOND* pGameLoginRepsond)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_GAME_LOGIN_RESPOND* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_game_login_respond);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_GAME_LOGIN_RESPOND*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);

    memcpy((void*)pGameLoginRepsond, pRespond, sizeof(R2G_GAME_LOGIN_RESPOND));

    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}

BOOL KGatewaySimulator::CheckGameWorldVersion(int nLowerProtocolVersion, int nUpperProtocolVersion)
{
    int nResult = false;
    int nRetCode = false;
    IKG_Buffer* piRecvPackage = NULL;
    R2G_SET_GAME_WORLD_VERSION* pRespond = NULL;

    piRecvPackage = ReceiveDataOnce(r2g_delete_role_respond);
    KG_PROCESS_ERROR(piRecvPackage);

    pRespond = (R2G_SET_GAME_WORLD_VERSION*)piRecvPackage->GetData();
    KG_PROCESS_ERROR(pRespond);

    KG_PROCESS_ERROR(pRespond->nLowerVersion == nLowerProtocolVersion);
    KG_PROCESS_ERROR(pRespond->nUpperVersion == nUpperProtocolVersion);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piRecvPackage);
    return nResult;
}


bool KGatewaySimulator::CheckLoginRespond(BYTE byCode, DWORD dwRoleID, DWORD& dwQueueID)
{
	int nResult = false;
	int nRetCode = false;
	IKG_Buffer* piRecvPackage = NULL;
	R2G_GAME_LOGIN_RESPOND* pRespond = NULL;

	piRecvPackage = ReceiveDataOnce(r2g_game_login_respond);
	KG_PROCESS_ERROR(piRecvPackage);

	pRespond = (R2G_GAME_LOGIN_RESPOND*)piRecvPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->byCode == byCode);
	KG_PROCESS_ERROR(pRespond->dwRoleID == dwRoleID);
	dwQueueID = pRespond->dwQueueID;

	nResult = true;
Exit0:
	KG_COM_RELEASE(piRecvPackage);
	return nResult;
}


int KGatewaySimulator::GetProtocolIndex(IKG_Buffer* pBuffer)
{
	if (pBuffer)
	{
		int nSize = pBuffer->GetSize();
		if (nSize >= sizeof(KGR_PROTOCOL_HEADER))
		{
			KGR_PROTOCOL_HEADER* p  = (KGR_PROTOCOL_HEADER*)pBuffer->GetData();
			return p->byProtocol;
		}
	}
	return -1;
}

bool KGatewaySimulator::Init()
{
	return Connect(GCSETTING->m_szGatewayIP, GCSETTING->m_nGatewayPort, GCSETTING->m_nGatewayPingCycle);
}