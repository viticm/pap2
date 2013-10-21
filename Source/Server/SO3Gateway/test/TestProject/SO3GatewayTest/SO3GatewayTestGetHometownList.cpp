///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   SO3GatewayTestGetHometownList.cpp
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

void KGatewaytest::FillTestGetHometownListInfo(
    int nHometownCount,
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
    m_GatewayTestConfig.GameCenterConfig.nGetRoleListCount   = 0;
    m_GatewayTestConfig.GameCenterConfig.nMapCount           = nHometownCount;
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
    m_GatewayTestConfig.ForecastConfig.nTestPoint = KG_HOMETOWN_LIST;
    m_GatewayTestConfig.ForecastConfig.nResult = nResult;
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;    

    //[State]
    m_ReturnResult.nTestPoint = m_GatewayTestConfig.ForecastConfig.nTestPoint;
    m_ReturnResult.nResult = 0;
    m_ReturnResult.nTypeResult = 0;
}

void KGatewaytest::TestTestGetHometownListHaveZeroMap()     // caseID:5143
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5143,Paysys正常登陆时，玩家不在线,用户获取新手村，GameCenter中新手村数目为0时,Gateway的操作是否准确";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5143\n");

    FillTestGetHometownListInfo(0, 0, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onQueryNewbieMaps
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

void KGatewaytest::TestTestGetHometownListHaveTwoMap()  // caseID:5144
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5144, Paysys正常登陆时，玩家不在线,用户获取新手村，GameCenter中新手村数目为2时,Gateway的操作是否准确";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5144\n");

    FillTestGetHometownListInfo(2, 2, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onQueryNewbieMaps
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

void KGatewaytest::TestGetHometownListGCNotRespondUserDisconnect()  // caseID:5145
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5145, GameCenter不返回新手村,Client玩家断线,Paysys收到登出信息";
    int HadRevUserLoginOut = false;
    unsigned    nLoopTimes            = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:5145\n");

    FillTestGetHometownListInfo(2, PAYSYS_HAD_REV_USER_LOGINOUT, true);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol = g2r_query_newbie_maps; // g2r_query_newbie_maps

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        5, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onQueryNewbieMaps,
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

void KGatewaytest::TestGetHometownListUserDisconnectReLogin()  // caseID:6681
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6681,测试Paysys正常登陆时,Client玩家发送请求角色列表请求后断线接着用同一账号重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6681\n");

    FillTestGetHometownListInfo(2, 2, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        8, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onQueryNewbieMaps,
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
