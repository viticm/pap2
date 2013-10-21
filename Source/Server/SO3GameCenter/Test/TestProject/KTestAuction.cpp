#include "StdAfx.h"
#include "KTestDBManager.h"
#include "KGameServerSimulator.h"
#include "KGameCenterWrapper.h"
#include "testtools.h"
#include "MailTools.h"
#include "gtest/gtest.h"
#include "Common/CRC32.h"

class KTestAuction :  public testing::Test 
{
protected:
KTestAuction(void)
{
	m_pGameServerA = NULL;
}

~KTestAuction(void)
{
}

void SetUp()
{
	BOOL  bResult  = false;
	int   nRetCode = false;

	m_pGameServerA = new KGameServerSimulator();
	nRetCode = m_pGameServerA->Init();
	ASSERT_TRUE(nRetCode);

	for(int i = 0; i < 10 ; i++)
	{
		TestTools::KRoleInfo& Role = TestTools::CreateRole();
		ASSERT_TRUE(Role.dwPlayerID != ERROR_ID);
		m_RoleList.push_back(Role);
	}

	m_TestRoleA    = &m_RoleList[0];
	m_TestRoleB = &m_RoleList[1];

	for (int i = 0; i <3; i++)
	{
		nRetCode = TestTools::PlayerEnterGame(m_pGameServerA, &m_RoleList[i], m_pGameServerA->m_CreatedMapCopy[0].dwMapID,
			m_pGameServerA->m_CreatedMapCopy[0].nMapCopy);
		ASSERT_TRUE(nRetCode);
	}
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	memset(m_szBuffer2, 0, sizeof(m_szBuffer2));
}

void TearDown()
{
	SAFE_DELETE(m_pGameServerA);
	KGThread_Sleep(200);
	for (int i = 0; i < m_RoleList.size(); ++i)
	{
		TestTools::DeleteRole(&m_RoleList[i]);
	}
	g_DBManager.ClearTable("Auction");
}
int CRC(int nSize)
{
	char szBuffer[1024] = {0};
	return CRC32(0, szBuffer, nSize);
}
protected:
	KGameServerSimulator* m_pGameServerA;

	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
	ROLE_VECTOR      m_RoleList;

	TestTools::KRoleInfo*       m_TestRoleA;
	TestTools::KRoleInfo*       m_TestRoleB;
	char m_szBuffer[1024];
	char m_szBuffer2[10240];
};

TEST_F(KTestAuction,TestSell_1)
{
	int nRetCode = false;
	int nResult = false;
	char m_szBuffer[1024];
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;

	CASE_INFO("玩家寄卖物品", "非法数据");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	strcpy(pAuctionSellRequset->szSaleName, "test");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	strcpy(pAuctionSellRequset->szSaleName, "test");
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	strcpy(pAuctionSellRequset->szSaleName, "test");
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	strcpy(pAuctionSellRequset->szSaleName, "test");
	pAuctionSellRequset->dwSellerID = 100;
	pAuctionSellRequset->uItemDataLen = 1;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	strcpy(pAuctionSellRequset->szSaleName, "test");
	pAuctionSellRequset->dwSellerID = 100;
	pAuctionSellRequset->uItemDataLen = 1;

	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	strcpy(pAuctionSellRequset->szSaleName, "test");
	pAuctionSellRequset->dwSellerID = 100;
	pAuctionSellRequset->uItemDataLen = 1;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	KGThread_Sleep(1000);
}

TEST_F(KTestAuction,TestSell_2)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "玩家寄卖物品");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(R2S_AUCTION_LOOKUP_RESPOND));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->nPrice == pAuctionSellRequset->nStartPrice);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->nBuyItNowPrice == pAuctionSellRequset->nBuyItNowPrice);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize == pAuctionSellRequset->uItemDataLen);
}

