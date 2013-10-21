#include "stdafx.h"
#include "KTestUpdaterPrepare.h"
#include "KRegInfo.h"
#include "KConfig.h" 

//测试数据
const TCHAR CFG_HIGH_VERSION[]		= _T("1-0-8-1341");			//版本信息文件（version.cfg）测试用版本号
const TCHAR CFG_LOW_VERSION[]		= _T("1-0-8-1300");
const TCHAR REG_HIGH_VERSION[]		= _T("1.0.8.1341");			//注册表测试用版本号
const TCHAR REG_LOW_VERSION[]		= _T("1.0.8.1300");
const TCHAR VALIDATE_HIGH_VERSION[]	= _T("1.0.8.1341");			//验证结果版本号
const TCHAR VALIDATE_LOW_VERSION[]	= _T("1.0.8.1300");
const TCHAR GU_CONFIG_SECTION[]		= _T("server_1");			//配置文件（gameupdater.ini）中服务器的Section名称 
const TCHAR GU_CONFIG_KEY[]			= _T("ip");					//配置文件（gameupdater.ini）中ip的Key名称

void KTestUpdaterPrepare::setUp()
{
	int nRetCode = false;
	int nResult  = false;
	m_bUpdateDirCreated = false;
	m_szAppDir[0]		 = _T('\0');
	m_szGUconfig[0]		 = _T('\0');
	m_szVersionFile[0]	 = _T('\0');
	m_szUpdateTempDir[0] = _T('\0');
	//构造验证用的临时更新目录
	nRetCode = _sntprintf(m_szUpdateTempDir, MAX_PATH, _T("%s\\%s\\"), g_szAppPath, TEMP_UPDATE_DIR);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	m_szUpdateTempDir[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CreateDirectory(m_szUpdateTempDir, NULL);
	KGLOG_PROCESS_ERROR(nRetCode || GetLastError() == ERROR_FILE_EXISTS);
	m_bUpdateDirCreated = true;
	//构造验证用的程序运行目录，比全局的程序运行目录（g_szAppPath）在最后多一个'\'
	nRetCode = _sntprintf(m_szAppDir, MAX_PATH, _T("%s\\"), g_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	m_szAppDir[MAX_PATH - 1] = _T('\0');

	nResult = true;
Exit0:
	if (!nResult && m_bUpdateDirCreated)
	{
		nRetCode = ::RemoveDirectory(m_szUpdateTempDir);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Remove Update Directory Failed");
		}
	}
	CPPUNIT_ASSERT(nResult);
}
void KTestUpdaterPrepare::tearDown()
{
	int nRetCode = false;
	int nResult  = false;

	if (m_bUpdateDirCreated)
	{
		nRetCode = ::RemoveDirectory(m_szUpdateTempDir);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Remove Update Directory Failed");
		}
	}

	nResult = true;
//Exit0:
	CPPUNIT_ASSERT(nResult);
}

int KTestUpdaterPrepare::PrepareVersionFile(const TCHAR cszVersion[])
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bPrepareVersionFile = false;
	TCHAR szTestResFile[MAX_PATH];

	ASSERT(cszVersion);

	//准备测试用版本信息文件
	nRetCode = _sntprintf(szTestResFile, MAX_PATH, _T("%s\\%s"), g_szTestingResDir, VERSION_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestResFile[MAX_PATH - 1] = _T('\0');
	nRetCode = _sntprintf(m_szVersionFile, MAX_PATH, _T("%s\\%s"), g_szAppPath, VERSION_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	m_szVersionFile[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CopyFile(szTestResFile, m_szVersionFile, FALSE);
	KGLOG_PROCESS_ERROR(nRetCode);
	bPrepareVersionFile = true;
	//修改版本号用于测试
	nRetCode = KTestCommon::ModifyVersionFile(m_szVersionFile, cszVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult && bPrepareVersionFile)
	{
		//删除版本信息文件
		nRetCode = ::DeleteFile(m_szVersionFile);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Delete version.cfg failed");
		}
	}
	return nResult;
}

int KTestUpdaterPrepare::PrepareGUconfigFile(const TCHAR cszRemotePath[])
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bPrepareGUconfigFile = false;
	TCHAR szTestResFile[MAX_PATH];

	ASSERT(cszRemotePath);

	nRetCode = _sntprintf(szTestResFile, MAX_PATH, _T("%s\\%s"), g_szTestingResDir, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestResFile[MAX_PATH - 1] = _T('\0');
	nRetCode = _sntprintf(m_szGUconfig, MAX_PATH, _T("%s\\%s"), g_szAppPath, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	m_szGUconfig[MAX_PATH - 1] = _T('\0');
	nRetCode = ::CopyFile(szTestResFile, m_szGUconfig, FALSE);
	KGLOG_PROCESS_ERROR(nRetCode);
	bPrepareGUconfigFile = true;
	//修改测试用配置文件的服务器ip地址，用于测试
	nRetCode = ::WritePrivateProfileString(
		GU_CONFIG_SECTION, 
		GU_CONFIG_KEY, 
		cszRemotePath, 
		m_szGUconfig);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult && bPrepareGUconfigFile)
	{
		//删除配置文件
		nRetCode = ::DeleteFile(m_szGUconfig);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Delete gameupdater.ini failed");
		}
	}
	return nResult;
}

