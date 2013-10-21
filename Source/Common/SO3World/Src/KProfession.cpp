#include "stdafx.h"
#include "KProfession.h"

KProfession::KProfession()
{
	m_dwID = 0;
	m_dwCraftRadarID = 0;
	ZeroMemory(m_nMaxProficiencyPerLevel,sizeof(m_nMaxProficiencyPerLevel));
	ZeroMemory(m_nPrice, sizeof(m_nPrice));
	ZeroMemory(m_szProfessionName, sizeof(m_szProfessionName));
	ZeroMemory(m_bBranchUsed, sizeof(m_bBranchUsed));
}

BOOL KProfession::LoadFromFile(const char *szFileName)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
	int     nHeight     = 0;
	int     nIndex      = 0;
	int     nExp        = 0;
	ITabFile* piProfessionFile = NULL;
	char szFullPath[MAX_PATH];

	KGLOG_PROCESS_ERROR(szFileName[0] != '\0');

	snprintf(szFullPath, sizeof(szFullPath), "%s/Craft/ExpTab/%s", SETTING_DIR, szFileName);
    szFullPath[sizeof(szFullPath) - 1] = '\0';

	piProfessionFile = g_OpenTabFile(szFullPath);
	KGLOG_PROCESS_ERROR(piProfessionFile);
	
	nHeight = piProfessionFile->GetHeight();
	KGLOG_PROCESS_ERROR((nHeight - 1) <= PROFESSION_MAX_LEVEL);

	for (nIndex = 2; nIndex <= nHeight; nIndex++)
	{
		bRetCode = piProfessionFile->GetInteger(nIndex, "Exp", 100, (int*)&nExp);
		KGLOG_PROCESS_ERROR(bRetCode);
        
        KGLOG_CHECK_ERROR((nExp < USHRT_MAX) && "Craft exp too big!\n");

		m_nMaxProficiencyPerLevel[nIndex - 1] = nExp;
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piProfessionFile);
	return bResult;
}

BOOL KProfession::LoadBranchData(const char* szFileName)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	int nHeight = 0;
	int nIndex = 0;
	DWORD dwID = 0;
	ITabFile* piTabFile = NULL;
	char szFullPath[MAX_PATH];

	KGLOG_PROCESS_ERROR(szFileName && szFileName[0] != '\0');

	snprintf(szFullPath, sizeof(szFullPath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFullPath[sizeof(szFullPath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFullPath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 0);

	for (nIndex = 2; nIndex <= nHeight; nIndex++)
	{
		bRetCode = piTabFile->GetInteger(nIndex, "ID", 0, (int*)&dwID);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(dwID > 0 && dwID <= PROFESSION_BRANCH_COUNT);

		m_bBranchUsed[dwID - 1] = true;
	}
	
	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

DWORD KProfession::GetLevelProficiency(DWORD dwLevel)
{
	KGLOG_PROCESS_ERROR(dwLevel > 0 && dwLevel <= PROFESSION_MAX_LEVEL);
	return m_nMaxProficiencyPerLevel[dwLevel];
Exit0:
	return 0;
}

