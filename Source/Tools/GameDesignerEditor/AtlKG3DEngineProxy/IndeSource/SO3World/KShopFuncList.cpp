#include "stdafx.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KPlayerClient.h" 
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KShop.h"
#include "KNpc.h"

namespace KScriptFuncList
{
#if defined(_CLIENT)

int LuaOpenShopRequest(Lua_State *L)
{
    int         nResult         = ssrcInvalid;
    int         nRetCode        = 0;
    DWORD       dwShopID        = 0;
    DWORD       dwNpcID         = 0;
    KPlayer*    pPlayer         = NULL;
    KNpc*       pNpc            = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwShopID = (DWORD)Lua_ValueToNumber(L, 1);
    dwNpcID  = (DWORD)Lua_ValueToNumber(L, 2);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcTooFarAway);

    g_PlayerClient.DoOpenShopRequest(dwShopID);

    nResult = ssrcInvalid;
Exit0:
    if (nResult != ssrcInvalid)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond ShopRespond = { nResult, 0 };
            g_pGameWorldUIHandler->OnShopRespond(ShopRespond);
        }
    }
    return 0;
}

int LuaQueryShopPage(Lua_State *L)
{
    DWORD                       dwShopID        = 0;
    DWORD                       dwPageIndex     = 0;
    const KShop*                pcShop          = NULL;
    KItem*                      pItem           = NULL;
    KUIEventShopUpdateItem      UIShopUpdateItem;
    
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    dwShopID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 2);
    
    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);
    KGLOG_PROCESS_ERROR(dwPageIndex < pcShop->m_dwValidPage);    
    
    g_PlayerClient.DoQueryShopItems(dwShopID, dwPageIndex);

    // 如果物品信息没有改变，服务端不会回包，这里要通知更新UI
    for (int nIndex = 0; nIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nIndex)
    {
        pItem = pcShop->m_ShopPages[dwPageIndex].ShopItems[nIndex].pItem;
        if (pItem == NULL)
            continue;

        if (g_pGameWorldUIHandler)
        {
            UIShopUpdateItem.dwShopID    = dwShopID;
            UIShopUpdateItem.dwPageIndex = dwPageIndex;
            UIShopUpdateItem.dwPos       = (DWORD)nIndex;

            g_pGameWorldUIHandler->OnShopUpdateItem(UIShopUpdateItem);
        }
    }

Exit0:
    return 0;
}

int LuaGetShopItemID(Lua_State *L)
{
    int             nResult         = 0;
    int             nRetCode        = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPageIndex     = 0;
    DWORD           dwPosIndex      = 0;
    DWORD           dwItemID        = 0;
    const KShop    *pcShop          = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    dwShopID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 2);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 3);
   
    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);

    KGLOG_PROCESS_ERROR(dwPageIndex < pcShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);

    const KItem *pcItem = pcShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem;
    if (pcItem)
    {
        dwItemID = pcItem->m_dwID;
    }

    // 如果请求的格子里没有物品，则压入栈中的物品ID为0
    Lua_PushNumber(L, dwItemID);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetShopItemBuyPrice(Lua_State *L)
{
    int             nResult         = 0;
    int             nRetCode        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPageIndex     = 0;
    DWORD           dwPosIndex      = 0;
    int             nPrice          = 0;
    KShop*          pShop           = NULL;
    KPlayer*        pPlayer         = NULL;
    KNPC_SHOP_TEMPLATE_ITEM* pItemTemplate = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);
    
    dwNpcID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID     = (DWORD)Lua_ValueToNumber(L, 2);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 4);
    
    pShop  = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);    

    KGLOG_PROCESS_ERROR(dwPageIndex < pShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(pShop->m_dwTemplateID, pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].nItemTemplateIndex);
    KGLOG_PROCESS_ERROR(pItemTemplate);

    nPrice = pShop->GetPlayerBuyCost(pPlayer, pItemTemplate->nPrice, 1);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

    Lua_PushNumber(L, nPrice);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetShopItemBuyOtherInfo(Lua_State *L)
{
    int             nResult         = 0;
    int             nRetCode        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPageIndex     = 0;
    DWORD           dwPosIndex      = 0;
    KShop*          pShop           = NULL;
    KNPC_SHOP_TEMPLATE_ITEM* pItemTemplate = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);
    
    dwNpcID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID     = (DWORD)Lua_ValueToNumber(L, 2);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 4);
    
    pShop  = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    KGLOG_PROCESS_ERROR(dwPageIndex < pShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    
    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(
        pShop->m_dwTemplateID, pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].nItemTemplateIndex
    );
    KGLOG_PROCESS_ERROR(pItemTemplate);

    Lua_PushNumber(L, pItemTemplate->nPrestige);
    Lua_PushNumber(L, pItemTemplate->nContribution);

    nResult = 2;
