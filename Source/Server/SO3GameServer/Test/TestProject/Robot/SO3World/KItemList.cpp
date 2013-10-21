#include "stdafx.h"
#include "KObjectManager.h"
#include "KItemLib.h"
#include "KItemList.h"
#include "KRoleDBDataDef.h"
//#include "KSkillManager.h"
#include "KScriptCenter.h"
#include "KSO3World.h"
#include "KItem.h"


#define DEFAULT_PASSWORD	0

//装备栏规则表，1目标格子类型，2装备类型
BOOL g_CanEquipList[eitTotal][estTotal] =
{
//	 MW RW C  H  A  R  W  PE PA BO BA EX PG AR
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//eitMeleeWeapon,		//近战武器
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//eitRangeWeapon,		//远程武器
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//eitChest,				//上衣
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//eitHelm,				//头部
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//eitAmulet,			//项链
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},	//eitLeftRing,			//左手戒指
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},	//eitRightRing,			//右手戒指
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},	//eitWaist,				//腰带
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},	//eitPendant,			//腰缀
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},	//eitPants,				//裤子
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},	//eitBoots,				//鞋子
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},	//eitBangle,			//护臂
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},	//eitExtend,			//扩展类装备，酒壶之类的
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},	//eitPackage1,			//扩展背包1
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},	//eitPackage2,			//扩展背包2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},	//eitPackage3,			//扩展背包3
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},	//eitPackage4,			//扩展背包4
	{0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 1},	//eitArrow,				//暗器
};

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
	default:
		break;
	}

	return nResult;
}

KItemList::KItemList(void)
{
    m_dwBoxCount = 0;									//箱子的总个数

    m_State = ilsInvalid;								//箱子的状态
    m_dwPassword = DEFAULT_PASSWORD;					//密码
    m_pPlayer = NULL;									
}

KItemList::~KItemList(void)
{
}

//初始化
BOOL KItemList::Init(KPlayer* pPlayer)
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR(pPlayer);

    m_nMoney        = 0;
    m_nMoneyLimit   = MONEY_LIMIT ;

    m_dwPassword = DEFAULT_PASSWORD;
    m_State = ilsLocked;

    m_pPlayer = pPlayer;

    bRetCode = m_Box[ibEquip].Init(eitTotal, ivtEquipment, TRUE);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Box[ibPackage].Init(FIRST_PACKAGE_SIZE, ivtPackage, TRUE);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Box[ibPackage1].Init(0, ivtPackage, TRUE);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Box[ibPackage2].Init(0, ivtPackage, TRUE);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Box[ibPackage3].Init(0, ivtPackage, TRUE);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_Box[ibPackage4].Init(0, ivtPackage, TRUE);
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Box[ibBank].Init(MAX_ITEM_PACKAGE_SIZE, ivtBank, FALSE);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Box[ibSoldList].Init(MAX_SOLDLIST_PACKAGE_SIZE, ivtSoldlist, FALSE);
    //	bRetCode = m_Box[ibLootList].Init(MAX_LOOT_SIZE, ivtPackage, TRUE);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::UnInit()
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;
    KItem* pItem = NULL;

    for (int i = 0; i < ibTotal; i++)
    {
        for (DWORD j = 0; j < m_Box[i].m_dwSize; j++)
        {
            pItem = m_Box[i].GetItem(j);
            if (pItem)
            {
                bRetCode = g_ItemLib.FreeItem(pItem);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }
    }

    bResult = TRUE;

Exit0:

    return bResult;
}

//添加一个物品
BOOL KItemList::AddItem(KItem* pItem)
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;
    BOOL bFinishStack = FALSE;
    BOOL bFindPlace = FALSE;
    int	 nPlace = 0;

    ASSERT(m_pPlayer);
    KGLOG_PROCESS_ERROR(pItem);

    if (pItem->IsStackable())
    {
        for (int i = 0; i < ibTotal; i++)
        {
            if (m_Box[i].IsOpened() && m_Box[i].m_nBoxType != ivtEquipment &&
				CanPut(pItem, 0xffff, i, 0, FALSE))
            {
                bFinishStack = PushItem(pItem, i);
                if (bFinishStack)
                    break;
            }
        }
    }

    int nCount = pItem->GetStackNum(); //要添加的道具数
    int nMaxExistAmount = pItem->GetMaxExistAmount(); //能携带的道具上限
    int nItemAmount = GetItemAmount(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex); //身上的道具数量
    int nCanPushAmount = nMaxExistAmount - nItemAmount;	//还能增加的道具数量

    if (nCount > 0)
    {
        for (int i = 0; i < ibTotal; i++)
        {
            if (m_Box[i].IsOpened() && m_Box[i].m_nBoxType != ivtEquipment)
            {
                nPlace = m_Box[i].FindFreePlace();
                if (nPlace >= 0)
                {
                    if (nMaxExistAmount && (nCanPushAmount <= 0))
                    {
                        //由于数量限制，完全不能放下的情况
                        goto Exit0;
                    }
                    else if (nMaxExistAmount && (nCanPushAmount < nCount))
                    {
                        ASSERT(pItem->IsStackable());

                        //由于数量限制，不能完全放下的情况
                        KItem* pNewItem = g_ItemLib.CloneItem(pItem);
                        KGLOG_PROCESS_ERROR(pNewItem);

                        ASSERT(nCanPushAmount <= pNewItem->GetMaxStackNum());

                        pNewItem->SetStackNum(nCanPushAmount);

                        bResult = m_Box[i].PlaceItem(pNewItem, nPlace);
                        _ASSERT(bResult);
                        bFindPlace = TRUE;
                        bRetCode = pItem->SetStackNum(nCount - nCanPushAmount);
                        KGLOG_PROCESS_ERROR(bRetCode);

                        goto Exit0;
                    }
                    else
                    {
                        bResult = m_Box[i].PlaceItem(pItem, nPlace);
                        _ASSERT(bResult);
                        bFindPlace = TRUE;
                        break;
                    }
                }
            }
        }
        KG_PROCESS_ERROR(bFindPlace);
    }
    else
    {
        //Remove the item because stack num = 0;
        bRetCode = g_ItemLib.FreeItem(pItem);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bResult = TRUE;

Exit0:
    return bResult;
}

BOOL KItemList::AddItem(KItem* pItem, DWORD dwBoxIndex, DWORD dwX)
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

    bRetCode = m_Box[dwBoxIndex].PlaceItem(pItem, dwX);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (dwBoxIndex == ibEquip)
    {
        bRetCode = Equip(pItem, dwX);
        KGLOG_PROCESS_ERROR(bRetCode);

		DWORD dwSlotBox = g_GetBoxIndexFromSlot(dwX);
		if (dwSlotBox)
		{
			EnablePackage(pItem->m_nPackageSize, pItem->m_Common.nDetail, dwSlotBox);
		}

    }

    bResult = TRUE;

Exit0:

    return bResult;
}

//从身上删除一个物品，不从游戏世界删除掉
KItem* KItemList::DelItem(DWORD dwBoxIndex, DWORD dwX)
{
    BOOL bRetCode = FALSE;
    KItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);
    KGLOG_PROCESS_ERROR(dwX < m_Box[dwBoxIndex].m_dwSize);

    pItem = m_Box[dwBoxIndex].PickUpItem(dwX);

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

//解锁
BOOL KItemList::UnLock(DWORD dwPassword)
{
    if (dwPassword == m_dwPassword)
    {
        m_State = ilsOpened;

        return TRUE;
    }

    return FALSE;
}

//加锁
BOOL KItemList::Lock(void)
{
    m_State = ilsLocked;

    return TRUE;
}

