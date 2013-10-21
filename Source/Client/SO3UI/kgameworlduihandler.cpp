#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "SO3Represent/SO3RepresentHandler.h"
#include "./kgameworlduihandler.h"
#include "./SO3UI.h"
#include "Engine/KLuaPackage.h"

//#define SO3UI_TRACE_GAMEWORLD_UI_HANDLER 1

KGameWorldUIHandler g_GameWorldUIHandler;

KGameWorldUIHandler::KGameWorldUIHandler()
{
	m_bLogin = false;
}

KGameWorldUIHandler::~KGameWorldUIHandler()
{
}

void KGameWorldUIHandler::Release()
{
}

BOOL KGameWorldUIHandler::GetPassword(LPCSTR szTreePath, char szDestMD5String[64])
{
	if (g_pSO3UI->m_pUI && g_pSO3UI->m_pUI->GetPassword(szTreePath, szDestMD5String))
		return TRUE;
	return FALSE;
}

int KGameWorldUIHandler::Init()
{
	return true;
}

void KGameWorldUIHandler::Exit()
{
}

void KGameWorldUIHandler::OnOpenDoodad(KUIEventOpenDoodad const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("OPEN_DOODAD", 0);
	}
}

void KGameWorldUIHandler::OnCloseDoodad(KUIEventCloseDoodad const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("CLOSE_DOODAD", 0);
	}
}

void KGameWorldUIHandler::OnSyncLootList(KUIEventSyncLootList const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->FireEvent("SYNC_LOOT_LIST", 0);
	}
}

void KGameWorldUIHandler::OnLootItem(KUIEventLootItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwItemID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwCount);
		g_pSO3UI->m_pUI->FireEvent("LOOT_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnDistributeItem(KUIEventDistributeItem const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwItemID);
		g_pSO3UI->m_pUI->FireEvent("DISTRIBUTE_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnBeginRollItem(KUIEventBeginRollItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwItemID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nLeftRollFrame);
		g_pSO3UI->m_pUI->FireEvent("BEGIN_ROLL_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnRollItem(KUIEventRollItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwItemID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nRandom);
		g_pSO3UI->m_pUI->FireEvent("ROLL_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnCancelRollItem(KUIEventCancelRollItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwItemID);
		g_pSO3UI->m_pUI->FireEvent("CANCEL_ROLL_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnQuestAccepted(KUIEventQuestAccepted const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nQuestIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwQuestID);
		g_pSO3UI->m_pUI->FireEvent("QUEST_ACCEPTED", 0);
	}
}

void KGameWorldUIHandler::OnQuestFailed(KUIEventQuestFailed const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nQuestIndex);
		g_pSO3UI->m_pUI->FireEvent("QUEST_FAILED", 0);
	}
}

void KGameWorldUIHandler::OnQuestCanceled(KUIEventQuestCanceled const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwQuestID);
		g_pSO3UI->m_pUI->FireEvent("QUEST_CANCELED", 0);
	}
}

void KGameWorldUIHandler::OnFinishQuest(KUIEventFinishQuest const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwQuestID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.bForceFinish);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bAssist ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg3", param.nAddStamina);
        g_pSO3UI->m_pUI->PushArg("arg4", param.nAddThew);
		g_pSO3UI->m_pUI->FireEvent("QUEST_FINISHED", 0);
	}
}

void KGameWorldUIHandler::OnQuestShared(KUIEventQuestShared const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSrcPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwQuestID);
		g_pSO3UI->m_pUI->FireEvent("QUEST_SHARED", 0);
	}
}

void KGameWorldUIHandler::OnQuestDataUpdate(KUIEventQuestDataUpdate const & param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nQuestIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.eEventType);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nValue1);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nValue2);
		g_pSO3UI->m_pUI->FireEvent("QUEST_DATA_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnQuestListUpdate(void)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("QUEST_LIST_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnStartEscortQuest(KUIEventStartEscortQuest const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwStartPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwQuestID);
		g_pSO3UI->m_pUI->FireEvent("START_ESCORT_QUEST", 0);
	}
}

void KGameWorldUIHandler::OnQuestTimeUpdate(KUIEventQuestTimeUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nQuestIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nLastTime);
		g_pSO3UI->m_pUI->FireEvent("QUEST_TIME_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnQuestMarkUpdate(KUIEventQuestMarkUpdate const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
        g_pSO3UI->m_pUI->FireEvent("QUEST_MARK_UPDATE", 0);
    }
}

void KGameWorldUIHandler::OnCharacterSay(KUIEventCharacterSay const& param)
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.pszDialogKey);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTalkChannel);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwDialogID);
		g_pSO3UI->m_pUI->FireEvent("CHARACTER_SAY", param.dwCharacterID);
	}
}

void KGameWorldUIHandler::OnNpcTalk(KUIEventNpcTalk const& param)
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.pszDialogKey);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTalkChannel);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwDialogID);
		g_pSO3UI->m_pUI->FireEvent("NPC_TALK", param.dwTalkChannel);
	}
}

void KGameWorldUIHandler::OnItemListUpdate(KUIEventItemListUpdate const& param)
{
	static struct
	{
		LPCSTR	szEvent;
		DWORD	dwBoxOffset;
	} 
    s_aEvent[ibTotal] =
	{
		{ "EQUIP_ITEM_UPDATE",	ibEquip }, 		// ibEquip
		{ "BAG_ITEM_UPDATE",	ibPackage },	// ibPackage
		{ "BAG_ITEM_UPDATE",	ibPackage1 },	// ibPackage1
		{ "BAG_ITEM_UPDATE",	ibPackage2 },	// ibPackage2
		{ "BAG_ITEM_UPDATE",	ibPackage3 },	// ibPackage3
		{ "BAG_ITEM_UPDATE",	ibPackage4 },	// ibPackage4
		{ "BANK_ITEM_UPDATE",	ibBank },		// ibBank
		{ "BAG_ITEM_UPDATE",	ibBankPackage1 },
		{ "BAG_ITEM_UPDATE",	ibBankPackage2 },
		{ "BAG_ITEM_UPDATE",	ibBankPackage3 },
		{ "BAG_ITEM_UPDATE",	ibBankPackage4 },
		{ "BAG_ITEM_UPDATE",	ibBankPackage5 },
		{ "SOLD_ITEM_UPDATE",	ibSoldList },	 // ibSoldList
	};

	if (g_pSO3UI->m_pUI && !m_bLogin && param.dwBoxIndex < ibTotal)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwBoxIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwX);
		g_pSO3UI->m_pUI->FireEvent(s_aEvent[param.dwBoxIndex].szEvent, 0);
	}
}

void KGameWorldUIHandler::OnItemExchanged(KUIEvnetItemExchanged const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwBox1);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwX1);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwBox2);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwX2);
		g_pSO3UI->m_pUI->FireEvent("EXCHANGE_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnDestroyItem(KUIEventDestroyItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwBoxIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwX);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nVersion);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwTabType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwIndex);
		g_pSO3UI->m_pUI->FireEvent("DESTROY_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnMoneyUpdate(const KUIEventMoneyUpdate &param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nMoney);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nPrevMoney);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bShowMsg ? true : false);
		g_pSO3UI->m_pUI->FireEvent("MONEY_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnContributionUpdate(KUIEventContributionUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
        g_pSO3UI->m_pUI->PushArg("arg0", param.nPreContribution);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nContribution);
		g_pSO3UI->m_pUI->FireEvent("CONTRIBUTION_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnSysMsg(KUIEventSysErrorMsg const& param)
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SYS_ERROR));
		g_pSO3UI->m_pUI->PushArg("arg1", param.pszText);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}


void KGameWorldUIHandler::OnOpenWindow(KUIEventOpenWindow const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.szText);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTargetType);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwTargetID);
		g_pSO3UI->m_pUI->FireEvent("OPEN_WINDOW", 0);
	}
}


void KGameWorldUIHandler::OnSceneBeginLoad(KUIEventSceneBeginLoad const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSceneBeginLoad(%u, %s) begin\n", param.dwSceneID, param.szPath);
#endif

	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg1", param.szPath);
		g_pSO3UI->m_pUI->FireEvent("SCENE_BEGIN_LOAD", 0);
	}
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSceneBeginLoad(%u, %s) end\n", param.dwSceneID, param.szPath);
#endif
}


void KGameWorldUIHandler::OnSceneEndLoad(KUIEventSceneEndLoad const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSceneEndLoad(%u) begin\n", param.dwSceneID);
#endif
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSceneID);
		g_pSO3UI->m_pUI->FireEvent("SCENE_END_LOAD", 0);
	}
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSceneEndLoad(%u) end\n", param.dwSceneID);
#endif
}

void KGameWorldUIHandler::OnSyncRoleDataBegin()
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSyncRoleDataBegin() begin\n");
#endif

	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->FireEvent("SYNC_ROLE_DATA_BEGIN", 0);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSyncRoleDataBegin() end\n");
