#include "stdafx.h"
#include <math.h>
#include <algorithm>
#include "KPlayer.h"
#include "KMath.h"
#include "KReputation.h"
#include "KSO3World.h"

#define ROAD_SCRIPT_PATH        "scripts/Road"

#define ROAD_ROUTE_NODE_TAB     "Road/RoadNode.tab"
#define ROAD_TRACK_TAB          "Road/RoadTrack.tab"
#define ROAD_TAB                "Road/RoadPoint.tab"

#define COL_NAME_NODE_ID        "NodeID"
#define COL_NAME_DESCRIPTION    "Description"
#define COL_NAME_MAP_ID         "MapID"
#define COL_NAME_CITY_ID        "CityID"
#define COL_NAME_FORCE_ID       "ForceID"
#define COL_NAME_CAMP           "Camp"
#define COL_NAME_TONG_ID        "TongID"
#define COL_NAME_NEED_OPEN      "NeedOpen"
#define COL_NAME_SCRIPT_FILE    "ScriptFile"
#define COL_NAME_X              "X"
#define COL_NAME_Y              "Y"
#define COL_NAME_Z              "Z"
#define COL_NAME_FROM_NODE_ID   "FromNodeID"
#define COL_NAME_TO_NODE_ID     "ToNodeID"
#define COL_NAME_COST_MONEY     "CostMoney"
#define COL_NAME_TRACK_ID       "TrackID"
#define COL_NAME_VELOCITY       "Velocity"


typedef std::deque<int> KOPEN_QUEUE;
typedef std::map<int, int> KCLOSE_TABLE;

BOOL KRoadManager::Init()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    char szPath[PATH_MAX];

    snprintf(szPath, sizeof(szPath), "%s/%s", SETTING_DIR, ROAD_ROUTE_NODE_TAB);
    szPath[sizeof(szPath) - 1] = '\0';
    bRetCode = LoadRouteNode(szPath);
    KGLOG_PROCESS_ERROR(bRetCode && "Load RouteNode");

    snprintf(szPath, sizeof(szPath), "%s/%s", SETTING_DIR, ROAD_TRACK_TAB);
    szPath[sizeof(szPath) - 1] = '\0';
    bRetCode = LoadTrack(szPath);
    KGLOG_PROCESS_ERROR(bRetCode && "Load Track");

    snprintf(szPath, sizeof(szPath), "%s/%s", SETTING_DIR, ROAD_TAB);
    szPath[sizeof(szPath) - 1] = '\0';
    bRetCode = LoadTrackList(szPath);
    KGLOG_PROCESS_ERROR(bRetCode && "Load Point");

    bResult = true;
Exit0:
    return bResult;
}

void KRoadManager::UnInit()
{
    // ExportTable();

    m_RouteMap.clear();
    m_TrackMap.clear();
}

BOOL KRoadManager::LoadRouteNode(const char cszFileName[])
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    ITabFile*   piTableFile = NULL;
    int         nTabHeight  = 0;
    KROUTE_NODE RouteNode;
    char        szScriptFile[MAX_PATH];

    piTableFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTableFile);

    nTabHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 0);

    for (int i = 2; i <= nTabHeight; i++)
    {
        bRetCode = piTableFile->GetInteger(i, COL_NAME_NODE_ID, -1, &RouteNode.nNodeID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get NodeID");

        bRetCode = piTableFile->GetString(i, COL_NAME_DESCRIPTION, "", RouteNode.szDescription, sizeof(RouteNode.szDescription));
        KGLOG_PROCESS_ERROR(bRetCode && "Get Description");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_MAP_ID, 0, (int*)&RouteNode.dwMapID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get MapID");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_CITY_ID, 0, (int*)&RouteNode.dwCityID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get CityID");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_CAMP, 0, (int*)&RouteNode.eCamp);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Camp");

        RouteNode.eDefaultCamp = RouteNode.eCamp;

        bRetCode = piTableFile->GetInteger(i, COL_NAME_TONG_ID, 0, (int*)&RouteNode.dwTongID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get TongID");

        bRetCode = piTableFile->GetString(i, COL_NAME_SCRIPT_FILE, "", szScriptFile, sizeof(szScriptFile));
        KGLOG_PROCESS_ERROR(bRetCode);

        RouteNode.szScriptFile[0] = '\0';
        if (szScriptFile[0] != '\0')
        {
            snprintf(RouteNode.szScriptFile, sizeof(RouteNode.szScriptFile), "%s", szScriptFile);
            RouteNode.szScriptFile[sizeof(RouteNode.szScriptFile) - 1] = '\0';
        }

        bRetCode = piTableFile->GetInteger(i, COL_NAME_NEED_OPEN, false, &RouteNode.bIsNeedOpen);
        KGLOG_PROCESS_ERROR(bRetCode && "Get NeedOpen");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_X, 0, &RouteNode.nX);
        KGLOG_PROCESS_ERROR(bRetCode && "Get X");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_Y, 0, &RouteNode.nY);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Y");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_Z, 0, &RouteNode.nZ);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Z");

        if (!RouteNode.bIsNeedOpen && RouteNode.eCamp != 0)
        {
            KGLogPrintf(KGLOG_ERR, "Route Node ID %d, Camp Error.\n", RouteNode.nNodeID);
        }

        RouteNode.bEnable = true;
        RouteNode.bChange = false;

        m_RouteMap.insert(std::make_pair(RouteNode.nNodeID, RouteNode));
    }

    bResult = true;
Exit0:

    if (!bResult)
    {
        m_RouteMap.clear();
    }
    KG_COM_RELEASE(piTableFile);
    return bResult;
}

