#include "StdAfx.h"
#include "KTestDBManager.h"
#include "Common/CRC32.h"
#include "KGameServerSimulator.h"
#include "KGatewaySimulator.h"
#include "KGameCenterWrapper.h"
#include "testtools.h"
#include "Utility.h"
#include "NameGenerator.h"
#include "gtest/gtest.h"
#include <fstream>

class KTestRoleDB :  public testing::Test 
{
	protected:
		KTestRoleDB::KTestRoleDB(void)
{
    m_pGameServerA = NULL;

    for(int i = 0; i < 10 ; i++)
    {
		TestTools::KRoleInfo   RoleInfo;
		std::string RoleName = NameGenerator::Instance()->LegalName(3);
        sprintf(RoleInfo.szAccount, "TestAccount%d", i+1);
        strcpy(RoleInfo.szRoleName, RoleName.c_str());
        m_RoleList.push_back(RoleInfo);
    }

	for (int i = 0; i <= GC->GCSettings()->m_nMaxRoleCount ; i++)
	{
		TestTools::KRoleInfo   RoleInfo;
		sprintf(RoleInfo.szAccount, "TeAccount");
		std::string RoleName = NameGenerator::Instance()->LegalName(3);
		strcpy(RoleInfo.szRoleName, RoleName.c_str());
		m_AccountRoleList.push_back(RoleInfo);
	}

    m_TestRoleA    = &m_RoleList[0];
	m_TestRoleB   = &m_RoleList[1];
}

KTestRoleDB::~KTestRoleDB(void)
{
}

void KTestRoleDB::SetUp()
{
    int nResult = false;
    int nRetCode  = false;
    m_pGameServerA = new KGameServerSimulator();
    nRetCode = m_pGameServerA->Init();
    ASSERT_TRUE(nRetCode);
}

void KTestRoleDB::TearDown()
{
    SAFE_DELETE(m_pGameServerA);
	KGThread_Sleep(200);
	for (int i = 0; i < m_RoleList.size(); i++)
	{
		TestTools::DeleteRole(&m_RoleList[i]);
	}
	for (int i = 0; i < m_AccountRoleList.size(); i++)
	{
		TestTools::DeleteRole(&m_AccountRoleList[i]);
	}
}

void KTestRoleDB::GenerateRoleDBData(BYTE* pByte, unsigned int uSize)
{
	//KRoleBaseInfo* pRoleDBData = (KRoleBaseInfo*)pByte;
	//pRoleDBData->wVersion = rdvCurrentRoleDataVersion;
	//pRoleDBData->wBlockCount = 1;
	//pRoleDBData->dwDataLen = uSize;
	//pRoleDBData->dwCRC = 0;
	//pRoleDBData->dwCRC = CRC32(0, pRoleDBData, uSize);
}

int CreateRole(TestTools::KRoleInfo* pRoleInfo)
{
	int nResult = false;
	int nRetCode = false;
	KUSER_ROLE_GENERATOR_PARAM UserRoleParam;

	memset(&UserRoleParam, 0, sizeof(UserRoleParam));
	UserRoleParam.cRoleType = rtStandardMale;
	UserRoleParam.dwMapID = GC->GetHometownMapTable()[0];
	UserRoleParam.nMapCopyIndex = 0;

	strncpy(UserRoleParam.szRoleName, pRoleInfo->szRoleName, sizeof(UserRoleParam.szRoleName));
	GATEWAY->DoCreateRoleRequest(pRoleInfo->szAccount, UserRoleParam);

	BufferScopedPrt piRecvPackage(GATEWAY->ReceiveDataOnce(r2g_create_role_respond));
	if(piRecvPackage.get())
	{
		R2G_CREATE_ROLE_RESPOND* pRespond = (R2G_CREATE_ROLE_RESPOND*)piRecvPackage->GetData();
		if ((pRespond->byCode == eCreateRoleSucceed))
		{
			m_TestRoleA->dwPlayerID = ((KROLE_LIST_INFO*)pRespond->byData)->dwPlayerID;
		}
		return pRespond->byCode;
	}
	return -1;
}

protected:
    KGameServerSimulator* m_pGameServerA;
	typedef std::vector<TestTools::KRoleInfo>  ROLE_VECTOR;
    ROLE_VECTOR      m_RoleList;
	ROLE_VECTOR      m_AccountRoleList;
    TestTools::KRoleInfo*		 m_TestRoleA;
	TestTools::KRoleInfo*		 m_TestRoleB;
};

