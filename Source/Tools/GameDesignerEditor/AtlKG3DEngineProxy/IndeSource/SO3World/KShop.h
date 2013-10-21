////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KShop.h
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2005-11-14 17:01:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSHOP_H_
#define _INCLUDE_KSHOP_H_

#include "Luna.h"
#include "KItem.h"
#include "GS_Client_Protocol.h"
#include "SO3Result.h"

////////////////////////////////////////////////////////////////////////////////

class KNpc;
class KPlayer;

struct KSHOP_ITEM
{
    KItem*     pItem;
    int        nCount;
    int        nItemTemplateIndex;
};

struct KSHOP_PAGE 
{
    KSHOP_ITEM ShopItems[MAX_SHOP_PAGE_ITEM_COUNT];
};

enum SHOP_TYPE
{
    eShopType_Invalid,
    eShopType_NPC,
    eShopType_PLAYER
};

struct KSHOP_BUY_ITEM_PARAM
{
    DWORD dwPageIndex;
    DWORD dwPosIndex;
    DWORD dwItemID;
    int   nCount;
    int   nCost;
};

struct KSHOP_SELL_ITEM_PARAM
{
    DWORD dwBox;
    DWORD dwX;
    DWORD dwItemID;
    int   nCost;
};

struct KSHOP_REPAIR_ITEM_PARAM
{
    DWORD dwBox;
    DWORD dwX;
    DWORD dwItemID;
    int   nCost;
};

struct KSHOP_BUY_SOLD_LIST_ITEM_PARAM
{
    DWORD dwItemID;
    DWORD dwX;
};

struct KNPC_SHOP_TEMPLATE_ITEM;

// 该结构作为商店的数据容器
class KShop
{
public:
    KShop();

#ifdef _CLIENT 
    BOOL UpdateItem(DWORD dwPageIndex, DWORD dwPosIndex, const KSHOP_ITEM &crShopItem);
#endif
    
    SHOP_SYSTEM_RESPOND_CODE CanBuyItem(KPlayer* pBuyer, KNpc* pSeller, KSHOP_BUY_ITEM_PARAM& rParam);

    int GetPlayerBuyCost(KPlayer* pPlayer, int nTemplateItemPrice, int nCount);
    int GetPlayerSellPrice(KPlayer* pPlayer, DWORD dwBox, DWORD dwX);
    int GetPlayerRepairPrice(KPlayer* pPlayer, DWORD dwBox, DWORD dwX);

#ifdef _SERVER
    BOOL BuyItem(KPlayer* pPlayer, KSHOP_BUY_ITEM_PARAM& Param);
    BOOL SellItem(KPlayer* pPlayer, KSHOP_SELL_ITEM_PARAM& Param);
    BOOL RepairItem(KPlayer* pPlayer, KSHOP_REPAIR_ITEM_PARAM& Param);
    BOOL RepairAllItems(KPlayer* pPlayer, int nCost);
    BOOL BuySoldListItem(KPlayer* pPlayer, KSHOP_BUY_SOLD_LIST_ITEM_PARAM& Param);

    void Refresh();
#endif

public:
    DWORD     m_dwTemplateID;
    char      m_szShopName[_NAME_LEN];
    DWORD     m_dwShopID;
    int       m_nShopType;
    DWORD     m_dwValidPage;    
    BOOL      m_bCanRepair;
    DWORD     m_dwNpcID;
    KNpc*     m_pNpc;

    KSHOP_PAGE m_ShopPages[MAX_SHOP_PAGES];
    
private:
    KSHOP_ITEM* GetShopItem(DWORD dwTabType, DWORD dwIndex, DWORD dwRandSeed);

public:
    BOOL DeleteItem(DWORD dwPageIndex, DWORD dwPosIndex);

public:
    DECLARE_LUA_CLASS(KShop);
    DECLARE_LUA_DWORD(TemplateID);
    DECLARE_LUA_STRING(ShopName, sizeof(m_szShopName));
    DECLARE_LUA_DWORD(ShopID);
    DECLARE_LUA_INTEGER(ShopType);
    DECLARE_LUA_DWORD(ValidPage);
    DECLARE_LUA_DWORD(NpcID);
    DECLARE_LUA_BOOL(CanRepair);
};
#endif //_INCLUDE_KSHOP_H_
