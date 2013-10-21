#include "stdafx.h"
#include "KGWConstList.h"

#define INI_FILE_NAME		"GameWorldConstList.ini"

#define SECTION_AI	        "AI"
#define SECTION_PK	        "PK"
#define SECTION_MAIL        "MAIL"
#define SECTION_ITEM        "ITEM"
#define SECTION_FELLOWSHIP  "FELLOWSHIP"
#define SECTION_ACTION      "ACTION"
#define SECTION_PING        "PING"
#define SECTION_MISC        "MISC"
#define SECTION_CAMP        "CAMP"
#define SECTION_TALK        "TALK"

BOOL KGWConstList::Init()
{
	BOOL        bResult     = false;
	int         nRetCode    = false;
	IIniFile*   piIniFile   = NULL;
	char        szFilePath[MAX_PATH];

	nRetCode = snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, INI_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szFilePath));

	piIniFile = g_OpenIniFile(szFilePath);
	KGLOG_PROCESS_ERROR(piIniFile);

	nRetCode = LoadData(piIniFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piIniFile);
	return bResult;
}

void KGWConstList::UnInit()
{
}

BOOL KGWConstList::LoadData(IIniFile* piIniFile)
{
	BOOL bResult    = false;
	int  nRetCode   = 0;

	assert(piIniFile);

	nRetCode = piIniFile->GetInteger(SECTION_AI, "MaxThreatCount", 0, &nMaxThreatCount);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_AI, "NpcSkillCommonCD", 0, &nNpcSkillCommonCD);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_AI, "NpcCommonShortCD", 0, &nNpcCommonShortCD);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_AI, "AISearchByLifeConversionLevel", 0, &nAISearchByLifeConversionLevel);
	KGLOG_PROCESS_ERROR(nRetCode);

	//////////////////////////////////////////////////////////////////////////

	nRetCode = piIniFile->GetInteger(SECTION_PK, "PKFlagDoodadTemplateID", 0, &nPKFlagDoodadTemplateID);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "SlayCDIndex", 0, &nSlayCDIndex);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "WaitSlayTime", 0, &nWaitSlayTime);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "CloseSlayTime", 0, &nCloseSlayTime);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "MinDuelLevel", 0, &nMinDuelLevel);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "MinRevengeLevel", 0, &nMinRevengeLevel);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "MinSlayLevel", 0, &nMinSlayLevel);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_PK, "MinBeSlayedLevel", 0, &nMinBeSlayedLevel);
	KGLOG_PROCESS_ERROR(nRetCode);

	//////////////////////////////////////////////////////////////////////////

	nRetCode = piIniFile->GetInteger(SECTION_MAIL, "MailDelaySecondOnText", 0, &nMailDelaySecondOnText);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MAIL, "MailDelaySecondOnMoney", 0, &nMailDelaySecondOnMoney);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MAIL, "MailDelaySecondOnItem", 0, &nMailDelaySecondOnItem);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MAIL, "MailSurvivalTime", 0, &nMailSurvivalTime);
	KGLOG_PROCESS_ERROR(nRetCode);

	//////////////////////////////////////////////////////////////////////////

	nRetCode = piIniFile->GetMultiInteger(SECTION_ITEM, "BankPackagePrice", nBankPackagePrice, MAX_BANK_PACKAGE_COUNT);
	KGLOG_PROCESS_ERROR(nRetCode == MAX_BANK_PACKAGE_COUNT);

	for (int i = 0; i < MAX_BANK_PACKAGE_COUNT; ++i)
	{
		KGLOG_PROCESS_ERROR(nBankPackagePrice[i] >= 0);
	}

	//////////////////////////////////////////////////////////////////////////

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveInterval", 0, &nPartyAttractiveInterval);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveRange", 0, &nPartyAttractiveRange);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveUpLimit", 0, &nPartyAttractiveUpLimit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveAdd", 0, &nPartyAttractiveAdd);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveIntervalOnlyGroup", 0, &nPartyAttractiveIntervalOnlyGroup);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveRangeOnlyGroup", 0, &nPartyAttractiveRangeOnlyGroup);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveUpLimitOnlyGroup", 0, &nPartyAttractiveUpLimitOnlyGroup);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "PartyAttractiveAddOnlyGroup", 0, &nPartyAttractiveAddOnlyGroup);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "DuelAttractiveDailyCount", 0, &nDuelAttractiveDailyCount);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "DuelAttractiveUpLimit", 0, &nDuelAttractiveUpLimit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "DuelAttractiveAdd", 0, &nDuelAttractiveAdd);
	KGLOG_PROCESS_ERROR(nRetCode);
	if (nDuelAttractiveDailyCount >= 1 << 4)
	{
		KGLogPrintf(KGLOG_ERR, "[ConstList] DuelAttractiveDailyCount overflow: %d, max: %d", nDuelAttractiveDailyCount, (1 << 4) -1);
		nDuelAttractiveDailyCount = (1 << 4) - 1;
	}

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "AssistAttractiveUpLimit", 0, &nAssistAttractiveUpLimit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "WhisperAttractiveDailyCount", 0, &nWhisperAttractiveDailyCount);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "WhisperAttractiveUpLimit", 0, &nWhisperAttractiveUpLimit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_FELLOWSHIP, "WhisperAttractiveAdd", 0, &nWhisperAttractiveAdd);
	KGLOG_PROCESS_ERROR(nRetCode);
	if (nWhisperAttractiveDailyCount >= 1 << 4)
	{
		KGLogPrintf(KGLOG_ERR, "[ConstList] WhisperAttractiveDailyCount overflow: %d, max: %d", nWhisperAttractiveDailyCount, (1 << 4) -1);
		nWhisperAttractiveDailyCount = (1 << 4) - 1;
	}

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "SaveInterval", 0, &nSaveInterval);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "DrowningTime", 0, &nDrowningTime);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "CorpseGenerationFrame", 0, &nCorpseGenerationFrame);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "PlayerMaxLevel", 0, &nPlayerMaxLevel);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "RiseFrame", 0, &nRiseFrame);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "SkidFrame", 0, &nSkidFrame);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_MISC, "NoAddSkillExp", 0, &nNoAddSkillExp);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetMultiInteger(SECTION_MISC, "PlayerAndQuestDiffLevel", nPlayerAndQuestDiffLevel, PLAYER_AND_QUEST_DIFF_LEVEL_COUNT);
	KGLOG_PROCESS_ERROR(nRetCode == PLAYER_AND_QUEST_DIFF_LEVEL_COUNT);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "ReducePrestigeOnDeath", 0, &nReducePrestigeOnDeath);
	KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(nReducePrestigeOnDeath <= 0);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "SoloKill", 0, &nSoloKill);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "LastHit", 0, &nLastHit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "CommonKill", 0, &nCommonKill);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "KillerPartyMember", 0, &nKillerPartyMember);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "SameCampKill", 0, &nSameCampKill);
	KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(nSameCampKill <= 0);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "SameCampAssistKill", 0, &nSameCampAssistKill);
	KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(nSameCampAssistKill <= 0);

	nRetCode = piIniFile->GetInteger(SECTION_CAMP, "ResetKilledCountCycle", 0, &nResetKilledCountCycle);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetMultiInteger(SECTION_CAMP, "KilledCountPercent", nKilledCountPercent, MAX_KILL_COUNT);
	KGLOG_PROCESS_ERROR(nRetCode == MAX_KILL_COUNT);

	nRetCode = piIniFile->GetInteger(SECTION_TALK, "SceneChannelCoolDownID", 0, &nSceneChannelCoolDownID);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_TALK, "WorldChannelCoolDownID", 0, &nWorldChannelCoolDownID);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_TALK, "ForceChannelCoolDownID", 0, &nForceChannelCoolDownID);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_TALK, "ForceChannelDailyLimit", 0, &nForceChannelDailyLimit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_TALK, "CampChannelCoolDownID", 0, &nCampChannelCoolDownID);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_TALK, "CampChannelDailyLimit", 0, &nCampChannelDailyLimit);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = piIniFile->GetInteger(SECTION_ITEM, "NewUseItemStyle", 0, &bNewUseItemStyle);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return bResult;
}

