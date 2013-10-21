/************************************************************************/
/* GS和客户端之间协议定义					                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* Modify	 : Spe														*/
/* Remark	 : 不要随意修改分隔区块的注释，在自动生成代码的宏中需要使用 */
/* History	 :															*/
/*		2005.01.27	Create												*/
/************************************************************************/
#ifndef _GS_CLIENT_PROTOCOL_H_
#define _GS_CLIENT_PROTOCOL_H_

#include "Global.h"
#include "SO3ProtocolBasic.h"


//协议号定义
//GS 2 Client Protocol Begin
enum GS_CLIENT_PROTOCOL
{
    gs_client_connection_begin = 0,

    g2c_response_message,			// 应答消息
    g2c_frame_signal,				// 帧间隔
    g2c_terrain_test,               // 测试协议,用于检测 服务端<==>客户端 障碍信息的一致性

    // 登陆＆跨服＆踢号的相关协议
    g2c_sync_player_base_info,	    // 登录时同步玩家数据
    g2c_sync_quest_state,			// 登录时同步任务状态
    g2c_sync_quest_list,			// 登陆时同步任务列表

    g2c_kick_self_out,				// 踢号通知
    g2c_change_game_server,			// 跨服通知

    // 同步相关协议
    g2c_sync_region_player,	        // 将一个Region中的所有玩家同步到客户端
    g2c_sync_region_npc,	        // 将一个Region中的所有玩家同步到客户端
    g2c_sync_new_player,	        // 通知客户端: 一个玩家进入Region了
    g2c_sync_new_npc,	            // 通知客户端: 一个npc进入Region了

    g2c_move_character,				// 移动角色的位置
    g2c_remove_character,			// 从场景中移除角色

    g2c_sync_doodad_state,			// Doodad状态同步

    g2c_player_display_data,		// 玩家显示数据
    g2c_npc_display_data,			// Npc显示数据

    g2c_add_experience,				// 增加经验值
    g2c_common_health,              // 普通伤害或者增益,比如摔伤之类的

    // 角色移动同步
    g2c_revise_move_state,          // 修正客户端玩家的移动状态
    g2c_sync_move_state,            // 客户端发出移动指令的时候,如果服务端发现坐标不对,则通知客户端修正

    g2c_character_stand,
    g2c_character_walk,				// 角色走动的指令同步数据包
    g2c_character_run,				// 角色跑动的指令同步数据包
    g2c_character_swim,				// 角色游泳的指令同步数据包
    g2c_character_jump,				// 角色跳跃的指令同步数据包
    g2c_character_sit,
    g2c_character_meditate,         // 打坐同步

    g2c_character_action,			// 角色交互动作的同步数据包
    g2c_character_dash,
    g2c_character_knocked_down,
    g2c_character_knocked_back,
    g2c_character_knocked_off,
    g2c_character_stun,
    g2c_character_halt,
    g2c_character_death,			// 角色死亡的同步数据包
    g2c_player_revived_notify,      // 玩家被复活的同步数据包
    g2c_character_say,				// 角色说话的同步数据包
    g2c_character_turn,				// 同步角色转向
    g2c_over_time_action,

    // ------------- skill about protocol -------------------
    g2c_sync_skill_data,			//登录或者学习新技能时同步技能数据
    g2c_skill_prepare,			    
    g2c_skill_cast,
    g2c_sync_common_skill_auto_cast_state, // 新手技能自动释放状态
    g2c_reset_skill_cooldown,
    g2c_skill_effect_result,
    g2c_sync_behit_represent,
    g2c_sync_fight_state,
    g2c_sync_buff_list,		
    g2c_sync_buff_single,		
    g2c_sync_represent_state,
    g2c_sync_kungfu_mount,
    g2c_skill_beat_back,
    g2c_sync_sheath_state,

    // ------------- 数值同步 -----------------------------

    g2c_sync_self_max_lmrs,
    g2c_sync_self_current_lmrs,
    g2c_sync_target_max_lmr,
    g2c_sync_target_current_lmr,
    g2c_sync_party_member_max_lmr,
    g2c_sync_party_member_current_lmr,
    g2c_sync_party_member_misc,
    g2c_sync_party_member_coordinate,
    g2c_broadcast_character_life,

    // ------------- party about protocol -----------------
    g2c_sync_player_party_base_info,    // 玩家刚刚登陆游戏的时候同步其队伍的基本信息: ID,队长信息,分配方式 ... ...
    g2c_sync_player_party_member_info,  // 玩家刚刚登陆游戏的时候同步其队伍的成员列表,一次只发送一个队员的信息
    g2c_invite_join_party_request,     
    g2c_invite_join_party_respond,
    g2c_apply_join_party_request,
    g2c_apply_join_party_respond,
    g2c_party_create_notify,            // 通知队长"你建立了一支队伍",原来这里三个协议是一个,但是后来发现这样会把一些操作写得很复杂:)
    g2c_player_join_party_notify,       // 通知玩家，他（自己）加入某队伍
    g2c_party_add_member_notify,        // 通知队伍所有成员"某人加入了我们的队伍"
    g2c_party_del_member_notify,
    g2c_sync_party_leader_flag,         // 通知设置玩家的队长旗标
    g2c_party_change_leader_notify,
    g2c_party_set_distribute_mode_notify,
    g2c_party_disband_notify,
    g2c_sync_party_member_online_flag,
    g2c_party_member_signpost_notify,   // 通知队友小地图上的标记
    // <------------- party about protocol -------------

    // ------------- shop about protocol ------------->
    g2c_open_shop_notify,
    g2c_update_shop_item,
    g2c_shop_buy_respond,
    // <------------- shop about protocol -------------
    
    // -------------- Trading about ------------------>
    g2c_trading_invite_request,
    g2c_open_trading_box_notify,
    g2c_sync_trading_confirm,
    g2c_sync_trading_item_data,    
    g2c_sync_trading_money,
    g2c_trading_complete,
    // <------------- Trading about ------------------

    g2c_sync_money,					//同步金钱数量
	g2c_sync_temp_item_data,		//同步临时道具数据
    g2c_sync_item_data,				//同步道具数据
    g2c_sync_item_remove,			//同步道具删除
    g2c_exchange_item,				//交换装备
    g2c_update_item_amount,			//更新物品数量或耐久
    g2c_sync_equip_represent,       //换装广播

    g2c_learn_profession_notify,	//学习生活技能
    g2c_learn_recipe_notify,		//学习配方
    g2c_forget_profession_notify,		//遗忘技能
    g2c_forget_recipe_notify,		//遗忘配方

    g2c_add_profession_proficiency,		//增加生活技能经验
    g2c_set_max_profession_level_notify,//设置生活技能最大等级
    g2c_update_player_profession,		//同步更新玩家生活技能
    g2c_update_player_recipe,			//同步更新玩家配方列表
    g2c_cast_recipe,

    g2c_open_window,				//打开一个客户端窗口

    g2c_start_use_recipe,			//同步配方的使用
    g2c_end_cast,					//结束技能释放

    g2c_accept_quest,				//通知客户端接受任务
    g2c_finish_quest,				//通知客户端完成任务
    g2c_cancel_quest,				//通知客户端取消任务
    g2c_share_quest,				//共享任务
    g2c_sync_quest_value,			//同步任务变量
	g2c_share_quest_result,			//同步任务共享的结果

    g2c_sync_loot_list,				//同步拾取列表
    g2c_roll_item_message,			//掷骰子信息
    g2c_character_pick,				//角色拾取物品
    g2c_open_loot_list,				//打开拾取面板
	g2c_sync_fight_flag,			//同步战斗开关
	g2c_sync_repute,				//同步声望
	g2c_sync_fightflag_list,		//同步所有势力战斗开关
	g2c_sync_repute_list,			//同步所有的势力声望
	g2c_character_open,				//
	g2c_roll_money_message,			//组队广播金钱的拾取
	g2c_finish_loot,				//结束拾取
	g2c_begin_roll_item,			//start roll
	g2c_custom_OTAction,			//同步定制的用户消息
	

