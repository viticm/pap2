// GameConfig.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "GameConfig.h"
#include "GameConfigDlg.h"
#include "KProcess.h"
#include "kglog.h"
#include "config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// KGameConfigApp

BEGIN_MESSAGE_MAP(KGameConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// KGameConfigApp 构造

KGameConfigApp::KGameConfigApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 KGameConfigApp 对象

KGameConfigApp theApp;


// KGameConfigApp 初始化

BOOL KGameConfigApp::InitInstance()
{
	int nRetCode = false;
	int nResult = FALSE;
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	KGameConfigDlg dlg;
	
	KGLOG_PARAM KGParm = {"logs", "GameConfig", KGLOG_OPTION_FILE, 1024* 10};

	KGLogInit(KGParm, NULL);

	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	nRetCode = CanRun();
	if (!nRetCode)
	{
		return FALSE;
	}

	//如果是FirstRun 那么就检测内存是否高于1G，如果不是，那么就设定为低配置
	if (theApp.m_lpCmdLine[0] != _T('\0'))
	{
		nRetCode = _tcsnicmp(theApp.m_lpCmdLine, _T("FirstRun"), _tcslen(_T("FirstRun")));
		if (nRetCode == 0)
		{
			nRetCode = _CheckSystemMemory();
			KGLOG_CHECK_ERROR(nRetCode);
			KG_PROCESS_SUCCESS(true);
		}
	}

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

Exit1:
	nResult = TRUE;
Exit0:

	KGLogUnInit(NULL);
	return nResult;
}

int KGameConfigApp::CanRun()
{
	int nRetCode = FALSE;
	int nResult  = FALSE;

	TCHAR *pSlash = NULL;
	TCHAR szAppPath[MAX_PATH];
	int nPrecssNum = 0;
	CString strProcess;
	KProcess Proc;

	nRetCode = GetModuleFileName(NULL, szAppPath, sizeof(szAppPath) / sizeof(TCHAR));
	KGLOG_PROCESS_ERROR(nRetCode);
	pSlash = _tcsrchr(szAppPath, _T('\\'));
	KGLOG_PROCESS_ERROR(pSlash);
	*(pSlash + 1) = _T('\0');

	strProcess = szAppPath;
	strProcess += "GameConfig.exe";

	while (TRUE)
	{
		nRetCode = Proc.FindProcess(strProcess, &nPrecssNum);
		if (!nRetCode)
			break;

		if (nPrecssNum > 1)
			goto Exit0;

		break;
	}

	nResult = TRUE;
Exit0:
	return nResult;
}

int KGameConfigApp::_CheckSystemMemory()
{
	int   nResult                   = false;
	int   nRetCode                  = false;
	HKEY  hKey                      = NULL;
	DWORD dwDisp                    = 0;	
	int   nMemSize                  = 0;
	FILE* pFile                     = NULL;
    TCHAR *pchLastSlash = NULL;
	TCHAR szApplicationPath[MAX_PATH]       = {0};
	TCHAR szConfigFileName[MAX_PATH]        = {0};
	TCHAR szTempConfigFileName[MAX_PATH]    = {0};
	TCHAR szVersionCfgFileName[MAX_PATH]    = {0};
	TCHAR szVerLineName[MAX_PATH]           = {0};
	TCHAR szRegKey[MAX_PATH]                = {0};
	MEMORYSTATUSEX statex;

    nRetCode = GetModuleFileName(NULL, szApplicationPath, _countof(szApplicationPath));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationPath));

    pchLastSlash = _tcsrchr(szApplicationPath, _T('\\'));
    KGLOG_PROCESS_ERROR(pchLastSlash);
    *(pchLastSlash + 1) = _T('\0');

	nRetCode = _sntprintf(
        szConfigFileName, _countof(szConfigFileName), _T("%s%s"), szApplicationPath, _T("config.ini")
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szConfigFileName));

	nRetCode = _sntprintf(
        szVersionCfgFileName, _countof(szVersionCfgFileName), _T("%s%s"),
        szApplicationPath, _T("version.cfg")
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szVersionCfgFileName));

	nRetCode = _sntprintf(
        szTempConfigFileName, _countof(szTempConfigFileName), _T("%s%s"), szConfigFileName, _T(".bak")
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szTempConfigFileName));

	nRetCode = _GetCurrentVersionPath(szVersionCfgFileName, szVerLineName, _countof(szVerLineName));
	KGLOG_PROCESS_ERROR(nRetCode);

	memset(&statex, 0, sizeof(MEMORYSTATUSEX));
	statex.dwLength = sizeof(statex);

	nRetCode = GlobalMemoryStatusEx(&statex);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nMemSize = (int)((statex.ullTotalPhys) / 1024) / 1024;

	if (nMemSize < 1030)
	{
		nRetCode = _tremove(szTempConfigFileName);
		pFile = _tfopen(szTempConfigFileName, _T("a"));
		KGLOG_PROCESS_ERROR(pFile);

		nRetCode = fwrite(CONFIG_INI, 1, sizeof(CONFIG_INI), pFile);
		KGLOG_PROCESS_ERROR(nRetCode == sizeof(CONFIG_INI));

		fclose(pFile);
		pFile = NULL;

		nRetCode = _taccess(szConfigFileName, 0);
		if (nRetCode != -1)
		{
			DWORD dwFileAttribute = 0;

			dwFileAttribute = GetFileAttributes(szConfigFileName);
			dwFileAttribute &= ~dwFileAttribute;
			SetFileAttributes(szConfigFileName, dwFileAttribute);

			nRetCode = _tremove(szConfigFileName);
			if (nRetCode == -1)
			{
				for (int nTry = 0; nTry < 3; ++nTry)
				{
					if (nRetCode == 0)
						break;

					nRetCode = _tremove(szConfigFileName);
				}
			}
			KGLOG_PROCESS_ERROR(nRetCode == 0);
		}

		nRetCode = _trename(szTempConfigFileName, szConfigFileName);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	_sntprintf(szRegKey, MAX_PATH, _T("%s\\%s"), _T("SOFTWARE\\Kingsoft\\JX3"), szVerLineName);

	nRetCode = ::RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
	KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nRetCode = ::RegSetValueEx(hKey, _T("CONFIGGAME"), 0, REG_SZ, (BYTE *)_T("2"), (DWORD)(_tcslen(_T("2")) * sizeof(TCHAR)));
	KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nResult = true;
Exit0:
	if (hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}

	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return nResult;
}