#endif
}

void KGameWorldUIHandler::OnSyncRoleDataEnd()
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSyncRoleDataEnd() begin\n");
#endif

	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->FireEvent("SYNC_ROLE_DATA_END", 0);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSyncRoleDataEnd() end\n");
#endif
}

void KGameWorldUIHandler::OnCurrentPlayerForceChanged(KUIEventCurrentPlayerForceChanged const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCurrentPlayerID);
		g_pSO3UI->m_pUI->FireEvent("CURRENT_PLAYER_FORCE_CHANGED", param.dwCurrentPlayerID);
	}
}

void KGameWorldUIHandler::OnSkillUpdate(KUIEventSkillUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("SKILL_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnSkillExpUpdate(KUIEventSkillUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("SKILL_EXP_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnCraftUpdate(KUIEventCraftUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
		g_pSO3UI->m_pUI->FireEvent("CRAFT_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnCraftRemove(KUIEventCraftRemove const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
		g_pSO3UI->m_pUI->FireEvent("CRAFT_REMOVE", 0);
	}
}

void KGameWorldUIHandler::OnRecipeUpdate(KUIEventRecipeUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfession);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCraftID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwRecipeID);
		g_pSO3UI->m_pUI->FireEvent("RECIPE_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnLearnProfession(KUIEventLearnProfession const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_LEARN_PROFESSION));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);

		g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
		g_pSO3UI->m_pUI->FireEvent("LEARN_PROFESSION", 0);
    }
}

void KGameWorldUIHandler::OnLearnBranch(KUIEventLearnBranch const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_LEARN_BRANCH));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwBranchID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnForgetProfession(KUIEventForgetProfession const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_FORGET_PROFESSION));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);

        g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
        g_pSO3UI->m_pUI->FireEvent("FORGET_PROFESSION", 0);
    }
}

void KGameWorldUIHandler::OnAddProfessionProficiency(KUIEventAddProfessionProficiency const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_ADD_PROFESSION_PROFICIENCY));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwProficiency);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnProfessionLevelUp(KUIEventProfessionLevelUp const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_PROFESSION_LEVEL_UP));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwNewLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);

        g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwNewLevel);
        g_pSO3UI->m_pUI->FireEvent("PROFESSION_LEVEL_UP", 0);
    }
}	

void KGameWorldUIHandler::OnSetProfessAdjustLevel(KUIEventSetProfessionAdjustLevel const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SET_PROFESSION_ADJUST_LEVEL));
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwAdjustLevel);
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);

        g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwAdjustLevel);
        g_pSO3UI->m_pUI->FireEvent("PROFESSION_ADJUST_LEVEL_CHANGE", 0);
    }
}

void KGameWorldUIHandler::OnSetProfessionMaxLevel(KUIEventSetProfessionMaxLevel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SET_PROFESSION_MAX_LEVEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwProfessionID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwNewMaxLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);

		g_pSO3UI->m_pUI->PushArg("arg0", param.dwProfessionID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwNewMaxLevel);
		g_pSO3UI->m_pUI->FireEvent("PROFESSION_MAX_LEVEL_UP", 0);
    }
}

void KGameWorldUIHandler::OnLearnRecipe(KUIEventLearnRecipe const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_LEARN_RECIPE));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCraftID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwRecipeID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);

		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCraftID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwRecipeID);
		g_pSO3UI->m_pUI->FireEvent("LEARN_RECIPE", 0);
	}
}

void KGameWorldUIHandler::OnReputationRespond(KUIEventReputationRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwForceID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nNumber);
		g_pSO3UI->m_pUI->FireEvent("REPUTATION_LEVEL_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnCastingBarStart()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("CASTINGBAR_START", 0);
	}
}


void KGameWorldUIHandler::OnCastingBarEnd(KUIEventCastingBarEnd const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("CASTINGBAR_END", 0);
	}
}


void KGameWorldUIHandler::OnPlayerExperienceUpdate(KUIEventPlayerExperienceUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_EXPERIENCE_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnPlayerLevelUpdate(KUIEventPlayerLevelUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_LEVEL_UPDATE", param.dwPlayerID);

		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_LEVEL_UP));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nLevel);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nStrength);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nAgility);
		g_pSO3UI->m_pUI->PushArg("arg4", param.nVigor);
		g_pSO3UI->m_pUI->PushArg("arg5", param.nSpirit);
		g_pSO3UI->m_pUI->PushArg("arg6", param.nSpunk);
		g_pSO3UI->m_pUI->PushArg("arg7", param.nMaxLife);
		g_pSO3UI->m_pUI->PushArg("arg8", param.nMaxMana);
		g_pSO3UI->m_pUI->PushArg("arg9", param.nMaxRage);
		g_pSO3UI->m_pUI->PushArg("arg10", param.nMaxStamina);
		g_pSO3UI->m_pUI->PushArg("arg11", param.nMaxThew);
		g_pSO3UI->m_pUI->PushArg("arg12", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", param.dwPlayerID);
	}
}

void KGameWorldUIHandler::OnPlayerStateUpdate(KUIEventPlayerStateUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_STATE_UPDATE", param.dwPlayerID);
	}
}


void KGameWorldUIHandler::OnPlayerWeakUpdate(KUIEventPlayerWeakUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_WEAK_UPDATE", param.dwPlayerID);
	}
}

void KGameWorldUIHandler::OnPlayerDisplayDataUpdate(KUIEventPlayerDisplayDataUpdate const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnPlayerDisplayDataUpdate(%u) begin\n", param.dwPlayerID);
#endif

	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_DISPLAY_DATA_UPDATE", param.dwPlayerID);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnPlayerDisplayDataUpdate(%u) end\n", param.dwPlayerID);
#endif
}


void KGameWorldUIHandler::OnPlayerEnterScene(KUIEventPlayerEnterScene const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnPlayerEnterScene(%u) begin\n", param.dwPlayerID);
#endif

	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_ENTER_SCENE", param.dwPlayerID);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnPlayerEnterScene(%u) end\n", param.dwPlayerID);
#endif
}


void KGameWorldUIHandler::OnPlayerLeaveScene(KUIEventPlayerLeaveScene const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnPlayerLeaveScene(%u) begin\n", param.dwPlayerID);
#endif

	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_LEAVE_SCENE", param.dwPlayerID);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnPlayerLeaveScene(%u) end\n", param.dwPlayerID);
#endif
}


void KGameWorldUIHandler::OnNpcStateUpdate(KUIEventNpcStateUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("NPC_STATE_UPDATE", param.dwNpcID);
	}
}

void KGameWorldUIHandler::OnNpcDropTargetUpdate(KUIEventNpcDropTargetUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("NPC_DROP_TARGET_UPDATE", param.dwNpcID);
	}
}

void KGameWorldUIHandler::OnNpcWeakUpdate(KUIEventNpcWeakUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("NPC_WEAK_UPDATE", param.dwNpcID);
	}
}

void KGameWorldUIHandler::OnNpcDisplayDataUpdate(KUIEventNpcDisplayDataUpdate const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnNpcDisplayDataUpdate(%u) begin\n", param.dwNpcID);
#endif

    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("NPC_DISPLAY_DATA_UPDATE", param.dwNpcID);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnNpcDisplayDataUpdate(%u) end\n", param.dwNpcID);
#endif
}


void KGameWorldUIHandler::OnNpcEnterScene(KUIEventNpcEnterScene const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnNpcEnterScene(%u) begin\n", param.dwNpcID);
#endif

	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("NPC_ENTER_SCENE", param.dwNpcID);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnNpcEnterScene(%u) end\n", param.dwNpcID);
#endif
}


void KGameWorldUIHandler::OnNpcLeaveScene(KUIEventNpcLeaveScene const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnNpcLeaveScene(%u) begin\n", param.dwNpcID);
#endif

    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("NPC_LEAVE_SCENE", param.dwNpcID);
	}

#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
	KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnNpcLeaveScene(%u) end\n", param.dwNpcID);
#endif
}


void KGameWorldUIHandler::OnDoodadEnterScene(KUIEventDoodadEnterScene const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->FireEvent("DOODAD_ENTER_SCENE", param.dwDoodadID);
	}
}


void KGameWorldUIHandler::OnDoodadLeaveScene(KUIEventDoodadLeaveScene const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->FireEvent("DOODAD_LEAVE_SCENE", param.dwDoodadID);
	}
}


void KGameWorldUIHandler::OnSyncTeamBaseInfo(KUIEventSyncTeamBaseInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwLeaderID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nLootMode);
        g_pSO3UI->m_pUI->PushArg("arg3", param.nRollQuality);
		g_pSO3UI->m_pUI->PushArg("arg4", param.bAddTeamMemberFlag ? true : false);
		g_pSO3UI->m_pUI->FireEvent("PARTY_UPDATE_BASE_INFO", 0);
	}
}