	g2c_modify_threat,				//添加仇恨
	g2c_modify_threaten,			//修改被仇恨
	g2c_clear_threat,				//清除仇恨
	g2c_clear_threaten,				//清除被仇恨
    g2c_player_talk_to_one,         //密聊信息
    g2c_player_talk_to_many,        //群聊信息
	g2c_update_double_exp,			//同步双倍经验
	g2c_update_player_stamina,		//
    g2c_update_player_qi,           //同步'精''气''神'中的'气'
	g2c_doodad_loot_over,			//

    g2c_player_add_relation,
    g2c_player_del_relation,

	gs_client_connection_end
};
//GS 2 Client Protocol End

// Client 2 GS Protocol Begin
enum CLIENT_GS_PROTOCOL
{
    client_gs_connection_begin = 0,

    // 登陆＆跨服＆踢号相关协议
    c2g_apply_player_data,			// 登录时请求玩家数据
    c2g_ack_player_data,			// 确认同步玩家数据完成
    c2g_player_logout,				// 玩家登出

    c2g_character_stand,            // 角色站立的上传指令
    c2g_character_walk,				// 角色走动的上传指令
    c2g_character_run,				// 角色跑动的上传指令
    c2g_character_swim,				// 角色游泳的上传指令
    c2g_character_jump,				// 角色跳跃的上传指令
    c2g_character_sit,				// 角色坐下的上传指令

    c2g_character_action,			// 角色交互动作的上传指令
    c2g_character_say,				// 角色说话的上传指令
    c2g_character_turn,				// 角色朝向的上传指令

    // 同步相关协议
    c2g_apply_new_character_data,	// 请求新角色的数据
    c2g_apply_player_display_data,	// 玩家显示数据
    c2g_apply_npc_display_data,		// Npc显示数据

    c2g_stop_current_action,        // 中断玩家当前正在进行的行为

    // ------------ skill about protocol ------------->
    c2g_character_skill,			//角色攻击的上传指令
    c2g_mount_kungfu,
    c2g_cancel_buff,
    c2g_set_sheath_state,
    // <------------ skill about protocol -------------

    // ------------- party about protocol ------------->
    c2g_invite_join_party_request, 
    c2g_invite_join_party_respond, 
    c2g_apply_join_party_request,
    c2g_apply_join_party_respond,
    c2g_party_del_member_request,
    c2g_party_change_master_request,
    c2g_party_set_distribute_mode_request,
	c2g_party_set_roll_Quality_request,
    c2g_party_member_notify_signpost_request,   // 向队友发送小地图上的标记
    // <------------- party about protocol -------------

    //任务相关协议
    c2g_accept_quest,				//接受任务
    c2g_cancel_quest,				//取消任务
    c2g_finish_quest,				//完成任务
    c2g_share_quest,				//共享任务

    //GM指令上传协议
    c2g_gm_command,

    // ------------- shop about protocol ------------->
    c2g_open_shop_request,
    c2g_query_shop_items,
    c2g_shop_buy_request,
    c2g_shop_sell_request,
    c2g_shop_repair_request,
    c2g_sold_list_buy_request,
    // <------------- shop about protocol -------------

    // ------------- Trading about ------------------->
    c2g_trading_invite_request,
    c2g_trading_invite_respond,
    c2g_trading_add_item,
    c2g_trading_delete_item,
    c2g_trading_set_money,
    c2g_trading_confirm,
    // <-------------Trading about --------------------

    c2g_set_target,
    c2g_apply_exchange_item,		//申请交换装备
    c2g_apply_use_item,				//申请使用道具

    c2g_window_select,				//选择窗口选项

    c2g_cast_profession_skill,			//配方使用协议

    c2g_loot_or_roll_item,			//拾取物品或者掷骰子
    c2g_apply_loot_list,			//申请拾取列表
    c2g_character_open,				//打开
    c2g_death_respond,

	c2g_apply_fight_flag,			//改变战斗开关

	c2g_finish_loot,				//结束拾取
	c2g_loot_money,					//拾取金钱
	c2g_change_pk_mode,				//改变PK状态
	c2g_leader_distribute_item,		//队长分配物品

    c2g_player_talk_to_one,         //密聊信息
    c2g_player_talk_to_many,        //群聊信息

    c2g_player_add_relation,
    c2g_player_del_relation,

    client_gs_connection_end
};
// Client 2 GS Protocol End

//协议结构定义
#pragma	pack(1)

//GS 2 Client Struct Begin

// 应答消息
struct G2C_RESPONSE_MESSAGE : DOWNWARDS_PROTOCOL_HEADER
{
    BYTE	byErrorCodeType;
    BYTE	byErrorCode;
};

// 登录时同步玩家基本信息
struct G2C_SYNC_PLAYER_BASE_INFO : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwPlayerID;
    char	szRoleName[_NAME_LEN];
    BYTE    byRoleType;
    bool    bAlive;
    DWORD	dwMapID;
    DWORD	dwMapCopyIndex;
    int		nX;
    int		nY;
    int		nZ;
    BYTE    byFaceDirection;
    BYTE    byLevel;
    int     nExp;
	int		nDoubleExp;
    int		nGameLoop;
    WORD    wRepresentID[REPRESENT_COUNT];
};


// 同步任务状态
struct G2C_SYNC_QUEST_STATE : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD	dwPlayerID;
    char	szData[0];
};

//同步任务列表
struct G2C_SYNC_QUEST_LIST : UNDEFINED_SIZE_DOWNWARDS_HEADER 
{
    DWORD	dwPlayerID;
    char	szData[0];
};

//踢号通知
struct G2C_KICK_SELF_OUT : DOWNWARDS_PROTOCOL_HEADER
{

};

//跨服通知
struct G2C_CHANGE_GAME_SERVER : DOWNWARDS_PROTOCOL_HEADER
{
    char            szRoleName[_NAME_LEN];
    GUID            Guid;
    UINT			nIPAddr;
    unsigned short	wPort;	
};

// 同步Region中的所有玩家到客户端
struct G2C_SYNC_REGION_PLAYER : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    BYTE	byRegionX; // Region在地图中的X坐标
    BYTE	byRegionY; // Region在地图中的Y坐标
    struct	KPLAYER_INFO
    {
        DWORD	dwID;
        WORD	wRepresentState;
        BYTE    byFaceDirection;
        // uX, uY: 角色的坐标在Region中的偏移, 11 bits
        UINT64  uPartyLeaderFlag    : 1;                            // 是否队长
        UINT64	uX			        : (REGION_GRID_WIDTH_BIT_NUM  + CELL_LENGTH_BIT_NUM);	// 11 bits
        UINT64  uY			        : (REGION_GRID_HEIGHT_BIT_NUM + CELL_LENGTH_BIT_NUM);   // 11 bits
        UINT64	uZ				    : MAX_Z_POINT_BIT_NUM;	        // 22 bits
        UINT64  uVelocityXY         : MAX_ZOOM_VELOCITY_BIT_NUM;      // 11  bits
        UINT64  uDirectionXY        : DIRECTION_BIT_NUM;            // 8 bits
        // <-- 64 bits ---
        INT64   nVelocityZ          : MAX_VELOCITY_Z_BIT_NUM;       // 10 bits
        UINT64  uConvergenceSpeed   : MAX_VELOCITY_XY_BIT_NUM;      // 稳态速度 7 bits
        UINT64  uDestX              : MAX_X_COORDINATE_BIT;	        // 17 bits
        UINT64  uDestY              : MAX_Y_COORDINATE_BIT;	        // 17 bits
        UINT64  uCurrentGravity     : MAX_GRAVITY_BIT_NUM;          // 5 bits
        UINT64  uSwimAbility        : 1;                            // 游泳能力
        UINT64  uWaterFlyAbility    : 1;                            // 水上漂能力
        UINT64  uFightState         : 1;                            // 战斗状态
        UINT64  uSheathFlag         : 1;                            // 拔剑状态
        UINT64  uMoveState          : MAX_MOVE_STATE_BIT_NUM;       // 4 bits
        // <-- 64 bits ---
    } PlayerData[0];
};

