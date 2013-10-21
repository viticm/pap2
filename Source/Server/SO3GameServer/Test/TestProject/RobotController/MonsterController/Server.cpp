#include <stdafx.h>
#include <stdio.h>
#include "KMonsterController.h"

int InputRobotNumber();
int InputConnectID();
int Run();

int main()
{
    int nRetCode = FALSE;
    KCtrllerMgrParam Param;

    printf("Input Listen IP and Port: ");
    scanf("%s %d", &Param.szListenIP, &Param.nListenPort);
    nRetCode = g_CtrlMgr.Init(Param);
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
        printf("Invalid!!\n");

    return nCount;
}

int InputConnectID()
{
    int nID = 0;
    printf("Input Connect ID: ");
    scanf("%d", &nID);

    if (nID < -1 || nID > 5000)
        printf("Invalid!!\n");

    return nID;
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
        "       7.Exit                             \n"
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
            g_CtrlMgr.CreatMonsterSmart(InputRobotNumber(),InputConnectID());
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


//#include <stdio.h>
//#include <assert.h>
//#include "KG_Socket.h"
//
//#define PORT_DEFAULT					12345
//#define MAX_EVENT_ARRAY_SIZE			4096
//
//#define MAX_INTERNAL_RECV_BUFFER_SIZE   8192 * 8
//#define MAX_INTERNAL_SEND_BUFFER_SIZE   8192 * 8
//
//#define MODE_ENCODE_DECODE				KSG_ENCODE_DECODE_DYNAMIC
//
//using namespace std;
//
//int main()
//{
//	int                     nResult		   = false;
//	int                     nRetCode       = false;	
//	int                     nRetEventCount = 0;	
//	int                     nRecvCnt       = 0;
//    int                     nRecvErr       = 0;
//	int                     nSendErr       = 0;
//	int						nSendCnt	   = 0;
//	IKG_Buffer              *piBuf         = NULL;
//	KG_SOCKET_EVENT         Event[MAX_EVENT_ARRAY_SIZE];
//	KG_SocketServerAcceptor TestServer;
//    int nSendCount[MAX_EVENT_ARRAY_SIZE] = {0};
//    memset(&nSendCount, 0, sizeof(nSendCount));
//
//    int nStart = GetTickCount();
//    int nInterval = 0;
//    int nSentCount = 0;
//
//
//	nRetCode = TestServer.Init(NULL, PORT_DEFAULT, 512, MAX_INTERNAL_RECV_BUFFER_SIZE, 
//					MAX_INTERNAL_SEND_BUFFER_SIZE, MODE_ENCODE_DECODE, NULL
//				);
//	KG_PROCESS_ERROR(nRetCode);
//
//	printf("Server is start...\n");
//
//	while (true)
//	{
//		nRetCode = TestServer.Wait(MAX_EVENT_ARRAY_SIZE, Event, &nRetEventCount);
//		KG_PROCESS_ERROR(nRetCode);
//
//		for (int i = 0; i < nRetEventCount; ++i)
//		{
//			if (KG_SOCKET_EVENT_ACCEPT & Event[i].uEventFlag)
//				NULL;
//			if (KG_SOCKET_EVENT_IN & Event[i].uEventFlag)
//			{		
//				while(true)
//				{
//					nRetCode = Event[i].piSocket->Recv(&piBuf);
//					if (-2 == nRetCode)
//						break;
//					if (-1 == nRetCode)
//					{
//						KG_COM_RELEASE(Event[i].piSocket);
//                        ++nRecvErr;
//						break;
//					}
//					++nRecvCnt;
//
//                    if (nInterval * 16 > nSendCount[i] * 1000)
//                    {
//					    nRetCode = Event[i].piSocket->Send(piBuf);
//					    if (-1 == nRetCode && 0 == nRetCode)
//						    ++nSendErr;
//					    ++nSendCnt;
//                        ++nSendCount[i];
//                    }
//
//					KG_COM_RELEASE(piBuf);
//					printf("\rRecv total: %d   Send total: %d   Send error: %d", 
//							nRecvCnt, nSendCnt, nSendErr
//						);
//				}
//			}
//		}
//        Sleep(1);
//	}
//
//	nResult = true;
//
//Exit0:
//	KG_COM_RELEASE(piBuf);
//	TestServer.UnInit(NULL);
//	return nResult;
//}