#include "stdafx.h"

#include "KNpc.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KWorldSettings.h"
#include "KScene.h"
#include "KNpcReviveManager1.h"

KNpcReviveManager1::KNpcReviveManager1()
{
    m_pScene = NULL;
}

KNpcReviveManager1::~KNpcReviveManager1()
{
}

BOOL KNpcReviveManager1::Init(KScene* pScene)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KG_PROCESS_ERROR(pScene);
    m_pScene = pScene;

    m_NpcReviveMap.clear();

    m_NpcRandomMap.clear();

    m_NpcRandomDataMap.clear();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager1::UnInit()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    m_pScene = NULL;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        KNPC_REVIVE_QUEUE*  pNpcReviveQueue = &(it->second);

        for (std::deque<KNPC_REVIVE_NODE>::iterator QueueIt = pNpcReviveQueue->ReviveQueue.begin(); QueueIt != pNpcReviveQueue->ReviveQueue.end(); ++QueueIt)
        {
            KNpc* pNpc = QueueIt->pNpc;

            if (pNpc)
            {
                bRetCode = g_SO3World.DeleteNpc(pNpc);
                KGLOG_CHECK_ERROR(bRetCode);
                pNpc = NULL;
            }
        }
    }
    m_NpcReviveMap.clear();

    m_NpcRandomMap.clear();

    m_NpcRandomDataMap.clear();

    CheckChange("End", true);

    bResult = true;
Exit0:
    return bResult;
}

BOOL ReviveNodeFrameComp1(const KNPC_REVIVE_NODE& crLeft, const KNPC_REVIVE_NODE& crRight)
{
    return crLeft.nFrame < crRight.nFrame;
}

BOOL KNpcReviveManager1::AddNpc(KNpc* pNpc, int nReviveTime)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwReviveID  = 0;
    std::deque<KNPC_REVIVE_NODE>::iterator it;
    KNPC_REVIVE_NODE ReviveNode;

    KGLOG_PROCESS_ERROR(pNpc);
    dwReviveID = pNpc->m_dwReliveID;

    ReviveNode.pNpc = pNpc;
    ReviveNode.nFrame = g_SO3World.m_nGameLoop + nReviveTime * GAME_FPS;

    it = std::upper_bound(m_NpcReviveMap[dwReviveID].ReviveQueue.begin(), m_NpcReviveMap[dwReviveID].ReviveQueue.end(), ReviveNode, ReviveNodeFrameComp1);
    m_NpcReviveMap[dwReviveID].ReviveQueue.insert(it, ReviveNode);
    pNpc = NULL;
    bRetCode = CheckChange("AddNpc");
    KGLOG_PROCESS_ERROR(bRetCode);

    if (m_NpcReviveMap[dwReviveID].nMaxQueueSize && m_NpcReviveMap[dwReviveID].nMaxQueueSize < (int)m_NpcReviveMap[dwReviveID].ReviveQueue.size())
    {
        ReviveNpc(dwReviveID);
    }

    bResult = true;
Exit0:
    if (pNpc)
    {
        bRetCode = g_SO3World.DeleteNpc(pNpc);
        KGLOG_CHECK_ERROR(bRetCode);
        pNpc = NULL;
    }
    return bResult;
}

