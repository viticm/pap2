#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KAuctionManager.h"
#include "KItemDef.h"

#define REGISTER_OPERATION_FUNC(Type, Func) \
    m_OperationFunc[Type] = &KAuctionManager::Func

KAuctionManager::KAuctionManager()
{
    m_bBusy                       = false;
    m_nNextCheckWithdrawSalesTime = 0;

    memset(m_OperationFunc, 0, sizeof(m_OperationFunc));
    REGISTER_OPERATION_FUNC(eaoLookup, ProcessLookupOperation);
    REGISTER_OPERATION_FUNC(eaoBid, ProcessBidOperation);
    REGISTER_OPERATION_FUNC(eaoSell, ProcessSellOperation);
    REGISTER_OPERATION_FUNC(eaoCancel, ProcessCancelOperation);
}

KAuctionManager::~KAuctionManager()
{
}

BOOL KAuctionManager::Init()
{
    return true;
}

void KAuctionManager::UnInit()
{
    void* pvOperation = NULL;

    while (!m_OperationQueue.empty())
    {
        pvOperation = m_OperationQueue.front();
        m_OperationQueue.pop_front();

        KMEMORY_FREE(pvOperation);
    }
}

void KAuctionManager::Activate()
{
    BYTE*                    pbyData    = NULL;
    size_t                   uDataLen   = 0;
    KAUCTION_OPERATION_HEAD* pHead      = NULL;
    KAUCTION_OPERATION_FUNC  pFunc      = NULL;

    while (!m_bBusy && !m_OperationQueue.empty())
    {
        pHead = (KAUCTION_OPERATION_HEAD*)m_OperationQueue.front();
        KGLOG_PROCESS_ERROR(pHead);

        pbyData  = (BYTE*)pHead;
        uDataLen = pHead->uSize;

        pFunc = m_OperationFunc[pHead->byType];
        KGLOG_PROCESS_ERROR(pFunc);

        m_OperationQueue.pop_front();

        (this->*pFunc)(pbyData, uDataLen);

        KMEMORY_FREE(pHead);

        m_bBusy = true;
    }

    if (g_pSO3GameCenter->m_nTimeNow >= m_nNextCheckWithdrawSalesTime)
    {
        DoCheckDurationRequest();
        m_nNextCheckWithdrawSalesTime = g_pSO3GameCenter->m_nTimeNow + 10 * 60;
    }

Exit0:
    return;
}

BOOL KAuctionManager::PostRequestLookup(BYTE byRequestID, DWORD dwPlayerID, KAUCTION_LOOKUP_PARAM* pParam)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KAUCTION_LOOKUP_OPERATION*  pOperation  = NULL;

    assert(pParam);

    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);

    if (m_OperationQueue.size() > MAX_AUCTION_LOOKUP_COUNT)
    {
        g_pSO3GameCenter->m_GameServer.DoAuctionLookupRespond(dwPlayerID, byRequestID, arcServerBusy, NULL);

        goto Exit0;
    }

    pParam->szSaleName[sizeof(pParam->szSaleName) - 1] = '\0';
    pParam->szSellerName[sizeof(pParam->szSellerName) - 1] = '\0';

    KGLOG_PROCESS_ERROR(pParam->nLevelMin == 0 || pParam->nLevelMax == 0 || pParam->nLevelMin <= pParam->nLevelMax);
    KGLOG_PROCESS_ERROR(pParam->byAuction <= assNoOneBid);
    KGLOG_PROCESS_ERROR(pParam->nStartIndex >= 0);
    KGLOG_PROCESS_ERROR(pParam->byOrderType < aotTotal);

    pOperation = (KAUCTION_LOOKUP_OPERATION*)KMemory::Alloc(sizeof(KAUCTION_LOOKUP_OPERATION));
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->byType      = eaoLookup;
    pOperation->dwPlayerID  = dwPlayerID;
    pOperation->uSize       = sizeof(KAUCTION_LOOKUP_OPERATION);
    pOperation->byRequestID = byRequestID;
    pOperation->Param       = *pParam;

    m_OperationQueue.push_back(pOperation);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pOperation);
    }
    return bResult;
}

