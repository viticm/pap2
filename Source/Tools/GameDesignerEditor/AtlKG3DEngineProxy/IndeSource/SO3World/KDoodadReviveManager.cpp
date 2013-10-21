#include "stdafx.h"

#include "KDoodad.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KWorldSettings.h"
#include "KScene.h"
#include "KDoodadReviveManager.h"

KDoodadReviveManager::KDoodadReviveManager()
{
    m_pScene = NULL;
}

KDoodadReviveManager::~KDoodadReviveManager()
{
}

BOOL KDoodadReviveManager::Init(KScene* pScene)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    assert(pScene);

    m_pScene = pScene;

    m_DoodadReviveMap.clear();

    m_DoodadTableMap.clear();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::UnInit()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    for (KDoodadReviveMap::iterator it = m_DoodadReviveMap.begin(); it != m_DoodadReviveMap.end(); ++it)
    {
        KDOODAD_REVIVE_QUEUE* pDoodadReviveQueue = &(it->second);
        assert(pDoodadReviveQueue);

        for (
            std::deque<KDOODAD_REVIVE_NODE>::iterator Queueit = pDoodadReviveQueue->ReviveQueue.begin(); 
            Queueit != pDoodadReviveQueue->ReviveQueue.end(); ++Queueit
        )
        {
            KDoodad* pDoodad = Queueit->pDoodad;
            if (pDoodad)
            {
                g_pSO3World->DelDoodad(pDoodad);
                pDoodad = NULL;
            }
        }
    }

    m_DoodadReviveMap.clear();

    m_DoodadTableMap.clear();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::Load(const char cszSceneName[])
{
    BOOL bResult                = false;
    BOOL bRetCode               = false;
    char szFilePath[MAX_PATH];

    KGLOG_PROCESS_ERROR(cszSceneName);
    KGLOG_PROCESS_ERROR(m_pScene);

    snprintf(
        szFilePath, sizeof(szFilePath), 
        "%s/map/%s/DoodadRelive/DoodadReviveList.tab",
        SETTING_DIR, cszSceneName
    );
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    bRetCode = LoadDoodadReviveTable(szFilePath);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KDoodadReviveManager::Activate()
{
    BOOL bRetCode       = false;
    int  nReviveCount   = 0;

    for (KDoodadReviveMap::iterator it = m_DoodadReviveMap.begin(); it != m_DoodadReviveMap.end(); ++it)
    {
        KDOODAD_REVIVE_QUEUE* pDoodadReviveQueue = &(it->second);

        while (true)
        {
            if (nReviveCount > MAX_REVIVE_DOODAD_PER_FRAME)
            {
                break;
            }

            if ((int)pDoodadReviveQueue->ReviveQueue.size() <= pDoodadReviveQueue->nMinQueueSize)
                break;

            if (pDoodadReviveQueue->ReviveQueue.empty())
                break;

            if (pDoodadReviveQueue->ReviveQueue.front().nFrame > g_pSO3World->m_nGameLoop)
                break;

            ReviveDoodad(it->first);

            nReviveCount++;
        }
    }
}

BOOL ReviveNodeFrameComp(const KDOODAD_REVIVE_NODE& crLeft, const KDOODAD_REVIVE_NODE& crRight)
{
    return crLeft.nFrame < crRight.nFrame;
}

BOOL KDoodadReviveManager::AddDoodad(KDoodad* pDoodad, int nReviveTime)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwReviveID  = 0;
    
    std::deque<KDOODAD_REVIVE_NODE>::iterator   it;
    KDOODAD_REVIVE_NODE                         ReviveNode;

    KGLOG_PROCESS_ERROR(pDoodad);

    dwReviveID = pDoodad->m_dwReviveID;

    ReviveNode.pDoodad  = pDoodad;
    ReviveNode.nFrame   = g_pSO3World->m_nGameLoop + nReviveTime * GAME_FPS;

    it = std::upper_bound(
        m_DoodadReviveMap[dwReviveID].ReviveQueue.begin(), 
        m_DoodadReviveMap[dwReviveID].ReviveQueue.end(), 
        ReviveNode, ReviveNodeFrameComp
    );
    m_DoodadReviveMap[dwReviveID].ReviveQueue.insert(it, ReviveNode);
    pDoodad = NULL;

    if (m_DoodadReviveMap[dwReviveID].nMaxQueueSize < (int)m_DoodadReviveMap[dwReviveID].ReviveQueue.size())
    {
        ReviveDoodad(dwReviveID);
    }

    bResult = true;
Exit0:
    if (pDoodad)
    {
        bRetCode = g_pSO3World->DelDoodad(pDoodad);
        KGLOG_CHECK_ERROR(bRetCode);
        pDoodad = NULL;
    }
    return bResult;
}

