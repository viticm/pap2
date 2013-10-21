#include "stdafx.h"
#include "KMapFile.h"

#include "KSO3World.h"

#define MAP_BASE_INFO_SECTION	"BaseInfo"
#define MAP_CELL_TOTAL			"CellTotal"
#define MAP_REGION_WIDTH		"RegionWidth"
#define MAP_REGION_HEIGHT		"RegionHeigth"
#define MAP_SERVER_NPC_TOTAL	"ServerNpcTotal"
#define MAP_CLIENT_NPC_TOTAL	"ClientNpcTotal"
#define MAP_SERVER_DOODAD_TOTAL	"ServerDoodadTotal"
#define MAP_CLIENT_DOODAD_TOTAL	"ClientDoodadTotal"

KMapFile::KMapFile(void)
{
	m_pIniFile = NULL;
}

KMapFile::~KMapFile(void)
{
	KG_COM_RELEASE(m_pIniFile);
}

//读取相关
BOOL KMapFile::Open(char* pszMapName)
{
    char szFileName[MAX_PATH];

	KGLOG_PROCESS_ERROR(m_pIniFile == NULL);

	sprintf(szFileName, "%s/%s/%s.cfg", MAP_DIR, pszMapName, pszMapName);

	m_pIniFile = g_OpenIniFile(szFileName);
	if (!m_pIniFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KMapFile] Failed to open file: %s\n", szFileName);
        goto Exit0;
    }

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KMapFile::GetRegionCount(int& rnRegionWidth, int& rnRegionHeight)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_REGION_WIDTH, 0, &rnRegionWidth);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_REGION_HEIGHT, 0, &rnRegionHeight);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::GetCellTotal(DWORD& rdwCellTotal)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_CELL_TOTAL, 
		0, (int*)&rdwCellTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::GetNpcTotal(DWORD& rdwServerNpcTotal, DWORD& rdwClientNpcTotal)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_SERVER_NPC_TOTAL, 
		0, (int*)&rdwServerNpcTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_CLIENT_NPC_TOTAL, 
		0, (int*)&rdwClientNpcTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::GetDoodadTotal(DWORD& rdwServerDoodadTotal, DWORD& rdwClientDoodadTotal)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_SERVER_DOODAD_TOTAL, 
		0, (int*)&rdwServerDoodadTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pIniFile->GetInteger(MAP_BASE_INFO_SECTION, MAP_CLIENT_DOODAD_TOTAL, 
		0, (int*)&rdwClientDoodadTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::Close(void)
{
	KG_COM_RELEASE(m_pIniFile);

	return TRUE;
}

//写入相关，只有编辑器用
#ifdef _EDITOR
BOOL KMapFile::Create(void)
{
	KGLOG_PROCESS_ERROR(m_pIniFile == NULL);

	m_pIniFile = g_CreateIniFile();
	KGLOG_PROCESS_ERROR(m_pIniFile);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::SetRegionCount(DWORD dwRegionWidth, DWORD dwRegionHeight)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, 
		MAP_REGION_WIDTH, dwRegionWidth);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, 
		MAP_REGION_HEIGHT, dwRegionHeight);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::SetCellTotal(DWORD dwCellTotal)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, MAP_CELL_TOTAL, dwCellTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::SetNpcTotal(DWORD dwServerNpcTotal, DWORD dwClientNpcTotal)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, 
		MAP_SERVER_NPC_TOTAL, dwServerNpcTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, 
		MAP_CLIENT_NPC_TOTAL, dwClientNpcTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::SetDoodadTotal(DWORD dwServerDoodadTotal, DWORD dwClientDoodadTotal)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, 
		MAP_SERVER_DOODAD_TOTAL, dwServerDoodadTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pIniFile->WriteInteger(MAP_BASE_INFO_SECTION, 
		MAP_CLIENT_DOODAD_TOTAL, dwClientDoodadTotal);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KMapFile::Save(char* pszMapName)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pIniFile);

	char szFileName[MAX_PATH];
	sprintf(szFileName, "%s/%s/%s.cfg", MAP_DIR, pszMapName, pszMapName);

	bRetCode = m_pIniFile->Save(szFileName);
	KGLOG_PROCESS_ERROR(bRetCode);

	KG_COM_RELEASE(m_pIniFile);

	return TRUE;

Exit0:
	KG_COM_RELEASE(m_pIniFile);

	return FALSE;
}

#endif	//EDITOR
