#include "stdafx.h"

#include "KNpc.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KNpcReviveManager.h"

KNpcReviveManager::KNpcReviveManager()
{
    m_pScene = NULL;
}

KNpcReviveManager::~KNpcReviveManager()
{
}

BOOL KNpcReviveManager::Init(KScene* pScene)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    assert(pScene);

    m_pScene = pScene;

    m_NpcReviveMap.clear();

    m_NpcRandomMap.clear();

    m_NpcRandomDataMap.clear();
    
    m_NpcTableMap.clear();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::UnInit()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    m_pScene = NULL;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        KNPC_REVIVE_QUEUE*  pNpcReviveQueue = &(it->second);

        for (
            std::deque<KNPC_REVIVE_NODE>::iterator QueueIt = pNpcReviveQueue->ReviveQueue.begin(); 
            QueueIt != pNpcReviveQueue->ReviveQueue.end(); ++QueueIt
        )
        {
            g_pSO3World->DeleteNpc(QueueIt->pNpc);
        }
    }
    m_NpcReviveMap.clear();

    m_NpcRandomMap.clear();

    m_NpcRandomDataMap.clear();

    m_NpcTableMap.clear();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::Load(const char cszSceneName[])
{
    BOOL bResult                = false;
    BOOL bRetCode               = false;
    char szFilePath[MAX_PATH];

    KGLOG_PROCESS_ERROR(cszSceneName);
    KGLOG_PROCESS_ERROR(m_pScene);

    snprintf(
        szFilePath, sizeof(szFilePath), 
        "%s/map/%s/NpcRelive/MapReviveList.tab",
        SETTING_DIR, cszSceneName
    );
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    bRetCode = LoadNpcReviveTable(szFilePath);
    KGLOG_CHECK_ERROR(bRetCode);

    snprintf(
        szFilePath, sizeof(szFilePath), 
        "%s/map/%s/NpcRelive/NpcRandomList.tab", 
        SETTING_DIR, cszSceneName
    );
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    bRetCode = LoadNpcRandomTable(szFilePath, cszSceneName);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KNpcReviveManager::Activate()
{
    BOOL bResult        = false;
    BOOL bRetCode       = false;
    int  nReviveCount   = 0;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        KNPC_REVIVE_QUEUE*  pNpcReviveQueue = &(it->second);

        while (true)
        {
            if (nReviveCount > MAX_REVIVE_NPC_PER_FRAME)
            {
                break;
            }

            if ((int)pNpcReviveQueue->ReviveQueue.size() <= pNpcReviveQueue->nMinQueueSize)
                break;

            if (pNpcReviveQueue->ReviveQueue.empty())
                break;

            if (pNpcReviveQueue->ReviveQueue.begin()->nFrame > g_pSO3World->m_nGameLoop)
                break;

            ReviveNpc(it->first);

            nReviveCount++;
        }
    }
}

static BOOL ReviveNodeFrameComp(const KNPC_REVIVE_NODE& crLeft, const KNPC_REVIVE_NODE& crRight)
{
    return crLeft.nFrame < crRight.nFrame;
}

BOOL KNpcReviveManager::AddNpc(KNpc* pNpc, int nReviveTime)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwReviveID  = 0;
    std::deque<KNPC_REVIVE_NODE>::iterator it;
    KNPC_REVIVE_NODE ReviveNode;

    KGLOG_PROCESS_ERROR(pNpc);
    assert(pNpc->m_pTemplate);
    dwReviveID = pNpc->m_dwReliveID;

#ifdef _SERVER
    pNpc->m_SimpThreatList.ClearAllThreat();
    pNpc->m_SimpThreatList.ClearAllThreaten();
