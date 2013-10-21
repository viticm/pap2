///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathStressTestControl
// Creator  : ChenShiCai
// Date     : 2008-10-28
// Comment  : 
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <conio.h>
#include "KMathStressTestControl.h"

#define BUFFER_SIZE 1024
#define NET_COMMON  1024
#define TITLE       "IP\tVersion\tGetDistance3\tGetDistance2\tGetDirection\tInRange\tSin\n"

int KMathStressTestControl::Init() 
{
	int nResult = false;    
	int nRetCode = false;

    nRetCode = m_Acceptor.Init(SERVER_IP, SERVER_PORT, NET_COMMON, NET_COMMON, NET_COMMON, KSG_ENCODE_DECODE, NULL);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
	return nResult;
}

void KMathStressTestControl::UnInit()
{
    m_Acceptor.UnInit(NULL);
}

int KMathStressTestControl::Run() 
{
	int              nResult = false;    
	int              nRetCode = false;
    int              nRetEventCount = 0;
    KG_SOCKET_EVENT  SocketEventArray[NET_COMMON];
    FILE            *pFile = NULL;

    pFile = fopen("acai.tab", "w");
    KG_PROCESS_ERROR(pFile);

    fputs(TITLE, pFile);
    printf(TITLE);

    while (true)
    {
        if (kbhit())
            break;

        nRetCode = m_Acceptor.Wait(sizeof(SocketEventArray) / sizeof(SocketEventArray[0]), SocketEventArray, &nRetEventCount);
        KG_PROCESS_ERROR(nRetCode);

        for (int i = 0; i < nRetEventCount; ++i)
        {
            if (KG_SOCKET_EVENT_IN != SocketEventArray[i].uEventFlag)
                continue;

            SaveTestResult(SocketEventArray[i].piSocket, pFile);            
            KG_COM_RELEASE(SocketEventArray[i].piSocket);           
        }      
        Sleep(100);
    }

	nResult = true;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
	return nResult;
}

void KMathStressTestControl::SaveTestResult(IKG_SocketStream *piStream,  FILE *pFile)
{
	int          nResult     = false;    
	int          nRetCode    = false;
    IKG_Buffer  *piBuffer    = NULL;
    char         szBuffer[BUFFER_SIZE];


    KG_PROCESS_ERROR(piStream);
    KG_PROCESS_ERROR(pFile);
    
    while (true)
    {  
        C2S_COMMAND  *pCommand     = NULL;
        Test_Result  *pTestResult  = NULL;
        in_addr       RemoteIP;
        u_short       usRemotePort;

        nRetCode = piStream->Recv(&piBuffer);
        if (nRetCode == -2)
            break;

        KG_PROCESS_ERROR(nRetCode == 1);
        KG_PROCESS_ERROR(piBuffer->GetSize() == (sizeof(C2S_COMMAND) + sizeof(Test_Result)));

        pCommand = (C2S_COMMAND *)piBuffer->GetData();
        pTestResult = (Test_Result *)pCommand->chData;

        nRetCode = piStream->GetRemoteAddress(&RemoteIP, &usRemotePort);
        KG_PROCESS_ERROR(nRetCode);

        snprintf(szBuffer,sizeof(szBuffer), "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", RemoteIP.s_addr, pTestResult->nVersion,
            pTestResult->uGetDistance3Time, pTestResult->uGetDistance2Time, pTestResult->uGetDirectionTime,
            pTestResult->uInRangeTime, pTestResult->uSinTime);

        printf("%s", szBuffer);

        fputs(szBuffer, pFile);
        KG_COM_RELEASE(piBuffer);
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KG_COM_RELEASE(piBuffer);
        KG_COM_RELEASE(piStream);
    }
	return;
}

