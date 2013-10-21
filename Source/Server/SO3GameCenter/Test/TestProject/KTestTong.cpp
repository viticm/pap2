#include "stdafx.h"
#include "KGatewaySimulator.h"
#include "KGameServerSimulator.h"
#include "TestTools.h"
#include "KGameCenterWrapper.h"
#include "KGatewaySimulator.h"
#include "TongTools.h"
#include "gtest/gtest.h"
#include "NameGenerator.h"
#include "CampTools.h"

class KTestTong : public testing::Test 
{
protected:
	KTestTong(void)
	{
		m_pGameServerA = NULL;
		strcpy(m_szNoExistRole, "12312testt");
	}

	~KTestTong(void)
	{
	}

	void SetUp()
	{
		BOOL  bResult  = false;
		int   nRetCode = false;

		m_pGameServerA = new KGameServerSimulator();
		nRetCode = m_pGameServerA->Init();
		ASSERT_TRUE(nRetCode);

		stTongTools.m_pGS = m_pGameServerA;
		for (unsigned int i = 0; i < 20; i++)
		{
			m_RoleList.push_back(TestTools::CreateRole());
		}

		m_TongALeader = &m_RoleList[0];
		m_TestRoleA = &m_RoleList[1];
		m_TestRoleB = &m_RoleList[2];
		m_TongAMeberA = &m_RoleList[3];
		m_OffLineRole = &m_RoleList[m_RoleList.size() - 1];

		for(size_t i = 0; i < m_RoleList.size() / 2; i++)
		{
			nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
				m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
			ASSERT_TRUE(nRetCode);
		}


		std::string TongName = NameGenerator::Instance()->LegalName(3);
		ASSERT_TRUE(stTongTools.CreateTong(*m_TongALeader, TongName.c_str(), 0) != -1);
		ASSERT_TRUE(stTongTools.JoinTong(*m_TongALeader, *m_TongAMeberA));
	}

	void TearDown()
	{
		SAFE_DELETE(m_pGameServerA);

		KGThread_Sleep(300);
		for (int i = 0; i < m_RoleList.size(); i++)
		{
			TestTools::DeleteRole(&m_RoleList[i]);
		}
	}

protected:
	KGameServerSimulator* m_pGameServerA;

	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
	ROLE_VECTOR      m_RoleList;

	TestTools::KRoleInfo*       m_TestRoleA;
	TestTools::KRoleInfo*       m_TestRoleB;
	TestTools::KRoleInfo*       m_OffLineRole;
	TestTools::KRoleInfo*       m_TongALeader;
	TestTools::KRoleInfo*       m_TongAMeberA;
	TestTools::KRoleInfo*       m_TongAMeberB;
	char             m_szNoExistRole[_NAME_LEN];
	TongTools			stTongTools;
};

TEST_F(KTestTong, CreateTong_1)
{
	char szData[_NAME_LEN + 1];
	fill(szData, szData + _NAME_LEN, 'a');
	szData[_NAME_LEN] = '\0';
	CASE_INFO("非法参数", "无法创建");
	ASSERT_EQ(stTongTools.CreateTong(ERROR_ID, "哈哈", 0), ERROR_ID);
	ASSERT_EQ(stTongTools.CreateTong(m_TestRoleA->dwPlayerID, szData, 0), ERROR_ID);
	ASSERT_EQ(stTongTools.CreateTong(m_TestRoleA->dwPlayerID, szData, 1), ERROR_ID);
	ASSERT_EQ(stTongTools.CreateTong(m_TestRoleA->dwPlayerID, "可卡", CHAR_MAX), ERROR_ID);
}

TEST_F(KTestTong, CreateTong_2)
{
	CASE_INFO("创建帮会", "玩家已在帮会中");
	ASSERT_TRUE(stTongTools.CreateTong(*m_TestRoleA, "哈哈", 0) != ERROR_ID);
	ASSERT_EQ(stTongTools.CreateTong(*m_TestRoleA, "哈哈", 0), ERROR_ID);
}

TEST_F(KTestTong, CreateTong_3)
{
	CASE_INFO("创建帮会", "非法帮会名称");
	std::string TongName = NameGenerator::Instance()->IllegalName(3).c_str();
	ASSERT_EQ(stTongTools.CreateTong(*m_TestRoleA, TongName.c_str(), 0), ERROR_ID);
	ASSERT_EQ(stTongTools.TongID(m_TestRoleA->dwPlayerID), ERROR_ID);
}

