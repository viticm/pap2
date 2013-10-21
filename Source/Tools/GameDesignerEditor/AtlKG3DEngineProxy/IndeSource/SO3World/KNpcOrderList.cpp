#include "stdafx.h"
#include "KMath.h"
#include "Global.h"
#include "KNpcOrderList.h"
using namespace std;

BOOL KNpcOrderManager::Init()
{
    BOOL        bResult         = false;
    int         nRetCode        = false;
	ITabFile*	piTabFile       = NULL;
	char        szFileName[MAX_PATH];
    int         nTableHeight    = 0;

	snprintf(szFileName, MAX_PATH, "%s/NpcOrder/%s", SETTING_DIR, "OrderList.tab");
    szFileName[MAX_PATH - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	if (!piTabFile)
	{
        KGLogPrintf(KGLOG_ERR, "[KNpcOrderManager] Failed to open tab file: \"%s\".\n", szFileName);
        goto Exit0;
	}

    nTableHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTableHeight >= 1);

    for (int i = 0; i < nTableHeight - 1; ++i)
    {
        KORDER  Order;
        DWORD   dwID    = 0;
        float   fZoom   = 0.0f;
           
        nRetCode = piTabFile->GetInteger(2 + i, "ID", 0, (int*)&dwID);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetString(2 + i, "File", "", szFileName, MAX_PATH);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetFloat(2 + i, "Zoom", 0.0f, &fZoom);
        KGLOG_PROCESS_ERROR(nRetCode == 1);
        KGLOG_PROCESS_ERROR(fZoom != 0.0f);

        nRetCode = LoadOrder(Order, szFileName, fZoom);
        KGLOG_PROCESS_ERROR(nRetCode);

        m_OrderList[dwID] = Order;
    }
    
    bResult = true;
Exit0:
    if (!bResult)
    {
        m_OrderList.clear();
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KNpcOrderManager::UnInit()
{
    m_OrderList.clear();
}


const KORDER* KNpcOrderManager::GetOrder(DWORD dwID)
{
    KORDER_LIST::iterator it = m_OrderList.find(dwID);
    if (it == m_OrderList.end())
        return NULL;
    return &it->second;
}

BOOL KNpcOrderManager::LoadOrder(KORDER& rOrder, const char cszFileName[], float fZoom)
{
    BOOL        bResult         = false;
    int         nRetCode        = false;
	ITabFile*	piTabFile       = NULL;
	char        szFullName[MAX_PATH];
    int         nTableHeight    = 0;

	snprintf(szFullName, MAX_PATH, "%s/NpcOrder/%s", SETTING_DIR, cszFileName);
    szFullName[MAX_PATH - 1] = '\0';

	piTabFile = g_OpenTabFile(szFullName);
	if (!piTabFile)
	{
        KGLogPrintf(KGLOG_ERR, "[KNpcOrderManager] Failed to open tab file: \"%s\".\n", szFullName);
        goto Exit0;
	}

    nTableHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTableHeight > 1);

    for (int i = 0; i < nTableHeight - 1; ++i)
    {
        int         nIndex  = 0;
        float       fRadius = 0.0f;
        float       fAngel  = 0.0f;
        KORDER_NODE Node;

        nRetCode = piTabFile->GetInteger(2 + i, "Index", 0, &nIndex);
        KG_PROCESS_ERROR(nRetCode == 1);
        nRetCode = piTabFile->GetFloat(2 + i, "Radius", 0, &fRadius);
        KG_PROCESS_ERROR(nRetCode == 1);
        nRetCode = piTabFile->GetFloat(2 + i, "Angel", 0, &fAngel);  
        KG_PROCESS_ERROR(nRetCode == 1);

        KG_PROCESS_ERROR(nIndex == i + 1);

        Node.nRadius = (int)(fRadius * fZoom);
        Node.nAngel  = (int)(fAngel * DIRECTION_COUNT / (2 * SO3WORLD_PI));

        if (Node.nAngel < 0)
        {
            Node.nAngel += DIRECTION_COUNT;
        }
        KGLOG_PROCESS_ERROR(Node.nAngel < DIRECTION_COUNT);

        rOrder.push_back(Node);
    }
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;    
}
