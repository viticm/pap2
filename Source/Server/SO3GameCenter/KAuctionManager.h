#ifndef _KAUCTION_MANAGER_H_
#define _KAUCTION_MANAGER_H_

#define SYNC_SALE_LIST_MAX_LEN 6400

#include "KAuctionDef.h"
#include "KAuctionOperation.h"

#define MAX_AUCTION_LOOKUP_COUNT 1024

class KAuctionManager
{
public:
    KAuctionManager();
    ~KAuctionManager();

    BOOL Init();
    void UnInit();

    void Activate();

    // 查询
    BOOL PostRequestLookup(BYTE byRequestID, DWORD dwPlayerID, KAUCTION_LOOKUP_PARAM* pParam);

    // 出价
    BOOL PostRequestBid(DWORD dwBidderID, DWORD dwSaleID, DWORD dwCRC, int nPrice);

    // 卖
    BOOL PostRequestSell(
        DWORD dwSellerID, char* pszSaleName, int nAucGenre, int nAucSub, int nRequireLevel,
        int nQuality, int nStartPrice, int nBuyItNowPrice, int nCustodyCharges, time_t nDurationTime,
        BYTE* pbyItemData, size_t uDataLen
    );

    // 取消
    BOOL PostRequestCancel(DWORD dwSellerID, DWORD dwSaleID);

    // 检查商品是否到期
    BOOL DoCheckDurationRequest();

private:
    void ProcessLookupOperation(BYTE* pbyData, size_t uDataLen);
    void ProcessBidOperation(BYTE* pbyData, size_t uDataLen);
    void ProcessSellOperation(BYTE* pbyData, size_t uDataLen);
    void ProcessCancelOperation(BYTE* pbyData, size_t uDataLen);

public:
    void OnLookupRespond(BYTE byRespondID, DWORD dwPlayerID, KAUCTION_PAGE_HEAD* pSaleList);
    void OnBidRespond(
        BYTE byCode, DWORD dwSellerID, int nSellPrice, DWORD dwBidderID, int nBidPrice,
        DWORD dwOldBidderID, int nOldBidPrice, char* pszSaleName, int nCustodyCharges,
        size_t uItemDataLen, BYTE* pbyItemData
    );
    void OnSellRespond(BYTE byCode);
    void OnCancelRespond(
        BYTE byCode, DWORD dwSellerID, char* pszSaleName, size_t uItemDataLen, BYTE* pbyItemData
    );
    void OnCheckDurationRespond(
        char* pszSaleName, DWORD dwSellerID, DWORD dwBidderID, int nPrice, int nBuyItNowPrice,
        int nCustodyCharges, size_t uItemDataLen, BYTE* pbyItemData
    );

private:
    BOOL SendMoneyToSeller(
        DWORD dwSellerID, DWORD dwBuyerID, const char* pszSaleName, int nPrice, int nCustodyCharges
    );

    BOOL SendItemToSeller(
        DWORD dwSellerID, const char* pszSaleName, size_t uItemDataLen, BYTE* pbyItemData
    );

    BOOL SendMoneyToBidder(DWORD dwBidderID, const char* pszSaleName, int nMoney);

    BOOL SendItemToBidder(
        DWORD dwBidderID, DWORD dwSellerID, const char* pszSaleName, int nBidPrice,
        int nBuyItNowPrice, size_t uItemDataLen, BYTE* pbyItemData
    );

private:
    typedef void (KAuctionManager::*KAUCTION_OPERATION_FUNC)(BYTE*, size_t);
    KAUCTION_OPERATION_FUNC m_OperationFunc[eaoTotal];

    typedef std::list<void*, KMemory::KAllocator<void*> > KAUCTION_OPERATION_QUEUE;
    KAUCTION_OPERATION_QUEUE m_OperationQueue;

    BOOL    m_bBusy;
    time_t  m_nNextCheckWithdrawSalesTime;
};

#endif
