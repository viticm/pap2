#include "StdAfx.h"
#include "KTestDBManager.h"
#include "TestTools.h"
#include "KGameCenterWrapper.h"
#include "KGameServerSimulator.h"
#include "MailTools.h"
#include "Utility.h"
#include <gtest/gtest.h>


class KTestMail : public testing::Test  
{
protected:
	KTestMail(void)
	{
		m_pGameServerA = NULL;

		strncpy(m_szNoExistRole, "oooooooooooooo", _NAME_LEN);


		char szMailContent[] = "TestMailContent";
		BYTE byItem[] = "TestItem";
		m_MailContent = (KMail*)new char[sizeof(KMail)+ sizeof(szMailContent) + sizeof(byItem)];
		memset(m_MailContent, 0, sizeof(KMail)+ sizeof(szMailContent) + sizeof(byItem));
		m_MailContent->byType = eMailType_Player;
		m_MailContent->nMoney = 123;
		m_MailContent->wTextLen = sizeof(szMailContent);
		m_MailContent->ItemDesc[0].byDataLen = sizeof(byItem);
		memcpy(m_MailContent->byData, szMailContent, sizeof(szMailContent));
		memcpy(m_MailContent->byData + sizeof(szMailContent), byItem, sizeof(byItem));
	}

	~KTestMail(void)
	{
		if (m_MailContent)
		{
			delete (char*)m_MailContent;
		}
		m_MailContent = NULL;
	}

	void SetUp()
	{
		BOOL  bResult  = false;
		int   nRetCode = false;

		m_pGameServerA = new KGameServerSimulator();
		nRetCode = m_pGameServerA->Init();
		ASSERT_TRUE(nRetCode);
		
		m_mt.m_pGS = m_pGameServerA;

		for(int i = 0; i < 4 ; i++)
		{
			TestTools::KRoleInfo& RoleInfo = TestTools::CreateRole();
			ASSERT_TRUE(RoleInfo.dwPlayerID != ERROR_ID);
			m_RoleList.push_back(RoleInfo);
		}

		m_TestRoleA    = &m_RoleList[0];
		m_TestRoleB    = &m_RoleList[1];
		m_TestRoleC    = &m_RoleList[2];
		m_TestRoleD    = &m_RoleList[3];

		for (int i = 0 ; i < 3; i++)
		{
			nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i],
				m_pGameServerA->m_CreatedMapCopy.begin()->dwMapID, m_pGameServerA->m_CreatedMapCopy.begin()->nMapCopy);
			ASSERT_TRUE(nRetCode);
		}
	}

	void TearDown()
	{
		m_pGameServerA->UnInit();
		SAFE_DELETE(m_pGameServerA);
		KGThread_Sleep(100);
		for (int i = 0; i < m_RoleList.size(); i++)
		{
			TestTools::DeleteRole(&m_RoleList[i]);
		}
	}

	KGameServerSimulator* m_pGameServerA;
	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
	ROLE_VECTOR      m_RoleList;

	TestTools::KRoleInfo*       m_TestRoleA;
	TestTools::KRoleInfo*       m_TestRoleB;
	TestTools::KRoleInfo*       m_TestRoleC;
	TestTools::KRoleInfo*       m_TestRoleD;
	char             m_szNoExistRole[_NAME_LEN];

	KMail*   m_MailContent;
	MailTools m_mt;
};


TEST_F(KTestMail, TestSendMail_1)
{
	BOOL bResult  = false;
	int  nRetCode = false;

	CASE_INFO("发送邮件给不存在的玩家", "");
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_szNoExistRole, m_MailContent);
	ASSERT_EQ(eRetCode, mrcDstNotExist);

}

TEST_F(KTestMail, TestSendMail_2)
{
	BOOL bResult  = false;
	int  nRetCode = false;

	CASE_INFO("发送邮件给自己", "");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode = m_mt.SendMail(1, m_TestRoleA->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);
}

TEST_F(KTestMail, TestSendMail_3)
{
	CASE_INFO("发送邮件给不在线的玩家", "");
	
	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));
	
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);
}

TEST_F(KTestMail, TestSendMail_4)
{
	CASE_INFO("发送邮件给在线的玩家", "");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);
}

TEST_F(KTestMail, TestSendMail_5)
{
	CASE_INFO("发送邮件给在线的玩家", "玩家邮箱已满");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	for (int i = 0; i < GC->GCSettings()->m_nMaxPlayerMailCount; i++)
	{
		ASSERT_EQ(m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent), mrcSucceed);
	}

	ASSERT_EQ(m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent), mrcMailBoxFull);
}