BOOL KDoodadReviveManager::ReviveDoodad(DWORD dwReviveID)
{
    BOOL     bResult    = false;
    BOOL     bRetCode   = false;
    int      nSize      = 0;
    int      nIndex     = 0;
    int      nX         = 0;
    int      nY         = 0;
    int      nZ         = 0;
    KDoodad* pDoodad    = NULL;

    KDOODAD_REVIVE_QUEUE* pDoodadReviveQueue = NULL;

    pDoodadReviveQueue = &(m_DoodadReviveMap[dwReviveID]);
    assert(pDoodadReviveQueue);

    nSize = (int)pDoodadReviveQueue->ReviveQueue.size();

    if (pDoodadReviveQueue->bRandom)
    {
        nIndex = g_Random(nSize);
    }

    if (nIndex)
    {
        KDoodad* pTempDoodad = pDoodadReviveQueue->ReviveQueue[0].pDoodad;
        pDoodadReviveQueue->ReviveQueue[0].pDoodad = pDoodadReviveQueue->ReviveQueue[nIndex].pDoodad;
        pDoodadReviveQueue->ReviveQueue[nIndex].pDoodad = pTempDoodad;
    }

#ifdef _SERVER
    pDoodad = pDoodadReviveQueue->ReviveQueue[0].pDoodad;
    pDoodadReviveQueue->ReviveQueue.pop_front();

    assert(pDoodad);

    nX = pDoodad->m_nX;
    nY = pDoodad->m_nY;
    nZ = pDoodad->m_nZ;

    bRetCode = g_pSO3World->AddDoodad(pDoodad, m_pScene, nX, nY, nZ);
    if (bRetCode)
    {
        pDoodad->Revive();
    }
    else
    {
        KGLogPrintf(KGLOG_ERR, "Failed to revive Doodad: (%s, %u)\n", pDoodad->m_szName, pDoodad->m_dwTemplateID);
        g_pSO3World->DelDoodad(pDoodad);
    }

#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::LoadDoodadReviveTable(const char cszFileName[])
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    ITabFile*   piTable         = NULL;
    DWORD       dwReviveID      = 0;
    int         nTabHeight      = 0;
    int         nMinExistCount  = 0;
    int         nMaxExistCount  = 0;
    int         nIsRandom       = 0;

    KDOODAD_TABLE_DATA          TableData;
    KDoodadTableMap::iterator   it;

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

        TableData.nMinSize = nMinExistCount;
        TableData.nMaxSize = nMaxExistCount;
        TableData.bRandom  = (nIsRandom != 0);
        TableData.bRandom  = true;  // 场景编辑器中没有地方设置，暂时默认为true

        it = m_DoodadTableMap.find(dwReviveID);
        KGLOG_PROCESS_ERROR(it == m_DoodadTableMap.end() && "Revive Error");

        m_DoodadTableMap.insert(std::make_pair(dwReviveID, TableData));
    }

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piTable);
    return bResult;
}