DWORD KItemList::GetItemAmount(DWORD dwTabType, DWORD dwIndex)
{
    DWORD dwResult = 0;

    for (int i = 0; i < ibTotal; i++)
    {
        if (m_Box[i].IsOpened()) // && m_Box[i].m_nBoxType != ivtEquipment)
        {
            for (DWORD dwX = 0; dwX < m_Box[i].m_dwSize; dwX++)
            {
                KItem* pItem = m_Box[i].GetItem(dwX);
                if (pItem && pItem->m_GenParam.dwTabType == dwTabType && 
                    pItem->m_GenParam.dwIndex == dwIndex)
                {
                    dwResult += pItem->GetStackNum();
                }
            }
        }
    }

    return dwResult;
}

BOOL KItemList::GetItemPos(int nVersion, DWORD dwTabType, DWORD dwIndex, DWORD &dwBox, DWORD &dwX)
{
    for (int i = 0; i < ibTotal; i++)
    {
        if (m_Box[i].IsOpened())
        {
            for (int j = 0; j < (int)m_Box[i].m_dwSize; ++j)
            {
                KItem* pItem = m_Box[i].GetItem(j);
                if (pItem && pItem->m_GenParam.nVersion == nVersion &&
                    pItem->m_GenParam.dwTabType == dwTabType && 
                    pItem->m_GenParam.dwIndex == dwIndex)
                {
                    dwBox = i;
                    dwX = j;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL KItemList::GetItemPos(DWORD dwID, DWORD &dwBox, DWORD &dwX)
{
    for (int i = 0; i < ibTotal; i++)
    {
        if (m_Box[i].IsOpened())
        {
            for (int j = 0; j < (int)m_Box[i].m_dwSize; ++j)
            {
                KItem* pItem = m_Box[i].GetItem(j);
                if (pItem && pItem->m_dwID == dwID)
                {
                    dwBox = i;
                    dwX = j;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

int	KItemList::GetBoxType(DWORD dwBox)
{
    if (dwBox < ibTotal)
        return m_Box[dwBox].m_nBoxType;
    return ivtTotal;
}

BOOL KItemList::CostItem(DWORD dwTabType, DWORD dwIndex, DWORD dwAmount)
{
    BOOL bRetCode = FALSE;

    KG_PROCESS_ERROR(GetItemAmount(dwTabType, dwIndex) >= dwAmount);

    for (int i = 0; i < ibTotal; i++)
    {
        if (m_Box[i].IsOpened() && m_Box[i].m_nBoxType != ivtEquipment)
        {
            for (DWORD dwX = 0; dwX < m_Box[i].m_dwSize; dwX++)
            {
                KItem* pItem = m_Box[i].GetItem(dwX);
                if (pItem && pItem->m_GenParam.dwTabType == dwTabType && 
                    pItem->m_GenParam.dwIndex == dwIndex)
                {
					if (pItem->IsStackable())
					{
						if ((DWORD)pItem->m_nStackNum > dwAmount)
						{
							pItem->m_nStackNum -= dwAmount;
							return TRUE;
						}
						else if ((DWORD)pItem->m_nStackNum == dwAmount)
						{
							TItemPos ItemPos = {i, dwX};
							bRetCode = (DestoryItem(ItemPos) == ircSuccess);
							KGLOG_PROCESS_ERROR(bRetCode);

							return TRUE;
						}
						else
						{
							TItemPos ItemPos = {i, dwX};

							dwAmount -= pItem->m_nStackNum;

							bRetCode = (DestoryItem(ItemPos) == ircSuccess);
							KGLOG_PROCESS_ERROR(bRetCode);
						}
					}
					else
					{
						TItemPos ItemPos = {i, dwX};
						bRetCode = (DestoryItem(ItemPos) == ircSuccess);
						KGLOG_PROCESS_ERROR(bRetCode);
						dwAmount--;
						if (!dwAmount)
							return TRUE;
					}
                }
            }
        }
    }

Exit0:

    return FALSE;
}

BOOL KItemList::CostSingleItem(DWORD dwBoxIndex, DWORD dwX, DWORD dwAmount)
{
    BOOL bRetCode = FALSE;
    KItem* pItem = GetItem(dwBoxIndex, dwX);

    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR((DWORD)pItem->m_nCurrentDurability >= dwAmount);

    if ((DWORD)pItem->m_nCurrentDurability == dwAmount)
    {
        if (pItem->GetGenre() == igEquipment && !pItem->IsStackable())
        {
            if (dwBoxIndex == ivtEquipment)
            {
                //bRetCode = UnEquip(pItem, dwX);
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
            bRetCode = (DestoryItem(ItemPos) == ircSuccess);
            KGLOG_PROCESS_ERROR(bRetCode);
			KG_PROCESS_SUCCESS(bRetCode);
        }
    }
    else
    {
        pItem->m_nCurrentDurability -= dwAmount;
    }

Exit1:
    return TRUE;

Exit0:

    return FALSE;
}

//查找指定大小的空闲空间
BOOL KItemList::FindFreeRoom(DWORD dwBoxIndex, DWORD& dwX)
{
    KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);

    for (dwX = 0; dwX < m_Box[dwBoxIndex].m_dwSize; dwX++)
    {
        KItem* pItem = m_Box[dwBoxIndex].GetItem(dwX);
        if (pItem == NULL)
            return TRUE;
    }

Exit0:

    return FALSE;
}

BOOL KItemList::FindFreeSoldList(DWORD& dwX)
{
    int         nResult         = -1;
    BOOL        bRetCode        = FALSE;
    BOOL        bResult         = FALSE;
    int         nDelPos         = -1;

    nResult = m_Box[ibSoldList].FindFreeSoldList(nDelPos);
    KGLOG_PROCESS_ERROR(nResult >= 0);

    dwX = nResult;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::FindFreeRoomInPackage(DWORD& dwBoxIndex, DWORD& dwX)
{
    BOOL bRetCode = FALSE;

    for (int nIndex = 0; nIndex < ibTotal; nIndex++)
    {
        dwBoxIndex = nIndex;
        if (m_Box[nIndex].IsOpened() && m_Box[nIndex].m_nBoxType == ivtPackage)
        {
            bRetCode = FindFreeRoom(nIndex, dwX);
            KG_PROCESS_SUCCESS(bRetCode);
        }
    }

    return FALSE;
Exit1:
    return TRUE;
}

DWORD KItemList::GetFreeRoomSize(void)
{
    DWORD dwSize = 0;

    for (int nBoxIndex = 0; nBoxIndex < ibTotal; nBoxIndex++)
    {
        if (m_Box[nBoxIndex].IsOpened() && m_Box[nBoxIndex].m_nBoxType == ivtPackage)
        {
            for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; dwX++)
            {
                KItem* pItem = m_Box[nBoxIndex].GetItem(dwX);
                if (pItem == NULL)
                    dwSize++;
            }
        }
    }

Exit0:

    return dwSize;
}

BOOL KItemList::SetMoney(int nMoney)
{
    BOOL bResult = FALSE;
    KG_PROCESS_ERROR(nMoney >= 0);
    KG_PROCESS_ERROR(nMoney <= m_nMoneyLimit);

    m_nMoney = nMoney;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::AddMoney(int nMoney)
{
    BOOL bResult = FALSE;
    KG_PROCESS_ERROR(m_nMoney + nMoney >= 0);
    KG_PROCESS_ERROR(m_nMoney + nMoney <= m_nMoneyLimit);

    m_nMoney += nMoney;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::SetMoneyLimit(int nLimit)
{
    BOOL bResult = FALSE;

    KG_PROCESS_ERROR(nLimit > 0);

    m_nMoneyLimit = nLimit;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::AddMoneyLimit(int nLimit)
{
    BOOL bResult = FALSE;
    KG_PROCESS_ERROR(nLimit + m_nMoneyLimit > 0);

    m_nMoneyLimit += nLimit;

    bResult = TRUE;
Exit0:
    return bResult;
}

ITEM_RESULT_CODE KItemList::ExchangeItem(const TItemPos& PosSrc, const TItemPos& PosDest, DWORD dwAmount)
{
    ITEM_RESULT_CODE	nResult = ircFailed;
    BOOL				bRetCode = FALSE;
    int					nRetCode = 0;
    KItem*				pSrcItem = NULL;
    KItem*				pDestItem = NULL;
    KInventory*			pSrcBox = NULL;
    KInventory*			pDestBox = NULL;

	TItemPos Src = PosSrc;
	TItemPos Dest = PosDest;

    KG_ASSERT_EXIT(m_pPlayer);

    // 如果源和目标一致，说明不用换，返回失败。
    KGLOG_PROCESS_ERROR(Src.dwBox != Dest.dwBox || Src.dwX != Dest.dwX);

    KGLOG_PROCESS_ERROR(Src.dwBox < ibTotal && Src.dwX < m_Box[Src.dwBox].m_dwSize);
    KGLOG_PROCESS_ERROR(Dest.dwBox < ibTotal && Dest.dwX < m_Box[Dest.dwBox].m_dwSize);

    pSrcItem = GetItem(Src.dwBox, Src.dwX);
    // 交换道具时必须保证源道具不为空
    KGLOG_PROCESS_ERROR(pSrcItem);
    KGLOG_PROCESS_ERROR_RET_CODE(dwAmount <= (DWORD)pSrcItem->GetStackNum(), ircNotEnoughDurability);

    // 目标道具有可能为空
    pDestItem = GetItem(Dest.dwBox, Dest.dwX);

    pSrcBox = &m_Box[Src.dwBox];
    pDestBox = &m_Box[Dest.dwBox];

    // 必须是打开的包才能操作
    KGLOG_PROCESS_ERROR(pSrcBox->IsOpened());
    KGLOG_PROCESS_ERROR(pDestBox->IsOpened());

	KG_PROCESS_ERROR_RET_CODE(CanPut(pDestItem, Dest.dwBox, Src.dwBox, Src.dwX, TRUE), ircCannotPutToThatPlace);
	KG_PROCESS_ERROR_RET_CODE(CanPut(pSrcItem, Src.dwBox, Dest.dwBox, Dest.dwX, FALSE), ircCannotPutToThatPlace);
	
	//TODO:目标放到背包的装备格子中，要切换目标
	if (pDestItem && pSrcItem && pSrcItem->m_Common.nSub != estPackage && 
		IsPackageSlot(Dest.dwBox, Dest.dwX))
	{
		int nBoxIndex = g_GetBoxIndexFromSlot(Dest.dwX);
		KGLOG_PROCESS_ERROR(nBoxIndex);
		int nX = m_Box[nBoxIndex].FindFreePlace();
		Dest.dwBox = nBoxIndex;
		Dest.dwX = nX;
		pDestItem = NULL;
		pDestBox = &m_Box[nBoxIndex];
	}

    if (dwAmount == 0)
    {
        dwAmount = pSrcItem->GetStackNum();
    }

    // 没有向装备格上移动
    if ((pSrcBox->m_nBoxType != ivtEquipment) && (pDestBox->m_nBoxType != ivtEquipment))
    {
        ASSERT(pSrcItem);
        if (pDestItem)		// 目标有东西
        {
            if (pDestItem->CanStack(pSrcItem))				// 目标可以叠加
            {
                nRetCode = pDestItem->Stack(dwAmount);
                // 放不下的剩余部分堆回去
                nRetCode = pSrcItem->SetStackNum(pSrcItem->GetStackNum() - dwAmount + nRetCode);

                if (pSrcItem->GetStackNum() == 0)
                {
                    nRetCode = DestoryItem(pSrcItem, FALSE);
                    KGLOG_CHECK_ERROR(nRetCode == ircSuccess);	// 如果策划填了一个可叠加不可摧毁的东西，就会出这个LOG
                    pSrcItem = NULL;
                }
            }
            else											// 不可叠加，互换
            {
                KG_PROCESS_ERROR_RET_CODE(dwAmount == (DWORD)pSrcItem->GetStackNum(), ircCannotPutToThatPlace);

                KItem* pTemp;
                pTemp = pSrcBox->PickUpItem(Src.dwX);
                ASSERT(pTemp == pSrcItem);
                pTemp = pDestBox->PickUpItem(Dest.dwX);
                ASSERT(pTemp == pDestItem);
                bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
                ASSERT(bRetCode);
                bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
                ASSERT(bRetCode);
            }
        }
        else	// 目标没东西
        {
            KItem* pTemp = NULL;
            if (dwAmount == (DWORD)pSrcItem->GetStackNum())	// 全移动
            {
                pTemp= pSrcBox->PickUpItem(Src.dwX);
                ASSERT(pTemp == pSrcItem);
                bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
                ASSERT(bRetCode);
            }
            else	// 拆分出新的物品
            {
            }
        }
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
				ASSERT(pSrcItem);

				if (!pDestItem)
				{
					DWORD dwBox = g_GetBoxIndexFromSlot(Src.dwX);
					nResult = DisablePackage(dwBox);
				}
				else
				{
					ASSERT(pDestItem->m_Common.nGenre == igEquipment &&
						pDestItem->m_Common.nSub == estPackage);
					DWORD dwBox = g_GetBoxIndexFromSlot(Src.dwX);
					nResult = EnablePackage(pDestItem->m_nPackageSize, 
						pDestItem->m_Common.nDetail, dwBox);
				}

			}
			else if (bDestPackage)
			{
				ASSERT(pSrcItem);
				ASSERT(pSrcItem->m_Common.nGenre == igEquipment &&
					pSrcItem->m_Common.nSub == estPackage);

				DWORD dwBox = g_GetBoxIndexFromSlot(Dest.dwX);
				nResult = EnablePackage(pSrcItem->m_nPackageSize, 
						pSrcItem->m_Common.nDetail, dwBox);
			}
			else
			{
				ASSERT(FALSE);
			}
			
			if (nResult == ircSuccess)
			{
				KItem* pTemp = NULL;

				if (Src.dwBox == ibEquip && Dest.dwBox != ibEquip)
				{
					UnEquip(pSrcItem, Src.dwX);

					pTemp = pSrcBox->PickUpItem(Src.dwX);
					ASSERT(pTemp == pSrcItem);

					if (pDestItem)
					{
						bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
						ASSERT(bRetCode);
						Equip(pDestItem, Src.dwX);
					}

					bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
					ASSERT(bRetCode);
				}
				else if (Dest.dwBox == ibEquip && Src.dwBox != ibEquip)
				{
					if (pDestItem)
					{
						UnEquip(pDestItem, Dest.dwX);
						pTemp = pDestBox->PickUpItem(Dest.dwX);
						ASSERT(pTemp == pDestItem);
					}

					pTemp = pSrcBox->PickUpItem(Src.dwX);
					ASSERT(pTemp == pSrcItem);

					if (pDestItem)
					{
						bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
						ASSERT(bRetCode);
					}

					bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
					ASSERT(bRetCode);

					Equip(pSrcItem, Dest.dwX);
				}
				else
				{
					pTemp = pSrcBox->PickUpItem(Src.dwX);
					ASSERT(pTemp == pSrcItem);
					pTemp = pDestBox->PickUpItem(Dest.dwX);
					ASSERT(pTemp == pDestItem);
					if (pDestItem)
					{
						bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
						ASSERT(bRetCode);
					}
					bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
					ASSERT(bRetCode);
				}
			}
		}
		else if (pSrcBox->m_nBoxType == ivtEquipment && pDestBox->m_nBoxType == ivtEquipment)
		{
			// 两个都是装备格，并且可交换放置，那就直接交换好了。不用应用属性
			KItem* pTemp;
			pTemp = pSrcBox->PickUpItem(Src.dwX);
			ASSERT(pTemp == pSrcItem);
			pTemp = pDestBox->PickUpItem(Dest.dwX);
			ASSERT(pTemp == pDestItem);
			bRetCode = pSrcBox->PlaceItem(pDestItem, Src.dwX);
			ASSERT(bRetCode);
			bRetCode = pDestBox->PlaceItem(pSrcItem, Dest.dwX);
			ASSERT(bRetCode);

		}
        else if (pSrcBox->m_nBoxType == ivtEquipment)	// 源装备拆下，目标装备有的话是装上。
        {
            if (pDestItem)
            {
                KItem* pTemp = NULL;

//				是否能装备上面CanPut已经检查过了。
//              nRetCode = CanEquip(pDestItem, Src.dwX);
//              KGLOG_PROCESS_ERROR(nRetCode == ircSuccess);

                bRetCode = UnEquip(pSrcItem, Src.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                ASSERT(pTemp == pSrcItem);
                pTemp = m_Box[Dest.dwBox].PickUpItem(Dest.dwX);
                ASSERT(pTemp == pDestItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                ASSERT(bRetCode);
                bRetCode = m_Box[Src.dwBox].PlaceItem(pDestItem, Src.dwX);
                ASSERT(bRetCode);
                bRetCode = Equip(pDestItem, Src.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KItem* pTemp = NULL;

                bRetCode = UnEquip(pSrcItem, Src.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                ASSERT(pTemp == pSrcItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                ASSERT(bRetCode);
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

                bRetCode = UnEquip(pDestItem, Dest.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                ASSERT(pTemp == pSrcItem);
                pTemp = m_Box[Dest.dwBox].PickUpItem(Dest.dwX);
                ASSERT(pTemp == pDestItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                ASSERT(bRetCode);
                bRetCode = m_Box[Src.dwBox].PlaceItem(pDestItem, Src.dwX);
                ASSERT(bRetCode);
                bRetCode = Equip(pSrcItem, Dest.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KItem* pTemp = NULL;

                pTemp = m_Box[Src.dwBox].PickUpItem(Src.dwX);
                ASSERT(pTemp == pSrcItem);
                bRetCode = m_Box[Dest.dwBox].PlaceItem(pSrcItem, Dest.dwX);
                ASSERT(bRetCode);
                bRetCode = Equip(pSrcItem, Dest.dwX);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }
    }

Exit1:
    nResult = ircSuccess;

Exit0:
    return nResult;
}

static void UpdateEquipRepresentId(KPlayer* pPlayer, KItem* pItem, BOOL bEquip)
{
    static int const s_aRepresentIDIndex[estTotal] =
    {
        REPRESENT_EQUIP_MELEE_WEAPON,	// estMeleeWeapon
        -1,								// estRangeWeapon
        REPRESENT_EQUIP_CHEST,			// estChest
        REPRESENT_EQUIP_HELM,			// estHelm
        -1,								// estAmulet
        -1,								// estRing
        REPRESENT_EQUIP_WAIST,			// estWaist
        -1,								// estPendant
        REPRESENT_EQUIP_PANTS,			// estPants
        -1,								// estBoots
        REPRESENT_EQUIP_BANGLE,			// estBangle
        REPRESENT_EQUIP_EXTEND,			// estExtend
        -1,								// estPackage
		-1,								// estArrow
    };

    ASSERT(pPlayer);
    ASSERT(pItem);
    ASSERT(pItem->m_Common.nGenre == igEquipment);

    int nIndex = s_aRepresentIDIndex[pItem->m_Common.nSub];
    if (nIndex != -1)
    {
        pPlayer->m_wRepresentId[nIndex] = bEquip ? static_cast<WORD>(pItem->m_nRepresentId) : 0;
    }
}
BOOL KItemList::Equip(KItem* pItem, DWORD dwX)
{
    BOOL    bResult         =   FALSE;
    BOOL    bRetCode        =   FALSE;
    DWORD   dwOldRepresent  =   0;

    KGLOG_PROCESS_ERROR(m_pPlayer);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(dwX < m_Box[ibEquip].m_dwSize);
    KGLOG_PROCESS_ERROR(!(pItem->m_bApplyed));
	KGLOG_PROCESS_ERROR(!(pItem->m_bEquiped));
    ASSERT(GetItem(ibEquip, dwX) == pItem);

    //作用魔法属性
	if (pItem->m_nCurrentDurability > 0)
	{
		ApplyAttrib(pItem);
	}

	pItem->m_bEquiped = TRUE;

    if (!pItem->m_bBound && pItem->m_Common.nBindType == ibtBindOnEquipped)
    {
        pItem->m_bBound = TRUE;
    }

    switch (dwX)
    {
    case eitMeleeWeapon:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_MELEE_WEAPON];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_MELEE_WEAPON] = (WORD)pItem->m_nRepresentId;
        break;
    case eitHelm:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_HELM];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_HELM] = (WORD)pItem->m_nRepresentId;
        break;
    case eitChest:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_CHEST];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_CHEST] = (WORD)pItem->m_nRepresentId;
        break;
    case eitWaist:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_WAIST];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_WAIST] = (WORD)pItem->m_nRepresentId;
        break;
    case eitPants:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_PANTS];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_PANTS] = (WORD)pItem->m_nRepresentId;
        break;
    case eitBangle:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_BANGLE];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_BANGLE] = (WORD)pItem->m_nRepresentId;
        break;
    case eitExtend:
        dwOldRepresent = m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_EXTEND];
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_EXTEND] = (WORD)pItem->m_nRepresentId;
        break;
	default:
		dwOldRepresent = pItem->m_nRepresentId;
		break;
    }


    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::UnEquip(KItem* pItem, DWORD dwX)
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;
	BOOL bModifyRepresent = TRUE;

    KGLOG_PROCESS_ERROR(m_pPlayer);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(dwX < m_Box[ibEquip].m_dwSize);
    KGLOG_PROCESS_ERROR(pItem->m_bEquiped);
    ASSERT(GetItem(ibEquip, dwX) == pItem);

    //TODO:战斗状态等检查

    //反作用魔法属性
	if (pItem->m_bApplyed)
	{
		UnApplyAttrib(pItem);
	}

	pItem->m_bEquiped = FALSE;

    switch (dwX)
    {
    case eitMeleeWeapon:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_MELEE_WEAPON] = 0;
        break;
    case eitHelm:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_HELM] = 0;
        break;
    case eitChest:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_CHEST] = 0;
        break;
    case eitWaist:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_WAIST] = 0;
        break;
    case eitPants:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_PANTS] = 0;
        break;
    case eitBangle:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_BANGLE] = 0;
        break;
    case eitExtend:
        m_pPlayer->m_wRepresentId[REPRESENT_EQUIP_EXTEND] = 0;
        break;
	default:
		bModifyRepresent = FALSE;
		break;
    }


    bResult = TRUE;
Exit0:
    return bResult;
}

