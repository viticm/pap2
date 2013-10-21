
#include "stdafx.h"
#include "KTestFullRun.h"
#include "KFileMD5Maker.h"
#include "KRegInfo.h"
#include <iostream>
using namespace std;

//测试数据
const TCHAR LATEST_VERSION[]			 = _T("1.0.8.1505");						//更新列表中最新版本号
const TCHAR LATEST_VERSION_2[]			 = _T("1-0-8-1505");						//更新列表中最新版本号
const TCHAR UPDATE_SEVERAL_VERSION[]	 = _T("1.0.8.1492");						//更新多个版本，这里测试更新12个版本
const TCHAR UPDATE_ONE_VERSION[]		 = _T("1.0.8.1502");						//更新一个更新包的版本

const TCHAR COMBINEPAK_ONE_VERSION[]	 = _T("c277cd2e3688ec26c2475d16be1a0124");	//更新一个版本，合包后的MD5
const TCHAR NEWPAK_ONE_VERSION[]		 = _T("0ff7e25f87a94aec08eb0ec70bc02b05");	//更新一个版本，分包后新包的MD5
const TCHAR COMBINEPAK_SEVERAL_VERSION[] = _T("3de033f364baf7c25fe6f61f0a45354b");	//更新多个版本，合包后的MD5，本次测试更新12个版本
const TCHAR NEWPAK_SEVERAL_VERSION[]	 = _T("7434a8c1bef4db591246a8a9840b4c40");	//更新多个版本，分包后新包的MD5，本次测试更新三个版本

const TCHAR COMBINE_TAGETPAK_MD5[]		 = _T("a981130cf2b7e09f4686dc273cf7187e");	//用于合包的目标包（2G）的MD5值
const TCHAR PACKAGE_FILE_MD5[]			 = _T("be78697298eab28748be8ff8a420ee20");	//更新后没有分包情况的包配置文件(package.ini)的MD5值
const TCHAR VERSION_FILE_MD5[]			 = _T("b3bbd5a5080720fa13d8ebd4060960b3");	//更新后版本文件(version.cfg)的MD5值

const TCHAR TEST_FILE[]					 = _T("Test");								//用于生成与更新后的文件名匹配的空文件，验证覆盖更新新
const TCHAR SETTING_DIR[]				 = _T("settings");
const size_t SIZE_ONCE_WRITE = 2 * 1024 * 1024;				//分包条件，默认为1.8G，这里测试2G(每次写入2M，共写入1024次)，保证分包条件
const int WRITE_TIMES        = 1024;						//写入次数