void KTestUpdaterPrepare::TestPrepareSuccess()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bPrepareVersionFile  = false;
	BOOL bPrepareGUconfigFile = false;
	KConfig Config;
	//准备用例测试环境
	nRetCode = PrepareVersionFile(CFG_HIGH_VERSION);//1-0-8-1341
	KGLOG_PROCESS_ERROR(nRetCode);
	bPrepareVersionFile = true;
	nRetCode = PrepareGUconfigFile(REMOTE_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	bPrepareGUconfigFile = true;
	//执行加载
	nRetCode = Config.LoadConfig();
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证结果
	nRetCode = _tcsncmp(Config.GetCurrentVersion(), VALIDATE_HIGH_VERSION, MAX_VERSION_LENGTH);//1.0.8.1341
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _tcsncmp(Config.GetVersionLineName(), VERSION_LINE_NAME, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _tcsncmp(Config.GetAppPath(), m_szAppDir, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _tcsncmp(Config.GetUpdatePath(), m_szUpdateTempDir, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _tcsncmp(Config.GetRemotePath(), DOWNLOAD_URL, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	if (bPrepareVersionFile)
	{
		//删除版本信息文件
		nRetCode = ::DeleteFile(m_szVersionFile);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Delete version.cfg failed");
		}
	}
	if (bPrepareGUconfigFile)
	{
		//删除配置文件
		nRetCode = ::DeleteFile(m_szGUconfig);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Delete gameupdater.ini failed");
		}
	}
	
	CPPUNIT_ASSERT(nResult);
}

void KTestUpdaterPrepare::TestLostVersionFile()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bPrepareGUconfigFile = false;
	size_t uLength = 0;
	KConfig Config;

	nRetCode = PrepareGUconfigFile(REMOTE_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	bPrepareGUconfigFile = true;

	nRetCode = Config.LoadConfig();
	KGLOG_PROCESS_ERROR(nRetCode == false);
	uLength = _tcslen(Config.GetCurrentVersion());
	KGLOG_PROCESS_ERROR(uLength == 0);
	KGLogPrintf(KGLOG_INFO, "成功验证\"无法获取版本信息文件中的版本\"");

	nResult = true;
Exit0:
	if (bPrepareGUconfigFile)
	{
		//删除配置文件
		nRetCode = ::DeleteFile(m_szGUconfig);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Delete gameupdater.ini failed");
		}
	}
	CPPUNIT_ASSERT(nResult);
}

void KTestUpdaterPrepare::TestLostConfigFile()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bPrepareVersionFile = false;
	size_t uLength = 0;
	KConfig Config;

	nRetCode = PrepareVersionFile(CFG_HIGH_VERSION);//1-0-8-1341
	KGLOG_PROCESS_ERROR(nRetCode);
	bPrepareVersionFile = true;

	nRetCode = Config.LoadConfig();
	KGLOG_PROCESS_ERROR(nRetCode == false);
	uLength = _tcslen(Config.GetRemotePath());
	KGLOG_PROCESS_ERROR(uLength == 0);
	KGLogPrintf(KGLOG_INFO, "成功验证\"无法获取配置文件中的服务器ip地址\"");

	nResult = true;
Exit0:
	if (bPrepareVersionFile)
	{
		//删除版本信息文件
		nRetCode = ::DeleteFile(m_szVersionFile);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Delete version.cfg failed");
		}
	}
	CPPUNIT_ASSERT(nResult);
}
