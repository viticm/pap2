///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KClientController.cpp
// Creator  :   liuzhibiao
// Date     :   2008-11-24
// Comment  :   v1.0
//              v1.0    Create  
//              v1.1    2009-2-13 ÐÞ¸Älinux¼æÈÝ
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KClientController.h"

int KClientController::Init(const KG_SIMULATE_CLIENT_CONFIG& crSimulateClientConfig, int nTestPoint, int nClientCount)
{
    int              nResult    = false;    
//    int              nRetCode   = false; 
    KSimulateClient* pClient    = NULL;
    char             szNum[_NAME_LEN];
    KG_SIMULATE_CLIENT_CONFIG   ClientConfig;

    memcpy(&ClientConfig, &crSimulateClientConfig, sizeof(ClientConfig));

    m_nClientCount = 0;

    for (int i = 0; i < nClientCount; i++)
    {
        pClient = new KSimulateClient;
        KGLOG_PROCESS_ERROR(pClient);

        m_ClientsList.push_back(pClient);
        m_nClientCount++;

        //itoa(i, szNum, 10);
        sprintf(szNum, "%d", i);
        strncpy(ClientConfig.szClientAccountName, szNum, sizeof(ClientConfig.szClientAccountName));
        ClientConfig.szClientAccountName[sizeof(ClientConfig.szClientAccountName) - 1] = '\0';

        pClient->Init(ClientConfig, nTestPoint);
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        for (int i = 0; i < m_nClientCount; i++)
        {
            pClient = m_ClientsList[i];
            pClient->UnInit();
            KG_DELETE(pClient);
        }
        m_ClientsList.clear();
    }
    return nResult;
}

int KClientController::Run()
{
    int              nResult        = false;    
    int              nRetCode       = false;  
    unsigned         nLoopTimes     = MAX_RECV_LOOP_TIMES;
    int              nCompleteState = true;
    KSimulateClient* pClient        = NULL;

    for (int i = 0; i < m_nClientCount; i++)
    {
        pClient = m_ClientsList[i];

        KGThread_Sleep(400);
        nRetCode = pClient->Run();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    while(true)
    {
        KGThread_Sleep(100);
        CheckAllClientTaskCompleteState(&nCompleteState);
        KG_PROCESS_SUCCESS(nCompleteState);
        nLoopTimes--;
        KG_PROCESS_SUCCESS(nLoopTimes == 0);
    }

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        for (int i = 0; i < m_nClientCount; i++)
        {
            pClient = m_ClientsList[i];
            pClient->UnInit();
            KG_DELETE(pClient);
        }
        m_ClientsList.clear();
    }
    return nResult;
}

void KClientController::UnInit()
{
    KSimulateClient* pClient = NULL;

    m_nClientCount = (int)m_ClientsList.size();

    for (int i = 0; i < m_nClientCount; i++)
    {
        pClient = m_ClientsList[i];
        pClient->UnInit();
        KG_DELETE(pClient);
    }
    m_ClientsList.clear();
}

