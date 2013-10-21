#include "StdAfx.h"
#include "./ClientString.h"
#include "./KLoadingWindow.h"
#include "./KJX3ClientLua.h"
#include "./KSO3Client.h"
#include "./KJX3Sdoa.h"
#include "./KJX3CodePage.h"
#include "KSO3World.h"
#include "KAttribute.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3UI/ISO3UI.h"
#include "SO3UI/IKGUI.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KScriptFuncList.h"

extern KLuaConstList g_LuaConstList[];

static int GetExeFilePath(LPSTR pszExePath, size_t nExePathBufferSize)
{
    int nResult = false;
    LPSTR pszDelimiter = NULL;
    DWORD dwFileNameSize = 0;

    KGLOG_PROCESS_ERROR(pszExePath);

    dwFileNameSize = GetModuleFileNameA(NULL, pszExePath, (DWORD)nExePathBufferSize);
    KGLOG_PROCESS_ERROR(dwFileNameSize > 0 && dwFileNameSize < nExePathBufferSize);

    pszDelimiter = strrchr(pszExePath, '\\');
    KGLOG_PROCESS_ERROR(pszDelimiter);

    KGLOG_PROCESS_ERROR(pszDelimiter + 1 != pszExePath + nExePathBufferSize);

    *(pszDelimiter + 1) = '\0';

    nResult = true;
Exit0:
    return nResult;
}

static int InitDllPath(LPCSTR szExePath, KSO3ClientFilePath* pDllPath)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(szExePath);
    KGLOG_PROCESS_ERROR(pDllPath);

#ifdef _DEBUG
    static LPCSTR s_szSuffix = "D.dll";
#else
    static LPCSTR s_szSuffix = ".dll";
#endif

    strncpy_s(pDllPath->szExePath, MAX_PATH, szExePath, _TRUNCATE);
    
    _snprintf_s(pDllPath->szKG3DEngine, MAX_PATH, _TRUNCATE, "%sKG3DEngine%s", szExePath, s_szSuffix);
    _snprintf_s(pDllPath->szKG3DSound, MAX_PATH, _TRUNCATE, "%sKG3DSound%s", szExePath, s_szSuffix);
    _snprintf_s(pDllPath->szKGUI, MAX_PATH, _TRUNCATE, "%sKGUI%s", szExePath, s_szSuffix);
    _snprintf_s(pDllPath->szJX3UI, MAX_PATH, _TRUNCATE, "%sJX3UI%s", szExePath, s_szSuffix);
    _snprintf_s(pDllPath->szJX3Represent, MAX_PATH, _TRUNCATE, "%sJX3Represent%s", szExePath, s_szSuffix);
    _snprintf_s(pDllPath->szJX3Interaction, MAX_PATH, _TRUNCATE, "%sJX3Interaction%s", szExePath, s_szSuffix);
    _snprintf_s(pDllPath->szKLVideo, MAX_PATH, _TRUNCATE, "%sKLVideo%s", szExePath, s_szSuffix);

#ifdef _DEBUG
    _snprintf_s(pDllPath->szIceClientlib, MAX_PATH, _TRUNCATE, "%siceclientlib_d.dll", szExePath);
#else
    _snprintf_s(pDllPath->szIceClientlib, MAX_PATH, _TRUNCATE, "%siceclientlib.dll", szExePath);
#endif

    _snprintf_s(pDllPath->szSdoaGameWidget, MAX_PATH, _TRUNCATE, "%ssdoa/GameWidget.dll", szExePath);

    _snprintf_s(pDllPath->szPackageList, MAX_PATH, _TRUNCATE, "%spackage.ini", szExePath);
    _snprintf_s(pDllPath->szConfig, MAX_PATH, _TRUNCATE, "%sconfig.ini", szExePath);
    _snprintf_s(pDllPath->szVersion, MAX_PATH, _TRUNCATE, "%sversion.cfg", szExePath);

    nResult = true;
Exit0:
    return nResult;
}

