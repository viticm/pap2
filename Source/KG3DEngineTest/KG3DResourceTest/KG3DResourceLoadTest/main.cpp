#include "KG3DLoadResTestManager.h"

int main(int nArgc, char *pszArgv[])
{
	int nResult  = false;
	int nRetCode = false;
	int TestingInitFlag		 = false;
	int nTestLoadResInitFlag = false;
	int nCheckPathIndex = 1;
	KG3DLoadResTestManager TestLoadRes;

	nRetCode = KGTestCommon::TestingInit(TEST_PROJECT_NAME);
 	KGLOG_PROCESS_ERROR(nRetCode);
 	TestingInitFlag = true;

	if (nArgc == 3)
	{
		nCheckPathIndex = 2;
	}

	nRetCode = TestLoadRes.Init(pszArgv[1]);
	KGLOG_PROCESS_ERROR(nRetCode);
	nTestLoadResInitFlag = true;

 	nRetCode = TestLoadRes.Run(pszArgv[nCheckPathIndex]);
 	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = TestLoadRes.AnalyseLog();
	KGLOG_PROCESS_ERROR(nRetCode);

 	nRetCode = TestLoadRes.OutputResult();
 	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (nTestLoadResInitFlag)
	{
		TestLoadRes.UnInit();
	}
	if (TestingInitFlag)
	{
		KGTestCommon::TestingUnit(TEST_PROJECT_NAME);
	}
	return nResult ? 0 : 1;
}