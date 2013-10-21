#include "stdafx.h"
#include "KNpcClassList.h"

#define NPC_CLASS_FILE_NAME	"NpcClass.tab"

#define NPC_CLASS_ID		"ClassID"
#define NPC_CLASS_NAME		"ClassName"
#define NPC_CLASS_DROP_NAME	"ClassDrop"
#define NPC_PARENT_CLASS_ID	"ParentClassID"

KNpcClassList::KNpcClassList(void)
{
	m_NpcClassList = NULL;
	m_dwMaxClassID = 0;
}

KNpcClassList::~KNpcClassList(void)
{
	if(m_NpcClassList != NULL)
		UnInit();
}

BOOL KNpcClassList::Init(void)
{
	BOOL bRetCode = false;
	ITabFile* pTabFile = NULL;
	char szFileName[MAX_PATH];

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, NPC_CLASS_FILE_NAME);
    szFileName[sizeof(szFileName) - 1] = '\0';

	pTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(pTabFile);

	//获取最大ID
	m_dwMaxClassID = 0;
	for (int nRowIndex = 2; nRowIndex <= pTabFile->GetHeight(); nRowIndex++)
	{
		DWORD dwClassID = 0;
		bRetCode = pTabFile->GetInteger(nRowIndex, NPC_CLASS_ID, 0, (int*)&dwClassID);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (dwClassID > m_dwMaxClassID)
			m_dwMaxClassID = dwClassID;
	}

	//分配空间
	m_dwMaxClassID++;
	m_NpcClassList = new KNpcClass[m_dwMaxClassID];
	KGLOG_PROCESS_ERROR(m_NpcClassList);
	memset(m_NpcClassList, 0, sizeof(KNpcClass) * m_dwMaxClassID);

	for (int nIndex = 0; nIndex < pTabFile->GetHeight() - 1; nIndex++)
	{
		DWORD dwClassID = 0;

		bRetCode = pTabFile->GetInteger(nIndex + 2, NPC_CLASS_ID, 0, (int*)&dwClassID);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nIndex + 2, NPC_CLASS_NAME, "", 
			m_NpcClassList[dwClassID].szName, _NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetString(nIndex + 2, NPC_CLASS_DROP_NAME, "", 
			m_NpcClassList[dwClassID].szDropName, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nIndex + 2, NPC_PARENT_CLASS_ID, 0, 
			(int*)&(m_NpcClassList[dwClassID].dwParentID));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	KG_COM_RELEASE(pTabFile);

	return true;

Exit0:
	KG_COM_RELEASE(pTabFile);

	return false;
}

BOOL KNpcClassList::UnInit(void)
{
	KG_DELETE_ARRAY(m_NpcClassList);
	m_dwMaxClassID = 0;

	return true;
}

KNpcClass* KNpcClassList::GetData(DWORD dwClassID)
{
	KGLOG_PROCESS_ERROR(dwClassID < m_dwMaxClassID);

	return &(m_NpcClassList[dwClassID]);

Exit0:

	return NULL;
}

DWORD KNpcClassList::GetMaxClassID(void)
{
	return m_dwMaxClassID;
}

DWORD KNpcClassList::GetParentID(DWORD dwClassID)
{
	KGLOG_PROCESS_ERROR(dwClassID < m_dwMaxClassID);

	return m_NpcClassList[dwClassID].dwParentID;

Exit0:

	return ERROR_ID;
}
