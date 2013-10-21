///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : SO3GatewayTest.h
// Creator  : liuzhibiao
// Date     : 2009-2-12
// Comment  : v0.2
//            2008.08.27	Create0.1
//            v0.2      改变数据结构
//            v0.3      2009-2-12 加入单个角色冻结的测试用例
///////////////////////////////////////////////////////////////

#ifndef _SO3GATEWAYTEST_H_
#define _SO3GATEWAYTEST_H_

#include "KG_SO3GateWayTestDef.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

class KClientController;
class KSimulateClient;
class KSimulatePaysys;
class KSimulateRelay;
class KGateWayMonitor;
class KSO3Gateway;

class KGatewaytest : public CppUnit::TestFixture
{
public:
    int     Init();
    void    UnInit();
public:
    void    setUp(){Init();}
    void    tearDown(){UnInit();}

    CPPUNIT_TEST_SUITE(KGatewaytest);
    // HandShake
    CPPUNIT_TEST(TestHandShakeGatewayVersionNotMatch);  // caseID:4997
    CPPUNIT_TEST(TestHandShakeGameWorldMaintenance);    // caseID:4998
    CPPUNIT_TEST(TestHandShakeAccountSystemLost);       // caseID:4999
    CPPUNIT_TEST(TestHandShakeGameWorldVersionError);   // caseID:5000
    // UserLogin
    CPPUNIT_TEST(TestUserLoginGatewayLockTheAccount);           // caseID:5126
    CPPUNIT_TEST(TestUserLoginButGCNotRespondUserState);        // caseID:6655
    CPPUNIT_TEST(TestUserLoginGCRevKickAccountRequest);         // caseID:6656
    CPPUNIT_TEST(TestUserLoginClientRevRetryLoginInfo);         // caseID:5003
    CPPUNIT_TEST(TestUserLoginCGRetUserOnlineUserReLogin);      // caseID:6664
    CPPUNIT_TEST(TestUserLoginPaysysRefuseRevPWOrAKError);      // caseID:5001
    CPPUNIT_TEST(TestUserLoginPaysysRefuseUserReLogin);         // caseID:6665
    CPPUNIT_TEST(TestUserLoginPaysysNotRespondUserReLogin);     // caseID:6659
    CPPUNIT_TEST(TestUserLoginPaysysNotRespondAndUserNoOnLine); // caseID:5125
    CPPUNIT_TEST(TestUserLoginPaysysNotRespondAndUserOnLine);   // caseID:6660
    // Mibao
    CPPUNIT_TEST(TestUserLoginPaysysNeedMibaoSucceed);          // caseID:24733
    CPPUNIT_TEST(TestUserLoginPaysysNeedMibaoFailed);           // caseID:24734
    CPPUNIT_TEST(TestUserLoginPaysysNeedTokenSucceed);          // caseID:24735
    CPPUNIT_TEST(TestUserLoginPaysysNeedTokenFailed);           // caseID:24736
    CPPUNIT_TEST(TestUserLoginPaysysNeedMiBaoClientDisconnected);                         // caseID:24737
    CPPUNIT_TEST(TestUserLoginPaysysNeedMibaoClientDisConnectedReConnect);                // caseID:24738
    CPPUNIT_TEST(TestUserLoginPaysysNeedTokenClientDisconnectedPaysysNoRespond);          // caseID:24739
    CPPUNIT_TEST(TestUserLoginPaysysNeedTokenClientDisConnectedReConnectPaysysNoRespond); // caseID:24740
    // 
    // GetRoleList
    CPPUNIT_TEST(TestGetRoleListHaveZeroRoles);                 // caseID:5121
    CPPUNIT_TEST(TestGetRoleListHaveTwoRoles);                  // caseID:5122
    CPPUNIT_TEST(TestGetRoleListHaveThreeRoles);                // caseID:5123
    CPPUNIT_TEST(TestUserGetRoleListGCNotRespondUserRelogin);   // caseID:6661
    CPPUNIT_TEST(TestUserGetRoleListGCNotResponUserDisconnect); // caseID:6662
    CPPUNIT_TEST(TestUserGetRoleListUserRelogin);               // caseID:6666
    CPPUNIT_TEST(TestUserGetRoleListUserDisconnect);            // caseID:6663
    // RoleEnterGame
    CPPUNIT_TEST(TestRoleEnterGameSystemMaintenance);                           // caseID:5127
    CPPUNIT_TEST(TestRoleEnterGameUnknownError);                                // caseID:5128
    CPPUNIT_TEST(TestRoleEnterGameOverload);                                    // caseID:6519
    CPPUNIT_TEST(TestRoleEnterGameUserRevQueueWait);                            // caseID:6517
    CPPUNIT_TEST(TestRoleEnterGameUserGCRevLoginRequest);                       // caseID:6667
    CPPUNIT_TEST(TestRoleEnterGameGCNotRespondUserDisconnectPaysyRevLoginOut);  // caseID:5132
    CPPUNIT_TEST(TestRoleEnterGameLoginSucceed);                                // caseID:6668
    CPPUNIT_TEST(TestRoleEnterGameUserDisconnectReLogin);                       // caseID:6669
    CPPUNIT_TEST(TestRoleEnterGameGCNotRespondUserDisconnectReLogin);           // caseID:6670
    CPPUNIT_TEST(TestRoleEnterGameRoleFreeze);                                  // caseID:10992
    // 排队
    CPPUNIT_TEST(TestRoleEnterGameUserDisconnectPaysyRevLoginOut);                      // caseID:6677
    CPPUNIT_TEST(TestRoleEnterGameGCQueueWaitUserDisconnectReLogin);                    // caseID:6678
    CPPUNIT_TEST(TestTwoPlayerRoleEnterGameQueueWaitTheSecondUserGiveup);               // caseID:6518
    CPPUNIT_TEST(TestRoleEnterGameGCFullLoginAGSucceed);                                // caseID:6671
    CPPUNIT_TEST(TestRoleEnterGameGCFullLoginAGSystemMaintenance);                      // caseID:6673
    CPPUNIT_TEST(TestFiveUserRoleEnterGameGCFullLoginAGTheFirstOneSucceed);             // caseID:6674
    CPPUNIT_TEST(TestFiveUserRoleEnterGameGCFullLoginAGTheThirdOneGiveUp);              // caseID:6675
    CPPUNIT_TEST(TestFiveUserRoleEnterGameGCFullLoginAGAllSucceed);                     // caseID:6676
    CPPUNIT_TEST(TestUserRoleEnterGameGCFullLoginAGSucceed);                            // caseID:5131
    CPPUNIT_TEST(TestUserRoleEnterGameGCFullLoginAGGCChangeTheQueue);                   // caseID:6672
    CPPUNIT_TEST(TestRoleEnterGameGCFullLoginAGRoleFreeze);                             // caseID:10993
    CPPUNIT_TEST(TestDisconnectPaysyRevLoginOutRoleEnterGameGCFullLoginAGRoleFreeze);   // caseID:10994
    CPPUNIT_TEST(TestRoleEnterGameGCQueueWaitFreezeRoleUserDisconnectReLogin);          // caseID:10995
    //离队
    CPPUNIT_TEST(TestQueueCountRoleEnterGameGCFullLoginAGSucceed);                  // caseID:6824
    CPPUNIT_TEST(TestQueueCountUserRoleEnterGameGCFullLoginAGGCChangeTheQueue);     // caseID:6825
    CPPUNIT_TEST(TestQueueCountRoleEnterGameGCFullLoginAGSystemMaintenance);        // caseID:6826
    CPPUNIT_TEST(TestQueueCountRoleEnterGameGCFullLoginAGSystemUnknowError);        // caseID:6827
    CPPUNIT_TEST(TestQueueCountRoleEnterGameGCFullLoginAGGCNotRespond);             // caseID:6828
    CPPUNIT_TEST(TestQueueCountTwoPlayerRoleEnterGameQueueWaitTheSecondUserGiveup); //caseID: 6829
    CPPUNIT_TEST(TestQueueCountRoleEnterGameGCFullClientDisconnect);                // caseID:6830
    //// CreateRole
    CPPUNIT_TEST(TestCreateRoleGameCenterRefuse);                   // caseID:5133
    CPPUNIT_TEST(TestCreateRoleHaveZeroRoles);                      // caseID:5134
    CPPUNIT_TEST(TestCreateRoleHaveTwoRoles);                       // caseID:5135
    CPPUNIT_TEST(TestCreateRoleHaveThreeRoles);                     // caseID:5136
    CPPUNIT_TEST(TestClientCreateRoleCGNotRespondUserDisconnect);   // caseID:5137
    CPPUNIT_TEST(TestCreateRoleUserDisconnectReLogin);              // caseID:6679
    // DeleteRole
    CPPUNIT_TEST(TestDelRoleHaveOneRoles);                      // caseID:5138
    CPPUNIT_TEST(TestDelNotUsersRole);                          // caseID:5141
    CPPUNIT_TEST(TestClientDelRoleCGNotRespondUserDisconnect);  // caseID:5142
    CPPUNIT_TEST(TestDelRoleUserDisconnectReLogin);             // caseID:6680
    // GetHometownList
    CPPUNIT_TEST(TestTestGetHometownListHaveZeroMap);               // caseID:5143
    CPPUNIT_TEST(TestTestGetHometownListHaveTwoMap);                // caseID:5144
    CPPUNIT_TEST(TestGetHometownListGCNotRespondUserDisconnect);    // caseID:5145
    CPPUNIT_TEST(TestGetHometownListUserDisconnectReLogin);         // caseID:6681
    // Others
    CPPUNIT_TEST(TestGatewayReConnect);              // caseID:5148
    CPPUNIT_TEST(TestUserNotPingDisconnectRelogin);  // caseID:6831
    // GatewayTransmit
    CPPUNIT_TEST(TestGatewayTransmitPaysysReceiveChangeExtPointRequest);        // caseID:24741
    CPPUNIT_TEST(TestGatewayTransmitRelayReceiveChangeExtPointRespond);         // caseID:24742
    CPPUNIT_TEST(TestGatewayTransmitPaysysReceiveSetChargeFlagRequest);         // caseID:24743
    CPPUNIT_TEST(TestGatewayTransmitRelayReceiveDoSetChargeFlagRespond);        // caseID:24744
    CPPUNIT_TEST(TestGatewayTransmitPaysysReceiveActivePresentCoodeRequest);   // caseID:24745
    CPPUNIT_TEST(TestGatewayTransmitRelayReceiveActivePresentCoodeRespond);     // caseID:24746
    CPPUNIT_TEST(TestGatewayTransmitRelayReceiveGetZoneChargeFlagNotify);       // caseID:24747

