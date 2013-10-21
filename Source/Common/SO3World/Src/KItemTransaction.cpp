#include "stdafx.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "KItemTransaction.h"
#ifdef _SERVER
KItemTransaction::KItemTransaction()
{
    m_pPlayer           = NULL;
    m_pBox              = NULL;
}

KItemTransaction::~KItemTransaction()
{
    assert(m_pPlayer == NULL);
    assert(m_pBox    == NULL);
}

void KItemTransaction::Start(KPlayer* pPlayer)
{
    assert(pPlayer);
    assert(m_pPlayer == NULL);
    
    m_pPlayer = pPlayer;
    
    m_pBox = m_pPlayer->m_ItemList.GetAllInventory();

    return;
}

void KItemTransaction::Commit()
{
    BOOL                    bRetCode        = false;
    KOPERATION_INFO*        pOperation      = NULL;
    KItem*                  pItem           = NULL;
    int                     nStackNum       = 0;

    for (int i = 0; i < (int)m_Operation.size(); ++i)
    {
        pOperation = &m_Operation[i];

        switch(pOperation->eType)
        {
        case otAddItem:
            {
                pItem = m_pBox[pOperation->dwBox].GetItem(pOperation->dwPos);
                assert(pItem);
                
                nStackNum = pItem->GetStackNum();

                g_PlayerServer.DoSyncItemData(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pItem, pOperation->dwBox, pOperation->dwPos);
		        g_PlayerServer.DoAddItemNotify(m_pPlayer, pItem->m_dwID, nStackNum);
            }
        	break;
        case otUpdateItem:
            {
                pItem = m_pBox[pOperation->dwBox].GetItem(pOperation->dwPos);
                assert(pItem);
                
                nStackNum = pItem->GetStackNum();

                g_PlayerServer.DoUpdateItemAmount(m_pPlayer->m_nConnIndex, pOperation->dwBox, pOperation->dwPos, nStackNum);
	            g_PlayerServer.DoAddItemNotify(m_pPlayer, pItem->m_dwID, pOperation->nAddAmount);
            }
        	break;
        case otDestroyItem:
            {
                if (pOperation->dwBox == ibEquip)
                {
                    bRetCode = m_pPlayer->m_ItemList.UnEquip(pOperation->dwPos);
                    assert(bRetCode);

                    pItem = m_pPlayer->m_ItemList.PickUpItem(ibEquip, pOperation->dwPos);
                    assert(pItem);

                    m_pPlayer->UpdateWeaponAttackSpeed();
                }
                
                g_PlayerServer.DoSyncItemDestroy(m_pPlayer->m_nConnIndex, m_pPlayer->m_dwID, pOperation->dwBox, pOperation->dwPos);
                
                assert(pOperation->pItem);
                g_pSO3World->m_ItemManager.FreeItem(pOperation->pItem);
            }
            break;
        default:
            assert(false);
            break;
        }
    }     

    Reset();

    return;
}

void KItemTransaction::Rollback()
{
    BOOL                    bRetCode        = false;
    KOPERATION_INFO*        pOperation      = NULL;
    KItem*                  pItem           = NULL;
    int                     nStackNum       = 0;
    
    for (int i = (int)m_Operation.size() - 1; i >= 0 ; --i)
    {
        pOperation = &m_Operation[i];

        switch(pOperation->eType)
        {
        case otAddItem:
            {
                pItem = m_pBox[pOperation->dwBox].PickUpItem(pOperation->dwPos);
                assert(pItem);
                
                g_pSO3World->m_ItemManager.FreeItem(pItem);
            }
        	break;
        case otUpdateItem:
            {
                pItem = m_pBox[pOperation->dwBox].GetItem(pOperation->dwPos);
                assert(pItem);
                
                nStackNum = pItem->GetStackNum();
                
                assert(nStackNum > pOperation->nAddAmount);

                pItem->SetStackNum(nStackNum - pOperation->nAddAmount);
            }
        	break;
        case otDestroyItem:
            break;
        default:
            assert(false);
            break;
        }
    }

    Reset();

    return;
}

ADD_ITEM_RESULT_CODE KItemTransaction::AddItem(KItem* pAddItem)
{
    ADD_ITEM_RESULT_CODE    nResult            = aircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode           = aircFailed;
    BOOL                    bRetCode           = false;
    int                     nStackNum          = 0;
    KInventory*             pInventory         = 0;
    DWORD                   dwPlace            = UINT_MAX;
    int                     nTotalAmount       = 0;
    int                     nMaxExistAmount    = 0;
    int                     nContainItemGenerType = 0;
    int                     nContainItemSubType   = 0;

    assert(pAddItem);
    
    bRetCode = CheckItemMaxExistAmount(pAddItem);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, aircItemAmountLimited);

    if (pAddItem->IsStackable())
    {        
        for (int i = ibPackage; i <= ibPackage4; ++i)
        {
            pInventory = m_pBox + i;

            nStackNum = pAddItem->GetStackNum();
            
            for (DWORD j = 0; j < pInventory->GetSize(); ++j)
            {
                KItem* pItem = pInventory->GetItem(j);
                
                if (pItem && pItem->CanStack(pAddItem))
                {
                    int nOldStackNum = nStackNum;
                    nStackNum = pItem->Stack(nStackNum);

                    OnUpdateItemAmount(i, i, nOldStackNum - nStackNum);

                    if (nStackNum == 0)
                        break;
                }
            }

            bRetCode = pAddItem->SetStackNum(nStackNum);
            KGLOG_PROCESS_ERROR(bRetCode);

            if (nStackNum == 0)
            {
                g_pSO3World->m_ItemManager.FreeItem(pAddItem);
                pAddItem = NULL;
                
                goto Exit1;
            }
        }
    }

    for (int i = ibPackage; i <= ibPackage4; ++i)
    {
        pInventory = m_pBox + i;
        
        bRetCode = pInventory->CheckContainItemType(pAddItem);
        if (!bRetCode)
        {
            continue;
        }
        
        for (DWORD i = 0; i < pInventory->GetSize(); ++i)
        {
            KItem* pItem = pInventory->GetItem(i);
            if (pItem == NULL)
            {
                bRetCode = pInventory->PlaceItem(pAddItem, dwPlace);
                KGLOG_PROCESS_ERROR(bRetCode);

                OnAddItem(i, dwPlace);

                goto Exit1;
            }
        }
    }
    
    KG_PROCESS_ERROR_RET_CODE(false, aircNotEnoughFreeRoom);

