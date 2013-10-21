// MonsterController.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include "KMonsterController.h"

int InputRobotNumber();
int InputConnectID();
int Run();

int main()
{
	int nRetCode = FALSE;

	nRetCode = g_CtrlMgr.Init();
	if (!nRetCode)
	{
		printf("Startup Failed!\n");
		goto Exit0;
	}

	Run();

Exit0:
	g_CtrlMgr.UnInit();
	return TRUE;
}

int InputRobotNumber()
{
	int nCount = 0;
	printf("Input The Number of the Robots: ");
	scanf("%d", &nCount);

	if (nCount <= 0 || nCount > 5000)
	{
		printf("Invalid!!\n");
		nCount=0;
	}

	return nCount;
}

int InputConnectID()
{
	int nID = 0;
	printf("Input Connect ID: ");
	scanf("%d", &nID);

	if (nID < -1 || nID > 5000)
	{
		printf("Invalid!!\n");
		nID=0;
	}

	return nID;
}

int InputCpuLimit()
{
	int nCpuLimit = 0;
	printf("Input Cpu Limit: ");
	scanf("%d", &nCpuLimit);

	if (nCpuLimit < 0 || nCpuLimit > 100)
	{
		printf("Invalid!!\n");
		nCpuLimit=0;
	}

	return nCpuLimit;
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

	nRetCode = (int)strlen(szInput);
	KG_PROCESS_ERROR(nRetCode > 0);
	if (szInput[nRetCode - 1] == '\n')
		szInput[nRetCode - 1] = '\0';

	nRetCode = strcmp(szInput, "yes");
	KG_PROCESS_ERROR(nRetCode == 0);

	nResult = TRUE;
Exit0:
	return nResult;
}

int Run()
{
	int nResult  = false;
	int nRetCode = false;

	char cCmd;
	const char cszCommandTip[] = "\n\n"
		"==========================================\n"
		"       1.Smart Create Robot               \n"
		"       2.Create Robot                     \n"
		"       3.Destroy Robot                    \n"
		"       4.Destroy All                      \n"        
		"       5.Connect List                     \n"
		"       6.Statistic                        \n"
		"       7.ReInit Monstre                   \n"
		"       8.Exit                             \n"
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
			g_CtrlMgr.CreatMonsterSmart(InputRobotNumber(),InputCpuLimit());
			break;
		case '2':
			g_CtrlMgr.CreatMonster(InputConnectID(), InputRobotNumber());
			break;
		case '3':
			g_CtrlMgr.DestroyMonster(InputConnectID(), InputRobotNumber());
			break;
		case '4':            
			printf("***WARNING*** To Exit Monster?\n[yes/no]:");
			if (Affirm())
				g_CtrlMgr.DestroyAllMonster(InputConnectID());
			break;
		case '5':
			g_CtrlMgr.QueryMonsterList();
			break;
		case '6':
			g_CtrlMgr.Statistic();
			break;
		case '7':
			g_CtrlMgr.ReInitMonster();
			printf("reinit monster success\n");
			break;
		case '8':
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
	return nResult;
}
