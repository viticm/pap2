#include "stdafx.h"

#ifdef _SERVER
#include "KSO3World.h"

KLogClient g_LogClient;

KLogClient::KLogClient()
{
    m_piSocket      = NULL;
    m_nPingCycle    = 30;
    m_nNextPingTime = 0;
}

KLogClient::~KLogClient()
{
}

BOOL KLogClient::Init()
{
    BOOL               bResult          = false;
    BOOL               bRetCode         = false;
    IIniFile*	       piIniFile        = NULL;
    int                nPort            = 0;
    char               szIP[16];
    char               szGateway[_NAME_LEN];

    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile);

    bRetCode = piIniFile->GetString("LogServer", "IP", "127.0.0.1", szIP, sizeof(szIP));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piIniFile->GetInteger("LogServer", "Port", 5005, &nPort);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piIniFile->GetInteger("LogServer", "PingCycle", 30, &m_nPingCycle);
    KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(m_nPingCycle > 0);

    bRetCode = piIniFile->GetString("LogServer", "Identity", "", szGateway, sizeof(szGateway));
    KGLOG_PROCESS_ERROR(bRetCode);

    // 无论连上与否,均视为成功
    LoginServer(szIP, nPort, szGateway);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return bResult;
}

void KLogClient::UnInit()
{
    KG_COM_RELEASE(m_piSocket);
}

void KLogClient::Activate()
{
    int         nRetCode        = false;
    BOOL        bConnectionLost = false;
    IKG_Buffer* piBuffer        = NULL;

    while (m_piSocket)
    {
        const struct timeval        TimeVal     = {0, 0};
        tagProtocolHeader2*         pHeader     = NULL;

        nRetCode = m_piSocket->CheckCanRecv(&TimeVal);
        if (nRetCode == -1)
        {
            bConnectionLost = true;
            break;
        }

        if (nRetCode == 0)
        {
            break;
        }

        KG_COM_RELEASE(piBuffer);

        nRetCode = m_piSocket->Recv(&piBuffer);
        if (nRetCode != 1)
        {
            bConnectionLost = true;
            break;
        }
    }

    if (bConnectionLost)
    {
        KGLogPrintf(KGLOG_ERR, "Log server connection lost !\n");
        KG_COM_RELEASE(m_piSocket);
        goto Exit0;
    }

    Ping();

Exit0:
    KG_COM_RELEASE(piBuffer);
}

BOOL KLogClient::LogAcceptQuest(
    const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
)
{
    return LogPlayerAction(
        PLAYER_ACTION_ACCEPT_TASK, (unsigned)nQuestLevel, pszQuestName,
        pszAccount, pszRoleName, 0, NULL
    );
}

BOOL KLogClient::LogFinishQuest(
    const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
)
{
    return LogPlayerAction(
        PLAYER_ACTION_FINISH_TASK, (unsigned)nQuestLevel, pszQuestName,
        pszAccount, pszRoleName, 0, NULL
    );
}

BOOL KLogClient::LogCancelQuest(
    const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
)
{
    return LogPlayerAction(
        PLAYER_ACTION_CANCEL_TASK, (unsigned)nQuestLevel, pszQuestName,
        pszAccount, pszRoleName, 0, NULL
    );
}

BOOL KLogClient::LogPlayerLevelup(int nLevel, const char* pszAccount, const char* pszRoleName)
{
    BOOL bResult  = false;
    int  nRetCode = 0;
    char szLevel[10];

    nRetCode = snprintf(szLevel, sizeof(szLevel), "%d", nLevel);
    KGLOG_PROCESS_ERROR(nRetCode >= 0 && nRetCode < (int)sizeof(szLevel));

    bResult = LogPlayerAction(
        PLAYER_ACTION_PLAYER_LEVELUP, 100, szLevel,
        pszAccount, pszRoleName, 0, NULL
    );
Exit0:
    return bResult;
}