#endif

    ReviveNode.pNpc = pNpc;
    ReviveNode.nFrame = g_pSO3World->m_nGameLoop + nReviveTime * GAME_FPS;

    it = std::upper_bound(
        m_NpcReviveMap[dwReviveID].ReviveQueue.begin(), 
        m_NpcReviveMap[dwReviveID].ReviveQueue.end(), 
        ReviveNode, ReviveNodeFrameComp
    );
    m_NpcReviveMap[dwReviveID].ReviveQueue.insert(it, ReviveNode);

    if (m_NpcReviveMap[dwReviveID].nMaxQueueSize < (int)m_NpcReviveMap[dwReviveID].ReviveQueue.size())
    {
        ReviveNpc(dwReviveID);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::LoadNpc(KNpc* pNpc, KNPC_DATA& rNpcData)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    KNpcVector*             pNpcVector  = NULL;
    DWORD                   dwRandomID  = 0;
    KNpcRandomMap::iterator it;
    KNpcVector              NpcVector;
    KNpcRandomKey           NpcRandomKey;
    KNPC_RANDOM_NODE        NpcRandomNode;

    assert(pNpc);

    dwRandomID = rNpcData.dwRandomID;

    if (dwRandomID == 0)
    {
        bRetCode = pNpc->Load(rNpcData);
        KGLOG_PROCESS_ERROR(bRetCode);

        AddNpc(pNpc, 0);
        goto Exit1;
    }

    NpcRandomKey = std::make_pair(pNpc->m_dwReliveID, dwRandomID);
    NpcRandomNode.pNpc      = pNpc;
    NpcRandomNode.NpcData   = rNpcData;

    it = m_NpcRandomMap.find(NpcRandomKey);

    if (it == m_NpcRandomMap.end())
    {
        NpcVector.push_back(NpcRandomNode);

        m_NpcRandomMap.insert(std::make_pair(NpcRandomKey, NpcVector));

        goto Exit1;
    }

    pNpcVector = &(it->second);
    pNpcVector->push_back(NpcRandomNode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::ForceGroupNpcRevive(int dwReviveID)
{
    BOOL                    bResult = false;
    KNPC_REVIVE_QUEUE*      pReviveQueue    = NULL;
    KNpcReviveMap::iterator itFind;
    
    itFind = m_NpcReviveMap.find(dwReviveID);
    KGLOG_PROCESS_ERROR(itFind != m_NpcReviveMap.end());

    pReviveQueue = &(itFind->second);

    for (
        std::deque<KNPC_REVIVE_NODE>::iterator it = pReviveQueue->ReviveQueue.begin(), itEnd = pReviveQueue->ReviveQueue.end();
        it != itEnd;
        ++it
    )
    {
        it->nFrame = g_pSO3World->m_nGameLoop;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::CompleteLoading()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    assert(m_pScene);

    bRetCode = CreateRandomNpc();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CalculateReviveLimit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = DisorganizeReviveList();
    KGLOG_PROCESS_ERROR(bRetCode);

    for (
        KNpcReviveMap::iterator QueueIter = m_NpcReviveMap.begin(); 
        QueueIter != m_NpcReviveMap.end(); ++QueueIter
    )
    {
        deque<KNPC_REVIVE_NODE>* pQueue = &(QueueIter->second.ReviveQueue);
        for (
            deque<KNPC_REVIVE_NODE>::iterator NpcIter = pQueue->begin(); 
            NpcIter != pQueue->end(); ++NpcIter
        )
        {
            KNpc*           pNpc            = NpcIter->pNpc;
            KNpcTemplate*	pNpcTemplate    = pNpc->m_pTemplate;

            pNpc->SetRevivePoint(pNpc->m_nX, pNpc->m_nY, pNpc->m_nZ, pNpc->m_nFaceDirection);

            if (!pNpc->m_AIData.nAIType)
            {
                KGLogPrintf(
                    KGLOG_ERR, 
                    "[NPC] Invalid AIType: %d, MapId: %d, NpcTemplate: %d, X: %d, Y: %d, Z: %d", 
                    pNpc->m_AIData.nAIType, m_pScene->m_dwMapID, pNpcTemplate->dwTemplateID, pNpc->m_nX, pNpc->m_nY, pNpc->m_nZ
                );
            }

#ifdef _SERVER
            if (pNpc->m_nNpcTeamID)
			{
                bRetCode = m_pScene->m_ThreatLink.AddMember(pNpc->m_nNpcTeamID, pNpc);
                KGLOG_PROCESS_ERROR(bRetCode);
			}
#endif
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

// DEBUG BEGIN
BOOL KNpcReviveManager::GetReviveGroupState(int nGroupID, int* pnRetSize, int* pnRetMinSize, int* pnRetMaxSize, int* pnRetFristReviveTime)
{
    BOOL                    bResult         = false;
    KNPC_REVIVE_QUEUE*      pNpcReviveNode  = NULL;
    KNpcReviveMap::iterator it;

    assert(pnRetSize);
    assert(pnRetMinSize);
    assert(pnRetMaxSize);

    KGLOG_PROCESS_ERROR(nGroupID >= 0);

    it = m_NpcReviveMap.find(nGroupID);
    KGLOG_PROCESS_ERROR(it != m_NpcReviveMap.end());

    pNpcReviveNode = &(it->second);
    KGLOG_PROCESS_ERROR(pNpcReviveNode);

    *pnRetSize      = (int)pNpcReviveNode->ReviveQueue.size();
    *pnRetMaxSize   = pNpcReviveNode->nMaxQueueSize;
    *pnRetMinSize   = pNpcReviveNode->nMinQueueSize;

    if (!pNpcReviveNode->ReviveQueue.empty())
    {
        *pnRetFristReviveTime = pNpcReviveNode->ReviveQueue.front().nFrame;
    }

    bResult = true;
Exit0:
    return bResult;
}
// DEBUG END

BOOL KNpcReviveManager::DestroyNpc(KNpc* pNpc)
{
    BOOL                                    bResult         = false;
    KNPC_REVIVE_QUEUE*                      pNpcReviveQue   = NULL;
    KNpcReviveMap::iterator                 Findit;
    std::deque<KNPC_REVIVE_NODE>::iterator  it;
    std::deque<KNPC_REVIVE_NODE>::iterator  itEnd;

    assert(pNpc);

    Findit = m_NpcReviveMap.find(pNpc->m_dwReliveID);
    KGLOG_PROCESS_ERROR(Findit != m_NpcReviveMap.end());

    pNpcReviveQue = &(Findit->second);
    KGLOG_PROCESS_ERROR(pNpcReviveQue);

    itEnd = pNpcReviveQue->ReviveQueue.end();
    for (it = pNpcReviveQue->ReviveQueue.begin(); it != itEnd; ++it)
    {
        KNPC_REVIVE_NODE* pNode = &(*it);
        
        if (pNode->pNpc->m_dwID != pNpc->m_dwID)
            continue;

        pNpcReviveQue->ReviveQueue.erase(it);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::ForceReviveNpc(KNpc* pNpc)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    BOOL                                    bOldRandom      = false;
    KNPC_REVIVE_QUEUE*                      pNpcReviveQue   = NULL;
    KNPC_REVIVE_NODE*                       pNpcReviveNode  = NULL;
    KNpc*                                   pTempNpc        = NULL;
    KNpcReviveMap::iterator                 Findit;
    std::deque<KNPC_REVIVE_NODE>::iterator  it;
    std::deque<KNPC_REVIVE_NODE>::iterator  itEnd;

    assert(pNpc);

    Findit = m_NpcReviveMap.find(pNpc->m_dwReliveID);
    KGLOG_PROCESS_ERROR(Findit != m_NpcReviveMap.end());

    pNpcReviveQue = &(Findit->second);
    KGLOG_PROCESS_ERROR(pNpcReviveQue);

    itEnd = pNpcReviveQue->ReviveQueue.end();
    for (it = pNpcReviveQue->ReviveQueue.begin(); it != itEnd; ++it)
    {
        pNpcReviveNode = &(*it);

        if (pNpcReviveNode->pNpc->m_dwID == pNpc->m_dwID)
            break;
    }

    KGLOG_PROCESS_ERROR(it != itEnd);

    pTempNpc                           = pNpcReviveQue->ReviveQueue[0].pNpc;
    pNpcReviveQue->ReviveQueue[0].pNpc = pNpcReviveNode->pNpc;
    pNpcReviveNode->pNpc               = pTempNpc;

    bOldRandom = pNpcReviveQue->bRandom;
    pNpcReviveQue->bRandom = false;

    bRetCode = ReviveNpc(pNpc->m_dwReliveID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pNpcReviveQue->bRandom = bOldRandom;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::ReviveNpc(DWORD dwReviveID)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    BOOL                bProgressFlag   = false;
    int                 nSize           = 0;
    int                 nIndex          = 0;
    int                 nX              = 0;
    int                 nY              = 0;
    int                 nZ              = 0;
    int                 nDir            = 0;
    KNpc*               pNpc            = NULL;
    KNPC_REVIVE_QUEUE*  pReviveQueue    = NULL;

    pReviveQueue = &(m_NpcReviveMap[dwReviveID]);
    assert(pReviveQueue);

    nSize = (int)pReviveQueue->ReviveQueue.size();

    if (pReviveQueue->bRandom)
    {
        nIndex = g_Random(nSize);
    }

    if (nIndex)
    {
        KNpc* pTempNpc = pReviveQueue->ReviveQueue[nIndex].pNpc;
        pReviveQueue->ReviveQueue[nIndex].pNpc = pReviveQueue->ReviveQueue[0].pNpc;
        pReviveQueue->ReviveQueue[0].pNpc = pTempNpc;
        pTempNpc = NULL;
    }

#ifdef _SERVER
    // Revive the first NPC.
    pNpc = pReviveQueue->ReviveQueue[0].pNpc;
    pReviveQueue->ReviveQueue.pop_front();

    bRetCode = m_pScene->m_ProgressData.GetBit(pNpc->m_nProgressID, &bProgressFlag);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (bProgressFlag)
    {
        g_pSO3World->DeleteNpc(pNpc);
        goto Exit0;
    }

    bRetCode = pNpc->AdjustLevel();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pNpc->GetRevivePoint(m_pScene, &nX, &nY, &nZ, &nDir);
    KGLOG_PROCESS_ERROR(bRetCode);

    pNpc->m_nFaceDirection = nDir;
    pNpc->m_nFaceDirection %= DIRECTION_COUNT;
    if (pNpc->m_nFaceDirection < 0)
    {
        pNpc->m_nFaceDirection += DIRECTION_COUNT;
    }

    bRetCode = g_pSO3World->AddNpc(pNpc, m_pScene, nX, nY, nZ);
    if (bRetCode)
    {
        assert(pNpc->m_pCell);

        pNpc->Revive();

        bRetCode = pNpc->m_AIVM.Setup(pNpc, pNpc->m_AIData.nAIType);
        if (!bRetCode)
            KGLogPrintf(KGLOG_ERR, "[NPC] Setup Npc AI failed, Npc template = %lu, AIType = %d", pNpc->m_dwTemplateID, pNpc->m_AIData.nAIType);

        assert(pNpc->m_pRegion);
        pNpc->m_pRegion->SetActive();
    }
    else
    {
        KGLogPrintf(KGLOG_ERR, "Failed to revive NPC: (%s, %u)\n", pNpc->m_szName, pNpc->m_dwTemplateID);
        g_pSO3World->DeleteNpc(pNpc);
    }

    if (pNpc->m_nNpcTeamID)
    {
        bRetCode = m_pScene->m_ThreatLink.SyncAllThreat(pNpc->m_nNpcTeamID, pNpc);
        KGLOG_CHECK_ERROR(bRetCode);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::LoadNpcReviveTable(const char cszFileName[])
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    int                     nTabHeight      = 0;
    ITabFile*               piTable         = NULL;
    DWORD                   dwReviveID      = 0;
    int                     nMinExistCount  = 0;
    int                     nMaxExistCount  = 0;
    int                     nIsRandom       = 0;
    KNPC_TABLE_DATA         NpcTableData;
    KNpcTableMap::iterator  it;

    assert(cszFileName);

    piTable = g_OpenTabFile(cszFileName);
    KG_PROCESS_SUCCESS(piTable == NULL);

    nTabHeight = piTable->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight > 0);
    
    for (int i = 2; i <= nTabHeight; i++)
    {
        bRetCode = piTable->GetInteger(i, "ReviveID", 0, (int*)&dwReviveID);
        KGLOG_PROCESS_ERROR(bRetCode && "Get ReviveID");

        bRetCode = piTable->GetInteger(i, "Min", 0, &nMinExistCount);
        KGLOG_PROCESS_ERROR(bRetCode && "Min");

        bRetCode = piTable->GetInteger(i, "Max", 0, &nMaxExistCount);
        KGLOG_PROCESS_ERROR(bRetCode && "Max");

        bRetCode = piTable->GetInteger(i, "IsRandom", 0, &nIsRandom);
        KGLOG_PROCESS_ERROR(bRetCode && "IsRandom");

        KGLOG_PROCESS_ERROR(nMinExistCount <= nMaxExistCount);

        NpcTableData.nMinSize = nMinExistCount; // 這裡的Min和Max並不是隊列中使用的，還需要根據NPC的數量做一次運算
        NpcTableData.nMaxSize = nMaxExistCount;
        NpcTableData.bRandom  = (nIsRandom != 0);

        it = m_NpcTableMap.find(dwReviveID);
        KGLOG_PROCESS_ERROR(it == m_NpcTableMap.end() && "ReviveID Error");

        m_NpcTableMap.insert(std::make_pair(dwReviveID, NpcTableData));
    }

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piTable);
    return bResult;
}

BOOL KNpcReviveManager::LoadNpcRandomTable(const char cszFileName[], const char cszSceneName[])
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    ITabFile*   piTable                 = NULL;
    DWORD       dwRandomID              = 0;
    int         nTableHeight            = 0;
    char        szFilePath[MAX_PATH];
    char        szSubFileName[MAX_PATH];

    piTable = g_OpenTabFile(cszFileName);
    KG_PROCESS_SUCCESS(piTable == NULL);

    nTableHeight = piTable->GetHeight();
    KGLOG_PROCESS_ERROR(nTableHeight > 0);

    for (int i = 2; i <= nTableHeight; i++)
    {
        bRetCode = piTable->GetInteger(i, "RandomID", 0, (int*)&dwRandomID);
        KGLOG_PROCESS_ERROR(bRetCode && "NpcRandomTable RandomID");

        bRetCode = piTable->GetString(i, "FileName", "", szSubFileName, sizeof(szSubFileName));
        KGLOG_PROCESS_ERROR(bRetCode && "NpcRandomTable FileName");

        snprintf(
            szFilePath, sizeof(szFilePath), 
            "%s/map/%s/NpcRelive/%s",
            SETTING_DIR, cszSceneName, szSubFileName
        );
        szFilePath[sizeof(szFilePath) - 1] = '\0';

        bRetCode = LoadNpcRandomTableSub(dwRandomID, szFilePath);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piTable);
    return bResult;
}

BOOL KNpcReviveManager::LoadNpcRandomTableSub(DWORD dwRandomID, const char cszFileName[])
{
    BOOL                        bResult                 = false;
    BOOL                        bRetCode                = false;
    int                         nTableHeight            = 0;
    ITabFile*                   piTable                 = NULL;
    KNpcRandomDataVector*       pNpcRandomDataVector    = NULL;
    KNpcRandomDataVector        NpcRandomDataVector;
    KNpcRandomDataMap::iterator it;
    KNPC_RANDOM_DATA            NpcRandomData;

    assert(cszFileName);

    it = m_NpcRandomDataMap.find(dwRandomID);
    if (it == m_NpcRandomDataMap.end())
    {
        m_NpcRandomDataMap.insert(std::make_pair(dwRandomID, NpcRandomDataVector));

        it = m_NpcRandomDataMap.find(dwRandomID);
    }

    pNpcRandomDataVector = &(it->second);

    piTable = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTable);

    nTableHeight = piTable->GetHeight();
    KGLOG_PROCESS_ERROR(nTableHeight > 0);

    for (int i = 2; i <= nTableHeight; i++)
    {
        bRetCode = piTable->GetInteger(i, "NpcTemplateID", 0, (int*)&(NpcRandomData.dwNpcTemplateID));
        KGLOG_PROCESS_ERROR(bRetCode && "NpcRandomTableSub NpcTemplateID");

        pNpcRandomDataVector->push_back(NpcRandomData);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTable);
    return bResult;
}

BOOL KNpcReviveManager::CalculateReviveLimit()
{
    BOOL                bResult         = false;
    KNPC_REVIVE_QUEUE*  pNpcReviveQueue = NULL;
    int                 nMaxQueueSize   = 0;
    int                 nMinQueueSize   = 0;
    int                 nTableMinSize   = 0;
    int                 nTableMaxSize   = 0;
    BOOL                bTableRandom    = false;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        pNpcReviveQueue = &(it->second);
        assert(pNpcReviveQueue);

        nTableMinSize = m_NpcTableMap[it->first].nMinSize;
        nTableMaxSize = m_NpcTableMap[it->first].nMaxSize;
        bTableRandom  = m_NpcTableMap[it->first].bRandom;

        nMaxQueueSize = max((int)pNpcReviveQueue->ReviveQueue.size() - nTableMinSize, 0);
        nMinQueueSize = max((int)pNpcReviveQueue->ReviveQueue.size() - nTableMaxSize, 0);

        assert(nMinQueueSize <= nMaxQueueSize);

        pNpcReviveQueue->nMaxQueueSize = nMaxQueueSize;
        pNpcReviveQueue->nMinQueueSize = nMinQueueSize;
        pNpcReviveQueue->bRandom       = bTableRandom;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::DisorganizeReviveList()
{
    BOOL                bResult         = false;
    int                 nQueueSize      = 0;
    int                 nRand           = 0;
    KNPC_REVIVE_QUEUE*  pNpcReviveQueue = NULL;
    KNPC_REVIVE_NODE    TempReviveNode;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        pNpcReviveQueue = &(it->second);
        assert(pNpcReviveQueue);

        nQueueSize = (int)pNpcReviveQueue->ReviveQueue.size();

        for (int i = 0; i < nQueueSize; i++)
        {
            nRand = (int)g_Random(nQueueSize - i);

            TempReviveNode = pNpcReviveQueue->ReviveQueue[i];
            pNpcReviveQueue->ReviveQueue[i] = pNpcReviveQueue->ReviveQueue[i + nRand];
            pNpcReviveQueue->ReviveQueue[i + nRand] = TempReviveNode;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager::CreateRandomNpc()
{
    BOOL                        bResult                 = false;
    BOOL                        bRetCode                = false;
    KNpcVector*                 pNpcVector              = NULL;
    KNpcRandomDataVector*       pNpcRandomDataVector    = NULL;
    DWORD                       dwRandomID              = 0;
    int                         nNpcCount               = 0;
    int                         nRandomDataCount        = 0;
    int                         nX                      = 0;
    int                         nY                      = 0;
    int                         nZ                      = 0;
    int                         nDir                    = 0;
    KNpcRandomKey               NpcRandomKey;
    KNpcRandomDataMap::iterator NpcRandomDataIt;
    KNPC_RANDOM_NODE            NpcRandomNode;
    
    for (KNpcRandomMap::iterator it = m_NpcRandomMap.begin(); it != m_NpcRandomMap.end(); ++it)
    {
        NpcRandomKey = it->first;
        pNpcVector   = &(it->second);
        dwRandomID   = NpcRandomKey.second;
        nNpcCount    = (int)pNpcVector->size();

        NpcRandomDataIt = m_NpcRandomDataMap.find(dwRandomID);
        KGLOG_PROCESS_ERROR(NpcRandomDataIt != m_NpcRandomDataMap.end());

        pNpcRandomDataVector = &(NpcRandomDataIt->second);
        assert(pNpcRandomDataVector);

        nRandomDataCount = (int)pNpcRandomDataVector->size();
        KGLOG_PROCESS_ERROR(nRandomDataCount > 0);

        for (int i = 0; i < nNpcCount; i++)
        {
            int                 nIndex  = i % nRandomDataCount;
            int                 nRandom = g_Random(nRandomDataCount - nIndex);
            KNpc*               pNpc    = NULL;
            KNPC_RANDOM_DATA    NpcRandomData;
            KNPC_RANDOM_DATA    TempRandomData;

            TempRandomData  = (*pNpcRandomDataVector)[nIndex];
            (*pNpcRandomDataVector)[nIndex] = (*pNpcRandomDataVector)[nIndex + nRandom];
            (*pNpcRandomDataVector)[nIndex + nRandom] = TempRandomData;

            NpcRandomNode = (*pNpcVector)[i];
            pNpc          = NpcRandomNode.pNpc;
            NpcRandomData = (*pNpcRandomDataVector)[nIndex];
            assert(pNpc);

            NpcRandomNode.NpcData.dwTemplateID = NpcRandomData.dwNpcTemplateID;

            bRetCode = pNpc->Load(NpcRandomNode.NpcData);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = AddNpc(pNpc, 0);
            KGLOG_PROCESS_ERROR(bRetCode);
            pNpc = NULL;
        }

        pNpcRandomDataVector = NULL;
    }

    bResult = true;
Exit0:
    return bResult;
}
