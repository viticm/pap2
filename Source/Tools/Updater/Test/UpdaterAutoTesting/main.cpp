
#include "stdafx.h"
#include "KTestCommon.h"
#include "cppunit/KG_Cppunit.h"
//#include "KUtil.h"

const TCHAR TESTING_NAME[]     = _T("UpdaterAutoTesting");
const TCHAR TESTING_LOG_DIR[]  = _T("TestingLogs");
const TCHAR TESTING_LOG_NAME[] = _T("UpdaterAutoTesting");

int Init()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR *pSlash = NULL;

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

	nResult = true;
Exit0:
	return nResult;
}

int main(int nArgc, char *pszArgv[])
{
	int nRetCode = false;
	int nResult  = false;
	BOOL bLogInited = false;

	CppUnit::KG_Cppunit TestSuit(TESTING_NAME);

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

	nRetCode = TestSuit.RunTest();
	KG_PROCESS_ERROR(nRetCode);

 	nRetCode = TestSuit.Output();
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