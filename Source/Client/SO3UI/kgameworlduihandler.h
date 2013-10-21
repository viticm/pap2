#ifndef UI_CASE_K_GAME_WORLD_UI_HANDLER_H
#define UI_CASE_K_GAME_WORLD_UI_HANDLER_H

#include "SO3UI/SO3GameWorldUIHandler.h"

class KGameWorldUIHandler : public IKSO3GameWorldUIHandler
{
public:
	KGameWorldUIHandler();
	virtual~KGameWorldUIHandler();
	virtual void Release();

	virtual BOOL GetPassword(LPCSTR szTreePath, char szDestMD5String[64]);

	virtual void OnCharacterSay(KUIEventCharacterSay const& param);
	virtual void OnItemListUpdate(KUIEventItemListUpdate const& param);
	virtual void OnItemExchanged(KUIEvnetItemExchanged const& param);
	virtual void OnDestroyItem(KUIEventDestroyItem const& Param);

	virtual void OnMoneyUpdate(KUIEventMoneyUpdate const& param);
    virtual void OnContributionUpdate(KUIEventContributionUpdate const& param);
    virtual void OnSkillUpdate(KUIEventSkillUpdate const& param);
	virtual void OnSkillExpUpdate(KUIEventSkillUpdate const& param);
    virtual void OnCastingBarStart();
    virtual void OnCastingBarEnd(KUIEventCastingBarEnd const& param);

	virtual void OnCraftUpdate(KUIEventCraftUpdate const& param);
	virtual void OnCraftRemove(KUIEventCraftRemove const& param);
	virtual void OnRecipeUpdate(KUIEventRecipeUpdate const& param);

	virtual void OnLearnProfession(KUIEventLearnProfession const& param);
	virtual void OnLearnBranch(KUIEventLearnBranch const& param);
	virtual void OnForgetProfession(KUIEventForgetProfession const& param);
	virtual void OnAddProfessionProficiency(KUIEventAddProfessionProficiency const& param);
	virtual void OnProfessionLevelUp(KUIEventProfessionLevelUp const& param);
    virtual void OnSetProfessAdjustLevel(KUIEventSetProfessionAdjustLevel const& param);
	virtual void OnSetProfessionMaxLevel(KUIEventSetProfessionMaxLevel const& param);
	virtual void OnLearnRecipe(KUIEventLearnRecipe const& param); 
    virtual void OnReputationRespond(KUIEventReputationRespond const& param);

	virtual void OnOpenDoodad(KUIEventOpenDoodad const& param);
	virtual void OnCloseDoodad(KUIEventCloseDoodad const& param);
	virtual void OnSyncLootList(KUIEventSyncLootList const& param);
	virtual void OnLootItem(KUIEventLootItem const& param);
    virtual void OnDistributeItem(KUIEventDistributeItem const& param);
	virtual void OnBeginRollItem(KUIEventBeginRollItem const& param);
	virtual void OnRollItem(KUIEventRollItem const& param);
	virtual void OnCancelRollItem(KUIEventCancelRollItem const& param);

    virtual void OnSyncTeamBaseInfo(KUIEventSyncTeamBaseInfo const& param);
    virtual void OnSyncTeamMemberData(KUIEventTeamSyncMember const& param);
    virtual void OnTeamAddMember(KUIEventTeamAddMember const& param);
    virtual void OnTeamDelMember(KUIEventTeamDelMember const& param);
    virtual void OnTeamInviteRequest(KUIEventTeamInviteRequest const& param);
    virtual void OnTeamApplyRequest(KUIEventTeamApplyRequest const& param);
    virtual void OnTeamMessageNotify(KUIEventTeamMessageNotify const& param);
    virtual void OnUpdateTeamMemberInfo(KUIEventUpdateTeamMemberInfo const& param);
    virtual void OnTeamLootModeChanged(KUIEventTeamLootModeChanged const& param);
    virtual void OnTeamSetDistributeManNotify(KUIEventSetDistributeMan const& param);
    virtual void OnTeamSetFormationLeaderNotify(KUIEventSetFormationLeader const& param);

