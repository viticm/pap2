#include "stdafx.h"
#include "Engine/KG_CreateGUID.h"
#include "KGameServer.h"
#include "KSO3GameCenter.h"
#include "KMapManager.h"
#include "SO3Result.h"
#include "KFellowshipCenter.h"
#include "KRoleDB.h"
#include "KBattleFieldManager.h"

#define MAP_OPERATION_OVERTIME	120	// 地图创建或者删除的超时时间（单位：秒）

extern int g_nTestTime;
extern int g_nTestConnection;

KGameServer::KGameServer(void)
{
    m_nLastConnIndex                    = 1;
    m_bGameWorldProtocolValidFlag       = false;
    m_nGameWorldLowerProtocolVersion    = 0;
    m_nGameWorldUpperProtocolVersion    = 0;
    m_nConnectionTimeout                = 0;

    memset(m_GSList, 0, sizeof(m_GSList));
    memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
    memset(m_uProtocolSize, 0, sizeof(m_uProtocolSize));

	REGISTER_INTERNAL_FUNC(s2r_handshake_request,  &KGameServer::OnHandshakeRequest, sizeof(S2R_HANDSHAKE_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_ping_signal, &KGameServer::OnPingSignal, sizeof(S2R_PING_SIGNAL));
    REGISTER_INTERNAL_FUNC(s2r_update_performance, &KGameServer::OnUpdatePerformance, sizeof(S2R_UPDATE_PERFORMANCE));

    REGISTER_INTERNAL_FUNC(s2r_create_map_respond, &KGameServer::OnCreateMapRespond, sizeof(S2R_CREATE_MAP_RESPOND));

	REGISTER_INTERNAL_FUNC(s2r_player_login_respond, &KGameServer::OnPlayerLoginRespond, sizeof(S2R_PLAYER_LOGIN_RESPOND));
	REGISTER_INTERNAL_FUNC(s2r_confirm_player_login_request, &KGameServer::OnConfirmPlayerLoginRequest, sizeof(S2R_CONFIRM_PLAYER_LOGIN_REQUEST));

	REGISTER_INTERNAL_FUNC(s2r_player_leave_gs, &KGameServer::OnPlayerLeaveGS, sizeof(S2R_PLAYER_LEAVE_GS));

	REGISTER_INTERNAL_FUNC(s2r_search_map_request, &KGameServer::OnSearchMapRequest, sizeof(S2R_SEARCH_MAP_REQUEST));

	REGISTER_INTERNAL_FUNC(s2r_transfer_player_request, &KGameServer::OnTransferPlayerRequest, sizeof(S2R_TRANSFER_PLAYER_REQUEST));
	REGISTER_INTERNAL_FUNC(s2r_transfer_player_respond, &KGameServer::OnTransferPlayerRespond, sizeof(S2R_TRANSFER_PLAYER_RESPOND));

    REGISTER_INTERNAL_FUNC(s2r_invite_player_join_team_request, &KGameServer::OnInvitePlayerJoinTeamRequest, sizeof(S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_invite_player_join_team_respond, &KGameServer::OnInvitePlayerJoinTeamRespond, sizeof(S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND));
    REGISTER_INTERNAL_FUNC(s2r_apply_join_team_request, &KGameServer::OnApplyJoinTeamRequest, sizeof(S2R_APPLY_JOIN_TEAM_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_join_team_respond, &KGameServer::OnApplyJoinTeamRespond, sizeof(S2R_APPLY_JOIN_TEAM_RESPOND));
    REGISTER_INTERNAL_FUNC(s2r_team_del_member_request, &KGameServer::OnTeamDelMemberRequest, sizeof(S2R_TEAM_DEL_MEMBER_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_team_change_authority_request, &KGameServer::OnTeamChangeAuthorityRequest, sizeof(S2R_TEAM_CHANGE_AUTHORITY_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_team_set_loot_mode_request, &KGameServer::OnTeamSetLootModeRequest, sizeof(S2R_TEAM_SET_LOOT_MODE_REQUEST));
	REGISTER_INTERNAL_FUNC(s2r_team_set_roll_quality_request, &KGameServer::OnTeamSetRollQualityRequest, sizeof(S2R_TEAM_SET_ROLL_QUALITY_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_team_set_formation_leader_request, &KGameServer::OnTeamSetFormationLeader, sizeof(S2R_TEAM_SET_FORMATION_LEADER_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_team_disband_request, &KGameServer::OnPartyDisbandRequest, sizeof(S2R_TEAM_DISBAND_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_sync_team_member_max_lmr, &KGameServer::OnTeamSyncMemberMaxLMR, sizeof(S2R_SYNC_TEAM_MEMBER_MAX_LMR));
    REGISTER_INTERNAL_FUNC(s2r_sync_team_member_current_lmr, &KGameServer::OnTeamSyncMemberCurrentLMR, sizeof(S2R_SYNC_TEAM_MEMBER_CURRENT_LMR));
    REGISTER_INTERNAL_FUNC(s2r_sync_team_member_misc, &KGameServer::OnTeamSyncMemberMisc, sizeof(S2R_SYNC_TEAM_MEMBER_MISC));
    REGISTER_INTERNAL_FUNC(s2r_sync_team_member_position, &KGameServer::OnTeamSyncMemberPosition, sizeof(S2R_SYNC_TEAM_MEMBER_POSITION));


    REGISTER_INTERNAL_FUNC(s2r_talk_message, &KGameServer::OnTalkMessage, sizeof(S2R_TALK_MESSAGE));
    REGISTER_INTERNAL_FUNC(s2r_player_talk_error, &KGameServer::OnPlayerTalkError, sizeof(S2R_PLAYER_TALK_ERROR));

    REGISTER_INTERNAL_FUNC(s2r_apply_fellowship_data_request, &KGameServer::OnApplyFellowshipDataRequest, sizeof(S2R_UPDATE_FELLOWSHIP_DATA));
    REGISTER_INTERNAL_FUNC(s2r_update_fellowship_data, &KGameServer::OnUpdateFellowshipData, sizeof(S2R_UPDATE_FELLOWSHIP_DATA));
    REGISTER_INTERNAL_FUNC(s2r_add_fellowship_request, &KGameServer::OnAddFellowshipRequest, sizeof(S2R_ADD_FELLOWSHIP_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_get_fellowship_name_request, &KGameServer::OnGetFellowshipNameRequest, sizeof(S2R_GET_FELLOWSHIP_NAME_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_fellowship_player_level_and_forceid, &KGameServer::OnApplyFellowshipPlayerLevelAndForceID, sizeof(S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID));
    REGISTER_INTERNAL_FUNC(s2r_add_fellowship_notify, &KGameServer::OnAddFellowshipNotify, sizeof(S2R_ADD_FELLOWSHIP_NOTIFY));

    REGISTER_INTERNAL_FUNC(s2r_team_set_mark_request, &KGameServer::OnTeamSetMarkRequest, sizeof(S2R_TEAM_SET_MARK_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_send_mail_request, &KGameServer::OnSendMailRequest, sizeof(S2R_SEND_MAIL_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_send_global_mail_request, &KGameServer::OnSendGlobalMailRequest, sizeof(S2R_SEND_GLOBAL_MAIL_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_get_maillist_request, &KGameServer::OnGetMailListRequest, sizeof(S2R_GET_MAILLIST_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_get_mail_content, &KGameServer::OnGetMailContent, sizeof(S2R_GET_MAIL_CONTENT));
    REGISTER_INTERNAL_FUNC(s2r_get_mail_attachment, &KGameServer::OnGetMailAttachment, sizeof(S2R_GET_MAIL_ATTACHMENT));
    REGISTER_INTERNAL_FUNC(s2r_update_mail_data, &KGameServer::OnUpdateMailData, sizeof(S2R_UPDATE_MAIL_DATA));
    REGISTER_INTERNAL_FUNC(s2r_set_mail_read, &KGameServer::OnSetMailRead, sizeof(S2R_SET_MAIL_READ));
    REGISTER_INTERNAL_FUNC(s2r_delete_mail, &KGameServer::OnDeleteMail, sizeof(S2R_DELETE_MAIL));
    REGISTER_INTERNAL_FUNC(s2r_return_mail, &KGameServer::OnReturnMail, sizeof(S2R_RETURN_MAIL));

	REGISTER_INTERNAL_FUNC(s2r_sync_mid_map_mark, &KGameServer::OnSyncMidMapMark, sizeof(S2R_SYNC_MID_MAP_MARK));
    
    REGISTER_INTERNAL_FUNC(s2r_save_scene_player_list, &KGameServer::OnSaveScenePlayerList, sizeof(S2R_SAVE_SCENE_PLAYER_LIST));
    REGISTER_INTERNAL_FUNC(s2r_save_scene_data, &KGameServer::OnSaveSceneData, sizeof(S2R_SAVE_SCENE_DATA));
	REGISTER_INTERNAL_FUNC(s2r_reset_map_copy_request, &KGameServer::OnResetMapCopyRequest, sizeof(S2R_RESET_MAP_COPY_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_player_enter_scene_notify, &KGameServer::OnPlayerEnterSceneNotify, sizeof(S2R_PLAYER_ENTER_SCENE_NOTIFY));
    REGISTER_INTERNAL_FUNC(s2r_apply_player_saved_copy_request, &KGameServer::OnApplyPlayerSavedCopyRequest, sizeof(S2R_APPLY_PLAYER_SAVED_COPY_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_sync_road_track_force, &KGameServer::OnSyncRoadTrackForce, sizeof(S2R_SYNC_ROAD_TRACK_FORCE));

    REGISTER_INTERNAL_FUNC(s2r_load_role_data_request, &KGameServer::OnLoadRoleDataRequest, sizeof(S2R_LOAD_ROLE_DATA_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_sync_role_data, &KGameServer::OnSyncRoleData, sizeof(S2R_SYNC_ROLE_DATA));
    REGISTER_INTERNAL_FUNC(s2r_save_role_data, &KGameServer::OnSaveRoleData, sizeof(S2R_SAVE_ROLE_DATA));

    REGISTER_INTERNAL_FUNC(s2r_change_role_level_request, &KGameServer::OnChangeRoleLevelRequest, sizeof(S2R_CHANGE_ROLE_LEVEL_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_change_role_forceid_request, &KGameServer::OnChangeRoleForceIDRequest, sizeof(S2R_CHANGE_ROLE_FORCEID_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_send_gm_message, &KGameServer::OnSendGmMessage, sizeof(S2R_SEND_GM_MESSAGE));
    
    REGISTER_INTERNAL_FUNC(s2r_send_gm_command_gs, &KGameServer::OnSendGMCommandGS, sizeof(S2R_SEND_GM_COMMAND_GS));
    REGISTER_INTERNAL_FUNC(s2r_send_gm_command_gc, &KGameServer::OnSendGMCommandGC, sizeof(S2R_SEND_GM_COMMAND_GC));

    REGISTER_INTERNAL_FUNC(s2r_join_battle_field_queue_request, &KGameServer::OnJoinBattleFieldQueueRequest, sizeof(S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_leave_battle_field_queue_request, &KGameServer::OnLeaveBattleFieldQueueRequest, sizeof(S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_accept_join_battle_field, &KGameServer::OnAcceptJoinBattleField, sizeof(S2R_ACCEPT_JOIN_BATTLE_FIELD));
    REGISTER_INTERNAL_FUNC(s2r_get_battle_field_list, &KGameServer::OnGetBattleFieldList, sizeof(S2R_GET_BATTLE_FIELD_LIST));

    REGISTER_INTERNAL_FUNC(s2r_leave_battle_field_request, &KGameServer::OnLeaveBattleFieldRequest, sizeof(S2R_LEAVE_BATTLE_FIELD_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_apply_create_pq_request, &KGameServer::OnApplyCreatePQRequest, sizeof(S2R_APPLY_CREATE_PQ_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_delete_pq_request, &KGameServer::OnApplyDeletePQRequest, sizeof(S2R_APPLY_DELETE_PQ_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_change_pq_value_request, &KGameServer::OnApplyChangePQValueRequest, sizeof(S2R_APPLY_CHANGE_PQ_VALUE_REQUEST));
    
    REGISTER_INTERNAL_FUNC(s2r_add_camp_score, &KGameServer::OnAddCampScore, sizeof(S2R_ADD_CAMP_SCORE));
    REGISTER_INTERNAL_FUNC(s2r_apply_set_camp_request, &KGameServer::OnApplySetCampRequest, sizeof(S2R_APPLY_SET_CAMP_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_apply_tong_roster_request, &KGameServer::OnApplyTongRosterRequest, sizeof(S2R_APPLY_TONG_ROSTER_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_tong_info_request, &KGameServer::OnApplyTongInfoRequest, sizeof(S2R_APPLY_TONG_INFO_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_create_tong_request, &KGameServer::OnApplyCreateTongRequest, sizeof(S2R_APPLY_TONG_INFO_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_invite_player_join_tong_request, &KGameServer::OnInvitePlayerJoinTongRequest, sizeof(S2R_INVITE_PLAYER_JOIN_TONG_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_invite_player_join_tong_respond, &KGameServer::OnInvitePlayerJoinTongRespond, sizeof(S2R_INVITE_PLAYER_JOIN_TONG_RESPOND));
    REGISTER_INTERNAL_FUNC(s2r_apply_kick_out_tong_member_request, &KGameServer::OnApplyKickOutTongMemberRequest, sizeof(S2R_APPLY_KICK_OUT_TONG_MEMBER));
    REGISTER_INTERNAL_FUNC(s2r_modify_tong_info_request, &KGameServer::OnModifyTongInfoRequest, sizeof(S2R_MODIFY_TONG_INFO_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_modify_tong_schema_request, &KGameServer::OnModifyTongScheamRequest, sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_apply_quite_tong_request, &KGameServer::OnApplyQuiteTongRequest, sizeof(S2R_APPLY_QUITE_TONG_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_change_tong_member_group_request, &KGameServer::OnChangeTongMemberGroupRequest, sizeof(S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_change_tong_master_request, &KGameServer::OnChangeTongMasterRequest, sizeof(S2R_CHANGE_TONG_MASTER_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_change_tong_member_remark_request, &KGameServer::OnChangeTongMemberRemarkRequest, sizeof(S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_get_tong_description_request, &KGameServer::OnGetTongDescriptionRequest, sizeof(S2R_GET_TONG_DESCRIPTION_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_change_tong_camp_request, &KGameServer::OnChangeTongCampRequest, sizeof(S2R_CHANGE_TONG_CAMP_REQUEST));

    REGISTER_INTERNAL_FUNC(s2r_auction_lookup_request, &KGameServer::OnAuctionLookupRequest, sizeof(S2R_AUCTION_LOOKUP_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_auction_bid_request, &KGameServer::OnAuctionBidRequest, sizeof(S2R_AUCTION_BID_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_auction_sell_request, &KGameServer::OnAuctionSellRequest, sizeof(S2R_AUCTION_SELL_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_auction_cancel_request, &KGameServer::OnAuctionCancelRequest, sizeof(S2R_AUCTION_CANCEL_REQUEST));
    REGISTER_INTERNAL_FUNC(s2r_remote_lua_call, &KGameServer::OnRemoteLuaCall, sizeof(S2R_REMOTE_LUA_CALL));

    REGISTER_INTERNAL_FUNC(s2r_achievement_global_announce_request, &KGameServer::OnAchievementGlobalAnnounceRequest, sizeof(S2R_ACHIEVEMENT_ANNOUNCE_REQUEST));
    //AutoCode:注册协议
}

BOOL KGameServer::Init()
{
    BOOL            bResult  = false;
	int             nRetCode = false;
    struct timeval  TimeVal  = {0, 0};

    nRetCode = m_SocketAcceptor.Open("", g_pSO3GameCenter->m_Settings.m_nGameServerPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_SocketAcceptor.SetTimeout(&TimeVal);

	m_nConnectionTimeout        = g_pSO3GameCenter->m_Settings.m_nConnectionTimeout;
    m_dwNextLoginQueueID        = 1;

    bResult = true;
Exit0:
	return bResult;
}

void KGameServer::UnInit()
{
    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GSList[i].piSocket != NULL)
        {
            CloseConnection(m_GSList[i].nConnIndex);
        }
    }
    m_SocketAcceptor.Close();
}

void KGameServer::Activate()
{
    ProcessAcceptor();

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        KGS_INFO* pGSInfo = &m_GSList[i];

        if (pGSInfo->piSocket == NULL)
            continue;

        ProcessConnection(pGSInfo->nConnIndex, pGSInfo->piSocket);
    }
}

KGS_INFO* KGameServer::GetGSInfoByConnectionIndex(int nConnIndex)
{
    if (nConnIndex == 0)
        return NULL;

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GSList[i].nConnIndex == nConnIndex)
        {
            return &m_GSList[i];
        }
    }

    return NULL;
}

int KGameServer::GetBestGSConnectionForCreateMap(KMapInfo* pMapInfo)
{
    int     nConnIndex      = 0;
    float   fPerformance    = 0.0f;

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GSList[i].piSocket == NULL)
            continue;

        // 内存使用超标的一票否决
        if ((int)(m_GSList[i].uMemory >> 20) > g_pSO3GameCenter->m_Settings.m_nMemoryLimitForCreateMap)
            continue;

        // 考虑平均性能是为了避免瞬时性能过于乐观
        if (m_GSList[i].fAveragePerformance < g_pSO3GameCenter->m_Settings.m_fAveragePerformanceLimitForCreateMap)
            continue;

        // 考虑瞬时性能是为了避免平均性能无法即时反映性能下降
        if (m_GSList[i].fImmediatePerformance < g_pSO3GameCenter->m_Settings.m_fImmediatePerformanceLimitForCreateMap)
            continue;

        if (m_GSList[i].fAveragePerformance > fPerformance)
        {
            fPerformance = m_GSList[i].fAveragePerformance;
            nConnIndex   = m_GSList[i].nConnIndex;
        }
    }

    return nConnIndex;
}

BOOL KGameServer::IsPlayerLoginPerformanceLimit(int nConnIndex)
{
    BOOL        bResult = false;
    KGS_INFO*   pGSInfo = NULL;

    pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
    KGLOG_PROCESS_ERROR(pGSInfo);

    KG_PROCESS_ERROR(pGSInfo->fAveragePerformance < g_pSO3GameCenter->m_Settings.m_fAveragePerformanceLimitForNewPlayer);

    KG_PROCESS_ERROR(pGSInfo->fImmediatePerformance < g_pSO3GameCenter->m_Settings.m_fImmediatePerformanceLimitForNewPlayer);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGameServer::ProcessRoleLogin(
    KACCOUNT_LOGIN_INFO* pAccInfo, DWORD dwRoleID, KRoleBaseInfo* pBaseInfo, DWORD dwQueueID
)
{
    int							nResult				= false;
    int                         nRetCode            = false;
    unsigned                    uBufferSize         = 0;
    KRole*                      pRole               = NULL;
    KMapManager*                pMapManager         = &g_pSO3GameCenter->m_MapManager;                 
    KMapCopy*                   pMapCopy            = NULL;
    DWORD                       dwMapID             = 0;
    int                         nMapCopyIndex       = 0;
    KMapInfo*                   pMap                = NULL;
    const GUID                  Guid                = {0, 0, 0, 0};
    BOOL                        bGSPerformLimit     = false;

    assert(pAccInfo);
    assert(pBaseInfo);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    dwMapID         = pBaseInfo->CurrentPos.dwMapID;
    nMapCopyIndex   = pBaseInfo->CurrentPos.nMapCopyIndex;

    pMap = pMapManager->GetMapInfo(dwMapID);
    if (pMap == NULL)
    {
        KGLogPrintf(
            KGLOG_ERR, "Invalid map ID %u, Role: %s\n", 
            dwMapID, pRole->m_szName
        );

        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pAccInfo->ulIdentity, eGameLoginSystemMaintenance, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
        );

        goto Exit1;
    }

    pMapCopy = pMap->GetMapCopy(nMapCopyIndex);
    if (pMapCopy == NULL)
    {
        BOOL bIsHometown         = false;

        bIsHometown = g_pSO3GameCenter->m_HometownList.IsMapInHowntownList(dwMapID);        
        if (bIsHometown)
        {
            nMapCopyIndex = pMap->FindMapCopyHasFreeSpace();
            if (nMapCopyIndex == 0)
            {
                g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
                    pAccInfo->ulIdentity, eGameLoginSystemMaintenance, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
                );
                goto Exit1;
            }
            pBaseInfo->CurrentPos.nMapCopyIndex = nMapCopyIndex;
        }
    }

    pMapCopy = pMap->GetMapCopy(nMapCopyIndex);

    nRetCode = g_pSO3GameCenter->m_BattleFieldManager.IsBattleField(dwMapID);
    if (nRetCode)
    {
        nRetCode = g_pSO3GameCenter->m_BattleFieldManager.CheckPermission(dwRoleID, dwMapID, nMapCopyIndex);
        if (!nRetCode)
        {
            pMapCopy = NULL;
        }
    }

    if (pMapCopy == NULL && pMap->m_bUseLastEntry)
    {
        // 如果所在副本不存在,则用上一个入口点
        pMapCopy = pMapManager->GetMapCopy(pBaseInfo->LastEntry.dwMapID, pBaseInfo->LastEntry.nMapCopyIndex);
        if (pMapCopy == NULL)
        {
            // 注意,如果"上一个入口点"的地图也不存在,则可能是策划搞错了.
            // LastEntry不存在可能有两个原因(都是设置错误):
            // 1. 无效的LastEntry
            // 2. LastEntry可能是个副本,已经被重置或者刷新了(手动重置,定时刷新,空闲后被删除)
            g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
                pAccInfo->ulIdentity, eGameLoginSystemMaintenance, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
            );
            goto Exit1;
        }

        dwMapID         = pBaseInfo->LastEntry.dwMapID;
        nMapCopyIndex   = pBaseInfo->LastEntry.nMapCopyIndex;

        pBaseInfo->CurrentPos = pBaseInfo->LastEntry; // 使用上一个入口
    }
    
    pMap = pMapManager->GetMapInfo(dwMapID);
    if (pMap == NULL)
    {
        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pAccInfo->ulIdentity, eGameLoginSystemMaintenance, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
        );
        goto Exit1;
    }

    if (pMapCopy == NULL || pMapCopy->m_eState == eMapStateOffline)
    {
        // 尝试加载所在地图副本
        int     nGSConnIndex    = 0;
        time_t  nCreateTime     = time(NULL);
        KRole*  pPartyLeader    = NULL;

        if (pRole->m_dwTeamID != INVALID_PARTY_ID)
        {
            KTeam* pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pRole->m_dwTeamID);
            KGLOG_PROCESS_ERROR(pTeam);

            pPartyLeader = g_pSO3GameCenter->m_RoleManager.GetRole(pTeam->dwAuthority[tatLeader]);
            KGLOG_PROCESS_ERROR(pPartyLeader);
        }

        nRetCode = pMapManager->IsMapCopyInPreloadGroup(dwMapID, nMapCopyIndex);
        if (nRetCode)
        {
            // 该副本不能被动态加载
            g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
                pAccInfo->ulIdentity, eGameLoginSystemMaintenance, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
            );
            goto Exit1;
        }

        nRetCode = pMapManager->IsMapCopyCountLimit(pMap);
        if (nRetCode)
        {
            // 无法创建副本: 副本数超过限制
            g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
                pAccInfo->ulIdentity, eGameLoginOverload, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
            );
            goto Exit1;
        }

        nGSConnIndex = g_pSO3GameCenter->m_GameServer.GetBestGSConnectionForCreateMap(pMap);
        if (nGSConnIndex == 0)
        {
            g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
                pAccInfo->ulIdentity, eGameLoginOverload, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
            );
            goto Exit1;
        }

        if (pMapCopy)
        {
            pMapCopy->m_nConnIndex = nGSConnIndex;
        }
        else
        {
            pMapCopy = pMap->CreateMapCopy(nMapCopyIndex, nCreateTime, 0, nGSConnIndex, true);
            KGLOG_PROCESS_ERROR(pMapCopy);
        }

        DoCreateMapNotify(nGSConnIndex, dwMapID, nMapCopyIndex);

        pMapCopy->m_eState = eMapStateCreating;
        
        nRetCode = g_pSO3GameCenter->m_MapManager.SetMapCopyOwner(
            dwMapID, nMapCopyIndex, pPartyLeader ? pPartyLeader->m_dwPlayerID : dwRoleID
        );
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    pMapCopy->m_nLastTryLoginTime  = g_pSO3GameCenter->m_nTimeNow;

    // 如果副本需要排队,玩家又没有在指定队伍排队,则重新排队
    if (pMapCopy->m_dwQueueID != INVALID_QUEUE_ID && dwQueueID != pMapCopy->m_dwQueueID)
    {
        if (dwQueueID != INVALID_QUEUE_ID)
        {
            KGLogPrintf(KGLOG_DEBUG, "Incorrect QueueID : Player QID(%lu), MapCopy QID(%lu)", dwQueueID, pMapCopy->m_dwQueueID);
        }

        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pAccInfo->ulIdentity, eGameLoginQueueWait, pRole->m_dwPlayerID, pMapCopy->m_dwQueueID, 0, 0, Guid
        );
        goto Exit1;
    }

    // 如果副本正在创建,副本人数已满,目标GS性能不足 或者 副本需要预排队, 则暂时不让新玩家登陆,排队等待
    bGSPerformLimit = IsPlayerLoginPerformanceLimit(pMapCopy->m_nConnIndex);
    if (
        (pMapCopy->m_eState == eMapStateCreating) ||
        (pMapCopy->m_nPlayerCount >= pMap->m_nMaxPlayerCount) ||
        bGSPerformLimit ||
        (pMapCopy->m_nPlayerCount >= pMap->m_nPreQueuePlayerCount && g_pSO3GameCenter->m_nTimeNow < pMapCopy->m_nNextPermitLoginTime)
    )
    {
        if (pMapCopy->m_dwQueueID == INVALID_QUEUE_ID)
        {
            pMapCopy->m_dwQueueID = m_dwNextLoginQueueID++;

            KGLogPrintf(KGLOG_DEBUG, "New QueueID = %lu, MapCopyIndex = %d", pMapCopy->m_dwQueueID, pMapCopy->m_nCopyIndex);

        }

        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pAccInfo->ulIdentity, eGameLoginQueueWait, pRole->m_dwPlayerID, pMapCopy->m_dwQueueID, 0, 0, Guid
        );
        goto Exit1;
    }

    if (pMapCopy->m_eState != eMapStateNormal)
    {
        // 副本可能在其他状态,比如正被重置的过程中,这个算了,不好处理,直接不让进得了
        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pAccInfo->ulIdentity, eGameLoginSystemMaintenance, pRole->m_dwPlayerID, INVALID_QUEUE_ID, 0, 0, Guid
        );
        goto Exit1;
    }

    // 计算下一次允许登陆时间
    pMapCopy->m_nNextPermitLoginTime = g_pSO3GameCenter->m_nTimeNow + pMap->m_nLoginIntervalTime;

    // 在登录被拒绝的时候,需要把人数减回来;
    nRetCode = g_pSO3GameCenter->m_MapManager.IncreaseMapCopyPlayerCount(dwMapID, nMapCopyIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    pRole->m_nDestConnIndex     = pMapCopy->m_nConnIndex;
    pRole->m_dwDestMapID        = dwMapID;
    pRole->m_nDestMapCopyIndex  = nMapCopyIndex;

    g_pSO3GameCenter->m_GameServer.DoPlayerLoginRequest(
        pMapCopy->m_nConnIndex, *pAccInfo, dwRoleID, *pBaseInfo
    );

    pRole->SetRoleState(rsLG_WaitForPermit);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

BOOL KGameServer::GetGameWorldProtocolVersion(int* pnGameWorldLowerProtocolVersion, int* pnGameWorldUpperProtocolVersion)
{
    BOOL bResult = false;

    assert(pnGameWorldLowerProtocolVersion);
    assert(pnGameWorldUpperProtocolVersion);

    KG_PROCESS_ERROR(m_bGameWorldProtocolValidFlag);

    *pnGameWorldLowerProtocolVersion = m_nGameWorldLowerProtocolVersion;
    *pnGameWorldUpperProtocolVersion = m_nGameWorldUpperProtocolVersion;

    bResult = true;
Exit0:
    return bResult;
}

size_t KGameServer::GetFluxTotal()
{
    KGS_INFO*   pGS             = NULL;
    size_t      uTotalFlux      = 0;

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        pGS = &m_GSList[i];

        if (pGS->piSocket == NULL)
            continue;

        uTotalFlux   += pGS->uNetworkFlux;
    }

    return uTotalFlux;
}

BOOL KGameServer::SendGlobalSysMsg(const char cszMsg[])
{
    BOOL                     bResult            = false;
    BOOL                     bRetCode           = false;
    S2R_TALK_MESSAGE*        pUpMsg             = NULL;
    size_t                   uTalkDataSize      = 0;
    IKG_Buffer*              piSendBuffer       = NULL;
    R2S_TALK_MESSAGE*        pDownMsg           = NULL;

    uTalkDataSize = 1 + strlen(cszMsg) + 1;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_TALK_MESSAGE) + uTalkDataSize));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDownMsg = (R2S_TALK_MESSAGE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDownMsg);
    
    pDownMsg->wProtocolID      = r2s_talk_message;
    pDownMsg->byMsgType        = trGlobalSys;
	pDownMsg->dwTalkerID       = ERROR_ID;
    pDownMsg->dwReceiverID     = ERROR_ID;

    pDownMsg->szTalker[0]      = '\0';
    pDownMsg->szReceiver[0]    = '\0';

    pDownMsg->byTalkData[0]    = tdbText;

    strcpy((char*)pDownMsg->byTalkData + 1, cszMsg);

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}


BOOL KGameServer::LoadRoleData(int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, BYTE* pbyRoleData, size_t uRoleDataLen)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    BYTE*           pbyPos      = pbyRoleData;
    BYTE*           pbyTail     = pbyRoleData + uRoleDataLen;

    while (pbyPos < pbyTail)
    {
        size_t uPackSize = min(pbyTail - pbyPos, MAX_ROLE_DATA_PAK_SIZE);

        bRetCode = DoSyncRoleData(nConnIndex, dwPlayerID, pbyPos, pbyPos - pbyRoleData, uPackSize);
        KGLOG_PROCESS_ERROR(bRetCode);

        pbyPos += uPackSize;
    }

    bRetCode = DoLoadRoleData(nConnIndex, dwPlayerID, bSucceed, uRoleDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KGameServer::ProcessAcceptor()
{
    int                 nWorldIndex     = 0;
	IKG_SocketStream*   piSocket        = NULL;
    struct timeval      TimeVal         = {3, 0};
    KGS_INFO*           pGSInfo         = NULL;
    struct in_addr      RemoteAddr      = { 0 };
    u_short             wRemotePortNet  = 0;
    u_short             wRemotePortHost = 0;
    char*               pszRetString    = NULL;

    piSocket = m_SocketAcceptor.Accept();
    KG_PROCESS_ERROR(piSocket);
    
    piSocket->SetTimeout(&TimeVal);

    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GSList[i].piSocket == NULL)
        {
            nWorldIndex = i + 1;
            pGSInfo     = &m_GSList[i];
            break;
        }
    }

    KG_PROCESS_ERROR(pGSInfo);

    pGSInfo->piSocket = piSocket;
    pGSInfo->piSocket->AddRef();

    pGSInfo->nConnIndex             = m_nLastConnIndex++;
    pGSInfo->nWorldIndex            = nWorldIndex;
    pGSInfo->fImmediatePerformance  = 0.0f;
    pGSInfo->fAveragePerformance    = 0.0f;
    pGSInfo->uMemory                = 0;
    pGSInfo->nPlayerCount           = 0;
    pGSInfo->nConnectionCount       = 0;
    pGSInfo->uNetworkFlux           = 0;
    pGSInfo->nLastRecvPacketTime    = g_pSO3GameCenter->m_nTimeNow;

    piSocket->GetRemoteAddress(&RemoteAddr, &wRemotePortNet);
    wRemotePortHost = ntohs(wRemotePortNet);

    pszRetString = inet_ntoa(RemoteAddr);
    KGLOG_PROCESS_ERROR(pszRetString);

    KGLogPrintf(
        KGLOG_INFO, "GS %d connected from %s:%u, WorldIndex = %d\n", 
        pGSInfo->nConnIndex, pszRetString, wRemotePortHost, nWorldIndex
    );

Exit0:
    KG_COM_RELEASE(piSocket);
    return;
}


BOOL KGameServer::ProcessConnection(int nConnIndex, IKG_SocketStream* piSocket)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = 0;
    size_t                      uPakSize        = 0;
    IKG_Buffer*                 piPackage       = NULL;
    time_t                      nCurrentTime    = g_pSO3GameCenter->m_nTimeNow;
	KGS_INFO*                   pGSInfo         = NULL;
    int                         nIdleTime       = 0;

    assert(piSocket);
    // 在协议响应函数中,可能将Socket关闭,所以这里先AddRef
    piSocket->AddRef();

	pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
	KGLOG_PROCESS_ERROR(pGSInfo);

    while (true)
    {
        struct timeval              TimeVal         = {0, 0};
        INTERNAL_PROTOCOL_HEADER*   pHeader         = NULL;
        PROCESS_PROTOCOL_FUNC       ProcessFunc     = NULL;

        nRetCode = piSocket->CheckCanRecv(&TimeVal);
        if (nRetCode == 0)
            break;

        KG_PROCESS_ERROR(nRetCode == 1);
        
        KG_COM_RELEASE(piPackage);

        nRetCode = piSocket->Recv(&piPackage);
        KG_PROCESS_ERROR(nRetCode == 1);
        
        KGLOG_PROCESS_ERROR(piPackage);

	    pGSInfo->nLastRecvPacketTime = nCurrentTime;

	    pHeader = (INTERNAL_PROTOCOL_HEADER*)piPackage->GetData();
        KGLOG_PROCESS_ERROR(pHeader);

        KGLOG_PROCESS_ERROR(pHeader->wProtocolID > s2r_protocol_begin);
	    KGLOG_PROCESS_ERROR(pHeader->wProtocolID < s2r_protocol_end);

        uPakSize = piPackage->GetSize();
        KGLOG_PROCESS_ERROR(uPakSize >= m_uProtocolSize[pHeader->wProtocolID]);

	    ProcessFunc = m_ProcessProtocolFuns[pHeader->wProtocolID];
        KGLOG_PROCESS_ERROR(ProcessFunc);

        (this->*ProcessFunc)((BYTE*)(pHeader), uPakSize, nConnIndex);
    }
    
    nIdleTime = (int)(nCurrentTime - pGSInfo->nLastRecvPacketTime);
	if (nIdleTime > m_nConnectionTimeout)
    {
        KGLogPrintf(KGLOG_DEBUG, "[GS%d] LastPakTime = %u\n", nConnIndex, pGSInfo->nLastRecvPacketTime);
        KGLogPrintf(KGLOG_DEBUG, "[GS%d] CurrentTime = %u\n", nConnIndex, nCurrentTime);

        KGLogPrintf(KGLOG_ERR, "GS %d timeout(%d)\n", nConnIndex, nIdleTime);
        goto Exit0;
    }

    bResult = true;
Exit0:

    KG_COM_RELEASE(piSocket);

    if (!bResult)
    {
        CloseConnection(nConnIndex);
    }

    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::CloseConnection(int nConnIndex)
{
    BOOL      bResult   = false;
    KGS_INFO* pGSInfo   = GetGSInfoByConnectionIndex(nConnIndex);
    int       nGSCount  = 0;

    KGLOG_PROCESS_ERROR(pGSInfo);

    KGLogPrintf(KGLOG_INFO, "GS %d disconnected !\n", nConnIndex);

    g_pSO3GameCenter->m_RoleManager.OnGSCrash(nConnIndex);	

    g_pSO3GameCenter->m_MapManager.OnGameServerDestroy(nConnIndex);

    KG_COM_RELEASE(pGSInfo->piSocket);

    pGSInfo->nConnIndex = 0;

    pGSInfo->uRoleDataOffset = 0;

    // 如果所有的GS都断开了,则设置协议版本号为"未设置"
    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        if (m_GSList[i].piSocket != NULL)
        {
            nGSCount++;
        }
    }

    if (nGSCount == 0)
    {
        g_pSO3GameCenter->m_Gateway.DoSetGameWorldVersion(0, 0);
        m_bGameWorldProtocolValidFlag = false;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGameServer::Send(int nConnIndex, IKG_Buffer* piBuffer)
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    KGS_INFO*   pGSInfo     = NULL;

    KG_PROCESS_SUCCESS(nConnIndex == 0);

    pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
    KGLOG_PROCESS_ERROR(pGSInfo);    

    KGLOG_PROCESS_ERROR(pGSInfo->piSocket);    

    nRetCode = pGSInfo->piSocket->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KGameServer::Broadcast(IKG_Buffer* piBuffer, int nExceptConnIndex/* = -1*/)
{
    for (int i = 0; i < MAX_GS_COUNT; i++)
    {
        KGS_INFO* pGSInfo = &m_GSList[i];

        if (pGSInfo->piSocket == NULL)
            continue;

        if (pGSInfo->nConnIndex == nExceptConnIndex)
            continue;

        pGSInfo->piSocket->Send(piBuffer);
    }
}

BOOL KGameServer::CallGSConnectScript(int nConnIndex)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    int         nIndex              = 0;
    DWORD       dwScriptID          = 0;
    const char* pszFunction         = "OnGSConnect";

    dwScriptID = g_FileNameHash(MAIN_SCRIPT_FILE);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nIndex);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsScriptExist(dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(dwScriptID, pszFunction);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.Push(nConnIndex);

    g_pSO3GameCenter->m_ScriptManager.Call(dwScriptID, pszFunction, 0);

    bResult = true;
Exit0:
    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nIndex);
    return bResult;
}

void KGameServer::OnHandshakeRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bResult         = false;
	int                     nRetCode        = false;
    S2R_HANDSHAKE_REQUEST*  pHandshake      = (S2R_HANDSHAKE_REQUEST*)pbyData;

    if (m_bGameWorldProtocolValidFlag)
    {
        KGLOG_PROCESS_ERROR(pHandshake->nGameWorldLowerVersion == m_nGameWorldLowerProtocolVersion);
        KGLOG_PROCESS_ERROR(pHandshake->nGameWorldUpperVersion == m_nGameWorldUpperProtocolVersion);
    }
    else
    {
        m_nGameWorldLowerProtocolVersion = pHandshake->nGameWorldLowerVersion;
        m_nGameWorldUpperProtocolVersion = pHandshake->nGameWorldUpperVersion;

        g_pSO3GameCenter->m_Gateway.DoSetGameWorldVersion(m_nGameWorldLowerProtocolVersion, m_nGameWorldUpperProtocolVersion);

        m_bGameWorldProtocolValidFlag = true;
    }

    DoHandshakeRespond(nConnIndex);

    DoSyncCampInfo(nConnIndex);

    DoSyncGlobalSystemValue(nConnIndex);

    CallGSConnectScript(nConnIndex);

    g_pSO3GameCenter->m_TeamCenter.SyncNewServer(nConnIndex);

    g_pSO3GameCenter->m_MapManager.OnGameServerCreate(nConnIndex);

    g_pSO3GameCenter->m_PQCenter.OnGameServerCreate(nConnIndex);

    bResult = true;
Exit0:
    if (!bResult)
    {
        CloseConnection(nConnIndex);
    }
	return;
}

void KGameServer::OnPingSignal(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{	
}

void KGameServer::OnUpdatePerformance(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    KGS_INFO*                       pGSInfo             = NULL;
    S2R_UPDATE_PERFORMANCE*         pPerformanceData    = (S2R_UPDATE_PERFORMANCE*)pbyData;

    pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
    KGLOG_PROCESS_ERROR(pGSInfo);

    pGSInfo->fImmediatePerformance = 
        (pGSInfo->fImmediatePerformance * 60.0f + pPerformanceData->nIdleFrame * 40.0f) / 100.0f;

    pGSInfo->fAveragePerformance   =
        (pGSInfo->fAveragePerformance * 99.0f + pPerformanceData->nIdleFrame * 1.0f) / 100.0f;

    pGSInfo->uMemory               = pPerformanceData->uMemory;
    pGSInfo->nPlayerCount          = pPerformanceData->nPlayerCount;
    pGSInfo->nConnectionCount      = pPerformanceData->nConnectionCount;
    pGSInfo->uNetworkFlux          = pPerformanceData->uNetworkFlux;

Exit0:
    return;
}

void KGameServer::OnPlayerLoginRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL                        bRetCode    = false;
	S2R_PLAYER_LOGIN_RESPOND*   pRespond    = (S2R_PLAYER_LOGIN_RESPOND*)pbyData;
	KRole*                      pRole       = NULL;
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwPlayerID);
	KGLOG_PROCESS_ERROR(pRole);

	KGLOG_PROCESS_ERROR(pRole->GetRoleState() == rsLG_WaitForPermit);

	if (pRespond->bPermit)
	{
        g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pRespond->dwPacketIdentity, eGameLoginSucceed, pRole->m_dwPlayerID, INVALID_QUEUE_ID,
            pRespond->dwGSAddr, pRespond->nGSPort,  pRespond->Guid
        );

		pRole->SetRoleState(rsLG_WaitForLogin);
	}
	else
	{
		g_pSO3GameCenter->m_Gateway.DoGameLoginRespond(
            pRespond->dwPacketIdentity, eGameLoginUnknownError, pRole->m_dwPlayerID, INVALID_QUEUE_ID,
            pRespond->dwGSAddr, pRespond->nGSPort, pRespond->Guid
        );
        
        bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(pRole->m_dwDestMapID, pRole->m_nDestMapCopyIndex);
        KGLOG_PROCESS_ERROR(bRetCode);

		pRole->SetRoleState(rsOffline);
	}

Exit0:
	return;
}

void KGameServer::OnCreateMapRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL                            bRetCode            = false;
	S2R_CREATE_MAP_RESPOND*         pRespond            = (S2R_CREATE_MAP_RESPOND*)pbyData;
    KMapInfo*                       pMapInfo            = NULL;
    KMapCopy*                       pMapCopy            = NULL;
    KRole*                          pRole               = NULL;
    KROLE_STATE                     eRoleState          = rsInvalid;
    DWORD                           dwOwnerID           = ERROR_ID;

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(pRespond->dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pRespond->dwMapID, pRespond->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    KGLogPrintf(
        KGLOG_INFO, "Create map %s(%.2u, %d) on GS %d %s !\n", 
        pMapInfo->m_szName, pRespond->dwMapID, pRespond->nMapCopyIndex, nConnIndex, 
        pRespond->bResult ? "OK" : "Failed"
    );

    assert(pMapCopy->m_eState == eMapStateCreating);

    // 切换地图状态,处理等待在这张地图上的所有玩家

	if (pRespond->bResult)
	{
        pMapCopy->m_eState = eMapStateNormal;

        for (KCREATE_MAP_CG_CALLBACK_LIST::const_iterator it = pMapCopy->m_CGCallbackList.begin(); it != pMapCopy->m_CGCallbackList.end(); ++it)
        {
            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwPlayerID);
            KGLOG_PROCESS_ERROR(pRole);

            eRoleState = pRole->GetRoleState();
            KGLOG_PROCESS_ERROR(eRoleState == rsCG_WaitForCreateMap || eRoleState == rsOnline);
            
            if (eRoleState == rsOnline)
            {
                // 通知玩家地图已加载好，可以进来了
                g_pSO3GameCenter->m_GameServer.DoFinishCreateMapNotify(pRole->m_nConnIndex, pRole->m_dwPlayerID);
                continue;
            }

            // 注意,这里不见得就是跨服,确切的说应该是跨地图等待创建目标地图
            if (pRole->m_nConnIndex == nConnIndex)
            {
                bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(pRole->m_dwMapID, pRole->m_nMapCopyIndex);
                KGLOG_PROCESS_ERROR(bRetCode);

                pRole->m_dwMapID        = pRespond->dwMapID;
                pRole->m_nMapCopyIndex  = pRespond->nMapCopyIndex;

                bRetCode = g_pSO3GameCenter->m_MapManager.IncreaseMapCopyPlayerCount(pRespond->dwMapID, pRespond->nMapCopyIndex);
                KGLOG_PROCESS_ERROR(bRetCode);

                g_pSO3GameCenter->m_GameServer.DoSyncFellowshipMapID(pRole->m_dwPlayerID, pRole->m_dwMapID);

                // 玩家就在本服务器上,那么, 直接切换回"Online"即可
                DoSearchMapRespond(
                    pRole->m_nConnIndex, it->dwPlayerID, smrcSuccess, false, 
                    pRespond->dwMapID, pRespond->nMapCopyIndex,
                    it->nX, it->nY, it->nZ
                );
                pRole->SetRoleState(rsOnline);
            }
            else
            {
                // 玩家并不在本服务器上,那么等待源服务器传角色数据过来(跨服)
                DoSearchMapRespond(
                    pRole->m_nConnIndex, it->dwPlayerID, smrcSuccess, true, 
                    pRespond->dwMapID, pRespond->nMapCopyIndex,
                    it->nX, it->nY, it->nZ
                );

                pRole->SetRoleState(rsCG_WaitForPlayerData);

                pRole->m_dwDestMapID        = pRespond->dwMapID;
                pRole->m_nDestMapCopyIndex  = pRespond->nMapCopyIndex;
                pRole->m_nDestConnIndex     = nConnIndex;

                bRetCode = g_pSO3GameCenter->m_MapManager.IncreaseMapCopyPlayerCount(pRespond->dwMapID, pRespond->nMapCopyIndex);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }

        pMapCopy->ClearCGCallbackPlayer();
	}
    else
    {  
        for (KCREATE_MAP_CG_CALLBACK_LIST::const_iterator it = pMapCopy->m_CGCallbackList.begin(); it != pMapCopy->m_CGCallbackList.end(); ++it)
        {
            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwPlayerID);
            if (!pRole)
                continue;

            eRoleState = pRole->GetRoleState();

            KGLOG_PROCESS_ERROR(eRoleState == rsCG_WaitForCreateMap || eRoleState == rsOnline);
            // 通知玩家,前方路途不通,稍后再来
            DoSearchMapRespond(
                pRole->m_nConnIndex, it->dwPlayerID, smrcFailed, false, 
                pRespond->dwMapID, pRespond->nMapCopyIndex,
                it->nX, it->nY, it->nZ
            );
            pRole->SetRoleState(rsOnline);
        }

        pMapCopy->ClearCGCallbackPlayer();

        if (pMapCopy->m_piSceneData == NULL)
        {
            pMapInfo->DeleteMapCopy(pRespond->nMapCopyIndex);
        }
        else
        {
            pMapCopy->m_eState      = eMapStateOffline;
            pMapCopy->m_nConnIndex  = 0;
        }
    }

Exit0:
	return;
}

#define DO_SEARCH_MAP_FAILED_RETURN(C, Result)                      \
    do                                                              \
    {                                                               \
        if (!(C))                                                   \
        {                                                           \
            DoSearchMapRespond(                                     \
                nConnIndex, pRequest->dwPlayerID, Result, false,    \
                pRequest->dwMapID, nMapCopyIndex,                   \
                pRequest->nPosX, pRequest->nPosY, pRequest->nPosZ   \
            );                                                      \
            goto Exit0;                                             \
        }                                                           \
    } while (false)


void KGameServer::OnSearchMapRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
	S2R_SEARCH_MAP_REQUEST*     pRequest            = (S2R_SEARCH_MAP_REQUEST*)pbyData;
    KROLE_STATE                 eRoleState          = rsInvalid;
	int                         nMapCopyIndex       = pRequest->nMapCopyIndex;
	KRole*                      pRole               = NULL;
    KMapInfo*                   pMapInfo            = NULL;
    KMapCopy*                   pMapCopy            = NULL;
    KRole*                      pPartyLeader        = NULL;

    DO_SEARCH_MAP_FAILED_RETURN(pRequest->nMapCopyIndex >= 0, smrcFailed);

	pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    if (pRole->m_dwTeamID != INVALID_PARTY_ID)
    {
        KTeam* pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pRole->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        pPartyLeader = g_pSO3GameCenter->m_RoleManager.GetRole(pTeam->dwAuthority[tatLeader]);
        KGLOG_PROCESS_ERROR(pPartyLeader);
    }

	pMapInfo    = g_pSO3GameCenter->m_MapManager.GetMapInfo(pRequest->dwMapID);
    DO_SEARCH_MAP_FAILED_RETURN(pMapInfo != NULL, smrcFailed);

    eRoleState  = pRole->GetRoleState();
	DO_SEARCH_MAP_FAILED_RETURN(eRoleState == rsOnline, smrcFailed);

    DO_SEARCH_MAP_FAILED_RETURN(pRole->m_nConnIndex == nConnIndex, smrcFailed);

    switch (pMapInfo->m_nType)
    {
    case emtBirthMap:
        DO_SEARCH_MAP_FAILED_RETURN(nMapCopyIndex < pMapInfo->m_nSNGenerator, smrcFailed);
    
        pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pRequest->dwMapID, nMapCopyIndex);
        DO_SEARCH_MAP_FAILED_RETURN(pMapCopy, smrcFailed);
        DO_SEARCH_MAP_FAILED_RETURN(pMapCopy->m_eState == eMapStateNormal, smrcFailed);
    	break;

    case emtNormalMap:
        nMapCopyIndex = 1;
        
        pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pRequest->dwMapID, nMapCopyIndex);
        if (!pMapCopy)
        {
            // 不在预加载表里的才可动态加载
            bRetCode = g_pSO3GameCenter->m_MapManager.IsMapCopyInPreloadGroup(pRequest->dwMapID, nMapCopyIndex);
            DO_SEARCH_MAP_FAILED_RETURN(!bRetCode, smrcGameMaintenance);
        }
    	break;

    case emtDungeon:
        if (nMapCopyIndex == 0)
        {
            nMapCopyIndex = g_pSO3GameCenter->m_MapManager.FindPlayerCopyIndex(pRole, pPartyLeader, pRequest->dwMapID);
        }

        if (nMapCopyIndex == 0)
        {
            uint64_t    nNextCopyIndex = 0;

            bRetCode = g_pSO3GameCenter->m_piSequence->GenerateID(pMapInfo->m_szSequenceName, 16, &nNextCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            nMapCopyIndex = (int)nNextCopyIndex;
            pMapInfo->m_nSNGenerator++;
        }

        pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pRequest->dwMapID, nMapCopyIndex);
        break;

    case emtBattleField:
        // 战场不处理nMapCopyIndex
        pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pRequest->dwMapID, nMapCopyIndex);
        break;

    default:
        assert(false);
        break;
    }

    // 到这里,MapCopyIndex已经找到了
    if (pMapCopy == NULL || pMapCopy->m_eState == eMapStateOffline)
    {
        // 只有动态加载的NormalScene, Dungeon 和 BattleField能进入这里 
        int nDestConnIndex  = 0;

        bRetCode = g_pSO3GameCenter->m_MapManager.IsMapCopyCountLimit(pMapInfo);
        DO_SEARCH_MAP_FAILED_RETURN(!bRetCode, smrcMapCopyCountLimit);

		nDestConnIndex = g_pSO3GameCenter->m_GameServer.GetBestGSConnectionForCreateMap(pMapInfo);
        DO_SEARCH_MAP_FAILED_RETURN(nDestConnIndex != 0, smrcGameOverLoaded);
        
        bRetCode = g_pSO3GameCenter->m_MapManager.CheckPlayerEnterNewCopyTimes(
            pRequest->dwPlayerID, pRequest->dwMapID, nMapCopyIndex
        );  
        DO_SEARCH_MAP_FAILED_RETURN(bRetCode, smrcEnterNewCopyTooManyTimes);

        if (pMapCopy != NULL)
        {
            pMapCopy->m_nConnIndex = nDestConnIndex;
        }
        else
        {
            pMapCopy = pMapInfo->CreateMapCopy(nMapCopyIndex, g_pSO3GameCenter->m_nTimeNow, 0, nDestConnIndex, true);
		    KGLOG_PROCESS_ERROR(pMapCopy);
        }
        
        DoCreateMapNotify(nDestConnIndex, pRequest->dwMapID, nMapCopyIndex);

        pMapCopy->m_eState = eMapStateCreating;
        
        if (pMapInfo->m_nType == emtDungeon)
        {
            // 副本才有Owner
            bRetCode = g_pSO3GameCenter->m_MapManager.SetMapCopyOwner(
                pRequest->dwMapID, nMapCopyIndex, 
                pPartyLeader ? pPartyLeader->m_dwPlayerID : pRequest->dwPlayerID
            );
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        bRetCode = pMapCopy->RegisterCGCallbackPlayer(
            pRequest->dwPlayerID,
            pRequest->nPosX, pRequest->nPosY, pRequest->nPosZ
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        pRole->m_nDestConnIndex     = nDestConnIndex;
        pRole->m_dwDestMapID        = pRequest->dwMapID;
        pRole->m_nDestMapCopyIndex  = nMapCopyIndex;

		pRole->SetRoleState(rsCG_WaitForCreateMap);

        goto Exit0;
    }

    if (pMapCopy->m_eState == eMapStateCreating)
    {
        int nWaitPlayerCount = (int)(pMapCopy->m_CGCallbackList.size());
        DO_SEARCH_MAP_FAILED_RETURN(nWaitPlayerCount < pMapInfo->m_nMaxPlayerCount, smrcGameOverLoaded);

        bRetCode = pMapCopy->RegisterCGCallbackPlayer(
            pRequest->dwPlayerID,
            pRequest->nPosX, pRequest->nPosY, pRequest->nPosZ
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        pRole->m_nDestConnIndex     = pMapCopy->m_nConnIndex;
        pRole->m_dwDestMapID        = pRequest->dwMapID;
        pRole->m_nDestMapCopyIndex  = nMapCopyIndex;

		pRole->SetRoleState(rsCG_WaitForCreateMap);
        
        goto Exit0;
    }

    DO_SEARCH_MAP_FAILED_RETURN(pMapCopy->m_eState == eMapStateNormal, smrcFailed);

    DO_SEARCH_MAP_FAILED_RETURN(pMapCopy->m_nPlayerCount < pMapInfo->m_nMaxPlayerCount, smrcGameOverLoaded);

    // 找到了，看是不是要跨服务器
    if (pMapCopy->m_nConnIndex == nConnIndex)
    {        
	    // 不跨服务器, 通知GS换地图
        DoSearchMapRespond(
            nConnIndex, pRequest->dwPlayerID, smrcSuccess, false, 
            pRequest->dwMapID, nMapCopyIndex,
            pRequest->nPosX, pRequest->nPosY, pRequest->nPosZ
        );
        
        bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(pRole->m_dwMapID, pRole->m_nMapCopyIndex);
        KGLOG_PROCESS_ERROR(bRetCode);

	    pRole->m_dwMapID        = pRequest->dwMapID;
	    pRole->m_nMapCopyIndex  = nMapCopyIndex;

        g_pSO3GameCenter->m_GameServer.DoSyncFellowshipMapID(pRole->m_dwPlayerID, pRole->m_dwMapID);
        
        bRetCode = g_pSO3GameCenter->m_MapManager.IncreaseMapCopyPlayerCount(pRequest->dwMapID, nMapCopyIndex);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        // 进入跨服流程
	    DoSearchMapRespond(
            nConnIndex, pRequest->dwPlayerID, smrcSuccess, true, 
            pRequest->dwMapID, nMapCopyIndex,
            pRequest->nPosX, pRequest->nPosY, pRequest->nPosZ
        );  

	    pRole->SetRoleState(rsCG_WaitForPlayerData);

        pRole->m_nDestConnIndex     = pMapCopy->m_nConnIndex;
        pRole->m_dwDestMapID        = pRequest->dwMapID;
        pRole->m_nDestMapCopyIndex  = nMapCopyIndex;
        
        bRetCode = g_pSO3GameCenter->m_MapManager.IncreaseMapCopyPlayerCount(pRequest->dwMapID, nMapCopyIndex);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
	return;
}

#undef DO_SEARCH_MAP_FAILED_RETURN


void KGameServer::OnTransferPlayerRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL                            bRetCode            = false;
	S2R_TRANSFER_PLAYER_REQUEST*    pRequest            = (S2R_TRANSFER_PLAYER_REQUEST*)pbyData;
	KRole*                          pRole               = NULL;
    KROLE_STATE                     eRoleState          = rsInvalid;
    const KMapCopy*                 pcDestMapCopy       = NULL;
    IKG_Buffer*                     piPackage           = NULL;
    R2S_TRANSFER_PLAYER_REQUEST*    pTransferPackage    = NULL;

	pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwRoleID);
	KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
	KGLOG_PROCESS_ERROR(eRoleState == rsCG_WaitForPlayerData);
	KGLOG_PROCESS_ERROR(pRole->m_nConnIndex == nConnIndex);

    // "找不到目标地图"在极少数情况下还是可能发生的,比如目标GS宕机
    // 即使找到了,理论上,还有可能是宕机了后再启动起来的,所以下一步还要核对一下连接号
    pcDestMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(
        pRequest->RoleBaseInfo.CurrentPos.dwMapID, 
        pRequest->RoleBaseInfo.CurrentPos.nMapCopyIndex
    );
    if (pcDestMapCopy == NULL || pcDestMapCopy->m_nConnIndex != pRole->m_nDestConnIndex)
    {
        DoTransferPlayerRespond(nConnIndex, pRole->m_dwPlayerID, false);

        pRole->m_dwDestMapID        = 0;
        pRole->m_nDestMapCopyIndex  = 0;
        pRole->SetRoleState(rsOnline);

        goto Exit0;
    }

    KGLOG_PROCESS_ERROR(pcDestMapCopy->m_eState == eMapStateNormal);

	pRole->SetRoleState(rsCG_WaitForGuid);

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_TRANSFER_PLAYER_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pTransferPackage = (R2S_TRANSFER_PLAYER_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pTransferPackage);
    
    pTransferPackage->wProtocolID       = r2s_transfer_player_request;
    pTransferPackage->dwRoleID          = pRequest->dwRoleID;
    
    strncpy(pTransferPackage->szRoleName, pRole->m_szName, sizeof(pTransferPackage->szRoleName));
    pTransferPackage->szRoleName[sizeof(pTransferPackage->szRoleName) - 1] = '\0';

    strncpy(pTransferPackage->szAccount, pRole->m_szAccount, sizeof(pTransferPackage->szAccount));
    pTransferPackage->szAccount[sizeof(pTransferPackage->szAccount) - 1] = '\0';

    pTransferPackage->ExtPointInfo      = pRequest->ExtPointInfo;
    pTransferPackage->nlLeftTime        = pRequest->nlLeftTime;
    pTransferPackage->dwPartyID         = pRole->m_dwTeamID;
    pTransferPackage->BaseInfo          = pRequest->RoleBaseInfo;
    pTransferPackage->nBattleFieldSide  = pRequest->nBattleFieldSide;
    pTransferPackage->dwTongID          = pRole->m_dwTongID;

    bRetCode = Send(pcDestMapCopy->m_nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    KG_COM_RELEASE(piPackage);
	return;
}

void KGameServer::OnTransferPlayerRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL                            bRetCode    = false;
	S2R_TRANSFER_PLAYER_RESPOND*    pRespond    = (S2R_TRANSFER_PLAYER_RESPOND*)pbyData;
	KRole*                          pRole       = NULL;
    const KMapCopy*                 pcMapCopy   = NULL;
    KROLE_STATE                     eRoleState  = rsInvalid; 

	pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwPlayerID);
	KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState(); 

	if (pRespond->bSucceed)
	{
		switch (eRoleState)
		{
		case rsCG_WaitForGuid:
			pRole->SetRoleState(rsCG_WaitForEnter_Leave);
			break;

		case rsCG_WaitForDestTimeout:
            // 等会儿,GS会发来玩家的LeaveGS消息,然后再调pRole->OnLeaveGS(nConnIndex);
			break;

		default:
			assert(false);
		}
	}
	else
	{
        // 跨服被拒绝后,要把目标地图的人数减下来(之前 SearchMap的时候加的)
        bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(pRole->m_dwDestMapID, pRole->m_nDestMapCopyIndex);
        KGLOG_PROCESS_ERROR(bRetCode);

        pRole->m_nDestConnIndex     = 0;
        pRole->m_dwDestMapID        = 0;
        pRole->m_nDestMapCopyIndex  = 0;

		switch (eRoleState)
		{
		case rsCG_WaitForGuid:
			pRole->SetRoleState(rsOnline);
			break;

		case rsCG_WaitForDestTimeout:
			pRole->OnLeaveGS(nConnIndex);
			break;

		default:
			assert(false);
		}
	}

    DoTransferPlayerRespond(
        pRole->m_nConnIndex, pRole->m_dwPlayerID, pRespond->bSucceed, 
        pRespond->dwAddress, pRespond->wPort, &pRespond->Guid
    );

Exit0:
	return;
}


void KGameServer::OnConfirmPlayerLoginRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL                                bRetCode        = false;
	S2R_CONFIRM_PLAYER_LOGIN_REQUEST*   pRequest        = (S2R_CONFIRM_PLAYER_LOGIN_REQUEST*)pbyData;
	KRole*                              pRole           = NULL;
    KMapInfo*                           pMapInfo        = NULL;
    KMapCopy*                           pMapCopy        = NULL;
    const KPARTY*                       pcParty         = NULL;
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

	switch (pRole->GetRoleState())
	{
	case rsLG_WaitForLogin:
		KGLOG_PROCESS_ERROR(pRole->m_nDestConnIndex == nConnIndex);

        pRole->m_nConnIndex         = nConnIndex;
        pRole->m_dwMapID            = pRole->m_dwDestMapID;
        pRole->m_nMapCopyIndex      = pRole->m_nDestMapCopyIndex;

        pRole->m_nDestConnIndex     = 0;
        pRole->m_dwDestMapID        = 0;
        pRole->m_nDestMapCopyIndex  = 0;

        g_pSO3GameCenter->m_GameServer.DoSyncFellowshipMapID(pRole->m_dwPlayerID, pRole->m_dwMapID);

		pRole->SetRoleState(rsOnline);

	    DoConfirmPlayerLoginRespond(nConnIndex, pRequest->dwPlayerID, true);
        break;

	case rsCG_WaitForDestEnter:
		KGLOG_PROCESS_ERROR(pRole->m_nDestConnIndex == nConnIndex);

        pRole->m_dwMapID            = pRole->m_dwDestMapID;
        pRole->m_nMapCopyIndex      = pRole->m_nDestMapCopyIndex;
        pRole->m_nConnIndex         = pRole->m_nDestConnIndex;


        pRole->m_nDestConnIndex     = 0;
        pRole->m_dwDestMapID        = 0;
        pRole->m_nDestMapCopyIndex  = 0;
        
        g_pSO3GameCenter->m_GameServer.DoSyncFellowshipMapID(pRole->m_dwPlayerID, pRole->m_dwMapID);
        pRole->UpdateMapCopyOwnerForPartyLeader();

		pRole->SetRoleState(rsOnline);

        DoConfirmPlayerLoginRespond(nConnIndex, pRequest->dwPlayerID, true);
		break;

	case rsCG_WaitForEnter_Leave:
		KGLOG_PROCESS_ERROR(pRole->m_nDestConnIndex == nConnIndex);
		pRole->SetRoleState(rsCG_WaitForSrcLeave);
		break;

	default:
        assert(false);
	}
    
Exit0:
	return;
}

void KGameServer::OnPlayerLeaveGS(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	S2R_PLAYER_LEAVE_GS*    pPlayerLeave    = (S2R_PLAYER_LEAVE_GS*)pbyData;
	KRole*                  pRole           = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPlayerLeave->dwPlayerID);
	KGLOG_PROCESS_ERROR(pRole);

    pRole->OnLeaveGS(nConnIndex);
Exit0:
	return;
}

void KGameServer::OnInvitePlayerJoinTeamRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                  bRetCode               = false;
	S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST*  pInvitePlayerJoinParty = (S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST*)pbyData;
    KRole*                                pInviteSrc             = NULL;
    KRole*                                pInviteDst             = NULL;
    IKG_Buffer*                           piSendBuffer           = NULL;
    R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST*  pPartyInvitation       = NULL;

    pInvitePlayerJoinParty->szInviteRole[sizeof(pInvitePlayerJoinParty->szInviteRole) - 1] = '\0';

    pInviteSrc = g_pSO3GameCenter->m_RoleManager.GetRole(pInvitePlayerJoinParty->dwInviteSrc);
    KGLOG_PROCESS_ERROR(pInviteSrc);

    if (pInviteSrc->m_dwTeamID != INVALID_PARTY_ID)
    {
        // 邀请发起者已经处于组队状态
        // 1. 检查发起者是不是队长; 2. 检查队伍是否满员
        BOOL   bIsFulled    = true;

        KTeam* pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pInviteSrc->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        // 必须是队长
        KGLOG_PROCESS_ERROR(pInviteSrc->m_dwPlayerID == pTeam->dwAuthority[tatLeader]);
        // 满员?
        for (int i = 0; i < pTeam->nGroupNum; i++)
        {
            if (pTeam->MemberGroup[i].MemberList.size() < MAX_PLAYER_PER_GROUP)
            {
                bIsFulled = false;
                break;
            }
        }

        if (bIsFulled)
        {
            DoPartyMessageNotify(
                nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
                epncYourPartyIsFull, pInvitePlayerJoinParty->szInviteRole
            );
            goto Exit0;
        }

        // 阵营不符合
        if (pTeam->eCamp != cNeutral && pInviteSrc->m_eCamp && pTeam->eCamp != pInviteSrc->m_eCamp)
        {
            DoPartyMessageNotify(
                nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
                epncCampError, pInvitePlayerJoinParty->szInviteRole
            );
            goto Exit0;
        }
    }

    pInviteDst = g_pSO3GameCenter->m_RoleManager.GetRole(pInvitePlayerJoinParty->szInviteRole);
    if (!pInviteDst) 
    {
        DoPartyMessageNotify(
            nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
            epncPlayerInviteNotExist, pInvitePlayerJoinParty->szInviteRole
        );
        goto Exit0;
    }

    // 不能自己邀请自己组队
    KGLOG_PROCESS_ERROR(pInviteSrc != pInviteDst);

    // 如果src没有组队,dest在队伍里面,那么src会申请加入dest的队伍
    if (pInviteSrc->m_dwTeamID == INVALID_PARTY_ID && pInviteDst->m_dwTeamID != INVALID_PARTY_ID)
    {
        DoApplyJoinTeamRequest(pInviteDst, pInviteSrc);
        goto Exit0;
    }

    if (pInviteDst->m_dwTeamID != INVALID_PARTY_ID) 
    {
        if (pInviteDst->m_dwTeamID == pInviteSrc->m_dwTeamID)
        {
            DoPartyMessageNotify(
                nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
                epncPlayerAlreadInYourParty, pInvitePlayerJoinParty->szInviteRole
            );
        }
        else
        {
            DoPartyMessageNotify(
                nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
                epncPlayerAlreadInOtherParty, pInvitePlayerJoinParty->szInviteRole
            );
        }
        goto Exit0;
    }

    if (pInviteDst->GetRoleState() != rsOnline) 
    {
        DoPartyMessageNotify(
            nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
            epncPlayerNotOnline, pInvitePlayerJoinParty->szInviteRole
        );
        goto Exit0;
    }
    
    bRetCode = g_pSO3GameCenter->m_TeamCenter.IsInviteExist(pInviteSrc->m_dwPlayerID, pInviteDst->m_dwPlayerID);
    if (bRetCode)
    {
        DoPartyMessageNotify(
            nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
            epncPlayerIsBusy, pInvitePlayerJoinParty->szInviteRole
        );
        goto Exit0;
    }

    if (pInviteSrc->m_eCamp != cNeutral && pInviteDst->m_eCamp != cNeutral && pInviteSrc->m_eCamp != pInviteDst->m_eCamp)
    {
        DoPartyMessageNotify(
            nConnIndex, pInvitePlayerJoinParty->dwInviteSrc, 
            epncCampError, pInvitePlayerJoinParty->szInviteRole
        );
        goto Exit0;
    }

    g_pSO3GameCenter->m_TeamCenter.RegisterInvite(
        pInvitePlayerJoinParty->dwInviteSrc, pInviteDst->m_dwPlayerID
    );

    // 将邀请发给被邀请玩家所在的服务器
    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPartyInvitation = (R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPartyInvitation);

    pPartyInvitation->wProtocolID      = r2s_invite_player_join_team_request;
    pPartyInvitation->dwInviteDst      = pInviteDst->m_dwPlayerID;

    strncpy(pPartyInvitation->szInviteSrc, pInviteSrc->m_szName, sizeof(pPartyInvitation->szInviteSrc));
    pPartyInvitation->szInviteSrc[sizeof(pPartyInvitation->szInviteSrc) - 1] = '\0';

    bRetCode = Send(pInviteDst->m_nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    KG_COM_RELEASE(piSendBuffer);
}

void KGameServer::OnInvitePlayerJoinTeamRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                  bRetCode          = FALSE;
    S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND*  pInviteRespond    = (S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND*)pbyData;
    KRole*                                pInviteSrc        = NULL;
    KRole*                                pInviteDst        = NULL;
    KTeam*                                pTeam             = NULL;
    DWORD                                 dwTeamID          = INVALID_PARTY_ID;

    pInviteRespond->szInviteSrc[sizeof(pInviteRespond->szInviteSrc) - 1] = '\0';

    pInviteSrc = g_pSO3GameCenter->m_RoleManager.GetRole(pInviteRespond->szInviteSrc);
    KGLOG_PROCESS_ERROR(pInviteSrc);

    pInviteDst = g_pSO3GameCenter->m_RoleManager.GetRole(pInviteRespond->dwInviteDst);
    KGLOG_PROCESS_ERROR(pInviteDst);

    // 核对是否真的发起过邀请，或者邀请是否仍然有效
    bRetCode = g_pSO3GameCenter->m_TeamCenter.IsInviteExist(
        pInviteSrc->m_dwPlayerID, pInviteDst->m_dwPlayerID
    );
    KG_PROCESS_ERROR(bRetCode);

    // 被邀请者可能已加入其他队伍
    KG_PROCESS_ERROR(pInviteDst->m_dwTeamID == INVALID_PARTY_ID);

    // 检查邀请发起者是否仍然在线
    if (!pInviteSrc->IsOnline())
    {
        DoPartyMessageNotify(
            nConnIndex, pInviteDst->m_dwPlayerID, 
            epncInvitationOutOfDate, pInviteSrc->m_szName
        );
        goto Exit0;
    }

    if (!pInviteRespond->bAgreeFlag) 
    {
        DoPartyMessageNotify(
            pInviteSrc->m_nConnIndex, pInviteSrc->m_dwPlayerID, 
            epncInvitationDenied, pInviteDst->m_szName
        );
        goto Exit0;
    }

    // 检查邀请发起者的队伍状态: 要么不在任何队伍中，要么是队长
    if (pInviteSrc->m_dwTeamID == INVALID_PARTY_ID)
    {
        // 创建队伍:
        dwTeamID = g_pSO3GameCenter->m_TeamCenter.CreateTeam();
        KGLOG_PROCESS_ERROR(dwTeamID != INVALID_PARTY_ID);

        pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        bRetCode = g_pSO3GameCenter->m_TeamCenter.AddMember(dwTeamID, pInviteSrc->m_dwPlayerID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        BOOL   bIsNotFull = false;
        
        pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pInviteSrc->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        dwTeamID = pInviteSrc->m_dwTeamID;

        // 确认他还是队长
        if (pTeam->dwAuthority[tatLeader] != pInviteSrc->m_dwPlayerID)
        {
            DoPartyMessageNotify(
                nConnIndex, pInviteDst->m_dwPlayerID,
                epncInvitationOutOfDate, pInviteSrc->m_szName
            );
            goto Exit0;
        }

        // 确认是否满员
        for (int i = 0; i < pTeam->nGroupNum; i++)
        {
            if (pTeam->MemberGroup[i].MemberList.size() < MAX_PLAYER_PER_GROUP)
            {
                bIsNotFull = true;
                break;
            }
        }

        if (!bIsNotFull)
        {
            DoPartyMessageNotify(
                nConnIndex, pInviteDst->m_dwPlayerID, 
                epncInvitationOutOfDate, pInviteSrc->m_szName
            );
            goto Exit0;
        }
    }

    if (pTeam->eCamp != cNeutral && pInviteSrc->m_eCamp != cNeutral && pTeam->eCamp != pInviteSrc->m_eCamp)
    {
        DoPartyMessageNotify(
            nConnIndex, pInviteDst->m_dwPlayerID, 
            epncCampError, pInviteSrc->m_szName
        );
        goto Exit0;
    }

    // 队伍添加成员
    bRetCode = g_pSO3GameCenter->m_TeamCenter.AddMember(dwTeamID, pInviteRespond->dwInviteDst);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    // no matter player agreed or denied, no matter respond to which invatation, clean the "being in invited" flag !
    if (pInviteSrc && pInviteDst)
    {
        g_pSO3GameCenter->m_TeamCenter.UnregisterInvite(pInviteSrc->m_dwPlayerID, pInviteDst->m_dwPlayerID);
    }
}

void KGameServer::OnApplyJoinTeamRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    int                             nRetCode                = false;
    S2R_APPLY_JOIN_TEAM_REQUEST*    pApplyJoinTeamRequest   = (S2R_APPLY_JOIN_TEAM_REQUEST*)pbyData;
    KRole*                          pApplySrc               = NULL;
    KRole*                          pApplyDst               = NULL;
    R2S_APPLY_JOIN_TEAM_REQUEST*    pApplyToDst             = NULL;

    pApplyJoinTeamRequest->szApplyDst[sizeof(pApplyJoinTeamRequest->szApplyDst) - 1] = '\0';

    pApplySrc = g_pSO3GameCenter->m_RoleManager.GetRole(pApplyJoinTeamRequest->dwApplySrc);
    KGLOG_PROCESS_ERROR(pApplySrc);

    pApplyDst = g_pSO3GameCenter->m_RoleManager.GetRole(pApplyJoinTeamRequest->szApplyDst);
    if (!pApplyDst)
    {
        DoPartyMessageNotify(
            nConnIndex, pApplySrc->m_dwPlayerID, 
            epncPlayerApplyNotExist, pApplyJoinTeamRequest->szApplyDst
        );
        goto Exit0;
    }

    // 不能自己申请加入自己的队伍
    KGLOG_PROCESS_ERROR(pApplySrc != pApplyDst); 

    // 目标必须在线
    if (!pApplyDst->IsOnline())
    {
        DoPartyMessageNotify(
            nConnIndex, pApplySrc->m_dwPlayerID, 
            epncPlayerNotOnline, pApplyDst->m_szName
        );
        goto Exit0;
    }

    // 检查申请者是否已经处于组队状态
    if (pApplySrc->m_dwTeamID != INVALID_PARTY_ID)
    {
        DoPartyMessageNotify(
            nConnIndex, pApplySrc->m_dwPlayerID, 
            epncYouAlreadInPartyState, pApplyDst->m_szName
        );
        goto Exit0;
    }

    // 检查对方在队伍中
    KG_PROCESS_ERROR(pApplyDst->m_dwTeamID != INVALID_PARTY_ID);

    nRetCode = DoApplyJoinTeamRequest(pApplyDst, pApplySrc);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KGameServer::OnApplyJoinTeamRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    int                             nRetCode            = false;
    S2R_APPLY_JOIN_TEAM_RESPOND*    pApplyRespond       = (S2R_APPLY_JOIN_TEAM_RESPOND*)pbyData;
    BOOL                            bIsNotFull          = false;
    KRole*                          pPartyLeader        = NULL;
    KRole*                          pApplyPlayer        = NULL;
    KTeam*                          pTeam               = NULL;

    pApplyRespond->szApplySrc[sizeof(pApplyRespond->szApplySrc) - 1] = '\0';

    pPartyLeader = g_pSO3GameCenter->m_RoleManager.GetRole(pApplyRespond->dwApplyDst);
    KGLOG_PROCESS_ERROR(pPartyLeader);

    pApplyPlayer = g_pSO3GameCenter->m_RoleManager.GetRole(pApplyRespond->szApplySrc);
    KGLOG_PROCESS_ERROR(pApplyPlayer);

    // 核对玩家是否真的发起过申请，或者是不是已经覆盖(取消)
    nRetCode = g_pSO3GameCenter->m_TeamCenter.IsApplyExist(pApplyPlayer->m_dwPlayerID, pPartyLeader->m_dwPlayerID);
    if (!nRetCode)
    {
        DoPartyMessageNotify(
            nConnIndex, pPartyLeader->m_dwPlayerID, 
            epncApplicationOutOfDate, pApplyPlayer->m_szName
        );
        goto Exit0;
    }

    // 检查申请者是否已经处于组队状态
    if (pApplyPlayer->m_dwTeamID != INVALID_PARTY_ID || !pApplyPlayer->IsOnline())
    {
        DoPartyMessageNotify(
            nConnIndex, pPartyLeader->m_dwPlayerID, 
            epncApplicationOutOfDate, pApplyPlayer->m_szName
        );
        goto Exit0;
    }

    // 检查队长是否真的是队长
    KGLOG_PROCESS_ERROR(pPartyLeader->m_dwTeamID != INVALID_PARTY_ID);

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pPartyLeader->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(pPartyLeader->m_dwPlayerID == pTeam->dwAuthority[tatLeader]);

    if (!pApplyRespond->bAgreeFlag) 
    {
        // 告诉玩家,对方拒绝了入队申请
        DoPartyMessageNotify(
            pApplyPlayer->m_nConnIndex, pApplyPlayer->m_dwPlayerID, 
            epncApplicationDenied, pPartyLeader->m_szName
        );
        goto Exit0;
    }

    // 确认队伍是否已经满员
    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        if (pTeam->MemberGroup[i].MemberList.size() < MAX_PLAYER_PER_GROUP)
        {
            bIsNotFull = true;
            break;
        }
    }

    if (!bIsNotFull)
    {
        // 告诉申请者,目标队伍已经满员
        DoPartyMessageNotify(
            pApplyPlayer->m_nConnIndex, pApplyPlayer->m_dwPlayerID, 
            epncDestPartyIsFull, pPartyLeader->m_szName
        );
        goto Exit0;
    }

    if (pTeam->eCamp != cNeutral && pApplyPlayer->m_eCamp != cNeutral && pTeam->eCamp != pApplyPlayer->m_eCamp)
    {
        DoPartyMessageNotify(
            pApplyPlayer->m_nConnIndex, pApplyPlayer->m_dwPlayerID, 
            epncCampError, pPartyLeader->m_szName
        );
        goto Exit0;
    }

    // 队伍添加成员
    nRetCode = g_pSO3GameCenter->m_TeamCenter.AddMember(pPartyLeader->m_dwTeamID, pApplyPlayer->m_dwPlayerID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    if (pApplyPlayer && pPartyLeader)
    {
        g_pSO3GameCenter->m_TeamCenter.UnregisterApply(pApplyPlayer->m_dwPlayerID, pPartyLeader->m_dwPlayerID);
    }
}

void KGameServer::OnTeamDelMemberRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode            = FALSE;
    S2R_TEAM_DEL_MEMBER_REQUEST*    pDelMemberRequest   = (S2R_TEAM_DEL_MEMBER_REQUEST*)pbyData;
    KRole*                          pDelRole            = NULL;
    KTeam*                          pTeam               = NULL;
    int                             nTeamCout           = 0;

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pDelMemberRequest->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    pDelRole = g_pSO3GameCenter->m_RoleManager.GetRole(pDelMemberRequest->dwMemberID);
    KGLOG_PROCESS_ERROR(pDelRole);

    if (pDelMemberRequest->dwRequestSrc != pTeam->dwAuthority[tatLeader]) // 除了队长，只有自己能删除自己
    {
        KG_PROCESS_ERROR(pDelMemberRequest->dwMemberID == pDelMemberRequest->dwRequestSrc);
    }

    bRetCode = g_pSO3GameCenter->m_TeamCenter.DelMember(
        pDelMemberRequest->dwPartyID, pDelMemberRequest->dwMemberID
    );
    KG_PROCESS_ERROR(bRetCode);

    pDelRole->m_dwTeamID = INVALID_PARTY_ID;

Exit0:
    return;
}

void KGameServer::OnTeamChangeAuthorityRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode                = FALSE;
    S2R_TEAM_CHANGE_AUTHORITY_REQUEST*  pChangeAuthority        = (S2R_TEAM_CHANGE_AUTHORITY_REQUEST*)pbyData;
    KRole*                              pRequestRole            = NULL;
    KRole*                              pTargetRole             = NULL;
    KTeam*                              pTeam                   = NULL;

    pRequestRole = g_pSO3GameCenter->m_RoleManager.GetRole(pChangeAuthority->dwRequestSrc);
    KGLOG_PROCESS_ERROR(pRequestRole);

    pTargetRole = g_pSO3GameCenter->m_RoleManager.GetRole(pChangeAuthority->dwTargetID);
    KGLOG_PROCESS_ERROR(pTargetRole);

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pChangeAuthority->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    // 只有队长或者权限所有者能变更对应的权限
    KG_PROCESS_ERROR(
        pRequestRole->m_dwPlayerID == pTeam->dwAuthority[tatLeader] ||
        pRequestRole->m_dwPlayerID == pTeam->dwAuthority[pChangeAuthority->byType]
    );

    // 新队长必须在队伍里
    bRetCode = g_pSO3GameCenter->m_TeamCenter.IsRoleInTeam(
        pChangeAuthority->dwPartyID, pChangeAuthority->dwTargetID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_TeamCenter.SetAuthority(
        pChangeAuthority->dwPartyID, (KTEAM_AUTHORITY_TYPE)pChangeAuthority->byType, pChangeAuthority->dwTargetID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pChangeAuthority->byType == tatLeader)
    {
        pTargetRole->UpdateMapCopyOwnerForPartyLeader(); // 副本所属更新
    }

Exit0:
    return;
}

void KGameServer::OnTeamSetLootModeRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode    = FALSE;
    S2R_TEAM_SET_LOOT_MODE_REQUEST* pRequest    = (S2R_TEAM_SET_LOOT_MODE_REQUEST*)pbyData;
    KTeam*                          pTeam       = NULL;
    KRole*                          pRole       = NULL;

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pRequest->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwRequestSrc);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->m_dwPlayerID == pTeam->dwAuthority[tatDistribute]);

    KG_PROCESS_ERROR(pRequest->nLootMode > ilmInvalid && pRequest->nLootMode < ilmTotal); 

    pTeam->nLootMode = pRequest->nLootMode;

    DoTeamSetLootModeNotify(pRequest->dwPartyID, pTeam->nLootMode, pTeam->nRollQuality);

Exit0:
    return;
}

void KGameServer::OnTeamSetRollQualityRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL                                bRetCode = false;
    S2R_TEAM_SET_ROLL_QUALITY_REQUEST*  pRequest = (S2R_TEAM_SET_ROLL_QUALITY_REQUEST*)pbyData;
    KTeam*                          pTeam       = NULL;
    KRole*                          pRole       = NULL;

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pRequest->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwRequestSrc);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->m_dwPlayerID == pTeam->dwAuthority[tatDistribute]);

    pTeam->nRollQuality = pRequest->nRollQuality;

    DoTeamSetLootModeNotify(pRequest->dwPartyID, pTeam->nLootMode, pTeam->nRollQuality);

Exit0:
	return;
}

void KGameServer::OnTeamSetFormationLeader(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                    bRetCode = false;
    S2R_TEAM_SET_FORMATION_LEADER_REQUEST*  pRequest = (S2R_TEAM_SET_FORMATION_LEADER_REQUEST*)pbyData;
    KRole*                                  pRole    = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwNewFormationLeader);
    KGLOG_PROCESS_ERROR(pRole);
    KGLOG_PROCESS_ERROR(pRole->IsOnline());

    bRetCode = g_pSO3GameCenter->m_TeamCenter.SetFormationLeader(
        pRequest->dwPartyID, pRequest->nGourpIndex, pRequest->dwRequestSrc, pRequest->dwNewFormationLeader
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

void KGameServer::OnPartyDisbandRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode                = false;
    S2R_TEAM_DISBAND_REQUEST*   pPartyDisbandRequest    = NULL;
    KTeam*                      pTeam                   = NULL;

    // 处理解散队伍的请求
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_TEAM_DISBAND_REQUEST));
    pPartyDisbandRequest = (S2R_TEAM_DISBAND_REQUEST*)pbyData;
    KGLOG_PROCESS_ERROR(pPartyDisbandRequest);

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pPartyDisbandRequest->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);
    KGLOG_PROCESS_ERROR(pPartyDisbandRequest->dwLeaderID == pTeam->dwAuthority[tatLeader]);

    bRetCode = g_pSO3GameCenter->m_TeamCenter.DestroyTeam(pPartyDisbandRequest->dwPartyID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnTeamSyncMemberMaxLMR(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode        = FALSE;
    S2R_SYNC_TEAM_MEMBER_MAX_LMR*   pS2RPak         = NULL;
    IKG_Buffer*                     piSendBuffer    = NULL;
    R2S_SYNC_TEAM_MEMBER_MAX_LMR*   pR2SPak         = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_SYNC_TEAM_MEMBER_MAX_LMR));

    pS2RPak = (S2R_SYNC_TEAM_MEMBER_MAX_LMR*)pbyData;
    KGLOG_PROCESS_ERROR(pS2RPak);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_TEAM_MEMBER_MAX_LMR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pR2SPak = (R2S_SYNC_TEAM_MEMBER_MAX_LMR*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pR2SPak);

    pR2SPak->wProtocolID      = r2s_sync_team_member_max_lmr;
    pR2SPak->dwPartyID        = pS2RPak->dwPartyID;
    pR2SPak->dwMemberID       = pS2RPak->dwMemberID;
    pR2SPak->nMaxLife         = pS2RPak->nMaxLife;
    pR2SPak->nMaxMana         = pS2RPak->nMaxMana;

    Broadcast(piSendBuffer);

Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return ;
}

