#include "stdafx.h"
#include "KLevelMoneyDropList.h"
#include "Global.h"

#define DROP_LEVEL			"Level"
#define DROP_CLASS_ID		"ClassID"
#define DROP_MIN_MONEY		"MoneyMin"
#define DROP_MAX_MONEY		"MoneyMax"

KLevelMoneyDropList::KLevelMoneyDropList(void)
{
	m_LevelMoneyDropList = NULL;
	m_dwSize = 0;
}

KLevelMoneyDropList::~KLevelMoneyDropList(void)
{
	UnInit();
}

BOOL KLevelMoneyDropList::Init(char* pszFileName)
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

	m_LevelMoneyDropList = new LEVEL_MONEY_DROP[m_dwSize];
	KGLOG_PROCESS_ERROR(m_LevelMoneyDropList);

	for (DWORD dwIndex = 0; dwIndex < m_dwSize; dwIndex++)
	{
		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_LEVEL, 0, 
			(int*)&(m_LevelMoneyDropList[dwIndex].nLevel));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_CLASS_ID, 0,
			(int*)&(m_LevelMoneyDropList[dwIndex].nClassID));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MIN_MONEY, 0, 
			(int*)&(m_LevelMoneyDropList[dwIndex].nMinMoney));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MAX_MONEY, 0, 
			(int*)&(m_LevelMoneyDropList[dwIndex].nMaxMoney));
		KGLOG_PROCESS_ERROR(bRetCode);
	}


	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KLevelMoneyDropList::UnInit(void)
{
	KG_DELETE_ARRAY(m_LevelMoneyDropList);
	m_dwSize = 0;

	return TRUE;
}

BOOL KLevelMoneyDropList::GetData(DWORD dwIndex, int& rnLevel, int& rnClassID, int& rnMinMoney, int& rnMaxMoney)
{
	KGLOG_PROCESS_ERROR(dwIndex < m_dwSize);

	rnLevel = m_LevelMoneyDropList[dwIndex].nLevel;
	rnClassID = m_LevelMoneyDropList[dwIndex].nClassID;
	rnMinMoney = m_LevelMoneyDropList[dwIndex].nMinMoney;
	rnMaxMoney = m_LevelMoneyDropList[dwIndex].nMaxMoney;

	return TRUE;

Exit0:

	return FALSE;
}

DWORD KLevelMoneyDropList::GetSize(void)
{
	return m_dwSize;
}
