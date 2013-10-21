///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : SO3GatewayTest.cpp
// Creator  : liuzhibiao
// Date     : 2008-11-24
// Comment  : v0.6
//            2008.08.27	Create0.1
//            v0.6      增加Gateway监控模块 
///////////////////////////////////////////////////////////////

#include "stdafx.h" 
#pragma   warning( disable: 4127)
#include "KClientController.h"
#include "KSimulateClient.h"
#include "KSimulatePaysys.h"
#include "KSimulateRelay.h"
#include "SO3GatewayTest.h"
#include "KGateWayMonitor.h"
#include "KSO3Gateway.h"

#ifndef WIN32
#include <sys/wait.h>   //在linux下调用waitpid
#endif

CPPUNIT_TEST_SUITE_REGISTRATION(KGatewaytest); 

int KGatewaytest::Init()
{
    int     nResult    = false;    
    int     nRetCode   = false;   

    m_pPaysys               = NULL;
    m_pRelay                = NULL;
    m_pClientController     = NULL; 
    m_pGateway              = NULL;
    m_pGatewayMonitor       = NULL;

    m_nGatewayMonitorInitFlag   = false;
    m_nGatewayExitFlag          = false;
    m_nClientControllerInitFlag = false;
    m_nPaysysInitFlag           = false;
    m_nRelayInitFlag            = false;
    m_nGatewayInitFlag          = false;
    m_nHadReadTheConfig         = false;
    m_nGatewayWorkThreadFlag    = false;

    memset(&m_ReturnResult, 0, sizeof(m_ReturnResult));

    nRetCode = FillConnectConfig(GATEWAY_CONNECT_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(nRetCode);
    m_nHadReadTheConfig = true;

    m_pGatewayMonitor = new KGateWayMonitor;
    KGLOG_PROCESS_ERROR(m_pGatewayMonitor);
    
    m_pPaysys = new KSimulatePaysys;
    KGLOG_PROCESS_ERROR(m_pPaysys);

    m_pRelay = new KSimulateRelay;
    KGLOG_PROCESS_ERROR(m_pRelay);

    m_pClientController = new KClientController;
    KGLOG_PROCESS_ERROR(m_pClientController);

    m_pGateway = new KSO3Gateway;
    KGLOG_PROCESS_ERROR(m_pGateway);

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(m_pGateway);
        KG_DELETE(m_pClientController);
        KG_DELETE(m_pRelay);
        KG_DELETE(m_pPaysys);
        KG_DELETE(m_pGatewayMonitor);
    }
    return nResult;
}

void KGatewaytest::UnInit()
{
    int nResult  = false;
    //int nRetCode = false;
    if (m_nGatewayWorkThreadFlag)
    {
        m_GatewayWorkThread.Destroy();
        m_nGatewayWorkThreadFlag = false;
    }
    KG_DELETE(m_pGateway);
    KG_DELETE(m_pClientController);
    KG_DELETE(m_pRelay);      
    KG_DELETE(m_pPaysys);
    KG_DELETE(m_pGatewayMonitor);

    nResult = true;
//Exit0:
    return ;
}

