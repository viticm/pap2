#include "stdafx.h"
#include "KReputationLimit.h"

KReputationLimit::KReputationLimit()
{
	m_nMaxLevel = 0;
	m_pnReputeLimit = NULL;
}


KReputationLimit::~KReputationLimit()
{
    assert(m_pnReputeLimit == NULL);

    SAFE_DELETE_ARRAY(m_pnReputeLimit);
}


BOOL KReputationLimit::Init()
{
	BOOL	    bResult     = false;
	BOOL	    bRetCode    = false;
	ITabFile*   piTabFile   = NULL;
    int         nLevel      = 0;
    char	    szFile[MAX_PATH];

	snprintf(szFile, sizeof(szFile), "%s/%s", SETTING_DIR, "ReputationLimit.tab");
    szFile[sizeof(szFile) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFile);
	KGLOG_PROCESS_ERROR(piTabFile);

	m_nMaxLevel = piTabFile->GetHeight() - 1;
    KGLOG_PROCESS_ERROR(m_nMaxLevel > 0);

	m_pnReputeLimit = new int[m_nMaxLevel];
    KGLOG_PROCESS_ERROR(m_pnReputeLimit);

	for (int i = 0; i < m_nMaxLevel; i++)
	{	
		bRetCode = piTabFile->GetInteger(i + 2, "Level", 0, &nLevel);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nLevel == i + 1);

		bRetCode = piTabFile->GetInteger(i + 2, "MaxReputation", 1, &m_pnReputeLimit[i]);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(m_pnReputeLimit[i] > 0);
	}

	bResult = true;
Exit0:
	if (!bResult)
	{
		SAFE_DELETE_ARRAY(m_pnReputeLimit);
	}
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KReputationLimit::UnInit()
{
    m_nMaxLevel = 0;
    SAFE_DELETE_ARRAY(m_pnReputeLimit);
}

int KReputationLimit::GetMaxLevel()
{
	return m_nMaxLevel - 1;
}

int KReputationLimit::GetReputeLimit(int nLevel)
{
	int nResult = -1;

	KGLOG_PROCESS_ERROR(nLevel < m_nMaxLevel && nLevel >= 0);

	nResult = m_pnReputeLimit[nLevel];
Exit0:
	return nResult;
}
