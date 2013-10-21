#include "stdafx.h"
#include "KNpcTemplateList.h"
#include "KAttrModifier.h"
#include "KNpc.h"
#include "KSO3World.h"

#define DEFAULT_NPC_TEMPLATE_ID	0
#define NPC_TEMPLATE_FILENAME	"NpcTemplate.tab"


#define NPC_TEMPLATE_ID			    "ID"
#define NPC_NAME				    "Name"
#define NPC_TITLE					"Title"
#define NPC_MAP_NAME				"MapName"
#define NPC_KIND				    "Kind"
#define NPC_FORCE_ID				"ForceID"
#define NPC_SPECIES                 "Species"
#define NPC_LEVEL				    "Level"
#define NPC_ADJUST_LEVEL			"AdjustLevel"
#define NPC_HEIGHT					"Height"
#define NPC_MOVE_MODE				"MoveMode"

#define NPC_TOUCH_RANGE             "TouchRange"
#define NPC_INTENSITY               "Intensity"
#define NPC_ALARM_RANGE             "AlarmRange"
#define NPC_REVIVE_TIME             "ReviveTime"

#define NPC_IS_SELECTABLE			"IsSelectable"
#define NPC_CAN_SEE_LIFEBAR			"CanSeeLifeBar"
#define NPC_CAN_SEE_NAME            "CanSeeName"

#define NPC_MAX_LIFE			    "MaxLife"
#define NPC_LIFE_REPLENISH          "LifeReplenish"
#define NPC_LIFE_REPLENISH_PERCENT  "LifeReplenishPercent"
#define NPC_MAX_MANA			    "MaxMana"
#define NPC_MANA_REPLENISH          "ManaReplenish"
#define NPC_MANA_REPLENISH_PERCENT  "ManaReplenishPercent"

#define NPC_WALK_SPEED              "WalkSpeed"
#define NPC_RUN_SPEED               "RunSpeed"
#define NPC_JUMP_SPEED              "JumpSpeed"

#define NPC_MELEE_WEAPON_DAMAGE_BASE "MeleeWeaponDamageBase"
#define NPC_MELEE_WEAPON_DAMAGE_RAND "MeleeWeaponDamageRand"
#define NPC_RANGE_WEAPON_DAMAGE_BASE "RangeWeaponDamageBase"
#define NPC_RANGE_WEAPON_DAMAGE_RAND "RangeWeaponDamageRand"

#define NPC_HIT_BASE                "HitBase"
#define NPC_PHYSICS_HIT             "PhysicsAttackHit"
#define NPC_SOLAR_MAGIC_HIT         "SolarMagicHit"
#define NPC_NEUTRAL_MAGIC_HIT       "NeutralMagicHit"
#define NPC_LUNAR_MAGIC_HIT         "LunarMagicHit"
#define NPC_POISON_MAGIC_HIT        "PoisonMagicHit"

#define NPC_DODGE			        "Dodge"
#define NPC_PARRY                   "Parry"
#define NPC_PARRY_VALUE             "ParryValue"
#define NPC_SENSE                   "Sense"
#define NPC_STRAIN                  "Strain"
#define NPC_TOUGHNESS               "Toughness"
#define NPC_CRITICALDAMAGEPOWER     "CriticalDamagePower"
#define NPC_DECRITICALDAMAGEPOWER   "DecriticalDamagePower"

#define NPC_PHYSICS_CRITICAL_STRIKE "PhysicsCriticalStrike"
#define NPC_SOLAR_CRITICAL_STRIKE	"SolarCriticalStrike"	
#define NPC_NEUTRAL_CRITICAL_STRIKE	"NeutralCriticalStrike"	
#define NPC_LUNAR_CRITICAL_STRIKE	"LunarCriticalStrike"		
#define NPC_POISON_CRITICAL_STRIKE	"PoisonCriticalStrike"

#define NPC_PHYSICS_SHIELD_BASE     "PhysicsShieldBase"
#define NPC_SOLAR_MAGIC_DEFENCE	    "SolarMagicDefence"	
#define NPC_NEUTRAL_MAGIC_DEFENCE	"NeutralMagicDefence"	
#define NPC_LUNAR_MAGIC_DEFENCE	    "LunarMagicDefence"		
#define NPC_POISON_MAGIC_DEFENCE	"PoisonMagicDefence"

#define NPC_AI_TYPE					"AIType"
#define NPC_AI_PARAM_ID				"AIParamTemplateID"

#define NPC_CORPSE_DOODAD_ID		"CorpseDoodadID"
#define NPC_DROP_CLASS_ID			"DropClassID"

#define NPC_SKILL_ID1				"SkillID1"
#define NPC_SKILL_ID2				"SkillID2"
#define NPC_SKILL_ID3				"SkillID3"
#define NPC_SKILL_ID4				"SkillID4"

#define NPC_SKILL_LEVEL1			"SkillLevel1"
#define NPC_SKILL_LEVEL2			"SkillLevel2"
#define NPC_SKILL_LEVEL3			"SkillLevel3"
#define NPC_SKILL_LEVEL4			"SkillLevel4"

#define NPC_SKILL_INTERVAL1			"SkillInterval1"
#define NPC_SKILL_INTERVAL2			"SkillInterval2"
#define NPC_SKILL_INTERVAL3			"SkillInterval3"
#define NPC_SKILL_INTERVAL4			"SkillInterval4"

#define NPC_SKILL_TYPE1				"SkillType1"
#define NPC_SKILL_TYPE2				"SkillType2"
#define NPC_SKILL_TYPE3				"SkillType3"
#define NPC_SKILL_TYPE4				"SkillType4"

#define NPC_SKILL_RATE1				"SkillRate1"
#define NPC_SKILL_RATE2				"SkillRate2"
#define NPC_SKILL_RATE3				"SkillRate3"
#define NPC_SKILL_RATE4				"SkillRate4"

#define NPC_SKILL_ANI_FRAME1		"SkillAniFrame1"
#define NPC_SKILL_ANI_FRAME2		"SkillAniFrame2"
#define NPC_SKILL_ANI_FRAME3		"SkillAniFrame3"
#define NPC_SKILL_ANI_FRAME4		"SkillAniFrame4"

