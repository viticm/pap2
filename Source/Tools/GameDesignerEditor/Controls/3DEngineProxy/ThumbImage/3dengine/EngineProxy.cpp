#include "EngineProxy.h"
BOOL QueryClientRootPath(char szEngineRootPath[]);

extern EngineProxy* g_pEngineProxy;
IEKG3DEngineManager* g_pEngine = NULL;
BOOL g_bLoop = FALSE;
TCHAR g_pFileName[MAX_PATH];
TCHAR g_szAniFile[MAX_PATH];
BOOL WINAPI LoadModel(LPCTSTR pcszFileName);
BOOL g_bNeedRenderToTexture = FALSE;
BOOL g_bNeedAnimationTexture = FALSE;
BOOL g_bTextureReady = FALSE;
BOOL g_bLoadModelOK = FALSE;
BOOL g_bAnimationReady = TRUE;
SIZE g_bmSize;

IEKG3DGraphicsTool* g_pGraphicsTool = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
IEKG3DTextureTable* g_pTextureTable = NULL;

EngineProxy::EngineProxy(void)
{
	m_pEngineManager = NULL;
	m_pModelEditor   = NULL;
	m_hEngineMod     = NULL;
	m_pScene         = NULL;
	m_pLastModel     = NULL;
	m_pCamera        = NULL;
	m_hThreadEngine = NULL;
	m_hEngineInitFinishEvent = NULL;

	m_nOutputWindowID = -1;
}

EngineProxy::~EngineProxy(void)
{
}


BOOL EngineProxy::InitLog()
{
	BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
	char*                       pszRetCWD           = NULL;
	KGLOG_PARAM                 LogParam;
	char                        szCWD[MAX_PATH];

	pszRetCWD = _getcwd(szCWD, sizeof(szCWD));
	KGLOG_PROCESS_ERROR(pszRetCWD);
	
	memset(&LogParam, 0, sizeof(LogParam));
	snprintf(LogParam.szPath,sizeof(LogParam.szPath),"%s\\logs",szCWD);
	LogParam.szPath[sizeof(LogParam.szPath) - 1] = '\0';

	strcpy(LogParam.szIdent, "MeshThumbnail");
	LogParam.nMaxLineEachFile = 65536;
	LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;

	bRetCode = KGLogInit(LogParam, NULL);
	if (!bRetCode)
	{
		::MessageBox(NULL, "Log文件初始化失败!", NULL, NULL);
		goto Exit0;
	}
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL WINAPI MainThread(HWND hwd)
{
	BOOL bResult             = false;
	BOOL bRetCode            = false;

	BOOL bLogInitFlag = false;
	BOOL bEngineInitFlag = false;
	BOOL bSceneInitFlag = false;

#ifdef DEBUG
	bRetCode = g_pEngineProxy->InitLog();
	KGLOG_PROCESS_ERROR(bRetCode);
	bLogInitFlag = TRUE;
#endif

	KGLOG_PROCESS_ERROR(hwd);

	//TCHAR szClientRootPath[MAX_PATH] = {0};	
	//bRetCode = QueryClientRootPath(szClientRootPath);
	//KGLOG_PROCESS_ERROR(bRetCode);

	TCHAR szKG3DEngineDllPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szKG3DEngineDllPath, MAX_PATH -1);
	::PathRemoveFileSpec(szKG3DEngineDllPath);

	bRetCode = g_pEngineProxy->Init3DEngine(0,KG3DENG_NONE,hwd,hwd,szKG3DEngineDllPath);
	KGLOG_PROCESS_ERROR(bRetCode);

	bEngineInitFlag = true;
	KGLogPrintf(KGLOG_INFO, "Init 3dEngine");

	bRetCode = g_pEngineProxy->InitContext();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_pEngineProxy->InitScene(hwd, 0);
	KGLOG_PROCESS_ERROR(bRetCode);
	bSceneInitFlag = true;

	bRetCode = g_pEngineProxy->InitModeEditor();
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = TRUE;
	g_bLoop = TRUE;
	::SetEvent(g_pEngineProxy->m_hEngineInitFinishEvent);

	while (g_bLoop)
	{
		g_pEngine->FrameMove();
		g_pEngine->Render();

		if (*g_pFileName != 0)
		{
			g_pEngineProxy->UpdateEngineRootPath(g_pFileName);
			g_bLoadModelOK = LoadModel(g_pFileName);
			*g_pFileName = '\0';
		}
		if (g_bNeedRenderToTexture)
		{
			g_bNeedRenderToTexture = 0;
			
 
			TCHAR szTempBmpFileName[MAX_PATH];
			sprintf(szTempBmpFileName, "%s",  "c:\\mesh_mdl.bmp");
			if (g_pEngineProxy->m_pLastModel)
			{	
				bool bUsedForMeshMdlThumbnail = true;
				HRESULT hResult = g_pEngineProxy->m_pLastModel->RenderToPic(szTempBmpFileName,
					g_bmSize.cx, g_bmSize.cy, 0x00ffffff, bUsedForMeshMdlThumbnail);
				//bRetCode = SUCCEEDED(hResult);
			//	bRetCode = g_pEngineProxy->RenderModeToPic(szTempBmpFileName);
				//KGLOG_PROCESS_ERROR(bRetCode);
				g_bTextureReady = TRUE;
			}
		}
		if (g_bNeedAnimationTexture)
		{
			g_bNeedAnimationTexture = FALSE;

			TCHAR szTempBmpFileName[MAX_PATH];//20个图
			sprintf(szTempBmpFileName, "%s", "c:\\Animation");
			if (g_pEngineProxy->m_pLastModel)
			{
				HRESULT hResult = g_pEngineProxy->m_pLastModel->RenderAnimationToPicSerial(g_szAniFile, szTempBmpFileName,
					g_bmSize.cx, g_bmSize.cy, 0xffffffff);
				bRetCode = SUCCEEDED(hResult);
				KGLOG_PROCESS_ERROR(bRetCode);
				g_bAnimationReady = TRUE;
			}
		}
 		Sleep(50);
	}
	g_pEngineProxy->UnInit();

	bResult = TRUE;
Exit0:
	if (!bResult)
	{
		if (bLogInitFlag)
		{
			KGLogUnInit(NULL);
		}
		if (bEngineInitFlag)
		{
			g_pEngineProxy->UnInit3DEngine();
		}
		if (bSceneInitFlag)
		{

		}		
		::SetEvent(g_pEngineProxy->m_hEngineInitFinishEvent);
	}
	return bResult;
}

