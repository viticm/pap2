#include "StdAfx.h"
#include "KGameCenterWrapper.h"
#include "TestDef.h"

#define  HELPLOCK MutextHelper mh(&m_Lock)


KGameCenterWrapper::KGameCenterWrapper(void): m_GCInit(false), m_bRun(true), m_pGC(NULL), m_timeAccelera(0)
{
	
}

KGameCenterWrapper::~KGameCenterWrapper(void)
{
//	Stop();
}

KGameCenterWrapper* KGameCenterWrapper::Instance()
{
	static KGameCenterWrapper wrapper;
	return &wrapper;
}


KSO3GameCenterSettings* KGameCenterWrapper::GCSettings()
{
	static KSO3GameCenterSettings GcSettings;
	return &GcSettings;
}

int KGameCenterWrapper::GetRolePartyID(DWORD dwRoleId)
{
	HELPLOCK;
	ASSERT(m_pGC);
	KRole* pRole = m_pGC->m_RoleManager.GetRole(dwRoleId);
	if (pRole)
	{
		return pRole->m_dwTeamID;
	}
	return ERROR_ID;
}

int KGameCenterWrapper::GetPartyLeaderID(DWORD dwPartyID)
{
	HELPLOCK;
	ASSERT(m_pGC);
	KTeam* pTeam = m_pGC->m_TeamCenter.GetTeam(dwPartyID);
	if (pTeam)
	{
		return pTeam->dwAuthority[tatLeader];
	}
	return 0;
}


bool KGameCenterWrapper::Start()
{
	g_pSO3GameCenter = KMemory::New<KSO3GameCenter>();
	if(!g_pSO3GameCenter)
		return false;

	if (!g_pSO3GameCenter->Init())
	{
		return false;
	}
	m_BattleList = g_pSO3GameCenter->m_BattleFieldManager.m_BaseInfoTable;
	m_pGC = g_pSO3GameCenter;
	m_pGC->m_GameServer.m_nConnectionTimeout = 60 * 60 * 24 * 90;
	m_pGC->m_Gateway.m_nPingCycle = 60 * 60 * 24 * 90;
	m_GCInit = true;
	m_Thread.Create(GameCenterRunner, this);
	return true;
}

void KGameCenterWrapper::Stop()
{
	m_bRun = false;
	m_Thread.Destroy();
	if (m_GCInit)
	{
		m_pGC->m_GlobalCustomDataManager.SaveAll();
		m_pGC->m_MailManager.SaveAll();
		m_pGC->m_TongManager.SaveAll();
		m_pGC->m_StatDataManager.SaveAll();
		g_pSO3GameCenter->UnInit();
	}
	if (g_pSO3GameCenter)
	{
		KMemory::Delete(g_pSO3GameCenter);
		g_pSO3GameCenter = NULL;
	}
	m_pGC = NULL;
	m_GCInit = false;
	m_bRun = true;
	m_BattleList.clear();
}

void KGameCenterWrapper::GameCenterRunner(void* pSelf)
{
	KGameCenterWrapper* p = (KGameCenterWrapper*)pSelf;
	while (p->m_bRun)
	{	
		{
			MutextHelper(&p->m_Lock);
			p->GCActivate();
		}
		KGThread_Sleep(10);
	}
}

void KGameCenterWrapper::AccelerationGC(int nGameLoop)
{
	MutextHelper mh(&m_Lock);
	g_pSO3GameCenter->m_nWorkLoop += nGameLoop;
}

void KGameCenterWrapper::AccelerationMultipleGC(int nMultip)
{
	MutextHelper mh(&m_Lock);
	g_pSO3GameCenter->m_nWorkLoop += nMultip * GAME_FPS - g_pSO3GameCenter->m_nWorkLoop % (nMultip * GAME_FPS);
	g_pSO3GameCenter->m_nWorkLoop--;
 }

void KGameCenterWrapper::AccelerationRemainderGC(int nMultip, int Remainder)
{
	AccelerationMultipleGC(nMultip);
	MutextHelper mh(&m_Lock);
	g_pSO3GameCenter->m_nWorkLoop += Remainder;
}

std::vector<DWORD> KGameCenterWrapper::GetHometownMapTable()
{
	HELPLOCK;
	ASSERT(m_pGC);
	return *m_pGC->m_HometownList.GetHometownMapTable();
}