BOOL KAuctionManager::PostRequestBid(DWORD dwBidderID, DWORD dwSaleID, DWORD dwCRC, int nPrice)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    KAUCTION_BID_OPERATION* pOperation  = NULL;

    KGLOG_PROCESS_ERROR(dwBidderID != ERROR_ID);

    pOperation = (KAUCTION_BID_OPERATION*)KMemory::Alloc(sizeof(KAUCTION_BID_OPERATION));
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->byType      = eaoBid;
    pOperation->dwPlayerID  = dwBidderID;
    pOperation->uSize       = sizeof(KAUCTION_BID_OPERATION);
    pOperation->dwSaleID    = dwSaleID;
    pOperation->dwCRC       = dwCRC;
    pOperation->nPrice      = nPrice;

    m_OperationQueue.push_back(pOperation);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pOperation);
    }
    return bResult;
}

BOOL KAuctionManager::PostRequestSell(
    DWORD dwSellerID, char* pszSaleName, int nAucGenre, int nAucSub, int nRequireLevel,
    int nQuality, int nStartPrice, int nBuyItNowPrice, int nCustodyCharges, time_t nDurationTime,
    BYTE* pbyItemData, size_t uDataLen
)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KAUCTION_SELL_OPERATION*    pOperation  = NULL;
    KRole*                      pRole       = NULL;

    KGLOG_PROCESS_ERROR(pszSaleName);
    KGLOG_PROCESS_ERROR(pszSaleName[0] != '\0');
    KGLOG_PROCESS_ERROR(dwSellerID != ERROR_ID);
    KGLOG_PROCESS_ERROR(pbyItemData);
    KGLOG_PROCESS_ERROR(uDataLen > 0);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwSellerID);
    KGLOG_PROCESS_ERROR(pRole);

    pOperation = (KAUCTION_SELL_OPERATION*)KMemory::Alloc(sizeof(KAUCTION_SELL_OPERATION) + uDataLen);
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->byType          = eaoSell;
    pOperation->dwPlayerID      = dwSellerID;
    pOperation->uSize           = sizeof(KAUCTION_SELL_OPERATION) + uDataLen;

    strncpy(pOperation->szSaleName, pszSaleName, sizeof(pOperation->szSaleName));
    pOperation->szSaleName[sizeof(pOperation->szSaleName) - 1] = '\0';

    strncpy(pOperation->szSellerName, pRole->m_szName, sizeof(pOperation->szSellerName));
    pOperation->szSellerName[sizeof(pOperation->szSellerName) - 1] = '\0';

    pOperation->nAucGenre       = nAucGenre;
    pOperation->nAucSub         = nAucSub;
    pOperation->nRequireLevel   = nRequireLevel;
    pOperation->nQuality        = nQuality;
    pOperation->nStartPrice     = nStartPrice;
    pOperation->nBuyItNowPrice  = nBuyItNowPrice;
    pOperation->nCustodyCharges = nCustodyCharges;
    pOperation->nDurationTime   = nDurationTime;
    pOperation->uDataLen        = uDataLen;

    memcpy(pOperation->byItemData, pbyItemData, uDataLen);

    m_OperationQueue.push_back(pOperation);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pOperation);
    }
    return bResult;
}

BOOL KAuctionManager::PostRequestCancel(DWORD dwSellerID, DWORD dwSaleID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KAUCTION_CANCEL_OPERATION*  pOperation  = NULL;

    KGLOG_PROCESS_ERROR(dwSellerID != ERROR_ID);

    pOperation = (KAUCTION_CANCEL_OPERATION*)KMemory::Alloc(sizeof(KAUCTION_CANCEL_OPERATION));
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->byType      = eaoCancel;
    pOperation->dwPlayerID  = dwSellerID;
    pOperation->uSize       = sizeof(KAUCTION_CANCEL_OPERATION);
    pOperation->dwSaleID    = dwSaleID;

    m_OperationQueue.push_back(pOperation);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pOperation);
    }
    return bResult;
}