BOOL KRoadManager::LoadTrack(const char cszFileName[])
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    ITabFile*                       piTableFile     = NULL;
    int                             nTabHeight      = 0;
    int                             nTempNode       = 0;
    KTRACK*                         pThisTrack      = NULL;
    KROUTE_NODE*                    pFromRouteNode  = NULL;
    KROUTE_NODE*                    pToRouteNode    = NULL;
    KTRACK                          Track;
    pair<KTrackMap::iterator, bool> itRetCode;

    piTableFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTableFile);

    nTabHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 0);

    for (int i = 2; i <= nTabHeight; i++)
    {
        bRetCode = piTableFile->GetInteger(i, COL_NAME_FROM_NODE_ID, 0, &Track.nFromNode);
        KGLOG_PROCESS_ERROR(bRetCode && "Get FromNodeID");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_TO_NODE_ID, 0, &Track.nToNode);
        KGLOG_PROCESS_ERROR(bRetCode && "Get ToNodeID");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_COST_MONEY, 0, &Track.nCostMoney);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Cost Money");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_TRACK_ID, 0, &Track.nTrackID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get TrackID");

        KGLOG_PROCESS_ERROR(Track.nTrackID > 0 && Track.nTrackID <= MAX_TRACK_ID);

        bRetCode = piTableFile->GetInteger(i, COL_NAME_VELOCITY, 0, &Track.nVelocity);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Velocity");

        KGLOG_PROCESS_ERROR(Track.nVelocity > 0);

        Track.nFrame = 0;   //  这里置0，将在读入路点的时候，累计每个路点的时间。

        itRetCode = m_TrackMap.insert(std::make_pair(Track.nTrackID, Track));
        KGLOG_PROCESS_ERROR(itRetCode.second);

        pThisTrack      = GetTrackInfo(Track.nTrackID);
        KGLOG_PROCESS_ERROR(pThisTrack);

        pFromRouteNode  = GetNodeInfo(Track.nFromNode);
        KGLOG_PROCESS_ERROR(pFromRouteNode);

        pToRouteNode    = GetNodeInfo(Track.nToNode);
        KGLOG_PROCESS_ERROR(pToRouteNode);
        
        pFromRouteNode->vLinkNode.push_back(std::make_pair(pToRouteNode, pThisTrack));
        pToRouteNode->vLinkNode.push_back(std::make_pair(pFromRouteNode, pThisTrack));
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_TrackMap.clear();
    }
    
    KG_COM_RELEASE(piTableFile);
    return bResult;
}

BOOL KRoadManager::LoadTrackList(const char cszFileName[])
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    ITabFile*       piTableFile     = NULL;
    int             nTrackID        = 0;
    int             nTabHeight      = 0;
    int             nDistance       = 0;
    int             nCurrentFrame   = 0;
    KTRACK*         pTrack          = NULL;
    KTRACK_POINT    PrevTrackPoint;
    KTRACK_POINT    TrackPoint;

    piTableFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTableFile);

    nTabHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 0);

    for (int i = 2; i <= nTabHeight; i++)
    {
        bRetCode = piTableFile->GetInteger(i, COL_NAME_TRACK_ID, 0, &nTrackID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get TrackID");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_X, 0, &TrackPoint.nX);
        KGLOG_PROCESS_ERROR(bRetCode && "Get X");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_Y, 0, &TrackPoint.nY);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Y");

        bRetCode = piTableFile->GetInteger(i, COL_NAME_Z, 0, &TrackPoint.nZ);
        KGLOG_PROCESS_ERROR(bRetCode && "Get Z");

        pTrack = GetTrackInfo(nTrackID);
        KGLOG_PROCESS_ERROR(pTrack);

        if (pTrack->vPoint.empty())
        {
            pTrack->nFrame = 0;
            TrackPoint.nFrame = 0;
            pTrack->vPoint.push_back(TrackPoint);
            continue;
        }

        PrevTrackPoint = pTrack->vPoint.back();

        nDistance = g_GetDistance3(
            PrevTrackPoint.nX, PrevTrackPoint.nY, ZPOINT_TO_XYPOINT(PrevTrackPoint.nZ),
            TrackPoint.nX, TrackPoint.nY, ZPOINT_TO_XYPOINT(TrackPoint.nZ)
        );
        nCurrentFrame = (int)sqrt((double)nDistance) / pTrack->nVelocity;

        pTrack->nFrame += nCurrentFrame;
        TrackPoint.nFrame = pTrack->nFrame;

        pTrack->vPoint.push_back(TrackPoint);
    }

    bResult = true;
Exit0:
    if (!bResult && pTrack)
        pTrack->vPoint.clear();

    KG_COM_RELEASE(piTableFile);
    return bResult;
}

struct SAVE_ROUTE_NODE 
{
    SAVE_ROUTE_NODE()
    {
        piTableFile = NULL;
        nRow        = 2;
    }

    BOOL operator () (KROUTE_NODE* pNode)
    {
        BOOL bContinue  = false;
        BOOL bRetCode   = false;

        assert(pNode);
        assert(piTableFile);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_NODE_ID, pNode->nNodeID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteString(nRow, COL_NAME_DESCRIPTION, pNode->szDescription);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_MAP_ID, pNode->dwMapID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_CITY_ID, pNode->dwCityID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_CAMP, pNode->eCamp);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_TONG_ID, pNode->dwTongID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_NEED_OPEN, pNode->bIsNeedOpen);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteString(nRow, COL_NAME_SCRIPT_FILE, pNode->szScriptFile);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_X, pNode->nX);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_Y, pNode->nY);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_Z, pNode->nZ);
        KGLOG_PROCESS_ERROR(bRetCode);

        nRow++;

        bContinue = true;
    Exit0:
        return bContinue;
    }

    int         nRow;
    ITabFile*   piTableFile;
};

