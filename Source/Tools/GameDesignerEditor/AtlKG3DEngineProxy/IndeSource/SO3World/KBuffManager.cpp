#include "stdafx.h"
#include "Engine/KMemory.h"
#include "KAttribute.h"
#include "StrMap.h"
#include "KSkill.h"
#include "KBuffManager.h"

#define BUFF_SCRIPT_PATH            "scripts/skill"
#define PARAM_STRING_LEN            64

#define BUFF_TAB                    "skill/Buff.tab"
#define BUFF_RECIPE_TAB             "skill/BuffRecipe.tab"
#define BUFF_DECAY_TAB              "skill/DecayType.tab"

#ifdef _SERVER
#define BUFF_RECIPE_CACHE_SIZE      8192
#define BUFF_RECIPE_CACHE_SHRINK    32
#else
#define BUFF_RECIPE_CACHE_SIZE      256
#define BUFF_RECIPE_CACHE_SHRINK    4
#endif

#define READ_PERSIST_MOVE_STATE(InfoObj, MoveState)                         \
    do {                                                                    \
        int nRPMSTmpFlag = 0;                                               \
        piTabFile->GetInteger(nIndex, #MoveState, 0, &nRPMSTmpFlag);        \
        assert(nRPMSTmpFlag == 1 || nRPMSTmpFlag == 0);                     \
        InfoObj.m_nPersistMoveStateMask |= nRPMSTmpFlag << cms##MoveState;  \
    } while(0)

#define MAKE_LONG_LONG(__A__, __B__) ((((unsigned long long)(__A__)) << 32) + ((unsigned long long)(__B__)))

BOOL KBuffManager::Init()
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;

    m_dwBuffSNGenerator = 1;

    m_BuffInfoMap.clear();
    bRetCode = LoadBuffInfo(BUFF_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_BuffRecipInfoMap.clear();
    bRetCode = LoadBuffRecipeInfo(BUFF_RECIPE_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadBuffDecayInfo(BUFF_DECAY_TAB);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_BuffRecipeCache.Init(BUFF_RECIPE_CACHE_SIZE, BUFF_RECIPE_CACHE_SHRINK);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckBuffExclude();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        CleanUp();
    }
	return bResult;
}

BOOL KBuffManager::UnInit()
{
    CleanUp();
    return true;
}

KBuff* KBuffManager::GetBuff_RAW(DWORD dwID, int nLevel)
{
    unsigned long long          ullKey  = MAKE_LONG_LONG(dwID, nLevel);
    KBUFF_INFO_MAP::iterator    it      = m_BuffInfoMap.find(ullKey);

    if (it != m_BuffInfoMap.end())
        return &(it->second);

    return NULL;
}

KBuff* KBuffManager::GetBuff(const KBUFF_RECIPE_KEY& crBuffRecipeKey)
{
    KBuff*                      pResult         = NULL;
    KBuff*                      pBuff           = NULL;
    BOOL                        bRetCode        = false;
    BOOL                        bRecipeFlag     = false;
    BOOL                        bNewCacheNode   = false;
    KBuff*                      pBuffBaseInfo   = NULL;
    KBUFF_INFO_MAP::iterator    itBuffInfo;
    unsigned long long          ullKey          = MAKE_LONG_LONG(crBuffRecipeKey.dwID, crBuffRecipeKey.nLevel);

    for (int i = MAX_BUFF_REICPE_COUNT - 1; i >= 0; i--)
    {
        if (crBuffRecipeKey.dwRecipeKey[i] != 0)
        {
            bRecipeFlag = true;
            break;
        }
    }

    if (!bRecipeFlag)
    {
        pResult = GetBuff_RAW(crBuffRecipeKey.dwID, crBuffRecipeKey.nLevel);
        goto Exit0;
    }

    pBuff = m_BuffRecipeCache.Find(crBuffRecipeKey);
    KG_PROCESS_SUCCESS(pBuff);

    // If not in cache
    pBuff = m_BuffRecipeCache.Insert(crBuffRecipeKey);
    KGLOG_PROCESS_ERROR(pBuff);

    pBuff->m_pBeginAttribList     = NULL;
    pBuff->m_pActiveAttribList    = NULL;
    pBuff->m_pEndTimeAttribList   = NULL;

    bNewCacheNode = true;

    itBuffInfo = m_BuffInfoMap.find(ullKey);
    KGLOG_PROCESS_ERROR(itBuffInfo != m_BuffInfoMap.end());
    pBuffBaseInfo = &itBuffInfo->second;
    KG_ASSERT_EXIT(pBuffBaseInfo);

    *pBuff = *pBuffBaseInfo;
    pBuff->m_dwSerialNumber = m_dwBuffSNGenerator++;

    pBuff->m_pBeginAttribList     = NULL;
    pBuff->m_pActiveAttribList    = NULL;
    pBuff->m_pEndTimeAttribList   = NULL;

    for (int i = 0; i < MAX_BUFF_REICPE_COUNT; ++i)
    {
        KBUFF_RECIPE_INFO* pBuffRecipe = NULL;

        if (crBuffRecipeKey.dwRecipeKey[i] == 0)
            continue;

        pBuffRecipe = GetBuffRecipeInfo(crBuffRecipeKey.dwRecipeKey[i]);
        KGLOG_PROCESS_ERROR(pBuffRecipe);

        pBuff->m_nIntensity         += pBuffRecipe->nIntensityAdd;
        pBuff->m_nActiveCount       += pBuffRecipe->nCountAdd;
        pBuff->m_nIntervalFrame     += pBuffRecipe->nIntervalFrameAdd;
        pBuff->m_nDamageAddPercent  += pBuffRecipe->nDamageAddPercent;

        bRetCode = CloneAttribListToEnd(&(pBuff->m_pBeginAttribList), pBuffRecipe->pBeginAttributeListAdd);
        KG_PROCESS_ERROR(bRetCode);

        bRetCode = CloneAttribListToEnd(&(pBuff->m_pActiveAttribList), pBuffRecipe->pActiveAttributeListAdd);
        KG_PROCESS_ERROR(bRetCode);

        bRetCode = CloneAttribListToEnd(&(pBuff->m_pEndTimeAttribList), pBuffRecipe->pEndTimeAttributeListAdd);
        KG_PROCESS_ERROR(bRetCode);
    }

    pBuff->m_nActiveCount   = max(1, pBuff->m_nActiveCount);
    pBuff->m_nIntervalFrame = max(1, pBuff->m_nIntervalFrame);

    bRetCode = CloneAttribListToEnd(&(pBuff->m_pBeginAttribList), pBuffBaseInfo->m_pBeginAttribList);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pBuff->m_pActiveAttribList), pBuffBaseInfo->m_pActiveAttribList);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = CloneAttribListToEnd(&(pBuff->m_pEndTimeAttribList), pBuffBaseInfo->m_pEndTimeAttribList);
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    pResult = pBuff;
Exit0:
    if (!pResult && bNewCacheNode)
    {
        m_BuffRecipeCache.Erase(crBuffRecipeKey);
        bNewCacheNode = false;
    }
    return pResult;
}