int KClientController::FillOperate(int nOperateCount, int nOperate...)
{
    int              nResult           = false;
    int              nRetCode          = false;
    KSimulateClient* pClient           = NULL;
    va_list          ap;
    int              nTempOperate      = 0;

    KGLOG_PROCESS_ERROR(nOperateCount > 0);
    KGLOG_PROCESS_ERROR(nOperate > onStart);
    KGLOG_PROCESS_ERROR(nOperate < onTotal);

    for (int i = 0; i < m_nClientCount; i++)
    {
        pClient  = m_ClientsList[i];
        nRetCode = pClient->FillOperateToOperateList(nOperate, osReady);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
  
    va_start(ap, nOperate);
    for(int i = 1; i < nOperateCount; i++)
    { 
        nTempOperate = va_arg(ap,int);
        KGLOG_PROCESS_ERROR(nTempOperate > onStart);
        KGLOG_PROCESS_ERROR(nTempOperate < onTotal);

        for (int j = 0; j< m_nClientCount; j++)
        {
            pClient  = m_ClientsList[j];
            nRetCode = pClient->FillOperateToOperateList(nTempOperate, osNotReady);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }
    va_end(ap);

    nResult = true;
Exit0:
    return nResult; 
}

int KClientController::AddOperateToClient(int nClientNum, int nOperateCount, int nOperate...)
{
    int              nResult        = false;
    //int              nRetCode       = false;
    KSimulateClient* pClient        = NULL;
    va_list          ap;
    int              nTempOperte    = 0;

    KGLOG_PROCESS_ERROR(nOperateCount > 0);
    KGLOG_PROCESS_ERROR(nOperate > onStart);
    KGLOG_PROCESS_ERROR(nOperate < onTotal);

    KGLOG_PROCESS_ERROR((nClientNum >= 0) && (nClientNum < (int)m_ClientsList.size()));

    pClient = m_ClientsList[nClientNum];

    pClient->FillOperateToOperateList(nOperate, osReady);

    va_start(ap, nOperate);
    for(int i = 1; i < nOperateCount; i++)
    { 
        nTempOperte = va_arg(ap,int);
        KGLOG_PROCESS_ERROR(nTempOperte > onStart);
        KGLOG_PROCESS_ERROR(nTempOperte < onTotal);

        pClient->FillOperateToOperateList(nTempOperte, osNotReady);
    }
    va_end(ap);

    nResult = true;
Exit0:
    return nResult; 
}

int KClientController::GetReturnResult(int nClientNum, KG_RETURN_RESULT* pResult)
{
    int              nResult    = false;
    int              nRetCode   = false;
    KSimulateClient* pClient    = NULL;

    KGLOG_PROCESS_ERROR(pResult);
    KGLOG_PROCESS_ERROR((nClientNum >= 0) && (nClientNum < (int)m_ClientsList.size()));

    pClient  = m_ClientsList[nClientNum];
    nRetCode = pClient->GetReturnResult(pResult);
    KGLOG_PROCESS_ERROR(nRetCode);
  
    nResult = true;
Exit0:
    return nResult; 
}

int KClientController::CheckTheAccountLockedOrNot(int nClientNum, int* pnAccountLockedState)
{
    int              nResult    = false;
    int              nRetCode   = false;
    KSimulateClient* pClient    = NULL;
    
    KGLOG_PROCESS_ERROR(pnAccountLockedState);
    KGLOG_PROCESS_ERROR((nClientNum >= 0) && (nClientNum < (int)m_ClientsList.size()));

    pClient  = m_ClientsList[nClientNum];
    nRetCode = pClient->CheckTheAccountLockedOrNot(pnAccountLockedState);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult; 
}

int KClientController::ModifyClientUserName(int nClientNum, char szNewName[])
{
    int                 nResult     = false;
    int                 nRetCode    = false;
    KSimulateClient*    pClient     = NULL;

    KGLOG_PROCESS_ERROR(szNewName);
    KGLOG_PROCESS_ERROR((nClientNum >= 0) && (nClientNum < (int)m_ClientsList.size()));

    pClient  = m_ClientsList[nClientNum];
    nRetCode = pClient->ModifyUserName(szNewName);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult; 
}

int KClientController::CheckAllClientTaskCompleteState(int* pnCompleteState)
{
    int                 nResult              = false;
    int                 nRetCode             = false;
    KSimulateClient*    pClient              = NULL;
    int                 nSingleCompleteState = false;

    *pnCompleteState = false;
    m_nClientCount   = (int)m_ClientsList.size();

    for (int i = 0; i < m_nClientCount; i++)
    {
        pClient = m_ClientsList[i];
        nRetCode = pClient->TaskIsCompleted(&nSingleCompleteState);
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(!nSingleCompleteState);
    }

    *pnCompleteState = true;

Exit1:
    nResult = true;
Exit0:
    return nResult; 
}

int KClientController::GetClientConfig(KG_SIMULATE_CLIENT_CONFIG* pClientConfig, int nClientNum)
{
    int                 nResult     = false;
    int                 nRetCode    = false;
    KSimulateClient*    pClient     = NULL;
    
    KGLOG_PROCESS_ERROR(pClientConfig);
    KGLOG_PROCESS_ERROR((nClientNum >= 0) && (nClientNum < (int)m_ClientsList.size()));

    pClient  = m_ClientsList[nClientNum];
    nRetCode = pClient->GetConfig(pClientConfig);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult; 
}
