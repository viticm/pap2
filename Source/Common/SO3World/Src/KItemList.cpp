#include "stdafx.h"
#include "KPlayerServer.h"
#include "KItemList.h"
#include "KRoleDBDataDef.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "KItem.h"
#include "KPlayer.h"
#include "KItemLib.h"
#ifdef _CLIENT
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#endif

// 装备栏规则表
EQUIPMENT_SUB_TYPE g_CanEquipList[eitTotal] =
{
    estMeleeWeapon, estRangeWeapon, estChest,   estHelm,       estAmulet,  estRing,        estRing,    
    estWaist,       estPendant,     estPants,   estBoots,      estBangle,  estWaistExtend, estPackage, 
    estPackage,     estPackage,     estPackage, estPackage,    estPackage, estPackage,     estPackage,
	estPackage,     estArrow,       estBackExtend, estHorse
};

int g_DefaultBoxSize[ibTotal] = { 
    eitTotal,  FIRST_PACKAGE_SIZE, 0, 0, 0, 0, INIT_BANK_SIZE, 
    0, 0, 0, 0, 0, MAX_SOLDLIST_PACKAGE_SIZE
};

BOOL g_IsPackage(DWORD dwBoxIndex)
{
    return dwBoxIndex >= ibPackage && dwBoxIndex <= ibPackage4;
}

BOOL g_IsBank(DWORD dwBoxIndex)
{
    return dwBoxIndex >= ibBank && dwBoxIndex <= ibBankPackage5;
}

DWORD g_GetBoxIndexFromSlot(DWORD dwSlot)
{
	DWORD nResult = 0;
	switch(dwSlot)
	{
	case eitPackage1:
		nResult = ibPackage1;
		break;
	case eitPackage2:
		nResult = ibPackage2;
		break;
	case eitPackage3:
		nResult = ibPackage3;
		break;
	case eitPackage4:
		nResult = ibPackage4;
		break;
	case eitBankPackage1:
		nResult = ibBankPackage1;
		break;
	case eitBankPackage2:
		nResult = ibBankPackage2;
		break;
	case eitBankPackage3:
		nResult = ibBankPackage3;
		break;
	case eitBankPackage4:
		nResult = ibBankPackage4;
		break;
	case eitBankPackage5:
		nResult = ibBankPackage5;
		break;
	default:
		break;
	}

	return nResult;
}

BOOL KItemList::Init(KPlayer* pPlayer)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    assert(pPlayer);

    m_nMoney            = 0;
    m_nMoneyLimit       = MONEY_LIMIT;

    m_bBankOpened       = false;

#ifdef _SERVER
    m_bSoldListOpened   = false;
    m_bFinishLoadData   = false;
#endif

    m_nEnabledBankPackageCount = 0;

    m_pPlayer = pPlayer;
    m_nNextSoldListPos = 0;

    for (int i = ibEquip; i < ibTotal; ++i)
    {
        bRetCode = m_Box[i].Init(g_DefaultBoxSize[i]);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    m_dwBoxItemID = ERROR_ID;

    bResult = true;
Exit0:
    return bResult;
}

void KItemList::UnInit()
{
    for (int i = 0; i < ibTotal; i++)
    {
        m_Box[i].UnInit();
    }

    ClearBox();
}

#ifdef _SERVER
void KItemList::Activate()
{    
    if ((g_pSO3World->m_nGameLoop - m_pPlayer->m_dwID) % (30 * GAME_FPS) != 0)
    {
        return;
    }

    for (DWORD dwBoxIndex = 0; dwBoxIndex < ibTotal; dwBoxIndex++)
    {
        if (dwBoxIndex == ibSoldList)
        {
            continue;
        }

        for (DWORD dwX = 0; dwX < m_Box[dwBoxIndex].m_dwSize; dwX++)
        {
            KItem* pItem = m_Box[dwBoxIndex].GetItem(dwX);
            
            if (pItem == NULL)
            {
                continue;
            }

            if (pItem->m_nEnchantTime > 0 && g_pSO3World->m_nCurrentTime > pItem->m_nEnchantTime)
            {
                RemoveEnchant(dwBoxIndex, dwX, eiTemporaryEnchant);
                pItem->m_nEnchantTime = 0;
            }
            
            assert(pItem->m_pItemInfo);

            if (pItem->m_pItemInfo->nExistType == ketOnLine || pItem->m_pItemInfo->nExistType == ketOnlineAndOffLine)
            {
                ITEM_RESULT_CODE eRetCode = ircFailed;
                TItemPos ItemPos = {dwBoxIndex, dwX};
                KITEM_LOG_INFO  ItemLogInfo;

                if (pItem->m_GenParam.nGenTime + pItem->m_pItemInfo->nMaxExistTime > g_pSO3World->m_nCurrentTime)
                {
                    continue;
                }

                g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);
                eRetCode = DestroyItem(ItemPos);
                KGLOG_CHECK_ERROR(eRetCode == ircSuccess);
                if (eRetCode == ircSuccess)
                {
                    g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, &ItemLogInfo, "item exist time over");
                }
            }
        }
    }

    return;
}
#endif

ADD_ITEM_RESULT_CODE KItemList::CanAddItem(KItem* pItem, BOOL bUseOnlySpecialBox /*= false*/)
{
    ADD_ITEM_RESULT_CODE    nResult         = aircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode        = aircFailed;
    BOOL                    bRetCode        = false;
    int                     nAddAmount      = 0;
    int                     nExistAmount    = 0;
    int                     nMaxExistAmount = 0;
    int                     nCanPushAmount  = 0;
    KItem*                  pDestItem       = NULL;

    assert(pItem);

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(m_bFinishLoadData);
#endif

    if (pItem->IsStackable())
    {
        nAddAmount = pItem->GetStackNum();
    
        nExistAmount = GetItemTotalAmount(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex);
        nMaxExistAmount = pItem->GetMaxExistAmount();

        KG_PROCESS_ERROR_RET_CODE(nMaxExistAmount == 0 || (nExistAmount + nAddAmount) <= nMaxExistAmount, aircItemAmountLimited);

        for (DWORD dwBox = ibPackage; dwBox <= ibPackage4; ++dwBox)
        {
            if (bUseOnlySpecialBox)
            {
                if (m_Box[dwBox].m_nContainItemGenerType == INVALID_CONTAIN_ITEM_TYPE)
                    continue;

                bRetCode = m_Box[dwBox].CheckContainItemType(pItem);
                if (!bRetCode)
                    continue;
            }

            for (DWORD dwX = 0; dwX < m_Box[dwBox].m_dwSize; ++dwX)
            {
                pDestItem = m_Box[dwBox].m_pItemArray[dwX];
                if (pDestItem && pDestItem->CanStack(pItem))
                {
                    nCanPushAmount += pDestItem->GetMaxStackNum() - pDestItem->GetStackNum();
                    KG_PROCESS_SUCCESS(nCanPushAmount >= nAddAmount);
                }
            }
        }
    }
    
    for (DWORD dwIndex = ibPackage; dwIndex <= ibPackage4; ++dwIndex)
    {
        KInventory* pBox = &m_Box[dwIndex];
        DWORD       dwX = 0;

        if (bUseOnlySpecialBox)
        {
            if (pBox->m_nContainItemGenerType == INVALID_CONTAIN_ITEM_TYPE)
                continue;

            bRetCode = pBox->CheckContainItemType(pItem);
            if (!bRetCode)
                continue;
        }

        dwX = GetEmptyPos(dwIndex);
        if (dwX == UINT_MAX)
            continue;
        
        eRetCode = CanAddItemToPos(pItem, dwIndex, dwX);
        KG_PROCESS_ERROR_RET_CODE(eRetCode != aircItemAmountLimited, eRetCode);

        if (eRetCode != aircSuccess)
            continue;

        goto Exit1;
    }
    
    KG_PROCESS_ERROR_RET_CODE(false, aircNotEnoughFreeRoom);

Exit1:
    nResult = aircSuccess;
Exit0:
    return nResult;
}

ADD_ITEM_RESULT_CODE KItemList::CanAddItemToPos(KItem* pItem, DWORD dwDestBox, DWORD dwDestX)
{
    ADD_ITEM_RESULT_CODE	nResult             = aircFailed;
    ITEM_RESULT_CODE        eRetCode            = ircFailed;
    BOOL                    bRetCode            = false;
    int                     nAddAmount          = 0;
    int                     nExistAmount        = 0;
    int                     nMaxExistAmount     = 0;
    KItem*                  pDestItem           = NULL;

    assert(pItem);

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(m_bFinishLoadData);
#endif
    KGLOG_PROCESS_ERROR(dwDestBox < ibTotal);
    KGLOG_PROCESS_ERROR(dwDestX < m_Box[dwDestBox].m_dwSize);

    nAddAmount = pItem->GetStackNum();
    nExistAmount = GetItemTotalAmount(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex);
    nMaxExistAmount = pItem->GetMaxExistAmount();

    KG_PROCESS_ERROR_RET_CODE(nMaxExistAmount == 0 || (nExistAmount + nAddAmount) <= nMaxExistAmount, aircItemAmountLimited);
    
    pDestItem = GetItem(dwDestBox, dwDestX);
    if (pDestItem == NULL)
    {
        eRetCode = CanExchangeItemToPos(pItem, dwDestBox, dwDestX);
        KG_PROCESS_ERROR(eRetCode == ircSuccess);
    }
    else
    {
        int nStackNum       = 0;
        int nDestStackNum   = 0;
        int nMaxStackNum    = 0;
        
        KGLOG_PROCESS_ERROR(dwDestBox >= ibPackage && dwDestBox <= ibPackage4);

        bRetCode = m_Box[dwDestBox].CheckContainItemType(pItem);
        KG_PROCESS_ERROR(bRetCode);

        bRetCode = pDestItem->CanStack(pItem);
        KG_PROCESS_ERROR(bRetCode);

        nStackNum       = pItem->GetStackNum();
        nDestStackNum   = pDestItem->GetStackNum();
        nMaxStackNum    = pDestItem->GetMaxStackNum();    
        KG_PROCESS_ERROR(nStackNum + nDestStackNum <= nMaxStackNum);
    }

    nResult = aircSuccess;
Exit0:
    return nResult;
}

ADD_ITEM_RESULT_CODE KItemList::AddItem(KItem* pItem, BOOL bNotifyClient)
{
    ADD_ITEM_RESULT_CODE     nResult        = aircFailed;
    ADD_ITEM_RESULT_CODE     eRetCode       = aircFailed;

    assert(pItem);

    eRetCode = AddItemInternal(pItem, bNotifyClient, true);
    KG_PROCESS_SUCCESS(eRetCode == aircSuccess);

    eRetCode = AddItemInternal(pItem, bNotifyClient);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);

Exit1:
    nResult = aircSuccess;
Exit0:
    return nResult;
}

ADD_ITEM_RESULT_CODE KItemList::AddItemToPos(KItem* pItem, DWORD dwBoxIndex, DWORD dwX)
{
    ADD_ITEM_RESULT_CODE    nResult    = aircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode   = aircFailed;
    BOOL                    bRetCode   = false;
    KCustomEquipInfo*       pEquipInfo = NULL;
    KItem*                  pDestItem  = NULL;

    assert(pItem);

    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

#ifdef _SERVER
    eRetCode = CanAddItemToPos(pItem, dwBoxIndex, dwX);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);
    
    pDestItem = GetItem(dwBoxIndex, dwX);
    if (pDestItem)
    {
        int nStackNum = 0;
        int nRetCode  = 0;
        int nDestStackNum = 0;

        assert(dwBoxIndex >= ibPackage && dwBoxIndex <= ibPackage4);
        
        nStackNum = pItem->GetStackNum();
        nRetCode = pDestItem->Stack(nStackNum);
        assert(nRetCode == 0);
        nDestStackNum = pDestItem->GetStackNum();

        g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, dwBoxIndex, dwX, nDestStackNum);
        
        if (pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }

        goto Exit1;
    }
#endif

    bRetCode = m_Box[dwBoxIndex].PlaceItem(pItem, dwX);
    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, dwBoxIndex, dwX);
#endif

    if (dwBoxIndex == ibEquip)
    {
        DWORD dwSlotBox = 0;

        bRetCode = Equip(pItem, dwX);
        KGLOG_CHECK_ERROR(bRetCode);

		dwSlotBox = g_GetBoxIndexFromSlot(dwX);
		if (dwSlotBox)
		{
            KGLOG_PROCESS_ERROR(
                pItem->m_GenParam.dwTabType == ittCustWeapon    || 
                pItem->m_GenParam.dwTabType == ittCustArmor     ||
                pItem->m_GenParam.dwTabType == ittCustTrinket 
            );

            pEquipInfo = (KCustomEquipInfo*)pItem->GetItemInfo();
			KGLOG_PROCESS_ERROR(pEquipInfo);
            KGLOG_PROCESS_ERROR(pEquipInfo->nSub == estPackage);
            EnablePackage(dwSlotBox, pItem->m_nPackageSize, pEquipInfo->nPackageGenerType, pEquipInfo->nPackageSubType);
		}
    }

Exit1:
    nResult = aircSuccess;
Exit0:
    return nResult;
}

ADD_ITEM_RESULT_CODE KItemList::AddItemInternal(KItem* pItem, BOOL bNotifyClient, BOOL bUseOnlySpecialBox)
{
    ADD_ITEM_RESULT_CODE     nResult        = aircFailed;
    ADD_ITEM_RESULT_CODE     eRetCode       = aircFailed;
    BOOL                     bRetCode       = false;
    int	                     nPlace         = 0;
    int                      nItemAmount    = 0;
    int                      nCanPushAmount = 0;
    KItem*                   pDestItem      = NULL;

    assert(m_pPlayer);
    assert(pItem);
    
    eRetCode = CanAddItem(pItem, bUseOnlySpecialBox);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);
    
    if (pItem->IsStackable())
    {
        int nAddAmount = pItem->GetStackNum();

        for (DWORD dwBox = ibPackage; dwBox <= ibPackage4; ++dwBox)
        {
            if (bUseOnlySpecialBox)
            {
                if (m_Box[dwBox].m_nContainItemGenerType == INVALID_CONTAIN_ITEM_TYPE)
                {
                    continue;
                }

                bRetCode = m_Box[dwBox].CheckContainItemType(pItem);
                if (!bRetCode)
                {
                    continue;
                }
            }

            for (DWORD dwX = 0; dwX < m_Box[dwBox].m_dwSize; ++dwX)
            {
                pDestItem = m_Box[dwBox].m_pItemArray[dwX];
                if (pDestItem && pDestItem->CanStack(pItem))
                {
                    nCanPushAmount += pDestItem->GetMaxStackNum() - pDestItem->GetStackNum();
                    if (nCanPushAmount >= nAddAmount)
                    {
                        break;
                    }
                }
            }
        }
        
        if (nCanPushAmount >= nAddAmount)
        {
            for (int i = ibPackage; i <= ibPackage4; ++i)
            {
                if (bUseOnlySpecialBox)
                {
                    if (m_Box[i].m_nContainItemGenerType == INVALID_CONTAIN_ITEM_TYPE)
                    {
                        continue;
                    }

                    bRetCode = m_Box[i].CheckContainItemType(pItem);
                    if (!bRetCode)
                    {
                        continue;
                    }
                }

                bRetCode = PushItem(pItem, i, bNotifyClient);
                assert(bRetCode);

                nItemAmount = pItem->GetStackNum();
                if (nItemAmount == 0)
                {
                    break;
                }
            }
            assert(nItemAmount == 0);

            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;

            goto Exit1;
        }
    }

    nItemAmount = pItem->GetStackNum(); // 要添加的道具数

    for (int i = ibPackage; i <= ibPackage4; ++i)
    {
        KInventory* pBox = &m_Box[i];
        DWORD       dwX  = 0;
        
        if (bUseOnlySpecialBox)
        {
            if (pBox->m_nContainItemGenerType == INVALID_CONTAIN_ITEM_TYPE)
            {
                continue;
            }

            bRetCode = pBox->CheckContainItemType(pItem);
            if (!bRetCode)
            {
                continue;
            }
        }

        dwX = GetEmptyPos(i);
        if (dwX == UINT_MAX)
        {
            continue;
        }

        eRetCode = CanAddItemToPos(pItem, i, dwX);
        if (eRetCode != aircSuccess)
        {
            continue;
        }

        bRetCode = m_Box[i].PlaceItem(pItem, dwX);
        _ASSERT(bRetCode);
#ifdef _SERVER
        g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, i, dwX);
        if (bNotifyClient)
        {
            g_PlayerServer.DoAddItemNotify(m_pPlayer, pItem->m_dwID, nItemAmount);
        }
#endif
        goto Exit1;
    }
    
    goto Exit0;

Exit1:
    nResult = aircSuccess;
Exit0:
    return nResult;
}

KItem* KItemList::PickUpItem(DWORD dwBoxIndex, DWORD dwX, BOOL bSync)
{
    BOOL    bRetCode    = false;
    KItem*  pItem       = NULL;

    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

    pItem = m_Box[dwBoxIndex].PickUpItem(dwX);

#ifdef _CLIENT
    if (pItem)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventDestroyItem Item;
            Item.dwX = dwBoxIndex;
            Item.dwBoxIndex = dwX;
            Item.nVersion = 0;
            Item.dwTabType = pItem->getTabType();
            Item.dwIndex = pItem->getIndex();
            g_pGameWorldUIHandler->OnDestroyItem(Item);
        }
    }
#endif

#ifdef _SERVER
    if (bSync)
    {
        g_PlayerServer.DoSyncItemDestroy(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, dwBoxIndex, dwX);
    }
#endif

Exit0:
    return pItem;
}

//根据箱子中的坐标找物品
KItem* KItemList::GetItem(DWORD dwBoxIndex, DWORD dwX)
{
    KItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

    pItem = m_Box[dwBoxIndex].GetItem(dwX);

Exit0:
    return pItem;
}

DWORD KItemList::GetItemAmountInEquipAndPackage(DWORD dwTabType, DWORD dwIndex, int nBookRecipeID)
{
    DWORD dwResult      = 0;
    DWORD dwItemAmount  = 0;

    for (int i = ibEquip; i <= ibPackage4; ++i)
    {
        dwItemAmount = GetItemAmountInBox(i, dwTabType, dwIndex, nBookRecipeID);
        dwResult += dwItemAmount;
    }

    return dwResult;
}

DWORD KItemList::GetItemTotalAmount(DWORD dwTabType, DWORD dwIndex, int nBookRecipeID)
{
    DWORD dwResult      = 0;
    DWORD dwItemAmount  = 0;

    for (int i = 0; i < ibTotal; ++i)
    {
        if (i == ibSoldList)
        {
            continue;
        }

        dwItemAmount = GetItemAmountInBox(i, dwTabType, dwIndex, nBookRecipeID);
        dwResult += dwItemAmount;
    }

    return dwResult;
}

