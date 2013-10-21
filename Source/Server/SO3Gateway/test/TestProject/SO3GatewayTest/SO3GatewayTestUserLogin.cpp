///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   SO3GatewayTestUserLogin.cpp
// Creator  :   liuzhibiao
// Date     :   2008-9-22
// Comment  :   v0.1
//              v0.1 2008-9-22 create
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SO3GatewayTest.h"
#include "KClientController.h"
#include "KSimulateRelay.h"
#include "KSimulatePaysys.h"

void KGatewaytest::FillTestUserLoginInfo(
    int nTestPoint,
    int nPaysysUserLoginResult, 
    WORD wType,
    BYTE byMibaoMode,
    int nMibaoVerifyResult,
    int nRelayUserOnlineResult, 
    int nClientHadRevKick,
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
    m_GatewayTestConfig.PaysysManagerConfig.nMibaoResult                = nMibaoVerifyResult;
    m_GatewayTestConfig.ClientConfig.nNeedTestTimeOut                   = false;
    m_GatewayTestConfig.ClientConfig.nNeedStopWhenRevAppointInfo        = nNeedStopWhenRevAppointInfo;
    m_GatewayTestConfig.ClientConfig.nNeedSendPing                      = true;

    m_GatewayTestConfig.PaysysManagerConfig.nUserLoginResult = nPaysysUserLoginResult;
    m_GatewayTestConfig.GameCenterConfig.nUserOnlineResult   = nRelayUserOnlineResult;
    m_GatewayTestConfig.ClientConfig.nHadRevKick             = nClientHadRevKick;

    m_GatewayTestConfig.PaysysManagerConfig.wType            = wType;//临时
    m_GatewayTestConfig.PaysysManagerConfig.byMibaoMode      = byMibaoMode;//临时

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
    m_GatewayTestConfig.ForecastConfig.nTestPoint = nTestPoint;
    m_GatewayTestConfig.ForecastConfig.nResult = nResult;
    m_GatewayTestConfig.ForecastConfig.nTypeResult = 0;    

    //[State]
    m_ReturnResult.nTestPoint = m_GatewayTestConfig.ForecastConfig.nTestPoint;
    m_ReturnResult.nResult = 0;
    m_ReturnResult.nTypeResult = 0;
}

void KGatewaytest::TestUserLoginPaysysRefuseRevPWOrAKError()   // caseID:5001
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5001,测试Paysys返回登陆不成功时，Client玩家收到账号密码不匹配";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5001\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, E_ACCOUNT_OR_PASSWORD, AccountUserLoginInfo,  PASSPODMODE_UNBIND, ACTION_SUCCESS, false, false, E_ACCOUNT_OR_PASSWORD, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
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

void KGatewaytest::TestUserLoginGatewayLockTheAccount() // caseID:5126
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5126,账号登陆,Paysys验证通过,但该帐户在另外的地方正在登陆,Client玩家收到账号已锁定";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5126\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_UNBIND, ACTION_SUCCESS, false, true, USER_ACCOUNT_LOCKED, false);

    nRetCode = InitTestEnvironment(true, true, true, 2, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->ModifyClientUserName(1,"0");
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
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

void KGatewaytest::TestUserLoginButGCNotRespondUserState()  // caseID:6655
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6655,测试Paysys正常登陆,但GameCenter不返回状态信息时,Client玩家断线之后重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6655\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, false, true, USER_ACCOUNT_LOCKED, false);
    m_GatewayTestConfig.GameCenterConfig.nNotRespondProtocol      = g2r_query_account_state;

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        7, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
        onDisConnect,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->CheckTheAccountLockedOrNot(0, &nAccountLockedState);
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

void KGatewaytest::TestUserLoginGCRevKickAccountRequest()   // caseID:6656
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6656,测试Paysys正常登陆时，GameCenter返回Client玩家在线时,GameCenter收到踢号信息";
    int nGCHadRevKickInfo = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6656\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, true, true, USER_ACCOUNT_LOCKED, false);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    nGCHadRevKickInfo = m_pRelay->HadRevKickInfo();
    KGLOG_PROCESS_ERROR(nGCHadRevKickInfo);

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

void KGatewaytest::TestUserLoginClientRevRetryLoginInfo()   // caseID:5003
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:5003, 测试Paysys正常登陆时，GameCenter返回Client玩家在线时,Client玩家收到请重试信息";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:5003\n");

    FillTestUserLoginInfo(KG_USER_RETRY, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, true, true, CLIENT_HAD_KICK, false);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify
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

