#include "stdafx.h"

#include "KTestSelfUpdater.h"
#include "KGU_SelfUpdater.h"

//测试数据
const TCHAR SERVER_LATEST_VERSION[]		 = _T("3.0.0.101");							//服务器上的最新版本。可触发自更新

const TCHAR LATEST_VERSION[]			 = _T("1.0.8.1505");						//更新列表中最新版本号
const TCHAR HEGHER_VERSION[]			 = _T("0.0.0.0");							//本地版本号比服务器的版本号高
const TCHAR EQUAL_VERSION[]				 = _T("2.0.0.216");							//本地版本号与服务器的版本号相等

const TCHAR LATEST_VERSION_2[]			 = _T("1-0-8-1505");						//更新列表中最新版本号
const TCHAR UPDATE_ONE_VERSION[]		 = _T("1.0.8.1502");						//更新一个更新包的版本

const TCHAR COMBINEPAK_ONE_VERSION[]	 = _T("c277cd2e3688ec26c2475d16be1a0124");	//更新一个版本，合包后的MD5
const TCHAR PAK_FILE_MD5_B[]			 = _T("deb12820df829931492e814b9a7ed2f1");	//更新前的MD5值
const TCHAR PACKAGE_FILE_MD5[]			 = _T("be78697298eab28748be8ff8a420ee20");	//更新后没有分包情况的包配置文件(package.ini)的MD5值
const TCHAR VERSION_FILE_MD5[]			 = _T("b3bbd5a5080720fa13d8ebd4060960b3");	//更新后版本文件(version.cfg)的MD5值

//const TCHAR NEW_GAMEUPDATER_MD5[]		 = _T("3fc2aa5b6212b7093aab64c3ff740b8a");	//自更新后新的GameUpdater.exe的MD5值
const TCHAR NEW_GAMEUPDATER_INI_MD5[]	 = _T("89c7238bbfd8d228b046705eaf5c74fd");	//自更新后新的GameUpdater.ini的MD5值
const TCHAR NEW_UPDATE_DLL_MD5[]		 = _T("9aa277c917e0bb13a8fcb9f4ab53b5ad");	//自更新后新的update.dll的MD5值

//const TCHAR OLD_GAMEUPDATER_MD5[]		 = _T("3fc2aa5b6212b7093aab64c3ff740b8a");	//自更新前旧的GameUpdater.exe的MD5值
const TCHAR OLD_GAMEUPDATER_INI_MD5[]	 = _T("0a4c9a3f0ca9873d2850beee7e672ba6");	//自更新前旧的GameUpdater.ini的MD5值
const TCHAR OLD_UPDATE_DLL_MD5[]		 = _T("9aa277c917e0bb13a8fcb9f4ab53b5ad");	//自更新前旧的update.dll的MD5值

const TCHAR TEST_FILE[]					 = _T("Test");								//用于生成与更新后的文件名匹配的空文件，验证覆盖更新新