DWORD KItemList::GetItemAmountInBox(
    int nBoxIndex, DWORD dwTabType, DWORD dwIndex, int nBookRecipeID
)
{
    DWORD dwResult = 0;
    
    KGLOG_PROCESS_ERROR(nBoxIndex >= ibEquip && nBoxIndex < ibTotal);

    for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; dwX++)
    {
        KItem* pItem = m_Box[nBoxIndex].GetItem(dwX);
        
        if (pItem == NULL)
            continue;
        
        if (pItem->m_GenParam.dwTabType != dwTabType || pItem->m_GenParam.dwIndex != dwIndex)
            continue;
        
        if (nBookRecipeID != -1)
        {
            if (pItem->m_Common.nGenre != igBook || pItem->m_nCurrentDurability != nBookRecipeID)
                continue;
        }
            
        dwResult += pItem->GetStackNum();
    }

Exit0:
    return dwResult;
}

BOOL KItemList::GetItemPos(DWORD dwTabType, DWORD dwIndex, DWORD& dwBox, DWORD& dwX)
{
	for (int i = 0; i < ibTotal; ++i)
	{
        if ((g_IsBank(i) && !m_bBankOpened) || i == ibSoldList)
        {
            continue;
        }

		for (int j = 0; j < (int)m_Box[i].GetSize(); ++j)
		{
			KItem* pItem = m_Box[i].GetItem(j);
			if (pItem && pItem->m_GenParam.dwTabType == dwTabType && pItem->m_GenParam.dwIndex == dwIndex)
			{
				dwBox = i;
				dwX = j;
				return true;
			}
		}
	}

	return false;
}

BOOL KItemList::GetItemPos(DWORD dwID, DWORD &dwBox, DWORD &dwX)
{
    for (int i = 0; i < ibTotal; ++i)
    {
        if ((g_IsBank(i) && !m_bBankOpened) || i == ibSoldList)
        {
            continue;
        }

        for (int j = 0; j < (int)m_Box[i].m_dwSize; ++j)
        {
            KItem* pItem = m_Box[i].GetItem(j);
            if (pItem && pItem->m_dwID == dwID)
            {
                dwBox = i;
                dwX = j;
                return true;
            }
        }
    }

    return false;
}

int	KItemList::GetBoxType(DWORD dwBox)
{
    int nResult = ivtTotal;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);

    if (g_IsPackage(dwBox))
    {
        nResult = ivtPackage;
    }
    else if (g_IsBank(dwBox))
    {
        nResult = ivtBank;
    }
    else if (dwBox == ibSoldList)
    {
        nResult = ivtSoldlist;
    }
    else if (dwBox == ibEquip)
    {
        nResult = ivtEquipment;
    }

Exit0:
    return nResult;
}