KBattleFieldManager::KBASE_INFO_TABLE&  KGameCenterWrapper::GetBattleList()
{
	return m_pGC->m_BattleFieldManager.m_BaseInfoTable;
}

bool KGameCenterWrapper::AddInBlackList(DWORD dwID, time_t BlackTime)
{
	HELPLOCK;
	return m_pGC->m_BattleFieldManager.AddInBlackList(dwID, BlackTime);
}


bool KGameCenterWrapper::CheckPermission(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex)
{
	HELPLOCK;
	return m_pGC->m_BattleFieldManager.CheckPermission(dwRoleID, dwMapID, nCopyIndex);
}
int KGameCenterWrapper::GetBattleFieldSide(DWORD dwMapID, int nCopyIndex, DWORD dwRoleID)
{
	HELPLOCK;
	return m_pGC->m_BattleFieldManager.GetBattleFieldSide(dwMapID, nCopyIndex, dwRoleID);
}

int KGameCenterWrapper::CreateBattleField(DWORD dwMapID)
{
	HELPLOCK;

	BOOL                        bResult         = false;
	BOOL                        bRetCode        = false;
	KBattleFieldManager::KBASE_INFO*                 pBase           = NULL;
	KMapInfo*                   pMapInfo        = NULL;
	KMapCopy*                   pMapCopy        = NULL;
	int                         nDestConnIndex  = 0;
	int                         nMapCopyIndex   = 0;

	KBattleFieldManager::KBASE_INFO_TABLE::iterator Iter  = g_pSO3GameCenter->m_BattleFieldManager.m_BaseInfoTable.find(dwMapID);
	KGLOG_PROCESS_ERROR(Iter != g_pSO3GameCenter->m_BattleFieldManager.m_BaseInfoTable.end());
	pBase = &Iter->second;


	pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
	KGLOG_PROCESS_ERROR(pMapInfo);

	nMapCopyIndex = pMapInfo->m_nSNGenerator++;

	nDestConnIndex = g_pSO3GameCenter->m_GameServer.GetBestGSConnectionForCreateMap(pMapInfo);
	KG_PROCESS_ERROR(nDestConnIndex);

	pMapCopy = pMapInfo->CreateMapCopy(
		nMapCopyIndex, g_pSO3GameCenter->m_nTimeNow, 0, 
		nDestConnIndex, true
		);
	KGLOG_PROCESS_ERROR(pMapCopy);

	g_pSO3GameCenter->m_GameServer.DoCreateMapNotify(nDestConnIndex, dwMapID, nMapCopyIndex);

	pMapCopy->m_eState = eMapStateCreating;

	g_pSO3GameCenter->m_BattleFieldManager.m_dwCreatingMapID       = dwMapID;
	g_pSO3GameCenter->m_BattleFieldManager.m_nCreatingMapCopyIndex = nMapCopyIndex;

Exit0:
	return nMapCopyIndex;
}


void KGameCenterWrapper::GCActivate()
{
	g_pSO3GameCenter->m_nTimeNow = time(NULL) + m_timeAccelera;
	g_pSO3GameCenter->m_nWorkLoop++;

	g_pSO3GameCenter->m_Gateway.Activate();

	g_pSO3GameCenter->m_GameServer.Activate();

	g_pSO3GameCenter->m_MapManager.Activate();

	g_pSO3GameCenter->m_FellowshipCenter.Active();

	g_pSO3GameCenter->m_MailManager.Activate();

	g_pSO3GameCenter->m_AuctionManager.Activate();

	g_pSO3GameCenter->m_RoleDB.Activate();

	g_pSO3GameCenter->m_RoleManager.Activate();

	g_pSO3GameCenter->m_MiscDB.Activate();

	g_pSO3GameCenter->m_Eyes.Activate();

	g_pSO3GameCenter->m_ScriptManager.Activate();

	g_pSO3GameCenter->m_BattleFieldManager.Activate();

	// m_PartyCenter.Activate();

	g_pSO3GameCenter->m_TeamCenter.Activate();

	g_pSO3GameCenter->m_TongManager.Activate();

	g_pSO3GameCenter->m_PQCenter.Activate();

	g_pSO3GameCenter->m_CampManager.Activate();

	g_pSO3GameCenter->m_GlobalCustomDataManager.Activate();

	g_pSO3GameCenter->m_GlobalSystemValueManager.Activate();
	
	g_pSO3GameCenter->m_StatDataManager.Activate();

	g_pSO3GameCenter->m_ActivityManager.Activate();

	g_pSO3GameCenter->m_ApexProxy.Breathe();

	g_pSO3GameCenter->m_GameCardManager.Activate();

	g_pSO3GameCenter->m_LogServerAgency.Activate();

	g_pSO3GameCenter->m_AntiFarmerManager.Activate();

	if (g_pSO3GameCenter->m_nWorkLoop % GAME_FPS == 0)
	{
		g_pSO3GameCenter->CallMainScript("Activate");
	}
}

