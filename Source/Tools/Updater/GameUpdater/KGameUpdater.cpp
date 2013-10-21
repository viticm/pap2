#include "StdAfx.h"
#include <Shlwapi.h>
#include <io.h>
#include "KLastError.h"
#include "resource.h"
#include "KGameUpdater.h"
#include "KVersionNumber.h"
#include "KProcess.h"
#include "KSystemCheck.h"
#include "KGU_RemoveRedundantFiles.h"

#ifdef _TEST
	#include "KUpdateGlobalEvent.h"
#endif

const int WINDOW_HEIGHT = 908;
const int WINDOW_WIDTH  = 580;

KGameUpdater::KGameUpdater(void)
{
	m_FlashCallFunctions.insert(std::make_pair(std::string("Startgame"), &KGameUpdater::_OnStartGame));
	m_FlashCallFunctions.insert(std::make_pair(std::string("Configgame"), &KGameUpdater::_OnConfigGame));
	m_FlashCallFunctions.insert(std::make_pair(std::string("CloseWindow"), &KGameUpdater::_OnQuitUpdate));

	m_eUpdateStatus   = UPDATE_STATUS_NO_NEW_VERSION;
}

KGameUpdater::~KGameUpdater(void)
{
}

int KGameUpdater::FlashCallback(const tstring &strFunName, const PARAM_LIST &ParamList)
{
	int nResult  = false;
	FLASH_CALL_MAP::iterator itor;

	itor = m_FlashCallFunctions.find(strFunName);
	KG_PROCESS_ERROR(itor != m_FlashCallFunctions.end());
	(this->*itor->second)();

	if (KLastError::GetErrorCode() != UPDATE_ERR_OK)
	{
		_CallFlash("SetError", (int)KLastError::GetErrorCode());
	}

	nResult = true;
Exit0:	
	return nResult;
}

int KGameUpdater::Init(const TCHAR cszSwfFileName[])
{
	int   nRetCode = false;
	int   nResult  = false;
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);	
    int x = (nScreenWidth - WINDOW_HEIGHT) / 2;
    int y = (nScreenHeight - WINDOW_WIDTH) / 2;
    HINSTANCE hInstance = NULL;
	KGameupdaterText Text;
    CSystemInfo SysInfo;
    TCHAR szInfo[256];
    TCHAR szVersion[256];

    KGLOG_PROCESS_ERROR(cszSwfFileName);
    KGLOG_PROCESS_ERROR(cszSwfFileName[0]);

    hInstance = GetModuleHandle(NULL);
    KGLOG_PROCESS_ERROR(hInstance);

    m_bmp16BPPBack = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	KGLOG_CHECK_ERROR(m_bmp16BPPBack);

	nRetCode = m_FlashWnd.Create(
		cszSwfFileName, this, tstring(_T("JX3Gameupdater")), tstring(Text.GetText(IDS_GAMEUPDATER_TITLE)),
		x, y, WINDOW_HEIGHT, WINDOW_WIDTH, hInstance, IDI_SMALL, m_bmp16BPPBack, 
		(m_FlashWnd.GetBPP() > 16) ? RGB(0x66, 0x33, 0x33) : RGB(0x63, 0x30, 0x31)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

    m_Config.LoadConfig();

    _stprintf(szInfo, _T("%d&%d&%s"), SysInfo.GetCpuCoreNum(), m_FlashWnd.GetBPP(), m_Config.GetBulletinURL());
	_CallFlash("SetComputerInfo", szInfo);

	_sntprintf(szVersion, MAX_PATH, _T("%s %s"), m_Config.GetCurrentVersion(), m_Config.GetVersionName());
	_CallFlash("SetVersion", szVersion);

    m_ClientReport.ReportClientInfo(m_Config.GetCurrentVersion());

	nResult = true;
Exit0:
	_CallFlash("SetError", (int)KLastError::GetErrorCode());
	return nResult;
}

int KGameUpdater::UnInit()
{
	m_FlashWnd.Destroy();

	if (m_bmp16BPPBack)
	{
		DeleteObject(m_bmp16BPPBack);
		m_bmp16BPPBack = NULL;
	}
	return true;
}