TEST_F(KTestMail, TestSendMail_6)
{
	CASE_INFO("发送邮件", "不存在的玩家");

	strcpy(m_MailContent->szSenderName, "");

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcFailed);
}

TEST_F(KTestMail, TestGetMailList_1)
{
	CASE_INFO("获取邮件列表玩家不在线,获取邮件列表", "");

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));

	std::vector<KMailListInfo>& MailList =  m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_TRUE(MailList.empty());
}

TEST_F(KTestMail, TestGetMailList_2)
{
	CASE_INFO("玩家在线,获取邮件列表", "");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	for (int i = 0; i < 20; i++)
	{
		ASSERT_EQ(m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent), mrcSucceed);
	}

	std::vector<KMailListInfo>& MailList =  m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailList.size(), 20);
}

TEST_F(KTestMail, TestGetMailConnent_1)
{
	CASE_INFO("获取邮件内容玩家不在线, 获取邮件内容", "");

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));

	ASSERT_EQ(m_mt.QueryMailContent(m_TestRoleB->dwPlayerID, ERROR_ID, eMailType_Player), mrcFailed);
}

TEST_F(KTestMail, TestGetMailConnent_2)
{
	CASE_INFO("系统没有玩家邮件信息获取邮件内容", "");

	ASSERT_EQ(m_mt.QueryMailContent(m_TestRoleB->dwPlayerID, ERROR_ID, eMailType_Player), mrcFailed);
}

TEST_F(KTestMail, TestGetMailConnent_4)
{
	CASE_INFO("系统存在玩家邮件内容获取邮件内容", "");
	byte Buffer[1024];
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = rand() % 100 + 10;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	KMailContent* pMailContent = (KMailContent*)Buffer;
	ASSERT_EQ(m_mt.QueryMailContent(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, eMailType_Player, pMailContent), mrcSucceed);
	ASSERT_EQ(pMailContent->nMoney, m_MailContent->nMoney);
}

TEST_F(KTestMail, TestGetMailAttachment_1)
{
	CASE_INFO("玩家不在线,GetMailAttachment", "");

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));

	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, ERROR_ID, 0, NULL), mrcFailed);
}

TEST_F(KTestMail, TestGetMailAttachment_2)
{
	CASE_INFO("系统没有玩家邮件信息GetMailAttachment", "");

	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, ERROR_ID, 0, NULL), mrcFailed);
}

TEST_F(KTestMail, TestGetMailAttachment_3)
{
	CASE_INFO("系统存在玩家邮件GetMailAttachment", "错误的index");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = rand() % 100 + 10;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, KMAIL_MAX_ITEM_COUNT + 1, NULL), mrcFailed);

	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, KMAIL_MAX_ITEM_COUNT  - 1, NULL), mrcFailed);
}

TEST_F(KTestMail, TestGetMailAttachment_4)
{
	CASE_INFO("系统存在玩家邮件GetMailAttachment", "index");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = rand() % 100 + 10;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	byte byBuffer[1024];
	R2S_GIVE_MAIL_ITEM_TO_PLAYER* pRespond  = (R2S_GIVE_MAIL_ITEM_TO_PLAYER*)byBuffer;
	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID,  0, pRespond), mrcSucceed);
}

TEST_F(KTestMail, TestGetMailAttachment_5)
{
	CASE_INFO("系统存在玩家邮件GetMailAttachment", "在锁定时间里再次获取");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = rand() % 100 + 10;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	byte byBuffer[1024];
	R2S_GIVE_MAIL_ITEM_TO_PLAYER* pRespond  = (R2S_GIVE_MAIL_ITEM_TO_PLAYER*)byBuffer;
	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID,  0, pRespond), mrcSucceed);
	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID,  0, pRespond), mrcFailed);
}

TEST_F(KTestMail, TestGetMailAttachment_6)
{
	CASE_INFO("系统存在玩家邮件GetMailAttachment", "超过锁定时间");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = rand() % 100 + 10;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	byte byBuffer[1024];
	R2S_GIVE_MAIL_ITEM_TO_PLAYER* pRespond  = (R2S_GIVE_MAIL_ITEM_TO_PLAYER*)byBuffer;
	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID,  0, pRespond), mrcSucceed);
	GC->AccelerationGCTime(60 * 10 + 1);
	ASSERT_EQ(m_mt.AcquiteMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID,  0, pRespond), mrcSucceed);
}

