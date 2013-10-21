///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   SO3GatewayTestCreateRole.cpp
// Creator  :   liuzhibiao
// Date     :   2008-9-23
// Comment  :   v0.1
//              v0.1 2008-9-23 create
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SO3GatewayTest.h"
#include "KClientController.h"
#include "KSimulatePaysys.h"
#include "GatewayRelayProtocol.h"

void KGatewaytest::FillTestCreateRoleInfo(
    int nRoleCount,
    int nRetGameCenterCreateRole,
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
    m_GatewayTestConfig.GameCenterConfig.nGetRoleListCount   = nRoleCount;
    m_GatewayTestConfig.GameCenterConfig.nCreatRoleResult    = nRetGameCenterCreateRole;
    m_GatewayTestConfig.ClientConfig.nHadRevKick             = false;

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
    m_GatewayTestConfig.ForecastConfig.nTestPoint = KG_CREATE_ROLE;
    m_GatewayTestConfig.ForecastConfig.nResult = nResult;
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;    

    //[State]
    m_ReturnResult.nTestPoint = m_GatewayTestConfig.ForecastConfig.nTestPoint;
    m_ReturnResult.nResult = 0;
    m_ReturnResult.nTypeResult = 0;
}

void KGatewaytest::TestCreateRoleHaveZeroRoles()    // caseID:5134
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5134,Paysys正常登陆时，玩家不在线,GameCenter中角色数目为0时, 用户创建角色,Gateway的操作是否准确";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5134\n");

    FillTestCreateRoleInfo(0, eCreateRoleSucceed, eCreateRoleSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onCreateRole
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

void KGatewaytest::TestCreateRoleHaveTwoRoles() // caseID:5135
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5135,Paysys正常登陆时，玩家不在线,GameCenter中角色数目为2时, 用户创建角色,Gateway的操作是否准确";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5135\n");

    FillTestCreateRoleInfo(2, eCreateRoleSucceed, eCreateRoleSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onCreateRole
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

void KGatewaytest::TestCreateRoleHaveThreeRoles()   // caseID:5136
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5136,Paysys正常登陆时，玩家不在线,GameCenter中角色数目为3时, 用户创建角色,Gateway的操作是否准确";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5136\n");

    FillTestCreateRoleInfo(3, eCreateRoleUnableToCreate, eCreateRoleUnableToCreate, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onCreateRole
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

#ifndef WIN32
    {
        Idle(50);
    }
#endif

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
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

void KGatewaytest::TestCreateRoleGameCenterRefuse() // caseID:5133
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5133,测试Client玩家创建角色失败时，在GameCenter中返回创建角色失败, 角色未被创建";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5133\n");

    FillTestCreateRoleInfo(0, eCreateRoleInvalidRoleName, eCreateRoleInvalidRoleName, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onCreateRole
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

void KGatewaytest::TestClientCreateRoleCGNotRespondUserDisconnect() // caseID:5137
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5137, GameCenter不返回创建结果,Client玩家断线,Paysys收到登出信息";
    int HadRevUserLoginOut = false;
    unsigned    nLoopTimes            = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:5137\n");

    FillTestCreateRoleInfo(0, eCreateRoleSucceed, PAYSYS_HAD_REV_USER_LOGINOUT, true);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol = g2r_create_role_request; // g2r_create_role_request

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        5, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onCreateRole,
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


void KGatewaytest::TestCreateRoleUserDisconnectReLogin()  // caseID:6679
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6679,测试Paysys正常登陆时,Client玩家发送创建角色请求后断线接着用同一账号重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;
    
    KGLogPrintf(KGLOG_INFO, "caseID:6679\n");

    FillTestCreateRoleInfo(0, eCreateRoleSucceed, eCreateRoleSucceed, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        8, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onCreateRole,
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
        CPPUNIT_ASSERT(nResult);
    }
#endif
}

