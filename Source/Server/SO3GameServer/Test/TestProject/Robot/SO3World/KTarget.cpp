#include "stdafx.h"
#include "KTarget.h"
#include "KCharacter.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KDoodad.h"
#include "KObjectManager.h"

KTarget::KTarget()
{
    m_eTargetType   = ttInvalid;
    m_dwID          = ERROR_ID;
    memset(&m_Coordination, 0, sizeof(m_Coordination));
    m_pPlayer       = NULL;
    m_pNpc          = NULL;
    m_pDoodad       = NULL;
}

KTarget::~KTarget()
{
    m_eTargetType   = ttInvalid;
    m_dwID          = ERROR_ID;
}

TARGET_TYPE KTarget::GetTargetType()
{
    return m_eTargetType;
}

BOOL KTarget::SetTarget(KCharacter* pCharacter)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(pCharacter);

    if (g_IsPlayer(pCharacter->m_dwID))
    {
        m_eTargetType = ttPlayer;
        m_dwID        = pCharacter->m_dwID;
        m_pPlayer     = (KPlayer*)pCharacter;
    }
    else
    {
        m_eTargetType = ttNpc;
        m_dwID        = pCharacter->m_dwID;
        m_pNpc        = (KNpc*)pCharacter;
    }
    
Exit1:
    bResult = true;
Exit0:
    return bResult;
}


BOOL KTarget::SetTarget(KDoodad* pDoodad)
{
    BOOL bResult = false;
    
    KG_PROCESS_ERROR(pDoodad);

    m_eTargetType = ttDoodad;
    m_dwID        = pDoodad->m_dwID;
    m_pDoodad     = pDoodad;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::SetTarget(KItem* pItem)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(pItem);

    m_eTargetType = ttItem;
    m_dwID        = pItem->m_dwID;
    m_pItem       = pItem;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::SetTarget(int nX, int nY, int nZ)
{
    m_eTargetType   = ttCoordination;
    m_Coordination.nX = nX;
    m_Coordination.nY = nY;
    m_Coordination.nZ = nZ;

    return true;
}

BOOL KTarget::SetTarget(TARGET_TYPE eType, DWORD dwID)
{
    BOOL bResult = false;

    //switch (eType)
    //{
    //case ttNoTarget:
    //    m_eTargetType = ttNoTarget;
    //    break;
    //
    //case ttNpc:
    //    KGLOG_PROCESS_ERROR(!g_IsPlayer(dwID));
    //    m_pNpc = g_ObjectManager.m_NpcSet.GetObj(dwID);
    //    KG_PROCESS_ERROR(m_pNpc);
    //    
    //    m_eTargetType = eType;
    //    m_dwID        = dwID;
    //    break;

    //case ttPlayer:
    //    KGLOG_PROCESS_ERROR(g_IsPlayer(dwID));
    //    m_pPlayer = g_ObjectManager.m_PlayerSet.GetObj(dwID);
    //    KG_PROCESS_ERROR(m_pPlayer);
    //    
    //    m_eTargetType = eType;
    //    m_dwID        = dwID;
    //    break;

    //case ttDoodad:
    //    m_pDoodad = g_ObjectManager.m_DoodadSet.GetObj(dwID);
    //    KG_PROCESS_ERROR(m_pDoodad);
    //    m_eTargetType = eType;
    //    m_dwID        = dwID;
    //    break;
    //
    //case ttItem:
    //    m_pItem = g_ObjectManager.m_ItemSet.GetObj(dwID);
    //    KG_PROCESS_ERROR(m_pItem);
    //    m_eTargetType   = eType;
    //    m_dwID          = dwID;
    //    break;

    //default:
    //    KGLOG_PROCESS_ERROR(false && "Unexecpt Target Type!");
    //}

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_eTargetType = ttNoTarget;
    }
    return bResult;
}

BOOL KTarget::GetTarget(KCharacter** ppCharacter)
{
    BOOL bResult = false;

    switch (m_eTargetType)
    {
    case ttNpc:
        KG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwID);     
        *ppCharacter = (KCharacter*)m_pNpc;
        break;
    case ttPlayer:
        KG_PROCESS_ERROR(m_pPlayer->m_dwID == m_dwID);
        *ppCharacter = (KCharacter*)m_pPlayer;
        break;
    default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KNpc** ppNpc)    
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(m_eTargetType == ttNpc);
    KG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwID);

    *ppNpc = m_pNpc;

    bResult = true;
Exit0:
    return bResult;
}
BOOL KTarget::GetTarget(KPlayer** ppPlayer)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(m_eTargetType == ttPlayer);
    KG_PROCESS_ERROR(m_pPlayer->m_dwID == m_dwID);

    *ppPlayer = m_pPlayer;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KDoodad** ppDoodad)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(m_eTargetType == ttDoodad);
    KG_PROCESS_ERROR(m_pDoodad->m_dwID == m_dwID);

    *ppDoodad = m_pDoodad;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KItem**  ppItem)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(m_eTargetType == ttItem);
    KG_PROCESS_ERROR(m_pItem->m_dwID == m_dwID);

    *ppItem = m_pItem;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KSceneObject** ppSceneObject)
{
    BOOL bResult = false;

    switch (m_eTargetType)
    {
    case ttNpc:
        KG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwID);
        *ppSceneObject = (KSceneObject*)m_pNpc;
        break;
    case ttPlayer:
        KG_PROCESS_ERROR(m_pPlayer->m_dwID == m_dwID);
        *ppSceneObject = (KSceneObject*)m_pPlayer;
        break;
    case ttDoodad:
        KG_PROCESS_ERROR(m_pDoodad->m_dwID == m_dwID);
        *ppSceneObject = (KSceneObject*)m_pDoodad;
        break;
    default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(int* pnX, int* pnY, int* pnZ)
{
    BOOL bResult = false;

    switch (m_eTargetType)
    {
    case ttNpc:
        KG_PROCESS_ERROR(m_pNpc->m_dwID == m_dwID);
        KG_PROCESS_ERROR(m_pNpc->m_pCell);
        *pnX = m_pNpc->m_nX;
        *pnY = m_pNpc->m_nY;
        *pnZ = m_pNpc->m_nZ + m_pNpc->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
        break;
    
    case ttPlayer:
        KG_PROCESS_ERROR(m_pPlayer->m_dwID == m_dwID);
        KG_PROCESS_ERROR(m_pPlayer->m_pCell);
        *pnX = m_pPlayer->m_nX;
        *pnY = m_pPlayer->m_nY;
        *pnZ = m_pPlayer->m_nZ + m_pPlayer->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
        break;
    case ttDoodad:
        KG_PROCESS_ERROR(m_pDoodad->m_dwID == m_dwID);
        KG_PROCESS_ERROR(m_pDoodad->m_pCell);
        *pnX = m_pDoodad->m_nX;
        *pnY = m_pDoodad->m_nY;
        *pnZ = m_pDoodad->m_nZ + m_pDoodad->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
        break;
    case ttCoordination:
        *pnX = m_Coordination.nX;
        *pnY = m_Coordination.nY;
        *pnZ = m_Coordination.nZ;
        break;
    default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::ClearTarget()
{
    m_eTargetType = ttNoTarget;
    
    return true;
}