TEST_F(KTestTong, CreateTong_4)
{
	CASE_INFO("创建帮会", "已存在帮会名称");
	std::string TongName = NameGenerator::Instance()->LegalName(3);
	ASSERT_TRUE(stTongTools.CreateTong(*m_TestRoleA, TongName.c_str(), 0) != ERROR_ID);
	ASSERT_EQ(stTongTools.CreateTong(*m_TestRoleB, TongName.c_str(), 0), ERROR_ID);
	ASSERT_EQ(stTongTools.TongID(m_TestRoleB->dwPlayerID), ERROR_ID);
}

TEST_F(KTestTong, InviteJoinTong_1)
{
	CASE_INFO("邀请加入帮会", "非法参数");
	char szData[_NAME_LEN + 1];
	fill(szData, szData + _NAME_LEN, 'a');
	szData[_NAME_LEN] = '\0';
	std::string TongName = NameGenerator::Instance()->LegalName(3);
	stTongTools.InviteJoinTong(m_TestRoleA->dwPlayerID, szData);
}

TEST_F(KTestTong, InviteJoinTong_2)
{
	CASE_INFO("邀请加入帮会", "邀请者不在帮会");
	std::string TongName = NameGenerator::Instance()->LegalName(3);
	ASSERT_EQ(stTongTools.InviteJoinTong(m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName), false);
}

TEST_F(KTestTong, InviteJoinTong_3)
{
	CASE_INFO("邀请加入帮会", "被邀请者在帮会");
	std::string TongName = NameGenerator::Instance()->LegalName(3);
	ASSERT_TRUE(stTongTools.CreateTong(*m_TestRoleA, TongName.c_str(), 0) != ERROR_ID);
	ASSERT_EQ(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName), false);
}

TEST_F(KTestTong, InviteJoinTong_4)
{
	CASE_INFO("邀请加入帮会", "被邀请者不在线");
	ASSERT_EQ(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_OffLineRole->szRoleName), false);
}

TEST_F(KTestTong, InviteJoinTong_5)
{
	CASE_INFO("邀请加入帮会", "邀请者玩家不存在");
	ASSERT_EQ(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_szNoExistRole), false);
}

TEST_F(KTestTong, InviteJoinTong_6)
{
	CASE_INFO("邀请加入帮会", "邀请者玩家在对立阵营");
	CampTools ct;
	ct.m_pGS = m_pGameServerA;
	ASSERT_TRUE(stTongTools.ChangeTongCamp(m_TongALeader->dwPlayerID, cGood));
	ASSERT_TRUE(ct.SetCamp(m_TestRoleA->dwPlayerID, cEvil));
	ASSERT_EQ(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName), false);
}

TEST_F(KTestTong, InviteJoinTong_7)
{
	CASE_INFO("邀请加入帮会", "玩家已被邀请");
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName));
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName) == false);
}


TEST_F(KTestTong, InviteJoinTong_8)
{
	CASE_INFO("邀请加入帮会", "邀请者没有权限");
	ASSERT_TRUE(stTongTools.JoinTong(*m_TongALeader, *m_TestRoleA));
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TestRoleA->dwPlayerID, m_TestRoleB->szRoleName) == false);
}

TEST_F(KTestTong, RespondInviteJoinTong_1)
{
	CASE_INFO("回复邀请加入帮会", "没有被邀请");
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, m_TongALeader->dwPlayerID, m_TestRoleA->dwPlayerID, m_TongALeader->dwTongID));
}

TEST_F(KTestTong, RespondInviteJoinTong_2)
{
	CASE_INFO("回复邀请加入帮会", "邀请者不存在");
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, ERROR_ID, m_TestRoleA->dwPlayerID, m_TongALeader->dwTongID));
}

TEST_F(KTestTong, RespondInviteJoinTong_3)
{
	CASE_INFO("回复邀请加入帮会", "玩家ID错误");
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, m_TongALeader->dwPlayerID, ERROR_ID, m_TongALeader->dwTongID));
}

TEST_F(KTestTong, RespondInviteJoinTong_4)
{
	CASE_INFO("回复邀请加入帮会", "帮会ID错误");
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName));
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, m_TongALeader->dwPlayerID, m_TestRoleA->dwPlayerID, ERROR_ID));
}

TEST_F(KTestTong, RespondInviteJoinTong_5)
{
	CASE_INFO("回复邀请加入帮会", "帮会阵营错误");
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName));
	ASSERT_TRUE(stTongTools.ChangeTongCamp(m_TongALeader->dwPlayerID, cGood));
	CampTools ct;
	ct.m_pGS = m_pGameServerA;
	ASSERT_TRUE(ct.SetCamp(m_TestRoleA->dwPlayerID, cEvil));
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, m_TongALeader->dwPlayerID, m_TestRoleA->dwPlayerID, m_TongALeader->dwTongID));
}

