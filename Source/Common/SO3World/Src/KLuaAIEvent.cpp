#include "stdafx.h"
#include "KAIEvent.h"

#ifdef _SERVER

int KAIEvent::LuaInit(Lua_State* L)
{
    DWORD   dwAction        = 0;
	BOOL    bRetCode        = FALSE;
    BOOL    bConditionLogic = FALSE;
    BOOL    bRunOnce        = FALSE;

	int nArgCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nArgCount >= 1 && nArgCount <= 3);

	dwAction = (DWORD)Lua_ValueToNumber(L, 1);

	if (nArgCount >= 2)
    {
		bConditionLogic = (BOOL)Lua_ValueToNumber(L, 2);
    }
	
	if (nArgCount >= 3)
    {
		bRunOnce = (BOOL)Lua_ValueToNumber(L, 3);
    }

	bRetCode = Init((AI_EVENT_ACTION)dwAction, bConditionLogic, bRunOnce);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	lua_pushboolean(L, bRetCode);
	return 1;
}

int KAIEvent::LuaSetCDFrame(Lua_State* L)
{
	BOOL    bRetCode = FALSE;
    int     nCDFrame = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
	
	nCDFrame = (int)Lua_ValueToNumber(L, 1);

	bRetCode = SetCDFrame(nCDFrame);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	lua_pushboolean(L, bRetCode);
	return 1;
}

int KAIEvent::LuaAddCondition(Lua_State* L)
{
	BOOL    bRetCode    = FALSE;
    DWORD   dwCondition = 0;
    int     nValue      = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwCondition	= (DWORD)Lua_ValueToNumber(L, 1);
	nValue		= (int)Lua_ValueToNumber(L, 2);

	bRetCode = AddCondition((AI_EVENT_CONDITION)dwCondition, nValue);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	lua_pushboolean(L, bRetCode);
	return 1;
}

int KAIEvent::LuaGetConditionCount(Lua_State* L)
{
	int nResult = 0;
	
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	nResult = GetConditionCount();

Exit0:
	lua_pushinteger(L, nResult);
	return 1;
}

int KAIEvent::LuaSetEscapeChance(Lua_State* L)
{
	BOOL bRetCode       = FALSE;
    int  nEscapeChance  = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nEscapeChance = (int)Lua_ValueToNumber(L, 1);

	bRetCode = m_Action.SetEscapeChance(nEscapeChance);
	KG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	lua_pushboolean(L, bRetCode);
	return 1;
}

int KAIEvent::LuaSetCallHelpRange(Lua_State* L)
{
	BOOL bRetCode       = FALSE;
    int  nCallHelpRange = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nCallHelpRange = (int)Lua_ValueToNumber(L, 1);

	bRetCode = m_Action.SetCallHelpRange(nCallHelpRange);
	KG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	lua_pushboolean(L, bRetCode);
	return 1;
}

int KAIEvent::LuaSetScriptName(Lua_State* L)
{
	BOOL        bRetCode      = FALSE;
    const char* pszScriptName = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	pszScriptName = Lua_ValueToString(L, 1);

	bRetCode = m_Action.SetScriptName(pszScriptName);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	lua_pushboolean(L, bRetCode);
	return 1;
}

DEFINE_LUA_CLASS_BEGIN(KAIEvent)
	REGISTER_LUA_BOOL_READONLY(KAIEvent, IsActive)

	REGISTER_LUA_FUNC(KAIEvent, Init)
	REGISTER_LUA_FUNC(KAIEvent, SetCDFrame)
	REGISTER_LUA_FUNC(KAIEvent, AddCondition)
	REGISTER_LUA_FUNC(KAIEvent, GetConditionCount)

	REGISTER_LUA_FUNC(KAIEvent, SetEscapeChance)
	REGISTER_LUA_FUNC(KAIEvent, SetCallHelpRange)
	REGISTER_LUA_FUNC(KAIEvent, SetScriptName)
DEFINE_LUA_CLASS_END(KAIEvent)

#endif //_SERVER
