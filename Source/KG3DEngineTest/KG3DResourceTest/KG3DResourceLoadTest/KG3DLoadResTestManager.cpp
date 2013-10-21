#include "KG3DLoadResTestManager.h"

KG3DLoadResTestManager::KG3DLoadResTestManager(void)
{
	m_nTotalFileNum		= 0;
	m_nMtlFileNum		= 0;
	m_nMeshFileNum		= 0;
	m_szClientPath[0]	= '\0';
	m_hEngineDLL		= NULL;
	m_pEngineMgr		= NULL;
	m_pTestInterface	= NULL;
}

KG3DLoadResTestManager::~KG3DLoadResTestManager(void)
{
}

int KG3DLoadResTestManager::Init(const char cszClientPath[])
{
	int nResult  = false;
	int nRetCode = false;
	char szResultPath[MAX_PATH] = {0};
	char szClientPath[MAX_PATH] = {0};

	if (cszClientPath == NULL)
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), g_szApplicationPath, strlen(g_szApplicationPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}
	else
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), cszClientPath, strlen(cszClientPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	nRetCode = KGTestCommon::FormatPath(m_szClientPath, sizeof(m_szClientPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = LoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	//构造输出结果目录
	nRetCode = KGTestCommon::MakeResultPath(TEST_PROJECT_NAME, szResultPath, sizeof(szResultPath));
	KGLOG_PROCESS_ERROR(nRetCode);
	
	nRetCode = m_TestLoadMaterial.Init(m_pTestInterface, szResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_TestLoadMesh.Init(m_pTestInterface, szResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);
		
	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KG3DLoadResTestManager::UnInit()
{
	m_TestLoadMaterial.UnInit();
	m_TestLoadMesh.UnInit();
	if (m_pTestInterface)
	{
		m_pTestInterface->Release();
		m_pTestInterface = NULL;
	}
	if (m_pEngineMgr)
	{
		m_pEngineMgr->UnInit();
		m_pEngineMgr = NULL;
	}
	if (m_hEngineDLL)
	{
		FreeLibrary(m_hEngineDLL);
		m_hEngineDLL = NULL;
	}
}

int KG3DLoadResTestManager::Run(const char cszTestPath[])
{
	int nRetCode = false;
	int nResult	 = false;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	char szFileName[MAX_PATH]	  = {0};
	char szAllFile[MAX_PATH]	  = {0};
	char szFullPathFile[MAX_PATH] = {0};
	char szPath[MAX_PATH]		  = {0};
	char szExt[MAX_PATH]		  = {0};
	WIN32_FIND_DATA FindFileData;

	if (cszTestPath == NULL)
	{
		nRetCode = strncpy_s(szPath, sizeof(szPath), g_szApplicationPath, strlen(g_szApplicationPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}
	else
	{
		nRetCode = strncpy_s(szPath, sizeof(szPath), cszTestPath, strlen(cszTestPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	nRetCode = KGTestCommon::FormatPath(szPath, sizeof(szPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(
		szAllFile,
		sizeof(szAllFile),
		sizeof(szAllFile) - 1,
		"%s%s", szPath,
		"*.*"
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		nRetCode = strncpy_s(
			szFileName,
			sizeof(szFileName),
			FindFileData.cFileName,
			strlen(FindFileData.cFileName)
			);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		//过滤"."、".."、".svn"
		if (szFileName[0] != '.')
		{
			nRetCode = _snprintf_s(
				szFullPathFile,
				sizeof(szFullPathFile),
				sizeof(szFullPathFile) - 1,
				"%s%s",
				szPath,
				szFileName
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = Run(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				m_nTotalFileNum++;

				nRetCode = _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _strlwr_s(szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);

				if (strncmp(szExt, ".mtl", sizeof(".mtl")) == 0)
				{
					m_nMtlFileNum++;
					m_TestLoadMaterial.TestLoadOneMtlFile(szFullPathFile);
				}
				else if (strncmp(szExt, ".mesh", sizeof(".mesh")) == 0)
				{
					m_nMeshFileNum++;
					m_TestLoadMesh.TestLoadOneMeshFile(szFullPathFile);
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

void KG3DLoadResTestManager::RefreshConsole()
{
	int nRetCode   = false;
	HANDLE hStdOut = INVALID_HANDLE_VALUE;
	COORD coordScreen = {0, 0};

	hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	KGLOG_PROCESS_ERROR(hStdOut != INVALID_HANDLE_VALUE);
	nRetCode = ::SetConsoleCursorPosition(hStdOut, coordScreen);
	KGLOG_PROCESS_ERROR(nRetCode);
	if (m_nTotalFileNum == 1)
	{
		system("cls");
	}
	printf("============Test Load Resource Files============\n");
	printf("File All Files Num: %d\n", m_nTotalFileNum);
	printf("Loaded Mtl Files Num: %d\n", m_nMtlFileNum);
	printf("Loaded Mesh Files Num: %d\n", m_nMeshFileNum);
Exit0:
	;
}

int KG3DLoadResTestManager::OutputResult()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = m_TestLoadMaterial.OutputResult();
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_TestLoadMesh.OutputResult();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG3DLoadResTestManager::LoadEngineDLL()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HWND hWindow = NULL;
	char szEngineDll[MAX_PATH] = {0};

	g_SetRootPath(m_szClientPath);
	::SetCurrentDirectory(m_szClientPath);

	nRetCode = _snprintf_s(
		szEngineDll,
		sizeof(szEngineDll) - 1,        
		"%s%s",
		g_szApplicationPath,
		ENGINE_DLL_NAME
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = _access(szEngineDll, 0);
	if (nRetCode != 0)
	{
		nRetCode = _snprintf_s(
			szEngineDll,
			sizeof(szEngineDll) - 1,        
			"%s%s",
			m_szClientPath,
			ENGINE_DLL_NAME
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
	}

	m_hEngineDLL = LoadLibrary(szEngineDll);
	KGLOG_PROCESS_ERROR(m_hEngineDLL);

	hWindow = ::GetDesktopWindow();
	KGLOG_PROCESS_ERROR(hWindow);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;

	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KGLOG_PROCESS_ERROR(m_pEngineMgr);
	hrRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, hWindow, hWindow);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	typedef HRESULT (*pfnCreateAutoTestInterface)(IKG3DAutoTestInterface** ppvTestInterface);
	pfnCreateAutoTestInterface CreateAutoTestInterface;

	CreateAutoTestInterface = (pfnCreateAutoTestInterface)GetProcAddress(m_hEngineDLL, "CreateAutoTestInterface");
	CreateAutoTestInterface(&m_pTestInterface);
	KGLOG_PROCESS_ERROR(m_pTestInterface);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pTestInterface)
		{
			m_pTestInterface->Release();
			m_pTestInterface = NULL;
		}
		if (m_pEngineMgr)
		{
			m_pEngineMgr->UnInit();
			m_pEngineMgr = NULL;
		}

		if (m_hEngineDLL)
		{
			FreeLibrary(m_hEngineDLL);
			m_hEngineDLL = NULL;
		}
	}
	return nResult;
}

int KG3DLoadResTestManager::AnalyseLog()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = m_TestLoadMaterial.AnalyseLog();
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_TestLoadMesh.AnalyseLog();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}