#include "stdafx.h"
#include "KGameCardManager.h"
#include "KGameCardDef.h"
#include "KSO3GameCenter.h"

#define REGISTER_OPERATION_FUNC(Type, Func) \
    m_OperationFunc[Type] = &KGameCardManager::Func

KGameCardManager::KGameCardManager()
{
    m_bLookupBusy               = false;
    m_nNextCheckDurationTime    = 0;
    m_dwNextResquestID          = 0;
}

KGameCardManager::~KGameCardManager()
{
}

BOOL KGameCardManager::Init()
{
    return true;
}

void KGameCardManager::UnInit()
{
}

void KGameCardManager::Activate()
{
    BOOL bRetCode = false;

    for (KGAME_CARD_SELL_TABLE::iterator it = m_SellTable.begin(), itEnd = m_SellTable.end(); it != itEnd; NULL)
    {
        if (g_pSO3GameCenter->m_nTimeNow > it->second.nEndTime)
        {
            KRole*                              pRole       = NULL;
            const KGAME_CARD_SELL_OPERATION&    rOperation  = it->second;

            KGLogPrintf(KGLOG_INFO, "Paysys freeze coin timeout, RequestID = %lu", it->first);

            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->second.dwPlayerID);
            if (pRole)
            {
                g_pSO3GameCenter->m_LogServerAgency.LogFreezeCoinTimeout(
                    pRole, rOperation.nCoin, rOperation.nPrice
                );
            }

            m_SellTable.erase(it++);
            continue;
        }
        
        ++it;
    }

    for (KGAME_CARD_BUY_TABLE::iterator it = m_BuyTable.begin(), itEnd = m_BuyTable.end(); it != itEnd; NULL)
    {
        if (g_pSO3GameCenter->m_nTimeNow > it->second.nEndTime)
        {
            KRole*                          pSellerRole = NULL;
            KRole*                          pBuyyerRole = NULL;
            const KGAME_CARD_BUY_OPERATION& rOperation  = it->second;

            KGLogPrintf(KGLOG_INFO, "Paysys account exchange timeout, RequestID = %lu", it->first);

            pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->second.dwSellerID);
            pBuyyerRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->second.dwPlayerID);
            if (pSellerRole && pBuyyerRole)
            {
                g_pSO3GameCenter->m_LogServerAgency.LogAccountExchangeTimeout(
                    pSellerRole, pBuyyerRole, rOperation.nCoin, rOperation.nGameTime, rOperation.nPrice
                );
            }

            m_BuyTable.erase(it++);
            continue;
        }

        ++it;
    }

    if (!m_LookupList.empty() && !m_bLookupBusy)
    {
        KGAME_CARD_LOOKUP_OPERATION* pOperation = m_LookupList.front();

        m_LookupList.pop_front();

        g_pSO3GameCenter->m_MiscDB.DoGameCardLookupRequest(
            pOperation->dwPlayerID, pOperation->byCardType, pOperation->nStartIndex,
            pOperation->byOrderType, pOperation->bDesc
        );

        KMEMORY_FREE(pOperation);

        m_bLookupBusy = true;
    }

    bRetCode = g_pSO3GameCenter->m_Gateway.IsAvailable();
    if (bRetCode && g_pSO3GameCenter->m_nTimeNow >= m_nNextCheckDurationTime)
    {
        g_pSO3GameCenter->m_MiscDB.DoGameCardCheckDuration(g_pSO3GameCenter->m_nTimeNow);
        m_nNextCheckDurationTime = g_pSO3GameCenter->m_nTimeNow + 5 * 60;
    }
}

