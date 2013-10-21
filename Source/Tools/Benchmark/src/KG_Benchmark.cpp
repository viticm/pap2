////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_Benchmark.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-8-13 15:31:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "KG_Benchmark.h"
#include "KG_OpenAutomateMode.h"
#include "KG_NormalMode.h"
#include "KG_InfoSender.h"

KGBenchmarkOption g_benchMarkOption;
HINSTANCE g_hInstance;
BOOL g_bNormalMode;

////////////////////////////////////////////////////////////////////////////////
int _ParseArgs(
    const char cszCmdLine[], int *pnRetOAModeFlag, const char **ppcszOAModeArg
)
{
    int nResult  = false;
    int nRetCode = false;
    const char     OPTION[] = "\"-openautomate\"";
    const unsigned OPTION_LENGTH = sizeof(OPTION) / sizeof(OPTION[0]) - 1;
	
	const char     OPTION1[] = "-openautomate";
	const unsigned OPTION_LENGTH1 = sizeof(OPTION1) / sizeof(OPTION1[0]) - 1;

	unsigned uLength = OPTION_LENGTH;
    unsigned uCmdLineLength = 0;
    const char *pcszOAModeArg = NULL;

    ASSERT(cszCmdLine);
    ASSERT(pnRetOAModeFlag);
    ASSERT(ppcszOAModeArg);

    *pnRetOAModeFlag = false;
    *ppcszOAModeArg  = NULL;

    uCmdLineLength = (unsigned)strlen(cszCmdLine);
    KG_PROCESS_SUCCESS(uCmdLineLength == 0);

	
	nRetCode = strncmp(cszCmdLine, OPTION, OPTION_LENGTH);
	if (nRetCode != 0)
	{
		nRetCode = strncmp(cszCmdLine, OPTION1, OPTION_LENGTH1);
		uLength = OPTION_LENGTH1;
	}
    KG_PROCESS_ERROR(nRetCode == 0); // only support one option "-openautomate"
	
    pcszOAModeArg = cszCmdLine + uLength;
    while (*pcszOAModeArg == ' ')
    {
        ++pcszOAModeArg;    // skip space
    }
    KG_PROCESS_ERROR(*pcszOAModeArg != '\0');//"-openautomate" option must have an argument"

	*pnRetOAModeFlag = true;
	*ppcszOAModeArg  = pcszOAModeArg;

	
Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        ::MessageBox(NULL, "USAGE: \r\n    JX3Benchmark [-openautomate argument]",  "HELP", MB_ICONSTOP);
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow
)
{
    int nResult  = false;
    int nRetCode = false;
    int nOAModeFlag = false;
    const char *pcszOAModeArg = NULL;
    KGLOG_PARAM LogParam = {"logs", "RecPlayer", KGLOG_OPTION_FILE, 10000};
    int nLogInitFlag = false;
    int nLoadPackageFiles = false;
    int nExitFlag = false;
    g_hInstance = hInstance;
    KG_USE_ARGUMENT(hInstance);
    KG_USE_ARGUMENT(hPrevInstance);
    KG_USE_ARGUMENT(nCmdShow);
	KMemory::Initialize("Benchmark.memory");
    ::SetUnhandledExceptionFilter(CreateDumpFile);

    g_SetRootPath(NULL);
    g_SetFindFileMode(FALSE);
    g_SetFilePath("");

    nRetCode = g_LoadPackageFiles("config.ini", "PackFile");
    KG_PROCESS_ERROR(nRetCode);

    nLoadPackageFiles = true;

	nRetCode = _ParseArgs(lpCmdLine, &nOAModeFlag, &pcszOAModeArg);
    KG_PROCESS_ERROR(nRetCode);


    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

    //nRetCode = ReadIniFile();
    //KGLOG_PROCESS_ERROR(nRetCode);

    //nRetCode = CheckVersion(&nExitFlag);
    //KGLOG_PROCESS_ERROR(nRetCode);
    //KG_PROCESS_SUCCESS(nExitFlag);

    g_benchMarkOption.SetDefault();
    g_benchMarkOption.LoadFromFile("DebugFiles/BenchMark/BenchMark.ini");
    g_bNormalMode = !nOAModeFlag;
    if (nOAModeFlag)
    {
		nRetCode = RunAsOpenAutomateMode(pcszOAModeArg);//NV 使用的，会使用命令测试的模式
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = RunAsNormalMode();
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    nResult = true;
Exit0:

    //nRetCode = SendSystemInfo();
    //KGLOG_PROCESS_ERROR(nRetCode);

    if (nLoadPackageFiles)
    {
        g_ClearPackageFiles();
    }
    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }
	KMemory::Finalize();
    return nResult ? 0 : -1;
}



VOID KGBenchmarkOption::SetDefault()
{
    ZeroMemory(this, sizeof(*this));
    _tcscpy_s(strCudaRecFile, _T("test.rec"));
	_tcscpy_s(strNormalRecFile, _T("test.rec"));
    bPlayAll = 1;
}

HRESULT KGBenchmarkOption::LoadFromFile( LPCTSTR strFileName )
{
	HRESULT hr = E_FAIL;
    TCHAR strBenchMarkOpPath[] = _T("DebugFiles/BenchMark/BenchMark.ini");

    IIniFile* pIni = g_OpenIniFile(strBenchMarkOpPath);
    KGLOG_PROCESS_ERROR(NULL != pIni);

    KGBenchmarkOption tmpOpt;
    tmpOpt.SetDefault();

    {
        INT nRet = FALSE;
        nRet = (0 < pIni->GetInteger("Global", "DisablePreload", 0, &tmpOpt.bDisablePreload));
        nRet = (0 < pIni->GetInteger("Global", "DisablePreloadShader", 0, &tmpOpt.bDisablePreloadShader));
        nRet = (0 < pIni->GetInteger("Global", "PlayAll", 0, &tmpOpt.bPlayAll));
        KGLOG_PROCESS_ERROR(0 < pIni->GetString("Global", "NormalRecFile", "test.rec", tmpOpt.strNormalRecFile, _countof(tmpOpt.strNormalRecFile)));
		KGLOG_PROCESS_ERROR(0 < pIni->GetString("Global", "CudaRecFile", "test.rec", tmpOpt.strCudaRecFile, _countof(tmpOpt.strCudaRecFile)));
		//nRet = (0 < pIni->GetInteger("Global", "CudaTestEnable", 0, &tmpOpt.bPlayCudaTest));
		//nRet = (0 < pIni->GetInteger("Global", "NoramlTestEnable", 1, &tmpOpt.bPlayNoramlTest));
    }
    *this = tmpOpt;

	hr = S_OK;
Exit0:
	SAFE_RELEASE(pIni);
    return hr;
}