TEST_F(KTestTong, RespondInviteJoinTong_6)
{
	CASE_INFO("回复邀请加入帮会", "邀请者退出帮会");
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName));
	ASSERT_TRUE(stTongTools.JoinTong(*m_TongALeader, *m_TestRoleB));
	KGThread_Sleep(200);
	ASSERT_TRUE(stTongTools.ChangeTongMaster(m_TongALeader->dwPlayerID, m_TestRoleB->dwPlayerID));
	ASSERT_TRUE(stTongTools.QuitTong(m_TongALeader->dwPlayerID));
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, m_TongALeader->dwPlayerID, m_TestRoleA->dwPlayerID, m_TongALeader->dwTongID));
}

TEST_F(KTestTong, RespondInviteJoinTong_7)
{
	CASE_INFO("回复邀请加入帮会", "邀请者没有权限");
	ASSERT_TRUE(stTongTools.InviteJoinTong(m_TongALeader->dwPlayerID, m_TestRoleA->szRoleName));
	ASSERT_TRUE(stTongTools.JoinTong(*m_TongALeader, *m_TestRoleB));
	KGThread_Sleep(200);
	ASSERT_TRUE(stTongTools.ChangeTongMaster(m_TongALeader->dwPlayerID, m_TestRoleB->dwPlayerID));
	ASSERT_FALSE(stTongTools.CheckInviteRespond(true, m_TongALeader->dwPlayerID, m_TestRoleA->dwPlayerID, m_TongALeader->dwTongID));
}

TEST_F(KTestTong, BaseOperator_1)
{
	CASE_INFO("修改基本操作权限", "非法参数");
	ASSERT_FALSE(stTongTools.ModifyBaseOperator(ERROR_ID, 0, 0, 0, 0));
	ASSERT_FALSE(stTongTools.ModifyBaseOperator(m_TongALeader->dwPlayerID, MAX_TONG_GROUP + 1, 0, 0, 0));
	ASSERT_FALSE(stTongTools.ModifyBaseOperator(m_TongALeader->dwPlayerID, 0, MAX_TONG_GROUP + 1, 0, 0));
	ASSERT_FALSE(stTongTools.ModifyBaseOperator(m_TongALeader->dwPlayerID, 0, 0, MAX_TONG_GROUP + 1, 0));
}

TEST_F(KTestTong, BaseOperator_2)
{
	CASE_INFO("修改基本操作权限", "有权限设置的组");
	KTongSchemaData Data;
	ASSERT_TRUE(stTongTools.GetScheMaData(m_TongALeader->dwTongID, &Data));
	int GroupID = stTongTools.GetGroupID(m_TongALeader->dwTongID, m_TongALeader->dwPlayerID);
	ASSERT_TRUE(GroupID != -1);
	KTongGroup TongGroup;
	for(int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < TONG_OPERATION_NUM; k++)
		{
			if (Data.Data[GroupID].byBaseGrantMask[i] & (1 << k))
			{
				ASSERT_TRUE(stTongTools.ModifyBaseOperator(m_TongALeader->dwPlayerID, i, tmsBaseOperation, k, true));
				ASSERT_TRUE(stTongTools.GetTongGroup(m_TongALeader->dwTongID, i, &TongGroup));
				ASSERT_TRUE(TongGroup.byBaseOperationMask & (1 << k));
				ASSERT_TRUE(stTongTools.ModifyBaseOperator(m_TongALeader->dwPlayerID, i, tmsBaseOperation, k, false));
				ASSERT_TRUE(stTongTools.GetTongGroup(m_TongALeader->dwTongID, i, &TongGroup));
				ASSERT_FALSE(TongGroup.byBaseOperationMask & (1 << k));
			}
		}
	}
}

TEST_F(KTestTong, BaseOperator_3)
{
	CASE_INFO("修改基本操作权限", "没有权限设置的组");
	KTongSchemaData Data;
	ASSERT_TRUE(stTongTools.GetScheMaData(m_TongALeader->dwTongID, &Data));
	int GroupID = stTongTools.GetGroupID(m_TongALeader->dwTongID, m_TongALeader->dwPlayerID);
	ASSERT_TRUE(GroupID != -1);
	for(int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < TONG_OPERATION_NUM; k++)
		{
			if (!(Data.Data[GroupID].byBaseGrantMask[i] & (1 << k)))
			{
				ASSERT_FALSE(stTongTools.ModifyBaseOperator(m_TongALeader->dwPlayerID, i, tmsBaseOperation, k, true));
				return;
			}
		}
	}
}