TEST_F(KTestAuction,TestLookUp_1)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家查看寄卖物品", "系统无物品物品");

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(R2S_AUCTION_LOOKUP_RESPOND));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 0);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.uSize == 0);
}


TEST_F(KTestAuction,TestLookUp_2)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "玩家寄卖物品");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	for (int i = 0; i < 51; i++)
		m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(R2S_AUCTION_LOOKUP_RESPOND));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 51);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.uSize == (sizeof(KAUCTION_PAGE_NODE) + pAuctionSellRequset->uItemDataLen) * 50);
}


TEST_F(KTestAuction,TestLookUp_3)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有最小等级需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nRequireLevel = 10;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nRequireLevel = 20;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nLevelMin = 15;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==2);
}

TEST_F(KTestAuction,TestLookUp_4)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有最大等级需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nRequireLevel = 10;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nRequireLevel = 20;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nLevelMax = 15;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==1);
}


TEST_F(KTestAuction,TestLookUp_5)
{
	bool nRetCode = false;
	bool nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有最小最大等级需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nRequireLevel = 10;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nRequireLevel = 20;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nRequireLevel = 22;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nLevelMin = 15;
	pAuctionLookUpRequest->Param.nLevelMax = 21;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==2);
}


TEST_F(KTestAuction,TestLookUp_6)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有sub类别需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nAucSub = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nAucSub = 6;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nAucSub = 7;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nAucSub = 6;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==2);
}


TEST_F(KTestAuction,TestLookUp_7)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有大类类别需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nAucGenre = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nAucGenre = 6;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nAucGenre = 7;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nAucGenre = 6;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==2);
}

TEST_F(KTestAuction,TestLookUp_8)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有Itme名称需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	strcpy(pAuctionSellRequset->szSaleName, "TestItem2");
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	strcpy(pAuctionSellRequset->szSaleName, "TestItem3");
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	strcpy(pAuctionLookUpRequest->Param.szSaleName, "TestItem3");
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==3);
}

TEST_F(KTestAuction,TestLookUp_9)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有品质需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nAucGenre = 5;
	pAuctionSellRequset->nQuality = 1;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nQuality = 2;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nQuality = 3;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nQuality = 2;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 2);
}

TEST_F(KTestAuction,TestLookUp_10)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有买家姓名需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 100;
	pAuctionSellRequset->nStartPrice = 1000;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nAucGenre = 5;
	pAuctionSellRequset->nQuality = 1;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nQuality = 2;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nQuality = 3;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nQuality = -1;
	strcpy(pAuctionLookUpRequest->Param.szSellerName, m_TestRoleA->szRoleName);
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 3);
	ASSERT_TRUE(((KAUCTION_PAGE_NODE*)pAuctionLookUpRespond->Page.bySaleList)->uItemSize ==1);
}


TEST_F(KTestAuction,TestLookUp_11)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有最高价格需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nStartPrice = 10;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nStartPrice = 20;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nQuality = -1;
	pAuctionLookUpRequest->Param.nTopPrice = 5;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
}

TEST_F(KTestAuction,TestLookUp_12)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家寄卖物品", "寄卖物品有已出价需求");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nStartPrice = 10;
	pAuctionSellRequset->uItemDataLen = 2;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	pAuctionSellRequset->nStartPrice = 20;
	pAuctionSellRequset->uItemDataLen = 3;
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 200, CRC(1));
	memset(pAuctionLookUpRequest, 0, sizeof(m_szBuffer2));
	pAuctionLookUpRequest->dwPlayerID = m_TestRoleB->dwPlayerID;
	pAuctionLookUpRequest->Param.nQuality = -1;
	pAuctionLookUpRequest->Param.byAuction = 1;
	m_pGameServerA->DoAuctionLookupRequest(*pAuctionLookUpRequest);

	memset(pAuctionLookUpRespond, 0, sizeof(m_szBuffer2));
	nRetCode = m_pGameServerA->CheckAuctionLookUpRespond(*pAuctionLookUpRespond);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(pAuctionLookUpRespond->byCode == arcSucceed);
	ASSERT_TRUE(pAuctionLookUpRespond->Page.nTotalCount == 1);
}