	virtual void OnTeamRollQualityChanged(KUIEventTeamRollQualityChanged const& param);
    virtual void OnSetTeamMemberOnlineFlag(KUIEventSetTeamMemberOnlineFlag const& param);
    virtual void OnTeamDisband(KUIEventTeamDisband const& param);
    virtual void OnTeamLeaderChanged(KUIEventTeamLeaderChanged const& param);
    virtual void OnTeamAuthorityChanged(KUIEventTeamAuthorityChanged const& param);
    virtual void OnTeamMemberSignpostNotify(KUIEventTeamMemberSignpostNotify const& param);
	virtual void OnUpdateTeamMemberLMR(KUIEventUpdateTeamMemberLMR const& param);
	virtual void OnUpdateTeamMemberPosition(KUIEventUpdateTeamMemberInfo const& param);
	virtual void OnTeamSetMark();
    virtual void OnTeamCampChange();
    virtual void OnTeamLevelUpRaid();
    virtual void OnTeamChangeMemberGroup(KUIEventTeamChangeMemberGroup const& param);

	virtual void OnQuestAccepted(KUIEventQuestAccepted const& param);
    virtual void OnQuestFailed(KUIEventQuestFailed const& param);
	virtual void OnQuestCanceled(KUIEventQuestCanceled const& param);
	virtual void OnFinishQuest(KUIEventFinishQuest const& param);
	virtual void OnQuestShared(KUIEventQuestShared const& param);
	virtual void OnQuestDataUpdate(KUIEventQuestDataUpdate const& param);
	virtual void OnQuestListUpdate(void);
	virtual void OnStartEscortQuest(KUIEventStartEscortQuest const& param);
	virtual void OnQuestTimeUpdate(KUIEventQuestTimeUpdate const& param);
    virtual void OnQuestMarkUpdate(KUIEventQuestMarkUpdate const& param);

	virtual void OnSysMsg(KUIEventSysErrorMsg const& param);
	virtual void OnOpenWindow(KUIEventOpenWindow const& param);

	virtual void OnSceneBeginLoad(KUIEventSceneBeginLoad const& param);
	virtual void OnSceneEndLoad(KUIEventSceneEndLoad const& param);

	virtual void OnPlayerExperienceUpdate(KUIEventPlayerExperienceUpdate const& param);
    virtual void OnPlayerLevelUpdate(KUIEventPlayerLevelUpdate const& param);

    virtual void OnSyncRoleDataBegin();
    virtual void OnSyncRoleDataEnd();

	virtual void OnCurrentPlayerForceChanged(KUIEventCurrentPlayerForceChanged const& param);
    virtual void OnPlayerStateUpdate(KUIEventPlayerStateUpdate const& param); 
    virtual void OnPlayerWeakUpdate(KUIEventPlayerWeakUpdate const& param); 
	virtual void OnPlayerDisplayDataUpdate(KUIEventPlayerDisplayDataUpdate const& param);
	virtual void OnPlayerEnterScene(KUIEventPlayerEnterScene const& param);
	virtual void OnPlayerLeaveScene(KUIEventPlayerLeaveScene const& param);
    virtual void OnPlayerTalk(KUIEventPlayerTalk const& param);
    virtual void OnNpcTalkUseSentenceID(KUIEventNpcTalkUseSentenceID const& param);

    virtual void OnFellowshipUpdate();
    virtual void OnFoeUpdate();
    virtual void OnBlackListUpdate();
    virtual void OnFellowshipRespond(KUIEventFellowshipRespond const& param);
    virtual void OnFellowshipChange(KUIEventFellowshipChange const& param);
    virtual void OnFellowshipLogin(KUIEventFellowshipNotify const& param);
    virtual void OnAddFellowshipAttraction(KUIEventAddFellowshipAttraction const& param);
    virtual void OnFellowshipAttractionFallOff();
    virtual void OnBeAddFellowship(KUIEventBeAddFellowship const& param);

