#include "StdAfx.h"
#include "KTestDBManager.h"
#include "KGameCenterWrapper.h"
#include "KGameServerSimulator.h"
#include "TestTools.h"
#include "gtest/gtest.h"

class KTestFellowShip : public testing::Test 
{
protected:
	KTestFellowShip(void)
{
    m_pGameServerA = NULL;
    m_pGameServerB = NULL;
}

~KTestFellowShip(void)
{
}

void SetUp()
{
    BOOL  bResult  = false;
    int   nRetCode = false;

    m_pGameServerA = new KGameServerSimulator();
    nRetCode = m_pGameServerA->Init();
    ASSERT_TRUE(nRetCode);

    m_pGameServerB = new KGameServerSimulator();
    nRetCode = m_pGameServerB->Init();
    ASSERT_TRUE(nRetCode);

	for(int i = 0; i < 10 ; i++)
	{
		TestTools::KRoleInfo& RoleInfo = TestTools::CreateRole();
		ASSERT_TRUE(RoleInfo.dwPlayerID != ERROR_ID);
		m_RoleList.push_back(RoleInfo);
	}

	m_TestRoleA = &m_RoleList[0];
	m_TestRoleB = &m_RoleList[1];

    nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, m_TestRoleA,
        m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
    ASSERT_TRUE(nRetCode);

    nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, m_TestRoleB,
        m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
    ASSERT_TRUE(nRetCode);
}

void TearDown()
{
    SAFE_DELETE(m_pGameServerA);
    SAFE_DELETE(m_pGameServerB);
    KGThread_Sleep(200);
    for (int i = 0 ; i < m_RoleList.size(); i++)
    {
		TestTools::DeleteRole(&m_RoleList[i]);
    }
}
	void ClearSystemData()
	{

	}
protected:
	KGameServerSimulator* m_pGameServerA;
	KGameServerSimulator* m_pGameServerB;

	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
	ROLE_VECTOR      m_RoleList;

	TestTools::KRoleInfo*       m_TestRoleA;
	TestTools::KRoleInfo*       m_TestRoleB;
};

TEST_F(KTestFellowShip, TestApplyFellowData_1)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;

    RecordProperty("系统和数据数据库都不存在好友数据", "");

    nRetCode = m_pGameServerA->DoApplyFellowshipDataRequest(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckApplyFellowshipDataRespond(m_TestRoleA->dwPlayerID, NULL, 0);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestApplyFellowData_2)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    char szTestbuff[] = "testbuffer";

    RecordProperty("系统不存在好友数据,数据数据库存在", "");

    nRetCode = g_DBManager.UpDateFellowShipData(m_TestRoleA->dwPlayerID, 
        (BYTE*)szTestbuff, strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->DoApplyFellowshipDataRequest(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckApplyFellowshipDataRespond(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestApplyFellowData_3)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    char szTestbuff[] = "testbuffer";

    RecordProperty("系统不存在好友数据,数据数据库存在", "");

    nRetCode = m_pGameServerA->DoUpdateFellowshipData(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->DoApplyFellowshipDataRequest(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckApplyFellowshipDataRespond(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestUpdateFellowShip_1)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    char szTestbuff[] = "testbuffer";

    RecordProperty("系统不存在好友数据,更新好友数据", "");

    nRetCode = m_pGameServerA->DoUpdateFellowshipData(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->DoApplyFellowshipDataRequest(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckApplyFellowshipDataRespond(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestUpdateFellowShip_2)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    char szTestbuff[] = "testbuffer";

    RecordProperty("系统存在好友数据,更新好友数据", "");

    nRetCode = m_pGameServerA->DoUpdateFellowshipData(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->DoUpdateFellowshipData(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->DoApplyFellowshipDataRequest(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckApplyFellowshipDataRespond(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestSaveFellowShipData_1)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    char szTestbuff[] = "testbuffer";

    RecordProperty("系统存在好友数据,保存好友数据", "");

    nRetCode = m_pGameServerA->DoUpdateFellowshipData(m_TestRoleA->dwPlayerID, (BYTE*)szTestbuff, (unsigned)strlen(szTestbuff));
    ASSERT_TRUE(nRetCode);

	GC->SaveFellowShipData(m_TestRoleA->dwPlayerID);

    ClearSystemData();

    nRetCode = m_pGameServerA->DoApplyFellowshipDataRequest(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestSaveFellowShipData_2)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    char szTestbuff[] = "testbuffer";

    RecordProperty("系统存在好友数据,保存好友数据", "");

    ClearSystemData();
	GC->SaveFellowShipData(m_TestRoleA->dwPlayerID);

    ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestFellowShip, TestSaveFellowShipData_3)
{
	RecordProperty("保存错误玩家数据", "");

	GC->SaveFellowShipData(ERROR_ID);
}


TEST_F(KTestFellowShip, TestAddFellowship)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;

    RecordProperty("添加好友", "");

    nRetCode = m_pGameServerA->DoAddFellowshipRequest(m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckAddFellowshipRespond(m_TestRoleA->dwPlayerID, m_TestRoleB->dwPlayerID);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestGetFellowshipName)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;
    DWORD dwPlayerID[7];
    int ArraySize = sizeof dwPlayerID / sizeof dwPlayerID[0];

    RecordProperty("添加好友", "");

    for (int i = 2; i < 4 ; i++)
    {
        nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
            m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
        ASSERT_TRUE(nRetCode);
    }

    for (int i = 0; i < ArraySize ; i++)
    {
        dwPlayerID[i] = m_RoleList[i].dwPlayerID;
    }

    nRetCode = m_pGameServerA->DoGetFellowshipNameRequest(m_TestRoleA->dwPlayerID, ArraySize, dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckGetFellowshipNameRespond(m_TestRoleA->dwPlayerID, ArraySize, dwPlayerID);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestApplyFellowshipPlayerLevel_1)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;

    RecordProperty("Apply Fellowship Player Level", "Error player ID ");

    nRetCode = m_pGameServerA->DoApplyFellowshipPlayerLevel(ERROR_ID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckSyncFellowshipPlayerLevel(ERROR_ID, 0);
    ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestFellowShip, TestApplyFellowshipPlayerLevel_2)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;

    RecordProperty("Apply Fellowship Player Level", "right player ID");

    nRetCode = m_pGameServerA->DoApplyFellowshipPlayerLevel(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckSyncFellowshipPlayerLevel(m_TestRoleA->dwPlayerID, 1);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFellowShip, TestSyncFellowshipPlayerLevel_1)
{
    BOOL bResult  = false;
    BOOL nRetCode = false;

    RecordProperty("Sync Fellowship Player Level", "Error player ID ");

    nRetCode = m_pGameServerA->DoApplyFellowshipPlayerLevel(ERROR_ID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckSyncFellowshipPlayerLevel(ERROR_ID, 0);
    ASSERT_TRUE(!nRetCode);
}