void KGameWorldUIHandler::OnSyncTeamMemberData(KUIEventTeamSyncMember const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nGroupIndex);
        g_pSO3UI->m_pUI->FireEvent("PARTY_SYNC_MEMBER_DATA", 0);
	}
}

void KGameWorldUIHandler::OnTeamAddMember(KUIEventTeamAddMember const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nGroupIndex);
		g_pSO3UI->m_pUI->FireEvent("PARTY_ADD_MEMBER", 0);
	}
}


void KGameWorldUIHandler::OnTeamDelMember(KUIEventTeamDelMember const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.szName);
        g_pSO3UI->m_pUI->PushArg("arg3", param.nGroupIndex);
        g_pSO3UI->m_pUI->FireEvent("PARTY_DELETE_MEMBER", 0);
	}
}

void KGameWorldUIHandler::OnTeamInviteRequest(KUIEventTeamInviteRequest const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.szInviteSrc);
		g_pSO3UI->m_pUI->FireEvent("PARTY_INVITE_REQUEST", 0);
	}
}

void KGameWorldUIHandler::OnTeamApplyRequest(KUIEventTeamApplyRequest const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.szApplySrc);
		g_pSO3UI->m_pUI->FireEvent("PARTY_APPLY_REQUEST", 0);
	}
}

void KGameWorldUIHandler::OnTeamMessageNotify(KUIEventTeamMessageNotify const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nCode);
		g_pSO3UI->m_pUI->PushArg("arg1", param.szName);
		g_pSO3UI->m_pUI->FireEvent("PARTY_MESSAGE_NOTIFY", 0);
	}
}

void KGameWorldUIHandler::OnUpdateTeamMemberInfo(KUIEventUpdateTeamMemberInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
		g_pSO3UI->m_pUI->FireEvent("PARTY_UPDATE_MEMBER_INFO", param.dwMemberID);
	}
}

void KGameWorldUIHandler::OnTeamLootModeChanged(KUIEventTeamLootModeChanged const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nLootMode);
		g_pSO3UI->m_pUI->FireEvent("PARTY_LOOT_MODE_CHANGED", 0);
	}
}

void KGameWorldUIHandler::OnTeamSetDistributeManNotify(KUIEventSetDistributeMan const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDistributeMan);
		g_pSO3UI->m_pUI->FireEvent("PARTY_SET_DISTRIBUTE_MAN", 0);
	}
}

void KGameWorldUIHandler::OnTeamSetFormationLeaderNotify(KUIEventSetFormationLeader const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwFormationLeader);
		g_pSO3UI->m_pUI->FireEvent("PARTY_SET_FORMATION_LEADER", 0);
	}
}

void KGameWorldUIHandler::OnTeamSetMark()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("PARTY_SET_MARK", 0);
	}
}

void KGameWorldUIHandler::OnTeamCampChange()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("PARTY_CAMP_CHANGE", 0);
    }
}

void KGameWorldUIHandler::OnTeamLevelUpRaid()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("PARTY_LEVEL_UP_RAID", 0);
    }
}

void KGameWorldUIHandler::OnTeamChangeMemberGroup(KUIEventTeamChangeMemberGroup const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwMemberID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nSrcGroupIndex);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nDstGroupIndex);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwDstMemberID);
        g_pSO3UI->m_pUI->FireEvent("TEAM_CHANGE_MEMBER_GROUP", 0);
    }
}

void KGameWorldUIHandler::OnTeamRollQualityChanged(KUIEventTeamRollQualityChanged const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRollQuality);
		g_pSO3UI->m_pUI->FireEvent("PARTY_ROLL_QUALITY_CHANGED", 0);
	}
}

void KGameWorldUIHandler::OnSetTeamMemberOnlineFlag(KUIEventSetTeamMemberOnlineFlag const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bOnlineFlag);
		g_pSO3UI->m_pUI->FireEvent("PARTY_SET_MEMBER_ONLINE_FLAG", param.dwMemberID);
	}
}

void KGameWorldUIHandler::OnTeamMemberSignpostNotify(KUIEventTeamMemberSignpostNotify const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nX);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nY);
		g_pSO3UI->m_pUI->FireEvent("PARTY_NOTIFY_SIGNPOST", 0);
	}
}

void KGameWorldUIHandler::OnUpdateTeamMemberLMR(KUIEventUpdateTeamMemberLMR const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
		g_pSO3UI->m_pUI->FireEvent("PARTY_UPDATE_MEMBER_LMR", param.dwMemberID);
	}
}

void KGameWorldUIHandler::OnUpdateTeamMemberPosition(KUIEventUpdateTeamMemberInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwMemberID);
		g_pSO3UI->m_pUI->FireEvent("PARTY_UPDATE_MEMBER_POSITION", param.dwMemberID);
	}
}

void KGameWorldUIHandler::OnTeamDisband(KUIEventTeamDisband const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->FireEvent("PARTY_DISBAND", 0);
	}
}

void KGameWorldUIHandler::OnTeamLeaderChanged(KUIEventTeamLeaderChanged const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTeamID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwNewLeaderID);
		g_pSO3UI->m_pUI->FireEvent("PARTY_LEADER_CHANGED", 0);
	}
}

void KGameWorldUIHandler::OnTeamAuthorityChanged(KUIEventTeamAuthorityChanged const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nAuthorityType);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTeamID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwOldAuthorityID);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwNewAuthorityID);
        g_pSO3UI->m_pUI->FireEvent("TEAM_AUTHORITY_CHANGED", 0);
    }
}

void KGameWorldUIHandler::OnShopUpdateItem(KUIEventShopUpdateItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwShopID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPageIndex);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwPos);
		g_pSO3UI->m_pUI->FireEvent("SHOP_UPDATEITEM", 0);
	}
}

void KGameWorldUIHandler::OnShopOpenNotify(KUIEventShopOpenNotify const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwShopID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nShopType);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwValidPage);
		g_pSO3UI->m_pUI->PushArg("arg3", param.bCanRepair ? true : false);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("SHOP_OPENSHOP", 0);
	}
}

void KGameWorldUIHandler::OnTradingInvite(KUIEventTradingInvite const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTradingInviter);
		g_pSO3UI->m_pUI->FireEvent("TRADING_INVITE", 0);
	}
}

void KGameWorldUIHandler::OnTradingOpenNotify(KUIEventTradingOpenNotify const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->FireEvent("TRADING_OPEN_NOTIFY", 0);
	}
}

void KGameWorldUIHandler::OnTradingUpdateConfirm(KUIEventTradingUpdateConfirm const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.bConfirm ? true : false);
		g_pSO3UI->m_pUI->FireEvent("TRADING_UPDATE_CONFIRM", 0);
	}
}

void KGameWorldUIHandler::OnTradingUpdateItem(KUIEventTradingUpdateItem const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwBoxIndex);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwPosIndex);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwGridIndex);
		g_pSO3UI->m_pUI->FireEvent("TRADING_UPDATE_ITEM", 0);
	}
}

void KGameWorldUIHandler::OnTradingUpdateMoney(KUIEventTradingUpdateMoney const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nMoney);
		g_pSO3UI->m_pUI->FireEvent("TRADING_UPDATE_MONEY", 0);
	}
}

void KGameWorldUIHandler::OnTradingClose(KUIEventTradingClose const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("TRADING_CLOSE", 0);
	}
}

void KGameWorldUIHandler::OnSkillCastLog(KUIEventSkillCastLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_CAST_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillCastRespondLog(KUIEventSkillCastRespondLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_CAST_RESPOND_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwLevel);
		g_pSO3UI->m_pUI->PushArg("arg4", param.nRespond);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillEffectLog(KUIEventSkillEffectLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_EFFECT_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg3", param.bReact);
		g_pSO3UI->m_pUI->PushArg("arg4", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwID);
		g_pSO3UI->m_pUI->PushArg("arg6", param.dwLevel);
		g_pSO3UI->m_pUI->PushArg("arg7", param.bCriticalStrike ? true : false);
		g_pSO3UI->m_pUI->PushArg("arg8", param.nDamageType);
		g_pSO3UI->m_pUI->PushArg("arg9", param.nValue);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillBlockLog(KUIEventSkillBlockLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_BLOCK_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwID);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwLevel);
		g_pSO3UI->m_pUI->PushArg("arg6", param.nDamageType);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillShieldLog(KUIEventSkillShieldLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_SHIELD_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwID);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillMissLog(KUIEventSkillMissLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_MISS_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwID);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillHitLog(KUIEventSkillHitLog const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_HIT_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwID);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    }
}

void KGameWorldUIHandler::OnSkillDodgeLog(KUIEventSkillDodgeLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_DODGE_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwID);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnBuffLog(KUIEventBuffLog const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_BUFF_LOG));
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bCanCancel ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwID);
        g_pSO3UI->m_pUI->PushArg("arg4", param.bAddOrDel);
        g_pSO3UI->m_pUI->PushArg("arg5", param.nLevel);
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    }
}