#ifdef _SERVER
BOOL KItemList::CostItemInEquipAndPackage(
    DWORD dwTabType, DWORD dwIndex, DWORD dwAmount, int nBookRecipeID
)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    DWORD   dwItemAmount    = 0;

    dwItemAmount = GetItemAmountInEquipAndPackage(dwTabType, dwIndex, nBookRecipeID);
    KGLOG_PROCESS_ERROR(dwItemAmount >= dwAmount);

    for (int i = ibEquip; i <= ibPackage4; ++i)
    {
        if (dwAmount == 0)
            break;

        bRetCode = CostItemInBox(i, dwTabType, dwIndex, dwAmount, nBookRecipeID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    KGLOG_PROCESS_ERROR(dwAmount == 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KItemList::CostItemInAllBox(
    DWORD dwTabType, DWORD dwIndex, DWORD dwAmount, int nBookRecipeID
)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    
    for (int i = 0; i < ibTotal; ++i)
    {
        if (i == ibSoldList)
        {
            continue;
        }

        if (dwAmount == 0)
            break;

        bRetCode = CostItemInBox(i, dwTabType, dwIndex, dwAmount, nBookRecipeID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KItemList::CostSingleItem(DWORD dwBoxIndex, DWORD dwX, int nAmount)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KItem*  pItem       = NULL;
    
    pItem = GetItem(dwBoxIndex, dwX);
    KGLOG_PROCESS_ERROR(pItem);

    KGLOG_PROCESS_ERROR(nAmount <= pItem->m_nCurrentDurability);

    if (pItem->m_nCurrentDurability == nAmount)
    {
        if (pItem->GetGenre() == igEquipment && !pItem->IsStackable())
        {
            if (dwBoxIndex == ivtEquipment)
            {
				if (pItem->m_bApplyed)
				{
					bRetCode = UnApplyAttrib(pItem);	
					KGLOG_PROCESS_ERROR(bRetCode);
				}

                pItem->m_nCurrentDurability = 0;
            }
        }
        else
        {
            TItemPos ItemPos = {dwBoxIndex, dwX};
            bRetCode = DestroyItem(ItemPos);
            KGLOG_PROCESS_ERROR(bRetCode == ircSuccess);
        }
    }
    else
    {
        pItem->m_nCurrentDurability -= nAmount;
    }

#ifdef _SERVER
    g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, dwBoxIndex, dwX, pItem->m_nCurrentDurability);
#endif

    bResult = true;
Exit0:
    return bResult;
}

// 查找一个有道具并且可以叠加指定物品的空格
BOOL KItemList::FindStackRoom(DWORD dwBoxIndex, DWORD& dwX, KItem* pItem)
{	
	BOOL bResult = false;

	assert(pItem);

	KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

	for (dwX = 0; dwX < m_Box[dwBoxIndex].m_dwSize; dwX++)
	{
		KItem* pTargetItem = m_Box[dwBoxIndex].GetItem(dwX);
		if (pTargetItem && pTargetItem->CanStack(pItem))
		{
			if (pTargetItem->m_nStackNum + pItem->m_nStackNum <= pTargetItem->m_nMaxStackNum)
			{
				goto Exit1;
			}
		}
	}
    
    goto Exit0;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KItemList::FindFreeSoldList(DWORD& dwX)
{
    BOOL        bResult         = false;
    int         nPos            = m_nNextSoldListPos;
    KItem*      pItem           = NULL;

    KGLOG_PROCESS_ERROR(nPos >= 0 && nPos < MAX_SOLDLIST_PACKAGE_SIZE);

    pItem = m_Box[ibSoldList].PickUpItem(nPos);

    if (pItem)
    {
        g_PlayerServer.DoSyncItemDestroy(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, ibSoldList, nPos);

        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }

    m_nNextSoldListPos = (m_nNextSoldListPos + 1) % MAX_SOLDLIST_PACKAGE_SIZE;
    
    dwX = (DWORD)nPos;

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KItemList::FindFreeRoomInPackage(
    DWORD& dwBoxIndex, DWORD& dwX, int nItemGenerType, int nItemSubType
)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwPos       = 0;

    for (DWORD dwBox = ibPackage; dwBox <= ibPackage4; ++dwBox)
    {
        KInventory* pInventory = &m_Box[dwBox];
        
        bRetCode = pInventory->CheckContainItemType(nItemGenerType, nItemSubType);
        if (!bRetCode)
        {
            continue;
        }

        dwPos = GetEmptyPos(dwBox);
        if (dwPos != UINT_MAX)
        {
            dwBoxIndex = dwBox;
            dwX        = dwPos;

            return true;
        }
    }

    return false;
}

BOOL KItemList::FindStackRoomInPackage(DWORD& dwBoxIndex, DWORD& dwX, KItem* pItem)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    DWORD   dwBox       = 0;
    DWORD   dwPos       = 0;

	assert(pItem);

	// 先寻找可叠加的位置
	for (dwBox = ibPackage; dwBox < ibBank; ++dwBox)
	{
		bRetCode = FindStackRoom(dwBox, dwPos, pItem);
        if (bRetCode)
        {
            dwBoxIndex = dwBox;
            dwX        = dwPos;
            
            bResult = true;
            goto Exit0;
        }
	}

	// 再找空格
	for (dwBox = ibPackage; dwBox <= ibPackage4; ++dwBox)
	{
        KInventory* pBox = &m_Box[dwBox];
        bRetCode = pBox->CheckContainItemType(pItem);
        if (!bRetCode)
        {
            continue;
        }
        
		dwPos = GetEmptyPos(dwBox);
        if (dwPos != UINT_MAX)
        {
            dwBoxIndex = dwBox;
            dwX        = dwPos;
            
            bResult = true;
            goto Exit0;
        }
	}
    
Exit0:
    return bResult;
}

BOOL KItemList::FindStackRoomInBank(DWORD& dwBoxIndex, DWORD& dwX, KItem* pItem)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwBox       = 0;
    DWORD   dwPos       = 0;

    assert(pItem);
    
    KGLOG_PROCESS_ERROR(m_bBankOpened);

    // 先寻找可叠加的位置
    for (dwBox = ibBank; dwBox <= ibBankPackage5; ++dwBox)
    {
        bRetCode = FindStackRoom(dwBox, dwPos, pItem);
        if (bRetCode)
        {
            dwBoxIndex = dwBox;
            dwX        = dwPos;

            bResult = true;
            goto Exit0;
        }
    }

    // 再找空格
    for (dwBox = ibBank; dwBox <= ibBankPackage5; ++dwBox)
    {
        KInventory* pBox = &m_Box[dwBox];
        bRetCode = pBox->CheckContainItemType(pItem);
        if (!bRetCode)
        {
            continue;
        }
        
        dwPos = GetEmptyPos(dwBox);
        if (dwPos != UINT_MAX)
        {
            dwBoxIndex = dwBox;
            dwX        = dwPos;

            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

int KItemList::GetAllPackageFreeRoomSize(void)
{
    int         nRetSize        = -1;
    int         nFreeRoomSize   = 0;
    KInventory* pInventory      = NULL;

    for (int nBoxIndex = ibPackage; nBoxIndex <= ibPackage4; ++nBoxIndex)
    {
        pInventory = &m_Box[nBoxIndex];
        
        if (m_Box[nBoxIndex].m_nContainItemGenerType != INVALID_CONTAIN_ITEM_TYPE) // 不找材料包的空格
        {
            continue;
        }

        for (DWORD dwX = 0; dwX < pInventory->m_dwSize; dwX++)
        {
            KItem* pItem = pInventory->GetItem(dwX);
            if (pItem == NULL)
            {
                ++nFreeRoomSize;
            }
        }
    }
    
    nRetSize = nFreeRoomSize;
Exit0:
    return nRetSize;
}

int KItemList::GetBoxFreeRoomSize(int nBoxIndex)
{
    int         nRetSize        = -1;
    int         nFreeRoomSize   = 0;
    KInventory* pInventory      = NULL;

    KGLOG_PROCESS_ERROR(nBoxIndex >= ibEquip && nBoxIndex < ibTotal);
    KG_PROCESS_ERROR(!g_IsBank(nBoxIndex) || m_bBankOpened);
    KG_PROCESS_ERROR(nBoxIndex != ivtSoldlist);

    pInventory = &m_Box[nBoxIndex];
    for (DWORD dwX = 0; dwX < pInventory->m_dwSize; dwX++)
    {
        KItem* pItem = pInventory->GetItem(dwX);
        if (pItem == NULL)
        {
            ++nFreeRoomSize;
        }
    }
    
    nRetSize = nFreeRoomSize;
Exit0:
    return nRetSize;
}

BOOL KItemList::SetMoney(int nMoney)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(nMoney <= m_nMoneyLimit);

    m_nMoney = nMoney;

#ifdef _SERVER
    assert(m_pPlayer);
    g_PlayerServer.DoSyncMoney(m_pPlayer->m_nConnIndex, m_nMoney, false);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KItemList::AddMoney(int nMoney, BOOL bShowMsg)
{
    BOOL        bResult     = false;

    assert(m_pPlayer);

    if (nMoney >= 0)
    {
        if (m_nMoney > INT_MAX - nMoney)
        {
            m_nMoney = INT_MAX;
        }
        else
        {
            m_nMoney += nMoney;
        }
    }
    else
    {
        if (m_nMoney < INT_MIN - nMoney)
        {
            m_nMoney = INT_MIN;
        }
        else
        {
            m_nMoney += nMoney;
        }
    }

    if (m_nMoney > m_nMoneyLimit)
    {
        m_nMoney = m_nMoneyLimit;
    }

#ifdef _SERVER
    g_PlayerServer.DoSyncMoney(m_pPlayer->m_nConnIndex, m_nMoney, bShowMsg);
#endif

    bResult = true;
Exit0:
    return bResult;
}

ITEM_RESULT_CODE KItemList::CanExchangeItemToPos(KItem* pItem, DWORD dwDestBox, DWORD dwDestX)
{
    ITEM_RESULT_CODE	nResult             = ircFailed;
    BOOL	            bRetCode            = false;
    ITEM_RESULT_CODE    eRetCode            = ircFailed;
    int                 nCurrentDurability  = 0;
    DWORD               dwNewBox            = 0;

    KG_PROCESS_SUCCESS(pItem == NULL);

    KGLOG_PROCESS_ERROR(dwDestBox < ibTotal);

    assert(m_pPlayer);

    switch(dwDestBox)
    {
    case ibEquip:
        // 如果不是放到装备栏的背包上，则该物品类型必须是装备
        dwNewBox = g_GetBoxIndexFromSlot(dwDestX);
        KG_PROCESS_ERROR(dwNewBox != 0 || pItem->m_Common.nGenre == igEquipment); 

        if (dwNewBox == 0) // 向装备格子上放
        {
            eRetCode = CanEquip(pItem, dwDestX);
            KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
        }
        else	// 向背包上放
        {
            KItem* pPackage = m_Box[ibEquip].GetItem(dwDestX);
            EQUIPMENT_INVENTORY_TYPE eInventoryType = (EQUIPMENT_INVENTORY_TYPE)dwDestX;

            //检测银行背包的扩充格子是否激活
            if (eInventoryType >= eitBankPackage1 && eInventoryType <= eitBankPackage5)
            {
                KG_PROCESS_ERROR(m_bBankOpened);

                bRetCode = IsBankPackageEnabled(eInventoryType);
                KG_PROCESS_ERROR(bRetCode);
            }

            if (pPackage)
            {
                DWORD dwUseCount = GetBoxUsedSize(dwNewBox);

                if (pItem->m_Common.nSub == estPackage)
                {
                    assert(dwNewBox < ibTotal);

                    // 背包类型相同或者新换上去的包是无限制类型，或者目标包中无东西或者源包已经装备
                    KG_PROCESS_ERROR_RET_CODE(
                        pPackage->m_Common.nDetail == pItem->m_Common.nDetail || 
                        pItem->m_Common.nDetail == 0 || dwUseCount == 0 ||
                        pItem->m_bEquiped, ircCannotPutToThatPlace
                    );

                    KG_PROCESS_ERROR_RET_CODE(pItem->m_nPackageSize >= (int)dwUseCount, ircCannotPutToThatPlace);
                }
                else
                {
                    KG_PROCESS_ERROR_RET_CODE(dwUseCount < m_Box[dwNewBox].m_dwSize, ircPackageNotEnoughSize);
                    
                    bRetCode = m_Box[dwNewBox].CheckContainItemType(pItem);
                    KG_PROCESS_ERROR_RET_CODE(bRetCode, ircCannotPutToThatPlace);
                }
            }
            else
            {
                eRetCode = CanEquip(pItem, dwDestX);
                KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
            }
        }
        break;

    case ibPackage:
    case ibPackage1:
    case ibPackage2:
    case ibPackage3:
    case ibPackage4:
    case ibBank:
    case ibBankPackage1:
    case ibBankPackage2:
    case ibBankPackage3:
    case ibBankPackage4:
    case ibBankPackage5:
        bRetCode = m_Box[dwDestBox].CheckContainItemType(pItem);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, ircCannotPutToThatPlace);
        break;

    case ibSoldList:
        break;
    }

Exit1:
    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::CanExchangeItem(DWORD dwSrcBox, DWORD dwSrcX, DWORD dwDestBox, DWORD dwDestX)
{
    ITEM_RESULT_CODE	nResult         = ircFailed;
    ITEM_RESULT_CODE    eRetCode        = ircFailed;
    KItem*              pSrcItem        = NULL;
    KItem*              pDestItem       = NULL;
    BOOL                bBank           = false;

    KG_PROCESS_ERROR(dwSrcBox >= ibEquip  && dwSrcBox < ibTotal);
    KG_PROCESS_ERROR(dwDestBox >= ibEquip && dwDestBox < ibTotal);

    KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_eMoveState != cmsOnDeath, ircPlayerIsDead);
   
#ifdef _SERVER
    if (dwSrcBox == ibSoldList || dwDestBox == ibSoldList)
    {
        KG_PROCESS_ERROR(m_bSoldListOpened); // 必须已打开
    }
#endif

    bBank = IsBankPackageSlot(dwSrcBox, dwSrcX);
    if (bBank)
    {
        KG_PROCESS_ERROR(m_bBankOpened); 
    }
    
    bBank = IsBankPackageSlot(dwDestBox, dwDestX);
    if (bBank)
    {
        KG_PROCESS_ERROR(m_bBankOpened); 
    }

    if (g_IsBank(dwSrcBox) || g_IsBank(dwDestBox))
    {
        KG_PROCESS_ERROR(m_bBankOpened); // 必须已打开
    }

    pSrcItem  = GetItem(dwSrcBox, dwSrcX); // 源物品不能为空
    KGLOG_PROCESS_ERROR(pSrcItem);
    
    if (dwSrcBox == ibEquip)
    {
        eRetCode = CanUnEquip(dwSrcX);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
    }

    pDestItem = GetItem(dwDestBox, dwDestX);
    
    if (dwDestBox == ibEquip && pDestItem)
    {
        eRetCode = CanUnEquip(dwDestX);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
    }

    nResult = CanExchangeItemToPos(pSrcItem, dwDestBox, dwDestX);
    KG_PROCESS_ERROR_RET_CODE(nResult == ircSuccess, nResult);	
    
    if (IsPackageSlot(dwDestBox, dwDestX))
    {
        DWORD dwPackageBox = g_GetBoxIndexFromSlot(dwDestX);
        KG_PROCESS_ERROR_RET_CODE(dwPackageBox != dwSrcBox, ircCannotPutToThatPlace);
    }
    else // 如果目标是背包，则不用下面的检查
    {
        nResult = CanExchangeItemToPos(pDestItem, dwSrcBox, dwSrcX);
        KG_PROCESS_ERROR_RET_CODE(nResult == ircSuccess, nResult);
    }

    if (IsPackageSlot(dwSrcBox, dwSrcX))
    {
        // 判断是否把背包放到了自己里
        DWORD dwPackageBox = g_GetBoxIndexFromSlot(dwSrcX);
        KG_PROCESS_ERROR_RET_CODE(dwPackageBox != dwDestBox, ircCannotPutToThatPlace);
        
        if (!IsPackageSlot(dwDestBox, dwDestX))
        {
            DWORD dwUsedCount = GetBoxUsedSize(dwPackageBox);
            KG_PROCESS_ERROR_RET_CODE(dwUsedCount == 0, ircCannotDisablePackage);
        }

        // 目标不为空则必须是一个背包道具
        if (pDestItem)
        {
            KG_PROCESS_ERROR_RET_CODE(
                pDestItem->m_Common.nGenre == igEquipment && pDestItem->m_Common.nSub == estPackage,
                ircCannotPutToThatPlace
            );
        }
    }

    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::ExchangeItem(const TItemPos& PosSrc, const TItemPos& PosDest, DWORD dwAmount)
{
    ITEM_RESULT_CODE	nResult             = ircFailed;
    BOOL				bRetCode            = false;
    int					nRetCode            = 0;
    KItem*				pSrcItem            = NULL;
    KItem*				pDestItem           = NULL;
    KInventory*			pSrcBox             = NULL;
    KInventory*			pDestBox            = NULL;
	TItemPos            Src                 = PosSrc;
	TItemPos            Dest                = PosDest;
    DWORD               dwSrcItemTabType    = 0;
    DWORD               dwSrcItemIndex      = 0;

    assert(m_pPlayer);

	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_eMoveState != cmsOnDeath, ircPlayerIsDead);

    // 如果源和目标一致，说明不用换，返回失败。
    KGLOG_PROCESS_ERROR(Src.dwBox != Dest.dwBox || Src.dwX != Dest.dwX);

    KGLOG_PROCESS_ERROR(Src.dwBox < ibTotal && Src.dwX < m_Box[Src.dwBox].m_dwSize);
    KGLOG_PROCESS_ERROR(Dest.dwBox < ibTotal && Dest.dwX < m_Box[Dest.dwBox].m_dwSize);

    pSrcItem = GetItem(Src.dwBox, Src.dwX);
    // 交换道具时必须保证源道具不为空
    KGLOG_PROCESS_ERROR(pSrcItem);
    KGLOG_PROCESS_ERROR_RET_CODE(dwAmount <= (DWORD)pSrcItem->GetStackNum(), ircNotEnoughDurability);

    dwSrcItemTabType = pSrcItem->m_GenParam.dwTabType;
    dwSrcItemIndex   = pSrcItem->m_GenParam.dwIndex;

    // 目标道具有可能为空
    pDestItem = GetItem(Dest.dwBox, Dest.dwX);
    
    pSrcBox = &m_Box[Src.dwBox];
    pDestBox = &m_Box[Dest.dwBox];

#ifdef _SERVER // 客户端不用检测
    nResult = CanExchangeItem(Src.dwBox, Src.dwX, Dest.dwBox, Dest.dwX);
    KG_PROCESS_ERROR_RET_CODE(nResult == ircSuccess, nResult);
#endif

	// 目标放到背包的装备格子中，要切换目标
	if (pDestItem && pSrcItem && pSrcItem->m_Common.nSub != estPackage && IsPackageSlot(Dest.dwBox, Dest.dwX))
	{
        DWORD   dwX         = 0;
        KInventory* pBox    = NULL;
		int     nBoxIndex   = g_GetBoxIndexFromSlot(Dest.dwX);
        KGLOG_PROCESS_ERROR(nBoxIndex >= ibEquip && nBoxIndex < ibTotal);

        pBox = &m_Box[nBoxIndex];

        dwX = GetEmptyPos(nBoxIndex);
        KG_PROCESS_ERROR_RET_CODE(dwX != UINT_MAX, ircCannotPutToThatPlace);

		Dest.dwBox  = nBoxIndex;
		Dest.dwX    = dwX;
		pDestItem   = NULL;
		pDestBox    = &m_Box[nBoxIndex];
	}

    if (dwAmount == 0)
    {
        dwAmount = pSrcItem->GetStackNum();
    }

    // 没有向装备格上移动
    if ((Src.dwBox != ibEquip) && (Dest.dwBox != ibEquip))
    {
        assert(pSrcItem);
        
        if (pDestItem)		// 目标有东西
        {
            if (pDestItem->CanStack(pSrcItem))				// 目标可以叠加
            {
                assert(pDestItem->m_pItemInfo);
                if (
                    (pDestItem->m_pItemInfo->nExistType == ketOnLine || pDestItem->m_pItemInfo->nExistType == ketOnlineAndOffLine) &&
                    pDestItem->m_GenParam.nGenTime > pSrcItem->m_GenParam.nGenTime
                )
                {
                    pDestItem->m_GenParam.nGenTime = pSrcItem->m_GenParam.nGenTime;
                }

                nRetCode = pDestItem->Stack(dwAmount);
                // 放不下的剩余部分堆回去
                nRetCode = pSrcItem->SetStackNum(pSrcItem->GetStackNum() - dwAmount + nRetCode);

                if (pSrcItem->GetStackNum() == 0)
                {
                    nRetCode = DestroyItem(pSrcItem, false);
                    KGLOG_CHECK_ERROR(nRetCode == ircSuccess);	// 如果策划填了一个可叠加不可摧毁的东西，就会出这个LOG
                    pSrcItem = NULL;
                }
            }
            else											// 不可叠加，互换
            {
                KG_PROCESS_ERROR_RET_CODE(dwAmount == (DWORD)pSrcItem->GetStackNum(), ircCannotPutToThatPlace);

                KItem* pTemp;
                pTemp = pSrcBox->PickUpItem(Src.dwX);
                assert(pTemp == pSrcItem);
                pTemp = pDestBox->PickUpItem(Dest.dwX);
                assert(pTemp == pDestItem);
                bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
                assert(bRetCode);
                bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
                assert(bRetCode);
            }
        }
        else	// 目标没东西
        {
            KItem* pTemp = NULL;
            if (dwAmount == (DWORD)pSrcItem->GetStackNum())	// 全移动
            {
                pTemp= pSrcBox->PickUpItem(Src.dwX);
                assert(pTemp == pSrcItem);
                bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
                assert(bRetCode);
            }
            else	// 拆分出新的物品
            {
#if defined(_SERVER)		// 服务端生成物品，客户端不做操作，等物品同步
                pTemp = g_pSO3World->m_ItemManager.GenerateItem(
                    pSrcItem->m_GenParam.dwTabType, pSrcItem->m_GenParam.dwIndex, 
                    pSrcItem->m_GenParam.nGenTime, ERROR_ID, pSrcItem->m_GenParam.dwRandSeed
                );

                bRetCode = pSrcItem->SetStackNum(pSrcItem->GetStackNum() - dwAmount);
                assert(bRetCode);

                bRetCode = g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, Src.dwBox,
                    Src.dwX, pSrcItem->GetStackNum());
                KGLOG_CHECK_ERROR(bRetCode);

                bRetCode = pTemp->SetStackNum(dwAmount);	
                assert(bRetCode);

                bRetCode = m_Box[Dest.dwBox].PlaceItem(pTemp, Dest.dwX);
                assert(bRetCode);

                bRetCode = g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pTemp, 
                    Dest.dwBox, Dest.dwX);
                KGLOG_CHECK_ERROR(bRetCode);

                goto Exit1;
#endif
            }
        }

#ifdef _CLIENT
        if ((!g_IsPackage(Src.dwBox)) && g_IsPackage(Dest.dwBox))
        {
            m_pPlayer->m_QuestList.OnUpdateItem(dwSrcItemTabType, dwSrcItemIndex, true);
        }
        else if (g_IsPackage(Src.dwBox) && (!g_IsPackage(Dest.dwBox)))
        {
            m_pPlayer->m_QuestList.OnUpdateItem(dwSrcItemTabType, dwSrcItemIndex, false);
        }
#endif
    }
    else	// 装备或反装备
    {
        KGLOG_PROCESS_ERROR(dwAmount == 1 || pSrcItem->m_Common.nSub == estArrow);
		BOOL bSrcPackage = IsPackageSlot(Src.dwBox, Src.dwX);
		BOOL bDestPackage = IsPackageSlot(Dest.dwBox, Dest.dwX);

		if ( bSrcPackage || bDestPackage)
		{
			if (bSrcPackage && bDestPackage)
			{
				DWORD dwSrcBox = g_GetBoxIndexFromSlot(Src.dwX);
				DWORD dwDestBox = g_GetBoxIndexFromSlot(Dest.dwX);
				nResult = ExchangePackageSlot(dwSrcBox, dwDestBox);
			}
			else if (bSrcPackage)
			{
				assert(pSrcItem);

				if (!pDestItem)
				{
					DWORD dwBox = g_GetBoxIndexFromSlot(Src.dwX);
					nResult = DisablePackage(dwBox);
				}
				else
				{
					KCustomEquipInfo* pEquipInfo = NULL;
                    DWORD dwBox = g_GetBoxIndexFromSlot(Src.dwX);

					assert(pDestItem->m_Common.nGenre == igEquipment &&	pDestItem->m_Common.nSub == estPackage);
                    
                    pEquipInfo = (KCustomEquipInfo*)pDestItem->GetItemInfo();
			        KGLOG_PROCESS_ERROR(pEquipInfo);

					nResult = EnablePackage(
                        dwBox, pDestItem->m_nPackageSize, pEquipInfo->nPackageGenerType, pEquipInfo->nPackageSubType
                    );
				}

			}
			else if (bDestPackage)
			{
				assert(pSrcItem);
				assert(pSrcItem->m_Common.nGenre == igEquipment && pSrcItem->m_Common.nSub == estPackage);

				DWORD dwBox = g_GetBoxIndexFromSlot(Dest.dwX);
                KCustomEquipInfo* pEquipInfo = (KCustomEquipInfo*)pSrcItem->GetItemInfo();
                KGLOG_PROCESS_ERROR(pEquipInfo);

				nResult = EnablePackage(
                    dwBox, pSrcItem->m_nPackageSize, pEquipInfo->nPackageGenerType, pEquipInfo->nPackageSubType
                );
			}
			else
			{
				assert(false);
			}
			
			if (nResult == ircSuccess)
			{
				KItem* pTemp = NULL;

				if (Src.dwBox == ibEquip && Dest.dwBox != ibEquip)
				{
					UnEquip(Src.dwX);

					pTemp = pSrcBox->PickUpItem(Src.dwX);
					assert(pTemp == pSrcItem);

					if (pDestItem)
					{
						pTemp = pDestBox->PickUpItem(Dest.dwX);
						assert(pTemp == pDestItem);

						bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
						assert(bRetCode);
						Equip(pDestItem, Src.dwX);
					}

					bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
					assert(bRetCode);
				}
				else if (Dest.dwBox == ibEquip && Src.dwBox != ibEquip)
				{
					if (pDestItem)
					{
						UnEquip(Dest.dwX);
						pTemp = pDestBox->PickUpItem(Dest.dwX);
						assert(pTemp == pDestItem);
					}

					pTemp = pSrcBox->PickUpItem(Src.dwX);
					assert(pTemp == pSrcItem);

					if (pDestItem)
					{
						bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
						assert(bRetCode);
					}

					bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
					assert(bRetCode);

					Equip(pSrcItem, Dest.dwX);
				}
				else
				{
					pTemp = pSrcBox->PickUpItem(Src.dwX);
					assert(pTemp == pSrcItem);
					pTemp = pDestBox->PickUpItem(Dest.dwX);
					assert(pTemp == pDestItem);
					if (pDestItem)
					{
						bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
						assert(bRetCode);
					}
					bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
					assert(bRetCode);
				}
			}
		}
		else if (Src.dwBox == ibEquip && Dest.dwBox == ibEquip)
		{
			// 两个都是装备格，并且可交换放置，那就直接交换好了。不用应用属性
			KItem* pTemp;
			pTemp = pSrcBox->PickUpItem(Src.dwX);
			assert(pTemp == pSrcItem);
            
            if (pDestItem)
            {
                pTemp = pDestBox->PickUpItem(Dest.dwX);
			    assert(pTemp == pDestItem);
			    bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
			    assert(bRetCode);
            }

			bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
			assert(bRetCode);

		}
        else if (Src.dwBox == ibEquip)	// 源装备拆下，目标装备有的话是装上。
        {
            if (pDestItem)
            {
                KItem* pTemp = NULL;

//				是否能装备前面已经检查过了。
//              nRetCode = CanEquip(pDestItem, Src.dwX);
//              KGLOG_PROCESS_ERROR(nRetCode == ircSuccess);
                
                bRetCode = UnEquip(Src.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                assert(pTemp == pSrcItem);
                pTemp = m_Box[Dest.dwBox].PickUpItem(Dest.dwX);
                assert(pTemp == pDestItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                assert(bRetCode);
                bRetCode = m_Box[Src.dwBox].PlaceItem(pDestItem, Src.dwX);
                assert(bRetCode);
                bRetCode = Equip(pDestItem, Src.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KItem* pTemp = NULL;
                
                bRetCode = UnEquip(Src.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                assert(pTemp == pSrcItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                assert(bRetCode);
            }
        }
        else	// 源装备装上，目标装备有的话是拆下。
        {
            if (pDestItem)
            {
                KItem* pTemp = NULL;

//				是否能装备上面CanPut已经检查过了。
//              nRetCode = CanEquip(pSrcItem, Dest.dwX);
//              KGLOG_PROCESS_ERROR(nRetCode == ircSuccess);

                bRetCode = UnEquip(Dest.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                assert(pTemp == pSrcItem);
                pTemp = m_Box[Dest.dwBox].PickUpItem(Dest.dwX);
                assert(pTemp == pDestItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                assert(bRetCode);
                bRetCode = m_Box[Src.dwBox].PlaceItem(pDestItem, Src.dwX);
                assert(bRetCode);
                bRetCode = Equip(pSrcItem, Dest.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KItem* pTemp = NULL;

                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                assert(pTemp == pSrcItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                assert(bRetCode);
                bRetCode = Equip(pSrcItem, Dest.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }
    }

#ifdef _SERVER
    g_PlayerServer.DoExchangeItem(
        m_pPlayer->m_nConnIndex, Src.dwBox, Src.dwX, Dest.dwBox, Dest.dwX, dwAmount
    );
#endif

Exit1:
    m_pPlayer->UpdateWeaponAttackSpeed();

    nResult = ircSuccess;
Exit0:
#ifdef _SERVER
    if (nResult != ircSuccess)
    {
        g_PlayerServer.DoMessageNotify(m_pPlayer->m_nConnIndex, ectItemErrorCode, (BYTE)nResult);
    }
#endif
    return nResult;
}

BOOL KItemList::GetEquipPos(int nRepresentIndex, DWORD *pdwEquipPos)
{
    BOOL bResult = false;

    switch (nRepresentIndex)
    {
    case perHelmStyle:
        *pdwEquipPos = eitHelm;
        break;
    
    case perChestStyle:
        *pdwEquipPos = eitChest;
        break;    

    case perWaistStyle:
        *pdwEquipPos = eitWaist;
        break;
    
    case perBangleStyle:
        *pdwEquipPos = eitBangle;
        break;

    case perBootsStyle:
        *pdwEquipPos = eitBoots;
        break;
    
    case perWeaponStyle:
        *pdwEquipPos = eitMeleeWeapon;
        break;
    
    case perBackExtend:
        *pdwEquipPos = eitBackExtend;
        break;

    case perWaistExtend:
        *pdwEquipPos = eitWaistExtend;
        break;
    
    case perHorseStyle:
        *pdwEquipPos = eitHorse;
        break;

	default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

EQUIPMENT_INVENTORY_TYPE KItemList::GetEquipPosBySubType(EQUIPMENT_SUB_TYPE eSubType)
{
    EQUIPMENT_INVENTORY_TYPE eRetType = eitTotal;

    assert(eSubType != estPackage);
    assert(eSubType != estRing); // estPackage和estRing类型的装备能放在多个位置，要特殊处理，不能调这个函数
    
    for (int i = 0; i < eitTotal; ++i)
    {
        if (g_CanEquipList[i] == eSubType)
        {
            eRetType = (EQUIPMENT_INVENTORY_TYPE)i;
            break;
        }
    }
    
    return eRetType;
}

ITEM_RESULT_CODE KItemList::GetCanEquipPos(KItem* pItem, int* pnRetPos)
{
    ITEM_RESULT_CODE         nResult    = ircInvalid;
    ITEM_RESULT_CODE         nRetCode   = ircInvalid;
    EQUIPMENT_INVENTORY_TYPE ePos       = eitTotal;
    KItem*                   pTempItem  = NULL;

    assert(pnRetPos);

    switch(pItem->m_Common.nSub)
    {
    case estPackage:
        for (int i = eitPackage1; i <= eitPackage4; ++i)
        {
            nRetCode = CanEquip(pItem, i);
            if (nRetCode == ircSuccess)
            {
                pTempItem = GetItem(igEquipment, i);
                if (pTempItem == NULL)
                {
                    ePos = (EQUIPMENT_INVENTORY_TYPE)i;
                    goto Exit1;
                }
                else if (ePos == eitTotal)
                {
                    ePos = (EQUIPMENT_INVENTORY_TYPE)i;
                }
            }
        }
        KG_PROCESS_ERROR_RET_CODE(ePos != eitTotal, nRetCode);

        break;
    case estRing:
        nRetCode = CanEquip(pItem, eitLeftRing);
        if (nRetCode == ircSuccess)
        {
            ePos = eitLeftRing;
            pTempItem = GetItem(igEquipment, eitLeftRing);
            KG_PROCESS_SUCCESS(pTempItem == NULL);
        }

        nRetCode = CanEquip(pItem, eitRightRing);
        if (nRetCode == ircSuccess)
        {
            ePos = eitRightRing;
            goto Exit1;
        }
        KG_PROCESS_ERROR_RET_CODE(ePos != eitTotal, nRetCode);

        break;
    default:
        ePos = GetEquipPosBySubType((EQUIPMENT_SUB_TYPE)pItem->m_Common.nSub);
        KGLOG_PROCESS_ERROR(ePos < eitTotal);

        nRetCode = CanEquip(pItem, ePos);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == ircSuccess, nRetCode);
        
        break;
    }
    
Exit1:
    *pnRetPos = ePos;
    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::CanEquip(KItem* pItem, DWORD dwX)
{
    ITEM_RESULT_CODE    nResult     = ircFailed;
    BOOL                bRetCode    = false;
    KAttribute*         pData       = NULL;
    KItemInfo*          pItemInfo   = NULL;

    assert(pItem);

    KG_PROCESS_ERROR_RET_CODE(pItem->m_Common.nGenre == igEquipment, ircCannotEquip);
    KGLOG_PROCESS_ERROR(dwX < m_Box[ibEquip].m_dwSize);

    KG_PROCESS_ERROR_RET_CODE(g_CanEquipList[dwX] == pItem->m_Common.nSub, ircErrorEquipPlace);

    if (pItem->m_Common.nSub != estMeleeWeapon && pItem->m_Common.nSub != estRangeWeapon && pItem->m_Common.nSub != estHorse && pItem->m_Common.nSub != estArrow)
        KG_PROCESS_ERROR_RET_CODE(!m_pPlayer->m_bFightState, ircPlayerInFight);

    pData = pItem->m_pRequireAttr;
    while (NULL != pData)
    {
        switch(pData->nKey)
        {
        case rqtFaction:
            // 门派是否相同
            KGLOG_PROCESS_ERROR_RET_CODE(pData->nValue1 == 0 || m_pPlayer->m_dwForceID == (DWORD)pData->nValue1, ircForceError);
            break;
        case rqtStrength:
            // 力量是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentStrength >= pData->nValue1, ircTooLowStrength);
            break;
        case rqtAgility:
            // 敏捷是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentAgility >= pData->nValue1, ircTooLowAgility);
            break;
        case rqtSpirit:
            // 根骨是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentSpirit >= pData->nValue1, ircTooLowSpirit);
            break;
        case rqtVitality:
            // 体质是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentVitality >= pData->nValue1, ircTooLowVitality);
            break;
        case rqtLevel:
            // 等级是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= pData->nValue1, ircTooLowLevel);
            break;
        case rqtGender:
            // 性别是否符合装备需求
            KGLOG_PROCESS_ERROR_RET_CODE(
                pData->nValue1 == 0 || m_pPlayer->m_eGender == pData->nValue1, ircGenderError
            );
            break;
        default:
            break;
        }
        pData = pData->pNext;
    }

    pItemInfo = pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);
    
    if (pItemInfo->dwRequireProfessionID != 0)
    {
        bRetCode = m_pPlayer->m_ProfessionList.IsProfessionLearned(pItemInfo->dwRequireProfessionID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, ircRequireProfession);

        if (pItemInfo->dwRequireProfessionBranch != 0)
        {
            DWORD dwBranchID = m_pPlayer->m_ProfessionList.GetBranchID(pItemInfo->dwRequireProfessionID);
            KG_PROCESS_ERROR_RET_CODE(dwBranchID == pItemInfo->dwRequireProfessionBranch, ircRequireProfessionBranch);
        }

        if (pItemInfo->nRequireProfessionLevel != 0)
        {
            int nProfessionLevel = m_pPlayer->m_ProfessionList.GetProfessionLevel(pItemInfo->dwRequireProfessionID);

            nProfessionLevel += m_pPlayer->m_ProfessionList.GetProfessionAdjustLevel(pItemInfo->dwRequireProfessionID);
            KG_PROCESS_ERROR_RET_CODE(nProfessionLevel >= pItemInfo->nRequireProfessionLevel, ircProfessionLevelTooLow);
        }
    }

    KG_PROCESS_ERROR_RET_CODE(pItemInfo->nRequireCamp & (0x01 << m_pPlayer->m_eCamp), ircCampCannotEquip);

    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::CanUnEquip(DWORD dwX)
{
    ITEM_RESULT_CODE    nResult  = ircFailed;
    KItem*              pItem    = NULL;
    
    assert(m_pPlayer);
    KGLOG_PROCESS_ERROR(dwX < m_Box[ibEquip].m_dwSize);

    pItem = GetItem(ibEquip, dwX);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(pItem->m_bEquiped);

    if (pItem->m_Common.nSub != estMeleeWeapon && pItem->m_Common.nSub != estRangeWeapon && pItem->m_Common.nSub != estHorse && pItem->m_Common.nSub != estArrow)
        KG_PROCESS_ERROR_RET_CODE(!m_pPlayer->m_bFightState, ircPlayerInFight);
    
    KG_PROCESS_ERROR_RET_CODE(!(dwX == eitHorse && m_pPlayer->m_bOnHorse), ircOnHorse); // 骑马的时候不能UnEquip马

    nResult = ircSuccess;
Exit0:
    return nResult;
}

BOOL KItemList::Equip(KItem* pItem, DWORD dwX)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    int     nRepresentIndex     = 0;
    WORD    wRepresentID        = 0;
    BOOL    bRepresentIsLock    = false;
    int     nCurrentDurability  = 0;

    assert(m_pPlayer);
    KGLOG_PROCESS_ERROR(dwX < m_Box[ibEquip].m_dwSize);
    KGLOG_PROCESS_ERROR(!(pItem->m_bApplyed));
	KGLOG_PROCESS_ERROR(!(pItem->m_bEquiped));
    assert(GetItem(ibEquip, dwX) == pItem);
    
    nCurrentDurability = pItem->GetCurrentDurability();

	if (
        nCurrentDurability != 0 && 
        (m_pPlayer->m_nUnarmCounter == 0 || dwX != eitMeleeWeapon) && 
        (dwX != eitHorse) // 装备马的时候不应用属性，在骑马的时候应用
    )
	{
		ApplyAttrib(pItem);
	}

#ifdef _CLIENT
    if (dwX == eitHorse && m_pPlayer->m_bOnHorse)
    {
        ApplyAttrib(pItem);
    }
#endif

	pItem->m_bEquiped = true;

    if (!pItem->m_bBind && pItem->m_Common.nBindType == ibtBindOnEquipped)
    {
        pItem->m_bBind = true;
    }

#ifdef _SERVER
    if ((pItem->m_dwScriptID != 0 || pItem->m_dwSkillID != 0) && pItem->m_Common.nGenre == igEquipment)
    {
        bRetCode = pItem->ResetCoolDown(m_pPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
#endif
    
    // 表现相关 ------------- >
    bRetCode = GetEquipRepresentIndex(dwX, &nRepresentIndex);
    if (bRetCode)
    {
        SetRepresentID(nRepresentIndex, pItem->m_nRepresentID);
    }
    
    for (int i = eiPermanentEnchant; i < eiTotal; ++i)
    {
        if (pItem->m_dwEnchantID[i] == 0)
        {
            continue;
        }

        KENCHANT* pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[i]);
        KGLOG_PROCESS_ERROR(pEnchant);

        SetRepresentID(pEnchant->nRepresentIndex, pEnchant->nRepresentID);
    }
    
    bRetCode = GetEquipColorIndex(dwX, &nRepresentIndex);
    if (bRetCode)
    {
        SetRepresentID(nRepresentIndex, pItem->m_nColorID);
    }
    // <-------------- 表现相关

    bResult = true;
Exit0:
    return bResult;
}

BOOL KItemList::UnEquip(DWORD dwX)
{
    BOOL     bResult            = false;
    BOOL     bRetCode           = false;
    int      nRepresentIndex    = 0;
    BOOL     bRepresentIsLock   = false;
    KItem*   pItem              = NULL;
    
    pItem = GetItem(ibEquip, dwX);
    KGLOG_PROCESS_ERROR(pItem);

#ifdef _SERVER
    bRetCode = CanUnEquip(dwX);
    KGLOG_PROCESS_ERROR(bRetCode == ircSuccess);
#endif

	if (pItem->m_bApplyed)
	{
		UnApplyAttrib(pItem);
	}

	pItem->m_bEquiped = false;
    
    bRetCode = GetEquipRepresentIndex(dwX, &nRepresentIndex);
    if (bRetCode)
    {
        SetRepresentID(nRepresentIndex, 0);
    }
    
    for (int i = eiPermanentEnchant; i < eiTotal; ++i)
    {
        if (pItem->m_dwEnchantID[i] == 0)
        {
            continue;
        }

        KENCHANT* pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[i]);
        KGLOG_PROCESS_ERROR(pEnchant);

        SetRepresentID(pEnchant->nRepresentIndex, 0);
    }

    bRetCode = GetEquipColorIndex(dwX, &nRepresentIndex);
    if (bRetCode)
    {
        SetRepresentID(nRepresentIndex, 0);
    }

    bResult = true;
Exit0:
    return bResult;
}

ITEM_RESULT_CODE KItemList::DestroyItem(const TItemPos& Pos, BOOL bSync /*= true*/)
{
    ITEM_RESULT_CODE    nResult     = ircFailed;
    BOOL                bRetCode    = false;
    KItem*              pItem       = NULL;
    
    assert(m_pPlayer);

    KGLOG_PROCESS_ERROR(Pos.dwBox < ibTotal && Pos.dwX < m_Box[Pos.dwBox].m_dwSize);
    
    pItem = GetItem(Pos.dwBox, Pos.dwX);
    KG_PROCESS_ERROR_RET_CODE(pItem, ircItemNotExist);

    if (Pos.dwBox == ibEquip)
    {
        KG_PROCESS_ERROR_RET_CODE(pItem->m_Common.nSub != estPackage, ircEquipedPackageCannotDestroy);
        bRetCode = UnEquip(Pos.dwX);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    pItem = m_Box[Pos.dwBox].PickUpItem(Pos.dwX);
    KG_PROCESS_ERROR_RET_CODE(pItem, ircItemNotExist);

    m_pPlayer->UpdateWeaponAttackSpeed();

#ifdef _CLIENT
    if (m_pPlayer->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventDestroyItem UIEvent;

            UIEvent.dwX         = Pos.dwX;
            UIEvent.dwBoxIndex  = Pos.dwBox;
            UIEvent.nVersion    = 0;
            UIEvent.dwTabType   = pItem->getTabType();
            UIEvent.dwIndex     = pItem->getIndex();

            g_pGameWorldUIHandler->OnDestroyItem(UIEvent);
        }
    }
#endif

#ifdef _SERVER
    
    if (bSync)
    {
        bRetCode = g_PlayerServer.DoSyncItemDestroy(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, Pos.dwBox, Pos.dwX);
        KGLOG_CHECK_ERROR(bRetCode);
    }
#endif

    bRetCode = g_pSO3World->m_ItemManager.FreeItem(pItem);
    assert(bRetCode);

    nResult = ircSuccess;
Exit0:
#ifdef _SERVER
    if (nResult != ircSuccess && bSync)
    {
        g_PlayerServer.DoMessageNotify(m_pPlayer->m_nConnIndex, ectItemErrorCode, (BYTE)nResult);
    }
#endif
     return nResult;
}

ITEM_RESULT_CODE KItemList::DestroyItem(KItem* pItem, BOOL bSync/* = true*/)
{
    ITEM_RESULT_CODE nResult = ircFailed;
    BOOL bRetCode = false;
    KGLOG_PROCESS_ERROR(pItem);

    for (int nBoxIndex = 0; nBoxIndex < ibTotal; nBoxIndex++)
    {
        for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; dwX++)
        {
            KItem* pTempItem = m_Box[nBoxIndex].GetItem(dwX);
            if (pItem == pTempItem)
            {
                TItemPos ItemPos = {nBoxIndex, dwX};
                nResult = DestroyItem(ItemPos, bSync);
                KGLOG_PROCESS_ERROR(nResult == ircSuccess);

                return ircSuccess;
            }
        }
    }

Exit0:
    return nResult;
}

DWORD KItemList::GetBoxSize(DWORD dwBox)
{
    DWORD	dwResult = 0;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);

    dwResult = m_Box[dwBox].m_dwSize;
Exit0:
    return dwResult;
}

DWORD KItemList::GetBoxUsedSize(DWORD dwBox)
{
    DWORD	dwResult = 0;
    DWORD   dwBoxSize = 0;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    dwBoxSize = m_Box[dwBox].GetSize();

    for (DWORD i = 0; i < dwBoxSize; ++i)
    {
        KItem* pItem = m_Box[dwBox].GetItem(i);
        if (pItem)
        {
            ++dwResult;
        }
    }

Exit0:
    return dwResult;
}

DWORD KItemList::GetEmptyPos(DWORD dwBox)
{
    DWORD dwBoxSize = 0;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    dwBoxSize = m_Box[dwBox].GetSize();

    for (DWORD i = 0; i < dwBoxSize; ++i)
    {
        KItem* pItem = m_Box[dwBox].GetItem(i);
        if (pItem == NULL)
        {
            return i;
        }
    }

Exit0:
    return UINT_MAX;
}

#ifdef _SERVER
BOOL KItemList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    size_t      uLeftSize   = uDataLen;
    BYTE*       pbyOffset   = pbyData;
    int         nItemCount  = 0;
    KItem*      pItem       = NULL;
    
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));
    m_nMoney = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);
    
    g_PlayerServer.DoSyncMoney(m_pPlayer->m_nConnIndex, m_nMoney, false);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
    m_nEnabledBankPackageCount = *(WORD*)pbyOffset;
    uLeftSize -= sizeof(WORD);
    pbyOffset += sizeof(WORD);
    
    g_PlayerServer.DoSyncEnableBankPackage(m_pPlayer->m_nConnIndex, m_nEnabledBankPackageCount);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
    nItemCount = *(WORD*)pbyOffset;
    uLeftSize -= sizeof(WORD);
    pbyOffset += sizeof(WORD);

    for (int nIndex = 0; nIndex < nItemCount; nIndex++)
    {
        KITEM_DB_HEADER*    pItemDataHeader     = NULL;
        DWORD               dwBoxIndex          = 0;
        DWORD               dwX                 = 0;
        KENCHANT*           pEnchant            = NULL;
        time_t              nTotalLogoutTime    = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KITEM_DB_HEADER));
        pItemDataHeader = (KITEM_DB_HEADER*)pbyOffset;
        uLeftSize -= sizeof(KITEM_DB_HEADER);
        pbyOffset += sizeof(KITEM_DB_HEADER);

        dwBoxIndex          = pItemDataHeader->byBox;
        dwX                 = pItemDataHeader->byPos;

        KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);
        
        if (pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }
        
        KGLOG_PROCESS_ERROR(uLeftSize >= pItemDataHeader->byDataLen);
        uLeftSize -= pItemDataHeader->byDataLen;
        pbyOffset += pItemDataHeader->byDataLen;

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(
            ERROR_ID, pItemDataHeader->byData, pItemDataHeader->byDataLen
        );
        if (pItem == NULL)
        {
            KGLogPrintf(KGLOG_ERR, "[ITEM] GenerateItemFromBinaryData failed, Acc: %s, Role: %s<%u>. \n", m_pPlayer->m_szAccount, m_pPlayer->m_szName, m_pPlayer->m_dwID);
            continue;
        }
        
        nTotalLogoutTime = g_pSO3World->m_nCurrentTime - m_pPlayer->m_nLastSaveTime;

        assert(pItem->m_pItemInfo);
        if (pItem->m_pItemInfo->nExistType == ketOffLine)
        {
            if (pItem->m_pItemInfo->nMaxExistTime && nTotalLogoutTime > pItem->m_pItemInfo->nMaxExistTime)
            {
                g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, pItem, "OffLine item up to nMaxExistTime");
                continue;
            }
        }
        else if (pItem->m_pItemInfo->nExistType == ketOnLine)
        {
            pItem->m_GenParam.nGenTime += nTotalLogoutTime;
        }

        bRetCode = m_Box[dwBoxIndex].PlaceItem(pItem, dwX);
        if (!bRetCode)
        {
            g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, pItem, "load item failed.");
            continue;
        }

        if (dwBoxIndex == ibEquip)
        {
            DWORD dwSlotBox = 0;
            
            bRetCode = Equip(pItem, dwX);
            if (!bRetCode)
            {
                m_Box[dwBoxIndex].PickUpItem(dwX);
                goto Exit0;
            }

            dwSlotBox = g_GetBoxIndexFromSlot(dwX);
            if (dwSlotBox)
            {
                KCustomEquipInfo* pCustomItemInfo = NULL;
                
                KGLOG_PROCESS_ERROR(pItem->m_Common.nGenre == igEquipment && pItem->m_Common.nSub == estPackage);
                
                pCustomItemInfo = (KCustomEquipInfo*)pItem->GetItemInfo();
                KGLOG_PROCESS_ERROR(pCustomItemInfo);

                EnablePackage(dwSlotBox, pItem->m_nPackageSize, pCustomItemInfo->nPackageGenerType, pCustomItemInfo->nPackageSubType);
            }
        }

        g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, dwBoxIndex, dwX);
        pItem = NULL;
    }

    CheckEquipRequire();

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    if (!bResult)
        KGLogPrintf(KGLOG_ERR, "[ITEM] Load item failed, Acc: %s, Role: %s<%u>. \n", m_pPlayer->m_szAccount, m_pPlayer->m_szName, m_pPlayer->m_dwID);

    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return bResult;
}

BOOL KItemList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult	                    = false;
    BOOL    bRetCode                    = false;
    BYTE*   pbyOffset                   = pbyBuffer;
    size_t  uLeftSize                   = uBufferSize;
    WORD*   pwItemCount                 = NULL;
    int     nItemCount                  = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = m_nMoney;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
    *(WORD*)pbyOffset = (WORD)m_nEnabledBankPackageCount;
    uLeftSize -= sizeof(WORD);
    pbyOffset += sizeof(WORD);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
    pwItemCount = (WORD*)pbyOffset;
    uLeftSize -= sizeof(WORD);
    pbyOffset += sizeof(WORD);

    for (int nBoxIndex = 0; nBoxIndex < ibTotal; nBoxIndex++)
    {
        if (nBoxIndex == ibSoldList)
        {
            continue;
        }

        for (int nX = 0; nX < (int)m_Box[nBoxIndex].m_dwSize; nX++)
        {
            KItem*              pItem               = m_Box[nBoxIndex].GetItem(nX);
            KITEM_DB_HEADER*    pItemDataHeader     = NULL;
            size_t              uItemDataSize       = 0;

            if (pItem == NULL)
            {
                continue;
            }

            KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KITEM_DB_HEADER));
            pItemDataHeader = (KITEM_DB_HEADER*)pbyOffset;
            uLeftSize -= sizeof(KITEM_DB_HEADER);
            pbyOffset += sizeof(KITEM_DB_HEADER);

            bRetCode = pItem->GetBinaryData(&uItemDataSize, pItemDataHeader->byData, uLeftSize);
            KGLOG_PROCESS_ERROR(bRetCode);

            assert(uItemDataSize < UCHAR_MAX);

            pItemDataHeader->byBox      = (BYTE)nBoxIndex;
            pItemDataHeader->byPos      = (BYTE)nX;
            pItemDataHeader->byDataLen  = (BYTE)uItemDataSize;

            uLeftSize -= uItemDataSize;
            pbyOffset += uItemDataSize;

            nItemCount++;
        }
    }

    *pwItemCount = (WORD)nItemCount;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

