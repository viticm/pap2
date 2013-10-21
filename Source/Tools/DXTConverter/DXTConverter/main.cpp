#include "stdafx.h"
#include "DXTConverter.h"
#include "Tools.h"

int AnalyseCommand(int nArgc, char *pszArgv[]);

const char HELP[] = {
    "USAGE: DXTConverter.exe [[options]...]\n"
    "--- options list ---\n"
    "   -p: the path of DXTConvertor.ini, if you donot give the path it will use current path\n"
    "example:\n"
    "   DXTConverter.exe -p d:\\kingsoft\\DXTConverter\\\n"
};

char g_szIniPath[MAX_PATH] = {0};

int main(int nArgc, char *pszArgv[])
{
    int nResult = false;
    int nRetCode = false;
    int nLogInitFlag = false;
    int nCommandFlag = false;
    KGDXTConverterManager DXTConverterManager;
    KGLOG_PARAM LogParam  = {"logs", "DXTConverter", KGLOG_OPTION_FILE, 65536 * 2};

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;
    
    g_SetRootPath(NULL);

    if (nArgc == 3)
    {
        nRetCode = AnalyseCommand(nArgc, pszArgv);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = Tools::IsPathExist(g_szIniPath, sizeof(g_szIniPath));
        KGLOG_PROCESS_ERROR(nRetCode && "IniPath is not exist!");

        nCommandFlag = true;
    }
    else
    {   
        printf("%s", HELP);
    }

    //only tga dds files
    nRetCode = DXTConverterManager.Init(g_szIniPath, nCommandFlag);
    KGLOG_PROCESS_ERROR(nRetCode && "KGDXTConverterManager::Init() failed!");

    nRetCode = DXTConverterManager.Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    DXTConverterManager.UnInit();
    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }
    return nResult ? 0 : 1;
}

int AnalyseCommand(int nArgc, char *pszArgv[])
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(nArgc > 2);
    KGLOG_PROCESS_ERROR(pszArgv);

    for (int i = 1; i < nArgc - 1; i++)
    {
        if (
            !(_stricmp(pszArgv[i], "-p")) &&
            (pszArgv[i+1][0] != '-')
            )
        {
            strncpy(g_szIniPath, pszArgv[i+1], sizeof(g_szIniPath) - 1);
            g_szIniPath[sizeof(g_szIniPath) - 1] = '\0';
            i++;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}