BOOL KLogClient::LogTrainvalueChange(int nAddValue, const char* pszAccount, const char* pszRoleName)
{
    BOOL bResult  = false;
    int  nRetCode = 0;
    char szAddValue[10];

    KG_PROCESS_ERROR(nAddValue != 0);

    nRetCode = snprintf(szAddValue, sizeof(szAddValue), "%d", nAddValue);
    KGLOG_PROCESS_ERROR(nRetCode >= 0 && nRetCode < (int)sizeof(szAddValue));

    bResult = LogPlayerAction(
        PLAYER_ACTION_TRAINVALUE_CHANGE, 50, szAddValue,
        pszAccount, pszRoleName, 0, NULL
    );
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerLogin(const char* pszAccount, const char* pszRoleName)
{
    return LogPlayerAction(PLAYER_ACTION_LOGIN, 50, "Login", pszAccount, pszRoleName, 0, NULL);
}

BOOL KLogClient::LogPlayerLogout(const char* pszAccount, const char* pszRoleName)
{
    return LogPlayerAction(PLAYER_ACTION_LOGOUT, 50, "Logout", pszAccount, pszRoleName, 0, NULL);
}

BOOL KLogClient::LoginServer(const char cszIP[], int nPort, const char cszGateway[])
{
    BOOL                bResult      = false;
    BOOL                bRetCode     = false;
    IKG_Buffer*         piSendBuffer = NULL;
    KGCSLogServerLogin* pLogin       = NULL;
    KG_SocketConnector  Connector;
    struct timeval      TimeVal;

    assert(m_piSocket == NULL);

    m_piSocket = Connector.Connect(cszIP, nPort);
    KG_PROCESS_ERROR(m_piSocket);

    TimeVal.tv_sec  = 2;
    TimeVal.tv_usec = 0;

    bRetCode = m_piSocket->SetTimeout(&TimeVal);
    KGLOG_PROCESS_ERROR(bRetCode);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGCSLogServerLogin));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pLogin = (KGCSLogServerLogin*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pLogin);

    pLogin->ProtocolType = LOGC2S_LOG_LOGIN;
    pLogin->ulIdentity   = 0;

    strncpy(pLogin->szGatewayName, cszGateway, sizeof(pLogin->szGatewayName));
    pLogin->szGatewayName[sizeof(pLogin->szGatewayName) - 1] = '\0';
    
    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KGLogPrintf(
        KGLOG_INFO, "Connect to log server %s:%d ... ... [%s]",
        cszIP, nPort, bResult ? "OK" : "Failed"
    );
    if (!bResult)
    {
        KG_COM_RELEASE(m_piSocket);
    }
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KLogClient::Ping()
{
    BOOL                bResult      = false;
    BOOL                bRetCode     = false;
    IKG_Buffer*         piSendBuffer = NULL;
    tagProtocolHeader2* pPing        = NULL;

    KG_PROCESS_ERROR(g_pSO3World->m_nCurrentTime >= m_nNextPingTime);

    m_nNextPingTime = g_pSO3World->m_nCurrentTime + m_nPingCycle;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(tagProtocolHeader2));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPing = (tagProtocolHeader2*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPing);

    pPing->ProtocolType = LOGC2S_PING;
    pPing->ulIdentity   = 0;

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KLogClient::LogPlayerAction(
    PLAYER_ACTION_TYPE eActionType, unsigned uActionLevel, const char* pszActionTarget,
    const char* pszAccountName, const char* pszRoleName, size_t uCommentSize,
    BYTE* pbyComment
)
{
    BOOL                bResult      = false;
    BOOL                bRetCode     = false;
    size_t              uExtendSize  = 0;
    IKG_Buffer*         piSendBuffer = NULL;
    KGPlayerActionLog*  pLog         = NULL;

    assert(pszActionTarget);
    assert(pszAccountName);
    assert(pszRoleName);

    uExtendSize = uCommentSize ? uCommentSize - 1 : 0;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KGPlayerActionLog) + uExtendSize));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pLog = (KGPlayerActionLog*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pLog);

    pLog->ProtocolType = LOGC2S_LOG_PLAYER_ACTION;
    pLog->ulIdentity   = 0;
    pLog->uActionLevel = uActionLevel;
    pLog->eActionType  = eActionType;

    strncpy(pLog->szActionTarget, pszActionTarget, sizeof(pLog->szActionTarget));
    pLog->szActionTarget[sizeof(pLog->szActionTarget) - 1] = '\0';

    strncpy(pLog->szAccountName, pszAccountName, sizeof(pLog->szAccountName));
    pLog->szAccountName[sizeof(pLog->szAccountName) - 1] = '\0';

    strncpy(pLog->szRoleName, pszRoleName, sizeof(pLog->szRoleName));
    pLog->szRoleName[sizeof(pLog->szRoleName) - 1] = '\0';

    pLog->uCommentSize = (unsigned)uCommentSize;
    if (pbyComment)
    {
        memcpy(pLog->pbyData, pbyComment, uCommentSize);
    }

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KLogClient::Send(IKG_Buffer* piBuffer)
{
    BOOL bResult  = false;
    int  nRetCode = false;

    KG_PROCESS_ERROR(m_piSocket);

    nRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    return bResult;
}

#endif // _SERVER