//检测全身装备是否还符合需求
BOOL KItemList::CheckEquipRequire(void)
{
    BOOL bRetCode =  FALSE;
    BOOL bResult = FALSE;

    for (DWORD dwX = eitMeleeWeapon; dwX < eitTotal; dwX++)
    {
        KItem* pItem = m_Box[ibEquip].GetItem(dwX);
        if (pItem)
        {
            bRetCode = CanEquip(pItem, dwX);
            if (!bRetCode)
            {
                bResult = TRUE;
                bRetCode = UnEquip(pItem, dwX);
                KGLOG_CHECK_ERROR(bRetCode);
            }
        }
    }

    //Exit0:

    return bResult;
}

ITEM_RESULT_CODE KItemList::CanEquip(KEquipInfo* pItem, DWORD dwX)
{
    ITEM_RESULT_CODE nResult = ircInvalid;
    BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR_RET_CODE(pItem, ircFailed);
    KGLOG_PROCESS_ERROR_RET_CODE(pItem->nGenre == igEquipment, ircCannotEquip);
    KGLOG_PROCESS_ERROR_RET_CODE(dwX < m_Box[ibEquip].m_dwSize, ircFailed);

    //TODO:检查格子是否正确
    KG_PROCESS_ERROR_RET_CODE(g_CanEquipList[dwX][pItem->nSub], ircErrorEquipPlace);

    //TODO:战斗状态等检查

    // 检查玩家条件是否符合装备要求
    for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
    {
        KAttribKey* pData = &pItem->RequireAttrib[i];
        switch(pData->nAttribID)
        {
        case rqtFaction:
            // 门派是否相同
            //KG_PROCESS_ERROR();
            break;
        case rqtStrength:
            // 力量是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentStrength >= pData->nValue,
                ircTooLowStrength);
            break;
        case rqtAgility:
            // 敏捷是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentAgility >= pData->nValue,
                ircTooLowAgility);
            break;
        case rqtSpirit:
            // 根骨是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentSpirit >= pData->nValue,
                ircTooLowSpirit);
            break;
        case rqtVitality:
            // 体质是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentVitality >= pData->nValue,
                ircTooLowVitality);
            break;
        case rqtLevel:
            // 等级是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= pData->nValue,
                ircTooLowLevel);
            break;
        default:
            break;
        }
    }

    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::CanEquip(KCustomEquipInfo* pItem, DWORD dwX)
{
    ITEM_RESULT_CODE nResult = ircInvalid;
    BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR_RET_CODE(pItem, ircFailed);
    KGLOG_PROCESS_ERROR_RET_CODE(pItem->nGenre == igEquipment, ircCannotEquip);
    KGLOG_PROCESS_ERROR_RET_CODE(dwX < m_Box[ibEquip].m_dwSize, ircFailed);

    //TODO:检查格子是否正确
    KG_PROCESS_ERROR_RET_CODE(g_CanEquipList[dwX][pItem->nSub], ircErrorEquipPlace);

    //TODO:战斗状态等检查

    // 检查玩家条件是否符合装备要求
    for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
    {
        KAttribKey* pData = &pItem->RequireAttrib[i];
        switch(pData->nAttribID)
        {
        case rqtFaction:
            // 门派是否相同
            //KG_PROCESS_ERROR();
            break;
        case rqtStrength:
            // 力量是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentStrength >= pData->nValue,
                ircTooLowStrength);
            break;
        case rqtAgility:
            // 敏捷是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentAgility >= pData->nValue,
                ircTooLowAgility);
            break;
        case rqtSpirit:
            // 根骨是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentSpirit >= pData->nValue,
                ircTooLowSpirit);
            break;
        case rqtVitality:
            // 体质是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentVitality >= pData->nValue,
                ircTooLowVitality);
            break;
        case rqtLevel:
            // 等级是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= pData->nValue,
                ircTooLowLevel);
            break;
        default:
            break;
        }
    }

    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::CanEquip(KItem* pItem, DWORD dwX)
{
    ITEM_RESULT_CODE nResult = ircInvalid;
    BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR_RET_CODE(pItem, ircFailed);
    KG_PROCESS_ERROR_RET_CODE(pItem->getGenre() == igEquipment, ircCannotEquip);
    KGLOG_PROCESS_ERROR_RET_CODE(dwX < m_Box[ibEquip].m_dwSize, ircFailed);

    //检测耐久度
    //KG_PROCESS_ERROR_RET_CODE(pItem->m_nCurrentDurability > 0, ircTooLowDurability);

    //TODO:检查格子是否正确
    KG_PROCESS_ERROR_RET_CODE(g_CanEquipList[dwX][pItem->m_Common.nSub], ircErrorEquipPlace);

    //TODO:战斗状态等检查

    // 检查玩家条件是否符合装备要求
    KAttribute* pData = pItem->m_pRequireAttr;
    while (NULL != pData)
    {
        switch(pData->nKey)
        {
        case rqtFaction:
            // 门派是否相同
            //KG_PROCESS_ERROR();
            break;
        case rqtStrength:
            // 力量是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentStrength >= pData->nValue1,
                ircTooLowStrength);
            break;
        case rqtAgility:
            // 敏捷是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentAgility >= pData->nValue1,
                ircTooLowAgility);
            break;
        case rqtSpirit:
            // 根骨是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentSpirit >= pData->nValue1,
                ircTooLowSpirit);
            break;
        case rqtVitality:
            // 体质是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nCurrentVitality >= pData->nValue1,
                ircTooLowVitality);
            break;
        case rqtLevel:
            // 等级是否高于或等于装备要求
            KGLOG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= pData->nValue1,
                ircTooLowLevel);
            break;
        default:
            break;
        }
        pData = pData->pNext;
    }

    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::DestoryItem(const TItemPos& Pos, BOOL bSync /*= TRUE*/)
{
    ITEM_RESULT_CODE nResult = ircFailed;
    BOOL bRetCode = FALSE;
    KItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(Pos.dwBox < ibTotal && Pos.dwX < m_Box[Pos.dwBox].m_dwSize);


    pItem = GetItem(Pos.dwBox, Pos.dwX);
    KG_PROCESS_ERROR_RET_CODE(pItem, ircItemNotExist);

    if (Pos.dwBox == ibEquip)
    {
        bRetCode = UnEquip(pItem, Pos.dwX);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    pItem = m_Box[Pos.dwBox].PickUpItem(Pos.dwX);
    KG_PROCESS_ERROR_RET_CODE(pItem, ircItemNotExist);

    bRetCode = g_ItemLib.FreeItem(pItem);
    ASSERT(bRetCode);

    nResult = ircSuccess;
Exit0:
    return nResult;
}

ITEM_RESULT_CODE KItemList::DestoryItem(KItem* pItem, BOOL bSync/* = TRUE*/)
{
    ITEM_RESULT_CODE nResult = ircFailed;
    BOOL bRetCode = FALSE;
    KGLOG_PROCESS_ERROR(pItem);

    for (int nBoxIndex = 0; nBoxIndex < ibTotal; nBoxIndex++)
    {
        for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; dwX++)
        {
            KItem* pTempItem = m_Box[nBoxIndex].GetItem(dwX);
            if (pItem == pTempItem)
            {
                TItemPos ItemPos = {nBoxIndex, dwX};
                nResult = DestoryItem(ItemPos, bSync);
                KGLOG_PROCESS_ERROR(nResult == ircSuccess);

                return ircSuccess;
            }
        }
    }

Exit0:

    return nResult;
}

