#include "StdAfx.h"
#include "engine\kglog.h"
#include "TestShortPath.h"
#include <iostream>
using namespace std;

const int DEFAULT_RETURN_COST = 100;

TestShortPath::TestShortPath()
{
    m_OpenNodeVector.clear();
}

TestShortPath::~TestShortPath()
{
    m_OpenNodeVector.clear();
}

BOOL TestShortPath::ShortestPath(
                                int *pnRetMiniCost, int nFromNodeID, int nTargetCity,  int nExcludeNodeID, 
                                const KOPEN_ROUTE_NODE_VECTOER* cpOpenNodeList, KReputation* pReputation
)
{
    BOOL                                                        bResult         = false;
    BOOL                                                        bRetCode        = false;
    int                                                         nCurrentNodeID  = 0;
    int                                                         nMinCost        = INT_MAX;
    int                                                         nReputationLvl  = NEUTRAL_REPUTE_LEVEL;
    KROUTE_NODE*                                                pCurrentNode    = NULL;
    KROUTE_NODE*                                                pLinkNode       = NULL;
    KTRACK*                                                     pTrack          = NULL;
    KCLOSE_TABLE                                                CloseTab;
    KCLOSE_TABLE::iterator                                      itCloseTab;
    KOPEN_QUEUE                                                 OpenQue;
    pair<KCLOSE_TABLE::iterator, bool>                          itRetCode;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    it;
    std::vector<std::pair<KROUTE_NODE*, KTRACK*> >::iterator    itEnd;

    pCurrentNode = GetNodeInfo(nFromNodeID);
    KGLOG_PROCESS_ERROR(pCurrentNode);

    *pnRetMiniCost = INT_MAX;

    if (cpOpenNodeList && pCurrentNode->bIsNeedOpen) // 起点都没有开
    {
        bRetCode = IsOpenRouteNode(nFromNodeID, cpOpenNodeList);
        KG_PROCESS_SUCCESS(!bRetCode);
    }

    if (pReputation) // 起点的势力不符合
    {
        nReputationLvl = pReputation->GetReputeLevel(pCurrentNode->dwForceID);
        KG_PROCESS_SUCCESS(nReputationLvl < NEUTRAL_REPUTE_LEVEL);
    }

    KG_PROCESS_SUCCESS(nFromNodeID == nExcludeNodeID); // 起点就不可走

    *pnRetMiniCost = 0;

    // 如果该点就是目标城市点，则无需巡路，开销为0
    KG_PROCESS_SUCCESS(pCurrentNode->dwCityID == (DWORD)nTargetCity);

    OpenQue.push_back(nFromNodeID);
    itRetCode = CloseTab.insert(make_pair(nFromNodeID, 0));
    KGLOG_PROCESS_ERROR(itRetCode.second);

    while (true)
    {
        if (OpenQue.empty())
        {
            break;
        }

        nCurrentNodeID = OpenQue.front();
        OpenQue.pop_front();

        pCurrentNode = GetNodeInfo(nCurrentNodeID);
        KGLOG_PROCESS_ERROR(pCurrentNode);

        itEnd = pCurrentNode->vLinkNode.end();
        for (it = pCurrentNode->vLinkNode.begin(); it != itEnd; ++it)
        {
            int nCost = 0;
            pLinkNode   = it->first;
            pTrack      = it->second;

            if (pLinkNode->nNodeID == nExcludeNodeID)
            {
                continue;
            }

            if (cpOpenNodeList && pLinkNode->bIsNeedOpen)
            {
                bRetCode = IsOpenRouteNode(pLinkNode->nNodeID, cpOpenNodeList);
                if (!bRetCode)
                {
                    continue;
                }
            }

            if (pReputation)
            {
                nReputationLvl = pReputation->GetReputeLevel(pLinkNode->dwForceID);
                if (nReputationLvl < NEUTRAL_REPUTE_LEVEL)
                {
                    continue;
                }
            }

            itCloseTab = CloseTab.find(pCurrentNode->nNodeID);
            KGLOG_PROCESS_ERROR(itCloseTab != CloseTab.end());
            nCost = itCloseTab->second;

            nCost += pTrack->nCostMoney;

            itCloseTab = CloseTab.find(pLinkNode->nNodeID);
            if (itCloseTab == CloseTab.end())
            {
                itRetCode = CloseTab.insert(make_pair(pLinkNode->nNodeID, nCost));
                KGLOG_PROCESS_ERROR(itRetCode.second);

                itCloseTab = itRetCode.first;
                OpenQue.push_back(pLinkNode->nNodeID);
            }

            if (itCloseTab->second > nCost)
            {
                itCloseTab->second = nCost;
                OpenQue.push_back(pLinkNode->nNodeID);
            }

            if ((int)pLinkNode->dwCityID != nTargetCity)
            {
                continue;
            }

            if (nMinCost > nCost)
            {
                nMinCost = nCost;
            }
        }
    }

    *pnRetMiniCost = nMinCost;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL TestShortPath::Test(
                         int nFromNodeID, int nTargetCity,  int nExcludeNodeID, 
                         int nOpenNodeArray[], int nOpenNodeCount, KReputation* pReputation,
                         int nDesireRetValue, int nDesireRetCost
)
{
    int nResult        = false;
    int nRetCode       = false;
    int nInit          = false;
    int nRetMiniCost   = DEFAULT_RETURN_COST;

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);
    nInit = true;

    nRetCode = AddOpenNode(nOpenNodeArray, nOpenNodeCount);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = ShortestPath(&nRetMiniCost, nFromNodeID, nTargetCity, nExcludeNodeID, &m_OpenNodeVector, pReputation);
    KG_PROCESS_ERROR(nRetCode == nDesireRetValue); 

    if (nRetMiniCost != nDesireRetCost)
    {
        cout << "[Failed] FromNode:" << nFromNodeID << "  TargetCity:" << nTargetCity << endl;
        KG_PROCESS_ERROR(false);
    }

    nResult = true;
Exit0:
    ClearOpenNodeList();

    if (nInit)
    {
        UnInit();
        nInit = false;
    }
    return nResult;
}