BOOL KBuffManager::GetBuffDecayInfo(DWORD dwDecayType, int* pnRetDecayFrame, int* pnRetImmunityFrame)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(dwDecayType >= 0 && dwDecayType < MAX_BUFF_DECAY_TYPE);
    assert(pnRetDecayFrame);
    assert(pnRetImmunityFrame);

    *pnRetDecayFrame    = m_BuffDecayTable[dwDecayType].nDecayFrame;
    *pnRetImmunityFrame = m_BuffDecayTable[dwDecayType].nImmunityFrame;

    bResult = true;
Exit0:
    return bResult;
}

KBUFF_RECIPE_INFO* KBuffManager::GetBuffRecipeInfo(DWORD dwRecipeKey)
{
    KBUFF_RECIPE_INFO*              pResult = NULL;
    KBUFF_RECIPE_INFO_MAP::iterator itRecipe;

    itRecipe = m_BuffRecipInfoMap.find(dwRecipeKey);
    KGLOG_PROCESS_ERROR(itRecipe != m_BuffRecipInfoMap.end());

    pResult = &(itRecipe->second);
Exit0:
    return pResult;
}

BOOL LoadAttributeEx(ITabFile *piTabFile,  int nRow, const char* szPrefix, int nIndex, int &rnKey, int &rnValueA, int &rnValueB)
{
    BOOL bResult    = false;
    int  nRetCode   = false;
    char szKeyName[PARAM_STRING_LEN];
    char szValueAName[PARAM_STRING_LEN];
    char szValueBName[PARAM_STRING_LEN];
    char szKeyValue[KATTRIBUTE_STRING_LEN];

    assert(piTabFile);
    assert(szPrefix);

    snprintf(szKeyName, sizeof(szKeyName),          "%sAttrib%d", szPrefix, nIndex);
    szKeyName[sizeof(szKeyName) - 1] = '\0';

    snprintf(szValueAName, sizeof(szValueAName),    "%sValue%dA", szPrefix, nIndex);
    szValueAName[sizeof(szValueAName) - 1] = '\0';

    snprintf(szValueBName, sizeof(szValueBName),    "%sValue%dB", szPrefix, nIndex);
    szValueBName[sizeof(szValueBName) - 1] = '\0';

    nRetCode = piTabFile->FindColumn(szKeyName);
    KG_PROCESS_ERROR(nRetCode >= 0);

    nRetCode = piTabFile->GetString(nRow, szKeyName, "", szKeyValue, sizeof(szKeyValue));
    KG_PROCESS_ERROR(nRetCode > 0);

    MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szKeyValue, rnKey);

    if (rnKey == atExecuteScript)
    {
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

BOOL KBuffManager::LoadBuffInfo(const char* szTabFile)
{
	BOOL                bResult                 = false;
	int                 nRetCode                = 0;
	ITabFile*           piTabFile               = NULL;
	int                 nTabFileHeight          = 0;
	int                 nKey                    = 0;
	int                 nValue1                 = 0;
	int                 nValue2                 = 0;
	KAttribute*         pTmpAttrib              = NULL;
	KAttribute*         pTmpBeginAttribList     = NULL;
	KAttribute*         pTmpActiveAttribList    = NULL;
	KAttribute*         pTmpEndTimeAttribList   = NULL;
	KBuff               BuffInfo;
	char                szFilePath[MAX_PATH];
	char                szScriptFile[MAX_PATH];
	char                szValue[PARAM_STRING_LEN];

	DECLARE_STRING_MAP_BEGIN(KBUFF_TYPE)
	REGISTER_STR_TO_VALUE("Invalid",        btpInvalid)
		REGISTER_STR_TO_VALUE("Stun",           btpStun)
		REGISTER_STR_TO_VALUE("Daze",           btpDaze)
		REGISTER_STR_TO_VALUE("Halt",           btpHalt)
		REGISTER_STR_TO_VALUE("Fear",           btpFear)
		DECLARE_STRING_MAP_END(KBUFF_TYPE)

	DECLARE_STRING_MAP_BEGIN(KBUFF_REPRESENT_POS)
	REGISTER_STR_TO_VALUE("Trample",        brpTrample)
		REGISTER_STR_TO_VALUE("Body",           brpBody)
		REGISTER_STR_TO_VALUE("TopHead",        brpTopHead)
		DECLARE_STRING_MAP_END(KBUFF_REPRESENT_POS)

	DECLARE_STRING_MAP_BEGIN(KSKILL_KIND_TYPE)
	REGISTER_STR_TO_VALUE("Physics",        sktPhysics)         
		REGISTER_STR_TO_VALUE("SolarMagic",     sktSolarMagic)      
		REGISTER_STR_TO_VALUE("NeutralMagic",   sktNeutralMagic)    
		REGISTER_STR_TO_VALUE("LunarMagic",     sktLunarMagic)      
		REGISTER_STR_TO_VALUE("Poison",         sktPoison)                   
		REGISTER_STR_TO_VALUE("None",           sktNone)                   
		DECLARE_STRING_MAP_END(KSKILL_KIND_TYPE)

	DECLARE_STRING_MAP_BEGIN(KSKILL_FUNCTION_TYPE)
	REGISTER_STR_TO_VALUE("Normal",         sftNormal)
		REGISTER_STR_TO_VALUE("Slow",           sftSlow)
		REGISTER_STR_TO_VALUE("Fear",           sftFear)
		REGISTER_STR_TO_VALUE("Halt",           sftHalt)
		REGISTER_STR_TO_VALUE("Silence",        sftSilence)
		REGISTER_STR_TO_VALUE("Chaos",          sftChaos)
		REGISTER_STR_TO_VALUE("Charm",          sftCharm)
		REGISTER_STR_TO_VALUE("Stun",           sftStun)
		REGISTER_STR_TO_VALUE("Enmity",         sftEnmity)
		REGISTER_STR_TO_VALUE("Blooding",       sftBoolding)
		REGISTER_STR_TO_VALUE("Daze",           sftDaze)
		REGISTER_STR_TO_VALUE("Damage",         sftDamage)
		DECLARE_STRING_MAP_END(KSKILL_FUNCTION_TYPE)

#ifdef _SERVER
	DECLARE_STRING_MAP_BEGIN(KBUFF_PERSIST_SHIELD_TYPE)
	REGISTER_STR_TO_VALUE("None",           pmsNone)

		REGISTER_STR_TO_VALUE("GlobalMana",     pmsGlobalMana)
		REGISTER_STR_TO_VALUE("PhysicsMana",    pmsPhysicsMana)
		REGISTER_STR_TO_VALUE("SolarMana",      pmsSolarMana)
		REGISTER_STR_TO_VALUE("NeutralMana",    pmsNeutralMana)
		REGISTER_STR_TO_VALUE("LunarMana",      pmsLunarMana)
		REGISTER_STR_TO_VALUE("PoisonMana",     pmsPoisonMana)

		REGISTER_STR_TO_VALUE("GlobalAbsorb",   pmsGlobalAbsorb)
		REGISTER_STR_TO_VALUE("PhysicsAbsorb",  pmsPhysicsAbsorb)
		REGISTER_STR_TO_VALUE("SolarAbsorb",    pmsSolarAbsorb)
		REGISTER_STR_TO_VALUE("NeutralAbsorb",  pmsNeutralAbsorb)
		REGISTER_STR_TO_VALUE("LunarAbsorb",    pmsLunarAbsorb)
		REGISTER_STR_TO_VALUE("PoisonAbsorb",   pmsPoisonAbsorb)
		DECLARE_STRING_MAP_END(KBUFF_PERSIST_SHIELD_TYPE)

	DECLARE_STRING_MAP_BEGIN(KBUFF_PERSIST_ON_FIGHT)
	REGISTER_STR_TO_VALUE("Any",            pofAny)
		REGISTER_STR_TO_VALUE("Fight",          pofFight)
		REGISTER_STR_TO_VALUE("NotFight",       pofNotFight)
		DECLARE_STRING_MAP_END(KBUFF_PERSIST_ON_FIGHT)

	DECLARE_STRING_MAP_BEGIN(KBUFF_PERSIST_ON_HORSE)
	REGISTER_STR_TO_VALUE("Any",            pohAny)
		REGISTER_STR_TO_VALUE("Horse",          pohHorse)
		REGISTER_STR_TO_VALUE("NotHorse",       pohNotHorse)
		DECLARE_STRING_MAP_END(KBUFF_PERSIST_ON_HORSE)

	DECLARE_STRING_MAP_BEGIN(KBUFF_PERSIST_ON_TERRAIN)
	REGISTER_STR_TO_VALUE("Any",            potAny)
		REGISTER_STR_TO_VALUE("Land",           potLand)
		REGISTER_STR_TO_VALUE("Water",          potWater)
		DECLARE_STRING_MAP_END(KBUFF_PERSIST_ON_TERRAIN)
#endif

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
	szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	if (!piTabFile)
	{
		KGLogPrintf(KGLOG_ERR, "[Buff] Unable to open table file \"%s\"\n", szFilePath);
		goto Exit0;
	}

	nTabFileHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nTabFileHeight > 1);

	memset(&BuffInfo, 0, sizeof(BuffInfo));

	for (int nIndex = 2; nIndex <= nTabFileHeight; nIndex++)
	{
		// load a line
		KBUFF_INFO_MAP::iterator    it;
		unsigned long long          ullKey          = 0;
		int                         nActiveCount    = 0;
		int                         nIntervalFrame  = 0;

		memset(&BuffInfo, 0, sizeof(BuffInfo));

		nRetCode = piTabFile->GetInteger(nIndex, "ID", 0, (int*)&BuffInfo.m_dwID);
		KGLOG_PROCESS_ERROR((nRetCode > 0) && "[Buff] Get ID");

		nRetCode = piTabFile->GetInteger(nIndex, "Level", 0, (int*)&BuffInfo.m_nLevel);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get Level");

		ullKey = MAKE_LONG_LONG(BuffInfo.m_dwID, BuffInfo.m_nLevel);

		it = m_BuffInfoMap.find(ullKey);
		KGLOG_PROCESS_ERROR(it == m_BuffInfoMap.end());

		nRetCode = piTabFile->GetString(nIndex, "KindType", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get KindType");
		MAP_STRING_LOCAL(KSKILL_KIND_TYPE, szValue, BuffInfo.m_nKindType);

		nRetCode = piTabFile->GetString(nIndex, "FunctionType", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get FunctionType");
		MAP_STRING_LOCAL(KSKILL_FUNCTION_TYPE, szValue, BuffInfo.m_nFunctionType);

		nRetCode = piTabFile->GetString(nIndex, "BuffType", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get BuffType");
		MAP_STRING_LOCAL(KBUFF_TYPE, szValue, BuffInfo.m_nBuffType);

		nRetCode = piTabFile->GetInteger(nIndex, "AppendType", 0, (int*)&BuffInfo.m_nAppendType);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get AppendType");

		nRetCode = piTabFile->GetInteger(nIndex, "DetachType", 0, (int*)&BuffInfo.m_nDetachType);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get DetachType");

		nRetCode = piTabFile->GetInteger(nIndex, "DecayType", -1, &BuffInfo.m_nDecayType);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get DecayType");

		nRetCode = piTabFile->GetString(nIndex, "RepresentPos", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get RepresentPos");
		MAP_STRING_LOCAL(KBUFF_REPRESENT_POS, szValue, BuffInfo.m_nRepresentPos);

		nRetCode = piTabFile->GetInteger(nIndex, "RepresentID", 0, (int*)&BuffInfo.m_dwRepresentID);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get RepresentID");
		KGLOG_PROCESS_ERROR(BuffInfo.m_dwRepresentID < MAX_BUFF_REPRESENT_ID);

		nRetCode = piTabFile->GetInteger(nIndex, "Intensity", 0, (int *)&BuffInfo.m_nIntensity);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get Intensity");

		nRetCode = piTabFile->GetInteger(nIndex, "IsStackable", 0, (int *)&BuffInfo.m_bIsStackable);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get IsStackable");

		nRetCode = piTabFile->GetInteger(nIndex, "MaxStackNum", 0, (int *)&BuffInfo.m_nMaxStackNum);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get MaxStackNum");
		KGLOG_PROCESS_ERROR((!BuffInfo.m_bIsStackable) || (BuffInfo.m_nMaxStackNum > 0 && BuffInfo.m_nMaxStackNum < SCHAR_MAX));

		nRetCode = piTabFile->GetInteger(nIndex, "Count", 0, (int *)&nActiveCount);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get Count");
		KGLOG_PROCESS_ERROR(nActiveCount > 0);
		BuffInfo.m_nActiveCount = nActiveCount;

		nRetCode = piTabFile->GetInteger(nIndex, "Interval", 0, (int *)&nIntervalFrame);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get Interval");
		KGLOG_PROCESS_ERROR(nIntervalFrame >= 0);
		BuffInfo.m_nIntervalFrame = (!nIntervalFrame) ? INT_MAX : nIntervalFrame;

		nRetCode = piTabFile->GetInteger(nIndex, "Exclude", 0, (int *)&BuffInfo.m_bExclude);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get Exclude");

		nRetCode = piTabFile->GetInteger(nIndex, "GlobalExclude", 0, (int *)&BuffInfo.m_bGlobalExclude);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get GlobalExclude");

		nRetCode = piTabFile->GetInteger(nIndex, "UniqueTarget", 0, (int *)&BuffInfo.m_bUniqueTarget);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get UniqueTarget");

		nRetCode = piTabFile->GetInteger(nIndex, "CanCancel", 0, (int *)&BuffInfo.m_bCanCancel);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get CanCancel");

		nRetCode = piTabFile->GetInteger(nIndex, "IsCountable", 0, (int *)&BuffInfo.m_bIsCountable);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get IsCountable");

		nRetCode = piTabFile->GetInteger(nIndex, "Save", 0, (int *)&BuffInfo.m_bSave);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get Save");

		nRetCode = piTabFile->GetInteger(nIndex, "IsOffLineClock", 0, (int *)&BuffInfo.m_bIsOffLineClock);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get IsOffLineClock");

#ifdef _SERVER
		BuffInfo.m_nPersistMoveStateMask = 0;

		nRetCode = piTabFile->GetInteger(nIndex, "MoveStateMask", 0, &BuffInfo.m_nPersistMoveStateMask);
		KGLOG_PROCESS_ERROR(nRetCode && "Get MoveStateMask");

		nRetCode = piTabFile->GetString(nIndex, "OnManaShield", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get OnManaShield");
		MAP_STRING_LOCAL(KBUFF_PERSIST_SHIELD_TYPE, szValue, BuffInfo.m_nPersistManaShield);

		nRetCode = piTabFile->GetString(nIndex, "OnFight", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get OnFight");
		MAP_STRING_LOCAL(KBUFF_PERSIST_ON_FIGHT, szValue, BuffInfo.m_nPersistFight);

		nRetCode = piTabFile->GetString(nIndex, "OnHorse", "", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get OnHorse");
		MAP_STRING_LOCAL(KBUFF_PERSIST_ON_HORSE, szValue, BuffInfo.m_nPersistHorse);

		nRetCode = piTabFile->GetString(nIndex, "OnTerrain","", szValue, sizeof(szValue));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get OnTerrain");
		MAP_STRING_LOCAL(KBUFF_PERSIST_ON_TERRAIN, szValue, BuffInfo.m_nPersistTerrain);

		nRetCode = piTabFile->GetInteger(nIndex, "FormationLeader", 0, &nValue1);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get FormationLeader");
		BuffInfo.m_bPersistFormationLeader = (nValue1 == 0 ? false : true);

		nRetCode = piTabFile->GetInteger(nIndex, "MapBanMask", 0, (int*)&BuffInfo.m_dwMapBanMask);
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get MapBanMask");
#endif


		nRetCode = piTabFile->GetString(nIndex, "ScriptFile", "", szScriptFile, sizeof(szScriptFile));
		KGLOG_PROCESS_ERROR(nRetCode && "[Buff] Get ScriptFile");

		snprintf(BuffInfo.m_szScript, sizeof(BuffInfo.m_szScript), "%s/%s", BUFF_SCRIPT_PATH, szScriptFile);
		BuffInfo.m_szScript[sizeof(BuffInfo.m_szScript) - 1] = 0;

		// BeginAttribute
		for (int nAttribIndex = 1; nAttribIndex; nAttribIndex++)
		{
			nRetCode = LoadAttributeEx(piTabFile, nIndex, "Begin", nAttribIndex, nKey, nValue1, nValue2);
			if (!nRetCode)
				break;

			pTmpAttrib = KMemory::New<KAttribute>();
			KGLOG_PROCESS_ERROR(pTmpAttrib);

			pTmpAttrib->nKey    = nKey;
			pTmpAttrib->nValue1 = nValue1;
			pTmpAttrib->nValue2 = nValue2;
			pTmpAttrib->pNext   = NULL;

			ADD_ATTRIBUTE_TO_END(pTmpBeginAttribList, pTmpAttrib);
			pTmpAttrib          = NULL;
		}

		// ActiveAttribute
		for (int nAttribIndex = 1; nAttribIndex; nAttribIndex++)
		{
			nRetCode = LoadAttributeEx(piTabFile, nIndex, "Active", nAttribIndex, nKey, nValue1, nValue2);
			if (!nRetCode)
				break;

			pTmpAttrib = KMemory::New<KAttribute>();
			KGLOG_PROCESS_ERROR(pTmpAttrib);

			pTmpAttrib->nKey    = nKey;
			pTmpAttrib->nValue1 = nValue1;
			pTmpAttrib->nValue2 = nValue2;
			pTmpAttrib->pNext   = NULL;

			ADD_ATTRIBUTE_TO_END(pTmpActiveAttribList, pTmpAttrib);
			pTmpAttrib          = NULL;
		}

		// EndTimeAttribute
		for (int nAttribIndex = 1; nAttribIndex; nAttribIndex++)
		{
			nRetCode = LoadAttributeEx(piTabFile, nIndex, "EndTime", nAttribIndex, nKey, nValue1, nValue2);
			if (!nRetCode)
				break;

			pTmpAttrib = KMemory::New<KAttribute>();
			KGLOG_PROCESS_ERROR(pTmpAttrib);

			pTmpAttrib->nKey    = nKey;
			pTmpAttrib->nValue1 = nValue1;
			pTmpAttrib->nValue2 = nValue2;
			pTmpAttrib->pNext   = NULL;

			ADD_ATTRIBUTE_TO_END(pTmpEndTimeAttribList, pTmpAttrib);
			pTmpAttrib          = NULL;
		}

		BuffInfo.m_pBeginAttribList   = pTmpBeginAttribList;
		pTmpBeginAttribList         = NULL;

		BuffInfo.m_pActiveAttribList  = pTmpActiveAttribList;
		pTmpActiveAttribList        = NULL;

		BuffInfo.m_pEndTimeAttribList = pTmpEndTimeAttribList;
		pTmpEndTimeAttribList       = NULL;

		BuffInfo.m_dwSerialNumber = m_dwBuffSNGenerator++;
		m_BuffInfoMap[ullKey]   = BuffInfo;
	}

	if (BuffInfo.m_nIntervalFrame == INT_MAX)
	{
		KGLOG_PROCESS_ERROR(BuffInfo.m_nDecayType < 0 || BuffInfo.m_nDecayType >= MAX_BUFF_DECAY_TYPE);
	}

	bResult = true;
Exit0:
	if (!bResult)
	{
		for (KBUFF_INFO_MAP::iterator it = m_BuffInfoMap.begin(); it != m_BuffInfoMap.end(); ++it)
		{
			KBuff* pBuff = &it->second;

			FREE_ATTRIB_LIST(pBuff->m_pBeginAttribList);
			FREE_ATTRIB_LIST(pBuff->m_pActiveAttribList);
			FREE_ATTRIB_LIST(pBuff->m_pEndTimeAttribList);
		}
		m_BuffInfoMap.clear();
	}

	FREE_ATTRIB_LIST(pTmpBeginAttribList);
	FREE_ATTRIB_LIST(pTmpActiveAttribList);
	FREE_ATTRIB_LIST(pTmpEndTimeAttribList);
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KBuffManager::LoadBuffRecipeInfo(const char* szTabFile)
{
    BOOL                bResult             = false;
    int                 nRetCode            = false;
    ITabFile*           piTabFile           = NULL;
    int                 nTabFileHeight      = 0;
    DWORD               dwRecipeKey         = 0;
    KBUFF_RECIPE_INFO   DefaultBuffRecipeBaseInfo;
    KBUFF_RECIPE_INFO   BuffRecipeInfo;
    char                szFilePath[MAX_PATH];

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, szTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    if (!piTabFile)
    {
        KGLogPrintf(KGLOG_ERR, "[Buff] Unable to open table file \"%s\"\n", szFilePath);
        goto Exit0;
    }

    nTabFileHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabFileHeight > 1);

    memset(&DefaultBuffRecipeBaseInfo, 0, sizeof(DefaultBuffRecipeBaseInfo));

    nRetCode = LoadBuffRecipeLine(piTabFile, 2, &dwRecipeKey, &DefaultBuffRecipeBaseInfo, DefaultBuffRecipeBaseInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_BuffRecipInfoMap.clear();
    for (int nIndex = 3; (nIndex <= nTabFileHeight); nIndex++)
    {
        nRetCode = LoadBuffRecipeLine(piTabFile, nIndex, &dwRecipeKey, &BuffRecipeInfo, DefaultBuffRecipeBaseInfo);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(dwRecipeKey);

        m_BuffRecipInfoMap.insert(std::make_pair(dwRecipeKey, BuffRecipeInfo));
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_BuffRecipInfoMap.clear();
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KBuffManager::LoadBuffRecipeLine(ITabFile* piTabFile, int nIndex, DWORD* pdwRecipeKey, KBUFF_RECIPE_INFO* pValue, const KBUFF_RECIPE_INFO& crDefault)
{
    BOOL        bResult                 = false;
    BOOL        nRetCode                = false;
    DWORD       dwRecipeKey             = 0;
    DWORD       dwRecipeID              = 0;
    DWORD       dwRecipeLevel           = 0;
    int         nKey                    = atInvalid;
    int         nValue1                 = 0;
    int         nValue2                 = 0;
    KAttribute* pTmpAttrib              = NULL;
    KAttribute* pTmpBeginAttribList     = NULL;
    KAttribute* pTmpActiveAttribList    = NULL;
    KAttribute* pTmpEndTimeAttribList   = NULL;

    KG_PROCESS_ERROR(piTabFile);
    KG_PROCESS_ERROR(pdwRecipeKey);
    KG_PROCESS_ERROR(pValue);

    nRetCode = piTabFile->GetInteger(nIndex, "RecipeID", 0, (int*)&dwRecipeID);
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get RecipeID");

    nRetCode = piTabFile->GetInteger(nIndex, "RecipeLevel", 0, (int*)&dwRecipeLevel);
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get RecipeLevel");

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    // -----------------------------------------------------------------------

    nRetCode = piTabFile->GetInteger(nIndex, "BuffID", crDefault.dwID, (int*)&(pValue->dwID));
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get BuffID");

    nRetCode = piTabFile->GetInteger(nIndex, "BuffLevel", crDefault.nLevel, (int*)&(pValue->nLevel));
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get BuffLevel");

    nRetCode = piTabFile->GetInteger(nIndex, "IntensityAdd", crDefault.nIntensityAdd, (int*)&(pValue->nIntensityAdd));
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get IntensityAdd");

    nRetCode = piTabFile->GetInteger(nIndex, "CountAdd", crDefault.nCountAdd, (int*)&(pValue->nCountAdd));
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get CountAdd");

    nRetCode = piTabFile->GetInteger(nIndex, "IntervalFrameAdd", crDefault.nIntervalFrameAdd, (int*)&(pValue->nIntervalFrameAdd));
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get IntervalFrameAdd");

    nRetCode = piTabFile->GetInteger(nIndex, "DamageAddPercent", crDefault.nDamageAddPercent, (int*)&(pValue->nDamageAddPercent));
    KGLOG_PROCESS_ERROR(nRetCode && "[Recipe] Get DamageAddPercent");

    // -----------------------------------------------------------------------

    // Begin
    for (int nAttribIndex = 1; nAttribIndex; nAttribIndex++)
    {
        nRetCode = LoadAttributeEx(piTabFile, nIndex, "Begin", nAttribIndex, nKey, nValue1, nValue2);
        if (!nRetCode)
            break;

        pTmpAttrib = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pTmpAttrib);

        pTmpAttrib->nKey    = nKey;
        pTmpAttrib->nValue1 = nValue1;
        pTmpAttrib->nValue2 = nValue2;
        pTmpAttrib->pNext   = NULL;

        ADD_ATTRIBUTE_TO_END(pTmpBeginAttribList, pTmpAttrib);
        pTmpAttrib = NULL;
    }

    // Active
    for (int nAttribIndex = 1; nAttribIndex; nAttribIndex++)
    {
        nRetCode = LoadAttributeEx(piTabFile, nIndex, "Active", nAttribIndex, nKey, nValue1, nValue2);
        if (!nRetCode)
            break;

        pTmpAttrib = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pTmpAttrib);

        pTmpAttrib->nKey    = nKey;
        pTmpAttrib->nValue1 = nValue1;
        pTmpAttrib->nValue2 = nValue2;
        pTmpAttrib->pNext   = NULL;

        ADD_ATTRIBUTE_TO_END(pTmpActiveAttribList, pTmpAttrib);
        pTmpAttrib = NULL;
    }

    // EndTime
    for (int nAttribIndex = 1; nAttribIndex; nAttribIndex++)
    {
        nRetCode = LoadAttributeEx(piTabFile, nIndex, "EndTime", nAttribIndex, nKey, nValue1, nValue2);
        if (!nRetCode)
            break;

        pTmpAttrib = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pTmpAttrib);

        pTmpAttrib->nKey    = nKey;
        pTmpAttrib->nValue1 = nValue1;
        pTmpAttrib->nValue2 = nValue2;
        pTmpAttrib->pNext   = NULL;

        ADD_ATTRIBUTE_TO_END(pTmpEndTimeAttribList, pTmpAttrib);
        pTmpAttrib = NULL;
    }

    pValue->pBeginAttributeListAdd = pTmpBeginAttribList;
    pTmpBeginAttribList = NULL;

    pValue->pActiveAttributeListAdd = pTmpActiveAttribList;
    pTmpActiveAttribList = NULL;

    pValue->pEndTimeAttributeListAdd = pTmpEndTimeAttribList;
    pTmpEndTimeAttribList = NULL;

    // -----------------------------------------------------------------------

    *pdwRecipeKey = dwRecipeKey;

    bResult = true;
Exit0:
    FREE_ATTRIB_LIST(pTmpBeginAttribList);
    FREE_ATTRIB_LIST(pTmpActiveAttribList);
    FREE_ATTRIB_LIST(pTmpEndTimeAttribList);
    return bResult;
}

BOOL KBuffManager::LoadBuffDecayInfo(const char* cszTabFile)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    ITabFile*   piTabFile           = NULL;
    int         nHeight             = 0;
    DWORD       dwTmpDecayType      = 0;
    int         nTmpDecayFrame      = 0;
    int         nTmpImunityFrame    = 0;
    char        szFilePath[MAX_PATH];

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, cszTabFile);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    memset(m_BuffDecayTable, 0, sizeof(m_BuffDecayTable));

    piTabFile = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 1);

    for (int i = 2; i <= nHeight; i++)
    {
        bRetCode = piTabFile->GetInteger(i, "DecayType", -1, (int*)&dwTmpDecayType);
        KGLOG_PROCESS_ERROR(bRetCode && "Get DecayType");

        bRetCode = piTabFile->GetInteger(i, "DecayFrame", 0, &nTmpDecayFrame);
        KGLOG_PROCESS_ERROR(bRetCode && "Get DecayFrame");

        bRetCode = piTabFile->GetInteger(i, "ImmunityFrame", 0, &nTmpImunityFrame);
        KGLOG_PROCESS_ERROR(bRetCode && "Get ImmunityFrame");

        KGLOG_PROCESS_ERROR(dwTmpDecayType >= 0 && dwTmpDecayType < MAX_BUFF_DECAY_TYPE);

        m_BuffDecayTable[dwTmpDecayType].nDecayFrame    = nTmpDecayFrame;
        m_BuffDecayTable[dwTmpDecayType].nImmunityFrame = nTmpImunityFrame;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

struct KEXCLUDE_ID_LEVEL
{
    BOOL    bExclude;
    DWORD   dwID;
    int     nLevel;
};

BOOL KBuffManager::CheckBuffExclude()
{
    BOOL    bResult         = false;
    DWORD   dwLastBuffID    = 0;
    int     nLastLevel      = 0;
    int     nLastAppendType = 0;
    KBUFF_INFO_MAP::iterator    it;
    KBUFF_INFO_MAP::iterator    itEnd;

    itEnd = m_BuffInfoMap.end();
    for (it = m_BuffInfoMap.begin(); it != itEnd; ++it)
    {
        KBuff& rBuffInfo = it->second;

        // m_BuffInfoMap is sorted use ID and Level.
        if (rBuffInfo.m_dwID == dwLastBuffID && rBuffInfo.m_nAppendType != nLastAppendType)
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "[Buff] Same 'ID' but different 'AppendType' : (%u, %d) --> (%u, %d).", 
                dwLastBuffID, nLastLevel, 
                rBuffInfo.m_dwID, rBuffInfo.m_nLevel
            );
            goto Exit0;
        }

        dwLastBuffID    = rBuffInfo.m_dwID;
        nLastLevel      = rBuffInfo.m_nLevel;
        nLastAppendType = rBuffInfo.m_nAppendType;
    }

    bResult = true;
Exit0:
    return bResult;
}

void KBuffManager::CleanUp()
{
    m_BuffRecipeCache.UnInit();
    m_BuffRecipInfoMap.clear();
    m_BuffInfoMap.clear();
}
