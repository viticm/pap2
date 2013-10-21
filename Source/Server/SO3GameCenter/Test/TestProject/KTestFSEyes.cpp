#include "StdAfx.h"
#include "KGameCenterWrapper.h"
#include "KGuardSimulator.h"
#include "TestTools.h"
#include "KGatewaySimulator.h"
#include "KGameServerSimulator.h"
#include "KTestDBManager.h"
#include "Utility.h"
#include "MailTools.h"
#include "TestDef.h"
#include "gtest/gtest.h"

#define  INDENTSTRING "MARK"

class KTestFSEyes : public testing::Test 
{
protected:
	KTestFSEyes::KTestFSEyes(void)
{
}

KTestFSEyes::~KTestFSEyes(void)
{
}

void KTestFSEyes::SetUp()
{
    int nResult = false;
    int nRetCode = false;

    ZeroMemory( &m_StartupInfo, sizeof(m_StartupInfo) );
    m_StartupInfo.cb = sizeof(m_StartupInfo);
    ZeroMemory( &m_ProcessInfo, sizeof(m_ProcessInfo) );

    nRetCode = m_GuardSimulator.Init();
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GS.Init();
    ASSERT_TRUE(nRetCode);

    for (int i = 0; i < 5; i++)
    {
		TestTools::KRoleInfo& RoleInfo = TestTools::CreateRole();
		ASSERT_TRUE(RoleInfo.dwPlayerID != ERROR_ID);
		m_RoleInfo.push_back(RoleInfo);
    }
  
    for (int i = 0; i < OnlineSize; i++)
    {
        nRetCode = TestTools::PlayerEnterGame(&m_GS, &m_RoleInfo[i], 
            m_GS.m_CreatedMapCopy.begin()->dwMapID, m_GS.m_CreatedMapCopy.begin()->nMapCopy);
        ASSERT_TRUE(nRetCode);
    }

    m_OnLineRole = &m_RoleInfo[0];
    m_OffLineRole = &m_RoleInfo[OnlineSize];
}

void KTestFSEyes::TearDown()
{
    m_GuardSimulator.UnInit();
    m_GS.UnInit();
	KGThread_Sleep(300);
	for (int i = 0; i < m_RoleInfo.size(); i++)
	{
		TestTools::DeleteRole(&m_RoleInfo[i]);
	}
	MutextHelper mh(GC->Mutex());
	GC->m_pGC->m_GameServer.m_bQuiting = false;
	GC->m_pGC->m_Gateway.m_bQuiting = false;
}
protected:
	STARTUPINFO m_StartupInfo;
	PROCESS_INFORMATION m_ProcessInfo;

	KGuardSimulator m_GuardSimulator;

	enum { OnlineSize = 3};
	std::vector<TestTools::KRoleInfo> m_RoleInfo;
	TestTools::KRoleInfo* m_OnLineRole;
	TestTools::KRoleInfo* m_OffLineRole;
	KGameServerSimulator m_GS;
	char m_szSciript[1024];
};

TEST_F(KTestFSEyes, TestE2LGetPid)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("获取进程ID", "启动GC向GC发送获取进程ID协议");

    m_GuardSimulator.DoE2LGetPid();

    nRetCode = m_GuardSimulator.CheckE2LGetPid(GetCurrentProcessId());
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestG2LProtocol)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("获取进程ID", "启动GC向GC发送获取进程ID协议");

    m_GuardSimulator.DoG2LProtocol();

    nRetCode = m_GuardSimulator.CheckG2LProtocol(GetCurrentProcessId());
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LStop)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("关闭GC", "启动GC向GC发送关闭协议");

    m_GuardSimulator.DoE2LStop();

    KGThread_Sleep(500);

    ASSERT_TRUE(GC->GCRun() == false);
}

