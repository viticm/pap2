#include "stdafx.h"
#include "KSO3World.h"
#include "KAuctionClient.h"
#include "Common/CRC32.h"

#ifdef _CLIENT

KAuctionClient::KAuctionClient()
{
}

KAuctionClient::~KAuctionClient()
{
}

BOOL KAuctionClient::UpdateItemList(int nIndex, KAUCTION_PAGE_HEAD* pPage)
{
    BOOL                bResult     = false;
    KAUCTION_ITEM_LIST* pItemList   = NULL;
    BYTE*               pbyOffset   = NULL;
    size_t              uLeftSize   = 0;

    assert(pPage);

    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AUCTION_ITEM_LIST_COUNT);
    pItemList = &m_aItemListGroup[nIndex];

    Clear(pItemList);

    pItemList->nTotalCount = pPage->nTotalCount;

    pbyOffset = pPage->bySaleList;
    uLeftSize = pPage->uSize;

    while (uLeftSize > 0)
    {
        KAUCTION_PAGE_NODE* pNode       = (KAUCTION_PAGE_NODE*)pbyOffset;
        size_t              uSaleSize   = sizeof(KAUCTION_PAGE_NODE) + pNode->uItemSize;
        KItem*              pItem       = NULL;
        KAUCTION_ITEM       AuctionItem;

        KGLOG_PROCESS_ERROR(uLeftSize >= uSaleSize);
        pbyOffset += uSaleSize;
        uLeftSize -= uSaleSize;

        AuctionItem.dwSaleID        = pNode->dwSaleID;

        strncpy(AuctionItem.szSellerName, pNode->szSellerName, sizeof(AuctionItem.szSellerName));
        AuctionItem.szSellerName[sizeof(AuctionItem.szSellerName) - 1] = '\0';

        strncpy(AuctionItem.szBidderName, pNode->szBidderName, sizeof(AuctionItem.szBidderName));
        AuctionItem.szBidderName[sizeof(AuctionItem.szBidderName) - 1] = '\0';

        AuctionItem.nPrice          = pNode->nPrice;
        AuctionItem.nBuyItNowPrice  = pNode->nBuyItNowPrice;
        AuctionItem.nLeftTime       = pNode->nLeftTime;

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pNode->byItemData, pNode->uItemSize);
        KGLOG_PROCESS_ERROR(pItem);

        AuctionItem.dwItemID        = pItem->m_dwID;
        AuctionItem.dwCRC           = (DWORD)CRC32(0, pNode->byItemData, (unsigned)pNode->uItemSize);

        pItemList->ItemVector.push_back(AuctionItem);
    }


    bResult = true;
Exit0:
    return bResult;
}

BOOL KAuctionClient::ClearAll()
{
    for (int i = 0; i < MAX_AUCTION_ITEM_LIST_COUNT; i++)
    {
        Clear(&m_aItemListGroup[i]);
    }
    return true;
}

BOOL KAuctionClient::Clear(KAUCTION_ITEM_LIST* pItemList)
{
    KItem*  pItem = NULL;

    assert(pItemList);

    for (KAUCTION_ITEM_VECTOR::iterator it = pItemList->ItemVector.begin(); it != pItemList->ItemVector.end(); ++it)
    {
        pItem = g_pSO3World->m_ItemSet.GetObj(it->dwItemID);
        if (pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }
        it->dwItemID = ERROR_ID;
    }

    pItemList->ItemVector.clear();
    pItemList->nTotalCount = 0;

    return true;
}
#endif // _CLIENT