void KGatewaytest::TestUserLoginCGRetUserOnlineUserReLogin()  // caseID:6664
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6664, 测试Paysys正常登陆时，GameCenter返回Client玩家在线时,Client玩家端断线之后重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6664\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, false, true, USER_ACCOUNT_LOCKED, false);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        7, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
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

void KGatewaytest::TestUserLoginPaysysRefuseUserReLogin()  // caseID:6665
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6665, 测试Paysys返回登陆不成功时，Client玩家断线后重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6665\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, E_ACCOUNT_OR_PASSWORD, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, false, true, USER_ACCOUNT_LOCKED, false);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        7, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
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

void KGatewaytest::TestUserLoginPaysysNotRespondUserReLogin()  // caseID:6659
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6659,测试Paysys但不返回用户验证结果时,Client玩家断线,重登,Client玩家没有收到锁号信息(表示没被Gateway锁号)";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:6659\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, E_ACCOUNT_OR_PASSWORD, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, false, true, USER_ACCOUNT_LOCKED, false);
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 62; // B2P_IB_PLAYER_IDENTITY_VERIFY = 62

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        7, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
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

void KGatewaytest::TestUserLoginPaysysNotRespondAndUserNoOnLine()  // caseID:5125
{
    int         nResult             = false;
    int         nRetCode            = false;
    char        szTestCaseDscribe[] = "// caseID:5125,测试Paysys正常登陆但不返回验证结果,GameCenter返回用户不在线时,Client玩家断线,Paysys收到登出信息";
    int         HadRevUserLoginOut  = false;
    unsigned    nLoopTimes          = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:5125\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, E_ACCOUNT_OR_PASSWORD, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, false, true, USER_ACCOUNT_LOCKED, true);
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 62; // B2P_IB_PLAYER_IDENTITY_VERIFY = 62

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
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

void KGatewaytest::TestUserLoginPaysysNotRespondAndUserOnLine()  // caseID:6660
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:6660,测试Paysys正常登陆但不返回验证结果,GameCenter返回用户在线时,Client玩家断线,Paysys收到登出信息";
    int HadRevUserLoginOut = false;
    unsigned    nLoopTimes            = 100;

    KGLogPrintf(KGLOG_INFO, "caseID:6660\n");

    FillTestUserLoginInfo(KG_USER_LOGIN, E_ACCOUNT_OR_PASSWORD, AccountUserLoginInfo,  PASSPODMODE_UNBIND,  ACTION_SUCCESS, true, true, USER_ACCOUNT_LOCKED, false);
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondProtocol = 62; // B2P_IB_PLAYER_IDENTITY_VERIFY = 62

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        4, 
        onConnectToGateway,
        onHandShake,
        onAccountVerify,
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
        KG_PROCESS_SUCCESS(!HadRevUserLoginOut);

        nLoopTimes--;
        KG_PROCESS_ERROR(nLoopTimes);
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

void KGatewaytest::TestUserLoginPaysysNeedMibaoFailed()   // caseID:24734
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24734,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了密保卡,输入密保卡密码错误,玩家收到密码错误提示";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:24734\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPMATRIX,  ACTION_FAILED, false, false, ACTION_FAILED, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    //nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPMATRIX, ACTION_FAILED);
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 1, onMibaoVerify);
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

void KGatewaytest::TestUserLoginPaysysNeedMibaoSucceed()   // caseID:24733
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24733,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了密保卡,输入密保卡密码正确,玩家收到密码正确提示";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID24733:\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPMATRIX, ACTION_SUCCESS, false, false, ACTION_SUCCESS, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    //nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPMATRIX, ACTION_SUCCESS);
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 1, onMibaoVerify);
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

void KGatewaytest::TestUserLoginPaysysNeedMibaoPaysysNoRespond()   // caseID:
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID????:,测试Paysys返回登陆账号密码正确,需要密保验证时,客户端已经输入密保矩阵,但paysys不返回验证结果，Client玩家收到密保密码错误";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID????:\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPMATRIX, ACTION_SUCCESS, false, false, ACTION_FAILED, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);
    m_GatewayTestConfig.PaysysManagerConfig.nNotRespondSubProtocol = AccountUserPasspodVerify;

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPMATRIX, ACTION_FAILED);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 1, onMibaoVerify);
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

