////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KShop.cpp
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2005-11-17 14:50:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KShop.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KPlayerServer.h"
#include "KSO3World.h"

KShop::KShop()
{
#ifdef _SERVER
    m_dwTemplateID    = 0;
#endif

    m_dwShopID        = 0;
    m_nShopType       = eShopType_Invalid;
    m_dwValidPage     = 0;
    m_dwNpcID         = 0;
    m_pNpc            = NULL;
    m_bCanRepair      = false;
    memset(m_ShopPages, 0, sizeof(m_ShopPages));
}

#ifdef _CLIENT
BOOL KShop::UpdateItem(DWORD dwPageIndex, DWORD dwPosIndex, const KSHOP_ITEM &crShopItem)
{
    BOOL                bResult     = false;
    KSHOP_ITEM*         pShopItem   = NULL;
  
    KGLOG_PROCESS_ERROR(dwPageIndex < m_dwValidPage);
    KGLOG_PROCESS_ERROR(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);

    pShopItem = &m_ShopPages[dwPageIndex].ShopItems[dwPosIndex];
    *pShopItem = crShopItem;

    bResult = true;
Exit0:
    return bResult;
}
#endif

SHOP_SYSTEM_RESPOND_CODE KShop::CanBuyItem(KPlayer* pBuyer, KNpc* pSeller, KSHOP_BUY_ITEM_PARAM& rParam)
{
    SHOP_SYSTEM_RESPOND_CODE    nResult         = ssrcBuyFailed;
    int                         nRetCode        = 0;
    KSHOP_ITEM*                 pShopItem       = NULL;
    int                         nPlayerMoney    = 0;
    KNPC_SHOP_TEMPLATE_ITEM*    pItemTemplate   = NULL;

    KGLOG_PROCESS_ERROR(pBuyer);
    KGLOG_PROCESS_ERROR(pSeller);
    KGLOG_PROCESS_ERROR(rParam.dwPageIndex < m_dwValidPage);
    KGLOG_PROCESS_ERROR(rParam.dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    KGLOG_PROCESS_ERROR(rParam.nCount > 0);

    nRetCode = g_InRange(pBuyer, pSeller, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, ssrcTooFarAway);

    nRetCode = pBuyer->GetNpcRelation(pSeller);
    KGLOG_PROCESS_ERROR(!(nRetCode & sortEnemy));
    
    pShopItem = &m_ShopPages[rParam.dwPageIndex].ShopItems[rParam.dwPosIndex];
    KGLOG_PROCESS_ERROR(pShopItem->pItem);
    KGLOG_PROCESS_ERROR(pShopItem->pItem->m_dwID == rParam.dwItemID);

    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(m_dwTemplateID, pShopItem->nItemTemplateIndex);
    KGLOG_PROCESS_ERROR(pItemTemplate);
    KGLOG_PROCESS_ERROR(pItemTemplate->nType == (int)pShopItem->pItem->m_GenParam.dwTabType);
    KGLOG_PROCESS_ERROR(pItemTemplate->nIndex == (int)pShopItem->pItem->m_GenParam.dwIndex);

    nRetCode = pBuyer->m_ReputeList.GetReputeLevel(pSeller->m_dwForceID);
    KG_PROCESS_ERROR_RET_CODE(nRetCode >= pItemTemplate->nReputeLevel, ssrcNotEnoughRepate);
    
    nRetCode = pShopItem->pItem->GetMaxStackNum();
    KG_PROCESS_ERROR_RET_CODE(rParam.nCount <= nRetCode, ssrcBuyFailed);

    nRetCode = GetPlayerBuyCost(pBuyer, pItemTemplate->nPrice, rParam.nCount);
    KGLOG_PROCESS_ERROR(nRetCode == rParam.nCost);

    nPlayerMoney = pBuyer->m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nPlayerMoney >= nRetCode, ssrcNotEnoughMoney);
    
    if (pItemTemplate->nPrestige > 0)
    {
        if (pBuyer->m_eCamp == cNeutral)
        {
            KGLOG_PROCESS_ERROR(pBuyer->m_nCurrentPrestige == 0);
        }

        KG_PROCESS_ERROR_RET_CODE(
            pBuyer->m_nCurrentPrestige >= pItemTemplate->nPrestige, ssrcNotEnoughPrestige
        );
    }
    
    if (pItemTemplate->nContribution > 0)
    {
        KG_PROCESS_ERROR_RET_CODE(pBuyer->m_nContribution >= pItemTemplate->nContribution, ssrcNotEnoughContribution);
    }
    
    if (pItemTemplate->nRequireAchievementRecord > 0)
    {
        KG_PROCESS_ERROR_RET_CODE(pBuyer->m_nAchievementRecord >= pItemTemplate->nRequireAchievementRecord, ssrcAchievementRecordError);
    }

    if (pItemTemplate->nAchievementPoint > 0)
    {
        KG_PROCESS_ERROR_RET_CODE(pBuyer->m_nAchievementPoint >= pItemTemplate->nAchievementPoint, ssrcNotEnoughAchievementPoint);
    }

    if (pItemTemplate->nLimit != -1)
    {
        KGLOG_PROCESS_ERROR(pShopItem->nCount >= rParam.nCount);
    }
    
    nRetCode = pBuyer->m_ItemList.CanAddItem(pShopItem->pItem);
    KG_PROCESS_ERROR_RET_CODE(nRetCode != aircNotEnoughFreeRoom, ssrcBagFull);
    KG_PROCESS_ERROR_RET_CODE(nRetCode != aircItemAmountLimited, ssrcItemExistLimit);
    KG_PROCESS_ERROR(nRetCode == aircSuccess);
	
    nResult = ssrcBuySuccess;
Exit0:
    return nResult;
}