TEST_F(KTestMail, TestReleaseMailItem_1)
{
	CASE_INFO("relaseItem", "offline player");

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));

	ASSERT_EQ(m_mt.ReleaseMailItem(m_TestRoleB->dwPlayerID, ERROR_ID, ERROR_ID), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailItem_2)
{
	CASE_INFO("relaseItem", "Error ID ");

	ASSERT_EQ(m_mt.ReleaseMailItem(m_TestRoleB->dwPlayerID, ERROR_ID, ERROR_ID), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailItem_3)
{
	CASE_INFO("relaseItem", "Error index");
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_EQ(m_mt.ReleaseMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, 10), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailItem_4)
{
	CASE_INFO("relaseItem", "right index");
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_EQ(m_mt.ReleaseMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, KMAIL_MAX_ITEM_COUNT - 1), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailItem_5)
{
	CASE_INFO("relaseItem", "right index");
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);

	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	byte byBuffer[1024];
	KMailContent* pContent = (KMailContent*)byBuffer;
	ASSERT_EQ(m_mt.ReleaseMailItem(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, 0, pContent), mrcSucceed);
	ASSERT_TRUE(pContent->ItemDesc[0].bAcquired);
}

TEST_F(KTestMail, TestDeleteMail_1)
{
	CASE_INFO("玩家不在线删除邮件", "");

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));

	ASSERT_TRUE(m_mt.DeleteMail(m_TestRoleB->dwPlayerID, ERROR_ID), mrcFailed);
}

TEST_F(KTestMail, TestDeleteMail_2)
{
	CASE_INFO("系统没有玩家邮件ID号删除邮件", "");

	ASSERT_TRUE(m_mt.DeleteMail(m_TestRoleB->dwPlayerID, ERROR_ID), mrcFailed);
}

TEST_F(KTestMail, TestDeleteMail_3)
{
	CASE_INFO("系统存在玩家邮件删除邮件", "");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_TRUE(m_mt.DeleteMail(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID), mrcFailed);
}

TEST_F(KTestMail, TestUserReturnMail_1)
{
	CASE_INFO("玩家不在线玩家退信", "");

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleB));

	ASSERT_TRUE(m_mt.UserReturnMail(m_TestRoleB->dwPlayerID, ERROR_ID));
}

TEST_F(KTestMail, TestUserReturnMail_2)
{
	CASE_INFO("玩家在线,系统存在邮件玩家退信", "");
	ASSERT_TRUE(m_mt.UserReturnMail(m_TestRoleB->dwPlayerID, ERROR_ID));
}

TEST_F(KTestMail, TestUserReturnMail_3)
{
	CASE_INFO("玩家在线,系统不存在邮件玩家退信", "");

	ASSERT_TRUE(m_mt.UserReturnMail(ERROR_ID, ERROR_ID));
}

TEST_F(KTestMail, TestUserReturnMail_4)
{
	CASE_INFO("玩家在线", "玩家退信");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_EQ(m_mt.UserReturnMail(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID), mrcSucceed);
}

