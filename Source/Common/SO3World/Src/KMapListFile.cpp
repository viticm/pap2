#include "stdafx.h"
#include "KMapListFile.h"
#include "KSO3World.h"

BOOL KMapListFile::Init(void)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	ITabFile*	piTabFile   = NULL;
	char        szFilePath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, MAP_LIST_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	for (int nRowIndex = 2; nRowIndex <= piTabFile->GetHeight(); nRowIndex++)
	{
        KMapParams  MapParam;
        memset(&MapParam, 0, sizeof(MapParam));

		bRetCode = piTabFile->GetInteger(nRowIndex, "ID", 0, (int*)&MapParam.dwMapID);
		KGLOG_PROCESS_ERROR(bRetCode && MapParam.dwMapID <= MAX_MAP_ID);

		bRetCode = piTabFile->GetString(nRowIndex, "Name", "", MapParam.szMapName, sizeof(MapParam.szMapName));
		KGLOG_PROCESS_ERROR(bRetCode && MapParam.szMapName[0] != '\0');

        bRetCode = piTabFile->GetInteger(nRowIndex, "Type", 1, &MapParam.nType);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "AllScenePlayerInFight", 0, &MapParam.bAllScenePlayerInFight);
        KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(nRowIndex, "Broadcast", 1, &MapParam.nBroadcast);
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "bCanPK", 1, &MapParam.bCanPK);
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "CampType", emctAllProtect, &MapParam.nCampType);
        KGLOG_PROCESS_ERROR(bRetCode);
        KGLOG_PROCESS_ERROR(MapParam.nCampType > emctInvalid && MapParam.nCampType < emctTotal);

		bRetCode = piTabFile->GetString(nRowIndex, "MapDrop", "", MapParam.szDropName, sizeof(MapParam.szDropName));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetString(nRowIndex, "ResourcePath", "", MapParam.szResourceFilePath, sizeof(MapParam.szResourceFilePath));
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "ReviveInSitu", 0, (int*)&MapParam.bReviveInSitu);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "MaxPlayerCount", 0, (int*)&MapParam.nMaxPlayerCount);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "BanSkillMask", 0, (int*)&MapParam.dwBanSkillMask);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "BattleRelationMask", 0, (int*)&MapParam.dwBattleRelationMask);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "DoNotGoThroughRoof", 0, (int*)&MapParam.bDoNotGoThroughRoof);
        KGLOG_PROCESS_ERROR(bRetCode);
        
        bRetCode = piTabFile->GetInteger(nRowIndex, "RefreshCycle", 0, (int*)&MapParam.nRefreshCycle);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nRowIndex, "QuestCountAchID", -1, &MapParam.nQuestCountAchID);
        KGLOG_PROCESS_ERROR(bRetCode);
        
        bRetCode = piTabFile->GetInteger(nRowIndex, "LimitTimes", 0, &MapParam.nLimitTimes);
        KGLOG_PROCESS_ERROR(bRetCode);
        
        MapParam.nRefreshCycle *= 60;

        m_MapParamTable[MapParam.dwMapID] = MapParam;
	}

    bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

KMapParams*	KMapListFile::GetMapParamByID(DWORD dwMapID)
{
    KMAP_PARAM_TABLE::iterator it = m_MapParamTable.find(dwMapID);

    if (it != m_MapParamTable.end())
    {
        return &it->second;
    }

	return NULL;
}

KMapParams*	KMapListFile::GetMapParamByName(const char cszName[])
{
    int                        nRetCode = 0;
    KMAP_PARAM_TABLE::iterator it       = m_MapParamTable.begin();
    
    while (it != m_MapParamTable.end())
    {
        nRetCode = strcmp(it->second.szMapName, cszName);
        if (nRetCode == 0)
        {
            return &it->second;
        }
        ++it;
    }

    return NULL;
}

