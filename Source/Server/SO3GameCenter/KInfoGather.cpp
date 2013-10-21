#include "StdAfx.h"
#include "KInfoGather.h"

#define WILDCARD_CHAR	'?'

KInfoGather::KInfoGather(void)
{
	m_pInfoKeyHead = NULL;
}

KInfoGather::~KInfoGather(void)
{
}

BOOL KInfoGather::AddInfo(const char* pcszInfo)
{
	KGLOG_PROCESS_ERROR(pcszInfo);

	KInformation* pInfo = (KInformation*)new char[sizeof(KInformation) + strlen(pcszInfo) + 1];
	KGLOG_PROCESS_ERROR(pInfo);

	pInfo->m_nAccessCount = 0;
	pInfo->m_nKeyRefCount = 0;
	pInfo->m_nInfoLen = strlen(pcszInfo) + 1;
	strcpy(pInfo->m_szInfo, pcszInfo);

	KAddInfoProcessor AddInfoProcessor;
	AddInfoProcessor.m_pInfo = pInfo;
	AnalysisKey(pcszInfo, AddInfoProcessor);

	if (pInfo->m_nKeyRefCount == 0)
	{
		delete pInfo;
	}

	return TRUE;

Exit0:

	return FALSE;
}

const char* KInfoGather::GetInfo(const char* pcszExample)
{
	const char* pResult = NULL;
	std::map<KInformation*, int> InfoList;
	std::map<KInformation*, int>::iterator it;

	KGLOG_PROCESS_ERROR(pcszExample);

	KGetInfoProcessor GetInfoProcessor;
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

	KInformation* pInfo = NULL;
	int nMaxWeight = 0;
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
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pcszKey);

	KInfoKey* pInfoKey = new KInfoKey;
	KGLOG_PROCESS_ERROR(pInfoKey);

	memset(pInfoKey, 0, sizeof(KInfoKey));
	pInfoKey->m_nWeight = nWeight;
	pInfoKey->m_byEndPos = strlen(pcszKey) - 1;
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
			pNextTable[nTableIndex] = nPatternIndex + 1;
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

	KInfoNode* pInfoNode = &(pInfoKey->m_InfoNode);
	while (pInfoNode->m_pNext != NULL)
		pInfoNode = pInfoNode->m_pNext;

	pInfoNode->m_pNext = new KInfoNode;
	pInfoNode->m_pNext->m_pInfo = m_pInfo;
	pInfoNode->m_pNext->m_pNext = NULL;
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
