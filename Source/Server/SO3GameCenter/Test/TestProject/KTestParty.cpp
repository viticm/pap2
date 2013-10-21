#include "stdafx.h"

#include <gtest/gtest.h>

#include "KGatewaySimulator.h"
#include "KGameServerSimulator.h"
#include "TestTools.h"
#include "KGameCenterWrapper.h"
#include "KGatewaySimulator.h"

class KTestParty : public testing::Test 
{
protected:
	KTestParty(void)
	{
		m_pGameServerA = NULL;
		m_pGameServerB = NULL;

		strcpy(m_szNoExistRole, "12312testt");
	}

	KTestParty::~KTestParty(void)
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

		for (int i = 0; i < 10; i++)
		{
			TestTools::KRoleInfo& RoleInfo = TestTools::CreateRole();
			ASSERT_TRUE(RoleInfo.dwPlayerID != ERROR_ID);
			m_RoleList.push_back(RoleInfo);
		}

		m_TestRoleA    = &m_RoleList[0];
		m_TestRoleB    = &m_RoleList[1];
		m_TestRoleC    = &m_RoleList[2];
		m_PartyALeader = &m_RoleList[9];
		m_PartyAMeberA = &m_RoleList[4];
		m_PartyAMeberB = &m_RoleList[3];

		for(size_t i = 0; i < m_RoleList.size() / 2; i++)
		{
			nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
				m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
			ASSERT_TRUE(nRetCode);
		}

		for(size_t i = m_RoleList.size() / 2; i < m_RoleList.size(); i++)
		{
			nRetCode = TestTools::PlayerEnterGame(m_pGameServerB, &m_RoleList[i],
				m_pGameServerB->m_CreatedMapCopy[0].dwMapID, m_pGameServerB->m_CreatedMapCopy[0].nMapCopy);
			ASSERT_TRUE(nRetCode);
		}

		nRetCode = CreateParty(m_PartyALeader, m_PartyAMeberA);
		ASSERT_TRUE(nRetCode);

		nRetCode = PlayerApplyJoinParty(m_PartyAMeberB, m_PartyALeader);
		ASSERT_TRUE(nRetCode);
	}

	void TearDown()
	{
		SAFE_DELETE(m_pGameServerA);
		SAFE_DELETE(m_pGameServerB);
		KGThread_Sleep(300);
		for (int i = 0; i < m_RoleList.size(); i++)
		{
			TestTools::DeleteRole(&m_RoleList[i]);
		}
	}

protected:
	KGameServerSimulator* m_pGameServerA;
	KGameServerSimulator* m_pGameServerB;


	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
	ROLE_VECTOR      m_RoleList;

	TestTools::KRoleInfo*       m_TestRoleA;
	TestTools::KRoleInfo*       m_TestRoleB;
	TestTools::KRoleInfo*       m_TestRoleC;
	TestTools::KRoleInfo*       m_PartyALeader;
	TestTools::KRoleInfo*       m_PartyAMeberA;
	TestTools::KRoleInfo*       m_PartyAMeberB;
	char             m_szNoExistRole[_NAME_LEN];
};


TEST_F(KTestParty, TestCreatePartyFail_1)
{
	BOOL  bResult		= false;
	int   nRetCode		= false;
	DWORD dwPartyID		= MAXDWORD;

	RecordProperty("玩家A邀请自己组队", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_pGameServerA->DoInvitePlayerJoinPartyRequest(m_TestRoleA->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(300);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestCreatePartyFail_2)
{
	BOOL  bResult		= false;
	int   nRetCode		= false;
	DWORD dwPartyID		= MAXDWORD;

	RecordProperty("玩家A邀请不存在角色组队", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_pGameServerA->DoInvitePlayerJoinPartyRequest(m_TestRoleA->dwPlayerID, m_szNoExistRole);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyMessageNotify(m_TestRoleA->dwPlayerID, epncPlayerInviteNotExist);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

}

TEST_F(KTestParty, TestCreatePartyFail_3)
{
	BOOL  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队,B在队伍中", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestCreatePartyFail_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("玩家A向玩家B邀请组队,B不在线", "");

	nRetCode  = TestTools::PlayerLeaveGame(m_TestRoleB);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncPlayerNotOnline);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);


}

TEST_F(KTestParty, TestCreatePartyFail_5)
{
	BOOL  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B重复邀请组队", "");

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_PartyALeader->szRoleName, m_TestRoleB->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncPlayerIsBusy);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

}

