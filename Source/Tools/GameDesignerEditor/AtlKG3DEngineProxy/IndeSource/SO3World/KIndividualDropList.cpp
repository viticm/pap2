#include "stdafx.h"
#include "KIndividualDropList.h"
#include "Global.h"

#define DROP_ITEM_TYPE			"ItemType"
#define DROP_ITEM_INDEX			"ItemIndex"
#define DROP_RATE				"DropRate"
#define DROP_MIN				"Min"
#define DROP_MAX				"Max"
#define DROP_QUEST_ID			"QuestID"
#define DROP_QUEST_STATE		"QuestState"
#define DROP_FOR_EACH			"DropForEach"

KIndividualDropList::KIndividualDropList(void)
{
	m_IndividualDropList = NULL;
	m_dwSize = 0;
}

KIndividualDropList::~KIndividualDropList(void)
{
	UnInit();
}

BOOL KIndividualDropList::Init(char* pszFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int			nTempValue	= 0;
	ITabFile*   piTabFile   = NULL;

	piTabFile = g_OpenTabFile(pszFileName);
	KG_PROCESS_ERROR(piTabFile);

	m_dwSize = piTabFile->GetHeight() - 1;

	m_IndividualDropList = new DROP_DATA[m_dwSize];
	KGLOG_PROCESS_ERROR(m_IndividualDropList);
	
	ZeroMemory(m_IndividualDropList, sizeof(DROP_DATA) * m_dwSize);

	for (DWORD dwIndex = 0; dwIndex < m_dwSize; dwIndex++)
	{
		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_ITEM_TYPE, 0, 
			(int*)&(m_IndividualDropList[dwIndex].dwTabType));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_ITEM_INDEX, 0, 
			(int*)&(m_IndividualDropList[dwIndex].dwItemIndex));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_RATE, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_IndividualDropList[dwIndex].nDropRate = nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MIN, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_IndividualDropList[dwIndex].dwMin = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MAX, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_IndividualDropList[dwIndex].dwMax = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_QUEST_ID, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_IndividualDropList[dwIndex].dwQuestID = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_QUEST_STATE, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_IndividualDropList[dwIndex].dwQuestState = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_FOR_EACH, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue == 0 || nTempValue == 1);
		m_IndividualDropList[dwIndex].bDropForEach = (BOOL)nTempValue;
	}

    bResult = true;
Exit0:
	if (!bResult)
	{
		KGLogPrintf(KGLOG_ERR, "[KIndividualDropList] Failed to open file \"%s\" !\n", pszFileName);
	}

	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KIndividualDropList::UnInit(void)
{
	KG_DELETE_ARRAY(m_IndividualDropList);
	m_dwSize = 0;

	return true;
}

DWORD KIndividualDropList::GetSize(void)
{
	return m_dwSize;
}

BOOL KIndividualDropList::GetData(int nIndex, DROP_DATA& DropData)
{
	KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_dwSize);

	DropData = m_IndividualDropList[nIndex];
	
	return true;
Exit0:
	return false;
}