void KGameServer::OnTeamSyncMemberCurrentLMR(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode        = FALSE;
    S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*   pS2RPak         = NULL;
    IKG_Buffer*                         piSendBuffer    = NULL;
    R2S_SYNC_TEAM_MEMBER_CURRENT_LMR*   pR2SPak         = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_SYNC_TEAM_MEMBER_CURRENT_LMR));

    pS2RPak = (S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*)pbyData;
    KGLOG_PROCESS_ERROR(pS2RPak);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_TEAM_MEMBER_CURRENT_LMR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pR2SPak = (R2S_SYNC_TEAM_MEMBER_CURRENT_LMR*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pR2SPak);

    pR2SPak->wProtocolID      = r2s_sync_team_member_current_lmr;
    pR2SPak->dwPartyID        = pS2RPak->dwPartyID;
    pR2SPak->dwMemberID       = pS2RPak->dwMemberID;
    pR2SPak->byLifePercent    = pS2RPak->byLifePercent;
    pR2SPak->byManaPercent    = pS2RPak->byManaPercent;

    Broadcast(piSendBuffer);

Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return ;
}

void KGameServer::OnTeamSyncMemberMisc(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode        = FALSE;
    S2R_SYNC_TEAM_MEMBER_MISC*         pS2RPak         = NULL;
    IKG_Buffer*                         piSendBuffer    = NULL;
    R2S_SYNC_TEAM_MEMBER_MISC*         pR2SPak         = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_SYNC_TEAM_MEMBER_MISC));

    pS2RPak = (S2R_SYNC_TEAM_MEMBER_MISC*)pbyData;
    KGLOG_PROCESS_ERROR(pS2RPak);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_TEAM_MEMBER_MISC));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pR2SPak = (R2S_SYNC_TEAM_MEMBER_MISC*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pR2SPak);

    pR2SPak->wProtocolID      = r2s_sync_team_member_misc;
    pR2SPak->dwPartyID        = pS2RPak->dwPartyID;
    pR2SPak->dwMemberID       = pS2RPak->dwMemberID;

    pR2SPak->PortraitInfo     = pS2RPak->PortraitInfo;

    pR2SPak->dwForceID        = pS2RPak->dwForceID;
    pR2SPak->byCamp           = pS2RPak->byCamp;
    pR2SPak->byLevel          = pS2RPak->byLevel;
	pR2SPak->byRoleType       = pS2RPak->byRoleType;
    pR2SPak->bDeathFlag       = pS2RPak->bDeathFlag;

    Broadcast(piSendBuffer);

Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return ;
}

