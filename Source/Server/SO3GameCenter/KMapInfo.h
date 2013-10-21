#ifndef _KMAP_INFO_H_
#define _KMAP_INFO_H_

#include "KMapCopy.h"
#include "Luna.h"

typedef KMemory::KAllocator<std::pair<int, KMapCopy> > KMAP_COPY_TABLE_ALLOCTOR;
typedef std::map<int, KMapCopy, std::less<int>, KMAP_COPY_TABLE_ALLOCTOR> KMAP_COPY_TABLE;

struct KPLAYER_ENTER_NEW_COPY_TIMES
{
    int     nCopyIndex;
    int     nTimes;
};

typedef KMemory::KAllocator<std::pair<DWORD, KPLAYER_ENTER_NEW_COPY_TIMES> > KNEW_MAP_LIMIT_TABLE_ALLOCTOR;
typedef std::map<DWORD, KPLAYER_ENTER_NEW_COPY_TIMES, std::less<DWORD>, KNEW_MAP_LIMIT_TABLE_ALLOCTOR> KNEW_MAP_LIMIT_TABLE;

struct KMapInfo
{
    KMapInfo();

    KMapCopy* GetMapCopy(int nMapCopyIndex)
    {
        KMAP_COPY_TABLE::iterator it = m_CopyTable.find(nMapCopyIndex);
        if (it != m_CopyTable.end())
        {
            return &(it->second);
        }
        return NULL;
    }

    void Activate();

    void Refresh();

    KMapCopy* CreateMapCopy(
        int     nMapCopyIndex,
        time_t  nCreateTime,
        time_t  nLastSaveTime,
        int     nConnIndex, 
        BOOL    bAutoDelete
    );

    BOOL    DeleteMapCopy(int nMapCopyIndex);

    void    DeleteAllCopyOnConnection(int nConnection);

    int     GetOwnerCopy(DWORD dwPlayerID);
    int     GetCopyIndexByPlayerList(DWORD dwPlayerID);

    // 玩家进入一个副本的时候,判断副本的新Owner
    DWORD   GetNewOwnerForPlayerEnter(KRole* pRole, int nCopyIndex);
    BOOL    CheckPlayerEnterNewCopyTimes(DWORD dwPlayerID, int nNewCopyIndex);
    void    AddPlayerEnterNewCopyTimes(DWORD dwPlayerID, int nNewCopyIndex);
    BOOL    GetPlayerSavedCopys(DWORD dwPlayerID, std::vector<int>& vecCopys);

    int LuaGetMapCopyList(Lua_State* L);
    int LuaGetMapCopy(Lua_State* L);

    DWORD                   m_dwMapID;
    char                    m_szName[_NAME_LEN];
    char                    m_szSequenceName[_NAME_LEN];    // 获取唯一序列号用的名字
    int		                m_nSNGenerator;         // 新手村,正常场景,战场的下一个序号,任务副本代表本次运行已创建过的数量+1
    int                     m_nType;                // Dungeon 才有Owner
    int                     m_nMinPlayerCount;      // 开新副本的最低人数,预加载副本不受此限制,如果这个值大于1,则需排队
    int                     m_nMaxPlayerCount;      // 副本人数最大值,如果超过这个值,则之后进入者需排队等待
    int                     m_nMaxCopyCount;
    int                     m_nKeepTime;            // 无人状态在内存中的保留时间,表里填的单位: 分钟
    int                     m_bManualReset;         // 是否可以手动重置
    int                     m_nRefreshCycle;        // 系统自动刷新该地图的周期，表里填的单位：分钟，读表的时候已经转成秒了
    BOOL                    m_bUseLastEntry;        // 是否使用上一个入口点
    int                     m_nLimitTimes;          // 能进入本场景的新副本的次数限制
    time_t                  m_nNextRefreshTime;     // 上一次系统自动刷新该地图的时间
    int                     m_nPlayerCount;
    int                     m_nLoginIntervalTime;   // 登陆间隔时间
    int                     m_nPreQueuePlayerCount; // 当玩家人数超过这个值时 启动间隔时间排队机制
    KMAP_COPY_TABLE	        m_CopyTable;
    KNEW_MAP_LIMIT_TABLE    m_PlayerEnterNewCopyTimes;

    DECLARE_LUA_CLASS(KMapInfo);

    DECLARE_LUA_DWORD(MapID);
    DECLARE_LUA_STRING(Name, sizeof(m_szName));
    DECLARE_LUA_INTEGER(SNGenerator);
    DECLARE_LUA_INTEGER(Type);
    DECLARE_LUA_INTEGER(MinPlayerCount);
    DECLARE_LUA_INTEGER(MaxPlayerCount);
    DECLARE_LUA_INTEGER(MaxCopyCount);
    DECLARE_LUA_INTEGER(KeepTime);
    DECLARE_LUA_INTEGER(RefreshCycle);
    DECLARE_LUA_INTEGER(LimitTimes);
    DECLARE_LUA_TIME(NextRefreshTime);
    DECLARE_LUA_INTEGER(PlayerCount);
    DECLARE_LUA_INTEGER(LoginIntervalTime);
    DECLARE_LUA_INTEGER(PreQueuePlayerCount);
};

#endif