BOOL TestShortPath::AddOpenNode(int nOpenNodeArray[], const int cnNodeCount)
{  
    for (int i = 0; i < cnNodeCount; i++)
    {
        m_OpenNodeVector.push_back(nOpenNodeArray[i]);
    }

    return true;
}

void TestShortPath::ClearOpenNodeList()
{
    m_OpenNodeVector.clear();
}

BOOL TestShortPath::TestAll()
{
    int                       nResult         = false;
    int                       nRetCode        = false;
    int                       nFromNodeID     = 0;
    int                       nTargetCity     = 0;
    int                       nExcludeNodeID  = 0;
    int                       nDesireRetCost  = 0;
    int                       nDesireRetValue = 0;
    int                       nOpenNodeCount = 0;
    KReputation*              pReputation     = NULL;
    KOPEN_ROUTE_NODE_VECTOER* cpOpenNodeList  = NULL;
    int nAllTrackNode[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int nOpenNodeArray[MAX_TRACK_ID];

    // TestCase1: 当前路径点不存在
    //--------------------------------------------------------------------------
    nFromNodeID       = 20;
    nTargetCity       = 1;
    nExcludeNodeID    = 0;
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = false;
    nDesireRetCost    = DEFAULT_RETURN_COST;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase1 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase2: 目标城市不存在
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 100;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase2 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase3: 搜索排除点是当前路径点
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 1;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase3 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase4: 搜素排除点是目标城市的路径点
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 2;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 15;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase4 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase5: 搜素排除点是目标城市的路径点，且目标城市中只有这个路径点
    //--------------------------------------------------------------------------
    nFromNodeID       = 3;
    nTargetCity       = 4;
    nExcludeNodeID    = 4;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase5 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase6: 无搜索排除点
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 10;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase6 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase7: 当前路径点不在玩家的打开列表中
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeArray[0] = 0;
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase7 failed!" << endl;
    } 
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase8: 目标城市有多个路径点，都未开通	
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeArray[1] = 0;
    nOpenNodeArray[6] = 0;
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase8 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase9: 目标城市有多个路径点，都开通
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 10;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase9 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase10: 目标城市有多个路径点，部分开通	
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 2;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeArray[1] = 0;
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 15;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase10 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase11: 玩家开通列表中的一个点不存在
    //--------------------------------------------------------------------------
    nFromNodeID       = 3;
    nTargetCity       = 4;
    nExcludeNodeID    = 1;
    nOpenNodeArray[0] = 20;
    memcpy(&nOpenNodeArray[1], nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 5;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase11 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase12: 当前路径点属于目标城市
    //--------------------------------------------------------------------------
    nFromNodeID       = 2;
    nTargetCity       = 2;
    nExcludeNodeID    = 1;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 0;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase12 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase13: 当前路径点和目标城市不在同一个主干道上
    //--------------------------------------------------------------------------
    nFromNodeID       = 6;
    nTargetCity       = 8;
    nExcludeNodeID    = 5;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase13 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase14: 当前路径点和目标城市相邻
    //--------------------------------------------------------------------------
    nFromNodeID       = 3;
    nTargetCity       = 4;
    nExcludeNodeID    = 1;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 5;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase14 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase15: 当前路径点和目标城市之间有3条道路，且总费用都不相同	
    //--------------------------------------------------------------------------
    nFromNodeID       = 1;
    nTargetCity       = 3;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 10;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase15 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase16: 当前路径点和目标城市之间有3条道路，其中两条费用相同	
    //--------------------------------------------------------------------------
    nFromNodeID       = 2;
    nTargetCity       = 4;
    nExcludeNodeID    = 1;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 25;

    // 找到的是哪条路呢？
    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase16 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase17: 当前路径点和目标城市之间有2条道路，费用少的1条走的路径点多
    //--------------------------------------------------------------------------
    nFromNodeID       = 4;
    nTargetCity       = 6;
    nExcludeNodeID    = 3;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 10;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase17 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase18: 当前路径点和目标城市之间有1条道路，但途径路径点有一个是敌对势力	
    //--------------------------------------------------------------------------
    nFromNodeID       = 8;
    nTargetCity       = 10;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    pReputation       = new KReputation;
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase18 failed!" << endl;
    }
    delete pReputation;
    pReputation = NULL;
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase19: 玩家当前所在路径点为敌对势力的路径点
    //--------------------------------------------------------------------------
    nFromNodeID       = 9;
    nTargetCity       = 10;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    pReputation       = new KReputation;
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase19 failed!" << endl;
    }
    delete pReputation;
    pReputation = NULL;
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase20: 玩家目标城市为敌对势力
    //--------------------------------------------------------------------------
    nFromNodeID       = 8;
    nTargetCity       = 9;
    nExcludeNodeID    = 0;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    pReputation       = new KReputation;
    nDesireRetValue   = true;
    nDesireRetCost    = INT_MAX;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase20 failed!" << endl;
    }
    delete pReputation;
    pReputation = NULL;
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase21: 当前路径点和目标城市相邻，但不在同一张地图
    //--------------------------------------------------------------------------
    nFromNodeID       = 6;
    nTargetCity       = 11;
    nExcludeNodeID    = 5;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 20;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase21 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));

    // TestCase22: 目标路径点未开通，但不需要开通	
    //--------------------------------------------------------------------------
    nFromNodeID       = 3;
    nTargetCity       = 4;
    nExcludeNodeID    = 1;
    memcpy(nOpenNodeArray, nAllTrackNode, sizeof(nAllTrackNode));
    nOpenNodeArray[3] = 0;
    nOpenNodeCount    = sizeof(nOpenNodeArray) / sizeof(nOpenNodeArray[0]);
    nDesireRetValue   = true;
    nDesireRetCost    = 5;

    nRetCode = Test(nFromNodeID, nTargetCity, nExcludeNodeID, nOpenNodeArray, nOpenNodeCount, pReputation, nDesireRetValue, nDesireRetCost);
    if (!nRetCode)
    {
        cout << "TestCase22 failed!" << endl;
    }
    memset(nOpenNodeArray, 0, sizeof(nOpenNodeArray));
    cout << "Test finish!" << endl;

    nResult = true;
Exit0:
    return nResult;
}