#include "stdafx.h"
#include "Global.h"
#include "KDesignationList.h"
#include "KDesignation.h"

#define PREFIX_INFO_TAB "DesignationPrefixInfo.tab"
#define POSTFIX_INFO_TAB "DesignationPostfixInfo.tab"

BOOL KDesignationList::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = LoadPrefixInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadPostfixInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KDesignationList::UnInit()
{
    m_PrefixList.clear();
    m_PostfixList.clear();
}

KPrefixInfo* KDesignationList::GetPrefixInfo(int nPrefix)
{
    KPrefixInfo*            pResult = NULL;
    KPREFIX_MAP::iterator   it;

    it = m_PrefixList.find(nPrefix);
    KGLOG_PROCESS_ERROR(it != m_PrefixList.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

KPostfixInfo* KDesignationList::GetPostfixInfo(int nPostfix)
{
    KPostfixInfo*           pResult = NULL;
    KPOSTFIX_MAP::iterator  it;

    it = m_PostfixList.find(nPostfix);
    KGLOG_PROCESS_ERROR(it != m_PostfixList.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

BOOL KDesignationList::LoadPrefixInfo()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    ITabFile*   piTabFile               = NULL;
    int         nLine                   = 0;
    int         nHeight                 = 0;
    char        szFileName[MAX_PATH];

    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, PREFIX_INFO_TAB);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szFileName));

    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight(); 
    KGLOG_PROCESS_ERROR(nHeight > 0);

    for (nLine = 2; nLine <= nHeight; nLine++)
    {
        int         nID             = 0;
        int         nAnnounceType   = 0;
        KPrefixInfo Info;

        nRetCode = piTabFile->GetInteger(nLine, "ID", 0, &nID);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nID > 0 && nID <= MAX_DESIGNATION_FIX_ID);

        nRetCode = piTabFile->GetInteger(nLine, "AnnounceType", 1, &nAnnounceType);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nAnnounceType > datInvalid && nAnnounceType < datTotal);
        Info.byAnnounceType = (BYTE)nAnnounceType;

        m_PrefixList[nID] = Info;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_PrefixList.clear();
        KGLogPrintf(KGLOG_DEBUG, "Load %s failed at line %d.", PREFIX_INFO_TAB, nLine);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KDesignationList::LoadPostfixInfo()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    ITabFile*   piTabFile               = NULL;
    int         nLine                   = 0;
    int         nHeight                 = 0;
    char        szFileName[MAX_PATH];

    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, POSTFIX_INFO_TAB);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szFileName));

    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight(); 
    KGLOG_PROCESS_ERROR(nHeight > 0);

    for (nLine = 2; nLine <= nHeight; nLine++)
    {
        int             nID             = 0;
        int             nAnnounceType   = 0;
        KPostfixInfo    Info;

        nRetCode = piTabFile->GetInteger(nLine, "ID", 0, &nID);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nID > 0 && nID <= MAX_DESIGNATION_FIX_ID);

        nRetCode = piTabFile->GetInteger(nLine, "AnnounceType", 0, &nAnnounceType);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nAnnounceType > datInvalid && nAnnounceType < datTotal);
        Info.byAnnounceType = (BYTE)nAnnounceType;

        m_PostfixList[nID] = Info;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_PostfixList.clear();
        KGLogPrintf(KGLOG_DEBUG, "Load %s failed at line %d.", POSTFIX_INFO_TAB, nLine);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

