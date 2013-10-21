#ifndef _KMAP_COPY_H_
#define _KMAP_COPY_H_

#include <map>
#include <set>
#include "Global.h"
#include "KRoleDB.h"
#include "GatewayDef.h"
#include "Luna.h"

#define MAX_MAP_COPY_DATA_LEN   (1024 * 32)

class KRole;

// 关于副本的Owner问题:
// 1. 玩家换地图进入副本
//    1.1 不跨服的话,在SearchMap时即可切换Owner,或者在CreateMapRespond的时候
//    1.2 跨服的话,在ConfirmLogin的时候处理Owner切换
// 2. 玩家在副本中上线可能发生队长切换,这个就在组队代码中做了
// 3. 队长切换,这个需要在组队代码中处理,注意,队长切换在登录的时候也可能发生
// 4. 进入副本后组队(组队->进副本->解散->再组队),所以,在CreateMap的时候要处理


enum KMAP_COPY_STATE
{
    eMapStateInvalid = 0,

    eMapStateOffline,
    eMapStateCreating,
    eMapStateNormal,
    eMapStateRefreshing,
    eMapStateUnloading,

    eMapStateTotal
};

struct KCreateMapCGCallbackInfo
{
    DWORD               dwPlayerID;
    int                 nX;
    int                 nY;
    int                 nZ;
    time_t              nStartWaitTime;
};

typedef std::list<KCreateMapCGCallbackInfo, KMemory::KAllocator<KCreateMapCGCallbackInfo> > KCREATE_MAP_CG_CALLBACK_LIST;
typedef std::set<DWORD, std::less<DWORD>, KMemory::KAllocator<DWORD> > KMAP_COPY_PLAYER_LIST;
typedef std::map<DWORD, time_t, std::less<DWORD>, KMemory::KAllocator<DWORD> > KMAP_COPY_PLAYER_KEEP_LIST;

struct KMapCopy
{
    KMapCopy();
    ~KMapCopy();

    void Activate();

    BOOL RegisterCGCallbackPlayer( DWORD dwPlayerID, int nX, int nY, int nZ);

    BOOL UnregisterCGCallbackPlayer(DWORD dwPlayerID);

    BOOL ClearCGCallbackPlayer()
    {
        m_CGCallbackList.clear();
        return true;
    }

    BOOL CanReset();

    BOOL Save(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize);
    BOOL Load(BYTE* pbyData, size_t uDataLen);

    BOOL RegisterKeepPlayer(DWORD dwPlayerID);
    BOOL UnRegisterKeepPlayer(DWORD dwPlayerID);
    BOOL IsPlayerKeeped(DWORD dwPlayerID);

    int                             m_nCopyIndex;
    KMAP_COPY_STATE	                m_eState;	               // 地图副本状态
    BOOL                            m_bAutoDelete;             // 是否要自动删除
    time_t                          m_nCreateTime;             // 副本的创建时间
    time_t                          m_nUnloadTime;             // 当副本人数不足时,保持到什么时候为止
    time_t                          m_nRefreshTime;            // 当副本到什么时候开始重置
    time_t                          m_nLastSaveTime;           // 最后一次保存时间
    int				                m_nConnIndex;		       // 创建地图副本的GS的连接号
    int			                    m_nPlayerCount;	           // 在这张副本中的玩家数目
    int                             m_nMinPlayerCount;         // 在这张副本中最少需要多少玩家

    DWORD			                m_dwOwnerID;		       // 副本拥有者的角色ID
    KMAP_COPY_PLAYER_LIST           m_PlayerList;              // 拥有副本进入权限的玩家列表  
    IKG_Buffer*                     m_piSceneData;             // 副本场景数据,包含场景自定义数据和场景进度数据
    BOOL                            m_bIsChanged;              // 拥有者和地图数据是否改变   

    KCREATE_MAP_CG_CALLBACK_LIST    m_CGCallbackList;          // 地图创建过程中等待的跨地图玩家列表

    DWORD                           m_dwQueueID;               // 队伍ID 
    time_t                          m_nLastTryLoginTime;       // 上一次尝试登录的时间

    KMAP_COPY_PLAYER_KEEP_LIST      m_KeepList;

    DECLARE_LUA_CLASS(KMapCopy);
    DECLARE_LUA_INTEGER(CopyIndex);
    DECLARE_LUA_TIME(CreateTime);
    DECLARE_LUA_TIME(UnloadTime);
    DECLARE_LUA_TIME(RefreshTime);
    DECLARE_LUA_INTEGER(ConnIndex);
    DECLARE_LUA_INTEGER(PlayerCount);
    DECLARE_LUA_INTEGER(MinPlayerCount);
    DECLARE_LUA_DWORD(OwnerID);
    DECLARE_LUA_DWORD(QueueID);
};

#endif

