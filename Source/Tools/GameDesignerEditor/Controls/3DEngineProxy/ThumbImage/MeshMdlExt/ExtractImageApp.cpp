#include "StdAfx.h"
#include "ExtractImageApp.h"
//#include "KGImage.h"
#include <psapi.h>

#include <shlobj.h>
#include <exdisp.h>
#include <shlwapi.h>

#define MESH_MDL_BMP_FILE_NAME _T("c:\\mesh_mdl.bmp")
//建立两个窗口，一个用来发送文件名称，另一个接受文件名称，并处理得到文件的缩略图，然后回传

//引擎窗口名称
#define EngineWindowName "_EngineWindowName_For_Mesh_"

#define IDTIFY_SHARE_FILE_MAPPING_NAME "my_shared_memory_with_3dengine"

#define MAX_SHARE_SIZE 4096

#define MAX_ENGINE_PROCESS_WORK_SIZE 1024*100
#define MAX_ENGINE_PROCESS_PAGE_SIZE 1024*60

#define WM_TELL_HWND			WM_USER + 100
#define WM_SEND_MESH_FILE_NAME	WM_USER + 101
#define WM_CREATE_BMP_OK		WM_USER + 102
#define WM_ASK_HWND				WM_USER + 103
#define WM_REINIT_ENGINE		WM_USER + 104
#define WM_SEND_ANI_FILE_NAME	WM_USER	+ 105
#define WM_ANIMATION_FILE		WM_USER + 106
#define WM_USER_RERENDERBMP		WM_USER + 107

//引擎进程
PROCESS_INFORMATION g_Engine_Process_Information = {0};

//映射文件句柄
HANDLE g_hMapFile = NULL;

//共享内存地址
void *g_pShareBuffer = NULL;

//引擎的窗口句柄
HWND g_hwnd_Engine = NULL;

//初始化映射文件
BOOL InitShareMemory();

//启动引擎程序
BOOL StartEngineProcess(LPCSTR lpEngineAppName);

//发送“文件名称”
BOOL SendFileName(LPCSTR szFileName);

//判断引擎程序是否存在
BOOL IsEngineProcessExist(HANDLE);

//获取引擎程序的窗口句柄
HWND GetEngineWnd();

//是否初始化的Log
BOOL g_bInitLog;

//初始化全局
BOOL InitGlobal();

//是否初始化全局
BOOL g_bInitGloable = FALSE;


//初始化日志
BOOL InitLog();

//从注册表读取临界内存值
STDAPI SetMaxMemory();

//注册表读取3dengine.exe路径
BOOL QueryEngineAppPath(char szEngineAppPath[]);

//获取进程的内存使用和虚拟内存大小
BOOL GetWorkingSetSizeAddPageFileSize(HANDLE hProcess, PSIZE_T lpWorkSetSize, PSIZE_T lpPageFileSize);


//检查引擎进程占用内存，太大则关闭引擎进程
void CheckEngineProcessMemory();

void Debug_MessageBox_Info(LPCSTR info);


//3dengine.exe引擎程序名称
TCHAR g_szEngineAppName[MAX_PATH] = "3dengine.exe";

//是否设置了3dengine.exe的路径
BOOL g_bInitEngineAppPath = FALSE;
//
TCHAR g_szLastPath[MAX_PATH];

BOOL CALLBACK EnumWindowsProc(HWND hwnd,
							  LPARAM lParam
							  );

BOOL GetWndModuleFileName(HWND hwnd, LPSTR lpFileName);

