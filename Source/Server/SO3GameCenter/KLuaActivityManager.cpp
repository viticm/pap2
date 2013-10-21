#include "stdafx.h"
#include "KActivityManager.h"
#include "KSO3GameCenter.h"

int KActivityManager::LuaAdd(Lua_State* L)
{
    BOOL            bRetCode    = false;
    int             nTopIndex   = 0;
    KActivityRecord Record;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    memset(&Record, 0, sizeof(Record));

    Record.dwRoleID = (DWORD)Lua_ValueToNumber(L, 1);
    Record.nType = (int)Lua_ValueToNumber(L, 2);

    Record.nEndTime = (time_t)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow < Record.nEndTime);

    Record.bUpdate = true;

    bRetCode = Add(Record);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KActivityManager::LuaDel(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwRoleID    = ERROR_ID;
    int     nType       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwRoleID = (DWORD)Lua_ValueToNumber(L, 1);
    nType = (int)Lua_ValueToNumber(L, 2);

    bRetCode = Delete(dwRoleID, nType);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KActivityManager::LuaGet(Lua_State* L)
{
    int                 nResult     = 0;
    int                 nTopIndex   = 0;
    DWORD               dwRoleID    = ERROR_ID;
    int                 nType       = 0;
    int                 nIndex      = 0;
    KActivityRecord*    pRecord     = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwRoleID = (DWORD)Lua_ValueToNumber(L, 1);
    nType = (int)Lua_ValueToNumber(L, 2);

    nIndex = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_ACTIVITY_VALUE_COUNT);

    pRecord = GetRecord(dwRoleID, nType);
    KG_PROCESS_ERROR(pRecord);

    Lua_PushNumber(L, pRecord->anValue[nIndex]);

    nResult = 1;
Exit0:
    return nResult;
}

int KActivityManager::LuaSet(Lua_State* L)
{
    int                 nTopIndex   = 0;
    DWORD               dwRoleID    = ERROR_ID;
    int                 nType       = 0;
    int                 nIndex      = 0;
    int                 nValue      = 0;
    KActivityRecord*    pRecord     = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    dwRoleID = (DWORD)Lua_ValueToNumber(L, 1);
    nType = (int)Lua_ValueToNumber(L, 2);

    nIndex = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_ACTIVITY_VALUE_COUNT);

    nValue = (int)Lua_ValueToNumber(L, 4);

    pRecord = GetRecord(dwRoleID, nType);
    KGLOG_PROCESS_ERROR(pRecord);

    pRecord->anValue[nIndex] = nValue;
    pRecord->bUpdate = true;

Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KActivityManager)

    REGISTER_LUA_FUNC(KActivityManager, Add)
    REGISTER_LUA_FUNC(KActivityManager, Del)
    REGISTER_LUA_FUNC(KActivityManager, Get)
    REGISTER_LUA_FUNC(KActivityManager, Set)

DEFINE_LUA_CLASS_END(KActivityManager)