BOOL EngineProxy::Start(HWND hwd)
{
	 m_hEngineInitFinishEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	 if (m_hEngineInitFinishEvent == NULL)
	 {
		 return false;
	 }
	 
	 m_hThreadEngine = CreateThread(0,0,(LPTHREAD_START_ROUTINE)MainThread,(LPVOID)hwd,0,0); 

	::WaitForSingleObject(m_hEngineInitFinishEvent, INFINITE);

	::CloseHandle(m_hEngineInitFinishEvent);
	m_hEngineInitFinishEvent = NULL;

	return g_bLoop;
}

BOOL EngineProxy::UnInit()
{
	int bRetCode = false;
	bool bResult = false;

	bRetCode = KGLogUnInit(NULL);
	KGLOG_PROCESS_ERROR(bRetCode);

	if (m_pScene)
	{
		m_pScene->RemoveOutputWindow(m_nOutputWindowID);
		if (m_pEngineManager)
		{
			m_pEngineManager->RemoveScene(m_pScene);
		}
		m_pScene = NULL;
	}


	UnInit3DEngine();	

	m_pEngineManager = NULL;
	m_pModelEditor   = NULL;
	m_hEngineMod     = NULL;
	m_pScene         = NULL;
	m_pLastModel     = NULL;


	m_nOutputWindowID = -1;

	bResult = TRUE;
Exit0:
	return bResult;
}
BOOL EngineProxy::Stop()
{
	int bRetCode = false;
	bool bResult = false;

	g_bLoop = FALSE;

	::WaitForSingleObject(m_hThreadEngine, INFINITE);
	::CloseHandle(m_hThreadEngine);

	bResult = TRUE;
Exit0:
	

	return bResult;
}

BOOL EngineProxy::Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow,const char szKG3DEngineDllPath[])
{
	bool bRetCode = false;
	bool bResult = false;

	TCHAR szCurDir[MAX_PATH];
	sprintf(szCurDir, szKG3DEngineDllPath);
	g_SetRootPath(szCurDir);
	SetCurrentDirectory(szCurDir);

	

#ifdef _DEBUG
	::PathAppend(szCurDir, "KG3DEngineD.dll");	
#else
	::PathAppend(szCurDir, "KG3DEngine.dll");
#endif

	m_hEngineMod = ::LoadLibrary(szCurDir);
	KGLOG_PROCESS_ERROR(m_hEngineMod);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** pEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineMod, "Get3DEngineInterface");
	KGLOG_PROCESS_ERROR(Get3DEngineInterface);	

	IEKG3DEngineManager* pInterface = NULL;

	HRESULT hret = Get3DEngineInterface((void**)&pInterface);
	bRetCode = SUCCEEDED(hret);
	KGLOG_PROCESS_ERROR(bRetCode);

	hret = pInterface->Init(uMode, uFlag, hBaseWindow, hRenderWindow);
	bRetCode = SUCCEEDED(hret);
	KGLOG_PROCESS_ERROR(bRetCode);

	pInterface->SetAutoReloadTexture(TRUE);

	m_pEngineManager = pInterface;
	g_pEngine = m_pEngineManager;

	g_pEngine->GetGraphicsTool(&g_pGraphicsTool);	
	g_pEngine->GetD3DDevice(&g_pd3dDevice);
	g_pEngine->GetIEKG3DTextureTable(&g_pTextureTable);


	bResult = true;
