#include "stdafx.h"
#include "KRelationList.h"


KRelationList::KRelationList()
{
	m_nSize = 0;
	m_nRelation = NULL;
}

KRelationList::~KRelationList()
{
	for (int i = 0; i < m_nSize; i++)
	{
		SAFE_DELETE_ARRAY(m_nRelation[i]);
	}
	SAFE_DELETE_ARRAY(m_nRelation);
	m_nSize = 0;
}

BOOL KRelationList::Init(const char* szSettingFile)
{
	BOOL bResult = false;
	int nSize = 0;
	int nRetCode = 0;
	ITabFile*	pTabFile = NULL;
	char	szFileName[MAX_PATH];

	KGLOG_PROCESS_ERROR(szSettingFile);

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, szSettingFile);
    szFileName[sizeof(szFileName) - 1] = '\0';

	pTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(pTabFile);

	nSize = pTabFile->GetHeight() - 1;
	KGLOG_PROCESS_ERROR(nSize > 0);
	assert(m_nRelation == NULL);

	m_nRelation = new int*[nSize];
	for (int i = 0; i < nSize; i++)
	{
		m_nRelation[i] = new int[nSize];
	}
	m_nSize = nSize;

	for (int i = 2; i < nSize + 2; i++)
	{
		int nId = 0;
		nRetCode = pTabFile->GetInteger(i, RELATION_ID, 0, &nId);
		KGLOG_PROCESS_ERROR(nRetCode && nId == i - 2);
		for (int j = 0; j < nSize; j++)
		{
			char szKey[256];
			sprintf(szKey, "%d", j);
			nRetCode = pTabFile->GetInteger(i, szKey, 0, &m_nRelation[i - 2][j]);
			KGLOG_PROCESS_ERROR(nRetCode != 0);
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(pTabFile);
	return bResult;
}

int	KRelationList::GetRelation(int nA, int nB)
{
	int nResult = 0;
	KGLOG_PROCESS_ERROR(nA >= 0 && nA < m_nSize);
	KGLOG_PROCESS_ERROR(nB >= 0 && nB < m_nSize);

	nResult = m_nRelation[nA][nB];
Exit0:
	return nResult;
}