// 同步Region中的所有NPC到客户端
struct G2C_SYNC_REGION_NPC : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    BYTE	byRegionX; // Region在地图中的X坐标
    BYTE	byRegionY; // Region在地图中的Y坐标
    struct	KNPC_INFO
    {
        DWORD	dwID;
        WORD	wRepresentState;
        BYTE    byFaceDirection;
        // uX, uY: 角色的坐标在Region中的偏移, 11 bits
        UINT64	uX			        : (REGION_GRID_WIDTH_BIT_NUM  + CELL_LENGTH_BIT_NUM);	// 11 bits
        UINT64  uY			        : (REGION_GRID_HEIGHT_BIT_NUM + CELL_LENGTH_BIT_NUM);   // 11 bits
        UINT64	uZ				    : MAX_Z_POINT_BIT_NUM;	        // 22 bits
        UINT64  uVelocityXY         : MAX_ZOOM_VELOCITY_BIT_NUM;    // 11 bits
        UINT64  uDirectionXY        : DIRECTION_BIT_NUM;            // 8 bits
        UINT64  uSheathFlag         : 1;                            // 拔剑状态
        // <-- 64 bits ---
        INT64   nVelocityZ          : MAX_VELOCITY_Z_BIT_NUM;       // 10 bits
        UINT64  uConvergenceSpeed   : MAX_VELOCITY_XY_BIT_NUM;      // 稳态速度 7 bits
        UINT64  uDestX              : MAX_X_COORDINATE_BIT;	        // 17 bits
        UINT64  uDestY              : MAX_Y_COORDINATE_BIT;	        // 17 bits
        UINT64  uCurrentGravity     : MAX_GRAVITY_BIT_NUM;          // 5 bits
        UINT64  uSwimAbility        : 1;                            // 游泳能力
        UINT64  uWaterFlyAbility    : 1;                            // 水上漂能力
        UINT64  uFightState         : 1;                            // 战斗状态
        UINT64  uMoveState          : MAX_MOVE_STATE_BIT_NUM;       // 4 bits
        // <-- 63 bits ---
    } NpcData[0];
};

// 同步新进入的玩家到客户端
struct G2C_SYNC_NEW_PLAYER : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwID;
    WORD	wRepresentState;
    BYTE    byFaceDirection;
    UINT64  uMoveState          : MAX_MOVE_STATE_BIT_NUM;       // 4 bits
    UINT64  uPosX               : MAX_X_COORDINATE_BIT;	        // 17 bits
    UINT64  uPosY               : MAX_Y_COORDINATE_BIT;         // 17 bits
    UINT64  uPosZ               : MAX_Z_POINT_BIT_NUM;          // 22 bits
    UINT64  uSwimAbility        : 1;                            // 游泳能力
    UINT64  uWaterFlyAbility    : 1;                            // 水上漂能力
    UINT64  uFightState         : 1;                            // 战斗状态
    UINT64  uSheathFlag         : 1;                            // 拔剑状态
    // <-- 64 bits ---
    UINT64  uConvergenceSpeed   : MAX_VELOCITY_XY_BIT_NUM;      // 稳态速度 7 bits
    UINT64  uDirectionXY        : DIRECTION_BIT_NUM;            // 8 bits
    INT64   nVelocityZ          : MAX_VELOCITY_Z_BIT_NUM;       // 10 bits
    UINT64  uDestX              : MAX_X_COORDINATE_BIT;	        // 17 bits
    UINT64  uDestY              : MAX_Y_COORDINATE_BIT;	        // 17 bits
    UINT64  uCurrentGravity     : MAX_GRAVITY_BIT_NUM;          // 5 bits
    // <-- 64 bits ---
    UINT16  uVelocityXY         : MAX_ZOOM_VELOCITY_BIT_NUM;    // 11  bits
    UINT16  uPartyLeaderFlag    : 1;                            // 是否队长
    // <-- 12 bits ---
};

// 同步新进入的NPC到客户端
struct G2C_SYNC_NEW_NPC : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwID;
    WORD	wRepresentState;
    BYTE    byFaceDirection;
    UINT64  uMoveState          : MAX_MOVE_STATE_BIT_NUM;       // 4 bits
    UINT64  uPosX               : MAX_X_COORDINATE_BIT;	        // 17 bits
    UINT64  uPosY               : MAX_Y_COORDINATE_BIT;         // 17 bits
    UINT64  uPosZ               : MAX_Z_POINT_BIT_NUM;          // 22 bits
    UINT64  uSwimAbility        : 1;                            // 游泳能力
    UINT64  uWaterFlyAbility    : 1;                            // 水上漂能力
    UINT64  uFightState         : 1;                            // 战斗状态
    UINT64  uSheathFlag         : 1;                            // 拔剑状态
    // <-- 64 bits ---
    UINT64  uConvergenceSpeed   : MAX_VELOCITY_XY_BIT_NUM;      // 稳态速度 7 bits
    UINT64  uDirectionXY        : DIRECTION_BIT_NUM;            // 8 bits
    INT64   nVelocityZ          : MAX_VELOCITY_Z_BIT_NUM;       // 10 bits
    UINT64  uDestX              : MAX_X_COORDINATE_BIT;	        // 17 bits
    UINT64  uDestY              : MAX_Y_COORDINATE_BIT;	        // 17 bits
    UINT64  uCurrentGravity     : MAX_GRAVITY_BIT_NUM;          // 5 bits
    // <-- 64 bits ---
    UINT16  uVelocityXY         : MAX_ZOOM_VELOCITY_BIT_NUM;    // 11 bits
};



// 从场景中移除角色
struct G2C_REMOVE_CHARACTER : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD		dwCharacterID;
};

// Doodad状态同步
struct G2C_SYNC_DOODAD_STATE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD		dwDoodadID;
    DWORD		dwTemplateID;
    int			nLifePercent;
    int			nDoodadInfo;
    int			nKind;
	int			nState;
	DWORD		dwNpcTemplateID;
	DWORD		dwRepresentID;
    unsigned    nX : MAX_X_COORDINATE_BIT;
    unsigned	nY : MAX_Y_COORDINATE_BIT;
    unsigned	nZ : MAX_Z_ALTITUDE_BIT_NUM;			//Z坐标
	int			nFaceDirection;
	BOOL		bLooted;
};


// 客户端玩家的瞬时移动状态同步
struct G2C_REVISE_MOVE_STATE :  DOWNWARDS_PROTOCOL_HEADER
{
    int     nFrame;
    char    cMoveState;
    int     nX;
    int     nY;
    int     nZ;
    BYTE    byFaceDirection;
    int     nDestX;
    int     nDestY;
    int     nVelocityXY;
    BYTE    byDirectionXY; 
    int     nVelocityZ;
    int     nConvergenceSpeed;
    BYTE    byCurrentGravity;
    char    cJumpCount;
};

// 角色的移动参数变化(比如受到加速效果)时,重新广播自己的移动参数
struct G2C_SYNC_MOVE_STATE : DOWNWARDS_PROTOCOL_HEADER 
{
    DWORD	    dwCharacterID;
    UINT64      uPosX                   : MAX_X_COORDINATE_BIT;	     // 17 bits
    UINT64      uPosY                   : MAX_Y_COORDINATE_BIT;      // 17 bits
    UINT64      uPosZ                   : MAX_Z_POINT_BIT_NUM;       // 22 bits
    UINT64      uFaceDirection          : DIRECTION_BIT_NUM;         // 8  bits
    // <-- 64 bit --
    UINT64      uMoveState              : MAX_MOVE_STATE_BIT_NUM;    // 4 bits
    UINT64      uVelocityXY             : MAX_ZOOM_VELOCITY_BIT_NUM; // 11 bits
    UINT64      uDirectionXY            : DIRECTION_BIT_NUM;         // 8 bits
    INT64       nVelocityZ              : MAX_VELOCITY_Z_BIT_NUM;    // 10 bits
    UINT64      uConvergenceSpeed       : MAX_VELOCITY_XY_BIT_NUM;   // 8 bits, 稳态收敛速度
    UINT64      uCurrentGravity         : MAX_GRAVITY_BIT_NUM;       // 5 bits
    UINT64      uSwimAbility            : 1;      
    UINT64      uWaterFlyAbility        : 1;  
    // <-- 48 bits --
};

struct G2C_CHARACTER_STAND : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX                  : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uSrcY                  : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ                  : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection         : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT32      uVelocityXY            : MAX_ZOOM_VELOCITY_BIT_NUM;  // 11 bits
    UINT32      uDirectionXY           : DIRECTION_BIT_NUM;          // 8 bits
    INT32       nVelocityZ             : MAX_VELOCITY_Z_BIT_NUM;     // 10 bits    
    // <-- 29 bits
};