int KShop::GetPlayerBuyCost(KPlayer* pPlayer, int nTemplateItemPrice, int nCount)
{
    int         nResult     = -1;
    int         nPrice      = 0;
    KSHOP_ITEM* pShopItem   = NULL;
    
    KGLOG_PROCESS_ERROR(nCount > 0);
    
    KGLOG_PROCESS_ERROR(m_pNpc);
    KGLOG_PROCESS_ERROR(m_dwNpcID == m_pNpc->m_dwID);
    
    switch (m_nShopType)
    {
    case eShopType_NPC:
        {
            int nReputeLevel    = pPlayer->m_ReputeList.GetReputeLevel(m_pNpc->m_dwForceID);
            int nRebate         = g_pSO3World->m_ShopCenter.GetReputationRebate(m_pNpc->m_dwForceID, nReputeLevel);
            int nMaxRebate      = g_pSO3World->m_ShopCenter.GetMaxRebate(m_pNpc->m_dwForceID, nReputeLevel);
            long long llShopItemPrice = nTemplateItemPrice; // 防止溢出

            nRebate = max(nRebate, nMaxRebate);

            nPrice = (int)(llShopItemPrice * nRebate / KILO_NUM);
            KG_ASSERT_EXIT(nPrice >= 0);

            nPrice = max(nPrice, 1);
        }
        break;
    case eShopType_PLAYER:       
        nPrice = nTemplateItemPrice;
        break;
    default:
        break;
    }

    nResult = nPrice * nCount;
Exit0:
    return nResult;
}

int KShop::GetPlayerSellPrice(KPlayer* pPlayer, DWORD dwBox, DWORD dwX)
{
    int         nResult             = -1;
    long long   llPrice             = 0;
    KItem*      pItem               = NULL;
    int         nCurrentDurability  = 0;

    assert(pPlayer);

    pItem = pPlayer->m_ItemList.GetItem(dwBox, dwX);
    KGLOG_PROCESS_ERROR(pItem);

    switch (m_nShopType)
    {
    case eShopType_NPC:
        llPrice = pItem->GetPrice();
        KGLOG_PROCESS_ERROR(llPrice >= 0);
        // 如果物品掉耐久度,要扣除相应的费用(这里的修理费用不能根据声望来打折, 为了防止刷钱)
        
        nCurrentDurability = pItem->GetCurrentDurability();
        if (nCurrentDurability != -1 && pItem->m_nMaxDurability > 0)
        {
            llPrice = (llPrice * nCurrentDurability / pItem->m_nMaxDurability);
        }

        break;
    case eShopType_PLAYER:
        // TODO
        break;
    default:
        break;
    }

    nResult = (int)llPrice;
Exit0:
    return nResult;
}

