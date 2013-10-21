#include "stdafx.h"
#include "KIndividualDropList.h"
#include "Global.h"

#define DROP_ITEM_TYPE		"ItemType"
#define DROP_ITEM_INDEX		"ItemIndex"
#define DROP_RATE			"DropRate"
#define DROP_MIN			"Min"
#define DROP_MAX			"Max"

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
	BOOL        bRetCode    = FALSE;
	ITabFile*   piTabFile   = NULL;

	piTabFile = g_OpenTabFile(pszFileName);

    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KIndividualDropList] Failed to open file \"%s\" !\n", pszFileName);
        goto Exit0;
    }

	m_dwSize = piTabFile->GetHeight() - 1;

	m_IndividualDropList = new INDIVIDUAL_DROP[m_dwSize];
	KGLOG_PROCESS_ERROR(m_IndividualDropList);

	for (DWORD dwIndex = 0; dwIndex < m_dwSize; dwIndex++)
	{
		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_ITEM_TYPE, 0, 
			(int*)&(m_IndividualDropList[dwIndex].dwTabType));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_ITEM_INDEX, 0, 
			(int*)&(m_IndividualDropList[dwIndex].dwItemIndex));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_RATE, 0, 
			&(m_IndividualDropList[dwIndex].nDropRate));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MIN, 0,
			(int*)&(m_IndividualDropList[dwIndex].dwMin));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(dwIndex + 2, DROP_MAX, 0,
			(int*)&(m_IndividualDropList[dwIndex].dwMax));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

    bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KIndividualDropList::UnInit(void)
{
	KG_DELETE_ARRAY(m_IndividualDropList);
	m_dwSize = 0;

	return TRUE;
}

DWORD KIndividualDropList::GetSize(void)
{
	return m_dwSize;
}

BOOL KIndividualDropList::GetData(int nIndex, DWORD& rdwTabType, DWORD& rdwItemIndex, int& fnDropRate, 
								  DWORD& rdwMin, DWORD& rdwMax)
{
	KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_dwSize);

	rdwTabType = m_IndividualDropList[nIndex].dwTabType;
	rdwItemIndex = m_IndividualDropList[nIndex].dwItemIndex;
	fnDropRate = m_IndividualDropList[nIndex].nDropRate;
	rdwMin = m_IndividualDropList[nIndex].dwMin;
	rdwMax = m_IndividualDropList[nIndex].dwMax;

	return TRUE;

Exit0:

	return FALSE;
}