struct G2C_CHARACTER_WALK : DOWNWARDS_PROTOCOL_HEADER				
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX                   : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uSrcY                   : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ                   : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection          : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY             : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY            : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ              : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX                  : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uDestY                  : MAX_Y_COORDINATE_BIT;	       // 17 bits
    // <-- 63 bits
    UINT16      uMoveState              : MAX_MOVE_STATE_BIT_NUM;      // 4 bits
    UINT16      uConvergenceSpeed       : MAX_VELOCITY_XY_BIT_NUM;     // 8 bits, 稳态收敛速度
};


struct G2C_CHARACTER_RUN : DOWNWARDS_PROTOCOL_HEADER				
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX                   : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uSrcY                   : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ                   : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection          : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY             : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY            : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ              : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX                  : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uDestY                  : MAX_Y_COORDINATE_BIT;	       // 17 bits
    // <-- 63 bits
    UINT16      uMoveState              : MAX_MOVE_STATE_BIT_NUM;      // 4 bits
    UINT16      uConvergenceSpeed       : MAX_VELOCITY_XY_BIT_NUM;     // 8 bits, 稳态收敛速度
};

struct G2C_CHARACTER_SWIM : DOWNWARDS_PROTOCOL_HEADER				
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX                   : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uSrcY                   : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ                   : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection          : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY             : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY            : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ              : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX                  : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uDestY                  : MAX_Y_COORDINATE_BIT;	       // 17 bits
    // <-- 63 bits
    UINT32      uMoveState              : MAX_MOVE_STATE_BIT_NUM;      // 4 bits
    UINT32      uConvergenceSpeed       : MAX_VELOCITY_XY_BIT_NUM;     // 8 bits, 稳态收敛速度
    UINT32      uPitchDirection         : DIRECTION_BIT_NUM;           // 8 bits 俯仰角
};

struct G2C_CHARACTER_JUMP : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX                   : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uSrcY                   : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ                   : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection          : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY             : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY            : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ              : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX                  : MAX_X_COORDINATE_BIT;	       // 17 bits
    UINT64      uDestY                  : MAX_Y_COORDINATE_BIT;	       // 17 bits
    // <-- 63 bits
    UINT16      uMoveState              : MAX_MOVE_STATE_BIT_NUM;      // 4 bits
    UINT16      uConvergenceSpeed       : MAX_VELOCITY_XY_BIT_NUM;     // 8 bits, 稳态收敛速度
};

struct G2C_CHARACTER_SIT : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	  // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;    // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;     // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;       // 8  bits
    // <-- 64 bit
};

struct G2C_CHARACTER_MEDITATE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	  // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;    // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;     // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;       // 8  bits
    // <-- 64 bit
};

struct G2C_CHARACTER_DASH : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY        : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY       : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ         : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX             : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uDestY             : MAX_Y_COORDINATE_BIT;	      // 17 bits
    // <-- 63 bits
};

struct G2C_CHARACTER_KNOCKED_DOWN : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    WORD        wFrame;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	  // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;    // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;     // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;       // 8  bits
    // <-- 64 bit
};

struct G2C_CHARACTER_KNOCKED_BACK : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY        : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY       : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ         : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX             : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uDestY             : MAX_Y_COORDINATE_BIT;	      // 17 bits
    // <-- 63 bits
};

struct G2C_CHARACTER_KNOCKED_OFF : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;        // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;         // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;           // 8  bits
    // <-- 64 bit
    UINT64      uVelocityXY        : MAX_ZOOM_VELOCITY_BIT_NUM;   // 11 bits
    UINT64      uDirectionXY       : DIRECTION_BIT_NUM;           // 8 bits
    INT64       nVelocityZ         : MAX_VELOCITY_Z_BIT_NUM;      // 10 bits
    UINT64      uDestX             : MAX_X_COORDINATE_BIT;	      // 17 bits
    UINT64      uDestY             : MAX_Y_COORDINATE_BIT;	      // 17 bits
    // <-- 63 bits
};

struct G2C_CHARACTER_STUN : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	  // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;    // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;     // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;       // 8  bits
    // <-- 64 bit
};

struct G2C_CHARACTER_HALT : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	    dwCharacterID;
    UINT64      uSrcX              : MAX_X_COORDINATE_BIT;	  // 17 bits
    UINT64      uSrcY              : MAX_Y_COORDINATE_BIT;    // 17 bits
    UINT64      uSrcZ              : MAX_Z_POINT_BIT_NUM;     // 22 bits
    UINT64      uFaceDirection     : DIRECTION_BIT_NUM;       // 8  bits
    // <-- 64 bit
};

// 角色死亡的同步数据包
struct G2C_CHARACTER_DEATH : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwCharacterID;
    DWORD	dwKillerID;
    DWORD   dwPunishTime;  // 死亡惩罚时间
    UINT64  uSrcX              : MAX_X_COORDINATE_BIT;	  // 17 bits
    UINT64  uSrcY              : MAX_Y_COORDINATE_BIT;    // 17 bits
    UINT64  uSrcZ              : MAX_Z_POINT_BIT_NUM;     // 22 bits
    UINT64  uFaceDirection     : DIRECTION_BIT_NUM;       // 8  bits
    // <-- 64 bit
};

// 移动角色位置
struct G2C_MOVE_CHARACTER : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwID;
    DWORD   dwMapID;
    DWORD   dwMapCopyIndex;
    BYTE    byFaceDirection;
    UINT64  uPosX               : MAX_X_COORDINATE_BIT;	        // 17 bits
    UINT64  uPosY               : MAX_Y_COORDINATE_BIT;         // 17 bits
    UINT64  uPosZ               : MAX_Z_POINT_BIT_NUM;          // 22 bits
    UINT64  uDirectionXY        : DIRECTION_BIT_NUM;            // 8 bits
    // <-- 64 bits ---
    UINT32  uVelocityXY         : MAX_ZOOM_VELOCITY_BIT_NUM;    // 11  bits
    INT32   nVelocityZ          : MAX_VELOCITY_Z_BIT_NUM;       // 10 bits
    UINT32  uConvergenceSpeed   : MAX_VELOCITY_XY_BIT_NUM;      // 稳态速度 7 bits
    UINT32  uMoveState          : MAX_MOVE_STATE_BIT_NUM;       // 4 bits
    // <-- 32 bits ---
};


// 角色交互动作的同步数据包
struct G2C_CHARACTER_ACTION : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwSrcCharacterID;
    DWORD	dwDestCharacterID;
    BYTE	byAction;
};


struct G2C_PLAYER_REVIVED_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;    // A复活B, A的ID
};

//角色说话的同步数据包
struct G2C_CHARACTER_SAY : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD	dwCharacterID;
    char	szText[0];		//对话内容字符串
};

// 同步角色转向
struct G2C_CHARACTER_TURN : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwCharacterID;
    BYTE	byDirection;
};

struct G2C_OVER_TIME_ACTION : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    char  cActionType;
    WORD  wActionID;
    BYTE  byDirection;
    WORD  wDurationFrame;
};

// 玩家显示数据
struct G2C_PLAYER_DISPLAY_DATA : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwPlayerID;
    char	szRoleName[_NAME_LEN];
    BYTE	byRoleType;
    BYTE    byLevel;
    WORD	wRepresentId[REPRESENT_COUNT];
};

// Npc显示数据
struct G2C_NPC_DISPLAY_DATA : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwNpcID;
    DWORD	dwTemplateID;
	int		nLevel;
	DWORD	dwRepresentID;
};

// 获取经验值
struct G2C_ADD_EXPERIENCE : DOWNWARDS_PROTOCOL_HEADER
{
    int		nExpIncrement;
	BOOL	bUseDoubleExp;
};

struct G2C_COMMON_HEALTH :  DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    int   nDeltaLife;
};

//帧间隔信号
struct G2C_FRAME_SIGNAL : DOWNWARDS_PROTOCOL_HEADER
{
    int	nGameLoop;
};


struct G2C_TERRAIN_TEST : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    int  nX;
    int  nY;
    BYTE byCellCount;
    struct KCELL_INFO 
    {
        bool bBlockCharacter;
        BYTE byCellType;
        BYTE bySlipDirection;
        BYTE byGradient;
        BYTE byBarrierDirection;
	    WORD wLowLayer;	        // 下层高度(以海平面为基准)
	    WORD wHighLayer;        // 上层高度(以海平面为基准)
    } CellInfo[0];
};