BOOL KNpcReviveManager1::LoadNpc(KNpc* pNpc)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    KNpcVector*             pNpcVector  = NULL;
    DWORD                   dwRandomID  = 0;
    KNpcRandomMap::iterator it;
    KNpcVector              NpcVector;
    KNpcRandomKey           NpcRandomKey;


    KGLOG_PROCESS_ERROR(pNpc);

    dwRandomID = pNpc->m_dwRandomID;

    if (dwRandomID == 0)
    {
        AddNpc(pNpc, 0);
        goto Exit1;
    }

    NpcRandomKey = std::make_pair(pNpc->m_dwReliveID, dwRandomID);

    it = m_NpcRandomMap.find(NpcRandomKey);

    if (it == m_NpcRandomMap.end())
    {
        NpcVector.push_back(pNpc);

        m_NpcRandomMap.insert(std::make_pair(NpcRandomKey, NpcVector));

        goto Exit1;
    }

    pNpcVector = &(it->second);
    pNpcVector->push_back(pNpc);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager1::CheckNpcRevive()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        KNPC_REVIVE_QUEUE*  pNpcReviveQueue = &(it->second);
        ASSERT(pNpcReviveQueue);

        while (true)
        {
            if (pNpcReviveQueue->nMinQueueSize && (int)pNpcReviveQueue->ReviveQueue.size() <= pNpcReviveQueue->nMinQueueSize)
                break;

            if (pNpcReviveQueue->ReviveQueue.empty())
                break;

            if (pNpcReviveQueue->ReviveQueue.begin()->nFrame > g_SO3World.m_nGameLoop)
                break;

            bRetCode = ReviveNpc(it->first);
            KGLOG_CHECK_ERROR(bRetCode);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager1::ReviveNpc(DWORD dwReviveID)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nSize       = 0;
    int     nIndex      = 0;
    int     nX          = 0;
    int     nY          = 0;
    int     nZ          = 0;
    int     nDir        = 0;
    KNpc*   pNpc        = NULL;

    KNPC_REVIVE_QUEUE*  pReviveQueue    = NULL;

    pReviveQueue = &(m_NpcReviveMap[dwReviveID]);
    ASSERT(pReviveQueue);

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

    bRetCode = pNpc->AdjustLevel();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pNpc->m_AIController.GetOriginPoint(&nX, &nY, &nZ, &nDir);
    KGLOG_PROCESS_ERROR(bRetCode);

    pNpc->m_nFaceDirection = nDir;
    pNpc->m_nFaceDirection %= DIRECTION_COUNT;
    if (pNpc->m_nFaceDirection < 0)
    {
        pNpc->m_nFaceDirection += DIRECTION_COUNT;
    }

    bRetCode = g_SO3World.AddNpc(pNpc, m_pScene, nX, nY, nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    pNpc->Revive();
    KGLOG_PROCESS_ERROR(pNpc->m_pCell);

    pNpc->m_AIController.Restart();
    pNpc->m_AIController.DoIdle(0);
#endif

    bRetCode = CheckChange("Before ReviveNpc");  
    KGLOG_PROCESS_ERROR(bRetCode);
    pReviveQueue->ReviveQueue.pop_front();
    bRetCode = CheckChange("End ReviveNpc");
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager1::LoadNpcReviveTable(const char *cpszFilePath)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    int                     nTabHeight      = 0;
    ITabFile*               piTable         = NULL;
    DWORD                   dwReviveID      = 0;
    int                     nMinExistCount  = 0;
    int                     nMaxExistCount  = 0;
    int                     nIsRandom       = 0;
    KNPC_REVIVE_QUEUE       ReviveQueue;
    KNpcReviveMap::iterator it;

    KGLOG_PROCESS_ERROR(cpszFilePath);

    piTable = g_OpenTabFile(cpszFilePath);
    KGLOG_PROCESS_ERROR(piTable);

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

        ReviveQueue.nMinQueueSize = nMinExistCount; // 這裡的Min和Max並不是隊列中使用的，還需要根據NPC的數量做一次運算
        ReviveQueue.nMaxQueueSize = nMaxExistCount;
        ReviveQueue.bRandom       = (nIsRandom != 0);

        it = m_NpcReviveMap.find(dwReviveID);
        if (it != m_NpcReviveMap.end())
        {
            it->second.nMinQueueSize = ReviveQueue.nMinQueueSize;
            it->second.nMaxQueueSize = ReviveQueue.nMaxQueueSize;
            it->second.bRandom       = ReviveQueue.bRandom;
            continue;
        }
        m_NpcReviveMap.insert(std::make_pair(dwReviveID, ReviveQueue));
    }

    bResult = true;
Exit0:

    SAFE_RELEASE(piTable);

    return bResult;
}

BOOL KNpcReviveManager1::LoadNpcRandomTable(const char* cpszFilePath)
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    ITabFile*   piTable                 = NULL;
    DWORD       dwRandomID              = 0;
    int         nTableHeight            = 0;
    char        szSubFileName[MAX_PATH];

    szSubFileName[0] = '\0';

    piTable = g_OpenTabFile(cpszFilePath);
    KGLOG_PROCESS_ERROR(piTable);

    nTableHeight = piTable->GetHeight();
    KGLOG_PROCESS_ERROR(nTableHeight > 0);

    for (int i = 2; i <= nTableHeight; i++)
    {
        bRetCode = piTable->GetInteger(i, "RandomID", 0, (int*)&dwRandomID);
        KGLOG_PROCESS_ERROR(bRetCode && "NpcRandomTable RandomID");

        bRetCode = piTable->GetString(i, "FileName", "", szSubFileName, sizeof(szSubFileName));
        KGLOG_PROCESS_ERROR(bRetCode && "NpcRandomTable FileName");

        bRetCode = LoadNpcRandomTableSub(dwRandomID, szSubFileName);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    SAFE_RELEASE(piTable);

    return bResult;
}

