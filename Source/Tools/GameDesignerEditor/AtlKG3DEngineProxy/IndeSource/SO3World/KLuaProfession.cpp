
#include "stdafx.h"
#include "KProfession.h"

int KProfession::LuaGetLevelProficiency(Lua_State* L)
{
	DWORD dwResult = 0;
    DWORD dwLevel  = 0;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
	dwLevel = (DWORD)Lua_ValueToNumber(L, 1);
	dwResult = GetLevelProficiency(dwLevel);
Exit0:
	Lua_PushNumber(L, dwResult);
	return 1;
}

int KProfession::LuaGetBranchName(Lua_State* L)
{
	const char* pcszName = NULL;
	DWORD dwBranchID = 0;
	int nTopIndex = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwBranchID = (DWORD)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(dwBranchID > 0 && dwBranchID <= PROFESSION_BRANCH_COUNT);
	KG_PROCESS_ERROR(m_bBranchUsed[dwBranchID - 1]);

	pcszName = m_szBranchName[dwBranchID - 1];

	Lua_PushString(L, pcszName);
	return 1;
Exit0:
	return 0;
}


DEFINE_LUA_CLASS_BEGIN(KProfession)
	REGISTER_LUA_DWORD(KProfession, ID)
	REGISTER_LUA_DWORD(KProfession, CraftRadarID)
	REGISTER_LUA_STRING(KProfession, ProfessionName)

	REGISTER_LUA_FUNC(KProfession, GetLevelProficiency)
	REGISTER_LUA_FUNC(KProfession, GetBranchName)
DEFINE_LUA_CLASS_END(KProfession)