void KItemList::TidyUpSoldList()
{
    KItem*      pItem           = NULL;
    int         nFreePos        = 0;
    KItem*      TempItemList[MAX_SOLDLIST_PACKAGE_SIZE];

    memset(TempItemList, 0, sizeof(TempItemList));
    
    for (int i = 0; i < MAX_SOLDLIST_PACKAGE_SIZE; ++i)
    {
        pItem = m_Box[ibSoldList].GetItem(i);
        if (pItem)
        {
            g_PlayerServer.DoSyncItemDestroy(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, ibSoldList, i);
            TempItemList[nFreePos++] = pItem;
        }
    }
    
    m_nNextSoldListPos = nFreePos % MAX_SOLDLIST_PACKAGE_SIZE;
    
    memcpy(m_Box[ibSoldList].m_pItemArray, TempItemList, sizeof(m_Box[ibSoldList].m_pItemArray));
    
    for (int i = 0; i < MAX_SOLDLIST_PACKAGE_SIZE; ++i)
    {
        pItem = m_Box[ibSoldList].GetItem(i);
        if (pItem)
        {
            g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, ibSoldList, i);
        }
    }

Exit0:
    return;
}

void KItemList::AbradeArmor()
{
    AbradeItemAtEquipBox(eitChest);
    AbradeItemAtEquipBox(eitHelm);
    AbradeItemAtEquipBox(eitWaist);
    AbradeItemAtEquipBox(eitPants);
    AbradeItemAtEquipBox(eitBoots);
    AbradeItemAtEquipBox(eitBangle);
}