int KShop::GetPlayerRepairPrice(KPlayer* pPlayer, DWORD dwBox, DWORD dwX)
{
    int     nResult     = -1;
    int     nPrice      = 0;
    KItem*  pItem       = NULL;
    BOOL    bRepairable = false;

    KGLOG_PROCESS_ERROR(pPlayer);

    pItem = pPlayer->m_ItemList.GetItem(dwBox, dwX);
    KGLOG_PROCESS_ERROR(pItem);

    bRepairable = pItem->IsRepairable();
    KG_PROCESS_ERROR(bRepairable);

    KGLOG_PROCESS_ERROR(m_pNpc);
    KGLOG_PROCESS_ERROR(m_dwNpcID == m_pNpc->m_dwID);
    
    switch (m_nShopType)
    {
    case eShopType_NPC:
        {
            int         nReputeLevel         = 0;
            int         nRebate              = 0;
            int         nMaxRebate           = 0;
            long long   llPricePerDurability = 0;

            nReputeLevel    = pPlayer->m_ReputeList.GetReputeLevel(m_pNpc->m_dwForceID);
            nRebate         = g_pSO3World->m_ShopCenter.GetReputationRebate(m_pNpc->m_dwForceID, nReputeLevel);
            nMaxRebate      = g_pSO3World->m_ShopCenter.GetMaxRebate(m_pNpc->m_dwForceID, nReputeLevel);
            
            nRebate = max(nRebate, nMaxRebate);
            
            if (pItem->m_Common.nLevel <= 70)
            {
                llPricePerDurability = (588 * pItem->m_Common.nLevel * pItem->m_Common.nLevel - 1765 * pItem->m_Common.nLevel + 19190) / 20000;
            }
            else
            {
                llPricePerDurability = (1385 + 2 * (pItem->m_Common.nLevel - 70)) / 10;
            }

            nPrice = (int)((pItem->m_nMaxDurability - pItem->m_nCurrentDurability) * llPricePerDurability * nRebate / KILO_NUM);
            KGLOG_PROCESS_ERROR(nPrice >= 0); 

            nPrice = max(nPrice, 1);
        }

        break;
    case eShopType_PLAYER:
        KG_ASSERT_EXIT(false);
        break;
    default:
        assert(false);
        break;
    }

    nResult = nPrice;
Exit0:
    return nResult;
}

#ifdef _SERVER
BOOL KShop::BuyItem(KPlayer* pPlayer, KSHOP_BUY_ITEM_PARAM& rParam)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    KSHOP_ITEM*                 pShopItem           = NULL;
    KItem*                      pItemBuy            = NULL; 
    BOOL                        bSyncEntireItem     = false;
    int                         nCost               = 0;
    KNPC_SHOP_TEMPLATE_ITEM*    pItemTemplate       = NULL;  

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(rParam.dwPageIndex < m_dwValidPage);
    KGLOG_PROCESS_ERROR(rParam.dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    
    KGLOG_PROCESS_ERROR(m_pNpc);
    KGLOG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwNpcID);
    
    bRetCode = CanBuyItem(pPlayer, m_pNpc, rParam);
    if (bRetCode != ssrcBuySuccess)
    {
        g_PlayerServer.DoSyncShopItem(
            pPlayer->m_nConnIndex, this, rParam.dwPageIndex, rParam.dwPosIndex, true
        );
        goto Exit0;
    }

    pShopItem = &m_ShopPages[rParam.dwPageIndex].ShopItems[rParam.dwPosIndex];
    KGLOG_PROCESS_ERROR(pShopItem->pItem);
    
    // ============== 执行购买 ============================>
    
    pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(m_dwTemplateID, pShopItem->nItemTemplateIndex);
    KGLOG_PROCESS_ERROR(pItemTemplate);

    if (pItemTemplate->nLimit != -1)
    {
        assert(rParam.nCount <= pShopItem->nCount);
        pShopItem->nCount -= rParam.nCount;
    }

    pItemBuy = g_pSO3World->m_ItemManager.CloneItem(pShopItem->pItem);
    KGLOG_PROCESS_ERROR(pItemBuy);

    if (pItemBuy->IsStackable())
    {
        bRetCode = pItemBuy->SetStackNum(rParam.nCount);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bRetCode = pPlayer->m_ItemList.AddMoney(-nCost);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pItemTemplate->nPrestige > 0)
    {
        bRetCode = pPlayer->AddPrestige(-pItemTemplate->nPrestige);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pItemTemplate->nContribution > 0)
    {
        bRetCode = pPlayer->AddContribution(-pItemTemplate->nContribution);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    if (pItemTemplate->nAchievementPoint > 0)
    {
        bRetCode = pPlayer->AddAchievementPoint(-pItemTemplate->nAchievementPoint);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bRetCode = pPlayer->m_ItemList.AddItem(pItemBuy);
    KGLOG_PROCESS_ERROR(bRetCode == aircSuccess);
    pItemBuy = NULL;

    // <============== 执行购买 ============================

    // 如果一个商店不为NPC商店或被购买的物品是限量的，则玩家购买后，该格子物品的数量肯定是改变了的
    if ((m_nShopType != eShopType_NPC) || (pItemTemplate->nLimit != -1))
    {
        if ((m_nShopType == eShopType_PLAYER) && (pShopItem->nCount == 0))
        {
            // 如果一商店为玩家商店，某物品被购买后的个数为0时，则需将物品从商店中去除
            bRetCode = DeleteItem(rParam.dwPageIndex, rParam.dwPosIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            bSyncEntireItem = true;
        }

        g_PlayerServer.DoSyncShopItem(
            pPlayer->m_nConnIndex, this, rParam.dwPageIndex, rParam.dwPosIndex, bSyncEntireItem
        );
    }

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pItemBuy)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItemBuy);
            pItemBuy = NULL;
        }
    }
    return bResult;
}