void KGameServer::OnTeamSyncMemberPosition(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode        = false;
    S2R_SYNC_TEAM_MEMBER_POSITION*  pS2RPak         = NULL;
    IKG_Buffer*                     piSendBuffer    = NULL;
    R2S_SYNC_TEAM_MEMBER_POSITION*  pR2SPak         = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_SYNC_TEAM_MEMBER_POSITION));

    pS2RPak = (S2R_SYNC_TEAM_MEMBER_POSITION*)pbyData;
    KGLOG_PROCESS_ERROR(pS2RPak);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_TEAM_MEMBER_POSITION));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pR2SPak = (R2S_SYNC_TEAM_MEMBER_POSITION*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pR2SPak);

    pR2SPak->wProtocolID      = r2s_sync_team_member_position;
    pR2SPak->dwPartyID        = pS2RPak->dwPartyID;
    pR2SPak->dwMemberID       = pS2RPak->dwMemberID;
    pR2SPak->dwMapID          = pS2RPak->dwMapID;
    pR2SPak->nMapCopyIndex    = pS2RPak->nMapCopyIndex;
    pR2SPak->nPosX            = pS2RPak->nPosX;
	pR2SPak->nPosY            = pS2RPak->nPosY;

    Broadcast(piSendBuffer);

Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return ;
}