BOOL KAuctionManager::DoCheckDurationRequest()
{
    g_pSO3GameCenter->m_MiscDB.DoAuctionCheckDuration(g_pSO3GameCenter->m_nTimeNow);
    return true;
}

void KAuctionManager::ProcessLookupOperation(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_LOOKUP_OPERATION* pOperation = (KAUCTION_LOOKUP_OPERATION*)pbyData;

    g_pSO3GameCenter->m_MiscDB.DoAuctionLookup(
        pOperation->byRequestID, pOperation->dwPlayerID, &pOperation->Param
    );
}

void KAuctionManager::ProcessBidOperation(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_BID_OPERATION* pOperation = (KAUCTION_BID_OPERATION*)pbyData;

    g_pSO3GameCenter->m_MiscDB.DoAuctionBid(
        pOperation->dwPlayerID, pOperation->dwSaleID, pOperation->dwCRC, pOperation->nPrice
    );
}

void KAuctionManager::ProcessSellOperation(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_SELL_OPERATION* pOperation = (KAUCTION_SELL_OPERATION*)pbyData;

    g_pSO3GameCenter->m_MiscDB.DoAuctionSell(
        pOperation->dwPlayerID, pOperation->szSaleName, pOperation->szSellerName, pOperation->nAucGenre,
        pOperation->nAucSub, pOperation->nRequireLevel, pOperation->nQuality, pOperation->nStartPrice,
        pOperation->nBuyItNowPrice, pOperation->nCustodyCharges, pOperation->nDurationTime,
        pOperation->byItemData, pOperation->uDataLen
    );
}

void KAuctionManager::ProcessCancelOperation(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_CANCEL_OPERATION* pOperation = (KAUCTION_CANCEL_OPERATION*)pbyData;

    g_pSO3GameCenter->m_MiscDB.DoAuctionCancel(pOperation->dwPlayerID, pOperation->dwSaleID);

}