void KGameCenterWrapper::AccelerationGCTime(time_t uTime)
{
	HELPLOCK;
	m_timeAccelera += uTime;
}

void KGameCenterWrapper::ManualActivate()
{
	HELPLOCK;
	GCActivate();
}

KGPQ* KGameCenterWrapper::GetPQ(DWORD dwPQ)
{
	HELPLOCK;
	return g_pSO3GameCenter->m_PQCenter.GetPQ(dwPQ);
}

void KGameCenterWrapper::SaveFellowShipData(DWORD dwRoleID)
{
	HELPLOCK;
	g_pSO3GameCenter->m_FellowshipCenter.SaveFellowshipData(dwRoleID);
}

bool KGameCenterWrapper::GCRun()
{
	HELPLOCK;
	return g_pSO3GameCenter->m_bQuitFlag;
}

KROLE_STATE KGameCenterWrapper::RoleSate(DWORD dwRoleID)
{
	HELPLOCK;
	KRole* p  = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
	if (p)
	{
		return p->GetRoleState();
	}
	return rsInvalid;
}


bool KGameCenterWrapper::CanCreateMap(DWORD& dwMapID, int& nMapCopyIndex)
{
	HELPLOCK;
	std::vector<DWORD> MapList;
	g_pSO3GameCenter->m_MapManager.GetMapList(MapList);
	for (int i = 0; i < MapList.size(); i++)
	{
		for (int k = 1; k < 100; k++)
		{
			KMapInfo* p = g_pSO3GameCenter->m_MapManager.GetMapInfo(MapList[i]);
			if (p->m_nType == emtDungeon)
			{
				if (!g_pSO3GameCenter->m_MapManager.IsMapCopyInPreloadGroup(MapList[i], k))
				{
					dwMapID = MapList[i];
					nMapCopyIndex = k;
					return true;
				}
			}
		}
	}
	return false;
}

bool LoadFromBuffer(const char* pszScriptName, const char* pszBuffer, lua_State* pLua)
{
	BOOL    bResult             = false;
	int     nRetCode            = 0;

	assert(pszScriptName);
	assert(pszBuffer);

	if (lua_gc(pLua, LUA_GCCOUNT, 0) > 20480) 
	{
		lua_gc(pLua, LUA_GCCOLLECT, 0);
	}
	
	nRetCode = luaL_loadbuffer(pLua, pszBuffer, strlen(pszBuffer), pszScriptName);
	KG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = lua_pcall(pLua, 0, 0, 0);
	KG_PROCESS_ERROR(nRetCode == 0);

	bResult = true;
Exit0:
	if (!bResult)
	{
		const char* pszErrorMsg = Lua_ValueToString(pLua, -1);
		if (pszErrorMsg)
		{
			KGLogPrintf(KGLOG_ERR, "[Lua] %s", pszErrorMsg);
		}
	}
	return bResult;
}


bool LuaResult(lua_State* pLua)
{
	lua_getglobal(pLua, "Result");
	if (Lua_IsBoolean(pLua, -1))
	{
		return lua_toboolean(pLua, -1);
	}
	return false;
}

bool KGameCenterWrapper::CallScript(const char* pszScript)
{
	HELPLOCK;
	int     nLuaTopIndex    = 0;
	bool    bResult  = false;
	assert(pszScript);

	m_pGC->m_ScriptManager.SafeCallBegin(&nLuaTopIndex);

	bool bRetCode = LoadFromBuffer("TEST_SUPPORT", pszScript, m_pGC->m_ScriptManager.GetScriptHolder()->GetLuaState());
	if (bRetCode)
	{
		bResult = LuaResult(m_pGC->m_ScriptManager.GetScriptHolder()->GetLuaState());
	}
	else
	{
		printf("LUA Error \n");
	}
	
	m_pGC->m_ScriptManager.SafeCallEnd(nLuaTopIndex);
	return bResult;
}