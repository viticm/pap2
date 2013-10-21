#include "stdafx.h"
#include "KCampInfo.h"
#include "Global.h"

KCampInfo::KCampInfo() : m_nCampScore(0), m_nCampLevel(0), m_nVersion(0)
{
    ZeroMemory(m_nLevelScore, sizeof(m_nLevelScore));
    ZeroMemory(m_GoodCampLevelParam, sizeof(m_GoodCampLevelParam));
	ZeroMemory(m_EvilCampLevelParam, sizeof(m_EvilCampLevelParam));
    ZeroMemory(m_nActivePlayerCount, sizeof(m_nActivePlayerCount));

    m_bGoodCampClosed = false;
    m_bEvilCampClosed = false;
}

BOOL KCampInfo::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = LoadCampLevelParam();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult; 
}

int KCampInfo::GetMoneyPercent(KCAMP eCamp, int nLevel)
{
    int nResult = KILO_NUM;
    
    assert(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);

    if (eCamp == cGood)
    {
        nResult = m_GoodCampLevelParam[m_nCampLevel].nMoneyPercent;
    }
    else if (eCamp == cEvil)
    {
        nResult = m_EvilCampLevelParam[m_nCampLevel].nMoneyPercent;
    }
    
    return nResult;
}

int KCampInfo::GetReputePercent(KCAMP eCamp, int nLevel)
{
    int nResult = KILO_NUM;
    
    assert(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);

    if (eCamp == cGood)
    {
        nResult = m_GoodCampLevelParam[m_nCampLevel].nReputePercent;
    }
    else if (eCamp == cEvil)
    {
        nResult = m_EvilCampLevelParam[m_nCampLevel].nReputePercent;
    }
    
    return nResult;
}

int KCampInfo::GetPrestigePercent(KCAMP eCamp, int nLevel)
{
    int nResult = KILO_NUM;

    assert(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);

    if (eCamp == cGood)
    {
        nResult = m_GoodCampLevelParam[m_nCampLevel].nPrestigePercent;
    }
    else if (eCamp == cEvil)
    {
        nResult = m_EvilCampLevelParam[m_nCampLevel].nPrestigePercent;
    }
    
    return nResult;
}

int KCampInfo::GetReducePrestigeOnDeath(KCAMP eCamp, int nLevel)
{
    int nResult = KILO_NUM;
    
    assert(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);

    if (eCamp == cGood)
    {
        nResult = m_GoodCampLevelParam[m_nCampLevel].nReducePrestigeOnDeath;
    }
    else if (eCamp == cEvil)
    {
        nResult = m_EvilCampLevelParam[m_nCampLevel].nReducePrestigeOnDeath;
    }
    
    return nResult;
}

int KCampInfo::GetDamagePercent(KCAMP eCamp, int nLevel)
{
    int nResult = KILO_NUM;
    
    assert(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);

    if (eCamp == cGood)
    {
        nResult = m_GoodCampLevelParam[m_nCampLevel].nDamagePercent;
    }
    else if (eCamp == cEvil)
    {
        nResult = m_EvilCampLevelParam[m_nCampLevel].nDamagePercent;
    }
    
    return nResult;
}

int KCampInfo::GetShieldPercent(KCAMP eCamp, int nLevel)
{
    int nResult = KILO_NUM;
    
    assert(nLevel >= 0 && nLevel < CAMP_LEVEL_COUNT);

    if (eCamp == cGood)
    {
        nResult = m_GoodCampLevelParam[m_nCampLevel].nShieldPercent;
    }
    else if (eCamp == cEvil)
    {
        nResult = m_EvilCampLevelParam[m_nCampLevel].nShieldPercent;
    }
    
    return nResult;
}

BOOL KCampInfo::LoadCampLevelParam()
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    ITabFile*	piTabFile   = NULL;
    int         nTabHeight  = 0;
	char		szFileName[MAX_PATH];
    char        szColumn[32];

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, "CampLevelParam.tab");
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(piTabFile);

    nTabHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight >= 1);

    for (int i = 0; i < CAMP_LEVEL_COUNT; i++)
    {
        snprintf(szColumn, sizeof(szColumn), "Level%d", i);
        szColumn[sizeof(szColumn) - 1] = '\0';
        
        nRetCode = piTabFile->GetInteger("Score", szColumn, 0, &m_nLevelScore[i]);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("GoodCampMoneyPercent", szColumn, 0, &m_GoodCampLevelParam[i].nMoneyPercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("GoodCampReputePercent", szColumn, 0, &m_GoodCampLevelParam[i].nReputePercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("GoodCampPrestigePercent", szColumn, 0, &m_GoodCampLevelParam[i].nPrestigePercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("GoodCampReducePrestigeOnDeath", szColumn, 0, &m_GoodCampLevelParam[i].nReducePrestigeOnDeath);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("GoodCampDamagePercent", szColumn, 0, &m_GoodCampLevelParam[i].nDamagePercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("GoodCampShieldPercent", szColumn, 0, &m_GoodCampLevelParam[i].nShieldPercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("EvilCampMoneyPercent", szColumn, 0, &m_EvilCampLevelParam[i].nMoneyPercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("EvilCampReputePercent", szColumn, 0, &m_EvilCampLevelParam[i].nReputePercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("EvilCampPrestigePercent", szColumn, 0, &m_EvilCampLevelParam[i].nPrestigePercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("EvilCampReducePrestigeOnDeath", szColumn, 0, &m_EvilCampLevelParam[i].nReducePrestigeOnDeath);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("EvilCampDamagePercent", szColumn, 0, &m_EvilCampLevelParam[i].nDamagePercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger("EvilCampShieldPercent", szColumn, 0, &m_EvilCampLevelParam[i].nShieldPercent);
        KGLOG_PROCESS_ERROR(nRetCode == 1);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}


