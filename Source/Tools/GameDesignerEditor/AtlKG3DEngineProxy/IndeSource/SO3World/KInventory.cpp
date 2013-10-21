#include "stdafx.h"
#include "KItem.h"
#include "KInventory.h"
#include "KSO3World.h"

BOOL KInventory::Init(DWORD dwSize, int nBoxType)
{
	BOOL bResult = false;

	KGLOG_PROCESS_ERROR(dwSize <= MAX_ITEM_PACKAGE_SIZE);
	KGLOG_PROCESS_ERROR(nBoxType < ivtTotal);

	m_dwSize                 = dwSize;
	m_dwUsed                 = 0;
	m_nBoxType               = nBoxType;
    m_nContainItemGenerType  = INVALID_CONTAIN_ITEM_TYPE;
    m_nContainItemSubType    = INVALID_CONTAIN_ITEM_TYPE;
	m_nSoldListHead          = 0;

	ZeroMemory(m_pItemArray, sizeof(KItem*) * MAX_ITEM_PACKAGE_SIZE);

	bResult = true;
Exit0:
	return bResult;
}

KItem* KInventory::GetItem(DWORD dwX)
{
	KItem* pItem = NULL;

	assert(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);

	KGLOG_PROCESS_ERROR(dwX < m_dwSize);

	pItem = m_pItemArray[dwX];
Exit0:
	return pItem;
}

DWORD KInventory::GetItemPosByItemID(DWORD dwItemID)
{
    for (DWORD dwX = 0; dwX < m_dwSize; dwX++)
    {
        KItem* pTempItem = GetItem(dwX);
        if (pTempItem && pTempItem->m_dwID == dwItemID)
        {
            return dwX;
        }
    }
    return ULONG_MAX;
}

// 放置装备，要求原本格子为空
BOOL KInventory::PlaceItem(KItem* pItem, DWORD dwX)
{
	BOOL bResult = false;

	assert(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(dwX < m_dwSize);
	KGLOG_PROCESS_ERROR(NULL == m_pItemArray[dwX]);
    
	m_pItemArray[dwX] = pItem;
	m_dwUsed++;

	bResult = true;
Exit0:
	return bResult;
}

// 清除装备，返回被清的装备指针
KItem* KInventory::PickUpItem(DWORD dwX)
{
	KItem* pItem = NULL;

	assert(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);
	KGLOG_PROCESS_ERROR(dwX < m_dwSize);

	pItem = m_pItemArray[dwX];
	m_pItemArray[dwX] = NULL;
	if (pItem)
	{
		m_dwUsed--;
	}

Exit0:
	return pItem;
}

int KInventory::FindFreePlace()
{
	assert(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);

	for (DWORD i = 0; i < m_dwSize; ++i)
	{
		if (m_pItemArray[i] == NULL)
			return i;
	}

	return -1;
}

int KInventory::FindFreeSoldList(int& rnDelPos)
{
	int nResult = -1;
    int nPlace  = 0;
	
	KGLOG_PROCESS_ERROR(m_nBoxType == ivtSoldlist);
	KGLOG_PROCESS_ERROR(m_dwSize <= MAX_SOLDLIST_PACKAGE_SIZE);
	assert(m_nSoldListHead >= 0);
	assert((DWORD)m_nSoldListHead < m_dwSize);

	nPlace = m_nSoldListHead - 1;
	if (nPlace < 0)
	{
		nPlace += m_dwSize;
	}
	if (m_pItemArray[nPlace])
	{
		BOOL bRetCode = g_pSO3World->m_ItemManager.FreeItem(m_pItemArray[nPlace]);
		KGLOG_PROCESS_ERROR(bRetCode);

		m_pItemArray[nPlace] = NULL;
        rnDelPos = nPlace;
	}
    m_nSoldListHead = nPlace;

	nResult = nPlace;    
Exit0:
	return nResult;
}

int KInventory::GetSoldListHead()
{
    return m_nSoldListHead;
}

void KInventory::SetContainItemType(int nContainItemGenerType, int nContainItemSubType)
{
    m_nContainItemGenerType = nContainItemGenerType;
    m_nContainItemSubType   = nContainItemSubType;
}

void KInventory::GetContainItemType(int* pnContainItemGenerType, int* pnContainItemSubType)
{
    assert(pnContainItemGenerType);
    assert(pnContainItemSubType);

    *pnContainItemGenerType = m_nContainItemGenerType;
    *pnContainItemSubType   = m_nContainItemSubType;
}

BOOL KInventory::CheckContainItemType(int nItemGenerType, int nItemSubType)
{
    BOOL bResult = false;

    if (m_nContainItemGenerType != INVALID_CONTAIN_ITEM_TYPE)
    {
        KG_PROCESS_ERROR(m_nContainItemGenerType == nItemGenerType);

        if (m_nContainItemSubType != INVALID_CONTAIN_ITEM_TYPE)
        {
            KG_PROCESS_ERROR(m_nContainItemSubType == nItemSubType);
        }
    }
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KInventory::CheckContainItemType(KItem* pItem)
{
    assert(pItem);
    return CheckContainItemType(pItem->m_Common.nGenre, pItem->m_Common.nSub);
}