TEST_F(KTestTong, AdvanceOperator_1)
{
	CASE_INFO("修改高级操作权限", "非法参数");
	ASSERT_FALSE(stTongTools.ModifyAdvanceOperator(ERROR_ID, 0, 0, 0, 0));
	ASSERT_FALSE(stTongTools.ModifyAdvanceOperator(m_TongALeader->dwPlayerID, MAX_TONG_GROUP + 1, 0, 0, 0));
	ASSERT_FALSE(stTongTools.ModifyAdvanceOperator(m_TongALeader->dwPlayerID, 0, MAX_TONG_GROUP + 1, 0, 0));
	ASSERT_FALSE(stTongTools.ModifyAdvanceOperator(m_TongALeader->dwPlayerID, 0, 0, MAX_TONG_GROUP + 1, 0));
}

TEST_F(KTestTong, AdvanceOperator_2)
{
	CASE_INFO("修改高级操作权限", "有权限设置的组");
	KTongSchemaData Data;
	ASSERT_TRUE(stTongTools.GetScheMaData(m_TongALeader->dwTongID, &Data));
	int GroupID = stTongTools.GetGroupID(m_TongALeader->dwTongID, m_TongALeader->dwPlayerID);
	ASSERT_TRUE(GroupID != -1);
	KTongGroup TongGroup;
	for(int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int m = 0 ; m < MAX_TONG_GROUP; m++)
		{

			for (int k = 0; k < TONG_OPERATION_NUM; k++)
			{
				if (Data.Data[GroupID].byAdvanceGrantMask[i][m] & (1 << k))
				{
					ASSERT_TRUE(stTongTools.ModifyAdvanceOperator(m_TongALeader->dwPlayerID, i, m, k, true));
					ASSERT_TRUE(stTongTools.ModifyAdvanceOperator(m_TongALeader->dwPlayerID, i, m, k, false));
				}
			}
		}
	}
}

TEST_F(KTestTong, AdvanceOperator_3)
{
	CASE_INFO("修改高级操作权限", "没有权限设置的组");
	KTongSchemaData Data;
	ASSERT_TRUE(stTongTools.GetScheMaData(m_TongALeader->dwTongID, &Data));
	int GroupID = stTongTools.GetGroupID(m_TongALeader->dwTongID, m_TongALeader->dwPlayerID);
	ASSERT_TRUE(GroupID != -1);
	for(int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int m = 0 ; m < MAX_TONG_GROUP; m++)
		{
			for (int k = 0; k < TONG_OPERATION_NUM; k++)
			{
				if (!(Data.Data[GroupID].byAdvanceGrantMask[i][m] & (1 << k)))
				{
					ASSERT_FALSE(stTongTools.ModifyAdvanceOperator(m_TongALeader->dwPlayerID, i, m, k, true));
					return;
				}
			}
		}
	}
}

TEST_F(KTestTong, ChangeGroupName_1)
{
	CASE_INFO("修改groupname权限", "有权限groupname");
	KTongGroup TongGroup;
	for(int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < MAX_TONG_GROUP; k++)
		{
			if (stTongTools.CanAdvanceOperate(m_TongALeader->dwTongID, i, k, totModifyGroupName))
			{
				ASSERT_TRUE(stTongTools.ModifyGroupName(m_TongALeader->dwPlayerID, i, "Test"));
				ASSERT_TRUE(stTongTools.GetTongGroup(m_TongALeader->dwTongID, i, &TongGroup));
				ASSERT_STREQ(TongGroup.szName, "Test");
			}
		}
	}
}


TEST_F(KTestTong, ChangeGroupName_2)
{
	CASE_INFO("修改groupname权限", "没有权限groupname");
	for(int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < MAX_TONG_GROUP; k++)
		{
			if (!stTongTools.CanAdvanceOperate(m_TongALeader->dwTongID, i, k, totModifyGroupName))
			{
				stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
				ASSERT_FALSE(stTongTools.ModifyGroupName(m_TongAMeberA->dwPlayerID, k, "Test"));
				return;
			}
		}
	}
}


TEST_F(KTestTong, ChangeGroupWage_1)
{
	CASE_INFO("修改groupname权限", "有权限groupwage");
	KTongGroup TongGroup;

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < MAX_TONG_GROUP; k++)
		{
			if (stTongTools.CanAdvanceOperate(m_TongALeader->dwTongID, i, k, totModifyWage))
			{
				stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
				KGThread_Sleep(100);
				ASSERT_TRUE(stTongTools.ModifyGroupWage(m_TongAMeberA->dwPlayerID, k, 100));
				ASSERT_TRUE(stTongTools.GetTongGroup(m_TongALeader->dwTongID, k, &TongGroup));
				ASSERT_EQ(TongGroup.nWage, 100);
				return;
			}
		}
	}
}