void KGameWorldUIHandler::OnBuffImmunityLog(KUIEventBuffImmunityLog const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_BUFF_IMMUNITY));
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTarget);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bCanCancel ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwID);
        g_pSO3UI->m_pUI->PushArg("arg4", param.nLevel);
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    }   
}

void KGameWorldUIHandler::OnExpLog(KUIEventExpLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_EXP_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nAddExp);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnCommonHealthLog(KUIEventCommonHealthLog& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_COMMON_HEALTH_LOG));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nDeltaLife);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);    
	}
}

void KGameWorldUIHandler::OnCommonHealthText(KUIEventCommonHealthText& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nDeltaLife);
		g_pSO3UI->m_pUI->FireEvent("COMMON_HEALTH_TEXT", 0);
	}
}

void KGameWorldUIHandler::OnCharacterDeath(KUIEventCharacterDeath const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_DEATH_NOTIFY));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg2", INT_MAX);
		g_pSO3UI->m_pUI->PushArg("arg3", param.szKiller);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", param.dwCharacterID);
	}
}

void KGameWorldUIHandler::OnClientPlayerDeath()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("PLAYER_DEATH", 0);
	}
}

void KGameWorldUIHandler::OnSyncPlayerRevive(KUISyncPlayerRevive const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
		g_pSO3UI->m_pUI->PushArg("arg0", param.bReviveInSite ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg1", param.bReviveInAlter ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg2", param.bReviveByPlayer ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg3", param.nLeftReviveFrame);
        g_pSO3UI->m_pUI->PushArg("arg4", param.dwReviver);
        g_pSO3UI->m_pUI->PushArg("arg5", param.nMessageID);
        g_pSO3UI->m_pUI->FireEvent("SYNC_PLAYER_REVIVE", 0);
    }
}

void KGameWorldUIHandler::OnFightHint(KUIEventFightHint const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.bFight ? true : false);
		g_pSO3UI->m_pUI->FireEvent("FIGHT_HINT", 0);
	}
}

void KGameWorldUIHandler::OnSkillEffectText(KUIEventSkillEffectText const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCasterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bCriticalStrike ? true : false);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.nValue);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg6", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("SKILL_EFFECT_TEXT", 0);
	}
}


void KGameWorldUIHandler::OnSkillMissText(KUIEventSkillMissText const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCasterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetID);
		g_pSO3UI->m_pUI->FireEvent("SKILL_MISS", 0);
	}
}

void KGameWorldUIHandler::OnSkillBlockText(KUIEventSkillBlockText const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCasterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetID);
		g_pSO3UI->m_pUI->FireEvent("SKILL_BLOCK", 0);
	}
}

void KGameWorldUIHandler::OnSkillBuffText(KUIEventSkillBuffText const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.bCanCancel ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwID);
        g_pSO3UI->m_pUI->PushArg("arg3", param.nLevel);
        g_pSO3UI->m_pUI->FireEvent("SKILL_BUFF", 0);
    }
}

void KGameWorldUIHandler::OnBuffImmunityText(KUIEventBuffImmunityText const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->FireEvent("BUFF_IMMUNITY", 0);
	}
}

void KGameWorldUIHandler::OnSkillDodgeText(KUIEventSkillDodgeText const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCasterID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetID);
		g_pSO3UI->m_pUI->FireEvent("SKILL_DODGE", 0);
	}
}

void KGameWorldUIHandler::OnSkillCastRespond(KUIEventSkillCastRespond  const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SKILL_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSkillMountKungFu(KUIEventSkillUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("SKILL_MOUNT_KUNG_FU", 0);
	}
}

void KGameWorldUIHandler::OnSkillUnmountKungfu(KUIEventSkillUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("SKILL_UNMOUNT_KUNG_FU", 0);
	}
}

void KGameWorldUIHandler::OnBuffUpdata(KUIBuffUpdata  const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.bDelete ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nIndex);
        g_pSO3UI->m_pUI->PushArg("arg3", param.bCanCancel ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg4", param.dwBuffID);
        g_pSO3UI->m_pUI->PushArg("arg5", param.nCount);
        g_pSO3UI->m_pUI->PushArg("arg6", param.nEndFrame);
        g_pSO3UI->m_pUI->PushArg("arg7", param.bInit ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg8", param.nBuffLevel);
        g_pSO3UI->m_pUI->FireEvent("BUFF_UPDATE", 0);
    }
}

void KGameWorldUIHandler::OnOpenRecipeBook(KUIEventOpenRecipeBook const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwItemID);
		g_pSO3UI->m_pUI->FireEvent("OPEN_RECIPE_BOOK", 0);
	}
}

void KGameWorldUIHandler::OnOpenBook(KUIEventOpenBook const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwBookID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSubID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwItemID);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwRecipeID);
		g_pSO3UI->m_pUI->FireEvent("OPEN_BOOK", 0);
	}
}

void KGameWorldUIHandler::OnUpdateBookState(KUIEventUpdateBookState const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwBookID);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_BOOK_STATE", 0);
	}
}

void KGameWorldUIHandler::OnPeekPlayerBookState(KUIEventPeekPlayerBookState const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PEEK_PLAYER_BOOK_STATE", 0);
	}
}

void KGameWorldUIHandler::OnPeekPlayerQuest(KUIEventPeekPlayerQuest const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PEEK_PLAYER_QUEST", 0);
	}
}

void KGameWorldUIHandler::OnSkillPrepareProgress(KUIEventSkillProgress const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nTotalFrame);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("DO_SKILL_PREPARE_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnSkillChannelProgress(KUIEventSkillProgress const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nTotalFrame);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwSkillLevel);
		g_pSO3UI->m_pUI->FireEvent("DO_SKILL_CHANNEL_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnSkillAccumulateProgress(KUIEventSkillProgress const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
		g_pSO3UI->m_pUI->PushArg("arg0", param.nTotalFrame);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwSkillLevel);
        g_pSO3UI->m_pUI->FireEvent("DO_SKILL_ACCUMULATE_PROGRESS", 0);
    }
}

void KGameWorldUIHandler::OnSkillCast(KUIEventSkillCastLog const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCaster);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwLevel);
		g_pSO3UI->m_pUI->FireEvent("DO_SKILL_CAST", 0);
	}
}

void KGameWorldUIHandler::OnSkillAccumulateSuccess(KUIEventSkillProgress const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSkillID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSkillLevel);
        g_pSO3UI->m_pUI->FireEvent("DO_SKILL_ACCUMULATE_SUCCESS", 0);
    }
}

void KGameWorldUIHandler::OnRecipePrepareProgress(KUIEventRecipeProgress const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nTotalFrame);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwCraftID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwRecipeID);
		g_pSO3UI->m_pUI->FireEvent("DO_RECIPE_PREPARE_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnOTActionProgressUpdate(int nFrame)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", nFrame);
		g_pSO3UI->m_pUI->FireEvent("OT_ACTION_PROGRESS_UPDATE", 0);
	}
}


void KGameWorldUIHandler::OnOTActionProgressBreak(DWORD dwCharacterID)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", dwCharacterID);
		g_pSO3UI->m_pUI->FireEvent("OT_ACTION_PROGRESS_BREAK", 0);
	}
}

void KGameWorldUIHandler::OnItemUseRespond(KUIEventItemUseRespond  const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_ITEM_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnAddItemRespond(KUIEventAddItemRespond  const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_ADD_ITEM_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
        g_pSO3UI->m_pUI->PushArg("arg2", param.szItemName);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnUseItemRespond(KUIEventUseItemRespond  const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_USE_ITEM_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwProfessionID);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwProfessionBranchID);
        g_pSO3UI->m_pUI->PushArg("arg4", param.dwProfessionLevel);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnTradingRespond(KUIEventTradingRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_TRADING_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnLootRespond(KUIEventLootRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_LOOT_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnCraftCastRespond(KUIEventCraftCastRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_CRAFT_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nCraftCastRespond);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwCraftID);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwRecipeID);
		g_pSO3UI->m_pUI->PushArg("arg4", param.dwTargetType);
		g_pSO3UI->m_pUI->PushArg("arg5", param.dwTargetID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnQuestRespond(KUIEventQuestRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_QUEST_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nQuestRespond);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwQuestID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnPickPrepareProgress(KUIEventPickProgress const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nTotalFrame);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwDoodadID);
		g_pSO3UI->m_pUI->FireEvent("DO_PICK_PREPARE_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnCustomOTActionProgress(KUIEventCustomOTActionProgress const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nTotalFrame);
		g_pSO3UI->m_pUI->PushArg("arg1", param.szActionName);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nType);
		g_pSO3UI->m_pUI->FireEvent("DO_CUSTOM_OTACTION_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnLoginNotify(KUIEventLoginNotify const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nEvent);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szMatrixPosition);
		g_pSO3UI->m_pUI->FireEvent("LOGIN_NOTIFY", 0);
	}
}