int KGameUpdater::AutoUpdate(HANDLE hSucessSelfUpdateEvent, HANDLE hFailedSelfUpdateEvent)
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(
        (hSucessSelfUpdateEvent && hFailedSelfUpdateEvent) ||
        (hSucessSelfUpdateEvent == NULL && hFailedSelfUpdateEvent == NULL)
    );

    nRetCode = m_Update.GetLastVersion(
        m_Config.GetRemotePath(),
        m_Config.GetUpdatePath(),
        m_Config.GetVersionLineName(),
        this,
        hSucessSelfUpdateEvent,
        hFailedSelfUpdateEvent
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    m_FlashWnd.ShowWnd();
    
    nResult = true;
Exit0:
    return nResult;
}

int KGameUpdater::ManualUpdate(const TCHAR cszManualUpdateFileName[])
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(cszManualUpdateFileName);
    KGLOG_PROCESS_ERROR(cszManualUpdateFileName[0]);

    nRetCode = m_Update.ManualUpdate(cszManualUpdateFileName, m_Config.GetCurrentVersion(), m_Config.GetAppPath(), this);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_FlashWnd.ShowWnd();
    
    nResult = true;
Exit0:
    return nResult;
}

int KGameUpdater::_OnQuitUpdate()
{
	int              nRetCode = false;
	int              nResult  = false;
	KGameupdaterText Text;

	if (m_eUpdateStatus == UPDATE_STATUS_DOWNLOADING_PACK && KLastError::GetErrorCode() == UPDATE_ERR_OK)
	{
		nRetCode = MessageBox(m_FlashWnd.GetHWND(), Text.GetText(IDS_QUIT_WARNNING).c_str(), Text.GetText(IDS_WARNNING_DIALOG_TITLE).c_str(), MB_YESNO | MB_ICONQUESTION);
		if (nRetCode == IDNO)
			goto Exit0;
	}

	if (m_eUpdateStatus != UPDATE_STATUS_CLIENT_UPGRADING || KLastError::GetErrorCode() != UPDATE_ERR_OK)
	{
		m_Update.Abort();

		m_FlashWnd.Destroy();
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGameUpdater::_OnStartGame()
{
	int nRetCode = false;
	int nResult  = false;
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInfo;
	std::string strApp = std::string(m_Config.GetAppPath()) + CLIENT_EXE_NAME;

	ZeroMemory(&StartInfo, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);

	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	nRetCode = _taccess(strApp.c_str(), 0);
	if (nRetCode == -1)
	{
		KGameupdaterText Text;
		KGLogPrintf(KGLOG_ERR, "start game err:%s", strApp.c_str());
		MessageBox(0, (Text.GetText(IDS_CANNOT_RUN) + strApp).c_str(), Text.GetText(IDS_WARNNING_DIALOG_TITLE).c_str(), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	strApp = std::string("\"") + strApp + "\" " + CLIENT_START_STRING;
	nRetCode = CreateProcess(
		NULL,
		(LPTSTR)strApp.c_str(),
		NULL, 
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL, 
		&StartInfo, 
		&ProcessInfo
		);
	KGLOG_PROCESS_ERROR(nRetCode && "StartGame");
	_OnQuitUpdate();

	nResult = true;
Exit0:	
	return nResult;
}

int KGameUpdater::_OnConfigGame()
{
	int nRetCode = false;
	int nResult  = false;

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInfo;
	std::string strApp = std::string(m_Config.GetAppPath()) + CONFIG_EXE_NAME;

	ZeroMemory(&StartInfo, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);

	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	nRetCode = _taccess(strApp.c_str(), 0);
	if (nRetCode == -1)
	{
		KGameupdaterText Text;
		KGLogPrintf(KGLOG_ERR, "config game err:%s", strApp.c_str());
		MessageBox(m_FlashWnd.GetHWND(), (Text.GetText(IDS_CANNOT_RUN) + strApp).c_str(), Text.GetText(IDS_WARNNING_DIALOG_TITLE).c_str(), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	nRetCode = CreateProcess(
		NULL,
		(LPTSTR)((std::string("\"") + strApp + std::string("\"")).c_str()),
		NULL, 
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL, 
		&StartInfo, 
		&ProcessInfo
		);
	KGLOG_PROCESS_ERROR(nRetCode && "Run Config");

	nResult = true;
Exit0:
	return nResult;
}

int KGameUpdater::OnDownlodProgress(float fDownloadPercent)
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szPercent[32];

	KG_PROCESS_ERROR(fDownloadPercent >= 0.0f);
	KG_PROCESS_ERROR(fDownloadPercent <= 100.0f);

	_stprintf(szPercent, _T("%.3f"), fDownloadPercent);

	_CallFlash("ProgressPercent", szPercent);

	nResult = true;
Exit0:
	return nResult;
}

int KGameUpdater::OnError(UPDATE_FLASH_SHOW_ERROR_MESSAGES eErrorCode)
{
	_CallFlash("SetError", (int)eErrorCode);
	KLastError::SetErrorCode((int)eErrorCode);

#ifdef _TEST
	if (eErrorCode != UPDATE_ERR_OK)
		KUpdateGlobalEvent::SetSignal();
#endif
	
	return true;
}

int KGameUpdater::OnStateChange(UPDATE_PROGRESS_STATUS eStatusID)
{
	if (eStatusID == UPDATE_STATUS_CLIENT_UPGRADE_COMPLETE)
	{
		TCHAR szVersion[MAX_PATH];

		m_Config.ReLoadVersion();
		m_Config.SaveVersionAndPath();
		m_ClientReport.ReportUpdateInfo(m_Config.GetCurrentVersion());

		_sntprintf(szVersion, MAX_PATH, _T("%s %s"), m_Config.GetCurrentVersion(), m_Config.GetVersionName());
		_CallFlash("SetVersion", szVersion);
	}

    if (
        eStatusID == UPDATE_STATUS_CLIENT_UPGRADE_COMPLETE ||
        eStatusID == UPDATE_STATUS_NO_NEW_VERSION
    )
    {
        KGU_CheckGameEnvironment();
        KGU_RemoveRedundantFiles();
    }

    m_eUpdateStatus = eStatusID;
    _CallFlash("SetStatus", eStatusID);

	return true;
}

int KGameUpdater::OnLastVersion(const TCHAR szLastVersion[])
{
	int nRetCode = false;
	int nResult  = false;
	KVersionNumber LastVer(szLastVersion);
	KVersionNumber NowVer(m_Config.GetCurrentVersion());
	
	if (LastVer > NowVer)
	{
		KGameupdaterText Text;
		std::string strProcess;
		KProcess Proc;
		int nPrecssNum = 0;

		strProcess = m_Config.GetAppPath();
		strProcess += CLIENT_EXE_NAME;
		while (TRUE)
		{
			Proc.FindProcess(strProcess.c_str(), &nPrecssNum);
			if (nPrecssNum == 0)
			{
				nRetCode = m_Update.AutoUpdate(m_Config.GetRemotePath(), m_Config.GetCurrentVersion(), m_Config.GetVersionLineName(), m_Config.GetAppPath(), this);
				KGLOG_PROCESS_ERROR(nRetCode);				
				break;
			}
			else 
			{
				nRetCode = MessageBox(m_FlashWnd.GetHWND(), Text.GetText(IDS_FOUND_NEW_VERSION).c_str(), Text.GetText(IDS_WARNNING_DIALOG_TITLE).c_str(), MB_ICONEXCLAMATION | MB_ICONWARNING | MB_RETRYCANCEL);
				if (nRetCode == IDCANCEL)
				{
					m_FlashWnd.Destroy();			
					break;
				}
			}			
		}
	}
	else
	{
		OnStateChange(UPDATE_STATUS_NO_NEW_VERSION);
	}
	nResult = true;
Exit0:
	return nResult;
}

int KGameUpdater::_CallFlash(const TCHAR cpszCallBackName[], int nReturnCode)
{
	int nResult  = false;
	char szParam[10] = {0};

	KGLOG_PROCESS_ERROR(cpszCallBackName);

	itoa(nReturnCode, szParam, 10);
	_CallFlash(cpszCallBackName, szParam);

	nResult = true;
Exit0:
	return nResult;
}

int KGameUpdater::_CallFlash(const TCHAR cpszCallBackName[], const TCHAR pszRetString[])
{
	int nResult  = false;
	PARAM_LIST Params;

	KGLOG_PROCESS_ERROR(cpszCallBackName);
	KGLOG_PROCESS_ERROR(pszRetString);

	Params.push_back(tstring(pszRetString));

	m_FlashWnd.SendCommandToFlash(tstring(cpszCallBackName), Params);

	nResult = true;
Exit0:
	return nResult;
}