void KItemList::AbradeItemAtEquipBox(DWORD dwPos)
{
	BOOL    bRetCode            = false;
    KItem*  pItem               = NULL;
    int     nCurrentDurability  = 0;

    KGLOG_PROCESS_ERROR(dwPos < m_Box[ibEquip].m_dwSize);

    pItem = m_Box[ibEquip].m_pItemArray[dwPos];
    KG_PROCESS_ERROR(pItem);

    bRetCode = pItem->Abrade();
    KG_PROCESS_ERROR(bRetCode);
    
    nCurrentDurability = pItem->GetCurrentDurability();

#ifdef _SERVER
	g_PlayerServer.DoUpdateItemAmount(
        m_pPlayer->m_nConnIndex, ibEquip, dwPos, nCurrentDurability
	);
#endif

    if (nCurrentDurability == 0 && pItem->m_bApplyed)
	{
		UnApplyAttrib(pItem);
	}

Exit0:
    return;
}

BOOL KItemList::AbradeEquipment(int nBoxType, int nAbradePercent)
{
    BOOL bResult = false;
    BOOL bRetCode = false;

    KGLOG_PROCESS_ERROR(nBoxType == ivtEquipment || nBoxType == ivtPackage);
    KGLOG_PROCESS_ERROR(nAbradePercent >= 0 && nAbradePercent <= 100);

    for (int nBoxIndex = ibEquip; nBoxIndex <= ibPackage4; ++nBoxIndex)
    {
        if (nBoxType == ivtEquipment)
        {
            if (nBoxIndex != ibEquip)
            {
                continue;
            }
        }
        else
        {
            if (!g_IsPackage(nBoxIndex))
            {
                continue;
            }
        }
        
        for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; ++dwX)
        {
            int     nCurrentDurability  = 0;
            int     nMaxDurability      = 0;
            int     nAbradeDurability   = 0;
            KItem*  pItem               = m_Box[nBoxIndex].GetItem(dwX);

            if (pItem == NULL)
                continue;

            if (pItem->GetGenre() != igEquipment)
                continue;
            
            nCurrentDurability  = pItem->GetCurrentDurability();
            nMaxDurability      = pItem->GetMaxDurability();

            if (nCurrentDurability <= 0 || nMaxDurability <= 0)
            {
                continue;
            }

            nAbradeDurability   = max(1, nMaxDurability * nAbradePercent / 100);
            nCurrentDurability  = nCurrentDurability - nAbradeDurability;

            MAKE_IN_RANGE(nCurrentDurability, 0, nMaxDurability);
            bRetCode = pItem->SetCurrentDurability(nCurrentDurability);
            KGLOG_PROCESS_ERROR(bRetCode);
            if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
			{
				UnApplyAttrib(pItem);
			}
#ifdef _SERVER
            assert(m_pPlayer);
            g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, nBoxIndex, dwX, nCurrentDurability);
#endif
        }
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KItemList::ApplyAttrib(KItem* pItem)
{
	BOOL bResult    = false;
	BOOL bRetCode   = false;

	assert(pItem);
	
	m_pPlayer->ApplyAttribute(pItem->m_pBaseAttr);

	m_pPlayer->ApplyAttribute(pItem->m_pMagicAttr);
    
    for (int i = eiPermanentEnchant; i < eiTotal; ++i)
    {
        if (pItem->m_dwEnchantID[i] == 0)
        {
            continue;
        }

        KENCHANT* pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[i]);
        KGLOG_PROCESS_ERROR(pEnchant);

        m_pPlayer->ApplyAttribute(pEnchant->pAttribute);
    }

	if (pItem->m_Common.dwSetID)
	{
        // +1 是因为GetSetNum会少算自身
		// 不用检查返回值，有可能正常没套装属性应用的。
		ApplySetAttrib(pItem->m_Common.dwSetID, GetEquipedSetNum(pItem->m_Common.dwSetID) + 1);
	}

	pItem->m_bApplyed = true;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KItemList::UnApplyAttrib(KItem* pItem)
{
	BOOL bResult    = false;
	BOOL bRetCode   = false;

    assert(pItem->m_bApplyed);

    for (int i = eiPermanentEnchant; i < eiTotal; ++i)
    {
        if (pItem->m_dwEnchantID[i] == 0)
        {
            continue;
        }

        KENCHANT* pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[i]);
        KGLOG_PROCESS_ERROR(pEnchant);

        m_pPlayer->UnApplyAttribute(pEnchant->pAttribute);
    }
	
	m_pPlayer->UnApplyAttribute(pItem->m_pMagicAttr);

	m_pPlayer->UnApplyAttribute(pItem->m_pBaseAttr);

	if (pItem->m_Common.dwSetID)
	{
		// 这里就不 +1 了,GetSetNum会包括自身
        // 不用检查返回值，有可能正常没套装属性应用的。
		UnApplySetAttrib(pItem->m_Common.dwSetID, GetEquipedSetNum(pItem->m_Common.dwSetID));
	}

	pItem->m_bApplyed = false;

	bResult = true;
Exit0:
	return bResult;

}

BOOL KItemList::IsPackageSlot(DWORD dwBox, DWORD dwX)
{
	BOOL bResult = false;

	KG_PROCESS_ERROR(dwBox == ibEquip);
	KG_PROCESS_ERROR(
        dwX == eitPackage1 || dwX == eitPackage2 ||	dwX == eitPackage3 || dwX == eitPackage4 ||
		dwX == eitBankPackage1 || dwX == eitBankPackage2 || dwX == eitBankPackage3 || 
        dwX == eitBankPackage4 || dwX == eitBankPackage5
    );

	bResult = true;
Exit0:
	return bResult;
}

BOOL KItemList::IsBankPackageSlot(DWORD dwBox, DWORD dwX)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(dwBox == ibEquip);
    KG_PROCESS_ERROR(
        dwX == eitBankPackage1 || dwX == eitBankPackage2 || dwX == eitBankPackage3 || 
        dwX == eitBankPackage4 || dwX == eitBankPackage5
    );

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KItemList::Repair(DWORD dwBox, DWORD dwX)
{
	BOOL    bResult             = false;
	BOOL    bRetCode            = false;
	KItem*  pItem               = NULL;
    int     nCurrentDurability  = 0;

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
	KGLOG_PROCESS_ERROR(dwX < m_Box[dwBox].m_dwSize);

	pItem = m_Box[dwBox].GetItem(dwX);
	KGLOG_PROCESS_ERROR(pItem);

    bRetCode = pItem->Repair();
    KGLOG_PROCESS_ERROR(bRetCode);
    
    nCurrentDurability = pItem->GetCurrentDurability();
    KGLOG_PROCESS_ERROR(nCurrentDurability > 0);

    g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, dwBox, dwX, nCurrentDurability);

	if (pItem->m_bEquiped && !pItem->m_bApplyed)	// 装备着并且没应用过属性
	{
		ApplyAttrib(pItem);
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KItemList::EnableBankPackage()
{
	BOOL    bResult     = false;
	int     nCostMoney  = 0;

	KG_PROCESS_ERROR(m_nEnabledBankPackageCount < MAX_BANK_PACKAGE_COUNT);

	nCostMoney = g_pSO3World->m_Settings.m_ConstList.nBankPackagePrice[m_nEnabledBankPackageCount];
	KG_PROCESS_ERROR(m_nMoney >= nCostMoney); // 玩家能付的起钱

	bResult = SetMoney(m_nMoney - nCostMoney);
	KGLOG_PROCESS_ERROR(bResult);

	++m_nEnabledBankPackageCount;

	g_PlayerServer.DoSyncEnableBankPackage(m_pPlayer->m_nConnIndex, m_nEnabledBankPackageCount);

	bResult = true;
Exit0:
	return bResult;
}
#endif

int KItemList::GetBankPackageCount()
{
	return m_nEnabledBankPackageCount;
}

#ifdef _CLIENT
BOOL KItemList::SetEnablePackageCount(int nCount)
{
    BOOL bResult = false;
    
	KGLOG_PROCESS_ERROR(nCount >= 0 && nCount <= MAX_BANK_PACKAGE_COUNT);

	m_nEnabledBankPackageCount = nCount;

	bResult = true;
Exit0:
	return bResult;
}
#endif

BOOL KItemList::IsBankPackageEnabled(EQUIPMENT_INVENTORY_TYPE eInventoryType)
{
	BOOL    bResult         = false;
	int     nPackageIndex   = 0;

	KGLOG_PROCESS_ERROR(eInventoryType >= eitBankPackage1 && eInventoryType <= eitBankPackage5);

	nPackageIndex = (eInventoryType - eitBankPackage1 + 1);
	KG_PROCESS_ERROR(nPackageIndex <= m_nEnabledBankPackageCount);
	
	bResult = true;
Exit0:
	return bResult;
}

#ifdef _SERVER
BOOL KItemList::ConsumeAmmo(DWORD dwAmount)
{
	BOOL	bResult     = false;
	BOOL	bRetCode    = false;
	DWORD	dwTabType   = 0;
	DWORD	dwIndex     = 0;
	KItem*  pItem       = NULL;

    pItem = m_Box[ibEquip].GetItem(eitArrow);
	KG_PROCESS_ERROR(pItem);

	dwTabType   = pItem->m_GenParam.dwTabType;
	dwIndex     = pItem->m_GenParam.dwIndex;

	bRetCode = CostSingleItem(ibEquip, eitArrow, dwAmount);
	KGLOG_PROCESS_ERROR(bRetCode);

	pItem = m_Box[ibEquip].GetItem(eitArrow);
	if (pItem == NULL)
	{
		ReloadAmmo(dwTabType, dwIndex);
	}

	bResult = true;
Exit0:
	return bResult;
}

void KItemList::ReloadAmmo(DWORD dwTabType, DWORD dwIndex)
{
	ITEM_RESULT_CODE	eRetCode    = ircFailed;
	DWORD	            dwBox       = 0;
	DWORD	            dwX         = 0;
	KItem*              pItem       = NULL;
    TItemPos            ItemPos     = {0, 0};
	TItemPos            EquipPos    = {ibEquip, eitArrow};

    pItem = FindItem(dwTabType, dwIndex, dwBox, dwX);
    KG_PROCESS_ERROR(pItem);

	eRetCode = ExchangeItem(ItemPos, EquipPos);
	KG_PROCESS_ERROR(eRetCode == ircSuccess);
	
Exit0:
	return;
}

ENCHANT_RESULT_CODE KItemList::AddEnchant(DWORD dwBoxIndex, DWORD dwX, DWORD dwEnchantID, KItem* pSourceItem /*= NULL*/)
{
    ENCHANT_RESULT_CODE  nResult        = ercFailed;
    ENCHANT_RESULT_CODE  eRetCode       = ercFailed;
    BOOL                 bRetCode       = false;
    KItem*               pItem          = NULL;
    KENCHANT*            pNewEnchant    = NULL;
    int                  nMountIndex    = 0;
    
    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

	pItem = GetItem(dwBoxIndex, dwX);
	KGLOG_PROCESS_ERROR(pItem && pItem->m_Common.nGenre == igEquipment);

    pNewEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(dwEnchantID);
	KGLOG_PROCESS_ERROR(pNewEnchant);
    
	KGLOG_PROCESS_ERROR(pNewEnchant->nDestItemSubType == pItem->m_Common.nSub);
    
    if (pSourceItem) // 根据源道具来决定要镶嵌的位置
    {
        nMountIndex = pSourceItem->GetMountIndex();
        KGLOG_PROCESS_ERROR(nMountIndex != -1);
    }
    else // 为了兼容旧的接口
    {
        if (pNewEnchant->nTime <= 0)
        {
            nMountIndex = eiPermanentEnchant;
        }
        else
        {
            nMountIndex = eiTemporaryEnchant;
        }
    }
    
    assert(nMountIndex >= eiPermanentEnchant && nMountIndex <= eiMount4);

    if (pItem->m_dwEnchantID[nMountIndex])
    {
        eRetCode = RemoveEnchant(dwBoxIndex, dwX, nMountIndex);
        KGLOG_PROCESS_ERROR_RET_CODE(eRetCode == ercSuccess, eRetCode);
        pItem->m_dwEnchantID[nMountIndex] = 0;
    }

    pItem->m_dwEnchantID[nMountIndex] = dwEnchantID;
    
    if (nMountIndex == eiTemporaryEnchant)
    {
        pItem->m_nEnchantTime = g_pSO3World->m_nCurrentTime + pNewEnchant->nTime;
    }

    if (pItem->m_bApplyed)
    {
        m_pPlayer->ApplyAttribute(pNewEnchant->pAttribute);
    }

    if (pItem->m_bEquiped)
    {
        SetRepresentID(pNewEnchant->nRepresentIndex, pNewEnchant->nRepresentID);
    }

	g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, dwBoxIndex, dwX);

    nResult = ercSuccess;
Exit0:
    return nResult;
}

ENCHANT_RESULT_CODE KItemList::RemoveEnchant(DWORD dwBoxIndex, DWORD dwX, int nMountIndex)
{
    ENCHANT_RESULT_CODE nResult             = ercFailed;
    int                 nRetCode            = false;
	KItem*              pItem               = NULL;
    KENCHANT*           pCurrentEnchant     = NULL;
    KItem*              pAddItem            = NULL;

	pItem = GetItem(dwBoxIndex, dwX);
	KGLOG_PROCESS_ERROR(pItem && pItem->m_Common.nGenre == igEquipment);
    
    KGLOG_PROCESS_ERROR(nMountIndex >= eiPermanentEnchant && nMountIndex < eiTotal);

    if (nMountIndex == eiPermanentEnchant)
    {
        if (pItem->m_dwEnchantID[eiPermanentEnchant])
        {
            pCurrentEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[eiPermanentEnchant]);
            KGLOG_PROCESS_ERROR(pCurrentEnchant);

            pItem->m_dwEnchantID[eiPermanentEnchant] = 0;
        }
    }
    else if (nMountIndex == eiTemporaryEnchant)
    {
        if (pItem->m_dwEnchantID[eiTemporaryEnchant])
        {
            pCurrentEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[eiTemporaryEnchant]);
            KGLOG_PROCESS_ERROR(pCurrentEnchant);

            pItem->m_dwEnchantID[eiTemporaryEnchant]  = 0;
            pItem->m_nEnchantTime = 0;
        }
    }
    else // 道具镶嵌
    {
        KGLOG_PROCESS_ERROR(pItem->m_dwEnchantID[nMountIndex] > 0);

        pCurrentEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[nMountIndex]);
        KGLOG_PROCESS_ERROR(pCurrentEnchant);
        
        if (pCurrentEnchant->dwTabType != 0 && pCurrentEnchant->dwTabIndex != 0)
        {
            KITEM_LOG_INFO ItemLogInfo;

            int nFreeRoomSize = GetAllPackageFreeRoomSize();
            KG_PROCESS_ERROR_RET_CODE(nFreeRoomSize > 0, ercPackageIsFull);
            
            pAddItem = g_pSO3World->GenerateItem(
                pCurrentEnchant->dwTabType, pCurrentEnchant->dwTabIndex
            );
            KGLOG_PROCESS_ERROR(pAddItem);
            
            if (pItem->m_Common.nBindType == ibtBindOnEquipped)
            {
                pAddItem->m_bBind = true;
            }
            
            g_LogClient.GetItemLogInfo(pAddItem, &ItemLogInfo);
            nRetCode = AddItem(pAddItem);
            KGLOG_PROCESS_ERROR(nRetCode == aircSuccess);
            pAddItem = NULL;

            g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "unmount item");
        }

        pItem->m_dwEnchantID[nMountIndex] = 0;
    }

    if (pCurrentEnchant)
    {
	    if (pItem->m_bApplyed)
	    {
            m_pPlayer->UnApplyAttribute(pCurrentEnchant->pAttribute);
        }

        if (pItem->m_bEquiped)
        {
            DWORD dwEquipPos        = 0;
            int   nOldRepresentID   = 0;

            nRetCode = GetEquipPos(pCurrentEnchant->nRepresentIndex, &dwEquipPos);
            if (nRetCode)
            {
                KItem* pTempItem = GetItem(ibEquip, dwEquipPos);

                if (pTempItem)
                {
                    nOldRepresentID = pTempItem->m_nRepresentID;
                }
            }

            SetRepresentID(pCurrentEnchant->nRepresentIndex, nOldRepresentID);
        }

        g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, dwBoxIndex, dwX);
    }

    nResult = ercSuccess;
Exit0:
    if (pAddItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pAddItem);
        pAddItem = NULL;
    }
    return nResult;
}
#endif

ENCHANT_RESULT_CODE KItemList::CanMountItem(
    DWORD dwSrcBox, DWORD dwSrcX, DWORD dwDestBox, DWORD dwDestX
)
{
    ENCHANT_RESULT_CODE nResult             = ercFailed;
    ENCHANT_RESULT_CODE eRetCode            = ercFailed;
    USE_ITEM_RESULT_CODE eUseRet            = uircFailed;
    KENCHANT*           pNewEnchant         = NULL;
    int                 nMountIndex         = -1;
    KOtherItemInfo*     pItemInfo           = NULL;
    KItem*              pSourceItem         = NULL;
    KItem*              pDestItem           = NULL;
    KUSE_ITEM           UseItemInfo         = {0};
    KTarget             Target;
    
    Target.ClearTarget();

    pSourceItem = GetItem(dwSrcBox, dwSrcX);
    KGLOG_PROCESS_ERROR(pSourceItem);
    KG_PROCESS_ERROR(pSourceItem->m_Common.nGenre == igMountItem);
    KG_PROCESS_ERROR(pSourceItem->m_GenParam.dwTabType == ittOther);
    
    pItemInfo = (KOtherItemInfo*)g_pSO3World->m_ItemManager.GetItemInfo(
        pSourceItem->m_GenParam.dwTabType, pSourceItem->m_GenParam.dwIndex
    );
    KGLOG_PROCESS_ERROR(pItemInfo);
    KG_PROCESS_ERROR(pItemInfo->dwEnchantID != 0);

    pDestItem = GetItem(dwDestBox, dwDestX);
    KGLOG_PROCESS_ERROR(pDestItem && pDestItem->m_Common.nGenre == igEquipment);

    nMountIndex = pSourceItem->GetMountIndex();
    KG_PROCESS_ERROR(nMountIndex != -1);

    if (pDestItem->m_dwEnchantID[nMountIndex])
    {
        eRetCode = CanUnMountItem(dwDestBox, dwDestX, nMountIndex);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == ercSuccess, eRetCode);
    }
    
    pNewEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItemInfo->dwEnchantID);
    KGLOG_PROCESS_ERROR(pNewEnchant);

    KG_PROCESS_ERROR(pNewEnchant->nDestItemSubType == pDestItem->m_Common.nSub);

    eUseRet = CanUseItem(pSourceItem, dwSrcBox, &UseItemInfo, Target);
    KG_PROCESS_ERROR(eUseRet == uircSuccess);
    
    nResult = ercSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != ercSuccess)
    {
        if (eUseRet != uircFailed && g_pGameWorldUIHandler)
        {
            KUIEventUseItemRespond  UINotify;

            UINotify.nRespondCode           = eUseRet;
            UINotify.dwProfessionID         = UseItemInfo.dwProfessionID;
            UINotify.dwProfessionBranchID   = UseItemInfo.dwProfessionBranchID;
            UINotify.dwProfessionLevel      = UseItemInfo.dwProfessionLevel;

            g_pGameWorldUIHandler->OnUseItemRespond(UINotify);
        }
    }
