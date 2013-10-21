#ifndef _KROAD_MANAGER_H_
#define _KROAD_MANAGER_H_

#include <vector>
#include <map>
#include <deque>
#include <set>

#include "Luna.h"
#include "SO3Result.h"

#define MAX_TRACK_ID    2000

class KPlayer;
class KReputation;

typedef std::vector<int> KOPEN_ROUTE_NODE_VECTOER;

struct KTRACK_POINT
{
    int     nFrame;
    int     nX;
    int     nY;
    int     nZ;
};

struct KTRACK
{
   int      nTrackID;
   int      nFromNode;
   int      nToNode;
   int      nVelocity;

   int      nFrame; // 此轨迹总共帧数
   int     	nCostMoney;
   std::vector<KTRACK_POINT> vPoint;
};
typedef std::map<int/*nTrackID*/, KTRACK> KTrackMap;

struct KPOSITION
{
    int     nDirection;
    int     nVelocity;
    int     nX;
    int     nY;
    int     nZ;
};

struct KROUTE_NODE
{
   int      nNodeID;

   // 此RouteID所在的地图。
   // 如果前后两个RouteNode的dwMapID不一致，程序将会自动切换地图。
   DWORD    dwMapID;
   DWORD    dwCityID;   // 作为目的地判断依据
   KCAMP    eCamp;
   KCAMP    eDefaultCamp;
   DWORD    dwTongID;
   BOOL     bEnable;
   
   BOOL     bIsNeedOpen;

   int      nX;
   int      nY;
   int      nZ;

   std::vector<std::pair<KROUTE_NODE*, KTRACK*> > vLinkNode;    // 邻接点

   char     szDescription[_NAME_LEN];
   char     szScriptFile[MAX_PATH];

   DECLARE_LUA_CLASS(KROUTE_NODE);
   DECLARE_LUA_STRUCT_DWORD(MapID, dwMapID);
   DECLARE_LUA_STRUCT_DWORD(CityID, dwCityID);
   DECLARE_LUA_STRUCT_ENUM(Camp, eCamp);
   DECLARE_LUA_STRUCT_DWORD(TongID, dwTongID);

   DECLARE_LUA_STRUCT_BOOL(IsNeedOpen, bIsNeedOpen);

   DECLARE_LUA_STRUCT_INTEGER(X, nX);
   DECLARE_LUA_STRUCT_INTEGER(Y, nY);
   DECLARE_LUA_STRUCT_INTEGER(Z, nZ);
};
typedef std::map<int /*nNodeID*/, KROUTE_NODE> KRouteMap; 

class KRoadManager
{
public:
    BOOL Init();

    void UnInit();

    KROUTE_NODE* GetNextNode(int nFromNode, int nTrackID);

    BOOL GetNextTrack(
        int* pnRetTrack, int nFromNode,
        int nCurrentNode, int nTargetCity,
        KCAMP eCamp, DWORD dwTongID,
        const KOPEN_ROUTE_NODE_VECTOER* cpOpenList
    );

    ROAD_TRACK_RESULT_CODE IsReachable(
        int* pnRetCostMoney,
        int nFromNode, int nTargetCity,
        KCAMP eCamp, DWORD dwTongID,
        const KOPEN_ROUTE_NODE_VECTOER* cpOpenList
    );

    BOOL CalcCostMoney(int* pnRetCost, int nFromNode, int nTargetCity, KPlayer* pTraveller);

    BOOL GetPosition(KPOSITION* pRetPosition, int nTrackID, int nFrame, int nFromNode);

    KROUTE_NODE* GetNodeInfo(int nNode);

    KTRACK* GetTrackInfo(int nTrack);
    KTRACK* GetTrackInfo(int nFromNodeID, int nToNodeID);

    BOOL IsOpenRouteNode(int nRouteNode, const KOPEN_ROUTE_NODE_VECTOER* cpNodeList);

    KROUTE_NODE* GetTargetNodeByCity(DWORD dwCity, KCAMP eCamp);

    template<class TFunc>
    BOOL TraverseNode(DWORD dwMapID, TFunc& Func);

    template<class TFunc>
    BOOL TraverseNode(TFunc& Func);

    template<class TFunc>
    BOOL TraverseTrack(TFunc& Func);

    //////////////////////////////////////////////////////////////////////////
    // Editor
    BOOL AddNode(int* pnRetNodeID);
    BOOL AddTrack(int* pnRetTrackID, int nNodeFrom, int nNodeTo);

    BOOL CanDelNode(int nNodeID);
    BOOL CanDelTrack(int nTrackID);

    BOOL DelNode(int nNodeID);
    BOOL DelNodeWithTrack(int nNodeID);
    BOOL DelTrack(int nTrackID);


    BOOL Clear(DWORD dwMapID);

    BOOL ExportTable();
    //////////////////////////////////////////////////////////////////////////

private:

    static BOOL PointFrameCmp(const KTRACK_POINT& crPointFrom, const KTRACK_POINT& crPointTo);

    BOOL ShortestPath(
        int *pnRetMiniCost,
        int nFromNodeID, int nTargetCity,
        int nExcludeNodeID, KCAMP eCamp,
        const KOPEN_ROUTE_NODE_VECTOER* cpOpenNodeList
    );
    BOOL LoadRouteNode(const char cszFileName[]);
    BOOL LoadTrack(const char cszFileName[]);
    BOOL LoadTrackList(const char cszFileName[]);

    //////////////////////////////////////////////////////////////////////////
    // Editor
    BOOL ExportNodeTable(const char cszFileName[]);
    BOOL ExportTrackTable(const char cszFileName[]);
    BOOL ExportPointTable(const char cszFileName[]);
    //////////////////////////////////////////////////////////////////////////

private:

    KTrackMap m_TrackMap;
    KRouteMap m_RouteMap;
};

template<class TFunc>
BOOL KRoadManager::TraverseNode(DWORD dwMapID, TFunc& Func)
{
    BOOL                bResult     = false;
    BOOL                bContinue   = false;
    KRouteMap::iterator it          = m_RouteMap.begin();
    KRouteMap::iterator itEnd       = m_RouteMap.end();

    assert(dwMapID != ERROR_ID);

    while (it != itEnd)
    {
        if (it->second.dwMapID != dwMapID)
            continue;

        bContinue = Func(&(it->second));
        if (!bContinue)
            break;

        ++it;
    }

    bResult = true;
    //Exit0:
    return bResult;
}

template<class TFunc>
BOOL KRoadManager::TraverseNode(TFunc& Func)
{
    BOOL                bResult     = false;
    BOOL                bContinue   = false;
    KRouteMap::iterator it          = m_RouteMap.begin();
    KRouteMap::iterator itEnd       = m_RouteMap.end();

    while (it != itEnd)
    {
        bContinue = Func(&(it->second));
        if (!bContinue)
            break;

        ++it;
    }

    bResult = true;
    //Exit0:
    return bResult;
}

template<class TFunc>
BOOL KRoadManager::TraverseTrack(TFunc& Func)
{
    BOOL                bResult     = false;
    BOOL                bContinue   = false;
    KTrackMap::iterator it          = m_TrackMap.begin();
    KTrackMap::iterator itEnd       = m_TrackMap.end();

    while (it != itEnd)
    {
        bContinue = Func(&(it->second));
        if (!bContinue)
            break;

        ++it;
    }

    bResult = true;
    //Exit0:
    return bResult;
}

#endif
