#include "stdafx.h"

#ifdef WIN32
#include <conio.h>
#endif

#include <stdio.h>
#include "KMath.h"

#if defined(_CLIENT)
#include "KPlayerClient.h"
#include "Engine/KG_Time.h"
#include "Common/CRC32.h"
#include "KSO3World.h"
#include "KShop.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "Global.h"
#include "SO3Result.h"
#include "KRecipe.h"
#include "KRegion.h"
#include "KScene.h"
#include "KCraft.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KLootList.h"
#include "Engine/KMemory.h"
#include "KMailDef.h"

KPlayerClient g_PlayerClient;

#define MAX_PACKAGE_COUNT				10240				//缓冲的最大协议数目
#define FORCE_SYNC_RANGE				((CELL_LENGTH) * 8)	//强制同步范围，单位：象素点
#define PLAYER_DISPLAY_DATA_MAX_CACHE	512
#define NPC_DISPLAY_DATA_MAX_CACHE		512
#define MAX_CLIENT_SEND_PACK_FRAME      32
#define MAX_CLIENT_SEND_PACK_QUEUE      512
#define MAX_DIALOGE_KEY_SIZE            64

KPlayerClient::KPlayerClient(void)
{
    m_piSocketStream    = NULL;
    m_bSendErrorFlag    = false;
    m_nFrameSendCount   = 0;
    m_nLastSendCountResetFrame = 0;

	m_nGSGameLoop = 0;

	memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
	memset(m_nProtocolSize, 0, sizeof(m_nProtocolSize));

	REGISTER_EXTERNAL_FUNC(s2c_message_notify, &KPlayerClient::OnMessageNotify, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_sync_player_base_info, &KPlayerClient::OnSyncPlayerBaseInfo, sizeof(S2C_SYNC_PLAYER_BASE_INFO));
	REGISTER_EXTERNAL_FUNC(s2c_sync_quest_data, &KPlayerClient::OnSyncQuestData, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_account_kickout, &KPlayerClient::OnAccountKickout, sizeof(S2C_ACCOUNT_KICKOUT));
	REGISTER_EXTERNAL_FUNC(s2c_switch_gs, &KPlayerClient::OnSwitchGS, sizeof(S2C_SWITCH_GS));
    REGISTER_EXTERNAL_FUNC(s2c_switch_map, &KPlayerClient::OnSwitchMap, sizeof(S2C_SWITCH_MAP));

    REGISTER_EXTERNAL_FUNC(s2c_sync_new_player, &KPlayerClient::OnSyncNewPlayer, sizeof(S2C_SYNC_NEW_PLAYER));
    REGISTER_EXTERNAL_FUNC(s2c_sync_new_npc, &KPlayerClient::OnSyncNewNpc, sizeof(S2C_SYNC_NEW_NPC));
    REGISTER_EXTERNAL_FUNC(s2c_sync_new_doodad, &KPlayerClient::OnSyncNewDoodad, sizeof(S2C_SYNC_NEW_DOODAD));

	REGISTER_EXTERNAL_FUNC(s2c_move_character, &KPlayerClient::OnMoveCharacter, sizeof(S2C_MOVE_CHARACTER));
	REGISTER_EXTERNAL_FUNC(s2c_remove_character, &KPlayerClient::OnRemoveCharacter, sizeof(S2C_REMOVE_CHARACTER));
    REGISTER_EXTERNAL_FUNC(s2c_remove_doodad, &KPlayerClient::OnRemoveDoodad, sizeof(S2C_REMOVE_DOODAD));

	REGISTER_EXTERNAL_FUNC(s2c_character_action, &KPlayerClient::OnCharacterAction, sizeof(S2C_CHARACTER_ACTION));

    REGISTER_EXTERNAL_FUNC(s2c_over_time_action, &KPlayerClient::OnOverTimeAction, sizeof(S2C_OVER_TIME_ACTION));

    REGISTER_EXTERNAL_FUNC(s2c_sync_move_ctrl, &KPlayerClient::OnSyncMoveCtrl, sizeof(S2C_SYNC_MOVE_CTRL));
    REGISTER_EXTERNAL_FUNC(s2c_sync_move_param, &KPlayerClient::OnSyncMoveParam, sizeof(S2C_SYNC_MOVE_PARAM));
    REGISTER_EXTERNAL_FUNC(s2c_adjust_player_move, &KPlayerClient::OnAdjustPlayerMove, sizeof(S2C_ADJUST_PLAYER_MOVE));
    REGISTER_EXTERNAL_FUNC(s2c_sync_move_state, &KPlayerClient::OnSyncMoveState, sizeof(S2C_SYNC_MOVE_STATE));

	REGISTER_EXTERNAL_FUNC(s2c_character_death, &KPlayerClient::OnCharacterDeath, sizeof(S2C_CHARACTER_DEATH));

	REGISTER_EXTERNAL_FUNC(s2c_sync_doodad_state, &KPlayerClient::OnSyncDoodadState, sizeof(S2C_SYNC_DOODAD_STATE));

    REGISTER_EXTERNAL_FUNC(s2c_sync_represent_state, &KPlayerClient::OnSyncRepresentState, sizeof(S2C_SYNC_REPRESENT_STATE));

	REGISTER_EXTERNAL_FUNC(s2c_player_display_data, &KPlayerClient::OnPlayerDisplayData, sizeof(S2C_PLAYER_DISPLAY_DATA));

	REGISTER_EXTERNAL_FUNC(s2c_sync_experience, &KPlayerClient::OnSyncExperience, sizeof(S2C_SYNC_EXPERIENCE));
    REGISTER_EXTERNAL_FUNC(s2c_player_level_up, &KPlayerClient::OnPlayerLevelUp, sizeof(S2C_PLAYER_LEVEL_UP));
    
    REGISTER_EXTERNAL_FUNC(s2c_common_health, &KPlayerClient::OnCommonHealth, sizeof(S2C_COMMON_HEALTH));
    

	REGISTER_EXTERNAL_FUNC(s2c_frame_signal, &KPlayerClient::OnFrameSignal, sizeof(S2C_FRAME_SIGNAL));
    REGISTER_EXTERNAL_FUNC(s2c_ping_signal, &KPlayerClient::OnPingSignal, sizeof(S2C_PING_SIGNAL));

    // ------------------------------ skill about -------------------------------------------->

	REGISTER_EXTERNAL_FUNC(s2c_sync_skill_data, &KPlayerClient::OnSyncSkillData, sizeof(S2C_SYNC_SKILL_DATA));
    REGISTER_EXTERNAL_FUNC(s2c_sync_skill_recipe, &KPlayerClient::OnSyncSkillRecipe, UNDEFINED_PROTOCOL_SIZE);

	REGISTER_EXTERNAL_FUNC(s2c_skill_prepare, &KPlayerClient::OnSkillPrepare, sizeof(S2C_SKILL_PREPARE));
    REGISTER_EXTERNAL_FUNC(s2c_skill_cast, &KPlayerClient::OnSkillCast, sizeof(S2C_SKILL_CAST));
    REGISTER_EXTERNAL_FUNC(s2c_sync_common_skill_auto_cast_state, &KPlayerClient::OnSyncCommonSkillAutoCastState, sizeof(S2C_SYNC_COMMON_SKILL_AUTO_CAST_STATE));
    REGISTER_EXTERNAL_FUNC(s2c_reset_cooldown, &KPlayerClient::OnResetCooldown, sizeof(S2C_RESET_COOLDOWN));
    REGISTER_EXTERNAL_FUNC(s2c_skill_chain_effect, &KPlayerClient::OnSkillChainEffect, UNDEFINED_PROTOCOL_SIZE);
    
	REGISTER_EXTERNAL_FUNC(s2c_skill_effect_result, &KPlayerClient::OnSkillEffectResult, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_sync_behit_represent, &KPlayerClient::OnSyncBehitRepresent, sizeof(S2C_SYNC_BEHIT_REPRESENT));
    REGISTER_EXTERNAL_FUNC(s2c_sync_buff_list, &KPlayerClient::OnSyncBuffList, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_sync_buff_single, &KPlayerClient::OnSyncBuffSingle, sizeof(S2C_SYNC_BUFF_SINGLE));
    REGISTER_EXTERNAL_FUNC(s2c_sync_kungfu_mount, &KPlayerClient::OnSyncKungfuMount, sizeof(S2C_SYNC_KUNGFU_MOUNT));
    REGISTER_EXTERNAL_FUNC(s2c_skill_beat_back, &KPlayerClient::OnSkillBeatBack, sizeof(S2C_SKILL_BEAT_BACK));
    REGISTER_EXTERNAL_FUNC(s2c_sync_sheath_state, &KPlayerClient::OnSyncSheathState, sizeof(S2C_SYNC_SHEATH_STATE));

    // -------------------------------- 数值同步 -----------------------------------------------
    REGISTER_EXTERNAL_FUNC(s2c_sync_self_max_lmrs, &KPlayerClient::OnSyncSelfMaxLMRS, sizeof(S2C_SYNC_SELF_MAX_LMRS));
    REGISTER_EXTERNAL_FUNC(s2c_sync_self_current_lmrs, &KPlayerClient::OnSyncSelfCurrentLMRS, sizeof(S2C_SYNC_SELF_CURRENT_LMRS));
    REGISTER_EXTERNAL_FUNC(s2c_sync_self_weak_info, &KPlayerClient::OnSyncSelfWeakInfo, sizeof(S2C_SYNC_SELF_WEAK_INFO));
    REGISTER_EXTERNAL_FUNC(s2c_sync_select_character_max_lmr, &KPlayerClient::OnSyncSelectCharacterMaxLMR, sizeof(S2C_SYNC_SELECT_CHARACTER_MAX_LMR));
    REGISTER_EXTERNAL_FUNC(s2c_sync_select_character_current_lmr, &KPlayerClient::OnSyncSelectCharacterCurrentLMR, sizeof(S2C_SYNC_SELECT_CHARACTER_CURRENT_LMR));
    REGISTER_EXTERNAL_FUNC(s2c_sync_select_character_weak_info, &KPlayerClient::OnSyncSelectCharacterWeakInfo, sizeof(S2C_SYNC_SELECT_CHARACTER_WEAK_INFO));
    REGISTER_EXTERNAL_FUNC(s2c_sync_team_member_max_lmr, &KPlayerClient::OnSyncTeamMemberMaxLMR, sizeof(S2C_SYNC_TEAM_MEMBER_MAX_LMR));
    REGISTER_EXTERNAL_FUNC(s2c_sync_team_member_current_lmr, &KPlayerClient::OnSyncTeamMemberCurrentLMR, sizeof(S2C_SYNC_TEAM_MEMBER_CURRENT_LMR));
    REGISTER_EXTERNAL_FUNC(s2c_sync_team_member_misc, &KPlayerClient::OnSyncTeamMemberMisc, sizeof(S2C_SYNC_TEAM_MEMBER_MISC));
    REGISTER_EXTERNAL_FUNC(s2c_sync_team_member_position, &KPlayerClient::OnSyncTeamMemberPosition, sizeof(S2C_SYNC_TEAM_MEMBER_POSITION));
	REGISTER_EXTERNAL_FUNC(s2c_broadcast_character_life, &KPlayerClient::OnBroadcastCharacterLife, sizeof(S2C_BROADCAST_CHARACTER_LIFE));
    REGISTER_EXTERNAL_FUNC(s2c_sync_player_state_info, &KPlayerClient::OnSyncPlayerStateInfo, sizeof(S2C_SYNC_PLAYER_STATE_INFO));

    // ------------------------------ 组队相关 --------------------------------------------

    REGISTER_EXTERNAL_FUNC(s2c_sync_player_team_base_info, &KPlayerClient::OnSyncPlayerTeamBaseInfo, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_invite_join_team_request, &KPlayerClient::OnInvitePlayerJoinTeamRequest, sizeof(S2C_INVITE_JOIN_TEAM_REQUEST));
    REGISTER_EXTERNAL_FUNC(s2c_apply_join_team_request, &KPlayerClient::OnApplyJoinTeamRequest, sizeof(S2C_APPLY_JOIN_TEAM_REQUEST));
    REGISTER_EXTERNAL_FUNC(s2c_team_add_member_notify, &KPlayerClient::OnTeamAddMemberNotify, sizeof(S2C_TEAM_ADD_MEMBER_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_del_member_notify, &KPlayerClient::OnTeamDelMemberNotify, sizeof(S2C_TEAM_DEL_MEMBER_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_change_authority_notify, &KPlayerClient::OnTeamChangeAuthorityNotify, sizeof(S2C_TEAM_CHANGE_AUTHORITY_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_set_formation_leader_notify, &KPlayerClient::OnTeamSetFormationLeaderNotify, sizeof(S2C_TEAM_SET_FORMATION_LEADER_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_set_loot_mode_notify, &KPlayerClient::OnTeamSetLootModeNotify, sizeof(S2C_TEAM_SET_LOOT_MODE_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_disband_notify, &KPlayerClient::OnTeamDisbandNotify, sizeof(S2C_TEAM_DISBAND_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_team_member_online_flag, &KPlayerClient::OnSyncTeamMemberOnlineFlag, sizeof(S2C_SYNC_TEAM_MEMBER_ONLINE_FLAG));
    REGISTER_EXTERNAL_FUNC(s2c_team_member_signpost_notify, &KPlayerClient::OnTeamMemberSignpostNotify, sizeof(S2C_TEAM_MEMBER_SIGNPOST_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_set_mark_respond, &KPlayerClient::OnTeamSetMarkRespond, sizeof(S2C_TEAM_SET_MARK_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_team_camp_change, &KPlayerClient::OnTeamCampChange, sizeof(S2C_TEAM_CAMP_CHANGE));
    REGISTER_EXTERNAL_FUNC(s2c_team_level_up_raid_notify, &KPlayerClient::OnTeamLevelUpRaidNotify, sizeof(S2C_TEAM_LEVEL_UP_RAID_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_team_change_member_group_notify, &KPlayerClient::OnTeamChangeMemberGroupNotify, sizeof(S2C_TEAM_CHANGE_MEMBER_GROUP_NOTIFY));

    REGISTER_EXTERNAL_FUNC(s2c_open_shop_notify, &KPlayerClient::OnOpenShopNotify, sizeof(S2C_OPEN_SHOP_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_update_shop_item, &KPlayerClient::OnUpdateShopItem, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_trading_invite_request, &KPlayerClient::OnTradingInviteRequest, sizeof(S2C_TRADING_INVITE_REQUEST));
   
    REGISTER_EXTERNAL_FUNC(s2c_open_trading_box_notify, &KPlayerClient::OnOpenTradingBoxNotify, sizeof(S2C_OPEN_TRADING_BOX_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_trading_confirm, &KPlayerClient::OnSyncTradingConfirm, sizeof(S2C_SYNC_TRADING_CONFIRM));
    REGISTER_EXTERNAL_FUNC(s2c_sync_trading_item_data, &KPlayerClient::OnSyncTradingItemData, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_sync_trading_money, &KPlayerClient::OnSyncTradingMoney, sizeof(S2C_SYNC_TRADING_MONEY));
    REGISTER_EXTERNAL_FUNC(s2c_trading_complete, &KPlayerClient::OnTradingComplete, sizeof(S2C_TRADING_COMPLETE));

    REGISTER_EXTERNAL_FUNC(s2c_sync_money, &KPlayerClient::OnSyncMoney, sizeof(S2C_SYNC_MONEY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_contribution, &KPlayerClient::OnSyncContribution, sizeof(S2C_SYNC_CONTRIBUTION));
    REGISTER_EXTERNAL_FUNC(s2c_sync_item_data, &KPlayerClient::OnSyncItemData, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_sync_item_destroy, &KPlayerClient::OnSyncItemDestroy, sizeof(S2C_SYNC_ITEM_DESTROY));
	REGISTER_EXTERNAL_FUNC(s2c_exchange_item, &KPlayerClient::OnExchangeItem, sizeof(S2C_EXCHANGE_ITEM));
	REGISTER_EXTERNAL_FUNC(s2c_update_item_amount, &KPlayerClient::OnUpdateItemAmount, sizeof(S2C_UPDATE_ITEM_AMOUNT));
    REGISTER_EXTERNAL_FUNC(s2c_sync_equip_represent, &KPlayerClient::OnSyncEquipRepresent, sizeof(S2C_SYNC_EQUIP_REPRESENT));
	REGISTER_EXTERNAL_FUNC(s2c_add_item_notify, &KPlayerClient::OnAddItemNotify, sizeof(S2C_ADD_ITEM_NOTIFY));
    

	REGISTER_EXTERNAL_FUNC(s2c_learn_profession_notify, &KPlayerClient::OnLearnProfessionNotify, sizeof(S2C_LEARN_PROFESSION_NOTIFY));
	REGISTER_EXTERNAL_FUNC(s2c_learn_branch_notify, &KPlayerClient::OnLearnBranchNotify,  sizeof(S2C_LEARN_BRANCH_NOTIFY));
	REGISTER_EXTERNAL_FUNC(s2c_forget_profession_notify, &KPlayerClient::OnForgetProfessionNotify, sizeof(S2C_FORGET_PROFESSION_NOTIFY));
	REGISTER_EXTERNAL_FUNC(s2c_add_profession_proficiency, &KPlayerClient::OnAddProfessionProficiency, sizeof(S2C_ADD_PROFESSION_PROFICIENCY));
	REGISTER_EXTERNAL_FUNC(s2c_set_max_profession_level_notify, &KPlayerClient::OnSetMaxProfessionLevelNotify, sizeof(S2C_SET_MAX_PROFESSION_LEVEL_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_profession_adjust_level, &KPlayerClient::OnSyncProfessionAdjustLevel, sizeof(S2C_SYNC_PROFESSION_ADJUST_LEVEL));
	REGISTER_EXTERNAL_FUNC(s2c_update_player_profession, &KPlayerClient::OnUpdatePlayerProfession, sizeof(S2C_UPDATE_PLAYER_PROFESSION));

	REGISTER_EXTERNAL_FUNC(s2c_learn_recipe_notify, &KPlayerClient::OnLearnRecipeNotify, sizeof(S2C_LEARN_RECIPE_NOTIFY));
	REGISTER_EXTERNAL_FUNC(s2c_forget_recipe_notify, &KPlayerClient::OnForgetRecipeNotify, sizeof(S2C_FORGET_RECIPE_NOTIFY));
	REGISTER_EXTERNAL_FUNC(s2c_sync_recipe_state, &KPlayerClient::OnSyncRecipeState, UNDEFINED_PROTOCOL_SIZE);
    
	REGISTER_EXTERNAL_FUNC(s2c_open_window, &KPlayerClient::OnOpenWindow, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_start_use_recipe, &KPlayerClient::OnStartUseRecipe, sizeof(S2C_START_USE_RECIPE));

	REGISTER_EXTERNAL_FUNC(s2c_accept_quest, &KPlayerClient::OnAcceptQuest, sizeof(S2C_ACCEPT_QUEST));
	REGISTER_EXTERNAL_FUNC(s2c_finish_quest, &KPlayerClient::OnFinishQuest, sizeof(S2C_FINISH_QUEST));
	REGISTER_EXTERNAL_FUNC(s2c_cancel_quest, &KPlayerClient::OnCancelQuest, sizeof(S2C_CANCEL_QUEST));
    REGISTER_EXTERNAL_FUNC(s2c_clear_quest, &KPlayerClient::OnClearQuest, sizeof(S2C_CLEAR_QUEST));
	REGISTER_EXTERNAL_FUNC(s2c_share_quest, &KPlayerClient::OnShareQuest, sizeof(S2C_SHARE_QUEST));
	REGISTER_EXTERNAL_FUNC(s2c_sync_quest_value, &KPlayerClient::OnSyncQuestValue, sizeof(S2C_SYNC_QUEST_VALUE));
	REGISTER_EXTERNAL_FUNC(s2c_start_escort_quest, &KPlayerClient::OnStartEscortQuest, sizeof(S2C_START_ESCORT_QUEST));

	REGISTER_EXTERNAL_FUNC(s2c_sync_loot_list, &KPlayerClient::OnSyncLootList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_roll_item_message, &KPlayerClient::OnRollItemMessage, sizeof(S2C_ROLL_ITEM_MESSAGE));
	REGISTER_EXTERNAL_FUNC(s2c_open_loot_list, &KPlayerClient::OnOpenLootList, sizeof(S2C_OPEN_LOOT_LIST));
	REGISTER_EXTERNAL_FUNC(s2c_sync_fight_flag, &KPlayerClient::OnSyncFightFlag, sizeof(S2C_SYNC_FIGHT_FLAG));
	REGISTER_EXTERNAL_FUNC(s2c_sync_repute, &KPlayerClient::OnSyncRepute, sizeof(S2C_SYNC_REPUTE));
	REGISTER_EXTERNAL_FUNC(s2c_sync_fightflag_list, &KPlayerClient::OnSyncFightflagList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_sync_repute_list, &KPlayerClient::OnSyncReputeList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_show_reputation, &KPlayerClient::OnShowReputation, sizeof(S2C_SHOW_REPUTATION));

	REGISTER_EXTERNAL_FUNC(s2c_character_open, &KPlayerClient::OnCharacterOpen, sizeof(S2C_CHARACTER_OPEN));
    REGISTER_EXTERNAL_FUNC(s2c_distribute_item_message, &KPlayerClient::OnDistributeItemMessage, sizeof(S2C_DISTRIBUTE_ITEM_MESSAGE));
	REGISTER_EXTERNAL_FUNC(s2c_finish_loot, &KPlayerClient::OnFinishLoot, sizeof(S2C_FINISH_LOOT));
	REGISTER_EXTERNAL_FUNC(s2c_begin_roll_item, &KPlayerClient::OnBeginRollItem, sizeof(S2C_BEGIN_ROLL_ITEM));
	REGISTER_EXTERNAL_FUNC(s2c_custom_OTAction, &KPlayerClient::OnCustomOTAction, sizeof(S2C_CUSTOM_OTACTION));
	REGISTER_EXTERNAL_FUNC(s2c_modify_threat, &KPlayerClient::OnModifyThreat, sizeof(S2C_MODIFY_THREAT));
	REGISTER_EXTERNAL_FUNC(s2c_modify_threaten, &KPlayerClient::OnModifyThreaten, sizeof(S2C_MODIFY_THREATEN));
	REGISTER_EXTERNAL_FUNC(s2c_clear_threat, &KPlayerClient::OnClearThreat, sizeof(S2C_CLEAR_THREAT));
	REGISTER_EXTERNAL_FUNC(s2c_clear_threaten, &KPlayerClient::OnClearThreaten, sizeof(S2C_CLEAR_THREATEN));

    REGISTER_EXTERNAL_FUNC(s2c_talk_message, &KPlayerClient::OnTalkMessage, UNDEFINED_PROTOCOL_SIZE);

	REGISTER_EXTERNAL_FUNC(s2c_sync_self_current_st, &KPlayerClient::OnSyncSelfCurrentST, sizeof(S2C_SYNC_SELF_CURRENT_ST));
    REGISTER_EXTERNAL_FUNC(s2c_doodad_loot_over, &KPlayerClient::OnDoodadLootOver, sizeof(S2C_DOODAD_LOOT_OVER));

    REGISTER_EXTERNAL_FUNC(s2c_sync_fellowship_list, &KPlayerClient::OnSyncFellowshipList, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_sync_fellowship_group_name, &KPlayerClient::OnSyncFellowshipGroupName, sizeof(S2C_SYNC_FELLOWSHIP_GROUP_NAME));

	REGISTER_EXTERNAL_FUNC(s2c_npc_talk_to_player, &KPlayerClient::OnNpcTalkToPlayer, sizeof(S2C_NPC_TALK_TO_PLAYER));
	REGISTER_EXTERNAL_FUNC(s2c_set_force, &KPlayerClient::OnSetForce, sizeof(S2C_SET_FORCE));
	
    REGISTER_EXTERNAL_FUNC(s2c_send_mail_respond, &KPlayerClient::OnSendMailRespond, sizeof(S2C_SEND_MAIL_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_get_maillist_respond, &KPlayerClient::OnGetMailListRespond, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_new_mail_notify, &KPlayerClient::OnNewMailNotify, sizeof(S2C_NEW_MAIL_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_mail_content, &KPlayerClient::OnSyncMailContent, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_mail_general_respond, &KPlayerClient::OnMailGeneralRespond, sizeof(S2C_MAIL_GENERAL_RESPOND));
    
    REGISTER_EXTERNAL_FUNC(s2c_sync_force_id, &KPlayerClient::OnSyncForceId, sizeof(S2C_SYNC_FORCE_ID));

	REGISTER_EXTERNAL_FUNC(s2c_sync_user_preferences, &KPlayerClient::OnSyncUserPreferences, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_sync_all_book_state, &KPlayerClient::OnSyncAllBookState, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(s2c_sync_book_state, &KPlayerClient::OnSyncBookState, sizeof(S2C_SYNC_BOOK_STATE));
	REGISTER_EXTERNAL_FUNC(s2c_character_change_model, &KPlayerClient::OnCharacterChangeModel, sizeof(S2C_CHARACTER_CHANGE_MODEL));
	REGISTER_EXTERNAL_FUNC(s2c_sync_npc_dialog_flag, &KPlayerClient::OnSyncNpcDialogFlag, sizeof(S2C_SYNC_NPC_DIALOG_FLAG));
	
	REGISTER_EXTERNAL_FUNC(s2c_sync_pk_state, &KPlayerClient::OnSyncPKState, sizeof(S2C_SYNC_PK_STATE));
	REGISTER_EXTERNAL_FUNC(s2c_win_duel_broadcast, &KPlayerClient::OnWinDuelBroadcast, sizeof(S2C_WIN_DUEL_BROADCAST));
	
	REGISTER_EXTERNAL_FUNC(s2c_sync_role_data_over, &KPlayerClient::OnSyncRoleDataOver, sizeof(S2C_SYNC_ROLE_DATA_OVER));
	REGISTER_EXTERNAL_FUNC(s2c_sync_enable_bank_package, &KPlayerClient::OnSyncEnableBankPackage, sizeof(S2C_SYNC_ENABLE_BANK_PACKAGE));
	REGISTER_EXTERNAL_FUNC(s2c_update_map_mark, &KPlayerClient::OnUpdateMapMark, sizeof(S2C_UPDATE_MAP_MARK));

	REGISTER_EXTERNAL_FUNC(s2c_sync_mid_map_mark, &KPlayerClient::OnSyncMidMapMark,sizeof(S2C_SYNC_MID_MAP_MARK));
	REGISTER_EXTERNAL_FUNC(s2c_play_sound, &KPlayerClient::OnPlaySound, sizeof(S2C_PLAY_SOUND));
	REGISTER_EXTERNAL_FUNC(s2c_play_mini_game, &KPlayerClient::OnPlayMiniGame, sizeof(S2C_PLAY_MINI_GAME));
    
    REGISTER_EXTERNAL_FUNC(s2c_sync_horse_flag, &KPlayerClient::OnSyncHorseFlag, sizeof(S2C_SYNC_HORSE_FLAG));

    REGISTER_EXTERNAL_FUNC(s2c_road_track_force_respond, &KPlayerClient::OnRoadTrackForceRespond, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_sync_route_node_open_list, &KPlayerClient::OnSyncRouteNodeOpenList, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(s2c_sync_player_train, &KPlayerClient::OnSyncPlayerTrain, sizeof(S2C_SYNC_PLAYER_TRAIN));
    REGISTER_EXTERNAL_FUNC(s2c_broadcast_player_school_id, &KPlayerClient::OnBroadcastPlayerSchoolID, sizeof(S2C_BROADCAST_PLAYER_SCHOOL_ID));
    REGISTER_EXTERNAL_FUNC(s2c_sync_prink_force, &KPlayerClient::OnSyncPrinkForce, sizeof(S2C_SYNC_PRINK_FORCE));
    REGISTER_EXTERNAL_FUNC(s2c_sync_visit_map_info, &KPlayerClient::OnSyncVisitMapInfo, sizeof(S2C_SYNC_VISIT_MAP_INFO));

    REGISTER_EXTERNAL_FUNC(s2c_sync_accumulate_value, &KPlayerClient::OnSyncAccumulateValue, sizeof(S2C_SYNC_ACCUMULATE_VALUE));

    REGISTER_EXTERNAL_FUNC(s2c_set_target, &KPlayerClient::OnSetTarget, sizeof(S2C_SET_TARGET));
    REGISTER_EXTERNAL_FUNC(s2c_sync_kill_point, &KPlayerClient::OnSyncKillPoint, sizeof(S2C_SYNC_KILL_POINT));
    REGISTER_EXTERNAL_FUNC(s2c_set_player_red_name, &KPlayerClient::OnSetPlayerRedName, sizeof(S2C_SET_PLAYER_RED_NAME));
    REGISTER_EXTERNAL_FUNC(s2c_set_player_dialog_flag, &KPlayerClient::OnSetPlayerDialogFlag, sizeof(S2C_SET_PLAYER_DIALOG_FLAG));
    REGISTER_EXTERNAL_FUNC(s2c_set_only_revive_in_situ, &KPlayerClient::OnSetOnlyReviveInSitu, sizeof(S2C_SET_ONLY_REVIVE_IN_SITU));

    REGISTER_EXTERNAL_FUNC(s2c_sync_formation_coefficient, &KPlayerClient::OnSyncFormationCoefficient, sizeof(S2C_SYNC_FORMATION_COEFFICIENT));

    REGISTER_EXTERNAL_FUNC(s2c_join_battle_field_queue_respond, &KPlayerClient::OnJoinBattleFieldQueueRespond, sizeof(S2C_JOIN_BATTLE_FIELD_QUEUE_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_leave_battle_field_queue_respond, &KPlayerClient::OnLeaveBattleFieldQueueRespond, sizeof(S2C_LEAVE_BATTLE_FIELD_QUEUE_RESPOND));

    REGISTER_EXTERNAL_FUNC(s2c_battle_field_notify, &KPlayerClient::OnBattleFieldNotify, sizeof(S2C_BATTLE_FIELD_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_battle_field_list, &KPlayerClient::OnSyncBattleFieldList, sizeof(S2C_SYNC_BATTLE_FIELD_LIST));
    REGISTER_EXTERNAL_FUNC(s2c_sync_battle_field_side, &KPlayerClient::OnSyncBattleFieldSide, sizeof(S2C_SYNC_BATTLE_FIELD_SIDE));
    REGISTER_EXTERNAL_FUNC(s2c_battle_field_end, &KPlayerClient::OnBattleFieldEnd, sizeof(S2C_BATTLE_FIELD_END));

    REGISTER_EXTERNAL_FUNC(s2c_sync_camp_info, &KPlayerClient::OnSyncCampInfo, sizeof(S2C_SYNC_CAMP_INFO));
    REGISTER_EXTERNAL_FUNC(s2c_sync_current_prestige, &KPlayerClient::OnSyncCurrentPrestige, sizeof(S2C_SYNC_CURRENT_PRESTIGE));
    
    // ------------------------------ 帮会相关 --------------------------------------------
    REGISTER_EXTERNAL_FUNC(s2c_sync_tong_member_info, &KPlayerClient::OnSyncTongMemberInfo, sizeof(S2C_SYNC_TONG_MEMBER_INFO));
    REGISTER_EXTERNAL_FUNC(s2c_delete_tong_member_notify, &KPlayerClient::OnTongDeleteMemberNotify, sizeof(S2C_DELETE_TONG_MEMBER_NOTIFY));   
    REGISTER_EXTERNAL_FUNC(s2c_apply_tong_info_respond, &KPlayerClient::OnApplyTongInfoRespond, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_update_tong_client_data_version, &KPlayerClient::OnUpdateTongClientDataVersion, sizeof(S2C_UPDATE_TONG_CLIENT_DATA_VERSION));
    
    //REGISTER_EXTERNAL_FUNC(s2c_sync_tong_repertory_page_respond, &KPlayerClient::OnSyncTongRepertoryPageRespond, UNDEFINED_PROTOCOL_SIZE);
    
    REGISTER_EXTERNAL_FUNC(s2c_invite_player_join_tong_request,  &KPlayerClient::OnInvitePlayerJoinTongRequest, sizeof(S2C_INVITE_PLAYER_JOIN_TONG_REQUEST));
    REGISTER_EXTERNAL_FUNC(s2c_change_player_tong_notify, &KPlayerClient::OnChangePlayerTongNotify, sizeof(S2C_CHANGE_PLAYER_TONG_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_tong_broadcast_message, &KPlayerClient::OnBroadcastTongMessage, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_get_tong_description_respond, &KPlayerClient::OnGetTongDescriptionRespond, sizeof(S2C_GET_TONG_DESCRIPTION_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_broadcast_player_change_tong, &KPlayerClient::OnBroadcastPlayerChangeTong, sizeof(S2C_BROADCAST_PLAYER_CHANGE_TONG));
    REGISTER_EXTERNAL_FUNC(s2c_sync_tong_online_message, &KPlayerClient::OnSyncTongOnlineMessage, UNDEFINED_PROTOCOL_SIZE);
    //REGISTER_EXTERNAL_FUNC(s2c_apply_open_tong_repertory_respond, &KPlayerClient::OnApplyOpenTongRepertoryRespond, sizeof(S2C_APPLY_OPEN_TONG_REPERTORY_RESPOND));
    //REGISTER_EXTERNAL_FUNC(s2c_sync_tong_history_respond, &KPlayerClient::OnSyncTongHistoryRespond, UNDEFINED_PROTOCOL_SIZE);
    // ------------------------------------------------------------------------------------

    REGISTER_EXTERNAL_FUNC(s2c_set_camp, &KPlayerClient::OnSetCamp, sizeof(S2C_SET_CAMP));

    REGISTER_EXTERNAL_FUNC(s2c_sync_battlefield_statistics, &KPlayerClient::OnSyncBattleFieldStatistics, sizeof(S2C_SYNC_BATTLEFIELD_STATISTICS));    
    REGISTER_EXTERNAL_FUNC(s2c_sync_battlefield_objective, &KPlayerClient::OnSyncBattleFieldObjective, sizeof(S2C_SYNC_BATTLEFIELD_OBJECTIVE));
    REGISTER_EXTERNAL_FUNC(s2c_sync_battlefield_pq_info, &KPlayerClient::OnSyncBattleFieldPQInfo, sizeof(S2C_SYNC_BATTLEFIELD_PQ_INFO));

    REGISTER_EXTERNAL_FUNC(s2c_sync_hero_flag, &KPlayerClient::OnSyncHeroFlag, sizeof(S2C_SYNC_HERO_FLAG));

    REGISTER_EXTERNAL_FUNC(s2c_sync_npc_drop_target, &KPlayerClient::OnSyncNpcDropTarget, sizeof(S2C_SYNC_NPC_DROP_TARGET));

    // Auction about
    REGISTER_EXTERNAL_FUNC(s2c_auction_lookup_respond, &KPlayerClient::OnAuctionLookupRespond, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_auction_bid_respond, &KPlayerClient::OnAuctionBidRespond, sizeof(S2C_AUCTION_BID_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_auction_sell_respond, &KPlayerClient::OnAuctionSellRespond, sizeof(S2C_AUCTION_SELL_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_auction_cancel_respond, &KPlayerClient::OnAuctionCancelRespond, sizeof(S2C_AUCTION_CANCEL_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_auction_message_notify, &KPlayerClient::OnAuctionMessageNotify, sizeof(S2C_AUCTION_MESSAGE_NOTIFY));

    REGISTER_EXTERNAL_FUNC(s2c_remote_lua_call, &KPlayerClient::OnRemoteLuaCall, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(s2c_sync_player_revive_ctrl, &KPlayerClient::OnSyncPlayerReviveCtrl, sizeof(S2C_SYNC_PLAYER_REVIVE_CTRL));

    REGISTER_EXTERNAL_FUNC(s2c_sync_achievement_data, &KPlayerClient::OnSyncAchievementData, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_new_achievement_notify, &KPlayerClient::OnNewAchievementNotify, sizeof(S2C_NEW_ACHIEVEMENT_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_achievement_announce_notify, &KPlayerClient::OnAchievementAnnounceNotify, sizeof(S2C_ACHIEVEMENT_ANNOUNCE_NOTIFY));
    REGISTER_EXTERNAL_FUNC(s2c_sync_achievement_point, &KPlayerClient::OnSyncAchievementPoint, sizeof(S2C_SYNC_ACHIEVEMENT_POINT));
    REGISTER_EXTERNAL_FUNC(s2c_sync_achievement_count, &KPlayerClient::OnSyncAchievementCount, sizeof(S2C_SYNC_ACHIEVEMENT_COUNT));

    REGISTER_EXTERNAL_FUNC(s2c_sync_designation_data, &KPlayerClient::OnSyncDesignationData, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_set_generation, &KPlayerClient::OnSetGenerationNotify, sizeof(S2C_SET_GENERATION));
    REGISTER_EXTERNAL_FUNC(s2c_acquire_designation, &KPlayerClient::OnAcquireDesignation, sizeof(S2C_ACQUIRE_DESIGNATION));
    REGISTER_EXTERNAL_FUNC(s2c_designation_announce, &KPlayerClient::OnDesignationAnnounce, sizeof(S2C_DESIGNATION_ANNOUNCE));
    REGISTER_EXTERNAL_FUNC(s2c_remove_designation, &KPlayerClient::OnRemoveDesignation, sizeof(S2C_REMOVE_DESIGNATION));
    REGISTER_EXTERNAL_FUNC(s2c_sync_player_designation, &KPlayerClient::OnSyncPlayerDesignation, sizeof(S2C_SYNC_PLAYER_DESIGNATION));

    REGISTER_EXTERNAL_FUNC(s2c_sync_max_level, &KPlayerClient::OnSyncMaxLevel, sizeof(S2C_SYNC_MAX_LEVEL));
    REGISTER_EXTERNAL_FUNC(s2c_use_item_prepare, &KPlayerClient::OnUseItemPrepare, sizeof(S2C_USE_ITEM_PREPARE));
    REGISTER_EXTERNAL_FUNC(s2c_create_view_point_respond, &KPlayerClient::OnCreateViewPointRespond, sizeof(S2C_CREATE_VIEW_POINT_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_remove_view_point_respond, &KPlayerClient::OnRemoveViewPointRespond, sizeof(S2C_REMOVE_VIEW_POINT_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_move_view_point_respond, &KPlayerClient::OnMoveViewPointRespond, sizeof(S2C_MOVE_VIEW_POINT_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_account_end_of_day_time, &KPlayerClient::OnAccountEndOfDayTime, sizeof(S2C_ACCOUNT_END_OF_DAY_TIME));
    REGISTER_EXTERNAL_FUNC(s2c_sync_free_limit_flag_info, &KPlayerClient::OnSyncFreeLimitFlagInfo, sizeof(S2C_SYNC_FREE_LIMIT_FLAG_INFO));

    REGISTER_EXTERNAL_FUNC(s2c_apex_protocol, &KPlayerClient::OnApexProtocol, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(s2c_game_card_sell_respond, &KPlayerClient::OnGameCardSellRespond, sizeof(S2C_GAME_CARD_SELL_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_game_card_buy_respond, &KPlayerClient::OnGameCardBuyRespond, sizeof(S2C_GAME_CARD_BUY_RESPOND));
    REGISTER_EXTERNAL_FUNC(s2c_game_card_lookup_respond, &KPlayerClient::OnGameCardLookupRespond, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(s2c_game_card_cancel_respond, &KPlayerClient::OnGameCardCancelRespond, sizeof(S2C_GAME_CARD_CANCEL_RESPOND));

    REGISTER_EXTERNAL_FUNC(s2c_sync_coin, &KPlayerClient::OnSyncCoin, sizeof(S2C_SYNC_COIN));

    REGISTER_EXTERNAL_FUNC(s2c_seek_mentor_yell, &KPlayerClient::OnSeekMentorYell, sizeof(S2C_SEEK_MENTOR_YELL));
    REGISTER_EXTERNAL_FUNC(s2c_seek_apprentice_yell, &KPlayerClient::OnSeekApprenticeYell, sizeof(S2C_SEEK_APPRENTICE_YELL));    
    REGISTER_EXTERNAL_FUNC(s2c_sync_mentor_data, &KPlayerClient::OnSyncMentorData, sizeof(S2C_SYNC_MENTOR_DATA));
    REGISTER_EXTERNAL_FUNC(s2c_delete_mentor_data, &KPlayerClient::OnDeleteMentorData, sizeof(S2C_DELETE_MENTOR_DATA));
    REGISTER_EXTERNAL_FUNC(s2c_sync_camp_flag, &KPlayerClient::OnSyncPlayerCampFlag, sizeof(S2C_SYNC_CAMP_FLAG));

    REGISTER_EXTERNAL_FUNC(s2c_character_threat_ranklist_respond, &KPlayerClient::OnCharacterThreatRanklistRespond, UNDEFINED_PROTOCOL_SIZE);
    //AutoCode:注册协议
}

KPlayerClient::~KPlayerClient(void)
{
    assert(!m_piSocketStream);
}

BOOL KPlayerClient::Init()
{
    int nResult  = false;
	int nRetCode = false;

	nRetCode = m_PlayerDisplayDataCache.Init(PLAYER_DISPLAY_DATA_MAX_CACHE);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_NpcDisplayDataCache.Init(NPC_DISPLAY_DATA_MAX_CACHE);
	KGLOG_PROCESS_ERROR(nRetCode);
    
    m_bSendErrorFlag    = false;
    m_nFrameSendCount   = 0;
    m_nLastSendCountResetFrame = 0;
    m_dwPingEchoTime    = 0;
    m_dwNextPingTime    = 0;

    nRetCode = InitStartTimeEvaluation(&m_StartTimeEvaluation);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
	return (BOOL)nResult;
}

void KPlayerClient::UnInit(void)
{
    IKG_Buffer* piPacket = NULL;

    while (!m_BufferPacketList.empty())
    {
        piPacket = m_BufferPacketList.front();
        KG_COM_RELEASE(piPacket);
        m_BufferPacketList.pop_front();
    }

    while (!m_SendBufferList.empty())
    {
        piPacket = m_SendBufferList.front();
        KG_COM_RELEASE(piPacket);
        m_SendBufferList.pop_front();
    }

    KG_COM_RELEASE(m_piSocketStream);

    return;
}

BOOL KPlayerClient::Connect(const char* pszIPAddr, int nPort)
{
    BOOL                        bResult         = false;
	int                         nRetCode        = false;
    timeval                     TimeoutValue    = {0, 0};
    IKG_Buffer*                 piPacket        = NULL;
    KG_BufferedSocketConnector  Connector;

    assert(pszIPAddr);

    while (!m_BufferPacketList.empty())
    {
        piPacket = m_BufferPacketList.front();
        KGLOG_PROCESS_ERROR(piPacket);

        KG_COM_RELEASE(piPacket);
        m_BufferPacketList.pop_front();
    }

    while (!m_SendBufferList.empty())
    {
        piPacket = m_SendBufferList.front();
        KGLOG_PROCESS_ERROR(piPacket);

        KG_COM_RELEASE(piPacket);
        m_SendBufferList.pop_front();
    }

    assert(m_piSocketStream == NULL);

    m_piSocketStream = Connector.ConnectSecurity(pszIPAddr, nPort, KSG_ENCODE_DECODE);
    KGLOG_PROCESS_ERROR(m_piSocketStream);

    nRetCode = m_piSocketStream->SetTimeout(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_bSendErrorFlag        = false;
    m_nFrameSendCount       = 0;
    m_nLastSendCountResetFrame = 0;
    m_nGSGameLoop           = 0;
    m_dwNextPingTime        = 0;
    m_dwPingEchoTime        = 0;

    bResult = true;
Exit0:
    if (!bResult)
    {
		if (g_pGameWorldUIHandler)
		{
			g_pGameWorldUIHandler->OnConnectGameServerFailed();
		}
        KG_COM_RELEASE(m_piSocketStream);
    }

    KGLogPrintf(
        KGLOG_INFO, 
        "[KPlayerClient] Connect to %s : %d ... ... [%s]", 
        pszIPAddr, nPort, bResult ? "OK" : "Failed"
    );

    return bResult;
}

BOOL KPlayerClient::Disconnect()
{
    BOOL        bResult  = false;
	int         nRetCode = false;
    IKG_Buffer* piPacket = NULL;

    while (!m_BufferPacketList.empty())
    {
        piPacket = m_BufferPacketList.front();
        KG_COM_RELEASE(piPacket);
        m_BufferPacketList.pop_front();
    }

    while (!m_SendBufferList.empty())
    {
        piPacket = m_SendBufferList.front();
        KG_COM_RELEASE(piPacket);
        m_SendBufferList.pop_front();
    }

    KG_COM_RELEASE(m_piSocketStream);

    KGLogPrintf(KGLOG_INFO, "[KPlayerClient] GS disconnected.");

	m_nGSGameLoop = 0;

    bResult = true;
Exit0:
	return bResult;
}

int KPlayerClient::Breathe(void)
{
    int                     nResult          = false;
    int                     nRetCode         = false;
    IKG_Buffer*             piBuffer         = NULL;
	BYTE*			        pbyData          = NULL;
	unsigned int	        uSize            = 0;
    IKG_Buffer*             piBufferPacket   = NULL;
	KBufferPacket*          pBufferPacket    = NULL;
    PROCESS_PROTOCOL_FUNC   PFunc            = NULL;
    int                     nConnectionAlive = true;

    if (m_nLastSendCountResetFrame != g_pSO3World->m_nGameLoop)
    {
        m_nFrameSendCount = 0;
        m_nLastSendCountResetFrame = g_pSO3World->m_nGameLoop;
    }

    // 首先上一帧没有发送出去的数据先发送完。
    while (!m_SendBufferList.empty() && m_nFrameSendCount < MAX_CLIENT_SEND_PACK_FRAME)
    {
        piBufferPacket = m_SendBufferList.front();
        KGLOG_PROCESS_ERROR(piBufferPacket);

        nRetCode = RealSend(piBufferPacket);
        KGLOG_CHECK_ERROR(nRetCode);

        KG_COM_RELEASE(piBufferPacket);
        m_SendBufferList.pop_front();
    }

	// 先检测数据包缓存中的数据包，以保证解析的顺序
    while (!m_BufferPacketList.empty())
    {
        piBufferPacket = m_BufferPacketList.front();
        KGLOG_PROCESS_ERROR(piBufferPacket);

        pBufferPacket = (KBufferPacket*)piBufferPacket->GetData();
        KGLOG_PROCESS_ERROR(pBufferPacket);

        if (pBufferPacket->m_nGameLoop > g_pSO3World->m_nGameLoop)
        {
            break;
        }

        PFunc = m_ProcessProtocolFuns[pBufferPacket->m_pHeader->byProtocolID];
        KGLOG_PROCESS_ERROR(PFunc);

        m_BufferPacketList.pop_front();

        (this->*PFunc)((BYTE*)pBufferPacket->m_pHeader, pBufferPacket->m_uSize, pBufferPacket->m_nGameLoop);

        KG_COM_RELEASE(piBufferPacket);
    }

    KG_PROCESS_ERROR(m_piSocketStream);

    if (m_bSendErrorFlag)
    {
        nConnectionAlive = false;
        m_bSendErrorFlag = false;
        goto Exit0;
    }

    while (true)
    {
        timeval TimeoutValue    = {0, 0};

        KG_COM_RELEASE(piBuffer);
        KG_PROCESS_ERROR(m_piSocketStream);

        nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
        if (nRetCode == -1)
        {
            nConnectionAlive = false;
            break;
        }
        if (nRetCode == 0)
            break;  // if no data arrive

        nRetCode = m_piSocketStream->Recv(&piBuffer);
        if (nRetCode == -1)
        {
            nConnectionAlive = false;
            break;
        }
        if (nRetCode == -2)
            break;

        KGLOG_PROCESS_ERROR(nRetCode == 1);  // if timeout or error then error
        KGLOG_PROCESS_ERROR(piBuffer);

        pbyData = (BYTE*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        uSize = piBuffer->GetSize();

        nRetCode = ProcessPackage(pbyData, uSize);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    DoPingSignal();

    nResult = true;
Exit0:
    if (m_piSocketStream && !nConnectionAlive)
    {
        if (g_pGameWorldUIHandler)
		    g_pGameWorldUIHandler->OnDisconnect();
        if (g_pGameWorldRepresentHandler)
            g_pGameWorldRepresentHandler->OnDisconnect();

        Disconnect();
    }
    KG_COM_RELEASE(piBuffer);
	return nResult;
}

BOOL KPlayerClient::ProcessPackage(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult         = false;
    PROCESS_PROTOCOL_FUNC   PFunc           = NULL;
    IKG_Buffer*             piPacket        = NULL;
    KBufferPacket*          pBufferPacket   = NULL;

    while (uDataLen > 0)
    {
	    DOWNWARDS_PROTOCOL_HEADER* pHeader = (DOWNWARDS_PROTOCOL_HEADER*)pbyData;

        KGLOG_PROCESS_ERROR(pHeader->byProtocolID > gs_client_connection_begin);
        KGLOG_PROCESS_ERROR(pHeader->byProtocolID < gs_client_connection_end);

	    int nProtocolSize = m_nProtocolSize[pHeader->byProtocolID];
	    if (nProtocolSize == UNDEFINED_PROTOCOL_SIZE)
	    {
		    UNDEFINED_SIZE_DOWNWARDS_HEADER* pUndefinedSizeHeader = (UNDEFINED_SIZE_DOWNWARDS_HEADER*)pbyData;
		    nProtocolSize = pUndefinedSizeHeader->wSize;
	    }

	    KGLOG_PROCESS_ERROR((int)uDataLen >= nProtocolSize);

	    // 如果客户端游戏循环大于数据包的游戏循环数，
	    // 或者是帧间隔信号,则立刻解析
	    if (
            (g_pSO3World->m_nGameLoop >= m_nGSGameLoop) ||
		    (pHeader->byProtocolID == s2c_frame_signal) ||
            (pHeader->byProtocolID == s2c_sync_player_base_info) ||
			(pHeader->byProtocolID == s2c_ping_signal
        )
        )
	    {
            PFunc = m_ProcessProtocolFuns[pHeader->byProtocolID];
            KGLOG_PROCESS_ERROR(PFunc);

		    (this->*PFunc)(pbyData, nProtocolSize, m_nGSGameLoop);
	    }
	    else
	    {
		    piPacket = KG_MemoryCreateBuffer(sizeof(KBufferPacket) + nProtocolSize);
		    KGLOG_PROCESS_ERROR(piPacket);

		    pBufferPacket = (KBufferPacket*)piPacket->GetData();
            KGLOG_PROCESS_ERROR(pBufferPacket);

		    pBufferPacket->m_nGameLoop  = m_nGSGameLoop;
		    pBufferPacket->m_uSize      = nProtocolSize;
		    pBufferPacket->m_pHeader    = (EXTERNAL_PROTOCOL_HEADER*)((char*)pBufferPacket + sizeof(KBufferPacket));

		    memcpy(pBufferPacket->m_pHeader, pbyData, nProtocolSize);

            m_BufferPacketList.push_back(piPacket);
            piPacket = NULL;
	    }

	    uDataLen   -= nProtocolSize;
	    pbyData    += nProtocolSize;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPacket);
    return bResult;
}

BOOL KPlayerClient::Send(void* pvData, size_t nSize)
{
    int                         nResult     = false;
    int                         nRetCode    = false;
	UPWARDS_PROTOCOL_HEADER*    pHeader     = (UPWARDS_PROTOCOL_HEADER*)pvData;
    IKG_Buffer*                 piBuffer    = NULL;

	pHeader->nFrame = g_pSO3World->m_nGameLoop;

    assert(nSize > 0);
    
    KGLOG_PROCESS_ERROR(m_piSocketStream);

    piBuffer = KG_MemoryCreateBuffer((unsigned)nSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    memcpy(piBuffer->GetData(), pvData, nSize);

    if (m_nFrameSendCount < MAX_CLIENT_SEND_PACK_FRAME && m_SendBufferList.empty())
    {
        nRetCode = RealSend(piBuffer);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        size_t uListSize = m_SendBufferList.size();
        if (uListSize >= MAX_CLIENT_SEND_PACK_QUEUE)
        {
            KGLogPrintf(KGLOG_ERR, "Client send pack list full!");

            while (!m_SendBufferList.empty())
            {
                IKG_Buffer*                 piPendBuffer    = NULL;
                UPWARDS_PROTOCOL_HEADER*    pPendHeader     = NULL;

                piPendBuffer = m_SendBufferList.front();
                m_SendBufferList.pop_front();

                pPendHeader = (UPWARDS_PROTOCOL_HEADER*)piPendBuffer->GetData();

                KGLogPrintf(KGLOG_ERR, "ProtocolID: %d", pPendHeader->byProtocolID);
                KG_COM_RELEASE(piPendBuffer);
            }

            goto Exit0;
        }

        piBuffer->AddRef();
        m_SendBufferList.push_back(piBuffer);
    }

	nResult = true;
Exit0:
    if (!nResult)
        m_bSendErrorFlag = true;

    KG_COM_RELEASE(piBuffer);
	return nResult;
}

BOOL KPlayerClient::RealSend(IKG_Buffer* piBuffer)
{
    int nResult     = false;
    int nRetCode    = false;

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    nRetCode = m_piSocketStream->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_nFrameSendCount++;

    nResult = true;
Exit0:
    if (!nResult)
        m_bSendErrorFlag = true;

    return nResult;
}

BOOL KPlayerClient::DoHandshakeRequest(DWORD dwRoleID, const GUID& crGuid)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	C2S_HANDSHAKE_REQUEST   Handshake;

	Handshake.byProtocolID    = c2s_handshake_request;
    Handshake.dwRoleID        = dwRoleID;
	Handshake.Guid            = crGuid;

	bRetCode = Send(&Handshake, sizeof(Handshake));
	KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_DEBUG, "Apply player data: %u\n", dwRoleID);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoApplyEnterScene()
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
	C2S_APPLY_ENTER_SCENE   ApplyEnterScene;

    pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

	ApplyEnterScene.byProtocolID    = c2s_apply_enter_scene;
    ApplyEnterScene.nSceneTimeStamp = pScene->m_nTimeStamp;

	bRetCode = Send(&ApplyEnterScene, sizeof(ApplyEnterScene));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoPlayerLogout(void)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	C2S_PLAYER_LOGOUT   PlayerLogout;

	PlayerLogout.byProtocolID = c2s_player_logout;

	bRetCode = Send(&PlayerLogout, sizeof(PlayerLogout));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoPingSignal()
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
    DWORD               dwCurrentTime   = KG_GetTickCount(); // 必须实时取最新当前时间
    C2S_PING_SIGNAL     Ping;    

    KG_PROCESS_ERROR(dwCurrentTime > m_dwNextPingTime);

    m_dwNextPingTime = dwCurrentTime + 3000;

    Ping.byProtocolID   = c2s_ping_signal;
    Ping.dwTime         = dwCurrentTime;

    bRetCode = Send(&Ping, sizeof(Ping));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoMoveCtrl(KPlayer* pPlayer, BOOL bMove, BOOL bWalk, int nDirection, int nTurn)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
    C2S_MOVE_CTRL       MoveCtrl;

    MoveCtrl.byProtocolID       = c2s_move_ctrl;
    MoveCtrl.bMove              = (bool)bMove;
    MoveCtrl.bWalk              = (bool)bWalk;
    MoveCtrl.byDirection8       = (BYTE)nDirection;
    MoveCtrl.nTurn              = (int8_t)nTurn;
    MoveCtrl.nMoveCount         = ++pPlayer->m_MoveCtrl.nMoveCount;
    MoveCtrl.Param.byMoveState  = (BYTE)pPlayer->m_eMoveState;
    MoveCtrl.Param.nGravity     = pPlayer->m_nCurrentGravity;
    MoveCtrl.Param.nJumpSpeed   = pPlayer->m_nJumpSpeed;
    MoveCtrl.Param.nRunSpeed    = pPlayer->m_nRunSpeed;
    MoveCtrl.Param.nX           = pPlayer->m_nX;
    MoveCtrl.Param.nY           = pPlayer->m_nY;
    MoveCtrl.Param.nZ           = pPlayer->m_nZ;

	bRetCode = Send(&MoveCtrl, sizeof(MoveCtrl));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoApplyMoveAdjust()
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_APPLY_MOVE_ADJUST   Pak;

    Pak.byProtocolID = c2s_apply_move_adjust;

	bRetCode = Send(&Pak, sizeof(Pak));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCharacterStand(KPlayer* pPlayer, int nCurrentState)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	C2S_CHARACTER_STAND CharacterStand;

    assert(pPlayer->m_nFaceDirection >= 0 && pPlayer->m_nFaceDirection <= UCHAR_MAX);

	CharacterStand.byProtocolID       = c2s_character_stand;
    CharacterStand.nMoveCount         = ++pPlayer->m_MoveCtrl.nMoveCount;
    CharacterStand.Param.byMoveState  = (BYTE)nCurrentState;
    CharacterStand.Param.nGravity     = pPlayer->m_nCurrentGravity;
    CharacterStand.Param.nJumpSpeed   = pPlayer->m_nJumpSpeed;
    CharacterStand.Param.nRunSpeed    = pPlayer->m_nRunSpeed;
    CharacterStand.Param.nX           = pPlayer->m_nX;
    CharacterStand.Param.nY           = pPlayer->m_nY;
    CharacterStand.Param.nZ           = pPlayer->m_nZ;
	
	bRetCode = Send(&CharacterStand, sizeof(CharacterStand));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

// 角色跳跃
BOOL KPlayerClient::DoCharacterJump(KPlayer* pPlayer, int nCurrentState, BOOL bStandJump, int nDirectionXY)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
	C2S_CHARACTER_JUMP  CharacterJump;

    assert(nDirectionXY >= 0 && nDirectionXY <= UCHAR_MAX);
    assert(pPlayer->m_nFaceDirection >= 0 && pPlayer->m_nFaceDirection <= UCHAR_MAX);

	CharacterJump.byProtocolID       = c2s_character_jump;
    CharacterJump.bStandJump         = (bool)bStandJump;
    CharacterJump.byJumpDirection    = (BYTE)nDirectionXY;
    CharacterJump.byFaceDirection    = (BYTE)pPlayer->m_nFaceDirection;
    CharacterJump.nMoveCount         = ++pPlayer->m_MoveCtrl.nMoveCount;
    CharacterJump.Param.byMoveState  = (BYTE)nCurrentState;
    CharacterJump.Param.nGravity     = pPlayer->m_nCurrentGravity;
    CharacterJump.Param.nJumpSpeed   = pPlayer->m_nJumpSpeed;
    CharacterJump.Param.nRunSpeed    = pPlayer->m_nRunSpeed;
    CharacterJump.Param.nX           = pPlayer->m_nX;
    CharacterJump.Param.nY           = pPlayer->m_nY;
    CharacterJump.Param.nZ           = pPlayer->m_nZ;

	bRetCode = Send(&CharacterJump, sizeof(CharacterJump));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCharacterSit(KPlayer* pPlayer, int nCurrentState)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
	C2S_CHARACTER_SIT   CharacterSit;

    assert(pPlayer->m_nFaceDirection >= 0 && pPlayer->m_nFaceDirection <= UCHAR_MAX);

	CharacterSit.byProtocolID       = c2s_character_sit;
    CharacterSit.nMoveCount         = ++pPlayer->m_MoveCtrl.nMoveCount;
    CharacterSit.Param.byMoveState  = (BYTE)nCurrentState;
    CharacterSit.Param.nGravity     = pPlayer->m_nCurrentGravity;
    CharacterSit.Param.nJumpSpeed   = pPlayer->m_nJumpSpeed;
    CharacterSit.Param.nRunSpeed    = pPlayer->m_nRunSpeed;
    CharacterSit.Param.nX           = pPlayer->m_nX;
    CharacterSit.Param.nY           = pPlayer->m_nY;
    CharacterSit.Param.nZ           = pPlayer->m_nZ;

	bRetCode = Send(&CharacterSit, sizeof(CharacterSit));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 角色朝向
BOOL KPlayerClient::DoSyncDirection(KPlayer* pPlayer)
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
	C2S_SYNC_DIRECTION  Pak;

	Pak.byProtocolID       = c2s_sync_direction;
	Pak.byFaceDirection    = (BYTE)pPlayer->m_nFaceDirection;
    Pak.byPitchDirection   = (BYTE)pPlayer->m_nPitchDirection;
    Pak.nMoveCount         = ++pPlayer->m_MoveCtrl.nMoveCount;
    Pak.Param.byMoveState  = (BYTE)pPlayer->m_eMoveState;
    Pak.Param.nGravity     = pPlayer->m_nCurrentGravity;
    Pak.Param.nJumpSpeed   = pPlayer->m_nJumpSpeed;
    Pak.Param.nRunSpeed    = pPlayer->m_nRunSpeed;
    Pak.Param.nX           = pPlayer->m_nX;
    Pak.Param.nY           = pPlayer->m_nY;
    Pak.Param.nZ           = pPlayer->m_nZ;

	bRetCode = Send(&Pak, sizeof(Pak));
	KG_PROCESS_ERROR(bRetCode);

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[move] turn, count=%d,frame=%d<%d,%d,%d,%d>,face=%d", 
    //    pPlayer->m_MoveCtrl.nMoveCount, g_pSO3World->m_nGameLoop, pPlayer->m_eMoveState, 
    //    pPlayer->m_nX, pPlayer->m_nY, pPlayer->m_nZ, pPlayer->m_nFaceDirection
    //);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoStopCurrentAction()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    C2S_STOP_CURRENT_ACTION StopCurrentAction;
    
    StopCurrentAction.byProtocolID = c2s_stop_current_action;

    bRetCode = Send(&StopCurrentAction, sizeof(StopCurrentAction));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCharacterSkill(DWORD dwSkillID, DWORD dwSkillLevel, int nFaceDirection, KTarget* pTarget)
{
	BOOL                bRetCode        = false;
	TARGET_TYPE         eTargetType     = ttInvalid;
    C2S_CHARACTER_SKILL CharacterSkill;

	assert(nFaceDirection >= 0 && nFaceDirection < DIRECTION_COUNT);
    assert(nFaceDirection >= 0 && nFaceDirection <= UCHAR_MAX); // byFaceDirection

	assert(pTarget);

	CharacterSkill.byProtocolID     = c2s_character_skill;
	CharacterSkill.dwSkillID        = dwSkillID;
	CharacterSkill.bySkillLevel     = (BYTE)dwSkillLevel;

    eTargetType = pTarget->GetTargetType();
	switch(eTargetType)
	{
	case ttCoordination:
		{
			CharacterSkill.byTargetType = (BYTE)eTargetType;
            pTarget->GetTarget(&CharacterSkill.nParam1, &CharacterSkill.nParam2, &CharacterSkill.nParam3);
		}
		break;
	case ttPlayer:
	case ttNpc:
	case ttDoodad:
		{
            KSceneObject* pSceneObject = NULL;

            bRetCode = pTarget->GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);

			CharacterSkill.byTargetType = (BYTE)eTargetType;
			CharacterSkill.nParam1 = (int)pSceneObject->m_dwID;
			CharacterSkill.nParam2 = 0;
		}
		break;
	}

	bRetCode = Send(&CharacterSkill, sizeof(CharacterSkill));
	KG_PROCESS_ERROR(bRetCode);

	return true;

Exit0:

	return false;
}

//角色交互动作
BOOL KPlayerClient::DoCharacterAction(DWORD dwCharacterID, DWORD dwAction)
{
	BOOL bRetCode = false;

	C2S_CHARACTER_ACTION CharacterAction;

	CharacterAction.byProtocolID = c2s_character_action;
	CharacterAction.dwCharacterID = dwCharacterID;
	CharacterAction.dwAction = dwAction;

	bRetCode = Send(&CharacterAction, sizeof(CharacterAction));
	KG_PROCESS_ERROR(bRetCode);

	return true;

Exit0:

	return false;
}

BOOL KPlayerClient::DoApplyPlayerDisplayData(KPlayer* pPlayer, DWORD dwCheckSum)
{
    BOOL                            bResult     = false;
	BOOL                            bRetCode    = false;	
	C2S_APPLY_PLAYER_DISPLAY_DATA   ApplyPlayerDisplayData;

	ApplyPlayerDisplayData.byProtocolID = c2s_apply_player_display_data;
	ApplyPlayerDisplayData.dwPlayerID   = pPlayer->m_dwID;
	ApplyPlayerDisplayData.dwCheckSum   = dwCheckSum;

	bRetCode = Send(&ApplyPlayerDisplayData, sizeof(ApplyPlayerDisplayData));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoMountKungfu(DWORD dwID, DWORD dwLevel)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    C2S_MOUNT_KUNGFU    Pak;

    Pak.byProtocolID = c2s_mount_kungfu;
    Pak.dwID         = dwID;
    Pak.dwLevel      = dwLevel;

    bRetCode = Send(&Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCancelBuff(int nIndex)
{
    BOOL                bResult = false;
    C2S_CANCEL_BUFF     Pak;

    Pak.byProtocolID = c2s_cancel_buff;
    Pak.nIndex       = nIndex;

    Send(&Pak, sizeof(Pak));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetSheathState(BOOL bSheathFlag)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_SET_SHEATH_STATE    Pak;

    Pak.byProtocolID = c2s_set_sheath_state;
    Pak.bSheathFlag  = (bool)bSheathFlag;

    bRetCode = Send(&Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetHideHatFlag(BOOL bHideHat)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_SET_HIDE_HAT_FLAG   Pak;

    Pak.byProtocolID    = c2s_set_hide_hat_flag;
    Pak.bHideHat        = (bool)bHideHat;

    bRetCode = Send(&Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyPlayerBuffList(DWORD dwPlayerID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    C2S_APPLY_PLAYER_BUFFLIST   Pak;

    KGLOG_PROCESS_ERROR(dwPlayerID);

    Pak.byProtocolID = c2s_apply_player_bufflist;
    Pak.dwPlayerID  = dwPlayerID;

    bRetCode = Send(&Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


BOOL KPlayerClient::DoInviteJoinTeamRequest(const char cszInvitePlayer[])
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    KPlayer*                        pClientPlayer   = NULL;
    C2S_INVITE_JOIN_TEAM_REQUEST    TeamInvitation;
    KUIEventTeamMessageNotify      UIEvent;
    
    assert(cszInvitePlayer);

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    bRetCode = strcmp(pClientPlayer->m_szName, cszInvitePlayer);
    KG_PROCESS_ERROR(bRetCode);

    TeamInvitation.byProtocolID = c2s_invite_join_team_request;
    
    strncpy(TeamInvitation.szInvitedPlayer, cszInvitePlayer, sizeof(TeamInvitation.szInvitedPlayer));
    TeamInvitation.szInvitedPlayer[sizeof(TeamInvitation.szInvitedPlayer) - 1] = '\0';

    bRetCode = Send(&TeamInvitation, sizeof(TeamInvitation));
    KG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        UIEvent.nCode = epncInvitationDone;
        strncpy(UIEvent.szName, cszInvitePlayer, sizeof(UIEvent.szName));
        UIEvent.szName[sizeof(UIEvent.szName) - 1] = '\0';

        g_pGameWorldUIHandler->OnTeamMessageNotify(UIEvent);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoInviteJoinTeamRespond(const char cszInviteSrc[], BOOL bAgreeFlag)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_INVITE_JOIN_TEAM_RESPOND    Respond;

    Respond.byProtocolID = c2s_invite_join_team_respond;
    Respond.bAgreeFlag   = bAgreeFlag;

    strncpy(Respond.szInviteSrc, cszInviteSrc, sizeof(Respond.szInviteSrc));
    Respond.szInviteSrc[sizeof(Respond.szInviteSrc) - 1] = '\0';

    bRetCode = Send(&Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    if (!bAgreeFlag)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTeamMessageNotify      UIEvent;

            UIEvent.nCode = epncInvitationReject;
            strncpy(UIEvent.szName, cszInviteSrc, sizeof(UIEvent.szName));
            UIEvent.szName[sizeof(UIEvent.szName) - 1] = '\0';

            g_pGameWorldUIHandler->OnTeamMessageNotify(UIEvent);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyJoinTeamRequest(const char cszApplyDst[])
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    KPlayer*                        pClientPlayer   = NULL;
    C2S_APPLY_JOIN_TEAM_REQUEST     TeamApply;
    KUIEventTeamMessageNotify      UIEvent;
    
    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    bRetCode = strcmp(pClientPlayer->m_szName, cszApplyDst);
    KG_PROCESS_ERROR(bRetCode);

    TeamApply.byProtocolID = c2s_apply_join_team_request;

    strncpy(TeamApply.szApplyDst, cszApplyDst, sizeof(TeamApply.szApplyDst));
    TeamApply.szApplyDst[sizeof(TeamApply.szApplyDst) - 1] = '\0';

    bRetCode = Send(&TeamApply, sizeof(TeamApply));
    KG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        UIEvent.nCode = epncApplicationDone;
        strncpy(UIEvent.szName, cszApplyDst, sizeof(UIEvent.szName));
        UIEvent.szName[sizeof(UIEvent.szName) - 1] = '\0';

        g_pGameWorldUIHandler->OnTeamMessageNotify(UIEvent);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyJoinTeamRespond(const char cszApplySrc[], BOOL bAgreeFlag)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_APPLY_JOIN_TEAM_RESPOND     Respond;

    Respond.byProtocolID = c2s_apply_join_team_respond;
    Respond.bAgreeFlag   = bAgreeFlag;

    strncpy(Respond.szApplySrc, cszApplySrc, sizeof(Respond.szApplySrc));
    Respond.szApplySrc[sizeof(Respond.szApplySrc) - 1] = '\0';

    bRetCode = Send(&Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    if (!bAgreeFlag)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTeamMessageNotify UIEvent;

            UIEvent.nCode = epncApplicationReject;
            strncpy(UIEvent.szName, cszApplySrc, sizeof(UIEvent.szName));
            UIEvent.szName[sizeof(UIEvent.szName) - 1] = '\0';

            g_pGameWorldUIHandler->OnTeamMessageNotify(UIEvent);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoLeaveTeamRequest()
{
    BOOL                            bResult                 = false;
    int                             nRetCode                = false;
    KPlayer*                        pSelf                   = NULL;
    C2S_TEAM_DEL_MEMBER_REQUEST    TeamDelMemberRequest;

    pSelf   = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    KG_PROCESS_ERROR(!g_pSO3World->m_TeamClient.m_bSystem); // System Team不能退出

    KGLOG_PROCESS_ERROR(pSelf->m_dwTeamID != ERROR_ID);

    TeamDelMemberRequest.byProtocolID = c2s_team_del_member_request;
    TeamDelMemberRequest.dwMemberID   = g_pSO3World->m_dwClientPlayerID;

    nRetCode = Send(&TeamDelMemberRequest, sizeof(TeamDelMemberRequest));
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}
 
BOOL KPlayerClient::DoKickoutTeamMember(const char cszRoleName[])
{
    BOOL                            bResult                 = false;
    int                             nRetCode                = false;
    KPlayer*                        pSelf                   = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo             = NULL;
    C2S_TEAM_DEL_MEMBER_REQUEST     TeamDelMemberRequest;

    KG_ASSERT_EXIT(cszRoleName);

    KG_PROCESS_ERROR(!g_pSO3World->m_TeamClient.m_bSystem); // SystemTeam不能踢人

    pSelf   = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    nRetCode = STR_CASE_CMP(pSelf->m_szName, cszRoleName);
    // 不能踢出自己
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(pSelf->m_dwTeamID != ERROR_ID);

    // 自己必须是队长
    KGLOG_PROCESS_ERROR(g_pSO3World->m_dwClientPlayerID == g_pSO3World->m_TeamClient.m_dwAuthority[tatLeader]);

    // 离队玩家确实在队伍中
    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfoByName(cszRoleName);
    KG_PROCESS_ERROR(pMemberInfo);

    TeamDelMemberRequest.byProtocolID = c2s_team_del_member_request;
    TeamDelMemberRequest.dwMemberID   = pMemberInfo->dwMemberID;

    nRetCode = Send(&TeamDelMemberRequest, sizeof(TeamDelMemberRequest));
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTeamChangeAuthorityRequest(KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    KPlayer*                            pSelf           = NULL;
    DWORD                               dwTeamID        = ERROR_ID;
    KTEAM_MEMBER_INFO*                  pMemberInfo     = NULL;
    C2S_TEAM_CHANGE_AUTHORITY_REQUEST   ChangeAuthority;

    KGLOG_PROCESS_ERROR(eType > tatInvalid && eType < tatTotal);

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);
    
    dwTeamID = pSelf->GetCurrentTeamID();
    KGLOG_PROCESS_ERROR(dwTeamID != ERROR_ID);

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(dwTargetID);
    KG_PROCESS_ERROR(pMemberInfo);
    KG_PROCESS_ERROR(pMemberInfo->bOnlineFlag);

    ChangeAuthority.byProtocolID    = c2s_team_change_authority_request;
    ChangeAuthority.byType          = (BYTE)eType;
    ChangeAuthority.dwNewLeaderID   = pMemberInfo->dwMemberID;
    
    bRetCode = Send(&ChangeAuthority, sizeof(ChangeAuthority));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTeamSetLootMode(int nLootMode, KPlayer* pPlayer)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    C2S_TEAM_SET_LOOT_MODE_REQUEST      Request;

    Request.byProtocolID    = c2s_team_set_distribute_mode_request;
    Request.nLootMode		= nLootMode;

    KG_PROCESS_ERROR(!g_pSO3World->m_TeamClient.m_bSystem); // System Team不能更改LootMode

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    KGLOG_PROCESS_ERROR(pPlayer->m_dwID == g_pSO3World->m_TeamClient.m_dwAuthority[tatDistribute]);

	KG_PROCESS_ERROR(g_pSO3World->m_TeamClient.m_nLootMode != nLootMode);

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;    
}

BOOL KPlayerClient::DoTeamSetRollQuality(int nRollQuality, KPlayer* pPlayer)
{
    BOOL                                bResult     = false;
	BOOL	                            bRetCode    = false;
	C2S_TEAM_SET_ROLL_QUALITY_REQUEST   Request;

	Request.byProtocolID = c2s_team_set_roll_quality_request;
	Request.nRollQuality = nRollQuality;

    KG_PROCESS_ERROR(!g_pSO3World->m_TeamClient.m_bSystem); // System Team无法更改

	KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    bRetCode = (pPlayer->m_dwID == g_pSO3World->m_TeamClient.m_dwAuthority[tatDistribute]);
    KGLOG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(g_pSO3World->m_TeamClient.m_nRollQuality != nRollQuality);

	bRetCode = Send(&Request, sizeof(Request));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult; 	
}

BOOL KPlayerClient::DoTeamSetFormationLeader(DWORD dwNewFormationLeaderID, int nGroupIndex, KPlayer* pPlayer)
{
    BOOL                                    bResult             = false;
	BOOL	                                bRetCode            = false;
    DWORD                                   dwTeamID            = ERROR_ID;
    int                                     nPlayerGroupIndex   = 0;
    C2S_TEAM_SET_FORMATION_LEADER_REQUEST   Request;

    assert(pPlayer);

    dwTeamID = pPlayer->GetCurrentTeamID();

	KGLOG_PROCESS_ERROR(dwTeamID != ERROR_ID);
    KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < MAX_TEAM_GROUP_NUM);

    nPlayerGroupIndex = pPlayer->GetCurrentTeamGroupIndex();
    KGLOG_PROCESS_ERROR(nPlayerGroupIndex >= 0 && nPlayerGroupIndex < MAX_TEAM_GROUP_NUM);

    KG_PROCESS_ERROR(
        pPlayer->m_dwID == g_pSO3World->m_TeamClient.m_TeamGroup[nPlayerGroupIndex].dwFormationLeader ||
        pPlayer->m_dwID == g_pSO3World->m_TeamClient.m_dwAuthority[tatLeader]
    );

    Request.byProtocolID            = c2s_team_set_formation_leader_request;
    Request.dwNewFormationLeaderID  = dwNewFormationLeaderID;
    Request.nGroupIndex             = nGroupIndex;

    bRetCode = Send(&Request, sizeof(Request));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoTeamMemberNotifySignpost(int nX, int nY)
{
    BOOL                                    bResult  = false;
    BOOL                                    bRetCode = false;
    DWORD                                   dwTeamID = ERROR_ID;
    KPlayer*                                pSelf    = NULL;
    C2S_TEAM_MEMBER_NOTIFY_SIGNPOST_REQUEST Request;

    pSelf   = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_ASSERT_EXIT(pSelf);

    dwTeamID = pSelf->GetCurrentTeamID();
    KG_PROCESS_SUCCESS(dwTeamID == ERROR_ID);

    Request.byProtocolID = c2s_team_member_notify_signpost_request;
    Request.nX           = nX;
    Request.nY           = nY;

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult; 
}

BOOL KPlayerClient::DoTeamSetMarkRequest(int nMarkType, DWORD dwTargetID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    DWORD                       dwTeamID    = ERROR_ID;
    KPlayer*                    pSelf       = NULL;
    C2S_TEAM_SET_MARK_REQUEST   Request;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    dwTeamID = pSelf->GetCurrentTeamID();
    KG_PROCESS_SUCCESS(dwTeamID == ERROR_ID);

    Request.byProtocolID = c2s_team_set_mark_request;
    Request.nMarkType    = nMarkType;
    Request.dwTargetID   = dwTargetID;

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTeamLevelUpRaidRequest()
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_TEAM_LEVEL_UP_RAID_REQUEST  Request;

    Request.byProtocolID = c2s_team_level_up_raid_request;

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTeamChangeMemberGroupRequest(DWORD dwMemberID, int nDstGroupIndex, DWORD dwDstMemberID)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    C2S_TEAM_CHANGE_MEMBER_GROUP_REQUEST    Request;

    Request.byProtocolID    = c2s_team_change_member_group_request;
    Request.dwMemberID      = dwMemberID;
    Request.nDstGroupIndex  = nDstGroupIndex;
    Request.dwDstMemberID   = dwDstMemberID;

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 接受任务
BOOL KPlayerClient::DoAcceptQuest(KTarget* pTarget, DWORD dwQuestID)
{
	BOOL                bRetCode      = false;
    KSceneObject*       pSceneObject  = NULL;
	KItem*				pItem		  = NULL;
	TARGET_TYPE			eTargetType	  = ttInvalid;
	C2S_ACCEPT_QUEST    AcceptQuest;

	KGLOG_PROCESS_ERROR(pTarget);

	eTargetType = pTarget->GetTargetType();
    
	AcceptQuest.byProtocolID = c2s_accept_quest;
	AcceptQuest.dwQuestID = dwQuestID;
	AcceptQuest.byTargetType = (BYTE)eTargetType;
	if (eTargetType == ttNoTarget)
	{
		AcceptQuest.dwTargetID = 0;
	}
	else if (eTargetType == ttItem)
	{
		bRetCode = pTarget->GetTarget(&pItem);
		KGLOG_PROCESS_ERROR(bRetCode && pItem);

		AcceptQuest.dwTargetID = pItem->m_dwID;
	}
	else
	{
		bRetCode = pTarget->GetTarget(&pSceneObject);
		KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);

		AcceptQuest.dwTargetID = pSceneObject->m_dwID;
	}

	bRetCode = Send(&AcceptQuest, sizeof(AcceptQuest));
	KG_PROCESS_ERROR(bRetCode);

	return true;

Exit0:

	return false;
}

//完成任务
BOOL KPlayerClient::DoFinishQuest(DWORD dwQuestID, KTarget& rTarget, int nPresentChoice1, int nPresentChoice2)
{
	BOOL             bRetCode        = false;
    KSceneObject*    pSceneObject    = NULL;
	C2S_FINISH_QUEST FinishQuest;

    bRetCode = rTarget.GetTarget(&pSceneObject);
    KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);

	FinishQuest.byProtocolID = c2s_finish_quest;
	FinishQuest.dwQuestID = dwQuestID;
	FinishQuest.dwTargetType = (DWORD)rTarget.GetTargetType();
	FinishQuest.dwTargetID = pSceneObject->m_dwID;
	FinishQuest.nPresentChoice1 = nPresentChoice1;
	FinishQuest.nPresentChoice2 = nPresentChoice2;

	bRetCode = Send(&FinishQuest, sizeof(FinishQuest));
	KG_PROCESS_ERROR(bRetCode);

	return true;

Exit0:

	return false;
}

//取消任务
BOOL KPlayerClient::DoCancelQuest(int nQuestIndex)
{
	BOOL bRetCode = false;

	C2S_CANCEL_QUEST CancelQuest;

	CancelQuest.byProtocolID = c2s_cancel_quest;
	CancelQuest.nQuestIndex = nQuestIndex;

	bRetCode = Send(&CancelQuest, sizeof(CancelQuest));
	KG_PROCESS_ERROR(bRetCode);

	return true;

Exit0:

	return false;
}

// 共享任务
BOOL KPlayerClient::DoShareQuest(int nQuestIndex)
{
    BOOL            bResult     = false;
	BOOL            bRetCode    = false;
	C2S_SHARE_QUEST ShareQuest;

	ShareQuest.byProtocolID = c2s_share_quest;
	ShareQuest.nQuestIndex = nQuestIndex;

	bRetCode = Send(&ShareQuest, sizeof(ShareQuest));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoAcceptEscortQuest(DWORD dwStartPlayerID, DWORD dwQuestID)
{
    BOOL                        bResult         = false;
	BOOL                        bRetCode        = false;
    C2S_ACCEPT_ESCORT_QUEST     AcceptEscortQuest;
	
	AcceptEscortQuest.byProtocolID    = c2s_accept_escort_quest;
	AcceptEscortQuest.dwStartPlayerID = dwStartPlayerID;
	AcceptEscortQuest.dwQuestID       = dwQuestID;
	
	bRetCode = Send(&AcceptEscortQuest, sizeof(AcceptEscortQuest));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoGmCommand(const char cszCommand[])
{
    BOOL            bResult     = false;
	BOOL            bRetCode    = false;
    IKG_Buffer*     piBuffer    = NULL;
	C2S_GM_COMMAND* pGmCommand  = NULL;
    size_t          uPakSize    = sizeof(C2S_GM_COMMAND) + strlen(cszCommand) + 1;

    KGLOG_PROCESS_ERROR(uPakSize < MAX_EXTERNAL_PACKAGE_SIZE);

    piBuffer = KG_MemoryCreateBuffer((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pGmCommand  = (C2S_GM_COMMAND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pGmCommand);

	pGmCommand->byProtocolID    = c2s_gm_command;

	strcpy(pGmCommand->szGmCommand, cszCommand);

	bRetCode = Send(pGmCommand, uPakSize);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
	return bResult;
}

BOOL KPlayerClient::DoOpenShopRequest(DWORD dwShopID)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_OPEN_SHOP_REQUEST   Request;
    
    Request.byProtocolID = c2s_open_shop_request;
    Request.dwShopID = dwShopID;
    
    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoQueryShopItems(DWORD dwShopID, DWORD dwPageIndex)
{
    BOOL                 bResult  = false;
    BOOL                 bRetCode = false;
    const KShop*         pcShop   = NULL;
    C2S_QUERY_SHOP_ITEMS Request;

    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);
    KGLOG_PROCESS_ERROR(dwPageIndex < pcShop->m_dwValidPage);

    Request.byProtocolID = c2s_query_shop_items;
    Request.dwShopID     = dwShopID;
    Request.byPageIndex  = (BYTE)dwPageIndex;

    for (int nPosIndex = 0; nPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nPosIndex)
    {
        Request.ShopPageItems[nPosIndex].dwItemID = 0;
        if (pcShop->m_ShopPages[dwPageIndex].ShopItems[nPosIndex].pItem)
        {
            Request.ShopPageItems[nPosIndex].dwItemID = pcShop->m_ShopPages[dwPageIndex].ShopItems[nPosIndex].pItem->m_dwID;
        }

        Request.ShopPageItems[nPosIndex].nCount     = pcShop->m_ShopPages[dwPageIndex].ShopItems[nPosIndex].nCount;
    }

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoShopBuyRequest(
    DWORD dwShopID, DWORD dwPageIndex, DWORD dwPosIndex, DWORD dwItemID, int nCount, int nCost
)
{
    BOOL                  bResult       = false;
    BOOL                  bRetCode      = false;
    C2S_SHOP_BUY_REQUEST  Request;

    Request.byProtocolID = c2s_shop_buy_request;
    Request.dwShopID     = dwShopID;
    Request.dwItemID     = dwItemID;
    Request.byPageIndex  = (BYTE)dwPageIndex;
    Request.byPosIndex   = (BYTE)dwPosIndex;
    Request.nCount       = nCount;
    Request.nCost        = nCost;

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoShopSellRequest(
	DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex
)
{
	BOOL 					bResult  = false;
	BOOL 					bRetCode = false;
	C2S_SHOP_SELL_REQUEST	Request;

	Request.byProtocolID = c2s_shop_sell_request;
	Request.dwShopID	 = dwShopID;
	Request.dwItemID	 = dwItemID;
	Request.nCost		 = nCost;
    Request.dwBoxIndex   = dwBoxIndex;
    Request.dwPosIndex   = dwPosIndex;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoShopRepairRequest(
    DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex
)
{
    BOOL 					bResult  = false;
    BOOL 					bRetCode = false;
    C2S_SHOP_REPAIR_REQUEST	Request;

    Request.byProtocolID = c2s_shop_repair_request;
    Request.dwShopID	 = dwShopID;
    Request.dwItemID	 = dwItemID;
    Request.nCost		 = nCost;
    Request.dwBoxIndex   = dwBoxIndex;
    Request.dwPosIndex   = dwPosIndex;


    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSoldListBuyRequest(DWORD dwShopID, DWORD dwX, DWORD dwItemID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    const KShop*                pcShop      = NULL;
    C2S_SOLD_LIST_BUY_REQUEST   Request;

    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KG_ASSERT_EXIT(pcShop);

    Request.byProtocolID    = c2s_sold_list_buy_request;
    Request.dwShopID        = dwShopID;
    Request.dwX             = dwX;
    Request.dwItemID        = dwItemID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingInviteRequest(DWORD dwInviteDst)
{
    BOOL                       bResult  = false;
    BOOL                       bRetCode = false;
    C2S_TRADING_INVITE_REQUEST Request;

    Request.byProtocolID = c2s_trading_invite_request;
    Request.dwInviteDst  = dwInviteDst;

    bRetCode = Send(&Request, sizeof(Request));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;

}

BOOL KPlayerClient::DoTradingInviteRespond(BOOL bAccept)
{
    BOOL                       bResult  = false;
    BOOL                       bRetCode = false;
    C2S_TRADING_INVITE_RESPOND Respond;

    Respond.byProtocolID = c2s_trading_invite_respond;
    Respond.bAccept      = bAccept;

    bRetCode = Send(&Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingAddItem(DWORD dwBoxIndex, DWORD dwPosIndex, DWORD dwGridIndex)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    C2S_TRADING_ADD_ITEM    AddItem;

    AddItem.byProtocolID = c2s_trading_add_item;
    AddItem.byBoxIndex   = (BYTE)dwBoxIndex;
    AddItem.byPosIndex   = (BYTE)dwPosIndex;
    AddItem.byGridIndex  = (BYTE)dwGridIndex;

    bRetCode = Send(&AddItem, sizeof(AddItem));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingDeleteItem(DWORD dwGridIndex)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_TRADING_DELETE_ITEM DeleteItem;
    
    DeleteItem.byProtocolID = c2s_trading_delete_item;
    DeleteItem.byGridIndex  = (BYTE)dwGridIndex;
    
    bRetCode = Send(&DeleteItem, sizeof(DeleteItem));
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingSetMoney(int nMoney)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    C2S_TRADING_SET_MONEY   SetMoney;

    SetMoney.byProtocolID = c2s_trading_set_money;
    SetMoney.nMoney       = nMoney;

    bRetCode = Send(&SetMoney, sizeof(SetMoney));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingConfirm(BOOL bConfirm, int nKey)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    C2S_TRADING_CONFIRM     TradingConfirm;

    TradingConfirm.byProtocolID = c2s_trading_confirm;
    TradingConfirm.bConfirm     = bConfirm;
    TradingConfirm.nKey         = nKey;

    bRetCode = Send(&TradingConfirm, sizeof(TradingConfirm));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRequestTarget(int nTargetType, DWORD dwTargetID)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
    C2S_REQUEST_TARGET  RequestTarget;

    RequestTarget.byProtocolID = c2s_request_target;
    RequestTarget.byTargetType = (BYTE)nTargetType;
    RequestTarget.dwTargetID   = dwTargetID;

    bRetCode = Send(&RequestTarget, sizeof(RequestTarget));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


// 申请交换装备
BOOL KPlayerClient::DoApplyExchangeItem(DWORD dwBox1, DWORD dwX1, DWORD dwBox2, DWORD dwX2, DWORD dwAmount)
{
	BOOL bResult = false;
	BOOL bRetCode = false;

	C2S_APPLY_EXCHANGE_ITEM ApplyExchangeItem;

	ApplyExchangeItem.byProtocolID = c2s_apply_exchange_item;
	ApplyExchangeItem.byBox1 = (BYTE)dwBox1;
	ApplyExchangeItem.byBox2 = (BYTE)dwBox2;
	ApplyExchangeItem.byX1 = (BYTE)dwX1;
	ApplyExchangeItem.byX2 = (BYTE)dwX2;
	ApplyExchangeItem.wAmount = (WORD)dwAmount;

	bRetCode = Send(&ApplyExchangeItem, sizeof(ApplyExchangeItem));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;

Exit0:

	return bResult;
}


// 选择窗口选项
BOOL KPlayerClient::DoWindowSelect(DWORD dwIndex, BYTE bySelect)
{
	BOOL bRetCode = false;

	C2S_WINDOW_SELECT WindowSelect;

	WindowSelect.byProtocolID = c2s_window_select;
	WindowSelect.dwIndex = dwIndex;
	WindowSelect.bySelect = bySelect;

	bRetCode = Send(&WindowSelect, sizeof(WindowSelect));
	KG_PROCESS_ERROR(bRetCode);

	return true;

Exit0:

	return false;
}


// 配方使用协议
BOOL KPlayerClient::DoCastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwRBookItemID, KTarget* pTarget)
{
    BOOL         bResult        = false;
	BOOL         bRetCode		= false;
	TARGET_TYPE  eTargetType	= ttInvalid;
	C2S_CAST_PROFESSION_SKILL CastProfessionSkill;

	KGLOG_PROCESS_ERROR(pTarget);

	CastProfessionSkill.byProtocolID    = c2s_cast_profession_skill;
	CastProfessionSkill.dwCraftID       = dwCraftID;
	CastProfessionSkill.dwRecipeID      = dwRecipeID;
	CastProfessionSkill.dwRBookItemID   = dwRBookItemID;
    
    KGLOG_PROCESS_ERROR(pTarget);
    eTargetType = pTarget->GetTargetType();

	if(pTarget == NULL || eTargetType == ttNoTarget)
	{
		CastProfessionSkill.byTargetType = (BYTE)ttNoTarget;
	}
	else if (eTargetType == ttCoordination)
	{
        int nZ = 0;
		CastProfessionSkill.byTargetType = (BYTE)eTargetType;
        bRetCode = pTarget->GetTarget(&CastProfessionSkill.nParam1, &CastProfessionSkill.nParam2, &nZ);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	else if ((eTargetType == ttPlayer) || (eTargetType == ttNpc) || (eTargetType == ttDoodad))
	{
        KSceneObject* pSceneObject = NULL;

        bRetCode = pTarget->GetTarget(&pSceneObject);
        KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);

		CastProfessionSkill.byTargetType = (BYTE)eTargetType;
		CastProfessionSkill.nParam1 = (int)pSceneObject->m_dwID;
		CastProfessionSkill.nParam2 = 0;
	}
	else if (eTargetType == ttItem)
	{
		KItem* pItem = NULL;

		bRetCode = pTarget->GetTarget(&pItem);
		KGLOG_PROCESS_ERROR(bRetCode && pItem);

		CastProfessionSkill.byTargetType = (BYTE)eTargetType;
		CastProfessionSkill.nParam1 = (int)pItem->m_dwID;
		CastProfessionSkill.nParam2 = 0;
	}

	bRetCode = Send(&CastProfessionSkill, sizeof(CastProfessionSkill));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoLootItem(DWORD dwDoodadID, DWORD dwItemID)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	C2S_LOOT_ITEM           LootItem;

	LootItem.byProtocolID = c2s_loot_item;
	LootItem.dwDoodadID   = dwDoodadID;
	LootItem.dwItemID     = dwItemID;

	bRetCode = Send(&LootItem, sizeof(LootItem));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRollItem(DWORD dwDoodadID, DWORD dwItemID, BOOL bCancel)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	C2S_ROLL_ITEM           RollItem;

	RollItem.byProtocolID = c2s_roll_item;
	RollItem.dwDoodadID   = dwDoodadID;
	RollItem.dwItemID     = dwItemID;
	RollItem.bCancel      = bCancel;

	bRetCode = Send(&RollItem, sizeof(RollItem));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyLootList(DWORD dwDoodadID)
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    C2S_APPLY_LOOT_LIST     ApplyLootList;

	ApplyLootList.byProtocolID  = c2s_apply_loot_list;
	ApplyLootList.dwDoodadID    = dwDoodadID;

	bRetCode = Send(&ApplyLootList, sizeof(ApplyLootList));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCharacterOpen(DWORD dwDoodadID)
{
	BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
	C2S_CHARACTER_OPEN      CharacterOpen;

	CharacterOpen.byProtocolID  = c2s_character_open;
	CharacterOpen.dwDoodadID    = dwDoodadID;

	bRetCode = Send(&CharacterOpen, sizeof(CharacterOpen));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyUseItem(DWORD dwBox, DWORD dwX, KTarget& rTarget)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    TARGET_TYPE             eTargetType     = ttInvalid;
	C2S_APPLY_USE_ITEM      ApplyUseItem;
    
    ZeroMemory(&ApplyUseItem, sizeof(ApplyUseItem));

	ApplyUseItem.byProtocolID   = c2s_apply_use_item;
	ApplyUseItem.byBox          = (BYTE)dwBox;
	ApplyUseItem.byX            = (BYTE)dwX;

    eTargetType = rTarget.GetTargetType();
    
    switch(eTargetType)
    {
    case ttNoTarget:
        ApplyUseItem.byTargetType = (BYTE)eTargetType;
    	break;

    case ttCoordination:
        {
            ApplyUseItem.byTargetType = (BYTE)eTargetType;
            bRetCode = rTarget.GetTarget(&ApplyUseItem.nParam1, &ApplyUseItem.nParam2, &ApplyUseItem.nParam3);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    	break;

    case ttPlayer:
    case ttNpc:
    case ttDoodad:
        {
            KSceneObject* pSceneObject = NULL;
            bRetCode = rTarget.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);

	        ApplyUseItem.byTargetType = (BYTE)eTargetType;
	        ApplyUseItem.nParam1 = (int)pSceneObject->m_dwID;
	        ApplyUseItem.nParam2 = 0;
	        ApplyUseItem.nParam3 = 0;
        }
        break;

    case ttItemPos:
        {
		    KPlayer* pPlayer = NULL;
		    DWORD dwTargetItemBox = 0;
		    DWORD dwTargetItemX = 0;

		    bRetCode = rTarget.GetTarget(&pPlayer, &dwTargetItemBox, &dwTargetItemX);
		    KGLOG_PROCESS_ERROR(pPlayer);

		    ApplyUseItem.byTargetType = (BYTE)ttItemPos;
		    ApplyUseItem.nParam1 = pPlayer->m_dwID;
		    ApplyUseItem.nParam2 = dwTargetItemBox;
		    ApplyUseItem.nParam3 = dwTargetItemX;
	    }
        break;

    default:
        KGLOG_PROCESS_ERROR(false);
        break;
    }

	bRetCode = Send(&ApplyUseItem, sizeof(ApplyUseItem));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyFightFlag(DWORD dwForceID, BOOL bFight)
{
    BOOL                 bResult        = false;
	BOOL                 bRetCode       = false;
	C2S_APPLY_FIGHT_FLAG ApplyFightFlag;

	ApplyFightFlag.byProtocolID = c2s_apply_fight_flag;
	ApplyFightFlag.byForceID    = (BYTE)dwForceID;
	ApplyFightFlag.byOpen       = (BYTE)bFight;

	bRetCode = Send(&ApplyFightFlag, sizeof(ApplyFightFlag));
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoFinishLoot(DWORD dwDoodadID)
{
	BOOL                bResult        = false;
	BOOL                bRetCode       = false;
	C2S_FINISH_LOOT     FinishLoot;

	FinishLoot.byProtocolID = c2s_finish_loot;
	FinishLoot.dwDoodadID   = dwDoodadID;

	bRetCode = Send(&FinishLoot, sizeof(FinishLoot));
	KG_PROCESS_ERROR(bRetCode);
	
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoLootMoney(DWORD dwDoodadID)
{
	BOOL            bResult        = false;
	BOOL            bRetCode       = false;
	C2S_LOOT_MONEY  LootMoney;

	LootMoney.byProtocolID = c2s_loot_money;
	LootMoney.dwDoodadID = dwDoodadID;
	
	bRetCode = Send(&LootMoney, sizeof(LootMoney));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoChangePkMode(int nMode)
{
    BOOL                bResult        = false;
	BOOL                bRetCode       = false;
	C2S_CHANGE_PK_MODE  ChangePkMode;

	ChangePkMode.byProtocolID   = c2s_change_pk_mode;
	ChangePkMode.byPKMode       = (BYTE)nMode;
	
	bRetCode = Send(&ChangePkMode, sizeof(ChangePkMode));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoDistributeItem(DWORD dwDoodadID, DWORD dwItemID, DWORD dwDstPlayerID)
{
    BOOL                 bResult        = false;
	BOOL                 bRetCode       = false;
	C2S_DISTRIBUTE_ITEM  DistributeItem;

	DistributeItem.byProtocolID   = c2s_distribute_item;
	DistributeItem.dwDoodadID     = dwDoodadID;
	DistributeItem.dwItemID       = dwItemID;
	DistributeItem.dwDstPlayerID  = dwDstPlayerID;

	bRetCode = Send(&DistributeItem, sizeof(DistributeItem));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTalkMessage(int nMsgType, const char cszReceiver[], size_t uDataLen, BYTE* pbyData)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    IKG_Buffer*         piBuffer    = NULL;
    C2S_TALK_MESSAGE*   pTalkMsg    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(C2S_TALK_MESSAGE) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);
         
    pTalkMsg = (C2S_TALK_MESSAGE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pTalkMsg);

    pTalkMsg->byProtocolID  = c2s_talk_message;
    pTalkMsg->byMsgType     = (BYTE)nMsgType;

    pTalkMsg->szReceiver[0] = '\0';
    if (cszReceiver)
    {
        strncpy(pTalkMsg->szReceiver, cszReceiver, sizeof(pTalkMsg->szReceiver));
        pTalkMsg->szReceiver[sizeof(pTalkMsg->szReceiver) - 1] = '\0';
    }

    memcpy(pTalkMsg->byTalkData, pbyData, uDataLen);

    bRetCode = Send(pTalkMsg, sizeof(C2S_TALK_MESSAGE) + uDataLen);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KPlayerClient::DoAddFellowshipRequest(const char cszAlliedPlayerName[], int nType)
{
    BOOL                        bResult                 = false;
    BOOL                        bRetCode                = false;
    unsigned                    uAlliedPlayerNameLen    = 0;
    C2S_ADD_FELLOWSHIP_REQUEST  AddFellowship;
    KUIEventFellowshipRespond   UIParam;

    KGLOG_PROCESS_ERROR(cszAlliedPlayerName);
    KGLOG_PROCESS_ERROR(nType >= 1);
    KGLOG_PROCESS_ERROR(nType <= 3);

    uAlliedPlayerNameLen = (unsigned)strlen(cszAlliedPlayerName);

    if (uAlliedPlayerNameLen >= _NAME_LEN)
    {
        if (g_pGameWorldUIHandler)
        {
            UIParam.nFellowshipRespond = rrcInvalidName;
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }

    KGLOG_PROCESS_ERROR(uAlliedPlayerNameLen > 0 && uAlliedPlayerNameLen < sizeof(AddFellowship.szAlliedPlayerName));

    AddFellowship.byProtocolID  = c2s_add_fellowship_request;
    AddFellowship.byType        = (BYTE)nType;
    strncpy(AddFellowship.szAlliedPlayerName, cszAlliedPlayerName, sizeof(AddFellowship.szAlliedPlayerName));
    AddFellowship.szAlliedPlayerName[sizeof(AddFellowship.szAlliedPlayerName) - sizeof('\0')] = '\0';

    bRetCode = Send(&AddFellowship, sizeof(C2S_ADD_FELLOWSHIP_REQUEST));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoDelFellowshipRequest(DWORD dwAlliedPlayerID, int nType)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    C2S_DEL_FELLOWSHIP_REQUEST  DelFellowship;

    KGLOG_PROCESS_ERROR(dwAlliedPlayerID > 0);
    KGLOG_PROCESS_ERROR(nType >= 1);
    KGLOG_PROCESS_ERROR(nType <= 3);

    DelFellowship.byProtocolID      = c2s_del_fellowship_request;
    DelFellowship.dwAlliedPlayerID  = dwAlliedPlayerID;
    DelFellowship.byType            = (BYTE)nType;

    bRetCode = Send(&DelFellowship, sizeof(C2S_DEL_FELLOWSHIP_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAddFellowshipGroupRequest(const char cszGroupName[])
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    unsigned                            uGroupNameLen   = 0;
    C2S_ADD_FELLOWSHIP_GROUP_REQUEST    AddGroup;
    KUIEventFellowshipRespond           UIParam;

    KGLOG_PROCESS_ERROR(cszGroupName);
    uGroupNameLen = (unsigned)strlen(cszGroupName);

    if (uGroupNameLen >= sizeof(AddGroup.szGroupName))
    {
        if (g_pGameWorldUIHandler)
        {
            UIParam.nFellowshipRespond = rrcInvalidGroupName;
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }

    KGLOG_PROCESS_ERROR(uGroupNameLen > 0 && uGroupNameLen < sizeof(AddGroup.szGroupName));

    AddGroup.byProtocolID = c2s_add_fellowship_group_request;
    strncpy(AddGroup.szGroupName, cszGroupName, sizeof(AddGroup.szGroupName));
    AddGroup.szGroupName[sizeof(AddGroup.szGroupName) - sizeof('\0')] = '\0';

    bRetCode = Send(&AddGroup, sizeof(C2S_ADD_FELLOWSHIP_GROUP_REQUEST));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoDelFellowshipGroupRequest(DWORD dwGroupID)
{
    BOOL                                bResult = false;
    BOOL                                bRetCode = false;
    C2S_DEL_FELLOWSHIP_GROUP_REQUEST    DelGroup;

    KGLOG_PROCESS_ERROR(dwGroupID < KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    DelGroup.byProtocolID   = c2s_del_fellowship_group_request;
    DelGroup.dwGroupID      = dwGroupID;

    bRetCode = Send(&DelGroup, sizeof(C2S_DEL_FELLOWSHIP_GROUP_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRenameFellowshipGroupRequest(DWORD dwGroupID, const char cszGroupName[])
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    unsigned                            uGroupNameLen   = 0;
    C2S_RENAME_FELLOWSHIP_GROUP_REQUEST Rename;

    KGLOG_PROCESS_ERROR(cszGroupName);
    uGroupNameLen = (unsigned)strlen(cszGroupName);

    if (uGroupNameLen >= sizeof(Rename.szGroupName))
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam;
            UIParam.nFellowshipRespond = rrcInvalidGroupName;
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }

    KGLOG_PROCESS_ERROR(uGroupNameLen > 0 && uGroupNameLen < sizeof(Rename.szGroupName));

    KGLOG_PROCESS_ERROR(dwGroupID < KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    Rename.byProtocolID = c2s_rename_fellowship_group_request;
    Rename.dwGroupID    = dwGroupID;
    strncpy(Rename.szGroupName, cszGroupName, sizeof(Rename.szGroupName));
    Rename.szGroupName[sizeof(Rename.szGroupName) - sizeof('\0')] = '\0';

    bRetCode = Send(&Rename, sizeof(C2S_RENAME_FELLOWSHIP_GROUP_REQUEST));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetFellowshipRemarkRequest(DWORD dwAlliedPlayerID, const char cszRemark[])
{
    BOOL                        bResult = false;
    BOOL                        bRetCode = false;
    unsigned                    uRemarkLen = 0;
    C2S_SET_FELLOWSHIP_REMARK   SetRemark;
    KUIEventFellowshipRespond   UIParam;

    KGLOG_PROCESS_ERROR(cszRemark);
    uRemarkLen = (unsigned)strlen(cszRemark);

    if (uRemarkLen >= sizeof(SetRemark.szRemark))
    {
        if (g_pGameWorldUIHandler)
        {
            UIParam.nFellowshipRespond = rrcInvalidRemark;
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }

    KGLOG_PROCESS_ERROR(uRemarkLen > 0 && uRemarkLen < sizeof(SetRemark.szRemark));

    KGLOG_PROCESS_ERROR(dwAlliedPlayerID > 0);

    SetRemark.byProtocolID      = c2s_set_fellowship_remark_request;
    SetRemark.dwAlliedPlayerID  = dwAlliedPlayerID;
    strncpy(SetRemark.szRemark, cszRemark, sizeof(SetRemark.szRemark));
    SetRemark.szRemark[sizeof(SetRemark.szRemark) - sizeof('\0')] = '\0';

    bRetCode = Send(&SetRemark, sizeof(C2S_SET_FELLOWSHIP_REMARK));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetFellowshipGroupRequest(DWORD dwAlliedPlayerID, DWORD dwGroupID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    C2S_SET_FELLOWSHIP_GROUP    SetGroup;

    KGLOG_PROCESS_ERROR(dwAlliedPlayerID > 0);

    SetGroup.byProtocolID       = c2s_set_fellowship_group_request;
    SetGroup.dwAlliedPlayerID   = dwAlliedPlayerID;
    SetGroup.dwGroupID          = dwGroupID;

    bRetCode = Send(&SetGroup, sizeof(C2S_SET_FELLOWSHIP_GROUP));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGetFellowshipListRequest(int nType)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    C2S_GET_FELLOWSHIP_LIST_REQUEST GetFellowshipList;

    GetFellowshipList.byProtocolID  = c2s_get_fellowship_list_request;
    GetFellowshipList.byType        = (BYTE)nType;

    switch(nType)
    {
    case 1:
        g_pSO3World->m_FellowshipMgr.ClearFellowship(g_pSO3World->m_dwClientPlayerID);
        break;
    case 2:
        g_pSO3World->m_FellowshipMgr.ClearFoe(g_pSO3World->m_dwClientPlayerID);
        break;
    case 3:
        g_pSO3World->m_FellowshipMgr.ClearBlackList(g_pSO3World->m_dwClientPlayerID);
        break;
    }

    bRetCode = Send(&GetFellowshipList, sizeof(C2S_GET_FELLOWSHIP_LIST_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSendMailRequest(
    DWORD dwNpcID, int nRequestID, char szDestName[], char szTitle[], int nMoney, BYTE byTextData[], size_t uTextLen, 
    DWORD dwBox[KMAIL_MAX_ITEM_COUNT], DWORD dwX[KMAIL_MAX_ITEM_COUNT], int nItemPrice[KMAIL_MAX_ITEM_COUNT]
)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_SEND_MAIL_REQUEST*  pPak        = NULL;
    size_t                  uPakSize    = 0;
    BYTE*                   pszBuffer   = NULL;

    KGLOG_PROCESS_ERROR(dwNpcID);
    
    pszBuffer = new BYTE[MAX_EXTERNAL_PACKAGE_SIZE];
    KG_PROCESS_ERROR(pszBuffer);

    uPakSize = sizeof(C2S_SEND_MAIL_REQUEST) + uTextLen;
    KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPak = (C2S_SEND_MAIL_REQUEST*)pszBuffer;

    strncpy(pPak->szDstName, szDestName, sizeof(pPak->szDstName));
    pPak->szDstName[sizeof(pPak->szDstName) - 1] = '\0';

    strncpy(pPak->szTitle, szTitle, sizeof(pPak->szTitle));
    pPak->szTitle[sizeof(pPak->szTitle) - 1] = '\0';

    pPak->byProtocolID = c2s_send_mail_request;
    pPak->dwNpcID      = dwNpcID;
    pPak->byRequestID  = (BYTE)nRequestID;
    pPak->nMoney       = nMoney;

    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        pPak->byBox[i]        = (BYTE)dwBox[i];
        pPak->byX[i]          = (BYTE)dwX[i];
        pPak->nItemPrice[i]   = nItemPrice[i];
    }

    pPak->wTextLen     = (WORD)uTextLen;
    memcpy(pPak->szText, byTextData, uTextLen);

    bRetCode = Send(pszBuffer, uPakSize);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_DELETE_ARRAY(pszBuffer);
    return bResult;
}

BOOL KPlayerClient::DoGetMailListRequest(DWORD dwStartID)
{
    BOOL            bResult   = false;
    BOOL            bRetCode  = false;
    C2S_GET_MAILLIST_REQUEST  Request;

    Request.byProtocolID = c2s_get_maillist_request;
    Request.dwStartID    = dwStartID;

    bRetCode = Send(&Request, sizeof(C2S_GET_MAILLIST_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoQueryMailContent(DWORD dwNpcID, DWORD dwMailID, int nMailType)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2S_QUERY_MAIL_CONTENT  GetMailContent;

    KGLOG_PROCESS_ERROR(dwMailID);

    GetMailContent.byProtocolID = c2s_query_mail_content;
    GetMailContent.dwNpcID      = dwNpcID;
    GetMailContent.dwMailID     = dwMailID;
    GetMailContent.byMailType   = (BYTE)nMailType;

    bRetCode = Send(&GetMailContent, sizeof(C2S_QUERY_MAIL_CONTENT));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAcquireMailMoneyRequest(DWORD dwMailID)
{
    BOOL                            bResult    = false;
    BOOL                            bRetCode   = false;
    C2S_ACQUIRE_MAIL_MONEY_REQUEST  Request;

    KGLOG_PROCESS_ERROR(dwMailID);

    Request.byProtocolID  = c2s_acquire_mail_money_request;
    Request.dwMailID      = dwMailID;

    bRetCode = Send(&Request, sizeof(C2S_ACQUIRE_MAIL_MONEY_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAcquireMailItemRequest(DWORD dwMailID, int nIndex)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_ACQUIRE_MAIL_ITEM_REQUEST   Request;

    KGLOG_PROCESS_ERROR(dwMailID);
    KGLOG_PROCESS_ERROR(nIndex >=0 && nIndex < KMAIL_MAX_ITEM_COUNT);

    Request.byProtocolID  = c2s_acquire_mail_item_request;
    Request.dwMailID      = dwMailID;
    Request.byIndex       = (BYTE)nIndex;

    bRetCode = Send(&Request, sizeof(C2S_ACQUIRE_MAIL_ITEM_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetMailRead(DWORD dwMailID)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    C2S_SET_MAIL_READ   SetMailRead;

    SetMailRead.byProtocolID = c2s_set_mail_read;
    SetMailRead.dwMailID     = dwMailID;

    bRetCode = Send(&SetMailRead, sizeof(C2S_SET_MAIL_READ));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoDeleteMail(DWORD dwMailID)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    C2S_DELETE_MAIL     DeleteMail;

    DeleteMail.byProtocolID = c2s_delete_mail;
    DeleteMail.dwMailID     = dwMailID;

    bRetCode = Send(&DeleteMail, sizeof(C2S_DELETE_MAIL));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoReturnMail(DWORD dwMailID)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    C2S_RETURN_MAIL     ReturnMail;

    ReturnMail.byProtocolID = c2s_return_mail;
    ReturnMail.dwMailID     = dwMailID;

    bRetCode = Send(&ReturnMail, sizeof(C2S_RETURN_MAIL));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyPKOperate(BYTE byPKOperate, DWORD dwParam1, DWORD dwParam2)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
	C2S_APPLY_PK_OPERATE	ApplyPKOperate;

	ApplyPKOperate.byProtocolID = c2s_apply_pk_operate;
	ApplyPKOperate.byPKOperate  = byPKOperate;
	ApplyPKOperate.dwParam1     = dwParam1;
	ApplyPKOperate.dwParam2     = dwParam2;

	bRetCode = Send(&ApplyPKOperate, sizeof(C2S_APPLY_PK_OPERATE));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSyncUserPreferences(int nOffset, int nLength, BYTE* pbyData)
{
	BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    size_t                      uPakSize        = sizeof(C2S_SYNC_USER_PREFERENCES) + nLength;
    IKG_Buffer*                 piBuffer        = NULL;
	C2S_SYNC_USER_PREFERENCES*  pPak            = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pPak = (C2S_SYNC_USER_PREFERENCES*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

	pPak->byProtocolID  = c2s_sync_user_preferences;
	pPak->nOffset       = nOffset;
	pPak->wDataLen      = (WORD)nLength;

    memcpy(pPak->byData, pbyData, nLength);

	bRetCode = Send(pPak, uPakSize);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
	return bResult;
}

BOOL KPlayerClient::DoSetBankOpenedFlag(BOOL bBankOpenedFlag, DWORD dwNpcID)
{
    BOOL                        bResult         = false;
	BOOL                        bRetCode        = false;
	C2S_SET_BANK_OPENED_FLAG    SetBankOpened;

	SetBankOpened.byProtocolID   = c2s_set_bank_opened_flag;
    SetBankOpened.byBankOpend    = (BYTE)bBankOpenedFlag;
	SetBankOpened.dwNpcID        = dwNpcID;

	bRetCode = Send(&SetBankOpened, sizeof(C2S_SET_BANK_OPENED_FLAG));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoEnableBankPackage()
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	C2S_ENABLE_BANK_PACKAGE EnableBankPackage;

	EnableBankPackage.byProtocolID = c2s_enable_bank_package;

	bRetCode = Send(&EnableBankPackage, sizeof(C2S_ENABLE_BANK_PACKAGE));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoSyncMidMapMark(int nMapID, int nX, int nY, int nType, const char* pszComment)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	C2S_SYNC_MID_MAP_MARK SyncMidMapMark;

	SyncMidMapMark.byProtocolID = c2s_sync_mid_map_mark;
	SyncMidMapMark.nMapID = nMapID;
	SyncMidMapMark.nX = nX;
	SyncMidMapMark.nY = nY;
	SyncMidMapMark.nType = nType;
	
	ZeroMemory(SyncMidMapMark.szComment, sizeof(SyncMidMapMark.szComment));
	if (pszComment)
	{
		strncpy(SyncMidMapMark.szComment, pszComment, MAX_MIDMAP_MARK_COMMENT_LEN);
		SyncMidMapMark.szComment[MAX_MIDMAP_MARK_COMMENT_LEN - 1] = '\0';
	}

	bRetCode = Send(&SyncMidMapMark, sizeof(SyncMidMapMark));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoPlayerLearnSkill(DWORD dwSkillID, DWORD dwNpcID)
{
    BOOL                    bResult  = false;
	BOOL                    bRetCode = false;
	C2S_PLAYER_LEARN_SKILL  PlayerLearnSkill;

    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);
    KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);

	PlayerLearnSkill.byProtocolID   = c2s_player_learn_skill;
	PlayerLearnSkill.dwSkillID      = dwSkillID;
    PlayerLearnSkill.dwNpcID        = dwNpcID;

	bRetCode =  Send(&PlayerLearnSkill, sizeof(PlayerLearnSkill));
	KGLOG_CHECK_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoPlayerLearnRecipe(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwNpcID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	C2S_PLAYER_LEARN_RECIPE PlayerLearnRecipe; 

	KGLOG_PROCESS_ERROR(dwCraftID);
	KGLOG_PROCESS_ERROR(dwRecipeID);
	KGLOG_PROCESS_ERROR(dwNpcID);

	PlayerLearnRecipe.byProtocolID = c2s_player_learn_recipe;
	PlayerLearnRecipe.dwCraftID = dwCraftID;
	PlayerLearnRecipe.dwRecipeID = dwRecipeID;
	PlayerLearnRecipe.dwNpcID = dwNpcID;

	bRetCode = Send(&PlayerLearnRecipe, sizeof(PlayerLearnRecipe));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoPlayerActiveSkillRecipe(DWORD dwSkillRecipeKey, BOOL bActive)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    C2S_PLAYER_ACTIVE_SKILL_RECIPE PlayerActiveSkillRecipe; 

    assert(dwSkillRecipeKey);

    PlayerActiveSkillRecipe.byProtocolID = c2s_player_active_skill_recipe;
    PlayerActiveSkillRecipe.dwRecipeKey = dwSkillRecipeKey;
    PlayerActiveSkillRecipe.bActive = bActive;

    bRetCode = Send(&PlayerActiveSkillRecipe, sizeof(PlayerActiveSkillRecipe));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoPeekOtherPlayerEquip(DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_PEEK_OTHER_PLAYER_EQUIP     PeekOtherPlayer; 

    PeekOtherPlayer.byProtocolID    = c2s_peek_other_player_equip;
    PeekOtherPlayer.dwPlayerID      = dwPlayerID;

    bRetCode = Send(&PeekOtherPlayer, sizeof(PeekOtherPlayer));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPeekOtherPlayerBook(DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_PEEK_OTHER_PLAYER_BOOK      PeekOtherPlayer; 

    PeekOtherPlayer.byProtocolID    = c2s_peek_other_player_book;
    PeekOtherPlayer.dwPlayerID      = dwPlayerID;

    bRetCode = Send(&PeekOtherPlayer, sizeof(PeekOtherPlayer));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPeekOtherPlayerQuest(DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_PEEK_OTHER_PLAYER_QUEST     PeekOtherPlayer; 

    PeekOtherPlayer.byProtocolID    = c2s_peek_other_player_quest;
    PeekOtherPlayer.dwPlayerID      = dwPlayerID;

    bRetCode = Send(&PeekOtherPlayer, sizeof(PeekOtherPlayer));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRoadTrackStartOut(int nFromNode, int nTargetCityID, int nCost)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    C2S_ROAD_TRACK_START_OUT    RoadTrackStartOut;
    
    RoadTrackStartOut.byProtocolID  = c2s_road_track_start_out;
    RoadTrackStartOut.nFromNode     = nFromNode;
    RoadTrackStartOut.nTargetCityID = nTargetCityID;
    RoadTrackStartOut.nCost         = nCost;

    bRetCode = Send(&RoadTrackStartOut, sizeof(RoadTrackStartOut));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRoadTrackForceRequest()
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_ROAD_TRACK_FORCE_REQUEST    ForceRequest;

    ForceRequest.byProtocolID = c2s_road_track_force_request;

    bRetCode = Send(&ForceRequest, sizeof(ForceRequest));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoOpenVenation(int nVenationID)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    C2S_OPEN_VENATION   OpenVenation;

    OpenVenation.byProtocolID = c2s_open_venation;
    OpenVenation.nVenationID  = nVenationID;

    bRetCode = Send(&OpenVenation, sizeof(OpenVenation));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSendGmMessage(const char cszMessage[])
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    BYTE                    byBuffer[MAX_EXTERNAL_PACKAGE_SIZE];
    C2S_SEND_GM_MESSAGE*    pMessage    = (C2S_SEND_GM_MESSAGE*)byBuffer;
    size_t                  uStrLen     = 0;

    assert(cszMessage);

    uStrLen = strlen(cszMessage);
    KGLOG_PROCESS_ERROR(sizeof(C2S_SEND_GM_MESSAGE) + uStrLen < MAX_EXTERNAL_PACKAGE_SIZE);

    pMessage->byProtocolID = c2s_send_gm_message;

    strcpy(pMessage->szMessage, cszMessage);

    bRetCode = Send(pMessage, sizeof(C2S_SEND_GM_MESSAGE) + uStrLen + 1);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoJoinBattleFieldQueueRequest(DWORD dwNpcID, DWORD dwMapID, int nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    C2S_JOIN_BATTLE_FIELD_QUEUE_REQUEST JoinRequest;

    JoinRequest.byProtocolID = c2s_join_battle_field_queue_request;
    JoinRequest.dwNpcID      = dwNpcID;
    JoinRequest.dwMapID      = dwMapID;
    JoinRequest.nCopyIndex   = nCopyIndex;
    JoinRequest.dwGroupID    = dwGroupID;
    JoinRequest.bTeamJoin    = bTeamJoin;

    bRetCode = Send(&JoinRequest, sizeof(JoinRequest));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoLeaveBattleFieldQueueRequest(DWORD dwMapID)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    C2S_LEAVE_BATTLE_FIELD_QUEUE_REQUEST    LeaveRequest;

    LeaveRequest.byProtocolID = c2s_leave_battle_field_queue_request;
    LeaveRequest.dwMapID      = dwMapID;

    bRetCode = Send(&LeaveRequest, sizeof(LeaveRequest));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAcceptJoinBattleField(DWORD dwMapID, int nCopyIndex)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_ACCEPT_JOIN_BATTLE_FIELD    AcceptJoin;

    AcceptJoin.byProtocolID = c2s_accept_join_battle_field;
    AcceptJoin.dwMapID      = dwMapID;
    AcceptJoin.nCopyIndex   = nCopyIndex;

    bRetCode = Send(&AcceptJoin, sizeof(AcceptJoin));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGetBattleFieldList(DWORD dwMapID)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    C2S_GET_BATTLE_FIELD_LIST   GetBattleFieldList;

    GetBattleFieldList.byProtocolID = c2s_get_battle_field_list;
    GetBattleFieldList.dwMapID      = dwMapID;

    bRetCode = Send(&GetBattleFieldList, sizeof(GetBattleFieldList));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoLeaveBattleField()
{
    BOOL                    bResult             = false;
    BOOL                    bRetCode            = false;
    C2S_LEAVE_BATTLE_FIELD  LeaveBattleField;

    LeaveBattleField.byProtocolID = c2s_leave_battle_field;

    bRetCode = Send(&LeaveBattleField, sizeof(LeaveBattleField));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyTongRosterRequest()
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    int                             nMemberCount    = 0;
    size_t                          uPakSize        = 0;
    IKG_Buffer*                     piPackage       = NULL;
    C2S_APPLY_TONG_ROSTER_REQUEST*  pSyncRequest    = NULL;

    nMemberCount = g_pSO3World->m_TongClient.GetMemberCount();
    KGLOG_PROCESS_ERROR(nMemberCount >= 0);

    uPakSize = sizeof(C2S_APPLY_TONG_ROSTER_REQUEST) + sizeof(DWORD) * nMemberCount;

    piPackage = KG_MemoryCreateBuffer((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(piPackage);

    pSyncRequest = (C2S_APPLY_TONG_ROSTER_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSyncRequest);

    pSyncRequest->byProtocolID      = c2s_update_tong_roster_request;
    pSyncRequest->nLastUpdateFrame  = g_pSO3World->m_TongClient.m_nLastRosterUpdateFrame;
    pSyncRequest->nMemberCount      = nMemberCount;
    
    bRetCode = g_pSO3World->m_TongClient.GetMemberList(pSyncRequest->dwMemberList, nMemberCount);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = Send(pSyncRequest, uPakSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KPlayerClient::DoApplyTongInfoRequest()
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_APPLY_TONG_INFO_REQUEST     ApplyRequest;

    ApplyRequest.byProtocolID       = c2s_apply_tong_info_request;
    ApplyRequest.nLastUpdateFrame   = g_pSO3World->m_TongClient.m_nLastInfoUpdateFrame;

    bRetCode = Send(&ApplyRequest, sizeof(C2S_APPLY_TONG_INFO_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyTongRepertoryPageRequest(int nPageIndex, int nLastUpdateCounter)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    C2S_APPLY_TONG_REPERTORY_PAGE_REQUEST   ApplyRequest;
    
    ApplyRequest.byProtocolID       = c2s_apply_tong_repertory_page_request;
    ApplyRequest.byPageIndex        = (BYTE)nPageIndex;
    ApplyRequest.nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(&ApplyRequest, sizeof(C2S_APPLY_TONG_REPERTORY_PAGE_REQUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoInvitePlayerJoinTongRequest(const char* cpszNewMemberName)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    C2S_INVITE_PLAYER_JOIN_TONG_REQUEST InviteRequest;

    InviteRequest.byProtocolID  = c2s_invite_player_join_tong_request;
    
    strncpy(InviteRequest.szNewMemberName, cpszNewMemberName, sizeof(InviteRequest.szNewMemberName));
    InviteRequest.szNewMemberName[sizeof(InviteRequest.szNewMemberName) - 1] = '\0';

    bRetCode = Send(&InviteRequest, sizeof(InviteRequest));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoInvitePlayerJoinTongRespond(DWORD dwInviterID, DWORD dwTongID, BOOL bAccept)
{
    BOOL                                bResult    = false;
    BOOL                                bRetCode   = false;
    C2S_INVITE_PLAYER_JOIN_TONG_RESPOND Respond;

    Respond.byProtocolID    = c2s_invite_player_join_tong_respond;
    Respond.dwInviterID     = dwInviterID;
    Respond.dwTongID        = dwTongID;
    Respond.byAccept        = (BYTE)(bAccept ? itrSuccess : itrRefuse);

    bRetCode = Send(&Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyKickOutTongMember(DWORD dwMemberID)
{
    BOOL                                    bResult  = false;
    BOOL                                    bRetCode = false;
    C2S_APPLY_KICK_OUT_TONG_MEMBER_REQUEST  Request;

    Request.byProtocolID    = c2s_apply_kick_out_tong_member_request;
    Request.dwMemberID      = dwMemberID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoModifyTongInfoRequest(int nDataType, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    C2S_MODIFY_TONG_INFO_REQUEST*       pRequest = NULL;

    assert(pbyData);
    assert(nDataType > tmrBegin && nDataType < tmrEnd);
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(C2S_MODIFY_TONG_INFO_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (C2S_MODIFY_TONG_INFO_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocolID  = c2s_modify_tong_info_request;
    pRequest->byInfoType    = (BYTE)nDataType;
    pRequest->wDataLen      = (WORD)uDataLen;
    memcpy(pRequest->byData, pbyData, uDataLen);

    bRetCode = Send(pRequest, sizeof(C2S_MODIFY_TONG_INFO_REQUEST) + uDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KPlayerClient::DoModifyTongSchemaRequest(int nGroupIndex, int nModifyType, BYTE* byData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    C2S_MODIFY_TONG_SCHEMA_REQUEST* pRequest    = NULL;

    assert(byData);
    assert(nModifyType > tmsBegin && nModifyType < tmsEnd);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(C2S_MODIFY_TONG_SCHEMA_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (C2S_MODIFY_TONG_SCHEMA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byProtocolID = c2s_modify_tong_schema_request;
    pRequest->byModifyType = (BYTE)nModifyType;
    pRequest->byGroupIndex = (BYTE)nGroupIndex;

    memcpy(pRequest->byData, byData, uDataLen);
    
    bRetCode = Send(pRequest, sizeof(C2S_MODIFY_TONG_SCHEMA_REQUEST) + uDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KPlayerClient::DoApplyQuitTongRequest()
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_APPLY_QUIT_TONG_REQUEST    Request;

    Request.byProtocolID = c2s_apply_quit_tong_request;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoChangeTongMemberGroupRequest(DWORD dwTargetMemberID, int nTargetGroup)
{
    BOOL                                    bResult  = false;
    BOOL                                    bRetCode = false;
    C2S_CHANGE_TONG_MEMBER_GROUP_REQUEST    Request;

    Request.byProtocolID        = c2s_change_tong_member_group_request;
    Request.dwTargetMemberID    = dwTargetMemberID;
    Request.byTargetGroup       = (BYTE)nTargetGroup;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoChangeTongMasterRequest(DWORD dwNewMasterID)
{
    BOOL                            bResult    = false;
    BOOL                            bRetCode   = false;
    C2S_CHANGE_TONG_MASTER_REQUEST  Request;

    Request.byProtocolID    = c2s_change_tong_master_request;
    Request.dwNewMasterID   = dwNewMasterID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoChangeMemberRemarkRequest(DWORD dwTargetMemberID, const char* cpszRemark)
{
    BOOL                                    bResult    = false;
    BOOL                                    bRetCode   = false;
    C2S_CHANGE_TONG_MEMBER_REMARK_REQUEST   Request;

    Request.byProtocolID = c2s_change_tong_member_remark_request;

    Request.dwTargetMemberID = dwTargetMemberID;
    
    strncpy(Request.szRemark, cpszRemark, sizeof(Request.szRemark));
    Request.szRemark[sizeof(Request.szRemark) - 1] = '\0';

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGetTongDescriptionRequest(DWORD dwTongID)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    C2S_GET_TONG_DESCRIPTION_REQUEST    Request;

    Request.byProtocolID    = c2s_get_tong_description_request;
    Request.dwTongID        = dwTongID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSaveMoneyInTongRequest(int nSaveMoney)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    C2S_SAVE_MONEY_IN_TONG_REQUEST  Request;

    Request.byProtocolID    = c2s_save_money_in_tong_request;
    Request.nMoney          = nSaveMoney;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPayTongSalaryRequest(int nTotalSalary)
{
    BOOL                        bResult    = false;
    BOOL                        bRetCode   = false;
    C2S_PAY_TONG_SALARY_REQUEST Request;

    Request.byProtocolID = c2s_pay_tong_salary_request;
    Request.nTotalSalary = nTotalSalary;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGetTongSalaryRequest()
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    C2S_GET_TONG_SALARY_REQUEST Request;

    Request.byProtocolID = c2s_get_tong_salary_request;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyOpenTongRepertoryRequest(DWORD dwNpcID)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    C2S_APPLY_OPEN_TONG_REPERTORY_REQUEST   Request;

    Request.byProtocolID    = c2s_apply_open_tong_repertory_request;
    Request.dwNpcID         = dwNpcID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTakeTongRepertoryItemToPosRequest(int nPageIndex, int nPagePos, int nBoxIndex, int nBoxPos, int nLastUpdateCounter)
{
    BOOL                                        bResult     = false;
    BOOL                                        bRetCode    = false;
    C2S_TAKE_TONG_REPERTORY_ITEM_TO_POS_REQUEST Request;

    Request.byProtocolID    = c2s_take_tong_repertory_item_to_pos_request;
    Request.byPageIndex     = (BYTE)nPageIndex;
    Request.byPagePos       = (BYTE)nPagePos;
    Request.byBoxIndex      = (BYTE)nBoxIndex;
    Request.byBoxPos        = (BYTE)nBoxPos;
    Request.nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTakeTongRepertoryItemRequest(int nPageIndex, int nPagePos, int nLastUpdateCounter)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    C2S_TAKE_TONG_REPERTORY_ITEM_REQUEST    Request;

    Request.byProtocolID    = c2s_take_tong_repertory_item_request;
    Request.byPageIndex     = (BYTE)nPageIndex;
    Request.byPagePos       = (BYTE)nPagePos;
    Request.nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPutTongRepertoryItemToPosRequest(int nBoxIndex, int nBoxPos, int nPageIndex, int nPagePos)
{
    BOOL                                        bResult    = false;
    BOOL                                        bRetCode   = false;
    C2S_PUT_TONG_REPERTORY_ITEM_TO_POS_REQUEST  Request;

    Request.byProtocolID    = c2s_put_tong_repertory_item_to_pos_request;
    Request.byBoxIndex      = (BYTE)nBoxIndex;
    Request.byBoxPos        = (BYTE)nBoxPos;
    Request.byPageIndex     = (BYTE)nPageIndex;
    Request.byPagePos       = (BYTE)nPagePos;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPutTongRepertoryItemRequest(int nBoxIndex, int nBoxPos, int nPageIndex)
{
    BOOL                                bResult    = false;
    BOOL                                bRetCode   = false;
    C2S_PUT_TONG_REPERTORY_ITEM_REQUEST Request;

    Request.byProtocolID    = c2s_put_tong_repertory_item_request;
    Request.byBoxIndex      = (BYTE)nBoxIndex;
    Request.byBoxPos        = (BYTE)nBoxPos;
    Request.byPageIndex     = (BYTE)nPageIndex;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoExchangeTongRepertoryItemPosRequest(
    int nSrcPageIndex, int nSrcPagePos, int nSrcPageLastUpdateCounter, 
    int nDstPageIndex, int nDstPagePos, int nDstPageLastUpdateCounter
)
{
    BOOL                                            bResult     = false;
    BOOL                                            bRetCode    = false;
    C2S_EXCHANGE_TONG_REPERTORY_ITEM_POS_REQUEST    Request;

    Request.byProtocolID    = c2s_exchange_tong_repertory_item_pos_request;
    Request.bySrcPageIndex  = (BYTE)nSrcPageIndex;
    Request.bySrcPagePos    = (BYTE)nSrcPagePos;
    Request.byDstPageIndex  = (BYTE)nDstPageIndex;
    Request.byDstPagePos    = (BYTE)nDstPagePos;
    Request.nSrcPageLastUpdateCounter = nSrcPageLastUpdateCounter;
    Request.nDstPageLastUpdateCounter = nDstPageLastUpdateCounter;
    
    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoStackTongRepertoryItemRequest(
    int nPageIndex, int nPagePos, int nBoxIndex, int nBoxPos, int nStackNum,
    int nLastUpdateCounter
)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    C2S_STACK_TONG_REPERTORY_ITEM_REQUEST   Request;

    Request.byProtocolID    = c2s_stack_tong_repertory_item_request;
    Request.byPageIndex     = (BYTE)nPageIndex;
    Request.byPagePos       = (BYTE)nPagePos;
    Request.byBoxIndex      = (BYTE)nBoxIndex;
    Request.byBoxPos        = (BYTE)nBoxPos;
    Request.wStackNum       = (WORD)nStackNum;
    Request.nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyBattleFieldStatistics()
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    C2S_APPLY_BATTLEFIELD_STATISTICS    ApplyStatistics;

    assert(g_pSO3World);

    g_pSO3World->m_BattleFieldStatisticsMap.clear();

    ApplyStatistics.byProtocolID = c2s_apply_battlefield_statistics;

    bRetCode = Send(&ApplyStatistics, sizeof(C2S_APPLY_BATTLEFIELD_STATISTICS));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetHeroFlag(BOOL bHeroFlag)
{
    BOOL               bResult     = false;
    BOOL               bRetCode    = false;
    C2S_SET_HERO_FLAG  SetHeroFlag;

    SetHeroFlag.byProtocolID = c2s_set_hero_flag;
    SetHeroFlag.byHeroFlag   = (BYTE)bHeroFlag;

    bRetCode = Send(&SetHeroFlag, sizeof(C2S_SET_HERO_FLAG));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAuctionLookupRequest(
    DWORD dwNpcID, BYTE byRequestID, const char* pszSaleName, int nAucGenre, int nAucSub, int nLevelMin,
    int nLevelMax, int nQuality, const char* pszSellerName, DWORD dwSellerID, DWORD dwBidderID, int nTopPrice, int nAuction,
    int nStartIndex, BYTE byOrderType, BOOL bDesc
)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_AUCTION_LOOKUP_REQUEST  Request;

    Request.byProtocolID = c2s_auction_lookup_request;
    
    Request.dwNpcID     = dwNpcID;
    Request.byRequestID = byRequestID;

    strncpy(Request.Param.szSaleName, pszSaleName, sizeof(Request.Param.szSaleName));
    Request.Param.szSaleName[sizeof(Request.Param.szSaleName) - 1] = '\0';

    Request.Param.nAucGenre   = nAucGenre;
    Request.Param.nAucSub     = nAucSub;
    Request.Param.nLevelMin   = nLevelMin;
    Request.Param.nLevelMax   = nLevelMax;
    Request.Param.nQuality    = nQuality;
    
    strncpy(Request.Param.szSellerName, pszSellerName, sizeof(Request.Param.szSellerName));
    Request.Param.szSellerName[sizeof(Request.Param.szSellerName) - 1] = '\0';

    Request.Param.dwSellerID  = dwSellerID;
    Request.Param.dwBidderID  = dwBidderID;
    Request.Param.nTopPrice   = nTopPrice;
    Request.Param.byAuction   = (BYTE)nAuction;
    Request.Param.nStartIndex = nStartIndex;
    Request.Param.byOrderType = byOrderType;
    Request.Param.byDesc      = (BYTE)bDesc;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAuctionBidRequest(DWORD dwNpcID, DWORD dwSaleID, DWORD dwCRC, int nPrice)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
    C2S_AUCTION_BID_REQUEST Request;

    Request.byProtocolID    = c2s_auction_bid_request;
    Request.dwNpcID         = dwNpcID;
    Request.dwSaleID        = dwSaleID;
    Request.dwCRC           = dwCRC;
    Request.nPrice          = nPrice;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAuctionSellRequest(DWORD dwNpcID, DWORD dwBox, DWORD dwX, int nStartPrice, int nBuyItNowPrice, int nLeftHour)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_AUCTION_SELL_REQUEST    Request;

    Request.byProtocolID    = c2s_auction_sell_request;
    Request.dwNpcID         = dwNpcID;
    Request.byBox           = (BYTE)dwBox;
    Request.byX             = (BYTE)dwX;
    Request.nStartPrice     = nStartPrice;
    Request.nBuyItNowPrice  = nBuyItNowPrice;
    Request.byLeftHour      = (BYTE)nLeftHour;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoAuctionCancelRequest(DWORD dwNpcID, DWORD dwSaleID)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_AUCTION_CANCEL_REQUEST  Request;

    Request.byProtocolID    = c2s_auction_cancel_request;
    Request.dwNpcID         = dwNpcID;
    Request.dwSaleID        = dwSaleID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplySetCampRequest(KCAMP eNewCamp)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_APPLY_SET_CAMP_REQUEST  Request;
    
    Request.byProtocolID = c2s_apply_set_camp_request;
    Request.byCamp       = (BYTE)eNewCamp;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRemoteLuaCall(const char cszFunction[], BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    C2S_REMOTE_LUA_CALL*    pLuaCall        = NULL;
    size_t                  uPakSize        = sizeof(C2S_REMOTE_LUA_CALL) + uDataLen;
    IKG_Buffer*             piBuffer        = NULL;
    size_t                  uFuncNameSize   = 0;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pLuaCall = (C2S_REMOTE_LUA_CALL*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pLuaCall);

    pLuaCall->byProtocolID = c2s_remote_lua_call;
    pLuaCall->uDataLen     = uDataLen;

    uFuncNameSize = strlen(cszFunction) + 1;
    KGLOG_PROCESS_ERROR(uFuncNameSize <= sizeof(pLuaCall->szFunction));

    memcpy(pLuaCall->szFunction, cszFunction, uFuncNameSize);
    memcpy(pLuaCall->byData, pbyData, uDataLen);

    bRetCode = Send(pLuaCall, uPakSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KPlayerClient::DoPlayerReviveRequest(int nReviveCode)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_PLAYER_REVIVE_REQUEST   Request;

    Request.byProtocolID    = c2s_player_revive_request;
    Request.nReviveCode     = nReviveCode;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


BOOL KPlayerClient::DoApplyAchievementDataRequest(DWORD dwPlayerID)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    C2S_APPLY_ACHIEVEMENT_DATA_REQUEST  Request;

    Request.byProtocolID    = c2s_apply_achievement_data_request;
    Request.dwPlayerID      = dwPlayerID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoQueryProductIdentity(int nMagic)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    C2S_QUERY_PRODUCT_IDENTITY  Request;

    Request.byProtocolID = c2s_query_product_identity;
    Request.nMagic       = nMagic;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;    
}

BOOL KPlayerClient::DoSetCurrentDesignationRequest(int nPrefix, int nPostfix, BOOL bDisplayFlag)
{
    BOOL                                 bResult  = false;
    BOOL                                 bRetCode = false;
    C2S_SET_CURRENT_DESIGNATION_REQUEST  Request;

    Request.byProtocolID     = c2s_set_current_designation_request;
    Request.byPrefix         = (BYTE)nPrefix;
    Request.byPostfix        = (BYTE)nPostfix;
    Request.byDisplayFlag    = (BYTE)bDisplayFlag;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCreateViewPointRequest(int nX, int nY, int nLifeFrame)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_CREATE_VIEW_POINT_REQUEST   Request;

    Request.byProtocolID = c2s_create_view_point_request;
    Request.nX           = nX;
    Request.nY           = nY;
    Request.nLifeFrame   = nLifeFrame;
    
    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoRemoveViewPointRequest()
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_REMOVE_VIEW_POINT_REQUEST   Request;

    Request.byProtocolID = c2s_remove_view_point_request;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoMoveViewPointRequest(int nX, int nY)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_MOVE_VIEW_POINT_REQUEST     MoveViewPoint;

    MoveViewPoint.byProtocolID  = c2s_move_view_point_request;
    MoveViewPoint.nX            = nX;
    MoveViewPoint.nY            = nY;

    bRetCode = Send(&MoveViewPoint, sizeof(MoveViewPoint));
    KGLOG_PROCESS_ERROR(bRetCode);
	
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApexProtocol(BOOL bIsUserData, BYTE* pbyData, size_t nDataSize)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    C2S_APEX_PROTOCOL*  pApexProtocol   = NULL;
    size_t              nPackageSize    = 0;
    char                szBuffer[MAX_EXTERNAL_PACKAGE_SIZE];

    nPackageSize = sizeof(C2S_APEX_PROTOCOL) + nDataSize;
    KGLOG_PROCESS_ERROR(nPackageSize < MAX_EXTERNAL_PACKAGE_SIZE);

    pApexProtocol = (C2S_APEX_PROTOCOL*)szBuffer;

    pApexProtocol->byProtocolID   = c2s_apex_protocol;
    pApexProtocol->byIsUserData   = (BYTE)bIsUserData;
    pApexProtocol->nSize          = nDataSize;

    memcpy(pApexProtocol->byData, pbyData, nDataSize);

    Send(pApexProtocol, nPackageSize);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGameCardSellRequest(DWORD dwNpcID,int nTypeIndex, int nPrice, int nDurationTime)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_GAME_CARD_SELL_REQUEST  Request;

    Request.byProtocolID    = c2s_game_card_sell_request;
    Request.dwNpcID         = dwNpcID;
    Request.nTypeIndex      = nTypeIndex;
    Request.nPrice          = nPrice;
    Request.nDurationTime   = nDurationTime;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGameCardBuyRequest(DWORD dwNpcID,DWORD dwID, int nBuyPrice)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    C2S_GAME_CARD_BUY_REQUEST   Request;

    Request.byProtocolID    = c2s_game_card_buy_request;
    Request.dwNpcID         = dwNpcID;
    Request.dwID            = dwID;
    Request.nBuyPrice       = nBuyPrice;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGameCardLookupRequest(DWORD dwNpcID, BYTE byCardType, int nStartIndex, BYTE byOrderType, BOOL bDesc)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_GAME_CARD_LOOKUP_REQUEST    Request;

    Request.byProtocolID    = c2s_game_card_lookup_request;
    Request.dwNpcID         = dwNpcID;
    Request.byCardType      = byCardType;
    Request.nStartIndex     = nStartIndex;
    Request.byOrderType     = byOrderType;
    Request.bDesc           = bDesc;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoGameCardCancelRequest(DWORD dwNpcID,DWORD dwID)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    C2S_GAME_CARD_CANCEL_REQUEST    Request;

    Request.byProtocolID    = c2s_game_card_cancel_request;
    Request.dwNpcID         = dwNpcID;
    Request.dwID            = dwID;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyCharacterThreatRankList(DWORD dwCharacterID)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KPlayer*                            pSelf       = NULL;
    DWORD                               dwCoolDown  = g_pSO3World->m_Settings.m_ConstList.nApplyThreatRankListCD;
    C2S_APPLY_CHARACTER_THREAT_RANKLIST Request;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(pSelf);

    bRetCode = pSelf->m_TimerList.CheckTimer(dwCoolDown);
    KG_PROCESS_ERROR(bRetCode);

    Request.byProtocolID    = c2s_apply_character_threat_ranklist;
    Request.dwCharacterID   = dwCharacterID;

    Send(&Request, sizeof(Request));

    bResult = true;
Exit0:
    return bResult;
}

//AutoCode:-发送协议函数结束-

/************************************************************************/
/* 接收数据包                                                           */
/************************************************************************/

void KPlayerClient::OnMessageNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_MESSAGE_NOTIFY* pResponseMessage = (S2C_MESSAGE_NOTIFY*)pbyData;
    KItem*              pTempItem        = NULL;

	switch (pResponseMessage->byType)
	{
    case ectFellowshipErrorCode:
        {
            if (g_pGameWorldUIHandler)
            {
                KUIEventFellowshipRespond UIParam;
                UIParam.nFellowshipRespond = pResponseMessage->byCode;
                g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
            }

            if (pResponseMessage->byCode == rrcSuccessAdd)
            {
                if (g_pGameWorldUIHandler)
                {
                    KUIEventFellowshipChange UIFellowshipChange;

                    UIFellowshipChange.nRespondCode = pResponseMessage->byCode;
                    UIFellowshipChange.dwPlayerID   = g_pSO3World->m_dwClientPlayerID;
                    strncpy(UIFellowshipChange.szName, (char*)pResponseMessage->byData, sizeof(UIFellowshipChange.szName));
                    UIFellowshipChange.szName[sizeof(UIFellowshipChange.szName) - 1] = '\0';

                    g_pGameWorldUIHandler->OnFellowshipChange(UIFellowshipChange);
                }

                g_PlayerClient.DoGetFellowshipListRequest(1);
            }

            if (pResponseMessage->byCode == rrcSuccessAddFoe)
            {
                if (g_pGameWorldUIHandler)
                {
                    KUIEventFellowshipChange UIFellowshipChange;

                    UIFellowshipChange.nRespondCode = pResponseMessage->byCode;
                    UIFellowshipChange.dwPlayerID   = g_pSO3World->m_dwClientPlayerID;
                    strncpy(UIFellowshipChange.szName, (char*)pResponseMessage->byData, sizeof(UIFellowshipChange.szName));
                    UIFellowshipChange.szName[sizeof(UIFellowshipChange.szName) - 1] = '\0';

                    g_pGameWorldUIHandler->OnFellowshipChange(UIFellowshipChange);
                }
                
                g_PlayerClient.DoGetFellowshipListRequest(2);
            }

            if (pResponseMessage->byCode == rrcSuccessAddBlackList)
            {
                if (g_pGameWorldUIHandler)
                {
                    KUIEventFellowshipChange UIFellowshipChange;

                    UIFellowshipChange.nRespondCode = pResponseMessage->byCode;
                    UIFellowshipChange.dwPlayerID   = g_pSO3World->m_dwClientPlayerID;
                    strncpy(UIFellowshipChange.szName, (char*)pResponseMessage->byData, sizeof(UIFellowshipChange.szName));
                    UIFellowshipChange.szName[sizeof(UIFellowshipChange.szName) - 1] = '\0';

                    g_pGameWorldUIHandler->OnFellowshipChange(UIFellowshipChange);
                }

                g_PlayerClient.DoGetFellowshipListRequest(3);
            }

            if (pResponseMessage->byCode == rrcBeAddFriend || pResponseMessage->byCode == rrcBeAddFoe || pResponseMessage->byCode == rrcBeAddBlackList)
            {
                if (g_pGameWorldUIHandler)
                {
                    KUIEventBeAddFellowship UIBeAddFellowship;

                    UIBeAddFellowship.nRespondCode = pResponseMessage->byCode;
                    UIBeAddFellowship.dwPlayerID   = g_pSO3World->m_dwClientPlayerID;
                    strncpy(UIBeAddFellowship.szName, (char*)pResponseMessage->byData, sizeof(UIBeAddFellowship.szName));
                    UIBeAddFellowship.szName[sizeof(UIBeAddFellowship.szName) - 1] = '\0';

                    g_pGameWorldUIHandler->OnBeAddFellowship(UIBeAddFellowship);
                }
            }
        }
        break;

    case ectFellowshipAttractionErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventAddFellowshipAttraction UIParam;
            KMESSAGE_ATTRACTION             Message;

            memcpy(&Message, &(pResponseMessage->byData), sizeof(KMESSAGE_ATTRACTION));

            strncpy(UIParam.szAlliedPlayerName, Message.szAlliedPlayerName, sizeof(UIParam.szAlliedPlayerName));
            UIParam.szAlliedPlayerName[sizeof(UIParam.szAlliedPlayerName) - 1] = '\0';
            UIParam.nAttaction = Message.nAttraction;

            g_pGameWorldUIHandler->OnAddFellowshipAttraction(UIParam);
        }
        break;

    case ectFellowshipAttractionFallOff:
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnFellowshipAttractionFallOff();
        }
        break;

	case ectQuestErrorCode:
        if (g_pGameWorldUIHandler)
		{
			KUIEventQuestRespond UIParam;
            UIParam.dwQuestID     = *(DWORD*)pResponseMessage->byData;
			UIParam.nQuestRespond = pResponseMessage->byCode;
			g_pGameWorldUIHandler->OnQuestRespond(UIParam);
		}
		break;

	case ectSkillErrorCode:
        if (g_pGameWorldUIHandler)
		{
			KUIEventSkillCastRespond UINotify;
			UINotify.nRespondCode = pResponseMessage->byCode;
			g_pGameWorldUIHandler->OnSkillCastRespond(UINotify);
		}
		break;

	case ectItemErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventItemUseRespond UINotify;
            UINotify.nRespondCode = pResponseMessage->byCode;
            g_pGameWorldUIHandler->OnItemUseRespond(UINotify);
        }
		break;

    case ectAddItemErrorCode:
        {
            KUIEventAddItemRespond UINotify;
            size_t uNameLen = nSize - sizeof(S2C_MESSAGE_NOTIFY);

            UINotify.nRespondCode = pResponseMessage->byCode;
            UINotify.szItemName[0] = '\0';

            if (uNameLen > 0)
            {
                pResponseMessage->byData[uNameLen - 1] = '\0';
                memcpy(UINotify.szItemName, pResponseMessage->byData, uNameLen);
            }

            g_pGameWorldUIHandler->OnAddItemRespond(UINotify);
        }
        break;

    case ectUseItemErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventUseItemRespond  UINotify;
            KUSE_ITEM               UseItemInfo;
            
            memcpy(&UseItemInfo, pResponseMessage->byData, sizeof(KUSE_ITEM));

            UINotify.nRespondCode           = pResponseMessage->byCode;
            UINotify.dwProfessionID         = UseItemInfo.dwProfessionID;
            UINotify.dwProfessionBranchID   = UseItemInfo.dwProfessionBranchID;
            UINotify.dwProfessionLevel      = UseItemInfo.dwProfessionLevel;

            g_pGameWorldUIHandler->OnUseItemRespond(UINotify);
        }
		break;

    case ectTradingErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingRespond UINofity;
			UINofity.nRespondCode = pResponseMessage->byCode;
			g_pGameWorldUIHandler->OnTradingRespond(UINofity);
        }
        break;

    case ectTalkErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventResponseMsg UIEventResponseMsg;
            UIEventResponseMsg.nRespondCode = pResponseMessage->byCode;
            g_pGameWorldUIHandler->OnResponseMsg(UIEventResponseMsg);
        }
        break;

	case ectLootItemErrorCode:
        if (g_pGameWorldUIHandler)
		{
			KUIEventLootRespond UIParam;
			UIParam.nRespondCode = pResponseMessage->byCode;
			g_pGameWorldUIHandler->OnLootRespond(UIParam);
		}
		break;

	case ectCraftErrorCode:
        {
            KPlayer* pPlayer = NULL;
            
            pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
            KGLOG_PROCESS_ERROR(pPlayer);
            
            if (pPlayer->m_OTActionParam.eActionType == otActionRecipePrepare) // 可能客户端还处于这个状态
            {
                pPlayer->DoActionIdle();
        
                if (g_pGameWorldRepresentHandler)
                {
                    g_pGameWorldRepresentHandler->OnCharacterEndCraftAnimation(
                        pPlayer, pPlayer->m_OTActionParam.ProfessionSkill.dwCraftID
                    );
                }
            }

            if (g_pGameWorldUIHandler)
		    {
			    KUIEventCraftCastRespond    UIParam;
			    KCraftCastState             CraftCastState;
    	
			    memcpy(&CraftCastState, pResponseMessage->byData, sizeof(KCraftCastState));

			    UIParam.nCraftCastRespond = pResponseMessage->byCode;
			    UIParam.dwCraftID = CraftCastState.dwCraftID;
			    UIParam.dwRecipeID = CraftCastState.dwRecipeID;
			    UIParam.dwTargetType = CraftCastState.eTargetType;
			    UIParam.dwTargetID = CraftCastState.dwTargetID;
			    g_pGameWorldUIHandler->OnCraftCastRespond(UIParam);
		    }
        }
        
		break;

    case ectMailErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond Respond;
            Respond.nRespondCode = pResponseMessage->byCode;
            g_pGameWorldUIHandler->OnMailRespond(Respond);
        }
        break;

    case ectTeamNotifyCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventTeamMessageNotify UIEvent;

            UIEvent.nCode       = pResponseMessage->byCode;
            UIEvent.szName[0]   = '\0';

            if (nSize > sizeof(S2C_MESSAGE_NOTIFY))
            {
                strncpy(UIEvent.szName, (char*)pResponseMessage->byData, sizeof(UIEvent.szName));
                UIEvent.szName[sizeof(UIEvent.szName) - 1] = '\0';
            }
            
            g_pGameWorldUIHandler->OnTeamMessageNotify(UIEvent);
        }
        break;

    case ectPeekOtherPlayerErrorCode:
        {
            DWORD* pdwID = NULL;

            KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_MESSAGE_NOTIFY) + sizeof(DWORD));

            pdwID = (DWORD*)&(pResponseMessage->byData);
            assert(pdwID);
            
            if (g_pGameWorldUIHandler)
            {
                KUIEventPeekOtherPlayer UIEvent;

                UIEvent.nResult     = pResponseMessage->byCode;
                UIEvent.dwPlayerID  = *pdwID;

                g_pGameWorldUIHandler->OnPeekOtherPlayer(UIEvent);
            }
        }
        break;

    case ectShopEventNotifyCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventShopRespond UIEvent = { ssrcInvalid, 0 };

            UIEvent.nRespondCode = pResponseMessage->byCode;

            if (pResponseMessage->byCode == ssrcRepairSuccess)
            {
                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_MESSAGE_NOTIFY) + sizeof(int));
                UIEvent.nMoney = *((int*)pResponseMessage->byData);
            }

            g_pGameWorldUIHandler->OnShopRespond(UIEvent);
        }
        break;
	case ectPKErrorCode:
        if (g_pGameWorldUIHandler)
		{
			//TODO:PK的出错提示
			KUIEventPKRespond UIEvent;

			UIEvent.nRespondCode = (BYTE)(pResponseMessage->byCode);

			g_pGameWorldUIHandler->OnPKRespond(UIEvent);
		}
		break;
    case ectFellowshipOnline:
        {
            DWORD                       dwAlliedPlayerID    = ERROR_ID;
            KGFellowship*               pFellowship         = NULL;
            KUIEventFellowshipNotify    UIEvent;

            dwAlliedPlayerID = *(DWORD*)pResponseMessage->byData;

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(g_pSO3World->m_dwClientPlayerID, dwAlliedPlayerID);
            if (pFellowship)
            {
                if (g_pGameWorldUIHandler)
                {
                    UIEvent.bLoad   = (bool)pResponseMessage->byCode;
                    UIEvent.bFoe    = false;

                    strncpy(UIEvent.szName, pFellowship->m_szName, sizeof(UIEvent.szName));
                    UIEvent.szName[sizeof(UIEvent.szName) - sizeof('\0')] = '\0';

                    g_pGameWorldUIHandler->OnFellowshipLogin(UIEvent);
                }
            }
        }
        break;
    case ectFoeOnline:
        {
            DWORD                       dwAlliedPlayerID    = ERROR_ID;
            KGFoe*                      pFoe                = NULL;
            KUIEventFellowshipNotify    UIEvent;

            dwAlliedPlayerID = *(DWORD*)pResponseMessage->byData;

            pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(g_pSO3World->m_dwClientPlayerID, dwAlliedPlayerID);
            if (pFoe)
            {
                if (g_pGameWorldUIHandler)
                {
                    UIEvent.bLoad   = (bool)pResponseMessage->byCode;
                    UIEvent.bFoe    = true;

                    strncpy(UIEvent.szName, pFoe->m_szName, sizeof(UIEvent.szName));
                    UIEvent.szName[sizeof(UIEvent.szName) - sizeof('\0')] = '\0';

                    g_pGameWorldUIHandler->OnFellowshipLogin(UIEvent);
                }
            }
        }
        break;
    case ectBanishNotifyCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventBanishPlayer UIEvent;
            int                  nLeftSeconds = 0;

            memcpy(&nLeftSeconds, pResponseMessage->byData, sizeof(nLeftSeconds));
            
            UIEvent.nLeftSeconds    = nLeftSeconds;
            UIEvent.nBanishCode     = pResponseMessage->byCode;

            g_pGameWorldUIHandler->OnBanishPlayer(UIEvent);
        }
        break;

    case ectRoadTrackErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventRoadTrackErrorID UIEvent;

            UIEvent.nErrorID = pResponseMessage->byCode;

            g_pGameWorldUIHandler->OnRoadTrackErrorID(UIEvent);
        }
        break;
    case ectSwitchMapErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventSwitchMapResult UIEvent;
            KPlayer*                pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
            KGLOG_PROCESS_ERROR(pPlayer);

            UIEvent.nErrorID = pResponseMessage->byCode;

            g_pGameWorldUIHandler->OnSwitchMapResult(UIEvent);

            if (pPlayer->m_eMoveState != cmsOnDeath)
            {
                pPlayer->m_nCurrentTrack = 0;
                pPlayer->Stop();
            }
        }
        break;
    case ectOpenVenationRetCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventOpenVenationRetCode UIEvent;

            UIEvent.bOpenVenationRetCode = pResponseMessage->byCode;
            UIEvent.nVenationID          = *((int*)pResponseMessage->byData);

            g_pGameWorldUIHandler->OnOpenVenationRetCode(UIEvent);
        }
        break;
    case ectShareQuestErrorCode:
        if (g_pGameWorldUIHandler)
        {
            KSHARE_QUEST_RESULT*        pShareQuestResult = (KSHARE_QUEST_RESULT*)pResponseMessage->byData;
            KUIEventShareQuestResult    KUIEvent;
            
            KGLOG_PROCESS_ERROR(pShareQuestResult);
            
            KUIEvent.dwQuestID      = pShareQuestResult->dwQuestID;
            KUIEvent.dwDestPlayerID = pShareQuestResult->dwTargetPlayerID;
            KUIEvent.nResultCode    = pResponseMessage->byCode;

            g_pGameWorldUIHandler->OnShareQuestResult(KUIEvent);
        }
        break;
    case ectSetQuestAssistDailyCount:
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
            KGLOG_PROCESS_ERROR(pPlayer);

            pPlayer->m_QuestList.SetAssistDailyCount(pResponseMessage->byCode);

            g_pGameWorldUIHandler->OnUpdateAssistDailyCount();
        }
        break;
    case ectFinishAssistQuest:
        {
            KASSIST_PRESENT_NOTIFY* pNotify = NULL;
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
            KGLOG_PROCESS_ERROR(pPlayer);

            KGLOG_PROCESS_ERROR(pResponseMessage->wSize == sizeof(S2C_MESSAGE_NOTIFY) + sizeof(KASSIST_PRESENT_NOTIFY));
            pNotify = (KASSIST_PRESENT_NOTIFY*)pResponseMessage->byData;
            assert(pNotify);

            if (g_pGameWorldUIHandler)
            {
                KUIEventFinishQuest Param;
                Param.dwQuestID     = pNotify->dwQuestID;
                Param.bForceFinish  = false;
                Param.bAssist       = true;
                Param.nAddStamina   = pNotify->nAddStamina;
                Param.nAddThew      = pNotify->nAddThew;
                g_pGameWorldUIHandler->OnFinishQuest(Param);
            }
            if (g_pGameWorldRepresentHandler)
            {
                g_pGameWorldRepresentHandler->OnFinishQuest(pPlayer, pNotify->dwQuestID, true);
            }
        }
        break;
    case ectAddSkillRecipe:
        {
            KG_ADD_SKILL_RECIPE_NOTIFY* pNotify = NULL;
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
            KGLOG_PROCESS_ERROR(pPlayer);

            KGLOG_PROCESS_ERROR(pResponseMessage->wSize == sizeof(S2C_MESSAGE_NOTIFY) + sizeof(KG_ADD_SKILL_RECIPE_NOTIFY));
            pNotify = (KG_ADD_SKILL_RECIPE_NOTIFY*)pResponseMessage->byData;
            assert(pNotify);

            if (g_pGameWorldUIHandler)
            {
                KUIAddSkillRecipe   Param;
                Param.dwRecipeID    = pNotify->dwRecipeID;
                Param.dwRecipeLevel = pNotify->dwRecipeLevel;
                g_pGameWorldUIHandler->OnAddSkillRecipe(Param);
            }
        }
        break;
    case ectCampResultCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventCampInfo Param = {pResponseMessage->byCode};
            g_pGameWorldUIHandler->OnCampInfoNotify(Param);
        }
        break;
    case ectChargeLimitCode:
        if (g_pGameWorldUIHandler)
        {
            KUIEventChargeLimitNotify Param = {pResponseMessage->byCode};
            g_pGameWorldUIHandler->OnChargeLimitNotify(Param);
        }
	default:
        break;
	}

Exit0:
    if (pTempItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pTempItem);
        pTempItem = NULL;
    }
	return;
}

// 同步玩家基本信息
void KPlayerClient::OnSyncPlayerBaseInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
	S2C_SYNC_PLAYER_BASE_INFO*  pPlayerBaseInfo     = (S2C_SYNC_PLAYER_BASE_INFO*)pbyData;
	KPlayer*                    pPlayer             = NULL;
	int                         nRegionX            = 0;
    int                         nRegionY            = 0;
    KScene*                     pScene              = NULL;
    int                         nMaxRegionCountX    = 0;
    int                         nMaxRegionCountY    = 0;

    KGLogPrintf(KGLOG_INFO, "Sync player base info !");

	g_pSO3World->m_dwClientPlayerID     = pPlayerBaseInfo->dwPlayerID;
	g_pSO3World->m_dwClientMapID        = pPlayerBaseInfo->dwMapID;
	g_pSO3World->m_nClientMapCopyIndex  = pPlayerBaseInfo->nMapCopyIndex;
	g_pSO3World->m_dwStartTime          = KG_GetTickCount();
    g_pSO3World->m_nGameLoop            = pPlayerBaseInfo->nFrame;
    g_pSO3World->m_nStartLoop           = pPlayerBaseInfo->nFrame;
    g_pSO3World->m_nBaseTime            = pPlayerBaseInfo->nBaseTime;
	m_nGSGameLoop                       = pPlayerBaseInfo->nFrame;

    bRetCode = InitStartTimeEvaluation(&m_StartTimeEvaluation);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		g_pGameWorldUIHandler->SetInLogin(TRUE);
		KMapParams* pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(g_pSO3World->m_dwClientMapID);
		KGLOG_PROCESS_ERROR(pMapParams);

		KUIEventSceneBeginLoad	paramBegin;
		strncpy(paramBegin.szPath, pMapParams->szResourceFilePath, sizeof(paramBegin.szPath));
		paramBegin.szPath[sizeof(paramBegin.szPath) - 1] = '\0';
		char *pszLastSep = strrchr(paramBegin.szPath, '\\');
		if (pszLastSep)
		{
			char *pszTemp = strrchr(pszLastSep, '/');
			if (pszTemp)
				pszLastSep = pszTemp;
			*pszLastSep = '\0';
		}
		g_pGameWorldUIHandler->OnSceneBeginLoad(paramBegin);
	}

	pPlayer = g_pSO3World->NewPlayer(pPlayerBaseInfo->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pScene = g_pSO3World->NewClientScene(pPlayerBaseInfo->dwMapID, pPlayerBaseInfo->nMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	{
		KMapParams* pMapParams = NULL;

		pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(pScene->m_dwMapID);
		KGLOG_PROCESS_ERROR(pMapParams);

        pScene->m_nBroadcastRegion = pMapParams->nBroadcast;

		strncpy(pScene->m_szFilePath, pMapParams->szResourceFilePath, sizeof(pScene->m_szFilePath));
		pScene->m_szFilePath[sizeof(pScene->m_szFilePath) - 1] = '\0';

		if (g_pGameWorldRepresentHandler)
            g_pGameWorldRepresentHandler->OnNewScene(pScene, pPlayerBaseInfo->nX, pPlayerBaseInfo->nY, pPlayerBaseInfo->nZ, false);

        if (g_pGameWorldUIHandler)
        {
            KUIEventSceneEndLoad paramEnd = { pScene->m_dwID };
            g_pGameWorldUIHandler->OnSceneEndLoad(paramEnd);
        }
	}

    nMaxRegionCountX = pScene->GetRegionWidth();
    nMaxRegionCountY = pScene->GetRegionHeight();

	nRegionX = pPlayerBaseInfo->nX / CELL_LENGTH / REGION_GRID_WIDTH;
	nRegionY = pPlayerBaseInfo->nY / CELL_LENGTH / REGION_GRID_HEIGHT;

    bRetCode = pScene->ValidateRegions(nRegionX, nRegionY);
    KGLOG_PROCESS_ERROR(bRetCode);

	strncpy(pPlayer->m_szName, pPlayerBaseInfo->szRoleName, _NAME_LEN);
	pPlayer->m_szName[_NAME_LEN - 1] = 0;

    pPlayer->m_nFaceDirection    = pPlayerBaseInfo->byFaceDirection;
    pPlayer->m_eRoleType         = (ROLE_TYPE)pPlayerBaseInfo->byRoleType;

    pPlayer->SetGenderByRoleType(pPlayerBaseInfo->byRoleType);
    
    memcpy(pPlayer->m_wRepresentId, pPlayerBaseInfo->wRepresentId, sizeof(pPlayer->m_wRepresentId));

	pPlayer->SetLevel(pPlayerBaseInfo->byLevel);

    pPlayer->m_eCamp = (KCAMP)pPlayerBaseInfo->byCamp;
    
    pPlayer->SetForceID(pPlayerBaseInfo->byForceID);

	pPlayer->m_eGameStatus = gsPlaying;

    pPlayer->m_nX               = pPlayerBaseInfo->nX;
    pPlayer->m_nY               = pPlayerBaseInfo->nY;
    pPlayer->m_nZ               = pPlayerBaseInfo->nZ;
    pPlayer->m_dwTongID         = pPlayerBaseInfo->dwTongID;
    pPlayer->m_nBattleFieldSide = pPlayerBaseInfo->nBattleFieldSide;

    if (g_pGameWorldUIHandler)
    {
	    g_pGameWorldUIHandler->OnSyncRoleDataBegin();
    }

	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnResetFrameData(g_pSO3World->m_nStartLoop, g_pSO3World->m_dwStartTime);
    }

    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "Sync base info: load scene %d !", bResult);

    if (!bResult)
    {
	    if (pPlayer)
        {
		    g_pSO3World->DelPlayer(pPlayer);
        }

	    if (pScene)
        {
		    g_pSO3World->DeleteScene(pScene);
        }

        Disconnect();
    }

	return;
}

void KPlayerClient::OnSyncQuestData(BYTE* pbyData, size_t uSize, int nFrame)
{
    BOOL                    bRetCode        = false;
    KPlayer*                pPlayer         = NULL;
    S2C_SYNC_QUEST_DATA*    pSyncQuestData  = NULL;
    size_t                  uUsedSize       = 0;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(uSize >= sizeof(S2C_SYNC_QUEST_DATA));
    pSyncQuestData = (S2C_SYNC_QUEST_DATA*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncQuestData->dwDestPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);
    
    switch(pSyncQuestData->byQuestDataType)
    {
    case qdtQuestState:
        bRetCode = pPlayer->m_QuestList.LoadQuestState(&uUsedSize, pSyncQuestData->byData, uSize - sizeof(S2C_SYNC_QUEST_DATA));
	    KGLOG_PROCESS_ERROR(bRetCode);
    	break;
    case qdtQuestList:
        bRetCode = pPlayer->m_QuestList.LoadQuestList(&uUsedSize, pSyncQuestData->byData, uSize - sizeof(S2C_SYNC_QUEST_DATA));
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        if (g_pSO3World->m_dwClientPlayerID != pSyncQuestData->dwDestPlayerID) // 查看别人的任务
        {
            if (g_pGameWorldUIHandler)
            {
                KUIEventPeekPlayerQuest UIParam;
		        UIParam.dwPlayerID = pSyncQuestData->dwDestPlayerID;
		        g_pGameWorldUIHandler->OnPeekPlayerQuest(UIParam);
            }
        }

    	break;
    case qdtDailyQuest:
        bRetCode = pPlayer->m_QuestList.LoadDailyQuest(&uUsedSize, pSyncQuestData->byData, uSize - sizeof(S2C_SYNC_QUEST_DATA));
	    KGLOG_PROCESS_ERROR(bRetCode);
        break;
    default:
        break;
    }
	
    if (g_pGameWorldUIHandler)
    {
	    g_pGameWorldUIHandler->OnQuestListUpdate();
    }
Exit0:
	return;
}

void KPlayerClient::OnAccountKickout(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                        bRetCode        = false;
    KScene*                     pScene          = NULL;

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnPlayerKickAccount();

	Disconnect();

	pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

    bRetCode = g_pSO3World->DeleteScene(pScene);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerClient::OnSwitchGS(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL            bRetCode            = false;
    char*           pszGameServerIP     = NULL;
    KScene*         pScene              = NULL;
	S2C_SWITCH_GS   ChangeGameServer;
	in_addr         Addr;

	memcpy(&ChangeGameServer, pbyData, sizeof(S2C_SWITCH_GS));

	Addr.s_addr = ChangeGameServer.dwIPAddr;

	pszGameServerIP = inet_ntoa(Addr);
    KGLOG_PROCESS_ERROR(pszGameServerIP);

    // 注意: 删除场景有时候会耗时很长,可能造成服务器等待超时
    // 所以,先DoHandshakeRequest,再删除旧场景
	pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	Disconnect();

    g_pSO3World->m_TeamClient.Reset();
    g_pSO3World->m_FellowshipMgr.Reset();
    g_pSO3World->m_ShopCenter.ClearShop();

    g_pSO3World->m_dwClientPlayerID     = ERROR_ID;
    g_pSO3World->m_nGameLoop            = 0;
    g_pSO3World->m_dwStartTime          = 0;
    g_pSO3World->m_nStartLoop           = 0;

	bRetCode = Connect(pszGameServerIP, ChangeGameServer.wPort);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = DoHandshakeRequest(ChangeGameServer.dwPlayerID, ChangeGameServer.Guid);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_pSO3World->DeleteScene(pScene);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    g_pSO3World->m_dwClientMapID        = ERROR_ID; // DeleteScene 需要这个参数
    g_pSO3World->m_nClientMapCopyIndex  = 0;

	return;
}

void KPlayerClient::OnSwitchMap(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL            bResult              = false;
    BOOL            bRetCode             = false;
    S2C_SWITCH_MAP* pSwitchMap           = (S2C_SWITCH_MAP*)pbyData;
    KPlayer*        pClientPlayer        = NULL;
    KScene*         pFromScene           = NULL;
    KScene*         pToScene             = NULL;
    int             nRegionX             = 0;
	int             nRegionY             = 0;

	if (g_pGameWorldUIHandler)
	{
		KUIEventSceneBeginLoad	paramBegin;
		KMapParams* pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(pSwitchMap->dwMapID);
		KGLOG_PROCESS_ERROR(pMapParams);
		
		strncpy(paramBegin.szPath, pMapParams->szResourceFilePath, sizeof(paramBegin.szPath));
		paramBegin.szPath[sizeof(paramBegin.szPath) - 1] = '\0';
		char *pszLastSep = strrchr(paramBegin.szPath, '\\');
		if (pszLastSep)
		{
			char *pszTemp = strrchr(pszLastSep, '/');
			if (pszTemp)
				pszLastSep = pszTemp;
			*pszLastSep = '\0';
		}
		g_pGameWorldUIHandler->OnSceneBeginLoad(paramBegin);
	}

	pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pClientPlayer);

    pFromScene = pClientPlayer->m_pScene;
    KGLOG_PROCESS_ERROR(pFromScene);

	bRetCode = g_pSO3World->RemovePlayer(pClientPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_pSO3World->DeleteScene(pFromScene);
	KGLOG_PROCESS_ERROR(bRetCode);

    pFromScene = NULL;

	g_pSO3World->m_dwClientMapID        = pSwitchMap->dwMapID;
	g_pSO3World->m_nClientMapCopyIndex  = pSwitchMap->nCopyIndex;

	pToScene = g_pSO3World->NewClientScene(pSwitchMap->dwMapID, pSwitchMap->nCopyIndex);
	KGLOG_PROCESS_ERROR(pToScene);

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnNewScene(pToScene, pSwitchMap->nX, pSwitchMap->nY, pSwitchMap->nZ, false);

    if (g_pGameWorldUIHandler)
    {
        KUIEventSceneEndLoad param = { pToScene->m_dwID };
        g_pGameWorldUIHandler->OnSceneEndLoad(param);
    }

	nRegionX = pSwitchMap->nX / CELL_LENGTH / REGION_GRID_WIDTH;
	nRegionY = pSwitchMap->nY / CELL_LENGTH / REGION_GRID_HEIGHT;

    bRetCode = pToScene->ValidateRegions(nRegionX, nRegionY);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_pSO3World->AddPlayer(
        pClientPlayer, pToScene, pSwitchMap->nX,
        pSwitchMap->nY, pSwitchMap->nZ
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	assert(pClientPlayer->m_pRegion);

    pClientPlayer->m_MoveCtrl.bMove = false;
    pClientPlayer->m_MoveCtrl.nTurn = 0;

    pClientPlayer->m_nBattleFieldSide = pSwitchMap->nBattleFieldSide;

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pToScene)
        {
            g_pSO3World->DeleteScene(pToScene);
            pToScene = NULL;
        }
        Disconnect();
    }
    return;
}

void KPlayerClient::OnSyncNewPlayer(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bResult                 = false;
	BOOL                            bRetCode                = false;
	S2C_SYNC_NEW_PLAYER*            pPlayerData             = (S2C_SYNC_NEW_PLAYER*)pbyData;
    KScene*                         pScene                  = NULL;
    KRegion*                        pRegion                 = NULL;
    KPlayer*                        pPlayer                 = NULL;
    KPlayer*                        pClientPlayer           = NULL;
    S2C_PLAYER_DISPLAY_DATA*        pPlayerDisplayData      = NULL;
    DWORD                           dwCheckSum              = 0;
    DWORD                           dwDisplayDataSize       = 0;
    KPOSITION                       PlayPos;

    KGLOG_PROCESS_ERROR(pPlayerData->dwID != g_pSO3World->m_dwClientPlayerID);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPlayerData->dwID);
	if (pPlayer)
	{
		g_pSO3World->DelPlayer(pPlayer);
	}

	pPlayer = g_pSO3World->NewPlayer(pPlayerData->dwID);
	KGLOG_PROCESS_ERROR(pPlayer);

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    memcpy(pPlayer->m_byRepresentState, pPlayerData->byRepresentState, sizeof(pPlayer->m_byRepresentState));

    pPlayer->m_nFaceDirection               = pPlayerData->byFaceDirection;
    pPlayer->m_nPitchDirection              = pPlayerData->byPitchDirection;
    pPlayer->m_nMoveFrameCounter            = pPlayerData->byMoveFrameCount;

	pPlayer->SetMoveState(pPlayerData->uMoveState);

    pPlayer->m_bFightState                  = pPlayerData->uFightState;
    pPlayer->m_bSheathFlag                  = pPlayerData->uSheathFlag;

    pPlayer->m_nConvergenceSpeed            = pPlayerData->uConvergenceSpeed;
    pPlayer->m_nDirectionXY                 = pPlayerData->uDirectionXY;
    pPlayer->m_nVelocityZ                   = pPlayerData->nVelocityZ;
    pPlayer->m_nDestX                       = pPlayerData->uDestX;
    pPlayer->m_nDestY                       = pPlayerData->uDestY;
    pPlayer->m_bOnHorse                     = pPlayerData->uOnHorse;
    pPlayer->m_bTryPathState                = pPlayerData->uTryPathState;
    pPlayer->m_nTryPathSide                 = pPlayerData->uTryPathSide;

    pPlayer->m_nCurrentGravity              = pPlayerData->uCurrentGravity;
	pPlayer->m_nVelocityXY                  = pPlayerData->uVelocityXY;	
    pPlayer->m_bRedName                     = pPlayerData->uRedName;
    pPlayer->m_nRunSpeed                    = pPlayerData->uRunSpeed;
    pPlayer->m_MoveCtrl.bDisable            = pPlayerData->uMoveCtrlDisable;
    pPlayer->m_MoveCtrl.bMove               = pPlayerData->uMoveCtrlMoving;
    pPlayer->m_MoveCtrl.bWalk               = pPlayerData->uMoveCtrlWalk;
    pPlayer->m_MoveCtrl.nDirection8         = pPlayerData->uMoveCtrlDirection8;
    pPlayer->m_MoveCtrl.nTurn               = pPlayerData->nMoveCtrlTurn;

    pPlayer->m_bSlip                        = pPlayerData->uSlipFlag;
    pPlayer->m_nWaterFlyAbility             = pPlayerData->uWaterFlyAbility;
    pPlayer->m_bCampFlag                    = pPlayerData->uCampFlag;

    if (pPlayerData->uMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = NULL;
        pPlayer->m_nCurrentTrack            = pPlayerData->uPosX;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pPlayer->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        pPlayer->m_nFromNode                = pTrack->nFromNode;
        if (pPlayerData->uPosY)
        {
            pPlayer->m_nFromNode            = pTrack->nToNode;
        }

        pPlayer->m_nMoveFrameCounter        = pPlayerData->uPosZ;

        bRetCode = g_pSO3World->m_RoadManager.GetPosition(&PlayPos, pPlayer->m_nCurrentTrack, pPlayer->m_nMoveFrameCounter, pPlayer->m_nFromNode);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        PlayPos.nX = pPlayerData->uPosX;
        PlayPos.nY = pPlayerData->uPosY;
        PlayPos.nZ = pPlayerData->uPosZ;
    }

    if (pPlayer->m_nMaxLife <= 0)
        pPlayer->m_nMaxLife = UCHAR_MAX;
    pPlayer->m_nCurrentLife = pPlayer->m_nMaxLife * pPlayerData->byLifePercent / UCHAR_MAX;

	pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	pRegion = pScene->GetRegionByPoint(PlayPos.nX, PlayPos.nY);
	KG_PROCESS_ERROR(pRegion && pRegion->IsActive());

    bRetCode = g_pSO3World->AddPlayer(pPlayer, pScene, PlayPos.nX, PlayPos.nY, PlayPos.nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_eGameStatus                  = gsPlaying;

    pPlayerDisplayData = (S2C_PLAYER_DISPLAY_DATA*)m_PlayerDisplayDataCache.GetData(pPlayer->m_dwID, dwDisplayDataSize);
    if (pPlayerDisplayData)
    {
		pPlayer->SetDisplayData(*pPlayerDisplayData);  
        dwCheckSum = CRC32(0, pPlayerDisplayData, sizeof(S2C_PLAYER_DISPLAY_DATA));
    }
    
	bRetCode = DoApplyPlayerDisplayData(pPlayer, dwCheckSum);
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_TeamClient.IsPlayerInTeam(pClientPlayer->m_dwID);
    if (bRetCode)
    {
        bRetCode = DoApplyPlayerBuffList(pPlayer->m_dwID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    if (pPlayerData->uPK)
    {
        pPlayer->m_PK.SetState(pksSlaying);
    }
    
    if (pPlayer->m_bRedName && g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pPlayer);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pPlayer)
        {
            g_pSO3World->DelPlayer(pPlayer);
            pPlayer = NULL;
        }
    }
	return;
}

void KPlayerClient::OnSyncNewNpc(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                         bResult                = false;
	BOOL                         bRetCode               = false;
	S2C_SYNC_NEW_NPC*            pNpcData               = (S2C_SYNC_NEW_NPC*)pbyData;
    KScene*                      pScene                 = NULL;
    KRegion*                     pRegion                = NULL;
    KNpc*                        pNpc                   = NULL;
    S2C_NPC_DISPLAY_DATA*        pNpcDisplayData        = NULL;
    DWORD                        dwCheckSum             = 0;
    DWORD                        dwDisplayDataSize      = 0;
    KPOSITION                    NpcPos;

	pNpc = g_pSO3World->m_NpcSet.GetObj(pNpcData->dwID);
	if (pNpc)
	{
		g_pSO3World->DeleteNpc(pNpc);
	}

	pNpc = g_pSO3World->NewNpc(pNpcData->dwID);
	KGLOG_PROCESS_ERROR(pNpc);

    bRetCode = g_pSO3World->m_Settings.m_NpcTemplateList.LoadFromTemplate(pNpcData->dwTemplateID, pNpc);
    KGLOG_PROCESS_ERROR(bRetCode);

    memcpy(pNpc->m_byRepresentState, pNpcData->byRepresentState, sizeof(pNpc->m_byRepresentState));

    pNpc->m_nFaceDirection               = pNpcData->byFaceDirection;
    pNpc->m_nLevel                       = pNpcData->byLevel;
    pNpc->m_dwModelID                    = pNpcData->wModelID;

	pNpc->SetMoveState(pNpcData->uMoveState);

    pNpc->m_bFightState                  = pNpcData->uFightState;
    pNpc->m_bSheathFlag                  = pNpcData->uSheathFlag;
    pNpc->m_bTryPathState                = pNpcData->uTryPathState;

	pNpc->m_nVelocityXY                  = pNpcData->uVelocityXY;
    pNpc->m_nConvergenceSpeed            = pNpcData->uConvergenceSpeed;
    pNpc->m_nVelocityZ                   = pNpcData->nVelocityZ;
    pNpc->m_nDestX                       = pNpcData->uDestX;
    pNpc->m_nDestY                       = pNpcData->uDestY;

    pNpc->m_nDirectionXY                 = pNpcData->uDirectionXY;
    pNpc->m_nTryPathSide                 = pNpcData->uTryPathSide;
    pNpc->m_nCurrentGravity              = pNpcData->uCurrentGravity;
    pNpc->m_bSlip                        = pNpcData->uSlipFlag;
    pNpc->m_bDialogFlag                  = pNpcData->uDialogFlag;

    if (pNpc->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = NULL;

        pNpc->m_nCurrentTrack            = pNpcData->uPosX;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pNpc->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        pNpc->m_nFromNode                = pTrack->nFromNode;
        if (pNpcData->uPosY)
        {
            pNpc->m_nFromNode            = pTrack->nToNode;
        }

        pNpc->m_nMoveFrameCounter        = pNpcData->uPosZ;

        bRetCode = g_pSO3World->m_RoadManager.GetPosition(&NpcPos, pNpc->m_nCurrentTrack, pNpc->m_nMoveFrameCounter, pNpc->m_nFromNode);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        NpcPos.nX = pNpcData->uPosX;
        NpcPos.nY = pNpcData->uPosY;
        NpcPos.nZ = pNpcData->uPosZ;
    }

    if (pNpc->m_nMaxLife <= 0)
        pNpc->m_nMaxLife = UCHAR_MAX;
    pNpc->m_nCurrentLife = pNpc->m_nMaxLife * pNpcData->byLifePercent / UCHAR_MAX;
    
	pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	pRegion = pScene->GetRegionByPoint(NpcPos.nX, NpcPos.nY);
	KG_PROCESS_ERROR(pRegion && pRegion->IsActive());

	bRetCode = g_pSO3World->AddNpc(pNpc, pScene, NpcPos.nX, NpcPos.nY, NpcPos.nZ);
	KGLOG_PROCESS_ERROR(bRetCode);

    pNpc->SetForceID(pNpcData->byForceID);

	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterDisplayData(pNpc);

    if (g_pGameWorldUIHandler)
	{
		KUIEventNpcDisplayDataUpdate Param = { pNpcData->dwID };
		g_pGameWorldUIHandler->OnNpcDisplayDataUpdate(Param);
	}

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pNpc)
        {
            g_pSO3World->DeleteNpc(pNpc);
            pNpc = NULL;
        }
    }
	return;
}

void KPlayerClient::OnSyncNewDoodad(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                     bResult                 = false;
	BOOL                                     bRetCode                = false;
	KDoodad*                                 pDoodad                 = NULL;
	S2C_SYNC_NEW_DOODAD*                     pDoodadData             = (S2C_SYNC_NEW_DOODAD*)pbyData;
    KScene*                                  pScene                  = NULL;
	KRegion*                                 pRegion                 = NULL;
    int                                      nSyncCount              = 0;
    int                                      nFrameDiff              = g_pSO3World->m_nGameLoop - nFrame;      

	pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	pRegion   = pScene->GetRegionByPoint(pDoodadData->uPosX, pDoodadData->uPosY);
	KG_PROCESS_ERROR(pRegion && pRegion->IsActive());

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(pDoodadData->dwID);
	if (pDoodad)
	{
		g_pSO3World->DelDoodad(pDoodad);
	}

	pDoodad = g_pSO3World->NewDoodad(pDoodadData->dwID);
	KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = g_pSO3World->m_Settings.m_DoodadTemplateList.LoadFromTemplate(pDoodadData->dwTemplateID, pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_eKind            = (DOODAD_KIND)pDoodadData->cKind;
	pDoodad->m_dwNpcTemplateID  = pDoodadData->dwNpcTemplateID;
    pDoodad->m_dwNpcID          = pDoodadData->dwNpcID;
	pDoodad->m_nFaceDirection   = pDoodadData->byFaceDirection;
	pDoodad->m_bLooted		    = pDoodadData->bLooted;
	pDoodad->m_dwRepresentID    = pDoodadData->dwRepresentID;
    pDoodad->m_nObstacleGroup   = pDoodadData->nObstacleGroup;

	if (pDoodadData->dwNpcTemplateID != ERROR_ID)
	{
		KNpcTemplate* pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pDoodadData->dwNpcTemplateID);

		KGLOG_PROCESS_ERROR(pNpcTemplate); 

		strncpy(pDoodad->m_szName, pNpcTemplate->szName, _NAME_LEN);
        pDoodad->m_szName[sizeof(pDoodad->m_szName) - 1] = '\0';
	}

	bRetCode = g_pSO3World->AddDoodad(pDoodad, pScene, pDoodadData->uPosX, pDoodadData->uPosY, pDoodadData->uPosZ);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    if (pDoodad->m_eKind == dkDoor && pDoodad->m_nObstacleGroup != INVALID_OBSTACLE_GROUP)
    {
        bRetCode = pScene->SetDynamicObstacleState(pDoodad->m_nObstacleGroup, pDoodadData->bObstacleState);
	    KGLOG_PROCESS_ERROR(bRetCode);
    }

    pDoodad->ChangeState((DOODAD_STATE)pDoodadData->cState);

	DoApplyLootList(pDoodadData->dwID);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pDoodad)
        {
            g_pSO3World->DelDoodad(pDoodad);
            pDoodad = NULL;
        }
    }
	return;
}

// 从场景中移除角色
void KPlayerClient::OnRemoveCharacter(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode            = false;
	S2C_REMOVE_CHARACTER*   pRemoveCharacter    = (S2C_REMOVE_CHARACTER*)pbyData;
	DWORD                   dwCharacterID       = pRemoveCharacter->dwCharacterID;

	KG_PROCESS_ERROR(dwCharacterID != g_pSO3World->m_dwClientPlayerID);

	if (IS_PLAYER(dwCharacterID))
	{
		KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);
		KGLOG_PROCESS_ERROR(pPlayer);
		g_pSO3World->DelPlayer(pPlayer);
	}
	else
	{
		KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
		KGLOG_PROCESS_ERROR(pNpc);
		g_pSO3World->DeleteNpc(pNpc, pRemoveCharacter->bKilled);
	}

Exit0:
	return;
}

void KPlayerClient::OnRemoveDoodad(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                bRetCode         = false;
	S2C_REMOVE_DOODAD*  pRemoveDoodad    = (S2C_REMOVE_DOODAD*)pbyData;
    KDoodad*            pDoodad          = NULL;

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pRemoveDoodad->dwID);
	KG_PROCESS_ERROR(pDoodad);

	g_pSO3World->DelDoodad(pDoodad);

Exit0:
	return;
}

void KPlayerClient::OnSyncMoveCtrl(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                bRetCode    = false;
    S2C_SYNC_MOVE_CTRL* pPak        = (S2C_SYNC_MOVE_CTRL*)pbyData;
    KPlayer*            pPlayer     = NULL;
    KPOSITION           Pos;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_nMoveFrameCounter        = pPak->byMoveFrameCount;
    pPlayer->m_nFaceDirection           = pPak->byFaceDirection;
    pPlayer->m_nPitchDirection          = pPak->byPitchDirection;

    pPlayer->m_MoveCtrl.bDisable        = pPak->uMoveCtrlDisable;
    pPlayer->m_MoveCtrl.bMove           = pPak->uMoveCtrlMoving;
    pPlayer->m_MoveCtrl.bWalk           = pPak->uMoveCtrlWalk;
    pPlayer->m_MoveCtrl.nDirection8     = pPak->uMoveCtrlDirection8;
    pPlayer->m_MoveCtrl.nTurn           = pPak->nMoveCtrlTurn;

    pPlayer->m_nVelocityXY              = pPak->uVelocityXY;
    pPlayer->m_nVelocityZ               = pPak->nVelocityZ;
    pPlayer->m_nDirectionXY             = pPak->uDirectionXY;
    pPlayer->m_nConvergenceSpeed        = pPak->uConvergenceSpeed;
    pPlayer->m_nCurrentGravity          = pPak->uCurrentGravity;
    pPlayer->m_bTryPathState            = pPak->uTryPathState;
    pPlayer->m_nTryPathSide             = pPak->uTryPathSide;
    pPlayer->m_bSlip                    = pPak->uSlipFlag;
    pPlayer->m_nRunSpeed                = pPak->uRunSpeed;

    pPlayer->SetMoveState(pPak->uMoveState);

    if (pPak->uMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = NULL;

        pPlayer->m_nCurrentTrack            = pPak->uPosX;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pPlayer->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        pPlayer->m_nFromNode                = pTrack->nFromNode;

        if (pPak->uPosY)
        {
            pPlayer->m_nFromNode            = pTrack->nToNode;
        }

        pPlayer->m_nMoveFrameCounter        = pPak->uPosZ;

        bRetCode = g_pSO3World->m_RoadManager.GetPosition(
            &Pos, pPlayer->m_nCurrentTrack, 
            pPlayer->m_nMoveFrameCounter, pPlayer->m_nFromNode
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        Pos.nX = pPak->uPosX;
        Pos.nY = pPak->uPosY;
        Pos.nZ = pPak->uPosZ;
    }

    bRetCode = pPlayer->MoveTo(Pos.nX, Pos.nY, Pos.nZ, true, false);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerClient::OnSyncMoveParam(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    S2C_SYNC_MOVE_PARAM*        pPak        = (S2C_SYNC_MOVE_PARAM*)pbyData;
    KCharacter*                 pCharacter  = NULL;
    KPOSITION                   Pos;

    if (IS_PLAYER(pPak->dwID))
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pPak->dwID);
    else
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pPak->dwID);
    KG_PROCESS_ERROR(pCharacter);
    
    pCharacter->m_nFaceDirection           = pPak->byFaceDirection;
    pCharacter->m_nPitchDirection          = pPak->byPitchDirection;
    pCharacter->m_nMoveFrameCounter        = pPak->byMoveFrameCount;

    pCharacter->m_bTryPathState            = pPak->uTryPathState;
    pCharacter->m_nTryPathSide             = pPak->uTryPathSide;

    pCharacter->m_nVelocityXY              = pPak->uVelocityXY;
    pCharacter->m_nRunSpeed                = pPak->uRunSpeed;
    pCharacter->m_nVelocityZ               = pPak->nVelocityZ;
    pCharacter->m_nDestX                   = pPak->uDestX;
    pCharacter->m_nDestY                   = pPak->uDestY;

    pCharacter->m_nConvergenceSpeed        = pPak->uConvergenceSpeed;
    pCharacter->m_nDirectionXY             = pPak->uDirectionXY;
    pCharacter->m_nCurrentGravity          = pPak->uCurrentGravity;
    pCharacter->m_bSlip                    = pPak->uSlipFlag;

    pCharacter->SetMoveState(pPak->uMoveState);

    if (pPak->uMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = NULL;

        pCharacter->m_nCurrentTrack            = pPak->uPosX;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pCharacter->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        pCharacter->m_nFromNode                = pTrack->nFromNode;

        if (pPak->uPosY)
        {
            pCharacter->m_nFromNode            = pTrack->nToNode;
        }

        pCharacter->m_nMoveFrameCounter        = pPak->uPosZ;

        bRetCode = g_pSO3World->m_RoadManager.GetPosition(
            &Pos, pCharacter->m_nCurrentTrack, 
            pCharacter->m_nMoveFrameCounter, pCharacter->m_nFromNode
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        Pos.nX = pPak->uPosX;
        Pos.nY = pPak->uPosY;
        Pos.nZ = pPak->uPosZ;
    }

    bRetCode = pCharacter->MoveTo(Pos.nX, Pos.nY, Pos.nZ, true, false);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}


void KPlayerClient::OnAdjustPlayerMove(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    S2C_ADJUST_PLAYER_MOVE*     pPak        = (S2C_ADJUST_PLAYER_MOVE*)pbyData;
    KPlayer*                    pPlayer     = NULL;
    KPOSITION                   Pos;

    cprintf("Adjust: %d\n", pPak->nMoveCount);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pPak->nMoveCount < pPlayer->m_MoveCtrl.nMoveCount || pPlayer->m_pScene == NULL)
    {
        //KGLogPrintf(
        //    KGLOG_DEBUG, "[move] apply adjust(%d < %d)", 
        //    pPak->nMoveCount, pPlayer->m_MoveCtrl.nMoveCount
        //);

        DoApplyMoveAdjust();
        goto Exit0;
    }

    pPlayer->m_nJumpLimitFrame          = pPak->nJumpLimitFrame;
    pPlayer->m_nFaceDirection           = pPak->byFaceDirection;
    pPlayer->m_nPitchDirection          = pPak->byPitchDirection;
    pPlayer->m_nMoveFrameCounter        = pPak->byMoveFrameCount;

    pPlayer->m_bTryPathState            = pPak->uTryPathState;
    pPlayer->m_nTryPathSide             = pPak->uTryPathSide;

    pPlayer->m_nVelocityXY              = pPak->uVelocityXY;
    pPlayer->m_nRunSpeed                = pPak->uRunSpeed;
    pPlayer->m_nVelocityZ               = pPak->nVelocityZ;
    pPlayer->m_nDestX                   = pPak->uDestX;
    pPlayer->m_nDestY                   = pPak->uDestY;

    pPlayer->m_nConvergenceSpeed        = pPak->uConvergenceSpeed;
    pPlayer->m_nDirectionXY             = pPak->uDirectionXY;
    pPlayer->m_nCurrentGravity          = pPak->uCurrentGravity;
    pPlayer->m_bSlip                    = pPak->uSlipFlag;

    pPlayer->SetMoveState(pPak->uMoveState);

    if (pPak->uMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = NULL;

        pPlayer->m_nCurrentTrack            = pPak->uPosX;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pPlayer->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        pPlayer->m_nFromNode                = pTrack->nFromNode;

        if (pPak->uPosY)
        {
            pPlayer->m_nFromNode            = pTrack->nToNode;
        }

        pPlayer->m_nMoveFrameCounter        = pPak->uPosZ;

        bRetCode = g_pSO3World->m_RoadManager.GetPosition(
            &Pos, pPlayer->m_nCurrentTrack, 
            pPlayer->m_nMoveFrameCounter, pPlayer->m_nFromNode
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        Pos.nX = pPak->uPosX;
        Pos.nY = pPak->uPosY;
        Pos.nZ = pPak->uPosZ;
    }

    bRetCode = pPlayer->MoveTo(Pos.nX, Pos.nY, Pos.nZ, true, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pPak->nVirtualFrame < g_pSO3World->m_nGameLoop)
    {
        pPlayer->ForwardPosition(g_pSO3World->m_nGameLoop - pPak->nVirtualFrame);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncMoveState(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode        = false;
    S2C_SYNC_MOVE_STATE*        pPak            = (S2C_SYNC_MOVE_STATE*)pbyData;
    KCharacter*                 pCharacter      = NULL;
    KPOSITION                   Pos;
    BOOL                        bOldFightState  = false;

    //KGLogPrintf(KGLOG_DEBUG, "Adjust: %d,%d,%d,%d", pPak->byMoveState, pPak->nX, pPak->nY, pPak->nZ);

    if (IS_PLAYER(pPak->dwID))
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(pPak->dwID);
    else
        pCharacter = g_pSO3World->m_NpcSet.GetObj(pPak->dwID);
    KG_PROCESS_ERROR(pCharacter);
    
    bOldFightState = pCharacter->m_bFightState;

    if (pCharacter->m_nFaceDirection != pPak->byFaceDirection)
    {
        pCharacter->m_nFaceDirection           = pPak->byFaceDirection;
        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterTurnTo(pCharacter, true);
        }
    }
    pCharacter->m_nPitchDirection          = pPak->byPitchDirection;
    pCharacter->m_nMoveFrameCounter        = pPak->byMoveFrameCount;

    pCharacter->m_bFightState              = pPak->uFightState;
    pCharacter->m_bTryPathState            = pPak->uTryPathState;
    pCharacter->m_nTryPathSide             = pPak->uTryPathSide;

    pCharacter->m_nVelocityXY              = pPak->uVelocityXY;
    pCharacter->m_nVelocityZ               = pPak->nVelocityZ;
    pCharacter->m_nConvergenceSpeed        = pPak->uConvergenceSpeed;
    pCharacter->m_nDestX                   = pPak->uDestX;
    pCharacter->m_nDestY                   = pPak->uDestY;

    pCharacter->m_nDirectionXY             = pPak->uDirectionXY;
    pCharacter->m_nJumpCount               = pPak->uJumpCount;
    pCharacter->m_bSlip                    = pPak->uSlipFlag;

    pCharacter->SetMoveState(pPak->uMoveState);

    if (pPak->uMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = NULL;

        pCharacter->m_nCurrentTrack            = pPak->uPosX;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pCharacter->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        pCharacter->m_nFromNode                = pTrack->nFromNode;

        if (pPak->uPosY)
        {
            pCharacter->m_nFromNode            = pTrack->nToNode;
        }

        pCharacter->m_nMoveFrameCounter        = pPak->uPosZ;

        bRetCode = g_pSO3World->m_RoadManager.GetPosition(
            &Pos, pCharacter->m_nCurrentTrack, 
            pCharacter->m_nMoveFrameCounter, pCharacter->m_nFromNode
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        Pos.nX = pPak->uPosX;
        Pos.nY = pPak->uPosY;
        Pos.nZ = pPak->uPosZ;
    }

    bRetCode = pCharacter->MoveTo(Pos.nX, Pos.nY, Pos.nZ, true, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (bOldFightState != pCharacter->m_bFightState && g_pSO3World->m_dwClientPlayerID == pCharacter->m_dwID)
    {
        KUIEventFightHint param;
        KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

        param.bFight = pCharacter->m_bFightState;
        g_pGameWorldUIHandler->OnFightHint(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnCharacterDeath(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                     bRetCode        = false;
	S2C_CHARACTER_DEATH*     pMoveCommand    = (S2C_CHARACTER_DEATH*)pbyData;
	KCharacter*              pKiller         = NULL;
    KCharacter*              pCharacter      = NULL;

	if (IS_PLAYER(pMoveCommand->dwCharacterID))
		pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pMoveCommand->dwCharacterID);
	else
		pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pMoveCommand->dwCharacterID);

    KG_PROCESS_ERROR(pCharacter);

    bRetCode = pCharacter->MoveTo(pMoveCommand->uSrcX, pMoveCommand->uSrcY, pMoveCommand->uSrcZ, true, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    pCharacter->m_nFaceDirection          = pMoveCommand->uFaceDirection;
    pCharacter->m_nConvergenceSpeed       = 0;
    pCharacter->m_nVelocityXY             = 0;

    pCharacter->SetMoveState(cmsOnDeath);

    if (pCharacter->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        if (((KPlayer*)pCharacter)->m_pTradingBox)
        {
            DoTradingConfirm(false, 0);

            if (g_pGameWorldUIHandler)
            {
                KUIEventTradingClose TradingClose;
                g_pGameWorldUIHandler->OnTradingClose(TradingClose);

                KUIEventTradingRespond TradingRespond;
                TradingRespond.nRespondCode = trcTradingCancel;
                g_pGameWorldUIHandler->OnTradingRespond(TradingRespond);
            }

            ((KPlayer*)pCharacter)->m_pTradingBox->Cancel();
        }
    }

	if (IS_PLAYER(pMoveCommand->dwKillerID))
	{
		pKiller = g_pSO3World->m_PlayerSet.GetObj(pMoveCommand->dwKillerID);
	}
	else
	{
		pKiller = g_pSO3World->m_NpcSet.GetObj(pMoveCommand->dwKillerID);
	}

    if (g_pGameWorldUIHandler)
    {
        KUIEventCharacterDeath CharacterDeath;
        
        CharacterDeath.dwCharacterID = pMoveCommand->dwCharacterID;
	    CharacterDeath.szKiller[0]   = '\0';

	    if (pKiller)
	    {
		    strncpy(CharacterDeath.szKiller, pKiller->GetName(), sizeof(CharacterDeath.szKiller));
		    CharacterDeath.szKiller[sizeof(CharacterDeath.szKiller) - 1] = '\0';
	    }
        
        if (pMoveCommand->dwCharacterID == g_pSO3World->m_dwClientPlayerID)
        {
            g_pGameWorldUIHandler->OnClientPlayerDeath();
        }

        g_pGameWorldUIHandler->OnCharacterDeath(CharacterDeath);
    }

Exit0:
	return;
}

// 移动角色的位置
void KPlayerClient::OnMoveCharacter(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = false;
	S2C_MOVE_CHARACTER* pMoveCharacter  = (S2C_MOVE_CHARACTER*)pbyData;
    KCharacter*         pCharacter      = NULL;

    if (IS_PLAYER(pMoveCharacter->dwID))
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(pMoveCharacter->dwID);
    else
        pCharacter = g_pSO3World->m_NpcSet.GetObj(pMoveCharacter->dwID);

    KG_PROCESS_ERROR(pCharacter);

    pCharacter->m_nFaceDirection    = pMoveCharacter->byFaceDirection;
    pCharacter->m_nDirectionXY      = pMoveCharacter->byDirectionXY;
    pCharacter->m_nVelocityXY       = pMoveCharacter->uVelocityXY;
    pCharacter->m_nVelocityZ        = pMoveCharacter->nVelocityZ;
    pCharacter->m_nConvergenceSpeed = pMoveCharacter->uConvergenceSpeed;
    pCharacter->SetMoveState(pMoveCharacter->byMoveState);

    bRetCode = pCharacter->MoveTo(
        pMoveCharacter->nPosX, pMoveCharacter->nPosY, pMoveCharacter->nPosZ, 
        pMoveCharacter->bIgnoreBlock, false
    );
	KGLOG_PROCESS_ERROR(bRetCode);
 
Exit0:
	return;
}

//同步角色动作
void KPlayerClient::OnCharacterAction(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	S2C_CHARACTER_ACTION* pCharacterAction = (S2C_CHARACTER_ACTION *)pbyData;

	if (IS_PLAYER(pCharacterAction->dwSrcCharacterID))
	{
		KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(pCharacterAction->dwSrcCharacterID);
		KG_PROCESS_ERROR(pPlayer);

		bRetCode = pPlayer->Action(pCharacterAction->dwDestCharacterID, pCharacterAction->dwAction);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	else
	{
		KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(pCharacterAction->dwSrcCharacterID);
		KG_PROCESS_ERROR(pNpc);

		bRetCode = pNpc->Action(pCharacterAction->dwDestCharacterID, pCharacterAction->dwAction);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit0:
	return;
}

void KPlayerClient::OnOverTimeAction(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_OVER_TIME_ACTION* pAction       = (S2C_OVER_TIME_ACTION*)pbyData;
    KCharacter*           pCharacter    = NULL;
    KOT_ACTION_PARAM      Action;

    if (IS_PLAYER(pAction->dwCharacterID))
    {
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pAction->dwCharacterID);
    }
    else
    {
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pAction->dwCharacterID);
    }
    KGLOG_PROCESS_ERROR(pCharacter);

    memset(&Action, 0, sizeof(Action));

    Action.eActionType = (KOT_ACTION_TYPE)pAction->cActionType;
    Action.nEndFrame   = g_pSO3World->m_nGameLoop + (int)pAction->wDurationFrame;    

    if (Action.eActionType == otActionIdle)
    {
        // Idle必须走专门的接口
        pCharacter->DoActionIdle();
    }
    else
    {
        pCharacter->DoOTAction(Action);
    }

    // 通知表现逻辑:
    // ... ...

Exit0:
    return;
}

//角色显示数据
void KPlayerClient::OnPlayerDisplayData(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                            bRetCode            = false;
	S2C_PLAYER_DISPLAY_DATA*        pPlayerDisplayData  = (S2C_PLAYER_DISPLAY_DATA*)pbyData;
    KPlayer*                        pPlayer             = NULL;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPlayerDisplayData->dwPlayerID);
	if (pPlayer)
	{
        pPlayer->SetDisplayData(*pPlayerDisplayData); 
	}

	m_PlayerDisplayDataCache.AddData(
        pPlayerDisplayData->dwPlayerID, pPlayerDisplayData, sizeof(S2C_PLAYER_DISPLAY_DATA)
    );

	return;
}

void KPlayerClient::OnSyncExperience(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                        bRetCode        = false;
	S2C_SYNC_EXPERIENCE*        pSyncExperience = (S2C_SYNC_EXPERIENCE*)pbyData;
    KPlayer*                    pPlayer         = NULL;
    KUIEventExpLog              ExpLog;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_nExperience = pSyncExperience->nCurrentExp;

    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerExperienceUpdate param = { g_pSO3World->m_dwClientPlayerID };
        g_pGameWorldUIHandler->OnPlayerExperienceUpdate(param);

        ExpLog.dwPlayerID = pPlayer->m_dwID;
        ExpLog.nAddExp    = pSyncExperience->nDeltaExp;

        g_pGameWorldUIHandler->OnExpLog(ExpLog);
    }

Exit0:
	return;
}

void KPlayerClient::OnPlayerLevelUp(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_PLAYER_LEVEL_UP*                pLevelUp                    = (S2C_PLAYER_LEVEL_UP*)pbyData;
    KPlayer*                            pPlayer                     = NULL;
    KLevelUpData*                       pLevelUpDataFrom            = NULL;
    KLevelUpData*                       pLevelUpDataTo              = NULL;
    KUIEventPlayerLevelUpdate           PlayerLevelUpNotify;
    KUIEventPlayerLevelUpdate           param;
    KUIEventQuestDataUpdate             QuestDataUpdate;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pLevelUp->dwID);
	KG_PROCESS_ERROR(pPlayer);

    // 播放升级特效
    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pPlayer, REPRESENT_SFX_PLAYER_LEVEL_UP);

    pLevelUpDataFrom = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(pPlayer->m_eRoleType, pPlayer->m_nLevel);
    KGLOG_PROCESS_ERROR(pLevelUpDataFrom);

    pLevelUpDataTo = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(pPlayer->m_eRoleType, pLevelUp->nLevel);
    KGLOG_PROCESS_ERROR(pLevelUpDataTo);

    pPlayer->SetLevel(pLevelUp->nLevel);

    if (g_pGameWorldUIHandler)
    {
        PlayerLevelUpNotify.dwPlayerID = pLevelUp->dwID;
        g_pGameWorldUIHandler->OnPlayerLevelUp(PlayerLevelUpNotify);
    }

    KG_PROCESS_SUCCESS(pLevelUp->dwID != g_pSO3World->m_dwClientPlayerID);
    // 后面的操作只对客户端玩家进行

	pPlayer->m_QuestList.UpdateNpcQuestMark(-1);

    if (g_pGameWorldUIHandler)
    {
        param.dwPlayerID    = pLevelUp->dwID;
        param.nLevel        = pLevelUp->nLevel;
        param.nStrength     = pLevelUpDataTo->nStrength     - pLevelUpDataFrom->nStrength;
        param.nAgility      = pLevelUpDataTo->nAgility      - pLevelUpDataFrom->nAgility;
        param.nVigor        = pLevelUpDataTo->nVigor        - pLevelUpDataFrom->nVigor;
        param.nSpirit       = pLevelUpDataTo->nSpirit       - pLevelUpDataFrom->nSpirit;
        param.nSpunk        = pLevelUpDataTo->nSpunk        - pLevelUpDataFrom->nSpunk;
        param.nMaxLife      = pLevelUpDataTo->nMaxLife      - pLevelUpDataFrom->nMaxLife;
        param.nMaxMana      = pLevelUpDataTo->nMaxMana      - pLevelUpDataFrom->nMaxMana;
        param.nMaxStamina   = pLevelUpDataTo->nMaxStamina   - pLevelUpDataFrom->nMaxStamina;
        param.nMaxThew      = pLevelUpDataTo->nMaxThew      - pLevelUpDataFrom->nMaxThew;

        g_pGameWorldUIHandler->OnPlayerLevelUpdate(param);

        QuestDataUpdate.nQuestIndex = -1;
        QuestDataUpdate.eEventType  = qetUpdateAll;

        g_pGameWorldUIHandler->OnQuestDataUpdate(QuestDataUpdate);
    }

Exit1:
Exit0:
    return;
}

void KPlayerClient::OnCommonHealth(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_COMMON_HEALTH*          pPak        = (S2C_COMMON_HEALTH*)pbyData;
    KCharacter*                 pCharacter  = NULL;
    KUIEventCommonHealthLog     LogParam;
    KUIEventCommonHealthText    TextParam;

    if (IS_PLAYER(pPak->dwCharacterID))
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    else
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);

    KG_PROCESS_ERROR(pCharacter);

    if (g_pGameWorldUIHandler)
    {
        LogParam.dwCharacterID = pPak->dwCharacterID;
        LogParam.nDeltaLife    = pPak->nDeltaLife;

        g_pGameWorldUIHandler->OnCommonHealthLog(LogParam);

        TextParam.dwCharacterID = pPak->dwCharacterID;
        TextParam.nDeltaLife    = pPak->nDeltaLife;

        g_pGameWorldUIHandler->OnCommonHealthText(TextParam);
    }

Exit0:
    return;
}

// Doodad状态同步数据
void KPlayerClient::OnSyncDoodadState(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_SYNC_DOODAD_STATE*  pDoodadState    = (S2C_SYNC_DOODAD_STATE*)pbyData;
    KDoodad*                pDoodad         = NULL;
    KScene*                 pScene          = NULL;

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pDoodadState->dwID);
    KG_PROCESS_ERROR(pDoodad);

    pDoodad->ChangeState((DOODAD_STATE)pDoodadState->cState);
    
    pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    pScene->SetDynamicObstacleState(pDoodad->m_nObstacleGroup, pDoodadState->bObstacleState);

Exit0:
	return;
}

void KPlayerClient::OnFrameSignal(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_FRAME_SIGNAL*   pFrameSignal    = (S2C_FRAME_SIGNAL*)pbyData;

    m_nGSGameLoop = pFrameSignal->nFrame;
}

void KPlayerClient::OnPingSignal(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_PING_SIGNAL*    pEcho       = (S2C_PING_SIGNAL*)pbyData;
    DWORD               dwTimeNow   = KG_GetTickCount();

    m_dwPingEchoTime = dwTimeNow - pEcho->dwTime;
}

// ------------------- skill about ------------------------->

// 同步技能数据
void KPlayerClient::OnSyncSkillData(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                 bRetCode           = false;
	S2C_SYNC_SKILL_DATA* pSyncSkillData     = (S2C_SYNC_SKILL_DATA*)pbyData;
    KPlayer*             pPlayer            = NULL;
    DWORD                dwOldSkillLevel    = 0;
    KUIEventSkillUpdate  UIParam;

    KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_SYNC_SKILL_DATA));

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    dwOldSkillLevel = pPlayer->m_SkillList.GetSkillLevel(pSyncSkillData->dwSkillID);

    bRetCode = pPlayer->m_SkillList.UpdateSkill(
        pSyncSkillData->dwSkillID, 
        pSyncSkillData->bySkillLevel, 
        pSyncSkillData->dwSkillExp
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pSyncSkillData->bNotifyPlayer); // Initial data won't notify UI. 

    if (g_pGameWorldUIHandler)
    {
        //UI: 技能数据改变
        UIParam.dwSkillID       = pSyncSkillData->dwSkillID;
        UIParam.dwSkillLevel    = pSyncSkillData->bySkillLevel;
        if (dwOldSkillLevel != pSyncSkillData->bySkillLevel)    // Skill Level up!
        {
            g_pGameWorldUIHandler->OnSkillUpdate(UIParam);
        }
        else
        {
            g_pGameWorldUIHandler->OnSkillExpUpdate(UIParam);
        }
    }

    if (dwOldSkillLevel != pSyncSkillData->bySkillLevel)    // Skill Level up!
    {
        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pPlayer, REPRESENT_SFX_SKILL_LEVEL_UP);
        }

        pPlayer->m_QuestList.UpdateNpcQuestMark(-1);
    }

Exit0:
	return;
}

void KPlayerClient::OnSyncSkillRecipe(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode       = false;
	S2C_SYNC_SKILL_RECIPE*  pSyncSkillData = NULL;;
    KPlayer*                pPlayer        = NULL;

    KGLOG_PROCESS_ERROR(nSize >= sizeof(S2C_SYNC_SKILL_RECIPE));
    pSyncSkillData = (S2C_SYNC_SKILL_RECIPE*)pbyData;
    assert(pSyncSkillData);

    KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_SYNC_SKILL_RECIPE) + sizeof(S2C_SYNC_SKILL_RECIPE::_RecipeNode) * pSyncSkillData->wCount);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_ASSERT_EXIT(pPlayer);

    pPlayer->m_SkillRecipeList.Clear();

    for (int i = 0; i < pSyncSkillData->wCount; i++)
    {
        DWORD   dwRecipeKey = pSyncSkillData->RecipeArray[i].dwRecipeKey;
        BOOL    bActive     = pSyncSkillData->RecipeArray[i].bActive;

        bRetCode = pPlayer->m_SkillRecipeList.Add(dwRecipeKey);
        KGLOG_PROCESS_ERROR(bRetCode);

        if (!bActive)
        {
            continue;
        }

        bRetCode = pPlayer->m_SkillRecipeList.Active(dwRecipeKey);
        if (!bRetCode)
        {
            continue;
        }
    }

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnSkillRecipeListUpdate();
    }

Exit0:
	return;
}

void KPlayerClient::OnSkillPrepare(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode            = false;
	SKILL_RESULT_CODE               nRetCode            = srcFailed;
    S2C_SKILL_PREPARE*              pSkillPrepare       = (S2C_SKILL_PREPARE*)pbyData;;
    TARGET_TYPE                     eTargetType         = ttInvalid;
    KCharacter*                     pCaster             = NULL;
    KCharacter*                     pTarget             = NULL;
    KTarget                         Target;

    if (IS_PLAYER(pSkillPrepare->dwCharacterID))
        pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pSkillPrepare->dwCharacterID);
    else
        pCaster = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pSkillPrepare->dwCharacterID);
    KG_PROCESS_ERROR(pCaster);

    eTargetType      = (TARGET_TYPE)pSkillPrepare->byTargetType;
    switch (eTargetType)
    {
    case ttCoordination:
        bRetCode = Target.SetTarget(pSkillPrepare->nParam1, pSkillPrepare->nParam2, pSkillPrepare->nParam3);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

    case ttNpc:
    case ttDoodad:
    case ttPlayer:
        bRetCode = Target.SetTarget(eTargetType, pSkillPrepare->nParam1);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

    default:
        KG_ASSERT_EXIT(!"Unexpected target type !");
        break;
    }

    pCaster->PrepareSkill(pSkillPrepare->dwSkillID, pSkillPrepare->bySkillLevel, Target, pSkillPrepare->nPrepareFrame, nFrame + pSkillPrepare->nPrepareFrame);

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterPrepareCastSkill(pCaster, pSkillPrepare->dwSkillID, pSkillPrepare->bySkillLevel);
    
    bRetCode = Target.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);

    if (pCaster->m_dwID != g_pSO3World->m_dwClientPlayerID && pTarget->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        KPlayer* pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pClientPlayer);

        eTargetType = pClientPlayer->m_SelectTarget.GetTargetType();
        KG_PROCESS_ERROR(eTargetType == ttNoTarget);

        bRetCode = pClientPlayer->SwitchSelectTarget(pCaster->m_dwID);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit0:
	return;
}

void KPlayerClient::OnSkillCast(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                             bRetCode            = false;
    S2C_SKILL_CAST*                 pSkillCast          = (S2C_SKILL_CAST*)pbyData;
    KCharacter*                     pCaster             = NULL;
    TARGET_TYPE                     eTargetType         = ttInvalid;
    KCharacter*                     pTarget             = NULL;
    KTarget                         Target;

    if (IS_PLAYER(pSkillCast->dwCharacterID))
        pCaster = g_pSO3World->m_PlayerSet.GetObj(pSkillCast->dwCharacterID);
    else
        pCaster = g_pSO3World->m_NpcSet.GetObj(pSkillCast->dwCharacterID);
    KG_PROCESS_ERROR(pCaster);

    eTargetType     = (TARGET_TYPE)pSkillCast->byTargetType;
    switch (eTargetType)
    {
    case ttCoordination:
        bRetCode = Target.SetTarget(pSkillCast->nParam1, pSkillCast->nParam2, pSkillCast->nParam3);
        KG_PROCESS_ERROR(bRetCode);
        break;

    case ttNpc:
    case ttDoodad:
    case ttPlayer:
        bRetCode = Target.SetTarget(eTargetType, pSkillCast->nParam1);
        KG_PROCESS_ERROR(bRetCode);
        break;

    default:
        KG_ASSERT_EXIT(!"Unexpected target type !");
        break;
    }
    
    if (pSkillCast->dwCharacterID != g_pSO3World->m_dwClientPlayerID)
    {
        pCaster->TurnTo(pSkillCast->byFaceDirection, false);
    }

    if (pSkillCast->bSubCast)
    {
        pCaster->CastSkillSub(pCaster, pSkillCast->dwSkillID, pSkillCast->bySkillLevel, Target, pSkillCast->dwBulletID);
    }
    else
    {
        pCaster->CastSkill(pSkillCast->dwSkillID, pSkillCast->bySkillLevel, Target, pSkillCast->dwBulletID);
    }

    bRetCode = Target.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);

    if (pCaster->m_dwID != g_pSO3World->m_dwClientPlayerID && pTarget->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        KPlayer* pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pClientPlayer);

        eTargetType = pClientPlayer->m_SelectTarget.GetTargetType();
        KG_PROCESS_ERROR(eTargetType == ttNoTarget);

        bRetCode = pClientPlayer->SwitchSelectTarget(pCaster->m_dwID);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit1:

Exit0:
    return;
}

void KPlayerClient::OnSyncCommonSkillAutoCastState(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                    bRetCode = false;
    S2C_SYNC_COMMON_SKILL_AUTO_CAST_STATE*  pNotify  = (S2C_SYNC_COMMON_SKILL_AUTO_CAST_STATE*)pbyData;
    KPlayer*                                pPlayer  = NULL;
    KSKILL_RECIPE_KEY                       SkillRecipeKey;
    KSkillRecipePointer                     SkillRecipePointer;
    KUIEventCastCommonSkill                 EventCastCommonSkill;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pNotify->bStart)
    {
        int     nWeaponDetailType   = wdtFist;
        DWORD   dwSkillID           = 0;
        KSkill* pSkill              = NULL;
        KItem*  pWeapon             = NULL;

        if (pNotify->bMelee)
        {
            pWeapon = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
            if (pWeapon)
            {
                nWeaponDetailType = pWeapon->m_Common.nDetail;
            }
        }
        else
        {
            pWeapon = pPlayer->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
            KGLOG_PROCESS_ERROR(pWeapon);
            nWeaponDetailType = pWeapon->m_Common.nDetail;
        }
        
        bRetCode = g_pSO3World->m_SkillManager.WeaponMapCommonSkill(nWeaponDetailType, dwSkillID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, 1, pPlayer);

        pSkill = SkillRecipePointer.SetRecipeKey(SkillRecipeKey);
        KGLOG_PROCESS_ERROR(pSkill);

        pPlayer->m_AutoCastSkillParam.SkillRecipePointer = SkillRecipePointer;
    }
    else
    {
        pPlayer->m_AutoCastSkillParam.SkillRecipePointer.ClearRecipeKey();
    }

    pPlayer->m_AutoCastSkillParam.bMelee = pNotify->bMelee;

    // 通知UI
    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

    EventCastCommonSkill.bEnterOrLeaveCast = pNotify->bStart;

    g_pGameWorldUIHandler->OnCastCommonSkill(EventCastCommonSkill);

Exit0:
    return;
}

void KPlayerClient::OnResetCooldown(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                bRetCode    = false;
    S2C_RESET_COOLDOWN* pNotify     = (S2C_RESET_COOLDOWN*)pbyData;
    KPlayer*            pPlayer     = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    int                 nInterval   = 0;

    KGLOG_PROCESS_ERROR(pPlayer);

    if (pNotify->nEndFrame == -1) // 同步过来为-1证明是ClearTimer的函数调用同步过来的数据
    {
        pPlayer->m_TimerList.ClearTimer(pNotify->dwCoolDownID);
    }
    else
    {
        pPlayer->m_TimerList.SetTimer(pNotify->dwCoolDownID, pNotify->nInterval, pNotify->nEndFrame);
    }

Exit0:
    return;
}

void KPlayerClient::OnSkillChainEffect(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode                = false;
    S2C_SKILL_CHAIN_EFFECT*         pPak                    = (S2C_SKILL_CHAIN_EFFECT*)pbyData;
    int                             nIndex                  = 0;
    int                             nTargetListCount        = 0;
    int                             nCurrentCount           = 0;
    KSkill*                         pSkill                  = NULL;
    KCharacter*                     pCaster                 = NULL;
    KCharacter*                     pCharacter              = NULL;
    KSKILL_RECIPE_KEY               SkillRecipeKey          = {0};
    KChainDisplayInfo               ChainDisplayList[MAX_CHAIN_TARGET];
    DWORD                           dwTargetList[MAX_CHAIN_TARGET];
    
    if (IS_PLAYER(pPak->dwCasterID))
        pCaster = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCasterID);
    else
        pCaster = g_pSO3World->m_NpcSet.GetObj(pPak->dwCasterID);
    KG_PROCESS_ERROR(pCaster);

    SkillRecipeKey.dwSkillID    = pPak->dwSkillID;
    SkillRecipeKey.dwSkillLevel = pPak->dwSkillLevel;
    pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nTargetListCount = 0;
    for (int i = 0; i < (int)pPak->uCount; i++)
    {
        DWORD dwCharacterID = pPak->dwCharacterID[i];
        KGLOG_PROCESS_ERROR(dwCharacterID);

        if (IS_PLAYER(dwCharacterID))
        {
            pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);
        }
        else
        {
            pCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
        }

        if (pCharacter)
        {
            dwTargetList[nTargetListCount++] = pPak->dwCharacterID[i];
        }
    }

    nIndex = 1;
    nCurrentCount = pSkill->m_nChainBranch;
    for (int i = 0; i < pSkill->m_nChainDepth && nIndex < nTargetListCount; i++)
    {
        for (int j = 0; j < nCurrentCount && nIndex < nTargetListCount; j++)
        {
            ChainDisplayList[nIndex - 1].dwDstCharacterID = dwTargetList[nIndex];
            ChainDisplayList[nIndex - 1].dwSrcCharacterID = dwTargetList[(nIndex - 1) / pSkill->m_nChainBranch];
            ChainDisplayList[nIndex - 1].dwSkillID        = pSkill->m_SubsectionSkill[i].dwSubSkillID;
            ChainDisplayList[nIndex - 1].dwSkillLevel     = pSkill->m_SubsectionSkill[i].dwSubSkillLevel;

            nIndex++;
        }
        nCurrentCount *= pSkill->m_nChainBranch;
    }

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterSkillChainDisplay(nTargetListCount - 1, ChainDisplayList);

Exit0:
    return;
}

void KPlayerClient::OnSkillEffectResult(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                               bRetCode         = false;
    S2C_SKILL_EFFECT_RESULT*           pSkillResult     = NULL;
    BOOL                               bShield          = false; 
    BOOL                               bHitTarget       = false;
    BOOL                               bReact           = false;
    BOOL                               bCriticalStrike  = false;
    BOOL                               bNeedUIText      = true;

    KG_PROCESS_ERROR(g_pGameWorldUIHandler);

    KGLOG_PROCESS_ERROR(nSize >= sizeof(S2C_SKILL_EFFECT_RESULT));
    pSkillResult = (S2C_SKILL_EFFECT_RESULT *)pbyData;

    bReact          = pSkillResult->bReact;       
    bShield         = pSkillResult->bShield;
    bHitTarget      = pSkillResult->bHitTarget;
    bCriticalStrike = pSkillResult->bCriticalStrike;

    // ----------- 战斗记录, "Miss", "+life", "-life"的UI显示 ------------------------------

    if (bShield)
    {
        KUIEventSkillShieldLog ShieldLog;
        ShieldLog.dwCaster     = pSkillResult->dwSkillSrc;
        ShieldLog.dwTarget     = pSkillResult->dwSkillDst;
        ShieldLog.nType        = (int)pSkillResult->EffectSrc.byType;
        ShieldLog.dwID         = pSkillResult->EffectSrc.dwID;
        ShieldLog.dwLevel      = (DWORD)pSkillResult->EffectSrc.byLevel;
        g_pGameWorldUIHandler->OnSkillShieldLog(ShieldLog); 
    }

    KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_SKILL_EFFECT_RESULT) + sizeof(S2C_SKILL_EFFECT_RESULT::KSKILL_RESULT) * pSkillResult->cResultCount);

    if (bHitTarget)
    {
        if (pSkillResult->bSkillDodge)
        {
            KUIEventSkillDodgeText SkillDodge = { pSkillResult->dwSkillSrc, pSkillResult->dwSkillDst };
            g_pGameWorldUIHandler->OnSkillDodgeText(SkillDodge);

            KUIEventSkillDodgeLog DodgeLog;
            DodgeLog.dwCaster     = pSkillResult->dwSkillSrc;
            DodgeLog.dwTarget     = pSkillResult->dwSkillDst;
            DodgeLog.nType        = (int)pSkillResult->EffectSrc.byType;
            DodgeLog.dwID         = pSkillResult->EffectSrc.dwID;
            DodgeLog.dwLevel      = (DWORD)pSkillResult->EffectSrc.byLevel;
            g_pGameWorldUIHandler->OnSkillDodgeLog(DodgeLog); 
        }

        // 格挡 
        if (pSkillResult->bPhysicsBlock)
        {
            KUIEventSkillBlockText SkillBlockText = { pSkillResult->dwSkillSrc, pSkillResult->dwSkillDst };
            g_pGameWorldUIHandler->OnSkillBlockText(SkillBlockText);

            KUIEventSkillBlockLog SkillBlockLog;

            SkillBlockLog.dwCaster      = pSkillResult->dwSkillSrc;
            SkillBlockLog.dwTarget      = pSkillResult->dwSkillDst;
            SkillBlockLog.nType         = (int)pSkillResult->EffectSrc.byType;
            SkillBlockLog.dwID          = pSkillResult->EffectSrc.dwID;
            SkillBlockLog.dwLevel       = (DWORD)pSkillResult->EffectSrc.byLevel;
            SkillBlockLog.nDamageType   = serPhysicsDamage;
            g_pGameWorldUIHandler->OnSkillBlockLog(SkillBlockLog);
        }

        if (pSkillResult->bSolarMagicBlock)
        {
            KUIEventSkillBlockText SkillBlockText = { pSkillResult->dwSkillSrc, pSkillResult->dwSkillDst };
            g_pGameWorldUIHandler->OnSkillBlockText(SkillBlockText);

            KUIEventSkillBlockLog SkillBlockLog;

            SkillBlockLog.dwCaster      = pSkillResult->dwSkillSrc;
            SkillBlockLog.dwTarget      = pSkillResult->dwSkillDst;
            SkillBlockLog.nType         = (int)pSkillResult->EffectSrc.byType;
            SkillBlockLog.dwID          = pSkillResult->EffectSrc.dwID;
            SkillBlockLog.dwLevel       = (DWORD)pSkillResult->EffectSrc.byLevel;
            SkillBlockLog.nDamageType   = serSolarMagicDamage;
            g_pGameWorldUIHandler->OnSkillBlockLog(SkillBlockLog);
        }

        if (pSkillResult->bNeutralMagicBlock)
        {
            KUIEventSkillBlockText SkillBlockText = { pSkillResult->dwSkillSrc, pSkillResult->dwSkillDst };
            g_pGameWorldUIHandler->OnSkillBlockText(SkillBlockText);

            KUIEventSkillBlockLog SkillBlockLog;

            SkillBlockLog.dwCaster      = pSkillResult->dwSkillSrc;
            SkillBlockLog.dwTarget      = pSkillResult->dwSkillDst;
            SkillBlockLog.nType         = (int)pSkillResult->EffectSrc.byType;
            SkillBlockLog.dwID          = pSkillResult->EffectSrc.dwID;
            SkillBlockLog.dwLevel       = (DWORD)pSkillResult->EffectSrc.byLevel;
            SkillBlockLog.nDamageType   = serNeutralMagicDamage;
            g_pGameWorldUIHandler->OnSkillBlockLog(SkillBlockLog);
        }

        if (pSkillResult->bLunarMagicBlock)
        {
            KUIEventSkillBlockText SkillBlockText = { pSkillResult->dwSkillSrc, pSkillResult->dwSkillDst };
            g_pGameWorldUIHandler->OnSkillBlockText(SkillBlockText);

            KUIEventSkillBlockLog SkillBlockLog;

            SkillBlockLog.dwCaster      = pSkillResult->dwSkillSrc;
            SkillBlockLog.dwTarget      = pSkillResult->dwSkillDst;
            SkillBlockLog.nType         = (int)pSkillResult->EffectSrc.byType;
            SkillBlockLog.dwID          = pSkillResult->EffectSrc.dwID;
            SkillBlockLog.dwLevel       = (DWORD)pSkillResult->EffectSrc.byLevel;
            SkillBlockLog.nDamageType   = serLunarMagicDamage;
            g_pGameWorldUIHandler->OnSkillBlockLog(SkillBlockLog);
        }

        if (pSkillResult->bPoisonBlock)
        {
            KUIEventSkillBlockText SkillBlockText = { pSkillResult->dwSkillSrc, pSkillResult->dwSkillDst };
            g_pGameWorldUIHandler->OnSkillBlockText(SkillBlockText);

            KUIEventSkillBlockLog SkillBlockLog;

            SkillBlockLog.dwCaster      = pSkillResult->dwSkillSrc;
            SkillBlockLog.dwTarget      = pSkillResult->dwSkillDst;
            SkillBlockLog.nType         = (int)pSkillResult->EffectSrc.byType;
            SkillBlockLog.dwID          = pSkillResult->EffectSrc.dwID;
            SkillBlockLog.dwLevel       = (DWORD)pSkillResult->EffectSrc.byLevel;
            SkillBlockLog.nDamageType   = serPoisonDamage;
            g_pGameWorldUIHandler->OnSkillBlockLog(SkillBlockLog);
        }

        if (pSkillResult->bSkillImmunity)
        {
            // TODO: 技能被免疫！
        }

        if (pSkillResult->bBuffResist)
        {
            // UI提示
            KUIEventBuffImmunityText ImmunityText;
            ImmunityText.dwCharacterID = pSkillResult->dwSkillDst;
            g_pGameWorldUIHandler->OnBuffImmunityText(ImmunityText);
            // 写日志
            KUIEventBuffImmunityLog  ImmunityLog;
            ImmunityLog.dwTarget    = pSkillResult->dwSkillDst;
            ImmunityLog.bCanCancel  = false;
            ImmunityLog.dwID        = pSkillResult->dwBuffID;
            ImmunityLog.nLevel      = pSkillResult->byBuffLevel;
            g_pGameWorldUIHandler->OnBuffImmunityLog(ImmunityLog); 
        }

        if (pSkillResult->bBuffImmunity)
        {
            // UI提示
            KUIEventBuffImmunityText ImmunityText;
            ImmunityText.dwCharacterID = pSkillResult->dwSkillDst;
            g_pGameWorldUIHandler->OnBuffImmunityText(ImmunityText);
            // 写日志
            KUIEventBuffImmunityLog  ImmunityLog;
            ImmunityLog.dwTarget    = pSkillResult->dwSkillDst;
            ImmunityLog.bCanCancel  = false;
            ImmunityLog.dwID        = pSkillResult->dwBuffID;
            ImmunityLog.nLevel      = pSkillResult->byBuffLevel;
            g_pGameWorldUIHandler->OnBuffImmunityLog(ImmunityLog); 
        }

        for (int i = 0; i < pSkillResult->cResultCount; i++)
        {
            if (pSkillResult->SkillResult[i].nValue == 0)
                continue;

            KUIEventSkillEffectLog SkillEffectLog;
            SkillEffectLog.dwCaster         = pSkillResult->dwSkillSrc;
            SkillEffectLog.dwTarget         = pSkillResult->dwSkillDst;
            SkillEffectLog.bReact           = bReact;
            SkillEffectLog.nType            = (int)pSkillResult->EffectSrc.byType;
            SkillEffectLog.dwID             = pSkillResult->EffectSrc.dwID;
            SkillEffectLog.dwLevel          = (DWORD)pSkillResult->EffectSrc.byLevel;
            SkillEffectLog.bCriticalStrike  = bCriticalStrike;
            SkillEffectLog.nDamageType      = pSkillResult->SkillResult[i].cEffectResultType;
            SkillEffectLog.nValue           = pSkillResult->SkillResult[i].nValue;
            g_pGameWorldUIHandler->OnSkillEffectLog(SkillEffectLog);
        }
    }
    else if(!pSkillResult->bShield)
    {   
        // 未命中
        // UI提示
        KUIEventSkillMissText SkillMissText;
        SkillMissText.dwCasterID = pSkillResult->dwSkillSrc;
        SkillMissText.dwTargetID = pSkillResult->dwSkillDst;
        g_pGameWorldUIHandler->OnSkillMissText(SkillMissText);

        // 写日志
        KUIEventSkillMissLog MissLog;
        MissLog.dwCaster     = pSkillResult->dwSkillSrc;
        MissLog.dwTarget     = pSkillResult->dwSkillDst;
        MissLog.nType        = (int)pSkillResult->EffectSrc.byType;
        MissLog.dwID         = pSkillResult->EffectSrc.dwID;
        MissLog.dwLevel      = (DWORD)pSkillResult->EffectSrc.byLevel;
        g_pGameWorldUIHandler->OnSkillMissLog(MissLog); 
    }
    
	// 通知表现逻辑
	if (pSkillResult->EffectSrc.byType == setSkill)
	{
		KCharacter* pSkillSrc   = NULL;
		KCharacter* pSkillDst   = NULL;
		KCharacter* pCaster     = NULL; 
		KCharacter* pTarget     = NULL; 
		KSkill*     pSkill      = NULL;
		KTarget     Target;
		KRepresentSkillEffectResult EffectResult;

		if (IS_PLAYER(pSkillResult->dwSkillSrc))
			pSkillSrc = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pSkillResult->dwSkillSrc);
		else
			pSkillSrc = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pSkillResult->dwSkillSrc);

		if (IS_PLAYER(pSkillResult->dwSkillDst))
			pSkillDst = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pSkillResult->dwSkillDst);
		else
			pSkillDst = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pSkillResult->dwSkillDst);

		KG_PROCESS_ERROR(pSkillSrc);
		KG_PROCESS_ERROR(pSkillDst);

		pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(pSkillResult->EffectSrc.dwID, pSkillResult->EffectSrc.byLevel);
		KG_PROCESS_ERROR(pSkill);

		pCaster = bReact ? pSkillDst : pSkillSrc;
		pTarget = bReact ? pSkillSrc : pSkillDst;

        EffectResult.bImmediately = ((pSkill->m_pBaseInfo->bIsChannelSkill) || !(pSkill->m_pBaseInfo->bBindCombatTag));
        EffectResult.dwSkillID = pSkillResult->EffectSrc.dwID;
        EffectResult.dwSkillLevel = (DWORD)pSkillResult->EffectSrc.byLevel;
        EffectResult.dwBulletID = pSkillResult->dwBulletID;
		EffectResult.bHitTarget = pSkillResult->bHitTarget;
		EffectResult.bSkillDodge = pSkillResult->bSkillDodge;
		EffectResult.bCriticalStrike = pSkillResult->bCriticalStrike;
        EffectResult.bParry = pSkillResult->bParry;
        EffectResult.bInsight = pSkillResult->bInsight;
        EffectResult.nEffectPlayType = pSkill->m_pBaseInfo->nEffectPlayType;

        memset(&EffectResult.aSkillBlockFlag, 0, sizeof(EffectResult.aSkillBlockFlag));
        memset(&EffectResult.aSkillResultFlag, 0, sizeof(EffectResult.aSkillResultFlag));
        memset(&EffectResult.aSkillResultValue, 0, sizeof(EffectResult.aSkillResultValue));

        EffectResult.aSkillBlockFlag[serPhysicsDamage] = pSkillResult->bPhysicsBlock;
        EffectResult.aSkillBlockFlag[serSolarMagicDamage] = pSkillResult->bSolarMagicBlock;
        EffectResult.aSkillBlockFlag[serNeutralMagicDamage] = pSkillResult->bNeutralMagicBlock;
        EffectResult.aSkillBlockFlag[serLunarMagicDamage] = pSkillResult->bLunarMagicBlock;
        EffectResult.aSkillBlockFlag[serPoisonDamage] = pSkillResult->bPoisonBlock;

        for (int i = 0; i < pSkillResult->cResultCount; ++i)
        {
            if (pSkillResult->SkillResult[i].nValue != 0)
			{
				int nType = pSkillResult->SkillResult[i].cEffectResultType;
				int nValue = pSkillResult->SkillResult[i].nValue;

				EffectResult.aSkillResultFlag[nType] = nValue != 0;
				EffectResult.aSkillResultValue[nType] = nValue;
			}
        }

        // 反弹则直接冒血
        if (EffectResult.aSkillResultFlag[serReflectiedDamage])
            EffectResult.bImmediately = true;

        bNeedUIText = EffectResult.bImmediately;

		Target.SetTarget(pTarget);
		Target.GetTargetData(EffectResult.Target);

		EffectResult.Caster.dwCharacterID = pCaster->m_dwID;

		if (IS_PLAYER(pCaster->m_dwID))
		{
			EffectResult.Caster.eTargetType = ttPlayer;
			EffectResult.Caster.nRoleType = reinterpret_cast<KPlayer*>(pCaster)->m_eRoleType;
		}
		else
		{
			EffectResult.Caster.eTargetType = ttNpc;
			EffectResult.Caster.dwRepresentID = reinterpret_cast<KNpc*>(pCaster)->m_dwModelID;
		}

        g_pGameWorldRepresentHandler->OnCharacterSkillEffectResult(pTarget, &EffectResult);
	}

	if (bNeedUIText)
	{
        for (int i = 0; i < pSkillResult->cResultCount; ++i)
        {
            if (pSkillResult->SkillResult[i].nValue != 0)
			{
				KUIEventSkillEffectText SkillEffectText;

				SkillEffectText.dwCasterID = pSkillResult->dwSkillSrc;
				SkillEffectText.dwTargetID = pSkillResult->dwSkillDst;
				SkillEffectText.nType = pSkillResult->SkillResult[i].cEffectResultType;
				SkillEffectText.bCriticalStrike = pSkillResult->bCriticalStrike;
				SkillEffectText.nValue = pSkillResult->SkillResult[i].nValue;

                if (pSkillResult->EffectSrc.byType == setSkill)
                {
                    SkillEffectText.dwSkillID = pSkillResult->EffectSrc.dwID;
                    SkillEffectText.dwSkillLevel = pSkillResult->EffectSrc.byLevel;
                }
                else
                {
                    SkillEffectText.dwSkillID = 0;
                    SkillEffectText.dwSkillLevel = 0;
                }

				g_pGameWorldUIHandler->OnSkillEffectText(SkillEffectText);
			}
        }
	}

    
    if (
        pSkillResult->dwSkillSrc != g_pSO3World->m_dwClientPlayerID && 
        pSkillResult->dwSkillDst == g_pSO3World->m_dwClientPlayerID
    )
    {
        TARGET_TYPE eTargetType     = ttInvalid;
        KPlayer*    pClientPlayer   = NULL;
        
        pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pClientPlayer);

        eTargetType = pClientPlayer->m_SelectTarget.GetTargetType();
        KG_PROCESS_ERROR(eTargetType == ttNoTarget);

        bRetCode = pClientPlayer->SwitchSelectTarget(pSkillResult->dwSkillSrc);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncBehitRepresent(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_BEHIT_REPRESENT*           pPak                = (S2C_SYNC_BEHIT_REPRESENT *)pbyData;   
    KCharacter*                         pSkillSrc           = NULL;
    KCharacter*                         pSkillDst           = NULL;
	KCharacter*                         pCaster             = NULL; 
	KCharacter*                         pTarget             = NULL; 
    KSkill*                             pSkill              = NULL;
    bool                                bReact              = false;
	KRepresentSkillEffectResult			EffectResult;
	KTarget                             Target;
    
    KG_PROCESS_SUCCESS(pPak->EffectSrc.byType != setSkill);    
    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);
    
    if (IS_PLAYER(pPak->dwSkillSrc))
        pSkillSrc = (KCharacter *)g_pSO3World->m_PlayerSet.GetObj(pPak->dwSkillSrc);
    else
        pSkillSrc = (KCharacter *)g_pSO3World->m_NpcSet.GetObj(pPak->dwSkillSrc);

    if (IS_PLAYER(pPak->dwSkillDst))
        pSkillDst = (KCharacter *)g_pSO3World->m_PlayerSet.GetObj(pPak->dwSkillDst);
    else
        pSkillDst = (KCharacter *)g_pSO3World->m_NpcSet.GetObj(pPak->dwSkillDst);

    KG_PROCESS_ERROR(pSkillSrc);
    KG_PROCESS_ERROR(pSkillDst);
    
    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(pPak->EffectSrc.dwID, pPak->EffectSrc.byLevel);
    KG_PROCESS_ERROR(pSkill);
    
    pCaster = bReact ? pSkillDst : pSkillSrc;
    pTarget = bReact ? pSkillSrc : pSkillDst;

    memset(&EffectResult.aSkillBlockFlag, 0, sizeof(EffectResult.aSkillBlockFlag));
    memset(&EffectResult.aSkillResultFlag, 0, sizeof(EffectResult.aSkillResultFlag));
	memset(&EffectResult.aSkillResultValue, 0, sizeof(EffectResult.aSkillResultValue));

    EffectResult.bImmediately = ((pSkill->m_pBaseInfo->bIsChannelSkill) || !(pSkill->m_pBaseInfo->bBindCombatTag));
    EffectResult.dwSkillID = pPak->EffectSrc.dwID;
    EffectResult.dwSkillLevel = (DWORD)pPak->EffectSrc.byLevel;
	EffectResult.dwBulletID = pPak->dwBulletID;
	EffectResult.bHitTarget = pPak->bHitTarget;
    EffectResult.bSkillDodge = pPak->bSkillDodge;
    EffectResult.bCriticalStrike = pPak->bCriticalStrike;
    EffectResult.bParry = pPak->bParry;
    EffectResult.bInsight = pPak->bInsight;
    EffectResult.nEffectPlayType = pSkill->m_pBaseInfo->nEffectPlayType;

    EffectResult.aSkillBlockFlag[serPhysicsDamage] = pPak->bPhysicsBlock;
    EffectResult.aSkillBlockFlag[serSolarMagicDamage] = pPak->bSolarMagicBlock;
    EffectResult.aSkillBlockFlag[serNeutralMagicDamage] = pPak->bNeutralMagicBlock;
    EffectResult.aSkillBlockFlag[serLunarMagicDamage] = pPak->bLunarMagicBlock;
    EffectResult.aSkillBlockFlag[serPoisonDamage] = pPak->bPoisonBlock;

	EffectResult.aSkillResultFlag[serPhysicsDamage] = pPak->bPhysicsDamage;
	EffectResult.aSkillResultFlag[serSolarMagicDamage] = pPak->bSolarMagicDamage;
	EffectResult.aSkillResultFlag[serNeutralMagicDamage] = pPak->bNeutralMagicDamage;
	EffectResult.aSkillResultFlag[serLunarMagicDamage] = pPak->bLunarMagicDamage;
	EffectResult.aSkillResultFlag[serPoisonDamage] = pPak->bPoisonDamage;
	EffectResult.aSkillResultFlag[serReflectiedDamage] = pPak->bReflectiedDamage;
	EffectResult.aSkillResultFlag[serTherapy] = pPak->bTherapy;
    EffectResult.aSkillResultFlag[serStealLife] = pPak->bStealLife;
	EffectResult.aSkillResultFlag[serAbsorbDamage] = pPak->bAbsorbDamage;
	EffectResult.aSkillResultFlag[serShieldDamage] = pPak->bShieldDamage;

    // 反弹则直接冒血
    if (EffectResult.aSkillResultFlag[serReflectiedDamage])
        EffectResult.bImmediately = true;

	Target.SetTarget(pTarget);
	Target.GetTargetData(EffectResult.Target);

	EffectResult.Caster.dwCharacterID = pCaster->m_dwID;

	if (IS_PLAYER(pCaster->m_dwID))
    {
        EffectResult.Caster.eTargetType = ttPlayer;
		EffectResult.Caster.nRoleType = reinterpret_cast<KPlayer*>(pCaster)->m_eRoleType;
    }
    else
    {
        EffectResult.Caster.eTargetType = ttNpc;
		EffectResult.Caster.dwRepresentID = reinterpret_cast<KNpc*>(pCaster)->m_dwModelID;
    }

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterSkillEffectResult(pTarget, &EffectResult);

    if (EffectResult.bHitTarget)
    {
        KUIEventSkillHitLog HitLog;
        HitLog.dwCaster     = pPak->dwSkillSrc;
        HitLog.dwTarget     = pPak->dwSkillDst;
        HitLog.nType        = (int)pPak->EffectSrc.byType;
        HitLog.dwID         = pPak->EffectSrc.dwID;
        HitLog.dwLevel      = (DWORD)pPak->EffectSrc.byLevel;
        g_pGameWorldUIHandler->OnSkillHitLog(HitLog);
    }
    else
    {
        // 写日志
        KUIEventSkillMissLog MissLog;
        MissLog.dwCaster     = pPak->dwSkillSrc;
        MissLog.dwTarget     = pPak->dwSkillDst;
        MissLog.nType        = (int)pPak->EffectSrc.byType;
        MissLog.dwID         = pPak->EffectSrc.dwID;
        MissLog.dwLevel      = (DWORD)pPak->EffectSrc.byLevel;
        g_pGameWorldUIHandler->OnSkillMissLog(MissLog); 
    }

Exit1:

Exit0:
    return;
}

void KPlayerClient::OnSyncBuffList(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode    = false;
    S2C_SYNC_BUFF_LIST*     pSyncBuff   = (S2C_SYNC_BUFF_LIST*)pbyData;
    KCharacter*             pCharacter  = NULL;

    KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_SYNC_BUFF_LIST) + pSyncBuff->wDataSize);

    bRetCode = IS_PLAYER(pSyncBuff->dwCharacterID);
    if (bRetCode)
    {
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pSyncBuff->dwCharacterID);
    }
    else
    {
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pSyncBuff->dwCharacterID);
    }
    KG_PROCESS_ERROR(pCharacter);

    pCharacter->m_BuffList.UpdateBuffData(pSyncBuff->byData, pSyncBuff->wDataSize);

Exit0:
    return;
}

void KPlayerClient::OnSyncBuffSingle(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode    = false;
    S2C_SYNC_BUFF_SINGLE*   pBuffSingle = (S2C_SYNC_BUFF_SINGLE*)pbyData;
    KCharacter*             pCharacter  = NULL;
    KBuff*                  pBuff       = NULL;
    KBUFF_LIST_NODE         BuffListNode;
    KBUFF_RECIPE_KEY        BuffRecipeKey;

    bRetCode = IS_PLAYER(pBuffSingle->dwCharacterID);
    if (bRetCode)
    {
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pBuffSingle->dwCharacterID);
    }
    else
    {
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pBuffSingle->dwCharacterID);
    }
    KGLOG_PROCESS_ERROR(pCharacter);

    BuffRecipeKey.dwID              = pBuffSingle->dwBuffID;
    BuffRecipeKey.nLevel            = pBuffSingle->byLevel;
    BuffRecipeKey.dwRecipeKey[0]    = pBuffSingle->dwRecipeKey;

    pBuff = BuffListNode.BuffRecipePointer.SetRecipeKey(BuffRecipeKey);
    KGLOG_PROCESS_ERROR(pBuff);

    BuffListNode.nIndex     = pBuffSingle->nIndex;
    BuffListNode.nStackNum  = pBuffSingle->byStackNum;

    assert(pBuff->m_nIntervalFrame);
    if (pBuff->m_nIntervalFrame == INT_MAX)
    {
        BuffListNode.nLeftActiveCount = 1;
        BuffListNode.nNextActiveFrame = INT_MAX;
    }
    else
    {
        BuffListNode.nLeftActiveCount = (pBuffSingle->nEndFrame - g_pSO3World->m_nGameLoop) / pBuff->m_nIntervalFrame;
        if ((pBuffSingle->nEndFrame - g_pSO3World->m_nGameLoop) % pBuff->m_nIntervalFrame)
        {
            BuffListNode.nLeftActiveCount++;
        }

        BuffListNode.nNextActiveFrame = pBuffSingle->nEndFrame - pBuff->m_nIntervalFrame * (BuffListNode.nLeftActiveCount - 1);
    }

    pCharacter->m_BuffList.UpdateBuff(BuffListNode);

Exit0:
    return;
}

void KPlayerClient::OnSyncRepresentState(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_REPRESENT_STATE*       pPak                = (S2C_SYNC_REPRESENT_STATE*)pbyData;
    KCharacter*                     pCharacter          = NULL;

    if (IS_PLAYER(pPak->dwCharacterID))
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    else
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);
    KG_PROCESS_ERROR(pCharacter);

    memcpy(pCharacter->m_byRepresentState, pPak->byRepresentState, sizeof(pCharacter->m_byRepresentState));

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterSkillBuffData(pCharacter);

Exit0:
    return;
}

void KPlayerClient::OnSyncKungfuMount(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode  = false;
    S2C_SYNC_KUNGFU_MOUNT*  pPak      = (S2C_SYNC_KUNGFU_MOUNT*)pbyData;
    KPlayer*                pPlayer   = NULL;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_ASSERT_EXIT(pPlayer);

    if (pPak->dwID == INVALID_SKILL_ID)
    {
        bRetCode = pPlayer->UmountKungfu();
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        bRetCode = pPlayer->MountKungfu(pPak->dwID, pPak->dwLevel);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return;
}

void KPlayerClient::OnSkillBeatBack(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                  nRetCode   = false;
    S2C_SKILL_BEAT_BACK* pPak       = (S2C_SKILL_BEAT_BACK*)pbyData;
    KCharacter*          pCharacter = NULL;

    if (IS_PLAYER(pPak->dwCharacterID))
    {
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    }
    else
    {
        pCharacter = g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);
    }

    KGLOG_PROCESS_ERROR(pCharacter);

    switch (pCharacter->m_OTActionParam.eActionType)
    {
    case otActionSkillPrepare:
	case otActionCustomPrepare:
	case otActionPickPrepare:
    case otActionUseItemPrepare:
    case otActionRecipePrepare:
		if (pPak->bBreak)
		{
            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnOTActionProgressBreak(pPak->dwCharacterID);
            }
			pCharacter->DoActionIdle();
		}
		else
		{
			if (pPak->dwCharacterID == g_pSO3World->m_dwClientPlayerID)
			{
                if (g_pGameWorldUIHandler)
                {
				    g_pGameWorldUIHandler->OnOTActionProgressUpdate(pPak->nBeatBackFrame);
                }
			}
			pCharacter->OnOTActionBeatBack(pPak->nBeatBackFrame);
		}
        break;
    case otActionSkillChannel:
	case otActionCustomChannel:
		if (pPak->bBreak)
		{
            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnOTActionProgressBreak(pPak->dwCharacterID);
            }
			pCharacter->DoActionIdle();
		}
		else
		{
			if (pPak->dwCharacterID == g_pSO3World->m_dwClientPlayerID)
			{
                if (g_pGameWorldUIHandler)
                {
				    g_pGameWorldUIHandler->OnOTActionProgressUpdate(-pPak->nBeatBackFrame);
                }
			}
			pCharacter->OnOTActionBeatBack(pPak->nBeatBackFrame);
		}
        break;
    case otActionPicking:
        if (pCharacter->m_dwID == g_pSO3World->m_dwClientPlayerID)
        {
            KDoodad* pDoodad = NULL;

            pDoodad = g_pSO3World->m_DoodadSet.GetObj(pCharacter->m_OTActionParam.Data.dwDoodadID);
            KG_PROCESS_ERROR(pDoodad);
            KG_PROCESS_ERROR(pDoodad->m_pRegion);

            if (g_pGameWorldUIHandler)
            {
                KUIEventCloseDoodad param;
                param.dwDoodadID = pDoodad->m_dwID;
                param.dwPlayerID = pCharacter->m_dwID;
                g_pGameWorldUIHandler->OnCloseDoodad(param);
            }
        }
        pCharacter->DoActionIdle();
        break;
    default:
        break;
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncSheathState(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SHEATH_STATE*              pPak                = (S2C_SYNC_SHEATH_STATE*)pbyData;

    if (IS_PLAYER(pPak->dwCharacterID))
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
        KG_PROCESS_ERROR(pPlayer);
        pPlayer->m_bSheathFlag = pPak->bSheathFlag;
    }
    else
    {
        KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);
        KG_PROCESS_ERROR(pNpc);
        pNpc->m_bSheathFlag = pPak->bSheathFlag;
    }

Exit0:
    return;
}

// ------------------- 数值同步 -----------------------------

void KPlayerClient::OnSyncSelfMaxLMRS(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SELF_MAX_LMRS*     pPak    = (S2C_SYNC_SELF_MAX_LMRS*)pbyData;
    KPlayer*                    pSelf   = NULL;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    pSelf->m_nMaxLife     = pPak->nMaxLife;
    pSelf->m_nMaxMana     = pPak->nMaxMana;
    pSelf->m_nMaxRage     = pPak->nMaxRage;

    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerStateUpdate param = { g_pSO3World->m_dwClientPlayerID };
        g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncSelfCurrentLMRS(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SELF_CURRENT_LMRS* pPak    = (S2C_SYNC_SELF_CURRENT_LMRS*)pbyData;
    KPlayer*                    pSelf   = NULL;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    pSelf->m_nCurrentLife     = pPak->nCurrentLife;
    pSelf->m_nCurrentMana     = pPak->nCurrentMana;
    pSelf->m_nCurrentRage     = pPak->nCurrentRage;

    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerStateUpdate param = { g_pSO3World->m_dwClientPlayerID };
        g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncSelfWeakInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SELF_WEAK_INFO*    pPak    = (S2C_SYNC_SELF_WEAK_INFO*)pbyData;
    KPlayer*                    pSelf   = NULL;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    pSelf->m_nUpperWeakLevel  = pPak->byUpper;
    pSelf->m_nMiddleWeakLevel = pPak->byMiddle;
    pSelf->m_nLowerWeakLevel  = pPak->byLower;

    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerWeakUpdate param = { g_pSO3World->m_dwClientPlayerID };
        g_pGameWorldUIHandler->OnPlayerWeakUpdate(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncSelectCharacterMaxLMR(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SELECT_CHARACTER_MAX_LMR*  pPak            = (S2C_SYNC_SELECT_CHARACTER_MAX_LMR*)pbyData;
    KCharacter*                         pTarget         = NULL;
    int                                 nLifePercent    = 0;
    int                                 nManaPercent    = 0;
    int                                 nRagePercent    = 0;

    if (IS_PLAYER(pPak->dwCharacterID))
        pTarget = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    else
        pTarget = g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);

    KG_PROCESS_ERROR(pTarget);

    // 在更新最大值之前,先将原来的当前值(根据比例同步算来的)记下来

    if (pTarget->m_nMaxLife > 0)
        nLifePercent = pTarget->m_nCurrentLife * UCHAR_MAX / pTarget->m_nMaxLife;

    if (pTarget->m_nMaxMana > 0)
        nManaPercent = pTarget->m_nCurrentMana * UCHAR_MAX / pTarget->m_nMaxMana;

    if (pTarget->m_nMaxRage > 0)
        nRagePercent = pTarget->m_nCurrentRage * UCHAR_MAX / pTarget->m_nMaxRage;

    pTarget->m_nMaxLife = pPak->nMaxLife;
    pTarget->m_nMaxMana = pPak->nMaxMana;
    pTarget->m_nMaxRage = pPak->nMaxRage;

    pTarget->m_nCurrentLife = pTarget->m_nMaxLife * nLifePercent / UCHAR_MAX;
    pTarget->m_nCurrentMana = pTarget->m_nMaxMana * nManaPercent / UCHAR_MAX;
    pTarget->m_nCurrentRage = pTarget->m_nMaxRage * nRagePercent / UCHAR_MAX;

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[LMR] Target max %s --> (%d / %d)\n", 
    //    pTarget->m_szName, pTarget->m_nCurrentLife, pTarget->m_nMaxLife
    //);

    if (g_pGameWorldUIHandler)
    {
        if (IS_PLAYER(pPak->dwCharacterID))
        {
            KUIEventPlayerStateUpdate param = { pTarget->m_dwID };
            g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
        }
        else
        {
            KUIEventNpcStateUpdate param = { pTarget->m_dwID };
            g_pGameWorldUIHandler->OnNpcStateUpdate(param);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncSelectCharacterCurrentLMR(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SELECT_CHARACTER_CURRENT_LMR*  pPak    = (S2C_SYNC_SELECT_CHARACTER_CURRENT_LMR*)pbyData;
    KCharacter*                             pTarget = NULL;

    if (IS_PLAYER(pPak->dwCharacterID))
        pTarget = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    else
        pTarget = g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);

    KG_PROCESS_ERROR(pTarget);

    // 注意,当前血量可能比最大血量数值先到,所以这里要处理这种情况

    if (pTarget->m_nMaxLife == 0)
    {
        pTarget->m_nMaxLife = UCHAR_MAX;
    }

    if (pTarget->m_nMaxMana == 0)
    {
        pTarget->m_nMaxMana = UCHAR_MAX;
    }

    if (pTarget->m_nMaxRage == 0)
    {
        pTarget->m_nMaxRage = UCHAR_MAX;
    }

    pTarget->m_nCurrentLife = pTarget->m_nMaxLife * pPak->byLifePercent / UCHAR_MAX;
    pTarget->m_nCurrentMana = pTarget->m_nMaxMana * pPak->byManaPercent / UCHAR_MAX;
    pTarget->m_nCurrentRage = pTarget->m_nMaxRage * pPak->byRagePercent / UCHAR_MAX;

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[LMR] Target current %s --> (%d / %d)\n", 
    //    pTarget->m_szName, pTarget->m_nCurrentLife, pTarget->m_nMaxLife
    //);

    if (g_pGameWorldUIHandler)
    {
        if (IS_PLAYER(pPak->dwCharacterID))
        {
            KUIEventPlayerStateUpdate param = { pTarget->m_dwID };
            g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
        }
        else
        {
            KUIEventNpcStateUpdate param = { pTarget->m_dwID };
            g_pGameWorldUIHandler->OnNpcStateUpdate(param);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncSelectCharacterWeakInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_SELECT_CHARACTER_WEAK_INFO*    pPak    = (S2C_SYNC_SELECT_CHARACTER_WEAK_INFO*)pbyData;
    KCharacter*                             pTarget = NULL;

    if (IS_PLAYER(pPak->dwCharacterID))
        pTarget = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    else
        pTarget = g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);

    KG_PROCESS_ERROR(pTarget);

    pTarget->m_nUpperWeakLevel  = pPak->byUpper;
    pTarget->m_nMiddleWeakLevel = pPak->byMiddle;
    pTarget->m_nLowerWeakLevel  = pPak->byLower;

    if (g_pGameWorldUIHandler)
    {
        if (IS_PLAYER(pPak->dwCharacterID))
        {
            KUIEventPlayerWeakUpdate param = { pTarget->m_dwID };
            g_pGameWorldUIHandler->OnPlayerWeakUpdate(param);
        }
        else
        {
            KUIEventNpcWeakUpdate param = { pTarget->m_dwID };
            g_pGameWorldUIHandler->OnNpcWeakUpdate(param);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncTeamMemberMaxLMR(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_TEAM_MEMBER_MAX_LMR*   pPak            = (S2C_SYNC_TEAM_MEMBER_MAX_LMR*)pbyData;
    KPlayer*                        pClientPlayer   = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;
    KPlayer*                        pPlayer         = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nMaxLife = pPak->nMaxLife;
    pMemberInfo->nMaxMana = pPak->nMaxMana;
    pMemberInfo->nMaxRage = pPak->nMaxRage;

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[LMR] Team max %s --> (%dp / %dm)\n", 
    //    pMemberInfo->szRoleName, pMemberInfo->nLifePercent, pMemberInfo->nMaxLife
    //);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateTeamMemberLMR param  = 
		{ 
			g_pSO3World->m_TeamClient.m_dwTeamID, 
			pPak->dwMemberID,
		};
        g_pGameWorldUIHandler->OnUpdateTeamMemberLMR(param);
	}

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwMemberID);
    KG_PROCESS_ERROR(pPlayer && pPlayer != pClientPlayer);

    pPlayer->m_nMaxLife = pPak->nMaxLife;
    pPlayer->m_nMaxMana = pPak->nMaxMana;
    pPlayer->m_nMaxRage = pPak->nMaxRage;

    if (g_pGameWorldUIHandler)
    {
		KUIEventPlayerStateUpdate param = { pPlayer->m_dwID };
		g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }
    
Exit0:
    return;
}

void KPlayerClient::OnSyncTeamMemberCurrentLMR(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_TEAM_MEMBER_CURRENT_LMR*   pPak            = (S2C_SYNC_TEAM_MEMBER_CURRENT_LMR*)pbyData;
    KPlayer*                            pClientPlayer   = NULL;
    KTEAM_MEMBER_INFO*                  pMemberInfo     = NULL;
    KPlayer*                            pPlayer         = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    // 这里,最大血量可能还没有下来,所以要做处理
    if (pMemberInfo->nMaxLife == 0)
    {
        pMemberInfo->nMaxLife = UCHAR_MAX;
    }

    if (pMemberInfo->nMaxMana == 0)
    {
        pMemberInfo->nMaxMana = UCHAR_MAX;
    }

    if (pMemberInfo->nMaxRage == 0)
    {
        pMemberInfo->nMaxRage = UCHAR_MAX;
    }

    pMemberInfo->nLifePercent = pPak->byLifePercent;
    pMemberInfo->nManaPercent = pPak->byManaPercent;
    pMemberInfo->nRagePercent = pPak->byRagePercent;

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[LMR] Team current %s --> (%dp / %dm)\n", 
    //    pMemberInfo->szRoleName, pMemberInfo->nLifePercent, pMemberInfo->nMaxLife
    //);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateTeamMemberLMR param  = 
		{ 
			g_pSO3World->m_TeamClient.m_dwTeamID, 
			pPak->dwMemberID,
		};
		g_pGameWorldUIHandler->OnUpdateTeamMemberLMR(param);
	}

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwMemberID);
    KG_PROCESS_ERROR(pPlayer && pPlayer != pClientPlayer);

    pPlayer->m_nCurrentLife = pMemberInfo->nLifePercent * pMemberInfo->nMaxLife / UCHAR_MAX;
    pPlayer->m_nCurrentMana = pMemberInfo->nManaPercent * pMemberInfo->nMaxMana / UCHAR_MAX;
    pPlayer->m_nCurrentRage = pMemberInfo->nRagePercent * pMemberInfo->nMaxRage / UCHAR_MAX;

    pPlayer->m_nMaxLife     = pMemberInfo->nMaxLife;
    pPlayer->m_nMaxMana     = pMemberInfo->nMaxMana;
    pPlayer->m_nMaxRage     = pMemberInfo->nMaxRage;

    if (g_pGameWorldUIHandler)
    {
		KUIEventPlayerStateUpdate param = { pPlayer->m_dwID };
		g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncTeamMemberMisc(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_TEAM_MEMBER_MISC*      pPak            = (S2C_SYNC_TEAM_MEMBER_MISC*)pbyData;
    KPlayer*                        pClientPlayer   = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->PortraitInfo   = pPak->PortraitInfo;
    pMemberInfo->dwForceID      = pPak->dwForceID;
    pMemberInfo->eCamp          = (KCAMP)pPak->byCamp;
    pMemberInfo->nLevel         = pPak->byLevel;
	pMemberInfo->nRoleType      = pPak->byRoleType;
    pMemberInfo->bDeathFlag     = pPak->bDeathFlag;

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateTeamMemberInfo param = { g_pSO3World->m_TeamClient.m_dwTeamID, pPak->dwMemberID };
        g_pGameWorldUIHandler->OnUpdateTeamMemberInfo(param);
	}

Exit0:
    return;
}

void KPlayerClient::OnSyncTeamMemberPosition(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_TEAM_MEMBER_POSITION*  pPak            = (S2C_SYNC_TEAM_MEMBER_POSITION*)pbyData;
    KPlayer*                        pClientPlayer   = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->dwMapID        = pPak->dwMapID;
    pMemberInfo->nMapCopyIndex  = pPak->nMapCopyIndex;
    pMemberInfo->nPosX          = pPak->nPosX;
    pMemberInfo->nPosY          = pPak->nPosY;

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateTeamMemberInfo param = { g_pSO3World->m_TeamClient.m_dwTeamID, pPak->dwMemberID };
        g_pGameWorldUIHandler->OnUpdateTeamMemberPosition(param);
	}

Exit0:
    return;
}

void KPlayerClient::OnBroadcastCharacterLife(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                            bRetCode        = false;
	S2C_BROADCAST_CHARACTER_LIFE*   pPak            = (S2C_BROADCAST_CHARACTER_LIFE*)pbyData;
    KCharacter*                     pCharacter      = NULL;
    KPlayer*                        pClientPlayer   = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    if (IS_PLAYER(pPak->dwCharacterID))
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(pPak->dwCharacterID);
    else
        pCharacter = g_pSO3World->m_NpcSet.GetObj(pPak->dwCharacterID);
    KG_PROCESS_ERROR(pCharacter);

    if (pCharacter->m_nMaxLife <= 0)
    {
        pCharacter->m_nMaxLife = UCHAR_MAX;
    }

	pCharacter->m_nCurrentLife = pCharacter->m_nMaxLife * pPak->byLifePercent / UCHAR_MAX;

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[LMR] Broadcast %s --> (%d / %d)\n", 
    //    pCharacter->m_szName, pCharacter->m_nCurrentLife, pCharacter->m_nMaxLife
    //);

    if (g_pGameWorldUIHandler)
    {
        if (IS_PLAYER(pPak->dwCharacterID))
        {
            KUIEventPlayerStateUpdate param = { pCharacter->m_dwID };
            g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
        }
        else
        {
            KUIEventNpcStateUpdate param = { pCharacter->m_dwID };
            g_pGameWorldUIHandler->OnNpcStateUpdate(param);
        }
    }

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pPak->dwCharacterID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nLifePercent = pPak->byLifePercent;

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateTeamMemberLMR param = { g_pSO3World->m_TeamClient.m_dwTeamID, pPak->dwCharacterID };
        g_pGameWorldUIHandler->OnUpdateTeamMemberLMR(param);
	}

Exit0:
	return;
}


// ------------------- party about ---------------------------

void KPlayerClient::OnSyncPlayerTeamBaseInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                                  nRetCode   = false;
    S2C_SYNC_PLAYER_TEAM_BASE_INFO*      pBaseInfo  = (S2C_SYNC_PLAYER_TEAM_BASE_INFO*)pbyData;;
    KPlayer*                             pPlayer    = NULL;

    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);
    KGLOG_PROCESS_ERROR(g_pGameWorldRepresentHandler);

    if (g_pSO3World->m_TeamClient.m_dwTeamID != ERROR_ID)
        g_pSO3World->m_TeamClient.Destroy();
    
    nRetCode = g_pSO3World->m_TeamClient.Create();
    KGLOG_PROCESS_ERROR(nRetCode);

    g_pSO3World->m_TeamClient.m_dwTeamID                    = pBaseInfo->dwTeamID;
    g_pSO3World->m_TeamClient.m_bSystem                     = pBaseInfo->bSystem;
    g_pSO3World->m_TeamClient.m_dwAuthority[tatLeader]      = pBaseInfo->dwLeaderID;
    g_pSO3World->m_TeamClient.m_nGroupNum                   = pBaseInfo->nGroupNum;
    g_pSO3World->m_TeamClient.m_nLootMode                   = pBaseInfo->nLootMode;
    g_pSO3World->m_TeamClient.m_nRollQuality                = pBaseInfo->nRollQuality;
    g_pSO3World->m_TeamClient.m_eCamp                       = (KCAMP)pBaseInfo->byCamp;
    g_pSO3World->m_TeamClient.m_dwAuthority[tatDistribute]  = pBaseInfo->dwDistributeMan;
    g_pSO3World->m_TeamClient.m_dwAuthority[tatMark]        = pBaseInfo->dwMarkMan;

    for (int i = 0; i < g_pSO3World->m_TeamClient.m_nGroupNum; i++)
    {
        g_pSO3World->m_TeamClient.m_TeamGroup[i].dwFormationLeader = pBaseInfo->dwFormationLeader[i];
    }

    KGLogPrintf(
        KGLOG_DEBUG, 
        "[Team] Sync party base info: TeamID(0x%.8x), LeaderID(%u), LootMode(%d), RollQuality(%d)\n", 
        pBaseInfo->dwTeamID, 
        pBaseInfo->dwLeaderID,
        pBaseInfo->nLootMode,
		pBaseInfo->nRollQuality
    );

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    pPlayer->SetCurrentTeamID(g_pSO3World->m_TeamClient.m_bSystem, pBaseInfo->dwTeamID);

    memcpy(g_pSO3World->m_TeamClient.m_dwTeamMark, pBaseInfo->dwTeamMark, sizeof(g_pSO3World->m_TeamClient.m_dwTeamMark));

    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        KCharacter* pCharacter = NULL;

        if (!g_pSO3World->m_TeamClient.m_dwTeamMark[i])
            continue;

        if (IS_PLAYER(g_pSO3World->m_TeamClient.m_dwTeamMark[i]))
        {
            pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_TeamClient.m_dwTeamMark[i]);
        }
        else
        {
            pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(g_pSO3World->m_TeamClient.m_dwTeamMark[i]);
        }
        if (!pCharacter)
            continue;

        g_pGameWorldRepresentHandler->OnCharacterUpdateTitleMark(pCharacter);
    }

    for (int i = 0; i < pBaseInfo->byMemberCount; i++)
    {
        KTEAM_MEMBER_INFO* pMemberInfo      = NULL;
        KPlayer*           pMemberPlayer    = NULL;

        nRetCode = g_pSO3World->m_TeamClient.AddMember(
            pBaseInfo->MemberInfo[i].dwMemberID, pBaseInfo->MemberInfo[i].byGroupIndex,
            pBaseInfo->MemberInfo[i].szMemberName
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pBaseInfo->MemberInfo[i].dwMemberID);
        KGLOG_PROCESS_ERROR(pMemberInfo);

        pMemberInfo->dwMemberID               = pBaseInfo->MemberInfo[i].dwMemberID;
        pMemberInfo->PortraitInfo             = pBaseInfo->MemberInfo[i].PortraitInfo;
        pMemberInfo->dwMapID                  = pBaseInfo->MemberInfo[i].dwMapID;
        pMemberInfo->dwForceID                = pBaseInfo->MemberInfo[i].byForceID;
        pMemberInfo->nLevel                   = pBaseInfo->MemberInfo[i].byLevel;
        pMemberInfo->bOnlineFlag              = pBaseInfo->MemberInfo[i].byOnLineFlag;
        pMemberInfo->nMaxLife                 = pBaseInfo->MemberInfo[i].nMaxLife;
        pMemberInfo->nMaxMana                 = pBaseInfo->MemberInfo[i].nMaxMana;
        pMemberInfo->nMaxRage                 = pBaseInfo->MemberInfo[i].nMaxRage;
        pMemberInfo->nLifePercent             = pBaseInfo->MemberInfo[i].byLifePercent;
        pMemberInfo->nManaPercent             = pBaseInfo->MemberInfo[i].byManaPercent;
        pMemberInfo->nRagePercent             = pBaseInfo->MemberInfo[i].byRagePercent;
        pMemberInfo->dwMapID                  = pBaseInfo->MemberInfo[i].dwMapID;
        pMemberInfo->nMapCopyIndex            = pBaseInfo->MemberInfo[i].nMapCopyIndex;
        pMemberInfo->nPosX                    = pBaseInfo->MemberInfo[i].nPosX;
        pMemberInfo->nPosY                    = pBaseInfo->MemberInfo[i].nPosY;
        pMemberInfo->nRoleType                = pBaseInfo->MemberInfo[i].byRoleType;
        pMemberInfo->bDeathFlag               = pBaseInfo->MemberInfo[i].byDeathFlag;
        pMemberInfo->nFormationCoefficient    = 0;

        pBaseInfo->MemberInfo[i].szMemberName[sizeof(pBaseInfo->MemberInfo[i].szMemberName) - 1] = '\0';
        strncpy(pMemberInfo->szRoleName, pBaseInfo->MemberInfo[i].szMemberName, sizeof(pMemberInfo->szRoleName));
        pMemberInfo->szRoleName[sizeof(pMemberInfo->szRoleName) - 1] = '\0';

        KGLogPrintf(
            KGLOG_DEBUG, 
            "[Team] Sync party member, TeamID(0x%.8x), MemberID(%u), MemberName(\"%s\")\n", 
            pBaseInfo->dwTeamID,
            pMemberInfo->dwMemberID,
            pMemberInfo->szRoleName
        );

        pMemberPlayer = g_pSO3World->m_PlayerSet.GetObj(pMemberInfo->dwMemberID);
        if (pMemberPlayer)
        {
            DoApplyPlayerBuffList(pMemberInfo->dwMemberID);
        }

    }

    if (g_pGameWorldUIHandler)
    {
        KUIEventSyncTeamBaseInfo   UISyncTeamNotify;

        UISyncTeamNotify.dwTeamID           = pBaseInfo->dwTeamID;
        UISyncTeamNotify.dwLeaderID          = pBaseInfo->dwLeaderID;
        UISyncTeamNotify.nLootMode           = pBaseInfo->nLootMode;
        UISyncTeamNotify.nRollQuality        = pBaseInfo->nRollQuality;
        UISyncTeamNotify.bAddTeamMemberFlag = pBaseInfo->bAddTeamMemberFlag;

        g_pGameWorldUIHandler->OnSyncTeamBaseInfo(UISyncTeamNotify);

        for (int i = 0; i < pBaseInfo->byMemberCount; i++)
        {
            KPlayer*    pClientPlayer       = NULL;
            KCharacter* pSelectCharacter    = NULL;
            BOOL        bRetCode            = false;

            KUIEventTeamSyncMember     UITeamSyncMemberNotify;

            UITeamSyncMemberNotify.dwTeamID   = pBaseInfo->dwTeamID;
            UITeamSyncMemberNotify.dwMemberID  = pBaseInfo->MemberInfo[i].dwMemberID;
            UITeamSyncMemberNotify.nGroupIndex = pBaseInfo->MemberInfo[i].byGroupIndex;

            g_pGameWorldUIHandler->OnSyncTeamMemberData(UITeamSyncMemberNotify);

            g_pGameWorldUIHandler->OnUpdateAllRelation();
            g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();

            pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
            KGLOG_PROCESS_ERROR(pClientPlayer);

            bRetCode = pClientPlayer->m_SelectTarget.GetTarget(&pSelectCharacter);
            if (bRetCode)
            {
                KUIEventUpdateRelation UIEventUpdateRelation;
                UIEventUpdateRelation.dwCharacterID = pSelectCharacter->m_dwID;
                g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);

                g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pSelectCharacter);
            }
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnInvitePlayerJoinTeamRequest(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                             nRetCode     = false;
    S2C_INVITE_JOIN_TEAM_REQUEST*   pTeamInvite = (S2C_INVITE_JOIN_TEAM_REQUEST*)pbyData;;

    KGLogPrintf(KGLOG_DEBUG, "[Team] Recv party invite from \"%s\" !\n", pTeamInvite->szInviteSrc);

    // UI: 显示组队邀请
    if (g_pGameWorldUIHandler)
    {
        KUIEventTeamInviteRequest UITeamInviteNotify;

        strncpy(UITeamInviteNotify.szInviteSrc, pTeamInvite->szInviteSrc, sizeof(UITeamInviteNotify.szInviteSrc));
        UITeamInviteNotify.szInviteSrc[sizeof(UITeamInviteNotify.szInviteSrc) - 1] = '\0';

        g_pGameWorldUIHandler->OnTeamInviteRequest(UITeamInviteNotify);
    }

Exit0:
    return;
}
    
void KPlayerClient::OnApplyJoinTeamRequest(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                           nRetCode    = false;
    S2C_APPLY_JOIN_TEAM_REQUEST*  pTeamApply = (S2C_APPLY_JOIN_TEAM_REQUEST*)pbyData;

    KGLogPrintf(KGLOG_DEBUG, "[Team] Recv party apply from \"%s\" !\n", pTeamApply->szApplySrc);

    // UI: 显示入队申请
    if (g_pGameWorldUIHandler) 
    {
        KUIEventTeamApplyRequest UITeamApply;

        strncpy(UITeamApply.szApplySrc, pTeamApply->szApplySrc, sizeof(UITeamApply.szApplySrc));
        UITeamApply.szApplySrc[sizeof(UITeamApply.szApplySrc) - 1] = '\0';

        g_pGameWorldUIHandler->OnTeamApplyRequest(UITeamApply);
    }

Exit0:
    return;
}

void KPlayerClient::OnTeamAddMemberNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                             nRetCode            = false;
    S2C_TEAM_ADD_MEMBER_NOTIFY*     pAddMemberNotify    = (S2C_TEAM_ADD_MEMBER_NOTIFY*)pbyData;
    KPlayer*                        pSelf               = NULL;
    KPlayer*                        pMemberPlayer       = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo         = NULL;

    pSelf   = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    // 自己加入队伍时不走这个途径通知
    KG_PROCESS_ERROR(pAddMemberNotify->dwMemberID != g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(pAddMemberNotify->dwTeamID == g_pSO3World->m_TeamClient.m_dwTeamID);

    nRetCode = g_pSO3World->m_TeamClient.AddMember(
        pAddMemberNotify->dwMemberID, pAddMemberNotify->nGroupIndex, pAddMemberNotify->szMemberName
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLogPrintf(
        KGLOG_DEBUG, 
        "[Team] Team add member:  TeamID(0x%.8x), MemberID(%u), Group(%d) MemberName(\"%s\")\n", 
        pAddMemberNotify->dwTeamID,
        pAddMemberNotify->dwMemberID,
        pAddMemberNotify->nGroupIndex,
        pAddMemberNotify->szMemberName 
    );

    pMemberPlayer = g_pSO3World->m_PlayerSet.GetObj(pAddMemberNotify->dwMemberID);
    if (pMemberPlayer)
    {
        DoApplyPlayerBuffList(pAddMemberNotify->dwMemberID);
    }

    //UI通知:
    KG_PROCESS_ERROR(g_pGameWorldUIHandler);
    KG_PROCESS_ERROR(g_pGameWorldRepresentHandler);

    {
        KPlayer*    pClientPlayer       = NULL;
        KCharacter* pSelectCharacter    = NULL;
        BOOL        bRetCode            = false;
        KUIEventTeamAddMember UIAddMemberNotify;

        UIAddMemberNotify.dwTeamID   = pAddMemberNotify->dwTeamID;
        UIAddMemberNotify.dwMemberID  = pAddMemberNotify->dwMemberID;
        UIAddMemberNotify.nGroupIndex = pAddMemberNotify->nGroupIndex;

        g_pGameWorldUIHandler->OnTeamAddMember(UIAddMemberNotify);

        g_pGameWorldUIHandler->OnUpdateAllRelation();
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();

        pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pClientPlayer);

        bRetCode = pClientPlayer->m_SelectTarget.GetTarget(&pSelectCharacter);
        if (bRetCode)
        {
            KUIEventUpdateRelation UIEventUpdateRelation;
            UIEventUpdateRelation.dwCharacterID = pSelectCharacter->m_dwID;
            g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);

            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pSelectCharacter);
        }
    }

Exit0:
    return;
}
    
void KPlayerClient::OnTeamDelMemberNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                                 nRetCode          = false;
    KPlayer*                            pMember           = NULL;
    S2C_TEAM_DEL_MEMBER_NOTIFY*         pDelMemberNotify  = (S2C_TEAM_DEL_MEMBER_NOTIFY*)pbyData;
    KTEAM_MEMBER_INFO*                  pTmpMemberInfo    = NULL;
    char                                szName[_NAME_LEN];

    KG_PROCESS_ERROR(pDelMemberNotify->dwTeamID == g_pSO3World->m_TeamClient.m_dwTeamID);

    pTmpMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(pTmpMemberInfo);

    strncpy(szName, pTmpMemberInfo->szRoleName, sizeof(szName));
    szName[sizeof(szName) - 1] = '\0';

    if (pDelMemberNotify->dwMemberID == g_pSO3World->m_dwClientPlayerID) // 如果是我自己离队
    {
        pMember = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pMember);
        pMember->SetCurrentTeamID(g_pSO3World->m_TeamClient.m_bSystem, ERROR_ID);

        nRetCode = g_pSO3World->m_TeamClient.Destroy();
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLogPrintf(
            KGLOG_DEBUG, 
            "[Team] Leave party: TeamID(0x%.8x), LeaderName(\"%s\")\n", 
            pDelMemberNotify->dwTeamID,
            szName
        );
    }
    else    // 如果只是普通玩家离队
    {
        pTmpMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pDelMemberNotify->dwMemberID);
        KG_PROCESS_ERROR(pTmpMemberInfo);

        strncpy(szName, pTmpMemberInfo->szRoleName, sizeof(szName));
        szName[sizeof(szName) - 1] = '\0';

        nRetCode = g_pSO3World->m_TeamClient.DelMember(pDelMemberNotify->dwMemberID, pDelMemberNotify->nGroupIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLogPrintf(
            KGLOG_DEBUG, 
            "[Team] Member leave party: TeamID(0x%.8x), MemberID(%u), MemberName(\"%s\")\n", 
            pDelMemberNotify->dwTeamID,
            pDelMemberNotify->dwMemberID,
            szName
        );
    }

    KG_PROCESS_ERROR(g_pGameWorldUIHandler);
    KG_PROCESS_ERROR(g_pGameWorldRepresentHandler);

	{
        KPlayer*    pClientPlayer       = NULL;
        KCharacter* pSelectCharacter    = NULL;
        BOOL        bRetCode            = false;
		KUIEventTeamDelMember UIDelMemberNotify;

		UIDelMemberNotify.dwTeamID   = pDelMemberNotify->dwTeamID;
		UIDelMemberNotify.dwMemberID  = pDelMemberNotify->dwMemberID;
        UIDelMemberNotify.nGroupIndex = pDelMemberNotify->nGroupIndex;
		strncpy(UIDelMemberNotify.szName, szName, _countof(UIDelMemberNotify.szName));
		UIDelMemberNotify.szName[_countof(UIDelMemberNotify.szName) - 1] = '\0';

		g_pGameWorldUIHandler->OnTeamDelMember(UIDelMemberNotify);
    
        g_pGameWorldUIHandler->OnUpdateAllRelation();
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();

        pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pClientPlayer);

        bRetCode = pClientPlayer->m_SelectTarget.GetTarget(&pSelectCharacter);
        if (bRetCode)
        {
            KUIEventUpdateRelation UIEventUpdateRelation;
            UIEventUpdateRelation.dwCharacterID = pSelectCharacter->m_dwID;
            g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);

            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pSelectCharacter);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnTeamChangeAuthorityNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                                 nRetCode        = false;
    S2C_TEAM_CHANGE_AUTHORITY_NOTIFY*   pNotify         = (S2C_TEAM_CHANGE_AUTHORITY_NOTIFY*)pbyData;
    KPlayer*                            pSelf           = NULL;
    KTEAM_MEMBER_INFO*                  pOldTeamLeader = NULL;
    KTEAM_MEMBER_INFO*                  pNewTeamLeader = NULL;

    pSelf   = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    KG_PROCESS_ERROR(pNotify->dwTeamID == g_pSO3World->m_TeamClient.m_dwTeamID);

    pOldTeamLeader = g_pSO3World->m_TeamClient.GetMemberInfo(g_pSO3World->m_TeamClient.m_dwAuthority[pNotify->byType]);

    g_pSO3World->m_TeamClient.m_dwAuthority[pNotify->byType] = pNotify->dwNewID;

    pNewTeamLeader = g_pSO3World->m_TeamClient.GetMemberInfo(pNotify->dwNewID);

    if (pOldTeamLeader && pNewTeamLeader)
    {
        KGLogPrintf(
            KGLOG_DEBUG, 
            "[Team] Change party authority(%d) \"%s\" ==> \"%s\"!", 
            pNotify->byType,
            pOldTeamLeader->szRoleName, 
            pNewTeamLeader->szRoleName
        );
    }
       
    // UI通知
    if (g_pGameWorldUIHandler)
    {
        KUIEventTeamAuthorityChanged param;

        param.nAuthorityType    = pNotify->byType;
        param.dwTeamID          = pNotify->dwTeamID;
        param.dwNewAuthorityID  = pNotify->dwNewID;
        param.dwOldAuthorityID  = ERROR_ID;

        if (pOldTeamLeader)
        {
            param.dwOldAuthorityID  = pOldTeamLeader->dwMemberID;
        }

        g_pGameWorldUIHandler->OnTeamAuthorityChanged(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnTeamSetLootModeNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                             nRetCode        = false;
    S2C_TEAM_SET_LOOT_MODE_NOTIFY*  pNotify         = (S2C_TEAM_SET_LOOT_MODE_NOTIFY*)pbyData;
    KPlayer*                        pSelf           = NULL;
	int								nOldLootMode    = 0;
	int								nOldRollQuality = 0;

    pSelf   = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    KG_PROCESS_ERROR(pNotify->dwTeamID == g_pSO3World->m_TeamClient.m_dwTeamID);

	nOldLootMode = g_pSO3World->m_TeamClient.m_nLootMode;
	nOldRollQuality = g_pSO3World->m_TeamClient.m_nRollQuality;

	if (pNotify->nLootMode != nOldLootMode)
	{
		g_pSO3World->m_TeamClient.m_nLootMode = pNotify->nLootMode;

		KGLogPrintf(KGLOG_DEBUG, "[Team] Team loot mode is set to [%d]\n", pNotify->nLootMode);

        if (g_pGameWorldUIHandler)
		{
			KUIEventTeamLootModeChanged UILootModeChangeNotify;

			UILootModeChangeNotify.dwTeamID = pNotify->dwTeamID;
			UILootModeChangeNotify.nLootMode = pNotify->nLootMode;

			g_pGameWorldUIHandler->OnTeamLootModeChanged(UILootModeChangeNotify);
		}
	}

	if (pNotify->nRollQuality != nOldRollQuality)
	{
		g_pSO3World->m_TeamClient.m_nRollQuality = pNotify->nRollQuality;

		KGLogPrintf(KGLOG_DEBUG, "[Team] Team roll quality is set to [%d]\n", pNotify->nRollQuality);

        if (g_pGameWorldUIHandler)
		{
			KUIEventTeamRollQualityChanged UIRollQualityChangedNotify;

			UIRollQualityChangedNotify.dwTeamID = pNotify->dwTeamID;
			UIRollQualityChangedNotify.nRollQuality = pNotify->nRollQuality;

			g_pGameWorldUIHandler->OnTeamRollQualityChanged(UIRollQualityChangedNotify);
		}
	}
Exit0:
    return;
}
    
void KPlayerClient::OnTeamSetFormationLeaderNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                                     nRetCode    = false;
    S2C_TEAM_SET_FORMATION_LEADER_NOTIFY*   pNotify     = (S2C_TEAM_SET_FORMATION_LEADER_NOTIFY*)pbyData;
    KPlayer*                                pSelf       = NULL;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    KG_PROCESS_ERROR(pNotify->dwTeamID == g_pSO3World->m_TeamClient.m_dwTeamID);

    KGLOG_PROCESS_ERROR(pNotify->nGroupIndex >= 0 && pNotify->nGroupIndex < g_pSO3World->m_TeamClient.m_nGroupNum);

    KGLOG_PROCESS_ERROR(
        g_pSO3World->m_TeamClient.m_TeamGroup[pNotify->nGroupIndex].dwFormationLeader != pNotify->dwNewFormationLeader
    );

    if (pNotify->dwNewFormationLeader != ERROR_ID)
    {
        nRetCode = g_pSO3World->m_TeamClient.IsPlayerInTeam(pNotify->dwNewFormationLeader);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = g_pSO3World->m_TeamClient.SetFormationLeader(pNotify->nGroupIndex, pNotify->dwNewFormationLeader);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        g_pSO3World->m_TeamClient.m_TeamGroup[pNotify->nGroupIndex].dwFormationLeader = ERROR_ID;
    }

    if (g_pGameWorldUIHandler)
    {
	    KUIEventSetFormationLeader Event = { pNotify->dwNewFormationLeader };

	    g_pGameWorldUIHandler->OnTeamSetFormationLeaderNotify(Event);
    }

Exit0:
    return;
}

void KPlayerClient::OnTeamDisbandNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                         nRetCode               = false;
    S2C_TEAM_DISBAND_NOTIFY*    pTeamDisbandNotify    = (S2C_TEAM_DISBAND_NOTIFY*)pbyData;
    KPlayer*                    pPlayer                = NULL;


    KG_PROCESS_ERROR(pTeamDisbandNotify->dwTeamID == g_pSO3World->m_TeamClient.m_dwTeamID);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->SetCurrentTeamID(g_pSO3World->m_TeamClient.m_bSystem, ERROR_ID);

    nRetCode = g_pSO3World->m_TeamClient.Destroy();
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLogPrintf(KGLOG_DEBUG, "[Team] Team disband !\n");

    // UI: 你的队伍解散了
    KG_PROCESS_ERROR(g_pGameWorldUIHandler);
    KG_PROCESS_ERROR(g_pGameWorldRepresentHandler);

    {
        KPlayer*    pClientPlayer       = NULL;
        KCharacter* pSelectCharacter    = NULL;
        BOOL        bRetCode            = false;
        KUIEventTeamDisband UITeamDisbandNotify;

        UITeamDisbandNotify.dwTeamID = pTeamDisbandNotify->dwTeamID;
        g_pGameWorldUIHandler->OnTeamDisband(UITeamDisbandNotify);
    
        g_pGameWorldUIHandler->OnUpdateAllRelation();
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();

        pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pClientPlayer);

        bRetCode = pClientPlayer->m_SelectTarget.GetTarget(&pSelectCharacter);
        if (bRetCode)
        {
            KUIEventUpdateRelation UIEventUpdateRelation;
            UIEventUpdateRelation.dwCharacterID = pSelectCharacter->m_dwID;
            g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);

            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pSelectCharacter);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncTeamMemberOnlineFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                                 nRetCode          = false;
    S2C_SYNC_TEAM_MEMBER_ONLINE_FLAG*   pMemberOnlineInfo = (S2C_SYNC_TEAM_MEMBER_ONLINE_FLAG*)pbyData;
    KTEAM_MEMBER_INFO*                  pMember           = NULL;

    pMember = g_pSO3World->m_TeamClient.GetMemberInfo(pMemberOnlineInfo->dwMemberID);
    KGLOG_PROCESS_ERROR(pMember);
    pMember->bOnlineFlag = pMemberOnlineInfo->bOnlineFlag;

    KGLogPrintf(
        KGLOG_DEBUG, 
        "[Team] Team member (\"%s\") turn %s !\n", 
        pMember->szRoleName, 
        pMemberOnlineInfo->bOnlineFlag ? "online" : "offline"
    );

    if (g_pGameWorldUIHandler)
    {
        KUIEventSetTeamMemberOnlineFlag UISetTeamMemberOnlineFlag;

        UISetTeamMemberOnlineFlag.dwTeamID    = pMemberOnlineInfo->dwTeamID;
        UISetTeamMemberOnlineFlag.dwMemberID   = pMemberOnlineInfo->dwMemberID;
        UISetTeamMemberOnlineFlag.bOnlineFlag  = pMemberOnlineInfo->bOnlineFlag;

        g_pGameWorldUIHandler->OnSetTeamMemberOnlineFlag(UISetTeamMemberOnlineFlag);
    }

Exit0:
    return;
}

void KPlayerClient::OnTeamMemberSignpostNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                               nRetCode  = false;
    S2C_TEAM_MEMBER_SIGNPOST_NOTIFY*  pPak      = (S2C_TEAM_MEMBER_SIGNPOST_NOTIFY*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventTeamMemberSignpostNotify param = { pPak->nX, pPak->nY };
        g_pGameWorldUIHandler->OnTeamMemberSignpostNotify(param);
    }
}

void KPlayerClient::OnTeamSetMarkRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_TEAM_SET_MARK_RESPOND* pPak    = (S2C_TEAM_SET_MARK_RESPOND*)pbyData;

    g_pSO3World->m_TeamClient.SetTeamMark(pPak->nMarkType, pPak->dwTargetID);
	if (g_pGameWorldUIHandler)
	{
		g_pGameWorldUIHandler->OnTeamSetMark();
	}
Exit0:
    return;
}

void KPlayerClient::OnTeamCampChange(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_TEAM_CAMP_CHANGE* pTeamCampChange   = (S2C_TEAM_CAMP_CHANGE*)pbyData;
    
    g_pSO3World->m_TeamClient.m_eCamp = (KCAMP)pTeamCampChange->byCamp;
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnTeamCampChange();
    }
}

void KPlayerClient::OnTeamLevelUpRaidNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_TEAM_LEVEL_UP_RAID_NOTIFY* pNotify = (S2C_TEAM_LEVEL_UP_RAID_NOTIFY*)pbyData;

    KG_PROCESS_ERROR(g_pSO3World->m_TeamClient.m_dwTeamID == pNotify->dwTeamID);

    g_pSO3World->m_TeamClient.m_nGroupNum = pNotify->byGroupNum;
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnTeamLevelUpRaid();
    }

Exit0:
    return;
}

void KPlayerClient::OnTeamChangeMemberGroupNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                 bRetCode = false;
    S2C_TEAM_CHANGE_MEMBER_GROUP_NOTIFY* pNotify  = (S2C_TEAM_CHANGE_MEMBER_GROUP_NOTIFY*)pbyData;

    KG_PROCESS_ERROR(g_pSO3World->m_TeamClient.m_dwTeamID == pNotify->dwTeamID);

    bRetCode = g_pSO3World->m_TeamClient.ChangeMember(
        pNotify->dwMemberID, pNotify->bySrcGroupIndex, pNotify->dwDstMemberID, pNotify->byDstGroupIndex
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        KUIEventTeamChangeMemberGroup param;

        param.dwMemberID     = pNotify->dwMemberID;
        param.dwDstMemberID  = pNotify->dwDstMemberID;
        param.nSrcGroupIndex = pNotify->bySrcGroupIndex;
        param.nDstGroupIndex = pNotify->byDstGroupIndex;

        g_pGameWorldUIHandler->OnTeamChangeMemberGroup(param);
    }

Exit0:
    return;
}
// <------------------- party about -------------------------

// ------------------- shop about ------------------------->

void KPlayerClient::OnOpenShopNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode   = false;
    S2C_OPEN_SHOP_NOTIFY*       pNotify    = (S2C_OPEN_SHOP_NOTIFY*)pbyData;
    KShop*                      pShop      = NULL;
    KNpc*                       pNpc       = NULL;

    bRetCode = g_pSO3World->m_ShopCenter.UpDateShopInfo(
        pNotify->dwShopTemplateID, pNotify->dwShopID, pNotify->cShopType, pNotify->byValidPage, pNotify->bCanRepair
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pNpc = g_pSO3World->m_NpcSet.GetObj(pNotify->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(pNotify->dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    pShop->m_pNpc    = pNpc;
    pShop->m_dwNpcID = pNpc->m_dwID;
    pNpc->m_pShop    = pShop;

    // UI: 通知打开商店界面
    if (g_pGameWorldUIHandler)
    {
        KUIEventShopOpenNotify UIShopOpenNotify;

        UIShopOpenNotify.dwShopID    = pNotify->dwShopID;
        UIShopOpenNotify.nShopType   = (int)pNotify->cShopType;
        UIShopOpenNotify.dwValidPage = (DWORD)pNotify->byValidPage;
        UIShopOpenNotify.bCanRepair  = pNotify->bCanRepair;
        UIShopOpenNotify.dwNpcID     = pNotify->dwNpcID;

        g_pGameWorldUIHandler->OnShopOpenNotify(UIShopOpenNotify);
    }

Exit0:
    return;
}


void KPlayerClient::OnUpdateShopItem(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                         bResult         = false;
    BOOL                         bRetCode        = false;
    S2C_UPDATE_SHOP_ITEM*        pUpdateShopItem = (S2C_UPDATE_SHOP_ITEM*)pbyData;
    KItem*                       pItem           = NULL;   
    KShop*                       pShop           = NULL;
    BOOL                         bNewItemFlag    = false;
    KSHOP_ITEM                   ShopItem;
    
    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(pUpdateShopItem->dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    
    ShopItem.pItem = NULL;

    if (pUpdateShopItem->wItemDataLength)
    {
        bRetCode = pShop->DeleteItem(pUpdateShopItem->byPageIndex, pUpdateShopItem->byPosIndex);
        KGLOG_PROCESS_ERROR(bRetCode);

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData( 
            pUpdateShopItem->dwItemID, pUpdateShopItem->byItemData, pUpdateShopItem->wItemDataLength
        );
        KGLOG_PROCESS_ERROR(pItem);
        
        ShopItem.pItem  = pItem;
        bNewItemFlag = true;
    }
    else if (pUpdateShopItem->dwItemID != 0)
    {
        ShopItem.pItem = pShop->m_ShopPages[pUpdateShopItem->byPageIndex].ShopItems[pUpdateShopItem->byPosIndex].pItem;
    }

    ShopItem.nCount = pUpdateShopItem->nCount;
    ShopItem.nItemTemplateIndex = pUpdateShopItem->byItemTemplateIndex;

    bRetCode = pShop->UpdateItem(pUpdateShopItem->byPageIndex, pUpdateShopItem->byPosIndex, ShopItem);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventShopUpdateItem UIShopUpdateItem;

        UIShopUpdateItem.dwShopID    = pUpdateShopItem->dwShopID;
        UIShopUpdateItem.dwPageIndex = (DWORD)pUpdateShopItem->byPageIndex;
        UIShopUpdateItem.dwPos       = (DWORD)pUpdateShopItem->byPosIndex;

        g_pGameWorldUIHandler->OnShopUpdateItem(UIShopUpdateItem);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bNewItemFlag && pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }
    }
    return;
}

// <------------------- shop about -------------------------

// -------------------- Trading ----------------------------->
void KPlayerClient::OnTradingInviteRequest(BYTE* pbyData, size_t nSize, int nFrame)
{

    S2C_TRADING_INVITE_REQUEST* pReqesut        = (S2C_TRADING_INVITE_REQUEST*)pbyData;
    KUIEventTradingInvite       TradingInvite;
        
    // UI通知
    if (g_pGameWorldUIHandler)
    {
        TradingInvite.dwTradingInviter = pReqesut->dwTradingInviteSrc;
        g_pGameWorldUIHandler->OnTradingInvite(TradingInvite);
    }
    
Exit0:
    return;
}

void KPlayerClient::OnOpenTradingBoxNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bResult                 = false;
    int                             nRetCode                = false;
    KTradingBox*                    pTradingBox             = NULL;
    KPlayer*                        pClientPlayer           = NULL;
    KPlayer*                        pOtherSide              = NULL;
    KTradingBox*                    pSelfTradingBox         = NULL;
    BOOL                            bSelfTradingBoxInit     = false;
    KTradingBox*                    pOtherSideBox           = NULL;
    BOOL                            bOtherSideBoxInit       = false;
    S2C_OPEN_TRADING_BOX_NOTIFY*    pOpenBoxNotify          = (S2C_OPEN_TRADING_BOX_NOTIFY*)pbyData;
    KUIEventTradingOpenNotify       OpenNotify;
    
    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    pOtherSide = g_pSO3World->m_PlayerSet.GetObj(pOpenBoxNotify->dwOtherPlayer);
    KGLOG_PROCESS_ERROR(pOtherSide);

    assert(!pClientPlayer->m_pTradingBox);
    pSelfTradingBox = KMemory::New<KTradingBox>();
    KGLOG_PROCESS_ERROR(pSelfTradingBox);

    nRetCode = pSelfTradingBox->Init(pClientPlayer, pOtherSide);
    KGLOG_PROCESS_ERROR(nRetCode);
    bSelfTradingBoxInit = true;

    pClientPlayer->m_pTradingBox = pSelfTradingBox;
    
    assert(!pOtherSide->m_pTradingBox);
    pOtherSideBox = KMemory::New<KTradingBox>();
    KGLOG_PROCESS_ERROR(pOtherSideBox);

    nRetCode = pOtherSideBox->Init(pOtherSide, pClientPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
    bOtherSideBoxInit = true;

    pOtherSide->m_pTradingBox = pOtherSideBox;

    // 通知UI打开交易面板
    if (g_pGameWorldUIHandler)
    {
        OpenNotify.dwCharacterID = pOpenBoxNotify->dwOtherPlayer;
        g_pGameWorldUIHandler->OnTradingOpenNotify(OpenNotify);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pSelfTradingBox)
        {
            pClientPlayer->m_pTradingBox = NULL;
            if (bSelfTradingBoxInit)
            {
                pSelfTradingBox->UnInit();
                bSelfTradingBoxInit = false;
            }
            KMemory::Delete(pSelfTradingBox);
            pSelfTradingBox = NULL;
        }

        if (pOtherSideBox)
        {
            pOtherSide->m_pTradingBox = NULL;
            if (bOtherSideBoxInit)
            {
                pOtherSideBox->UnInit();
                bOtherSideBoxInit = false;
            }
            KMemory::Delete(pOtherSideBox);
            pOtherSideBox = NULL;
        }
        
        // 打开面板失败, 则发送请求到服务端, 取消交易
        DoTradingConfirm(false, 0);
    }
    return;
}

void KPlayerClient::OnSyncTradingConfirm(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bResult          = 0;
    KPlayer*                        pPlayer          = NULL;
    KPlayer*                        pOtherSide       = NULL;
    S2C_SYNC_TRADING_CONFIRM*       pTradingConfirm  = (S2C_SYNC_TRADING_CONFIRM*)pbyData;

    assert(pTradingConfirm->dwPlayerID != g_pSO3World->m_dwClientPlayerID);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pTradingConfirm->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_pTradingBox);

    pOtherSide = pPlayer->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOtherSide);
    KGLOG_PROCESS_ERROR(pOtherSide->m_pTradingBox);

    if (!pTradingConfirm->bConfirm)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingClose TradingClose;
            g_pGameWorldUIHandler->OnTradingClose(TradingClose);

            KUIEventTradingRespond TradingRespond;
            TradingRespond.nRespondCode = trcTradingCanceled;
            g_pGameWorldUIHandler->OnTradingRespond(TradingRespond);
        }
        goto Exit1;
    }
    else
    {
        pPlayer->m_pTradingBox->Confirm();
    }

Exit1:
    bResult = true;
Exit0:
    if (!pTradingConfirm->bConfirm)
    {
        if (pPlayer && pPlayer->m_pTradingBox)
        {
            pPlayer->m_pTradingBox->ClearTradingInfo();
        }

        if (pOtherSide && pOtherSide->m_pTradingBox)
        {
            pOtherSide->m_pTradingBox->ClearTradingInfo();
        }
    }

    return;
}

void KPlayerClient::OnSyncTradingItemData(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                         nRetCode        = false;
    KItem*                      pItem           = NULL;
    DWORD                       dwGridIndex     = 0;
    DWORD                       dwBoxIndex      = 0;
    DWORD                       dwPosIndex      = 0;
    KPlayer*                    pClientPlayer   = NULL;
    KPlayer*                    pItemOwner      = NULL;
    KPlayer*                    pOtherPlayer    = NULL;
    S2C_SYNC_TRADING_ITEM_DATA* pItemData       = (S2C_SYNC_TRADING_ITEM_DATA*)pbyData;
    KUIEventTradingUpdateItem   UpdateItem;
    
    KG_ASSERT_EXIT(nSize >= sizeof(S2C_SYNC_TRADING_ITEM_DATA));

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    assert(pClientPlayer);
    KGLOG_PROCESS_ERROR(pClientPlayer->m_pTradingBox);

    pOtherPlayer = pClientPlayer->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOtherPlayer);
    KGLOG_PROCESS_ERROR(pOtherPlayer->m_pTradingBox);

    pItemOwner = g_pSO3World->m_PlayerSet.GetObj(pItemData->dwItemOwnerID);
    KGLOG_PROCESS_ERROR(pItemOwner);

    pClientPlayer->m_pTradingBox->SetKey(pItemData->nKey);
    pOtherPlayer->m_pTradingBox->SetKey(pItemData->nKey);
    
    if (pItemData->wSize - sizeof(S2C_SYNC_TRADING_ITEM_DATA) == 0)
    {   
        // 客户端用户向交易面板中加物品
        if ((int)pItemData->byBoxIndex != ibInvalid)    
        {
            assert(pItemOwner == pClientPlayer);
            pItem = pItemOwner->m_ItemList.GetItem((DWORD)pItemData->byBoxIndex, (DWORD)pItemData->byPosIndex);
            KGLOG_PROCESS_ERROR(pItem);
			
			pItemOwner->m_pTradingBox->DelItem((DWORD)pItemData->byGridIndex);
            pItemOwner->m_pTradingBox->AddItem((DWORD)pItemData->byGridIndex, pItem);
        }
        else  // 移除物品
        {
            pItemOwner->m_pTradingBox->DelItem((DWORD)pItemData->byGridIndex);
        }
    }

    // 交易的另一方, 在交易面板中加入物品
    if (pItemData->wSize - sizeof(S2C_SYNC_TRADING_ITEM_DATA) > 0)
    {
        assert(pItemOwner != pClientPlayer);

        pItemOwner->m_pTradingBox->DelItem((DWORD)pItemData->byGridIndex);
        
        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pItemData->szBuff, pItemData->wSize - sizeof(S2C_SYNC_TRADING_ITEM_DATA));
        KGLOG_PROCESS_ERROR(pItem);
        
        if (pItem->IsStackable())
        {
            pItem->SetStackNum(pItemData->byStackNum);
        }
        else
        {
            pItem->SetCurrentDurability(pItemData->byStackNum);
        }
        
        nRetCode = pItemOwner->m_pTradingBox->AddItem(pItemData->byGridIndex, pItem);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    // 通知UI
    if (g_pGameWorldUIHandler)
    {
        UpdateItem.dwCharacterID	= pItemData->dwItemOwnerID;
        UpdateItem.dwBoxIndex		= pItemData->byBoxIndex;
        UpdateItem.dwPosIndex		= pItemData->byPosIndex;
        UpdateItem.dwGridIndex		= (DWORD)pItemData->byGridIndex;
        g_pGameWorldUIHandler->OnTradingUpdateItem(UpdateItem);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncTradingMoney(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                    pOtherPlayer  = NULL;
    KPlayer*                    pMoneyOwner   = NULL;
    S2C_SYNC_TRADING_MONEY*     pSyncMoney    = (S2C_SYNC_TRADING_MONEY*)pbyData;
    KUIEventTradingUpdateMoney  UpdateMoney;
    
    KG_ASSERT_EXIT(pSyncMoney);
    
    pMoneyOwner = g_pSO3World->m_PlayerSet.GetObj(pSyncMoney->dwPlayerID);
    KGLOG_PROCESS_ERROR(pMoneyOwner);
    assert(pMoneyOwner->m_pTradingBox);
    pOtherPlayer = pMoneyOwner->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOtherPlayer);
    KGLOG_PROCESS_ERROR(pOtherPlayer->m_pTradingBox);

    pMoneyOwner->m_pTradingBox->SetKey(pSyncMoney->nKey);
    pOtherPlayer->m_pTradingBox->SetKey(pSyncMoney->nKey);
   
    pMoneyOwner->m_pTradingBox->SetMoney(pSyncMoney->nMoney);
   
    // 通知UI, 交易金钱修改
    if (g_pGameWorldUIHandler)
    {
        UpdateMoney.dwCharacterID = pSyncMoney->dwPlayerID;
        UpdateMoney.nMoney        = pSyncMoney->nMoney;
        g_pGameWorldUIHandler->OnTradingUpdateMoney(UpdateMoney);
    }

Exit0:
    return;
}

void KPlayerClient::OnTradingComplete(BYTE* pbyData, size_t nSize, int nFrame)
{
    int                     nResult          = false;
    int                     nRetCode         = false;
    KPlayer*                pClientPlayer    = NULL;
    KPlayer*                pOtherSide       = NULL;
    S2C_TRADING_COMPLETE*   pTradingComplete = (S2C_TRADING_COMPLETE*)pbyData;
    KUIEventTradingClose    TradingClose;

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnTradingClose(TradingClose);

        if (pTradingComplete->bComplete)
        {
            KUIEventTradingRespond UINofity;
            UINofity.nRespondCode = trcSuccess;
            g_pGameWorldUIHandler->OnTradingRespond(UINofity);
        }
        //else  交易失败, 失败原因会另行通知
    }

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);
    KGLOG_PROCESS_ERROR(pClientPlayer->m_pTradingBox);

    pOtherSide = pClientPlayer->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOtherSide);
    KGLOG_PROCESS_ERROR(pOtherSide->m_pTradingBox);
    
Exit0:
    if (pClientPlayer && pClientPlayer->m_pTradingBox)
    {
        pClientPlayer->m_pTradingBox->ClearTradingInfo();
    }

    if (pOtherSide && pOtherSide->m_pTradingBox)
    {
        pOtherSide->m_pTradingBox->ClearTradingInfo();
    }

    return;
}

// <--------------------- Trading ---------------------------------
void KPlayerClient::OnSyncMoney(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL            bRetCode    = false;
    int             nPrevMoney  = 0;
	S2C_SYNC_MONEY* pSyncMoney  = (S2C_SYNC_MONEY*)pbyData;
	KPlayer*        pPlayer     = NULL;
        
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    nPrevMoney = pPlayer->m_ItemList.GetMoney();

	bRetCode = pPlayer->m_ItemList.SetMoney(pSyncMoney->nMoney);
	KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_QuestList.UpdateNpcQuestMark(-1);

    if (g_pGameWorldUIHandler)
	{
        KUIEventMoneyUpdate param;

		param.nMoney        = pSyncMoney->nMoney;
        param.nPrevMoney    = nPrevMoney;
		param.bShowMsg      = pSyncMoney->bShowMsg;
		g_pGameWorldUIHandler->OnMoneyUpdate(param);
	}

Exit0:
	return;
}

void KPlayerClient::OnSyncContribution(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_CONTRIBUTION*  pSyncContribution   = (S2C_SYNC_CONTRIBUTION*)pbyData;
    KPlayer*                pPlayer             = NULL;
    int                     nPrecontribution    = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    nPrecontribution            =   pPlayer->m_nContribution;
    pPlayer->m_nContribution    =   pSyncContribution->nContribution;
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventContributionUpdate param;

        param.nPreContribution  = nPrecontribution;
        param.nContribution     = pPlayer->m_nContribution;
        g_pGameWorldUIHandler->OnContributionUpdate(param);
    }
Exit0:
    return;
}

// 同步道具数据
void KPlayerClient::OnSyncItemData(BYTE* pbyData, size_t nSize, int nFrame)
{
	int	                    nRetCode        = false;
    ADD_ITEM_RESULT_CODE    eRetCode        = aircFailed;  
	KItem*	                pItem           = NULL;
	S2C_SYNC_ITEM_DATA*     pSyncItemData   = (S2C_SYNC_ITEM_DATA*)pbyData;
    KPlayer*                pPlayer         = NULL;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncItemData->dwPlayerID); 
	KGLOG_PROCESS_ERROR(pPlayer);

	pItem = g_pSO3World->m_ItemSet.GetObj(pSyncItemData->dwItemID);
	if (pItem)
	{
		nRetCode = pPlayer->m_ItemList.DestroyItem(pItem);
		if (nRetCode != ircSuccess)
		{
            // 服务端同步客户端已经存在的物品很容易导致bug，通常要先以某种方式删掉客户端存在的物品，服务端再同步
            assert(false); 
			g_pSO3World->m_ItemManager.FreeItem(pItem);
        }
        pItem = NULL;
	}

    if (pSyncItemData->wSize - sizeof(S2C_SYNC_ITEM_DATA) > 0)
    {
        DWORD dwTab   = 0;
        DWORD dwIndex = 0;
        KInventory* pInventory = NULL;

        if (g_pSO3World->m_dwClientPlayerID == pSyncItemData->dwPlayerID)
        {
            pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData( 
                pSyncItemData->dwItemID, 
                pSyncItemData->byBuff, pSyncItemData->wSize - sizeof(S2C_SYNC_ITEM_DATA)
            );
            KGLOG_PROCESS_ERROR(pItem);

            dwTab   = pItem->m_GenParam.dwTabType;
            dwIndex = pItem->m_GenParam.dwIndex;

            eRetCode = pPlayer->m_ItemList.AddItemToPos(pItem, pSyncItemData->byBox, pSyncItemData->byX);
            KGLOG_PROCESS_ERROR(eRetCode == aircSuccess);
            pItem = NULL;
        }
        else
        {
            pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(
                ERROR_ID, pSyncItemData->byBuff, pSyncItemData->wSize - sizeof(S2C_SYNC_ITEM_DATA)
            );
            KGLOG_PROCESS_ERROR(pItem);

            dwTab   = pItem->m_GenParam.dwTabType;
            dwIndex = pItem->m_GenParam.dwIndex;

            pInventory = pPlayer->m_ItemList.GetInventory(pSyncItemData->byBox);
            KGLOG_PROCESS_ERROR(pInventory);

            nRetCode = pInventory->PlaceItem(pItem, pSyncItemData->byX);
            KGLOG_PROCESS_ERROR(nRetCode);
            pItem = NULL;
        }

        if (g_pGameWorldUIHandler)
        {
            KUIEventItemListUpdate param = { pSyncItemData->byBox, pSyncItemData->byX };
            g_pGameWorldUIHandler->OnItemListUpdate(param);
        }
        
        pPlayer->m_QuestList.OnUpdateItem(dwTab, dwIndex, true);
    }

Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
	return;
}

// 同步道具删除
void KPlayerClient::OnSyncItemDestroy(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode            = false;
	DWORD                   dwItemTab           = 0;
	DWORD                   dwItemIndex         = 0;
	KItem*                  pItem               = NULL;
	S2C_SYNC_ITEM_DESTROY*  pSyncItemDestroy    = (S2C_SYNC_ITEM_DESTROY*)pbyData;
	KPlayer*                pPlayer             = NULL;
    TItemPos	            Pos;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncItemDestroy->dwOwnerID); 
	KGLOG_PROCESS_ERROR(pPlayer);

	Pos.dwBox = pSyncItemDestroy->byBoX;
	Pos.dwX   = pSyncItemDestroy->byX;

	pItem = pPlayer->m_ItemList.GetItem(Pos.dwBox, Pos.dwX);
	KGLOG_PROCESS_ERROR(pItem);

	dwItemTab   = pItem->m_GenParam.dwTabType;
	dwItemIndex = pItem->m_GenParam.dwIndex;

	bRetCode = (pPlayer->m_ItemList.DestroyItem(Pos) == ircSuccess);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		KUIEventItemListUpdate param = { Pos.dwBox, Pos.dwX };
		g_pGameWorldUIHandler->OnItemListUpdate(param);
	}

	pPlayer->m_QuestList.OnUpdateItem(dwItemTab, dwItemIndex, false);
	pPlayer->m_QuestList.UpdateNpcQuestMark(-1);

Exit0:
	return;
}

// 交换装备
void KPlayerClient::OnExchangeItem(BYTE* pbyData, size_t nSize, int nFrame)
{
    ITEM_RESULT_CODE    eRetCode      = ircFailed;
	S2C_EXCHANGE_ITEM*  pExchangeItem = (S2C_EXCHANGE_ITEM*)pbyData;
	KPlayer*            pPlayer       = NULL;
    TItemPos            Src, Dest;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID); 
	KGLOG_PROCESS_ERROR(pPlayer);

	Src.dwBox   = pExchangeItem->byBox1;
	Src.dwX     = pExchangeItem->byX1;
	Dest.dwBox  = pExchangeItem->byBox2;
	Dest.dwX    = pExchangeItem->byX2;

    eRetCode = pPlayer->m_ItemList.ExchangeItem(Src, Dest, pExchangeItem->wAmount);
	KGLOG_PROCESS_ERROR(eRetCode == ircSuccess);

    if (g_pGameWorldUIHandler)
	{
		KUIEventItemListUpdate paramSrc = { Src.dwBox, Src.dwX };
		g_pGameWorldUIHandler->OnItemListUpdate(paramSrc);

		KUIEventItemListUpdate paramDst = { Dest.dwBox, Dest.dwX };
		g_pGameWorldUIHandler->OnItemListUpdate(paramDst);

		KUIEvnetItemExchanged paramExchanged = { Src.dwBox, Src.dwX, Dest.dwBox, Dest.dwX };
		g_pGameWorldUIHandler->OnItemExchanged(paramExchanged);
	}

Exit0:
	return;
}

// 换装广播
void KPlayerClient::OnSyncEquipRepresent(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bResult = false;
    S2C_SYNC_EQUIP_REPRESENT*   pPak    = (S2C_SYNC_EQUIP_REPRESENT*)pbyData;
    KPlayer*                    pPlayer = NULL;
    
    if (!(pPak->byIndex >= 0 && pPak->byIndex < perRepresentCount))
    {
        goto Exit0;
    }

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    pPlayer->m_wRepresentId[pPak->byIndex] = pPak->wRepresentID;

    if (g_pGameWorldRepresentHandler && pPlayer->m_pScene)
        g_pGameWorldRepresentHandler->OnCharacterDisplayData(pPlayer);

    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerDisplayDataUpdate Param = { pPlayer->m_dwID };
		g_pGameWorldUIHandler->OnPlayerDisplayDataUpdate(Param);

        KUIEventPlayerStateUpdate param = { pPak->dwPlayerID };
        g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }

Exit0:
    return;
}

// 同步道具获得的消息
void KPlayerClient::OnAddItemNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_ADD_ITEM_NOTIFY*	pAddItemNotify	= (S2C_ADD_ITEM_NOTIFY*)pbyData;
	KPlayer*				pPlayer			= NULL;
    KItem*                  pItem           = NULL;
    KItem*                  pTempItem       = NULL;
    DWORD                   dwTempItemID    = ERROR_ID;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pAddItemNotify->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    pItem = g_pSO3World->m_ItemSet.GetObj(pAddItemNotify->dwItemID);
    KGLOG_PROCESS_ERROR(pItem);

    pTempItem = g_pSO3World->m_ItemManager.CloneItem(pItem);
    KGLOG_PROCESS_ERROR(pTempItem);

    // If queue is full, then delete the first one.
    if (g_pSO3World->m_LinkItemQue.size() >= CLIENT_TALK_LINK_ITEM_BUFFER_SIZE)
    {
        KItem* pDeleteItem = g_pSO3World->m_LinkItemQue.front();
        if (pDeleteItem)
        {
            pDeleteItem->UnInit();
			g_pSO3World->m_ItemSet.Unregister(pDeleteItem);
			KMemory::Delete(pDeleteItem);
            pDeleteItem = NULL;
        }
        g_pSO3World->m_LinkItemQue.pop_front();
    }
    
    dwTempItemID = pTempItem->m_dwID;
    g_pSO3World->m_LinkItemQue.push_back(pTempItem);
    pTempItem = NULL;

    // 通知UI
    if (g_pGameWorldUIHandler)
    {
        KUIEventLootItem UIParam;
        UIParam.dwPlayerID  = pAddItemNotify->dwPlayerID;
        UIParam.dwItemID    = dwTempItemID;
        UIParam.dwCount     = pAddItemNotify->dwCount;
        g_pGameWorldUIHandler->OnLootItem(UIParam);
    }

Exit0:
    if (pTempItem)
    {
        pTempItem->UnInit();
		g_pSO3World->m_ItemSet.Unregister(pTempItem);
		KMemory::Delete(pTempItem);
        pTempItem = NULL;
    }
	return;
}

// 更新物品数量或耐久
void KPlayerClient::OnUpdateItemAmount(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KItem* pItem = NULL;

	S2C_UPDATE_ITEM_AMOUNT* pUpdateItemAmount = (S2C_UPDATE_ITEM_AMOUNT*)pbyData;

	KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID); 
	assert(pPlayer);

	pItem = pPlayer->m_ItemList.GetItem(pUpdateItemAmount->byBox, pUpdateItemAmount->byX);
	KG_PROCESS_ERROR(pItem);

    if (pItem->m_Common.bStack == false)
    {
		KGLOG_PROCESS_ERROR(pItem->m_Common.nGenre == igEquipment);
        pItem->SetCurrentDurability(pUpdateItemAmount->wAmount);
		if (pItem->m_bEquiped)
		{
			if (pItem->m_nCurrentDurability == 0 && pItem->m_bApplyed)
			{
				pPlayer->m_ItemList.UnApplyAttrib(pItem);
			}
			else if (!pItem->m_bApplyed && pItem->m_nCurrentDurability > 0)
			{
				pPlayer->m_ItemList.ApplyAttrib(pItem);
			}

            if (g_pGameWorldUIHandler)
            {
                KUIEventPlayerStateUpdate param = { g_pSO3World->m_dwClientPlayerID };
                g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
            }
		}

    }
	else
    {
        pItem->SetStackNum(pUpdateItemAmount->wAmount);
    }

    if (g_pGameWorldUIHandler)
	{
		KUIEventItemListUpdate param = { pUpdateItemAmount->byBox, pUpdateItemAmount->byX };
		g_pGameWorldUIHandler->OnItemListUpdate(param);
	}

    pItem = pPlayer->m_ItemList.GetItem(pUpdateItemAmount->byBox, pUpdateItemAmount->byX);
    pPlayer->m_QuestList.OnUpdateItem(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, true);

	return;
Exit0:

	return;
}

void KPlayerClient::OnLearnProfessionNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                            bRetCode                = false;
	KPlayer*                        pSelf                   = NULL;
	S2C_LEARN_PROFESSION_NOTIFY*    pLearnProfessionNotify  = (S2C_LEARN_PROFESSION_NOTIFY*)pbyData;
	
	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pSelf);

	bRetCode = pSelf->LearnProfession(pLearnProfessionNotify->dwProfessionID);
	KGLOG_PROCESS_ERROR(bRetCode);

    pSelf->m_QuestList.UpdateNpcQuestMark(-1);

    if (g_pGameWorldUIHandler)
	{
        KUIEventCraftUpdate UIEventCraftUpdate = { pLearnProfessionNotify->dwProfessionID };
		g_pGameWorldUIHandler->OnCraftUpdate(UIEventCraftUpdate);

        KUIEventLearnProfession UIEventLearnProfession = { pLearnProfessionNotify->dwProfessionID };
		g_pGameWorldUIHandler->OnLearnProfession(UIEventLearnProfession);
	}

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pSelf, REPRESENT_SFX_LEARN_PROFESSION);
    }

Exit0:
	return;
}

// 学习生活技能分支
void KPlayerClient::OnLearnBranchNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                     bRetCode            = false;
	KPlayer*                 pSelf               = NULL;
	S2C_LEARN_BRANCH_NOTIFY* pLearnBranchNotify  = (S2C_LEARN_BRANCH_NOTIFY*)pbyData;
	
	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pSelf);

	bRetCode = pSelf->LearnBranch(pLearnBranchNotify->dwProfessionID, pLearnBranchNotify->dwBranchID);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
        KUIEventCraftUpdate UIEventCraftUpdate = { pLearnBranchNotify->dwProfessionID };
		g_pGameWorldUIHandler->OnCraftUpdate(UIEventCraftUpdate);

        KUIEventLearnBranch UIEventLearnBranch;
		UIEventLearnBranch.dwProfessionID = pLearnBranchNotify->dwProfessionID;
		UIEventLearnBranch.dwBranchID = pLearnBranchNotify->dwBranchID;
		g_pGameWorldUIHandler->OnLearnBranch(UIEventLearnBranch);
	}

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pSelf, REPRESENT_SFX_LEARN_PROFESSION);
    }
	
Exit0:
	return;
}


// 遗忘技能
void KPlayerClient::OnForgetProfessionNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	
	S2C_FORGET_PROFESSION_NOTIFY* pForgetProfessionNotify = (S2C_FORGET_PROFESSION_NOTIFY*)pbyData;
	KPlayer *pSelf = NULL;

	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_ASSERT_EXIT(pSelf);

	bRetCode = pSelf->ForgetProfession(pForgetProfessionNotify->dwProfessionID);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
        KUIEventCraftRemove UIEventCraftRemove = { pForgetProfessionNotify->dwProfessionID };
		g_pGameWorldUIHandler->OnCraftRemove(UIEventCraftRemove);

        KUIEventForgetProfession UIEventForgetProfession = { pForgetProfessionNotify->dwProfessionID };
		g_pGameWorldUIHandler->OnForgetProfession(UIEventForgetProfession);
	}

    if (g_pGameWorldRepresentHandler)
    {
        KPlayer* pPlayer = NULL;

        pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pPlayer);

        g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pPlayer, REPRESENT_SFX_FORGET_PROFESSION);
    }

    pSelf->m_QuestList.UpdateNpcQuestMark(-1);

Exit0:
	return;
}

// 增加生活技能经验
void KPlayerClient::OnAddProfessionProficiency(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                                bRetCode                    = false;
	int                                 nOldLevel                   = 0;
	int                                 nNewLevel                   = 0;
	S2C_ADD_PROFESSION_PROFICIENCY*     pAddProfessionProficiency   = NULL;
	KPlayer*                            pSelf                       = NULL;

    pAddProfessionProficiency = (S2C_ADD_PROFESSION_PROFICIENCY*)pbyData;

	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pSelf);

	nOldLevel = (int)pSelf->m_ProfessionList.GetProfessionLevel(pAddProfessionProficiency->dwProfessionID);
	KGLOG_PROCESS_ERROR(nOldLevel > 0);

	bRetCode = pSelf->AddProfessionProficiency(
        pAddProfessionProficiency->dwProfessionID, pAddProfessionProficiency->dwExp
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		KUIEventCraftUpdate UIParam;
		UIParam.dwProfessionID = pAddProfessionProficiency->dwProfessionID;

		g_pGameWorldUIHandler->OnCraftUpdate(UIParam);

        if (pAddProfessionProficiency->dwExp)
        {
            KUIEventAddProfessionProficiency UIParam;
            UIParam.dwProfessionID = pAddProfessionProficiency->dwProfessionID;
            UIParam.dwProficiency = pAddProfessionProficiency->dwExp;

            g_pGameWorldUIHandler->OnAddProfessionProficiency(UIParam);
        }
    }

	nNewLevel = pSelf->m_ProfessionList.GetProfessionLevel(pAddProfessionProficiency->dwProfessionID);
	if (nNewLevel > nOldLevel)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventProfessionLevelUp UIParam;
            KPlayer* pPlayer = NULL;
            UIParam.dwProfessionID = pAddProfessionProficiency->dwProfessionID;
            UIParam.dwNewLevel = nNewLevel;

            g_pGameWorldUIHandler->OnProfessionLevelUp(UIParam);
        }

        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pSelf, REPRESENT_SFX_PROFESSION_LEVEL);
        }

        pSelf->m_QuestList.UpdateNpcQuestMark(-1);
	}

Exit0:
	return;
}

void KPlayerClient::OnSetMaxProfessionLevelNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                                    bRetCode                        = false;
	KPlayer*                                pSelf                           = NULL;
	S2C_SET_MAX_PROFESSION_LEVEL_NOTIFY*    pSetMaxProfessionLevelNotify    = NULL;
        
    pSetMaxProfessionLevelNotify = (S2C_SET_MAX_PROFESSION_LEVEL_NOTIFY*)pbyData;
	
	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pSelf);

	bRetCode = pSelf->SetMaxProfessionLevel(
        pSetMaxProfessionLevelNotify->dwProfessionID, pSetMaxProfessionLevelNotify->dwLevel
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
        KUIEventCraftUpdate UIEventCraftUpdate = { pSetMaxProfessionLevelNotify->dwProfessionID };
		g_pGameWorldUIHandler->OnCraftUpdate(UIEventCraftUpdate);

        KUIEventSetProfessionMaxLevel UIEventSetProfessionMaxLevel;
		UIEventSetProfessionMaxLevel.dwProfessionID = pSetMaxProfessionLevelNotify->dwProfessionID;
		UIEventSetProfessionMaxLevel.dwNewMaxLevel = pSetMaxProfessionLevelNotify->dwLevel;
		g_pGameWorldUIHandler->OnSetProfessionMaxLevel(UIEventSetProfessionMaxLevel);
	}

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pSelf, REPRESENT_SFX_PROFESSION_MAX_LEVEL);
    }

Exit0:
	return;
}

void KPlayerClient::OnSyncProfessionAdjustLevel(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                bRetCode                   = false;
    S2C_SYNC_PROFESSION_ADJUST_LEVEL*   pSyncProfessionAdjustLevel = (S2C_SYNC_PROFESSION_ADJUST_LEVEL*)pbyData;
    KPlayer*                            pSelf                      = NULL;

    pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pSelf);

    bRetCode = pSelf->m_ProfessionList.SetAdjustLevel(
        pSyncProfessionAdjustLevel->dwProfessionID, pSyncProfessionAdjustLevel->dwAdjustLevel
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        KUIEventSetProfessionAdjustLevel param;

        param.dwProfessionID = pSyncProfessionAdjustLevel->dwProfessionID;
        param.dwAdjustLevel  = pSyncProfessionAdjustLevel->dwAdjustLevel;

        g_pGameWorldUIHandler->OnSetProfessAdjustLevel(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnUpdatePlayerProfession(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                            bRetCode                    = false;
	S2C_UPDATE_PLAYER_PROFESSION*   pUpdatePlayerProfession     = (S2C_UPDATE_PLAYER_PROFESSION*)pbyData;
	KPlayer*                        pSelf                       = NULL;

	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pSelf);

	bRetCode = pSelf->m_ProfessionList.UpdatePlayerProfession(
        pUpdatePlayerProfession->dwProfessionID,    pUpdatePlayerProfession->dwMaxLevel,
        pUpdatePlayerProfession->dwCurrentLevel,    pUpdatePlayerProfession->dwCurrentProficiency,
        pUpdatePlayerProfession->byBranchID
    );
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

// 学习配方
void KPlayerClient::OnLearnRecipeNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	S2C_LEARN_RECIPE_NOTIFY* pLearnRecipeNotify = (S2C_LEARN_RECIPE_NOTIFY*)pbyData;
	KCraft* pCraft = NULL;
	KPlayer* pSelf = NULL;

	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_ASSERT_EXIT(pSelf);

	bRetCode = pSelf->m_RecipeList.LearnRecipe(pLearnRecipeNotify->dwCraftID, pLearnRecipeNotify->dwRecipeID);
	KGLOG_PROCESS_ERROR(bRetCode);

	pCraft = g_pSO3World->m_ProfessionManager.GetCraft(pLearnRecipeNotify->dwCraftID);
	KGLOG_PROCESS_ERROR(pCraft);

    if (g_pGameWorldUIHandler)
	{
		KUIEventRecipeUpdate UIEventRecipeUpdate;
		UIEventRecipeUpdate.dwProfession = pCraft->m_dwProfessionID;
		UIEventRecipeUpdate.dwCraftID = pLearnRecipeNotify->dwCraftID;
		UIEventRecipeUpdate.dwRecipeID = pLearnRecipeNotify->dwRecipeID;

		g_pGameWorldUIHandler->OnRecipeUpdate(UIEventRecipeUpdate);

        KUIEventLearnRecipe UIEventLearnRecipe;
		UIEventLearnRecipe.dwCraftID = pLearnRecipeNotify->dwCraftID;
		UIEventLearnRecipe.dwRecipeID = pLearnRecipeNotify->dwRecipeID;

		g_pGameWorldUIHandler->OnLearnRecipe(UIEventLearnRecipe);
	}

    if (g_pGameWorldRepresentHandler)
    {
        KPlayer* pPlayer = NULL;

        pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        KGLOG_PROCESS_ERROR(pPlayer);

        g_pGameWorldRepresentHandler->OnCharacterLevelUp((KCharacter*)pPlayer, REPRESENT_SFX_LEARN_RECIPE);
    }

Exit0:
	return;
}

// 遗忘配方
void KPlayerClient::OnForgetRecipeNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	S2C_FORGET_RECIPE_NOTIFY* pForgetRecipeNotify = (S2C_FORGET_RECIPE_NOTIFY*)pbyData;
	KPlayer* pSelf = NULL;

	pSelf = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_ASSERT_EXIT(pSelf);

	bRetCode = pSelf->m_RecipeList.ForgetRecipe(pForgetRecipeNotify->dwCraftID);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

// 同步配方列表
void KPlayerClient::OnSyncRecipeState(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode            = false;
	KPlayer*                pPlayer             = NULL;
	S2C_SYNC_RECIPE_STATE*  pSyncRecipeState    = NULL;
    BYTE*                   pbyOffset           = (BYTE*)pbyData;
    size_t                  uLeftSize           = nSize;
    size_t                  uUsedSize           = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(S2C_SYNC_RECIPE_STATE));
    pSyncRecipeState = (S2C_SYNC_RECIPE_STATE*)pbyOffset;
    uLeftSize -= sizeof(S2C_SYNC_RECIPE_STATE);
    pbyOffset += sizeof(S2C_SYNC_RECIPE_STATE);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_RecipeList.Load(pbyOffset, uLeftSize, pSyncRecipeState->byVersion);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

// 打开一个客户端窗口
void KPlayerClient::OnOpenWindow(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;

	S2C_OPEN_WINDOW* pOpenWindow = (S2C_OPEN_WINDOW*)pbyData;
	DWORD dwTargetType = pOpenWindow->dwTargetType;
	DWORD dwTargetID = pOpenWindow->dwTargetID;

	KGLOG_PROCESS_ERROR(pOpenWindow);

    if (g_pGameWorldUIHandler)
	{
		KUIEventOpenWindow param = { pOpenWindow->dwIndex, pOpenWindow->szText, dwTargetType, dwTargetID };
		g_pGameWorldUIHandler->OnOpenWindow(param);
	}

	return;
Exit0:

	return;
}

void KPlayerClient::OnStartUseRecipe(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode        = false;
	KPlayer*                pPlayer         = NULL;
    TARGET_TYPE             eTargetType     = ttInvalid;
	S2C_START_USE_RECIPE*   pStartUseRecipe = (S2C_START_USE_RECIPE*)pbyData;
	DWORD                   dwPlayerID      = pStartUseRecipe->dwPlayerID;
    KTarget                 Target;	

	KGLOG_PROCESS_ERROR(IS_PLAYER(dwPlayerID));

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

	eTargetType = (TARGET_TYPE)pStartUseRecipe->byTargetType;

	switch (eTargetType)
	{
	case ttNoTarget:
		break;
	case ttCoordination:
        bRetCode = Target.SetTarget(pStartUseRecipe->nParam1, pStartUseRecipe->nParam2, 0);
        KGLOG_PROCESS_ERROR(bRetCode);
		break;

	case ttNpc:
	case ttDoodad:
	case ttPlayer:
        bRetCode = Target.SetTarget(eTargetType, pStartUseRecipe->nParam1);
        KGLOG_PROCESS_ERROR(bRetCode);
		break;
	case ttItem:
		bRetCode = Target.SetTarget(eTargetType, pStartUseRecipe->nParam1);
		KGLOG_PROCESS_ERROR(bRetCode);
		break;
	default:
		KG_ASSERT_EXIT(!"Unexpected target type !");
		break;
	}

	bRetCode = pPlayer->CastProfessionSkill(pStartUseRecipe->dwCraftID, pStartUseRecipe->dwRecipeID, 0, Target);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerClient::OnAcceptQuest(BYTE* pbyData, size_t nSize, int nFrame)
{
	QUEST_RESULT_CODE eRetCode      = qrcFailed;
	S2C_ACCEPT_QUEST* pAcceptQuest  = (S2C_ACCEPT_QUEST*)pbyData;
	KPlayer*          pPlayer       = NULL;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	eRetCode = pPlayer->m_QuestList.Accept(pAcceptQuest->dwQuestID, pAcceptQuest->byDailyQuest);
	KGLOG_PROCESS_ERROR(eRetCode == qrcSuccess);
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventQuestAccepted QuestAccepted;
        QuestAccepted.nQuestIndex = pAcceptQuest->nQuestIndex;
        QuestAccepted.dwQuestID = pAcceptQuest->dwQuestID;
        g_pGameWorldUIHandler->OnQuestAccepted(QuestAccepted);
    }

	pPlayer->m_QuestList.UpdateNpcQuestMark(pAcceptQuest->dwQuestID);

Exit0:
	return;
}

// 完成任务
void KPlayerClient::OnFinishQuest(BYTE* pbyData, size_t nSize, int nFrame)
{
	QUEST_RESULT_CODE   nRetCode = qrcFailed;
	S2C_FINISH_QUEST*   pFinishQuest = (S2C_FINISH_QUEST*)pbyData;
    KQuestInfo*         pQuestInfo  = NULL;

	KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	nRetCode = pPlayer->m_QuestList.Finish(pFinishQuest->dwQuestID);
	KGLOG_PROCESS_ERROR(nRetCode == qrcSuccess);

    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(pFinishQuest->dwQuestID);
    KGLOG_PROCESS_ERROR(pQuestInfo);

    if (g_pGameWorldUIHandler)
    {
        KUIEventFinishQuest Param;
        Param.dwQuestID     = pFinishQuest->dwQuestID;
        Param.bForceFinish  = pFinishQuest->bForce;
        Param.bAssist       = false;
        Param.nAddStamina   = 0;
        Param.nAddThew      = 0;
        g_pGameWorldUIHandler->OnFinishQuest(Param);
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnUpdateDoodadQuestEffect();

        if (pQuestInfo->bAccept)    // 不需要接的任务，没有任务完成特效
            g_pGameWorldRepresentHandler->OnFinishQuest(pPlayer, pFinishQuest->dwQuestID, false);
    }

    pPlayer->m_QuestList.UpdateNpcQuestMark(-1);

Exit0:
	return;
}

void KPlayerClient::OnCancelQuest(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_CANCEL_QUEST*       pCancelQuest    = (S2C_CANCEL_QUEST*)pbyData;
	KPlayer*                pPlayer         = NULL;
    DWORD                   dwQuestID        = 0;
	KUIEventQuestCanceled   QuestCanceled;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	dwQuestID = pPlayer->m_QuestList.GetQuestID(pCancelQuest->nQuestIndex);

	pPlayer->m_QuestList.Cancel(pCancelQuest->nQuestIndex);

    if (g_pGameWorldUIHandler)
    {
        QuestCanceled.dwQuestID = dwQuestID;
        g_pGameWorldUIHandler->OnQuestCanceled(QuestCanceled);
    }

	pPlayer->m_QuestList.UpdateNpcQuestMark(dwQuestID);

Exit0:
	return;
}

void KPlayerClient::OnClearQuest(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_CLEAR_QUEST*    pClearQuest = (S2C_CLEAR_QUEST*)pbyData;
	KPlayer*            pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_QuestList.ClearQuest(pClearQuest->dwQuestID);

    pPlayer->m_QuestList.UpdateNpcQuestMark(-1);

Exit0:
    return;
}

void KPlayerClient::OnShareQuest(BYTE* pbyData, size_t nSize, int nFrame)
{
    QUEST_RESULT_CODE   eRetCode        = qrcInvalid;
    BOOL                bRetCode        = false;
	S2C_SHARE_QUEST*    pShareQuest     = (S2C_SHARE_QUEST*)pbyData;
	KPlayer*            pPlayer         = NULL;
    KQuestInfo*         pQuestInfo      = NULL;
    KTarget             Target;    
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(pShareQuest->dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

    bRetCode = Target.SetTarget(ttPlayer, pShareQuest->dwSrcPlayerID);
    KGLOG_PROCESS_ERROR(bRetCode);

	eRetCode = pPlayer->m_QuestList.CanAccept(pQuestInfo, &Target);
	if (eRetCode == qrcSuccess)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventQuestShared QuestShared;

            QuestShared.dwSrcPlayerID   = pShareQuest->dwSrcPlayerID;
            QuestShared.dwQuestID       = pShareQuest->dwQuestID;

            g_pGameWorldUIHandler->OnQuestShared(QuestShared);
        }
	}

Exit0:
	return;
}

// 同步任务变量
void KPlayerClient::OnSyncQuestValue(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode        = false;
	S2C_SYNC_QUEST_VALUE*   pSyncQuestValue = (S2C_SYNC_QUEST_VALUE*)pbyData;
	KPlayer*                pPlayer         = NULL;
    int                     nQuestID        = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT * 2; nIndex++)
	{
		bRetCode = pPlayer->m_QuestList.SetQuestValue(
            pSyncQuestValue->byQuestIndex, nIndex, pSyncQuestValue->nQuestValue[nIndex]
        );
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		bRetCode = pPlayer->m_QuestList.SetKillNpcCount(
            pSyncQuestValue->byQuestIndex, nIndex, pSyncQuestValue->dwKillNpcCount[nIndex]
        );
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = pPlayer->m_QuestList.SetQuestFailedFlag(pSyncQuestValue->byQuestIndex, pSyncQuestValue->bFailed);
	KGLOG_PROCESS_ERROR(bRetCode);

	if (pSyncQuestValue->bFailed)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventQuestFailed UIParam;
            UIParam.nQuestIndex = pSyncQuestValue->byQuestIndex;

            g_pGameWorldUIHandler->OnQuestFailed(UIParam);
        }
	}

	nQuestID = pPlayer->m_QuestList.GetQuestID(pSyncQuestValue->byQuestIndex);
	pPlayer->m_QuestList.UpdateNpcQuestMark(nQuestID);

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnUpdateDoodadQuestEffect();
    }

Exit0:
	return;
}

void KPlayerClient::OnStartEscortQuest(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_START_ESCORT_QUEST*     pStartEscortQuest   = (S2C_START_ESCORT_QUEST*)pbyData;
	KPlayer*                    pPlayer             = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);
	
    if (g_pGameWorldUIHandler)
	{
		KUIEventStartEscortQuest param;
		param.dwStartPlayerID   = pStartEscortQuest->dwStartPlayerID;
		param.dwQuestID         = pStartEscortQuest->dwQuestID;
		g_pGameWorldUIHandler->OnStartEscortQuest(param);
	}	

Exit0:
	return;
}

// 同步拾取列表
void KPlayerClient::OnSyncLootList(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
	S2C_SYNC_LOOT_LIST* pSyncLootList   = (S2C_SYNC_LOOT_LIST*)pbyData;
	KLootList*          pLootList       = NULL;
    KDoodad*            pDoodad         = NULL;
    BYTE*               pbyItemData     = NULL;
    KItem*              pItem           = NULL;

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pSyncLootList->dwDoodadID);
	KG_PROCESS_ERROR(pDoodad);

	if (pDoodad->m_pLootList)
	{
		pDoodad->m_pLootList->UnInit();
		KMemory::Delete(pDoodad->m_pLootList);
        pDoodad->m_pLootList = NULL;
	}

	pLootList = KMemory::New<KLootList>();
	KGLOG_PROCESS_ERROR(pLootList);

	bRetCode = pLootList->Init(0);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pLootList->SetMoney(pSyncLootList->nMoney);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    pLootList->m_nLooterCount = pSyncLootList->byLooterCount;
    assert(pLootList->m_nLooterCount <= MAX_PARTY_SIZE);

    memcpy(pLootList->m_LooterList, pSyncLootList->dwLooterIDList, sizeof(DWORD) * pLootList->m_nLooterCount);

	pbyItemData = pbyData + sizeof(S2C_SYNC_LOOT_LIST) + sizeof(DWORD) * pLootList->m_nLooterCount;
	while ((pbyItemData - pbyData) < pSyncLootList->wSize)
	{
        BYTE        bySize          = 0;
        DWORD       dwID            = ERROR_ID;
        KLootItem*  pLootItem       = NULL;
        LOOT_ITEM*  pBinaryItem     = NULL;
        
        pBinaryItem = (LOOT_ITEM*)pbyItemData;
        pbyItemData += sizeof(LOOT_ITEM);

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(
            pBinaryItem->dwItemID, pbyItemData, pBinaryItem->byItemDataSize
        );
        KGLOG_PROCESS_ERROR(pItem);
        pbyItemData += pBinaryItem->byItemDataSize;
        
        pLootItem = pLootList->AddItem(pItem);
		KGLOG_PROCESS_ERROR(pLootItem);
        pItem = NULL;

        pLootItem->eLootItemType    = (LOOT_ITEM_TYPE)pBinaryItem->byItemLootType;
		pLootItem->bDisplay         = (bool)pBinaryItem->byDisplay;
	}
    
    pDoodad->m_pLootList = pLootList;
    pLootList = NULL;

	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnDoodadLootList(pDoodad);
    }
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventSyncLootList SyncLootList;
        SyncLootList.dwDoodadID = pSyncLootList->dwDoodadID;
        g_pGameWorldUIHandler->OnSyncLootList(SyncLootList);
    }

    bResult = true;
Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }

    if (pLootList)
    {
        pLootList->UnInit();
	    KMemory::Delete(pLootList);
        pLootList = NULL;
    }
	return;
}

void KPlayerClient::OnRollItemMessage(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode            = false;
	S2C_ROLL_ITEM_MESSAGE*  pRollItemMessage    = (S2C_ROLL_ITEM_MESSAGE*)pbyData;
	KPlayer*                pPlayer             = NULL;
    KDoodad*                pDoodad             = NULL;
    KLootList*              pLootList           = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pRollItemMessage->dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);

	pLootList = pDoodad->m_pLootList;
	KGLOG_PROCESS_ERROR(pLootList);

	if (pRollItemMessage->nRollPoint >= 0)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventRollItem RollItem;
            RollItem.dwPlayerID = pRollItemMessage->dwPlayerID;
            RollItem.dwItemID   = pRollItemMessage->dwItemID;
            RollItem.nRandom    = pRollItemMessage->nRollPoint;
            g_pGameWorldUIHandler->OnRollItem(RollItem);
        }

        goto Exit0;
	}

    if (g_pGameWorldUIHandler)
    {
        KUIEventCancelRollItem CancelRollItem;
        CancelRollItem.dwPlayerID   = pRollItemMessage->dwPlayerID;
        CancelRollItem.dwItemID     = pRollItemMessage->dwItemID;
        g_pGameWorldUIHandler->OnCancelRollItem(CancelRollItem);

        if (pPlayer->m_OTActionParam.eActionType == otActionPicking && pPlayer->m_OTActionParam.Data.dwDoodadID == pDoodad->m_dwID)
        {
            KUIEventSyncLootList SyncLootList;
            SyncLootList.dwDoodadID = pRollItemMessage->dwDoodadID;
            g_pGameWorldUIHandler->OnSyncLootList(SyncLootList);
        }
    }

Exit0:
	return;
}

void KPlayerClient::OnOpenLootList(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = false;
	KPlayer*            pPlayer         = NULL;
	S2C_OPEN_LOOT_LIST* pOpenLootList   = (S2C_OPEN_LOOT_LIST*)pbyData;
	DWORD               dwPlayerID      = pOpenLootList->dwPlayerID;
	DWORD				dwDoodadID		= pOpenLootList->dwDoodadID;
    KDoodad*            pDoodad         = NULL;

    bRetCode = IS_PLAYER(dwPlayerID);
	KGLOG_PROCESS_ERROR(bRetCode);

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pDoodad->OpenLootList((KCharacter*)pPlayer);
	
Exit0:
	return;
}

void KPlayerClient::OnSyncFightFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                     bRetCode        = false;
	KPlayer*                 pPlayer         = NULL;
	S2C_SYNC_FIGHT_FLAG*     pSyncFightFlag  = (S2C_SYNC_FIGHT_FLAG*)pbyData;
    KUIEventUpdateReputation UIParam;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_ReputeList.SetFightFlag(pSyncFightFlag->byForceID, pSyncFightFlag->byOpen, true);
	KGLOG_PROCESS_ERROR(bRetCode);

    UIParam.dwForceID = pSyncFightFlag->byForceID;
    UIParam.bNewForce = false;

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnUpdateReputation(UIParam);

Exit0:
	return;
}

// 同步声望
void KPlayerClient::OnSyncRepute(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                bRetCode    = false;
	int                 nOldLevel   = 0;
	KPlayer*            pPlayer     = NULL;
	S2C_SYNC_REPUTE*    pSyncRepute = (S2C_SYNC_REPUTE*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	nOldLevel = pPlayer->m_ReputeList.GetReputeLevel(pSyncRepute->byForceID);

	bRetCode = pPlayer->m_ReputeList.SetReputation(pSyncRepute->byForceID, pSyncRepute->byLevel, pSyncRepute->nRepute);
	KGLOG_PROCESS_ERROR(bRetCode);

	KUIEventUpdateReputation UIParam;

	UIParam.dwForceID = pSyncRepute->byForceID;
	UIParam.bNewForce = false;

    if (g_pGameWorldUIHandler)
	    g_pGameWorldUIHandler->OnUpdateReputation(UIParam);

	if (pSyncRepute->nDiffValue != 0)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventReputationRespond UIParam;

            UIParam.dwForceID   = pSyncRepute->byForceID;
            UIParam.nNumber     = pSyncRepute->nDiffValue;

            g_pGameWorldUIHandler->OnReputationRespond(UIParam);
        }
	}

	if (nOldLevel != pSyncRepute->byLevel)
	{
        BOOL bLevelUp = pSyncRepute->byLevel > nOldLevel;

        if (g_pGameWorldUIHandler)
        {
		    g_pGameWorldUIHandler->OnUpdateAllRelation();
            g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();
            g_pGameWorldRepresentHandler->OnReputationLevelUp(pPlayer, bLevelUp);
        }

        if (pPlayer->m_ReputeList.IsShow(pSyncRepute->byForceID))
        {
            if (g_pGameWorldUIHandler)
            {
                KUIEventReputationLevelUp Param = { pSyncRepute->byForceID, bLevelUp };
                g_pGameWorldUIHandler->OnReputationLevelUp(Param);
            }
        }

        pPlayer->m_QuestList.UpdateNpcQuestMark(-1);
	}

Exit0:
	return;
}


// 同步所有势力战斗开关
void KPlayerClient::OnSyncFightflagList(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                        bRetCode            = false;
	KPlayer*                    pPlayer             = NULL;
	S2C_SYNC_FIGHTFLAG_LIST*    pSyncFightflagList  = (S2C_SYNC_FIGHTFLAG_LIST*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	for (int i = 0; i < pSyncFightflagList->byFightFlagCount; i++)
	{
		pPlayer->m_ReputeList.SetFightFlag(i, pSyncFightflagList->byFightFlag[i]);
	}

Exit0:
	return;
}


// 同步所有的势力声望
void KPlayerClient::OnSyncReputeList(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode        = false;
	KPlayer*                pPlayer         = NULL;
	S2C_SYNC_REPUTE_LIST*   pSyncReputeList = (S2C_SYNC_REPUTE_LIST*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	for (int i = 0; i < pSyncReputeList->byForceCount; i++)
	{
		pPlayer->m_ReputeList.SetReputation(i, pSyncReputeList->Repute[i].byLevel, pSyncReputeList->Repute[i].shRepute);

        if (g_pGameWorldUIHandler)
        {
            KUIEventUpdateReputation UIParam;

            UIParam.dwForceID = i;
            UIParam.bNewForce = false;

            g_pGameWorldUIHandler->OnUpdateReputation(UIParam);
        }
	}

Exit0:
	return;
}

// 同步声望修改标记
void KPlayerClient::OnShowReputation(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode            = false;
	KPlayer*                pPlayer             = NULL;
    BOOL                    bShow               = false;
	S2C_SHOW_REPUTATION*    pSyncModifiedFlag   = (S2C_SHOW_REPUTATION*)pbyData;
    
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    bShow = pPlayer->m_ReputeList.IsShow(pSyncModifiedFlag->dwForceID);
    
    if (!bShow)
    {
        bRetCode = pPlayer->m_ReputeList.Show(pSyncModifiedFlag->dwForceID);
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
	
    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateReputation UIParam;
        UIParam.dwForceID = pSyncModifiedFlag->dwForceID;
        UIParam.bNewForce = !bShow;
        g_pGameWorldUIHandler->OnUpdateReputation(UIParam);
        
        if (!pSyncModifiedFlag->bLoginLoading)
        {
            KUIEventReputationLevelUp UIParamLevelUp;
            UIParamLevelUp.dwForceID = pSyncModifiedFlag->dwForceID;
            UIParamLevelUp.bLevelUp = true;

            g_pGameWorldUIHandler->OnReputationLevelUp(UIParamLevelUp);
        }
    }

Exit0:
	return;
}

void KPlayerClient::OnCharacterOpen(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = false;
    KPlayer*            pPlayer         = NULL;
	KDoodad*            pDoodad         = NULL;
	S2C_CHARACTER_OPEN* pCharacterOpen  = (S2C_CHARACTER_OPEN*)pbyData;
	DWORD               dwCharacterID   = pCharacterOpen->dwCharacterID;
	DWORD               dwDoodadID      = pCharacterOpen->dwDoodadID;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);
	KG_PROCESS_ERROR(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
	KG_PROCESS_ERROR(pDoodad);

	pPlayer->OpenDoodad(pDoodad);

Exit0:
	return;
}

void KPlayerClient::OnDistributeItemMessage(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_DISTRIBUTE_ITEM_MESSAGE* pDistributeItemMessage = (S2C_DISTRIBUTE_ITEM_MESSAGE*)pbyData;
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventDistributeItem KUIEvent;

        KUIEvent.dwPlayerID = pDistributeItemMessage->dwDestPlayerID;
        KUIEvent.dwItemID   = pDistributeItemMessage->dwItemID;

        g_pGameWorldUIHandler->OnDistributeItem(KUIEvent);
    }

Exit0:
    return;
}

void KPlayerClient::OnFinishLoot(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_FINISH_LOOT*    pFinishLoot = (S2C_FINISH_LOOT*)pbyData;
	KPlayer*            pPlayer     = NULL;
    KDoodad*            pDoodad     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pFinishLoot->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);
    
    pPlayer->DoActionIdle();

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pFinishLoot->dwDoodadID);
	KG_PROCESS_ERROR(pDoodad);

	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterEndPickDoodad((KCharacter*)pPlayer, pDoodad);
    }

Exit0:
	return;
}

void KPlayerClient::OnBeginRollItem(BYTE* pbyData, size_t nSize, int nFrame)
{
    KDoodad*             pDoodad = NULL;
    KItem*               pItem   = NULL;
	S2C_BEGIN_ROLL_ITEM* pBeginRollItem = (S2C_BEGIN_ROLL_ITEM*)pbyData;
    
    pDoodad = g_pSO3World->m_DoodadSet.GetObj(pBeginRollItem->dwDoodadID);
    KGLOG_PROCESS_ERROR(pDoodad);

    pItem = g_pSO3World->m_ItemSet.GetObj(pBeginRollItem->dwItemID);
    KGLOG_PROCESS_ERROR(pItem);

    if (g_pGameWorldUIHandler)
    {
        KUIEventBeginRollItem param = { pBeginRollItem->dwDoodadID, pBeginRollItem->dwItemID, pBeginRollItem->wLeftRollFrame};
        g_pGameWorldUIHandler->OnBeginRollItem(param);
    }

Exit0:
	return;
}

// 同步定制的用户消息
void KPlayerClient::OnCustomOTAction(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	static char szActionName[CUSTOM_OTACTION_NAME_LEN];
	KTarget Target;
	S2C_CUSTOM_OTACTION* pCustomOtaction = (S2C_CUSTOM_OTACTION*)pbyData;

	KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(pCustomOtaction->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

	KGLOG_PROCESS_ERROR(pCustomOtaction->szActionName);

	int nLen = (int)strlen(pCustomOtaction->szActionName);
	KGLOG_PROCESS_ERROR(nLen > 0 && nLen < CUSTOM_OTACTION_NAME_LEN);

	switch(pCustomOtaction->nTargetType)
	{
	case ttInvalid:
	case ttNoTarget:
		Target.ClearTarget();
		break;
	case ttNpc:
	case ttPlayer:
	case ttDoodad:
        bRetCode = Target.SetTarget((TARGET_TYPE)pCustomOtaction->nTargetType, pCustomOtaction->nTargetID);
        KG_PROCESS_ERROR(bRetCode);
		break;
	default:
		KGLOG_PROCESS_ERROR(false);
		break;
	}

	if (pCustomOtaction->nType == 0) // 非通道技能
	{
		pPlayer->DoCustomOTActionPrepare(pCustomOtaction->bIsBreakable, pCustomOtaction->nFrame, Target, pCustomOtaction->dwScriptID);
	}
	else // 通道技
	{
		pPlayer->DoCustomOTActionChannel(pCustomOtaction->bIsBreakable, pCustomOtaction->nFrame, Target, pCustomOtaction->dwScriptID);
	}

	// UI
	if (pPlayer->m_dwID == g_pSO3World->m_dwClientPlayerID)
	{
        if (g_pGameWorldUIHandler)
        {
            strncpy(szActionName, pCustomOtaction->szActionName, CUSTOM_OTACTION_NAME_LEN);
            szActionName[CUSTOM_OTACTION_NAME_LEN - 1] = 0;

            KUIEventCustomOTActionProgress param;
            param.nTotalFrame = pCustomOtaction->nFrame;
            strncpy(param.szActionName, pCustomOtaction->szActionName, CUSTOM_OTACTION_NAME_LEN);
            param.szActionName[CUSTOM_OTACTION_NAME_LEN - 1] = 0;
            param.nType = pCustomOtaction->nType;

            g_pGameWorldUIHandler->OnCustomOTActionProgress(param);
        }
	}

Exit0:
	return;
}



// 添加仇恨
void KPlayerClient::OnModifyThreat(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_MODIFY_THREAT* pModifyThreat = (S2C_MODIFY_THREAT*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_ThreatList.ModifyThreat(pModifyThreat->dwCharacterID);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateRelation param = { pModifyThreat->dwCharacterID };
		g_pGameWorldUIHandler->OnUpdateRelation(param);
	}

    if (g_pGameWorldRepresentHandler)
    {
        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(pModifyThreat->dwCharacterID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pModifyThreat->dwCharacterID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pModifyThreat->dwCharacterID);

        KG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }

	return;
Exit0:

	return;
}


// 修改被仇恨
void KPlayerClient::OnModifyThreaten(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_MODIFY_THREATEN* pModifyThreaten = (S2C_MODIFY_THREATEN*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_ThreatList.ModifyThreaten(pModifyThreaten->dwCharacterID);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateRelation param = { pModifyThreaten->dwCharacterID };
		g_pGameWorldUIHandler->OnUpdateRelation(param);
	}

    if (g_pGameWorldRepresentHandler)
    {
        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(pModifyThreaten->dwCharacterID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pModifyThreaten->dwCharacterID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pModifyThreaten->dwCharacterID);

        KG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }

	return;
Exit0:

	return;
}


// 清除仇恨
void KPlayerClient::OnClearThreat(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_CLEAR_THREAT* pClearThreat = (S2C_CLEAR_THREAT*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_ThreatList.ClearThreat(pClearThreat->dwCharacterID);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateRelation param = { pClearThreat->dwCharacterID };
		g_pGameWorldUIHandler->OnUpdateRelation(param);
	}

    if (g_pGameWorldRepresentHandler)
    {
        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(pClearThreat->dwCharacterID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pClearThreat->dwCharacterID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pClearThreat->dwCharacterID);

        KG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }

	return;
Exit0:

	return;
}

// 清除被仇恨
void KPlayerClient::OnClearThreaten(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_CLEAR_THREATEN* pClearThreaten = (S2C_CLEAR_THREATEN*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_ThreatList.ClearThreaten(pClearThreaten->dwCharacterID);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateRelation param = { pClearThreaten->dwCharacterID };
		g_pGameWorldUIHandler->OnUpdateRelation(param);
	}

    if (g_pGameWorldRepresentHandler)
    {
        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(pClearThreaten->dwCharacterID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pClearThreaten->dwCharacterID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pClearThreaten->dwCharacterID);

        KG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }

    return;
Exit0:

	return;
}

void KPlayerClient::OnTalkMessage(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_TALK_MESSAGE*           pTalkMsg        = (S2C_TALK_MESSAGE*)pbyData;
    KPlayer*                    pPlayer         = NULL;
    KUIEventPlayerTalk          UIPlayerTalk;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    UIPlayerTalk.dwTalkerID     = pTalkMsg->dwTalkerID;
    UIPlayerTalk.dwTalkChannel  = pTalkMsg->byMsgType;
    UIPlayerTalk.bEcho          = false;
    UIPlayerTalk.szName[0]      = '\0';

    switch(pTalkMsg->byMsgType)
    {
    case trNearby:
    case trTeam:
    case trRaid:
    case trBattleField:
    case trFace:
    case trNpcNearby:
    case trNpcTeam:
    case trNpcSence:
    case trNpcFace:
    case trNpcSayTo:
    case trNpcYellTo:
    case trNpcSayToCamp:
    case trTong:
    case trForce:
    case trCamp:
    case trSence:
    case trWorld:
    case trMentor:
        if (g_pGameWorldUIHandler)
        {
            size_t uCopyNameLen = min(sizeof(UIPlayerTalk.szName) - 1, pTalkMsg->byTalkData[0]);
            size_t uCopyDataLen = pTalkMsg->wSize - sizeof(S2C_TALK_MESSAGE) - 1 - pTalkMsg->byTalkData[0];

            memcpy(UIPlayerTalk.szName, pTalkMsg->byTalkData + 1, uCopyNameLen);
            UIPlayerTalk.szName[uCopyNameLen] = '\0';

            pPlayer->CopyTalkData(pTalkMsg->dwTalkerID, uCopyDataLen, pTalkMsg->byTalkData + 1 + pTalkMsg->byTalkData[0]);

            if (pTalkMsg->byMsgType == trNpcSayTo)
                UIPlayerTalk.dwTalkChannel = trNpcNearby;

            if (pTalkMsg->byMsgType == trNpcYellTo)
                UIPlayerTalk.dwTalkChannel = trNpcSence;

            g_pGameWorldUIHandler->OnPlayerTalk(UIPlayerTalk);
        }
        break;

    case trNpcWhisper:
    case trWhisper:
    case trFriends:
        if (g_pGameWorldUIHandler)
        {
            size_t uCopyNameLen = min(sizeof(UIPlayerTalk.szName) - 1, pTalkMsg->byTalkData[1]);
            size_t uCopyDataLen = pTalkMsg->wSize - sizeof(S2C_TALK_MESSAGE) - 2 - pTalkMsg->byTalkData[1];

            UIPlayerTalk.bEcho = pTalkMsg->byTalkData[0];
            memcpy(UIPlayerTalk.szName, pTalkMsg->byTalkData + 2, uCopyNameLen);
            UIPlayerTalk.szName[uCopyNameLen] = '\0';

            pPlayer->CopyTalkData(pTalkMsg->dwTalkerID, uCopyDataLen, pTalkMsg->byTalkData + 2 + pTalkMsg->byTalkData[1]);

            g_pGameWorldUIHandler->OnPlayerTalk(UIPlayerTalk);
        }
        break;

    case trNpcSayToTargetUseSentenceID:
        if (g_pGameWorldUIHandler)
        {
            KUIEventNpcTalkUseSentenceID Event;
            size_t uCopyNameLen = min(sizeof(UIPlayerTalk.szName) - 1, pTalkMsg->byTalkData[1]);

            Event.dwTalkerID        = pTalkMsg->dwTalkerID;
            Event.dwTalkChannel     = pTalkMsg->byMsgType;

            memcpy(Event.szTalkerName, pTalkMsg->byTalkData + 2, uCopyNameLen);
            Event.szTalkerName[uCopyNameLen] = '\0';

            Event.dwSentenceID = *(DWORD*)(pTalkMsg->byTalkData + 2 + pTalkMsg->byTalkData[1]);

            g_pGameWorldUIHandler->OnNpcTalkUseSentenceID(Event);
        }
        break;

    case trLocalSys:
    case trGlobalSys:
    case trGmMessage:
    case trDebugThreat:
        assert(g_pGameWorldUIHandler);

        UIPlayerTalk.szName[0] = '\0';

        pPlayer->CopyTalkData(pTalkMsg->dwTalkerID, pTalkMsg->wSize - sizeof(S2C_TALK_MESSAGE), pTalkMsg->byTalkData);

        g_pGameWorldUIHandler->OnPlayerTalk(UIPlayerTalk);
        break;

    default:
        KGLOG_PROCESS_ERROR(!"Undefined talk type!");
    }

Exit0:
    return;
}
 
void KPlayerClient::OnSyncSelfCurrentST(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode    = false;
	KPlayer* pPlayer = NULL;

	S2C_SYNC_SELF_CURRENT_ST* pUpdatePlayerST = (S2C_SYNC_SELF_CURRENT_ST*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pPlayer->m_nCurrentStamina = pUpdatePlayerST->nCurrentStamina;
    pPlayer->m_nCurrentThew    = pUpdatePlayerST->nCurrentThew;

	MAKE_IN_RANGE(pPlayer->m_nCurrentStamina, 0, pPlayer->m_nMaxStamina);
    MAKE_IN_RANGE(pPlayer->m_nCurrentThew, 0, pPlayer->m_nMaxThew);

	//UI 临时用更新经验的事件
    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerExperienceUpdate param = { g_pSO3World->m_dwClientPlayerID };
        g_pGameWorldUIHandler->OnPlayerExperienceUpdate(param);
    }

	return;
Exit0:

	return;
}

void KPlayerClient::OnDoodadLootOver(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode        = false;
	KDoodad*                pDoodad         = NULL;
	S2C_DOODAD_LOOT_OVER*   pDoodadLootOver = (S2C_DOODAD_LOOT_OVER*)pbyData;
	DWORD                   dwDoodadID      = pDoodadLootOver->dwDoodadID;

	pDoodad= g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
	KG_PROCESS_ERROR(pDoodad);

	pDoodad->m_bLooted = true;
	if (pDoodad->m_pLootList)
	{
		pDoodad->m_pLootList->UnInit();
		KMemory::Delete(pDoodad->m_pLootList);
		pDoodad->m_pLootList = NULL;
	}

	//通知表现逻辑更新拾取特效
	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnDoodadLootList(pDoodad);
    }

	return;
Exit0:

	return;
}
void KPlayerClient::OnSyncFellowshipList(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode            = false;
    S2C_SYNC_FELLOWSHIP_LIST*   pSyncFellowshipList = (S2C_SYNC_FELLOWSHIP_LIST*)pbyData;
    KG_FELLOWSHIP_ALL_GS*       pFellowshipList     = NULL;
    KG_FOE_ALL_GS*              pFoeList            = NULL;
    KG_BLACK_LIST_ALL_GS*       pBlackList          = NULL;
    size_t                      uSize               = 0;
                
    KGLOG_PROCESS_ERROR(nSize >= sizeof(S2C_SYNC_FELLOWSHIP_LIST));

    switch (pSyncFellowshipList->byType)
    {
    case 1:
        uSize = sizeof(S2C_SYNC_FELLOWSHIP_LIST) + pSyncFellowshipList->nFellowshipCount * sizeof(KG_FELLOWSHIP_ALL_GS);
        KGLOG_PROCESS_ERROR(nSize == uSize);

        pFellowshipList = (KG_FELLOWSHIP_ALL_GS*)pSyncFellowshipList->byFellowshipList;

        for (int i = 0; i < pSyncFellowshipList->nFellowshipCount; ++i)
        {
            KGFellowship* pFellowship = NULL;

            pFellowship = g_pSO3World->m_FellowshipMgr.AddFellowship(
                g_pSO3World->m_dwClientPlayerID, 
                pFellowshipList[i].dwAlliedPlayerID,
                pFellowshipList[i].szName,
                false
            );

            KG_PROCESS_ERROR(pFellowship);

            pFellowship->m_nAttraction  = pFellowshipList[i].nAttraction;
            pFellowship->m_nPlayerLevel = pFellowshipList[i].nLevel;
            pFellowship->m_dwMapID      = pFellowshipList[i].dwMapID;
            pFellowship->m_dwForceID    = pFellowshipList[i].dwForceID;

            assert(sizeof(pFellowship->m_BitFlag) == sizeof(pFellowshipList[i].byFlags));
            memcpy(&pFellowship->m_BitFlag, pFellowshipList[i].byFlags, sizeof(pFellowship->m_BitFlag));

            strncpy(pFellowship->m_szName, pFellowshipList[i].szName, sizeof(pFellowship->m_szName));
            pFellowship->m_szName[sizeof(pFellowship->m_szName) - sizeof('\0')] = '\0';

            strncpy(pFellowship->m_szRemark, pFellowshipList[i].szRemark, sizeof(pFellowship->m_szRemark));
            pFellowship->m_szRemark[sizeof(pFellowship->m_szRemark) - sizeof('\0')] = '\0';
        }
        break;

    case 2:
        uSize = sizeof(S2C_SYNC_FELLOWSHIP_LIST) + pSyncFellowshipList->nFellowshipCount * sizeof(KG_FOE_ALL_GS);
        KGLOG_PROCESS_ERROR(nSize == uSize);

        pFoeList = (KG_FOE_ALL_GS*)pSyncFellowshipList->byFellowshipList;

        for (int i = 0; i < pSyncFellowshipList->nFellowshipCount; ++i)
        {
            KGFoe* pFoe = NULL;

            pFoe = g_pSO3World->m_FellowshipMgr.AddFoe(
                g_pSO3World->m_dwClientPlayerID, 
                pFoeList[i].dwAlliedPlayerID,
                pFoeList[i].szName,
                false
            );

            KG_PROCESS_ERROR(pFoe);

            pFoe->m_nPlayerLevel = pFoeList[i].nLevel;
            pFoe->m_dwForceID = pFoeList[i].dwForceID;

            strncpy(pFoe->m_szName, pFoeList[i].szName, sizeof(pFoe->m_szName));
            pFoe->m_szName[sizeof(pFoe->m_szName) - sizeof('\0')] = '\0';
            
            if (g_pGameWorldUIHandler)
            {
                KUIEventUpdateRelation UIEventUpdateRelation;
                UIEventUpdateRelation.dwCharacterID = pFoeList[i].dwAlliedPlayerID;
                g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);
            }
            
            if (g_pGameWorldRepresentHandler)
            {
                KPlayer* pDestPlayer = g_pSO3World->m_PlayerSet.GetObj(pFoeList[i].dwAlliedPlayerID);
                if (pDestPlayer)
                {
                    g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pDestPlayer);  
                }
            }
        }
        break;

    case 3:
        uSize = sizeof(S2C_SYNC_FELLOWSHIP_LIST) + pSyncFellowshipList->nFellowshipCount * sizeof(KG_BLACK_LIST_ALL_GS);
        KGLOG_PROCESS_ERROR(nSize == uSize);

        pBlackList = (KG_BLACK_LIST_ALL_GS*)pSyncFellowshipList->byFellowshipList;

        for (int i = 0; i < pSyncFellowshipList->nFellowshipCount; ++i)
        {
            KGBlackNode* pBlackNode = NULL;

            pBlackNode = g_pSO3World->m_FellowshipMgr.AddBlackList(
                g_pSO3World->m_dwClientPlayerID, 
                pBlackList[i].dwAlliedPlayerID,
                pBlackList[i].szName,
                false
            );

            KG_PROCESS_ERROR(pBlackNode);

            strncpy(pBlackNode->m_szName, pBlackList[i].szName, sizeof(pBlackNode->m_szName));
            pBlackNode->m_szName[sizeof(pBlackNode->m_szName) - sizeof('\0')] = '\0';
        }
        break;
    }
    
    if (pSyncFellowshipList->nFellowshipCount == 0)   // Sync finished. 
    {
        g_pSO3World->m_FellowshipMgr.m_bWaitInitSync = false;

        if (g_pGameWorldUIHandler)
        {
            switch (pSyncFellowshipList->byType)
            {
            case 1:
                g_pGameWorldUIHandler->OnFellowshipUpdate();
                break;

            case 2:
                g_pGameWorldUIHandler->OnFoeUpdate();
                break;

            case 3:
                g_pGameWorldUIHandler->OnBlackListUpdate();
                break;
            }
        }
    }
Exit0:
    return;
}

void KPlayerClient::OnSyncFellowshipGroupName(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode        = false;
    S2C_SYNC_FELLOWSHIP_GROUP_NAME* pSyncGroupName  = (S2C_SYNC_FELLOWSHIP_GROUP_NAME*)pbyData;
    DWORD                           dwPlayerID      = g_pSO3World->m_dwClientPlayerID;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator it;

    g_pSO3World->m_FellowshipMgr.m_GroupNamesMap[dwPlayerID].nGroupCount = 0;
    it = g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.find(dwPlayerID);
    for (int i = 0; i < KG_FELLOWSHIP_MAX_CUSTEM_GROUP; i++)
    {
        if (pSyncGroupName->szGroupName[i][0] == '\0')
            break;

        strncpy(it->second.szGroupName[i], pSyncGroupName->szGroupName[i], sizeof(it->second.szGroupName[i]));
        it->second.szGroupName[i][sizeof(it->second.szGroupName[i]) - sizeof('\0')] = '\0';

        it->second.nGroupCount++;
    }

    for (int i = it->second.nGroupCount; i < KG_FELLOWSHIP_MAX_CUSTEM_GROUP; i++)
    {
        it->second.szGroupName[i][0] = '\0';
    }

Exit0:
    return;
}

// 
void KPlayerClient::OnNpcTalkToPlayer(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KNpc* pNpc = NULL;
	const char* szText = NULL;
    char szDialogKey[MAX_DIALOGE_KEY_SIZE];

	S2C_NPC_TALK_TO_PLAYER* pNpcTalkToPlayer = (S2C_NPC_TALK_TO_PLAYER*)pbyData;

	pNpc = g_pSO3World->m_NpcSet.GetObj(pNpcTalkToPlayer->dwNpcID);
	KG_PROCESS_ERROR(pNpc);

	bRetCode = g_pSO3World->m_Settings.m_SmartDialogList.GetDialogKey(
        pNpcTalkToPlayer->eTalkActionType, 
        pNpcTalkToPlayer->dwTalkIndex,
        szDialogKey,
        sizeof(szDialogKey)
    );
	KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(szDialogKey[0]);
	
    if (g_pGameWorldUIHandler)
	{
		//通知UI
		KUIEventCharacterSay CharacterSay;
        CharacterSay.dwDialogID = pNpcTalkToPlayer->dwDialogID;
		CharacterSay.dwCharacterID = pNpc->m_dwID;
		CharacterSay.dwTalkChannel = (DWORD)pNpcTalkToPlayer->eTalkType;
		CharacterSay.pszDialogKey = const_cast<char*>(szDialogKey);

		KUIEventNpcTalk NpcTalk;
        NpcTalk.dwDialogID = pNpcTalkToPlayer->dwDialogID;
        NpcTalk.dwNpcID = pNpc->m_dwID;
		NpcTalk.dwTalkChannel = (DWORD)pNpcTalkToPlayer->eTalkType;
		NpcTalk.pszDialogKey = const_cast<char*>(szDialogKey);

		//TODO:
		//设置Target参数;
		g_pGameWorldUIHandler->OnCharacterSay(CharacterSay);
		g_pGameWorldUIHandler->OnNpcTalk(NpcTalk);
	}

	return;
Exit0:

	return;
}

void KPlayerClient::OnSetForce(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL            bRetCode    = false;
	S2C_SET_FORCE*  pSetForce   = (S2C_SET_FORCE*)pbyData;
    KCharacter*     pCharacter  = NULL;

	if (IS_PLAYER(pSetForce->dwCharacterID))
	{
		pCharacter = g_pSO3World->m_PlayerSet.GetObj(pSetForce->dwCharacterID);
		KG_PROCESS_ERROR(pCharacter);
	}
	else
	{
		pCharacter = g_pSO3World->m_NpcSet.GetObj(pSetForce->dwCharacterID);
		KG_PROCESS_ERROR(pCharacter);
	}

    bRetCode = pCharacter->SetForceID(pSetForce->byForceID);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerClient::OnSendMailRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode    = false;
    S2C_SEND_MAIL_RESPOND*  pRespond    = (S2C_SEND_MAIL_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventMailRespond MailRespond;
        MailRespond.nRespondCode = pRespond->byResult;
        g_pGameWorldUIHandler->OnMailRespond(MailRespond);

        KUIEventSendMailResult MailResult;
        MailResult.nIndex  = pRespond->byRespondID;
        MailResult.nResult = pRespond->byResult;
        g_pGameWorldUIHandler->OnSendMailResult(MailResult);
    }

Exit0:
    return;
}

void KPlayerClient::OnGetMailListRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    S2C_GET_MAIL_LIST_RESPOND*  pRespond    = (S2C_GET_MAIL_LIST_RESPOND*)pbyData;

    KGLOG_PROCESS_ERROR((size_t)pRespond->wSize == sizeof(S2C_GET_MAIL_LIST_RESPOND) + sizeof(KMailListInfo) * pRespond->byMailCount);
    KGLOG_PROCESS_ERROR(g_pSO3World->m_MailClient.m_bMailListApplying);

    if (pRespond->byMailCount == 0) // The end of sync
    {
        if (!g_pSO3World->m_MailClient.m_bGotMessageMail)
        {
            g_pSO3World->m_MailClient.m_bGotMessageMail = true;
            g_pSO3World->m_MailClient.ApplyMessageMailContent();
        }

        g_pSO3World->m_MailClient.m_bMailListApplying = false;

        goto Exit0;
    }

    bRetCode = g_pSO3World->m_MailClient.AddMailList(pRespond->byMailCount, pRespond->MailListInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    DoGetMailListRequest(pRespond->MailListInfo[pRespond->byMailCount - 1].dwMailID);

    g_pSO3World->m_MailClient.CountMail();
    // 通知UI，更新
    if (g_pGameWorldUIHandler)
    {
        KUIEventMailListUpdate MailListUpdate;
        MailListUpdate.nUnreadCount = g_pSO3World->m_MailClient.m_nUnreadCount;
        MailListUpdate.nTotalCount  = g_pSO3World->m_MailClient.m_nTotalCount;
        g_pGameWorldUIHandler->OnMailListUpdate(MailListUpdate);
    }

Exit0:
    return;
}

void KPlayerClient::OnNewMailNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode    = false;
    S2C_NEW_MAIL_NOTIFY*    pNotify     = (S2C_NEW_MAIL_NOTIFY*)pbyData;

    bRetCode = g_pSO3World->m_MailClient.AddMailList(1, &(pNotify->NewMailListInfo));
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_MailClient.CountMail();

    // 通知UI，更新
    if (g_pGameWorldUIHandler)
	{
		KUIEventMailListUpdate  MailListUpdate;
        KUIEventNewMailNotify   NewMailNotify;

		MailListUpdate.nUnreadCount = g_pSO3World->m_MailClient.m_nUnreadCount;
		MailListUpdate.nTotalCount  = g_pSO3World->m_MailClient.m_nTotalCount;
		g_pGameWorldUIHandler->OnMailListUpdate(MailListUpdate);

        NewMailNotify.nUnreadCount  = g_pSO3World->m_MailClient.m_nUnreadCount;
        NewMailNotify.nTotalCount   = g_pSO3World->m_MailClient.m_nTotalCount;
        g_pGameWorldUIHandler->OnNewMailNotify(NewMailNotify);
	}

Exit0:
    return;
}

void KPlayerClient::OnSyncMailContent(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                     bResult         = false;
    BOOL                     bRetCode        = false;
    S2C_SYNC_MAIL_CONTENT*   pRespond        = (S2C_SYNC_MAIL_CONTENT*)pbyData;
    KMailContent*            pContent        = NULL;
    int                      nMailType       = 0;

    KG_PROCESS_ERROR(pRespond->byResult == mrcSucceed);

    assert(nSize - sizeof(S2C_SYNC_MAIL_CONTENT) >= sizeof(KMailContent));

    pContent = (KMailContent*)pRespond->byData;

    bRetCode = g_pSO3World->m_MailClient.UpdateMailContent(pRespond->dwMailID, pContent, &nMailType);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        KUIEventGetMailContent MailContent;
        MailContent.dwMailID = pRespond->dwMailID;

        if (nMailType == eMailType_GmMessage)
        {
            g_pGameWorldUIHandler->OnGetGmMessageMail(MailContent);

        }
        else if (nMailType == eMailType_PlayerMessage)
        {
            g_pGameWorldUIHandler->OnGetPlayerMessageMail(MailContent);
        }
        else
        {
            g_pGameWorldUIHandler->OnGetMailContent(MailContent);
        }
    }
    
    bResult = true;
Exit0:
    if (pRespond->byResult != mrcSucceed)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond MailRespond;
            MailRespond.nRespondCode = pRespond->byResult;
            g_pGameWorldUIHandler->OnMailRespond(MailRespond);
        }
    }
    return;
}

void KPlayerClient::OnMailGeneralRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_MAIL_GENERAL_RESPOND* pRespond = (S2C_MAIL_GENERAL_RESPOND*)pbyData;

    KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_MAIL_GENERAL_RESPOND));

    if (pRespond->byResult != mrcSucceed)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond MailRespond;
            MailRespond.nRespondCode = pRespond->byResult;
            g_pGameWorldUIHandler->OnMailRespond(MailRespond);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncForceId(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_SYNC_FORCE_ID* pSyncForceId = (S2C_SYNC_FORCE_ID*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->SetForceID(pSyncForceId->dwForceID);
	KGLOG_PROCESS_ERROR(bRetCode);

	return;
Exit0:

	return;
}

void KPlayerClient::OnSyncUserPreferences(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_SYNC_USER_PREFERENCES* pSyncUserPreferences = (S2C_SYNC_USER_PREFERENCES*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(
        pSyncUserPreferences->nOffset, pSyncUserPreferences->nLength, pSyncUserPreferences->byData
    );
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return ;
}

void KPlayerClient::OnSyncAllBookState(BYTE* pbyData, size_t uSize, int nFrame)
{
	BOOL                        bRetCode            = false;
	KPlayer*                    pPlayer             = NULL;
	S2C_SYNC_ALL_BOOK_STATE*    pSyncAllBookState   = (S2C_SYNC_ALL_BOOK_STATE*)pbyData;
    
    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_ALL_BOOK_STATE) + pSyncAllBookState->uDataLen == uSize);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncAllBookState->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_BookList.Load(pSyncAllBookState->byData, pSyncAllBookState->uDataLen);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    if (pSyncAllBookState->dwPlayerID != g_pSO3World->m_dwClientPlayerID) // 查看别人的书
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventPeekPlayerBookState UIParam;
		    UIParam.dwPlayerID = pSyncAllBookState->dwPlayerID;
		    g_pGameWorldUIHandler->OnPeekPlayerBookState(UIParam);
        }
    }
Exit0:
	return;
}

void KPlayerClient::OnSyncBookState(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	S2C_SYNC_BOOK_STATE* pSyncBookState = (S2C_SYNC_BOOK_STATE*)pbyData;
	
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_BookList.SetBookState(pSyncBookState->dwBookID, pSyncBookState->byState);
	KGLOG_PROCESS_ERROR(bRetCode);

	if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateBookState UIParam;
		UIParam.dwBookID = pSyncBookState->dwBookID;
		g_pGameWorldUIHandler->OnUpdateBookState(UIParam);
	}

Exit0:
	return;
}

void KPlayerClient::OnCharacterChangeModel(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                        bRetCode            = false;
    KCharacter*                 pCharacter          = NULL;
	S2C_CHARACTER_CHANGE_MODEL* pChangeModel        = (S2C_CHARACTER_CHANGE_MODEL*)pbyData;

    if (IS_PLAYER(pChangeModel->dwCharacterID))
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(pChangeModel->dwCharacterID);
    else
	    pCharacter = g_pSO3World->m_NpcSet.GetObj(pChangeModel->dwCharacterID);

	KG_PROCESS_ERROR(pCharacter);
	
	pCharacter->m_dwModelID = pChangeModel->dwModelID;

	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterDisplayData(pCharacter);

    if (g_pGameWorldUIHandler)
    {
        if (IS_PLAYER(pChangeModel->dwCharacterID))
        {
            KUIEventPlayerDisplayDataUpdate Param = { pChangeModel->dwCharacterID };
            g_pGameWorldUIHandler->OnPlayerDisplayDataUpdate(Param);
        }
        else
        {
            KUIEventNpcDisplayDataUpdate Param = { pChangeModel->dwCharacterID };
            g_pGameWorldUIHandler->OnNpcDisplayDataUpdate(Param);
        }
    }

Exit0:
	return;
}

void KPlayerClient::OnSyncNpcDialogFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KNpc* pNpc = NULL;

	S2C_SYNC_NPC_DIALOG_FLAG* pSyncNpcDialogFlag = (S2C_SYNC_NPC_DIALOG_FLAG*)pbyData;
	pNpc = g_pSO3World->m_NpcSet.GetObj(pSyncNpcDialogFlag->dwNpcID);
	KG_PROCESS_ERROR(pNpc);

	pNpc->m_bDialogFlag = pSyncNpcDialogFlag->bDialogFlag;

	//通知表项逻辑和UI
	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterUpdateTitleMark((KCharacter*)pNpc);

    if (g_pGameWorldUIHandler)
	{
		KUIEventNpcDisplayDataUpdate Param = { pNpc->m_dwID };
		g_pGameWorldUIHandler->OnNpcDisplayDataUpdate(Param);
	}

Exit0:
	return;
}

void KPlayerClient::OnSyncPKState(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = false;
	KPlayer*            pPlayer         = NULL;
	S2C_SYNC_PK_STATE*  pSyncPKState    = (S2C_SYNC_PK_STATE*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncPKState->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pPlayer->m_PK.SetState((PK_STATE)pSyncPKState->byPKState, pSyncPKState->nEndFrame, pSyncPKState->dwTargetID, pSyncPKState->byRefuseDuel);

Exit0:
	return;
}

void KPlayerClient::OnWinDuelBroadcast(BYTE* pbyData, size_t nSize, int nFrame)
{
	S2C_WIN_DUEL_BROADCAST* pWinDuelBroadcast = (S2C_WIN_DUEL_BROADCAST*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventWinDuel UIParam;
        UIParam.dwWinnerID = pWinDuelBroadcast->dwWinnerID;
        UIParam.dwLosserID = pWinDuelBroadcast->dwLosserID;
        g_pGameWorldUIHandler->OnWinDuel(UIParam);
    }

	return;
}

void KPlayerClient::OnSyncRoleDataOver(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode            = false;
    KPlayer*                        pPlayer             = NULL;
    KScene*                         pScene              = NULL;

    KGLogPrintf(KGLOG_INFO, "Sync role data over !");

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pScene = g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    bRetCode = pScene->ValidateRegions(
        pPlayer->m_nX / CELL_LENGTH / REGION_GRID_WIDTH,
        pPlayer->m_nY / CELL_LENGTH / REGION_GRID_HEIGHT
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_TongClient.TryGetTongName(pPlayer->m_dwTongID, pPlayer->m_szTongName, sizeof(pPlayer->m_szTongName));

	bRetCode = g_pSO3World->AddPlayer(pPlayer, pScene, pPlayer->m_nX, pPlayer->m_nY, pPlayer->m_nZ);
	KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ApexClient.StartApexClient();

    if (g_pGameWorldUIHandler)
    {
	    g_pGameWorldUIHandler->OnSyncRoleDataEnd();
		g_pGameWorldUIHandler->SetInLogin(FALSE);
		KUIEventPlayerEnterScene param = { pPlayer->m_dwID };
		g_pGameWorldUIHandler->OnPlayerEnterScene(param);
    }

Exit0:
	return;
}

void KPlayerClient::OnSyncEnableBankPackage(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL							bRetCode				= false;
	KPlayer*						pPlayer					= NULL;
	S2C_SYNC_ENABLE_BANK_PACKAGE*	pSyncEnableBankPackage	= (S2C_SYNC_ENABLE_BANK_PACKAGE*)pbyData;
	
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pPlayer->m_ItemList.SetEnablePackageCount(pSyncEnableBankPackage->nEnabledCount);

    if (g_pGameWorldUIHandler)
	    g_pGameWorldUIHandler->OnUpdateBankSlot();

Exit0:
	return;
}

void KPlayerClient::OnUpdateMapMark(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;
	S2C_UPDATE_MAP_MARK* pUpdateMapMark = (S2C_UPDATE_MAP_MARK*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateMapMark UIParam;
        UIParam.nX = pUpdateMapMark->nX;
        UIParam.nY = pUpdateMapMark->nY;
        UIParam.nZ = pUpdateMapMark->nZ;
        UIParam.nType = pUpdateMapMark->nType;
        strncpy(UIParam.szComment, pUpdateMapMark->szComment, MAX_MAP_MARK_COMMENT_LEN);
        UIParam.szComment[MAX_MAP_MARK_COMMENT_LEN - 1] = '\0';

        g_pGameWorldUIHandler->OnUpdateMapMark(UIParam);
    }
}

void KPlayerClient::OnSyncMidMapMark(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;
	S2C_SYNC_MID_MAP_MARK* pSyncMidMapMark = (S2C_SYNC_MID_MAP_MARK*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	KG_PROCESS_ERROR(pPlayer->m_dwID != pSyncMidMapMark->dwSrcPlayerID);
	KG_PROCESS_ERROR(pSyncMidMapMark->szComment);

    if (g_pGameWorldUIHandler)
	{
		KUIEventUpdateMidMapMark UIParam;
		UIParam.nMapID = pSyncMidMapMark->nMap;
		UIParam.nX = pSyncMidMapMark->nX;
		UIParam.nY = pSyncMidMapMark->nY;
		UIParam.nType = pSyncMidMapMark->nType;
		strncpy(UIParam.szComment, pSyncMidMapMark->szComment, MAX_MIDMAP_MARK_COMMENT_LEN);
		UIParam.szComment[MAX_MIDMAP_MARK_COMMENT_LEN - 1] = '\0';

		g_pGameWorldUIHandler->OnUpdateMidMapMark(UIParam);
	}

Exit0:
	return;
}

void KPlayerClient::OnPlaySound(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;
	S2C_PLAY_SOUND* pPlaySound = (S2C_PLAY_SOUND*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterSound((KCharacter*)pPlayer, pPlaySound->dwSoundID);

Exit0:
	return;
}

void KPlayerClient::OnPlayMiniGame(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;
	S2C_PLAY_MINI_GAME* pPlayMiniGame = (S2C_PLAY_MINI_GAME*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    if (g_pGameWorldUIHandler)
	{
		KUIEventPlayMiniGame UIParam;
		UIParam.dwGameID = pPlayMiniGame->dwGameID;

		g_pGameWorldUIHandler->OnPlayMiniGame(UIParam);
	}
Exit0:
	return;
}

void KPlayerClient::OnSyncHorseFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
	BOOL                    bRetCode    = false;
	KPlayer*                pPlayer     = NULL;
	S2C_SYNC_HORSE_FLAG*    pHorseFlag  = (S2C_SYNC_HORSE_FLAG*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pHorseFlag->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    if (pPlayer->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        if (pHorseFlag->bHorseFlag)
        {
            pPlayer->RideHorse();
        }
        else
        {
            pPlayer->DownHorse();
        }
    }
    else
    {
	    pPlayer->m_bOnHorse = pHorseFlag->bHorseFlag;
    }

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterMountHorse(pPlayer, pHorseFlag->bHorseFlag);

    if (g_pGameWorldUIHandler)
	{
		KUIEventPlayerDisplayDataUpdate Param = { pPlayer->m_dwID };
		g_pGameWorldUIHandler->OnPlayerDisplayDataUpdate(Param);
    }

Exit0:
	return;
}

void KPlayerClient::OnRoadTrackForceRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    KROUTE_NODE*                    pNode           = NULL;
    IKG_Buffer*                     piBuffer        = NULL;
    S2C_ROAD_TRACK_FORCE_RESPOND*   pTrackForce     = (S2C_ROAD_TRACK_FORCE_RESPOND*)pbyData;

    for (int i = 0; i < pTrackForce->byNodeCount; i++)
    {
        pNode = g_pSO3World->m_RoadManager.GetNodeInfo(pTrackForce->NodeForce[i].nNodeID);
        KGLOG_PROCESS_ERROR(pNode);

        pNode->eCamp    = (KCAMP)pTrackForce->NodeForce[i].byCamp;
        pNode->bEnable  = pTrackForce->NodeForce[i].byEnable;
    }

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnUpdateRoadTrackForce();

Exit0:
    return;
}

void KPlayerClient::OnSyncRouteNodeOpenList(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                        pPlayer         = NULL;
    KROUTE_NODE*                    pNode           = NULL;
    S2C_SYNC_ROUTE_NODE_OPEN_LIST*  pSyncOpenList   = (S2C_SYNC_ROUTE_NODE_OPEN_LIST*)pbyData;

    KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_SYNC_ROUTE_NODE_OPEN_LIST) + pSyncOpenList->byNodeCount * sizeof(int));

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_OpenRouteNodeList.clear();

    for (int i = 0; i < pSyncOpenList->byNodeCount; i++)
    {
        pNode = g_pSO3World->m_RoadManager.GetNodeInfo(pSyncOpenList->nNodeID[i]);
        KGLOG_PROCESS_ERROR(pNode);

        pPlayer->m_OpenRouteNodeList.push_back(pSyncOpenList->nNodeID[i]);
    }

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnUpdateRouteNodeOpenList();

Exit0:
    return;
}

void KPlayerClient::OnSyncPlayerStateInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode            = false;
    KPlayer*                        pPlayer             = NULL;
    S2C_SYNC_PLAYER_STATE_INFO*     pPlayerStateData    = (S2C_SYNC_PLAYER_STATE_INFO*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->SetMoveState(pPlayerStateData->byMoveState);

    pPlayer->m_nCurrentTrack                    = pPlayerStateData->wCurrentTrack;
    pPlayer->m_nMoveFrameCounter                = pPlayerStateData->nMoveFrameCounter;
    pPlayer->m_nFromNode                        = pPlayerStateData->nFromNode;
    pPlayer->m_nTargetCity                      = pPlayerStateData->nTargetCity;
    pPlayer->m_nExperience                      = pPlayerStateData->nExperience;

    pPlayer->m_nCurrentTrainValue               = pPlayerStateData->nCurrentTrainValue;
    pPlayer->m_nUsedTrainValue                  = pPlayerStateData->nUsedTrainValue;

    pPlayer->m_nCurrentPrestige                 = pPlayerStateData->nCurrentPrestige;

    pPlayer->m_nMaxLevel                        = pPlayerStateData->nMaxLevel;
    pPlayer->m_bHideHat                         = pPlayerStateData->bHideHat;

    if (pPlayer->m_bHideHat)
        pPlayer->m_wRepresentId[perHelmStyle] = 0;

    pPlayer->m_nContribution                    = pPlayerStateData->nContribution;

    switch (pPlayer->m_eMoveState)
    {
    case cmsOnAutoFly:
    case cmsOnFloat:
        break;

    case cmsOnDeath:
        break;

    case cmsOnSwim:
        pPlayer->m_eMoveState = cmsOnFloat;
        break;

    default:
        pPlayer->m_eMoveState = cmsOnStand;
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncPlayerTrain(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                 pPlayer         = NULL;
    S2C_SYNC_PLAYER_TRAIN*   pPlayerTrain    = (S2C_SYNC_PLAYER_TRAIN*)pbyData;
    KUIEventTrainValueUpdate param;

    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    param.nAddTrain = pPlayerTrain->nCurrentTrainValue - pPlayer->m_nCurrentTrainValue;

    pPlayer->m_nCurrentTrainValue   = pPlayerTrain->nCurrentTrainValue;
    pPlayer->m_nUsedTrainValue      = pPlayerTrain->nUsedTrainValue;
    
    g_pGameWorldUIHandler->OnTrainValueUpdate(param);

Exit0:
    return;
}

void KPlayerClient::OnBroadcastPlayerSchoolID(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                        pPlayer         = NULL;
    S2C_BROADCAST_PLAYER_SCHOOL_ID* pPlayerSchoolID = (S2C_BROADCAST_PLAYER_SCHOOL_ID*)pbyData;
    KUIEventUpdatePlayerSchoolID    UIParam;

    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPlayerSchoolID->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_dwSchoolID = pPlayerSchoolID->dwSchoolID;

    UIParam.dwPlayerID = pPlayer->m_dwID;
    UIParam.dwSchoolID = pPlayer->m_dwSchoolID;

    g_pGameWorldUIHandler->OnUpdatePlayerSchoolID(UIParam);

Exit0:
    return;
}

void KPlayerClient::OnSyncPrinkForce(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode        = false;
    KPlayer*                pPlayer         = NULL;
    S2C_SYNC_PRINK_FORCE*   pPrinkForce     = (S2C_SYNC_PRINK_FORCE*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->m_ReputeList.SetPrinkForce(pPrinkForce->byPrinkForceID, pPrinkForce->byLevel);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnUpdateAllRelation();

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();

    pPlayer->m_QuestList.UpdateNpcQuestMark(-1);

Exit0:
    return;
}

void KPlayerClient::OnSyncVisitMapInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode            = false;
    KPlayer*                    pPlayer             = NULL;
    S2C_SYNC_VISIT_MAP_INFO*    pSyncVisitMapInfo   = (S2C_SYNC_VISIT_MAP_INFO*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    pPlayer->m_SceneVisitRecord.Load(pSyncVisitMapInfo->byVisitMap, sizeof(pSyncVisitMapInfo->byVisitMap));
    
	if (g_pGameWorldUIHandler)
	{
		g_pGameWorldUIHandler->OnMapVisitFlagChanged();
	}

Exit0:
    return;
}

void KPlayerClient::OnSyncAccumulateValue(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_ACCUMULATE_VALUE*  pSyncAccumulateValue = (S2C_SYNC_ACCUMULATE_VALUE*)pbyData;
    KPlayer*                    pPlayer              = NULL;

    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_nAccumulateValue = (int)pSyncAccumulateValue->byAccumualtevalue;

    g_pGameWorldUIHandler->OnUpdateAccumulateValue();

Exit0:
    return;
}

void KPlayerClient::OnSetTarget(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SET_TARGET*             pSetTarget = (S2C_SET_TARGET*)pbyData;
    KCharacter*                 pCharacter = NULL;

    assert(g_pGameWorldUIHandler);

    if (IS_PLAYER(pSetTarget->dwCharacterID))
    {
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(pSetTarget->dwCharacterID);
    }
    else
    {
        pCharacter = g_pSO3World->m_NpcSet.GetObj(pSetTarget->dwCharacterID);
    }
    KG_PROCESS_ERROR(pCharacter);

    pCharacter->m_SelectTarget.SetTarget((TARGET_TYPE)pSetTarget->byTargetType, pSetTarget->dwTargetID);

    g_pGameWorldUIHandler->OnUpdateSelectTarget();

Exit0:
    return;
}

void KPlayerClient::OnSyncKillPoint(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_KILL_POINT* pSyncKillPoint = (S2C_SYNC_KILL_POINT*)pbyData;
    KPlayer*             pPlayer        = NULL;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncKillPoint->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_nCurrentKillPoint = pSyncKillPoint->wCurrentKillPoint;
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventKillPoint	Param;

        Param.dwPlayerID	= pSyncKillPoint->dwPlayerID;
		Param.dwPoint		= pSyncKillPoint->wCurrentKillPoint;
		g_pGameWorldUIHandler->OnUpdateKillPoint(Param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSetPlayerRedName(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SET_PLAYER_RED_NAME* pSetRedName    = (S2C_SET_PLAYER_RED_NAME*)pbyData;
    KPlayer*                 pPlayer        = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSetRedName->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    pPlayer->m_bRedName = pSetRedName->byRedName;  

    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateRelation UIEventUpdateRelation;
        UIEventUpdateRelation.dwCharacterID = pSetRedName->dwPlayerID;
        g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pPlayer);  
    }
    
Exit0:
    return;
}

void KPlayerClient::OnSetPlayerDialogFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SET_PLAYER_DIALOG_FLAG* pSetPlayerDialogFlag = (S2C_SET_PLAYER_DIALOG_FLAG*)pbyData;
    KPlayer*                    pPlayer              = NULL;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSetPlayerDialogFlag->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_bCannotDialogWithNPC = pSetPlayerDialogFlag->byDialogFlag;

Exit0:
    return;
}

void KPlayerClient::OnSetOnlyReviveInSitu(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SET_ONLY_REVIVE_IN_SITU* pSetOnlyReviveInSitu = (S2C_SET_ONLY_REVIVE_IN_SITU*)pbyData;
    KPlayer*                     pPlayer              = NULL;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSetOnlyReviveInSitu->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_bOnlyReviveInSitu = pSetOnlyReviveInSitu->byOnlyReviveInSitu;

Exit0:
    return;
}

void KPlayerClient::OnSyncFormationCoefficient(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode      = false;
    S2C_SYNC_FORMATION_COEFFICIENT* pFormationCof = (S2C_SYNC_FORMATION_COEFFICIENT*)pbyData;
    KTEAM_MEMBER_INFO*              pMemberInfo   = NULL;

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(pFormationCof->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nFormationCoefficient = pFormationCof->nCoefficient;

    if (g_pGameWorldUIHandler)
    {
        KUIEventPlayerStateUpdate param = { pFormationCof->dwMemberID };
        g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnJoinBattleFieldQueueRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_JOIN_BATTLE_FIELD_QUEUE_RESPOND* pJoinRespond = (S2C_JOIN_BATTLE_FIELD_QUEUE_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventJoinBattleFieldQueue param;

        param.nErrorCode = pJoinRespond->nErrorCode;

        g_pGameWorldUIHandler->OnJoinBattleFieldQueue(param);
    }
}
 
void KPlayerClient::OnLeaveBattleFieldQueueRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_LEAVE_BATTLE_FIELD_QUEUE_RESPOND* pLeaveRespond = (S2C_LEAVE_BATTLE_FIELD_QUEUE_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventLeaveBattleFieldQueue param;

        param.nErrorCode = pLeaveRespond->nErrorCode;

        g_pGameWorldUIHandler->OnLeaveBattleFieldQueue(param);
    }
}

void KPlayerClient::OnBattleFieldNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_BATTLE_FIELD_NOTIFY* pNotify = (S2C_BATTLE_FIELD_NOTIFY*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventBattleFieldNotify param;

        param.nType             = pNotify->eType;
        param.nAvgQueueTime     = (int)pNotify->nAvgQueueTime;
        param.nPassTime         = pNotify->nPassTime;
        param.dwMapID           = pNotify->dwMapID;
        param.nCopyIndex        = (int)pNotify->nCopyIndex;

        g_pGameWorldUIHandler->OnBattleFieldNotify(param);
    }
}

void KPlayerClient::OnSyncBattleFieldList(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_BATTLE_FIELD_LIST* pBattleList = (S2C_SYNC_BATTLE_FIELD_LIST*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateBattleFieldList param;

        param.dwMapID       = pBattleList->dwMapID;
        param.nCopyIndex    = pBattleList->nCopyIndex;

        g_pGameWorldUIHandler->OnUpdateBattleFieldList(param);
    }
}

void KPlayerClient::OnSyncBattleFieldSide(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_BATTLE_FIELD_SIDE* pBattleFieldSide = (S2C_SYNC_BATTLE_FIELD_SIDE*)pbyData;
    KPlayer*                    pPlayer          = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pBattleFieldSide->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    if (pBattleFieldSide->nSide == INVALID_BATTLE_SIDE)
    {
        g_pSO3World->m_dwBattleFieldPQID            = 0;
        g_pSO3World->m_dwBattleFieldPQTemplateID    = 0;
        g_pSO3World->m_nBattleFieldPQBeginTime      = 0;
        g_pSO3World->m_nBattleFieldPQEndTime        = 0;
        memset(g_pSO3World->m_nBattleFieldObjectiveCurrent, 0, sizeof(g_pSO3World->m_nBattleFieldObjectiveCurrent));
        memset(g_pSO3World->m_nBattleFieldObjectiveTarget, 0, sizeof(g_pSO3World->m_nBattleFieldObjectiveTarget));
    }

    pPlayer->m_nBattleFieldSide = pBattleFieldSide->nSide;

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pPlayer);
    }

    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateRelation param = {pPlayer->m_dwID};

        g_pGameWorldUIHandler->OnUpdateRelation(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnBattleFieldEnd(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_BATTLE_FIELD_END*   pBattleFieldEnd = (S2C_BATTLE_FIELD_END*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnBattleFieldEnd();
    }
Exit0:
    return;
}

void KPlayerClient::OnSyncCampInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                bRetCode        = false;
    S2C_SYNC_CAMP_INFO* pSyncCampInfo   = (S2C_SYNC_CAMP_INFO*)pbyData;

    g_pSO3World->m_CampInfo.SetCampLevel(pSyncCampInfo->byCampLevel);
    g_pSO3World->m_CampInfo.SetCampScore(pSyncCampInfo->nCampScore);
    
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateCampInfo();
    }
    
Exit0:
    return;
}

void KPlayerClient::OnSyncCurrentPrestige(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode                = false;
    S2C_SYNC_CURRENT_PRESTIGE*  pSyncCurrentPrestige    = (S2C_SYNC_CURRENT_PRESTIGE*)pbyData;
    KPlayer*                    pPlayer                 = NULL;
    int                         nOldPrestige            = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    nOldPrestige = pPlayer->m_nCurrentPrestige;
    pPlayer->m_nCurrentPrestige = pSyncCurrentPrestige->nCurrentPrestige;
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdatePrestige Event;
        Event.nOldPrestige = nOldPrestige;
        g_pGameWorldUIHandler->OnUpdatePrestige(Event);

        if (
            nOldPrestige <= pPlayer->m_nCurrentPrestige &&
            pPlayer->m_nCurrentPrestige >= g_pSO3World->m_Settings.m_ConstList.nMaxPrestige
        )
        {
            g_pGameWorldUIHandler->OnMaxPrestigeNotify();
        }
    }
    
Exit0:
    return;
}

void KPlayerClient::OnSyncTongMemberInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode        = false;
    S2C_SYNC_TONG_MEMBER_INFO*  pSyncRespond    = (S2C_SYNC_TONG_MEMBER_INFO*)pbyData;

    assert(g_pSO3World);

    bRetCode = g_pSO3World->m_TongClient.UpdateMemberInfo(pSyncRespond->TongMemberInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerClient::OnTongDeleteMemberNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode    = false;
    S2C_DELETE_TONG_MEMBER_NOTIFY*  pPackage    = (S2C_DELETE_TONG_MEMBER_NOTIFY*)pbyData;
    
    assert(g_pSO3World);
    bRetCode = g_pSO3World->m_TongClient.DeleteMember(pPackage->dwMemberID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerClient::OnApplyTongInfoRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode    = false;
    S2C_APPLY_TONG_INFO_RESPOND*    pRespond    = (S2C_APPLY_TONG_INFO_RESPOND*)pbyData;

    assert(g_pSO3World);

    switch (pRespond->byRespondType)
    {
    case tdtBaseInfo:
        KGLOG_PROCESS_ERROR(pRespond->wSize == sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_BASE_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateBaseInfo(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtDevelopmentInfo:
        KGLOG_PROCESS_ERROR(pRespond->wSize == sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_DEVELOPMENT_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateDevelopmentInfo(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtAnnouncement:
        KGLOG_PROCESS_ERROR(pRespond->wSize >= sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_TEXT_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateAnnouncement(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtOnlineMessage:
        KGLOG_PROCESS_ERROR(pRespond->wSize >= sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_TEXT_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateOnlineMessage(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtIntroduction:
        KGLOG_PROCESS_ERROR(pRespond->wSize >= sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_TEXT_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateIntroduction(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtRules:
        KGLOG_PROCESS_ERROR(pRespond->wSize >= sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_TEXT_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateRules(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtSchemaInfo:
        KGLOG_PROCESS_ERROR(pRespond->wSize == sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_SCHEMA_DATA_INFO));
        bRetCode = g_pSO3World->m_TongClient.UpdateSchemaInfo(pRespond->byData, nSize - sizeof(S2C_APPLY_TONG_INFO_RESPOND));
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case tdtMemorabilia:
        KGLOG_PROCESS_ERROR(pRespond->wSize >= sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_MEMORABILIA_INFO));
        {
            KTONG_MEMORABILIA_INFO* pInfo = (KTONG_MEMORABILIA_INFO*)pRespond->byData;
            
            KGLOG_PROCESS_ERROR(
                pRespond->wSize == sizeof(S2C_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_MEMORABILIA_INFO) + pInfo->byCount * sizeof(KTongIncidentRecord)
            );

            bRetCode = g_pSO3World->m_TongClient.UpdateMemorabilia(pInfo->Memorabilia, (unsigned)pInfo->byCount);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        break;
    default:
        KGLogPrintf(KGLOG_DEBUG, "[KPlayerClient] unrecognize Apply Tong Info Respond InfoRespond type!\n");
        break;
    }

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }

Exit0:
    return;
}

void KPlayerClient::OnUpdateTongClientDataVersion(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                    bRetCode        = false;
    S2C_UPDATE_TONG_CLIENT_DATA_VERSION*    pUpdateVersion  = (S2C_UPDATE_TONG_CLIENT_DATA_VERSION*)pbyData;
    KTongClient*                            pTongClient     = NULL;
    
    assert(g_pSO3World);
    pTongClient = &(g_pSO3World->m_TongClient);

    switch (pUpdateVersion->byFrameType)
    {
    case tufInfo:
        pTongClient->m_nLastInfoUpdateFrame = pUpdateVersion->nUpdateFrame;
        break;

    case tufRoster:
        pTongClient->m_nLastRosterUpdateFrame = pUpdateVersion->nUpdateFrame;
        break;

    default:
        KGLogPrintf(KGLOG_DEBUG, "[KPlayerClient] unrecognized UpdateFrame type\n");
        goto Exit0;
    }

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongClientDataFinish((KTONG_UPDATE_FLAG)pUpdateVersion->byFrameType);
    }
Exit0:
    return;
}

void KPlayerClient::OnSyncTongRepertoryPageRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                    bRetCode            = false;
    BYTE*                                   pbyOffset           = NULL;
    BYTE*                                   pbyEnd              = pbyData + nSize;
    KTONG_ITEM_SYNC_DATA*                   pItemData           = NULL;
    KItem*                                  pItem               = NULL;
    size_t                                  uItemSize           = 0;
    size_t                                  uItemInfoSize       = 0;
    KTongRepertoryCachePage*                pPage               = NULL;
    S2C_SYNC_TONG_REPERTORY_PAGE_RESPOND*   pRespond            = (S2C_SYNC_TONG_REPERTORY_PAGE_RESPOND*)pbyData;
    int                                     nUpdateItemCount    = 0;
    int                                     nInvailedItemCount  = 0;
    BYTE                                    byTargetPos         = 0;

    KGLOG_PROCESS_ERROR(pRespond->byPageIndex < TONG_REPERTORY_PAGE_NUM);
    pPage = &g_pSO3World->m_TongClient.m_RepertoryCache.m_Pages[pRespond->byPageIndex];
    KGLOG_PROCESS_ERROR(pRespond->nServerCounter >= pPage->m_nLastUpdateCounter);

    nUpdateItemCount    = pRespond->byUpdateItemCount;
    nInvailedItemCount  = pRespond->byInvailedItemCount;

    pbyOffset = pRespond->byData;
    for (int i = 0; i < nUpdateItemCount; i++)
    {
        pItemData       = (KTONG_ITEM_SYNC_DATA*)pbyOffset;
        uItemSize       = (size_t)pItemData->bySize;
        uItemInfoSize   = sizeof(KTONG_ITEM_SYNC_DATA) + uItemSize;

        KGLOG_PROCESS_ERROR(pbyOffset + uItemInfoSize <= pbyEnd);
        pbyOffset += uItemInfoSize;

        KGLOG_PROCESS_ERROR(pItemData->byPos < TONG_REPERTORY_PAGE_CAPACITY);
        
        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pItemData->byData, uItemSize);
        KGLOG_PROCESS_ERROR(pItem);

        if (pPage->m_pItems[pItemData->byPos])
        {
            bRetCode = g_pSO3World->m_ItemManager.FreeItem(pPage->m_pItems[pItemData->byPos]);
            KGLOG_PROCESS_ERROR(bRetCode);

            pPage->m_pItems[pItemData->byPos] = NULL;
        }

        pPage->m_pItems[pItemData->byPos] = pItem;
    }

    KGLOG_PROCESS_ERROR(pbyOffset + sizeof(BYTE) * nInvailedItemCount == pbyEnd);
    for (int i = 0; i < nInvailedItemCount; i++)
    {
        byTargetPos = *pbyOffset;
        ++pbyOffset;

        KGLOG_PROCESS_ERROR(byTargetPos < TONG_REPERTORY_PAGE_CAPACITY);

        if (pPage->m_pItems[byTargetPos])
        {
            bRetCode = g_pSO3World->m_ItemManager.FreeItem(pPage->m_pItems[byTargetPos]);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        pPage->m_pItems[byTargetPos] = NULL;
    }

    pPage->m_nLastUpdateCounter = pRespond->nServerCounter;
    pPage->m_bGetRight          = pRespond->byGetRight ? true : false;
    pPage->m_bPutRight          = pRespond->byPutRight ? true : false;

    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateTongRepertoryPage UpdateEvent;
        UpdateEvent.nPageIndex = (int)pRespond->byPageIndex;

        g_pGameWorldUIHandler->OnUpdateTongRepertoryPage(UpdateEvent);
    }

Exit0:
    return;
}

void KPlayerClient::OnInvitePlayerJoinTongRequest(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_INVITE_PLAYER_JOIN_TONG_REQUEST* pRequest = (S2C_INVITE_PLAYER_JOIN_TONG_REQUEST*)pbyData;

    pRequest->szInviterName[sizeof(pRequest->szInviterName) - 1] = '\0';
    pRequest->szTongName[sizeof(pRequest->szTongName) -1] = '\0';
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventInviteJoinTongRequest param;

        param.dwInviterID = pRequest->dwInviterID;
        param.dwTongID = pRequest->dwTongID;

        strncpy(param.szInviterName, pRequest->szInviterName, sizeof(param.szInviterName));
        param.szInviterName[sizeof(param.szInviterName) - 1] = '\0';

        strncpy(param.szTongName, pRequest->szTongName, sizeof(param.szTongName));
        param.szTongName[sizeof(param.szTongName) - 1] = '\0';

        g_pGameWorldUIHandler->OnInvitePlayerJoinTongRequest(param);
    }
}

void KPlayerClient::OnChangePlayerTongNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                        pPlayer = NULL;
    S2C_CHANGE_PLAYER_TONG_NOTIFY*  pNotify = (S2C_CHANGE_PLAYER_TONG_NOTIFY*)pbyData;

    KGLOG_PROCESS_ERROR(pNotify->byReason >= ctrCreate && pNotify->byReason <= ctrFired);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_dwTongID = pNotify->dwTongID;
    g_pSO3World->m_TongClient.Reset();

    if (g_pGameWorldUIHandler)
    {
        KUIEventChangeTongNotify param;

        param.eReason = (KCHANGE_TONG_REASON)pNotify->byReason;

        strncpy(param.szTongName, pNotify->szTongName, sizeof(param.szTongName));
        param.szTongName[sizeof(param.szTongName) -  1] = '\0';

        g_pGameWorldUIHandler->OnPlayerChangeTongNotify(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnBroadcastTongMessage(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_TONG_BROADCAST_MESSAGE* pMessage = (S2C_TONG_BROADCAST_MESSAGE*)pbyData;

    KGLOG_PROCESS_ERROR(pMessage->byCode > tbmInvalid && pMessage->byCode < tbmTotal);

    if (g_pGameWorldUIHandler)
    {
        switch(pMessage->byCode)
        {
        case tbmTongStateChange:
            {
                BYTE                    byTongState = 0;
                KUIEventTongStateChange param;
                
                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(BYTE));
                
                byTongState = *((BYTE*)pMessage->byData);
                KGLOG_PROCESS_ERROR(byTongState >= tsInvalid && byTongState < tsTotal);

                param.nTongState = (int)byTongState;
                g_pGameWorldUIHandler->OnTongStateChange(param);
            }
            break;
        case tbmGroupRightChange:
            {
                KTONG_CHANGE_GROUP_RIGHT_BROADCAST* pChangeInfo = NULL;
                KUIEventTongGroupRightChange        param;

                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_CHANGE_GROUP_RIGHT_BROADCAST));

                pChangeInfo = (KTONG_CHANGE_GROUP_RIGHT_BROADCAST*)pMessage->byData;

                strncpy(param.szGroupName, pChangeInfo->szGroupName, sizeof(param.szGroupName));
                param.szGroupName[sizeof(param.szGroupName) - 1] = '\0';
                g_pGameWorldUIHandler->OnTongGroupRightChange(param);
            }
            break;
        case tbmGroupNameChange:
            {
                KTONG_CHANGE_GROUP_NAME_BROADCAST*  pChangeInfo = NULL;
                KUIEventTongGroupNameChange         param;

                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_CHANGE_GROUP_NAME_BROADCAST));

                pChangeInfo = (KTONG_CHANGE_GROUP_NAME_BROADCAST*)pMessage->byData;

                strncpy(param.szOldGroupName, pChangeInfo->szOldGroupName, sizeof(param.szOldGroupName));
                param.szOldGroupName[sizeof(param.szOldGroupName) - 1] = '\0';

                strncpy(param.szNewGroupName, pChangeInfo->szNewGroupName, sizeof(param.szNewGroupName));
                param.szNewGroupName[sizeof(param.szNewGroupName) - 1] = '\0';

                g_pGameWorldUIHandler->OnTongGroupNameChange(param);
            }
            break;
        case tbmGroupWageChange:
            {
                KTONG_CHANGE_GROUP_WAGE_BROADCAST*  pChangeInfo = NULL;
                KUIEventTongGroupWageChange         param;

                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_CHANGE_GROUP_WAGE_BROADCAST));

                pChangeInfo = (KTONG_CHANGE_GROUP_WAGE_BROADCAST*)pMessage->byData;
                
                KGLOG_PROCESS_ERROR(pChangeInfo->nWage >= 0);

                strncpy(param.szGroupName, pChangeInfo->szGroupName, sizeof(param.szGroupName));
                param.szGroupName[sizeof(param.szGroupName) - 1] = '\0';

                param.nWage = pChangeInfo->nWage;

                g_pGameWorldUIHandler->OnTongGroupWageChange(param);
            }
            break;
        case tbmMemberJoin:
        case tbmMemberQuit:
        case tbmMemberFired:
            {
                KTONG_MEMBER_JOIN_OR_QUIT_BROADCAST*    pChangeInfo = NULL;
                KUIEventTongMemberJoinOrQuit            param;

                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_MEMBER_JOIN_OR_QUIT_BROADCAST));
            
                pChangeInfo = (KTONG_MEMBER_JOIN_OR_QUIT_BROADCAST*)pMessage->byData;

                strncpy(param.szMemberName, pChangeInfo->szMemberName, sizeof(param.szMemberName));
                param.szMemberName[sizeof(param.szMemberName) - 1] = '\0';
                param.nType = (int)pMessage->byCode;

                g_pGameWorldUIHandler->OnTongMemberJoinOrQuit(param);
            }
            break;
        case tbmMemberChangeGroup:
            {
                KTONG_MEMBER_CHANGE_GROUP_BROADCAST*    pChangeInfo = NULL;
                KUIEventTongMemberChangeGroup           param;

                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_MEMBER_CHANGE_GROUP_BROADCAST));

                pChangeInfo = (KTONG_MEMBER_CHANGE_GROUP_BROADCAST*)pMessage->byData;

                strncpy(param.szMemberName, pChangeInfo->szMemberName, sizeof(param.szMemberName));
                param.szMemberName[sizeof(param.szMemberName) - 1] = '\0';

                strncpy(param.szOldGroupName, pChangeInfo->szOldGroupName, sizeof(param.szOldGroupName));
                param.szOldGroupName[sizeof(param.szOldGroupName) - 1] = '\0';

                strncpy(param.szNewGroupName, pChangeInfo->szNewGroupName, sizeof(param.szNewGroupName));
                param.szNewGroupName[sizeof(param.szNewGroupName) - 1] = '\0';

                g_pGameWorldUIHandler->OnTongMemberChangeGroup(param);
            }
            break;
        case tbmMasterChange:
            {
                KTONG_CHANGE_MASTER_BROADCAST*  pChangeInfo = NULL;
                KUIEventTongMasterChange        param;

                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_CHANGE_MASTER_BROADCAST));

                pChangeInfo = (KTONG_CHANGE_MASTER_BROADCAST*)pMessage->byData;

                strncpy(param.szOldMasterName, pChangeInfo->szOldMasterName, sizeof(param.szOldMasterName));
                param.szOldMasterName[sizeof(param.szOldMasterName) - 1] = '\0';

                strncpy(param.szNewMasterName, pChangeInfo->szNewMasterName, sizeof(param.szNewMasterName));
                param.szNewMasterName[sizeof(param.szNewMasterName) - 1] = '\0';

                g_pGameWorldUIHandler->OnTongMasterChange(param);
            }
            break;
        case tbmCampChange:
            {
                BYTE byCamp = 0;
                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(BYTE));
                byCamp = *pMessage->byData;

                KGLOG_PROCESS_ERROR(byCamp == cGood || byCamp == cEvil);
                g_pGameWorldUIHandler->OnTongCampChange((KCAMP )byCamp);
            }
            break;
        case tbmMemberOnline:
        case tbmMemberOffline:
            {
                KTONG_MEMBER_ONLINE_STATE_CHANGE_BROADCAST* pChangeInfo = NULL;
                KUIEventTongMemberOnlineStateChange         param;
                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_MEMBER_ONLINE_STATE_CHANGE_BROADCAST));
                pChangeInfo = (KTONG_MEMBER_ONLINE_STATE_CHANGE_BROADCAST*)pMessage->byData;

                strncpy(param.szMemberName, pChangeInfo->szMemberName, sizeof(param.szMemberName));
                param.szMemberName[sizeof(param.szMemberName) - 1] = '\0';

                param.bIsOnline = (pMessage->byCode == tbmMemberOnline);

                g_pGameWorldUIHandler->OnTongMemberOnlineStateChange(param);
            }
            break;
        case tbmGroupEnabled:
            {
                KTONG_GROUP_ENABLED_BROADCAST*  pGroupInfo = NULL;
                KUIEventTongGroupEnabled        param;
                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(KTONG_GROUP_ENABLED_BROADCAST));

                pGroupInfo = (KTONG_GROUP_ENABLED_BROADCAST*)pMessage->byData;

                strncpy(param.szGroupName, pGroupInfo->szGroupName, sizeof(param.szGroupName));
                param.szGroupName[sizeof(param.szGroupName) - 1] = '\0';

                g_pGameWorldUIHandler->OnTongGroupEnabled(param);
            }
            break;
        case tbmMaxMemberCountChange:
            {
                KUIEventTongMaxMemberCountChange param;
                KGLOG_PROCESS_ERROR(nSize == sizeof(S2C_TONG_BROADCAST_MESSAGE) + sizeof(int));

                param.nMaxMemberCount = *((int*)pMessage->byData);

                g_pGameWorldUIHandler->OnTongMaxMemberCountChange(param);
            }
            break;
        default:
            assert(0);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnGetTongDescriptionRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_GET_TONG_DESCRIPTION_RESPOND*   pRespond = (S2C_GET_TONG_DESCRIPTION_RESPOND*)pbyData;

    pRespond->szTongName[sizeof(pRespond->szTongName) - 1] = '\0';
    g_pSO3World->m_TongClient.OnGetTongDescrioptionRespond(pRespond->dwTongID, pRespond->szTongName);
}

void KPlayerClient::OnBroadcastPlayerChangeTong(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                            pPlayer = NULL;
    S2C_BROADCAST_PLAYER_CHANGE_TONG*   pChangeInfo = (S2C_BROADCAST_PLAYER_CHANGE_TONG*)pbyData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pChangeInfo->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_dwTongID = pChangeInfo->dwTongID;
    
	if (g_pGameWorldRepresentHandler)
    {
        g_pSO3World->m_TongClient.TryGetTongName(pPlayer->m_dwTongID, pPlayer->m_szTongName, sizeof(pPlayer->m_szTongName));
        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation((KCharacter*)pPlayer);
	}
Exit0:
    return;
}

void KPlayerClient::OnSyncTongOnlineMessage(BYTE* pbyData, size_t nSize, int nFrame)
{
    KPlayer*                        pPlayer     = NULL;
    S2C_SYNC_TONG_ONLINE_MESSAGE*   pMsg        = (S2C_SYNC_TONG_ONLINE_MESSAGE*)pbyData;
    size_t                          uDataLen    = 0;

    KGLOG_PROCESS_ERROR(pMsg->wSize > sizeof(S2C_SYNC_TONG_ONLINE_MESSAGE));

    uDataLen = pMsg->wSize - sizeof(S2C_SYNC_TONG_ONLINE_MESSAGE);
    pMsg->szOnlineMessage[uDataLen - 1] = '\0';

    strncpy(g_pSO3World->m_TongClient.m_szOnlineMessage, pMsg->szOnlineMessage, sizeof(g_pSO3World->m_TongClient.m_szOnlineMessage));
    g_pSO3World->m_TongClient.m_szOnlineMessage[sizeof(g_pSO3World->m_TongClient.m_szOnlineMessage) - 1] = '\0';

    if (g_pGameWorldUIHandler)
    {
        KUIEventShowTongOnlineMessage param;
        param.pszOnlineMessage = g_pSO3World->m_TongClient.m_szOnlineMessage;

        g_pGameWorldUIHandler->OnShowTongOnlineMessage(param);
    }
Exit0:
    return;
}

void KPlayerClient::OnApplyOpenTongRepertoryRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_APPLY_OPEN_TONG_REPERTORY_RESPOND*  pRespond = (S2C_APPLY_OPEN_TONG_REPERTORY_RESPOND*)pbyData;
    int                                     nPageNum = 0;

    nPageNum = (int)pRespond->byRepertoryPageNum;
    KGLOG_PROCESS_ERROR(nPageNum >= 0 && nPageNum <= TONG_REPERTORY_PAGE_NUM);

    g_pSO3World->m_TongClient.m_RepertoryCache.m_nMaxPageNum = nPageNum;
    
    if (g_pGameWorldUIHandler)
    {
        KUIEventOpenTongRepertory param;
        
        strncpy(param.szTongName, pRespond->szTongName, sizeof(param.szTongName));
        param.szTongName[sizeof(param.szTongName) - 1] = '\0';

        param.nEnabledPageNum = (int)pRespond->byRepertoryPageNum;

        g_pGameWorldUIHandler->OnOpenTongRepertory(param);
    }
Exit0:
    return;
}

void KPlayerClient::OnSyncTongHistoryRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                            bRetCode    = false;
    S2C_SYNC_TONG_HISTORY_RESPOND*  pRespond    = (S2C_SYNC_TONG_HISTORY_RESPOND*)pbyData;
    int                             nCount      = 0;
    int                             nType       = 0;

    KGLOG_PROCESS_ERROR(nSize > sizeof(S2C_SYNC_TONG_HISTORY_RESPOND));

    nCount = (int)pRespond->byCount;
    KGLOG_PROCESS_ERROR(nCount > 0);

    KGLOG_PROCESS_ERROR(pRespond->byType < ohtTotal);
    nType = (KTONG_OPERATION_HISTORY_TYPE)pRespond->byType;

    bRetCode = g_pSO3World->m_TongClient.UpdateHistory(
        nType, pRespond->dwStartIndex, pRespond->byCount, 
        pRespond->byData, nSize - sizeof(S2C_SYNC_TONG_HISTORY_RESPOND)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        // g_pGameWorldUIHandler->UpdateHistoryPage(nPageIndex, dwTopIndex, dwButtonIndex);
    }
Exit0:
    return;
}

void KPlayerClient::OnAuctionLookupRespond(BYTE* pbyData, size_t uSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    S2C_AUCTION_LOOKUP_RESPOND* pRespond    = (S2C_AUCTION_LOOKUP_RESPOND*)pbyData;

    KG_PROCESS_ERROR(pRespond->byCode == arcSucceed);

    bRetCode = g_pSO3World->m_AuctionClient.UpdateItemList((int)pRespond->byRespondID, &pRespond->Page);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    if (g_pGameWorldUIHandler)
    {
        KUIEventAuctionLookupRespond Param;

        Param.nApplyType    = (int)pRespond->byRespondID;
        Param.nRespondCode  = (int)pRespond->byCode;

        g_pGameWorldUIHandler->OnAuctionLookupRespond(Param);
    }
    return;
}

void KPlayerClient::OnAuctionBidRespond(BYTE* pbyData, size_t uSize, int nFrame)
{
    S2C_AUCTION_BID_RESPOND*    pRespond = (S2C_AUCTION_BID_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventAuctionCommonRespond Param;

        Param.nRespondCode = (int)pRespond->byCode;

        g_pGameWorldUIHandler->OnAuctionBidRespond(Param);
    }
}

void KPlayerClient::OnAuctionSellRespond(BYTE* pbyData, size_t uSize, int nFrame)
{
    S2C_AUCTION_SELL_RESPOND*   pRespond = (S2C_AUCTION_SELL_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventAuctionCommonRespond Param;

        Param.nRespondCode = (int)pRespond->byCode;

        g_pGameWorldUIHandler->OnAuctionSellRespond(Param);
    }
}

void KPlayerClient::OnAuctionCancelRespond(BYTE* pbyData, size_t uSize, int nFrame)
{
    S2C_AUCTION_CANCEL_RESPOND* pRespond = (S2C_AUCTION_CANCEL_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventAuctionCommonRespond Param;

        Param.nRespondCode = (int)pRespond->byCode;

        g_pGameWorldUIHandler->OnAuctionCancelRespond(Param);
    }
}

void KPlayerClient::OnAuctionMessageNotify(BYTE* pbyData, size_t uSize, int nFrame)
{
    S2C_AUCTION_MESSAGE_NOTIFY* pNotify = (S2C_AUCTION_MESSAGE_NOTIFY*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventAuctionMessageNotify Param;

        Param.nNotifyCode   = (int)pNotify->byCode;

        strncpy(Param.szSaleName, pNotify->szSaleName, sizeof(Param.szSaleName));
        Param.szSaleName[sizeof(Param.szSaleName) - 1] = '\0';

        Param.nPrice        = pNotify->nPrice;

        g_pGameWorldUIHandler->OnAuctionMessageNotify(Param);
    }
}

void KPlayerClient::OnSyncHeroFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_HERO_FLAG*     pSyncHeroFlag   = (S2C_SYNC_HERO_FLAG*)pbyData;
    KPlayer*                pPlayer         = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    pPlayer->m_bHeroFlag = (BOOL)pSyncHeroFlag->byHeroFlag;

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateHeroFlag();
    }

Exit0:
    return;
}

void KPlayerClient::OnSetCamp(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL            bRetCode    = false;
	S2C_SET_CAMP*   pSetCamp    = (S2C_SET_CAMP*)pbyData;
    KPlayer*        pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSetCamp->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->SetCamp((KCAMP)pSetCamp->byCamp);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerClient::OnSyncBattleFieldStatistics(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_BATTLEFIELD_STATISTICS*    pStatistics = (S2C_SYNC_BATTLEFIELD_STATISTICS*)pbyData;
    KPQ_STATISTICS_DATA*                pData       = NULL;

    if (pStatistics->dwPlayerID == ERROR_ID)
    {
        if (g_pGameWorldUIHandler)
            g_pGameWorldUIHandler->OnSyncBattleFieldStatistics();

        goto Exit0;
    }

    pData = &g_pSO3World->m_BattleFieldStatisticsMap[pStatistics->dwPlayerID];

    pData->dwPlayerForceID  = pStatistics->dwPlayerForceID;
    pData->nBattleFieldSide = pStatistics->nBattleFieldSide;
    strncpy(pData->szPlayerName, pStatistics->szPlayerName, sizeof(pData->szPlayerName));
    pData->szPlayerName[sizeof(pData->szPlayerName) - 1] = '\0';

    assert(sizeof(pData->nValueArray) == sizeof(pStatistics->nValueArray));
    memcpy(pData->nValueArray, pStatistics->nValueArray, sizeof(pData->nValueArray));

Exit0:
    return;
}

void KPlayerClient::OnSyncBattleFieldObjective(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_BATTLEFIELD_OBJECTIVE* pObjective = (S2C_SYNC_BATTLEFIELD_OBJECTIVE*)pbyData;

    KG_PROCESS_ERROR(pObjective->byIndex < PQ_PARAM_COUNT);
    
    g_pSO3World->m_nBattleFieldObjectiveCurrent[pObjective->byIndex]    = pObjective->nCurrentValue;
    g_pSO3World->m_nBattleFieldObjectiveTarget[pObjective->byIndex]     = pObjective->nTargetValue;

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnUpdateBattleFieldObjective();

Exit0:
    return;
}

void KPlayerClient::OnSyncBattleFieldPQInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_BATTLEFIELD_PQ_INFO*   pPQInfo         = (S2C_SYNC_BATTLEFIELD_PQ_INFO*)pbyData;
    time_t                          nCurrentTime    = g_pSO3World->m_nBaseTime + g_pSO3World->m_nGameLoop / GAME_FPS;

    g_pSO3World->m_dwBattleFieldPQID            = pPQInfo->dwPQID;
    g_pSO3World->m_dwBattleFieldPQTemplateID    = pPQInfo->dwPQTemplateID;
    g_pSO3World->m_nBattleFieldPQBeginTime      = nCurrentTime - pPQInfo->nLapseTime;
    g_pSO3World->m_nBattleFieldPQEndTime        = pPQInfo->nLeftTime == -1 ? 0 : nCurrentTime + pPQInfo->nLeftTime;

Exit0:
    return;
}

void KPlayerClient::OnSyncNpcDropTarget(BYTE* pbyData, size_t nSize, int nFrame)
{
    KNpc*                     pNpc           = NULL;
    S2C_SYNC_NPC_DROP_TARGET* pNpcDropTarget = (S2C_SYNC_NPC_DROP_TARGET*)pbyData;

    KGLOG_PROCESS_ERROR(IS_NPC(pNpcDropTarget->dwNpcID));

    pNpc = g_pSO3World->m_NpcSet.GetObj(pNpcDropTarget->dwNpcID);
    KG_PROCESS_ERROR(pNpc);

    pNpc->m_dwDropTargetPlayerID = pNpcDropTarget->dwDropTargetID;

    if (g_pGameWorldUIHandler)
    {
        KUIEventNpcDropTargetUpdate param = {pNpcDropTarget->dwNpcID};
        g_pGameWorldUIHandler->OnNpcDropTargetUpdate(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnRemoteLuaCall(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                 bRetCode       = false;
    S2C_REMOTE_LUA_CALL* pLuaCall       = (S2C_REMOTE_LUA_CALL*)pbyData;
    size_t               uExtDataLen    = nSize - sizeof(S2C_REMOTE_LUA_CALL);

    g_pSO3World->m_ScriptClient.Accept(
        pLuaCall->szFunction, pLuaCall->byData, uExtDataLen
    );
}

void KPlayerClient::OnSyncPlayerReviveCtrl(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_PLAYER_REVIVE_CTRL*    pSyncReviveCtrl = (S2C_SYNC_PLAYER_REVIVE_CTRL*)pbyData;
    KPlayer*                        pPlayer         = NULL;

    if (g_pGameWorldUIHandler)
    {
        KUISyncPlayerRevive param;

        param.bReviveInSite     = pSyncReviveCtrl->bReviveInSitu;
        param.bReviveInAlter    = pSyncReviveCtrl->bReviveInAltar;
        param.bReviveByPlayer   = pSyncReviveCtrl->bReviveByPlayer;
        param.nLeftReviveFrame  = pSyncReviveCtrl->nReviveFrame;
        param.dwReviver         = pSyncReviveCtrl->dwReviver;
        param.nMessageID        = pSyncReviveCtrl->nMessageID;

        g_pGameWorldUIHandler->OnSyncPlayerRevive(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncAchievementData(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                                bRetCode    = false;
    S2C_SYNC_ACHIEVEMENT_DATA*          pPak        = (S2C_SYNC_ACHIEVEMENT_DATA*)pbyData;
    KPlayer*                            pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->m_Achievement.Load(pPak->byData, pPak->uDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        KUIEventSyncAchievementData param;

        param.dwPlayerID = pPak->dwPlayerID;

        g_pGameWorldUIHandler->OnSyncAchievementData(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnNewAchievementNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_NEW_ACHIEVEMENT_NOTIFY* pNotify = (S2C_NEW_ACHIEVEMENT_NOTIFY*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventNewAchievement param;

        param.nAchievementID = pNotify->nAchievementID;

        g_pGameWorldUIHandler->OnNewAchievement(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnAchievementAnnounceNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_ACHIEVEMENT_ANNOUNCE_NOTIFY* pNotify = (S2C_ACHIEVEMENT_ANNOUNCE_NOTIFY*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventAchievementAnnounce param;

        strncpy(param.szRoleName, pNotify->szRoleName, sizeof(param.szRoleName));
        param.szRoleName[sizeof(param.szRoleName) - 1] = '\0';

        param.byType            = pNotify->byType;
        param.nAchievementID    = pNotify->nAchievementID;

        g_pGameWorldUIHandler->OnAchievementAnnounce(param);
    }
}

void KPlayerClient::OnSyncAchievementPoint(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    S2C_SYNC_ACHIEVEMENT_POINT* pPak        = (S2C_SYNC_ACHIEVEMENT_POINT*)pbyData;
    KPlayer*                    pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->m_Achievement.AddPoint(pPak->nDeltaPoint);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateAchievementPoint(pPak->nDeltaPoint);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncAchievementCount(BYTE* pbyData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    S2C_SYNC_ACHIEVEMENT_COUNT* pPak        = (S2C_SYNC_ACHIEVEMENT_COUNT*)pbyData;
    KPlayer*                    pPlayer     = NULL;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->m_Achievement.AddCount(pPak->nID, pPak->nDeltaCount);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateAchievementCount();
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncDesignationData(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_DESIGNATION_DATA* pPak    = (S2C_SYNC_DESIGNATION_DATA*)pbyData;
    KPlayer*                   pPlayer = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_Designation.Load(pPak->byData, pPak->uDataLen);

    if (g_pGameWorldUIHandler)
    {
        KUIEventSyncDesignationData param;
        param.dwPlayerID = pPlayer->m_dwID;

        g_pGameWorldUIHandler->OnSyncDesignationData(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSetGenerationNotify(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SET_GENERATION* pPak    = (S2C_SET_GENERATION*)pbyData;
    KPlayer*            pPlayer = NULL;
 
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_Designation.m_nGenerationIndex  = pPak->byGenerationIndex;
    pPlayer->m_Designation.m_nBynameIndex = pPak->nNameInForceIndex;

    if (g_pGameWorldUIHandler)
    {
        KUIEventSetGeneration param;
        param.dwPlayerID        = pPak->dwPlayerID;
        param.nGenerationIndex  = (int)pPak->byGenerationIndex;
        param.nNameInForceIndex = pPak->nNameInForceIndex;

        g_pGameWorldUIHandler->OnSetGenerationNotify(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnAcquireDesignation(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_ACQUIRE_DESIGNATION* pPak     = (S2C_ACQUIRE_DESIGNATION*)pbyData;
    KPlayer*                 pPlayer  = NULL;
    BOOL                     bRetCode = false;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pPak->byPrefix != 0)
    {
        bRetCode = pPlayer->m_Designation.AcquirePrefix(pPak->byPrefix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pPak->byPostfix != 0)
    {
        bRetCode = pPlayer->m_Designation.AcquirePostfix(pPak->byPostfix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (g_pGameWorldUIHandler)
    {
        KUIEventAcquireDesignation param;
        param.nPrefix    = (int)pPak->byPrefix;
        param.nPostfix   = (int)pPak->byPostfix;

        g_pGameWorldUIHandler->OnAcquireDesignation(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnDesignationAnnounce(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_DESIGNATION_ANNOUNCE* pPak      = (S2C_DESIGNATION_ANNOUNCE*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventDesignationAnnounce param;
        param.nPrefix   = (int)pPak->byPrefix;
        param.nPostfix  = (int)pPak->byPostfix;
        param.byType    = pPak->byType;

        strncpy(param.szRoleName, pPak->szRoleName, sizeof(param.szRoleName));
        param.szRoleName[sizeof(param.szRoleName) - 1] = '\0';

        g_pGameWorldUIHandler->OnDesignationAnnounce(param);
    }
}

void KPlayerClient::OnRemoveDesignation(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_REMOVE_DESIGNATION* pPak     = (S2C_REMOVE_DESIGNATION*)pbyData;
    KPlayer*                pPlayer  = NULL;
    BOOL                    bRetCode = false;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pPak->byPrefix != 0)
    {
        bRetCode = pPlayer->m_Designation.RemovePrefix(pPak->byPrefix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pPak->byPostfix != 0)
    {
        bRetCode = pPlayer->m_Designation.RemovePostfix(pPak->byPostfix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (g_pGameWorldUIHandler)
    {
        KUIEventRemoveDesignation param;
        param.dwPlayerID = pPlayer->m_dwID;
        param.nPrefix    = (int)pPak->byPrefix;
        param.nPostfix   = (int)pPak->byPostfix;
        g_pGameWorldUIHandler->OnRemoveDesignation(param);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncPlayerDesignation(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_PLAYER_DESIGNATION*         pSyncDesignation   = (S2C_SYNC_PLAYER_DESIGNATION*)pbyData;
    KPlayer*                             pPlayer            = NULL;
    BOOL                                 bRetCode           = false;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSyncDesignation->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_Designation.m_nCurrentPrefix   = (int)pSyncDesignation->byPrefix;
    pPlayer->m_Designation.m_nCurrentPostfix  = (int)pSyncDesignation->byPostfix;
    pPlayer->m_Designation.m_nGenerationIndex = pSyncDesignation->byGenerationIndex;
    pPlayer->m_Designation.m_nBynameIndex     = pSyncDesignation->wByname;
    pPlayer->m_Designation.m_bDisplayFlag     = pSyncDesignation->bDisplayFlag;

    if (g_pGameWorldUIHandler)
    {
        KUIEventSetCurrentDesignationRespond param;

        param.dwPlayerID    = pSyncDesignation->dwPlayerID;
        param.nPrefix       = (int)pSyncDesignation->byPrefix;
        param.nPostfix      = (int)pSyncDesignation->byPostfix;
        param.bDisplayFlag  = (BOOL)pSyncDesignation->byGenerationIndex;

        g_pGameWorldUIHandler->OnSetCurrentDesignationRespond(param);
    }

Exit0:
     return;
}

void KPlayerClient::OnSyncMaxLevel(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_MAX_LEVEL* pPak        = (S2C_SYNC_MAX_LEVEL*)pbyData;
    KPlayer*            pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_nMaxLevel = pPak->nMaxLevel;
    
Exit0:
    return;  
}

void KPlayerClient::OnUseItemPrepare(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_USE_ITEM_PREPARE*   pUseItemPrepare = (S2C_USE_ITEM_PREPARE*)pbyData;
    KPlayer*                pPlayer         = NULL;
    KOT_ACTION_PARAM        Param;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pUseItemPrepare->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    Param.eActionType       = otActionUseItemPrepare;
    Param.nEndFrame         = g_pSO3World->m_nGameLoop + pUseItemPrepare->nFrames;
    Param.dwEndAniID        = pUseItemPrepare->dwEndAniID;

    pPlayer->DoOTAction(Param);

    // UI
	if (pPlayer->m_dwID == g_pSO3World->m_dwClientPlayerID)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventCustomOTActionProgress param;
            param.nType         = 0;
            param.nTotalFrame   = pUseItemPrepare->nFrames;

            strncpy(param.szActionName, pUseItemPrepare->szText, sizeof(param.szActionName));
            param.szActionName[CUSTOM_OTACTION_NAME_LEN - 1] = 0;
            
            g_pGameWorldUIHandler->OnCustomOTActionProgress(param);
        }
	}
    
    if (g_pGameWorldRepresentHandler && pUseItemPrepare->dwOTAniID)
    {
        g_pGameWorldRepresentHandler->OnCharacterPlayAnimation(pPlayer, pUseItemPrepare->dwOTAniID);
    }

Exit0:
    return;
}

void KPlayerClient::OnCreateViewPointRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_CREATE_VIEW_POINT_RESPOND*  pCreateViewPoint = (S2C_CREATE_VIEW_POINT_RESPOND*)pbyData;
    KPlayer*                        pPlayer          = NULL;
    KRegion*                        pRegion          = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);
    pPlayer->m_pScene->ValidateRegions(pCreateViewPoint->nRegionX, pCreateViewPoint->nRegionY);

    pRegion = pPlayer->m_pScene->GetRegion(pCreateViewPoint->nRegionX, pCreateViewPoint->nRegionY);
    KGLOG_PROCESS_ERROR(pRegion);
    
    pPlayer->m_pViewPointRegion = pRegion;

Exit0:
    return;
}

void KPlayerClient::OnRemoveViewPointRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_REMOVE_VIEW_POINT_RESPOND*  pRemoveViewPoint = (S2C_REMOVE_VIEW_POINT_RESPOND*)pbyData;
    KPlayer*                        pPlayer          = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_pViewPointRegion = NULL;

Exit0:
    return;
}

void KPlayerClient::OnMoveViewPointRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_MOVE_VIEW_POINT_RESPOND*    pMoveViewPoint  = (S2C_MOVE_VIEW_POINT_RESPOND*)pbyData;
    KPlayer*                        pPlayer         = NULL;
    KRegion*                        pRegion         = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_pScene->ValidateRegions(pMoveViewPoint->nRegionX, pMoveViewPoint->nRegionY);
    pRegion = pPlayer->m_pScene->GetRegion(pMoveViewPoint->nRegionX, pMoveViewPoint->nRegionY);
    KGLOG_PROCESS_ERROR(pRegion);

    pPlayer->m_pViewPointRegion = pRegion;

Exit0:
    return;
}

void KPlayerClient::OnAccountEndOfDayTime(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_ACCOUNT_END_OF_DAY_TIME* pNotify = (S2C_ACCOUNT_END_OF_DAY_TIME*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        if (pNotify->nLeftTime <= 0)
        {
            g_pGameWorldUIHandler->OnDisconnect();
        }
        else
        {
            g_pGameWorldUIHandler->OnAccountEndOfDayTime();
        }
    }
}

void KPlayerClient::OnSyncFreeLimitFlagInfo(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_FREE_LIMIT_FLAG_INFO*  pSyncChargeFlagInfo = (S2C_SYNC_FREE_LIMIT_FLAG_INFO*)pbyData;
    KPlayer*                        pPlayer             = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_bFreeLimitFlag = pSyncChargeFlagInfo->byFreeLimitFlag;

    if (pPlayer->m_bFreeLimitFlag)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventChargeLimitNotify Param;

            Param.nCode = cltLoginMessage;

            g_pGameWorldUIHandler->OnChargeLimitNotify(Param);
        }
    }

Exit0:
    return;
}

void KPlayerClient::OnApexProtocol(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_APEX_PROTOCOL* pApexProtocol = (S2C_APEX_PROTOCOL*)pbyData;

    g_pSO3World->m_ApexClient.OnRecvUserData(pApexProtocol->byData, pApexProtocol->nSize);
}

void KPlayerClient::OnGameCardSellRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_GAME_CARD_SELL_RESPOND* pRespond = (S2C_GAME_CARD_SELL_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventGameCardSellRespond Param;

        Param.byCode = pRespond->byCode;

        g_pGameWorldUIHandler->OnGameCardSellRespond(Param);
    }
}

void KPlayerClient::OnGameCardBuyRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_GAME_CARD_BUY_RESPOND* pRespond = (S2C_GAME_CARD_BUY_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventGameCardBuyRespond Param;

        Param.byCode = pRespond->byCode;

        g_pGameWorldUIHandler->OnGameCardBuyRespond(Param);
    }
}

void KPlayerClient::OnGameCardLookupRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_GAME_CARD_LOOKUP_RESPOND* pRespond = (S2C_GAME_CARD_LOOKUP_RESPOND*)pbyData;

    g_pSO3World->m_GameCardClient.UpdateList(pRespond->nTotalCount, pRespond->nCount, pRespond->CardList);

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnGameCardLookupRespond();
    }
}

void KPlayerClient::OnGameCardCancelRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_GAME_CARD_CANCEL_RESPOND* pRespond = (S2C_GAME_CARD_CANCEL_RESPOND*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventGameCardCancelRespond param;

        param.byCode = pRespond->byCode;

        g_pGameWorldUIHandler->OnGameCardCancelRespond(param);
    }
}

void KPlayerClient::OnSyncCoin(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_COIN*  pSyncPak    = (S2C_SYNC_COIN*)pbyData;
    KPlayer*        pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_nCoin = pSyncPak->nCoin;

Exit0:
    return;
}

void KPlayerClient::OnSeekMentorYell(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SEEK_MENTOR_YELL* pPak = (S2C_SEEK_MENTOR_YELL*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventSeekMentorYell param;

        strncpy(param.szPlayerName, pPak->szPlayerName, sizeof(param.szPlayerName));
        param.szPlayerName[sizeof(param.szPlayerName) - 1] = '\0';

        strncpy(param.szTongName, pPak->szTongName, sizeof(param.szTongName));
        param.szTongName[sizeof(param.szTongName) - 1] = '\0';

        param.byForceID   = pPak->byForceID;
        param.byRoleLevel = pPak->byRoleLevel;
        param.byRoleType  = pPak->byRoleType;

        g_pGameWorldUIHandler->OnSeekMentorYell(param);
    }
}

void KPlayerClient::OnSeekApprenticeYell(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SEEK_APPRENTICE_YELL* pPak = (S2C_SEEK_APPRENTICE_YELL*)pbyData;

    if (g_pGameWorldUIHandler)
    {
        KUIEventSeekApprenticeYell param;

        strncpy(param.szPlayerName, pPak->szPlayerName, sizeof(param.szPlayerName));
        param.szPlayerName[sizeof(param.szPlayerName) - 1] = '\0';

        strncpy(param.szTongName, pPak->szTongName, sizeof(param.szTongName));
        param.szTongName[sizeof(param.szTongName) - 1] = '\0';

        param.byForceID   = pPak->byForceID;
        param.byRoleLevel = pPak->byRoleLevel;
        param.byRoleType  = pPak->byRoleType;

        g_pGameWorldUIHandler->OnSeekApprenticeYell(param);
    }
}

void KPlayerClient::OnSyncMentorData(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_MENTOR_DATA* pPak = (S2C_SYNC_MENTOR_DATA*)pbyData;
    KMentorRecordBase     Info;
    int                   nMentorValueChange = 0;

    Info.nMentorValue = pPak->nValue;
    Info.byState      = pPak->byState;

    g_pSO3World->m_MentorClient.Update(pPak->dwMentor, pPak->dwApprentice, Info, &nMentorValueChange);

    if (nMentorValueChange > 0 && g_pGameWorldUIHandler)
    {
        KUIEventMentorValueChange param;

        param.nValueChange = nMentorValueChange;

        g_pGameWorldUIHandler->OnMentorValueChange(param);
    }
}

void KPlayerClient::OnDeleteMentorData(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_DELETE_MENTOR_DATA* pPak = (S2C_DELETE_MENTOR_DATA*)pbyData;

    g_pSO3World->m_MentorClient.DeleteRecord(pPak->dwMentor, pPak->dwApprentice);
}

void KPlayerClient::OnSyncPlayerCampFlag(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_SYNC_CAMP_FLAG* pPak = (S2C_SYNC_CAMP_FLAG*)pbyData;
    KPlayer* pPlayer = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    pPlayer->m_bCampFlag = pPak->byCampFlag;
    pPlayer->m_nCloseCampFlagTime = 0;
    
    if (g_pGameWorldUIHandler && g_pGameWorldRepresentHandler)
    {
        g_pGameWorldUIHandler->OnChangeCampFlag(pPlayer->m_dwID);
        g_pGameWorldUIHandler->OnUpdateAllRelation();
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();
    }

Exit0:
    return;
}

void KPlayerClient::OnCharacterThreatRanklistRespond(BYTE* pbyData, size_t nSize, int nFrame)
{
    S2C_CHARACTER_THREAT_RANKLIST* pRespond = (S2C_CHARACTER_THREAT_RANKLIST*)pbyData;

    if (g_pGameWorldUIHandler && g_pGameWorldRepresentHandler)
    {
        KUIThreatRankList RankList[MAX_THREAT_RANKLIST_COUNT];

        for (int i = 0; i < pRespond->wCount && i < MAX_THREAT_RANKLIST_COUNT; i++)
        {
            RankList[i].dwCharacterID = pRespond->RankList[i].dwCharacterID;
            RankList[i].nRank         = pRespond->RankList[i].wThreatRank;
        }

        g_pGameWorldUIHandler->OnCharacterThreatRanklist(pRespond->dwThreatListOwnerID, pRespond->wCount, RankList);
    }

Exit0:
    return;
}

//AutoCode:-处理协议函数结束-

#endif	//_CLIENT
