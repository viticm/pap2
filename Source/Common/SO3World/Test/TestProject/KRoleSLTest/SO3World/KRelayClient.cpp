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

KRelayClient g_RelayClient;
extern int g_nServerTestTime;

KRelayClient::KRelayClient(void)
{
    m_piSocketStream           = NULL;
	m_nPingCycle               = 0;
	m_nLastSendPacketTime      = 0;

    m_dwSyncRoleID              = ERROR_ID;
    m_pbySyncRoleBuffer         = NULL;
    m_uSyncRoleOffset           = 0;

    m_pbySaveRoleBuffer         = NULL;

    memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
    memset(m_uProtocolSize, 0, sizeof(m_uProtocolSize));

    // ------------------ 协议注册 --------------------------------------------------------------------
    REGISTER_INTERNAL_FUNC(r2s_handshake_respond, &KRelayClient::OnHandshakeRespond, sizeof(R2S_HANDSHAKE_RESPOND));
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
    REGISTER_INTERNAL_FUNC(r2s_party_message_notify, &KRelayClient::OnPartyMessageNotify, sizeof(R2S_PARTY_MESSAGE_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_team_set_formation_leader_notify, &KRelayClient::OnSetFormationLeaderNotify, sizeof(R2S_TEAM_SET_FORMATION_LEADER_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_online_flag, &KRelayClient::OnSyncTeamMemberOnlineFlag, sizeof(R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_max_lmr, &KRelayClient::OnTeamSyncMemberMaxLMR, sizeof(R2S_SYNC_TEAM_MEMBER_MAX_LMR));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_current_lmr, &KRelayClient::OnTeamSyncMemberCurrentLMR, sizeof(R2S_SYNC_TEAM_MEMBER_CURRENT_LMR));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_misc, &KRelayClient::OnTeamSyncMemberMisc, sizeof(R2S_SYNC_TEAM_MEMBER_MISC));
    REGISTER_INTERNAL_FUNC(r2s_sync_team_member_position, &KRelayClient::OnTeamSyncMemberPosition, sizeof(R2S_SYNC_TEAM_MEMBER_POSITION));
    REGISTER_INTERNAL_FUNC(r2s_team_set_mark_respond, &KRelayClient::OnTeamSetMarkRespond, sizeof(R2S_TEAM_SET_MARK_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_team_camp_change, &KRelayClient::OnTeamCampChange, sizeof(R2S_TEAM_CAMP_CHANGE));

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
    REGISTER_INTERNAL_FUNC(r2s_get_mail_content, &KRelayClient::OnGetMailContent, sizeof(R2S_GET_MAIL_CONTENT));
    REGISTER_INTERNAL_FUNC(r2s_get_mail_attachment, &KRelayClient::OnGetMailAttachment, sizeof(R2S_GET_MAIL_ATTACHMENT));
    REGISTER_INTERNAL_FUNC(r2s_new_mail_notify, &KRelayClient::OnNewMailNotify, sizeof(R2S_NEW_MAIL_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_mail_general_respond, &KRelayClient::OnMailGeneralRespond, sizeof(R2S_MAIL_GENERAL_RESPOND));

    REGISTER_INTERNAL_FUNC(r2s_sync_mid_map_mark, &KRelayClient::OnSyncMidMapMark, sizeof(R2S_SYNC_MID_MAP_MARK));
    REGISTER_INTERNAL_FUNC(r2s_map_copy_do_clear_player_prepare, &KRelayClient::OnMapCopyDoClearPlayerPrepare, sizeof(R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE));
    REGISTER_INTERNAL_FUNC(r2s_map_copy_do_clear_player, &KRelayClient::OnMapCopyDoClearPlayer, sizeof(R2S_MAP_COPY_DO_CLEAR_PLAYER));
    REGISTER_INTERNAL_FUNC(r2s_set_map_copy_owner, &KRelayClient::OnSetMapCopyOwner, sizeof(R2S_SET_MAP_COPY_OWNER));
    REGISTER_INTERNAL_FUNC(r2s_apply_player_saved_copy_respond, &KRelayClient::OnApplyPlayerSavedCopyRespond, sizeof(R2S_APPLY_PLAYER_SAVED_COPY_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_road_track_force, &KRelayClient::OnSyncRoadTrackForce, sizeof(R2S_SYNC_ROAD_TRACK_FORCE));
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
    REGISTER_INTERNAL_FUNC(r2s_update_tong_client_data_version, &KRelayClient::OnUpdateTongClientDataVersion, sizeof(R2S_UPDATE_TONG_CLIENT_DATA_VERSION));
    REGISTER_INTERNAL_FUNC(r2s_invite_player_join_tong_request, &KRelayClient::OnInvitePlayerJoinTongRequest, sizeof(R2S_INVITE_PLAYER_JOIN_TONG_REQUEST));
    REGISTER_INTERNAL_FUNC(r2s_change_player_tong_notify, &KRelayClient::OnChangePlayerTongNotify, sizeof(R2S_CHANGE_PLAYER_TONG_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_tong_event_notify, &KRelayClient::OnTongEventNotify, sizeof(R2S_TONG_EVENT_NOTIFY));
    REGISTER_INTERNAL_FUNC(r2s_tong_broadcast_message, &KRelayClient::OnBroadcastTongMessage, sizeof(R2S_TONG_BROADCAST_MESSAGE));
    REGISTER_INTERNAL_FUNC(r2s_get_tong_description_respond, &KRelayClient::OnGetTongDescriptionRespond, sizeof(R2S_GET_TONG_DESCRIPTION_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_tong_online_message, &KRelayClient::OnSyncTongOnlineMessage, sizeof(R2S_SYNC_TONG_ONLINE_MESSAGE));

    REGISTER_INTERNAL_FUNC(r2s_auction_lookup_respond, &KRelayClient::OnAuctionLookupRespond, sizeof(R2S_AUCTION_LOOKUP_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_bid_respond, &KRelayClient::OnAuctionBidRespond, sizeof(R2S_AUCTION_BID_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_cancel_respond, &KRelayClient::OnAuctionCancelRespond, sizeof(R2S_AUCTION_CANCEL_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_auction_message_notify, &KRelayClient::OnAuctionMessageNotify, sizeof(R2S_AUCTION_MESSAGE_NOTIFY));

    REGISTER_INTERNAL_FUNC(r2s_remote_lua_call, &KRelayClient::OnRemoteLuaCall, sizeof(R2S_REMOTE_LUA_CALL));

    REGISTER_INTERNAL_FUNC(r2s_achievement_global_announce_respond, &KRelayClient::OnAchievementGlobalAnnounceRespond, sizeof(R2S_ACHIEVEMENT_GLOBAL_ANNOUNCE_RESPOND));
    REGISTER_INTERNAL_FUNC(r2s_sync_global_system_value, &KRelayClient::OnSyncGlobalSystemValue, sizeof(R2S_SYNC_GLOBAL_SYSTEM_VALUE));
    
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
    
	m_nPingCycle    = nPingCycle;
    m_nWorldIndex   = 0;

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

BOOL KRelayClient::Breathe()
{
    BOOL                    bResult         = false;
	int                     nRetCode        = false;
    IKG_Buffer*             piBuffer        = NULL;
    int                     szTest[] = {16384, 32767, 32768, 32769, 49152, 65536, 262143, 262144, 262145};

    assert(m_piSocketStream);

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

        if (g_nServerTestTime < 9)
        {
            KPlayer* kTestPlayer = new KPlayer();
            KG_PROCESS_ERROR(kTestPlayer);

            kTestPlayer->m_dwID = szTest[g_nServerTestTime];
            kTestPlayer->m_eGameStatus = gsPlaying;
            SaveRoleData(kTestPlayer);

            KG_DELETE(kTestPlayer);
            g_nServerTestTime++;
        }
        else
        {
            if (g_nServerTestTime == 9)
            {
                printf("Save Test Over\n");
                DoLoadRoleDataRequest(szTest[g_nServerTestTime]);
                g_nServerTestTime++;
            }

        }

	    nRetCode = m_piSocketStream->CheckCanRecv(&TimeVal);
        if (nRetCode == 0)
        {
            break;
        }

        KGLOG_PROCESS_ERROR(nRetCode == 1);

        KG_COM_RELEASE(piBuffer);

		nRetCode = m_piSocketStream->Recv(&piBuffer);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		pHeader = (INTERNAL_PROTOCOL_HEADER*)piBuffer->GetData();
        KGLOG_PROCESS_ERROR(pHeader);
          
        KGLOG_PROCESS_ERROR(pHeader->wProtocolID < r2s_protocol_end);
        
        uPakSize = piBuffer->GetSize();
        KGLOG_PROCESS_ERROR(uPakSize >= m_uProtocolSize[pHeader->wProtocolID]);

        pFunc = m_ProcessProtocolFuns[pHeader->wProtocolID];
        KGLOG_PROCESS_ERROR(pFunc);

		(this->*pFunc)((BYTE*)pHeader, uPakSize);
	}

    DoUpdatePerformance();

    bResult = true;
Exit0:
    if (!bResult)
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

struct KTraversePlayerForSaveOnExit
{
    BOOL operator()(DWORD dwID, KPlayer* pPlayer)
    {
        KGLogPrintf(KGLOG_INFO, "Save role, waiting ... %u ...\n", m_uRoleCount);

        if (pPlayer->m_eGameStatus == gsPlaying)
        {
            pPlayer->SavePosition();

            g_RelayClient.SaveRoleData(pPlayer);
        }

        m_uRoleCount--;

        return true;
    }
    
    size_t m_uRoleCount;
};

BOOL KRelayClient::SaveAllPlayersForExit()
{
    BOOL bResult = false;

    KTraversePlayerForSaveOnExit TraverseFunc;

    TraverseFunc.m_uRoleCount = g_pSO3World->m_PlayerSet.GetObjCount();

    g_pSO3World->m_PlayerSet.Traverse(TraverseFunc);

    return true;
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

    bResult = true;
Exit0:
	return bResult;
}

void KRelayClient::OnHandshakeRespond(BYTE* pbyData, size_t uDataLen)
{
	BOOL                    bRetCode   = false;
	R2S_HANDSHAKE_RESPOND*  pHandshake = (R2S_HANDSHAKE_RESPOND*)pbyData;

	assert(pHandshake->nWorldIndex > PLAYER_ID_PREFIX);
	assert(pHandshake->nWorldIndex < CLIENT_ID_PREFIX);
    assert(g_pSO3World->m_piRecorderTools);

    g_pSO3World->m_nBaseTime   = pHandshake->nBaseTime;
    g_pSO3World->m_dwStartTime = g_pSO3World->m_piRecorderTools->GetTickCount();;
    g_pSO3World->m_nStartLoop  = pHandshake->nStartFrame;
    g_pSO3World->m_nGameLoop   = pHandshake->nStartFrame;

	g_pSO3World->m_SceneSet.SetPrefix(pHandshake->nWorldIndex);
	g_pSO3World->m_PlayerSet.SetPrefix(PLAYER_ID_PREFIX);
	g_pSO3World->m_NpcSet.SetPrefix(pHandshake->nWorldIndex);
	g_pSO3World->m_DoodadSet.SetPrefix(pHandshake->nWorldIndex);
	g_pSO3World->m_ItemSet.SetPrefix(pHandshake->nWorldIndex);

    m_nWorldIndex = pHandshake->nWorldIndex;

	KGLogPrintf(KGLOG_INFO, "Set world index = %d\n", pHandshake->nWorldIndex);

Exit0:
	return;
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

    pScene->m_bCanPK = pMapParams->bCanPK;

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
    pScene->m_nReviveCycle          = pMapParams->nReviveCycle;
    pScene->m_bDoNotGoThroughRoof   = pMapParams->bDoNotGoThroughRoof;
    pScene->m_nType                 = pMapParams->nType;

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
		pPlayer->m_eGameStatus                  = gsSaveDataForTransmission;
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

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRequest->dwRoleID);
    if (pPlayer)
    {
        GUID Guid = {0, 0, 0, 0};
        DoTransferPlayerRespond(pRequest->dwRoleID, false, Guid);
        goto Exit0;
    }

	pPlayer = g_pSO3World->NewPlayer(pRequest->dwRoleID);
	KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->LoadBaseInfo(&pRequest->BaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_ExtPointInfo     = pRequest->ExtPointInfo;
	pPlayer->m_nlLeftTime       = pRequest->nlLeftTime;
    pPlayer->m_dwTeamID        = pRequest->dwPartyID;
	pPlayer->m_nTimer           = 0;
	pPlayer->m_eGameStatus      = gsWaitForConnect;
    pPlayer->m_nBattleFieldSide = pRequest->nBattleFieldSide;
    pPlayer->m_dwTongID         = pRequest->dwTongID;
    
    strncpy(pPlayer->m_szAccount, pRequest->szAccount, sizeof(pPlayer->m_szAccount));
    pPlayer->m_szAccount[sizeof(pPlayer->m_szAccount) - 1] = '\0';

    strncpy(pPlayer->m_szName, pRequest->szRoleName, sizeof(pPlayer->m_szName));
    pPlayer->m_szName[sizeof(pPlayer->m_szName) - 1] = '\0';

    assert(g_pSO3World->m_piRecorderTools);
    bRetCode = g_pSO3World->m_piRecorderTools->CreateGUID(&pPlayer->m_Guid);
    KGLOG_PROCESS_ERROR(bRetCode);

	DoTransferPlayerRespond(pRequest->dwRoleID, true, pPlayer->m_Guid);

    bResult = true;
Exit0:

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
	KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsTransmission);

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

    g_PlayerServer.DoSyncBattleFieldSide(pPlayer); // 如果在战场，需要通知客户端你目前所属的阵营

	pPlayer->m_eGameStatus  = gsWaitForDisconnect;

Exit0:
	return;
}

void KRelayClient::OnPlayerLoginRequest(BYTE* pbyData, size_t uDataLen)
{
	BOOL                        bRetCode            = false;
	BOOL                        bResult             = false;
	KPlayer*                    pPlayer             = NULL;
    R2S_PLAYER_LOGIN_REQUEST*   pRequest            = (R2S_PLAYER_LOGIN_REQUEST*)pbyData;
	
	pPlayer = g_pSO3World->NewPlayer(pRequest->dwRoleID);
	KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = pPlayer->LoadBaseInfo(&pRequest->BaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayer->m_ExtPointInfo     = pRequest->ExtPointInfo;
	pPlayer->m_nlLeftTime       = pRequest->nlLeftTime;
    pPlayer->m_dwTeamID        = pRequest->dwPartyID;
    pPlayer->m_dwTongID         = pRequest->dwTongID;
    pPlayer->m_nTimer           = 0;
    pPlayer->m_nBattleFieldSide = pRequest->nBattleSide;
	pPlayer->m_eGameStatus      = gsWaitForConnect;

    strncpy(pPlayer->m_szAccount, pRequest->szAccount, sizeof(pPlayer->m_szAccount));
    pPlayer->m_szAccount[sizeof(pPlayer->m_szAccount) - 1] = '\0';

    strncpy(pPlayer->m_szName, pRequest->szRoleName, sizeof(pPlayer->m_szName));
    pPlayer->m_szName[sizeof(pPlayer->m_szName) - 1] = '\0';

    assert(g_pSO3World->m_piRecorderTools);
	bRetCode = g_pSO3World->m_piRecorderTools->CreateGUID(&pPlayer->m_Guid);
    KGLOG_PROCESS_ERROR(bRetCode);

	DoPlayerLoginRespond(pRequest->dwRoleID, true, pPlayer->m_Guid, pRequest->dwPacketIdentity);

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

void KRelayClient::OnConfirmPlayerLoginRespond(BYTE* pbyData, size_t uDataLen)
{
	R2S_CONFIRM_PLAYER_LOGIN_RESPOND*   pRespond = (R2S_CONFIRM_PLAYER_LOGIN_RESPOND*)pbyData;
    KPlayer*                            pPlayer  = NULL;

	// 登陆期间，客户端断线，就有可能在这里找不到
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForPermit);

    g_LogClient.LogPlayerLogin(pPlayer->m_szAccount, pPlayer->m_szName);

    if (!pRespond->bPermit)
    {
        KGLogPrintf(KGLOG_INFO, "Login denied by game center, shutdown(%d)\n", pPlayer->m_nConnIndex);

        g_PlayerServer.Detach(pPlayer->m_nConnIndex);
        goto Exit0;
    }

    g_PlayerServer.DoSyncPlayerBaseInfo(pPlayer->m_nConnIndex, pPlayer);

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
    R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST* pPartyInvitation = (R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST*)pbyData;
    KPlayer* pDstPlayer = NULL;
    
    pDstPlayer = g_pSO3World->m_PlayerSet.GetObj(pPartyInvitation->dwInviteDst);
    KG_PROCESS_ERROR(pDstPlayer);
    
    if (pDstPlayer->m_eGameStatus != gsPlaying)
    {
        g_RelayClient.DoInviteJoinTeamRespond(
            pPartyInvitation->szInviteSrc, pPartyInvitation->dwInviteDst, false
        );
        goto Exit0;
    }
    
    pPartyInvitation->szInviteSrc[sizeof(pPartyInvitation->szInviteSrc) - 1] = '\0';

    g_PlayerServer.DoInvitePlayerJoinPartyRequest(
        pPartyInvitation->dwInviteDst, pPartyInvitation->szInviteSrc
    );

Exit0:
    return;
}

void KRelayClient::OnApplyJoinTeamRequest(BYTE* pbyData, size_t uDataLen)
{
    R2S_APPLY_JOIN_TEAM_REQUEST* pApplyJoinParty = (R2S_APPLY_JOIN_TEAM_REQUEST*)pbyData;

    pApplyJoinParty->szApplySrc[sizeof(pApplyJoinParty->szApplySrc) - 1] = '\0';

    g_PlayerServer.DoApplyJoinTeamRequest(
        pApplyJoinParty->dwApplyDst, pApplyJoinParty->szApplySrc
    );

    return;
}

void KRelayClient::OnTeamCreateNotify(BYTE* pbyData, size_t uDataLen)
{
    int                      nRetCode       = false;
    R2S_TEAM_CREATE_NOTIFY*  pNotify        = (R2S_TEAM_CREATE_NOTIFY*)pbyData;

    nRetCode = g_pSO3World->m_TeamServer.Create(pNotify->dwPartyID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KRelayClient::OnTeamAddMemberNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode                = false;
    R2S_TEAM_ADD_MEMBER_NOTIFY* pPartyAddMemberNotify   = (R2S_TEAM_ADD_MEMBER_NOTIFY*)pbyData;
    KTeam*                      pTeam                   = NULL;
    KTEAM_MEMBER_INFO*          pMemberInfo             = NULL;
    KPlayer*                    pPartyLeader            = NULL;
    KPlayer*                    pPartyMember            = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPartyAddMemberNotify->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    bRetCode = g_pSO3World->m_TeamServer.AddMember(
        pPartyAddMemberNotify->dwPartyID,
        pPartyAddMemberNotify->nGroupIndex,
        pPartyAddMemberNotify->dwMemberID
    );  
    KGLOG_PROCESS_ERROR(bRetCode);

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPartyAddMemberNotify->dwPartyID, pPartyAddMemberNotify->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    strncpy(pMemberInfo->szRoleName, pPartyAddMemberNotify->szMemberName, sizeof(pMemberInfo->szRoleName));
    pMemberInfo->szRoleName[sizeof(pMemberInfo->szRoleName) - 1] = '\0';

    pPartyMember = g_pSO3World->m_PlayerSet.GetObj(pPartyAddMemberNotify->dwMemberID);
    if (pPartyMember)
    {
        g_PlayerServer.DoMessageNotify(
            pPartyMember->m_nConnIndex, ectPartyNotifyCode, epncPartyJoined, 
            pMemberInfo->szRoleName, sizeof(pMemberInfo->szRoleName)
        );

        g_PlayerServer.DoSyncTeamBaseInfo(pPartyMember, true);

        for (int i = 0; i < pTeam->nGroupNum; i++)
        {
            for (
                KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
                it != pTeam->MemberGroup[i].MemberList.end(); ++it
            )
            {
                g_PlayerServer.DoSyncTeamMemberInfo(pPartyMember, i, it->dwMemberID);
            }
        }
    }
    //else: player not in this server !

    g_PlayerServer.DoTeamAddMemberNotify(
        pPartyAddMemberNotify->dwPartyID, pPartyAddMemberNotify->nGroupIndex, pPartyAddMemberNotify->dwMemberID
    );

    if (pPartyMember)
    {
        pPartyMember->ResetPartyLastSyncParamRecord();
    }

Exit0:
    return;
}


void KRelayClient::OnTeamDelMemberNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode                = false;
    R2S_TEAM_DEL_MEMBER_NOTIFY* pPartyDelMemberNotify   = (R2S_TEAM_DEL_MEMBER_NOTIFY*)pbyData;
    KPlayer*                    pPlayer                 = NULL;

    bRetCode = g_pSO3World->m_TeamServer.DelMember(
        pPartyDelMemberNotify->dwPartyID, pPartyDelMemberNotify->nGroupIndex, pPartyDelMemberNotify->dwMemberID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoTeamDelMemberNotify(
        pPartyDelMemberNotify->dwPartyID, pPartyDelMemberNotify->nGroupIndex, pPartyDelMemberNotify->dwMemberID
    );

Exit0:
    return;
}

void KRelayClient::OnTeamDisbandNotify(BYTE* pbyData, size_t uDataLen)
{
    int                         nRetCode            = false;
    R2S_TEAM_DISBAND_NOTIFY*    pPartyDisbandNotify = (R2S_TEAM_DISBAND_NOTIFY*)pbyData;
    KTeam*                      pTeam               = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPartyDisbandNotify->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    g_PlayerServer.DoTeamDisbandNotify(pPartyDisbandNotify->dwPartyID);

    g_pSO3World->m_TeamServer.Destroy(pPartyDisbandNotify->dwPartyID);  

Exit0:
    return;
}

void KRelayClient::OnTeamChangeAuthorityNotify(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode                    = false;
    R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*   pPartyChangeLeaderNotify    = (R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*)pbyData;

    // 设置队长
    bRetCode = g_pSO3World->m_TeamServer.SetAuthority(
        pPartyChangeLeaderNotify->dwPartyID, (KTEAM_AUTHORITY_TYPE)pPartyChangeLeaderNotify->byType, pPartyChangeLeaderNotify->dwNewID
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    // 通知队伍成员
    g_PlayerServer.DoTeamChangeAuthorityNotify(
        pPartyChangeLeaderNotify->dwPartyID, (KTEAM_AUTHORITY_TYPE)pPartyChangeLeaderNotify->byType, pPartyChangeLeaderNotify->dwNewID
    );

Exit0:
    return;
}

void KRelayClient::OnTeamSetLootModeNotify(BYTE* pbyData, size_t uDataLen)
{
    int                             nRetCode    = false;
    R2S_TEAM_SET_LOOT_MODE_NOTIFY*  pNotify     = (R2S_TEAM_SET_LOOT_MODE_NOTIFY*)pbyData;
    KTeam*                          pTeam       = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pNotify->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->nLootMode    = pNotify->nLootMode;
    pTeam->nRollQuality = pNotify->nRollQuality;

    g_PlayerServer.DoTeamSetLootModeNotify(pNotify->dwPartyID, pNotify->nLootMode, pNotify->nRollQuality);
    
Exit0:
    return;
}

void KRelayClient::OnSyncTeamMemberOnlineFlag(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode    = false;
    R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*   pPak        = (R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*)pbyData;
    size_t                              uPakSize    = 0;
    KTEAM_MEMBER_INFO*                  pMemberInfo = NULL;



    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwPartyID, pPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->bOnlineFlag = pPak->bOnlineFlag;

    g_PlayerServer.DoSyncTeamMemberOnlineFlag(pPak->dwPartyID, pPak->dwMemberID, pPak->bOnlineFlag);

Exit0:
    return;
}

void KRelayClient::OnPartyMessageNotify(BYTE* pbyData, size_t uDataLen)
{
    R2S_PARTY_MESSAGE_NOTIFY* pMsg      = (R2S_PARTY_MESSAGE_NOTIFY*)pbyData;
    KPlayer*                  pPlayer   = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pMsg->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoMessageNotify(
        pPlayer->m_nConnIndex, ectPartyNotifyCode, pMsg->nCode, 
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
        pNotify->dwPartyID, pNotify->nGroupIndex, pNotify->dwNewFormationLeader
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoTeamSetFormationLeaderNotify(
        pNotify->dwPartyID, pNotify->nGroupIndex, pNotify->dwNewFormationLeader
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

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwPartyID, pPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nMaxLife         = pPak->nMaxLife;
    pMemberInfo->nMaxMana         = pPak->nMaxMana;

    g_PlayerServer.DoSyncTeamMemberMaxLMR(pPak->dwPartyID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberCurrentLMR(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bRetCode        = false;
    R2S_SYNC_TEAM_MEMBER_CURRENT_LMR*   pPak            = (R2S_SYNC_TEAM_MEMBER_CURRENT_LMR*)pbyData;
    KTEAM_MEMBER_INFO*                  pMemberInfo     = NULL;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwPartyID, pPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->nLifePercent     = pPak->byLifePercent;
    pMemberInfo->nManaPercent     = pPak->byManaPercent;

    g_PlayerServer.DoSyncTeamMemberCurrentLMRGlobal(pPak->dwPartyID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberMisc(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode    = false;
    R2S_SYNC_TEAM_MEMBER_MISC*  pPak        = (R2S_SYNC_TEAM_MEMBER_MISC*)pbyData;
    size_t                      uPakSize    = 0;
    KTEAM_MEMBER_INFO*          pMemberInfo = NULL;
    
    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwPartyID, pPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->PortraitInfo   = pPak->PortraitInfo;
    pMemberInfo->dwForceID      = pPak->dwForceID;
    pMemberInfo->eCamp          = (KCAMP)pPak->byCamp;
    pMemberInfo->nLevel         = pPak->byLevel;
	pMemberInfo->nRoleType      = pPak->byRoleType;
    pMemberInfo->bDeathFlag     = pPak->bDeathFlag;

    g_PlayerServer.DoSyncTeamMemberMisc(pPak->dwPartyID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSyncMemberPosition(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode        = false;
    R2S_SYNC_TEAM_MEMBER_POSITION*  pPak            = (R2S_SYNC_TEAM_MEMBER_POSITION*)pbyData;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;
    size_t                          uPakSize        = 0;

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPak->dwPartyID, pPak->dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    pMemberInfo->dwMapID         = pPak->dwMapID;
    pMemberInfo->nMapCopyIndex   = pPak->nMapCopyIndex;
    pMemberInfo->nPosX           = pPak->nPosX;
    pMemberInfo->nPosY           = pPak->nPosY;

    g_PlayerServer.DoSyncTeamMemberPositionGlobal(pPak->dwPartyID, pMemberInfo);

Exit0:
    return;
}

void KRelayClient::OnTeamSetMarkRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode        = false;
    R2S_TEAM_SET_MARK_RESPOND*  pPartySetMark   = (R2S_TEAM_SET_MARK_RESPOND*)pbyData;

    g_pSO3World->m_TeamServer.SetMark(pPartySetMark->dwPartyID, pPartySetMark->nMarkType, pPartySetMark->dwTarget);

    g_PlayerServer.DoTeamSetMarkRespond(pPartySetMark->dwPartyID, pPartySetMark->nMarkType, pPartySetMark->dwTarget);

Exit0:
    return;
}

void KRelayClient::OnTeamCampChange(BYTE* pbyData, size_t uDataLen)
{
    R2S_TEAM_CAMP_CHANGE* pTeamCampChange   = (R2S_TEAM_CAMP_CHANGE*)pbyData;
    KTeam*                pTeam             = NULL;

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pTeamCampChange->dwPartyID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->eCamp = (KCAMP)pTeamCampChange->byCamp;

    g_PlayerServer.DoTeamCampChange(pTeamCampChange->dwPartyID, pTeam->eCamp);

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
    R2S_ADD_FELLOWSHIP_RESPOND* pPak = (R2S_ADD_FELLOWSHIP_RESPOND*)pbyData;

    switch (pPak->byType)
    {
    case 1:
        g_pSO3World->m_FellowshipMgr.AddFellowship(pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, true);
        break;
    case 2:
        g_pSO3World->m_FellowshipMgr.AddFoe(pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, true);
        break;
    case 3:
        g_pSO3World->m_FellowshipMgr.AddBlackList(pPak->dwPlayerID, pPak->dwAlliedPlayerID, pPak->szAlliedPlayerName, true);
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

            g_pSO3World->m_FellowshipMgr.DelFellowship(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
            g_pSO3World->m_FellowshipMgr.DelFoe(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
            g_pSO3World->m_FellowshipMgr.DelBlackList(pPak->dwPlayerID, pPak->AlliedPlayerInfo[i].dwID);
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
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = 0;
    R2S_SEND_MAIL_RESPOND*  pPak        = (R2S_SEND_MAIL_RESPOND*)pbyData;
    KPlayer*                pPlayer     = NULL;
    KMAIL_GET_CONTENT_TYPE  eGetContent = mgtInvalid;
    BYTE*                   pbyMailData = NULL;
    int                     nMoney      = 0;
    KItem*                  pItem       = NULL;
    BOOL                    bAddItem    = false;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwSrcID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_SUCCESS(pPak->byResult == mrcSucceed);

    // Money
    nMoney = pPak->Content.nMoney + KMAIL_COST;
    if (nMoney)
    {
        int nPlayerMoney = 0;
        int nMoneyLimit  = 0;

        nPlayerMoney = pPlayer->m_ItemList.GetMoney();
        nMoneyLimit  = pPlayer->m_ItemList.GetMoneyLimit();

        KG_PROCESS_ERROR(nMoneyLimit - nPlayerMoney >= nMoney);

        pPlayer->m_ItemList.AddMoney(nMoney, false);
    }

    // Item
    if (pPak->Content.wItemSize)
    {
        BYTE*   pbyItemData     = pPak->Content.byTextAndItemData + pPak->Content.wTextSize;
        size_t  uItemDataSize   = pPak->Content.wItemSize;

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pbyItemData, uItemDataSize);
        KGLOG_PROCESS_ERROR(pItem);

        bRetCode = pPlayer->m_ItemList.AddItem(pItem);
        if (bRetCode != aircSuccess)
        {
            KGLogPrintf(KGLOG_ERR, "[Mail] Add item(%d, %d, %d) to player(%s) failed!",
                pItem->m_GenParam.nVersion, pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex,
                pPlayer->m_szName
            );
            goto Exit0;
        }

        pItem = NULL;
    }

Exit1:
    bResult = true;
Exit0:
    if (pPlayer)
    {
        g_PlayerServer.DoSendMailRespond(pPlayer->m_nConnIndex, pPak->byIndex, (KMAIL_RESULT_CODE)pPak->byResult);
    }

    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }

    return;
}

void KRelayClient::OnGetMailListRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bRetCode        = false;
    KPlayer*                    pPlayer         = NULL;
    R2S_GET_MAILLIST_RESPOND*   pPak            = (R2S_GET_MAILLIST_RESPOND*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    bRetCode = g_PlayerServer.DoGetMailListRespond(pPlayer->m_nConnIndex, pPak->nMailCount, pPak->MailList);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KRelayClient::OnGetMailContent(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bRetCode    = false;
    KPlayer*                pPlayer     = NULL;
    R2S_GET_MAIL_CONTENT*   pPak        = (R2S_GET_MAIL_CONTENT*)pbyData;
    KMAIL_CONTENT*          pContent    = NULL;

    if (pPak->byResult == mrcSucceed)
    {
        pContent = &(pPak->Content);
    }

    bRetCode = g_PlayerServer.DoGetMailContentRespond(pPak->dwPlayerID, pPak->dwMailID, pPak->byResult, pContent);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KRelayClient::OnGetMailAttachment(BYTE* pbyData, size_t uDataLen)
{
    int                         nRetCode    = 0;
    KPlayer*                    pPlayer     = NULL;
    R2S_GET_MAIL_ATTACHMENT*    pPak        = (R2S_GET_MAIL_ATTACHMENT*)pbyData;
    BOOL                        bGotMoney   = false;
    BOOL                        bGotText    = false;
    BOOL                        bGotItem    = false;
    KItem*                      pItem       = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pPak->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsPlaying);

    KG_PROCESS_ERROR(pPak->byResult == mrcSucceed);
    
    // Money
    if (pPak->Content.nMoney)
    {
        int nMoney          = 0;
        int nPlayerMoney    = pPlayer->m_ItemList.GetMoney();
        int nMoneyLimit     = pPlayer->m_ItemList.GetMoneyLimit();

        nMoney = pPak->Content.nMoney;

        KG_PROCESS_ERROR(nMoneyLimit - nPlayerMoney >= nMoney);

        nRetCode = pPlayer->m_ItemList.AddMoney(nMoney);
        assert(nRetCode);

        bGotMoney = true;
    }

    // Text
    if (pPak->Content.wTextSize)
    {
        char*   pText       = (char*)pPak->Content.byTextAndItemData;
        size_t  uTextSize   = pPak->Content.wTextSize;

        // Do something here. 

        bGotText = true;
    }
    
    // Item
    if (pPak->Content.wItemSize)
    {
        BYTE*   pItemData   = pPak->Content.byTextAndItemData + pPak->Content.wTextSize;
        size_t  uItemSize   = pPak->Content.wItemSize;

        pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pItemData, uItemSize);
        KGLOG_PROCESS_ERROR(pItem);

        pPak->Content.wItemSize = 0;

        nRetCode = pPlayer->m_ItemList.CanAddItem(pItem);
        if (nRetCode == aircSuccess)
        {
            // TODO: 检查物品拥有的数量限制

            nRetCode = pPlayer->m_ItemList.AddItem(pItem);
            KGLOG_PROCESS_ERROR(nRetCode == aircSuccess);
            pItem = NULL;

            bGotItem = true;
        }
    }

Exit0:
    // 通知GameCenter
    DoUpdateMailData(pPak->dwPlayerID, pPak->dwMailID, bGotMoney, bGotText, bGotItem);

    // 通知客户端结果
    g_PlayerServer.DoMailGeneralRespond(pPak->dwPlayerID, pPak->dwMailID, pPak->byResult);

    if (pPlayer)
    {
        g_PlayerServer.DoMailReleaseAttachment(pPlayer->m_nConnIndex, pPak->dwMailID, bGotMoney, bGotText, bGotItem);
    }

    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
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

    g_PlayerServer.DoNewMailNotify(pPlayer->m_nConnIndex, &(pNotify->NewMailListNode));

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
        pPak->dwPartyID, pPak->dwSrcPlayerID, pPak->nMapID, 
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

void KRelayClient::OnApplyPlayerSavedCopyRespond(BYTE* pbyData, size_t uDataLen)
{
    R2S_APPLY_PLAYER_SAVED_COPY_RESPOND* pRespond = (R2S_APPLY_PLAYER_SAVED_COPY_RESPOND*)pbyData;
    KPlayer*                             pPlayer  = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoSyncPlayerSavedCopy(pPlayer->m_nConnIndex, pRespond->wCopyCount, pRespond->byData, pRespond->uDataLen);

Exit0:
    return;
}

void KRelayClient::OnSyncRoadTrackForce(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    KROUTE_NODE*                pNode       = NULL;
    R2S_SYNC_ROAD_TRACK_FORCE*  pRoadTrack  = (R2S_SYNC_ROAD_TRACK_FORCE*)pbyData;

    pNode = g_pSO3World->m_RoadManager.GetNodeInfo(pRoadTrack->nNodeID);
    KGLOG_PROCESS_ERROR(pNode);

    pNode->dwForceID = pRoadTrack->dwForceID;

    bResult = true;
Exit0:
    return;
}

void KRelayClient::OnSaveRoleDataRespond(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode        = false;
    int                             nStateForSaving = 0;
    KPlayer*                        pPlayer         = NULL;
    R2S_SAVE_ROLE_DATA_RESPOND*     pRespond        = (R2S_SAVE_ROLE_DATA_RESPOND*)pbyData;    

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pRespond->dwPlayerID);
    if (pPlayer != NULL &&  pRespond->nUserValue == gsSaveDataForTransmission)
    {
        KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsSaveDataForTransmission);
        
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
        pPlayer->m_eGameStatus = gsTransmission;
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

    char                szFileName[100];
    FILE*               fp        = NULL;

    KGLOG_PROCESS_ERROR(pNotify->uRoleDataLen == m_uSyncRoleOffset);
    KGLOG_PROCESS_ERROR(pNotify->dwRoleID == m_dwSyncRoleID || m_dwSyncRoleID == ERROR_ID);

    snprintf(szFileName, sizeof(szFileName), "savelog/Load_Server%d", pNotify->dwRoleID);
    szFileName[sizeof(szFileName) - 1] = '\0';

    fp = fopen(szFileName, "a");
    KG_PROCESS_ERROR(fp);

    for (size_t nAddCount = 0; nAddCount < pNotify->uRoleDataLen; nAddCount++)
    {
        fprintf(fp, "%d", m_pbySyncRoleBuffer[nAddCount]);
    }

    fclose(fp);
    fp = NULL;


 //   KGLOG_PROCESS_ERROR(pNotify->uRoleDataLen == m_uSyncRoleOffset);
 //   KGLOG_PROCESS_ERROR(pNotify->dwRoleID == m_dwSyncRoleID || m_dwSyncRoleID == ERROR_ID);

 //   pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwRoleID);
 //   KGLOG_PROCESS_ERROR(pPlayer);

 //   KGLOG_PROCESS_ERROR(pNotify->bSucceed);

 //   // 注意: 由于取角色数据是异步的,所以,有可能回包到达时已经是玩家断线后的下一次登陆了
 //   KGLOG_PROCESS_ERROR(pPlayer->m_eGameStatus == gsWaitForRoleData);

 //   pScene = g_pSO3World->GetScene(pPlayer->m_SavePosition.dwMapID, pPlayer->m_SavePosition.nMapCopyIndex);
 //   KGLOG_PROCESS_ERROR(pScene);

 //   pPlayer->m_nFaceDirection   = pPlayer->m_SavePosition.nFaceDirection;

 //   bRetCode = g_pSO3World->AddPlayer(
 //       pPlayer, pScene,
 //       pPlayer->m_SavePosition.nX,
 //       pPlayer->m_SavePosition.nY,
 //       pPlayer->m_SavePosition.nZ
 //   );
 //   KGLOG_PROCESS_ERROR(bRetCode);
 //   
 //   bRetCode = pPlayer->Load(m_pbySyncRoleBuffer, pNotify->uRoleDataLen);
 //   KGLOG_PROCESS_ERROR(bRetCode);

	//g_PlayerServer.DoSyncRoleDataOver(pPlayer->m_nConnIndex);
 //   
 //   pScene->CallEnterSceneScript(pPlayer);

 //   bRetCode = pPlayer->CallLoginScript();
 //   KGLOG_PROCESS_ERROR(bRetCode);

 //   pPlayer->m_nVirtualFrame    = g_pSO3World->m_nGameLoop;
 //   pPlayer->m_nRecordCount     = 0;
 //   pPlayer->m_eGameStatus      = gsPlaying;

 //   bResult = true;
Exit0:
    m_uSyncRoleOffset = 0;
    m_dwSyncRoleID    = ERROR_ID;

    if (!bResult)
    {
        if (pPlayer)
        {
            g_PlayerServer.Detach(pPlayer->m_nConnIndex);
        }
    }
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

    bRetCode = g_pSO3World->m_CampInfo.SetCampLevel(pSyncCampInfo->nCampLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_CampInfo.SetCampScore(pSyncCampInfo->nCampScore);
    KGLOG_PROCESS_ERROR(bRetCode);
    
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

void KRelayClient::OnTongEventNotify(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                pPlayer = NULL;
    R2S_TONG_EVENT_NOTIFY*  pNotify = (R2S_TONG_EVENT_NOTIFY*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pNotify->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    g_PlayerServer.DoMessageNotify(
        pPlayer->m_nConnIndex, etcTongEventNotifyCode, pNotify->byCode, 
        pNotify->byData, uDataLen - sizeof(R2S_TONG_EVENT_NOTIFY)
    );
Exit0:
    return;
}

void KRelayClient::OnBroadcastTongMessage(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                    pPlayer     = NULL;
    R2S_TONG_BROADCAST_MESSAGE* pMessage    = (R2S_TONG_BROADCAST_MESSAGE*)pbyData;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pMessage->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

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
    KGLOG_PROCESS_ERROR(pPlayer);
    
    pRespond->szTongName[sizeof(pRespond->szTongName) - 1] = '\0';

    g_PlayerServer.DoGetTongDescriptionRespond(pPlayer->m_nConnIndex, pRespond->dwTongID, pRespond->szTongName);
Exit0:
    return;
}

void KRelayClient::OnSyncTongOnlineMessage(BYTE* pbyData, size_t uDataLen)
{
    KPlayer*                        pPlayer = NULL;
    R2S_SYNC_TONG_ONLINE_MESSAGE*   pSync   = (R2S_SYNC_TONG_ONLINE_MESSAGE*)pbyData;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(pSync->dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(R2S_SYNC_TONG_ONLINE_MESSAGE) + (size_t)pSync->byDataLen);

    g_PlayerServer.DoSyncTongOnlineMessage(pPlayer->m_nConnIndex, pSync->szOnlineMessage, (size_t)pSync->byDataLen);
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
    }

    g_PlayerServer.DoAuctionBidRespond(pRespond->dwPlayerID, pRespond->byCode);

Exit0:
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


}

void KRelayClient::OnSyncGlobalSystemValue(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bRetCode    = false;
    R2S_SYNC_GLOBAL_SYSTEM_VALUE*   pPak        = (R2S_SYNC_GLOBAL_SYSTEM_VALUE*)pbyData;

    g_pSO3World->m_GlobalSystemValueCache.m_nGameWorldStartTime = pPak->nGameWorldStartTime;
    g_pSO3World->m_GlobalSystemValueCache.m_nMaxPlayerLevel     = pPak->nMaxPlayerLevel;

    return;
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

	pHandshake->wProtocolID            = s2r_handshake_request;
    pHandshake->nGameWorldLowerVersion = GAME_WORLD_LOWEST_VERSION;
    pHandshake->nGameWorldUpperVersion = GAME_WORLD_CURRENT_VERSION;

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
    
    pRequest->wProtocolID       = s2r_transfer_player_request;
    pRequest->dwRoleID          = pPlayer->m_dwID;
    pRequest->ExtPointInfo      = pPlayer->m_ExtPointInfo;
    pRequest->nlLeftTime        = pPlayer->m_nlLeftTime;
    pRequest->nBattleFieldSide  = pPlayer->m_nBattleFieldSide;

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

BOOL KRelayClient::DoTeamDisbandRequest(DWORD dwLeaderID, DWORD dwPartyID)
{
    BOOL                       bResult              = false;
    BOOL                       bRetCode             = false;
    S2R_TEAM_DISBAND_REQUEST*  pPartyDisbandRequest = NULL;
    IKG_Buffer*                piSendBuffer         = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_DISBAND_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPartyDisbandRequest = (S2R_TEAM_DISBAND_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPartyDisbandRequest);

    pPartyDisbandRequest->wProtocolID      = s2r_team_disband_request;
    pPartyDisbandRequest->dwLeaderID       = dwLeaderID;
    pPartyDisbandRequest->dwPartyID        = dwPartyID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamDelMemberRequest(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwPlayerID)
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
    pDelMemberRequest->dwPartyID        = dwPartyID;
    pDelMemberRequest->dwRequestSrc     = dwRequestSrc;
    pDelMemberRequest->dwMemberID       = dwPlayerID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamChangeAuthorityRequest(DWORD dwPartyID, DWORD dwRequestSrc, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
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
    pChangeLeaderRequest->dwPartyID        = dwPartyID;
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

BOOL KRelayClient::DoTeamSetLootModeRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nLootMode)
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
    pRequest->dwPartyID     = dwPartyID;
    pRequest->dwRequestSrc  = dwRequestSrc;
    pRequest->nLootMode     = nLootMode;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSetRollQualityRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nRollQuality)
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
	pRequest->dwPartyID        = dwPartyID;
	pRequest->dwRequestSrc     = dwRequestSrc;
	pRequest->nRollQuality	   = nRollQuality;

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoTeamSetFormationLeader(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwNewFormationLeaderID, int nGroupIndex)
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
	pRequest->dwPartyID             = dwPartyID;
	pRequest->dwRequestSrc          = dwRequestSrc;
	pRequest->dwNewFormationLeader	= dwNewFormationLeaderID;
    pRequest->nGourpIndex           = nGroupIndex;

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

BOOL KRelayClient::DoTeamSyncMemberMaxLMR(KPlayer* pPlayer)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    KTeam*                          pTeam           = NULL;
    KTEAM_MEMBER_INFO*              pMemberInfo     = NULL;
    BOOL                            bValueChanged   = false;
    IKG_Buffer*                     piSendBuffer    = NULL;
    S2R_SYNC_TEAM_MEMBER_MAX_LMR*   pPak            = NULL;

    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != INVALID_PARTY_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPlayer->m_dwTeamID, pPlayer->m_dwID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    bValueChanged |= (pMemberInfo->nMaxLife != pPlayer->m_nMaxLife);
    bValueChanged |= (pMemberInfo->nMaxMana != pPlayer->m_nMaxMana);

    KG_PROCESS_ERROR(bValueChanged);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_MAX_LMR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_MAX_LMR*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pMemberInfo->nMaxLife = pPlayer->m_nMaxLife;
    pMemberInfo->nMaxMana = pPlayer->m_nMaxMana;

    pPak->wProtocolID      = s2r_sync_team_member_max_lmr;
    pPak->dwPartyID        = pPlayer->m_dwTeamID;
    pPak->dwMemberID       = pPlayer->m_dwID;
    pPak->nMaxLife         = pPlayer->m_nMaxLife;
    pPak->nMaxMana         = pPlayer->m_nMaxMana;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

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
    BOOL                                bValueChanged   = false;
    IKG_Buffer*                         piSendBuffer    = NULL;
    S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*   pPak            = NULL;

    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != INVALID_PARTY_ID);

    if (pPlayer->m_nMaxLife > 0)
        nNewLifePercent = pPlayer->m_nCurrentLife * UCHAR_MAX / pPlayer->m_nMaxLife;

    if (pPlayer->m_nMaxMana > 0)
        nNewManaPercent = pPlayer->m_nCurrentMana * UCHAR_MAX / pPlayer->m_nMaxMana;

    bValueChanged |= (nNewLifePercent != pPlayer->m_nLastSyncGlobalPartyLifePercent);
    bValueChanged |= (nNewManaPercent != pPlayer->m_nLastSyncGlobalPartyManaPercent);

    KG_PROCESS_ERROR(bValueChanged);

    pPlayer->m_nLastSyncGlobalPartyLifePercent = nNewLifePercent;
    pPlayer->m_nLastSyncGlobalPartyManaPercent = nNewManaPercent;

    // 全局同步覆盖局部同步数值
    pPlayer->m_nLastSyncLocalPartyLifePercent = nNewLifePercent;
    pPlayer->m_nLastSyncLocalPartyManaPercent = nNewManaPercent;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_CURRENT_LMR));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID      = s2r_sync_team_member_current_lmr;
    pPak->dwPartyID        = pPlayer->m_dwTeamID;
    pPak->dwMemberID       = pPlayer->m_dwID;
    pPak->byLifePercent    = (BYTE)nNewLifePercent;
    pPak->byManaPercent    = (BYTE)nNewManaPercent;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoPartySyncMemberMisc(KPlayer* pPlayer)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KTEAM_MEMBER_INFO*          pMemberInfo     = NULL;
    BOOL                        bValueChanged   = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    S2R_SYNC_TEAM_MEMBER_MISC*  pPak            = NULL;
    KPORTRAIT_INFO              PortraitInfo;

    KG_PROCESS_ERROR(pPlayer->m_pScene);

    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != INVALID_PARTY_ID);

    pMemberInfo = g_pSO3World->m_TeamServer.GetMember(pPlayer->m_dwTeamID, pPlayer->m_dwID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    PortraitInfo.dwFaceStyle     = pPlayer->m_wRepresentId[perFaceStyle];
    PortraitInfo.dwHelmStyle     = pPlayer->m_wRepresentId[perHelmStyle];
    PortraitInfo.dwHelmColor     = pPlayer->m_wRepresentId[perHelmColor];
    PortraitInfo.dwHairStyle     = pPlayer->m_wRepresentId[perHairStyle];
    PortraitInfo.dwReserved      = 0;

    bValueChanged |= (pMemberInfo->PortraitInfo != PortraitInfo);
    bValueChanged |= (pMemberInfo->dwForceID != pPlayer->m_dwForceID);
    bValueChanged |= (pMemberInfo->eCamp != pPlayer->m_eCamp);
    bValueChanged |= (pMemberInfo->nLevel != pPlayer->m_nLevel);
	bValueChanged |= (pMemberInfo->nRoleType != pPlayer->m_eRoleType);
    bValueChanged |= (pMemberInfo->bDeathFlag != (pPlayer->m_eMoveState == cmsOnDeath));

    KG_PROCESS_ERROR(bValueChanged);

    pMemberInfo->PortraitInfo   = PortraitInfo;
    pMemberInfo->dwForceID      = pPlayer->m_dwForceID;
    pMemberInfo->nLevel         = pPlayer->m_nLevel;
    pMemberInfo->eCamp          = pPlayer->m_eCamp;
	pMemberInfo->nRoleType      = pPlayer->m_eRoleType;
    pMemberInfo->bDeathFlag     = (pPlayer->m_eMoveState == cmsOnDeath);

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_MISC));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_MISC*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID      = s2r_sync_team_member_misc;
    pPak->dwPartyID        = pPlayer->m_dwTeamID;
    pPak->dwMemberID       = pPlayer->m_dwID;
    
    pPak->PortraitInfo     = PortraitInfo;
    pPak->dwForceID        = pPlayer->m_dwForceID;
    pPak->byCamp           = (BYTE)pPlayer->m_eCamp;
    pPak->byLevel          = (BYTE)pPlayer->m_nLevel;
	pPak->byRoleType       = (BYTE)pPlayer->m_eRoleType;
    pPak->bDeathFlag       = (pPlayer->m_eMoveState == cmsOnDeath);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

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
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != INVALID_PARTY_ID);

    bValueChanged |= (pPlayer->m_pScene->m_dwMapID != pPlayer->m_dwLastSyncGlobalPartyMemberMapID);
    bValueChanged |= (pPlayer->m_pScene->m_nCopyIndex != pPlayer->m_nLastSyncGlobalPartyMemberMapCopyIndex);
    bValueChanged |= (pPlayer->m_nX != pPlayer->m_nLastSyncGlobalPartyMemberPosX);
    bValueChanged |= (pPlayer->m_nY != pPlayer->m_nLastSyncGlobalPartyMemberPosY);

    KG_PROCESS_ERROR(bValueChanged);

    pPlayer->m_dwLastSyncGlobalPartyMemberMapID         = pPlayer->m_pScene->m_dwMapID;
    pPlayer->m_nLastSyncGlobalPartyMemberMapCopyIndex   = pPlayer->m_pScene->m_nCopyIndex;
    pPlayer->m_nLastSyncGlobalPartyMemberPosX           = pPlayer->m_nX;
    pPlayer->m_nLastSyncGlobalPartyMemberPosY           = pPlayer->m_nY;

    // 全局同步时覆盖局部同步数值
    pPlayer->m_dwLastSyncLocalPartyMemberMapID         = pPlayer->m_pScene->m_dwMapID;
    pPlayer->m_nLastSyncLocalPartyMemberMapCopyIndex   = pPlayer->m_pScene->m_nCopyIndex;
    pPlayer->m_nLastSyncLocalPartyMemberPosX           = pPlayer->m_nX;
    pPlayer->m_nLastSyncLocalPartyMemberPosY           = pPlayer->m_nY;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_POSITION));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SYNC_TEAM_MEMBER_POSITION*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID        = s2r_sync_team_member_position;
	pPak->dwPartyID			 = pPlayer->m_dwTeamID;
    pPak->dwMemberID         = pPlayer->m_dwID;
    pPak->dwMapID            = pPlayer->m_pScene->m_dwMapID;
    pPak->nMapCopyIndex      = pPlayer->m_pScene->m_nCopyIndex;
    pPak->nPosX              = pPlayer->m_nX;
    pPak->nPosY              = pPlayer->m_nY;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoTeamSetMarkRequest(DWORD dwPartyID, DWORD dwPlayerID, int nMarkType, DWORD dwTargetID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuff      = NULL;
    S2R_TEAM_SET_MARK_REQUEST*  pPartySetMark   = NULL;

    piSendBuff = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_MARK_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuff);

    pPartySetMark = (S2R_TEAM_SET_MARK_REQUEST*)piSendBuff->GetData();
    KGLOG_PROCESS_ERROR(pPartySetMark);

    pPartySetMark->wProtocolID = s2r_team_set_mark_request;
    pPartySetMark->dwPartyID   = dwPartyID;
    pPartySetMark->dwMemberID  = dwPlayerID;
    pPartySetMark->nMarkType   = nMarkType;
    pPartySetMark->dwTargetID  = dwTargetID;

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

BOOL KRelayClient::DoSendMailRequest(
    int nRequestID, const char cszSrcName[], const char cszDstName[], const char cszTitle[], int nDelayTime,
    BOOL bSystemMail, int nMoney, const char cszText[], BYTE* pbyItemData, size_t uItemDataLen
)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_SEND_MAIL_REQUEST*  pPak            = NULL;
    size_t                  uTextLen        = 0;

    assert(cszSrcName);
    assert(cszDstName);
    assert(cszTitle);
    assert(cszText);
    assert(pbyItemData);

    uTextLen = strlen(cszText) + 1;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SEND_MAIL_REQUEST) + uTextLen + uItemDataLen);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SEND_MAIL_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_send_mail_request;
    pPak->byRequestID       = (BYTE)nRequestID;

    strncpy(pPak->szSenderName, cszSrcName, sizeof(pPak->szSenderName));
    pPak->szSenderName[sizeof(pPak->szSenderName) - 1] = '\0';

    strncpy(pPak->szReceiverName, cszDstName, sizeof(pPak->szReceiverName));
    pPak->szReceiverName[sizeof(pPak->szReceiverName) - 1] = '\0';

    strncpy(pPak->szTitle, cszTitle, sizeof(pPak->szTitle));
    pPak->szTitle[sizeof(pPak->szTitle) - 1] = '\0';

    pPak->nDelaySecond = nDelayTime;
    pPak->bSystemMail  = (uint8_t)bSystemMail;

    pPak->Content.nMoney    = nMoney;

    pPak->Content.wTextSize = (WORD)uTextLen;
    memcpy(pPak->Content.byTextAndItemData, cszText, uTextLen);

    pPak->Content.wItemSize = (WORD)uItemDataLen;
    memcpy(pPak->Content.byTextAndItemData + uTextLen, pbyItemData, uItemDataLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoSendGlobalMailRequest(
    const char sczSenderName[], const char cszTitle[], int nLifeTime,
    int nMoney, const char cszText[], BYTE* pbyItemData, size_t uItemDataLen
)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    IKG_Buffer*                     piSendBuffer    = NULL;
    S2R_SEND_GLOBAL_MAIL_REQUEST*   pPak            = NULL;
    size_t                          uTextLen        = 0;

    assert(cszTitle);
    assert(cszText);
    assert(pbyItemData);

    uTextLen = strlen(cszText) + 1;

    piSendBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_SEND_GLOBAL_MAIL_REQUEST) + uTextLen + uItemDataLen);
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_SEND_GLOBAL_MAIL_REQUEST*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_send_global_mail_request;

    strncpy(pPak->szSenderName, sczSenderName, sizeof(pPak->szSenderName));
    pPak->szSenderName[sizeof(pPak->szSenderName) - 1] = '\0';

    strncpy(pPak->szTitle, cszTitle, sizeof(pPak->szTitle));
    pPak->szTitle[sizeof(pPak->szTitle) - 1] = '\0';

    pPak->nLifeTime 		= nLifeTime;

    pPak->Content.nMoney    = nMoney;

    pPak->Content.wTextSize = (WORD)uTextLen;
    memcpy(pPak->Content.byTextAndItemData, cszText, uTextLen);

    pPak->Content.wItemSize = (WORD)uItemDataLen;
    memcpy(pPak->Content.byTextAndItemData + uTextLen, pbyItemData, uItemDataLen);

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetMailListRequest(DWORD dwPlayerID)
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

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetMailContent(DWORD dwPlayerID, DWORD dwMailID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_GET_MAIL_CONTENT*   pPak            = NULL;
    
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_GET_MAIL_CONTENT));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_GET_MAIL_CONTENT*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_get_mail_content;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwMailID          = dwMailID;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoGetMailAttachment(DWORD dwPlayerID, DWORD dwMailID, BOOL bGetMoney, BOOL bGetText, BOOL bGetItem)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piSendBuffer    = NULL;
    S2R_GET_MAIL_ATTACHMENT*    pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_GET_MAIL_ATTACHMENT));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_GET_MAIL_ATTACHMENT*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_get_mail_attachment;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwMailID          = dwMailID;
    pPak->bMoneyFlag        = bGetMoney;
    pPak->bTextFlag         = bGetText;
    pPak->bItemFlag         = bGetItem;
    pPak->bReserve          = 0;

    bRetCode = Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piSendBuffer);
    return bResult;
}

BOOL KRelayClient::DoUpdateMailData(DWORD dwPlayerID, DWORD dwMailID, BOOL bMoneyFlag, BOOL bTextFlag, BOOL bItemFlag)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    IKG_Buffer*             piSendBuffer    = NULL;
    S2R_UPDATE_MAIL_DATA*   pPak            = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_UPDATE_MAIL_DATA));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pPak = (S2R_UPDATE_MAIL_DATA*)piSendBuffer->GetData();
    KGLOG_PROCESS_ERROR(pPak);

    pPak->wProtocolID       = s2r_update_mail_data;
    pPak->dwPlayerID        = dwPlayerID;
    pPak->dwMailID          = dwMailID;
    pPak->bMoneyFlag        = bMoneyFlag;
    pPak->bTextFlag         = bTextFlag;
    pPak->bItemFlag         = bItemFlag;
    pPak->bReserve          = 0;

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

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SET_MAIL_READ));
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

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_DELETE_MAIL));
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

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_RETURN_MAIL));
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
	KG_PROCESS_ERROR(pSrc->m_dwTeamID != INVALID_PARTY_ID);

	pPak->wProtocolID = s2r_sync_mid_map_mark;
	pPak->dwPartyID = pSrc->m_dwTeamID;
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

	bRetCode = Send(piSendBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

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

BOOL KRelayClient::DoResetMapCopyRequest(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piPackage       = NULL;
    S2R_RESET_MAP_COPY_REQUEST* pRequest        = NULL;
    
    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_RESET_MAP_COPY_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

    pRequest                = (S2R_RESET_MAP_COPY_REQUEST*)piPackage->GetData();
    pRequest->wProtocolID   = s2r_reset_map_copy_request;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwMapID       = dwMapID;
    pRequest->nMapCopyIndex = nMapCopyIndex;

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

BOOL KRelayClient::DoApplyPlayerSavedCopyRequest(DWORD dwPlayerID)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    IKG_Buffer*                             piPackage   = NULL;
    S2R_APPLY_PLAYER_SAVED_COPY_REQUEST*    pRequest    = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_PLAYER_SAVED_COPY_REQUEST));
    KGLOG_PROCESS_ERROR(piPackage);

    pRequest = (S2R_APPLY_PLAYER_SAVED_COPY_REQUEST*)piPackage->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_apply_player_saved_copy_request;
    pRequest->dwPlayerID    = dwPlayerID;

    bRetCode = Send(piPackage);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piPackage);
    return bResult;
}