Exit0:
	return bResult;
}
BOOL EngineProxy::InitContext()
{
	bool bResult = false;

	KG3D_GAME_INIT_CONTEXT init_context = {0};

	init_context.gameTime.fBoostFactor = 1.0f;

	init_context.logicalConstance.altitudeUnit = 1.0f;
	init_context.logicalConstance.cellLength = 1.0f;
	init_context.logicalConstance.invalidObstableGroup = 1;
	init_context.logicalConstance.logicalCellCmLength = 1.0f;
	init_context.logicalConstance.pointPerAltitude = 1.0f;
	init_context.logicalConstance.regionGridHeight = 1.0f;
	init_context.logicalConstance.regionGridWidth = 1.0f;
	init_context.logicalConstance.terrainMaxHeight = 1.0f;
	init_context.logicalConstance.terrainMinHeight = 1.0f;

	KGLOG_PROCESS_ERROR(m_pEngineManager);

	HRESULT hRetCode = m_pEngineManager->InitGameContext(&init_context);
	KGLOG_PROCESS_ERROR(SUCCEEDED(hRetCode));

	bResult = true;
Exit0:
	return bResult;


}
BOOL EngineProxy::InitScene(HWND hwndOutput, LONG iOption)
{
	bool bResult = false;

	INT windowid = 0;
	ULONG dwOutputWndType;

	IKG3DCamera* pCamera = NULL;
	D3DXVECTOR3 vCameraPos(80.f, 80.f, -80.f);

	m_pScene = NULL;

	m_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR, (IKG3DScene**)&m_pScene);
	KGLOG_PROCESS_ERROR(m_pScene);

	
	m_pScene->LoadFromFile("",0,0);

	m_pEngineManager->SetFocusScene(m_pScene);


	dwOutputWndType = OUTPUTWND_MODELEDITOR_MAIN;

	m_pScene->AddOutputWindow("SceneMain", hwndOutput, dwOutputWndType, 0, &windowid);

	m_nOutputWindowID = windowid;
	m_pScene->SetCurrentOutputWindow(windowid);

	m_pCamera = m_pScene->GetCurrentCamera();
	KGLOG_PROCESS_ERROR(m_pCamera);

	m_pCamera->SetPosition(vCameraPos);


	/*IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	m_pScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
	piSceneCurOutputWnd->InitCameraStation(EDITOR_CONFIG_FILE);	*/

	bResult = true;
Exit0:
	return bResult;
}
BOOL EngineProxy::InitModeEditor()
{
	BOOL bResult = false;
	

	KGLOG_PROCESS_ERROR(m_pEngineManager);
	KGLOG_PROCESS_ERROR(m_pScene);

	m_pScene->GetSceneModelEditor(&m_pModelEditor);
	KGLOG_PROCESS_ERROR(m_pModelEditor);

	bResult = true;
Exit0:
	return bResult;
}

void EngineProxy::UnInit3DEngine()
{	
	if (m_pEngineManager)
	{
		m_pEngineManager->UnInit();
		m_pEngineManager = NULL;
	}
	
	if (m_hEngineMod)
	{
		::FreeLibrary(m_hEngineMod);
		m_hEngineMod = NULL;
	}
	g_bLoop = FALSE;
}


BOOL WINAPI LoadModel(LPCTSTR pcszFileName)
{
	BOOL bResult = false;

	if(!g_IsFileExist(pcszFileName))
	{
		STRING strErr = TEXT("文件不存在：") + STRING(pcszFileName);
		OutputDebugString(strErr.c_str());
		return FALSE;
	}
	if (g_pEngineProxy->m_pLastModel)
	{
		g_pEngineProxy->m_pModelEditor->RemoveModel(g_pEngineProxy->m_pLastModel);
		g_pEngineProxy->m_pLastModel = NULL;
	}

	g_pEngineProxy->m_pModelEditor->ClearModel();	
	g_pEngineProxy->m_pModelEditor->OpenModel(pcszFileName, "", "", FALSE, &g_pEngineProxy->m_pLastModel);
	
	KGLOG_PROCESS_ERROR(g_pEngineProxy->m_pLastModel);

	bResult = true;
Exit0:
	return bResult;
}

