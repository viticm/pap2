#include "stdafx.h"
#include "Relay_GS_Protocol.h"
#include "KRelayClient.h"

#if defined(_SERVER)
#include "SO3Result.h"
#include "Engine/KG_CreateGUID.h"
#include "KRoleDBDataDef.h"
#include "KPlayerServer.h"
#include "KScene.h"
#include "KTerrainData.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KTongDef.h"
#include "KMailDef.h"

KRelayClient g_RelayClient;

KRelayClient::KRelayClient(void)
{
    m_piSocketStream        = NULL;
    m_bSendErrorFlag        = false;
    m_bQuiting              = false;
    m_bSyncDataOver     = false;
    m_nNextQuitingSaveTime  = 0;
	m_nPingCycle            = 0;
	m_nLastSendPacketTime   = 0;

    m_dwSyncRoleID          = ERROR_ID;
    m_pbySyncRoleBuffer     = NULL;
    m_uSyncRoleOffset       = 0;

    m_pbySaveRoleBuffer     = NULL;

    memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
    memset(m_uProtocolSize, 0, sizeof(m_uProtocolSize));

    // ------------------ 协议注册 --------------------------------------------------------------------
    REGISTER_INTERNAL_FUNC(r2s_handshake_respond, &KRelayClient::OnHandshakeRespond, sizeof(R2S_HANDSHAKE_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_quit_notify, &KRelayClient::OnQuitNotify, sizeof(R2S_QUIT_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_confirm_player_login_respond, &KRelayClient::OnConfirmPlayerLoginRespond, sizeof(R2S_CONFIRM_PLAYER_LOGIN_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_create_map_notify, &KRelayClient::OnCreateMapNotify, sizeof(R2S_CREATE_MAP_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_finish_create_map_notify, &KRelayClient::OnFinishCreateMapNotify, sizeof(R2S_FINISH_CREATE_MAP_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_delete_map_notify, &KRelayClient::OnDeleteMapNotify, sizeof(R2S_DELETE_MAP_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_player_login_request, &KRelayClient::OnPlayerLoginRequest, sizeof(R2S_PLAYER_LOGIN_REQUEST));
    REGISTER_INTERNAL_FUNC(r2s_search_map_respond, &KRelayClient::OnSearchMapRespond, sizeof(R2S_SEARCH_MAP_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_transfer_player_request, &KRelayClient::OnTransferPlayerRequest, sizeof(R2S_TRANSFER_PLAYER_REQUEST));
    REGISTER_INTERNAL_FUNC(r2s_transfer_player_respond, &KRelayClient::OnTransferPlayerRespond, sizeof(R2S_TRANSFER_PLAYER_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_kick_account_notify, &KRelayClient::OnKickAccountNotify, sizeof(R2S_KICK_ACCOUNT_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_invite_player_join_team_request, &KRelayClient::OnInvitePlayerJoinTeamRequest, sizeof(R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST));
    REGISTER_INTERNAL_FUNC(r2s_apply_join_team_request, &KRelayClient::OnApplyJoinTeamRequest, sizeof(R2S_APPLY_JOIN_TEAM_REQUEST));
    REGISTER_INTERNAL_FUNC(r2s_team_create_notify, &KRelayClient::OnTeamCreateNotify, sizeof(R2S_TEAM_CREATE_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_add_member_notify, &KRelayClient::OnTeamAddMemberNotify, sizeof(R2S_TEAM_ADD_MEMBER_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_del_member_notify, &KRelayClient::OnTeamDelMemberNotify, sizeof(R2S_TEAM_DEL_MEMBER_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_disband_notify, &KRelayClient::OnTeamDisbandNotify, sizeof(R2S_TEAM_DISBAND_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_change_authority_notify, &KRelayClient::OnTeamChangeAuthorityNotify, sizeof(R2S_TEAM_CHANGE_AUTHORITY_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_set_loot_mode_notify, &KRelayClient::OnTeamSetLootModeNotify, sizeof(R2S_TEAM_SET_LOOT_MODE_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_party_message_notify, &KRelayClient::OnTeamMessageNotify, sizeof(R2S_PARTY_MESSAGE_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_set_formation_leader_notify, &KRelayClient::OnSetFormationLeaderNotify, sizeof(R2S_TEAM_SET_FORMATION_LEADER_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_online_flag, &KRelayClient::OnSyncTeamMemberOnlineFlag, sizeof(R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_max_lmr, &KRelayClient::OnTeamSyncMemberMaxLMR, sizeof(R2S_SYNC_TEAM_MEMBER_MAX_LMR));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_current_lmr, &KRelayClient::OnTeamSyncMemberCurrentLMR, sizeof(R2S_SYNC_TEAM_MEMBER_CURRENT_LMR));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_misc, &KRelayClient::OnTeamSyncMemberMisc, sizeof(R2S_SYNC_TEAM_MEMBER_MISC));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_position, &KRelayClient::OnTeamSyncMemberPosition, sizeof(R2S_SYNC_TEAM_MEMBER_POSITION));
    REGISTER_INTERNAL_FUNC(r2s_team_set_mark_respond, &KRelayClient::OnTeamSetMarkRespond, sizeof(R2S_TEAM_SET_MARK_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_team_camp_change, &KRelayClient::OnTeamCampChange, sizeof(R2S_TEAM_CAMP_CHANGE));
    REGISTER_INTERNAL_FUNC(r2s_team_level_up_raid_notify, &KRelayClient::OnTeamLevelUpRaidNotify, sizeof(R2S_TEAM_LEVEL_UP_RAID_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_change_member_group_notify, &KRelayClient::OnTeamChangeMemberGroupNotify, sizeof(R2S_TEAM_CHANGE_MEMBER_GROUP_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_player_talk_error, &KRelayClient::OnPlayerTalkError, sizeof(R2S_PLAYER_TALK_ERROR));
    REGISTER_INTERNAL_FUNC(r2s_player_talk_whisper_success_respond, &KRelayClient::OnPlayerTalkWhisperSuccessRespond, sizeof(R2S_PLAYER_TALK_WHISPER_SUCCESS_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_talk_message, &KRelayClient::OnTalkMessage, sizeof(R2S_TALK_MESSAGE));

    REGISTER_INTERNAL_FUNC(r2s_apply_fellowship_data_respond, &KRelayClient::OnApplyFellowshipDataRespond, sizeof(R2S_APPLY_FELLOWSHIP_DATA_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_add_fellowship_respond, &KRelayClient::OnAddFellowshipRespond, sizeof(R2S_ADD_FELLOWSHIP_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_get_fellowship_name_respond, &KRelayClient::OnGetFellowshipNameRespond, sizeof(R2S_GET_FELLOWSHIP_NAME_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_fellowship_mapid, &KRelayClient::OnSyncFellowshipMapID, sizeof(R2S_SYNC_FELLOWSHIP_MAPID));
    REGISTER_INTERNAL_FUNC(r2s_sync_fellowship_player_level, &KRelayClient::OnSyncFellowshipPlayerLevel, sizeof(R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL));
    REGISTER_INTERNAL_FUNC(r2s_sync_fellowship_player_forceid, &KRelayClient::OnSyncFellowshipPlayerForceID, sizeof(R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID));
    REGISTER_INTERNAL_FUNC(r2s_add_fellowship_notify, &KRelayClient::OnAddFellowshipNotify, sizeof(R2S_ADD_FELLOWSHIP_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_send_mail_respond, &KRelayClient::OnSendMailRespond, sizeof(R2S_SEND_MAIL_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_get_maillist_respond, &KRelayClient::OnGetMailListRespond, sizeof(R2S_GET_MAILLIST_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_mail_content, &KRelayClient::OnSyncMailContent, sizeof(R2S_SYNC_MAIL_CONTENT));
    REGISTER_INTERNAL_FUNC(r2s_give_mail_money_to_player, &KRelayClient::OnGiveMailMoneyToPlayer, sizeof(R2S_GIVE_MAIL_MONEY_TO_PLAYER));
    REGISTER_INTERNAL_FUNC(r2s_give_mail_item_to_player, &KRelayClient::OnGiveMailItemToPlayer, sizeof(R2S_GIVE_MAIL_ITEM_TO_PLAYER));
    REGISTER_INTERNAL_FUNC(r2s_new_mail_notify, &KRelayClient::OnNewMailNotify, sizeof(R2S_NEW_MAIL_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_mail_general_respond, &KRelayClient::OnMailGeneralRespond, sizeof(R2S_MAIL_GENERAL_RESPOND));
    
    REGISTER_INTERNAL_FUNC(r2s_sync_mid_map_mark, &KRelayClient::OnSyncMidMapMark, sizeof(R2S_SYNC_MID_MAP_MARK));
    REGISTER_INTERNAL_FUNC(r2s_map_copy_do_clear_player_prepare, &KRelayClient::OnMapCopyDoClearPlayerPrepare, sizeof(R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE));
    REGISTER_INTERNAL_FUNC(r2s_map_copy_do_clear_player, &KRelayClient::OnMapCopyDoClearPlayer, sizeof(R2S_MAP_COPY_DO_CLEAR_PLAYER));
    REGISTER_INTERNAL_FUNC(r2s_set_map_copy_owner, &KRelayClient::OnSetMapCopyOwner, sizeof(R2S_SET_MAP_COPY_OWNER));
    REGISTER_INTERNAL_FUNC(r2s_sync_road_track_info, &KRelayClient::OnSyncRoadTrackInfo, sizeof(R2S_SYNC_ROAD_TRACK_INFO));
    REGISTER_INTERNAL_FUNC(r2s_save_role_data_respond, &KRelayClient::OnSaveRoleDataRespond, sizeof(R2S_SAVE_ROLE_DATA_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_role_data, &KRelayClient::OnSyncRoleData, sizeof(R2S_SYNC_ROLE_DATA));
    REGISTER_INTERNAL_FUNC(r2s_load_role_data, &KRelayClient::OnLoadRoleData, sizeof(R2S_LOAD_ROLE_DATA));
    REGISTER_INTERNAL_FUNC(r2s_gm_command, &KRelayClient::OnGmCommand, sizeof(R2S_GM_COMMAND));

    REGISTER_INTERNAL_FUNC(r2s_join_battle_field_queue_respond, &KRelayClient::OnJoinBattleFieldQueueRespond, sizeof(R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_leave_battle_field_queue_respond, &KRelayClient::OnLeaveBattleFieldQueueRespond, sizeof(R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_battle_field_notify, &KRelayClient::OnBattleFieldNotify, sizeof(R2S_BATTLE_FIELD_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_sync_battle_field_list, &KRelayClient::OnSyncBattleFieldList, sizeof(R2S_SYNC_BATTLE_FIELD_LIST));
    REGISTER_INTERNAL_FUNC(r2s_accept_join_battle_field_respond, &KRelayClient::OnAcceptJoinBattleFieldRespond, sizeof(R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_leave_battle_field_respond, &KRelayClient::OnLeaveBattleFieldRespond, sizeof(R2S_LEAVE_BATTLE_FIELD_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_sync_camp_info, &KRelayClient::OnSyncCampInfo, sizeof(R2S_SYNC_CAMP_INFO));
    REGISTER_INTERNAL_FUNC(r2s_apply_set_camp_respond, &KRelayClient::OnApplySetCampRespond, sizeof(R2S_APPLY_SET_CAMP_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_sync_pq, &KRelayClient::OnBroadcastSyncPQ, sizeof(R2S_SYNC_PQ));
    REGISTER_INTERNAL_FUNC(r2s_delete_pq, &KRelayClient::OnBroadcastDeletePQ, sizeof(R2S_DELETE_PQ));
    REGISTER_INTERNAL_FUNC(r2s_pq_value_change, &KRelayClient::OnBroadcastPQValueChange, sizeof(R2S_PQ_VALUE_CHANGE));
    REGISTER_INTERNAL_FUNC(r2s_pq_finish, &KRelayClient::OnBroadcastPQFinish, sizeof(R2S_PQ_FINISH));

    REGISTER_INTERNAL_FUNC(r2s_apply_tong_info_respond, &KRelayClient::OnApplyTongInfoRespond, sizeof(R2S_APPLY_TONG_INFO_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_tong_member_info, &KRelayClient::OnSyncTongMemberInfo, sizeof(R2S_SYNC_TONG_MEMBER_INFO));
    REGISTER_INTERNAL_FUNC(r2s_delete_tong_member_notify, &KRelayClient::OnDeleteTongMemberNotify, sizeof(R2S_DELETE_TONG_MEMBER_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_sync_tong_repertory_page_respond, &KRelayClient::OnSyncTongRepertoryPageRespond, sizeof(R2S_SYNC_TONG_REPERTORY_PAGE_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_update_tong_client_data_version, &KRelayClient::OnUpdateTongClientDataVersion, sizeof(R2S_UPDATE_TONG_CLIENT_DATA_VERSION));
    REGISTER_INTERNAL_FUNC(r2s_invite_player_join_tong_request, &KRelayClient::OnInvitePlayerJoinTongRequest, sizeof(R2S_INVITE_PLAYER_JOIN_TONG_REQUEST));
    REGISTER_INTERNAL_FUNC(r2s_change_player_tong_notify, &KRelayClient::OnChangePlayerTongNotify, sizeof(R2S_CHANGE_PLAYER_TONG_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_tong_broadcast_message, &KRelayClient::OnBroadcastTongMessage, sizeof(R2S_TONG_BROADCAST_MESSAGE));
    REGISTER_INTERNAL_FUNC(r2s_get_tong_description_respond, &KRelayClient::OnGetTongDescriptionRespond, sizeof(R2S_GET_TONG_DESCRIPTION_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_tong_online_message, &KRelayClient::OnSyncTongOnlineMessage, sizeof(R2S_SYNC_TONG_ONLINE_MESSAGE));

    REGISTER_INTERNAL_FUNC(r2s_take_tong_repertory_item_to_pos_respond,  &KRelayClient::OnTakeTongRepertoryItemToPosRespond, sizeof(R2S_TAKE_TONG_REPERTORY_ITEM_TO_POS_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_take_tong_repertory_item_respond,  &KRelayClient::OnTakeTongRepertoryItemRespond, sizeof(R2S_TAKE_TONG_REPERTORY_ITEM_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_put_tong_repertory_item_respond, &KRelayClient::OnPutTongRepertoryItemRespond, sizeof(R2S_PUT_TONG_REPERTORY_ITEM_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_apply_stack_tong_repertory_item_respond, &KRelayClient::OnApplyStackTongRepertoryItemRespond, sizeof(R2S_APPLY_STACK_TONG_REPERTORY_ITEM_RESPOND))
    REGISTER_INTERNAL_FUNC(r2s_apply_open_tong_repertpry_respond, &KRelayClient::OnApplyOpenTongRepertoryRespond, sizeof(R2S_APPLY_OPEN_TONG_REPERTPRY_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_get_tong_salary_respond, &KRelayClient::OnGetTongSalaryRespond, sizeof(R2S_GET_TONG_SALARY_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_tong_history_respond, &KRelayClient::OnSyncTongHistoryRespond, sizeof(R2S_SYNC_TONG_HISTORY_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_auction_lookup_respond, &KRelayClient::OnAuctionLookupRespond, sizeof(R2S_AUCTION_LOOKUP_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_bid_respond, &KRelayClient::OnAuctionBidRespond, sizeof(R2S_AUCTION_BID_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_sell_respond, &KRelayClient::OnAuctionSellRespond, sizeof(R2S_AUCTION_SELL_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_cancel_respond, &KRelayClient::OnAuctionCancelRespond, sizeof(R2S_AUCTION_CANCEL_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_message_notify, &KRelayClient::OnAuctionMessageNotify, sizeof(R2S_AUCTION_MESSAGE_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_remote_lua_call, &KRelayClient::OnRemoteLuaCall, sizeof(R2S_REMOTE_LUA_CALL));

    REGISTER_INTERNAL_FUNC(r2s_achievement_global_announce_respond, &KRelayClient::OnAchievementGlobalAnnounceRespond, sizeof(R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_designation_global_announce_respond, &KRelayClient::OnDesignationGlobalAnnounceRespond, sizeof(R2S_DESIGNATION_GLOBAL_ANNOUNCE_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_global_system_value, &KRelayClient::OnSyncGlobalSystemValue, sizeof(R2S_SYNC_GLOBAL_SYSTEM_VALUE));
    REGISTER_INTERNAL_FUNC(r2s_query_stat_id_respond, &KRelayClient::OnQueryStatIDRespond, sizeof(R2S_QUERY_STAT_ID_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_change_ext_point_respond, &KRelayClient::OnChangeExtPointRespond, sizeof(R2S_CHANGE_EXT_POINT_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_set_charge_flag_respond, &KRelayClient::OnSetChargeFlagRespond, sizeof(R2S_SET_CHARGE_FLAG_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_zone_charge_flag, &KRelayClient::OnSyncZoneChargeFlag, sizeof(R2S_SYNC_ZONE_CHARGE_FLAG));
    REGISTER_INTERNAL_FUNC(r2s_active_present_code_respond, &KRelayClient::OnActivePresentCodeRespond, sizeof(R2S_ACTIVE_PRESENT_CODE_RESPOND));
    
    REGISTER_INTERNAL_FUNC(r2s_apex_protocol, &KRelayClient::OnApexProtocol, sizeof(R2S_APEX_PROTOCOL));

    REGISTER_INTERNAL_FUNC(r2s_set_farmer_limit_flag_request, &KRelayClient::OnSetFarmerLimitFlagRequest, sizeof(R2S_SET_FARMER_LIMIT_FLAG_REQUEST));

    REGISTER_INTERNAL_FUNC(r2s_game_card_sell_respond, &KRelayClient::OnGameCardSellRespond, sizeof(R2S_GAME_CARD_SELL_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_game_card_buy_respond, &KRelayClient::OnGameCardBuyRespond, sizeof(R2S_GAME_CARD_BUY_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_game_card_lookup_respond, &KRelayClient::OnGameCardLookupRespond, sizeof(R2S_GAME_CARD_LOOKUP_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_game_card_cancel_respond, &KRelayClient::OnGameCardCancelRespond, sizeof(R2S_GAME_CARD_CANCEL_RESPOND));
    
    REGISTER_INTERNAL_FUNC(r2s_sync_mentor_data, &KRelayClient::OnSyncMentorData, sizeof(R2S_SYNC_MENTOR_DATA));
    REGISTER_INTERNAL_FUNC(r2s_delete_mentor_record, &KRelayClient::OnDeleteMentorRecord, sizeof(R2S_DELETE_MENTOR_RECORD));
    REGISTER_INTERNAL_FUNC(r2s_update_mentor_record, &KRelayClient::OnUpdateMentorRecord, sizeof(R2S_UPDATE_MENTOR_RECORD));
    REGISTER_INTERNAL_FUNC(r2s_seek_mentor_yell, &KRelayClient::OnSeekMentorYell, sizeof(R2S_SEEK_MENTOR_YELL));
    REGISTER_INTERNAL_FUNC(r2s_seek_apprentice_yell, &KRelayClient::OnSeekApprenticeYell, sizeof(R2S_SEEK_APPRENTICE_YELL));
    //AutoCode:注册协议
}

BOOL KRelayClient::Init(IRecorderFactory* piFactory)
{
    BOOL               bResult          = false;
    BOOL               bRetCode         = false;
    IIniFile*	       piIniFile        = NULL;
	int                nRelayPort       = 0;
	int                nPingCycle       = 0;
    ISocketClient*     piSocketClient   = NULL;
    char               szRelayAddr[16];
    struct timeval     TimeVal;
    
    assert(piFactory);

    m_pbySyncRoleBuffer = new BYTE[MAX_ROLE_DATA_SIZE];
    KGLOG_PROCESS_ERROR(m_pbySyncRoleBuffer);

    m_uSyncRoleOffset = 0;
    m_dwSyncRoleID    = ERROR_ID;

    m_pbySaveRoleBuffer = new BYTE[MAX_ROLE_DATA_SIZE];
    KGLOG_PROCESS_ERROR(m_pbySaveRoleBuffer);

    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);

    piIniFile->GetString("Relay", "IP", "127.0.0.1", szRelayAddr, 16);
	piIniFile->GetInteger("Relay", "Port", 5003, &nRelayPort);
	piIniFile->GetInteger("Relay", "PingCycle", 20, &nPingCycle);
    
    piSocketClient = piFactory->CreateSocketClient();
    KGLOG_PROCESS_ERROR(piSocketClient);

	m_piSocketStream = piSocketClient->Connect(szRelayAddr, nRelayPort);
	KGLOG_PROCESS_ERROR(m_piSocketStream);

	TimeVal.tv_sec  = 8;
	TimeVal.tv_usec = 0;

	bRetCode = m_piSocketStream->SetTimeout(&TimeVal);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    m_bSendErrorFlag    = false;
	m_nPingCycle        = nPingCycle;
    m_nWorldIndex       = 0;

    m_nUpTraffic      = 0;
    m_nDownTraffic    = 0;
    m_fUpTraffic      = 0.0f;
    m_fDownTraffic    = 0.0f;

    bRetCode = DoHandshakeRequest();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_COM_RELEASE(m_piSocketStream);
        KG_DELETE_ARRAY(m_pbySaveRoleBuffer);
        KG_DELETE_ARRAY(m_pbySyncRoleBuffer);
    }

    KGLogPrintf(
        KGLOG_INFO, "Connect to center server %s:%d ... ... [%s]",
        szRelayAddr, nRelayPort, bResult ? "OK" : "Failed"
    );
    
    KG_COM_RELEASE(piSocketClient);
    KG_COM_RELEASE(piIniFile);
    return bResult;
}

void KRelayClient::UnInit(void)
{
    KG_COM_RELEASE(m_piSocketStream);
    KG_DELETE_ARRAY(m_pbySaveRoleBuffer);
    KG_DELETE_ARRAY(m_pbySyncRoleBuffer);
}

struct KSearchPlayerForExitingSave
{
    KSearchPlayerForExitingSave()
    {
        m_pPlayer = NULL;
    }

    BOOL operator()(DWORD dwID, KPlayer* pPlayer)
    {
        assert(pPlayer);
        if (pPlayer->m_nConnIndex >= 0)
        {
            m_pPlayer = pPlayer;
            return false;
        }

        return true;
    }

    KPlayer* m_pPlayer;
};

void KRelayClient::Activate()
{
    if (m_bQuiting)
    {
        int nPlayerCount = (int)g_pSO3World->m_PlayerSet.GetObjCount();
        if (nPlayerCount == 0)
        {
            g_pSO3World->m_bRunFlag = false;
        }

        if (g_pSO3World->m_nCurrentTime > m_nNextQuitingSaveTime)
        {
            KSearchPlayerForExitingSave TraverseFunc;

            g_pSO3World->m_PlayerSet.Traverse(TraverseFunc);

            if (TraverseFunc.m_pPlayer)
            {
                g_PlayerServer.Detach(TraverseFunc.m_pPlayer->m_nConnIndex);
            }

            m_nNextQuitingSaveTime = g_pSO3World->m_nCurrentTime + 5;
        }
    }

    DoUpdatePerformance();

    if (g_pSO3World->m_nGameLoop % GAME_FPS == 0)
    {
        m_fUpTraffic    = m_fUpTraffic * 0.9f + (float)m_nUpTraffic * 0.1f;
        m_fDownTraffic  = m_fDownTraffic * 0.9f + (float)m_nDownTraffic * 0.1f;

        m_nUpTraffic    = 0;
        m_nDownTraffic  = 0;
    }
}

BOOL KRelayClient::ProcessPackage()
{
    BOOL                    bResult             = false;
	int                     nRetCode            = false;
    IKG_Buffer*             piBuffer            = NULL;
    int                     nConnectionAlive    = true;

    assert(m_piSocketStream);

    if (m_bSendErrorFlag)
    {
        nConnectionAlive = false;
        m_bSendErrorFlag = false;
        goto Exit0;
    }

	while (true)
	{
	    const struct timeval        TimeVal     = {0, 0};
        INTERNAL_PROTOCOL_HEADER*   pHeader     = NULL;
        size_t                      uPakSize    = 0;
        PROCESS_PROTOCOL_FUNC       pFunc       = NULL;
        
		if (g_pSO3World->m_nCurrentTime - m_nLastSendPacketTime > m_nPingCycle)
		{
			DoPingSignal();
		}

	    nRetCode = m_piSocketStream->CheckCanRecv(&TimeVal);
        if (nRetCode == -1)
        {
            nConnectionAlive = false;
            goto Exit0;
        }
        if (nRetCode == 0)
        {
            break;
        }

        KGLOG_PROCESS_ERROR(nRetCode == 1);

        KG_COM_RELEASE(piBuffer);

		nRetCode = m_piSocketStream->Recv(&piBuffer);
        if (nRetCode == -1)
        {
            nConnectionAlive = false;
            goto Exit0;
        }
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		pHeader = (INTERNAL_PROTOCOL_HEADER*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pHeader);
          
        KGLOG_PROCESS_ERROR(pHeader->wProtocolID < r2s_protocol_end);
        
        uPakSize = piBuffer->GetSize();
        KGLOG_PROCESS_ERROR(uPakSize >= m_uProtocolSize[pHeader->wProtocolID]);

        m_nDownTraffic += (int)uPakSize;

        pFunc = m_ProcessProtocolFuns[pHeader->wProtocolID];
        KGLOG_PROCESS_ERROR(pFunc);

		(this->*pFunc)((BYTE*)pHeader, uPakSize);
	}
     
    bResult = true;
Exit0:
    if (m_piSocketStream && !nConnectionAlive)
    {
        KGLogPrintf(KGLOG_DEBUG, "[Relay] LastPakTime = %u\n", m_nLastSendPacketTime);
        KGLogPrintf(KGLOG_DEBUG, "[Relay] CurrentTime = %u\n", g_pSO3World->m_nCurrentTime);

        KGLogPrintf(KGLOG_ERR, "Game center lost, shutdown !\n");
        g_pSO3World->m_bRunFlag = false;
    }
    KG_COM_RELEASE(piBuffer);
	return bResult;
}

BOOL KRelayClient::SaveRoleData(KPlayer* pPlayer)
{
    BOOL            bResult      = false;
    BOOL            bRetCode     = false;
    size_t          uRoleDataLen = 0;
    BYTE*           pbyPos       = m_pbySaveRoleBuffer;
    BYTE*           pbyTail      = NULL;

    bRetCode = pPlayer->Save(&uRoleDataLen, m_pbySaveRoleBuffer, MAX_ROLE_DATA_SIZE);
    KGLOG_PROCESS_ERROR(bRetCode);

    pbyTail = m_pbySaveRoleBuffer + uRoleDataLen;

    while (pbyPos < pbyTail)
    {
        size_t uSubPakSize = min(pbyTail - pbyPos, MAX_ROLE_DATA_PAK_SIZE);

        bRetCode = DoSyncRoleData(pPlayer->m_dwID, pbyPos, pbyPos - m_pbySaveRoleBuffer, uSubPakSize);
        KGLOG_PROCESS_ERROR(bRetCode);

        pbyPos += uSubPakSize;
    }

    bRetCode = DoSaveRoleData(pPlayer, uRoleDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRelayClient::Send(IKG_Buffer* piBuffer)
{
    BOOL    bResult  = false;
    int     nRetCode = false;

    assert(piBuffer);
    KGLOG_PROCESS_ERROR(m_piSocketStream);

	nRetCode = m_piSocketStream->Send(piBuffer);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_nLastSendPacketTime = g_pSO3World->m_nCurrentTime;

    m_nUpTraffic += piBuffer->GetSize();

    bResult = true;
Exit0:
    if (!bResult)
        m_bSendErrorFlag = true;

	return bResult;
}

void KRelayClient::OnHandshakeRespond(BYTE* pbyData, size_t uDataLen)
{
	BOOL                    bRetCode   = false;
	R2S_HANDSHAKE_RESPOND*  pHandshake = (R2S_HANDSHAKE_RESPOND*)pbyData;

    assert(g_pSO3World->m_piRecorderTools);

    g_pSO3World->m_nBaseTime   = pHandshake->nBaseTime;
    g_pSO3World->m_dwStartTime = g_pSO3World->m_piRecorderTools->GetTickCount();;
    g_pSO3World->m_nStartLoop  = pHandshake->nStartFrame;
    g_pSO3World->m_nGameLoop   = pHandshake->nStartFrame;

    m_nWorldIndex = pHandshake->nWorldIndex;

    g_RelayClient.DoApplyMentorData(0, 0);

	KGLogPrintf(KGLOG_INFO, "Set world index = %d\n", pHandshake->nWorldIndex);

Exit0:
	return;
}

void KRelayClient::OnQuitNotify(BYTE* pbyData, size_t uDataLen)
{
    KGLogPrintf(KGLOG_INFO, "Quit signal from center server !");
    PrepareToQuit();
}

void KRelayClient::OnCreateMapNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult                 = false;
	BOOL                    bRetCode                = false;
	R2S_CREATE_MAP_NOTIFY*  pNotify                 = (R2S_CREATE_MAP_NOTIFY*)pbyData;
    KScene*                 pScene                  = NULL;
    BOOL                    bSceneInitFlag          = false;
	KMapParams*             pMapParams              = NULL;
    KScene*                 pCopyScene              = NULL;

    pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(pNotify->dwMapID);
    KGLOG_PROCESS_ERROR(pMapParams);

    KGLogPrintf(
        KGLOG_INFO, "%s(%.2u, %d) loading ... ...", 
        pMapParams->szMapName, pNotify->dwMapID, pNotify->nMapCopyIndex
    );

	pScene = KMemory::New<KScene>();
	KGLOG_PROCESS_ERROR(pScene);

	bRetCode = pScene->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bSceneInitFlag = true;

    pScene->m_bCanPK        = pMapParams->bCanPK;
    pScene->m_nCampType     = pMapParams->nCampType;

    pCopyScene = g_pSO3World->GetScene(pNotify->dwMapID, 0);
    if (pCopyScene)
    {
        assert(pCopyScene->m_pTerrainScene);
        pScene->m_pTerrainScene = pCopyScene->m_pTerrainScene;
        pScene->m_pTerrainScene->AddRef();
    }
    else
    {
        pScene->m_pTerrainScene = KMemory::New<KTerrainScene>();
        KGLOG_PROCESS_ERROR(pScene->m_pTerrainScene);
    }

	pScene->m_dwMapID               = pNotify->dwMapID;
    pScene->m_nBroadcastRegion      = pMapParams->nBroadcast;
	pScene->m_nCopyIndex            = pNotify->nMapCopyIndex;
    pScene->m_bReviveInSitu         = pMapParams->bReviveInSitu;
    pScene->m_dwOwnerID             = ERROR_ID;
    pScene->m_pPartitionTable       = pScene->m_pTerrainScene->GetPartitionTable();
    pScene->m_dwBanSkillMask        = pMapParams->dwBanSkillMask;
    pScene->m_dwBattleRelationMask  = pMapParams->dwBattleRelationMask;
    pScene->m_bDoNotGoThroughRoof   = pMapParams->bDoNotGoThroughRoof;
    pScene->m_nType                 = pMapParams->nType;
    pScene->m_nQuestCountAchID      = pMapParams->nQuestCountAchID;

	strncpy(pScene->m_szName, pMapParams->szMapName, sizeof(pScene->m_szName));
	pScene->m_szName[sizeof(pScene->m_szName) - 1] = 0;

    if (pNotify->uDataLen > 0)
    {
        BYTE*   pbyOffset   = pNotify->byData;
        size_t  uLeftSize   = pNotify->uDataLen;
        DWORD   dwBlockSize = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));
        dwBlockSize = *(DWORD*)pbyOffset;
        pbyOffset += sizeof(DWORD);
        uLeftSize -= sizeof(DWORD);

        KGLOG_PROCESS_ERROR(uLeftSize >= dwBlockSize);
        bRetCode = pScene->m_ProgressData.Load(pbyOffset, (size_t)dwBlockSize);
		KGLOG_PROCESS_ERROR(bRetCode);

        pbyOffset += dwBlockSize;
        uLeftSize -= dwBlockSize;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));
        dwBlockSize = *(DWORD*)pbyOffset;
        pbyOffset += sizeof(DWORD);
        uLeftSize -= sizeof(DWORD);

        KGLOG_PROCESS_ERROR(uLeftSize >= dwBlockSize);
		bRetCode = pScene->m_CustomData.Load(pbyOffset, dwBlockSize);
		KGLOG_PROCESS_ERROR(bRetCode);

        pbyOffset += dwBlockSize;
        uLeftSize -= dwBlockSize;

        KGLOG_PROCESS_ERROR(uLeftSize == 0);
                
        pScene->m_bSaved = true;
    }

    pScene->m_eSceneState = ssLoading;

    bRetCode = g_pSO3World->m_piSceneLoader->PostLoadingScene(pScene->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);
   
    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bSceneInitFlag)
        {
            pScene->UnInit();
            bSceneInitFlag = false;
        }

        if (pScene)
        {
            KMemory::Delete(pScene);
            pScene = NULL;
        }

	    DoCreateMapRespond(pNotify->dwMapID, pNotify->nMapCopyIndex, false);
    }
    return;
}

void KRelayClient::OnFinishCreateMapNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_FINISH_CREATE_MAP_NOTIFY* pNotify = (R2S_FINISH_CREATE_MAP_NOTIFY*)pbyData;
    KPlayer*                      pPlayer = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectSwitchMapErrorCode, smrcCreateMapFinished);

Exit0:
    return;
}

void KRelayClient::OnDeleteMapNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_DELETE_MAP_NOTIFY* pNotify  = (R2S_DELETE_MAP_NOTIFY*)pbyData;
    KScene*                pScene   = NULL;
	BOOL				   bRetCode = false; 

    pScene = g_pSO3World->GetScene(pNotify->dwMapID, pNotify->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    KGLogPrintf(KGLOG_INFO, "Delete map %s(%u, %d) !\n", pScene->m_szName, pScene->m_dwMapID, pScene->m_nCopyIndex);
    
    g_pSO3World->DeleteScene(pScene);

Exit0:
	return;
}

void KRelayClient::OnSearchMapRespond(BYTE* pbyData, size_t uDataLen)
{
	BOOL                        bRetCode            = false;
	R2S_SEARCH_MAP_RESPOND*     pRespond            = (R2S_SEARCH_MAP_RESPOND*)pbyData;
    KPlayer*                    pPlayer             = NULL;
    KScene*                     pScene              = NULL;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsSearchMap);

    if (pRespond->nResultCode != smrcSuccess)
    {
        pScene = g_pSO3World->GetScene(pPlayer->m_SavePosition.dwMapID, pPlayer->m_SavePosition.nMapCopyIndex);
	    KGLOG_PROCESS_ERROR(pScene);

	    bRetCode = g_pSO3World->AddPlayer(
            pPlayer, pScene, pPlayer->m_SavePosition.nX,
            pPlayer->m_SavePosition.nY, pPlayer->m_SavePosition.nZ
        );
	    KGLOG_PROCESS_ERROR(bRetCode);

        pScene->CallEnterSceneScript(pPlayer);

        pPlayer->m_eGameStatus      = gsPlaying;
        pPlayer->m_nRecordCount     = 0;
        
        if (pPlayer->m_eMoveState != cmsOnDeath)
        {
            pPlayer->m_nCurrentTrack = 0;
            pPlayer->Stop();
        }

        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectSwitchMapErrorCode, pRespond->nResultCode);

        goto Exit0;
    }

	if (pRespond->bSwitchServer)
	{
        // 将角色数据存盘,位置保存为跨服的目标点
        pPlayer->m_SavePosition.nX              = pRespond->nPosX;
        pPlayer->m_SavePosition.nY              = pRespond->nPosY;
        pPlayer->m_SavePosition.nZ              = pRespond->nPosZ;
        pPlayer->m_SavePosition.dwMapID         = pRespond->dwMapID;
        pPlayer->m_SavePosition.nMapCopyIndex   = pRespond->nMapCopyIndex;
        pPlayer->m_SavePosition.nFaceDirection  = pPlayer->m_nFaceDirection;
		pPlayer->m_eGameStatus                  = gsWaitForTransmissionSave;
        pPlayer->m_nBattleFieldSide             = pRespond->nBattleFieldSide;

	    bRetCode = SaveRoleData(pPlayer);
	    KGLOG_PROCESS_ERROR(bRetCode);
	}
    else
	{
        int nMaxX = 0;
        int nMaxY = 0;

		pScene = g_pSO3World->GetScene(pRespond->dwMapID, pRespond->nMapCopyIndex);
		KGLOG_PROCESS_ERROR(pScene);

        nMaxX = pScene->GetRegionWidth() * REGION_GRID_WIDTH * CELL_LENGTH - 1;
        nMaxY = pScene->GetRegionHeight() * REGION_GRID_HEIGHT * CELL_LENGTH - 1;

        MAKE_IN_RANGE(pRespond->nPosX, 0, nMaxX);
        MAKE_IN_RANGE(pRespond->nPosY, 0, nMaxY);
        MAKE_IN_RANGE(pRespond->nPosZ, 0, MAX_Z_POINT);

        pPlayer->m_SavePosition.nX              = pRespond->nPosX;
        pPlayer->m_SavePosition.nY              = pRespond->nPosY;
        pPlayer->m_SavePosition.nZ              = pRespond->nPosZ;
        pPlayer->m_SavePosition.dwMapID         = pRespond->dwMapID;
        pPlayer->m_SavePosition.nMapCopyIndex   = pRespond->nMapCopyIndex;
        pPlayer->m_nBattleFieldSide             = pRespond->nBattleFieldSide;

		g_PlayerServer.DoSwitchMap(
            pPlayer, pRespond->dwMapID, pRespond->nMapCopyIndex, 
            pRespond->nPosX, pRespond->nPosY, pRespond->nPosZ
        );

		pPlayer->m_eGameStatus = gsWaitForNewMapLoading;
	}

Exit0:
	return;
}

void KRelayClient::OnTransferPlayerRequest(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
	BOOL                            bRetCode    = false;
	R2S_TRANSFER_PLAYER_REQUEST*    pRequest    = (R2S_TRANSFER_PLAYER_REQUEST*)pbyData;
	KPlayer*                        pPlayer     = NULL;
    GUID                            Guid        = {0, 0, 0, 0};

    KG_PROCESS_ERROR(!m_bQuiting);
    KG_PROCESS_ERROR(m_bSyncDataOver);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRequest->dwRoleID);
    KGLOG_PROCESS_ERROR(pPlayer == NULL);

	pPlayer = g_pSO3World->NewPlayer(pRequest->dwRoleID);
	KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->LoadBaseInfo(&pRequest->BaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_bChargeFlag          = pRequest->bChargeFlag;
    pPlayer->m_ExtPointInfo         = pRequest->ExtPointInfo;
    pPlayer->m_bExtPointLock        = pRequest->bExtPointLock;
    pPlayer->m_nLastExtPointIndex   = pRequest->nLastExtPointIndex;
    pPlayer->m_nLastExtPointValue   = pRequest->nLastExtPointValue;
    pPlayer->m_nEndTimeOfFee        = pRequest->nEndTimeOfFee;
    pPlayer->m_nCoin                = pRequest->nCoin;
    pPlayer->m_dwSystemTeamID       = pRequest->dwSystemTeamID;
    pPlayer->m_dwTeamID             = pRequest->dwTeamID;
	pPlayer->m_nTimer               = 0;
	pPlayer->m_eGameStatus          = gsWaitForConnect;
    pPlayer->m_nBattleFieldSide     = pRequest->nBattleFieldSide;
    pPlayer->m_dwTongID             = pRequest->dwTongID;
    pPlayer->m_bFarmerLimit         = pRequest->byFarmerLimit;
    
    strncpy(pPlayer->m_szAccount, pRequest->szAccount, sizeof(pPlayer->m_szAccount));
    pPlayer->m_szAccount[sizeof(pPlayer->m_szAccount) - 1] = '\0';

    strncpy(pPlayer->m_szName, pRequest->szRoleName, sizeof(pPlayer->m_szName));
    pPlayer->m_szName[sizeof(pPlayer->m_szName) - 1] = '\0';

    assert(g_pSO3World->m_piRecorderTools);

    bRetCode = g_pSO3World->m_piRecorderTools->CreateGUID(&Guid);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_Guid = Guid;

    bResult = true;
Exit0:
	DoTransferPlayerRespond(pRequest->dwRoleID, bResult, Guid);

    if (!bResult)
    {
        if (pPlayer)
        {
            g_pSO3World->DelPlayer(pPlayer);
        }
    }

	return;
}

void KRelayClient::OnTransferPlayerRespond(BYTE* pbyData, size_t uDataLen)
{
	BOOL                            bRetCode    = false;
	R2S_TRANSFER_PLAYER_RESPOND*    pRespond    = (R2S_TRANSFER_PLAYER_RESPOND*)pbyData;
    KPlayer*                        pPlayer     = NULL;

    // 跨服中断线就可能找不到
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForTransmissionGuid);

    if (!pRespond->bSucceed)
    {
        KGLogPrintf(KGLOG_INFO, "Switch GS failed, shutdown(%d)\n", pPlayer->m_nConnIndex);

        g_PlayerServer.Detach(pPlayer->m_nConnIndex);
        goto Exit0;
    }

	g_PlayerServer.DoSwitchGS(
        pPlayer->m_nConnIndex, pRespond->dwPlayerID, 
        pRespond->Guid, pRespond->dwAddress, pRespond->wPort
    );

	g_PlayerServer.Detach(pPlayer->m_nConnIndex);

Exit0:
	return;
}

void KRelayClient::OnPlayerLoginRequest(BYTE* pbyData, size_t uDataLen)
{
	BOOL                        bRetCode            = false;
	BOOL                        bResult             = false;
	KPlayer*                    pPlayer             = NULL;
    R2S_PLAYER_LOGIN_REQUEST*   pRequest            = (R2S_PLAYER_LOGIN_REQUEST*)pbyData;
    GUID                        guid                = {0, 0, 0, 0};

    KG_PROCESS_ERROR(!m_bQuiting);
    KG_PROCESS_ERROR(m_bSyncDataOver);
	
	pPlayer = g_pSO3World->NewPlayer(pRequest->dwRoleID);
	KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->LoadBaseInfo(&pRequest->BaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_bChargeFlag      = (BOOL)pRequest->nChargeFlag;
    pPlayer->m_ExtPointInfo     = pRequest->ExtPointInfo;
    pPlayer->m_nEndTimeOfFee    = pRequest->nEndTimeOfFee;
    pPlayer->m_nCoin            = pRequest->nCoin;
    pPlayer->m_dwSystemTeamID   = pRequest->dwSystemTeamID;
    pPlayer->m_dwTeamID         = pRequest->dwTeamID;
    pPlayer->m_dwTongID         = pRequest->dwTongID;
    pPlayer->m_bFarmerLimit     = pRequest->byFarmerLimit;
    pPlayer->m_nTimer           = 0;
    pPlayer->m_nBattleFieldSide = pRequest->nBattleSide;
	pPlayer->m_eGameStatus      = gsWaitForConnect;

    strncpy(pPlayer->m_szAccount, pRequest->szAccount, sizeof(pPlayer->m_szAccount));
    pPlayer->m_szAccount[sizeof(pPlayer->m_szAccount) - 1] = '\0';

    strncpy(pPlayer->m_szName, pRequest->szRoleName, sizeof(pPlayer->m_szName));
    pPlayer->m_szName[sizeof(pPlayer->m_szName) - 1] = '\0';

    assert(g_pSO3World->m_piRecorderTools);

	bRetCode = g_pSO3World->m_piRecorderTools->CreateGUID(&guid);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_Guid = guid;

    g_pSO3World->m_StatDataServer.UpdateClientLoginPermit();

	bResult = true;
Exit0:
	DoPlayerLoginRespond(pRequest->dwRoleID, bResult, guid, pRequest->dwPacketIdentity);

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

void KRelayClient::OnConfirmPlayerLoginRespond(BYTE* pbyData, size_t uDataLen)
{
	R2S_CONFIRM_PLAYER_LOGIN_RESPOND*   pRespond = (R2S_CONFIRM_PLAYER_LOGIN_RESPOND*)pbyData;
    KPlayer*                            pPlayer  = NULL;

	// 登陆期间，客户端断线，就有可能在这里找不到
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForPermit);

    g_LogClient.LogPlayerLogin(pPlayer->m_szAccount, pPlayer->m_szName, pPlayer->m_szClientIP);

    if (!pRespond->bPermit)
    {
        KGLogPrintf(KGLOG_INFO, "Login denied by game center, shutdown(%d)\n", pPlayer->m_nConnIndex);

        g_PlayerServer.Detach(pPlayer->m_nConnIndex);
        goto Exit0;
    }

    g_PlayerServer.DoSyncPlayerBaseInfo(pPlayer->m_nConnIndex, pPlayer);
    g_pSO3World->m_MentorCache.SyncPlayerMentorData(pPlayer);

	pPlayer->m_eGameStatus  = gsWaitForLoginLoading;
    pPlayer->m_nTimer       = 0;
    pPlayer->m_nRecordCount = 0;

Exit0:
	return;
}

void KRelayClient::OnKickAccountNotify(BYTE* pbyData, size_t uDataLen)
{
	BOOL                        bRetCode    = false;
	R2S_KICK_ACCOUNT_NOTIFY*    pNotify     = (R2S_KICK_ACCOUNT_NOTIFY*)pbyData;
    KPlayer*                    pPlayer     = NULL;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

	g_PlayerServer.DoAccountKickout(pPlayer->m_nConnIndex);

    if (pPlayer->m_nConnIndex >= 0)
    {
        KGLogPrintf(KGLOG_INFO, "Kick account from game center, shutdown(%d)\n", pPlayer->m_nConnIndex);

        g_PlayerServer.Detach(pPlayer->m_nConnIndex);
    }

Exit0:
	return;
}

//帮派相关协议

//帮派数据更新协议


void KRelayClient::OnInvitePlayerJoinTeamRequest(BYTE* pbyData, size_t uDataLen)
{
    R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST* pTeamInvitation = (R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST*)pbyData;
    KPlayer* pDstPlayer = NULL;
    
    pDstPlayer = g_pSO3World->m_PlayerSet.GetObj(pTeamInvitation->dwInviteDst);
    KG_PROCESS_ERROR(pDstPlayer);
    
    if (pDstPlayer->m_eGameStatus != gsPlaying)
    {
        g_RelayClient.DoInviteJoinTeamRespond(
            pTeamInvitation->szInviteSrc, pTeamInvitation->dwInviteDst, false
        );
        goto Exit0;
    }

    pTeamInvitation->szInviteSrc[sizeof(pTeamInvitation->szInviteSrc) - 1] = '\0';

    if (
        (KCAMP)pTeamInvitation->bySrcCamp != cNeutral && pDstPlayer->m_eCamp != cNeutral &&
        (KCAMP)pTeamInvitation->bySrcCamp != pDstPlayer->m_eCamp
    )
    {
        KGFellowship* pFellowship       = NULL;
        int           nFellowshipLevel  = 0;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pTeamInvitation->dwInviteSrc, pDstPlayer->m_dwID);

        if (pFellowship)
        {
            nFellowshipLevel = KGFellowshipMgr::AttractionValue2Level(pFellowship->m_nAttraction);
        }

        if (nFellowshipLevel < g_pSO3World->m_Settings.m_ConstList.nDifferentCampApplyJoinTeam)
        {
            g_RelayClient.DoInviteJoinTeamRespond(pTeamInvitation->szInviteSrc, pTeamInvitation->dwInviteDst, false);
            goto Exit0;
        }
    }

    g_PlayerServer.DoInvitePlayerJoinTeamRequest(
        pTeamInvitation->dwInviteDst, pTeamInvitation->szInviteSrc
    );

Exit0:
    return;
}

void KRelayClient::OnApplyJoinTeamRequest(BYTE* pbyData, size_t uDataLen)
{
    R2S_APPLY_JOIN_TEAM_REQUEST* pApplyJoinTeam = (R2S_APPLY_JOIN_TEAM_REQUEST*)pbyData;
    KPlayer*                     pPlayer        = NULL;

    pApplyJoinTeam->szApplySrc[sizeof(pApplyJoinTeam->szApplySrc) - 1] = '\0';

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pApplyJoinTeam->dwApplyDst);
    KG_PROCESS_ERROR(pPlayer);

    if (
        (KCAMP)pApplyJoinTeam->bySrcCamp != cNeutral && pPlayer->m_eCamp != cNeutral &&
        (KCAMP)pApplyJoinTeam->bySrcCamp != pPlayer->m_eCamp
    )
    {
        KGFellowship* pFellowship       = NULL;
        int           nFellowshipLevel  = 0;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pPlayer->m_dwID, pApplyJoinTeam->dwApplySrc);

        if (pFellowship)
        {
            nFellowshipLevel = KGFellowshipMgr::AttractionValue2Level(pFellowship->m_nAttraction);
        }

        if (nFellowshipLevel < g_pSO3World->m_Settings.m_ConstList.nDifferentCampApplyJoinTeam)
        {
            g_RelayClient.DoApplyJoinTeamRespond(pApplyJoinTeam->dwApplyDst, pApplyJoinTeam->szApplySrc, false);
            goto Exit0;
        }
    }

    g_PlayerServer.DoApplyJoinTeamRequest(
        pApplyJoinTeam->dwApplyDst, pApplyJoinTeam->szApplySrc
    );

Exit0:
    return;
}

void KRelayClient::OnTeamCreateNotify(BYTE* pbyData, size_t uDataLen)
{
    int                      nRetCode       = false;
    R2S_TEAM_CREATE_NOTIFY*  pNotify        = (R2S_TEAM_CREATE_NOTIFY*)pbyData;
    KTeam*                   pTeam          = NULL;

    nRetCode = g_pSO3World->m_TeamServer.Create(pNotify->dwTeamID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pNotify->dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->bSystem      = pNotify->bSystem;
    pTeam->nGroupNum    = pNotify->nGroupNum;
    pTeam->nLootMode    = pNotify->nLootMode;
    pTeam->nRollQuality = pNotify->nRollQuality;
    pTeam->eCamp        = (KCAMP)pNotify->byCamp;

    memcpy(pTeam->dwAuthority, pNotify->dwAuthority, sizeof(pTeam->dwAuthority));
    memcpy(pTeam->dwMark, pNotify->dwTeamMark, sizeof(pTeam->dwMark));

    for (int i = 0; i < pNotify->byMemberCount; i++)
    {
        KTEAM_MEMBER_INFO* pMemberInfo  = NULL;

        nRetCode = g_pSO3World->m_TeamServer.AddMember(
            pNotify->dwTeamID, pNotify->Member[i].nGroupIndex, pNotify->Member[i].dwMemberID
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pNotify->dwTeamID, pNotify->Member[i].dwMemberID);
        KGLOG_PROCESS_ERROR(pMemberInfo);

        pMemberInfo->bOnlineFlag = (BOOL)pNotify->Member[i].byOnLineFlag;
        pMemberInfo->nLevel      = (int)pNotify->Member[i].byLevel;

        strncpy(pMemberInfo->szRoleName, pNotify->Member[i].szMemberName, sizeof(pMemberInfo->szRoleName));
        pMemberInfo->szRoleName[sizeof(pMemberInfo->szRoleName) - 1] = '\0';
    }

Exit0:
    return;
}

void KRelayClient::OnTeamAddMemberNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode                = false;
    R2S_TEAM_ADD_MEMBER_NOTIFY* pTeamAddMemberNotify   = (R2S_TEAM_ADD_MEMBER_NOTIFY*)pbyData;
    KTeam*                      pTeam                   = NULL;
    KTEAM_MEMBER_INFO*          pMemberInfo             = NULL;
    KTEAM_MEMBER_INFO*          pLeaderInfo             = NULL;
    KPlayer*                    pTeamMember             = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pTeamAddMemberNotify->dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    bRetCode = g_pSO3World->m_TeamServer.AddMember(
        pTeamAddMemberNotify->dwTeamID,
        pTeamAddMemberNotify->nGroupIndex,
        pTeamAddMemberNotify->dwMemberID
    );  
    KGLOG_PROCESS_ERROR(bRetCode);

    pLeaderInfo = g_pSO3World->m_TeamServer.GetMember(pTeamAddMemberNotify->dwTeamID, pTeam->dwAuthority[tatLeader]);

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pTeamAddMemberNotify->dwTeamID, pTeamAddMemberNotify->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    strncpy(pMemberInfo->szRoleName, pTeamAddMemberNotify->szMemberName, sizeof(pMemberInfo->szRoleName));
    pMemberInfo->szRoleName[sizeof(pMemberInfo->szRoleName) - 1] = '\0';

    pTeamMember = g_pSO3World->m_PlayerSet.GetObj(pTeamAddMemberNotify->dwMemberID);
    if (pLeaderInfo && pTeamMember) // if Team had leader
    {
        g_PlayerServer.DoMessageNotify(
            pTeamMember->m_nConnIndex, ectTeamNotifyCode, epncTeamJoined, 
            pLeaderInfo->szRoleName, sizeof(pLeaderInfo->szRoleName)
        );

        if (pTeam->bSystem || pTeamMember->m_dwSystemTeamID == ERROR_ID)
            g_PlayerServer.DoSyncTeamBaseInfo(pTeamAddMemberNotify->dwTeamID, pTeamMember, true);
    }
    else if (pTeamMember) // New Team, You are Leader
    {
        g_PlayerServer.DoMessageNotify(
            pTeamMember->m_nConnIndex, ectTeamNotifyCode, epncTeamCreated,
            0, 0
        );

        if (pTeam->bSystem || pTeamMember->m_dwSystemTeamID == ERROR_ID)
            g_PlayerServer.DoSyncTeamBaseInfo(pTeamAddMemberNotify->dwTeamID, pTeamMember, true);
    }
    //else: player not in this server !

    if (pTeamMember)
    {
        g_LogClient.LogPlayerJoinTeam(pTeamMember, pTeam);
    }

    g_PlayerServer.DoTeamAddMemberNotify(
        pTeamAddMemberNotify->dwTeamID, pTeamAddMemberNotify->nGroupIndex, pTeamAddMemberNotify->dwMemberID
    );

    if (pTeamMember)
    {
        pTeamMember->ResetTeamLastSyncParamRecord();
    }

Exit0:
    return;
}


void KRelayClient::OnTeamDelMemberNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode                = false;
    R2S_TEAM_DEL_MEMBER_NOTIFY* pTeamDelMemberNotify   = (R2S_TEAM_DEL_MEMBER_NOTIFY*)pbyData;
    KPlayer*                    pPlayer                 = NULL;
    DWORD                       dwTeamID                = ERROR_ID;

    bRetCode = g_pSO3World->m_TeamServer.DelMember(
        pTeamDelMemberNotify->dwTeamID, pTeamDelMemberNotify->nGroupIndex, pTeamDelMemberNotify->dwMemberID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoTeamDelMemberNotify(
        pTeamDelMemberNotify->dwTeamID, pTeamDelMemberNotify->nGroupIndex, pTeamDelMemberNotify->dwMemberID
    );

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pTeamDelMemberNotify->dwMemberID);
    if (pPlayer)
    {
        dwTeamID = pPlayer->GetCurrentTeamID();
        if (dwTeamID != ERROR_ID)
        {
            g_PlayerServer.DoSyncTeamBaseInfo(dwTeamID, pPlayer);
        }

        g_LogClient.LogPlayerLeaveTeam(pPlayer);
    }

Exit0:
    return;
}

void KRelayClient::OnTeamDisbandNotify(BYTE* pbyData, size_t uDataLen)
{
    int                         nRetCode            = false;
    R2S_TEAM_DISBAND_NOTIFY*    pTeamDisbandNotify = (R2S_TEAM_DISBAND_NOTIFY*)pbyData;
    KTeam*                      pTeam               = NULL;
    KTeam                       TempTeam;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pTeamDisbandNotify->dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    TempTeam = *pTeam;

    g_PlayerServer.DoTeamDisbandNotify(pTeamDisbandNotify->dwTeamID);

    g_pSO3World->m_TeamServer.Destroy(pTeamDisbandNotify->dwTeamID);

    for (int i = 0; i < TempTeam.nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = TempTeam.MemberGroup[i].MemberList.begin();
            it != TempTeam.MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer* pPlayer  = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
            DWORD    dwTeamID = ERROR_ID;

            if (!pPlayer)
                continue;

            g_LogClient.LogPlayerLeaveTeam(pPlayer);

            dwTeamID = pPlayer->GetCurrentTeamID();
            
            if (dwTeamID == ERROR_ID)
                continue;

            g_PlayerServer.DoSyncTeamBaseInfo(dwTeamID, pPlayer);
        }
    }

Exit0:
    return;
}

void KRelayClient::OnTeamChangeAuthorityNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode                    = false;
    R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*   pTeamChangeLeaderNotify    = (R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*)pbyData;

    // 设置队长
    bRetCode = g_pSO3World->m_TeamServer.SetAuthority(
        pTeamChangeLeaderNotify->dwTeamID, (KTEAM_AUTHORITY_TYPE)pTeamChangeLeaderNotify->byType, pTeamChangeLeaderNotify->dwNewID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    // 通知队伍成员
    g_PlayerServer.DoTeamChangeAuthorityNotify(
        pTeamChangeLeaderNotify->dwTeamID, (KTEAM_AUTHORITY_TYPE)pTeamChangeLeaderNotify->byType, pTeamChangeLeaderNotify->dwNewID
    );

Exit0:
    return;
}

void KRelayClient::OnTeamSetLootModeNotify(BYTE* pbyData, size_t uDataLen)
{
    int                             nRetCode    = false;
    R2S_TEAM_SET_LOOT_MODE_NOTIFY*  pNotify     = (R2S_TEAM_SET_LOOT_MODE_NOTIFY*)pbyData;
    KTeam*                          pTeam       = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pNotify->dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->nLootMode    = pNotify->nLootMode;
    pTeam->nRollQuality = pNotify->nRollQuality;

    g_PlayerServer.DoTeamSetLootModeNotify(pNotify->dwTeamID, pNotify->nLootMode, pNotify->nRollQuality);
    
Exit0:
    return;
}

void KRelayClient::OnSyncTeamMemberOnlineFlag(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode    = false;
    R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*   pPak        = (R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*)pbyData;
    size_t                              uPakSize    = 0;
    KTEAM_MEMBER_INFO*                  pMemberInfo = NULL;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwTeamID, pPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->bOnlineFlag = pPak->bOnlineFlag;

    g_PlayerServer.DoSyncTeamMemberOnlineFlag(pPak->dwTeamID, pPak->dwMemberID, pPak->bOnlineFlag);

Exit0:
    return;
}

void KRelayClient::OnTeamMessageNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_PARTY_MESSAGE_NOTIFY* pMsg      = (R2S_PARTY_MESSAGE_NOTIFY*)pbyData;
    KPlayer*                  pPlayer   = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pMsg->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoMessageNotify(
        pPlayer->m_nConnIndex, ectTeamNotifyCode, pMsg->nCode, 
        pMsg->byData, uDataLen - sizeof(R2S_PARTY_MESSAGE_NOTIFY)
    );

Exit0:
    return;
}

void KRelayClient::OnSetFormationLeaderNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                    bRetCode    = false;
    R2S_TEAM_SET_FORMATION_LEADER_NOTIFY*   pNotify     = (R2S_TEAM_SET_FORMATION_LEADER_NOTIFY*)pbyData;

    bRetCode = g_pSO3World->m_TeamServer.SetFormationLeader(
        pNotify->dwTeamID, pNotify->nGroupIndex, pNotify->dwNewFormationLeader
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoTeamSetFormationLeaderNotify(
        pNotify->dwTeamID, pNotify->nGroupIndex, pNotify->dwNewFormationLeader
    );
    
Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberMaxLMR(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode    = false;
    R2S_SYNC_TEAM_MEMBER_MAX_LMR*   pPak        = (R2S_SYNC_TEAM_MEMBER_MAX_LMR*)pbyData;
    size_t                          uPakSize    = 0;
    KTEAM_MEMBER_INFO*              pMemberInfo = NULL;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwTeamID, pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nMaxLife         = pPak->nMaxLife;
    pMemberInfo->nMaxMana         = pPak->nMaxMana;
    pMemberInfo->nMaxRage         = pPak->nMaxRage;

    g_PlayerServer.DoSyncTeamMemberMaxLMR(pPak->dwTeamID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberCurrentLMR(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode        = false;
    R2S_SYNC_TEAM_MEMBER_CURRENT_LMR*   pPak            = (R2S_SYNC_TEAM_MEMBER_CURRENT_LMR*)pbyData;
    KTEAM_MEMBER_INFO*                  pMemberInfo     = NULL;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwTeamID, pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nLifePercent     = pPak->byLifePercent;
    pMemberInfo->nManaPercent     = pPak->byManaPercent;
    pMemberInfo->nRagePercent     = pPak->byRagePercent;

    g_PlayerServer.DoSyncTeamMemberCurrentLMRGlobal(pPak->dwTeamID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberMisc(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode    = false;
    R2S_SYNC_TEAM_MEMBER_MISC*  pPak        = (R2S_SYNC_TEAM_MEMBER_MISC*)pbyData;
    size_t                      uPakSize    = 0;
    KTEAM_MEMBER_INFO*          pMemberInfo = NULL;
    
    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwTeamID, pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->PortraitInfo   = pPak->PortraitInfo;
    pMemberInfo->dwForceID      = pPak->dwForceID;
    pMemberInfo->eCamp          = (KCAMP)pPak->byCamp;
    pMemberInfo->nLevel         = pPak->byLevel;
	pMemberInfo->nRoleType      = pPak->byRoleType;
    pMemberInfo->bDeathFlag     = pPak->bDeathFlag;

    g_PlayerServer.DoSyncTeamMemberMisc(pPak->dwTeamID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberPosition(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode        = false;
    R2S_SYNC_TEAM_MEMBER_POSITION*  pPak            = (R2S_SYNC_TEAM_MEMBER_POSITION*)pbyData;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;
    size_t                          uPakSize        = 0;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwTeamID, pPak->dwMemberID);
    KG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->dwMapID         = pPak->dwMapID;
    pMemberInfo->nMapCopyIndex   = pPak->nMapCopyIndex;
    pMemberInfo->nPosX           = pPak->nPosX;
    pMemberInfo->nPosY           = pPak->nPosY;

    g_PlayerServer.DoSyncTeamMemberPositionGlobal(pPak->dwTeamID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSetMarkRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode        = false;
    R2S_TEAM_SET_MARK_RESPOND*  pTeamSetMark   = (R2S_TEAM_SET_MARK_RESPOND*)pbyData;

    g_pSO3World->m_TeamServer.SetMark(pTeamSetMark->dwTeamID, pTeamSetMark->nMarkType, pTeamSetMark->dwTarget);

    g_PlayerServer.DoTeamSetMarkRespond(pTeamSetMark->dwTeamID, pTeamSetMark->nMarkType, pTeamSetMark->dwTarget);

Exit0:
    return;
}

void KRelayClient::OnTeamCampChange(BYTE* pbyData, size_t uDataLen)
{
    R2S_TEAM_CAMP_CHANGE* pTeamCampChange   = (R2S_TEAM_CAMP_CHANGE*)pbyData;
    KTeam*                pTeam             = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pTeamCampChange->dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->eCamp = (KCAMP)pTeamCampChange->byCamp;

    g_PlayerServer.DoTeamCampChange(pTeamCampChange->dwTeamID, pTeam->eCamp);

Exit0:
    return;
}

void KRelayClient::OnTeamLevelUpRaidNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_TEAM_LEVEL_UP_RAID_NOTIFY* pNotify  = (R2S_TEAM_LEVEL_UP_RAID_NOTIFY*)pbyData;
    KTeam*                         pTeam    = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pNotify->dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->nGroupNum = pNotify->nGroupNum;

    g_PlayerServer.DoTeamLevelUpRaidNotify(pNotify->dwTeamID, pNotify->nGroupNum);
Exit0:
    return;
}

void KRelayClient::OnTeamChangeMemberGroupNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                 bRetCode   = false;
    R2S_TEAM_CHANGE_MEMBER_GROUP_NOTIFY* pNotify    = (R2S_TEAM_CHANGE_MEMBER_GROUP_NOTIFY*)pbyData;

    bRetCode = g_pSO3World->m_TeamServer.ChangeMember(
        pNotify->dwTeamID, pNotify->nOldGroupIndex, pNotify->nNewGroupIndex,
        pNotify->dwMemberID, pNotify->dwDstMemberID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoTeamChangeMemberGroupNotify(
        pNotify->dwTeamID, pNotify->nOldGroupIndex, pNotify->nNewGroupIndex, pNotify->dwDstMemberID, pNotify->dwMemberID
    );

Exit0:
    return;
}

void KRelayClient::OnPlayerTalkError(BYTE* pbyData, size_t uDataLen)
{
    BOOL                     bRetCode         = false;
    KPlayer*                 pPlayer          = NULL;
    int                      nPlayerConnIndex = -1;
    R2S_PLAYER_TALK_ERROR*   pPak             = (R2S_PLAYER_TALK_ERROR*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwReceiverID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nPlayerConnIndex = pPlayer->m_nConnIndex;
    KGLOG_PROCESS_ERROR(nPlayerConnIndex >= 0);

    bRetCode = g_PlayerServer.DoMessageNotify(nPlayerConnIndex, ectTalkErrorCode, pPak->nErrorCode);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KRelayClient::OnPlayerTalkWhisperSuccessRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                        bRetCode    = false;
    R2S_PLAYER_TALK_WHISPER_SUCCESS_RESPOND*    pPak        = (R2S_PLAYER_TALK_WHISPER_SUCCESS_RESPOND*)pbyData;
    KGFellowship*                               pFellowship = NULL;

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pPak->dwTalkerID, pPak->dwReceiverID);
    if (pFellowship && pFellowship->m_BitFlag.nWhisperDailyCount > 0 && pFellowship->m_nAttraction < g_pSO3World->m_Settings.m_ConstList.nWhisperAttractiveUpLimit)
    {
        g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(pPak->dwTalkerID, pPak->dwReceiverID, g_pSO3World->m_Settings.m_ConstList.nWhisperAttractiveAdd, false);
        pFellowship->m_BitFlag.nWhisperDailyCount--;
    }

Exit0:
    return;
}

void KRelayClient::OnTalkMessage(BYTE* pbyData, size_t uDataLen)
{
    R2S_TALK_MESSAGE*   pTalkMsg        = (R2S_TALK_MESSAGE*)pbyData;
    size_t              uTalkDataLen    = 0;

    uTalkDataLen = uDataLen - sizeof(R2S_TALK_MESSAGE);

	g_PlayerServer.DoTalkMessage(
        pTalkMsg->byMsgType, 
        pTalkMsg->dwTalkerID, pTalkMsg->szTalker,
        pTalkMsg->dwReceiverID, pTalkMsg->szReceiver,
        uTalkDataLen, pTalkMsg->byTalkData
    );
    
    return;
}

void KRelayClient::OnApplyFellowshipDataRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode    = false;
    KPlayer*                            pPlayer     = NULL;
    R2S_APPLY_FELLOWSHIP_DATA_RESPOND*  pPak        = (R2S_APPLY_FELLOWSHIP_DATA_RESPOND*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = g_pSO3World->m_FellowshipMgr.OnLoadFellowshipData(pPak->dwPlayerID, uDataLen - sizeof(R2S_APPLY_FELLOWSHIP_DATA_RESPOND), pPak->byData);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KRelayClient::OnAddFellowshipRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_ADD_FELLOWSHIP_RESPOND* pPak        = (R2S_ADD_FELLOWSHIP_RESPOND*)pbyData;
    KPlayer*                    pPlayer     = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KGFellowship*               pFellowship = NULL;
    KGFoe*                      pFoe        = NULL;

    switch (pPak->byType)
    {
    case 1:
        pFellowship = g_pSO3World->m_FellowshipMgr.AddFellowship(pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, true);

        if (pPlayer && pPlayer->m_pScene)
            g_pSO3World->m_FellowshipMgr.ProcessFellowshipCountAchievement(pPlayer);

        if (pPlayer && pFellowship)
            g_LogClient.LogPlayerAddFriend(pPlayer, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName);

        break;

    case 2:
        pFoe = g_pSO3World->m_FellowshipMgr.AddFoe(pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, true);

        if (pPlayer && pFoe)
            g_LogClient.LogPlayerAddFoe(pPlayer, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName);

        break;

    case 3:
        g_pSO3World->m_FellowshipMgr.AddBlackList(pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, true);
        break;

    default:
        break;
    }

    return;
}

void KRelayClient::OnGetFellowshipNameRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_GET_FELLOWSHIP_NAME_RESPOND* pPak = (R2S_GET_FELLOWSHIP_NAME_RESPOND*)pbyData;

    for (int i = 0; i < (int)pPak->byCount; i++)
    {
        KGFellowship*   pFellowship = NULL;
        KGFoe*          pFoe        = NULL;
        KGBlackNode*    pBlackNode  = NULL;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
        pFoe        = g_pSO3World->m_FellowshipMgr.GetFoe(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
        pBlackNode  = g_pSO3World->m_FellowshipMgr.GetBlackListNode(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);

        if (pPak->AlliedPlayerInfo[i].szName[0] == '\0')
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "In Player(%u)'s fellowship list, player(%u) can't find in game center. This fellowship is removed.", 
                pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID 
            );

            if (pFellowship)
            {
                g_pSO3World->m_FellowshipMgr.DelFellowship(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
                pFellowship = NULL;
            }

            if (pFoe)
            {
                g_pSO3World->m_FellowshipMgr.DelFoe(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
                pFoe = NULL;
            }

            if (pBlackNode)
            {
                g_pSO3World->m_FellowshipMgr.DelBlackList(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
                pFoe = NULL;
            }

            continue;
        }

        if (pFellowship)
        {
            pFellowship->m_dwMapID = pPak->AlliedPlayerInfo[i].dwMapID;

            strncpy(pFellowship->m_szName, pPak->AlliedPlayerInfo[i].szName, sizeof(pFellowship->m_szName));
            pFellowship->m_szName[sizeof(pFellowship->m_szName) - sizeof('\0')] = '\0';
        }

        if (pFoe)
        {
            strncpy(pFoe->m_szName, pPak->AlliedPlayerInfo[i].szName, sizeof(pFoe->m_szName));
            pFoe->m_szName[sizeof(pFoe->m_szName) - sizeof('\0')] = '\0';
        }

        if (pBlackNode)
        {
            strncpy(pBlackNode->m_szName, pPak->AlliedPlayerInfo[i].szName, sizeof(pBlackNode->m_szName));
            pBlackNode->m_szName[sizeof(pBlackNode->m_szName) - sizeof('\0')] = '\0';
        }
    }

Exit0:
    return;
}

struct _UpdateMapID 
{
    bool operator () (DWORD dwAlliedPlayerID, DWORD dwPlayerID)
    {
        KGFellowship* pFellowship = NULL;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, dwAlliedPlayerID);

        if (pFellowship)
        {
            pFellowship->m_dwMapID = dwMapID;
        }

        return true;
    }

    DWORD   dwMapID;
};

void KRelayClient::OnSyncFellowshipMapID(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_FELLOWSHIP_MAPID* pPak = (R2S_SYNC_FELLOWSHIP_MAPID*)pbyData;
    _UpdateMapID UpdateMapID;

    UpdateMapID.dwMapID = pPak->dwMapID;

    g_pSO3World->m_FellowshipMgr.TraverseReverseFellowshipID(pPak->dwPlayerID, UpdateMapID);

Exit0:
    return;
}

struct _UpdateFellowshipLevel 
{
    bool operator () (DWORD dwAlliedPlayerID, DWORD dwPlayerID)
    {
        KGFellowship*   pFellowship     = NULL;
        int             nOldLevel       = 0;
        KPlayer*        pPlayer         = NULL;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, dwAlliedPlayerID);
        if (!pFellowship)
            goto Exit1;

        nOldLevel = pFellowship->m_nPlayerLevel;
        pFellowship->m_nPlayerLevel = nLevel;

        if (!bNotifyClient || (!nOldLevel) == (!nLevel))
            goto Exit1;

        pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
        if (!pPlayer)
            goto Exit1;

        g_PlayerServer.DoMessageNotify(
            pPlayer->m_nConnIndex, ectFellowshipOnline, (nLevel != 0), 
            &dwAlliedPlayerID, sizeof(DWORD)
        );

Exit1:
        return true;
    }

    int     nLevel;
    BOOL    bNotifyClient;
};

struct _UpdateFoeLevel 
{
    bool operator () (DWORD dwAlliedPlayerID, DWORD dwPlayerID)
    {
        KGFoe*      pFoe        = NULL;
        int         nOldLevel   = 0;
        KPlayer*    pPlayer     = NULL;

        pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(dwPlayerID, dwAlliedPlayerID);
        if (!pFoe)
            goto Exit1;

        nOldLevel = pFoe->m_nPlayerLevel;
        pFoe->m_nPlayerLevel = nLevel;

        if (!bNotifyClient || (!nOldLevel) == (!nLevel))
            goto Exit1;

        pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
        if (!pPlayer)
            goto Exit1;

        g_PlayerServer.DoMessageNotify(
            pPlayer->m_nConnIndex, ectFoeOnline, (nLevel != 0), 
            &dwAlliedPlayerID, sizeof(DWORD)
        );

Exit1:
        return true;
    }

    int     nLevel;
    BOOL    bNotifyClient;
};

void KRelayClient::OnSyncFellowshipPlayerLevel(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL*   pPak = (R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL*)pbyData;
    _UpdateFellowshipLevel              UpdateFellowshipLevel;
    _UpdateFoeLevel                     UpdateFoeLevel;

    UpdateFellowshipLevel.nLevel        = pPak->byLevel;
    UpdateFellowshipLevel.bNotifyClient = pPak->bNotifyClient;
    UpdateFoeLevel.nLevel               = pPak->byLevel;
    UpdateFoeLevel.bNotifyClient        = pPak->bNotifyClient;

    g_pSO3World->m_FellowshipMgr.TraverseReverseFellowshipID(pPak->dwPlayerID, UpdateFellowshipLevel);
    g_pSO3World->m_FellowshipMgr.TraverseReverseFoeID(pPak->dwPlayerID, UpdateFoeLevel);

Exit0:
    return;
}

struct _UpdateFellowshipForceID 
{
    bool operator () (DWORD dwAlliedPlayerID, DWORD dwPlayerID)
    {
        KGFellowship* pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, dwAlliedPlayerID);
        if (!pFellowship)
            goto Exit1;

        pFellowship->m_dwForceID = dwForceID;

Exit1:
        return true;
    }

    DWORD dwForceID;
};

struct _UpdateFoeForceID 
{
    bool operator () (DWORD dwAlliedPlayerID, DWORD dwPlayerID)
    {
        KGFoe* pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(dwPlayerID, dwAlliedPlayerID);
        if (!pFoe)
            goto Exit1;

        pFoe->m_dwForceID = dwForceID;

Exit1:
        return true;
    }

    DWORD dwForceID;
};

void KRelayClient::OnSyncFellowshipPlayerForceID(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID* pPak = (R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID*)pbyData;
    _UpdateFellowshipForceID            UpdateFellowshipForceID;
    _UpdateFoeForceID                   UpdateFoeForceID;

    UpdateFellowshipForceID.dwForceID   = pPak->byForceID;
    UpdateFoeForceID.dwForceID          = pPak->byForceID;

    g_pSO3World->m_FellowshipMgr.TraverseReverseFellowshipID(pPak->dwPlayerID, UpdateFellowshipForceID);
    g_pSO3World->m_FellowshipMgr.TraverseReverseFoeID(pPak->dwPlayerID, UpdateFoeForceID);

Exit0:
    return;
}

void KRelayClient::OnAddFellowshipNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_ADD_FELLOWSHIP_NOTIFY*  pPak        = (R2S_ADD_FELLOWSHIP_NOTIFY*)pbyData;
    void*                       pvRetCode   = NULL;
    KPlayer*                    pPlayer     = NULL;
    int                         nCode       = rrcInvalid;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    switch(pPak->nType)
    {
    case 1:
        pvRetCode = (void*)g_pSO3World->m_FellowshipMgr.GetFellowship(pPak->dwPlayerID, pPak->dwAlliedPlayerID);
        KG_PROCESS_ERROR(pvRetCode == NULL);

        nCode = rrcBeAddFriend;
        break;
    case 2:
        pvRetCode = (void*)g_pSO3World->m_FellowshipMgr.GetFoe(pPak->dwPlayerID, pPak->dwAlliedPlayerID);
        KG_PROCESS_ERROR(pvRetCode == NULL);

        nCode = rrcBeAddFoe;
        break;
    case 3:
        pvRetCode = (void*)g_pSO3World->m_FellowshipMgr.GetBlackListNode(pPak->dwPlayerID, pPak->dwAlliedPlayerID);
        KG_PROCESS_ERROR(pvRetCode == NULL);

        nCode = rrcBeAddBlackList;
        break;
    }

    pPak->szAlliedPlayerName[sizeof(pPak->szAlliedPlayerName) - 1] = '\0';

    g_PlayerServer.DoMessageNotify(
        pPlayer->m_nConnIndex, ectFellowshipErrorCode, nCode, 
        pPak->szAlliedPlayerName, sizeof(pPak->szAlliedPlayerName)
    );

Exit0:
    return;
}

void KRelayClient::OnSendMailRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bRetCode     = 0;
    R2S_SEND_MAIL_RESPOND*  pPak         = (R2S_SEND_MAIL_RESPOND*)pbyData;
    KPlayer*                pPlayer      = NULL;
    KMail*                  pMail        = (KMail*)pPak->byData;
    KItem*                  pItem        = NULL;
    BYTE*                   pbyItemData  = NULL;
    int                     uMailLen     = 0;
    int                     nMoney       = 0;
    int                     nPlayerMoney = 0;
    int                     nMoneyLimit  = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwSrcID);
    KG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    KG_PROCESS_SUCCESS(pPak->byResult == mrcSucceed);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(R2S_SEND_MAIL_RESPOND) + sizeof(KMail));
    uMailLen = sizeof(KMail) + pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        uMailLen += pMail->ItemDesc[i].byDataLen;
    }
    KGLOG_PROCESS_ERROR(uMailLen + sizeof(R2S_SEND_MAIL_RESPOND) == uDataLen);

    // Money
    nMoney = pMail->nMoney + KMAIL_COST;

    nPlayerMoney = pPlayer->m_ItemList.GetMoney();
    nMoneyLimit  = pPlayer->m_ItemList.GetMoneyLimit();

    if(nMoneyLimit - nPlayerMoney < nMoney)
    {
        KGLogPrintf(KGLOG_ERR, "[Mail] Add money(%d) to player(%s) failed!",
            nMoney, pPlayer->m_szName
        );
    }
    else
    {
        pPlayer->m_ItemList.AddMoney(nMoney, false);

        g_LogClient.LogPlayerMoneyChange(nMoney, pPlayer, "send mail failed: ", pPak->szReceiver);
    }

    // Item
    pbyItemData = pMail->byData + pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        size_t uItemDataLen = pMail->ItemDesc[i].byDataLen;
        if (uItemDataLen == 0)
            continue;

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pbyItemData, uItemDataLen);
        KGLOG_PROCESS_ERROR(pItem);
        pbyItemData += uItemDataLen;

        bRetCode = pPlayer->m_ItemList.AddItem(pItem);
        if (bRetCode != aircSuccess)
        {
            KGLogPrintf(KGLOG_ERR, "[Mail] Add item(%d, %d) to player(%s) failed!",
                pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, pPlayer->m_szName
            );
            g_pSO3World->m_ItemManager.FreeItem(pItem);
        }
        else
        {
            g_LogClient.LogPlayerItemChange(
                pPlayer, ITEM_OPERATION_TYPE_IN, pItem, "send mail failed: ", pPak->szReceiver
            );
        }

        pItem = NULL;
    }

Exit1:
Exit0:
    if (pPlayer)
    {
        g_PlayerServer.DoSendMailRespond(pPlayer->m_nConnIndex, pPak->byRespondID, pPak->byResult);
    }

    return;
}

void KRelayClient::OnGetMailListRespond(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                    pPlayer         = NULL;
    R2S_GET_MAILLIST_RESPOND*   pPak            = (R2S_GET_MAILLIST_RESPOND*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    g_PlayerServer.DoGetMailListRespond(
        pPlayer->m_nConnIndex, pPak->MailList, pPak->nMailCount
    );

Exit0:
    return;
}

void KRelayClient::OnSyncMailContent(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                pPlayer     = NULL;
    R2S_SYNC_MAIL_CONTENT*  pPak        = (R2S_SYNC_MAIL_CONTENT*)pbyData;
    KMailContent*           pContent    = NULL;
    size_t                  uContentLen = 0;

    if (pPak->byResult == mrcSucceed)
    {
        pContent = (KMailContent*)pPak->byData;

        KGLOG_PROCESS_ERROR(uDataLen >= sizeof(R2S_SYNC_MAIL_CONTENT) + sizeof(KMailContent));
        uContentLen = sizeof(KMailContent) + pContent->wTextLen;
        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            uContentLen += pContent->ItemDesc[i].byDataLen;
        }
    }
    KGLOG_PROCESS_ERROR(uContentLen + sizeof(R2S_SYNC_MAIL_CONTENT) == uDataLen);

    g_PlayerServer.DoSyncMailContent(
        pPak->dwPlayerID, pPak->dwMailID, pPak->byResult, pContent
    );

Exit0:
    return;
}

void KRelayClient::OnGiveMailMoneyToPlayer(BYTE* pbyData, size_t uDataLen)
{
    KMAIL_RESULT_CODE               eResult       = mrcFailed;
    int                             nRetCode      = 0;
    KPlayer*                        pPlayer       = NULL;
    R2S_GIVE_MAIL_MONEY_TO_PLAYER*  pPak          = (R2S_GIVE_MAIL_MONEY_TO_PLAYER*)pbyData;
    int                             nPlayerMoney  = 0;
    int                             nMoneyLimit   = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    KGLOG_PROCESS_ERROR(pPak->nMoney > 0);

    nPlayerMoney    = pPlayer->m_ItemList.GetMoney();
    nMoneyLimit     = pPlayer->m_ItemList.GetMoneyLimit();

    if (nMoneyLimit - nPlayerMoney < pPak->nMoney)
    {
        eResult = mrcMoneyLimit;
        goto Exit0;
    }

    nRetCode = pPlayer->m_ItemList.AddMoney(pPak->nMoney);
    KGLOG_PROCESS_ERROR(nRetCode);

    g_LogClient.LogPlayerMoneyChange(pPak->nMoney, pPlayer, "get from mail: ", pPak->szSender);

    eResult = mrcSucceed;
Exit0:
    DoGiveMailMoneyToPlayerRespond(pPak->dwPlayerID, pPak->dwMailID, eResult);
    return;
}

void KRelayClient::OnGiveMailItemToPlayer(BYTE* pbyData, size_t uDataLen)
{
    int                             nRetCode        = 0;
    KPlayer*                        pPlayer         = NULL;
    R2S_GIVE_MAIL_ITEM_TO_PLAYER*   pPak            = (R2S_GIVE_MAIL_ITEM_TO_PLAYER*)pbyData;
    BOOL                            bGotItem        = false;
    KItem*                          pItem           = NULL;
    int                             nPlayerMoney    = 0;
    KMAIL_RESULT_CODE               eResult         = mrcFailed;

    KGLOG_PROCESS_ERROR(pPak->byItemLen > 0);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    KG_PROCESS_ERROR(pPak->byResult == mrcSucceed);

    KG_PROCESS_ERROR(pPak->nItemPrice >= 0);

    nPlayerMoney    = pPlayer->m_ItemList.GetMoney();

    if(nPlayerMoney < pPak->nItemPrice)
    {
        eResult = mrcNotEnoughMoney;
        goto Exit0;
    }

    pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pPak->byData, pPak->byItemLen);
    KGLOG_PROCESS_ERROR(pItem);

    nRetCode = pPlayer->m_ItemList.CanAddItem(pItem);
    if (nRetCode == aircSuccess)
    {
        KITEM_LOG_INFO  ItemLogInfo;

        g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);

        nRetCode = pPlayer->m_ItemList.AddItem(pItem);
        KGLOG_PROCESS_ERROR(nRetCode == aircSuccess);
        pItem = NULL;
    
        g_LogClient.LogPlayerItemChange(pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "get from mail: ", pPak->szSender);

        nRetCode = pPlayer->m_ItemList.AddMoney(-pPak->nItemPrice);
        KGLOG_PROCESS_ERROR(nRetCode);

        g_LogClient.LogPlayerMoneyChange(-pPak->nItemPrice, pPlayer, "cash on delivery to: ", pPak->szSender);

        bGotItem = true;
        eResult  = mrcSucceed;
    }
    else if (nRetCode == aircNotEnoughFreeRoom)
    {
        eResult = mrcNotEnoughRoom;
    }
    else if (nRetCode == aircItemAmountLimited)
    {
        eResult = mrcItemAmountLimit;
    }

Exit0:
    DoGiveMailItemToPlayerRespond(pPak->dwPlayerID, pPak->dwMailID, pPak->byItemIndex, eResult);
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return;
}

void KRelayClient::OnNewMailNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_NEW_MAIL_NOTIFY*    pNotify  = (R2S_NEW_MAIL_NOTIFY*)pbyData;
    KPlayer*                pPlayer  = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    g_PlayerServer.DoNewMailNotify(pPlayer->m_nConnIndex, &(pNotify->NewMailListInfo));

Exit0:
    return;
}

void KRelayClient::OnMailGeneralRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_MAIL_GENERAL_RESPOND*   pRespond    = (R2S_MAIL_GENERAL_RESPOND*)pbyData;

    g_PlayerServer.DoMailGeneralRespond(pRespond->dwPlayerID, pRespond->dwMailID, pRespond->byRespond);
    
    return;
}

void KRelayClient::OnSyncMidMapMark(BYTE* pbyData, size_t uDataLen)
{
	R2S_SYNC_MID_MAP_MARK* pPak = (R2S_SYNC_MID_MAP_MARK*)pbyData;

	g_PlayerServer.DoSyncMidMapMark(
        pPak->dwTeamID, pPak->dwSrcPlayerID, pPak->nMapID, 
        pPak->nX, pPak->nY, pPak->nType, pPak->szComment
    );

	return;
}

struct KClearPlayerPrepareFunc
{
    int m_nDelayTime;
    int m_nType;

    BOOL operator () (KPlayer* pPlayer) const
    {
        g_PlayerServer.DoMessageNotify(
            pPlayer->m_nConnIndex, ectBanishNotifyCode, m_nType, &m_nDelayTime, sizeof(m_nDelayTime)
        );

        return true;
    }
};

void KRelayClient::OnMapCopyDoClearPlayerPrepare(BYTE* pbyData, size_t uDataLen)
{   
    KScene*			                        pScene          = NULL;
    R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE*   pPrepareClear   = (R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE*)pbyData;
    KClearPlayerPrepareFunc                 Func;

    Func.m_nDelayTime = pPrepareClear->nDelayTime;
    Func.m_nType      = pPrepareClear->nType;

    pScene = g_pSO3World->GetScene(pPrepareClear->dwMapID, pPrepareClear->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraversePlayer(Func);

Exit0:
    return;
}

struct KClearPlayerFunc
{
    BOOL operator () (KPlayer* pPlayer) const
    {
        pPlayer->SwitchMap(
            pPlayer->m_LastEntry.dwMapID, pPlayer->m_LastEntry.nMapCopyIndex,
            pPlayer->m_LastEntry.nX, pPlayer->m_LastEntry.nY, pPlayer->m_LastEntry.nZ
        );

        return true;
    }
};

void KRelayClient::OnMapCopyDoClearPlayer(BYTE* pbyData, size_t uDataLen)
{   
    KScene*			                pScene   = NULL;
    R2S_MAP_COPY_DO_CLEAR_PLAYER*   pClear    = (R2S_MAP_COPY_DO_CLEAR_PLAYER*)pbyData;
    KClearPlayerFunc                Func;
    
    pScene = g_pSO3World->GetScene(pClear->dwMapID, pClear->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraversePlayer(Func);
    pScene->DeleteAllNpc();
    pScene->DeleteAllDoodad();

Exit0:
    return;
}

void KRelayClient::OnSetMapCopyOwner(BYTE* pbyData, size_t uDataLen)
{
    R2S_SET_MAP_COPY_OWNER* pSetOwner = (R2S_SET_MAP_COPY_OWNER*)pbyData;
    KScene*                 pScene    = NULL;

    pScene = g_pSO3World->GetScene(pSetOwner->dwMapID, pSetOwner->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    pScene->m_dwOwnerID = pSetOwner->dwOwnerID;

Exit0:
    return;
}

void KRelayClient::OnSyncRoadTrackInfo(BYTE* pbyData, size_t uDataLen)
{
    KROUTE_NODE*                pNode       = NULL;
    R2S_SYNC_ROAD_TRACK_INFO*   pRoadTrack  = (R2S_SYNC_ROAD_TRACK_INFO*)pbyData;

    pNode = g_pSO3World->m_RoadManager.GetNodeInfo(pRoadTrack->nNodeID);
    KGLOG_PROCESS_ERROR(pNode);

    pNode->eCamp    = (KCAMP)pRoadTrack->byCamp;
    pNode->bEnable  = pRoadTrack->byEnable;
    pNode->bChange  = true;

Exit0:
    return;
}

void KRelayClient::OnSaveRoleDataRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode        = false;
    int                             nStateForSaving = 0;
    KPlayer*                        pPlayer         = NULL;
    R2S_SAVE_ROLE_DATA_RESPOND*     pRespond        = (R2S_SAVE_ROLE_DATA_RESPOND*)pbyData;

    m_nNextQuitingSaveTime = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    if (pPlayer != NULL &&  pRespond->nUserValue == gsWaitForTransmissionSave)
    {
        KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForTransmissionSave);
        
        if (!pRespond->bSucceed)
        {
            KGLogPrintf(
                KGLOG_ERR, "Transmission save failed: ID = %u, Name = %s\n",
                pPlayer->m_dwID, pPlayer->m_szName
            );
            g_PlayerServer.Detach(pPlayer->m_nConnIndex);
            goto Exit0;
        }

        g_RelayClient.DoTransferPlayerRequest(pPlayer);
        pPlayer->m_eGameStatus = gsWaitForTransmissionGuid;
    }
    
Exit0:
    return;
}

void KRelayClient::OnSyncRoleData(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    R2S_SYNC_ROLE_DATA* pSyncData       = (R2S_SYNC_ROLE_DATA*)pbyData;
    BYTE*               pbyOffset       = m_pbySyncRoleBuffer + m_uSyncRoleOffset;
    size_t              uRoleDataLen    = uDataLen - sizeof(R2S_SYNC_ROLE_DATA);

    KGLOG_PROCESS_ERROR(pSyncData->uOffset == m_uSyncRoleOffset);
    KGLOG_PROCESS_ERROR(MAX_ROLE_DATA_SIZE - m_uSyncRoleOffset >= uRoleDataLen);
    KGLOG_PROCESS_ERROR(pSyncData->uOffset == 0 || pSyncData->dwRoleID == m_dwSyncRoleID);

    memcpy(pbyOffset, pSyncData->byData, uRoleDataLen);

    m_uSyncRoleOffset += uRoleDataLen;
    m_dwSyncRoleID     = pSyncData->dwRoleID;

    bResult = true;    
Exit0:
    if(!bResult)
    {
        m_dwSyncRoleID    = ERROR_ID;
        m_uSyncRoleOffset = 0;
    }
    return;
}

void KRelayClient::OnLoadRoleData(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult   = false;
    BOOL                bRetCode  = false;
    KPlayer*            pPlayer   = NULL;
    R2S_LOAD_ROLE_DATA* pNotify   = (R2S_LOAD_ROLE_DATA*)pbyData;
    KScene*             pScene    = NULL;

    KGLOG_PROCESS_ERROR(pNotify->uRoleDataLen == m_uSyncRoleOffset);
    KGLOG_PROCESS_ERROR(pNotify->dwRoleID == m_dwSyncRoleID || m_dwSyncRoleID == ERROR_ID);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwRoleID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pNotify->bSucceed);

    // 注意: 由于取角色数据是异步的,所以,有可能回包到达时已经是玩家断线后的下一次登陆了
    KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForRoleData);

    pScene = g_pSO3World->GetScene(pPlayer->m_SavePosition.dwMapID, pPlayer->m_SavePosition.nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    pPlayer->m_nFaceDirection   = pPlayer->m_SavePosition.nFaceDirection;

    bRetCode = g_pSO3World->AddPlayer(
        pPlayer, pScene,
        pPlayer->m_SavePosition.nX,
        pPlayer->m_SavePosition.nY,
        pPlayer->m_SavePosition.nZ
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = pPlayer->Load(m_pbySyncRoleBuffer, pNotify->uRoleDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

	g_PlayerServer.DoSyncRoleDataOver(pPlayer->m_nConnIndex);
    
    pScene->CallEnterSceneScript(pPlayer);

    bRetCode = pPlayer->CallLoginScript();
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_nVirtualFrame    = g_pSO3World->m_nGameLoop;
    pPlayer->m_nRecordCount     = 0;
    pPlayer->m_eGameStatus      = gsPlaying;

    bResult = true;
Exit0:
    m_uSyncRoleOffset = 0;
    m_dwSyncRoleID    = ERROR_ID;

    // 这里失败不调用detach是因为玩家可能是第二次登录,玩家状态是waitforconnect,调用detach会宕机
    return;
}

void KRelayClient::OnGmCommand(BYTE* pbyData, size_t uDataLen)
{
    BOOL            bRetCode        = false;
    R2S_GM_COMMAND* pGmCommand      = (R2S_GM_COMMAND*)pbyData;
    KPlayer*        pPlayer         = NULL;

    KGLOG_PROCESS_ERROR(uDataLen > sizeof(R2S_GM_COMMAND));
    pGmCommand->szGmCommand[uDataLen - sizeof(R2S_GM_COMMAND) - 1] = '\0';
    pGmCommand->szGmName[sizeof(pGmCommand->szGmName) - 1] = '\0';

    if (pGmCommand->dwPlayerID)
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(pGmCommand->dwPlayerID);

    KGLogPrintf(
        KGLOG_INFO, "GMC[%s, %u]: %s\n", 
        pGmCommand->szGmName, pGmCommand->dwPlayerID, pGmCommand->szGmCommand
    );

    bRetCode = g_pSO3World->m_ScriptCenter.ExecuteGMCScript(pPlayer, pGmCommand->szGmName, pGmCommand->szGmCommand); // pPlayer may be NULL!
    if (!bRetCode)
    {
        g_RelayClient.DoSendGmMessage(pGmCommand->szGmName, "SCRIPT_ERROR");        
        goto Exit0;
    }

Exit0:
    return;
}

void KRelayClient::OnBroadcastSyncPQ(BYTE *pbyData, size_t uDataLen)
{
    BOOL                bRetCode    = false;
    R2S_SYNC_PQ*        pSyncPQ     = (R2S_SYNC_PQ*)pbyData;
    KGPQTemplateInfo*   pPQTemplate = g_pSO3World->m_PQManager.GetPQTemplate(pSyncPQ->dwPQTemplateID);
    KGPQ                PQ;

    KGLOG_PROCESS_ERROR(pPQTemplate);

    PQ.m_dwPQID         = pSyncPQ->dwPQID;
    PQ.m_pPQTemplate    = pPQTemplate;

    assert(sizeof(PQ.m_nPQValue) == sizeof(pSyncPQ->nPQValue));
    memcpy(PQ.m_nPQValue, pSyncPQ->nPQValue, sizeof(PQ.m_nPQValue));

    PQ.m_nBeginTime     = pSyncPQ->nBeginTime;
    PQ.m_bFinished      = pSyncPQ->bFinished;
    PQ.m_eOwnerType     = (KGPQ_OWNER_TYPE)pSyncPQ->byOnwerType;
    PQ.m_dwOwnerID1     = pSyncPQ->dwOnwerID1;
    PQ.m_dwOwnerID2     = pSyncPQ->dwOnwerID2;
    PQ.m_dwOwnerID3     = pSyncPQ->dwOnwerID3;

    bRetCode = g_pSO3World->m_PQManager.LoadPQ(PQ);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (PQ.m_eOwnerType == pqotScene || PQ.m_eOwnerType == pqotBattleField)
    {
        KScene* pScene          = NULL;
        DWORD   dwMapID         = PQ.m_dwOwnerID1;
        int     nMapCopyIndex   = PQ.m_dwOwnerID2;

        pScene = g_pSO3World->GetScene(dwMapID, nMapCopyIndex);
        if (pScene)
            pScene->m_PQIDFlagVector.push_back(std::make_pair(pSyncPQ->dwPQID, true));
    }

Exit0:
    return;
}

struct KREMOVE_PQID_FALG  
{
    inline bool operator () (std::pair<DWORD, BOOL>& rPair)
    {
        return rPair.first == PQID;
    };

    DWORD   PQID;
};

void KRelayClient::OnBroadcastDeletePQ(BYTE *pbyData, size_t uDataLen)
{
    BOOL            bRetCode    = false;
    R2S_DELETE_PQ*  pDeletePQ   = (R2S_DELETE_PQ*)pbyData;
    KGPQ*           pPQ         = g_pSO3World->m_PQManager.GetPQ(pDeletePQ->dwPQID);

    KG_PROCESS_ERROR(pPQ);

    if (pPQ->m_eOwnerType == pqotScene || pPQ->m_eOwnerType == pqotBattleField)
    {
        KScene* pScene          = NULL;
        DWORD   dwMapID         = pPQ->m_dwOwnerID1;
        int     nMapCopyIndex   = pPQ->m_dwOwnerID2;
        KREMOVE_PQID_FALG   Remover;

        Remover.PQID = pDeletePQ->dwPQID;

        pScene = g_pSO3World->GetScene(dwMapID, nMapCopyIndex);
        if (pScene)
            pScene->m_PQIDFlagVector.erase(
                std::remove_if(pScene->m_PQIDFlagVector.begin(), pScene->m_PQIDFlagVector.end(), Remover),
                pScene->m_PQIDFlagVector.end()
            );
    }

    bRetCode = g_pSO3World->m_PQManager.DeletePQ(pDeletePQ->dwPQID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

struct KBROADCAST_BATTLEFIELD_OBJECTIVE 
{
    KBROADCAST_BATTLEFIELD_OBJECTIVE(DWORD dwPQID, int nIndex, int nCurrentValue, int nTargetValue)
    {
        m_dwPQID        = dwPQID;
        m_nIndex        = nIndex;
        m_nCurrentValue = nCurrentValue;
        m_nTargetValue  = nTargetValue;
    };

    BOOL operator () (KPlayer* pPlayer)
    {
        KGPQList::PQ_VECTOR::iterator it;

        if (!pPlayer)
            return true;
         
        it = std::find(pPlayer->m_PQList.m_PQVector.begin(), pPlayer->m_PQList.m_PQVector.end(), m_dwPQID);
        if (it == pPlayer->m_PQList.m_PQVector.end())
            return true;

        g_PlayerServer.DoSyncBattleFieldObjective(pPlayer->m_nConnIndex, m_nIndex, m_nCurrentValue, m_nTargetValue);

        return true;
    };

    DWORD   m_dwPQID;
    int     m_nIndex;
    int     m_nCurrentValue;
    int     m_nTargetValue;
};

void KRelayClient::OnBroadcastPQValueChange(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bRetCode        = false;
    R2S_PQ_VALUE_CHANGE*    pPQValueChange  = (R2S_PQ_VALUE_CHANGE*)pbyData;
    DWORD                   dwPQID          = pPQValueChange->dwPQID;
    int                     nIndex          = pPQValueChange->nIndex;
    KGPQ*                   pPQ             = NULL;

    pPQ = g_pSO3World->m_PQManager.GetPQ(dwPQID);
    KGLOG_PROCESS_ERROR(pPQ);

    bRetCode = pPQ->SetValue(nIndex, pPQValueChange->nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pPQ->m_eOwnerType == pqotBattleField)
    {
        KScene* pScene = g_pSO3World->GetScene(pPQ->m_dwOwnerID1, pPQ->m_dwOwnerID2);
        KBROADCAST_BATTLEFIELD_OBJECTIVE BroadcastBattlefieldObjective(dwPQID, nIndex, pPQ->m_nPQValue[nIndex], pPQ->m_pPQTemplate->nPQValue[nIndex]);
        if (pScene)
            pScene->TraversePlayer(BroadcastBattlefieldObjective);
    }

Exit0:
    return;
}

void KRelayClient::OnBroadcastPQFinish(BYTE* pbyData, size_t uDataLen)
{
    BOOL            bRetCode    = false;
    R2S_PQ_FINISH*  pPQFinish   = (R2S_PQ_FINISH*)pbyData;
    KGPQ*           pPQ         = NULL;

    pPQ = g_pSO3World->m_PQManager.GetPQ(pPQFinish->dwPQID);
    KGLOG_PROCESS_ERROR(pPQ);

    bRetCode = pPQ->Finish();
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KRelayClient::OnJoinBattleFieldQueueRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND* pJoinRespond = (R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND*)pbyData;

    g_PlayerServer.DoJoinBattleFieldRespond(pJoinRespond->dwPlayerID, pJoinRespond->dwMapID, pJoinRespond->nErrorCode);
}

void KRelayClient::OnLeaveBattleFieldQueueRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND* pLeaveRespond = (R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND*)pbyData;

    g_PlayerServer.DoLeaveBattleFieldRespond(pLeaveRespond->dwPlayerID, pLeaveRespond->dwMapID, pLeaveRespond->nErrorCode);
}

void KRelayClient::OnBattleFieldNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_BATTLE_FIELD_NOTIFY* pNotify = (R2S_BATTLE_FIELD_NOTIFY*)pbyData;

    g_PlayerServer.DoBattleFieldNotify(
        pNotify->dwPlayerID, pNotify->nAvgQueueTime, pNotify->nPassTime,
        pNotify->dwMapID, pNotify->nCopyIndex, pNotify->eType
    );
}

void KRelayClient::OnSyncBattleFieldList(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_BATTLE_FIELD_LIST* pBattleFieldList = (R2S_SYNC_BATTLE_FIELD_LIST*)pbyData;

    g_PlayerServer.DoSyncBattleFieldList(
        pBattleFieldList->dwPlayerID, pBattleFieldList->dwMapID, pBattleFieldList->nCopyIndex
    );
}

void KRelayClient::OnAcceptJoinBattleFieldRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND*   pAcceptJoin = (R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND*)pbyData;
    KPlayer*                                pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pAcceptJoin->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pPlayer->m_eMoveState == cmsOnAutoFly) // 如果正在交通，则停止
    {
        pPlayer->Stop();
    }

    pPlayer->m_LastEntry.dwMapID        = pAcceptJoin->dwLeaveMapID;
    pPlayer->m_LastEntry.nMapCopyIndex  = pAcceptJoin->nLeaveCopyIndex;
    pPlayer->m_LastEntry.nX             = pAcceptJoin->nLeaveX;
    pPlayer->m_LastEntry.nY             = pAcceptJoin->nLeaveY;
    pPlayer->m_LastEntry.nZ             = pAcceptJoin->nLeaveZ;
    pPlayer->m_LastEntry.nFaceDirection = pPlayer->m_nFaceDirection;

    pPlayer->SwitchMap(
        pAcceptJoin->dwEnterMapID, pAcceptJoin->nEnterCopyIndex,
        pAcceptJoin->nEnterX, pAcceptJoin->nEnterY, pAcceptJoin->nEnterZ
    );
Exit0:
    return;
}

void KRelayClient::OnLeaveBattleFieldRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_LEAVE_BATTLE_FIELD_RESPOND* pLeaveBattleField   = (R2S_LEAVE_BATTLE_FIELD_RESPOND*)pbyData;
    KPlayer*                        pPlayer             = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pLeaveBattleField->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->SwitchMap(
        pPlayer->m_LastEntry.dwMapID, pPlayer->m_LastEntry.nMapCopyIndex,
        pPlayer->m_LastEntry.nX, pPlayer->m_LastEntry.nY, pPlayer->m_LastEntry.nZ
    );

Exit0:
    return;
}

void KRelayClient::OnSyncCampInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bRetCode      = false;
    R2S_SYNC_CAMP_INFO* pSyncCampInfo = (R2S_SYNC_CAMP_INFO*)pbyData;

    g_pSO3World->m_CampInfo.SetCampLevel(pSyncCampInfo->nCampLevel);
    g_pSO3World->m_CampInfo.SetCampScore(pSyncCampInfo->nCampScore);

    memcpy(
        g_pSO3World->m_CampInfo.m_nActivePlayerCount, 
        pSyncCampInfo->nActivePlayerCount, 
        sizeof(g_pSO3World->m_CampInfo.m_nActivePlayerCount)
    );

    g_pSO3World->m_CampInfo.SetVersion(g_pSO3World->m_nGameLoop);

Exit0:
    return;
}

void KRelayClient::OnApplySetCampRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode    = false;
    KPlayer*                    pPlayer     = NULL;
    R2S_APPLY_SET_CAMP_RESPOND* pRespond    = (R2S_APPLY_SET_CAMP_RESPOND*)pbyData;
        
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    if ((CAMP_RESULT_CODE)pRespond->byResult == crtSuccess)
    {
        pPlayer->SetCamp((KCAMP)pRespond->byCamp);
    }
    
    g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectCampResultCode, pRespond->byResult);

Exit0:
    return;
}

void KRelayClient::OnSyncTongMemberInfo(BYTE* pbyData, size_t uDataLen)
{   
    R2S_SYNC_TONG_MEMBER_INFO* pSyncRespond = (R2S_SYNC_TONG_MEMBER_INFO*)pbyData;

    g_PlayerServer.DoSyncTongMemberInfo(pSyncRespond->dwPlayerID, pSyncRespond->MemberInfo);
}

void KRelayClient::OnDeleteTongMemberNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_DELETE_TONG_MEMBER_NOTIFY* pPackage = (R2S_DELETE_TONG_MEMBER_NOTIFY*)pbyData;

    g_PlayerServer.DoTongDeleteMemberNotify(pPackage->dwPlayerID, pPackage->dwMemberID);
}

void KRelayClient::OnApplyTongInfoRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_APPLY_TONG_INFO_RESPOND* pSyncRespond = (R2S_APPLY_TONG_INFO_RESPOND*)pbyData;

    g_PlayerServer.DoApplyTongInfoRespond(
        pSyncRespond->dwPlayerID, pSyncRespond->byInfoType, 
        pSyncRespond->byData, uDataLen - sizeof(R2S_APPLY_TONG_INFO_RESPOND)
    );
}

void KRelayClient::OnUpdateTongClientDataVersion(BYTE* pbyData, size_t uDataLen)
{
    R2S_UPDATE_TONG_CLIENT_DATA_VERSION* pUpdateTongData = (R2S_UPDATE_TONG_CLIENT_DATA_VERSION*)pbyData;

    g_PlayerServer.DoUpdateTongClientDataVersion(
        pUpdateTongData->dwPlayerID, (KTONG_UPDATE_FLAG)pUpdateTongData->byFrameType,
        pUpdateTongData->nUpdateFrame
    );
}

void KRelayClient::OnSyncTongRepertoryPageRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_TONG_REPERTORY_PAGE_RESPOND* pRespond = (R2S_SYNC_TONG_REPERTORY_PAGE_RESPOND*)pbyData;

    g_PlayerServer.DoSyncTongRepertoryPageRespond(
        pRespond->dwPlayerID, pRespond->byPageIndex, pRespond->nServerCounter,
        pRespond->byGetRight, pRespond->byPutRight,
        pRespond->byUpdateItemCount, pRespond->byInvailedItemCount,
        uDataLen - sizeof(R2S_SYNC_TONG_REPERTORY_PAGE_RESPOND), pRespond->byData
    );
}

void KRelayClient::OnInvitePlayerJoinTongRequest(BYTE* pbyData, size_t uDataLen)
{
    R2S_INVITE_PLAYER_JOIN_TONG_REQUEST* pRequest = (R2S_INVITE_PLAYER_JOIN_TONG_REQUEST*)pbyData;

    g_PlayerServer.DoInvitePlayerJoinTongRequest(
        pRequest->dwPlayerID, pRequest->dwInviterID, pRequest->dwTongID, pRequest->szInviterName, pRequest->szTongName
    ); 
}

void KRelayClient::OnChangePlayerTongNotify(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                        pPlayer = NULL;
    R2S_CHANGE_PLAYER_TONG_NOTIFY*  pNotify = (R2S_CHANGE_PLAYER_TONG_NOTIFY*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_dwTongID = pNotify->dwTongID;

    g_PlayerServer.DoChangePlayerTongNotify(pPlayer->m_nConnIndex, pNotify->dwTongID, pNotify->byReason, pNotify->szTongName);

    g_PlayerServer.DoBroadcastPlayerChangeTong(pPlayer);

Exit0:
    return;
}

void KRelayClient::OnBroadcastTongMessage(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                    pPlayer     = NULL;
    R2S_TONG_BROADCAST_MESSAGE* pMessage    = (R2S_TONG_BROADCAST_MESSAGE*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pMessage->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pMessage->byCode > tbmInvalid && pMessage->byCode < tbmTotal);

    g_PlayerServer.DoBroadcastTongMessage(
        pPlayer->m_nConnIndex, pMessage->byCode, pMessage->byData, uDataLen - sizeof(R2S_TONG_BROADCAST_MESSAGE)
    );

Exit0:
    return;
}

void KRelayClient::OnGetTongDescriptionRespond(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                            pPlayer     = NULL;
    R2S_GET_TONG_DESCRIPTION_RESPOND*   pRespond    = (R2S_GET_TONG_DESCRIPTION_RESPOND*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    
    pRespond->szTongName[sizeof(pRespond->szTongName) - 1] = '\0';

    g_PlayerServer.DoGetTongDescriptionRespond(pPlayer->m_nConnIndex, pRespond->dwTongID, pRespond->szTongName);
Exit0:
    return;
}

void KRelayClient::OnSyncTongOnlineMessage(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                        pPlayer = NULL;
    R2S_SYNC_TONG_ONLINE_MESSAGE*   pSync   = (R2S_SYNC_TONG_ONLINE_MESSAGE*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(R2S_SYNC_TONG_ONLINE_MESSAGE) + (size_t)pSync->byDataLen);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSync->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoSyncTongOnlineMessage(pPlayer->m_nConnIndex, pSync->szOnlineMessage, (size_t)pSync->byDataLen);
Exit0:
    return;
}

void KRelayClient::OnTakeTongRepertoryItemToPosRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                            bResult         = false;
    BOOL                                            bRetCode        = false;
    int                                             nRetCode        = 0;
    KPlayer*                                        pPlayer         = NULL;
    ADD_ITEM_RESULT_CODE                            eRetCode        = aircFailed;
    R2S_TAKE_TONG_REPERTORY_ITEM_TO_POS_RESPOND*    pRespond        = (R2S_TAKE_TONG_REPERTORY_ITEM_TO_POS_RESPOND*)pbyData;
    KItem*                                          pItem           = NULL;
    KItemList*                                      pItemList       = NULL;
    int                                             nBoxSize        = 0;
    KITEM_LOG_INFO                                  ItemLogInfo;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(R2S_TAKE_TONG_REPERTORY_ITEM_TO_POS_RESPOND) + pRespond->bySize);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    pItemList = &pPlayer->m_ItemList;

    KGLOG_PROCESS_ERROR(pRespond->byBoxIndex >= ibPackage && pRespond->byBoxIndex <= ibPackage4);
    nBoxSize = pItemList->GetBoxSize(pRespond->byBoxIndex);
    KGLOG_PROCESS_ERROR(pRespond->byBoxPos < nBoxSize);

    pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pRespond->byData, (size_t)pRespond->bySize);
    KGLOG_PROCESS_ERROR(pItem);

    g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);
    
    eRetCode = pItemList->AddItemToPos(pItem, pRespond->byBoxIndex, pRespond->byBoxPos);
    KG_PROCESS_ERROR(eRetCode == aircSuccess);
    pItem = NULL;

    g_LogClient.LogPlayerItemChange(pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "get from tong");

    bResult = true;
Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }

    DoTakeTongRepertoryItemRespond(pRespond->dwPlayerID, pRespond->dwTongID, pRespond->byPageIndex, pRespond->byRepertoryPos, bResult);
    return;
}


void KRelayClient::OnTakeTongRepertoryItemRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                    bResult     = false;
    KPlayer*                                pPlayer     = NULL;
    KItem*                                  pItem       = NULL;
    ADD_ITEM_RESULT_CODE                    eRetCode    = aircFailed;
    R2S_TAKE_TONG_REPERTORY_ITEM_RESPOND*   pRespond    = (R2S_TAKE_TONG_REPERTORY_ITEM_RESPOND*)pbyData;
    KITEM_LOG_INFO                          ItemLogInfo;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(R2S_TAKE_TONG_REPERTORY_ITEM_RESPOND) + pRespond->bySize);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pRespond->byData, (size_t)pRespond->bySize);
    KGLOG_PROCESS_ERROR(pItem);
    
    g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);

    eRetCode = pPlayer->m_ItemList.AddItem(pItem);
    KG_PROCESS_ERROR(eRetCode == aircSuccess);
    g_LogClient.LogPlayerItemChange(pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "get from tong");
    pItem = NULL;

    bResult = true;
Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }

    DoTakeTongRepertoryItemRespond(pRespond->dwPlayerID, pRespond->dwTongID, pRespond->byPageIndex, pRespond->byRepertoryPos, bResult);
    return;
}

void KRelayClient::OnPutTongRepertoryItemRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                    bRetCode        = false;
    BOOL                                    bSuccess        = false;
    KPlayer*                                pPlayer         = NULL;
    KItem*                                  pItem           = NULL;
    size_t                                  uItemDataLen    = 0;
    R2S_PUT_TONG_REPERTORY_ITEM_RESPOND*    pRespond        = (R2S_PUT_TONG_REPERTORY_ITEM_RESPOND*)pbyData;
    KItemList*                              pItemList       = NULL;
    int                                     nBoxSize        = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    pItemList = &pPlayer->m_ItemList;

    KGLOG_PROCESS_ERROR(pRespond->byBoxIndex >= ibPackage  && pRespond->byBoxIndex <= ibPackage4);

    nBoxSize = pItemList->GetBoxSize(pRespond->byBoxIndex);
    KGLOG_PROCESS_ERROR(pRespond->byBoxPos < nBoxSize);
    pItem = pItemList->GetItem(pRespond->byBoxIndex, pRespond->byBoxPos);
    KG_PROCESS_ERROR(pItem);

    KG_PROCESS_ERROR(pItem->m_dwID == pRespond->dwServerItemID);
    bRetCode = pItem->IsBind();
    KGLOG_PROCESS_ERROR(!bRetCode);

    bRetCode = DoPutTongRepertoryItemRespond(pItem, pRespond->dwPlayerID, pRespond->dwTongID, pRespond->byPageIndex, pRespond->byPagePos);
    KGLOG_PROCESS_ERROR(bRetCode);

    pItem = pItemList->PickUpItem(pRespond->byBoxIndex, pRespond->byBoxPos, true);
    assert(pItem);

    g_LogClient.LogPlayerItemChange(pPlayer, ITEM_OPERATION_TYPE_OUT, pItem, "put to tong");
    
    g_pSO3World->m_ItemManager.FreeItem(pItem);
    pItem = NULL;

    bSuccess = true;
Exit0:
    if (!bSuccess)
    {
        DoUnlockTongRepertoryGrid(pRespond->dwTongID, pRespond->byPageIndex, pRespond->byPagePos);
    }
    return;
}

void KRelayClient::OnApplyStackTongRepertoryItemRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                            bRetCode    = false;
    BOOL                                            bSuccess    = false;
    int                                             nRetCode    = 0;
    KPlayer*                                        pPlayer     = NULL;
    R2S_APPLY_STACK_TONG_REPERTORY_ITEM_RESPOND*    pRespond    = (R2S_APPLY_STACK_TONG_REPERTORY_ITEM_RESPOND*)pbyData;
    KItemList*                                      pItemList   = NULL;
    KItem*                                          pItem       = NULL;
    DWORD                                           dwTabType   = 0;
    DWORD                                           dwTabIndex  = 0;
    int                                             nBoxSize    = 0;
    int                                             nStackNum   = 0;
    int                                             nMaxStackNum = 0;

    assert(pRespond);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pRespond->dwTongID == pPlayer->m_dwTongID);

    KGLOG_PROCESS_ERROR(pRespond->byBoxIndex >= ibPackage  && pRespond->byBoxIndex <= ibPackage4);

    pItemList = &pPlayer->m_ItemList;
    assert(pItemList);

    nBoxSize = pItemList->GetBoxSize(pRespond->byBoxIndex);
    KGLOG_PROCESS_ERROR(pRespond->byBoxPos < nBoxSize);

    pItem = pItemList->GetItem(pRespond->byBoxIndex, pRespond->byBoxPos);
    KG_PROCESS_ERROR(pItem);
    KG_PROCESS_ERROR(pItem->m_dwID == pRespond->dwItemID);    
    KG_PROCESS_ERROR(pRespond->wStackNum <= pItem->m_nStackNum);
    nMaxStackNum = pItem->GetMaxStackNum();

    dwTabType = pItem->m_GenParam.dwTabType;
    dwTabIndex = pItem->m_GenParam.dwIndex;
    nStackNum = pItem->GetStackNum();
    nStackNum -= pRespond->wStackNum;

    bRetCode = pItem->SetStackNum(nStackNum);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pItem->m_nStackNum == 0)
    {
        nRetCode = pItemList->DestroyItem(pItem, true);
        KGLOG_CHECK_ERROR(nRetCode == ircSuccess);
        pItem = NULL;
    }
    else
    {
        bRetCode = g_PlayerServer.DoUpdateItemAmount(
            pPlayer->m_nConnIndex, pRespond->byBoxIndex,
            pRespond->byBoxPos, nStackNum
        );
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bRetCode = DoStackTongRepertoryItemRequest(
        pRespond->dwPlayerID, pRespond->dwTongID,
        pRespond->byPageIndex, pRespond->byPagePos, 
        (BYTE)dwTabType, (WORD)dwTabIndex,
        pRespond->wStackNum, (WORD)nMaxStackNum
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bSuccess = true;
Exit0:
    if(!bSuccess)
    {
        DoUnlockTongRepertoryGrid(pRespond->dwTongID, pRespond->byPageIndex, pRespond->byPagePos);
    }
    return;
}

void KRelayClient::OnApplyOpenTongRepertoryRespond(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                                pPlayer     = NULL;
    R2S_APPLY_OPEN_TONG_REPERTPRY_RESPOND*  pRespond    = (R2S_APPLY_OPEN_TONG_REPERTPRY_RESPOND*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoApplyOpenTongRepertoryRespond(
        pPlayer->m_nConnIndex, pRespond->dwPlayerID, pRespond->szTongName, pRespond->byRepertoryPageNum
    );
Exit0:
    return;
}

void KRelayClient::OnGetTongSalaryRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KPlayer*                        pPlayer     = NULL;
    int                             nMoney      = 0;
    R2S_GET_TONG_SALARY_RESPOND*    pRespond    = (R2S_GET_TONG_SALARY_RESPOND*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pRespond->nSilver >= 0);
    KGLOG_PROCESS_ERROR(INT_MAX / MONEY_CONVERSION_SILVER_RATE >= pRespond->nSilver);

    nMoney = pRespond->nSilver * MONEY_CONVERSION_SILVER_RATE;
    
    bRetCode = pPlayer->m_ItemList.AddMoney(nMoney);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_LogClient.LogPlayerMoneyChange(nMoney, pPlayer, "get tong salary");

    bResult = true;
Exit0:
    if (!bResult)
    {
        DoGetTongSalaryFailRespond(pRespond->dwPlayerID, pRespond->dwTongID, pRespond->nSilver);
    }
    return;
}

void KRelayClient::OnSyncTongHistoryRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KPlayer*                        pPlayer     = NULL;
    R2S_SYNC_TONG_HISTORY_RESPOND*  pRespond    = (R2S_SYNC_TONG_HISTORY_RESPOND*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pRespond->byCount > 0);
    KGLOG_PROCESS_ERROR(uDataLen > sizeof(R2S_SYNC_TONG_HISTORY_RESPOND));

    g_PlayerServer.DoSyncTongHistoryRespond(
        pPlayer->m_nConnIndex, pRespond->byType, pRespond->dwStartIndex, (int)pRespond->byCount, 
        pRespond->byData, uDataLen - sizeof(R2S_SYNC_TONG_HISTORY_RESPOND)
    );
Exit0:
    return;
}

void KRelayClient::OnAuctionLookupRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_AUCTION_LOOKUP_RESPOND* pRespond = (R2S_AUCTION_LOOKUP_RESPOND*)pbyData;

    g_PlayerServer.DoAuctionLookupRespond(pRespond->dwPlayerID, pRespond->byRespondID, pRespond->byCode, &pRespond->Page);
}

void KRelayClient::OnAuctionBidRespond(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                    pPlayer     = NULL;
    R2S_AUCTION_BID_RESPOND*    pRespond    = (R2S_AUCTION_BID_RESPOND*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pRespond->byCode != arcSucceed)
    {
        pPlayer->m_ItemList.AddMoney(pRespond->nBidPrice, false);

        g_LogClient.LogPlayerMoneyChange(
            pRespond->nBidPrice, pPlayer, "buy item from auction failed"
        );
    }

    g_PlayerServer.DoAuctionBidRespond(pRespond->dwPlayerID, pRespond->byCode);

Exit0:
    return;
}

void KRelayClient::OnAuctionSellRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode        = false;
    R2S_AUCTION_SELL_RESPOND*   pRespond        = (R2S_AUCTION_SELL_RESPOND*)pbyData;
    KPlayer*                    pPlayer         = NULL;
    KItem*                      pItem           = NULL;
    ADD_ITEM_RESULT_CODE        eAddItemCode    = aircInvalid;
    
    g_PlayerServer.DoAuctionSellRespond(pRespond->dwPlayerID, pRespond->byCode);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pRespond->byItemData, pRespond->uItemDataLen);
    KGLOG_PROCESS_ERROR(pItem);

    if (pRespond->byCode == arcSucceed)
    {
        char szComment[128];

        snprintf(
            szComment, sizeof(szComment), "put to auction, (StartPrice:%d), (BuyItNowPrice:%d)", 
            pRespond->nStartPrice, pRespond->nBuyItNowPrice
        );
        szComment[sizeof(szComment) - 1] = '\0';

        g_LogClient.LogPlayerItemChange(pPlayer, ITEM_OPERATION_TYPE_OUT, pItem, szComment);
    }
    else
    {
        // 出售失败 加回钱和道具
        bRetCode = pPlayer->m_ItemList.AddMoney(pRespond->nCustodyCharges, false);
        KGLOG_PROCESS_ERROR(bRetCode);

        g_LogClient.LogPlayerMoneyChange(pRespond->nCustodyCharges, pPlayer, "auction sell failed");

        eAddItemCode = pPlayer->m_ItemList.AddItem(pItem, false);
        if (eAddItemCode != aircSuccess)
        {
            KGLogPrintf(
                KGLOG_DEBUG, "[Auction] Add item failed! PlayerName = %s, TabIndex = %lu, Index = %lu, RetCode = %d",
                pPlayer->m_szName, pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, (int)eAddItemCode
            );
            goto Exit0;
        }
        pItem = NULL;
    }

Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return;
}

void KRelayClient::OnAuctionCancelRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_AUCTION_CANCEL_RESPOND* pRespond = (R2S_AUCTION_CANCEL_RESPOND*)pbyData;

    g_PlayerServer.DoAuctionCancelRespond(pRespond->dwPlayerID, pRespond->byCode);
}

void KRelayClient::OnAuctionMessageNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_AUCTION_MESSAGE_NOTIFY* pNotify = (R2S_AUCTION_MESSAGE_NOTIFY*)pbyData;

    g_PlayerServer.DoAuctionMessageNotify(pNotify->dwPlayerID, pNotify->byCode, pNotify->szSaleName, pNotify->nPrice);
}

void KRelayClient::OnRemoteLuaCall(BYTE* pbyData, size_t uDataLen)
{
    R2S_REMOTE_LUA_CALL* pLuaCall = (R2S_REMOTE_LUA_CALL*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(R2S_REMOTE_LUA_CALL) + pLuaCall->uParamLen);

    pLuaCall->szFunction[sizeof(pLuaCall->szFunction) - 1] = '\0';

    g_pSO3World->m_CenterRemote.Accept(
        pLuaCall->szFunction, pLuaCall->byParam, pLuaCall->uParamLen
    );

Exit0:
    return;
}

void KRelayClient::OnAchievementGlobalAnnounceRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND* pRespond = (R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND*)pbyData;

    g_PlayerServer.BroadcastAchievementAnnounce(ERROR_ID, aatGlobal, pRespond->szRoleName, pRespond->nAchievementID);
}

void KRelayClient::OnDesignationGlobalAnnounceRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_DESIGNATION_GLOBAL_ANNOUNCE_RESPOND* pRespond = (R2S_DESIGNATION_GLOBAL_ANNOUNCE_RESPOND*)pbyData;
    
    g_PlayerServer.BroadcastDesignationAnnounce(ERROR_ID, pRespond->szRoleName, pRespond->nPrefix, pRespond->nPostfix, datGlobal);
}

void KRelayClient::OnSyncGlobalSystemValue(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode    = false;
    R2S_SYNC_GLOBAL_SYSTEM_VALUE*   pPak        = (R2S_SYNC_GLOBAL_SYSTEM_VALUE*)pbyData;

    g_pSO3World->m_GlobalSystemValueCache.m_nGameWorldStartTime     = pPak->nGameWorldStartTime;
    g_pSO3World->m_GlobalSystemValueCache.m_nMaxPlayerLevel         = pPak->nMaxPlayerLevel;
    g_pSO3World->m_GlobalSystemValueCache.m_nGameWorldChargeTime    = pPak->nGameWorldChargeTime;

    return;
}

void KRelayClient::OnQueryStatIDRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_QUERY_STAT_ID_RESPOND* pRespond = (R2S_QUERY_STAT_ID_RESPOND*)pbyData;

    g_pSO3World->m_StatDataServer.SetNameID(pRespond->szName, pRespond->nID);
}

void KRelayClient::OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode    = false;
    R2S_CHANGE_EXT_POINT_RESPOND*   pRespond    = (R2S_CHANGE_EXT_POINT_RESPOND*)pbyData;
    KPlayer*                        pPlayer     = NULL;
    int                             nTopIndex   = 0;
    const char*                     pszFuncName = "OnSetExtPointSucceed";

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLogPrintf(
        KGLOG_INFO, "Change ExtPoint %s: Acc = %s, Role = %s, ID = %u",
        pRespond->bSucceed ? "succeed" : "failed", 
        pPlayer->m_szAccount, pPlayer->m_szName, pPlayer->m_dwID
    );

    pPlayer->m_bExtPointLock = false;

    if (!pRespond->bSucceed)
    {
        pPlayer->m_ExtPointInfo.nExtPoint[pPlayer->m_nLastExtPointIndex] = pPlayer->m_nLastExtPointValue;
        goto Exit0;
    }

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PLAYER_EXT_POINT_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PLAYER_EXT_POINT_SCRIPT, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer->m_nLastExtPointIndex);
    g_pSO3World->m_ScriptCenter.PushValueToStack((int)(pPlayer->m_nLastExtPointValue));
    g_pSO3World->m_ScriptCenter.CallFunction(PLAYER_EXT_POINT_SCRIPT, pszFuncName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return;
}

void KRelayClient::OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_SET_CHARGE_FLAG_RESPOND*    pRespond    = (R2S_SET_CHARGE_FLAG_RESPOND*)pbyData;
    KPlayer*                        pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->m_bChargeFlag      = (BOOL)pRespond->nChargeFlag;
    pPlayer->m_ExtPointInfo     = pRespond->ExtPointInfo;
    pPlayer->m_nEndTimeOfFee    = (time_t)pRespond->dwEndTimeOfFee;

Exit0:
    return;
}

void KRelayClient::OnSyncZoneChargeFlag(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_ZONE_CHARGE_FLAG* pPak = (R2S_SYNC_ZONE_CHARGE_FLAG*)pbyData;

    g_pSO3World->m_bZoneChargeFlag = (BOOL)pPak->nZoneChargeFlag;
}

void KRelayClient::OnActivePresentCodeRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode    = false;
    R2S_ACTIVE_PRESENT_CODE_RESPOND*    pRespond    = (R2S_ACTIVE_PRESENT_CODE_RESPOND*)pbyData;
    KPlayer*                            pPlayer     = NULL;
    int                                 nTopIndex   = 0;
    const char*                         pszFuncName = "OnActivePresentCodeSucceed";

    pRespond->szPresentCode[sizeof(pRespond->szPresentCode) - 1] = '\0';

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLogPrintf(
        KGLOG_INFO, "Active PresentCode %s: Acc = %s, Role = %s, ID = %u, PresentCode = %s",
        pRespond->bSucceed ? "succeed" : "failed", 
        pPlayer->m_szAccount, pPlayer->m_szName, pPlayer->m_dwID, pRespond->szPresentCode
    );

    KG_PROCESS_ERROR(pRespond->bSucceed);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PLAYER_PRESENT_CODE_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PLAYER_PRESENT_CODE_SCRIPT, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(pRespond->dwPresentType);
    g_pSO3World->m_ScriptCenter.CallFunction(PLAYER_PRESENT_CODE_SCRIPT, pszFuncName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return;
}

void KRelayClient::OnApexProtocol(BYTE* pbyData, size_t uDataLen)
{
    R2S_APEX_PROTOCOL* pApexProtocol = (R2S_APEX_PROTOCOL*)pbyData;

    g_PlayerServer.DoApexProtocol(pApexProtocol->dwRoleID, pApexProtocol->byData, pApexProtocol->nSize);
}

void KRelayClient::OnSetFarmerLimitFlagRequest(BYTE* pbyData, size_t uDataLen)
{
    R2S_SET_FARMER_LIMIT_FLAG_REQUEST*  pRequest    = (R2S_SET_FARMER_LIMIT_FLAG_REQUEST*)pbyData;
    KPlayer*                            pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRequest->dwRoleID);
    KG_PROCESS_ERROR(pPlayer);

    pPlayer->m_bFarmerLimit = pRequest->bLimit;

Exit0:
    return;
}

void KRelayClient::OnGameCardSellRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_GAME_CARD_SELL_RESPOND* pRespond = (R2S_GAME_CARD_SELL_RESPOND*)pbyData;
    KPlayer*                    pPlayer  = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pRespond->byCode != gorSucceed)
    {
        pPlayer->AddCoin(pRespond->nCoin);
    }

    g_PlayerServer.DoGameCardSellRespond(pPlayer, pRespond->byCode);

Exit0:
    return;
}

void KRelayClient::OnGameCardBuyRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_GAME_CARD_BUY_RESPOND*  pRespond = (R2S_GAME_CARD_BUY_RESPOND*)pbyData;
    KPlayer*                    pPlayer  = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pRespond->byCode == gorSucceed)
    {
        if (!pPlayer->m_bChargeFlag)
        {
            DoSetChargeFlagRequest(pPlayer, true);
        }
    }
    else
    {
        pPlayer->m_ItemList.AddMoney(pRespond->nBuyPrice, false);

        g_LogClient.LogPlayerMoneyChange(pRespond->nBuyPrice, pPlayer, "buy gamecard failed");
    }

    g_PlayerServer.DoGameCardBuyRespond(pPlayer, pRespond->byCode);

Exit0:
    return;
}