    virtual void OnNpcStateUpdate(KUIEventNpcStateUpdate const& param);
	virtual void OnNpcDropTargetUpdate(KUIEventNpcDropTargetUpdate const& param);
    virtual void OnNpcWeakUpdate(KUIEventNpcWeakUpdate const& param);
	virtual void OnNpcDisplayDataUpdate(KUIEventNpcDisplayDataUpdate const& param);
	virtual void OnNpcEnterScene(KUIEventNpcEnterScene const& param);
	virtual void OnNpcLeaveScene(KUIEventNpcLeaveScene const& param);
    virtual void OnNpcTalk(KUIEventNpcTalk const& param);

	virtual void OnDoodadEnterScene(KUIEventDoodadEnterScene const& param);
	virtual void OnDoodadLeaveScene(KUIEventDoodadLeaveScene const& param);

	virtual void OnShopUpdateItem(KUIEventShopUpdateItem const& param);
	virtual void OnShopOpenNotify(KUIEventShopOpenNotify const& param);

    virtual void OnTradingInvite(KUIEventTradingInvite const& param);
    virtual void OnTradingOpenNotify(KUIEventTradingOpenNotify const& param);
    virtual void OnTradingUpdateConfirm(KUIEventTradingUpdateConfirm const& param);
    virtual void OnTradingUpdateItem(KUIEventTradingUpdateItem const& param);
    virtual void OnTradingUpdateMoney(KUIEventTradingUpdateMoney const& param);
    virtual void OnTradingClose(KUIEventTradingClose const& param);
    
    virtual void OnSkillCastLog(KUIEventSkillCastLog const& param);
    virtual void OnSkillCastRespondLog(KUIEventSkillCastRespondLog const& param);
    virtual void OnSkillEffectLog(KUIEventSkillEffectLog const& param);
    virtual void OnSkillBlockLog(KUIEventSkillBlockLog const& param);
    virtual void OnSkillShieldLog(KUIEventSkillShieldLog const& param);
    virtual void OnSkillMissLog(KUIEventSkillMissLog const& param);
    virtual void OnSkillHitLog(KUIEventSkillHitLog const& param);
    virtual void OnSkillDodgeLog(KUIEventSkillDodgeLog const& param);  

    virtual void OnBuffLog(KUIEventBuffLog const& param);
    virtual void OnBuffImmunityLog(KUIEventBuffImmunityLog const& param);
    virtual void OnExpLog(KUIEventExpLog const& param);

    virtual void OnCommonHealthLog(KUIEventCommonHealthLog& param);
    virtual void OnCommonHealthText(KUIEventCommonHealthText& param);

    virtual void OnCharacterDeath(KUIEventCharacterDeath const& param);
    virtual void OnClientPlayerDeath();
    virtual void OnSyncPlayerRevive(KUISyncPlayerRevive const& param);

    virtual void OnFightHint(KUIEventFightHint const& param);
	virtual void OnSkillEffectText(KUIEventSkillEffectText const& param);
    virtual void OnSkillMissText(KUIEventSkillMissText const& param);
    virtual void OnSkillBlockText(KUIEventSkillBlockText const& param);
    virtual void OnSkillBuffText(KUIEventSkillBuffText const& param);
    virtual void OnBuffImmunityText(KUIEventBuffImmunityText const& param);
    virtual void OnSkillDodgeText(KUIEventSkillDodgeText const& param);
    virtual void OnSkillCastRespond(KUIEventSkillCastRespond  const& param);

	virtual void OnSkillMountKungFu(KUIEventSkillUpdate const& param);
	virtual void OnSkillUnmountKungfu(KUIEventSkillUpdate const& param);

	virtual void OnBuffUpdata(KUIBuffUpdata  const& param);

	virtual void OnOpenRecipeBook(KUIEventOpenRecipeBook const& param);
	virtual void OnOpenBook(KUIEventOpenBook const& param);
	virtual void OnUpdateBookState(KUIEventUpdateBookState const& param);
    virtual void OnPeekPlayerBookState(KUIEventPeekPlayerBookState const& param);
    virtual void OnPeekPlayerQuest(KUIEventPeekPlayerQuest const& param);

