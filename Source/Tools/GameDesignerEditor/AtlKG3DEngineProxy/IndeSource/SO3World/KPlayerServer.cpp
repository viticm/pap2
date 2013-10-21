#include "stdafx.h"
#include "KPlayerServer.h"

#if defined(_SERVER)
#include "Engine/KG_CreateGUID.h"
#include "Common/CRC32.h"
#include "KRelayClient.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KShop.h"
#include "KMath.h"
#include "KRegion.h"
#include "KScene.h"
#include "Engine/KG_Time.h"
#include "KSkillRecipeList.h"
#include "KRecipeMaster.h"
#include "KCraft.h"
#include "KLootList.h"
#include "Engine/KTextFilter.h"
#include "KProductIdentity.h"

KPlayerServer g_PlayerServer;

#define MAX_ITEM_SYNC_PROTOCOL_SIZE	256


KPlayerServer::KPlayerServer(void)
{
    _Construct();

	memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
	memset(m_nProtocolSize, 0, sizeof(m_nProtocolSize));

	REGISTER_EXTERNAL_FUNC(c2s_handshake_request, &KPlayerServer::OnHandshakeRequest, sizeof(C2S_HANDSHAKE_REQUEST));
	REGISTER_EXTERNAL_FUNC(c2s_apply_enter_scene, &KPlayerServer::OnApplyEnterScene, sizeof(C2S_APPLY_ENTER_SCENE));
	REGISTER_EXTERNAL_FUNC(c2s_player_logout, &KPlayerServer::OnPlayerLogout, sizeof(C2S_PLAYER_LOGOUT));
    REGISTER_EXTERNAL_FUNC(c2s_ping_signal, &KPlayerServer::OnPingSignal, sizeof(C2S_PING_SIGNAL));

    REGISTER_EXTERNAL_FUNC(c2s_move_ctrl, &KPlayerServer::OnMoveCtrl, sizeof(C2S_MOVE_CTRL));
    REGISTER_EXTERNAL_FUNC(c2s_apply_move_adjust, &KPlayerServer::OnApplyMoveAdjust, sizeof(C2S_APPLY_MOVE_ADJUST));
    REGISTER_EXTERNAL_FUNC(c2s_character_stand, &KPlayerServer::OnCharacterStand, sizeof(C2S_CHARACTER_STAND));
	REGISTER_EXTERNAL_FUNC(c2s_character_jump, &KPlayerServer::OnCharacterJump, sizeof(C2S_CHARACTER_JUMP));
    REGISTER_EXTERNAL_FUNC(c2s_character_sit, &KPlayerServer::OnCharacterSit, sizeof(C2S_CHARACTER_SIT));
	REGISTER_EXTERNAL_FUNC(c2s_sync_direction, &KPlayerServer::OnSyncDirection, sizeof(C2S_SYNC_DIRECTION));

    REGISTER_EXTERNAL_FUNC(c2s_stop_current_action, &KPlayerServer::OnStopCurrentAction, sizeof(C2S_STOP_CURRENT_ACTION));
	REGISTER_EXTERNAL_FUNC(c2s_character_skill, &KPlayerServer::OnCharacterSkill, sizeof(C2S_CHARACTER_SKILL));
	REGISTER_EXTERNAL_FUNC(c2s_character_action, &KPlayerServer::OnCharacterAction, sizeof(C2S_CHARACTER_ACTION));

	REGISTER_EXTERNAL_FUNC(c2s_apply_player_display_data, &KPlayerServer::OnApplyPlayerDisplayData, sizeof(C2S_APPLY_PLAYER_DISPLAY_DATA));

	REGISTER_EXTERNAL_FUNC(c2s_gm_command, &KPlayerServer::OnGmCommand, UNDEFINED_PROTOCOL_SIZE);

	REGISTER_EXTERNAL_FUNC(c2s_accept_quest, &KPlayerServer::OnAcceptQuest, sizeof(C2S_ACCEPT_QUEST));
	REGISTER_EXTERNAL_FUNC(c2s_finish_quest, &KPlayerServer::OnFinishQuest, sizeof(C2S_FINISH_QUEST));
	REGISTER_EXTERNAL_FUNC(c2s_cancel_quest, &KPlayerServer::OnCancelQuest, sizeof(C2S_CANCEL_QUEST));
	REGISTER_EXTERNAL_FUNC(c2s_share_quest, &KPlayerServer::OnShareQuest, sizeof(C2S_SHARE_QUEST));
	REGISTER_EXTERNAL_FUNC(c2s_accept_escort_quest, &KPlayerServer::OnAcceptEscortQuest, sizeof(C2S_ACCEPT_ESCORT_QUEST));

    REGISTER_EXTERNAL_FUNC(c2s_open_shop_request, &KPlayerServer::OnOpenShopRequest, sizeof(C2S_OPEN_SHOP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_query_shop_items, &KPlayerServer::OnQueryShopItems, sizeof(C2S_QUERY_SHOP_ITEMS));
    REGISTER_EXTERNAL_FUNC(c2s_shop_buy_request, &KPlayerServer::OnShopBuyRequest, sizeof(C2S_SHOP_BUY_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_shop_sell_request, &KPlayerServer::OnShopSellRequest, sizeof(C2S_SHOP_SELL_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_sold_list_buy_request, &KPlayerServer::OnSoldListBuyRequest, sizeof(C2S_SOLD_LIST_BUY_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_shop_repair_request, &KPlayerServer::OnShopRepairRequest, sizeof(C2S_SHOP_REPAIR_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_trading_invite_request, &KPlayerServer::OnTradingInviteRequest, sizeof(C2S_TRADING_INVITE_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_trading_invite_respond, &KPlayerServer::OnTradingInviteRespond, sizeof(C2S_TRADING_INVITE_RESPOND));
    REGISTER_EXTERNAL_FUNC(c2s_trading_add_item, &KPlayerServer::OnTradingAddItem, sizeof(C2S_TRADING_ADD_ITEM));
    REGISTER_EXTERNAL_FUNC(c2s_trading_delete_item, &KPlayerServer::OnTradingDeleteItem, sizeof(C2S_TRADING_DELETE_ITEM));
    REGISTER_EXTERNAL_FUNC(c2s_trading_set_money, &KPlayerServer::OnTradingSetMoney, sizeof(C2S_TRADING_SET_MONEY));
    REGISTER_EXTERNAL_FUNC(c2s_trading_confirm, &KPlayerServer::OnTradingConfirm, sizeof(C2S_TRADING_CONFIRM));

    REGISTER_EXTERNAL_FUNC(c2s_mount_kungfu, &KPlayerServer::OnMountKungfu, sizeof(C2S_MOUNT_KUNGFU));    
    REGISTER_EXTERNAL_FUNC(c2s_cancel_buff, &KPlayerServer::OnCancelBuff, sizeof(C2S_CANCEL_BUFF));
    REGISTER_EXTERNAL_FUNC(c2s_set_sheath_state, &KPlayerServer::OnSetSheathState, sizeof(C2S_SET_SHEATH_STATE));
    REGISTER_EXTERNAL_FUNC(c2s_set_hide_hat_flag, &KPlayerServer::OnSetHideHatFlag, sizeof(C2S_SET_HIDE_HAT_FLAG));
    
    REGISTER_EXTERNAL_FUNC(c2s_apply_player_bufflist, &KPlayerServer::OnApplyPlayerBuffList, sizeof(C2S_APPLY_PLAYER_BUFFLIST));

    REGISTER_EXTERNAL_FUNC(c2s_invite_join_team_request, &KPlayerServer::OnInviteJoinTeamRequest, sizeof(C2S_INVITE_JOIN_TEAM_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_invite_join_team_respond, &KPlayerServer::OnInviteJoinTeamRespond, sizeof(C2S_INVITE_JOIN_TEAM_RESPOND));
    REGISTER_EXTERNAL_FUNC(c2s_apply_join_team_request, &KPlayerServer::OnApplyJoinTeamRequest, sizeof(C2S_APPLY_JOIN_TEAM_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_apply_join_team_respond, &KPlayerServer::OnApplyJoinTeamRespond, sizeof(C2S_APPLY_JOIN_TEAM_RESPOND));
    REGISTER_EXTERNAL_FUNC(c2s_team_del_member_request, &KPlayerServer::OnTeamDelMemberRequest, sizeof(C2S_TEAM_DEL_MEMBER_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_change_authority_request, &KPlayerServer::OnTeamChangeAuthorityRequest, sizeof(C2S_TEAM_CHANGE_AUTHORITY_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_set_distribute_mode_request, &KPlayerServer::OnTeamSetLootMode, sizeof(C2S_TEAM_SET_LOOT_MODE_REQUEST));
	REGISTER_EXTERNAL_FUNC(c2s_team_set_roll_quality_request, &KPlayerServer::OnTeamSetRollQuality, sizeof(C2S_TEAM_SET_ROLL_QUALITY_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_set_formation_leader_request, &KPlayerServer::OnTeamSetFormationLeader, sizeof(C2S_TEAM_SET_FORMATION_LEADER_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_member_notify_signpost_request, &KPlayerServer::OnTeamMemberNotifySignpost, sizeof(C2S_TEAM_MEMBER_NOTIFY_SIGNPOST_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_set_mark_request, &KPlayerServer::OnTeamSetMarkRequest, sizeof(C2S_TEAM_SET_MARK_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_level_up_raid_request, &KPlayerServer::OnTeamLevelUpRaidRequest, sizeof(C2S_TEAM_LEVEL_UP_RAID_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_team_change_member_group_request, &KPlayerServer::OnTeamChangeMemberGroupRequest, sizeof(C2S_TEAM_CHANGE_MEMBER_GROUP_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_request_target, &KPlayerServer::OnRequestTarget, sizeof(C2S_REQUEST_TARGET));
	REGISTER_EXTERNAL_FUNC(c2s_apply_exchange_item, &KPlayerServer::OnApplyExchangeItem, sizeof(C2S_APPLY_EXCHANGE_ITEM));
	REGISTER_EXTERNAL_FUNC(c2s_window_select, &KPlayerServer::OnWindowSelect, sizeof(C2S_WINDOW_SELECT));
	REGISTER_EXTERNAL_FUNC(c2s_cast_profession_skill, &KPlayerServer::OnCastProfessionSkill, sizeof(C2S_CAST_PROFESSION_SKILL));
	REGISTER_EXTERNAL_FUNC(c2s_loot_item, &KPlayerServer::OnLootItem, sizeof(C2S_LOOT_ITEM));
    REGISTER_EXTERNAL_FUNC(c2s_roll_item, &KPlayerServer::OnRollItem, sizeof(C2S_ROLL_ITEM));
	REGISTER_EXTERNAL_FUNC(c2s_apply_loot_list, &KPlayerServer::OnApplyLootList, sizeof(C2S_APPLY_LOOT_LIST));
	REGISTER_EXTERNAL_FUNC(c2s_character_open, &KPlayerServer::OnCharacterOpen, sizeof(C2S_CHARACTER_OPEN));
    REGISTER_EXTERNAL_FUNC(c2s_death_respond, &KPlayerServer::OnDeathRespond, sizeof(C2S_DEATH_RESPOND));
	REGISTER_EXTERNAL_FUNC(c2s_apply_use_item, &KPlayerServer::OnApplyUseItem, sizeof(C2S_APPLY_USE_ITEM));
	REGISTER_EXTERNAL_FUNC(c2s_apply_fight_flag, &KPlayerServer::OnApplyFightFlag, sizeof(C2S_APPLY_FIGHT_FLAG));
	REGISTER_EXTERNAL_FUNC(c2s_finish_loot, &KPlayerServer::OnFinishLoot, sizeof(C2S_FINISH_LOOT));
	REGISTER_EXTERNAL_FUNC(c2s_loot_money, &KPlayerServer::OnLootMoney, sizeof(C2S_LOOT_MONEY));
	REGISTER_EXTERNAL_FUNC(c2s_distribute_item, &KPlayerServer::OnDistributeItem, sizeof(C2S_DISTRIBUTE_ITEM));
    REGISTER_EXTERNAL_FUNC(c2s_talk_message, &KPlayerServer::OnTalkMessage, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(c2s_get_fellowship_list_request, &KPlayerServer::OnGetFellowshipListRequest, sizeof(C2S_GET_FELLOWSHIP_LIST_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_add_fellowship_request, &KPlayerServer::OnAddFellowshipRequest, sizeof(C2S_ADD_FELLOWSHIP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_del_fellowship_request, &KPlayerServer::OnDelFellowshipRequest, sizeof(C2S_DEL_FELLOWSHIP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_add_fellowship_group_request, &KPlayerServer::OnAddFellowshipGroupRequest, sizeof(C2S_ADD_FELLOWSHIP_GROUP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_del_fellowship_group_request, &KPlayerServer::OnDelFellowshipGroupRequest, sizeof(C2S_DEL_FELLOWSHIP_GROUP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_rename_fellowship_group_request, &KPlayerServer::OnRenameFellowshipGroupRequest, sizeof(C2S_RENAME_FELLOWSHIP_GROUP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_set_fellowship_remark_request, &KPlayerServer::OnSetFellowshipRemarkRequest, sizeof(C2S_SET_FELLOWSHIP_REMARK));
    REGISTER_EXTERNAL_FUNC(c2s_set_fellowship_group_request, &KPlayerServer::OnSetFellowshipGroupRequest, sizeof(C2S_SET_FELLOWSHIP_GROUP));

    REGISTER_EXTERNAL_FUNC(c2s_send_mail_request, &KPlayerServer::OnSendMailRequest, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(c2s_get_maillist_request, &KPlayerServer::OnGetMaillistRequest, sizeof(C2S_GET_MAILLIST_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_get_mail_content, &KPlayerServer::OnGetMailContent, sizeof(C2S_GET_MAIL_CONTENT));
    REGISTER_EXTERNAL_FUNC(c2s_get_mail_attachment, &KPlayerServer::OnGetMailAttachment, sizeof(C2S_GET_MAIL_ATTACHMENT));
    REGISTER_EXTERNAL_FUNC(c2s_set_mail_read, &KPlayerServer::OnSetMailRead, sizeof(C2S_SET_MAIL_READ));
    REGISTER_EXTERNAL_FUNC(c2s_delete_mail, &KPlayerServer::OnDeleteMail, sizeof(C2S_DELETE_MAIL));
    REGISTER_EXTERNAL_FUNC(c2s_return_mail, &KPlayerServer::OnReturnMail, sizeof(C2S_RETURN_MAIL));

	REGISTER_EXTERNAL_FUNC(c2s_apply_pk_operate, &KPlayerServer::OnApplyPKOperate, sizeof(C2S_APPLY_PK_OPERATE));
	REGISTER_EXTERNAL_FUNC(c2s_sync_user_preferences, &KPlayerServer::OnSyncUserPreferences, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(c2s_set_bank_opened_flag, &KPlayerServer::OnSetBankOpenedFlag, sizeof(C2S_SET_BANK_OPENED_FLAG));
	REGISTER_EXTERNAL_FUNC(c2s_enable_bank_package, &KPlayerServer::OnEnableBankPackage, sizeof(C2S_ENABLE_BANK_PACKAGE));

	REGISTER_EXTERNAL_FUNC(c2s_sync_mid_map_mark, &KPlayerServer::OnSyncMidMapMark, sizeof(C2S_SYNC_MID_MAP_MARK));
    
    REGISTER_EXTERNAL_FUNC(c2s_player_learn_skill, &KPlayerServer::OnPlayerLearnSkill, sizeof(C2S_PLAYER_LEARN_SKILL));
	REGISTER_EXTERNAL_FUNC(c2s_player_learn_recipe, &KPlayerServer::OnPlayerLearnRecipe, sizeof(C2S_PLAYER_LEARN_RECIPE));

    REGISTER_EXTERNAL_FUNC(c2s_player_active_skill_recipe, &KPlayerServer::OnPlayerActiveSkillRecipe, sizeof(C2S_PLAYER_ACTIVE_SKILL_RECIPE));

    REGISTER_EXTERNAL_FUNC(c2s_peek_other_player_equip, &KPlayerServer::OnPeekOtherPlayerEquip, sizeof(C2S_PEEK_OTHER_PLAYER_EQUIP));
    REGISTER_EXTERNAL_FUNC(c2s_peek_other_player_book, &KPlayerServer::OnPeekOtherPlayerBook, sizeof(C2S_PEEK_OTHER_PLAYER_BOOK));
    REGISTER_EXTERNAL_FUNC(c2s_peek_other_player_quest, &KPlayerServer::OnPeekOtherPlayerQuest, sizeof(C2S_PEEK_OTHER_PLAYER_QUEST));
    
    REGISTER_EXTERNAL_FUNC(c2s_road_track_start_out, &KPlayerServer::OnRoadTrackStartOut, sizeof(C2S_ROAD_TRACK_START_OUT));
    REGISTER_EXTERNAL_FUNC(c2s_road_track_force_request, &KPlayerServer::OnRoadTrackForceRequest, sizeof(C2S_ROAD_TRACK_FORCE_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_open_venation, &KPlayerServer::OnOpenVenation, sizeof(C2S_OPEN_VENATION));
    REGISTER_EXTERNAL_FUNC(c2s_send_gm_message, &KPlayerServer::OnSendGmMessage, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(c2s_join_battle_field_queue_request, &KPlayerServer::OnJoinBattleFieldQueueRequest, sizeof(C2S_JOIN_BATTLE_FIELD_QUEUE_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_leave_battle_field_queue_request, &KPlayerServer::OnLeaveBattleFieldQueueRequest, sizeof(C2S_LEAVE_BATTLE_FIELD_QUEUE_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_accept_join_battle_field, &KPlayerServer::OnAcceptJoinBattleField, sizeof(C2S_ACCEPT_JOIN_BATTLE_FIELD));
    REGISTER_EXTERNAL_FUNC(c2s_get_battle_field_list, &KPlayerServer::OnGetBattleFieldList, sizeof(C2S_GET_BATTLE_FIELD_LIST));
    REGISTER_EXTERNAL_FUNC(c2s_leave_battle_field, &KPlayerServer::OnLeaveBattleField, sizeof(C2S_LEAVE_BATTLE_FIELD));

    REGISTER_EXTERNAL_FUNC(c2s_update_tong_roster_request, &KPlayerServer::OnApplyTongRosterRequest, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(c2s_apply_tong_info_request, &KPlayerServer::OnApplyTongInfoRequest, sizeof(C2S_APPLY_TONG_INFO_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_invite_player_join_tong_request, &KPlayerServer::OnInvitePlayerJoinTongRequest, sizeof(C2S_INVITE_PLAYER_JOIN_TONG_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_invite_player_join_tong_respond, &KPlayerServer::OnInvitePlayerJoinTongRespond, sizeof(C2S_INVITE_PLAYER_JOIN_TONG_RESPOND));
    REGISTER_EXTERNAL_FUNC(c2s_apply_kick_out_tong_member_request, &KPlayerServer::OnApplyKickOutTongMemberRequest, sizeof(C2S_APPLY_KICK_OUT_TONG_MEMBER_REQUEST));   
    REGISTER_EXTERNAL_FUNC(c2s_modify_tong_info_request, &KPlayerServer::OnModifyTongInfoRequest, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(c2s_modify_tong_scheam_request, &KPlayerServer::OnModifyTongScheamRequest, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(c2s_apply_quite_tong_request, &KPlayerServer::OnApplyQuiteTongRequest, sizeof(C2S_APPLY_QUITE_TONG_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_change_tong_member_group_request, &KPlayerServer::OnChangeTongMemberGroupRequest, sizeof(C2S_CHANGE_TONG_MEMBER_GROUP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_change_tong_master_request, &KPlayerServer::OnChangeTongMasterRequest, sizeof(C2S_CHANGE_TONG_MASTER_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_change_tong_member_remark_request, &KPlayerServer::OnChangeTongMemberRemarkRequest, sizeof(C2S_CHANGE_TONG_MEMBER_REMARK_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_get_tong_description_request, &KPlayerServer::OnGetTongDescriptionRequest, sizeof(C2S_GET_TONG_DESCRIPTION_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_apply_battlefield_statistics, &KPlayerServer::OnApplyBattleFieldStatistics, sizeof(C2S_APPLY_BATTLEFIELD_STATISTICS));
	REGISTER_EXTERNAL_FUNC(c2s_set_hero_flag, &KPlayerServer::OnSetHeroFlag, sizeof(C2S_SET_HERO_FLAG));

    REGISTER_EXTERNAL_FUNC(c2s_auction_lookup_request, &KPlayerServer::OnAuctionLookupRequest, sizeof(C2S_AUCTION_LOOKUP_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_auction_bid_request, &KPlayerServer::OnAuctionBidRequest, sizeof(C2S_AUCTION_BID_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_auction_sell_request, &KPlayerServer::OnAuctionSellRequest, sizeof(C2S_AUCTION_SELL_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_auction_cancel_request, &KPlayerServer::OnAuctionCancelRequest, sizeof(C2S_AUCTION_CANCEL_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_apply_set_camp_request, &KPlayerServer::OnApplySetCampRequest, sizeof(C2S_APPLY_SET_CAMP_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_remote_lua_call, &KPlayerServer::OnRemoteLuaCall, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(c2s_player_revive_request, &KPlayerServer::OnPlayerReviveRequest, sizeof(C2S_PLAYER_REVIVE_REQUEST));

    REGISTER_EXTERNAL_FUNC(c2s_apply_achievement_data_request, &KPlayerServer::OnApplyAchievementDataRequest, sizeof(C2S_APPLY_ACHIEVEMENT_DATA_REQUEST));
    REGISTER_EXTERNAL_FUNC(c2s_query_product_identity, &KPlayerServer::OnQueryProductIdentity, sizeof(C2S_QUERY_PRODUCT_IDENTITY));
    //AutoCode:注册协议
}

KPlayerServer::~KPlayerServer(void)
{
    _Destroy();
}


// 返回应答信息
BOOL KPlayerServer::DoMessageNotify(int nConnIndex, int nType, int nCode, const void* pvData, size_t uDataLen)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	S2C_MESSAGE_NOTIFY*     pMsg            = (S2C_MESSAGE_NOTIFY*)m_byTempData;
    size_t                  uPakSize        = sizeof(S2C_MESSAGE_NOTIFY) + uDataLen;

    KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

    pMsg->byProtocolID    = s2c_message_notify;
    pMsg->wSize           = (WORD)uPakSize;
	pMsg->byType          = (BYTE)nType;
	pMsg->byCode          = (BYTE)nCode;

    if (uDataLen > 0)
    {
        memcpy(pMsg->byData, pvData, uDataLen);
    }

    bRetCode = Send(nConnIndex, pMsg, uPakSize);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

// 同步玩家数据
BOOL KPlayerServer::DoSyncPlayerBaseInfo(int nConnIndex, KPlayer* pPlayer)
{
    BOOL                        bResult  = false;
	BOOL                        bRetCode = false;
	S2C_SYNC_PLAYER_BASE_INFO   SyncPlayerBaseInfo;

	SyncPlayerBaseInfo.byProtocolID     = s2c_sync_player_base_info;
	SyncPlayerBaseInfo.dwPlayerID       = pPlayer->m_dwID;
    SyncPlayerBaseInfo.byRoleType       = (BYTE)pPlayer->m_eRoleType;
	SyncPlayerBaseInfo.dwMapID          = pPlayer->m_SavePosition.dwMapID;
	SyncPlayerBaseInfo.nMapCopyIndex    = pPlayer->m_SavePosition.nMapCopyIndex;
	SyncPlayerBaseInfo.nX               = pPlayer->m_SavePosition.nX;
	SyncPlayerBaseInfo.nY               = pPlayer->m_SavePosition.nY;
	SyncPlayerBaseInfo.nZ               = pPlayer->m_SavePosition.nZ;
    SyncPlayerBaseInfo.byFaceDirection  = (BYTE)pPlayer->m_SavePosition.nFaceDirection;
	SyncPlayerBaseInfo.byLevel          = (BYTE)pPlayer->m_nLevel;
    SyncPlayerBaseInfo.byCamp           = (BYTE)pPlayer->m_eCamp;
    SyncPlayerBaseInfo.byForceID        = (BYTE)pPlayer->m_dwForceID;
    SyncPlayerBaseInfo.nFrame           = g_pSO3World->m_nGameLoop;
    SyncPlayerBaseInfo.nBaseTime        = g_pSO3World->m_nBaseTime;
    SyncPlayerBaseInfo.dwTongID         = pPlayer->m_dwTongID;
    SyncPlayerBaseInfo.nBattleFieldSide = pPlayer->m_nBattleFieldSide;

    strncpy(SyncPlayerBaseInfo.szRoleName, pPlayer->m_szName, _NAME_LEN);
    SyncPlayerBaseInfo.szRoleName[_NAME_LEN - 1] = 0;
    
    memcpy(SyncPlayerBaseInfo.wRepresentId, pPlayer->m_wRepresentId, sizeof(SyncPlayerBaseInfo.wRepresentId));
    
	bRetCode = Send(nConnIndex, &SyncPlayerBaseInfo, sizeof(SyncPlayerBaseInfo));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncQuestData(
    int nConnIndex, DWORD dwDestPlayerID, BYTE* pbyData, size_t uDataLen, BYTE byDataType
)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
	S2C_SYNC_QUEST_DATA*    pSyncQuestList  = (S2C_SYNC_QUEST_DATA*)m_byTempData;

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_QUEST_DATA) + uDataLen <= MAX_EXTERNAL_PACKAGE_SIZE);

	pSyncQuestList->byProtocolID    = s2c_sync_quest_data;
    pSyncQuestList->wSize           = (WORD)(sizeof(S2C_SYNC_QUEST_DATA) + uDataLen);
    pSyncQuestList->dwDestPlayerID  = dwDestPlayerID;
    pSyncQuestList->byQuestDataType = byDataType;

	memcpy(pSyncQuestList->byData, pbyData, uDataLen);

	bRetCode = Send(nConnIndex, pSyncQuestList, pSyncQuestList->wSize);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAccountKickout(int nConnIndex)
{
    BOOL                        bResult       = false;
    BOOL                        bRetCode      = false;
	S2C_ACCOUNT_KICKOUT         Notify;

	Notify.byProtocolID = s2c_account_kickout;

	bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

// 通知客户端切换服务器
BOOL KPlayerServer::DoSwitchGS(int nConnIndex, DWORD dwPlayerID, GUID& Guid, DWORD dwIPAddr, WORD wPort)
{
    BOOL                    bResult             = false;
	BOOL                    bRetCode            = false;
	S2C_SWITCH_GS           Notify;

	Notify.byProtocolID     = s2c_switch_gs;
    Notify.dwPlayerID       = dwPlayerID;
	Notify.Guid             = Guid;
	Notify.dwIPAddr         = dwIPAddr;
	Notify.wPort            = wPort;

	bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSwitchMap(KPlayer* pPlayer, DWORD dwMapID, int nCopyIndex, int nX, int nY, int nZ)
{
    BOOL                    bResult             = false;
	BOOL                    bRetCode            = false;
	S2C_SWITCH_MAP          Notify;

	Notify.byProtocolID     = s2c_switch_map;
    Notify.dwMapID          = dwMapID;
	Notify.nCopyIndex       = nCopyIndex;
	Notify.nX               = nX;
    Notify.nY               = nY;
    Notify.nZ               = nZ;
    Notify.nBattleFieldSide = pPlayer->m_nBattleFieldSide;

	bRetCode = Send(pPlayer->m_nConnIndex, &Notify, sizeof(Notify));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}	

BOOL KPlayerServer::DoSyncNewPlayer(int nConnIndex, KPlayer* pPlayer)
{
    BOOL                  bResult         = false;
	BOOL                  bRetCode        = false;
    PK_STATE              ePKState        = pPlayer->m_PK.GetState();
	S2C_SYNC_NEW_PLAYER   Pak;

	assert(pPlayer->m_nX >= 0 && pPlayer->m_nX <= MAX_X_COORDINATE);
	assert(pPlayer->m_nY >= 0 && pPlayer->m_nY <= MAX_Y_COORDINATE);
    assert(pPlayer->m_nDirectionXY >= 0 && pPlayer->m_nDirectionXY <= DIRECTION_COUNT);
    assert(pPlayer->m_nConvergenceSpeed >= 0 && pPlayer->m_nConvergenceSpeed <= MAX_VELOCITY_XY);
    assert(pPlayer->m_nDestX >= 0 && pPlayer->m_nDestX <= MAX_X_COORDINATE);
    assert(pPlayer->m_nDestY >= 0 && pPlayer->m_nDestY <= MAX_Y_COORDINATE);
    assert(pPlayer->m_nCurrentGravity >= 0 && pPlayer->m_nCurrentGravity <= MAX_GRAVITY);
    assert(pPlayer->m_nVelocityXY >= 0 && pPlayer->m_nVelocityXY <= MAX_ZOOM_VELOCITY);
    assert(pPlayer->m_nCurrentTrack >= 0 && pPlayer->m_nCurrentTrack <= MAX_TRACK_ID);
    
	Pak.byProtocolID         = s2c_sync_new_player;
	Pak.dwID                 = pPlayer->m_dwID;
    Pak.wRepresentState      = (WORD)pPlayer->m_dwRepresentState;
    Pak.byFaceDirection      = (BYTE)pPlayer->m_nFaceDirection;
    Pak.byPitchDirection     = (BYTE)pPlayer->m_nPitchDirection;
    Pak.byMoveFrameCount     = (BYTE)pPlayer->m_nMoveFrameCounter;
    Pak.byLifePercent        = (BYTE)pPlayer->m_nLastBroadcastLife;

    Pak.uMoveState           = pPlayer->m_eMoveState;
    Pak.uPosX                = pPlayer->m_nX;
    Pak.uPosY                = pPlayer->m_nY;
    Pak.uPosZ                = pPlayer->m_nZ;
    Pak.uFightState          = pPlayer->m_bFightState;
    Pak.uSheathFlag          = pPlayer->m_bSheathFlag;
    Pak.uPK                  = (ePKState == pksSlaying || ePKState == pksExitSlay);

    Pak.uConvergenceSpeed    = pPlayer->m_nConvergenceSpeed;
    Pak.uDirectionXY         = pPlayer->m_nDirectionXY;
    Pak.nVelocityZ           = pPlayer->m_nVelocityZ;
    Pak.uDestX               = pPlayer->m_nDestX;
    Pak.uDestY               = pPlayer->m_nDestY;
    Pak.uOnHorse             = pPlayer->m_bOnHorse;
    Pak.uTryPathState        = pPlayer->m_bTryPathState;
    Pak.uTryPathSide         = pPlayer->m_nTryPathSide;

    Pak.uCurrentGravity      = pPlayer->m_nCurrentGravity;
    Pak.uVelocityXY          = pPlayer->m_nVelocityXY;
    Pak.uRedName             = pPlayer->m_bRedName;
    Pak.uRunSpeed            = pPlayer->m_nRunSpeed;
    Pak.uMoveCtrlDisable     = pPlayer->m_MoveCtrl.bDisable;
    Pak.uMoveCtrlMoving      = pPlayer->m_MoveCtrl.bMove;
    Pak.uMoveCtrlWalk        = pPlayer->m_MoveCtrl.bWalk;
    Pak.uMoveCtrlDirection8  = pPlayer->m_MoveCtrl.nDirection8;
    Pak.nMoveCtrlTurn        = pPlayer->m_MoveCtrl.nTurn;

    Pak.uSlipFlag            = pPlayer->m_bSlip;
    Pak.uWaterFlyAbility     = (pPlayer->m_nWaterFlyAbility > 0);

    // AutoMove模式下,一些参数具有特殊含义,被重新解释
    if (pPlayer->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = g_pSO3World->m_RoadManager.GetTrackInfo(pPlayer->m_nCurrentTrack);

        KGLOG_PROCESS_ERROR(pTrack);

        Pak.uPosX   = pPlayer->m_nCurrentTrack;
        Pak.uPosY   = (pPlayer->m_nFromNode == pTrack->nToNode);
        Pak.uPosZ   = pPlayer->m_nMoveFrameCounter;
    }

	bRetCode = Send(nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncNewNpc(int nConnIndex, KNpc* pNpc)
{
    BOOL                  bResult             = false;
	BOOL                  bRetCode            = false;
	S2C_SYNC_NEW_NPC      Pak;

	assert(pNpc->m_nX >= 0 && pNpc->m_nX <= MAX_X_COORDINATE);
	assert(pNpc->m_nY >= 0 && pNpc->m_nY <= MAX_Y_COORDINATE);
    assert(pNpc->m_nDirectionXY >= 0 && pNpc->m_nDirectionXY <= DIRECTION_COUNT);
    assert(pNpc->m_nConvergenceSpeed >= 0 && pNpc->m_nConvergenceSpeed <= MAX_VELOCITY_XY);
    assert(pNpc->m_nDestX >= 0 && pNpc->m_nDestX <= MAX_X_COORDINATE);
    assert(pNpc->m_nDestY >= 0 && pNpc->m_nDestY <= MAX_Y_COORDINATE);
    assert(pNpc->m_nCurrentGravity >= 0 && pNpc->m_nCurrentGravity <= MAX_GRAVITY);
    assert(pNpc->m_nVelocityXY >= 0 && pNpc->m_nVelocityXY <= MAX_ZOOM_VELOCITY);
    assert(pNpc->m_nCurrentTrack >= 0 && pNpc->m_nCurrentTrack <= MAX_TRACK_ID);

	Pak.byProtocolID         = s2c_sync_new_npc;
	Pak.dwID                 = pNpc->m_dwID;
    Pak.dwTemplateID         = pNpc->m_dwTemplateID;
    Pak.wRepresentState      = (WORD)pNpc->m_dwRepresentState;
    Pak.byFaceDirection      = (BYTE)pNpc->m_nFaceDirection;
    Pak.byMoveFrameCount     = (BYTE)pNpc->m_nMoveFrameCounter;
    Pak.byLevel              = (BYTE)pNpc->m_nLevel;
    Pak.wModelID             = (WORD)pNpc->m_dwModelID;
    Pak.byForceID            = (BYTE)pNpc->m_dwForceID;
    Pak.byLifePercent        = (BYTE)pNpc->m_nLastBroadcastLife;

    Pak.uMoveState           = pNpc->m_eMoveState;
    Pak.uPosX                = pNpc->m_nX;
    Pak.uPosY                = pNpc->m_nY;
    Pak.uPosZ                = pNpc->m_nZ;
    Pak.uFightState          = pNpc->m_bFightState;
    Pak.uSheathFlag          = pNpc->m_bSheathFlag;
    Pak.uTryPathState        = pNpc->m_bTryPathState;

    Pak.uVelocityXY          = pNpc->m_nVelocityXY;
    Pak.uConvergenceSpeed    = pNpc->m_nConvergenceSpeed;
    Pak.nVelocityZ           = pNpc->m_nVelocityZ;
    Pak.uDestX               = pNpc->m_nDestX;
    Pak.uDestY               = pNpc->m_nDestY;

    Pak.uDirectionXY         = pNpc->m_nDirectionXY;
    Pak.uTryPathSide         = pNpc->m_nTryPathSide;
    Pak.uCurrentGravity      = pNpc->m_nCurrentGravity;
    Pak.uSlipFlag            = pNpc->m_bSlip;
    Pak.uDialogFlag          = pNpc->m_bDialogFlag;

    if (pNpc->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack  = g_pSO3World->m_RoadManager.GetTrackInfo(pNpc->m_nCurrentTrack);

        KGLOG_PROCESS_ERROR(pTrack);

        Pak.uPosX = pNpc->m_nCurrentTrack;
        Pak.uPosY = (pNpc->m_nFromNode == pTrack->nToNode);
        Pak.uPosZ = pNpc->m_nMoveFrameCounter;
    }

	bRetCode = Send(nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncNewDoodad(int nConnIndex, KDoodad* pDoodad)
{
    BOOL                  bResult     = false;
    BOOL                  bRetCode    = false;
	S2C_SYNC_NEW_DOODAD   Pak;

	assert(pDoodad);
    assert(pDoodad->m_pScene);

    Pak.byProtocolID       = s2c_sync_new_doodad;
	Pak.dwID               = pDoodad->m_dwID;
    Pak.dwTemplateID       = pDoodad->m_dwTemplateID;
    Pak.byFaceDirection    = (BYTE)pDoodad->m_nFaceDirection;
	Pak.cKind              = (char)pDoodad->m_eKind;
	Pak.cState             = (char)pDoodad->m_eState;
	Pak.dwNpcTemplateID    = pDoodad->m_dwNpcTemplateID;
	Pak.dwRepresentID	   = pDoodad->m_dwRepresentID;
	Pak.bLooted			   = (bool)pDoodad->m_bLooted;
    Pak.nObstacleGroup     = pDoodad->m_nObstacleGroup;
	Pak.uPosX              = pDoodad->m_nX;
	Pak.uPosY              = pDoodad->m_nY;
	Pak.uPosZ              = pDoodad->m_nZ;
    Pak.bObstacleState     = pDoodad->m_pScene->GetDynamicObstacleState(pDoodad->m_nObstacleGroup);
    
    bRetCode = Send(nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

// 广播Doodad的状态同步
BOOL KPlayerServer::DoSyncDoodadState(KDoodad* pDoodad)
{
    BOOL                    bResult         = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_SYNC_DOODAD_STATE   SyncDoodadState;

    assert(pDoodad);

    pScene = pDoodad->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

	SyncDoodadState.byProtocolID    = s2c_sync_doodad_state;
	SyncDoodadState.dwID            = pDoodad->m_dwID;
	SyncDoodadState.cState          = (char)pDoodad->m_eState;
    SyncDoodadState.bObstacleState  = (bool)pScene->GetDynamicObstacleState(pDoodad->m_nObstacleGroup);

	BroadcastFunc.m_pvData            = &SyncDoodadState;
	BroadcastFunc.m_uSize             = sizeof(SyncDoodadState);

    pScene->Broadcast(pDoodad->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncMoveCtrl(KPlayer* pPlayer)
{
    BOOL                bResult         = false;
    KBroadcastFunc      BroadcastFunc;
    S2C_SYNC_MOVE_CTRL  Pak;

    assert(pPlayer->m_pScene);

    Pak.byProtocolID =  s2c_sync_move_ctrl;

    Pak.dwID                = pPlayer->m_dwID;
    Pak.byMoveFrameCount    = (BYTE)pPlayer->m_nMoveFrameCounter;
    Pak.byFaceDirection     = (BYTE)pPlayer->m_nFaceDirection;
    Pak.byPitchDirection    = (BYTE)pPlayer->m_nPitchDirection;

    Pak.uMoveCtrlDisable    = pPlayer->m_MoveCtrl.bDisable;
    Pak.uMoveCtrlMoving     = pPlayer->m_MoveCtrl.bMove;
    Pak.uMoveCtrlWalk       = pPlayer->m_MoveCtrl.bWalk;
    Pak.uMoveCtrlDirection8 = pPlayer->m_MoveCtrl.nDirection8;
    Pak.nMoveCtrlTurn       = pPlayer->m_MoveCtrl.nTurn;
    Pak.uPosX               = pPlayer->m_nX;
    Pak.uPosY               = pPlayer->m_nY;
    Pak.uPosZ               = pPlayer->m_nZ;

    Pak.uMoveState          = pPlayer->m_eMoveState;
    Pak.uVelocityXY         = pPlayer->m_nVelocityXY;
    Pak.nVelocityZ          = pPlayer->m_nVelocityZ;
    Pak.uDirectionXY        = pPlayer->m_nDirectionXY;
    Pak.uConvergenceSpeed   = pPlayer->m_nConvergenceSpeed;
    Pak.uCurrentGravity     = pPlayer->m_nCurrentGravity;
    Pak.uTryPathState       = pPlayer->m_bTryPathState;
    Pak.uTryPathSide        = pPlayer->m_nTryPathSide;
    Pak.uSlipFlag           = pPlayer->m_bSlip;
    Pak.uRunSpeed           = pPlayer->m_nRunSpeed;

    if (pPlayer->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pPlayer->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        Pak.uPosX         = pPlayer->m_nCurrentTrack;
        Pak.uPosY         = (pPlayer->m_nFromNode == pTrack->nToNode);
        Pak.uPosZ         = pPlayer->m_nMoveFrameCounter;
    }

    BroadcastFunc.m_pvData            = &Pak;
    BroadcastFunc.m_uSize             = sizeof(Pak);
    BroadcastFunc.m_dwExclusive       = pPlayer->m_dwOwner;
    BroadcastFunc.m_dwExcept          = pPlayer->m_dwID;
    
    pPlayer->m_pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncMoveParam(KCharacter* pCharacter)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KBroadcastFunc              BroadcastFunc;
    S2C_SYNC_MOVE_PARAM         Pak;

    KG_PROCESS_ERROR(pCharacter->m_pScene);

    assert(pCharacter->m_nDirectionXY >= 0 && pCharacter->m_nDirectionXY < DIRECTION_COUNT);
    assert(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection < DIRECTION_COUNT);
    assert(pCharacter->m_nCurrentGravity >= 0 && pCharacter->m_nCurrentGravity <= MAX_GRAVITY);

    Pak.byProtocolID        = s2c_sync_move_param;

    Pak.dwID                = pCharacter->m_dwID;
    Pak.byFaceDirection     = (BYTE)pCharacter->m_nFaceDirection;
    Pak.byPitchDirection    = (BYTE)pCharacter->m_nPitchDirection;
    Pak.byMoveFrameCount    = (BYTE)pCharacter->m_nMoveFrameCounter;

    Pak.uMoveState          = (BYTE)pCharacter->m_eMoveState;
    Pak.uPosX               = pCharacter->m_nX;
    Pak.uPosY               = pCharacter->m_nY;
    Pak.uPosZ               = pCharacter->m_nZ;
    Pak.uTryPathState       = pCharacter->m_bTryPathState;
    Pak.uTryPathSide        = pCharacter->m_nTryPathSide;

    Pak.uVelocityXY         = pCharacter->m_nVelocityXY;
    Pak.uRunSpeed           = pCharacter->m_nRunSpeed;
    Pak.nVelocityZ          = pCharacter->m_nVelocityZ;
    Pak.uDestX              = pCharacter->m_nDestX;
    Pak.uDestY              = pCharacter->m_nDestY;

    Pak.uConvergenceSpeed   = pCharacter->m_nConvergenceSpeed;
    Pak.uDirectionXY        = pCharacter->m_nDirectionXY;
    Pak.uCurrentGravity     = pCharacter->m_nCurrentGravity;
    Pak.uSlipFlag           = pCharacter->m_bSlip;

    if (pCharacter->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pCharacter->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        Pak.uPosX         = pCharacter->m_nCurrentTrack;
        Pak.uPosY         = (pCharacter->m_nFromNode == pTrack->nToNode);
        Pak.uPosZ         = pCharacter->m_nMoveFrameCounter;
    }

    BroadcastFunc.m_pvData            = &Pak;
    BroadcastFunc.m_uSize             = sizeof(Pak);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;
    BroadcastFunc.m_dwExcept          = pCharacter->m_dwID;
    
    pCharacter->m_pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAdjustPlayerMove(KPlayer* pPlayer)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2C_ADJUST_PLAYER_MOVE      Pak;

    assert(pPlayer->m_nDirectionXY >= 0 && pPlayer->m_nDirectionXY < DIRECTION_COUNT);
    assert(pPlayer->m_nFaceDirection >= 0 && pPlayer->m_nFaceDirection < DIRECTION_COUNT);
    assert(pPlayer->m_nCurrentGravity >= 0 && pPlayer->m_nCurrentGravity <= MAX_GRAVITY);

    // KGLogPrintf(KGLOG_DEBUG, "DoReviseMoveState, m_nVirtualFrame: %d", pPlayer->m_nVirtualFrame);

    Pak.byProtocolID        = s2c_adjust_player_move;

    Pak.nMoveCount          = pPlayer->m_MoveCtrl.nMoveCount;
    Pak.nVirtualFrame       = pPlayer->m_nVirtualFrame;
    Pak.nJumpLimitFrame     = pPlayer->m_nJumpLimitFrame;
    Pak.byFaceDirection     = (BYTE)pPlayer->m_nFaceDirection;
    Pak.byPitchDirection    = (BYTE)pPlayer->m_nPitchDirection;
    Pak.byMoveFrameCount    = (BYTE)pPlayer->m_nMoveFrameCounter;

    Pak.uMoveState          = (BYTE)pPlayer->m_eMoveState;
    Pak.uPosX               = pPlayer->m_nX;
    Pak.uPosY               = pPlayer->m_nY;
    Pak.uPosZ               = pPlayer->m_nZ;
    Pak.uTryPathState       = pPlayer->m_bTryPathState;
    Pak.uTryPathSide        = pPlayer->m_nTryPathSide;

    Pak.uVelocityXY         = pPlayer->m_nVelocityXY;
    Pak.uRunSpeed           = pPlayer->m_nRunSpeed;
    Pak.nVelocityZ          = pPlayer->m_nVelocityZ;
    Pak.uDestX              = pPlayer->m_nDestX;
    Pak.uDestY              = pPlayer->m_nDestY;

    Pak.uConvergenceSpeed   = pPlayer->m_nConvergenceSpeed;
    Pak.uDirectionXY        = pPlayer->m_nDirectionXY;
    Pak.uCurrentGravity     = pPlayer->m_nCurrentGravity;
    Pak.uSlipFlag           = pPlayer->m_bSlip;

    if (pPlayer->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pPlayer->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        Pak.uPosX         = pPlayer->m_nCurrentTrack;
        Pak.uPosY         = (pPlayer->m_nFromNode == pTrack->nToNode);
        Pak.uPosZ         = pPlayer->m_nMoveFrameCounter;
    }

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[move] do adjust, count=%d,frame=%d<%d,%d,%d,%d>", 
    //    Pak.nMoveCount, Pak.nVirtualFrame,
    //    pPlayer->m_eMoveState, pPlayer->m_nX, pPlayer->m_nY, pPlayer->m_nZ
    //);

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncMoveState(KCharacter* pCharacter, BOOL bSyncSelf)
{
    BOOL                    bResult         = false;
    KBroadcastFunc          BroadcastFunc;
    S2C_SYNC_MOVE_STATE     MoveState;

    assert(pCharacter->m_pScene);
    assert(pCharacter->m_nVelocityZ >= MIN_VELOCITY_Z && pCharacter->m_nVelocityZ <= MAX_VELOCITY_Z);
    assert(pCharacter->m_nVelocityXY >= 0 && pCharacter->m_nVelocityXY <= MAX_ZOOM_VELOCITY);

    MoveState.byProtocolID      = s2c_sync_move_state;
    MoveState.dwID              = pCharacter->m_dwID;
    MoveState.byFaceDirection   = (BYTE)pCharacter->m_nFaceDirection;
    MoveState.byPitchDirection  = (BYTE)pCharacter->m_nPitchDirection;
    MoveState.byMoveFrameCount  = (BYTE)pCharacter->m_nMoveFrameCounter;

    MoveState.uMoveState        = pCharacter->m_eMoveState;
    MoveState.uPosX             = pCharacter->m_nX;
    MoveState.uPosY             = pCharacter->m_nY;
    MoveState.uPosZ             = pCharacter->m_nZ;
    MoveState.uFightState       = pCharacter->m_bFightState;
    MoveState.uTryPathState     = pCharacter->m_bTryPathState;
    MoveState.uTryPathSide      = pCharacter->m_nTryPathSide;

    MoveState.uVelocityXY       = pCharacter->m_nVelocityXY;
    MoveState.nVelocityZ        = pCharacter->m_nVelocityZ;
    MoveState.uConvergenceSpeed = pCharacter->m_nConvergenceSpeed;
    MoveState.uDestX            = pCharacter->m_nDestX;
    MoveState.uDestY            = pCharacter->m_nDestY;

    MoveState.uDirectionXY      = pCharacter->m_nDirectionXY;
    MoveState.uJumpCount        = pCharacter->m_nJumpCount;
    MoveState.uSlipFlag         = pCharacter->m_bSlip;

    if (pCharacter->m_eMoveState == cmsOnAutoFly)
    {
        KTRACK* pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pCharacter->m_nCurrentTrack);
        KGLOG_PROCESS_ERROR(pTrack);

        MoveState.uPosX         = pCharacter->m_nCurrentTrack;
        MoveState.uPosY         = (pCharacter->m_nFromNode == pTrack->nToNode);
        MoveState.uPosZ         = pCharacter->m_nMoveFrameCounter;
    }

    BroadcastFunc.m_pvData            = &MoveState;
    BroadcastFunc.m_uSize             = sizeof(MoveState);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;
    BroadcastFunc.m_dwExcept          = bSyncSelf ? ERROR_ID : pCharacter->m_dwID;
    
    pCharacter->m_pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

// 角色死亡指令同步
BOOL KPlayerServer::DoCharacterDeath(KCharacter* pCharacter, DWORD dwKillerID, int nSituReviveFrame)
{
    BOOL                bResult         = false;
    KScene*             pScene          = NULL;
    KBroadcastFunc      BroadcastFunc;
	S2C_CHARACTER_DEATH MoveCommand;

    assert(pCharacter->m_pScene);

	MoveCommand.byProtocolID     = s2c_character_death;
	MoveCommand.dwCharacterID    = pCharacter->m_dwID;
    MoveCommand.nSituReviveFrame = nSituReviveFrame;
	MoveCommand.dwKillerID       = dwKillerID;

    MoveCommand.uSrcX             = pCharacter->m_nX;
    MoveCommand.uSrcY             = pCharacter->m_nY;
    MoveCommand.uSrcZ             = pCharacter->m_nZ;
    MoveCommand.uFaceDirection    = pCharacter->m_nFaceDirection;

	BroadcastFunc.m_pvData            = &MoveCommand;
	BroadcastFunc.m_uSize             = sizeof(MoveCommand);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;

	pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);
    
    if (IS_PLAYER(pCharacter->m_dwID))
    {
        ((KPlayer*)pCharacter)->m_nRecordCount = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

// 移动角色的位置
BOOL KPlayerServer::DoMoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ, BOOL bIgnoreBlock)
{
    BOOL                bResult     = false;
	int                 nRetCode    = false;
    KScene*             pScene      = NULL;
    KBroadcastFunc      BroadcastFunc;
	S2C_MOVE_CHARACTER	MoveCharacter;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);
    assert(pCharacter->m_nVelocityXY >= 0 && pCharacter->m_nVelocityXY <= MAX_ZOOM_VELOCITY);
    assert(pCharacter->m_nVelocityZ >= MIN_VELOCITY_Z && pCharacter->m_nVelocityZ <= MAX_VELOCITY_Z);

	MoveCharacter.byProtocolID      = s2c_move_character;
	MoveCharacter.dwID              = pCharacter->m_dwID;
    MoveCharacter.byFaceDirection   = (BYTE)pCharacter->m_nFaceDirection;

	MoveCharacter.nPosX             = nX;
	MoveCharacter.nPosY             = nY;
	MoveCharacter.nPosZ             = nZ;
    MoveCharacter.byDirectionXY     = (BYTE)pCharacter->m_nDirectionXY;

    MoveCharacter.uVelocityXY       = (uint16_t)pCharacter->m_nVelocityXY;
    MoveCharacter.nVelocityZ        = (int16_t)pCharacter->m_nVelocityZ;
    MoveCharacter.uConvergenceSpeed = (uint16_t)pCharacter->m_nConvergenceSpeed;
    MoveCharacter.byMoveState       = (BYTE)pCharacter->m_eMoveState;
    MoveCharacter.bIgnoreBlock      = bIgnoreBlock ? 1 : 0;

	BroadcastFunc.m_pvData            = &MoveCharacter;
	BroadcastFunc.m_uSize             = sizeof(MoveCharacter);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        ((KPlayer*)pCharacter)->m_nRecordCount = 0;
    }

    bResult = true;
Exit0:
	return bResult;
}

// 玩家被复活通知
BOOL KPlayerServer::DoPlayerRevivedNotiy(int nConnIndex, KCharacter* pCharacter)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    S2C_PLAYER_REVIVED_NOTIFY   Notify;

    Notify.byProtocolID  = s2c_player_revived_notify;
    Notify.dwCharacterID = pCharacter->m_dwID;

    bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoCharacterAction(KCharacter* pCharacter, DWORD dwCharacterID, DWORD dwAction)
{
    BOOL                    bResult         = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_CHARACTER_ACTION	CharacterAction;
    
    assert(pCharacter->m_pScene);

    CharacterAction.byProtocolID = s2c_character_action;
	CharacterAction.dwSrcCharacterID = pCharacter->m_dwID;
	CharacterAction.dwDestCharacterID = dwCharacterID;
	CharacterAction.dwAction = dwAction;

	BroadcastFunc.m_pvData      = &CharacterAction;
	BroadcastFunc.m_uSize       = sizeof(CharacterAction);
    BroadcastFunc.m_dwExclusive = pCharacter->m_dwOwner;

	pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);
    
    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoOverTimeAction(
    KCharacter* pCharacter, int nActionType, DWORD dwActionID, int nDirection, int nDurationFrame
)
{
    int                     nResult         = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
    S2C_OVER_TIME_ACTION    OTAction;

    assert(pCharacter->m_pScene);

    OTAction.byProtocolID      = s2c_over_time_action;
    OTAction.dwCharacterID     = pCharacter->m_dwID;
    OTAction.cActionType       = (char)nActionType;
    OTAction.wActionID         = (WORD)dwActionID;
    OTAction.byDirection       = (BYTE)nDirection;
    OTAction.wDurationFrame    = (WORD)nDurationFrame;

	BroadcastFunc.m_pvData          = &OTAction;
	BroadcastFunc.m_uSize           = (DWORD)sizeof(OTAction);
    BroadcastFunc.m_dwExclusive     = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

// 发送玩家显示数据
BOOL KPlayerServer::DoPlayerDisplayData(DWORD dwPlayerID, DWORD dwCheckSum, int nConnIndex)
{
    BOOL                    bResult             = false;
	BOOL                    bRetCode            = false;
    KPlayer*                pPlayer             = NULL;
	S2C_PLAYER_DISPLAY_DATA PlayerDisplayData;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	PlayerDisplayData.byProtocolID              = s2c_player_display_data;
	PlayerDisplayData.dwPlayerID                = pPlayer->m_dwID;
	PlayerDisplayData.byRoleType                = (BYTE)pPlayer->m_eRoleType;
	PlayerDisplayData.byLevel					= (BYTE)pPlayer->m_nLevel;
    PlayerDisplayData.byCamp                    = (BYTE)pPlayer->m_eCamp;
    PlayerDisplayData.byForceID					= (BYTE)pPlayer->m_dwForceID;
    PlayerDisplayData.dwTongID                  = pPlayer->m_dwTongID;
    PlayerDisplayData.byBattleFieldSide         = (char)pPlayer->m_nBattleFieldSide;

	strncpy(PlayerDisplayData.szRoleName, pPlayer->m_szName, _NAME_LEN);
	PlayerDisplayData.szRoleName[_NAME_LEN - 1] = 0;
    
    PlayerDisplayData.wModelID            = (WORD)pPlayer->m_dwModelID;

    PlayerDisplayData.uFaceStyle          = pPlayer->m_wRepresentId[perFaceStyle];
    PlayerDisplayData.uHairStyle          = pPlayer->m_wRepresentId[perHairStyle];
    PlayerDisplayData.uHelmStyle          = pPlayer->m_wRepresentId[perHelmStyle];
    PlayerDisplayData.uHelmColor          = pPlayer->m_wRepresentId[perHelmColor];
    PlayerDisplayData.uHelmEnchant        = pPlayer->m_wRepresentId[perHelmEnchant];
    PlayerDisplayData.uChestStyle         = pPlayer->m_wRepresentId[perChestStyle];   
    PlayerDisplayData.uChestColor         = pPlayer->m_wRepresentId[perChestColor];
    PlayerDisplayData.uChestEnchant       = pPlayer->m_wRepresentId[perChestEnchant];
    PlayerDisplayData.uWaistStyle         = pPlayer->m_wRepresentId[perWaistStyle];
    PlayerDisplayData.uWaistColor         = pPlayer->m_wRepresentId[perWaistColor];
    PlayerDisplayData.uWaistEnchant       = pPlayer->m_wRepresentId[perWaistEnchant];

    PlayerDisplayData.uBangleStyle        = pPlayer->m_wRepresentId[perBangleStyle];
    PlayerDisplayData.uBangleColor        = pPlayer->m_wRepresentId[perBangleColor];
    PlayerDisplayData.uBangleEnchant      = pPlayer->m_wRepresentId[perBangleEnchant];
    PlayerDisplayData.uBootsStyle         = pPlayer->m_wRepresentId[perBootsStyle];
    PlayerDisplayData.uBootsColor         = pPlayer->m_wRepresentId[perBootsColor];
    PlayerDisplayData.uWeaponStyle        = pPlayer->m_wRepresentId[perWeaponStyle];
    PlayerDisplayData.uWeaponEnchant1     = pPlayer->m_wRepresentId[perWeaponEnchant1];
    PlayerDisplayData.uWeaponEnchant2     = pPlayer->m_wRepresentId[perWeaponEnchant2];
    PlayerDisplayData.uBackExtend         = pPlayer->m_wRepresentId[perBackExtend];
    PlayerDisplayData.uWaistExtend        = pPlayer->m_wRepresentId[perWaistExtend];

    PlayerDisplayData.uHorseStyle         = pPlayer->m_wRepresentId[perHorseStyle];     
    PlayerDisplayData.uHorseAdornment1    = pPlayer->m_wRepresentId[perHorseAdornment1];
    PlayerDisplayData.uHorseAdornment2    = pPlayer->m_wRepresentId[perHorseAdornment2];
    PlayerDisplayData.uHorseAdornment3    = pPlayer->m_wRepresentId[perHorseAdornment3];
    PlayerDisplayData.uHorseAdornment4    = pPlayer->m_wRepresentId[perHorseAdornment4];
    PlayerDisplayData.uSchoolID           = pPlayer->m_dwSchoolID;

	if (dwCheckSum == 0 || dwCheckSum != CRC32(0, &PlayerDisplayData, sizeof(PlayerDisplayData)))
	{
		bRetCode = Send(nConnIndex, &PlayerDisplayData, sizeof(PlayerDisplayData));
		KG_PROCESS_ERROR(bRetCode);
	}

    bResult = true;
Exit0:
	return bResult;
}

// 获得经验值
BOOL KPlayerServer::DoSyncExperience(KPlayer* pPlayer, int nDeltaExp)
{
    BOOL                bResult         = false;
	int                 nRetCode        = false;
	S2C_SYNC_EXPERIENCE ExpInfo;

	ExpInfo.byProtocolID  = s2c_sync_experience;
    ExpInfo.nCurrentExp   = pPlayer->m_nExperience;
    ExpInfo.nDeltaExp     = nDeltaExp;

	nRetCode = Send(pPlayer->m_nConnIndex, &ExpInfo, sizeof(ExpInfo));
	KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoPlayerLevelUp(KPlayer* pPlayer)
{
    int                 nResult         = false;
    BOOL                bRetCode        = false;
    KScene*             pScene          = NULL;
    KBroadcastFunc      BroadcastFunc;
    S2C_PLAYER_LEVEL_UP Pak;
    
    assert(pPlayer);
    assert(pPlayer->m_pScene);

    Pak.byProtocolID    = s2c_player_level_up;
    Pak.dwID            = pPlayer->m_dwID;
    Pak.nLevel          = pPlayer->m_nLevel;

    BroadcastFunc.m_pvData  = &Pak;
    BroadcastFunc.m_uSize   = sizeof(Pak);

    pScene = pPlayer->m_pScene;
    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoCommonHealth(KCharacter* pCharacter, int nDeltaLife)
{
    int                 nResult         = false;
    BOOL                bRetCode        = false;
    KScene*             pScene          = NULL;
    KBroadcastFunc      BroadcastFunc;
    S2C_COMMON_HEALTH   Pak;

    assert(pCharacter);
    assert(pCharacter->m_pScene);

    Pak.byProtocolID    = s2c_common_health;
    Pak.dwCharacterID   = pCharacter->m_dwID;
    Pak.nDeltaLife      = nDeltaLife;

    BroadcastFunc.m_pvData            = &Pak;
    BroadcastFunc.m_uSize             = (DWORD)sizeof(S2C_COMMON_HEALTH);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}


// ----------------- skill about -------------------------->

BOOL KPlayerServer::DoSyncSkillData(int nConnIndex, DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwSkillExp, BOOL bNotifyPlayer)
{
    int                 nResult     = false;
    BOOL                bRetCode    = false;
    S2C_SYNC_SKILL_DATA SyncSkillData;

    SyncSkillData.byProtocolID  = s2c_sync_skill_data;
    SyncSkillData.dwSkillID     = dwSkillID;
    SyncSkillData.bySkillLevel  = (BYTE)dwSkillLevel;
    SyncSkillData.dwSkillExp    = dwSkillExp;
    SyncSkillData.bNotifyPlayer = bNotifyPlayer;

    bRetCode = Send(nConnIndex, &SyncSkillData, (DWORD)sizeof(SyncSkillData));
    KG_PROCESS_ERROR(bRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncSkillRecipe(int nConnIndex, size_t uArrayCount, KG_SKILL_RECIPE_LIST_NODE* pRecipeArray)
{
    BOOL                    bResult             = false;
    BOOL                    bRetCode            = false;
    S2C_SYNC_SKILL_RECIPE*  pSyncSkillRecipe    = (S2C_SYNC_SKILL_RECIPE*)m_byTempData;
    size_t                  uSize               = sizeof(S2C_SYNC_SKILL_RECIPE) + sizeof(S2C_SYNC_SKILL_RECIPE::_RecipeNode) * uArrayCount;

    KGLOG_PROCESS_ERROR(uSize <= MAX_EXTERNAL_PACKAGE_SIZE);

    if (uArrayCount)
    {
        assert(pRecipeArray);
    }

    pSyncSkillRecipe->byProtocolID  = s2c_sync_skill_recipe;
    pSyncSkillRecipe->wSize         = (WORD)uSize;
    pSyncSkillRecipe->wCount        = (WORD)uArrayCount;

    for (size_t i = 0; i < uArrayCount; i++)
    {
        pSyncSkillRecipe->RecipeArray[i].dwRecipeKey    = pRecipeArray[i].dwRecipeKey;
        pSyncSkillRecipe->RecipeArray[i].bActive        = pRecipeArray[i].bActive;
    }

    bRetCode = Send(nConnIndex, pSyncSkillRecipe, pSyncSkillRecipe->wSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// sync character doing skill
BOOL KPlayerServer::DoSkillPrepare(
    KCharacter *pCharacter, DWORD dwSkillID, DWORD dwSkillLevel, int nPrepareFrame, KTarget &rTarget
)
{
    int                     nResult         = false;
    BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    TARGET_TYPE             eTargetType     = ttInvalid;
    KSceneObject*           pSceneObject    = NULL;
    KBroadcastFunc          BroadcastFunc;
    S2C_SKILL_PREPARE       PrepareSkill;

    KG_ASSERT_EXIT(pCharacter);
    assert(pCharacter->m_pScene);

    PrepareSkill.byProtocolID     = s2c_skill_prepare;
    PrepareSkill.dwCharacterID    = pCharacter->m_dwID;
    PrepareSkill.dwSkillID        = dwSkillID;
    PrepareSkill.bySkillLevel     = (BYTE)dwSkillLevel;
    PrepareSkill.nPrepareFrame    = nPrepareFrame;

    eTargetType = rTarget.GetTargetType();

    switch (eTargetType)
    {
    case ttCoordination:
        PrepareSkill.byTargetType   = (BYTE)eTargetType;
        bRetCode = rTarget.GetTarget(&PrepareSkill.nParam1, &PrepareSkill.nParam2, &PrepareSkill.nParam3);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

    case ttNpc:
    case ttDoodad:
    case ttPlayer:
        bRetCode = rTarget.GetTarget(&pSceneObject);
        KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);
        PrepareSkill.byTargetType = (BYTE)eTargetType;
        PrepareSkill.nParam1      = pSceneObject->m_dwID;
        break;

    default:
        KG_ASSERT_EXIT(!"Unexpected target type !");
    }

    BroadcastFunc.m_pvData          = &PrepareSkill;
    BroadcastFunc.m_uSize           = (DWORD)sizeof(PrepareSkill);
    BroadcastFunc.m_dwExclusive     = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSkillCast(KCharacter *pCharacter, KSkill* pSkill, KTarget &rTarget, BOOL bSubCast)
{
    int                     nResult         = false;
    BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    TARGET_TYPE             eTargetType     = ttInvalid;
    KSceneObject*           pSceneObject    = NULL;
    KBroadcastFunc          BroadcastFunc;
    S2C_SKILL_CAST          Notify;

    assert(pCharacter);
    assert(pCharacter->m_pScene);
    assert(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX); // byFaceDirection

    Notify.byProtocolID     = s2c_skill_cast;
    Notify.dwBulletID       = g_pSO3World->m_SkillManager.m_dwBulletIDIndex;
    Notify.dwCharacterID    = pCharacter->m_dwID;
    Notify.dwSkillID        = pSkill->m_pBaseInfo->dwSkillID;
    Notify.bySkillLevel     = (BYTE)pSkill->m_dwLevel;
    Notify.byFaceDirection  = (BYTE)pCharacter->m_nFaceDirection;
    Notify.bSubCast         = (bool)bSubCast;

    eTargetType = rTarget.GetTargetType();

    switch (eTargetType)
    {
    case ttCoordination:
        Notify.byTargetType     = (BYTE)eTargetType;
        bRetCode = rTarget.GetTarget(&Notify.nParam1, &Notify.nParam2, &Notify.nParam3);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

    case ttNpc:
    case ttDoodad:
    case ttPlayer:
        bRetCode = rTarget.GetTarget(&pSceneObject);
        KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);
        Notify.byTargetType = (BYTE)eTargetType;
        Notify.nParam1      = pSceneObject->m_dwID;
        break;

    default:
        KG_ASSERT_EXIT(!"Unexpected target type !");
    }

    BroadcastFunc.m_pvData        = &Notify;
    BroadcastFunc.m_uSize         = (DWORD)sizeof(Notify);
    BroadcastFunc.m_dwExclusive   = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncCommonSkillAutoCastState(int nConnIndex, BOOL bStart, BOOL bMelee)
{
    int                                   nResult  = false;
    int                                   nRetCode = 0;
    S2C_SYNC_COMMON_SKILL_AUTO_CAST_STATE Notify;

    Notify.byProtocolID     = s2c_sync_common_skill_auto_cast_state;
    Notify.bStart           = bStart;
    Notify.bMelee           = bMelee;
  
    nRetCode = Send(nConnIndex, &Notify, (DWORD)sizeof(Notify));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}
BOOL KPlayerServer::DoResetCooldown(int nConnIndex, DWORD dwCoolDownID, int nEndFrame, int nInterval)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    S2C_RESET_COOLDOWN          Notify;

    Notify.byProtocolID = s2c_reset_cooldown;
    Notify.dwCoolDownID = dwCoolDownID;
    Notify.nEndFrame    = nEndFrame;
    Notify.nInterval    = nInterval;

    bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSkillChainEffect(KCharacter* pCaster, DWORD dwSkillID, DWORD dwSkillLevel, size_t uTargetCount, KSKILL_CHAIN_TARGET* pTarget)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    S2C_SKILL_CHAIN_EFFECT* pPak        = NULL;
    KScene*                 pScene      = NULL;
    size_t                  uSize       = sizeof(S2C_SKILL_CHAIN_EFFECT) + sizeof(DWORD) * uTargetCount;
    KBroadcastFunc          BroadcastFunc;

    assert(pCaster);
    assert(pCaster->m_pScene);
    assert(uTargetCount <= MAX_CHAIN_TARGET);
    assert(pTarget);

    assert(uSize <= sizeof(m_byTempData));
    pPak = (S2C_SKILL_CHAIN_EFFECT*)m_byTempData;

    pPak->byProtocolID      = s2c_skill_chain_effect;
    pPak->dwSkillID         = dwSkillID;
    pPak->dwSkillLevel      = dwSkillLevel;
    pPak->dwCasterID        = pCaster->m_dwID;
    pPak->uCount            = uTargetCount;
    pPak->wSize             = (WORD)uSize;

    for (int i = 0; i < (int)pPak->uCount; i++)
    {
        pPak->dwCharacterID[i] = pTarget[i].pCharacter->m_dwID;
    }

    BroadcastFunc.m_pvData          = pPak;
    BroadcastFunc.m_uSize           = (size_t)pPak->wSize;
    BroadcastFunc.m_dwExclusive     = pCaster->m_dwOwner;

    pScene = pCaster->m_pScene;
    pScene->Broadcast(pCaster->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

struct KTeamSkillBroadcast
{
    DWORD   m_dwTeamID;
    DWORD   m_dwExceptID;
    void*   m_pvData;
    size_t  m_uSize;
    KScene* m_pScene;

    BOOL operator () (KTEAM_MEMBER_INFO* pMember)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(pMember->dwMemberID);
        if (!pPlayer)
            return true;

        if (m_dwExceptID == pMember->dwMemberID)
            return true;

        if (m_pScene != pPlayer->m_pScene)
            return true;

        g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uSize);
        return true;
    };
};

BOOL KPlayerServer::DoSkillEffectResult(
    KCharacter* pSkillSrc, KCharacter* pSkillDst, KCharacter* pEffectDst, 
    KSKILL_CALCULATE_RESULT* pSkillResult, DWORD dwBulletID
)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KPlayer*                    pPlayerSrc  = NULL;
    KPlayer*                    pPlayerDst  = NULL;
    S2C_SKILL_EFFECT_RESULT*    pPak        = (S2C_SKILL_EFFECT_RESULT*)m_byTempData;
    DWORD                       dwPakSize   = 0;
    int                         nItemCount  = 0;

    KG_PROCESS_ERROR(pEffectDst);
    KGLOG_PROCESS_ERROR(pSkillResult);
    KGLOG_PROCESS_ERROR(pSkillSrc || pSkillDst);
    KGLOG_PROCESS_ERROR(
        sizeof(S2C_SKILL_EFFECT_RESULT) + sizeof(S2C_SKILL_EFFECT_RESULT::KSKILL_RESULT) * serTotal < 
        MAX_EXTERNAL_PACKAGE_SIZE
    );
        
    pPak->byProtocolID        = s2c_skill_effect_result;
    pPak->dwSkillSrc          = pSkillSrc ? pSkillSrc->m_dwID : ERROR_ID;
    pPak->dwSkillDst          = pSkillDst ? pSkillDst->m_dwID : ERROR_ID;
	pPak->dwBulletID          = dwBulletID;
    
    pPak->EffectSrc.byType    = (BYTE)pSkillResult->EffectSrc.eType;
    pPak->EffectSrc.dwID      = pSkillResult->EffectSrc.dwID;
    pPak->EffectSrc.byLevel   = (BYTE)pSkillResult->EffectSrc.nLevel;

    pPak->bReact              = (pSkillSrc != pSkillDst) && (pEffectDst == pSkillSrc);
    pPak->bShield             = pSkillResult->bShield;
    pPak->bHitTarget          = pSkillResult->bHitTarget;
    pPak->bSkillDodge         = pSkillResult->bSkillDodge;
    pPak->bCriticalStrike     = pSkillResult->bCriticalStrike;   
    pPak->bSkillImmunity      = pSkillResult->bImmunity;
    pPak->bParry              = pSkillResult->bParry;
    pPak->bInsight            = pSkillResult->bInsight;

    pPak->bPhysicsBlock       = pSkillResult->bPhysicsBlock;       
    pPak->bSolarMagicBlock    = pSkillResult->bSolarMagicBlock; 
    pPak->bNeutralMagicBlock  = pSkillResult->bNeutralMagicBlock; 
    pPak->bLunarMagicBlock    = pSkillResult->bLunarMagicBlock; 
    pPak->bPoisonBlock        = pSkillResult->bPoisonBlock; 

    pPak->bBuffResist       = pSkillResult->bBuffResist;
    pPak->bBuffImmunity     = pSkillResult->bBuffImmunity;
    pPak->dwBuffID          = (DWORD)pSkillResult->nBuffID;
    pPak->byBuffLevel       = (BYTE)pSkillResult->nBuffLevel;
    
	for (int nEffectResultType = serPhysicsDamage; nEffectResultType < serTotal; ++nEffectResultType)
    {
        if (pSkillResult->nSkillEffectResult[nEffectResultType] > 0)
        {
            pPak->SkillResult[nItemCount].cEffectResultType = (char)nEffectResultType;
            pPak->SkillResult[nItemCount].nValue            = pSkillResult->nSkillEffectResult[nEffectResultType];
            nItemCount++;
        }
    }

    pPak->cResultCount = (char)nItemCount;

    dwPakSize = (DWORD)sizeof(S2C_SKILL_EFFECT_RESULT) + (DWORD)sizeof(S2C_SKILL_EFFECT_RESULT::KSKILL_RESULT) * nItemCount;

    pPak->wSize = (WORD)dwPakSize;
    // 技能命中结果同步给src, dst及其各自的同一场景中的队友

    if (pSkillSrc && IS_PLAYER(pSkillSrc->m_dwID))
    {
        pPlayerSrc = (KPlayer*)pSkillSrc;
        bRetCode = Send(pPlayerSrc->m_nConnIndex, m_byTempData, dwPakSize);
        //KGLOG_CHECK_ERROR(nRetCode);

        // 队友同步
        KTeam* pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayerSrc->m_dwTeamID);
        if (pTeam)
        {
            KTeamSkillBroadcast Func;

            Func.m_dwExceptID = pSkillSrc->m_dwID;
            Func.m_dwTeamID   = pPlayerSrc->m_dwTeamID;
            Func.m_pvData     = &m_byTempData;
            Func.m_uSize      = dwPakSize;
            Func.m_pScene     = pSkillSrc->m_pScene;

            g_pSO3World->m_TeamServer.Traverse(Func);
        }
    }

    if (pSkillDst && (pSkillDst != pSkillSrc) && (IS_PLAYER(pSkillDst->m_dwID)))
    {
        pPlayerDst = (KPlayer*)pSkillDst;        

        if (pPlayerDst->m_dwTeamID == ERROR_ID)
        {
            // 如果他并没有组队，则只给他自己同步即可
            bRetCode = Send(pPlayerDst->m_nConnIndex, m_byTempData, dwPakSize);
            //KGLOG_CHECK_ERROR(nRetCode);
        }
        else // 他组队了，则同步给他的整个队伍，包括他自己
        {
            BOOL bInSamePartyWithSkillSrc = false;

            if (pSkillSrc && IS_PLAYER(pSkillSrc->m_dwID))
            {
                // 看他和pPlayerSrc是否在同一个队伍里面
                pPlayerSrc = (KPlayer*)pSkillSrc;
                bInSamePartyWithSkillSrc = (pPlayerSrc->m_dwTeamID == pPlayerDst->m_dwTeamID);
            }
            // pSkillSrc是Npc，当然不可能与他在同一个队伍

            if (!bInSamePartyWithSkillSrc)
            {
                // 如果不在同一个队伍里面，则给所有同场景中的队员广播
                KTeam* pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayerDst->m_dwTeamID);
                if (pTeam)
                {
                    KTeamSkillBroadcast Func;
                    
                    Func.m_dwTeamID = pPlayerDst->m_dwTeamID;
                    Func.m_pScene   = pPlayerDst->m_pScene;
                    Func.m_pvData   = &m_byTempData;
                    Func.m_uSize    = dwPakSize;

                    g_pSO3World->m_TeamServer.Traverse(Func);
                }
            }
            // else: 在同一个队伍里面的话,上面已经发送过了,不用再次发送
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBehitRepresent(
    KCharacter* pSkillSrc, KCharacter* pSkillDst, KCharacter* pEffectDst, 
    KSKILL_CALCULATE_RESULT* pSkillResult, DWORD dwBulletID
)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    KScene*                     pScene              = NULL;
    KBroadcastBeHitFunc         BroadcastBeHitFunc;
    S2C_SYNC_BEHIT_REPRESENT    Pak;

    KG_PROCESS_ERROR(pEffectDst);
    KGLOG_PROCESS_ERROR(pSkillResult);
    KGLOG_PROCESS_ERROR(pSkillSrc || pSkillDst);

    memset(&Pak, 0, sizeof(Pak));
    
    Pak.byProtocolID        = s2c_sync_behit_represent;
    Pak.dwSkillSrc          = pSkillSrc ? pSkillSrc->m_dwID : ERROR_ID;
    Pak.dwSkillDst          = pSkillDst ? pSkillDst->m_dwID : ERROR_ID;
    Pak.EffectSrc.byType    = (BYTE)pSkillResult->EffectSrc.eType;
    Pak.EffectSrc.dwID      = pSkillResult->EffectSrc.dwID;
    Pak.EffectSrc.byLevel   = (BYTE)pSkillResult->EffectSrc.nLevel;
    Pak.dwBulletID          = dwBulletID;
    
    Pak.bReact              = (pSkillSrc != pSkillDst) && (pEffectDst == pSkillSrc);
    Pak.bHitTarget          = pSkillResult->bHitTarget;
    Pak.bSkillDodge         = pSkillResult->bSkillDodge;
    Pak.bCriticalStrike     = pSkillResult->bCriticalStrike;
    Pak.bParry              = pSkillResult->bParry;
    Pak.bInsight            = pSkillResult->bInsight;
    
    Pak.bPhysicsBlock       = pSkillResult->bPhysicsBlock;
    Pak.bSolarMagicBlock    = pSkillResult->bSolarMagicBlock;
    Pak.bNeutralMagicBlock  = pSkillResult->bNeutralMagicBlock;
    Pak.bLunarMagicBlock    = pSkillResult->bLunarMagicBlock;
    Pak.bPoisonBlock        = pSkillResult->bPoisonBlock;

    Pak.bPhysicsDamage      = pSkillResult->nSkillEffectResult[serPhysicsDamage]        > 0 ? true : false;
    Pak.bSolarMagicDamage   = pSkillResult->nSkillEffectResult[serSolarMagicDamage]     > 0 ? true : false;
    Pak.bNeutralMagicDamage = pSkillResult->nSkillEffectResult[serNeutralMagicDamage]   > 0 ? true : false;
    Pak.bLunarMagicDamage   = pSkillResult->nSkillEffectResult[serLunarMagicDamage]     > 0 ? true : false;
    Pak.bPoisonDamage       = pSkillResult->nSkillEffectResult[serPoisonDamage]         > 0 ? true : false;
    Pak.bReflectiedDamage   = pSkillResult->nSkillEffectResult[serReflectiedDamage]     > 0 ? true : false;

    Pak.bTherapy            = pSkillResult->nSkillEffectResult[serTherapy]      > 0 ? true : false;
    Pak.bStealLife          = pSkillResult->nSkillEffectResult[serStealLife]    > 0 ? true : false;
    Pak.bAbsorbDamage       = pSkillResult->nSkillEffectResult[serAbsorbDamage] > 0 ? true : false;
    Pak.bShieldDamage       = pSkillResult->nSkillEffectResult[serShieldDamage] > 0 ? true : false;

	pScene = pSkillDst->m_pScene;
    KG_PROCESS_ERROR(pScene);

    BroadcastBeHitFunc.m_pvData         = &Pak;
    BroadcastBeHitFunc.m_uSize          = sizeof(S2C_SYNC_BEHIT_REPRESENT);

    if (pSkillSrc)
    {
        bRetCode = IS_PLAYER(pSkillSrc->m_dwID);
        if (bRetCode)
        {
            BroadcastBeHitFunc.m_pExceptSrc = (KPlayer*)pSkillSrc;
        }
    }

    if (pSkillDst)
    {
        bRetCode = IS_PLAYER(pSkillDst->m_dwID);
        if (bRetCode)
        {
            BroadcastBeHitFunc.m_pExceptDst = (KPlayer*)pSkillDst;
        }
    }

    pScene->Broadcast(pSkillDst->m_pRegion, BroadcastBeHitFunc, 1);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetBuffList(KPlayer* pPlayer)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    S2C_SYNC_BUFF_LIST*             pSyncBuffData   = (S2C_SYNC_BUFF_LIST*)m_byTempData;
    size_t                          uBuffDataSize   = 0;
    size_t                          uPakSize        = 0;
    KCharacter*                     pTarget         = NULL;
    DWORD                           dwNewCRC        = 0;
    TARGET_TYPE                     eTargetType     = ttInvalid;

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_BUFF_LIST) <= MAX_EXTERNAL_PACKAGE_SIZE);

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();

    switch (eTargetType)
    {
    case ttNpc:
    case ttPlayer:
        bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
        KG_PROCESS_ERROR(bRetCode && pTarget);
        break;

    default:
        goto Exit1;
    }

    KG_PROCESS_ERROR(pTarget && pTarget->m_dwID != pPlayer->m_dwID);

    pSyncBuffData->byProtocolID  = s2c_sync_buff_list;
    pSyncBuffData->dwCharacterID = pTarget->m_dwID;

    bRetCode = pTarget->m_BuffList.GetTransferData(
        pSyncBuffData->byData, MAX_EXTERNAL_PACKAGE_SIZE - (DWORD)sizeof(S2C_SYNC_BUFF_LIST), &uBuffDataSize
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    uPakSize = (DWORD)sizeof(S2C_SYNC_BUFF_LIST) + uBuffDataSize;
    pSyncBuffData->wDataSize = (WORD)uBuffDataSize;
    pSyncBuffData->wSize     = (WORD)uPakSize;

    dwNewCRC = CRC32(0, m_byTempData, (unsigned)uPakSize);
    KG_PROCESS_SUCCESS(dwNewCRC == pPlayer->m_dwTargetBuffCRC);
    pPlayer->m_dwTargetBuffCRC = dwNewCRC;

    bRetCode = Send(pPlayer->m_nConnIndex, m_byTempData, uPakSize);
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetTargetBuffList(KPlayer* pPlayer)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    S2C_SYNC_BUFF_LIST*             pSyncBuffData   = (S2C_SYNC_BUFF_LIST*)m_byTempData;
    size_t                          uBuffDataSize   = 0;
    size_t                          uPakSize        = 0;
    KCharacter*                     pTarget         = NULL;
    KCharacter*                     pTargetTarget   = NULL;
    DWORD                           dwNewCRC        = 0;
    TARGET_TYPE                     eTargetType     = ttInvalid;

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTarget);

    eTargetType = pTarget->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pTarget->m_SelectTarget.GetTarget(&pTargetTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTargetTarget);

    KG_PROCESS_SUCCESS(pTargetTarget->m_dwID == pPlayer->m_dwID);

    pSyncBuffData->byProtocolID  = s2c_sync_buff_list;
    pSyncBuffData->dwCharacterID = pTargetTarget->m_dwID;

    bRetCode = pTargetTarget->m_BuffList.GetTransferData(
        pSyncBuffData->byData, MAX_EXTERNAL_PACKAGE_SIZE - (DWORD)sizeof(S2C_SYNC_BUFF_LIST), &uBuffDataSize
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    uPakSize = (DWORD)sizeof(S2C_SYNC_BUFF_LIST) + uBuffDataSize;
    pSyncBuffData->wDataSize = (WORD)uBuffDataSize;
    pSyncBuffData->wSize     = (WORD)uPakSize;

    dwNewCRC = CRC32(0, m_byTempData, (unsigned)uPakSize);
    KG_PROCESS_SUCCESS(dwNewCRC == pPlayer->m_dwTargetTargetBuffCRC);
    pPlayer->m_dwTargetTargetBuffCRC = dwNewCRC;

    bRetCode = Send(pPlayer->m_nConnIndex, m_byTempData, uPakSize);
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetDropID(KPlayer* pPlayer)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    DWORD                       dwTargetID  = 0;
    KNpc*                       pNpc        = NULL;
    S2C_SYNC_NPC_DROP_TARGET    NpcDropTarget;

    KG_PROCESS_ERROR(pPlayer->m_SelectTarget.GetTargetType() == ttNpc);

    dwTargetID = pPlayer->m_SelectTarget.GetTargetID();
    bRetCode = pPlayer->m_SelectTarget.GetTarget(&pNpc);
    KG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(pNpc);

    KG_PROCESS_ERROR(pPlayer->m_dwTargetDropID != pNpc->m_dwDropTargetPlayerID);

    NpcDropTarget.byProtocolID   = s2c_sync_npc_drop_target;
    NpcDropTarget.dwNpcID        = pNpc->m_dwID;
    NpcDropTarget.dwDropTargetID = pNpc->m_dwDropTargetPlayerID;

    Send(pPlayer->m_nConnIndex, &NpcDropTarget, sizeof(NpcDropTarget));

    pPlayer->m_dwTargetDropID = pNpc->m_dwDropTargetPlayerID;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBuffList(KPlayer* pPlayer, KPlayer* pDstPlayer)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    S2C_SYNC_BUFF_LIST*     pSyncBuffData   = (S2C_SYNC_BUFF_LIST*)m_byTempData;
    size_t                  uBuffDataSize   = 0;
    size_t                  uPakSize        = 0;

    assert(sizeof(S2C_SYNC_BUFF_LIST) <= MAX_EXTERNAL_PACKAGE_SIZE);

    pSyncBuffData->byProtocolID     = s2c_sync_buff_list;
    pSyncBuffData->dwCharacterID    = pPlayer->m_dwID;

    bRetCode = pPlayer->m_BuffList.GetTransferData(
        pSyncBuffData->byData, MAX_EXTERNAL_PACKAGE_SIZE - (DWORD)sizeof(S2C_SYNC_BUFF_LIST), &uBuffDataSize
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    uPakSize = (DWORD)sizeof(S2C_SYNC_BUFF_LIST) + uBuffDataSize;

    pSyncBuffData->wDataSize    = (WORD)uBuffDataSize;
    pSyncBuffData->wSize        = (WORD)uPakSize;

    Send(pDstPlayer->m_nConnIndex, m_byTempData, uPakSize);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBuffSingle(
    KCharacter* pCharacter, int nIndex,
    DWORD dwBuffID, int nLevel, DWORD dwRecipeKey[],
    int nStackNum, int nEndFrame
)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KPlayer*                pPlayer         = NULL;
    KTeam*                  pTeam           = NULL;
    S2C_SYNC_BUFF_SINGLE    SyncBuffSingle;
    KTeamBroadcast          Func;

    bRetCode = IS_PLAYER(pCharacter->m_dwID);
    KG_PROCESS_SUCCESS(!bRetCode);

    pPlayer = (KPlayer*)pCharacter;

    SyncBuffSingle.byProtocolID  = s2c_sync_buff_single;
    SyncBuffSingle.dwCharacterID = pPlayer->m_dwID;
    SyncBuffSingle.nIndex        = nIndex;
    SyncBuffSingle.dwBuffID      = dwBuffID;
    SyncBuffSingle.byLevel       = (BYTE)nLevel;
    assert(MAX_BUFF_REICPE_COUNT == 1);
    SyncBuffSingle.dwRecipeKey   = dwRecipeKey[0];
    SyncBuffSingle.byStackNum    = (BYTE)nStackNum;
    SyncBuffSingle.nEndFrame     = nEndFrame;

    Send(pPlayer->m_nConnIndex, &SyncBuffSingle, sizeof(S2C_SYNC_BUFF_SINGLE));

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KG_PROCESS_SUCCESS(pTeam == NULL);

    Func.m_dwTeamID   = pPlayer->m_dwTeamID;
    Func.m_dwExceptID = pPlayer->m_dwID;
    Func.m_pvData     = (void*)&SyncBuffSingle;
    Func.m_uSize      = sizeof(S2C_SYNC_BUFF_SINGLE);
    
    g_pSO3World->m_TeamServer.Traverse(Func);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncRepresentState(KCharacter* pCharacter)
{
    int                         nResult         = false;
    KScene*                     pScene          = NULL;
    KBroadcastFunc              BroadcastFunc;
    S2C_SYNC_REPRESENT_STATE    SyncPak;
    
    assert(pCharacter);
    KG_PROCESS_ERROR(pCharacter->m_pScene);

    SyncPak.byProtocolID    = s2c_sync_represent_state;
    SyncPak.dwCharacterID   = pCharacter->m_dwID;
    SyncPak.wRepresentState = (WORD)pCharacter->m_dwRepresentState;

	BroadcastFunc.m_pvData             = &SyncPak;
	BroadcastFunc.m_uSize              = (DWORD)sizeof(SyncPak);
    BroadcastFunc.m_dwExclusive        = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncKungfuMount(int nConnIndex, DWORD dwKungfuID, DWORD dwKungfuLevel)
{
    int                      nResult  = false;
    int                      nRetCode = false;
    S2C_SYNC_KUNGFU_MOUNT    Pak;

    Pak.byProtocolID = s2c_sync_kungfu_mount;
    Pak.dwID         = dwKungfuID;
    Pak.dwLevel      = dwKungfuLevel;

    nRetCode = Send(nConnIndex, &Pak, sizeof(Pak));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSkillBeatBack(KCharacter* pCharacter, int nBeatBackFrame, BOOL bBreak)
{
    int                      nResult        = false;
    KScene*                  pScene         = NULL;
    KBroadcastFunc           BroadcastFunc;
    S2C_SKILL_BEAT_BACK      SyncPak;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);

    SyncPak.byProtocolID    = s2c_skill_beat_back;
    SyncPak.dwCharacterID   = pCharacter->m_dwID;
    SyncPak.nBeatBackFrame  = nBeatBackFrame;
	SyncPak.bBreak			= bBreak;

	BroadcastFunc.m_pvData           = &SyncPak;
	BroadcastFunc.m_uSize            = (DWORD)sizeof(SyncPak);
    BroadcastFunc.m_dwExclusive      = pCharacter->m_dwOwner;

    pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncSheathState(KCharacter* pCharacter)
{
    int                         nResult     = false;
    KScene*                     pScene      = NULL;
    KBroadcastFunc              BroadcastFunc;
    S2C_SYNC_SHEATH_STATE       SyncPak;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);

    SyncPak.byProtocolID    = s2c_sync_sheath_state;
    SyncPak.dwCharacterID   = pCharacter->m_dwID;
    SyncPak.bSheathFlag     = (bool)pCharacter->m_bSheathFlag;

	BroadcastFunc.m_pvData            = &SyncPak;
	BroadcastFunc.m_uSize             = (DWORD)sizeof(SyncPak);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;

	pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    nResult = true;
Exit0:
    return nResult;
}

// ---------------- 数值同步 -------------------------------------

BOOL KPlayerServer::DoSyncSelfMaxLMRS(KPlayer* pPlayer)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    BOOL                    bValuaChanged   = false;
    S2C_SYNC_SELF_MAX_LMRS  Pak;

    bValuaChanged |= (pPlayer->m_nMaxLife       != pPlayer->m_nLastSyncSelfMaxLife);
    bValuaChanged |= (pPlayer->m_nMaxMana       != pPlayer->m_nLastSyncSelfMaxMana);

    KG_PROCESS_ERROR(bValuaChanged);

    pPlayer->m_nLastSyncSelfMaxLife     = pPlayer->m_nMaxLife;
    pPlayer->m_nLastSyncSelfMaxMana     = pPlayer->m_nMaxMana;

    Pak.byProtocolID = s2c_sync_self_max_lmrs;
    Pak.nMaxLife     = pPlayer->m_nMaxLife;
    Pak.nMaxMana     = pPlayer->m_nMaxMana;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncSelfCurrentLMRS(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    BOOL                        bValuaChanged   = false;
    S2C_SYNC_SELF_CURRENT_LMRS  Pak;

    bValuaChanged |= (pPlayer->m_nCurrentLife       != pPlayer->m_nLastSyncSelfCurrentLife);
    bValuaChanged |= (pPlayer->m_nCurrentMana       != pPlayer->m_nLastSyncSelfCurrentMana);

    KG_PROCESS_ERROR(bValuaChanged);

    pPlayer->m_nLastSyncSelfCurrentLife     = pPlayer->m_nCurrentLife;
    pPlayer->m_nLastSyncSelfCurrentMana     = pPlayer->m_nCurrentMana;

    Pak.byProtocolID     = s2c_sync_self_current_lmrs;
    Pak.nCurrentLife     = pPlayer->m_nCurrentLife;
    Pak.nCurrentMana     = pPlayer->m_nCurrentMana;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncSelfWeakInfo(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    BOOL                        bValuaChanged   = false;
    S2C_SYNC_SELF_WEAK_INFO     Pak;

    bValuaChanged |= (pPlayer->m_nUpperWeakLevel  != pPlayer->m_nLastSyncSelfUpperWeak);
    bValuaChanged |= (pPlayer->m_nMiddleWeakLevel != pPlayer->m_nLastSyncSelfMiddleWeak);
    bValuaChanged |= (pPlayer->m_nLowerWeakLevel  != pPlayer->m_nLastSyncSelfLowerWeak);

    KG_PROCESS_SUCCESS(!bValuaChanged);

    pPlayer->m_nLastSyncSelfUpperWeak  = pPlayer->m_nUpperWeakLevel;
    pPlayer->m_nLastSyncSelfMiddleWeak = pPlayer->m_nMiddleWeakLevel;
    pPlayer->m_nLastSyncSelfLowerWeak  = pPlayer->m_nLowerWeakLevel;

    Pak.byProtocolID    = s2c_sync_self_weak_info;
    Pak.byUpper         = (BYTE)pPlayer->m_nUpperWeakLevel;
    Pak.byMiddle        = (BYTE)pPlayer->m_nMiddleWeakLevel;
    Pak.byLower         = (BYTE)pPlayer->m_nLowerWeakLevel;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetMaxLMR(KPlayer* pPlayer)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    BOOL                                bValuaChanged   = false;
    KCharacter*                         pTarget         = NULL;
    TARGET_TYPE                         eTargetType     = ttInvalid;
    S2C_SYNC_SELECT_CHARACTER_MAX_LMR   Pak;

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
	KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

	bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pTarget && pTarget->m_dwID != pPlayer->m_dwID);

    bValuaChanged |= (pPlayer->m_nLastSyncTargetMaxLife  !=  pTarget->m_nMaxLife);
    bValuaChanged |= (pPlayer->m_nLastSyncTargetMaxMana  !=  pTarget->m_nMaxMana);

    KG_PROCESS_ERROR(bValuaChanged);

    pPlayer->m_nLastSyncTargetMaxLife     = pTarget->m_nMaxLife;
    pPlayer->m_nLastSyncTargetMaxMana     = pTarget->m_nMaxMana;

    Pak.byProtocolID    = s2c_sync_select_character_max_lmr;
    Pak.dwCharacterID   = pTarget->m_dwID;
    Pak.nMaxLife        = pTarget->m_nMaxLife;
    Pak.nMaxMana        = pTarget->m_nMaxMana;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetCurrentLMR(KPlayer* pPlayer)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    BOOL                                    bValuaChanged   = false;
    KCharacter*                             pTarget         = NULL;
    int                                     nLifePercent    = 0;
    int                                     nManaPercent    = 0;
    TARGET_TYPE                             eTargetType     = ttInvalid;
    S2C_SYNC_SELECT_CHARACTER_CURRENT_LMR   Pak;

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
	KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

	bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pTarget && pTarget->m_dwID != pPlayer->m_dwID);

    if (pTarget->m_nMaxLife > 0)
    {
        nLifePercent = pTarget->m_nCurrentLife * UCHAR_MAX / pTarget->m_nMaxLife;

        if (pTarget->m_eMoveState != cmsOnDeath && nLifePercent * pTarget->m_nMaxLife / UCHAR_MAX <= 0)
        {
            // 特殊处理,一面客户端看到一个血量为0却没死的NPC
            nLifePercent = UCHAR_MAX / pTarget->m_nMaxLife;
            if (nLifePercent * pTarget->m_nMaxLife < UCHAR_MAX)
            {
                nLifePercent++;
            }
        }
    }

    if (pTarget->m_nMaxMana > 0)
        nManaPercent = pTarget->m_nCurrentMana * UCHAR_MAX / pTarget->m_nMaxMana;

    bValuaChanged |= (pPlayer->m_nLastSyncTargetLifePercent != nLifePercent);
    bValuaChanged |= (pPlayer->m_nLastSyncTargetManaPercent != nManaPercent);

    KG_PROCESS_ERROR(bValuaChanged);

    pPlayer->m_nLastSyncTargetLifePercent = nLifePercent;
    pPlayer->m_nLastSyncTargetManaPercent = nManaPercent;

    Pak.byProtocolID     = s2c_sync_select_character_current_lmr;
    Pak.dwCharacterID    = pTarget->m_dwID;
    Pak.byLifePercent    = (BYTE)nLifePercent;
    Pak.byManaPercent    = (BYTE)nManaPercent;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetWeakInfo(KPlayer* pPlayer)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    BOOL                                bValuaChanged   = false;
    TARGET_TYPE                         eTargetType     = ttInvalid;
    KCharacter*                         pTarget         = NULL;
    S2C_SYNC_SELECT_CHARACTER_WEAK_INFO Pak;

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pTarget && pTarget->m_dwID != pPlayer->m_dwID);

    bValuaChanged |= (pTarget->m_nUpperWeakLevel  != pPlayer->m_nLastSyncTargetUpperWeak);
    bValuaChanged |= (pTarget->m_nMiddleWeakLevel != pPlayer->m_nLastSyncTargetMiddleWeak);
    bValuaChanged |= (pTarget->m_nLowerWeakLevel  != pPlayer->m_nLastSyncTargetLowerWeak);

    KG_PROCESS_SUCCESS(!bValuaChanged);

    pPlayer->m_nLastSyncTargetUpperWeak  = pTarget->m_nUpperWeakLevel;
    pPlayer->m_nLastSyncTargetMiddleWeak = pTarget->m_nMiddleWeakLevel;
    pPlayer->m_nLastSyncTargetLowerWeak  = pTarget->m_nLowerWeakLevel;

    Pak.byProtocolID    = s2c_sync_select_character_weak_info;
    Pak.dwCharacterID   = pTarget->m_dwID;
    Pak.byUpper         = (BYTE)pTarget->m_nUpperWeakLevel;
    Pak.byMiddle        = (BYTE)pTarget->m_nMiddleWeakLevel;
    Pak.byLower         = (BYTE)pTarget->m_nLowerWeakLevel;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetTargetMaxLMR(KPlayer* pPlayer)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    BOOL                                bValuaChanged   = false;
    KCharacter*                         pTarget         = NULL;
    KCharacter*                         pTargetTarget   = NULL;
    TARGET_TYPE                         eTargetType     = ttInvalid;
    S2C_SYNC_SELECT_CHARACTER_MAX_LMR   Pak;

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTarget);

    eTargetType = pTarget->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pTarget->m_SelectTarget.GetTarget(&pTargetTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTargetTarget);

    KG_PROCESS_SUCCESS(pTargetTarget->m_dwID == pPlayer->m_dwID);

    bValuaChanged |= (pTargetTarget->m_dwID != pPlayer->m_dwLastSyncTargetTargetID);
    bValuaChanged |= (pTargetTarget->m_nMaxLife != pPlayer->m_nLastSyncTargetTargetMaxLife);
    bValuaChanged |= (pTargetTarget->m_nMaxMana != pPlayer->m_nLastSyncTargetTargetMaxMana);

    KG_PROCESS_SUCCESS(!bValuaChanged);

    pPlayer->m_dwLastSyncTargetTargetID     = pTargetTarget->m_dwID;
    pPlayer->m_nLastSyncTargetTargetMaxLife = pTargetTarget->m_nMaxLife;
    pPlayer->m_nLastSyncTargetTargetMaxMana = pTargetTarget->m_nMaxMana;

    Pak.byProtocolID    = s2c_sync_select_character_max_lmr;
    Pak.dwCharacterID   = pTargetTarget->m_dwID;
    Pak.nMaxLife        = pTargetTarget->m_nMaxLife;
    Pak.nMaxMana        = pTargetTarget->m_nMaxMana;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTargetTargetCurrentLMR(KPlayer* pPlayer)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    BOOL                                    bValueChange    = false;
    KCharacter*                             pTarget         = NULL;
    KCharacter*                             pTargetTarget   = NULL;
    TARGET_TYPE                             eTargetType     = ttInvalid;
    int                                     nLifePercent    = 0;
    int                                     nManaPercent    = 0;
    S2C_SYNC_SELECT_CHARACTER_CURRENT_LMR   Pak;

    eTargetType = pPlayer->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pPlayer->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTarget);

    eTargetType = pTarget->m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttPlayer);

    bRetCode = pTarget->m_SelectTarget.GetTarget(&pTargetTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTargetTarget);

    KG_PROCESS_SUCCESS(pTargetTarget->m_dwID == pPlayer->m_dwID);

    if (pTargetTarget->m_nMaxLife > 0)
    {
        nLifePercent = pTargetTarget->m_nCurrentLife * UCHAR_MAX / pTargetTarget->m_nMaxLife;

        if (pTargetTarget->m_eMoveState != cmsOnDeath && nLifePercent * pTargetTarget->m_nMaxLife / UCHAR_MAX <= 0)
        {
            // 特殊处理,一面客户端看到一个血量为0却没死的NPC
            nLifePercent = UCHAR_MAX / pTargetTarget->m_nMaxLife;
            if (nLifePercent * pTargetTarget->m_nMaxLife < UCHAR_MAX)
            {
                nLifePercent++;
            }
        }
    }

    if (pTargetTarget->m_nMaxMana > 0)
    {
        nManaPercent = pTargetTarget->m_nCurrentMana * UCHAR_MAX / pTargetTarget->m_nMaxMana;
    }

    bValueChange |= (pPlayer->m_dwLastSyncTargetTargetID != pTargetTarget->m_dwID);
    bValueChange |= (pPlayer->m_nLastSyncTargetTargetLifePercent != nLifePercent);
    bValueChange |= (pPlayer->m_nLastSyncTargetTargetManaPercent != nManaPercent);

    KG_PROCESS_SUCCESS(!bValueChange);

    pPlayer->m_dwLastSyncTargetTargetID         = pTargetTarget->m_dwID;
    pPlayer->m_nLastSyncTargetTargetLifePercent = nLifePercent;
    pPlayer->m_nLastSyncTargetTargetManaPercent = nManaPercent;

    Pak.byProtocolID    = s2c_sync_select_character_current_lmr;
    Pak.dwCharacterID   = pTargetTarget->m_dwID;
    Pak.byLifePercent   = (BYTE)nLifePercent;
    Pak.byManaPercent   = (BYTE)nManaPercent;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamMemberMaxLMR(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2C_SYNC_TEAM_MEMBER_MAX_LMR    Pak;

    Pak.byProtocolID = s2c_sync_team_member_max_lmr;
    Pak.dwMemberID   = pMember->dwMemberID;
    Pak.nMaxLife     = pMember->nMaxLife;
    Pak.nMaxMana     = pMember->nMaxMana;

    bRetCode = PartyBroadcast(dwTeamID, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamMemberCurrentLMRGlobal(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    S2C_SYNC_TEAM_MEMBER_CURRENT_LMR    Pak;

    Pak.byProtocolID    = s2c_sync_team_member_current_lmr;
    Pak.dwMemberID      = pMember->dwMemberID;
    Pak.byLifePercent   = (BYTE)pMember->nLifePercent;
    Pak.byManaPercent   = (BYTE)pMember->nManaPercent;

    bRetCode = PartyBroadcast(dwTeamID, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamMemberCurrentLMRLocal(KPlayer* pPlayer)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    KTeam*                              pTeam           = NULL;
    BOOL                                bValueChanged   = false;
    int                                 nLifePercent    = 0;
    int                                 nManaPercent    = 0;
    S2C_SYNC_TEAM_MEMBER_CURRENT_LMR    Pak;

    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    if (pPlayer->m_nMaxLife > 0)
        nLifePercent = pPlayer->m_nCurrentLife * UCHAR_MAX / pPlayer->m_nMaxLife;

    if (pPlayer->m_nMaxMana > 0)
        nManaPercent = pPlayer->m_nCurrentMana * UCHAR_MAX / pPlayer->m_nMaxMana;

    bValueChanged |= (pPlayer->m_nLastSyncLocalPartyLifePercent != nLifePercent);
    bValueChanged |= (pPlayer->m_nLastSyncLocalPartyManaPercent != nManaPercent);

    KG_PROCESS_ERROR(bValueChanged);

    pPlayer->m_nLastSyncLocalPartyLifePercent = nLifePercent;
    pPlayer->m_nLastSyncLocalPartyManaPercent = nManaPercent;

    Pak.byProtocolID     = s2c_sync_team_member_current_lmr;
    Pak.dwMemberID       = pPlayer->m_dwID;
    Pak.byLifePercent    = (BYTE)nLifePercent;
    Pak.byManaPercent    = (BYTE)nManaPercent;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

	for (int i = 0 ; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
	    {
		    KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

            if (it->dwMemberID == pPlayer->m_dwID)
            {
                it->nLifePercent   = nLifePercent;
                it->nManaPercent   = nManaPercent;
                continue;
            }
            
            if (!pMember)
                continue;

            if (pMember->m_pScene != pPlayer->m_pScene)
                continue;

            Send(pMember->m_nConnIndex, &Pak, sizeof(Pak));
	    }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamMemberMisc(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2C_SYNC_TEAM_MEMBER_MISC   Pak;

    Pak.byProtocolID = s2c_sync_team_member_misc;
    Pak.dwMemberID   = pMember->dwMemberID;
    Pak.PortraitInfo = pMember->PortraitInfo;
    Pak.dwMapID      = pMember->dwMapID;
    Pak.dwForceID    = pMember->dwForceID;
    Pak.byCamp       = (BYTE)pMember->eCamp;
    Pak.byLevel      = (BYTE)pMember->nLevel;
	Pak.byRoleType   = (BYTE)pMember->nRoleType;
    Pak.bDeathFlag   = (bool)pMember->bDeathFlag;

    bRetCode = PartyBroadcast(dwTeamID, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamMemberPositionGlobal(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2C_SYNC_TEAM_MEMBER_POSITION   Pak;

    Pak.byProtocolID    = s2c_sync_team_member_position;
    Pak.dwMemberID      = pMember->dwMemberID;
    Pak.dwMapID         = pMember->dwMapID;
    Pak.nMapCopyIndex   = pMember->nMapCopyIndex;
    Pak.nPosX           = pMember->nPosX;
    Pak.nPosY           = pMember->nPosY;

    bRetCode = PartyBroadcast(dwTeamID, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamMemberPositionLocal(KPlayer* pPlayer)
{
    BOOL                             bResult        = false;
    BOOL                             bRetCode       = false;
    KTeam*                           pTeam          = NULL;
    BOOL                             bValueChanged  = false;
    S2C_SYNC_TEAM_MEMBER_POSITION    Pak;

    KG_PROCESS_ERROR(pPlayer->m_pScene);
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    bValueChanged |= (pPlayer->m_pScene->m_dwMapID != pPlayer->m_dwLastSyncLocalPartyMemberMapID);
    bValueChanged |= (pPlayer->m_pScene->m_nCopyIndex != pPlayer->m_nLastSyncLocalPartyMemberMapCopyIndex);
    bValueChanged |= (pPlayer->m_nX != pPlayer->m_nLastSyncLocalPartyMemberPosX);
    bValueChanged |= (pPlayer->m_nY != pPlayer->m_nLastSyncLocalPartyMemberPosY);

    KG_PROCESS_ERROR(bValueChanged);

    pPlayer->m_dwLastSyncLocalPartyMemberMapID         = pPlayer->m_pScene->m_dwMapID;
    pPlayer->m_nLastSyncLocalPartyMemberMapCopyIndex   = pPlayer->m_pScene->m_nCopyIndex;
    pPlayer->m_nLastSyncLocalPartyMemberPosX           = pPlayer->m_nX;
    pPlayer->m_nLastSyncLocalPartyMemberPosY           = pPlayer->m_nY;

    Pak.byProtocolID   = s2c_sync_team_member_position;
    Pak.dwMemberID     = pPlayer->m_dwID;
    Pak.dwMapID        = pPlayer->m_pScene->m_dwMapID;
    Pak.nMapCopyIndex  = pPlayer->m_pScene->m_nCopyIndex;
    Pak.nPosX          = pPlayer->m_nX;
    Pak.nPosY          = pPlayer->m_nY;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);

	for (int i = 0; i < pTeam->nGroupNum; i++)
	{
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

            if (it->dwMemberID == pPlayer->m_dwID)
            {
                it->dwMapID        = pPlayer->m_pScene->m_dwMapID;
                it->nMapCopyIndex  = pPlayer->m_pScene->m_nCopyIndex;
                it->nPosX          = pPlayer->m_nX;
                it->nPosY          = pPlayer->m_nY;
                continue;
            }

            if (!pMember)
                continue;

            if (pMember->m_pScene != pPlayer->m_pScene)
                continue;

            Send(pMember->m_nConnIndex, &Pak, sizeof(Pak));
        }
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBroadcastCharacterLife(KCharacter* pCharacter)
{
	BOOL				            bResult			= false;
    KScene*                         pScene          = NULL;
    int                             nLifePercent    = 0;
    KBroadcastFunc                  BroadcastFunc;
	S2C_BROADCAST_CHARACTER_LIFE	Pak;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);

    if (pCharacter->m_nMaxLife > 0)
    {
	    nLifePercent	= pCharacter->m_nCurrentLife * UCHAR_MAX / pCharacter->m_nMaxLife;
        MAKE_IN_RANGE(nLifePercent, 0, UCHAR_MAX);

        if (pCharacter->m_eMoveState != cmsOnDeath && nLifePercent * pCharacter->m_nMaxLife / UCHAR_MAX <= 0)
        {
            // 特殊处理,以免客户端看到一个血量为0却没死的NPC
            nLifePercent = UCHAR_MAX / pCharacter->m_nMaxLife;
            if (nLifePercent * pCharacter->m_nMaxLife < UCHAR_MAX)
            {
                nLifePercent++;
            }
        }
    }

    if (nLifePercent == pCharacter->m_nLastBroadcastLife)
    {
        goto Exit0;
    }

    pCharacter->m_nLastBroadcastLife = nLifePercent;

	Pak.byProtocolID	    = s2c_broadcast_character_life;
	Pak.dwCharacterID		= pCharacter->m_dwID;
    Pak.byLifePercent       = (BYTE)nLifePercent;

	BroadcastFunc.m_pvData			= &Pak;
	BroadcastFunc.m_uSize			= sizeof(Pak);
    BroadcastFunc.m_dwExcept        = pCharacter->m_dwID;
    BroadcastFunc.m_dwExclusive     = pCharacter->m_dwOwner;

	pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncPlayerTrain(KPlayer* pPlayer)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    S2C_SYNC_PLAYER_TRAIN   Pak;
    
    Pak.byProtocolID        = s2c_sync_player_train;
    Pak.nCurrentTrainValue  = pPlayer->m_nCurrentTrainValue;
    Pak.nUsedTrainValue     = pPlayer->m_nUsedTrainValue;

    bRetCode = Send(pPlayer->m_nConnIndex, &Pak, sizeof(Pak));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBroadcastPlayerSchoolID(KPlayer* pPlayer)
{
    BOOL                            bResult         = false;
    KScene*                         pScene          = NULL;
    S2C_BROADCAST_PLAYER_SCHOOL_ID  PlayerSchoolID;
    KBroadcastFunc                  BroadcastFunc;

    assert(pPlayer);
    
    pScene = pPlayer->m_pScene;
    KG_PROCESS_ERROR(pScene);

    PlayerSchoolID.byProtocolID = s2c_broadcast_player_school_id;
    PlayerSchoolID.dwPlayerID   = pPlayer->m_dwID;
    PlayerSchoolID.dwSchoolID   = pPlayer->m_dwSchoolID;

    BroadcastFunc.m_pvData      = &PlayerSchoolID;
    BroadcastFunc.m_uSize       = sizeof(PlayerSchoolID);
    BroadcastFunc.m_dwExcept    = pPlayer->m_dwID;

    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

// ----------------- party about -------------------------->

BOOL KPlayerServer::DoLoginPartySync(KPlayer* pPlayer)
{
    BOOL                                 bResult        = false;
    int                                  nRetCode       = false;
    KTeam*                               pTeam          = NULL;

    KG_PROCESS_SUCCESS(pPlayer->m_dwTeamID == ERROR_ID);

    nRetCode = DoSyncTeamBaseInfo(pPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTeamBaseInfo(KPlayer* pPlayer, BOOL bAddPartyMemberFlag /* = false */)
{
    int                              nResult            = false;
    int                              nRetCode           = false;
    int                              nMemberIndex       = 0;
    size_t                           uLeftSize          = sizeof(m_byTempData);
    size_t                           uUsedSize          = 0;
    KTeam*                           pTeam              = NULL;
    S2C_SYNC_PLAYER_TEAM_BASE_INFO*  pSyncPartyBaseInfo = (S2C_SYNC_PLAYER_TEAM_BASE_INFO*)m_byTempData;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_PLAYER_TEAM_BASE_INFO) <= uLeftSize);
    uLeftSize -= sizeof(S2C_SYNC_PLAYER_TEAM_BASE_INFO);
    uUsedSize += sizeof(S2C_SYNC_PLAYER_TEAM_BASE_INFO);

    pSyncPartyBaseInfo->byProtocolID          = s2c_sync_player_team_base_info;
    pSyncPartyBaseInfo->dwPartyID             = pPlayer->m_dwTeamID;
    pSyncPartyBaseInfo->dwLeaderID            = pTeam->dwAuthority[tatLeader];
    pSyncPartyBaseInfo->nGroupNum             = pTeam->nGroupNum;
    pSyncPartyBaseInfo->nLootMode             = pTeam->nLootMode;
	pSyncPartyBaseInfo->nRollQuality          = pTeam->nRollQuality;
    pSyncPartyBaseInfo->byCamp                = (BYTE)pTeam->eCamp;
    pSyncPartyBaseInfo->dwDistributeMan       = pTeam->dwAuthority[tatDistribute];
    pSyncPartyBaseInfo->dwMarkMan             = pTeam->dwAuthority[tatMark];
    pSyncPartyBaseInfo->bAddPartyMemberFlag   = (bool)bAddPartyMemberFlag;

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        pSyncPartyBaseInfo->dwFormationLeader[i] = pTeam->MemberGroup[i].dwFormationLeader;
    }

    memcpy(pSyncPartyBaseInfo->dwPartyMark, pTeam->dwMark, sizeof(pSyncPartyBaseInfo->dwPartyMark));

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_PLAYER_TEAM_BASE_INFO::KMEMBER_INFO) <= uLeftSize);
            uLeftSize -= sizeof(S2C_SYNC_PLAYER_TEAM_BASE_INFO::KMEMBER_INFO);
            uUsedSize += sizeof(S2C_SYNC_PLAYER_TEAM_BASE_INFO::KMEMBER_INFO);

            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].dwMemberID       = it->dwMemberID;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byGroupIndex     = (BYTE)i;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].PortraitInfo     = it->PortraitInfo;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].dwMapID          = it->dwMapID;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byForceID        = (BYTE)it->dwForceID;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byLevel          = (BYTE)it->nLevel;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byOnLineFlag     = (BYTE)it->bOnlineFlag;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].nMaxLife         = it->nMaxLife;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].nMaxMana         = it->nMaxMana; 
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byLifePercent    = (BYTE)it->nLifePercent;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byManaPercent    = (BYTE)it->nManaPercent;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].dwMapID          = it->dwMapID;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].nMapCopyIndex    = it->nMapCopyIndex;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].nPosX            = it->nPosX;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].nPosY            = it->nPosY;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byRoleType       = (BYTE)it->nRoleType;
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].byDeathFlag      = (BYTE)it->bDeathFlag;

            strncpy(
                pSyncPartyBaseInfo->MemberInfo[nMemberIndex].szMemberName,
                it->szRoleName, sizeof(pSyncPartyBaseInfo->MemberInfo[nMemberIndex].szMemberName)
            );
            pSyncPartyBaseInfo->MemberInfo[nMemberIndex].szMemberName[sizeof(pSyncPartyBaseInfo->MemberInfo[nMemberIndex].szMemberName) - 1] = '\0';

            nMemberIndex++;
        }
    }

    pSyncPartyBaseInfo->byMemberCount = (BYTE)nMemberIndex;

    KGLOG_PROCESS_ERROR(uUsedSize < USHRT_MAX);
    pSyncPartyBaseInfo->wSize = (WORD)uUsedSize;

    nRetCode = Send(pPlayer->m_nConnIndex, pSyncPartyBaseInfo, uUsedSize);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}


BOOL KPlayerServer::DoInvitePlayerJoinPartyRequest(DWORD dwInviteDst, const char cszInviteSrc[])
{
    int                             nResult         = false;
    int                             nRetCode        = false;
    KPlayer*                        pInviteDst      = NULL;
    S2C_INVITE_JOIN_TEAM_REQUEST    PartyInvite;

    pInviteDst = g_pSO3World->m_PlayerSet.GetObj(dwInviteDst);
    KG_PROCESS_ERROR(pInviteDst);

    PartyInvite.byProtocolID         = s2c_invite_join_team_request;
    strncpy(PartyInvite.szInviteSrc, cszInviteSrc, sizeof(PartyInvite.szInviteSrc));
    PartyInvite.szInviteSrc[sizeof(PartyInvite.szInviteSrc) - 1] = '\0';

    nRetCode = Send(pInviteDst->m_nConnIndex, &PartyInvite, sizeof(PartyInvite));
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoApplyJoinTeamRequest(DWORD dwLeader, const char cszApplySrc[])
{
    int                             nResult         = false;
    int                             nRetCode        = false;
    KPlayer*                        pPartyLeader    = NULL;
    S2C_APPLY_JOIN_TEAM_REQUEST     PartyApply;

    pPartyLeader = g_pSO3World->m_PlayerSet.GetObj(dwLeader);
    KG_PROCESS_ERROR(pPartyLeader);

    PartyApply.byProtocolID         = s2c_apply_join_team_request;
    strncpy(PartyApply.szApplySrc, cszApplySrc, sizeof(PartyApply.szApplySrc));
    PartyApply.szApplySrc[sizeof(PartyApply.szApplySrc) - 1] = '\0';

    nRetCode = Send(pPartyLeader->m_nConnIndex, &PartyApply, sizeof(PartyApply));
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTeamAddMemberNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwMemberID)
{
    int                         nResult     = false;
    KPlayer*                    pPlayer     = NULL;
    KTEAM_MEMBER_INFO*          pMemberInfo = NULL;
    S2C_TEAM_ADD_MEMBER_NOTIFY  Notify;
    KTeamBroadcast              Func;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(dwPartyID, dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Notify.byProtocolID = s2c_team_add_member_notify;
    Notify.dwPartyID    = dwPartyID;
    Notify.nGroupIndex  = nGroupIndex;
    Notify.dwMemberID   = dwMemberID;

    strncpy(Notify.szMemberName, pMemberInfo->szRoleName, sizeof(Notify.szMemberName));
    Notify.szMemberName[sizeof(Notify.szMemberName) - 1] = '\0';

    Func.m_dwExceptID = dwMemberID;
    Func.m_dwTeamID   = dwPartyID;
    Func.m_pvData     = (void*)&Notify;
    Func.m_uSize      = sizeof(S2C_TEAM_ADD_MEMBER_NOTIFY);

    g_pSO3World->m_TeamServer.Traverse(Func);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTeamDelMemberNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwMemberID)
{
    int                         nResult                 = false;
    DWORD                       dwPlayerID              = 0;
    KPlayer*                    pPlayer                 = NULL;
    S2C_TEAM_DEL_MEMBER_NOTIFY  PartyDelMemberNotify;
    KTeamBroadcast              Func;

    PartyDelMemberNotify.byProtocolID = s2c_team_del_member_notify;
    PartyDelMemberNotify.dwPartyID    = dwPartyID;
    PartyDelMemberNotify.nGroupIndex  = nGroupIndex;
    PartyDelMemberNotify.dwMemberID   = dwMemberID;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwMemberID);

    if (pPlayer)
    {
        Send(pPlayer->m_nConnIndex, &PartyDelMemberNotify, sizeof(PartyDelMemberNotify));
    }
    //else: 玩家可能不在本服务器上

    // 为什么不把上面的这个Send纳入下面的循环? 因为这个玩家可能已经从列表中删除了,循环就遍历不到这个玩家.

    Func.m_dwTeamID     = dwPartyID;
    Func.m_dwExceptID   = dwMemberID;
    Func.m_pvData       = (void*)&PartyDelMemberNotify;
    Func.m_uSize        = sizeof(PartyDelMemberNotify);

    g_pSO3World->m_TeamServer.Traverse(Func);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTeamChangeAuthorityNotify(DWORD dwPartyID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
{
    int                                 nResult     = false;
    S2C_TEAM_CHANGE_AUTHORITY_NOTIFY    Notify;
    KTeamBroadcast                      Func;

    Notify.byProtocolID   = s2c_team_change_authority_notify;
    Notify.dwPartyID      = dwPartyID;
    Notify.byType         = (BYTE)eType;
    Notify.dwNewID        = dwTargetID;

    // 逐个通知各个队员，注: 队员可能既不在新队长附近，也不在旧队长附近
    // 客户端的 "xxx成为队长"的消息是这里通知的，而上面主要是设置队长的标旗
    Func.m_dwTeamID = dwPartyID;
    Func.m_pvData   = (void*)&Notify;
    Func.m_uSize    = sizeof(Notify);

    g_pSO3World->m_TeamServer.Traverse(Func);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTeamSetLootModeNotify(DWORD dwPartyID, int nLootMode, int nRollQuality)
{
    int                             nResult     = false;
    S2C_TEAM_SET_LOOT_MODE_NOTIFY   Notify;

    Notify.byProtocolID     = s2c_team_set_loot_mode_notify;
    Notify.dwPartyID        = dwPartyID;
    Notify.nLootMode		= nLootMode;
	Notify.nRollQuality		= nRollQuality;

    PartyBroadcast(dwPartyID, &Notify, sizeof(Notify));

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTeamSetFormationLeaderNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwNewFormationLeader)
{
    BOOL                                    bResult        = false;
    BOOL                                    bRetCode       = false;
    S2C_TEAM_SET_FORMATION_LEADER_NOTIFY    Notify;

    Notify.byProtocolID         = s2c_team_set_formation_leader_notify;
    Notify.dwPartyID            = dwPartyID;
    Notify.nGourpIndex          = nGroupIndex;
    Notify.dwNewFormationLeader = dwNewFormationLeader;

    PartyBroadcast(dwPartyID, &Notify, sizeof(Notify));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoTeamDisbandNotify(DWORD dwPartyID)
{
    int                     nResult     = false;
    int                     nRetCode    = false;
    S2C_TEAM_DISBAND_NOTIFY Notify;

    Notify.byProtocolID = s2c_team_disband_notify;
    Notify.dwPartyID    = dwPartyID;

    PartyBroadcast(dwPartyID, &Notify, sizeof(Notify));        
    
    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncTeamMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, BOOL bOnlineFlag)
{
    int                                 nResult             = false;
    S2C_SYNC_TEAM_MEMBER_ONLINE_FLAG    MemberOnlineInfo;

    MemberOnlineInfo.byProtocolID = s2c_sync_team_member_online_flag;
    MemberOnlineInfo.dwPartyID    = dwPartyID;
    MemberOnlineInfo.dwMemberID   = dwMemberID;
    MemberOnlineInfo.bOnlineFlag  = (bool)bOnlineFlag;

    PartyBroadcast(dwPartyID, &MemberOnlineInfo, sizeof(MemberOnlineInfo));

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTeamSetMarkRespond(DWORD dwPartyID, int nMarkType, DWORD dwTargetID)
{
    BOOL                        bResult     = false;
    S2C_TEAM_SET_MARK_RESPOND   SetPartymark;

    SetPartymark.byProtocolID = s2c_team_set_mark_respond;
    SetPartymark.nMarkType    = nMarkType;
    SetPartymark.dwTargetID   = dwTargetID;

    PartyBroadcast(dwPartyID, &SetPartymark, sizeof(SetPartymark));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoTeamCampChange(DWORD dwPartyID, KCAMP eCamp)
{
    BOOL                    bResult         = false;
    S2C_TEAM_CAMP_CHANGE    TeamCampChange;

    TeamCampChange.byProtocolID = s2c_team_camp_change;
    TeamCampChange.byCamp       = (BYTE)eCamp;

    PartyBroadcast(dwPartyID, &TeamCampChange, sizeof(TeamCampChange));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoTeamLevelUpRaidNotify(DWORD dwTeamID, int nGroupNum)
{
    BOOL                            bResult     = false;
    S2C_TEAM_LEVEL_UP_RAID_NOTIFY   Notify;

    Notify.byProtocolID = s2c_team_level_up_raid_notify;
    Notify.dwTeamID     = dwTeamID;
    Notify.byGroupNum   = (BYTE)nGroupNum;

    PartyBroadcast(dwTeamID, &Notify, sizeof(Notify));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoTeamChangeMemberGroupNotify(DWORD dwTeamID, int nSrcGroupIndex, int nDstGroupIndex, DWORD dwMemberID)
{
    BOOL                                bResult = false;
    S2C_TEAM_CHANGE_MEMBER_GROUP_NOTIFY Notify;

    Notify.byProtocolID     = s2c_team_change_member_group_notify;
    Notify.dwTeamID         = dwTeamID;
    Notify.bySrcGroupIndex  = (BYTE)nSrcGroupIndex;
    Notify.byDstGroupIndex  = (BYTE)nDstGroupIndex;
    Notify.dwMemberID       = dwMemberID;

    PartyBroadcast(dwTeamID, &Notify, sizeof(Notify));

    bResult = true;
Exit0:
    return bResult;
}

// <----------------- party about --------------------------


// ----------------- shop about -------------------------->

BOOL KPlayerServer::DoOpenShopNotify(int nConnectIndex, DWORD dwShopID)
{
    int                     nResult     = false;
    int                     nRetCode    = false;
    const KShop*            pcShop      = NULL;
    S2C_OPEN_SHOP_NOTIFY    Notify;

    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);
    KGLOG_PROCESS_ERROR(pcShop->m_pNpc);

    Notify.byProtocolID = s2c_open_shop_notify;
    Notify.dwShopID     = dwShopID;
    Notify.byValidPage  = (BYTE)pcShop->m_dwValidPage;
    Notify.cShopType    = (char)pcShop->m_nShopType;
    Notify.bCanRepair   = (bool)pcShop->m_bCanRepair;
    Notify.dwNpcID      = pcShop->m_pNpc->m_dwID;
    Notify.dwShopTemplateID = pcShop->m_dwTemplateID;

    nRetCode = Send(nConnectIndex, &Notify, sizeof(Notify));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncShopItem(int nConnIndex, const KShop *pcShop, DWORD dwPageIndex, DWORD dwPosIndex, BOOL bSyncEntireItem)
{
    BOOL                    bResult                 = false;
    BOOL                    bRetCode                = false;
    S2C_UPDATE_SHOP_ITEM*   pRespond                = (S2C_UPDATE_SHOP_ITEM *)m_byTempData;
    size_t                  uItemDataSize           = 0;
    const KSHOP_ITEM*       pcShopItem              = NULL;

    assert(dwPageIndex < pcShop->m_dwValidPage);
    KGLOG_PROCESS_ERROR(sizeof(S2C_UPDATE_SHOP_ITEM) + sizeof(KTRAIN_EQUIP_DATA) <= MAX_EXTERNAL_PACKAGE_SIZE);

    pcShopItem  = &pcShop->m_ShopPages[dwPageIndex].ShopItems[dwPosIndex];

    pRespond->byProtocolID         = s2c_update_shop_item;
    pRespond->dwShopID             = pcShop->m_dwShopID;
    pRespond->byPageIndex          = (BYTE)dwPageIndex;
    pRespond->byPosIndex           = (BYTE)dwPosIndex;
    pRespond->byItemTemplateIndex  = (BYTE)pcShopItem->nItemTemplateIndex;
    pRespond->dwItemID             = 0;

    if (pcShopItem->pItem)
    {
        pRespond->dwItemID = pcShopItem->pItem->m_dwID;
    }

    pRespond->nCount        = pcShopItem->nCount;
    
    uItemDataSize = 0;
    if (pcShopItem->pItem && bSyncEntireItem)
    {
        bRetCode = pcShopItem->pItem->GetBinaryData(
            &uItemDataSize, m_byTempData + sizeof(S2C_UPDATE_SHOP_ITEM), 
            MAX_EXTERNAL_PACKAGE_SIZE - sizeof(S2C_UPDATE_SHOP_ITEM)
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    pRespond->wItemDataLength   = (WORD)uItemDataSize;
    pRespond->wSize             = (WORD)(sizeof(S2C_UPDATE_SHOP_ITEM) + uItemDataSize);

    bRetCode = Send(nConnIndex, pRespond, sizeof(S2C_UPDATE_SHOP_ITEM) + uItemDataSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}
// <----------------- shop about --------------------------

BOOL KPlayerServer::DoTradingInviteRequest(KPlayer* pTradingInviteSrc, KPlayer *pTradingInviteDst)
{   
    int                         nResult         = false;
    int                         nRetCode        = false;
    S2C_TRADING_INVITE_REQUEST  InviteRequest;
    
    InviteRequest.byProtocolID          = s2c_trading_invite_request;
    InviteRequest.dwTradingInviteSrc    = pTradingInviteSrc->m_dwID;
    
    nRetCode = Send(pTradingInviteDst->m_nConnIndex, &InviteRequest, sizeof(InviteRequest));
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoOpenTradingBoxNotify(int nConnIndex, DWORD dwOtherPlayer)
{
    int                         nResult     = false;
    int                         nRetCode    = false;
    KPlayer*                    pPlayer     = NULL;
    S2C_OPEN_TRADING_BOX_NOTIFY Notify;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwOtherPlayer);
    KG_PROCESS_ERROR(pPlayer);

    Notify.byProtocolID = s2c_open_trading_box_notify;
    Notify.dwOtherPlayer = dwOtherPlayer;

    nRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoSyncTradingConfirm(int nConnIndex, DWORD dwPlayerID, BOOL bConfirm)
{
    BOOL                      bResult         = false;
    BOOL                      bRetCode        = false;
    S2C_SYNC_TRADING_CONFIRM  TradingConfirm;

    TradingConfirm.byProtocolID = s2c_sync_trading_confirm;
    TradingConfirm.dwPlayerID   = dwPlayerID;
    TradingConfirm.bConfirm     = bConfirm;

    bRetCode = Send(nConnIndex, &TradingConfirm, sizeof(TradingConfirm));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTradingItemData(
    int nConnIndex, KItem* pItem, DWORD dwItemOwnerID, DWORD dwGridIndex, 
    DWORD dwBoxIndex, DWORD dwPosIndex, int nKey
)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2C_SYNC_TRADING_ITEM_DATA* pItemData   = (S2C_SYNC_TRADING_ITEM_DATA*)m_byTempData;
    size_t                      uItemSize   = 0;    
    
    assert(sizeof(S2C_SYNC_TRADING_ITEM_DATA) <= MAX_EXTERNAL_PACKAGE_SIZE);

    pItemData->byProtocolID   = s2c_sync_trading_item_data;
    pItemData->byBoxIndex     = (BYTE)dwBoxIndex;
    pItemData->byPosIndex     = (BYTE)dwPosIndex;
    pItemData->byGridIndex    = (BYTE)dwGridIndex;
    pItemData->dwItemOwnerID  = dwItemOwnerID;
    pItemData->nKey           = nKey;    
    
    if (pItem)
    {
        if (pItem->IsStackable())
        {
            pItemData->byStackNum = (BYTE)pItem->GetStackNum();
        }
        else
        {
            pItemData->byStackNum = (BYTE)pItem->GetCurrentDurability();
        }

        bRetCode = pItem->GetBinaryData(
            &uItemSize, m_byTempData + sizeof(S2C_SYNC_TRADING_ITEM_DATA), 
            MAX_ITEM_SYNC_PROTOCOL_SIZE - sizeof(S2C_SYNC_TRADING_ITEM_DATA)
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        pItemData->byStackNum = 0;
    }
    
    pItemData->wSize      = (WORD)(sizeof(S2C_SYNC_TRADING_ITEM_DATA) + uItemSize);

    bRetCode = Send(nConnIndex, pItemData, pItemData->wSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncTradingMoney(int nConnIndex, DWORD dwPlayerID, int nMoney, int nKey)
{
    int                     nResult     = false;
    int                     nRetCode    = false;
    S2C_SYNC_TRADING_MONEY  SyncTradingMoney;
    
    SyncTradingMoney.byProtocolID   = s2c_sync_trading_money;
    SyncTradingMoney.dwPlayerID     = dwPlayerID;
    SyncTradingMoney.nMoney         = nMoney;
    SyncTradingMoney.nKey           = nKey;

    nRetCode = Send(nConnIndex, &SyncTradingMoney, sizeof(SyncTradingMoney));
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = true;
Exit0:
    return nResult;
}

BOOL KPlayerServer::DoTradingComplete(int nConnIndex, BOOL bComplete)
{
    int                     nResult = false;
    int                     nRetCode = false;
    S2C_TRADING_COMPLETE    TradingComplete;

    TradingComplete.byProtocolID = s2c_trading_complete;
    TradingComplete.bComplete    = bComplete;

    nRetCode = Send(nConnIndex, &TradingComplete, sizeof(TradingComplete));
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

// 通知客户端某个角色被移除了
BOOL KPlayerServer::DoRemoveCharacter(KCharacter* pCharacter, BOOL bKilled)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_REMOVE_CHARACTER    RemoveCharacter;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);
    assert(pCharacter->m_pRegion);

	RemoveCharacter.byProtocolID    = s2c_remove_character;
	RemoveCharacter.dwCharacterID   = pCharacter->m_dwID;
	RemoveCharacter.bKilled         = bKilled;

	BroadcastFunc.m_pvData            = &RemoveCharacter;
	BroadcastFunc.m_uSize             = sizeof(RemoveCharacter);
    BroadcastFunc.m_dwExclusive       = pCharacter->m_dwOwner;

	pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoRemoveDoodad(KDoodad* pDoodad)
{
    BOOL                    bResult         = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_REMOVE_DOODAD       RemoveDoodad;
    
    assert(pDoodad);
    assert(pDoodad->m_pScene);

	RemoveDoodad.byProtocolID   = s2c_remove_doodad;
	RemoveDoodad.dwID           = pDoodad->m_dwID;

	BroadcastFunc.m_pvData      = &RemoveDoodad;
	BroadcastFunc.m_uSize       = sizeof(RemoveDoodad);

	pScene = pDoodad->m_pScene;
	pScene->Broadcast(pDoodad->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoFrameSignal(int nConnIndex)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	S2C_FRAME_SIGNAL    FrameSignal;

	FrameSignal.byProtocolID = s2c_frame_signal;
	FrameSignal.nFrame       = g_pSO3World->m_nGameLoop;

	bRetCode = Send(nConnIndex, &FrameSignal, sizeof(FrameSignal));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncMoney(int nConnIndex, int nMoney, BOOL bShowMsg)
{
	BOOL                bResult       = false;
	BOOL                bRetCode      = false;
	S2C_SYNC_MONEY      SyncMoney;

	SyncMoney.byProtocolID = s2c_sync_money;
	SyncMoney.nMoney = nMoney;
	SyncMoney.bShowMsg = bShowMsg;

	bRetCode = Send(nConnIndex, &SyncMoney, sizeof(SyncMoney));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncContribution(int nConnIndex, int nContribution)
{
    BOOL                    bResult    = false;
    BOOL                    bRetCode   = false;
    S2C_SYNC_CONTRIBUTION   SyncContribution;

    SyncContribution.byProtocolID   = s2c_sync_contribution;
    SyncContribution.nContribution  = nContribution;

    bRetCode = Send(nConnIndex, &SyncContribution, sizeof(SyncContribution));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 同步道具数据
BOOL KPlayerServer::DoSyncItemData(int nConnIndex, DWORD dwPlayerID, KItem* pItem, DWORD dwBox, DWORD dwX)
{
    BOOL                bRetCode        = false;
    BOOL                bResult         = false;
    size_t              uBufferSize     = MAX_EXTERNAL_PACKAGE_SIZE - sizeof(S2C_SYNC_ITEM_DATA);
    size_t              uItemDataLen    = 0;
    S2C_SYNC_ITEM_DATA* pSyncItemData   = (S2C_SYNC_ITEM_DATA *)m_byTempData;

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_ITEM_DATA) + sizeof(KTRAIN_EQUIP_DATA) <= MAX_EXTERNAL_PACKAGE_SIZE);

    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(dwBox < ibTotal);
    KGLOG_PROCESS_ERROR(dwX < MAX_ITEM_PACKAGE_SIZE);

    pSyncItemData->byProtocolID = s2c_sync_item_data;
    pSyncItemData->dwPlayerID   = dwPlayerID;
	pSyncItemData->byBox        = (BYTE)dwBox;
	pSyncItemData->byX          = (BYTE)dwX;
    pSyncItemData->dwItemID     = pItem->m_dwID;

    bRetCode = pItem->GetBinaryData(&uItemDataLen, pSyncItemData->byBuff, uBufferSize);
    KGLOG_PROCESS_ERROR(bRetCode);

    pSyncItemData->wSize = (WORD)(sizeof(S2C_SYNC_ITEM_DATA) + uItemDataLen);

	bRetCode = Send(nConnIndex, pSyncItemData, pSyncItemData->wSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 同步道具删除
BOOL KPlayerServer::DoSyncItemDestroy(int nConnIndex, DWORD dwOwnerID, DWORD dwBox, DWORD dwX)
{
    BOOL                    bRetCode = false;
    BOOL                    bResult  = false;
	S2C_SYNC_ITEM_DESTROY   SyncItemDestroy;

	KGLOG_PROCESS_ERROR(dwBox < ibTotal);

	SyncItemDestroy.byProtocolID     = s2c_sync_item_destroy;
    SyncItemDestroy.dwOwnerID        = dwOwnerID;
	SyncItemDestroy.byBoX            = (BYTE)dwBox;
	SyncItemDestroy.byX              = (BYTE)dwX;

	bRetCode = Send(nConnIndex, &SyncItemDestroy, sizeof(SyncItemDestroy));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

// 交换装备
BOOL KPlayerServer::DoExchangeItem(int nConnIndex, DWORD dwBox1, DWORD dwX1, DWORD dwBox2, DWORD dwX2, DWORD dwAmount)
{
    BOOL                bRetCode = false;
    BOOL                bResult  = false;
	S2C_EXCHANGE_ITEM   ExchangeItem;

	ExchangeItem.byProtocolID   = s2c_exchange_item;
	ExchangeItem.byBox1         = (BYTE)dwBox1;
	ExchangeItem.byX1           = (BYTE)dwX1;
	ExchangeItem.byBox2         = (BYTE)dwBox2;
	ExchangeItem.byX2           = (BYTE)dwX2;
	ExchangeItem.wAmount        = (WORD)dwAmount;

	bRetCode = Send(nConnIndex, &ExchangeItem, sizeof(ExchangeItem));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncEquipRepresent(KPlayer* pPlayer, int nIndex, DWORD dwRepresentID)
{
    BOOL                     bResult         = false;
    KScene*                  pScene          = NULL;
    KBroadcastFunc           BroadcastFunc;
    S2C_SYNC_EQUIP_REPRESENT Pak;
    
    assert(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_pScene);

	Pak.byProtocolID    = s2c_sync_equip_represent;
	Pak.dwPlayerID      = pPlayer->m_dwID;
    Pak.byIndex         = (BYTE)nIndex;
    Pak.wRepresentID    = (WORD)dwRepresentID;

	BroadcastFunc.m_pvData           = &Pak;
	BroadcastFunc.m_uSize            = sizeof(Pak);

	pScene = pPlayer->m_pScene;
    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

// 同步道具获得的消息
BOOL KPlayerServer::DoAddItemNotify(KPlayer* pAddItemPlayer, DWORD dwItemID, int nCount, int nDestConnIndex)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
    int                 nConnIndex  = nDestConnIndex;
    S2C_ADD_ITEM_NOTIFY AddItemNotify;
    
    assert(pAddItemPlayer);
	KG_PROCESS_ERROR(nCount > 0);

	AddItemNotify.byProtocolID  = s2c_add_item_notify;
	AddItemNotify.dwPlayerID    = pAddItemPlayer->m_dwID;
	AddItemNotify.dwItemID      = dwItemID;
	AddItemNotify.dwCount       = (DWORD)nCount;
    
    if (nConnIndex == -1)
    {
        nConnIndex = pAddItemPlayer->m_nConnIndex;
    }

	bRetCode = Send(nConnIndex, &AddItemNotify, sizeof(AddItemNotify));
	KG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
	return bResult;
}

// 更新物品数量或耐久
BOOL KPlayerServer::DoUpdateItemAmount(int nConnIndex, DWORD dwBox, DWORD dwX, int nAmount)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
	S2C_UPDATE_ITEM_AMOUNT  UpdateItemAmount;

	UpdateItemAmount.byProtocolID   = s2c_update_item_amount;
	UpdateItemAmount.byBox          = (BYTE)dwBox;
	UpdateItemAmount.byX            = (BYTE)dwX;
	UpdateItemAmount.wAmount        = (WORD)nAmount;

	bRetCode = Send(nConnIndex, &UpdateItemAmount, sizeof(UpdateItemAmount));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}


// 学习生活技能
BOOL KPlayerServer::DoLearnProfessionNotify(int nConnIndex, DWORD dwProfessionID)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
	S2C_LEARN_PROFESSION_NOTIFY LearnProfessionNotify;

	LearnProfessionNotify.byProtocolID = s2c_learn_profession_notify;
	LearnProfessionNotify.dwProfessionID = dwProfessionID;

	bRetCode = Send(nConnIndex, &LearnProfessionNotify, sizeof(LearnProfessionNotify));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoLearnBranchNotify(int nConnIndex, DWORD dwProfessionID, DWORD dwBranchID)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;	
	S2C_LEARN_BRANCH_NOTIFY LearnBranchNotify;

	LearnBranchNotify.byProtocolID = s2c_learn_branch_notify;
	LearnBranchNotify.dwProfessionID = dwProfessionID;
	LearnBranchNotify.dwBranchID = dwBranchID;

	bRetCode = Send(nConnIndex, &LearnBranchNotify, sizeof(LearnBranchNotify));
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    return bResult;
}

// 遗忘技能
BOOL KPlayerServer::DoForgetProfessionNotify(int nConnIndex, DWORD dwProfessionID)
{
    BOOL                         bResult  = false;
    BOOL                         bRetCode = false;
	S2C_FORGET_PROFESSION_NOTIFY ForgetProfessionNotify;

	ForgetProfessionNotify.byProtocolID = s2c_forget_profession_notify;
	ForgetProfessionNotify.dwProfessionID = dwProfessionID;

	bRetCode = Send(nConnIndex, &ForgetProfessionNotify, sizeof(ForgetProfessionNotify));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 增加生活技能经验
BOOL KPlayerServer::DoAddProfessionProficiency(int nConnIndex, DWORD dwProfessionID, DWORD dwExp)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
	S2C_ADD_PROFESSION_PROFICIENCY  AddProfessionProficiency;

	AddProfessionProficiency.byProtocolID = s2c_add_profession_proficiency;
	AddProfessionProficiency.dwProfessionID = dwProfessionID;
	AddProfessionProficiency.dwExp = dwExp;

	bRetCode = Send(nConnIndex, &AddProfessionProficiency, sizeof(AddProfessionProficiency));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 设置生活技能最大等级
BOOL KPlayerServer::DoSetMaxProfessionLevelNotify(int nConnIndex, DWORD dwProfessionID, DWORD dwLevel)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
	S2C_SET_MAX_PROFESSION_LEVEL_NOTIFY SetMaxProfessionLevelNotify;
	
    SetMaxProfessionLevelNotify.byProtocolID = s2c_set_max_profession_level_notify;
	SetMaxProfessionLevelNotify.dwProfessionID = dwProfessionID;
	SetMaxProfessionLevelNotify.dwLevel = dwLevel;

	bRetCode = Send(nConnIndex, &SetMaxProfessionLevelNotify, sizeof(SetMaxProfessionLevelNotify));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 同步更新玩家生活技能
BOOL KPlayerServer::DoUpdatePlayerProfession(int nConnIndex, DWORD dwProfessionID, DWORD dwMaxLevel, 
											 DWORD dwCurrentLevel, DWORD dwCurrentProficiency, DWORD dwBranchID)
{
    BOOL                         bResult  = false;
    BOOL                         bRetCode = false;
	S2C_UPDATE_PLAYER_PROFESSION UpdatePlayerProfession;

	UpdatePlayerProfession.byProtocolID = s2c_update_player_profession;
	UpdatePlayerProfession.dwProfessionID = dwProfessionID;
	UpdatePlayerProfession.dwMaxLevel = dwMaxLevel;
	UpdatePlayerProfession.dwCurrentLevel = dwCurrentLevel;
	UpdatePlayerProfession.dwCurrentProficiency = dwCurrentProficiency;
	UpdatePlayerProfession.byBranchID = (BYTE)dwBranchID;

	bRetCode = Send(nConnIndex, &UpdatePlayerProfession, sizeof(UpdatePlayerProfession));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 学习配方
BOOL KPlayerServer::DoLearnRecipeNotify(int nConnIndex, DWORD dwCraftID, DWORD dwRecipeID)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
	S2C_LEARN_RECIPE_NOTIFY LearnRecipeNotify;

	LearnRecipeNotify.byProtocolID = s2c_learn_recipe_notify;
	LearnRecipeNotify.dwCraftID = dwCraftID;
	LearnRecipeNotify.dwRecipeID = dwRecipeID;

	bRetCode = Send(nConnIndex, &LearnRecipeNotify, sizeof(LearnRecipeNotify));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 遗忘配方
BOOL KPlayerServer::DoForgetRecipeNotify(int nConnIndex, DWORD dwCraftID)
{
    BOOL                        bResult         = false;
	BOOL                        bRetCode        = false;
	S2C_FORGET_RECIPE_NOTIFY    ForgetRecipeNotify;

	ForgetRecipeNotify.byProtocolID = s2c_forget_recipe_notify;
	ForgetRecipeNotify.dwCraftID    = dwCraftID;

	bRetCode = Send(nConnIndex, &ForgetRecipeNotify, sizeof(ForgetRecipeNotify));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

// 同步配方列表
BOOL KPlayerServer::DoSyncRecipeState(int nConnIndex, BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult             = false;
	BOOL                    bRetCode            = false;
	S2C_SYNC_RECIPE_STATE*  pSyncRecipeState    = (S2C_SYNC_RECIPE_STATE*)m_byTempData;
	KPlayer*                pPlayer             = GetPlayerByConnection(nConnIndex);

	KG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_RECIPE_STATE) + uDataLen <= MAX_EXTERNAL_PACKAGE_SIZE);

	pSyncRecipeState->byProtocolID  = s2c_sync_recipe_state;
	pSyncRecipeState->dwPlayerID    = pPlayer->m_dwID;
	pSyncRecipeState->wSize         = (WORD)(sizeof(S2C_SYNC_RECIPE_STATE) + uDataLen);

	memcpy(pSyncRecipeState->szData, pbyData, uDataLen);

	bRetCode = Send(nConnIndex, pSyncRecipeState, pSyncRecipeState->wSize);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

// 打开一个客户端窗口
BOOL KPlayerServer::DoOpenWindow(DWORD dwIndex, TARGET_TYPE eTarget, DWORD dwTargetID, const char* pcszText, int nConnIndex)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
    S2C_OPEN_WINDOW*    pOpenWindow = (S2C_OPEN_WINDOW*)m_byTempData;
	size_t              uTextLen    = strlen(pcszText);
    size_t              uPakSize    = sizeof(S2C_OPEN_WINDOW) + uTextLen + 1;

	KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

	pOpenWindow->byProtocolID   = s2c_open_window;
	pOpenWindow->wSize          = (WORD)uPakSize;
	pOpenWindow->dwIndex        = dwIndex;
	pOpenWindow->dwTargetType   = (DWORD)eTarget;
	pOpenWindow->dwTargetID     = dwTargetID;

	strcpy(pOpenWindow->szText, pcszText);
	pOpenWindow->szText[uTextLen] = 0;

	bRetCode = Send(nConnIndex, m_byTempData, pOpenWindow->wSize);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

// 同步配方的使用
BOOL KPlayerServer::DoStartUseRecipe(KPlayer* pPlayer, DWORD dwCraftID, DWORD dwRecipeID, KTarget& rTarget)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    TARGET_TYPE             eTargetType     = ttInvalid;
    KSceneObject*           pSceneObject    = NULL;
	KItem*					pItem			= NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_START_USE_RECIPE    StartUseRecipe;
	
    assert(pPlayer);
    assert(pPlayer->m_pScene);

    StartUseRecipe.byProtocolID = s2c_start_use_recipe;
	StartUseRecipe.dwPlayerID   = pPlayer->m_dwID;
	StartUseRecipe.dwCraftID	= dwCraftID;
	StartUseRecipe.dwRecipeID   = dwRecipeID;

	bRetCode = IS_PLAYER(StartUseRecipe.dwPlayerID);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    eTargetType = rTarget.GetTargetType();

	switch (eTargetType)
	{
	case ttNoTarget:
		StartUseRecipe.byTargetType = (BYTE)eTargetType;
		break;

	case ttCoordination:
        {
            int nZ = 0;
            StartUseRecipe.byTargetType = (BYTE)eTargetType;
            bRetCode = rTarget.GetTarget(&StartUseRecipe.nParam1, &StartUseRecipe.nParam2, &nZ);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
		break;

	case ttNpc:
	case ttDoodad:
	case ttPlayer:
        bRetCode = rTarget.GetTarget(&pSceneObject);
		KGLOG_PROCESS_ERROR(bRetCode && pSceneObject);
        StartUseRecipe.byTargetType = (BYTE)eTargetType;
		StartUseRecipe.nParam1      = pSceneObject->m_dwID;
		break;

	case ttItem:
		bRetCode = rTarget.GetTarget(&pItem);
		KGLOG_PROCESS_ERROR(bRetCode && pItem);
		StartUseRecipe.byTargetType = (BYTE)eTargetType;
		StartUseRecipe.nParam1		= pItem->m_dwID;
		break;

	default:
		KG_ASSERT_EXIT(!"Unexpected target type !");
	}

	BroadcastFunc.m_pvData         = &StartUseRecipe;
	BroadcastFunc.m_uSize          = (DWORD)sizeof(StartUseRecipe);

	pScene = pPlayer->m_pScene;
	pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);
	
    bResult = true;
Exit0:
    return bResult;
}

// 通知客户端接受任务
BOOL KPlayerServer::DoAcceptQuest(int nQuestIndex, DWORD dwQuestID, int nConnIndex, BOOL bDailyQuest)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
	S2C_ACCEPT_QUEST    AcceptQuest;
    
	AcceptQuest.byProtocolID    = s2c_accept_quest;
    AcceptQuest.nQuestIndex     = nQuestIndex;
	AcceptQuest.dwQuestID       = dwQuestID;
    AcceptQuest.byDailyQuest    = (BYTE)bDailyQuest;

	bRetCode = Send(nConnIndex, &AcceptQuest, sizeof(AcceptQuest));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;	
}

// 完成任务
BOOL KPlayerServer::DoFinishQuest(DWORD dwQuestID, int nConnIndex, BOOL bForceFinished)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	S2C_FINISH_QUEST FinishQuest;
	FinishQuest.byProtocolID = s2c_finish_quest;
	FinishQuest.dwQuestID = dwQuestID;
	FinishQuest.bForce = bForceFinished;

	bRetCode = Send(nConnIndex, &FinishQuest, sizeof(FinishQuest));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 通知客户端取消任务
BOOL KPlayerServer::DoCancelQuest(int nQuestIndex, int nConnIndex)
{
    BOOL             bResult        = false;
    BOOL             bRetCode       = false;
	S2C_CANCEL_QUEST CancelQuest;

	CancelQuest.byProtocolID    = s2c_cancel_quest;
	CancelQuest.nQuestIndex     = nQuestIndex;

	bRetCode = Send(nConnIndex, &CancelQuest, sizeof(CancelQuest));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoClearQuest(int nConnIndex, DWORD dwQuestID)
{
    BOOL            bResult    = false;
    BOOL            bRetCode   = false;
    S2C_CLEAR_QUEST ClearQuest;

    ClearQuest.byProtocolID = s2c_clear_quest;
    ClearQuest.dwQuestID    = dwQuestID;

	bRetCode = Send(nConnIndex, &ClearQuest, sizeof(ClearQuest));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoShareQuest(KPlayer* pDstPlayer, KPlayer* pPlayer, DWORD dwQuestID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    S2C_SHARE_QUEST ShareQuest;
	KTarget         Target;
    
    assert(pDstPlayer);
    assert(pPlayer);

    Target.SetTarget(pPlayer);

	ShareQuest.byProtocolID     = s2c_share_quest;
	ShareQuest.dwSrcPlayerID    = pPlayer->m_dwID;
	ShareQuest.dwQuestID        = dwQuestID;

	bRetCode = Send(pDstPlayer->m_nConnIndex, &ShareQuest, sizeof(ShareQuest));
	KG_PROCESS_ERROR(bRetCode);
	
    bResult = true;
Exit0:
    return bResult;
}

// 同步任务变量
BOOL KPlayerServer::DoSyncQuestValue(int nQuestIndex, int nConnIndex)
{
    BOOL                    bResult          = false;
    BOOL                    bRetCode         = false;
	KPlayer*                pPlayer          = GetPlayerByConnection(nConnIndex);
    S2C_SYNC_QUEST_VALUE    SyncQuestValue;

	KGLOG_PROCESS_ERROR(pPlayer);

	SyncQuestValue.byProtocolID = s2c_sync_quest_value;
	SyncQuestValue.byQuestIndex = (BYTE)nQuestIndex;

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT * 2; nIndex++)
	{
        SyncQuestValue.nQuestValue[nIndex] = pPlayer->m_QuestList.GetQuestValue(nQuestIndex, nIndex);
	}
	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		SyncQuestValue.dwKillNpcCount[nIndex] = pPlayer->m_QuestList.GetKillNpcCount(nQuestIndex, nIndex);
	}

	SyncQuestValue.bFailed = pPlayer->m_QuestList.GetQuestFailedFlag(nQuestIndex);

	bRetCode = Send(nConnIndex, &SyncQuestValue, sizeof(SyncQuestValue));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoStartEscortQuest(int nConnIndex, DWORD dwStartPlayerID, DWORD dwQuestID)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
	S2C_START_ESCORT_QUEST  StartEscortQuest;

	StartEscortQuest.byProtocolID     = s2c_start_escort_quest;
	StartEscortQuest.dwStartPlayerID  = dwStartPlayerID;
	StartEscortQuest.dwQuestID        = dwQuestID;

	bRetCode = Send(nConnIndex, &StartEscortQuest, sizeof(S2C_START_ESCORT_QUEST));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncLootList(KPlayer* pPlayer, KDoodad* pDoodad)
{
    BOOL                 bResult            = false;
    BOOL                 bRetCode           = false;
    S2C_SYNC_LOOT_LIST*  pSyncLootList      = NULL;
    BYTE*                pbyPos             = NULL;
    KLootList*           pLootList          = NULL;
    int                  nItemAmount        = 0;

	assert(pPlayer);
	assert(pDoodad);
    
    pSyncLootList = (S2C_SYNC_LOOT_LIST*)m_byTempData;
    pbyPos = m_byTempData + sizeof(S2C_SYNC_LOOT_LIST);

	pSyncLootList->byProtocolID = s2c_sync_loot_list;
	pSyncLootList->dwDoodadID   = pDoodad->m_dwID;

	pLootList = pDoodad->m_pLootList;
	KG_PROCESS_ERROR(pLootList);
	
    if (pDoodad->m_eKind == dkCorpse) // 尸体，只有当player在拾取列表中才需要同步掉落。
    {
        bRetCode = pLootList->IsInLooterList(pPlayer->m_dwID);
        KG_PROCESS_ERROR(bRetCode);
    }

	pSyncLootList->nMoney        = pLootList->GetMoney();
    pSyncLootList->byLooterCount = (BYTE)pLootList->m_nLooterCount;

    for (int i = 0; i < pLootList->m_nLooterCount; ++i)
    {
        pSyncLootList->dwLooterIDList[i] = pLootList->m_LooterList[i];
        pbyPos += sizeof(DWORD);
    }

    nItemAmount = pLootList->GetItemCount();

	for (int i = 0; i < nItemAmount; ++i)
	{
        size_t      uItemDataLen        = 0;
		KLootItem*  pLootItem           = NULL;
        LOOT_ITEM*  pBinaryItem         = NULL;
        BOOL        bDisplayInClient    = false;

        pLootItem = pLootList->GetLootItem(i);
		KGLOG_PROCESS_ERROR(pLootItem);
        
        if (pLootItem->pItem == NULL)
        {
            continue;
        }
        
        bDisplayInClient = pLootList->CanDisplayInClient(i, pPlayer);
        
        pBinaryItem = (LOOT_ITEM*)pbyPos;
        pbyPos += sizeof(LOOT_ITEM);
        
        pBinaryItem->dwItemID       = pLootItem->pItem->m_dwID;
        pBinaryItem->byItemLootType = (BYTE)pLootItem->eLootItemType;
        pBinaryItem->byDisplay      = (BYTE)bDisplayInClient;

        bRetCode = pLootItem->pItem->GetBinaryData(
            &uItemDataLen, pbyPos, (MAX_EXTERNAL_PACKAGE_SIZE - (pbyPos - m_byTempData))
        );
        KGLOG_PROCESS_ERROR(bRetCode);
        pbyPos += uItemDataLen;

        KGLOG_PROCESS_ERROR(uItemDataLen < UCHAR_MAX);
        pBinaryItem->byItemDataSize = (BYTE)uItemDataLen;
	}

	pSyncLootList->wSize = (WORD)(pbyPos - m_byTempData);

	bRetCode = Send(pPlayer->m_nConnIndex, m_byTempData, pSyncLootList->wSize);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoRollItemMessage(int nDestConnIndex, KPlayer* pRoller, int nRollPoint, DWORD dwDoodadID, DWORD dwItemID)
{
    BOOL                    bResult          = false;
    BOOL                    bRetCode         = false;
	S2C_ROLL_ITEM_MESSAGE   RollItemMessage;

	RollItemMessage.byProtocolID    = s2c_roll_item_message;
	RollItemMessage.dwPlayerID      = pRoller->m_dwID;
	RollItemMessage.dwDoodadID      = dwDoodadID;
	RollItemMessage.dwItemID        = dwItemID;
	RollItemMessage.nRollPoint      = nRollPoint;

    bRetCode = Send(nDestConnIndex, &RollItemMessage, sizeof(RollItemMessage));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoOpenLootList(int nConnIndex, KPlayer* pPlayer, KDoodad* pDoodad)
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	S2C_OPEN_LOOT_LIST      OpenLootList;
    
    assert(pPlayer);
    assert(pDoodad);

    KG_PROCESS_ERROR(pDoodad->m_pLootList);

	OpenLootList.byProtocolID   = s2c_open_loot_list;
	OpenLootList.dwPlayerID     = pPlayer->m_dwID;
	OpenLootList.dwDoodadID     = pDoodad->m_dwID;

	bRetCode = Send(nConnIndex, &OpenLootList, sizeof(OpenLootList));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncFightFlag(int nConnIndex, DWORD dwForceID, BOOL bFight)
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	S2C_SYNC_FIGHT_FLAG     SyncFightFlag;

	SyncFightFlag.byProtocolID  = s2c_sync_fight_flag;
	SyncFightFlag.byForceID     = (BYTE)dwForceID;
	SyncFightFlag.byOpen        = (BYTE)bFight;

	bRetCode = Send(nConnIndex, &SyncFightFlag, sizeof(SyncFightFlag));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

// 同步声望
BOOL KPlayerServer::DoSyncRepute(int nConnIndex, DWORD dwForceID, int nLevel, int nRepute, int nDiffValue)
{
	BOOL                bResult       = false;
	BOOL                bRetCode      = false;
	S2C_SYNC_REPUTE     SyncRepute;

	SyncRepute.byProtocolID = s2c_sync_repute;
	SyncRepute.byForceID    = (BYTE)dwForceID;
	SyncRepute.nRepute      = nRepute;
	SyncRepute.byLevel      = (BYTE)nLevel;
	SyncRepute.nDiffValue   = nDiffValue;

	bRetCode = Send(nConnIndex, &SyncRepute, sizeof(SyncRepute));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

// 同步所有势力战斗开关
BOOL KPlayerServer::DoSyncFightflagList(int nConnIndex, int nCount, KPlayer* pPlayer)
{
	BOOL              bResult       = false;
	BOOL              bRetCode      = false;
    S2C_SYNC_FIGHTFLAG_LIST* pSyncFightflagList = NULL;

	KGLOG_PROCESS_ERROR(nCount <= MAX_FORCE_COUNT);
	KGLOG_PROCESS_ERROR(pPlayer);

	pSyncFightflagList = (S2C_SYNC_FIGHTFLAG_LIST *)m_byTempData;
    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_FIGHTFLAG_LIST) + nCount * sizeof(BYTE) <= MAX_EXTERNAL_PACKAGE_SIZE);


	pSyncFightflagList->byProtocolID = s2c_sync_fightflag_list;
	pSyncFightflagList->byFightFlagCount = (BYTE)nCount;
	for (int i = 0; i < nCount; i++)
	{
		pSyncFightflagList->byFightFlag[i] = (BYTE)pPlayer->m_ReputeList.GetFightFlag((DWORD)i);
	}
	pSyncFightflagList->wSize = (WORD)(sizeof(S2C_SYNC_FIGHTFLAG_LIST) + nCount * sizeof(BYTE));

	bRetCode = Send(nConnIndex, pSyncFightflagList, pSyncFightflagList->wSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 同步所有的势力声望
BOOL KPlayerServer::DoSyncReputeList(int nConnIndex, int nCount, KPlayer* pPlayer)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    size_t                  uPakSize        = 0;
	S2C_SYNC_REPUTE_LIST*   pSyncReputeList = (S2C_SYNC_REPUTE_LIST*)m_byTempData;

	KGLOG_PROCESS_ERROR(nCount <= MAX_FORCE_COUNT);

	pSyncReputeList->byProtocolID = s2c_sync_repute_list;
	pSyncReputeList->byForceCount = (BYTE)nCount;

    uPakSize = sizeof(S2C_SYNC_REPUTE_LIST) + sizeof(S2C_SYNC_REPUTE_LIST::KFORCE_REPUTE) * nCount;
    KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

	pSyncReputeList->wSize = (WORD)uPakSize;

	for (int i = 0; i < nCount; i++)
	{
		pSyncReputeList->Repute[i].shRepute = (short)pPlayer->m_ReputeList.GetReputation((DWORD)i);
		pSyncReputeList->Repute[i].byLevel = (BYTE)pPlayer->m_ReputeList.GetReputeLevel((DWORD)i);
	}

	bRetCode = Send(nConnIndex, pSyncReputeList, uPakSize);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

// 同步声望修改标记
BOOL KPlayerServer::DoShowReputation(int nConnIndex, DWORD dwForceID, BOOL bLoginLoading)
{
	BOOL                    bResult             = false;
	BOOL                    bRetCode            = false;
	S2C_SHOW_REPUTATION     SyncModifiedFlag;
	
	SyncModifiedFlag.byProtocolID   = s2c_show_reputation;
	SyncModifiedFlag.dwForceID      = dwForceID;
	SyncModifiedFlag.bShow          = true;
    SyncModifiedFlag.bLoginLoading  = bLoginLoading;

	bRetCode = Send(nConnIndex, &SyncModifiedFlag, sizeof(SyncModifiedFlag));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

// 同步拾取动作
BOOL KPlayerServer::DoCharacterOpen(KCharacter *pCharacter, KDoodad *pDoodad)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_CHARACTER_OPEN      CharacterOpen;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);

	CharacterOpen.byProtocolID  = s2c_character_open;
	CharacterOpen.dwCharacterID = pCharacter->m_dwID;
	CharacterOpen.dwDoodadID    = pDoodad->m_dwID;

	BroadcastFunc.m_pvData = &CharacterOpen;
	BroadcastFunc.m_uSize = sizeof(CharacterOpen);

	pScene = pCharacter->m_pScene;
	pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoDistributeItem(int nConnIndex, DWORD dwDestPlayerID, DWORD dwItemID)
{
    BOOL                        bResult     = false;
	BOOL                        bRetCode    = false;
	S2C_DISTRIBUTE_ITEM_MESSAGE DistributeItem;
    
    DistributeItem.byProtocolID     = s2c_distribute_item_message;
    DistributeItem.dwDestPlayerID   = dwDestPlayerID;
    DistributeItem.dwItemID         = dwItemID;
	
	bRetCode = Send(nConnIndex, &DistributeItem, sizeof(DistributeItem));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoFinishLoot(KPlayer* pPlayer, KDoodad* pDoodad)
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_FINISH_LOOT         FinishLoot;
    
    assert(pPlayer);
    assert(pPlayer->m_pScene);

	FinishLoot.byProtocolID = s2c_finish_loot;
	FinishLoot.dwPlayerID   = pPlayer->m_dwID;
	FinishLoot.dwDoodadID   = pDoodad->m_dwID;

	BroadcastFunc.m_pvData      = &FinishLoot;
	BroadcastFunc.m_uSize       = sizeof(FinishLoot);

    pScene = pPlayer->m_pScene;
    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);
	
    bResult = true;
Exit0:
   	return bResult;
}

BOOL KPlayerServer::DoBeginRollItem(int nConnIndex, KItem* pItem, KDoodad* pDoodad, int nLeftRollFrame)
{
	BOOL                bResult       = false;
	BOOL                bRetCode      = false;
	S2C_BEGIN_ROLL_ITEM BeginRollItem;

	BeginRollItem.byProtocolID      = s2c_begin_roll_item;
	BeginRollItem.dwItemID          = pItem->m_dwID;
	BeginRollItem.dwDoodadID        = pDoodad->m_dwID;
    BeginRollItem.wLeftRollFrame    = (WORD)nLeftRollFrame;

	bRetCode = Send(nConnIndex, &BeginRollItem, sizeof(BeginRollItem));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

// 同步定制的用户消息
BOOL KPlayerServer::DoCustomOTAction(
    KPlayer* pPlayer, int nType, BOOL bIsBreakable, int nFrame, 
    int nTargetType, DWORD dwTargetID, DWORD dwScriptID, const char* pszActionName
)
{
	BOOL                bResult         = false;
	BOOL                bRetCode        = false;
    KScene*             pScene          = NULL;
    KBroadcastFunc      BroadcastFunc;
	S2C_CUSTOM_OTACTION CustomOtaction;
        
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);
    KGLOG_PROCESS_ERROR(pszActionName);
	
	CustomOtaction.byProtocolID = s2c_custom_OTAction;
	CustomOtaction.dwPlayerID   = pPlayer->m_dwID;
	CustomOtaction.nType        = nType;
	CustomOtaction.bIsBreakable = bIsBreakable;
	CustomOtaction.nFrame       = nFrame;
	CustomOtaction.nTargetType  = nTargetType;
	CustomOtaction.nTargetID    = dwTargetID;
	CustomOtaction.dwScriptID   = dwScriptID;

	strncpy(CustomOtaction.szActionName, pszActionName, CUSTOM_OTACTION_NAME_LEN);
	CustomOtaction.szActionName[CUSTOM_OTACTION_NAME_LEN - 1] = 0;

	BroadcastFunc.m_pvData = &CustomOtaction;
	BroadcastFunc.m_uSize  = sizeof(CustomOtaction);

	pScene = pPlayer->m_pScene;
    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
    return bResult;
}

// 添加仇恨
BOOL KPlayerServer::DoModifyThreat(int nConnIndex, DWORD dwCharacterID)
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	S2C_MODIFY_THREAT       ModifyThreat;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	ModifyThreat.byProtocolID   = s2c_modify_threat;
	ModifyThreat.dwCharacterID  = dwCharacterID;

	Send(nConnIndex, &ModifyThreat, sizeof(ModifyThreat));

	bResult = true;
Exit0:
	return bResult;
}

// 修改被仇恨
BOOL KPlayerServer::DoModifyThreaten(int nConnIndex, DWORD dwCharacterID)
{
	BOOL                bResult       = false;
	S2C_MODIFY_THREATEN ModifyThreaten;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	ModifyThreaten.byProtocolID = s2c_modify_threaten;
	ModifyThreaten.dwCharacterID = dwCharacterID;

	Send(nConnIndex, &ModifyThreaten, sizeof(ModifyThreaten));

	bResult = true;
Exit0:
    return bResult;
}

// 清除仇恨
BOOL KPlayerServer::DoClearThreat(int nConnIndex, DWORD dwCharacterID)
{
	BOOL              bResult       = false;
	BOOL              bRetCode      = false;
	S2C_CLEAR_THREAT  ClearThreat;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	ClearThreat.byProtocolID = s2c_clear_threat;
	ClearThreat.dwCharacterID = dwCharacterID;

	Send(nConnIndex, &ClearThreat, sizeof(ClearThreat));
	
	bResult = true;
Exit0:
    return bResult;
}

// 清除被仇恨
BOOL KPlayerServer::DoClearThreaten(int nConnIndex, DWORD dwCharacterID)
{
	BOOL               bResult       = false;
	BOOL               bRetCode      = false;
	S2C_CLEAR_THREATEN ClearThreaten;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	ClearThreaten.byProtocolID = s2c_clear_threaten;
	ClearThreaten.dwCharacterID = dwCharacterID;

	Send(nConnIndex, &ClearThreaten, sizeof(ClearThreaten));

	bResult = true;
Exit0:
    return bResult;
}

struct KFuncTraverseAllPlayerSend
{
    size_t  m_uDataLen;
    void*   m_pvData;

    BOOL operator()(DWORD dwID, KPlayer* pPlayer) 
    {
        g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uDataLen);
        return true;
    }

};

BOOL KPlayerServer::DoTalkMessage(
    int nMsgType, 
    DWORD dwTalkerID, const char cszTalker[], 
    DWORD dwReceiverID, const char cszReceiver[], 
    size_t uDataLen, BYTE* pbyTalkData, 
    int nRange /* = 0  */
)
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;
    size_t              uNameLen            = 0;
    size_t              uPakSize            = 0;
    S2C_TALK_MESSAGE*   pTalkMsg            = (S2C_TALK_MESSAGE*)m_byTempData;
    KCharacter*         pTalker             = NULL;
    KPlayer*            pReceiver           = NULL;
    KScene*             pScene              = NULL;
    KTeam*              pTeam               = NULL;
    KBroadcastTalkFunc  BroadcastTalkFunc;
    KFuncTraverseAllPlayerSend TraverseAllPlayerSendFunc;

    KGLOG_PROCESS_ERROR(nMsgType > trInvalid && nMsgType < trTotal);
    assert(uDataLen);

    KGLOG_PROCESS_ERROR(MAX_CHAT_TEXT_LEN <= MAX_EXTERNAL_PACKAGE_SIZE);

    pTalkMsg->byProtocolID  = s2c_talk_message;
    pTalkMsg->dwTalkerID    = dwTalkerID;
    pTalkMsg->byMsgType     = (BYTE)nMsgType;

    switch (nMsgType)
    {
    case trNearby:
    case trFace:
    case trNpcNearby:
    case trNpcFace:
        {
            assert(nRange >= 0);
            KGLOG_PROCESS_ERROR(dwTalkerID);

            if (IS_PLAYER(dwTalkerID))
            {
                pTalker = g_pSO3World->m_PlayerSet.GetObj(dwTalkerID);
                BroadcastTalkFunc.m_pTalker = (KPlayer*)pTalker;
            }
            else
            {
                pTalker = g_pSO3World->m_NpcSet.GetObj(dwTalkerID);                
            }
            KGLOG_PROCESS_ERROR(pTalker);

            pScene = pTalker->m_pScene;
            KGLOG_PROCESS_ERROR(pScene);

            uNameLen = strlen(cszTalker);
            uPakSize = sizeof(S2C_TALK_MESSAGE) + 1 + uNameLen + uDataLen;
            KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);
            pTalkMsg->byTalkData[0] = (BYTE)uNameLen;
            memcpy(pTalkMsg->byTalkData + 1, cszTalker, uNameLen);
            memcpy(pTalkMsg->byTalkData + 1 + uNameLen, pbyTalkData, uDataLen);
            pTalkMsg->wSize = (WORD)uPakSize;

            BroadcastTalkFunc.m_pvData          = pTalkMsg;
            BroadcastTalkFunc.m_uSize           = uPakSize;
            BroadcastTalkFunc.m_dwExclusive     = pTalker->m_dwOwner;

            if (nRange)
            {
                BroadcastTalkFunc.m_pCenterObject   = pTalker;
                BroadcastTalkFunc.m_nRange          = nRange;
            }

            pScene->Broadcast(pTalker->m_pRegion, BroadcastTalkFunc, 1);
        }
        break;

    case trParty:
        {
            KGLOG_PROCESS_ERROR(dwReceiverID);

            uNameLen = strlen(cszTalker);
            uPakSize = sizeof(S2C_TALK_MESSAGE) + 1 + uNameLen + uDataLen;
            KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);
            pTalkMsg->byTalkData[0] = (BYTE)uNameLen;
            memcpy(pTalkMsg->byTalkData + 1, cszTalker, uNameLen);
            memcpy(pTalkMsg->byTalkData + 1 + uNameLen, pbyTalkData, uDataLen);
            pTalkMsg->wSize = (WORD)uPakSize;

            pTeam = g_pSO3World->m_TeamServer.GetTeam(dwReceiverID);
            KGLOG_PROCESS_ERROR(pTeam);
            PartyBroadcastTalk(pTeam, dwTalkerID, pTalkMsg, uPakSize);
        }
        break;

    case trSence:
    case trNpcSence:
        {
            KGLOG_PROCESS_ERROR(dwTalkerID);

            if (IS_PLAYER(dwTalkerID))
            {
                pTalker = g_pSO3World->m_PlayerSet.GetObj(dwTalkerID);
                BroadcastTalkFunc.m_pTalker = static_cast<KPlayer*>(pTalker);
            }
            else
            {
                pTalker = g_pSO3World->m_NpcSet.GetObj(dwTalkerID);
            }
            KGLOG_PROCESS_ERROR(pTalker);
            KGLOG_PROCESS_ERROR(pTalker->m_pScene);

            uNameLen = strlen(cszTalker);
            uPakSize = sizeof(S2C_TALK_MESSAGE) + 1 + uNameLen + uDataLen;
            KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);
            pTalkMsg->byTalkData[0] = (BYTE)uNameLen;
            memcpy(pTalkMsg->byTalkData + 1, cszTalker, uNameLen);
            memcpy(pTalkMsg->byTalkData + 1 + uNameLen, pbyTalkData, uDataLen);
            pTalkMsg->wSize = (WORD)uPakSize;

            BroadcastTalkFunc.m_pvData          = pTalkMsg;
            BroadcastTalkFunc.m_uSize           = uPakSize;
            BroadcastTalkFunc.m_nMaxSyncCount   = INT_MAX;
            BroadcastTalkFunc.m_dwExclusive     = pTalker->m_dwOwner;

            if (pTalker->m_pScene)
            {
                pTalker->m_pScene->TraversePlayer(BroadcastTalkFunc);
            }
        }
        break;

    case trWhisper:
        {
            KGBlackNode*    pBlackNode  = NULL;
            KGFellowship*   pFellowship = NULL;

            KGLOG_PROCESS_ERROR(dwTalkerID);
            KGLOG_PROCESS_ERROR(dwReceiverID);

            // 接收人在说话人的黑名单中
            pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(dwTalkerID, dwReceiverID);
            if (pBlackNode)
            {
                g_RelayClient.DoPlayerTalkError(dwTalkerID, TALK_ERROR_E_YOU_BLACKLIST_TARGET);
                goto Exit1;
            }

            // 说话人在接收人的黑名单中
            pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(dwReceiverID, dwTalkerID);
            if (pBlackNode)
            {
                g_RelayClient.DoPlayerTalkError(dwTalkerID, TALK_ERROR_E_TARGET_BLACKLIST_YOU);
                goto Exit1;
            }

            pReceiver = g_pSO3World->m_PlayerSet.GetObj(dwReceiverID);
            KG_PROCESS_ERROR(pReceiver);

            uNameLen = strlen(cszTalker);
            uPakSize = sizeof(S2C_TALK_MESSAGE) + 2 + uNameLen + uDataLen;
            KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);
            pTalkMsg->byTalkData[1] = (BYTE)uNameLen;
            memcpy(pTalkMsg->byTalkData + 2, cszTalker, uNameLen);
            memcpy(pTalkMsg->byTalkData + 2 + uNameLen, pbyTalkData, uDataLen);
            pTalkMsg->wSize = (WORD)uPakSize;
            pTalkMsg->byTalkData[0] = false; // 表示是否Echo消息

            Send(pReceiver->m_nConnIndex, pTalkMsg, uPakSize);

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwReceiverID, dwTalkerID);
            if (pFellowship && pFellowship->m_BitFlag.nWhisperDailyCount > 0 && pFellowship->m_nAttraction < g_pSO3World->m_Settings.m_ConstList.nWhisperAttractiveUpLimit)
            {
                g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(dwReceiverID, dwTalkerID, g_pSO3World->m_Settings.m_ConstList.nWhisperAttractiveAdd, false);
                pFellowship->m_BitFlag.nWhisperDailyCount--;
            }
        }
        break;

    case trNpcWhisper:
        {
            KGLOG_PROCESS_ERROR(dwReceiverID);

            pReceiver = g_pSO3World->m_PlayerSet.GetObj(dwReceiverID);
            if (pReceiver)
            {
                uNameLen = strlen(cszTalker);
                uPakSize = sizeof(S2C_TALK_MESSAGE) + 2 + uNameLen + uDataLen;
                KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);
                pTalkMsg->byTalkData[1] = (BYTE)uNameLen;
                memcpy(pTalkMsg->byTalkData + 2, cszTalker, uNameLen);
                memcpy(pTalkMsg->byTalkData + 2 + uNameLen, pbyTalkData, uDataLen);
                pTalkMsg->wSize = (WORD)uPakSize;
                pTalkMsg->byTalkData[0] = false; // 表示是否Echo消息

                Send(pReceiver->m_nConnIndex, pTalkMsg, uPakSize);
            }
        }
        break;

    case trNpcSayTo:
    case trNpcYellTo:
    case trNpcSayToTargetUseSentenceID:
    case trTong:
        {
            KGLOG_PROCESS_ERROR(dwReceiverID);

            pReceiver = g_pSO3World->m_PlayerSet.GetObj(dwReceiverID);
            if (pReceiver)
            {
                uNameLen = strlen(cszTalker);
                uPakSize = sizeof(S2C_TALK_MESSAGE) + 1 + uNameLen + uDataLen;
                KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);
                pTalkMsg->byTalkData[0] = (BYTE)uNameLen;
                memcpy(pTalkMsg->byTalkData + 1, cszTalker, uNameLen);
                memcpy(pTalkMsg->byTalkData + 1 + uNameLen, pbyTalkData, uDataLen);
                pTalkMsg->wSize = (WORD)uPakSize;

                Send(pReceiver->m_nConnIndex, pTalkMsg, uPakSize);
            }
        }
        break;

    case trLocalSys:
    case trDebugThreat:
    case trGmMessage:
        {
            uPakSize = sizeof(S2C_TALK_MESSAGE) + uDataLen;
            memcpy(pTalkMsg->byTalkData, pbyTalkData, uDataLen);

            pTalkMsg->wSize = (WORD)uPakSize;

            pReceiver = g_pSO3World->m_PlayerSet.GetObj(dwReceiverID);
            KGLOG_PROCESS_ERROR(pReceiver);

            Send(pReceiver->m_nConnIndex, pTalkMsg, uPakSize);
        }
        break;

    case trGlobalSys:
        {
            uPakSize = sizeof(S2C_TALK_MESSAGE) + uDataLen;
            memcpy(pTalkMsg->byTalkData, pbyTalkData, uDataLen);

            pTalkMsg->wSize = (WORD)uPakSize;

            TraverseAllPlayerSendFunc.m_pvData      = pTalkMsg;
            TraverseAllPlayerSendFunc.m_uDataLen    = uPakSize;

            g_pSO3World->m_PlayerSet.Traverse(TraverseAllPlayerSendFunc);
        }
        break;

    default:
        KGLOG_PROCESS_ERROR(!"Undefined talk type!");
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

struct KFuncTraverseScenePlayerSend
{
    size_t  m_uDataLen;
    void*   m_pvData;

    BOOL operator()(KPlayer* pPlayer) 
    {
        g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uDataLen);
        return true;
    }

};

BOOL KPlayerServer::DoSceneMessage(KScene* pScene, const char* pszMessage)
{
    BOOL                bResult     = false;
    S2C_TALK_MESSAGE*   pTalkMsg    = (S2C_TALK_MESSAGE*)m_byTempData;
    size_t              uDataLen    = 0;
    KFuncTraverseScenePlayerSend TraverseScenePlayerSendFunc;

    assert(pScene);
    assert(pszMessage);

    uDataLen = sizeof(S2C_TALK_MESSAGE) + 1 + strlen(pszMessage) + 1;
    KGLOG_PROCESS_ERROR(uDataLen < MAX_EXTERNAL_PACKAGE_SIZE);

    pTalkMsg->byProtocolID  = s2c_talk_message;
    pTalkMsg->byMsgType     = trLocalSys;
    pTalkMsg->dwTalkerID    = ERROR_ID;
    pTalkMsg->wSize         = (WORD)uDataLen;

    pTalkMsg->byTalkData[0] = tdbText;
    strcpy((char*)pTalkMsg->byTalkData + 1, pszMessage);

    TraverseScenePlayerSendFunc.m_pvData    = m_byTempData;
    TraverseScenePlayerSendFunc.m_uDataLen  = uDataLen;

    pScene->TraversePlayer(TraverseScenePlayerSendFunc);

    bResult = true;
Exit0:
    return bResult;
}
// 
BOOL KPlayerServer::DoSyncSelfCurrentST(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
	S2C_SYNC_SELF_CURRENT_ST    UpdatePlayerST;
    BOOL                        bValuaChanged   = false;

    bValuaChanged |= (pPlayer->m_nCurrentStamina    != pPlayer->m_nLastSyncSelfCurrentStamina);
    bValuaChanged |= (pPlayer->m_nCurrentThew       != pPlayer->m_nLastSyncSelfCurrentThew);

    KG_PROCESS_ERROR(bValuaChanged);

	UpdatePlayerST.byProtocolID           = s2c_sync_self_current_st;
	UpdatePlayerST.nCurrentStamina        = pPlayer->m_nCurrentStamina;
    UpdatePlayerST.nCurrentThew           = pPlayer->m_nCurrentThew;
    
    pPlayer->m_nLastSyncSelfCurrentStamina = pPlayer->m_nCurrentStamina;
    pPlayer->m_nLastSyncSelfCurrentThew    = pPlayer->m_nCurrentThew;
    
	bRetCode = Send(pPlayer->m_nConnIndex, &UpdatePlayerST, sizeof(UpdatePlayerST));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoDoodadLootOver(KDoodad* pDoodad)
{
	BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_DOODAD_LOOT_OVER    DoodadLootOver;
    
    assert(pDoodad);

    pScene = pDoodad->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

	DoodadLootOver.byProtocolID = s2c_doodad_loot_over;
	DoodadLootOver.dwDoodadID = pDoodad->m_dwID;

	BroadcastFunc.m_pvData = &DoodadLootOver;
	BroadcastFunc.m_uSize = sizeof(DoodadLootOver);

    pScene->Broadcast(pDoodad->m_pRegion, BroadcastFunc);
    
	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncFellowshipList(DWORD dwPlayerID, int nCount, KG_FELLOWSHIP_ALL_GS* pData)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KPlayer*                    pPlayer         = NULL;
    S2C_SYNC_FELLOWSHIP_LIST*   pSyncPak        = (S2C_SYNC_FELLOWSHIP_LIST*)m_byTempData;
    int                         nSendCount      = 0;
    int                         nTotalIndex     = 0;
    int                         nSendMax        = 0;
    KG_FELLOWSHIP_ALL_GS*       pFellowshipList = NULL;

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_FELLOWSHIP_LIST) <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_SUCCESS(!pPlayer);

    pSyncPak->byProtocolID  = s2c_sync_fellowship_list;
    pSyncPak->byType        = (BYTE)1;

    if (nCount == 0)
    {
        pSyncPak->wSize = (WORD)(sizeof(S2C_SYNC_FELLOWSHIP_LIST));    
        pSyncPak->nFellowshipCount  = 0;

        bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, pSyncPak->wSize);
        KG_PROCESS_ERROR(bRetCode);

        goto Exit1;
    }

    nSendMax = (MAX_EXTERNAL_PACKAGE_SIZE - sizeof(S2C_SYNC_FELLOWSHIP_LIST)) / sizeof(KG_FELLOWSHIP_ALL_GS);

    while (nCount > 0) // 分包发送客户端
    {
        nSendCount = nCount >= nSendMax ? nSendMax : nCount;
        nCount -= nSendCount;

        pSyncPak->nFellowshipCount  = (BYTE)nSendCount;

        pSyncPak->wSize = (WORD)(sizeof(S2C_SYNC_FELLOWSHIP_LIST) + sizeof(KG_FELLOWSHIP_ALL_GS) * nSendCount);
        pFellowshipList = (KG_FELLOWSHIP_ALL_GS*)pSyncPak->byFellowshipList;

        for (int nIndex = 0; nIndex < nSendCount; nIndex++)
        {
            int nAttractionLevel = KGFellowshipMgr::AttractionValue2Level(pData[nTotalIndex].nAttraction);

            memcpy(pFellowshipList + nIndex, pData + nTotalIndex, sizeof(KG_FELLOWSHIP_ALL_GS));

            if (nAttractionLevel <= 2)
                pFellowshipList[nIndex].dwMapID = 0;

            nTotalIndex++;
        }

        bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, pSyncPak->wSize);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncFoeList(DWORD dwPlayerID, int nCount, KG_FOE_ALL_GS* pData)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KPlayer*                    pPlayer         = NULL;
    S2C_SYNC_FELLOWSHIP_LIST*   pSyncPak        = (S2C_SYNC_FELLOWSHIP_LIST*)m_byTempData;
    int                         nSendCount      = 0;
    int                         nTotalIndex     = 0;
    int                         nSendMax        = 0;
    KG_FOE_ALL_GS*              pFellowshipList = NULL;

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_FELLOWSHIP_LIST) <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_SUCCESS(!pPlayer);

    pSyncPak->byProtocolID  = s2c_sync_fellowship_list;
    pSyncPak->byType        = (BYTE)2;

    if (nCount == 0)
    {
        pSyncPak->wSize = (WORD)(sizeof(S2C_SYNC_FELLOWSHIP_LIST));    
        pSyncPak->nFellowshipCount  = 0;

        bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, pSyncPak->wSize);
        KG_PROCESS_ERROR(bRetCode);

        goto Exit1;
    }

    nSendMax = (MAX_EXTERNAL_PACKAGE_SIZE - sizeof(S2C_SYNC_FELLOWSHIP_LIST)) / sizeof(KG_FOE_ALL_GS);

    while (nCount > 0) // 分包发送客户端
    {
        nSendCount = nCount >= nSendMax ? nSendMax : nCount;
        nCount -= nSendCount;

        pSyncPak->nFellowshipCount  = (BYTE)nSendCount;

        pSyncPak->wSize = (WORD)(sizeof(S2C_SYNC_FELLOWSHIP_LIST) + sizeof(KG_FOE_ALL_GS) * nSendCount);
        pFellowshipList = (KG_FOE_ALL_GS*)pSyncPak->byFellowshipList;

        for (int nIndex = 0; nIndex < nSendCount; nIndex++)
        {
            memcpy(pFellowshipList + nIndex, pData + nTotalIndex, sizeof(KG_FOE_ALL_GS));
            nTotalIndex++;
        }

        bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, pSyncPak->wSize);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBlackList(DWORD dwPlayerID, int nCount, KG_BLACK_LIST_ALL_GS* pData)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KPlayer*                    pPlayer         = NULL;
    S2C_SYNC_FELLOWSHIP_LIST*   pSyncPak        = (S2C_SYNC_FELLOWSHIP_LIST*)m_byTempData;
    int                         nSendCount      = 0;
    int                         nTotalIndex     = 0;
    int                         nSendMax        = 0;
    KG_BLACK_LIST_ALL_GS*       pFellowshipList = NULL;

    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_FELLOWSHIP_LIST) <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_SUCCESS(!pPlayer);

    pSyncPak->byProtocolID  = s2c_sync_fellowship_list;
    pSyncPak->byType        = (BYTE)3;

    if (nCount == 0)
    {
        pSyncPak->wSize = (WORD)(sizeof(S2C_SYNC_FELLOWSHIP_LIST));    
        pSyncPak->nFellowshipCount  = 0;

        bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, pSyncPak->wSize);
        KG_PROCESS_ERROR(bRetCode);

        goto Exit1;
    }

    nSendMax = (MAX_EXTERNAL_PACKAGE_SIZE - sizeof(S2C_SYNC_FELLOWSHIP_LIST)) / sizeof(KG_BLACK_LIST_ALL_GS);

    while (nCount > 0) // 分包发送客户端
    {
        nSendCount = nCount >= nSendMax ? nSendMax : nCount;
        nCount -= nSendCount;

        pSyncPak->nFellowshipCount = (BYTE)nSendCount;

        pSyncPak->wSize = (WORD)(sizeof(S2C_SYNC_FELLOWSHIP_LIST) + sizeof(KG_BLACK_LIST_ALL_GS) * nSendCount);
        pFellowshipList = (KG_BLACK_LIST_ALL_GS*)pSyncPak->byFellowshipList;

        for (int nIndex = 0; nIndex < nSendCount; nIndex++)
        {
            memcpy(pFellowshipList + nIndex, pData + nTotalIndex, sizeof(KG_BLACK_LIST_ALL_GS));
            nTotalIndex++;
        }

        bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, pSyncPak->wSize);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncFellowshipGroupName(DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KPlayer*                        pPlayer     = NULL;
    S2C_SYNC_FELLOWSHIP_GROUP_NAME* pSyncPak    = (S2C_SYNC_FELLOWSHIP_GROUP_NAME*)m_byTempData;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator  it;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    it = g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.find(dwPlayerID);
    KG_PROCESS_ERROR(it != g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.end());

    pSyncPak->byProtocolID = s2c_sync_fellowship_group_name;

    assert(it->second.nGroupCount <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);
    for (int i = 0; i < it->second.nGroupCount; i++)
    {
        strncpy(pSyncPak->szGroupName[i], it->second.szGroupName[i], sizeof(pSyncPak->szGroupName[i]));
        pSyncPak->szGroupName[i][sizeof(pSyncPak->szGroupName[i]) - sizeof('\0')] = '\0';
    }
    for (int i = it->second.nGroupCount; i < KG_FELLOWSHIP_MAX_CUSTEM_GROUP; i++)
    {
        pSyncPak->szGroupName[i][0] = '\0';
    }

    bRetCode = Send(pPlayer->m_nConnIndex, pSyncPak, sizeof(S2C_SYNC_FELLOWSHIP_GROUP_NAME));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoNpcTalkToPlayer(KNpc* pNpc, DWORD dwDialogID, NPC_TALK_ACTION_TYPE eTalkActionType, DWORD dwTalkIndex, CHARACTER_TALK_TYPE eTalkType, KTarget& rTarget)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	KPlayer*                pPlayer         = NULL;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_NPC_TALK_TO_PLAYER  NpcTalkToPlayer;
    
    assert(pNpc);
    assert(pNpc->m_pScene);

	NpcTalkToPlayer.byProtocolID    = s2c_npc_talk_to_player;
	NpcTalkToPlayer.dwDialogID      = dwDialogID;
	NpcTalkToPlayer.eTalkActionType = eTalkActionType;
	NpcTalkToPlayer.dwTalkIndex     = dwTalkIndex;
	NpcTalkToPlayer.eTalkType       = eTalkType;
	NpcTalkToPlayer.dwNpcID         = pNpc->m_dwID;

	NpcTalkToPlayer.dwTargetType = (DWORD)(rTarget.GetTargetType());
	if (eTalkType == trWhisper)
	{
		bRetCode = rTarget.GetTarget(&pPlayer);
		KG_PROCESS_ERROR(bRetCode && pPlayer);

		NpcTalkToPlayer.dwTargetID = pPlayer->m_dwID;
	}

	switch(eTalkType)
	{
	case trWhisper:
		// 密语
		bRetCode = Send(pPlayer->m_nConnIndex, (void*)&(NpcTalkToPlayer), sizeof(NpcTalkToPlayer));
		KG_PROCESS_ERROR(bRetCode);
		break;
	case trNearby:
		// 近聊
		BroadcastFunc.m_pvData          = &NpcTalkToPlayer;
		BroadcastFunc.m_uSize           = sizeof(NpcTalkToPlayer);
        BroadcastFunc.m_dwExclusive     = pNpc->m_dwOwner;
		BroadcastFunc.m_pCenterObject   = pNpc;
		BroadcastFunc.m_nRange          = CELL_LENGTH * 2 * 12; // 策划指定范围

        pScene = pNpc->m_pScene;
        pScene->Broadcast(pNpc->m_pRegion, BroadcastFunc);

		break;
	case trSence:
		// 场景喊话

	    break;
	default:
		KGLOG_PROCESS_ERROR(false);
	    break;
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSetForce(KCharacter* pCharacter)
{
    BOOL                    bResult         = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_SET_FORCE           SetForce;
    
    assert(pCharacter);

    pScene = pCharacter->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

	SetForce.byProtocolID       = s2c_set_force;
	SetForce.dwCharacterID      = pCharacter->m_dwID;
	SetForce.byForceID          = (BYTE)pCharacter->m_dwForceID;
    
	BroadcastFunc.m_pvData      = &SetForce;
	BroadcastFunc.m_uSize       = sizeof(SetForce);
    BroadcastFunc.m_dwExclusive = pCharacter->m_dwOwner;

	pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSendMailRespond(int nConnIndex, BYTE byRespondID, KMAIL_RESULT_CODE eResult)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    S2C_SEND_MAIL_RESPOND   Respond;

    Respond.byProtocolID = s2c_send_mail_respond;
    Respond.byRespondID  = byRespondID;
    Respond.byResult     = (BYTE)eResult;

    bRetCode = Send(nConnIndex, &Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoGetMailListRespond(int nConnIndex, int nMailCount, KMAIL_LIST_NODE* pMailListNode)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2C_GET_MAIL_LIST_RESPOND*  pPak        = (S2C_GET_MAIL_LIST_RESPOND*)m_byTempData;
    size_t                      uSize       = 0;

    KGLOG_PROCESS_ERROR(pMailListNode);
    KGLOG_PROCESS_ERROR(nMailCount >= 0);

    uSize = nMailCount * sizeof(KMAIL_LIST_NODE);

    KGLOG_PROCESS_ERROR(sizeof(S2C_GET_MAIL_LIST_RESPOND) + uSize < MAX_EXTERNAL_PACKAGE_SIZE);
   
    pPak->wSize         = (WORD)(sizeof(S2C_GET_MAIL_LIST_RESPOND) + uSize);    
    pPak->byProtocolID  = s2c_get_maillist_respond;
    pPak->byMailCount   = (BYTE)nMailCount;
    
    if (nMailCount)
        memcpy(pPak->MailListNode, pMailListNode, uSize);
    
    bRetCode = Send(nConnIndex, pPak, pPak->wSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoNewMailNotify(int nConnIndex, KMAIL_LIST_NODE* pMailListNode)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    S2C_NEW_MAIL_NOTIFY Notify;

    KGLOG_PROCESS_ERROR(pMailListNode);

    Notify.byProtocolID = s2c_new_mail_notify;
    Notify.NewMailListNode  = *pMailListNode;

    bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoGetMailContentRespond(DWORD dwPlayerID, DWORD dwMailID, BYTE byResult, KMAIL_CONTENT* pContent)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2C_GET_MAIL_CONTENT_RESPOND*   pRespond    = (S2C_GET_MAIL_CONTENT_RESPOND*)m_byTempData;
    KPlayer*                        pPlayer     = NULL;
    unsigned                        uPakSize    = 0;

    KGLOG_PROCESS_ERROR(dwPlayerID);
    KGLOG_PROCESS_ERROR(dwMailID);

    if (byResult == mrcSucceed)
    {
        KGLOG_PROCESS_ERROR(pContent);
    }

    uPakSize = sizeof(S2C_GET_MAIL_CONTENT_RESPOND);
    if (byResult == mrcSucceed)
    {
        uPakSize += pContent->wTextSize + pContent->wItemSize;
    }

    KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pRespond->byProtocolID  = s2c_get_mail_content_respond;
    pRespond->dwMailID      = dwMailID;
    pRespond->wSize         = (WORD)uPakSize;
    pRespond->byResult      = byResult;

    pRespond->Content.nMoney    = 0;
    pRespond->Content.wTextSize = 0;
    pRespond->Content.wItemSize = 0;

    if (byResult == mrcSucceed)
    {
        memcpy(
            &(pRespond->Content), pContent, 
            sizeof(KMAIL_CONTENT) + pContent->wTextSize + pContent->wItemSize
        );
    }

    bRetCode = Send(pPlayer->m_nConnIndex, pRespond, pRespond->wSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoMailGeneralRespond(DWORD dwPlayerID, DWORD dwMailID, BYTE byResult)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2C_MAIL_GENERAL_RESPOND*   pRespond    = (S2C_MAIL_GENERAL_RESPOND*)m_byTempData;
    KPlayer*                    pPlayer     = NULL;
    unsigned                    uPakSize    = 0;

    KGLOG_PROCESS_ERROR(dwPlayerID);
    KGLOG_PROCESS_ERROR(dwMailID);

    uPakSize = sizeof(S2C_MAIL_GENERAL_RESPOND);
    KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    pRespond->byProtocolID  = s2c_mail_general_respond;
    pRespond->dwMailID      = dwMailID;
    pRespond->byResult      = byResult;

    bRetCode = Send(pPlayer->m_nConnIndex, pRespond, sizeof(S2C_MAIL_GENERAL_RESPOND));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoMailReleaseAttachment(int nConnIndex, DWORD dwMailID, BOOL bReleaseMoney, BOOL bReleaseText, BOOL bReleaseItem)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2C_MAIL_RELEASE_ATTACHMENT     Respond;

    KGLOG_PROCESS_ERROR(nConnIndex >= 0);
    KGLOG_PROCESS_ERROR(dwMailID);

    Respond.byProtocolID  = s2c_mail_release_attachment;
    Respond.dwMailID      = dwMailID;
    Respond.bReleaseMoney = bReleaseMoney;
    Respond.bReleaseText  = bReleaseText;
    Respond.bReleaseItem  = bReleaseItem;

    bRetCode = Send(nConnIndex, &Respond, sizeof(S2C_MAIL_RELEASE_ATTACHMENT));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncPKState(int nConnIndex, KPlayer* pPlayer, int nEndFrame /* = 0 */, BOOL bRefuseDuelFlag /* = false */)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	S2C_SYNC_PK_STATE   SyncPKState;

	SyncPKState.byProtocolID = s2c_sync_pk_state;
    SyncPKState.dwPlayerID   = pPlayer->m_dwID;
    SyncPKState.byPKState    = (BYTE)pPlayer->m_PK.GetState();
    SyncPKState.dwTargetID   = pPlayer->m_PK.GetTargetID();
    SyncPKState.byRefuseDuel = (BYTE)bRefuseDuelFlag;
    SyncPKState.nEndFrame    = nEndFrame;

    bRetCode = Send(nConnIndex, &SyncPKState, sizeof(SyncPKState));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBroadcastPKState(KPlayer* pPlayer)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
    S2C_SYNC_PK_STATE   SyncPKState;
    KBroadcastFunc		BroadcastFunc;

    assert(pPlayer);
    assert(pPlayer->m_pScene);

	SyncPKState.byProtocolID = s2c_sync_pk_state;
    SyncPKState.dwPlayerID   = pPlayer->m_dwID;
    SyncPKState.byPKState    = (BYTE)pPlayer->m_PK.GetState();
    SyncPKState.dwTargetID   = pPlayer->m_PK.GetTargetID();
    SyncPKState.nEndFrame    = 0;

    BroadcastFunc.m_pvData  = &SyncPKState;
    BroadcastFunc.m_uSize   = sizeof(SyncPKState);

    pPlayer->m_pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoWinDuelBroadcast(DWORD dwWinnerID, DWORD dwLosserID)
{
	BOOL						bResult         = false;
    KScene*                     pScene          = NULL;
    KPlayer*                    pWinner         = NULL;
	KBroadcastFunc				BroadcastFunc;
	S2C_WIN_DUEL_BROADCAST		WinDuelBroadcast;
    
    pWinner = g_pSO3World->m_PlayerSet.GetObj(dwWinnerID);
    KGLOG_PROCESS_ERROR(pWinner);
    KGLOG_PROCESS_ERROR(pWinner->m_pScene);

	WinDuelBroadcast.byProtocolID  = s2c_win_duel_broadcast;
	WinDuelBroadcast.dwWinnerID    = dwWinnerID;
	WinDuelBroadcast.dwLosserID    = dwLosserID;

	BroadcastFunc.m_pvData         = &WinDuelBroadcast;
	BroadcastFunc.m_uSize          = sizeof(WinDuelBroadcast);

	pScene = pWinner->m_pScene;
	pScene->Broadcast(pWinner->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncUserPreferences(int nConnIndex, int nOffset, int nLength, BYTE* pbyData)
{
    BOOL                        bResult                 = false;
	BOOL                        bRetCode                = false;
    S2C_SYNC_USER_PREFERENCES*  pSyncUserPreferences    = (S2C_SYNC_USER_PREFERENCES*)m_byTempData;

    KGLOG_PROCESS_ERROR(nLength <= MAX_USER_PREFERENCES_LEN);
    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_USER_PREFERENCES) + nLength <= MAX_EXTERNAL_PACKAGE_SIZE);

	pSyncUserPreferences->byProtocolID  = s2c_sync_user_preferences;
	pSyncUserPreferences->nOffset       = nOffset;
	pSyncUserPreferences->nLength       = nLength;
	pSyncUserPreferences->wSize         = (WORD)(sizeof(S2C_SYNC_USER_PREFERENCES) + nLength);

	memcpy(pSyncUserPreferences->byData, pbyData, (size_t)nLength);

	bRetCode = Send(nConnIndex, pSyncUserPreferences, pSyncUserPreferences->wSize);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}	

BOOL KPlayerServer::DoSyncAllBookState(int nConnIndex, KPlayer* pPlayer)
{
	BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
	S2C_SYNC_ALL_BOOK_STATE*    pSyncAllBookState   = (S2C_SYNC_ALL_BOOK_STATE*)m_byTempData;
    size_t                      uBookDataSize       = 0;
    
    assert(pPlayer);
    KGLOG_PROCESS_ERROR(sizeof(S2C_SYNC_ALL_BOOK_STATE) < MAX_EXTERNAL_PACKAGE_SIZE);

	pSyncAllBookState->byProtocolID = s2c_sync_all_book_state;
    
    bRetCode = pPlayer->m_BookList.Save(
        &uBookDataSize, pSyncAllBookState->byData, MAX_EXTERNAL_PACKAGE_SIZE - sizeof(S2C_SYNC_ALL_BOOK_STATE)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    pSyncAllBookState->uDataLen     = uBookDataSize;
    pSyncAllBookState->wSize        = (WORD)(sizeof(S2C_SYNC_ALL_BOOK_STATE) + uBookDataSize);
    pSyncAllBookState->dwPlayerID   = pPlayer->m_dwID;
    
	bRetCode = Send(nConnIndex, m_byTempData, pSyncAllBookState->wSize);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBookState(int nConnIndex, DWORD dwBookID, BYTE byState)
{
	BOOL                bResult       = false;
	BOOL                bRetCode      = false;
	S2C_SYNC_BOOK_STATE SyncBookState;

	SyncBookState.byProtocolID  = s2c_sync_book_state;
	SyncBookState.dwBookID      = dwBookID;
	SyncBookState.byState       = byState;

	bRetCode = Send(nConnIndex, &SyncBookState, sizeof(SyncBookState));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    return bResult;
}

// 
BOOL KPlayerServer::DoCharacterChangeModel(KCharacter* pCharacter)
{
    BOOL                        bResult     = false;
	BOOL                        bRetCode    = false;
    KScene*                     pScene      = NULL;
    KBroadcastFunc              BroadcastFunc;
	S2C_CHARACTER_CHANGE_MODEL  ChangeModel;
    
    assert(pCharacter);
    assert(pCharacter->m_pScene);

	ChangeModel.byProtocolID    = s2c_character_change_model;
	ChangeModel.dwCharacterID   = pCharacter->m_dwID;
	ChangeModel.dwModelID       = pCharacter->m_dwModelID;

	BroadcastFunc.m_pvData      = &ChangeModel;
	BroadcastFunc.m_uSize       = sizeof(ChangeModel);
    BroadcastFunc.m_dwExclusive = pCharacter->m_dwOwner;

	pScene = pCharacter->m_pScene;
    pScene->Broadcast(pCharacter->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncNpcDialogFlag(KNpc* pNpc)
{
    BOOL                        bResult         = false;
	BOOL                        bRetCode        = false;
    KScene*                     pScene          = NULL;
    KBroadcastFunc              BroadcastFunc;
	S2C_SYNC_NPC_DIALOG_FLAG    SyncNpcDialogFlag;
    
    assert(pNpc);
    assert(pNpc->m_pScene);

	SyncNpcDialogFlag.byProtocolID  = s2c_sync_npc_dialog_flag;
	SyncNpcDialogFlag.dwNpcID       = pNpc->m_dwID;
	SyncNpcDialogFlag.bDialogFlag   = pNpc->m_bDialogFlag;

	BroadcastFunc.m_pvData          = &SyncNpcDialogFlag;
	BroadcastFunc.m_uSize           = sizeof(SyncNpcDialogFlag);
    BroadcastFunc.m_dwExclusive     = pNpc->m_dwOwner;

    pScene = pNpc->m_pScene;
    pScene->Broadcast(pNpc->m_pRegion, BroadcastFunc);
    
	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncRoleDataOver(int nConnIndex)
{
    BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
	S2C_SYNC_ROLE_DATA_OVER     Notify;

	Notify.byProtocolID = s2c_sync_role_data_over;

	bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncEnableBankPackage(int nConnIndex, int nEnabledCount)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	S2C_SYNC_ENABLE_BANK_PACKAGE SyncEnableBankPackage;

	SyncEnableBankPackage.byProtocolID = s2c_sync_enable_bank_package;
	SyncEnableBankPackage.nEnabledCount = nEnabledCount;

	bRetCode = Send(nConnIndex, &SyncEnableBankPackage, sizeof(SyncEnableBankPackage));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoUpdateMapMark(int nConnIndex, int nX, int nY, int nZ, int nType, const char* pszComment)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	S2C_UPDATE_MAP_MARK UpdateMapMark;

	UpdateMapMark.byProtocolID = s2c_update_map_mark;
	UpdateMapMark.nX = nX;
	UpdateMapMark.nY = nY;
	UpdateMapMark.nZ = nZ;
	UpdateMapMark.nType = nType;
	strncpy(UpdateMapMark.szComment, pszComment, MAX_MAP_MARK_COMMENT_LEN);

	bRetCode = Send(nConnIndex, &UpdateMapMark, sizeof(UpdateMapMark));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncMidMapMark(
    DWORD dwPartyID, DWORD dwSrcPlayerID, int nMapID, 
    int nX, int nY, int nType, const char* pszComment
)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	S2C_SYNC_MID_MAP_MARK SyncMidMapMark;

	SyncMidMapMark.byProtocolID = s2c_sync_mid_map_mark;
	SyncMidMapMark.dwSrcPlayerID = dwSrcPlayerID;
	SyncMidMapMark.nMap = nMapID;
	SyncMidMapMark.nX = nX;
	SyncMidMapMark.nY = nY;
	SyncMidMapMark.nType = nType;

	strncpy(SyncMidMapMark.szComment, pszComment, MAX_MIDMAP_MARK_COMMENT_LEN);
    SyncMidMapMark.szComment[sizeof(SyncMidMapMark.szComment) - 1] = '\0';
		
	PartyBroadcast(dwPartyID, &SyncMidMapMark, sizeof(SyncMidMapMark));

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoPlaySound(int nConnIndex, DWORD dwSoundID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	S2C_PLAY_SOUND PlayerSound;

	PlayerSound.byProtocolID = s2c_play_sound;
	PlayerSound.dwSoundID = dwSoundID;

	bRetCode = Send(nConnIndex, &PlayerSound, sizeof(PlayerSound));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoPlayMiniGame(int nConnIndex, DWORD dwGameID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	S2C_PLAY_MINI_GAME PlayMiniGame;

	PlayMiniGame.byProtocolID = s2c_play_mini_game;
	PlayMiniGame.dwGameID = dwGameID;

	bRetCode = Send(nConnIndex, &PlayMiniGame, sizeof(PlayMiniGame));
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerServer::DoSyncHorseFlag(KPlayer* pPlayer)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KScene*             pScene          = NULL;
    KBroadcastFunc      BroadcastFunc;
    S2C_SYNC_HORSE_FLAG HorseFlag;
    
    assert(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_pScene);

    HorseFlag.byProtocolID = s2c_sync_horse_flag;
    HorseFlag.dwPlayerID   = pPlayer->m_dwID;
    HorseFlag.bHorseFlag   = (bool)pPlayer->m_bOnHorse;

    BroadcastFunc.m_pvData = &HorseFlag;
	BroadcastFunc.m_uSize  = sizeof(HorseFlag);

	pScene = pPlayer->m_pScene;
	pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);
	
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoRoadTrackForceRespond(int nConnIndex)
{
    BOOL                                        bResult             = false;
    BOOL                                        bRetCode            = false;
    int                                         nSyncCount          = 0;
    int                                         nMaxCount           = 0;
    size_t                                      uSize               = 0;
    KPlayer*                                    pPlayer             = NULL;
    KROUTE_NODE*                                pNode               = NULL;
    S2C_ROAD_TRACK_FORCE_RESPOND*               pTrackForceRespond  = (S2C_ROAD_TRACK_FORCE_RESPOND*)m_byTempData;
    KOPEN_ROUTE_NODE_VECTOER::iterator it;
    KOPEN_ROUTE_NODE_VECTOER::iterator itEnd;

    pPlayer = GetPlayerByConnection(nConnIndex);

    nMaxCount = (sizeof(m_byTempData) - sizeof(S2C_ROAD_TRACK_FORCE_RESPOND)) / sizeof(S2C_ROAD_TRACK_FORCE_RESPOND::KNODE_FORCE);

    itEnd = pPlayer->m_OpenRouteNodeList.end();
    for (it = pPlayer->m_OpenRouteNodeList.begin(); it != itEnd; ++it)
    {
        pNode = g_pSO3World->m_RoadManager.GetNodeInfo(*it);
        KGLOG_PROCESS_ERROR(pNode);

        if (pNode->dwDefaultForceID == pNode->dwForceID)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(nMaxCount > nSyncCount);

        pTrackForceRespond->NodeForce[nSyncCount].nNodeID   = *it;
        pTrackForceRespond->NodeForce[nSyncCount].dwForceID = pNode->dwForceID;

        nSyncCount++;
    }

    uSize = sizeof(S2C_ROAD_TRACK_FORCE_RESPOND) + sizeof(S2C_ROAD_TRACK_FORCE_RESPOND::KNODE_FORCE) * nSyncCount;

    pTrackForceRespond->byProtocolID    = s2c_road_track_force_respond;
    pTrackForceRespond->byNodeCount     = (BYTE)nSyncCount;
    pTrackForceRespond->wSize           = (WORD)uSize;

    bRetCode = Send(nConnIndex, pTrackForceRespond, uSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncRouteNodeOpenList(int nConnIndex, int nCount, int* pnNodeList)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    size_t                          uSize           = 0;
    S2C_SYNC_ROUTE_NODE_OPEN_LIST*  pSyncOpenList   = (S2C_SYNC_ROUTE_NODE_OPEN_LIST*)m_byTempData;

    uSize = sizeof(S2C_SYNC_ROUTE_NODE_OPEN_LIST) + sizeof(int) * nCount;
    KGLOG_PROCESS_ERROR(uSize < sizeof(m_byTempData));

    for (int i = 0; i < nCount; i++)
    {
        pSyncOpenList->nNodeID[i] = pnNodeList[i];
    }

    pSyncOpenList->byProtocolID = s2c_sync_route_node_open_list;
    pSyncOpenList->byNodeCount  = (BYTE)nCount;
    pSyncOpenList->wSize        = (WORD)uSize;

    bRetCode = Send(nConnIndex, pSyncOpenList, uSize);
    KG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncPlayerStateInfo(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    S2C_SYNC_PLAYER_STATE_INFO  PlayerStateData;
    
    assert(pPlayer);
    
    PlayerStateData.byProtocolID            = s2c_sync_player_state_info;
    PlayerStateData.byMoveState             = (BYTE)pPlayer->m_eMoveState;
    PlayerStateData.wCurrentTrack           = (WORD)pPlayer->m_nCurrentTrack;
    PlayerStateData.nMoveFrameCounter       = pPlayer->m_nMoveFrameCounter;
    PlayerStateData.nFromNode               = pPlayer->m_nFromNode;
    PlayerStateData.nTargetCity             = pPlayer->m_nTargetCity;
    PlayerStateData.nExperience             = pPlayer->m_nExperience;
    PlayerStateData.nCurrentTrainValue      = pPlayer->m_nCurrentTrainValue;
    PlayerStateData.nUsedTrainValue         = pPlayer->m_nUsedTrainValue;
    PlayerStateData.nSituReviveFrame        = pPlayer->m_ReviveParam.nSituReviveFrame;
    PlayerStateData.nLastSituReviveTime     = pPlayer->m_ReviveParam.nLastSituReviveTime;
    PlayerStateData.bySituReviveCount       = (BYTE)pPlayer->m_ReviveParam.nSituReviveCount;
    PlayerStateData.nCurrentPrestige        = pPlayer->m_nCurrentPrestige;
    PlayerStateData.nMaxLevel               = pPlayer->m_nMaxLevel;
    PlayerStateData.bHideHat                = pPlayer->m_bHideHat;
    PlayerStateData.nContribution           = pPlayer->m_nContribution;

    bRetCode = Send(pPlayer->m_nConnIndex, &PlayerStateData, sizeof(PlayerStateData));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncPrinkForce(KPlayer* pPlayer, DWORD dwForceID, int nLevel)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    S2C_SYNC_PRINK_FORCE        PrinkForce;

    assert(pPlayer);

    PrinkForce.byProtocolID     = s2c_sync_prink_force;
    PrinkForce.byPrinkForceID   = (BYTE)dwForceID;
    PrinkForce.byLevel          = (BYTE)nLevel;

    bRetCode = Send(pPlayer->m_nConnIndex, &PrinkForce, sizeof(PrinkForce));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncVisitMapInfo(KPlayer* pPlayer)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    size_t                      uUsedSize           = 0;
    S2C_SYNC_VISIT_MAP_INFO     SyncVisitMapInfo;
    
    assert(pPlayer);
    
    SyncVisitMapInfo.byProtocolID = s2c_sync_visit_map_info;

    bRetCode = pPlayer->m_SceneVisitRecord.Save(
        &uUsedSize, SyncVisitMapInfo.byVisitMap, sizeof(SyncVisitMapInfo.byVisitMap)
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = Send(pPlayer->m_nConnIndex, &SyncVisitMapInfo, sizeof(SyncVisitMapInfo));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncAccumulateValue(KPlayer* pPlayer)
{
    BOOL                        bResult             = false;
    S2C_SYNC_ACCUMULATE_VALUE   SyncAccumulateValue;

    assert(pPlayer);

    SyncAccumulateValue.byProtocolID        = s2c_sync_accumulate_value;
    SyncAccumulateValue.byAccumualtevalue   = (BYTE)pPlayer->m_nAccumulateValue;

    Send(pPlayer->m_nConnIndex, &SyncAccumulateValue, sizeof(SyncAccumulateValue));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSetTarget(KPlayer* pPlayer, KCharacter* pCharacter)
{
    BOOL            bResult     = false;
    TARGET_TYPE     eTargetType = ttInvalid;
    DWORD           dwTargetID  = 0;
    S2C_SET_TARGET  SetTarget;

    assert(pPlayer);
    assert(pCharacter);

    eTargetType = pCharacter->m_SelectTarget.GetTargetType();
    dwTargetID  = pCharacter->m_SelectTarget.GetTargetID();

    SetTarget.byProtocolID  = s2c_set_target;
    SetTarget.dwCharacterID = pCharacter->m_dwID;
    SetTarget.byTargetType  = (BYTE)eTargetType;
    SetTarget.dwTargetID    = dwTargetID;

    Send(pPlayer->m_nConnIndex, &SetTarget, sizeof(SetTarget));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBroadcastTarget(KCharacter* pCharacter)
{
    BOOL                    bResult     = false;
    KScene*                 pScene      = NULL;
    KBroadcastTargetFunc    BroadCastFunc;

    assert(pCharacter);

    pScene = pCharacter->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    BroadCastFunc.m_pSrcCharacter = pCharacter;

    pScene->TraversePlayer(BroadCastFunc);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncKillPoint(KPlayer* pPlayer)
{
    BOOL                bResult         = false;
    S2C_SYNC_KILL_POINT SyncKillPiont;
    
    assert(pPlayer);

    SyncKillPiont.byProtocolID      = s2c_sync_kill_point;
    SyncKillPiont.dwPlayerID        = pPlayer->m_dwID;
    SyncKillPiont.wCurrentKillPoint = (WORD)pPlayer->m_nCurrentKillPoint;
    
    Send(pPlayer->m_nConnIndex, &SyncKillPiont, sizeof(SyncKillPiont));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSetPlayerRedName(KPlayer* pPlayer)
{
    BOOL                        bResult             = false;
    S2C_SET_PLAYER_RED_NAME     SetPlayerRedName;
    KBroadcastFunc		        BroadcastFunc;

    assert(pPlayer);
    assert(pPlayer->m_pScene);

	SetPlayerRedName.byProtocolID    = s2c_set_player_red_name;
    SetPlayerRedName.dwPlayerID      = pPlayer->m_dwID;
    SetPlayerRedName.byRedName       = (BYTE)pPlayer->m_bRedName;

    BroadcastFunc.m_pvData           = &SetPlayerRedName;
    BroadcastFunc.m_uSize            = sizeof(SetPlayerRedName);

    pPlayer->m_pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSetPlayerDialogFlag(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    S2C_SET_PLAYER_DIALOG_FLAG  SetDialogFlag;
    
    assert(pPlayer);

    SetDialogFlag.byProtocolID  = s2c_set_player_dialog_flag;
    SetDialogFlag.dwPlayerID    = pPlayer->m_dwID;
    SetDialogFlag.byDialogFlag  = (BYTE)pPlayer->m_bCannotDialogWithNPC;
    
    Send(pPlayer->m_nConnIndex, &SetDialogFlag, sizeof(SetDialogFlag));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSetOnlyReviveInSitu(KPlayer* pPlayer)
{
    BOOL                         bResult             = false;
    S2C_SET_ONLY_REVIVE_IN_SITU  SetOnlyReviveInSitu;
    
    assert(pPlayer);

    SetOnlyReviveInSitu.byProtocolID        = s2c_set_only_revive_in_situ;
    SetOnlyReviveInSitu.dwPlayerID          = pPlayer->m_dwID;
    SetOnlyReviveInSitu.byOnlyReviveInSitu  = (BYTE)pPlayer->m_bOnlyReviveInSitu;
    
    Send(pPlayer->m_nConnIndex, &SetOnlyReviveInSitu, sizeof(SetOnlyReviveInSitu));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncFormationCoefficient(KPlayer* pPlayer, DWORD dwMemberID, int nCoefficient)
{
    BOOL                            bResult         = false;
    S2C_SYNC_FORMATION_COEFFICIENT  FormationCof;
    
    FormationCof.byProtocolID = s2c_sync_formation_coefficient;
    FormationCof.dwMemberID   = dwMemberID;
    FormationCof.nCoefficient = nCoefficient;

    Send(pPlayer->m_nConnIndex, &FormationCof, sizeof(FormationCof));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoJoinBattleFieldRespond(DWORD dwPlayerID, DWORD dwMapID, int nErrorCode)
{
    BOOL                                bResult     = false;
    KPlayer*                            pPlayer     = NULL;
    S2C_JOIN_BATTLE_FIELD_QUEUE_RESPOND JoinRespond;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    JoinRespond.byProtocolID = s2c_join_battle_field_queue_respond;
    JoinRespond.dwMapID      = dwMapID;
    JoinRespond.nErrorCode   = nErrorCode;

    Send(pPlayer->m_nConnIndex, &JoinRespond, sizeof(JoinRespond));

    bResult = true;
Exit0:
    return bResult;
}
BOOL KPlayerServer::DoLeaveBattleFieldRespond(DWORD dwPlayerID, DWORD dwMapID, int nErrorCode)
{
    BOOL                                    bResult     = false;
    KPlayer*                                pPlayer     = NULL;
    S2C_LEAVE_BATTLE_FIELD_QUEUE_RESPOND    LeaveRespond;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    LeaveRespond.byProtocolID = s2c_leave_battle_field_queue_respond;
    LeaveRespond.dwMapID      = dwMapID;
    LeaveRespond.nErrorCode   = nErrorCode;

    Send(pPlayer->m_nConnIndex, &LeaveRespond, sizeof(LeaveRespond));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBattleFieldNotify(
    DWORD dwPlayerID, time_t nAvgQueueTime, int nPassTime,
    DWORD dwMapID, int nCopyIndex, BATTLE_FIELD_NOTIFY_TYPE eType
)
{
    BOOL                    bResult = false;
    KPlayer*                pPlayer = NULL;
    S2C_BATTLE_FIELD_NOTIFY Notify;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    Notify.byProtocolID     = s2c_battle_field_notify;
    Notify.nAvgQueueTime    = nAvgQueueTime;
    Notify.nPassTime        = nPassTime;
    Notify.dwMapID          = dwMapID;
    Notify.nCopyIndex       = nCopyIndex;
    Notify.eType            = eType;

    Send(pPlayer->m_nConnIndex, &Notify, sizeof(Notify));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBattleFieldList(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex)
{
    BOOL                        bResult         = false;
    KPlayer*                    pPlayer         = NULL;
    S2C_SYNC_BATTLE_FIELD_LIST  BattleFieldList;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    BattleFieldList.byProtocolID = s2c_sync_battle_field_list;
    BattleFieldList.dwMapID      = dwMapID;
    BattleFieldList.nCopyIndex   = nCopyIndex;

    Send(pPlayer->m_nConnIndex, &BattleFieldList, sizeof(BattleFieldList));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBattleFieldSide(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    KScene*                     pScene          = NULL;
    S2C_SYNC_BATTLE_FIELD_SIDE  BattleFieldSide;

    assert(pPlayer);

    BattleFieldSide.byProtocolID    = s2c_sync_battle_field_side;
    BattleFieldSide.dwPlayerID      = pPlayer->m_dwID;
    BattleFieldSide.nSide           = pPlayer->m_nBattleFieldSide;

    Send(pPlayer->m_nConnIndex, &BattleFieldSide, sizeof(BattleFieldSide));

    bResult = true;
Exit0:
    return bResult;
}

struct KFuncTraverseBattleFieldPlayerSendEnd
{
    void*   m_pvData;
    size_t  m_uDataLen;
    BOOL operator()(KPlayer* pPlayer)
    {
        g_PlayerServer.Send(pPlayer->m_nConnIndex, m_pvData, m_uDataLen);

        return true;
    }
};

BOOL KPlayerServer::DoBattleFieldEnd(DWORD dwMapID, int nCopyIndex)
{
    BOOL                                    bResult         = false;
    KScene*                                 pScene          = NULL;
    S2C_BATTLE_FIELD_END                    BattleFieldEnd;
    KFuncTraverseBattleFieldPlayerSendEnd   Func;

    pScene = g_pSO3World->GetScene(dwMapID, nCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    BattleFieldEnd.byProtocolID = s2c_battle_field_end;

    Func.m_pvData   = &BattleFieldEnd;
    Func.m_uDataLen = sizeof(S2C_BATTLE_FIELD_END);

    pScene->TraversePlayer(Func);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncCampInfo(KPlayer* pPlayer)
{
    BOOL                bResult                 = false;
    int                 nServerCampInfoVersion  = 0;
    S2C_SYNC_CAMP_INFO  SyncCampInfo;    

    assert(pPlayer);
    
    nServerCampInfoVersion = g_pSO3World->m_CampInfo.GetVersion();
    if (nServerCampInfoVersion == pPlayer->m_nClientCampInfoVersion)
    {
        goto Exit1;
    }

    SyncCampInfo.byProtocolID = s2c_sync_camp_info;
    SyncCampInfo.byCampLevel  = (BYTE)g_pSO3World->m_CampInfo.GetCampLevel();
    SyncCampInfo.nCampScore   = g_pSO3World->m_CampInfo.GetCampScore();
    
    Send(pPlayer->m_nConnIndex, &SyncCampInfo, sizeof(SyncCampInfo));

    pPlayer->m_nClientCampInfoVersion = nServerCampInfoVersion;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncCurrentPrestige(KPlayer* pPlayer)
{
    BOOL                        bResult             = false;
    S2C_SYNC_CURRENT_PRESTIGE   SyncCurrentPrestige;    
    
    assert(pPlayer);

    SyncCurrentPrestige.byProtocolID        = s2c_sync_current_prestige;
    SyncCurrentPrestige.nCurrentPrestige    = pPlayer->m_nCurrentPrestige;
    
    Send(pPlayer->m_nConnIndex, &SyncCurrentPrestige, sizeof(SyncCurrentPrestige));

    bResult = true;

Exit0:
    return bResult; 
}

BOOL KPlayerServer::DoSyncTongMemberInfo(DWORD dwPlayerID, const KTongMemberInfo& crTongMember)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KPlayer*                    pPlayer         = NULL;
    S2C_SYNC_TONG_MEMBER_INFO   SyncInfo;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    SyncInfo.byProtocolID   = s2c_sync_tong_member_info;
    SyncInfo.TongMemberInfo = crTongMember;

    bRetCode = Send(pPlayer->m_nConnIndex, &SyncInfo, sizeof(SyncInfo));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    return bResult;
}

BOOL KPlayerServer::DoTongDeleteMemberNotify(DWORD dwPlayerID, DWORD dwMemberID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KPlayer*                        pPlayer     = NULL;
    S2C_DELETE_TONG_MEMBER_NOTIFY*  pPackage    = (S2C_DELETE_TONG_MEMBER_NOTIFY*)m_byTempData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    pPackage->byProtocolID  = s2c_delete_tong_member_notify;
    pPackage->dwMemberID   = dwMemberID;

    bRetCode = Send(pPlayer->m_nConnIndex, pPackage, sizeof(S2C_DELETE_TONG_MEMBER_NOTIFY));
    KG_PROCESS_ERROR(bRetCode);

    bResult =  true;

Exit0:
    return bResult;
}

BOOL KPlayerServer::DoApplyTongInfoRespond(DWORD dwPlayerID, BYTE byRespondType, BYTE* pbyRespondData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KPlayer*                        pPlayer     = NULL;
    S2C_APPLY_TONG_INFO_RESPOND*    pRespond    = (S2C_APPLY_TONG_INFO_RESPOND*)m_byTempData;
    size_t                          uPakSize    = sizeof(S2C_APPLY_TONG_INFO_RESPOND) + uDataLen;

    KGLOG_PROCESS_ERROR(uPakSize <= MAX_EXTERNAL_PACKAGE_SIZE);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    pRespond->byProtocolID  = s2c_apply_tong_info_respond;
    pRespond->byRespondType = byRespondType;
    pRespond->wSize         = (WORD)uPakSize;
    memcpy(pRespond->byData, pbyRespondData, uDataLen);

    bRetCode = Send(pPlayer->m_nConnIndex, pRespond, uPakSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    return bResult;
}

BOOL KPlayerServer::DoUpdateTongClientDataVersion(DWORD dwPlayerID, KTONG_UPDATE_FLAG eFrameType, int nUpdateFrame)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KPlayer*                            pPlayer     = NULL;
    S2C_UPDATE_TONG_CLIENT_DATA_VERSION Package;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    Package.byProtocolID    = s2c_update_tong_client_data_version;
    Package.byFrameType     = (BYTE)eFrameType;
    Package.nUpdateFrame    = nUpdateFrame ;

    bRetCode = Send(pPlayer->m_nConnIndex, &Package, sizeof(Package));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    return bResult;
}

BOOL KPlayerServer::DoInvitePlayerJoinTongRequest(
    DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, char szInviterName[], char szTongName[]
)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KPlayer*                            pPlayer     = NULL;
    S2C_INVITE_PLAYER_JOIN_TONG_REQUEST Package;

    assert(szInviterName);
    assert(szTongName);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    Package.byProtocolID    = s2c_invite_player_join_tong_request;
    Package.dwInviterID     = dwInviterID;
    Package.dwTongID        = dwTongID;

    strncpy(Package.szInviterName, szInviterName, sizeof(Package.szInviterName));
    Package.szInviterName[sizeof(Package.szInviterName) - 1] = '\0';

    strncpy(Package.szTongName, szTongName, sizeof(Package.szTongName));
    Package.szTongName[sizeof(Package.szTongName) - 1] = '\0';

    bRetCode = Send(pPlayer->m_nConnIndex, &Package, sizeof(Package));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    return bResult;
}

BOOL KPlayerServer::DoChangePlayerTongNotify(int nConnIndex, DWORD dwTongID, BYTE byReason, char szTongName[])
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    S2C_CHANGE_PLAYER_TONG_NOTIFY   Notify;
    
    Notify.byProtocolID = s2c_change_player_tong_notify;
    Notify.dwTongID     = dwTongID;
    Notify.byReason     = byReason;
    strncpy(Notify.szTongName, szTongName, sizeof(Notify.szTongName));
    Notify.szTongName[sizeof(Notify.szTongName) - 1] = '\0';

    bRetCode = Send(nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBroadcastTongMessage(int nConnIndex, BYTE byCode, BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2C_TONG_BROADCAST_MESSAGE* pMessage    = (S2C_TONG_BROADCAST_MESSAGE*)m_byTempData;
    size_t                      uPackSize   = sizeof(S2C_TONG_BROADCAST_MESSAGE) + uDataLen;

    KGLOG_PROCESS_ERROR(MAX_EXTERNAL_PACKAGE_SIZE >= uPackSize);

    pMessage->byProtocolID  = s2c_tong_broadcast_message;
    pMessage->wSize         = (WORD)uPackSize;
    pMessage->byCode        = byCode;
    memcpy(pMessage->byData, pbyData, uDataLen);

    bRetCode = Send(nConnIndex, pMessage, uPackSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoGetTongDescriptionRespond(int nConnIndex, DWORD dwTongID, char szTongName[])
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    S2C_GET_TONG_DESCRIPTION_RESPOND    Respond;

    Respond.byProtocolID = s2c_get_tong_description_respond;
    Respond.dwTongID = dwTongID;
    strncpy(Respond.szTongName, szTongName, sizeof(Respond.szTongName));
    Respond.szTongName[sizeof(Respond.szTongName) -  1] = '\0';

    bRetCode = Send(nConnIndex, &Respond, sizeof(Respond));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoBroadcastPlayerChangeTong(KPlayer* pPlayer)
{
    BOOL                                bResult = false;
    KScene*                             pScene  = NULL;
    KBroadcastFunc                      BroadcastFunc;
    S2C_BROADCAST_PLAYER_CHANGE_TONG    Pak;

    assert(pPlayer);

    Pak.byProtocolID    = s2c_broadcast_player_change_tong;
    Pak.dwPlayerID      = pPlayer->m_dwID;
    Pak.dwTongID        = pPlayer->m_dwTongID;

    BroadcastFunc.m_pvData = &Pak;
    BroadcastFunc.m_uSize = sizeof(Pak);

	pScene = pPlayer->m_pScene;
    KG_PROCESS_ERROR(pScene);

    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);
    bResult = true;

Exit0:    
    return bResult;
}

BOOL KPlayerServer::DoSyncTongOnlineMessage(int m_nConnIndex, char szOnlineMessage[], size_t uDataLen)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    S2C_SYNC_TONG_ONLINE_MESSAGE*   pSync = (S2C_SYNC_TONG_ONLINE_MESSAGE*)m_byTempData;

    KGLOG_PROCESS_ERROR(MAX_EXTERNAL_PACKAGE_SIZE >= sizeof(S2C_SYNC_TONG_ONLINE_MESSAGE) + uDataLen);

    pSync->byProtocolID = s2c_sync_tong_online_message;
    pSync->wSize        = (WORD)(sizeof(S2C_SYNC_TONG_ONLINE_MESSAGE) + uDataLen);
    strncpy(pSync->szOnlineMessage, szOnlineMessage, uDataLen);
    assert(uDataLen > 0);
    pSync->szOnlineMessage[uDataLen - 1] = '\0';

    bRetCode = Send(m_nConnIndex, pSync, sizeof(S2C_SYNC_TONG_ONLINE_MESSAGE) + uDataLen);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAuctionLookupRespond(DWORD dwPlayerID, BYTE byRespondID, BYTE byCode, KAUCTION_PAGE_HEAD* pPage)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    KPlayer*                    pPlayer  = NULL;
    S2C_AUCTION_LOOKUP_RESPOND* pRespond = (S2C_AUCTION_LOOKUP_RESPOND*)m_byTempData;

    assert(pPage);

    KGLOG_PROCESS_ERROR(MAX_EXTERNAL_PACKAGE_SIZE >= sizeof(S2C_AUCTION_LOOKUP_RESPOND) + pPage->uSize);

    pRespond->byProtocolID  = s2c_auction_lookup_respond;
    pRespond->wSize         = (WORD)(sizeof(S2C_AUCTION_LOOKUP_RESPOND) + pPage->uSize);
    pRespond->byRespondID   = byRespondID;
    pRespond->byCode        = byCode;
    memcpy(&pRespond->Page, pPage, sizeof(KAUCTION_PAGE_HEAD) + pPage->uSize);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = Send(pPlayer->m_nConnIndex, pRespond, sizeof(S2C_AUCTION_LOOKUP_RESPOND) + pPage->uSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAuctionBidRespond(DWORD dwPlayerID, BYTE byCode)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    KPlayer*                    pPlayer  = NULL;
    S2C_AUCTION_BID_RESPOND     Respond;

    Respond.byProtocolID    = s2c_auction_bid_respond;
    Respond.byCode          = byCode;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = Send(pPlayer->m_nConnIndex, &Respond, sizeof(Respond));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAuctionSellRespond(DWORD dwPlayerID, BYTE byCode)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    KPlayer*                    pPlayer  = NULL;
    S2C_AUCTION_SELL_RESPOND    Respond;

    Respond.byProtocolID    = s2c_auction_sell_respond;
    Respond.byCode          = byCode;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = Send(pPlayer->m_nConnIndex, &Respond, sizeof(Respond));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAuctionCancelRespond(DWORD dwPlayerID, BYTE byCode)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    KPlayer*                    pPlayer  = NULL;
    S2C_AUCTION_CANCEL_RESPOND  Respond;

    Respond.byProtocolID    = s2c_auction_cancel_respond;
    Respond.byCode          = byCode;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    bRetCode = Send(pPlayer->m_nConnIndex, &Respond, sizeof(Respond));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoAuctionMessageNotify(DWORD dwPlayerID, BYTE byCode, const char* pszSaleName, int nPrice)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    KPlayer*                    pPlayer  = NULL;
    S2C_AUCTION_MESSAGE_NOTIFY  Notify;
    
    KGLOG_PROCESS_ERROR(pszSaleName);

    Notify.byProtocolID     = s2c_auction_message_notify;
    Notify.byCode           = byCode;
    Notify.nPrice           = nPrice;

    strncpy(Notify.szSaleName, pszSaleName, sizeof(Notify.szSaleName));
    Notify.szSaleName[sizeof(Notify.szSaleName) - 1] = '\0';
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    bRetCode = Send(pPlayer->m_nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSetCamp(KPlayer* pPlayer)
{
    BOOL                    bResult         = false;
    KScene*                 pScene          = NULL;
    KBroadcastFunc          BroadcastFunc;
	S2C_SET_CAMP            SetCamp;
    
    assert(pPlayer);

    pScene = pPlayer->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

	SetCamp.byProtocolID = s2c_set_camp;
	SetCamp.dwPlayerID   = pPlayer->m_dwID;
    SetCamp.byCamp       = (BYTE)pPlayer->m_eCamp;
    
	BroadcastFunc.m_pvData  = &SetCamp;
	BroadcastFunc.m_uSize   = sizeof(SetCamp);

	pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

	bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBattleFieldStatistics(int nConnIndex, KPlayer* pPlayer, const KPQ_STATISTICS_DATA& crStatisticsData)
{
    BOOL                            bResult         = false;
    S2C_SYNC_BATTLEFIELD_STATISTICS BattlefieldStatistics;

    BattlefieldStatistics.byProtocolID      = s2c_sync_battlefield_statistics;
    BattlefieldStatistics.dwPlayerID        = ERROR_ID;
    BattlefieldStatistics.dwPlayerForceID   = 0;
    BattlefieldStatistics.nBattleFieldSide  = INVALID_BATTLE_SIDE;
    BattlefieldStatistics.szPlayerName[0]   = '\0';

    if (pPlayer)
    {
        BattlefieldStatistics.dwPlayerID        = pPlayer->m_dwID;
        BattlefieldStatistics.dwPlayerForceID   = pPlayer->m_dwForceID;
        BattlefieldStatistics.nBattleFieldSide  = pPlayer->m_nBattleFieldSide;

        strncpy(BattlefieldStatistics.szPlayerName, pPlayer->m_szName, sizeof(BattlefieldStatistics.szPlayerName));
        BattlefieldStatistics.szPlayerName[sizeof(BattlefieldStatistics.szPlayerName) - 1] = '\0';
    }

    assert(sizeof(BattlefieldStatistics.nValueArray) == sizeof(crStatisticsData.nValueArray));
    memcpy(BattlefieldStatistics.nValueArray, crStatisticsData.nValueArray, sizeof(BattlefieldStatistics.nValueArray));

    BattlefieldStatistics.nValueArray[psiThreatOutput]  = 0;
    BattlefieldStatistics.nValueArray[psiSkillMark]     = 0;

    Send(nConnIndex, &BattlefieldStatistics, sizeof(BattlefieldStatistics));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBattleFieldObjective(int nConnIndex, int nIndex, int nCurrentValue, int nTargetValue)
{
    BOOL                            bResult         = false;
    S2C_SYNC_BATTLEFIELD_OBJECTIVE  BattlefieldObjective;

    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < PQ_PARAM_COUNT);
    assert(nIndex < UCHAR_MAX);

    BattlefieldObjective.byProtocolID   = s2c_sync_battlefield_objective;
    BattlefieldObjective.byIndex        = (BYTE)nIndex;
    BattlefieldObjective.nCurrentValue  = nCurrentValue;
    BattlefieldObjective.nTargetValue   = nTargetValue;

    Send(nConnIndex, &BattlefieldObjective, sizeof(BattlefieldObjective));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncBattleFieldPQInfo(int nConnIndex, DWORD dwPQID, DWORD dwPQTemplateID, int nLapseTime, int nLeftTime)
{
    BOOL                            bResult         = false;
    S2C_SYNC_BATTLEFIELD_PQ_INFO    BattlefieldPQInfo;

    BattlefieldPQInfo.byProtocolID      = s2c_sync_battlefield_pq_info;
    BattlefieldPQInfo.dwPQID            = dwPQID;
    BattlefieldPQInfo.dwPQTemplateID    = dwPQTemplateID;
    BattlefieldPQInfo.nLapseTime        = nLapseTime;
    BattlefieldPQInfo.nLeftTime         = nLeftTime;

    Send(nConnIndex, &BattlefieldPQInfo, sizeof(BattlefieldPQInfo));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncHeroFlag(KPlayer* pPlayer)
{
    BOOL                   bResult         = false;
    S2C_SYNC_HERO_FLAG     SyncHeroFlag;
    
    assert(pPlayer);

    SyncHeroFlag.byProtocolID   = s2c_sync_hero_flag;
    SyncHeroFlag.byHeroFlag     = (BYTE)pPlayer->m_bHeroFlag;

    Send(pPlayer->m_nConnIndex, &SyncHeroFlag, sizeof(SyncHeroFlag));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoRemoteLuaCall(int nConnIndex, const char cszFunction[], BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    size_t                  uStrLen     = 0;
    S2C_REMOTE_LUA_CALL*    pLuaCall    = (S2C_REMOTE_LUA_CALL*)m_byTempData;
    size_t                  uPakSize    = sizeof(S2C_REMOTE_LUA_CALL) + uDataLen;

    KGLOG_PROCESS_ERROR(uPakSize < MAX_EXTERNAL_PACKAGE_SIZE);

    pLuaCall->byProtocolID = s2c_remote_lua_call;
    pLuaCall->wSize        = (WORD)uPakSize;

    uStrLen = strlen(cszFunction);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(pLuaCall->szFunction));
    strcpy(pLuaCall->szFunction, cszFunction);

    memcpy(pLuaCall->byData, pbyData, uDataLen);

    bRetCode = Send(nConnIndex, m_byTempData, uPakSize);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncPlayerReviveCtrl(KPlayer* pPlayer)
{
    BOOL bResult = false;
    S2C_SYNC_PLAYER_REVIVE_CTRL SyncPlayerReviveCtrl;

    assert(pPlayer);

    SyncPlayerReviveCtrl.byProtocolID       = s2c_sync_player_revive_ctrl;
    SyncPlayerReviveCtrl.nReviveFrame       = pPlayer->m_ReviveCtrl.nReviveFrame;
    SyncPlayerReviveCtrl.bReviveInSitu      = pPlayer->m_ReviveCtrl.bReviveInSitu;
    SyncPlayerReviveCtrl.bReviveInAltar     = pPlayer->m_ReviveCtrl.bReviveInAltar;

    SyncPlayerReviveCtrl.bReviveByPlayer    = false;
    SyncPlayerReviveCtrl.dwReviver          = ERROR_ID;
    if (pPlayer->m_ReviveCtrl.bReviveByPlayer && pPlayer->m_ReviveParam.dwReviver)
    {
        SyncPlayerReviveCtrl.bReviveByPlayer = pPlayer->m_ReviveCtrl.bReviveByPlayer;
        SyncPlayerReviveCtrl.dwReviver = pPlayer->m_ReviveParam.dwReviver;
    }

    Send(pPlayer->m_nConnIndex, &SyncPlayerReviveCtrl, sizeof(SyncPlayerReviveCtrl));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncAchievementData(KPlayer* pPlayer)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    size_t                      uUsedSize   = 0;
    S2C_SYNC_ACHIEVEMENT_DATA   SyncAchievementData;

    SyncAchievementData.byProtocolID        = s2c_sync_achievement_data;
    SyncAchievementData.dwPlayerID          = pPlayer->m_dwID;
    SyncAchievementData.nAchievementPoint   = pPlayer->m_nAchievementPoint;
    SyncAchievementData.nAchievementRecord  = pPlayer->m_nAchievementRecord;
    
    bRetCode = pPlayer->m_AchievementSet.Save(&uUsedSize, SyncAchievementData.byAchievementSet, sizeof(SyncAchievementData.byAchievementSet));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = Send(pPlayer->m_nConnIndex, &SyncAchievementData, sizeof(SyncAchievementData));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoNewAchievementNotify(KPlayer* pPlayer, int nAchievementID)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    S2C_NEW_ACHIEVEMENT_NOTIFY  Notify;

    Notify.byProtocolID     = s2c_new_achievement_notify;
    Notify.nAchievementID   = nAchievementID;

    bRetCode = Send(pPlayer->m_nConnIndex, &Notify, sizeof(Notify));
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::BroadcastAchievementAnnounce(DWORD dwPlayerID, BYTE byType, const char* pszRoleName, int nAchievementID)
{
    BOOL                            bResult     = false;
    int                             nConnIndex  = 0;
    KPlayer*                        pPlayer     = NULL;
    KScene*                         pScene      = NULL;
    S2C_ACHIEVEMENT_ANNOUNCE_NOTIFY Notify;
    KBroadcastFunc                  BroadcastFunc;
    KFuncTraverseAllPlayerSend      TraverseAllPlayerFunc;

    assert(pszRoleName);

    Notify.byProtocolID = s2c_achievement_announce_notify;

    strncpy(Notify.szRoleName, pszRoleName, sizeof(Notify.szRoleName));
    Notify.szRoleName[sizeof(Notify.szRoleName) - 1] = '\0';

    Notify.nAchievementID = nAchievementID;

    switch (byType)
    {
    case aatNearby:
    case aatScene:
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
        KGLOG_PROCESS_ERROR(pPlayer);

        pScene = pPlayer->m_pScene;
        KGLOG_PROCESS_ERROR(pScene);

        BroadcastFunc.m_pvData  = &Notify;
        BroadcastFunc.m_uSize   = sizeof(Notify);

        if (byType == aatNearby)
        {
            pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc, 1);
        }
        else //if (byType == aatScene)
        {
            pScene->TraversePlayer(BroadcastFunc);
        }
        break;

    case aatGlobal:
        TraverseAllPlayerFunc.m_pvData      = &Notify;
        TraverseAllPlayerFunc.m_uDataLen    = sizeof(Notify);

        g_pSO3World->m_PlayerSet.Traverse(TraverseAllPlayerFunc);
        break;

    default:
        assert(false);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerServer::DoSyncAchievementPoint(KPlayer* pPlayer, int nDeltaPoint)
{
    S2C_SYNC_ACHIEVEMENT_POINT SyncPoint;

    SyncPoint.byProtocolID  = s2c_sync_achievement_point;
    SyncPoint.nDeltaPoint   = nDeltaPoint;

    Send(pPlayer->m_nConnIndex, &SyncPoint, sizeof(SyncPoint));

    return true;
}

BOOL KPlayerServer::DoSyncMaxLevel(int nConnIndex, int nMaxLevel)
{
    S2C_SYNC_MAX_LEVEL SyncMaxLevel;

    SyncMaxLevel.byProtocolID   = s2c_sync_max_level;
    SyncMaxLevel.nMaxLevel      = nMaxLevel;

    Send(nConnIndex, &SyncMaxLevel, sizeof(SyncMaxLevel));

    return true;
}

BOOL KPlayerServer::DoUseItemPrepare(
    KPlayer* pPlayer, int nFrames, DWORD dwOTAniID, DWORD dwEndAniID, const char* pcszText
)
{
    BOOL                    bResult     = false;
	BOOL                    bRetCode    = false;
    KScene*                 pScene      = NULL;
    KBroadcastFunc          BroadcastFunc;
    S2C_USE_ITEM_PREPARE    UseItemPrepare;
    
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pcszText);

    UseItemPrepare.byProtocolID = s2c_use_item_prepare;
    UseItemPrepare.dwPlayerID   = pPlayer->m_dwID;
    UseItemPrepare.dwOTAniID    = dwOTAniID;
    UseItemPrepare.dwEndAniID   = dwEndAniID;
    UseItemPrepare.nFrames      = nFrames;

    strncpy(UseItemPrepare.szText, pcszText, sizeof(UseItemPrepare.szText));
    UseItemPrepare.szText[sizeof(UseItemPrepare.szText) - 1] = '\0';

    BroadcastFunc.m_pvData  = &UseItemPrepare;
    BroadcastFunc.m_uSize   = (DWORD)sizeof(UseItemPrepare);

    pScene = pPlayer->m_pScene;
    pScene->Broadcast(pPlayer->m_pRegion, BroadcastFunc);

    bResult = true;
Exit0:
	return bResult;
}
//AutoCode:-发送协议函数结束-

void KPlayerServer::OnHandshakeRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bResult     = false;
	BOOL                    bRetCode    = false;
	C2S_HANDSHAKE_REQUEST*  pRequest    = (C2S_HANDSHAKE_REQUEST*)pData;
    KPlayer*                pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRequest->dwRoleID);
	KGLOG_PROCESS_ERROR(pPlayer);

	KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForConnect);

	KGLOG_PROCESS_ERROR(pPlayer->m_Guid == pRequest->Guid);

    bRetCode = Attach(pPlayer, nConnIndex);
    KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_eGameStatus  = gsWaitForPermit;
	pPlayer->m_nTimer       = 0;

	g_RelayClient.DoConfirmPlayerLoginRequest(pPlayer->m_dwID);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_INFO, "Apply data error, connection: %d\n", nConnIndex);
    }
	return;
}

void KPlayerServer::OnApplyEnterScene(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
    C2S_APPLY_ENTER_SCENE*  pApply          = (C2S_APPLY_ENTER_SCENE*)pData;
    const KPARTY_INFO*      pcPartyInfo     = NULL;
    KScene*                 pScene          = NULL;
	KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);
    BOOL                    bLoginLoading   = false;
    
	assert(pPlayer);

	KGLOG_PROCESS_ERROR(
        pPlayer->m_eGameStatus == gsWaitForLoginLoading || 
        pPlayer->m_eGameStatus == gsWaitForNewMapLoading
    );

    if (pPlayer->m_eGameStatus == gsWaitForLoginLoading)
    {
        bLoginLoading = true;
    }

    pScene = g_pSO3World->GetScene(pPlayer->m_SavePosition.dwMapID, pPlayer->m_SavePosition.nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    if (pApply->nSceneTimeStamp != pScene->m_nTimeStamp)
    {
        KGLogPrintf(
            KGLOG_WARNING, "Unmatched scene data: role(%s, %u), scene(%s)\n",
            pPlayer->m_szName, pPlayer->m_dwID, pScene->m_szName
        );
    }

    if (bLoginLoading)
    {
	    g_RelayClient.DoLoadRoleDataRequest(pPlayer->m_dwID);

	    pPlayer->m_nTimer           = 0;
	    pPlayer->m_eGameStatus      = gsWaitForRoleData;

        goto Exit1;
    }

	bRetCode = g_pSO3World->AddPlayer(
        pPlayer, pScene,
        pPlayer->m_SavePosition.nX,
        pPlayer->m_SavePosition.nY,
        pPlayer->m_SavePosition.nZ
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    pScene->CallEnterSceneScript(pPlayer);

    pPlayer->m_eGameStatus      = gsPlaying;
    pPlayer->m_nFaceDirection   = pPlayer->m_SavePosition.nFaceDirection;
    pPlayer->m_nVirtualFrame    = g_pSO3World->m_nGameLoop;
    pPlayer->m_nRecordCount     = 0;

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_INFO, "Apply enter scene failed, connection: %d\n", nConnIndex);
	    g_PlayerServer.Detach(nConnIndex);
    }
	return;
}

// 玩家登出
void KPlayerServer::OnPlayerLogout(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	C2S_PLAYER_LOGOUT*  pPlayerLogout   = (C2S_PLAYER_LOGOUT*)pData;
	KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);

	KGLOG_PROCESS_ERROR(pPlayer && pPlayer->m_eGameStatus == gsPlaying);

    Detach(nConnIndex);

    KGLogPrintf(KGLOG_INFO, "Player logout: %s", pPlayer->m_szName);

Exit0:
	return;
}

void KPlayerServer::OnPingSignal(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_PING_SIGNAL* pPingSignal = (C2S_PING_SIGNAL*)pData;
    S2C_PING_SIGNAL  PingEcho;

    PingEcho.byProtocolID = s2c_ping_signal;
    PingEcho.dwTime = pPingSignal->dwTime;

    Send(nConnIndex, &PingEcho, sizeof(PingEcho));
}

void KPlayerServer::OnMoveCtrl(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bAdjust         = false;
    C2S_MOVE_CTRL*          pMoveCtrl       = (C2S_MOVE_CTRL*)pData;
	KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);

    bAdjust = pPlayer->CheckMoveAdjust(nFrame, pMoveCtrl->Param);

    pPlayer->m_MoveCtrl.nMoveCount  = pMoveCtrl->nMoveCount;
    pPlayer->m_MoveCtrl.bMove       = pMoveCtrl->bMove ? 1 : 0;
    pPlayer->m_MoveCtrl.bWalk       = pMoveCtrl->bWalk ? 1 : 0;
    pPlayer->m_MoveCtrl.nDirection8 = pMoveCtrl->byDirection8;
    pPlayer->m_MoveCtrl.nTurn       = pMoveCtrl->nTurn;
    pPlayer->m_MoveCtrl.bUpdate     = true;

    // 防止客户端传入非法参数导致同步出错(取位域)
    MAKE_IN_RANGE(pPlayer->m_MoveCtrl.nDirection8, 0, 7);
    MAKE_IN_RANGE(pPlayer->m_MoveCtrl.nTurn, -1, 1);

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[move] ctrl: frame(%d), bMove(%d), bWalk(%d), nDirection8(%d), nTurn(%d)\n",
    //    pPlayer->m_nVirtualFrame, pMoveCtrl->bMove, pMoveCtrl->bWalk, 
    //    pMoveCtrl->byDirection8, pMoveCtrl->nTurn
    //);

    if (bAdjust)
    {
        DoAdjustPlayerMove(pPlayer);
    }

	return;
}

void KPlayerServer::OnApplyMoveAdjust(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[move] apply adjust, count=%d,frame=%d<%d,%d,%d,%d>", 
    //    pPlayer->m_MoveCtrl.nMoveCount, pPlayer->m_nVirtualFrame,
    //    pPlayer->m_eMoveState, pPlayer->m_nX, pPlayer->m_nY, pPlayer->m_nZ
    //);

    DoAdjustPlayerMove(pPlayer);
}

void KPlayerServer::OnCharacterStand(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                    bAdjust         = false;
	C2S_CHARACTER_STAND*    pMoveCommand    = (C2S_CHARACTER_STAND*)pData;
	KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);

    KG_PROCESS_ERROR(!pPlayer->m_MoveCtrl.bDisable);

    bAdjust = pPlayer->CheckMoveAdjust(nFrame, pMoveCommand->Param);
    KG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_MoveCtrl.nMoveCount = pMoveCommand->nMoveCount;

    pPlayer->Stand(false);

    pPlayer->InvalidateMoveState(1, false);

    if (bAdjust)
    {
        DoAdjustPlayerMove(pPlayer);
    }

Exit0:
	return;
}

void KPlayerServer::OnCharacterJump(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bAdjust         = false;    
	C2S_CHARACTER_JUMP* pMoveCommand    = (C2S_CHARACTER_JUMP*)pData;
	KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);

    KG_PROCESS_ERROR(!pPlayer->m_MoveCtrl.bDisable);

    bAdjust = pPlayer->CheckMoveAdjust(nFrame, pMoveCommand->Param);
    KG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_MoveCtrl.nMoveCount = pMoveCommand->nMoveCount;

	pPlayer->Jump(pMoveCommand->bStandJump, pMoveCommand->byJumpDirection, false);

    if (bAdjust)
    {
        DoAdjustPlayerMove(pPlayer);
    }

Exit0:
    return;
}

void KPlayerServer::OnCharacterSit(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bAdjust         = false;    
	C2S_CHARACTER_SIT*  pMoveCommand    = (C2S_CHARACTER_SIT*)pData;
	KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);

    KG_PROCESS_ERROR(!pPlayer->m_MoveCtrl.bDisable);

    bAdjust = pPlayer->CheckMoveAdjust(nFrame, pMoveCommand->Param);
    KG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_MoveCtrl.nMoveCount = pMoveCommand->nMoveCount;

	pPlayer->SitDown(false);

    if (bAdjust)
    {
        DoAdjustPlayerMove(pPlayer);
    }

Exit0:
    return;
}

// 角色转向
void KPlayerServer::OnSyncDirection(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bAdjust         = false;   
	C2S_SYNC_DIRECTION* pPak            = (C2S_SYNC_DIRECTION*)pData;
	KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);

    KG_PROCESS_ERROR(!pPlayer->m_MoveCtrl.bDisable);

    bAdjust = pPlayer->CheckMoveAdjust(nFrame, pPak->Param);
    KG_PROCESS_ERROR(pPlayer->m_pScene);

	pPlayer->TurnTo(pPak->byFaceDirection, false);
    pPlayer->PitchTo(pPak->byPitchDirection, false);
    
    // 打断技能自动朝向
    pPlayer->m_bAutoTurnFlag = false;

    pPlayer->m_MoveCtrl.nMoveCount = pPak->nMoveCount;

    pPlayer->InvalidateMoveState(2, false);

    //KGLogPrintf(
    //    KGLOG_DEBUG, "[move] turn, count=%d,frame=%d<%d,%d,%d,%d>,face=%d", 
    //    pPak->nMoveCount, nFrame, pPlayer->m_eMoveState, 
    //    pPlayer->m_nX, pPlayer->m_nY, pPlayer->m_nZ, pPlayer->m_nFaceDirection
    //);

    if (bAdjust)
    {
        DoAdjustPlayerMove(pPlayer);
    }

Exit0:
	return;
}

void KPlayerServer::OnStopCurrentAction(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                      nRetCode           = false;
    C2S_STOP_CURRENT_ACTION* pStopCurrentAction = (C2S_STOP_CURRENT_ACTION*)pData;
    KPlayer*                 pPlayer            = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pPlayer->m_OTActionParam.eActionType == otActionSkillPrepare)
    {
        KSkill* pSkill = pPlayer->m_OTActionParam.Data.SkillRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pSkill);

        pSkill->ClearPublicCooldown(pPlayer);
    }

    if (pPlayer->m_OTActionParam.eActionType == otActionRecipePrepare)
    {
        KCraft* pCraft  = NULL;

        pCraft = g_pSO3World->m_ProfessionManager.GetCraft(pPlayer->m_OTActionParam.ProfessionSkill.dwCraftID);
	    KGLOG_PROCESS_ERROR(pCraft);

        pCraft->CallBreakScriptFunction(pPlayer, pPlayer->m_OTActionParam.ProfessionSkill.dwRecipeID);
    }

    if (pPlayer->m_OTActionParam.eActionType != otActionIdle)
    {
        pPlayer->DoActionIdle();
        DoSkillBeatBack(pPlayer, 0, true);
        goto Exit0;
    }

    if (pPlayer->m_OTActionParam.eActionType == otActionUseItemPrepare)
    {
        KItem* pItem = g_pSO3World->m_ItemSet.GetObj(pPlayer->m_OTActionParam.Data.dwItemID);
        KGLOG_PROCESS_ERROR(pItem);

        pPlayer->m_ItemList.OnBreakUseItem(pItem);
    }
    
    pPlayer->DoAutoCastSkillIdle();
    
Exit0:
    return;
}

// 角色攻击指令
void KPlayerServer::OnCharacterSkill(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	int                     nResult         = srcFailed;
	int                     nRetCode        = false;
	C2S_CHARACTER_SKILL*    pCharacterSkill = (C2S_CHARACTER_SKILL*)pData;
    KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);
	KTarget                 Target;

	assert(pPlayer);

    // 这段代码是用来减轻放冲刺等技能时"扯"的问题的
    if (pPlayer->m_nVirtualFrame > nFrame)
    {
        pPlayer->ReversePosition(pPlayer->m_nVirtualFrame - nFrame);
    }
    else if (pPlayer->m_nVirtualFrame < nFrame)
    {
        pPlayer->ForwardPosition(nFrame - pPlayer->m_nVirtualFrame);
    }
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

	switch (pCharacterSkill->byTargetType)
	{
	case ttCoordination:
        nRetCode = Target.SetTarget(pCharacterSkill->nParam1, pCharacterSkill->nParam2, pCharacterSkill->nParam3);
		KGLOG_PROCESS_ERROR(nRetCode);
        break;
    
    case ttNpc:
    case ttPlayer:
    case ttDoodad:
        nRetCode = Target.SetTarget((TARGET_TYPE)pCharacterSkill->byTargetType, (DWORD)pCharacterSkill->nParam1);
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
	default:
        KGLOG_PROCESS_ERROR(!"Unexpected target type !");
        break;
	}

    nRetCode = g_pSO3World->m_SkillManager.IsCommonSkill(pCharacterSkill->dwSkillID);
    if (nRetCode)
    {
        KSkill*                 pSkill          = NULL;
        KItem*                  pWeapon         = NULL;
        int                     nWeaponGenre    = swrtInvalid;
        KAUTO_CAST_SKILL_PARAM  AutoCaseSkillParam;
        KSKILL_RECIPE_KEY       RecipeKey;
        KSkillRecipePointer     SkillRecipePointer;

        nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, pCharacterSkill->dwSkillID, 1, pPlayer);
        KGLOG_PROCESS_ERROR(nRetCode);

        pSkill = SkillRecipePointer.SetRecipeKey(RecipeKey);
        KGLOG_PROCESS_ERROR(pSkill);

        // 普通攻击对武器的需求不可能是"任意"
        KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->dwWeaponRequest > 0);

        nWeaponGenre = g_pSO3World->m_Settings.m_WeaponTypeList.GetWeaponGenre(pSkill->m_pBaseInfo->dwWeaponRequest);
        
        switch (nWeaponGenre)
        {
        case swrtMeleeWeapon:
            AutoCaseSkillParam.pnCastCycle   = &pPlayer->m_nMeleeWeaponAttackSpeed;
            AutoCaseSkillParam.bMelee = true;
            break;

        case swrtRangeWeapon:
            pWeapon = pPlayer->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
            KG_PROCESS_ERROR(pWeapon);

            AutoCaseSkillParam.pnCastCycle   = &pPlayer->m_nRangeWeaponAttackSpeed;
            AutoCaseSkillParam.bMelee = false;
            break;

        default:
            assert(false);
            goto Exit0;
        }

        AutoCaseSkillParam.SkillRecipePointer = SkillRecipePointer;

        pPlayer->m_SelectTarget = Target;
        pPlayer->DoAutoCastSkill(AutoCaseSkillParam);
        goto Exit1;
    }

	// 服务器要发的技能都是合法的，外挂只能通过协议影响，所以只在这儿检查。
    nRetCode = g_pSO3World->m_SkillManager.IsInstinct(pCharacterSkill->dwSkillID, pCharacterSkill->bySkillLevel);
    if (!nRetCode)
    {
        nRetCode = pPlayer->m_SkillList.IsSkillExist(
            pCharacterSkill->dwSkillID, pCharacterSkill->bySkillLevel
        );
        KGLOG_PROCESS_ERROR_RET_CODE(nRetCode, srcInvalidSkill);
    }

	nRetCode = pPlayer->CastSkill(
        pCharacterSkill->dwSkillID, pCharacterSkill->bySkillLevel, Target
    );
	KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    nRetCode = g_pSO3World->m_SkillManager.IsAccumulate(pCharacterSkill->dwSkillID, pCharacterSkill->bySkillLevel);
    if (nRetCode)
    {
        nRetCode = pPlayer->CastSkillAccumulate(pCharacterSkill->dwSkillID, pCharacterSkill->bySkillLevel, Target);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    }

Exit1:
	nResult = srcSuccess;
Exit0:

    if (nResult != srcSuccess)
    {
		DoMessageNotify(pPlayer->m_nConnIndex, ectSkillErrorCode, nResult);
    }
	return;
}

// 角色交互动作
void KPlayerServer::OnCharacterAction(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                    bRetCode            = false;
	C2S_CHARACTER_ACTION*   pCharacterAction    = (C2S_CHARACTER_ACTION*)pData;
	KPlayer*                pPlayer             = NULL;
        
    pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->Action(pCharacterAction->dwCharacterID, pCharacterAction->dwAction);
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}


// 玩家显示数据
void KPlayerServer::OnApplyPlayerDisplayData(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	C2S_APPLY_PLAYER_DISPLAY_DATA* pApplyPlayerDisplayData = (C2S_APPLY_PLAYER_DISPLAY_DATA*)pData;

	DoPlayerDisplayData(pApplyPlayerDisplayData->dwPlayerID, pApplyPlayerDisplayData->dwCheckSum, nConnIndex);
}

void KPlayerServer::OnMountKungfu(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int               nResult               = srcFailed;
    int               nRetCode              = false;
    C2S_MOUNT_KUNGFU* pPak                  = (C2S_MOUNT_KUNGFU*)pData;
    KPlayer*          pPlayer               = NULL;
    KSkill*           pOldMountKungfu       = NULL;
    KSkill*           pKungfu               = NULL;
    BOOL              bIsSameSchool         = false;
    int               nDuration             = 0;

	pPlayer = GetPlayerByConnection(nConnIndex);
    KG_ASSERT_EXIT(pPlayer);

    KG_PROCESS_SUCCESS(pPak->dwID == pPlayer->m_SkillList.m_dwMountKungfuID);

    pOldMountKungfu = g_pSO3World->m_SkillManager.GetSkill_K(
        pPlayer->m_SkillList.m_dwMountKungfuID, pPlayer->m_SkillList.m_dwMountKungfuLevel
    );
    // 这里pOldMountKungfu为空是正常流程，使用之前必须注意!

    if (pPak->dwID == INVALID_SKILL_ID) // 如果是卸下内功流程
    {
        int nDuration = 0;

        KGLOG_PROCESS_ERROR(pOldMountKungfu); // 没有内功的时候卸下内功，是不可能的。

        nDuration = pPlayer->GetCDValue(pOldMountKungfu->m_dwCoolDownID[OTHER_SCHOOL_KUNGFU_CD_INDEX]);
        nDuration += pOldMountKungfu->m_nCoolDownAlter[OTHER_SCHOOL_KUNGFU_CD_INDEX];
        nDuration = max(0, nDuration);

        pPlayer->m_TimerList.ResetTimer(pOldMountKungfu->m_dwCoolDownID[OTHER_SCHOOL_KUNGFU_CD_INDEX], nDuration);

        nRetCode = pPlayer->UmountKungfu();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }

    pKungfu = g_pSO3World->m_SkillManager.GetSkill_K(pPak->dwID, pPak->dwLevel);
    KGLOG_PROCESS_ERROR_RET_CODE(pKungfu, srcInvalidSkill);

    nRetCode = pPlayer->m_SkillList.IsSkillExist(pPak->dwID, pPak->dwLevel);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (pOldMountKungfu && pOldMountKungfu->m_pBaseInfo->dwBelongSchool == pKungfu->m_pBaseInfo->dwBelongSchool)
    {
        bIsSameSchool   = true;
    }

    if (bIsSameSchool)
    {
        nRetCode = pPlayer->m_TimerList.CheckTimer(pKungfu->m_dwCoolDownID[SAME_SCHOOL_KUNGFU_CD_INDEX]);
        KG_PROCESS_ERROR_RET_CODE(nRetCode, srcSkillNotReady);
    }
    else
    {
        nRetCode = pPlayer->m_TimerList.CheckTimer(pKungfu->m_dwCoolDownID[OTHER_SCHOOL_KUNGFU_CD_INDEX]);
        KG_PROCESS_ERROR_RET_CODE(nRetCode, srcSkillNotReady);
    }

    if (pPlayer->m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID)
    {
        nRetCode = pPlayer->UmountKungfu();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = pPlayer->MountKungfu(pPak->dwID, pPak->dwLevel);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (bIsSameSchool)
    {
        nDuration = pPlayer->GetCDValue(pKungfu->m_dwCoolDownID[SAME_SCHOOL_KUNGFU_CD_INDEX]);
        nDuration += pKungfu->m_nCoolDownAlter[SAME_SCHOOL_KUNGFU_CD_INDEX];
        nDuration = max(0, nDuration);

        pPlayer->m_TimerList.ResetTimer(pKungfu->m_dwCoolDownID[SAME_SCHOOL_KUNGFU_CD_INDEX], nDuration);
    }
    else
    {
        nDuration = pPlayer->GetCDValue(pKungfu->m_dwCoolDownID[SAME_SCHOOL_KUNGFU_CD_INDEX]);
        nDuration += pKungfu->m_nCoolDownAlter[SAME_SCHOOL_KUNGFU_CD_INDEX];
        nDuration = max(0, nDuration);

        pPlayer->m_TimerList.ResetTimer(pKungfu->m_dwCoolDownID[SAME_SCHOOL_KUNGFU_CD_INDEX], nDuration);

        nDuration = pPlayer->GetCDValue(pKungfu->m_dwCoolDownID[OTHER_SCHOOL_KUNGFU_CD_INDEX]);
        nDuration += pKungfu->m_nCoolDownAlter[OTHER_SCHOOL_KUNGFU_CD_INDEX];
        nDuration = max(0, nDuration);

        pPlayer->m_TimerList.ResetTimer(pKungfu->m_dwCoolDownID[OTHER_SCHOOL_KUNGFU_CD_INDEX], nDuration);
    }

Exit1:
    nResult = srcSuccess;
Exit0:

    if (nResult != srcSuccess)
    {
        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectSkillErrorCode, nResult);
    }
    return;
}

void KPlayerServer::OnCancelBuff(char *pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_CANCEL_BUFF*    pPak    = (C2S_CANCEL_BUFF*)pData;
    KPlayer*            pPlayer = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_BuffList.CancelBuff(pPak->nIndex);

Exit0:
    return;
}


void KPlayerServer::OnSetSheathState(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_SET_SHEATH_STATE* pPak       = (C2S_SET_SHEATH_STATE*)pData;
    KPlayer*              pPlayer    = GetPlayerByConnection(nConnIndex);

    KGLOG_PROCESS_ERROR(pPlayer);
    // 战斗状态不允许玩家插拔武器
    KG_PROCESS_ERROR(!pPlayer->m_bFightState);

    pPlayer->m_bSheathFlag = pPak->bSheathFlag;

    DoSyncSheathState(pPlayer);

Exit0:
    return;
}

void KPlayerServer::OnSetHideHatFlag(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode        = false;
    C2S_SET_HIDE_HAT_FLAG*      pPak            = (C2S_SET_HIDE_HAT_FLAG*)pData;
    KPlayer*                    pPlayer         = GetPlayerByConnection(nConnIndex);
    KItem*                      pItem           = NULL;
    int                         nRepresentID    = 0;

    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_bHideHat = pPak->bHideHat;

    if (!pPlayer->m_bHideHat)
    {
        pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitHelm);

        if (pItem)
        {
            nRepresentID = (WORD)pItem->m_nRepresentID;
        }
    }

    if (pPlayer->m_wRepresentId[perHelmStyle] != (WORD)nRepresentID)
    {
        DoSyncEquipRepresent(pPlayer, perHelmStyle, nRepresentID);
        pPlayer->m_wRepresentId[perHelmStyle] = (WORD)nRepresentID;
    }
    
Exit0:
    return;
}

void KPlayerServer::OnApplyPlayerBuffList(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_APPLY_PLAYER_BUFFLIST*  pPak        = (C2S_APPLY_PLAYER_BUFFLIST*)pData;
    KPlayer*                    pPlayer     = NULL;
    KPlayer*                    pDstPlayer  = GetPlayerByConnection(nConnIndex);

    KGLOG_PROCESS_ERROR(pDstPlayer);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    DoSyncBuffList(pPlayer, pDstPlayer);

Exit0:
    return;
}

void KPlayerServer::OnInviteJoinTeamRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                            bRetCode                = false;
    C2S_INVITE_JOIN_TEAM_REQUEST*   pInviteJoinTeamRequest  = (C2S_INVITE_JOIN_TEAM_REQUEST*)pData;
    KPlayer*                        pPlayerSrc              = NULL;    

	pPlayerSrc = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayerSrc);
    
	pInviteJoinTeamRequest->szInvitedPlayer[sizeof(pInviteJoinTeamRequest->szInvitedPlayer) - 1] = '\0';
    g_RelayClient.DoInviteJoinTeamRequest(pPlayerSrc->m_dwID, pInviteJoinTeamRequest->szInvitedPlayer);
	
Exit0:
    return;
}

void KPlayerServer::OnInviteJoinTeamRespond(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                  bRetCode                = false;
    C2S_INVITE_JOIN_TEAM_RESPOND*         pInviteJoinTeamRespond = (C2S_INVITE_JOIN_TEAM_RESPOND*)pData;
    KPlayer*                              pPlayer                 = GetPlayerByConnection(nConnIndex);

    pInviteJoinTeamRespond->szInviteSrc[sizeof(pInviteJoinTeamRespond->szInviteSrc) - 1] = '\0';

    g_RelayClient.DoInviteJoinTeamRespond(
        pInviteJoinTeamRespond->szInviteSrc, pPlayer->m_dwID, pInviteJoinTeamRespond->bAgreeFlag
    );

Exit0:
    return;
}

void KPlayerServer::OnApplyJoinTeamRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_APPLY_JOIN_TEAM_REQUEST*  pApplyJoinTeam  = (C2S_APPLY_JOIN_TEAM_REQUEST*)pData;
    KPlayer*                      pPlayerSrc      = NULL;

    pApplyJoinTeam->szApplyDst[sizeof(pApplyJoinTeam->szApplyDst) - 1] = '\0';

	pPlayerSrc = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayerSrc);

    // 交给Relay去处理
    g_RelayClient.DoApplyJoinTeamRequest(pPlayerSrc->m_dwID, pApplyJoinTeam->szApplyDst);
Exit0:
    return;
}

void KPlayerServer::OnApplyJoinTeamRespond(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_APPLY_JOIN_TEAM_RESPOND* pApplyRespond = (C2S_APPLY_JOIN_TEAM_RESPOND*)pData;
    KPlayer*                      pPlayer       = NULL;

    pApplyRespond->szApplySrc[sizeof(pApplyRespond->szApplySrc) - 1] = '\0';

	pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    g_RelayClient.DoApplyJoinTeamRespond(
        pPlayer->m_dwID, pApplyRespond->szApplySrc, pApplyRespond->bAgreeFlag
    );
Exit0:
    return;
}


void KPlayerServer::OnTeamDelMemberRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                          bRetCode               = false;
    C2S_TEAM_DEL_MEMBER_REQUEST*  pPartyDelMemberRequest = (C2S_TEAM_DEL_MEMBER_REQUEST*)pData;
    KPlayer*                      pPlayer                = NULL;

	pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_RelayClient.DoTeamDelMemberRequest(
        pPlayer->m_dwTeamID, pPlayer->m_dwID, pPartyDelMemberRequest->dwMemberID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnTeamChangeAuthorityRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                bRetCode             = false;
    C2S_TEAM_CHANGE_AUTHORITY_REQUEST*  pChangeLeaderRequest = (C2S_TEAM_CHANGE_AUTHORITY_REQUEST*)pData;
    KPlayer*                            pPlayer              = NULL;

	pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    // 注: 移交的目标玩家的状态的正确性Relay会核准的

    bRetCode = g_RelayClient.DoTeamChangeAuthorityRequest(
        pPlayer->m_dwTeamID, pPlayer->m_dwID, (KTEAM_AUTHORITY_TYPE)pChangeLeaderRequest->byType, pChangeLeaderRequest->dwNewLeaderID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnTeamSetLootMode(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                bRetCode     = false;
    C2S_TEAM_SET_LOOT_MODE_REQUEST*     pRequest     = (C2S_TEAM_SET_LOOT_MODE_REQUEST*)pData;
    KPlayer*                            pPlayer      = NULL;
    KTeam*                              pTeam        = NULL;

	pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(pPlayer->m_dwID == pTeam->dwAuthority[tatDistribute]); //只有分配者才能修改分配模式

    bRetCode = g_RelayClient.DoTeamSetLootModeRequest(pPlayer->m_dwTeamID, pPlayer->m_dwID, pRequest->nLootMode);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnTeamSetRollQuality(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL	                            bRetCode    = false;
	C2S_TEAM_SET_ROLL_QUALITY_REQUEST*  pRequest    = (C2S_TEAM_SET_ROLL_QUALITY_REQUEST*)pData;
    KPlayer*                            pPlayer     = NULL;
    KTeam*                              pTeam       = NULL;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(pPlayer->m_dwID == pTeam->dwAuthority[tatDistribute]); //只有分配者才能修改分配模式

	g_RelayClient.DoTeamSetRollQualityRequest(pPlayer->m_dwTeamID, pPlayer->m_dwID, pRequest->nRollQuality);

Exit0:
	return;
}

void KPlayerServer::OnTeamSetFormationLeader(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL	                                bRetCode            = false;
	C2S_TEAM_SET_FORMATION_LEADER_REQUEST*  pRequest            = (C2S_TEAM_SET_FORMATION_LEADER_REQUEST*)pData;
    BOOL                                    bIsFormationLeader  = false;
	KPlayer*                                pPlayer             = NULL;
    KTeam*                                  pTeam               = NULL;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);
    
    KGLOG_PROCESS_ERROR(pRequest->nGroupIndex < pTeam->nGroupNum);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        if (pPlayer->m_dwID == pTeam->MemberGroup[i].dwFormationLeader)
        {
            bIsFormationLeader = true;
            break;
        }
    }

    KGLOG_PROCESS_ERROR(bIsFormationLeader || pPlayer->m_dwID == pTeam->dwAuthority[tatLeader]);

	g_RelayClient.DoTeamSetFormationLeader(
        pPlayer->m_dwTeamID, pPlayer->m_dwID, pRequest->dwNewFormationLeaderID, pRequest->nGroupIndex
    );

Exit0:
	return;
}

void KPlayerServer::OnTeamMemberNotifySignpost(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                        bRetcode         = false;
    KPlayer*                                    pPlayer          = NULL;
    KPlayer*                                    pMemberPlayer    = NULL;
    KTeam*                                      pTeam            = NULL;
    C2S_TEAM_MEMBER_NOTIFY_SIGNPOST_REQUEST*    pRequest         = (C2S_TEAM_MEMBER_NOTIFY_SIGNPOST_REQUEST*)pData;
    S2C_TEAM_MEMBER_SIGNPOST_NOTIFY             Response;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);
    KG_PROCESS_ERROR(pPlayer->m_pScene != NULL);

    Response.byProtocolID = s2c_team_member_signpost_notify;
    Response.nX           = pRequest->nX;
    Response.nY           = pRequest->nY;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == pPlayer->m_dwID)
            {
                continue;
            }

            pMemberPlayer = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
            if (pMemberPlayer == NULL)
            {
                continue;
            }

            if (pMemberPlayer->m_pScene == NULL)
            {
                continue;
            }

            // 只发给在同一地图内的队友
            if (pPlayer->m_pScene != pMemberPlayer->m_pScene)
            {
                continue;
            }

            Send(pMemberPlayer->m_nConnIndex, &Response, sizeof(S2C_TEAM_MEMBER_SIGNPOST_NOTIFY));
        }
    }

Exit0:
    return;
}

void KPlayerServer::OnTeamSetMarkRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_TEAM_SET_MARK_REQUEST*  pSetPartyMarkRequest = (C2S_TEAM_SET_MARK_REQUEST*)pData;
    KPlayer*                    pPlayer              = NULL;
    KTeam*                      pTeam                = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KG_PROCESS_ERROR(pTeam->dwAuthority[tatLeader] == pPlayer->m_dwID);

    g_RelayClient.DoTeamSetMarkRequest(
        pPlayer->m_dwTeamID, pPlayer->m_dwID, pSetPartyMarkRequest->nMarkType, pSetPartyMarkRequest->dwTargetID
    );

Exit0:
    return;
}

void KPlayerServer::OnTeamLevelUpRaidRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_TEAM_LEVEL_UP_RAID_REQUEST* pRequest = (C2S_TEAM_LEVEL_UP_RAID_REQUEST*)pData;
    KPlayer*                        pPlayer  = NULL;
    KTeam*                          pTeam    = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KG_PROCESS_ERROR(pTeam->dwAuthority[tatLeader] == pPlayer->m_dwID);
    KGLOG_PROCESS_ERROR(pTeam->nGroupNum == 1);

    g_RelayClient.DoTeamLevelUpRequest(pPlayer->m_dwTeamID, pPlayer->m_dwID);
Exit0:
    return;
}

void KPlayerServer::OnTeamChangeMemberGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_TEAM_CHANGE_MEMBER_GROUP_REQUEST*   pRequest = (C2S_TEAM_CHANGE_MEMBER_GROUP_REQUEST*)pData;
    KPlayer*                                pPlayer  = NULL;
    KTeam*                                  pTeam    = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KG_PROCESS_ERROR(pTeam->dwAuthority[tatLeader] == pPlayer->m_dwID);
    KG_PROCESS_ERROR(pRequest->nDstGroupIndex >= 0 && pRequest->nDstGroupIndex <= pTeam->nGroupNum);

    g_RelayClient.DoTeamChangeMemberGroupRequest(
        pPlayer->m_dwTeamID, pRequest->dwMemberID, pRequest->nDstGroupIndex, pPlayer->m_dwID
    );
Exit0:
    return;
}

// 接受任务
void KPlayerServer::OnAcceptQuest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int               nResult       = qrcFailed;
	int               nRetCode      = false;
	TARGET_TYPE       eTargetType   = ttInvalid;
    KTarget           Target;
    C2S_ACCEPT_QUEST* pAcceptQuest  = (C2S_ACCEPT_QUEST*)pData;

	KPlayer* pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

    eTargetType = (TARGET_TYPE)pAcceptQuest->byTargetType;
    switch (eTargetType)
    {
	case ttNoTarget:
		Target.ClearTarget();
		break;
    case ttCoordination:
        KGLOG_PROCESS_ERROR(false);
		break;
	case ttItem:
    case ttNpc:
    case ttPlayer:
    case ttDoodad:
        nRetCode = Target.SetTarget(eTargetType, pAcceptQuest->dwTargetID);
        KGLOG_PROCESS_ERROR_RET_CODE(nRetCode, qrcFailed);
        break;

    default:
        KGLOG_PROCESS_ERROR(false);
    }

	nRetCode = pPlayer->m_QuestList.Accept(&Target, pAcceptQuest->dwQuestID);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);

    nResult = qrcSuccess;
Exit0:
	DoMessageNotify(nConnIndex, ectQuestErrorCode, (BYTE)nResult);
	return;
}

//完成任务
void KPlayerServer::OnFinishQuest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                 nResult         = qrcFailed;
	int                 nRetCode        = false;
    TARGET_TYPE         eTargetType     = ttInvalid;
	C2S_FINISH_QUEST*   pFinishQuest    = (C2S_FINISH_QUEST*)pData;
	KTarget             Target;
    KPlayer*            pPlayer         = NULL;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

    eTargetType = (TARGET_TYPE)pFinishQuest->dwTargetType;
    KGLOG_PROCESS_ERROR(eTargetType == ttNpc || eTargetType == ttDoodad);
    
    nRetCode = Target.SetTarget(eTargetType, pFinishQuest->dwTargetID);
    KGLOG_PROCESS_ERROR_RET_CODE(nRetCode, qrcFailed);

	nRetCode = pPlayer->m_QuestList.Finish(
        pFinishQuest->dwQuestID, Target,
		pFinishQuest->nPresentChoice1, pFinishQuest->nPresentChoice2
    );
    KG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);

    nResult = qrcSuccess;
Exit0:
	DoMessageNotify(nConnIndex, ectQuestErrorCode, (BYTE)nResult);
	return;
}

//取消任务
void KPlayerServer::OnCancelQuest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	QUEST_RESULT_CODE nRetCode = qrcFailed;
	C2S_CANCEL_QUEST* pCancelQuest = (C2S_CANCEL_QUEST*)pData;

	KPlayer* pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	nRetCode = pPlayer->m_QuestList.Cancel(pCancelQuest->nQuestIndex);
	
Exit0:
	DoMessageNotify(nConnIndex, (BYTE)ectQuestErrorCode, (BYTE)nRetCode);

	return;
}

void KPlayerServer::OnShareQuest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL             bRetCode       = false;
	KPlayer*         pPlayer        = NULL;
    DWORD            dwQuestID      = 0;
    C2S_SHARE_QUEST* pShareQuest    = (C2S_SHARE_QUEST*)pData;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	dwQuestID = pPlayer->m_QuestList.GetQuestID(pShareQuest->nQuestIndex);
	KGLOG_PROCESS_ERROR(dwQuestID > 0);

	bRetCode = pPlayer->ShareQuest(dwQuestID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

// 接受护送任务
void KPlayerServer::OnAcceptEscortQuest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int						 nResult			= false;
    int						 nRetCode			= false;
	KPlayer*				 pPlayer			= NULL;
	KPlayer*				 pStarter			= NULL;
	KQuestInfo*				 pQuestInfo			= NULL;
	C2S_ACCEPT_ESCORT_QUEST* pAcceptEscortQuest	= (C2S_ACCEPT_ESCORT_QUEST*)pData;
	KTarget					 Target;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);
	KGLOG_PROCESS_ERROR(pPlayer->m_pScene);
	
	pStarter = g_pSO3World->m_PlayerSet.GetObj(pAcceptEscortQuest->dwStartPlayerID);
	KG_PROCESS_ERROR(pStarter);
    KG_PROCESS_ERROR(pStarter->m_pScene);
    
    KG_PROCESS_ERROR(pStarter->m_dwTeamID == pPlayer->m_dwTeamID && pPlayer->m_dwTeamID != ERROR_ID);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(pAcceptEscortQuest->dwQuestID);
	KG_PROCESS_ERROR(pQuestInfo);
	KG_PROCESS_ERROR(pQuestInfo->bEscortQuest);

	nRetCode = Target.SetTarget(pStarter);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pStarter->m_pScene->m_EscortQuestMgr.AddPlayer(pAcceptEscortQuest->dwQuestID, pPlayer->m_dwID);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pPlayer->m_QuestList.Accept(&Target, pAcceptEscortQuest->dwQuestID);
	KG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);

	nResult = qrcSuccess;
Exit0:
	DoMessageNotify(nConnIndex, ectQuestErrorCode, (BYTE)nResult);
	return;
}

void KPlayerServer::OnGmCommand(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL            bRetCode        = false;
	C2S_GM_COMMAND* pGmCommand      = (C2S_GM_COMMAND*)pData;
    KPlayer*        pPlayer         = GetPlayerByConnection(nConnIndex);

    KGLOG_PROCESS_ERROR(nSize > sizeof(C2S_GM_COMMAND));
    pGmCommand->szGmCommand[nSize - sizeof(C2S_GM_COMMAND) - 1] = '\0';

    if (m_nClientGM == 0)
    {
        goto Exit0;
    }

    if (m_nClientGM != 1)
    {
        bRetCode = g_pSO3World->m_Settings.m_GMList.CheckPermission(pPlayer->m_szAccount);
        KG_PROCESS_ERROR(bRetCode);
    }

    KGLOG_PROCESS_ERROR(pPlayer && pPlayer->m_eGameStatus == gsPlaying);

    KGLogPrintf(KGLOG_INFO, "GM(%u, %d): %s\n", pPlayer->m_dwID, nConnIndex, pGmCommand->szGmCommand);

	if (strcmp(pGmCommand->szGmCommand, "ReloadAllScripts();") == 0)
	{
        g_pSO3World->m_ScriptCenter.ReloadAll();        
        goto Exit0;
	}

	bRetCode = g_pSO3World->m_ScriptCenter.ExecutePlayerScript(pPlayer, pGmCommand->szGmCommand);
	KGLOG_PROCESS_ERROR(bRetCode);
	
Exit0:
	return;
}

void KPlayerServer::OnOpenShopRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                     nRetCode            = false;
    DWORD                   dwShopID            = 0;
    KNpc*                   pNpc                = NULL;
    const KShop*            pcShop              = NULL;
    KPlayer*                pPlayer             = NULL;
    C2S_OPEN_SHOP_REQUEST*  pRequest            = (C2S_OPEN_SHOP_REQUEST *)pData;
    int                     nRelation           = 0;

    dwShopID = pRequest->dwShopID;
    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);

    pNpc = pcShop->m_pNpc;
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pNpc->m_dwID == pcShop->m_dwNpcID);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene == pNpc->m_pScene);
    
    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR(nRetCode);

    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    DoOpenShopNotify(nConnIndex, dwShopID);

Exit0:
    return;
}

void KPlayerServer::OnQueryShopItems(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                     nRetCode    = false;
    C2S_QUERY_SHOP_ITEMS*   pRequest    = (C2S_QUERY_SHOP_ITEMS *)pData;
    const KShop*            pcShop      = NULL;
    KPlayer*                pPlayer     = NULL;
    KNpc*                   pNpc        = NULL;

    pcShop = g_pSO3World->m_ShopCenter.GetShopInfo(pRequest->dwShopID);
    KGLOG_PROCESS_ERROR(pcShop);

    KGLOG_PROCESS_ERROR(pRequest->byPageIndex < pcShop->m_dwValidPage);

    for (int nPosIndex = 0; nPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nPosIndex)
    {
        const KSHOP_ITEM*   pcShopItem = &pcShop->m_ShopPages[pRequest->byPageIndex].ShopItems[nPosIndex];
        DWORD               dwItemID   = 0;

        if (pcShopItem->pItem)
        {
            dwItemID = pcShopItem->pItem->m_dwID;
        }

        if (dwItemID != pRequest->ShopPageItems[nPosIndex].dwItemID)
        {
            DoSyncShopItem(nConnIndex, pcShop, pRequest->byPageIndex, nPosIndex, true);
            continue;
        }
        
        KNPC_SHOP_TEMPLATE_ITEM* pItemTemplate = g_pSO3World->m_ShopCenter.GetShopTemplateItem(
            pcShop->m_dwTemplateID, pcShopItem->nItemTemplateIndex
        );
        KGLOG_PROCESS_ERROR(pItemTemplate);

        if (pcShopItem->nCount != pRequest->ShopPageItems[nPosIndex].nCount)
        {
            DoSyncShopItem(nConnIndex, pcShop, pRequest->byPageIndex, nPosIndex, false);
            continue;
        }
    }

Exit0:
    return;
}

void KPlayerServer::OnShopBuyRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                     nRetCode            = false;
    C2S_SHOP_BUY_REQUEST*   pRequest            = (C2S_SHOP_BUY_REQUEST *)pData;
    KPlayer*                pPlayer             = NULL;
    KShop*                  pShop               = NULL;
    KNpc*                   pNpc                = NULL;
    int                     nRelation           = 0;
    KSHOP_BUY_ITEM_PARAM    Param;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);
    
    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(pRequest->dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    pNpc = pShop->m_pNpc;
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene == pNpc->m_pScene);

    Param.dwItemID      = pRequest->dwItemID;
    Param.dwPageIndex   = (DWORD)pRequest->byPageIndex;
    Param.dwPosIndex    = (DWORD)pRequest->byPosIndex;
    Param.nCount        = pRequest->nCount;
    Param.nCost         = pRequest->nCost;

    nRetCode = pShop->BuyItem(pPlayer, Param);
    KGLOG_PROCESS_ERROR(nRetCode);

    DoMessageNotify(nConnIndex, ectShopEventNotifyCode, ssrcBuySuccess, NULL, 0);

Exit0:
    return;
}

void KPlayerServer::OnShopSellRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                     nRetCode        = false;
    C2S_SHOP_SELL_REQUEST*  pRequest        = (C2S_SHOP_SELL_REQUEST *)pData;
    KShop*                  pShop           = NULL;
    KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);
    KNpc*                   pNpc            = NULL;
    int                     nRelation       = 0;
    int                     nDistance3      = 0;
    int                     nPlayerZCell    = 0;
    int                     nNpcZCell       = 0;

    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(pRequest->dwShopID);
    KGLOG_PROCESS_ERROR(pShop);
    
    // 是否在同个场景
    pNpc = pShop->m_pNpc;
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene == pNpc->m_pScene);

    // 距离
    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR(nRetCode);

    // 玩家与NPC的关系
    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));
  
    {
        KSHOP_SELL_ITEM_PARAM Param;
        Param.dwBox = pRequest->dwBoxIndex;
        Param.dwX = pRequest->dwPosIndex;
        Param.dwItemID = pRequest->dwItemID;
        Param.nCost = pRequest->nCost;

        nRetCode = pShop->SellItem(pPlayer, Param);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    DoMessageNotify(nConnIndex, ectShopEventNotifyCode, ssrcSellSuccess, NULL, 0);

Exit0:

    return;
}

void KPlayerServer::OnShopRepairRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                         nRetCode        = 0;
    C2S_SHOP_REPAIR_REQUEST*    pRequest        = (C2S_SHOP_REPAIR_REQUEST *)pData;
    KShop*                      pShop           = NULL;
    KPlayer*                    pPlayer         = GetPlayerByConnection(nConnIndex);
    KNpc*                       pNpc            = NULL;
    int                         nRelation       = 0;
    int                         nOldMoney       = 0;
    int                         nRepairMoney    = 0;

    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(pRequest->dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    pNpc = pShop->m_pNpc;
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene == pNpc->m_pScene);

    nRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR(nRetCode);

    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    nOldMoney = pPlayer->m_ItemList.m_nMoney;

    if (pRequest->dwItemID == ERROR_ID)
    {
        nRetCode = pShop->RepairAllItems(pPlayer, pRequest->nCost);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KSHOP_REPAIR_ITEM_PARAM Param;
        Param.dwBox     = pRequest->dwBoxIndex;
        Param.dwX       = pRequest->dwPosIndex;
        Param.dwItemID  = pRequest->dwItemID;
        Param.nCost     = pRequest->nCost;

        nRetCode = pShop->RepairItem(pPlayer, Param);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRepairMoney = nOldMoney - pPlayer->m_ItemList.m_nMoney;

    DoMessageNotify(nConnIndex, ectShopEventNotifyCode, ssrcRepairSuccess, &nRepairMoney, sizeof(nRepairMoney));

Exit0:
    return;
}

void KPlayerServer::OnSoldListBuyRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode        = false;
    KShop*                      pShop           = NULL;
    KPlayer*                    pPlayer         = NULL;
    KNpc*                       pNpc            = NULL;
    C2S_SOLD_LIST_BUY_REQUEST*  pRequest        = (C2S_SOLD_LIST_BUY_REQUEST*)pData;
    int                         nRelation       = 0;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    pShop = g_pSO3World->m_ShopCenter.GetShopInfo(pRequest->dwShopID);
    KGLOG_PROCESS_ERROR(pShop);

    pNpc = pShop->m_pNpc;
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene == pNpc->m_pScene);

    bRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_SHOP_DISTANCE);
    KG_PROCESS_ERROR(bRetCode);

    nRelation = pPlayer->GetNpcRelation(pNpc);
    KGLOG_PROCESS_ERROR(!(nRelation & sortEnemy));

    {
        KSHOP_BUY_SOLD_LIST_ITEM_PARAM Param;
        Param.dwX       = pRequest->dwX;
        Param.dwItemID  = pRequest->dwItemID;

        bRetCode = pShop->BuySoldListItem(pPlayer, Param);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return;
}

void KPlayerServer::OnTradingInviteRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                         nResult         = trcInvalid;
    int                         nRetCode        = false;
    KPlayer*                    pInviteSrc      = NULL;
    KPlayer*                    pInviteDst      = NULL;
    int                         nRelation       = 0;
    C2S_TRADING_INVITE_REQUEST* pRequest        = (C2S_TRADING_INVITE_REQUEST*)pData;
       
    pInviteSrc = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pInviteSrc);

    KG_PROCESS_ERROR(pInviteSrc->m_eGameStatus == gsPlaying);
    KG_PROCESS_ERROR_RET_CODE(pInviteSrc->m_eMoveState != cmsOnDeath, trcYouDead);

    // 交易发起人不应处于交易状态
    KG_PROCESS_ERROR_RET_CODE(!pInviteSrc->m_pTradingBox, trcYouBusy);
    
    pInviteDst = g_pSO3World->m_PlayerSet.GetObj(pRequest->dwInviteDst);
    KG_PROCESS_ERROR_RET_CODE(pInviteDst, trcTargetNotInGame);
    KG_PROCESS_ERROR_RET_CODE(pInviteDst->m_eGameStatus == gsPlaying, trcTargetNotInGame);
    KG_PROCESS_ERROR_RET_CODE(pInviteDst->m_eMoveState != cmsOnDeath, trcTargetDead);
    
    KG_PROCESS_ERROR(pInviteSrc != pInviteDst);

    // 被邀请人不应处于交易状态
    KG_PROCESS_ERROR_RET_CODE((!pInviteDst->m_pTradingBox), trcTargetBusy);

    // 或被邀请交易状态
    KG_PROCESS_ERROR_RET_CODE(pInviteDst->m_dwTradingInviteSrc == ERROR_ID, trcTargetBusy);

    // 或邀请交易状态
    KG_PROCESS_ERROR_RET_CODE(pInviteDst->m_dwTradingInviteDst == ERROR_ID, trcTargetBusy);
    
    nRelation = pInviteSrc->GetPlayerRelation(pInviteDst);
    KG_PROCESS_ERROR_RET_CODE(!(nRelation & sortEnemy), trcInvalidTarget);

    // 交易位置检测
    KG_PROCESS_ERROR(pInviteSrc->m_pScene == pInviteDst->m_pScene);

    nRetCode = g_InRange(pInviteSrc, pInviteDst, MAX_TRADING_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, trcTooFar);

    pInviteSrc->m_dwTradingInviteDst = pInviteDst->m_dwID;
    pInviteDst->m_dwTradingInviteSrc = pInviteSrc->m_dwID;

    nRetCode = DoTradingInviteRequest(pInviteSrc, pInviteDst);
    KG_PROCESS_ERROR(nRetCode);
    
    nResult = trcSuccess;
Exit0:
    if (pInviteSrc && nResult != trcSuccess)
    {
        DoMessageNotify(pInviteSrc->m_nConnIndex, ectTradingErrorCode, nResult);
    }
    return;
}

void KPlayerServer::OnTradingInviteRespond(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                         nResult                 = trcFailed;
    int                         nRetCode                = false;
    KPlayer*                    pInviteSrc              = NULL;
    KPlayer*                    pInviteDst              = NULL;
    KTradingBox*                pNewTradingBoxSrc       = NULL;
    BOOL                        bNewTradingBoxSrcInit   = false;
    KTradingBox*                pNewTradingBoxDst       = NULL;
    BOOL                        bNewTradingBoxDstInit   = false;
    int                         nRelation               = 0;
    C2S_TRADING_INVITE_RESPOND* pRespond                = (C2S_TRADING_INVITE_RESPOND*)pData;

    pInviteDst = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pInviteDst);

    KG_PROCESS_ERROR(pInviteDst->m_eGameStatus == gsPlaying);  
    
    pInviteSrc = g_pSO3World->m_PlayerSet.GetObj(pInviteDst->m_dwTradingInviteSrc);
    pInviteDst->m_dwTradingInviteSrc = ERROR_ID;
    KGLOG_PROCESS_ERROR(pInviteSrc);

    KG_PROCESS_ERROR(pInviteSrc->m_eGameStatus == gsPlaying);

    if (!pRespond->bAccept)
    {
        DoMessageNotify(pInviteSrc->m_nConnIndex, ectTradingErrorCode, trcRefuseInvite);
        if (pInviteSrc->m_dwTradingInviteDst == pInviteDst->m_dwID)
        {
            pInviteSrc->m_dwTradingInviteDst = ERROR_ID;
        }
        goto Exit1;
    }
    
    // 核对该交易是否还有效
    KG_PROCESS_ERROR_RET_CODE(pInviteSrc->m_dwTradingInviteDst == pInviteDst->m_dwID, trcInvalidInvite);
    pInviteSrc->m_dwTradingInviteDst = ERROR_ID;

    KG_PROCESS_ERROR(!pInviteSrc->m_pTradingBox);
    KG_PROCESS_ERROR(pInviteSrc->m_eMoveState != cmsOnDeath);

    KG_PROCESS_ERROR(!pInviteDst->m_pTradingBox);
    KG_PROCESS_ERROR(pInviteDst->m_eMoveState != cmsOnDeath);

    nRelation = pInviteSrc->GetPlayerRelation(pInviteDst);
    KG_PROCESS_ERROR_RET_CODE(!(nRelation & sortEnemy), trcInvalidTarget);

    // 交易位置检测
    KG_PROCESS_ERROR(pInviteSrc->m_pScene == pInviteDst->m_pScene);

    nRetCode = g_InRange(pInviteSrc, pInviteDst, MAX_TRADING_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, trcTooFar);

    pNewTradingBoxSrc = KMemory::New<KTradingBox>();
    KGLOG_PROCESS_ERROR(pNewTradingBoxSrc);
    
    nRetCode = pNewTradingBoxSrc->Init(pInviteSrc, pInviteDst);
    KGLOG_PROCESS_ERROR(nRetCode);
    bNewTradingBoxSrcInit = true;

    pInviteSrc->m_pTradingBox = pNewTradingBoxSrc;

    pNewTradingBoxDst = KMemory::New<KTradingBox>();
    KGLOG_PROCESS_ERROR(pNewTradingBoxDst);

    nRetCode = pNewTradingBoxDst->Init(pInviteDst, pInviteSrc);
    KGLOG_PROCESS_ERROR(nRetCode);
    bNewTradingBoxDstInit = true;
        
    pInviteDst->m_pTradingBox = pNewTradingBoxDst;

    nRetCode = DoOpenTradingBoxNotify(pInviteSrc->m_nConnIndex, pInviteDst->m_dwID);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = DoOpenTradingBoxNotify(pInviteDst->m_nConnIndex, pInviteSrc->m_dwID);
    KG_PROCESS_ERROR(nRetCode);

Exit1:    
    nResult = trcSuccess;
Exit0:

    if (pInviteDst)
    {
        pInviteDst->m_dwTradingInviteSrc = ERROR_ID;
    }
    
    if (nResult != trcSuccess) 
    {
        if (pNewTradingBoxSrc)
        {
            pInviteSrc->m_pTradingBox = NULL;
            if (bNewTradingBoxSrcInit)
            {
                pNewTradingBoxSrc->UnInit();
                bNewTradingBoxSrcInit = false;
            }
            KMemory::Delete(pNewTradingBoxSrc);
            pNewTradingBoxSrc = NULL;
        }

        if (pNewTradingBoxDst)
        {
            pInviteDst->m_pTradingBox = NULL;
            if (bNewTradingBoxDstInit)
            {
                pNewTradingBoxDst->UnInit();
                bNewTradingBoxDstInit = false;
            }
            KMemory::Delete(pNewTradingBoxDst);
            pNewTradingBoxDst = NULL;
        }
        
        if (pInviteDst)
        {
            DoMessageNotify(pInviteDst->m_nConnIndex, ectTradingErrorCode, nResult);
        }
    }

    return;
}

void KPlayerServer::OnTradingAddItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                     nResult     = trcFailed;
    int                     nRetCode    = false;
    KPlayer*                pPlayer     = NULL;
    KPlayer*                pOtherSide  = NULL;
    C2S_TRADING_ADD_ITEM*   pAddItem    = (C2S_TRADING_ADD_ITEM*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_pTradingBox);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, trcYouDead);
    
    pOtherSide = pPlayer->m_pTradingBox->GetOtherSide();
    assert(pOtherSide);
    assert(pOtherSide->m_pTradingBox);

    KG_PROCESS_ERROR_RET_CODE(pOtherSide->m_eGameStatus == gsPlaying, trcInvalidTarget);
    KG_PROCESS_ERROR_RET_CODE(pOtherSide->m_eMoveState != cmsOnDeath, trcTargetDead);

    assert(pOtherSide->m_pTradingBox->GetOtherSide() == pPlayer);

    nRetCode = pPlayer->m_pTradingBox->AddItem(pAddItem->byGridIndex, pAddItem->byBoxIndex, pAddItem->byPosIndex);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == trcSuccess, nRetCode);

    nResult = trcSuccess;
Exit0:
    if (nResult != trcSuccess)
    {
        DoMessageNotify(pPlayer->m_nConnIndex, ectTradingErrorCode, nResult);
    }

    return;
}

void KPlayerServer::OnTradingDeleteItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                 pPlayer     = NULL;
    KPlayer*                 pOther      = NULL;
    C2S_TRADING_DELETE_ITEM* pDeleteItem = (C2S_TRADING_DELETE_ITEM*)pData;
 
    pPlayer = GetPlayerByConnection(nConnIndex);
    KG_ASSERT_EXIT(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_pTradingBox);
    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);

    pOther = pPlayer->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KGLOG_PROCESS_ERROR(pOther->m_pTradingBox);
    KG_PROCESS_ERROR(pOther->m_eMoveState != cmsOnDeath);
    assert(pOther->m_pTradingBox->GetOtherSide() == pPlayer);

    pPlayer->m_pTradingBox->DelItem((DWORD)pDeleteItem->byGridIndex);

Exit0:
    return;
}

void KPlayerServer::OnTradingSetMoney(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                     nResult          = trcInvalid;
    int                     nRetCode         = false;
    KPlayer*                pPlayer          = NULL;
    KPlayer*                pOther           = NULL;
    C2S_TRADING_SET_MONEY*  pTradingSetMoney = (C2S_TRADING_SET_MONEY*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KG_ASSERT_EXIT(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_pTradingBox);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, trcYouDead);

    pOther = pPlayer->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KGLOG_PROCESS_ERROR(pOther->m_pTradingBox);
    KG_PROCESS_ERROR_RET_CODE(pOther->m_eGameStatus == gsPlaying, trcInvalidTarget);
    KG_PROCESS_ERROR_RET_CODE(pOther->m_eMoveState != cmsOnDeath, trcTargetDead);

    assert(pOther->m_pTradingBox->GetOtherSide() == pPlayer);

    nRetCode = pPlayer->m_pTradingBox->SetMoney(pTradingSetMoney->nMoney);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, trcNotEnoughMoney);

    nResult = trcSuccess;
Exit0:
    if (nResult != trcSuccess)
    {
        DoMessageNotify(pPlayer->m_nConnIndex, ectTradingErrorCode, nResult);
    }
    return;
}

void KPlayerServer::OnTradingConfirm(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                  nResult            = trcFailed;
    int                  nRetCode           = false;
    KPlayer*             pPlayer            = NULL;
    KPlayer*             pOther             = NULL;
    BOOL                 bConfirm           = false;
    int                  nKey               = 0;
    int                  nRelation          = 0;
    int                  nPlayerZCell       = 0;
    int                  nOtherZCell        = 0;
    int                  nDistance3         = 0;
    C2S_TRADING_CONFIRM* pTradingConfirm    = (C2S_TRADING_CONFIRM*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_pTradingBox, trcInvalid);

    pOther = pPlayer->m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KG_PROCESS_ERROR_RET_CODE(pOther->m_pTradingBox, trcInvalid);

    assert(pOther->m_pTradingBox->GetOtherSide() == pPlayer);
    // 取消交易
    if (!pTradingConfirm->bConfirm)
    {
        pPlayer->m_pTradingBox->Cancel();
        goto Exit1;
    }

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_eMoveState != cmsOnDeath, trcYouDead);
    
    KG_PROCESS_ERROR_RET_CODE(pOther->m_eGameStatus == gsPlaying, trcInvalidTarget);
    KG_PROCESS_ERROR_RET_CODE(pOther->m_eMoveState != cmsOnDeath, trcTargetDead);
    
    nRelation = pPlayer->GetPlayerRelation(pOther);
    KG_PROCESS_ERROR_RET_CODE(!(nRelation & sortEnemy), trcInvalidTarget);
    
    // 核对看到的物品是否正确
    nKey = pOther->m_pTradingBox->GetKey();
    KG_PROCESS_ERROR_RET_CODE(nKey == pTradingConfirm->nKey, trcSuccess);
    
    // 交易位置检测
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_pScene == pOther->m_pScene, trcTooFar);

    nRetCode = g_InRange(pPlayer, pOther, MAX_TRADING_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, trcTooFar);

    pPlayer->m_pTradingBox->Confirm();

Exit1:
    nResult = trcSuccess;
Exit0: 
    if (nResult != trcSuccess)
    {
        if (pPlayer)
        {
            DoTradingComplete(pPlayer->m_nConnIndex, false);
            DoMessageNotify(pPlayer->m_nConnIndex, ectTradingErrorCode, nResult);
            if (pPlayer->m_pTradingBox)
            {
                pPlayer->m_pTradingBox->ClearTradingInfo();
            }
        }

        if (pOther)
        {
            DoTradingComplete(pOther->m_nConnIndex, false);
            DoMessageNotify(pOther->m_nConnIndex, ectTradingErrorCode, nResult);
            if (pOther->m_pTradingBox)
            {
                pOther->m_pTradingBox->ClearTradingInfo();
            }
        }
    }
    return;
}

// <-------------------------- Trading ----------------------------

// 申请交换装备
void KPlayerServer::OnApplyExchangeItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    ITEM_RESULT_CODE            nResult             = ircInvalid;
	ITEM_RESULT_CODE            eRetCode            = ircInvalid;
	C2S_APPLY_EXCHANGE_ITEM*    pApplyExchangeItem  = (C2S_APPLY_EXCHANGE_ITEM*)pData;
	KPlayer*                    pPlayer             = GetPlayerByConnection(nConnIndex);
    TItemPos                    Src;
    TItemPos                    Dest;

	KGLOG_PROCESS_ERROR(pPlayer);

	Src.dwBox   = pApplyExchangeItem->byBox1;
	Src.dwX     = pApplyExchangeItem->byX1;
	Dest.dwBox  = pApplyExchangeItem->byBox2;
	Dest.dwX    = pApplyExchangeItem->byX2;
	
	if (Dest.dwBox != ibInvalid)
	{
		eRetCode = pPlayer->m_ItemList.ExchangeItem(Src, Dest, (DWORD)pApplyExchangeItem->wAmount);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
	}
	else
	{
		KItem* pItem = pPlayer->m_ItemList.GetItem(Src.dwBox, Src.dwX);
		KGLOG_PROCESS_ERROR(pItem);

		KG_PROCESS_ERROR_RET_CODE(pItem->m_Common.bCanDestroy, ircCannotDestroy);
        
        if (Src.dwBox == ibEquip)
        {
            eRetCode = pPlayer->m_ItemList.CanUnEquip(Src.dwX);
            KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
        }

		eRetCode = pPlayer->m_ItemList.DestroyItem(Src);
		KG_PROCESS_ERROR_RET_CODE(eRetCode == ircSuccess, eRetCode);
	}
    
    nResult = ircSuccess;
Exit0:
    if (nResult != ircSuccess && pPlayer)
    {
        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectItemErrorCode, nResult);
    }
	return;
}

void KPlayerServer::OnRequestTarget(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	SKILL_RESULT_CODE	nResult		    = srcInvalid;
    BOOL                bRetCode        = false;
    C2S_REQUEST_TARGET*	pRequestTarget	= (C2S_REQUEST_TARGET*)pData;
	KPlayer*			pPlayer		    = GetPlayerByConnection(nConnIndex);

    assert(pPlayer);

	switch (pRequestTarget->byTargetType)
	{
	case ttNpc:
		{
			KNpc* pTarget = g_pSO3World->m_NpcSet.GetObj(pRequestTarget->dwTargetID);

			KG_PROCESS_ERROR_RET_CODE(pTarget, srcInvalidTarget);
            
            bRetCode = pPlayer->m_SelectTarget.SetTarget(pTarget);
            KGLOG_PROCESS_ERROR(bRetCode);

            g_PlayerServer.DoSetTarget(pPlayer, pTarget);
		}
		break;

	case ttPlayer:
		{
			KPlayer* pTarget = g_pSO3World->m_PlayerSet.GetObj(pRequestTarget->dwTargetID);
			KG_PROCESS_ERROR_RET_CODE(pTarget, srcInvalidTarget);

            bRetCode = pPlayer->m_SelectTarget.SetTarget(pTarget);
            KGLOG_PROCESS_ERROR(bRetCode);
            
            g_PlayerServer.DoSetTarget(pPlayer, pTarget);
		}
		break;

	case ttDoodad:
		{
			KDoodad* pTarget = g_pSO3World->m_DoodadSet.GetObj(pRequestTarget->dwTargetID);
			KG_PROCESS_ERROR_RET_CODE(pTarget, srcInvalidTarget);

            bRetCode = pPlayer->m_SelectTarget.SetTarget(pTarget);
            KGLOG_PROCESS_ERROR(bRetCode);
		}
		break;

	case ttNoTarget:
		{
			bRetCode = pPlayer->m_SelectTarget.ClearTarget();
            KGLOG_PROCESS_ERROR(bRetCode);

            // 应策划要求，取消目标选择的同时取消自动攻击。
            pPlayer->DoAutoCastSkillIdle();
		}
		break;

	default:
        KGLogPrintf(KGLOG_ERR, "Unexcept target type: %d\n", pRequestTarget->byTargetType);
        goto Exit0;
	}

    pPlayer->m_nLastSyncTargetMaxLife           = -1;
    pPlayer->m_nLastSyncTargetMaxMana           = -1; 
    pPlayer->m_nLastSyncTargetLifePercent       = -1;
    pPlayer->m_nLastSyncTargetManaPercent       = -1; 
    pPlayer->m_dwLastSyncTargetTargetID         = ERROR_ID;
    pPlayer->m_nLastSyncTargetTargetMaxLife     = -1;
    pPlayer->m_nLastSyncTargetTargetMaxMana     = -1; 
    pPlayer->m_nLastSyncTargetTargetLifePercent = -1;
    pPlayer->m_nLastSyncTargetTargetManaPercent = -1; 
    pPlayer->m_dwTargetBuffCRC                  = 0;
    pPlayer->m_dwTargetTargetBuffCRC            = 0;
    pPlayer->m_dwTargetDropID                   = ERROR_ID;

    DoSyncTargetMaxLMR(pPlayer);

    DoSyncTargetCurrentLMR(pPlayer);

    DoSyncTargetWeakInfo(pPlayer);

    DoSyncTargetBuffList(pPlayer);

    DoSyncTargetTargetMaxLMR(pPlayer);

    DoSyncTargetTargetCurrentLMR(pPlayer);

    DoSyncTargetTargetBuffList(pPlayer);

    DoSyncTargetDropID(pPlayer);

    DoBroadcastTarget(pPlayer);

    nResult = srcSuccess;
Exit0:
    if (nResult != srcSuccess)
    {
        DoSetTarget(pPlayer, pPlayer);

	    DoMessageNotify(nConnIndex, (BYTE)ectSkillErrorCode, (BYTE)nResult);
    }
    return;
}

// 选择窗口选项
void KPlayerServer::OnWindowSelect(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bRetCode        = false;
    int                 nDistance3      = 0;
	KPlayer*            pPlayer         = NULL;
    C2S_WINDOW_SELECT*  pWindowSelect   = (C2S_WINDOW_SELECT*)pData;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	//检测流水号是否正确
	KG_PROCESS_ERROR(pWindowSelect->dwIndex == pPlayer->m_SelectList.dwIndex);

	//检测地图是否正确
	KG_PROCESS_ERROR(pPlayer->m_pScene);
	KG_PROCESS_ERROR(pPlayer->m_pScene->m_dwMapID == pPlayer->m_SelectList.dwMapID);
	KG_PROCESS_ERROR(pPlayer->m_pScene->m_nCopyIndex == pPlayer->m_SelectList.nMapCopyIndex);

	//检测距离是否设定范围内
	nDistance3 = g_GetDistance3(
		pPlayer->m_nX / CELL_LENGTH, 
        pPlayer->m_nY / CELL_LENGTH, 
		(int)(pPlayer->m_nZ / POINT_PER_ALTITUDE / (LOGICAL_CELL_CM_LENGTH / ALTITUDE_UNIT)),
		(int)(pPlayer->m_SelectList.nX / CELL_LENGTH), 
        (int)(pPlayer->m_SelectList.nY / CELL_LENGTH), 
		(int)(pPlayer->m_SelectList.nZ / POINT_PER_ALTITUDE / (LOGICAL_CELL_CM_LENGTH / ALTITUDE_UNIT))
    );
#define SELECT_AVAILABLE_RANGE	(REGION_GRID_WIDTH * REGION_GRID_WIDTH * REGION_GRID_WIDTH)
	KG_PROCESS_ERROR(nDistance3 < SELECT_AVAILABLE_RANGE);

	//检测选择序号是否在范围内
	KGLOG_PROCESS_ERROR(pWindowSelect->bySelect <= pPlayer->m_SelectList.nFuncCount);

	//回调函数
	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pPlayer->m_SelectList.dwScriptID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pPlayer->m_SelectList.dwScriptID, pPlayer->m_SelectList.szFuncName[pWindowSelect->bySelect]);
	if (bRetCode)
    {
	    int             nTopIndex = 0;
        KSceneObject*   pSceneObject = NULL;
        
        bRetCode = pPlayer->m_SelectList.Target.GetTarget(&pSceneObject);
        if (bRetCode && pSceneObject)
		{
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
			g_pSO3World->m_ScriptCenter.PushValueToStack((int)(pPlayer->m_SelectList.Target.GetTargetType()));
			g_pSO3World->m_ScriptCenter.PushValueToStack(pSceneObject->m_dwID);
			g_pSO3World->m_ScriptCenter.PushValueToStack(pWindowSelect->bySelect);

			g_pSO3World->m_ScriptCenter.CallFunction(pPlayer->m_SelectList.dwScriptID, pPlayer->m_SelectList.szFuncName[pWindowSelect->bySelect], 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
		else
		{
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
			g_pSO3World->m_ScriptCenter.PushValueToStack((int)ttNoTarget);
			g_pSO3World->m_ScriptCenter.PushValueToStack(0);
			g_pSO3World->m_ScriptCenter.PushValueToStack(pWindowSelect->bySelect);

			g_pSO3World->m_ScriptCenter.CallFunction(pPlayer->m_SelectList.dwScriptID, pPlayer->m_SelectList.szFuncName[pWindowSelect->bySelect], 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
    }

	return;

Exit0:

	return;
}

void KPlayerServer::OnCastProfessionSkill(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                        bRetCode		        = false;
	CRAFT_RESULT_CODE           eRetCode                = crcInvalid;
	DWORD                       dwRBookItemID	        = 0;
	KPlayer*                    pPlayer	                = NULL;
	C2S_CAST_PROFESSION_SKILL*  pCastProfessionSkill    = (C2S_CAST_PROFESSION_SKILL*)pData;
    KTarget                     Target;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
	KG_PROCESS_ERROR(pPlayer);

	KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

	dwRBookItemID = pCastProfessionSkill->dwRBookItemID;

	switch(pCastProfessionSkill->byTargetType)
	{
	case ttNoTarget:
		Target.ClearTarget();
		break;
    case ttCoordination:
        bRetCode = Target.SetTarget(pCastProfessionSkill->nParam1, pCastProfessionSkill->nParam2, 0);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
	case ttPlayer:
	case ttNpc:
    case ttDoodad:
        bRetCode = Target.SetTarget((TARGET_TYPE)pCastProfessionSkill->byTargetType, pCastProfessionSkill->nParam1);
		KGLOG_PROCESS_ERROR(bRetCode);
        break;
	case ttItem:
		bRetCode = Target.SetTarget((TARGET_TYPE)pCastProfessionSkill->byTargetType, pCastProfessionSkill->nParam1);
		KGLOG_PROCESS_ERROR(bRetCode);
		break;
	default:
		KGLOG_PROCESS_ERROR(!"Unexpected target type !");
		goto Exit0;
		break;
	}

	eRetCode = pPlayer->CastProfessionSkill(
        pCastProfessionSkill->dwCraftID, pCastProfessionSkill->dwRecipeID, dwRBookItemID, Target
    );
	KGLOG_PROCESS_ERROR(eRetCode == crcSuccess);

Exit0:
	return;
}

void KPlayerServer::OnLootItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    LOOT_ITEM_RESULT_CODE   nResult         = lircFailed;
    LOOT_ITEM_RESULT_CODE   eRetCode        = lircFailed;
    BOOL                    bRetCode        = false;
	C2S_LOOT_ITEM*          pLootItem       = (C2S_LOOT_ITEM*)pData;
	KPlayer*                pPlayer         = NULL;
    KDoodad*                pDoodad         = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pLootItem->dwDoodadID);
	KG_PROCESS_ERROR_RET_CODE(pDoodad, lircNoLootTarget);

	KG_PROCESS_SUCCESS(pDoodad->m_pLootList == NULL);

	bRetCode = pDoodad->m_pLootList->IsInLootRange(pPlayer, pDoodad);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, lircTooFarToLoot);

	eRetCode = pDoodad->m_pLootList->LootItem(pPlayer, pDoodad, pLootItem->dwItemID);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == lircSuccess, eRetCode);
	
Exit1:
	nResult = lircSuccess;
Exit0:
    if (nResult != lircSuccess)
    {
        DoMessageNotify(nConnIndex, ectLootItemErrorCode, (BYTE)nResult);
    }
	return;
}

void KPlayerServer::OnRollItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    LOOT_ITEM_RESULT_CODE   nResult         = lircFailed;
    LOOT_ITEM_RESULT_CODE   eRetCode        = lircFailed;
    BOOL                    bRetCode        = false;
	C2S_ROLL_ITEM*          pRollItem       = (C2S_ROLL_ITEM*)pData;
	DWORD                   dwDoodadID      = pRollItem->dwDoodadID;
	KPlayer*                pPlayer         = NULL;
    KDoodad*                pDoodad         = NULL;
    KLootList*              pLootList       = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
	KG_PROCESS_ERROR_RET_CODE(pDoodad, lircNoLootTarget);

	pLootList = pDoodad->m_pLootList;
	KGLOG_PROCESS_ERROR(pLootList);
    
    if (pRollItem->bCancel)
    {
        eRetCode = pLootList->CancelRollItem(pPlayer, pDoodad, pRollItem->dwItemID);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == lircSuccess, eRetCode);
    }
    else
    {
        eRetCode = pLootList->RollItem(pPlayer, pDoodad, pRollItem->dwItemID);
	    KG_PROCESS_ERROR_RET_CODE(eRetCode == lircSuccess, eRetCode);
    }
	
	nResult = lircSuccess;
Exit0:
    if (nResult != lircSuccess)
    {
        DoMessageNotify(nConnIndex, ectLootItemErrorCode, (BYTE)nResult);
    }
	return;
}

void KPlayerServer::OnApplyLootList(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                    bRetCode        = false;
	C2S_APPLY_LOOT_LIST*    pApplyLootList  = (C2S_APPLY_LOOT_LIST*)pData;
    KDoodad*                pDoodad         = NULL;
	KPlayer*                pPlayer         = NULL;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pApplyLootList->dwDoodadID);
    KG_PROCESS_ERROR(pDoodad);

	bRetCode = DoSyncLootList(pPlayer, pDoodad);
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerServer::OnCharacterOpen(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bRetCode        = false;
	KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);
	C2S_CHARACTER_OPEN* pCharacterOpen  = (C2S_CHARACTER_OPEN*)pData;
    KDoodad*            pDoodad         = NULL;

	assert(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pCharacterOpen->dwDoodadID);
    KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = pPlayer->OpenDoodad(pDoodad);
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerServer::OnDeathRespond(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bRetCode    = false;
    C2S_DEATH_RESPOND*      pRespond    = (C2S_DEATH_RESPOND*)pData;
    KPlayer*                pPlayer     = GetPlayerByConnection(nConnIndex);

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);
    KGLOG_PROCESS_ERROR(pPlayer->m_eMoveState == cmsOnDeath);
    
    if (pPlayer->m_bOnlyReviveInSitu && pPlayer->m_pScene->m_bReviveInSitu)
    {
        KGLOG_PROCESS_ERROR(pRespond->nRespondCode == drcReviveInSitu);
    }

    switch (pRespond->nRespondCode)
    {
    case drcReviveInSitu:
        {
            KGLOG_PROCESS_ERROR(pPlayer->m_pScene->m_bReviveInSitu);
            if (g_pSO3World->m_nGameLoop < pPlayer->m_ReviveParam.nSituReviveFrame)
            {
                KGLogPrintf(
                    KGLOG_ERR, "[SITE REVIVE] Player(%lu: %s, %s) should be revive at %lu, current game loop is %lu.\n", 
                    pPlayer->m_dwID, pPlayer->m_szAccount, pPlayer->m_szName, pPlayer->m_ReviveParam.nSituReviveFrame, g_pSO3World->m_nGameLoop
                );
                goto Exit0;
            }

            if (pPlayer->m_ReviveParam.nSituReviveCount + 1 < MAX_DEATH_PUNISH_TIME_SIZE)
            {
                ++(pPlayer->m_ReviveParam.nSituReviveCount);
            }

            pPlayer->m_ReviveParam.nLastSituReviveTime = g_pSO3World->m_nCurrentTime;
        }
        break;

    case drcRevivedByPlayer:
        KGLOG_PROCESS_ERROR(pPlayer->m_ReviveParam.eState == crsConfirmRevive);
        break;

    case drcRevivedInAltar:
        if (pPlayer->m_pScene->m_nReviveCycle > 0)
        {
            if (g_pSO3World->m_nGameLoop < pPlayer->m_ReviveParam.nSituReviveFrame)
            {
                KGLogPrintf(
                    KGLOG_ERR, "[ALTAR REVIVE] Player(%lu: %s, %s) should be revive at %lu, current game loop is %lu.\n", 
                    pPlayer->m_dwID, pPlayer->m_szAccount, pPlayer->m_szName, pPlayer->m_ReviveParam.nSituReviveFrame, g_pSO3World->m_nGameLoop
                );
                goto Exit0;
            }
        }
        break;

    default:
        goto Exit0;
    }

    pPlayer->Revive(pRespond->nRespondCode);

Exit0:
    return;
}

void KPlayerServer::OnApplyUseItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bRetCode        = false;
	KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);;
    TARGET_TYPE         eTargetType     = ttInvalid;
	C2S_APPLY_USE_ITEM* pApplyUseItem   = (C2S_APPLY_USE_ITEM*)pData;
	TItemPos            Pos;
	KTarget             Target;

    assert(pPlayer);

	Pos.dwBox   = pApplyUseItem->byBox;
	Pos.dwX     = pApplyUseItem->byX;
    
    eTargetType = (TARGET_TYPE)pApplyUseItem->byTargetType;

    switch (eTargetType)
    {
	case ttNoTarget:
		bRetCode = Target.ClearTarget();
		break;

    case ttCoordination:
        bRetCode = Target.SetTarget(pApplyUseItem->nParam1, pApplyUseItem->nParam2, pApplyUseItem->nParam3);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

    case ttNpc:
    case ttPlayer:
    case ttDoodad:
        bRetCode = Target.SetTarget(eTargetType, (DWORD)pApplyUseItem->nParam1);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

	case ttItemPos:
		{
			KPlayer* pTargetPlayer = NULL;

			pTargetPlayer = g_pSO3World->m_PlayerSet.GetObj(pApplyUseItem->nParam1);
			KG_PROCESS_ERROR(pTargetPlayer);

			bRetCode = Target.SetTarget(pTargetPlayer, pApplyUseItem->nParam2, pApplyUseItem->nParam3);
			KG_PROCESS_ERROR(bRetCode);
		}
		break;

    default:
        goto Exit0;
    }

	pPlayer->m_ItemList.UseItem(Pos, Target);

Exit0:
	return;
}

// 改变战斗开关
void KPlayerServer::OnApplyFightFlag(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	C2S_APPLY_FIGHT_FLAG*   pApplyFightFlag = (C2S_APPLY_FIGHT_FLAG*)pData;
	KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);
	DWORD                   dwForceID       = 0;
	BOOL                    bFightFlag      = false;

    assert(pPlayer);

	dwForceID = (DWORD)pApplyFightFlag->byForceID;
	bFightFlag = (BOOL)pApplyFightFlag->byOpen;

	pPlayer->m_ReputeList.SetFightFlag(dwForceID, bFightFlag);

	return;
}

// 结束拾取
void KPlayerServer::OnFinishLoot(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                bRetCode        = false;
    KPlayer*            pPlayer         = GetPlayerByConnection(nConnIndex);
	C2S_FINISH_LOOT*    pFinishLoot     = (C2S_FINISH_LOOT*)pData;
    KDoodad*            pDoodad         = NULL;

    assert(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pFinishLoot->dwDoodadID);
	KG_PROCESS_ERROR(pDoodad && pDoodad->m_pScene);

	pPlayer->DoActionIdle();
    pPlayer->m_SelectTarget.ClearTarget();

    KG_PROCESS_ERROR(pDoodad->m_pScene);

	pDoodad->ChangeState(dsIdle);
    
    g_PlayerServer.DoFinishLoot(pPlayer, pDoodad);

    if (pDoodad->m_pLootList)
	{
		pDoodad->Close();
	}

Exit0:
	return;
}

void KPlayerServer::OnLootMoney(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    LOOT_ITEM_RESULT_CODE   nResult     = lircFailed;
    LOOT_ITEM_RESULT_CODE   eRetCode    = lircFailed;
	BOOL                    bRetCode    = false;
	C2S_LOOT_MONEY*         pLootMoney  = (C2S_LOOT_MONEY*)pData;
	KDoodad*                pDoodad     = NULL;
    KPlayer*                pPlayer     = NULL;

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(pLootMoney->dwDoodadID);
	KG_PROCESS_ERROR_RET_CODE(pDoodad, lircNoLootTarget);

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	KG_PROCESS_SUCCESS(pDoodad->m_pLootList == NULL);

	bRetCode = pDoodad->m_pLootList->IsInLootRange(pPlayer, pDoodad);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, lircTooFarToLoot);

	eRetCode = pDoodad->m_pLootList->LootMoney(pPlayer, pDoodad);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == lircSuccess, eRetCode);
    
Exit1:
    nResult = lircSuccess;
Exit0:
    if (nResult != lircSuccess)
    {
        DoMessageNotify(nConnIndex, ectLootItemErrorCode, (BYTE)nResult);
    }
	return;
}

void KPlayerServer::OnDistributeItem(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    LOOT_ITEM_RESULT_CODE       nResult                 = lircFailed;
    LOOT_ITEM_RESULT_CODE       eRetCode                = lircFailed;
	BOOL                        bRetCode                = false;
	C2S_DISTRIBUTE_ITEM*        pDistributeItem         = (C2S_DISTRIBUTE_ITEM*)pData;
	KPlayer*                    pPlayer                 = GetPlayerByConnection(nConnIndex);
    KDoodad*                    pDoodad                 = NULL;
    KPlayer*                    pDstPlayer              = NULL;

    KGLOG_PROCESS_ERROR(pPlayer);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(pDistributeItem->dwDoodadID);
	KG_PROCESS_ERROR_RET_CODE(pDoodad, lircNoLootTarget);

	pDstPlayer = g_pSO3World->m_PlayerSet.GetObj(pDistributeItem->dwDstPlayerID);
	KG_PROCESS_ERROR_RET_CODE(pDstPlayer, lircNoLootTarget);

	bRetCode = pDoodad->m_pLootList->IsInLootRange(pPlayer, pDoodad);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, lircTooFarToLoot);

	eRetCode = pDoodad->m_pLootList->DistributeItem(
        pPlayer, pDstPlayer, pDoodad, pDistributeItem->dwItemID
    );
	KG_PROCESS_ERROR_RET_CODE(eRetCode == lircSuccess, eRetCode);
    
    nResult = lircSuccess;
Exit0:
    if (nResult != lircSuccess)
    {
        DoMessageNotify(nConnIndex, ectLootItemErrorCode, (BYTE)nResult);
    }
	return;
}

void KPlayerServer::OnTalkMessage(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bRetCode        = false;
    C2S_TALK_MESSAGE*       pTalkMsg        = (C2S_TALK_MESSAGE*)pData;
    S2C_TALK_MESSAGE*       pEchoMsg        = (S2C_TALK_MESSAGE*)m_byTempData;
    size_t                  uTalkDataLen    = 0;
    KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);
    KTeam*                  pTeam           = NULL;
    BYTE*                   pbyTalkData     = NULL;
    BYTE*                   pbyTalkDataEnd  = NULL;

    KGLOG_PROCESS_ERROR(nSize > sizeof(C2S_TALK_MESSAGE));
    KGLOG_PROCESS_ERROR(pPlayer);

    pTalkMsg->szReceiver[sizeof(pTalkMsg->szReceiver) - 1] = '\0';

    uTalkDataLen = nSize - sizeof(C2S_TALK_MESSAGE);

    if (pPlayer->m_nBanTime > g_pSO3World->m_nCurrentTime)
    {
        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectTalkErrorCode, TALK_ERROR_E_BAN);
        goto Exit0;
    }

    switch (pTalkMsg->byMsgType)
    {
    case trSence:
        {
            int nCoolDownID = g_pSO3World->m_Settings.m_ConstList.nPlayerSceneTalkCD;
            int nDuration = pPlayer->GetCDValue(nCoolDownID);

            if (!pPlayer->m_TimerList.CheckTimer(nCoolDownID))
            {
                g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectTalkErrorCode, TALK_ERROR_E_SCENE_CD);
                goto Exit0;
            }

            nDuration = max(1, nDuration);

            pPlayer->m_TimerList.ResetTimer(nCoolDownID, nDuration);
        }
    case trFace:
    case trNearby:
        DoTalkMessage(
            pTalkMsg->byMsgType, 
            pPlayer->m_dwID, pPlayer->m_szName, 
            ERROR_ID, NULL,
            uTalkDataLen, pTalkMsg->byTalkData
        );
        break;

    case trParty:
        pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
        if (!pTeam)
        {
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectTalkErrorCode, TALK_ERROR_E_NOT_IN_PARTY);
            break;
        }

        g_RelayClient.DoTalkMessage(
            pTalkMsg->byMsgType, 
            pPlayer->m_dwID, pPlayer->m_szName,
            pPlayer->m_dwTeamID, "",
            uTalkDataLen, pTalkMsg->byTalkData
        );
        break;

    case trTong:
        g_RelayClient.DoTalkMessage(
            pTalkMsg->byMsgType, 
            pPlayer->m_dwID, pPlayer->m_szName,
            ERROR_ID, "",
            uTalkDataLen, pTalkMsg->byTalkData
        );
        break;

    case trWhisper:
        {
            size_t uNameLen = 0;
            size_t uPakSize = 0;

            //TODO: 优化,密聊时如果目标在本服务器则无需经过GC中转
            g_RelayClient.DoTalkMessage(
                pTalkMsg->byMsgType, 
                pPlayer->m_dwID, pPlayer->m_szName,
                ERROR_ID, pTalkMsg->szReceiver,
                uTalkDataLen, pTalkMsg->byTalkData
            );

            // Send Echo message
            uNameLen = strlen(pTalkMsg->szReceiver);
            uPakSize = sizeof(S2C_TALK_MESSAGE) + 2 + uNameLen + uTalkDataLen;

            KGLOG_PROCESS_ERROR(uPakSize <= MAX_CHAT_TEXT_LEN && uNameLen <= UCHAR_MAX);

            pEchoMsg->byProtocolID = s2c_talk_message;
            pEchoMsg->dwTalkerID = pPlayer->m_dwID;
            pEchoMsg->byMsgType = pTalkMsg->byMsgType;

            pEchoMsg->byTalkData[1] = (BYTE)uNameLen;

            memcpy(pEchoMsg->byTalkData + 2, pTalkMsg->szReceiver, uNameLen);
            memcpy(pEchoMsg->byTalkData + 2 + uNameLen, pTalkMsg->byTalkData, uTalkDataLen);

            pEchoMsg->wSize = (WORD)uPakSize;
            pEchoMsg->byTalkData[0] = true; // 表示是否Echo消息

            Send(pPlayer->m_nConnIndex, pEchoMsg, uPakSize);
        }
        break;

    default:
        break;
    }

Exit0:
    return;
}

struct _GetFellowshipDataFunc 
{
    BOOL operator()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        KGFellowship*          pFellowship = NULL;
        KG_FELLOWSHIP_ALL_GS   Data;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, dwAlliedPlayerID);
        if (!pFellowship)
            return true;

        assert(sizeof(Data.byFlags) == sizeof(pFellowship->m_BitFlag));

        Data.dwAlliedPlayerID   = dwAlliedPlayerID;
        Data.nAttraction        = pFellowship->m_nAttraction;
        Data.dwMapID            = pFellowship->m_dwMapID;
        Data.dwForceID          = pFellowship->m_dwForceID;
        Data.nLevel             = pFellowship->m_nPlayerLevel;

        memcpy(Data.byFlags, &pFellowship->m_BitFlag, sizeof(Data.byFlags));

        strncpy(Data.szName, pFellowship->m_szName, sizeof(Data.szName));
        Data.szName[sizeof(Data.szName) - sizeof('\0')] = '\0';

        strncpy(Data.szRemark, pFellowship->m_szRemark, sizeof(Data.szRemark));
        Data.szRemark[sizeof(Data.szRemark) - sizeof('\0')] = '\0';

        g_PlayerServer.DoSyncFellowshipList(dwPlayerID, 1, &Data);

        return true;
    }
};

struct _GetFoeDataFunc 
{
    BOOL operator()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        KGFoe*          pFoe = NULL;
        KG_FOE_ALL_GS   Data;

        pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(dwPlayerID, dwAlliedPlayerID);
        if (!pFoe)
            return true;

        Data.dwAlliedPlayerID   = dwAlliedPlayerID;
        Data.nLevel             = pFoe->m_nPlayerLevel;
        Data.dwForceID          = pFoe->m_dwForceID;

        strncpy(Data.szName, pFoe->m_szName, sizeof(Data.szName));
        Data.szName[sizeof(Data.szName) - sizeof('\0')] = '\0';

        g_PlayerServer.DoSyncFoeList(dwPlayerID, 1, &Data);

        return true;
    }
};

struct _GetBlackListDataFunc 
{
    BOOL operator()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        KGBlackNode*            pBlackNode = NULL;
        KG_BLACK_LIST_ALL_GS    Data;

        pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(dwPlayerID, dwAlliedPlayerID);
        if (!pBlackNode)
            return true;

        Data.dwAlliedPlayerID   = dwAlliedPlayerID;

        strncpy(Data.szName, pBlackNode->m_szName, sizeof(Data.szName));
        Data.szName[sizeof(Data.szName) - sizeof('\0')] = '\0';

        g_PlayerServer.DoSyncBlackList(dwPlayerID, 1, &Data);

        return true;
    }
};

void KPlayerServer::OnGetFellowshipListRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                bRetCode                    = false;
    KPlayer*                            pPlayer                     = GetPlayerByConnection(nConnIndex);
    C2S_GET_FELLOWSHIP_LIST_REQUEST*    pGetFellowshipListRequest   = (C2S_GET_FELLOWSHIP_LIST_REQUEST*)pData;
    _GetFellowshipDataFunc              GetFellowshipDataFunc;
    _GetFoeDataFunc                     GetFoeDataFunc;
    _GetBlackListDataFunc               GetBlackListDataFunc;

    assert(pPlayer);

    switch (pGetFellowshipListRequest->byType)
    {
    case 1:
        DoSyncFellowshipGroupName(pPlayer->m_dwID);
        g_pSO3World->m_FellowshipMgr.TraverseFellowshipID(pPlayer->m_dwID, GetFellowshipDataFunc);
        DoSyncFellowshipList(pPlayer->m_dwID, 0, NULL);  // Indicate the end of sync. 
        break;

    case 2:
        g_pSO3World->m_FellowshipMgr.TraverseFoeID(pPlayer->m_dwID, GetFoeDataFunc);
        DoSyncFoeList(pPlayer->m_dwID, 0, NULL);
        break;

    case 3:
        g_pSO3World->m_FellowshipMgr.TraverseBlackListID(pPlayer->m_dwID, GetBlackListDataFunc);
        DoSyncBlackList(pPlayer->m_dwID, 0, NULL);
        break;

    default:
        break;
    }

Exit0:
    return;
}

void KPlayerServer::OnAddFellowshipRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode        = false;
    KPlayer*                    pPlayer         = GetPlayerByConnection(nConnIndex);
    C2S_ADD_FELLOWSHIP_REQUEST* pAddFellowship  = (C2S_ADD_FELLOWSHIP_REQUEST*)pData;

    assert(pPlayer);

    pAddFellowship->szAlliedPlayerName[sizeof(pAddFellowship->szAlliedPlayerName) - 1] = '\0';

    if (pAddFellowship->byType == 2)
    {
        bRetCode = pPlayer->CanAddFoe();
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    g_RelayClient.DoPlayerAddFellowshipRequest(
        pPlayer->m_dwID, pAddFellowship->szAlliedPlayerName, pAddFellowship->byType
    );

Exit0:
    return;
}

void KPlayerServer::OnDelFellowshipRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode            = false;
    DWORD                       dwAlliedPlayerID    = ERROR_ID;
    KPlayer*                    pPlayer             = GetPlayerByConnection(nConnIndex);
    C2S_DEL_FELLOWSHIP_REQUEST* pDelFellowship      = (C2S_DEL_FELLOWSHIP_REQUEST*)pData;

    assert(pPlayer);

    dwAlliedPlayerID = pDelFellowship->dwAlliedPlayerID;
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    switch (pDelFellowship->byType)
    {
    case 1:
        bRetCode = g_pSO3World->m_FellowshipMgr.DelFellowship(pPlayer->m_dwID, dwAlliedPlayerID);
        break;

    case 2:
        KG_PROCESS_ERROR(!pPlayer->m_bFightState);
        bRetCode = g_pSO3World->m_FellowshipMgr.DelFoe(pPlayer->m_dwID, dwAlliedPlayerID);
        break;

    case 3:
        bRetCode = g_pSO3World->m_FellowshipMgr.DelBlackList(pPlayer->m_dwID, dwAlliedPlayerID);
        break;

    default:
        break;
    }

Exit0:
    return;
}

void KPlayerServer::OnAddFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                    bRetCode    = false;
    KPlayer*                                pPlayer     = GetPlayerByConnection(nConnIndex);
    C2S_ADD_FELLOWSHIP_GROUP_REQUEST*       pAddGroup   = (C2S_ADD_FELLOWSHIP_GROUP_REQUEST*)pData;

    assert(pPlayer);

    pAddGroup->szGroupName[sizeof(pAddGroup->szGroupName) - 1] = '\0';

    g_pSO3World->m_FellowshipMgr.AddFellowshipGroup(pPlayer->m_dwID, pAddGroup->szGroupName);

Exit0:
    return;
}

void KPlayerServer::OnDelFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                bRetCode    = false;
    DWORD                               dwGroupID   = 0;
    KPlayer*                            pPlayer     = GetPlayerByConnection(nConnIndex);
    C2S_DEL_FELLOWSHIP_GROUP_REQUEST*   pDelGroup   = (C2S_DEL_FELLOWSHIP_GROUP_REQUEST*)pData;

    assert(pPlayer);

    dwGroupID = pDelGroup->dwGroupID;
    KGLOG_PROCESS_ERROR(dwGroupID < KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    bRetCode = g_pSO3World->m_FellowshipMgr.DelFellowshipGroup(pPlayer->m_dwID, dwGroupID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnRenameFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                    bRetCode    = false;
    DWORD                                   dwGroupID   = 0;
    KPlayer*                                pPlayer     = GetPlayerByConnection(nConnIndex);
    C2S_RENAME_FELLOWSHIP_GROUP_REQUEST*    pRename     = (C2S_RENAME_FELLOWSHIP_GROUP_REQUEST*)pData;

    assert(pPlayer);

    dwGroupID = pRename->dwGroupID;
    KGLOG_PROCESS_ERROR(dwGroupID < KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    pRename->szGroupName[sizeof(pRename->szGroupName) - 1] = '\0';

    g_pSO3World->m_FellowshipMgr.RenameFellowshipGroup(pPlayer->m_dwID, dwGroupID, pRename->szGroupName);

Exit0:
    return;
}

void KPlayerServer::OnSetFellowshipRemarkRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode            = false;
    DWORD                       dwAlliedPlayerID    = ERROR_ID;
    KPlayer*                    pPlayer             = GetPlayerByConnection(nConnIndex);
    C2S_SET_FELLOWSHIP_REMARK*  pSetRemark          = (C2S_SET_FELLOWSHIP_REMARK*)pData;
    KGFellowship*               pFellowship         = NULL;

    assert(pPlayer);

    pSetRemark->szRemark[sizeof(pSetRemark->szRemark) - 1] = '\0';

    dwAlliedPlayerID = pSetRemark->dwAlliedPlayerID;
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pPlayer->m_dwID, dwAlliedPlayerID);
    KG_PROCESS_ERROR(pFellowship);

    strncpy(pFellowship->m_szRemark, pSetRemark->szRemark, sizeof(pSetRemark->szRemark));
    pFellowship->m_szRemark[sizeof(pSetRemark->szRemark) - sizeof('\0')] = '\0';

Exit0:
    return;
}

void KPlayerServer::OnSetFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode            = false;
    DWORD                       dwAlliedPlayerID    = 0;
    DWORD                       dwGroupID           = 0;
    KPlayer*                    pPlayer             = GetPlayerByConnection(nConnIndex);
    C2S_SET_FELLOWSHIP_GROUP*   pSetGroup           = (C2S_SET_FELLOWSHIP_GROUP*)pData;
    KGFellowship*               pFellowship         = NULL;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator  it;

    assert(pPlayer);

    dwAlliedPlayerID = pSetGroup->dwAlliedPlayerID;
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    dwGroupID = pSetGroup->dwGroupID;
    KGLOG_PROCESS_ERROR(dwGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    it = g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.find(pPlayer->m_dwID);
    KG_PROCESS_ERROR(it != g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.end());

    KGLOG_PROCESS_ERROR(dwGroupID <= (DWORD)it->second.nGroupCount);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pPlayer->m_dwID, dwAlliedPlayerID);
    KG_PROCESS_ERROR(pFellowship);

    if (pFellowship->m_nAttraction < 0)
    {
        pFellowship->m_nAttraction = 0;
    }

    pFellowship->m_BitFlag.dwGroupID = dwGroupID;

Exit0:
    return;
}

void KPlayerServer::OnSendMailRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bRetCode        = false;
    KPlayer*                pPlayer         = GetPlayerByConnection(nConnIndex);
    KNpc*                   pNpc            = NULL;
    C2S_SEND_MAIL_REQUEST*  pSendMail       = (C2S_SEND_MAIL_REQUEST*)pData;
    KItem*                  pItem           = NULL;
    int                     nDelayTime      = 0;
    size_t                  uItemDataLen    = 0;
    int                     nPlayerMoney    = 0;

    assert(pPlayer);

    pSendMail->szDstName[sizeof(pSendMail->szDstName) - 1] = '\0';
    pSendMail->szTitle[sizeof(pSendMail->szTitle) - 1] = '\0';

    KGLOG_PROCESS_ERROR(nSize == sizeof(C2S_SEND_MAIL_REQUEST) + pSendMail->wTextSize);
    KGLOG_PROCESS_ERROR(pSendMail->wTextSize > 0); // 空字符串也有1个字节
    KGLOG_PROCESS_ERROR(pSendMail->wTextSize <= MAX_MAIL_TEXT_LEN);
    pSendMail->szText[pSendMail->wTextSize - 1] = '\0';

    if (STR_CASE_CMP(pPlayer->m_szName, pSendMail->szDstName) == 0)
    {
        DoSendMailRespond(nConnIndex, pSendMail->byRequestID, mrcDstNotSelf);
        goto Exit0;
    }
    
    if (pSendMail->szText[0] != '\0')
    {
        nDelayTime = max(nDelayTime, g_pSO3World->m_Settings.m_ConstList.nMailDelaySecondOnText);
    }

    // 检查金钱
    KGLOG_PROCESS_ERROR(pSendMail->nMoney >= 0);
    nPlayerMoney = pPlayer->m_ItemList.GetMoney();
    KGLOG_PROCESS_ERROR(nPlayerMoney - KMAIL_COST >= pSendMail->nMoney);
    
    if (pSendMail->nMoney > 0)
    {
        nDelayTime = max(nDelayTime, g_pSO3World->m_Settings.m_ConstList.nMailDelaySecondOnMoney);
    }

    // 检查道具
    if (pSendMail->byBox != ibInvalid)
    {
        int nBoxType = pPlayer->m_ItemList.GetBoxType(pSendMail->byBox);
        KGLOG_PROCESS_ERROR(nBoxType == ivtPackage);

        pItem = pPlayer->m_ItemList.GetItem(pSendMail->byBox, pSendMail->byX);
        KGLOG_PROCESS_ERROR(pItem);
        KGLOG_PROCESS_ERROR(!pItem->IsBind());
        
        nDelayTime = max(nDelayTime, g_pSO3World->m_Settings.m_ConstList.nMailDelaySecondOnItem);
    }
        
    if (pItem)
    {
        // 取道具
        bRetCode = pItem->GetBinaryData(&uItemDataLen, m_byTempData, MAX_EXTERNAL_PACKAGE_SIZE);
        KGLOG_PROCESS_ERROR(bRetCode);

        pItem = pPlayer->m_ItemList.PickUpItem(pSendMail->byBox, pSendMail->byX);
        KGLOG_PROCESS_ERROR(pItem);
        
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }

    // 扣钱
    bRetCode = pPlayer->m_ItemList.AddMoney(-(pSendMail->nMoney + KMAIL_COST));
    assert(bRetCode);
    
    g_RelayClient.DoSendMailRequest(
        pSendMail->byRequestID, pPlayer->m_szName, pSendMail->szDstName, pSendMail->szTitle, nDelayTime,
        false, pSendMail->nMoney, pSendMail->szText, m_byTempData, uItemDataLen
    );

Exit0:
    return;
}

void KPlayerServer::OnGetMaillistRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode    = false;
    KPlayer*                    pPlayer     = NULL;
    C2S_GET_MAILLIST_REQUEST*   pRequest    = (C2S_GET_MAILLIST_REQUEST*)pData;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eMoveState == cmsOnStand);

    // 检查邮箱位置是否合法: TODO
    // ... ...

    bRetCode = g_RelayClient.DoGetMailListRequest(pPlayer->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnGetMailContent(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bRetCode    = false;
    KPlayer*                pPlayer     = NULL;
    C2S_GET_MAIL_CONTENT*   pPak        = (C2S_GET_MAIL_CONTENT*)pData;

    KGLOG_PROCESS_ERROR(pPak->dwMailID);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eMoveState  == cmsOnStand);

    bRetCode = g_RelayClient.DoGetMailContent(pPlayer->m_dwID, pPak->dwMailID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnGetMailAttachment(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode    = false;
    KPlayer*                    pPlayer     = NULL;
    C2S_GET_MAIL_ATTACHMENT*    pPak        = (C2S_GET_MAIL_ATTACHMENT*)pData;

    KGLOG_PROCESS_ERROR(pPak->dwMailID);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);
    KG_PROCESS_ERROR(pPlayer->m_eMoveState  == cmsOnStand);

    bRetCode = g_RelayClient.DoGetMailAttachment(
        pPlayer->m_dwID, pPak->dwMailID, 
        pPak->bGetMoney, pPak->bGetText, pPak->bGetItem
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnSetMailRead(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                bRetCode    = false;
    KPlayer*            pPlayer     = NULL;
    C2S_SET_MAIL_READ*  pPak        = (C2S_SET_MAIL_READ*)pData;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
       
    bRetCode = g_RelayClient.DoSetMailRead(pPlayer->m_dwID, pPak->dwMailID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnDeleteMail(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                bRetCode    = false;
    KPlayer*            pPlayer     = NULL;
    C2S_DELETE_MAIL*    pPak        = (C2S_DELETE_MAIL*)pData;
    
    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
       
    bRetCode = g_RelayClient.DoDeleteMail(pPlayer->m_dwID, pPak->dwMailID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnReturnMail(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                bRetCode    = false;
    KPlayer*            pPlayer     = NULL;
    C2S_RETURN_MAIL*    pPak        = (C2S_RETURN_MAIL*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_RelayClient.DoReturnMail(pPlayer->m_dwID, pPak->dwMailID);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnApplyPKOperate(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	C2S_APPLY_PK_OPERATE*	pApplyPKOperate = (C2S_APPLY_PK_OPERATE*)pData;
	KPlayer*				pPlayer			= GetPlayerByConnection(nConnIndex);
	BOOL					bRetCode		= false;
	PK_RESULT_CODE			nPKRespond		= pkrcInvalid;

    assert(pPlayer);

	switch (pApplyPKOperate->byPKOperate)
	{
	case pkoApplyDuel:
		nPKRespond = pPlayer->m_PK.ApplyDuel(pApplyPKOperate->dwParam1);
		break;

    case pkoRefuseDuel:
		nPKRespond = pPlayer->m_PK.RefuseDuel();
		break;

	case pkoAcceptDuel:
		nPKRespond = pPlayer->m_PK.AcceptDuel();
	    break;

	case pkoLossDuel:
		nPKRespond = pPlayer->m_PK.LossDuel();
	    break;

	case pkoApplySlay:
		nPKRespond = pPlayer->m_PK.ApplySlay();
		break;

	case pkoCloseSlay:
		nPKRespond = pPlayer->m_PK.CloseSlay();
        break;

	default:
        break;
	}
    
	if (nPKRespond != pkrcSuccess)
	{	
		g_PlayerServer.DoMessageNotify(nConnIndex, ectPKErrorCode, nPKRespond);
	}

	return;
}

void KPlayerServer::OnSyncUserPreferences(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                        bRetCode            = false;
	KPlayer*                    pPlayer             = NULL;
	C2S_SYNC_USER_PREFERENCES*  pUserPreferences    = (C2S_SYNC_USER_PREFERENCES*)pData;

    KGLOG_PROCESS_ERROR(nSize == sizeof(C2S_SYNC_USER_PREFERENCES) + pUserPreferences->wDataLen);

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(
        pUserPreferences->nOffset, (int)pUserPreferences->wDataLen, pUserPreferences->byData
    );
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return ;
}

void KPlayerServer::OnSetBankOpenedFlag(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                        bRetCode        = false;
	KPlayer*                    pPlayer         = NULL;
	KNpc*                       pNpc            = NULL;
	C2S_SET_BANK_OPENED_FLAG*   pSetBankOpened  = (C2S_SET_BANK_OPENED_FLAG*)pData;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);
    
    if (pSetBankOpened->byBankOpend)
    {
        KGLOG_PROCESS_ERROR(pSetBankOpened->dwNpcID != ERROR_ID);

        pNpc = g_pSO3World->m_NpcSet.GetObj(pSetBankOpened->dwNpcID);
	    KG_PROCESS_ERROR(pNpc);
	    KG_PROCESS_ERROR(pNpc->m_pScene);

	    //必须是有银行功能的Npc
	    KG_PROCESS_ERROR(pNpc->m_bBank);

	    pPlayer->m_dwBankNpcID = pNpc->m_dwID;
    }

    pPlayer->m_ItemList.m_bBankOpened = pSetBankOpened->byBankOpend;
	
Exit0:
	return;
}

void KPlayerServer::OnEnableBankPackage(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL bRetCode = false;
	KPlayer* pPlayer = NULL;

	C2S_ENABLE_BANK_PACKAGE* pEnableBankPackage = (C2S_ENABLE_BANK_PACKAGE*)pData;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->m_ItemList.EnableBankPackage();
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return ;
}

void KPlayerServer::OnSyncMidMapMark(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                    bRetCode        = false;
	KPlayer*                pPlayer         = NULL;
	C2S_SYNC_MID_MAP_MARK*  pSyncMidMapMark = (C2S_SYNC_MID_MAP_MARK*)pData;
	
	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

    pSyncMidMapMark->szComment[sizeof(pSyncMidMapMark->szComment) - 1] = '\0';

	g_RelayClient.DoSyncMidMapMark(
        pPlayer, pSyncMidMapMark->nMapID, pSyncMidMapMark->nX, 
		pSyncMidMapMark->nY, pSyncMidMapMark->nType, pSyncMidMapMark->szComment
    );

Exit0:
	return;
}

void KPlayerServer::OnPlayerLearnSkill(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL                    bRetCode            = false;
	KPlayer*                pPlayer             = NULL;
	C2S_PLAYER_LEARN_SKILL* pPlayerLearnSkill   = (C2S_PLAYER_LEARN_SKILL*)pData;
    KNpc*                   pNpc                = NULL;
    DWORD                   dwMasterID          = 0;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

    pNpc = g_pSO3World->m_NpcSet.GetObj(pPlayerLearnSkill->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    bRetCode = pNpc->CanDialog(pPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    dwMasterID = pNpc->m_dwSkillMasterID;
    KG_PROCESS_ERROR(dwMasterID);

    bRetCode = pPlayer->m_SkillList.CanLearnSkill(pPlayerLearnSkill->dwSkillID, dwMasterID);
	KG_PROCESS_ERROR(bRetCode);

    bRetCode = pPlayer->m_SkillList.LearnSkill(pPlayerLearnSkill->dwSkillID, true, dwMasterID);
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerServer::OnPlayerLearnRecipe(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
	BOOL		bRetCode	= false;
	KPlayer*	pPlayer		= NULL;
	KNpc*		pNpc		= NULL;
	DWORD		dwMasterID	= 0;
	KRecipeMaster*		pRecipeMaster	= NULL;
	KRecipeLearningInfo* pLearnInfo		= NULL;
	C2S_PLAYER_LEARN_RECIPE* pPlayerLearnRecipe = (C2S_PLAYER_LEARN_RECIPE*)pData;

	pPlayer = GetPlayerByConnection(nConnIndex);
	KGLOG_PROCESS_ERROR(pPlayer);

	pNpc = g_pSO3World->m_NpcSet.GetObj(pPlayerLearnRecipe->dwNpcID);
	KGLOG_PROCESS_ERROR(pNpc);

	bRetCode = pNpc->CanDialog(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

	dwMasterID = pNpc->m_dwCraftMasterID;
	KG_PROCESS_ERROR(dwMasterID);

	bRetCode = pPlayer->CanLearnRecipe(pPlayerLearnRecipe->dwCraftID, pPlayerLearnRecipe->dwRecipeID, dwMasterID);
	KG_PROCESS_ERROR(bRetCode);

	pRecipeMaster = g_pSO3World->m_ProfessionManager.GetRecipeMaster(dwMasterID);
	KG_PROCESS_ERROR(pRecipeMaster);

	pLearnInfo = pRecipeMaster->GetLearnInfo(pPlayerLearnRecipe->dwCraftID, pPlayerLearnRecipe->dwRecipeID);
	KG_PROCESS_ERROR(pLearnInfo);

	//扣除学习的钱
	bRetCode = pPlayer->m_ItemList.AddMoney(-(pLearnInfo->nPrice));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pPlayer->LearnRecipe(pPlayerLearnRecipe->dwCraftID, pPlayerLearnRecipe->dwRecipeID);
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KPlayerServer::OnPlayerActiveSkillRecipe(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                            bRetCode            = false;
    KPlayer*                        pPlayer             = NULL;
    C2S_PLAYER_ACTIVE_SKILL_RECIPE* pActiveSkillRecipe  = (C2S_PLAYER_ACTIVE_SKILL_RECIPE*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(!pPlayer->m_bFightState);

    KGLOG_PROCESS_ERROR(pActiveSkillRecipe->dwRecipeKey);

    if (pActiveSkillRecipe->bActive)
    {
        bRetCode = pPlayer->m_SkillRecipeList.Active(pActiveSkillRecipe->dwRecipeKey);
    }
    else
    {
        bRetCode = pPlayer->m_SkillRecipeList.Deactive(pActiveSkillRecipe->dwRecipeKey);
    }
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnPeekOtherPlayerEquip(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                             nResult             = prcFailed;
    BOOL                            bRetCode            = false;
    int                             nBoxSize            = 0;
    KPlayer*	                    pDstPlayer		    = NULL;
    KPlayer*                        pSrcPlayer          = NULL;
    KItem*                          pItem               = NULL;
    C2S_PEEK_OTHER_PLAYER_EQUIP*    pPeekOtherPlayer    = (C2S_PEEK_OTHER_PLAYER_EQUIP*)pData;
    
    pSrcPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pSrcPlayer);

    pDstPlayer = g_pSO3World->m_PlayerSet.GetObj(pPeekOtherPlayer->dwPlayerID);
    KG_PROCESS_ERROR_RET_CODE(pDstPlayer, prcCanNotFindPlayer);

    KGLOG_PROCESS_ERROR(pDstPlayer->m_pScene == pSrcPlayer->m_pScene);

    bRetCode = pDstPlayer->CanDialog(pSrcPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    nBoxSize = pDstPlayer->m_ItemList.GetBoxSize(ibEquip);
    for (int i = 0; i < nBoxSize; i++)
    {
        pItem = pDstPlayer->m_ItemList.GetItem(ibEquip, i);
        if (pItem == NULL || pItem->m_Common.nSub == estPackage)
        {
            continue;
        }

        g_PlayerServer.DoSyncItemData(nConnIndex, pDstPlayer->m_dwID, pItem, ibEquip, i);
    }
            
    nResult = prcSuccess;
Exit0:
    g_PlayerServer.DoMessageNotify(
        nConnIndex, ectPeekOtherPlayerErrorCode, nResult, &(pPeekOtherPlayer->dwPlayerID), sizeof(pPeekOtherPlayer->dwPlayerID)
    );
    return;
}

void KPlayerServer::OnPeekOtherPlayerBook(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                         nResult             = prcFailed;
    BOOL                        bRetCode            = false;
    KPlayer*	                pDstPlayer		    = NULL;
    KPlayer*                    pSrcPlayer          = NULL;
    C2S_PEEK_OTHER_PLAYER_BOOK* pPeekOtherPlayer    = (C2S_PEEK_OTHER_PLAYER_BOOK*)pData;
    
    pSrcPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pSrcPlayer);

    pDstPlayer = g_pSO3World->m_PlayerSet.GetObj(pPeekOtherPlayer->dwPlayerID);
    KG_PROCESS_ERROR_RET_CODE(pDstPlayer, prcCanNotFindPlayer);

    KGLOG_PROCESS_ERROR(pDstPlayer->m_pScene == pSrcPlayer->m_pScene);

    bRetCode = pDstPlayer->CanDialog(pSrcPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncAllBookState(pSrcPlayer->m_nConnIndex, pDstPlayer);
    
    nResult = prcSuccess;
Exit0:
    if (nResult != prcSuccess)
    {
        g_PlayerServer.DoMessageNotify(
            nConnIndex, ectPeekOtherPlayerErrorCode, nResult, &(pPeekOtherPlayer->dwPlayerID), sizeof(pPeekOtherPlayer->dwPlayerID)
        );
    }
    return;
}

#define MAX_PEEK_DATA_SIZE 2048

void KPlayerServer::OnPeekOtherPlayerQuest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                             nResult             = prcFailed;
    BOOL                            bRetCode            = false;
    KPlayer*	                    pDstPlayer		    = NULL;
    KPlayer*                        pSrcPlayer          = NULL;
    C2S_PEEK_OTHER_PLAYER_QUEST*    pPeekOtherPlayer    = (C2S_PEEK_OTHER_PLAYER_QUEST*)pData;
    size_t                          uUsedSize           = 0;
    BYTE                            byPeekData[MAX_PEEK_DATA_SIZE];
    
    pSrcPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pSrcPlayer);

    pDstPlayer = g_pSO3World->m_PlayerSet.GetObj(pPeekOtherPlayer->dwPlayerID);
    KG_PROCESS_ERROR_RET_CODE(pDstPlayer, prcCanNotFindPlayer);

    KGLOG_PROCESS_ERROR(pDstPlayer->m_pScene == pSrcPlayer->m_pScene);

    bRetCode = pDstPlayer->CanDialog(pSrcPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pDstPlayer->m_QuestList.SaveQuestList(&uUsedSize, byPeekData, MAX_PEEK_DATA_SIZE);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncQuestData(pSrcPlayer->m_nConnIndex, pDstPlayer->m_dwID, byPeekData, uUsedSize, (BYTE)qdtQuestList);

    nResult = prcSuccess;
Exit0:
    if (nResult != prcSuccess)
    {
        g_PlayerServer.DoMessageNotify(
            nConnIndex, ectPeekOtherPlayerErrorCode, nResult, &(pPeekOtherPlayer->dwPlayerID), sizeof(pPeekOtherPlayer->dwPlayerID)
        );
    }
    return;
}

void KPlayerServer::OnRoadTrackStartOut(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    int                         nResult             = rtrcFailed;
    int                         nRetCode            = false;
    int                         nCost               = 0;
    BOOL                        bCostMoney          = false;
    KPlayer*                    pSrcPlayer          = NULL;
    C2S_ROAD_TRACK_START_OUT*   pRoadTrackStartOut  = (C2S_ROAD_TRACK_START_OUT*)pData;

    pSrcPlayer = GetPlayerByConnection(nConnIndex);
    KG_PROCESS_ERROR(pSrcPlayer);

    nRetCode = g_pSO3World->m_RoadManager.CalcCostMoney(&nCost, pRoadTrackStartOut->nFromNode, pRoadTrackStartOut->nTargetCityID, pSrcPlayer);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, rtrcNotOpen);

    KG_PROCESS_ERROR_RET_CODE(nCost == pRoadTrackStartOut->nCost, rtrcCostError);
    
    nRetCode = pSrcPlayer->m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nRetCode >= nCost, rtrcNotEnoughMoney);

    KGLOG_PROCESS_ERROR(nCost >= 0);

    nRetCode = pSrcPlayer->m_ItemList.AddMoney(-nCost);
    KGLOG_PROCESS_ERROR(nRetCode);

    bCostMoney = true;

    nRetCode = pSrcPlayer->AutoFly(pRoadTrackStartOut->nFromNode, pRoadTrackStartOut->nTargetCityID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = rtrcSuccess;
Exit0:
    g_PlayerServer.DoMessageNotify(
        nConnIndex,
        ectRoadTrackErrorCode, nResult, 
        &(pSrcPlayer->m_dwID), sizeof(pSrcPlayer->m_dwID)
    );

    if (bCostMoney && !nResult)
    {
        pSrcPlayer->m_ItemList.AddMoney(nCost);
    }
    return;
}

void KPlayerServer::OnRoadTrackForceRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_ROAD_TRACK_FORCE_REQUEST* pForceRequest = (C2S_ROAD_TRACK_FORCE_REQUEST*)pData;

    g_PlayerServer.DoRoadTrackForceRespond(nConnIndex);

    return;
}

void KPlayerServer::OnOpenVenation(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*            pPlayer         = NULL;
    BOOL                bRetCode        = false;
    C2S_OPEN_VENATION*  pOpenVenation   = (C2S_OPEN_VENATION*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_pSO3World->m_SkillManager.OpenVenation(pPlayer, pOpenVenation->nVenationID);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    g_PlayerServer.DoMessageNotify(nConnIndex, ectOpenVenationRetCode, bRetCode, &(pOpenVenation->nVenationID), sizeof(int));

    return;
}

void KPlayerServer::OnSendGmMessage(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bRetCode    = false;
    C2S_SEND_GM_MESSAGE*    pMessage    = (C2S_SEND_GM_MESSAGE*)pData;
    size_t                  uStrLen     = 0;

    KGLOG_PROCESS_ERROR(nSize > sizeof(C2S_SEND_GM_MESSAGE));

    uStrLen = nSize - sizeof(C2S_SEND_GM_MESSAGE) - 1;

    pMessage->szMessage[uStrLen] = '\0';

    bRetCode = g_RelayClient.DoSendGmMessage("_CLIENT_ASK_", pMessage->szMessage);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnJoinBattleFieldQueueRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                    bRetCode        = false;
    KPlayer*                                pPlayer         = NULL;
    KNpc*                                   pNpc            = NULL;
    DWORD                                   dwMapID         = 0;
    C2S_JOIN_BATTLE_FIELD_QUEUE_REQUEST*    pJoinRequest    = (C2S_JOIN_BATTLE_FIELD_QUEUE_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

//     pNpc = g_pSO3World->m_NpcSet.GetObj(pJoinRequest->dwNpcID);
//     KGLOG_PROCESS_ERROR(pNpc);

    dwMapID = pPlayer->GetMapID();

    bRetCode = g_RelayClient.DoJoinBattleFieldQueueRequest(
        pPlayer->m_dwID, pJoinRequest->dwMapID, pJoinRequest->nCopyIndex,
        pJoinRequest->dwGroupID, pJoinRequest->bTeamJoin, dwMapID
    );
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnLeaveBattleFieldQueueRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                                    bRetCode        = false;
    KPlayer*                                pPlayer         = NULL;
    C2S_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*   pLeaveRequest   = (C2S_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_RelayClient.DoLeaveBattleFieldQueueRequest(pPlayer->m_dwID, pLeaveRequest->dwMapID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnAcceptJoinBattleField(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KPlayer*                        pPlayer     = NULL;
    C2S_ACCEPT_JOIN_BATTLE_FIELD*   pAcceptJoin = (C2S_ACCEPT_JOIN_BATTLE_FIELD*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_RelayClient.DoAcceptJoinBattleField(pPlayer->m_dwID, pAcceptJoin->dwMapID, pAcceptJoin->nCopyIndex);
    KGLOG_PROCESS_ERROR(bRetCode);
Exit0:
    return;
}

void KPlayerServer::OnGetBattleFieldList(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode         = false;
    KPlayer*                    pPlayer          = NULL;
    C2S_GET_BATTLE_FIELD_LIST*  pBattleFieldList = (C2S_GET_BATTLE_FIELD_LIST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_RelayClient.DoGetBattleFieldList(pPlayer->m_dwID, pBattleFieldList->dwMapID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnLeaveBattleField(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                    bRetCode            = false;
    KPlayer*                pPlayer             = NULL;
    C2S_LEAVE_BATTLE_FIELD* pLeaveBattleField   = (C2S_LEAVE_BATTLE_FIELD*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_RelayClient.DoLeaveBattleFieldRequest(pPlayer->m_dwID);

Exit0:
    return;
}

void KPlayerServer::OnApplyTongRosterRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                        pPlayer         = NULL;
    C2S_APPLY_TONG_ROSTER_REQUEST*  pRosterRequest  = (C2S_APPLY_TONG_ROSTER_REQUEST*)pData;

    KGLOG_PROCESS_ERROR(nSize == sizeof(C2S_APPLY_TONG_ROSTER_REQUEST) + sizeof(DWORD) * pRosterRequest->nMemberCount);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_RelayClient.DoApplyTongRosterRequest(
        pPlayer->m_dwID, pRosterRequest->nLastUpdateFrame, 
        pRosterRequest->dwMemberList, pRosterRequest->nMemberCount
    );

Exit0:
    return;
}

void KPlayerServer::OnApplyTongInfoRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                        pPlayer         = NULL;
    C2S_APPLY_TONG_INFO_REQUEST*    pInfoRequest  = (C2S_APPLY_TONG_INFO_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_RelayClient.DoApplyTongInfoRequest(pPlayer->m_dwID, pInfoRequest->nLastUpdateFrame);

Exit0:
    return;
}

void KPlayerServer::OnInvitePlayerJoinTongRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                                pPlayer         = NULL;
    C2S_INVITE_PLAYER_JOIN_TONG_REQUEST*    pInfoRequest    = (C2S_INVITE_PLAYER_JOIN_TONG_REQUEST*)pData;

    pInfoRequest->szNewMemberName[sizeof(pInfoRequest->szNewMemberName) - 1] = '\0';

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    g_RelayClient.DoInvitePlayerJoinTongRequest(pPlayer->m_dwID, pInfoRequest->szNewMemberName);

Exit0:
    return;
}

void KPlayerServer::OnInvitePlayerJoinTongRespond(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                                pPlayer     = NULL;
    C2S_INVITE_PLAYER_JOIN_TONG_RESPOND*    pConfirm    = (C2S_INVITE_PLAYER_JOIN_TONG_RESPOND*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID == ERROR_ID);

    g_RelayClient.DoInvitePlayerJoinTongRespond(pPlayer->m_dwID, pConfirm->dwInviterID, pConfirm->dwTongID, pConfirm->byAccept);

Exit0:
    return;
}

void KPlayerServer::OnApplyKickOutTongMemberRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                                pPlayer     = NULL;
    C2S_APPLY_KICK_OUT_TONG_MEMBER_REQUEST* pRequest    = (C2S_APPLY_KICK_OUT_TONG_MEMBER_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    g_RelayClient.DoApplyKickOutTongMemberRequest(pPlayer->m_dwID, pRequest->dwMemberID);

Exit0:
    return;
}

void KPlayerServer::OnModifyTongInfoRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                        pPlayer     = NULL;
    C2S_MODIFY_TONG_INFO_REQUEST*   pRequest    = (C2S_MODIFY_TONG_INFO_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(nSize == sizeof(C2S_MODIFY_TONG_INFO_REQUEST) + pRequest->wDataLen);

    g_RelayClient.DoModifyTongInfoRequest(pPlayer->m_dwID, pRequest->byInfoType, pRequest->byData, (size_t)pRequest->wDataLen);

Exit0:
    return;
}

void KPlayerServer::OnModifyTongScheamRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                        pPlayer     = GetPlayerByConnection(nConnIndex);
    C2S_MODIFY_TONG_SCHEAM_REQUEST* pRequest    = (C2S_MODIFY_TONG_SCHEAM_REQUEST*)pData;

    assert(pPlayer);

    KGLOG_PROCESS_ERROR(nSize > sizeof(C2S_MODIFY_TONG_SCHEAM_REQUEST));

    g_RelayClient.DoModifyTongScheamRequest(
        pPlayer->m_dwID, pRequest->byGroupIndex, pRequest->byModifyType, 
        pRequest->byData, nSize - sizeof(C2S_MODIFY_TONG_SCHEAM_REQUEST)
    );

Exit0:
    return;
}

void KPlayerServer::OnApplyQuiteTongRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer* pPlayer = NULL;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    g_RelayClient.DoApplyQuiteTongRequest(pPlayer->m_dwID);

Exit0:
    return;
}

void KPlayerServer::OnChangeTongMemberGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                                pPlayer     = NULL;
    C2S_CHANGE_TONG_MEMBER_GROUP_REQUEST*   pRequest    = (C2S_CHANGE_TONG_MEMBER_GROUP_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_RelayClient.DoChangeTongMemberGroupRequest(pPlayer->m_dwID, pRequest->dwTargetMemberID, (int)pRequest->byTargetGroup);

Exit0:
    return;
}

void KPlayerServer::OnChangeTongMasterRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                        pPlayer     = NULL;
    C2S_CHANGE_TONG_MASTER_REQUEST* pRequest    = (C2S_CHANGE_TONG_MASTER_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_RelayClient.DoChangeTongMasterRequest(pPlayer->m_dwID, pRequest->dwNewMasterID);

Exit0:
    return;
}

void KPlayerServer::OnChangeTongMemberRemarkRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                                pPlayer     = NULL;
    C2S_CHANGE_TONG_MEMBER_REMARK_REQUEST*  pRequest    = (C2S_CHANGE_TONG_MEMBER_REMARK_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    pRequest->szRemark[sizeof(pRequest->szRemark) - 1] = '\0';

    g_RelayClient.DoChangeTongMemberRemarkRequest(pPlayer->m_dwID, pRequest->dwTargetMemberID, pRequest->szRemark);

Exit0:
    return;
}

void KPlayerServer::OnGetTongDescriptionRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer* pPlayer = NULL;
    C2S_GET_TONG_DESCRIPTION_REQUEST* pRequest = (C2S_GET_TONG_DESCRIPTION_REQUEST*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_RelayClient.DoGetTongDescriptionRequest(pPlayer->m_dwID, pRequest->dwTongID);
Exit0:
    return;
}

void KPlayerServer::OnApplyBattleFieldStatistics(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                            pPlayer     = NULL;
    C2S_APPLY_BATTLEFIELD_STATISTICS*   pRequest    = (C2S_APPLY_BATTLEFIELD_STATISTICS*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    for (KGPQList::PQ_VECTOR::iterator it = pPlayer->m_PQList.m_PQVector.begin(), itEnd = pPlayer->m_PQList.m_PQVector.end(); it != itEnd; ++it)
    {
        KGPQ* pPQ = g_pSO3World->m_PQManager.GetPQ(*it);

        if (!pPQ)
            continue;
        
        if (pPQ->m_eOwnerType != pqotBattleField)
            continue;

        for (KGPQ::KPQ_STATISTICS_MAP::iterator it = pPQ->m_StatisticsMap.begin(), itEnd = pPQ->m_StatisticsMap.end(); it != itEnd; ++it)
        {
            KPlayer*    pPlayer         = g_pSO3World->m_PlayerSet.GetObj(it->first);
            DWORD       dwMapID         = pPQ->m_dwOwnerID1;
            int         nMapCopyIndex   = pPQ->m_dwOwnerID2;

            if (!pPlayer)
                continue;

            if (!pPlayer->m_pScene)
                continue;

            if (pPlayer->m_pScene->m_dwMapID != dwMapID)
                continue;

            if (pPlayer->m_pScene->m_nCopyIndex != nMapCopyIndex)
                continue;

            DoSyncBattleFieldStatistics(nConnIndex, pPlayer, it->second);
        }

        DoSyncBattleFieldStatistics(nConnIndex, NULL, KPQ_STATISTICS_DATA());
    }

Exit0:
    return;
}

void KPlayerServer::OnSetHeroFlag(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*             pPlayer     = NULL;
    C2S_SET_HERO_FLAG*   pRequest    = (C2S_SET_HERO_FLAG*)pData;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_bHeroFlag != pRequest->byHeroFlag);

    pPlayer->m_bHeroFlag = pRequest->byHeroFlag;

    DoSyncHeroFlag(pPlayer);

Exit0:
    return;
}

void KPlayerServer::OnAuctionLookupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode    = false;
    KPlayer*                    pPlayer     = NULL;
    KNpc*                       pNpc        = NULL;
    C2S_AUCTION_LOOKUP_REQUEST* pRequest    = (C2S_AUCTION_LOOKUP_REQUEST*)pData;

    pNpc = g_pSO3World->m_NpcSet.GetObj(pRequest->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pNpc->m_bAuction);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    bRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_AUCTION_DISTANCE);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_RelayClient.DoAuctionLookupRequest(pRequest->byRequestID, pPlayer->m_dwID, &pRequest->Param);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnAuctionBidRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode    = false;
    KPlayer*                    pPlayer     = NULL;
    KNpc*                       pNpc        = NULL;
    C2S_AUCTION_BID_REQUEST*    pRequest    = (C2S_AUCTION_BID_REQUEST*)pData;

    pNpc = g_pSO3World->m_NpcSet.GetObj(pRequest->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pNpc->m_bAuction);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_AUCTION_DISTANCE);
    KG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(pPlayer->m_ItemList.m_nMoney >= pRequest->nPrice);

    // 先扣钱
    bRetCode = pPlayer->m_ItemList.AddMoney(-pRequest->nPrice, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_RelayClient.DoAuctionBidRequest(
        pPlayer->m_dwID, pRequest->dwSaleID, pRequest->dwCRC, pRequest->nPrice
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnAuctionSellRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode        = false;
    BYTE                        byCode          = arcUnknownError;
    KPlayer*                    pPlayer         = NULL;
    KNpc*                       pNpc            = NULL;
    C2S_AUCTION_SELL_REQUEST*   pRequest        = (C2S_AUCTION_SELL_REQUEST*)pData;
    KItem*                      pItem           = NULL;
    KItemInfo*                  pItemInfo       = NULL;
    int                         nLeftTime       = 0;
    int                         nCustodyCharges = 0;
    size_t                      uItemDataLen    = 0;
    int                         nAucGenre       = 0;
    int                         nAucSub         = 0;
    int                         nRequireLevel   = 0;
    int                         nQuality        = 0;
    char                        szSaleName[SALE_NAME_LEN];

    pNpc = g_pSO3World->m_NpcSet.GetObj(pRequest->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pNpc->m_bAuction);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_AUCTION_DISTANCE);
    KG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(pRequest->nStartPrice > 0);
    KGLOG_PROCESS_ERROR(pRequest->nStartPrice <= pRequest->nBuyItNowPrice);

    pItem = pPlayer->m_ItemList.GetItem((DWORD)pRequest->byBox, (DWORD)pRequest->byX);
    KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(!pItem->IsBind());
    KGLOG_PROCESS_ERROR(pItem->m_Common.nGenre != igBook);

    // 计算拍卖时间
    nLeftTime = (int)pRequest->byLeftHour * 3600;

    // 计算保管费(卖店价的40% 每12小时)，保管费至少10铜，小于10铜的算10铜
    nCustodyCharges = (int)pRequest->byLeftHour * pItem->m_Common.nPrice / 30;
    nCustodyCharges = max(nCustodyCharges, 10);

    KGLOG_PROCESS_ERROR(pPlayer->m_ItemList.m_nMoney >= nCustodyCharges);

    bRetCode = pItem->GetBinaryData(&uItemDataLen, m_byTempData, MAX_EXTERNAL_PACKAGE_SIZE);
    KGLOG_PROCESS_ERROR(bRetCode);

    pItemInfo = pItem->GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    strncpy(szSaleName, pItem->m_Common.szItemName, sizeof(szSaleName));
    szSaleName[sizeof(szSaleName) - 1] = '\0';

    nAucGenre       = pItemInfo->nAucGenre;
    nAucSub         = pItemInfo->nAucSub;
    nRequireLevel   = pItem->GetRequireLevel();
    nQuality        = pItem->m_GenParam.nQuality;

    // 扣钱
    bRetCode = pPlayer->m_ItemList.AddMoney(-nCustodyCharges, false);
    assert(bRetCode);

    pItem = pPlayer->m_ItemList.PickUpItem((DWORD)pRequest->byBox, (DWORD)pRequest->byX, true);
    assert(pItem);
    
    g_pSO3World->m_ItemManager.FreeItem(pItem);
    pItem = NULL;

    g_RelayClient.DoAuctionSellRequest(
        pPlayer->m_dwID, szSaleName, nAucGenre, nAucSub, nRequireLevel, nQuality, pRequest->nStartPrice,
        pRequest->nBuyItNowPrice, nCustodyCharges, nLeftTime, uItemDataLen, m_byTempData
    );

    byCode = arcSucceed;
Exit0:
    if (byCode == arcSucceed)
    {
        DoAuctionSellRespond(pPlayer->m_dwID, byCode);
    }
    return;
}

void KPlayerServer::OnAuctionCancelRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode = false;
    C2S_AUCTION_CANCEL_REQUEST* pRequest = (C2S_AUCTION_CANCEL_REQUEST*)pData;
    KPlayer*                    pPlayer  = NULL;
    KNpc*                       pNpc     = NULL;

    pNpc = g_pSO3World->m_NpcSet.GetObj(pRequest->dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);
    KGLOG_PROCESS_ERROR(pNpc->m_bAuction);

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_InRange(pPlayer, pNpc, MAX_OPEN_AUCTION_DISTANCE);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_RelayClient.DoAuctionCancelRequest(pPlayer->m_dwID, pRequest->dwSaleID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KPlayerServer::OnApplySetCampRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                    pPlayer     = NULL;
    C2S_APPLY_SET_CAMP_REQUEST* pRequest    = (C2S_APPLY_SET_CAMP_REQUEST*)pData;
    KCAMP                       eNewCamp    = (KCAMP)pRequest->byCamp;

    pPlayer = GetPlayerByConnection(nConnIndex);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(eNewCamp == cNeutral || eNewCamp == cGood || eNewCamp == cEvil);

    if (eNewCamp != cNeutral)
    {
        KGLOG_PROCESS_ERROR(pPlayer->m_eCamp == cNeutral);
    }
    
    g_RelayClient.DoApplySetCampRequest(pPlayer->m_dwID, eNewCamp);

Exit0:
    return;
}

void KPlayerServer::OnRemoteLuaCall(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                 bRetCode = false;
    KPlayer*             pPlayer  = GetPlayerByConnection(nConnIndex);
    C2S_REMOTE_LUA_CALL* pLuaCall = (C2S_REMOTE_LUA_CALL*)pData;

    KGLOG_PROCESS_ERROR(nSize == sizeof(C2S_REMOTE_LUA_CALL) + pLuaCall->uDataLen);

    pLuaCall->szFunction[sizeof(pLuaCall->szFunction) - 1] = '\0';

    bRetCode = g_pSO3World->m_ScriptServer.IsFunctionRegistered(pLuaCall->szFunction);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptServer.Accept(
        pPlayer, pLuaCall->szFunction, pLuaCall->byData, pLuaCall->uDataLen
    );

Exit0:
    return;
}

void KPlayerServer::OnPlayerReviveRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    BOOL                        bRetCode    = false;
    C2S_PLAYER_REVIVE_REQUEST*  pRequest    = (C2S_PLAYER_REVIVE_REQUEST*)pData;
    KPlayer*                    pPlayer     = GetPlayerByConnection(nConnIndex);

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);
    KGLOG_PROCESS_ERROR(pPlayer->m_eMoveState == cmsOnDeath);
    KGLOG_PROCESS_ERROR(g_pSO3World->m_Settings.m_ConstList.bUsePlayerReviveCtrl);

    switch (pRequest->nReviveCode)
    {
    case drcReviveInSitu:
        KGLOG_PROCESS_ERROR(pPlayer->m_ReviveCtrl.bReviveInSitu);

        pPlayer->Revive(pRequest->nReviveCode);

        pPlayer->m_ReviveCtrl.nSituReviveCount++;
        pPlayer->m_ReviveCtrl.nLastSituReviveTime = g_pSO3World->m_nCurrentTime;
        break;

    case drcRevivedInAltar:
        KGLOG_PROCESS_ERROR(pPlayer->m_ReviveCtrl.bReviveInAltar);

        pPlayer->Revive(pRequest->nReviveCode);

        break;

    case drcRevivedByPlayer:
        KGLOG_PROCESS_ERROR(pPlayer->m_ReviveCtrl.bReviveByPlayer);
        KGLOG_PROCESS_ERROR(pPlayer->m_ReviveParam.dwReviver != ERROR_ID);

        pPlayer->Revive(pRequest->nReviveCode);

        pPlayer->m_ReviveParam.dwReviver = ERROR_ID;

        break;

    default:
        goto Exit0;
    }

Exit0:
    return;
}

void KPlayerServer::OnApplyAchievementDataRequest(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    C2S_APPLY_ACHIEVEMENT_DATA_REQUEST* pRequest    = (C2S_APPLY_ACHIEVEMENT_DATA_REQUEST*)pData;
    KPlayer*                            pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRequest->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    DoSyncAchievementData(pPlayer);

Exit0:
    return;
}

void KPlayerServer::OnQueryProductIdentity(char* pData, size_t nSize, int nConnIndex, int nFrame)
{
    KPlayer*                    pPlayer     = GetPlayerByConnection(nConnIndex);
    C2S_QUERY_PRODUCT_IDENTITY* pQuery      = (C2S_QUERY_PRODUCT_IDENTITY*)pData;
    const char*                 pszText     = PRODUCT_IDENTITY"\nBuild at " __TIME__ " " __DATE__;
    size_t                      uDataLen    = 0;
    IKG_Buffer*                 piBuffer    = NULL;
    BYTE*                       pbyBuffer   = NULL;
    
    KG_PROCESS_ERROR(pQuery->nMagic == 53696136);

    uDataLen = 1 + strlen(pszText) + 1;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    *pbyBuffer = tdbText;
    strcpy((char*)pbyBuffer + 1, pszText);

    DoTalkMessage(trLocalSys, ERROR_ID, NULL, pPlayer->m_dwID, NULL, uDataLen, pbyBuffer);

Exit0:
    KG_COM_RELEASE(piBuffer);
}
//AutoCode:-处理协议函数结束-

#endif	//_SERVER