void KRelayClient::OnGameCardLookupRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_GAME_CARD_LOOKUP_RESPOND*   pRespond    = (R2S_GAME_CARD_LOOKUP_RESPOND*)pbyData;
    KPlayer*                        pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoGameCardLookupRespond(pPlayer, pRespond->nTotalCount, pRespond->nCount, pRespond->CardList);

Exit0:
    return;
}

void KRelayClient::OnGameCardCancelRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_GAME_CARD_CANCEL_RESPOND*   pRespond    = (R2S_GAME_CARD_CANCEL_RESPOND*)pbyData;
    KPlayer*                        pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoGameCardCancelRespond(pPlayer, pRespond->byCode);

Exit0:
    return;
}

void KRelayClient::OnSyncMentorData(BYTE* pbyData, size_t uDataLen)
{
    R2S_SYNC_MENTOR_DATA* pRespond = (R2S_SYNC_MENTOR_DATA*)pbyData;
    KMentorRecordBase     Record;

    for (int i = 0; i < pRespond->nRecordCount; i++)
    {
        Record.nMentorValue = pRespond->Data[i].nValue;
        Record.byState      = pRespond->Data[i].byState;

        g_pSO3World->m_MentorCache.AddMentorData(
            pRespond->Data[i].dwMentor, pRespond->Data[i].dwApprentice, Record
        );
    }

    if (pRespond->nRecordCount > 0)
    {
        int     nLastIndex          = pRespond->nRecordCount - 1;
        DWORD   dwLastMentor        = pRespond->Data[nLastIndex].dwMentor;
        DWORD   dwLastApprentice    = pRespond->Data[nLastIndex].dwApprentice;

        DoApplyMentorData(dwLastMentor, dwLastApprentice);
    }
    else
    {
        m_bSyncDataOver = true;
    }
}

