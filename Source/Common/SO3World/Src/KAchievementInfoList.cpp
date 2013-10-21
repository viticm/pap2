#include "stdafx.h"
#include "Global.h"
#include "KAchievementInfoList.h"

#define ACHIEVEMENT_INFO_TAB "AchievementInfo.tab"

KAchievementInfoList::KAchievementInfoList()
{
}

KAchievementInfoList::~KAchievementInfoList()
{
}

BOOL KAchievementInfoList::Init()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    ITabFile*   piTabFile               = NULL;
    int         nLine                   = 0;
    int         nHeight                 = 0;
    char        szFileName[MAX_PATH];

    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, ACHIEVEMENT_INFO_TAB);
    szFileName[sizeof(szFileName) - 1] = '\0';
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szFileName));

    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight(); 
    KGLOG_PROCESS_ERROR(nHeight > 0);

    for (nLine = 2; nLine <= nHeight; nLine++)
    {
        int                     nID;
        int                     nShiftType;
        int                     nPrefix;
        int                     nPostfix;
        KACHIEVEMENT_INFO       Info;
        KINFO_LIST::iterator    it;

        nRetCode = piTabFile->GetInteger(nLine, "ID", 0, &nID);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nID >= 0 && nID < MAX_ACHIEVEMENT_ID);

        nRetCode = piTabFile->GetInteger(nLine, "TriggerVal", 1, &Info.nTriggerVal);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "ShiftID", 0, &Info.nShiftID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "ShiftType", astNeverShift, (int*)&nShiftType);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nShiftType >= 0 && nShiftType < astTotal);
        Info.byShiftType = (BYTE)nShiftType;

        nRetCode = piTabFile->GetInteger(nLine, "Point", 0, &Info.nPoint);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Info.nPoint >= 0);

        nRetCode = piTabFile->GetInteger(nLine, "Exp", 0, &Info.nExp);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Info.nExp >= 0);

        nRetCode = piTabFile->GetInteger(nLine, "Prefix", 0, &nPrefix);
        KGLOG_PROCESS_ERROR(nRetCode);
        Info.byPrefix = (BYTE)nPrefix;

        nRetCode = piTabFile->GetInteger(nLine, "Postfix", 0, &nPostfix);
        KGLOG_PROCESS_ERROR(nRetCode);
        Info.byPostfix = (BYTE)nPostfix;

        it = m_InfoList.find(nID);
        KGLOG_PROCESS_ERROR(it == m_InfoList.end());

        m_InfoList.insert(std::make_pair(nID, Info));
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_InfoList.clear();
        KGLogPrintf(KGLOG_DEBUG, "Load %s failed at line %d.", ACHIEVEMENT_INFO_TAB, nLine);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KAchievementInfoList::UnInit()
{
    m_InfoList.clear();
}

KACHIEVEMENT_INFO* KAchievementInfoList::GetInfo(int nAchievementID)
{
    KACHIEVEMENT_INFO*      pResult = NULL;
    KINFO_LIST::iterator    it;
    
    it = m_InfoList.find(nAchievementID);
    KG_PROCESS_ERROR(it != m_InfoList.end());

    pResult = &it->second;
Exit0:
    return pResult;
}
