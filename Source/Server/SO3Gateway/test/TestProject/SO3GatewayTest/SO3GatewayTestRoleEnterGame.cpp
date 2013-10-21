///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   SO3GatewayTestRoleEnterGame.cpp
// Creator  :   liuzhibiao
// Date     :   2009-2-12
// Comment  :   v0.2
//              v0.1 2008-9-23 create
//              v0.2 2008-11-24 增加离线检测用例代码和linux的cppunit兼容代码
//              v0.3 2009-2-12 加入单个角色冻结测试用例
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SO3GatewayTest.h"
#include "KClientController.h"
#include "KSimulateRelay.h"
#include "KSimulatePaysys.h"
#include "KGateWayMonitor.h"

void KGatewaytest::FillTestRoleEnterGameInfo(
    int nRetGameCenterRoleEnterGame,
    int nResult,
    int nNeedStopWhenRevAppointInfo
)
{
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult         = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect          = false;
    m_GatewayTestConfig.GameCenterConfig.nGameWorldProtocolLowerVersion = 0;
    m_GatewayTestConfig.GameCenterConfig.nGameWorldProtocolUpperVersion = 100;
    m_GatewayTestConfig.ClientConfig.nClientGatewayVersion              = CLIENT_GATEWAY_VERSION;
    m_GatewayTestConfig.ClientConfig.nClientGameWorldVersion            = 20;
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol            = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol         = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol          = 0;
    m_GatewayTestConfig.ClientConfig.nNeedTestTimeOut                   = false;
    m_GatewayTestConfig.ClientConfig.nNeedStopWhenRevAppointInfo        = nNeedStopWhenRevAppointInfo;
    m_GatewayTestConfig.ClientConfig.nNeedSendPing                      = true;

    m_GatewayTestConfig.PaysysManagerConfig.nUserLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.GameCenterConfig.nUserOnlineResult   = false;
    m_GatewayTestConfig.GameCenterConfig.nGetRoleListCount   = 3;
    m_GatewayTestConfig.GameCenterConfig.nEnterGameResult    = nRetGameCenterRoleEnterGame;
    m_GatewayTestConfig.ClientConfig.nHadRevKick             = false;
    m_GatewayTestConfig.ClientConfig.nUserGiveUpQueue        = false;

    m_GatewayTestConfig.PaysysManagerConfig.wType            = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode      = PASSPODMODE_UNBIND;//临时

    strncpy(
        m_GatewayTestConfig.ClientConfig.szClientAccountName,
        "Jx3AccountName", 
        sizeof(m_GatewayTestConfig.ClientConfig.szClientAccountName)
    );
    strncpy(
        m_GatewayTestConfig.ClientConfig.szClientActivateCode,
        "Jx3ActivateCode",
        sizeof(m_GatewayTestConfig.ClientConfig.szClientActivateCode)
    );
    strncpy(
        m_GatewayTestConfig.ClientConfig.szClientAccountPassword.szPassword,
        "Jx3AccountPassword",
        sizeof(m_GatewayTestConfig.ClientConfig.szClientAccountPassword.szPassword)
    );

    //[Forecast]
    m_GatewayTestConfig.ForecastConfig.nTestPoint = KG_ROLE_ENTER_GAME;
    m_GatewayTestConfig.ForecastConfig.nResult = nResult;
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;    

    //[State]
    m_ReturnResult.nTestPoint = m_GatewayTestConfig.ForecastConfig.nTestPoint;
    m_ReturnResult.nResult = 0;
    m_ReturnResult.nTypeResult = 0;
}

