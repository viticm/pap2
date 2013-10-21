#include "KGTestStressSaveAndLoad.h"

const char TEST_CASE_SAVE_LOAD[] = "SaveAndLoadSressTesting";

KGTestStressSaveAndLoad::KGTestStressSaveAndLoad()
{
	nMaxRunTimes  = 50;
	nMaxRunFiles  = 10;
	nCurrentFiles = 0;
}

KGTestStressSaveAndLoad::~KGTestStressSaveAndLoad()
{
	UnInit();
}

int KGTestStressSaveAndLoad::Init(IKG3DEngineManager*		pEngineMgr,
								  IKG3DAutoTestInterface*	pTestInterface,
								  const char				cszClientPath[],
								  const char				cszResultPath[])
{
	int nResult	 = false;
	int nRetCode = false;

	nRetCode = KGMaterialTestCase::Init(pEngineMgr, pTestInterface, cszClientPath, cszResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = TestSaveAndLoad.SetCaseName(TEST_CASE_SAVE_LOAD);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = TestSaveAndLoad.Init(pEngineMgr, pTestInterface, cszClientPath, cszResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KGTestStressSaveAndLoad::UnInit()
{
	TestSaveAndLoad.UnInit();
	KGMaterialTestCase::UnInit();
}

void KGTestStressSaveAndLoad::RunTest(const char cszFileName[])
{
	if (nCurrentFiles < nMaxRunFiles)
	{
		for (int i = 0; i < nMaxRunTimes; i++)
		{
			TestSaveAndLoad.RunTest(cszFileName);
		}
		nCurrentFiles++;
	}
}