#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include "KMath.h"
#include "KPlayerClient.h"

#include "Engine/KG_Time.h"
#include "Common/CRC32.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
//#include "KShop.h"
//#include "KShopCenter.h"
//#include "KItemLib.h"
#include "Global.h"
#include "SO3Result.h"
//#include "KSkillManager.h"
#include "KRegion.h"
#include "KScene.h"
#include "KMonster.h"

#define MAX_PACKAGE_COUNT				10240				//缓冲的最大协议数目
#define FORCE_SYNC_RANGE				((CELL_LENGTH) * 8)	//强制同步范围，单位：象素点
#define PLAYER_DISPLAY_DATA_MAX_CACHE	512
#define NPC_DISPLAY_DATA_MAX_CACHE		512

char KPlayerClient::ms_szStateDecription[gmsTotal][265] = 
{
    "Invalid",
    "GS",
    "无法连接Gameserver",
    "同步角色数据出错",
    "Gameserver断开"
};

KPlayerClient::KPlayerClient(void)
{
    m_pOwner = NULL;

    m_piSocketStream = NULL;

	m_nGSGameLoop = 0;

	memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
	memset(m_nProtocolSize, UNUSED_PROTOCOL_ID, sizeof(m_nProtocolSize));

	REGISTER_EXTERNAL_FUNC(g2c_response_message, &KPlayerClient::OnResponseMessage, sizeof(G2C_RESPONSE_MESSAGE));
	REGISTER_EXTERNAL_FUNC(g2c_sync_player_base_info, &KPlayerClient::OnSyncPlayerBaseInfo, sizeof(G2C_SYNC_PLAYER_BASE_INFO));
	REGISTER_EXTERNAL_FUNC(g2c_sync_quest_state, &KPlayerClient::OnSyncQuestState, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_sync_quest_list, &KPlayerClient::OnSyncQuestList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_kick_self_out, &KPlayerClient::OnKickSelfOut, sizeof(G2C_KICK_SELF_OUT));
	REGISTER_EXTERNAL_FUNC(g2c_change_game_server, &KPlayerClient::OnChangeGameServer, sizeof(G2C_CHANGE_GAME_SERVER));

    REGISTER_EXTERNAL_FUNC(g2c_sync_region_player, &KPlayerClient::OnSyncRegionPlayer, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_sync_region_npc, &KPlayerClient::OnSyncRegionNpc, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_sync_new_player, &KPlayerClient::OnSyncNewPlayer, sizeof(G2C_SYNC_NEW_PLAYER));
    REGISTER_EXTERNAL_FUNC(g2c_sync_new_npc, &KPlayerClient::OnSyncNewNpc, sizeof(G2C_SYNC_NEW_NPC));

	REGISTER_EXTERNAL_FUNC(g2c_move_character, &KPlayerClient::OnMoveCharacter, sizeof(G2C_MOVE_CHARACTER));
	REGISTER_EXTERNAL_FUNC(g2c_remove_character, &KPlayerClient::OnRemoveCharacter, sizeof(G2C_REMOVE_CHARACTER));
	REGISTER_EXTERNAL_FUNC(g2c_character_action, &KPlayerClient::OnCharacterAction, sizeof(G2C_CHARACTER_ACTION));

	REGISTER_EXTERNAL_FUNC(g2c_character_say, &KPlayerClient::OnCharacterSay, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_over_time_action, &KPlayerClient::OnOverTimeAction, sizeof(G2C_OVER_TIME_ACTION));

    REGISTER_EXTERNAL_FUNC(g2c_revise_move_state, &KPlayerClient::OnReviseMoveState, sizeof(G2C_REVISE_MOVE_STATE));
    REGISTER_EXTERNAL_FUNC(g2c_sync_move_state, &KPlayerClient::OnSyncMoveState, sizeof(G2C_SYNC_MOVE_STATE));

    REGISTER_EXTERNAL_FUNC(g2c_character_stand, &KPlayerClient::OnCharacterStand, sizeof(G2C_CHARACTER_STAND));
    REGISTER_EXTERNAL_FUNC(g2c_character_walk, &KPlayerClient::OnCharacterWalk, sizeof(G2C_CHARACTER_WALK));
	REGISTER_EXTERNAL_FUNC(g2c_character_run, &KPlayerClient::OnCharacterRun, sizeof(G2C_CHARACTER_RUN));
    REGISTER_EXTERNAL_FUNC(g2c_character_swim, &KPlayerClient::OnCharacterSwim, sizeof(G2C_CHARACTER_SWIM));
	REGISTER_EXTERNAL_FUNC(g2c_character_jump, &KPlayerClient::OnCharacterJump, sizeof(G2C_CHARACTER_JUMP));
    REGISTER_EXTERNAL_FUNC(g2c_character_sit, &KPlayerClient::OnCharacterSit, sizeof(G2C_CHARACTER_SIT));
    REGISTER_EXTERNAL_FUNC(g2c_character_meditate, &KPlayerClient::OnCharacterMeditate, sizeof(G2C_CHARACTER_MEDITATE));

	REGISTER_EXTERNAL_FUNC(g2c_character_dash, &KPlayerClient::OnCharacterDash, sizeof(G2C_CHARACTER_DASH));
	REGISTER_EXTERNAL_FUNC(g2c_character_knocked_down, &KPlayerClient::OnCharacterKnockedDown, sizeof(G2C_CHARACTER_KNOCKED_DOWN));
	REGISTER_EXTERNAL_FUNC(g2c_character_knocked_back, &KPlayerClient::OnCharacterKnockedBack, sizeof(G2C_CHARACTER_KNOCKED_BACK));
	REGISTER_EXTERNAL_FUNC(g2c_character_knocked_off, &KPlayerClient::OnCharacterKnockedOff, sizeof(G2C_CHARACTER_KNOCKED_OFF));
    REGISTER_EXTERNAL_FUNC(g2c_character_stun, &KPlayerClient::OnCharacterStun, sizeof(G2C_CHARACTER_STUN));

	REGISTER_EXTERNAL_FUNC(g2c_character_death, &KPlayerClient::OnCharacterDeath, sizeof(G2C_CHARACTER_DEATH));
    REGISTER_EXTERNAL_FUNC(g2c_player_revived_notify, &KPlayerClient::OnPlayerRevivedNotify, sizeof(G2C_PLAYER_REVIVED_NOTIFY));
	REGISTER_EXTERNAL_FUNC(g2c_character_turn, &KPlayerClient::OnCharacterTurn, sizeof(G2C_CHARACTER_TURN));

	REGISTER_EXTERNAL_FUNC(g2c_sync_doodad_state, &KPlayerClient::OnSyncDoodadState, sizeof(G2C_SYNC_DOODAD_STATE));

    REGISTER_EXTERNAL_FUNC(g2c_sync_represent_state, &KPlayerClient::OnSyncRepresentState, sizeof(G2C_SYNC_REPRESENT_STATE));

	REGISTER_EXTERNAL_FUNC(g2c_player_display_data, &KPlayerClient::OnPlayerDisplayData, sizeof(G2C_PLAYER_DISPLAY_DATA));
	REGISTER_EXTERNAL_FUNC(g2c_npc_display_data, &KPlayerClient::OnNpcDisplayData, sizeof(G2C_NPC_DISPLAY_DATA));
	REGISTER_EXTERNAL_FUNC(g2c_add_experience, &KPlayerClient::OnAddExperience, sizeof(G2C_ADD_EXPERIENCE));
    REGISTER_EXTERNAL_FUNC(g2c_common_health, &KPlayerClient::OnCommonHealth, sizeof(G2C_COMMON_HEALTH));
    

	REGISTER_EXTERNAL_FUNC(g2c_frame_signal, &KPlayerClient::OnFrameSignal, sizeof(G2C_FRAME_SIGNAL));
    REGISTER_EXTERNAL_FUNC(g2c_terrain_test, &KPlayerClient::OnTerrainTest, UNDEFINED_PROTOCOL_SIZE);
    

    // ------------------------------ skill about -------------------------------------------->

	REGISTER_EXTERNAL_FUNC(g2c_sync_skill_data, &KPlayerClient::OnSyncSkillData, sizeof(G2C_SYNC_SKILL_DATA));

	REGISTER_EXTERNAL_FUNC(g2c_skill_prepare, &KPlayerClient::OnSkillPrepare, sizeof(G2C_SKILL_PREPARE));
    REGISTER_EXTERNAL_FUNC(g2c_skill_cast, &KPlayerClient::OnSkillCast, sizeof(G2C_SKILL_CAST));
    REGISTER_EXTERNAL_FUNC(g2c_sync_common_skill_auto_cast_state, &KPlayerClient::OnSyncCommonSkillAutoCastState, sizeof(G2C_SYNC_COMMON_SKILL_AUTO_CAST_STATE));
    REGISTER_EXTERNAL_FUNC(g2c_reset_skill_cooldown, &KPlayerClient::OnResetSkillCooldown, sizeof(G2C_RESET_SKILL_COOLDOWN));
    
	REGISTER_EXTERNAL_FUNC(g2c_skill_effect_result, &KPlayerClient::OnSkillEffectResult, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_sync_behit_represent, &KPlayerClient::OnSyncBehitRepresent, sizeof(G2C_SYNC_BEHIT_REPRESENT));
    REGISTER_EXTERNAL_FUNC(g2c_sync_fight_state, &KPlayerClient::OnSyncFightState, sizeof(G2C_SYNC_FIGHT_STATE));
	REGISTER_EXTERNAL_FUNC(g2c_sync_buff_list, &KPlayerClient::OnSyncBuffList, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_sync_buff_single, &KPlayerClient::OnSyncBuffSingle, sizeof(G2C_SYNC_BUFF_SINGLE));
    REGISTER_EXTERNAL_FUNC(g2c_sync_kungfu_mount, &KPlayerClient::OnSyncKungfuMount, sizeof(G2C_SYNC_KUNGFU_MOUNT));
    REGISTER_EXTERNAL_FUNC(g2c_skill_beat_back, &KPlayerClient::OnSkillBeatBack, sizeof(G2C_SKILL_BEAT_BACK));
    REGISTER_EXTERNAL_FUNC(g2c_sync_sheath_state, &KPlayerClient::OnSyncSheathState, sizeof(G2C_SYNC_SHEATH_STATE));

    // -------------------------------- 数值同步 -----------------------------------------------
    REGISTER_EXTERNAL_FUNC(g2c_sync_self_max_lmrs, &KPlayerClient::OnSyncSelfMaxLMRS, sizeof(G2C_SYNC_SELF_MAX_LMRS));
    REGISTER_EXTERNAL_FUNC(g2c_sync_self_current_lmrs, &KPlayerClient::OnSyncSelfCurrentLMRS, sizeof(G2C_SYNC_SELF_CURRENT_LMRS));
    REGISTER_EXTERNAL_FUNC(g2c_sync_target_max_lmr, &KPlayerClient::OnSyncTargetMaxLMR, sizeof(G2C_SYNC_TARGET_MAX_LMR));
    REGISTER_EXTERNAL_FUNC(g2c_sync_target_current_lmr, &KPlayerClient::OnSyncTargetCurrentLMR, sizeof(G2C_SYNC_TARGET_CURRENT_LMR));
    REGISTER_EXTERNAL_FUNC(g2c_sync_party_member_max_lmr, &KPlayerClient::OnSyncPartyMemberMaxLMR, sizeof(G2C_SYNC_PARTY_MEMBER_MAX_LMR));
    REGISTER_EXTERNAL_FUNC(g2c_sync_party_member_current_lmr, &KPlayerClient::OnSyncPartyMemberCurrentLMR, sizeof(G2C_SYNC_PARTY_MEMBER_CURRENT_LMR));
    REGISTER_EXTERNAL_FUNC(g2c_sync_party_member_misc, &KPlayerClient::OnSyncPartyMemberMisc, sizeof(G2C_SYNC_PARTY_MEMBER_MISC));
    REGISTER_EXTERNAL_FUNC(g2c_sync_party_member_coordinate, &KPlayerClient::OnSyncPartyMemberCoordinate, sizeof(G2C_SYNC_PARTY_MEMBER_COORDINATE));
	REGISTER_EXTERNAL_FUNC(g2c_broadcast_character_life, &KPlayerClient::OnBroadcastCharacterLife, sizeof(G2C_BROADCAST_CHARACTER_LIFE));

    // ------------------------------ 组队相关 --------------------------------------------

    REGISTER_EXTERNAL_FUNC(g2c_sync_player_party_base_info, &KPlayerClient::OnSyncPlayerPartyBaseInfo, sizeof(G2C_SYNC_PLAYER_PARTY_BASE_INFO));
    REGISTER_EXTERNAL_FUNC(g2c_sync_player_party_member_info, &KPlayerClient::OnSyncPlayerPartyMemberInfo, sizeof(G2C_SYNC_PLAYER_PARTY_MEMBER_INFO));
    REGISTER_EXTERNAL_FUNC(g2c_invite_join_party_request, &KPlayerClient::OnInvitePlayerJoinPartyRequest, sizeof(G2C_INVITE_JOIN_PARTY_REQUEST));
    REGISTER_EXTERNAL_FUNC(g2c_invite_join_party_respond, &KPlayerClient::OnInvitePlayerJoinPartyRespond, sizeof(G2C_INVITE_JOIN_PARTY_RESPOND));
    REGISTER_EXTERNAL_FUNC(g2c_apply_join_party_request, &KPlayerClient::OnApplyJoinPartyRequest, sizeof(G2C_APPLY_JOIN_PARTY_REQUEST));
    REGISTER_EXTERNAL_FUNC(g2c_apply_join_party_respond, &KPlayerClient::OnApplyJoinPartyRespond, sizeof(G2C_APPLY_JOIN_PARTY_RESPOND));
    REGISTER_EXTERNAL_FUNC(g2c_party_create_notify, &KPlayerClient::OnPartyCreateNotify, sizeof(G2C_PARTY_CREATE_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_player_join_party_notify, &KPlayerClient::OnPlayerJoinPartyNotify, sizeof(G2C_PLAYER_JOIN_PARTY_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_party_add_member_notify, &KPlayerClient::OnPartyAddMemberNotify, sizeof(G2C_PARTY_ADD_MEMBER_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_party_del_member_notify, &KPlayerClient::OnPartyDelMemberNotify, sizeof(G2C_PARTY_DEL_MEMBER_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_sync_party_leader_flag, &KPlayerClient::OnSyncPartyLeaderFlag, sizeof(G2C_SYNC_PARTY_LEADER_FLAG));
    REGISTER_EXTERNAL_FUNC(g2c_party_change_leader_notify, &KPlayerClient::OnPartyChangeLeaderNotify, sizeof(G2C_PARTY_CHANGE_LEADER_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_party_set_distribute_mode_notify, &KPlayerClient::OnPartySetLootModeNotify, sizeof(G2C_PARTY_SET_LOOT_MODE_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_party_disband_notify, &KPlayerClient::OnPartyDisbandNotify, sizeof(G2C_PARTY_DISBAND_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_sync_party_member_online_flag, &KPlayerClient::OnSyncPartyMemberOnlineFlag, sizeof(G2C_SYNC_PARTY_MEMBER_ONLINE_FLAG));
    REGISTER_EXTERNAL_FUNC(g2c_party_member_signpost_notify, &KPlayerClient::OnPartyMemberSignpostNotify, sizeof(G2C_PARTY_MEMBER_SIGNPOST_NOTIFY));

    REGISTER_EXTERNAL_FUNC(g2c_open_shop_notify, &KPlayerClient::OnOpenShopNotify, sizeof(G2C_OPEN_SHOP_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_update_shop_item, &KPlayerClient::OnUpdateShopItem, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_shop_buy_respond, &KPlayerClient::OnShopBuyRespond, sizeof(G2C_SHOP_BUY_RESPOND));
    REGISTER_EXTERNAL_FUNC(g2c_trading_invite_request, &KPlayerClient::OnTradingInviteRequest, sizeof(G2C_TRADING_INVITE_REQUEST));
   
    REGISTER_EXTERNAL_FUNC(g2c_open_trading_box_notify, &KPlayerClient::OnOpenTradingBoxNotify, sizeof(G2C_OPEN_TRADING_BOX_NOTIFY));
    REGISTER_EXTERNAL_FUNC(g2c_sync_trading_confirm, &KPlayerClient::OnSyncTradingConfirm, sizeof(G2C_SYNC_TRADING_CONFIRM));
    REGISTER_EXTERNAL_FUNC(g2c_sync_trading_item_data, &KPlayerClient::OnSyncTradingItemData, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_sync_trading_money, &KPlayerClient::OnSyncTradingMoney, sizeof(G2C_SYNC_TRADING_MONEY));
    REGISTER_EXTERNAL_FUNC(g2c_trading_complete, &KPlayerClient::OnTradingComplete, sizeof(G2C_TRADING_COMPLETE));

    REGISTER_EXTERNAL_FUNC(g2c_sync_money, &KPlayerClient::OnSyncMoney, sizeof(G2C_SYNC_MONEY));
	REGISTER_EXTERNAL_FUNC(g2c_sync_temp_item_data, &KPlayerClient::OnSyncTempItemData, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_sync_item_data, &KPlayerClient::OnSyncItemData, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_sync_item_remove, &KPlayerClient::OnSyncItemRemove, sizeof(G2C_SYNC_ITEM_REMOVE));
	REGISTER_EXTERNAL_FUNC(g2c_exchange_item, &KPlayerClient::OnExchangeItem, sizeof(G2C_EXCHANGE_ITEM));
	REGISTER_EXTERNAL_FUNC(g2c_update_item_amount, &KPlayerClient::OnUpdateItemAmount, sizeof(G2C_UPDATE_ITEM_AMOUNT));
    REGISTER_EXTERNAL_FUNC(g2c_sync_equip_represent, &KPlayerClient::OnSyncEquipRepresent, sizeof(G2C_SYNC_EQUIP_REPRESENT));
    

	REGISTER_EXTERNAL_FUNC(g2c_learn_profession_notify, &KPlayerClient::OnLearnProfessionNotify, sizeof(G2C_LEARN_PROFESSION_NOTIFY));
	REGISTER_EXTERNAL_FUNC(g2c_learn_recipe_notify, &KPlayerClient::OnLearnRecipeNotify, sizeof(G2C_LEARN_RECIPE_NOTIFY));
	REGISTER_EXTERNAL_FUNC(g2c_forget_profession_notify, &KPlayerClient::OnForgetProfessionNotify, sizeof(G2C_FORGET_PROFESSION_NOTIFY));
	REGISTER_EXTERNAL_FUNC(g2c_forget_recipe_notify, &KPlayerClient::OnForgetRecipeNotify, sizeof(G2C_FORGET_RECIPE_NOTIFY));
	REGISTER_EXTERNAL_FUNC(g2c_add_profession_proficiency, &KPlayerClient::OnAddProfessionProficiency, sizeof(G2C_ADD_PROFESSION_PROFICIENCY));
	REGISTER_EXTERNAL_FUNC(g2c_set_max_profession_level_notify, &KPlayerClient::OnSetMaxProfessionLevelNotify, sizeof(G2C_SET_MAX_PROFESSION_LEVEL_NOTIFY));
	REGISTER_EXTERNAL_FUNC(g2c_update_player_profession, &KPlayerClient::OnUpdatePlayerProfession, sizeof(G2C_UPDATE_PLAYER_PROFESSION));
	REGISTER_EXTERNAL_FUNC(g2c_update_player_recipe, &KPlayerClient::OnUpdatePlayerRecipe, sizeof(G2C_UPDATE_PLAYER_RECIPE));
    REGISTER_EXTERNAL_FUNC(g2c_cast_recipe, &KPlayerClient::OnCastRecipe, sizeof(G2C_CAST_RECIPE));
    
	REGISTER_EXTERNAL_FUNC(g2c_open_window, &KPlayerClient::OnOpenWindow, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_start_use_recipe, &KPlayerClient::OnStartUseRecipe, sizeof(G2C_START_USE_RECIPE));
	REGISTER_EXTERNAL_FUNC(g2c_end_cast, &KPlayerClient::OnEndCast, sizeof(G2C_END_CAST));

	REGISTER_EXTERNAL_FUNC(g2c_accept_quest, &KPlayerClient::OnAcceptQuest, sizeof(G2C_ACCEPT_QUEST));
	REGISTER_EXTERNAL_FUNC(g2c_finish_quest, &KPlayerClient::OnFinishQuest, sizeof(G2C_FINISH_QUEST));
	REGISTER_EXTERNAL_FUNC(g2c_cancel_quest, &KPlayerClient::OnCancelQuest, sizeof(G2C_CANCEL_QUEST));
	REGISTER_EXTERNAL_FUNC(g2c_share_quest, &KPlayerClient::OnShareQuest, sizeof(G2C_SHARE_QUEST));
	REGISTER_EXTERNAL_FUNC(g2c_sync_quest_value, &KPlayerClient::OnSyncQuestValue, sizeof(G2C_SYNC_QUEST_VALUE));

	REGISTER_EXTERNAL_FUNC(g2c_sync_loot_list, &KPlayerClient::OnSyncLootList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_roll_item_message, &KPlayerClient::OnRollItemMessage, sizeof(G2C_ROLL_ITEM_MESSAGE));
	REGISTER_EXTERNAL_FUNC(g2c_character_pick, &KPlayerClient::OnCharacterPick, sizeof(G2C_CHARACTER_PICK));
	REGISTER_EXTERNAL_FUNC(g2c_open_loot_list, &KPlayerClient::OnOpenLootList, sizeof(G2C_OPEN_LOOT_LIST));
	REGISTER_EXTERNAL_FUNC(g2c_sync_fight_flag, &KPlayerClient::OnSyncFightFlag, sizeof(G2C_SYNC_FIGHT_FLAG));
	REGISTER_EXTERNAL_FUNC(g2c_sync_repute, &KPlayerClient::OnSyncRepute, sizeof(G2C_SYNC_REPUTE));
	REGISTER_EXTERNAL_FUNC(g2c_sync_fightflag_list, &KPlayerClient::OnSyncFightflagList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_sync_repute_list, &KPlayerClient::OnSyncReputeList, UNDEFINED_PROTOCOL_SIZE);
	REGISTER_EXTERNAL_FUNC(g2c_character_open, &KPlayerClient::OnCharacterOpen, sizeof(G2C_CHARACTER_OPEN));
	REGISTER_EXTERNAL_FUNC(g2c_roll_money_message, &KPlayerClient::OnRollMoneyMessage, sizeof(G2C_ROLL_MONEY_MESSAGE));
	REGISTER_EXTERNAL_FUNC(g2c_finish_loot, &KPlayerClient::OnFinishLoot, sizeof(G2C_FINISH_LOOT));
	REGISTER_EXTERNAL_FUNC(g2c_begin_roll_item, &KPlayerClient::OnBeginRollItem, sizeof(G2C_BEGIN_ROLL_ITEM));
	REGISTER_EXTERNAL_FUNC(g2c_custom_OTAction, &KPlayerClient::OnCustomOTAction, sizeof(G2C_CUSTOM_OTACTION));
	REGISTER_EXTERNAL_FUNC(g2c_share_quest_result, &KPlayerClient::OnShareQuestResult, sizeof(G2C_SHARE_QUEST_RESULT));
	REGISTER_EXTERNAL_FUNC(g2c_modify_threat, &KPlayerClient::OnModifyThreat, sizeof(G2C_MODIFY_THREAT));
	REGISTER_EXTERNAL_FUNC(g2c_modify_threaten, &KPlayerClient::OnModifyThreaten, sizeof(G2C_MODIFY_THREATEN));
	REGISTER_EXTERNAL_FUNC(g2c_clear_threat, &KPlayerClient::OnClearThreat, sizeof(G2C_CLEAR_THREAT));
	REGISTER_EXTERNAL_FUNC(g2c_clear_threaten, &KPlayerClient::OnClearThreaten, sizeof(G2C_CLEAR_THREATEN));

    REGISTER_EXTERNAL_FUNC(g2c_player_talk_to_one, &KPlayerClient::OnPlayerTalkToOne, UNDEFINED_PROTOCOL_SIZE);
    REGISTER_EXTERNAL_FUNC(g2c_player_talk_to_many, &KPlayerClient::OnPlayerTalkToMany, UNDEFINED_PROTOCOL_SIZE);

    REGISTER_EXTERNAL_FUNC(g2c_update_double_exp, &KPlayerClient::OnUpdateDoubleExp, sizeof(G2C_UPDATE_DOUBLE_EXP));
	REGISTER_EXTERNAL_FUNC(g2c_update_player_stamina,&KPlayerClient::OnUpdatePlayerStamina, sizeof(G2C_UPDATE_PLAYER_STAMINA));
    REGISTER_EXTERNAL_FUNC(g2c_update_player_qi, &KPlayerClient::OnUpdatePlayerQi, sizeof(G2C_UPDATE_PLAYER_QI));
    REGISTER_EXTERNAL_FUNC(g2c_doodad_loot_over, &KPlayerClient::OnDoodadLootOver, sizeof(G2C_DOODAD_LOOT_OVER));

    REGISTER_EXTERNAL_FUNC(g2c_player_add_relation, &KPlayerClient::OnPlayerAddRelationRespond, sizeof(G2C_PLAYER_ADD_RELATION));
    REGISTER_EXTERNAL_FUNC(g2c_player_del_relation, &KPlayerClient::OnPlayerDelRelationRespond, sizeof(G2C_PLAYER_DEL_RELATION));
    //AutoCode:注册协议
   
}

KPlayerClient::~KPlayerClient(void)
{
	IKG_Buffer* piPacket = (IKG_Buffer*)m_BufferPacketList.Pop();
	ASSERT(piPacket == NULL);

    ASSERT(!m_piSocketStream);
}

BOOL KPlayerClient::Init(KMonster *pOwner)
{
    int nResult  = false;
	int nRetCode = false;

    KG_PROCESS_ERROR(pOwner);

	nRetCode = m_BufferPacketList.Init(MAX_PACKAGE_COUNT);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_PlayerDisplayDataCache.Init(PLAYER_DISPLAY_DATA_MAX_CACHE);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_NpcDisplayDataCache.Init(NPC_DISPLAY_DATA_MAX_CACHE);
	KGLOG_PROCESS_ERROR(nRetCode);

    m_pOwner = pOwner;
    
	nResult = true;

Exit0:

	return (BOOL)nResult;
}

BOOL KPlayerClient::UnInit(void)
{
	Disconnect();
        
    m_pOwner = NULL;

	return TRUE;
}

BOOL KPlayerClient::Connect(const char* pszIPAddr, int nPort)
{
    int nResult = false;
	int nRetCode = false;
    KG_SocketConnector  Connector;
    timeval             TimeoutValue    = {60, 0};

	//检测传入的IP地址字符串指针					  
	KGLOG_PROCESS_ERROR(pszIPAddr);

	//清空协议缓冲区
	IKG_Buffer* piPacket = (IKG_Buffer*)m_BufferPacketList.Pop();
	while (piPacket != NULL)
	{
		KG_COM_RELEASE(piPacket);

		piPacket = (IKG_Buffer*)m_BufferPacketList.Pop();
	}

     KGLOG_PROCESS_ERROR(!m_piSocketStream);
     m_piSocketStream = Connector.ConnectSecurity(pszIPAddr, nPort, KSG_ENCODE_DECODE);
     KGLogPrintf(
         KGLOG_INFO, 
         "[KPlayerClient] Connect to %s : %d ... ... [%s]", 
         pszIPAddr, nPort, m_piSocketStream ? "OK" : "Failed"
     );
     KGLOG_PROCESS_ERROR(m_piSocketStream);

     nRetCode = m_piSocketStream->SetTimeout(&TimeoutValue);
     KGLOG_PROCESS_ERROR(nRetCode);

     m_nState = gmsOnline;

    nResult = true;
Exit0:
    if (!nResult)
    {
        m_nState = gmsConnectFail;
        KG_COM_RELEASE(m_piSocketStream);
    }

    return (BOOL)nResult;
}

BOOL KPlayerClient::Disconnect()
{
	int nRetCode = false;

	//清空协议缓冲区
	IKG_Buffer* piPacket = (IKG_Buffer*)m_BufferPacketList.Pop();
	while (piPacket != NULL)
	{
		KG_COM_RELEASE(piPacket);

		piPacket = (IKG_Buffer*)m_BufferPacketList.Pop();
	}

    KG_COM_RELEASE(m_piSocketStream);

	m_ApplyNewCharacterList.clear();
	m_nGSGameLoop = 0;

	return TRUE;
}

int KPlayerClient::Breathe(void)
{
    int             nResult  = 0;
    int             nRetCode = false;
    IKG_Buffer     *piBuffer = NULL;
	char*			pData    = NULL;
	unsigned int	uSize    = 0;

	//先检测数据包缓存中的数据包，以保证解析的顺序
	KBufferPacket* pBufferPacket = NULL;
	IKG_Buffer* piPacket = (IKG_Buffer*)m_BufferPacketList.Check();
	if (piPacket != NULL)
		pBufferPacket = (KBufferPacket*)piPacket->GetData();
	else
		pBufferPacket = NULL;

    while (pBufferPacket != NULL && pBufferPacket->m_nGameLoop <= m_pOwner->m_nGameLoop)
	{
		//如果到了解析的游戏循环数，则弹出缓存
		m_BufferPacketList.Pop();

		//调用相应的接收函数
		ASSERT(m_ProcessProtocolFuns[pBufferPacket->m_pHeader->byProtocolID]);
		(this->*m_ProcessProtocolFuns[pBufferPacket->m_pHeader->byProtocolID])
			((char*)pBufferPacket->m_pHeader, pBufferPacket->m_nSize, pBufferPacket->m_nGameLoop);

		//释放内存块
		KG_COM_RELEASE(piPacket);

		//取下一个下协议包
        pBufferPacket = NULL;
		piPacket = (IKG_Buffer*)m_BufferPacketList.Check();
		if (piPacket)
			pBufferPacket = (KBufferPacket*)piPacket->GetData();
	}
    
    KG_PROCESS_ERROR(m_piSocketStream);
    nRetCode = m_piSocketStream->IsAlive();
    KGLOG_PROCESS_ERROR(nRetCode);

	//再检测网络连接上传送来的数据
    while (true)
    {
        KG_COM_RELEASE(piBuffer);

        timeval TimeoutValue    = {0, 0};

        KG_PROCESS_ERROR(m_piSocketStream);

        nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
        KGLOG_PROCESS_ERROR(nRetCode != -1);
        if (nRetCode == 0)
            break;  // if no data arrive

        nRetCode = m_piSocketStream->Recv(&piBuffer);
        KGLOG_PROCESS_SUCCESS(nRetCode != 1);  // if timeout or error then error

        ASSERT(piBuffer);
        pData = (char *)piBuffer->GetData();
        uSize = piBuffer->GetSize();

        while (uSize > 0)
        {
		    DOWNWARDS_PROTOCOL_HEADER* pHeader = (DOWNWARDS_PROTOCOL_HEADER*)pData;

		    //检测协议号
		    KGLOG_PROCESS_ERROR(pHeader->byProtocolID > gs_client_connection_begin);
		    KGLOG_PROCESS_ERROR(pHeader->byProtocolID < gs_client_connection_end);

		    //检测协议包的大小
		    int nProtocolSize = m_nProtocolSize[pHeader->byProtocolID];
		    if (nProtocolSize == UNDEFINED_PROTOCOL_SIZE)
		    {
			    UNDEFINED_SIZE_DOWNWARDS_HEADER* pUndefinedSizeHeader = (UNDEFINED_SIZE_DOWNWARDS_HEADER*)pData;
			    nProtocolSize = pUndefinedSizeHeader->wSize;
		    }
		    KGLOG_PROCESS_ERROR(nProtocolSize <= (int)uSize);

		    //如果客户端游戏循环大于数据包的游戏循环数，
		    //或者是帧间隔信号,则立刻解析
		    if (
                m_nGSGameLoop <= m_pOwner->m_nGameLoop ||
			    pHeader->byProtocolID == g2c_frame_signal
            )
		    {
			    ASSERT(m_ProcessProtocolFuns[pHeader->byProtocolID]);
			    (this->*m_ProcessProtocolFuns[pHeader->byProtocolID])(pData, nProtocolSize, m_nGSGameLoop);
		    }
		    else
		    {
			    //其他协议包则缓存起来等到合适的时候再解析
			    IKG_Buffer* piPacket = KG_MemoryCreateBuffer(sizeof(KBufferPacket) + nProtocolSize);
			    KGLOG_PROCESS_ERROR(piPacket);

			    KBufferPacket* pBufferPacket = (KBufferPacket*)piPacket->GetData();
			    pBufferPacket->m_nGameLoop = m_nGSGameLoop;
			    pBufferPacket->m_nSize = nProtocolSize;
			    pBufferPacket->m_pHeader = (EXTERNAL_PROTOCOL_HEADER*)((char*)pBufferPacket + sizeof(KBufferPacket));
			    memcpy(pBufferPacket->m_pHeader, pData, nProtocolSize);

			    //放入缓存中
			    if (!m_BufferPacketList.Push(piPacket))
			    {
				    //缓冲区已满，断线
				    KG_COM_RELEASE(piPacket);
				    goto Exit0;
			    }
            }       

	        //uSize是网络库传出来的数据块的大小，nSize是已经解析的协议包的大小
	        //网络库传出的数据块可能有若干协议包组成
	        uSize -= nProtocolSize;
	        pData += nProtocolSize;
        }
	}
    
Exit1:
    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_nGSGameLoop > 0)
        {
            m_nState = gmsDisconnect;
            Disconnect();
        }
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

/************************************************************************/
/* 发送数据包                                                           */
/************************************************************************/
BOOL KPlayerClient::Send(void* pvData, size_t nSize)
{
    int nResult  = false;
    int nRetCode = false;
	BOOL                     bRetCode   = FALSE;
	UPWARDS_PROTOCOL_HEADER* pHeader    = (UPWARDS_PROTOCOL_HEADER*)pvData;
    IKG_Buffer              *piBuffer   = NULL;

	pHeader->nFrame = m_pOwner->m_nGameLoop;

    KGLOG_PROCESS_ERROR(m_piSocketStream);

    piBuffer = KG_MemoryCreateBuffer((unsigned)nSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    memcpy(piBuffer->GetData(), pvData, nSize);

    nRetCode = m_piSocketStream->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

	nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
	return nResult;
}

//登录请求玩家数据
BOOL KPlayerClient::DoApplyPlayerData(char* pszRoleName, GUID Guid)
{
	BOOL bRetCode = FALSE;

	C2G_APPLY_PLAYER_DATA ApplyPlayerData;

	ApplyPlayerData.byProtocolID = c2g_apply_player_data;
	strncpy(ApplyPlayerData.szName, pszRoleName, _NAME_LEN);
	ApplyPlayerData.szName[_NAME_LEN - 1] = 0;
	ApplyPlayerData.Guid = Guid;

	bRetCode = Send((char*)&ApplyPlayerData, sizeof(ApplyPlayerData));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

//确认同步玩家数据完成
BOOL KPlayerClient::DoAckPlayerData()
{
	BOOL bRetCode = FALSE;

	C2G_ACK_PLAYER_DATA AckPlayerData;

	AckPlayerData.byProtocolID = c2g_ack_player_data;

	bRetCode = Send((char*)&AckPlayerData, sizeof(AckPlayerData));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

//玩家登出
BOOL KPlayerClient::DoPlayerLogout(void)
{
	BOOL bRetCode = FALSE;

	C2G_PLAYER_LOGOUT PlayerLogout;

	PlayerLogout.byProtocolID = c2g_player_logout;

	bRetCode = Send((char*)&PlayerLogout, sizeof(PlayerLogout));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayerClient::DoCharacterStand(KCharacter* pCharacter)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	C2G_CHARACTER_STAND CharacterStand;

    ASSERT(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX);

	CharacterStand.byProtocolID      = c2g_character_stand;
    CharacterStand.nSrcX             = pCharacter->m_nX;
    CharacterStand.nSrcY             = pCharacter->m_nY;
    CharacterStand.nSrcZ             = pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pCharacter->m_nZ;
	CharacterStand.byFaceDirection   = (BYTE)pCharacter->m_nFaceDirection;
	
	bRetCode = Send(&CharacterStand, sizeof(CharacterStand));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}


BOOL KPlayerClient::DoCharacterWalk(KCharacter* pCharacter, int nDstX, int nDstY)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	C2G_CHARACTER_WALK  CharacterWalk;

    ASSERT(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX);

	CharacterWalk.byProtocolID      = c2g_character_walk;
    CharacterWalk.nSrcX             = pCharacter->m_nX;
    CharacterWalk.nSrcY             = pCharacter->m_nY;
    CharacterWalk.nSrcZ             = pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pCharacter->m_nZ;
	CharacterWalk.nDstX             = nDstX;
	CharacterWalk.nDstY             = nDstY;
	CharacterWalk.byFaceDirection   = (BYTE)pCharacter->m_nFaceDirection;
	
	bRetCode = Send(&CharacterWalk, sizeof(CharacterWalk));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoCharacterRun(KCharacter* pCharacter, int nDstX, int nDstY)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	C2G_CHARACTER_RUN   CharacterRun;

    ASSERT(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX);

	CharacterRun.byProtocolID      = c2g_character_run;
    CharacterRun.nSrcX             = pCharacter->m_nX;
    CharacterRun.nSrcY             = pCharacter->m_nY;
    CharacterRun.nSrcZ             = pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pCharacter->m_nZ;
	CharacterRun.nDstX             = nDstX;
	CharacterRun.nDstY             = nDstY;
	CharacterRun.byFaceDirection   = (BYTE)pCharacter->m_nFaceDirection;

	bRetCode = Send(&CharacterRun, sizeof(CharacterRun));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoCharacterSwim(KCharacter* pCharacter, int nDstX, int nDstY, int nDstZ)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	C2G_CHARACTER_SWIM  CharacterSwim;

    ASSERT(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX);
    ASSERT(pCharacter->m_nPitchDirection >= 0 && pCharacter->m_nPitchDirection <= UCHAR_MAX);

	CharacterSwim.byProtocolID      = c2g_character_swim;
    CharacterSwim.nSrcX             = pCharacter->m_nX;
    CharacterSwim.nSrcY             = pCharacter->m_nY;
    CharacterSwim.nSrcZ             = pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pCharacter->m_nZ;
	CharacterSwim.nDstX             = nDstX;
	CharacterSwim.nDstY             = nDstY;
    CharacterSwim.nDstZ             = nDstZ;
	CharacterSwim.byFaceDirection   = (BYTE)pCharacter->m_nFaceDirection;
    CharacterSwim.byPitchDirection  = (BYTE)pCharacter->m_nPitchDirection;

	bRetCode = Send(&CharacterSwim, sizeof(CharacterSwim));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

// 角色跳跃
BOOL KPlayerClient::DoCharacterJump(KCharacter* pCharacter, BOOL bStandJump, int nDirectionXY)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
	C2G_CHARACTER_JUMP  CharacterJump;

    ASSERT(nDirectionXY >= 0 && nDirectionXY <= UCHAR_MAX);
    ASSERT(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX);

	CharacterJump.byProtocolID     = c2g_character_jump;
    CharacterJump.nSrcX            = pCharacter->m_nX;
    CharacterJump.nSrcY            = pCharacter->m_nY;
    CharacterJump.nSrcZ            = pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pCharacter->m_nZ;
    CharacterJump.bStandJump       = (bool)bStandJump;
    CharacterJump.byJumpDirection  = (BYTE)nDirectionXY;
    CharacterJump.byFaceDirection  = (BYTE)pCharacter->m_nFaceDirection;

	bRetCode = Send(&CharacterJump, sizeof(CharacterJump));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCharacterSit(KCharacter* pCharacter)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
	C2G_CHARACTER_SIT   CharacterSit;

    ASSERT(pCharacter->m_nFaceDirection >= 0 && pCharacter->m_nFaceDirection <= UCHAR_MAX);

	CharacterSit.byProtocolID     = c2g_character_sit;
    CharacterSit.nSrcX            = pCharacter->m_nX;
    CharacterSit.nSrcY            = pCharacter->m_nY;
    CharacterSit.nSrcZ            = pCharacter->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pCharacter->m_nZ;
    CharacterSit.byFaceDirection  = (BYTE)pCharacter->m_nFaceDirection;

	bRetCode = Send(&CharacterSit, sizeof(CharacterSit));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

// 角色说话
BOOL KPlayerClient::DoCharacterSay(char* pszText)
{
	BOOL bRetCode = FALSE;
    size_t uTextLen = 0;
	unsigned char byBuf[sizeof(C2G_CHARACTER_SAY) + MAX_CHAT_TEXT_LEN];
	C2G_CHARACTER_SAY* pCharacterSay = (C2G_CHARACTER_SAY*)byBuf;

    KG_ASSERT_EXIT(pszText);
	uTextLen = strlen(pszText);
    KG_ASSERT_EXIT(uTextLen <= (MAX_CHAT_TEXT_LEN - sizeof('\0')));

	pCharacterSay->byProtocolID = c2g_character_say;

    pszText[uTextLen] = '\0';
    memcpy(pCharacterSay->szText, pszText, uTextLen + sizeof('\0'));
	pCharacterSay->wSize = (WORD)(sizeof(C2G_CHARACTER_SAY) + uTextLen + sizeof('\0'));

	bRetCode = Send(byBuf, pCharacterSay->wSize);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayerClient::DoStopCurrentAction()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    C2G_STOP_CURRENT_ACTION StopCurrentAction;
    
    StopCurrentAction.byProtocolID = c2g_stop_current_action;

    bRetCode = Send(&StopCurrentAction, sizeof(StopCurrentAction));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCharacterSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget* pTarget)
{
	BOOL                bRetCode        = FALSE;
	TARGET_TYPE         eTargetType     = ttInvalid;
    C2G_CHARACTER_SKILL CharacterSkill;

	KG_ASSERT_EXIT(pTarget);

	CharacterSkill.byProtocolID = c2g_character_skill;
	CharacterSkill.dwSkillID    = dwSkillID;
	CharacterSkill.bySkillLevel = (BYTE)dwSkillLevel;

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

	bRetCode = Send((char*)&CharacterSkill, sizeof(CharacterSkill));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

//角色交互动作
BOOL KPlayerClient::DoCharacterAction(DWORD dwCharacterID, BYTE byAction)
{
	BOOL bRetCode = FALSE;

	C2G_CHARACTER_ACTION CharacterAction;

	CharacterAction.byProtocolID = c2g_character_action;
	CharacterAction.dwCharacterID = dwCharacterID;
	CharacterAction.byAction = byAction;

	bRetCode = Send((char*)&CharacterAction, sizeof(CharacterAction));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

// 角色朝向
BOOL KPlayerClient::DoCharacterTurn(int nNewDirection)
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
    KPlayer*            pPlayer         = NULL;
	C2G_CHARACTER_TURN  CharacterTurn;

    ASSERT(nNewDirection >= 0 && nNewDirection <= UCHAR_MAX);

    // 站立状态才同步,其他状态就不用专门同步了,移动指令会带上的
    KG_PROCESS_ERROR(m_pOwner->m_pRole->m_eMoveState == cmsOnStand);

	CharacterTurn.byProtocolID = c2g_character_turn;
	CharacterTurn.byDirection  = (BYTE)nNewDirection;

	bRetCode = Send(&CharacterTurn, sizeof(CharacterTurn));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

//请求新角色的数据
BOOL KPlayerClient::DoApplyNewCharacterData(DWORD dwCharacterID)
{
    BOOL bResult  = FALSE;
	BOOL bRetCode = FALSE;
    APPLY_NEW_CHARACTER_LIST::iterator it;
	std::pair<APPLY_NEW_CHARACTER_LIST::iterator, bool> InsRet;

    KG_PROCESS_SUCCESS(g_IsPlayer(dwCharacterID));

	// 先看看以前请求过没有,如果请求过了,就不再发了
	it = m_ApplyNewCharacterList.find(dwCharacterID);
	KG_PROCESS_SUCCESS(it != m_ApplyNewCharacterList.end());

	InsRet = m_ApplyNewCharacterList.insert(std::make_pair(dwCharacterID, 0));
	KGLOG_PROCESS_ERROR(InsRet.second);

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoApplyPlayerDisplayData(KPlayer* pPlayer, DWORD dwCheckSum)
{
    BOOL                            bResult     = false;
	BOOL                            bRetCode    = false;	
	C2G_APPLY_PLAYER_DISPLAY_DATA   ApplyPlayerDisplayData;

	ApplyPlayerDisplayData.byProtocolID = c2g_apply_player_display_data;
	ApplyPlayerDisplayData.dwPlayerID   = pPlayer->m_dwID;
	ApplyPlayerDisplayData.dwCheckSum   = dwCheckSum;

	bRetCode = Send((char*)&ApplyPlayerDisplayData, sizeof(ApplyPlayerDisplayData));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoApplyNpcDisplayData(KNpc* pNpc, DWORD dwCheckSum)
{
    BOOL                        bResult  = false;
	BOOL                        bRetCode = false;
	C2G_APPLY_NPC_DISPLAY_DATA  ApplyNpcDisplayData;

	ApplyNpcDisplayData.byProtocolID = c2g_apply_npc_display_data;
	ApplyNpcDisplayData.dwNpcID      = pNpc->m_dwID;
	ApplyNpcDisplayData.dwCheckSum   = dwCheckSum;

	bRetCode = Send((char*)&ApplyNpcDisplayData, sizeof(ApplyNpcDisplayData));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}


BOOL KPlayerClient::DoMountKungfu(DWORD dwID, DWORD dwLevel)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    C2G_MOUNT_KUNGFU    Pak;

    Pak.byProtocolID = c2g_mount_kungfu;
    Pak.dwID         = dwID;
    Pak.dwLevel      = dwLevel;

    bRetCode = Send((char*)&Pak, sizeof(Pak));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoCancelBuff(int nStyle, int nIndex)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    C2G_CANCEL_BUFF     Pak;

    Pak.byProtocolID = c2g_cancel_buff;
    Pak.cStyle       = (char)nStyle;
    Pak.nIndex       = nIndex;

    bRetCode = Send((char*)&Pak, sizeof(Pak));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetSheathState(BOOL bSheathFlag)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2G_SET_SHEATH_STATE    Pak;

    Pak.byProtocolID = c2g_set_sheath_state;
    Pak.bSheathFlag  = (bool)bSheathFlag;

    bRetCode = Send((char*)&Pak, sizeof(Pak));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


BOOL KPlayerClient::DoInviteJoinPartyRequest(const char cszInvitePlayer[])
{
    BOOL                            bResult  = FALSE;
    BOOL                            bRetCode = FALSE;
    C2G_INVITE_JOIN_PARTY_REQUEST   PartyInvitation;

    KG_ASSERT_EXIT(cszInvitePlayer);
    PartyInvitation.byProtocolID = c2g_invite_join_party_request;
    strncpy(PartyInvitation.szInvitedPlayer, cszInvitePlayer, sizeof(PartyInvitation.szInvitedPlayer));
    PartyInvitation.szInvitedPlayer[sizeof(PartyInvitation.szInvitedPlayer) - 1] = '\0';

    bRetCode = Send((char *)&PartyInvitation, sizeof(PartyInvitation));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoInviteJoinPartyRespond(const char cszInviteSrc[], const GUID &crGuid, BOOL bAgreeFlag)
{
    BOOL                            bResult  = FALSE;
    BOOL                            bRetCode = FALSE;
    C2G_INVITE_JOIN_PARTY_RESPOND   Respond;

    KG_ASSERT_EXIT(cszInviteSrc);

    Respond.byProtocolID = c2g_invite_join_party_respond;
    Respond.guid         = crGuid;
    Respond.bAgreeFlag   = bAgreeFlag;

    strncpy(Respond.szInviteSrc, cszInviteSrc, sizeof(Respond.szInviteSrc));
    Respond.szInviteSrc[sizeof(Respond.szInviteSrc) - 1] = '\0';

    bRetCode = Send((char *)&Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoApplyJoinPartyRequest(const char cszApplyDst[])
{
    BOOL                            bResult  = FALSE;
    BOOL                            bRetCode = FALSE;
    C2G_APPLY_JOIN_PARTY_REQUEST    PartyApply;

    KG_ASSERT_EXIT(cszApplyDst);
    PartyApply.byProtocolID = c2g_apply_join_party_request;
    strncpy(PartyApply.szApplyDst, cszApplyDst, sizeof(PartyApply.szApplyDst));
    PartyApply.szApplyDst[sizeof(PartyApply.szApplyDst) - 1] = '\0';

    bRetCode = Send((char *)&PartyApply, sizeof(PartyApply));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    return bResult;
}
BOOL KPlayerClient::DoApplyJoinPartyRespond(const char cszApplySrc[], const GUID &crGuid, BOOL bAgreeFlag)
{
    BOOL                            bResult  = FALSE;
    BOOL                            bRetCode = FALSE;
    C2G_APPLY_JOIN_PARTY_RESPOND    Respond;

    KG_ASSERT_EXIT(cszApplySrc);

    Respond.byProtocolID = c2g_apply_join_party_respond;
    Respond.guid         = crGuid;
    Respond.bAgreeFlag   = bAgreeFlag;

    strncpy(Respond.szApplySrc, cszApplySrc, sizeof(Respond.szApplySrc));
    Respond.szApplySrc[sizeof(Respond.szApplySrc) - 1] = '\0';

    bRetCode = Send((char *)&Respond, sizeof(Respond));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoLeavePartyRequest()
{
    BOOL                            bResult                 = FALSE;
    int                             nRetCode                = FALSE;
    

    bResult = TRUE;
Exit0:
    return bResult;
}
 
BOOL KPlayerClient::DoKickoutPartyMember(const char cszRoleName[])
{
    BOOL                            bResult                 = FALSE;
    int                             nRetCode                = FALSE;
    

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPartyChangeLeaderRequest(const char cszNewLeader[])
{
    BOOL                            bResult         = FALSE;
    BOOL                            bRetCode        = FALSE;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoPartySetLootMode(int nLootMode, KPlayer* pPlayer)
{
    BOOL                                    bResult  = FALSE;

    bResult = TRUE;
Exit0:
    return bResult;    
}

BOOL KPlayerClient::DoPartySetRollQuality(int nRollQuality, KPlayer* pPlayer)
{
	BOOL	bRetCode = FALSE;

	bRetCode = TRUE;
Exit0:
	return bRetCode; 	
}

BOOL KPlayerClient::DoPartyMemberNotifySignpost(int nX, int nY)
{
    BOOL     bResult  = FALSE;
    BOOL     bRetCode = FALSE;
    KPlayer *pSelf    = NULL;

Exit1:
    bResult = TRUE;
Exit0:
    return bRetCode; 
}

// 接受任务
BOOL KPlayerClient::DoAcceptQuest(KTarget* pTarget, DWORD dwQuestID)
{

	return TRUE;
}

//完成任务
BOOL KPlayerClient::DoFinishQuest(DWORD dwQuestID, KTarget& rTarget, int nPresentChoice1, int nPresentChoice2)
{
	return TRUE;
}

//取消任务
BOOL KPlayerClient::DoCancelQuest(int nQuestIndex)
{
	BOOL bRetCode = FALSE;

	C2G_CANCEL_QUEST CancelQuest;

	CancelQuest.byProtocolID = c2g_cancel_quest;
	CancelQuest.nQuestIndex = nQuestIndex;

	bRetCode = Send((char*)&CancelQuest, sizeof(CancelQuest));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

// 共享任务
BOOL KPlayerClient::DoShareQuest(int nQuestIndex)
{
	BOOL bRetCode = FALSE;

	C2G_SHARE_QUEST ShareQuest;

	ShareQuest.byProtocolID = c2g_share_quest;
	ShareQuest.nQuestIndex = nQuestIndex;

	bRetCode = Send((char*)&ShareQuest, sizeof(ShareQuest));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

//GM指令
BOOL KPlayerClient::DoGmCommand(const char* pszGmCommand)
{
	BOOL bRetCode = FALSE;
	char szPackage[MAX_EXTERNAL_PACKAGE_SIZE];
	C2G_GM_COMMAND* pGmCommand = (C2G_GM_COMMAND*)szPackage;

	KGLOG_PROCESS_ERROR(pszGmCommand);
	KGLOG_PROCESS_ERROR((sizeof(C2G_GM_COMMAND) + strlen(pszGmCommand) + 1) < MAX_EXTERNAL_PACKAGE_SIZE);

	pGmCommand->byProtocolID = c2g_gm_command;
	pGmCommand->wSize = (WORD)(sizeof(C2G_GM_COMMAND) + strlen(pszGmCommand) + 1);

	strcpy(pGmCommand->szGmCommand, pszGmCommand);
	pGmCommand->szGmCommand[strlen(pszGmCommand)] = 0;

	bRetCode = Send((char*)pGmCommand, pGmCommand->wSize);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayerClient::DoOpenShopRequest(DWORD dwShopID)
{
    BOOL                    bResult     = FALSE;
    BOOL                    bRetCode    = FALSE;
    C2G_OPEN_SHOP_REQUEST   Request;
    
    Request.byProtocolID = c2g_open_shop_request;
    Request.dwShopID = dwShopID;
    
    bRetCode = Send((char *)&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoQueryShopItems(DWORD dwShopID, DWORD dwPageIndex)
{
    BOOL                 bResult  = FALSE;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoShopBuyRequest(
    DWORD dwShopID, DWORD dwPageIndex, DWORD dwPosIndex, DWORD dwItemID, int nCount, int nCost
)
{
    BOOL                  bResult       = FALSE;
    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoShopSellRequest(
	DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex
)
{
	BOOL 					bResult  = FALSE;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KPlayerClient::DoShopRepairRequest(
    DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex
)
{
    BOOL 					bResult  = FALSE;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSoldListBuyRequest(DWORD dwShopID, DWORD dwX, DWORD dwItemID)
{
    BOOL                        bResult     = FALSE;

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingInviteRequest(DWORD dwInviteDst)
{
    BOOL                       bResult  = false;
    BOOL                       bRetCode = false;
    C2G_TRADING_INVITE_REQUEST Request;

    Request.byProtocolID = c2g_trading_invite_request;
    Request.dwInviteDst  = dwInviteDst;

    bRetCode = Send(&Request, sizeof(Request));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;

}

BOOL KPlayerClient::DoTradingInviteRespond(BOOL bAccept)
{
    BOOL                       bResult  = false;
    BOOL                       bRetCode = false;
    C2G_TRADING_INVITE_RESPOND Respond;

    Respond.byProtocolID = c2g_trading_invite_respond;
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
    C2G_TRADING_ADD_ITEM    AddItem;

    AddItem.byProtocolID = c2g_trading_add_item;
    AddItem.byBoxIndex   = (BYTE)dwBoxIndex;
    AddItem.byPosIndex   = (BYTE)dwPosIndex;
    AddItem.byGridIndex  = (BYTE)dwGridIndex;

    bRetCode = Send((char*)&AddItem, sizeof(AddItem));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingDeleteItem(DWORD dwGridIndex)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    C2G_TRADING_DELETE_ITEM DeleteItem;
    
    DeleteItem.byProtocolID = c2g_trading_delete_item;
    DeleteItem.byGridIndex  = (BYTE)dwGridIndex;
    
    bRetCode = Send((char*)&DeleteItem, sizeof(DeleteItem));
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingSetMoney(int nMoney)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    C2G_TRADING_SET_MONEY   SetMoney;

    SetMoney.byProtocolID = c2g_trading_set_money;
    SetMoney.nMoney       = nMoney;

    bRetCode = Send((char*)&SetMoney, sizeof(SetMoney));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoTradingConfirm(BOOL bConfirm, int nKey)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    C2G_TRADING_CONFIRM     TradingConfirm;

    TradingConfirm.byProtocolID = c2g_trading_confirm;
    TradingConfirm.bConfirm     = bConfirm;
    TradingConfirm.nKey         = nKey;

    bRetCode = Send((char*)&TradingConfirm, sizeof(TradingConfirm));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayerClient::DoSetTarget(int nTargetType, DWORD dwTargetID)
{
    BOOL            bResult  = false;
    BOOL            bRetCode = false;
    C2G_SET_TARGET  SetTarget;

    SetTarget.byProtocolID = c2g_set_target;
    SetTarget.byTargetType = (BYTE)nTargetType;
    SetTarget.dwTargetID   = dwTargetID;

    bRetCode = Send((char *)&SetTarget, sizeof(SetTarget));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


// 申请交换装备
BOOL KPlayerClient::DoApplyExchangeItem(DWORD dwBox1, DWORD dwX1, DWORD dwBox2, DWORD dwX2, DWORD dwAmount)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	C2G_APPLY_EXCHANGE_ITEM ApplyExchangeItem;

	ApplyExchangeItem.byProtocolID = c2g_apply_exchange_item;
	ApplyExchangeItem.byBox1 = (BYTE)dwBox1;
	ApplyExchangeItem.byBox2 = (BYTE)dwBox2;
	ApplyExchangeItem.byX1 = (BYTE)dwX1;
	ApplyExchangeItem.byX2 = (BYTE)dwX2;
	ApplyExchangeItem.wAmount = (WORD)dwAmount;

	bRetCode = Send((char *)&ApplyExchangeItem, sizeof(ApplyExchangeItem));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = TRUE;

Exit0:

	return bResult;
}


// 选择窗口选项
BOOL KPlayerClient::DoWindowSelect(DWORD dwIndex, BYTE bySelect)
{
	BOOL bRetCode = FALSE;

	C2G_WINDOW_SELECT WindowSelect;

	WindowSelect.byProtocolID = c2g_window_select;
	WindowSelect.dwIndex = dwIndex;
	WindowSelect.bySelect = bySelect;

	bRetCode = Send((char*)&WindowSelect, sizeof(WindowSelect));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}


// 配方使用协议
BOOL KPlayerClient::DoCastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, KTarget* pTarget)
{

	return TRUE;
}


// 拾取物品或者掷骰子
BOOL KPlayerClient::DoLootOrRollItem(DWORD dwDoodadID, DWORD dwItemID, BOOL bCancel)
{
	BOOL bRetCode = FALSE;

	C2G_LOOT_OR_ROLL_ITEM LootOrRollItem;

	LootOrRollItem.byProtocolID = c2g_loot_or_roll_item;
	LootOrRollItem.dwDoodadID = dwDoodadID;
	LootOrRollItem.dwItemID = dwItemID;
	LootOrRollItem.bCancel = bCancel;

	bRetCode = Send((char*)&LootOrRollItem, sizeof(LootOrRollItem));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

// 申请拾取列表
BOOL KPlayerClient::DoApplyLootList(DWORD dwDoodadID)
{
	BOOL bRetCode = FALSE;
	C2G_APPLY_LOOT_LIST ApplyLootList;

	ApplyLootList.byProtocolID = c2g_apply_loot_list;
	ApplyLootList.dwDoodadID = dwDoodadID;

	bRetCode = Send((char*)&ApplyLootList, sizeof(ApplyLootList));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}


// 打开
BOOL KPlayerClient::DoCharacterOpen(DWORD dwDoodadID)
{
	BOOL bRetCode = FALSE;

	C2G_CHARACTER_OPEN CharacterOpen;

	CharacterOpen.byProtocolID = c2g_character_open;
	CharacterOpen.dwDoodadID = dwDoodadID;

	bRetCode = Send((char*)&CharacterOpen, sizeof(CharacterOpen));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayerClient::DoDeathRespond(int nRespondCode)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
    C2G_DEATH_RESPOND   Respond;

    Respond.byProtocolID = c2g_death_respond;
    Respond.nRespondCode = nRespondCode;

	bRetCode = Send((char*)&Respond, sizeof(Respond));
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


// 申请使用道具
BOOL KPlayerClient::DoApplyUseItem(DWORD dwBox, DWORD dwX, KTarget& rTarget)
{
	return TRUE;
}


// 改变战斗开关
BOOL KPlayerClient::DoApplyFightFlag(DWORD dwForceID, BOOL bFight)
{
	BOOL bRetCode = FALSE;

	C2G_APPLY_FIGHT_FLAG ApplyFightFlag;

	ApplyFightFlag.byProtocolID = c2g_apply_fight_flag;
	ApplyFightFlag.byForceID = (BYTE)dwForceID;
	ApplyFightFlag.byOpen = (BYTE)bFight;

	bRetCode = Send((char*)&ApplyFightFlag, sizeof(ApplyFightFlag));
	KGLOG_PROCESS_ERROR(bRetCode);
	
	return TRUE;

Exit0:

	return FALSE;
}


// 结束拾取
BOOL KPlayerClient::DoFinishLoot(DWORD dwDoodadID)
{
	BOOL bRetCode = FALSE;

	C2G_FINISH_LOOT FinishLoot;

	FinishLoot.byProtocolID = c2g_finish_loot;
	FinishLoot.dwDoodadID = dwDoodadID;
	bRetCode = Send((char*)&FinishLoot, sizeof(FinishLoot));
	KGLOG_PROCESS_ERROR(bRetCode);
	return TRUE;

Exit0:

	return FALSE;
}


// 拾取金钱
BOOL KPlayerClient::DoLootMoney(DWORD dwDoodadID)
{
	BOOL bRetCode = FALSE;

	C2G_LOOT_MONEY LootMoney;

	LootMoney.byProtocolID = c2g_loot_money;
	LootMoney.dwDoodadID = dwDoodadID;
	
	bRetCode = Send((char*)&LootMoney, sizeof(LootMoney));
	KGLOG_PROCESS_ERROR(bRetCode);
	return TRUE;

Exit0:

	return FALSE;
}

// 改变PK状态
BOOL KPlayerClient::DoChangePkMode(int nMode)
{
	BOOL bRetCode = FALSE;

	C2G_CHANGE_PK_MODE ChangePkMode;

	ChangePkMode.byProtocolID = c2g_change_pk_mode;
	ChangePkMode.byPKMode = (BYTE)nMode;
	
	bRetCode = Send((char*)&ChangePkMode, sizeof(ChangePkMode));
	KGLOG_PROCESS_ERROR(bRetCode);
	return TRUE;

Exit0:

	return FALSE;
}

// 队长分配物品
BOOL KPlayerClient::DoLeaderDistributeItem(DWORD dwDoodadID, DWORD dwItemID, DWORD dwDstPlayerID)
{
	BOOL bRetCode = FALSE;

	C2G_LEADER_DISTRIBUTE_ITEM LeaderDistributeItem;

	LeaderDistributeItem.byProtocolID = c2g_leader_distribute_item;
	LeaderDistributeItem.dwDoodadID = dwDoodadID;
	LeaderDistributeItem.dwItemID = dwItemID;
	LeaderDistributeItem.dwDstPlayerID = dwDstPlayerID;

	bRetCode = Send((char*)&LeaderDistributeItem, sizeof(LeaderDistributeItem));
	KGLOG_PROCESS_ERROR(bRetCode);
	return TRUE;

Exit0:

	return FALSE;
}

// Client 向 GameServer 发送密聊信息
BOOL KPlayerClient::DoPlayerTalkToOne(const char cszTalkTo[], unsigned uTalkDataSize, BYTE* pbyTalkData)
{
    return FALSE;
}

// Client 向 GameServer 发送群聊信息
BOOL KPlayerClient::DoPlayerTalkToMany(BYTE byTalkRange, unsigned uTalkDataSize, BYTE* pbyTalkData)
{
    return FALSE;
}

BOOL KPlayerClient::DoPlayerAddRelationRequest(const char cszAlliedPlayerName[])
{
    BOOL bResult  = FALSE;
    return bResult;
}

BOOL KPlayerClient::DoPlayerDelRelationRequest(DWORD dwAlliedPlayerID)
{
    BOOL bResult  = FALSE;
    return bResult;
}

//AutoCode:-发送协议函数结束-

/************************************************************************/
/* 接收数据包                                                           */
/************************************************************************/

//出错信息
void KPlayerClient::OnResponseMessage(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步玩家基本信息
void KPlayerClient::OnSyncPlayerBaseInfo(char* pData, size_t nSize, int nFrame)
{
    BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
	G2C_SYNC_PLAYER_BASE_INFO*  pPlayerBaseInfo     = (G2C_SYNC_PLAYER_BASE_INFO*)pData;
	KPlayer*                    pPlayer             = NULL;
	int                         nRegionX           = 0;
    int                         nRegionY           = 0;
    KScene*                     pScene              = NULL;
    KRegion*                    pAllocRegion        = NULL;
    int                         nMaxRegionCountX   = 0;
    int                         nMaxRegionCountY   = 0;
   
	//创建场景
	pScene = g_SO3World.GetScene(pPlayerBaseInfo->dwMapID, pPlayerBaseInfo->dwMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

    //加载
    nRegionX = pPlayerBaseInfo->nX / CELL_LENGTH / REGION_GRID_WIDTH;
    nRegionY = pPlayerBaseInfo->nY / CELL_LENGTH / REGION_GRID_HEIGHT;
    bRetCode = pScene->ClientLoadRegion(nRegionX, nRegionY);
    KGLOG_PROCESS_ERROR(bRetCode);
	
	m_pOwner->m_dwClientMapID = pPlayerBaseInfo->dwMapID;
	m_pOwner->m_dwClientMapCopyIndex = pPlayerBaseInfo->dwMapCopyIndex;

	m_nGSGameLoop = pPlayerBaseInfo->nGameLoop;
	m_pOwner->m_nGameLoop = pPlayerBaseInfo->nGameLoop;

	//创建玩家
	pPlayer = m_pOwner->m_Eyeshot.NewPlayer(pPlayerBaseInfo->dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);   

    strncpy(pPlayer->m_szName, pPlayerBaseInfo->szRoleName, _NAME_LEN);
    pPlayer->m_szName[_NAME_LEN - 1] = 0;

    pPlayer->m_nFaceDirection   = pPlayerBaseInfo->byFaceDirection;
    pPlayer->m_eRoleType    = (ROLE_TYPE)pPlayerBaseInfo->byRoleType;
    pPlayer->m_nDoubleExp   = pPlayerBaseInfo->nDoubleExp;
    pPlayer->m_nExperience  = pPlayerBaseInfo->nExp;
    pPlayer->m_eMoveState   = pPlayerBaseInfo->bAlive ? cmsOnStand : cmsOnDeath;
    memcpy(pPlayer->m_wRepresentId, pPlayerBaseInfo->wRepresentID, sizeof(pPlayer->m_wRepresentId));

    pPlayer->setLevel(pPlayerBaseInfo->byLevel);

    bRetCode = DoAckPlayerData();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_SO3World.AddPlayer(pPlayer, pScene, pPlayerBaseInfo->nX, pPlayerBaseInfo->nY, pPlayerBaseInfo->nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_pOwner->m_pRole = pPlayer;
    m_pOwner->m_piOperator->Start();

	pPlayer->m_eGameStatus = gsPlaying;

	return;

Exit0:

	if (pPlayer && pPlayer->m_eGameStatus == gsPlaying)
		g_SO3World.RemovePlayer(pPlayer);

	if (pPlayer)
		m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);

    m_nState = gmsSyncDataFail;
    Disconnect();

	return;
}

//同步任务状态
void KPlayerClient::OnSyncQuestState(char* pData, size_t nSize, int nFrame)
{
    return;
}

//同步任务列表
void KPlayerClient::OnSyncQuestList(char* pData, size_t nSize, int nFrame)
{
	return;
}

//踢号通知
void KPlayerClient::OnKickSelfOut(char* pData, size_t nSize, int nFrame)
{
    return;
}

//切换服务器的通知
void KPlayerClient::OnChangeGameServer(char* pData, size_t nSize, int nFrame)
{
	BOOL bRetCode = FALSE;
	G2C_CHANGE_GAME_SERVER ChangeGameServer;
	memcpy(&ChangeGameServer, pData, sizeof(G2C_CHANGE_GAME_SERVER));
	struct in_addr GameServerAddress;

	GameServerAddress.s_addr = ChangeGameServer.nIPAddr;
	char* pszGameServerIP = inet_ntoa(GameServerAddress);

	Disconnect();

	KScene* pScene = g_SO3World.GetScene(m_pOwner->m_dwClientMapID, 
		m_pOwner->m_dwClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	bRetCode = g_SO3World.DeleteScene(pScene, TRUE);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = Connect(pszGameServerIP, ChangeGameServer.wPort);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = DoApplyPlayerData(ChangeGameServer.szRoleName, ChangeGameServer.Guid);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:

	return;
}


void KPlayerClient::OnSyncRegionPlayer(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnSyncRegionNpc(char* pData, size_t nSize, int nFrame)
{
	BOOL                                     bRetCode                = FALSE;
	G2C_SYNC_REGION_NPC*                     pSyncData               = (G2C_SYNC_REGION_NPC*)pData;
    G2C_SYNC_REGION_NPC::KNPC_INFO*          pNpcData                = NULL;
    KScene*                                  pScene                  = NULL;
	KRegion*                                 pRegion                 = NULL;
    int                                      nSyncCount              = 0;    
    int                                      nFrameDiff              = m_pOwner->m_nGameLoop - nFrame;
      

	pScene = g_SO3World.GetScene(m_pOwner->m_dwClientMapID, m_pOwner->m_dwClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	pRegion   = pScene->GetRegion(pSyncData->byRegionX, pSyncData->byRegionY);

	KGLOG_PROCESS_ERROR(pRegion);
	KG_PROCESS_ERROR(m_pOwner->m_Eyeshot.IsVisible(pRegion));


	nSyncCount = (int)(nSize - sizeof(G2C_SYNC_REGION_NPC)) / sizeof(G2C_SYNC_REGION_NPC::KNPC_INFO);

	for (int nIndex = 0; nIndex < nSyncCount; nIndex++)
	{
        G2C_NPC_DISPLAY_DATA*    pNpcDisplayData    = NULL;
		KNpc*                    pNpc               = NULL;
        int                      nX                 = 0;
        int                      nY                 = 0;
        DWORD                    dwCheckSum         = 0;
        DWORD                    dwDisplayDataSize  = 0;

	    pNpcData = &pSyncData->NpcData[nIndex];

        pNpc = m_pOwner->m_Eyeshot.GetNpc(pNpcData->dwID);
		if (pNpc)
		{
			bRetCode = g_SO3World.RemoveNpc(pNpc);
			KGLOG_CHECK_ERROR(bRetCode);

			bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
			KGLOG_CHECK_ERROR(bRetCode);
		}

		pNpc = m_pOwner->m_Eyeshot.NewNpc(pNpcData->dwID);
		KGLOG_PROCESS_ERROR(pNpc);

        nX = (int)(pSyncData->byRegionX * REGION_GRID_WIDTH * CELL_LENGTH + pNpcData->uX);
        nY = (int)(pSyncData->byRegionY * REGION_GRID_HEIGHT * CELL_LENGTH + pNpcData->uY);
	
        pNpc->m_dwRepresentState             = pNpcData->wRepresentState;
        pNpc->m_nFaceDirection               = pNpcData->byFaceDirection;

		pNpc->m_eMoveState                   = (CHARACTER_MOVE_STATE)pNpcData->uMoveState;
		pNpc->m_nVelocityXY                  = pNpcData->uVelocityXY;
        pNpc->m_nDirectionXY                 = pNpcData->uDirectionXY;
        pNpc->m_bSheathFlag                  = pNpcData->uSheathFlag;

        pNpc->m_nVelocityZ                   = pNpcData->nVelocityZ;
        pNpc->m_nConvergenceSpeed            = pNpcData->uConvergenceSpeed;
        pNpc->m_nDestX                       = pNpcData->uDestX;
        pNpc->m_nDestY                       = pNpcData->uDestY;
        pNpc->m_nCurrentGravity              = pNpcData->uCurrentGravity;
        pNpc->m_nSwimAbility                 = pNpcData->uSwimAbility;
        pNpc->m_nWaterFlyAbility             = pNpcData->uWaterFlyAbility;
        pNpc->m_bFightState                  = pNpcData->uFightState;

		bRetCode = g_SO3World.AddNpc(pNpc, pScene, nX, nY, pNpcData->uZ);
		KGLOG_PROCESS_ERROR(bRetCode);

        pNpcDisplayData = (G2C_NPC_DISPLAY_DATA*)m_NpcDisplayDataCache.GetData(pNpc->m_dwID, dwDisplayDataSize);
        if (pNpcDisplayData)
        {
            pNpc->SetDisplayData(pNpcDisplayData->dwTemplateID, pNpcDisplayData->nLevel, pNpcDisplayData->dwRepresentID);  

	        dwCheckSum = CRC32(0, pNpcDisplayData, sizeof(G2C_NPC_DISPLAY_DATA));
        }
        
	    //bRetCode = DoApplyNpcDisplayData(pNpc, dwCheckSum);
	    //KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit0:

	return;
}

void KPlayerClient::OnSyncNewPlayer(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnSyncNewNpc(char* pData, size_t nSize, int nFrame)
{
	BOOL                         bRetCode                = FALSE;
	G2C_SYNC_NEW_NPC*            pNpcData             = (G2C_SYNC_NEW_NPC*)pData;
    KScene*                      pScene                  = NULL;
    KRegion*                     pRegion                 = NULL;
    KNpc*                        pNpc                 = NULL;
    G2C_NPC_DISPLAY_DATA*        pNpcDisplayData      = NULL;
    DWORD                        dwCheckSum              = 0;
    DWORD                        dwDisplayDataSize       = 0;


    m_ApplyNewCharacterList.erase(pNpcData->dwID);

	pScene = g_SO3World.GetScene(m_pOwner->m_dwClientMapID, m_pOwner->m_dwClientMapCopyIndex);
	KGLOG_PROCESS_ERROR(pScene);

	pRegion = pScene->GetRegionByPoint(pNpcData->uPosX, pNpcData->uPosY);

	KGLOG_PROCESS_ERROR(pRegion);
	KG_PROCESS_ERROR(m_pOwner->m_Eyeshot.IsVisible(pRegion));

	pNpc = m_pOwner->m_Eyeshot.GetNpc(pNpcData->dwID);
	if (pNpc)
	{
		bRetCode = g_SO3World.RemoveNpc(pNpc);
		KGLOG_CHECK_ERROR(bRetCode);

		bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
		KGLOG_CHECK_ERROR(bRetCode);
	}

	pNpc = m_pOwner->m_Eyeshot.NewNpc(pNpcData->dwID);
	KGLOG_PROCESS_ERROR(pNpc);

    pNpc->m_dwRepresentState             = pNpcData->wRepresentState;
    pNpc->m_nFaceDirection               = pNpcData->byFaceDirection;

	pNpc->m_eMoveState                   = (CHARACTER_MOVE_STATE)pNpcData->uMoveState;
    pNpc->m_nSwimAbility                 = pNpcData->uSwimAbility;
    pNpc->m_nWaterFlyAbility             = pNpcData->uWaterFlyAbility;
    pNpc->m_bFightState                  = pNpcData->uFightState;
    pNpc->m_bSheathFlag                  = pNpcData->uSheathFlag;

	pNpc->m_nVelocityXY                  = pNpcData->uVelocityXY;
    pNpc->m_nDirectionXY                 = pNpcData->uDirectionXY;
    pNpc->m_nVelocityZ                   = pNpcData->nVelocityZ;
    pNpc->m_nDestX                       = pNpcData->uDestX;
    pNpc->m_nDestY                       = pNpcData->uDestY;
    pNpc->m_nCurrentGravity              = pNpcData->uCurrentGravity;

    pNpc->m_nConvergenceSpeed            = pNpcData->uConvergenceSpeed;

	bRetCode = g_SO3World.AddNpc(pNpc, pScene, pNpcData->uPosX, pNpcData->uPosY, pNpcData->uPosZ);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    pNpcDisplayData = (G2C_NPC_DISPLAY_DATA*)m_NpcDisplayDataCache.GetData(pNpc->m_dwID, dwDisplayDataSize);
    if (pNpcDisplayData)
    {
        pNpc->SetDisplayData(pNpcDisplayData->dwTemplateID, pNpcDisplayData->nLevel, pNpcDisplayData->dwRepresentID);  
        dwCheckSum = CRC32(0, pNpcDisplayData, sizeof(G2C_NPC_DISPLAY_DATA));
    }
	else
	{
		// TODO: 表格默认模型
		pNpc->SetDisplayData(0, 99, 0);
	}

	//bRetCode = DoApplyNpcDisplayData(pNpc, dwCheckSum);
	//KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

//从场景中移除角色
void KPlayerClient::OnRemoveCharacter(char* pData, size_t nSize, int nFrame)
{
	BOOL bRetCode = FALSE;
	G2C_REMOVE_CHARACTER* pRemoveCharacter = (G2C_REMOVE_CHARACTER*)pData;

	DWORD dwCharacterID = pRemoveCharacter->dwCharacterID;

	if (g_IsPlayer(dwCharacterID))
	{
		KG_PROCESS_ERROR(dwCharacterID != m_pOwner->m_pRole->m_dwID);

		KPlayer* pPlayer = m_pOwner->m_Eyeshot.GetPlayer(dwCharacterID);
		KG_PROCESS_ERROR(pPlayer);

		bRetCode = g_SO3World.RemovePlayer(pPlayer);
		KG_PROCESS_ERROR(bRetCode);

		bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	else
	{
		KNpc* pNpc = m_pOwner->m_Eyeshot.GetNpc(dwCharacterID);
		KGLOG_PROCESS_ERROR(pNpc);

		bRetCode = g_SO3World.RemoveNpc(pNpc);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit0:

	return;
}

void KPlayerClient::OnReviseMoveState(char* pData, size_t nSize, int nFrame)
{
    BOOL                        bRetCode    = false;
    G2C_REVISE_MOVE_STATE*      pPak        = (G2C_REVISE_MOVE_STATE*)pData;
    KPlayer*                    pPlayer     = NULL;

    pPlayer = m_pOwner->m_pRole;
    KG_ASSERT_EXIT(pPlayer);

    //cprintf("Try move to (%d, %d, %d)\n", pPak->nX, pPak->nY, pPak->nZ);
    
    bRetCode = pPlayer->MoveTo(pPak->nX, pPak->nY, pPak->nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    //{
    //    int nPosZ = pPlayer->m_pCell->m_wHighLayer + pPlayer->m_nZ;
    //    cprintf("New position (%d, %d, %d)\n", pPlayer->m_nX, pPlayer->m_nY, nPosZ);
    //}

    pPlayer->m_eMoveState               = (CHARACTER_MOVE_STATE)pPak->cMoveState;
    pPlayer->m_nFaceDirection           = pPak->byFaceDirection;
    pPlayer->m_nDestX                   = pPak->nDestX;
    pPlayer->m_nDestY                   = pPak->nDestY;
    pPlayer->m_nVelocityXY              = pPak->nVelocityXY;
    pPlayer->m_nDirectionXY             = pPak->byDirectionXY;
    pPlayer->m_nVelocityZ               = pPak->nVelocityZ;
    pPlayer->m_nConvergenceSpeed        = pPak->nConvergenceSpeed;
    pPlayer->m_nCurrentGravity          = pPak->byCurrentGravity;
    pPlayer->m_nJumpCount               = pPak->cJumpCount;

    if (pPak->nFrame < m_pOwner->m_nGameLoop)
    {
        // 注意,这个ForwardPosition是必不可少的,否则会造成一旦发生位置错误,则可能永远错下去(移动状态对得上,但帧对不上)
        pPlayer->ForwardPosition(m_pOwner->m_nGameLoop - pPak->nFrame);
    }

Exit0:
    return;
}

void KPlayerClient::OnSyncMoveState(char* pData, size_t nSize, int nFrame)
{
    BOOL                 bRetCode   = false;
    G2C_SYNC_MOVE_STATE* pMoveState = (G2C_SYNC_MOVE_STATE*)pData;
    KCharacter*          pCharacter = NULL;

    bRetCode = g_IsPlayer(pMoveState->dwCharacterID);
    if (bRetCode)
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pMoveState->dwCharacterID);
    else
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pMoveState->dwCharacterID);

    if (!pCharacter)
    {
		DoApplyNewCharacterData(pMoveState->dwCharacterID);
        goto Exit0;
    }

    bRetCode = pCharacter->MoveTo(pMoveState->uPosX, pMoveState->uPosY, pMoveState->uPosZ);
    if (!bRetCode)
    {
        KGLOG_PROCESS_ERROR(pMoveState->dwCharacterID != m_pOwner->m_pRole->m_dwID);

        if (g_IsPlayer(pCharacter->m_dwID))
        {
	        KPlayer* pPlayer = (KPlayer*)pCharacter;
	        bRetCode = g_SO3World.RemovePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
	        KNpc* pNpc = (KNpc*)pCharacter;

	        bRetCode = g_SO3World.RemoveNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }

        goto Exit0;
    }

    pCharacter->m_nFaceDirection = pMoveState->uFaceDirection;

    pCharacter->m_eMoveState            = (CHARACTER_MOVE_STATE)pMoveState->uMoveState;
    pCharacter->m_nVelocityXY           = pMoveState->uVelocityXY;
    pCharacter->m_nDirectionXY          = pMoveState->uDirectionXY;      
    pCharacter->m_nVelocityZ            = pMoveState->nVelocityZ;        
    pCharacter->m_nConvergenceSpeed     = pMoveState->uConvergenceSpeed;
    pCharacter->m_nCurrentGravity       = pMoveState->uCurrentGravity;   
    pCharacter->m_nSwimAbility          = pMoveState->uSwimAbility;      
    pCharacter->m_nWaterFlyAbility      = pMoveState->uWaterFlyAbility;

Exit0:
    return;
}

void KPlayerClient::OnCharacterStand(char* pData, size_t nSize, int nFrame)
{
    BOOL                    bRetCode        = false;
    KCharacter*             pCharacter      = NULL;
    G2C_CHARACTER_STAND*    pCharacterStand = (G2C_CHARACTER_STAND*)pData;

    bRetCode = g_IsPlayer(pCharacterStand->dwCharacterID);
	if (bRetCode)
		pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pCharacterStand->dwCharacterID);
	else
		pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pCharacterStand->dwCharacterID);

	if (!pCharacter)
	{
		bRetCode = DoApplyNewCharacterData(pCharacterStand->dwCharacterID);
		KGLOG_PROCESS_ERROR(bRetCode);
		goto Exit0;
	}

    bRetCode = pCharacter->MoveTo(pCharacterStand->uSrcX, pCharacterStand->uSrcY, pCharacterStand->uSrcZ);
    if (!bRetCode)
    {
        KGLOG_PROCESS_ERROR(pCharacterStand->dwCharacterID != m_pOwner->m_pRole->m_dwID);
        if (g_IsPlayer(pCharacter->m_dwID))
        {
	        KPlayer* pPlayer = (KPlayer*)pCharacter;
	        bRetCode = g_SO3World.RemovePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
	        KNpc* pNpc = (KNpc*)pCharacter;

	        bRetCode = g_SO3World.RemoveNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }

        goto Exit0;
    }
   
    pCharacter->m_nFaceDirection    = pCharacterStand->uFaceDirection;
    pCharacter->m_nVelocityXY       = pCharacterStand->uVelocityXY;
    pCharacter->m_nDirectionXY      = pCharacterStand->uDirectionXY;
    pCharacter->m_nVelocityZ        = pCharacterStand->nVelocityZ;

    pCharacter->m_nConvergenceSpeed = 0;

    pCharacter->DoStand();
	
Exit0:
    return;
}

void KPlayerClient::OnCharacterWalk(char* pData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = false;
	G2C_CHARACTER_WALK* pCharacterWalk  = (G2C_CHARACTER_WALK*)pData;
    KCharacter*         pCharacter      = NULL;

    bRetCode = g_IsPlayer(pCharacterWalk->dwCharacterID);
    if (bRetCode)
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pCharacterWalk->dwCharacterID);
    else
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pCharacterWalk->dwCharacterID);

    if (!pCharacter)
    {
		DoApplyNewCharacterData(pCharacterWalk->dwCharacterID);
        goto Exit0;
    }


    bRetCode = pCharacter->MoveTo(pCharacterWalk->uSrcX, pCharacterWalk->uSrcY, pCharacterWalk->uSrcZ);
    if (!bRetCode)
    {
        KG_PROCESS_ERROR(pCharacterWalk->dwCharacterID != m_pOwner->m_pRole->m_dwID);

        if (g_IsPlayer(pCharacter->m_dwID))
        {
	        //KPlayer* pPlayer = (KPlayer*)pCharacter;
	        //bRetCode = g_SO3World.RemovePlayer(pPlayer);
	        //KGLOG_PROCESS_ERROR(bRetCode);

	        //bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
	        //KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
	        KNpc* pNpc = (KNpc*)pCharacter;

	        bRetCode = g_SO3World.RemoveNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }

        goto Exit0;
    }

   
    pCharacter->m_nFaceDirection    = pCharacterWalk->uFaceDirection;

    pCharacter->m_eMoveState        = (CHARACTER_MOVE_STATE)pCharacterWalk->uMoveState;
    pCharacter->m_nVelocityXY       = pCharacterWalk->uVelocityXY;
    pCharacter->m_nDirectionXY      = pCharacterWalk->uDirectionXY;
    pCharacter->m_nVelocityZ        = pCharacterWalk->nVelocityZ;
    pCharacter->m_nDestX            = pCharacterWalk->uDestX;
    pCharacter->m_nDestY            = pCharacterWalk->uDestY;

    pCharacter->m_nConvergenceSpeed = pCharacterWalk->uConvergenceSpeed;
	
Exit0:
	return;
}

void KPlayerClient::OnCharacterRun(char* pData, size_t nSize, int nFrame)
{ 
	BOOL                bRetCode        = false;
	G2C_CHARACTER_RUN*  pCharacterRun   = (G2C_CHARACTER_RUN*)pData;
    KCharacter*         pCharacter      = NULL;

    bRetCode = g_IsPlayer(pCharacterRun->dwCharacterID);
    if (bRetCode)
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pCharacterRun->dwCharacterID);
    else
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pCharacterRun->dwCharacterID);

    if (!pCharacter)
    {
		DoApplyNewCharacterData(pCharacterRun->dwCharacterID);
        goto Exit0;
    }


    bRetCode = pCharacter->MoveTo(pCharacterRun->uSrcX, pCharacterRun->uSrcY, pCharacterRun->uSrcZ);
    if (!bRetCode)
    {
        KG_PROCESS_ERROR(pCharacterRun->dwCharacterID != m_pOwner->m_pRole->m_dwID);
        if (g_IsPlayer(pCharacter->m_dwID))
        {
	     //   KPlayer* pPlayer = (KPlayer*)pCharacter;
	     //   bRetCode = g_SO3World.RemovePlayer(pPlayer);
	     //   KGLOG_PROCESS_ERROR(bRetCode);

		    //bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
		    //KGLOG_PROCESS_ERROR(bRetCode);
	    }
        else
        {
	        KNpc* pNpc = (KNpc*)pCharacter;

	        bRetCode = g_SO3World.RemoveNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }

        goto Exit0;
    }

   
    pCharacter->m_nFaceDirection    = pCharacterRun->uFaceDirection;

    pCharacter->m_eMoveState        = (CHARACTER_MOVE_STATE)pCharacterRun->uMoveState;
    pCharacter->m_nVelocityXY       = pCharacterRun->uVelocityXY;
    pCharacter->m_nDirectionXY      = pCharacterRun->uDirectionXY;
    pCharacter->m_nVelocityZ        = pCharacterRun->nVelocityZ;
    pCharacter->m_nDestX            = pCharacterRun->uDestX;
    pCharacter->m_nDestY            = pCharacterRun->uDestY;

    pCharacter->m_nConvergenceSpeed = pCharacterRun->uConvergenceSpeed;

Exit0:
	return;
}

void KPlayerClient::OnCharacterSwim(char* pData, size_t nSize, int nFrame)
{ 
	BOOL                bRetCode        = false;
	G2C_CHARACTER_SWIM* pCharacterSwim   = (G2C_CHARACTER_SWIM*)pData;
    KCharacter*         pCharacter      = NULL;

    bRetCode = g_IsPlayer(pCharacterSwim->dwCharacterID);
    if (bRetCode)
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pCharacterSwim->dwCharacterID);
    else
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pCharacterSwim->dwCharacterID);

    if (!pCharacter)
    {
		DoApplyNewCharacterData(pCharacterSwim->dwCharacterID);
        goto Exit0;
    }


    bRetCode = pCharacter->MoveTo(pCharacterSwim->uSrcX, pCharacterSwim->uSrcY, pCharacterSwim->uSrcZ);
    if (!bRetCode)
    {
        KG_PROCESS_ERROR(pCharacterSwim->dwCharacterID != m_pOwner->m_pRole->m_dwID);

        if (g_IsPlayer(pCharacter->m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)pCharacter;
            bRetCode = g_SO3World.RemovePlayer(pPlayer);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
            KNpc* pNpc = (KNpc*)pCharacter;

            bRetCode = g_SO3World.RemoveNpc(pNpc);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

        goto Exit0;
    }

    pCharacter->m_nFaceDirection    = pCharacterSwim->uFaceDirection;
   
    pCharacter->m_eMoveState        = (CHARACTER_MOVE_STATE)pCharacterSwim->uMoveState;
    pCharacter->m_nVelocityXY       = pCharacterSwim->uVelocityXY;
    pCharacter->m_nDirectionXY      = pCharacterSwim->uDirectionXY;
    pCharacter->m_nVelocityZ        = pCharacterSwim->nVelocityZ;
    pCharacter->m_nDestX            = pCharacterSwim->uDestX;
    pCharacter->m_nDestY            = pCharacterSwim->uDestY;

    pCharacter->m_nConvergenceSpeed = pCharacterSwim->uConvergenceSpeed;
    pCharacter->m_nPitchDirection   = pCharacterSwim->uPitchDirection;
	
Exit0:
	return;
}


void KPlayerClient::OnCharacterJump(char* pData, size_t nSize, int nFrame)
{
	BOOL                        bRetCode                = FALSE;
    KCharacter*                 pCharacter              = NULL;
	G2C_CHARACTER_JUMP*         pCharacterJump          = (G2C_CHARACTER_JUMP*)pData;

	if (g_IsPlayer(pCharacterJump->dwCharacterID))
		pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pCharacterJump->dwCharacterID);
	else
		pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pCharacterJump->dwCharacterID);

    if (!pCharacter)
    {
		DoApplyNewCharacterData(pCharacterJump->dwCharacterID);
        goto Exit0;
    }


    bRetCode = pCharacter->MoveTo(pCharacterJump->uSrcX, pCharacterJump->uSrcY, pCharacterJump->uSrcZ);
    if (!bRetCode)
    {
        KG_PROCESS_ERROR(pCharacterJump->dwCharacterID != m_pOwner->m_pRole->m_dwID);

        if (g_IsPlayer(pCharacter->m_dwID))
        {
	        KPlayer* pPlayer = (KPlayer*)pCharacter;
	        bRetCode = g_SO3World.RemovePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);

		        bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
		        KGLOG_PROCESS_ERROR(bRetCode);
	        }
	        else
	        {
		        KNpc* pNpc = (KNpc*)pCharacter;

		        bRetCode = g_SO3World.RemoveNpc(pNpc);
		        KGLOG_PROCESS_ERROR(bRetCode);

		        bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
		        KGLOG_PROCESS_ERROR(bRetCode);
	        }

            goto Exit0;
    }

    pCharacter->m_nFaceDirection    = pCharacterJump->uFaceDirection;
    pCharacter->m_eMoveState        = (CHARACTER_MOVE_STATE)pCharacterJump->uMoveState;
    pCharacter->m_nVelocityXY       = pCharacterJump->uVelocityXY;
    pCharacter->m_nDirectionXY      = pCharacterJump->uDirectionXY;
    pCharacter->m_nVelocityZ        = pCharacterJump->nVelocityZ;
    pCharacter->m_nDestX            = pCharacterJump->uDestX;
    pCharacter->m_nDestY            = pCharacterJump->uDestY;

    pCharacter->m_nConvergenceSpeed = pCharacterJump->uConvergenceSpeed;

Exit0:
	return;
}

void KPlayerClient::OnCharacterSit(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnCharacterMeditate(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnCharacterDash(char* pData, size_t nSize, int nFrame)
{
	return;
}


void KPlayerClient::OnCharacterKnockedDown(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnCharacterKnockedBack(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnCharacterKnockedOff(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnCharacterStun(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnCharacterHalt(char* pData, size_t nSize, int nFrame)
{
    return;
}

// 角色死亡
void KPlayerClient::OnCharacterDeath(char* pData, size_t nSize, int nFrame)
{
	BOOL                     bRetCode        = FALSE;
	G2C_CHARACTER_DEATH*     pMoveCommand    = (G2C_CHARACTER_DEATH*)pData;
	KCharacter*              pKiller         = NULL;
    KCharacter*              pCharacter      = NULL;


	if (g_IsPlayer(pMoveCommand->dwCharacterID))
		pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pMoveCommand->dwCharacterID);
	else
		pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pMoveCommand->dwCharacterID);

	if (!pCharacter)
	{
		DoApplyNewCharacterData(pMoveCommand->dwCharacterID);
		goto Exit0;
	}

    bRetCode = pCharacter->MoveTo(pMoveCommand->uSrcX, pMoveCommand->uSrcY, pMoveCommand->uSrcZ);
    if (!bRetCode)
    {
        KG_PROCESS_ERROR(pMoveCommand->dwCharacterID != m_pOwner->m_pRole->m_dwID);

        if (g_IsPlayer(pCharacter->m_dwID))
        {
	        KPlayer* pPlayer = (KPlayer*)pCharacter;
	        bRetCode = g_SO3World.RemovePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeletePlayer(pPlayer);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
	        KNpc* pNpc = (KNpc*)pCharacter;

	        bRetCode = g_SO3World.RemoveNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);

	        bRetCode = m_pOwner->m_Eyeshot.DeleteNpc(pNpc);
	        KGLOG_PROCESS_ERROR(bRetCode);
        }

        goto Exit0;
    }

    pCharacter->m_nFaceDirection          = pMoveCommand->uFaceDirection;
    pCharacter->m_nConvergenceSpeed       = 0;
    if (pCharacter == (KCharacter *)m_pOwner->m_pRole)
    {
        m_pOwner->m_pRole->m_ReviveParam.nLastSituReviveTime = time(NULL) + pMoveCommand->dwPunishTime;
    }

    pCharacter->DoDeath(0);
    //if (pCharacter->m_dwID == m_pOwner->m_pRole->m_dwID)
    //{
        //if (((KPlayer*)pCharacter)->m_pTradingBox)
        //{
        //    DoTradingConfirm(false, 0);
        //}
    //}
    
	//if (g_IsPlayer(pMoveCommand->dwKillerID))
	//{
	//	pKiller = m_pOwner->m_Eyeshot.GetPlayer(pMoveCommand->dwKillerID);
	//}
	//else
	//{
	//	pKiller = m_pOwner->m_Eyeshot.GetNpc(pMoveCommand->dwKillerID);
	//}

	//if (!g_IsPlayer(pMoveCommand->dwCharacterID))
	//{
		//KNpc* pNpc = m_pOwner->m_Eyeshot.GetNpc(pMoveCommand->dwCharacterID);

		//KG_PROCESS_ERROR(pNpc);

		// 调用队友的任务接口
		//if (pKiller && g_IsPlayer(pKiller->m_dwID))
		//{
		//	if (((KPlayer*)pKiller)->m_dwPartyID != INVALID_PARTY_ID)
		//	{
		//		const KPARTY_INFO* pcPartyInfo = g_PartyCache.GetParty(((KPlayer*)pKiller)->m_dwPartyID);

		//		KGLOG_PROCESS_ERROR(pcPartyInfo);

		//		for (KPARTY_PLAYER_LIST::const_iterator it = pcPartyInfo->PlayerList.begin();
		//			it != pcPartyInfo->PlayerList.end(); ++it)
		//		{
		//			KPlayer* pPlayer = g_ObjectManager.m_PlayerSet.GetObj(it->dwMemberID);
		//			if (pPlayer)
		//			{
		//				pPlayer->m_QuestList.OnKillNpc(pNpc);
		//			}
		//		}
		//	}
		//	else
		//	{
		//		((KPlayer*)pKiller)->m_QuestList.OnKillNpc(pNpc);
		//	}
		//}
	//}

Exit0:
	return;
}

// 移动角色的位置
void KPlayerClient::OnMoveCharacter(char* pData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = false;
	G2C_MOVE_CHARACTER* pMoveCharacter  = (G2C_MOVE_CHARACTER*)pData;
    KCharacter*         pCharacter      = NULL;

    if (g_IsPlayer(pMoveCharacter->dwID))
        pCharacter = m_pOwner->m_Eyeshot.GetPlayer(pMoveCharacter->dwID);
    else
        pCharacter = m_pOwner->m_Eyeshot.GetNpc(pMoveCharacter->dwID);

    KG_PROCESS_ERROR(pCharacter);

	if (pMoveCharacter->dwMapID == m_pOwner->m_dwClientMapID)
	{
        pCharacter->m_nFaceDirection    = pMoveCharacter->byFaceDirection;
        pCharacter->m_nDirectionXY      = pMoveCharacter->uDirectionXY;
        pCharacter->m_nVelocityXY       = pMoveCharacter->uVelocityXY;
        pCharacter->m_nVelocityZ        = pMoveCharacter->nVelocityZ;
        pCharacter->m_nConvergenceSpeed = pMoveCharacter->uConvergenceSpeed;
        pCharacter->m_eMoveState        = (CHARACTER_MOVE_STATE)pMoveCharacter->uMoveState;

        bRetCode = pCharacter->MoveTo(pMoveCharacter->uPosX, pMoveCharacter->uPosY, pMoveCharacter->uPosZ);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	else	
	{
        // 在原服务器上跨地图, 注意: 跨服务器时不走这个协议
		KScene* pScene = g_SO3World.GetScene(m_pOwner->m_dwClientMapID, m_pOwner->m_dwClientMapCopyIndex);
		KGLOG_PROCESS_ERROR(pScene);

        KPlayer *pClientPlayer = m_pOwner->m_pRole;
        KGLOG_PROCESS_ERROR(pClientPlayer);

		bRetCode = pScene->RemovePlayer(pClientPlayer);
		KGLOG_PROCESS_ERROR(bRetCode);

		//bRetCode = g_SO3World.DeleteScene(pScene, TRUE);
		//KGLOG_PROCESS_ERROR(bRetCode);

		m_pOwner->m_dwClientMapID = pMoveCharacter->dwMapID;
		m_pOwner->m_dwClientMapCopyIndex = pMoveCharacter->dwMapCopyIndex;

        pScene = g_SO3World.GetScene(pMoveCharacter->dwMapID, pMoveCharacter->dwMapCopyIndex);
        KGLOG_PROCESS_ERROR(pScene);

		//分配Region
		int nRegionX = (DWORD)(pMoveCharacter->uPosX / CELL_LENGTH / REGION_GRID_WIDTH);
		int nRegionY = (DWORD)(pMoveCharacter->uPosY / CELL_LENGTH / REGION_GRID_HEIGHT);

        bRetCode = pScene->ClientLoadRegion(nRegionX, nRegionY);
        KGLOG_PROCESS_ERROR(bRetCode);

		pClientPlayer->m_nX                = pMoveCharacter->uPosX;
		pClientPlayer->m_nY                = pMoveCharacter->uPosY;
		pClientPlayer->m_nZ                = pMoveCharacter->uPosZ;
		pClientPlayer->m_nXCell            = (int)(pMoveCharacter->uPosX / CELL_LENGTH % REGION_GRID_WIDTH);
		pClientPlayer->m_nYCell            = (int)(pMoveCharacter->uPosY / CELL_LENGTH % REGION_GRID_HEIGHT);

        pClientPlayer->m_nFaceDirection    = pMoveCharacter->byFaceDirection;
        pClientPlayer->m_nDirectionXY      = pMoveCharacter->uDirectionXY;
        pClientPlayer->m_nVelocityXY       = pMoveCharacter->uVelocityXY;
        pClientPlayer->m_nVelocityZ        = pMoveCharacter->nVelocityZ;
        pClientPlayer->m_nConvergenceSpeed = pMoveCharacter->uConvergenceSpeed;
        pClientPlayer->m_eMoveState        = (CHARACTER_MOVE_STATE)pMoveCharacter->uMoveState;

		bRetCode = pScene->AddPlayer(pClientPlayer);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit0:
    return;
}

void KPlayerClient::OnPlayerRevivedNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

// 同步角色转向
void KPlayerClient::OnCharacterTurn(char* pData, size_t nSize, int nFrame)
{
    BOOL                bRetCode        = false;
	G2C_CHARACTER_TURN* pCharacterTurn  = (G2C_CHARACTER_TURN*)pData;
    KCharacter*         pCharacter      = NULL;

    KG_PROCESS_ERROR(pCharacterTurn->dwCharacterID != m_pOwner->m_pRole->m_dwID);

    if (g_IsPlayer(pCharacterTurn->dwCharacterID))
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pCharacterTurn->dwCharacterID);
    else
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pCharacterTurn->dwCharacterID);
    
	if (!pCharacter)
	{
		DoApplyNewCharacterData(pCharacterTurn->dwCharacterID);
		goto Exit0;
	}

    pCharacter->Turn(pCharacterTurn->byDirection);

Exit0:
    return;
}

//同步角色动作
void KPlayerClient::OnCharacterAction(char* pData, size_t nSize, int nFrame)
{
	return;
}

//同步角色说话
void KPlayerClient::OnCharacterSay(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnOverTimeAction(char* pData, size_t nSize, int nFrame)
{
    return;
}

//角色显示数据
void KPlayerClient::OnPlayerDisplayData(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnNpcDisplayData(char* pData, size_t nSize, int nFrame)
{
}

//增加经验值
void KPlayerClient::OnAddExperience(char* pData, size_t nSize, int nFrame)
{
    BOOL                     bRetCode        = FALSE;
    G2C_ADD_EXPERIENCE*      pAddExperience  = (G2C_ADD_EXPERIENCE*)pData;

    KPlayer* pPlayer = m_pOwner->m_pRole;
    KGLOG_PROCESS_ERROR(pPlayer);

    pPlayer->AddExp(pAddExperience->nExpIncrement,pAddExperience->bUseDoubleExp);

Exit0:

    return;
}

void KPlayerClient::OnCommonHealth(char* pData, size_t nSize, int nFrame)
{
    G2C_COMMON_HEALTH*          pPak        = (G2C_COMMON_HEALTH*)pData;    
    KCharacter*                 pCharacter  = NULL;

    if (g_IsPlayer(pPak->dwCharacterID))
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(pPak->dwCharacterID);
    else
        pCharacter = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(pPak->dwCharacterID);

    if (!pCharacter)
    {
        DoApplyNewCharacterData(pPak->dwCharacterID);
        goto Exit0;
    }

Exit0:
    return;
}

// Doodad状态同步数据
void KPlayerClient::OnSyncDoodadState(char* pData, size_t nSize, int nFrame)
{
	return;
}

//帧间隔信号
void KPlayerClient::OnFrameSignal(char* pData, size_t nSize, int nFrame)
{
	BOOL                bRetCode        = FALSE;
	G2C_FRAME_SIGNAL*   pFrameSignal    = (G2C_FRAME_SIGNAL*)pData;
	APPLY_NEW_CHARACTER_LIST::iterator it;

	m_nGSGameLoop = pFrameSignal->nGameLoop;

	it = m_ApplyNewCharacterList.begin();
	while (it != m_ApplyNewCharacterList.end())
	{
		if (it->second == 0)
		{
			C2G_APPLY_NEW_CHARACTER_DATA ApplyNewCharacterData;

			ApplyNewCharacterData.byProtocolID = c2g_apply_new_character_data;
			ApplyNewCharacterData.dwCharacterID = it->first;

			bRetCode = Send(&ApplyNewCharacterData, sizeof(ApplyNewCharacterData));
			KGLOG_PROCESS_ERROR(bRetCode);

			it->second = nFrame;

			break;
		}
        else if (nFrame - it->second > GAME_FPS * 2)
        {
			m_ApplyNewCharacterList.erase(it);
			break;
        }

		++it;
	}

Exit0:
	return;
}

void KPlayerClient::OnTerrainTest(char* pData, size_t nSize, int nFrame)
{
    return;
}

// ------------------- skill about ------------------------->

//同步技能数据
void KPlayerClient::OnSyncSkillData(char* pData, size_t nSize, int nFrame)
{
	return;
}

//角色释放技能数据
void KPlayerClient::OnSkillPrepare(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnSkillCast(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncCommonSkillAutoCastState(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnResetSkillCooldown(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSkillEffectResult(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncBehitRepresent(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncFightState(char* pData, size_t nSize, int nFrame)
{
    return;
}


void KPlayerClient::OnSyncBuffList(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnSyncBuffSingle(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncRepresentState(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncKungfuMount(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSkillBeatBack(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncSheathState(char* pData, size_t nSize, int nFrame)
{
    return;
}

// ------------------- 数值同步 -----------------------------

void KPlayerClient::OnSyncSelfMaxLMRS(char* pData, size_t nSize, int nFrame)
{
    G2C_SYNC_SELF_MAX_LMRS*     pPak    = (G2C_SYNC_SELF_MAX_LMRS*)pData;
    KPlayer*                    pSelf   = NULL;

    pSelf = m_pOwner->m_pRole;
    KGLOG_PROCESS_ERROR(pSelf);

    pSelf->m_nMaxLife     = pPak->nMaxLife;
    pSelf->m_nMaxMana     = pPak->nMaxMana;
    pSelf->m_nMaxRage     = pPak->nMaxRage;   

Exit0:
    return;
}

void KPlayerClient::OnSyncSelfCurrentLMRS(char* pData, size_t nSize, int nFrame)
{
    G2C_SYNC_SELF_CURRENT_LMRS* pPak    = (G2C_SYNC_SELF_CURRENT_LMRS*)pData;
    KPlayer*                    pSelf   = NULL;

    pSelf = m_pOwner->m_pRole;
    KGLOG_PROCESS_ERROR(pSelf);

    pSelf->m_nCurrentLife     = pPak->nCurrentLife;
    pSelf->m_nCurrentMana     = pPak->nCurrentMana;
    pSelf->m_nCurrentRage     = pPak->nCurrentRage;

Exit0:
    return;
}

void KPlayerClient::OnSyncTargetMaxLMR(char* pData, size_t nSize, int nFrame)
{
    G2C_SYNC_TARGET_MAX_LMR*    pPak            = (G2C_SYNC_TARGET_MAX_LMR*)pData;
    KCharacter*                 pTarget         = NULL;
    int                         nLifePercent    = 0;
    int                         nManaPercent    = 0;
    int                         nRagePercent    = 0;

    if (g_IsPlayer(pPak->dwCharacterID))
        pTarget = m_pOwner->m_Eyeshot.GetPlayer(pPak->dwCharacterID);
    else
        pTarget = m_pOwner->m_Eyeshot.GetNpc(pPak->dwCharacterID);

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

Exit0:
    return;
}

void KPlayerClient::OnSyncTargetCurrentLMR(char* pData, size_t nSize, int nFrame)
{
    G2C_SYNC_TARGET_CURRENT_LMR*    pPak    = (G2C_SYNC_TARGET_CURRENT_LMR*)pData;
    KCharacter*                     pTarget = NULL;

    if (g_IsPlayer(pPak->dwCharacterID))
        pTarget = m_pOwner->m_Eyeshot.GetPlayer(pPak->dwCharacterID);
    else
        pTarget = m_pOwner->m_Eyeshot.GetNpc(pPak->dwCharacterID);

    KG_PROCESS_ERROR(pTarget);

    pTarget->m_nCurrentLife = pTarget->m_nMaxLife * pPak->byLifePercent / UCHAR_MAX;
    pTarget->m_nCurrentMana = pTarget->m_nMaxMana * pPak->byManaPercent / UCHAR_MAX;
    pTarget->m_nCurrentRage = pTarget->m_nMaxRage * pPak->byRagePercent / UCHAR_MAX;

Exit0:
    return;
}

void KPlayerClient::OnSyncPartyMemberMaxLMR(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncPartyMemberCurrentLMR(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncPartyMemberMisc(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncPartyMemberCoordinate(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnBroadcastCharacterLife(char* pData, size_t nSize, int nFrame)
{
	return;
}


// ------------------- party about ---------------------------

void KPlayerClient::OnSyncPlayerPartyBaseInfo(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncPlayerPartyMemberInfo(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnInvitePlayerJoinPartyRequest(char* pData, size_t nSize, int nFrame)
{
    return;
}
    
void KPlayerClient::OnInvitePlayerJoinPartyRespond(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnApplyJoinPartyRequest(char* pData, size_t nSize, int nFrame)
{
    return;
}
    
void KPlayerClient::OnApplyJoinPartyRespond(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnPartyCreateNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnPlayerJoinPartyNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}
    
void KPlayerClient::OnPartyAddMemberNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}
    
void KPlayerClient::OnPartyDelMemberNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncPartyLeaderFlag(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnPartyChangeLeaderNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnPartySetLootModeNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnPartyDisbandNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncPartyMemberOnlineFlag(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnPartyMemberSignpostNotify(char* pData, size_t nSize, int nFrame)
{
}
// <------------------- party about -------------------------



// ------------------- shop about ------------------------->

void KPlayerClient::OnOpenShopNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}


void KPlayerClient::OnUpdateShopItem(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnShopBuyRespond(char* pData, size_t nSize, int nFrame)
{
    return;
}

// <------------------- shop about -------------------------

// -------------------- Trading ----------------------------->
void KPlayerClient::OnTradingInviteRequest(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnOpenTradingBoxNotify(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncTradingConfirm(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncTradingItemData(char* pData, size_t nSize, int nFrame)
{
    return;
}

void KPlayerClient::OnSyncTradingMoney(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnTradingComplete(char* pData, size_t nSize, int nFrame)
{
    return;
}

// <--------------------- Trading ---------------------------------

// 同步金钱数量
void KPlayerClient::OnSyncMoney(char* pData, size_t nSize, int nFrame)
{
	return;
}

//同步临时的道具数据
void KPlayerClient::OnSyncTempItemData(char* pData, size_t nSize, int nFrame)
{
	return;
}
// 同步道具数据
void KPlayerClient::OnSyncItemData(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步道具删除
void KPlayerClient::OnSyncItemRemove(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 交换装备
void KPlayerClient::OnExchangeItem(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 换装广播
void KPlayerClient::OnSyncEquipRepresent(char* pData, size_t nSize, int nFrame)
{
    return;
}

// 更新物品数量或耐久
void KPlayerClient::OnUpdateItemAmount(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 学习生活技能
void KPlayerClient::OnLearnProfessionNotify(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 学习配方
void KPlayerClient::OnLearnRecipeNotify(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 遗忘技能
void KPlayerClient::OnForgetProfessionNotify(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 遗忘配方
void KPlayerClient::OnForgetRecipeNotify(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 增加生活技能经验
void KPlayerClient::OnAddProfessionProficiency(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 设置生活技能最大等级
void KPlayerClient::OnSetMaxProfessionLevelNotify(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步更新玩家生活技能
void KPlayerClient::OnUpdatePlayerProfession(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步更新玩家配方列表
void KPlayerClient::OnUpdatePlayerRecipe(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnCastRecipe(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 打开一个客户端窗口
void KPlayerClient::OnOpenWindow(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步配方的使用
void KPlayerClient::OnStartUseRecipe(char* pData, size_t nSize, int nFrame)
{
	return;
}


// 结束技能释放
void KPlayerClient::OnEndCast(char* pData, size_t nSize, int nFrame)
{
    return;
}

// 通知客户端接受任务
void KPlayerClient::OnAcceptQuest(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 完成任务
void KPlayerClient::OnFinishQuest(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 通知客户端取消任务
void KPlayerClient::OnCancelQuest(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 共享任务
void KPlayerClient::OnShareQuest(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步任务变量
void KPlayerClient::OnSyncQuestValue(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步任务共享的结果
void KPlayerClient::OnShareQuestResult(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步拾取列表
void KPlayerClient::OnSyncLootList(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 掷骰子信息
void KPlayerClient::OnRollItemMessage(char* pData, size_t nSize, int nFrame)
{

	return;
}

// 角色拾取物品
void KPlayerClient::OnCharacterPick(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 打开拾取面板
void KPlayerClient::OnOpenLootList(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步战斗开关
void KPlayerClient::OnSyncFightFlag(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步声望
void KPlayerClient::OnSyncRepute(char* pData, size_t nSize, int nFrame)
{
	return;
}


// 同步所有势力战斗开关
void KPlayerClient::OnSyncFightflagList(char* pData, size_t nSize, int nFrame)
{
	return;
}


// 同步所有的势力声望
void KPlayerClient::OnSyncReputeList(char* pData, size_t nSize, int nFrame)
{

	return;
}

// 
void KPlayerClient::OnCharacterOpen(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 组队广播金钱的拾取
void KPlayerClient::OnRollMoneyMessage(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 结束拾取
void KPlayerClient::OnFinishLoot(char* pData, size_t nSize, int nFrame)
{
	return;
}

// start roll
void KPlayerClient::OnBeginRollItem(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 同步定制的用户消息
void KPlayerClient::OnCustomOTAction(char* pData, size_t nSize, int nFrame)
{
	return;
}



// 添加仇恨
void KPlayerClient::OnModifyThreat(char* pData, size_t nSize, int nFrame)
{
    return;
}


// 修改被仇恨
void KPlayerClient::OnModifyThreaten(char* pData, size_t nSize, int nFrame)
{
}


// 清除仇恨
void KPlayerClient::OnClearThreat(char* pData, size_t nSize, int nFrame)
{
	return;
}

// 清除被仇恨
void KPlayerClient::OnClearThreaten(char* pData, size_t nSize, int nFrame)
{
}

// 收到 GameServer 发来的密聊信息，交 UI 显示
void KPlayerClient::OnPlayerTalkToOne(char* pData, size_t nSize, int nFrame)
{
    return;
}

// 收到 GameServer 发来的群聊信息，交 UI 显示
void KPlayerClient::OnPlayerTalkToMany(char* pData, size_t nSize, int nFrame)
{
    return;
}


// 同步双倍经验
void KPlayerClient::OnUpdateDoubleExp(char* pData, size_t nSize, int nFrame)
{
	return;
}


// 
void KPlayerClient::OnUpdatePlayerStamina(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnUpdatePlayerQi(char* pData, size_t nSize, int nFrame)
{
    return;
}

// 
void KPlayerClient::OnDoodadLootOver(char* pData, size_t nSize, int nFrame)
{
	return;
}

void KPlayerClient::OnPlayerAddRelationRespond(char* pData, size_t nSize, int nFrame)
{
    return ;
}

void KPlayerClient::OnPlayerDelRelationRespond(char* pData, size_t nSize, int nFrame)
{
    return ;
}
//AutoCode:-处理协议函数结束-