#endif
    return nResult;
}

ENCHANT_RESULT_CODE KItemList::CanUnMountItem(DWORD dwBoxIndex, DWORD dwX, int nMountIndex)
{
    ENCHANT_RESULT_CODE nResult             = ercFailed;
    KItem*              pItem               = NULL;
    KENCHANT*           pCurrentEnchant     = NULL;
    int                 nFreeRoomSize       = 0;

    pItem = GetItem(dwBoxIndex, dwX);
    KGLOG_PROCESS_ERROR(pItem && pItem->m_Common.nGenre == igEquipment);

    KGLOG_PROCESS_ERROR(nMountIndex >= eiMount1 && nMountIndex <= eiMount4);
    KGLOG_PROCESS_ERROR(pItem->m_dwEnchantID[nMountIndex] > 0);

    pCurrentEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(pItem->m_dwEnchantID[nMountIndex]);
    KGLOG_PROCESS_ERROR(pCurrentEnchant);
    
    if (pCurrentEnchant->dwTabType > 0 && pCurrentEnchant->dwTabIndex > 0)
    {
        nFreeRoomSize = GetAllPackageFreeRoomSize();
        KG_PROCESS_ERROR_RET_CODE(nFreeRoomSize > 0, ercPackageIsFull);
    }

    nResult = ercSuccess;
Exit0:
    return nResult;
}

BOOL KItemList::SetEquipColor(DWORD dwBoxIndex, DWORD dwPos, int nColorID)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    KItem*      pItem               = NULL;
    KItemInfo*  pItemInfo           = NULL;
    int         nRepresentIndex     = 0;

	pItem = GetItem(dwBoxIndex, dwPos);
	KGLOG_PROCESS_ERROR(pItem);
    
    KGLOG_PROCESS_ERROR(pItem->m_Common.nGenre == igEquipment);

    pItemInfo = pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    switch (pItem->m_GenParam.dwTabType)
    {
    case ittCustWeapon:
    case ittCustArmor:
    case ittCustTrinket:
        KGLOG_PROCESS_ERROR(((KCustomEquipInfo*)pItemInfo)->bCanSetColor);
        break;

    default:
        KGLogPrintf(KGLOG_DEBUG, "Invalid TabType %d\n", pItem->m_GenParam.dwTabType);
        goto Exit0;
        break;
    }

    pItem->m_nColorID = nColorID;

    if (pItem->m_bEquiped)
    {
        bRetCode = GetEquipColorIndex(dwPos, &nRepresentIndex);
        if (bRetCode)
        {
            SetRepresentID(nRepresentIndex, nColorID);
        }
    }

#ifdef _SERVER
	g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, dwBoxIndex, dwPos);
#endif

    bResult = true;
Exit0:
    return bResult;
}

KInventory* KItemList::GetAllInventory()
{
    return m_Box;
}

KInventory* KItemList::GetInventory(DWORD dwIndex)
{
    KInventory* pResult = NULL;
    
    KGLOG_PROCESS_ERROR(dwIndex >= ibEquip && dwIndex < ibTotal);
    
    pResult = &m_Box[dwIndex];
Exit0:
    return pResult;
}

void KItemList::OnChangeCamp()
{
    for (DWORD dwX = 0; dwX < m_Box[ibEquip].m_dwSize; ++dwX)
    {
        KItem*      pItem       = m_Box[ibEquip].GetItem(dwX);
        KItemInfo*  pItemInfo   = NULL;

        if (pItem == NULL)
            continue;

        if (pItem->GetGenre() != igEquipment)
            continue;
        
        if (dwX == eitHorse)
        {
            m_pPlayer->DownHorse();
            continue;
        }

        pItemInfo = pItem->GetItemInfo();
        KGLOG_PROCESS_ERROR(pItemInfo);
        
        // 不能装备且应用了属性
        if (!(pItemInfo->nRequireCamp & (0x01 << m_pPlayer->m_eCamp)) && pItem->m_bApplyed)
        {
            UnApplyAttrib(pItem);
        }
    }

Exit0:
    return;
}

KItem* KItemList::FindItem(DWORD dwTabType, DWORD dwIndex, DWORD& dwBox, DWORD& dwX)
{
	KItem* pItem = NULL;

	for (int i = ibPackage; i <= ibPackage4; i++)
	{
		for (DWORD j = 0; j < m_Box[i].m_dwSize; j++)
		{
			pItem = m_Box[i].GetItem(j);
			if (pItem && pItem->m_GenParam.dwTabType == dwTabType && pItem->m_GenParam.dwIndex == dwIndex)
			{
				dwBox = i;
				dwX = j;
				return pItem;
			}
		}
	}
	return pItem;
}
#ifdef _SERVER
void KItemList::CallItemScript(KItem* pItem, const TItemPos& crItemPos, KTarget& rTarget, BOOL& rbContinueSkill, BOOL& rbDestroyItem)
{
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    TARGET_TYPE eType;

    assert(pItem);

    KG_PROCESS_ERROR(pItem->m_dwScriptID != 0);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pItem->m_dwScriptID);
    KG_PROCESS_ERROR(bRetCode);
    
    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pItem->m_dwScriptID, SCRIPT_ON_USE_ITEM);
    KG_PROCESS_ERROR(bRetCode);
        
    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
	g_pSO3World->m_ScriptCenter.PushValueToStack(pItem);
	g_pSO3World->m_ScriptCenter.PushValueToStack(crItemPos.dwBox);
	g_pSO3World->m_ScriptCenter.PushValueToStack(crItemPos.dwX);
    
    eType = rTarget.GetTargetType();
    
    if (eType == ttItemPos)
    {   
        KPlayer*    pTargetPlayer   = NULL;
		DWORD       dwTargetBox     = 0;
		DWORD       dwTargetX       = 0;

		bRetCode = rTarget.GetTarget(&pTargetPlayer, &dwTargetBox, &dwTargetX);
		KG_PROCESS_ERROR(bRetCode);

		g_pSO3World->m_ScriptCenter.PushValueToStack(dwTargetBox);
		g_pSO3World->m_ScriptCenter.PushValueToStack(dwTargetX);
	}

    g_pSO3World->m_ScriptCenter.CallFunction(pItem->m_dwScriptID, SCRIPT_ON_USE_ITEM, 2);
	g_pSO3World->m_ScriptCenter.GetValuesFromStack("bb", &rbContinueSkill, &rbDestroyItem);
    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
    
Exit0:
    return;
}

USE_ITEM_RESULT_CODE KItemList::CastItemSkill(KItem* pItem, KTarget& rTarget, BOOL* pbCostItemAtOnce)
{
    USE_ITEM_RESULT_CODE    nResult         = uircFailed;
    KSkill*                 pSkill          = NULL;
    SKILL_RESULT_CODE       nSkillResult    = srcInvalid;
    
    assert(pItem);
    assert(pbCostItemAtOnce);

    KG_PROCESS_SUCCESS(pItem->m_dwSkillID == INVALID_SKILL_ID);

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(pItem->m_dwSkillID, pItem->m_dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkill);

    if (m_pPlayer->m_OTActionParam.eActionType == otActionSkillChannel)
    {
        g_PlayerServer.DoSkillBeatBack(m_pPlayer, 0, true);
        m_pPlayer->DoActionIdle();
    }

    KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_OTActionParam.eActionType == otActionIdle, uircCannotCastSkill);

    if (pSkill->m_nPrepareFrames > 0)
    {
        KOT_ACTION_PARAM Param;

        nSkillResult = pSkill->CanCast(m_pPlayer, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nSkillResult == srcSuccess, uircCastSkillFailed);

        Param.eActionType       = otActionUseItemPrepare;
        Param.Data.dwItemID     = pItem->m_dwID;
        Param.nEndFrame         = g_pSO3World->m_nGameLoop + pSkill->m_nPrepareFrames;
        Param.nNextActFrame     = 0;
        Param.nBeatBackCount    = 0;
        Param.Target            = rTarget;

        m_pPlayer->m_SkillTarget = rTarget;
        m_pPlayer->DoOTAction(Param);

        g_PlayerServer.DoSkillPrepare(
            m_pPlayer, pItem->m_dwSkillID, pItem->m_dwSkillLevel,
            pSkill->m_nPrepareFrames, rTarget
        );

        *pbCostItemAtOnce = false;
    }
    else
    {
        nSkillResult = m_pPlayer->CastSkill(pItem->m_dwSkillID, pItem->m_dwSkillLevel, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nSkillResult == srcSuccess, uircCastSkillFailed);    
        *pbCostItemAtOnce = true;
    }

Exit1:
    nResult = uircSuccess;
Exit0:
    return nResult;
}
#endif

void KItemList::SetRepresentID(int nRepresentIndex, int nRepresentID)
{
    KG_PROCESS_ERROR(nRepresentIndex >= perFaceStyle && nRepresentIndex < perRepresentCount);
    
    KG_PROCESS_ERROR(!(m_pPlayer->m_dwRepresentIdLock & (0x1 << nRepresentIndex)));
    
    if (nRepresentIndex == perHelmStyle)
    {
        KG_PROCESS_ERROR(!m_pPlayer->m_bHideHat);
    }

#ifdef _SERVER
    if (m_pPlayer->m_wRepresentId[nRepresentIndex] != (WORD)nRepresentID)
    {
        g_PlayerServer.DoSyncEquipRepresent(m_pPlayer, nRepresentIndex, nRepresentID);
    }
#endif

    m_pPlayer->m_wRepresentId[nRepresentIndex] = (WORD)nRepresentID;
    
Exit0:
	return;
}

BOOL KItemList::CheckEquipRequire(void)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    for (DWORD dwX = eitMeleeWeapon; dwX < eitTotal; dwX++)
    {
        KItem* pItem = m_Box[ibEquip].GetItem(dwX);
        if (pItem == NULL)
        {
            continue;
        }

        bRetCode = CanEquip(pItem, dwX);
        if (bRetCode != ircSuccess)
        {
            if (pItem->m_bApplyed)
            {
                UnApplyAttrib(pItem);
            }
        }
    }

    bResult = true;
//Exit0:
    return bResult;
}

