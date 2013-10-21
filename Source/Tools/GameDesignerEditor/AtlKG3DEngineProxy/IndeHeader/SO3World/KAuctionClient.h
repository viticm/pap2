#ifndef _KAUCTION_CLIENT_H_
#define _KAUCTION_CLIENT_H_

#ifdef _CLIENT

#include "Luna.h"

#define MAX_AUCTION_ITEM_LIST_COUNT 3

struct KAUCTION_ITEM
{
    DWORD   dwSaleID;
    char    szSellerName[ROLE_NAME_LEN];
    char    szBidderName[ROLE_NAME_LEN];
    int     nPrice;
    int     nBuyItNowPrice;
    int     nLeftTime;
    DWORD   dwItemID;
    DWORD   dwCRC;
};

typedef std::vector<KAUCTION_ITEM> KAUCTION_ITEM_VECTOR;

struct KAUCTION_ITEM_LIST 
{
    KAUCTION_ITEM_VECTOR    ItemVector;
    int                     nTotalCount;
};

class KAuctionClient
{
public:
    KAuctionClient();
    ~KAuctionClient();

    BOOL UpdateItemList(int nIndex, KAUCTION_PAGE_HEAD* pPage);
    BOOL ClearAll();

    int LuaApplyLookup(Lua_State* L);
    int LuaGetLookupResult(Lua_State* L);
    int LuaBid(Lua_State* L);
    int LuaSell(Lua_State* L);
    int LuaCancel(Lua_State* L);

private:
    BOOL Clear(KAUCTION_ITEM_LIST* pItemList);

    KAUCTION_ITEM_LIST m_aItemListGroup[MAX_AUCTION_ITEM_LIST_COUNT];

public:
    DECLARE_LUA_CLASS(KAuctionClient);
};

#endif // _CLIENT

#endif // _KAUCTION_CLIENT_H_