void KTestSelfUpdater::setUp()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bEventCreated = false;
	TCHAR szUpdateDir[MAX_PATH]	 = _T("");
	TCHAR szPakDir[MAX_PATH]	 = _T("");
	m_bRegSaved = false;

	KTestCommon::CleanPath(g_szTestingSpaceDir);

	//创建pak目录
	nRetCode = _sntprintf(szPakDir, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PAK_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPakDir[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szPakDir, NULL); 
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_FILE_EXISTS);

	//初始化进程信息
	ZeroMemory(&m_ServerStartupinfo, sizeof(m_ServerStartupinfo));
	m_ServerStartupinfo.cb = sizeof(m_ServerStartupinfo);
	ZeroMemory(&m_ServerProcessInfo, sizeof(m_ServerProcessInfo));

	ZeroMemory(&m_GameUpdaterStartupinfo, sizeof(m_GameUpdaterStartupinfo));
	m_GameUpdaterStartupinfo.cb = sizeof(m_GameUpdaterStartupinfo);
	ZeroMemory(&m_GameUpdaterProcessInfo, sizeof(m_GameUpdaterProcessInfo));

	//记录测试前的注册表数据，以便还原
	nRetCode = KTestCommon::SaveRegData();
	KGLOG_PROCESS_ERROR(nRetCode);
	m_bRegSaved = true;

	nRetCode = GetServerVersion(m_szServerVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = SetServerVersion(SERVER_LATEST_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);


	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::tearDown()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = SetServerVersion(m_szServerVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	//还原注册表数据
	if (m_bRegSaved)
	{
		nRetCode = KTestCommon::RestoreRegData();
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "RestoreRegData Failed");
		}
	}
	//清理测试空间
	nRetCode = KTestCommon::CleanPath(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestNoUpdatePath()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH];

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, MAX_TIMEOUT);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证更新后的数据
	nRetCode = ValidateFileData();
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini不被修改
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestNoIniFile()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH];

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\gameupdater.ini"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = DeleteFile(szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, 30 * 1000);//30秒超时
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5不变
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PAK_FILE_MD5_B);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestReadOnlyFiles()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH];

	//准备GameUpdater运行的测试资源 
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = KTestCommon::SetAllFilesAttributes(g_szTestingSpaceDir, FILE_ATTRIBUTE_READONLY);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, 1 * 60 * 1000);//1分钟超时
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证更新后的数据
	nRetCode = ValidateFileData();
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	KTestCommon::SetAllFilesAttributes(g_szTestingSpaceDir, FILE_ATTRIBUTE_NORMAL);
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestGameUpdaterIniURL()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szURL[MAX_PATH] = {0};

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//修改gameupdater.ini
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	nRetCode = ::WritePrivateProfileString(GAME_CONFIG_SECTION_NAME, _T("URL"), _T("http://127.0.0.1/jx3/zhcn/"), szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = ::WritePrivateProfileString(GAME_CONFIG_SECTION_NAME, _T("ip"), _T("Error IP For Test"), szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, 1 * 60 * 1000);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证更新后的数据
	nRetCode = ValidateFileData();
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini不被修改
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestSelfUpdater_Normal()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempPath[MAX_PATH] = {0};
	TCHAR szTestSpacePath[MAX_PATH] = {0};
	TCHAR szAppName[MAX_PATH] = {0};
	TCHAR szServerName[MAX_PATH] = {0};
	KGU_SelfUpdater selfUpdater;
	int nNeedSelfUpdateFlag = false;

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//启动http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szServerName[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(
		NULL,
		szServerName,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&m_ServerStartupinfo,
		&m_ServerProcessInfo
		); 
	KGLOG_PROCESS_ERROR(nRetCode); 

	//等待服务器进程启动完毕
	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);	
	Sleep(2000);//临时解决。有时服务器会启动较慢。导致游戏更新时找不到服务器而失败

	//开始自更新
	nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\update\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempPath[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szTempPath, NULL);
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_FILE_EXISTS);

	nRetCode = _sntprintf(szTestSpacePath, MAX_PATH, _T("%s\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestSpacePath[MAX_PATH - 1] = _T('\0');

	nRetCode = _sntprintf(szAppName, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szAppName[MAX_PATH - 1] = _T('\0');

	nRetCode = selfUpdater.Init(szAppName, szTestSpacePath, szTempPath, NULL);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = selfUpdater.CheckNeedUpdate(&nNeedSelfUpdateFlag);
	KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdater.CheckNeedUpdate()");
	KGLOG_PROCESS_ERROR(nNeedSelfUpdateFlag == 1);
	nRetCode = selfUpdater.DownloadFiles();
	KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdater.DownloadFiles()");
	KTestCommon::CloseProcess(GAMEUPDATER_PROC_NAME);
	nRetCode = selfUpdater.UpdateFiles();
	KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdater.UpdateFiles()");

	//验证gamedater.exe, gameupdater.ini, update.dll自更新成功
	//nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szTempPath[MAX_PATH - 1] = _T('\0');
	//nRetCode = KTestCommon::ValidateFileMD5(szTempPath, NEW_GAMEUPDATER_MD5);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempPath[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempPath, NEW_GAMEUPDATER_INI_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\update.dll"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempPath[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempPath, NEW_UPDATE_DLL_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	selfUpdater.UnInit(NULL);
	//关闭http服务器进程
	if (m_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		m_ServerProcessInfo.hProcess = NULL;
	}
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestSelfUpdater_NoIni()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szTempPath[MAX_PATH] = {0};
	TCHAR szTestSpacePath[MAX_PATH] = {0};
	TCHAR szAppName[MAX_PATH] = {0};
	TCHAR szServerName[MAX_PATH] = {0};
	KGU_SelfUpdater selfUpdater;

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\gameupdater.ini"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = DeleteFile(szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//启动http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szServerName[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(
		NULL,
		szServerName,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&m_ServerStartupinfo,
		&m_ServerProcessInfo
		); 
	KGLOG_PROCESS_ERROR(nRetCode); 

	//等待服务器进程启动完毕
	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);	
	Sleep(2000);//临时解决。有时服务器会启动较慢。导致游戏更新时找不到服务器而失败

	//开始自更新
	nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\update\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempPath[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szTempPath, NULL);
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_FILE_EXISTS);

	nRetCode = _sntprintf(szTestSpacePath, MAX_PATH, _T("%s\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestSpacePath[MAX_PATH - 1] = _T('\0');

	nRetCode = _sntprintf(szAppName, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szAppName[MAX_PATH - 1] = _T('\0');

	nRetCode = selfUpdater.Init(szAppName, szTestSpacePath, szTempPath, NULL);
	CPPUNIT_ASSERT(!nRetCode);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	selfUpdater.UnInit(NULL);
	//关闭http服务器进程
	if (m_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		m_ServerProcessInfo.hProcess = NULL;
	}	
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestSelfUpdater_ErrParam()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szTempPath[MAX_PATH] = {0};
	TCHAR szTestSpacePath[MAX_PATH] = {0};
	TCHAR szAppName[MAX_PATH] = {0};
	TCHAR szServerName[MAX_PATH] = {0};
	KGU_SelfUpdater selfUpdater;

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\gameupdater.ini"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = DeleteFile(szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//启动http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szServerName[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(
		NULL,
		szServerName,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&m_ServerStartupinfo,
		&m_ServerProcessInfo
		); 
	KGLOG_PROCESS_ERROR(nRetCode); 

	//等待服务器进程启动完毕
	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);	
	Sleep(2000);//临时解决。有时服务器会启动较慢。导致游戏更新时找不到服务器而失败

	//开始自更新
	nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\update\\ErrorPath\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempPath[MAX_PATH - 1] = _T('\0');

	nRetCode = _sntprintf(szTestSpacePath, MAX_PATH, _T("%s\\ErrorPath"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestSpacePath[MAX_PATH - 1] = _T('\0');

	nRetCode = _sntprintf(szAppName, MAX_PATH, _T("%s\\ErrorPath\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szAppName[MAX_PATH - 1] = _T('\0');

	nRetCode = selfUpdater.Init(szAppName, szTestSpacePath, szTempPath, NULL);
	CPPUNIT_ASSERT(!nRetCode);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	selfUpdater.UnInit(NULL);
	//关闭http服务器进程
	if (m_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		m_ServerProcessInfo.hProcess = NULL;
	}
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestSelfUpdater_ErrServerURL()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szTempPath[MAX_PATH] = {0};
	TCHAR szTestSpacePath[MAX_PATH] = {0};
	TCHAR szAppName[MAX_PATH] = {0};
	TCHAR szServerName[MAX_PATH] = {0};
	KGU_SelfUpdater selfUpdater;
	int nNeedSelfUpdateFlag = false;

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//修改gameupdater.ini
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	nRetCode = ::WritePrivateProfileString(GAME_CONFIG_SECTION_NAME, _T("ip"), _T("Error IP For Test"), szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//启动http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szServerName[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(
		NULL,
		szServerName,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&m_ServerStartupinfo,
		&m_ServerProcessInfo
		); 
	KGLOG_PROCESS_ERROR(nRetCode); 

	//等待服务器进程启动完毕
	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);	
	Sleep(2000);//临时解决。有时服务器会启动较慢。导致游戏更新时找不到服务器而失败

	//开始自更新
	nRetCode = _sntprintf(szTempPath, MAX_PATH, _T("%s\\update\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempPath[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szTempPath, NULL);
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_FILE_EXISTS);

	nRetCode = _sntprintf(szTestSpacePath, MAX_PATH, _T("%s\\"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestSpacePath[MAX_PATH - 1] = _T('\0');

	nRetCode = _sntprintf(szAppName, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szAppName[MAX_PATH - 1] = _T('\0');

	nRetCode = selfUpdater.Init(szAppName, szTestSpacePath, szTempPath, NULL);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = selfUpdater.CheckNeedUpdate(&nNeedSelfUpdateFlag);
	CPPUNIT_ASSERT(!nRetCode);

	nResult = true;
Exit0:
	selfUpdater.UnInit(NULL);
	//关闭http服务器进程
	if (m_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		m_ServerProcessInfo.hProcess = NULL;
	}
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestVersionHigher()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szServerVersion[32] = {0};//版本号不超过32个字符如："1-0-0-1505"

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//设置服务器上的版本
	nRetCode = GetServerVersion(szServerVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = SetServerVersion(HEGHER_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, MAX_TIMEOUT);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini不被修改
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证gamedater.exe, gameupdater.ini, update.dll没有自更新
	//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szTempFile[MAX_PATH - 1] = _T('\0');
	//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, OLD_GAMEUPDATER_MD5);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, OLD_GAMEUPDATER_INI_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\update.dll"), g_szTestingSpaceDir);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szTempFile[MAX_PATH - 1] = _T('\0');
	//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, OLD_UPDATE_DLL_MD5);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	SetServerVersion(szServerVersion);//恢复服务器上的版本
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestVersionEqual()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szServerVersion[32] = {0};//版本号不超过32个字符如："1-0-0-1505"

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//设置服务器上的版本
	nRetCode = GetServerVersion(szServerVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = SetServerVersion(EQUAL_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, MAX_TIMEOUT);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini不被修改
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证gamedater.exe, gameupdater.ini, update.dll没有自更新
	//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szTempFile[MAX_PATH - 1] = _T('\0');
	//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, OLD_GAMEUPDATER_MD5);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, OLD_GAMEUPDATER_INI_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\update.dll"), g_szTestingSpaceDir);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szTempFile[MAX_PATH - 1] = _T('\0');
	//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, OLD_UPDATE_DLL_MD5);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	SetServerVersion(szServerVersion);//恢复服务器上的版本
	CPPUNIT_ASSERT(nResult);
}

void KTestSelfUpdater::TestStartSeveralUpdateProc()
{
	int nResult  = false;
	int nRetCode = false;
	STARTUPINFO			GameUpdaterStartupinfo;	//GameUpdater进程启动信息
	PROCESS_INFORMATION GameUpdaterProcessInfo;	//GameUpdater进程信息结构体
	TCHAR szTempFile[MAX_PATH] = {0};
	TCHAR szGameUpdaterPath[MAX_PATH] = {0};

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备用于合包的目标包
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//启动多一个GameUpdater进程
	ZeroMemory(&GameUpdaterStartupinfo, sizeof(GameUpdaterStartupinfo));
	GameUpdaterStartupinfo.cb = sizeof(GameUpdaterStartupinfo);
	ZeroMemory(&GameUpdaterProcessInfo, sizeof(GameUpdaterProcessInfo));
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
		&GameUpdaterStartupinfo,
		&GameUpdaterProcessInfo
		);
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, MAX_TIMEOUT);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证更新后的数据
	nRetCode = ValidateFileData();
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini不被修改
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak的MD5
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_ONE_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (GameUpdaterProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(GameUpdaterProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait GameUpdater Process Failed");
		}
		GameUpdaterProcessInfo.hProcess = NULL;
	}
	CPPUNIT_ASSERT(nResult);
}




int KTestSelfUpdater::PrepareExistFiles()
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

int KTestSelfUpdater::ValidateFileData()
{
	int nResult  = false;
	int nRetCode = false;

	TCHAR szTempDir[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];
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
	KGLOG_PROCESS_ERROR(nIsEmpty);
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

	//验证gameupdater.exe
	//nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
	//KGLOG_PROCESS_ERROR(nRetCode != -1);
	//szTempFile[MAX_PATH - 1] = _T('\0');
	//nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_GAMEUPDATER_MD5);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestSelfUpdater::PrepareTestSpace(const TCHAR cszTestVersion[])
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVersionFile[MAX_PATH];

	ASSERT(cszTestVersion);

	//准备version.cfg，并修改为测试的版本号
	nRetCode = KTestCommon::PrepareTestResoure(VERSION_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _sntprintf(szVersionFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, VERSION_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szVersionFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ModifyVersionFile(szVersionFile, cszTestVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
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
	//nRetCode = KTestCommon::PrepareTestResoure(UPDATE_DLL);
	//KGLOG_PROCESS_ERROR(nRetCode);
	//准备update.swf
	nRetCode = KTestCommon::PrepareTestResoure(UPDATER_SWF);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestSelfUpdater::StartProc()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szServerName[MAX_PATH] = {0};
	TCHAR szGameUpdaterPath[MAX_PATH] = {0};

	//启动http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szServerName[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateProcess(
		NULL,
		szServerName,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&m_ServerStartupinfo,
		&m_ServerProcessInfo
		); 
	KGLOG_PROCESS_ERROR(nRetCode); 

	//等待服务器进程启动完毕
	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);	
	Sleep(2000);//临时解决。有时服务器会启动较慢。导致游戏更新时找不到服务器而失败

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
		&m_GameUpdaterStartupinfo,
		&m_GameUpdaterProcessInfo
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		WaitAndCloseProc(LATEST_VERSION_2, 0);
	}
	return nResult;
}

int KTestSelfUpdater::WaitAndCloseProc(const TCHAR szLatestVersion[], UINT uMaxTimeOut)
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVersionFile[MAX_PATH] = {0};
	TCHAR szNewVersion[32] = {0};; //版本号长度不超过32个字符"1-0-8-1505"

	//等待并关闭GameUpdater进程
	nRetCode = _sntprintf(szVersionFile, MAX_PATH, _T("%s\\version.cfg"), g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	int nSleepTimes = uMaxTimeOut / 10000;
	for (int i = 0; i < nSleepTimes; i++)
	{
		nRetCode = KTestCommon::GetCurVersion(szVersionFile, szNewVersion);
		KGLOG_PROCESS_ERROR(nRetCode);
		//KG_PROCESS_SUCCESS(_tcscmp(szLatestVersion, szNewVersion) == 0);
		if (_tcscmp(szLatestVersion, szNewVersion) == 0)
			break;
		::Sleep(10000);
	}

	if (m_GameUpdaterProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_GameUpdaterProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait GameUpdater Process Failed");
		}
		m_GameUpdaterProcessInfo.hProcess = NULL;
	}

	KTestCommon::CloseProcess(GAMEUPDATER_PROC_NAME);

	//关闭http服务器进程
	if (m_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		m_ServerProcessInfo.hProcess = NULL;
	}

	nResult = true;
Exit0:
	Sleep(3000);//临时解决
	return nResult;
}

int KTestSelfUpdater::SetServerVersion(const TCHAR* pszVersion)
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVersionFileName[MAX_PATH] = {0};

	KGLOG_PROCESS_ERROR(pszVersion);

	nRetCode = _sntprintf(szVersionFileName, MAX_PATH, _T("%s\\HttpServer\\jx3\\zhcn\\updaterV2\\updaterversion.txt"), g_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nRetCode = ::WritePrivateProfileString(
		REG_CURRENT_VERSION_NAME, 
		"LatestVersion",
		pszVersion,
		szVersionFileName
		);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestSelfUpdater::GetServerVersion(OUT TCHAR* pszRetVersion)
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVersionFileName[MAX_PATH] = {0};

	KGLOG_PROCESS_ERROR(pszRetVersion);

	nRetCode = _sntprintf(szVersionFileName, MAX_PATH, _T("%s\\HttpServer\\jx3\\zhcn\\updaterV2\\updaterversion.txt"), g_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nRetCode = ::GetPrivateProfileString(
		REG_CURRENT_VERSION_NAME, 
		"LatestVersion",
		NULL,
		pszRetVersion,
		MAX_PATH,
		szVersionFileName
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}