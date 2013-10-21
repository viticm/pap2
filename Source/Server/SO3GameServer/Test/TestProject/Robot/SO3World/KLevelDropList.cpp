#include "stdafx.h"
#include "KLevelDropList.h"
#include "Global.h"

#define DROP_LEVEL			"Level"
#define DROP_ITEM_TYPE		"ItemType"
#define DROP_ITEM_INDEX		"ItemIndex"
#define DROP_RATE			"DropRate"
#define DROP_MIN			"Min"
#define DROP_MAX			"Max"

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
	BOOL        bRetCode    = FALSE;
	ITabFile*   piTabFile   = NULL;

	piTabFile = g_OpenTabFile(pszFileName);
	if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KLevelDropList] Failed to open file \"%s\" !\n", pszFileName);
        goto Exit0;
    }

	m_dwSize = piTabFile->GetHeight() - 1;

	m_LevelDropList = new LEVEL_DROP[m_dwSize];
	KGLOG_PROCESS_ERROR(m_LevelDropList);

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

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_RATE, 0, 
			(int*)&(m_LevelDropList[dwIndex].nDropRate));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MIN, 0,
			(int*)&(m_LevelDropList[dwIndex].dwMin));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MAX, 0,
			(int*)&(m_LevelDropList[dwIndex].dwMax));
		KGLOG_PROCESS_ERROR(bRetCode);
	}


    bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KLevelDropList::UnInit(void)
{
	KG_DELETE_ARRAY(m_LevelDropList);
	m_dwSize = 0;

	return TRUE;
}

BOOL KLevelDropList::GetData(DWORD dwIndex, int& rnLevel, DWORD& rdwTabType, DWORD& rdwItemIndex, int& rnDropRate,
							 DWORD& rdwMin, DWORD& rdwMax)
{
	KGLOG_PROCESS_ERROR(dwIndex < m_dwSize);

	rnLevel = m_LevelDropList[dwIndex].nLevel;
	rdwTabType = m_LevelDropList[dwIndex].dwTabType;
	rdwItemIndex = m_LevelDropList[dwIndex].dwItemIndex;
	rnDropRate = m_LevelDropList[dwIndex].nDropRate;
	rdwMin = m_LevelDropList[dwIndex].dwMin;
	rdwMax = m_LevelDropList[dwIndex].dwMax;

	return TRUE;

Exit0:

	return FALSE;
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
