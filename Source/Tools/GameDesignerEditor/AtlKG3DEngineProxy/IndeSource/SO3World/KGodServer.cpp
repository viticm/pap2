#include "stdafx.h"
#include "SO3ProtocolBasic.h"
#include "KGodServer.h"
#include "KCell.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSO3World.h"
#include "Engine/KMemory.h"

#if defined(_SERVER)

KGodServer g_GodServer;

#define MAX_GOD_CONNECTION       8

KGodServer::KGodServer()
{
    m_pGod      = NULL;

    memset(m_pfnProtocolFunctions, 0, sizeof(m_pfnProtocolFunctions));
    memset(m_uProtocolSize, 0, sizeof(m_uProtocolSize));

    RegisterProtocol(e2g_load_scene_request, sizeof(E2G_LOAD_SCENE_REQUEST), &KGodServer::OnLoadSceneRequest);
    RegisterProtocol(e2g_sync_npc_data, sizeof(E2G_SYNC_NPC_DATA), &KGodServer::OnSyncNpcData);
    RegisterProtocol(e2g_sync_doodad_data, sizeof(E2G_SYNC_DOODAD_DATA), &KGodServer::OnSyncDoodadData);
    RegisterProtocol(e2g_delete_npc, sizeof(E2G_DELETE_NPC), &KGodServer::OnDeleteNpc);
    RegisterProtocol(e2g_delete_doodad, sizeof(E2G_DELETE_DOODAD), &KGodServer::OnDeleteDoodad);
    RegisterProtocol(e2g_delete_all_objects, sizeof(E2G_DELETE_ALL_OBJECTS), &KGodServer::OnDeleteAllObjects);
    RegisterProtocol(e2g_move_character, sizeof(E2G_MOVE_CHARACTER), &KGodServer::OnMoveCharacter);    
}

KGodServer::~KGodServer()
{
}

BOOL KGodServer::Init()
{
    BOOL                bResult             = false;
	BOOL                bRetCode            = false;
    HRESULT             hrRetCode           = S_FALSE;

	m_pGod = new KGod[MAX_GOD_CONNECTION];
	KGLOG_PROCESS_ERROR(m_pGod);
    memset(m_pGod, 0, sizeof(KGod) * MAX_GOD_CONNECTION);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_DELETE_ARRAY(m_pGod);
    }
	return bResult;
}

void KGodServer::UnInit()
{
    KG_DELETE_ARRAY(m_pGod);
}

void KGodServer::Breathe()
{
}

BOOL KGodServer::Send(int nConnectionIndex, void* pvBuffer, size_t uBufferSize)
{
	return false;
}


void KGodServer::RegisterProtocol(int nProtocolID, size_t uPakSize, PGOD_PROTOCOL_FUNC pfnProtocolFunction)
{
    assert(nProtocolID >= g2e_protocol_begin);
    assert(nProtocolID < g2e_protocol_end);

    m_pfnProtocolFunctions[nProtocolID] = pfnProtocolFunction;
    m_uProtocolSize[nProtocolID]        = uPakSize;
}