int KGameConfigApp::_GetCurrentVersionPath(TCHAR szVersionPath[], TCHAR* szRetVersinPath, unsigned uBuffSize)
{
	int   nRetCode                    = false;
	int   nResult                     = false;
	FILE* pFile                       = NULL;
	TCHAR szBuffer[MAX_PATH]          = {0};
	TCHAR szVersionLineName[MAX_PATH] = {0};
	TCHAR szVersionEx[MAX_PATH]       = {0};

	KGLOG_PROCESS_ERROR(szVersionPath);
	KGLOG_PROCESS_ERROR(szVersionPath[0]);
	KGLOG_PROCESS_ERROR(szRetVersinPath);
	KGLOG_PROCESS_ERROR(uBuffSize > 0);

	nRetCode = _taccess(szVersionPath, 0);
	KGLOG_PROCESS_ERROR((nRetCode != -1) && _T("can't find version.cfg"));

	pFile = _tfopen(szVersionPath, _T("r"));
	KGLOG_PROCESS_ERROR(pFile);

	while (true)
	{
		size_t uLength = 0;
		
		TCHAR *pRet = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pFile);
		if (!pRet)
		{
			if (ferror(pFile))
				KGLOG_PROCESS_ERROR(false && _T("ferror(pFile)"));
			else
				break;
		}
		szBuffer[sizeof(szBuffer) / sizeof(TCHAR) - 1] = _T('\0');

		_StringTrim(szBuffer);

		if (szBuffer[0] == _T('#'))
			continue;
		
		uLength = _tcslen(_T("Sword3.VersionLineName"));
		nRetCode = _tcsncicmp(szBuffer, _T("Sword3.VersionLineName"), uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(szVersionLineName, szBuffer + uLength + 1, sizeof(szVersionLineName) / sizeof(TCHAR));
			szVersionLineName[sizeof(szVersionLineName) / sizeof(TCHAR) - 1] = _T('\0');            
		}

		uLength = _tcslen(_T("Sword3.versionex"));
		nRetCode = _tcsncicmp(szBuffer, _T("Sword3.versionex"), uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(szVersionEx, szBuffer + uLength + 1, sizeof(szVersionEx) / sizeof(TCHAR));
			szVersionEx[sizeof(szVersionEx) / sizeof(TCHAR) - 1] = _T('\0');
		}
	}
	
	KGLOG_PROCESS_ERROR(szVersionLineName[0] != _T('\0') && _T("VersionLineName is empty?"));

	if (szVersionEx[0] != _T('\0'))
	{
		_tcscat(szVersionLineName, _T("_"));
		_tcscat(szVersionLineName, szVersionEx);
	}

	unsigned uLen = _tcslen(szVersionLineName);
	KGLOG_PROCESS_ERROR(uBuffSize > uLen);

	_tcsncpy(szRetVersinPath, szVersionLineName, uLen);

	nRetCode = true;
Exit0:
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return nRetCode;
}

void KGameConfigApp::_StringTrim(TCHAR szString[])
{
	TCHAR *pSrc = szString;
	TCHAR *pDes = szString;

	while (pSrc && *pSrc)
	{
		if (
			(*pSrc != _T(' '))  &&
			(*pSrc != _T('\n')) &&
			(*pSrc != _T('\r')) &&
			(*pSrc != _T('\t'))
			)
		{
			*pDes = *pSrc;
			++pDes;
		}
		++pSrc;
	}
	*pDes = _T('\0');
}