BOOL KItemList::GetEquipRepresentIndex(DWORD dwEquipPos, int* pnIndex)
{
    BOOL bResult = false;

    switch (dwEquipPos)
    {
    case eitMeleeWeapon:
        *pnIndex = perWeaponStyle;
        break;

    case eitHelm:
        *pnIndex = perHelmStyle;
        break;

    case eitChest:
        *pnIndex = perChestStyle;
        break;

    case eitWaist:
        *pnIndex = perWaistStyle;
        break;

    case eitBoots:
        *pnIndex = perBootsStyle;
        break;

    case eitBangle:
        *pnIndex = perBangleStyle;
        break;

    case eitWaistExtend:
        *pnIndex = perWaistExtend;
        break;

    case eitBackExtend:
        *pnIndex = perBackExtend;
        break;

    case eitHorse:
        *pnIndex = perHorseStyle;
        break;

	default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KItemList::GetEquipColorIndex(DWORD dwEquipPos, int* pnIndex)
{
    BOOL bResult = false;

    switch (dwEquipPos)
    {
    case eitHelm:
        *pnIndex = perHelmColor;
        break;

    case eitChest:
        *pnIndex = perChestColor;
        break;

    case eitWaist:
        *pnIndex = perWaistColor;
        break;

    case eitBoots:
        *pnIndex = perBootsColor;
        break;

    case eitBangle:
        *pnIndex = perBangleColor;
        break;

	default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KItemList::PushItem(KItem* pItem, DWORD dwBox, BOOL bNotifyClient)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nCount          = 0;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    KGLOG_PROCESS_ERROR(g_IsPackage(dwBox));
    KGLOG_PROCESS_ERROR(pItem);

    assert(m_pPlayer);

    nCount = pItem->GetStackNum();

    for (DWORD i = 0; i < m_Box[dwBox].m_dwSize; i++)
    {
        if (m_Box[dwBox].m_pItemArray[i] && m_Box[dwBox].m_pItemArray[i]->CanStack(pItem))
        {
            int nOldCount = nCount;
            nCount = m_Box[dwBox].m_pItemArray[i]->Stack(nCount);
#ifdef _SERVER
            g_PlayerServer.DoUpdateItemAmount(
                m_pPlayer->m_nConnIndex, dwBox, i, m_Box[dwBox].m_pItemArray[i]->GetStackNum()
            );
            if (bNotifyClient)
            {
			    g_PlayerServer.DoAddItemNotify(
                    m_pPlayer, m_Box[dwBox].m_pItemArray[i]->m_dwID, nOldCount - nCount
                );
            }
#endif
            if (nCount == 0)
                break;
        }
    }

    bRetCode = pItem->SetStackNum(nCount);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

ITEM_RESULT_CODE KItemList::ExchangePackageSlot(DWORD dwBox1, DWORD dwBox2)
{
	ITEM_RESULT_CODE    nResult     = ircFailed;
    KInventory          ivTemp;

	KGLOG_PROCESS_ERROR(dwBox1 < ibTotal && dwBox2 < ibTotal);
	KGLOG_PROCESS_ERROR(dwBox1 != dwBox2);
    
	ivTemp = m_Box[dwBox1];
	
    m_Box[dwBox1].m_dwSize                  = m_Box[dwBox2].m_dwSize;
    m_Box[dwBox1].m_nContainItemGenerType   = m_Box[dwBox2].m_nContainItemGenerType;
    m_Box[dwBox1].m_nContainItemSubType     = m_Box[dwBox2].m_nContainItemSubType;

    memcpy(m_Box[dwBox1].m_pItemArray, m_Box[dwBox2].m_pItemArray, sizeof(m_Box[dwBox1].m_pItemArray));

    m_Box[dwBox2].m_dwSize                  = ivTemp.m_dwSize;
    m_Box[dwBox2].m_nContainItemGenerType   = ivTemp.m_nContainItemGenerType;
    m_Box[dwBox2].m_nContainItemSubType     = ivTemp.m_nContainItemSubType;

    memcpy(m_Box[dwBox2].m_pItemArray, ivTemp.m_pItemArray, sizeof(m_Box[dwBox2].m_pItemArray));

	nResult = ircSuccess;
Exit0:
	return nResult;
}

ITEM_RESULT_CODE KItemList::EnablePackage(DWORD dwBox, DWORD dwSize, int nContainGenerType, int nContainSubType)
{
	ITEM_RESULT_CODE    nResult     = ircFailed;
    DWORD               dwOldSize   = 0;
    DWORD               dwUsedSize  = 0;
    KInventory*         pBox        = NULL;

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    
    pBox = &m_Box[dwBox];
    dwUsedSize = GetBoxUsedSize(dwBox);
    
    if (nContainGenerType != INVALID_CONTAIN_ITEM_TYPE)
    {
        if (nContainGenerType != pBox->m_nContainItemGenerType)
        {
            KG_PROCESS_ERROR_RET_CODE(dwUsedSize == 0, ircPackageErrorType);
        }

        if (nContainSubType != INVALID_CONTAIN_ITEM_TYPE && nContainSubType != pBox->m_nContainItemSubType)
        {
            KG_PROCESS_ERROR_RET_CODE(dwUsedSize == 0, ircPackageErrorType);
        }
    }
    
    KG_PROCESS_ERROR_RET_CODE(dwUsedSize <= dwSize, ircPackageNotEnoughSize);
    
    pBox->SetContainItemType(nContainGenerType, nContainSubType);

	dwOldSize = m_Box[dwBox].m_dwSize;
	// 重排
	if (dwOldSize > dwSize)
	{
		DWORD dwFirstFree = 0;
		DWORD dwFirstOver = dwSize;
		while(true)
		{
			DWORD i, j;
			for (j = dwFirstOver; j < dwOldSize; j++)
			{
				if (m_Box[dwBox].GetItem(j))
				{
					dwFirstOver = j;
					break;
				}
			}

			if (j == dwOldSize)
				break;

			for (i = dwFirstFree; i < dwSize; i++)
			{
				if (m_Box[dwBox].GetItem(i) == NULL)
				{
					dwFirstFree = i;
					break;
				}	
			}
			assert(i < dwSize);
			KItem* pItem = m_Box[dwBox].PickUpItem(dwFirstOver);
			m_Box[dwBox].PlaceItem(pItem, dwFirstFree);
		}
	}
	
	m_Box[dwBox].m_dwSize = dwSize;

	nResult = ircSuccess;
Exit0:
	return nResult;
}

ITEM_RESULT_CODE KItemList::DisablePackage(DWORD dwBox)
{
	ITEM_RESULT_CODE nResult = ircFailed;
    DWORD            dwUsedCount = 0;

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    
    dwUsedCount = GetBoxUsedSize(dwBox);
    KGLOG_PROCESS_ERROR(dwUsedCount == 0);

	m_Box[dwBox].m_nContainItemGenerType = INVALID_CONTAIN_ITEM_TYPE;
    m_Box[dwBox].m_nContainItemSubType   = INVALID_CONTAIN_ITEM_TYPE;
	m_Box[dwBox].m_dwSize = 0;

	nResult = ircSuccess;
Exit0:
	return nResult;
}

int KItemList::GetEquipedSetNum(DWORD dwSetID)
{
	int nResult = 0;

	for (int i = 0; i < (int)(m_Box[ibEquip].m_dwSize); i++)
	{
		KItem* pItem = GetItem(ibEquip, i);
		if (pItem && pItem->m_bApplyed && pItem->m_Common.dwSetID == dwSetID)
		{
			nResult++;
		}
	}
	return nResult;
}

BOOL KItemList::ApplySetAttrib(DWORD dwSetID, int nSetNum)
{
	BOOL	bResult = false;
	BOOL	bRetCode = false;

	KAttribute	Attrib;
	bRetCode = g_pSO3World->m_ItemManager.GetSetAttrib(Attrib, dwSetID, nSetNum);
	KG_PROCESS_ERROR(bRetCode);

	m_pPlayer->ApplyAttribute(&Attrib);

	bResult = bRetCode;
Exit0:
	return bResult;
}

BOOL KItemList::UnApplySetAttrib(DWORD dwSetID, int nSetNum)
{
	BOOL	bResult = false;
	BOOL	bRetCode = false;

	KAttribute	Attrib;
	bRetCode = g_pSO3World->m_ItemManager.GetSetAttrib(Attrib, dwSetID, nSetNum);
	KG_PROCESS_ERROR(bRetCode);

	m_pPlayer->UnApplyAttribute(&Attrib);

	bResult = bRetCode;
Exit0:
	return bResult;
}

BOOL KItemList::CostItemInBox(int nBoxIndex, DWORD dwTabType, DWORD dwIndex, DWORD& rdwAmount, int nBookRecipeID)
{
    BOOL                bResult         = false;
    ITEM_RESULT_CODE    eRetCode        = ircFailed;
    
    KGLOG_PROCESS_ERROR(nBoxIndex >= 0 && nBoxIndex < ibTotal);

    for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; ++dwX)
    {
        KItem*      pItem   = m_Box[nBoxIndex].GetItem(dwX);
        TItemPos    ItemPos = {nBoxIndex, dwX};

        if (rdwAmount == 0)
            break;

        if (pItem == NULL)
            continue;

        if (pItem->m_GenParam.dwTabType != dwTabType || pItem->m_GenParam.dwIndex != dwIndex)
            continue;
        
        if (nBookRecipeID != -1)
        {
            if (pItem->m_Common.nGenre != igBook || pItem->m_nCurrentDurability != nBookRecipeID)
            {
                continue;
            }
        }
		if (pItem->IsStackable())
		{
			if ((DWORD)pItem->m_nStackNum > rdwAmount)
			{
				pItem->m_nStackNum -= (int)rdwAmount;
                #ifdef _SERVER
				g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, nBoxIndex, dwX, pItem->GetStackNum());
                #endif
                rdwAmount = 0;
				goto Exit1;
			}

			if ((DWORD)pItem->m_nStackNum == rdwAmount)
			{
				eRetCode = DestroyItem(ItemPos);
                KGLOG_PROCESS_ERROR(eRetCode == ircSuccess);
                rdwAmount = 0;
				goto Exit1;
			}

			rdwAmount -= pItem->m_nStackNum;

			eRetCode = DestroyItem(ItemPos);
            KGLOG_PROCESS_ERROR(eRetCode == ircSuccess);
		}
		else
		{
			eRetCode = DestroyItem(ItemPos);
            KGLOG_PROCESS_ERROR(eRetCode == ircSuccess);
			--rdwAmount;
		}
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

USE_ITEM_RESULT_CODE KItemList::CanUseItem(
    KItem* pItem, DWORD dwBox, KUSE_ITEM* pUseItemInfo, KTarget& rTarget
)
{
    USE_ITEM_RESULT_CODE nResult        = uircFailed;
    BOOL                 bRetCode       = false;
    DWORD                dwTabType      = 0;
    KOtherItemInfo*      pItemInfo      = NULL;

    assert(pItem);
    assert(pUseItemInfo);

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);

    pItemInfo = (KOtherItemInfo*)pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    KG_PROCESS_ERROR(
        pItem->m_dwScriptID != 0 || pItem->m_dwSkillID != 0 || pItemInfo->dwBoxTemplateID != 0 ||
        pItem->m_Common.nGenre == igDesignation || pItem->m_Common.nGenre == igMountItem
    );
    
    if (pItem->m_Common.nGenre == igEquipment)
    {
        KGLOG_PROCESS_ERROR(dwBox == ibEquip);
    }
    else
    {
        KGLOG_PROCESS_ERROR(g_IsPackage(dwBox));
    }

    if (pItem->m_Common.bConsumeDurabiltity)
    {
        int nCurrentDurability = pItem->GetCurrentDurability();
        KG_PROCESS_ERROR_RET_CODE(nCurrentDurability != 0, uircNotEnoughDurability);
    }
    
    if (pItemInfo->dwCoolDownID)
    {
        bRetCode = m_pPlayer->m_TimerList.CheckTimer(pItemInfo->dwCoolDownID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, uircNotReady);
    }

    dwTabType = pItem->m_GenParam.dwTabType;
    KG_PROCESS_SUCCESS(dwTabType != ittOther);

    KG_PROCESS_ERROR_RET_CODE(pItemInfo->nRequireLevel == 0 || m_pPlayer->m_nLevel >= pItemInfo->nRequireLevel, uircTooLowLevel);
    
    if (pItemInfo->dwRequireProfessionID != 0)
    {
        pUseItemInfo->dwProfessionID = pItemInfo->dwRequireProfessionID;

        bRetCode = m_pPlayer->m_ProfessionList.IsProfessionLearned(pItemInfo->dwRequireProfessionID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, uircRequireProfession);

        if (pItemInfo->dwRequireProfessionBranch != 0)
        {
            DWORD dwBranchID = 0;
            dwBranchID = m_pPlayer->m_ProfessionList.GetBranchID(pItemInfo->dwRequireProfessionID);

            pUseItemInfo->dwProfessionBranchID = pItemInfo->dwRequireProfessionBranch;
            KG_PROCESS_ERROR_RET_CODE(dwBranchID == pItemInfo->dwRequireProfessionBranch, uircRequireProfessionBranch);
        }

        if (pItemInfo->nRequireProfessionLevel != 0)
        {
            int nProfessionLevel = 0;
            nProfessionLevel = m_pPlayer->m_ProfessionList.GetProfessionLevel(pItemInfo->dwRequireProfessionID);
            nProfessionLevel += m_pPlayer->m_ProfessionList.GetProfessionAdjustLevel(pItemInfo->dwRequireProfessionID);

            pUseItemInfo->dwProfessionLevel = pItemInfo->nRequireProfessionLevel;

            KG_PROCESS_ERROR_RET_CODE(nProfessionLevel >= pItemInfo->nRequireProfessionLevel, uircProfessionLevelTooLow);
        }
    }
    
    if (pItemInfo->nRequireGender != 0)
    {
        if (m_pPlayer->m_eGender == cgMale)
        {
            KG_PROCESS_ERROR_RET_CODE(pItemInfo->nRequireGender == m_pPlayer->m_eGender, uircRequireFeMale);
        }
        else if (m_pPlayer->m_eGender == cgFemale)
        {
            KG_PROCESS_ERROR_RET_CODE(pItemInfo->nRequireGender == m_pPlayer->m_eGender, uircRequireMale);
        }
    }
    
    KG_PROCESS_ERROR_RET_CODE(!m_pPlayer->m_bOnHorse || pItemInfo->bCanUseOnHorse != 0, uircOnHorse);
    KG_PROCESS_ERROR_RET_CODE(!m_pPlayer->m_bFightState || pItemInfo->bCanUseInFight != 0, uircInFight);
    KG_PROCESS_ERROR_RET_CODE(pItemInfo->nRequireCamp & (0x01 << m_pPlayer->m_eCamp), uircCampCannotUse);
    KG_PROCESS_ERROR_RET_CODE(
        pItemInfo->dwRequireForceID == 0 || pItemInfo->dwRequireForceID == m_pPlayer->m_dwForceID, 
        uircForceError
    );

    if (pItem->m_dwSkillID && pItem->m_dwSkillLevel)
    {
        KSKILL_RECIPE_KEY   SkillRecipeKey;
        KSkill*             pSkill = NULL;

        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(
            &SkillRecipeKey, pItem->m_dwSkillID, pItem->m_dwSkillLevel, m_pPlayer
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
        KG_PROCESS_ERROR(pSkill);

        bRetCode = pSkill->CanCast(m_pPlayer, rTarget);
        KG_PROCESS_ERROR_RET_CODE(bRetCode == srcSuccess, uircCastSkillFailed);
    }

Exit1:
    nResult = uircSuccess;
Exit0:
    return nResult;
}

BOOL KItemList::CheckUseItemTargetType(KItem* pItem)
{
    BOOL        bResult     = false;
    TARGET_TYPE eType       = ttInvalid;
    DWORD       dwTargetID  = ERROR_ID;
    KItemInfo*  pItemInfo   = NULL;

    assert(pItem);
    assert(m_pPlayer);
    
    eType = m_pPlayer->m_SelectTarget.GetTargetType();
    KGLOG_PROCESS_ERROR(eType != ttInvalid);
    
    dwTargetID = m_pPlayer->m_SelectTarget.GetTargetID();

    pItemInfo = pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);
    
    switch (pItemInfo->nUseTargetType)
    {
    case iuttNoTarget:
        break;
    case iuttSelf:
        KGLOG_PROCESS_ERROR(eType == ttPlayer && dwTargetID == m_pPlayer->m_dwID);
    	break;
    case iuttOtherPlayer:
        KGLOG_PROCESS_ERROR(eType == ttPlayer && dwTargetID != m_pPlayer->m_dwID);
        break;
    case iuttNpc:
        KGLOG_PROCESS_ERROR(eType == ttNpc);
        break;
    case iuttCoordinates:
        KGLOG_PROCESS_ERROR(eType == ttCoordination);
    	break;
    case iuttDoodad:
        KGLOG_PROCESS_ERROR(eType == ttDoodad);
    	break;
    case iuttItem:
        KGLOG_PROCESS_ERROR(eType == ttItemPos);
        break;
    default:
        assert(false);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
USE_ITEM_RESULT_CODE KItemList::UseDesignationItem(DWORD dwBox, DWORD dwX)
{
    USE_ITEM_RESULT_CODE    nResult     = uircFailed;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    KItem*                  pItem       = NULL;    
    KOtherItemInfo*         pItemInfo   = NULL;
    TItemPos                ItemPos     = {dwBox, dwX};
    KUSE_ITEM               UseItemInfo;
    KTarget                 Target;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    
    Target.ClearTarget();

    pItem = GetItem(dwBox, dwX);
    KGLOG_PROCESS_ERROR(pItem);
    
    KGLOG_PROCESS_ERROR(pItem->m_Common.nGenre == igDesignation);
    KGLOG_PROCESS_ERROR(pItem->m_GenParam.dwTabType == ittOther);

    pItemInfo = (KOtherItemInfo*)pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);
    
    ZeroMemory(&UseItemInfo, sizeof(KUSE_ITEM));
    eRetCode = CanUseItem(pItem, dwBox, &UseItemInfo, Target);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);
    
    assert(m_pPlayer);

    if (pItemInfo->nPrefix > 0)
    {
        bRetCode = m_pPlayer->m_Designation.IsPrefixAcquired(pItemInfo->nPrefix);
        KG_PROCESS_ERROR_RET_CODE(!bRetCode, uircAleardHaveDesignation);
    }
    
    if (pItemInfo->nPostfix > 0)
    {
        bRetCode = m_pPlayer->m_Designation.IsPostfixAcquired(pItemInfo->nPostfix);
        KG_PROCESS_ERROR_RET_CODE(!bRetCode, uircAleardHaveDesignation);
    }
    
    if (pItemInfo->nPrefix > 0)
    {
        bRetCode = m_pPlayer->m_Designation.AcquirePrefix(pItemInfo->nPrefix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pItemInfo->nPostfix > 0)
    {
        bRetCode = m_pPlayer->m_Designation.AcquirePostfix(pItemInfo->nPostfix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bRetCode = DestroyItem(ItemPos);
    KGLOG_PROCESS_ERROR(bRetCode == uircSuccess);

    nResult = uircSuccess;
Exit0:
    if (nResult != uircSuccess)
    {
        g_PlayerServer.DoMessageNotify(
            m_pPlayer->m_nConnIndex, ectUseItemErrorCode, nResult, &UseItemInfo, sizeof(UseItemInfo)
        );
    }
    return nResult;
}

USE_ITEM_RESULT_CODE KItemList::UseItem(TItemPos ItemPos, KTarget& rTarget)
{
    USE_ITEM_RESULT_CODE    nResult         = uircFailed;
    USE_ITEM_RESULT_CODE    eRetCode        = uircFailed;
    BOOL                    bRetCode        = false;
    BOOL                    bCostItemAtOnce = true;
	BOOL                    bDestroyItem    = false;
	BOOL                    bContinueSkill  = true;    
    KItem*                  pItem           = NULL;
    KUSE_ITEM               UseItemInfo;
    KITEM_LOG_INFO          ItemLogInfo;
    
    assert(m_pPlayer);
    KGLOG_PROCESS_ERROR(ItemPos.dwBox < ibTotal);

    pItem = GetItem(ItemPos.dwBox, ItemPos.dwX);
    KG_PROCESS_ERROR_RET_CODE(pItem, uircItemNotExist);
    g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);

    eRetCode = CanUseItem(pItem, ItemPos.dwBox, &UseItemInfo, rTarget);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);

    CallItemScript(pItem, ItemPos, rTarget, bContinueSkill, bDestroyItem);
    
    pItem = GetItem(ItemPos.dwBox, ItemPos.dwX); // 防止脚本中删除道具导致宕机
    KGLOG_PROCESS_ERROR(pItem);

	if (bContinueSkill)
    {
        eRetCode = CastItemSkill(pItem, rTarget, &bCostItemAtOnce);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);
    }

    bRetCode = pItem->ResetCoolDown(m_pPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (bContinueSkill) // 释放技能要消耗
    {
        if (!bCostItemAtOnce) // 吟唱技能不能立刻消耗物品
        {
            goto Exit1;
        }

        if (pItem->m_Common.bConsumeDurabiltity)
        {
            bRetCode = CostSingleItem(ItemPos.dwBox, ItemPos.dwX, 1);
		    KGLOG_PROCESS_ERROR_RET_CODE(bRetCode,  uircCostItemDurabilityFailed);
            g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, &ItemLogInfo, "use item");

            goto Exit1;
        }
    }

	if (bDestroyItem) // 脚本返回要消耗
    {
        if (pItem->m_Common.bConsumeDurabiltity)
        {
            bRetCode = CostSingleItem(ItemPos.dwBox, ItemPos.dwX, 1);
		    KGLOG_PROCESS_ERROR_RET_CODE(bRetCode,  uircCostItemDurabilityFailed);
            g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, &ItemLogInfo, "use item");
            goto Exit1;
        }

        bRetCode = DestroyItem(ItemPos);
        KGLOG_PROCESS_ERROR(bRetCode == uircSuccess);
        g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, &ItemLogInfo, "use item");
    }

Exit1:
    nResult = uircSuccess;
Exit0:
    if (nResult != uircSuccess)
    {
        g_PlayerServer.DoMessageNotify(
            m_pPlayer->m_nConnIndex, ectUseItemErrorCode, nResult, &UseItemInfo, sizeof(UseItemInfo)
        );
    }
    return nResult;
}

USE_ITEM_RESULT_CODE KItemList::UseItem(DWORD dwBox, DWORD dwX, KTarget& rTarget)
{
    USE_ITEM_RESULT_CODE    nResult         = uircFailed;
    USE_ITEM_RESULT_CODE    eRetCode        = uircFailed;
    BOOL                    bRetCode        = false;
    KItem*                  pItem           = NULL;
    KUSE_ITEM               UseItemInfo;

    assert(m_pPlayer);
    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    
    KGLOG_PROCESS_ERROR(m_pPlayer->m_eMoveState != cmsOnDeath);

    pItem = GetItem(dwBox, dwX);
    KG_PROCESS_ERROR(pItem);

    eRetCode = CanUseItem(pItem, dwBox, &UseItemInfo, rTarget);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);
    
    if (pItem->m_dwSkillID == 0 && pItem->m_dwScriptID != 0)
    {
        eRetCode = UseItemScripts(pItem, rTarget);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);
    }
    else if (pItem->m_dwSkillID != 0 && pItem->m_dwScriptID == 0)
    {
        eRetCode = UseItemSkill(pItem, rTarget);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);
    }
    else if (pItem->m_dwSkillID != 0 && pItem->m_dwScriptID != 0)
    {
        eRetCode = UseItemScriptsAndSkill(pItem, rTarget);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);
    }

    nResult = uircSuccess;
Exit0:
    if (nResult != uircSuccess)
    {
        g_PlayerServer.DoMessageNotify(
            m_pPlayer->m_nConnIndex, ectUseItemErrorCode, nResult, &UseItemInfo, sizeof(UseItemInfo)
        );
    }
    return nResult;
}

USE_ITEM_RESULT_CODE KItemList::UseItemScripts(KItem* pSrcItem, KTarget& rTarget)
{
    USE_ITEM_RESULT_CODE    nResult     = uircFailed;
    BOOL                    bRetCode    = false;
    KSTART_USE_ITEM_PARAM   StartUseItemParam;
    KOT_ACTION_PARAM        Param;

    assert(pSrcItem);
    
    bRetCode = CallStartUseItemScripts(pSrcItem, rTarget, StartUseItemParam);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    Param.eActionType       = otActionUseItemPrepare;
    Param.Data.dwItemID     = pSrcItem->m_dwID;
    Param.nEndFrame         = g_pSO3World->m_nGameLoop + StartUseItemParam.nFrames;
    Param.Target            = rTarget;
    
    bRetCode = m_pPlayer->DoOTAction(Param);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoUseItemPrepare(
        m_pPlayer, StartUseItemParam.nFrames, StartUseItemParam.dwOTAniID, 
        StartUseItemParam.dwEndAniID, StartUseItemParam.szText
    );

    nResult = uircSuccess;
Exit0:
    return nResult;
}

USE_ITEM_RESULT_CODE KItemList::UseItemSkill(KItem* pSrcItem, KTarget& rTarget)
{
    USE_ITEM_RESULT_CODE    nResult         = uircFailed;
    int                     nRetCode        = false;
    BOOL                    bRetCode        = false;
    KSkill*                 pSkill          = NULL;
    SKILL_RESULT_CODE       nSkillResult    = srcInvalid;
    KSKILL_RECIPE_KEY       RecipeKey;
    KSkillRecipePointer     SkillRecipePointer;

    assert(pSrcItem);
    
    bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(
        &RecipeKey, pSrcItem->m_dwSkillID, pSrcItem->m_dwSkillLevel, m_pPlayer
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    pSkill = SkillRecipePointer.SetRecipeKey(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);
    
    nSkillResult = pSkill->CanCast(m_pPlayer, rTarget);
    KG_PROCESS_ERROR_RET_CODE(nSkillResult == srcSuccess, uircCastSkillFailed);

    if (m_pPlayer->m_OTActionParam.eActionType == otActionSkillChannel)
    {
        g_PlayerServer.DoSkillBeatBack(m_pPlayer, 0, true);
        m_pPlayer->DoActionIdle();
    }

    KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_OTActionParam.eActionType == otActionIdle, uircCannotCastSkill);
    KGLOG_PROCESS_ERROR(!pSkill->m_pBaseInfo->bIsChannelSkill); // 不支持通道技

    pSkill->ResetPublicCooldown(m_pPlayer);

    if (pSkill->m_nPrepareFrames > 0)
    {
        KOT_ACTION_PARAM Param;

        Param.eActionType       = otActionUseItemPrepare;
        Param.Data.dwItemID     = pSrcItem->m_dwID;
        Param.nEndFrame         = g_pSO3World->m_nGameLoop + pSkill->m_nPrepareFrames;
        Param.nNextActFrame     = 0;
        Param.nBeatBackCount    = 0;
        Param.Target            = rTarget;

        m_pPlayer->m_SkillTarget = rTarget;
        m_pPlayer->DoOTAction(Param);

        g_PlayerServer.DoSkillPrepare(
            m_pPlayer, pSrcItem->m_dwSkillID, pSrcItem->m_dwSkillLevel,
            pSkill->m_nPrepareFrames, rTarget
        );
    }
    else
    {
        nSkillResult = m_pPlayer->CastSkill(pSrcItem->m_dwSkillID, pSrcItem->m_dwSkillLevel, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nSkillResult == srcSuccess, uircCastSkillFailed);

        if (pSrcItem->m_Common.bConsumeDurabiltity)
        {
            DWORD dwBox = ibInvalid;
            DWORD dwX   = 0;
            
            bRetCode = GetItemPos(pSrcItem->m_dwID, dwBox, dwX);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = CostSingleItem(dwBox, dwX, 1);
		    KGLOG_PROCESS_ERROR_RET_CODE(bRetCode,  uircCostItemDurabilityFailed);
        }
    }

    nResult = uircSuccess;
Exit0:
    return nResult;
}

