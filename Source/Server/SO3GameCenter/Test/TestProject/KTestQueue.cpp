#include "StdAfx.h"
//#include "StdAfx.h"
//#include "KGameCenterWrapper.h"
//#include "KGameServerSimulator.h"
//#include "KGatewaySimulator.h"
//#include "TestTools.h"
//#include "gtest/gtest.h"
//
//class KTestQueue : public testing::Test 
//{
//protected:
//KTestQueue(void)
//{
//	m_pGameServerA = NULL;
//}
//
//~KTestQueue(void)
//{
//}
//
//void SetUp()
//{
//	BOOL  bResult  = false;
//	int   nRetCode = false;
//
//	m_pGameServerA = new KGameServerSimulator();
//	nRetCode = m_pGameServerA->Init();
//	ASSERT_TRUE(nRetCode);
//
//	for (int i = 0; i < 100; i++)
//	{
//		m_RoleList.push_back(TestTools::CreateRole());
//		
//	}
//	m_TestRoleA    = &m_RoleList[0];
//	m_TestRoleB = &m_RoleList[1];
//}
//
//void TearDown()
//{
//	m_pGameServerA->UnInit();
//	SAFE_DELETE(m_pGameServerA);
//	KGThread_Sleep(200);
//	for (int i = 0; i < m_RoleList.size(); ++i)
//	{
//		TestTools::DeleteRole(&m_RoleList[i]);
//	}
//}
//
//protected:
//	KGameServerSimulator* m_pGameServerA;
//
//	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
//	ROLE_VECTOR      m_RoleList;
//	TestTools::KRoleInfo*       m_TestRoleA;
//	TestTools::KRoleInfo*       m_TestRoleB;
//};
//
//TEST_F(KTestQueue, TestQueueEmpty)
//{
//	int nRetCode = false;
//	int nResult = false;
//
//	RecordProperty("地图人数", "玩家进入地图");
//
//	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, m_TestRoleA,
//		m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
//	ASSERT_TRUE(nRetCode);
//}
//
//
//TEST_F(KTestQueue, TestQueueNotFull)
//{
//	int nRetCode = false;
//	int nResult = false;
//	int nPlayerLimit = 0;
//	DWORD dwMapID = 0;
//
//	RecordProperty("玩家进入地图", "地图人数没有到达排队上限");
//	
//	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
//	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
//
//	for (int i = 0; i < nPlayerLimit; i++)
//	{
//		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
//			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
//		ASSERT_TRUE(nRetCode);
//	}
//}
//
//TEST_F(KTestQueue, TestQueueFull_1)
//{
//	int nRetCode = false;
//	int nResult = false;
//	int nPlayerLimit = 0;
//	DWORD dwMapID = 0;
//	DWORD dwQueueID = 0;
//
//	RecordProperty("玩家进入地图", "地图人数到达排队上限, 在限制登录时间里登录");
//
//	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
//	//nPlayerLimit = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nPreQueuePlayerCount;
//	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
//
//	for (int i = 0; i < nPlayerLimit; i++)
//	{
//		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
//			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
//		ASSERT_TRUE(nRetCode);
//	}
//
//	KROLE_LIST_INFO BaseData;
//
//	strncpy(BaseData.szAccount, m_RoleList[nPlayerLimit].szAccount, sizeof(BaseData.szAccount));
//	strncpy(BaseData.szRoleName, m_RoleList[nPlayerLimit].szRoleName, sizeof(BaseData.szRoleName));
//	BaseData.dwPlayerID = m_RoleList[nPlayerLimit].dwPlayerID;
//	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
//	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy;
//
//	nRetCode = GATEWAY->DoGameLoginRequest(m_RoleList[nPlayerLimit].szAccount, m_RoleList[nPlayerLimit + 1].dwPlayerID, BaseData);
//	ASSERT_TRUE(nRetCode);
//
//	nRetCode = GATEWAY->CheckLoginRespond(eGameLoginQueueWait, m_RoleList[nPlayerLimit].dwPlayerID, dwQueueID);
//	ASSERT_TRUE(nRetCode);
//}
//
//TEST_F(KTestQueue, TestQueueFull_2)
//{
//	int nRetCode = false;
//	int nResult = false;
//	int nPlayerLimit = 0;
//	int nTime;
//	DWORD dwMapID = 0;
//	DWORD dwQueueID = 0;
//
//	RecordProperty("玩家进入地图", "地图人数到达排队上限, 在限制登录时间里登录");
//
//	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
//	//nPlayerLimit = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nPreQueuePlayerCount;
//	//nTime = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nLoginIntervalTime * 1000;
//	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
//
//	for (int i = 0; i < nPlayerLimit; i++)
//	{
//		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
//			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
//		ASSERT_TRUE(nRetCode);
//	}
//
//	KROLE_LIST_INFO BaseData;
//
//	strncpy(BaseData.szAccount, m_RoleList[nPlayerLimit].szAccount, sizeof(BaseData.szAccount));
//	strncpy(BaseData.szRoleName, m_RoleList[nPlayerLimit].szRoleName, sizeof(BaseData.szRoleName));
//	BaseData.dwPlayerID = m_RoleList[nPlayerLimit].dwPlayerID;
//	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
//	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy;
//
//	GATEWAY->DoGameLoginRequest(m_RoleList[nPlayerLimit].szAccount, m_RoleList[nPlayerLimit + 1].dwPlayerID, BaseData);
//
//	nRetCode = GATEWAY->CheckLoginRespond(eGameLoginQueueWait, m_RoleList[nPlayerLimit].dwPlayerID, dwQueueID);
//	ASSERT_TRUE(nRetCode);
//
//	KGThread_Sleep(100);
//
//	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[nPlayerLimit],
//		m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy, dwQueueID);
//	ASSERT_TRUE(nRetCode);
//}
//
//
//TEST_F(KTestQueue, TestQueueFull_3)
//{
//	int nRetCode = false;
//	int nResult = false;
//	int nPlayerLimit = 0;
//	DWORD dwMapID = 0;
//	DWORD dwQueueID = 0;
//
//	RecordProperty("玩家进入地图", "地图人数到达排队上限后人数减少");
//
//	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
//	//nPlayerLimit = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nPreQueuePlayerCount;
//	//nTime = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nLoginIntervalTime * 1000;
//	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
//
//	for (int i = 0; i < nPlayerLimit; i++)
//	{
//		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
//			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
//		ASSERT_TRUE(nRetCode);
//	}
//
//	KROLE_LIST_INFO BaseData;
//
//	strncpy(BaseData.szAccount, m_RoleList[nPlayerLimit].szAccount, sizeof(BaseData.szAccount));
//	strncpy(BaseData.szRoleName, m_RoleList[nPlayerLimit].szRoleName, sizeof(BaseData.szRoleName));
//	BaseData.dwPlayerID = m_RoleList[nPlayerLimit].dwPlayerID;
//	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
//	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy;
//
//	GATEWAY->DoGameLoginRequest(m_RoleList[nPlayerLimit].szAccount, m_RoleList[nPlayerLimit + 1].dwPlayerID, BaseData);
//
//	nRetCode = GATEWAY->CheckLoginRespond(eGameLoginQueueWait, m_RoleList[nPlayerLimit].dwPlayerID, dwQueueID);
//	ASSERT_TRUE(nRetCode);
//
//	nRetCode = TestTools::PlayerLeaveGame(&m_RoleList[0]);
//	ASSERT_TRUE(nRetCode);
//
//	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[nPlayerLimit],
//		m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy, dwQueueID);
//	ASSERT_TRUE(nRetCode);
//}
////TEST_F(KTestQueue, TestQueueEmpty)
////{
////	int nRetCode = false;
////	int nResult = false;
////
////	RecordProperty("地图人数", "玩家进入地图");
////
////	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, m_TestRoleA,
////		m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
////	ASSERT_TRUE(nRetCode);
////}
////
////
////TEST_F(KTestQueue, TestQueueNotFull)
////{
////	int nRetCode = false;
////	int nResult = false;
////	int nPlayerLimit = 0;
////	DWORD dwMapID = 0;
////
////	RecordProperty("玩家进入地图", "地图人数没有到达排队上限");
////	
////	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
////	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
////
////	for (int i = 0; i < nPlayerLimit; i++)
////	{
////		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
////			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
////		ASSERT_TRUE(nRetCode);
////	}
////}
////
////TEST_F(KTestQueue, TestQueueFull_1)
////{
////	int nRetCode = false;
////	int nResult = false;
////	int nPlayerLimit = 0;
////	DWORD dwMapID = 0;
////	DWORD dwQueueID = 0;
////
////	RecordProperty("玩家进入地图", "地图人数到达排队上限, 在限制登录时间里登录");
////
////	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
////	//nPlayerLimit = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nPreQueuePlayerCount;
////	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
////
////	for (int i = 0; i < nPlayerLimit; i++)
////	{
////		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
////			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
////		ASSERT_TRUE(nRetCode);
////	}
////
////	KROLE_LIST_INFO BaseData;
////
////	strncpy(BaseData.szAccount, m_RoleList[nPlayerLimit].szAccount, sizeof(BaseData.szAccount));
////	strncpy(BaseData.szRoleName, m_RoleList[nPlayerLimit].szRoleName, sizeof(BaseData.szRoleName));
////	BaseData.dwPlayerID = m_RoleList[nPlayerLimit].dwPlayerID;
////	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
////	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy;
////
////	nRetCode = GATEWAY->DoGameLoginRequest(m_RoleList[nPlayerLimit].szAccount, m_RoleList[nPlayerLimit + 1].dwPlayerID, BaseData);
////	ASSERT_TRUE(nRetCode);
////
////	nRetCode = GATEWAY->CheckLoginRespond(eGameLoginQueueWait, m_RoleList[nPlayerLimit].dwPlayerID, dwQueueID);
////	ASSERT_TRUE(nRetCode);
////}
////
////TEST_F(KTestQueue, TestQueueFull_2)
////{
////	int nRetCode = false;
////	int nResult = false;
////	int nPlayerLimit = 0;
////	int nTime;
////	DWORD dwMapID = 0;
////	DWORD dwQueueID = 0;
////
////	RecordProperty("玩家进入地图", "地图人数到达排队上限, 在限制登录时间里登录");
////
////	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
////	//nPlayerLimit = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nPreQueuePlayerCount;
////	//nTime = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nLoginIntervalTime * 1000;
////	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
////
////	for (int i = 0; i < nPlayerLimit; i++)
////	{
////		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
////			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
////		ASSERT_TRUE(nRetCode);
////	}
////
////	KROLE_LIST_INFO BaseData;
////
////	strncpy(BaseData.szAccount, m_RoleList[nPlayerLimit].szAccount, sizeof(BaseData.szAccount));
////	strncpy(BaseData.szRoleName, m_RoleList[nPlayerLimit].szRoleName, sizeof(BaseData.szRoleName));
////	BaseData.dwPlayerID = m_RoleList[nPlayerLimit].dwPlayerID;
////	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
////	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy;
////
////	GATEWAY->DoGameLoginRequest(m_RoleList[nPlayerLimit].szAccount, m_RoleList[nPlayerLimit + 1].dwPlayerID, BaseData);
////
////	nRetCode = GATEWAY->CheckLoginRespond(eGameLoginQueueWait, m_RoleList[nPlayerLimit].dwPlayerID, dwQueueID);
////	ASSERT_TRUE(nRetCode);
////
////	KGThread_Sleep(nTime);
////
////	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[nPlayerLimit],
////		m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy, dwQueueID);
////	ASSERT_TRUE(nRetCode);
////}
////
////
////TEST_F(KTestQueue, TestQueueFull_3)
////{
////	int nRetCode = false;
////	int nResult = false;
////	int nPlayerLimit = 0;
////	DWORD dwMapID = 0;
////	DWORD dwQueueID = 0;
////
////	RecordProperty("玩家进入地图", "地图人数到达排队上限后人数减少");
////
////	dwMapID = m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID;
////	//nPlayerLimit = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nPreQueuePlayerCount;
////	//nTime = g_TestSettings.m_MapTable.find(dwMapID)->second.m_nLoginIntervalTime * 1000;
////	ASSERT_TRUE(nPlayerLimit <= m_RoleList.size());
////
////	for (int i = 0; i < nPlayerLimit; i++)
////	{
////		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
////			m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
////		ASSERT_TRUE(nRetCode);
////	}
////
////	KROLE_LIST_INFO BaseData;
////
////	strncpy(BaseData.szAccount, m_RoleList[nPlayerLimit].szAccount, sizeof(BaseData.szAccount));
////	strncpy(BaseData.szRoleName, m_RoleList[nPlayerLimit].szRoleName, sizeof(BaseData.szRoleName));
////	BaseData.dwPlayerID = m_RoleList[nPlayerLimit].dwPlayerID;
////	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
////	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy;
////
////	GATEWAY->DoGameLoginRequest(m_RoleList[nPlayerLimit].szAccount, m_RoleList[nPlayerLimit + 1].dwPlayerID, BaseData);
////
////	nRetCode = GATEWAY->CheckLoginRespond(eGameLoginQueueWait, m_RoleList[nPlayerLimit].dwPlayerID, dwQueueID);
////	ASSERT_TRUE(nRetCode);
////
////	nRetCode = TestTools::PlayerLeaveGame(&m_RoleList[0]);
////	ASSERT_TRUE(nRetCode);
////
////	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[nPlayerLimit],
////		m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy, dwQueueID);
////	ASSERT_TRUE(nRetCode);
////}