BOOL KItemList::PushItem(KItem* pItem, DWORD dwBox)
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;
    int nCount = 0;
    int nMaxExistAmount = 0;
    int nItemAmount = 0;
    int nCanPushCount = INT_MAX;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    KGLOG_PROCESS_ERROR(m_Box[dwBox].IsOpened());
    KGLOG_PROCESS_ERROR(pItem);
    ASSERT(m_Box[dwBox].m_dwSize <= MAX_ITEM_PACKAGE_SIZE);
    ASSERT(m_pPlayer);

    nCount = pItem->GetStackNum();
    nMaxExistAmount = pItem->GetMaxExistAmount();
    nItemAmount = GetItemAmount(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex);

    if (nMaxExistAmount)
    {
        //物品的携带限制数量
        nCanPushCount = nMaxExistAmount - nItemAmount;
    }

    for (DWORD i = 0; i < m_Box[dwBox].m_dwSize; i++)
    {
        if (m_Box[dwBox].m_pItemArray[i] && m_Box[dwBox].m_pItemArray[i]->CanStack(pItem))
        {
            if (nMaxExistAmount && (nCanPushCount <= 0))
            {
                //有物品的携带限制，已经完全不能放下了
                break;
            }
            else if (nMaxExistAmount && (nCanPushCount < nCount))
            {
                //有物品的携带限制，已经不能完全放下了
                nCount = m_Box[dwBox].m_pItemArray[i]->Stack(nCanPushCount);
                break;
            }
            else
            {
                int nOldCount = nCount;
                nCount = m_Box[dwBox].m_pItemArray[i]->Stack(nCount);
                nCanPushCount -= nOldCount - nCanPushCount;
                if (!nCount)
                    break;
            }
        }
    }

    bRetCode = pItem->SetStackNum(nCount);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = (nCount == 0);
