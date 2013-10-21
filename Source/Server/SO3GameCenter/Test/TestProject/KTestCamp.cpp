#include "StdAfx.h"
#include "KGameServerSimulator.h"
#include "KGatewaySimulator.h"
#include "KGameCenterWrapper.h"
#include "testtools.h"
#include "Utility.h"
#include "CampTools.h"
#include "KTestDBManager.h"
#include "gtest/gtest.h"


class KTestCamp :  public testing::Test 
{
protected:
	KTestCamp::KTestCamp(void)
	{
		m_pGameServerA = NULL;
	}

	KTestCamp::~KTestCamp(void)
	{
	}

	void KTestCamp::SetUp()
	{
		int nResult = false;
		int nRetCode  = false;
		m_pGameServerA = new KGameServerSimulator();
		nRetCode = m_pGameServerA->Init();
		ASSERT_TRUE(nRetCode);
		m_CT.m_pGS = m_pGameServerA;
		m_CT.GetLevelScore(m_LevelScore);
		m_CT.SetScore(0);
		m_CT.SetLevel(INIT_CAMP_LEVEL);
	}

	void KTestCamp::TearDown()
	{
		SAFE_DELETE(m_pGameServerA);
	}

protected:
	KGameServerSimulator* m_pGameServerA;
	CampTools m_CT;
	int m_LevelScore[CAMP_LEVEL_COUNT];
};


TEST_F(KTestCamp, AddScore_1)
{
	CASE_INFO("加分", "加分范围在最大范围类");
	int nSroce  = m_CT.GetScore();
	int nLevel  = m_CT.GetLevel();
	int nNewSroce = m_CT.AddScore(m_LevelScore[CAMP_LEVEL_COUNT - 1]);
	ASSERT_TRUE(nNewSroce != nSroce);
}

TEST_F(KTestCamp, AddScore_2)
{
	CASE_INFO("加分", "加分范围超出最大范围类");
	int nTotalSroce = 0;
	for (int i = 0; i < CAMP_LEVEL_COUNT; i++)
	{
		nTotalSroce += m_LevelScore[i];
	}
	nTotalSroce += 1;

	int nNewSroce = m_CT.AddScore(nTotalSroce);
	ASSERT_EQ(m_CT.GetScore(), m_LevelScore[CAMP_LEVEL_COUNT - 1]);
	ASSERT_EQ(m_CT.GetLevel(), CAMP_LEVEL_COUNT - 1);
}


TEST_F(KTestCamp, MinusScore_1)
{
	CASE_INFO("减分", "加分范围在最大范围类");
	int nSroce  = m_CT.GetScore();
	int nLevel  = m_CT.GetLevel();
	int nNewSroce = m_CT.AddScore(-m_LevelScore[CAMP_LEVEL_COUNT - 1]);
	ASSERT_TRUE(nNewSroce != nSroce);
}

TEST_F(KTestCamp, MinusScore_2)
{
	CASE_INFO("减分", "加分范围超出最大范围类");
	int nTotalSroce = 0;
	for (int i = 0; i < CAMP_LEVEL_COUNT; i++)
	{
		nTotalSroce += m_LevelScore[i];
	}
	nTotalSroce += 1;

	int nNewSroce = m_CT.AddScore(-nTotalSroce);
	ASSERT_EQ(m_CT.GetScore(), -m_LevelScore[CAMP_LEVEL_COUNT - 1]);
	ASSERT_EQ(m_CT.GetLevel(), 0);
}

TEST_F(KTestCamp, SroceSave)
{
	CASE_INFO("阵营数据保存", "数据保存");

	m_CT.AddScore(m_LevelScore[CAMP_LEVEL_COUNT - 1]);
	int nSroce  = m_CT.GetScore();
	int nLevel  = m_CT.GetLevel();
	
	int nTotalScore = nSroce;
	if (nLevel > INIT_CAMP_LEVEL)
	{
		while (nLevel-- > INIT_CAMP_LEVEL)
		{
			nTotalScore += m_LevelScore[nLevel];
		}
	} 
	else if (nLevel < INIT_CAMP_LEVEL)
	{
		while (nLevel++ < INIT_CAMP_LEVEL)
		{
			nTotalScore -= m_LevelScore[CAMP_LEVEL_COUNT - nLevel - 1];
		}
	}

	GC->AccelerationMultipleGC(GAME_FPS);
	GC->ManualActivate();
	KGThread_Sleep(1000);

	std::string szSQL = "select value from GlobalSystemValue where name = 'camp_score'";
	BufferScopedPrt DBSroce(g_DBManager.QueyRueslt(szSQL.c_str()));
	ASSERT_TRUE(DBSroce.get());
	ASSERT_EQ(nTotalScore, strtol((char*)DBSroce->GetData(), NULL, 10));
}