void KRelayClient::OnDeleteMentorRecord(BYTE* pbyData, size_t uDataLen)
{
    R2S_DELETE_MENTOR_RECORD* pPak = (R2S_DELETE_MENTOR_RECORD*)pbyData;

    g_pSO3World->m_MentorCache.DeleteMentorRecord(pPak->uMKey);
}

void KRelayClient::OnUpdateMentorRecord(BYTE* pbyData, size_t uDataLen)
{
    R2S_UPDATE_MENTOR_RECORD* pPak          = (R2S_UPDATE_MENTOR_RECORD*)pbyData;
    KPlayer*                  pPlayer       = NULL;
    KMentorRecordBase         Record;

    Record.nMentorValue = pPak->nValue;
    Record.byState      = pPak->byState;

    g_pSO3World->m_MentorCache.UpdateMentorData(pPak->dwMentor, pPak->dwApprentice, Record);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwMentor);
    if (pPlayer != NULL)
    {
        g_PlayerServer.DoSyncMentorData(
            pPlayer->m_nConnIndex,
            pPak->dwMentor, pPak->dwApprentice,
            Record
        );
    }

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwApprentice);
    if (pPlayer != NULL)
    {
        g_PlayerServer.DoSyncMentorData(
            pPlayer->m_nConnIndex,
            pPak->dwMentor, pPak->dwApprentice,
            Record
        );
    }
}

