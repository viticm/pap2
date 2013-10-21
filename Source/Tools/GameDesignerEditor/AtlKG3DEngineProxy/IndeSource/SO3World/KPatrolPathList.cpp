#include "stdafx.h"
#include "KPatrolPathList.h"
#include "KPatrolPath.h"
#include "KSO3World.h"

#define PATROL_FILE_PATH		"PatrolPath/PatrolPathList.tab"

#define PATROL_PATH_ID			"PatrolPathID"
#define PATROL_PATH_WALK_SPEED  "PatrolPathWalkSpeed"
#define PATROL_PATH_ORDER_ID    "PatrolPathOrderID"
#define PATROL_PATH_FILE_NAME	"PatrolPathFileName"


KPatrolPathList::KPatrolPathList(void)
{
	
}

KPatrolPathList::~KPatrolPathList(void)
{

}

BOOL KPatrolPathList::Init()
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

BOOL KPatrolPathList::UnInit(void)
{
	m_mapPatrolPathList.clear();

	return true;
}

BOOL KPatrolPathList::Load(DWORD dwMapID)
{
    BOOL        bResult         = false;
	BOOL		bRetCode		= false;
	int			nIndex			= 0;
	int			nPatrolID		= 0;
    int         nWalkSpeed      = 0;
    int         nOrderID        = 0;
    int         nMaxSize        = 0; 
	ITabFile*   piTabFile       = NULL;
    KMapParams* pMapParams      = NULL;
	char        szFileName[MAX_PATH];
	char		szPatrolPathFileName[MAX_PATH];

    szPatrolPathFileName[0] = 0;

	pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(dwMapID);
	KGLOG_PROCESS_ERROR(pMapParams);

	snprintf(szFileName, sizeof(szFileName), "%s/map/%s/%s", SETTING_DIR, pMapParams->szMapName, PATROL_FILE_PATH);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	if (!piTabFile)
	{
		goto Exit1;
	}

	nMaxSize = piTabFile->GetHeight() - 1;
	KG_PROCESS_ERROR(nMaxSize >= 0);

	for (nIndex = 0; nIndex < nMaxSize; nIndex++)
	{
		bRetCode = piTabFile->GetInteger(nIndex + 2, PATROL_PATH_ID, 0, &nPatrolID);
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex + 2, PATROL_PATH_WALK_SPEED, 0, &nWalkSpeed);
        KGLOG_PROCESS_ERROR(bRetCode);
        KGLOG_PROCESS_ERROR(nWalkSpeed > 0);

        bRetCode = piTabFile->GetInteger(nIndex + 2, PATROL_PATH_ORDER_ID, 0, &nOrderID);
        KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetString(nIndex + 2, PATROL_PATH_FILE_NAME, "", szPatrolPathFileName, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(szPatrolPathFileName[0] != 0);

		int64_t nKey = MAKE_INT64(dwMapID, nPatrolID);

		bRetCode = m_mapPatrolPathList[nKey].Init(szPatrolPathFileName);
        if (!bRetCode)
        {
            m_mapPatrolPathList.erase(nKey);
            continue;
        }

        m_mapPatrolPathList[nKey].m_nWalkSpeed = nWalkSpeed;
        m_mapPatrolPathList[nKey].m_nOrderID = nOrderID;
	}

Exit1:
	bResult = true;
Exit0:
    if (!bResult)
    {
        if (pMapParams)
        {
            KGLogPrintf(
                KGLOG_ERR, "[KPatrolPathList] Init Failed. Map: %s!\n",
                pMapParams->szMapName
            );
        }
    }
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KPatrolPathList::Save(DWORD dwMapID, const char* szMapName)
{
    BOOL        bResult         = false;
	int         nRetCode        = false;
	int			nIndex			= 0;
	int			nPatrolID		= 0;
	ITabFile*   piTabFile       = NULL;
	char        szFileName[MAX_PATH];
	KPATROL_PATH_LIST::iterator iter;

	KGLOG_PROCESS_ERROR(szMapName);
	KGLOG_PROCESS_ERROR(szMapName[0] != '\0');

	snprintf(szFileName, sizeof(szFileName), "%s/map/%s/%s", SETTING_DIR, szMapName, PATROL_FILE_PATH);
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_CreateTabFile();
	KGLOG_PROCESS_ERROR(piTabFile);

	nRetCode = piTabFile->InsertNewCol(PATROL_PATH_ID);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = piTabFile->InsertNewCol(PATROL_PATH_WALK_SPEED);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = piTabFile->InsertNewCol(PATROL_PATH_ORDER_ID);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

	nRetCode = piTabFile->InsertNewCol(PATROL_PATH_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nIndex = 2;
	for (iter = m_mapPatrolPathList.begin(); iter != m_mapPatrolPathList.end(); iter++)
	{
		DWORD dwCurrentMapID = (DWORD)(iter->first >> 32);

		if (dwCurrentMapID != dwMapID)
			continue;

		nPatrolID = (int)(iter->first);
		nRetCode = piTabFile->WriteInteger(nIndex, PATROL_PATH_ID, nPatrolID);
		KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->WriteInteger(nIndex, PATROL_PATH_WALK_SPEED, (iter->second).m_nWalkSpeed);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->WriteInteger(nIndex, PATROL_PATH_ORDER_ID, (iter->second).m_nOrderID);
        KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = piTabFile->WriteString(nIndex, PATROL_PATH_FILE_NAME, (iter->second).m_szTabFileName);
		KGLOG_PROCESS_ERROR(nRetCode);

		nIndex++;

		iter->second.Save();
	}

	nRetCode = piTabFile->Save(szFileName);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

KPatrolPath* KPatrolPathList::AddPatrolPath(DWORD dwMapID, DWORD dwPatrolPathID, char* szTabFileName)
{
	BOOL            bRetCode = false;
    KPatrolPath*    pPatrolPath = NULL;
	ITabFile*       piTabFile = NULL;
    int64_t         nKey    = 0;
	char            szFullPath[MAX_PATH];

	KGLOG_PROCESS_ERROR(szTabFileName[0] != 0);

	nKey = MAKE_INT64(dwMapID, dwPatrolPathID);

	bRetCode = m_mapPatrolPathList[nKey].Init(NULL);
	KGLOG_PROCESS_ERROR(bRetCode);

    m_mapPatrolPathList[nKey].m_nWalkSpeed = 6;
    m_mapPatrolPathList[nKey].m_nOrderID = 1;

	strcpy(m_mapPatrolPathList[nKey].m_szTabFileName, szTabFileName);

	piTabFile = g_CreateTabFile();
	KGLOG_PROCESS_ERROR(piTabFile);

	snprintf(szFullPath, sizeof(szFullPath), "%s/%s", SETTING_DIR, szTabFileName);
    szFullPath[sizeof(szFullPath) - 1] = '\0';

	piTabFile->Save(szFullPath);

	pPatrolPath = &m_mapPatrolPathList[nKey];
Exit0:
	KG_COM_RELEASE(piTabFile);
	return pPatrolPath;
}

KPatrolPath* KPatrolPathList::GetPatrolPath(DWORD dwMapID, DWORD dwPatrolPathID)
{
	KPATROL_PATH_LIST::iterator iter;

	int64_t nKey = MAKE_INT64(dwMapID, dwPatrolPathID);

	iter = m_mapPatrolPathList.find(nKey);
	KGLOG_PROCESS_ERROR(iter != m_mapPatrolPathList.end());

	return &(iter->second);
Exit0:
	return NULL;
}

int KPatrolPathList::GetSize()
{
	int nResult = (int)m_mapPatrolPathList.size();

	return nResult;
}

