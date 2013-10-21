/************************************************************************/
/* 地图相关数据								                            */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.11.19	Create												*/
/************************************************************************/
#ifndef _MAP_MANAGER_H_
#define _MAP_MANAGER_H_
#include <map>
#include <set>
#include "Global.h"
#include "KRoleDB.h"
#include "KMapInfo.h"

class KMapManager
{
public:
	KMapManager();

	BOOL Init();
    void UnInit();

    void Activate();

	KMapInfo* GetMapInfo(DWORD dwMapID);

	KMapCopy* GetMapCopy(DWORD dwMapID, int nMapCopyIndex);

	void    OnGameServerCreate(int nConnIndex);
    void    OnGameServerDestroy(int nConnIndex);

    // 在预加载表里面的副本是不能根据玩家请求动态加载的,通过这个函数检查
    BOOL    IsMapCopyInPreloadGroup(DWORD dwMapID, int nCopySN);
    
    BOOL    ResetAllMap(DWORD dwPlayerID, std::vector<DWORD>& vecFailedMap);
    BOOL    ResetMap(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex);
    BOOL    RefreshMap(DWORD dwMapID);
    // 地图副本信息相关操作
    BOOL    IncreaseMapCopyPlayerCount(DWORD dwMapID, int nCopyIndex); // ++PlayerCount;
    BOOL    DecreaseMapCopyPlayerCount(DWORD dwMapID, int nCopyIndex); // --PlayerCount;
    BOOL    SetMapCopyOwner(DWORD dwMapID, int nCopyIndex, DWORD dwOwnerID);
    BOOL    IsMapCopyCountLimit(KMapInfo* pMapInfo);
    
    // 副本进入规则1, 2, 3, 4: 按规则查找副本,见文档说明
    int     FindPlayerCopyIndex(KRole* pRole, KRole* pLeader, DWORD dwMapID);

    void    OnPlayerEnterMapCopy(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex);

    int     GetPlayerOnlineTotal() { return m_nPlayerOnlineTotal; }

    void    GetMapList(std::vector<DWORD>& rRetMap);
    
    void    ResetPlayerEnterNewCopyTime();
private:
    BOOL    LoadMapList();
    BOOL    LoadMapGroup();
    
private:

    typedef KMemory::KAllocator<std::pair<DWORD, KMapInfo> > KMAP_TABLE_ALLOCTOR;
    typedef std::map<DWORD, KMapInfo, std::less<DWORD>, KMAP_TABLE_ALLOCTOR>	KMAP_TABLE;
	KMAP_TABLE		m_MapTable;

    struct KMAP_GROUP_ITEM
    {
        DWORD dwMapID;
        int   nCopySN;
    };

    struct KMapGroup 
    {
        int                             nConnIndex; // 0表示空闲
        std::vector<KMAP_GROUP_ITEM>    MapTable;
    };

    KMapGroup       m_GroupTable[MAX_GS_COUNT];

    int    m_nPlayerOnlineTotal;
};

#endif	//_MAPDATA_H_