BOOL EngineProxy::CreateThumbnail(const SIZE bmSize, LPCTSTR lpFileName)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = false;
	HBITMAP hThumb = NULL; 	
	g_bmSize = bmSize;
	
	g_bLoadModelOK = FALSE;
	strcpy(g_pFileName, lpFileName);

	CHAR szFileRootPath[MAX_PATH] = {0};

	g_GetFullPath(szFileRootPath, lpFileName);
	g_GetHalfPath(szFileRootPath, lpFileName);

	
	while (*g_pFileName != 0)
	{
		Sleep(0);
	}

	KGLOG_PROCESS_ERROR(g_bLoadModelOK);
	KGLOG_PROCESS_ERROR(m_pLastModel);

	while (!m_pLastModel->IsResourceReady(TRUE))
	{
		Sleep(0);
	}


	//bRetCode = LoadModel(lpFileName);
	//KGLOG_PROCESS_ERROR(bRetCode);

	g_bNeedRenderToTexture = TRUE;
	g_bTextureReady = FALSE;

	while (!g_bTextureReady)
	{
		Sleep(0);
	}

	bResult = TRUE;
Exit0:
	return bResult; 
}

BOOL EngineProxy::CreateAnimationFile(const SIZE bmSize, LPCTSTR lpFileName, LPCTSTR lpAniFileName)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = false;
	HBITMAP hThumb = NULL; 	
	g_bmSize = bmSize;
	
	strcpy(g_pFileName, lpFileName);
	strcpy(g_szAniFile, lpAniFileName);

	while (!m_pLastModel || !m_pLastModel->IsResourceReady(TRUE))
	{
		Sleep(0);
	}

	g_bNeedAnimationTexture = TRUE;
	g_bAnimationReady = 0;

	while (!g_bAnimationReady)
	{
		Sleep(1);
	}
	
	bResult = TRUE;
Exit0:
	return bResult; 
}

BOOL QueryClientRootPath(char szEngineRootPath[])
{
	BOOL bRetCode = false;
	BOOL bResult = false;

	TCHAR szEngineRootPathKeyName[] = ".mesh";
	TCHAR szValueName[] = "3DEngineRootPath";
	BYTE  byData[256];
	DWORD dwDataLen = 256;
	
	HKEY hKeyReturn;
	DWORD dwLenReturn;
	DWORD dwTypeReturn;

	BOOL bOpenKeySucess = true;


	LRESULT lresult = RegOpenKeyEx(HKEY_CLASSES_ROOT,LPCSTR(szEngineRootPathKeyName), 0, KEY_READ,  &hKeyReturn);

	if (lresult != ERROR_SUCCESS)
	{		
		KGLogPrintf(KGLOG_ERR, "open registry key %s failed!", szEngineRootPathKeyName);
		bOpenKeySucess = false;
		goto Exit0;
	}
	lresult = RegQueryValueEx(hKeyReturn, LPCTSTR(szValueName), NULL, &dwTypeReturn , byData, &dwDataLen);
	if (lresult != ERROR_SUCCESS)
	{		
		KGLogPrintf(KGLOG_ERR, "query registry key:%s value %s failed! errorcode = %d", szEngineRootPathKeyName, szValueName, lresult);
		//goto Exit0;
	}
	if (byData[dwDataLen - 1] == '\\' || byData[dwDataLen - 1] == '/')
	{
		sprintf(szEngineRootPath, "%s", byData);
	}
	else
	{
		sprintf(szEngineRootPath, "%s\\", byData);
	}
	
	KGLogPrintf(KGLOG_DEBUG, "szenginerootpath = %s", szEngineRootPath);
	
	
	bResult = TRUE;
Exit0:
	if (bOpenKeySucess)
	{
		RegCloseKey(hKeyReturn);
	}
	return bResult;

}

BOOL EngineProxy::UpdateEngineRootPath(LPCSTR lpFileName)
{
	BOOL bRetCode = false;
	BOOL bResult = false;

	CHAR szFileRootPath[MAX_PATH] = {0};
	CHAR* pData = NULL;
	CHAR szOldEngineRootPath[MAX_PATH] = {0};

	KG_PROCESS_ERROR(strlen(lpFileName) < MAX_PATH);

	strcpy(szFileRootPath, lpFileName);

	pData = StrStrI(szFileRootPath, "\\data\\");
	KG_PROCESS_ERROR(pData);

	*pData = 0;

//	g_GetRootPath(szOldEngineRootPath);

//	if (strcmp(szOldEngineRootPath, szFileRootPath) != 0)
	{
		g_SetRootPath(szFileRootPath);
	}

	bResult = TRUE;
Exit0:	
	return bResult;
}