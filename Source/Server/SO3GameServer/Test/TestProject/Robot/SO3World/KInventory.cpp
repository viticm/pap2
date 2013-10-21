#include "stdafx.h"
#include "KItem.h"
#include "KItemLib.h"
#include "KInventory.h"

KInventory::KInventory()
{
	m_dwSize = 0;
	m_dwUsed = 0;
	m_bTakeWith = FALSE;
	m_nSoldListHead = 0;
}

KInventory::~KInventory()
{
}

BOOL KInventory::Init(DWORD dwSize, int nBoxType, BOOL bTakeWith)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(dwSize <= MAX_ITEM_PACKAGE_SIZE);
	KGLOG_PROCESS_ERROR(nBoxType < ivtTotal);

	m_dwSize = dwSize;
	m_dwUsed = 0;
	m_nBoxType = nBoxType;
	m_bTakeWith = bTakeWith;
	m_nSoldListHead = 0;
	ZeroMemory(m_pItemArray, sizeof(KItem *) * MAX_ITEM_PACKAGE_SIZE);

	bResult = TRUE;
Exit0:
	return bResult;
}

KItem* KInventory::GetItem(DWORD dwX)
{
	KItem* pItem = NULL;

	ASSERT(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);

	KGLOG_PROCESS_ERROR(dwX < m_dwSize);

	pItem = m_pItemArray[dwX];
Exit0:
	return pItem;
}

// 放置装备，要求原本格子为空
BOOL KInventory::PlaceItem(KItem* pItem, DWORD dwX)
{
	BOOL bResult = FALSE;

	ASSERT(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);

	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(dwX < m_dwSize);
	KGLOG_PROCESS_ERROR(NULL == m_pItemArray[dwX]);
    
	m_pItemArray[dwX] = pItem;
	m_dwUsed++;

	bResult = TRUE;
Exit0:
	return bResult;
}

// 清除装备，返回被清的装备指针
KItem* KInventory::PickUpItem(DWORD dwX)
{
	KItem* pItem = NULL;

	ASSERT(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);
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
	DWORD i = 0;

	ASSERT(m_dwSize <= MAX_ITEM_PACKAGE_SIZE);

	for (i = 0; i < m_dwSize; i++)
	{
		if (NULL == m_pItemArray[i])
			return i;
	}

	return -1;
}

int KInventory::FindFreeSoldList(int& nDelPos)
{
	int nResult = -1;
	
	KGLOG_PROCESS_ERROR(m_nBoxType == ivtSoldlist);
	KGLOG_PROCESS_ERROR(m_dwSize <= MAX_SOLDLIST_PACKAGE_SIZE);
	ASSERT(m_nSoldListHead >= 0);
	ASSERT((DWORD)m_nSoldListHead < m_dwSize);

	int nPlace = m_nSoldListHead - 1;
	if (nPlace < 0)
	{
		nPlace += m_dwSize;
	}
	if (m_pItemArray[nPlace])
	{
		BOOL bRetCode = g_ItemLib.FreeItem(m_pItemArray[nPlace]);
		KGLOG_PROCESS_ERROR(bRetCode);

		m_pItemArray[nPlace] = NULL;
        nDelPos = nPlace;
	}
    m_nSoldListHead = nPlace;

	nResult = nPlace;    
Exit0:
	return nResult;
}

BOOL KInventory::IsOpened()
{
	return m_bTakeWith;
}

void KInventory::Open()
{
	m_bTakeWith = TRUE;
}

void KInventory::Close()
{
	m_bTakeWith = FALSE;
}

int KInventory::GetSoldListHead()
{
    return m_nSoldListHead;
}

int KInventory::GetFreeCount()
{
	return m_dwSize - m_dwUsed;
}
