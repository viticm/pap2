#include "stdafx.h"
#include "KCampInfo.h"

int KCampInfo::LuaGetLevelScore(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nLevel          = 0;
    int     nMaxScore       = 0;
    int     nMinScore       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    
    nLevel = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nMaxScore = m_nLevelScore[nLevel];
    nMinScore = -m_nLevelScore[CAMP_LEVEL_COUNT - nLevel - 1];

    Lua_PushNumber(L, nMaxScore);
    Lua_PushNumber(L, nMinScore);

    nResult = 2;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetMoneyPercent(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nCamp           = 0;
    int     nLevel          = 0;
    int     nMoneyPercent   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);
    
    nLevel = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nMoneyPercent = GetMoneyPercent((KCAMP)nCamp, nLevel);
    
    Lua_PushNumber(L, nMoneyPercent);

    nResult = 1;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetReputePercent(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nCamp           = 0;
    int     nLevel          = 0;
    int     nReputePercent  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);
    
    nLevel = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nReputePercent = GetReputePercent((KCAMP)nCamp, nLevel);

    Lua_PushNumber(L, nReputePercent);

    nResult = 1;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetPrestigePercent(Lua_State* L)
{
    int     nResult             = 0;
    int     nTopIndex           = 0;
    int     nCamp               = 0;
    int     nLevel              = -1;
    int     nPrestigePercent    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);
    
    nLevel = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nPrestigePercent = GetPrestigePercent((KCAMP)nCamp, nLevel);

    Lua_PushNumber(L, nPrestigePercent);

    nResult = 1;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetReducePrestigeOnDeath(Lua_State* L)
{
    int     nResult                 = 0;
    int     nTopIndex               = 0;
    int     nCamp                   = 0;
    int     nLevel                  = -1;
    int     nReducePrestigeOnDeath  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);
    
    nLevel = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nReducePrestigeOnDeath = GetReducePrestigeOnDeath((KCAMP)nCamp, nLevel);

    Lua_PushNumber(L, nReducePrestigeOnDeath);

    nResult = 1;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetDamagePercent(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nCamp           = 0;
    int     nLevel          = -1;
    int     nDamagePercent  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);
    
    nLevel = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nDamagePercent = GetDamagePercent((KCAMP)nCamp, nLevel);
    
    Lua_PushNumber(L, nDamagePercent);

    nResult = 1;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetShieldPercent(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nCamp           = 0;
    int     nLevel          = -1;
    int     nShieldPercent  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);
    
    nLevel = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);
    
    nShieldPercent = GetShieldPercent((KCAMP)nCamp, nLevel);

    Lua_PushNumber(L, nShieldPercent);

    nResult = 1;
Exit0:
    return nResult;
}

int KCampInfo::LuaGetActivePlayerCount(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nCamp           = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil || nCamp == (int)cNeutral);

    Lua_PushNumber(L, m_nActivePlayerCount[nCamp]);

    nResult = 1;
Exit0:
    return nResult;
}

DEFINE_LUA_CLASS_BEGIN(KCampInfo)
    REGISTER_LUA_INTEGER(KCampInfo, CampLevel)
    REGISTER_LUA_INTEGER(KCampInfo, CampScore)
    REGISTER_LUA_BOOL(KCampInfo, GoodCampClosed)
    REGISTER_LUA_BOOL(KCampInfo, EvilCampClosed)
    
    REGISTER_LUA_FUNC(KCampInfo, GetLevelScore)
    REGISTER_LUA_FUNC(KCampInfo, GetMoneyPercent)
    REGISTER_LUA_FUNC(KCampInfo, GetReputePercent)
    REGISTER_LUA_FUNC(KCampInfo, GetPrestigePercent)
    REGISTER_LUA_FUNC(KCampInfo, GetReducePrestigeOnDeath)
    REGISTER_LUA_FUNC(KCampInfo, GetDamagePercent)
    REGISTER_LUA_FUNC(KCampInfo, GetShieldPercent)
    REGISTER_LUA_FUNC(KCampInfo, GetActivePlayerCount)
DEFINE_LUA_CLASS_END(KCampInfo)
