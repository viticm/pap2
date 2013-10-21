#include "stdafx.h"
#include "KRoadCenter.h"
#include "KSO3GameCenter.h"

KRoadCenter::KRoadCenter()
{
}

BOOL KRoadCenter::Init()
{
    m_RoadInfo.clear();
    return true;
}

void KRoadCenter::UnInit()
{
    m_RoadInfo.clear();
}

BOOL KRoadCenter::SyncNewServer(int nConnIndex)
{
    for (KROAD_INFO::iterator it = m_RoadInfo.begin(); it != m_RoadInfo.end(); ++it)
    {
        g_pSO3GameCenter->m_GameServer.DoSyncRoadTrackInfo(nConnIndex, it->first, it->second.eCamp, it->second.bEnable);
    }

    return true;
}

BOOL KRoadCenter::UpdateRoadInfo(int nRoadNodeID, KCAMP eCamp, BOOL bEnable)
{
    BOOL                 bResult = false;
    KROAD_INFO::iterator itFind;
    KROAD_NODE           Node;

    Node.eCamp      = eCamp;
    Node.bEnable    = bEnable;

    m_RoadInfo[nRoadNodeID] = Node;

    return true;
}