    CPPUNIT_TEST(TestGatewayTransmitPaysysReceiveFreezeCoinRequest);
    CPPUNIT_TEST(TestGatewayTransmitRelayReceiveFreezeCoinRespond);
    CPPUNIT_TEST(TestGatewayTransmitPaysysReceiveAccountExchangeRequest);
    CPPUNIT_TEST(TestGatewayTransmitRelayReceiveAccountExchangeRespond);

    CPPUNIT_TEST_SUITE_END();
public:
    // HandShake
    void FillTestHandShakeInfo(
        int nClientSendGatewayVersion,
        int nClientSendGameWorldVersion,
        int nGameWorldLowerVersion,
        int nGameWorldUpperVersion,
        int nPaysysLostconnect,
        int nClientForecastRecv,
        int nNeedStopWhenRevAppointInfo
    );
    void TestHandShakeGatewayVersionNotMatch(); // caseID:4997
    void TestHandShakeGameWorldMaintenance();   // caseID:4998
    void TestHandShakeAccountSystemLost();      // caseID:4999
    void TestHandShakeGameWorldVersionError();  // caseID:5000
    // UserLogin
    void FillTestUserLoginInfo(
        int nTestPoint,
        int nPaysysUserLoginResult, 
        WORD wType,
        BYTE byMibaoMode,
        int nMibaoVerifyResult,
        int nRelayUserOnlineResult, 
        int nClientHadRevKick,
        int nResult,
        int nNeedStopWhenRevAppointInfo
    );
    void TestUserLoginGatewayLockTheAccount();              // caseID:5126
    void TestUserLoginButGCNotRespondUserState();           // caseID:6655
    void TestUserLoginGCRevKickAccountRequest();            // caseID:6656
    void TestUserLoginClientRevRetryLoginInfo();            // caseID:5003
    void TestUserLoginCGRetUserOnlineUserReLogin();         // caseID:6664
    void TestUserLoginPaysysRefuseRevPWOrAKError();         // caseID:5001
    void TestUserLoginPaysysRefuseUserReLogin();            // caseID:6665
    void TestUserLoginPaysysNotRespondUserReLogin();        // caseID:6659
    void TestUserLoginPaysysNotRespondAndUserNoOnLine();    // caseID:5125
    void TestUserLoginPaysysNotRespondAndUserOnLine();      // caseID:6660
    // Mibao
    void TestUserLoginPaysysNeedMibaoSucceed();             // caseID:24733
    void TestUserLoginPaysysNeedMibaoFailed();              // caseID:24734
    void TestUserLoginPaysysNeedTokenSucceed();             // caseID:24735
    void TestUserLoginPaysysNeedTokenFailed();              // caseID:24736
    void TestUserLoginPaysysNeedMiBaoClientDisconnected();  // caseID:24737
    void TestUserLoginPaysysNeedMibaoClientDisConnectedReConnect();                 // caseID:24738
    void TestUserLoginPaysysNeedMibaoPaysysNoRespond();                             // caseID:????
    void TestUserLoginPaysysNeedTokenClientDisconnectedPaysysNoRespond();           // caseID:24739
    void TestUserLoginPaysysNeedTokenClientDisConnectedReConnectPaysysNoRespond();  // caseID:24740
    // GetRoleList
    void FillTestGetRoleListInfo(
        int nRoleCount, 
        int nForeResult,
        int nNeedStopWhenRevAppointInfo
    );
    void TestGetRoleListHaveZeroRoles();                    // caseID:5121
    void TestGetRoleListHaveTwoRoles();                     // caseID:5122
    void TestGetRoleListHaveThreeRoles();                   // caseID:5123
    void TestUserGetRoleListGCNotRespondUserRelogin();      // caseID:6661
    void TestUserGetRoleListGCNotResponUserDisconnect();    // caseID:6662
    void TestUserGetRoleListUserRelogin();                  // caseID:6666
    void TestUserGetRoleListUserDisconnect();               // caseID:6663
    // RoleEnterGame
    void FillTestRoleEnterGameInfo(
        int nRetGameCenterRoleEnterGame,
        int nResult,
        int nNeedStopWhenRevAppointInfo
    );
    void TestRoleEnterGameSystemMaintenance();                              // caseID:5127
    void TestRoleEnterGameOverload();                                       // caseID:6519
    void TestRoleEnterGameUnknownError();                                   // caseID:5128
    void TestRoleEnterGameLoginSucceed();                                   // caseID:6668
    void TestRoleEnterGameUserRevQueueWait();                               // caseID:6517
    void TestRoleEnterGameUserGCRevLoginRequest();                          // caseID:6667
    void TestRoleEnterGameGCNotRespondUserDisconnectPaysyRevLoginOut();     // caseID:5132
    void TestRoleEnterGameUserDisconnectReLogin();                          // caseID:6669
    void TestRoleEnterGameGCNotRespondUserDisconnectReLogin();              // caseID:6670
    void TestRoleEnterGameRoleFreeze();                                     // caseID:10992
    //排队
    void TestRoleEnterGameUserDisconnectPaysyRevLoginOut();                     // caseID:6677
    void TestRoleEnterGameGCQueueWaitUserDisconnectReLogin();                   // caseID:6678
    void TestTwoPlayerRoleEnterGameQueueWaitTheSecondUserGiveup();              // caseID:6518
    void TestRoleEnterGameGCFullLoginAGSucceed();                               // caseID:6671
    void TestRoleEnterGameGCFullLoginAGSystemMaintenance();                     // caseID:6673
    void TestFiveUserRoleEnterGameGCFullLoginAGTheFirstOneSucceed();            // caseID:6674
    void TestFiveUserRoleEnterGameGCFullLoginAGTheThirdOneGiveUp();             // caseID:6675
    void TestFiveUserRoleEnterGameGCFullLoginAGAllSucceed();                    // caseID:6676
    void TestUserRoleEnterGameGCFullLoginAGSucceed();                           // caseID:5131
    void TestUserRoleEnterGameGCFullLoginAGGCChangeTheQueue();                  // caseID:6672
    void TestRoleEnterGameGCFullLoginAGRoleFreeze();                            // caseID:10993
    void TestDisconnectPaysyRevLoginOutRoleEnterGameGCFullLoginAGRoleFreeze();  // caseID:10994
    void TestRoleEnterGameGCQueueWaitFreezeRoleUserDisconnectReLogin();         // caseID:10995
    //离队
    void TestQueueCountRoleEnterGameGCFullLoginAGSucceed();                     // caseID:6824
    void TestQueueCountUserRoleEnterGameGCFullLoginAGGCChangeTheQueue();        // caseID:6825
    void TestQueueCountRoleEnterGameGCFullLoginAGSystemMaintenance();           // caseID:6826
    void TestQueueCountRoleEnterGameGCFullLoginAGSystemUnknowError();           // caseID:6827
    void TestQueueCountRoleEnterGameGCFullLoginAGGCNotRespond();                // caseID:6828
    void TestQueueCountTwoPlayerRoleEnterGameQueueWaitTheSecondUserGiveup();    //caseID: 6829
    void TestQueueCountRoleEnterGameGCFullClientDisconnect();                   // caseID:6830
    // CreateRole
    void FillTestCreateRoleInfo(
        int nRoleCount,
        int nRetGameCenterCreateRole,
        int nResult,
        int nNeedStopWhenRevAppointInfo
    );
    void TestCreateRoleGameCenterRefuse();                  // caseID:5133
    void TestCreateRoleHaveZeroRoles();                     // caseID:5134
    void TestCreateRoleHaveTwoRoles();                      // caseID:5135
    void TestCreateRoleHaveThreeRoles();                    // caseID:5136
    void TestClientCreateRoleCGNotRespondUserDisconnect();  // caseID:5137
    void TestCreateRoleUserDisconnectReLogin();             // caseID:6679
    // DeleteRole
    void FillTestDeleteRoleInfo(
        int nRoleCount,
        int nDelRoleID,
        int nRetGameDeleteCreateRole,
        int nResult,
        int nNeedStopWhenRevAppointInfo
    );
    void TestDelRoleHaveOneRoles();                     // caseID:5138
    void TestDelNotUsersRole();                         // caseID:5141
    void TestClientDelRoleCGNotRespondUserDisconnect(); // caseID:5142
    void TestDelRoleUserDisconnectReLogin();            // caseID:6680
    // GetHometownList
    void FillTestGetHometownListInfo(
        int nHometownCount,
        int nResult,
        int nNeedStopWhenRevAppointInfo
    );
    void TestTestGetHometownListHaveZeroMap();              // caseID:5143
    void TestTestGetHometownListHaveTwoMap();               // caseID:5144
    void TestGetHometownListGCNotRespondUserDisconnect();   // caseID:5145
    void TestGetHometownListUserDisconnectReLogin();        // caseID:6681
    // Others
    void TestGatewayReConnect();                // caseID:5148
    void TestUserNotPingDisconnectRelogin();    // caseID:6831
    // GatewayTransmit
    void TestGatewayTransmitPaysysReceiveChangeExtPointRequest();       // caseID:24741
    void TestGatewayTransmitRelayReceiveChangeExtPointRespond();        // caseID:24742
    void TestGatewayTransmitPaysysReceiveSetChargeFlagRequest();        // caseID:24743
    void TestGatewayTransmitRelayReceiveDoSetChargeFlagRespond();       // caseID:24744
    void TestGatewayTransmitPaysysReceiveActivePresentCoodeRequest();   // caseID:24745
    void TestGatewayTransmitRelayReceiveActivePresentCoodeRespond();    // caseID:24746
    void TestGatewayTransmitRelayReceiveGetZoneChargeFlagNotify();      // caseID:24747