static int InitConfig(HWND hwnd, KJX3Config* pConfig, LPCSTR szConfigFilePath)
{
    int nRetCode = false;
    int nResult = false;
    IIniFile* pIni = NULL;
    char szMessage[512];

    KGLOG_PROCESS_ERROR(pConfig);
    KGLOG_PROCESS_ERROR(szConfigFilePath);

    pIni = g_OpenIniFile(szConfigFilePath);
    KGLOG_PROCESS_ERROR(pIni);

    pIni->GetInteger("Debug", "Console", 0, &pConfig->bConsole);
    pIni->GetInteger("Debug", "PakFirst", 1, &pConfig->bPakFirst);

    pIni->GetInteger("Launch", "PriorExeRootPath", 0, &pConfig->bPriorExeRootPath);

    pIni->GetInteger("Main", "UTF8", 0, &pConfig->bUTF8);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pIni == NULL && szConfigFilePath && pConfig)
        {
            sprintf_s(szMessage, _countof(szMessage), "config.ini does not exist.(%s)\n", szConfigFilePath);
            MessageBoxA(hwnd, szMessage, "error", MB_OK | MB_ICONEXCLAMATION);
        }
    }
    SAFE_RELEASE(pIni);
    return nResult;
}

static int InitVersion(HWND hwnd, KJX3Version* pVersion, LPCSTR szVersionFilePath)
{
    int nRetCode = false;
    int nResult = false;
    IIniFile* pIniFile = NULL;
    char szMessage[512];

    KGLOG_PROCESS_ERROR(pVersion);
    KGLOG_PROCESS_ERROR(szVersionFilePath);

    pIniFile = g_OpenIniFile(szVersionFilePath);
    KGLOG_PROCESS_ERROR(pIniFile);

    nRetCode = pIniFile->GetString("Version", "Sword3.versionex", "", 
        pVersion->szVersionEx, _countof(pVersion->szVersionEx));
    pVersion->szVersionEx[_countof(pVersion->szVersionEx) - 1] = '\0';

    nRetCode = pIniFile->GetString("Version", "Sword3.VersionLineFullName", "", 
        pVersion->szVersionLineFullName, _countof(pVersion->szVersionLineFullName));
    KGLOG_PROCESS_ERROR(nRetCode);
    pVersion->szVersionLineFullName[_countof(pVersion->szVersionLineFullName) - 1] = '\0';

    nRetCode = pIniFile->GetString("Version", "Sword3.version", "", pVersion->szVersion, _countof(pVersion->szVersion));
    KGLOG_PROCESS_ERROR(nRetCode);
    pVersion->szVersion[_countof(pVersion->szVersion) - 1] = '\0';

    nRetCode = pIniFile->GetString("Version", "Sword3.VersionLineName", "", 
        pVersion->szVersionLineName, _countof(pVersion->szVersionLineName));
    KGLOG_PROCESS_ERROR(nRetCode);
    pVersion->szVersionLineName[_countof(pVersion->szVersionLineName) - 1] = '\0';

    nRetCode = pIniFile->GetString("Version", "Sword3.AddOnVersion", "", 
        pVersion->szAddonVersion, _countof(pVersion->szAddonVersion));
    KGLOG_PROCESS_ERROR(nRetCode);
    pVersion->szAddonVersion[_countof(pVersion->szAddonVersion) - 1] = '\0';

    nRetCode = pIniFile->GetString("Version", "Sword3.SvrListUrl", "", 
        pVersion->szServerListUrl, _countof(pVersion->szServerListUrl));
    pVersion->szServerListUrl[_countof(pVersion->szServerListUrl) - 1] = '\0';

    for (size_t i = 0; pVersion->szVersion[i] != '\0'; ++i)
    {
        if (pVersion->szVersion[i] == '-')
        {
            pVersion->szVersion[i] = '.';
        }
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pIniFile == NULL && szVersionFilePath && pVersion)
        {
            sprintf_s(szMessage, _countof(szMessage), "version.cfg does not exist.(%s)\n", szVersionFilePath);
            MessageBoxA(hwnd, szMessage, "error", MB_OK | MB_ICONEXCLAMATION);
        }
    }
    KG_COM_RELEASE(pIniFile);
    return nResult;
}

