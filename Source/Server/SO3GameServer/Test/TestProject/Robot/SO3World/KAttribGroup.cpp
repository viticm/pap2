#include "stdafx.h"
#include "KAttribGroup.h"

KAttribGroup::KAttribGroup()
{
}

KAttribGroup::~KAttribGroup()
{
}

BOOL KAttribGroup::Init(const char* pszFile)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	ITabFile* pFile = NULL;

	KG_PROCESS_ERROR(pszFile);

	pFile = g_OpenTabFile(pszFile);
	KG_PROCESS_ERROR(pFile);

	int nHeight = pFile->GetHeight();
	// 第一行是默认值, 所以一个有效的表至少有两行。
	KGLOG_PROCESS_ERROR(nHeight > 1);

	for (int i = 0; i < MAX_ATTRIB_GROUP_NUM; i++)
	{
		char	szKey[32];
		int		nCount = 0;

		sprintf(szKey, "Group%d", i + 1);

		for (int nIndex = 2; nIndex < nHeight + 1; nIndex++)
		{
			KAttribRate	AR;
			int nRate = 0;
			bRetCode = pFile->GetInteger(nIndex, szKey, 0, &nRate);
			KGLOG_PROCESS_ERROR(nRate >= 0);
			if (!bRetCode)	// Group读完了
			{
				bResult = TRUE;
				goto Exit0;
			}

			if (nRate > 0)
			{
				AR.nAttribIndex		= nIndex;
				AR.nRate			= nRate;
				m_Rate[i].push_back(AR);
				nCount++;
			}
		}
	}
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAttribGroup::UnInit()
{
	for (int i = 0; i < MAX_ATTRIB_GROUP_NUM; i++)
	{
		m_Rate[i].clear();
	}
//Exit0:
	return TRUE;
}

int KAttribGroup::GetAttribIndex(int nGroup)
{
	int nResult = ERROR_ID;
	vector<KAttribRate>::iterator it;

	KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_ATTRIB_GROUP_NUM);

	int nRand = g_Random(MILLION_NUM);

	int nCurrentRate = 0;

	it = m_Rate[nGroup].begin();
	
	while (it != m_Rate[nGroup].end())
	{
		nCurrentRate += (*it).nRate;
		nResult = (*it).nAttribIndex;
		KG_PROCESS_ERROR(nRand > nCurrentRate);
		it++;
	}
	nResult = ERROR_ID;
Exit0:
	return nResult;
}