Exit0:
    return nResult;
}

int LuaGetShopItemSellPrice(Lua_State* L)
{
    int             nRetCode        = 0;
    int             nResult         = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwBoxIndex      = 0;
    DWORD           dwPosIndex      = 0;    
    int             nPrice          = 0;
    KPlayer*        pPlayer         = NULL;
    KShop*          pShop           = NULL;
    KItem*          pItem           = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID    = (DWORD)Lua_ValueToNumber(L, 2);
    dwBoxIndex  = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex  = (DWORD)Lua_ValueToNumber(L, 4);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID); 
    KG_ASSERT_EXIT(pPlayer);
    
    pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(pItem);
    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    nPrice = pShop->GetPlayerSellPrice(pPlayer, dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

    // !!!下面这句是有问题的; 暗器就不适合   
    nPrice = nPrice * pItem->GetStackNum();

    Lua_PushNumber(L, nPrice);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetShopItemRepairPrice(Lua_State* L)
{
    int             nRetCode        = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwBoxIndex      = 0;
    DWORD           dwPosIndex      = 0;
    int             nPrice          = 0;
    KPlayer*        pPlayer         = NULL;
    KShop*          pShop           = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    dwNpcID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID   = (DWORD)Lua_ValueToNumber(L, 2);
    dwBoxIndex = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex = (DWORD)Lua_ValueToNumber(L, 4);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_ASSERT_EXIT(pPlayer);
    
    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    
    nPrice = pShop->GetPlayerRepairPrice(pPlayer, dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

Exit0:
    Lua_PushNumber(L, nPrice);
    return 1;
}

int LuaGetRepairAllItemsPrice(Lua_State* L)
{
    int             nRetCode    = 0;
    DWORD           dwShopID    = 0;
    DWORD           dwNpcID     = 0;
    int             nCost       = 0;
    KPlayer*        pPlayer     = NULL;
    KItem*          pItem       = NULL;
    KNpc*           pNpc        = NULL;
    KShop*          pShop       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwNpcID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID   = (DWORD)Lua_ValueToNumber(L, 2);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_ASSERT_EXIT(pPlayer);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    for (DWORD dwBoxIndex = ibEquip; dwBoxIndex < ibBank; ++dwBoxIndex)
    {
        DWORD dwBoxSize = pPlayer->m_ItemList.GetBoxSize(dwBoxIndex);

        for (DWORD dwX = 0; dwX < dwBoxSize; dwX++)
        {
            KItem* pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwX);
            if (pItem == NULL)
            {
                continue;
            }
            
            int nPrice = pShop->GetPlayerRepairPrice(pPlayer, dwBoxIndex, dwX);
            if (nPrice > 0)
            {
                nCost += nPrice;
            }                
        }
    }

Exit0:
    Lua_PushNumber(L, nCost);
    return 1;
}

int LuaGetShopItemCount(Lua_State* L)
{
    int             nResult         = 0;
    int             nRetCode        = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPageIndex     = 0;
    DWORD           dwPosIndex      = 0;
    int             nCount          = 0;
    const KShop    *pcShop         = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);
    
    dwShopID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 2);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 3);
 
    pcShop  = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);    
    
    KGLOG_PROCESS_ERROR(dwPageIndex < pcShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    KGLOG_PROCESS_ERROR(pcShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem);

    nCount = pcShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].nCount;
    Lua_PushNumber(L, nCount);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetShopItemReputeLevel(Lua_State* L)
{
    int             nResult         = 0;
    int             nRetCode        = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPageIndex     = 0;
    DWORD           dwPosIndex      = 0;
    KShop*          pShop           = NULL;
    KNPC_SHOP_TEMPLATE_ITEM* pItemTemplate = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    dwShopID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 2);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 3);

    pShop  = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);    

    KGLOG_PROCESS_ERROR(dwPageIndex < pShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    KGLOG_PROCESS_ERROR(pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem);
    
    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(
        pShop->m_dwTemplateID, pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].nItemTemplateIndex
    );
    KGLOG_PROCESS_ERROR(pItemTemplate);

    Lua_PushNumber(L, pItemTemplate->nReputeLevel);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetShopItemLimit(Lua_State *L)
{
    int             nResult         = 0;
    int             nRetCode        = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPageIndex     = 0;
    DWORD           dwPosIndex      = 0;
    KShop*          pShop           = NULL;
    KNPC_SHOP_TEMPLATE_ITEM* pItemTemplate = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);
    
    dwShopID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 2);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 3);

    pShop  = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);    
    
    KGLOG_PROCESS_ERROR(dwPageIndex < pShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    KGLOG_PROCESS_ERROR(pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem);
    
    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(
        pShop->m_dwTemplateID, pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].nItemTemplateIndex
    );
    KGLOG_PROCESS_ERROR(pItemTemplate);

    Lua_PushNumber(L, pItemTemplate->nLimit);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaBuyItem(Lua_State *L)
{
    SHOP_SYSTEM_RESPOND_CODE    nResult         = ssrcBuyFailed;
    SHOP_SYSTEM_RESPOND_CODE    eRetCode        = ssrcBuyFailed;
    DWORD                       dwNpcID         = 0;
    DWORD                       dwShopID        = 0;
    DWORD                       dwPageIndex     = 0;
    DWORD                       dwPosIndex      = 0;
    int                         nCount          = 0;
    KPlayer*                    pPlayer         = 0;
    KShop*                      pShop           = NULL;
    KNpc*                       pNpc            = NULL;
    KItem*                      pItem           = NULL;
    KNPC_SHOP_TEMPLATE_ITEM*    pItemTemplate   = NULL;
    KSHOP_BUY_ITEM_PARAM        Param;
    
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

    dwNpcID      = (DWORD)Lua_ValueToNumber(L, 1);      
    dwShopID     = (DWORD)Lua_ValueToNumber(L, 2);
    dwPageIndex  = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 4);
    nCount       = (int)Lua_ValueToNumber(L, 5);
    
    pShop  = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    KGLOG_PROCESS_ERROR(dwPageIndex < pShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    KGLOG_PROCESS_ERROR(nCount > 0);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_ASSERT_EXIT(pPlayer);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, ssrcYouDeath);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);
    
    pItem = pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem;
    KGLOG_PROCESS_ERROR(pItem);

    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(
        pShop->m_dwTemplateID, pShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].nItemTemplateIndex
    );
    KGLOG_PROCESS_ERROR(pItemTemplate);
    
    Param.dwItemID    = pItem->m_dwID;
    Param.dwPageIndex = dwPageIndex;
    Param.dwPosIndex  = dwPosIndex;
    Param.nCost       = pShop->GetPlayerBuyCost(pPlayer, pItemTemplate->nPrice, nCount);;
    Param.nCount      = nCount;

    eRetCode = pShop->CanBuyItem(pPlayer, pNpc, Param);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == ssrcBuySuccess, eRetCode);

    g_PlayerClient.DoShopBuyRequest(dwShopID, dwPageIndex, dwPosIndex, pItem->m_dwID, nCount, Param.nCost);

    nResult = ssrcBuySuccess;