int KSO3Client::OnInit()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    HRESULT                 hr                  = S_OK;
    BOOL                    bFullScreen         = false;
    DWORD                   dw3DEngineFlag      = 0;
    IKSO3RepresentHandler*  pRepresentHandler   = NULL;
    BOOL                    bInitConsole        = false;
    BOOL                    bOleInitialize      = false;
    BOOL                    bInitSdoa           = false;
    BOOL                    bInitPackage        = false;
    BOOL                    bInit3DEngine       = false;
    BOOL                    bInitRepresent      = false;
    BOOL                    bInitUI             = false;
	BOOL                    bInitSoundShell     = false;
    BOOL                    bInitPlayerClient   = false;
    IRecorderFactory*       piRecorderFactory   = NULL;
    BOOL                    bGameWorldInitFlag  = false;
	KLoadingWindow			LoadingWindow;

	LoadingWindow.Init(GetHINSTANCE(), GetTitle(), GetIcon());
	LoadingWindow.StartLoading(26954);
	LoadingWindow.SetPercentage(0.0f, 0.0004f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    //nRetCode = GetExeFilePath(m_szExePath, _countof(m_szExePath));
    //KGLOG_PROCESS_ERROR(nRetCode);

	getcwd(m_szExePath, _countof(m_szExePath));
	strcat(m_szExePath, "\\");

    KGLogPrintf(KGLOG_INFO, "Exe path:%s\n", m_szExePath);

	g_SetRootPath();
	g_SetFilePath("");

    nRetCode = InitDllPath(m_szExePath, &m_FilePath);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = InitConfig(LoadingWindow.GetHWND(), &m_Config, m_FilePath.szConfig);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = InitVersion(LoadingWindow.GetHWND(), &m_Version, m_FilePath.szVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (m_Config.bConsole)
    {
        bInitConsole = AllocConsole();
        freopen("CONOUT$", "w+t", stdout);
    }

    if (m_Config.bPriorExeRootPath)
    {
        g_SetPriorRootPath(m_FilePath.szExePath);
    }

	SetMouseHoverTime(200);

    hr = OleInitialize(NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    bOleInitialize = true;

    nRetCode = SetCodePage(m_Config.bUTF8 ? CP_UTF8 : CODE_PAGE_GBK);
    KGLOG_PROCESS_ERROR(nRetCode);

    InitPackage(m_Config.bPakFirst);
    bInitPackage = true;

    piRecorderFactory = CreateRecorderFactoryInterface(ermNormal);
    KGLOG_PROCESS_ERROR(piRecorderFactory);

	LoadingWindow.SetPercentage(0.0004f, 0.2974f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    g_pSO3World = new KSO3World;
    KGLOG_PROCESS_ERROR(g_pSO3World);

    nRetCode = g_pSO3World->Init(piRecorderFactory);
    KGLOG_PROCESS_ERROR(nRetCode);
    bGameWorldInitFlag = true;

	LoadingWindow.SetPercentage(0.2974f, 0.3627f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    nRetCode = InitSdoa();
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitSdoa = true;

    nRetCode = Init3DEngine();
    KGLOG_PROCESS_ERROR(nRetCode);

    bInit3DEngine = true;

	LoadingWindow.SetPercentage(0.3627f, 0.3631f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    nRetCode = InitRepresent();
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitRepresent = true;

	LoadingWindow.SetPercentage(0.3631f, 0.4056f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    nRetCode = InitUI();
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitUI = true;

	LoadingWindow.SetPercentage(0.4056f, 0.4172f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    //初始化与服务端的连接
    nRetCode = g_PlayerClient.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitPlayerClient = true;

    nRetCode = InitLua();
    KGLOG_PROCESS_ERROR(nRetCode);

	LoadingWindow.SetPercentage(0.4172f, 0.5258f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    pRepresentHandler = m_pRepresent->GetRepresentHandelr();
    KGLOG_PROCESS_ERROR(pRepresentHandler);

    g_SetGameWorldUIHandler(&m_pUI->GetGameWorldUIHandler());
    g_SetGameWorldRepresentHandler(m_pRepresent->GetGameWorldRepresentHandler());

    hr = pRepresentHandler->AttachGameWorld(g_pSO3World, QuerySO3WorldClientInterface());
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pRepresentHandler->Attach3DEngine(m_p3DEngine, m_p3DEngine->Get3DModelTable(), m_p3DEngine->Get3DUI());
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pRepresentHandler->AttachUI(m_pUI, &m_pUI->GetGameWorldUIHandler());
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pRepresentHandler->InitHWND(KGWin32App::GetHWND());
    KGLOG_COM_PROCESS_ERROR(hr);

    nRetCode = m_pRepresent->Init();
    KGLOG_PROCESS_ERROR(nRetCode);

	//声音初始化要放在最后， 需要挂接到不同模块上
	bInitSoundShell = InitSoundShell();

	LoadingWindow.SetPercentage(0.5258f, 1.0f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

    m_pUI->Run();

    //设置启动时间
    m_dwStartTime = KG_GetTickCount();
    m_nLastBreatheTime = m_dwStartTime * GAME_FPS;

	LoadingWindow.SetPercentage(1.0f, 1.0f);
	KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

	while (true)
	{
		KG_PROCESS_ERROR(!LoadingWindow.IsCanceled());

		if (LoadingWindow.IsFinished())
		{
            LoadingWindow.HideWindow();

			if (::GetWindowLong(GetHWND(), GWL_STYLE) & WS_MAXIMIZE)
				::ShowWindow(GetHWND(), SW_SHOWMAXIMIZED);
			else
				::ShowWindow(GetHWND(), SW_SHOWNORMAL);

			::SetForegroundWindow(GetHWND());
			::SetFocus(GetHWND());

			LoadingWindow.EndLoading();

			break;
		}
		Sleep(10);
	}

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bInitPlayerClient)
            g_PlayerClient.UnInit();

        if (bGameWorldInitFlag)
        {
            g_pSO3World->UnInit();
            bGameWorldInitFlag = false;
        }

        if (bInitUI)
            UnInitUI();

        if (bInitRepresent)
            UnInitRepresent();

        if (bInit3DEngine)
            UnInit3DEngine();

		if (bInitSoundShell)
			UnInitSoundShell();

        if (bInitSdoa)
            UnInitSdoa();

        g_SetGameWorldUIHandler(NULL);
        g_SetGameWorldRepresentHandler(NULL);

        KG_DELETE(g_pSO3World);

        if (bInitPackage)
            UnInitPackage();

        if (bOleInitialize)
            OleUninitialize();

        if (bInitConsole)
            FreeConsole();

        KGLogPrintf(KGLOG_ERR, "%s\n", INIT_GAME_ERROR);
    }
    KG_COM_RELEASE(piRecorderFactory);
    return nResult;
}

void KSO3Client::OnExit()
{
    g_PlayerClient.UnInit();

    if (g_pSO3World)
    {
        g_pSO3World->UnInit();
    }

    UnInitUI();
    UnInitRepresent();
    UnInitSdoa();   // Sdoa资源管理有问题，光是Finalize是不能释放所有显示资源的，必须可耻的Release，跟Sdoa交涉未果，本来应该放到UnInitSoundShell后面
    UnInit3DEngine();
	UnInitSoundShell();

    g_SetGameWorldUIHandler(NULL);
    g_SetGameWorldRepresentHandler(NULL);

    KG_DELETE(g_pSO3World);

    UnInitPackage();

    OleUninitialize();

    FreeConsole();
}

int KSO3Client::InitSdoa()
{
    int nRetCode = false;
    int nResult = false;
    int nInitModule = false;
    int nInitJX3Sdoa = false;
    HMODULE hmodule = NULL;
    unsigned int uCodePage = 0;

    nRetCode = strcmp(m_Version.szVersionEx, "snda") != 0;
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = !g_IsFileExist(m_FilePath.szSdoaGameWidget);
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = m_ModuleSdoaGameWidget.Init(m_FilePath.szSdoaGameWidget);
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitModule = true;

    hmodule = m_ModuleSdoaGameWidget.GetDllHandle();

    uCodePage = GetCodePage();

    nRetCode = InitJX3Sdoa(&m_JX3Sdoa, hmodule, ATL::CA2W(m_Version.szVersion, uCodePage));
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitJX3Sdoa = true;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nInitJX3Sdoa)
        {
            UnInitJX3Sdoa(&m_JX3Sdoa);
        }

        if (nInitModule)
        {
            m_ModuleSdoaGameWidget.UnInit();
        }
    }
    return nResult;
}

void KSO3Client::UnInitSdoa()
{
    UnInitJX3Sdoa(&m_JX3Sdoa);
    m_ModuleSdoaGameWidget.UnInit();
}

int KSO3Client::Init3DEngine()
{
    typedef HRESULT (*fnGet3DEngineInterface)(void**);

    int nResult = false;
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    fnGet3DEngineInterface pfnGet3DEngineInterface = NULL;
    IKG3DUI* p3DUI = NULL;
    HWND hWnd = NULL;
    BOOL bInitModule3DEngine = FALSE;
    BOOL bInit3DEngine = FALSE;
    BOOL bInitJX33DUICallback = FALSE;
    BOOL bSet3DDeviceCallback = FALSE;

    hWnd = KGWin32App::GetHWND();
    KGLOG_PROCESS_ERROR(hWnd);

    nRetCode = m_Module3DEngine.Init(m_FilePath.szKG3DEngine);
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitModule3DEngine = TRUE;

    pfnGet3DEngineInterface = (fnGet3DEngineInterface)GetProcAddress(m_Module3DEngine.GetDllHandle(), "Get3DEngineInterface");
    KGLOG_PROCESS_ERROR(pfnGet3DEngineInterface);

    hr = pfnGet3DEngineInterface((void**)&m_p3DEngine);
    KGLOG_COM_PROCESS_ERROR(hr);
    KGLOG_PROCESS_ERROR(m_p3DEngine);

    p3DUI = m_p3DEngine->Get3DUI();
    KGLOG_PROCESS_ERROR(p3DUI);

    nRetCode = m_JX33DDeviceCallback.Init(&m_JX3Sdoa);
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitJX33DUICallback = TRUE;

    hr = p3DUI->SetDeviceCallback(&m_JX33DDeviceCallback);
    KGLOG_COM_PROCESS_ERROR(hr);

    bSet3DDeviceCallback = TRUE;

    hr = m_p3DEngine->Init(0, KG3DENG_CLIENT, hWnd, hWnd);
    KGLOG_COM_PROCESS_ERROR(hr);

    bInit3DEngine = TRUE;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bInit3DEngine)
        {
            m_p3DEngine->UnInit();
            m_p3DEngine = NULL;
        }
        if (bSet3DDeviceCallback)
        {
            p3DUI->SetDeviceCallback(NULL);
        }
        if (bInitJX33DUICallback)
        {
            nRetCode = m_JX33DDeviceCallback.UnInit();
            KGLOG_CHECK_ERROR(nRetCode);
        }
        if (bInitModule3DEngine)
        {
            m_Module3DEngine.UnInit();
        }
    }
    return nResult;

}

void KSO3Client::UnInit3DEngine()
{
    int nRetCode = false;

    nRetCode = m_JX33DDeviceCallback.UnInit();
    KGLOG_CHECK_ERROR(nRetCode);

    if (m_p3DEngine)
    {
        m_p3DEngine->UnInit();
        m_p3DEngine = NULL;
    }
    m_Module3DEngine.UnInit();
}

int KSO3Client::InitRepresent()
{
    int nResult = false;
    int nRetCode = false;
    fnCreateSO3Represent pfnCreateSO3Represent = NULL;
    BOOL bInitModuleRepresent = FALSE;
    BOOL bCreateSO3Represent = FALSE;

    nRetCode = m_ModuleRepresent.Init(m_FilePath.szJX3Represent);
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitModuleRepresent = TRUE;

    pfnCreateSO3Represent = (fnCreateSO3Represent)GetProcAddress(m_ModuleRepresent.GetDllHandle(), FN_CREATE_SO3REPRESENT);
    KGLOG_PROCESS_ERROR(pfnCreateSO3Represent);

    m_pRepresent = pfnCreateSO3Represent();
    KGLOG_PROCESS_ERROR(m_pRepresent);

    bCreateSO3Represent = TRUE;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bCreateSO3Represent)
        {
            SAFE_RELEASE(m_pRepresent);
        }
        if (bInitModuleRepresent)
        {
            m_ModuleRepresent.UnInit();
        }
    }
    return nResult;
}

void KSO3Client::UnInitRepresent()
{
    SAFE_RELEASE(m_pRepresent);
    m_ModuleRepresent.UnInit();
}

int KSO3Client::InitUI()
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    fnCreateSO3UI pfnCreateSO3UI = NULL;
    SO3UI_INIT_PARAM Param;
    BOOL bInitModuleUI = FALSE;
    BOOL bInitUI = FALSE;

    KGLOG_PROCESS_ERROR(m_p3DEngine && "init 3d engine first!");
    KGLOG_PROCESS_ERROR(m_pRepresent && "init represent first!");

    strncpy_s(Param.szKGUIDllPath, _countof(Param.szKGUIDllPath), m_FilePath.szKGUI, _TRUNCATE);
    strncpy_s(Param.szJX3InteractionDllPath, _countof(Param.szJX3InteractionDllPath), m_FilePath.szJX3Interaction, _TRUNCATE);
    strncpy_s(Param.szKLVideoDllPath, _countof(Param.szKLVideoDllPath), m_FilePath.szKLVideo, _TRUNCATE);
    strncpy_s(Param.szIceClientLibDllPath, _countof(Param.szIceClientLibDllPath), m_FilePath.szIceClientlib, _TRUNCATE);
    strncpy_s(Param.szConfigPath, _countof(Param.szConfigPath), m_FilePath.szConfig, _TRUNCATE);
    strncpy_s(Param.szVersionPath, _countof(Param.szVersionPath), m_FilePath.szVersion, _TRUNCATE);

    nRetCode = m_ModuleUI.Init(m_FilePath.szJX3UI);
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitModuleUI = TRUE;

    pfnCreateSO3UI = (fnCreateSO3UI)GetProcAddress(m_ModuleUI.GetDllHandle(), FN_CREATE_SO3UI);
    KGLOG_PROCESS_ERROR(pfnCreateSO3UI);

    m_pUI = pfnCreateSO3UI();
    KGLOG_PROCESS_ERROR(m_pUI);

    Param.hInst = KGWin32App::GetHINSTANCE();
    Param.hWnd = KGWin32App::GetHWND();
    Param.fnAttribute = AttributeStringToID;
    Param.fnRequire = RequireStringToID;
    Param.p3DEngineManager = m_p3DEngine;
    Param.p3DModelManager = m_p3DEngine->Get3DModelTable();
    Param.p3DUI = m_p3DEngine->Get3DUI();
    Param.pRepresent = m_pRepresent->GetRepresentHandelr();
	Param.pSoundShellHolder = this;
    Param.phwndEmbedWebPage = &m_hwndEmbedWebPage;
    Param.uCodePage = GetCodePage();

    nRetCode = (BOOL)m_pUI->Init(&Param);
    KGLOG_PROCESS_ERROR(nRetCode);

    bInitUI = TRUE;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bInitUI)
        {
            m_pUI->UnInit();
        }
        if (m_pUI)
        {
            m_pUI->Exit();
            m_pUI = NULL;
        }
        if (bInitModuleUI)
        {
            m_ModuleUI.UnInit();
        }
    }
    return nResult;
}