void CALLBACK RecalcText(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

int GetMDLFile(char* pchPath);

void CallRander();

SIZE_T gs_MaxWorkSetSize = 1024 * 100;
SIZE_T gs_MaxPageSize = 1024 * 100;



CExtractImageApp::CExtractImageApp(void)
{
}

CExtractImageApp::~CExtractImageApp(void)
{
}


// CExtractImageApp
BOOL CExtractImageApp::InitInstance()
{
	BOOL bResult = TRUE;
	InitGlobal();
	SetTimer(NULL, 1, 100, RecalcText);
	return bResult;
}

int CExtractImageApp::ExitInstance()
{
	/*if (g_hwnd_Engine)
	{
		::PostMessage(g_hwnd_Engine, WM_DESTROY, 0, NULL);
	}*/
	if (g_pShareBuffer)
	{
		::UnmapViewOfFile(g_pShareBuffer);
	}
	if (g_hMapFile)
	{
		CloseHandle(g_hMapFile);
	}
	return 0;

}

void Debug_MessageBox_Info(LPCSTR info)
{
	::MessageBox(NULL, info, "", 0);
}

BOOL CExtractImageApp::CreateThumbnail(HBITMAP* phBmpThumbnail, LPCSTR lpFileName)
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!g_bInitGloable)
	{
		InitGlobal();
	}
	
	g_hwnd_Engine = GetEngineWnd();

	if (!g_bInitEngineAppPath)
	{
		g_bInitEngineAppPath = GetWndModuleFileName(g_hwnd_Engine, g_szEngineAppName);
	}

	if (g_hMapFile == NULL)
	{
		InitShareMemory();
	}


	if (g_hwnd_Engine == NULL)
	{	
		KGLogPrintf(KGLOG_INFO, "begin start 3dengine");
		
		if (StartEngineProcess(g_szEngineAppName))
		{	
			//等待引擎初始化完毕(g_hwnd_Engine != NULL)
			while (IsEngineProcessExist(g_Engine_Process_Information.hProcess) && g_hwnd_Engine == NULL)
			{
				g_hwnd_Engine = GetEngineWnd();

				::SwitchToThread();
			}
			::CloseHandle(g_Engine_Process_Information.hProcess);
			::CloseHandle(g_Engine_Process_Information.hThread);
		}
	}
	

	LPTSTR lpExtName = 	::PathFindExtension(lpFileName);
	if (lpExtName && 
		(StrCmpI(lpExtName, ".ani") == 0 || StrCmpI(lpExtName, ".tani") == 0)
		)
	{
		return E_UNEXPECTED;
	}

	
	HBITMAP hbmp = NULL; 
	if (g_hwnd_Engine)
	{
		SendFileName(lpFileName);
		LRESULT lresult = ::SendMessage(g_hwnd_Engine, WM_SEND_MESH_FILE_NAME, 0, NULL);
		if (lresult == S_OK)
		{
			hbmp = (HBITMAP)::LoadImage(NULL, MESH_MDL_BMP_FILE_NAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}

		CheckEngineProcessMemory();
	}
	*phBmpThumbnail = hbmp;
	
	
	return NOERROR; 
}

void CheckEngineProcessMemory()
{
	if (g_hwnd_Engine)
	{
		//检查引擎进程是否占用了太多的内存
		DWORD dwThreadId, dwProcessId;
		dwThreadId = ::GetWindowThreadProcessId(g_hwnd_Engine, &dwProcessId);

		HANDLE hProcess;
		SIZE_T uEngineProcessWorkSetSize = 0;
		SIZE_T uEngineProcessPageSize = 0;
		hProcess = ::OpenProcess(PROCESS_ALL_ACCESS , FALSE, dwProcessId);

		if (hProcess)
		{
			GetWorkingSetSizeAddPageFileSize(hProcess, &uEngineProcessWorkSetSize, &uEngineProcessPageSize);
			if (uEngineProcessPageSize > gs_MaxPageSize || uEngineProcessWorkSetSize > gs_MaxWorkSetSize)
			{
				KGLogPrintf(KGLOG_INFO,"3dengine use too much memory:workset:%lu,pagesize:%lu", uEngineProcessWorkSetSize, uEngineProcessPageSize);	
				::SendMessage(g_hwnd_Engine, WM_DESTROY, 0, NULL);
			}
			::CloseHandle(hProcess);
		}
	}

}



BOOL SendFileName(LPCSTR szFileName)
{
	assert(szFileName);
	BOOL bResult = TRUE;

	size_t uLen = strlen(szFileName);
	memset(g_pShareBuffer, 0, MAX_SHARE_SIZE);
	memcpy(g_pShareBuffer, szFileName, uLen);


	return bResult;
}

BOOL StartEngineProcess(LPCSTR lpEngineAppName)
{
	BOOL bResult  = FALSE;
	BOOL bRetCode = FALSE;

	STARTUPINFO si = {sizeof(si)};

	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;


	ZeroMemory( &g_Engine_Process_Information, sizeof(g_Engine_Process_Information) );



	// Start the child process. 
	bRetCode = CreateProcess(NULL,   // No module name (use command line)
		g_szEngineAppName,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&g_Engine_Process_Information );           // Pointer to PROCESS_INFORMATION structure
		
	
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL InitShareMemory()
{
	BOOL bResult = TRUE;

	g_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T(IDTIFY_SHARE_FILE_MAPPING_NAME));


	if (g_hMapFile == NULL)
	{
		g_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SHARE_SIZE, _T(IDTIFY_SHARE_FILE_MAPPING_NAME));
		if (g_pShareBuffer)
		{
			::UnmapViewOfFile(g_pShareBuffer);
		}
	}

	if (g_hMapFile == NULL)
	{
		bResult = FALSE;
	}
	else
	{
		LPVOID lpMapAddress = MapViewOfFile(g_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		g_pShareBuffer = lpMapAddress;
	}

	return bResult;
}


