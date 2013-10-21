#include "stdafx.h"
#include <algorithm>
#include "Engine/KMemory.h"
#include "KSO3World.h"
#include "StrMap.h"
#include "KAttribute.h"
#include "KItemLib.h"
#include "KPlayer.h"
#include "KBuffManager.h"
#include "KPlayerServer.h"

#define SKILL_FILE_NAME				"skill/skills.tab"
#define SKILL_SCRIPT_PATH           "scripts/skill"

#define SKILL_EVENT_TAB_FILE        "SkillEvent.tab"

#define WEAPON_MAP_SKILL_TAB_FILE   "WeaponMapSkill.tab"

#define DEFENCE_COEFFICIENT_TAB     "skill/DefenceCoefficient.tab"

#define KUNG_FU_EXP_TAB             "skill/KungFuExp.tab"

#define SKILL_RECIPE_TAB            "skill/recipeSkill.tab"

#define SKILL_LEARNING_TAB          "skill/SkillLearning.tab"
#define SKILL_REALIZATION_TAB       "skill/SkillRealization.tab"

#define VENATION_SCRIPT             "scripts/skill/经脉/Venation.lua"

#define MAX_SKILL_LEVEL             (SCHAR_MAX)

#define ARRAY_LEN                   64

#ifdef _SERVER
#define SKILL_RECIPE_CACHE_SIZE     8192
#define SKILL_RECIPE_CACHE_SHRINK   32
#else
#define SKILL_RECIPE_CACHE_SIZE     256
#define SKILL_RECIPE_CACHE_SHRINK   4
#endif

KSkillManager::KSkillManager(void)
{

};


BOOL KSkillManager::Init(void)
{
    BOOL                             bResult                 = false;
	BOOL                             bRetCode                = false;
	ITabFile*                        piSkillFile             = NULL;
	char                             szFilePath[MAX_PATH]    = "???";
	int                              nHeight                 = 0;
    SKILL_TABLE_LINE                 TableLineDefaultValue;
	KSkill*                          pKSkill                 = NULL;

    m_dwSkillSNGenerator    = 1;

    KGLogPrintf(KGLOG_INFO, "Skill loading ... ...");

#ifdef _SERVER
    bRetCode = LoadSkillEvent();
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, SKILL_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';
	piSkillFile = g_OpenTabFile(szFilePath);
	if (!piSkillFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

	nHeight = piSkillFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);

    memset(&TableLineDefaultValue, 0, sizeof(TableLineDefaultValue));
    bRetCode = LoadSkillTableLine(piSkillFile, 2, &TableLineDefaultValue, TableLineDefaultValue);
    KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(TableLineDefaultValue.dwSkillID == 0);

	for (int nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		SKILL_TABLE_LINE TableLine;
        KSKILL_BASE_INFO SkillBaseInfo;

		bRetCode = LoadSkillTableLine(piSkillFile, nIndex, &TableLine, TableLineDefaultValue);
		if (!bRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "[Skill] Load skill table file line failed, see file \"%s\", line %d\n", szFilePath, nIndex);
            goto Exit0;
        }

        bRetCode = TranslateTableInfo(&SkillBaseInfo, TableLine);
        if (!bRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "[Skill] Load skill base info failed, see file \"%s\", line %d\n", szFilePath, nIndex);
            goto Exit0;
        }

        m_SkillBaseInfoList[SkillBaseInfo.dwSkillID] = SkillBaseInfo;
	}

    bRetCode = LoadWeaponMapSkill();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadDefenceCoefficientTable(DEFENCE_COEFFICIENT_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadKungFuExpTable(KUNG_FU_EXP_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadSkillRecipeBaseInfo(SKILL_RECIPE_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_SkillRecipeCache.Init(SKILL_RECIPE_CACHE_SIZE, SKILL_RECIPE_CACHE_SHRINK);
    KGLOG_PROCESS_ERROR(bRetCode);

#if defined (_SERVER)
    for (SKILL_BASE_INFO_LIST::iterator it = m_SkillBaseInfoList.begin(); it != m_SkillBaseInfoList.end(); ++it)
    {
        bRetCode = LoadSkillDataFull(it->first);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bRetCode = CheckError();
    KGLOG_PROCESS_ERROR(bRetCode);
    m_dwBulletIDIndex = 1;

#endif
    bRetCode = LoadSkillRealizationTable(SKILL_REALIZATION_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadSkillLearningTable(SKILL_LEARNING_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "%u skill loaded !", m_SkillBaseInfoList.size());
    if (!bResult)
    {
        ClearnUp();
        KGLogPrintf(KGLOG_ERR, "[Skill] Init skill manager failed !\n");
    }
	KG_COM_RELEASE(piSkillFile);
	return bResult;
}

void KSkillManager::UnInit(void)
{
    ClearnUp();
}


KSkill* KSkillManager::GetSkill_RAW(DWORD dwSkillID, DWORD dwSkillLevel)
{
    KSkill*                  pResult            = NULL;
    BOOL                     bRetCode           = false;
	SKILL_LIST::iterator     it;
    const KSKILL_BASE_INFO*  pcSkillBaseInfo    = NULL; 
    KSkill*                  pSkill             = NULL;
    SKILL_BASE_INFO_LIST::iterator BaseInfoIt;


    BaseInfoIt = m_SkillBaseInfoList.find(dwSkillID);
    KG_PROCESS_ERROR(BaseInfoIt != m_SkillBaseInfoList.end());
    pcSkillBaseInfo = &BaseInfoIt->second;

    KG_PROCESS_ERROR((dwSkillLevel > 0) && (dwSkillLevel <= pcSkillBaseInfo->dwMaxLevel));

	it =  m_SkillList.find(dwSkillID);
#if defined (_CLIENT)
	if (it == m_SkillList.end())
	{
	    bRetCode = LoadSkillDataFull(dwSkillID);
        KGLOG_PROCESS_ERROR(bRetCode);
        it =  m_SkillList.find(dwSkillID);
    }
#endif
    KGLOG_PROCESS_ERROR(it != m_SkillList.end());
    
    pSkill  = it->second;
    KG_ASSERT_EXIT(pSkill);

    pResult = pSkill + (dwSkillLevel - 1);

Exit0:
	return pResult;
}

KSkill* KSkillManager::GetSkill_RAW_S(DWORD dwSkillID, DWORD dwSkillLevel)
{
    KSkill* pResult = NULL;
    KSkill* pSkill  = GetSkill_RAW(dwSkillID, dwSkillLevel);

    KG_PROCESS_ERROR(pSkill);
    KG_PROCESS_ERROR(pSkill->m_pBaseInfo->dwBelongKungfu != INVALID_SKILL_ID);

    pResult = pSkill;
Exit0:
    return pResult;
}

KSkill* KSkillManager::GetSkill_K(DWORD dwSkillID, DWORD dwSkillLevel)
{
    KSkill* pResult = NULL;
    KSkill* pSkill  = GetSkill_RAW(dwSkillID, dwSkillLevel);

    KG_PROCESS_ERROR(pSkill);
    KG_PROCESS_ERROR(pSkill->m_pBaseInfo->dwBelongKungfu == INVALID_SKILL_ID);

    pResult = pSkill;
Exit0:
    return pResult;
}

struct K_GET_PLAYER_SKILL_RECIPE
{
    K_GET_PLAYER_SKILL_RECIPE()
    {
        pRecipeSkillBaseInfoMap = NULL;
        bHaveSystemRecipe       = false;
        dwSkillID               = 0;
        dwSkillLevel            = 0;
        nRecipeType             = 0;
        nRecipeKeyCount         = 0;
    };

    BOOL operator() (const KG_SKILL_RECIPE_LIST_NODE& crRecipeListNode);

    BOOL GetResult(KSKILL_RECIPE_KEY* pSkillRecipeKey);

    std::map<DWORD, KSKILL_RECIPE_BASE_INFO>*   pRecipeSkillBaseInfoMap;
    BOOL    bHaveSystemRecipe;
    DWORD   dwSkillID;
    DWORD   dwSkillLevel;
    int     nRecipeType;
    DWORD   dwRecipeKeyArray[MAX_SKILL_REICPE_COUNT];
    int     nRecipeKeyCount;
};

BOOL K_GET_PLAYER_SKILL_RECIPE::operator () (const KG_SKILL_RECIPE_LIST_NODE& crRecipeListNode)
{
    BOOL                        bResult                 = false;
    KSKILL_RECIPE_BASE_INFO*    pRecipeSkillBaseInfo    = NULL;
    std::map<DWORD, KSKILL_RECIPE_BASE_INFO>::iterator it;

    assert(pRecipeSkillBaseInfoMap);

    it = pRecipeSkillBaseInfoMap->find(crRecipeListNode.dwRecipeKey);
    KGLOG_PROCESS_ERROR(it != pRecipeSkillBaseInfoMap->end());

    pRecipeSkillBaseInfo = &(it->second);
    assert(pRecipeSkillBaseInfo);

    // Non-active user recipe will be ignore.
    KG_PROCESS_SUCCESS(!pRecipeSkillBaseInfo->bIsSystemRecipe && !crRecipeListNode.bActive);

    if (
        (pRecipeSkillBaseInfo->dwSkillID == dwSkillID && pRecipeSkillBaseInfo->dwSkillLevel == 0) ||
        (pRecipeSkillBaseInfo->dwSkillID == dwSkillID && pRecipeSkillBaseInfo->dwSkillLevel == dwSkillLevel) ||
        (pRecipeSkillBaseInfo->nSkillRecipeType && !pRecipeSkillBaseInfo->bIsSystemRecipe && pRecipeSkillBaseInfo->nSkillRecipeType == nRecipeType)
    )
    {
        // Set system recipe flag, and clean all user recipe. 
        if (pRecipeSkillBaseInfo->bIsSystemRecipe && !bHaveSystemRecipe)
        {
            nRecipeKeyCount     = 0;
            bHaveSystemRecipe   = true;
        }

        // If we're in system recipe mode, avoid user recipes. 
        KG_PROCESS_SUCCESS(!pRecipeSkillBaseInfo->bIsSystemRecipe && bHaveSystemRecipe);

        KGLOG_PROCESS_ERROR(nRecipeKeyCount < MAX_SKILL_REICPE_COUNT);
        dwRecipeKeyArray[nRecipeKeyCount++] = crRecipeListNode.dwRecipeKey;
    }

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        nRecipeKeyCount = 0;
    }
    return bResult;
}

BOOL K_GET_PLAYER_SKILL_RECIPE::GetResult(KSKILL_RECIPE_KEY* pSkillRecipeKey)
{
    BOOL nResult = false;

    assert(pSkillRecipeKey);

    assert(nRecipeKeyCount <= MAX_SKILL_REICPE_COUNT);
    for (int i = 0; i < MAX_SKILL_REICPE_COUNT - nRecipeKeyCount; i++)
    {
        pSkillRecipeKey->dwRecipeKey[i] = 0;
    }
    for (int i = MAX_SKILL_REICPE_COUNT - nRecipeKeyCount; i < MAX_SKILL_REICPE_COUNT; i++)
    {
        pSkillRecipeKey->dwRecipeKey[i] = dwRecipeKeyArray[i - MAX_SKILL_REICPE_COUNT + nRecipeKeyCount];
    }

    nResult = true;
Exit0:
    return nResult;
}

BOOL KSkillManager::GetSkillRecipeKey(KSKILL_RECIPE_KEY* pSkillRecipeKey, DWORD dwSkillID, DWORD dwSkillLevel, KCharacter* pCharacter)
{
    BOOL    nResult  = false;
    BOOL    nRetCode = false;
    KSkill* pSkill   = NULL;

    assert(pSkillRecipeKey);
    assert(pCharacter);

    memset(pSkillRecipeKey, 0, sizeof(KSKILL_RECIPE_KEY));

    pSkill = GetSkill_RAW(dwSkillID, dwSkillLevel);
    if (!pSkill)
    {
        KGLogPrintf(
            KGLOG_ERR, "[Skill] GetSkill Error, SkillID: %lu, SkillLevel: %lu.",
            dwSkillID, dwSkillLevel
        );
        goto Exit0;
    }

    pSkillRecipeKey->dwSkillID    = dwSkillID;
    pSkillRecipeKey->dwSkillLevel = dwSkillLevel;

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pCharacter;
        K_GET_PLAYER_SKILL_RECIPE Traver;

        Traver.pRecipeSkillBaseInfoMap  = &m_SkillRecipeBaseInfoMap;
        Traver.dwSkillID                = dwSkillID;
        Traver.dwSkillLevel             = dwSkillLevel;
        Traver.nRecipeType              = pSkill->m_pBaseInfo->nRecipeType;

        nRetCode = pPlayer->m_SkillRecipeList.Traverse(Traver);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = Traver.GetResult(pSkillRecipeKey);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

KSkill* KSkillManager::GetSkill(const KSKILL_RECIPE_KEY& crSkillRecipeKey)
{
    KSkill* pResult         = NULL;
    BOOL    bRetCode        = false;
    BOOL    bRecipeFlag     = false;
    BOOL    bNewSkillFlag   = false;
    KSkill* pSkillBase      = NULL;
    KSkill* pSkill          = NULL;
    DWORD   dwMinRecipeKey  = ULONG_MAX;

    assert(sizeof(DWORD) == sizeof(unsigned long));

    for (int i = MAX_SKILL_REICPE_COUNT - 1; i >= 0; i--)
    {
        const DWORD cdwRecipeKey = crSkillRecipeKey.dwRecipeKey[i];
        if (cdwRecipeKey != 0)
        {
            bRecipeFlag = true;
        }

        assert((!cdwRecipeKey || cdwRecipeKey < dwMinRecipeKey) && "Not all recipe key ascending ordered. ");
        dwMinRecipeKey = cdwRecipeKey;
    }

    if (!bRecipeFlag)
    {
        pResult = GetSkill_RAW_S(crSkillRecipeKey.dwSkillID, crSkillRecipeKey.dwSkillLevel);
        goto Exit0;
    }

    pSkill = m_SkillRecipeCache.Find(crSkillRecipeKey);
    KG_PROCESS_SUCCESS(pSkill);

    // If not in cache
    pSkill = m_SkillRecipeCache.Insert(crSkillRecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    bNewSkillFlag = true;

    pSkillBase = GetSkill_RAW_S(crSkillRecipeKey.dwSkillID, crSkillRecipeKey.dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkillBase);
    *pSkill = *pSkillBase;

    pSkill->m_pAttrbuteEffectToSelfAndRollback = NULL;
    pSkill->m_pAttrbuteEffectToSelfNotRollback = NULL;
    pSkill->m_pAttrbuteEffectToDestAndRollback = NULL;
    pSkill->m_pAttrbuteEffectToDestNotRollback = NULL;
    pSkill->m_pAttrbuteEffectToDestOnMaxWeak   = NULL;
    pSkill->m_pAttrbuteEffectToDestOnMajorWeak = NULL;
    pSkill->m_pAttrbuteEffectToDestOnMinorWeak = NULL;

    for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
    {
        KSKILL_RECIPE_BASE_INFO*                pSkillRecipe    = NULL;
        KSKILL_RECIPE_BASE_INFO_MAP::iterator   itBase;
        int                                     nLuaTopIndex    = 0;
        DWORD                                   dwReciepID      = 0;
        DWORD                                   dwRecipeLevel   = 0;

        if (crSkillRecipeKey.dwRecipeKey[i] == 0)
            continue;

        GET_RECIPE_ID_OF_KEY(dwReciepID, crSkillRecipeKey.dwRecipeKey[i]);
        GET_RECIPE_LEVEL_OF_KEY(dwRecipeLevel, crSkillRecipeKey.dwRecipeKey[i]);

        itBase = m_SkillRecipeBaseInfoMap.find(crSkillRecipeKey.dwRecipeKey[i]);
        KG_PROCESS_ERROR(itBase != m_SkillRecipeBaseInfoMap.end());

        pSkillRecipe = &(itBase->second);

        pSkill->m_nPrepareFrames += pSkillRecipe->nPrepareFramesAdd;
        pSkill->m_nPrepareFrames  = pSkill->m_nPrepareFrames * (pSkillRecipe->nPrepareFramesPercent) / KILO_NUM;

        for (int j = 0; j < MAX_SKILL_COOL_DOWN_TIMER; j++)
        {
            pSkill->m_nCoolDownAlter[j] += pSkillRecipe->nCoolDownAdd[j];
        }

        pSkill->m_nMinRadius   += pSkillRecipe->nMinRadiusAdd;
        pSkill->m_nMaxRadius   += pSkillRecipe->nMaxRadiusAdd;

        pSkill->m_nCostLife     = pSkill->m_nCostLife * (pSkillRecipe->nCostLifeAddPercent + KILO_NUM) / KILO_NUM;
        pSkill->m_nCostMana     = pSkill->m_nCostMana * (pSkillRecipe->nCostManaAddPercent + KILO_NUM) / KILO_NUM;
        pSkill->m_nCostStamina += pSkillRecipe->nCostStaminaAdd;

        pSkill->m_nDamageAddPercent += pSkillRecipe->nDamageAddPercent;

        bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pSkillRecipe->szScriptFile);
        if (bRetCode)
        {
            g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

            g_pSO3World->m_ScriptCenter.PushValueToStack(pSkill);
            g_pSO3World->m_ScriptCenter.PushValueToStack(dwReciepID);
            g_pSO3World->m_ScriptCenter.PushValueToStack(dwRecipeLevel);

            bRetCode = g_pSO3World->m_ScriptCenter.CallFunction(pSkillRecipe->szScriptFile, "GetSkillRecipeData", 1);
            g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    // 对秘籍修饰后的技能数值做必要的修正，策划在脚本里设置的数值，由脚本自己检查。
    pSkill->m_nPrepareFrames    = max(0, pSkill->m_nPrepareFrames);
    pSkill->m_nMinRadius        = max(0, pSkill->m_nMinRadius);
    pSkill->m_nMaxRadius        = max(pSkill->m_nMinRadius, pSkill->m_nMaxRadius);
    pSkill->m_nCostLife         = max(0, pSkill->m_nCostLife);
    pSkill->m_nCostMana         = max(0, pSkill->m_nCostMana);
    pSkill->m_nCostStamina      = max(0, pSkill->m_nCostStamina);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToSelfAndRollback), pSkillBase->m_pAttrbuteEffectToSelfAndRollback);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToSelfNotRollback), pSkillBase->m_pAttrbuteEffectToSelfNotRollback);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToDestAndRollback), pSkillBase->m_pAttrbuteEffectToDestAndRollback);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToDestNotRollback), pSkillBase->m_pAttrbuteEffectToDestNotRollback);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToDestOnMaxWeak), pSkillBase->m_pAttrbuteEffectToDestOnMaxWeak);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToDestOnMajorWeak), pSkillBase->m_pAttrbuteEffectToDestOnMajorWeak);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pSkill->m_pAttrbuteEffectToDestOnMinorWeak), pSkillBase->m_pAttrbuteEffectToDestOnMinorWeak);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    pResult = pSkill;