void KSO3Client::UnInitUI()
{
    if (m_pUI)
    {
        m_pUI->UnInit();
        m_pUI->Exit();
        m_pUI = NULL;
    }

    m_ModuleUI.UnInit();
}

BOOL KSO3Client::InitSoundShell()
{
	int nResult = false;
	int nRetCode = false;
	fnCreate3DSoundShell pfnCreateSound = NULL;
	BOOL bInitModuleSound = FALSE;
	BOOL bCreateSound = FALSE;

	SAFE_RELEASE(m_pSoundShell);
	m_ModuleSound.UnInit();

    nRetCode = m_ModuleSound.Init(m_FilePath.szKG3DSound);
	KGLOG_PROCESS_ERROR(nRetCode);

	bInitModuleSound = TRUE;

	pfnCreateSound = (fnCreate3DSoundShell)GetProcAddress(m_ModuleSound.GetDllHandle(), FN_CREATE_3DSOUND_SHELL);
	KGLOG_PROCESS_ERROR(pfnCreateSound);

	m_pSoundShell = pfnCreateSound();
	KGLOG_PROCESS_ERROR(m_pSoundShell);

	m_pSoundShell->Init(GetHWND());
	
	UpdateSoundShell();
	bCreateSound = TRUE;

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (bCreateSound)
		{
			SAFE_RELEASE(m_pSoundShell);
		}
		if (bInitModuleSound)
		{
			m_ModuleSound.UnInit();
		}
	}
	return nResult;
}

