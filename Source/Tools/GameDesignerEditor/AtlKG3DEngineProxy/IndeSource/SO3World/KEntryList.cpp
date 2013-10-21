//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KEntryList.cpp
//  Version     :   1.0
//  Creater     :   Zhao ChunFeng
//  Date        :   2007-03-16 10:18:48
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Global.h"
#include "KEntryList.h"


KEntryList::KEntryList()
{
}

BOOL KEntryList::Init()
{
    BOOL            bResult         = false;
    int             nRetCode        = false;
    ITabFile*       piTabFile       = NULL;
    int             nTabHeight      = 0;
    char            szFileName[PATH_MAX];

    snprintf(szFileName, sizeof(szFileName), "%s/EntryList.tab", SETTING_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[EntryList] Failed to open entry list file : %s\n", szFileName);
        goto Exit0;
    }

    nTabHeight = piTabFile->GetHeight();

    for (int nLine = 2; nLine <= nTabHeight; nLine++)
    {
        __int64     nKey        = 0;
        int         nMapID      = 0;
        int         nIndex      = 0;
        KMapEntry   EntryInfo;

        nRetCode = piTabFile->GetInteger(nLine, "Map", 0, &nMapID);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "Index", 0, &nIndex);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PosX", 0, &EntryInfo.nPosX);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PosY", 0, &EntryInfo.nPosY);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "PosZ", 0, &EntryInfo.nPosZ);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, "Direction", 0, &EntryInfo.nDirection);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nKey = MAKE_INT64(nMapID, nIndex);
        
        m_EntryList[nKey] = EntryInfo;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    if (!bResult)
    {
        m_EntryList.clear();
    }
    return bResult;
}

void KEntryList::UnInit()
{
    m_EntryList.clear();
}

const KMapEntry* KEntryList::GetMapEntry(DWORD dwMapID, int nIndex)
{
    KMapEntry*                  pResult = NULL;
    __int64                     nKey    = 0;
    KMAP_ENTRY_LIST::iterator   it;
    
    nKey = MAKE_INT64(dwMapID, nIndex);

    it = m_EntryList.find(nKey);
    KGLOG_PROCESS_ERROR(it != m_EntryList.end());
    
    pResult = &it->second;
Exit0:
    return pResult;
}
