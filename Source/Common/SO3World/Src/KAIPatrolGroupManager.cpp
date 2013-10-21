#include "stdafx.h"

#include <algorithm>
#include "KPatrolPath.h"
#include "KAIPatrolGroupManager.h"
#include "KPatrolPathList.h"
#include "KNpcOrderList.h"
#include "KSO3World.h"
#include "KScene.h"

KPatrolGroup::KPatrolGroup()
{
    MemberIDs.clear();
    nDestX = 0;
    nDestY = 0;
    nCurrentX = 0;
    nCurrentY = 0;
    nCurrentZ = 0;
    nDirection = -1;
    nPatrolPathIndex = 0;
    nRestUntilFrame = 0;
}

BOOL KAIPatrolGroupManager::Init(KScene* pScene)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(pScene);

    m_PatrolGroupMap.clear();
    m_pScene = pScene;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAIPatrolGroupManager::UnInit()
{
    m_PatrolGroupMap.clear();
    return true;
}

BOOL KAIPatrolGroupManager::JoinGroup(DWORD dwPatrolPathID, DWORD dwCharacterID)
{
    BOOL bResult = false;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];
    std::vector<DWORD>::iterator it;

    it = std::find(rGroup.MemberIDs.begin(), rGroup.MemberIDs.end(), dwCharacterID);
    KG_PROCESS_SUCCESS(it != rGroup.MemberIDs.end());

    rGroup.MemberIDs.push_back(dwCharacterID);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KAIPatrolGroupManager::LeaveGroup(DWORD dwPatrolPathID, DWORD dwCharacterID)
{
    BOOL bResult = false;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];
    std::vector<DWORD>::iterator it;

    it = std::find(rGroup.MemberIDs.begin(), rGroup.MemberIDs.end(), dwCharacterID);
    KG_PROCESS_SUCCESS(it == rGroup.MemberIDs.end());

    rGroup.MemberIDs.erase(it);

    if (rGroup.MemberIDs.empty())
        m_PatrolGroupMap.erase(m_PatrolGroupMap.find(dwPatrolPathID));

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

DWORD KAIPatrolGroupManager::GetLeader(DWORD dwPatrolPathID)
{
    DWORD dwResult = ERROR_ID;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];

    KG_PROCESS_ERROR(!rGroup.MemberIDs.empty());

    dwResult = m_PatrolGroupMap[dwPatrolPathID].MemberIDs.front();
Exit0:
    return dwResult;
}

BOOL KAIPatrolGroupManager::SetLeader(DWORD dwPatrolPathID, DWORD dwCharacterID)
{
    BOOL bResult = false;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];
    std::vector<DWORD>::iterator it;

    it = std::find(rGroup.MemberIDs.begin(), rGroup.MemberIDs.end(), dwCharacterID);
    if (it != rGroup.MemberIDs.end())
    {
        rGroup.MemberIDs.erase(it);
    }

    rGroup.MemberIDs.insert(rGroup.MemberIDs.begin(), dwCharacterID);

    bResult = true;
Exit0:
    return bResult;
}

void GetRelativePosition(int nSrcX, int nSrcY, int nDistance, int nDir, int* pnDstX, int* pnDstY)
{
    assert(pnDstX);
    assert(pnDstY);

    //保证角度在[0, DIRECTION_COUNT)
    while (nDir < 0)
    {
        nDir += DIRECTION_COUNT;
    }

    if (nDir >= DIRECTION_COUNT)
    {
        nDir = nDir % DIRECTION_COUNT;
    }

    *pnDstX = nSrcX + nDistance * g_Cos(nDir) / SIN_COS_NUMBER;
    *pnDstY = nSrcY + nDistance * g_Sin(nDir) / SIN_COS_NUMBER;
}

BOOL KAIPatrolGroupManager::UpdateGroupPosition(
    DWORD dwPatrolPathID, int nOrderIndex, int nDestX, int nDestY, 
    int nCurrentX, int nCurrentY, int nCurrentZ,int nDirection
)
{
    BOOL                bResult     = false;
    KPatrolPath*        pPatrolPath = NULL;
    const KORDER*       cpOrder     = NULL;
    const KORDER_NODE*  cpOrderNode = NULL;
    int                 nGroupX     = nDestX;
    int                 nGroupY     = nDestY;
    KPatrolGroup&       rGroup      = m_PatrolGroupMap[dwPatrolPathID];

    KGLOG_PROCESS_ERROR(dwPatrolPathID);

    pPatrolPath = g_pSO3World->m_Settings.m_PatrolPathList.GetPatrolPath(m_pScene->m_dwMapID, dwPatrolPathID);
    KGLOG_PROCESS_ERROR(pPatrolPath);

    KG_PROCESS_SUCCESS(!pPatrolPath->m_nOrderID);
    KGLOG_PROCESS_ERROR(nOrderIndex >= 0);

    cpOrder = g_pSO3World->m_Settings.m_OrderManager.GetOrder(pPatrolPath->m_nOrderID);
    KGLOG_PROCESS_ERROR(cpOrder);

    KGLOG_PROCESS_ERROR(nOrderIndex < (int)cpOrder->size());
    cpOrderNode = &(*(cpOrder->begin() + nOrderIndex));

    GetRelativePosition(nGroupX, nGroupY, -cpOrderNode->nRadius, nDirection + cpOrderNode->nAngel, &nGroupX, &nGroupY);

Exit1:
    rGroup.nDestX       = nGroupX;
    rGroup.nDestY       = nGroupY;
    rGroup.nCurrentX    = nCurrentX;
    rGroup.nCurrentY    = nCurrentY;
    rGroup.nCurrentZ    = nCurrentZ;
    rGroup.nDirection   = nDirection;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, 
            "KAIPatrolGroupManager::UpdateGroupPosition(MapID %d, PathID %d, OrderIndex %d) !\n",
            m_pScene->m_dwMapID,
            dwPatrolPathID,
            nOrderIndex
        );
    }
    return bResult;
}