    virtual void OnSkillPrepareProgress(KUIEventSkillProgress const& param);
    virtual void OnSkillChannelProgress(KUIEventSkillProgress const& param);
    virtual void OnSkillAccumulateProgress(KUIEventSkillProgress const& param);
    virtual void OnSkillCast(KUIEventSkillCastLog const& param);

    virtual void OnSkillAccumulateSuccess(KUIEventSkillProgress const& param);

	virtual void OnRecipePrepareProgress(KUIEventRecipeProgress const& param);

    virtual void OnOTActionProgressUpdate(int nFrame);
    virtual void OnOTActionProgressBreak(DWORD dwCharacterID);       
    virtual void OnPickPrepareProgress(KUIEventPickProgress const& param);
	virtual void OnCustomOTActionProgress(KUIEventCustomOTActionProgress const& param);

	virtual void OnItemUseRespond(KUIEventItemUseRespond const& param);
    virtual void OnAddItemRespond(KUIEventAddItemRespond const& param);
    virtual void OnUseItemRespond(KUIEventUseItemRespond const& param);

    virtual void OnTradingRespond(KUIEventTradingRespond const& param);

	virtual void OnLootRespond(KUIEventLootRespond const& param);

	virtual void OnCraftCastRespond(KUIEventCraftCastRespond const& param);

	virtual void OnQuestRespond(KUIEventQuestRespond const& param);

	virtual void OnLoginNotify(KUIEventLoginNotify const& param);
	virtual void OnLoginQueueState(KUIEventLoginQueueState const& param);

    virtual void OnShopRespond(KUIEventShopRespond const& param);
	virtual void OnPKRespond(KUIEventPKRespond const& param);

    virtual void OnSendMailResult(KUIEventSendMailResult const& param);
    virtual void OnMailRespond(KUIEventMailRespond const& param);
	virtual void OnMailListUpdate(KUIEventMailListUpdate const& param);
    virtual void OnGetMailContent(KUIEventGetMailContent const& param);
    virtual void OnGetGmMessageMail(KUIEventGetMailContent const& param);
    virtual void OnGetPlayerMessageMail(KUIEventGetMailContent const& param);
    virtual void OnNewMailNotify(KUIEventNewMailNotify const& param);

    virtual void OnUpdateSelectTarget();

    virtual void OnResponseMsg(KUIEventResponseMsg const& param);

	virtual void OnUpdateMiniMapNpcMark(KUIEventUpdateMiniMapNpcMark const& param);
	virtual void OnUpdateMiniMapDoodadMark(KUIEventUpdateMiniMapDoodadMark const& param);

	virtual void OnUpdateRelation(KUIEventUpdateRelation const& param);
	virtual void OnUpdateAllRelation();
	virtual void OnUpdateKillPoint(KUIEventKillPoint const& Param);

	virtual void OnApplyDuel(KUIEventApplyDuel const& param);
    virtual void OnLeaveDuel();
    virtual void OnReturnDuel();
	virtual void OnAcceptDuel(KUIEventAcceptDuel const& param);
	virtual void OnRefuseDuel(KUIEventRefuseDuel const& param);
	virtual void OnStartDuel(KUIEventStartDuel const& param);
	virtual void OnCancelDuel(KUIEventCancelDuel const& param);
    virtual void OnFinishDuel();
	virtual void OnWinDuel(KUIEventWinDuel const& param);

	virtual void OnApplySlay(KUIEventSlayInfo const& param);
    virtual void OnStartSlay(KUIEventSlayInfo const& param);
	virtual void OnCloseSlay(KUIEventSlayInfo const& param);
	virtual void OnSlayClosed(KUIEventSlayInfo const& param);
	
	virtual void OnOpenBank(KUIEventOpenBank const& param);
	virtual void OnUpdateBankSlot();

	virtual void OnUpdateMapMark(KUIEventUpdateMapMark const& param);

	virtual void OnUpdateRegionInfo(KUIEventUpdateRegionInfo const& param);

