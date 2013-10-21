#include "stdafx.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KPlayerClient.h"
#include "KAuctionClient.h"

#ifdef _CLIENT
#include "SO3UI/SO3GameWorldUIHandler.h"

int KAuctionClient::LuaApplyLookup(Lua_State* L)
{
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    DWORD       dwNpcID         = ERROR_ID;
    const char* pszSaleName     = NULL;
    int         nAucGenre       = 0;
    int         nAucSub         = 0;
    int         nLevelMin       = 0;
    int         nLevelMax       = 0;
    int         nQuality        = 0;
    BYTE        byRequestID     = 0;
    const char* pszSellerName   = 0;
    DWORD       dwSellerID      = ERROR_ID;
    DWORD       dwBidderID      = ERROR_ID;
    int         nTopPrice       = 0;
    int         nAuction        = 0;
    int         nStartIndex     = 0;
    BYTE        byOrderType     = 0;
    BOOL        bDesc           = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 16);

    dwNpcID         = (DWORD)Lua_ValueToNumber(L, 1);
    byRequestID     = (BYTE)Lua_ValueToNumber(L, 2);

    pszSaleName     = Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszSaleName);

    nAucGenre       = (int)Lua_ValueToNumber(L, 4);
    nAucSub         = (int)Lua_ValueToNumber(L, 5);
    nLevelMin       = (int)Lua_ValueToNumber(L, 6);
    nLevelMax       = (int)Lua_ValueToNumber(L, 7);
    nQuality        = (int)Lua_ValueToNumber(L, 8);

    pszSellerName   = Lua_ValueToString(L, 9);
    KGLOG_PROCESS_ERROR(pszSellerName);

    dwSellerID      = (DWORD)Lua_ValueToNumber(L, 10);
    dwBidderID      = (DWORD)Lua_ValueToNumber(L, 11);
    nTopPrice       = (int)Lua_ValueToNumber(L, 12);
    nAuction        = (int)Lua_ValueToNumber(L, 13);
    nStartIndex     = (int)Lua_ValueToNumber(L, 14);
    byOrderType     = (BYTE)Lua_ValueToNumber(L, 15);
    bDesc           = (BOOL)Lua_ValueToNumber(L, 16);

    bRetCode = g_PlayerClient.DoAuctionLookupRequest(
        dwNpcID, byRequestID, pszSaleName, nAucGenre, nAucSub, nLevelMin, nLevelMax, nQuality, pszSellerName,
        dwSellerID, dwBidderID, nTopPrice, nAuction, nStartIndex, byOrderType, bDesc
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KAuctionClient::LuaGetLookupResult(Lua_State* L)
{
    int                 nResult     = 0;
    int                 nTopIndex   = 0;
    int                 nIndex      = 0;
    int                 nCount      = 0;
    KAUCTION_ITEM_LIST* pItemList   = NULL;
    int                 nSaleID     = 0;
    int                 nCRC        = 0;
    KItem*              pItem       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AUCTION_ITEM_LIST_COUNT);

    pItemList = &m_aItemListGroup[nIndex];
    assert(pItemList);

    Lua_PushNumber(L, pItemList->nTotalCount);

    Lua_NewTable(L);

    for (
        KAUCTION_ITEM_VECTOR::iterator it = pItemList->ItemVector.begin();
        it != pItemList->ItemVector.end();
        ++it
    )
    {
        Lua_PushNumber(L, ++nCount);
        Lua_NewTable(L);

        nSaleID = (int)it->dwSaleID;
        Lua_PushString(L, "ID");
        Lua_PushNumber(L, nSaleID);
        Lua_SetTable(L, -3);

        nCRC = (int)it->dwCRC;
        Lua_PushString(L, "CRC");
        Lua_PushNumber(L, nCRC);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "Price");
        Lua_PushNumber(L, it->nPrice);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "BuyItNowPrice");
        Lua_PushNumber(L, it->nBuyItNowPrice);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "SellerName");
        Lua_PushString(L, it->szSellerName);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "BidderName");
        Lua_PushString(L, it->szBidderName);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "LeftTime");
        Lua_PushNumber(L, it->nLeftTime);
        Lua_SetTable(L, -3);

        pItem = g_pSO3World->m_ItemSet.GetObj(it->dwItemID);
        KGLOG_PROCESS_ERROR(pItem);

        Lua_PushString(L, "Item");
        pItem->LuaGetObj(L);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

    nResult = 2;
Exit0:
    return nResult;
}

int KAuctionClient::LuaBid(Lua_State* L)
{
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    DWORD       dwNpcID         = ERROR_ID;
    DWORD       dwSaleID        = 0;
    DWORD       dwCRC           = 0;
    int         nPrice          = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwSaleID    = (DWORD)Lua_ValueToNumber(L, 2);
    dwCRC       = (DWORD)Lua_ValueToNumber(L, 3);
    nPrice      = (int)Lua_ValueToNumber(L, 4);

    bRetCode = g_PlayerClient.DoAuctionBidRequest(dwNpcID, dwSaleID, dwCRC, nPrice);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KAuctionClient::LuaSell(Lua_State* L)
{
    BOOL                    bRetCode        = false;
    int                     nTopIndex       = 0;
    DWORD                   dwNpcID         = ERROR_ID;
    DWORD                   dwBox           = 0;
    DWORD                   dwX             = 0;
    int                     nStartPrice     = 0;
    int                     nBuyItNowPrice  = 0;
    int                     nLeftHour       = 0;
    KItem*                  pItem           = NULL;
    KPlayer*                pPlayer         = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 6);

    dwNpcID         = (DWORD)Lua_ValueToNumber(L, 1);
    dwBox           = (DWORD)Lua_ValueToNumber(L, 2);
    dwX             = (DWORD)Lua_ValueToNumber(L, 3);
    nStartPrice     = (int)Lua_ValueToNumber(L, 4);
    nBuyItNowPrice  = (int)Lua_ValueToNumber(L, 5);
    nLeftHour       = (int)Lua_ValueToNumber(L, 6);

    KGLOG_PROCESS_ERROR(nStartPrice <= nBuyItNowPrice);
    KG_PROCESS_ERROR(dwBox >= ibPackage && dwBox <= ibPackage4);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pItem = pPlayer->m_ItemList.GetItem(dwBox, dwX);
    KGLOG_PROCESS_ERROR(pItem);

    KGLOG_PROCESS_ERROR(!pItem->IsBind());
    assert(pItem->m_pItemInfo);
    KGLOG_PROCESS_ERROR(pItem->m_pItemInfo->nExistType == ketPermanent);

    bRetCode = g_PlayerClient.DoAuctionSellRequest(
        dwNpcID, dwBox, dwX, nStartPrice, nBuyItNowPrice, nLeftHour
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
Exit0:
    return 0;
}

int KAuctionClient::LuaCancel(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwNpcID     = ERROR_ID;
    DWORD   dwSaleID    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);
    dwSaleID = (DWORD)Lua_ValueToNumber(L, 2);

    bRetCode = g_PlayerClient.DoAuctionCancelRequest(dwNpcID, dwSaleID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KAuctionClient)

    REGISTER_LUA_FUNC(KAuctionClient, ApplyLookup)
    REGISTER_LUA_FUNC(KAuctionClient, GetLookupResult)
    REGISTER_LUA_FUNC(KAuctionClient, Bid)
    REGISTER_LUA_FUNC(KAuctionClient, Sell)
    REGISTER_LUA_FUNC(KAuctionClient, Cancel)

DEFINE_LUA_CLASS_END(KAuctionClient)

#endif // _CLIENT