BOOL KRoadManager::ExportNodeTable(const char cszFileName[])
{
    BOOL            bResult     = false;
    int             nRetCode    = false;
    ITabFile*       piTableFile = NULL;
    SAVE_ROUTE_NODE SaveRouteNode;

    piTableFile = g_OpenTabFile(cszFileName, false, true);
    KGLOG_PROCESS_ERROR(piTableFile);

    piTableFile->Clear();

    //////////////////////////////////////////////////////////////////////////
    nRetCode = piTableFile->InsertNewCol(COL_NAME_NODE_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_DESCRIPTION);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_MAP_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_CITY_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_CAMP);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_TONG_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_NEED_OPEN);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_SCRIPT_FILE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_X);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_Y);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_Z);
    KGLOG_PROCESS_ERROR(nRetCode);
    //////////////////////////////////////////////////////////////////////////

    SaveRouteNode.piTableFile = piTableFile;
    nRetCode = TraverseNode(SaveRouteNode);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->Save(cszFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Save RoadNode.tab");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTableFile);
    return bResult;
}

struct SAVE_TRACK 
{
    SAVE_TRACK()
    {
        piTableFile = NULL;
        nRow        = 2;
    }

    BOOL operator () (KTRACK* pTrack)
    {
        BOOL bContinue  = false;
        BOOL bRetCode   = false;

        assert(pTrack);
        assert(piTableFile);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_FROM_NODE_ID, pTrack->nFromNode);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_TO_NODE_ID, pTrack->nToNode);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_COST_MONEY, pTrack->nCostMoney);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_TRACK_ID, pTrack->nTrackID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_VELOCITY, pTrack->nVelocity);
        KGLOG_PROCESS_ERROR(bRetCode);

        nRow++;

        bContinue = true;
    Exit0:
        return bContinue;
    }

    int         nRow;
    ITabFile*   piTableFile;
};

BOOL KRoadManager::ExportTrackTable(const char cszFileName[])
{
    BOOL            bResult     = false;
    int             nRetCode    = false;
    ITabFile*       piTableFile = NULL;
    SAVE_TRACK      SaveTrack;

    piTableFile = g_OpenTabFile(cszFileName, false, true);
    KGLOG_PROCESS_ERROR(piTableFile);

    piTableFile->Clear();

    //////////////////////////////////////////////////////////////////////////
    nRetCode = piTableFile->InsertNewCol(COL_NAME_FROM_NODE_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_TO_NODE_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_COST_MONEY);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_TRACK_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_VELOCITY);
    KGLOG_PROCESS_ERROR(nRetCode);
    //////////////////////////////////////////////////////////////////////////

    SaveTrack.piTableFile = piTableFile;
    nRetCode = TraverseTrack(SaveTrack);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->Save(cszFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Save RoadTrack.tab");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTableFile);
    return bResult;
}

struct SAVE_TRACK_POINT 
{
    SAVE_TRACK_POINT()
    {
        piTableFile = NULL;
        nRow        = 2;
    }

    BOOL operator () (KTRACK* pTrack)
    {
        BOOL bContinue  = false;
        BOOL bRetCode   = false;
        std::vector<KTRACK_POINT>::iterator it;
        std::vector<KTRACK_POINT>::iterator itEnd;

        assert(pTrack);
        assert(piTableFile);

        it = pTrack->vPoint.begin();
        itEnd = pTrack->vPoint.end();

        while (it != itEnd)
        {
            bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_TRACK_ID, pTrack->nTrackID);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_X, it->nX);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_Y, it->nY);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = piTableFile->WriteInteger(nRow, COL_NAME_Z, it->nZ);
            KGLOG_PROCESS_ERROR(bRetCode);

            nRow++;
            ++it;
        }

        bContinue = true;
    Exit0:
        return bContinue;
    }

    int         nRow;
    ITabFile*   piTableFile;
};

BOOL KRoadManager::ExportPointTable(const char cszFileName[])
{
    BOOL                bResult     = false;
    int                 nRetCode    = false;
    ITabFile*           piTableFile = NULL;
    SAVE_TRACK_POINT    SaveTrackPoint;

    piTableFile = g_OpenTabFile(cszFileName, false, true);
    KGLOG_PROCESS_ERROR(piTableFile);

    piTableFile->Clear();

    //////////////////////////////////////////////////////////////////////////
    nRetCode = piTableFile->InsertNewCol(COL_NAME_TRACK_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_X);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_Y);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->InsertNewCol(COL_NAME_Z);
    KGLOG_PROCESS_ERROR(nRetCode);
    //////////////////////////////////////////////////////////////////////////

    SaveTrackPoint.piTableFile = piTableFile;
    nRetCode = TraverseTrack(SaveTrackPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTableFile->Save(cszFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Save RoadPoint.tab");

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTableFile);
    return bResult;
}


KROUTE_NODE* KRoadManager::GetNodeInfo(int nNode)
{
    KROUTE_NODE*        pResult = NULL;
    KRouteMap::iterator itFind;

    itFind = m_RouteMap.find(nNode);
    KG_PROCESS_ERROR(itFind != m_RouteMap.end());

    pResult = &(itFind->second);
Exit0:
    return pResult;
}