void KTestFullRun::setUp()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bEventCreated = false;
	TCHAR szUpdateDir[MAX_PATH]	 = _T("");
	TCHAR szPakDir[MAX_PATH]	 = _T("");
	m_bRegSaved = false;

	//创建pak目录
	nRetCode = _sntprintf(szPakDir, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PAK_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPakDir[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szPakDir, NULL); 
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_FILE_EXISTS);

	//创建update目录
	nRetCode = _sntprintf(szUpdateDir, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, TEMP_UPDATE_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szUpdateDir[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(szUpdateDir, NULL);
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

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestFullRun::tearDown()
{
	int nResult  = false;
	int nRetCode = false;

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
	//nRetCode = KTestCommon::CleanPathExceptLogAndPak(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestFullRun::TestUpdateOneVersionAndCombinePack()
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

void KTestFullRun::TestUpdateOneVersionAndNewPack()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH];

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);//1.0.8.1336 to 1.0.8.1337
	KGLOG_PROCESS_ERROR(nRetCode);

	//创建满足分包条件的合并目标包
	nRetCode = CreatePakForNewPack();
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备与更新后的文件名匹配的空文件，用于验证覆盖更新
	nRetCode = PrepareExistFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, MAX_TIMEOUT);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证数据
	nRetCode = ValidateFileData();
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

	//验证update_1.pak
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINE_TAGETPAK_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestFullRun::TestUpdateSeveralVersionAndCombinePack()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempFile[MAX_PATH];

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_SEVERAL_VERSION);
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

	//验证数据
	nRetCode = ValidateFileData();
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证package.ini
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, PACKAGE_FILE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak
	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szTempFile, COMBINEPAK_SEVERAL_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestFullRun::TestUpdateSeveralVersionAndNewPack()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szCombineFile[MAX_PATH];
	TCHAR szNewFile[MAX_PATH];

	//准备GameUpdater运行的测试资源
	nRetCode = PrepareTestSpace(UPDATE_SEVERAL_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//创建满足分包条件的合并目标包
	nRetCode = CreatePakForNewPack();
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备与更新后的文件名匹配的空文件，用于验证覆盖更新
	nRetCode = PrepareExistFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	//运行更新
	nRetCode = StartProc();
	KGLOG_PROCESS_ERROR(nRetCode);

	//关闭更新相关进程
	nRetCode = WaitAndCloseProc(LATEST_VERSION_2, MAX_TIMEOUT);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证数据
 	nRetCode = ValidateFileData();
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证package.ini
	nRetCode = KTestCommon::ValidatePackageFile();
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_2.pak
	nRetCode = _sntprintf(szNewFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_2);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szNewFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szNewFile, NEWPAK_SEVERAL_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证update_1.pak
	nRetCode = _sntprintf(szCombineFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szCombineFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::ValidateFileMD5(szCombineFile, COMBINE_TAGETPAK_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

int KTestFullRun::PrepareExistFiles()
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

int KTestFullRun::ValidateFileData()
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

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::PrepareTestSpace(const TCHAR cszTestVersion[])
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
	//nRetCode = KTestCommon::PrepareTestResoure(UPDATER_SWF);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::CreatePakForNewPack()
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
	KGLOG_PROCESS_ERROR(nFreeBytesAvailable.QuadPart > (SIZE_ONCE_WRITE * WRITE_TIMES));

	nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), PAK_DIR, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTempFile[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_3, szTempFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::ValidateRegData(const TCHAR cszVersion[])
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

int KTestFullRun::StartProc()
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

int KTestFullRun::WaitAndCloseProc(const TCHAR szLatestVersion[], UINT uMaxTimeOut)
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






//
////临时测试运行一天。更新多个升级包的验证，同时验证分包和覆盖更新
//int  KTestFullRun::SetUp_OneDay()
//{
//	int nResult  = false;
//	int nRetCode = false;
//	BOOL bEventCreated = false;
//	TCHAR szUpdateDir[MAX_PATH]	 = _T("");
//	TCHAR szPakDir[MAX_PATH]	 = _T("");
//	m_bRegSaved = false;
//
//	//初始化进程信息
//	ZeroMemory(&m_ServerStartupinfo, sizeof(m_ServerStartupinfo));
//	m_ServerStartupinfo.cb = sizeof(m_ServerStartupinfo);
//	ZeroMemory(&m_ServerProcessInfo, sizeof(m_ServerProcessInfo));
//
//	ZeroMemory(&m_GameUpdaterStartupinfo, sizeof(m_GameUpdaterStartupinfo));
//	m_GameUpdaterStartupinfo.cb = sizeof(m_GameUpdaterStartupinfo);
//	ZeroMemory(&m_GameUpdaterProcessInfo, sizeof(m_GameUpdaterProcessInfo));
//
//	//记录测试前的注册表数据，以便还原
//	nRetCode = KTestCommon::SaveRegData();
//	KGLOG_PROCESS_ERROR(nRetCode);
//	m_bRegSaved = true;
//
//	nResult = true;
//Exit0:
//	return nResult;
//}
//int  KTestFullRun::TearDown_OneDay()
//{
//	int nResult  = false;
//	int nRetCode = false;
//
//	//还原注册表数据
//	if (m_bRegSaved)
//	{
//		nRetCode = KTestCommon::RestoreRegData();
//		if (!nRetCode)
//		{
//			KGLogPrintf(KGLOG_ERR, "RestoreRegData Failed");
//		}
//	}
//	//清理测试空间
//	nRetCode = KTestCommon::CleanPathExceptLogAndPak(g_szTestingSpaceDir);
//	KGLOG_PROCESS_ERROR(nRetCode);
//
//	nResult = true;
//Exit0:
//	return nResult;
//}
//
//void KTestFullRun::TestUpdateSeveralVersionAndNewPack_OneDay()
//{
//	int nResult  = false;
//	int nRetCode = false;
//	int nErrCount = 0;
//	int nSucCount = 0;
//	TCHAR szTempFile[MAX_PATH] = {0};
//	TCHAR szServerName[MAX_PATH] = {0};
//	const TCHAR NEW_GAMEUPDATER_MD5[]		 = _T("52ee7b68ed67f895aedea73ddb6e90fe");	//自更新后新的GameUpdater.exe的MD5值
//	const TCHAR NEW_GAMEUPDATER_INI_MD5[]	 = _T("89c7238bbfd8d228b046705eaf5c74fd");	//自更新后新的GameUpdater.ini的MD5值
//	const TCHAR NEW_UPDATE_DLL_MD5[]		 = _T("9aa277c917e0bb13a8fcb9f4ab53b5ad");	//自更新后新的update.dll的MD5值
//
//	//创建满足分包条件的合并目标包
//	//nRetCode = CreatePakForNewPack();
//	//KGLOG_PROCESS_ERROR(nRetCode);
//
//	//启动http服务器进程
//	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
//	szServerName[MAX_PATH - 1] = _T('\0');
//	nRetCode = ::CreateProcess(NULL, szServerName, NULL, NULL, FALSE, 0, NULL, NULL, &m_ServerStartupinfo, &m_ServerProcessInfo); 
//	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
//	Sleep(3000);
//
//	while (true)
//	{
//		nResult = false;
//		nRetCode = TearDown_OneDay();
//		KGLOG_PROCESS_ERROR(nRetCode);
//		nRetCode = SetUp_OneDay();
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//准备GameUpdater运行的测试资源
//		nRetCode = PrepareTestSpace(UPDATE_ONE_VERSION);
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//准备与更新后的文件名匹配的空文件，用于验证覆盖更新
//		nRetCode = PrepareExistFiles();
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//运行更新
//		nRetCode = StartProc();
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//关闭更新相关进程
//		nRetCode = WaitAndCloseProc(LATEST_VERSION_2, 1 * 60 * 1000);//1分钟超时
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//验证数据
//		nRetCode = ValidateFileData();
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//验证package.ini
//		nRetCode = KTestCommon::ValidatePackageFile();
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//验证update_2.pak
//		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s\\%s"), g_szTestingSpaceDir, PAK_DIR, TEST_PACK_2);
//		KGLOG_PROCESS_ERROR(nRetCode != -1);
//		szTempFile[MAX_PATH - 1] = _T('\0');
//		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEWPAK_ONE_VERSION);
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		//验证gamedater.exe, gameupdater.ini, update.dll自更新成功
//		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, GAMEUPDATER_EXE_NAME);
//		KGLOG_PROCESS_ERROR(nRetCode != -1);
//		szTempFile[MAX_PATH - 1] = _T('\0');
//		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_GAMEUPDATER_MD5);
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, CONFIG_FILE_NAME);
//		KGLOG_PROCESS_ERROR(nRetCode != -1);
//		szTempFile[MAX_PATH - 1] = _T('\0');
//		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_GAMEUPDATER_INI_MD5);
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		nRetCode = _sntprintf(szTempFile, MAX_PATH, _T("%s\\update.dll"), g_szTestingSpaceDir);
//		KGLOG_PROCESS_ERROR(nRetCode != -1);
//		szTempFile[MAX_PATH - 1] = _T('\0');
//		nRetCode = KTestCommon::ValidateFileMD5(szTempFile, NEW_UPDATE_DLL_MD5);
//		KGLOG_PROCESS_ERROR(nRetCode);
//
//		nResult = true;
//Exit0:
//		nResult ? ++nSucCount : ++nErrCount;
//		cout<<endl<<endl<<"****************************************"<<endl
//			<<"失败："<<nErrCount<<endl
//			<<"成功："<<nSucCount<<endl
//			<<"****************************************"<<endl<<endl;
//
//		//if (nErrCount >= 1000 || nSucCount >= 1000)
//		//	break;
//	}
//
//	//关闭http服务器进程
//	if (m_ServerProcessInfo.hProcess != NULL)
//	{
//		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
//		if (!nRetCode)
//		{
//			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
//		}
//		m_ServerProcessInfo.hProcess = NULL;
//	}
//
//	cout<<"==========================================================";
//	cout<<endl<<"成功："<<nSucCount<<'\t'<<"失败："<<nErrCount<<endl;
//	cout<<"=========================================================="<<endl;
//	CPPUNIT_ASSERT(!nErrCount);
//}