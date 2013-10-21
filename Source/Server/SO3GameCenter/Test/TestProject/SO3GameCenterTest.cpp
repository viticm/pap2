#include "stdafx.h"
#include <conio.h>
#include "KGatewaySimulator.h"
#include "KTestDBManager.h"
#include "TestDef.h"
#include "KGameCenterWrapper.h"
#include "KSO3GameCenter.h"
#include "KGameServerSimulator.h"
#include "TestTools.h"
#include "NameGenerator.h"
#include "gtest/gtest.h"
#include <fstream>

struct KMemoryHelp
{
	static KMemoryHelp* Create()
	{
		static KMemoryHelp kmh;
		return &kmh;
	}
	~KMemoryHelp()
	{
		KMemory::Finalize();
	}
private:
	KMemoryHelp()
	{
		KMemory::Initialize("game_center.memory");
	}
};
KSO3GameCenter* g_pSO3GameCenter = NULL;


int main(int argc, char* argv[])
{
    int         nResult         = false;
	int         nRetCode        = false;
    int         nLogInitFlag    = false;
    int         nMemoryInitFlag = false;
	int         nBSInitFlag     = false;
	int         nGCInitFlag     = false;
	int         bDBInitFlag		= false;
    int         bMemoryPoolInit = false;
	int         bTestSettingInit = false;
	KMemoryHelp::Create();
	KGLOG_PARAM LogParam;
    char        szCWD[MAX_PATH];
	char        szTestResult[MAX_PATH];
    char*       pszRetCWD       = NULL;
	std::string filter;
    setlocale(LC_ALL, "");

#ifdef WIN32    
	WORD    wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif

    pszRetCWD = getcwd(szCWD, sizeof(szCWD));    
    KG_PROCESS_ERROR(pszRetCWD);

	snprintf(szTestResult, sizeof(szTestResult), "%s/TestResult", szCWD);

	memset(&LogParam, 0, sizeof(LogParam));
    snprintf(LogParam.szPath, sizeof(LogParam.szPath), "%s/logs", szCWD);
	strcpy(LogParam.szIdent, "SO3GameCenterTest");
	LogParam.nMaxLineEachFile = 65536 * 2;
	LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);

	nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

	nRetCode = KG_MemoryInit(NULL);
	KGLOG_PROCESS_ERROR(nRetCode);
    nMemoryInitFlag = true;

	g_SetRootPath();
	nRetCode = KGameCenterWrapper::GCSettings()->Load();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = NameGenerator::Instance()->Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = g_DBManager.Init();
	KGLOG_PROCESS_ERROR(nRetCode);
	bDBInitFlag = true;

	nRetCode = GC->Start();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = GATEWAY->Init();
	KG_PROCESS_ERROR(nRetCode);

	testing::InitGoogleTest(&argc, argv);
	
	{
		std::ifstream TestFilter("Filter.txt");
		TestFilter >> filter;
	}
	
	if (!filter.empty())
	{
		testing::GTEST_FLAG(filter) = filter;
	}
	
	RUN_ALL_TESTS();

Exit0:
	GATEWAY->UnInit();
	GC->Stop();
    if (nMemoryInitFlag)
    {
	    KG_MemoryUnInit(NULL);
        nMemoryInitFlag = false;
    }

    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }
	return 0;
}
