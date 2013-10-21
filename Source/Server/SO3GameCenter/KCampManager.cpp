#include "stdafx.h"
#include "KCampManager.h"
#include "Global.h"
#include "KSO3GameCenter.h"

BOOL KCampManager::Init()
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    ITabFile*	piTabFile   = NULL;
    char		szFileName[MAX_PATH];
    char        szColumn[32];
    
    m_nScore    = 0;
    m_nLevel    = INIT_CAMP_LEVEL; // ≥ı ºµ»º∂

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, "CampLevelParam.tab");
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(piTabFile);
    
    for (int i = 0; i < CAMP_LEVEL_COUNT; i++)
    {
        snprintf(szColumn, sizeof(szColumn), "Level%d", i);
        szColumn[sizeof(szColumn) - 1] = '\0';

        nRetCode = piTabFile->GetInteger("Score", szColumn, 0, &m_nLevelScore[i]);
        KGLOG_PROCESS_ERROR(nRetCode == 1);
    }

    for (int i = 0; i < CAMP_LEVEL_COUNT; ++i)
    {
        KGLOG_PROCESS_ERROR(m_nLevelScore[i] >= 0);
    }
    
    memset(m_nActivePlayerCount, 0, sizeof(m_nActivePlayerCount));

    m_bUpdate = false;

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KCampManager::Activate()
{
    if (g_pSO3GameCenter->m_nWorkLoop % GAME_FPS == 0)
    {
        Save();
    }
}

void KCampManager::AddScore(int nScore)
{   
    m_nScore += nScore;
    
    if (nScore > 0)
    {
        while (m_nScore >= m_nLevelScore[m_nLevel] && m_nLevel < CAMP_LEVEL_COUNT - 1)
        {
            m_nScore -= m_nLevelScore[m_nLevel];
            m_nLevel++;
        }
    }
    else
    {
        while (m_nScore <= -m_nLevelScore[CAMP_LEVEL_COUNT - m_nLevel - 1] && m_nLevel > 0)
        {
            m_nScore += m_nLevelScore[CAMP_LEVEL_COUNT - m_nLevel - 1];
            m_nLevel--;
        }
    }
    
    MAKE_IN_RANGE(m_nScore, -m_nLevelScore[CAMP_LEVEL_COUNT - m_nLevel - 1], m_nLevelScore[m_nLevel]);

    m_bUpdate = true;

    g_pSO3GameCenter->m_GameServer.DoSyncCampInfo(-1);
}

void KCampManager::Reset()
{
    m_nScore    = 0;
    m_nLevel    = INIT_CAMP_LEVEL;
    m_bUpdate   = true;

    g_pSO3GameCenter->m_GameServer.DoSyncCampInfo(-1);
}

void KCampManager::Save()
{
    int nTotalScore = m_nScore;
    int nLevel      = m_nLevel;
    
    KG_PROCESS_ERROR(m_bUpdate);

    assert(m_nLevel >= 0 && m_nLevel < CAMP_LEVEL_COUNT);

    if (nLevel > INIT_CAMP_LEVEL)
    {
        while (nLevel-- > INIT_CAMP_LEVEL)
        {
            nTotalScore += m_nLevelScore[nLevel];
        }
    } 
    else if (nLevel < INIT_CAMP_LEVEL)
    {
        while (nLevel++ < INIT_CAMP_LEVEL)
        {
            nTotalScore -= m_nLevelScore[CAMP_LEVEL_COUNT - nLevel - 1];
        }
    }

    g_pSO3GameCenter->m_MiscDB.DoSaveGlobalSystemValue(CAMP_SCORE_IN_DB, nTotalScore);

    m_bUpdate = false;

Exit0:
    return;
}