BOOL IsEngineProcessExist(HANDLE hProcess)
{
	DWORD dwExitCode = 0;

	::GetExitCodeProcess(hProcess, &dwExitCode);

	return dwExitCode == STILL_ACTIVE;


}
BOOL CALLBACK EnumWindowsProc(HWND hwnd,
							  LPARAM lParam
							  )
{
	BOOL bRet = TRUE;
	HWND* hWndOut = (HWND*)lParam;


	char szTempWndText[128];

	::GetWindowText(hwnd, szTempWndText, 128);

	if (StrCmp(szTempWndText, EngineWindowName) == 0)
	{
		*hWndOut = hwnd;
		bRet = FALSE;

	}

	return bRet;
}
HWND GetEngineWnd()
{
	HWND hWnd_Ret = NULL;

	::EnumWindows((WNDENUMPROC)(EnumWindowsProc),(LPARAM) &hWnd_Ret);

	return hWnd_Ret;
}

BOOL InitGlobal()
{
	g_pShareBuffer = NULL;
	g_hMapFile = NULL;
	g_hwnd_Engine = NULL;
	g_Engine_Process_Information.hProcess = NULL;
	g_Engine_Process_Information.hThread = NULL;

#ifdef Debug
	InitLog();
#endif
	

	SetMaxMemory();

	g_bInitGloable = TRUE;

	return TRUE;

}

BOOL InitLog()
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

	strcpy(LogParam.szIdent, "MeshMdlShlExt");
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

BOOL GetWorkingSetSizeAddPageFileSize(HANDLE hProcess, PSIZE_T lpWorkSetSize, PSIZE_T lpPageFileSize)
{
	BOOL bResult = FALSE;
	BOOL bRetcode = FALSE;


	PROCESS_MEMORY_COUNTERS pmc = {0};

	KGLOG_PROCESS_ERROR(hProcess);

	bRetcode = ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
	KGLOG_PROCESS_ERROR(bRetcode);

	*lpPageFileSize = pmc.PagefileUsage / 1024;
	*lpWorkSetSize = pmc.WorkingSetSize / 1024;

	bResult = TRUE;
Exit0:
	return bResult;
}



STDAPI SetMaxMemory()
{
	TCHAR szKeyName[] = ".mesh";
	TCHAR szWorkSetSizeValueName[] = "MaxWorkSetSize";
	TCHAR szPageSizeValueName[] = "MaxPageSize";
	BYTE  byData[256];
	DWORD dwDataLen = 256;

	HKEY hKeyReturn;
	DWORD dwTypeReturn;

	BOOL bOpenKeySucess = true;

	LRESULT lresult = RegOpenKeyEx(HKEY_CLASSES_ROOT,LPCSTR(szKeyName), 0, KEY_READ,  &hKeyReturn);

	if (lresult != ERROR_SUCCESS)
	{		
		KGLogPrintf(KGLOG_ERR, "open registry key %s failed!", szKeyName);
		bOpenKeySucess = false;
		goto Exit0;
	}
	lresult = RegQueryValueEx(hKeyReturn, LPCTSTR(szWorkSetSizeValueName), NULL, &dwTypeReturn , byData, &dwDataLen);
	if (lresult != ERROR_SUCCESS)
	{		
		KGLogPrintf(KGLOG_ERR, "query registry key:%s value %s failed! errorcode = %d", szKeyName, szWorkSetSizeValueName, lresult);
		goto Exit0;
	}

	gs_MaxWorkSetSize = atol((char*)byData) * 1024;
	lresult = RegQueryValueEx(hKeyReturn, LPCTSTR(szPageSizeValueName), NULL, &dwTypeReturn , byData, &dwDataLen);
	if (lresult != ERROR_SUCCESS)
	{		
		KGLogPrintf(KGLOG_ERR, "query registry key:%s value %s failed! errorcode = %d", szKeyName, szPageSizeValueName, lresult);
		goto Exit0;
	}
	
	gs_MaxPageSize = atol((char*)byData) * 1024;

Exit0:
	return S_OK;
}