TEST_F(KTestRoleDB, TestCreateRole_1)
{
    int nRetCode = false;
    unsigned long ulIdentity = 100;
    KROLE_LIST_INFO* pBaseData = NULL;
    
    CASE_INFO("数据库不存在角色，创建角色", "角色名正确，不重名");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_BASE_INFO_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(CreateRole(m_TestRoleA) == eCreateRoleSucceed);
    
    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_NAME_FIELD_NAME, KG_ID_FIELD_NAME, m_TestRoleA->dwPlayerID));
    ASSERT_TRUE(pRoleData.get());
   
	ASSERT_STREQ(m_TestRoleA->szRoleName, (char*)pRoleData->GetData());

    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ACCOUNT_FIELD_NAME, KG_ID_FIELD_NAME, m_TestRoleA->dwPlayerID));
    ASSERT_TRUE(pRoleData.get());

	ASSERT_STREQ(m_TestRoleA->szAccount, (char*)pRoleData->GetData());
}

TEST_F(KTestRoleDB, TestCreateRole_2)
{
    int nRetCode = false;
    string strFilterStr;
    int nPositison;

    CASE_INFO("数据库中存在帐号，创建角色，创建角色名包含过滤字符", "角色名包含过滤字符");
    
	std::ifstream NameFilterFile("NameFilter.txt");
    ASSERT_TRUE(NameFilterFile);

    ASSERT_TRUE(getline(NameFilterFile, strFilterStr));
  
    while ((nPositison = strFilterStr.find('*')) != string::npos)
    {
        strFilterStr.erase(strFilterStr.begin() + nPositison);
    }
    
    while ((nPositison = strFilterStr.find('?')) != string::npos)
    {
        strFilterStr.erase(strFilterStr.begin() + nPositison);
    }

    strcpy(m_TestRoleA->szRoleName, strFilterStr.c_str());

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

    ASSERT_TRUE(CreateRole(m_TestRoleA) == eCreateRoleInvalidRoleName);

    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(!pRoleData.get());
}

TEST_F(KTestRoleDB, TestCreateRole_3)
{
    CASE_INFO("数据库中存在帐号，创建角色，创建角色名为空", "角色名为空");
	strcpy(m_TestRoleA->szRoleName, "");
    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

    
    ASSERT_TRUE(CreateRole(m_TestRoleA) == eCreateRoleNameTooShort);

    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(!pRoleData.get());
}

TEST_F(KTestRoleDB, TestCreateRole_4)
{
    CASE_INFO("不能创建同名的角色，创建于其他帐号同名角色", "创建角色与其他角色同名");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(!pRoleData.get());

    ASSERT_TRUE(CreateRole(m_TestRoleA) == eCreateRoleSucceed);

	strcpy(m_TestRoleB->szRoleName, m_TestRoleA->szRoleName);
    ASSERT_TRUE(CreateRole(m_TestRoleB) == eCreateRoleNameAlreadyExist);

    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleB->szAccount));
    ASSERT_TRUE(!pRoleData.get());
}

TEST_F(KTestRoleDB, TestCreateRole_5)
{

    CASE_INFO("游戏帐号下角色已满不能创建角色", "游戏帐号下角色数据已满");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_AccountRoleList[GC->GCSettings()->m_nMaxRoleCount].szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

    for (int i = 0; i < GC->GCSettings()->m_nMaxRoleCount; i++)
    {    
		ASSERT_TRUE(TestTools::CreateRole(&m_AccountRoleList[i]));
    }

	ASSERT_TRUE(CreateRole(&m_AccountRoleList[GC->GCSettings()->m_nMaxRoleCount]) == eCreateRoleUnableToCreate);

    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_AccountRoleList[GC->GCSettings()->m_nMaxRoleCount].szRoleName));
    ASSERT_TRUE(!pRoleData.get());
}


