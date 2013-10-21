#include "stdafx.h"
#include "KDoodadClassList.h"

#define DOODAD_CLASS_FILE_NAME	"DoodadClass.tab"

#define DOODAD_CLASS_ID					"ClassID"
#define DOODAD_CLASS_NAME				"ClassName"

#define DOODAD_CLASS_DROP_NAME1			"Drop1"
#define DOODAD_CLASS_DROP_COUNT1		"Count1"

#define DOODAD_CLASS_DROP_NAME2			"Drop2"
#define DOODAD_CLASS_DROP_COUNT2		"Count2"

#define DOODAD_CLASS_DROP_NAME3			"Drop3"
#define DOODAD_CLASS_DROP_COUNT3		"Count3"

#define DOODAD_CLASS_DROP_NAME4			"Drop4"
#define DOODAD_CLASS_DROP_COUNT4		"Count4"


KDoodadClassList::KDoodadClassList(void)
{
	m_DoodadClassList = NULL;
	m_dwMaxClassID = 0;
}

KDoodadClassList::~KDoodadClassList(void)
{
	if (m_DoodadClassList != NULL)
		UnInit();
}

BOOL KDoodadClassList::Init(void)
{
	BOOL bRetCode		= false;
	ITabFile *pTabFile	= NULL;
	char szFileName[MAX_PATH];

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, DOODAD_CLASS_FILE_NAME);
    szFileName[sizeof(szFileName) - 1] = '\0';

	pTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(pTabFile);

	//获取最大ID
	m_dwMaxClassID = 0;
	for (int nRowIndex = 2; nRowIndex <= pTabFile->GetHeight(); nRowIndex++)
	{
		DWORD dwClassID = 0;
		bRetCode = pTabFile->GetInteger(nRowIndex, DOODAD_CLASS_ID, 0, (int*)&dwClassID);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (dwClassID > m_dwMaxClassID)
			m_dwMaxClassID = dwClassID;
	}

	m_DoodadClassList = new KDoodadClass[m_dwMaxClassID + 1];
	KGLOG_PROCESS_ERROR(m_DoodadClassList);
	memset(m_DoodadClassList, 0, sizeof(KDoodadClass) * (m_dwMaxClassID + 1));
	
	for (int nRowIndex = 2; nRowIndex <= pTabFile->GetHeight(); nRowIndex++)
	{
		DWORD dwClassID = 0;

		bRetCode = pTabFile->GetInteger(nRowIndex, DOODAD_CLASS_ID, 0, (int*)&dwClassID);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, DOODAD_CLASS_NAME, "", 
						m_DoodadClassList[dwClassID].szName, _NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, DOODAD_CLASS_DROP_NAME1, "",
						m_DoodadClassList[dwClassID].szDropName[0], MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nRowIndex, DOODAD_CLASS_DROP_COUNT1, 0,
						(int*)&m_DoodadClassList[dwClassID].dwDropCount[0]);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, DOODAD_CLASS_DROP_NAME2, "",
			m_DoodadClassList[dwClassID].szDropName[1], MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nRowIndex, DOODAD_CLASS_DROP_COUNT2, 0,
			(int*)&m_DoodadClassList[dwClassID].dwDropCount[1]);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, DOODAD_CLASS_DROP_NAME3, "",
			m_DoodadClassList[dwClassID].szDropName[2], MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nRowIndex, DOODAD_CLASS_DROP_COUNT3, 0,
			(int*)&m_DoodadClassList[dwClassID].dwDropCount[2]);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nRowIndex, DOODAD_CLASS_DROP_NAME4, "",
			m_DoodadClassList[dwClassID].szDropName[3], MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nRowIndex, DOODAD_CLASS_DROP_COUNT4, 0,
			(int*)&m_DoodadClassList[dwClassID].dwDropCount[3]);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	KG_COM_RELEASE(pTabFile);

	return true;
Exit0:
	KG_COM_RELEASE(pTabFile);

	return false;
}

BOOL KDoodadClassList::UnInit(void)
{
	KG_DELETE_ARRAY(m_DoodadClassList);
	m_dwMaxClassID = 0;

	return true;
}

KDoodadClass* KDoodadClassList::GetData(DWORD dwClassID)
{
	KGLOG_PROCESS_ERROR(dwClassID <= m_dwMaxClassID);

	return &(m_DoodadClassList[dwClassID]);
Exit0:
	return NULL;
}

DWORD KDoodadClassList::GetMaxClassID(void)
{
	return m_dwMaxClassID;
}

