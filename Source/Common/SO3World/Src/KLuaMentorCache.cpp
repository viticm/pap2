#include "stdafx.h"
#include "KMentorCache.h"
#include "KRelayClient.h"

#ifdef _SERVER

int KMentorCache::LuaIsMentorship(Lua_State* L)
{
    int     bResult         = 0;
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwMentorID      = ERROR_ID;
    DWORD   dwApprenticeID  = ERROR_ID;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    bRetCode = IsMentorship(dwMentorID, dwApprenticeID);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KMentorCache::LuaAddMentorValue(Lua_State* L)
{
    int     nResult         = 0;
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwMentorID      = ERROR_ID;
    DWORD   dwApprenticeID  = ERROR_ID;
    int     nDeltaValue     = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);
    nDeltaValue     = (int)Lua_ValueToNumber(L, 3);

    bRetCode = AddMentorValue(dwMentorID, dwApprenticeID, nDeltaValue);

    Lua_PushBoolean(L, bRetCode);
    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCache::LuaGetMentorList(Lua_State* L)
{
    int                     nResult         = 0;
    int                     nTopIndex       = 0;
    DWORD                   dwApprenticeID  = ERROR_ID;
    int                     nCount          = 1;
    uint64_t                uKeyLower       = 0;
    uint64_t                uKeyUpper       = 0;    
    KCacheMTable::iterator  itLower;
    KCacheMTable::iterator  itUpper;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwApprenticeID = (DWORD)Lua_ValueToNumber(L, 1);

    uKeyLower = MAKE_INT64(dwApprenticeID, 0);
    uKeyUpper = MAKE_INT64(dwApprenticeID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower <= uKeyUpper);

    itLower = m_CacheMTable.lower_bound(uKeyLower);
    itUpper = m_CacheMTable.lower_bound(uKeyUpper);

    Lua_NewTable(L);
    for (KCacheMTable::iterator it = itLower; it != itUpper; ++it)
    {
        DWORD dwMentorID = LOW_DWORD_IN_UINT64(it->first);
        
        Lua_PushNumber(L, nCount);
        Lua_PushNumber(L, dwMentorID);
        
        Lua_SetTable(L, -3);
        nCount++;
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCache::LuaGetApprenticeList(Lua_State* L)
{
    int                     nResult     = 0;
    int                     nTopIndex   = 0;
    DWORD                   dwMentorID  = ERROR_ID;
    int                     nCount      = 1;
    uint64_t                uKeyLower   = 0;
    uint64_t                uKeyUpper   = 0;    
    KCacheATable::iterator  itLower;
    KCacheATable::iterator  itUpper;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMentorID = (DWORD)Lua_ValueToNumber(L, 1);

    uKeyLower = MAKE_INT64(dwMentorID, 0);
    uKeyUpper = MAKE_INT64(dwMentorID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower <= uKeyUpper);

    itLower = m_CacheATable.lower_bound(uKeyLower);
    itUpper = m_CacheATable.lower_bound(uKeyUpper);

    Lua_NewTable(L);
    for (KCacheATable::iterator it = itLower; it != itUpper; ++it)
    {
        DWORD dwAppernticeID = LOW_DWORD_IN_UINT64(it->first);

        Lua_PushNumber(L, nCount);
        Lua_PushNumber(L, dwAppernticeID);

        Lua_SetTable(L, -3);
        nCount++;
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCache::LuaGetMentorCount(Lua_State* L)
{
    int                    nResult           = 0;
    int                    nTopIndex         = 0;
    int                    dwApprenticeID    = 0;
    int                    nCount            = 0;
    KCacheMTable::iterator itLower;
    KCacheMTable::iterator itUpper;  

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwApprenticeID = (DWORD)Lua_ValueToNumber(L, 1);

    itLower        = m_CacheMTable.lower_bound(MAKE_INT64(dwApprenticeID, 0));
    itUpper        = m_CacheMTable.lower_bound(MAKE_INT64(dwApprenticeID + 1, 0));

    for (KCacheMTable::iterator it = itLower; it != itUpper; it++)
    {
        int nState = it->second.byState;

        KGLOG_PROCESS_ERROR(nState > emrsInvalid && nState < emrsTotal);

        if (nState == emrsBroken || nState == emrsGraduating || nState == emrsGraduated)
            continue;

        nCount++;
    }

    nResult = nCount;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KMentorCache)
    REGISTER_LUA_FUNC(KMentorCache, IsMentorship)
    REGISTER_LUA_FUNC(KMentorCache, AddMentorValue)
    REGISTER_LUA_FUNC(KMentorCache, GetMentorList)
    REGISTER_LUA_FUNC(KMentorCache, GetApprenticeList)
    REGISTER_LUA_FUNC(KMentorCache, GetMentorCount)
DEFINE_LUA_CLASS_END(KMentorCache)

#endif // _SERVER
