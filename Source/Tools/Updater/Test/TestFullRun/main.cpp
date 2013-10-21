
//#include "stdafx.h"
#include <iostream>
#include <vector>
using namespace std;
#include "KTestCommon.h"
#include "KRegInfo.h"
#include "shlwapi.h"

//////////////////////////////////////////////////////////////////////////
const TCHAR TESTING_NAME[]     = _T("UpdaterAutoTesting");
const TCHAR TESTING_LOG_DIR[]  = _T("TestingLogs");
const TCHAR TESTING_LOG_NAME[] = _T("TestFullRun");

const TCHAR LATEST_VERSION_2[]		= _T("1-0-8-1505");							//更新列表中最新版本号
const TCHAR UPDATE_ONE_VERSION[]	= _T("1.0.8.1502");							//更新一个更新包的版本
const TCHAR NEWPAK_ONE_VERSION[]	= _T("0ff7e25f87a94aec08eb0ec70bc02b05");	//更新一个版本，分包后新包的MD5
const TCHAR TEST_FILE[]				= _T("Test");								//用于生成与更新后的文件名匹配的空文件，验证覆盖更新新
const TCHAR VERSION_FILE_MD5[]		= _T("b3bbd5a5080720fa13d8ebd4060960b3");	//更新后版本文件(version.cfg)的MD5值
const TCHAR CONFIG_INI_MD5[]		= _T("a6d6808c6295be7d6bab8c0cb6dd3ae5");	//更新后的config.ini值

STARTUPINFO			g_ServerStartupinfo;				
STARTUPINFO			g_GameUpdaterStartupinfo;			
PROCESS_INFORMATION g_ServerProcessInfo;				
PROCESS_INFORMATION g_GameUpdaterProcessInfo;			
BOOL				g_bRegSaved;						

//////////////////////////////////////////////////////////////////////////
int Init();
int Run();
int UnInit();

int _PrepareExistFiles();
int _ValidateFileData();
int _PrepareTestSpace(const TCHAR cszTestVersion[]);
int _CreatePakForNewPack();
int _ValidateRegData(const TCHAR cszVersion[]);
int _StartProc();
int _WaitAndCloseProc(const TCHAR szLatestVersion[], UINT uMaxTimeOut);
int _SetUp();
int _TearDown();
int _CheckUpdatePath();