void KGameWorldUIHandler::OnLoginQueueState(KUIEventLoginQueueState const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nPosition);
		g_pSO3UI->m_pUI->FireEvent("LOGIN_QUEUE_STATE", 0);
	}
}

void KGameWorldUIHandler::OnShopRespond(KUIEventShopRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SHOP_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nMoney);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnPKRespond(KUIEventPKRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_PK_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}	

void KGameWorldUIHandler::OnSendMailResult(KUIEventSendMailResult const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nIndex);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nResult);
		g_pSO3UI->m_pUI->FireEvent("SEND_MAIL_RESULT", 0);
	}
}

void KGameWorldUIHandler::OnMailRespond(KUIEventMailRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_MAIL_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnMailListUpdate(KUIEventMailListUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nUnreadCount);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nTotalCount);
		g_pSO3UI->m_pUI->FireEvent("MAIL_LIST_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnGetMailContent(KUIEventGetMailContent const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwMailID);
		g_pSO3UI->m_pUI->FireEvent("GET_MAIL_CONTENT", 0);
	}
}

void KGameWorldUIHandler::OnGetGmMessageMail(KUIEventGetMailContent const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwMailID);
        g_pSO3UI->m_pUI->FireEvent("GET_GM_MESSAGE_MAIL", 0);
    }
}

void KGameWorldUIHandler::OnGetPlayerMessageMail(KUIEventGetMailContent const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwMailID);
        g_pSO3UI->m_pUI->FireEvent("GET_PLAYER_MESSAGE_MAIL", 0);
    }    
}

void KGameWorldUIHandler::OnNewMailNotify(KUIEventNewMailNotify const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_MAIL_COUNT_INFO));
        g_pSO3UI->m_pUI->PushArg("arg1", param.nUnreadCount);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nTotalCount);
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    }
}

void KGameWorldUIHandler::OnUpdateSelectTarget()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("UPDATE_SELECT_TARGET", 0);
	}
}

void KGameWorldUIHandler::OnPlayerTalk(KUIEventPlayerTalk const& param)
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwTalkerID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTalkChannel);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bEcho ? true : false);
		g_pSO3UI->m_pUI->PushArg("arg3", param.szName);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_TALK", param.dwTalkerID);
	}
}

void KGameWorldUIHandler::OnNpcTalkUseSentenceID(KUIEventNpcTalkUseSentenceID const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwTalkerID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTalkChannel);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwSentenceID);
        g_pSO3UI->m_pUI->PushArg("arg3", param.szTalkerName);
        g_pSO3UI->m_pUI->FireEvent("NPC_TALK_USE_SENTENCE_ID", param.dwTalkerID);
    }
}

void KGameWorldUIHandler::OnResponseMsg(KUIEventResponseMsg const& param)
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_CHAT_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nRespondCode);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnFellowshipUpdate()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("PLAYER_FELLOWSHIP_UPDATE", 0);
	}
}

void KGameWorldUIHandler::OnFoeUpdate()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("PLAYER_FOE_UPDATE", 0);
    }
}

void KGameWorldUIHandler::OnBlackListUpdate()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("PLAYER_BLACK_LIST_UPDATE", 0);
    }
}

void KGameWorldUIHandler::OnFellowshipRespond(KUIEventFellowshipRespond const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_FELLOWSHIP_RESPOND));
		g_pSO3UI->m_pUI->PushArg("arg1", param.nFellowshipRespond);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnFellowshipChange(KUIEventFellowshipChange const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nRespondCode);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwValue1);
		g_pSO3UI->m_pUI->PushArg("arg3", param.dwValue2);
		g_pSO3UI->m_pUI->PushArg("arg4", param.szName);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_FELLOWSHIP_CHANGE", 0);
	}
}

void KGameWorldUIHandler::OnBeAddFellowship(KUIEventBeAddFellowship const& param)
{
    if (g_pSO3UI->m_pUI)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nRespondCode);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.szName);
        g_pSO3UI->m_pUI->FireEvent("PLAYER_BE_ADD_FELLOWSHIP", 0);
    }
}

void KGameWorldUIHandler::OnFellowshipLogin(KUIEventFellowshipNotify const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.bLoad ? true : false);
		g_pSO3UI->m_pUI->PushArg("arg1", param.szName);
		g_pSO3UI->m_pUI->PushArg("arg2", param.bFoe ? true : false);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_FELLOWSHIP_LOGIN", 0);
	}
}

void KGameWorldUIHandler::OnAddFellowshipAttraction(KUIEventAddFellowshipAttraction const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szAlliedPlayerName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nAttaction);
        g_pSO3UI->m_pUI->FireEvent("PLAYER_ADD_FELLOWSHIP_ATTRACTION", 0);
    }
}

void KGameWorldUIHandler::OnFellowshipAttractionFallOff()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("PLAYER_FELLOWSHIP_ATTRACTION_FALL_OFF", 0);
    }
}

void KGameWorldUIHandler::OnUpdateMiniMapNpcMark(KUIEventUpdateMiniMapNpcMark const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nX);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nY);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_NPC_MINIMAP_MARK", param.dwNpcID);
	}
}

void KGameWorldUIHandler::OnUpdateMiniMapDoodadMark(KUIEventUpdateMiniMapDoodadMark const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwDoodadID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nX);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nY);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nParam);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_DOODAD_MINIMAP_MARK", param.dwDoodadID);
	}
}

void KGameWorldUIHandler::OnUpdateRelation(KUIEventUpdateRelation const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwCharacterID);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_RELATION", param.dwCharacterID);
	}
}

void KGameWorldUIHandler::OnUpdateAllRelation()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("UPDATE_ALL_RELATION", 0);
	}
}

void KGameWorldUIHandler::OnUpdateKillPoint(KUIEventKillPoint const& Param) 
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("UPDATE_KILL_POINT", 0);
	}
}

void KGameWorldUIHandler::OnApplyDuel(KUIEventApplyDuel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwSrcPlayerID);
		g_pSO3UI->m_pUI->FireEvent("APPLY_DUEL", 0);

		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_APPLY_DUEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSrcPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwDstPlayerID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnLeaveDuel()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("LEAVE_DUEL", 0);
	}
}

void KGameWorldUIHandler::OnReturnDuel()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("RETURN_DUEL", 0);
	}
}

void KGameWorldUIHandler::OnAcceptDuel(KUIEventAcceptDuel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_ACCEPT_DUEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSrcPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwDstPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg3", param.nEndFrame);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnRefuseDuel(KUIEventRefuseDuel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_REFUSE_DUEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwSrcPlayerID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwDstPlayerID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnStartDuel(KUIEventStartDuel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_START_DUEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetPlayerID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnCancelDuel(KUIEventCancelDuel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_CANCEL_DUEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetPlayerID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnFinishDuel()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("FINISH_DUEL", 0);

        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_FINISH_DUEL));
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    }
}

void KGameWorldUIHandler::OnWinDuel(KUIEventWinDuel const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_WIN_DUEL));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwWinnerID);
		g_pSO3UI->m_pUI->PushArg("arg2", param.dwLosserID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnApplySlay(KUIEventSlayInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_APPLY_SLAY));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nSeconds);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnStartSlay(KUIEventSlayInfo const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_START_SLAY));
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    }
}

void KGameWorldUIHandler::OnCloseSlay(KUIEventSlayInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_CLOSE_SLAY));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nSeconds);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnSlayClosed(KUIEventSlayInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_SLAY_CLOSED));
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnOpenBank(KUIEventOpenBank const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwNpcID);
		g_pSO3UI->m_pUI->FireEvent("OPEN_BANK", 0);
	}
}

void KGameWorldUIHandler::OnUpdateBankSlot()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("UPDATE_BANK_SLOT", 0);
	}
}

void KGameWorldUIHandler::OnUpdateMapMark(KUIEventUpdateMapMark const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nX);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nY);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nZ);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.szComment);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_MAP_MARK", 0);
	}
}

void KGameWorldUIHandler::OnUpdateRegionInfo(KUIEventUpdateRegionInfo const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwRegionInfo);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_REGION_INFO", 0);
	}
}

void KGameWorldUIHandler::OnUpdateMidMapMark(KUIEventUpdateMidMapMark const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nMapID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.nX);
		g_pSO3UI->m_pUI->PushArg("arg2", param.nY);
		g_pSO3UI->m_pUI->PushArg("arg3", param.nType);
		g_pSO3UI->m_pUI->PushArg("arg4", param.szComment);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_MID_MAP_MARK", 0);
	}
}

void KGameWorldUIHandler::OnPlayerLevelUp(KUIEventPlayerLevelUpdate const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PLAYER_LEVEL_UP", param.dwPlayerID);
	}
}

