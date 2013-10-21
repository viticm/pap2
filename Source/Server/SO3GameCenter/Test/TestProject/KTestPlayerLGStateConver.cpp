#include "StdAfx.h"
#include "KGameCenterWrapper.h"
#include "KGameServerSimulator.h"
#include "KGatewaySimulator.h"
#include "TestTools.h"
#include "gtest/gtest.h"

class KTestPlayerLGStateConver : public testing::Test 
{
protected:
	KTestPlayerLGStateConver(void)
{

	m_dwMapIDInGC = 0;
	m_nCreateingMapCopyID = 0;
	m_nNoExistMapCopyID = 0;
	m_nCreatedMapCopyID = 0;
}

~KTestPlayerLGStateConver(void)
{
}

void SetUp()
{
	BOOL bResult = false;
	int  nRetCode = false;

	m_pGameServer = new KGameServerSimulator();

	nRetCode = m_pGameServer->Init();
	ASSERT_TRUE(nRetCode);
    
	m_dwMapIDInGC = m_pGameServer->m_CreatedMapCopy[0].dwMapID;
	m_nCreatedMapCopyID = m_pGameServer->m_CreatedMapCopy[0].nMapCopy;
	m_nNoExistMapCopyID = UINT_MAX; //副本最大ID，不在GC副本初始化列表中


    m_TestRoleInfo = TestTools::CreateRole();
    ASSERT_TRUE(m_TestRoleInfo.dwPlayerID != ERROR_ID);

    strncpy(m_BaseData.szAccount, m_TestRoleInfo.szAccount, sizeof(m_BaseData.szAccount));
    strncpy(m_BaseData.szRoleName, m_TestRoleInfo.szRoleName, sizeof(m_BaseData.szRoleName));
    m_BaseData.dwPlayerID = m_TestRoleInfo.dwPlayerID;
}

void TearDown()
{
	m_pGameServer->UnInit();
	delete m_pGameServer;
	KGThread_Sleep(200);
	TestTools::DeleteRole(&m_TestRoleInfo);
}

BOOL ComparePlayerState(DWORD dwPlayerID, KROLE_STATE eState)
{
	KGThread_Sleep(500); //等待GC改变玩家状态
	return GC->RoleSate(dwPlayerID) == eState;
}

BOOL MakeRoleToCreateMapState(TestTools::KRoleInfo& stRoleInfo, DWORD dwMapID, __int64 nMapCopyID)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

	m_BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
	m_BaseData.BaseInfo.CurrentPos.nMapCopyIndex = nMapCopyID;

	nRetCode = GATEWAY->DoGameLoginRequest(stRoleInfo.szAccount, stRoleInfo.dwPlayerID, m_BaseData);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pGameServer->CheckCreateMapNotify(dwMapID, nMapCopyID);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return  bResult;
}

BOOL MakeRoleToPermitState(TestTools::KRoleInfo& stRoleInfo, DWORD dwMapID, __int64 nMapCopyID)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

	m_BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
	m_BaseData.BaseInfo.CurrentPos.nMapCopyIndex = nMapCopyID;

	nRetCode = GATEWAY->DoGameLoginRequest(stRoleInfo.szAccount, stRoleInfo.dwPlayerID, m_BaseData);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pGameServer->CheckPlayerLoginRequset(stRoleInfo.dwPlayerID, dwMapID, nMapCopyID);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return  bResult;
}

BOOL MakeRoleToLoginState(TestTools::KRoleInfo& stRoleInfo, DWORD dwMapID, __int64 nMapCopyID)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

	MakeRoleToPermitState(stRoleInfo, dwMapID, nMapCopyID);

	nRetCode = ComparePlayerState(stRoleInfo.dwPlayerID, rsLG_WaitForPermit);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pGameServer->DoPlayerLoginRespond(stRoleInfo.dwPlayerID, TRUE);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = GATEWAY->CheckReplyLoginData(stRoleInfo.dwPlayerID, eGameLoginSucceed);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return  bResult;
}
protected:
	KGameServerSimulator* m_pGameServer;
	TestTools::KRoleInfo    m_TestRoleInfo;
	DWORD        m_dwMapIDInGC;

	KROLE_LIST_INFO    m_BaseData;

	__int64      m_nCreatedMapCopyID;
	__int64      m_nCreateingMapCopyID;
	__int64      m_nNoExistMapCopyID;
};