TEST_F(KTestTong, ChangeGroupWage_2)
{
	CASE_INFO("修改groupwage权限", "没有权限groupwage");

	for (int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < MAX_TONG_GROUP; k++)
		{
			if (!stTongTools.CanAdvanceOperate(m_TongALeader->dwTongID, i, k, totModifyWage))
			{
				stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
				KGThread_Sleep(100);
				ASSERT_FALSE(stTongTools.ModifyGroupWage(m_TongAMeberA->dwPlayerID, k, 100));
				return;
			}
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_1)
{
	CASE_INFO("修改groupwage权限", "非法参数");
	char szData[100];
	fill(szData, szData + 100, 'a');
	szData[100 - 1] = '\0';
	stTongTools.ModifyTongInfo(ERROR_ID, -1, "");
	stTongTools.ModifyTongInfo(m_TestRoleA->dwPlayerID, -1, "");
	stTongTools.ModifyTongInfo(m_TongALeader->dwPlayerID, tmrEnd + 1, "");
	stTongTools.ModifyTongInfo(m_TongALeader->dwPlayerID, tmrRules, szData);
}

TEST_F(KTestTong, ModifyTongInfo_2)
{
	CASE_INFO("修改帮会名称权限", "不是帮主");
	ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrTongName, "test"), tenModifyTongNameNoPermissionError);
}

TEST_F(KTestTong, ModifyTongInfo_3)
{
	CASE_INFO("修改帮会名称权限", "是帮主");
	ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongALeader->dwPlayerID, tmrTongName, "test"), tenModifyTongNameSuccess);
}

TEST_F(KTestTong, ModifyTongInfo_4)
{
	CASE_INFO("修改Announcement权限", "有权限Announcement");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyAnnouncement))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrAnnouncement, "45645"), tenModifyAnnouncementSuccess);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_5)
{
	CASE_INFO("修改Announcement权限", "没有权限Announcement");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (!stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyAnnouncement))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrAnnouncement, "45645"), tenModifyAnnouncementNoPermissionError);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_6)
{
	CASE_INFO("修改OnlineMessage权限", "有权限OnlineMessage");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyOnlineMessage))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrOnlineMessage, "45645"), tenModifyOnlineMessageSuccess);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_7)
{
	CASE_INFO("修改OnlineMessage权限", "没有权限OnlineMessage");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (!stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyOnlineMessage))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrOnlineMessage, "45645"), tenModifyOnlineMessageNoPermissionError);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_8)
{
	CASE_INFO("修改Introduction权限", "有权限Introduction");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyIntroduction))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrIntroduction, "45645"), tenModifyIntroductionSuccess);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_9)
{
	CASE_INFO("修改Introduction权限", "没有权限Introduction");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (!stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyIntroduction))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrIntroduction, "45645"), tenModifyIntroductionNoPermissionError);
			return;
		}
	}
}


TEST_F(KTestTong, ModifyTongInfo_10)
{
	CASE_INFO("修改Introduction权限", "有权限Introduction");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyRules))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrRules, "45645"), tenModifyRulesSuccess);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyTongInfo_11)
{
	CASE_INFO("修改Introduction权限", "没有权限Introduction");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (!stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyRules))
		{
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyTongInfo(m_TongAMeberA->dwPlayerID, tmrRules, "45645"), tenModifyRulesNoPermissionError);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyMorabilia_1)
{
	CASE_INFO("修改Introduction权限", "非法参数");

	KTONG_MODIFY_MEMORABILIA_INFO info;
	info.byOperationType = mmtDelete + 1;
	stTongTools.ModifyMemorabilia(m_TongAMeberA->dwPlayerID, &info, sizeof(info));
}


TEST_F(KTestTong, ModifyMorabilia_2)
{
	CASE_INFO("修改Introduction权限", "有权限Introduction");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyMemorabilia))
		{
			KTONG_ADD_MEMORABILIA Info;
			Info.byOperationType = mmtAdd;
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongAMeberA->dwPlayerID, &Info, sizeof(Info)), tenModifyMemorabiliaSuccess);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyMorabilia_3)
{
	CASE_INFO("修改Introduction权限", "没有权限Introduction");

	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (!stTongTools.CanBaseOperate(m_TongALeader->dwTongID, i, totModifyMemorabilia))
		{
			KTONG_ADD_MEMORABILIA Info;
			Info.byOperationType = mmtAdd;
			stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
			ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongAMeberA->dwPlayerID, &Info, sizeof(Info)), tenModifyMemorabiliaError);
			return;
		}
	}
}

TEST_F(KTestTong, ModifyMorabilia_4)
{
	CASE_INFO("修改Introduction权限", "添加超过上限");
	KTONG_ADD_MEMORABILIA Info;
	Info.byOperationType = mmtAdd;

	for (int i = 0; i < MAX_TONG_MEMORABILIA_COUNT; i++)
	{
		ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &Info, sizeof(Info)), tenModifyMemorabiliaSuccess);
	}

	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &Info, sizeof(Info)), tenModifyMemorabiliaError);
}

