#include "KG3DMapFormatTest.h"

int main(int nArgc, char *pszArgv[])
{
	int nResult  = false;
	int nRetCode = false;
	int TestingInitFlag        = false;
	int nTestMapFormatInitFlag = false;
	KG3DMapFormatTest TestMapFormat;

	nRetCode = KGTestCommon::TestingInit(TEST_PROJECT_NAME);
	KG_PROCESS_ERROR(nRetCode);
	TestingInitFlag = true;

	nRetCode = TestMapFormat.Init(pszArgv[1]);
	KGLOG_PROCESS_ERROR(nRetCode);
	nTestMapFormatInitFlag = true;

	nRetCode = TestMapFormat.Run();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (nTestMapFormatInitFlag)
	{
		TestMapFormat.UnInit();
	}
	if (TestingInitFlag)
	{
		KGTestCommon::TestingUnit(TEST_PROJECT_NAME);
	}
	return nResult ? 0 : 1;
}