// ------------------ skill about protocol ----------------->

//同步技能数据,bySkillLevel为0意味着这个技能没有了(Forget)
struct G2C_SYNC_SKILL_DATA : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwSkillID;
    BYTE	bySkillLevel;
    DWORD   dwProficiency;
};

struct G2C_SKILL_PREPARE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwCharacterID;
    DWORD	dwSkillID;
    BYTE	bySkillLevel;
    int     nPrepareFrame;
    BYTE    byTargetType;		    //TARGET_TYPE类型
    int	    nParam1;			    //如果TargetType不是坐标，则这里表示角色ID
    int	    nParam2;
    int	    nParam3;
};

struct G2C_SKILL_CAST : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    DWORD dwSkillID;
    BYTE  bySkillLevel;
    DWORD dwBulletID;           //该技能所产生的子弹ID    
    bool  bSubCast;
    BYTE  byTargetType;		    //TARGET_TYPE类型
    int	  nParam1;			    //如果TargetType不是坐标，则这里表示角色ID
    int	  nParam2;
    int	  nParam3;
};

struct G2C_SYNC_COMMON_SKILL_AUTO_CAST_STATE : DOWNWARDS_PROTOCOL_HEADER
{
    bool bStart;
    bool bMelee;
};

struct G2C_RESET_SKILL_COOLDOWN :  DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwSkillID;
    BYTE  bySkillLevel;
};

// 技能打断,终止,包括吟唱的终止,通道技的终止
struct G2C_SKILL_BREAK :  DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
};

// 吟唱打退
struct  G2C_PREPARE_BACK : DOWNWARDS_PROTOCOL_HEADER
{
};

struct G2C_SKILL_EFFECT_RESULT : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD dwSkillSrc;   // 技能发出者的ID
    DWORD dwSkillDst;
    struct
    {
        BYTE    byType;
        DWORD   dwID;
        BYTE    byLevel;  
    } EffectSrc;
  
    BYTE  bShield : 1;
    BYTE  bPhysicsBlock : 1;
    BYTE  bSolarMagicBlock : 1;
    BYTE  bNeutralMagicBlock : 1;
    BYTE  bLunarMagicBlock : 1;
    BYTE  bPoisonBlock : 1;
    BYTE  bReact : 1;
    BYTE  bHitTarget : 1;
    BYTE  bSkillDodge : 1;
    BYTE  bCriticalStrike : 1;
    BYTE  bDebuffDodge : 1;
    BYTE  bDebuffImmunity : 1;
    BYTE  bDotDodge : 1;
    DWORD dwDebuffID;
    BYTE  byDebuffLevel;
    DWORD dwDotID;
    BYTE  byDotLevel;

    char  cResultCount;
    struct KSKILL_RESULT 
    {
        char cEffectResultType;
        int  nValue;
    } SkillResult[0];
};

struct G2C_SYNC_BEHIT_REPRESENT: UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD dwSkillSrc;
    DWORD dwSkillDst;
    DWORD dwBulletID;
    
    struct
    {
        BYTE    byType;
        DWORD   dwID;
        BYTE    byLevel;  
    } EffectSrc;

    int bReact: 1;
    int bHitTarget : 1;
    int bSkillDodge : 1;
    int bCriticalStrike : 1;    

    int bPhysicsBlock : 1;
    int bSolarMagicBlock : 1;
    int bNeutralMagicBlock : 1;
    int bLunarMagicBlock : 1;
    int bPoisonBlock : 1;

    int bPhysicsDamage : 1;
    int bSolarMagicDamage : 1;
    int bNeutralMagicDamage : 1;
    int bLunarMagicDamage : 1;
    int bPoisonDamage : 1;
    int bReflectiedDamage : 1;  

    int bTherapy : 1;          
    int bStealLife : 1;         
    int bAbsorbDamage : 1;     
    int bShieldDamage : 1;
};

struct G2C_SYNC_FIGHT_STATE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    bool  bFightState;
};

struct G2C_SYNC_BUFF_LIST : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD dwCharacterID;
    WORD  wDataSize;
    BYTE  byData[0];
};

struct G2C_SYNC_BUFF_SINGLE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD   dwCharacterID;
    int     nIndex;
    char    cStyle;
    DWORD	dwBuffID;
    char    cLevel;
    char    cStackNum;
    int     nEndFrame;
};

struct G2C_SYNC_REPRESENT_STATE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    WORD  wRepresentState;
};

struct G2C_SYNC_KUNGFU_MOUNT :  DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwID;
    DWORD dwLevel;
};

struct G2C_SKILL_BEAT_BACK :  DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    int   nBeatBackFrame;
	BOOL  bBreak;
};

struct G2C_SYNC_SHEATH_STATE :  DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    bool  bSheathFlag;
};

// ------------------  人物数值同步 -----------------------------

struct G2C_SYNC_SELF_MAX_LMRS : DOWNWARDS_PROTOCOL_HEADER
{
    int nMaxLife;
    int nMaxMana;
    int nMaxRage;
};

struct G2C_SYNC_SELF_CURRENT_LMRS : DOWNWARDS_PROTOCOL_HEADER
{
    int nCurrentLife;
    int nCurrentMana;
    int nCurrentRage;
};

struct G2C_SYNC_TARGET_MAX_LMR : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD   dwCharacterID;
    int     nMaxLife;
    int     nMaxMana;
    int     nMaxRage;
};

struct G2C_SYNC_TARGET_CURRENT_LMR : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD   dwCharacterID;
    BYTE    byLifePercent;
    BYTE    byManaPercent;
    BYTE    byRagePercent;
};

struct G2C_SYNC_PARTY_MEMBER_MAX_LMR : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD   dwMemberID;
    int     nMaxLife;
    int     nMaxMana;
    int     nMaxRage;
};

struct G2C_SYNC_PARTY_MEMBER_CURRENT_LMR : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD   dwMemberID;
    BYTE    byLifePercent;
    BYTE    byManaPercent;
    BYTE    byRagePercent;
};

struct G2C_SYNC_PARTY_MEMBER_MISC : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwMemberID;
    DWORD dwPortraitID;
    DWORD dwMapID;
    DWORD dwForceID;
    BYTE  byLevel;    
};

struct G2C_SYNC_PARTY_MEMBER_COORDINATE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwMemberID;
    int   nX;
    int   nY;
};

struct G2C_BROADCAST_CHARACTER_LIFE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD		dwCharacterID;
    BYTE	    byLifePercent;
};

// ------------------ party about protocol ------------------>

// 玩家登陆服务器时同步队伍基本信息
struct G2C_SYNC_PLAYER_PARTY_BASE_INFO : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwLeaderID;
    int   nLootMode;
	int   nRollQuality;
};

// 玩家登陆服务器时同步队伍成员信息,每次只同步一个成员
struct G2C_SYNC_PLAYER_PARTY_MEMBER_INFO : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID;
    char  szMemberName[_NAME_LEN];
    DWORD dwPortraitID; // 头像ID
    DWORD dwMapID;
    DWORD dwForceID;
    BYTE  byLevel;
    bool  bOnlineFlag;
    int   nMaxLife; 
    int   nMaxMana; 
    int   nMaxRage;
    BYTE  byLifePercent; 
    BYTE  byManaPercent; 
    BYTE  byRagePercent;
    int   nPosX;
    int   nPosY;
};

// xxx邀请你组队
struct G2C_INVITE_JOIN_PARTY_REQUEST : DOWNWARDS_PROTOCOL_HEADER
{
    char  szInviteSrc[_NAME_LEN]; // 发出组队请求的玩家
    GUID  guid;
};

// 对组队邀请的应答: 目前只有对方拒绝组队时才发这个协议
struct G2C_INVITE_JOIN_PARTY_RESPOND : DOWNWARDS_PROTOCOL_HEADER
{
    char  szInvitedRole[_NAME_LEN];
    int   nRespondCode;
};

// xxx申请加入xxx的队伍
struct G2C_APPLY_JOIN_PARTY_REQUEST : DOWNWARDS_PROTOCOL_HEADER
{
    char  szApplySrc[_NAME_LEN]; // 发出入队申请的玩家
    GUID  guid;
};

