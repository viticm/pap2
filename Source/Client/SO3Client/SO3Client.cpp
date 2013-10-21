// SO3Client.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
//#include "vld.h"
#include "resource.h"
#include "ClientString.h"
#include "SO3Client.h"

#pragma data_seg(".jx3ips")
LONG volatile g_lProcessCount = 0;
#pragma data_seg()
#pragma comment(linker, "/section:.jx3ips,rws")

KSO3Client g_SO3Client;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int         nRetCode        = false;
    int         nLogInitFlag    = false;
    int         nMemoryInitFlag = false;
    char		szLogsPath[MAX_PATH] = { 0 };
    char		szRootPath[MAX_PATH] = { 0 };
    KGLOG_PARAM LogParam;
    HMODULE		hDumper = NULL;
    BOOL        bMemoryPoolInitFlag = false;
    LONG        lCount = 0;

#ifdef KG_PUBLISH
	if (!lpCmdLine || strcmp(lpCmdLine, "DOTNOTSTARTGAMEBYJX3CLIENT.EXE"))
	{
		char szGameUpdater[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szRootPath);
		_snprintf(szGameUpdater, _countof(szGameUpdater), "%s\\%s", szRootPath, "gameupdater.exe");
		WinExec(szGameUpdater, SW_NORMAL);
		goto Exit0;
	}
#endif

    lCount = InterlockedIncrement(&g_lProcessCount);
    KG_PROCESS_ERROR(lCount < JX3_MAX_USER);

#ifdef _DEBUG
	hDumper = LoadLibraryA("DumperD.dll");
#else
    hDumper = LoadLibraryA("Dumper.dll");
#endif

    setlocale(LC_ALL, "Chinese (Simplified)_People's Republic of China.936");

    memset(&LogParam, 0, sizeof(LogParam));

    GetCurrentDirectoryA(MAX_PATH, szRootPath);
    _snprintf(LogParam.szPath, _countof(LogParam.szPath), "%s\\%s", szRootPath, "logs");

    strncpy(LogParam.szIdent, "JX3Client", _countof(LogParam.szIdent));
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

    nRetCode = KG_MemoryInit(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nMemoryInitFlag = true;

    nRetCode = KMemory::Initialize("JX3Client.memory");
    KGLOG_PROCESS_ERROR(nRetCode);
    bMemoryPoolInitFlag = true;

    if (g_SO3Client.Init(hInstance, GAME_TITLE_NAME_A, IDI_ICON_MAIN))
        g_SO3Client.Run();

    KGLogPrintf(KGLOG_INFO, "JX3Client exit.\n");

Exit0:
    if (bMemoryPoolInitFlag)
    {
        KMemory::Finalize();
        bMemoryPoolInitFlag = false;
    }

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

    InterlockedDecrement(&g_lProcessCount);
    return 0;
}