Exit0:
    if (!pResult && bNewSkillFlag)
    {
        m_SkillRecipeCache.Erase(crSkillRecipeKey);
        bNewSkillFlag = false;
    }
    return pResult;
}

KSKILL_RECIPE_BASE_INFO* KSkillManager::GetSkillRecipeBaseInfo(DWORD dwRecipeKey)
{
    KSKILL_RECIPE_BASE_INFO* pResult = NULL;
    KSKILL_RECIPE_BASE_INFO_MAP::iterator it;

    it = m_SkillRecipeBaseInfoMap.find(dwRecipeKey);
    if (it != m_SkillRecipeBaseInfoMap.end())
    {
        pResult = &it->second;
    }

    return pResult;
}

KSKILL_EVENT* KSkillManager::GetEventInfo(int nID)
{
    KSKILL_EVENT* pResult = NULL;
    KSKILL_EVENT_MAP::iterator it;

    it = m_SkillEventList.find(nID);
    if (it != m_SkillEventList.end())
    {
        pResult = &it->second;
    }

    return pResult;
}

int KSkillManager::GetDefenceCoefficient(int nRow, int nProbability)
{
    int    nResult  = -1;
    
    KG_PROCESS_ERROR(nRow >= 0 && nRow < MAX_ROW - 1);
    KG_PROCESS_ERROR(nProbability >= 0 && nProbability < 10000);

    for (int nIndex = 0; nIndex < MAX_COL - 1; ++nIndex)
    {
        if (nProbability <= m_nDefenceCoefficient[nRow][nIndex])
        {
            nResult = m_nDefenceCoefficient[MAX_ROW - 1][nIndex];
            goto Exit0;
        }
    }

    KGLogPrintf(
        KGLOG_ERR, "[GetDefenceCoefficient] nRow: %d, nProbability: %d.",
        nRow, nProbability
    );

Exit0:
    return nResult;
}

KSKILL_LEARNING_INFO* KSkillManager::GetSkillLearningInfo(DWORD dwMasterID, DWORD dwSkillID, DWORD dwSkillLevel)
{
    KSKILL_LEARNING_INFO*   pResult     = NULL;
    BOOL                    bRetCode    = false;
    KSKILL_MASTER_KEY       SkillMasterKey;
    KSKILL_LEARNING_INFO_MAP::iterator it;

    KG_PROCESS_ERROR(dwMasterID);
    KG_PROCESS_ERROR(dwSkillID);
    KG_PROCESS_ERROR(dwSkillLevel);

    SkillMasterKey.dwMasterID   = dwMasterID;
    SkillMasterKey.dwSkillID    = dwSkillID;
    SkillMasterKey.dwSkillLevel = dwSkillLevel;

    it = m_SkillLearningInfoMap.find(SkillMasterKey);
    bRetCode = (it != m_SkillLearningInfoMap.end());
    KG_PROCESS_ERROR(bRetCode);

    pResult = &(it->second);
Exit0:
    return pResult;
}

