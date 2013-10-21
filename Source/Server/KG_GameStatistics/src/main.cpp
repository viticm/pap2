////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : main.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  13:19:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_GameStatistics.h"

int _SetRuntimeEnvironment(const char cszAppFullPathName[MAX_PATH])
{
    int nResult  = false;
    int nRetCode = false;
    char szProgramPath[MAX_PATH];

    KGLOG_PROCESS_ERROR(cszAppFullPathName);

    // Set the locale to the default, which is the user-default ANSI code page obtained from 
    // the operating system.
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

    nResult = true;
Exit0:
    return nResult;
}

int main(int argc, char *argv[])
{
	int nResult  = false;
	int nRetCode = false;
    int nKGLogInitFlag = false;
    int nKGMemoryInitFlag = false;
    int nGameStatisticsInitFlag = false;
    KGLOG_PARAM LogParam  = {"Logs", "KG_GameStatistics", KGLOG_OPTION_FILE, 1000000};
    KG_GameStatistics GameStatistics;

    puts("[KG_GameStatistics] Initializing ...");

    nRetCode = _SetRuntimeEnvironment(argv[0]);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = KG_MemoryInit(NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_MemoryInit()");
    nKGMemoryInitFlag = true;

    nRetCode = GameStatistics.Init(NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "GameStatistics.Init()");
    nGameStatisticsInitFlag = true;

    nRetCode = GameStatistics.Run();
    KGLOG_PROCESS_ERROR(nRetCode && "GameStatistics.Run()");    

	nResult = true;
Exit0:    
    if (nResult)
    {
        puts("Operation had been done successfully.");
        KGLogPrintf(KGLOG_INFO, "Operation had been done successfully.");
    }
    else
    {
        puts("Operation failed!");
        KGLogPrintf(KGLOG_INFO, "Operation failed!");
    }
    if (nGameStatisticsInitFlag)
    {
        GameStatistics.UnInit(NULL);
        nGameStatisticsInitFlag = false;
    }
    if (nKGMemoryInitFlag)
    {
        KG_MemoryUnInit(NULL);
        nKGMemoryInitFlag = false;
    }
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    return nResult ? 0 : 1;
}