// 入队申请的相应(只有失败的时候才会有这个响应)
struct G2C_APPLY_JOIN_PARTY_RESPOND : DOWNWARDS_PROTOCOL_HEADER
{
    char  szApplyDst[_NAME_LEN];
    int   nRespondCode;
};

// 通知玩家"你建立了一支队伍"
struct G2C_PARTY_CREATE_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
};

// 通知玩家"你加入xxx的队伍"
struct G2C_PLAYER_JOIN_PARTY_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    char  szLeaderName[_NAME_LEN];
};

// 如果A玩家刚刚加入某队伍,通知队伍里面的所有成员"A的加入了队伍"
struct G2C_PARTY_ADD_MEMBER_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID;
    char  szMemberName[_NAME_LEN];
    DWORD dwPortraitID;
    bool  bOnlineFlag;
};

// 通知客户端xxx离开xxx的队伍
struct G2C_PARTY_DEL_MEMBER_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID;
};

struct G2C_SYNC_PARTY_LEADER_FLAG : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    bool  bLeaderFlag; //是变为队长，还是从队长变成非队长
};

struct G2C_PARTY_CHANGE_LEADER_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwNewLeaderID;
};

struct G2C_PARTY_SET_LOOT_MODE_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwPartyID;
	int   nLootMode;
	int   nRollQuality;
};

struct G2C_PARTY_DISBAND_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
};

struct G2C_SYNC_PARTY_MEMBER_ONLINE_FLAG : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID;
    bool  bOnlineFlag;
};

struct  G2C_PARTY_MEMBER_SIGNPOST_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    int   nX;
    int   nY;
};

// <------------------ party about protocol ------------------


// ------------------ shop about protocol ------------------>

struct G2C_OPEN_SHOP_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD         dwShopID;
    char          cShopType;
    BYTE          byValidPage;
    bool          bCanRepair;
    DWORD         dwNpcID;
};


// 同步商店格子信息
struct G2C_UPDATE_SHOP_ITEM : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD   dwShopID;
    BYTE    byPageIndex;
    BYTE    byPosIndex;
    DWORD   dwItemID;
    int     nPrice;
    int     nCount;
    int     nLimit;
    BYTE    byReputeLevel;
    char    cSyncType;
    WORD    wItemDataLength;
    BYTE    byItemData[0];

};

struct G2C_SHOP_BUY_RESPOND : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD   dwShopID;
    BYTE    byPageIndex;
    BYTE    byPosIndex;
    char    cRespondCode;
};

// 注意，卖出物品失败的时候才会发送这个协议
// 目前只考虑NPC商店的情况下尚无失败的情况,但是玩家商店会出现这种情况
struct G2C_SHOP_SELL_RESPOND : DOWNWARDS_PROTOCOL_HEADER
{
};


// <------------------ shop about protocol ------------------

struct G2C_TRADING_INVITE_REQUEST : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwTradingInviteSrc;
};

struct G2C_OPEN_TRADING_BOX_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwOtherPlayer;
};

struct G2C_SYNC_TRADING_CONFIRM : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    bool  bConfirm;
};

struct G2C_SYNC_TRADING_ITEM_DATA : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    BYTE    byItemType;
    DWORD   dwItemOwnerID;
    BYTE    byBoxIndex;
    BYTE    byPosIndex;
    BYTE    byGridIndex;
    BYTE    byStackNum;     // 物品叠加数量或耐久度
    int     nKey;
    char    szBuff[0];
};

struct G2C_SYNC_TRADING_MONEY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    int   nMoney;
    int   nKey;
};

struct G2C_TRADING_COMPLETE : DOWNWARDS_PROTOCOL_HEADER
{
    bool bComplete;
};

// 同步金钱数量
struct G2C_SYNC_MONEY : DOWNWARDS_PROTOCOL_HEADER
{
    int						nMoney;
};

// 同步临时道具数据
struct G2C_SYNC_TEMP_ITEM_DATA : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
	BYTE					nItemType;
	char					szBuff[0];
};
 

// 同步道具数据
struct G2C_SYNC_ITEM_DATA : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    BYTE					nItemType;
    BYTE					byBox;
    BYTE					byX;
    char					szBuff[0];
};

// 同步道具删除
struct G2C_SYNC_ITEM_REMOVE : DOWNWARDS_PROTOCOL_HEADER
{
    BYTE					byBoX;
    BYTE					byX;
};


// 交换装备
struct G2C_EXCHANGE_ITEM : DOWNWARDS_PROTOCOL_HEADER
{
    BYTE					byBox1;
    BYTE					byX1;
    BYTE					byBox2;
    BYTE					byX2;
};

// 换装广播
struct G2C_SYNC_EQUIP_REPRESENT : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    char  cInventoryType;
    WORD  wRepresentId;
};


// 更新物品数量或耐久
struct G2C_UPDATE_ITEM_AMOUNT : DOWNWARDS_PROTOCOL_HEADER
{
    BYTE					byBox;
    BYTE					byX;
    WORD					wAmount;
};


// 学习生活技能
struct G2C_LEARN_PROFESSION_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwProfessionID;
};


// 学习配方
struct G2C_LEARN_RECIPE_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCraftID;
    DWORD dwRecipeID;
};


// 遗忘技能
struct G2C_FORGET_PROFESSION_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwProfessionID;
};


// 遗忘配方
struct G2C_FORGET_RECIPE_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCraftID;
    DWORD dwRecipeID;
};


// 增加生活技能经验
struct G2C_ADD_PROFESSION_PROFICIENCY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwProfessionID;
    DWORD dwExp;
};


// 设置生活技能等级
struct G2C_SET_MAX_PROFESSION_LEVEL_NOTIFY : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwProfessionID;
    DWORD dwLevel;
};


// 同步更新玩家生活技能
struct G2C_UPDATE_PLAYER_PROFESSION : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwProfessionID;
    DWORD dwMaxLevel;
    DWORD dwCurrentLevel; 
    DWORD dwCurrentProficiency;
};


// 同步更新玩家配方列表
struct G2C_UPDATE_PLAYER_RECIPE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwRecipeID;
    BOOL  bIsUsed;
    DWORD dwEndCoolDownFrame;
};

struct G2C_CAST_RECIPE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
};

// 打开一个客户端窗口
struct G2C_OPEN_WINDOW : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD	dwIndex;			//流水号
	DWORD	dwTargetType;		//目标类型 Doodad? Npc?
	DWORD	dwTargetID;			//目标ID
    char	szText[0];
};


// 同步配方的使用
struct G2C_START_USE_RECIPE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwPlayerID;
    DWORD	dwRecipeID;
    BYTE	byTargetType;
    int		nParam1;
    int		nParam2;
};

// 结束技能释放
struct G2C_END_CAST : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwCastEndType;
};


// 通知客户端接受任务
struct G2C_ACCEPT_QUEST : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwQuestID;
};


// 通知客户端完成任务
struct G2C_FINISH_QUEST : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwQuestID;
};


// 通知客户端取消任务
struct G2C_CANCEL_QUEST : DOWNWARDS_PROTOCOL_HEADER
{
    int		nQuestIndex;
};

// 共享任务
struct G2C_SHARE_QUEST : DOWNWARDS_PROTOCOL_HEADER
{
    BYTE	byTargetType;
    DWORD	dwTargetID;
    DWORD	dwQuestID;
};

// 同步任务变量
struct G2C_SYNC_QUEST_VALUE : DOWNWARDS_PROTOCOL_HEADER
{
    BYTE	byQuestIndex;
    DWORD	dwKillNpcCount[QUEST_PARAM_COUNT];
    int		nQuestValue[QUEST_PARAM_COUNT * 2];
};


// 同步拾取列表
struct G2C_SYNC_LOOT_LIST : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    DWORD	dwDoodadID;
	int		nMoney; 
	int		nLootMode;
	int		nRollQuality;
	int		dwLooterIDList[MAX_PARTY_SIZE];
    char	szData[0];
};


// 掷骰子信息
struct G2C_ROLL_ITEM_MESSAGE : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD	dwPlayerID;
    DWORD	dwDoodadID;
    DWORD	dwItemID;
    BOOL	bLooted;
    int		nRandom;
};


// 角色拾取物品
struct G2C_CHARACTER_PICK : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwCharactorID;
};


