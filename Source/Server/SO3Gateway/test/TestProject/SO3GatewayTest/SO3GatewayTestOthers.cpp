///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   SO3GatewayTestOthers.cpp
// Creator  :   liuzhibiao
// Date     :   2008-9-24
// Comment  :   v0.1
//              v0.1 2008-9-24 create
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SO3GatewayTest.h"
#include "Protocol/AccountLoginDef.h"
#include "KSimulateRelay.h"
#include "KSimulatePaysys.h"

void KGatewaytest::TestGatewayReConnect()   // caseID:5148
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5148,测试Paysys 无数据交互时间超过设定时间，Gateway自动重连.";
    unsigned    nLoopTimes            = 300;
    int nGatewayConnectCount = 0;

    KGLogPrintf(KGLOG_INFO, "caseID:5148\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType            = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode      = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    {
        KGThread_Sleep(300);
        m_pPaysys->AllDisConnected(false);
        while(true)
        {
            KGThread_Sleep(100);
            nRetCode  = m_pPaysys->GetGatewayConnectCount(m_GatewayTestConfig.m_szTestGWUserName, &nGatewayConnectCount);
            KGLOG_PROCESS_ERROR(nRetCode);
            KG_PROCESS_SUCCESS(nGatewayConnectCount >= 2);

            nLoopTimes--;
            KGLOG_PROCESS_ERROR(nLoopTimes);
        }
    }
Exit1:
    nResult = true;
Exit0:
    UnInitTestEnvironment();