BOOL QueryEngineAppPath(char szEngineAppPath[])
{
	BOOL bRetCode = false;
	BOOL bResult = false;

	TCHAR szEngineRootPathKeyName[] = ".mesh";
	TCHAR szValueName[] = "EngineAppName";
	BYTE  byData[256];
	DWORD dwDataLen = 256;

	HKEY hKeyReturn;
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
	sprintf(szEngineAppPath, "%s", byData);
	

	bResult = TRUE;
Exit0:
	if (bOpenKeySucess)
	{
		RegCloseKey(hKeyReturn);
	}
	return bResult;

}

BOOL QueryRegKeyValue(HKEY hkeyMain, LPCSTR lpSubKeyName, LPCSTR lpValueName, LPSTR lpReturn, LPDWORD lpcbData)
{
	BOOL bRetCode = false;
	BOOL bResult = false;

	HKEY hKeyReturn;
	DWORD dwTypeReturn;

	BYTE  byData[256];
	DWORD dwDataLen = 256;

	BOOL bOpenKeySucess = true;

	LRESULT lresult = RegOpenKeyEx(hkeyMain,lpSubKeyName,  0, KEY_READ,  &hKeyReturn);
	if (lresult == ERROR_SUCCESS)
	{
		lresult = RegQueryValueEx(hKeyReturn, lpValueName, NULL, &dwTypeReturn , byData, &dwDataLen);
		if (lresult == ERROR_SUCCESS)
		{
			sprintf(lpReturn, "%s", byData);
			bResult = TRUE;
		}
		RegCloseKey(hKeyReturn);
	}

	return bResult;
}

void CALLBACK RecalcText(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	HWND		hwndFind	= GetForegroundWindow();
	HRESULT		hRet		= E_FAIL;

	TCHAR szPath[MAX_PATH];
	TCHAR szItem[MAX_PATH];

	strcpy(szPath, "\0");
	strcpy(szItem, "\0");

	IShellWindows*		psw			= NULL;
	IDispatch*			pdisp		= NULL;

	BOOL				fFound		= FALSE;

	IWebBrowserApp*		pwba		= NULL;
	HWND				hwndWBA		= NULL;
	IServiceProvider*	psp			= NULL;
	IShellBrowser*		psb			= NULL;
	IShellView*			psv			= NULL;
	IFolderView*		pfv			= NULL;
	IPersistFolder2*	ppf2		= NULL;
	LPITEMIDLIST		pidlFolder  = NULL;
	LPITEMIDLIST		pidlItem	= NULL;
	IShellFolder*		psf			= NULL;
	VARIANT				v;
	LRESULT				lresult		= NULL;
	char*				pchPos		= NULL;
	BOOL				bAniFile	= FALSE;

	if (!g_bInitGloable || !g_hwnd_Engine || !g_hMapFile)
		return;

	CoInitialize(NULL);
	COM_CALL(hRet = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&psw));
	
	V_VT(&v) = VT_I4;

	for (V_I4(&v) = 0; !fFound && psw->Item(v, &pdisp) == S_OK; V_I4(&v)++) 
	{
		COM_CALL(pdisp->QueryInterface(IID_IWebBrowserApp, (void**)&pwba));
		COM_CALL(hRet = pwba->get_HWND((LONG_PTR*)&hwndWBA));
		if (hwndWBA != hwndFind)
			goto FAIL0; 
		fFound = TRUE;	
		COM_CALL(pwba->QueryInterface(IID_IServiceProvider, (void**)&psp)); 		
		COM_CALL((psp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&psb)));
		COM_CALL((psb->QueryActiveShellView(&psv))) 
		COM_CALL(hRet = (psv->QueryInterface(IID_IFolderView, (void**)&pfv))) 
		COM_CALL((pfv->GetFolder(IID_IPersistFolder2,(void**)&ppf2))) 

		COM_CALL((ppf2->GetCurFolder(&pidlFolder))) 
		if (!SHGetPathFromIDList(pidlFolder, szPath)) 
		{
			lstrcpyn(szPath, TEXT("<not a directory>"), MAX_PATH);
		}  
		int iFocus;
		COM_CALL((pfv->GetFocusedItem(&iFocus))) 
		COM_CALL(pfv->Item(iFocus, &pidlItem)); 
		COM_CALL((ppf2->QueryInterface(IID_IShellFolder, (void**)&psf))) 
										
		STRRET str;
		COM_CALL((psf->GetDisplayNameOf(pidlItem, SHGDN_INFOLDER, &str))) 
		
		StrRetToBuf(&str, pidlItem, szItem, MAX_PATH);

		strcat_s(szPath, MAX_PATH, "\\");
		strcat_s(szPath, MAX_PATH, szItem);
		pchPos = strrchr(szPath, '.');

		bAniFile = (pchPos != NULL) && 
			(strcmp(pchPos, ".ani") == 0) || (strcmp(pchPos, ".ANI") == 0) || (strcmp(pchPos, ".Ani") == 0);
			//(strcmp(pchPos, ".tani") == 0) || (strcmp(pchPos, ".TANI") == 0) || (strcmp(pchPos, ".Tani") == 0);

		if (bAniFile)
		{
			SendFileName(szPath);

			lresult = ::SendMessage(g_hwnd_Engine, WM_SEND_ANI_FILE_NAME, 0, NULL);
			if (lresult == S_OK)
			{
				pchPos = strrchr(szPath, '\\');
				*pchPos = '\0';
				pchPos = strrchr(szPath, '\\');
				strcpy(pchPos, "\\模型\\");

				int nRet = GetMDLFile(szPath);
				if (nRet)
				{
					SendFileName(szPath);
					lresult = ::SendMessage(g_hwnd_Engine, WM_ANIMATION_FILE, 0, NULL);
					if (lresult == S_OK)
					{
						CallRander();
					}
				}
			}
		}