// 打开拾取面板
struct G2C_OPEN_LOOT_LIST : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    DWORD dwDoodadID;
};


// 同步战斗开关
struct G2C_SYNC_FIGHT_FLAG : DOWNWARDS_PROTOCOL_HEADER
{
	BYTE byForceID;
	BYTE byOpen;
};

// 同步声望
struct G2C_SYNC_REPUTE : DOWNWARDS_PROTOCOL_HEADER
{
	BYTE byForceID;
	short shRepute;
};


// 同步所有势力战斗开关
struct G2C_SYNC_FIGHTFLAG_LIST : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
	BYTE	byFightFlagCount;
	BYTE	byFightFlag[0];
};


// 同步所有的势力声望
struct G2C_SYNC_REPUTE_LIST : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
	BYTE	byForceCount;
	short	shRepute[0];	
};

// 
struct G2C_CHARACTER_OPEN : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCharacterID;
	DWORD dwDoodadID;
};


// 组队广播金钱的拾取
struct G2C_ROLL_MONEY_MESSAGE : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwPlayerID;
	DWORD dwDoodadID;
};


// 结束拾取
struct G2C_FINISH_LOOT : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwPlayerID;
	DWORD dwDoodadID;
};


// start roll
struct G2C_BEGIN_ROLL_ITEM : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwItemID;
	DWORD dwDoodadID;
};


// 同步定制的用户消息
struct G2C_CUSTOM_OTACTION : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwPlayerID;
	int nType;
	BOOL bIsBreakable;
	int nFrame;
	int nTargetType;
	int nTargetID;
	DWORD dwScriptID;
	char szActionName[CUSTOM_OTACTION_NAME_LEN];
};


// 同步任务共享的结果
struct G2C_SHARE_QUEST_RESULT : DOWNWARDS_PROTOCOL_HEADER
{
	int nResult;
	DWORD dwDstPlayerID;
	BOOL bAccept;
};


// 添加仇恨
struct G2C_MODIFY_THREAT : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCharacterID;
};


// 修改被仇恨
struct G2C_MODIFY_THREATEN : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCharacterID;
};


// 清除仇恨
struct G2C_CLEAR_THREAT : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCharacterID;
};


// 清除被仇恨
struct G2C_CLEAR_THREATEN : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwCharacterID;
};


// 密聊信息
struct G2C_PLAYER_TALK_TO_ONE : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    BYTE byReplyFlag;           // 表明是否是对说话者的回应
    char szName[_NAME_LEN];     // bReplyFlag = 1, 接受者的名字; bReplyFlag = 0, 说话人的名字;
    BYTE byTalkData[0];         // 对话内容数据
};

// 群聊信息
struct G2C_PLAYER_TALK_TO_MANY : UNDEFINED_SIZE_DOWNWARDS_HEADER
{
    BYTE byTalkRange;
    char szTalker[_NAME_LEN];   // 说话人的名字
    BYTE byTalkData[0];         // 对话内容数据
};


// 同步双倍经验
struct G2C_UPDATE_DOUBLE_EXP : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwDoubleExp;
};


// 
struct G2C_UPDATE_PLAYER_STAMINA : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwStamina;
};

struct G2C_UPDATE_PLAYER_QI : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwQi;
};

// 
struct G2C_DOODAD_LOOT_OVER : DOWNWARDS_PROTOCOL_HEADER
{
	DWORD dwDoodadID;
};

struct G2C_PLAYER_ADD_RELATION : DOWNWARDS_PROTOCOL_HEADER
{
    char  szAlliedPlayerName[_NAME_LEN];
    BYTE  byResult;
    DWORD dwAlliedPlayerID;
};

struct G2C_PLAYER_DEL_RELATION : DOWNWARDS_PROTOCOL_HEADER
{
    DWORD dwAlliedPlayerID;
    BYTE  byResult;
};

//AutoCode:GS 2 Client Struct End

/************************************************************************/
//Client 2 GS Struct Begin

//登录时请求玩家数据
struct C2G_APPLY_PLAYER_DATA : UPWARDS_PROTOCOL_HEADER
{
    char	szName[_NAME_LEN];
    GUID	Guid;
};

//登录时确认同步玩家数据完成
struct C2G_ACK_PLAYER_DATA : UPWARDS_PROTOCOL_HEADER
{
};

//玩家登出
struct C2G_PLAYER_LOGOUT : UPWARDS_PROTOCOL_HEADER
{

};

struct C2G_CHARACTER_STAND : UPWARDS_PROTOCOL_HEADER
{
    int     nSrcX;
    int     nSrcY;
    int     nSrcZ;
    BYTE	byFaceDirection;
};


struct C2G_CHARACTER_WALK : UPWARDS_PROTOCOL_HEADER
{
    int     nSrcX;
    int     nSrcY;
    int     nSrcZ;
    int		nDstX;
    int		nDstY;
    BYTE	byFaceDirection;
};

struct C2G_CHARACTER_RUN : UPWARDS_PROTOCOL_HEADER
{
    int     nSrcX;
    int     nSrcY;
    int     nSrcZ;
    int		nDstX;
    int		nDstY;
    BYTE	byFaceDirection;
};

struct C2G_CHARACTER_SWIM : UPWARDS_PROTOCOL_HEADER
{
    int     nSrcX;
    int     nSrcY;
    int     nSrcZ;
    int		nDstX;
    int		nDstY;
    int     nDstZ;
    BYTE	byFaceDirection;
    BYTE    byPitchDirection;
};


struct C2G_CHARACTER_JUMP : UPWARDS_PROTOCOL_HEADER
{
    int     nSrcX;
    int     nSrcY;
    int     nSrcZ;
    bool    bStandJump;
    BYTE    byJumpDirection;
    BYTE	byFaceDirection;
};

struct C2G_CHARACTER_SIT :  UPWARDS_PROTOCOL_HEADER
{
    int     nSrcX;
    int     nSrcY;
    int     nSrcZ;
    BYTE	byFaceDirection;
};

// 玩家的交互动作上传指令
struct C2G_CHARACTER_ACTION : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwCharacterID;
    BYTE	byAction;
};

//玩家说话上传指令
struct C2G_CHARACTER_SAY : UNDEFINED_SIZE_UPWARDS_HEADER
{
    char	szText[0];			//说话内容
};

// 角色朝向上传指令
struct C2G_CHARACTER_TURN : UPWARDS_PROTOCOL_HEADER
{
    BYTE	byDirection;
};

// 请求新角色的数据
struct C2G_APPLY_NEW_CHARACTER_DATA : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwCharacterID;
};

// 玩家显示数据
struct C2G_APPLY_PLAYER_DISPLAY_DATA : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwPlayerID;
    DWORD	dwCheckSum;
};

// Npc显示数据
struct C2G_APPLY_NPC_DISPLAY_DATA : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwNpcID;
    DWORD	dwCheckSum;
};

struct C2G_STOP_CURRENT_ACTION : UPWARDS_PROTOCOL_HEADER
{

};

// ------------------ skill about protocol ----------------->

//角色攻击上传指令
struct C2G_CHARACTER_SKILL : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwSkillID;			//技能ID
    BYTE	bySkillLevel;		//技能等级
    BYTE	byTargetType;		
    int		nParam1;	        //当TARGET_TYPE类型是坐标时,坐标值为(nParam1, nParam2, nParam3)
    int		nParam2;            //当类型为xxID时,nParam1为ID值,nParam2没意义
    int     nParam3;
};

struct C2G_MOUNT_KUNGFU :  UPWARDS_PROTOCOL_HEADER
{
    DWORD dwID;
    DWORD dwLevel;
};

struct C2G_CANCEL_BUFF : UPWARDS_PROTOCOL_HEADER
{
    char cStyle;
    int  nIndex;
};

struct C2G_SET_SHEATH_STATE :  UPWARDS_PROTOCOL_HEADER
{
    bool bSheathFlag;
};

// <------------------ skill about protocol -----------------


// ----------------- party about protocol ----------------->

// xxx邀请xxx组队
struct C2G_INVITE_JOIN_PARTY_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    char szInvitedPlayer[_NAME_LEN]; // 邀请该玩家组队
};

// xxx申请加入xxx的队伍
struct C2G_APPLY_JOIN_PARTY_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    char  szApplyDst[_NAME_LEN]; // 申请加入这个玩家的队伍
};