Exit1:
    nResult = aircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemTransaction::CostItem(DWORD dwBox, DWORD dwPos, int nCostAmount)
{
    ITEM_RESULT_CODE nResult    = ircFailed;
    ITEM_RESULT_CODE eRetCode   = ircFailed;
    BOOL             bRetCode   = false;
    KInventory*      pInventory = NULL;
    KItem*           pItem      = NULL;
    int              nStackNum  = 0;

    KGLOG_PROCESS_ERROR(dwBox >= 0 && dwBox < ibTotal);
    
    pInventory = &m_pBox[dwBox];
    assert(pInventory);

    pItem = pInventory->GetItem(dwPos);
    KG_PROCESS_ERROR_RET_CODE(pItem, ircItemNotExist);
    
    nStackNum = pItem->GetStackNum();
    KG_PROCESS_ERROR(nStackNum >= nCostAmount);

    if (nStackNum == nCostAmount)
    {
        if (dwBox == ibEquip)
        {
            KG_PROCESS_ERROR_RET_CODE(pItem->m_Common.nSub != estPackage, ircEquipedPackageCannotDestroy);

            eRetCode = m_pPlayer->m_ItemList.CanUnEquip(dwPos);
            KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
        }
        
        OnDestroyItem(dwBox, dwPos, pItem);

        goto Exit1;
    }
    
    bRetCode = pItem->SetStackNum(nStackNum - nCostAmount);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    OnUpdateItemAmount(dwBox, dwPos, -nCostAmount);

Exit1:
    nResult = ircSuccess;
Exit0:
    return nResult;
}

void KItemTransaction::OnAddItem(DWORD dwBox, DWORD dwPos)
{
    KOPERATION_INFO Operation;
    
    ZeroMemory(&Operation, sizeof(Operation));

    Operation.dwBox   = dwBox;
    Operation.dwPos   = dwPos;

    m_Operation.push_back(Operation);

    return;
}

void KItemTransaction::OnUpdateItemAmount(DWORD dwBox, DWORD dwPos, int nAddAmount)
{
    KOPERATION_INFO Operation;
    
    ZeroMemory(&Operation, sizeof(Operation));

    Operation.dwBox         = dwBox;
    Operation.dwPos         = dwPos;
    Operation.nAddAmount    = nAddAmount;

    m_Operation.push_back(Operation);

    return;
}

void KItemTransaction::OnDestroyItem(DWORD dwBox, DWORD dwPos, KItem* pItem)
{
    KOPERATION_INFO Operation;
    
    ZeroMemory(&Operation, sizeof(Operation));

    Operation.dwBox         = dwBox;
    Operation.dwPos         = dwPos;
    Operation.pItem         = pItem;

    m_Operation.push_back(Operation);

    return;
}

BOOL KItemTransaction::CheckItemMaxExistAmount(KItem* pAddItem)
{
    BOOL         bResult            = false;
    int          nStackNum          = 0;
    int          nMaxExistAmount    = 0;
    KInventory*  pInventory         = NULL;  
    int          nTotalAmount       = 0;

    assert(pAddItem);

    nStackNum = pAddItem->GetStackNum();
    nMaxExistAmount = pAddItem->GetMaxExistAmount();
    
    KG_PROCESS_ERROR(nMaxExistAmount > 0);

    for (int i = 0; i < ibTotal; ++i)
    {
        pInventory = m_pBox + i;

        if (i == ibSoldList)
        {
            continue;
        }

        for (DWORD dwX = 0; dwX < pInventory->GetSize(); ++dwX)
        {
            KItem* pExistItem = pInventory->GetItem(dwX);

            if (pExistItem == NULL)
            {
                continue;
            }
            
            if (pExistItem->m_GenParam.dwTabType != pAddItem->m_GenParam.dwTabType)
            {
                continue;
            }

            if (pExistItem->m_GenParam.dwIndex != pAddItem->m_GenParam.dwIndex)
            {
                continue;
            }

            nTotalAmount += pExistItem->GetStackNum();
        }
    }
    
    KG_PROCESS_ERROR(nTotalAmount + nStackNum <= nMaxExistAmount);

    bResult = true;
Exit0:
    return bResult;
}

void KItemTransaction::Reset()
{
    m_Operation.clear();

    m_pBox      = NULL;
    m_pPlayer   = NULL;
}
#endif
