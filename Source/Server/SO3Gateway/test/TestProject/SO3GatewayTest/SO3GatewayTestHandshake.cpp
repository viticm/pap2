///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   SO3GatewayTestHandshake.cpp
// Creator  :   liuzhibiao
// Date     :   2008-11-5
// Comment  :   v0.2
//              v0.1 2008-9-22 create
//              v0.2 2008-11-5 添加新字段
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SO3GatewayTest.h"
#include "KClientController.h"

void KGatewaytest::FillTestHandShakeInfo(
    int nClientSendGatewayVersion,
    int nClientSendGameWorldVersion,
    int nGameWorldLowerVersion,
    int nGameWorldUpperVersion,
    int nPaysysLostconnect,
    int nClientForecastRecv,
    int nNeedStopWhenRevAppointInfo
)
{
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult         = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect          = nPaysysLostconnect;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol         = 0;
    m_GatewayTestConfig.GameCenterConfig.nGameWorldProtocolLowerVersion = nGameWorldLowerVersion;
    m_GatewayTestConfig.GameCenterConfig.nGameWorldProtocolUpperVersion = nGameWorldUpperVersion;
    m_GatewayTestConfig.ClientConfig.nClientGatewayVersion              = nClientSendGatewayVersion;
    m_GatewayTestConfig.ClientConfig.nClientGameWorldVersion            = nClientSendGameWorldVersion;
    m_GatewayTestConfig.PaysysManagerConfig.nUserLoginResult            = ACTION_SUCCESS;
    m_GatewayTestConfig.GameCenterConfig.nGetRoleListCount              = 0;
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol            = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol          = 0;
    m_GatewayTestConfig.ClientConfig.nNeedTestTimeOut                   = false;
    m_GatewayTestConfig.ClientConfig.nNeedStopWhenRevAppointInfo        = nNeedStopWhenRevAppointInfo;
    m_GatewayTestConfig.ClientConfig.nNeedSendPing                      = true;

    //[Forecast]
    m_GatewayTestConfig.ForecastConfig.nTestPoint = KG_CLIENT_HANDSHAKE;
    m_GatewayTestConfig.ForecastConfig.nResult = nClientForecastRecv;
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;    

    //[State]
    m_ReturnResult.nTestPoint = m_GatewayTestConfig.ForecastConfig.nTestPoint;
    m_ReturnResult.nResult = 0;
    m_ReturnResult.nTypeResult = 0;
}

void KGatewaytest::TestHandShakeGameWorldMaintenance()  // caseID:4998
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:4998,Client握手,GateCenter发回的游戏世界版本号范围上下限都为0,握手失败,Client玩家收到系统正在维护";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:4998\n");

    FillTestHandShakeInfo(CLIENT_GATEWAY_VERSION, 20, 0, 0, false, ghcGameWorldMaintenance, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        2, 
        onConnectToGateway,
        onHandShake
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

void KGatewaytest::TestHandShakeAccountSystemLost() // caseID:4999
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:4999,Client握手,Paysys连接不可用,握手失败,Client玩家收到帐号系统(paysys)连接丢失";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:4999\n");

    FillTestHandShakeInfo(CLIENT_GATEWAY_VERSION, 20, 0, 100, true, ghcAccountSystemLost, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        2, 
        onConnectToGateway,
        onHandShake
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

#ifdef __GNUC__
    KGThread_Sleep(20000);  //在linux下保证其他用例初始化环境时成功
#endif

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

void KGatewaytest::TestHandShakeGameWorldVersionError() // caseID:5000
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5000,Client握手,GateCenter发回的游戏世界版本号与客户端版本号不匹配,握手失败,Client玩家收到游戏世界版本不匹配,请更新客户端";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5000\n");

    FillTestHandShakeInfo(CLIENT_GATEWAY_VERSION, 20, 50, 100, false, ghcGameWorldVersionError, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        2, 
        onConnectToGateway,
        onHandShake
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

void KGatewaytest::TestHandShakeGatewayVersionNotMatch()    // caseID:4997
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:4997,Client握手,客户端保存的网关版本标志位与当前测试网关版本不匹配,握手失败,Client玩家收到网关版本不匹配,请更新Client玩家";
    KG_RETURN_RESULT Result;
    
    KGLogPrintf(KGLOG_INFO, "caseID:4997\n");

    FillTestHandShakeInfo(5, 20, 0, 100, false, ghcGatewayVersionError, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pClientController->FillOperate(
        2, 
        onConnectToGateway,
        onHandShake
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
