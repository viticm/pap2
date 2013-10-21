#include "stdafx.h"
#include "KLevelUpList.h"
using namespace std;

const char g_cszLevelUpDir[] = "LevelUpData";

KLevelUpList::KLevelUpList(void)
{
}

KLevelUpList::~KLevelUpList(void)
{
}

BOOL KLevelUpList::Init(void)
{
    BOOL bResult = false;
    int  nRetCode = false;
    char szFileName[MAX_PATH];

    snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.tab", SETTING_DIR, g_cszLevelUpDir, "StandardMale");
    szFileName[sizeof(szFileName) - 1] = '\0';
    nRetCode = LoadLevelData(szFileName, rtStandardMale);
    KGLOG_PROCESS_ERROR(nRetCode);

    snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.tab", SETTING_DIR, g_cszLevelUpDir, "StandardFemale");
    szFileName[sizeof(szFileName) - 1] = '\0';
    nRetCode = LoadLevelData(szFileName, rtStandardFemale);
    KGLOG_PROCESS_ERROR(nRetCode);

    snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.tab", SETTING_DIR, g_cszLevelUpDir, "StrongMale");
    szFileName[sizeof(szFileName) - 1] = '\0';
    nRetCode = LoadLevelData(szFileName, rtStrongMale);
    KGLOG_PROCESS_ERROR(nRetCode);

    snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.tab", SETTING_DIR, g_cszLevelUpDir, "SexyFemale");
    szFileName[sizeof(szFileName) - 1] = '\0';
    nRetCode = LoadLevelData(szFileName, rtSexyFemale);
    KGLOG_PROCESS_ERROR(nRetCode);

    snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.tab", SETTING_DIR, g_cszLevelUpDir, "LittleBoy");
    szFileName[sizeof(szFileName) - 1] = '\0';
    nRetCode = LoadLevelData(szFileName, rtLittleBoy);
    KGLOG_PROCESS_ERROR(nRetCode);

    snprintf(szFileName, sizeof(szFileName), "%s/%s/%s.tab", SETTING_DIR, g_cszLevelUpDir, "LittleGirl");
    szFileName[sizeof(szFileName) - 1] = '\0';
    nRetCode = LoadLevelData(szFileName, rtLittleGirl);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_LevelData.clear();
    }
    return bResult;
}

BOOL KLevelUpList::UnInit(void)
{
    m_LevelData.clear();
	return true;
}

KLevelUpData* KLevelUpList::GetLevelUpData(int nType, int nLevel)
{
    KLevelUpData* pResult = NULL;
    KLEVEL_DATA_LIST::iterator it;
    LONG         lKey = MAKELONG(nType, nLevel);

    it = m_LevelData.find(lKey);
    KGLOG_PROCESS_ERROR(it != m_LevelData.end());
    pResult = &it->second;
Exit0:
    if (!pResult)
    {
        KGLogPrintf(KGLOG_ERR, "[KLevelUpList] Get level up data(%d, %d) failed !\n", nType, nLevel);
    }
    return pResult;
}

