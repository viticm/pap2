#include "stdafx.h"
#include "KLogServerAgency.h"
#include "KSO3GameCenter.h"
#include "KItemDef.h"

KLogServerAgency::KLogServerAgency()
{
    m_piSocket      = NULL;
    m_nPingCycle    = 30;
    m_nNextPingTime = 0;
    m_szIdentity[0] = '\0';
}

BOOL KLogServerAgency::Init()
{
    const KSO3GameCenterSettings& rSettings = g_pSO3GameCenter->m_Settings;

    // 无论连上与否,均视为成功
    LoginServer(rSettings.m_szLogServerIP, rSettings.m_nLogServerPort, rSettings.m_szLogServerIdentity);

    return true;
}

void KLogServerAgency::UnInit()
{
    KG_COM_RELEASE(m_piSocket);
}

void KLogServerAgency::Activate()
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

void KLogServerAgency::LogFreezeCoin(KRole* pRole, int nCoin, const char* pszComment)
{
    int     nRetCode    = false;
    size_t  uStrLen     = 0;
    char    szCoin[32];

    assert(pRole);
    assert(nCoin > 0);
    assert(pszComment);

    nRetCode = snprintf(szCoin, sizeof(szCoin), "%d", nCoin);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szCoin));

    uStrLen = strlen(pszComment) + 1;

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_FREEZE_COIN, pRole->m_byLevel, szCoin, pRole->m_pszAccount,
        pRole->m_szName, uStrLen, (BYTE*)pszComment
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogUnfreezeCoin(KRole* pRole, int nCoin, const char* pszComment)
{
    int     nRetCode    = false;
    size_t  uStrLen     = 0;
    char    szCoin[32];

    assert(pRole);
    assert(nCoin > 0);
    assert(pszComment);

    nRetCode = snprintf(szCoin, sizeof(szCoin), "%d", nCoin);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szCoin));

    uStrLen = strlen(pszComment) + 1;

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_UNFREEZE_COIN, pRole->m_byLevel, szCoin, pRole->m_pszAccount,
        pRole->m_szName, uStrLen, (BYTE*)pszComment
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogGameCardSell(KRole* pRole, int nCoin, const char* pszComment)
{
    int     nRetCode    = false;
    size_t  uStrLen     = 0;
    char    szCoin[32];

    assert(pRole);
    assert(nCoin > 0);
    assert(pszComment);

    nRetCode = snprintf(szCoin, sizeof(szCoin), "%d", nCoin);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szCoin));

    uStrLen = strlen(pszComment) + 1;

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_GAME_CARD_SELL, pRole->m_byLevel, szCoin, pRole->m_pszAccount,
        pRole->m_szName, uStrLen, (BYTE*)pszComment
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogGameCardBuy(KRole* pRole, int nGameTime, BYTE byType, const char* pszComment)
{
    int     nRetCode    = false;
    size_t  uStrLen     = 0;
    char    szTarget[32];

    assert(pRole);
    assert(nGameTime > 0);
    assert(pszComment);

    nRetCode = snprintf(szTarget, sizeof(szTarget), "%d %u", nGameTime, (unsigned)byType);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szTarget));

    uStrLen = strlen(pszComment) + 1;

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_GAME_CARD_BUY, pRole->m_byLevel, szTarget, pRole->m_pszAccount,
        pRole->m_szName, uStrLen, (BYTE*)pszComment
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogRoleAntiFarmer(KRole* pRole, time_t nAntiSeconds)
{
    BOOL    bRetCode        = false;
    char    szComment[64];
    
    snprintf(szComment, sizeof(szComment), "anti time : %d seconds", (int)nAntiSeconds);
    szComment[sizeof(szComment) - 1] = '\0';
    
    bRetCode = LogPlayerAction(PLAYER_ACTION_ANTI_FARMER, pRole->m_byLevel, szComment, pRole->m_pszAccount, pRole->m_szName, 0, NULL);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogBuyAuctionItem(
    DWORD dwBuyerID, DWORD dwSellerID, size_t uItemDataLen, BYTE* pbyItemData, int nPrice
)
{
    BOOL                bRetCode        = false;
    KRole*              pBuyer          = NULL;
    KRole*              pSeller         = NULL;
    KCOMMON_ITEM_DATA*  pItemData       = NULL;
    size_t              uStrLen         = 0;
    char                szComment[256];

    assert(pbyItemData);
    
    pBuyer = g_pSO3GameCenter->m_RoleManager.GetRole(dwBuyerID);
    KGLOG_PROCESS_ERROR(pBuyer);
    
    KGLOG_PROCESS_ERROR(uItemDataLen >= sizeof(KCOMMON_ITEM_DATA));
    pItemData = (KCOMMON_ITEM_DATA*)pbyItemData;
    
    pSeller = g_pSO3GameCenter->m_RoleManager.GetRole(dwSellerID);
    if (pSeller) // seller maybe deleted
    {
        snprintf(
            szComment, sizeof(szComment), "Seller acc : %s, name : %s, item tabtype : %d, index: %d, durability : %d", 
            pSeller->m_pszAccount, pSeller->m_szName, pItemData->byTabType, pItemData->wTabIndex, pItemData->wDurability
        );
        szComment[sizeof(szComment) - 1] = '\0';
    }
    else
    {
        snprintf(
            szComment, sizeof(szComment), "Seller role id : %lu, item tabtype : %d, index: %d, durability : %d", 
            dwSellerID, pItemData->byTabType, pItemData->wTabIndex, pItemData->wDurability
        );
        szComment[sizeof(szComment) - 1] = '\0';
    }

    uStrLen = strlen(szComment) + 1;

    bRetCode = LogPlayerAction(
        PLAYER_ACTION_BUY_AUCTION_ITEM, nPrice, "BUY AUCTION ITEM", pBuyer->m_pszAccount,
        pBuyer->m_szName, uStrLen, (BYTE*)&szComment[0]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogApexKickRole(KRole* pRole, int nActionID)
{
    BOOL    nRetCode        = 0;
    size_t  uCommentSize    = 0;
    char    szComment[256];

    nRetCode = snprintf(szComment, sizeof(szComment), "%d", nActionID);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szComment));

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_CHEAT_KICKED, 100, "KICK_ROLE", pRole->m_pszAccount, pRole->m_szName,
        uCommentSize, (BYTE*)szComment
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogFreezeCoinTimeout(KRole* pRole, int nCoin, int nPrice)
{
    int     nRetCode = 0;
    char    szTarget[32];

    assert(pRole);

    nRetCode = snprintf(szTarget, sizeof(szTarget), "%d", nCoin);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szTarget));

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_FREEZE_COIN_TIMEOUT, nPrice, szTarget,
        pRole->m_pszAccount, pRole->m_szName, 0, NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogAccountExchangeTimeout(KRole* pSellerRole, KRole* pBuyyerRole, int nCoin, int nGameTime, int nPrice)
{
    int     nRetCode    = 0;
    size_t  uStrLen     = 0;
    char    szTarget[32];
    char    szComment[64];

    assert(pSellerRole);
    assert(pBuyyerRole);

    nRetCode = snprintf(szTarget, sizeof(szTarget), "%d %d", nCoin, nGameTime);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szTarget));

    nRetCode = snprintf(szComment, sizeof(szComment), "Seller:%s(%s)", pSellerRole->m_szName, pSellerRole->m_pszAccount);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szComment));

    uStrLen = strlen(szComment) + 1;

    nRetCode = LogPlayerAction(
        PLAYER_ACTION_ACCOUNT_EXCHANGE_TIMEOUT, nPrice, szTarget,
        pBuyyerRole->m_pszAccount, pBuyyerRole->m_szName, uStrLen, (BYTE*)szComment
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KLogServerAgency::LogMentorshipCreate(DWORD dwMentorID, DWORD dwApprenticeID, const char* pszComment)
{
    KRole*  pARole      = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
    size_t  uCommentLen = 0;
    char    szMentorID[32];

    KGLOG_PROCESS_ERROR(pARole);

    snprintf(szMentorID, sizeof(szMentorID), "%lu", dwMentorID);
    szMentorID[sizeof(szMentorID) - 1] = '\0';

    uCommentLen = strlen(pszComment) + 1;

    LogPlayerAction(
        PLAYER_ACTION_MENTORSHIP_CREATE, pARole->m_byLevel, szMentorID,
        pARole->m_pszAccount, pARole->m_szName, uCommentLen, (BYTE*)pszComment
    );

Exit0:
    return;
}

void KLogServerAgency::LogMentorshipDelete(DWORD dwMentorID, DWORD dwApprenticeID, int nMentorValue, const char* pszComment)
{
    KRole*  pARole      = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
    size_t  uCommentLen = 0;
    char    szMentorID[32];

    KG_PROCESS_ERROR(pARole);

    snprintf(szMentorID, sizeof(szMentorID), "%lu", dwMentorID);
    szMentorID[sizeof(szMentorID) - 1] = '\0';

    uCommentLen = strlen(pszComment) + 1;

    LogPlayerAction(
        PLAYER_ACTION_MENTORSHIP_DELETE, nMentorValue, szMentorID,
        pARole->m_pszAccount, pARole->m_szName, uCommentLen, (BYTE*)pszComment
    );

Exit0:
    return;
}

void KLogServerAgency::LogAcquireMentorValue(DWORD dwMentorID, DWORD dwApprenticeID, int nValue, const char* pszComment)
{
    KRole*  pMRole      = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);
    size_t  uCommentLen = 0;
    char    szApprenticeID[32];

    KG_PROCESS_ERROR(pMRole);

    snprintf(szApprenticeID, sizeof(szApprenticeID), "%lu", dwApprenticeID);
    szApprenticeID[sizeof(szApprenticeID) - 1] = '\0';

    uCommentLen = strlen(pszComment) + 1;

    LogPlayerAction(
        PLAYER_ACTION_ACQUIRE_MENTOR_VALUE, nValue, szApprenticeID,
        pMRole->m_pszAccount, pMRole->m_szName, uCommentLen, (BYTE*)pszComment
    );

Exit0:
    return;
}