Exit0:
    if (nResult != ssrcBuySuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond ShopRespond = { nResult, 0 };
            g_pGameWorldUIHandler->OnShopRespond(ShopRespond);
        }
    }
    return 0;
}

int LuaSellItem(Lua_State *L)
{
    int             nResult         = ssrcSellFailed;
    int             nRetCode        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwBoxIndex      = 0;
    DWORD           dwPosIndex      = 0;    
    int             nCount          = 0;
    DWORD           dwItemID        = 0;
    int             nCost           = 0; 
    int             nPrice          = 0;
    int             nBoxType        = 0;
    int             nRelation       = 0;    
    KPlayer*        pPlayer         = NULL;
    KItem*          pItem           = NULL;
    KShop*          pShop           = NULL;
    KNpc*           pNpc            = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 5);

    dwNpcID      = (DWORD)Lua_ValueToNumber(L,1);
    dwShopID     = (DWORD)Lua_ValueToNumber(L, 2);
    dwBoxIndex   = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex   = (DWORD)Lua_ValueToNumber(L, 4);
    nCount       = (int)Lua_ValueToNumber(L, 5);

 	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID); 
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, ssrcYouDeath);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcTooFarAway);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    KGLOG_PROCESS_ERROR(pShop->m_nShopType == eShopType_NPC);

    nBoxType = pPlayer->m_ItemList.GetBoxType(dwBoxIndex);
    KG_PROCESS_ERROR_RET_CODE(nBoxType == ivtPackage, ssrcItemNotInPackage);

	pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(pItem);
    KG_PROCESS_ERROR_RET_CODE(pItem->IsCanTrade(), ssrcCanNotSell);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);
    
    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    nPrice = pShop->GetPlayerSellPrice(pPlayer, dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

    dwItemID = pItem->m_dwID;
    nCost    = nPrice * nCount;

    g_PlayerClient.DoShopSellRequest(dwShopID, dwItemID, nCost, dwBoxIndex, dwPosIndex);
    
    nResult = ssrcSellSuccess;
Exit0:
    if (nResult != ssrcSellSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond ShopRespond = { nResult, 0 };
            g_pGameWorldUIHandler->OnShopRespond(ShopRespond);
        }
    }
    return 0;
}

