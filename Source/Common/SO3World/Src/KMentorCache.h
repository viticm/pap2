#ifndef _MENTOR_CACHE_H_
#define _MENTOR_CACHE_H_

#ifdef _SERVER

#include <map>
#include "Luna.h"
#include "Engine/KMemory.h"
#include "KMentorDef.h"

class KMentorCache
{
public:
    BOOL AddMentorData(DWORD dwMentor, DWORD dwApprentice, const KMentorRecordBase& crMentorInfo);
    BOOL UpdateMentorData(DWORD dwMentor, DWORD dwApprentice, const KMentorRecordBase& crMentorInfo);

    BOOL DeleteMentorRecord(uint64_t uKey);
    void Clear();
    void SyncPlayerMentorData(KPlayer* pPlayer);

    BOOL IsMentorship(DWORD dwMentorID, DWORD dwApprenticeID);

    BOOL IsMentor(DWORD dwPlayerID);
    BOOL IsApprentice(DWORD dwPlayerID);

    BOOL AddMentorValue(DWORD dwMentorID, DWORD dwApprenticeID, int nDeltaValue);

private:
    // Key = MAKE_INT64(dwApprenticeID, dwMentorID)
    typedef KMemory::KAllocator<std::pair<uint64_t, KMentorRecordBase> > KCacheMTableAlloctor;
    typedef std::map<uint64_t, KMentorRecordBase, std::less<uint64_t>, KCacheMTableAlloctor> KCacheMTable;
    KCacheMTable m_CacheMTable;

    // Key = MAKE_INT64(dwMentorID, dwApprenticeID)
    typedef KMemory::KAllocator<std::pair<uint64_t, KMentorRecordBase*> > KCacheATableAlloctor;
    typedef std::map<uint64_t, KMentorRecordBase*, std::less<uint64_t>, KCacheATableAlloctor> KCacheATable;
    KCacheATable m_CacheATable;

public:
    int LuaIsMentorship(Lua_State* L);
    int LuaAddMentorValue(Lua_State* L);
    int LuaGetMentorList(Lua_State* L);
    int LuaGetApprenticeList(Lua_State* L);
    int LuaGetMentorCount(Lua_State* L);

public:
    DECLARE_LUA_CLASS(KMentorCache);
};
#endif // _SERVER

#endif // _MENTOR_CACHE_H_
