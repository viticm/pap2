#include "stdafx.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KGodClient.h"
#include "GE_Protocol.h"
#include "KNpcTemplateList.h"
#include "KCell.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSO3World.h"
#include "Engine/KMemory.h"

#if defined(_CLIENT)

KGodClient g_GodClient;

KGodClient::KGodClient()
{
    m_piSocket      = NULL;
    m_pCurrentScene = NULL;

    memset(m_pfnProtocolFunctions, 0, sizeof(m_pfnProtocolFunctions));
    memset(m_uProtocolSize, 0, sizeof(m_uProtocolSize));

    RegisterProtocol(g2e_load_scene_respond, sizeof(G2E_LOAD_SCENE_RESPOND), &KGodClient::OnLoadSceneRespond);
    RegisterProtocol(g2e_create_npc, sizeof(G2E_CREATE_NPC), &KGodClient::OnCreateNpc);
    RegisterProtocol(g2e_create_doodad, sizeof(G2E_CREATE_DOODAD), &KGodClient::OnCreateDoodad);
}

KGodClient::~KGodClient()
{
}

BOOL KGodClient::Init()
{
    BOOL bResult   = false;
    
    KG_ASSERT_EXIT(!m_piSocket);
    KG_ASSERT_EXIT(!m_pCurrentScene);

    bResult = true;
Exit0:
    return bResult;
}

void KGodClient::UnInit()
{
    KG_COM_RELEASE(m_piSocket);

    m_NpcData.clear();
    m_dwNpcDataIDAlloc = 0;
    m_NpcSyncList.clear();
    m_Npc2Data.clear();

    m_DoodadData.clear();
    m_dwDoodadDataIDAlloc = 0;
    m_DoodadSyncList.clear();
    m_Doodad2Data.clear();

    m_pCurrentScene = NULL;
}