void KLogServerAgency::LogPlayerItemChange(
    KRole* pRole, ITEM_OPERATION_TYPE eOperationType,
    DWORD dwItemTabType, DWORD dwItemIndex, int nStackNum, 
    const char* pcszComment, const char* pcszCommentExt
)
{
    BOOL                bRetCode            = false;
    int                 nRetCode            = 0;
    IKG_Buffer*         piSendBuffer        = NULL;
    KGItemLog*          pItemLog            = NULL;
    size_t              uRestoreDataSize    = 0;
    size_t              uCommentSize        = 0;
    size_t              uCommentExtSize     = 0;
    char                szRestoreData[128];

    assert(pRole);
    assert(pcszComment);

    nRetCode = snprintf(
        szRestoreData, sizeof(szRestoreData), 
        "dwTabType: %lu, dwIndex: %lu, nDurability: %d", 
        dwItemIndex, dwItemIndex, nStackNum
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szRestoreData));
    uRestoreDataSize = nRetCode + 1;

    uCommentSize = strlen(pcszComment) + 1;
    if (pcszCommentExt)
    {
        uCommentExtSize = strlen(pcszCommentExt) + 1;
    }

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGItemLog) - 1 + uRestoreDataSize + uCommentSize + uCommentExtSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pItemLog = (KGItemLog*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pItemLog);

    pItemLog->cProtocol         = LOGC2S_LOG_ITEM_OPERATION;
    pItemLog->ulIdentity        = 0;
    pItemLog->eOperationType    = eOperationType;

    strncpy(pItemLog->szAccountName, pRole->m_pszAccount, sizeof(pItemLog->szAccountName));
    pItemLog->szAccountName[sizeof(pItemLog->szAccountName) - 1] = '\0';

    strncpy(pItemLog->szRoleName, pRole->m_szName, sizeof(pItemLog->szRoleName));
    pItemLog->szRoleName[sizeof(pItemLog->szRoleName) - 1] = '\0';

    pItemLog->ullItemID = 0;
    pItemLog->szItemName[0] = '\0';         // 无法取得Item名称
    pItemLog->uItemValue = 0;               // 无法取得品质
    pItemLog->uItemRestoreDataSize = (unsigned)uRestoreDataSize;
    pItemLog->uCommentSize = (unsigned)(uCommentSize + uCommentExtSize);

    memcpy(pItemLog->pbyData, szRestoreData, uRestoreDataSize);
    memcpy(pItemLog->pbyData + uRestoreDataSize, pcszComment, uCommentSize);
    if (pcszCommentExt)
    {
        pItemLog->pbyData[uRestoreDataSize + uCommentSize - 1] = ' ';
        memcpy(pItemLog->pbyData + uRestoreDataSize + uCommentSize, pcszCommentExt, uCommentExtSize);
    }

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return;
}

BOOL KLogServerAgency::LoginServer(const char cszIP[], int nPort, const char cszIdentity[])
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

    strncpy(pLogin->szGatewayName, cszIdentity, sizeof(pLogin->szGatewayName));
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

BOOL KLogServerAgency::Ping()
{
    BOOL                bResult      = false;
    BOOL                bRetCode     = false;
    IKG_Buffer*         piSendBuffer = NULL;
    tagProtocolHeader2* pPing        = NULL;

    KG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= m_nNextPingTime);

    m_nNextPingTime = g_pSO3GameCenter->m_nTimeNow + m_nPingCycle;

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

BOOL KLogServerAgency::LogPlayerAction(
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

BOOL KLogServerAgency::Send(IKG_Buffer* piBuffer)
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
