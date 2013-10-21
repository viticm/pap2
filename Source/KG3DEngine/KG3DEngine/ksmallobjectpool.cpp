#include "stdafx.h"
#include "./ksmallobjectpool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct KChunk
{
    KChunk* pNext;
    char pData[1];
};

struct KObject
{
    union
    {
        KObject* pNext;
        void* pData;
    };
};

KSmallObjectPool::KSmallObjectPool()
: m_szName("")
, m_pChunkList(NULL)
, m_pFreeObjectList(NULL)
, m_nChunkObjectCount(0)
, m_nObjectSize(0)
{
}

KSmallObjectPool::KSmallObjectPool(LPCSTR szName, size_t nObjectSize, size_t nChunkObjectCount)
: m_szName(szName ? szName : "")
, m_pChunkList(NULL)
, m_pFreeObjectList(NULL)
, m_nChunkObjectCount(nChunkObjectCount)
, m_nObjectSize(nObjectSize)
{
}

KSmallObjectPool::~KSmallObjectPool()
{
    size_t nAllocObjectCount = 0;
	size_t nFreeObjectCount = 0;

    for (KChunk* pChunk = m_pChunkList; pChunk; pChunk = pChunk->pNext)
    {
        nAllocObjectCount += m_nChunkObjectCount;
    }

    for (KObject* pObject = m_pFreeObjectList; pObject; pObject = pObject->pNext)
    {
        ++nFreeObjectCount;
    }

	if (nAllocObjectCount > nFreeObjectCount)
	{
		KGLogPrintf(KGLOG_ERR, "[KSmallObjectPool] Resource Leak %d %s\n", nAllocObjectCount - nFreeObjectCount, m_szName);
	}

	Purge();
}

int KSmallObjectPool::Init(size_t nObjectSize, size_t nChunkObjectCount)
{
    ASSERT(m_pFreeObjectList == NULL);
    ASSERT(m_pChunkList == NULL);
    ASSERT(nObjectSize >= sizeof(KObject*));

    m_nObjectSize = nObjectSize;
    m_nChunkObjectCount = nChunkObjectCount;

    return true;
}

void* KSmallObjectPool::Alloc()
{
    KObject* pObject = NULL;

    if (m_pFreeObjectList == NULL) 
    {
        ASSERT(m_nObjectSize > 0);
        ASSERT(m_nChunkObjectCount > 0);

        KChunk* pChunk = (KChunk*)(malloc(sizeof(KChunk*) + m_nChunkObjectCount * m_nObjectSize));
        if (pChunk == NULL)
        {
            KGLogPrintf(KGLOG_ERR, "[KSmallObjectPool] alloc failed %d %d %s\n", m_nChunkObjectCount, m_nObjectSize, m_szName);
            return NULL;
        }

        pChunk->pNext = m_pChunkList;
        m_pChunkList = pChunk;  

        for (size_t nIndex = 0; nIndex < m_nChunkObjectCount; ++nIndex)
        {
            pObject = (KObject*)(m_pChunkList->pData + nIndex * m_nObjectSize);
            pObject->pNext = m_pFreeObjectList;
            m_pFreeObjectList = pObject;
        }
    }

    pObject = m_pFreeObjectList;
    m_pFreeObjectList = m_pFreeObjectList->pNext;

    return (void*)pObject;
}

void KSmallObjectPool::Free(void* pValue)
{
    KObject* pObject = NULL;

    ASSERT(pValue);

    pObject = (KObject*)pValue;
    
    pObject->pNext = m_pFreeObjectList;
    m_pFreeObjectList = pObject;
}

void KSmallObjectPool::Purge()
{
    while (m_pChunkList)
    {
        KChunk* pChunk = m_pChunkList;
        m_pChunkList = m_pChunkList->pNext;
        free(pChunk);
    }

    m_pChunkList = NULL;
    m_pFreeObjectList = NULL;
}