BOOL KGodServer::CheckProtocol(BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                    bResult         = false;
    KGOD_PROTOCOL_HEADER*   pProtocolHeader = NULL;
    
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KGOD_PROTOCOL_HEADER));
    pProtocolHeader = (KGOD_PROTOCOL_HEADER*)pbyBuffer;

    KGLOG_PROCESS_ERROR(pProtocolHeader->byProtocolID < e2g_protocol_end);

    KGLOG_PROCESS_ERROR(
        (uBufferSize == m_uProtocolSize[pProtocolHeader->byProtocolID]) || 
        (m_uProtocolSize[pProtocolHeader->byProtocolID] == UNDEFINED_PROTOCOL_SIZE)
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodServer::OnLoadSceneRequest(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    E2G_LOAD_SCENE_REQUEST* pRequest    = (E2G_LOAD_SCENE_REQUEST*)pbyData;
    KScene*                 pScene      = NULL;
    G2E_LOAD_SCENE_RESPOND  Respond;

    // 先直接写死副本ID号为1了,以后再改
    pScene = g_pSO3World->GetScene(pRequest->dwMapID, 1);
    KGLOG_PROCESS_ERROR(pScene);

    m_pGod[nConnIndex].pScene = pScene;

    Respond.byProtocolID = g2e_load_scene_respond;
    Respond.dwMapID      = pScene->m_dwMapID;
    Respond.nCopyIndex   = pScene->m_nCopyIndex;

    bRetCode = Send(nConnIndex, &Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    pScene->DeleteAllObjects();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodServer::OnSyncNpcData(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    E2G_SYNC_NPC_DATA*  pRequest        = (E2G_SYNC_NPC_DATA*)pbyData;
    KNpc*               pNpc            = NULL;
	BOOL                bNpcRegFlag     = false;
	BOOL                bNpcInitFlag    = false;
    G2E_CREATE_NPC      Respond;

    pNpc = KMemory::New<KNpc>();
    KGLOG_PROCESS_ERROR(pNpc);

	bRetCode = g_pSO3World->m_NpcSet.Register(pNpc, ERROR_ID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bNpcRegFlag = true;

    bRetCode = pNpc->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bNpcInitFlag = true;

    bRetCode = pNpc->Load(pRequest->NpcData);
    KGLOG_PROCESS_ERROR(bRetCode);

    Respond.byProtocolID = g2e_create_npc;
    Respond.dwDataID     = pRequest->dwDataID;
    Respond.dwNpcID      = pNpc->m_dwID;

    bRetCode = Send(nConnIndex, &Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->AddNpc(
        pNpc, m_pGod[nConnIndex].pScene, 
        pRequest->NpcData.nX, pRequest->NpcData.nY, pRequest->NpcData.nZ
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bNpcInitFlag)
        {
            pNpc->UnInit();
            bNpcInitFlag = false;
        }

		if (bNpcRegFlag)
		{
			g_pSO3World->m_NpcSet.Unregister(pNpc);
			bNpcRegFlag = false;
		}

        if (pNpc)
        {
            KMemory::Delete(pNpc);
            pNpc = NULL;
        }
    }
    return bResult;
}

BOOL KGodServer::OnSyncDoodadData(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    E2G_SYNC_DOODAD_DATA*   pRequest        = (E2G_SYNC_DOODAD_DATA*)pbyData;
    KDoodad*                pDoodad         = NULL;
	BOOL                    bDoodadRegFlag  = false;
	BOOL                    bDoodadInitFlag = false;
    G2E_CREATE_DOODAD       Respond;

    pDoodad = KMemory::New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bDoodadRegFlag = true;

    bRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadInitFlag = true;

    bRetCode = pDoodad->Load(pRequest->DoodadData);
    KGLOG_PROCESS_ERROR(bRetCode);

    Respond.byProtocolID = g2e_create_doodad;
    Respond.dwDataID     = pRequest->dwDataID;
    Respond.dwDoodadID   = pDoodad->m_dwID;

    bRetCode = Send(nConnIndex, &Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->AddDoodad(
        pDoodad, m_pGod[nConnIndex].pScene, 
        pRequest->DoodadData.nX, pRequest->DoodadData.nY, pRequest->DoodadData.nZ
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bDoodadInitFlag)
        {
            pDoodad->UnInit();
            bDoodadInitFlag = false;
        }

		if (bDoodadRegFlag)
		{
			g_pSO3World->m_DoodadSet.Unregister(pDoodad);
			bDoodadRegFlag = false;
		}

        if (pDoodad)
        {
            KMemory::Delete(pDoodad);
            pDoodad = NULL;
        }
    }
    return bResult;
}

BOOL KGodServer::OnDeleteNpc(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    E2G_DELETE_NPC*     pRequest    = (E2G_DELETE_NPC*)pbyData;
    KNpc*               pNpc        = NULL;

    pNpc = g_pSO3World->m_NpcSet.GetObj(pRequest->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    bRetCode = g_pSO3World->DeleteNpc(pNpc);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodServer::OnDeleteDoodad(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    E2G_DELETE_DOODAD*      pRequest    = (E2G_DELETE_DOODAD*)pbyData;
    KDoodad*                pDoodad     = NULL;

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(pRequest->dwDoodadID);
    KGLOG_PROCESS_ERROR(pDoodad);

    bRetCode = g_pSO3World->DelDoodad(pDoodad);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodServer::OnDeleteAllObjects(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                    bResult     = false;
    E2G_DELETE_ALL_OBJECTS* pRequest    = (E2G_DELETE_ALL_OBJECTS*)pbyData;
    KScene*                 pScene      = NULL;

    pScene = m_pGod[nConnIndex].pScene;
    KGLOG_PROCESS_ERROR(pScene);

    KGLOG_PROCESS_ERROR(pScene->m_dwMapID == pRequest->dwMapID);
    KGLOG_PROCESS_ERROR(pScene->m_nCopyIndex == pRequest->nMapCopyIndex);

    pScene->DeleteAllObjects();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodServer::OnMoveCharacter(int nConnIndex, BYTE* pbyData, size_t uSize)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    E2G_MOVE_CHARACTER*     pRequest    = (E2G_MOVE_CHARACTER*)pbyData;
    KScene*                 pScene      = NULL;
    KCharacter*             pCharacter  = NULL;

    pScene = m_pGod[nConnIndex].pScene;
    KGLOG_PROCESS_ERROR(pScene);

    if (IS_PLAYER(pRequest->dwCharacterID))
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pRequest->dwCharacterID);
    else
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pRequest->dwCharacterID);
    KGLOG_PROCESS_ERROR(pCharacter);

    bRetCode = pCharacter->MoveTo(pRequest->nX, pRequest->nY, pRequest->nZ, false, true);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

#endif	//_SERVER
