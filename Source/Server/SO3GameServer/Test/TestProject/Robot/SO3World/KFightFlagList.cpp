#include "stdafx.h"
#include "KRelationList.h"
#include "KFightFlagList.h"

KFightFlagList::KFightFlagList()
{
	m_nSize = 0;
	m_ForceDefault = NULL;
}

KFightFlagList::~KFightFlagList()
{
	SAFE_DELETE(m_ForceDefault);
	m_nSize = 0;
}

BOOL KFightFlagList::Init()
{
	char		szFile[MAX_PATH];
	ITabFile*	pTabFile;
	BOOL		bResult = FALSE;
	BOOL		bRetCode = FALSE;
	int			nId = 0;

	sprintf(szFile, "%s/%s", SETTING_DIR, RELATION_FORCE_FILE_NAME);
	pTabFile = g_OpenTabFile(szFile);
	KGLOG_PROCESS_ERROR(pTabFile);

	m_nSize = pTabFile->GetHeight() - 1;
	KGLOG_PROCESS_ERROR(m_nSize > 0);

	m_ForceDefault = new FORCE_DEFAULT[m_nSize];
	KGLOG_PROCESS_ERROR(m_ForceDefault);

	for (int i = 0; i < m_nSize; i++)
	{
		bRetCode = pTabFile->GetInteger(i + 2, RELATION_ID, 0, &nId);
		KGLOG_PROCESS_ERROR(bRetCode && nId == i);

		bRetCode = pTabFile->GetInteger(i + 2, DEFAULT_FLAG, 0, &m_ForceDefault[i].bDefaultFlag);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(i + 2, FLAG_IS_LOCKED, 0, &m_ForceDefault[i].bLockedFlag);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(i + 2, DEFAULT_REPUTE, 0, &m_ForceDefault[i].nRepute);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	
	bResult = TRUE;
Exit0:
	return bResult;
}

FORCE_DEFAULT* KFightFlagList::GetDefaultRepute(DWORD dwForceID)
{
	FORCE_DEFAULT* pResult = NULL;
	KGLOG_PROCESS_ERROR(dwForceID < (DWORD)m_nSize);

	pResult = &m_ForceDefault[dwForceID];
Exit0:
	return pResult;
}