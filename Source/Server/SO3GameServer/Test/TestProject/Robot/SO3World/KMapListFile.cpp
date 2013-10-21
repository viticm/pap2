#include "stdafx.h"
#include "KMapListFile.h"
#include "KSO3World.h"
#include "KMapFile.h"

KMapListFile::KMapListFile(void)
{
	memset(m_MapParamsList, 0, sizeof(m_MapParamsList));
}

KMapListFile::~KMapListFile(void)
{
}

BOOL KMapListFile::Init(void)
{
	BOOL bRetCode = FALSE;

	ITabFile*	pTabFile = NULL;

	char szFilePath[MAX_PATH];

	sprintf(szFilePath, "%s/%s", SETTING_DIR, MAP_LIST_FILE_NAME);
	pTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(pTabFile);

	for (int nRowIndex = 2; nRowIndex <= pTabFile->GetHeight(); nRowIndex++)
	{
		DWORD dwMapID = 0;
		
		bRetCode = pTabFile->GetInteger(nRowIndex, MAP_MAP_ID, 0, (int*)&dwMapID);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(dwMapID > 0 && dwMapID < MAX_MAP_INDEX);
		m_MapParamsList[dwMapID].dwMapID = dwMapID;

		bRetCode = pTabFile->GetString(nRowIndex, MAP_DROP_NAME, "", 
			m_MapParamsList[dwMapID].szDropName, _NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, MAP_MAP_NAME, "", 
			m_MapParamsList[dwMapID].szMapName, _NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);
		
		KMapFile MapFile;

		bRetCode = MapFile.Open(m_MapParamsList[dwMapID].szMapName);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = MapFile.GetRegionCount(m_MapParamsList[dwMapID].nRegionWidth, m_MapParamsList[dwMapID].nRegionHeight);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = MapFile.GetCellTotal(m_MapParamsList[dwMapID].dwCellTotal);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = MapFile.GetNpcTotal(
			m_MapParamsList[dwMapID].dwServerNpcTotal,
			m_MapParamsList[dwMapID].dwClientNpcTotal);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = MapFile.GetDoodadTotal(
			m_MapParamsList[dwMapID].dwServerDoodadTotal,
			m_MapParamsList[dwMapID].dwClientDoodadTotal);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, MAP_RESOURCE_FILE_PATH, "", 
			m_MapParamsList[dwMapID].szResourceFilePath, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = pTabFile->GetInteger(nRowIndex, MAP_REVIVE_IN_SITU, 0, 
            (int*)&m_MapParamsList[dwMapID].bReviveInSitu);
        KGLOG_PROCESS_ERROR(bRetCode);
		MapFile.Close();
	}

	KG_COM_RELEASE(pTabFile);

	return TRUE;

Exit0:

	KG_COM_RELEASE(pTabFile);

	return FALSE;
}

KMapParams*	KMapListFile::GetData(DWORD dwMapID)
{
	KGLOG_PROCESS_ERROR(dwMapID < MAX_MAP_INDEX);

	return &(m_MapParamsList[dwMapID]);

Exit0:

	return NULL;
}