int LuaBuySoldListItem(Lua_State *L)
{
    int             nResult         = ssrcBuyFailed;
    int             nRetCode        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwPackage       = 0;
    DWORD           dwX             = 0;
    int             nRelation       = 0; 
    const KShop*    pcShop          = NULL;
    KPlayer*        pPlayer         = NULL;
    KItem*          pItem           = NULL;
    KNpc*           pNpc            = NULL;
    DWORD           dwPosX          = 0;
    KItemInfo*      pItemInfo       = NULL;
    int             nItemAmount     = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID    = (DWORD)Lua_ValueToNumber(L, 2);
    dwX         = (DWORD)Lua_ValueToNumber(L, 3);
    
    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);
    KGLOG_PROCESS_ERROR(pcShop->m_nShopType == eShopType_NPC);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);  
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, ssrcYouDeath);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcTooFarAway);

    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    pItem = pPlayer->m_ItemList.GetItem(ibSoldList, dwX);
    KGLOG_PROCESS_ERROR(pItem);
    
    nRetCode = pPlayer->m_ItemList.FindFreeRoomInPackage(dwPackage, dwPosX);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcBagFull);

    nItemAmount = pPlayer->m_ItemList.GetItemTotalAmount(
        pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex
    );
    
    nItemAmount += pItem->GetStackNum();

    pItemInfo = pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);
    
    KG_PROCESS_ERROR_RET_CODE(
        pItemInfo->nMaxExistAmount == 0 || nItemAmount <= pItemInfo->nMaxExistAmount,
        ssrcItemExistLimit
    );

    g_PlayerClient.DoSoldListBuyRequest(dwShopID, dwX, pItem->m_dwID);

    nResult = ssrcBuySuccess;
Exit0:
    if (nResult != ssrcBuySuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond ShopRespond = { nResult, 0 };
            g_pGameWorldUIHandler->OnShopRespond(ShopRespond);
        }
    }
    return 0;
}

int LuaRepairItem(Lua_State *L)
{
    int             nResult         = ssrcRepairFailed;
    int             nRetCode        = 0;
    DWORD           dwShopID        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwBoxIndex      = 0;
    DWORD           dwPosIndex      = 0;
    int             nBoxType        = 0;
    BOOL            bIsRepairable   = false;
    int             nRelation       = 0;
    int             nPrice          = 0;
    int             nMoney          = 0;
    KShop*          pShop           = NULL;
    KPlayer*        pPlayer         = NULL;
    KItem*          pItem           = NULL;
    KNpc*           pNpc            = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    dwNpcID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID   = (DWORD)Lua_ValueToNumber(L, 2);
    dwBoxIndex = (DWORD)Lua_ValueToNumber(L, 3);
    dwPosIndex = (DWORD)Lua_ValueToNumber(L, 4);
    
    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    KGLOG_PROCESS_ERROR(pShop->m_bCanRepair);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, ssrcYouDeath);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcTooFarAway);

    nBoxType = pPlayer->m_ItemList.GetBoxType(dwBoxIndex);
    KGLOG_PROCESS_ERROR(nBoxType == ivtEquipment || nBoxType == ivtPackage);
  
    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(pItem);
    
    bIsRepairable = pItem->IsRepairable();
    KG_PROCESS_ERROR_RET_CODE(bIsRepairable, ssrcItemNotNeedRepair);

    nPrice = pShop->GetPlayerRepairPrice(pPlayer, dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

    nMoney = pPlayer->m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nMoney >= nPrice, ssrcNotEnoughMoney);

    g_PlayerClient.DoShopRepairRequest(dwShopID, pItem->m_dwID, nPrice, dwBoxIndex, dwPosIndex);
    
    nResult = ssrcRepairSuccess;