void KGameServer::OnTeamSetMarkRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    S2R_TEAM_SET_MARK_REQUEST*  pS2RPak     = NULL;
    KTeam*                      pTeam       = NULL;
    KRole*                      pRole       = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_TEAM_SET_MARK_REQUEST));

    pS2RPak = (S2R_TEAM_SET_MARK_REQUEST*)pbyData;
    KGLOG_PROCESS_ERROR(pS2RPak);

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pS2RPak->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pS2RPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pTeam->dwAuthority[tatMark] == pRole->m_dwPlayerID);

    bRetCode = g_pSO3GameCenter->m_TeamCenter.SetMark(
        pS2RPak->dwPartyID, pS2RPak->nMarkType, pS2RPak->dwTargetID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

BOOL KGameServer::KTALK_TONG_TRAVERSE::operator () (KTongMember* pMember)
{
    KRole* pReceiver = g_pSO3GameCenter->m_RoleManager.GetRole(pMember->dwID);
    if (!pReceiver)
        return true;

    pDownMsg->dwReceiverID = pReceiver->m_dwPlayerID;
    pGameServer->Send(pReceiver->m_nConnIndex, piSendBuff);

    return true;
}

void KGameServer::OnTalkMessage(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                     bRetCode           = FALSE;
    S2R_TALK_MESSAGE*        pUpMsg             = NULL;
    size_t                   uTalkDataSize      = 0;
    IKG_Buffer*              piSendBuffer       = NULL;
    R2S_TALK_MESSAGE*        pDownMsg           = NULL;

    pUpMsg = (S2R_TALK_MESSAGE*)pbyData;
    KGLOG_PROCESS_ERROR(pUpMsg);

    pUpMsg->szTalker[sizeof(pUpMsg->szTalker) - 1] = '\0';
    pUpMsg->szReceiver[sizeof(pUpMsg->szReceiver) - 1] = '\0';

    uTalkDataSize = uDataLen - sizeof(S2R_TALK_MESSAGE);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_TALK_MESSAGE) + uTalkDataSize));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDownMsg = (R2S_TALK_MESSAGE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDownMsg);
    
    pDownMsg->wProtocolID      = r2s_talk_message;
    pDownMsg->byMsgType        = pUpMsg->byMsgType;
	pDownMsg->dwTalkerID       = pUpMsg->dwTalkerID;
    pDownMsg->dwReceiverID     = pUpMsg->dwReceiverID;

    strcpy(pDownMsg->szTalker, pUpMsg->szTalker);
    strcpy(pDownMsg->szReceiver, pUpMsg->szReceiver);

    memcpy(pDownMsg->byTalkData, pUpMsg->byTalkData, uTalkDataSize);

    switch(pUpMsg->byMsgType)
    {
    case trWhisper:
        {
            KRole* pReceiver = g_pSO3GameCenter->m_RoleManager.GetRole(pUpMsg->szReceiver);
            KRole* pTalker = g_pSO3GameCenter->m_RoleManager.GetRole(pUpMsg->dwTalkerID);
            KGLOG_PROCESS_ERROR(pTalker);

            // 密聊时,上行协议的dwReceiverID是无效的,需要根据目标的名字查找到目标玩家的ID
            if (pReceiver == NULL)
            {
                DoPlayerTalkError(nConnIndex, pUpMsg->dwTalkerID, TALK_ERROR_E_PLAYER_NOT_FOUND);
                goto Exit0;
            }

            if (!pReceiver->IsOnline())
            {
                DoPlayerTalkError(nConnIndex, pUpMsg->dwTalkerID, TALK_ERROR_E_PLAYER_OFFLINE);
                goto Exit0;
            }

            pDownMsg->dwReceiverID = pReceiver->m_dwPlayerID;
            Send(pReceiver->m_nConnIndex, piSendBuffer);

            DoPlayerTalkWhisperSuccessRespond(nConnIndex, pUpMsg->dwTalkerID, pReceiver->m_dwPlayerID);
        }
        break;

    case trParty:
        {
            // 队聊时,要取对应的队伍ID
            KRole* pTalker = g_pSO3GameCenter->m_RoleManager.GetRole(pUpMsg->dwTalkerID);
            KGLOG_PROCESS_ERROR(pTalker);

            KG_PROCESS_ERROR(pTalker->m_dwTeamID == pUpMsg->dwReceiverID);

            pDownMsg->dwReceiverID = pTalker->m_dwTeamID;

            Broadcast(piSendBuffer);
        }
        break;

    case trTong:
        {
            KRole*              pTalker = NULL;
            KTong*              pTong   = NULL;
            KTALK_TONG_TRAVERSE TalkTongTraverse;

            pTalker = g_pSO3GameCenter->m_RoleManager.GetRole(pUpMsg->dwTalkerID);
            KGLOG_PROCESS_ERROR(pTalker);

            if (!pTalker->m_dwTongID)
            {
                DoPlayerTalkError(nConnIndex, pUpMsg->dwTalkerID, TALK_ERROR_E_NOT_IN_TONG);
                goto Exit0;
            }

            pTong = g_pSO3GameCenter->m_TongManager.GetTong(pTalker->m_dwTongID);
            KGLOG_PROCESS_ERROR(pTong);

            if (!pTong->CanSpeak(pTalker->m_dwPlayerID))
            {
                DoPlayerTalkError(nConnIndex, pUpMsg->dwTalkerID, TALK_ERROR_E_TONG_CAN_NOT_SPEAK);
                goto Exit0;
            }

            TalkTongTraverse.pDownMsg    = pDownMsg;
            TalkTongTraverse.piSendBuff  = piSendBuffer;
            TalkTongTraverse.pGameServer = this;
            pTong->TraverseMember(TalkTongTraverse);
        }
        break;

    case trGlobalSys:
        Broadcast(piSendBuffer);
        break;

    default:
        KGLOG_PROCESS_ERROR(!"Unexcepted message type. ");
    }

Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return ;
}

void KGameServer::OnPlayerTalkError(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bRetCode    = false;
    S2R_PLAYER_TALK_ERROR*  pPak        = NULL;
    KRole*                  pRole       = NULL;

    pPak = (S2R_PLAYER_TALK_ERROR*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwReceiverID != ERROR_ID);
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwReceiverID);
    KGLOG_PROCESS_ERROR(pRole);

    bRetCode = DoPlayerTalkError(pRole->m_nConnIndex, pPak->dwReceiverID, pPak->nErrorCode);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    return ;
}

// --------------------- 好友相关 --------------------------------

void KGameServer::OnApplyFellowshipDataRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode    = false;
    S2R_APPLY_FELLOWSHIP_DATA_REQUEST*  pPak        = NULL;

    pPak = (S2R_APPLY_FELLOWSHIP_DATA_REQUEST*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwPlayerID != ERROR_ID);

    bRetCode = g_pSO3GameCenter->m_FellowshipCenter.GetFellowshipData(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return ;
}

void KGameServer::OnUpdateFellowshipData(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    S2R_UPDATE_FELLOWSHIP_DATA* pPak        = NULL;
    
    pPak = (S2R_UPDATE_FELLOWSHIP_DATA*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwPlayerID != ERROR_ID);

    bRetCode = g_pSO3GameCenter->m_FellowshipCenter.UpdateFellowshipData(
        pPak->dwPlayerID, 
        uDataLen - sizeof(S2R_UPDATE_FELLOWSHIP_DATA), 
        pPak->byData
    );

    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return ;
}

void KGameServer::OnAddFellowshipRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    S2R_ADD_FELLOWSHIP_REQUEST* pPak        = NULL;
    KRole*                      pRole       = NULL;
    DWORD                       dwPlayerID  = ERROR_ID;

    pPak = (S2R_ADD_FELLOWSHIP_REQUEST*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwPlayerID != ERROR_ID);

    pPak->szAlliedPlayerName[sizeof(pPak->szAlliedPlayerName) - sizeof('\0')] = '\0';

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->szAlliedPlayerName);
    if (pRole)
    {
        assert(pRole->m_dwPlayerID != ERROR_ID);
        dwPlayerID = pRole->m_dwPlayerID;
    }

    DoAddFellowshipRespond(pPak->dwPlayerID, dwPlayerID, pPak->szAlliedPlayerName, pPak->byType);

Exit0:
    return ;
}

void KGameServer::OnGetFellowshipNameRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode    = false;
    S2R_GET_FELLOWSHIP_NAME_REQUEST*    pPak        = NULL;
    KRole*                              pRole       = NULL;

    pPak = (S2R_GET_FELLOWSHIP_NAME_REQUEST*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwPlayerID != ERROR_ID);

    DoGetFellowshipNameRespond(pPak->dwPlayerID, pPak->byCount, pPak->dwAlliedPlayerIDArray);

Exit0:
    return ;
}

void KGameServer::OnApplyFellowshipPlayerLevelAndForceID(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                            bRetCode    = false;
    S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID*  pPak        = NULL;
    KRole*                                          pRole       = NULL;
    int                                             nLevel      = 0;

    pPak = (S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwPlayerID != ERROR_ID);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    if (pRole->IsOnline())
        nLevel = pRole->m_nLevel;

    DoSyncFellowshipPlayerLevel(pRole->m_dwPlayerID, nLevel, false);
    DoSyncFellowshipPlayerForceID(pRole->m_dwPlayerID, pRole->m_dwForceID);

Exit0:
    return ;
}

void KGameServer::OnAddFellowshipNotify(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    S2R_ADD_FELLOWSHIP_NOTIFY*  pPak        = NULL;
    KRole*                      pRole       = NULL;
    int                         nLevel      = 0;

    pPak = (S2R_ADD_FELLOWSHIP_NOTIFY*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(pPak->dwPlayerID != ERROR_ID);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    DoAddFellowshipNotify(pRole->m_nConnIndex, pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, pPak->nType);

Exit0:
    return ;
}

// --------------------- 邮件相关 --------------------------------
void KGameServer::OnSendMailRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    int                     nResult      = mrcFailed;
    BOOL                    bRetCode     = false;
    DWORD                   dwSrcID      = 0;
    DWORD                   dwDstID      = 0;
    KRole*                  pDstRole     = NULL;
    S2R_SEND_MAIL_REQUEST*  pMailData    = (S2R_SEND_MAIL_REQUEST*)pbyData;

    KGLOG_PROCESS_ERROR(
        uDataLen == (sizeof(S2R_SEND_MAIL_REQUEST) + pMailData->Content.wTextSize + pMailData->Content.wItemSize)
    );

    pMailData->szSenderName[sizeof(pMailData->szSenderName) - 1] = '\0';
    pMailData->szReceiverName[sizeof(pMailData->szReceiverName) - 1] = '\0';
    pMailData->szTitle[sizeof(pMailData->szTitle) - 1] = '\0';

    pDstRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMailData->szReceiverName);
    KG_PROCESS_ERROR_RET_CODE(pDstRole, mrcDstNotExist);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        (int)pMailData->byRequestID, pDstRole->m_dwPlayerID, pMailData->nDelaySecond, pMailData->szSenderName,
        pMailData->szTitle, &(pMailData->Content), pMailData->bSystemMail
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = mrcSucceed;
Exit0:
    if (nResult != mrcSucceed && !pMailData->bSystemMail)
    {
        KRole*  pSrcRole = NULL;

        pSrcRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMailData->szSenderName);
        if (pSrcRole)
        {
            DoSendMailRespond(
                pSrcRole->m_nConnIndex, pMailData->byRequestID, pSrcRole->m_dwPlayerID, (BYTE)nResult, &pMailData->Content
            ); 
        }
    }
    return;
}