TEST_F(KTestTong, ModifyMorabilia_5)
{
	CASE_INFO("修改Introduction权限", "修改不存在的ID");
	KTONG_MODIFY_MEMORABILIA Info;
	Info.byOperationType = mmtModify;
	Info.dwID = UINT_MAX;
	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &Info, sizeof(Info)), tenModifyMemorabiliaError);
}


TEST_F(KTestTong, ModifyMorabilia_6)
{
	CASE_INFO("修改Introduction权限", "修改存在的ID");
	KTONG_ADD_MEMORABILIA AddInfo;
	AddInfo.byOperationType = mmtAdd;
	KTONG_MODIFY_MEMORABILIA ModifInfo;
	ModifInfo.byOperationType = mmtModify;
	ModifInfo.dwID = 1;
	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &AddInfo, sizeof(AddInfo)), tenModifyMemorabiliaSuccess);
	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &ModifInfo, sizeof(ModifInfo)), tenModifyMemorabiliaSuccess);
}


TEST_F(KTestTong, ModifyMorabilia_7)
{
	CASE_INFO("修改Introduction权限", "删除不存在的ID");
	KTONG_DELETE_MEMORABILIA Info;
	Info.byOperationType = mmtDelete;
	Info.dwID = UINT_MAX;
	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &Info, sizeof(Info)), tenModifyMemorabiliaError);
}


TEST_F(KTestTong, ModifyMorabilia_8)
{
	CASE_INFO("修改Introduction权限", "删除存在的ID");
	KTONG_ADD_MEMORABILIA AddInfo;
	AddInfo.byOperationType = mmtAdd;
	KTONG_DELETE_MEMORABILIA DeleteInfo;
	DeleteInfo.byOperationType = mmtDelete;
	DeleteInfo.dwID = 1;
	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &AddInfo, sizeof(AddInfo)), tenModifyMemorabiliaSuccess);
	ASSERT_EQ(stTongTools.ModifyMemorabilia(m_TongALeader->dwPlayerID, &DeleteInfo, sizeof(DeleteInfo)), tenModifyMemorabiliaSuccess);
}


TEST_F(KTestTong, QutiTong_1)
{
	CASE_INFO("退出帮会", "ErrorID");
	ASSERT_FALSE(stTongTools.QuitTong(ERROR_ID));
}

TEST_F(KTestTong, QutiTong_2)
{
	CASE_INFO("退出帮会", "不再帮会中玩家");
	ASSERT_FALSE(stTongTools.QuitTong(m_TestRoleA->dwPlayerID));
}

TEST_F(KTestTong, QutiTong_3)
{
	CASE_INFO("退出帮会", "帮主推出");
	ASSERT_FALSE(stTongTools.QuitTong(m_TongALeader->dwPlayerID));
}

TEST_F(KTestTong, QutiTong_4)
{
	CASE_INFO("退出帮会", "帮员推出");
	ASSERT_TRUE(stTongTools.QuitTong(m_TongAMeberA->dwPlayerID));
	ASSERT_EQ(stTongTools.TongID(m_TongAMeberA->dwPlayerID), ERROR_ID);
}


TEST_F(KTestTong, ChangeGroup_1)
{
	CASE_INFO("改变帮组", "非法参数");
	ASSERT_FALSE(stTongTools.ChangeGroup(ERROR_ID, 0, 0));
	ASSERT_FALSE(stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, ERROR_ID, 0));
	ASSERT_FALSE(stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, -1));
}

TEST_F(KTestTong, ChangeGroup_2)
{
	CASE_INFO("改变帮组", "改变者没有权限");
	ASSERT_FALSE(stTongTools.ChangeGroup(m_TongAMeberA->dwPlayerID, m_TongAMeberA->dwPlayerID, 0));
}

TEST_F(KTestTong, ChangeGroup_3)
{
	CASE_INFO("改变帮组", "帮组改变者自己租");
	ASSERT_FALSE(stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongALeader->dwPlayerID, 2));
}

TEST_F(KTestTong, ChangeGroup_4)
{
	CASE_INFO("改变帮组", "改变到相同组");
	int nGroup = stTongTools.GetGroupID(m_TongAMeberA->dwTongID, m_TongAMeberA->dwPlayerID);
	ASSERT_FALSE(stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, nGroup));
}

TEST_F(KTestTong, ChangeGroup_5)
{
	CASE_INFO("改变帮组", "改变到未开组");
	int nGroup = stTongTools.GetGroupID(m_TongAMeberA->dwTongID, m_TongAMeberA->dwPlayerID);
	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (i != nGroup)
		{
			if (!stTongTools.IsEnableGroup(m_TongAMeberA->dwTongID, i))
			{
				ASSERT_FALSE(stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i));
				return;
			}
		}
	}
}