void KRelayClient::OnSeekMentorYell(BYTE* pbyData, size_t uDataLen)
{
    R2S_SEEK_MENTOR_YELL* pPak = (R2S_SEEK_MENTOR_YELL*)pbyData;

    g_PlayerServer.BroadcastSeekMentorYell(
        pPak->szRoleName, pPak->szTongName, pPak->byForceID, pPak->byRoleLevel, pPak->byRoleType
    );
}

void KRelayClient::OnSeekApprenticeYell(BYTE* pbyData, size_t uDataLen)
{
    R2S_SEEK_APPRENTICE_YELL* pPak = (R2S_SEEK_APPRENTICE_YELL*)pbyData;

    g_PlayerServer.BroadcastSeekApprenticeYell(
        pPak->szRoleName, pPak->szTongName, pPak->byForceID, pPak->byRoleLevel, pPak->byRoleType
    );
}
//AutoCode:-处理协议函数结束-

/************************************************************************/

BOOL KRelayClient::DoHandshakeRequest()
{
    BOOL                    bResult         = false;
	BOOL                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
    S2R_HANDSHAKE_REQUEST*  pHandshake      = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_HANDSHAKE_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pHandshake = (S2R_HANDSHAKE_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pHandshake);

	pHandshake->wProtocolID             = s2r_handshake_request;
    pHandshake->nGameWorldLowerVersion  = GAME_WORLD_LOWEST_VERSION;
    pHandshake->nGameWorldUpperVersion  = GAME_WORLD_CURRENT_VERSION;

    assert(g_pSO3World->m_piRecorderTools);
    pHandshake->nServerTime = g_pSO3World->m_piRecorderTools->GetTime();

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoPingSignal()
{
	BOOL						bResult				= false;
	BOOL						bRetCode			= false;
	IKG_Buffer*                 piSendBuffer        = NULL;
	S2R_PING_SIGNAL*			pPingSingal			= NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_PING_SIGNAL));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPingSingal = (S2R_PING_SIGNAL*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPingSingal);

	pPingSingal->wProtocolID       = s2r_ping_signal;
	pPingSingal->dwReserved        = 0;

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);   

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoUpdatePerformance()
{
	BOOL						bResult				= false;
	BOOL						bRetCode			= false;
	IKG_Buffer*                 piSendBuffer        = NULL;
	S2R_UPDATE_PERFORMANCE*	    pPerform			= NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_UPDATE_PERFORMANCE));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPerform = (S2R_UPDATE_PERFORMANCE*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPerform);

	pPerform->wProtocolID       = s2r_update_performance;
	pPerform->nIdleFrame        = g_pSO3World->m_nIdleFrame;
    pPerform->uMemory           = KMemory::GetUsedSize();
    pPerform->nPlayerCount      = (int)g_pSO3World->m_PlayerSet.GetObjCount();
    pPerform->nConnectionCount  = g_PlayerServer.GetConnectionCount();
    pPerform->uNetworkFlux      = g_PlayerServer.GetNetworkFlux();

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);   

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoPlayerLoginRespond(DWORD dwPlayerID, BOOL bPermit, GUID Guid, DWORD dwPacketIdentity)
{
    BOOL                        bResult     = false;
	BOOL                        bRetCode    = false;
	IKG_Buffer*                 piPackage   = NULL;
    S2R_PLAYER_LOGIN_RESPOND*   pRespond    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_PLAYER_LOGIN_RESPOND));
	KGLOG_PROCESS_ERROR(piPackage);

    pRespond = (S2R_PLAYER_LOGIN_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	pRespond->wProtocolID       = s2r_player_login_respond;
	pRespond->dwPacketIdentity  = dwPacketIdentity;
	pRespond->bPermit           = bPermit;
	pRespond->dwPlayerID        = dwPlayerID;
	pRespond->Guid              = Guid;
	pRespond->dwGSAddr          = g_PlayerServer.m_dwInternetAddr;
	pRespond->nGSPort           = g_PlayerServer.m_nListenPort;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoConfirmPlayerLoginRequest(DWORD dwPlayerID)
{
    BOOL                                bResult     = false;
	BOOL                                bRetCode    = false;
    IKG_Buffer*                         piPackage   = NULL;
    S2R_CONFIRM_PLAYER_LOGIN_REQUEST*   pRequest    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_CONFIRM_PLAYER_LOGIN_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_CONFIRM_PLAYER_LOGIN_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID         = s2r_confirm_player_login_request;
	pRequest->dwPlayerID          = dwPlayerID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoPlayerLeaveGS(DWORD dwPlayerID)
{
    BOOL                    bResult     = false;
	BOOL                    bRetCode    = false;
    IKG_Buffer*             piPackage   = NULL;
    S2R_PLAYER_LEAVE_GS*    pLeaveGS    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_PLAYER_LEAVE_GS));
	KGLOG_PROCESS_ERROR(piPackage);

	pLeaveGS = (S2R_PLAYER_LEAVE_GS*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pLeaveGS);

	pLeaveGS->wProtocolID         = s2r_player_leave_gs;
	pLeaveGS->dwPlayerID          = dwPlayerID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoCreateMapRespond(DWORD dwMapID, int nMapCopyIndex, BOOL bSucceed)
{
    BOOL                    bResult     = false;
	BOOL                    bRetCode    = false;
    IKG_Buffer*             piPackage   = NULL;
    S2R_CREATE_MAP_RESPOND* pRespond    = NULL;
    KMapParams*             pMapParam   = NULL;

    pMapParam = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(dwMapID);
    KGLOG_PROCESS_ERROR(pMapParam);

    KGLogPrintf(
        KGLOG_INFO, "Create map %s(%.2u, %d) ... ... [%s]\n", 
        pMapParam->szMapName, dwMapID, nMapCopyIndex, bSucceed ? "OK" : "Failed"
    );

	piPackage = KG_MemoryCreateBuffer(sizeof(S2R_CREATE_MAP_RESPOND));
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (S2R_CREATE_MAP_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	pRespond->wProtocolID        = s2r_create_map_respond;
	pRespond->dwMapID            = dwMapID;
	pRespond->nMapCopyIndex      = nMapCopyIndex;
	pRespond->bResult            = bSucceed;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoSearchMapRequest(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex, int nX, int nY, int nZ)
{
    BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
    IKG_Buffer*                 piSendBuffer        = NULL;
	S2R_SEARCH_MAP_REQUEST*     pSearchMapRequest   = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SEARCH_MAP_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSearchMapRequest = (S2R_SEARCH_MAP_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSearchMapRequest);

	pSearchMapRequest->wProtocolID       = s2r_search_map_request;
	pSearchMapRequest->dwPlayerID        = dwPlayerID;
	pSearchMapRequest->dwMapID           = dwMapID;
    pSearchMapRequest->nMapCopyIndex     = nMapCopyIndex;
    pSearchMapRequest->nPosX             = nX;
    pSearchMapRequest->nPosY             = nY;
    pSearchMapRequest->nPosZ             = nZ;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoTransferPlayerRequest(KPlayer* pPlayer)
{
    BOOL                            bResult         = false;
	BOOL                            bRetCode        = false;
	IKG_Buffer*                     piPackage       = NULL;
    S2R_TRANSFER_PLAYER_REQUEST*    pRequest        = NULL;
    size_t                          uUsedSize       = 0;

	piPackage = KG_MemoryCreateBuffer(sizeof(S2R_TRANSFER_PLAYER_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_TRANSFER_PLAYER_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);
    
    pRequest->wProtocolID           = s2r_transfer_player_request;
    pRequest->dwRoleID              = pPlayer->m_dwID;
    pRequest->bChargeFlag           = pPlayer->m_bChargeFlag;
    pRequest->ExtPointInfo          = pPlayer->m_ExtPointInfo;
    pRequest->bExtPointLock         = pPlayer->m_bExtPointLock;
    pRequest->nLastExtPointIndex    = pPlayer->m_nLastExtPointIndex;
    pRequest->nLastExtPointValue    = pPlayer->m_nLastExtPointValue;
    pRequest->nEndTimeOfFee         = pPlayer->m_nEndTimeOfFee;
    pRequest->nCoin                 = pPlayer->m_nCoin;
    pRequest->nBattleFieldSide      = pPlayer->m_nBattleFieldSide;

    bRetCode  = pPlayer->SaveBaseInfo(&pRequest->RoleBaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoTransferPlayerRespond(DWORD dwPlayerID , BOOL bSucceed, GUID Guid)
{
    BOOL                            bResult     = false;
	BOOL                            bRetCode    = false;
	IKG_Buffer*                     piPackage   = NULL;
    S2R_TRANSFER_PLAYER_RESPOND*    pRespond    = NULL;

	piPackage = KG_MemoryCreateBuffer(sizeof(S2R_TRANSFER_PLAYER_RESPOND));
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (S2R_TRANSFER_PLAYER_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	pRespond->wProtocolID       = s2r_transfer_player_respond;
	pRespond->dwPlayerID        = dwPlayerID;
	pRespond->bSucceed          = bSucceed;
	pRespond->Guid              = Guid;
	pRespond->dwAddress         = g_PlayerServer.m_dwInternetAddr;
	pRespond->wPort             = (WORD)g_PlayerServer.m_nListenPort;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KRelayClient::DoInviteJoinTeamRequest(DWORD dwInviteSrc, const char cszInviteDst[])
{
    BOOL                                    bResult                 = false;
    BOOL                                    bRetCode                = false;
	IKG_Buffer*							    piSendBuffer			= NULL;
	S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST*    pInvitePlayerJoinTeam   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pInvitePlayerJoinTeam = (S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST *)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pInvitePlayerJoinTeam);

	pInvitePlayerJoinTeam->wProtocolID		 = s2r_invite_player_join_team_request;
	pInvitePlayerJoinTeam->dwInviteSrc		 = dwInviteSrc;

	strncpy(pInvitePlayerJoinTeam->szInviteRole, cszInviteDst, sizeof(pInvitePlayerJoinTeam->szInviteRole));
	pInvitePlayerJoinTeam->szInviteRole[sizeof(pInvitePlayerJoinTeam->szInviteRole) - 1] = '\0';

	bResult = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bResult);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoInviteJoinTeamRespond(const char cszInviteSrc[], DWORD dwInviteDst, BOOL bAgreeFlag)
{
    BOOL                                    bResult                 = false;
    BOOL                                    bRetCode                = false;
	IKG_Buffer*							    piSendBuffer			= NULL;
	S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND*    pInviteRespond          = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pInviteRespond = (S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pInviteRespond);

	pInviteRespond->wProtocolID		 = s2r_invite_player_join_team_respond;
	pInviteRespond->dwInviteDst		 = dwInviteDst;
    pInviteRespond->bAgreeFlag       = bAgreeFlag;

	strncpy(pInviteRespond->szInviteSrc, cszInviteSrc, sizeof(pInviteRespond->szInviteSrc));
	pInviteRespond->szInviteSrc[sizeof(pInviteRespond->szInviteSrc) - 1] = '\0';

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyJoinTeamRequest(DWORD dwPlayerID, const char cszApplyDst[])
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    S2R_APPLY_JOIN_TEAM_REQUEST*    pApplyJoinTeam  = NULL;
    IKG_Buffer*                     piSendBuffer    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_JOIN_TEAM_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pApplyJoinTeam = (S2R_APPLY_JOIN_TEAM_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pApplyJoinTeam);

    pApplyJoinTeam->wProtocolID        = s2r_apply_join_team_request;
    pApplyJoinTeam->dwApplySrc         = dwPlayerID;

    strncpy(pApplyJoinTeam->szApplyDst, cszApplyDst, sizeof(pApplyJoinTeam->szApplyDst));
    pApplyJoinTeam->szApplyDst[sizeof(pApplyJoinTeam->szApplyDst) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyJoinTeamRespond(DWORD dwLeaderID, const char cszApplySrc[], BOOL bAgreeFlag)
{
    BOOL                            bResult                 = false;
    BOOL                            bRetCode                = false;
	IKG_Buffer*						piSendBuffer			= NULL;
	S2R_APPLY_JOIN_TEAM_RESPOND*    pApplyRespond           = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_JOIN_TEAM_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pApplyRespond = (S2R_APPLY_JOIN_TEAM_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pApplyRespond);

	pApplyRespond->wProtocolID		 = s2r_apply_join_team_respond;
	pApplyRespond->dwApplyDst		 = dwLeaderID;
    pApplyRespond->bAgreeFlag        = bAgreeFlag;

	strncpy(pApplyRespond->szApplySrc, cszApplySrc, sizeof(pApplyRespond->szApplySrc));
	pApplyRespond->szApplySrc[sizeof(pApplyRespond->szApplySrc) - 1] = '\0';

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamDisbandRequest(DWORD dwLeaderID, DWORD dwTeamID)
{
    BOOL                       bResult              = false;
    BOOL                       bRetCode             = false;
    S2R_TEAM_DISBAND_REQUEST*  pTeamDisbandRequest = NULL;
    IKG_Buffer*                piSendBuffer         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_DISBAND_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pTeamDisbandRequest = (S2R_TEAM_DISBAND_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pTeamDisbandRequest);

    pTeamDisbandRequest->wProtocolID      = s2r_team_disband_request;
    pTeamDisbandRequest->dwLeaderID       = dwLeaderID;
    pTeamDisbandRequest->dwTeamID        = dwTeamID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamDelMemberRequest(DWORD dwTeamID, DWORD dwRequestSrc, DWORD dwPlayerID)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    S2R_TEAM_DEL_MEMBER_REQUEST*    pDelMemberRequest   = NULL;
    IKG_Buffer*                     piSendBuffer        = NULL;

    // TODO: 向Relay发请求: 将玩家从队伍删除
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_DEL_MEMBER_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDelMemberRequest = (S2R_TEAM_DEL_MEMBER_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDelMemberRequest);

    pDelMemberRequest->wProtocolID      = s2r_team_del_member_request;
    pDelMemberRequest->dwTeamID        = dwTeamID;
    pDelMemberRequest->dwRequestSrc     = dwRequestSrc;
    pDelMemberRequest->dwMemberID       = dwPlayerID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamChangeAuthorityRequest(DWORD dwTeamID, DWORD dwRequestSrc, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
{
    BOOL                                bResult                 = false;
    BOOL                                bRetCode                = false;
    S2R_TEAM_CHANGE_AUTHORITY_REQUEST*  pChangeLeaderRequest    = NULL;
    IKG_Buffer*                         piSendBuffer            = NULL;

    // 向Relay发请求: 设置队长
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_CHANGE_AUTHORITY_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pChangeLeaderRequest = (S2R_TEAM_CHANGE_AUTHORITY_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pChangeLeaderRequest);

    pChangeLeaderRequest->wProtocolID      = s2r_team_change_authority_request;
    pChangeLeaderRequest->dwTeamID        = dwTeamID;
    pChangeLeaderRequest->dwRequestSrc     = dwRequestSrc;
    pChangeLeaderRequest->byType           = (BYTE)eType;
    pChangeLeaderRequest->dwTargetID       = dwTargetID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSetLootModeRequest(DWORD dwTeamID, DWORD dwRequestSrc, int nLootMode)
{
    BOOL                            bResult                 = false;
    BOOL                            bRetCode                = false;
    S2R_TEAM_SET_LOOT_MODE_REQUEST* pRequest                = NULL;
    IKG_Buffer*                     piSendBuffer            = NULL;

    // 向Relay发请求: 设置队长
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_LOOT_MODE_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRequest = (S2R_TEAM_SET_LOOT_MODE_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_team_set_loot_mode_request;
    pRequest->dwTeamID     = dwTeamID;
    pRequest->dwRequestSrc  = dwRequestSrc;
    pRequest->nLootMode     = nLootMode;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSetRollQualityRequest(DWORD dwTeamID, DWORD dwRequestSrc, int nRollQuality)
{
	BOOL                                bResult         = false;
	BOOL                                bRetCode        = false;
	S2R_TEAM_SET_ROLL_QUALITY_REQUEST*  pRequest        = NULL;
	IKG_Buffer*                         piSendBuffer    = NULL;

	// 向Relay发请求: 设置队伍分配的ROLL品质等级
	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_ROLL_QUALITY_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pRequest = (S2R_TEAM_SET_ROLL_QUALITY_REQUEST *)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID      = s2r_team_set_roll_quality_request;
	pRequest->dwTeamID        = dwTeamID;
	pRequest->dwRequestSrc     = dwRequestSrc;
	pRequest->nRollQuality	   = nRollQuality;

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoTeamSetFormationLeader(DWORD dwTeamID, DWORD dwRequestSrc, DWORD dwNewFormationLeaderID, int nGroupIndex)
{
    BOOL                                    bResult         = false;
	BOOL                                    bRetCode        = false;
	S2R_TEAM_SET_FORMATION_LEADER_REQUEST*  pRequest        = NULL;
	IKG_Buffer*                             piSendBuffer    = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_FORMATION_LEADER_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pRequest = (S2R_TEAM_SET_FORMATION_LEADER_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID           = s2r_team_set_formation_leader_request;
	pRequest->dwTeamID             = dwTeamID;
	pRequest->dwRequestSrc          = dwRequestSrc;
	pRequest->dwNewFormationLeader	= dwNewFormationLeaderID;
    pRequest->nGroupIndex           = nGroupIndex;

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoTeamSyncMemberMaxLMR(KPlayer* pPlayer)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    KTeam*                          pTeam               = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo         = NULL;
    KTEAM_MEMBER_INFO*              pSystemMemberInfo   = NULL;
    BOOL                            bValueChanged       = false;
    BOOL                            bValueSystemChanged = false;
    IKG_Buffer*                     piSendBuffer        = NULL;
    S2R_SYNC_TEAM_MEMBER_MAX_LMR*   pPak                = NULL;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPlayer->m_dwTeamID, pPlayer->m_dwID);
    if (pMemberInfo)
    {
        bValueChanged |= (pMemberInfo->nMaxLife != pPlayer->m_nMaxLife);
        bValueChanged |= (pMemberInfo->nMaxMana != pPlayer->m_nMaxMana);
        bValueChanged |= (pMemberInfo->nMaxRage != pPlayer->m_nMaxRage);
    }

    pSystemMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPlayer->m_dwSystemTeamID, pPlayer->m_dwID);
    if (pSystemMemberInfo)
    {
        bValueSystemChanged |= (pSystemMemberInfo->nMaxLife != pPlayer->m_nMaxLife);
        bValueSystemChanged |= (pSystemMemberInfo->nMaxMana != pPlayer->m_nMaxMana);
        bValueSystemChanged |= (pSystemMemberInfo->nMaxRage != pPlayer->m_nMaxRage);
    }

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_MAX_LMR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_MAX_LMR*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID      = s2r_sync_team_member_max_lmr;
    pPak->dwMemberID       = pPlayer->m_dwID;
    pPak->nMaxLife         = pPlayer->m_nMaxLife;
    pPak->nMaxMana         = pPlayer->m_nMaxMana;
    pPak->nMaxRage         = pPlayer->m_nMaxRage;

    if (bValueChanged)
    {
        pPak->dwTeamID    = pPlayer->m_dwTeamID;

        assert(pMemberInfo);

        pMemberInfo->nMaxLife = pPlayer->m_nMaxLife;
        pMemberInfo->nMaxMana = pPlayer->m_nMaxMana;
        pMemberInfo->nMaxRage = pPlayer->m_nMaxRage;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (bValueSystemChanged)
    {
        pPak->dwTeamID    = pPlayer->m_dwSystemTeamID;

        assert(pSystemMemberInfo);

        pSystemMemberInfo->nMaxLife = pPlayer->m_nMaxLife;
        pSystemMemberInfo->nMaxMana = pPlayer->m_nMaxMana;
        pSystemMemberInfo->nMaxRage = pPlayer->m_nMaxRage;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSyncMemberCurrentLMR(KPlayer* pPlayer)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    int                                 nNewLifePercent = 0;
    int                                 nNewManaPercent = 0;
    int                                 nNewRagePercent = 0;
    BOOL                                bValueChanged   = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*   pPak            = NULL;

    if (pPlayer->m_nMaxLife > 0)
        nNewLifePercent = pPlayer->m_nCurrentLife * UCHAR_MAX / pPlayer->m_nMaxLife;

    if (pPlayer->m_nMaxMana > 0)
        nNewManaPercent = pPlayer->m_nCurrentMana * UCHAR_MAX / pPlayer->m_nMaxMana;

    if (pPlayer->m_nMaxRage > 0)
        nNewRagePercent = pPlayer->m_nCurrentRage * UCHAR_MAX / pPlayer->m_nMaxRage;

    bValueChanged |= (nNewLifePercent != pPlayer->m_nLastSyncGlobalTeamLifePercent);
    bValueChanged |= (nNewManaPercent != pPlayer->m_nLastSyncGlobalTeamManaPercent);
    bValueChanged |= (nNewRagePercent != pPlayer->m_nLastSyncGlobalTeamRagePercent);

    KG_PROCESS_ERROR(bValueChanged);

    pPlayer->m_nLastSyncGlobalTeamLifePercent = nNewLifePercent;
    pPlayer->m_nLastSyncGlobalTeamManaPercent = nNewManaPercent;
    pPlayer->m_nLastSyncGlobalTeamRagePercent = nNewRagePercent;

    // 全局同步覆盖局部同步数值
    pPlayer->m_nLastSyncLocalTeamLifePercent = nNewLifePercent;
    pPlayer->m_nLastSyncLocalTeamManaPercent = nNewManaPercent;
    pPlayer->m_nLastSyncLocalTeamRagePercent = nNewRagePercent;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_CURRENT_LMR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID      = s2r_sync_team_member_current_lmr;
    pPak->dwMemberID       = pPlayer->m_dwID;
    pPak->byLifePercent    = (BYTE)nNewLifePercent;
    pPak->byManaPercent    = (BYTE)nNewManaPercent;
    pPak->byRagePercent    = (BYTE)nNewRagePercent;

    if (pPlayer->m_dwSystemTeamID != ERROR_ID)
    {
        pPak->dwTeamID    = pPlayer->m_dwSystemTeamID;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pPlayer->m_dwTeamID != ERROR_ID)
    {
        pPak->dwTeamID     = pPlayer->m_dwTeamID;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSyncMemberMisc(KPlayer* pPlayer)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    KTEAM_MEMBER_INFO*          pMemberInfo         = NULL;
    KTEAM_MEMBER_INFO*          pSystemMemberInfo   = NULL;
    BOOL                        bValueChanged       = false;
    BOOL                        bValueSystemChanged = false;
    IKG_Buffer*                 piSendBuffer        = NULL;
    S2R_SYNC_TEAM_MEMBER_MISC*  pPak                = NULL;
    KPORTRAIT_INFO              PortraitInfo;

    KG_PROCESS_ERROR(pPlayer->m_pScene);

    PortraitInfo.dwFaceStyle     = pPlayer->m_wRepresentId[perFaceStyle];
    PortraitInfo.dwHelmStyle     = pPlayer->m_wRepresentId[perHelmStyle];
    PortraitInfo.dwHelmColor     = pPlayer->m_wRepresentId[perHelmColor];
    PortraitInfo.dwHairStyle     = pPlayer->m_wRepresentId[perHairStyle];
    PortraitInfo.dwReserved      = 0;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPlayer->m_dwTeamID, pPlayer->m_dwID);
    if (pMemberInfo)
    {
        bValueChanged |= (pMemberInfo->PortraitInfo != PortraitInfo);
        bValueChanged |= (pMemberInfo->dwForceID != pPlayer->m_dwForceID);
        bValueChanged |= (pMemberInfo->eCamp != pPlayer->m_eCamp);
        bValueChanged |= (pMemberInfo->nLevel != pPlayer->m_nLevel);
        bValueChanged |= (pMemberInfo->nRoleType != pPlayer->m_eRoleType);
        bValueChanged |= (pMemberInfo->bDeathFlag != (pPlayer->m_eMoveState == cmsOnDeath));
    }

    pSystemMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPlayer->m_dwSystemTeamID, pPlayer->m_dwID);
    if (pSystemMemberInfo)
    {
        bValueSystemChanged |= (pSystemMemberInfo->PortraitInfo != PortraitInfo);
        bValueSystemChanged |= (pSystemMemberInfo->dwForceID != pPlayer->m_dwForceID);
        bValueSystemChanged |= (pSystemMemberInfo->eCamp != pPlayer->m_eCamp);
        bValueSystemChanged |= (pSystemMemberInfo->nLevel != pPlayer->m_nLevel);
        bValueSystemChanged |= (pSystemMemberInfo->nRoleType != pPlayer->m_eRoleType);
        bValueSystemChanged |= (pSystemMemberInfo->bDeathFlag != (pPlayer->m_eMoveState == cmsOnDeath)); 
    }

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_MISC));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_MISC*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID      = s2r_sync_team_member_misc;
    pPak->dwMemberID       = pPlayer->m_dwID;

    pPak->PortraitInfo     = PortraitInfo;
    pPak->dwForceID        = pPlayer->m_dwForceID;
    pPak->byCamp           = (BYTE)pPlayer->m_eCamp;
    pPak->byLevel          = (BYTE)pPlayer->m_nLevel;
    pPak->byRoleType       = (BYTE)pPlayer->m_eRoleType;
    pPak->bDeathFlag       = (pPlayer->m_eMoveState == cmsOnDeath);

    if (bValueChanged)
    {
        pPak->dwTeamID    = pPlayer->m_dwTeamID;

        pMemberInfo->PortraitInfo   = PortraitInfo;
        pMemberInfo->dwForceID      = pPlayer->m_dwForceID;
        pMemberInfo->nLevel         = pPlayer->m_nLevel;
        pMemberInfo->eCamp          = pPlayer->m_eCamp;
        pMemberInfo->nRoleType      = pPlayer->m_eRoleType;
        pMemberInfo->bDeathFlag     = (pPlayer->m_eMoveState == cmsOnDeath);

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (bValueSystemChanged)
    {
        pPak->dwTeamID    = pPlayer->m_dwSystemTeamID;

        pSystemMemberInfo->PortraitInfo   = PortraitInfo;
        pSystemMemberInfo->dwForceID      = pPlayer->m_dwForceID;
        pSystemMemberInfo->nLevel         = pPlayer->m_nLevel;
        pSystemMemberInfo->eCamp          = pPlayer->m_eCamp;
        pSystemMemberInfo->nRoleType      = pPlayer->m_eRoleType;
        pSystemMemberInfo->bDeathFlag     = (pPlayer->m_eMoveState == cmsOnDeath);

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoSyncTeamMemberPosition(KPlayer* pPlayer)
{
    BOOL                             bResult        = false;
    BOOL                             bRetCode       = false;
    BOOL                             bValueChanged  = false;
    IKG_Buffer*                      piSendBuffer   = NULL;
    S2R_SYNC_TEAM_MEMBER_POSITION*   pPak           = NULL;

    KG_PROCESS_ERROR(pPlayer->m_pScene);

    bValueChanged |= (pPlayer->m_pScene->m_dwMapID != pPlayer->m_dwLastSyncGlobalTeamMemberMapID);
    bValueChanged |= (pPlayer->m_pScene->m_nCopyIndex != pPlayer->m_nLastSyncGlobalTeamMemberMapCopyIndex);
    bValueChanged |= (pPlayer->m_nX != pPlayer->m_nLastSyncGlobalTeamMemberPosX);
    bValueChanged |= (pPlayer->m_nY != pPlayer->m_nLastSyncGlobalTeamMemberPosY);

    KG_PROCESS_ERROR(bValueChanged);

    pPlayer->m_dwLastSyncGlobalTeamMemberMapID         = pPlayer->m_pScene->m_dwMapID;
    pPlayer->m_nLastSyncGlobalTeamMemberMapCopyIndex   = pPlayer->m_pScene->m_nCopyIndex;
    pPlayer->m_nLastSyncGlobalTeamMemberPosX           = pPlayer->m_nX;
    pPlayer->m_nLastSyncGlobalTeamMemberPosY           = pPlayer->m_nY;

    // 全局同步时覆盖局部同步数值
    pPlayer->m_dwLastSyncLocalTeamMemberMapID         = pPlayer->m_pScene->m_dwMapID;
    pPlayer->m_nLastSyncLocalTeamMemberMapCopyIndex   = pPlayer->m_pScene->m_nCopyIndex;
    pPlayer->m_nLastSyncLocalTeamMemberPosX           = pPlayer->m_nX;
    pPlayer->m_nLastSyncLocalTeamMemberPosY           = pPlayer->m_nY;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_POSITION));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_POSITION*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID        = s2r_sync_team_member_position;
    pPak->dwMemberID         = pPlayer->m_dwID;
    pPak->dwMapID            = pPlayer->m_pScene->m_dwMapID;
    pPak->nMapCopyIndex      = pPlayer->m_pScene->m_nCopyIndex;
    pPak->nPosX              = pPlayer->m_nX;
    pPak->nPosY              = pPlayer->m_nY;

    if (pPlayer->m_dwSystemTeamID != ERROR_ID)
    {
        pPak->dwTeamID		 = pPlayer->m_dwSystemTeamID;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pPlayer->m_dwTeamID != ERROR_ID)
    {
        pPak->dwTeamID		 = pPlayer->m_dwTeamID;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSetMarkRequest(DWORD dwTeamID, DWORD dwPlayerID, int nMarkType, DWORD dwTargetID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuff      = NULL;
    S2R_TEAM_SET_MARK_REQUEST*  pTeamSetMark   = NULL;

    piSendBuff = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_MARK_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuff);

    pTeamSetMark = (S2R_TEAM_SET_MARK_REQUEST*)piSendBuff->GetData();
    KGLOG_PROCESS_ERROR(pTeamSetMark);

    pTeamSetMark->wProtocolID = s2r_team_set_mark_request;
    pTeamSetMark->dwTeamID   = dwTeamID;
    pTeamSetMark->dwMemberID  = dwPlayerID;
    pTeamSetMark->nMarkType   = nMarkType;
    pTeamSetMark->dwTargetID  = dwTargetID;

    bRetCode = Send(piSendBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuff);
    return bResult;
}

BOOL KRelayClient::DoTeamLevelUpRequest(DWORD dwTeamID, DWORD dwRequestSrc)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piSendBuff  = NULL;
    S2R_TEAM_LEVEL_UP_RAID_REQUEST* pRequest    = NULL;

    piSendBuff = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_TEAM_LEVEL_UP_RAID_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuff);

    pRequest = (S2R_TEAM_LEVEL_UP_RAID_REQUEST*)piSendBuff->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID  = s2r_team_level_up_raid_request;
    pRequest->dwTeamID     = dwTeamID;
    pRequest->dwRequestSrc = dwRequestSrc;

    bRetCode = Send(piSendBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuff);
    return bResult;
}

BOOL KRelayClient::DoTeamChangeMemberGroupRequest(
    DWORD dwTeamID, DWORD dwMemberID, int nDstGroupIndex, DWORD dwDstMemberID, DWORD dwRequestSrc
)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    IKG_Buffer*                             piSendBuff  = NULL;
    S2R_TEAM_CHANGE_MEMBER_GROUP_REQUEST*   pRequest    = NULL;

    piSendBuff = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_TEAM_CHANGE_MEMBER_GROUP_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuff);

    pRequest = (S2R_TEAM_CHANGE_MEMBER_GROUP_REQUEST*)piSendBuff->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_team_change_member_group_request;
    pRequest->dwTeamID          = dwTeamID;
    pRequest->dwMemberID        = dwMemberID;
    pRequest->nDstGroupIndex    = nDstGroupIndex;
    pRequest->dwDstMemberID     = dwDstMemberID;
    pRequest->dwRequestSrc      = dwRequestSrc;

    bRetCode = Send(piSendBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuff);
    return bResult;
}

BOOL KRelayClient::DoTalkMessage(
    int nMsgType, 
    DWORD dwTalkerID, const char cszTalker[],
    DWORD dwReceiverID, const char cszReceiver[],
    size_t uDataLen, const BYTE*  pbyTalkData
)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    IKG_Buffer*         piSendBuffer    = NULL;
    S2R_TALK_MESSAGE*   pTalkMsg        = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_TALK_MESSAGE) + uDataLen));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pTalkMsg = (S2R_TALK_MESSAGE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pTalkMsg);

    pTalkMsg->wProtocolID       = s2r_talk_message;
    pTalkMsg->byMsgType         = (BYTE)nMsgType;
    pTalkMsg->dwTalkerID        = dwTalkerID;
    pTalkMsg->dwReceiverID      = dwReceiverID;

    pTalkMsg->szTalker[0] = '\0';
    if (cszTalker)
    {
        strncpy(pTalkMsg->szTalker, cszTalker, sizeof(pTalkMsg->szTalker));
        pTalkMsg->szTalker[sizeof(pTalkMsg->szTalker) - 1] = '\0';
    }

    pTalkMsg->szReceiver[0] = '\0';
    if (cszReceiver)
    {
        strncpy(pTalkMsg->szReceiver, cszReceiver, sizeof(pTalkMsg->szReceiver));
        pTalkMsg->szReceiver[sizeof(pTalkMsg->szReceiver) - 1] = '\0';
    }

    memcpy(pTalkMsg->byTalkData, pbyTalkData, uDataLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoPlayerTalkError(DWORD dwPlayerID, int nErrorCode)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_PLAYER_TALK_ERROR*  pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_PLAYER_TALK_ERROR)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_PLAYER_TALK_ERROR*)piSendBuffer->GetData();
    assert(pPak);

    pPak->wProtocolID   = s2r_player_talk_error;
    pPak->dwReceiverID  = dwPlayerID;
    pPak->nErrorCode    = nErrorCode;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyFellowshipDataRequest(DWORD dwPlayerID)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    S2R_APPLY_FELLOWSHIP_DATA_REQUEST*  pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_APPLY_FELLOWSHIP_DATA_REQUEST)));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_APPLY_FELLOWSHIP_DATA_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID                = s2r_apply_fellowship_data_request;
    pPak->dwPlayerID                 = dwPlayerID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoUpdateFellowshipData(DWORD dwPlayerID, size_t uDataSize, BYTE byData[])
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    S2R_UPDATE_FELLOWSHIP_DATA* pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_UPDATE_FELLOWSHIP_DATA) + uDataSize));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_UPDATE_FELLOWSHIP_DATA*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_update_fellowship_data;
    pPak->dwPlayerID        = dwPlayerID;
    memcpy(pPak->byData, byData, uDataSize);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoPlayerAddFellowshipRequest(DWORD dwPlayerID, char szAlliedPlayerName[], int nType)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    S2R_ADD_FELLOWSHIP_REQUEST* pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ADD_FELLOWSHIP_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_ADD_FELLOWSHIP_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = s2r_add_fellowship_request;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->byType        = (BYTE)nType;

    strncpy(pPak->szAlliedPlayerName, szAlliedPlayerName, sizeof(pPak->szAlliedPlayerName));
    pPak->szAlliedPlayerName[sizeof(pPak->szAlliedPlayerName) - sizeof('\0')] = '\0';

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetFellowshipName(DWORD dwPlayerID, int nArrayLength, DWORD dwAlliedPlayerIDArray[])
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    S2R_GET_FELLOWSHIP_NAME_REQUEST*    pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);
    assert(nArrayLength >= 0);
    assert(nArrayLength <= UCHAR_MAX);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GET_FELLOWSHIP_NAME_REQUEST) + sizeof(DWORD) * nArrayLength);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_GET_FELLOWSHIP_NAME_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = s2r_get_fellowship_name_request;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->byCount       = (BYTE)nArrayLength;

    for (int i = 0; i < nArrayLength; i++)
    {
        pPak->dwAlliedPlayerIDArray[i] = dwAlliedPlayerIDArray[i];
    }

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyFellowshipPlayerLevelAndForceID(DWORD dwPlayerID)
{
    BOOL                                            bResult         = false;
    BOOL                                            bRetCode        = false;
    IKG_Buffer*                                     piSendBuffer    = NULL;
    S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID*  pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = s2r_apply_fellowship_player_level_and_forceid;
    pPak->dwPlayerID    = dwPlayerID;

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoAddFellowshipNotify(DWORD dwPlayerID, DWORD dwAlliedPlayerID, char szAlliedPlayerName[], int nType)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    S2R_ADD_FELLOWSHIP_NOTIFY*  pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);
    assert(szAlliedPlayerName);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ADD_FELLOWSHIP_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_ADD_FELLOWSHIP_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_add_fellowship_notify;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwAlliedPlayerID  = dwAlliedPlayerID;
    pPak->nType             = nType;

    strncpy(pPak->szAlliedPlayerName, szAlliedPlayerName, sizeof(pPak->szAlliedPlayerName));
    pPak->szAlliedPlayerName[sizeof(pPak->szAlliedPlayerName) - 1] = '\0';

    bRetCode = Send(piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

// ------------------- 邮件系统 ------------------------------->

BOOL KRelayClient::DoSendMailRequest(int nRequestID, const char cszDstName[], KMail* pMail, size_t uMailLen)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_SEND_MAIL_REQUEST*  pPak            = NULL;

    assert(cszDstName);
    assert(pMail);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_SEND_MAIL_REQUEST) + uMailLen));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SEND_MAIL_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = s2r_send_mail_request;
    pPak->byRequestID   = (BYTE)nRequestID;

    strncpy(pPak->szReceiverName, cszDstName, sizeof(pPak->szReceiverName));
    pPak->szReceiverName[sizeof(pPak->szReceiverName) - 1] = '\0';

    memcpy(pPak->byData, pMail, uMailLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoSendGlobalMailRequest(
    KMail* pMail, size_t uMailLen, int nLifeTime
)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piSendBuffer    = NULL;
    S2R_SEND_GLOBAL_MAIL_REQUEST*   pPak            = NULL;

    assert(pMail);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SEND_GLOBAL_MAIL_REQUEST) + uMailLen);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SEND_GLOBAL_MAIL_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_send_global_mail_request;
    
    pPak->nLifeTime 		= nLifeTime;

    memcpy(pPak->byData, pMail, uMailLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetMailListRequest(DWORD dwPlayerID, DWORD dwStartID)
{
    BOOL                        bResult      = false;
    BOOL                        bRetCode     = false;
    IKG_Buffer*                 piSendBuffer = NULL;
    S2R_GET_MAILLIST_REQUEST*   pPak         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_GET_MAILLIST_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_GET_MAILLIST_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID      = s2r_get_maillist_request;
    pPak->dwPlayerID       = dwPlayerID;
    pPak->dwStartID        = dwStartID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoQueryMailContent(DWORD dwPlayerID, DWORD dwMailID, int nMailType)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_QUERY_MAIL_CONTENT* pPak            = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_QUERY_MAIL_CONTENT));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_QUERY_MAIL_CONTENT*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_query_mail_content;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwMailID          = dwMailID;
    pPak->byMailType        = (BYTE)nMailType;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoAcquireMailMoneyRequest(DWORD dwPlayerID, DWORD dwMailID)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piSendBuffer    = NULL;
    S2R_ACQUIRE_MAIL_MONEY_REQUEST* pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_ACQUIRE_MAIL_MONEY_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_ACQUIRE_MAIL_MONEY_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = s2r_acquire_mail_money_request;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->dwMailID      = dwMailID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoAcquireMailItemRequest(DWORD dwPlayerID, DWORD dwMailID, int nIndex)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piSendBuffer    = NULL;
    S2R_ACQUIRE_MAIL_ITEM_REQUEST*  pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ACQUIRE_MAIL_ITEM_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_ACQUIRE_MAIL_ITEM_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = s2r_acquire_mail_item_request;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->dwMailID      = dwMailID;
    pPak->byIndex       = (BYTE)nIndex;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGiveMailMoneyToPlayerRespond(DWORD dwPlayerID, DWORD dwMailID, KMAIL_RESULT_CODE eResult)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    IKG_Buffer*                             piSendBuffer    = NULL;
    S2R_GIVE_MAIL_MONEY_TO_PLAYER_RESPOND*  pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GIVE_MAIL_MONEY_TO_PLAYER_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_GIVE_MAIL_MONEY_TO_PLAYER_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_give_mail_money_to_player;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwMailID          = dwMailID;
    pPak->byResult          = (BYTE)eResult;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGiveMailItemToPlayerRespond(DWORD dwPlayerID, DWORD dwMailID, int nIndex, KMAIL_RESULT_CODE eResult)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    IKG_Buffer*                             piSendBuffer    = NULL;
    S2R_GIVE_MAIL_ITEM_TO_PLAYER_RESPOND*   pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GIVE_MAIL_ITEM_TO_PLAYER_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_GIVE_MAIL_ITEM_TO_PLAYER_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_give_mail_item_to_player;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwMailID          = dwMailID;
    pPak->byIndex           = (BYTE)nIndex;
    pPak->byResult          = (BYTE)eResult;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoSetMailRead(DWORD dwPlayer, DWORD dwMailID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_SET_MAIL_READ*      pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SET_MAIL_READ));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SET_MAIL_READ*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_set_mail_read;
    pPak->dwPlayerID        = dwPlayer;
    pPak->dwMailID          = dwMailID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoDeleteMail(DWORD dwPlayer, DWORD dwMailID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_DELETE_MAIL*        pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_DELETE_MAIL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_DELETE_MAIL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_delete_mail;
    pPak->dwPlayerID        = dwPlayer;
    pPak->dwMailID          = dwMailID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoReturnMail(DWORD dwPlayer, DWORD dwMailID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_RETURN_MAIL*        pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_RETURN_MAIL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_RETURN_MAIL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_return_mail;
    pPak->dwPlayerID        = dwPlayer;
    pPak->dwMailID          = dwMailID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

// <------------------- 邮件系统 -------------------------------

//同步队伍标记
BOOL KRelayClient::DoSyncMidMapMark(KPlayer* pSrc, int nMap, int nX, int nY, int nType, const char* pszComment)
{
    BOOL                    bResult         = false;
	BOOL					bRetCode		= false;
	IKG_Buffer*				piSendBuffer	= NULL;
	S2R_SYNC_MID_MAP_MARK*	pPak			= NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_MID_MAP_MARK));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPak = (S2R_SYNC_MID_MAP_MARK*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPak);

	KG_PROCESS_ERROR(pSrc);

	pPak->wProtocolID = s2r_sync_mid_map_mark;
	pPak->dwSrcPlayerID = pSrc->m_dwID;
	pPak->nMapID = nMap;
	pPak->nX = nX;
	pPak->nY = nY;
	pPak->nType = nType;
	
	ZeroMemory(pPak->szComment, sizeof(pPak->szComment));
	if (pszComment)
	{
		strncpy(pPak->szComment, pszComment, MAX_MIDMAP_MARK_COMMENT_LEN);
		pPak->szComment[MAX_MIDMAP_MARK_COMMENT_LEN - 1] = '\0';
	}

    if (pSrc->m_dwSystemTeamID != ERROR_ID)
    {
        pPak->dwTeamID = pSrc->m_dwSystemTeamID;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pSrc->m_dwTeamID != ERROR_ID)
    {
        pPak->dwTeamID = pSrc->m_dwTeamID;

        bRetCode = Send(piSendBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

struct KFuncGetScenePlayer
{
    std::vector<DWORD, KMemory::KAllocator<DWORD> > m_vecPlayerID;

    BOOL operator()(KPlayer* pPlayer) 
    {
        m_vecPlayerID.push_back(pPlayer->m_dwID);
        return true;
    }
};

BOOL KRelayClient::DoSaveScenePlayerList(KScene* pScene)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    IKG_Buffer*                     piPackage           = NULL;
    S2R_SAVE_SCENE_PLAYER_LIST*     pPlayerList         = NULL;
    int                             nPlayerCount        = 0;
    KFuncGetScenePlayer             GetScenePlayerFunc;

    pScene->TraversePlayer(GetScenePlayerFunc);
    
    nPlayerCount = (int)GetScenePlayerFunc.m_vecPlayerID.size();
    KG_PROCESS_SUCCESS(nPlayerCount == 0);
    
    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SAVE_SCENE_PLAYER_LIST) + sizeof(DWORD) * nPlayerCount);
	KGLOG_PROCESS_ERROR(piPackage);

    pPlayerList = (S2R_SAVE_SCENE_PLAYER_LIST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pPlayerList);

    pPlayerList->wProtocolID   = s2r_save_scene_player_list;
    pPlayerList->dwMapID       = pScene->m_dwMapID;
    pPlayerList->nMapCopyIndex = pScene->m_nCopyIndex;
    pPlayerList->nPlayerCount  = nPlayerCount;

    memcpy(pPlayerList->dwPlayerList, &(GetScenePlayerFunc.m_vecPlayerID[0]), sizeof(DWORD) * nPlayerCount);

    bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSaveSceneData(KScene* pScene)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    size_t                  uDataLen        = 0;
    IKG_Buffer*             piPackage       = NULL;
    S2R_SAVE_SCENE_DATA*    pSceneData      = NULL;
    BYTE*                   pbyOffset       = NULL;
    size_t                  uLeftSize       = 0;
    size_t                  uUsedSize       = 0;

    uDataLen = sizeof(size_t) + SCENE_CUSTOM_DATA_SIZE + sizeof(size_t) + SCENE_PROGRESS_DATA_LEN;
    
    piPackage = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_SAVE_SCENE_DATA) + uDataLen));
	KGLOG_PROCESS_ERROR(piPackage);

    pSceneData = (S2R_SAVE_SCENE_DATA*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSceneData);

    pSceneData->wProtocolID   = s2r_save_scene_data;
    pSceneData->dwMapID       = pScene->m_dwMapID;
    pSceneData->nMapCopyIndex = pScene->m_nCopyIndex;
    pSceneData->uDataLen      = uDataLen;

    pbyOffset = pSceneData->byData;
    uLeftSize = uDataLen;

    *(DWORD*)pbyOffset = SCENE_PROGRESS_DATA_LEN;
    pbyOffset += sizeof(DWORD);
    uLeftSize -= sizeof(DWORD);
    
    bRetCode = pScene->m_ProgressData.Save(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);

    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    *(DWORD*)pbyOffset = SCENE_CUSTOM_DATA_SIZE;
    pbyOffset += sizeof(DWORD);
    uLeftSize -= sizeof(DWORD);

	bRetCode = pScene->m_CustomData.Save(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);

    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    assert(uLeftSize == 0);

    bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoPlayerEnterSceneNotify(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    IKG_Buffer*                         piPackage       = NULL;
    S2R_PLAYER_ENTER_SCENE_NOTIFY*      pNotify         = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_PLAYER_ENTER_SCENE_NOTIFY));
    KGLOG_PROCESS_ERROR(piPackage);

    pNotify = (S2R_PLAYER_ENTER_SCENE_NOTIFY*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID   = s2r_player_enter_scene_notify;
    pNotify->dwPlayerID    = dwPlayerID;
    pNotify->dwMapID       = dwMapID;
    pNotify->nMapCopyIndex = nMapCopyIndex;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSyncRoadTrackInfo(int nNodeID, KCAMP eCamp, BOOL bEnable)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piPackage       = NULL;
    S2R_SYNC_ROAD_TRACK_INFO*   pRoadTrackInfo  = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SYNC_ROAD_TRACK_INFO));
	KGLOG_PROCESS_ERROR(piPackage);

    pRoadTrackInfo               = (S2R_SYNC_ROAD_TRACK_INFO*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRoadTrackInfo);

    pRoadTrackInfo->wProtocolID  = s2r_sync_road_track_force;
    pRoadTrackInfo->nNodeID      = nNodeID;
    pRoadTrackInfo->byCamp       = (BYTE)eCamp;
    pRoadTrackInfo->byEnable     = (BYTE)bEnable;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoLoadRoleDataRequest(DWORD dwRoleID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piPackage       = NULL;
    S2R_LOAD_ROLE_DATA_REQUEST* pLoadRoleData   = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_LOAD_ROLE_DATA_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pLoadRoleData               = (S2R_LOAD_ROLE_DATA_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pLoadRoleData);

    pLoadRoleData->wProtocolID  = s2r_load_role_data_request;
    pLoadRoleData->dwPlayerID   = dwRoleID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSyncRoleData(DWORD dwID, BYTE* pbyData, size_t uOffset, size_t uDataLen)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    IKG_Buffer*         piPackage       = NULL;
    S2R_SYNC_ROLE_DATA* pSyncRoleData   = NULL;
    
    piPackage = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_SYNC_ROLE_DATA) + uDataLen));
    KGLOG_PROCESS_ERROR(piPackage);

    pSyncRoleData = (S2R_SYNC_ROLE_DATA*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSyncRoleData);

    pSyncRoleData->wProtocolID  = s2r_sync_role_data;
    pSyncRoleData->dwRoleID     = dwID;
    pSyncRoleData->uOffset      = uOffset;

    memcpy(pSyncRoleData->byData, pbyData, uDataLen);

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSaveRoleData(KPlayer* pPlayer, size_t uRoleDataLen)
{
    BOOL                bResult       = false;
    BOOL                bRetCode      = false;
    IKG_Buffer*         piPackage     = NULL;
    S2R_SAVE_ROLE_DATA* pSaveRoleData = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SAVE_ROLE_DATA));
    KGLOG_PROCESS_ERROR(piPackage);

    pSaveRoleData = (S2R_SAVE_ROLE_DATA*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSaveRoleData);

    pSaveRoleData->wProtocolID  = s2r_save_role_data;
    pSaveRoleData->dwRoleID     = pPlayer->m_dwID;
    pSaveRoleData->nUserValue   = pPlayer->m_eGameStatus;
    pSaveRoleData->uRoleDataLen = uRoleDataLen;

    bRetCode = pPlayer->SaveBaseInfo(&pSaveRoleData->BaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoChangeRoleLevelRequest(KPlayer* pPlayer)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    IKG_Buffer*                     piPackage           = NULL;
    S2R_CHANGE_ROLE_LEVEL_REQUEST*  pChangeRoleLevel    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_CHANGE_ROLE_LEVEL_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pChangeRoleLevel = (S2R_CHANGE_ROLE_LEVEL_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pChangeRoleLevel);

    pChangeRoleLevel->wProtocolID = s2r_change_role_level_request;
    pChangeRoleLevel->dwPlayerID  = pPlayer->m_dwID;
    pChangeRoleLevel->byLevel     = (BYTE)pPlayer->m_nLevel;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoChangeRoleForceIDRequest(KPlayer* pPlayer)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    IKG_Buffer*                         piPackage           = NULL;
    S2R_CHANGE_ROLE_FORCEID_REQUEST*    pChangeRoleForceID  = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_CHANGE_ROLE_FORCEID_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pChangeRoleForceID = (S2R_CHANGE_ROLE_FORCEID_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pChangeRoleForceID);

    pChangeRoleForceID->wProtocolID = s2r_change_role_forceid_request;
    pChangeRoleForceID->dwPlayerID  = pPlayer->m_dwID;
    pChangeRoleForceID->byForceID   = (BYTE)pPlayer->m_dwForceID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSendGmMessage(const char cszGmName[], const char cszMessage[])
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    IKG_Buffer*             piPackage   = NULL;
    S2R_SEND_GM_MESSAGE*    pGmMessage  = NULL;
    size_t                  uStrLen     = 0;
    size_t                  uPakSize    = 0;

    assert(cszGmName);
    assert(cszMessage);

    uStrLen = strlen(cszMessage);
    uPakSize = sizeof(S2R_SEND_GM_MESSAGE) + uStrLen + sizeof('\0');

    piPackage = KG_MemoryCreateBuffer((unsigned int)uPakSize);
    KGLOG_PROCESS_ERROR(piPackage);

    pGmMessage = (S2R_SEND_GM_MESSAGE*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pGmMessage);

    pGmMessage->wProtocolID = s2r_send_gm_message;
    
    strncpy(pGmMessage->szGmName, cszGmName, sizeof(pGmMessage->szGmName));
    pGmMessage->szGmName[sizeof(pGmMessage->szGmName) - sizeof('\0')] = '\0';

    strncpy(pGmMessage->szMessage, cszMessage, uStrLen + sizeof('\0'));
    pGmMessage->szMessage[uStrLen] = '\0';

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSendGmChn(const char cszAccount[], const char cszRole[], const char cszMessage[])
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    IKG_Buffer*         piPackage   = NULL;
    S2R_SEND_GM_CHN*    pGmChn      = NULL;
    size_t              uStrLen     = 0;
    size_t              uPakSize    = 0;

    assert(cszAccount);
    assert(cszRole);
    assert(cszMessage);

    uStrLen = strlen(cszMessage);
    uPakSize = sizeof(S2R_SEND_GM_CHN) + uStrLen + sizeof('\0');

    piPackage = KG_MemoryCreateBuffer((unsigned int)uPakSize);
    KGLOG_PROCESS_ERROR(piPackage);

    pGmChn = (S2R_SEND_GM_CHN*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pGmChn);

    pGmChn->wProtocolID = s2r_send_gm_chn;

    strncpy(pGmChn->szAccount, cszAccount, sizeof(pGmChn->szAccount));
    pGmChn->szAccount[sizeof(pGmChn->szAccount) - 1] = '\0';

    strncpy(pGmChn->szRole, cszRole, sizeof(pGmChn->szRole));
    pGmChn->szRole[sizeof(pGmChn->szRole) - 1] = '\0';

    strncpy(pGmChn->szMessage, cszMessage, uStrLen + 1);
    pGmChn->szMessage[uStrLen] = '\0';

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSendGMCommandGS(const char cszRoleName[], const char cszCommand[])
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piPackage       = NULL;
    S2R_SEND_GM_COMMAND_GS* pGMCommandGS    = NULL;
    size_t                  uStrLen         = 0;
    size_t                  uPakSize        = 0;

    assert(cszRoleName);
    assert(cszCommand);

    uStrLen     = strlen(cszCommand);
    uPakSize    = sizeof(S2R_SEND_GM_COMMAND_GS) + uStrLen + sizeof('\0');

    piPackage = KG_MemoryCreateBuffer((unsigned int)uPakSize);
    KGLOG_PROCESS_ERROR(piPackage);

    pGMCommandGS = (S2R_SEND_GM_COMMAND_GS*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pGMCommandGS);

    pGMCommandGS->wProtocolID = s2r_send_gm_command_gs;

    strncpy(pGMCommandGS->szRoleName, cszRoleName, sizeof(pGMCommandGS->szRoleName));
    pGMCommandGS->szRoleName[sizeof(pGMCommandGS->szRoleName) - sizeof('\0')] = '\0';

    strncpy(pGMCommandGS->szCommand, cszCommand, uStrLen + sizeof('\0'));
    pGMCommandGS->szCommand[uStrLen] = '\0';

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSendGMCommandGC(const char cszCommand[])
{
    BOOL                    bResult      = false;
    BOOL                    bRetCode     = false;
    IKG_Buffer*             piPackage    = NULL;
    S2R_SEND_GM_COMMAND_GC* pGMCommandGC = NULL;
    size_t                  uStrLen      = 0;
    size_t                  uPakSize     = 0;

    assert(cszCommand);

    uStrLen     = strlen(cszCommand);
    uPakSize    = uStrLen + sizeof(S2R_SEND_GM_COMMAND_GC) + sizeof('\0');

    piPackage = KG_MemoryCreateBuffer((unsigned int)uPakSize);
    KGLOG_PROCESS_ERROR(piPackage);

    pGMCommandGC = (S2R_SEND_GM_COMMAND_GC*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pGMCommandGC);

    pGMCommandGC->wProtocolID = s2r_send_gm_command_gc;

    strncpy(pGMCommandGC->szCommand, cszCommand, uStrLen + sizeof('\0'));
    pGMCommandGC->szCommand[uStrLen] = '\0';

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoJoinBattleFieldQueueRequest(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin, DWORD dwJoinValue)
{
    BOOL                                 bResult        = false;
    BOOL                                 bRetCode       = false;
    IKG_Buffer*                          piPackage      = NULL;
    S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST* pJoinRequest   = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pJoinRequest = (S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pJoinRequest);

    pJoinRequest->wProtocolID = s2r_join_battle_field_queue_request;
    pJoinRequest->dwRoleID    = dwPlayerID;
    pJoinRequest->dwMapID     = dwMapID;
    pJoinRequest->nCopyIndex  = nCopyIndex;
    pJoinRequest->dwGroupID   = dwGroupID;
    pJoinRequest->bTeamJoin   = bTeamJoin;
    pJoinRequest->dwJoinValue = dwJoinValue;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoLeaveBattleFieldQueueRequest(DWORD dwPlayerID, DWORD dwMapID)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    IKG_Buffer*                             piPackage       = NULL;
    S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*   pLeaveRequest   = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pLeaveRequest = (S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pLeaveRequest);

    pLeaveRequest->wProtocolID  = s2r_leave_battle_field_queue_request;
    pLeaveRequest->dwMapID      = dwMapID;
    pLeaveRequest->dwRoleID     = dwPlayerID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoAcceptJoinBattleField(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piPackage   = NULL;
    S2R_ACCEPT_JOIN_BATTLE_FIELD*   pAcceptJoin = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ACCEPT_JOIN_BATTLE_FIELD));
    KGLOG_PROCESS_ERROR(piPackage);

    pAcceptJoin = (S2R_ACCEPT_JOIN_BATTLE_FIELD*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pAcceptJoin);

    pAcceptJoin->wProtocolID    = s2r_accept_join_battle_field;
    pAcceptJoin->dwRoleID       = dwPlayerID;
    pAcceptJoin->dwMapID        = dwMapID;
    pAcceptJoin->nCopyIndex     = nCopyIndex;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoGetBattleFieldList(DWORD dwPlayerID, DWORD dwMapID)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    IKG_Buffer*                 piPackage           = NULL;
    S2R_GET_BATTLE_FIELD_LIST*  pGetBattleFieldList = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GET_BATTLE_FIELD_LIST));
    KGLOG_PROCESS_ERROR(piPackage);

    pGetBattleFieldList = (S2R_GET_BATTLE_FIELD_LIST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pGetBattleFieldList);

    pGetBattleFieldList->wProtocolID    = s2r_get_battle_field_list;
    pGetBattleFieldList->dwPlayerID     = dwPlayerID;
    pGetBattleFieldList->dwMapID        = dwMapID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoLeaveBattleFieldRequest(DWORD dwPlayerID)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    IKG_Buffer*                     piPackage           = NULL;
    S2R_LEAVE_BATTLE_FIELD_REQUEST* pLeaveBattleField   = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_LEAVE_BATTLE_FIELD_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pLeaveBattleField = (S2R_LEAVE_BATTLE_FIELD_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pLeaveBattleField);

    pLeaveBattleField->wProtocolID = s2r_leave_battle_field_request;
    pLeaveBattleField->dwPlayerID  = dwPlayerID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyCreatePQ(DWORD dwPQTemplateID)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    S2R_APPLY_CREATE_PQ_REQUEST*    pApplyCreatePQ  = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_CREATE_PQ_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyCreatePQ = (S2R_APPLY_CREATE_PQ_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyCreatePQ);

    pApplyCreatePQ->wProtocolID     = s2r_apply_create_pq_request;
    pApplyCreatePQ->dwPQTemplateID  = dwPQTemplateID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyDeletePQ(DWORD dwPQID)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    S2R_APPLY_DELETE_PQ_REQUEST*    pApplyDeletePQ  = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_DELETE_PQ_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyDeletePQ = (S2R_APPLY_DELETE_PQ_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyDeletePQ);

    pApplyDeletePQ->wProtocolID = s2r_apply_delete_pq_request;
    pApplyDeletePQ->dwPQID      = dwPQID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyChangePQValue(DWORD dwPQID, int nIndex, int nAddValue)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    IKG_Buffer*                         piPackage           = NULL;
    S2R_APPLY_CHANGE_PQ_VALUE_REQUEST*  pApplyChangePQValue = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_CHANGE_PQ_VALUE_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyChangePQValue = (S2R_APPLY_CHANGE_PQ_VALUE_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyChangePQValue);

    pApplyChangePQValue->wProtocolID    = s2r_apply_change_pq_value_request;
    pApplyChangePQValue->dwPQID         = dwPQID;
    pApplyChangePQValue->nIndex         = nIndex;
    pApplyChangePQValue->nAddValue      = nAddValue;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoAddCampScore(int nAddCampScore)
{
    BOOL                        bResult        = false;
    BOOL                        bRetCode       = false;
    IKG_Buffer*                 piPackage      = NULL;
    S2R_ADD_CAMP_SCORE*         pAddCampScore  = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_ADD_CAMP_SCORE));
    KGLOG_PROCESS_ERROR(piPackage);

    pAddCampScore = (S2R_ADD_CAMP_SCORE*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pAddCampScore);

    pAddCampScore->wProtocolID    = s2r_add_camp_score;
    pAddCampScore->nAddCampScore  = nAddCampScore;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplySetCampRequest(DWORD dwPlayerID, KCAMP eNewCamp)
{
    BOOL                        bResult                = false;
    BOOL                        bRetCode               = false;
    IKG_Buffer*                 piPackage              = NULL;
    S2R_APPLY_SET_CAMP_REQUEST* pApplySetCampRequest   = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_SET_CAMP_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplySetCampRequest = (S2R_APPLY_SET_CAMP_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplySetCampRequest);

    pApplySetCampRequest->wProtocolID  = s2r_apply_set_camp_request;
    pApplySetCampRequest->dwPlayerID   = dwPlayerID;
    pApplySetCampRequest->byNewCamp    = (BYTE)eNewCamp;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyTongRosterRequest(DWORD dwPlayerID, int nLastUpdateFrame, DWORD dwMemberList[], int nMemberCount)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    S2R_APPLY_TONG_ROSTER_REQUEST*  pRequest        = NULL;

    KGLOG_PROCESS_ERROR(nMemberCount >= 0);
    KGLOG_PROCESS_ERROR(nMemberCount <= MAX_TONG_MEMBER_COUNT);

    piPackage =  KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_APPLY_TONG_ROSTER_REQUEST) + sizeof(DWORD) * nMemberCount));
    KGLOG_PROCESS_ERROR(piPackage);

    pRequest  = (S2R_APPLY_TONG_ROSTER_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_apply_tong_roster_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->nLastUpdateFrame  = nLastUpdateFrame;
    pRequest->nMemberCount      = nMemberCount;
    
    if (nMemberCount > 0)
    {
        memcpy(pRequest->dwMemberList, dwMemberList, sizeof(DWORD) * nMemberCount);
    }

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyTongInfoRequest(DWORD dwPlayerID, int nLastUpdateFrame)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    S2R_APPLY_TONG_INFO_REQUEST*    pApplyRequest   = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_TONG_INFO_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyRequest = (S2R_APPLY_TONG_INFO_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyRequest);

    pApplyRequest->wProtocolID      = s2r_apply_tong_info_request;
    pApplyRequest->dwPlayerID       = dwPlayerID;
    pApplyRequest->nLastUpdateFrame = nLastUpdateFrame;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyCreateTongRequest(DWORD dwPlayerID, const char cszTongName[], int nTemplateIndex)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    S2R_APPLY_CREATE_TONG_REQUEST*  pApplyRequest   = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_CREATE_TONG_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyRequest = (S2R_APPLY_CREATE_TONG_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyRequest);

    pApplyRequest->wProtocolID      = s2r_apply_create_tong_request;
    pApplyRequest->dwPlayerID       = dwPlayerID;
    pApplyRequest->byTemplateIndex  = (BYTE)nTemplateIndex;

    strncpy(pApplyRequest->szTongName, cszTongName, sizeof(pApplyRequest->szTongName));
    pApplyRequest->szTongName[sizeof(pApplyRequest->szTongName) - 1] = '\0';

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoInvitePlayerJoinTongRequest(DWORD dwPlayerID, const char cszNewMemberName[])
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    IKG_Buffer*                             piPackage       = NULL;
    S2R_INVITE_PLAYER_JOIN_TONG_REQUEST*    pInvitRequest   = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_INVITE_PLAYER_JOIN_TONG_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pInvitRequest = (S2R_INVITE_PLAYER_JOIN_TONG_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pInvitRequest);

    pInvitRequest->wProtocolID      = s2r_invite_player_join_tong_request;
    pInvitRequest->dwPlayerID       = dwPlayerID;

    strncpy(pInvitRequest->szNewMemberName, cszNewMemberName, sizeof(pInvitRequest->szNewMemberName));
    pInvitRequest->szNewMemberName[sizeof(pInvitRequest->szNewMemberName) - 1] = '\0';

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoInvitePlayerJoinTongRespond(DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, INVITE_TONG_RESULT_CODE eAccept)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    IKG_Buffer*                             piPackage   = NULL;
    S2R_INVITE_PLAYER_JOIN_TONG_RESPOND*    pRespond    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_INVITE_PLAYER_JOIN_TONG_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pRespond = (S2R_INVITE_PLAYER_JOIN_TONG_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID   = s2r_invite_player_join_tong_respond;
    pRespond->dwPlayerID    = dwPlayerID;
    pRespond->dwInviterID   = dwInviterID;
    pRespond->dwTongID      = dwTongID;
    pRespond->byAccept      = (BYTE)eAccept;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyKickOutTongMemberRequest(DWORD dwPlayerID, DWORD dwMemberID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piPackage   = NULL;
    S2R_APPLY_KICK_OUT_TONG_MEMBER* pRequest    = NULL;   

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_KICK_OUT_TONG_MEMBER));
    KGLOG_PROCESS_ERROR(piPackage);

    pRequest = (S2R_APPLY_KICK_OUT_TONG_MEMBER*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID = s2r_apply_kick_out_tong_member_request;
    pRequest->dwPlayerID  = dwPlayerID;
    pRequest->dwMemberID  = dwMemberID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoModifyTongInfoRequest(DWORD dwPlayerID, BYTE byInfoType, BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    S2R_MODIFY_TONG_INFO_REQUEST*   pRequest    = NULL;

    KGLOG_PROCESS_ERROR(byInfoType > tmrBegin && byInfoType < tmrEnd);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_MODIFY_TONG_INFO_REQUEST) + uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_MODIFY_TONG_INFO_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_modify_tong_info_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->byInfoType    = byInfoType;
    pRequest->wDataLen      = (WORD)uDataLen;

    memcpy(pRequest->byData, pbyData, uDataLen);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoModifyTongSchemaRequest(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byModifyType, BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    S2R_MODIFY_TONG_SCHEMA_REQUEST* pRequest    = NULL;

    KGLOG_PROCESS_ERROR(byModifyType > tmsBegin && byModifyType < tmsEnd);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_MODIFY_TONG_SCHEMA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_modify_tong_schema_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->byGroupIndex  = byGroupIndex;
    pRequest->byModifyType  = byModifyType;

    memcpy(pRequest->byData, pbyData, uDataLen);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyQuitTongRequest(DWORD dwPlayerID)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    S2R_APPLY_QUIT_TONG_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_QUIT_TONG_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_APPLY_QUIT_TONG_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID = s2r_apply_quit_tong_request;
    pRequest->dwPlayerID  = dwPlayerID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoChangeTongMemberGroupRequest(DWORD dwPlayerID, DWORD dwTargetMemberID, int nTargetGroup)
{
    BOOL                                    bResult  = false;
    BOOL                                    bRetCode = false;
    IKG_Buffer*                             piBuffer = NULL;
    S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_change_tong_member_group_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->dwTargetMemberID  = dwTargetMemberID;
    pRequest->byTargetGroup     = (BYTE)nTargetGroup;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoChangeTongMasterRequest(DWORD dwPlayerID, DWORD dwNewMasterID)
{
    BOOL                             bResult    = false;
    BOOL                             bRetCode   = false;
    IKG_Buffer*                      piBuffer   = NULL;
    S2R_CHANGE_TONG_MASTER_REQUEST*  pRequest   = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_TONG_MASTER_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_CHANGE_TONG_MASTER_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_change_tong_master_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwNewMasterID = dwNewMasterID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoChangeTongMemberRemarkRequest(DWORD dwPlayerID, DWORD dwTargetMemberID, char szRemark[])
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    IKG_Buffer*                             piBuffer    = NULL;
    S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST*  pRequest    = NULL; 

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_change_tong_member_remark_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->dwTargetMemberID  = dwTargetMemberID;

    strncpy(pRequest->szRemark, szRemark, sizeof(pRequest->szRemark));
    pRequest->szRemark[sizeof(pRequest->szRemark) - 1] = '\0';

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetTongDescriptionRequest(DWORD dwPlayerID, DWORD dwTongID)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    S2R_GET_TONG_DESCRIPTION_REQUEST*   pRequest    = NULL;
    IKG_Buffer*                         piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GET_TONG_DESCRIPTION_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_GET_TONG_DESCRIPTION_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_get_tong_description_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwTongID      = dwTongID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoSaveMoneyInTongRequest(DWORD dwPlayerID, int nSaveMoney)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_SAVE_MONEY_IN_TONG_REQUEST* pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SAVE_MONEY_IN_TONG_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_SAVE_MONEY_IN_TONG_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_save_money_in_tong_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->nMoney        = nSaveMoney;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoPayTongSalaryRequest(DWORD dwPlayerID, int nTotalSalary)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_PAY_TONG_SALARY_REQUEST*    pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_PAY_TONG_SALARY_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_PAY_TONG_SALARY_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_pay_tong_salary_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->nTotalSalary  = nTotalSalary;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetTongSalaryRequest(DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_GET_TONG_SALARY_REQUEST*    pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GET_TONG_SALARY_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_GET_TONG_SALARY_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_get_tong_salary_request;
    pRequest->dwPlayerID    = dwPlayerID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetTongSalaryFailRespond(DWORD dwPlayerID, DWORD dwTongID, int nSilver)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    S2R_GET_TONG_SALARY_FAIL_RESPOND*   pRespond    = NULL;
    IKG_Buffer*                         piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GET_TONG_SALARY_FAIL_RESPOND));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (S2R_GET_TONG_SALARY_FAIL_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID   = s2r_get_tong_salary_fail_respond;
    pRespond->dwPlayerID    = dwPlayerID;
    pRespond->dwTongID      = dwTongID;
    pRespond->nSilver       = nSilver;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoChangeTongCampRequest(DWORD dwPlayerID, KCAMP eCamp)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_CHANGE_TONG_CAMP_REQUEST*   pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_TONG_CAMP_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_CHANGE_TONG_CAMP_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_change_tong_camp_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->byCamp        = (BYTE)eCamp;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyTongRepertoryPageRequest(DWORD dwPlayerID, BYTE byPageIndex, int nLastUpdateCounter)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    IKG_Buffer*                             piPackage   = NULL;
    S2R_APPLY_TONG_REPERTORY_PAGE_REQUEST*  pApply      = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_TONG_REPERTORY_PAGE_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pApply = (S2R_APPLY_TONG_REPERTORY_PAGE_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApply);

    pApply->wProtocolID         = s2r_apply_tong_repertory_page_request;
    pApply->dwPlayerID          = dwPlayerID;
    pApply->byPageIndex         = byPageIndex;
    pApply->nLastUpdateCounter  = nLastUpdateCounter;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoApplyOpenTongRepertoryRequest(DWORD dwPlayerID)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    S2R_APPLY_OPEN_TONG_REPERTORY_REQUEST*  pRequest    = NULL;
    IKG_Buffer*                             piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_OPEN_TONG_REPERTORY_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_APPLY_OPEN_TONG_REPERTORY_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_apply_open_tong_repertory_request;
    pRequest->dwPlayerID    = dwPlayerID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoTakeTongRepertoryItemToPosRequest(
    DWORD dwPlayerID, BYTE byPageIndex, BYTE byRepertoryPos,
    BYTE byBoxIndex, BYTE byBoxPos, int nLastUpdateCounter
)
{
    BOOL                                            bResult     = false;
    BOOL                                            bRetCode    = false;
    S2R_TAKE_TONG_REPERTORY_ITEM_TO_POS_REQUEST*    pRequest    = NULL;
    IKG_Buffer*                                     piBuffer    = NULL;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_TAKE_TONG_REPERTORY_ITEM_TO_POS_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_TAKE_TONG_REPERTORY_ITEM_TO_POS_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_take_tong_repertory_item_to_pos_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->byPageIndex       = byPageIndex;
    pRequest->byRepertoryPos    = byRepertoryPos;
    pRequest->byBoxIndex        = byBoxIndex;
    pRequest->byBoxPos          = byBoxPos;
    pRequest->nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult= true;
Exit0:
	KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoTakeTongRepertoryItemRequest(DWORD dwPlayerID, BYTE byPageIndex, BYTE byRepertoryPos, int nLastUpdateCounter)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    S2R_TAKE_TONG_REPERTORY_ITEM_REQUEST*   pRequest    = NULL;
    IKG_Buffer*                             piBuffer    = NULL;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_TAKE_TONG_REPERTORY_ITEM_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_TAKE_TONG_REPERTORY_ITEM_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_take_tong_repertory_item_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->byPageIndex       = byPageIndex;
    pRequest->byRepertoryPos    = byRepertoryPos;
    pRequest->nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult= true;
Exit0:
	KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoTakeTongRepertoryItemRespond(
    DWORD dwPlayerID, DWORD dwTongID, BYTE byPageIndex, BYTE byRepertoryPos, BOOL bSuccess
)
{
    BOOL                                    bResult  = false;
    BOOL                                    bRetCode = false;
    S2R_TAKE_TONG_REPERTORY_ITEM_RESPOND*   pRespond = NULL;
    IKG_Buffer*                             piBuffer = NULL;
        
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_TAKE_TONG_REPERTORY_ITEM_RESPOND));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (S2R_TAKE_TONG_REPERTORY_ITEM_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID     = s2r_take_tong_repertory_item_respond;
    pRespond->dwPlayerID      = dwPlayerID;
    pRespond->dwTongID        = dwTongID;
    pRespond->byPageIndex     = byPageIndex;
    pRespond->byRepertoryPos  = byRepertoryPos;
    pRespond->bSuccess        = (bool)bSuccess;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = false;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoPutTongRepertoryItemToPosRequest(
    DWORD dwPlayerID, DWORD dwItemID, BYTE byBoxIndex, BYTE byBoxPos, BYTE byPageIndex, BYTE byPagePos
)
{
    BOOL                                        bResult     = false;
    BOOL                                        bRetCode    = false;
    S2R_PUT_TONG_REPERTORY_ITEM_TO_POS_REQUEST* pRequest    = NULL;
    IKG_Buffer*                                 piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_PUT_TONG_REPERTORY_ITEM_TO_POS_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_PUT_TONG_REPERTORY_ITEM_TO_POS_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_put_tong_repertory_item_to_pos_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->dwServerItemID    = dwItemID;
    pRequest->byBoxIndex        = byBoxIndex;
    pRequest->byBoxPos          = byBoxPos;
    pRequest->byPageIndex       = byPageIndex;
    pRequest->byPagePos         = byPagePos;
    
    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoPutTongRepertoryItemRequest(DWORD dwPlayerID, DWORD dwItemID, BYTE byBoxIndex, BYTE byBoxPos, BYTE byPageIndex)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    S2R_PUT_TONG_REPERTORY_ITEM_REQUEST*    pRequest    = NULL;
    IKG_Buffer*                             piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_PUT_TONG_REPERTORY_ITEM_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_PUT_TONG_REPERTORY_ITEM_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_put_tong_repertory_item_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->dwServerItemID    = dwItemID;
    pRequest->byBoxIndex        = byBoxIndex;
    pRequest->byBoxPos          = byBoxPos;
    pRequest->byPageIndex       = byPageIndex;
    
    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoPutTongRepertoryItemRespond(KItem* pItem, DWORD dwPlayerID, DWORD dwTongID, BYTE byPageIndex, BYTE byPagePos)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    S2R_PUT_TONG_REPERTORY_ITEM_RESPOND*    pRespond    = NULL;
    IKG_Buffer*                             piBuffer    = NULL;
    IKG_Buffer_ReSize*                      piReSize    = NULL;
    size_t                                  uDataSize   = 0;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_PUT_TONG_REPERTORY_ITEM_RESPOND) + 1024);
    KGLOG_PROCESS_ERROR(piBuffer);
    piBuffer->QueryInterface(IID_IKG_Buffer_ReSize, (void**)&piReSize);
    KGLOG_PROCESS_ERROR(piReSize);

    pRespond = (S2R_PUT_TONG_REPERTORY_ITEM_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    bRetCode = pItem->GetBinaryData(&uDataSize, pRespond->byData, 1024);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piReSize->SetSmallerSize((unsigned)(sizeof(S2R_PUT_TONG_REPERTORY_ITEM_RESPOND) + uDataSize));
    KGLOG_PROCESS_ERROR(bRetCode);

    pRespond->wProtocolID   = s2r_put_tong_repertory_item_respond;
    pRespond->dwPlayerID    = dwPlayerID;
    pRespond->dwTongID      = dwTongID;
    pRespond->byPageIndex   = byPageIndex;
    pRespond->byPagePos     = byPagePos;
    pRespond->byDataSize    = (BYTE)uDataSize;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    KG_COM_RELEASE(piReSize);
    return bResult;
}

BOOL KRelayClient::DoUnlockTongRepertoryGrid(DWORD dwTongID, BYTE byPageIndex, BYTE byPagePos)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_UNLOCK_TONG_REPERTORY_GRID* pRespond    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_UNLOCK_TONG_REPERTORY_GRID));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (S2R_UNLOCK_TONG_REPERTORY_GRID*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID = s2r_unlock_tong_repertory_grid;
    pRespond->dwTongID    = dwTongID;
    pRespond->byPageIndex = byPageIndex;
    pRespond->byPagePos   = byPagePos;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoExchangeTongRepertoryItemPosRequest(
    DWORD dwPlayerID, BYTE bySrcPageIndex, BYTE bySrcPagePos, int nSrcUpdateCounter,
    BYTE byDstPageIndex, BYTE byDstPagePos, int nDstUpdateCounter
)
{
    BOOL                                            bResult     = false;
    BOOL                                            bRetCode    = false;
    IKG_Buffer*                                     piBuffer    = NULL;
    S2R_EXCHANGE_TONG_REPERTORY_ITEM_POS_REQUEST*   pRequest    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_EXCHANGE_TONG_REPERTORY_ITEM_POS_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_EXCHANGE_TONG_REPERTORY_ITEM_POS_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_exchange_tong_repertory_item_pos_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->bySrcPageIndex    = bySrcPageIndex;
    pRequest->bySrcPagePos      = bySrcPagePos;
    pRequest->byDstPageIndex    = byDstPageIndex;
    pRequest->byDstPagePos      = byDstPagePos;
    pRequest->nSrcUpdateCounter = nSrcUpdateCounter;
    pRequest->nDstUpdateCounter = nDstUpdateCounter;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyStackTongRepertoryItemRequest(
    DWORD dwPlayerID, DWORD dwTongID,
    BYTE byPageIndex, BYTE byPagePos, BYTE byBoxIndex, BYTE byBoxPos, DWORD dwItemID,
    BYTE byTabType, WORD wTabIndex, WORD wStackNum, WORD wMaxStackNum, int nLastUpdateCounter
)
{
    BOOL                                            bResult     = false;
    BOOL                                            bRetCode    = false;
    IKG_Buffer*                                     piBuffer    = NULL;
    S2R_APPLY_STACK_TONG_REPERTORY_ITEM_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_STACK_TONG_REPERTORY_ITEM_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_APPLY_STACK_TONG_REPERTORY_ITEM_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_apply_stack_tong_repertory_item_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwTongID      = dwTongID;
    pRequest->byPageIndex   = byPageIndex;
    pRequest->byPagePos     = byPagePos;
    pRequest->byBoxIndex    = byBoxIndex;
    pRequest->byBoxPos      = byBoxPos;
    pRequest->dwItemID      = dwItemID;
    pRequest->byTabType     = byTabType;
    pRequest->wTabIndex     = wTabIndex;
    pRequest->wStackNum     = wStackNum;
    pRequest->wMaxStackNum  = wMaxStackNum;
    pRequest->nLastUpdateCounter = nLastUpdateCounter;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoStackTongRepertoryItemRequest(
    DWORD dwPlayerID, DWORD dwTongID,
    BYTE byPageIndex, BYTE byPagePos, BYTE byTabType, WORD wTabIndex,
    WORD wStackNum, WORD wMaxStackNum
)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    IKG_Buffer*                             piBuffer    = NULL;
    S2R_STACK_TONG_REPERTORY_ITEM_REQUEST*  pRequest = NULL;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_STACK_TONG_REPERTORY_ITEM_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_STACK_TONG_REPERTORY_ITEM_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_stack_tong_repertory_item_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwTongID      = dwTongID;
    pRequest->byPageIndex   = byPageIndex;
    pRequest->byPagePos     = byPagePos;
    pRequest->byTabType     = byTabType;
    pRequest->wTabIndex     = wTabIndex;
    pRequest->wStackNum     = wStackNum;
    pRequest->wMaxStackNum  = wMaxStackNum;
    
    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAuctionLookupRequest(BYTE byRequestID, DWORD dwPlayerID, KAUCTION_LOOKUP_PARAM* pParam)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    S2R_AUCTION_LOOKUP_REQUEST* pRequest = NULL;

    assert(pParam);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_LOOKUP_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);
    
    pRequest = (S2R_AUCTION_LOOKUP_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_auction_lookup_request;
    pRequest->byRequestID   = byRequestID;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->Param         = *pParam;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAuctionBidRequest(DWORD dwBidderID, DWORD dwSaleID, DWORD dwCRC, int nPrice)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    S2R_AUCTION_BID_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_BID_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_AUCTION_BID_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_auction_bid_request;
    pRequest->dwBidderID    = dwBidderID;
    pRequest->dwSaleID      = dwSaleID;
    pRequest->dwCRC         = dwCRC;
    pRequest->nPrice        = nPrice;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAuctionSellRequest(
    DWORD dwSellerID, char* pszSaleName, int nAucGenre, int nAucSub, int nRequireLevel, int nQuality, 
    int nStartPrice, int nBuyItNowPrice, int nCustodyCharges, int nLeftTime, size_t uItemDataLen, BYTE* pbyItemData
)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    S2R_AUCTION_SELL_REQUEST*   pRequest = NULL;

    assert(pszSaleName);
    assert(pbyItemData);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_AUCTION_SELL_REQUEST) + uItemDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_AUCTION_SELL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_auction_sell_request;

    pRequest->dwSellerID        = dwSellerID;

    strncpy(pRequest->szSaleName, pszSaleName, sizeof(pRequest->szSaleName));
    pRequest->szSaleName[sizeof(pRequest->szSaleName) - 1] = '\0';

    pRequest->nAucGenre         = nAucGenre;
    pRequest->nAucSub           = nAucSub;
    pRequest->nRequireLevel     = nRequireLevel;
    pRequest->nQuality          = nQuality;
    pRequest->nStartPrice       = nStartPrice;
    pRequest->nBuyItNowPrice    = nBuyItNowPrice;
    pRequest->nCustodyCharges   = nCustodyCharges;
    pRequest->nLeftTime         = nLeftTime;
    pRequest->uItemDataLen      = uItemDataLen;
    
    memcpy(pRequest->byItemData, pbyItemData, uItemDataLen);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAuctionCancelRequest(DWORD dwPlayerID, DWORD dwSaleID)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    S2R_AUCTION_CANCEL_REQUEST* pRequest = NULL;

    KGLOG_PROCESS_ERROR(dwPlayerID);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_CANCEL_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_AUCTION_CANCEL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_auction_cancel_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwSaleID      = dwSaleID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoRemoteLuaCall(const char cszFunction[], BYTE* pbyParam, size_t uParamLen)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piBuffer        = NULL;
    S2R_REMOTE_LUA_CALL*    pRequest        = NULL;
    size_t                  uFuncNameSize   = 0;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_REMOTE_LUA_CALL) + uParamLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_REMOTE_LUA_CALL*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_remote_lua_call;
    pRequest->uParamLen     = uParamLen;
    
    uFuncNameSize = strlen(cszFunction) + 1;
    KGLOG_PROCESS_ERROR(uFuncNameSize <= sizeof(pRequest->szFunction));

    memcpy(pRequest->szFunction, cszFunction, uFuncNameSize);
    memcpy(pRequest->byParam, pbyParam, uParamLen);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAchievementGlobalAnnounceRequest(DWORD dwPlayerID, int nAchievementID)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    S2R_ACHIEVEMENT_ANNOUNCE_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ACHIEVEMENT_ANNOUNCE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_ACHIEVEMENT_ANNOUNCE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_achievement_global_announce_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->nAchievementID    = nAchievementID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoDesignationGlobalAnnounceRequest(DWORD dwPlayerID, int nPrefix, int nPostfix)
{
    BOOL bResult                                        = false;
    BOOL bRetCode                                       = false;
    IKG_Buffer* piBuffer                                = NULL;
    S2R_DESIGNATION_GLOBAL_ANNOUNCE_REQUEST* pRequest   = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_DESIGNATION_GLOBAL_ANNOUNCE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_DESIGNATION_GLOBAL_ANNOUNCE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID = s2r_designation_global_announce_request;
    pRequest->dwPlayerID  = dwPlayerID;
    pRequest->nPrefix     = nPrefix;
    pRequest->nPostfix    = nPostfix;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoUpdateStatDataRequest()
{
    BOOL                            	bResult  = false;
    BOOL                            	bRetCode = false;
    HRESULT                             hRetCode = E_FAIL;
    IKG_Buffer*                     	piBuffer = NULL;
    IKG_Buffer_ReSize*                  piResize = NULL;
    S2R_UPDATE_STAT_DATA_REQUEST*	    pRequest = NULL;
    int                                 nCount   = 0;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(S2R_UPDATE_STAT_DATA_REQUEST) + sizeof(KSTAT_DATA_MODIFY) * MAX_UPDATE_STAT_COUNT));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_UPDATE_STAT_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);
    
    pRequest->wProtocolID  = s2r_update_stat_data_request;

    nCount = g_pSO3World->m_StatDataServer.GetUpdateStatData(pRequest->Values, MAX_UPDATE_STAT_COUNT);
    KG_PROCESS_ERROR(nCount > 0);

    pRequest->nCount = nCount;

    hRetCode = piBuffer->QueryInterface(IID_IKG_Buffer_ReSize, (void**)&piResize);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    bRetCode = piResize->SetSmallerSize((unsigned)(sizeof(S2R_UPDATE_STAT_DATA_REQUEST) + sizeof(KSTAT_DATA_MODIFY) * nCount));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piResize);
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoQueryStatIDRequest(const char cszName[])
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    S2R_QUERY_STAT_ID_REQUEST*          pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_QUERY_STAT_ID_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_QUERY_STAT_ID_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID = s2r_query_stat_id_request;

    strncpy(pRequest->szName, cszName, sizeof(pRequest->szName));
    pRequest->szName[sizeof(pRequest->szName) - 1] = '\0';

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoChangeExtPointRequest(DWORD dwPlayerID, WORD wExtPointIndex, WORD wChangeValue)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    S2R_CHANGE_EXT_POINT_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_EXT_POINT_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_CHANGE_EXT_POINT_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_change_ext_point_request;
    pRequest->dwPlayerID        = dwPlayerID;
    pRequest->wExtPointIndex    = wExtPointIndex;
    pRequest->wChangeValue      = wChangeValue;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoSetChargeFlagRequest(KPlayer* pPlayer, BOOL bChargeFlag)
{
    BOOL                            bResult     = false;
    int                             nRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    S2R_SET_CHARGE_FLAG_REQUEST*    pRequest    = NULL;
    KConnectionData*                pConnection = NULL;
    u_short                         uClientPort = 0;
    struct in_addr                  ClientIP;

    assert(pPlayer);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SET_CHARGE_FLAG_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_SET_CHARGE_FLAG_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pConnection = g_PlayerServer.GetConnection(pPlayer->m_nConnIndex);
    KGLOG_PROCESS_ERROR(pConnection);
    KGLOG_PROCESS_ERROR(pConnection->piSocketStream);

    nRetCode = pConnection->piSocketStream->GetRemoteAddress(&ClientIP, &uClientPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    pRequest->wProtocolID       = s2r_set_charge_flag_request;
    pRequest->dwPlayerID        = pPlayer->m_dwID;
    pRequest->dwLastLoginIP     = ClientIP.s_addr;
    pRequest->nChargeFlag       = bChargeFlag ? 1 : 0;

    nRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoMapCopyKeepPlayer(KPlayer* pPlayer)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    S2R_MAP_COPY_KEEP_PLAYER*           pRequest = NULL;

    assert(pPlayer->m_pScene);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_MAP_COPY_KEEP_PLAYER));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_MAP_COPY_KEEP_PLAYER*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID = s2r_map_copy_keep_player;

    pRequest->dwMapID       = pPlayer->m_pScene->m_dwMapID;
    pRequest->nCopyIndex    = pPlayer->m_pScene->m_nCopyIndex;
    pRequest->dwPlayerID    = pPlayer->m_dwID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoActivePresentCodeRequest(KPlayer* pPlayer, const char* pszPresentCode)
{
    BOOL                                bResult  = false;
    int                                 nRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    S2R_ACTIVE_PRESENT_CODE_REQUEST*    pRequest = NULL;
    KConnectionData*                    pConnection = NULL;
    u_short                             uClientPort = 0;
    struct in_addr                      ClientIP;

    assert(pPlayer);
    assert(pszPresentCode);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ACTIVE_PRESENT_CODE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_ACTIVE_PRESENT_CODE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pConnection = g_PlayerServer.GetConnection(pPlayer->m_nConnIndex);
    KGLOG_PROCESS_ERROR(pConnection);
    KGLOG_PROCESS_ERROR(pConnection->piSocketStream);

    nRetCode = pConnection->piSocketStream->GetRemoteAddress(&ClientIP, &uClientPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    pRequest->wProtocolID   = s2r_active_present_code_request;
    pRequest->dwPlayerID    = pPlayer->m_dwID;
    pRequest->dwActiveIP    = ClientIP.s_addr;

    strncpy(pRequest->szPresentCode, pszPresentCode, sizeof(pRequest->szPresentCode));
    pRequest->szPresentCode[sizeof(pRequest->szPresentCode) - 1] = '\0';

    nRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoApexProtocol(DWORD dwPlayerID, BOOL bIsUserData, BYTE* pbyData, size_t nDataSize)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    S2R_APEX_PROTOCOL*  pApexProtocol   = NULL;
    IKG_Buffer*         piBuffer        = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APEX_PROTOCOL) + nDataSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pApexProtocol = (S2R_APEX_PROTOCOL*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pApexProtocol);

    pApexProtocol->wProtocolID  = s2r_apex_protocol;
    pApexProtocol->dwRoleID     = dwPlayerID;
    pApexProtocol->byIsUserData = (BYTE)bIsUserData;
    pApexProtocol->nSize        = nDataSize;
    memcpy(pApexProtocol->byData, pbyData, nDataSize);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoReportFarmerPlayer(DWORD dwPlayerID, time_t nPunishTime)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    IKG_Buffer*                         piBuffer    = NULL;
    S2R_REPORT_FARMER_PLAYER_REQUEST*   pRequest    = NULL;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_REPORT_FARMER_PLAYER_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_REPORT_FARMER_PLAYER_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_report_farmer_player_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->nPunishTime   = nPunishTime;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGameCardSellRequest(
    DWORD dwPlayerID, int nCoin, int nGameTime, BYTE byType, int nPrice, int nDurationTime
)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2R_GAME_CARD_SELL_REQUEST* pRequest    = NULL;
    IKG_Buffer*                 piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GAME_CARD_SELL_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_GAME_CARD_SELL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_game_card_sell_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->nCoin         = nCoin;
    pRequest->nGameTime     = nGameTime;
    pRequest->byType        = byType;
    pRequest->nPrice        = nPrice;
    pRequest->nDurationTime = nDurationTime;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGameCardBuyRequest(DWORD dwPlayerID, DWORD dwID, int nBuyPrice)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    S2R_GAME_CARD_BUY_REQUEST*  pRequest    = NULL;
    IKG_Buffer*                 piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GAME_CARD_BUY_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_GAME_CARD_BUY_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_game_card_buy_request;
    pRequest->dwBuyerID     = dwPlayerID;
    pRequest->dwID          = dwID;
    pRequest->nBuyPrice     = nBuyPrice;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGameCardLookupRequest(DWORD dwPlayerID, BYTE byCardType, int nStartIndex, BYTE byOrderType, BOOL bDesc)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_GAME_CARD_LOOKUP_REQUEST*   pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GAME_CARD_LOOKUP_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_GAME_CARD_LOOKUP_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_game_card_lookup_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->byCardType    = byCardType;
    pRequest->nStartIndex   = nStartIndex;
    pRequest->byOrderType   = byOrderType;
    pRequest->bDesc         = bDesc;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoGameCardCancelRequest(DWORD dwPlayerID, DWORD dwID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_GAME_CARD_CANCEL_REQUEST*   pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GAME_CARD_CANCEL_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_GAME_CARD_CANCEL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_game_card_cancel_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwID          = dwID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoApplyMentorData(DWORD dwMentorID, DWORD dwApprenticeID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_APPLY_MENTOR_DATA*          pRequest    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_MENTOR_DATA));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_APPLY_MENTOR_DATA*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID       = s2r_apply_mentor_data;
    pRequest->dwMentorID        = dwMentorID;
    pRequest->dwApprenticeID    = dwApprenticeID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoUpdateMaxApprenticeNum(DWORD dwMentorID, int nMaxApprenticeNum)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_UPDATE_MAX_APPRENTICE_NUM*  pPak        = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_UPDATE_MAX_APPRENTICE_NUM));
    KGLOG_PROCESS_ERROR(piBuffer);

    pPak = (S2R_UPDATE_MAX_APPRENTICE_NUM*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_update_max_apprentice_num;
    pPak->dwMentorID        = dwMentorID;
    pPak->nMaxApprenticeNum = nMaxApprenticeNum;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAddMentorValueRequest(DWORD dwMentorID, DWORD dwApprenticeID, int nDeltaValue)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    S2R_ADD_MENTOR_VALUE_REQUEST*   pPak        = NULL;
    IKG_Buffer*                     piBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ADD_MENTOR_VALUE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pPak = (S2R_ADD_MENTOR_VALUE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_add_mentor_value_request;
    pPak->dwMentorID        = dwMentorID;
    pPak->dwApprenticeID    = dwApprenticeID;
    pPak->nDeltaVlue        = nDeltaValue;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}
//AutoCode:-发送协议函数结束-
#endif	//_SERVER