void KGameWorldUIHandler::OnPlayMiniGame(KUIEventPlayMiniGame const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwGameID);
		g_pSO3UI->m_pUI->FireEvent("PLAY_MINI_GAME", param.dwGameID);
	}
}

void KGameWorldUIHandler::OnUpdateRadar(KUIEventUpdateRadar const& param)
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwRadarType);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwRadarParam);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_RADAR", 0);
	}
}

void KGameWorldUIHandler::OnDisconnect()
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->FireEvent("DISCONNECT", 0);
	}
}

void KGameWorldUIHandler::OnConnectGameServerFailed() 
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->FireEvent("CONNECT_GAME_SERVER_FAILED", 0);
	}
}

void KGameWorldUIHandler::OnPlayerKickAccount()
{
	if (g_pSO3UI->m_pUI)
	{
		g_pSO3UI->m_pUI->FireEvent("KICK_ACCOUNT", 0);
	}
}

void KGameWorldUIHandler::OnShowSwimmingProgress() 
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("SHOW_SWIMMING_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnHideSwimmingProgress() 
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("HIDE_SWIMMING_PROGRESS", 0);
	}
}

void KGameWorldUIHandler::OnUpdateReputation(KUIEventUpdateReputation const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwForceID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.bNewForce ? true : false);
		g_pSO3UI->m_pUI->FireEvent("UPDATE_REPUTATION", 0);
	}
}

void KGameWorldUIHandler::OnReputationLevelUp(KUIEventReputationLevelUp const& param)
{	
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.dwForceID);
		g_pSO3UI->m_pUI->PushArg("arg1", param.bLevelUp ? true : false);
		g_pSO3UI->m_pUI->FireEvent("REPUTATION_LEVEL_UP", 0);
	}
}

void KGameWorldUIHandler::OnPeekOtherPlayer(KUIEventPeekOtherPlayer const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->PushArg("arg0", param.nResult);
		g_pSO3UI->m_pUI->PushArg("arg1", param.dwPlayerID);
		g_pSO3UI->m_pUI->FireEvent("PEEK_OTHER_PLAYER", 0);
	}
}

void KGameWorldUIHandler::OnSkillRecipeListUpdate()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("SKILL_RECIPE_LIST_UPDATE", 0);
    }
}

void KGameWorldUIHandler::OnBanishPlayer(KUIEventBanishPlayer const& param)
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_BANISH_PLAYER));
        g_pSO3UI->m_pUI->PushArg("arg1", param.nBanishCode);
	    g_pSO3UI->m_pUI->PushArg("arg2", param.nLeftSeconds);
		g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
	}
}

void KGameWorldUIHandler::OnUpdateRoadTrackForce()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_ROAD_TRACK_FORCE", 0);
    }
}

void KGameWorldUIHandler::OnRoadTrackErrorID(KUIEventRoadTrackErrorID const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nErrorID);
        g_pSO3UI->m_pUI->FireEvent("ROAD_TRACK_ERROR_ID", 0);
    }
}

void KGameWorldUIHandler::OnUpdateRouteNodeOpenList()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_ROUTE_NODE_OPEN_LIST", 0);
    }
}

void KGameWorldUIHandler::OnOpenDoodadMoveStateError()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", MAKE_UI_OUTPUT_MESSAGE_EVENT_TYPE_STR(UI_OME_CHECK_OPNE_DOODAD));
        g_pSO3UI->m_pUI->FireEvent("SYS_MSG", 0);
    } 
}

void KGameWorldUIHandler::OnUpdateAccumulateValue()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UI_UPDATE_ACCUMULATE", 0);
    }
}

void KGameWorldUIHandler::OnStartAutoFly()
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnStartAutoFly() begin\n");
#endif
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UI_START_AUTOFLY", 0);
    }
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnStartAutoFly() end\n");
#endif
}

void KGameWorldUIHandler::OnEndAutoFly()
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnEndAutoFly() begin\n");
#endif
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UI_END_AUTOFLY", 0);
    }
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnEndAutoFly() end\n");
#endif
}

void KGameWorldUIHandler::OnAutoFlySwitchMap()
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnAutoFlySwitchMap() begin\n");
#endif
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UI_AUTOFLY_SWITCH_MAP", 0);
    }
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnAutoFlySwitchMap() end\n");
#endif
}

void KGameWorldUIHandler::OnTrainValueUpdate(KUIEventTrainValueUpdate const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nAddTrain);
        g_pSO3UI->m_pUI->FireEvent("UI_TRAIN_VALUE_UPDATE", 0);
    }
}

void KGameWorldUIHandler::OnSwitchMapResult(KUIEventSwitchMapResult const& param)
{
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSwitchMapResult(%d) begin\n", param.nErrorID);
#endif
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nErrorID);
	    g_pSO3UI->m_pUI->FireEvent("ON_SWITCH_MAP", 0);
    }
#ifdef SO3UI_TRACE_GAMEWORLD_UI_HANDLER
    KGLogPrintf(KGLOG_INFO, "SO3UI KGameWorldUIHandler::OnSwitchMapResult(%d) end\n", param.nErrorID);
#endif
}

void KGameWorldUIHandler::OnCastCommonSkill(KUIEventCastCommonSkill const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
		g_pSO3UI->m_pUI->PushArg("arg0", param.bEnterOrLeaveCast ? true : false);
        g_pSO3UI->m_pUI->FireEvent("ON_CAST_COMMON_SKILL", 0);
    }
}

void KGameWorldUIHandler::OnOpenVenationRetCode(KUIEventOpenVenationRetCode const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
		g_pSO3UI->m_pUI->PushArg("arg0", param.bOpenVenationRetCode ? true : false);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nVenationID);
        g_pSO3UI->m_pUI->FireEvent("ON_OPEN_VENATION_RETCODE", 0);
    }
}

void KGameWorldUIHandler::OnShareQuestResult(KUIEventShareQuestResult const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nResultCode);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwQuestID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwDestPlayerID);

        g_pSO3UI->m_pUI->FireEvent("SHARE_QUEST", 0);
    }
}

void KGameWorldUIHandler::OnUpdatePlayerSchoolID(KUIEventUpdatePlayerSchoolID const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwSchoolID);
        g_pSO3UI->m_pUI->FireEvent("UPDATE_PLAYER_SCHOOL_ID", 0);
    }
}

void  KGameWorldUIHandler::OnUpdateAssistDailyCount()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_ASSIST_DAILY_COUNT", 0);
    }
}

void  KGameWorldUIHandler::OnMapVisitFlagChanged()
{
	if (g_pSO3UI->m_pUI && !m_bLogin)
	{
		g_pSO3UI->m_pUI->FireEvent("ON_MAP_VISIT_FLAG_CHANGED", 0);
	}
}

void KGameWorldUIHandler::SetInLogin(BOOL bLogin)
{
	m_bLogin = bLogin;
}

void KGameWorldUIHandler::OnAddSkillRecipe(KUIAddSkillRecipe const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwRecipeID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwRecipeLevel);
        g_pSO3UI->m_pUI->FireEvent("ADD_SKILL_RECIPE", 0);
    }
}

void KGameWorldUIHandler::OnDailyQuestUpdate()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("DAILY_QUEST_UPDATE", 0);
    }
}
void KGameWorldUIHandler::OnJoinBattleFieldQueue(KUIEventJoinBattleFieldQueue const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwMapID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nErrorCode);
        g_pSO3UI->m_pUI->FireEvent("JOIN_BATTLE_FIELD_QUEUE", 0);
    }
}

void KGameWorldUIHandler::OnLeaveBattleFieldQueue(KUIEventLeaveBattleFieldQueue const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwMapID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nErrorCode);
        g_pSO3UI->m_pUI->FireEvent("LEAVE_BATTLE_FIELD_QUEUE", 0);
    }
}

void KGameWorldUIHandler::OnBattleFieldNotify(KUIEventBattleFieldNotify const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nType);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nAvgQueueTime);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nPassTime);
        g_pSO3UI->m_pUI->PushArg("arg3", param.dwMapID);
        g_pSO3UI->m_pUI->PushArg("arg4", param.nCopyIndex);
        g_pSO3UI->m_pUI->FireEvent("BATTLE_FIELD_NOTIFY", 0);
    }
}

void KGameWorldUIHandler::OnBattleFieldEnd()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("BATTLE_FIELD_END", 0);
    }
}

void KGameWorldUIHandler::OnSyncBattleFieldStatistics()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("BATTLE_FIELD_SYNC_STATISTICS", 0);
    }  
}

void KGameWorldUIHandler::OnUpdateBattleFieldList(KUIEventUpdateBattleFieldList const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwMapID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nCopyIndex);
        g_pSO3UI->m_pUI->FireEvent("UPDATE_BATTLE_FIELD_LIST", 0);
    }
}

void KGameWorldUIHandler::OnUpdateBattleFieldObjective()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("BATTLE_FIELD_UPDATE_OBJECTIVE", 0);
    }  
}

