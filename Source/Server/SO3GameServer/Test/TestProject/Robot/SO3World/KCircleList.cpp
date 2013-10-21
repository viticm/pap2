#include "stdafx.h"
#include "KCircleList.h"

KCircleList::KCircleList(void)
{
	m_dwHead = m_dwRear = 0;
	m_ObjList = NULL;
	m_dwSize = 0;
}

KCircleList::~KCircleList(void)
{
	KG_DELETE_ARRAY(m_ObjList);
}

BOOL KCircleList::Init(DWORD dwSize)
{
	KG_DELETE_ARRAY(m_ObjList);
	m_dwSize = 0;
	
	m_ObjList = new void*[dwSize];
	
	if (m_ObjList == NULL)
		return FALSE;

	m_dwHead = m_dwRear = 0;
	m_dwSize = dwSize;

	return TRUE;
}

BOOL KCircleList::Push(void* pObj)
{
	ASSERT(m_dwSize);
	DWORD dwNextPos = (m_dwRear + 1) % m_dwSize;

	if (pObj == NULL || dwNextPos == m_dwHead)
		return FALSE;

	m_ObjList[m_dwRear] = pObj;
	m_dwRear = dwNextPos;

	return TRUE;
}

void* KCircleList::Pop(void)
{
	if (m_dwHead == m_dwRear)
		return NULL;

	ASSERT(m_dwSize);
	void* pObj = m_ObjList[m_dwHead];
	m_dwHead = (m_dwHead + 1) % m_dwSize;

	return pObj;
}

void* KCircleList::Check(void)
{
	if (m_dwHead == m_dwRear)
		return NULL;

	return m_ObjList[m_dwHead];
}