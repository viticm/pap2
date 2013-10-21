#include "StdAfx.h"
#include <math.h>
#include "KGPublic.h"
#include "engine\kglog.h"
#include "Engine\FileType.h"
#include "EnvironmentConstruct.h"

BOOL EnvironmentConstruct::Init()
{
    int           nResult        = false;
    int           nRetCode       = false;
    ITabFile*     piTableFile    = NULL;
    KTRACK*       pThisTrack     = NULL;
    KROUTE_NODE*  pFromRouteNode = NULL;
    KROUTE_NODE*  pToRouteNode   = NULL;
    KTRACK*       pTrack         = NULL;
    int           nTrackID       = 0;
    int           nTabHeight     = 0;
    int           nDistance      = 0;
    int           nCurrentFrame  = 0;
    char          szScriptFile[MAX_PATH];
    KROUTE_NODE   RouteNode;
    KTRACK        Track;
    KTRACK_POINT  TrackPoint;
    KTRACK_POINT  PrevTrackPoint;
    std::pair<KTrackMap::iterator, bool> itRetCode;

    // Load RoadNode.tab
    //--------------------------------------------------------------------------
    piTableFile = g_OpenTabFile("RoadNode.tab");
    KGLOG_PROCESS_ERROR(piTableFile);

    nTabHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 1);

    for (int i = 2; i <= nTabHeight; i++)
    {
        nRetCode = piTableFile->GetInteger(i, "NodeID", -1, &RouteNode.nNodeID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get NodeID");

        nRetCode = piTableFile->GetString(i, "Description", "", RouteNode.szDescription, sizeof(RouteNode.szDescription));
        KGLOG_PROCESS_ERROR(nRetCode && "Get Description");

        nRetCode = piTableFile->GetInteger(i, "MapID", 0, (int*)&RouteNode.dwMapID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get MapID");

        nRetCode = piTableFile->GetInteger(i, "CityID", 0, (int*)&RouteNode.dwCityID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get CityID");

        nRetCode = piTableFile->GetInteger(i, "ForceID", 0, (int*)&RouteNode.dwForceID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get ForceID");

        RouteNode.dwDefaultForceID = RouteNode.dwForceID;

        nRetCode = piTableFile->GetInteger(i, "TongID", 0, (int*)&RouteNode.dwTongID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get TongID");

        nRetCode = piTableFile->GetString(i, "ScriptFile", "", szScriptFile, sizeof(szScriptFile));
        KGLOG_PROCESS_ERROR(nRetCode);

        RouteNode.szScriptFile[0] = '\0';
        if (szScriptFile[0] != '\0')
        {
            snprintf(RouteNode.szScriptFile, sizeof(RouteNode.szScriptFile), "%s", szScriptFile);
            RouteNode.szScriptFile[sizeof(RouteNode.szScriptFile) - 1] = '\0';
        }

        nRetCode = piTableFile->GetInteger(i, "NeedOpen", false, &RouteNode.bIsNeedOpen);
        KGLOG_PROCESS_ERROR(nRetCode && "Get NeedOpen");

        nRetCode = piTableFile->GetInteger(i, "X", 0, &RouteNode.nX);
        KGLOG_PROCESS_ERROR(nRetCode && "Get X");

        nRetCode = piTableFile->GetInteger(i, "Y", 0, &RouteNode.nY);
        KGLOG_PROCESS_ERROR(nRetCode && "Get Y");

        nRetCode = piTableFile->GetInteger(i, "Z", 0, &RouteNode.nZ);
        KGLOG_PROCESS_ERROR(nRetCode && "Get Z");

        KG_PROCESS_ERROR(RouteNode.bIsNeedOpen || RouteNode.dwForceID == 0);

        m_RouteMap.insert(std::make_pair(RouteNode.nNodeID, RouteNode));
    }
    KG_COM_RELEASE(piTableFile);
    piTableFile = NULL;

    //Load RoadTrack.tab
    //--------------------------------------------------------------------------
    piTableFile = g_OpenTabFile("RoadTrack.tab");
    KGLOG_PROCESS_ERROR(piTableFile);

    nTabHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 1);

    for (int i = 2; i <= nTabHeight; i++)
    {
        nRetCode = piTableFile->GetInteger(i, "FromNodeID", 0, &Track.nFromNode);
        KGLOG_PROCESS_ERROR(nRetCode && "Get FromNodeID");

        nRetCode = piTableFile->GetInteger(i, "ToNodeID", 0, &Track.nToNode);
        KGLOG_PROCESS_ERROR(nRetCode && "Get ToNodeID");

        nRetCode = piTableFile->GetInteger(i, "CostMoney", 0, &Track.nCostMoney);
        KGLOG_PROCESS_ERROR(nRetCode && "Get Cost Money");

        nRetCode = piTableFile->GetInteger(i, "TrackID", 0, &Track.nTrackID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get TrackID");

        KGLOG_PROCESS_ERROR(Track.nTrackID > 0 && Track.nTrackID <= MAX_TRACK_ID);

        nRetCode = piTableFile->GetInteger(i, "Velocity", 0, &Track.nVelocity);
        KGLOG_PROCESS_ERROR(nRetCode && "Get Velocity");

        KGLOG_PROCESS_ERROR(Track.nVelocity > 0);

        Track.nFrame = 0;   //  这里置0，将在读入路点的时候，累计每个路点的时间。

        itRetCode = m_TrackMap.insert(std::make_pair(Track.nTrackID, Track));
        KGLOG_PROCESS_ERROR(itRetCode.second);

        pThisTrack      = GetTrackInfo(Track.nTrackID);
        pFromRouteNode  = GetNodeInfo(Track.nFromNode);
        pToRouteNode    = GetNodeInfo(Track.nToNode);

        pFromRouteNode->vLinkNode.push_back(std::make_pair(pToRouteNode, pThisTrack));
        pToRouteNode->vLinkNode.push_back(std::make_pair(pFromRouteNode, pThisTrack));
    }
    KG_COM_RELEASE(piTableFile);
    piTableFile = NULL;

    //Load RoadPoint.tab
    //--------------------------------------------------------------------------
    piTableFile = g_OpenTabFile("RoadPoint.tab");
    KGLOG_PROCESS_ERROR(piTableFile);

    nTabHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 1);

    for (int i = 2; i <= nTabHeight; i++)
    {
        nRetCode = piTableFile->GetInteger(i, "TrackID", 0, &nTrackID);
        KG_PROCESS_ERROR(nRetCode && "Get TrackID");

        nRetCode = piTableFile->GetInteger(i, "X", 0, &TrackPoint.nX);
        KG_PROCESS_ERROR(nRetCode && "Get X");

        nRetCode = piTableFile->GetInteger(i, "Y", 0, &TrackPoint.nY);
        KG_PROCESS_ERROR(nRetCode && "Get Y");

        nRetCode = piTableFile->GetInteger(i, "Z", 0, &TrackPoint.nZ);
        KG_PROCESS_ERROR(nRetCode && "Get Z");

        pTrack = GetTrackInfo(nTrackID);
        KG_PROCESS_ERROR(pTrack);

        if (pTrack->vPoint.empty())
        {
            pTrack->nFrame = 0;
            TrackPoint.nFrame = 0;
            pTrack->vPoint.push_back(TrackPoint);
            continue;
        }

        PrevTrackPoint = pTrack->vPoint.back();

        nDistance = g_GetDistance3(
            PrevTrackPoint.nX, PrevTrackPoint.nY, PrevTrackPoint.nZ,
            TrackPoint.nX, TrackPoint.nY, TrackPoint.nZ
            );
        nCurrentFrame = (int)sqrt((double)nDistance) / pTrack->nVelocity;

        pTrack->nFrame += nCurrentFrame;
        TrackPoint.nFrame = pTrack->nFrame;

        pTrack->vPoint.push_back(TrackPoint);
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        m_RouteMap.clear();
        m_TrackMap.clear();
        pTrack->vPoint.clear();
    }
    KG_COM_RELEASE(piTableFile);
    return nResult;
}

BOOL EnvironmentConstruct::UnInit()
{
    int nResult  = false;
    int nRetCode = false;

    m_RouteMap.clear();
    m_TrackMap.clear();

    nResult = true;
//Exit0:
    return nResult;
}

KRouteMap EnvironmentConstruct::GetRoutMap()
{
    return m_RouteMap;
}

KTrackMap EnvironmentConstruct::GetTrackMap()
{
    return m_TrackMap;
}

KROUTE_NODE* EnvironmentConstruct::GetNodeInfo(int nNode)
{
    KROUTE_NODE*        pResult = NULL;
    KRouteMap::iterator itFind;

    itFind = m_RouteMap.find(nNode);
    KG_PROCESS_ERROR(itFind != m_RouteMap.end());

    pResult = &(itFind->second);
Exit0:
    return pResult;
}

KTRACK* EnvironmentConstruct::GetTrackInfo(int nTrack)
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

BOOL EnvironmentConstruct::IsOpenRouteNode(int nRouteNode, const KOPEN_ROUTE_NODE_VECTOER* cpNodeList)
{
    int                                         nResult   = false;
    int                                         nRetCode  = false;
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