void KGameServer::OnSendGlobalMailRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode     = false;
    S2R_SEND_GLOBAL_MAIL_REQUEST*   pMailData    = (S2R_SEND_GLOBAL_MAIL_REQUEST*)pbyData;

    KGLOG_PROCESS_ERROR(
        uDataLen == (sizeof(S2R_SEND_GLOBAL_MAIL_REQUEST) + pMailData->Content.wTextSize + pMailData->Content.wItemSize)
    );

    pMailData->szSenderName[sizeof(pMailData->szSenderName) - 1] = '\0';
    pMailData->szTitle[sizeof(pMailData->szTitle) - 1] = '\0';

    bRetCode = g_pSO3GameCenter->m_MiscDB.DoSendGlobalMail(
        pMailData->nLifeTime, pMailData->szSenderName,
        pMailData->szTitle, &(pMailData->Content)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnGetMailListRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    DWORD                       dwPlayerID  = 0;
    KRole*                      pRole       = NULL;
    S2R_GET_MAILLIST_REQUEST*   pPak        = NULL;
    KROLE_STATE                 eRoleState  = rsInvalid;

    pPak = (S2R_GET_MAILLIST_REQUEST*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState == rsOnline);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestGetMailList(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnGetMailContent(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bRetCode        = false;
    S2R_GET_MAIL_CONTENT*   pPak            = NULL;
    KRole*                  pRole           = NULL;
    KROLE_STATE             eRoleState      = rsInvalid;

    pPak = (S2R_GET_MAIL_CONTENT*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState == rsOnline);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestGetMailContent(
        pPak->dwPlayerID, pPak->dwMailID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnGetMailAttachment(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode        = false;
    S2R_GET_MAIL_ATTACHMENT*    pPak            = NULL;
    KRole*                      pRole           = NULL;
    KROLE_STATE                 eRoleState      = rsInvalid;

    pPak = (S2R_GET_MAIL_ATTACHMENT*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState == rsOnline);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestAcquireMailAttachment(
        pPak->dwPlayerID, pPak->dwMailID, 
        pPak->bMoneyFlag, pPak->bTextFlag, pPak->bItemFlag
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnSetMailRead(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                bRetCode    = false;
    S2R_SET_MAIL_READ*  pPak        = NULL;
    KRole*              pRole       = NULL;
    KROLE_STATE         eRoleState  = rsInvalid;

    pPak = (S2R_SET_MAIL_READ*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState == rsOnline);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestSetMailRead(
        pPak->dwPlayerID, pPak->dwMailID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnDeleteMail(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                bRetCode    = false;
    S2R_DELETE_MAIL*    pPak        = NULL;
    KRole*              pRole       = NULL;
    KROLE_STATE         eRoleState  = rsInvalid;

    pPak = (S2R_DELETE_MAIL*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState == rsOnline);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestDeleteMail(
        pPak->dwPlayerID, pPak->dwMailID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnReturnMail(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                bRetCode    = false;
    unsigned            uPakSize    = 0;
    S2R_RETURN_MAIL*    pPak        = NULL;
    KRole*              pRole       = NULL;
    KROLE_STATE         eRoleState  = rsInvalid;

    pPak = (S2R_RETURN_MAIL*)pbyData;
    KGLOG_PROCESS_ERROR(pPak);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState == rsOnline);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestUserReturnMail(
        pPak->dwPlayerID, pPak->dwMailID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}
void KGameServer::OnUpdateMailData(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bRetCode    = false;
    S2R_UPDATE_MAIL_DATA*   pPak        = (S2R_UPDATE_MAIL_DATA*)pbyData;

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestReleaseMailAttachment(
        pPak->dwPlayerID, pPak->dwMailID, 
        pPak->bMoneyFlag, pPak->bTextFlag, pPak->bItemFlag
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnSyncMidMapMark(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
	BOOL		bRetCode = FALSE;
	S2R_SYNC_MID_MAP_MARK* pS2RPak = NULL;
	IKG_Buffer*    piSendBuffer    = NULL;
	R2S_SYNC_MID_MAP_MARK* pR2SPak = NULL;

	pS2RPak = (S2R_SYNC_MID_MAP_MARK*)pbyData;
	KGLOG_PROCESS_ERROR(pS2RPak);

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_MID_MAP_MARK));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pR2SPak = (R2S_SYNC_MID_MAP_MARK*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pR2SPak);

	pR2SPak->wProtocolID		= r2s_sync_mid_map_mark;
	pR2SPak->dwPartyID			= pS2RPak->dwPartyID;
	pR2SPak->dwSrcPlayerID		= pS2RPak->dwSrcPlayerID;
	pR2SPak->nMapID				= pS2RPak->nMapID;
	pR2SPak->nX					= pS2RPak->nX;
	pR2SPak->nY					= pS2RPak->nY;
	pR2SPak->nType				= pS2RPak->nType;
	strncpy(pR2SPak->szComment, pS2RPak->szComment, MAX_MIDMAP_MARK_COMMENT_LEN);

	Broadcast(piSendBuffer);
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return;
}

void KGameServer::OnSaveScenePlayerList(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode        = false;
    S2R_SAVE_SCENE_PLAYER_LIST* pRequest        = (S2R_SAVE_SCENE_PLAYER_LIST*)pbyData;
    KMapInfo*                   pMapInfo        = NULL;
    KMapCopy*                   pMapCopy        = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_SAVE_SCENE_PLAYER_LIST) + sizeof(DWORD) * pRequest->nPlayerCount);
    
    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(pRequest->dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo && pMapInfo->m_nType == emtDungeon);
    
    pMapCopy = pMapInfo->GetMapCopy(pRequest->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    KGLOG_PROCESS_ERROR(pMapCopy->m_eState == eMapStateNormal);
    
    for (int i = 0; i < pRequest->nPlayerCount; i++)
    {
        pMapCopy->m_PlayerList.insert(pRequest->dwPlayerList[i]);
    }
    
    pMapCopy->m_bIsChanged = true;

Exit0:
    return;
}

void KGameServer::OnSaveSceneData(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bRetCode        = false;
    S2R_SAVE_SCENE_DATA*    pData           = (S2R_SAVE_SCENE_DATA*)pbyData;
    IKG_Buffer*             piSceneData     = NULL;
    BYTE*                   pbySceneData    = NULL;
    size_t                  uSceneDataSize  = 0;
    KMapInfo*               pMapInfo        = NULL;
    KMapCopy*               pMapCopy        = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_SAVE_SCENE_DATA) + pData->uDataLen);
    
    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(pData->dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);
    
    pMapCopy = pMapInfo->GetMapCopy(pData->nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    KGLOG_PROCESS_ERROR(pMapCopy->m_eState == eMapStateNormal);
    
    piSceneData = KG_MemoryCreateBuffer((unsigned)pData->uDataLen);
    KGLOG_PROCESS_ERROR(piSceneData);

    pbySceneData = (BYTE*)piSceneData->GetData();
    KGLOG_PROCESS_ERROR(pbySceneData);

    memcpy(pbySceneData, pData->byData, pData->uDataLen);

    KG_COM_RELEASE(pMapCopy->m_piSceneData);

    piSceneData->AddRef();
    pMapCopy->m_piSceneData = piSceneData;

    pMapCopy->m_bIsChanged = true;

Exit0:
    KG_COM_RELEASE(piSceneData);
    return;
}

void KGameServer::OnResetMapCopyRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode        = false;
    S2R_RESET_MAP_COPY_REQUEST* pRequest        = (S2R_RESET_MAP_COPY_REQUEST*)pbyData;
    KMapCopy*                   pMapCopy        = NULL;
    KMapInfo*                   pMapInfo        = NULL;
    int                         nMapCopyIndex   = 0;
    
    KGLOG_PROCESS_ERROR(pRequest->dwPlayerID != ERROR_ID);
    
    nMapCopyIndex = pRequest->nMapCopyIndex;

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(pRequest->dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    KGLOG_PROCESS_ERROR(pMapInfo->m_bManualReset); // 该地图必须能够手动重置

    if (nMapCopyIndex == 0)
    {
        pMapInfo->ResetOwnerCopy(pRequest->dwPlayerID);
        goto Exit0;
    }

    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pRequest->dwMapID, nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    KGLOG_PROCESS_ERROR(pRequest->dwPlayerID == pMapCopy->m_dwOwnerID); // 只有副本的所有者才能重置

    bRetCode = pMapCopy->CanReset();
    KG_PROCESS_ERROR(bRetCode);

    DoDeleteMapNotify(pMapCopy->m_nConnIndex, pRequest->dwMapID, nMapCopyIndex);

    pMapInfo->DeleteMapCopy(nMapCopyIndex);

Exit0:
    return;
}

void KGameServer::OnPlayerEnterSceneNotify(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_PLAYER_ENTER_SCENE_NOTIFY*   pNotify   = (S2R_PLAYER_ENTER_SCENE_NOTIFY*)pbyData;
    
    g_pSO3GameCenter->m_MapManager.OnPlayerEnterMapCopy(
        pNotify->dwPlayerID, pNotify->dwMapID, pNotify->nMapCopyIndex
    );

    return;
}

void KGameServer::OnApplyPlayerSavedCopyRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_APPLY_PLAYER_SAVED_COPY_REQUEST*    pRequest      = (S2R_APPLY_PLAYER_SAVED_COPY_REQUEST*)pbyData;
    vector<KSAVED_COPY>                     SavedCopy;

    g_pSO3GameCenter->m_MapManager.GetPlayerSavedCopy(pRequest->dwPlayerID, SavedCopy);
    
    DoApplyPlayerSavedCopyRespond(nConnIndex, pRequest->dwPlayerID, SavedCopy);
    
    return;
}

void KGameServer::OnSyncRoadTrackForce(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piPackage       = NULL;
    S2R_SYNC_ROAD_TRACK_FORCE*  pRoadTrackForce  = (S2R_SYNC_ROAD_TRACK_FORCE*)pbyData;
    R2S_SYNC_ROAD_TRACK_FORCE*  BroadCastForce  = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_ROAD_TRACK_FORCE));
    KGLOG_PROCESS_ERROR(piPackage);

    BroadCastForce              = (R2S_SYNC_ROAD_TRACK_FORCE*)piPackage->GetData();
    BroadCastForce->wProtocolID = r2s_sync_road_track_force;
    BroadCastForce->nNodeID     = pRoadTrackForce->nNodeID;
    BroadCastForce->dwForceID   = pRoadTrackForce->dwForceID;

    Broadcast(piPackage);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return;
}

void KGameServer::OnLoadRoleDataRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    //S2R_LOAD_ROLE_DATA_REQUEST* pLoadRole = (S2R_LOAD_ROLE_DATA_REQUEST*)pbyData;
    //
    //g_pSO3GameCenter->m_RoleDB.DoLoadRoleData(nConnIndex, pLoadRole->dwPlayerID);

    g_nTestTime = 0;
    return;
}

void KGameServer::OnSyncRoleData(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                bResult         = false;
    S2R_SYNC_ROLE_DATA* pSyncData       = (S2R_SYNC_ROLE_DATA*)pbyData;
    KGS_INFO*           pGSInfo         = NULL; 
    BYTE*               pbyOffset       = NULL;
    size_t              uRoleDataLen    = uDataLen - sizeof(S2R_SYNC_ROLE_DATA);

    pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
    KGLOG_PROCESS_ERROR(pGSInfo);
    g_nTestConnection = nConnIndex;

    KGLOG_PROCESS_ERROR(pGSInfo->uRoleDataOffset == pSyncData->uOffset);
    KGLOG_PROCESS_ERROR(sizeof(pGSInfo->byRoleDataBuffer) - pGSInfo->uRoleDataOffset >= uRoleDataLen);
    KGLOG_PROCESS_ERROR(pSyncData->uOffset == 0 || pSyncData->dwRoleID == pGSInfo->dwRoleID);

    pbyOffset = pGSInfo->byRoleDataBuffer + pGSInfo->uRoleDataOffset;
    
    memcpy(pbyOffset, pSyncData->byData, uRoleDataLen);

    pGSInfo->uRoleDataOffset += uRoleDataLen;
    pGSInfo->dwRoleID         = pSyncData->dwRoleID;

    bResult = true;
Exit0:
    if(!bResult)
    {
        pGSInfo->dwRoleID        = ERROR_ID;
        pGSInfo->uRoleDataOffset = 0;
    }
    return;
}

void KGameServer::OnSaveRoleData(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                bResult     = false;
    S2R_SAVE_ROLE_DATA* pSaveData   = (S2R_SAVE_ROLE_DATA*)pbyData;
    KGS_INFO*           pGSInfo     = NULL; 
    IKG_Buffer*         piRoleData  = NULL;
    void*               pvRoleData  = NULL;
    BYTE*               pRoleData   = NULL;
    char                szFileName[100];
    FILE*               fp          = NULL;

    pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
    KGLOG_PROCESS_ERROR(pGSInfo);

    KGLOG_PROCESS_ERROR(pSaveData->uRoleDataLen == pGSInfo->uRoleDataOffset);
    KGLOG_PROCESS_ERROR(pSaveData->dwRoleID == pGSInfo->dwRoleID);

    piRoleData = KG_MemoryCreateBuffer((unsigned)pSaveData->uRoleDataLen);
    KGLOG_PROCESS_ERROR(piRoleData);
    
    pvRoleData = piRoleData->GetData();
    KGLOG_PROCESS_ERROR(pvRoleData);

    memcpy(pvRoleData, pGSInfo->byRoleDataBuffer, pSaveData->uRoleDataLen);

    //g_pSO3GameCenter->m_RoleDB.DoSaveRoleData(
    //    nConnIndex, pSaveData->dwRoleID, pSaveData->BaseInfo, piRoleData,pSaveData->nUserValue
    //);

    pRoleData = (BYTE*)pvRoleData;

    snprintf(szFileName, sizeof(szFileName), "savelog/Save_Center%d", pGSInfo->dwRoleID);
    szFileName[sizeof(szFileName) - 1] = '\0';

    fp = fopen(szFileName, "a");
    KG_PROCESS_ERROR(fp);

    for (size_t nAddCount = 0; nAddCount < pSaveData->uRoleDataLen; nAddCount++)
    {
        fprintf(fp, "%d", pRoleData[nAddCount]);
    }

    fclose(fp);
    fp = NULL;
    //Test Code End
    
    bResult = true;
Exit0:
    pGSInfo->uRoleDataOffset = 0;
    pGSInfo->dwRoleID        = ERROR_ID;
    KG_COM_RELEASE(piRoleData);
    return;
}

void KGameServer::OnChangeRoleLevelRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_CHANGE_ROLE_LEVEL_REQUEST*  pChangeRoleLevel = (S2R_CHANGE_ROLE_LEVEL_REQUEST*)pbyData;
    KRole*                          pRole            = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pChangeRoleLevel->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    pRole->m_nLevel = pChangeRoleLevel->byLevel;

    DoSyncFellowshipPlayerLevel(pRole->m_dwPlayerID, pRole->m_nLevel, true);

Exit0:
    return;
}

void KGameServer::OnChangeRoleForceIDRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_CHANGE_ROLE_FORCEID_REQUEST* pChangeRoleForceID = (S2R_CHANGE_ROLE_FORCEID_REQUEST*)pbyData;
    KRole*                           pRole              = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pChangeRoleForceID->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    pRole->m_dwForceID = pChangeRoleForceID->byForceID;

    DoSyncFellowshipPlayerForceID(pRole->m_dwPlayerID, pRole->m_dwForceID);

Exit0:
    return;
}

void KGameServer::OnSendGmMessage(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_SEND_GM_MESSAGE* pGmMessage = (S2R_SEND_GM_MESSAGE*)pbyData;

    pGmMessage->szGmName[sizeof(pGmMessage->szGmName) - 1] = '\0';
    pGmMessage->szMessage[uDataLen - sizeof(S2R_SEND_GM_MESSAGE) - 1] = '\0';

    g_pSO3GameCenter->m_Eyes.DoSendGmMessage(pGmMessage->szGmName, pGmMessage->szMessage);

    return;
}

void KGameServer::OnSendGMCommandGS(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bRetCode     = false;
    S2R_SEND_GM_COMMAND_GS* pGMCommandGS = (S2R_SEND_GM_COMMAND_GS*)pbyData;
    KRole*                  pRole        = NULL;

    KGLOG_PROCESS_ERROR(uDataLen > sizeof(S2R_SEND_GM_COMMAND_GS));

    pGMCommandGS->szRoleName[sizeof(pGMCommandGS->szRoleName) - 1] = '\0';

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pGMCommandGS->szRoleName);
    KGLOG_PROCESS_ERROR(pRole && "Role Not Found.");

    bRetCode = pRole->IsOnline();
    KGLOG_PROCESS_ERROR(bRetCode && "Role Not Online.");

    pGMCommandGS->szCommand[uDataLen - sizeof(S2R_SEND_GM_COMMAND_GS) - 1] = '\0';

    bRetCode = g_pSO3GameCenter->m_GameServer.DoGmCommand(
        pRole->m_nConnIndex, pRole->m_dwPlayerID, 
        "", pGMCommandGS->szCommand
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnSendGMCommandGC(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                    bRetCode        = NULL;
    S2R_SEND_GM_COMMAND_GC* pGMCommandGC    = (S2R_SEND_GM_COMMAND_GC*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen > sizeof(S2R_SEND_GM_COMMAND_GC));

    pGMCommandGC->szCommand[uDataLen - sizeof(S2R_SEND_GM_COMMAND_GC) - 1] = '\0';

    KGLogPrintf(KGLOG_INFO, "GS[%d] CMD: %s\n", nConnIndex, pGMCommandGC->szCommand);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.ExecuteGMCScript("", pGMCommandGC->szCommand);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnJoinBattleFieldQueueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BATTLE_FIELD_RESULT_CODE                nRetCode        = bfrcFailed;
    S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST*    pJoinRequest    = (S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST*)pbyData;

    nRetCode = g_pSO3GameCenter->m_BattleFieldManager.JoinQueue(
        pJoinRequest->dwRoleID, pJoinRequest->dwMapID, pJoinRequest->nCopyIndex,
        pJoinRequest->dwGroupID, pJoinRequest->bTeamJoin, pJoinRequest->dwJoinValue
    );

    DoJoinBattleFieldQueueRespond(nConnIndex, pJoinRequest->dwRoleID, pJoinRequest->dwMapID, nRetCode);
}

void KGameServer::OnLeaveBattleFieldQueueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                    bRetCode        = false;
    S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*   pLeaveRequest   = (S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.LeaveQueue(
        pLeaveRequest->dwRoleID, pLeaveRequest->dwMapID
    );

    DoLeaveBattleFieldQueueRespond(nConnIndex, pLeaveRequest->dwRoleID, pLeaveRequest->dwMapID, bRetCode);
}

void KGameServer::OnAcceptJoinBattleField(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode          = false;
    S2R_ACCEPT_JOIN_BATTLE_FIELD*   pAcceptJoinBattle = (S2R_ACCEPT_JOIN_BATTLE_FIELD*)pbyData;
    KMapCopy*                       pMapCopy          = NULL;

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.IsBattleField(pAcceptJoinBattle->dwMapID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.CheckPermission(
        pAcceptJoinBattle->dwRoleID, pAcceptJoinBattle->dwMapID, pAcceptJoinBattle->nCopyIndex
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(pAcceptJoinBattle->dwMapID, pAcceptJoinBattle->nCopyIndex);
    KG_PROCESS_ERROR(pMapCopy);

    KG_PROCESS_ERROR(pMapCopy->m_eState == eMapStateNormal);

    bRetCode = DoAcceptJoinBattleFieldRespond(
        nConnIndex, pAcceptJoinBattle->dwRoleID, pAcceptJoinBattle->dwMapID, pAcceptJoinBattle->nCopyIndex
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnGetBattleFieldList(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode            = false;
    S2R_GET_BATTLE_FIELD_LIST*  pGetBattleFieldList = (S2R_GET_BATTLE_FIELD_LIST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.GetBattleFieldList(
        pGetBattleFieldList->dwPlayerID, pGetBattleFieldList->dwMapID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnLeaveBattleFieldRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode            = false;
    S2R_LEAVE_BATTLE_FIELD_REQUEST* pLeaveBattleField   = (S2R_LEAVE_BATTLE_FIELD_REQUEST*)pbyData;

    bRetCode = DoLeaveBattleFieldRespond(nConnIndex, pLeaveBattleField->dwPlayerID);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnApplyCreatePQRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode                = false;
    S2R_APPLY_CREATE_PQ_REQUEST*    pApplyCreatePQRequest   = (S2R_APPLY_CREATE_PQ_REQUEST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_PQCenter.CreatePQ(pApplyCreatePQRequest->dwPQTemplateID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnApplyDeletePQRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode                = false;
    S2R_APPLY_DELETE_PQ_REQUEST*    pApplyDeletePQRequest   = (S2R_APPLY_DELETE_PQ_REQUEST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_PQCenter.DeletePQ(pApplyDeletePQRequest->dwPQID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnApplyChangePQValueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode                    = false;
    S2R_APPLY_CHANGE_PQ_VALUE_REQUEST*  pApplyChangePQValueRequest  = (S2R_APPLY_CHANGE_PQ_VALUE_REQUEST*)pbyData;
    KGPQ*                               pPQ                         = NULL;

    pPQ = g_pSO3GameCenter->m_PQCenter.GetPQ(pApplyChangePQValueRequest->dwPQID);
    KGLOG_PROCESS_ERROR(pPQ);

    bRetCode = pPQ->AddValue(pApplyChangePQValueRequest->nIndex, pApplyChangePQValueRequest->nAddValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnAddCampScore(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_ADD_CAMP_SCORE* pAddCampScore = (S2R_ADD_CAMP_SCORE*)pbyData;
    
    g_pSO3GameCenter->m_CampManager.AddScore(pAddCampScore->nAddCampScore);

    g_pSO3GameCenter->m_CampManager.m_bUpdate = true;

    DoSyncCampInfo(0);
}

void KGameServer::OnApplySetCampRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode                = false;
    KRole*                      pRole                   = NULL;
    KTong*                      pTong                   = NULL;
    S2R_APPLY_SET_CAMP_REQUEST* pApplySetCampRequest    = (S2R_APPLY_SET_CAMP_REQUEST*)pbyData;
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pApplySetCampRequest->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    KGLOG_PROCESS_ERROR(pApplySetCampRequest->byNewCamp >= cNeutral && pApplySetCampRequest->byNewCamp <= cEvil);
    
    if (pRole->m_dwTongID != ERROR_ID)
    {
        pTong = g_pSO3GameCenter->m_TongManager.GetTong(pRole->m_dwTongID);
        KGLOG_PROCESS_ERROR(pTong);

        bRetCode = pTong->CheckCampCompatible((KCAMP)pApplySetCampRequest->byNewCamp);
        
        if (!bRetCode)
        {
            DoApplySetCampRespond(pRole, crtTongConflict);
            goto Exit0;
        } 
    }

    if (pRole->m_dwTeamID != INVALID_PARTY_ID && pApplySetCampRequest->byNewCamp != cNeutral)
    {
        KTeam* pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pRole->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        if (pTeam->eCamp == cGood && (KCAMP)pApplySetCampRequest->byNewCamp != cGood)
        {
            DoApplySetCampRespond(pRole, crtPartyIsGood);
            goto Exit0;
        }
       
        if (pTeam->eCamp == cEvil && (KCAMP)pApplySetCampRequest->byNewCamp != cEvil)
        {
            DoApplySetCampRespond(pRole, crtPartyIsEvil);
            goto Exit0;
        }
    }

    pRole->m_eCamp = (KCAMP)pApplySetCampRequest->byNewCamp;

    DoApplySetCampRespond(pRole, crtSuccess);

Exit0:
    return;
}

void KGameServer::OnApplyTongRosterRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_APPLY_TONG_ROSTER_REQUEST*  pPackage = (S2R_APPLY_TONG_ROSTER_REQUEST*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_APPLY_TONG_ROSTER_REQUEST) + sizeof(DWORD) * pPackage->nMemberCount);

    g_pSO3GameCenter->m_TongManager.ProcessApplyRosterRequest(
        pPackage->dwPlayerID, 
        pPackage->nLastUpdateFrame, 
        pPackage->dwMemberList,
        pPackage->nMemberCount
    );

Exit0:
    return;
}

void KGameServer::OnApplyTongInfoRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_APPLY_TONG_INFO_REQUEST* pRequest    = (S2R_APPLY_TONG_INFO_REQUEST*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessApplyTongInfoRequest(pRequest->dwPlayerID, pRequest->nLastUpdateFrame);
}

void KGameServer::OnApplyCreateTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode        = false;
    S2R_APPLY_CREATE_TONG_REQUEST*  pApplyRequest   = (S2R_APPLY_CREATE_TONG_REQUEST*)pbyData;

    pApplyRequest->szTongName[sizeof(pApplyRequest->szTongName) - 1] = '\0';

    bRetCode = g_pSO3GameCenter->m_TongManager.CreateTong(
        pApplyRequest->dwPlayerID, pApplyRequest->szTongName, (int)pApplyRequest->byTemplateIndex
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnInvitePlayerJoinTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_INVITE_PLAYER_JOIN_TONG_REQUEST*    pInviteRequest  = (S2R_INVITE_PLAYER_JOIN_TONG_REQUEST*)pbyData;

    pInviteRequest->szNewMemberName[sizeof(pInviteRequest->szNewMemberName) - 1] = '\0';

    g_pSO3GameCenter->m_TongManager.ProcessInvitePlayerJoinTongRequest(
        pInviteRequest->dwPlayerID, pInviteRequest->szNewMemberName
    );
}

void KGameServer::OnInvitePlayerJoinTongRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_INVITE_PLAYER_JOIN_TONG_RESPOND*    pRespond   = (S2R_INVITE_PLAYER_JOIN_TONG_RESPOND*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessInvitePlayerJoinTongRespond(
        pRespond->dwPlayerID, pRespond->dwInviterID, pRespond->dwTongID, (BOOL)pRespond->byAccept
    );
}

void KGameServer::OnApplyKickOutTongMemberRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_APPLY_KICK_OUT_TONG_MEMBER* pRequest = (S2R_APPLY_KICK_OUT_TONG_MEMBER*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessKickOutTongMemberRequest(pRequest->dwPlayerID, pRequest->dwMemberID);
}

void KGameServer::OnModifyTongInfoRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_MODIFY_TONG_INFO_REQUEST*   pRequest = (S2R_MODIFY_TONG_INFO_REQUEST*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_MODIFY_TONG_INFO_REQUEST) + pRequest->wDataLen);

    g_pSO3GameCenter->m_TongManager.ProcessModifyTongInfoRequest(
        pRequest->dwPlayerID, (int)pRequest->byInfoType, pRequest->byData, (size_t)pRequest->wDataLen
    );

Exit0:
    return;
}

void KGameServer::OnModifyTongScheamRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_MODIFY_TONG_SCHEMA_REQUEST* pRequest = (S2R_MODIFY_TONG_SCHEMA_REQUEST*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessModifyTongScheamRequest(
        pRequest->dwPlayerID, pRequest->byGroupIndex, pRequest->byModifyType, 
        pRequest->byData, uDataLen - sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST)
    );
}

void KGameServer::OnApplyQuiteTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{ 
    S2R_APPLY_QUITE_TONG_REQUEST*   pRequest = (S2R_APPLY_QUITE_TONG_REQUEST*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessQuitRequest(pRequest->dwPlayerID);
}

void KGameServer::OnChangeTongMemberGroupRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST*   pRequest = (S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessChangeMemberGroupRequest(
        pRequest->dwPlayerID, pRequest->dwTargetMemberID, (int)pRequest->byTargetGroup
    );
}

void KGameServer::OnChangeTongMasterRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_CHANGE_TONG_MASTER_REQUEST* pRequest = (S2R_CHANGE_TONG_MASTER_REQUEST*)pbyData;

    g_pSO3GameCenter->m_TongManager.ProcessChangeMasterRequest(pRequest->dwPlayerID, pRequest->dwNewMasterID);
}

void KGameServer::OnChangeTongMemberRemarkRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST*  pRequest = (S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST*)pbyData;

    pRequest->szRemark[sizeof(pRequest->szRemark) - 1] = '\0';

    g_pSO3GameCenter->m_TongManager.ProcessChangeTongMemberRemarkRequest(
        pRequest->dwPlayerID, pRequest->dwTargetMemberID, pRequest->szRemark
    );
}

void KGameServer::OnGetTongDescriptionRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_GET_TONG_DESCRIPTION_REQUEST* pRequest = (S2R_GET_TONG_DESCRIPTION_REQUEST*)pbyData;
    KTong* pTong = NULL;
    
    pTong = g_pSO3GameCenter->m_TongManager.GetTong(pRequest->dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    DoGetTongDescriptionRepond(nConnIndex, pRequest->dwPlayerID, pTong);

Exit0:
    return;
}

void KGameServer::OnChangeTongCampRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                            bRetCode    = false;
    KRole*                          pRole       = NULL;
    KTong*                          pTong       = NULL;
    S2R_CHANGE_TONG_CAMP_REQUEST*   pRequest    = (S2R_CHANGE_TONG_CAMP_REQUEST*)pbyData;


    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRequest->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    pTong = g_pSO3GameCenter->m_TongManager.GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    if (pTong->m_dwMaster != pRequest->dwPlayerID)
    {
        // 不是帮主，没权限
        DoTongEventNotify(nConnIndex, pRequest->dwPlayerID, tenChangeCampPermissionError);
        goto Exit0;
    }

    KGLOG_PROCESS_ERROR(pRequest->byCamp == cGood || pRequest->byCamp == cEvil);
    bRetCode = pTong->ChangeCamp((KCAMP)pRequest->byCamp);

    if (!bRetCode)
    {
        // 阵营转换限制
        DoTongEventNotify(nConnIndex, pRequest->dwPlayerID, tenChangeCampLimitError);
        goto Exit0;
    }

    //  广播阵营变更
    DoTongBroadcastMessage(pTong, tbmCampChange, ERROR_ID, &pRequest->byCamp, sizeof(BYTE));

Exit0:
    return;
}

void KGameServer::OnAuctionLookupRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    S2R_AUCTION_LOOKUP_REQUEST* pRequest    = (S2R_AUCTION_LOOKUP_REQUEST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_AuctionManager.PostRequestLookup(
        pRequest->byRequestID, pRequest->dwPlayerID, &pRequest->Param
    );
    KG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnAuctionBidRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode = false;
    S2R_AUCTION_BID_REQUEST*    pRequest = (S2R_AUCTION_BID_REQUEST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_AuctionManager.PostRequestBid(
        pRequest->dwBidderID, pRequest->dwSaleID, pRequest->dwCRC, pRequest->nPrice
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnAuctionSellRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode    = false;
    S2R_AUCTION_SELL_REQUEST*   pRequest    = (S2R_AUCTION_SELL_REQUEST*)pbyData;
    
    KGLOG_PROCESS_ERROR(sizeof(S2R_AUCTION_SELL_REQUEST) + pRequest->uItemDataLen == uDataLen);

    pRequest->szSaleName[sizeof(pRequest->szSaleName) - 1] = '\0';

    bRetCode = g_pSO3GameCenter->m_AuctionManager.PostRequestSell(
        pRequest->dwSellerID, pRequest->szSaleName, pRequest->nAucGenre, 
        pRequest->nAucSub, pRequest->nRequireLevel, pRequest->nQuality,
        pRequest->nStartPrice, pRequest->nBuyItNowPrice, pRequest->nCustodyCharges, 
        g_pSO3GameCenter->m_nTimeNow + pRequest->nLeftTime,
        pRequest->byItemData, pRequest->uItemDataLen
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnAuctionCancelRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                        bRetCode = false;
    S2R_AUCTION_CANCEL_REQUEST* pRequest = (S2R_AUCTION_CANCEL_REQUEST*)pbyData;

    bRetCode = g_pSO3GameCenter->m_AuctionManager.PostRequestCancel(pRequest->dwPlayerID, pRequest->dwSaleID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KGameServer::OnRemoteLuaCall(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    S2R_REMOTE_LUA_CALL* pLuaCall = (S2R_REMOTE_LUA_CALL*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(S2R_REMOTE_LUA_CALL) + pLuaCall->uParamLen);

    pLuaCall->szFunction[sizeof(pLuaCall->szFunction) - 1] = '\0';

    g_pSO3GameCenter->m_RemoteScript.Accept(
        nConnIndex, pLuaCall->szFunction, pLuaCall->byParam, pLuaCall->uParamLen
    );

Exit0:
    return;
}

void KGameServer::OnAchievementGlobalAnnounceRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex)
{
    BOOL                                bRetCode = false;
    S2R_ACHIEVEMENT_ANNOUNCE_REQUEST*   pRequest = (S2R_ACHIEVEMENT_ANNOUNCE_REQUEST*)pbyData;
    
    bRetCode = DoAchievementAnnounceRespond(pRequest->dwPlayerID, pRequest->dwAchievementID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}
//AutoCode:-处理协议函数结束-

BOOL KGameServer::DoHandshakeRespond(int nConnIndex)
{
    BOOL                    bResult         = false;
	int                     nRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
    R2S_HANDSHAKE_RESPOND*  pRespond        = NULL;
    KGS_INFO*               pGSInfo         = NULL;

    pGSInfo = GetGSInfoByConnectionIndex(nConnIndex);
    KGLOG_PROCESS_ERROR(pGSInfo);
    
    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_HANDSHAKE_RESPOND));
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_HANDSHAKE_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	pRespond->wProtocolID        = r2s_handshake_respond;
    pRespond->nBaseTime          = g_pSO3GameCenter->m_nBaseTime;
	pRespond->nWorldIndex        = pGSInfo->nWorldIndex;
    pRespond->nStartFrame        = g_pSO3GameCenter->m_nWorkLoop;

	nRetCode = Send(nConnIndex, piPackage);
	KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoPlayerLoginRequest(
    int nConnIndex, const KACCOUNT_LOGIN_INFO& crAccInfo, DWORD dwRoleID, const KRoleBaseInfo& crBaseInfo
)
{
    BOOL                        bResult         = false;
	BOOL                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
    R2S_PLAYER_LOGIN_REQUEST*   pLoginRequest   = NULL;     
    KRole*                      pRole           = NULL;
    int                         nBattleSide     = INVALID_BATTLE_SIDE;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_PLAYER_LOGIN_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

    nBattleSide = g_pSO3GameCenter->m_BattleFieldManager.GetBattleFieldSide(
        pRole->m_dwDestMapID, pRole->m_nDestMapCopyIndex, pRole->m_dwPlayerID
    );

	pLoginRequest = (R2S_PLAYER_LOGIN_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pLoginRequest);

	pLoginRequest->wProtocolID          = r2s_player_login_request;
    pLoginRequest->dwRoleID             = pRole->m_dwPlayerID;

    strncpy(pLoginRequest->szRoleName, pRole->m_szName, sizeof(pLoginRequest->szRoleName));
    pLoginRequest->szRoleName[sizeof(pLoginRequest->szRoleName) - 1] = '\0';

    strncpy(pLoginRequest->szAccount, pRole->m_szAccount, sizeof(pLoginRequest->szAccount));
    pLoginRequest->szAccount[sizeof(pLoginRequest->szAccount) - 1] = '\0';

	pLoginRequest->dwPacketIdentity     = crAccInfo.ulIdentity;
	pLoginRequest->ExtPointInfo         = crAccInfo.ExtPointInfo;
	pLoginRequest->nlLeftTime           = crAccInfo.ulLeftTime;
    pLoginRequest->dwPartyID            = pRole->m_dwTeamID;
    pLoginRequest->BaseInfo             = crBaseInfo;
    pLoginRequest->nBattleSide          = nBattleSide;
    pLoginRequest->dwTongID             = pRole->m_dwTongID;

	bRetCode = Send(nConnIndex, piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoConfirmPlayerLoginRespond(int nConnIndex, DWORD dwPlayerID, BOOL bPermit)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
	IKG_Buffer*                         piPackage   = NULL;    
	R2S_CONFIRM_PLAYER_LOGIN_RESPOND*   pRespond    = NULL;
    
    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_CONFIRM_PLAYER_LOGIN_RESPOND));
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_CONFIRM_PLAYER_LOGIN_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	pRespond->wProtocolID       = r2s_confirm_player_login_respond;
	pRespond->dwPlayerID        = dwPlayerID;
	pRespond->bPermit           = bPermit;

	bRetCode = Send(nConnIndex, piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoKickAccountNotify(DWORD dwPlayerID)
{
    BOOL                        bResult     = false;
	int                         nRetCode    = false;
	IKG_Buffer*                 piPackage   = NULL;
    R2S_KICK_ACCOUNT_NOTIFY*    pNotify     = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_KICK_ACCOUNT_NOTIFY));
	KG_PROCESS_ERROR(piPackage);

	pNotify = (R2S_KICK_ACCOUNT_NOTIFY*)piPackage->GetData();
	KG_PROCESS_ERROR(pNotify);
	
	pNotify->wProtocolID        = r2s_kick_account_notify;
	pNotify->dwPlayerID         = dwPlayerID;

    Broadcast(piPackage);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoCreateMapNotify(int nConnIndex, DWORD dwMapID, int nMapCopyIndex)
{
    BOOL                    bResult         = false;
	int                     nRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
    R2S_CREATE_MAP_NOTIFY*  pCreateMap      = NULL;
    KMapInfo*               pMapInfo        = NULL;
    KMapCopy*               pMapCopy        = NULL;
    size_t                  uExtDataLen     = 0;
    BYTE*                   pbyExtData      = NULL;

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    pMapCopy = pMapInfo->GetMapCopy(nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    if (pMapCopy->m_piSceneData)
    {
        pbyExtData  = (BYTE*)pMapCopy->m_piSceneData->GetData();
        KGLOG_PROCESS_ERROR(pbyExtData);
        uExtDataLen = pMapCopy->m_piSceneData->GetSize();
    }
    
    piPackage = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_CREATE_MAP_NOTIFY) + uExtDataLen));
	KGLOG_PROCESS_ERROR(piPackage);

	pCreateMap = (R2S_CREATE_MAP_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pCreateMap);

	pCreateMap->wProtocolID         = r2s_create_map_notify;
	pCreateMap->dwMapID             = dwMapID;
	pCreateMap->nMapCopyIndex       = nMapCopyIndex;
    pCreateMap->uDataLen            = uExtDataLen;

    if (uExtDataLen > 0)
    {
        memcpy(pCreateMap->byData, pbyExtData, uExtDataLen);
    }

	nRetCode = Send(nConnIndex, piPackage);
	KGLOG_PROCESS_ERROR(nRetCode);

    KGLogPrintf(
        KGLOG_INFO, "Append map %s(%.2u, %d) to GS %d\n", 
        pMapInfo->m_szName, dwMapID, nMapCopyIndex, nConnIndex
    );

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoFinishCreateMapNotify(int nConnIndex, DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piPackage   = NULL;
    R2S_FINISH_CREATE_MAP_NOTIFY*   pPackage    = NULL;
    
    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_FINISH_CREATE_MAP_NOTIFY));
    KGLOG_PROCESS_ERROR(piPackage);

    pPackage = (R2S_FINISH_CREATE_MAP_NOTIFY*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pPackage);

    pPackage->wProtocolID   = r2s_finish_create_map_notify;
    pPackage->dwPlayerID    = dwPlayerID;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoDeleteMapNotify(int nConnIndex, DWORD dwMapID, int nMapCopyIndex)
{
    BOOL                    bResult     = false;
	int                     nRetCode    = false;
    KMapInfo*               pMapInfo    = NULL;
	IKG_Buffer*             piPackage   = NULL;
    R2S_DELETE_MAP_NOTIFY*  pDeleteMap  = NULL;

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    KGLogPrintf(KGLOG_INFO, "Delete map copy %s(%u, %d)\n", pMapInfo->m_szName, dwMapID, nMapCopyIndex);

	piPackage = KG_MemoryCreateBuffer(sizeof(R2S_DELETE_MAP_NOTIFY));
	KG_PROCESS_ERROR(piPackage);

	pDeleteMap = (R2S_DELETE_MAP_NOTIFY*)piPackage->GetData();
	KG_PROCESS_ERROR(pDeleteMap);

	pDeleteMap->wProtocolID     = r2s_delete_map_notify;
	pDeleteMap->dwMapID         = dwMapID;
	pDeleteMap->nMapCopyIndex   = nMapCopyIndex;

	nRetCode = Send(nConnIndex, piPackage);
	KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoSearchMapRespond(
    int nConnIndex, DWORD dwPlayerID, SWITCH_MAP_RESULT_CODE eResult, BOOL bSwitchServer,
    DWORD dwDestMapID, int nDestMapCopyIndex,
    int nX, int nY, int nZ
)
{
    BOOL                            bResult             = false;
	BOOL                            bRetCode            = false;
	IKG_Buffer*                     piPackage           = NULL;
    R2S_SEARCH_MAP_RESPOND*         pRespondPak         = NULL;
    int                             nBattleFieldSide    = INVALID_BATTLE_SIDE;

	piPackage = KG_MemoryCreateBuffer(sizeof(R2S_SEARCH_MAP_RESPOND));
	KGLOG_PROCESS_ERROR(piPackage);

	pRespondPak = (R2S_SEARCH_MAP_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespondPak);

    nBattleFieldSide = g_pSO3GameCenter->m_BattleFieldManager.GetBattleFieldSide(
        dwDestMapID, nDestMapCopyIndex, dwPlayerID
    );

	pRespondPak->wProtocolID        = r2s_search_map_respond;
	pRespondPak->dwPlayerID         = dwPlayerID;
	pRespondPak->nResultCode        = eResult;
	pRespondPak->bSwitchServer      = bSwitchServer;
	pRespondPak->dwMapID            = dwDestMapID;
	pRespondPak->nMapCopyIndex      = nDestMapCopyIndex;
    pRespondPak->nPosX              = nX;
    pRespondPak->nPosY              = nY;
    pRespondPak->nPosZ              = nZ;
    pRespondPak->nBattleFieldSide   = nBattleFieldSide;

	bRetCode = Send(nConnIndex, piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoTransferPlayerRespond(
    int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, 
    DWORD dwAddress, int nPort, const GUID* pcGuid
)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piPackage   = NULL;
    R2S_TRANSFER_PLAYER_RESPOND*    pPackage    = NULL;
    const GUID                      cGuid       = {0};

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_TRANSFER_PLAYER_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pPackage = (R2S_TRANSFER_PLAYER_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pPackage);

    pPackage->wProtocolID        = r2s_transfer_player_respond;
    pPackage->dwPlayerID         = dwPlayerID;
    pPackage->bSucceed           = bSucceed;
    pPackage->dwAddress          = dwAddress;
    pPackage->wPort              = (WORD)nPort;
    pPackage->Guid               = pcGuid ? *pcGuid : cGuid;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
	return bResult;
}

BOOL KGameServer::DoSyncTeamInfo(int nConnectionIndex, DWORD dwPartyID, KTeam* pTeam)
{
    BOOL                          bResult               = FALSE;
    BOOL                          bRetCode              = FALSE;
    IKG_Buffer*                   piPartyCreateNotify   = NULL;
    R2S_TEAM_CREATE_NOTIFY*       pPartyCreateNotify    = NULL;
    IKG_Buffer*                   piAddMemberNotify     = NULL;
    R2S_TEAM_ADD_MEMBER_NOTIFY*   pAddMemberNotify      = NULL;

    // 同步队伍数据，其实就是将所有的队伍数据用"Party add member"的方式重新逐个发送一编

    assert(pTeam);

    piPartyCreateNotify = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_CREATE_NOTIFY));
    KGLOG_PROCESS_ERROR(piPartyCreateNotify);
    pPartyCreateNotify = (R2S_TEAM_CREATE_NOTIFY*)piPartyCreateNotify->GetData();

    pPartyCreateNotify->wProtocolID         = r2s_team_create_notify;
    pPartyCreateNotify->dwPartyID           = dwPartyID;

    bRetCode = Send(nConnectionIndex, piPartyCreateNotify);
    KGLOG_PROCESS_ERROR(bRetCode);
    KG_COM_RELEASE(piPartyCreateNotify);

    piAddMemberNotify = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_ADD_MEMBER_NOTIFY));
    KGLOG_PROCESS_ERROR(piAddMemberNotify);
    pAddMemberNotify = (R2S_TEAM_ADD_MEMBER_NOTIFY*)piAddMemberNotify->GetData();
    KGLOG_PROCESS_ERROR(pAddMemberNotify);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            DWORD  dwPlayerID  = it->dwMemberID;
            KRole* pTeamMember = NULL;

            pTeamMember = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
            KGLOG_PROCESS_ERROR(pTeamMember);

            pAddMemberNotify->wProtocolID   = r2s_team_add_member_notify;
            pAddMemberNotify->dwPartyID     = dwPartyID;
            pAddMemberNotify->nGroupIndex   = i;
            pAddMemberNotify->dwMemberID    = dwPlayerID;

            strncpy(pAddMemberNotify->szMemberName, pTeamMember->m_szName, sizeof(pAddMemberNotify->szMemberName));
            pAddMemberNotify->szMemberName[sizeof(pAddMemberNotify->szMemberName) - 1] = '\0';

            bRetCode = Send(nConnectionIndex, piAddMemberNotify);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    bResult = TRUE;
Exit0:
    KG_COM_RELEASE(piPartyCreateNotify);
    KG_COM_RELEASE(piAddMemberNotify);
    return bResult;
}

BOOL KGameServer::DoApplyJoinTeamRequest(KRole* pApplyDst, KRole* pApplySrc)
{
    BOOL                            bResult         = false;
    int                             nRetCode        = false;
    BOOL                            bIsNotFull      = false;
    KTeam*                          pTeam           = NULL;
    KRole*                          pTeamLeader     = NULL;
    IKG_Buffer*                     piSendBuffer    = NULL;
    R2S_APPLY_JOIN_TEAM_REQUEST*    pApplyToDst     = NULL;
    
    assert(pApplyDst);
    assert(pApplySrc);

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pApplyDst->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    // 得到该队伍的队长
    pTeamLeader = g_pSO3GameCenter->m_RoleManager.GetRole(pTeam->dwAuthority[tatLeader]);
    KGLOG_PROCESS_ERROR(pTeamLeader);

    // 检查申请折的阵营是否和队伍阵营一致
    if (pTeam->eCamp != cNeutral && pApplySrc->m_eCamp && pTeam->eCamp != pApplySrc->m_eCamp)
    {
        DoPartyMessageNotify(
            pApplySrc->m_nConnIndex, pApplySrc->m_dwPlayerID, 
            epncCampError, pTeamLeader->m_szName
        );
        goto Exit0;
    }

    // 检查队伍是否满员
    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        if (pTeam->MemberGroup[i].MemberList.size() < MAX_PLAYER_PER_GROUP)
        {
            bIsNotFull = true;
            break;
        }
    }

    if (!bIsNotFull)
    {
        DoPartyMessageNotify(
            pApplySrc->m_nConnIndex, pApplySrc->m_dwPlayerID, 
            epncDestPartyIsFull, pApplyDst->m_szName
        );
        goto Exit0;
    }

    // 队长必须在线
    KGLOG_PROCESS_ERROR(pTeamLeader->IsOnline());

    g_pSO3GameCenter->m_TeamCenter.RegisterApply(pApplySrc->m_dwPlayerID, pTeamLeader->m_dwPlayerID);

    // 转发入队申请到目标玩家
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_APPLY_JOIN_TEAM_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pApplyToDst = (R2S_APPLY_JOIN_TEAM_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pApplyToDst);

    pApplyToDst->wProtocolID        = r2s_apply_join_team_request;
    pApplyToDst->dwApplyDst         = pTeamLeader->m_dwPlayerID;

    strncpy(pApplyToDst->szApplySrc, pApplySrc->m_szName, sizeof(pApplyToDst->szApplySrc));
    pApplyToDst->szApplySrc[sizeof(pApplyToDst->szApplySrc) - 1] = '\0';

    nRetCode = Send(pTeamLeader->m_nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamCreateNotify(DWORD dwTeamID)
{
    BOOL                    bResult         = FALSE;
    BOOL                    bRetCode        = FALSE;
    KRole*                  pLeader         = NULL;
    R2S_TEAM_CREATE_NOTIFY* pNotify         = NULL;
    IKG_Buffer*             piSendBuffer    = NULL;
    KTeam*                  pTeam           = NULL;

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_CREATE_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pNotify = (R2S_TEAM_CREATE_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID    = r2s_team_create_notify;
    pNotify->dwPartyID      = dwTeamID;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamChangeAuthorityNotify(DWORD dwPartyID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
{
    BOOL                                bResult                     = FALSE;
    BOOL                                bRetCode                    = FALSE;
    IKG_Buffer*                         piSendBuffer                = NULL;
    R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*   pPartyChangeLeaderNotify    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_CHANGE_AUTHORITY_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPartyChangeLeaderNotify = (R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPartyChangeLeaderNotify);

    pPartyChangeLeaderNotify->wProtocolID   = r2s_team_change_authority_notify;
    pPartyChangeLeaderNotify->dwPartyID     = dwPartyID;
    pPartyChangeLeaderNotify->byType        = (BYTE)eType;
    pPartyChangeLeaderNotify->dwNewID       = dwTargetID;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamSetLootModeNotify(DWORD dwPartyID, int nLootMode, int nRollQuality)
{
    BOOL                            bResult         = FALSE;
    BOOL                            bRetCode        = FALSE;
    IKG_Buffer*                     piSendBuffer    = NULL;
    R2S_TEAM_SET_LOOT_MODE_NOTIFY*  pNotify         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_SET_LOOT_MODE_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pNotify = (R2S_TEAM_SET_LOOT_MODE_NOTIFY *)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID      = r2s_team_set_loot_mode_notify;
    pNotify->dwPartyID        = dwPartyID;
    pNotify->nLootMode		  = nLootMode;
	pNotify->nRollQuality	  = nRollQuality;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamAddMemberNotify(DWORD dwTeamID, int nGroupIndex, DWORD dwMemberID)
{
    BOOL                        bResult             = false;
    IKG_Buffer*                 piSendBuffer        = NULL;
    R2S_TEAM_ADD_MEMBER_NOTIFY* pAddMemberNotify    = NULL;
    KRole*                      pRole               = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_TEAM_ADD_MEMBER_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pAddMemberNotify = (R2S_TEAM_ADD_MEMBER_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pAddMemberNotify);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwMemberID);
    KGLOG_PROCESS_ERROR(pRole);

    pAddMemberNotify->wProtocolID   = r2s_team_add_member_notify;
    pAddMemberNotify->dwPartyID     = dwTeamID;
    pAddMemberNotify->nGroupIndex   = nGroupIndex;
    pAddMemberNotify->dwMemberID    = dwMemberID;

    strncpy(pAddMemberNotify->szMemberName, pRole->m_szName, sizeof(pAddMemberNotify->szMemberName));
    pAddMemberNotify->szMemberName[sizeof(pAddMemberNotify->szMemberName) - 1] = '\0';

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamDelMemberNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwMemberID)
{
    BOOL                            bResult             = FALSE;
    BOOL                            bRetCode            = FALSE;
    IKG_Buffer*                     piSendBuffer        = NULL;
    R2S_TEAM_DEL_MEMBER_NOTIFY*     pDelMemberNotify    = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_DEL_MEMBER_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDelMemberNotify = (R2S_TEAM_DEL_MEMBER_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pDelMemberNotify);

    pDelMemberNotify->wProtocolID       = r2s_team_del_member_notify;
    pDelMemberNotify->dwPartyID         = dwPartyID;
    pDelMemberNotify->nGroupIndex       = nGroupIndex;
    pDelMemberNotify->dwMemberID        = dwMemberID;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamDisbandNotify(DWORD dwPartyID)
{
    BOOL                            bResult             = FALSE;
    BOOL                            bRetCode            = FALSE;
    IKG_Buffer*                     piSendBuffer        = NULL;
    R2S_TEAM_DISBAND_NOTIFY*        pPartyDisbandNotify = NULL;

    // 队伍解散通知
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_DISBAND_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPartyDisbandNotify = (R2S_TEAM_DISBAND_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPartyDisbandNotify);

    pPartyDisbandNotify->wProtocolID       = r2s_team_disband_notify;
    pPartyDisbandNotify->dwPartyID         = dwPartyID;

    Broadcast(piSendBuffer);

    bResult = TRUE;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoSyncTeamMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, BOOL bOnlineFlag)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    IKG_Buffer*                         piSendBuffer        = NULL;
    R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*   pSyncOnlineInfo     = NULL;

    // 同步队员在线状态
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSyncOnlineInfo = (R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSyncOnlineInfo);

    pSyncOnlineInfo->wProtocolID       = r2s_sync_team_member_online_flag;
    pSyncOnlineInfo->dwPartyID         = dwPartyID;
    pSyncOnlineInfo->dwMemberID        = dwMemberID;
    pSyncOnlineInfo->bOnlineFlag       = bOnlineFlag;
    
    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoPartyMessageNotify(int nConnIndex, DWORD dwPlayerID, int nCode, const char* pszText)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    size_t                              uTextLen            = 0;
    IKG_Buffer*                         piSendBuffer        = NULL;
    R2S_PARTY_MESSAGE_NOTIFY*           pMsg                = NULL;

    if (pszText)
    {
        uTextLen = strlen(pszText) + 1;
    }

    // 同步队员在线状态
    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_PARTY_MESSAGE_NOTIFY) + uTextLen));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pMsg = (R2S_PARTY_MESSAGE_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pMsg);

    pMsg->wProtocolID = r2s_party_message_notify;
    pMsg->dwPlayerID  = dwPlayerID;
    pMsg->nCode       = nCode;

    if (pszText)
    {
        memcpy(pMsg->byData, pszText, uTextLen + 1);
    }

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamSetFormationLeaderNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwNewFormationLeader)
{
    BOOL                                    bResult             = false;
    BOOL                                    bRetCode            = false;
    IKG_Buffer*                             piSendBuffer        = NULL;
    R2S_TEAM_SET_FORMATION_LEADER_NOTIFY*   pNotify             = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_TEAM_SET_FORMATION_LEADER_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pNotify = (R2S_TEAM_SET_FORMATION_LEADER_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID            = r2s_team_set_formation_leader_notify;
    pNotify->dwPartyID              = dwPartyID;
    pNotify->nGroupIndex            = nGroupIndex;
    pNotify->dwNewFormationLeader   = dwNewFormationLeader;
    
    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamSetMarkRespond(DWORD dwPartyID, int nMarkType, DWORD dwTargetID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_TEAM_SET_MARK_RESPOND*  pNotify         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(R2S_TEAM_SET_MARK_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pNotify = (R2S_TEAM_SET_MARK_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID    = r2s_team_set_mark_respond;
    pNotify->dwPartyID      = dwPartyID;
    pNotify->nMarkType      = nMarkType;
    pNotify->dwTarget       = dwTargetID;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoTeamCampChange(DWORD dwPartyID, KCAMP eCamp)
{
    BOOL                    bResult         = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    R2S_TEAM_CAMP_CHANGE*   pTeamCampChange;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_TEAM_CAMP_CHANGE));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pTeamCampChange = (R2S_TEAM_CAMP_CHANGE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pTeamCampChange);

    assert(eCamp < UCHAR_MAX);

    pTeamCampChange->wProtocolID = r2s_team_camp_change;
    pTeamCampChange->dwPartyID   = dwPartyID;
    pTeamCampChange->byCamp      = (BYTE)eCamp;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoPlayerTalkError(int nConnectionIndex, DWORD dwReceiverID, int nErrorCode)
{
    BOOL                    bResult             = false;
    BOOL                    bRetCode            = false;
    IKG_Buffer*             piSendBuffer        = NULL;
    R2S_PLAYER_TALK_ERROR*  pPlayerTalkError    = NULL;

    KGLOG_PROCESS_ERROR(dwReceiverID != 0);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_PLAYER_TALK_ERROR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPlayerTalkError = (R2S_PLAYER_TALK_ERROR *)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPlayerTalkError);

    pPlayerTalkError->wProtocolID   = r2s_player_talk_error;
    pPlayerTalkError->dwReceiverID  = dwReceiverID;
    pPlayerTalkError->nErrorCode    = nErrorCode;

    bRetCode = Send(nConnectionIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoPlayerTalkWhisperSuccessRespond(int nConnectionIndex, DWORD dwTalkerID, DWORD dwReceiverID)
{
    BOOL                                        bResult                             = false;
    BOOL                                        bRetCode                            = false;
    IKG_Buffer*                                 piSendBuffer                        = NULL;
    R2S_PLAYER_TALK_WHISPER_SUCCESS_RESPOND*    pPlayerTalkWhisperSuccessRespond    = NULL;

    KGLOG_PROCESS_ERROR(dwTalkerID);
    KGLOG_PROCESS_ERROR(dwReceiverID);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_PLAYER_TALK_WHISPER_SUCCESS_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPlayerTalkWhisperSuccessRespond = (R2S_PLAYER_TALK_WHISPER_SUCCESS_RESPOND *)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPlayerTalkWhisperSuccessRespond);

    pPlayerTalkWhisperSuccessRespond->wProtocolID   = r2s_player_talk_whisper_success_respond;
    pPlayerTalkWhisperSuccessRespond->dwTalkerID    = dwTalkerID;
    pPlayerTalkWhisperSuccessRespond->dwReceiverID  = dwReceiverID;

    bRetCode = Send(nConnectionIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

// --------------------- 好友相关 --------------------------------

BOOL KGameServer::DoApplyFellowshipDataRespond(DWORD dwPlayerID, size_t uDataLength, BYTE byData[])
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    KRole*                              pRole           = NULL;
    IKG_Buffer*                         piSendBuffer    = NULL;
    R2S_APPLY_FELLOWSHIP_DATA_RESPOND*  pPak            = NULL;
    size_t                              uPakSize        = sizeof(R2S_APPLY_FELLOWSHIP_DATA_RESPOND) + uDataLength;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_APPLY_FELLOWSHIP_DATA_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = r2s_apply_fellowship_data_respond;
    pPak->dwPlayerID    = dwPlayerID;
    memcpy(pPak->byData, byData, uDataLength);

    bRetCode = Send(pRole->m_nConnIndex, piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoAddFellowshipRespond(DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char szAlliedPlayerName[], int nType)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    KRole*                          pRole           = NULL;
    IKG_Buffer*                     piSendBuffer    = NULL;
    R2S_ADD_FELLOWSHIP_RESPOND*     pPak            = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_ADD_FELLOWSHIP_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_ADD_FELLOWSHIP_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = r2s_add_fellowship_respond;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwAlliedPlayerID  = dwAlliedPlayerID;
    pPak->byType            = (BYTE)nType;
    strncpy(pPak->szAlliedPlayerName, szAlliedPlayerName, sizeof(pPak->szAlliedPlayerName));
    pPak->szAlliedPlayerName[sizeof(pPak->szAlliedPlayerName) - 1] = '\0';

    bRetCode = Send(pRole->m_nConnIndex, piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoGetFellowshipNameRespond(DWORD dwPlayerID, int nArrayLength, DWORD dwAlliedPlayerIDArray[])
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    KRole*                              pRole           = NULL;
    IKG_Buffer*                         piSendBuffer    = NULL;
    R2S_GET_FELLOWSHIP_NAME_RESPOND*    pPak            = NULL;

    assert(nArrayLength >= 0);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_GET_FELLOWSHIP_NAME_RESPOND) + sizeof(R2S_GET_FELLOWSHIP_NAME_RESPOND::_AlliedPlayer) * nArrayLength);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_GET_FELLOWSHIP_NAME_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    KGLOG_PROCESS_ERROR(nArrayLength <= UCHAR_MAX);

    pPak->wProtocolID   = r2s_get_fellowship_name_respond;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->byCount       = (BYTE)nArrayLength;

    for (int i = 0; i < nArrayLength; i++)
    {
        KRole*  pAlliedRole = NULL;

        pPak->AlliedPlayerInfo[i].dwID = dwAlliedPlayerIDArray[i];
        pPak->AlliedPlayerInfo[i].szName[0] = '\0';

        pAlliedRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwAlliedPlayerIDArray[i]);
        if (!pAlliedRole)
        {
            continue;
        }

        pPak->AlliedPlayerInfo[i].dwMapID = pAlliedRole->m_dwMapID;

        strncpy(pPak->AlliedPlayerInfo[i].szName, pAlliedRole->m_szName, sizeof(pPak->AlliedPlayerInfo[i].szName));
        pPak->AlliedPlayerInfo[i].szName[sizeof(pPak->AlliedPlayerInfo[i].szName) - sizeof('\0')] = '\0';
    }

    bRetCode = Send(pRole->m_nConnIndex, piSendBuffer);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoSyncFellowshipMapID(DWORD dwPlayerID, DWORD dwMapID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_SYNC_FELLOWSHIP_MAPID*  pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_FELLOWSHIP_MAPID));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_SYNC_FELLOWSHIP_MAPID*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = r2s_sync_fellowship_mapid;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->dwMapID       = dwMapID;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoSyncFellowshipPlayerLevel(DWORD dwPlayerID, int nLevel, BOOL bNotifyClient)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL*   pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);
    assert(nLevel >= 0);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = r2s_sync_fellowship_player_level;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->byLevel       = (BYTE)nLevel;
    pPak->bNotifyClient = bNotifyClient ? 1 : 0;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoSyncFellowshipPlayerForceID(DWORD dwPlayerID, DWORD dwForceID)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID* pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID   = r2s_sync_fellowship_player_forceid;
    pPak->dwPlayerID    = dwPlayerID;
    pPak->byForceID     = (BYTE)dwForceID;

    Broadcast(piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoAddFellowshipNotify(int nConnIndex, DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char szAlliedPlayerName[], int nType)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_ADD_FELLOWSHIP_NOTIFY*  pPak            = NULL;

    assert(dwPlayerID != ERROR_ID);
    assert(szAlliedPlayerName);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_ADD_FELLOWSHIP_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_ADD_FELLOWSHIP_NOTIFY*)piSendBuffer->GetData();
    assert(pPak);

    pPak->wProtocolID       = r2s_add_fellowship_notify;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwAlliedPlayerID  = dwAlliedPlayerID;
    pPak->nType             = nType;

    strncpy(pPak->szAlliedPlayerName, szAlliedPlayerName, sizeof(pPak->szAlliedPlayerName));
    pPak->szAlliedPlayerName[sizeof(pPak->szAlliedPlayerName) - 1] = '\0';

    Send(nConnIndex, piSendBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

// --------------------- 邮件相关 --------------------------------
BOOL KGameServer::DoSendMailRespond(int nConnIndex, int nRespondID, DWORD dwSrcID, BYTE byResult, KMAIL_CONTENT* pContent)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    R2S_SEND_MAIL_RESPOND*  pPak            = NULL;
    unsigned                uPakSize        = (unsigned)sizeof(R2S_SEND_MAIL_RESPOND);
    
    KGLOG_PROCESS_ERROR(pContent);

    if (byResult != mrcSucceed)
        uPakSize += pContent->wTextSize + pContent->wItemSize;

    piSendBuffer = KG_MemoryCreateBuffer(uPakSize);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (R2S_SEND_MAIL_RESPOND*)piSendBuffer->GetData();

    pPak->wProtocolID      = r2s_send_mail_respond;
    pPak->byIndex          = (BYTE)nRespondID;
    pPak->dwSrcID          = dwSrcID;
    pPak->byResult         = byResult;

    if (byResult != mrcSucceed)
        memcpy(&(pPak->Content), pContent, sizeof(KMAIL_CONTENT) + pContent->wTextSize + pContent->wItemSize);
    else
        memset(&(pPak->Content), 0, sizeof(KMAIL_CONTENT));

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoGetMailListRespond(DWORD dwPlayerID, int nMailCount, IKG_Buffer** ppiMailBuffer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_GET_MAILLIST_RESPOND*   pRespond        = NULL;
    KRole*                      pRole           = NULL;

    KGLOG_PROCESS_ERROR(ppiMailBuffer);

    piSendBuffer = KG_MemoryCreateBuffer(
        (unsigned)(sizeof(R2S_GET_MAILLIST_RESPOND) + sizeof(KMAIL_LIST_NODE) * nMailCount)
    );
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2S_GET_MAILLIST_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    pRespond->wProtocolID       = r2s_get_maillist_respond;
    pRespond->dwPlayerID        = dwPlayerID;
    pRespond->nMailCount        = nMailCount;

    for (int i = 0; i < nMailCount; i++)
    {
        KMAIL*      pMail       = NULL;
        unsigned    uBuffSize   = 0;

        assert(ppiMailBuffer[i]);
        uBuffSize = ppiMailBuffer[i]->GetSize();
        KGLOG_PROCESS_ERROR(uBuffSize >= sizeof(KMAIL));
        pMail = (KMAIL*)ppiMailBuffer[i]->GetData();

        // Mail ID
        pRespond->MailList[i].dwMailID = pMail->dwMailID;

        // Sender name
        strncpy(
            pRespond->MailList[i].szSenderName, 
            pMail->szSenderName, 
            sizeof(pRespond->MailList[i].szSenderName)
        );
        pRespond->MailList[i].szSenderName[sizeof(pRespond->MailList[i].szSenderName) - 1] = '\0';

        // Title
        strncpy(
            pRespond->MailList[i].szTitle, 
            pMail->szTitle, 
            sizeof(pRespond->MailList[i].szTitle)
        );
        pRespond->MailList[i].szTitle[sizeof(pRespond->MailList[i].szTitle) - 1] = '\0';

        // Receive time
        pRespond->MailList[i].lReceiveTime = pMail->lReceiveTime;

        // Flags
        pRespond->MailList[i].bUnReadFlag  = (pMail->byFlags & KMAIL_BIT_FLAG_UNREAD) == 0 ? 0 : 1;
        pRespond->MailList[i].bMoneyFlag   = (pMail->byFlags & KMAIL_BIT_FLAG_MONEY)  == 0 ? 0 : 1;
        pRespond->MailList[i].bTextFlag    = (pMail->byFlags & KMAIL_BIT_FLAG_TEXT)   == 0 ? 0 : 1;
        pRespond->MailList[i].bItemFlag    = (pMail->byFlags & KMAIL_BIT_FLAG_ITEM)   == 0 ? 0 : 1;
        pRespond->MailList[i].bSystemFlag  = (pMail->byFlags & KMAIL_BIT_FLAG_SYSTEM) == 0 ? 0 : 1;
        pRespond->MailList[i].byReserve    = 0;
    }

    bRetCode = Send(pRole->m_nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

// 新邮件系统(未测试)
BOOL KGameServer::DoGetMailListRespondEx(DWORD dwPlayerID, int nMailCount, KMAIL_LIST_NODE MailList[])
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_GET_MAILLIST_RESPOND*   pRespond        = NULL;
    KRole*                      pRole           = NULL;

    KGLOG_PROCESS_ERROR(nMailCount >= 0);

    piSendBuffer = KG_MemoryCreateBuffer(
        (unsigned)(sizeof(R2S_GET_MAILLIST_RESPOND) + sizeof(KMAIL_LIST_NODE) * nMailCount)
    );
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2S_GET_MAILLIST_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID = r2s_get_maillist_respond;
    pRespond->dwPlayerID  = dwPlayerID;
    pRespond->nMailCount  = nMailCount;

    if (nMailCount > 0)
    {
        KGLOG_PROCESS_ERROR(MailList);
        memcpy(pRespond->MailList, MailList, sizeof(KMAIL_LIST_NODE) * nMailCount);
    }

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    bRetCode = Send(pRole->m_nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoGetMailContentRespond(
    int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byResult, KMAIL_CONTENT* pContent
)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    R2S_GET_MAIL_CONTENT*   pRespond        = NULL;

    if (!pContent)
    {
        piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_GET_MAIL_CONTENT));
    }
    else
    {
        piSendBuffer = KG_MemoryCreateBuffer(
            sizeof(R2S_GET_MAIL_CONTENT) + pContent->wTextSize + pContent->wItemSize
        );
    }
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2S_GET_MAIL_CONTENT*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID       = r2s_get_mail_content;
    pRespond->dwPlayerID        = dwPlayerID;
    pRespond->dwMailID          = dwMailID;
    pRespond->byResult          = byResult;

    pRespond->Content.nMoney    = 0;
    pRespond->Content.wTextSize = 0;
    pRespond->Content.wItemSize = 0;

    if (pContent)
    {
        memcpy(
            &(pRespond->Content), 
            pContent, 
            sizeof(KMAIL_CONTENT) + pContent->wTextSize + pContent->wItemSize
        );
    }

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoGetMailAttachmentRespond(
    int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byResult, 
    int nMoney, size_t uTextSize, char* pszText, size_t uItemDataSize, BYTE* pbyItemData
)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_GET_MAIL_ATTACHMENT*    pRespond        = NULL;

    KGLOG_PROCESS_ERROR(dwPlayerID);
    KGLOG_PROCESS_ERROR(dwMailID);

    if (uTextSize)
    {
        KGLOG_PROCESS_ERROR(pszText);
    }

    if (uItemDataSize)
    {
        KGLOG_PROCESS_ERROR(pbyItemData);
    }

    piSendBuffer = KG_MemoryCreateBuffer(
        (unsigned)(sizeof(R2S_GET_MAIL_ATTACHMENT) + uTextSize + uItemDataSize)
    );
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2S_GET_MAIL_ATTACHMENT*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID       = r2s_get_mail_attachment;
    pRespond->dwPlayerID        = dwPlayerID;
    pRespond->dwMailID          = dwMailID;
    pRespond->byResult          = byResult;

    pRespond->Content.nMoney = nMoney;

    pRespond->Content.wTextSize = (WORD)uTextSize;
    if (uTextSize)
    {
        memcpy(
            pRespond->Content.byTextAndItemData, 
            pszText, 
            pRespond->Content.wTextSize
        );
    }

    pRespond->Content.wItemSize = (WORD)uItemDataSize;
    if (uItemDataSize)
    {
        memcpy(
            pRespond->Content.byTextAndItemData + pRespond->Content.wTextSize, 
            pbyItemData, 
            pRespond->Content.wItemSize
        );
    }

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoNewMailNotity(int nConnIndex, DWORD dwPlayerID, KMAIL* pMail)
{
    BOOL                 bResult        = false;
    BOOL                 bRetCode       = false;
    IKG_Buffer*          piSendBuffer   = NULL;
    R2S_NEW_MAIL_NOTIFY* pNotify        = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_NEW_MAIL_NOTIFY));
    KGLOG_PROCESS_ERROR(piSendBuffer);
    
    pNotify = (R2S_NEW_MAIL_NOTIFY*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID      = r2s_new_mail_notify;
    pNotify->dwPlayerID       = dwPlayerID;

    // MailID
    pNotify->NewMailListNode.dwMailID = pMail->dwMailID;

    // Sender name
    strncpy(
        pNotify->NewMailListNode.szSenderName, 
        pMail->szSenderName, 
        sizeof(pNotify->NewMailListNode.szSenderName)
    );
    pNotify->NewMailListNode.szSenderName[sizeof(pNotify->NewMailListNode.szSenderName) - 1] = '\0';

    // Title
    strncpy(
        pNotify->NewMailListNode.szTitle, 
        pMail->szTitle, 
        sizeof(pNotify->NewMailListNode.szTitle)
    );
    pNotify->NewMailListNode.szTitle[sizeof(pNotify->NewMailListNode.szTitle) - 1] = '\0';

    // Receive Time
    pNotify->NewMailListNode.lReceiveTime   = pMail->lReceiveTime;

    // Flags
    pNotify->NewMailListNode.bUnReadFlag    = (pMail->byFlags & KMAIL_BIT_FLAG_UNREAD) == 0 ? 0 : 1;
    pNotify->NewMailListNode.bMoneyFlag     = (pMail->byFlags & KMAIL_BIT_FLAG_MONEY)  == 0 ? 0 : 1;
    pNotify->NewMailListNode.bTextFlag      = (pMail->byFlags & KMAIL_BIT_FLAG_TEXT)   == 0 ? 0 : 1;
    pNotify->NewMailListNode.bItemFlag      = (pMail->byFlags & KMAIL_BIT_FLAG_ITEM)   == 0 ? 0 : 1;
    pNotify->NewMailListNode.bSystemFlag    = (pMail->byFlags & KMAIL_BIT_FLAG_SYSTEM) == 0 ? 0 : 1;

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoMailGeneralRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byResult)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    R2S_MAIL_GENERAL_RESPOND*   pRespond        = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_MAIL_GENERAL_RESPOND));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2S_MAIL_GENERAL_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID       = r2s_mail_general_respond;
    pRespond->dwPlayerID        = dwPlayerID;
    pRespond->dwMailID          = dwMailID;
    pRespond->byRespond         = byResult;

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoMapCopyDoClearPlayerPrepare(int nConnIndex, DWORD dwMapID, int nMapCopyIndex, int nType, int nDaleyTime)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    IKG_Buffer*                             piSendBuffer    = NULL;
    R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE*   pClearPrepare   = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pClearPrepare = (R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pClearPrepare);

    pClearPrepare->wProtocolID    = r2s_map_copy_do_clear_player_prepare;
    pClearPrepare->dwMapID        = dwMapID;
    pClearPrepare->nMapCopyIndex  = nMapCopyIndex;
    pClearPrepare->nType          = nType;
    pClearPrepare->nDelayTime     = nDaleyTime;

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoMapCopyDoClearPlayer(int nConnIndex, DWORD dwMapID, int nMapCopyIndex)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piSendBuffer    = NULL;
    R2S_MAP_COPY_DO_CLEAR_PLAYER*   pClear          = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_MAP_COPY_DO_CLEAR_PLAYER));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pClear = (R2S_MAP_COPY_DO_CLEAR_PLAYER*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pClear);

    pClear->wProtocolID    = r2s_map_copy_do_clear_player;
    pClear->dwMapID        = dwMapID;
    pClear->nMapCopyIndex  = nMapCopyIndex;

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoSetMapCopyOwner(int nConnIndex, DWORD dwMapID, int nMapCopyIndex, DWORD dwOwnerID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    R2S_SET_MAP_COPY_OWNER* pSetOwner       = NULL;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SET_MAP_COPY_OWNER));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pSetOwner = (R2S_SET_MAP_COPY_OWNER*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSetOwner);

    pSetOwner->wProtocolID      = r2s_set_map_copy_owner;
    pSetOwner->dwMapID          = dwMapID;
    pSetOwner->nMapCopyIndex    = nMapCopyIndex;
    pSetOwner->dwOwnerID        = dwOwnerID;

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);    

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoApplyPlayerSavedCopyRespond(int nConnIndex, DWORD dwPlayerID, vector<KSAVED_COPY>& SavedCopy)
{
    BOOL                                    bResult             = false;
    BOOL                                    bRetCode            = false;
    size_t                                  uSavedCopyDataLen   = 0;
    KSAVED_COPY*                            pSavedCopy          = NULL;
    IKG_Buffer*                             piSendBuffer        = NULL;
    R2S_APPLY_PLAYER_SAVED_COPY_RESPOND*    pRespond            = NULL;
    size_t                                  uCopyCount          = 0;

    uCopyCount = SavedCopy.size();
        
    uSavedCopyDataLen = uCopyCount * sizeof(KSAVED_COPY);

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_APPLY_PLAYER_SAVED_COPY_RESPOND) + uSavedCopyDataLen));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRespond = (R2S_APPLY_PLAYER_SAVED_COPY_RESPOND*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID      = r2s_apply_player_saved_copy_respond;
    pRespond->dwPlayerID       = dwPlayerID;
    pRespond->wCopyCount       = (WORD)uCopyCount;
    pRespond->uDataLen         = uSavedCopyDataLen;
    
    pSavedCopy = (KSAVED_COPY*)pRespond->byData;
    for (vector<KSAVED_COPY>::iterator it = SavedCopy.begin(); it != SavedCopy.end(); ++it)
    {
        memcpy(pSavedCopy, &(*it), sizeof(KSAVED_COPY));
        ++pSavedCopy;
    }

    bRetCode = Send(nConnIndex, piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);    

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KGameServer::DoSaveRoleDataRespond(int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, int nUserValue)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piPackage       = NULL;
    R2S_SAVE_ROLE_DATA_RESPOND* pRespond        = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_SAVE_ROLE_DATA_RESPOND)));
    KGLOG_PROCESS_ERROR(piPackage);
    
    pRespond = (R2S_SAVE_ROLE_DATA_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRespond);
    
    pRespond->wProtocolID   = r2s_save_role_data_respond;
    pRespond->dwPlayerID    = dwPlayerID;
    pRespond->bSucceed      = bSucceed;
    pRespond->nUserValue    = nUserValue;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoSyncRoleData(int nConnIndex, DWORD dwRoleID, BYTE* pbyData, size_t uOffset, size_t uDataLen)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    IKG_Buffer*         piPackage   = NULL;
    R2S_SYNC_ROLE_DATA* pSyncData   = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_SYNC_ROLE_DATA) + uDataLen));
    KGLOG_PROCESS_ERROR(piPackage);

    pSyncData = (R2S_SYNC_ROLE_DATA*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSyncData);

    pSyncData->wProtocolID  = r2s_sync_role_data;
    pSyncData->dwRoleID     = dwRoleID;
    pSyncData->uOffset      = uOffset;

    if (pbyData)
    {
        memcpy(pSyncData->byData, pbyData, uDataLen);
    }

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoLoadRoleData(int nConnIndex, DWORD dwRoleID, BOOL bSucceed, size_t uRoleDataLen)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    IKG_Buffer*         piPackage   = NULL;
    R2S_LOAD_ROLE_DATA* pLoadData   = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_LOAD_ROLE_DATA));
    KGLOG_PROCESS_ERROR(piPackage);

    pLoadData = (R2S_LOAD_ROLE_DATA*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pLoadData);

    pLoadData->wProtocolID  = r2s_load_role_data;
    pLoadData->dwRoleID     = dwRoleID;
    pLoadData->bSucceed     = bSucceed;
    pLoadData->uRoleDataLen = uRoleDataLen;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoSyncTongMemberInfo(int nConnIndex, int dwPlayerID, DWORD dwMemberID, const KTongMember& crMember)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = 0;
    IKG_Buffer*                 piPackage   = NULL;
    R2S_SYNC_TONG_MEMBER_INFO*  pR2SPak     = NULL; 
    KTongMemberInfo*            pMemberInfo = NULL;
    KRole*                      pRole       = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_TONG_MEMBER_INFO));
    KGLOG_PROCESS_ERROR(piPackage);

    pR2SPak = (R2S_SYNC_TONG_MEMBER_INFO*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pR2SPak);

    pR2SPak->wProtocolID    = r2s_sync_tong_member_info;
    pR2SPak->dwPlayerID     = dwPlayerID;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(crMember.dwID);
    KGLOG_PROCESS_ERROR(pRole);

    pMemberInfo                     = &pR2SPak->MemberInfo;

    pMemberInfo->dwID               = crMember.dwID;
    pMemberInfo->nGroupID           = crMember.nGroupID;
    pMemberInfo->nJoinTime          = crMember.nJoinTime;
    pMemberInfo->nLastOfflineTime   = crMember.nLastOfflineTime;

    pMemberInfo->byLevel            = (BYTE)crMember.nLevel;
    pMemberInfo->byForceID          = (BYTE)crMember.dwForceID;
    pMemberInfo->dwMapID            = crMember.dwMapID;
    pMemberInfo->bIsOnline          = crMember.bIsOnline ? true : false;
    
    strncpy(pMemberInfo->szName, pRole->m_szName, sizeof(pMemberInfo->szName));
    pMemberInfo->szName[sizeof(pMemberInfo->szName) - 1] = '\0';

    strncpy(pMemberInfo->szRemark, crMember.szRemark, sizeof(pMemberInfo->szRemark));
    pMemberInfo->szRemark[sizeof(pMemberInfo->szRemark) - 1] = '\0';

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoTongDeleteMemberNotify(int nConnIndex, DWORD dwPlayerID, DWORD dwMemberID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = NULL;
    IKG_Buffer*                     piBuffer    = NULL;
    R2S_DELETE_TONG_MEMBER_NOTIFY*  pPackage    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_DELETE_TONG_MEMBER_NOTIFY));
    KGLOG_PROCESS_ERROR(piBuffer);

    pPackage = (R2S_DELETE_TONG_MEMBER_NOTIFY*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPackage);

    pPackage->wProtocolID   = r2s_delete_tong_member_notify;
    pPackage->dwPlayerID    = dwPlayerID;
    pPackage->dwMemberID    = dwMemberID;

    bRetCode = Send(nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoUpdateTongBaseInfo(int nConnIndex, DWORD dwPlayerID, KTong* pTong)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    R2S_APPLY_TONG_INFO_RESPOND*    pApplyRespond   = NULL;
    KTONG_BASE_INFO*                pBaseInfo       = NULL;

    piPackage       = KG_MemoryCreateBuffer(sizeof(R2S_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_BASE_INFO));
    KGLOG_PROCESS_ERROR(piPackage);
   
    pApplyRespond   = (R2S_APPLY_TONG_INFO_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyRespond);

    pApplyRespond->wProtocolID  = r2s_apply_tong_info_respond;
    pApplyRespond->dwPlayerID   = dwPlayerID;
    pApplyRespond->byInfoType   = (BYTE)tdtBaseInfo;

    pBaseInfo                       = (KTONG_BASE_INFO*)pApplyRespond->byData;
    pBaseInfo->dwMasterID           = pTong->m_dwMaster;
    pBaseInfo->byLevel              = (BYTE)pTong->m_nLevel;
    pBaseInfo->nMaxMemberCount      = pTong->m_nMaxMemberCount;
    pBaseInfo->byState              = (BYTE)pTong->m_eState;
    pBaseInfo->byCamp               = (BYTE)pTong->m_eCamp;
    pBaseInfo->nStateTimerOffset    = 0;
    
    if (pTong->m_nStateTimer)
    {
        pBaseInfo->nStateTimerOffset    = pTong->m_nStateTimer - g_pSO3GameCenter->m_nTimeNow;
    }    

    strncpy(pBaseInfo->szTongName, pTong->m_szName, sizeof(pBaseInfo->szTongName));
    pBaseInfo->szTongName[sizeof(pBaseInfo->szTongName) - 1] = '\0';

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoUpdateTongDevelopmentInfo(int nConnIndex, DWORD dwPlayerID, KTong* pTong)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    R2S_APPLY_TONG_INFO_RESPOND*    pApplyRespond   = NULL;
    KTONG_DEVELOPMENT_INFO*         pDevelopment    = NULL;

    piPackage       = KG_MemoryCreateBuffer(sizeof(R2S_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_DEVELOPMENT_INFO));
    KGLOG_PROCESS_ERROR(piPackage);
   
    pApplyRespond   = (R2S_APPLY_TONG_INFO_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyRespond);

    pApplyRespond->wProtocolID      = r2s_apply_tong_info_respond;
    pApplyRespond->dwPlayerID       = dwPlayerID;
    pApplyRespond->byInfoType       = (BYTE)tdtDevelopmentInfo;

    pDevelopment                    = (KTONG_DEVELOPMENT_INFO*)pApplyRespond->byData;
    pDevelopment->nFound            = pTong->m_nFund;
    pDevelopment->nDevelopmentPoint = pTong->m_nDevelopmentPoint;

    assert(sizeof(pDevelopment->m_byTechTree) == sizeof(pTong->m_byTechTree));
    memcpy(pDevelopment->m_byTechTree, pTong->m_byTechTree, sizeof(pDevelopment->m_byTechTree));

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoUpdateTongTextInfo(int nConnIndex, DWORD dwPlayerID, 
    int nDataType, const char cszText[]
)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    int                             nRetCode        = 0;
    HRESULT                         hRetCode        = E_FAIL;
    IKG_Buffer*                     piPackage       = NULL;
    R2S_APPLY_TONG_INFO_RESPOND*    pApplyRespond   = NULL;
    KTONG_TEXT_INFO*                pTextInfo       = NULL;
    size_t                          uStrLen         = 0;

    assert(cszText);

    uStrLen = strlen(cszText);

    piPackage = KG_MemoryCreateBuffer((unsigned int)sizeof(R2S_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_TEXT_INFO) + uStrLen + 1);
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyRespond = (R2S_APPLY_TONG_INFO_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyRespond);

    pApplyRespond->wProtocolID   = r2s_apply_tong_info_respond;
    pApplyRespond->dwPlayerID    = dwPlayerID;
    pApplyRespond->byInfoType    = (BYTE)nDataType;

    pTextInfo = (KTONG_TEXT_INFO*)pApplyRespond->byData;

    strcpy((char*)pTextInfo->byContent, cszText);

    pTextInfo->uDataLen = uStrLen + 1;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoUpdateTongSchemaInfo(int nConnIndex, DWORD dwPlayerID, KTongSchemaData* pData)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piPackage       = NULL;
    R2S_APPLY_TONG_INFO_RESPOND*    pApplyRespond   = NULL;
    KTONG_SCHEMA_DATA_INFO*         pUpdateGroup    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_APPLY_TONG_INFO_RESPOND) + sizeof(KTONG_SCHEMA_DATA_INFO));
    KGLOG_PROCESS_ERROR(piPackage);

    pApplyRespond = (R2S_APPLY_TONG_INFO_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pApplyRespond);

    pApplyRespond->wProtocolID   = r2s_apply_tong_info_respond;
    pApplyRespond->dwPlayerID    = dwPlayerID;
    pApplyRespond->byInfoType    = (BYTE)tdtSchemaInfo;

    pUpdateGroup = (KTONG_SCHEMA_DATA_INFO*)pApplyRespond->byData;
    memcpy(&pUpdateGroup->SchemaData, pData, sizeof(pUpdateGroup->SchemaData));

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoUpdateTongClientDataVersion(int nConnIndex, DWORD dwPlayerID, KTONG_UPDATE_FLAG eFrameType, int nUpdateFrame)
{
    BOOL                                    bResult             = false;
    BOOL                                    bRetcode            = false;
    IKG_Buffer*                             piPackage           = NULL;
    R2S_UPDATE_TONG_CLIENT_DATA_VERSION*    pServerDataVersion  = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_UPDATE_TONG_CLIENT_DATA_VERSION));
    KGLOG_PROCESS_ERROR(piPackage);

    pServerDataVersion = (R2S_UPDATE_TONG_CLIENT_DATA_VERSION*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pServerDataVersion);

    pServerDataVersion->wProtocolID     = r2s_update_tong_client_data_version;
    pServerDataVersion->dwPlayerID      = dwPlayerID;
    pServerDataVersion->byFrameType     = (BYTE)eFrameType;
    pServerDataVersion->nUpdateFrame    = nUpdateFrame;

    bRetcode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetcode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoInvitePlayerJoinTongRequest(
    int nConnIndex, DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, char szInviterName[], char szTongName[]
)
{
    BOOL                                    bResult             = false;
    BOOL                                    bRetcode            = false;
    IKG_Buffer*                             piPackage           = NULL;
    R2S_INVITE_PLAYER_JOIN_TONG_REQUEST*    pRequest            = NULL;

    assert(szInviterName);
    assert(szTongName);

    piPackage = KG_MemoryCreateBuffer(sizeof(R2S_INVITE_PLAYER_JOIN_TONG_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pRequest  = (R2S_INVITE_PLAYER_JOIN_TONG_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = r2s_invite_player_join_tong_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwInviterID   = dwInviterID;
    pRequest->dwTongID      = dwTongID;

    strncpy(pRequest->szInviterName, szInviterName, sizeof(pRequest->szInviterName));
    pRequest->szInviterName[sizeof(pRequest->szInviterName) - 1] = '\0';

    strncpy(pRequest->szTongName, szTongName, sizeof(pRequest->szTongName));
    pRequest->szTongName[sizeof(pRequest->szTongName) - 1] = '\0';

    bRetcode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetcode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoChangePlayerTongNotify(int nConnIndex, DWORD dwPlayerID, DWORD dwTongID, KCHANGE_TONG_REASON eReason, char szTongName[])
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    R2S_CHANGE_PLAYER_TONG_NOTIFY*  pNotify     = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_CHANGE_PLAYER_TONG_NOTIFY));
    KGLOG_PROCESS_ERROR(piBuffer);

    pNotify = (R2S_CHANGE_PLAYER_TONG_NOTIFY*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID    = r2s_change_player_tong_notify;
    pNotify->dwPlayerID     = dwPlayerID;
    pNotify->dwTongID       = dwTongID;
    pNotify->byReason       = (BYTE)eReason;
    strcpy(pNotify->szTongName, szTongName);

    bRetCode = Send(nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoTongEventNotify(int nConnIndex, DWORD dwPlayerID, int nCode, BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    IKG_Buffer*             piBuffer    = NULL;
    R2S_TONG_EVENT_NOTIFY*  pNotify     = NULL;

    KGLOG_PROCESS_ERROR(nCode > tenInvalid && nCode < tenTotal);
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_TONG_EVENT_NOTIFY) + uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pNotify = (R2S_TONG_EVENT_NOTIFY*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID = r2s_tong_event_notify;
    pNotify->dwPlayerID  = dwPlayerID;
    pNotify->byCode      = (BYTE)nCode;

    if (uDataLen)
    {
        KGLOG_PROCESS_ERROR(pbyData);
        memcpy(pNotify->byData, pbyData, uDataLen);
    }

    bRetCode = Send(nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::KTONG_BROADCAST_MESSAGE_TRAVERSE::operator () (KTongMember* pMember)
{
    KRole* pReceiver = NULL;

    KG_PROCESS_ERROR(pMember->dwID != dwExceptID);

    pReceiver = g_pSO3GameCenter->m_RoleManager.GetRole(pMember->dwID);
    KGLOG_PROCESS_ERROR(pReceiver);

    pMessage->dwPlayerID = pReceiver->m_dwPlayerID;
    pGameServer->Send(pReceiver->m_nConnIndex, piSendBuff);
Exit0:
    return true;
}

BOOL KGameServer::DoTongBroadcastMessage(KTong* pTong, int nCode, DWORD dwExceptID, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult     = false;
    IKG_Buffer*                         piBuffer    = NULL;
    R2S_TONG_BROADCAST_MESSAGE*         pMessage    = NULL;
    KTONG_BROADCAST_MESSAGE_TRAVERSE    Traverse;

    assert(pTong);
    KGLOG_PROCESS_ERROR(nCode > tbmInvalid && nCode < tbmTotal);
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_TONG_BROADCAST_MESSAGE) + uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pMessage = (R2S_TONG_BROADCAST_MESSAGE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pMessage);

    pMessage->wProtocolID   = r2s_tong_broadcast_message;
    pMessage->dwPlayerID    = 0;
    pMessage->byCode        = (BYTE)nCode;
    
    if (uDataLen)
    {
        assert(pbyData);
        memcpy(pMessage->byData, pbyData, uDataLen);
    }

    Traverse.pGameServer = this;
    Traverse.piSendBuff  = piBuffer;
    Traverse.pMessage    = pMessage;
    Traverse.dwExceptID  = dwExceptID;

    pTong->TraverseMember(Traverse);
    
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoGetTongDescriptionRepond(int nConnIndex, DWORD dwPlayerID, KTong* pTong)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    IKG_Buffer*                         piBuffer    = NULL;
    R2S_GET_TONG_DESCRIPTION_RESPOND*   pRespond    = NULL;

    assert(pTong);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_GET_TONG_DESCRIPTION_RESPOND));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (R2S_GET_TONG_DESCRIPTION_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID   = r2s_get_tong_description_respond;
    pRespond->dwPlayerID    = dwPlayerID;
    pRespond->dwTongID      = pTong->m_dwID;

    strncpy(pRespond->szTongName, pTong->m_szName, sizeof(pRespond->szTongName));
    pRespond->szTongName[sizeof(pRespond->szTongName) - 1] = '\0';

    bRetCode = Send(nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoSyncTongOnlineMessage(int nConnIndex, DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    R2S_SYNC_TONG_ONLINE_MESSAGE*   pSync       = NULL;
    IKG_Buffer*                     piBuffer    = NULL;
    KRole*                          pRole       = NULL;
    KTong*                          pTong       = NULL;
    size_t                          uDataLen    = 0;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->m_dwTongID);
    
    pTong = g_pSO3GameCenter->m_TongManager.GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    KG_PROCESS_ERROR(pTong->m_szOnlineMessage[0] != '\0');

    uDataLen = strlen(pTong->m_szOnlineMessage) + 1;

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_SYNC_TONG_ONLINE_MESSAGE) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pSync = (R2S_SYNC_TONG_ONLINE_MESSAGE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pSync);
    
    assert(uDataLen <= UCHAR_MAX);

    pSync->wProtocolID  = r2s_sync_tong_online_message;
    pSync->dwPlayerID   = dwPlayerID;
    pSync->byDataLen     = (BYTE)uDataLen;

    strncpy(pSync->szOnlineMessage , pTong->m_szOnlineMessage, uDataLen);
    pSync->szOnlineMessage[uDataLen - 1] = '\0';

    bRetCode = Send(nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoGmCommand(int nConnIndex, DWORD dwPlayerID, const char cszGmName[], const char cszGmCommand[])
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    IKG_Buffer*     piPackage   = NULL;
    R2S_GM_COMMAND* pGmCommand  = NULL;
    size_t          uPakSize    = 0;

    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);
    KGLOG_PROCESS_ERROR(cszGmName);
    KGLOG_PROCESS_ERROR(cszGmCommand);

    uPakSize = sizeof(R2S_GM_COMMAND) + strlen(cszGmCommand) + 1;
    KGLOG_PROCESS_ERROR(uPakSize < MAX_EXTERNAL_PACKAGE_SIZE);

    piPackage = KG_MemoryCreateBuffer((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(piPackage);

    pGmCommand  = (R2S_GM_COMMAND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pGmCommand);

    pGmCommand->wProtocolID = r2s_gm_command;

    pGmCommand->dwPlayerID  = dwPlayerID;
    strncpy(pGmCommand->szGmName, cszGmName, sizeof(pGmCommand->szGmName));
    pGmCommand->szGmName[sizeof(pGmCommand->szGmName) - 1] = '\0';
    strcpy(pGmCommand->szGmCommand, cszGmCommand);

    bRetCode = Send(nConnIndex, piPackage);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoJoinBattleFieldQueueRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nErrorCode)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    KRole*                                  pRole           = NULL;
    IKG_Buffer*                             piPackage       = NULL;
    R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND*    pJoinRespond    = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pJoinRespond = (R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pJoinRespond);

    pJoinRespond->wProtocolID = r2s_join_battle_field_queue_respond;
    pJoinRespond->dwPlayerID  = dwPlayerID;
    pJoinRespond->dwMapID     = dwMapID;
    pJoinRespond->nErrorCode  = nErrorCode;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoLeaveBattleFieldQueueRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nErrorCode)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    KRole*                                  pRole           = NULL;
    IKG_Buffer*                             piPackage       = NULL;
    R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND*   pLeaveRespond   = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pLeaveRespond = (R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pLeaveRespond);

    pLeaveRespond->wProtocolID = r2s_leave_battle_field_queue_respond;
    pLeaveRespond->dwPlayerID  = dwPlayerID;
    pLeaveRespond->dwMapID     = dwMapID;
    pLeaveRespond->nErrorCode  = nErrorCode;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoBattleFieldNotify(
    int nConnIndex, DWORD dwPlayerID, time_t nAvgQueueTime, time_t nPassTime,
    DWORD dwMapID, int nCopyIndex, BATTLE_FIELD_NOTIFY_TYPE eType
)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    IKG_Buffer*                 piPackage   = NULL;
    R2S_BATTLE_FIELD_NOTIFY*    pNotify     = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_BATTLE_FIELD_NOTIFY));
    KGLOG_PROCESS_ERROR(piPackage);

    pNotify = (R2S_BATTLE_FIELD_NOTIFY*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID    = r2s_battle_field_notify;
    pNotify->dwPlayerID     = dwPlayerID;
    pNotify->nAvgQueueTime  = nAvgQueueTime;
    pNotify->nPassTime      = (int)nPassTime;
    pNotify->dwMapID        = dwMapID;
    pNotify->nCopyIndex     = nCopyIndex;
    pNotify->eType          = eType;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoSyncBattleFieldList(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    IKG_Buffer*                 piPackage           = NULL;
    R2S_SYNC_BATTLE_FIELD_LIST* pBattleFieldList    = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_BATTLE_FIELD_LIST));
    KGLOG_PROCESS_ERROR(piPackage);

    pBattleFieldList = (R2S_SYNC_BATTLE_FIELD_LIST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pBattleFieldList);

    pBattleFieldList->wProtocolID   = r2s_sync_battle_field_list;
    pBattleFieldList->dwPlayerID    = dwPlayerID;
    pBattleFieldList->dwMapID       = dwMapID;
    pBattleFieldList->nCopyIndex    = nCopyIndex;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoAcceptJoinBattleFieldRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex)
{
    BOOL                                    bResult             = false;
    BOOL                                    bRetCode            = false;
    IKG_Buffer*                             piPackage           = NULL;
    R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND*   pAcceptJoinRespond  = NULL;
    int                                     nBattleSide         = 0;
    DWORD                                   dwJoinValue          = 0;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pAcceptJoinRespond = (R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pAcceptJoinRespond);

    nBattleSide = g_pSO3GameCenter->m_BattleFieldManager.GetBattleFieldSide(dwMapID, nCopyIndex, dwPlayerID);
    KGLOG_PROCESS_ERROR(nBattleSide != INVALID_BATTLE_SIDE);

    dwJoinValue = g_pSO3GameCenter->m_BattleFieldManager.GetBattleFieldJoinSign(dwMapID, nCopyIndex, dwPlayerID);

    pAcceptJoinRespond->wProtocolID     = r2s_accept_join_battle_field_respond;
    pAcceptJoinRespond->dwPlayerID      = dwPlayerID;
    pAcceptJoinRespond->dwEnterMapID    = dwMapID;
    pAcceptJoinRespond->nEnterCopyIndex = nCopyIndex;

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.GetEnterBattlePos(
        &pAcceptJoinRespond->nEnterX, &pAcceptJoinRespond->nEnterY, &pAcceptJoinRespond->nEnterZ,
        dwMapID, nBattleSide, dwJoinValue
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.GetLeaveBattlePos(
        &pAcceptJoinRespond->dwLeaveMapID, &pAcceptJoinRespond->nLeaveX,
        &pAcceptJoinRespond->nLeaveY, &pAcceptJoinRespond->nLeaveZ,
        dwMapID, nBattleSide, dwJoinValue
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    pAcceptJoinRespond->nLeaveCopyIndex = MAKE_INT64(0, 1);

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoLeaveBattleFieldRespond(int nConnIndex, DWORD dwPlayerID)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    IKG_Buffer*                     piPackage           = NULL;
    R2S_LEAVE_BATTLE_FIELD_RESPOND* pLeaveBattleField   = NULL;
    KRole*                          pRole               = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_LEAVE_BATTLE_FIELD_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pLeaveBattleField = (R2S_LEAVE_BATTLE_FIELD_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pLeaveBattleField);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    bRetCode = g_pSO3GameCenter->m_BattleFieldManager.IsBattleField(pRole->m_dwMapID);
    KGLOG_PROCESS_ERROR(bRetCode);

    pLeaveBattleField->wProtocolID = r2s_leave_battle_field_respond;
    pLeaveBattleField->dwPlayerID  = dwPlayerID;

    bRetCode = Send(nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoSyncPQ(int nConnIndex, const KGPQ& crPQ)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    IKG_Buffer*     piPackage   = NULL;
    R2S_SYNC_PQ*    pSyncPQ     = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_PQ));
    KGLOG_PROCESS_ERROR(piPackage);

    pSyncPQ  = (R2S_SYNC_PQ*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSyncPQ);

    pSyncPQ->wProtocolID    = r2s_sync_pq;
    pSyncPQ->dwPQID         = crPQ.m_dwPQID;
    pSyncPQ->dwPQTemplateID = crPQ.m_pPQTemplate->dwTemplateID;
    pSyncPQ->nBeginTime     = crPQ.m_nBeginTime;

    assert(sizeof(pSyncPQ->nPQValue) == sizeof(crPQ.m_nPQValue));
    memcpy(pSyncPQ->nPQValue, crPQ.m_nPQValue, sizeof(pSyncPQ->nPQValue));

    pSyncPQ->bFinished      = crPQ.m_bFinished ? 1 : 0;
    pSyncPQ->byOnwerType    = (BYTE)crPQ.m_eOwnerType;
    pSyncPQ->dwOnwerID1     = crPQ.m_dwOwnerID1;
    pSyncPQ->dwOnwerID2     = crPQ.m_dwOwnerID2;
    pSyncPQ->dwOnwerID3     = crPQ.m_dwOwnerID3;

    Send(nConnIndex, piPackage);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoBroadcastSyncPQ(const KGPQ& crPQ)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    IKG_Buffer*     piPackage   = NULL;
    R2S_SYNC_PQ*    pSyncPQ     = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_PQ));
    KGLOG_PROCESS_ERROR(piPackage);

    pSyncPQ  = (R2S_SYNC_PQ*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pSyncPQ);

    pSyncPQ->wProtocolID    = r2s_sync_pq;
    pSyncPQ->dwPQID         = crPQ.m_dwPQID;
    pSyncPQ->dwPQTemplateID = crPQ.m_pPQTemplate->dwTemplateID;
    pSyncPQ->nBeginTime     = crPQ.m_nBeginTime;

    assert(sizeof(pSyncPQ->nPQValue) == sizeof(crPQ.m_nPQValue));
    memcpy(pSyncPQ->nPQValue, crPQ.m_nPQValue, sizeof(pSyncPQ->nPQValue));

    pSyncPQ->bFinished      = crPQ.m_bFinished ? 1 : 0;
    pSyncPQ->byOnwerType    = (BYTE)crPQ.m_eOwnerType;
    pSyncPQ->dwOnwerID1     = crPQ.m_dwOwnerID1;
    pSyncPQ->dwOnwerID2     = crPQ.m_dwOwnerID2;
    pSyncPQ->dwOnwerID3     = crPQ.m_dwOwnerID3;

    Broadcast(piPackage);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoBroadcastDeletePQ(DWORD dwPQID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    IKG_Buffer*     piPackage   = NULL;
    R2S_DELETE_PQ*  pDeletePQ   = NULL;

    KGLOG_PROCESS_ERROR(dwPQID != ERROR_ID);

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_DELETE_PQ));
    KGLOG_PROCESS_ERROR(piPackage);

    pDeletePQ  = (R2S_DELETE_PQ*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pDeletePQ);

    pDeletePQ->wProtocolID  = r2s_delete_pq;
    pDeletePQ->dwPQID       = dwPQID;

    Broadcast(piPackage);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoBroadcastPQValueChange(DWORD dwPQID, int nIndex, int nValue)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piPackage       = NULL;
    R2S_PQ_VALUE_CHANGE*    pPQValueChange  = NULL;

    KGLOG_PROCESS_ERROR(dwPQID != ERROR_ID);

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_PQ_VALUE_CHANGE));
    KGLOG_PROCESS_ERROR(piPackage);

    pPQValueChange  = (R2S_PQ_VALUE_CHANGE*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pPQValueChange);

    pPQValueChange->wProtocolID    = r2s_pq_value_change;
    pPQValueChange->dwPQID         = dwPQID;
    pPQValueChange->nIndex         = nIndex;
    pPQValueChange->nValue         = nValue;

    Broadcast(piPackage);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoBroadcastPQFinish(DWORD dwPQID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    IKG_Buffer*     piPackage   = NULL;
    R2S_PQ_FINISH*  pPQFinish   = NULL;

    KGLOG_PROCESS_ERROR(dwPQID != ERROR_ID);

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_PQ_FINISH));
    KGLOG_PROCESS_ERROR(piPackage);

    pPQFinish  = (R2S_PQ_FINISH*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pPQFinish);

    pPQFinish->wProtocolID  = r2s_pq_finish;
    pPQFinish->dwPQID       = dwPQID;

    Broadcast(piPackage);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoSyncCampInfo(int nConnIndex)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    IKG_Buffer*         piPackage   = NULL;
    R2S_SYNC_CAMP_INFO* pCampInfo   = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_CAMP_INFO));
    KGLOG_PROCESS_ERROR(piPackage);

    pCampInfo  = (R2S_SYNC_CAMP_INFO*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pCampInfo);

    pCampInfo->wProtocolID  = r2s_sync_camp_info;
    pCampInfo->nCampLevel   = g_pSO3GameCenter->m_CampManager.m_nLevel;
    pCampInfo->nCampScore   = g_pSO3GameCenter->m_CampManager.m_nScore;
    
    if (nConnIndex != 0)
    {
        bRetCode = Send(nConnIndex, piPackage);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        Broadcast(piPackage);
    }
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoApplySetCampRespond(KRole* pRole, CAMP_RESULT_CODE eResult)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    IKG_Buffer*                 piPackage   = NULL;
    R2S_APPLY_SET_CAMP_RESPOND* pRespond    = NULL;

    piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_APPLY_SET_CAMP_RESPOND));
    KGLOG_PROCESS_ERROR(piPackage);

    pRespond  = (R2S_APPLY_SET_CAMP_RESPOND*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID  = r2s_apply_set_camp_respond;
    pRespond->dwPlayerID   = pRole->m_dwPlayerID;
    pRespond->byCamp       = (BYTE)pRole->m_eCamp;
    pRespond->byResult     = (BYTE)eResult;
    
    bRetCode = Send(pRole->m_nConnIndex, piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KGameServer::DoAuctionLookupRespond(DWORD dwPlayerID, BYTE byRespondID, BYTE byCode, KAUCTION_PAGE_HEAD* pPage)
{
    BOOL                        bResult     = false;
    BOOL                        bRetcode    = false;
    KRole*                      pRole       = NULL;
    IKG_Buffer*                 piBuffer    = NULL;
    R2S_AUCTION_LOOKUP_RESPOND* pRespond    = NULL;
    size_t                      uSize       = 0;

    if (pPage)
    {
        uSize = pPage->uSize;
    }

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_AUCTION_LOOKUP_RESPOND) + uSize));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (R2S_AUCTION_LOOKUP_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID       = r2s_auction_lookup_respond;
    pRespond->byRespondID       = byRespondID;
    pRespond->dwPlayerID        = dwPlayerID;
    pRespond->byCode            = byCode; 

    pRespond->Page.nTotalCount  = 0;
    pRespond->Page.uSize        = 0;

    if (pPage)
    {
        memcpy(&pRespond->Page, pPage, sizeof(pRespond->Page) + pPage->uSize);
    }

    bRetcode = Send(pRole->m_nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetcode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoAuctionBidRespond(DWORD dwPlayerID, BYTE byCode, int nBidPrice)
{
    BOOL                        bResult     = false;
    BOOL                        bRetcode    = false;
    KRole*                      pRole       = NULL;
    IKG_Buffer*                 piBuffer    = NULL;
    R2S_AUCTION_BID_RESPOND*    pRespond    = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_AUCTION_BID_RESPOND));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (R2S_AUCTION_BID_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID       = r2s_auction_bid_respond;
    pRespond->dwPlayerID        = dwPlayerID;
    pRespond->byCode            = byCode;
    pRespond->nBidPrice         = nBidPrice;

    bRetcode = Send(pRole->m_nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetcode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoAuctionCancelRespond(DWORD dwPlayerID, BYTE byCode)
{
    BOOL                        bResult     = false;
    BOOL                        bRetcode    = false;
    KRole*                      pRole       = NULL;
    IKG_Buffer*                 piBuffer    = NULL;
    R2S_AUCTION_CANCEL_RESPOND* pRespond    = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_AUCTION_CANCEL_RESPOND));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (R2S_AUCTION_CANCEL_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID   = r2s_auction_cancel_respond;
    pRespond->dwPlayerID    = dwPlayerID;
    pRespond->byCode        = byCode;

    bRetcode = Send(pRole->m_nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetcode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoAuctionMessageNotify(DWORD dwPlayerID, BYTE byCode, const char* pszSaleName, int nPrice)
{
    BOOL                        bResult     = false;
    BOOL                        bRetcode    = false;
    KRole*                      pRole       = NULL;
    IKG_Buffer*                 piBuffer    = NULL;
    R2S_AUCTION_MESSAGE_NOTIFY* pNotify     = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->IsOnline()); // 只通知在线玩家

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_AUCTION_MESSAGE_NOTIFY));
    KGLOG_PROCESS_ERROR(piBuffer);

    pNotify = (R2S_AUCTION_MESSAGE_NOTIFY*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pNotify);

    pNotify->wProtocolID    = r2s_auction_message_notify;
    pNotify->dwPlayerID     = dwPlayerID;
    pNotify->byCode         = byCode;
    
    pNotify->szSaleName[0] = '\0';
    if (pszSaleName)
    {
        strncpy(pNotify->szSaleName, pszSaleName, sizeof(pNotify->szSaleName));
        pNotify->szSaleName[sizeof(pNotify->szSaleName) - 1] = '\0';
    }

    pNotify->nPrice         = nPrice;

    bRetcode = Send(pRole->m_nConnIndex, piBuffer);
    KGLOG_PROCESS_ERROR(bRetcode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoRemoteLuaCall(int nConnIndex, const char cszFunction[], BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    BOOL                        bRetcode    = false;
    IKG_Buffer*                 piBuffer    = NULL;
    R2S_REMOTE_LUA_CALL*        pRemoteLua  = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(R2S_REMOTE_LUA_CALL) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRemoteLua = (R2S_REMOTE_LUA_CALL*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRemoteLua);

    pRemoteLua->wProtocolID   = r2s_remote_lua_call;

    strncpy(pRemoteLua->szFunction, cszFunction, sizeof(pRemoteLua->szFunction));
    pRemoteLua->szFunction[sizeof(pRemoteLua->szFunction) - 1] = '\0';

    pRemoteLua->uParamLen = uDataLen;
    memcpy(pRemoteLua->byParam, pbyData, uDataLen);

    if (nConnIndex == 0)
    {
        Broadcast(piBuffer, 0);
    }
    else
    {
        bRetcode = Send(nConnIndex, piBuffer);
        KGLOG_PROCESS_ERROR(bRetcode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoAchievementAnnounceRespond(DWORD dwPlayerID, DWORD dwAchievementID)
{
    BOOL                                        bResult     = false;
    BOOL                                        bRetcode    = false;
    IKG_Buffer*                                 piBuffer    = NULL;
    R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND*    pRespond    = NULL;
    KRole*                                      pRole       = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRespond = (R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRespond);

    pRespond->wProtocolID = r2s_achievement_global_announce_respond;

    strncpy(pRespond->szRoleName, pRole->m_szName, sizeof(pRespond->szRoleName));
    pRespond->szRoleName[sizeof(pRespond->szRoleName) - 1] = '\0';

    pRespond->dwAchievementID = dwAchievementID;

    Broadcast(piBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGameServer::DoSyncGlobalSystemValue(int nConnIndex)
{
    BOOL                            bResult                     = false;
    BOOL                            bRetCode                    = false;
    IKG_Buffer*                     piBuffer                    = NULL;
    R2S_SYNC_GLOBAL_SYSTEM_VALUE*   pPak                        = NULL;
    KGlobalSystemValueManager&      rGlobalSystemValueManager   = g_pSO3GameCenter->m_GlobalSystemValueManager;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(R2S_SYNC_GLOBAL_SYSTEM_VALUE));
    KGLOG_PROCESS_ERROR(piBuffer);

    pPak = (R2S_SYNC_GLOBAL_SYSTEM_VALUE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID           = r2s_sync_global_system_value;
    pPak->nGameWorldStartTime   = rGlobalSystemValueManager.m_GameWorldStartTime.nValue;
    pPak->nMaxPlayerLevel       = rGlobalSystemValueManager.m_MaxPlayerLevel.nValue;
    
    if (nConnIndex == 0)
    {
        Broadcast(piBuffer);
    }
    else
    {
        bRetCode = Send(nConnIndex, piBuffer);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

//AutoCode:-发送协议函数结束-