BOOL KGameCardManager::PostRequestSell(
    DWORD dwPlayerID, int nCoin, int nGameTime, BYTE byType, int nPrice, int nDurationTime
)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KRole*                          pRole       = NULL;
    DWORD                           dwRequestID = ++m_dwNextResquestID;
    KGAME_CARD_SELL_OPERATION       Operation;
    KGAME_CARD_SELL_TABLE::iterator it;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    Operation.dwRequestID   = dwRequestID;
    Operation.dwPlayerID    = dwPlayerID;
    Operation.nEndTime      = g_pSO3GameCenter->m_nTimeNow + GAME_CARD_TIMEOUT_TIME;
    Operation.nCoin         = nCoin;
    Operation.nGameTime     = nGameTime;
    Operation.byType        = byType;
    Operation.nPrice        = nPrice;
    Operation.nDurationTime = nDurationTime;

    m_SellTable[dwRequestID] = Operation;

    bRetCode = g_pSO3GameCenter->m_Gateway.DoFreezeCoinRequest(dwRequestID, pRole->m_pszAccount, true, nCoin);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(
        KGLOG_INFO, "Paysys freeze coin request: RequestID = %d, Account = %s, RoleName = %s, Coin = %d",
        dwRequestID, pRole->m_pszAccount, pRole->m_szName, nCoin
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGameCardManager::PostRequestBuy(
    DWORD dwSellerID, int nCoin, int nGameTime, BYTE byType, int nPrice, DWORD dwBuyerID
)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KRole*                          pSellerRole = NULL;
    KRole*                          pBuyerRole  = NULL;
    DWORD                           dwRequestID = ++m_dwNextResquestID;
    KGAME_CARD_BUY_OPERATION        Operation;
    KGAME_CARD_BUY_TABLE::iterator  it;

    KGLOG_PROCESS_ERROR(nGameTime > 0 && nCoin > 0);

    pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwSellerID);
    if (pSellerRole == NULL)
    {
        g_pSO3GameCenter->m_GameServer.DoGameCardBuyRespond(dwBuyerID, gorUnknownError, nPrice);
        goto Exit0;
    }

    pBuyerRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwBuyerID);
    KGLOG_PROCESS_ERROR(pBuyerRole);

    Operation.dwRequestID   = dwRequestID;
    Operation.dwPlayerID    = dwBuyerID;
    Operation.nEndTime      = g_pSO3GameCenter->m_nTimeNow + GAME_CARD_TIMEOUT_TIME;
    Operation.dwSellerID    = dwSellerID;
    Operation.nCoin         = nCoin;
    Operation.nGameTime     = nGameTime;
    Operation.byType        = byType;
    Operation.nPrice        = nPrice;

    m_BuyTable[dwRequestID] = Operation;

    bRetCode = g_pSO3GameCenter->m_Gateway.DoAccountExchangeRequest(
        dwRequestID, pSellerRole->m_pszAccount, pBuyerRole->m_pszAccount, (DWORD)byType,
        (DWORD)nGameTime, (DWORD)nCoin
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(
        KGLOG_INFO,
        "Paysys account exchange request: RequestID = %d, Seller(Account = %s, RoleName = %s, Coin = %d), "
        "Buyer(Account = %s, RoleName = %s, GameTime = %d), ExchangeType = %d",
        dwRequestID, pSellerRole->m_pszAccount, pSellerRole->m_szName, nCoin,
        pBuyerRole->m_pszAccount, pBuyerRole->m_szName, nGameTime, (int)byType
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGameCardManager::PostRequestLookup(DWORD dwPlayerID, BYTE byCardType, int nStartIndex, BYTE byOrderType, BOOL bDesc)
{
    BOOL                                bResult     = false;
    KGAME_CARD_LOOKUP_OPERATION*        pOperation = NULL;

    pOperation = (KGAME_CARD_LOOKUP_OPERATION*)KMemory::Alloc(sizeof(KGAME_CARD_LOOKUP_OPERATION));
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->dwPlayerID      = dwPlayerID;
    pOperation->nEndTime        = 0;
    pOperation->byCardType      = byCardType;
    pOperation->nStartIndex     = nStartIndex;
    pOperation->byOrderType     = byOrderType;
    pOperation->bDesc           = bDesc;

    m_LookupList.push_back(pOperation);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pOperation);
    }
    return bResult;
}

