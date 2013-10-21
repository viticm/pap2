#include "stdafx.h"
#include "KTarget.h"
#include "KCharacter.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KDoodad.h"
#include "KSO3World.h"

KTarget::KTarget()
{
    m_eTargetType   = ttInvalid;
    m_dwID          = ERROR_ID;
    memset(&m_Coordination, 0, sizeof(m_Coordination));
}

KTarget::~KTarget()
{
    m_eTargetType   = ttInvalid;
    m_dwID          = ERROR_ID;
}

void KTarget::SetTargetData(KTargetData const& TargetData)
{
	assert(sizeof(KTarget) == sizeof(KTargetData));
	memcpy(this, &TargetData, sizeof(KTargetData));
}

void KTarget::GetTargetData(KTargetData& TargetData)
{
	assert(sizeof(KTarget) == sizeof(KTargetData));
	memcpy(&TargetData, this, sizeof(KTargetData));
}

TARGET_TYPE KTarget::GetTargetType()
{
    return m_eTargetType;
}

DWORD KTarget::GetTargetID()
{
	return m_dwID;
}

BOOL KTarget::SetTarget(KCharacter* pCharacter)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(pCharacter);
    
    m_dwID  = pCharacter->m_dwID;
    m_eTargetType = ttNpc;

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        m_eTargetType = ttPlayer;
    }
    
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