KTRACK* KRoadManager::GetTrackInfo(int nTrack)
{
    KTRACK*             pResult = NULL;
    KTrackMap::iterator itFind;

    KGLOG_PROCESS_ERROR(nTrack <= MAX_TRACK_ID);

    itFind = m_TrackMap.find(nTrack);
    KG_PROCESS_ERROR(itFind != m_TrackMap.end());

    pResult = &(itFind->second);
Exit0:
    return pResult;
}

struct _FIND_TRACK_BY_NODE_ID 
{
public:
    _FIND_TRACK_BY_NODE_ID(int nFromNodeID, int nToNodeID)
    {
        m_nFromNodeID   = nFromNodeID;
        m_nToNodeID     = nToNodeID;
    }

    BOOL operator () (std::pair<const int, KTRACK>& rTrackPair)
    {
        BOOL    bResult     = false;
        int     nFromNodeID = rTrackPair.second.nFromNode;
        int     nToNodeID   = rTrackPair.second.nToNode;

        KG_PROCESS_ERROR(nFromNodeID == m_nFromNodeID);
        KG_PROCESS_ERROR(nToNodeID == m_nToNodeID);

        bResult = true;
    Exit0:
        return bResult;
    }

private:
    int m_nFromNodeID;
    int m_nToNodeID;
};

KTRACK* KRoadManager::GetTrackInfo(int nFromNodeID, int nToNodeID)
{
    KTRACK*                 pResult = NULL;
    KTrackMap::iterator     itFind;
    _FIND_TRACK_BY_NODE_ID  FindTrackByNodeID(nFromNodeID, nToNodeID);

    itFind = std::find_if(m_TrackMap.begin(), m_TrackMap.end(), FindTrackByNodeID);
    KG_PROCESS_ERROR(itFind != m_TrackMap.end());

    pResult = &(itFind->second);
Exit0:
    return pResult;
}

KROUTE_NODE* KRoadManager::GetNextNode(int nFromNode, int nTrackID)
{
    KROUTE_NODE*                                                pResult     = NULL;
    KROUTE_NODE*                                                pFromNode   = NULL;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    itFind;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    itEnd;

    pFromNode = GetNodeInfo(nFromNode);
    KGLOG_PROCESS_ERROR(pFromNode);

    itEnd = pFromNode->vLinkNode.end();
    for (itFind = pFromNode->vLinkNode.begin(); itFind != itEnd; ++itFind)
    {
        if (itFind->second->nTrackID == nTrackID)
        {
            break;
        }
    }
    KG_PROCESS_ERROR(itFind == itEnd);

    pResult = (itFind->first);
Exit0:
    return pResult;
}

BOOL KRoadManager::GetPosition(KPOSITION* pRetPosition, int nTrackID, int nFrame, int nFromNode)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    int                                 nDeltaFrame = 0;
    int                                 nFromFrame  = 0;
    int                                 nToFrame    = 0;
    KTRACK*                             pTrack      = NULL;
    KROUTE_NODE*                        pFromNode   = NULL;
    KTRACK_POINT                        PointNow;
    KTRACK_POINT                        PointFrom;
    KTRACK_POINT                        PointTo;
    std::vector<KTRACK_POINT>::iterator it;

    KGLOG_PROCESS_ERROR(pRetPosition);

    pTrack = GetTrackInfo(nTrackID);
    KGLOG_PROCESS_ERROR(pTrack);
    KG_PROCESS_ERROR(!pTrack->vPoint.empty());

    // 速度是绑在路上的，和时间无关
    pRetPosition->nVelocity = pTrack->nVelocity;

    // 逆向
    if (nFromNode == pTrack->nToNode)
    {
        nFrame = pTrack->nFrame - nFrame;

        assert(nFrame >= 0);
    }

    // 找到位置
    PointNow.nFrame = nFrame;
    it = std::lower_bound(pTrack->vPoint.begin(), pTrack->vPoint.end(), PointNow, PointFrameCmp);

    KGLOG_PROCESS_ERROR(it != pTrack->vPoint.end());
    // 如果在路点上
    if (it->nFrame == nFrame)
    {
        pRetPosition->nX = it->nX;
        pRetPosition->nY = it->nY;
        pRetPosition->nZ = it->nZ;

         //最后一个Point求方向时，没有下一个点了，所以就以前一个点为参考
        if (nFrame == pTrack->nFrame)
        {
            --it;
        }
        // 这里的赋值为了方便方向的计算
        PointFrom   = *it;
        PointTo     = *(it + 1);

        goto Exit1;
    }

    PointFrom   = *(it - 1);
    PointTo     = *it;

    KGLOG_PROCESS_ERROR(PointFrom.nFrame < nFrame);
    KGLOG_PROCESS_ERROR(PointTo.nFrame > nFrame);

    // 差值
    nDeltaFrame = PointTo.nFrame - PointFrom.nFrame;
    nFromFrame  = nFrame - PointFrom.nFrame;
    nToFrame    = PointTo.nFrame - nFrame;

    pRetPosition->nX = (PointFrom.nX * nToFrame + PointTo.nX * nFromFrame) / nDeltaFrame;
    pRetPosition->nY = (PointFrom.nY * nToFrame + PointTo.nY * nFromFrame) / nDeltaFrame;
    pRetPosition->nZ = (PointFrom.nZ * nToFrame + PointTo.nZ * nFromFrame) / nDeltaFrame;

Exit1:
    // 计算角色的朝向，只有在Point上的时候，才可能改变方向
    pRetPosition->nDirection = g_GetDirection(PointFrom.nX, PointFrom.nY, PointTo.nX, PointTo.nY);

    // 逆向时处理方向，给角色转180度
    if (nFromNode == pTrack->nToNode)
    {
        pRetPosition->nDirection = (pRetPosition->nDirection + DIRECTION_COUNT / 2) % DIRECTION_COUNT;
    }

    bResult = true;