Exit0:
    return bResult;
}

DWORD KItemList::GetBoxSize(DWORD dwBox)
{
    DWORD	dwResult = 0;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);

    dwResult = m_Box[dwBox].m_dwSize;

Exit0:

    return dwResult;
}

char* KItemList::Load(char* pData)
{
    BOOL bRetCode = FALSE;
    KItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(pData);

    m_nMoney = *(DWORD*)pData;
    pData += sizeof(DWORD);

    WORD* pwItemCount = (WORD*)pData;
    pData += sizeof(WORD);

    for (int nIndex = 0; nIndex < *pwItemCount; nIndex++)
    {
        BOOL bDeleteItem = FALSE;

        KCommonItemDBData* pCommonItemDBData = (KCommonItemDBData*)pData;
        int nVersion = pCommonItemDBData->byVersion;
        DWORD dwBoxIndex = pCommonItemDBData->byBoxIndex;
        KGLOG_PROCESS_ERROR(dwBoxIndex < ibTotal);
        DWORD dwX = pCommonItemDBData->byX;
        DWORD dwTabType = pCommonItemDBData->byTabType;
        DWORD dwTabIndex = pCommonItemDBData->wTabIndex;
        time_t nGenTime = pCommonItemDBData->nGenTime;
        int nCurrentDurability = pCommonItemDBData->wDurability;


        switch (pCommonItemDBData->byTabType)
        {
        case ittWeapon:
        case ittArmor:
        case ittTrinket:
            {
                KRandEquipDBData* pRandEquipDBData = (KRandEquipDBData*)pData;
                pData += sizeof(KRandEquipDBData);

                int nQuality = pRandEquipDBData->byQuality;
                DWORD dwRandSeed = pRandEquipDBData->dwRandSeed;

                int nMagic[MAX_ITEM_MAGIC_COUNT];
                for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
                {
                    nMagic[i] = pRandEquipDBData->wMagic[i];
                }

                pItem = g_ItemLib.GenerateItem(nVersion, dwTabType, dwTabIndex, 
                    ERROR_ID, dwRandSeed, MAX_ITEM_MAGIC_COUNT, nQuality, nMagic, nGenTime);
            }
            break;
        case ittCustWeapon:
        case ittCustArmor:
        case ittCustTrinket:
            {
                KCustomEquipDBData* pCustomEquipDBData = (KCustomEquipDBData*)pData;
                pData += sizeof(KCustomEquipDBData);

                DWORD dwRandSeed = pCustomEquipDBData->dwRandSeed;

                pItem = g_ItemLib.GenerateItem(nVersion, dwTabType, dwTabIndex, 
                    ERROR_ID, dwRandSeed, 0, 0, NULL, nGenTime);
            }
            break;
        case ittOther:
            {
                pData += sizeof(KCommonItemDBData);

                pItem = g_ItemLib.GenerateItem(nVersion, dwTabType, dwTabIndex, 
                    ERROR_ID, 0, 0, 0, NULL, nGenTime);
            }
            break;
        default:
            KGLOG_PROCESS_ERROR(FALSE);
        }

        KGLOG_PROCESS_ERROR(pItem);
        pItem->m_nCurrentDurability = nCurrentDurability;

        if (bDeleteItem == FALSE)
        {
            bRetCode = m_Box[dwBoxIndex].PlaceItem(pItem, dwX);
            KGLOG_PROCESS_ERROR(bRetCode);

            if (dwBoxIndex == ibEquip)
            {
                bRetCode = Equip(pItem, dwX);
                KGLOG_PROCESS_ERROR(bRetCode);

				DWORD dwSlotBox = g_GetBoxIndexFromSlot(dwX);
				if (dwSlotBox)
				{
					EnablePackage(pItem->m_nPackageSize, pItem->m_Common.nDetail, dwSlotBox);
				}
            }

       }
        else
        {
            bRetCode = g_ItemLib.FreeItem(pItem);
            KGLOG_CHECK_ERROR(bRetCode);
        }
    }

    CheckEquipRequire();

    return pData;
Exit0:
    g_ItemLib.FreeItem(pItem);

    return NULL;
}