TEST_F(KTestAuction,TestBid_1)
{
	int nRetCode = false;
	int nResult = false;

	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "拍卖价格低于售价");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 4, CRC(1));

	nRetCode = m_pGameServerA->CheckAuctionBidRespond(m_TestRoleB->dwPlayerID, arcPriceTooLow, 4);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestAuction,TestBid_2)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "出价比已有玩家出价高");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 10;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 10, CRC(1));

	nRetCode = m_pGameServerA->CheckAuctionBidRespond(m_TestRoleB->dwPlayerID, arcSucceed, 10);
	ASSERT_TRUE(nRetCode);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleA->dwPlayerID, 1, 11, CRC(1));

	nRetCode = m_pGameServerA->CheckAuctionBidRespond(m_TestRoleA->dwPlayerID, arcSucceed, 11);
	ASSERT_TRUE(nRetCode);
	
	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo>& MailList = mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);

	ASSERT_EQ(mt.QueryMailContent(m_TestRoleB->dwPlayerID, MailList[0].dwMailID, eMailType_Auction, pMailContent), mrcSucceed);
	ASSERT_TRUE(pMailContent->nMoney == 10);
}

TEST_F(KTestAuction,TestBid_3)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "第一次出道一口价");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
		pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 1000, CRC(1));

	nRetCode = m_pGameServerA->CheckAuctionBidRespond(m_TestRoleB->dwPlayerID, arcSucceed, 1000);
	ASSERT_TRUE(nRetCode);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo> MailList = mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);

	ASSERT_EQ(mt.QueryMailContent(m_TestRoleA->dwPlayerID, MailList[0].dwMailID, eMailType_Auction, pMailContent), mrcSucceed);
	ASSERT_EQ(pMailContent->nMoney, 1000 - 1000 / 20);
}

TEST_F(KTestAuction,TestBid_4)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "寄卖时间到，没有其他玩家出价");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 0;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 500, CRC(1));

	nRetCode = m_pGameServerA->CheckAuctionBidRespond(m_TestRoleB->dwPlayerID, arcSucceed, 500);
	ASSERT_TRUE(nRetCode);

	GC->AccelerationGCTime(10 * 60);

	KGThread_Sleep(500);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo>& MailList = mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);

	ASSERT_EQ(mt.QueryMailContent(m_TestRoleA->dwPlayerID, MailList[0].dwMailID, eMailType_Auction, pMailContent), mrcSucceed);
	ASSERT_EQ(pMailContent->nMoney, 500 - 500 / 20);
}


TEST_F(KTestAuction,TestBid_5)
{
	int nRetCode = false;
	int nResult = false;


	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "寄卖时间到，没有玩家出价");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 0;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	GC->AccelerationGCTime(10 * 60);
	KGThread_Sleep(100);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo>& MailList = mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);
}