void KAuctionManager::OnLookupRespond(BYTE byRespondID, DWORD dwPlayerID, KAUCTION_PAGE_HEAD* pPage)
{
    BOOL                bRetCode    = false;
    KRole*              pSellerRole = NULL;
    KRole*              pBidderRole = NULL;
    BYTE*               pbyOffset   = NULL;
    size_t              uLeftSize   = 0;

    m_bBusy = false;

    assert(pPage);

    pbyOffset = pPage->bySaleList;
    uLeftSize = pPage->uSize;

    while (uLeftSize > 0)
    {
        KAUCTION_PAGE_NODE* pNode       = (KAUCTION_PAGE_NODE*)pbyOffset;
        size_t              uSaleSize   = sizeof(KAUCTION_PAGE_NODE) + pNode->uItemSize;

        KGLOG_PROCESS_ERROR(uLeftSize >= uSaleSize);
        pbyOffset += uSaleSize;
        uLeftSize -= uSaleSize;

        pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(pNode->dwSellerID);
        if (pSellerRole)
        {
            strncpy(pNode->szSellerName, pSellerRole->m_szName, sizeof(pNode->szSellerName));
            pNode->szSellerName[sizeof(pNode->szSellerName) - 1] = '\0';
        }

        pBidderRole = g_pSO3GameCenter->m_RoleManager.GetRole(pNode->dwBidderID);
        if (pBidderRole)
        {
            strncpy(pNode->szBidderName, pBidderRole->m_szName, sizeof(pNode->szBidderName));
            pNode->szBidderName[sizeof(pNode->szBidderName) - 1] = '\0';
        }
    }

    bRetCode = g_pSO3GameCenter->m_GameServer.DoAuctionLookupRespond(dwPlayerID, byRespondID, arcSucceed, pPage);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KAuctionManager::OnBidRespond(
    BYTE byCode, DWORD dwSellerID, int nSellPrice, DWORD dwBidderID, int nBidPrice,
    DWORD dwOldBidderID, int nOldBidPrice, char* pszSaleName, int nCustodyCharges,
    size_t uItemDataLen, BYTE* pbyItemData
)
{
    BOOL    bRetCode    = false;

    m_bBusy = false;

    if (byCode != arcSucceed)
    {
        goto Exit1;
    }

    if (nBidPrice >= nSellPrice)
    {
        // 1 把商品和找的钱 mailto 买家 通知买家获得道具
        SendItemToBidder(dwBidderID, dwSellerID, pszSaleName, nBidPrice, nSellPrice, uItemDataLen, pbyItemData);

        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwBidderID, amcAcquireItem, pszSaleName, nSellPrice);

        // 2 把卖的钱 mailto 卖家 通知卖家道具已卖出
        SendMoneyToSeller(dwSellerID, dwBidderID, pszSaleName, nSellPrice, nCustodyCharges);

        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwSellerID, amcItemSold, pszSaleName, nSellPrice);

        g_pSO3GameCenter->m_LogServerAgency.LogBuyAuctionItem(dwBidderID, dwSellerID, uItemDataLen, pbyItemData, nSellPrice);
    }
    else
    {
        // 通知卖家道具被出价
        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwSellerID, amcSomeoneBid, pszSaleName, nBidPrice);
    }

    // 把上一个bidder的钱退还 通知上一个出价人出价被超过
    if (dwOldBidderID != ERROR_ID)
    {
        SendMoneyToBidder(dwOldBidderID, pszSaleName, nOldBidPrice);

        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwOldBidderID, amcBidLost, pszSaleName, nOldBidPrice);
    }

Exit1:
    bRetCode = g_pSO3GameCenter->m_GameServer.DoAuctionBidRespond(dwBidderID, byCode, nBidPrice);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KAuctionManager::OnSellRespond(BYTE byCode)
{
    m_bBusy = false;

    KGLOG_PROCESS_ERROR(byCode == arcSucceed);

Exit0:
    return;
}

void KAuctionManager::OnCancelRespond(
    BYTE byCode, DWORD dwSellerID, char* pszSaleName, size_t uItemDataLen, BYTE* pbyItemData
)
{
    BOOL    bRetCode    = false;

    m_bBusy = false;

    if (byCode != arcSucceed)
    {
        goto Exit1;
    }

    // 发邮件给卖家退货
    SendItemToSeller(dwSellerID, pszSaleName, uItemDataLen, pbyItemData);

Exit1:
    bRetCode = g_pSO3GameCenter->m_GameServer.DoAuctionCancelRespond(dwSellerID, byCode);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KAuctionManager::OnCheckDurationRespond(
    char* pszSaleName, DWORD dwSellerID, DWORD dwBidderID, int nPrice, int nBuyItNowPrice,
    int nCustodyCharges, size_t uItemDataLen, BYTE* pbyItemData
)
{
    KRole* pSellerRole = NULL;

    if (dwBidderID == ERROR_ID)
    {
        // 退回给卖家，过期无人出价
        SendItemToSeller(dwSellerID, pszSaleName, uItemDataLen, pbyItemData);

        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwSellerID, amcTimeOver, pszSaleName, nPrice);

        pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwSellerID);
        if (pSellerRole)
        {
            g_pSO3GameCenter->m_StatDataManager.UpdateMoneyStat(pSellerRole, -nCustodyCharges, "OTHER");
        }
    }
    else
    {
        // 寄给买家商品
        SendItemToBidder(dwBidderID, dwSellerID, pszSaleName, nPrice, nBuyItNowPrice, uItemDataLen, pbyItemData);

        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwBidderID, amcAcquireItem, pszSaleName, nPrice);

        // 寄钱给卖家
        SendMoneyToSeller(dwSellerID, dwBidderID, pszSaleName, nPrice, nCustodyCharges);

        g_pSO3GameCenter->m_GameServer.DoAuctionMessageNotify(dwSellerID, amcItemSold, pszSaleName, nPrice);
    }
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

