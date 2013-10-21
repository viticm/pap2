#include "stdafx.h"
#include "KRegion.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KPlayerEyeshot.h"
#include "KMonster.h"

KPlayerEyeshot::KPlayerEyeshot()  
{
    m_pOwner = NULL;
}
KPlayerEyeshot::~KPlayerEyeshot()
{
}

int KPlayerEyeshot::Init(KMonster *pOwner)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    KG_PROCESS_ERROR(pOwner);

    m_pOwner = pOwner;

    nRetCode = m_NpcSet.Init(1, CLIENT_ID_PREFIX);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = m_PlayerSet.Init(1, PLAYER_ID_PREFIX);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = m_DoodadSet.Init(1, CLIENT_ID_PREFIX);
    KG_PROCESS_ERROR(nRetCode);

    nResult =  TRUE;
Exit0:
    return nResult;
}

int KPlayerEyeshot::UnInit()
{
    DeleteAllObject();
    m_pOwner = NULL;

    return TRUE;
}

int KPlayerEyeshot::Activate()
{
    KActivateObjectFunc<KPlayer> PlayerActivate;
    m_PlayerSet.Traverse(PlayerActivate);

    KActivateObjectFunc<KNpc> NpcActivate;
    m_NpcSet.Traverse(NpcActivate);

    KActivateObjectFunc<KDoodad> DoodadActivate;
    m_DoodadSet.Traverse(DoodadActivate);

    return TRUE;
}

int KPlayerEyeshot::IsVisible(KSceneObject *pObject)
{
    return IsVisible(pObject->m_pRegion);
}

int KPlayerEyeshot::IsVisible(KRegion *pRegion)
{
    if (pRegion && m_pOwner && m_pOwner->m_pRole)
    {
        if (pRegion == m_pOwner->m_pRole->m_pRegion)
            return TRUE;

        for (int i = 0; i < CONNECTED_REGION_TOTAL; ++i)
        {
            if (pRegion == m_pOwner->m_pRole->m_pRegion->m_pConnectRegion[i])
                return TRUE;
        }  
    }
    return FALSE;
}

KPlayer *KPlayerEyeshot::NewPlayer(DWORD dwID)
{
    int nRetCode     = FALSE;
    KPlayer *pPlayer = NULL;

    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);

    pPlayer =  m_PlayerSet.New(dwID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = pPlayer->Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    pPlayer->m_pOwner = m_pOwner;

    return pPlayer;
Exit0:
    if (pPlayer)
    {
        pPlayer->UnInit();
        m_PlayerSet.Delete(pPlayer);
    }
    return NULL;
}

int KPlayerEyeshot::DeletePlayer(KPlayer *pPlayer)
{
    int nResult = FALSE;
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->UnInit();
    m_PlayerSet.Delete(pPlayer);

    nResult = TRUE;
Exit0:
    return nResult;
}

KPlayer *KPlayerEyeshot::GetPlayer(DWORD dwID)
{
    return m_PlayerSet.GetObj(dwID);
}

KNpc *KPlayerEyeshot::NewNpc(DWORD dwID)
{
    int nRetCode = FALSE;
    KNpc *pNpc   = NULL;

    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);

    pNpc =  m_NpcSet.New(dwID);
    KGLOG_PROCESS_ERROR(pNpc);

    nRetCode = pNpc->Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    pNpc->m_pOwner = m_pOwner;

    return pNpc;
Exit0:
    if (pNpc)
    {
        pNpc->UnInit();
        m_NpcSet.Delete(pNpc);
    }
    return NULL;
}

int KPlayerEyeshot::DeleteNpc(KNpc *pNpc)
{
    int nResult = FALSE;

    KGLOG_PROCESS_ERROR(pNpc);

    pNpc->UnInit();
    m_NpcSet.Delete(pNpc);

    nResult = TRUE;
Exit0:
    return nResult;
}

KNpc *KPlayerEyeshot::GetNpc(DWORD dwID)
{
    return m_NpcSet.GetObj(dwID);
}

KDoodad *KPlayerEyeshot::NewDoodad(DWORD dwID)
{
    BOOL bRetCode    = FALSE;
    KDoodad* pDoodad = NULL;

    pDoodad = m_DoodadSet.New(dwID);
    KGLOG_PROCESS_ERROR(pDoodad);

    bRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    pDoodad->m_pOwner = m_pOwner;

    return pDoodad;

Exit0:
    if (pDoodad)
    {
        pDoodad->UnInit();
        m_DoodadSet.Delete(pDoodad);
    }

    return NULL;
}

int KPlayerEyeshot::DeleteDoodad(KDoodad *pDoodad)
{
    int nResult = FALSE;
    KGLOG_PROCESS_ERROR(pDoodad);

    pDoodad->UnInit();
    m_DoodadSet.Delete(pDoodad);

    nResult = TRUE;
Exit0:
    return nResult;
}

KDoodad *KPlayerEyeshot::GetDoodad(DWORD dwID)
{
    return m_DoodadSet.GetObj(dwID);
}

KNpc *KPlayerEyeshot::GetNearNpc()
{
    KGetObjectFunc<KNpc> GetNpc(&m_NpcSet);
    m_NpcSet.Traverse(GetNpc);
    
    return GetNpc.pTarget;
}

int KPlayerEyeshot::DeleteAllObject()
{
    KDeleteObjectFunc<KPlayer> DelPlayer(&m_PlayerSet);
    m_PlayerSet.Traverse(DelPlayer);

    KDeleteObjectFunc<KNpc> DelNpc(&m_NpcSet);
    m_NpcSet.Traverse(DelNpc);

    KDeleteObjectFunc<KDoodad> DelDoodad(&m_DoodadSet);
    m_DoodadSet.Traverse(DelDoodad);

    return TRUE;
}