USE_ITEM_RESULT_CODE KItemList::UseItemScriptsAndSkill(KItem* pSrcItem, KTarget& rTarget)
{
    USE_ITEM_RESULT_CODE    nResult     = uircFailed;
    BOOL                    bRetCode    = false;
    KSTART_USE_ITEM_PARAM   StartUseItemParam;

    assert(pSrcItem);
    
    bRetCode = CallStartUseItemScripts(pSrcItem, rTarget, StartUseItemParam);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (StartUseItemParam.nFrames > 0)
    {
        KOT_ACTION_PARAM        Param;

        Param.eActionType       = otActionUseItemPrepare;
        Param.Data.dwItemID     = pSrcItem->m_dwID;
        Param.nEndFrame         = g_pSO3World->m_nGameLoop + StartUseItemParam.nFrames;
        Param.Target            = rTarget;
        
        bRetCode = m_pPlayer->DoOTAction(Param);
        KGLOG_PROCESS_ERROR(bRetCode);

        g_PlayerServer.DoUseItemPrepare(
            m_pPlayer, StartUseItemParam.nFrames, StartUseItemParam.dwOTAniID, 
            StartUseItemParam.dwEndAniID, StartUseItemParam.szText
        );

        goto Exit1;
    }
    
    UseItemSkill(pSrcItem, rTarget);

Exit1:
    nResult = uircSuccess;
Exit0:
    return nResult;
}

BOOL KItemList::CallStartUseItemScripts(KItem* pSrcItem, KTarget& rTarget, KSTART_USE_ITEM_PARAM& rStartUseItemParam)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    char*   pszText     = NULL;
    DWORD   dwItemID    = ERROR_ID;

    assert(pSrcItem);
    
    KGLOG_PROCESS_ERROR(pSrcItem->m_dwScriptID);
    
    dwItemID = pSrcItem->m_dwID;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pSrcItem->m_dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pSrcItem->m_dwScriptID, START_USE_ITEM);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(pSrcItem);
    g_pSO3World->m_ScriptCenter.PushValueToStack(&rTarget);

    g_pSO3World->m_ScriptCenter.CallFunction(pSrcItem->m_dwScriptID, START_USE_ITEM, 6);

    g_pSO3World->m_ScriptCenter.GetValuesFromStack(
        "dddsbb", (int*)&rStartUseItemParam.dwOTAniID, (int*)&rStartUseItemParam.dwEndAniID, &rStartUseItemParam.nFrames, 
        &pszText, &rStartUseItemParam.bSetCD, &rStartUseItemParam.bCastSkill
    );
    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
    
    rStartUseItemParam.szText[0] = '\0';
    if (pszText)
    {
        strncpy(&rStartUseItemParam.szText[0], pszText, sizeof(rStartUseItemParam.szText));
        rStartUseItemParam.szText[sizeof(rStartUseItemParam.szText) - 1] = '\0';
    }
    
    KGLOG_PROCESS_ERROR(rStartUseItemParam.nFrames >= 0);
    
    pSrcItem = g_pSO3World->m_ItemSet.GetObj(dwItemID); // 防止脚本中删掉道具导致宕机
    KGLOG_PROCESS_ERROR(pSrcItem);

    if (rStartUseItemParam.bSetCD)
    {
        bRetCode = pSrcItem->ResetCoolDown(m_pPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

void KItemList::OnBreakUseItem(KItem* pItem)
{   
    BOOL                    bRetCode    = false;
    KSkill*                 pSkill      = NULL;
    KSKILL_RECIPE_KEY       RecipeKey;
    KSkillRecipePointer     SkillRecipePointer;

    //KGLOG_PROCESS_ERROR(pItem); // pItem could be null
    KGLOG_PROCESS_ERROR(m_pPlayer->m_OTActionParam.eActionType == otActionUseItemPrepare);
    
    g_PlayerServer.DoSkillBeatBack(m_pPlayer, 0, true);
    m_pPlayer->DoActionIdle();
    
    KG_PROCESS_ERROR(pItem);
    
    if (pItem->m_dwSkillID)
    {
        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(
            &RecipeKey, pItem->m_dwSkillID, pItem->m_dwSkillLevel, m_pPlayer
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        pSkill = SkillRecipePointer.SetRecipeKey(RecipeKey);
        KGLOG_PROCESS_ERROR(pSkill);

        pSkill->ClearPublicCooldown(m_pPlayer); 
    }
    
    KG_PROCESS_ERROR(pItem->m_dwScriptID);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pItem->m_dwScriptID);
    if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pItem->m_dwScriptID, USE_ITEM_FAILD))
    {   
        int     nTopIndex = 0;
        BOOL    bCostItem = false;
        BOOL    bSetCD    = false;

        g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

        g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
        g_pSO3World->m_ScriptCenter.PushValueToStack(pItem);
        g_pSO3World->m_ScriptCenter.PushValueToStack(&(m_pPlayer->m_OTActionParam.Target));

        g_pSO3World->m_ScriptCenter.CallFunction(pItem->m_dwScriptID, USE_ITEM_FAILD, 2);

        g_pSO3World->m_ScriptCenter.GetValuesFromStack("bb", &bCostItem, &bSetCD);
        g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

        if (bSetCD)
        {
            bRetCode = pItem->ResetCoolDown(m_pPlayer);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        if (bCostItem)
        {
            DWORD dwBox = 0;
            DWORD dwX   = 0;

            bRetCode = GetItemPos(pItem->m_dwID, dwBox, dwX);
            KGLOG_PROCESS_ERROR(bRetCode);

            if (pItem->m_Common.bConsumeDurabiltity)
            {
                bRetCode = CostSingleItem(dwBox, dwX, 1);
		        KGLOG_PROCESS_ERROR(bRetCode);
                
                goto Exit0;
            }
        }
    }
    
Exit0:
    return;
}
#endif

void KItemList::FinishUseItem(KItem* pItem)
{
    BOOL bRetCode       = false;
    
#ifdef _SERVER
    BOOL    bCostItem   = false;
    DWORD   dwBox       = 0;
    DWORD   dwX         = 0;

    assert(pItem);
    
    bRetCode = GetItemPos(pItem->m_dwID, dwBox, dwX);
    KG_PROCESS_ERROR(bRetCode);

    if (pItem->m_dwSkillID)
    {
        DWORD	            dwSkillID		= 0;
	    DWORD	            dwSkillLevel	= 0;
        KSkill*	            pSkill			= NULL;
        KSKILL_RECIPE_KEY   RecipeKey;
        SKILL_RESULT_CODE   eRetCode        = srcFailed;

        dwSkillID = pItem->m_dwSkillID;
	    dwSkillLevel = pItem->m_dwSkillLevel;

        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, m_pPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);
    	
	    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
	    KGLOG_PROCESS_ERROR(pSkill);

        KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode != scmItem); // 对道具使用的技能不能走到该流程。

        eRetCode = pSkill->CanCastOnPrepare(m_pPlayer, m_pPlayer->m_OTActionParam.Target);
	    if (eRetCode != srcSuccess)
	    {
            OnBreakUseItem(pItem);
		    g_PlayerServer.DoMessageNotify(m_pPlayer->m_nConnIndex, ectSkillErrorCode, eRetCode);
		    goto Exit0;
	    }

	    pSkill->DoCost(m_pPlayer);
        pSkill->ResetNormalCooldown(m_pPlayer);

	    g_PlayerServer.DoSkillCast(m_pPlayer, pSkill, m_pPlayer->m_OTActionParam.Target, false);

	    eRetCode = pSkill->Cast(m_pPlayer, m_pPlayer, m_pPlayer->m_OTActionParam.Target, RecipeKey);
	    if (eRetCode != srcSuccess)
        {
            m_pPlayer->m_AIVM.FireEvent(aevOnCastSkillFailed, m_pPlayer->m_dwID, 0);
            OnBreakUseItem(pItem);
            goto Exit0;
        }

        bCostItem = true;
    }

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pItem->m_dwScriptID);
    if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pItem->m_dwScriptID, USE_ITEM_SUCCEED))
    {   
        int     nTopIndex = 0;
        
        BOOL    bSetCD    = false;

        g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

        g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
        g_pSO3World->m_ScriptCenter.PushValueToStack(pItem);
        g_pSO3World->m_ScriptCenter.PushValueToStack(&(m_pPlayer->m_OTActionParam.Target));

        g_pSO3World->m_ScriptCenter.CallFunction(pItem->m_dwScriptID, USE_ITEM_SUCCEED, 2);

        g_pSO3World->m_ScriptCenter.GetValuesFromStack("bb", &bCostItem, &bSetCD);
        g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

        if (bSetCD)
        {
            bRetCode = pItem->ResetCoolDown(m_pPlayer);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    if (bCostItem && pItem->m_Common.bConsumeDurabiltity)
    {
        KITEM_LOG_INFO  ItemLogInfo;
        g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);
        bRetCode = CostSingleItem(dwBox, dwX, 1);
        KGLOG_PROCESS_ERROR(bRetCode);
        g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, &ItemLogInfo, "use item");
    }
#endif
    
#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler && m_pPlayer->m_OTActionParam.dwEndAniID)
    {
        g_pGameWorldRepresentHandler->OnCharacterPlayAnimation(m_pPlayer, m_pPlayer->m_OTActionParam.dwEndAniID);
    }
#endif

Exit0:
    return;
}

void KItemList::ClearBox()
{
    for (int i = 0; i < (int)m_BoxItem.size(); ++i)
    {
        if (m_BoxItem[i])
        {
            g_pSO3World->m_ItemManager.FreeItem(m_BoxItem[i]);
            m_BoxItem[i] = NULL;
        }
    }
    m_BoxItem.clear();
    m_dwBoxItemID = ERROR_ID;
}

BOOL KItemList::OpenBox(KItem* pBox)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    KOtherItemInfo* pItemInfo       = NULL;
    KBOX_INFO*      pBoxInfo        = NULL;
    int             nDropRate       = 0;
    DWORD           dwOldRandSeed   = 0;
    KItem*          pItemInBox      = NULL;
    KBOX_ITEM*      pBoxItemInfo    = NULL;

    assert(pBox);
    
    ClearBox();

    KG_PROCESS_ERROR(pBox->m_GenParam.dwTabType == ittOther);

    pItemInfo = (KOtherItemInfo*)pBox->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    KG_PROCESS_ERROR(pItemInfo->dwBoxTemplateID);
    
    pBoxInfo = g_pSO3World->m_ItemManager.m_ItemLib.GetBoxInfo(pItemInfo->dwBoxTemplateID);
    KGLOG_PROCESS_ERROR(pBoxInfo);
    
    dwOldRandSeed = g_GetRandomSeed();
    // 乘上玩家ID为了放大差异，防止生成时间接近的箱子开出的物品相同
    g_RandomSeed((unsigned int)pBox->m_GenParam.nGenTime * (m_pPlayer->m_dwID + MILLION_NUM));

    for (int i = 0; i < BOX_ITEM_RATE_TYPE_NUM; ++i)
    {
        if (pBoxInfo->nRateType[i] == ebrtUniform)
        {
            nDropRate = g_Random(MILLION_NUM);
            
            for (int j = i * BOX_ITEM_GROUP_NUM; j < (i + 1) * BOX_ITEM_GROUP_NUM; ++j)
            {
                pBoxItemInfo = &pBoxInfo->BoxItem[j];
                if (nDropRate >= pBoxItemInfo->nRate)
                   continue;
                
                if (pBoxItemInfo->dwTabType == 0 || pBoxItemInfo->dwTabIndex == 0)
                {
                    continue;
                }

                pItemInBox = g_pSO3World->GenerateItem(
                    pBoxItemInfo->dwTabType, pBoxItemInfo->dwTabIndex
                );
                KGLOG_PROCESS_ERROR(pItemInBox);
                
                if (pItemInBox->IsStackable())
                {
                    bRetCode = pItemInBox->SetStackNum(pBoxItemInfo->nStackNum);
                    KGLOG_PROCESS_ERROR(bRetCode);
                }

                if (pItemInBox->m_Common.nGenre == igBook)
                {
                    pItemInBox->m_nCurrentDurability = pBoxItemInfo->nStackNum; // 书的配方
                }
                
                m_BoxItem.push_back(pItemInBox);
                pItemInBox = NULL;

                break;
            }
        }
        else if (pBoxInfo->nRateType[i] == ebrtIndependent)
        {
            for (int j = i * BOX_ITEM_GROUP_NUM; j < (i + 1) * BOX_ITEM_GROUP_NUM; ++j)
            {
                nDropRate = g_Random(MILLION_NUM);

                pBoxItemInfo = &pBoxInfo->BoxItem[j];
                if (nDropRate >= pBoxItemInfo->nRate)
                    continue;
                
                if (pBoxItemInfo->dwTabType == 0 || pBoxItemInfo->dwTabIndex == 0)
                {
                    continue;
                }

                pItemInBox = g_pSO3World->GenerateItem(
                    pBoxItemInfo->dwTabType, pBoxItemInfo->dwTabIndex
                );
                KGLOG_PROCESS_ERROR(pItemInBox);

                if (pItemInBox->IsStackable())
                {
                    bRetCode = pItemInBox->SetStackNum(pBoxItemInfo->nStackNum);
                    KGLOG_PROCESS_ERROR(bRetCode);
                }

                if (pItemInBox->m_Common.nGenre == igBook)
                {
                    pItemInBox->m_nCurrentDurability = pBoxItemInfo->nStackNum; // 书的配方
                }

                m_BoxItem.push_back(pItemInBox);
                pItemInBox = NULL;
            }
        }
    }
    
    m_dwBoxItemID = pBox->m_dwID;

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pItemInBox)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItemInBox);
            pItemInBox = NULL;
        }

        ClearBox();
    }
    
    if (dwOldRandSeed)
    {
        g_RandomSeed(dwOldRandSeed);
        dwOldRandSeed = 0;
    }

    return bResult;
}

ADD_ITEM_RESULT_CODE KItemList::CanLootBoxItem(DWORD dwBoxID)
{
    ADD_ITEM_RESULT_CODE    nResult                 = aircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode                = aircFailed;
    int                     nBoxItemCount           = 0;
    int                     nFreeRoomCount          = 0;
    DWORD                   dwAlreadyHavItemCount   = 0;
    DWORD                   dwItemCountInBox        = 0;
    DWORD                   dwMaxExistAmount        = 0;
    KItem*                  pItem                   = NULL;

    KG_PROCESS_ERROR(m_dwBoxItemID == dwBoxID);
    
    nBoxItemCount = (int)m_BoxItem.size();
    nFreeRoomCount = GetAllPackageFreeRoomSize();
    if (nFreeRoomCount < nBoxItemCount)
    {
        nResult = aircNotEnoughFreeRoom;
#ifdef _SERVER
        g_PlayerServer.DoMessageNotify(
            m_pPlayer->m_nConnIndex, ectAddItemErrorCode, aircNotEnoughFreeRoom
        );
#endif
        goto Exit0;
    }

    for (size_t i = 0; i < m_BoxItem.size(); ++i)
    {
        pItem = m_BoxItem[i];
        assert(pItem);

        dwMaxExistAmount = (DWORD)pItem->m_Common.nMaxExistAmount;
        if (dwMaxExistAmount == 0)
            continue;
        
        dwAlreadyHavItemCount = GetItemTotalAmount(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex);
        dwItemCountInBox = pItem->GetStackNum();

        for (size_t j = i + 1; j < m_BoxItem.size(); j++)
        {
            KItem* pTempItem = m_BoxItem[j];
            assert(pTempItem);

            if (
                pTempItem->m_GenParam.dwTabType != pItem->m_GenParam.dwTabType || 
                pTempItem->m_GenParam.dwIndex != pItem->m_GenParam.dwIndex
            )
                continue;

            if (pItem->m_Common.nGenre == igBook)
            {
                if (pTempItem->m_nCurrentDurability != pItem->m_nCurrentDurability)
                    continue;
            }

            dwItemCountInBox += pItem->GetStackNum();
        }

        if (dwAlreadyHavItemCount + dwItemCountInBox > dwMaxExistAmount)
        {
            nResult = aircItemAmountLimited;

#ifdef _SERVER
            size_t uNameLen = strlen(pItem->m_Common.szItemName) + 1;

            g_PlayerServer.DoMessageNotify(
                m_pPlayer->m_nConnIndex, ectAddItemErrorCode, aircItemAmountLimited,
                pItem->m_Common.szItemName, uNameLen
            );
#endif
            goto Exit0;
        }
    }

    nResult = aircSuccess;
Exit0:
    return nResult;
}

#ifdef _SERVER
BOOL KItemList::LootBoxItem(DWORD dwBoxIndex, DWORD dwPos)
{
    BOOL                    bResult     = false;
    int                     nRetCode    = false;
    KItem*                  pBox        = NULL;
    TItemPos                ItemPos     = {dwBoxIndex, dwPos};
    KITEM_LOG_INFO          ItemLogInfo;

    pBox = GetItem(ItemPos.dwBox, ItemPos.dwX);
    KGLOG_PROCESS_ERROR(pBox);

    nRetCode = CanLootBoxItem(pBox->m_dwID);
    KG_PROCESS_ERROR(nRetCode == aircSuccess);

    for (size_t i = 0; i < m_BoxItem.size(); ++i)
    {
        KGLOG_PROCESS_ERROR(m_BoxItem[i]);
        
        g_LogClient.GetItemLogInfo(m_BoxItem[i], &ItemLogInfo);

        nRetCode = AddItem(m_BoxItem[i]);
        if (nRetCode == aircSuccess)
        {
            m_BoxItem[i] = NULL;
            g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "get from box");
        }
        else
        {
            g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, &ItemLogInfo, "get from box failed");
        }
    }

    ClearBox();
    
    g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_OUT, pBox, "loot box over!");

    nRetCode = DestroyItem(ItemPos);
    assert(nRetCode == ircSuccess);

    bResult = true;
Exit0:
    return bResult;
}
#endif