TEST_F(KTestTong, ChangeGroup_6)
{
	CASE_INFO("改变帮组", "已开开组");
	int nGroup = stTongTools.GetGroupID(m_TongAMeberA->dwTongID, m_TongAMeberA->dwPlayerID);
	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		if (i != nGroup)
		{
			if (stTongTools.IsEnableGroup(m_TongAMeberA->dwTongID, i))
			{
				ASSERT_TRUE(stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i));
				return;
			}
		}
	}
}

TEST_F(KTestTong, SaveMoney_1)
{
	CASE_INFO("存钱", "ErrorID");
	ASSERT_EQ(stTongTools.SaveMoney(ERROR_ID, 0), tenInvalid);
}

TEST_F(KTestTong, SaveMoney_2)
{
	CASE_INFO("存钱", "不再帮会中成员");
	ASSERT_EQ(stTongTools.SaveMoney(m_TestRoleB->dwPlayerID, 0), tenInvalid);
}

TEST_F(KTestTong, SaveMoney_3)
{
	CASE_INFO("存钱", "money为0");
	ASSERT_EQ(stTongTools.SaveMoney(m_TongALeader->dwPlayerID, 0), tenInvalid);
}

TEST_F(KTestTong, SaveMoney_4)
{
	CASE_INFO("存钱", "money为负");
	ASSERT_EQ(stTongTools.SaveMoney(m_TongALeader->dwPlayerID, -110), tenInvalid);
}

TEST_F(KTestTong, SaveMoney_5)
{
	CASE_INFO("存钱", "money为正");
	ASSERT_EQ(stTongTools.SaveMoney(m_TongALeader->dwPlayerID, 100), tenSaveMoneySuccess);
}

TEST_F(KTestTong, ChangeTongMemberRemark_1)
{
	CASE_INFO("ChangeTongMemberRemark", "ErrorID");
	ASSERT_EQ(stTongTools.ChangeTongMemberRemark(ERROR_ID, ERROR_ID, ""), tenInvalid);
	ASSERT_EQ(stTongTools.ChangeTongMemberRemark(m_TongALeader->dwPlayerID, ERROR_ID, ""), tenInvalid);
}

TEST_F(KTestTong, ChangeTongMemberRemark_2)
{
	CASE_INFO("ChangeTongMemberRemark", "不再帮会中玩家");
	ASSERT_EQ(stTongTools.ChangeTongMemberRemark(m_TestRoleB->dwPlayerID, m_TongALeader->dwPlayerID, ""), tenInvalid);
	ASSERT_EQ(stTongTools.ChangeTongMemberRemark(m_TongALeader->dwPlayerID, m_TestRoleB->dwPlayerID, ""), tenInvalid);
}

TEST_F(KTestTong, ChangeTongMemberRemark_3)
{
	CASE_INFO("ChangeTongMemberRemark", "没有权限");
	for (int i = 1; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 1; k < MAX_TONG_GROUP; k++)
		{
			if (!stTongTools.CanAdvanceOperate(m_TongALeader->dwTongID, i, k, totModifyMemberRemark))
			{
				stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
				ASSERT_TRUE(stTongTools.JoinTong(*m_TongALeader, *m_TestRoleA));
				stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TestRoleA->dwPlayerID, k);
				ASSERT_EQ(stTongTools.ChangeTongMemberRemark(m_TongAMeberA->dwPlayerID, m_TestRoleA->dwPlayerID, ""), tenChangeMemberRemarkError);
				return;
			}
		}
	}
}


TEST_F(KTestTong, ChangeTongMemberRemark_4)
{
	CASE_INFO("ChangeTongMemberRemark", "有权限");
	for (int i = 0; i < MAX_TONG_GROUP; i++)
	{
		for (int k = 0; k < MAX_TONG_GROUP; k++)
		{
			if (stTongTools.CanAdvanceOperate(m_TongALeader->dwTongID, i, k, totModifyMemberRemark))
			{
				stTongTools.ChangeGroup(m_TongALeader->dwPlayerID, m_TongAMeberA->dwPlayerID, i);
				ASSERT_EQ(stTongTools.ChangeTongMemberRemark(m_TongAMeberA->dwPlayerID, m_TongAMeberA->dwPlayerID, ""), tenChangeMemberRemarkSuccess);
				return;
			}
		}
	}
}

TEST_F(KTestTong, TongState_1)
{
	CASE_INFO("tong状态转换", "新建帮会");
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsTrialTong);
}

TEST_F(KTestTong, TongState_2)
{
	CASE_INFO("tong状态转换", "超过试用期人数不满");
	GC->AccelerationGCTime(TONG_TRIAL_TIME + 10);
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsInvalid);
}