TEST_F(KTestFSEyes, TestPlayerCount)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("获取GC人数", "启动GC读取共享内存查看人数");


    nRetCode = m_GuardSimulator.CheckPlayerCount(OnlineSize);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGS_1)
{
    int nResult = false;
    int nRetCode = false;
    char szRoleName[_NAME_LEN];
    sprintf_s(szRoleName, sizeof(szRoleName), "no exit name");

    CASE_INFO("关闭GC", "GC不存在的玩家");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGS(INDENTSTRING, szRoleName, "command");
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage(INDENTSTRING, "PLAYER_NOT_FOUND");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGS_2)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("关闭GC", "玩家不在线");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGS(INDENTSTRING, m_OffLineRole->szRoleName, "command");
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage(INDENTSTRING, "PLAYER_NOT_ONLINE");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGS_3)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("关闭GC", "玩家在线");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGS(INDENTSTRING, m_OnLineRole->szRoleName, "command");
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GS.CheckGmCommand(m_OnLineRole->dwPlayerID, INDENTSTRING, "command");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("GC GM", "error commond");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, "command");
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage(INDENTSTRING, "SCRIPT_ERROR");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_1)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("GC GM", "Test");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, "Test() SendGmMessage('Test', 'OK')");
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage("Test", "OK");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_2)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("GC GM", "error commond");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC("", "command");
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage("", "SCRIPT_ERROR");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_4)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("GC GM", "GetAccountRoleList");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nR1= GetAccountRoleList('%s')\n" 
        "  if nR1 then\n"
		"  SendGmMessage('Test', nR1[1].szName)\n"
        "  end",
        m_OnLineRole->szAccount);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage("Test", m_OnLineRole->szRoleName);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_7)
{
    int nResult = false;
    int nRetCode = false;
    char szRoleID[_NAME_LEN];

    CASE_INFO("GC GM", "RoleNameToID");

    sprintf_s(m_szSciript, sizeof(m_szSciript), 
        "nResult = GetRole(%d)\n" 
        "  if nResult then\n"
        "  SendGmMessage('Test', nResult.szName)\n"
        "  else\n"
        "  SendGmMessage('Test', 'FAIL')\n" 
        "  end",
        m_OnLineRole->dwPlayerID);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage("Test", m_OnLineRole->szRoleName);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_8)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("GC GM", "LuaKickout");
    
    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = Kickout('%s')\n" 
        "  if nResult then\n"
        "  SendGmMessage('Test', nResult)\n"
        "  else\n"
        "  SendGmMessage('Test', 'FAIL')\n"
        "  end",
        m_OnLineRole->szRoleName);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GS.CheckKickOut(m_OnLineRole->dwPlayerID);
    ASSERT_TRUE(nRetCode);
}


TEST_F(KTestFSEyes, TestE2LGmCommandGC_9)
{
    int nResult = false;
    int nRetCode = false;
    BYTE byData[10] = {0};

    CASE_INFO("GC GM", "SendGlobalSysMsg");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, 
        "SendGlobalSysMsg('Test')\n" 
        ); 
    ASSERT_TRUE(nRetCode);

    byData[0] = tdbText;
    strcpy((char*)byData + 1, "Test");
    nRetCode = m_GS.CheckGlobalSysMsg(trGlobalSys, ERROR_ID, "", ERROR_ID, "", byData);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_10)
{
    int nResult = false;
    int nRetCode = false;
    char szRoleName[_NAME_LEN];
    sprintf_s(szRoleName, sizeof(szRoleName), "no exit name");

    CASE_INFO("PlayerExecute", "不存在的玩家");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "PlayerExecute('%s', '%s')",
        szRoleName, INDENTSTRING);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GS.CheckGmCommand(ERROR_ID, "", "");
    ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_11)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("PlayerExecute", "玩家不在线");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "PlayerExecute('%s', '%s')", 
        m_OffLineRole->szRoleName, INDENTSTRING);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage(INDENTSTRING, "PLAYER_NOT_ONLINE");
    ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestFSEyes, TestE2LGmCommandGC_12)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("PlayerExecute", "玩家在线");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "PlayerExecute('%s', '%s')",
        m_OnLineRole->szRoleName, INDENTSTRING);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GS.CheckGmCommand(m_OnLineRole->dwPlayerID, m_OnLineRole->szRoleName, INDENTSTRING);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestFreezeRole)
{
    int nResult = false;
    int nRetCode = false;
    int nTime = 0;

    CASE_INFO("FreezeRole", "Freeze time = 0");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = FreezeRole('%s', %d)\n",
        m_OffLineRole->szRoleName, nTime);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);
    
    nRetCode = TestTools::PlayerEnterGame(&m_GS, m_OffLineRole, m_GS.m_CreatedMapCopy.begin()->dwMapID, 
        m_GS.m_CreatedMapCopy.begin()->nMapCopy);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestFreezeRole_2)
{
    int nResult = false;
    int nRetCode = false;
    int nTime = -1000;

    CASE_INFO("FreezeRole", "Freeze time < 0");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = FreezeRole('%s', %d)\n",
        m_OffLineRole->szRoleName, nTime);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = TestTools::PlayerEnterGame(&m_GS, m_OffLineRole, m_GS.m_CreatedMapCopy.begin()->dwMapID, 
        m_GS.m_CreatedMapCopy.begin()->nMapCopy);
    ASSERT_TRUE(nRetCode);
}