//////////////////////////////////////////////////////////////////////////
int main(int nArgc, char *pszArgv[])
{
	int nRetCode = false;
	int nResult  = false;
	BOOL bLogInited = false;

	KGLOG_PARAM LogParam  = {_T(""), _T(""), KGLOG_OPTION_FILE, 65535 * 2};
	nRetCode = _tcscpy_s(LogParam.szPath, PATH_MAX, TESTING_LOG_DIR);
	KG_PROCESS_ERROR(nRetCode == 0);
	LogParam.szPath[PATH_MAX - 1] = _T('\0');
	nRetCode = _tcscpy_s(LogParam.szIdent, PATH_MAX, TESTING_LOG_NAME);
	KG_PROCESS_ERROR(nRetCode == 0);
	LogParam.szIdent[PATH_MAX - 1] = _T('\0');

	nRetCode = KGLogInit(LogParam, (PVOID)TESTING_LOG_NAME);
	KG_PROCESS_ERROR(nRetCode);
	bLogInited = true;

	nRetCode = Init();
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = Run();
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = UnInit();
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (bLogInited)
	{
		KGLogUnInit((PVOID)TESTING_LOG_NAME);
	}
	system("Pause");
	return nResult ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////
int Init()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR *pSlash = NULL;

	TCHAR szUpdateDir[MAX_PATH]	 = _T("");
	TCHAR szPakDir[MAX_PATH]	 = _T("");

	//获取应用程序路径，如：D:\UpdaterAutoTesting
	nRetCode = ::GetModuleFileName(NULL, g_szAppPath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	pSlash = _tcsrchr(g_szAppPath, _T('\\'));
	KGLOG_PROCESS_ERROR(pSlash);
	*pSlash = _T('\0');
	pSlash = NULL;
	//设置当前工作目录
	nRetCode = ::SetCurrentDirectory(g_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode);
	//构造测试资源路径
	nRetCode = _sntprintf(g_szTestingResDir, MAX_PATH, _T("%s\\%s"), g_szAppPath, TEST_RESOURCE_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	g_szTestingResDir[MAX_PATH - 1] = '\0';
	//构造测试空间路径
	nRetCode = _sntprintf(g_szTestingSpaceDir, MAX_PATH, _T("%s\\%s"), g_szAppPath, TEST_SPACE_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	g_szTestingSpaceDir[MAX_PATH - 1] = '\0';

	// :::::::::::::::::::初始化运行环境::::::::::::::::::::::::::::
	g_bRegSaved = false;

	//清理测试空间
	nRetCode = KTestCommon::CleanPathExceptLogAndPak(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	//创建pak目录
	nRetCode = _sntprintf(szPakDir, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PAK_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPakDir[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szPakDir, NULL); 
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_ALREADY_EXISTS);

	//初始化进程信息
	ZeroMemory(&g_ServerStartupinfo, sizeof(g_ServerStartupinfo));
	g_ServerStartupinfo.cb = sizeof(g_ServerStartupinfo);
	ZeroMemory(&g_ServerProcessInfo, sizeof(g_ServerProcessInfo));

	ZeroMemory(&g_GameUpdaterStartupinfo, sizeof(g_GameUpdaterStartupinfo));
	g_GameUpdaterStartupinfo.cb = sizeof(g_GameUpdaterStartupinfo);
	ZeroMemory(&g_GameUpdaterProcessInfo, sizeof(g_GameUpdaterProcessInfo));

	//记录测试前的注册表数据，以便还原
	nRetCode = KTestCommon::SaveRegData();
	//KGLOG_PROCESS_ERROR(nRetCode);
	g_bRegSaved = true;

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int UnInit()
{
	int nResult  = false;
	int nRetCode = false;

	//还原注册表数据
	if (g_bRegSaved)
	{
		nRetCode = KTestCommon::RestoreRegData();
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "RestoreRegData Failed");
		}
	}
	//清理测试空间
	nRetCode = KTestCommon::CleanPathExceptLogAndPak(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int Run()
{
	int nResult  = false;
	int nRetCode = false;
	int nErrCount = 0;
	int nSucCount = 0;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szServerName[MAX_PATH] = {0};

	HKEY hKey = NULL;
	TCHAR szConfigKey[MAX_PATH] = {0};
	DWORD dwPathSize = (DWORD)(MAX_PATH * sizeof(TCHAR));

	//const TCHAR NEW_GAMEUPDATER_MD5[]		 = _T("52ee7b68ed67f895aedea73ddb6e90fe");	//自更新后新的GameUpdater.exe的MD5值
	const TCHAR NEW_GAMEUPDATER_INI_MD5[]	 = _T("52a14ce2753398635cb92040d832e91a");	//自更新后新的GameUpdater.ini的MD5值
	//const TCHAR NEW_UPDATE_DLL_MD5[]		 = _T("9aa277c917e0bb13a8fcb9f4ab53b5ad");	//自更新后新的update.dll的MD5值

	int nIndex = 1;
	vector<int> arrErrIndeice;

	//创建满足分包条件的合并目标包
	nRetCode = _CreatePakForNewPack();
	KGLOG_PROCESS_ERROR(nRetCode);

	//启动http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	szServerName[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(NULL, szServerName, NULL, NULL, FALSE, 0, NULL, NULL, &g_ServerStartupinfo, &g_ServerProcessInfo); 
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = KTestCommon::WaitForProcessStarted(g_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);
	Sleep(3000);

	while (true)
	{
		nResult = false;
		nRetCode = _TearDown();
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = _SetUp();
		KGLOG_PROCESS_ERROR(nRetCode);

		//设置恢复Congfig注册表信息
		nRetCode = ::SHDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Kingsoft\\jx3\\");
		KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS || nRetCode == ERROR_FILE_NOT_FOUND);

		//准备GameUpdater运行的测试资源
		nRetCode = _PrepareTestSpace(UPDATE_ONE_VERSION);
		KGLOG_PROCESS_ERROR(nRetCode);

		//准备与更新后的文件名匹配的空文件，用于验证覆盖更新
		nRetCode = _PrepareExistFiles();
		KGLOG_PROCESS_ERROR(nRetCode);

		//运行更新
		nRetCode = _StartProc();
		KGLOG_PROCESS_ERROR(nRetCode);

		//关闭更新相关进程
		nRetCode = _WaitAndCloseProc(LATEST_VERSION_2, 2 * 60 * 1000);//2分钟超时
		KGLOG_PROCESS_ERROR(nRetCode);

		//验证数据
		nRetCode = _ValidateFileData();
		KGLOG_PROCESS_ERROR(nRetCode);

		//验证package.ini
		nRetCode = KTestCommon::ValidatePackageFile();
		KGLOG_PROCESS_ERROR(nRetCode);

		//验证update_2.pak
		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_2);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		szTempFile[MAX_PATH - 1] = _T('\0');
		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEWPAK_ONE_VERSION);
		KGLOG_PROCESS_ERROR(nRetCode);

		//验证gamedater.exe, gameupdater.ini, update.dll自更新成功
		//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
		//KGLOG_PROCESS_ERROR(nRetCode != -1);
		//szTempFile[MAX_PATH - 1] = _T('\0');
		//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_GAMEUPDATER_MD5);
		//KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		szTempFile[MAX_PATH - 1] = _T('\0');
		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_GAMEUPDATER_INI_MD5);
		KGLOG_PROCESS_ERROR(nRetCode);

		//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\update.dll"), g_szTestingSpaceDir);
		//KGLOG_PROCESS_ERROR(nRetCode != -1);
		//szTempFile[MAX_PATH - 1] = _T('\0');
		//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_UPDATE_DLL_MD5);
		//KGLOG_PROCESS_ERROR(nRetCode);

		//验证Config注册表、
		nRetCode = ::RegOpenKeyEx(REG_ROOT_KEY, "SOFTWARE\\Kingsoft\\jx3\\zhcn", 0, KEY_READ, &hKey);
		KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
		nRetCode = ::RegQueryValueEx(hKey, "CONFIGGAME", NULL, NULL, (LPBYTE)szConfigKey, &dwPathSize);
		KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
		nRetCode = _tcscmp(szConfigKey, "2");
		KG_PROCESS_ERROR(!nRetCode);

		//验证config.ini的MD5值
		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\config.ini"), g_szTestingSpaceDir);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		szTempFile[MAX_PATH - 1] = _T('\0');
		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, CONFIG_INI_MD5);
		KGLOG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		if (nResult)
		{
			++nSucCount; 
		}
		else
		{
			arrErrIndeice.push_back(nIndex);
			++nErrCount;
			MessageBox(NULL, "error!!", "Error", MB_OK);
		}
		cout<<endl<<endl
			<<"****************************************"<<endl
			<<"失败："<<nErrCount<<endl
			<<"成功："<<nSucCount<<endl;
		cout<<"失败ID:";
		for (UINT i = 0; i < arrErrIndeice.size(); ++i)
		{
			cout<<arrErrIndeice[i]<<"\t";
		}
		cout<<endl
			<<"****************************************"<<endl<<endl;

		//if (nErrCount >= 1000 || nSucCount >= 1000)
		//	break;

		++nIndex;
	}

	//关闭http服务器进程
	if (g_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(g_ServerProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		g_ServerProcessInfo.hProcess = NULL;
	}

	cout<<"==========================================================";
	cout<<endl<<"成功："<<nSucCount<<'\t'<<"失败："<<nErrCount<<endl;
	cout<<"=========================================================="<<endl;

	return true;
}

//////////////////////////////////////////////////////////////////////////
int _PrepareExistFiles()
{
	int nResult  = false;
	int nRetCode = false;

	for (int nIndex = 0; nIndex < UPDATE_FILE_COUNT; nIndex++)
	{
		nRetCode = KTestCommon::PrepareTestResoure(TEST_FILE, UPDATE_FILE_NAME[nIndex]);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _ValidateFileData()
{
	int nResult  = false;
	int nRetCode = false;

	//TCHAR szTempDir[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];
	TCHAR* pszFileName = NULL;
	HANDLE hFindFile  = INVALID_HANDLE_VALUE;
	//WIN32_FIND_DATA FindFileData;
	int nIsEmpty = true;

	//验证update文件夹清空
	nRetCode = _CheckUpdatePath();
	KGLOG_PROCESS_ERROR(nRetCode && "Update path isn't empty!");

	//验证更新的文件
	for (int nIndex = 0; nIndex < UPDATE_FILE_COUNT; nIndex++)
	{
		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, UPDATE_FILE_NAME[nIndex]);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		szTempFile[MAX_PATH - 1] = _T('\0');
		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, UPDATE_FILE_MD5[nIndex]);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	//验证version.cfg
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, VERSION_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, VERSION_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _PrepareTestSpace(const TCHAR cszTestVersion[])
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVersionFile[MAX_PATH];

	ASSERT(cszTestVersion);

	//准备version.cfg，并修改为测试的版本号
	nRetCode = KTestCommon::PrepareTestResoure(VERSION_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode);
	//nRetCode = _sntprintf(szVersionFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, VERSION_FILE_NAME);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szVersionFile[MAX_PATH - 1] = _T('\0');
// 	nRetCode = KTestCommon::ModifyVersionFile(szVersionFile, cszTestVersion);
// 	KGLOG_PROCESS_ERROR(nRetCode);
	//准备gameupdater.ini
	nRetCode = KTestCommon::PrepareTestResoure(CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode);
	//准备package.ini
	nRetCode = KTestCommon::PrepareTestResoure(PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode);
	//准备gameupdater.exe
	nRetCode = KTestCommon::PrepareTestResoure(GAMEUPDATER_EXE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode);
	//准备update.dll
	nRetCode = KTestCommon::PrepareTestResoure(UPDATE_DLL);
	KGLOG_PROCESS_ERROR(nRetCode);
	////准备update.swf
	//nRetCode = KTestCommon::PrepareTestResoure(UPDATER_SWF);
	//KGLOG_PROCESS_ERROR(nRetCode);
	//准备config.ini
	nRetCode = KTestCommon::PrepareTestResoure(CONFIG_INI);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _CreatePakForNewPack()
{
	int nResult  = false;
	int nRetCode = false;
	int nDataCreated = false;
	//FILE* pFile  = NULL; 
	//BYTE* pbData = NULL;
	TCHAR szTempFile[MAX_PATH] = {0};
	ULARGE_INTEGER nFreeBytesAvailable     = {0};
	ULARGE_INTEGER nTotalNumberOfBytes     = {0};
	ULARGE_INTEGER nTotalNumberOfFreeBytes = {0};

	//判断磁盘空间是否足够
	nRetCode = ::GetDiskFreeSpaceEx(
		NULL,
		&nFreeBytesAvailable,
		&nTotalNumberOfBytes,
		&nTotalNumberOfFreeBytes
		);
	KGLOG_PROCESS_ERROR(nRetCode);
	//判断可用空间是否大于2G
	KGLOG_PROCESS_ERROR(nFreeBytesAvailable.QuadPart > (2 * 1024 * 1024));

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_3, szTempFile);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _ValidateRegData(const TCHAR cszVersion[])
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szCurrentVersion[MAX_VERSION_LENGTH];
	TCHAR szInstallPath[MAX_PATH];

	ASSERT(cszVersion);

	//验证注册表
	nRetCode = KRegInfo::ReadInstallPath(VERSION_LINE_NAME, szInstallPath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _tcsncmp(szInstallPath, g_szAppPath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = KRegInfo::ReadCurrentVersion(VERSION_LINE_NAME, szCurrentVersion, MAX_VERSION_LENGTH);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _tcsncmp(szCurrentVersion, cszVersion, MAX_VERSION_LENGTH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _StartProc()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szServerName[MAX_PATH] = {0};
	TCHAR szGameUpdaterPath[MAX_PATH] = {0};
	static int snStartCnt = 0;

	//启动GameUpdater进程
	nRetCode = _sntprintf(szGameUpdaterPath, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szGameUpdaterPath[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(
		NULL,
		szGameUpdaterPath,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&g_GameUpdaterStartupinfo,
		&g_GameUpdaterProcessInfo
		);

	if (!nRetCode)
	{
		KGLOG_PROCESS_ERROR(nRetCode != -1 && "Create GameUpdater Process Fail!");
		_WaitAndCloseProc(LATEST_VERSION_2, 0);
		
		KGLOG_PROCESS_ERROR(snStartCnt <= 5 && "Create max times out!");
		
		snStartCnt++;
		Sleep(2000);
		_StartProc();
	}

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _WaitAndCloseProc(const TCHAR szLatestVersion[], UINT uMaxTimeOut)
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVersionFile[MAX_PATH] = {0};
	TCHAR szNewVersion[32] = {0};; //版本号长度不超过32个字符"1-0-8-1505"

	//等待并关闭GameUpdater进程
	//nRetCode = _sntprintf(szVersionFile, MAX_PATH, _T("%s\\version.cfg"), g_szTestingSpaceDir);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//int nSleepTimes = uMaxTimeOut / 10000;
	//for (int i = 0; i < nSleepTimes; i++)
	//{
	//	nRetCode = KTestCommon::GetCurVersion(szVersionFile, szNewVersion);
	//	KGLOG_PROCESS_ERROR(nRetCode);
	//	//KG_PROCESS_SUCCESS(_tcscmp(szLatestVersion, szNewVersion) == 0);
	//	if (_tcscmp(szLatestVersion, szNewVersion) == 0)
	//		break;
	//	::Sleep(10000); 
	//}
	
	if (uMaxTimeOut != 0)
	{
		Sleep(120000);//2分钟超时

		//nRetCode = _CheckUpdatePath();
		//if (!nRetCode)
		//{
		//	Sleep(60000);
		//}
	}


	if (g_GameUpdaterProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(g_GameUpdaterProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait GameUpdater Process Failed");
		}
		g_GameUpdaterProcessInfo.hProcess = NULL;
	}

	KTestCommon::CloseProcess(GAMEUPDATER_PROC_NAME);

	////关闭http服务器进程
	//if (g_ServerProcessInfo.hProcess != NULL)
	//{
	//	nRetCode = KTestCommon::CloseAndWaitForProcess(g_ServerProcessInfo.hProcess);
	//	if (!nRetCode)
	//	{
	//		KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
	//	}
	//	g_ServerProcessInfo.hProcess = NULL;
	//}

	nResult = true;
//Exit0:
	Sleep(3000);//临时解决
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int  _SetUp()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bEventCreated = false;
	TCHAR szUpdateDir[MAX_PATH]	 = _T("");
	TCHAR szPakDir[MAX_PATH]	 = _T("");
	g_bRegSaved = false;

	//初始化进程信息
	ZeroMemory(&g_ServerStartupinfo, sizeof(g_ServerStartupinfo));
	g_ServerStartupinfo.cb = sizeof(g_ServerStartupinfo);
	ZeroMemory(&g_ServerProcessInfo, sizeof(g_ServerProcessInfo));

	ZeroMemory(&g_GameUpdaterStartupinfo, sizeof(g_GameUpdaterStartupinfo));
	g_GameUpdaterStartupinfo.cb = sizeof(g_GameUpdaterStartupinfo);
	ZeroMemory(&g_GameUpdaterProcessInfo, sizeof(g_GameUpdaterProcessInfo));

	//记录测试前的注册表数据，以便还原
	nRetCode = KTestCommon::SaveRegData();
	//KGLOG_PROCESS_ERROR(nRetCode);
	g_bRegSaved = true;

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int  _TearDown()
{
	int nResult  = false;
	int nRetCode = false;

	//还原注册表数据
	if (g_bRegSaved)
	{
		nRetCode = KTestCommon::RestoreRegData();
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "RestoreRegData Failed");
		}
	}
	//清理测试空间
	nRetCode = KTestCommon::CleanPathExceptLogAndPak(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
int _CheckUpdatePath()
{
	int nResult  = false;
	int nRetCode = false;

	TCHAR szTempDir[MAX_PATH] = {0};
	//TCHAR szTempFile[MAX_PATH];
	TCHAR* pszFileName = NULL;
	HANDLE hFindFile  = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;
	int nIsEmpty = true;

	//验证update文件夹清空
	nRetCode = _sntprintf(szTempDir, MAX_PATH, _T("%s\\%s\\*.*"), g_szTestingSpaceDir, TEMP_UPDATE_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempDir[MAX_PATH - 1] = _T('\0');
	hFindFile = ::FindFirstFile(szTempDir, &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		while (true)
		{
			pszFileName = FindFileData.cFileName;
			if (_tcsncmp(pszFileName, _T("."), MAX_PATH) != 0 &&
				_tcsncmp(pszFileName, _T(".."), MAX_PATH) != 0)
			{
				nIsEmpty = false;
				break;
			}
			nRetCode = ::FindNextFile(hFindFile, &FindFileData);
			if (!nRetCode)
			{
				if (::GetLastError() == ERROR_NO_MORE_FILES)
					break;
			}
		}
		nRetCode = ::FindClose(hFindFile);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (!nIsEmpty)
	{
		cout<<pszFileName;
	}
	KGLOG_PROCESS_ERROR(nIsEmpty);
	
	nResult = true;
Exit0:
	return nResult;
}