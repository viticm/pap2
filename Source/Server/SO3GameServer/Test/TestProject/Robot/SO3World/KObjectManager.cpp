#include "stdafx.h"
#include <algorithm>
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
//#include "KBuffList.h"
#include "KRegion.h"
#include "KScene.h"

using namespace std;

KObjectManager g_ObjectManager;

KObjectManager::KObjectManager(void)
{
}

KObjectManager::~KObjectManager(void)
{
}

BOOL KObjectManager::Init(int nWorldIndex)
{
	BOOL	bResult		= false;
	BOOL	bRetCode	= false;
	DWORD	dwCount		= 0;
	float	fSize		= 0.0f; // µ¥Î»M

	printf("\tAlloc logic objects :\n");

	dwCount = (DWORD)g_WorldSettings.m_WorldParam.nSceneCount;
	fSize   = (float)dwCount * sizeof(KScene) / 1024 / 1024;
	bRetCode = m_SceneSet.Init(dwCount, nWorldIndex);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc Scene \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	dwCount = (DWORD)g_WorldSettings.m_WorldParam.nRegionCount;
	fSize   = (float)dwCount * sizeof(KRegion) / 1024 / 1024;
	bRetCode = m_RegionSet.Init(dwCount);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc Region \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	dwCount = (DWORD)g_WorldSettings.m_WorldParam.nCellCount;
	fSize   = (float)dwCount * sizeof(KCell) / 1024 / 1024;
	bRetCode = m_CellSet.Init(dwCount);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc Cell \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);


	//dwCount = (DWORD)g_WorldSettings.m_WorldParam.nPlayerCount;
	//fSize   = (float)dwCount * sizeof(KPlayer) / 1024 / 1024;
	//bRetCode = m_PlayerSet.Init(dwCount, PLAYER_ID_PREFIX);
	//KGLOG_PROCESS_ERROR(bRetCode);
	//printf("\tAlloc Player \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	//dwCount = (DWORD)g_WorldSettings.m_WorldParam.nNpcCount;
	//fSize   = (float)dwCount * sizeof(KNpc) / 1024 / 1024;
	//bRetCode = m_NpcSet.Init(dwCount, nWorldIndex);
	//KGLOG_PROCESS_ERROR(bRetCode);
	//printf("\tAlloc Npc \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	//dwCount = (DWORD)g_WorldSettings.m_WorldParam.nDoodadCount;
	//fSize   = (float)dwCount * sizeof(KDoodad) / 1024 / 1024;
	//bRetCode = m_DoodadSet.Init(dwCount, nWorldIndex);
	//KGLOG_PROCESS_ERROR(bRetCode);
	//printf("\tAlloc Doodad \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	dwCount = (DWORD)g_WorldSettings.m_WorldParam.nItemCount;
	fSize   = (float)dwCount * sizeof(KItem) / 1024 / 1024;
	bRetCode = m_ItemSet.Init(dwCount, nWorldIndex);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc Item \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);


	dwCount = (DWORD)g_WorldSettings.m_WorldParam.nLootListCount;
	fSize   = (float)dwCount * sizeof(KLootList) / 1024 / 1024;
	bRetCode = m_LootListSet.Init(dwCount);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc LootList \t\t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	dwCount = (DWORD)g_WorldSettings.m_WorldParam.nAttributeCount;
	fSize   = (float)dwCount * sizeof(KAttribute) / 1024 / 1024;
	bRetCode = m_AttributeSet.Init(dwCount);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc Attribute \t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);

	dwCount = (DWORD)(g_WorldSettings.m_WorldParam.nThreatNodeCount * 2);
	fSize = (float)dwCount * sizeof(KThreatLiteNode) / 1024 / 1024;
	bRetCode = m_ThreatLiteNodeSet.Init(dwCount);
	KGLOG_PROCESS_ERROR(bRetCode);
	printf("\tAlloc ThreatLiteNode \t ... ... Count=%u,\tSize=%.2fM\n", dwCount, fSize);


	bResult = true;
Exit0:
	return bResult;
}