char* KItemList::Save(char* pData)
{
    KGLOG_PROCESS_ERROR(pData);

    DWORD* pdwMoney = (DWORD*)pData;
    (*pdwMoney) = m_nMoney;
    pData += sizeof(DWORD);

    WORD* pwItemCount = (WORD*)pData;
    pData += sizeof(WORD);

    (*pwItemCount) = 0;

    for (DWORD dwBoxIndex = 0; dwBoxIndex < ibTotal; dwBoxIndex++)
    {
        if (dwBoxIndex == ibSoldList)
        {
            continue;
        }

        for (DWORD dwX = 0; dwX < m_Box[dwBoxIndex].m_dwSize; dwX++)
        {
            KItem* pItem = m_Box[dwBoxIndex].GetItem(dwX);
            if (pItem)
            {
                KCommonItemDBData* pCommonItemDBData = (KCommonItemDBData*)pData;
                pCommonItemDBData->byVersion = (BYTE)pItem->m_GenParam.nVersion;
                pCommonItemDBData->byBoxIndex = (BYTE)dwBoxIndex;
                pCommonItemDBData->byX = (BYTE)dwX;
                pCommonItemDBData->byReserved1 = 0;
                pCommonItemDBData->byReserved2 = 0;
                pCommonItemDBData->byTabType = (BYTE)pItem->m_GenParam.dwTabType;
                pCommonItemDBData->wTabIndex = (WORD)pItem->m_GenParam.dwIndex;
                pCommonItemDBData->nGenTime = pItem->m_GenParam.nGenTime;
                pCommonItemDBData->wDurability = (WORD)pItem->m_nCurrentDurability;

                switch(pItem->m_GenParam.dwTabType)
                {
                case ittWeapon:
                case ittArmor:
                case ittTrinket:
                    {
                        KRandEquipDBData* pRandEquipDBData = (KRandEquipDBData*)pData;
                        pData += sizeof(KRandEquipDBData);

                        pRandEquipDBData->byQuality = (BYTE)pItem->m_GenParam.nQuality;
                        pRandEquipDBData->dwRandSeed = pItem->m_GenParam.dwRandSeed;
                        for (int i = 0; i < MAX_ITEM_MAGIC_COUNT; i++)
                        {
                            pRandEquipDBData->wMagic[i] = (WORD)pItem->m_GenParam.nMagicIndex[i];
                        }
                        // 附魔未完成
                        pRandEquipDBData->wExtendMagic = 0;
                        pRandEquipDBData->wTemporaryMagic = 0;
                    }
                    break;
                case ittCustWeapon:
                case ittCustArmor:
                case ittCustTrinket:
                    {
                        KCustomEquipDBData* pCustomEquipDBData = (KCustomEquipDBData*)pData;
                        pData += sizeof(KCustomEquipDBData);

                        pCustomEquipDBData->dwRandSeed = pItem->m_GenParam.dwRandSeed;
                        // 附魔还未完成
                        pCustomEquipDBData->wExtendMagic = 0;
                        pCustomEquipDBData->wTemporaryMagic = 0;
                    }
                    break;
                case ittOther:
                    {
                        pData += sizeof(KCommonItemDBData);
                    }
                    break;
                default:
                    KGLOG_PROCESS_ERROR(FALSE);
                }

                (*pwItemCount)++;
            }
        }
    }

    return pData;

Exit0:

    return NULL;
}

