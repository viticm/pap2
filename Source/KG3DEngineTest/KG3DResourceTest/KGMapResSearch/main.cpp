#include "KGMapResSearchDlg.h"
#include "KGTestCommon.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) 
{
	int nResult  = false;
	int nRetCode = false;
	int TestingInitFlag = false;
	int nMapResSearchInitFlag = false;
	char szDllFile[MAX_PATH];
	IKG3DEngineManager* pEngineMgr = NULL;
	HMODULE hEngineDLL = NULL;
	HRESULT hrRetCode = E_FAIL;
	KGMapResSearchDlg SearchDlg;

	nRetCode = KGTestCommon::TestingInit("KGMapResSearch");
	KG_PROCESS_ERROR(nRetCode);
	TestingInitFlag = true;

	//load engine.dll//////////////////////////////////////////////////////////
	{
		HWND hWindow = NULL;

#if defined(DEBUG) | defined(_DEBUG)
		nRetCode = KGTestCommon::FormatPath(szCmdLine, strlen(szCmdLine) + 1);
		KGLOG_PROCESS_ERROR(nRetCode);
		g_SetRootPath(szCmdLine);

		nRetCode = _snprintf_s(
			szDllFile,
			sizeof(szDllFile),
			sizeof(szDllFile) - 1,
			"%s%s",
			szCmdLine,
			ENGINE_DLL_NAME);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
#else
 		g_SetRootPath(g_szApplicationPath);
		nRetCode = _snprintf_s(
			szDllFile,
			sizeof(szDllFile),
			sizeof(szDllFile) - 1,
			"%s%s",
			g_szApplicationPath,
			ENGINE_DLL_NAME);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
#endif
	 
 		hEngineDLL = LoadLibrary(szDllFile);
 		KGLOG_PROCESS_ERROR(hEngineDLL);
	 
 		hWindow = ::GetDesktopWindow();
 		KGLOG_PROCESS_ERROR(hWindow);
	 
 		typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
 		pfnGet3DEngineInterface Get3DEngineInterface;
	 
 		Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(hEngineDLL, "Get3DEngineInterface");
 		Get3DEngineInterface((void**)&pEngineMgr);
 		KGLOG_PROCESS_ERROR(pEngineMgr);
 		hrRetCode = pEngineMgr->Init(0, KG3DENG_CLIENT, hWindow, hWindow);
 		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	///////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
	nRetCode = 	SearchDlg.Init(hInstance, pEngineMgr, szCmdLine);
#else
	nRetCode = 	SearchDlg.Init(hInstance, pEngineMgr, g_szApplicationPath);
#endif
	KGLOG_PROCESS_ERROR(nRetCode);
	nMapResSearchInitFlag = true;

	nResult = true;
Exit0:
	if (pEngineMgr)
	{
		hrRetCode = pEngineMgr->UnInit();
		KGLOG_COM_CHECK_ERROR(hrRetCode);
		pEngineMgr = NULL;
	}
	if (hEngineDLL)
	{
		nRetCode = FreeLibrary(hEngineDLL);
		KGLOG_CHECK_ERROR(nRetCode);
		hEngineDLL = NULL;
	}
	if (TestingInitFlag)
	{
		KGTestCommon::TestingUnit("KGMapResSearch");
	}
	return nResult ? 0 : 1;

}