//BOOL KServerObjAlloctor::Init(int nNpcCount, int nDoodadCount, int nCellCount)
//{
//    BOOL bResult = false;
//    int nRetCode = false;
//
//    ASSERT(m_NpcVector.empty());
//    ASSERT(m_DoodadVector.empty());
//    ASSERT(m_CellVector.empty());
//
//    m_NpcVector.reserve(nNpcCount);
//    for (int i = 0; i < nNpcCount; i++)
//    {
//        KNpc* pNpc = g_ObjectManager.m_NpcSet.New();
//        KGLOG_PROCESS_ERROR(pNpc);
//        m_NpcVector.push_back(pNpc);
//    }
//
//    m_DoodadVector.reserve(nDoodadCount);
//    for (int i = 0; i < nDoodadCount; i++)
//    {
//        KDoodad* pDoodad = g_ObjectManager.m_DoodadSet.New();
//        KGLOG_PROCESS_ERROR(pDoodad);
//        m_DoodadVector.push_back(pDoodad);
//    }
//
//    m_CellVector.resize(nCellCount);
//    nRetCode = g_ObjectManager.m_CellSet.NewArray(nCellCount, &m_CellVector[0]);
//    KGLOG_PROCESS_ERROR(nRetCode);
//
//    bResult = true;
//Exit0:
//    if (!bResult)                                   
//    {
//        for (vector<KNpc *>::iterator it = m_NpcVector.begin(); it != m_NpcVector.end(); ++it)
//        {
//            g_ObjectManager.m_NpcSet.Delete(*it);
//        }
//        m_NpcVector.clear();
//
//        for (vector<KDoodad *>::iterator it = m_DoodadVector.begin(); it != m_DoodadVector.end(); ++it)
//        {
//            g_ObjectManager.m_DoodadSet.Delete(*it);
//        }
//        m_DoodadVector.clear();
//
//        g_ObjectManager.m_CellSet.DeleteArray((int)m_CellVector.size(), &m_CellVector[0]);
//        m_CellVector.clear();
//    }
//    return bResult;
//}
//
//void KServerObjAlloctor::UnInit()
//{
//    for (vector<KNpc *>::iterator it = m_NpcVector.begin(); it != m_NpcVector.end(); ++it)
//    {
//        g_ObjectManager.m_NpcSet.Delete(*it);
//    }
//    m_NpcVector.clear();
//
//    for (vector<KDoodad *>::iterator it = m_DoodadVector.begin(); it != m_DoodadVector.end(); ++it)
//    {
//        g_ObjectManager.m_DoodadSet.Delete(*it);
//    }
//    m_DoodadVector.clear();
//
//    g_ObjectManager.m_CellSet.DeleteArray((int)m_CellVector.size(), &m_CellVector[0]);
//    m_CellVector.clear();
//}
//
//KNpc *KServerObjAlloctor::NewServerNpc(DWORD dwId)
//{
//    KNpc *pNpc = NULL;
//    int nRetCode = false;
//
//    ASSERT(dwId == ERROR_ID);
//
//    nRetCode = m_NpcVector.empty();
//    if (!nRetCode)
//    {
//        pNpc = m_NpcVector.back();
//        m_NpcVector.pop_back();
//    }
//
//    return pNpc;
//}
//
//
//KDoodad *KServerObjAlloctor::NewServerDoodad(DWORD dwId)
//{
//    KDoodad *pDoodad = NULL;
//    int nRetCode = false;
//
//    ASSERT(dwId == ERROR_ID);
//
//    nRetCode = m_DoodadVector.empty();
//    if (!nRetCode)
//    {
//        pDoodad = m_DoodadVector.back();
//        m_DoodadVector.pop_back();
//    }
//    return pDoodad;
//}
//
//KCell* KServerObjAlloctor::NewCell()
//{
//    KCell *pCell = NULL;
//    int nRetCode = false;
//
//    nRetCode = m_CellVector.empty();
//    if (!nRetCode)
//    {
//        pCell = m_CellVector.back();
//        m_CellVector.pop_back();
//    }
//    return pCell;
//}
//
//void KServerObjAlloctor::DeleteServerNpc(KNpc *pNpc)
//{
//#ifdef _DEBUG
//    vector<KNpc *>::iterator it = find(m_NpcVector.begin(), m_NpcVector.end(), pNpc);
//    ASSERT(it == m_NpcVector.end());
//#endif
//    m_NpcVector.push_back(pNpc);
//}
//
//
//
//void KServerObjAlloctor::DeleteServerDoodad(KDoodad* pDoodad)
//{
//#ifdef _DEBUG
//    vector<KDoodad *>::iterator it = find(m_DoodadVector.begin(), m_DoodadVector.end(), pDoodad);
//    ASSERT(it == m_DoodadVector.end());
//#endif
//    m_DoodadVector.push_back(pDoodad);
//}
//
//
//void KServerObjAlloctor::DeleteCell(KCell* pCell)
//{
//#ifdef _DEBUG
//    vector<KCell*>::iterator it = find(m_CellVector.begin(), m_CellVector.end(), pCell);
//    ASSERT(it == m_CellVector.end());
//#endif
//    m_CellVector.push_back(pCell);
//}