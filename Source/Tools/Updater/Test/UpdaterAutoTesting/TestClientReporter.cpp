#include "TestClientReporter.h"
#include "KClientReport.h"
#include "atlstr.h"

const int GAME_NUM = 16;
const TCHAR* GAME_NAME[] = 
{
	"SD敢达Online", "春秋Q传online", "剑1", "剑2", "剑侠世界", "梦幻西游", "魔兽世界", "魔域",
	"奇迹世界", "天下2", "新龙骑士Online", "永恒之塔", "诛仙", "地下城与勇士", "热血传奇", "仙剑online"
};


void TestClientReporter::setUp()
{

}

void TestClientReporter::tearDown()
{

}

void TestClientReporter::KTestFullRun_01()
{
	int nResult  = false;
	int nRetCode = false; 
	CString strOtherGameFlag;
	
	KClientReport reporter;
	reporter.SerchClient();
	strOtherGameFlag = reporter.GetOtherGameFlag();
	for (int i = 0; i < GAME_NUM; i++)
	{
		KGLogPrintf(KGLOG_INFO, "%s: %c", GAME_NAME[i], strOtherGameFlag.GetAt(i));
	}
	//printf(strOtherGameFlag);



	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}