BOOL KShop::SellItem(KPlayer* pPlayer, KSHOP_SELL_ITEM_PARAM& rParam)
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    KItem*      pItem       = NULL;
    int         nPrice      = 0;
    int         nCount      = 0;
    int         nBoxType    = 0;
    int         nCost       = 0;
    int         nItemType   = 0;
    int         nMoney      = 0;
    int         nMaxMoney   = 0;
    DWORD       dwX         = 0;
    TItemPos    SrcPos;
    TItemPos    DestPos;

    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(m_pNpc);
    KGLOG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwNpcID);
    KG_PROCESS_ERROR(m_nShopType == eShopType_NPC);

    nBoxType = pPlayer->m_ItemList.GetBoxType(rParam.dwBox);
    KGLOG_PROCESS_ERROR(nBoxType == ivtEquipment || nBoxType == ivtPackage);

    // 确认玩家确实有这个物品(核对物品ID)
    pItem = pPlayer->m_ItemList.GetItem(rParam.dwBox, rParam.dwX);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(pItem->m_dwID == rParam.dwItemID);

    // 确认物品是否可卖: 物品"可交易"属性, 
    nRetCode = pItem->IsCanTrade();
    KGLOG_PROCESS_ERROR(nRetCode);

    nItemType = pItem->GetGenre();

    switch (nItemType)
    {
    case igEquipment:   // 装备
    case igPotion:      // 药品
    case igTaskItem:    // 任务
    case igMaterial:    // 材料
    case igBook:        // 书
        nCount = pItem->GetStackNum();
        KGLOG_PROCESS_ERROR(nCount > 0);
        break;
    default:
        KG_ASSERT_EXIT(!"未知类型的物品");
        break;
    }

    nPrice = GetPlayerSellPrice(pPlayer, rParam.dwBox, rParam.dwX);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

    nCost = nPrice * nCount;
    KG_ASSERT_EXIT(nCost >= 0);      // 溢出
    // 核对价值是否跟玩家看到的一致
    KGLOG_PROCESS_ERROR(nCost == rParam.nCost);

    // 确认玩家身上金钱不会超过上限
    nMoney = pPlayer->m_ItemList.GetMoney();
    nMaxMoney = pPlayer->m_ItemList.GetMoneyLimit();
    KGLOG_PROCESS_ERROR(nMoney <= nMaxMoney - nCost);    // 溢出检查

    // 转移物品和金钱
    switch (m_nShopType)
    {
    case eShopType_NPC:
        // 购回表
        nRetCode = pPlayer->m_ItemList.FindFreeSoldList(dwX);
        KGLOG_PROCESS_ERROR(nRetCode);

        SrcPos.dwBox = rParam.dwBox;
        SrcPos.dwX   = rParam.dwX;
        DestPos.dwBox = ibSoldList;
        DestPos.dwX   = dwX;
        
        pPlayer->m_ItemList.m_bSoldListOpened = true;
        nRetCode = pPlayer->m_ItemList.ExchangeItem(SrcPos, DestPos);
        KGLOG_PROCESS_ERROR(nRetCode == ircSuccess);
        pPlayer->m_ItemList.m_bSoldListOpened = false;

        nRetCode = pPlayer->m_ItemList.AddMoney(nCost);
        KG_ASSERT_EXIT(nRetCode);
        break;
    default:
        KG_ASSERT_EXIT(!"未定义的商店类型");
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KShop::RepairItem(KPlayer* pPlayer, KSHOP_REPAIR_ITEM_PARAM& rParam)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KItem*  pItem       = NULL;
    int     nBoxType    = 0;
    int     nCost       = 0;
    int     nMoney      = 0;

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(m_pNpc);
    KGLOG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwNpcID);

    // 确认商店是否可以修理该物品(玩家商店不能修理)
    KG_PROCESS_ERROR(m_nShopType == eShopType_NPC);
    KG_PROCESS_ERROR(m_bCanRepair);
    
    nBoxType = pPlayer->m_ItemList.GetBoxType(rParam.dwBox);
    KGLOG_PROCESS_ERROR(nBoxType == ivtEquipment || nBoxType == ivtPackage);

    // 确认玩家确实有这个物品(核对物品ID)
    pItem = pPlayer->m_ItemList.GetItem(rParam.dwBox, rParam.dwX);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(pItem->m_dwID == rParam.dwItemID);

    // 确认物品是否可修理
    KGLOG_PROCESS_ERROR(pItem->IsRepairable());

    // 取得物品价格, 并核对是否玩家看到的价格
    nCost = GetPlayerRepairPrice(pPlayer, rParam.dwBox, rParam.dwX);
    KGLOG_PROCESS_ERROR(nCost >= 0);

    if (nCost != rParam.nCost)
    {   
        // 同步玩家物品数据
        g_PlayerServer.DoSyncItemData(pPlayer->m_nConnIndex, pPlayer->m_dwID, pItem, rParam.dwBox, rParam.dwX);
        goto Exit0;
    }

    // 确认玩家身上金钱是否足够
    nMoney = pPlayer->m_ItemList.GetMoney();
    KGLOG_PROCESS_ERROR(nMoney >= nCost);
    // 修理物品
    bRetCode = pPlayer->m_ItemList.Repair(rParam.dwBox, rParam.dwX);
    KGLOG_PROCESS_ERROR(bRetCode);

    // 扣除金钱
    bRetCode = pPlayer->m_ItemList.AddMoney(-nCost);
    KGLOG_PROCESS_ERROR(bRetCode);
    // 不用同步了，在Repair内部就会同步了。