#define NPC_SKILL_REST_FRAME1		"SkillRestFrame1"
#define NPC_SKILL_REST_FRAME2		"SkillRestFrame2"
#define NPC_SKILL_REST_FRAME3		"SkillRestFrame3"
#define NPC_SKILL_REST_FRAME4		"SkillRestFrame4"

#define NPC_THREAT_TIME				"ThreatTime"
#define NPC_THREAT_PERCENT			"ThreatPercent"
#define NPC_OVER_THREAT_PERCENT		"OverThreatPercent"
#define NPC_PURSUIT_RANGE		    "PursuitRange"

#define NPC_DROP_ID1			    "Drop1"
#define NPC_DROP_ID2			    "Drop2"
#define NPC_DROP_ID3			    "Drop3"
#define NPC_DROP_ID4			    "Drop4"

#define NPC_DROP_COUNT1				"Count1"
#define NPC_DROP_COUNT2				"Count2"
#define NPC_DROP_COUNT3				"Count3"
#define NPC_DROP_COUNT4				"Count4"

#define NPC_EXP                     "NpcExp"
#define NPC_MIN_MONEY				"MoneyMin"
#define NPC_MAX_MONEY				"MoneyMax"
#define NPC_MONEY_DROP_RATE			"MoneyDropRate"

#define NPC_REPUTE_ID1				"ReputeID1"
#define NPC_REPUTE_ID2				"ReputeID2"
#define NPC_REPUTE_ID3				"ReputeID3"
#define NPC_REPUTE_ID4				"ReputeID4"

#define NPC_REPUTE_VALUE1			"ReputeValue1"
#define NPC_REPUTE_VALUE2			"ReputeValue2"
#define NPC_REPUTE_VALUE3			"ReputeValue3"
#define NPC_REPUTE_VALUE4			"ReputeValue4"

#define NPC_REPUTE_LOW_LIMIT1		"ReputeLowLimit1"
#define NPC_REPUTE_LOW_LIMIT2		"ReputeLowLimit2"
#define NPC_REPUTE_LOW_LIMIT3		"ReputeLowLimit3"
#define NPC_REPUTE_LOW_LIMIT4		"ReputeLowLimit4"

#define NPC_REPUTE_HIGH_LIMIT1		"ReputeHighLimit1"
#define NPC_REPUTE_HIGH_LIMIT2		"ReputeHighLimit2"
#define NPC_REPUTE_HIGH_LIMIT3		"ReputeHighLimit3"
#define NPC_REPUTE_HIGH_LIMIT4		"ReputeHighLimit4"

#define NPC_MODEL_FILE			    "ModelFile"

#define NPC_REPRESENT_ID1		    "RepresentID1"
#define NPC_REPRESENT_ID2		    "RepresentID2"
#define NPC_REPRESENT_ID3		    "RepresentID3"
#define NPC_REPRESENT_ID4		    "RepresentID4"
#define NPC_REPRESENT_ID5		    "RepresentID5"
#define NPC_REPRESENT_ID6		    "RepresentID6"
#define NPC_REPRESENT_ID7		    "RepresentID7"
#define NPC_REPRESENT_ID8		    "RepresentID8"
#define NPC_REPRESENT_ID9		    "RepresentID9"
#define NPC_REPRESENT_ID10		    "RepresentID10"

#define NPC_DIALOG_ID				"NpcDialogID"

#define NPC_DIALOG1					"IdleDialog1"
#define NPC_DIALOG_RATE1			"IdleDialogRate1"
#define NPC_DIALOG2					"IdleDialog2"
#define NPC_DIALOG_RATE2			"IdleDialogRate2"
#define NPC_DIALOG3					"IdleDialog3"
#define NPC_DIALOG_RATE3			"IdleDialogRate3"
#define NPC_DIALOG_AFTER_QUEST		"IdleDialogAfterQuest"
#define NPC_DIALOG_QUEST_ID			"IdleDialogQuestID"

#define NPC_HAS_BANK				"HasBank"
#define NPC_HAS_MAILBOX				"HasMailBox"
#define NPC_SHOP_TEMPLATE_ID        "ShopTemplateID"
#define NPC_MASTER_ID               "MasterID"
#define NPC_CRAFT_MASTER_ID			"CraftMasterID"

#define NPC_BANK_TEXT				"BankOptionText"
#define NPC_MAILBOX_TEXT			"MailOptionText"
#define NPC_SHOP_TEXT				"ShopOptionText"
#define NPC_MASTER_TEXT				"SkillMasterOptionText"
#define NPC_CRAFT_MASTER_TEXT		"CraftMasterOptionText"

#define NPC_SHOP_REPUTE_LEVEL           "ShopRequireReputeLevel"
#define NPC_MASTER_REPUTE_LEVEL         "MasterRequireReputeLevel"
#define NPC_CRAFT_MASTER_REPUTE_LEVEL   "CraftMasterRequireReputeLevel"
#define NPC_BANK_REPUTE_LEVEL           "BankRequireReputeLevel"
#define NPC_MAILBOX_REPUTE_LEVEL        "MailBoxRequireReputeLevel"
#define NPC_QUEST_REPUTE_LEVEL          "QuestRequireReputeLevel"

#define NPC_IMMUNITY_MASK           "ImmunityMask"

#define NPC_SCRIPT_NAME				"ScriptName"
#define NPC_DROP_NOT_QUEST_ITEM_FLAG "DropNotQuestItemFlag"
#define NPC_DAILY_QUEST_CYCLE       "DailyQuestCycle"
#define NPC_DAILY_QUEST_OFFSET      "DailyQuestOffset"
#define NPC_IS_RANDOM_DAILY_QUEST   "IsRandomDailyQuest"
#define NPC_PROGRESS_ID             "ProgressID"

#define NPC_HAS_AUCTION                     "HasAuction"
#define NPC_AUCTION_OPTION_TEXT             "AuctionOptionText"
#define NPC_AUCTION_REQUIRE_REPUTE_LEVEL    "AuctionRequireReputeLevel"