BOOL KSkillManager::IsInstinct(DWORD dwSkillID, DWORD dwSkillLevel)
{
    BOOL    bResult = false;
    KSkill* pSkill  = GetSkill_RAW(dwSkillID, dwSkillLevel);

    KG_PROCESS_ERROR(pSkill);
    KG_PROCESS_ERROR(pSkill->m_pBaseInfo->bIsInstinct);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::IsAutoTurn(DWORD dwSkillID, DWORD dwSkillLevel)
{
    BOOL    bResult = false;
    KSkill* pSkill  = GetSkill_RAW(dwSkillID, dwSkillLevel);

    KG_PROCESS_ERROR(pSkill);
    KG_PROCESS_ERROR(pSkill->m_pBaseInfo->bIsAutoTurn);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::IsAccumulate(DWORD dwSkillID, DWORD dwSkillLevel)
{
    BOOL    bResult = false;
    KSkill* pSkill  = GetSkill_RAW(dwSkillID, dwSkillLevel);

    KG_PROCESS_ERROR(pSkill);
    KG_PROCESS_ERROR(pSkill->m_bIsAccumulate);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::GetMasterSkillList(DWORD dwMasterID, size_t uMaxArrayCount, size_t* puRetArrayCount, KSKILL_ID_LEVEL* pRetSkillListArray)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KSKILL_MASTER_KEY                   SkillMasterKey;
    KSKILL_LEARNING_INFO_MAP::iterator  itBegin;
    KSKILL_LEARNING_INFO_MAP::iterator  itEnd;

    KG_PROCESS_ERROR(uMaxArrayCount > 0);
    KG_PROCESS_ERROR(pRetSkillListArray);

    *puRetArrayCount = 0;

    SkillMasterKey.dwMasterID   = dwMasterID;
    SkillMasterKey.dwSkillID    = 0;
    SkillMasterKey.dwSkillLevel = 0;

    itBegin = m_SkillLearningInfoMap.lower_bound(SkillMasterKey);

    SkillMasterKey.dwMasterID += 1;

    itEnd   = m_SkillLearningInfoMap.lower_bound(SkillMasterKey);

    for (
        KSKILL_LEARNING_INFO_MAP::iterator it = itBegin; 
        it != itEnd && *puRetArrayCount < uMaxArrayCount; 
        ++it
    )
    {
        if (it->first.dwMasterID != dwMasterID)
        {
            break;
        }

        pRetSkillListArray->dwSkillID       = it->first.dwSkillID;
        pRetSkillListArray->dwSkillLevel    = it->first.dwSkillLevel;
        pRetSkillListArray++;
        (*puRetArrayCount)++;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::IsCommonSkill(DWORD dwSkillID)
{
    KWEAPON_MAP_SKILL::iterator it;

    for (it = m_WeaponMapSkill.begin(); it != m_WeaponMapSkill.end(); ++it)
    {
        if (it->second.dwCommonSkillID == dwSkillID)
            return true;
    }

    return false;
}

BOOL KSkillManager::WeaponMapCommonSkill(int nWeaponType, DWORD& rdwSkillID)
{
    KWEAPON_MAP_SKILL::iterator it;
    
    it = m_WeaponMapSkill.find(nWeaponType);
    if (it != m_WeaponMapSkill.end())
    {
        rdwSkillID = it->second.dwCommonSkillID;
        return true;
    }

    return false;
}

int KSkillManager::CommonSkillMapWeapon(DWORD dwSkillID)
{
    int nResult = wdtTotal;
    KWEAPON_MAP_SKILL::iterator it;
    
    for (it = m_WeaponMapSkill.begin(); it != m_WeaponMapSkill.end(); ++it)
    {
        if (it->second.dwCommonSkillID == dwSkillID)
        {
            nResult = it->first;
            break;
        }
    }

    return nResult;
}

BOOL KSkillManager::WeaponMapBaseKungfu(int nWeaponType, DWORD& rdwKungfuID)
{
    KWEAPON_MAP_SKILL::iterator it;
    
    it = m_WeaponMapSkill.find(nWeaponType);
    if (it != m_WeaponMapSkill.end())
    {
        rdwKungfuID = it->second.dwBaseKungfuID;
        return true;
    }

    return false;
}

BOOL KSkillManager::GetKungFuExp(DWORD dwKungFuID, DWORD dwKungFuLevel, KKUNG_FU_EXP** ppKungFuExp)
{
    BOOL bResult = false;
    KKUNG_FU_EXP_LIST::iterator it;
    uint64_t uKey = 0;

    uKey = MAKE_INT64(dwKungFuID, dwKungFuLevel);

    it = m_KungFuExpList.find(uKey);
    KGLOG_PROCESS_ERROR(it != m_KungFuExpList.end());

    *ppKungFuExp = &(it->second);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::LoadSkillTableLine(ITabFile* pSkillFile, int nRow, SKILL_TABLE_LINE *pTableLineInfo, const SKILL_TABLE_LINE &crDefaultValue)
{
	BOOL    bResult         = false;
    int     nTemp           = 0;
	int     nRetCode        = false;
    char    szTableString[MAX_PATH];

	KG_ASSERT_EXIT(pSkillFile);
	KG_ASSERT_EXIT(nRow > 1);

	nRetCode = pSkillFile->GetString(
		nRow, 
		"SkillName", 
		crDefaultValue.szSkillName, 
		pTableLineInfo->szSkillName, 
		sizeof(pTableLineInfo->szSkillName)
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get SkillName");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"SkillID", 
		crDefaultValue.dwSkillID, 
		(int*)&pTableLineInfo->dwSkillID
	);
	KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR((pTableLineInfo->dwSkillID > 0) || (nRow == 2)); // row 2 is default value !
	//KGLOG_PROCESS_ERROR(pTableLineInfo->dwSkillID < MAX_SKILL_COUNT);

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "SkillMark", 
        crDefaultValue.nSkillMark, 
        (int*)&(pTableLineInfo->nSkillMark)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SkillMark");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"MaxLevel", 
		crDefaultValue.dwMaxLevel, 
		(int*)&(pTableLineInfo->dwMaxLevel)
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get MaxLevel");
    KGLOG_PROCESS_ERROR(pTableLineInfo->dwMaxLevel > 0);
    KGLOG_PROCESS_ERROR(pTableLineInfo->dwMaxLevel <= MAX_SKILL_LEVEL);
    
    nRetCode = pSkillFile->GetString(
        nRow, 
        "KindType", 
        crDefaultValue.szKindType, 
        pTableLineInfo->szKindType,
        sizeof(pTableLineInfo->szKindType)    
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get KindType");

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "RecipeType", 
        crDefaultValue.nRecipeType, 
        &pTableLineInfo->nRecipeType
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get RecipeType");

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "UIType", 
        crDefaultValue.nUIType, 
        &pTableLineInfo->nUIType
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get UIType");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "BelongKungfu",
        crDefaultValue.dwBelongKungfu,
        (int *)&pTableLineInfo->dwBelongKungfu
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get BelongKungfu");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "BelongSchool",
        crDefaultValue.dwBelongSchool,
        (int *)&pTableLineInfo->dwBelongSchool
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get BelongSchool");

	nRetCode = pSkillFile->GetString(
		nRow, 
		"CastMode", 
		crDefaultValue.szCastMode, 
		pTableLineInfo->szCastMode,
		sizeof(pTableLineInfo->szCastMode)
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get CastMode");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"WeaponRequest", 
		crDefaultValue.dwWeaponRequest, 
		(int *)&pTableLineInfo->dwWeaponRequest
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get WeaponRequest");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "IsCostAmmo",
        crDefaultValue.bCostAmmo,
        (int*)&nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get IsCostAmmo");
    pTableLineInfo->bCostAmmo = (nTemp != 0);

    nRetCode = pSkillFile->GetString(
        nRow,
        "CostItemType",
        crDefaultValue.szCostItemType,
        pTableLineInfo->szCostItemType,
        sizeof(pTableLineInfo->szCostItemType)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get CostItemType");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "CostItemIndex",
        crDefaultValue.nCostItemIndex,
        &pTableLineInfo->nCostItemIndex
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get CostItemIndex");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"MountRequestType", 
		crDefaultValue.dwMountRequestType, 
		(int *)&pTableLineInfo->dwMountRequestType
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get MountRequestType");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"MountRequestDetail", 
		crDefaultValue.dwMountRequestDetail, 
		(int *)&pTableLineInfo->dwMountRequestDetail
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get MountRequestDetail");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "MountRequestDetailLevel",
        crDefaultValue.dwMountRequestDetailLevel,
        (int *)&pTableLineInfo->dwMountRequestDetailLevel
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get MountRequestDetailLevel");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"MountType", 
		crDefaultValue.dwMountType, 
		(int *)&pTableLineInfo->dwMountType
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get MountType");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"IsMountAble", 
		crDefaultValue.bIsMountable, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get IsMountAble");
    pTableLineInfo->bIsMountable = (nTemp != 0);

    nRetCode = pSkillFile->GetString(
        nRow,
        "EffectType",
        crDefaultValue.szEffectType,
        pTableLineInfo->szEffectType,
        sizeof(pTableLineInfo->szEffectType)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get EffectType");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"IsPassiveSkill", 
		crDefaultValue.bIsPassiveSkill, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get IsPassiveSkill");
    pTableLineInfo->bIsPassiveSkill = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "IsChannelSkill", 
        crDefaultValue.bIsChannelSkill, 
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get IsChannelSkill");
    pTableLineInfo->bIsChannelSkill = (nTemp != 0);

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"IsExpSkill", 
		crDefaultValue.bIsExpSkill, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get IsExpSkill");
    pTableLineInfo->bIsExpSkill = (nTemp != 0);

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"IsExactHit", 
		crDefaultValue.bIsExactHit, 
		&pTableLineInfo->bIsExactHit
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get IsExactHit");

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"IsInstinct", 
		crDefaultValue.bIsInstinct, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get IsInstinct");
    pTableLineInfo->bIsInstinct = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "IsAutoTurn", 
        crDefaultValue.bIsAutoTurn, 
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get IsAutoTurn");
    pTableLineInfo->bIsAutoTurn = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "CheckBindBuff", 
        crDefaultValue.bCheckBindBuff, 
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get CheckBindBuff");
    pTableLineInfo->bCheckBindBuff = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "CauseAbradeEquipment", 
        crDefaultValue.bCauseAbradeEquipment, 
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get CauseAbradeEquipment");
    pTableLineInfo->bCauseAbradeEquipment = (nTemp != 0);

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"CauseBeatBreak", 
		crDefaultValue.bCauseBeatBreak, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get CauseBeatBreak");
    pTableLineInfo->bCauseBeatBreak = (nTemp != 0);

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"CauseBeatBack", 
		crDefaultValue.bCauseBeatBack, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get CauseBeatBack");
    pTableLineInfo->bCauseBeatBack = (nTemp != 0);

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"HasCriticalStrike", 
		crDefaultValue.bHasCriticalStrike, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get HasCriticalStrike");
    pTableLineInfo->bHasCriticalStrike = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "SkillEventMask1",
        crDefaultValue.dwSkillEventMask1,
        (int*)&pTableLineInfo->dwSkillEventMask1
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SkillEventMask1");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "SkillEventMask2",
        crDefaultValue.dwSkillEventMask2,
        (int*)&pTableLineInfo->dwSkillEventMask2
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SkillEventMask2");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "SkillCastFlag",
        crDefaultValue.dwSkillCastFlag,
        (int*)&pTableLineInfo->dwSkillCastFlag
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SkillCastFlag");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "CastMask",
        crDefaultValue.dwCastMask,
        (int*)&pTableLineInfo->dwCastMask
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get CastMask");

    nRetCode = pSkillFile->GetInteger(
		nRow, 
		"UseCastScript", 
		crDefaultValue.bUseCastScript, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get UseCastScript");
    pTableLineInfo->bUseCastScript = (nTemp != 0);

	nRetCode = pSkillFile->GetInteger(
		nRow, 
		"Use3DObstacle", 
		crDefaultValue.bUse3DObstacle, 
		&nTemp
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get Use3DObstacle");
    pTableLineInfo->bUse3DObstacle = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "IgnorePositiveShield",
        crDefaultValue.bIgnorePositiveShield,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get IgnorePositiveShield");
    pTableLineInfo->bIgnorePositiveShield = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "IgnoreNegativeShield",
        crDefaultValue.bIgnoreNegativeShield,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get IgnoreNegativeShield");
    pTableLineInfo->bIgnoreNegativeShield = (nTemp != 0);

    //  ------------------------ self ----------------------------

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "NeedOutOfFight",
        crDefaultValue.bNeedOutOfFight,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get NeedOutOfFight");
    pTableLineInfo->bNeedOutOfFight = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "SelfMoveStateMask",
        crDefaultValue.nSelfMoveStateMask,
        &(pTableLineInfo->nSelfMoveStateMask)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SelfMoveStateMask");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "SelfBackupMoveStateMask",
        crDefaultValue.nSelfBackupMoveStateMask,
        &(pTableLineInfo->nSelfBackupMoveStateMask)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SelfBackupMoveStateMask");

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "SelfOnFear", 
        crDefaultValue.bSelfOnFear, 
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get SelfOnFear");
    pTableLineInfo->bSelfOnFear = (nTemp != 0);

	nRetCode = pSkillFile->GetString(
		nRow, 
		"SelfHorseStateRequest", 
		crDefaultValue.szSelfHorseStateRequest, 
		pTableLineInfo->szSelfHorseStateRequest,
		sizeof(pTableLineInfo->szSelfHorseStateRequest)
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get SelfHorseStateRequest");

    //  ------------------------ target ----------------------------

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetTypePlayer",
        crDefaultValue.bTargetTypePlayer,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetTypePlayer");
    pTableLineInfo->bTargetTypePlayer = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetTypeNpc",
        crDefaultValue.bTargetTypeNpc,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetTypeNpc");
    pTableLineInfo->bTargetTypeNpc = (nTemp != 0);
    
    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationNone",
        crDefaultValue.bTargetRelationNone,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationNone");
    pTableLineInfo->bTargetRelationNone = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationSelf",
        crDefaultValue.bTargetRelationSelf,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationSelf");
    pTableLineInfo->bTargetRelationSelf = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationAlly",
        crDefaultValue.bTargetRelationAlly,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationAlly");
    pTableLineInfo->bTargetRelationAlly = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationEnemy",
        crDefaultValue.bTargetRelationEnemy,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationEnemy");
    pTableLineInfo->bTargetRelationEnemy = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationDialog",
        crDefaultValue.bTargetRelationNeutrality,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationDialog");
    pTableLineInfo->bTargetRelationNeutrality = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationParty",
        crDefaultValue.bTargetRelationTeam,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationParty");
    pTableLineInfo->bTargetRelationTeam = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationRaid",
        crDefaultValue.bTargetRelationRaid,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationRaid");
    pTableLineInfo->bTargetRelationRaid = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationMentor",
        crDefaultValue.bTargetRelationMentor,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationMentor");
    pTableLineInfo->bTargetRelationMentor = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetRelationApprentice",
        crDefaultValue.bTargetRelationApprentice,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetRelationApprentice");
    pTableLineInfo->bTargetRelationApprentice = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "TargetMoveStateMask",
        crDefaultValue.nTargetMoveStateMask,
        &(pTableLineInfo->nTargetMoveStateMask)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetMoveStateMask");

    nRetCode = pSkillFile->GetInteger(
        nRow, 
        "TargetOnFear", 
        crDefaultValue.bTargetOnFear, 
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get TargetOnFear");
    pTableLineInfo->bTargetOnFear = (nTemp != 0);

    nRetCode = pSkillFile->GetString(
		nRow, 
		"TargetHorseStateRequest", 
		crDefaultValue.szTargetHorseStateRequest, 
		pTableLineInfo->szTargetHorseStateRequest,
		sizeof(pTableLineInfo->szTargetHorseStateRequest)
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get TargetHorseStateRequest");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "AutoSelectTarget",
        crDefaultValue.bAutoSelectTarget,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get AutoSelectTarget");
    pTableLineInfo->bAutoSelectTarget = (nTemp != 0);

    nRetCode = pSkillFile->GetString(
        nRow,
        "EffectPlayType",
        crDefaultValue.szEffectPlayType,
        pTableLineInfo->szEffectPlayType,
        sizeof(pTableLineInfo->szEffectPlayType)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get EffectPlayType");

    nRetCode = pSkillFile->GetString(
        nRow,
        "CommonSkillActiveMode",
        crDefaultValue.szCommonSkillActiveMode,
        pTableLineInfo->szCommonSkillActiveMode,
        sizeof(pTableLineInfo->szCommonSkillActiveMode)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get CommonSkillActiveMode");

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "NPCWeaponType",
        crDefaultValue.nNPCWeaponType,
        &pTableLineInfo->nNPCWeaponType
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get NPCWeaponType");

    // -----------------------------------------------------------------------

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "IsBindCombatTag",
        crDefaultValue.bBindCombatTag,
        &nTemp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get BindCombatTag");
    pTableLineInfo->bBindCombatTag = (nTemp != 0);

    nRetCode = pSkillFile->GetInteger(
        nRow,
        "MapBanMask",
        crDefaultValue.dwMapBanMask,
        (int*)&pTableLineInfo->dwMapBanMask
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Get MapBanMask");
    // -----------------------------------------------------------------------

	nRetCode = pSkillFile->GetString(
		nRow, 
		"ScriptFile", 
		"", 
		szTableString,
		sizeof(szTableString)
	);
	KGLOG_PROCESS_ERROR(nRetCode && "Get ScriptFile");

    nRetCode = snprintf(
        pTableLineInfo->szScriptFile, 
        sizeof(pTableLineInfo->szScriptFile), 
        "%s/%s", SKILL_SCRIPT_PATH, 
        szTableString
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(pTableLineInfo->szScriptFile));

	bResult = true;
Exit0:
	return bResult;
}

BOOL KSkillManager::LoadSkillDataFull(DWORD dwSkillID)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    SKILL_BASE_INFO_LIST::iterator  it;
    KSKILL_BASE_INFO               *pSkillBaseInfo      = NULL;
    KSkill                         *pSkillArray         = NULL;
    KSkill                         *pSkill              = NULL;
    DWORD                           dwSkillInitCount    = 0;
    DWORD                           dwIndex             = 0;

    // confirm it has been add into the base info list !
    it = m_SkillBaseInfoList.find(dwSkillID);
    KGLOG_PROCESS_ERROR(it != m_SkillBaseInfoList.end());
    pSkillBaseInfo = &it->second;
    KG_ASSERT_EXIT(pSkillBaseInfo);

    pSkillArray = new KSkill[pSkillBaseInfo->dwMaxLevel];
    KGLOG_PROCESS_ERROR(pSkillArray);

    // Init skill info for each level !
    for (dwIndex = 0; dwIndex < pSkillBaseInfo->dwMaxLevel; ++dwIndex)
    {
        pSkill              = &pSkillArray[dwIndex];
        bRetCode            = pSkill->Init(pSkillBaseInfo, dwIndex + 1);
        KGLOG_PROCESS_ERROR(bRetCode);
        ++dwSkillInitCount;
    }

    m_SkillList[dwSkillID] = pSkillArray;

    bResult = true;
Exit0:
    if (!bResult)
    {
        for (dwIndex = 0; dwIndex < dwSkillInitCount; ++dwIndex)
        {
            assert(pSkillArray);
            pSkill = &pSkillArray[dwIndex];
            pSkill->UnInit();
        }
        KG_DELETE_ARRAY(pSkillArray);
    }
    return bResult;
}


BOOL KSkillManager::TranslateTableInfo(KSKILL_BASE_INFO *pSkillBaseInfo,  const SKILL_TABLE_LINE &crTableLineInfo)
{
    BOOL bResult = false;    

    DECLARE_STRING_MAP_BEGIN(KSKILL_KIND_TYPE)
        REGISTER_STR_TO_VALUE("Physics",        sktPhysics)         
        REGISTER_STR_TO_VALUE("SolarMagic",     sktSolarMagic)      
        REGISTER_STR_TO_VALUE("NeutralMagic",   sktNeutralMagic)    
        REGISTER_STR_TO_VALUE("LunarMagic",     sktLunarMagic)      
        REGISTER_STR_TO_VALUE("Poison",         sktPoison)                   
        REGISTER_STR_TO_VALUE("Leap",           sktLeap)
        REGISTER_STR_TO_VALUE("None",           sktNone)
    DECLARE_STRING_MAP_END(KSKILL_KIND_TYPE)

    DECLARE_STRING_MAP_BEGIN(SKILL_HORSE_STATE_CONDITION)
        REGISTER_STR_TO_VALUE("Any",            shcAny)
        REGISTER_STR_TO_VALUE("Horse",          shcHorse)
        REGISTER_STR_TO_VALUE("NotHorse",       shcNotHorse)
    DECLARE_STRING_MAP_END(SKILL_HORSE_STATE_CONDITION)

    DECLARE_STRING_MAP_BEGIN(KSKILL_CAST_MODE)
        REGISTER_STR_TO_VALUE("Sector",         scmSector)
        REGISTER_STR_TO_VALUE("CasterArea",     scmCasterArea)
        REGISTER_STR_TO_VALUE("TargetArea",     scmTargetArea)
        REGISTER_STR_TO_VALUE("PointArea",      scmPointArea)
        REGISTER_STR_TO_VALUE("CasterSingle",   scmCasterSingle)
        REGISTER_STR_TO_VALUE("TargetSingle",   scmTargetSingle)
        REGISTER_STR_TO_VALUE("TargetChain",    scmTargetChain)
        REGISTER_STR_TO_VALUE("Point",          scmPoint)
		REGISTER_STR_TO_VALUE("Item",			scmItem)
        REGISTER_STR_TO_VALUE("TargetLeader",   scmTargetLeader)
        REGISTER_STR_TO_VALUE("PartyArea",      scmTeamArea)
    DECLARE_STRING_MAP_END(KSKILL_CAST_MODE)

    DECLARE_STRING_MAP_BEGIN(ITEM_TABLE_TYPE)
        REGISTER_STR_TO_VALUE("Invalid",        ittInvalid)
        REGISTER_STR_TO_VALUE("Other",          ittOther)
        REGISTER_STR_TO_VALUE("CustomWeapon",   ittCustWeapon)
        REGISTER_STR_TO_VALUE("CustomArmor",    ittCustArmor)
        REGISTER_STR_TO_VALUE("CustomTrinket",  ittCustTrinket)
    DECLARE_STRING_MAP_END(ITEM_TABLE_TYPE)

    DECLARE_STRING_MAP_BEGIN(KSKILL_COMMON_ACTIVE_MODE)
        REGISTER_STR_TO_VALUE("Invalid",        scamInvalid)
        REGISTER_STR_TO_VALUE("None",           scamNone)
        REGISTER_STR_TO_VALUE("Melee",          scamMelee)
        REGISTER_STR_TO_VALUE("Range",          scamRange)
    DECLARE_STRING_MAP_END(KSKILL_COMMON_ACTIVE_MODE)

    DECLARE_STRING_MAP_BEGIN(KSKILL_CAST_EFFECT_TYPE)
        REGISTER_STR_TO_VALUE("Invalid",        scetInvalid)
        REGISTER_STR_TO_VALUE("Harmful",        scetHarmful)
        REGISTER_STR_TO_VALUE("Beneficial",     scetBeneficial)
        REGISTER_STR_TO_VALUE("Neutral",        scetNeutral)
    DECLARE_STRING_MAP_END(KSKILL_CAST_EFFECT_TYPE)

    DECLARE_STRING_MAP_BEGIN(KSKILL_EFFECT_PLAY_TYPE)
        REGISTER_STR_TO_VALUE("Invalid",        septInvalid)
        REGISTER_STR_TO_VALUE("AniOnly",        septAniOnly)
        REGISTER_STR_TO_VALUE("SfxOnly",        septSfxOnly)
        REGISTER_STR_TO_VALUE("AniAndSfx",      septAniAndSfx)
        REGISTER_STR_TO_VALUE("None",           septNone)
    DECLARE_STRING_MAP_END(KSKILL_EFFECT_PLAY_TYPE)

    KG_ASSERT_EXIT(pSkillBaseInfo);

    strncpy(pSkillBaseInfo->szSkillName, crTableLineInfo.szSkillName, sizeof(pSkillBaseInfo->szSkillName));
    pSkillBaseInfo->szSkillName[sizeof(pSkillBaseInfo->szSkillName) - 1] = '\0';

    pSkillBaseInfo->dwSkillID               = crTableLineInfo.dwSkillID;
    pSkillBaseInfo->nSkillMark              = crTableLineInfo.nSkillMark;
    pSkillBaseInfo->dwMaxLevel              = crTableLineInfo.dwMaxLevel;

    MAP_STRING_LOCAL(KSKILL_KIND_TYPE, crTableLineInfo.szKindType, pSkillBaseInfo->nKindType);
    pSkillBaseInfo->nRecipeType             = crTableLineInfo.nRecipeType;

    pSkillBaseInfo->nUIType                 = crTableLineInfo.nUIType;
    pSkillBaseInfo->dwBelongKungfu          = crTableLineInfo.dwBelongKungfu;
    pSkillBaseInfo->dwBelongSchool          = crTableLineInfo.dwBelongSchool;

    MAP_STRING_LOCAL(KSKILL_CAST_MODE, crTableLineInfo.szCastMode, pSkillBaseInfo->nCastMode);

    pSkillBaseInfo->dwWeaponRequest         = crTableLineInfo.dwWeaponRequest;
    pSkillBaseInfo->bCostAmmo               = crTableLineInfo.bCostAmmo;

    MAP_STRING_LOCAL(ITEM_TABLE_TYPE, crTableLineInfo.szCostItemType, pSkillBaseInfo->nCostItemType);
    pSkillBaseInfo->nCostItemIndex          = crTableLineInfo.nCostItemIndex;
    
    pSkillBaseInfo->dwMountRequestType      = crTableLineInfo.dwMountRequestType;
    pSkillBaseInfo->dwMountRequestDetail    = crTableLineInfo.dwMountRequestDetail;
    pSkillBaseInfo->dwMountRequestDetailLevel   = crTableLineInfo.dwMountRequestDetailLevel;
    pSkillBaseInfo->dwMountType             = crTableLineInfo.dwMountType;

    pSkillBaseInfo->bIsMountable            = crTableLineInfo.bIsMountable;

    MAP_STRING_LOCAL(KSKILL_CAST_EFFECT_TYPE, crTableLineInfo.szEffectType, pSkillBaseInfo->nEffectType);

    pSkillBaseInfo->bIsPassiveSkill         = crTableLineInfo.bIsPassiveSkill;
    pSkillBaseInfo->bIsChannelSkill         = crTableLineInfo.bIsChannelSkill;
    pSkillBaseInfo->bIsExpSkill             = crTableLineInfo.bIsExpSkill;
    pSkillBaseInfo->bIsExactHit             = crTableLineInfo.bIsExactHit;
    pSkillBaseInfo->bIsInstinct             = crTableLineInfo.bIsInstinct;
    pSkillBaseInfo->bIsAutoTurn             = crTableLineInfo.bIsAutoTurn;
    pSkillBaseInfo->bCheckBindBuff          = crTableLineInfo.bCheckBindBuff;
    pSkillBaseInfo->bCauseAbradeEquipment   = crTableLineInfo.bCauseAbradeEquipment;   
    pSkillBaseInfo->bCauseBeatBreak         = crTableLineInfo.bCauseBeatBreak;   
    pSkillBaseInfo->bCauseBeatBack          = crTableLineInfo.bCauseBeatBack;
    pSkillBaseInfo->bHasCriticalStrike      = crTableLineInfo.bHasCriticalStrike;
    pSkillBaseInfo->dwSkillEventMask1       = crTableLineInfo.dwSkillEventMask1;
    pSkillBaseInfo->dwSkillEventMask2       = crTableLineInfo.dwSkillEventMask2;
    pSkillBaseInfo->dwSkillCastFlag         = crTableLineInfo.dwSkillCastFlag;
    pSkillBaseInfo->dwCastMask              = crTableLineInfo.dwCastMask;

    pSkillBaseInfo->bUseCastScript          = crTableLineInfo.bUseCastScript;
    pSkillBaseInfo->bUse3DObstacle          = crTableLineInfo.bUse3DObstacle;
    pSkillBaseInfo->bIgnorePositiveShield   = crTableLineInfo.bIgnorePositiveShield;
    pSkillBaseInfo->bIgnoreNegativeShield   = crTableLineInfo.bIgnoreNegativeShield;

    pSkillBaseInfo->bNeedOutOfFight         = crTableLineInfo.bNeedOutOfFight;
    pSkillBaseInfo->nSelfMoveStateMask      = crTableLineInfo.nSelfMoveStateMask;
    pSkillBaseInfo->nSelfBackupMoveStateMask    = crTableLineInfo.nSelfBackupMoveStateMask;

    pSkillBaseInfo->bSelfOnFear             = crTableLineInfo.bSelfOnFear;

    pSkillBaseInfo->bBindCombatTag          = crTableLineInfo.bBindCombatTag;

    MAP_STRING_LOCAL(SKILL_HORSE_STATE_CONDITION, crTableLineInfo.szSelfHorseStateRequest, pSkillBaseInfo->nSelfHorseStateRequest);

    pSkillBaseInfo->bTargetTypePlayer       = crTableLineInfo.bTargetTypePlayer;
    pSkillBaseInfo->bTargetTypeNpc          = crTableLineInfo.bTargetTypeNpc;

    pSkillBaseInfo->nTargetRelationMask     = 0;
    pSkillBaseInfo->nTargetRelationMask    |= (crTableLineInfo.bTargetRelationNone   ? sortNone   : 0);
    pSkillBaseInfo->nTargetRelationMask    |= (crTableLineInfo.bTargetRelationSelf   ? sortSelf   : 0);
    pSkillBaseInfo->nTargetRelationMask    |= (crTableLineInfo.bTargetRelationAlly   ? sortAlly   : 0);
    pSkillBaseInfo->nTargetRelationMask    |= (crTableLineInfo.bTargetRelationEnemy  ? sortEnemy  : 0);
    pSkillBaseInfo->nTargetRelationMask    |= (crTableLineInfo.bTargetRelationNeutrality ? sortNeutrality : 0);
    pSkillBaseInfo->nTargetRelationMask    |= (crTableLineInfo.bTargetRelationTeam  ? sortParty  : 0);

    pSkillBaseInfo->bTargetRelationMentor       = crTableLineInfo.bTargetRelationMentor;
    pSkillBaseInfo->bTargetRelationApprentice   = crTableLineInfo.bTargetRelationApprentice;

    pSkillBaseInfo->nTargetMoveStateMask    = crTableLineInfo.nTargetMoveStateMask;

    pSkillBaseInfo->bTargetOnFear           = crTableLineInfo.bTargetOnFear;

    MAP_STRING_LOCAL(SKILL_HORSE_STATE_CONDITION, crTableLineInfo.szTargetHorseStateRequest, pSkillBaseInfo->nTargetHorseStateRequest);

    pSkillBaseInfo->bAutoSelectTarget       = crTableLineInfo.bAutoSelectTarget;
    pSkillBaseInfo->nNPCWeaponType          = crTableLineInfo.nNPCWeaponType;

    MAP_STRING_LOCAL(KSKILL_EFFECT_PLAY_TYPE, crTableLineInfo.szEffectPlayType, pSkillBaseInfo->nEffectPlayType);
    MAP_STRING_LOCAL(KSKILL_COMMON_ACTIVE_MODE, crTableLineInfo.szCommonSkillActiveMode, pSkillBaseInfo->nCommonSkillActiveMode);

    pSkillBaseInfo->dwMapBanMask            = crTableLineInfo.dwMapBanMask;

    strncpy(pSkillBaseInfo->szScriptFile, crTableLineInfo.szScriptFile, sizeof(pSkillBaseInfo->szScriptFile));
    pSkillBaseInfo->szScriptFile[sizeof(pSkillBaseInfo->szScriptFile) - 1] = '\0';

    bResult = true;
Exit0:
    return bResult;
}

template<class Ty>
struct _BuffFreeAttribErase
{
    inline BOOL operator() (Ty& rParam)
    {
        FREE_ATTRIB_LIST(rParam.second.pAttributeListAddToBegin);    
        FREE_ATTRIB_LIST(rParam.second.pEndTimeAttributeAddToBegin);
        return true;
    };
};

void KSkillManager::ClearnUp()
{
    SKILL_LIST::iterator it;

    m_SkillEventList.clear();

    for (it = m_SkillList.begin(); it != m_SkillList.end(); ++it)
    {
        KSkill *pSkillArray = it->second;
        assert(pSkillArray);
        int     nMaxLevel = (int)pSkillArray[0].m_pBaseInfo->dwMaxLevel;
        for (int i = 0; i < nMaxLevel; ++i)
        {
            pSkillArray[i].UnInit();
        }
        KG_DELETE_ARRAY(pSkillArray);
    }
	m_SkillList.clear();
    m_SkillBaseInfoList.clear();

    m_SkillRecipeBaseInfoMap.clear();
    
    m_SkillRecipeCache.UnInit();
}

BOOL KSkillManager::LoadAttribute(ITabFile *piTabFile,  int nRow, int nIndex, int &rnKey, int &rnValueA, int &rnValueB)
{
    BOOL bResult    = false;
    int  nRetCode   = false;
    char szKeyName[ARRAY_LEN];
    char szValueAName[ARRAY_LEN];
    char szValueBName[ARRAY_LEN];
    char szKeyValue[KATTRIBUTE_STRING_LEN];

    if (!nIndex)
    {
        snprintf(szKeyName, sizeof(szKeyName),          "EndTimeAttrib");
        szKeyName[sizeof(szKeyName) - 1] = '\0';

        snprintf(szValueAName, sizeof(szValueAName),    "Value1");
        szValueAName[sizeof(szValueAName) - 1] = '\0';

        snprintf(szValueBName, sizeof(szValueBName),    "Value2");
        szValueBName[sizeof(szValueBName) - 1] = '\0';
    }
    else
    {
        snprintf(szKeyName, sizeof(szKeyName),          "Attrib%d", nIndex);
        szKeyName[sizeof(szKeyName) - 1] = '\0';

        snprintf(szValueAName, sizeof(szValueAName),    "Value%dA", nIndex);
        szValueAName[sizeof(szValueAName) - 1] = '\0';

        snprintf(szValueBName, sizeof(szValueBName),    "Value%dB", nIndex);
        szValueBName[sizeof(szValueBName) - 1] = '\0';
    }

    nRetCode = piTabFile->GetString(nRow, szKeyName, "", szKeyValue, sizeof(szKeyValue));
    KG_PROCESS_ERROR(nRetCode > 0);

    MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szKeyValue, rnKey);

    if (rnKey == atExecuteScript)
    {
        // 特殊处理的魔法属性
        char szFileName[MAX_PATH];
        char szFilePath[MAX_PATH];

        piTabFile->GetString(nRow, szValueAName, "", szFileName, sizeof(szFileName));
        
        snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SCRIPT_DIR, szFileName);
        szFilePath[sizeof(szFilePath) - 1] = '\0';

        rnValueA = (int)g_FileNameHash(szFilePath);
    }
    else
    {
        nRetCode = piTabFile->GetInteger(nRow, szValueAName, 0, &rnValueA);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = piTabFile->GetInteger(nRow, szValueBName, 0, &rnValueB);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::LoadSkillEvent()
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    char        szFileName[MAX_PATH];
    ITabFile*   piTabFile               = NULL;
    int         nHeight                 = 0;

    snprintf(szFileName, sizeof(szFileName), "%s/skill/%s", SETTING_DIR, SKILL_EVENT_TAB_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KSkillManager] Failed to open tab file : %s\n", szFileName);
        goto Exit0;
    }

    nHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 1);

    for (int nLine = 2; nLine <= nHeight; nLine++)
    {
        bRetCode = LoadEventLine(piTabFile, nLine);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    if (!bResult)
    {
        m_SkillEventList.clear();
    }
    return bResult;
}

BOOL KSkillManager::LoadEventLine(ITabFile* piTabFile, int nLine)
{
    BOOL bResult    = false;
    int  nRetCode   = false;
    char szValue[ARRAY_LEN];
    int  nID        = 0;
    int  nTemp      = 0;
    KSKILL_EVENT_MAP::iterator it;

    KSKILL_EVENT Event;

    DECLARE_STRING_MAP_BEGIN(KSKILL_EVENT_TYPE)
        REGISTER_STR_TO_VALUE("Cast",               seCast)
        REGISTER_STR_TO_VALUE("BeCast",             seBeCast)
        REGISTER_STR_TO_VALUE("Hit",                seHit)
        REGISTER_STR_TO_VALUE("BeHit",              seBeHit)
        REGISTER_STR_TO_VALUE("Dodge",              seDodge)
        REGISTER_STR_TO_VALUE("BeDodge",            seBeDodge)
        REGISTER_STR_TO_VALUE("CriticalStrike",     seCriticalStrike)
        REGISTER_STR_TO_VALUE("BeCriticalStrike",   seBeCriticalStrike)
        REGISTER_STR_TO_VALUE("Block",              seBlock)
        REGISTER_STR_TO_VALUE("BeBlock",            seBeBlock)
        REGISTER_STR_TO_VALUE("Accumulate",         seAccumulate)
        REGISTER_STR_TO_VALUE("HitOTAction",        seHitOTAction)
        REGISTER_STR_TO_VALUE("BeHitOTAction",      seBeHitOTAction)
        REGISTER_STR_TO_VALUE("Kill",               seKill)
        REGISTER_STR_TO_VALUE("BeKill",             seBeKill)
        REGISTER_STR_TO_VALUE("OverHeal",           seOverHeal)
        REGISTER_STR_TO_VALUE("BeOverHeal",         seBeOverHeal)
        REGISTER_STR_TO_VALUE("Miss",               seMiss)
        REGISTER_STR_TO_VALUE("BeMiss",             seBeMiss)
    DECLARE_STRING_MAP_END(KSKILL_EVENT_TYPE)


    DECLARE_STRING_MAP_BEGIN(KEVENT_SKILL_CASTER_TARGET_TYPE)
        REGISTER_STR_TO_VALUE("EventCaster",        escttCaster)
        REGISTER_STR_TO_VALUE("EventTarget",        escttTarget)
    DECLARE_STRING_MAP_END(KEVENT_SKILL_CASTER_TARGET_TYPE)

    nRetCode = piTabFile->GetInteger(nLine, "ID", 0, &nID);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nRetCode = piTabFile->GetString(nLine, "EventType", "", szValue, sizeof(szValue));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    MAP_STRING_LOCAL(KSKILL_EVENT_TYPE, szValue, Event.nEventType);

    nRetCode = piTabFile->GetInteger(nLine, "Odds", 0, &Event.nOdds);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nRetCode = piTabFile->GetInteger(nLine, "EventMask1", 0, &nTemp);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    Event.dwEventMask1 = nTemp;

    nRetCode = piTabFile->GetInteger(nLine, "EventMask2", 0, &nTemp);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    Event.dwEventMask2 = nTemp;

    nRetCode = piTabFile->GetInteger(nLine, "SkillID", 0, (int*)&Event.dwSkillID);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nRetCode = piTabFile->GetInteger(nLine, "SkillLevel", 0, (int*)&Event.dwSkillLevel);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nRetCode = piTabFile->GetString(nLine, "SkillCaster", "", szValue, sizeof(szValue));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    MAP_STRING_LOCAL(KEVENT_SKILL_CASTER_TARGET_TYPE, szValue, Event.nCasterType);

    nRetCode = piTabFile->GetString(nLine, "SkillTarget", "", szValue, sizeof(szValue));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    MAP_STRING_LOCAL(KEVENT_SKILL_CASTER_TARGET_TYPE, szValue, Event.nTargetType);

    it = m_SkillEventList.find(nID);
    KGLOG_PROCESS_ERROR(it == m_SkillEventList.end() && "Skill Event ID already exist! ");

    m_SkillEventList[nID] = Event;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::LoadWeaponMapSkill()
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    int         nHeight                 = 0;
    ITabFile*   piTabFile               = NULL;
    char        szFileName[MAX_PATH];

    snprintf(szFileName, sizeof(szFileName), "%s/skill/%s", SETTING_DIR, WEAPON_MAP_SKILL_TAB_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[KSkillManager] Failed to open tab file : %s\n", szFileName);
        goto Exit0;
    }
    
    nHeight = piTabFile->GetHeight();
    for (int nLine = 2; nLine <= nHeight; ++nLine)
    {
        bRetCode = LoadWeaponMapSkillLine(piTabFile, nLine);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    if (!bResult)
    {
        m_WeaponMapSkill.clear();
    }
    return bResult;
}

BOOL KSkillManager::LoadWeaponMapSkillLine(ITabFile* piTabFile, int nLine)
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;
    int     nWeaponType = 0;
    DWORD   dwSkillID   = 0;
    char    szValue[ARRAY_LEN];
    KSKILL  Skill;

    DECLARE_STRING_MAP_BEGIN(WEAPON_DETAIL_TYPE)
        REGISTER_STR_TO_VALUE("wdtWand",                 wdtWand)
        REGISTER_STR_TO_VALUE("wdtSpear",                wdtSpear)
        REGISTER_STR_TO_VALUE("wdtSword",                wdtSword)
        REGISTER_STR_TO_VALUE("wdtFist",                 wdtFist)
        REGISTER_STR_TO_VALUE("wdtDoubleWeapon",         wdtDoubleWeapon)
        REGISTER_STR_TO_VALUE("wdtPen",                  wdtPen)
    DECLARE_STRING_MAP_END(WEAPON_DETAIL_TYPE)
    
    nRetCode = piTabFile->GetString(nLine, "WeaponType", "", szValue, sizeof(szValue));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    MAP_STRING_LOCAL(WEAPON_DETAIL_TYPE, szValue, nWeaponType);

    nRetCode = piTabFile->GetInteger(nLine, "CommonSkillID", 0, (int*)&Skill.dwCommonSkillID);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    
    nRetCode = piTabFile->GetInteger(nLine, "BaseKungfuID", 0, (int*)&Skill.dwBaseKungfuID);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    m_WeaponMapSkill[nWeaponType] = Skill;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::LoadDefenceCoefficientTable(const char* szTabFile)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nTabFileHeight      = 0;
    ITabFile*   piTabFile           = NULL;
    char        szFilePath[MAX_PATH];

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';
    
    ZeroMemory(m_nDefenceCoefficient, sizeof(m_nDefenceCoefficient));

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 0);

    for (int nIndex = 2; (nIndex <= nTabFileHeight) && (nIndex - 2 < MAX_ROW); nIndex++)
    {
        // load a line
        nRetCode = piTabFile->GetInteger(nIndex, "Value1", 0, &m_nDefenceCoefficient[nIndex - 2][0]);
        KGLOG_PROCESS_ERROR((nRetCode > 0) && "Value1");
        
        nRetCode = piTabFile->GetInteger(nIndex, "Value2", 0, &m_nDefenceCoefficient[nIndex - 2][1]);
        KGLOG_PROCESS_ERROR((nRetCode > 0) && "Value2");

        nRetCode = piTabFile->GetInteger(nIndex, "Value3", 0, &m_nDefenceCoefficient[nIndex - 2][2]);
        KGLOG_PROCESS_ERROR((nRetCode > 0) && "Value3");

        nRetCode = piTabFile->GetInteger(nIndex, "Value4", 0, &m_nDefenceCoefficient[nIndex - 2][3]);
        KGLOG_PROCESS_ERROR((nRetCode > 0) && "Value4");

        nRetCode = piTabFile->GetInteger(nIndex, "Value5", 0, &m_nDefenceCoefficient[nIndex - 2][4]);
        KGLOG_PROCESS_ERROR((nRetCode > 0) && "Value5");
        
        // 表中最后一行为防御系数值, 其他行中的列值代表取该列对应的防御系数的概率.
        if (nIndex < MAX_ROW)
        {
            m_nDefenceCoefficient[nIndex - 2][1] += m_nDefenceCoefficient[nIndex - 2][0];
            m_nDefenceCoefficient[nIndex - 2][2] += m_nDefenceCoefficient[nIndex - 2][1];
            m_nDefenceCoefficient[nIndex - 2][3] += m_nDefenceCoefficient[nIndex - 2][2];
            m_nDefenceCoefficient[nIndex - 2][4] += m_nDefenceCoefficient[nIndex - 2][3];
            KGLOG_PROCESS_ERROR(m_nDefenceCoefficient[nIndex - 2][4] == 9999);
        }
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KSkillManager::LoadKungFuExpTable(const char* szTabFile)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nTabFileHeight      = 0;
    ITabFile*   piTabFile           = NULL;
    char        szFilePath[MAX_PATH];

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 0);
    
    m_KungFuExpList.clear();
    for (int nIndex = 2; nIndex <= nTabFileHeight; nIndex++)
    {
        // load a line
        DWORD               dwKungFuID    = 0;
        DWORD               dwKungFuLevel = 0;
        uint64_t    uKey          = 0;
        KKUNG_FU_EXP        KungFuExp;

        nRetCode = piTabFile->GetInteger(nIndex, "KungFuID", 0, (int*)&dwKungFuID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get KungFuID");

        nRetCode = piTabFile->GetInteger(nIndex, "KungFuLevel", 0, (int*)&dwKungFuLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "GetKungFuID");

        nRetCode = piTabFile->GetInteger(nIndex, "ExpAdd", 0, &KungFuExp.nExpAdd);
        KGLOG_PROCESS_ERROR(nRetCode && "Get ExpAdd");

        nRetCode = piTabFile->GetInteger(nIndex, "CostQiPoint", 0, &KungFuExp.nCostQiPoint);
        KGLOG_PROCESS_ERROR(nRetCode && "Get CostQiPoint");
        
        uKey = MAKE_INT64(dwKungFuID, dwKungFuLevel);
        m_KungFuExpList[uKey] = KungFuExp;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KSkillManager::LoadSkillRealizationTable(const char* szTabFile)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nTabFileHeight      = 0;
    ITabFile*   piTabFile           = NULL;
    char        szFilePath[MAX_PATH];

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 0);
    
    for (int nRow = 2; nRow <= nTabFileHeight; nRow++)
    {
        // load a line
        DWORD   dwSkillID           = 0;
        DWORD   dwSkillLevel        = 0;
        DWORD   dwRequireExp        = 0;
        int     nRequirePlayerLevel = 0;
        int     nExpAddOdds         = 0;
        KSkill* pSkill              = NULL;

        nRetCode = piTabFile->GetInteger(nRow, "SkillID", 0, (int*)&dwSkillID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get SkillID");

        nRetCode = piTabFile->GetInteger(nRow, "SkillLevel", 0, (int*)&dwSkillLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "Get SkillLevel");

        nRetCode = piTabFile->GetInteger(nRow, "LevelUpExp", 0, (int*)&dwRequireExp);
        KGLOG_PROCESS_ERROR(nRetCode && "Get RequireExp");

        nRetCode = piTabFile->GetInteger(nRow, "LevelUpPlayerLevel", 0, &nRequirePlayerLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "Get RequirePlayerLevel");
        
        nRetCode = piTabFile->GetInteger(nRow, "ExpAddOdds", 0, &nExpAddOdds);
        KGLOG_PROCESS_ERROR(nRetCode && "Get ExpAddOdds");

        pSkill = GetSkill_RAW_S(dwSkillID, dwSkillLevel);
        if (!pSkill)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "Skill %u level %u does not exist, or it's a KungFu! "
                "Please check your table file. ", 
                szTabFile, nRow, 
                dwSkillID, dwSkillLevel
            );
            goto Exit0;
        }

        if (!pSkill->m_pBaseInfo->bIsExpSkill)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "Skill %u level %u is not a ExpSkill! "
                "Please check your table file. ", 
                szTabFile, nRow, 
                dwSkillID, dwSkillLevel
            );
            goto Exit0;
        }

        if (!dwRequireExp)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "RequireExp can't be \"0\"! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        if (!nRequirePlayerLevel)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "RequirePlayerLevel can't be \"0\"! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        if (!nExpAddOdds)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "ExpAddOdds can't be \"0\"! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        pSkill->m_dwLevelUpExp      = dwRequireExp;
        pSkill->m_nPlayerLevelLimit = nRequirePlayerLevel;
        pSkill->m_nExpAddOdds       = nExpAddOdds;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KSkillManager::LoadSubSkillLearningTable(DWORD dwMasterID, const char* szTabFile)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nTabFileHeight      = 0;
    ITabFile*   piTabFile           = NULL;
    char        szFilePath[MAX_PATH];

    KGLOG_PROCESS_ERROR(dwMasterID);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 0);

    if (nTabFileHeight - 1 > MAX_SKILLS_FOR_EACH_MASTER)
    {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\": "
                "Too many skills for one master, at most %d skills (and levels) allowed! "
                "Please check your table file. ", 
                szTabFile, 
                MAX_SKILLS_FOR_EACH_MASTER
            );
        goto Exit0;
    }
    
    for (int nRow = 2; nRow <= nTabFileHeight; nRow++)
    {
        KSkill*                 pSkill  = NULL;
        KSKILL_MASTER_KEY       SkillMasterKey;
        KSKILL_LEARNING_INFO    SkillLearningInfo;
        KSKILL_LEARNING_INFO_MAP::iterator it;

        SkillMasterKey.dwMasterID = dwMasterID;

        nRetCode = piTabFile->GetInteger(nRow, "SkillID", 0, (int*)&SkillMasterKey.dwSkillID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get SkillID");

        nRetCode = piTabFile->GetInteger(nRow, "SkillLevel", 0, (int*)&SkillMasterKey.dwSkillLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "Get SkillLevel");

        nRetCode = piTabFile->GetInteger(nRow, "RequireExp", 0, (int*)&SkillLearningInfo.dwRequireExp);
        KGLOG_PROCESS_ERROR(nRetCode && "Get RequireExp");

        nRetCode = piTabFile->GetInteger(nRow, "RequirePlayerLevel", 0, &SkillLearningInfo.nRequirePlayerLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "Get RequirePlayerLevel");

        nRetCode = piTabFile->GetInteger(nRow, "ReputationID", 0, (int*)&SkillLearningInfo.dwReputationID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get ReputationID");

        nRetCode = piTabFile->GetInteger(nRow, "ReputationLevel", 0, &SkillLearningInfo.nReputationLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "Get ReputationLevel");

        nRetCode = piTabFile->GetInteger(nRow, "Price", 0, &SkillLearningInfo.nPrice);
        KGLOG_PROCESS_ERROR(nRetCode && "Get Price");

        nRetCode = piTabFile->GetInteger(nRow, "CostTrain", 0, &SkillLearningInfo.nCostTrain);
        KGLOG_PROCESS_ERROR(nRetCode && "Get CostTrain");


        pSkill = GetSkill_RAW(SkillMasterKey.dwSkillID, SkillMasterKey.dwSkillLevel);
        if (!pSkill)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "Skill %u level %u does not exist! "
                "Please check your table file. ", 
                szTabFile, nRow, 
                SkillMasterKey.dwSkillID, SkillMasterKey.dwSkillLevel
            );
            goto Exit0;
        }

        if (!SkillLearningInfo.nRequirePlayerLevel)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "RequirePlayerLevel can't be \"0\"! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        if (SkillLearningInfo.nPrice < 0)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "Price can not be negative! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        it = m_SkillLearningInfoMap.find(SkillMasterKey);
        if (it != m_SkillLearningInfoMap.end())
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "MasterID %u, SkillID %u Level %u already exist! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        m_SkillLearningInfoMap[SkillMasterKey] = SkillLearningInfo;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KSkillManager::LoadSkillLearningTable(const char* szTabFile)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nTabFileHeight      = 0;
    ITabFile*   piTabFile           = NULL;
    char        szFilePath[MAX_PATH];

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 0);
    
    for (int nRow = 2; nRow <= nTabFileHeight; nRow++)
    {
        // load a line
        DWORD       dwMasterID      = ERROR_ID;
        char        szSubTabFile[MAX_PATH];

        nRetCode = piTabFile->GetInteger(nRow, "MasterID", 0, (int*)&dwMasterID);
        KGLOG_PROCESS_ERROR(nRetCode && "Get MasterID");

        nRetCode = piTabFile->GetString(nRow, "TabFile", "", szSubTabFile, MAX_PATH);
        KGLOG_PROCESS_ERROR(nRetCode && "Get TabFile");

        if (!dwMasterID)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "MasterID can't be \"0\"! "
                "Please check your table file. ", 
                szTabFile, nRow
            );
            goto Exit0;
        }

        nRetCode = LoadSubSkillLearningTable(dwMasterID, szSubTabFile);
        if (!nRetCode)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[SO3World][SKILL] In table \"%s\", line %d: "
                "Load subtable failed! MasterID = %u, Subtable = \"%s\". "
                "Please check your table file. ", 
                szTabFile, nRow, 
                dwMasterID, szSubTabFile
            );
        }
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KSkillManager::LoadSkillRecipeBaseLine(
    ITabFile* piTabFile, int nIndex, 
    DWORD* pdwRecipeKey, 
    KSKILL_RECIPE_BASE_INFO* pValue, const KSKILL_RECIPE_BASE_INFO& crDefault
)
{
    BOOL    bResult         = false;
    BOOL    nRetCode        = false;
    DWORD   dwRecipeKey     = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;
    int     nIsSystemRecipe = 0;
    char    szScriptFile[MAX_PATH];

    KG_PROCESS_ERROR(piTabFile);
    KG_PROCESS_ERROR(pdwRecipeKey);
    KG_PROCESS_ERROR(pValue);

    nRetCode = piTabFile->GetInteger(nIndex, "RecipeID", 0, (int*)&dwRecipeID);
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get RecipeID");

    nRetCode = piTabFile->GetInteger(nIndex, "RecipeLevel", 0, (int*)&dwRecipeLevel);
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get RecipeLevel");
    KGLOG_PROCESS_ERROR(dwRecipeLevel > 0);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    nRetCode = piTabFile->GetInteger(nIndex, "IsSystemRecipe", 0, (int*)&nIsSystemRecipe);
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get IsSystemRecipe");
    pValue->bIsSystemRecipe = (nIsSystemRecipe != 0);
    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetInteger(
        nIndex, "SkillID", 
        crDefault.dwSkillID, (int*)&(pValue->dwSkillID)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get SkillID");

    nRetCode = piTabFile->GetInteger(
        nIndex, "SkillLevel", 
        crDefault.dwSkillLevel, (int*)&(pValue->dwSkillLevel)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get SkillLevel");

    nRetCode = piTabFile->GetInteger(
        nIndex, "SkillRecipeType", 
        crDefault.nSkillRecipeType, &(pValue->nSkillRecipeType)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get SkillRecipeType");

    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetInteger(
        nIndex, "PrepareFramesAdd", 
        crDefault.nPrepareFramesAdd, (int*)&(pValue->nPrepareFramesAdd)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get PrepareFramesAdd");

    nRetCode = piTabFile->GetInteger(
        nIndex, "PrepareFramesPercent", 
        crDefault.nPrepareFramesPercent, (int*)&(pValue->nPrepareFramesPercent)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get PrepareFramesPercent");
    
    // -----------------------------------------------------------------------

    for (int i = 0; i < MAX_SKILL_COOL_DOWN_TIMER; i++)
    {
        char szCoolDownName[32];

        sprintf(szCoolDownName, "CoolDownAdd%d", i + 1);

        nRetCode = piTabFile->GetInteger(
            nIndex, szCoolDownName, 
            crDefault.nCoolDownAdd[i], (int*)&(pValue->nCoolDownAdd[i])
        );
        KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get CoolDownAdd");
    }

    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetInteger(
        nIndex, "MinRadiusAdd", 
        crDefault.nMinRadiusAdd, (int*)&(pValue->nMinRadiusAdd)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get MinRadiusAdd");

    nRetCode = piTabFile->GetInteger(
        nIndex, "MaxRadiusAdd", 
        crDefault.nMaxRadiusAdd, (int*)&(pValue->nMaxRadiusAdd)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get MaxRadiusAdd");

    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetInteger(
        nIndex, "CostLifeAddPercent", 
        crDefault.nCostLifeAddPercent, (int*)&(pValue->nCostLifeAddPercent)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get CostLifeAddPercent");

    nRetCode = piTabFile->GetInteger(
        nIndex, "CostManaAddPercent", 
        crDefault.nCostManaAddPercent, (int*)&(pValue->nCostManaAddPercent)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get CostManaAddPercent");

    nRetCode = piTabFile->GetInteger(
        nIndex, "CostStaminaAdd", 
        crDefault.nCostStaminaAdd, (int*)&(pValue->nCostStaminaAdd)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get CostStaminaAdd");

    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetInteger(
        nIndex, "DamageAddPercent", 
        crDefault.nDamageAddPercent, (int*)&(pValue->nDamageAddPercent)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get DamageAddPercent");

    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetString(
        nIndex, "ScriptFile", 
        crDefault.szScriptFile, szScriptFile, sizeof(szScriptFile)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get ScriptFile");

    nRetCode = snprintf(
        pValue->szScriptFile, 
        sizeof(pValue->szScriptFile), 
        "%s/%s", SKILL_SCRIPT_PATH, 
        szScriptFile
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    pValue->szScriptFile[sizeof(pValue->szScriptFile) - 1] = '\0';

    // -----------------------------------------------------------------------
    
    *pdwRecipeKey = dwRecipeKey;
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::LoadSkillRecipeBaseInfo(const char* szTabFile)
{
    BOOL                    bResult             = false;
    int                     nRetCode            = false;
    ITabFile*               piTabFile           = NULL;
    int                     nTabFileHeight      = 0;
    DWORD                   dwRecipeKey         = 0;
    char                    szFilePath[MAX_PATH];
    KSKILL_RECIPE_BASE_INFO DefaultSkillRecipeBaseInfo;

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 1);

    memset(&DefaultSkillRecipeBaseInfo, 0, sizeof(DefaultSkillRecipeBaseInfo));

    nRetCode = LoadSkillRecipeBaseLine(
        piTabFile, 2, 
        &dwRecipeKey, &DefaultSkillRecipeBaseInfo, DefaultSkillRecipeBaseInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    m_SkillRecipeBaseInfoMap.clear();
    for (int nIndex = 3; nIndex <= nTabFileHeight; nIndex++)
    {
        KSKILL_RECIPE_BASE_INFO SkillRecipeBaseInfo;
        // load a line
        nRetCode = LoadSkillRecipeBaseLine(
            piTabFile, nIndex, 
            &dwRecipeKey, &SkillRecipeBaseInfo, DefaultSkillRecipeBaseInfo
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(dwRecipeKey);

        m_SkillRecipeBaseInfoMap.insert(std::make_pair(dwRecipeKey, SkillRecipeBaseInfo));
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_SkillRecipeBaseInfoMap.clear();
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KSkillManager::CheckError()
{
    BOOL    bResult  = false;
    int     nRetCode = false;
    SKILL_LIST::iterator it;

    // 1. 检查子技能调用链是否成环

    // 2. 子技能不能是吟唱技,可能破坏技能状态机

    // 3. 子技能不能是通道技,可能破坏技能状态机

    // 4. 如果一个技能是一个武功并且可以被装备,那么: CastMode必须为CasterSingle, 只能带SysBuff.

    // 5. 技能不能被装备

    // 6. 可装备的武功绑定的Buff是不能存档的

    // 7. 检查技能事件绑定的技能是否正确

    for (it = m_SkillList.begin(); it != m_SkillList.end(); ++it)
    {
        KSkill* pSkillArray = it->second;
        DWORD   dwMaxLevel  = pSkillArray[0].m_pBaseInfo->dwMaxLevel;

        for (DWORD dwIndex = 0; dwIndex < dwMaxLevel; dwIndex++)
        {
            KSkill* pSkill = pSkillArray + dwIndex;
            nRetCode = CheckSkillParam(pSkill);
            KGLOG_PROCESS_ERROR(nRetCode);
            nRetCode = CheckKungfuParam(pSkill);
            KGLOG_PROCESS_ERROR(nRetCode);
            nRetCode = CheckMountRequest(pSkill);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    nRetCode = CheckEvent();
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::CheckSkillParam(KSkill* pSkill)
{
    BOOL    bResult = false;
    KSkill* pKungfu = NULL;

    KG_PROCESS_SUCCESS(pSkill->m_pBaseInfo->dwBelongKungfu == INVALID_SKILL_ID);

    pKungfu = GetSkill_K(pSkill->m_pBaseInfo->dwBelongKungfu, 1);
    if (!pKungfu)
    {
        KGLogPrintf(
            KGLOG_ERR, "[Skill] Skill(%u, %u) Belong's to a kungfu(%u) not exist!\n", 
            pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, pSkill->m_pBaseInfo->dwBelongKungfu
        ); 
        goto Exit0;
    }

    if (pKungfu->m_pBaseInfo->nUIType != pSkill->m_pBaseInfo->nUIType)
    {
        KGLogPrintf(
            KGLOG_ERR, "[Skill] Skill(%u, %u) Belong's to a kungfu(%u), but with a different UI type !\n", 
            pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, pKungfu->m_pBaseInfo->dwSkillID
        ); 
        goto Exit0;
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::CheckKungfuParam(KSkill* pSkill)
{
    BOOL bResult = false;

    // dwBelongKungfu非INVALID_SKILL_ID时是一个技能,而不是武功
    KG_PROCESS_SUCCESS(pSkill->m_pBaseInfo->dwBelongKungfu != INVALID_SKILL_ID);

    if (!pSkill->m_pBaseInfo->bIsExpSkill)
    {
        KGLogPrintf(
            KGLOG_WARNING, "[Skill] Kungfu(%u, %u) should be a exp skill !\n", 
            pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel
        ); 
        goto Exit0;
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::CheckMountRequest(KSkill* pSkill)
{
    BOOL    bResult = false;
    KSkill* pKungfu = NULL;

    // 看是不是技能
    KG_PROCESS_SUCCESS(pSkill->m_pBaseInfo->dwBelongKungfu == INVALID_SKILL_ID);

    // 如果它需求某个武功,则检查该武功是否存在,该武功真的可以装备么,类型是否匹配
    if (pSkill->m_pBaseInfo->dwMountRequestDetail != INVALID_SKILL_ID)
    {
        pKungfu = GetSkill_K(pSkill->m_pBaseInfo->dwMountRequestDetail, 1);
        if (!pKungfu)
        {
            KGLogPrintf(
                KGLOG_WARNING, "[Skill] Skill(%u, %u) need to mount a kungfu(%u) not exist or not a kungfu !", 
                pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, pSkill->m_pBaseInfo->dwMountRequestDetail
            );
            goto Exit0;
        }

        if (!pKungfu->m_pBaseInfo->bIsMountable)
        {
            KGLogPrintf(
                KGLOG_WARNING, "[Skill] Skill(%u, %u) need to mount a kungfu(%u) which can not amount !", 
                pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, pSkill->m_pBaseInfo->dwMountRequestDetail
            );
            goto Exit0;
        }

        if (
            (pSkill->m_pBaseInfo->dwMountRequestType != 0) && 
            (pKungfu->m_pBaseInfo->dwMountType != pSkill->m_pBaseInfo->dwMountRequestType)
        )
        {
            KGLogPrintf(
                KGLOG_WARNING, "[Skill] Skill(%u, %u) need to mount a kungfu(%u) with a unmatchable mounttype !", 
                pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, pSkill->m_pBaseInfo->dwMountRequestDetail
            );
            goto Exit0;
        }
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillManager::CheckEvent()
{
    BOOL bResult = false;
    KSKILL_EVENT_MAP::iterator it;

    for (it = m_SkillEventList.begin(); it != m_SkillEventList.end(); ++it)
    {
        DWORD dwSkillID     = it->second.dwSkillID;
        DWORD dwSkillLevel  = it->second.dwSkillLevel;
        KSkill* pSkill = GetSkill_RAW_S(dwSkillID, dwSkillLevel);
        if (!pSkill)
        {
            KGLogPrintf(
                KGLOG_ERR, "[Skill] Skill event(%d) refers to skill(%u, %u) but not exist !\n",
                it->first, dwSkillID, dwSkillLevel
            );
            goto Exit0;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KSkillManager::OpenVenation(KPlayer* pPlayer, int nVenationID)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;

    assert(pPlayer);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(VENATION_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(nVenationID);

    g_pSO3World->m_ScriptCenter.CallFunction(VENATION_SCRIPT, "OpenVenation", 1);

    g_pSO3World->m_ScriptCenter.GetValuesFromStack("b", &bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

    KG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncPlayerTrain(pPlayer);

    bResult = true;
Exit0:
    return bResult;
}
#endif