BOOL KDoodadReviveManager::CalculateReviveLimit()
{
    BOOL                    bResult             = false;
    KDOODAD_REVIVE_QUEUE*   pDoodadReviveQueue  = NULL;
    int                     nMaxQueueSize       = 0;
    int                     nMinQueueSize       = 0;
    int                     nTableMaxSize       = 0;
    int                     nTableMinSize       = 0;
    BOOL                    bTableRandom        = false;

    for (KDoodadReviveMap::iterator it = m_DoodadReviveMap.begin(); it != m_DoodadReviveMap.end(); ++it)
    {
        pDoodadReviveQueue = &(it->second);
        assert(pDoodadReviveQueue);

        nTableMinSize = m_DoodadTableMap[it->first].nMinSize;
        nTableMaxSize = m_DoodadTableMap[it->first].nMaxSize;
        bTableRandom  = m_DoodadTableMap[it->first].bRandom;

        nMaxQueueSize = max((int)pDoodadReviveQueue->ReviveQueue.size() - nTableMinSize, 0);
        nMinQueueSize = max((int)pDoodadReviveQueue->ReviveQueue.size() - nTableMaxSize, 0);

        assert(nMinQueueSize <= nMaxQueueSize);

        pDoodadReviveQueue->nMaxQueueSize = nMaxQueueSize;
        pDoodadReviveQueue->nMinQueueSize = nMinQueueSize;
        pDoodadReviveQueue->bRandom       = bTableRandom;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::DisorganizeReviveList()
{
    BOOL                    bResult             = false;
    int                     nQueueSize          = 0;
    int                     nRand               = 0;
    KDOODAD_REVIVE_QUEUE*   pDoodadReviveQueue  = NULL;
    KDOODAD_REVIVE_NODE     TempReviveNode;

    for (KDoodadReviveMap::iterator it = m_DoodadReviveMap.begin(); it != m_DoodadReviveMap.end(); ++it)
    {
        pDoodadReviveQueue = &(it->second);
        assert(pDoodadReviveQueue);

        nQueueSize = (int)pDoodadReviveQueue->ReviveQueue.size();

        for (int i = 0; i < nQueueSize; i++)
        {
            nRand = (int)g_Random(nQueueSize - i);

            TempReviveNode = pDoodadReviveQueue->ReviveQueue[i];
            pDoodadReviveQueue->ReviveQueue[i] = pDoodadReviveQueue->ReviveQueue[i + nRand];
            pDoodadReviveQueue->ReviveQueue[i + nRand] = TempReviveNode;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::CompleteLoading()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    assert(m_pScene);

    bRetCode = CalculateReviveLimit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = DisorganizeReviveList();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::DestroyDoodad(KDoodad* pDoodad)
{
    BOOL                                        bResult             = false;
    KDOODAD_REVIVE_QUEUE*                       pDoodadReviveQue    = NULL;
    KDoodadReviveMap::iterator                  Findit;
    std::deque<KDOODAD_REVIVE_NODE>::iterator   it;
    std::deque<KDOODAD_REVIVE_NODE>::iterator   itEnd;

    assert(pDoodad);

    Findit = m_DoodadReviveMap.find(pDoodad->m_dwReviveID);
    KGLOG_PROCESS_ERROR(Findit != m_DoodadReviveMap.end());

    pDoodadReviveQue = &(Findit->second);
    KGLOG_PROCESS_ERROR(pDoodadReviveQue);

    itEnd = pDoodadReviveQue->ReviveQueue.end();
    for (it = pDoodadReviveQue->ReviveQueue.begin(); it != itEnd; ++it)
    {
        KDOODAD_REVIVE_NODE* pDoodadReviveNode = &(*it);

        if (pDoodadReviveNode->pDoodad->m_dwID != pDoodad->m_dwID)
            continue;

        pDoodadReviveQue->ReviveQueue.erase(it);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodadReviveManager::ForceReviveDoodad(KDoodad* pDoodad)
{
    BOOL                                        bResult             = false;
    BOOL                                        bRetCode            = false;
    BOOL                                        bOldRandom          = false;
    KDOODAD_REVIVE_QUEUE*                       pDoodadReviveQue    = NULL;
    KDOODAD_REVIVE_NODE*                        pDoodadReviveNode   = NULL;
    KDoodad*                                    pTempDoodad         = NULL;
    KDoodadReviveMap::iterator                  Findit;
    std::deque<KDOODAD_REVIVE_NODE>::iterator   it;
    std::deque<KDOODAD_REVIVE_NODE>::iterator   itEnd;

    assert(pDoodad);

    Findit = m_DoodadReviveMap.find(pDoodad->m_dwReviveID);
    KGLOG_PROCESS_ERROR(Findit != m_DoodadReviveMap.end());

    pDoodadReviveQue = &(Findit->second);
    KGLOG_PROCESS_ERROR(pDoodadReviveQue);

    itEnd = pDoodadReviveQue->ReviveQueue.end();
    for (it = pDoodadReviveQue->ReviveQueue.begin(); it != itEnd; ++it)
    {
        pDoodadReviveNode = &(*it);

        if (pDoodadReviveNode->pDoodad->m_dwID == pDoodad->m_dwID)
            break;
    }

    KGLOG_PROCESS_ERROR(it != itEnd);

    pTempDoodad                              = pDoodadReviveQue->ReviveQueue[0].pDoodad;
    pDoodadReviveQue->ReviveQueue[0].pDoodad = pDoodadReviveNode->pDoodad;
    pDoodadReviveNode->pDoodad               = pTempDoodad;

    bOldRandom = pDoodadReviveQue->bRandom;
    pDoodadReviveQue->bRandom = false;

    bRetCode = ReviveDoodad(pDoodad->m_dwReviveID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pDoodadReviveQue->bRandom = bOldRandom;

    bResult = true;
Exit0:
    return bResult;
}