TEST_F(KTestMail, TestUserReturnMail_5)
{
	CASE_INFO("玩家在线", "玩家退信");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_TRUE(TestTools::PlayerLeaveGame(m_TestRoleA));
	ASSERT_TRUE(TestTools::DeleteRole(m_TestRoleA));
	ASSERT_EQ(m_mt.UserReturnMail(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailMoney_1)
{
	CASE_INFO("玩家在线", "更新邮件Money");

	ASSERT_EQ(m_mt.ReleaseMailMoney(ERROR_ID, ERROR_ID, NULL), mrcFailed);
	ASSERT_EQ(m_mt.ReleaseMailMoney(m_TestRoleB->dwPlayerID, ERROR_ID, NULL), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailMoney_2)
{
	CASE_INFO("玩家在线", "更新没钱邮件");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = 0;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	ASSERT_EQ(m_mt.ReleaseMailMoney(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, NULL), mrcFailed);
}

TEST_F(KTestMail, TestReleaseMailMoney_3)
{
	CASE_INFO("玩家在线", "更新有钱的邮件");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = 5;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	byte byBuffer[1024];
	KMailContent* pContent = (KMailContent*)byBuffer;
	ASSERT_EQ(m_mt.ReleaseMailMoney(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID, pContent), mrcSucceed);
	ASSERT_EQ(pContent->nMoney, 0);
}

TEST_F(KTestMail, TestSetMailRead_1)
{
	CASE_INFO("玩家在线,设置邮件已读", "Error ID");
	m_mt.SetMailRead(m_TestRoleB->dwPlayerID, ERROR_ID);
}


TEST_F(KTestMail, TestSetMailRead_2)
{
	CASE_INFO("玩家在线,设置邮件已读", "设置邮件");
	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = 5;
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);

	m_mt.SetMailRead(m_TestRoleB->dwPlayerID, MailIDList[0].dwMailID);

	MailIDList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);
	ASSERT_TRUE(MailIDList[0].bReadFlag);
}

TEST_F(KTestMail, TestWithDrawMail)
{
	CASE_INFO("玩家在线,设置邮件已读", "");

	strcpy(m_MailContent->szSenderName, m_TestRoleA->szRoleName);
	m_MailContent->nMoney = 5;
	m_MailContent->nRecvTime = time(NULL);
	KMAIL_RESULT_CODE eRetCode  =  m_mt.SendMail(1, m_TestRoleB->szRoleName, m_MailContent);
	ASSERT_EQ(eRetCode, mrcSucceed);

	GC->AccelerationGCTime(2 * GC->GCSettings()->m_nMailSurvivalTime);
	KGThread_Sleep(500);
	GC->AccelerationGC(GAME_FPS);
	KGThread_Sleep(2000);

	std::vector<KMailListInfo>& MailIDList = m_mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_EQ(MailIDList.size(), 1);
}

TEST_F(KTestMail, SendGobalMail_1)
{
	std::stringstream sql;
	IKG_Buffer* pBuff = NULL;

	CASE_INFO("发送系统邮件", "发送成功， 数据库保存此邮件");

	g_DBManager.ClearTable(GLOBAL_MAIL_TABLE_NAME);

	m_mt.DoSendGloalMail(1, m_MailContent);

	KGThread_Sleep(1000);

	sql << "select MailIndex, EndTime, MailInfo from "
		<< GLOBAL_MAIL_TABLE_NAME;

	BufferScopedPrt Buff(g_DBManager.QueyRueslt(sql.str().c_str()));
	ASSERT_TRUE(Buff.get());
}

TEST_F(KTestMail, ReceiveGolbalMail_1)
{
	BOOL bResult  = false;
	int  nRetCode = false;
	std::vector<KMailListInfo> MailList;

	CASE_INFO("发送系统邮件", "在系统邮件时间里");

	nRetCode = g_DBManager.ClearTable(GLOBAL_MAIL_TABLE_NAME);
	ASSERT_TRUE(nRetCode);

	strcpy(m_MailContent->szTitle, "testGolabMail");
	m_mt.DoSendGloalMail(1, m_MailContent);

	KGThread_Sleep(1000);

	MailList = m_mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_TRUE(!MailList.empty());

	size_t i;
	for (i = 0; i < MailList.size(); i++)
	{
		if (strcmp(MailList[i].szTitle, "testGolabMail") == 0)
		{
			break;
		}
	}
	ASSERT_TRUE(i < MailList.size());
	MailList.clear();

	MailList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_TRUE(!MailList.empty());

	for (i = 0; i < MailList.size(); i++)
	{
		if (strcmp(MailList[i].szTitle, "testGolabMail") == 0)
		{
			break;
		}
	}
	ASSERT_TRUE(i < MailList.size());
}

TEST_F(KTestMail, ReceiveGolbalMail_2)
{
	std::vector<KMailListInfo> MailList;
	std::stringstream sql;

	CASE_INFO("发送系统邮件", "发送成功， 数据库保存此邮件");


	for(int i = 0; i < MAIL_TITLE_LEN; i++)
		m_MailContent->szTitle[i] = rand()% 252 +1;
	m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = '\0';

	m_mt.DoSendGloalMail(1, m_MailContent);

	KGThread_Sleep(1000);

	TestTools::PlayerEnterGame(m_pGameServerA, m_TestRoleB, m_pGameServerA->m_CreatedMapCopy[0].dwMapID,
		m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);

	MailList = m_mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_FALSE(MailList.empty());

	size_t i;
	for (i = 0; i < MailList.size(); i++)
	{
		if (strcmp(MailList[i].szTitle, m_MailContent->szTitle) == 0)
		{
			break;
		}
	}
	ASSERT_TRUE(i < MailList.size());
}


TEST_F(KTestMail, ReceiveGolbalMail_3)
{
	bool nRetCode = false;
	std::vector<KMailListInfo> MailList;

	CASE_INFO("创建新角色", "有正在接受时间限制的系统邮件， 玩家在线，邮件已被接受");

	for(int i = 0; i < MAIL_TITLE_LEN; i++)
		m_MailContent->szTitle[i] = rand()% 252 +1;
	m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = '\0';

	m_mt.DoSendGloalMail(1, m_MailContent);
	
	KGThread_Sleep(500);

	TestTools::KRoleInfo Role = TestTools::CreateRole();
	ASSERT_TRUE(Role.dwPlayerID != 0);
	m_RoleList.push_back(Role);

	nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &Role, m_pGameServerA->m_CreatedMapCopy[0].dwMapID,
		m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
	ASSERT_TRUE(nRetCode);

	MailList = m_mt.GetMailList(Role.dwPlayerID);
	ASSERT_TRUE(!MailList.empty());

	size_t i;
	for (i = 0; i < MailList.size(); i++)
	{
		if (strcmp(MailList[i].szTitle, m_MailContent->szTitle) == 0)
		{
			break;
		}
	}
	ASSERT_TRUE(i < MailList.size());
}


TEST_F(KTestMail, ReceiveGolbalMail_4)
{
	BOOL bResult  = false;
	int  nRetCode = false;
	std::vector<KMailListInfo> MailList;

	CASE_INFO("玩家在系统邮件时间限制内未收取邮件", "玩家收取邮件");

	for(int i = 0; i < MAIL_TITLE_LEN; i++)
		m_MailContent->szTitle[i] = rand()% 252 +1;
	m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = '\0';

	m_mt.DoSendGloalMail(1, m_MailContent);

	KGThread_Sleep(500);
	GC->AccelerationGCTime(60);
	GC->ManualActivate();

	size_t i;
	for (i = 0; i < MailList.size(); i++)
	{
		if (strcmp(MailList[i].szTitle, m_MailContent->szTitle) == 0)
		{
			break;
		}
	}
	ASSERT_EQ(i, MailList.size());
}

TEST_F(KTestMail, ReceiveGolbalMail_5)
{
	BOOL bResult  = false;
	int  nRetCode = false;
	std::vector<KMailListInfo> MailList;
	std::vector<std::string> StringList;

	CASE_INFO("系统存在多封在接受时间内的系统邮件，玩家在线", "玩家收取邮件");

	for (int i = 0; i < 5; i++)
	{
		for(int i = 0; i < MAIL_TITLE_LEN - 1; i++)
			m_MailContent->szTitle[i] = rand()% 252 +1;
		m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = '\0';
		
		m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = 0;
		StringList.push_back(m_MailContent->szTitle);
		m_mt.DoSendGloalMail(1, m_MailContent);
	}
	KGThread_Sleep(2000);

	MailList = m_mt.GetMailList(m_TestRoleA->dwPlayerID);
	for (int i = 0; i < MailList.size(); i++)
	{
		for (size_t n = 0; n < StringList.size(); n++)
		{
			if (strcmp(MailList[i].szTitle, StringList[n].c_str()) == 0)
			{
				StringList.erase(StringList.begin() + n);
				break;
			}
		}
	}
	ASSERT_TRUE(StringList.empty());
}


TEST_F(KTestMail, DelectGolbalMail_1)
{
	BOOL bResult  = false;
	int  nRetCode = false;
	std::vector<KMailListInfo> MailList;
	IKG_Buffer* pBuffer = NULL;
	std::stringstream sql;

	CASE_INFO("系统邮件", "到系统邮件存活时间");

	for(int i = 0; i < MAIL_TITLE_LEN; i++)
		m_MailContent->szTitle[i] = rand()% 252 +1;
	m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = '\0';

	m_mt.DoSendGloalMail(1, m_MailContent);

	GC->AccelerationGCTime(60);
	GC->ManualActivate();
	KGThread_Sleep(500);

	sql << "select MailIndex from "
		<< GLOBAL_MAIL_TABLE_NAME;

	BufferScopedPrt Buffer(g_DBManager.QueyRueslt(sql.str().c_str()));
	ASSERT_FALSE(Buffer.get());
}

TEST_F(KTestMail, DelectGolbalMail_2)
{
	BOOL bResult  = false;
	int  nRetCode = false;
	std::vector<KMailListInfo> MailList;
	IKG_Buffer* pBuffer = NULL;
	std::stringstream sql;

	CASE_INFO("系统邮件", "没到系统邮件存活时间");

	for(int i = 0; i < MAIL_TITLE_LEN; i++)
		m_MailContent->szTitle[i] = rand()% 252 +1;
	m_MailContent->szTitle[MAIL_TITLE_LEN - 1] = '\0';

	m_mt.DoSendGloalMail(1000, m_MailContent);

	
	
	GC->AccelerationGCTime(62);
	GC->ManualActivate();
	KGThread_Sleep(1000);

	sql << "select MailInfo from "
		<< GLOBAL_MAIL_TABLE_NAME;

	BufferScopedPrt Buffer(g_DBManager.QueyRueslt(sql.str().c_str()));
	ASSERT_FALSE(Buffer.get());
}