void KGameWorldUIHandler::OnUpdateCampInfo()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_CAMP_INFO", 0);
    }  
}

void KGameWorldUIHandler::OnUpdatePrestige(KUIEventUpdatePrestige const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nOldPrestige);
        g_pSO3UI->m_pUI->FireEvent("UPDATE_PRESTIGE", 0);
    }  
}

void KGameWorldUIHandler::OnMaxPrestigeNotify()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("MAX_PRESTIGE_NOTIFY", 0);
    }  
}

void KGameWorldUIHandler::OnUpdateHeroFlag()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_HERO_FLAG", 0);
    }  
}

void KGameWorldUIHandler::OnUpdateTongInfo()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_TONG_INFO", 0);
    }  
}

void KGameWorldUIHandler::OnUpdateTongRoster()	
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_TONG_ROSTER", 0);
	}
}

void KGameWorldUIHandler::OnInvitePlayerJoinTongRequest(KUIEventInviteJoinTongRequest const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwInviterID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTongID);
        g_pSO3UI->m_pUI->PushArg("arg2", param.szInviterName);
        g_pSO3UI->m_pUI->PushArg("arg3", param.szTongName);
        g_pSO3UI->m_pUI->FireEvent("INVITE_JOIN_TONG_REQUEST", 0);
	}
}

void KGameWorldUIHandler::OnPlayerChangeTongNotify(KUIEventChangeTongNotify const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szTongName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.eReason);
        g_pSO3UI->m_pUI->FireEvent("CHANGE_TONG_NOTIFY", 0);
	}
}

void KGameWorldUIHandler::OnTongStateChange(KUIEventTongStateChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nTongState);
        g_pSO3UI->m_pUI->FireEvent("TONG_STATE_CHANGE", 0);   
    }
}

void KGameWorldUIHandler::OnTongGroupRightChange(KUIEventTongGroupRightChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szGroupName);
        g_pSO3UI->m_pUI->FireEvent("TONG_GROUP_RIGHT_CHANGE", 0);   
    }
}

void KGameWorldUIHandler::OnTongGroupNameChange(KUIEventTongGroupNameChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szOldGroupName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szNewGroupName);
        g_pSO3UI->m_pUI->FireEvent("TONG_GROUP_NAME_CHANGE", 0);   
    }
}

void KGameWorldUIHandler::OnTongGroupWageChange(KUIEventTongGroupWageChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szGroupName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nWage);
        g_pSO3UI->m_pUI->FireEvent("TONG_GROUP_WAGE_CHANGE", 0);   
    }
}

void KGameWorldUIHandler::OnTongMemberJoinOrQuit(KUIEventTongMemberJoinOrQuit const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {    
        switch (param.nType)
        {
        case tbmMemberJoin:
            g_pSO3UI->m_pUI->PushArg("arg0", param.szMemberName);
            g_pSO3UI->m_pUI->FireEvent("TONG_MEMBER_JOIN", 0);
            break;
        case tbmMemberQuit:
            g_pSO3UI->m_pUI->PushArg("arg0", param.szMemberName);
            g_pSO3UI->m_pUI->FireEvent("TONG_MEMBER_QUIT", 0);
            break;
        case tbmMemberFired:
            g_pSO3UI->m_pUI->PushArg("arg0", param.szMemberName);
            g_pSO3UI->m_pUI->FireEvent("TONG_MEMBER_FIRED", 0);
            break;
        default:
            KGLogPrintf(KGLOG_DEBUG, "unrecognized type %d\n", param.nType);
        }
    }
}

void KGameWorldUIHandler::OnTongMemberChangeGroup(KUIEventTongMemberChangeGroup const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szMemberName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szOldGroupName);
        g_pSO3UI->m_pUI->PushArg("arg2", param.szNewGroupName);
        g_pSO3UI->m_pUI->FireEvent("TONG_MEMBER_CHANGE_GROUP", 0);   
    }
}

void KGameWorldUIHandler::OnTongMasterChange(KUIEventTongMasterChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szOldMasterName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szNewMasterName);
        g_pSO3UI->m_pUI->FireEvent("TONG_MASTER_CHANGE", 0);   
    }
}

void KGameWorldUIHandler::OnTongCampChange(KCAMP eCamp)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", eCamp);
        g_pSO3UI->m_pUI->FireEvent("TONG_CAMP_CHANGE", 0);   
    }
}

void KGameWorldUIHandler::OnShowTongOnlineMessage(KUIEventShowTongOnlineMessage const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.pszOnlineMessage);
        g_pSO3UI->m_pUI->FireEvent("SHOW_TONG_ONLINE_MESSAGE", 0);   
    }
}

void KGameWorldUIHandler::OnOpenTongRepertory(KUIEventOpenTongRepertory const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szTongName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nEnabledPageNum);

        g_pSO3UI->m_pUI->FireEvent("OPEN_TONG_REPERTORY", 0);   
    }
}

void KGameWorldUIHandler::OnTongMemberOnlineStateChange(KUIEventTongMemberOnlineStateChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szMemberName);
        
        if (param.bIsOnline)
            g_pSO3UI->m_pUI->FireEvent("TONG_MEMBER_LOGIN", 0);
        else
            g_pSO3UI->m_pUI->FireEvent("TONG_MEMBER_LEAVE", 0);
    }
}

void KGameWorldUIHandler::OnTongGroupEnabled(KUIEventTongGroupEnabled const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szGroupName);
        g_pSO3UI->m_pUI->FireEvent("TONG_GROUP_ENABLED", 0);
    }
}

void KGameWorldUIHandler::OnTongMaxMemberCountChange(KUIEventTongMaxMemberCountChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nMaxMemberCount);
        g_pSO3UI->m_pUI->FireEvent("TONG_MAX_MEMBER_COUNT_CHANGE", 0);
    }
}

void KGameWorldUIHandler::OnUpdateTongClientDataFinish(KTONG_UPDATE_FLAG eFlag)
{
    KGLOG_PROCESS_ERROR(eFlag == tufInfo || eFlag == tufRoster);

    switch (eFlag)
    {
    case tufInfo:
        g_pSO3UI->m_pUI->FireEvent("UPDATE_TONG_INFO_FINISH", 0);
        break;

    case tufRoster:
        g_pSO3UI->m_pUI->FireEvent("UPDATE_TONG_ROSTER_FINISH", 0);
        break;
    }
Exit0:
    return;
}

void KGameWorldUIHandler::OnUpdateTongRepertoryPage(KUIEventUpdateTongRepertoryPage const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nPageIndex);
        g_pSO3UI->m_pUI->FireEvent("UPDATE_TONG_REPERTORY_PAGE", 0);
    }
}

void KGameWorldUIHandler::OnTongEventNotify(TONG_EVENT_NOTIFY_CODE eEventCode)
{    
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", eEventCode);
        g_pSO3UI->m_pUI->FireEvent("TONG_EVENT_NOTIFY", 0);
    }
}

void KGameWorldUIHandler::OnAuctionLookupRespond(KUIEventAuctionLookupRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nRespondCode);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nApplyType);
        g_pSO3UI->m_pUI->FireEvent("AUCTION_LOOKUP_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnAuctionSellRespond(KUIEventAuctionCommonRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nRespondCode);
        g_pSO3UI->m_pUI->FireEvent("AUCTION_SELL_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnAuctionBidRespond(KUIEventAuctionCommonRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nRespondCode);
        g_pSO3UI->m_pUI->FireEvent("AUCTION_BID_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnAuctionCancelRespond(KUIEventAuctionCommonRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nRespondCode);
        g_pSO3UI->m_pUI->FireEvent("AUCTION_CANCEL_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnAuctionMessageNotify(KUIEventAuctionMessageNotify const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nNotifyCode);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szSaleName);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nPrice);
        g_pSO3UI->m_pUI->FireEvent("AUCTION_MESSAGE_NOTIFY", 0);
    }
}

void KGameWorldUIHandler::OnCampInfoNotify(KUIEventCampInfo const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nResult);
        g_pSO3UI->m_pUI->FireEvent("CAMP_RESULT", 0);
    }
}

void KGameWorldUIHandler::OnChargeLimitNotify(KUIEventChargeLimitNotify const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nCode);
        g_pSO3UI->m_pUI->FireEvent("CHARGE_LIMIT_NOTIFT", 0);
    }
}