void KGameCardManager::OnDBBuyRespond(
    BYTE byCode, DWORD dwSellerID, int nCoin, int nGameTime, BYTE byType, int nPrice, DWORD dwBuyerID
)
{
    if (byCode == gorSucceed)
    {
        PostRequestBuy(dwSellerID, nCoin, nGameTime, byType, nPrice, dwBuyerID);
    }
    else
    {
        g_pSO3GameCenter->m_GameServer.DoGameCardBuyRespond(dwBuyerID, byCode, nPrice);
    }
}

void KGameCardManager::OnDBLookupRespond(DWORD dwPlayerID, int nTotalCount, int nCount, KGAME_CARD* pCardList)
{
    KRole* pRole = NULL;

    assert(pCardList);

    m_bLookupBusy = false;

    for (int i = 0; i < nCount; i++)
    {
        pCardList[i].szSellerName[0] = '\0';

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pCardList[i].dwSellerID);
        if (pRole)
        {
            strncpy(pCardList[i].szSellerName, pRole->m_szName, sizeof(pCardList[i].szSellerName));
            pCardList[i].szSellerName[sizeof(pCardList[i].szSellerName) - 1] = '\0';
        }
    }

    g_pSO3GameCenter->m_GameServer.DoGameCardLookupRespond(dwPlayerID, nTotalCount, nCount, pCardList);
}

void KGameCardManager::OnPaysysFreezeCoinRespond(BOOL bSucceed, DWORD dwRequestID)
{
    KGAME_CARD_SELL_OPERATION*      pOperation  = NULL;
    BYTE                            byCode      = gorFreezeCoinFailed;
    KRole*                          pRole       = NULL;
    KGAME_CARD_SELL_TABLE::iterator it;

    KGLogPrintf(
        KGLOG_INFO, "Paysys freeze coin respond: RequestID = %lu, Ret = %s\n",
        dwRequestID, bSucceed ? "succeed" : "failed"
    );

    it = m_SellTable.find(dwRequestID);
    KGLOG_PROCESS_ERROR(it != m_SellTable.end());

    pOperation = &(it->second);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_LogServerAgency.LogFreezeCoin(pRole, pOperation->nCoin, bSucceed ? "succeed" : "failed");

    if (bSucceed)
    {
        g_pSO3GameCenter->m_MiscDB.DoGameCardSellRequest(
            pOperation->dwPlayerID, pOperation->nCoin, pOperation->nGameTime,
            pOperation->byType, pOperation->nPrice, g_pSO3GameCenter->m_nTimeNow + pOperation->nDurationTime
        );
        byCode = gorSucceed;
    }

    g_pSO3GameCenter->m_GameServer.DoGameCardSellRespond(pOperation->dwPlayerID, byCode, pOperation->nCoin);

Exit0:
    m_SellTable.erase(dwRequestID);
    return;
}