TEST_F(KTestFSEyes, TestFreezeRole_3)
{
    int nResult = false;
    int nRetCode = false;
    int nTime = 30;

    CASE_INFO("FreezeRole", "Freeze time > 0");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = FreezeRole('%s', %d)\n",
        m_OffLineRole->szRoleName, nTime);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

	KGThread_Sleep(100);

    nRetCode = TestTools::PlayerEnterGame(&m_GS, m_OffLineRole, m_GS.m_CreatedMapCopy.begin()->dwMapID, 
        m_GS.m_CreatedMapCopy.begin()->nMapCopy);
    ASSERT_TRUE(!nRetCode);
}

TEST_F(KTestFSEyes, TestFreezeRole_4)
{
    int nResult = false;
    int nRetCode = false;
    int nTime = 1;

    CASE_INFO("FreezeRole", "Freeze time > 0");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = FreezeRole('%s', %d)\n",
        m_OffLineRole->szRoleName, nTime);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    KGThread_Sleep(1500);

    nRetCode = TestTools::PlayerEnterGame(&m_GS, m_OffLineRole, m_GS.m_CreatedMapCopy.begin()->dwMapID, 
        m_GS.m_CreatedMapCopy.begin()->nMapCopy);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSendSystemMail)
{
    int nResult = false;
    int nRetCode = false;
    int nTime = 1;

    CASE_INFO("SendSysMail", "ErrorID");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = SendSystemMail('', '%s', %d, 0)\n",
        "Error Name", nTime);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSendSystemMail_2)
{
    int nResult = false;
    int nRetCode = false;
    int nTime = 1;
	std::vector<KMailListInfo> MailList;

    CASE_INFO("SendSysMail", "OnLine Player");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
        "nResult = SendSystemMail('', '%s', 'haha', '')\n",
        m_OnLineRole->szRoleName, nTime);

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

	KGThread_Sleep(200);

	MailTools mt;
	mt.m_pGS = &m_GS;
	MailList = mt.GetMailList(m_OnLineRole->dwPlayerID);
	ASSERT_TRUE(MailList.size() != 0);

//    ASSERT_STREQ(MailList[0].szSenderName, "");
}

