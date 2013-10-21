#include "KGTestMTLResource.h"
#include "KGTestLoad.h"
#include "KGTestDifferLoad.h"
#include "KGTestSaveAndLoad.h"
#include "KGTestStressSaveAndLoad.h"
#include "KG3DMaterialTable.h"

KGTestMTLResource::KGTestMTLResource()
{
	m_hEngineDLL	 = NULL;
	m_pEngineMgr	 = NULL;
	m_pTestInterface = NULL;
	m_nTotalFileNum = 0;
	m_nMtlFileNum   = 0;
	m_szApplicationPath[0] = '\0';
	m_szResultPath[0]	   = '\0';
	m_vecTestCase.clear();
}

KGTestMTLResource::~KGTestMTLResource()
{
	UnInit();
}

int KGTestMTLResource::Init(const char cszClientPath[])
{
	int nResult  = false;
	int nRetCode = false;
	char* pszResult = NULL;

	_ASSERTE(cszClientPath);

	KGLogPrintf(KGLOG_INFO, "Begin to Testing");

	nRetCode = KGTestCommon::MakeResultPath(TEST_PROJECT_NAME, m_szResultPath, sizeof(m_szResultPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	//加载引擎动态库
	nRetCode = LoadNewEngineDLL(cszClientPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	//创建测试用例并初始化
	m_vecTestCase.push_back(new KGTestLoad());//此用例必须放在容器最前，保证后面对加载有依赖的用例能够正确执行
	m_vecTestCase.push_back(new KGTestDifferLoad());
	m_vecTestCase.push_back(new KGTestSaveAndLoad());
	m_vecTestCase.push_back(new KGTestStressSaveAndLoad());
	for (UINT i = 0; i < m_vecTestCase.size(); i++)
	{
		KGBaseResourceTestCase* pTestCase = m_vecTestCase[i];
		if (pTestCase)
		{
			nRetCode = pTestCase->Init(m_pEngineMgr, m_pTestInterface, cszClientPath, m_szResultPath);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KGTestMTLResource::UnInit()
{
	if (m_pEngineMgr)
	{
		m_pEngineMgr->UnInit();
		m_pEngineMgr = NULL;
	}
	if (m_pTestInterface)
	{
		m_pTestInterface->Release();
		m_pTestInterface = NULL;
	}
	for (UINT i = 0; i < m_vecTestCase.size(); i++)
	{
		KGBaseResourceTestCase* pTestCase = m_vecTestCase[i];
		if (pTestCase)
		{
			pTestCase->UnInit();
			SAFE_DELETE(pTestCase);
		}
	}
	if (m_hEngineDLL)
	{
		FreeLibrary(m_hEngineDLL);
		m_hEngineDLL = NULL;
	}
	m_vecTestCase.clear();
}

int KGTestMTLResource::RunTest(const char cszTestPath[])
{
	int nRetCode = false;
	int nResult	 = false;
	HANDLE hFindFile  = INVALID_HANDLE_VALUE;
	char szAllFile[MAX_PATH]	  = {0};
	char szFileName[MAX_PATH]	  = {0};
	char szFullPathFile[MAX_PATH] = {0};
	char szPath[MAX_PATH]		  = {0};
	char szExt[MAX_PATH]		  = {0};
	WIN32_FIND_DATA FindFileData;

	_ASSERTE(cszTestPath);

	nRetCode = strncpy_s(szPath, sizeof(szPath), cszTestPath, strlen(cszTestPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = KGTestCommon::FormatPath(szPath, sizeof(szPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(szAllFile,
						   sizeof(szAllFile),
						   sizeof(szAllFile) - 1,
						   "%s%s",
						   szPath,
						   "*.*"
				);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		nRetCode = strncpy_s(szFileName,
							 sizeof(szFileName),
							 FindFileData.cFileName,
							 strlen(FindFileData.cFileName)
					);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		//过滤"."、".."、".svn"
		if (szFileName[0] != '.')
		{
			nRetCode = _snprintf_s(szFullPathFile,
								   sizeof(szFullPathFile),
								   sizeof(szFullPathFile) - 1,
								   "%s%s",
								   szPath,
								   szFileName
						);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = RunTest(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);		
			}
			else
			{
				m_nTotalFileNum++;

				nRetCode = _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _strlwr_s(szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);

				nRetCode = strncmp(szExt, ".mtl", sizeof(".mtl"));
				if (nRetCode == 0)
				{
					m_nMtlFileNum++;
					//执行所有的测试用例
					TestOneMtlFile(szFullPathFile);
				}
				RefreshConsole();
			}
		}
		nRetCode = ::FindNextFile(hFindFile, &FindFileData);
		if (!nRetCode)
		{
			nRetCode = ::GetLastError();
			KG_PROCESS_SUCCESS(nRetCode == ERROR_NO_MORE_FILES);
			KGLOG_PROCESS_ERROR(false);
		}
	}

Exit1:
	nResult = true;
Exit0:
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KGTestMTLResource::LoadNewEngineDLL(const char cszClientPath[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HWND hWindow = NULL;

	_ASSERTE(cszClientPath);

	g_SetRootPath(cszClientPath);

	m_hEngineDLL = LoadLibrary(ENGINE_DLL_NAME);
	KGLOG_PROCESS_ERROR(m_hEngineDLL);

	hWindow = ::GetDesktopWindow();
	KGLOG_PROCESS_ERROR(hWindow);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;

	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KGLOG_PROCESS_ERROR(m_pEngineMgr);
	hrRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, hWindow, hWindow);
	KG_COM_PROCESS_ERROR(hrRetCode);

	typedef HRESULT (*pfnCreateAutoTestInterface)(IKG3DAutoTestInterface** ppvTestInterface);
	pfnCreateAutoTestInterface CreateAutoTestInterface;

	CreateAutoTestInterface = (pfnCreateAutoTestInterface)GetProcAddress(m_hEngineDLL, "CreateAutoTestInterface");
	CreateAutoTestInterface(&m_pTestInterface);
	KGLOG_PROCESS_ERROR(m_pTestInterface);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pEngineMgr)
		{
			m_pEngineMgr->UnInit();
			m_pEngineMgr = NULL;
		}
		if (m_pTestInterface)
		{
			m_pTestInterface->Release();
			m_pTestInterface = NULL;
		}
		if (m_hEngineDLL)
		{
			FreeLibrary(m_hEngineDLL);
			m_hEngineDLL = NULL;
		}

	}
	return nResult;
}


void KGTestMTLResource::RefreshConsole()
{
	HANDLE hStdOut = NULL;
	COORD coordScreen = {0, 0};

	hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	::SetConsoleCursorPosition(hStdOut,coordScreen);

	if (m_nTotalFileNum == 1)
	{
		system("cls");
	}
	printf("============Test Material Files============\n");
	printf("File All Files Num: %d\n", m_nTotalFileNum);
	printf("Checked Mtl Files Num: %d\n", m_nMtlFileNum);
}

void KGTestMTLResource::TestOneMtlFile(const char cszFileName[])
{
	_ASSERTE(cszFileName);
	for (UINT i = 0; i < m_vecTestCase.size(); i++)
	{
		KGBaseResourceTestCase* pTestCase = m_vecTestCase[i];
		if (pTestCase)
		{
			pTestCase->RunTest(cszFileName);
		}
	}
}