#ifdef WIN32
    {
        KG_CASE_INFO(szTestCaseDscribe, "");
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitPaysysReceiveChangeExtPointRequest()   // caseID:24741
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24741,测试Paysys正常登陆，GameCenter发来设置扩展点,paysys收到设置扩展点请求,Gateway是否转发正常";
    unsigned    nLoopTimes            = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24741\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoChangeExtPointRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pPaysys->m_nRevChangeExtPointRequest;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitRelayReceiveChangeExtPointRespond()   // caseID:24742
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24742,测试Paysys正常登陆，GameCenter发来设置扩展点,paysys收到设置扩展点请求并返回结果,Gateway是否转发正常";
    unsigned    nLoopTimes            = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24742\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoChangeExtPointRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pRelay->m_nRevChangeExtPointRespond;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitPaysysReceiveSetChargeFlagRequest()   // caseID:24743
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24743,测试Paysys正常登陆，GameCenter发来设置玩家收费模式请求,paysys收到设置玩家收费模式请求,Gateway是否转发正常";
    unsigned    nLoopTimes            = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24743\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoSetChargeFlagRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pPaysys->m_nPlayerSetChargeFlagRequest;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitRelayReceiveDoSetChargeFlagRespond()   // caseID:24744
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24744,测试Paysys正常登陆，GameCenter发来设置玩家收费模式请求,paysys收到设置玩家收费模式请求并返回结果,Gateway是否转发正常";
    unsigned    nLoopTimes            = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24744\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoSetChargeFlagRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pRelay->m_nRevPlayerSetChargeFlagRespond;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitPaysysReceiveActivePresentCoodeRequest()   // caseID:24745
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24745,测试Paysys正常登陆，GameCenter发来激活礼品包请求,paysys收到激活礼品包请求,Gateway是否转发正常";
    unsigned    nLoopTimes      = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24745\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoActivePresentCoodeRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pPaysys->m_nRevActivePresentCodeRequest;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitRelayReceiveActivePresentCoodeRespond()   // caseID:24746
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24746,测试Paysys正常登陆，GameCenter发来激活礼品包请求,paysys收到激活礼品包请求并返回结果,Gateway是否转发正常";
    unsigned    nLoopTimes            = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24746\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoActivePresentCoodeRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pRelay->m_nRevActivePresentCodeRespond;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitRelayReceiveGetZoneChargeFlagNotify()   // caseID:24747
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24747,测试Gateway正常连接Paysys,Paysys返回设置区服收费标记.Gateway是否正常转发";
    unsigned    nLoopTimes            = 300;

    KGLogPrintf(KGLOG_INFO, "caseID:24747\n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KG_PROCESS_ERROR(nRetCode);

    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pRelay->m_nRevGetZoneChargeFlagNotify;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

//2009-10-29 11:53:09
void KGatewaytest::TestGatewayTransmitPaysysReceiveFreezeCoinRequest()   // caseID:
{
    int         nResult             = false;
    int         nRetCode            = false;
    char        szTestCaseDscribe[] = "// caseID: ,测试Paysys正常登陆，GameCenter发来FreezeCoin请求,paysys收到FreezeCoin请求,Gateway是否转发正常";
    unsigned    nLoopTimes          = 300;

    KGLogPrintf(KGLOG_INFO, "caseID: \n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoFreezeCoinRequest();
    KGLOG_PROCESS_ERROR(nRetCode);

    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pPaysys->m_nFreezeCoinRequest;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitRelayReceiveFreezeCoinRespond()   // caseID:
{
    int         nResult             = false;
    int         nRetCode            = false;
    char        szTestCaseDscribe[] = "// caseID: ,测试Paysys正常登陆，GameCenter发来FreezeCoin请求,paysys收到FreezeCoin请求, Relay成功收到FreezeCoin返回,Gateway是否转发正常";
    unsigned    nLoopTimes          = 300;

    KGLogPrintf(KGLOG_INFO, "caseID: \n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoFreezeCoinRequest();
    KGLOG_PROCESS_ERROR(nRetCode);

    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pRelay->m_nRevFreezeCoinRespond;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitPaysysReceiveAccountExchangeRequest()   // caseID:
{
    int         nResult             = false;
    int         nRetCode            = false;
    char        szTestCaseDscribe[] = "// caseID: ,测试Paysys正常登陆，GameCenter发来AccountExchange请求,paysys收到AccountExchange请求,Gateway是否转发正常";
    unsigned    nLoopTimes          = 300;

    KGLogPrintf(KGLOG_INFO, "caseID: \n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoAccountExchangeRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pPaysys->m_nAccountExchangeRequest;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}

void KGatewaytest::TestGatewayTransmitRelayReceiveAccountExchangeRespond()   // caseID:
{
    int         nResult             = false;
    int         nRetCode            = false;
    char        szTestCaseDscribe[] = "// caseID: ,测试Paysys正常登陆，GameCenter发来AccountExchange请求,paysys收到AccountExchange请求, Relay成功收到AccountExchange返回,Gateway是否转发正常";
    unsigned    nLoopTimes          = 300;

    KGLogPrintf(KGLOG_INFO, "caseID: \n");

    m_GatewayTestConfig.ForecastConfig.nTestPoint               = KG_GATEWAY_RECONNECT;
    m_GatewayTestConfig.PaysysManagerConfig.nGatewayLoginResult = ACTION_SUCCESS;
    m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect  = true;
    m_GatewayTestConfig.PaysysManagerConfig.nNotHandleProtocol  = 0;
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 0;
    m_GatewayTestConfig.PaysysManagerConfig.wType               = AccountUserLoginInfo;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode         = PASSPODMODE_UNBIND;//临时

    nRetCode = TestThisCaseInPaysys();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pRelay->DoAccountExchangeRequest();
    KGLOG_PROCESS_ERROR(nRetCode);


    //if (m_GatewayTestConfig.PaysysManagerConfig.nPaysysLostconnect)
    //{
    //    KGThread_Sleep(300);
    //    m_pPaysys->AllDisConnected(false);
    while(true)
    {
        KGThread_Sleep(100);
        nRetCode  = m_pRelay->m_nRevAccountExchangeRespond;
        //KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(nRetCode);
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
        KG_CPPUNIT_ASSERT(nRetCode);
    }
#else
    {
        //InsertInfo(CASEPURPOSE, (szTestCaseDscribe));
        //InsertInfo(PRECONDITION, (""));
        CPPUNIT_ASSERT(nRetCode);
    }
#endif
}