// 客户端收到组队邀请后的应答
struct C2G_INVITE_JOIN_PARTY_RESPOND : UPWARDS_PROTOCOL_HEADER
{
    char  szInviteSrc[_NAME_LEN]; // 发出组队请求的玩家    
    GUID  guid;
    BOOL  bAgreeFlag;
};

// 客户端(队长)收到入队申请后的应答
struct C2G_APPLY_JOIN_PARTY_RESPOND : UPWARDS_PROTOCOL_HEADER
{
    char  szApplySrc[_NAME_LEN]; // 发出入队申请的玩家
    GUID  guid;
    BOOL  bAgreeFlag;
};

struct C2G_PARTY_DEL_MEMBER_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwMemberID;
};

struct C2G_PARTY_CHANGE_LEADER_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwNewLeaderID;
};

struct C2G_PARTY_SET_LOOT_MODE_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    int nLootMode;
};

struct C2G_PARTY_SET_ROLL_QUALITY_REQUEST : UPWARDS_PROTOCOL_HEADER
{
	int nRollQuality;
};

struct  C2G_PARTY_MEMBER_NOTIFY_SIGNPOST_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    int   nX;
    int   nY;
};

// <----------------- party about protocol -----------------

// 接受任务
struct C2G_ACCEPT_QUEST : UPWARDS_PROTOCOL_HEADER
{
    BYTE	byTargetType;
    DWORD	dwTargetID;
    DWORD	dwQuestID;
};

//完成任务
struct C2G_FINISH_QUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwQuestID;
	DWORD	dwTargetType;
	DWORD	dwTargetID;
    int		nPresentChoice1;
    int		nPresentChoice2;
};

//取消任务
struct C2G_CANCEL_QUEST : UPWARDS_PROTOCOL_HEADER
{
    int		nQuestIndex;
};

// 共享任务
struct C2G_SHARE_QUEST : UPWARDS_PROTOCOL_HEADER
{
    int		nQuestIndex;
};

//GM指令
struct C2G_GM_COMMAND : UNDEFINED_SIZE_UPWARDS_HEADER
{
    char	szGmCommand[0];
};

// ----------------- shop about protocol ----------------->


// 请求同步商店物品列表，注意，当服务端返回商店基本信息的时候，客户端就可以打开商店界面了，后面同步物品是
// 一件一件的同步的，并不存在一个标志说“物品同步完毕”

struct C2G_OPEN_SHOP_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwShopID;
};

struct C2G_QUERY_SHOP_ITEMS : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwShopID;    
    BYTE  byPageIndex;
    struct SHOP_SYNC_ITEM
    {
        BYTE  byPosIndex;
        DWORD dwItemID;
        int   nCount;
        int   nPrice;
    } ShopPageItems[MAX_SHOP_PAGE_ITEM_COUNT];
};

struct C2G_SHOP_BUY_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwShopID;
    DWORD dwItemID;
    int   nCount;
    int   nCost;
    BYTE  byPageIndex;
    BYTE  byPosIndex; 
};

struct C2G_SHOP_SELL_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwShopID;
    DWORD dwItemID;
    int   nCost;
    DWORD dwBoxIndex;
    DWORD dwPosIndex;
};

struct C2G_SHOP_REPAIR_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwShopID;
    DWORD dwItemID;     // 当该值为ERROR_ID(0)时,代表修理身上和包包里所有的装备
    int   nCost;
    DWORD dwBoxIndex;
    DWORD dwPosIndex;
};

struct C2G_SOLD_LIST_BUY_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwShopID;
    DWORD dwItemID;
    DWORD dwX;
};

// <----------------- shop about protocol -----------------

// ------------- Trading about ------------------->
struct C2G_TRADING_INVITE_REQUEST : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwInviteDst;
};

struct C2G_TRADING_INVITE_RESPOND : UPWARDS_PROTOCOL_HEADER
{
    bool bAccept;
};

struct C2G_TRADING_ADD_ITEM : UPWARDS_PROTOCOL_HEADER
{
    BYTE byBoxIndex;
    BYTE byPosIndex;
    BYTE byGridIndex;
};

struct C2G_TRADING_DELETE_ITEM : UPWARDS_PROTOCOL_HEADER
{
    BYTE byGridIndex;
};

struct C2G_TRADING_SET_MONEY : UPWARDS_PROTOCOL_HEADER
{
    int nMoney;
};

struct C2G_TRADING_CONFIRM : UPWARDS_PROTOCOL_HEADER
{
    bool bConfirm;
    int  nKey;
};
// <-------------Trading about --------------------

struct C2G_SET_TARGET : UPWARDS_PROTOCOL_HEADER
{
    BYTE    byTargetType;
    DWORD   dwTargetID;   // Target只能是Player, NPC, Doodad.
};


// 申请交换装备
struct C2G_APPLY_EXCHANGE_ITEM : UPWARDS_PROTOCOL_HEADER
{
    BYTE	byBox1;
    BYTE	byX1;
    BYTE	byBox2;
    BYTE	byX2;
    WORD	wAmount;
};


// 选择窗口选项
struct C2G_WINDOW_SELECT : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwIndex;
    BYTE	bySelect;
};


// 配方使用协议
struct C2G_CAST_PROFESSION_SKILL : UPWARDS_PROTOCOL_HEADER
{
	DWORD	dwCraftID;
    DWORD	dwRecipeID;
    BYTE	byTargetType;
    int		nParam1;
    int		nParam2;
};


// 拾取物品或者掷骰子
struct C2G_LOOT_OR_ROLL_ITEM : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwDoodadID;
    DWORD	dwItemID;
    BOOL	bCancel;
};


// 申请拾取列表
struct C2G_APPLY_LOOT_LIST : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwDoodadID;
};


// 打开
struct C2G_CHARACTER_OPEN : UPWARDS_PROTOCOL_HEADER
{
    DWORD	dwDoodadID;
};


struct C2G_DEATH_RESPOND : UPWARDS_PROTOCOL_HEADER
{
    int nRespondCode;
};

// 申请使用道具
struct C2G_APPLY_USE_ITEM : UPWARDS_PROTOCOL_HEADER
{
    BYTE	byBox;
    BYTE	byX;
    BYTE	byTargetType;
    int	    nParam1;			    //如果TargetType不是坐标，则这里表示ID
    int	    nParam2;
    int		nParam3;
};


// 改变战斗开关
struct C2G_APPLY_FIGHT_FLAG : UPWARDS_PROTOCOL_HEADER
{
	BYTE	byForceID;
	BYTE	byOpen;
};


// 结束拾取
struct C2G_FINISH_LOOT : UPWARDS_PROTOCOL_HEADER
{
	DWORD dwDoodadID;
};


// 拾取金钱
struct C2G_LOOT_MONEY : UPWARDS_PROTOCOL_HEADER
{
	DWORD dwDoodadID;
};

// 改变PK状态
struct C2G_CHANGE_PK_MODE : UPWARDS_PROTOCOL_HEADER
{
	BYTE byPKMode;
};

// 队长分配物品
struct C2G_LEADER_DISTRIBUTE_ITEM : UPWARDS_PROTOCOL_HEADER
{
	DWORD dwDoodadID;
	DWORD dwItemID;
	DWORD dwDstPlayerID;
};

// 密聊信息
struct C2G_PLAYER_TALK_TO_ONE : UNDEFINED_SIZE_UPWARDS_HEADER
{
    char szTalkTo[_NAME_LEN];   //对方的名字
    BYTE byTalkData[0];         //对话内容数据
};

// 群聊信息
struct C2G_PLAYER_TALK_TO_MANY : UNDEFINED_SIZE_UPWARDS_HEADER
{
    BYTE byTalkRange;           //群聊范围
    BYTE byTalkData[0];         //对话内容数据
};

struct C2G_PLAYER_ADD_RELATION : UPWARDS_PROTOCOL_HEADER
{
    char szAlliedPlayerName[_NAME_LEN];
};

struct C2G_PLAYER_DEL_RELATION : UPWARDS_PROTOCOL_HEADER
{
    DWORD dwAlliedPlayerID;
};

//AutoCode:Client 2 GS Struct End
#pragma pack()

#endif	//_GS_CLIENT_PROTOCOL_H_
