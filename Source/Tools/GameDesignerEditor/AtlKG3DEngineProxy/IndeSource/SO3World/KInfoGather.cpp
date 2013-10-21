#include "stdafx.h"
#include "KInfoGather.h"

#define WILDCARD_CHAR	'?'

KInfoGather::KInfoGather(void)
{
	m_pInfoKeyHead = NULL;
}

KInfoGather::~KInfoGather(void)
{
}

BOOL KInfoGather::Init(void)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pInfoKeyHead == NULL);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KInfoGather::UnInit(void)
{
	while (m_pInfoKeyHead)
	{
		KInfoNode* pInfoNode = m_pInfoKeyHead->m_InfoNode.m_pNext;
		while (pInfoNode)
		{
			pInfoNode->m_pInfo->m_nKeyRefCount--;
			if (pInfoNode->m_pInfo->m_nKeyRefCount <= 0)
			{
				delete pInfoNode->m_pInfo;
			}
			pInfoNode = pInfoNode->m_pNext;
		}
		KInfoKey* pInfoKey = m_pInfoKeyHead->m_pNext;
		delete m_pInfoKeyHead;
		m_pInfoKeyHead = pInfoKey;
	}

	return TRUE;
}

BOOL KInfoGather::AddInfo(const char* pcszInfo)
{
    KInformation* pInfo = NULL;
    
    KGLOG_PROCESS_ERROR(pcszInfo);

	pInfo = (KInformation*)new char[sizeof(KInformation) + strlen(pcszInfo) + 1];
	KGLOG_PROCESS_ERROR(pInfo);

	pInfo->m_nAccessCount = 0;
	pInfo->m_nKeyRefCount = 0;
	pInfo->m_nInfoLen = (int)strlen(pcszInfo) + 1;
	strcpy(pInfo->m_szInfo, pcszInfo);

	KAddInfoProcessor AddInfoProcessor;
	AddInfoProcessor.m_pInfo = pInfo;
	AnalysisKey(pcszInfo, AddInfoProcessor);

	if (pInfo->m_nKeyRefCount == 0)
	{
		delete[] pInfo;
	}

	return TRUE;

Exit0:

	return FALSE;
}

const char* KInfoGather::GetInfo(const char* pcszExample)
{
	const char*     pResult     = NULL;
    KInformation*   pInfo       = NULL;
    int             nMaxWeight  = 0;
    KGetInfoProcessor GetInfoProcessor;
	std::map<KInformation*, int> InfoList;
	std::map<KInformation*, int>::iterator it;

	KGLOG_PROCESS_ERROR(pcszExample);

	GetInfoProcessor.m_nCount = 0;
	
	AnalysisKey(pcszExample, GetInfoProcessor);

	for (int nIndex = 0; nIndex < GetInfoProcessor.m_nCount; nIndex++)
	{
		KInfoKey* pInfoKey = GetInfoProcessor.m_pResultList[nIndex];
		KGLOG_PROCESS_ERROR(pInfoKey);

		KInfoNode* pInfoNode = pInfoKey->m_InfoNode.m_pNext;
		while (pInfoNode)
		{
			it = InfoList.find(pInfoNode->m_pInfo);
			if (it == InfoList.end())
			{
				InfoList.insert(std::make_pair(pInfoNode->m_pInfo, pInfoKey->m_nWeight));
			}
			else
			{
				it->second += pInfoKey->m_nWeight;
			}

			pInfoNode = pInfoNode->m_pNext;
		}
	}

	for (it = InfoList.begin(); it != InfoList.end(); ++it)
	{
		if (it->second > nMaxWeight)
		{
			pInfo = it->first;
			nMaxWeight = it->second;
		}
	}

	if (pInfo)
	{
		pResult = pInfo->m_szInfo;
	}

Exit0:

	return pResult;
}