TEST_F(KTestParty, TestCreatePartyFail_6)
{
	BOOL  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();
	KGameServerSimulator* pGS = m_TestRoleA->pLoginOnGS;

	RecordProperty("玩家A向玩家B邀请组队后，A离开游戏", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	nRetCode = pGS->DoInvitePlayerJoinPartyRespond(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(500);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

}

TEST_F(KTestParty, TestCreatePartyFail_7)
{
	BOOL  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，玩家B处于组队状态", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = PlayerApplyJoinParty(m_TestRoleB, m_PartyALeader);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(500);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

}

TEST_F(KTestParty, TestCreatePartyFail_8)
{
	BOOL  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，玩家B不同意", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, FALSE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncInvitationDenied);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestCreatePartySuccess)
{
	BOOL  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	DWORD dwCreatedPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，玩家B同意", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_TestRoleA->szRoleName, m_TestRoleB->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	SCOPEPROTOCOL(m_pGameServerA, r2s_team_create_notify);
	SCOPEPROTOCOL(m_pGameServerA, r2s_team_add_member_notify);

	nRetCode = m_pGameServerA->CheckPartyCreateNotify(&dwCreatedPartyID, m_TestRoleA->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);


	ASSERT_TRUE(GC->GetRolePartyID(m_TestRoleA->dwPlayerID) == dwCreatedPartyID);
	ASSERT_TRUE(GC->GetRolePartyID(m_TestRoleB->dwPlayerID) == dwCreatedPartyID);
}

TEST_F(KTestParty, TestPartyLeaderChange_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("队长把称号移交给队员", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoPartyChangeAuthorityRequest(
		m_PartyALeader->dwPlayerID, tatLeader,m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckPartyChangeAuthorityNotify(m_PartyALeader->dwPartyID, tatLeader, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestPartyLeaderChange_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	DWORD dwNewLeaderID = 0;

	RecordProperty("队长下线", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyALeader);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(m_PartyAMeberA->dwPartyID, tatLeader, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwNewLeaderID = GC->GetPartyLeaderID(m_PartyAMeberA->dwPartyID);
	ASSERT_TRUE(dwNewLeaderID == m_PartyAMeberA->dwPlayerID);
}

TEST_F(KTestParty, TestPartyLeaderChange_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	DWORD dwLeaderID = 0;

	RecordProperty("队伍中只有队长在线,队长下线", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyAMeberA);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyAMeberB);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyALeader);
	ASSERT_TRUE(nRetCode);

	dwLeaderID = GC->GetPartyLeaderID(m_PartyALeader->dwPartyID);
	ASSERT_TRUE(dwLeaderID == 0);
}

TEST_F(KTestParty, TestPartyLeaderChange_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	DWORD dwLeaderID = 0;

	RecordProperty("队伍中所有队员都不在线,一个队员上线", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyAMeberA);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyAMeberB);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyALeader);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, m_PartyAMeberA,
		m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
	ASSERT_TRUE(nRetCode);

	dwLeaderID = GC->GetPartyLeaderID(m_PartyAMeberA->dwPartyID);
	ASSERT_TRUE(dwLeaderID == m_PartyAMeberA->dwPlayerID);
}

TEST_F(KTestParty, TestPartyLeaderChange_5)
{
	BOOL  bResult    = false;
	int   nRetCode   = false;
	DWORD dwPartyID  = 0;
	DWORD dwLeaderID = 0;

	RecordProperty("队长把称号移交给玩家B", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoPartyChangeAuthorityRequest(
		m_PartyALeader->dwPlayerID, tatLeader, m_TestRoleB->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(200);

	dwLeaderID = GC->GetPartyLeaderID(m_PartyALeader->dwPartyID);
	ASSERT_TRUE(dwLeaderID == m_PartyALeader->dwPlayerID);
}

TEST_F(KTestParty, TestPartyDisband_1)
{
	BOOL  bResult    = false;
	int   nRetCode   = false;
	DWORD dwPartyID  = 0;

	RecordProperty("队伍里只有个队员,其中一个离队", "");

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberB->dwPartyID, m_PartyAMeberB->dwPlayerID, m_PartyAMeberB->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberA->dwPartyID, m_PartyAMeberA->dwPlayerID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDisbandNotify(m_PartyAMeberA->dwPartyID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestPartyDisband_2)
{
	BOOL  bResult    = false;
	int   nRetCode   = false;
	DWORD dwPartyID  = 0;

	RecordProperty("队伍里只有个队员,队长把一个队员踢出队伍", "");

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberB->dwPartyID, m_PartyAMeberB->dwPlayerID, m_PartyAMeberB->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberA->dwPartyID, m_PartyALeader->dwPlayerID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDisbandNotify(m_PartyAMeberA->dwPartyID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestPartyDisband_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("队长解散队伍", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDisbandRequest(m_PartyALeader->dwPlayerID, m_PartyALeader->dwPartyID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDisbandNotify(m_PartyAMeberA->dwPartyID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestPlayerLeaveParty_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("队员申请离开队伍", "");

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberA->dwPartyID, m_PartyAMeberA->dwPlayerID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDelMemberNotify(m_PartyAMeberA->dwPartyID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestPlayerLeaveParty_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("队长把一个队员踢出队伍", "");

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberA->dwPartyID, m_PartyALeader->dwPlayerID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDelMemberNotify(m_PartyAMeberA->dwPartyID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestPlayerLeaveParty_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwNewLeader = 0 ;
	DWORD dwPartyID = 0;

	RecordProperty("队长申请离开队伍", "");

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyAMeberA->pLoginOnGS->DoPartyDelMemberRequest(
		m_PartyAMeberA->dwPartyID, m_PartyALeader->dwPlayerID, m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDelMemberNotify(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestPlayerLeaveParty_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("队员删除角色", "");

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyAMeberA);
	ASSERT_TRUE(nRetCode);

	nRetCode = GATEWAY->DoDeleteRoleRequest(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = GATEWAY->CheckDeleteRoleRespond(m_PartyAMeberA->dwPlayerID, true);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyDelMemberNotify(m_PartyAMeberA->dwPartyID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("A向不存在角色申请入队", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_szNoExistRole);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncPlayerApplyNotExist);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestApplyJionParty_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("玩家A、B不在队伍中,A向玩家B申请入队", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(200);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	int   nPartyNumberCount = 0;

	RecordProperty("A向玩家B队伍申请入队，玩家B队伍满员", "");

	for (unsigned int i = 1; i < m_RoleList.size(); i++)
	{
		if (m_RoleList[i].dwPartyID == m_PartyALeader->dwPartyID)
			nPartyNumberCount++;
	}

	for (unsigned int i = 1; i < m_RoleList.size(); i++)
	{
		if (m_RoleList[i].dwPartyID != ERROR_ID)
			continue;

		PlayerApplyJoinParty(&m_RoleList[i], m_PartyALeader);
		nPartyNumberCount++;

		if (nPartyNumberCount == MAX_PARTY_SIZE)
			break;
	}

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncDestTeamIsFull);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("A向玩家B队伍申请入队，玩家B队伍不在线", "");

	for (unsigned int i = 1; i < m_RoleList.size(); i++)
	{
		if (m_RoleList[i].dwPartyID == m_PartyALeader->dwPartyID)
			TestTools::PlayerLeaveGame(&m_RoleList[i]);
	}

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncPlayerNotOnline);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_5)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("A向玩家B队伍申请入队后，玩家A下线", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode  = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->DoApplyJoinPartyRespond(
		m_TestRoleA->szRoleName, m_PartyALeader->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode =  m_PartyALeader->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncApplicationOutOfDate);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_6)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();
	KGameServerSimulator* pGS = m_PartyALeader->pLoginOnGS;

	RecordProperty("A向玩家B队伍申请入队后，玩家B队长离线", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode  = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyALeader);
	ASSERT_TRUE(nRetCode);

	nRetCode = pGS->DoApplyJoinPartyRespond(
		m_TestRoleA->szRoleName, m_PartyALeader->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(200);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_7)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();
	KGameServerSimulator* pGS = m_PartyALeader->pLoginOnGS;

	RecordProperty("A向玩家B队伍申请入队后，玩家B队长更换", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode  = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->DoPartyChangeAuthorityRequest(
		m_PartyALeader->dwPlayerID, tatLeader, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(m_PartyAMeberA->dwPartyID, tatLeader, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = pGS->DoApplyJoinPartyRespond(
		m_TestRoleA->szRoleName, m_PartyALeader->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(200);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_8)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	int   nPartyNumberCount = 0;
	GUID  Guid     = GUID();

	RecordProperty("A向玩家B队伍申请入队后，玩家B队伍满员", "");

	for (unsigned int i = 1; i < m_RoleList.size(); i++)
	{
		if (m_RoleList[i].dwPartyID == m_PartyALeader->dwPartyID)
			nPartyNumberCount++;
	}

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	for (unsigned int i = 1; i < m_RoleList.size(); i++)
	{
		if (m_RoleList[i].dwPartyID != ERROR_ID)
			continue;

		PlayerApplyJoinParty(&m_RoleList[i], m_PartyALeader);
		nPartyNumberCount++;

		if (nPartyNumberCount == MAX_PARTY_SIZE)
			break;
	}

	nRetCode = m_PartyALeader->pLoginOnGS->DoApplyJoinPartyRespond(
		m_TestRoleA->szRoleName, m_PartyALeader->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncDestTeamIsFull);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}


TEST_F(KTestParty, TestApplyJionParty_9)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid     = GUID();

	RecordProperty("A向玩家B队伍申请入队，玩家B队伍队长不同意", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->DoApplyJoinPartyRespond(
		m_TestRoleA->szRoleName, m_PartyALeader->dwPlayerID, FALSE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncApplicationDenied);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestApplyJionParty_10)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid     = GUID();

	RecordProperty("A向玩家B队伍申请入队，玩家B队伍队长同意", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_TestRoleA->dwPlayerID, m_PartyAMeberA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->DoApplyJoinPartyRespond(
		m_TestRoleA->szRoleName, m_PartyALeader->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyAddMemberNotify(m_PartyALeader->dwPartyID, m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == m_PartyALeader->dwPartyID);
}

TEST_F(KTestParty, TestApplyJionParty_11)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("A已经组队中在申请入队", "");

	dwPartyID = GC->GetRolePartyID(m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_TestRoleA->pLoginOnGS->DoApplyJoinPartyRequest(m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncYouAlreadInTeamState);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("A向玩家B队伍申请入队，玩家B队伍队长同意", "");

	nRetCode = CreateParty(m_TestRoleB, m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncPlayerAlreadInOtherTeam);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("A向不存在角色邀请入队", "");

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_szNoExistRole);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncPlayerInviteNotExist);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;

	RecordProperty("玩家A向玩家B邀请组队,玩家B不在线", "");

	nRetCode = TestTools::PlayerLeaveGame(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncPlayerNotOnline);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	GUID  Guid     = GUID();

	RecordProperty("玩家A向玩家B重复邀请组队", "");

	nRetCode = CreateParty(m_TestRoleB, m_TestRoleC);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleB->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_TestRoleB->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_TestRoleB->szRoleName, m_TestRoleA->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncPlayerIsBusy);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_5)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，A离开游戏", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::PlayerLeaveGame(m_PartyALeader);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckPartyMessageNotify(
		m_TestRoleA->dwPlayerID, epncPlayerNotOnline);
	ASSERT_TRUE(nRetCode);	

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_6)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，玩家B处于组队状态", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = CreateParty(m_TestRoleA, m_TestRoleB);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, TRUE, Guid);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(200);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID != m_PartyALeader->dwPartyID);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_7)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，玩家B不同意", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, FALSE, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_PartyALeader->pLoginOnGS->CheckPartyMessageNotify(
		m_PartyALeader->dwPlayerID, epncInvitationDenied);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);
}

TEST_F(KTestParty, TestInvitePlayerJoinParty_8)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid      = GUID();

	RecordProperty("玩家A向玩家B邀请组队后，玩家B同意", "");

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == ERROR_ID);

	nRetCode = m_PartyALeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		m_PartyALeader->dwPlayerID, m_TestRoleA->szRoleName);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_TestRoleA->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		m_PartyALeader->szRoleName, m_TestRoleA->dwPlayerID, true, Guid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyAddMemberNotify(m_PartyALeader->dwPartyID, m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	dwPartyID = GC->GetRolePartyID(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(dwPartyID == m_PartyALeader->dwPartyID);
}

TEST_F(KTestParty, TestPartySyncMemberMaxLMR)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("同步队员LMR信息", "");

	m_pGameServerA->DoPartySyncMemberMaxLMR(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3);

	SCOPEPROTOCOL(m_pGameServerA, r2s_sync_team_member_max_lmr);
	SCOPEPROTOCOL(m_pGameServerB, r2s_sync_team_member_max_lmr);
	nRetCode = m_pGameServerA->CheckPartySyncMemberMaxLMR(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerB->CheckPartySyncMemberMaxLMR(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestPartySyncMemberCurrentLMR)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("同步队员当前LMR信息", "");

	nRetCode = m_pGameServerA->DoPartySyncMemberCurrentLMR(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3, 4);

	SCOPEPROTOCOL(m_pGameServerA, r2s_sync_team_member_current_lmr);
	SCOPEPROTOCOL(m_pGameServerB, r2s_sync_team_member_current_lmr);
	nRetCode = m_pGameServerA->CheckPartySyncMemberCurrentLMR(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3, 4);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerB->CheckPartySyncMemberCurrentLMR(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3, 4);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestPartySyncMemberMisc)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("同步队员Misc信息", "");

	nRetCode = m_pGameServerA->DoPartySyncMemberMisc(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3, 4, 2);


	SCOPEPROTOCOL(m_pGameServerA, r2s_sync_team_member_misc);
	SCOPEPROTOCOL(m_pGameServerB, r2s_sync_team_member_misc);
	nRetCode = m_pGameServerA->CheckPartySyncMemberMisc(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3, 4, 2);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerB->CheckPartySyncMemberMisc(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2, 3, 4, 2);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestPartySyncMemberPosition)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("同步队员Position信息", "");

	nRetCode = m_pGameServerA->DoPartySyncMemberPosition(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 1, 1, 2, 3);


	SCOPEPROTOCOL(m_pGameServerA, r2s_sync_team_member_position);
	SCOPEPROTOCOL(m_pGameServerB, r2s_sync_team_member_position);
	nRetCode = m_pGameServerA->CheckPartySyncMemberPosition(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 1, 1, 2, 3);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerB->CheckPartySyncMemberPosition(
		m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 1, 1, 2, 3);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestSetDistributeMan_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置分配者", "不合法的队伍ID");

	nRetCode = m_pGameServerA->DoPartyChangeAuthorityRequest(ERROR_ID, 0, 0);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(ERROR_ID, 0, 0);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestSetDistributeMan_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置分配者", "由没有设置分配权限的玩家设置");

	nRetCode = m_pGameServerA->DoPartyChangeAuthorityRequest(m_PartyAMeberA->dwPlayerID, tatDistribute, m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(m_PartyALeader->dwPartyID, tatDistribute, m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestSetDistributeMan_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置分配者", "由队长设置分配权限");

	m_pGameServerA->DoPartyChangeAuthorityRequest(m_PartyALeader->dwPlayerID, tatDistribute, m_PartyAMeberA->dwPlayerID);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(m_PartyALeader->dwPartyID, tatDistribute, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	m_pGameServerA->DoPartyChangeAuthorityRequest(m_PartyAMeberA->dwPlayerID, tatDistribute, m_PartyAMeberB->dwPlayerID);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(m_PartyALeader->dwPartyID, tatDistribute, m_PartyAMeberB->dwPlayerID);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestSetDistributeMan_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置分配者", "设置分配权限给不在队伍的人");

	nRetCode = m_pGameServerA->DoPartyChangeAuthorityRequest(m_PartyALeader->dwPlayerID, tatDistribute, m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartyChangeAuthorityNotify(m_PartyALeader->dwPartyID, tatDistribute, m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestPartySetLootMode_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置lootmode", "不合法的队伍ID");

	nRetCode = m_pGameServerA->DoPartySetLootModeRequest(ERROR_ID, 0, 0);

	nRetCode = m_pGameServerA->CheckPartySetLootModeRequest(ERROR_ID, 0);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestPartySetLootMode_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置lootmode", "由没有设置lootmode权限的玩家设置");

	nRetCode = m_pGameServerA->DoPartySetLootModeRequest(m_PartyALeader->dwPartyID, m_PartyAMeberA->dwPlayerID, 5);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetLootModeRequest(m_PartyALeader->dwPartyID, 5);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestPartySetLootMode_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置lootmode", "设置不合法的lootmode");

	nRetCode = m_pGameServerA->DoPartySetLootModeRequest(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, ilmInvalid);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetLootModeRequest(m_PartyALeader->dwPartyID, ilmInvalid);
	ASSERT_TRUE(!nRetCode);

	nRetCode = m_pGameServerA->DoPartySetLootModeRequest(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, ilmTotal);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetLootModeRequest(m_PartyALeader->dwPartyID, ilmTotal);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestPartySetLootMode_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置loot mode ", "set success");

	nRetCode = m_pGameServerA->DoPartySetLootModeRequest(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, ilmFreeForAll);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetLootModeRequest(m_PartyALeader->dwPartyID, ilmFreeForAll);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestSetRollQuality_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置SetRollQuality", "不合法的队伍ID");

	nRetCode = m_pGameServerA->DoPartySetRollQualityRequest(ERROR_ID, 0, 0);

	nRetCode = m_pGameServerA->CheckPartySetRollQualityRequest(ERROR_ID, 0);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestSetRollQuality_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置SetRollQuality", "由没有设置SetRollQuality权限的玩家设置");

	nRetCode = m_pGameServerA->DoPartySetRollQualityRequest(m_PartyALeader->dwPartyID, m_PartyAMeberA->dwPlayerID, 0);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetRollQualityRequest(m_PartyALeader->dwPartyID, 0);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestSetRollQuality_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置SetRollQuality", "set success");

	nRetCode = m_pGameServerA->DoPartySetRollQualityRequest(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, 2);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetRollQualityRequest(m_PartyALeader->dwPartyID, 2);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestSetFormationLeader_1)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置FormationLeader", "不合法的队伍ID");

	m_pGameServerA->DoPartySetFormationLeader(ERROR_ID, 0, 0, 0);

	nRetCode = m_pGameServerA->CheckPartySetFormationLeader(ERROR_ID, 0);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestSetFormationLeader_2)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置FormationLeader", "由没有设置FormationLeader权限的玩家设置");

	nRetCode = m_pGameServerA->DoPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyAMeberA->dwPlayerID, m_PartyALeader->dwPlayerID, 0);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID);
	ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestParty, TestSetFormationLeader_3)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置FormationLeader", "由队长设置FormationLeader");

	nRetCode = m_pGameServerA->DoPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, m_PartyAMeberA->dwPlayerID, 0);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyAMeberA->dwPlayerID);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->DoPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, m_PartyAMeberB->dwPlayerID, 0);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyAMeberB->dwPlayerID);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestParty, TestSetFormationLeader_4)
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	RecordProperty("设置FormationLeader", "设置FormationLeader给不在队伍的人");

	nRetCode = m_pGameServerA->DoPartySetFormationLeader(m_PartyALeader->dwPartyID, m_PartyALeader->dwPlayerID, m_TestRoleA->dwPlayerID, 0);
	ASSERT_TRUE(nRetCode);

	nRetCode = m_pGameServerA->CheckPartySetFormationLeader(m_PartyALeader->dwPartyID, m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(!nRetCode);
}