	virtual void OnUpdateMidMapMark(KUIEventUpdateMidMapMark const& param);

    virtual void OnPlayerLevelUp(KUIEventPlayerLevelUpdate const& param);

	virtual void OnPlayMiniGame(KUIEventPlayMiniGame const& param);

	virtual void OnUpdateRadar(KUIEventUpdateRadar const& param);

	virtual void OnDisconnect();
    virtual void OnPlayerKickAccount();
	virtual void OnConnectGameServerFailed();

	virtual void OnShowSwimmingProgress();
	virtual void OnHideSwimmingProgress();

	virtual void OnUpdateReputation(KUIEventUpdateReputation const& param);
	virtual void OnReputationLevelUp(KUIEventReputationLevelUp const& param);

    virtual void OnPeekOtherPlayer(KUIEventPeekOtherPlayer const& param);

    virtual void OnSkillRecipeListUpdate();

    virtual void OnBanishPlayer(KUIEventBanishPlayer const& param);

    virtual void OnUpdateRoadTrackForce();

    virtual void OnRoadTrackErrorID(KUIEventRoadTrackErrorID const& param);

    virtual void OnUpdateRouteNodeOpenList();
    
    virtual void OnOpenDoodadMoveStateError();

    virtual void OnUpdateAccumulateValue();

    virtual void OnStartAutoFly();
    virtual void OnEndAutoFly();
    virtual void OnAutoFlySwitchMap();
    
    virtual void OnTrainValueUpdate(KUIEventTrainValueUpdate const& param);

    virtual void OnSwitchMapResult(KUIEventSwitchMapResult const& param);

    virtual void OnCastCommonSkill(KUIEventCastCommonSkill const& param);

    virtual void OnOpenVenationRetCode(KUIEventOpenVenationRetCode const& param);
    
    virtual void OnShareQuestResult(KUIEventShareQuestResult const& param);

    virtual void OnUpdatePlayerSchoolID(KUIEventUpdatePlayerSchoolID const& param);

    virtual void OnUpdateAssistDailyCount();

	virtual void OnMapVisitFlagChanged();

	virtual void SetInLogin(BOOL bLogin);

    virtual void OnAddSkillRecipe(KUIAddSkillRecipe const& param);

    virtual void OnDailyQuestUpdate();

    virtual void OnJoinBattleFieldQueue(KUIEventJoinBattleFieldQueue const& param);
    virtual void OnLeaveBattleFieldQueue(KUIEventLeaveBattleFieldQueue const& param);
    virtual void OnBattleFieldNotify(KUIEventBattleFieldNotify const& param);
    virtual void OnBattleFieldEnd();
    virtual void OnSyncBattleFieldStatistics();
    virtual void OnUpdateBattleFieldList(KUIEventUpdateBattleFieldList const& param);
    virtual void OnUpdateBattleFieldObjective();
    
    virtual void OnUpdateCampInfo();
    virtual void OnUpdatePrestige(KUIEventUpdatePrestige const& param);
    virtual void OnMaxPrestigeNotify();
    virtual void OnUpdateHeroFlag();

    virtual void OnUpdateTongInfo();
    virtual void OnUpdateTongRoster();
    virtual void OnInvitePlayerJoinTongRequest(KUIEventInviteJoinTongRequest const& param);
    virtual void OnPlayerChangeTongNotify(KUIEventChangeTongNotify const& param);
    virtual void OnTongStateChange(KUIEventTongStateChange const& param);
    virtual void OnTongGroupRightChange(KUIEventTongGroupRightChange const& param);
    virtual void OnTongGroupNameChange(KUIEventTongGroupNameChange const& param);
    virtual void OnTongGroupWageChange(KUIEventTongGroupWageChange const& param);
    virtual void OnTongMemberJoinOrQuit(KUIEventTongMemberJoinOrQuit const& param);
    virtual void OnTongMemberChangeGroup(KUIEventTongMemberChangeGroup const& param);
    virtual void OnTongMasterChange(KUIEventTongMasterChange const& param);
    virtual void OnTongCampChange(KCAMP eCamp);
    virtual void OnShowTongOnlineMessage(KUIEventShowTongOnlineMessage const& param);
    virtual void OnOpenTongRepertory(KUIEventOpenTongRepertory const& param);
    virtual void OnTongMemberOnlineStateChange(KUIEventTongMemberOnlineStateChange const& param);
    virtual void OnUpdateTongClientDataFinish(KTONG_UPDATE_FLAG eFlag);
    virtual void OnTongGroupEnabled(KUIEventTongGroupEnabled const& param);
    virtual void OnTongMaxMemberCountChange(KUIEventTongMaxMemberCountChange const& param);
    virtual void OnUpdateTongRepertoryPage(KUIEventUpdateTongRepertoryPage const& param);
    virtual void OnTongEventNotify(TONG_EVENT_NOTIFY_CODE eEventCode);

