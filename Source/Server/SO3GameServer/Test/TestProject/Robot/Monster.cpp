// Monster.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include "Monster.h"
#include "KSO3World.h"
#include "KMonsterManager.h"

//#define PRINT_LOG

int main(int argc, char* argv[])
{
    int         nRetCode        = false;
    int         nLogInitFlag    = false;
    int         nMemoryInitFlag = false;
    int         nMonsterInitFlag = FALSE;
    TCHAR		szLogsPath[MAX_PATH] = { 0 };
    TCHAR		szRootPath[MAX_PATH] = { 0 };
    KGLOG_PARAM LogParam;

    g_SetRootPath();

    //setlocale(LC_ALL, "");

    memset(&LogParam, 0, sizeof(LogParam));

    ::GetCurrentDirectory(MAX_PATH, szRootPath);
    _sntprintf(LogParam.szPath, sizeof(LogParam.szPath) / sizeof(TCHAR), _T("%s\\%s"), szRootPath, _T("logs"));

    strcpy(LogParam.szIdent, "Monster");
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;
#ifdef PRINT_LOG
    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;
#endif
    //初始化内存管理模块
    nRetCode = KG_MemoryInit(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nMemoryInitFlag = true;

    nRetCode = MonsterInit();
    KGLOG_PROCESS_ERROR(nRetCode);
    nMonsterInitFlag = TRUE;

    UserControl();

Exit0:
    if (nMonsterInitFlag)
    {
        MonsterUnInit();
        nMonsterInitFlag = FALSE;
    }

    if (nMemoryInitFlag)
    {
        KG_MemoryUnInit(NULL);
        nMemoryInitFlag = false;
    }
#ifdef PRINT_LOG
    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }
#endif
	return 1;
}

int MonsterInit()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    nRetCode = g_SO3World.Init(CLIENT_ID_PREFIX);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_MonsterManager.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

int MonsterUnInit()
{
    g_MonsterManager.UnInit();
    g_SO3World.UnInit();

    return TRUE;
}

int MonsterCreate()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    nRetCode = InputRobotNumber();
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < 5000);

    nRetCode = g_MonsterManager.CreateMonster(nRetCode);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

int MonsterDestroy()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    nRetCode = InputRobotNumber();
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < 5000);

    nRetCode = g_MonsterManager.DestroyMonster(nRetCode);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

int MonsterDestroyAll()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    char szInput[32] = {0};

    printf("***WARNING*** To Destroy All the Robots?\n[yes/no]:");    
    nRetCode = Affirm();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = g_MonsterManager.DestroyAllMonster();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

int MonsterState()
{
    return g_MonsterManager.QueryAllMonsterState();
}

int MonsterStatistic()
{
    return g_MonsterManager.StateStatistic();
}

int InputRobotNumber()
{
    int nCount = 0;
    printf("Input the Number of the Robots: ");
    scanf("%d", &nCount);

    if (nCount <= 0 || nCount > 5000)
        printf("Invalid!!\n");

    return nCount;
}

int Affirm()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    char szInput[32] = {0};
    
    fflush(stdin);
    char *pszRet = fgets(szInput, sizeof(szInput), stdin);
    KG_PROCESS_ERROR(pszRet);

    szInput[sizeof(szInput) - 1] = '\0';

    nRetCode = strlen(szInput);
    KG_PROCESS_ERROR(nRetCode > 0);
    if (szInput[nRetCode - 1] == '\n')
        szInput[nRetCode - 1] = '\0';

    nRetCode = strcmp(szInput, "yes");
    KG_PROCESS_ERROR(nRetCode == 0);

    nResult = TRUE;
Exit0:
    return nResult;
}

int UserControl()
{
    int nResult  = false;
    int nRetCode = false;

    char cCmd;
    const char cszCommandTip[] = "\n\n"
        "==========================================\n"
        "       1.Create Robot                     \n"
        "       2.Destroy Robot                    \n"
        "       3.Destroy All                      \n"        
        "       4.Robots List                      \n"
        "       5.Statistic                        \n"
        "       6.Exit                             \n"
        "==========================================\n"
        "Select:";

    while (true)
    {
        printf(cszCommandTip);

        fflush(stdin);
        cCmd = getchar();

        switch(cCmd)
        {
        case '1':
            MonsterCreate(); 
            break;
        case '2':
            MonsterDestroy();
            break;
        case '3':
            MonsterDestroyAll();
            break;
        case '4':
            MonsterState();
            break;
        case '5':
            MonsterStatistic();
            break;
        case '6':
            printf("***WARNING*** To Exit Monster?\n[yes/no]:");
            nRetCode = Affirm();
            KG_PROCESS_SUCCESS(nRetCode);
            break;
        default:
            printf(cszCommandTip);
            break;
        }
        KGThread_Sleep(100);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}