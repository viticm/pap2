//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KHometownList.cpp
//  Version     :   1.0
//  Creater     :   Zhao ChunFeng
//  Date        :   2007-03-16 16:18:48
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <algorithm>
#include "KHometownList.h"
#include "KMapManager.h"
#include "KSO3GameCenter.h"
using namespace std;


KHowntownList::KHowntownList()
{
}

BOOL KHowntownList::Load()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    ITabFile*   piTabFile               = NULL;
    int         nTableHeight            = 0;
    char        szFileName[PATH_MAX];

    snprintf(szFileName, sizeof(szFileName), "%s/HometownList.tab", SETTING_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        goto Exit0;
    }

    nTableHeight = piTabFile->GetHeight();

    for (int nLine = 2; nLine <= nTableHeight; nLine++)
    {
        KMapInfo*                       pMapInfo   = NULL;
        KBornPointInfo                  BornPoint;
        KHOMETOWN_MAP_TABLE::iterator   MapTabIter;

        nRetCode = piTabFile->GetInteger(nLine, "MapID", 0, (int*)&BornPoint.dwMapID);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "RoleType", 0, &BornPoint.nRoleType);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PosX", 0, &BornPoint.nPosX);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PosY", 0, &BornPoint.nPosY);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PosZ", 0, &BornPoint.nPosZ);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "Direction", 0, &BornPoint.nDirection);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(BornPoint.dwMapID);
        KGLOG_PROCESS_ERROR(pMapInfo && pMapInfo->m_nType == emtBirthMap);

        MapTabIter = find(m_HometownMapTable.begin(), m_HometownMapTable.end(), BornPoint.dwMapID);
        if (MapTabIter == m_HometownMapTable.end())
        {
            m_HometownMapTable.push_back(BornPoint.dwMapID);
        }

        m_BornPointTable.push_back(BornPoint);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_ERR, "Load tab file %s failed !\n", szFileName);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KHowntownList::GetBornPointInfo(KBornPointInfo* pBornPointInfo, DWORD dwMapID, int nRoleType)
{
    BOOL bResult = false;

    for (KBORN_POINT_TABLE::iterator it = m_BornPointTable.begin(); it != m_BornPointTable.end(); ++it)
    {
        if (it->dwMapID == dwMapID && it->nRoleType == nRoleType)
        {
            *pBornPointInfo = *it;
            bResult = true;
            break;
        }
    }

    return bResult;
}

