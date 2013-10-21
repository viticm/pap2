#include "stdafx.h"
#include "KTransmissionList.h"

#define TRANSLATE_LIST_PATH "TransmissionList.tab"

KTransmissionList::KTransmissionList()
{
}

BOOL KTransmissionList::Init()
{
    BOOL bResult                = false;
    int  nRetCode               = 0;
    char szFilePath[MAX_PATH];

    nRetCode = snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, TRANSLATE_LIST_PATH);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode <= sizeof(szFilePath));

    m_TranslateMap.clear();

    nRetCode = LoadTab(szFilePath);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KTransmissionList::UnInit()
{
    m_TranslateMap.clear();
}

BOOL KTransmissionList::GetTranslatePosition(DWORD dwCityID, DWORD* pdwRetMapID, int* pnRetX, int* pnRetY, int* pnRetZ)
{
    BOOL                        bResult     = false;
    POSITION*                   pPosition   = NULL;
    KTRANSLATE_MAP::iterator    FindIt;

    assert(pdwRetMapID);
    assert(pnRetX);
    assert(pnRetY);
    assert(pnRetZ);

    FindIt = m_TranslateMap.find(dwCityID);
    KGLOG_PROCESS_ERROR(FindIt != m_TranslateMap.end());

    pPosition = &FindIt->second;

    *pdwRetMapID = pPosition->dwMapID;
    *pnRetX      = pPosition->nX;
    *pnRetY      = pPosition->nY;
    *pnRetZ      = pPosition->nZ;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTransmissionList::LoadTab(char szFilePath[])
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    ITabFile*   piFileTab   = NULL;
    int         nHeight     = 0;

    piFileTab = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(piFileTab);

    nHeight = piFileTab->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight >= 1);

    for (int nIndex = 2; nIndex <= nHeight; nIndex++)
    {
        DWORD       dwCityID = 0;
        POSITION    Position;

        nRetCode = piFileTab->GetInteger(nIndex, "CityID", 0, (int*)&dwCityID);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piFileTab->GetInteger(nIndex, "MapID", 0, (int*)&Position.dwMapID);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piFileTab->GetInteger(nIndex, "X", 0, (int*)&Position.nX);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piFileTab->GetInteger(nIndex, "Y", 0, (int*)&Position.nY);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piFileTab->GetInteger(nIndex, "Z", 0, (int*)&Position.nZ);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        m_TranslateMap.insert(make_pair(dwCityID, Position));
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piFileTab);
    return bResult;
}