BOOL KNpcReviveManager1::LoadNpcRandomTableSub(DWORD dwRandomID, const char* cpszFilePath)
{
    BOOL                        bResult                 = false;
    BOOL                        bRetCode                = false;
    int                         nTableHeight            = 0;
    ITabFile*                   piTable                 = NULL;
    KNpcRandomDataVector*       pNpcRandomDataVector    = NULL;
    KNpcRandomDataVector        NpcRandomDataVector;
    KNpcRandomDataMap::iterator it;
    KNPC_RANDOM_DATA            NpcRandomData;

    KGLOG_PROCESS_ERROR(cpszFilePath);

    it = m_NpcRandomDataMap.find(dwRandomID);
    if (it == m_NpcRandomDataMap.end())
    {
        m_NpcRandomDataMap.insert(std::make_pair(dwRandomID, NpcRandomDataVector));

        it = m_NpcRandomDataMap.find(dwRandomID);
    }

    pNpcRandomDataVector = &(it->second);

    piTable = g_OpenTabFile(cpszFilePath);
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
    SAFE_RELEASE(piTable);

    return bResult;
}

BOOL KNpcReviveManager1::CalculateReviveLimit()
{
    BOOL                bResult         = false;
    KNPC_REVIVE_QUEUE*  pNpcReviveQueue = NULL;
    int                 nMaxQueueSize   = 0;
    int                 nMinQueueSize   = 0;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        pNpcReviveQueue = &(it->second);
        ASSERT(pNpcReviveQueue);

        nMaxQueueSize = max((int)pNpcReviveQueue->ReviveQueue.size() - pNpcReviveQueue->nMinQueueSize, 0);
        nMinQueueSize = max((int)pNpcReviveQueue->ReviveQueue.size() - pNpcReviveQueue->nMaxQueueSize, 0);

        ASSERT(nMinQueueSize <= nMaxQueueSize);

        pNpcReviveQueue->nMaxQueueSize = nMaxQueueSize;
        pNpcReviveQueue->nMinQueueSize = nMinQueueSize;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpcReviveManager1::DisorganizeReviveList()
{
    BOOL                bResult         = false;
    int                 nQueueSize      = 0;
    int                 nRand           = 0;
    KNPC_REVIVE_QUEUE*  pNpcReviveQueue = NULL;
    KNPC_REVIVE_NODE    TempReviveNode;

    for (KNpcReviveMap::iterator it = m_NpcReviveMap.begin(); it != m_NpcReviveMap.end(); ++it)
    {
        pNpcReviveQueue = &(it->second);
        ASSERT(pNpcReviveQueue);

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

BOOL KNpcReviveManager1::CreateRandomNpc()
{
    BOOL                        bResult                 = false;
    BOOL                        bRetCode                = false;
    KNpcVector*                 pNpcVector              = NULL;
    KNpcRandomDataVector*       pNpcRandomDataVector    = NULL;
    DWORD                       dwRandomID              = 0;
    int                         nNpcCount               = 0;
    int                         nRandomDataCount        = 0;
    KNpcRandomKey               NpcRandomKey;
    KNpcRandomDataMap::iterator NpcRandomDataIt;
    
    for (KNpcRandomMap::iterator it = m_NpcRandomMap.begin(); it != m_NpcRandomMap.end(); ++it)
    {
        NpcRandomKey = it->first;
        pNpcVector   = &(it->second);
        dwRandomID   = NpcRandomKey.second;
        nNpcCount    = (int)pNpcVector->size();

        NpcRandomDataIt = m_NpcRandomDataMap.find(dwRandomID);
        KGLOG_PROCESS_ERROR(NpcRandomDataIt != m_NpcRandomDataMap.end());

        pNpcRandomDataVector = &(NpcRandomDataIt->second);
        ASSERT(pNpcRandomDataVector);

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

            pNpc          = (*pNpcVector)[i];
            NpcRandomData = (*pNpcRandomDataVector)[nIndex];

            bRetCode = g_SO3World.m_Settings.m_NpcTemplateList.LoadFromTemplate(NpcRandomData.dwNpcTemplateID, pNpc);
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

BOOL KNpcReviveManager1::LoadNpcComplete()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    char szFilePath[MAX_PATH];

    szFilePath[0] = '\0';

    ASSERT(m_pScene);

    snprintf(szFilePath, sizeof(szFilePath), "%s/map/%s/NpcRelive/MapReviveList.tab", SETTING_DIR, m_pScene->m_szName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    bRetCode = LoadNpcReviveTable(szFilePath);
    KGLOG_CHECK_ERROR(bRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/map/%s/NpcRelive/NpcRandomList.tab", SETTING_DIR, m_pScene->m_szName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    bRetCode = LoadNpcRandomTable(szFilePath);
    KGLOG_CHECK_ERROR(bRetCode);

    bRetCode = CreateRandomNpc();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CalculateReviveLimit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = DisorganizeReviveList();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckChange("LoadNpcComplete");
    KGLOG_PROCESS_ERROR(bRetCode);
    bResult = true;
Exit0:
    return bResult;
}

#ifndef  KG_INFO_LENGTH
#define  KG_INFO_LENGTH (1024 * 1024)
#endif

BOOL KNpcReviveManager1::CheckChange(char szCMDInfo[], int nCloseFlag /*= false*/)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    static FILE *pFile = NULL;
    KNpcReviveMap::iterator iter = NULL;
    KNpcReviveMap::iterator iterEnd = NULL;

    std::deque<KNPC_REVIVE_NODE>::iterator DequeIter;
    std::deque<KNPC_REVIVE_NODE>::iterator DequeIterEnd;

    static char szHeadInfo[KG_INFO_LENGTH];
    static char szInfo[KG_INFO_LENGTH];

    KG_PROCESS_ERROR(szCMDInfo);
    KG_PROCESS_ERROR(!nCloseFlag);
    bRetCode = strcmp(m_pScene->m_szName, "acai");
    KG_PROCESS_SUCCESS(bRetCode);

    if (!pFile)
    {
        pFile = fopen("TestNpcRevive.tab", "a");
        KGLOG_PROCESS_ERROR(pFile);

        bRetCode = fputs("\tFrame\tReviveID\tMinQueueSize\tMaxQueueSize\tRandom\tNpcID\tNpcName\tNpcReviveFrame\t\n", pFile);// 0\tNpcID 1\tNpcName 1\tNpcReviveFrame 1\tNpcID 2\tNpcName 2\tNpcReviveFrame 2\tNpcID 3\tNpcName 3\tNpcReviveFrame 3\tNpcID 4\tNpcName 4\tNpcReviveFrame 4\tNpcID 5\tNpcName 5\tNpcReviveFrame 5\tNpcID 6\tNpcName 6\tNpcReviveFrame 6\tNpcID 7\tNpcName 7\tNpcReviveFrame 7\tNpcID 8\tNpcName 8\tNpcReviveFrame 8\tNpcID 9\tNpcName 9\tNpcReviveFrame 9\t\n", pFile);
        KGLOG_PROCESS_ERROR(EOF != bRetCode);
    }

    fputs(szCMDInfo, pFile);
    KGLOG_PROCESS_ERROR(EOF != bRetCode);


    iterEnd = m_NpcReviveMap.end();
    for (iter = m_NpcReviveMap.begin(); iter != iterEnd; ++iter)
    {        
        snprintf(
            szHeadInfo, sizeof(szHeadInfo), "\t%d\t%u\t%d\t%d\t%d\t",     
            g_SO3World.m_nGameLoop, 
            iter->first, 
            iter->second.nMinQueueSize, 
            iter->second.nMaxQueueSize, 
            iter->second.bRandom
        );
        szHeadInfo[sizeof(szHeadInfo) - 1] = '\0';   

        DequeIterEnd = iter->second.ReviveQueue.end();
        for (DequeIter = iter->second.ReviveQueue.begin(); DequeIter != DequeIterEnd; ++DequeIter)
        {
            fputs(szHeadInfo, pFile);
            KGLOG_PROCESS_ERROR(EOF != bRetCode);

            snprintf(
                szInfo, sizeof(szInfo), "%d\t%s\t%d\t\n",  
                DequeIter->pNpc->m_dwID, 
                DequeIter->pNpc->m_szName, 
                DequeIter->nFrame
            );
            szInfo[sizeof(szInfo) - 1] = '\0';

            fputs(szInfo, pFile);
            KGLOG_PROCESS_ERROR(EOF != bRetCode);
        }
        fputs("\n", pFile);
        KGLOG_PROCESS_ERROR(EOF != bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    if (!bResult && pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    if (pFile)
    {
        fflush(pFile);
    }
    return bResult;
}