BOOL KInfoGather::AddKey(const char* pcszKey, int nWeight)
{
	BOOL        bRetCode = FALSE;
    KInfoKey*   pInfoKey = NULL;

	KGLOG_PROCESS_ERROR(pcszKey);

	pInfoKey = new KInfoKey;
	KGLOG_PROCESS_ERROR(pInfoKey);

	memset(pInfoKey, 0, sizeof(KInfoKey));
	pInfoKey->m_nWeight = nWeight;
	pInfoKey->m_byEndPos = (BYTE)strlen(pcszKey) - 1;
	strncpy(pInfoKey->m_szKey, pcszKey, INFO_KEY_LEN);
	bRetCode = CreateNextTable(pInfoKey->m_szKey, pInfoKey->m_NextTable);
	KGLOG_PROCESS_ERROR(bRetCode);

	pInfoKey->m_pNext = m_pInfoKeyHead;
	m_pInfoKeyHead = pInfoKey;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KInfoGather::CreateNextTable(const char* pcszPattern, char* pNextTable)
{
	if (pcszPattern == NULL || pNextTable == NULL)
		return FALSE;

	int nPatternSize = (int)strlen(pcszPattern);
	pNextTable[0] = -1;

	for (int nTableIndex = 1; nTableIndex < nPatternSize; nTableIndex++)
	{
		int nPatternIndex = pNextTable[nTableIndex - 1];
		while (pcszPattern[nTableIndex] != pcszPattern[nPatternIndex + 1] &&
			nPatternIndex >= 0)
		{
			nPatternIndex = pNextTable[nPatternIndex];
		}

		if (pcszPattern[nTableIndex] == pcszPattern[nPatternIndex + 1])
			pNextTable[nTableIndex] = (char)(nPatternIndex + 1);
		else
			pNextTable[nTableIndex] = -1;
	}

	return true;
}

template <class KEY_PROCESS_FUNC>	
int KInfoGather::AnalysisKey(const char* pcszText, KEY_PROCESS_FUNC& pFunc)
{
	BOOL bRetCode = FALSE;
	int nKeyCount = 0;
	KGLOG_PROCESS_ERROR(pcszText);

	while (*pcszText)
	{
		KInfoKey* pKey = m_pInfoKeyHead;
		while (pKey)
		{
			BOOL bFinished = FALSE;
			while (!bFinished)
			{
				char cPattern = pKey->m_szKey[pKey->m_byPatternPos];
				if (pKey->m_byPatternPos == pKey->m_byEndPos)
				{
					bRetCode = pFunc(pcszText, nKeyCount, pKey);
					KG_PROCESS_ERROR(bRetCode);
					nKeyCount++;
					bFinished = TRUE;
				}
				
				if (*pcszText == cPattern || cPattern == WILDCARD_CHAR)
				{
					pKey->m_byPatternPos++;
					bFinished = TRUE;
				}
				else
				{
					pKey->m_byPatternPos = pKey->m_NextTable[pKey->m_byPatternPos] + 1;

				}

				if (pKey->m_byPatternPos == 0)
				{
					bFinished = TRUE;
				}
			}

			pKey = pKey->m_pNext;
		}
		pcszText++;
	}

Exit0:

	return nKeyCount;
}

BOOL KInfoGather::KAddInfoProcessor::operator()(const char* pcszKeyPos, int nKeyIndex, KInfoKey* pInfoKey)
{
	ASSERT(pcszKeyPos);
	ASSERT(pInfoKey);

	KInfoNode* pInfoNode = new KInfoNode;
	pInfoNode->m_pInfo = m_pInfo;
	pInfoNode->m_pNext = pInfoKey->m_InfoNode.m_pNext;
	pInfoKey->m_InfoNode.m_pNext = pInfoNode;
	m_pInfo->m_nKeyRefCount++;

	return TRUE;
}

BOOL KInfoGather::KGetInfoProcessor::operator()(const char* pcszKeyPos, int nKeyIndex, KInfoKey* pInfoKey)
{
	ASSERT(pcszKeyPos);
	ASSERT(pInfoKey);

	m_pResultList[m_nCount++] = pInfoKey;

	return TRUE;
}

struct KInfoParam
{
    KInfoParam(int nWeight) : m_nWeight(nWeight),m_nRefCount(0) {};
    int		m_nWeight;
    int		m_nRefCount;
};

const char* KInfoGather::GetSpecialInfo(const char* pcszDistrictName, 
						  const char* pcszNpcName, 
						  const char* pcszPlayerName, int nKeyCount, ...)
{
	const char* pResult = NULL;
	int nMaxWeight      = 0;
	int nTotalWeight    = 0;
    int nEqualCount     = 0;
	std::map<KInformation*, KInfoParam>				mapInfo2Count;
	std::map<KInformation*, KInfoParam>::iterator	itInfo;
	va_list vlist;

	if (nKeyCount > 0)
	{
		va_start(vlist, nKeyCount);
		const char* pcszKey = va_arg(vlist, const char*);
		MAP_STR_2_KEY::iterator itKey = m_mapStrToKey.find(pcszKey);
		KInfoKey* pInfoKey = itKey->second;
		KInfoNode* pInfoNode = pInfoKey->m_InfoNode.m_pNext;
		while (pInfoNode)
		{
			mapInfo2Count.insert(
				std::make_pair(pInfoNode->m_pInfo, pInfoKey->m_nWeight));
			nTotalWeight += pInfoKey->m_nWeight;
			pInfoNode = pInfoNode->m_pNext;
		}

		for (int nIndex = 1; nIndex < nKeyCount; nIndex++)
		{
			const char* pcszKey = va_arg(vlist, const char*);
			MAP_STR_2_KEY::iterator itKey = m_mapStrToKey.find(pcszKey);
			KInfoKey* pInfoKey = itKey->second;
			KInfoNode* pInfoNode = pInfoKey->m_InfoNode.m_pNext;
			while (pInfoNode)
			{
				itInfo = mapInfo2Count.find(pInfoNode->m_pInfo);
				if (itInfo != mapInfo2Count.end())
				{
					itInfo->second.m_nRefCount++;
				}
			}
		}
		va_end(vlist);

		itInfo = mapInfo2Count.begin();
		while (itInfo != mapInfo2Count.end())
		{
			if (itInfo->second.m_nRefCount != nKeyCount - 1)
			{
				std::map<KInformation*, KInfoParam>::iterator itTemp = itInfo;
				++itInfo;
				mapInfo2Count.erase(itTemp);
			}
		}

		KG_PROCESS_ERROR(mapInfo2Count.size() > 0);
	}

	if (pcszDistrictName)
	{
		if (mapInfo2Count.size() == 0)
		{
			for (MAP_STR_2_INFO::iterator itDistrict = 
				m_mapDistrict2Info.lower_bound(pcszDistrictName);
				itDistrict != m_mapDistrict2Info.upper_bound(pcszDistrictName); 
				++itDistrict)
			{
				mapInfo2Count.insert(std::make_pair(itDistrict->second, 100));
				nTotalWeight += 100;
			}
		}
		else
		{
			itInfo = mapInfo2Count.begin(); 
			while (itInfo != mapInfo2Count.end())
			{
				if (strncmp(itInfo->first->m_szDistrictName, pcszDistrictName, 
					_NAME_LEN) == 0)
				{
					++itInfo;
				}
				else
				{
					std::map<KInformation*, KInfoParam>::iterator itTemp = itInfo;
					++itInfo;
					mapInfo2Count.erase(itTemp);
				}
			}
		}

		KG_PROCESS_ERROR(mapInfo2Count.size() > 0);
	}

	if (pcszNpcName)
	{
		if (mapInfo2Count.size() == 0)
		{
			for (MAP_STR_2_INFO::iterator itNpc = 
				m_mapNpc2Info.lower_bound(pcszNpcName);
				itNpc != m_mapNpc2Info.upper_bound(pcszNpcName); ++itNpc)
			{
				mapInfo2Count.insert(std::make_pair(itNpc->second, 100));
				nTotalWeight += 100;
			}
		}
		else
		{
			itInfo = mapInfo2Count.begin(); 
			while (itInfo != mapInfo2Count.end())
			{
				if (strncmp(itInfo->first->m_szNpcName, pcszNpcName, 
					_NAME_LEN) == 0)
				{
					++itInfo;
				}
				else
				{
					std::map<KInformation*, KInfoParam>::iterator itTemp = itInfo;
					++itInfo;
					mapInfo2Count.erase(itTemp);
				}
			}
		}

		KG_PROCESS_ERROR(mapInfo2Count.size() > 0);
	}

	if (pcszPlayerName)
	{
		if (mapInfo2Count.size() == 0)
		{
			for (MAP_STR_2_INFO::iterator itPlayer = 
				m_mapPlayer2Info.lower_bound(pcszPlayerName);
				itPlayer != m_mapPlayer2Info.upper_bound(pcszPlayerName); ++itPlayer)
			{
				mapInfo2Count.insert(std::make_pair(itPlayer->second, 100));
				nTotalWeight += 100;
			}
		}
		else
		{
			itInfo = mapInfo2Count.begin(); 
			while (itInfo != mapInfo2Count.end())
			{
				if (strncmp(itInfo->first->m_szPlayerName, pcszPlayerName, 
					_NAME_LEN) == 0)
				{
					++itInfo;
				}
				else
				{
					std::map<KInformation*, KInfoParam>::iterator itTemp = itInfo;
					++itInfo;
					mapInfo2Count.erase(itTemp);
				}
			}
		}

		KG_PROCESS_ERROR(mapInfo2Count.size() > 0);
	}

	nEqualCount = 1;
	for (itInfo = mapInfo2Count.begin(); itInfo != mapInfo2Count.end(); ++itInfo)
	{
		if (itInfo->second.m_nWeight > nMaxWeight)
		{
			pResult = itInfo->first->m_szInfo;
			nMaxWeight = itInfo->second.m_nWeight;
			nEqualCount = 1;
		}
		else if (itInfo->second.m_nWeight == nMaxWeight)
		{
			if (g_Random(nEqualCount + 1) == 0)
			{
				pResult = itInfo->first->m_szInfo;
			}
			nEqualCount++;
		}
	}

Exit0:

	return pResult;
}