#define NPC_CAMP_LOOT_PRESTIGE "CampLootPrestige"
#define NPC_PRESTIGE           "Prestige"
#define NPC_CONTRIBUTE         "Contribution"
#define NPC_ACHIEVEMENT_ID     "AchievementID"

KNpcTemplateList::KNpcTemplateList(void)
{
	m_pNpcTemplateList = NULL;
	m_nNpcTemplateCount = 0;
}

KNpcTemplateList::~KNpcTemplateList(void)
{
	assert(m_pNpcTemplateList == NULL);
	assert(m_nNpcTemplateCount == 0);
}

BOOL KNpcTemplateList::Init(void)
{
    BOOL            bResult                 = false;
	BOOL            bRetCode	            = false;
	ITabFile*       piTabFile               = NULL;
	int             nHeight                 = 0;
    int             nNpcCount               = 0;
	KNpcTemplate    NpcTemplate;
	char            szFilePath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, NPC_TEMPLATE_FILENAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	memset(&m_DefaultNpcTemplate, 0, sizeof(m_DefaultNpcTemplate));
	
	bRetCode = LoadNpcTemplate(2, piTabFile, m_DefaultNpcTemplate);
	KGLOG_PROCESS_ERROR(bRetCode);

	KG_PROCESS_SUCCESS(nHeight == 2);
    
    nNpcCount = nHeight - 2;
	m_pNpcTemplateList = new KNpcTemplate[nNpcCount];
	KGLOG_PROCESS_ERROR(m_pNpcTemplateList);

	for (int i = 0; i < nNpcCount; i++)
	{
        memset(&NpcTemplate, 0, sizeof(NpcTemplate));

        bRetCode = LoadNpcTemplate(i + 3, piTabFile, NpcTemplate);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (NpcTemplate.dwTemplateID == 0)
		{
			KGLogPrintf(
                KGLOG_ERR, "[KNpcTemplateList]Line:%d ID:(%d) in %s was repeated!",
				i + 3, NpcTemplate.dwTemplateID, NPC_TEMPLATE_FILENAME
            );
			goto Exit0;
		}

        m_pNpcTemplateList[i] = NpcTemplate;
		m_mapNpcTemplateList[NpcTemplate.dwTemplateID] = m_pNpcTemplateList + i;
	}

	m_nNpcTemplateCount = nNpcCount;

Exit1:
	bResult = true;
Exit0:
    if (!bResult)
    {
        SAFE_DELETE_ARRAY(m_pNpcTemplateList);
    }
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KNpcTemplateList::UnInit(void)
{
	KG_DELETE_ARRAY(m_pNpcTemplateList);
	m_nNpcTemplateCount = 0;
	m_mapNpcTemplateList.clear();

	return true;
}

BOOL KNpcTemplateList::LoadFromTemplate(DWORD dwNpcTemplateID, KNpc* pNpc)
{
	BOOL			    bRetCode		= false;
	KNpcTemplate*	    pNpcTemplate	= NULL;

    assert(pNpc);
    assert(pNpc->m_pTemplate == NULL);

	pNpcTemplate = GetTemplate(dwNpcTemplateID);
	if (pNpcTemplate == NULL)
	{
        KGLogPrintf(KGLOG_ERR, "Invalid NPC template ID: %u!\n", dwNpcTemplateID);

		pNpcTemplate = GetTemplate(0);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}

	pNpc->m_dwTemplateID			= dwNpcTemplateID;
	pNpc->m_dwForceID				= pNpcTemplate->dwForceID;
	pNpc->m_eKind                   = (CHARACTER_KIND)pNpcTemplate->nKind;
    pNpc->m_eSpecies                = (KNPC_SPECIES)pNpcTemplate->nSpecies;
	pNpc->m_nLevel                  = pNpcTemplate->nLevel;
    pNpc->m_nTouchRange             = pNpcTemplate->nTouchRange;
    pNpc->m_nIntensity              = pNpcTemplate->nIntensity;
    pNpc->m_nReviveTime             = pNpcTemplate->nReviveTime;
	pNpc->m_nHeight					= pNpcTemplate->nHeight;
    pNpc->m_eMoveMode               = pNpcTemplate->eMoveMode;
    pNpc->m_AIData.nAIType          = pNpcTemplate->nAIType;

    pNpc->CallAttributeFunction(atMaxLifeBase, true, pNpcTemplate->nMaxLife, 0);
    pNpc->CallAttributeFunction(atLifeReplenish, true, pNpcTemplate->nLifeReplenish, 0);
    pNpc->CallAttributeFunction(atLifeReplenishPercent, true, pNpcTemplate->nLifeReplenishPercent, 0);

    pNpc->CallAttributeFunction(atMaxManaBase, true, pNpcTemplate->nMaxMana, 0);
    pNpc->CallAttributeFunction(atManaReplenish, true, pNpcTemplate->nManaReplenish, 0);
    pNpc->CallAttributeFunction(atManaReplenishPercent, true, pNpcTemplate->nManaReplenishPercent, 0);

    pNpc->CallAttributeFunction(atRunSpeedBase, true, pNpcTemplate->nRunSpeed, 0);
    pNpc->CallAttributeFunction(atJumpSpeedBase, true, pNpcTemplate->nJumpSpeed, 0);

    pNpc->m_nMeleeWeaponDamageBase = pNpcTemplate->nMeleeWeaponDamageBase;
    pNpc->m_nMeleeWeaponDamageRand = pNpcTemplate->nMeleeWeaponDamageRand;
    pNpc->m_nRangeWeaponDamageBase = pNpcTemplate->nRangeWeaponDamageBase;
    pNpc->m_nRangeWeaponDamageRand = pNpcTemplate->nRangeWeaponDamageRand;

    pNpc->CallAttributeFunction(atPhysicsHitBaseRate, true, pNpcTemplate->nHitBase, 0);
    pNpc->CallAttributeFunction(atSolarHitBaseRate, true, pNpcTemplate->nHitBase, 0);
    pNpc->CallAttributeFunction(atNeutralHitBaseRate, true, pNpcTemplate->nHitBase, 0);
    pNpc->CallAttributeFunction(atLunarHitBaseRate, true, pNpcTemplate->nHitBase, 0);
    pNpc->CallAttributeFunction(atPoisonHitBaseRate, true, pNpcTemplate->nHitBase, 0);

    pNpc->CallAttributeFunction(atDodge, true, pNpcTemplate->nDodge, 0);
    pNpc->CallAttributeFunction(atParryBase, true, pNpcTemplate->nParry, 0);
    pNpc->CallAttributeFunction(atParryValueBase, true, pNpcTemplate->nParryValue, 0);
    pNpc->CallAttributeFunction(atSense, true, pNpcTemplate->nSense, 0);

    pNpc->CallAttributeFunction(atPhysicsCriticalStrike, true, pNpcTemplate->nPhysicsCriticalStrike, 0);
    pNpc->CallAttributeFunction(atSolarCriticalStrike, true, pNpcTemplate->nSolarCriticalStrike, 0);
    pNpc->CallAttributeFunction(atNeutralCriticalStrike, true, pNpcTemplate->nNeutralCriticalStrike, 0);
    pNpc->CallAttributeFunction(atLunarCriticalStrike, true, pNpcTemplate->nLunarCriticalStrike, 0);
    pNpc->CallAttributeFunction(atPoisonCriticalStrike, true, pNpcTemplate->nPoisonCriticalStrike, 0);

    pNpc->CallAttributeFunction(atPhysicsShieldBase, true, pNpcTemplate->nPhysicsShieldBase, 0);
    pNpc->CallAttributeFunction(atSolarMagicShieldBase, true, pNpcTemplate->nSolarMagicDefence, 0);
    pNpc->CallAttributeFunction(atNeutralMagicShieldBase, true, pNpcTemplate->nNeutralMagicDefence, 0);
    pNpc->CallAttributeFunction(atLunarMagicShieldBase, true, pNpcTemplate->nLunarMagicDefence, 0);
    pNpc->CallAttributeFunction(atPoisonMagicShieldBase, true, pNpcTemplate->nPoisonMagicDefence, 0);

    // 读取 Boss属性 免疫掩码
    assert(sftTotal <= (sizeof(DWORD) * CHAR_BIT + 1));
    for (int nFuncType = sftInvalid + 1; nFuncType < sftTotal; nFuncType++)
    {
        if (pNpcTemplate->dwImmunityMask & ((DWORD)0x1 << (nFuncType - 1)))
        {
            pNpc->m_ImmunityFlag.set(nFuncType - 1);
        }
        else
        {
            pNpc->m_ImmunityFlag.reset(nFuncType - 1);
        }
    }

	strncpy(pNpc->m_szName, pNpcTemplate->szName, _NAME_LEN);
	pNpc->m_szName[_NAME_LEN - 1] = 0;

	strncpy(pNpc->m_szTitle, pNpcTemplate->szTitle, TITLE_LEN);
	pNpc->m_szTitle[TITLE_LEN - 1] = 0;

	pNpc->m_nCurrentLife = pNpc->m_nMaxLife;
	pNpc->m_nCurrentMana = pNpc->m_nMaxMana;

	pNpc->m_pTemplate = pNpcTemplate;

#if defined(_SERVER)
	pNpc->m_nThreatTime	= pNpcTemplate->nThreatTime;		

	pNpc->m_bBank           = pNpcTemplate->bHasBank;
    pNpc->m_bMailbox        = pNpcTemplate->bHasMailBox;
    pNpc->m_dwSkillMasterID = pNpcTemplate->dwMasterID;
    pNpc->m_dwCraftMasterID = pNpcTemplate->dwCraftMasterID;
    pNpc->m_bAuction        = pNpcTemplate->bHasAuction;

    if (pNpcTemplate->nShopTemplateID > 0)
    {
        g_pSO3World->m_ShopCenter.BindNpcShop(pNpc, pNpcTemplate->nShopTemplateID);
    }
	
	if (pNpcTemplate->szScriptName[0] != '\0')
	{
		pNpc->m_dwScriptID = g_FileNameHash(pNpcTemplate->szScriptName);
	}

	pNpc->m_dwModelID  = pNpcTemplate->dwRepresentID[0]; //默认读第一个表现ID
#endif

	return true;

Exit0:
	KGLogPrintf(KGLOG_ERR, "KNpcTemplateList::LoadFromTemplate(%d)\n", dwNpcTemplateID);
	return false;
}

int KNpcTemplateList::GetNpcTemplateCount()
{
	return m_nNpcTemplateCount;
}

KNpcTemplate* KNpcTemplateList::GetTemplateByIndex(int nIndex)
{
	KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_nNpcTemplateCount);
	KGLOG_PROCESS_ERROR(m_pNpcTemplateList);

	return &(m_pNpcTemplateList[nIndex]);
Exit0:
	return NULL;
}

