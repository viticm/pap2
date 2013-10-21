#include "stdafx.h"
#include "KTestRegOperation.h"
#include "KRegInfo.h"
#include "KHttpDownloader.h"

const TCHAR TEST_VERSION[] = _T("1.0.8.1341");

void KTestRegOperation::setUp()
{
	int nResult  = false;
	int nRetCode = false;
	m_bRegSaved = false;
	//记录测试前的注册表数据，以便还原
	nRetCode = KTestCommon::SaveRegData();
	KGLOG_PROCESS_ERROR(nRetCode);
	m_bRegSaved = true;

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestRegOperation::tearDown()
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

	nResult = true;
//Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestRegOperation::TestInstallDirectory()
{
	int nRetCode = false;
	int nResult  = false;
	int nCompareResult = 0;
	TCHAR szVersion[MAX_PATH];
	TCHAR szOldVersion[MAX_PATH] = _T("");

	//采用写入和读取版本信息相互验证
	nRetCode = KRegInfo::WriteCurrentVersion(VERSION_LINE_NAME, TEST_VERSION);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = KRegInfo::ReadCurrentVersion(VERSION_LINE_NAME, szVersion, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	nCompareResult = _tcsncmp(szVersion, TEST_VERSION, MAX_PATH);
	KGLOG_PROCESS_ERROR(nCompareResult == 0);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestRegOperation::TestVersionInfo()
{
	int nRetCode	= false;
	int nResult		= false;
	int nCompareResult	= 0;
	TCHAR szInstallPath[MAX_PATH];
	TCHAR szOldPath[MAX_PATH] = _T("");

	//采用写入和读取版本信息相互验证
	nRetCode = KRegInfo::WriteInstallPath(VERSION_LINE_NAME, g_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = KRegInfo::ReadInstallPath(VERSION_LINE_NAME, szInstallPath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	nCompareResult = _tcsncmp(szInstallPath, g_szAppPath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nCompareResult == 0);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