void KGatewaytest::TestUserLoginPaysysNeedTokenFailed()   // caseID:24736
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24736,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了令牌密保,输入令牌密码错误,玩家收到密码错误提示";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:24736\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPTOKEN, ACTION_FAILED, false, false, ACTION_FAILED, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    //nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPTOKEN, ACTION_FAILED);
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 1, onMibaoVerify);
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

void KGatewaytest::TestUserLoginPaysysNeedTokenSucceed()   // caseID:24735
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24735:,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了令牌密保,输入令牌密码正确,玩家收到密码正确提示";
    KG_RETURN_RESULT Result;

    KGLogPrintf(KGLOG_INFO, "caseID:24735\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPTOKEN, ACTION_SUCCESS, false, false, ACTION_SUCCESS, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
        );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    //nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPTOKEN, ACTION_SUCCESS);
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 1, onMibaoVerify);
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

void KGatewaytest::TestUserLoginPaysysNeedMiBaoClientDisconnected()   // caseID:24737
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24737:,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了密保卡,但玩家不输入密保密码就断线";
    unsigned    nLoopTimes            = 100;
    int HadRevUserLoginOut = false;

    KGLogPrintf(KGLOG_INFO, "caseID:24737\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPMATRIX, ACTION_SUCCESS, false, false, ACTION_SUCCESS, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    //nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPMATRIX, ACTION_SUCCESS);
    //KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 1, onDisConnect);
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

void KGatewaytest::TestUserLoginPaysysNeedMibaoClientDisConnectedReConnect()   // caseID:24738
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24738,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了密保卡,但玩家不输入密保密码就断线,重登";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:24738\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPMATRIX, ACTION_SUCCESS, false, false, ACTION_SUCCESS, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onDisConnect
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    //nRetCode = m_pPaysys->SetAccountVerifyRespondType(AccountUserPasspodVerifyRet, PASSPODMODE_ZPMATRIX, ACTION_SUCCESS);
    //KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);
    nRetCode = m_pClientController->AddOperateToClient(0, 4, onDisConnect, onConnectToGateway, onHandShake,onAccountVerify);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

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

void KGatewaytest::TestUserLoginPaysysNeedTokenClientDisconnectedPaysysNoRespond()   // caseID:24739
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24739:,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了令牌密保,但玩家输入密保密码,但paysys不返回验证结果,玩家断线";
    unsigned    nLoopTimes   = 100;
    int HadRevUserLoginOut = false;

    KGLogPrintf(KGLOG_INFO, "caseID:24739\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPTOKEN, ACTION_SUCCESS, false, false, ACTION_SUCCESS, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pPaysys->SetNotHandleProtocol(74);//B2P_PLAYER_PASSPOD_VERIFY = 74
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 2, onMibaoVerify, onDisConnect);
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

void KGatewaytest::TestUserLoginPaysysNeedTokenClientDisConnectedReConnectPaysysNoRespond()   // caseID:24740
{
    int nResult  = false;
    int nRetCode = false;
    char szTestCaseDscribe[] = "// caseID:24740,测试Paysys正常登陆，GameCenter返回Client玩家不在线,玩家设置了令牌密保,但玩家输入密保密码,但paysys不返回验证结果,玩家断线重登";
    int nAccountLockedState = false;

    KGLogPrintf(KGLOG_INFO, "caseID:24740\n");

    FillTestUserLoginInfo(KG_USER_MIBAO, ACTION_SUCCESS, AccountUserLoginInfo,  PASSPODMODE_ZPTOKEN, ACTION_SUCCESS, false, false, ACTION_SUCCESS, true);

    nRetCode = InitTestEnvironment(true, true, true, 1, m_GatewayTestConfig.ForecastConfig.nTestPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->FillOperate(
        3,
        onConnectToGateway,
        onHandShake,
        onAccountVerify
        //onMibaoVerify
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pClientController->Run();
    KGLOG_PROCESS_ERROR(nRetCode);

    Idle(70);
    nRetCode = m_pPaysys->SetNotHandleProtocol(74);//B2P_PLAYER_PASSPOD_VERIFY = 74
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_pClientController->AddOperateToClient(0, 5, onMibaoVerify, onDisConnect, onConnectToGateway, onHandShake, onAccountVerify);
    KGLOG_PROCESS_ERROR(nRetCode);
    Idle(70);

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
