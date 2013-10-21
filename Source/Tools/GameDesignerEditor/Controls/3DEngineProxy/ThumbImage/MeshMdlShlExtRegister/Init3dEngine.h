//引擎的窗口句柄
HWND g_hwnd_Engine = NULL;

//引擎进程信息
PROCESS_INFORMATION g_Engine_Process_Information = {0};

//初始化3dengine
void Init3Dengine(LPSTR lpEngineAppName);

//启动引擎程序
BOOL StartEngineProcess(LPSTR lpEngineAppName);

//停止引擎程序
BOOL StopEngineProcess();

//判断引擎程序是否存在
BOOL IsEngineProcessExist(HANDLE);

//获取引擎程序的窗口句柄
HWND GetEngineWnd();

//枚举窗口的回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

//引擎窗口名称
#define EngineWindowName "_EngineWindowName_For_Mesh_"

#define WM_REINIT_ENGINE		WM_USER + 104

void Init3Dengine(LPSTR lpEngineAppName)
{
	g_hwnd_Engine = GetEngineWnd();
	
	if (g_hwnd_Engine == NULL)
	{	
		if (StartEngineProcess(lpEngineAppName))
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
}


BOOL StartEngineProcess(LPSTR lpEngineAppName)
{
	BOOL bResult = TRUE;

	STARTUPINFO si = {sizeof(si)};

	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;

	ZeroMemory( &g_Engine_Process_Information, sizeof(g_Engine_Process_Information) );



	// Start the child process. 
	if( !CreateProcess(NULL,   // No module name (use command line)
		lpEngineAppName,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&g_Engine_Process_Information )           // Pointer to PROCESS_INFORMATION structure
		) 
	{	
		bResult = FALSE;
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

	if (strcmp(szTempWndText, EngineWindowName) == 0)
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
BOOL StopEngineProcess()
{
	g_hwnd_Engine = GetEngineWnd();

	if (g_hwnd_Engine)
	{
		::SendMessage(g_hwnd_Engine, WM_DESTROY, 0, NULL);
	}

	return TRUE;
}