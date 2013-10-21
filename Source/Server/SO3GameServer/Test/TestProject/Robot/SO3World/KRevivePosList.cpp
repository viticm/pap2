#include "stdafx.h"
#include "StrMap.h"
#include "KPlayer.h"
#include "SO3GlobalDef.h"
#include "KRevivePosList.h"


#define REVIVE_POS_TAB "RevivePos.tab"

BOOL KRevivePosList::Init()
{
    m_RevivePositions.clear();

    LoadRevivePosList();

    return TRUE;
}

BOOL KRevivePosList::Unint()
{
    m_RevivePositions.clear();
    
    return TRUE;
}

KREVIVE_POSITION* KRevivePosList::GetRevivePos(int nIndex)
{
    REVIVE_POSITIONS::iterator it;

    for (it = m_RevivePositions.begin(); it != m_RevivePositions.end(); ++it)
    {
        if (it->second.nIndex == nIndex)
        {
            return &it->second;
        }
    }

    return NULL;
}

BOOL KRevivePosList::LoadRevivePosList()
{
    BOOL                bResult                 = FALSE;
    int                 nRetCode                = 0;
    ITabFile*           piTabFile               = NULL;
    int                 nHeight                 = 0;
    char                szFileName[MAX_PATH];
    char                szCamp[_NAME_LEN];          
    KREVIVE_POSITION    RevivePosition;

    DECLARE_STRING_MAP_BEGIN(KCAMP)
        REGISTER_STR_TO_VALUE("Neutral", cNeutral)   
        REGISTER_STR_TO_VALUE("Good",    cGood)
        REGISTER_STR_TO_VALUE("Evil",    cEvil)
    DECLARE_STRING_MAP_END(KCAMP)

    snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, REVIVE_POS_TAB);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[RevivePosList] Failed to open tab file : %s\n", szFileName);
        goto Exit0;
    }
    
    nHeight = piTabFile->GetHeight();

    for (int nLine = 2; nLine <= nHeight; ++nLine)
    {
        nRetCode = piTabFile->GetInteger(nLine, "MapID", 0, (int*)&RevivePosition.dwMapID);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && "Get MapID");

        nRetCode = piTabFile->GetInteger(nLine, "Index", 0, &RevivePosition.nIndex);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && "Get Index");

        nRetCode = piTabFile->GetInteger(nLine, "X", 0, &RevivePosition.nX);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && "Get X");

        nRetCode = piTabFile->GetInteger(nLine, "Y", 0, &RevivePosition.nY);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && "Get Y");

        nRetCode = piTabFile->GetInteger(nLine, "Z", 0, &RevivePosition.nZ);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && "Get Z");

        nRetCode = piTabFile->GetString(nLine, "Camp", "", szCamp, sizeof(szCamp));
        KGLOG_PROCESS_ERROR(nRetCode > 0 && "Get Camp");
        MAP_STRING_LOCAL(KCAMP, szCamp, RevivePosition.nCamp);

        m_RevivePositions.insert(make_pair(RevivePosition.dwMapID, RevivePosition));
    }


    bResult = TRUE;
Exit0:
    return bResult;
}