Exit0:
    if (nResult != ssrcRepairSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond ShopRespond = { nResult, 0 };
            g_pGameWorldUIHandler->OnShopRespond(ShopRespond);
        }
    }
    return 0;
}

int LuaRepairAllItems(Lua_State* L)
{
    int             nResult         = ssrcRepairFailed;
    int             nRetCode        = 0;
    DWORD           dwNpcID         = 0;
    DWORD           dwShopID        = 0;
    int             nCost           = 0;
    int             nMoney          = 0;
    KShop*          pShop           = NULL;
    KPlayer*        pPlayer         = NULL;
    KNpc*           pNpc            = NULL;
    int             nRelation       = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);
    
    dwNpcID  = (DWORD)Lua_ValueToNumber(L, 1);
    dwShopID = (DWORD)Lua_ValueToNumber(L, 2);
   
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, ssrcYouDeath);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    KGLOG_PROCESS_ERROR(pShop->m_bCanRepair);
   
    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcTooFarAway);

    for (DWORD dwBoxIndex = ibEquip; dwBoxIndex <= ibPackage4; ++dwBoxIndex)
    {
        DWORD dwBoxSize = pPlayer->m_ItemList.GetBoxSize(dwBoxIndex);

        for (DWORD dwX = 0; dwX < dwBoxSize; ++dwX)
        {
            KItem* pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwX);
            if (pItem == NULL)
            {
                continue;
            }
            
            int nPrice = pShop->GetPlayerRepairPrice(pPlayer, dwBoxIndex, dwX);
            if (nPrice > 0)
            {
                nCost += nPrice;
            }
        }
    }
    KG_PROCESS_ERROR_RET_CODE(nCost > 0, ssrcNoneItemNeedRepair);

    nMoney = pPlayer->m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nMoney >= nCost, ssrcNotEnoughMoney);
    
    g_PlayerClient.DoShopRepairRequest(dwShopID, ERROR_ID, nCost, 0, 0);

    nResult = ssrcRepairSuccess;
Exit0:
    if (nResult != ssrcRepairSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond ShopRespond = { nResult, 0 };
            g_pGameWorldUIHandler->OnShopRespond(ShopRespond);
        }
    }
    return 0;
}

#endif //_CLIENT

static KLuaFunc g_ShopFuncList[] = 
{
#if defined(_SERVER)
    {"",                    NULL},
#endif	//_SERVER

#if defined(_CLIENT)
    {"OpenShopRequest",             LuaOpenShopRequest},
    {"QueryShopPage",               LuaQueryShopPage},
    
    {"GetShopItemID",               LuaGetShopItemID},
    {"GetShopItemBuyPrice",         LuaGetShopItemBuyPrice},
    {"GetShopItemBuyOtherInfo",     LuaGetShopItemBuyOtherInfo},
    {"GetShopItemSellPrice",        LuaGetShopItemSellPrice},
    {"GetShopItemRepairPrice",      LuaGetShopItemRepairPrice},
    {"GetRepairAllItemsPrice",      LuaGetRepairAllItemsPrice},
    {"GetShopItemLimit",            LuaGetShopItemLimit},
    {"GetShopItemCount",            LuaGetShopItemCount},
    {"GetShopItemReputeLevel",      LuaGetShopItemReputeLevel},
    {"BuyItem",                     LuaBuyItem},
    {"SellItem",                    LuaSellItem},
    {"BuySoldListItem",             LuaBuySoldListItem},
    {"RepairItem",                  LuaRepairItem},
    {"RepairAllItems",              LuaRepairAllItems},
    
#endif //_CLIENT

    {0,                     0}
};

int GetShopFuncList(KLuaFunc*& pFuncList)
{
    pFuncList = g_ShopFuncList;
    return sizeof(g_ShopFuncList)/sizeof(KLuaFunc);
}
}