BOOL KAuctionManager::SendMoneyToSeller(
    DWORD dwSellerID, DWORD dwBuyerID, const char* pszSaleName, int nPrice, int nCustodyCharges
)
{
    BOOL                            bResult     = false;
    int                             nRetCode    = 0;
    KRole*                          pBuyerRole  = NULL;
    KRole*                          pSellerRole = NULL;
    int                             nTaxMoney   = 0;
    int                             nGetMoney   = 0;
    const KSO3GameCenterSettings&   rSettings   = g_pSO3GameCenter->m_Settings;
    char                            szText[MAX_MAIL_TEXT_LEN];
    char*                           pszText     = NULL;
    size_t                          uLeftSize   = 0;
    char                            szTitle[TITLE_LEN];
    size_t                          uTextLen    = 0;
    char                            szBuyerName[ROLE_NAME_LEN];

    assert(pszSaleName);
    KGLOG_PROCESS_ERROR(dwSellerID != ERROR_ID);

    pBuyerRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwBuyerID);
    szBuyerName[0] = '\0';
    if (pBuyerRole)
    {
        strncpy(szBuyerName,  pBuyerRole->m_szName, sizeof(szBuyerName));
        szBuyerName[sizeof(szBuyerName) - 1] = '\0';
    }

    nTaxMoney = (int)((double)nPrice * rSettings.m_nAuctionTaxRate / 1000);
    nGetMoney = nPrice + nCustodyCharges - nTaxMoney;
    snprintf(szTitle, sizeof(szTitle), "%s%s", rSettings.m_szSellSucceed, pszSaleName);
    szTitle[sizeof(szTitle) - 1] = '\0';

    pszText     = szText;
    uLeftSize   = sizeof(szText);

    nRetCode = snprintf(
        pszText, uLeftSize,
        "%s%s\n"
        "%s%s\n",
        rSettings.m_szSellSucceed, pszSaleName,
        rSettings.m_szBuyyer, szBuyerName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    nRetCode = _FormatMoney(pszText, uLeftSize, rSettings.m_szPrice, nPrice);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    nRetCode = _FormatMoney(pszText, uLeftSize, rSettings.m_szCustodyCharges, nCustodyCharges);
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
    uTextLen = strlen(szText) + 1;

    g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, dwSellerID, 0, eMailType_Auction, 
        rSettings.m_szAuctionName, szTitle, 
        nGetMoney, 
        (BYTE*)szText, uTextLen, 
        NULL, 0, 0
    );

    pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwSellerID);
    KGLOG_PROCESS_ERROR(pSellerRole);

    g_pSO3GameCenter->m_StatDataManager.UpdateMoneyStat(pSellerRole, -nTaxMoney, "OTHER");

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAuctionManager::SendItemToSeller(
    DWORD dwSellerID, const char* pszSaleName, size_t uItemDataLen, BYTE* pbyItemData
)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    const KSO3GameCenterSettings&   rSettings   = g_pSO3GameCenter->m_Settings;
    char                            szTitle[TITLE_LEN];
    char                            szText[]    = "";
    size_t                          uTextLen    = strlen(szText) + 1;

    assert(pszSaleName);
    assert(pbyItemData);
    KGLOG_PROCESS_ERROR(dwSellerID != ERROR_ID);

    snprintf(szTitle, sizeof(szTitle), "%s%s", rSettings.m_szCancelSucceed, pszSaleName);
    szTitle[sizeof(szTitle) - 1] = '\0';

    g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, dwSellerID, 0, eMailType_Auction, 
        rSettings.m_szAuctionName, szTitle, 
        0, 
        (BYTE*)szText, uTextLen, 
        pbyItemData, uItemDataLen, 0
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAuctionManager::SendMoneyToBidder(DWORD dwBidderID, const char* pszSaleName, int nMoney)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    const KSO3GameCenterSettings&   rSettings   = g_pSO3GameCenter->m_Settings;
    char                            szTitle[TITLE_LEN];
    char                            szText[]    = "";
    size_t                          uTextLen    = strlen(szText) + 1;

    assert(pszSaleName);
    KGLOG_PROCESS_ERROR(dwBidderID != ERROR_ID);

    snprintf(szTitle, sizeof(szTitle), "%s%s", g_pSO3GameCenter->m_Settings.m_szBidFailed, pszSaleName);
    szTitle[sizeof(szTitle) - 1] = '\0';

    g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, dwBidderID, 0, eMailType_Auction, 
        rSettings.m_szAuctionName, szTitle, 
        nMoney, 
        (BYTE*)szText, uTextLen, 
        NULL, 0, 0
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAuctionManager::SendItemToBidder(
    DWORD dwBidderID, DWORD dwSellerID, const char* pszSaleName, int nBidPrice,
    int nBuyItNowPrice, size_t uItemDataLen, BYTE* pbyItemData
)
{
    BOOL                            bResult         = false;
    int                             nRetCode        = 0;
    int                             nSellPrice      = 0;
    KRole*                          pSellerRole     = NULL;
    bool                            bBIN            = false;
    const KSO3GameCenterSettings&   rSettings       = g_pSO3GameCenter->m_Settings;
    char                            szTitle[TITLE_LEN];
    char*                           pszText         = NULL;
    size_t                          uLeftSize       = 0;
    size_t                          uTextLen        = 0;
    char                            szText[MAX_MAIL_TEXT_LEN];
    char                            szSellerName[ROLE_NAME_LEN];
    KCOMMON_ITEM_DATA*              pCommonItemData = (KCOMMON_ITEM_DATA*)pbyItemData;

    assert(pszSaleName);
    assert(pbyItemData);
    KGLOG_PROCESS_ERROR(dwBidderID != ERROR_ID);

    nSellPrice = min(nBidPrice, nBuyItNowPrice);
    bBIN = nBidPrice >= nBuyItNowPrice ? true : false;

    pSellerRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwSellerID);
    szSellerName[0] = '\0';
    if (pSellerRole)
    {
        strncpy(szSellerName, pSellerRole->m_szName, sizeof(szSellerName));
        szSellerName[sizeof(szSellerName) - 1] = '\0';
    }

    snprintf(szTitle, sizeof(szTitle), "%s%s", rSettings.m_szBuySucceed, pszSaleName);
    szTitle[sizeof(szTitle) - 1] = '\0';

    pszText     = szText;
    uLeftSize   = sizeof(szText);

    nRetCode = snprintf(
        pszText, uLeftSize,
        "%s%s%s\n"
        "%s%s\n",
        rSettings.m_szBuySucceed, pszSaleName, (nBidPrice >= nBuyItNowPrice ? rSettings.m_szBuyItNowPrice : rSettings.m_szTopPrice),
        rSettings.m_szSeller, szSellerName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    nRetCode = _FormatMoney(pszText, uLeftSize, rSettings.m_szPrice, nSellPrice);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
    pszText     += nRetCode;
    uLeftSize   -= nRetCode;

    szText[sizeof(szText) - 1] = '\0';
    uTextLen = strlen(szText) + 1;

    g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, dwBidderID, 0, eMailType_Auction, 
        rSettings.m_szAuctionName, szTitle, 
        nBidPrice - nSellPrice, 
        (BYTE*)szText, uTextLen,
        pbyItemData, uItemDataLen, 0
    );

    g_pSO3GameCenter->m_StatDataManager.UpdataAuctionItemStat(
        pCommonItemData->byTabType, pCommonItemData->wTabIndex, nSellPrice
    );

    bResult = true;
Exit0:
    return bResult;
}