void KSO3Client::UpdateSoundShell()
{
	if (m_pRepresent)
	{
		m_pRepresent->SetSoundShell(m_pSoundShell);
	}

	if (m_p3DEngine)
	{
		m_p3DEngine->SetSoundShell(m_pSoundShell);
	}

	if (m_pUI)
	{
		m_pUI->SetSoundShell(m_pSoundShell);
	}
}

void KSO3Client::UnInitSoundShell()
{
	SAFE_RELEASE(m_pSoundShell);
	m_ModuleSound.UnInit();
	UpdateSoundShell();
}

void KSO3Client::InitPackage(BOOL bPakFirst)
{
    g_SetFindFileMode(bPakFirst);

    if (g_IsFileExist(m_FilePath.szPackageList))
    {
        g_LoadPackageFiles(m_FilePath.szPackageList, "SO3Client");
    }
}

void KSO3Client::UnInitPackage()
{
    g_ClearPackageFiles();
}

int KSO3Client::InitLua()
{
    int nRetCode = false;
    int nResult = false;
    IKGUI* pUI = NULL;
    KLuaFunc *pFunc = NULL;
    luaL_reg const* pRegs = NULL;
    int nCount = 0;

    KG_ASSERT_EXIT(m_pUI);

    pUI = m_pUI->GetIKGUI();
    KGLOG_PROCESS_ERROR(pUI);

    pUI->RegisterConstList(g_LuaConstList);

    nCount = KScriptFuncList::GetBaseFuncList(pFunc);
    pUI->RegisterFunctions(pFunc, nCount);

    nCount = KScriptFuncList::GetShopFuncList(pFunc);
    pUI->RegisterFunctions(pFunc, nCount);

    nCount = KScriptFuncList::GetLoginFuncList(pFunc);
    pUI->RegisterFunctions(pFunc, nCount);

    pRegs = GetJX3SdoaFuncList();
    KGLOG_PROCESS_ERROR(pRegs);

    pUI->RegisterFunctions(pRegs);

    nResult = true;
Exit0:
    return nResult;
}

int KSO3Client::Reset()
{
    int nRetCode = false;
    int nResult = false;

    m_pUI->Reset();

    nRetCode = InitLua();
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pUI->Run();

    nResult = true;
Exit0:
    return nResult;
}

