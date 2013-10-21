
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

DEFINE_LUA_CLASS_BEGIN(KProfession)
	REGISTER_LUA_DWORD(KProfession, ID)
	REGISTER_LUA_DWORD(KProfession, CraftRadarID)
	REGISTER_LUA_STRING(KProfession, ProfessionName)

	REGISTER_LUA_FUNC(KProfession, GetLevelProficiency)
DEFINE_LUA_CLASS_END(KProfession)