BOOL KLevelUpList::LoadLevelData(char szFileName[], int nType)
{
    BOOL            bResult         = false;
    int             nRetCode        = false;
    ITabFile*       piTabFile       = NULL;
    int             nLevel          = 0;
    int             nMaxlevel       = 0;
    KLevelUpData    DefaultValue;
    KLevelUpData    LevelData;

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KLevelUpList] Failed to open player level up tab file : %s\n", szFileName);
        goto Exit0;
    }

    nMaxlevel = piTabFile->GetHeight() - 2;
    KGLOG_PROCESS_ERROR(nMaxlevel > 0);

    // Load default value :
    memset(&DefaultValue, 0, sizeof(DefaultValue));

    nRetCode = LoadLine(piTabFile, 2, DefaultValue, DefaultValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    // Load line data:
    for (nLevel = 1; nLevel <= nMaxlevel; nLevel++)
    {
        nRetCode = LoadLine(piTabFile, nLevel + 2, LevelData, DefaultValue);
        KGLOG_PROCESS_ERROR(nRetCode);

        LONG lKey = MAKELONG(nType, nLevel);
        m_LevelData[lKey] = LevelData;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KLevelUpList::LoadLine(ITabFile* piTabFile, int nLine, KLevelUpData& rLevelData, const KLevelUpData& crDefault)
{
    BOOL bResult    = false;
    int  nRetCode   = false;
    int  nLevel     = 0;

    nRetCode = piTabFile->GetInteger(nLine, "Level", 0, &nLevel);
    KGLOG_PROCESS_ERROR(nLevel == nLine - 2);

    nRetCode = piTabFile->GetInteger(nLine, "Experience", crDefault.nExperience, &rLevelData.nExperience);
    KGLOG_PROCESS_ERROR(nRetCode);    

    nRetCode = piTabFile->GetInteger(nLine, "Strength", crDefault.nStrength, &rLevelData.nStrength);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "Agility", crDefault.nAgility, &rLevelData.nAgility);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "Vigor", crDefault.nVigor, &rLevelData.nVigor);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "Spirit", crDefault.nSpirit, &rLevelData.nSpirit);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "Spunk", crDefault.nSpunk, &rLevelData.nSpunk);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "MaxLife", crDefault.nMaxLife, &rLevelData.nMaxLife);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "MaxMana", crDefault.nMaxMana, &rLevelData.nMaxMana);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "MaxStamina", crDefault.nMaxStamina, &rLevelData.nMaxStamina);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "MaxThew", crDefault.nMaxThew, &rLevelData.nMaxThew);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "RunSpeed", crDefault.nRunSpeed, &rLevelData.nRunSpeed);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "JumpSpeed", crDefault.nJumpSpeed, &rLevelData.nJumpSpeed);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "Height", crDefault.nHeight, &rLevelData.nHeight);
    KGLOG_PROCESS_ERROR(nRetCode);

    // ------------- 各种基础属性回复速度 ------------------------------------>

    nRetCode = piTabFile->GetInteger(nLine, "LifeReplenish", crDefault.nLifeReplenish, &rLevelData.nLifeReplenish);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "LifeReplenishPercent", crDefault.nLifeReplenishPercent, &rLevelData.nLifeReplenishPercent);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "LifeReplenishExt", crDefault.nLifeReplenishExt, &rLevelData.nLifeReplenishExt);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "ManaReplenish", crDefault.nManaReplenish, &rLevelData.nManaReplenish);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "ManaReplenishPercent", crDefault.nManaReplenishPercent, &rLevelData.nManaReplenishPercent);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "ManaReplenishExt", crDefault.nManaReplenishExt, &rLevelData.nManaReplenishExt);
    KGLOG_PROCESS_ERROR(nRetCode);

    // -------------- 命中值 -------------------------------------------------> 
    nRetCode = piTabFile->GetInteger(nLine, "HitBase", crDefault.nHitBase, &rLevelData.nHitBase);
    KGLOG_PROCESS_ERROR(nRetCode);

    // -------------- 暴击值 -------------------------------------------------->
    nRetCode = piTabFile->GetInteger(nLine, "PhysicsCriticalStrike", crDefault.nPhysicsCriticalStrike, &rLevelData.nPhysicsCriticalStrike);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "SolarCriticalStrike", crDefault.nSolarCriticalStrike, &rLevelData.nSolarCriticalStrike);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "NeutralCriticalStrike", crDefault.nNeutralCriticalStrike, &rLevelData.nNeutralCriticalStrike);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "LunarCriticalStrike", crDefault.nLunarCriticalStrike, &rLevelData.nLunarCriticalStrike);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(nLine, "PoisonCriticalStrike", crDefault.nPoisonCriticalStrike, &rLevelData.nPoisonCriticalStrike);
    KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piTabFile->GetInteger(nLine, "MaxAssistTimes", crDefault.nMaxAssistTimes, &rLevelData.nMaxAssistTimes);
	KGLOG_PROCESS_ERROR(nRetCode);

    // -------------- 空手时的攻击速度, 与新手技能有关 ----------------->
    nRetCode = piTabFile->GetInteger(nLine, "NoneWeaponAttackSpeedBase",crDefault.nNoneWeaponAttackSpeedBase, &rLevelData.nNoneWeaponAttackSpeedBase);
    KGLOG_PROCESS_ERROR(nRetCode);    

    // -------------- 外功防御系数的最大值 ------------------------>
    nRetCode = piTabFile->GetInteger(nLine, "MaxPhysicsDefence", crDefault.nMaxPhysicsDefence, &rLevelData.nMaxPhysicsDefence);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}