FAIL0:
		COM_RELEASE(psf);
		if (pidlItem)
		{
			CoTaskMemFree(pidlItem);
			pidlItem = NULL;
		}
		if (pidlFolder)
		{
			CoTaskMemFree(pidlFolder);
			pidlFolder = NULL;
		}
		COM_RELEASE(ppf2);
		COM_RELEASE(pfv);
		COM_RELEASE(psv);
		COM_RELEASE(psb);
		COM_RELEASE(psp);
		COM_RELEASE(pwba);
		COM_RELEASE(pdisp);
	}
Exit0:	
	COM_RELEASE(psf);
	if (pidlItem)
	{
		CoTaskMemFree(pidlItem);
		pidlItem = NULL;
	}
	if (pidlFolder)
	{
		CoTaskMemFree(pidlFolder);
		pidlFolder = NULL;
	}
	COM_RELEASE(ppf2);
	COM_RELEASE(pfv);
	COM_RELEASE(psv);
	COM_RELEASE(psb);
	COM_RELEASE(psp);
	COM_RELEASE(pwba);
	COM_RELEASE(pdisp);
	COM_RELEASE(psw);
	
	CoUninitialize();
}

int GetMDLFile(char* pchPath)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	strcpy(szFind, pchPath);
	strcat(szFind, "*.mdl");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)    
		return 0;

	char* pchPos = strrchr(pchPath, '\\');
	strcpy(pchPos + 1, FindFileData.cFileName);
	
	FindClose(hFind);
	return 1;
}

BOOL CALLBACK EnumWindowsProcRenderBmp(HWND hwnd, LPARAM lParam)
{
	BOOL bRet = TRUE;
	HWND* hWndOut = (HWND*)lParam;


	char szTempWndText[128];

	::GetWindowText(hwnd, szTempWndText, 128);

	if (StrCmp(szTempWndText, "__RENDER_BMP__") == 0)
	{
		*hWndOut = hwnd;
		bRet = FALSE;
	}

	return bRet;
}

void CallRander()
{
	HWND hWnd_Ret = NULL;
	::EnumWindows((WNDENUMPROC)(EnumWindowsProcRenderBmp),(LPARAM) &hWnd_Ret);
	if (hWnd_Ret != NULL)
	{
		SendMessage(hWnd_Ret, WM_USER_RERENDERBMP, 0, 0);
	}
	else
		_spawnl(P_NOWAIT, "c:\\RenderBMP.exe", "c:\\RenderBMP.exe", NULL);
}
BOOL GetWndModuleFileName(HWND hwnd, LPSTR lpFileName)
{
	BOOL bResult = FALSE;

	DWORD dwProId = 0;
	DWORD dwRetCode = 0;

	HANDLE hProcess_3dengine = NULL;

	KGLOG_PROCESS_ERROR(hwnd);
	

	::GetWindowThreadProcessId(hwnd, &dwProId);
	KGLOG_PROCESS_ERROR(dwProId);

	hProcess_3dengine = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProId);
	KGLOG_PROCESS_ERROR(hProcess_3dengine);
	
	dwRetCode = ::GetModuleFileNameEx(hProcess_3dengine, NULL, lpFileName, MAX_PATH - 1);
	KGLOG_PROCESS_ERROR(dwRetCode > 0);

	bResult = TRUE;
Exit0:

	return bResult;

}