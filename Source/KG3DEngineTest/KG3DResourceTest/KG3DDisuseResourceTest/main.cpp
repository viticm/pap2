#include "KGTestMapDisuseResource.h"

int main(int nArgc, char *pszArgv[])
{
	int nResult  = false;
	int nRetCode = false;
	int TestingInitFlag				   = false;
	int nTestMapDisuseResourceInitFlag = false;
	KGTestMapDisuseResource TestMapDisuseResource;

	nRetCode = KGTestCommon::TestingInit(TEST_PROJECT_NAME);
	KG_PROCESS_ERROR(nRetCode);
	TestingInitFlag = true;

	nRetCode = TestMapDisuseResource.Init();
	KGLOG_PROCESS_ERROR(nRetCode);
	nTestMapDisuseResourceInitFlag = true;

	nRetCode = TestMapDisuseResource.RunTest();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = TestMapDisuseResource.OutputResult();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (nTestMapDisuseResourceInitFlag)
	{
		TestMapDisuseResource.UnInit();
	}
	if (TestingInitFlag)
	{
		KGTestCommon::TestingUnit(TEST_PROJECT_NAME);
	}
	return nResult ? 0 : 1;
}