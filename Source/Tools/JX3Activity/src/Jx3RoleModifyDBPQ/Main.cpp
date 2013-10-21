///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   Main.cpp
// Creator  :   liuzhibiao
// Date     :   2009-10-14
// Comment  :   v0.5
//              2009-10-14 V0.5 Create   Jx3RoleModifyDBPQTools³ÌÐòÈë¿Ú
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <locale.h>
#include "KJx3RoleModifyDBPQTools.h"

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

int main(int argc, char *argv[]) 
{
    int nResult = false;    
    int nRetCode = false;

    KGLOG_PARAM                 LogParam;
    int                         nLogInitFlag                    = false;
//    int                         nMemInitFlag            = false;
    KJx3RoleModifyDBPQTools     jx3RoleModifyDBPQTools;
    int                         nJx3RoleModifyDBPQToolInitFlag  = false; 
    int                         nDBHadError                     = false;

    time_t     lStartTime = 0;
    time_t     lStopTime  = 0;
    struct tm* ptmTime = NULL;
    unsigned   uRunningSeconds = 0;

#ifdef WIN32
    SetConsoleInfo();
#ifdef _DEBUG
    EnableMemLeakCheck();
#endif
#endif

    strcpy(LogParam.szPath,  "Logs");
    strcpy(LogParam.szIdent, "Jx3RoleModifyDBPQ");
    LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);
    LogParam.nMaxLineEachFile = 160000;    

    nRetCode = KGLogInit(LogParam, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;  

    setlocale(LC_ALL, "");
    g_SetRootPath(NULL);

    ASSERT(argv);

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
    puts("[Jx3RoleModifyDBPQ] Build at "__DATE__" "__TIME__"\n");
    puts("[Jx3RoleModifyDBPQ] Initializing ...");

    nRetCode = jx3RoleModifyDBPQTools.Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    nJx3RoleModifyDBPQToolInitFlag = true;

    nRetCode = jx3RoleModifyDBPQTools.Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nDBHadError = jx3RoleModifyDBPQTools.IsDBHadError();

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
    if (!nResult)
    {
        puts("[Jx3RoleModifyDBPQ] Has Error, check the tab file and Log file...");
    }
    else
    {
        if (nDBHadError)
        {
            puts("[Jx3RoleModifyDBPQ] Connect Database Has Error, check the tab file...");
        }
        else
        {
            puts("[Jx3RoleModifyDBPQ] Had Exit normal ...");
        }
    }

    if (nJx3RoleModifyDBPQToolInitFlag)
    {
        jx3RoleModifyDBPQTools.UnInit();
        nJx3RoleModifyDBPQToolInitFlag = false;
    }

    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }

    return nResult ? 0 : 1;
}
