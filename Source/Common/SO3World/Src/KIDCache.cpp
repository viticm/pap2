#include "stdafx.h"
#include "KIDCache.h"

KIDCache::KIDCache(void)
{
	m_dwMaxSize = 0;

	return;
}

KIDCache::~KIDCache(void)
{
	Clear();

	return;
}

BOOL KIDCache::Init(DWORD dwMaxSize)
{
	//≥ı ºªØ
	BOOL bRetCode = false;

	bRetCode = Clear();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_dwMaxSize = dwMaxSize;

	return true;
Exit0:

	return false;
}

void* KIDCache::AddData(DWORD dwID, void* pData, DWORD dwSize)
{
	std::pair<MAP_ID_2_NODE::iterator, bool> InsRet;
	MAP_ID_2_NODE::iterator it;

	KIDCacheNode* pCacheNode = NULL;

	it = m_mapId2Node.find(dwID);
	if (it != m_mapId2Node.end())
	{
		pCacheNode = it->second;
		pCacheNode->Remove();
		delete [] ((char*)pCacheNode);
	}
	else if (m_mapId2Node.size() >= m_dwMaxSize)
	{
		pCacheNode = (KIDCacheNode*)m_CacheList.GetTail();
		m_mapId2Node.erase(pCacheNode->m_dwID);
		pCacheNode->Remove();
		delete [] ((char*)pCacheNode);
	}

	pCacheNode = (KIDCacheNode*)new char[sizeof(KIDCacheNode) + dwSize];
	KGLOG_PROCESS_ERROR(pCacheNode);

	pCacheNode->m_dwID = dwID;
	pCacheNode->m_dwSize = dwSize;
	memcpy(&(pCacheNode->m_szData), pData, dwSize);

	m_CacheList.AddHead(pCacheNode);

	if (it == m_mapId2Node.end())
	{
		InsRet = m_mapId2Node.insert(std::make_pair(dwID, pCacheNode));
		KGLOG_PROCESS_ERROR(InsRet.second);
	}
	else
	{
		it->second = pCacheNode;
	}

	return pCacheNode->m_szData;

Exit0:

	return NULL;
}

void* KIDCache::GetData(DWORD dwID, DWORD& rdwSize)
{
	BOOL            bRetCode   = false;
    KIDCacheNode*   pCacheNode = NULL;

	MAP_ID_2_NODE::iterator it = m_mapId2Node.find(dwID);

	KG_PROCESS_ERROR(it != m_mapId2Node.end());

	pCacheNode = it->second;
	KGLOG_PROCESS_ERROR(pCacheNode);

	pCacheNode->Remove();
	m_CacheList.AddHead(pCacheNode);

	rdwSize = pCacheNode->m_dwSize;

	return &(it->second->m_szData);

Exit0:

	return NULL;
}

//«Âø’ª∫¥Ê
BOOL KIDCache::Clear()
{
	KIDCacheNode* pNode = (KIDCacheNode*)m_CacheList.GetHead();
	while (pNode != NULL)
	{
		KIDCacheNode* pTempNode = (KIDCacheNode*)pNode->GetNext();

		pNode->Remove();

		delete [] ((char*)pNode);

		pNode = pTempNode;
	}

	m_mapId2Node.clear();

	return true;
}

BOOL KIDCache::LoadFromBuffer(char* pData, DWORD dwSize)
{
	KGLOG_PROCESS_ERROR(pData);

	while (dwSize > 0)
	{
		KIDCacheNode* pNode = (KIDCacheNode*)pData;
		AddData(pNode->m_dwID, pNode->m_szData, pNode->m_dwSize);
		dwSize -= pNode->m_dwSize;
		pData += (sizeof(KIDCacheNode) + pNode->m_dwSize);
	}

	return true;
Exit0:
	return false;
}

BOOL KIDCache::SaveToBuffer(char* pData, DWORD dwSize)
{
	KIDCacheNode* pNode = (KIDCacheNode*)m_CacheList.GetHead();
	while (pNode)
	{
		DWORD dwNodeSize = sizeof(KIDCacheNode) + pNode->m_dwSize;
		KGLOG_PROCESS_ERROR(dwSize >= dwNodeSize);

		memcpy(pData, pNode, dwNodeSize);
		pData += dwNodeSize;
		dwSize -= dwNodeSize;

		pNode = (KIDCacheNode*)pNode->GetNext();
	}

	return true;
Exit0:
	return false;
}