TEST_F(KTestPlayerLGStateConver, TestOfflineToLoginCreatingMap)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    CASE_INFO("测试rsOffline登录createingmap", "玩家状态为 rsOffline");

	KGameServerSimulator GS;
	SCOPEPROTOCOL(&GS, r2s_handshake_respond);
	SCOPEPROTOCOL(&GS, r2s_create_map_notify);

	ASSERT_TRUE(GS.Connect("127.0.0.1", GCSETTING->m_nGameServerPort, GCSETTING->m_nGatewayPingCycle));
	GS.DoHandshakeRequest();

	KGThread_Sleep(500);

	std::list<IKG_Buffer*>& List = GS.GetProtocolData(r2s_create_map_notify);
	ASSERT_TRUE(!List.empty());
	std::list<IKG_Buffer*>::iterator Iter = List.begin();
	R2S_CREATE_MAP_NOTIFY* p = (R2S_CREATE_MAP_NOTIFY*)((*Iter)->GetData());

    m_BaseData.BaseInfo.CurrentPos.dwMapID = p->dwMapID;
    m_BaseData.BaseInfo.CurrentPos.nMapCopyIndex = p->nMapCopyIndex;

	nRetCode = GATEWAY->DoGameLoginRequest(m_TestRoleInfo.szAccount, m_TestRoleInfo.dwPlayerID, m_BaseData);
	ASSERT_TRUE(nRetCode);

    R2G_GAME_LOGIN_RESPOND GameLoginRespond;
    nRetCode = GATEWAY->CheckGameLoginRespond(&GameLoginRespond);
    ASSERT_TRUE(nRetCode);
    ASSERT_TRUE(GameLoginRespond.byCode == (BYTE)eGameLoginQueueWait);
    ASSERT_TRUE(GameLoginRespond.dwRoleID == m_TestRoleInfo.dwPlayerID);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOffline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestOffLineToLgPermit)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    CASE_INFO("测试rsOffline到rsLG_WaitForPermit状态转换", "玩家状态为 rsOffline");

	m_BaseData.BaseInfo.CurrentPos.dwMapID = m_pGameServer->m_CreatedMapCopy[0].dwMapID;
    m_BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServer->m_CreatedMapCopy[0].nMapCopy;

    nRetCode = GATEWAY->DoGameLoginRequest(m_TestRoleInfo.szAccount, m_TestRoleInfo.dwPlayerID, m_BaseData);
    ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->CheckPlayerLoginRequset(m_TestRoleInfo.dwPlayerID, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForPermit);
	ASSERT_TRUE(nRetCode);
}


TEST_F(KTestPlayerLGStateConver, TestLGPremitToOffLine_1)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    RecordProperty("测试rsLG_WaitForPermit到rsOffline状态转换", "玩家状态为 rsLG_WaitForPermit");

	nRetCode = MakeRoleToPermitState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForPermit);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->DoPlayerLeaveGS(m_TestRoleInfo.dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = GATEWAY->CheckPlayerLeaveGame(m_TestRoleInfo.szAccount);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOffline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestLGPremitToOffLine_2)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    RecordProperty("测试rsLG_WaitForPermit到rsOffline状态转换", "玩家状态为 rsLG_WaitForPermit");

	nRetCode = MakeRoleToPermitState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForPermit);
	ASSERT_TRUE(nRetCode);

	m_pGameServer->CloseConnect();

	nRetCode = GATEWAY->CheckPlayerLeaveGame(m_TestRoleInfo.szAccount);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOffline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestLGPremitToOffLine_3)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    RecordProperty("测试rsLG_WaitForPermit到rsOffline状态转换", "玩家状态为 rsLG_WaitForPermit");

	nRetCode = MakeRoleToPermitState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForPermit);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->DoPlayerLoginRespond(m_TestRoleInfo.dwPlayerID, FALSE);
	ASSERT_TRUE(nRetCode);

    nRetCode = GATEWAY->CheckReplyLoginData(m_TestRoleInfo.dwPlayerID, eGameLoginUnknownError);
    ASSERT_TRUE(nRetCode);

	nRetCode = GATEWAY->CheckPlayerLeaveGame(m_TestRoleInfo.szAccount);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOffline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestLGPremitToLGLogin)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    RecordProperty("测试rsLG_WaitForPermit到rsLG_WaitForLogin状态转换", "玩家状态为 rsLG_WaitForLogin");

	nRetCode = MakeRoleToPermitState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForPermit);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->DoPlayerLoginRespond(m_TestRoleInfo.dwPlayerID, TRUE);
	ASSERT_TRUE(nRetCode);

	nRetCode = GATEWAY->CheckReplyLoginData(m_TestRoleInfo.dwPlayerID, eGameLoginSucceed);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForLogin);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestLGLoginToOffLine_1)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

    RecordProperty("测试rsLG_WaitForPermit到rsOffline状态转换", "玩家状态为 rsLG_WaitForLogin");

	nRetCode = MakeRoleToLoginState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForLogin);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->DoPlayerLeaveGS(m_TestRoleInfo.dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = GATEWAY->CheckPlayerLeaveGame(m_TestRoleInfo.szAccount);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOffline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestLGLoginToOffLine_2)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

     RecordProperty("测试rsLG_WaitForPermit到rsOffline状态转换", "玩家状态为 rsLG_WaitForLogin");

	nRetCode = MakeRoleToLoginState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForLogin);
	ASSERT_TRUE(nRetCode);

	m_pGameServer->CloseConnect();

	nRetCode = GATEWAY->CheckPlayerLeaveGame(m_TestRoleInfo.szAccount);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOffline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestPlayerLGStateConver, TestLGLoginToOnLine)
{
	BOOL	  bResult	 = false;
	int		  nRetCode	 = false;

     RecordProperty("测试rsLG_WaitForPermit到rsOnline状态转换", "玩家状态为 rsOnline");

	nRetCode = MakeRoleToLoginState(m_TestRoleInfo, m_dwMapIDInGC, m_nCreatedMapCopyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsLG_WaitForLogin);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->DoConfirmPlayerLoginRequest(m_TestRoleInfo.dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServer->CheckConfirmPlayerLoginRespond(m_TestRoleInfo.dwPlayerID, TRUE);
	ASSERT_TRUE(nRetCode);

	nRetCode = ComparePlayerState(m_TestRoleInfo.dwPlayerID, rsOnline);
	ASSERT_TRUE(nRetCode);
}