    virtual void OnAuctionLookupRespond(KUIEventAuctionLookupRespond const& param);
    virtual void OnAuctionSellRespond(KUIEventAuctionCommonRespond const& param);
    virtual void OnAuctionBidRespond(KUIEventAuctionCommonRespond const& param);
    virtual void OnAuctionCancelRespond(KUIEventAuctionCommonRespond const& param);
    virtual void OnAuctionMessageNotify(KUIEventAuctionMessageNotify const& param);

    virtual void OnCampInfoNotify(KUIEventCampInfo const& param);
    virtual void OnChargeLimitNotify(KUIEventChargeLimitNotify const& param);

    virtual void OnRemoteLuaCall(const char cszCommand[], BYTE* pbyData, size_t uDataLen);
    virtual void OnChangeCamp(KUIEventChangeCamp const& param);

    virtual void OnSyncAchievementData(KUIEventSyncAchievementData const& param);
    virtual void OnNewAchievement(KUIEventNewAchievement const& param);
    virtual void OnAchievementAnnounce(KUIEventAchievementAnnounce const& param);
    virtual void OnUpdateAchievementPoint(int nDelta);
    virtual void OnUpdateAchievementCount();

    virtual void OnSyncDesignationData(KUIEventSyncDesignationData const& param);
    virtual void OnSetGenerationNotify(KUIEventSetGeneration const& param);
    virtual void OnAcquireDesignation(KUIEventAcquireDesignation const& param);
    virtual void OnDesignationAnnounce(KUIEventDesignationAnnounce const& param);
    virtual void OnRemoveDesignation(KUIEventRemoveDesignation const& param);
    virtual void OnSetCurrentDesignationRespond(KUIEventSetCurrentDesignationRespond const& param);

    virtual void OnStopFollow(KUIEventStopAutoMove const& param);
    virtual void OnStopAutoMoveToTarget(KUIEventStopAutoMove const& param);
    virtual void OnStopAutoMoveToPoint(KUIEventReachPoint const& param);

    virtual void OnOptimizationHint(KUIEventOptimizationHint const& param);

    virtual void OnAccountEndOfDayTime();

    virtual void OnGameCardSellRespond(KUIEventGameCardSellRespond const& param);
    virtual void OnGameCardBuyRespond(KUIEventGameCardBuyRespond const& param);
    virtual void OnGameCardLookupRespond();
    virtual void OnGameCardCancelRespond(KUIEventGameCardCancelRespond const& param);

    virtual void OnSyncCoin();

    virtual void OnSeekMentorYell(KUIEventSeekMentorYell const& param);
    virtual void OnSeekApprenticeYell(KUIEventSeekApprenticeYell const& param);
    virtual void OnMentorValueChange(KUIEventMentorValueChange const& param);
    virtual void OnChangeCampFlag(DWORD dwPlayerID);

    virtual void OnCharacterThreatRanklist(DWORD dwThreatListOwnerID, int nRankCount, KUIThreatRankList* pRankList);

	int Init();
	void Exit();

private:
	BOOL m_bLogin;
};

extern KGameWorldUIHandler g_GameWorldUIHandler;

#endif // UI_CASE_K_GAME_WORLD_UI_HANDLER_H


