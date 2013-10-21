#include "stdafx.h"
#include <signal.h>
#include "KSO3Gateway.h"

static KSO3Gateway* g_pGateway = NULL;

#ifdef WIN32
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    if (g_pGateway)
    {
        KGLogPrintf(KGLOG_INFO, "Receive quit signal !\n");
        g_pGateway->Quit();
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
    if (g_pGateway)
    {
        KGLogPrintf(KGLOG_INFO, "Receive quit signal !\n");
        g_pGateway->Quit();
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
    int             nRetCode            = false;
    BOOL            bLogInitFlag        = false;
    BOOL            bMemInitFlag        = false;
    BOOL            bGatewayInitFlag    = false;
    char*           pszRetCWD           = NULL;
    KGLOG_PARAM     LogParam;
    char            szCWD[MAX_PATH];

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
#endif

    tzset(); // initialize time conversion information (timezone)
    setlocale(LC_ALL, "");

    pszRetCWD = getcwd(szCWD, sizeof(szCWD));    
    KG_PROCESS_ERROR(pszRetCWD);

    memset(&LogParam, 0, sizeof(LogParam));

    nRetCode = snprintf(LogParam.szPath, sizeof(LogParam.szPath), "%s/logs", szCWD);
    KG_PROCESS_ERROR(nRetCode >= 0);
    KG_PROCESS_ERROR(nRetCode < sizeof(LogParam.szPath));

    strncpy(LogParam.szIdent, "Gateway", sizeof(LogParam.szIdent));
    LogParam.szIdent[sizeof(LogParam.szIdent) - 1] = '\0';

    LogParam.nMaxLineEachFile   = 65536;
    LogParam.Options            = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    bLogInitFlag = true;

    nRetCode = KG_MemoryInit(NULL);
    KG_PROCESS_ERROR(nRetCode);
    bMemInitFlag = true;

    g_RandomSeed((unsigned int)time(NULL));

    KGLogPrintf(KGLOG_INFO, "%s\n", "Gateway server for JXOnline III");
    KGLogPrintf(KGLOG_INFO, "%s\n", "Build at " __TIME__ " " __DATE__);

    g_pGateway = new KSO3Gateway;
    KG_PROCESS_ERROR(g_pGateway);

    nRetCode = g_pGateway->Init();
    KG_PROCESS_ERROR(nRetCode);
    bGatewayInitFlag = true;

    KGLogPrintf(KGLOG_INFO, "Gateway server startup ... ... [OK]\n");

#ifdef WIN32
    KGLogPrintf(KGLOG_INFO, "Press [Ctrl + C] to exit !\n");
#endif

#ifdef __GNUC__
    nRetCode = SetDaemon();
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

    g_pGateway->Run();

Exit0:
    if (bMemInitFlag)
    {
        KG_MemoryUnInit(NULL);
        bMemInitFlag = false;
    }

    if (bLogInitFlag)
    {
        KGLogUnInit(NULL);
        bLogInitFlag = false;
    }

    if (bGatewayInitFlag)
    {
        g_pGateway->UnInit();
        bGatewayInitFlag = false;
    }

    KG_DELETE(g_pGateway);

#ifdef WIN32
    puts("Press any key to continue !");
    _getch();
#endif
	return 0;
}