TEST_F(KTestTong, TongState_3)
{
	CASE_INFO("tong状态转换", "超过试用期人数已满");
	for (unsigned int i = 0; i < 10; i++)
	{
		TestTools::KRoleInfo role = TestTools::CreateRole();
		bool bRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &role,
			m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
		ASSERT_TRUE(bRetCode);
		ASSERT_TRUE(stTongTools.JoinTong(*m_TongALeader, role));
	}

	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsNormalTong);
}

struct SocpeDeleteRole
{
	SocpeDeleteRole(std::vector<TestTools::KRoleInfo>& List)
	{
		pList = &List;
	}
	~SocpeDeleteRole()
	{
		for (int i = 0 ;i < pList->size(); i++)
		{
			TestTools::DeleteRole(&(*pList)[i]);
		}
	}
	std::vector<TestTools::KRoleInfo>* pList;
};
TEST_F(KTestTong, TongState_4)
{
	CASE_INFO("tong状态转换", "tsTrialTong时人数增加到最小人数");
	std::vector<TestTools::KRoleInfo> RoleList;
	SocpeDeleteRole SDR(RoleList);
	int JoinTongCount = 0;
	for (unsigned int i = 0; i < 2 * MIN_TONG_MEMBER_COUNT; i++)
	{
		TestTools::KRoleInfo role = TestTools::CreateRole();
		RoleList.push_back(role);
		bool bRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &RoleList[RoleList.size() - 1],
			m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
		if (!bRetCode)
		{
			continue;
		}
		bRetCode = stTongTools.JoinTong(*m_TongALeader, role);
		if (!bRetCode)
		{
			continue;
		}
		if (JoinTongCount ++ >= MIN_TONG_MEMBER_COUNT)
		{
			break;
		}
	}
	ASSERT_TRUE(JoinTongCount >= MIN_TONG_MEMBER_COUNT);
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsNormalTong);
}


TEST_F(KTestTong, TongState_5)
{
	CASE_INFO("tong状态转换", "tsNormalTong人数减少");
	std::vector<TestTools::KRoleInfo> RoleList;
	SocpeDeleteRole SDR(RoleList);
	int JoinTongCount = 0;
	for (unsigned int i = 0; i < 2 * MIN_TONG_MEMBER_COUNT; i++)
	{
		TestTools::KRoleInfo role = TestTools::CreateRole();
		RoleList.push_back(role);
		bool bRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &RoleList[RoleList.size() - 1],
			m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
		if (!bRetCode)
		{
			continue;
		}
		bRetCode = stTongTools.JoinTong(*m_TongALeader, role);
		if (!bRetCode)
		{
			continue;
		}
		if (JoinTongCount ++ >= MIN_TONG_MEMBER_COUNT)
		{
			break;
		}
	}
	ASSERT_TRUE(JoinTongCount >= MIN_TONG_MEMBER_COUNT);
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsNormalTong);
	for (int i = 0; i < RoleList.size(); i++)
	{
		stTongTools.QuitTong(RoleList[i].dwPlayerID);
	}
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsDisbandTong);
}

TEST_F(KTestTong, TongState_6)
{
	CASE_INFO("tong状态转换", "tsDisbandTong维持时间到");
	std::vector<TestTools::KRoleInfo> RoleList;
	SocpeDeleteRole SDR(RoleList);
	int JoinTongCount = 0;
	for (unsigned int i = 0; i < 2 * MIN_TONG_MEMBER_COUNT; i++)
	{
		TestTools::KRoleInfo role = TestTools::CreateRole();
		RoleList.push_back(role);
		bool bRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &RoleList[RoleList.size() - 1],
			m_pGameServerA->m_CreatedMapCopy[0].dwMapID, m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
		if (!bRetCode)
		{
			continue;
		}
		bRetCode = stTongTools.JoinTong(*m_TongALeader, role);
		if (!bRetCode)
		{
			continue;
		}
		if (JoinTongCount ++ >= MIN_TONG_MEMBER_COUNT)
		{
			break;
		}
	}
	ASSERT_TRUE(JoinTongCount >= MIN_TONG_MEMBER_COUNT);
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsNormalTong);
	for (int i = 0; i < RoleList.size(); i++)
	{
		stTongTools.QuitTong(RoleList[i].dwPlayerID);
	}
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsDisbandTong);
	GC->AccelerationGCTime(TONG_TRIAL_TIME + 10);
	GC->AccelerationRemainderGC(GAME_FPS * 10, 2);
	GC->ManualActivate();
	ASSERT_EQ(stTongTools.TongState(m_TongAMeberA->dwTongID), tsInvalid);
}