ITEM_RESULT_CODE KItemList::UseItem(TItemPos ItemPos, KTarget& rTarget)
{
    BOOL bRetCode = FALSE;
    return ircInvalid;
}

BOOL KItemList::TidyUpSoldList()
{
    int         nRetCode        = 0;
    BOOL        bResult         = FALSE;
    KItem*      pItem           = NULL;
    int         nFreePos     = -1;
    int         nPosIndex       = 0;
    int         nSoldListHead   = 0; 

    nSoldListHead = m_Box[ibSoldList].GetSoldListHead();
    nPosIndex = nSoldListHead;
    while (true)
    {
        pItem = m_Box[ibSoldList].GetItem(nPosIndex);
        if (pItem == NULL)
        {
            nFreePos = nPosIndex;
            break;    
        }

        ++nPosIndex;
        nPosIndex = nPosIndex >= MAX_SOLDLIST_PACKAGE_SIZE ? 0 : nPosIndex;

        if (nPosIndex == nSoldListHead)
        {
            break;
        }
    }

    m_Box[ibSoldList].Open();

    if (nFreePos != -1)
    {   
        nPosIndex = nFreePos + 1;
        while (true)
        {
            nPosIndex = nPosIndex >= MAX_SOLDLIST_PACKAGE_SIZE ? 0 : nPosIndex;
            if (nPosIndex == nSoldListHead)
            {
                break;
            }
            pItem = m_Box[ibSoldList].GetItem(nPosIndex);
            if (pItem)
            {
                TItemPos Src, Dest;
                Src.dwBox = ibSoldList;
                Src.dwX = nPosIndex;
                Dest.dwBox = ibSoldList;
                Dest.dwX = nFreePos;
                nRetCode = ExchangeItem(Src, Dest);
                KG_PROCESS_ERROR(nRetCode);

                ++nFreePos;
                nFreePos = (nFreePos >= MAX_SOLDLIST_PACKAGE_SIZE) ? 0 : nFreePos;
            }
            ++nPosIndex;
        }
    }

    bResult = TRUE;
Exit0:
    m_Box[ibSoldList].Close();
    return bResult;
}

BOOL KItemList::OpenBox(DWORD dwBox)
{
    BOOL bResult = FALSE;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    m_Box[dwBox].Open();
    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::CloseBox(DWORD dwBox)
{
    BOOL bResult = FALSE;

    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    m_Box[dwBox].Close();
    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::AbradeArmor()
{
    BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
    int i = 0;
    KItem* pItem = NULL;

    KGLOG_PROCESS_ERROR(m_Box[ibEquip].m_dwSize == eitTotal);
    pItem = m_Box[ibEquip].m_pItemArray[eitChest];
    if (pItem)
    {
        if  (pItem->Abrade())
		{
		}
		if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
		{
			UnApplyAttrib(pItem);
		}
    }

    pItem = m_Box[ibEquip].m_pItemArray[eitHelm];
    if (pItem)
    {
		if  (pItem->Abrade())
		{
		}
		if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
		{
			UnApplyAttrib(pItem);
		}
    }

    pItem = m_Box[ibEquip].m_pItemArray[eitWaist];
    if (pItem)
    {
		if  (pItem->Abrade())
		{
		}
		if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
		{
			UnApplyAttrib(pItem);
		}
    }

    pItem = m_Box[ibEquip].m_pItemArray[eitPants];
    if (pItem)
    {
		if  (pItem->Abrade())
		{
		}
		if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
		{
			UnApplyAttrib(pItem);
		}
    }

    pItem = m_Box[ibEquip].m_pItemArray[eitBoots];
    if (pItem)
    {
		if  (pItem->Abrade())
		{
		}
		if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
		{
			UnApplyAttrib(pItem);
		}
    }

    pItem = m_Box[ibEquip].m_pItemArray[eitBangle];
    if (pItem)
    {
		if  (pItem->Abrade())
		{
		}
		if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
		{
			UnApplyAttrib(pItem);
		}
    }

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::AbradeMeleeWeapon()
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;
    KItem* pItem = NULL;

    pItem = m_Box[ibEquip].m_pItemArray[eitMeleeWeapon];
    KG_PROCESS_ERROR(pItem);

	if  (pItem->Abrade())
	{
	}
	if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
	{
		UnApplyAttrib(pItem);
	}

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::AbradeRangeWeapon()
{
    BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
    KItem* pItem = NULL;

    pItem = m_Box[ibEquip].m_pItemArray[eitRangeWeapon];
    KG_PROCESS_ERROR(pItem);

	if  (pItem->Abrade())
	{
	}
	if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
	{
		UnApplyAttrib(pItem);
	}

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::AbradeEquipment(int nBoxType, int nAbradePercent)
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR(nBoxType == ivtEquipment || nBoxType == ivtPackage);
    KGLOG_PROCESS_ERROR(nAbradePercent >= 0 && nAbradePercent <= 100);

    for (int nBoxIndex = 0; nBoxIndex < ibTotal; nBoxIndex++)
    {
        if (m_Box[nBoxIndex].m_nBoxType == nBoxType)
        {
            for (DWORD dwX = 0; dwX < m_Box[nBoxIndex].m_dwSize; dwX++)
            {
                KItem* pItem = m_Box[nBoxIndex].GetItem(dwX);
                if (pItem == NULL)
                    continue;

                if (pItem->GetGenre() != igEquipment)
                    continue;

                if (pItem->GetCurrentDurability() > 0 && pItem->GetMaxDurability() > 0)
                {
                    int nCurrentDurability  = pItem->GetCurrentDurability();
                    int nMaxDurability      = pItem->GetMaxDurability();
                    int nAbradeDurability   = max(1, nMaxDurability * nAbradePercent / 100);

                    nCurrentDurability      = nCurrentDurability - nAbradeDurability;

                    MAKE_IN_RANGE(nCurrentDurability, 0, nMaxDurability);
                    bRetCode = pItem->SetCurrentDurability(nCurrentDurability);
					if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
					{
						UnApplyAttrib(pItem);
					}
                    KGLOG_PROCESS_ERROR(bRetCode);
                }

            }
        }
    }

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KItemList::ApplyAttrib(KItem* pItem)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pItem);
	
	bRetCode = m_pPlayer->ApplyAttribute(pItem->m_pBaseAttr);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_pPlayer->ApplyAttribute(pItem->m_pMagicAttr);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_pPlayer->ApplyAttribute(pItem->m_pEnchant);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_pPlayer->ApplyAttribute(pItem->m_pTemporaryEnchant);
	KGLOG_CHECK_ERROR(bRetCode);

	pItem->m_bApplyed = TRUE;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItemList::UnApplyAttrib(KItem* pItem)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pItem);
	
	bRetCode = m_pPlayer->UnApplyAttribute(pItem->m_pTemporaryEnchant);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_pPlayer->UnApplyAttribute(pItem->m_pEnchant);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_pPlayer->UnApplyAttribute(pItem->m_pMagicAttr);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_pPlayer->UnApplyAttribute(pItem->m_pBaseAttr);
	KGLOG_CHECK_ERROR(bRetCode);

	pItem->m_bApplyed = FALSE;

	bResult = TRUE;
Exit0:
	return bResult;

}

BOOL KItemList::Repair(DWORD dwBox, DWORD dwX)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	KItem* pItem = NULL;

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
	KGLOG_PROCESS_ERROR(dwX < m_Box[dwBox].m_dwSize);

	pItem = m_Box[dwBox].GetItem(dwX);
	KGLOG_PROCESS_ERROR(pItem);

	if (pItem->Repair())
	{
	}

	if (pItem->m_bEquiped && !pItem->m_bApplyed)	//装备着并且没应用过属性
	{
		bRetCode = ApplyAttrib(pItem);
		KGLOG_CHECK_ERROR(bRetCode);
	}

	bResult = TRUE;
Exit0:
	return bResult;
}