void KGatewaytest::TestRoleEnterGameSystemMaintenance() // caseID:5127
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5127,Paysys正常登陆时,GameCenter系统维护, 用户选择角色进入游戏,Client玩家收到服务器维护";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5127\n");

    FillTestRoleEnterGameInfo(eGameLoginSystemMaintenance, eGameLoginSystemMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameOverload()  // caseID:6519
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6519,Paysys正常登陆时,GameCenter服务器负载已满，无法登陆, Client玩家收到服务器已满";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6519\n");

    FillTestRoleEnterGameInfo(eGameLoginOverload, eGameLoginOverload, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameUnknownError()  // caseID:5128
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5128,Paysys正常登陆时，玩家不在线,GameCenter发生未知错误, 用户选择角色进入游戏,Client玩家收到未知错误";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5128\n");

    FillTestRoleEnterGameInfo(eGameLoginUnknownError, eGameLoginUnknownError, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameLoginSucceed()  // caseID:6668
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6668, Paysys正常登陆时，玩家不在线,GameCenter正常, 用户选择角色进入游戏,Client玩家收到成功登陆信息";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6668\n");

    FillTestRoleEnterGameInfo(eGameLoginSucceed, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameUserRevQueueWait() // caseID:6517
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6517,Paysys正常登陆时，玩家不在线,GameCenter正常人数已满需要排队, 用户选择角色进入游戏,Client玩家收到排队状态信息";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6517\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, CLIENT_REV_LOGINGAME_STATE_INFO, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameUserGCRevLoginRequest() // caseID:6667
{
    int  nResult                        = false;
    int  nRetCode                       = false;
    char szTestCaseDscribe[]            = "// caseID:6667,Paysys正常登陆时，玩家不在线,GameCenter正常人数已满需要排队, 用户选择角色进入游戏,Client玩家收到排队状态信息";
    int  nUserEnterGameRequestCount     = 0;
    unsigned    nLoopTimes              = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:6667\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, CLIENT_REV_LOGINGAME_STATE_INFO, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    while(true)
    {
        KGThread_Sleep(100);
        nUserEnterGameRequestCount = m_pRelay->GetUserEnterGameRequestCount();
        KG_PROCESS_SUCCESS(nUserEnterGameRequestCount >= 2);

        nLoopTimes--;
        KGLOG_PROCESS_ERROR(nLoopTimes);
    }

Exit1:
    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCNotRespondUserDisconnectPaysyRevLoginOut()  // caseID: 5132
{
    int  nResult  = false;
    int  nRetCode = false;
    char szTestCaseDscribe[] = "// caseID: 5132,GameCenter不响应登陆请求,Client玩家选择角色登陆后断线,Paysys收到登出信息";
    int  HadRevUserLoginOut = false;
    unsigned    nLoopTimes            = 100;   

    KGLogPrintf(KGLOG_INFO, "caseID:5132\n");

    FillTestRoleEnterGameInfo(eGameLoginSucceed, PAYSYS_HAD_REV_USER_LOGINOUT, true);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol = g2r_game_login_request; // g2r_game_login_request

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        5, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    while(true)
    {
        KGThread_Sleep(100);
        nRetCode = m_pPaysys->HadRevUserLoginOut("0", &HadRevUserLoginOut);
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(HadRevUserLoginOut);

        nLoopTimes--;
        KGLOG_PROCESS_ERROR(nLoopTimes);
    }

Exit1:
    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameUserDisconnectReLogin()  // caseID:6669
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6669,测试Client玩家获取角色信息成功时，GameCenter返回登陆成功之后Client玩家断线重连,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6669\n");

    FillTestRoleEnterGameInfo(eGameLoginSucceed, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        8, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->CheckTheAccountLockedOrNot(0, &nAccountLockedState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!nAccountLockedState);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCNotRespondUserDisconnectReLogin()  // caseID:6670
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6670,测试Client玩家获取角色信息成功，GameCenter不回登陆结果时,Client玩家断线重连,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6670\n");

    FillTestRoleEnterGameInfo(eGameLoginSucceed, eGameLoginSucceed, true);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol = g2r_game_login_request; // g2r_game_login_request

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        8, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->CheckTheAccountLockedOrNot(0, &nAccountLockedState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!nAccountLockedState);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

// 八、排队状态[easQueueWaiting](10个)

void KGatewaytest::TestRoleEnterGameUserDisconnectPaysyRevLoginOut()  // caseID: 6677
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID: 6677,测试获取角色信息成功时，GameCenter人数已满,Client玩家收到人数已满信息后断线,Paysys收到登出信息";
    int HadRevUserLoginOut = false;
    unsigned    nLoopTimes            = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:6677\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, PAYSYS_HAD_REV_USER_LOGINOUT, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        5, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    while(true)
    {
        KGThread_Sleep(100);
        nRetCode = m_pPaysys->HadRevUserLoginOut("0", &HadRevUserLoginOut);
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(HadRevUserLoginOut);

        nLoopTimes--;
        KGLOG_PROCESS_ERROR(nLoopTimes);
    }

Exit1:
    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCQueueWaitUserDisconnectReLogin()  // caseID:6678
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6678,测试获取角色信息成功时，GameCenter人数已满,Client玩家收到人数已满信息后断线,再重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6678\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, PAYSYS_HAD_REV_USER_LOGINOUT, true);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol = g2r_game_login_request; // g2r_game_login_request

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        8, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->CheckTheAccountLockedOrNot(0, &nAccountLockedState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!nAccountLockedState);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestTwoPlayerRoleEnterGameQueueWaitTheSecondUserGiveup()   //caseID: 6518
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "//caseID: 6518,测试两个Client玩家获取角色信息成功,都选择登陆游戏世界时,GameCenter人数已满,第二个Client玩家请求放弃排队登陆,第二个Client玩家收到放弃排队回应";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6518\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, CLIENT_REV_GIVEUP_QUEUE_RESPOND, true);

    nRetCode = InitTestEnvironment(true, true, true, 2, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    //nRetCode = m_pClientController->ModifyClientUserName(1, "二号玩家");
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(1, 1, onGiveUpQueue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(1, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCFullLoginAGSucceed()  // caseID:6671
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6671,测试模拟场景地图一已满人,Client玩家进入排队后模拟场景地图有退出了1位Client玩家，排队后,场景地图出现空位,Client玩家能正常登陆模拟场景地图一";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6671\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCFullLoginAGSystemMaintenance()  // caseID:6673
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6673,测试模拟场景地图一已满人,Client玩家进入排队之后GameCenter通知需要系统维护, Client玩家收到游戏世界维护信息";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6673\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSystemMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSystemMaintenance);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestFiveUserRoleEnterGameGCFullLoginAGTheFirstOneSucceed()  // caseID:6674
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6674,模拟场景地图一已满人,5个Client玩家进入排队之后模拟场景地图有退出了1位Client玩家.第1号Client玩家进入游戏世界,第2至4号Client玩家收到正确的排队信息包";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6674\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 5, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetAllowLoginInGameWorldCount(1);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    m_GatewayTestConfig.ForecastConfig.nResult = CLIENT_REV_LOGINGAME_STATE_INFO;
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;
    nRetCode = m_pClientController->GetReturnResult(1, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    m_GatewayTestConfig.ForecastConfig.nTypeResult = 1;
    nRetCode = m_pClientController->GetReturnResult(2, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    m_GatewayTestConfig.ForecastConfig.nTypeResult = 2;
    nRetCode = m_pClientController->GetReturnResult(3, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    m_GatewayTestConfig.ForecastConfig.nTypeResult = 3;
    nRetCode = m_pClientController->GetReturnResult(4, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestFiveUserRoleEnterGameGCFullLoginAGTheThirdOneGiveUp()  // caseID:6675
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6675,模拟场景地图一已满人,模拟场景地图一已满人,5个Client玩家进入排队.第3号Client玩家取消等待排队,各个Client玩家收到正确的排队信息包";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6675\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, CLIENT_REV_LOGINGAME_STATE_INFO, true);

    nRetCode = InitTestEnvironment(true, true, true, 5, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(2, 1, onGiveUpQueue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(120);
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;
    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    m_GatewayTestConfig.ForecastConfig.nTypeResult = 1;
    nRetCode = m_pClientController->GetReturnResult(1, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    m_GatewayTestConfig.ForecastConfig.nTypeResult = 2;
    nRetCode = m_pClientController->GetReturnResult(3, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    m_GatewayTestConfig.ForecastConfig.nTypeResult = 3;
    nRetCode = m_pClientController->GetReturnResult(4, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestFiveUserRoleEnterGameGCFullLoginAGAllSucceed()  // caseID:6676
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6676, 模拟场景地图一已满人,5个Client玩家进入排队之后模拟场景地图有退出了5位Client玩家.5个Client玩家实际登陆游戏世界的顺序与请求登陆游戏世界顺序相同";
    KInGameWorldPlayer        InGameWorldPlayer;

    KGLogPrintf(KGLOG_INFO, "caseID:6676\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 5, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetAllowLoginInGameWorldCount(100);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    nRetCode = m_pRelay->GetPlayer(&InGameWorldPlayer, "0");
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(InGameWorldPlayer.nNumInGameWorld == 1);

    nRetCode = m_pRelay->GetPlayer(&InGameWorldPlayer, "1");
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(InGameWorldPlayer.nNumInGameWorld == 2);

    nRetCode = m_pRelay->GetPlayer(&InGameWorldPlayer, "2");
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(InGameWorldPlayer.nNumInGameWorld == 3);

    nRetCode = m_pRelay->GetPlayer(&InGameWorldPlayer, "3");
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(InGameWorldPlayer.nNumInGameWorld == 4);

    nRetCode = m_pRelay->GetPlayer(&InGameWorldPlayer, "4");
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(InGameWorldPlayer.nNumInGameWorld == 5);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestUserRoleEnterGameGCFullLoginAGSucceed()  // caseID:5131
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5131,模拟场景地图一已满人,Client玩家进入排队之后模拟场景地图有退出了1位Client玩家.Client玩家进入游戏世界";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5131\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestUserRoleEnterGameGCFullLoginAGGCChangeTheQueue()  // caseID:6672
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6672,模拟场景地图一已满人,Client玩家进入排队之后GameCenter通知Gateway在模拟场景地图二上排队,GameCenter通知在另一幅场景排队登陆,Client玩家能正常登陆游戏世界";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:6672\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->ChangeTheLoginMap(2);
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(100);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountRoleEnterGameGCFullLoginAGSucceed()  // caseID:6824
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6824,测试模拟场景地图一已满人,Client玩家进入排队后模拟场景地图有退出了1位Client玩家，排队后,场景地图出现空位,Client玩家能正常登陆模拟场景地图一,Gateway队伍总人数为0（表示玩家离开了排队队伍）";
    //KG_RETURN_RESULT Result;
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6824\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSucceed);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(100);

    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 0);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountUserRoleEnterGameGCFullLoginAGGCChangeTheQueue()  // caseID:6825
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6825,模拟场景地图一已满人,Client玩家进入排队之后GameCenter通知Gateway在模拟场景地图二上排队,GameCenter通知在另一幅场景排队登陆,Gateway队伍总人数为1（表示玩家还在排队队伍中）";
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6825\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->ChangeTheLoginMap(2);
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 1);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountRoleEnterGameGCFullLoginAGSystemMaintenance()  // caseID:6826
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6826,测试模拟场景地图一已满人,Client玩家进入排队之后GameCenter通知需要系统维护, Client玩家收到游戏世界维护信息,Gateway队伍总人数为0（表示玩家离开了排队队伍）";
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6826\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSystemMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginSystemMaintenance);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(100);

    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 0);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountRoleEnterGameGCFullLoginAGSystemUnknowError()  // caseID:6827
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6827,测试模拟场景地图一已满人,Client玩家进入排队之后GameCenter发生未知错误, Client玩家收到未知错误,Gateway队伍总人数为0（表示玩家离开了排队队伍）";
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6827\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSystemMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginUnknownError);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(100);

    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 0);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountRoleEnterGameGCFullLoginAGGCNotRespond()  // caseID:6828
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6828,测试模拟场景地图一已满人,GameCenter第一次返回人数已满需要排队，GameCenter收到第二次请求登陆后，不返回结果信息，客户端断线,Gateway队伍总人数为0（表示玩家离开了排队队伍）";
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6828\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSystemMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetWhenRevDoNotRespond(g2r_game_login_request);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    m_pClientController->AddOperateToClient(0, 1, onDisConnect);
    Idle(100);

    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 0);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountTwoPlayerRoleEnterGameQueueWaitTheSecondUserGiveup()   //caseID: 6829
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "//caseID: 6829,测试两个Client玩家获取角色信息成功,都选择登陆游戏世界时,GameCenter人数已满,第二个Client玩家请求放弃排队登陆,Gateway队伍总人数为1（表示第二个玩家正常离开排队队伍）";
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6829\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, CLIENT_REV_GIVEUP_QUEUE_RESPOND, true);

    nRetCode = InitTestEnvironment(true, true, true, 2, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    //nRetCode = m_pClientController->ModifyClientUserName(1, "二号玩家");
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(1, 1, onGiveUpQueue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 1);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestQueueCountRoleEnterGameGCFullClientDisconnect()  // caseID:6830
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6830,测试获取角色信息成功，Client玩家选择登陆游戏世界时,GameCenter人数已满,Client玩家收到人数已满信息后断线,Gateway队伍总人数为0（表示玩家离开了排队队伍）";
    int nWaitingQueuePlayer = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:6830\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginSystemMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    m_pClientController->AddOperateToClient(0, 1, onDisConnect);
    Idle(100);

    nRetCode = m_pGatewayMonitor->GetWaitingQueuePlayerCount(&nWaitingQueuePlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(nWaitingQueuePlayer == 0);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameRoleFreeze() // caseID:10992
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:10992,测试Client玩家选择角色登陆游戏世界，GameCenter返回角色冻结，Client玩家收到角色冻结信息";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:10992\n");

    FillTestRoleEnterGameInfo(eGameLoginRoleFreeze, eGameLoginRoleFreeze, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCFullLoginAGRoleFreeze()  // caseID:10993
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:10993,测试Client玩家选择角色登陆游戏世界，进入了排队之后GameCenter返回角色冻结,Gateway运行是否准确";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:10993\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginRoleFreeze, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginRoleFreeze);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(100);

    nRetCode = m_pClientController->GetReturnResult(0, &Result);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = memcmp(&Result, &(m_GatewayTestConfig.ForecastConfig), sizeof(Result));
    KGLOG_PROCESS_ERROR(!nRetCode);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestDisconnectPaysyRevLoginOutRoleEnterGameGCFullLoginAGRoleFreeze()  // caseID:10994
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:10994,测试Client玩家选择角色登陆游戏世界，进入了排队之后GameCenter返回角色冻结,Client玩家收到角色冻结信息后断线, Paysys收到登出信息";
    int HadRevUserLoginOut = false;
    unsigned    nLoopTimes            = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:10994\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, eGameLoginRoleFreeze, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        5, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pRelay->SetGameLogResult(eGameLoginRoleFreeze);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

    while(true)
    {
        KGThread_Sleep(100);
        nRetCode = m_pPaysys->HadRevUserLoginOut("0", &HadRevUserLoginOut);
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(HadRevUserLoginOut);

        nLoopTimes--;
        KGLOG_PROCESS_ERROR(nLoopTimes);
    }

Exit1:
    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestRoleEnterGameGCQueueWaitFreezeRoleUserDisconnectReLogin()  // caseID:10995
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:10995,测试Client玩家选择角色登陆游戏世界，进入了排队之后GameCenter返回角色冻结,Client玩家收到角色冻结信息后断线,再重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:10995\n");

    FillTestRoleEnterGameInfo(eGameLoginQueueWait, PAYSYS_HAD_REV_USER_LOGINOUT, true);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol = g2r_game_login_request; // g2r_game_login_request

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        8, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onLoginGame,
        onDisConnect,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->CheckTheAccountLockedOrNot(0, &nAccountLockedState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!nAccountLockedState);

    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nResult);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nResult);
    }
#endif
}
