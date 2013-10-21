#include "KGBaseResourceTestCase.h"

KGBaseResourceTestCase::KGBaseResourceTestCase()
{
	m_pEngineMgr	 = NULL;
	m_pResultFile	 = NULL;
	m_pTestInterface = NULL;
	m_szClientPath[0] = '\0';
	m_szResultPath[0] = '\0';
	m_vecResult.clear();
}

KGBaseResourceTestCase::~KGBaseResourceTestCase()
{
}

int KGBaseResourceTestCase::Init(IKG3DEngineManager*	 pEngineMgr,
								 IKG3DAutoTestInterface* pTestInterface,
								 const char				 cszClientPath[],
								 const char				 cszResultPath[])
{
	int nResult  = false;
	int nRetCode = false;
	size_t uLen = 0;
	
	KG_ASSERT_EXIT(pEngineMgr);
	KG_ASSERT_EXIT(pTestInterface);
	_ASSERTE(cszClientPath);
	_ASSERTE(cszResultPath);

	m_pEngineMgr	 = pEngineMgr;
	m_pTestInterface = pTestInterface;
	m_pTestInterface->AddRef();

	nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), cszClientPath, strlen(cszClientPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = KGTestCommon::FormatPath(m_szClientPath, sizeof(m_szClientPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = strncpy_s(m_szResultPath, sizeof(m_szResultPath), cszResultPath, strlen(cszResultPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = KGTestCommon::FormatPath(m_szResultPath, sizeof(m_szResultPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KGBaseResourceTestCase::UnInit()
{
	if (m_pResultFile)
	{
		fclose(m_pResultFile);
		m_pResultFile = NULL;
	}
	if (m_pTestInterface)
	{
		m_pTestInterface->Release();
		m_pTestInterface = NULL;
	}
}

int KGBaseResourceTestCase::InitResultLog(const char cszCaseName[])
{
	int nResult  = false;
	int nRetCode = false;
	struct tm *pCurrentTime = NULL;
	time_t TimeValue = 0;
	char szResultFileName[MAX_PATH] = {0};
	char szHeadline[MAX_PATH]		= {0};

	_ASSERTE(cszCaseName);

	nRetCode = _snprintf_s(szResultFileName,
						   sizeof(szResultFileName),
						   sizeof(szResultFileName) - 1,
						   "%s%s.txt",
						   m_szResultPath,
						   cszCaseName
				);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	m_pResultFile = fopen(szResultFileName, "w");
	KGLOG_PROCESS_ERROR(m_pResultFile);

	nRetCode = _snprintf_s(szHeadline,
						   sizeof(szHeadline),
						   sizeof(szHeadline) - 1,
						   "======================================================\n"
						   "Test Case : %s"
						   "\n======================================================\n",
					       cszCaseName
				);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	fwrite(szHeadline, strlen(szHeadline), 1, m_pResultFile);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pResultFile)
		{
			fclose(m_pResultFile);
			m_pResultFile = NULL;
		}
	}
	return nResult;
}

int KGBaseResourceTestCase::OuputResult()
{
	int nResult  = false;
	int nRetCode = false;
	
	for (UINT i = 0; i < m_vecResult.size(); i++)
	{
		m_vecResult[i].append("\n");
		fwrite(m_vecResult[i].c_str(), m_vecResult[i].length(), 1, m_pResultFile);
	}
	m_vecResult.clear();

	nResult = true;
Exit0:
	return nResult;
}

