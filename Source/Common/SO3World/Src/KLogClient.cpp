#include "stdafx.h"

#ifdef _SERVER
#include "KSO3World.h"
#include "KItem.h"
#include "KPlayer.h"

KLogClient g_LogClient;

KLogClient::KLogClient()
{
    m_piSocket      = NULL;
    m_nPingCycle    = 30;
    m_nNextPingTime = 0;
    m_szIdentity[0] = '\0';
}

BOOL KLogClient::Init()
{
    BOOL               bResult          = false;
    BOOL               bRetCode         = false;
    IIniFile*	       piIniFile        = NULL;
    int                nPort            = 0;
    char               szIP[16];

    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile);

    bRetCode = piIniFile->GetString("LogServer", "IP", "127.0.0.1", szIP, sizeof(szIP));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piIniFile->GetInteger("LogServer", "Port", 5005, &nPort);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piIniFile->GetInteger("LogServer", "PingCycle", 30, &m_nPingCycle);
    KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(m_nPingCycle > 0);

    bRetCode = piIniFile->GetString("LogServer", "Identity", "", m_szIdentity, sizeof(m_szIdentity));
    KGLOG_PROCESS_ERROR(bRetCode);
    m_szIdentity[sizeof(m_szIdentity) - 1] = '\0';

    // 无论连上与否,均视为成功
    LoginServer(szIP, nPort, m_szIdentity);

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
    DWORD dwQuestID, const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
)
{
    size_t uCommentSize = 0;

    snprintf(m_szTempBuffer, sizeof(m_szTempBuffer), "%lu", dwQuestID);
    m_szTempBuffer[sizeof(m_szTempBuffer) - 1] = '\0';

    uCommentSize = (unsigned)strlen(m_szTempBuffer) + 1;

    return LogPlayerAction(
        PLAYER_ACTION_ACCEPT_TASK, (unsigned)nQuestLevel, pszQuestName,
        pszAccount, pszRoleName, uCommentSize, (BYTE*)m_szTempBuffer
    );
}

BOOL KLogClient::LogFinishQuest(
    DWORD dwQuestID, const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
)
{
    size_t uCommentSize = 0;

    snprintf(m_szTempBuffer, sizeof(m_szTempBuffer), "%lu", dwQuestID);
    m_szTempBuffer[sizeof(m_szTempBuffer) - 1] = '\0';

    uCommentSize = (unsigned)strlen(m_szTempBuffer) + 1;

    return LogPlayerAction(
        PLAYER_ACTION_FINISH_TASK, (unsigned)nQuestLevel, pszQuestName,
        pszAccount, pszRoleName, uCommentSize, (BYTE*)m_szTempBuffer
    );
}

