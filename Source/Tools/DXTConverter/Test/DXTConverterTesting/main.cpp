#include "stdafx.h"
#include "cppunit/KG_Cppunit.h"

const char TESTING_NAME[]     = "DXTConverterTesting";
const char TESTING_LOG_DIR[]  = "TestingLogs";
const char TESTING_LOG_NAME[] = "DXTConverterTestingLogs";

int main(int nArgc, char *pszArgv[])
{

	int nRetCode = false;
	int nResult  = false;
	BOOL bLogInited = false;

 	CppUnit::KG_Cppunit TestSuit(TESTING_NAME);

	KGLOG_PARAM LogParam  = {"", "", KGLOG_OPTION_FILE, 65535 * 2};
	nRetCode = strncpy_s(LogParam.szPath, PATH_MAX, TESTING_LOG_DIR, sizeof(TESTING_LOG_DIR));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strncpy_s(LogParam.szIdent, PATH_MAX, TESTING_LOG_NAME, sizeof(TESTING_LOG_NAME));
	KG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = KGLogInit(LogParam, (PVOID)TESTING_LOG_NAME);
	KG_PROCESS_ERROR(nRetCode);
	bLogInited = true;

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
	return nResult ? 0 : 1;
}