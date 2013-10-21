// SO3Client.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "resource.h"
#include "ClientString.h"
#include "KSO3Client.h"

#define LOG_FILE "3DTest\\SFX"

KSO3Client g_SO3Client;

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

#ifdef _DEBUG
    hDumper = LoadLibraryA("DumperD.dll");
#else
    hDumper = LoadLibraryA("Dumper.dll");
#endif

    setlocale(LC_ALL, "Chinese (Simplified)_People's Republic of China.936");

    memset(&LogParam, 0, sizeof(LogParam));

    GetCurrentDirectoryA(MAX_PATH, szRootPath);
    _snprintf(LogParam.szPath, _countof(LogParam.szPath), "%s\\%s", szRootPath, "logs");

    strncpy(LogParam.szIdent, LOG_FILE, _countof(LogParam.szIdent));
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

    nRetCode = KG_MemoryInit(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nMemoryInitFlag = true;

    //ASSERT(FALSE);
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