void KGodClient::Breathe()
{
    int                     nRetCode            = 0;
    timeval                 TimeValue           = {0, 100};
    IKG_Buffer*             piBuffer            = NULL;
    BYTE*                   pbyData             = NULL;
    size_t                  uDataLen            = 0;
    KGOD_PROTOCOL_HEADER*   pProtocolHeader     = NULL;
    PGOD_PROTOCOL_FUNC      pProtocolFunction   = NULL;

    KG_PROCESS_ERROR(m_piSocket);

    SyncNpcData();
    SyncDoodadData();

    nRetCode = m_piSocket->CheckCanRecv(&TimeValue);
    KG_PROCESS_ERROR(nRetCode == 1);

    nRetCode = m_piSocket->Recv(&piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pbyData  = (BYTE*)piBuffer->GetData();
    uDataLen = (size_t)piBuffer->GetSize();

    nRetCode = CheckProtocol(pbyData, uDataLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    pProtocolHeader     = (KGOD_PROTOCOL_HEADER*)pbyData;
    pProtocolFunction   = m_pfnProtocolFunctions[pProtocolHeader->byProtocolID];
    assert(pProtocolFunction);
    (this->*pProtocolFunction)(pbyData, uDataLen);

Exit0:
    KG_COM_RELEASE(piBuffer);
    return;
}


BOOL KGodClient::Connect(const char cszIP[], u_short uPort)
{
	BOOL				bResult = false;
    KG_SocketConnector	Connector;

	assert(!m_piSocket);

    m_piSocket = Connector.Connect(cszIP, uPort);
    KGLOG_PROCESS_ERROR(m_piSocket);

	bResult = true;
Exit0:
	return bResult;
}

void KGodClient::Disconnect()
{
	KG_COM_RELEASE(m_piSocket);
}

BOOL KGodClient::Send(void* pvData, size_t uSize)
{
    BOOL        bResult  = false;
    int         nRetCode = false;
    IKG_Buffer* piBuffer = NULL;
    void*       pvBuffer = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pvBuffer= piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pvBuffer);

    memcpy(pvBuffer, pvData, uSize);

    nRetCode = m_piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGodClient::SyncNpcData()
{
    BOOL                      bResult       = false;
    BOOL                      bRetCode      = false;
    DWORD                     dwNpcDataID   = 0;
    KNPC_DATA_TABLE::iterator NpcDataNode;
    const KNPC_DATA*          pcNpcData     = NULL;
    KNpcTemplate*             pNpcTemplate	= NULL;
    KNpc*                     pNpc          = NULL;
	BOOL                      bNpcRegFlag   = false;
    BOOL                      bNpcInitFlag  = false;

    KG_PROCESS_ERROR(g_pSO3World->m_eWorldMode == wmOnlineEdit);

    bRetCode = m_NpcSyncList.empty();
    KG_PROCESS_ERROR(!bRetCode);

    dwNpcDataID = m_NpcSyncList.front();
    m_NpcSyncList.pop_front();
    NpcDataNode = m_NpcData.find(dwNpcDataID);
    KGLOG_PROCESS_ERROR(NpcDataNode != m_NpcData.end());

    pcNpcData   = &NpcDataNode->second;

    pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pcNpcData->dwTemplateID);
    KGLOG_PROCESS_ERROR(pNpcTemplate);

    if (pNpcTemplate->nKind == ckClientNpc)
    {
        pNpc = KMemory::New<KNpc>();
        KGLOG_PROCESS_ERROR(pNpc);

		bRetCode = g_pSO3World->m_NpcSet.Register(pNpc, ERROR_ID);
		KGLOG_PROCESS_ERROR(bRetCode);
		bNpcRegFlag = true;

        bRetCode = pNpc->Init();
        KGLOG_PROCESS_ERROR(bRetCode);
        bNpcInitFlag = true;

        bRetCode = pNpc->Load(*pcNpcData);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = g_pSO3World->AddNpc(pNpc, m_pCurrentScene, pcNpcData->nX, pcNpcData->nY, pcNpcData->nZ);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        E2G_SYNC_NPC_DATA Pak;

        Pak.byProtocolID = e2g_sync_npc_data;
        Pak.dwDataID     = NpcDataNode->first;
        Pak.NpcData      = NpcDataNode->second;
        bRetCode = Send(&Pak, sizeof(Pak));
        KGLOG_PROCESS_ERROR(bRetCode);
    }

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

BOOL KGodClient::SyncDoodadData()
{
    BOOL                            bResult          = false;
    BOOL                            bRetCode         = false;
    DWORD                           dwDoodadDataID   = 0;
    const KDOODAD_DATA*             pcDoodadData     = NULL;
    KDoodadTemplate*                pDoodadTemplate  = NULL;
    KDoodad*                        pDoodad          = NULL;
	BOOL                            bDoodadRegFlag   = false;
	BOOL                            bDoodadInitFlag  = false;
    KDOODAD_DATA_TABLE::iterator    DoodadDataNode;

    KG_PROCESS_ERROR(g_pSO3World->m_eWorldMode == wmOnlineEdit);

    bRetCode = m_DoodadSyncList.empty();
    KG_PROCESS_ERROR(!bRetCode);

    dwDoodadDataID = m_DoodadSyncList.front();
    m_DoodadSyncList.pop_front();
    DoodadDataNode = m_DoodadData.find(dwDoodadDataID);
    KGLOG_PROCESS_ERROR(DoodadDataNode != m_DoodadData.end());

    pcDoodadData   = &DoodadDataNode->second;

    pDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pcDoodadData->dwTemplateID);
    KGLOG_PROCESS_ERROR(pDoodadTemplate);

    if (pDoodadTemplate->m_eKind == dkClientOnly)
    {
        pDoodad = KMemory::New<KDoodad>();
        KGLOG_PROCESS_ERROR(pDoodad);

		bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
		KGLOG_PROCESS_ERROR(bRetCode);
		bDoodadRegFlag = true;

        bRetCode = pDoodad->Init();
        KGLOG_PROCESS_ERROR(bRetCode);
        bDoodadInitFlag = true;

        bRetCode = pDoodad->Load(*pcDoodadData);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = g_pSO3World->AddDoodad(pDoodad, m_pCurrentScene, pcDoodadData->nX, pcDoodadData->nY, pcDoodadData->nZ);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        E2G_SYNC_DOODAD_DATA Pak;

        Pak.byProtocolID = e2g_sync_doodad_data;
        Pak.dwDataID     = DoodadDataNode->first;
        Pak.DoodadData   = DoodadDataNode->second;
        bRetCode = Send(&Pak, sizeof(Pak));
        KGLOG_PROCESS_ERROR(bRetCode);
    }

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

BOOL KGodClient::IsSyncComplete()
{
    BOOL bNpcSyncOver = m_NpcSyncList.empty();
    BOOL bDoodadSyncOver = m_DoodadSyncList.empty();
    return bNpcSyncOver && bDoodadSyncOver;
}

KScene* KGodClient::GetCurrentScene()
{
    return m_pCurrentScene;
}

BOOL KGodClient::Editor2Game()
{
    m_Npc2Data.clear();
    m_Doodad2Data.clear();

    return true;
}

BOOL KGodClient::Game2Editor()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;

    m_Npc2Data.clear();
    m_Doodad2Data.clear();

    for (KNPC_DATA_TABLE::iterator it = m_NpcData.begin(); it != m_NpcData.end(); ++it)
    {
        m_NpcSyncList.push_back(it->first);
    }

    for (KDOODAD_DATA_TABLE::iterator it = m_DoodadData.begin(); it != m_DoodadData.end(); ++it)
    {
        m_DoodadSyncList.push_back(it->first);
    }

    bResult = true;
Exit0:
    return bResult;
}


BOOL KGodClient::DoLoadSceneRequest(DWORD dwMapID)
{
    E2G_LOAD_SCENE_REQUEST Pak;

    Pak.byProtocolID = e2g_load_scene_request;
    Pak.dwMapID      = dwMapID;

    if (m_pCurrentScene)
    {
        m_NpcData.clear();
        m_dwNpcDataIDAlloc = 0;
        m_NpcSyncList.clear();
        m_Npc2Data.clear();

        m_DoodadData.clear();
        m_dwDoodadDataIDAlloc = 0;
        m_DoodadSyncList.clear();
        m_Doodad2Data.clear();

        m_pCurrentScene = NULL;
    }

    return Send(&Pak, sizeof(Pak));
}

BOOL KGodClient::DoAppendNpcData(const KNPC_DATA& crNpcData)
{
    m_NpcData[m_dwNpcDataIDAlloc] = crNpcData;
    m_NpcSyncList.push_back(m_dwNpcDataIDAlloc);
    m_dwNpcDataIDAlloc++;
    return true;
}

BOOL KGodClient::DoAppendDoodadData(const KDOODAD_DATA& crDoodadData)
{
    m_DoodadData[m_dwDoodadDataIDAlloc] = crDoodadData;
    m_DoodadSyncList.push_back(m_dwDoodadDataIDAlloc);
    m_dwDoodadDataIDAlloc++;
    return true;
}

    
BOOL KGodClient::DoDelNpcData(KNpc* pNpc)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    KOBJ_2_DATA_MAP::iterator IDMapNode;

    IDMapNode = m_Npc2Data.find(pNpc->m_dwID);
    KGLOG_PROCESS_ERROR(IDMapNode != m_Npc2Data.end());

    if (pNpc->m_eKind != ckClientNpc)
    {
        E2G_DELETE_NPC Pak;
        Pak.byProtocolID = e2g_delete_npc;
        Pak.dwNpcID      = pNpc->m_dwID;
        bRetCode = Send(&Pak, sizeof(Pak));
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    m_NpcData.erase(IDMapNode->second);
    m_Npc2Data.erase(IDMapNode);
    
    bResult = true;
Exit0:
    return bResult;
}

    
BOOL KGodClient::DoDelDoodadData(KDoodad* pDoodad)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    KOBJ_2_DATA_MAP::iterator IDMapNode;

    IDMapNode = m_Doodad2Data.find(pDoodad->m_dwID);
    KGLOG_PROCESS_ERROR(IDMapNode != m_Doodad2Data.end());

    if (pDoodad->m_eKind != dkClientOnly)
    {
        E2G_DELETE_DOODAD Pak;
        Pak.byProtocolID = e2g_delete_doodad;
        Pak.dwDoodadID   = pDoodad->m_dwID;
        bRetCode = Send(&Pak, sizeof(Pak));
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    m_DoodadData.erase(IDMapNode->second);
    m_Doodad2Data.erase(IDMapNode);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodClient::DoDeleteAllObjects()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    E2G_DELETE_ALL_OBJECTS Pak;

    assert(m_pCurrentScene);

    Pak.byProtocolID    = e2g_delete_all_objects;
    Pak.dwMapID         = m_pCurrentScene->m_dwID;
    Pak.nMapCopyIndex   = m_pCurrentScene->m_nCopyIndex;

    bRetCode = Send(&Pak, sizeof(Pak));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodClient::DoMoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
    E2G_MOVE_CHARACTER  Request;

    Request.byProtocolID    = e2g_move_character;
    Request.dwCharacterID   = pCharacter->m_dwID;
    Request.nX              = nX;
    Request.nY              = nY;
    Request.nZ              = nZ;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    if (!IS_PLAYER(pCharacter->m_dwID))
    {
        KOBJ_2_DATA_MAP::iterator Obj2Data;
        KNPC_DATA_TABLE::iterator DataNode;
        KNPC_DATA*                pNpcData = NULL;

        Obj2Data = m_Npc2Data.find(pCharacter->m_dwID);
        KGLOG_PROCESS_ERROR(Obj2Data != m_Npc2Data.end());

        DataNode = m_NpcData.find(Obj2Data->second);
        KGLOG_PROCESS_ERROR(DataNode != m_NpcData.end());

        pNpcData = &DataNode->second;

        pNpcData->nX = nX;
        pNpcData->nY = nY;
        pNpcData->nZ = nZ;
    }

    bResult = true;
Exit0:
    return bResult;
}

void KGodClient::RegisterProtocol(int nProtocolID, size_t uPakSize, PGOD_PROTOCOL_FUNC pfnProtocolFunction)
{
    assert(nProtocolID >= g2e_protocol_begin);
    assert(nProtocolID < g2e_protocol_end);

    m_pfnProtocolFunctions[nProtocolID] = pfnProtocolFunction;
    m_uProtocolSize[nProtocolID]        = uPakSize;
}

BOOL KGodClient::CheckProtocol(BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                    bResult         = false;
    KGOD_PROTOCOL_HEADER*   pProtocolHeader = NULL;
    
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KGOD_PROTOCOL_HEADER));
    pProtocolHeader = (KGOD_PROTOCOL_HEADER*)pbyBuffer;

    KGLOG_PROCESS_ERROR(pProtocolHeader->byProtocolID >= g2e_protocol_begin);
    KGLOG_PROCESS_ERROR(pProtocolHeader->byProtocolID < g2e_protocol_end);

    KGLOG_PROCESS_ERROR(
        (uBufferSize == m_uProtocolSize[pProtocolHeader->byProtocolID]) || 
        (m_uProtocolSize[pProtocolHeader->byProtocolID] == UNDEFINED_PROTOCOL_SIZE)
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodClient::OnLoadSceneRespond(BYTE* pbyData, size_t uSize)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    G2E_LOAD_SCENE_RESPOND* pRespond    = (G2E_LOAD_SCENE_RESPOND*)pbyData;
    KScene*                 pScene      = NULL;
	KMapParams*             pMapParams  = NULL;

    pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(pRespond->dwMapID);
	KGLOG_PROCESS_ERROR(pMapParams);

    g_pSO3World->m_eWorldMode = wmOnlineEdit;

    pScene = g_pSO3World->LoadEditorScene(pMapParams->szMapName,NULL);
    KGLOG_PROCESS_ERROR(pScene);

    pScene->m_nCopyIndex = pRespond->nCopyIndex;

    m_pCurrentScene = pScene;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGodClient::OnCreateNpc(BYTE* pbyData, size_t uSize)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    KNpc*           pNpc            = NULL;
    BOOL            bNpcInitFlag    = false;
    BOOL            bNpcRegFlag     = false;
    G2E_CREATE_NPC* pRespond        = (G2E_CREATE_NPC*)pbyData;
    KNPC_DATA*      pNpcData        = NULL;
    KNPC_DATA_TABLE::iterator NpcDataNode;

    KGLOG_PROCESS_ERROR(g_pSO3World->m_eWorldMode == wmOnlineEdit);
    KGLOG_PROCESS_ERROR(m_pCurrentScene);

    NpcDataNode = m_NpcData.find(pRespond->dwDataID);
    KGLOG_PROCESS_ERROR(NpcDataNode != m_NpcData.end());

    pNpcData = &NpcDataNode->second;

    pNpc = KMemory::New<KNpc>();
    KGLOG_PROCESS_ERROR(pNpc);

	bRetCode = g_pSO3World->m_NpcSet.Register(pNpc, pRespond->dwNpcID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bNpcRegFlag = true;

    bRetCode = pNpc->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bNpcInitFlag = true;

    bRetCode = pNpc->Load(*pNpcData);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->AddNpc(
        pNpc, m_pCurrentScene, pNpcData->nX, pNpcData->nY, pNpcData->nZ
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

BOOL KGodClient::OnCreateDoodad(BYTE* pbyData, size_t uSize)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KDoodad*            pDoodad         = NULL;
	BOOL                bDoodadRegFlag  = false;
	BOOL                bDoodadInitFlag = false;
    G2E_CREATE_DOODAD*  pRespond        = (G2E_CREATE_DOODAD*)pbyData;
    KDOODAD_DATA*       pDoodadData     = NULL;
    KDOODAD_DATA_TABLE::iterator DoodadDataNode;

    KGLOG_PROCESS_ERROR(g_pSO3World->m_eWorldMode == wmOnlineEdit);
    KGLOG_PROCESS_ERROR(m_pCurrentScene);

    DoodadDataNode = m_DoodadData.find(pRespond->dwDataID);
    KGLOG_PROCESS_ERROR(DoodadDataNode != m_DoodadData.end());
    pDoodadData = &DoodadDataNode->second;

    pDoodad = KMemory::New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, pRespond->dwDoodadID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bDoodadRegFlag = true;

    bRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadInitFlag = true;

    bRetCode = pDoodad->Load(*pDoodadData);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->AddDoodad(
        pDoodad, m_pCurrentScene, pDoodadData->nX, pDoodadData->nY, pDoodadData->nZ
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


#endif