TEST_F(KTestRoleDB, TestGetRoleList_1)
{
    int nResult = false;
    int nRetCode = false;
    char szAccount[] = "TestAccount";
    int nRoleCount = 0;

    CASE_INFO("查询数据库不存在帐号角色列表", "数据库不存在帐号");

    nRoleCount = g_DBManager.AccountRoleCount(szAccount);
    ASSERT_TRUE(nRoleCount == 0);

    nRetCode = GATEWAY->DonQueryRoleList(szAccount);
    ASSERT_TRUE(nRetCode);

    nRetCode = GATEWAY->CheckSyncRoleList(0, 0,  NULL);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestRoleDB, TestGetRoleList_2)
{
    int nRoleCount = 0;

    CASE_INFO("查询帐号有角色信息", "帐号角色列表不为空");

    nRoleCount = g_DBManager.AccountRoleCount(m_AccountRoleList[0].szAccount);
    ASSERT_TRUE(nRoleCount == 0);

    for (int i = 0; i < GC->GCSettings()->m_nMaxRoleCount; i++)
    {    
		ASSERT_TRUE(TestTools::CreateRole(&m_AccountRoleList[i]));
    }

    nRoleCount = g_DBManager.AccountRoleCount(m_AccountRoleList[0].szAccount);
    ASSERT_TRUE(nRoleCount == 3);

    ASSERT_TRUE(GATEWAY->DonQueryRoleList(m_AccountRoleList[0].szAccount));

  //  for (int i = 0; i < GC->GCSettings()->m_nMaxRoleCount; i++)
  //  {
  //  nRetCode = GATEWAY->CheckSyncRoleList(i + 1, GC->GCSettings()->m_nMaxRoleCount, &BaseData[i]);
  //  ASSERT_TRUE(nRetCode);
  //  }

  //  
  //  for (int i = 0; i < GC->GCSettings()->m_nMaxRoleCount; i++)
  //  {    
		//ASSERT_STREQ(BaseData[i].szAccount, szAccount);
  //  }
}

TEST_F(KTestRoleDB, TestDeleteRole_1)
{
    CASE_INFO("删除角色", "角色在数据库中，且不再游戏中");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(TestTools::CreateRole(m_TestRoleA));

	ASSERT_TRUE(TestTools::DeleteRole(m_TestRoleA));

    KGThread_Sleep(500);

    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);
}

TEST_F(KTestRoleDB, TestDeleteRole_2)
{
    DWORD dwPlayID = 4999;

    CASE_INFO("删除不存在的角色", "角色不存在数据库中");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_ID_FIELD_NAME, dwPlayID));
    ASSERT_TRUE(pRoleData.get() == NULL);

    ASSERT_TRUE(GATEWAY->DoDeleteRoleRequest(dwPlayID));

    ASSERT_TRUE(GATEWAY->CheckDeleteRoleRespond(dwPlayID, false));
}

TEST_F(KTestRoleDB, TestLoadRole_1)
{
	int nRetCode  = false;
    CASE_INFO("可以加载数据库保存的角色数据", "申请角色数据的角色名在数据库中");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(TestTools::CreateRole(m_TestRoleA));

	scoped_array<BYTE> pData(new BYTE[300]);
	KRoleDataHeader* pRoleDataHead  = (KRoleDataHeader*)pData.get();
	DWORD dwCRC = CRC32(0, pData.get() + sizeof(KRoleDataHeader), 300 - sizeof(KRoleDataHeader));
	pRoleDataHead->dwCRC = dwCRC;

	m_pGameServerA->DoSyncRoleData(m_TestRoleA->dwPlayerID, 0, pData.get(), 300);
	KRoleBaseInfo BaseInfo;
	m_pGameServerA->DoSaveRoleData(m_TestRoleA->dwPlayerID, 5, BaseInfo, 300);

	KGThread_Sleep(500);
    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_EXT_INFO_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get());

    nRetCode = m_pGameServerA->DoLoadRoleData(m_TestRoleA->dwPlayerID);
    ASSERT_TRUE(nRetCode);

    nRetCode = m_pGameServerA->CheckLoadRoleDataRespond(m_TestRoleA->dwPlayerID, true, NULL, pRoleData->GetSize());
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestRoleDB, TestLoadRole_2)
{
    int nRetCode = false;
    DWORD dwPlayerID = UINT_MAX;

    CASE_INFO("不可以加载数据库未保存的角色数据", "申请角色数据的角色名不在数据库中");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_ID_FIELD_NAME, dwPlayerID));
    ASSERT_TRUE(pRoleData.get() == NULL);

    m_pGameServerA->DoLoadRoleData(dwPlayerID);

    nRetCode = m_pGameServerA->CheckLoadRoleDataRespond(dwPlayerID, false, NULL, 0);
    ASSERT_TRUE(nRetCode);
}

