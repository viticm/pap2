///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : Main.cpp
// Creator  : liuzhibiao
// Date     : 2009-5-25
// Comment  : v0.1
//            v0.1  2009-5-25   Create  	
///////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "KSO3RoleDBUpdater.h"

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
#endif

int main(int argc, char* argv[])
{
    int         nResult      = false;
    int         nRetCode     = false;  
    int         nLogInitFlag = false;
    int         nRdbFlag     = false;
    KGLOG_PARAM LogParam;  

    KSO3RoleDBUpdater* pKSO3RoleDBUpdater = NULL;

#ifdef WIN32
    SetConsoleInfo();
#endif
    setlocale(LC_ALL, "");
    g_SetRootPath(NULL);

    strncpy(LogParam.szPath, "logs", sizeof(LogParam.szPath));
    LogParam.szPath[sizeof(LogParam.szPath) - 1] = '\0';
    strncpy(LogParam.szIdent, "SO3RoleDBUpdaterTest", sizeof(LogParam.szIdent));
    LogParam.szIdent[sizeof(LogParam.szIdent) - 1] = '\0';
    LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);
    LogParam.nMaxLineEachFile = 160000;

    nRetCode = KGLogInit(LogParam, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;  

    pKSO3RoleDBUpdater = new KSO3RoleDBUpdater();
    KGLOG_PROCESS_ERROR(pKSO3RoleDBUpdater);

    nRetCode = pKSO3RoleDBUpdater->Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    nRdbFlag = true;
    KGLogPrintf(KGLOG_INFO, "SO3RoleDBUpdaterTest had started up ... ...");

    nRetCode = pKSO3RoleDBUpdater->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLogPrintf(KGLOG_INFO, "SO3RoleDB UpdateTest Complete...");

    nResult = true;
Exit0:
    if (nRdbFlag)
    {
        pKSO3RoleDBUpdater->UnInit();
        nRdbFlag = false;
    }

    KG_DELETE(pKSO3RoleDBUpdater);

    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }
    
    return nResult ? 0 : 1;
}
