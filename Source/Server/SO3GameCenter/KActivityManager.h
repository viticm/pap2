////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KActivityManager.h
//  Version     : 1.0
//  Creator     : Yan Rui
//  Comment     : 用于记录客服活动的玩家名单和相关数据标记
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KACTIVITY_MANAGER_H_
#define _KACTIVITY_MANAGER_H_

#include <map>
#include "Luna.h"

#define MAX_ACTIVITY_VALUE_COUNT 6

struct KActivityRecord 
{
    DWORD   dwRoleID;
    int     nType;
    time_t  nEndTime;
    int     anValue[MAX_ACTIVITY_VALUE_COUNT];
    BOOL    bUpdate;
};

typedef KMemory::KAllocator<std::pair<uint64_t, KActivityRecord> > KActivityAllocator;
typedef std::map<uint64_t, KActivityRecord, std::less<uint64_t>, KActivityAllocator> KActivityTable;

class KActivityManager
{
public:
    KActivityManager();
    ~KActivityManager();

    void Activate();

    BOOL Add(const KActivityRecord& rRecord);
    BOOL Delete(DWORD dwRoleID, int nType);
    KActivityRecord* GetRecord(DWORD dwRoleID, int nType);

    int LuaAdd(Lua_State* L);
    int LuaDel(Lua_State* L);
    int LuaGet(Lua_State* L);
    int LuaSet(Lua_State* L);

private:
    KActivityTable  m_ActivityTable;
    time_t          m_nNextSaveTime;
    time_t          m_nNextCheckTimeoutTime;

public:
    DECLARE_LUA_CLASS(KActivityManager);
};

#endif