TEST_F(KTestRoleDB, TestSaveRole_1)
{
	int nRetCode = false;
    CASE_INFO("数据库存在角色数据，角色数据正确", "需保存数据正确");

    BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(TestTools::CreateRole(m_TestRoleA));

    scoped_array<BYTE> pData(new BYTE[300]);
    KRoleDataHeader* pRoleDataHead  = (KRoleDataHeader*)pData.get();
	DWORD dwCRC = CRC32(0, pData.get() + sizeof(KRoleDataHeader), 300 - sizeof(KRoleDataHeader));
	pRoleDataHead->dwCRC = dwCRC;
	
	m_pGameServerA->DoSyncRoleData(m_TestRoleA->dwPlayerID, 0, pData.get(), 300);
	KRoleBaseInfo BaseInfo;
	m_pGameServerA->DoSaveRoleData(m_TestRoleA->dwPlayerID, 5, BaseInfo, 300);
    
    nRetCode = m_pGameServerA->CheckSaveRoleDataRespond(m_TestRoleA->dwPlayerID, true, 5);
    ASSERT_TRUE(nRetCode);

	KGThread_Sleep(500);
    pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_EXT_INFO_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
    ASSERT_TRUE(pRoleData.get());
    nRetCode = memcmp(pRoleData->GetData(), pData.get(), pRoleData->GetSize());
    ASSERT_TRUE(nRetCode == 0);
}

TEST_F(KTestRoleDB, TestSaveRole_3)
{
    int nResult = false;

    CASE_INFO("数据库存在角色数据，角色数据错误", "需要保持数据CRC不正确");

	BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
	ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(TestTools::CreateRole(m_TestRoleA));

	scoped_array<BYTE> pData(new BYTE[300]);
	KRoleDataHeader* pRoleDataHead  = (KRoleDataHeader*)pData.get();
	DWORD dwCRC = CRC32(0, pData.get() + sizeof(KRoleDataHeader), 300 - sizeof(KRoleDataHeader));
	pRoleDataHead->dwCRC = dwCRC + 1;

	m_pGameServerA->DoSyncRoleData(m_TestRoleA->dwPlayerID, 0, pData.get(), 300);
	KRoleBaseInfo BaseInfo;
	m_pGameServerA->DoSaveRoleData(m_TestRoleA->dwPlayerID, 5, BaseInfo, 300);

	nResult = m_pGameServerA->CheckSaveRoleDataRespond(m_TestRoleA->dwPlayerID, true, 5);
	ASSERT_TRUE(nResult);
	
	KGThread_Sleep(100);

	pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_EXT_INFO_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
	ASSERT_TRUE(pRoleData.get() == NULL);
}

TEST_F(KTestRoleDB, TestSaveRole_2)
{
	int nRetCode = false;
	CASE_INFO("数据库存在角色数据，角色数据正确", "只有KRoleDataHeader数据");
	BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
	ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(TestTools::CreateRole(m_TestRoleA));

	KRoleDataHeader RoleHead;
	m_pGameServerA->DoSyncRoleData(m_TestRoleA->dwPlayerID, 0, (BYTE*)&RoleHead, sizeof(KRoleDataHeader));
	KRoleBaseInfo BaseInfo;
	m_pGameServerA->DoSaveRoleData(m_TestRoleA->dwPlayerID, 5, BaseInfo, sizeof(KRoleDataHeader));

	nRetCode = m_pGameServerA->CheckSaveRoleDataRespond(m_TestRoleA->dwPlayerID, true, 5);
	ASSERT_TRUE(nRetCode);

	KGThread_Sleep(100);

	pRoleData.reset(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_EXT_INFO_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
	ASSERT_TRUE(pRoleData.get() == NULL);
}

TEST_F(KTestRoleDB, TestSaveRole_4)
{
	int nRetCode = false;
    CASE_INFO("数据库存在角色数据，角色数据正确", "只保存baseinfo");
	BufferScopedPrt pRoleData(g_DBManager.GetRoleData(ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, m_TestRoleA->szRoleName));
	ASSERT_TRUE(pRoleData.get() == NULL);

	ASSERT_TRUE(TestTools::CreateRole(m_TestRoleA));

	m_pGameServerA->DoSyncRoleData(m_TestRoleA->dwPlayerID, 0, NULL, 0);
	KRoleBaseInfo BaseInfo;
	m_pGameServerA->DoSaveRoleData(m_TestRoleA->dwPlayerID, 5, BaseInfo, 0);

	nRetCode = m_pGameServerA->CheckSaveRoleDataRespond(m_TestRoleA->dwPlayerID, true, 5);
	ASSERT_TRUE(!nRetCode);
}