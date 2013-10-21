#include "KGTestLoad.h"

const char TEST_CASE_LOAD[] = "ValidateLoadMtl";

KGTestLoad::KGTestLoad(void)
{
}

KGTestLoad::~KGTestLoad(void)
{
	UnInit();
}

int KGTestLoad::Init(IKG3DEngineManager*	 pEngineMgr,
					 IKG3DAutoTestInterface* pTestInterface,
					 const char				 cszClientPath[],
					 const char				 cszResultPath[])
{
	int nResult	 = false;
	int nRetCode = false;

	nRetCode = KGMaterialTestCase::Init(pEngineMgr, pTestInterface, cszClientPath, cszResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = InitResultLog(TEST_CASE_LOAD);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KGTestLoad::RunTest(const char cszFileName[])
{
	int nRetCode = false;
	IEKG3DMaterial* pMtl = NULL;

	_ASSERTE(cszFileName);

	nRetCode = LoadMaterialByNewFunc(cszFileName, &pMtl);
	if (!nRetCode)
	{
		m_MtlCanBeLoaded = false;
		m_vecResult.push_back(string("MaterialFileName: ") + cszFileName);
		m_vecResult.push_back("Load material file failed!");
		OuputResult();
	}
	else
	{
		m_vecResult.clear();
		ReleaseMtl(pMtl);
		m_MtlCanBeLoaded = true;
	}
}

void KGTestLoad::UnInit()
{
	KGMaterialTestCase::UnInit();
}