    void TestGatewayTransmitPaysysReceiveFreezeCoinRequest();
    void TestGatewayTransmitRelayReceiveFreezeCoinRespond();
    void TestGatewayTransmitPaysysReceiveAccountExchangeRequest();
    void TestGatewayTransmitRelayReceiveAccountExchangeRespond();

private:
    int  InitTestEnvironment(int nNeedPaysys, int nNeedRelay, int nNeedGateway, int nClinetCount, int nTestPoint);
    void UnInitTestEnvironment();
    int  TestThisCaseInPaysys();
    void Idle(int nLoodTimes);

private:
    int  FillConnectConfig(const char cszConnectConfigFileName[]);

private:
    static void GatewayWorkThreadFunction(void *pvParam);
    int GatewayThreadFunction();

private:
    int                             m_nClientControllerInitFlag;
    int                             m_nPaysysInitFlag;
    int                             m_nRelayInitFlag;
    int                             m_nGatewayInitFlag;
    int                             m_nGatewayExitFlag;
    int                             m_nGatewayMonitorInitFlag;
    KG_RETURN_RESULT                m_ReturnResult;
    KClientController*              m_pClientController;
    KSimulatePaysys*                m_pPaysys;
    KSimulateRelay*                 m_pRelay;
    KSO3Gateway*                    m_pGateway;
    KGateWayMonitor*                m_pGatewayMonitor;
    KG_GATEWAY_TEST_MANAGER         m_GatewayTestConfig;
    int                             m_nHadReadTheConfig;

    KThread                         m_GatewayWorkThread;
    int                             m_nGatewayWorkThreadFlag;
    #ifdef WIN32
    PROCESS_INFORMATION             m_GWprocInfo;//Result
    #else
    int                             m_nGWpid;
    #endif
};

#endif  // _SO3GATEWAYTEST_H_

