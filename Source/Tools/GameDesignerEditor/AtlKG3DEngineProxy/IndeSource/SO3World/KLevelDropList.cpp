#include "stdafx.h"
#include "KLevelDropList.h"
#include "Global.h"

#define DROP_LEVEL				"Level"
#define DROP_ITEM_TYPE			"ItemType"
#define DROP_ITEM_INDEX			"ItemIndex"
#define DROP_RATE				"DropRate"
#define DROP_MIN				"Min"
#define DROP_MAX				"Max"
#define DROP_QUEST_ID			"QuestID"
#define DROP_QUEST_STATE		"QuestState"
#define DROP_FOR_EACH			"DropForEach"

KLevelDropList::KLevelDropList(void)
{
	m_LevelDropList = NULL;
	m_dwSize = 0;
}

KLevelDropList::~KLevelDropList(void)
{
	UnInit();
}

BOOL KLevelDropList::Init(char* pszFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int			nTempValue	= 0;
	ITabFile*   piTabFile   = NULL;

	piTabFile = g_OpenTabFile(pszFileName);
	KG_PROCESS_ERROR(piTabFile);

	m_dwSize = piTabFile->GetHeight() - 1;

	m_LevelDropList = new LEVEL_DROP_DATA[m_dwSize];
	KGLOG_PROCESS_ERROR(m_LevelDropList);

	ZeroMemory(m_LevelDropList, sizeof(LEVEL_DROP_DATA) * m_dwSize);

	for (DWORD dwIndex = 0; dwIndex < m_dwSize; dwIndex++)
	{
		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_LEVEL, 0, 
			(int*)&(m_LevelDropList[dwIndex].nLevel));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_ITEM_TYPE, 0, 
			(int*)&(m_LevelDropList[dwIndex].dwTabType));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_ITEM_INDEX, 0, 
			(int*)&(m_LevelDropList[dwIndex].dwItemIndex));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_RATE, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_LevelDropList[dwIndex].nDropRate = nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MIN, 0,&nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_LevelDropList[dwIndex].dwMin = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MAX, 0,&nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_LevelDropList[dwIndex].dwMax = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_QUEST_ID, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_LevelDropList[dwIndex].dwQuestID = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_QUEST_STATE, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue >= 0);
		m_LevelDropList[dwIndex].dwQuestState = (DWORD)nTempValue;

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_FOR_EACH, 0, &nTempValue);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nTempValue == 0 || nTempValue == 1);
		m_LevelDropList[dwIndex].bDropForEach = (BOOL)nTempValue;
		
	}


    bResult = true;
Exit0:
	if (!bResult)
	{
		KGLogPrintf(KGLOG_ERR, "[KLevelDropList] Failed to open file \"%s\" !\n", pszFileName);
	}

	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KLevelDropList::UnInit(void)
{
	KG_DELETE_ARRAY(m_LevelDropList);
	m_dwSize = 0;

	return true;
}

BOOL KLevelDropList::GetData(DWORD dwIndex, LEVEL_DROP_DATA& DropData)
{
	KGLOG_PROCESS_ERROR(dwIndex < m_dwSize);

	DropData = m_LevelDropList[dwIndex];
	
	return true;
Exit0:
	return false;
}

DWORD KLevelDropList::GetSize(void)
{
	return m_dwSize;
}

DWORD KLevelDropList::GetLevelCount(int nLevel)
{
	int nResult = 0;

	for (int nIndex = 0; nIndex < (int)m_dwSize; nIndex++)
	{
		if (m_LevelDropList[nIndex].nLevel == nLevel)
			nResult++;
	}

	return nResult;
}
