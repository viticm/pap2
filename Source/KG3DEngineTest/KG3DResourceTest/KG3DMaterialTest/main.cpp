#include "KGTestMTLResource.h"

int main(int nArgc, char *pszArgv[])
{
	int nResult  = false;
	int nRetCode = false;
	int nEngineManagerInitFlag = false;
	int nTestMtlInitFlag	   = false;
	int TestingInitFlag		   = false;
	int nCheckPathIndex = 1;
	KGTestMTLResource TesMtl;

	nRetCode = KGTestCommon::TestingInit(TEST_PROJECT_NAME);
	KG_PROCESS_ERROR(nRetCode);
	TestingInitFlag = true;

	KGLOG_PROCESS_ERROR(nArgc == 2 || nArgc == 3);
	if (nArgc == 3)
	{
		nCheckPathIndex = 2;
	}
	
	nRetCode = TesMtl.Init(pszArgv[1]);
	KGLOG_PROCESS_ERROR(nRetCode);
	nTestMtlInitFlag = true;

	nRetCode = TesMtl.RunTest(pszArgv[nCheckPathIndex]);
	KGLOG_PROCESS_ERROR(nRetCode);
	
	nResult = true;
Exit0:
	if (TestingInitFlag)
	{
		KGTestCommon::TestingUnit(TEST_PROJECT_NAME);
	}
	if (nTestMtlInitFlag)
	{
		TesMtl.UnInit();
	}
	return nResult ? 0 : 1;
}