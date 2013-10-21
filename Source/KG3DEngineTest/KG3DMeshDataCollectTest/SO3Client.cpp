// SO3Client.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "resource.h"
#include "ClientString.h"
#include "KSO3Client.h"
#include "getopt.h"

KSO3Client g_SO3Client;
char g_szMessDataTab[MAX_PATH] = "MeshData.tab";

int ProcessArgs()
{
	int nResult = false;
	int nRetCode = false;
	wchar_t *pwszCommandLine = NULL;
	wchar_t **ppwszArgv = NULL;
	int nArgsCount = 0;
	char **ppszArgv = NULL;
	char szBuffer[MAX_PATH] = {0};
	int nLength = 0;
	int nCh = 0;

	pwszCommandLine = GetCommandLineW();
	KGLOG_PROCESS_ERROR(pwszCommandLine);

	ppwszArgv = CommandLineToArgvW(pwszCommandLine, &nArgsCount);
	KGLOG_PROCESS_ERROR(pwszCommandLine);

	ppszArgv = new char *[nArgsCount];
	KGLOG_PROCESS_ERROR(ppszArgv);
	for (int n = 0; n < nArgsCount; ++n)
	{
		ppszArgv[n] = NULL;
	}

	for (int n = 0; n < nArgsCount; ++n)
	{
		nRetCode = wcstombs(szBuffer, ppwszArgv[n], MAX_PATH);
		KGLOG_PROCESS_ERROR(nRetCode);
		szBuffer[MAX_PATH - 1] = '\0';
		nLength = (int)strlen(szBuffer);
		ppszArgv[n] = new char[nLength + 1];
		KGLOG_PROCESS_ERROR(ppszArgv[n]);
		strcpy(ppszArgv[n], szBuffer);
	}

	opterr = 0;
	while((nCh = getopt(nArgsCount, ppszArgv, "I:i:T:t:")) != -1)
	{
		switch(nCh)
		{
		case 'I':
		case 'i':
			//strcpy(g_szMessDataIni, optarg);
			break;
		case 'T':
		case 't':
			strcpy(g_szMessDataTab, optarg);
			break;
		case '?':
			printf("Option(-%c) error! Unused option or can't find the arg for this option\n\n", optopt);
			return 0;
		default:
			assert(false && "nCh = getopt(...) is wrong!");
			break;
		}
	}

	nResult = true;
Exit0:
	if (ppwszArgv)
	{
		LocalFree(ppwszArgv);
		ppwszArgv = NULL;
	}
	for (int n = 0; n < nArgsCount; ++n)
	{
		if (ppszArgv[n])
		{
			delete[](ppszArgv[n]);
			ppszArgv[n] = NULL;
		}
	}
	if (ppszArgv)
	{
		delete[](ppszArgv);
		ppszArgv = NULL;
	}
	return nResult;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int         nRetCode                = false;
    int         nLogInitFlag            = false;
    int         nMemoryInitFlag         = false;
    char		szLogsPath[MAX_PATH]    = { 0 };
    char		szRootPath[MAX_PATH]    = { 0 };
    HMODULE		hDumper                 = NULL;
    BOOL        bMemoryPoolInitFlag     = false;
    KGLOG_PARAM LogParam;

    hDumper = LoadLibraryA("dumper.dll");

    setlocale(LC_ALL, "Chinese (Simplified)_People's Republic of China.936");

    memset(&LogParam, 0, sizeof(LogParam));

    GetCurrentDirectoryA(MAX_PATH, szRootPath);
    _snprintf(LogParam.szPath, _countof(LogParam.szPath), "%s\\%s", szRootPath, "logs");

    strncpy(LogParam.szIdent, "KG3DEngineTest", _countof(LogParam.szIdent));
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

	nRetCode = ProcessArgs();
	KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = KG_MemoryInit(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nMemoryInitFlag = true;

    if (g_SO3Client.Init(hInstance, GAME_TITLE_NAME, IDI_ICON_MAIN))
        g_SO3Client.Run();

Exit0:
    if (nMemoryInitFlag)
    {
        KG_MemoryUnInit(NULL);
        nMemoryInitFlag = false;
    }

    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }

    if (hDumper)
    {
        FreeLibrary(hDumper);
        hDumper = NULL;
    }
    return 0;
}
