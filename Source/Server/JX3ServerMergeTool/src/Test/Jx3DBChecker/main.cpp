//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   main.cpp
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-12-8
//  Comment     :   ³ÌÐòÈë¿Ú
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <locale.h>
#include <stdio.h>
#include "KG_Jx3DBChecker.h"
#include "Engine/KGLog.h"
#include "Engine/File.h"

#ifdef WIN32

void SetConsoleInfo()
{
    HANDLE      hConsole    = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD       BufferSize  = {108, 320};
    SMALL_RECT  WinRect     = {0, 0, BufferSize.X - 1, 27};

    SetConsoleScreenBufferSize(hConsole, BufferSize);
    SetConsoleWindowInfo(hConsole, true, &WinRect);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

}

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC

#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)

inline void EnableMemLeakCheck()
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

#endif
#endif
static int _SetRuntimeEnvironment(const char cszAppFullPathName[MAX_PATH])
{
    int nResult  = false;
    int nRetCode = false;
    char szProgramPath[MAX_PATH];

    ASSERT(cszAppFullPathName);

    // Set the locale to the default, which is the user-default ANSI code page 
    // obtained from the operating system.
    setlocale(LC_ALL, "");

    // Set working path to application path.
    g_ExtractFilePath(szProgramPath, const_cast<char *>(cszAppFullPathName));
    szProgramPath[sizeof(szProgramPath) - 1] = '\0';

    if (szProgramPath[0])
    {
        nRetCode = chdir(szProgramPath);
        KG_PROCESS_ERROR(nRetCode == 0);

        // Set root path for config file.
        g_SetRootPath(szProgramPath); 
    }
    else
    {
        g_SetRootPath(NULL);
    }

    // Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int main(int argc, char *argv[])
{
    int nResult  = false;
    int nRetCode = false;
    KGLOG_PARAM LogParam  = {"Logs", "Jx3DBChecker", (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE), 100000};
    int nKGLogInitFlag    = false;
    KG_Jx3DBChecker Jx3DBChecker;
    int nJx3DBCheckerFlag = false;
    time_t lStartTime = 0;
    time_t lStopTime  = 0;
    struct tm *ptmTime = NULL;
    unsigned uRunningSeconds = 0;


#ifdef WIN32
    SetConsoleInfo();
#ifdef _DEBUG
    EnableMemLeakCheck();
#endif
#endif

    lStartTime = time(NULL);
    ptmTime = localtime(&lStartTime);
    KG_PROCESS_ERROR(ptmTime);
    printf(
        "Start Time: %d-%d-%d %d:%d:%d\n", 
        ptmTime->tm_year + 1900, 
        ptmTime->tm_mon + 1, 
        ptmTime->tm_mday,
        ptmTime->tm_hour, 
        ptmTime->tm_min, 
        ptmTime->tm_sec
    );

    puts("[Jx3DBChecker] Initializing ...");

    nRetCode = _SetRuntimeEnvironment(argv[0]);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = Jx3DBChecker.Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    nJx3DBCheckerFlag = true;

    nRetCode = Jx3DBChecker.Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    lStopTime = time(NULL);
    ptmTime = localtime(&lStopTime);
    KGLOG_PROCESS_ERROR(ptmTime);
    printf(
        "Stop Time: %d-%d-%d %d:%d:%d\n", 
        ptmTime->tm_year + 1900, 
        ptmTime->tm_mon + 1, 
        ptmTime->tm_mday,
        ptmTime->tm_hour, 
        ptmTime->tm_min, 
        ptmTime->tm_sec
    );

    uRunningSeconds = (unsigned)(lStopTime - lStartTime);
    printf(
        "Program Running Time: %d days ,%d hours ,%d minutes, %d seconds.\n", 
        (uRunningSeconds / (60 * 60 * 24)),
        ((uRunningSeconds % (60 * 60 * 24)) / (60 * 60)),
        ((uRunningSeconds % (60 * 60)) / 60),
        (uRunningSeconds % 60)
    );

    nResult = true;
Exit0:
    if (nJx3DBCheckerFlag)
    {
        Jx3DBChecker.UnInit();
        nJx3DBCheckerFlag = false;
    }
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    return nResult ? 0 : 1;
}