KNpcTemplate* KNpcTemplateList::GetTemplate(DWORD dwNpcTemplateID)
{
	KNpcTemplate* pResult = NULL;
	KNPC_TEMPLATE_LIST::iterator iter;

	if (dwNpcTemplateID == 0)
	{
		pResult = &m_DefaultNpcTemplate;
		goto Exit0;
	}

	iter = m_mapNpcTemplateList.find(dwNpcTemplateID);
	if (iter != m_mapNpcTemplateList.end())
	{
		pResult = iter->second;
	}
Exit0:
	return pResult;
}

BOOL KNpcTemplateList::LoadNpcTemplate(int nIndex, ITabFile* piTabFile, KNpcTemplate& fNpcTemplate)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(piTabFile);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_TEMPLATE_ID,
		m_DefaultNpcTemplate.dwTemplateID, (int*)&(fNpcTemplate.dwTemplateID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_NAME, 
		m_DefaultNpcTemplate.szName, fNpcTemplate.szName, _NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_TITLE,
		m_DefaultNpcTemplate.szTitle, fNpcTemplate.szTitle, TITLE_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_MAP_NAME,
		m_DefaultNpcTemplate.szMapName, fNpcTemplate.szMapName, _NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_KIND, 
		m_DefaultNpcTemplate.nKind, (int*)&(fNpcTemplate.nKind)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_FORCE_ID, 
		m_DefaultNpcTemplate.dwForceID, (int*)&(fNpcTemplate.dwForceID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SPECIES, 
        m_DefaultNpcTemplate.nSpecies, &fNpcTemplate.nSpecies
    );
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_LEVEL, 
		m_DefaultNpcTemplate.nLevel, (int*)&(fNpcTemplate.nLevel)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    if (fNpcTemplate.nLevel <= 0)
    {
        KGLogPrintf(KGLOG_ERR, "Npc (%lu) level error!", fNpcTemplate.dwTemplateID);
        goto Exit0;
    }

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_ADJUST_LEVEL,
		m_DefaultNpcTemplate.nAdjustLevel, (int*)&(fNpcTemplate.nAdjustLevel)
    );	
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_HEIGHT,
		m_DefaultNpcTemplate.nHeight, (int*)&(fNpcTemplate.nHeight)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MOVE_MODE,
		m_DefaultNpcTemplate.eMoveMode, (int*)&(fNpcTemplate.eMoveMode)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_TOUCH_RANGE, 
		m_DefaultNpcTemplate.nTouchRange, (int*)&(fNpcTemplate.nTouchRange)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(fNpcTemplate.nTouchRange >= 0);

    //强度
	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_INTENSITY, 
		m_DefaultNpcTemplate.nIntensity, (int*)&(fNpcTemplate.nIntensity)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REVIVE_TIME, 
		m_DefaultNpcTemplate.nReviveTime, (int*)&(fNpcTemplate.nReviveTime)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_IS_SELECTABLE,
		m_DefaultNpcTemplate.bSelectable, (int*)&(fNpcTemplate.bSelectable)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CAN_SEE_LIFEBAR,
		m_DefaultNpcTemplate.bCanSeeLifeBar, (int*)&(fNpcTemplate.bCanSeeLifeBar)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CAN_SEE_NAME,
		m_DefaultNpcTemplate.bCanSeeName, (int*)&(fNpcTemplate.bCanSeeName)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MAX_LIFE, 
		m_DefaultNpcTemplate.nMaxLife, (int*)&(fNpcTemplate.nMaxLife)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    //生命回复点数
	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_LIFE_REPLENISH, 
		m_DefaultNpcTemplate.nLifeReplenish, (int*)&(fNpcTemplate.nLifeReplenish)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    //生命回复百分比
	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_LIFE_REPLENISH_PERCENT, 
		m_DefaultNpcTemplate.nLifeReplenishPercent, (int*)&(fNpcTemplate.nLifeReplenishPercent)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MAX_MANA, 
		m_DefaultNpcTemplate.nMaxMana, (int*)&(fNpcTemplate.nMaxMana)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    //内力回复点数
	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MANA_REPLENISH, 
		m_DefaultNpcTemplate.nManaReplenish, (int*)&(fNpcTemplate.nManaReplenish)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    //内力回复百分比
	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MANA_REPLENISH_PERCENT, 
		m_DefaultNpcTemplate.nManaReplenishPercent, (int*)&(fNpcTemplate.nManaReplenishPercent)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_WALK_SPEED, 
        m_DefaultNpcTemplate.nWalkSpeed, (int*)&(fNpcTemplate.nWalkSpeed)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_RUN_SPEED, 
		m_DefaultNpcTemplate.nRunSpeed, (int*)&(fNpcTemplate.nRunSpeed)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_JUMP_SPEED, 
		m_DefaultNpcTemplate.nJumpSpeed, (int*)&(fNpcTemplate.nJumpSpeed)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MELEE_WEAPON_DAMAGE_BASE,
        m_DefaultNpcTemplate.nMeleeWeaponDamageBase, (int*)&(fNpcTemplate.nMeleeWeaponDamageBase)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MELEE_WEAPON_DAMAGE_RAND,
        m_DefaultNpcTemplate.nMeleeWeaponDamageRand, (int*)&(fNpcTemplate.nMeleeWeaponDamageRand)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_RANGE_WEAPON_DAMAGE_BASE,
        m_DefaultNpcTemplate.nRangeWeaponDamageBase, (int*)&(fNpcTemplate.nRangeWeaponDamageBase)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_RANGE_WEAPON_DAMAGE_RAND,
        m_DefaultNpcTemplate.nRangeWeaponDamageRand, (int*)&(fNpcTemplate.nRangeWeaponDamageRand)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_HIT_BASE, 
		m_DefaultNpcTemplate.nHitBase, (int*)&(fNpcTemplate.nHitBase)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DODGE, 
		m_DefaultNpcTemplate.nDodge, (int*)&(fNpcTemplate.nDodge)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PARRY, 
		m_DefaultNpcTemplate.nParry, (int*)&(fNpcTemplate.nParry)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PARRY_VALUE, 
		m_DefaultNpcTemplate.nParryValue, (int*)&(fNpcTemplate.nParryValue)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SENSE, 
		m_DefaultNpcTemplate.nSense, (int*)&(fNpcTemplate.nSense)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PHYSICS_CRITICAL_STRIKE, 
		m_DefaultNpcTemplate.nPhysicsCriticalStrike, (int*)&(fNpcTemplate.nPhysicsCriticalStrike)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SOLAR_CRITICAL_STRIKE, 
        m_DefaultNpcTemplate.nSolarCriticalStrike, (int*)&(fNpcTemplate.nSolarCriticalStrike)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_NEUTRAL_CRITICAL_STRIKE, 
        m_DefaultNpcTemplate.nNeutralCriticalStrike, (int*)&(fNpcTemplate.nNeutralCriticalStrike)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_LUNAR_CRITICAL_STRIKE, 
        m_DefaultNpcTemplate.nLunarCriticalStrike, (int*)&(fNpcTemplate.nLunarCriticalStrike)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_POISON_CRITICAL_STRIKE, 
        m_DefaultNpcTemplate.nPoisonCriticalStrike, (int*)&(fNpcTemplate.nPoisonCriticalStrike)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PHYSICS_SHIELD_BASE, 
		m_DefaultNpcTemplate.nPhysicsShieldBase, (int*)&(fNpcTemplate.nPhysicsShieldBase)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SOLAR_MAGIC_DEFENCE, 
		m_DefaultNpcTemplate.nSolarMagicDefence, (int*)&(fNpcTemplate.nSolarMagicDefence)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_NEUTRAL_MAGIC_DEFENCE, 
		m_DefaultNpcTemplate.nNeutralMagicDefence, (int*)&(fNpcTemplate.nNeutralMagicDefence)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_LUNAR_MAGIC_DEFENCE, 
		m_DefaultNpcTemplate.nLunarMagicDefence, (int*)&(fNpcTemplate.nLunarMagicDefence)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_POISON_MAGIC_DEFENCE, 
		m_DefaultNpcTemplate.nPoisonMagicDefence, (int*)&(fNpcTemplate.nPoisonMagicDefence)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_AI_TYPE, 
		m_DefaultNpcTemplate.nAIType, (int*)&(fNpcTemplate.nAIType)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_AI_PARAM_ID,
		m_DefaultNpcTemplate.nAIParamTemplateID, (int*)&(fNpcTemplate.nAIParamTemplateID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CORPSE_DOODAD_ID, 
		m_DefaultNpcTemplate.nCorpseDoodadID, (int*)&(fNpcTemplate.nCorpseDoodadID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DROP_CLASS_ID,
		m_DefaultNpcTemplate.nDropClassID, &fNpcTemplate.nDropClassID
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_IMMUNITY_MASK,
        m_DefaultNpcTemplate.dwImmunityMask, (int*)&fNpcTemplate.dwImmunityMask
    );
    KGLOG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
	bRetCode = piTabFile->GetString(
        nIndex, NPC_DROP_ID1, 
		m_DefaultNpcTemplate.szDropName[0], fNpcTemplate.szDropName[0], MAX_PATH
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_DROP_ID2, 
		m_DefaultNpcTemplate.szDropName[1], fNpcTemplate.szDropName[1], MAX_PATH
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_DROP_ID3, 
		m_DefaultNpcTemplate.szDropName[2], fNpcTemplate.szDropName[2], MAX_PATH
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_DROP_ID4, 
		m_DefaultNpcTemplate.szDropName[3], fNpcTemplate.szDropName[3], MAX_PATH
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DROP_COUNT1,
		m_DefaultNpcTemplate.nDropCount[0], &fNpcTemplate.nDropCount[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DROP_COUNT2,
		m_DefaultNpcTemplate.nDropCount[1], &fNpcTemplate.nDropCount[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DROP_COUNT3,
		m_DefaultNpcTemplate.nDropCount[2], &fNpcTemplate.nDropCount[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DROP_COUNT4,
		m_DefaultNpcTemplate.nDropCount[3], &fNpcTemplate.nDropCount[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ID1,
		m_DefaultNpcTemplate.dwSkillIDList[0], (int*)&fNpcTemplate.dwSkillIDList[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ID2,
		m_DefaultNpcTemplate.dwSkillIDList[1], (int*)&fNpcTemplate.dwSkillIDList[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ID3,
		m_DefaultNpcTemplate.dwSkillIDList[2], (int*)&fNpcTemplate.dwSkillIDList[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ID4,
		m_DefaultNpcTemplate.dwSkillIDList[3], (int*)&fNpcTemplate.dwSkillIDList[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_LEVEL1,
		m_DefaultNpcTemplate.dwSkillLevelList[0], (int*)&fNpcTemplate.dwSkillLevelList[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_LEVEL2,
		m_DefaultNpcTemplate.dwSkillLevelList[1], (int*)&fNpcTemplate.dwSkillLevelList[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_LEVEL3,
		m_DefaultNpcTemplate.dwSkillLevelList[2], (int*)&fNpcTemplate.dwSkillLevelList[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_LEVEL4,
		m_DefaultNpcTemplate.dwSkillLevelList[3], (int*)&fNpcTemplate.dwSkillLevelList[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_INTERVAL1,
		m_DefaultNpcTemplate.nSkillCastInterval[0], (int*)&fNpcTemplate.nSkillCastInterval[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_INTERVAL2,
		m_DefaultNpcTemplate.nSkillCastInterval[1], (int*)&fNpcTemplate.nSkillCastInterval[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_INTERVAL3,
		m_DefaultNpcTemplate.nSkillCastInterval[2], (int*)&fNpcTemplate.nSkillCastInterval[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_INTERVAL4,
		m_DefaultNpcTemplate.nSkillCastInterval[3], (int*)&fNpcTemplate.nSkillCastInterval[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_TYPE1,
		m_DefaultNpcTemplate.nAISkillType[0], &fNpcTemplate.nAISkillType[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_TYPE2,
		m_DefaultNpcTemplate.nAISkillType[1], &fNpcTemplate.nAISkillType[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_TYPE3,
		m_DefaultNpcTemplate.nAISkillType[2], &fNpcTemplate.nAISkillType[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_TYPE4,
		m_DefaultNpcTemplate.nAISkillType[3], &fNpcTemplate.nAISkillType[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_RATE1,
		m_DefaultNpcTemplate.nSkillRate[0], & fNpcTemplate.nSkillRate[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_RATE2,
		m_DefaultNpcTemplate.nSkillRate[1], & fNpcTemplate.nSkillRate[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_RATE3,
		m_DefaultNpcTemplate.nSkillRate[2], & fNpcTemplate.nSkillRate[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_RATE4,
		m_DefaultNpcTemplate.nSkillRate[3], & fNpcTemplate.nSkillRate[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ANI_FRAME1,
        m_DefaultNpcTemplate.nSkillAniFrame[0], & fNpcTemplate.nSkillAniFrame[0]
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ANI_FRAME2,
        m_DefaultNpcTemplate.nSkillAniFrame[1], & fNpcTemplate.nSkillAniFrame[1]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ANI_FRAME3,
        m_DefaultNpcTemplate.nSkillAniFrame[2], & fNpcTemplate.nSkillAniFrame[2]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_ANI_FRAME4,
        m_DefaultNpcTemplate.nSkillAniFrame[3], & fNpcTemplate.nSkillAniFrame[3]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_REST_FRAME1,
        m_DefaultNpcTemplate.nSkillRestFrame[0], & fNpcTemplate.nSkillRestFrame[0]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_REST_FRAME2,
        m_DefaultNpcTemplate.nSkillRestFrame[1], & fNpcTemplate.nSkillRestFrame[1]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_REST_FRAME3,
        m_DefaultNpcTemplate.nSkillRestFrame[2], & fNpcTemplate.nSkillRestFrame[2]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SKILL_REST_FRAME4,
        m_DefaultNpcTemplate.nSkillRestFrame[3], & fNpcTemplate.nSkillRestFrame[3]
    );
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_THREAT_TIME,
		m_DefaultNpcTemplate.nThreatTime, (int*)&fNpcTemplate.nThreatTime
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_OVER_THREAT_PERCENT,
		m_DefaultNpcTemplate.nOverThreatPercent, (int*)&fNpcTemplate.nOverThreatPercent
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PURSUIT_RANGE,
        m_DefaultNpcTemplate.nPursuitRange, (int*)&fNpcTemplate.nPursuitRange
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_EXP,
        m_DefaultNpcTemplate.nExp, (int*)&fNpcTemplate.nExp
    );
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MIN_MONEY,
		m_DefaultNpcTemplate.nMinMoney, (int*)&fNpcTemplate.nMinMoney
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MAX_MONEY,
		m_DefaultNpcTemplate.nMaxMoney, (int*)&fNpcTemplate.nMaxMoney
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MONEY_DROP_RATE,
		m_DefaultNpcTemplate.nMoneyRate, (int*)&fNpcTemplate.nMoneyRate
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_ID1,
		m_DefaultNpcTemplate.dwReputeID[0], (int*)&fNpcTemplate.dwReputeID[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_VALUE1,
		m_DefaultNpcTemplate.nReputeValue[0], (int*)&fNpcTemplate.nReputeValue[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_LOW_LIMIT1,
		m_DefaultNpcTemplate.nReputeLowLimit[0], (int*)&fNpcTemplate.nReputeLowLimit[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_HIGH_LIMIT1,
		m_DefaultNpcTemplate.nReputeHighLimit[0], (int*)&fNpcTemplate.nReputeHighLimit[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_ID2,
		m_DefaultNpcTemplate.dwReputeID[1], (int*)&fNpcTemplate.dwReputeID[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_VALUE2,
		m_DefaultNpcTemplate.nReputeValue[1], (int*)&fNpcTemplate.nReputeValue[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_LOW_LIMIT2,
		m_DefaultNpcTemplate.nReputeLowLimit[1], (int*)&fNpcTemplate.nReputeLowLimit[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_HIGH_LIMIT2,
		m_DefaultNpcTemplate.nReputeHighLimit[1], (int*)&fNpcTemplate.nReputeHighLimit[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_ID3,
		m_DefaultNpcTemplate.dwReputeID[2], (int*)&fNpcTemplate.dwReputeID[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_VALUE3,
		m_DefaultNpcTemplate.nReputeValue[2], (int*)&fNpcTemplate.nReputeValue[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_LOW_LIMIT3,
		m_DefaultNpcTemplate.nReputeLowLimit[2], (int*)&fNpcTemplate.nReputeLowLimit[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_HIGH_LIMIT3,
		m_DefaultNpcTemplate.nReputeHighLimit[2], (int*)&fNpcTemplate.nReputeHighLimit[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_ID4,
		m_DefaultNpcTemplate.dwReputeID[3], (int*)&fNpcTemplate.dwReputeID[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_VALUE4,
		m_DefaultNpcTemplate.nReputeValue[3], (int*)&fNpcTemplate.nReputeValue[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_LOW_LIMIT4,
		m_DefaultNpcTemplate.nReputeLowLimit[3], (int*)&fNpcTemplate.nReputeLowLimit[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPUTE_HIGH_LIMIT4,
		m_DefaultNpcTemplate.nReputeHighLimit[3], (int*)&fNpcTemplate.nReputeHighLimit[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

#endif	//_SERVER 

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID1, 
		m_DefaultNpcTemplate.dwRepresentID[0], (int*)&fNpcTemplate.dwRepresentID[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID2, 
		m_DefaultNpcTemplate.dwRepresentID[1], (int*)&fNpcTemplate.dwRepresentID[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID3, 
		m_DefaultNpcTemplate.dwRepresentID[2], (int*)&fNpcTemplate.dwRepresentID[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID4, 
		m_DefaultNpcTemplate.dwRepresentID[3], (int*)&fNpcTemplate.dwRepresentID[3]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID5, 
		m_DefaultNpcTemplate.dwRepresentID[4], (int*)&fNpcTemplate.dwRepresentID[4]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID6, 
		m_DefaultNpcTemplate.dwRepresentID[5], (int*)&fNpcTemplate.dwRepresentID[5]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID7, 
		m_DefaultNpcTemplate.dwRepresentID[6], (int*)&fNpcTemplate.dwRepresentID[6]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID8, 
		m_DefaultNpcTemplate.dwRepresentID[7], (int*)&fNpcTemplate.dwRepresentID[7]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID9, 
		m_DefaultNpcTemplate.dwRepresentID[8], (int*)&fNpcTemplate.dwRepresentID[8]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_REPRESENT_ID10, 
		m_DefaultNpcTemplate.dwRepresentID[9], (int*)&fNpcTemplate.dwRepresentID[9]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DROP_NOT_QUEST_ITEM_FLAG,
		m_DefaultNpcTemplate.bDropNotQuestItemFlag, (int*)&fNpcTemplate.bDropNotQuestItemFlag
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_DIALOG1,
		m_DefaultNpcTemplate.szDialog[0], fNpcTemplate.szDialog[0], MAX_NPC_DIALOG_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DIALOG_RATE1,
		m_DefaultNpcTemplate.nDialogRate[0], (int*)&fNpcTemplate.nDialogRate[0]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_DIALOG2,
		m_DefaultNpcTemplate.szDialog[1], fNpcTemplate.szDialog[1], MAX_NPC_DIALOG_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DIALOG_RATE2,
		m_DefaultNpcTemplate.nDialogRate[1], (int*)&fNpcTemplate.nDialogRate[1]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_DIALOG3,
		m_DefaultNpcTemplate.szDialog[2], fNpcTemplate.szDialog[2], MAX_NPC_DIALOG_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DIALOG_RATE3,
		m_DefaultNpcTemplate.nDialogRate[2], (int*)&fNpcTemplate.nDialogRate[2]
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	fNpcTemplate.nTotalRate = fNpcTemplate.nDialogRate[0] + fNpcTemplate.nDialogRate[1] + fNpcTemplate.nDialogRate[2];
	KGLOG_PROCESS_ERROR(fNpcTemplate.nTotalRate >= 0);
	
	bRetCode = piTabFile->GetString(
        nIndex, NPC_DIALOG_AFTER_QUEST,
		m_DefaultNpcTemplate.szDialogAfterQuest, fNpcTemplate.szDialogAfterQuest, MAX_NPC_DIALOG_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DIALOG_QUEST_ID,
		m_DefaultNpcTemplate.dwDialogQuestID, (int*)&fNpcTemplate.dwDialogQuestID
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_HAS_BANK,
		m_DefaultNpcTemplate.bHasBank, (int*)&fNpcTemplate.bHasBank
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_HAS_MAILBOX,
		m_DefaultNpcTemplate.bHasMailBox, (int*)&fNpcTemplate.bHasMailBox
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SHOP_TEMPLATE_ID, 
        m_DefaultNpcTemplate.nShopTemplateID, (int*)&fNpcTemplate.nShopTemplateID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MASTER_ID, 
        m_DefaultNpcTemplate.dwMasterID, (int*)&fNpcTemplate.dwMasterID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CRAFT_MASTER_ID,
		m_DefaultNpcTemplate.dwCraftMasterID, (int*)&(fNpcTemplate.dwCraftMasterID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_BANK_TEXT,
		m_DefaultNpcTemplate.szBankOptionText, fNpcTemplate.szBankOptionText, MAX_OPTION_TEXT_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_MAILBOX_TEXT,
		m_DefaultNpcTemplate.szMailBoxOptionText, fNpcTemplate.szMailBoxOptionText, MAX_OPTION_TEXT_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_SHOP_TEXT,
		m_DefaultNpcTemplate.szShopOptionText, fNpcTemplate.szShopOptionText, MAX_OPTION_TEXT_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bRetCode = piTabFile->GetString(
        nIndex, NPC_MASTER_TEXT,
		m_DefaultNpcTemplate.szMasterOptionText, fNpcTemplate.szMasterOptionText, MAX_OPTION_TEXT_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, NPC_CRAFT_MASTER_TEXT,
		m_DefaultNpcTemplate.szCraftMasterOptionText, fNpcTemplate.szCraftMasterOptionText, MAX_OPTION_TEXT_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_SHOP_REPUTE_LEVEL,
        m_DefaultNpcTemplate.nShopRequireReputeLevel, &fNpcTemplate.nShopRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MASTER_REPUTE_LEVEL,
        m_DefaultNpcTemplate.nMasterRequireReputeLevel, &fNpcTemplate.nMasterRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CRAFT_MASTER_REPUTE_LEVEL,
        m_DefaultNpcTemplate.nCraftMasterRequireReputeLevel, &fNpcTemplate.nCraftMasterRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_BANK_REPUTE_LEVEL,
        m_DefaultNpcTemplate.nBankRequireReputeLevel, &fNpcTemplate.nBankRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_MAILBOX_REPUTE_LEVEL,
        m_DefaultNpcTemplate.nMailBoxRequireReputeLevel, &fNpcTemplate.nMailBoxRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_QUEST_REPUTE_LEVEL,
        m_DefaultNpcTemplate.nQuestRequireReputeLevel, &fNpcTemplate.nQuestRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

#endif

	bRetCode = piTabFile->GetString(
        nIndex, NPC_SCRIPT_NAME, 
		m_DefaultNpcTemplate.szScriptName, fNpcTemplate.szScriptName, MAX_SCIRPT_NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DIALOG_ID, 
		m_DefaultNpcTemplate.nNpcDialogID, (int*)&fNpcTemplate.nNpcDialogID
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DAILY_QUEST_CYCLE, 
		m_DefaultNpcTemplate.nDailyQuestCycle, (int*)&fNpcTemplate.nDailyQuestCycle
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    fNpcTemplate.nDailyQuestCycle *= 60; // 转成秒

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_DAILY_QUEST_OFFSET, 
		m_DefaultNpcTemplate.nDailyQuestOffset, (int*)&fNpcTemplate.nDailyQuestOffset
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    fNpcTemplate.nDailyQuestOffset *= 60; // 转成秒

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_IS_RANDOM_DAILY_QUEST, 
		m_DefaultNpcTemplate.bIsRandomDailyQuest, (int*)&fNpcTemplate.bIsRandomDailyQuest
    );
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PROGRESS_ID, m_DefaultNpcTemplate.nProgressID, (int*)&fNpcTemplate.nProgressID
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_HAS_AUCTION, m_DefaultNpcTemplate.bHasAuction, (int*)&fNpcTemplate.bHasAuction
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetString(
        nIndex, NPC_AUCTION_OPTION_TEXT, m_DefaultNpcTemplate.szAuctionOptionText,
        fNpcTemplate.szAuctionOptionText, MAX_OPTION_TEXT_LEN
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_AUCTION_REQUIRE_REPUTE_LEVEL, m_DefaultNpcTemplate.nAuctionRequireReputeLevel,
        &fNpcTemplate.nAuctionRequireReputeLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CAMP_LOOT_PRESTIGE, m_DefaultNpcTemplate.nCampLootPrestige, &fNpcTemplate.nCampLootPrestige
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_PRESTIGE, m_DefaultNpcTemplate.nPrestige, &fNpcTemplate.nPrestige
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_CONTRIBUTE, m_DefaultNpcTemplate.nContribution, &fNpcTemplate.nContribution
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, NPC_ACHIEVEMENT_ID, m_DefaultNpcTemplate.nAchievementID, &fNpcTemplate.nAchievementID
    );
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bResult = true;
Exit0:
	return bResult;
}
