#ifndef ENVIRONMENTCONSTRUCT_H
#define ENVIRONMENTCONSTRUCT_H

#include <Windows.h>
#include <vector>
#include <deque>
#include <map>

#define _NAME_LEN 32
#define NEUTRAL_REPUTE_LEVEL 3
#define MAX_TRACK_ID 2000

typedef std::vector<int> KOPEN_ROUTE_NODE_VECTOER;
typedef std::deque<int> KOPEN_QUEUE;
typedef std::map<int, int> KCLOSE_TABLE;

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
    int      nCostMoney;
    std::vector<KTRACK_POINT> vPoint;
};
typedef std::map<int/*nTrackID*/, KTRACK> KTrackMap;

struct KROUTE_NODE
{
    int      nNodeID;
    DWORD    dwMapID;
    DWORD    dwCityID; 
    DWORD    dwForceID;
    DWORD    dwDefaultForceID;
    DWORD    dwTongID;
    BOOL     bIsNeedOpen;
    int      nX;
    int      nY;
    int      nZ;

    std::vector<std::pair<KROUTE_NODE*, KTRACK*> > vLinkNode;    // 邻接点

    char     szDescription[_NAME_LEN];
    char     szScriptFile[MAX_PATH];
};
typedef std::map<int /*nNodeID*/, KROUTE_NODE> KRouteMap; 

template<typename T>
inline T g_GetDistance3(T nSourceX, T nSourceY, T nSourceZ, T nDestX, T nDestY, T nDestZ)
{
    T nDistance3 = 
        (nSourceX - nDestX) * (nSourceX - nDestX) + 
        (nSourceY - nDestY) * (nSourceY - nDestY) + 
        (nSourceZ - nDestZ) * (nSourceZ - nDestZ);

    return nDistance3;
}

// 桩类，用于获取声望等级
class KReputation
{
public:
    int  GetReputeLevel(DWORD dwForceID){return 2;}
};

class EnvironmentConstruct
{
public:
    EnvironmentConstruct(void){}
    ~EnvironmentConstruct(void){}

    BOOL Init();
    BOOL UnInit();

    KRouteMap GetRoutMap();
    KTrackMap GetTrackMap();

    KROUTE_NODE* GetNodeInfo(int nNode);
    KTRACK* GetTrackInfo(int nTrack);
    BOOL IsOpenRouteNode(int nRouteNode, const KOPEN_ROUTE_NODE_VECTOER* cpNodeList);

private:
    KRouteMap m_RouteMap;
    KTrackMap m_TrackMap;
};

#endif //ENVIRONMENTCONSTRUCT_H