BOOL KLogClient::LogCancelQuest(
    DWORD dwQuestID, const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
)
{
    size_t uCommentSize = 0;

    snprintf(m_szTempBuffer, sizeof(m_szTempBuffer), "%lu", dwQuestID);
    m_szTempBuffer[sizeof(m_szTempBuffer) - 1] = '\0';

    uCommentSize = (unsigned)strlen(m_szTempBuffer) + 1;

    return LogPlayerAction(
        PLAYER_ACTION_CANCEL_TASK, (unsigned)nQuestLevel, pszQuestName,
        pszAccount, pszRoleName, uCommentSize, (BYTE*)m_szTempBuffer
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

BOOL KLogClient::LogPlayerLogin(const char cszAccount[], const char cszRoleName[], const char cszIP[])
{
    size_t uCommentLen = strlen(cszIP) + 1;

    return LogPlayerAction(PLAYER_ACTION_LOGIN, 50, "Login", cszAccount, cszRoleName, uCommentLen, (BYTE*)cszIP);
}

BOOL KLogClient::LogPlayerLogout(const char cszAccount[], const char cszRoleName[])
{
    return LogPlayerAction(PLAYER_ACTION_LOGOUT, 50, "Logout", cszAccount, cszRoleName, 0, NULL);
}

BOOL KLogClient::GetItemLogInfo(KItem* pItem, KITEM_LOG_INFO* pItemLogInfo)
{
    BOOL                bResult          = false;
    BOOL                bRetCode         = false; 
    
    assert(pItem);
    assert(pItemLogInfo);

    ZeroMemory(pItemLogInfo, sizeof(KITEM_LOG_INFO));

    pItemLogInfo->dwItemID = pItem->m_dwID;
    
    strncpy(pItemLogInfo->szItemName, pItem->m_Common.szItemName, sizeof(pItemLogInfo->szItemName));
    pItemLogInfo->szItemName[sizeof(pItemLogInfo->szItemName) - 1] = '\0';

    pItemLogInfo->uItemValue = pItem->m_GenParam.nQuality;

    snprintf(
        pItemLogInfo->szRestoreData, sizeof(pItemLogInfo->szRestoreData), 
        "dwTabType: %lu, dwIndex: %lu, nDurability: %d", 
        pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, pItem->m_nCurrentDurability
    );
    pItemLogInfo->szRestoreData[sizeof(pItemLogInfo->szRestoreData) - 1] = '\0';

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerItemChange(
    KPlayer* pPlayer, ITEM_OPERATION_TYPE eOperationType, KITEM_LOG_INFO* pItemLogInfo, 
    const char* pszComment, const char* pszCommentExt
)
{
    BOOL                bResult          = false;
    BOOL                bRetCode         = false;
    IKG_Buffer*         piSendBuffer     = NULL;
    KGItemLog*          pItemLog         = NULL;
    size_t              uRestoreDataSize = 0;
    size_t              uCommentSize     = 0;
    size_t              uCommentExtSize  = 0;

    assert(pPlayer);
    assert(pItemLogInfo);
    assert(pszComment);
    
    uRestoreDataSize = strlen(pItemLogInfo->szRestoreData) + 1;
    
    uCommentSize = strlen(pszComment) + 1;
    if (pszCommentExt)
    {
        uCommentExtSize = strlen(pszCommentExt) + 1;
    }
    
    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGItemLog) - 1 + uRestoreDataSize + uCommentSize + uCommentExtSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pItemLog = (KGItemLog*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pItemLog);

    pItemLog->cProtocol         = LOGC2S_LOG_ITEM_OPERATION;
    pItemLog->ulIdentity        = 0;
    pItemLog->eOperationType    = eOperationType;
    
    strncpy(pItemLog->szAccountName, pPlayer->m_szAccount, sizeof(pItemLog->szAccountName));
    pItemLog->szAccountName[sizeof(pItemLog->szAccountName) - 1] = '\0';

    strncpy(pItemLog->szRoleName, pPlayer->m_szName, sizeof(pItemLog->szRoleName));
    pItemLog->szRoleName[sizeof(pItemLog->szRoleName) - 1] = '\0';

    pItemLog->ullItemID = pItemLogInfo->dwItemID;
    
    strncpy(pItemLog->szItemName, pItemLogInfo->szItemName, sizeof(pItemLog->szItemName));
    pItemLog->szItemName[sizeof(pItemLog->szItemName) - 1] = '\0';

    pItemLog->uItemValue = pItemLogInfo->uItemValue;
    pItemLog->uItemRestoreDataSize = (unsigned)uRestoreDataSize;
    pItemLog->uCommentSize = (unsigned)(uCommentSize + uCommentExtSize);

    memcpy(pItemLog->pbyData, pItemLogInfo->szRestoreData, uRestoreDataSize);
    memcpy(pItemLog->pbyData + uRestoreDataSize, pszComment, uCommentSize);
    if (pszCommentExt)
    {
        pItemLog->pbyData[uRestoreDataSize + uCommentSize - 1] = ' ';
        memcpy(pItemLog->pbyData + uRestoreDataSize + uCommentSize, pszCommentExt, uCommentExtSize);
    }

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KLogClient::LogPlayerItemChange(
    KPlayer* pPlayer, ITEM_OPERATION_TYPE eOperationType, KItem* pItem, 
    const char* pszComment, const char* pszCommentExt
)
{
    BOOL                bResult          = false;
    BOOL                bRetCode         = false;
    KITEM_LOG_INFO      ItemLogInfo;

    assert(pPlayer);
    assert(pItem);
    assert(pszComment);
    
    bRetCode = GetItemLogInfo(pItem, &ItemLogInfo);
    KG_PROCESS_ERROR(bRetCode);

    LogPlayerItemChange(pPlayer, eOperationType, &ItemLogInfo, pszComment, pszCommentExt);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerMoneyChange(
    int nChangeAmount, KPlayer* pPlayer, const char* pszComment, const char* pszCommentExt /*=NULL*/
)
{
    BOOL        bResult      = false;
    BOOL        bRetCode     = false;
    unsigned    uCommentSize = 0;
    char        szChangeMoney[16];

    assert(pPlayer);
    assert(pszComment);
    
    KG_PROCESS_ERROR(nChangeAmount != 0);

    if (pszCommentExt)
    {
        snprintf(m_szTempBuffer, sizeof(m_szTempBuffer), "%s %s", pszComment, pszCommentExt);
        m_szTempBuffer[sizeof(m_szTempBuffer) - 1] = '\0';
    }
    else
    {
        snprintf(m_szTempBuffer, sizeof(m_szTempBuffer), "%s", pszComment);
        m_szTempBuffer[sizeof(m_szTempBuffer) - 1] = '\0';
    }
    
    uCommentSize = (unsigned)strlen(m_szTempBuffer) + 1;

    snprintf(szChangeMoney, sizeof(szChangeMoney), "%d", nChangeAmount);
    szChangeMoney[sizeof(szChangeMoney) - 1] = '\0';

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_MONEY_CHANGE, pPlayer->m_ItemList.m_nMoney, szChangeMoney,
        pPlayer->m_szAccount, pPlayer->m_szName, uCommentSize, (BYTE*)m_szTempBuffer
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerJoinTeam(KPlayer* pPlayer, KTeam* pTeam)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    size_t  uMemberListSize = 0;
    char    szMemberList[_NAME_LEN * MAX_TEAM_GROUP_NUM * MAX_PLAYER_PER_GROUP]; // 能包含所有成员的名字。

    assert(pPlayer);
    assert(pTeam);

    szMemberList[0] = '\0';

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == pPlayer->m_dwID)
                continue;

            nRetCode = snprintf(szMemberList + uMemberListSize, sizeof(szMemberList), "%s,", it->szRoleName);
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode <= sizeof(szMemberList));
            uMemberListSize += strlen(it->szRoleName) + sizeof(',');
        }
    }

    if (uMemberListSize > 0)
    {
        szMemberList[uMemberListSize - 1] = '\0'; // 去掉最后一个','
    }

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_JOIN_TEAM, 100, "JOIN_TEAM", pPlayer->m_szAccount, pPlayer->m_szName,
        uMemberListSize, (BYTE*)szMemberList
    );
    KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerLeaveTeam(KPlayer* pPlayer)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;

    assert(pPlayer);

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_LEAVE_TEAM, 100, "LEAVE_TEAM",
        pPlayer->m_szAccount, pPlayer->m_szName, 0, NULL
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerAddFriend(KPlayer* pPlayer, DWORD dwFriendID, const char cszFriendName[])
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    char    szTarget[_NAME_LEN + 16];

    assert(pPlayer);

    if (cszFriendName)
        snprintf(szTarget, sizeof(szTarget), "%s(%lu)", cszFriendName, dwFriendID);
    else
        snprintf(szTarget, sizeof(szTarget), "NO_NAME(%lu)", dwFriendID);

    szTarget[sizeof(szTarget) - 1] = '\0';

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_ADD_FRIEND, 100, szTarget, 
        pPlayer->m_szAccount, pPlayer->m_szName, 0, NULL
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerDelFriend(KPlayer* pPlayer, DWORD dwFriendID, const char cszFriendName[])
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    char    szTarget[_NAME_LEN + 16];

    assert(pPlayer);

    if (cszFriendName)
        snprintf(szTarget, sizeof(szTarget), "%s(%lu)", cszFriendName, dwFriendID);
    else
        snprintf(szTarget, sizeof(szTarget), "NO_NAME(%lu)", dwFriendID);

    szTarget[sizeof(szTarget) - 1] = '\0';

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_DEL_FRIEND, 100, szTarget,
        pPlayer->m_szAccount, pPlayer->m_szName, 0, NULL
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerAddFoe(KPlayer* pPlayer, DWORD dwFoeID, const char cszFoeName[])
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    char    szTarget[_NAME_LEN + 16];

    assert(pPlayer);

    if (cszFoeName)
        snprintf(szTarget, sizeof(szTarget), "%s(%lu)", cszFoeName, dwFoeID);
    else
        snprintf(szTarget, sizeof(szTarget), "NO_NAME(%lu)", dwFoeID);

    szTarget[sizeof(szTarget) - 1] = '\0';

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_ADD_FOE, 100, szTarget,
        pPlayer->m_szAccount, pPlayer->m_szName, 0, NULL
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogClient::LogPlayerDelFoe(KPlayer* pPlayer, DWORD dwFoeID, const char cszFoeName[])
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    char    szTarget[_NAME_LEN + 16];

    assert(pPlayer);

    if (cszFoeName)
        snprintf(szTarget, sizeof(szTarget), "%s(%lu)", cszFoeName, dwFoeID);
    else
        snprintf(szTarget, sizeof(szTarget), "NO_NAME(%lu)", dwFoeID);

    szTarget[sizeof(szTarget) - 1] = '\0';

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_DEL_FOE, 100, szTarget,
        pPlayer->m_szAccount, pPlayer->m_szName, 0, NULL
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
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