BOOL KTarget::SetTarget(KPlayer* pPlayer, KItem* pItem)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	DWORD       dwBoxIndex  = 0;
	DWORD       dwX         = 0;

	KGLOG_PROCESS_ERROR(pPlayer);
	KGLOG_PROCESS_ERROR(pItem);

	m_eTargetType = ttItemPos;
	m_dwID = pPlayer->m_dwID;

	bRetCode = pPlayer->m_ItemList.GetItemPos(pItem->m_dwID, dwBoxIndex, dwX);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_ItemPos.dwBoxIndex = dwBoxIndex;
	m_ItemPos.dwX = dwX;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KTarget::SetTarget(KPlayer* pPlayer, DWORD dwBoxIndex, DWORD dwX)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KItem*      pItem       = NULL;

	KGLOG_PROCESS_ERROR(pPlayer);

	pItem = pPlayer->m_ItemList.GetItem(dwBoxIndex, dwX);
	KGLOG_PROCESS_ERROR(pItem);

	m_eTargetType = ttItemPos;
	m_dwID = pPlayer->m_dwID;
	m_ItemPos.dwBoxIndex = dwBoxIndex;
	m_ItemPos.dwX = dwX;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KTarget::SetTarget(TARGET_TYPE eType, DWORD dwID)
{
    BOOL bResult = false;

    switch (eType)
    {
    case ttNoTarget:
        break;
    
    case ttNpc:
        {
            KNpc* pNpc = NULL;
            KGLOG_PROCESS_ERROR(IS_NPC(dwID));

            pNpc = g_pSO3World->m_NpcSet.GetObj(dwID);
            KG_PROCESS_ERROR(pNpc);
            
            break;
        }
    case ttPlayer:
        {
            KPlayer* pPlayer = NULL;

            KGLOG_PROCESS_ERROR(IS_PLAYER(dwID));
            pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);
            KG_PROCESS_ERROR(pPlayer);

            break;
        }
    case ttDoodad:
        {
            KDoodad* pDoodad = NULL;

            pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwID);
            KG_PROCESS_ERROR(pDoodad);

            break;
        }
    case ttItem:
        {
            KItem* pItem = NULL;
            pItem = g_pSO3World->m_ItemSet.GetObj(dwID);
            KG_PROCESS_ERROR(pItem);
            
            break;
        }
    default:
        KGLOG_PROCESS_ERROR(false && "Unexecpt Target Type!");
    }
    
    m_eTargetType = eType;
    m_dwID        = dwID;

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
        {
            KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(m_dwID);
            KG_PROCESS_ERROR(pNpc);
            *ppCharacter = (KCharacter*)pNpc;
            break;
        }
    case ttPlayer:
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
            KG_PROCESS_ERROR(pPlayer);
            *ppCharacter = (KCharacter*)pPlayer;
            break;
        }
    default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KNpc** ppNpc)    
{
    BOOL    bResult = false;
    KNpc*   pNpc    = NULL;
    
    assert(ppNpc);

    KG_PROCESS_ERROR(m_eTargetType == ttNpc);

    pNpc = g_pSO3World->m_NpcSet.GetObj(m_dwID);
    KG_PROCESS_ERROR(pNpc);

    *ppNpc = pNpc;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KPlayer** ppPlayer)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    assert(ppPlayer);
    KG_PROCESS_ERROR(m_eTargetType == ttPlayer);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
    KG_PROCESS_ERROR(pPlayer);

    *ppPlayer = pPlayer;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KDoodad** ppDoodad)
{
    BOOL        bResult = false;
    KDoodad*    pDoodad = NULL;

    assert(ppDoodad);
    
    KG_PROCESS_ERROR(m_eTargetType == ttDoodad);
    
    pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_dwID);
    KG_PROCESS_ERROR(pDoodad);

    *ppDoodad = pDoodad;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KItem** ppItem)
{
    BOOL    bResult = false;
    KItem*  pItem   = NULL;

    assert(ppItem);
    
    KG_PROCESS_ERROR(m_eTargetType == ttItem);
    
    pItem = g_pSO3World->m_ItemSet.GetObj(m_dwID);
    KG_PROCESS_ERROR(pItem);

    *ppItem = pItem;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(KPlayer** ppPlayer, KItem** ppItem)
{
	BOOL bResult = false;
	KPlayer* pPlayer = NULL;
	KItem* pItem = NULL;

	KG_PROCESS_ERROR(m_eTargetType == ttItemPos);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
	KG_PROCESS_ERROR(pPlayer);

	pItem = pPlayer->m_ItemList.GetItem(m_ItemPos.dwBoxIndex, m_ItemPos.dwX);
	KG_PROCESS_ERROR(pItem);

	*ppPlayer = pPlayer;
	*ppItem = pItem;
		
	bResult = true;
Exit0:
	return bResult;
}

BOOL KTarget::GetTarget(KPlayer** ppPlayer, DWORD* pdwBoxIndex, DWORD* pdwX)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KPlayer*    pPlayer     = NULL;

	KG_PROCESS_ERROR(m_eTargetType == ttItemPos);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
	KG_PROCESS_ERROR(pPlayer);

	*ppPlayer = pPlayer;
	*pdwBoxIndex = m_ItemPos.dwBoxIndex;
	*pdwX = m_ItemPos.dwX;

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
        {
            KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(m_dwID);
            KG_PROCESS_ERROR(pNpc);
            *ppSceneObject = (KSceneObject*)pNpc;
            break;
        }
    case ttPlayer:
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
            KG_PROCESS_ERROR(pPlayer);
            *ppSceneObject = (KSceneObject*)pPlayer;
            break;
        }
    case ttDoodad:
        {
            KDoodad* pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_dwID);
            KG_PROCESS_ERROR(pDoodad);
            *ppSceneObject = (KSceneObject*)pDoodad;
            break;
        }
    default:
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTarget::GetTarget(int* pnX, int* pnY, int* pnZ)
{
    BOOL        bResult  = false;
    BOOL        bRetCode = false;
    KPlayer*    pPlayer  = NULL;
    KNpc*       pNpc     = NULL;
    KDoodad*    pDoodad  = NULL;

    switch (m_eTargetType)
    {
    case ttNpc:
        pNpc = g_pSO3World->m_NpcSet.GetObj(m_dwID);
        KG_PROCESS_ERROR(pNpc);
        bRetCode = pNpc->GetAbsoluteCoordinate(pnX, pnY, pnZ);
        KG_PROCESS_ERROR(bRetCode);
        break;
    
    case ttPlayer:
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
        KG_PROCESS_ERROR(pPlayer);
        bRetCode = pPlayer->GetAbsoluteCoordinate(pnX, pnY, pnZ);
        KG_PROCESS_ERROR(bRetCode);
        break;

    case ttDoodad:
        pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_dwID);
        KG_PROCESS_ERROR(pDoodad);
        bRetCode = pDoodad->GetAbsoluteCoordinate(pnX, pnY, pnZ);
        KG_PROCESS_ERROR(bRetCode);
        break;

    case ttCoordination:
        *pnX = m_Coordination.nX;
        *pnY = m_Coordination.nY;
        *pnZ = m_Coordination.nZ;
        break;

    case ttItem:
        goto Exit0;

	case ttItemPos:
		pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwID);
		KGLOG_PROCESS_ERROR(pPlayer);
        bRetCode = pPlayer->GetAbsoluteCoordinate(pnX, pnY, pnZ);
        KGLOG_PROCESS_ERROR(bRetCode);
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
