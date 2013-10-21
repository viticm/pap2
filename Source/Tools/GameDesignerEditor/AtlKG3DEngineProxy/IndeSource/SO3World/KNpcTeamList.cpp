#include "stdafx.h"
#include "KNpcTeamList.h"
#include "KSO3World.h"

#define NPC_TEAM_FILE_PATH	"NpcTeam/NpcTeamList.tab"

#define NPC_TEAM_ID			"TeamID"
#define NPC_TEAM_ORDER		"TeamOrder"

KNpcTeamList::KNpcTeamList()
{

}

KNpcTeamList::~KNpcTeamList()
{
	
}

BOOL KNpcTeamList::Init()
{
    BOOL                bResult  = false;
    KMAP_PARAM_TABLE    MapList  = g_pSO3World->m_Settings.m_MapListFile.GetMapList();

#ifdef _SERVER
    for (KMAP_PARAM_TABLE::iterator it = MapList.begin(); it != MapList.end(); ++it)
	{
		Load(it->first);
	}
#endif 

	bResult = true;
Exit0:
	return bResult;
}

BOOL KNpcTeamList::UnInit()
{
	m_mapNpcTeamList.clear();
	return true;
}

BOOL KNpcTeamList::Load(DWORD dwMapID)
{
    BOOL        bResult         = false;
	BOOL        bRetCode        = false;
	int			nIndex			= 0;
	int			nTeamID			= 0;
	int			nTeamOrder		= 0;
    int         nMaxSize        = 0; 
	ITabFile*   piTabFile       = NULL;
    KMapParams* pMapParams      = NULL;
	KNpcTeamTemplate NpcTeamTemplate;
	char        szFileName[MAX_PATH];
    

	pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(dwMapID);
	KGLOG_PROCESS_ERROR(pMapParams);
	KGLOG_PROCESS_ERROR(pMapParams->szMapName[0] != '\0');

	snprintf(szFileName, sizeof(szFileName), "%s/map/%s/%s", SETTING_DIR, pMapParams->szMapName, NPC_TEAM_FILE_PATH);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	if (!piTabFile)
	{
		//KGLogPrintf(KGLOG_DEBUG, "[KNpcTeamList] Failed to open tab file \"%s\" !\n", szFileName);
		goto Exit0;
	}

	nMaxSize = piTabFile->GetHeight() - 1;
	KG_PROCESS_ERROR(nMaxSize > 0);

	for (nIndex = 0; nIndex < nMaxSize; nIndex++)
	{
		bRetCode = piTabFile->GetInteger(nIndex + 2, NPC_TEAM_ID, 0, (int*)&NpcTeamTemplate.dwID);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(nIndex + 2, NPC_TEAM_ORDER, 0, (int*)&NpcTeamTemplate.nOrder);
		KGLOG_PROCESS_ERROR(bRetCode);

		int64_t nKey = MAKE_INT64(dwMapID, NpcTeamTemplate.dwID);
		NpcTeamTemplate.dwMapID = dwMapID;

		m_mapNpcTeamList[nKey] = NpcTeamTemplate;
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}	

BOOL KNpcTeamList::Save(DWORD dwMapID, const char* szMapName)
{
    BOOL        bResult         = false;
	BOOL        bRetCode        = false;
	int			nIndex			= 0;
	int			nTeamID			= 0;
	ITabFile*   piTabFile       = NULL;
	char        szFileName[MAX_PATH];
	KNPC_TEAM_TEMPLATE_LIST::iterator iter;

	KGLOG_PROCESS_ERROR(szMapName);
	KGLOG_PROCESS_ERROR(szMapName[0] != '\0');

	snprintf(szFileName, sizeof(szFileName), "%s/map/%s/%s", SETTING_DIR, szMapName, NPC_TEAM_FILE_PATH);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_CreateTabFile();
	KGLOG_PROCESS_ERROR(piTabFile);

	bRetCode = piTabFile->InsertNewCol(NPC_TEAM_ID);
	KGLOG_PROCESS_ERROR(bRetCode != -1);

	bRetCode = piTabFile->InsertNewCol(NPC_TEAM_ORDER);
	KGLOG_PROCESS_ERROR(bRetCode != -1);
		
	nIndex = 2;
	for (iter = m_mapNpcTeamList.begin(); iter != m_mapNpcTeamList.end(); iter++)
	{
		DWORD dwCurrentMapID = (iter->second).dwMapID;

		if (dwCurrentMapID != dwMapID)
			continue;

		nTeamID = (int)(iter->first);
		bRetCode = piTabFile->WriteInteger(nIndex, NPC_TEAM_ID, (iter->second).dwID);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->WriteInteger(nIndex, NPC_TEAM_ORDER, (iter->second).nOrder);
		KGLOG_PROCESS_ERROR(bRetCode);

		nIndex++;
	}

	bRetCode = piTabFile->Save(szFileName);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KNpcTeamList::AddNpcTeam(DWORD dwMapId, DWORD dwTeamID, DWORD dwTeamOrder)
{
    BOOL                bResult     = false;
    int64_t             nKey        = 0;
    KNpcTeamTemplate    NpcTeamTemplate;

	NpcTeamTemplate.dwID = dwTeamID;
	NpcTeamTemplate.dwMapID = dwMapId;
	NpcTeamTemplate.nOrder = (int)dwTeamOrder;

	nKey = MAKE_INT64(dwMapId, dwTeamID);

	m_mapNpcTeamList[nKey] = NpcTeamTemplate;

	bResult = true;
Exit0:
	return bResult;
}

KNpcTeamTemplate* KNpcTeamList::GetNpcTeamTemplate(DWORD dwMapID, DWORD dwTeamID)
{
	KNpcTeamTemplate*   pResult = 0;
    int64_t             nKey    = 0;
	KNPC_TEAM_TEMPLATE_LIST::iterator iter;
    
	KGLOG_PROCESS_ERROR(dwMapID > 0);
	KGLOG_PROCESS_ERROR(dwTeamID > 0);
	
	nKey = MAKE_INT64(dwMapID, dwTeamID);

	iter = m_mapNpcTeamList.find(nKey);
	if (iter != m_mapNpcTeamList.end())
	{
		pResult = &(iter->second);
	}
Exit0:
	return pResult;
}

BOOL KNpcTeamList::GetNpcTeamTemplate(DWORD dwMapID, OUT std::vector<KNpcTeamTemplate>& vecNpcTeamArray)
{
	BOOL bResult = false;
	KNPC_TEAM_TEMPLATE_LIST::iterator iter;
	KNpcTeamTemplate NpcTeamTemplate;
	vecNpcTeamArray.clear();

	for (iter = m_mapNpcTeamList.begin(); iter != m_mapNpcTeamList.end(); iter++)
	{
		
		if ((iter->second).dwMapID == dwMapID)
		{
			NpcTeamTemplate = iter->second;
			vecNpcTeamArray.push_back(NpcTeamTemplate);
		}
	}

	bResult = true;
Exit0:
	return bResult;
}

