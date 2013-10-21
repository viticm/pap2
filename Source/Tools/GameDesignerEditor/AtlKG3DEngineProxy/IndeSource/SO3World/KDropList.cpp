#include "stdafx.h"
#include "Global.h"
#include "KDropList.h"
#include "Engine/KGCRT.h"
#include "KSO3World.h"
#include "KItem.h"

KDropList::KDropList(void)
{
	m_ItemList = NULL;
	m_dwSize = 0;
	m_dwIndex = 1;
}

KDropList::~KDropList(void)
{
	KG_DELETE_ARRAY(m_ItemList);
}

//初始化，设置表的大小
BOOL KDropList::Init(DWORD dwSize)
{
	m_ItemList = new DROP_DATA[dwSize + 1];
	KGLOG_PROCESS_ERROR(m_ItemList);

	memset(m_ItemList, 0, sizeof(DROP_DATA) * (dwSize + 1));
	m_dwSize = dwSize;
	m_dwIndex = 1;

	return true;

Exit0:

	return false;
}

//设定道具掉落表，掉落表的序号，道具类型，道具编号，掉落率％
BOOL KDropList::AddItem(DROP_DATA& DropData)
{
	KGLOG_PROCESS_ERROR(m_dwIndex <= m_dwSize);

	m_ItemList[m_dwIndex] = DropData;
	m_dwIndex++;

	return true;
Exit0:
	return false;
}

//计算掉落率区间
BOOL KDropList::PreProcess(void)
{
	int nCurrentDropRate = -1;
	for (DWORD dwIndex = 1; dwIndex <= m_dwSize; dwIndex++)
	{
		nCurrentDropRate += m_ItemList[dwIndex].nDropRate;
		m_ItemList[dwIndex].nDropRate = nCurrentDropRate;
	}

	KGLOG_PROCESS_ERROR(nCurrentDropRate < MILLION_NUM);

	return true;

Exit0:

	return false;
}

KItem* KDropList::GetRandomItem(DROP_DATA& DropData)
{
	int		nHead		= 0;
	int		nRear		= m_dwSize;
	int		nMid		= 0;
	BOOL	bFound		= false;
	int		nDropRate	= 0;
	DWORD	dwItemTab	= 0;
	DWORD	dwItemIndex = 0;
	int		nCount		= 0;
	KItem*	pItem		= NULL;

	nDropRate = g_Random(MILLION_NUM);
	KG_PROCESS_ERROR(nDropRate<= m_ItemList[nRear].nDropRate);
	
	while (!bFound)
	{
		if (nHead + 1 == nRear)
			bFound = true;
		else
		{
			nMid = (nHead + nRear) / 2;
			if (m_ItemList[nMid].nDropRate < nDropRate)
			{
				nHead = nMid;
			}
			else if (m_ItemList[nMid].nDropRate > nDropRate)
			{
				nRear = nMid;
			}
			else
			{
				nRear = nMid;
				bFound = true;
			}
		}
	}

	dwItemTab = m_ItemList[nRear].dwTabType;
	dwItemIndex = m_ItemList[nRear].dwItemIndex;
	DropData = m_ItemList[nRear];

	if (m_ItemList[nRear].dwMin == 0 && m_ItemList[nRear].dwMax == 0)
	{
		//不能叠加的物品
		nCount = 0;
	}
	else
	{
		DWORD dwMin = m_ItemList[nRear].dwMin;
		DWORD dwMax = m_ItemList[nRear].dwMax;

		nCount = g_Random(dwMax - dwMin + 1);
		nCount += dwMin;
	}

	//生存道具
	pItem = g_pSO3World->m_ItemManager.GenerateItem(CURRENT_ITEM_VERSION, dwItemTab, dwItemIndex);
	KGLOG_PROCESS_ERROR(pItem);

	if (pItem->m_Common.nGenre == igBook)
	{
		pItem->m_nCurrentDurability = nCount;
	}
	else if (nCount > 0)
	{
		if (pItem->IsStackable())
		{
			pItem->SetStackNum(nCount);
		}
	}

	return pItem;
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
		pItem = NULL;
	}
	return NULL;
}

BOOL KMoneyDropList::Init()
{
	m_dwMinMoney = 0;
	m_dwMaxMoney = 0;
	m_dwMoneyDropRate = 0;

	return true;
}

BOOL KMoneyDropList::AddMoney(DWORD dwMinMoney, DWORD dwMaxMoney)
{
	KGLOG_PROCESS_ERROR(dwMinMoney <= dwMaxMoney);

	m_dwMinMoney += dwMinMoney;
	m_dwMaxMoney += dwMaxMoney;

	return true;
Exit0:
	return false;
}

BOOL KMoneyDropList::SetMoneyDropRate(DWORD dwMoneyDropRate)
{
	KGLOG_PROCESS_ERROR(dwMoneyDropRate <= MILLION_NUM);
	
	m_dwMoneyDropRate = dwMoneyDropRate;

	return true;
Exit0:
	return false;
}

DWORD KMoneyDropList::GetRandomMoney()
{
	int nResult = 0;

	int nMoneyRate = m_dwMaxMoney - m_dwMinMoney + 1;

	nResult = g_Random(nMoneyRate);
	nResult += m_dwMinMoney;
	
	return nResult;
}

BOOL KMoneyDropList::IsDropInRandom()
{
	BOOL bResult = false;

	int nRate = g_Random(MILLION_NUM);

	if (nRate <= (int)m_dwMoneyDropRate)
	{
		bResult = true;
	}

	return bResult;
}