Exit0:
    return bResult;
}

struct FindSmallestFreeNodeID 
{
    FindSmallestFreeNodeID() { nID = 1; }

    BOOL operator () (KROUTE_NODE* pNode)
    {
        BOOL bContinue = true;

        assert(pNode);

        if (pNode->nNodeID > nID)
            bContinue = false;

        if (pNode->nNodeID == nID)
            nID++;

        return bContinue;
    }

    int nID;
};

BOOL KRoadManager::AddNode(int* pnRetNodeID)
{
    BOOL                                    bResult = false;
    int                                     nNodeID = 0;
    KROUTE_NODE                             Node;
    FindSmallestFreeNodeID                  FreeNodeID;
    std::pair<KRouteMap::iterator, bool>    RetPair;

    assert(pnRetNodeID);
    *pnRetNodeID = 0;

    TraverseNode(FreeNodeID);
    nNodeID = FreeNodeID.nID;
    assert(nNodeID > 0);

    Node.nNodeID            = nNodeID;
    Node.dwCityID           = 0;
    Node.eCamp              = cNeutral;
    Node.eDefaultCamp       = cNeutral;
    Node.dwMapID            = 0;
    Node.dwTongID           = 0;
    Node.nX                 = 0;
    Node.nY                 = 0;
    Node.nZ                 = 0;
    Node.szDescription[0]   = '\0';
    Node.szScriptFile[0]    = '\0';
    Node.vLinkNode.clear();

    RetPair = m_RouteMap.insert(std::make_pair(nNodeID, Node));
    KGLOG_PROCESS_ERROR(RetPair.second);

    *pnRetNodeID = nNodeID;

    bResult = true;
Exit0:
    return bResult;
}

struct FindSmallestFreeTrackID 
{
    FindSmallestFreeTrackID() { nID = 1; }

    BOOL operator () (KTRACK* pTrack)
    {
        BOOL bContinue = true;

        assert(pTrack);

        if (pTrack->nTrackID > nID)
            bContinue = false;

        if (pTrack->nTrackID == nID)
            nID++;

        return bContinue;
    }

    int nID;
};

