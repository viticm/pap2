#ifndef _MENTOR_CENTER_H_
#define _MENTOR_CENTER_H_

#include <map>
#include "Global.h"
#include "KRole.h"
#include "KMentorDef.h"
#include "Luna.h"

struct KMentorInfo
{
    time_t  nCreateTime;        // 拜师时间
    int     nMentorValue;       // 师徒值
    time_t  nEndTime;           // 状态的截止时间
    BYTE    byState;            // 关系解除发起方
    BYTE    byOriginalALevel;   // 拜师时徒弟的等级    
    BOOL    bSaved;
};

class KMentorCenter
{
public:
    void Activate();

    BOOL Load(DWORD dwMentorID, DWORD dwApprenticeID, BYTE* pbyData, size_t uDataLen);

    BOOL AddRecord(DWORD dwMentorID, DWORD dwApprenticeID, const KMentorInfo& crMentorInfo);
    BOOL Delete(DWORD dwMentorID, DWORD dwApprenticeID);

    int  GetRelationCount() { return (int)m_A2MTable.size(); }

    KMentorInfo* GetMentorInfo(DWORD dwMentorID, DWORD dwApprenticeID);

    int GetMentorCount(DWORD dwApprenticeID);
    int GetApprenticeCount(DWORD dwMentorID);
    int GetMentorList(DWORD dwApprenticeID, DWORD dwMentorList[MAX_MENTOR_NUM]);
    int GetApprenticeList(DWORD dwMentorID, DWORD dwApprenticeList[MAX_APPRENTICE_NUM]);

    void SaveAll();
    void CheckStateEnd();
    void Clear();

    BOOL Break(DWORD dwMentorID, DWORD dwApprenticeID, BOOL bIsMentorBreak);//x

    BOOL AddMentorValue(DWORD dwMentorID, DWORD dwApprenticeID, int nDeltaValue);

    int  GetSyncMentorRecord(uint64_t uMentorKey, KMentorSyncInfo* pRecord, int nMaxCount);

    BOOL UpdateMaxApprenticeNum(DWORD dwMentorID, int nMaxApprenticeNum);
    BOOL ClearMaxApprenticeNum(DWORD dwMentorID);
    BOOL CreateShip(DWORD dwMentorID, DWORD dwApprenticeID, BYTE byALevel);
    void ActiveShip();

private:
    // Key = MAKE_INT64(dwApprenticeID, dwMentorID)
    typedef KMemory::KAllocator<std::pair<uint64_t, KMentorInfo> > KMentorAllocator;
    typedef std::map<uint64_t, KMentorInfo, std::less<uint64_t>, KMentorAllocator> KMentorTable;
    KMentorTable m_A2MTable;

    // Key = MAKE_INT64(dwMentorID, dwApprenticeID)
    typedef KMemory::KAllocator<std::pair<uint64_t, KMentorInfo*> > KApprenticeAllocator;
    typedef std::map<uint64_t, KMentorInfo*, std::less<uint64_t>, KApprenticeAllocator> KApprenticeTable;
    KApprenticeTable m_M2ATable;

    // Key = MAKE_INT64(dwMentorID, dwApprenticeID)
    typedef KMemory::KAllocator<std::pair<uint64_t, time_t> > KApplyMentorAllocator;
    typedef std::map<uint64_t, time_t, std::less<uint64_t>, KApplyMentorAllocator> KApplyMentorTable;
    KApplyMentorTable m_ApplyMentorTable;

    // Key = MAKE_INT64(dwMentorID, dwApprenticeID)
    typedef KMemory::KAllocator<std::pair<uint64_t, time_t> > KApplyApprenticeAllocator;
    typedef std::map<uint64_t, time_t, std::less<uint64_t>, KApplyApprenticeAllocator> KApplyApprenticeTable;
    KApplyApprenticeTable m_ApplyApprenticeTable;

    typedef KMemory::KAllocator<std::pair<DWORD, int> > KMaxApprenticeNumTableAllocator;
    typedef std::map<DWORD, int, std::less<DWORD>, KMaxApprenticeNumTableAllocator> KMaxApprenticeNumTable;
    KMaxApprenticeNumTable m_MaxApprenticeNumTable;

public:
    int LuaRegisterApplyMentor(Lua_State* L);
    int LuaRegisterApplyApprentice(Lua_State* L);
    int LuaGetMentorCount(Lua_State* L);
    int LuaGetApprenticeCount(Lua_State* L);
    int LuaTakeMentor(Lua_State* L);
    int LuaTakeApprentice(Lua_State* L);
    int LuaGetMentorList(Lua_State* L);
    int LuaGetApprenticeList(Lua_State* L);
    int LuaGetRecord(Lua_State* L);
    int LuaAddMentorValue(Lua_State* L);
    int LuaBreakMentor(Lua_State* L);
    int LuaBreakApprentice(Lua_State* L);
    int LuaCancelBreak(Lua_State* L);
    int LuaAcquireMentorValue(Lua_State* L);

    int LuaSeekMentorYell(Lua_State* L);
    int LuaSeekApprenticeYell(Lua_State* L);

    int LuaApprenticeGraduate(Lua_State* L);
    int LuaGetRelationCount(Lua_State* L);

public:
    DECLARE_LUA_CLASS(KMentorCenter);
};
#endif // _MENTOR_CENTER_H_
