/************************************************************************/
/*	剑网三总控服务器主程序												*/
/*	Copyright : Kingsoft 2004											*/
/*	Author    :	Zhu Jianqiu												*/
/*	History	  :															*/
/*		2004.11.19	Create												*/
/************************************************************************/

#include "stdafx.h"
#include <signal.h>
#include "KSO3GameCenter.h"

KSO3GameCenter* g_pSO3GameCenter = NULL;

#ifdef WIN32

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    if (g_pSO3GameCenter)
    {
        KGLogPrintf(KGLOG_INFO, "Receive quit signal from console !\n");
        g_pSO3GameCenter->Quit();
    }
    return true;
}

void SetConsoleInfo()
{
    HANDLE      hConsole    = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD       BufferSize  = {108, 320};
    SMALL_RECT  WinRect     = {0, 0, BufferSize.X - 1, 27};

    SetConsoleScreenBufferSize(hConsole, BufferSize);
    SetConsoleWindowInfo(hConsole, true, &WinRect);

#ifdef _DEBUG
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
#else
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif

    SetConsoleCtrlHandler(&ConsoleHandlerRoutine, true);
}

#else

void OnQuitSignal(int nSignal)
{
    if (g_pSO3GameCenter)
    {
        KGLogPrintf(KGLOG_INFO, "Receive quit signal from console !\n");
        g_pSO3GameCenter->Quit();
    }
}

BOOL SetDaemon()
{
    BOOL    bResult   = false;
    int     nRetCode  = 0;
    int     nNullFile = -1;
    rlimit  ResLimit;

    ResLimit.rlim_cur = SHRT_MAX;
    ResLimit.rlim_max = SHRT_MAX;

    nRetCode = setrlimit(RLIMIT_NOFILE, &ResLimit);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = getrlimit(RLIMIT_CORE, &ResLimit);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    ResLimit.rlim_cur = ResLimit.rlim_max;

    nRetCode = setrlimit(RLIMIT_CORE, &ResLimit);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    signal(SIGINT, OnQuitSignal);
    signal(SIGQUIT, OnQuitSignal);
    signal(SIGTERM, OnQuitSignal); 

    nNullFile = open("/dev/null", O_WRONLY);
    KGLOG_PROCESS_ERROR(nNullFile != -1);

    dup2(nNullFile, STDIN_FILENO);
    dup2(nNullFile, STDOUT_FILENO);
    dup2(nNullFile, STDERR_FILENO);

    bResult = true;
Exit0:
    if (nNullFile != -1)
    {
        close(nNullFile);
        nNullFile = -1;
    }
    return bResult;
}

#endif

int main(int argc, char* argv[])
{
    int         nResult         = false;
    int         nRetCode        = false;
    int         nLogInitFlag    = false;
    int         nMemoryInitFlag = false;
    int         nGCInitFlag     = false;
    char*       pszRetCWD       = NULL;
    BOOL        bMemoryPoolInit = false;
    char        szCWD[MAX_PATH];
    KGLOG_PARAM LogParam;

#ifdef WIN32    
    SetConsoleInfo();
#endif

#ifdef __GNUC__
    nRetCode = fork();
    if (nRetCode != 0)
    {
        // if fork error(-1), exit !
        // if parent process(!=0), exit !
        goto Exit0;
    }

    setsid();
    //umask(0);
#endif    

    tzset(); // initialize time conversion information (timezone)

    pszRetCWD = getcwd(szCWD, sizeof(szCWD));    
    KG_PROCESS_ERROR(pszRetCWD);

    nRetCode = KMemory::Initialize("center_server.memory");
    KG_PROCESS_ERROR(nRetCode);
    bMemoryPoolInit = true;

    g_pSO3GameCenter = KMemory::New<KSO3GameCenter>();
    KG_PROCESS_ERROR(g_pSO3GameCenter);

    memset(&LogParam, 0, sizeof(LogParam));
    snprintf(LogParam.szPath, sizeof(LogParam.szPath), "%s/logs", szCWD);
    strcpy(LogParam.szIdent, "SO3GameCenter");
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

    nRetCode = KG_MemoryInit(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nMemoryInitFlag = true;

    KGLogPrintf(KGLOG_INFO, "%s\n", "Center server for JXOnline III");
    KGLogPrintf(KGLOG_INFO, "%s\n", "Build at " __TIME__ " " __DATE__);

    nRetCode = g_pSO3GameCenter->Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    nGCInitFlag = true;

    KGLogPrintf(KGLOG_INFO, "Center server startup ... ... [OK]\n");

#ifdef WIN32
    KGLogPrintf(KGLOG_INFO, "Press [Ctrl + C] to exit !\n");
#endif

#ifdef __GNUC__
    nRetCode = SetDaemon();
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

    g_pSO3GameCenter->Run();

    nResult = true;
Exit0:

    if (nGCInitFlag)
    {
        g_pSO3GameCenter->UnInit();
        nGCInitFlag = false;
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

    if (g_pSO3GameCenter)
    {
        KMemory::Delete(g_pSO3GameCenter);
        g_pSO3GameCenter = NULL;
    }

    if (bMemoryPoolInit)
    {
        KMemory::Finalize();
        bMemoryPoolInit = false;
    }

    KGLogPrintf(KGLOG_INFO, "Game center exit !\n");

#ifdef WIN32
    puts("press any key to exit ...");
    getch();
#endif

    return 0;
}