BOOL KRoadManager::AddTrack(int* pnRetTrackID, int nNodeFrom, int nNodeTo)
{
    BOOL                                    bResult     = false;
    int                                     nTrackID    = 0;
    KROUTE_NODE*                            pNodeFrom   = NULL;
    KROUTE_NODE*                            pNodeTo     = NULL;
    KTRACK*                                 pTrack      = NULL;
    KTRACK                                  NewTrack;
    FindSmallestFreeTrackID                 FreeTrackID;
    std::pair<KTrackMap::iterator, bool>    RetPair;

    assert(pnRetTrackID);
    *pnRetTrackID = 0;

    TraverseTrack(FreeTrackID);
    nTrackID = FreeTrackID.nID;
    assert(nTrackID > 0);

    pNodeFrom = GetNodeInfo(nNodeFrom);
    KGLOG_PROCESS_ERROR(pNodeFrom);

    pNodeTo = GetNodeInfo(nNodeTo);
    KGLOG_PROCESS_ERROR(pNodeTo);

    NewTrack.nTrackID   = nTrackID;
    NewTrack.nFromNode  = nNodeFrom;
    NewTrack.nToNode    = nNodeTo;
    NewTrack.nVelocity  = 0;
    NewTrack.nFrame     = 0;
    NewTrack.nCostMoney = 0;
    NewTrack.vPoint.clear();

    RetPair = m_TrackMap.insert(std::make_pair(nTrackID, NewTrack));
    KGLOG_PROCESS_ERROR(RetPair.second);

    pTrack = &((RetPair.first)->second);

    pNodeFrom->vLinkNode.push_back(std::make_pair(pNodeTo, pTrack));
    pNodeTo->vLinkNode.push_back(std::make_pair(pNodeFrom, pTrack));

    *pnRetTrackID = nTrackID;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::CanDelNode(int nNodeID)
{
    BOOL            bResult = false;
    KROUTE_NODE*    pNode   = NULL;

    pNode = GetNodeInfo(nNodeID);
    KG_PROCESS_ERROR(pNode);

    KG_PROCESS_ERROR(pNode->vLinkNode.empty());

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::CanDelTrack(int nTrackID)
{
    BOOL            bResult     = false;
    KTRACK*         pTrack      = NULL;
    KROUTE_NODE*    pNodeFrom   = NULL;
    KROUTE_NODE*    pNodeTo     = NULL;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator it;

    pTrack = GetTrackInfo(nTrackID);
    KG_PROCESS_ERROR(pTrack);

    pNodeFrom = GetNodeInfo(pTrack->nFromNode);
    assert(pNodeFrom);

    pNodeTo = GetNodeInfo(pTrack->nToNode);
    assert(pNodeTo);

    it = std::find(pNodeFrom->vLinkNode.begin(), pNodeFrom->vLinkNode.end(), std::make_pair(pNodeTo, pTrack));
    KG_PROCESS_ERROR(it != pNodeFrom->vLinkNode.end());

    it = std::find(pNodeTo->vLinkNode.begin(), pNodeTo->vLinkNode.end(), std::make_pair(pNodeFrom, pTrack));
    KG_PROCESS_ERROR(it != pNodeTo->vLinkNode.end());

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::DelNode(int nNodeID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KROUTE_NODE*    pNode       = NULL;

    pNode = GetNodeInfo(nNodeID);
    KG_PROCESS_ERROR(pNode);

    bRetCode = CanDelNode(nNodeID);
    KG_PROCESS_ERROR(bRetCode);

    m_RouteMap.erase(nNodeID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::DelNodeWithTrack(int nNodeID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KROUTE_NODE*    pNode       = NULL;
    std::deque<int> DelTrackIDQueue;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator it;

    pNode = GetNodeInfo(nNodeID);
    KG_PROCESS_ERROR(pNode);

    for (it = pNode->vLinkNode.begin(); it != pNode->vLinkNode.end(); ++it)
    {
        int nTrackID = it->second->nTrackID;

        bRetCode = CanDelTrack(nTrackID);
        KG_PROCESS_ERROR(bRetCode);

        DelTrackIDQueue.push_back(nTrackID);
    }

    while (!DelTrackIDQueue.empty())
    {
        int nTrackID = DelTrackIDQueue.front();

        DelTrackIDQueue.pop_front();

        bRetCode = DelTrack(nTrackID);
        KG_PROCESS_ERROR(bRetCode);
    }

    bRetCode = CanDelNode(nNodeID);
    KG_PROCESS_ERROR(bRetCode);

    m_RouteMap.erase(nNodeID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::DelTrack(int nTrackID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KTRACK*         pTrack      = NULL;
    KROUTE_NODE*    pNodeFrom   = NULL;
    KROUTE_NODE*    pNodeTo     = NULL;

    bRetCode = CanDelTrack(nTrackID);
    KG_PROCESS_ERROR(bRetCode);

    pTrack = GetTrackInfo(nTrackID);
    assert(pTrack);

    pNodeFrom = GetNodeInfo(pTrack->nFromNode);
    assert(pNodeFrom);

    pNodeTo = GetNodeInfo(pTrack->nToNode);
    assert(pNodeTo);

    pNodeFrom->vLinkNode.erase(
        std::remove(pNodeFrom->vLinkNode.begin(), pNodeFrom->vLinkNode.end(), std::make_pair(pNodeTo, pTrack)),
        pNodeFrom->vLinkNode.end()
    );

    pNodeTo->vLinkNode.erase(
        std::remove(pNodeTo->vLinkNode.begin(), pNodeTo->vLinkNode.end(), std::make_pair(pNodeFrom, pTrack)),
        pNodeTo->vLinkNode.end()
    );

    m_TrackMap.erase(nTrackID);

    bResult = true;
Exit0:
    return bResult;
}

struct _CLEAR_NODE_AND_TRACK
{
    _CLEAR_NODE_AND_TRACK(KRoadManager* pRoadManager)
    {
        assert(pRoadManager);

        m_pRoadManager = pRoadManager;
        m_DelNodeIDQueue.clear();
        m_DelTrackIDQueue.clear();
    }

    BOOL operator () (KROUTE_NODE* pNode)
    {
        int bResult     = false;
        int bRetCode    = false;
        std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator it;

        assert(pNode);

        for (it = pNode->vLinkNode.begin(); it != pNode->vLinkNode.end(); ++it)
        {
            int nTrackID = it->second->nTrackID;

            bRetCode = m_pRoadManager->CanDelTrack(nTrackID);
            KGLOG_PROCESS_ERROR(bRetCode);

            m_DelTrackIDQueue.push_back(nTrackID);
        }

        m_DelNodeIDQueue.push_back(pNode->nNodeID);

        bResult = true;
    Exit0:
        return bResult;
    }

    BOOL Clear()
    {
        BOOL    bResult     = false;
        BOOL    bRetCode    = false;
        int     nNodeID     = 0;
        int     nTrackID    = 0;

        while (!m_DelTrackIDQueue.empty())
        {
            nTrackID = m_DelTrackIDQueue.front();

            m_DelTrackIDQueue.pop_front();

            bRetCode = m_pRoadManager->DelTrack(nTrackID);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        while (!m_DelNodeIDQueue.empty())
        {
            nNodeID = m_DelNodeIDQueue.front();

            m_DelNodeIDQueue.pop_front();

            bRetCode = m_pRoadManager->DelNode(nNodeID);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        bResult = true;
    Exit0:
        return bResult;
    }

    KRoadManager*   m_pRoadManager;
    std::deque<int> m_DelNodeIDQueue;
    std::deque<int> m_DelTrackIDQueue;
};

BOOL KRoadManager::Clear(DWORD dwMapID)
{
    _CLEAR_NODE_AND_TRACK ClearNodeAndTrack(this);

    TraverseNode(dwMapID, ClearNodeAndTrack);

    ClearNodeAndTrack.Clear();

    return true;
}

BOOL KRoadManager::ExportTable()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    char szPath[PATH_MAX];

    snprintf(szPath, sizeof(szPath), "%s/%s", SETTING_DIR, ROAD_ROUTE_NODE_TAB);
    szPath[sizeof(szPath) - 1] = '\0';
    bRetCode = ExportNodeTable(szPath);
    KGLOG_PROCESS_ERROR(bRetCode && "Export RouteNode");

    snprintf(szPath, sizeof(szPath), "%s/%s", SETTING_DIR, ROAD_TRACK_TAB);
    szPath[sizeof(szPath) - 1] = '\0';
    bRetCode = ExportTrackTable(szPath);
    KGLOG_PROCESS_ERROR(bRetCode && "Export Track");

    snprintf(szPath, sizeof(szPath), "%s/%s", SETTING_DIR, ROAD_TAB);
    szPath[sizeof(szPath) - 1] = '\0';
    bRetCode = ExportPointTable(szPath);
    KGLOG_PROCESS_ERROR(bRetCode && "Export Point");

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::PointFrameCmp(const KTRACK_POINT& crPointFrom, const KTRACK_POINT& crPointTo)
{
    return (crPointFrom.nFrame < crPointTo.nFrame);
}

BOOL KRoadManager::IsOpenRouteNode(int nRouteNode, const KOPEN_ROUTE_NODE_VECTOER* cpNodeList)
{
    int                                         nResult     = false;
    int                                         nRetCode    = false;
    KOPEN_ROUTE_NODE_VECTOER::const_iterator    cit;
    KOPEN_ROUTE_NODE_VECTOER::const_iterator    citEnd;

    citEnd = cpNodeList->end();
    for (cit = cpNodeList->begin(); cit != citEnd; ++cit)
    {
        if ((*cit) == nRouteNode)
        {
            break;
        }
    }
    KG_PROCESS_ERROR(cit != citEnd);

    nResult = true;
Exit0:
    return nResult;
}

KROUTE_NODE* KRoadManager::GetTargetNodeByCity(DWORD dwCity, KCAMP eCamp)
{
    KROUTE_NODE* pResult    = NULL;
    KROUTE_NODE* pNoCampRes = NULL;
    KROUTE_NODE* pCampRes   = NULL;

    for (KRouteMap::iterator it = m_RouteMap.begin(); it != m_RouteMap.end(); ++it)
    {
        if (it->second.dwCityID == dwCity && it->second.eCamp == cNeutral)
        {
            pNoCampRes = &(it->second);
        }

        if (it->second.dwCityID == dwCity && it->second.eCamp != cNeutral && it->second.eCamp == eCamp)
        {
            pCampRes = &(it->second);
        }

        if (pCampRes)
            break;
    }

    pResult = pCampRes;
    if (!pResult)
        pResult = pNoCampRes;

    return pResult;
}

BOOL KRoadManager::ShortestPath(
    int *pnRetMiniCost,
    int nFromNodeID, int nTargetCity,
    int nExcludeNodeID, KCAMP eCamp,
    const KOPEN_ROUTE_NODE_VECTOER* cpOpenNodeList
)
{
    BOOL                                                        bResult         = false;
    BOOL                                                        bRetCode        = false;
    int                                                         nCurrentNodeID  = 0;
    int                                                         nMinCost        = INT_MAX;
    int                                                         nReputationLvl  = NEUTRAL_REPUTE_LEVEL;
    KROUTE_NODE*                                                pCurrentNode    = NULL;
    KROUTE_NODE*                                                pLinkNode       = NULL;
    KROUTE_NODE*                                                pEndNode        = NULL;
    KTRACK*                                                     pTrack          = NULL;
    KCLOSE_TABLE                                                CloseTab;
    KCLOSE_TABLE::iterator                                      itCloseTab;
    KOPEN_QUEUE                                                 OpenQue;
    pair<KCLOSE_TABLE::iterator, bool>                          itRetCode;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    it;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    itEnd;

    pCurrentNode = GetNodeInfo(nFromNodeID);
    KGLOG_PROCESS_ERROR(pCurrentNode);

    pEndNode = GetTargetNodeByCity(nTargetCity, eCamp);
    KGLOG_PROCESS_ERROR(pEndNode);

    *pnRetMiniCost = INT_MAX;

    if (cpOpenNodeList && pCurrentNode->bIsNeedOpen) // 起点都没有开
    {
        bRetCode = IsOpenRouteNode(nFromNodeID, cpOpenNodeList);
        KG_PROCESS_SUCCESS(!bRetCode);
    }

    KG_PROCESS_SUCCESS(!pCurrentNode->bEnable);

    KG_PROCESS_SUCCESS(pCurrentNode->eCamp != cNeutral && pCurrentNode->eCamp != eCamp); // 起点的阵营不符合

    KG_PROCESS_SUCCESS(nFromNodeID == nExcludeNodeID); // 起点就不可走
    
    *pnRetMiniCost = 0;

    // 如果该点就是目标城市点，则无需巡路，开销为0
    KG_PROCESS_SUCCESS(pCurrentNode->dwCityID == (DWORD)nTargetCity);

    OpenQue.push_back(nFromNodeID);
    itRetCode = CloseTab.insert(std::make_pair(nFromNodeID, 0));
    KGLOG_PROCESS_ERROR(itRetCode.second);

    while (true)
    {
        if (OpenQue.empty())
        {
            break;
        }

        nCurrentNodeID = OpenQue.front();
        OpenQue.pop_front();

        pCurrentNode = GetNodeInfo(nCurrentNodeID);
        KGLOG_PROCESS_ERROR(pCurrentNode);

        itEnd = pCurrentNode->vLinkNode.end();
        for (it = pCurrentNode->vLinkNode.begin(); it != itEnd; ++it)
        {
            int nCost = 0;
            pLinkNode   = it->first;
            pTrack      = it->second;

            if (pLinkNode->nNodeID == nExcludeNodeID)
            {
                continue;
            }

            if (cpOpenNodeList && pLinkNode->bIsNeedOpen)
            {
                bRetCode = IsOpenRouteNode(pLinkNode->nNodeID, cpOpenNodeList);
                if (!bRetCode)
                {
                    continue;
                }
            }

            if (!pLinkNode->bEnable)
            {
                continue;
            }

            if (pLinkNode->eCamp != cNeutral && pLinkNode->eCamp != eCamp)
            {
                continue;
            }

            itCloseTab = CloseTab.find(pCurrentNode->nNodeID);
            KGLOG_PROCESS_ERROR(itCloseTab != CloseTab.end());
            nCost = itCloseTab->second;

            nCost += pTrack->nCostMoney;

            itCloseTab = CloseTab.find(pLinkNode->nNodeID);
            if (itCloseTab == CloseTab.end())
            {
                itRetCode = CloseTab.insert(std::make_pair(pLinkNode->nNodeID, nCost));
                KGLOG_PROCESS_ERROR(itRetCode.second);

                itCloseTab = itRetCode.first;
                OpenQue.push_back(pLinkNode->nNodeID);
            }

            if (itCloseTab->second > nCost)
            {
                itCloseTab->second = nCost;
                OpenQue.push_back(pLinkNode->nNodeID);
            }

            if (pLinkNode != pEndNode)
            {
                continue;
            }
            
            if (nMinCost > nCost)
            {
                nMinCost = nCost;
            }
        }
    }

    *pnRetMiniCost = nMinCost;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoadManager::GetNextTrack(
    int* pnRetTrack, int nFromNode,
    int nCurrentNode, int nTargetCity,
    KCAMP eCamp, DWORD dwTongID,
    const KOPEN_ROUTE_NODE_VECTOER* cpOpenList
)
{
    BOOL                                                        bResult         = false;
    BOOL                                                        bRetCode        = false;
    int                                                         nCurrentCost    = 0;
    int                                                         nMinCost        = INT_MAX;
    KROUTE_NODE*                                                pCurrentNode    = NULL;
    KTRACK*                                                     pCurrentTrack   = NULL;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    it;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    itEnd;

    assert(pnRetTrack);

    pCurrentNode = GetNodeInfo(nCurrentNode);
    KGLOG_PROCESS_ERROR(pCurrentNode);

    itEnd = pCurrentNode->vLinkNode.end();
    for (it = pCurrentNode->vLinkNode.begin(); it != itEnd; ++it)
    {
        if (it->first->nNodeID == nFromNode)
        {
            continue;
        }

        pCurrentTrack   = it->second;
        nCurrentCost    = INT_MAX;

        bRetCode = ShortestPath(&nCurrentCost, it->first->nNodeID, nTargetCity, nCurrentNode, eCamp, cpOpenList);
        KGLOG_PROCESS_ERROR(bRetCode);

        if (nCurrentCost == INT_MAX)
        {
            continue;
        }

        if ((nCurrentCost + pCurrentTrack->nCostMoney) < nMinCost)
        {
            nMinCost = (nCurrentCost + pCurrentTrack->nCostMoney);
            *pnRetTrack = it->second->nTrackID;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

ROAD_TRACK_RESULT_CODE KRoadManager::IsReachable(
    int* pnRetCostMoney,
    int nFromNode, int nTargetCity,
    KCAMP eCamp, DWORD dwTongID,
    const KOPEN_ROUTE_NODE_VECTOER* cpOpenList
)
{
    ROAD_TRACK_RESULT_CODE  nResult     = rtrcFailed;
    BOOL                    bRetCode    = false;
    int                     nMinCost    = INT_MAX;
    
    assert(pnRetCostMoney);
    
    bRetCode = ShortestPath(&nMinCost, nFromNode, nTargetCity, 0, eCamp, cpOpenList);
    KGLOG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR_RET_CODE(nMinCost != INT_MAX, rtrcNotOpen);

    *pnRetCostMoney = nMinCost;

    nResult = rtrcSuccess;
Exit0:
    return nResult;
}

BOOL KRoadManager::CalcCostMoney(
    int* pnRetCost,
    int nFromNode, int nTargetCity,
    KPlayer* pTraveller
)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nCostMoney      = 0;
    int             nLuaTopIndex    = 0;
    KROUTE_NODE*    pNode           = NULL;

    bRetCode = IsReachable(
        &nCostMoney, nFromNode, nTargetCity, pTraveller->m_eCamp, pTraveller->m_dwTongID, 
        &(pTraveller->m_OpenRouteNodeList)
    );
    KG_PROCESS_ERROR(bRetCode == rtrcSuccess);

    pNode = g_pSO3World->m_RoadManager.GetNodeInfo(nFromNode);
    KGLOG_PROCESS_ERROR(pNode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pNode->szScriptFile);
    if (!bRetCode)
    {
        goto Exit1;
    }

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pNode->szScriptFile, "CalcCost");
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack((DWORD)(pTraveller->m_dwID));
	g_pSO3World->m_ScriptCenter.PushValueToStack((KROUTE_NODE*)pNode);
    g_pSO3World->m_ScriptCenter.PushValueToStack((int)nTargetCity);
    g_pSO3World->m_ScriptCenter.PushValueToStack((int)nCostMoney);

	bRetCode = g_pSO3World->m_ScriptCenter.CallFunction(pNode->szScriptFile, "CalcCost", 1);

    g_pSO3World->m_ScriptCenter.GetValuesFromStack("d", &nCostMoney);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    *pnRetCost = nCostMoney;

    bResult = true;
Exit0:
    return bResult;
}

DEFINE_LUA_CLASS_BEGIN(KROUTE_NODE)
    REGISTER_LUA_DWORD(  KROUTE_NODE,   MapID)
    REGISTER_LUA_DWORD(  KROUTE_NODE,   CityID)
    REGISTER_LUA_ENUM(  KROUTE_NODE,   Camp)
    REGISTER_LUA_DWORD(  KROUTE_NODE,   TongID)
    REGISTER_LUA_BOOL(   KROUTE_NODE,   IsNeedOpen)
    REGISTER_LUA_INTEGER(KROUTE_NODE,   X)
    REGISTER_LUA_INTEGER(KROUTE_NODE,   Y)
    REGISTER_LUA_INTEGER(KROUTE_NODE,   Z)
DEFINE_LUA_CLASS_END(KROUTE_NODE)