void KGameWorldUIHandler::OnRemoteLuaCall(const char cszCommand[], BYTE* pbyData, size_t uDataLen)
{
    BYTE* pbyOffset = pbyData;
	int nIndex = 0;
	BYTE* pbyNext = pbyData;
	BYTE* pbyTail = pbyData + uDataLen;
	Lua_State * L = g_pSO3UI->m_pUI->GetLuaState();
	int nTopIndex = Lua_GetTopIndex(L);

    KG_PROCESS_ERROR(g_pSO3UI->m_pUI && !m_bLogin);

	Lua_GetGlobal(L, "OnRemoteCall");
	if (lua_isfunction(L, -1))
	{
		int nParamCount = 1;
		lua_pushstring(L, cszCommand);

		while (pbyNext < pbyTail)
		{
			pbyNext = LuaUnpack(L, pbyNext, (size_t)(pbyTail - pbyNext));
			KGLOG_PROCESS_ERROR(pbyNext);
			++nParamCount;
		}

		if (lua_pcall(L, nParamCount, 0, 0))
		{
			KGLogPrintf(KGLOG_ERR, "KGUI Call function failed [%s]\n", lua_tostring(L, -1));
		}
	}

Exit0:
	Lua_SetTopIndex(L, nTopIndex);
    return;
}

void KGameWorldUIHandler::OnChangeCamp(KUIEventChangeCamp const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->FireEvent("CHANGE_CAMP", 0);
    }
}

void KGameWorldUIHandler::OnSyncAchievementData(KUIEventSyncAchievementData const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->FireEvent("SYNC_ACHIEVEMENT_DATA", 0);
    }
}

void KGameWorldUIHandler::OnNewAchievement(KUIEventNewAchievement const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nAchievementID);
        g_pSO3UI->m_pUI->FireEvent("NEW_ACHIEVEMENT", 0);
    }
}

void KGameWorldUIHandler::OnAchievementAnnounce(KUIEventAchievementAnnounce const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szRoleName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.byType);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nAchievementID);
        g_pSO3UI->m_pUI->FireEvent("ACHIEVEMENT_ANNOUNCE", 0);
    }
}

void KGameWorldUIHandler::OnUpdateAchievementPoint(int nDelta)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
		g_pSO3UI->m_pUI->PushArg("arg0", nDelta);
        g_pSO3UI->m_pUI->FireEvent("UPDATE_ACHIEVEMENT_POINT", 0);
    }
}

void KGameWorldUIHandler::OnUpdateAchievementCount()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("UPDATE_ACHIEVEMENT_COUNT", 0);
    }
}

void KGameWorldUIHandler::OnSyncDesignationData(KUIEventSyncDesignationData const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->FireEvent("SYNC_DESIGNATION_DATA", 0);
    }
}

void KGameWorldUIHandler::OnSetGenerationNotify(KUIEventSetGeneration const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nGenerationIndex);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nNameInForceIndex);
        g_pSO3UI->m_pUI->FireEvent("SET_GENERATION_NOTIFY", 0);
    }
}

void KGameWorldUIHandler::OnAcquireDesignation(KUIEventAcquireDesignation const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nPrefix);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nPostfix);
        g_pSO3UI->m_pUI->FireEvent("ACQUIRE_DESIGNATION", 0);
    }
}

void KGameWorldUIHandler::OnDesignationAnnounce(KUIEventDesignationAnnounce const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szRoleName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nPrefix);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nPostfix);
        g_pSO3UI->m_pUI->PushArg("arg3", (int)param.byType);
        g_pSO3UI->m_pUI->FireEvent("DESIGNATION_ANNOUNCE", 0);
    }
}

void KGameWorldUIHandler::OnRemoveDesignation(KUIEventRemoveDesignation const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nPrefix);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nPostfix);
        g_pSO3UI->m_pUI->FireEvent("REMOVE_DESIGNATION", 0);
    }
}

void KGameWorldUIHandler::OnSetCurrentDesignationRespond(KUIEventSetCurrentDesignationRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwPlayerID);
        g_pSO3UI->m_pUI->PushArg("arg1", param.nPrefix);
        g_pSO3UI->m_pUI->PushArg("arg2", param.nPostfix);
        g_pSO3UI->m_pUI->PushArg("arg3", param.bDisplayFlag);
        g_pSO3UI->m_pUI->FireEvent("SET_CURRENT_DESIGNATION", 0);
    }
}

void KGameWorldUIHandler::OnStopFollow(KUIEventStopAutoMove const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.pszReason);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetType);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwTargetID);
        g_pSO3UI->m_pUI->FireEvent("STOP_FOLLOW", 0);
    }
}

void KGameWorldUIHandler::OnStopAutoMoveToTarget(KUIEventStopAutoMove const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.pszReason);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwTargetType);
        g_pSO3UI->m_pUI->PushArg("arg2", param.dwTargetID);
        g_pSO3UI->m_pUI->FireEvent("STOP_AUTO_MOVE_TO_TARGET", 0);
    }
}

void KGameWorldUIHandler::OnStopAutoMoveToPoint(KUIEventReachPoint const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.fX);
        g_pSO3UI->m_pUI->PushArg("arg1", param.fY);
        g_pSO3UI->m_pUI->PushArg("arg2", param.fZ);
        g_pSO3UI->m_pUI->FireEvent("STOP_AUTO_MOVE_TO_POINT", 0);
    }
}

void KGameWorldUIHandler::OnOptimizationHint(KUIEventOptimizationHint const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.dwLevel);
        g_pSO3UI->m_pUI->PushArg("arg1", param.dwFPS);
        g_pSO3UI->m_pUI->FireEvent("OPTIMIZATION_HINT", 0);
    }
}

void KGameWorldUIHandler::OnAccountEndOfDayTime()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("ACCOUNT_END_TIME", 0);
    }
}

void KGameWorldUIHandler::OnGameCardSellRespond(KUIEventGameCardSellRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.byCode);
        g_pSO3UI->m_pUI->FireEvent("GAME_CARD_SELL_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnGameCardBuyRespond(KUIEventGameCardBuyRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.byCode);
        g_pSO3UI->m_pUI->FireEvent("GAME_CARD_BUY_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnGameCardLookupRespond()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("GAME_CARD_LOOKUP_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnGameCardCancelRespond(KUIEventGameCardCancelRespond const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.byCode);
        g_pSO3UI->m_pUI->FireEvent("GAME_CARD_CANCEL_RESPOND", 0);
    }
}

void KGameWorldUIHandler::OnSyncCoin()
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->FireEvent("SYNC_COIN", 0);
    }
}

void KGameWorldUIHandler::OnSeekMentorYell(KUIEventSeekMentorYell const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szPlayerName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szTongName);
        g_pSO3UI->m_pUI->PushArg("arg2", (int)param.byForceID);
        g_pSO3UI->m_pUI->PushArg("arg3", (int)param.byRoleLevel);
        g_pSO3UI->m_pUI->PushArg("arg4", (int)param.byRoleType);

        g_pSO3UI->m_pUI->FireEvent("SEEK_MENTOR_YELL", 0);
    }
}

void KGameWorldUIHandler::OnSeekApprenticeYell(KUIEventSeekApprenticeYell const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.szPlayerName);
        g_pSO3UI->m_pUI->PushArg("arg1", param.szTongName);
        g_pSO3UI->m_pUI->PushArg("arg2", (int)param.byForceID);
        g_pSO3UI->m_pUI->PushArg("arg3", (int)param.byRoleLevel);
        g_pSO3UI->m_pUI->PushArg("arg4", (int)param.byRoleType);

        g_pSO3UI->m_pUI->FireEvent("SEEK_APPRENTICE_YELL", 0);
    }
}

void KGameWorldUIHandler::OnMentorValueChange(KUIEventMentorValueChange const& param)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", param.nValueChange);

        g_pSO3UI->m_pUI->FireEvent("MENTOR_VALUE_CHANGE", 0);
    }
}

void KGameWorldUIHandler::OnChangeCampFlag(DWORD dwPlayerID)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        g_pSO3UI->m_pUI->PushArg("arg0", dwPlayerID);
        g_pSO3UI->m_pUI->FireEvent("CHANGE_CAMP_FLAG", 0);
    }
}

void KGameWorldUIHandler::OnCharacterThreatRanklist(DWORD dwThreatListOwnerID, int nRankCount, KUIThreatRankList* pRankList)
{
    if (g_pSO3UI->m_pUI && !m_bLogin)
    {
        int         nThreatListOwnerID = (int)dwThreatListOwnerID;
        lua_State*  L = g_pSO3UI->m_pUI->GetLuaState();
        KGLOG_PROCESS_ERROR(L);

        lua_pushstring(L, "arg0");
        lua_pushnumber(L, nThreatListOwnerID);
        lua_settable(L, LUA_GLOBALSINDEX);

        lua_pushstring(L, "arg1");
        lua_newtable(L);
        for (int i = 0; i < nRankCount; i++)
        {
            int nCharacterID = (int)pRankList[i].dwCharacterID;
            lua_pushnumber(L, nCharacterID);
            lua_pushnumber(L, pRankList[i].nRank);
            lua_settable(L, -3);
        }
        lua_settable(L, LUA_GLOBALSINDEX);

        g_pSO3UI->m_pUI->FireEvent("CHARACTER_THREAT_RANKLIST", 0);
    }

Exit0:
    return;
}