// 检查物品是否符合背包限制
BOOL KItemList::CanPut(KItem* pItem, DWORD dwOrgBox, DWORD dwBox, DWORD dwX, BOOL bSrc)
{
	BOOL	bResult = FALSE;
	BOOL	bRetCode = FALSE;

	KG_PROCESS_SUCCESS(!pItem);	// 没道具，算成功。
	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
	
	switch(dwBox)
	{
	case ibEquip:
		{
			DWORD dwNewBox = g_GetBoxIndexFromSlot(dwX);
			if (!dwNewBox)
			{
				KG_PROCESS_ERROR(pItem->m_Common.nGenre == igEquipment);
			}
		}
		
		if (dwX != eitPackage1 && dwX != eitPackage2 && dwX != eitPackage3 && dwX != eitPackage4)
		{
			bRetCode = (CanEquip(pItem, dwX) == ircSuccess);
			KG_PROCESS_ERROR(bRetCode);
		}
		else	// 向背包上放
		{
			KItem* pPackage = m_Box[ibEquip].GetItem(dwX);

			if (pPackage)
			{
				DWORD dwBoxIndex = g_GetBoxIndexFromSlot(dwX);
				if (pItem->m_Common.nSub == estPackage)
				{
					KG_PROCESS_ERROR(dwBoxIndex != dwOrgBox);

					// 背包类型相同或者新换上去的包是无限制类型，或者目标包中无东西或者源包已经装备
					KG_PROCESS_ERROR(pPackage->m_Common.nDetail == pItem->m_Common.nDetail || 
						pItem->m_Common.nDetail == 0 || m_Box[dwBoxIndex].GetUsedCount() == 0 ||
						pItem->m_bEquiped);

					KG_PROCESS_ERROR(pItem->m_nPackageSize >= m_Box[dwBoxIndex].GetUsedCount());
				}
				else
				{
					 if (!bSrc)
					 {
						 KG_PROCESS_ERROR(m_Box[dwBoxIndex].m_nContainItemType == 0 || 
							(pItem->m_Common.nGenre == igMaterial && 
							m_Box[dwBoxIndex].m_nContainItemType == pItem->m_Common.nSub));
					 }
					 else
					 {
						 goto Exit0;
					 }
				}
			}
			else
			{
				bRetCode = (CanEquip(pItem, dwX) == ircSuccess);
				KG_PROCESS_ERROR(bRetCode);
			}
		}
		break;
	case ibSoldList:
		break;
	default:
		if (m_Box[dwBox].m_nContainItemType != 0)
		{
			KG_PROCESS_ERROR(pItem->m_Common.nGenre == igMaterial);
			KG_PROCESS_ERROR(pItem->m_Common.nSub == m_Box[dwBox].m_nContainItemType);
		}
		break;
	}
Exit1:
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItemList::IsPackageSlot(DWORD dwBox, DWORD dwX)
{
	BOOL bResult = FALSE;

	KG_PROCESS_ERROR(dwBox == ibEquip);
	KG_PROCESS_ERROR(dwX == eitPackage1 || dwX == eitPackage2 ||
		dwX == eitPackage3 || dwX == eitPackage4);

	bResult = TRUE;
Exit0:
	return bResult;
}

ITEM_RESULT_CODE KItemList::EnablePackage(DWORD dwSize, int nContainType, DWORD dwBox)
{
	ITEM_RESULT_CODE nResult = ircFailed;

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
	KG_PROCESS_ERROR_RET_CODE((nContainType == m_Box[dwBox].m_nContainItemType || 
		nContainType == 0 || m_Box[dwBox].GetUsedCount() == 0), ircPackageErrorType);
	KG_PROCESS_ERROR_RET_CODE(((DWORD)m_Box[dwBox].GetUsedCount() <= dwSize), ircPackageNotEnoughSize);

	m_Box[dwBox].m_nContainItemType = nContainType;
	DWORD dwOldSize = m_Box[dwBox].m_dwSize;
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
			ASSERT(i < dwSize);
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

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);
	KG_PROCESS_ERROR_RET_CODE((m_Box[dwBox].GetUsedCount() == 0), ircCannotDisablePackage);

	m_Box[dwBox].m_nContainItemType = 0;
	m_Box[dwBox].m_dwSize = 0;
	nResult = ircSuccess;
Exit0:
	return nResult;
}

ITEM_RESULT_CODE KItemList::ExchangePackageSlot(DWORD dwBox1, DWORD dwBox2)
{
	KInventory ivTemp;
	ITEM_RESULT_CODE nResult = ircFailed;

	KGLOG_PROCESS_ERROR(dwBox1 < ibTotal && dwBox2 < ibTotal);
	KGLOG_PROCESS_ERROR(dwBox1 != dwBox2);

	ivTemp = m_Box[dwBox1];
	m_Box[dwBox1] = m_Box[dwBox2];
	m_Box[dwBox2] = ivTemp;

	nResult = ircSuccess;
Exit0:
	return nResult;
}

BOOL KItemList::ConsumeAmmo(DWORD dwAmount)
{
	BOOL	bResult = FALSE;
	BOOL	bRetCode = FALSE;
	DWORD	dwTabType = 0;
	DWORD	dwIndex = 0;

	KItem* pItem = m_Box[ibEquip].GetItem(eitArrow);
	KG_PROCESS_ERROR(pItem);

	dwTabType = pItem->m_GenParam.dwTabType;
	dwIndex = pItem->m_GenParam.dwIndex;

	bRetCode = CostSingleItem(ibEquip, eitArrow, dwAmount);
	KGLOG_PROCESS_ERROR(bRetCode);	// 希望调用者先查数量再使用，否则会记LOG

	pItem = m_Box[ibEquip].GetItem(eitArrow);	//有可能在CostSingleItem时被Destory变成NULL
	if (!pItem)
	{
		bRetCode = ReloadAmmo(dwTabType, dwIndex);
		// 填弹成功与否无所谓
	}

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItemList::ReloadAmmo(DWORD dwTabType, DWORD dwIndex)
{
	BOOL	bResult = FALSE;
	BOOL	bRetCode = FALSE;
	DWORD	dwBox = 0;
	DWORD	dwX = 0;

	KItem* pItem = FindItem(dwTabType, dwIndex, dwBox, dwX);

	if (pItem)
	{
		TItemPos Pos1 = {dwBox, dwX};
		TItemPos Pos2 = {ibEquip, eitArrow};
		bRetCode = ExchangeItem(Pos1, Pos2);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bResult = TRUE;
Exit0:
	return bResult;
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