BOOL KAIPatrolGroupManager::GetMemberPosition(DWORD dwPatrolPathID, int nOrderIndex, int* pnX, int* pnY, int* pnDirection)
{
    BOOL                bResult     = false;
    KPatrolPath*        pPatrolPath = NULL;
    const KORDER*       cpOrder     = NULL;
    const KORDER_NODE*  cpOrderNode = NULL;
    KPatrolGroup&       rGroup      = m_PatrolGroupMap[dwPatrolPathID];
    int                 nGroupX     = rGroup.nDestX;
    int                 nGroupY     = rGroup.nDestY;

    KGLOG_PROCESS_ERROR(dwPatrolPathID);
    KGLOG_PROCESS_ERROR(nOrderIndex >= 0);
    assert(pnX);
    assert(pnY);
    assert(pnDirection);

    KG_PROCESS_ERROR(rGroup.nDirection != -1);

    pPatrolPath = g_pSO3World->m_Settings.m_PatrolPathList.GetPatrolPath(m_pScene->m_dwMapID, dwPatrolPathID);
    KGLOG_PROCESS_ERROR(pPatrolPath);

    KGLOG_PROCESS_SUCCESS(!pPatrolPath->m_nOrderID);

    cpOrder = g_pSO3World->m_Settings.m_OrderManager.GetOrder(pPatrolPath->m_nOrderID);
    KGLOG_PROCESS_ERROR(cpOrder);

    KGLOG_PROCESS_ERROR(nOrderIndex < (int)cpOrder->size());
    cpOrderNode = &(*(cpOrder->begin() + nOrderIndex));

    GetRelativePosition(nGroupX, nGroupY, cpOrderNode->nRadius, rGroup.nDirection + cpOrderNode->nAngel, &nGroupX, &nGroupY);

Exit1:
    *pnX = nGroupX;
    *pnY = nGroupY;
    *pnDirection = rGroup.nDirection;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, 
            "KAIPatrolGroupManager::GetMemberPosition(MapID %d, PathID %d, OrderIndex %d) !\n",
            m_pScene->m_dwMapID,
            dwPatrolPathID,
            nOrderIndex
        );
    }
    return bResult;
}

BOOL KAIPatrolGroupManager::SetGroupPatrolPathIndex(DWORD dwPatrolPathID, int nPatrolPathIndex)
{
    BOOL bResult = false;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];

    rGroup.nPatrolPathIndex = nPatrolPathIndex;

    bResult = true;
Exit0:
    return bResult;
}

int KAIPatrolGroupManager::GetGroupPatrolPathIndex(DWORD dwPatrolPathID)
{
    int nResult = -1;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];

    nResult = rGroup.nPatrolPathIndex;
Exit0:
    return nResult;
}

int KAIPatrolGroupManager::GetNpcIndex(DWORD dwPatrolPathID, DWORD dwCharacterID)
{
    int nResult = -1;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];
    std::vector<DWORD>::iterator it;

    it = std::find(rGroup.MemberIDs.begin(), rGroup.MemberIDs.end(), dwCharacterID);
    KG_PROCESS_ERROR(it != rGroup.MemberIDs.end());
    
    nResult = (int)std::distance(rGroup.MemberIDs.begin(), it);
Exit0:
    return nResult;
}

BOOL KAIPatrolGroupManager::SetRestFrame(DWORD dwPatrolPathID, int nRestFrame)
{
    BOOL bResult = false;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];

    rGroup.nRestUntilFrame = g_pSO3World->m_nGameLoop + nRestFrame;

    bResult = true;
Exit0:
    return bResult;
}

int KAIPatrolGroupManager::GetRestFrame(DWORD dwPatrolPathID)
{
    int nResult = 0;
    KPatrolGroup& rGroup = m_PatrolGroupMap[dwPatrolPathID];

    nResult = max(0, rGroup.nRestUntilFrame - g_pSO3World->m_nGameLoop);
Exit0:
    return nResult;
}

KPatrolGroup* KAIPatrolGroupManager::GetPatrolGroup(DWORD dwPatrolPathID)
{    
    std::map<DWORD, KPatrolGroup>::iterator it;

    it = m_PatrolGroupMap.find(dwPatrolPathID);
    if (it != m_PatrolGroupMap.end())
        return &(it->second);

    return NULL;
}