void KGameCardManager::OnPaysysAccountExchangeRespond(BOOL bSucceed, DWORD dwRequestID)
{
    KGAME_CARD_BUY_OPERATION*       pOperation  = NULL;
    BYTE                            byCode      = gorExchangeFailed;
    KRole*                          pBuyerRole  = NULL;
    KRole*                          pSellerRole = NULL;
    KGAME_CARD_BUY_TABLE::iterator  it;

    KGLogPrintf(
        KGLOG_INFO, "Paysys account exchange respond: RequestID = %lu, Ret = %s\n",
        dwRequestID, bSucceed ? "succeed" : "failed"
    );

    it = m_BuyTable.find(dwRequestID);
    KGLOG_PROCESS_ERROR(it != m_BuyTable.end());

    pOperation = &(it->second);

    pBuyerRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwPlayerID);
    KGLOG_PROCESS_ERROR(pBuyerRole);

    pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwSellerID);
    KGLOG_PROCESS_ERROR(pSellerRole);

    g_pSO3GameCenter->m_LogServerAgency.LogGameCardBuy(pBuyerRole, pOperation->nGameTime, pOperation->byType, bSucceed ? "succeed" : "failed");

    g_pSO3GameCenter->m_LogServerAgency.LogGameCardSell(pSellerRole, pOperation->nCoin, bSucceed ? "succeed" : "failed");

    if (bSucceed)
    {
        SendMoneyToSeller(pOperation);

        g_pSO3GameCenter->m_StatDataManager.UpdateGameCardDealAmmount(
            pOperation->nCoin, pOperation->nPrice
        );

        byCode = gorSucceed;
    }

    g_pSO3GameCenter->m_GameServer.DoGameCardBuyRespond(pOperation->dwPlayerID, byCode, pOperation->nPrice);

Exit0:
    m_BuyTable.erase(dwRequestID);
    return;
}

inline int _FormatMoney(char* pszDst, size_t uLeftSize, const char* pszName, int nMoney)
{
    int nRetCode = 0;

    nRetCode = snprintf(
        pszDst, uLeftSize,
        "%s%4d%s%2d%s%2d%s\n",
        pszName,
        nMoney / 10000, g_pSO3GameCenter->m_Settings.m_szGold,
        (nMoney / 100) % 100, g_pSO3GameCenter->m_Settings.m_szSilver,
        nMoney % 100, g_pSO3GameCenter->m_Settings.m_szCopper
    );

    return nRetCode;
}

BOOL KGameCardManager::SendMoneyToSeller(KGAME_CARD_BUY_OPERATION* pOperation)
{
    BOOL                            bResult     = false;
    int                             nRetCode    = 0;
    KRole*                          pBuyerRole  = NULL;
    size_t                          uTextSize   = 0;
    int                             nTaxMoney   = 0;
    int                             nGetMoney   = 0;
    const KSO3GameCenterSettings&   rSettings   = g_pSO3GameCenter->m_Settings;
    char*                           pszText     = NULL;
    size_t                          uLeftSize   = 0;
    char                            szTitle[TITLE_LEN];
    char                            szText[MAX_MAIL_TEXT_LEN];

    assert(pOperation);

    pBuyerRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwPlayerID);
    KGLOG_PROCESS_ERROR(pBuyerRole);

    nTaxMoney = (int)((double)pOperation->nPrice * rSettings.m_nGameCardTaxRate / 1000);
    nGetMoney = pOperation->nPrice - nTaxMoney;

    snprintf(szTitle, sizeof(szTitle), "%s%d", rSettings.m_szSellSucceed, pOperation->nCoin);
    szTitle[sizeof(szTitle) - 1] = '\0';

    pszText     = szText;
    uLeftSize   = sizeof(szText);

    nRetCode = snprintf(
        pszText, uLeftSize,
        "%s%d%s\n"
        "%s%s\n",
        rSettings.m_szSellSucceed, pOperation->nCoin, rSettings.m_szCoin,
        rSettings.m_szBuyyer, pBuyerRole->m_szName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    nRetCode = _FormatMoney(pszText, uLeftSize, rSettings.m_szPrice, pOperation->nPrice);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    nRetCode = _FormatMoney(pszText, uLeftSize, rSettings.m_szTaxMoney, nTaxMoney);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    nRetCode = _FormatMoney(pszText, uLeftSize, rSettings.m_szGetMoney, nGetMoney);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    szText[sizeof(szText) - 1] = '\0';
    uTextSize = strlen(szText) + 1;

    g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, pOperation->dwSellerID, 0, eMailType_Auction, rSettings.m_szAuctionName,
        szTitle, nGetMoney, (BYTE*)szText, uTextSize, NULL, 0, 0
    );

    bResult = true;
Exit0:
    return bResult;
}