int KGatewaytest::FillConnectConfig(const char cszConnectConfigFileName[])
{
    int         nResult     = false;
    int         nRetCode    = false;
    IIniFile*   piIniFile   = NULL;

    ASSERT(cszConnectConfigFileName);

    piIniFile = g_OpenIniFile(cszConnectConfigFileName);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString(
        "Paysys", 
        "UserName",
        "",
        m_GatewayTestConfig.m_szTestGWUserName,
        sizeof(m_GatewayTestConfig.m_szTestGWUserName)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "Relay", 
        "IP", 
        "", 
        m_GatewayTestConfig.GameCenterConfig.szRelayIPAddr,
        sizeof(m_GatewayTestConfig.GameCenterConfig.szRelayIPAddr)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(
        "Relay", 
        "Port", 
        0, 
        &m_GatewayTestConfig.GameCenterConfig.nRelayPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "Paysys",
        "IP", 
        "", 
        m_GatewayTestConfig.PaysysManagerConfig.szPaysysIPAddr, 
        sizeof(m_GatewayTestConfig.PaysysManagerConfig.szPaysysIPAddr)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(
        "Paysys",
        "Port", 
        0, 
        &m_GatewayTestConfig.PaysysManagerConfig.nPaysysPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "Player", 
        "IP",
        "", 
        m_GatewayTestConfig.ClientConfig.szConnectIPAddr, 
        sizeof(m_GatewayTestConfig.ClientConfig.szConnectIPAddr)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(
        "Player", 
        "Port", 
        0, 
        &m_GatewayTestConfig.ClientConfig.nConnectPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);

#ifdef WIN32
    nRetCode = piIniFile->GetString(
        "TestConfig", 
        "GateWay_WinPath",
        "d", 
        m_GatewayTestConfig.m_szGatewayPath, 
        sizeof(m_GatewayTestConfig.m_szGatewayPath)
    );
    KGLOG_PROCESS_ERROR(nRetCode);
#else
    nRetCode = piIniFile->GetString(
        "TestConfig", 
        "GateWay_LinuxPath",
        "d", 
        m_GatewayTestConfig.m_szGatewayPath, 
        sizeof(m_GatewayTestConfig.m_szGatewayPath)
    );
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

    nResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return nResult;
}

void KGatewaytest::UnInitTestEnvironment()
{
    if (m_nClientControllerInitFlag)
    {
        m_pClientController->UnInit();   
        m_nClientControllerInitFlag = false;
    }

    if (m_nGatewayWorkThreadFlag)
    {
        m_pGateway->Quit();
        KGThread_Sleep(10);
        m_GatewayWorkThread.Destroy();
        m_nGatewayWorkThreadFlag = false;
    }

    if (m_nGatewayInitFlag)
    {
        m_pGateway->UnInit();
        m_nGatewayInitFlag = false;
    }

    if (m_nRelayInitFlag)
    {
        m_pRelay->UnInit();  
        m_nRelayInitFlag = false;
    }
    if (m_nPaysysInitFlag)
    {
        m_pPaysys->UnInit();
        m_nPaysysInitFlag = false;
    }
//#ifdef WIN32
//    if (m_nGatewayInitFlag)
//    {
//
//        m_pGatewayMonitor->DoE2LStop();
//
//        HWND hGWwnd = ::FindWindow(NULL, m_GatewayTestConfig.m_szGatewayPath);
//        ::PostMessage(hGWwnd, WM_CHAR, ' ', NULL);
//
//        ::WaitForSingleObject(m_GWprocInfo.hProcess, 20000);
//        if (hGWwnd)
//        {
//            TerminateProcess(m_GWprocInfo.hProcess, 0);
//        }
//        else
//        {
//            KGLogPrintf(KGLOG_INFO, "SO3Gateway没正常关闭,之后的用例有可能执行不到\n");
//        }
//        // Close process and thread handles. 
//        ::CloseHandle(m_GWprocInfo.hProcess);
//        ::CloseHandle(m_GWprocInfo.hThread);
//    }
//#else
//    if (m_nGatewayInitFlag)
//    {
//        int nGWPid;
//        int nRet;
//
//        m_pGatewayMonitor->DoE2LGetPid();
//        KGThread_Sleep(1000);
//        m_pGatewayMonitor->GetGatewayPid(&nGWPid);
//
//        m_pGatewayMonitor->DoE2LStop();
//
//        nRet = waitpid(nGWPid, NULL, 0);  
//
//        KGThread_Sleep(6000);  //在linux下保证其他用例初始化环境时成功
//        m_nGatewayInitFlag = false;
//    }
//#endif

    if (m_pGatewayMonitor)
    {
        m_pGatewayMonitor->UnInit();
        m_nGatewayMonitorInitFlag = false;
    }
    m_nHadReadTheConfig = false;
}

int KGatewaytest::TestThisCaseInPaysys()
{
    int         nResult               = false;    
    int         nRetCode              = false;
    int         nPaysysHadConnected   = false;
    int         nRelayHadConnected    = false;

    KGLOG_PROCESS_ERROR(m_pPaysys);
    KGLOG_PROCESS_ERROR(m_pRelay);
    KGLOG_PROCESS_ERROR(m_pGateway);
#ifndef WIN32
    {
        Idle(110);
    }
#endif

    nRetCode = m_pPaysys->Init(m_GatewayTestConfig.PaysysManagerConfig);
    KG_PROCESS_ERROR(nRetCode);
    m_nPaysysInitFlag = true;

    nRetCode = m_pRelay->Init(m_GatewayTestConfig.GameCenterConfig);
    KG_PROCESS_ERROR(nRetCode);
    m_nRelayInitFlag = true;

    KGThread_Sleep(100);
    nRetCode = m_pGateway->Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    m_nGatewayInitFlag = true;

    nRetCode = m_GatewayWorkThread.Create(GatewayWorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    m_nGatewayWorkThreadFlag = true;

    //#ifdef WIN32

    //    LPSECURITY_ATTRIBUTES processA = NULL;//Default
    //    LPSECURITY_ATTRIBUTES threadA = NULL;//Default
    //    BOOL shareRights = TRUE;//Default
    //    DWORD creationMask = CREATE_NEW_CONSOLE;//Window per process.
    //    LPVOID enviroment = NULL;//Default
    //    LPSTR curDir = NULL;//Default
    //    STARTUPINFO startInfo;//Result

    //    ZeroMemory(&startInfo, sizeof(startInfo));
    //    startInfo.cb = sizeof(startInfo);

    //    startInfo.dwFlags = STARTF_USESHOWWINDOW;
    //    startInfo.wShowWindow = SW_MINIMIZE;

    //    for (int i =0; i < 10; i++)
    //    {
    //        nRetCode = CreateProcess(
    //            NULL,//File name of executable
    //            m_GatewayTestConfig.m_szGatewayPath,//command line
    //            processA,//Process inherited security
    //            threadA, //Thread inherited security
    //            shareRights,//Rights propagation
    //            creationMask,//various creation flags
    //            enviroment,//Enviroment variable
    //            curDir,           //Child's current directory
    //            &startInfo,
    //            &m_GWprocInfo
    //        );
    //        if (nRetCode)
    //        {
    //            break;
    //        }
    //    }
    //    KGLOG_PROCESS_ERROR(nRetCode);
    //    m_nGatewayInitFlag = true;
    //#else
    //    m_nGWpid = fork();
    //    if (m_nGWpid < 0)
    //    {
    //        goto Exit0;
    //    }
    //    else
    //    {
    //        if (m_nGWpid == 0)
    //        {
    //            execv("SO3Gateway_LinuxD", NULL);
    //            exit(0);
    //        }
    //    }
    //    m_nGatewayInitFlag = true;
    //#endif

    while (true)
    {
        nRetCode = m_pPaysys->HadConnections(&nPaysysHadConnected);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = m_pRelay->HadConnections(&nRelayHadConnected);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (nPaysysHadConnected && nRelayHadConnected)
        {
            break;
        }
    }

//Exit1:
    nResult  = true; 
Exit0:
    if (!nResult)
    {
        if (m_nGatewayWorkThreadFlag)
        {
            m_pGateway->Quit();
            KGThread_Sleep(100);

            m_GatewayWorkThread.Destroy();
            m_nGatewayWorkThreadFlag = false;
        }
        if (m_nGatewayInitFlag)
        {
            m_pGateway->UnInit();
            m_nGatewayInitFlag = false;
        }
        if (m_nRelayInitFlag)
        {
            m_pRelay->UnInit();
            m_nRelayInitFlag = false;
        }
        if (m_nPaysysInitFlag)
        {
            m_pPaysys->UnInit();
            m_nPaysysInitFlag = false;
        }
    }
    return nResult;
}

int KGatewaytest::InitTestEnvironment(
      int nNeedPaysys, 
      int nNeedRelay,
      int nNeedGateway, 
      int nClinetCount,
      int nTestPoint
)
{
    int     nResult                 = false;    
    int     nRetCode                = false;
    int     nPaysysHadConnected     = false;
    int     nRelayHadConnected      = false;

    KGLOG_PROCESS_ERROR(m_nHadReadTheConfig);
    KGLOG_PROCESS_ERROR(m_pPaysys);
    KGLOG_PROCESS_ERROR(m_pRelay);
    if (nClinetCount > 0)
    {
        KGLOG_PROCESS_ERROR(m_pClientController);
    }
    
#ifndef WIN32
    {
        Idle(20);
    }
#endif

    nRetCode = m_pGatewayMonitor->Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    m_nGatewayMonitorInitFlag = true;

    if (nNeedPaysys)
    {
        for (int i = 0; i < 10; i++)
        {
            nRetCode = m_pPaysys->Init(m_GatewayTestConfig.PaysysManagerConfig);
            if (nRetCode)
            {
                break;
            }
        }
        
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nPaysysInitFlag = true;
    }

    if (nNeedRelay)
    {
        for (int i = 0; i < 10; i++)
        {
            nRetCode = m_pRelay->Init(m_GatewayTestConfig.GameCenterConfig);
            if (nRetCode)
            {
                break;
            }
        }
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nRelayInitFlag = true;
    }

    //if (nNeedGateway)
    //{
    //    KGThread_Sleep(100);
    //    #ifdef WIN32
    //        LPSECURITY_ATTRIBUTES processA = NULL;//Default
    //        LPSECURITY_ATTRIBUTES threadA = NULL;//Default
    //        BOOL shareRights = TRUE;//Default
    //        DWORD creationMask = CREATE_NEW_CONSOLE;//Window per process.
    //        LPVOID enviroment = NULL;//Default
    //        LPSTR curDir = NULL;//Default
    //        STARTUPINFO startInfo;//Result

    //        ZeroMemory(&startInfo, sizeof(startInfo));
    //        startInfo.cb = sizeof(startInfo);
    //        
    //        startInfo.dwFlags = STARTF_USESHOWWINDOW;
    //        startInfo.wShowWindow = SW_MINIMIZE;
    //        //startInfo.wShowWindow = SW_HIDE;
    //        for (int i = 0; i < 10; i++)
    //        {
    //            nRetCode = CreateProcess(
    //                NULL,//File name of executable
    //                m_GatewayTestConfig.m_szGatewayPath,//command line
    //                processA,//Process inherited security
    //                threadA, //Thread inherited security
    //                shareRights,//Rights propagation
    //                creationMask,//various creation flags
    //                enviroment,//Enviroment variable
    //                curDir,           //Child's current directory
    //                &startInfo,
    //                &m_GWprocInfo
    //            );
    //            if (nRetCode)
    //            {
    //                break;
    //            }
    //        }
    //        KGLOG_PROCESS_ERROR(nRetCode);

    //        m_nGatewayInitFlag = true;
    //    #else
    //        m_nGWpid = fork();
    //        if (m_nGWpid < 0)
    //        {
    //            goto Exit0;
    //        }
    //        else
    //        {
    //            if (m_nGWpid == 0)
    //            {
    //                execv("SO3Gateway_LinuxD", NULL);
    //                exit(0);
    //            }
    //        }
    //        m_nGatewayInitFlag = true;
    //    #endif
    //}
    if (nNeedGateway)
    {
        KGThread_Sleep(100);
        nRetCode = m_pGateway->Init();
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nGatewayInitFlag = true;

        nRetCode = m_GatewayWorkThread.Create(GatewayWorkThreadFunction, this);
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nGatewayWorkThreadFlag = true;
    }

    while (true)
    {
        nRetCode = m_pPaysys->HadConnections(&nPaysysHadConnected);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_pRelay->HadConnections(&nRelayHadConnected);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (nPaysysHadConnected && nRelayHadConnected)
        {
            break;
        }
    }

    if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    {
        m_pPaysys->AllDisConnected(true);
    }

    if (nClinetCount > 0)
    {
        KGThread_Sleep(1000);
        nRetCode = m_pClientController->Init(m_GatewayTestConfig.ClientConfig, nTestPoint, nClinetCount);
        KGLOG_PROCESS_ERROR(nRetCode);
        m_nClientControllerInitFlag = true;
    }

    nResult  = true; 
Exit0:
    if (!nResult)
    {
        if (m_nClientControllerInitFlag)
        {
            m_pClientController->UnInit();
            m_nClientControllerInitFlag = false;
        }
        if (m_nGatewayWorkThreadFlag)
        {
            m_pGateway->Quit();
            KGThread_Sleep(100);

            m_GatewayWorkThread.Destroy();
            m_nGatewayWorkThreadFlag = false;
        }
        if (m_nGatewayInitFlag)
        {
            m_pGateway->UnInit();
            m_nGatewayInitFlag = false;
        }
        if (m_nRelayInitFlag)
        {
            m_pRelay->UnInit();
            m_nRelayInitFlag = false;
        }
        if (m_nPaysysInitFlag)
        {
            m_pPaysys->UnInit();
            m_nPaysysInitFlag = false;
        }
    }
    return nResult;
}

void KGatewaytest::Idle(int nLoodTimes)
{
    unsigned  nLoopTimes = nLoodTimes;

    while(true)
    {
        KGThread_Sleep(100);
        nLoopTimes--;
        if (!nLoopTimes)
        {
            break;
        }
    }
}

void KGatewaytest::GatewayWorkThreadFunction( void *pvParam )
{
    KGatewaytest *pThis = (KGatewaytest *)pvParam;

    ASSERT(pThis);

    pThis->GatewayThreadFunction();

}

int KGatewaytest::GatewayThreadFunction()
{
    int nResult  = false;    
    //int nRetCode = false;

    KGLOG_PROCESS_ERROR(m_pGateway);

    m_pGateway->Run();
    //KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}