BOOL KRelayClient::DoSyncRoadTrackForce(int nNodeID, DWORD dwForceID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piPackage       = NULL;
    S2R_SYNC_ROAD_TRACK_FORCE*  pRoadTrackForce = NULL;

    piPackage = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_ROAD_TRACK_FORCE));
	KGLOG_PROCESS_ERROR(piPackage);

    pRoadTrackForce               = (S2R_SYNC_ROAD_TRACK_FORCE*)piPackage->GetData();
    pRoadTrackForce->wProtocolID  = s2r_sync_road_track_force;
    pRoadTrackForce->nNodeID      = nNodeID;
    pRoadTrackForce->dwForceID    = dwForceID;

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

    pSaveRoleData =(S2R_SAVE_ROLE_DATA*)piPackage->GetData();
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
    S2R_APPLY_TONG_INFO_REQUEST*    pApplyRequest    = NULL;

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

BOOL KRelayClient::DoInvitePlayerJoinTongRespond(DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, BOOL bAccept)
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
    pRespond->byAccept      = (BYTE)bAccept;

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

BOOL KRelayClient::DoModifyTongScheamRequest(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byModifyType, BYTE* pbyData, size_t uDataLen)
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

BOOL KRelayClient::DoApplyQuiteTongRequest(DWORD dwPlayerID)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    S2R_APPLY_QUITE_TONG_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_QUITE_TONG_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_APPLY_QUITE_TONG_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID = s2r_apply_quite_tong_request;
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
    KGLOG_PROCESS_ERROR(piBuffer);
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
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    S2R_REMOTE_LUA_CALL*        pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_REMOTE_LUA_CALL) + uParamLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (S2R_REMOTE_LUA_CALL*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->wProtocolID   = s2r_remote_lua_call;
    pRequest->uParamLen     = uParamLen;

    strncpy(pRequest->szFunction, cszFunction, sizeof(pRequest->szFunction));
    pRequest->szFunction[sizeof(pRequest->szFunction) - 1] = '\0';

    memcpy(pRequest->byParam, pbyParam, uParamLen);

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KRelayClient::DoAchievementGlobalAnnounceRequest(DWORD dwPlayerID, DWORD dwAchievementID)
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
    pRequest->dwAchievementID   = dwAchievementID;

    bRetCode = Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

//AutoCode:-发送协议函数结束-
#endif	//_SERVER