//    bRetCode = g_PlayerServer.DoUpdateItemAmount(
//        pPlayer->m_nConnIndex, rParam.dwBox, rParam.dwX, pItem->m_nCurrentDurability
//    );
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KShop::RepairAllItems(KPlayer* pPlayer, int nCost)
{
    BOOL bResult        = false;
    BOOL bRetCode       = false;
    int  nNeedCost      = 0;

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(m_pNpc);
    KGLOG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwNpcID);

    // 确认商店是否可以修理该物品(玩家商店不能修理)
    KG_PROCESS_ERROR(m_nShopType == eShopType_NPC);
    KG_PROCESS_ERROR(m_bCanRepair);
    
    KG_PROCESS_SUCCESS(nCost == 0);

    // 检测修理价格
    for (DWORD dwBoxIndex = 0; dwBoxIndex < ibTotal; ++dwBoxIndex)
    {
        int nBoxType = pPlayer->m_ItemList.GetBoxType(dwBoxIndex);
        if (nBoxType == ivtEquipment || nBoxType == ivtPackage)
        {
            DWORD dwBoxSize = pPlayer->m_ItemList.GetBoxSize(dwBoxIndex);

            for (DWORD dwX = 0; dwX < dwBoxSize; dwX++)
            {
                KItem* pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwX);
                if (!pItem)
                    continue;
                
                BOOL bIsRepairable = pItem->IsRepairable();
                if (!bIsRepairable)
                    continue;

                int nPrice = GetPlayerRepairPrice(pPlayer, dwBoxIndex, dwX);
                KGLOG_PROCESS_ERROR(nPrice >= 0);
                
                nNeedCost += nPrice;
                KG_ASSERT_EXIT(nNeedCost >= 0); // 溢出检测
            }
        }
    }

    KG_PROCESS_ERROR(nNeedCost == nCost);

    for (DWORD dwBoxIndex = 0; dwBoxIndex < ibTotal; ++dwBoxIndex)
    {
        int nBoxType = pPlayer->m_ItemList.GetBoxType(dwBoxIndex);
        if (nBoxType == ivtEquipment || nBoxType == ivtPackage)
        {
            DWORD dwBoxSize = pPlayer->m_ItemList.GetBoxSize(dwBoxIndex);

            for (DWORD dwX = 0; dwX < dwBoxSize; dwX++)
            {
                KItem* pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwX);
                if (!pItem)
                    continue;
            
                BOOL bIsRepairable = pItem->IsRepairable();
                if (!bIsRepairable)
                    continue;
                
                KSHOP_REPAIR_ITEM_PARAM Param;
                Param.dwBox = dwBoxIndex;
                Param.dwX = dwX;
                Param.dwItemID = pItem->m_dwID;
                Param.nCost = GetPlayerRepairPrice(pPlayer, dwBoxIndex, dwX);

                bRetCode = RepairItem(pPlayer, Param);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KShop::BuySoldListItem(KPlayer* pPlayer, KSHOP_BUY_SOLD_LIST_ITEM_PARAM& rParam)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KItem*      pItem       = NULL;
    int         nPrice      = 0;
    int         nCost       = 0;
    int         nMoney      = 0;
    DWORD       dwBoxIndex  = 0;
    DWORD       dwX         = 0;
    TItemPos    SrcPos;
    TItemPos    DestPos;
    int         nNext       = 0;
    KItemInfo*  pItemInfo   = NULL;
    int         nItemAmount = 0;

    KG_ASSERT_EXIT(pPlayer);
    KGLOG_PROCESS_ERROR(m_pNpc);
    KG_PROCESS_ERROR(m_nShopType == eShopType_NPC);

    // 检查物品是否有改变
    pItem = pPlayer->m_ItemList.GetItem(ibSoldList, rParam.dwX);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(pItem->m_dwID == rParam.dwItemID);

    // 检查玩家是否有足够的钱
    nPrice = GetPlayerSellPrice(pPlayer, ibSoldList, rParam.dwX);
    KGLOG_PROCESS_ERROR(nPrice >= 0);

    nCost = nPrice * pItem->GetStackNum();
    KG_ASSERT_EXIT(nCost >= 0);
    
    nMoney = pPlayer->m_ItemList.GetMoney();
    KGLOG_PROCESS_ERROR(nCost <= nMoney);

    bRetCode = pPlayer->m_ItemList.FindFreeRoomInPackage(dwBoxIndex, dwX);
    KGLOG_PROCESS_ERROR(bRetCode);

    nItemAmount = pPlayer->m_ItemList.GetItemTotalAmount(
        pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex
    );

    pItemInfo = pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    KG_PROCESS_ERROR(pItemInfo->nMaxExistAmount == 0 || (nItemAmount + pItem->GetStackNum()) <= pItemInfo->nMaxExistAmount);

    SrcPos.dwBox = ibSoldList;
    SrcPos.dwX = rParam.dwX;
    DestPos.dwBox = dwBoxIndex;
    DestPos.dwX = dwX;
    
    pPlayer->m_ItemList.m_bSoldListOpened = true;

    bRetCode = pPlayer->m_ItemList.ExchangeItem(SrcPos, DestPos);
    KGLOG_PROCESS_ERROR(bRetCode == ircSuccess);

    bRetCode = pPlayer->m_ItemList.AddMoney(-nCost);
    KGLOG_PROCESS_ERROR(bRetCode);

    // 如果玩家卖回的物品是回购列表中间的某项，需整理回购包，以保证KItemList::FindFreeSoldList()的正确性。
    nNext = (rParam.dwX + 1) >= MAX_SOLDLIST_PACKAGE_SIZE ? 0 : (rParam.dwX + 1);
    pItem = pPlayer->m_ItemList.GetItem(ibSoldList, nNext);
    if (pItem)
    {
        bRetCode = pPlayer->m_ItemList.TidyUpSoldList();
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    pPlayer->m_ItemList.m_bSoldListOpened = false;

    bResult = true;
Exit0:
    return bResult;
}

void KShop::Refresh()
{
    KNPC_SHOP_TEMPLATE_ITEM* pItemTemplate = NULL;

    for (DWORD dwPageIndex = 0; dwPageIndex < m_dwValidPage; ++dwPageIndex)
    {
        for (DWORD dwPosIndex = 0; dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++dwPosIndex)
        {
            KSHOP_ITEM* pShopItem = &(m_ShopPages[dwPageIndex].ShopItems[dwPosIndex]);
            
            if (!pShopItem)
                continue;

            if (!pShopItem->pItem)
                continue;
            
            pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(m_dwTemplateID, pShopItem->nItemTemplateIndex);
            KGLOG_PROCESS_ERROR(pItemTemplate);

            if (pShopItem->nCount == pItemTemplate->nLimit)
                continue;
            
            pShopItem->nCount = pItemTemplate->nLimit;
        }
    }

Exit0:
    return;
}
#endif

BOOL KShop::DeleteItem(DWORD dwPageIndex, DWORD dwPosIndex)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KItem*      pItem       = NULL;

    assert(dwPageIndex < MAX_SHOP_PAGES);
    assert(dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT);
    
    pItem = m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem;
    KG_PROCESS_SUCCESS(!pItem);
    
    bRetCode = g_pSO3World->m_ItemManager.FreeItem(pItem);
    KGLOG_PROCESS_ERROR(bRetCode);
    m_ShopPages[dwPageIndex].ShopItems[dwPosIndex].pItem = NULL;
  
    if (m_dwValidPage == (dwPageIndex + 1))
    {
        BOOL bHadItemFlag = false;
        for (int nPosIndex = 0; nPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nPosIndex)
        {
            if (m_ShopPages[dwPageIndex].ShopItems[nPosIndex].pItem)
            {
                bHadItemFlag = true;
                break;
            }
        }
        
        if ((!bHadItemFlag) && m_dwValidPage > 0)
        {
            m_dwValidPage = m_dwValidPage - 1;
        }
    }

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "[Shop] Try to delete a item from shop %u failed; PageIndex = %u,  PosIndex = %u", 
            m_dwShopID, dwPageIndex, dwPosIndex
        );
    }
    else
    {
        KGLogPrintf(
            KGLOG_INFO, 
            "[Shop] delete a item from shop %u success; PageIndex = %u,  PosIndex = %u", 
            m_dwShopID, dwPageIndex, dwPosIndex
        );
    }
    return bResult;
}

KSHOP_ITEM* KShop::GetShopItem(DWORD dwTabType, DWORD dwIndex, DWORD dwRandSeed)
{
    KSHOP_ITEM* pResult     = NULL;
    KItem*      pItem       = NULL;
    KSHOP_ITEM* pShopItem   = NULL;

    for (DWORD dwPageIndex = 0; dwPageIndex < m_dwValidPage; ++dwPageIndex)
    {
        for (DWORD dwPosIndex = 0; dwPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++dwPosIndex)
        {
            pShopItem = &m_ShopPages[dwPageIndex].ShopItems[dwPosIndex];
            
            pItem = pShopItem->pItem;
            if (pItem == NULL)
            {
                continue;
            }
            
            if (
                pItem->m_GenParam.dwTabType != dwTabType    || 
                pItem->m_GenParam.dwIndex != dwIndex        || 
                pItem->m_GenParam.dwRandSeed != dwRandSeed
            )
            {
                continue;
            }

            pResult = pShopItem;

            goto Exit0;
        }
    }

Exit0:
    return pResult;
}