TEST_F(KTestFSEyes, TestRemoteLuaCall)
{
	int nResult = false;
	int nRetCode = false;
	BYTE ParamBuffer[100];
	size_t ParamLen = 0;
	double a = 10;

	CASE_INFO("IsRoleExit", "OnLine Player");
	
	ParamBuffer[0] = 0;
	memcpy(ParamBuffer + 1, &a, sizeof(double));
	nRetCode = m_GuardSimulator.DoE2LRemoteLuaCall("OnRemoteTestCall", (sizeof(double) + sizeof(BYTE)), ParamBuffer); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TRUE");
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_1)
{
    int nResult = false;
    int nRetCode = false;

    CASE_INFO("SetCustomData", "string");

    sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('', 'hahha')\n"
		"a = Manager.Get('')\n"
		"if (a ~= 'hahha') then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

    nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
    ASSERT_TRUE(nRetCode);

    nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_2)
{
	int nResult = false;
	int nRetCode = false;

	CASE_INFO("SetCustomData", "interg");

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('', 6)\n"
		"a = Manager.Get('')\n"
		"if (a ~= 6) then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_5)
{
	int nResult = false;
	int nRetCode = false;

	CASE_INFO("SetCustomData", "bool");

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('', false)\n"
		"a = Manager.Get('')\n"
		"if (a ~= false) then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_3)
{
	int nResult = false;
	int nRetCode = false;

	CASE_INFO("SetCustomData", "table");

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Account = {1,2,{3,3,4}}\n"
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('', Account)\n"
		"a = Manager.Get('')\n"
		"if (a[3][2] ~= Account[3][2]) then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}


TEST_F(KTestFSEyes, TestSaveCustomData)
{
	int nResult = false;
	int nRetCode = false;
	char szSql[1024];
	CASE_INFO("SaveCustomData", "insert a data");

	nRetCode = g_DBManager.ClearTable(GLOBAL_CUSTOM_DATA_TABLE_NAME);
	ASSERT_TRUE(nRetCode);

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('testdata', nil)\n"
		"Manager.Set('testdata', 'testdata')\n");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	GC->AccelerationGCTime(10);

	KGThread_Sleep(1000);

	sprintf_s(szSql, sizeof(szSql), "select Value from %s where name = '%s';", GLOBAL_CUSTOM_DATA_TABLE_NAME, "testdata");
	 BufferScopedPrt pBuffer(g_DBManager.QueyRueslt(szSql));
	 ASSERT_TRUE(pBuffer.get());
}

TEST_F(KTestFSEyes, TestDeleteCustomData)
{
	int nResult = false;
	int nRetCode = false;
	char szSql[1024];
	CASE_INFO("DeleteCustomData", "database has value");

	nRetCode = g_DBManager.ClearTable(GLOBAL_CUSTOM_DATA_TABLE_NAME);
	ASSERT_TRUE(nRetCode);
 
	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('testdata', 'testdata')\n");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	GC->AccelerationGCTime(10);
	KGThread_Sleep(100);

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('testdata', nil)\n");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(100);

	sprintf_s(szSql, sizeof(szSql), "select Value from %s where name = '%s';", GLOBAL_CUSTOM_DATA_TABLE_NAME, "testdata");
	BufferScopedPrt pBuffer(g_DBManager.QueyRueslt(szSql));
	ASSERT_TRUE(!pBuffer.get());
}

TEST_F(KTestFSEyes, TestSetCustomData_4)
{
	int nResult = false;
	int nRetCode = false;
	char szSql[1024];
	CASE_INFO("SetCustomData", "Nil key");

	nRetCode = g_DBManager.ClearTable(GLOBAL_CUSTOM_DATA_TABLE_NAME);
	ASSERT_TRUE(nRetCode);

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('', 'testdata')\n");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	GC->AccelerationGCTime(10);
	KGThread_Sleep(100);

	sprintf_s(szSql, sizeof(szSql), "select Value from %s where name = '';", GLOBAL_CUSTOM_DATA_TABLE_NAME);
	BufferScopedPrt pBuffer(g_DBManager.QueyRueslt(szSql));
	ASSERT_TRUE(pBuffer.get());
}

TEST_F(KTestFSEyes, TestGetCustomData_1)
{
	int nResult = false;
	int nRetCode = false;


	CASE_INFO("GetCustomData", "Nil value");

	nRetCode = g_DBManager.ClearTable(GLOBAL_CUSTOM_DATA_TABLE_NAME);
	ASSERT_TRUE(nRetCode);

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('testdata', nil)\n"
		"a = Manager.Get('testdata')\n"
		"if a == nil then\n"
		"SendGmMessage('Test', 'TURE')\n"
		"else \n"
		"SendGmMessage('Test', 'FAIL')\n"
		"end"); 

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}


TEST_F(KTestFSEyes, TestSetCustomData_6)
{
	int nResult = false;
	int nRetCode = false;

	CASE_INFO("SetCustomData", "change value");

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('', false)\n"
		"Manager.Set('', {})\n"
		"a = Manager.Get('')\n"
		"if type(a) ~= type({})  then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_7)
{
	int nResult = false;
	int nRetCode = false;

	CASE_INFO("SetCustomData", "empty string");

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('testdata', '')\n"
		"a = Manager.Get('testdata')\n"
		"if a ~= ''  then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_8)
{
	int nResult = false;
	int nRetCode = false;

	CASE_INFO("SetCustomData", "empty table");

	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('testdata', {})\n"
		"a = Manager.Get('testdata')\n"
		"if #a ~= 0  then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end");

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}

TEST_F(KTestFSEyes, TestSetCustomData_9)
{
	int nResult = false;
	int nRetCode = false;
	char szKey[128] ;

	CASE_INFO("SetCustomData", "empty table");

	for (int i = 0; i < sizeof(szKey); i++)
	{
		szKey[i] = 'a';
	}
	sprintf_s(m_szSciript, sizeof(m_szSciript),
		"Manager = GetGlobalCustomDataManager()\n"
		"Manager.Set('%s', {})\n"
		"a = Manager.Get('%s')\n"
		"if a ~= nil  then\n"
		"SendGmMessage('Test', 'FAIL')\n"
		"else "
		"SendGmMessage('Test', 'TURE')\n"
		"end", szKey, szKey);

	nRetCode = m_GuardSimulator.DoE2LGmCommandGC(INDENTSTRING, m_szSciript); 
	ASSERT_TRUE(nRetCode);

	nRetCode = m_GuardSimulator.CheckMessage("Test", "TURE");
	ASSERT_TRUE(nRetCode);
}