TEST_F(KTestAuction,TestBid_6)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "寄卖时间到，玩家删除角色");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 0;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);
	
	KGThread_Sleep(1000);
	
	TestTools::KRoleInfo Role  = *m_TestRoleA;
	nRetCode = TestTools::PlayerLeaveGame(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::DeleteRole(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	GC->AccelerationGCTime(10 * 60);
	KGThread_Sleep(100);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo>& MailList = mt.GetMailList(Role.dwPlayerID);
	ASSERT_TRUE(MailList.empty());
}


TEST_F(KTestAuction,TestBid_7)
{
	int nRetCode = false;
	int nResult = false;

	KMail* pMailContent = (KMail*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "其他玩家拍到物品，玩家删除角色");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	KGThread_Sleep(500);

	nRetCode = TestTools::PlayerLeaveGame(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::DeleteRole(m_TestRoleA);
	ASSERT_TRUE(nRetCode);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 1000, CRC(1));

	nRetCode = m_pGameServerA->CheckAuctionBidRespond(m_TestRoleB->dwPlayerID, arcSucceed, 1000);
	ASSERT_TRUE(nRetCode);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo>& MailList = mt.GetMailList(m_TestRoleB->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);
}


TEST_F(KTestAuction,TestBid_8)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "其他玩家拍到物品，玩家删除角色");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 10;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctioBidRequest(m_TestRoleB->dwPlayerID, 1, 1000, CRC(1));	
	
	nRetCode = TestTools::PlayerLeaveGame(m_TestRoleB);
	ASSERT_TRUE(nRetCode);

	nRetCode = TestTools::DeleteRole(m_TestRoleB);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(500);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo> MailList = mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);

	ASSERT_EQ(mt.QueryMailContent(m_TestRoleA->dwPlayerID, MailList[0].dwMailID, eMailType_Auction, pMailContent), mrcSucceed);
	ASSERT_EQ(pMailContent->nMoney,  1000 - 1000 / 20);
}

TEST_F(KTestAuction,TestCancleSell)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "玩家取消寄卖");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 0;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctionCancelRequest(m_TestRoleA->dwPlayerID, 1);

	nRetCode = m_pGameServerA->CheckAuctionCancelRespond(m_TestRoleA->dwPlayerID, arcSucceed);
	ASSERT_TRUE(nRetCode);

	MailTools mt;
	mt.m_pGS = m_pGameServerA;

	std::vector<KMailListInfo> MailList = mt.GetMailList(m_TestRoleA->dwPlayerID);
	ASSERT_EQ(MailList.size(), 1);
}

TEST_F(KTestAuction, TestCancleSell_1)
{
	int nRetCode = false;
	int nResult = false;
	CASE_INFO("玩家拍卖物品", "玩家取消寄卖");


	m_pGameServerA->DoAuctionCancelRequest(m_TestRoleA->dwPlayerID, 1);

	nRetCode = m_pGameServerA->CheckAuctionCancelRespond(m_TestRoleA->dwPlayerID, arcItemNotExist);
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestAuction, TestCancleSell_2)
{
	int nRetCode = false;
	int nResult = false;

	KMailContent* pMailContent = (KMailContent*)m_szBuffer2;
	S2R_AUCTION_SELL_REQUEST* pAuctionSellRequset = (S2R_AUCTION_SELL_REQUEST*)m_szBuffer;
	S2R_AUCTION_LOOKUP_REQUEST* pAuctionLookUpRequest = (S2R_AUCTION_LOOKUP_REQUEST*)m_szBuffer2;
	R2S_AUCTION_LOOKUP_RESPOND* pAuctionLookUpRespond = (R2S_AUCTION_LOOKUP_RESPOND*)m_szBuffer2;

	CASE_INFO("玩家拍卖物品", "玩家取消非自己寄卖物品");

	memset(pAuctionSellRequset, 0, sizeof(m_szBuffer));
	pAuctionSellRequset->dwSellerID = m_TestRoleA->dwPlayerID;
	pAuctionSellRequset->uItemDataLen = 1;
	pAuctionSellRequset->nBuyItNowPrice = 1000;
	pAuctionSellRequset->nStartPrice = 100;
	pAuctionSellRequset->nLeftTime = 0;
	pAuctionSellRequset->nStartPrice = 5;
	strcpy(pAuctionSellRequset->szSaleName, "TestItem1");
	m_pGameServerA->DoAuctionSellRequest(*pAuctionSellRequset);

	m_pGameServerA->DoAuctionCancelRequest(m_TestRoleB->dwPlayerID, 1);

	nRetCode = m_pGameServerA->CheckAuctionCancelRespond(m_TestRoleB->dwPlayerID, arcUnknownError);
	ASSERT_TRUE(nRetCode);
}